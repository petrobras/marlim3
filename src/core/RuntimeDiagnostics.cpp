#include "RuntimeDiagnostics.h"

#include "Log.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <mutex>
#include <tuple>
#include <utility>

namespace diagnostics {

namespace {

/// Where a recurring condition was met: how many times, the first state and the
/// range of pressure and temperature.
struct StateRange {
    long count = 0;
    double firstPressure = 0.;
    double firstTemperature = 0.;
    double firstTime = 0.;
    double minimumPressure = 0.;
    double maximumPressure = 0.;
    double minimumTemperature = 0.;
    double maximumTemperature = 0.;

    void add(double pressure, double temperature, double time) {
        if (count == 0) {
            firstPressure = minimumPressure = maximumPressure = pressure;
            firstTemperature = minimumTemperature = maximumTemperature = temperature;
            firstTime = time;
        } else {
            minimumPressure = std::min(minimumPressure, pressure);
            maximumPressure = std::max(maximumPressure, pressure);
            minimumTemperature = std::min(minimumTemperature, temperature);
            maximumTemperature = std::max(maximumTemperature, temperature);
        }
        count++;
    }
};

struct LightMixtureFallback {
    StateRange states;
    double minimumMolarMass = 0.;
    double maximumMolarMass = 0.;
};

/// What one valve's artificial gas added up to over a run.
struct ArtificialGasTotal {
    long steps = 0;
    int episodes = 0;
    int lastStep = 0;
    double firstTime = 0.;
    double lastTime = 0.;
    double mass = 0.;
    double peakRate = 0.;
};

/// How often a stock-tank default was applied, and when first.
struct DefaultCount {
    long count = 0;
    double firstTime = 0.;
};

std::mutex registryMutex;
std::map<std::pair<FlashRoutine, int>, StateRange> flashErrors;
std::map<StockTankDefault, DefaultCount> stockTankDefaults;
std::map<int, LightMixtureFallback> lightMixtureFallbacks;
std::map<std::tuple<std::string, int, int>, ArtificialGasTotal> artificialGas;

template <typename... Values>
std::string formatted(const char *pattern, Values... values) {
    int size = std::snprintf(nullptr, 0, pattern, values...);
    if (size <= 0)
        return std::string();
    std::string text(size, '\0');
    std::snprintf(text.data(), size + 1, pattern, values...);
    return text;
}

std::string countOf(long count, const char *noun, const char *plural = nullptr) {
    if (count == 1)
        return formatted("%ld %s", count, noun);
    return plural != nullptr ? formatted("%ld %s", count, plural) : formatted("%ld %ss", count, noun);
}

const char *flashName(FlashRoutine routine) {
    return routine == FlashRoutine::StockTank ? "stock-tank flash" : "mixture flash";
}

const char *routineName(FlashRoutine routine) {
    return routine == FlashRoutine::StockTank ? "Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions"
                                              : "Marlim_CalculateMixtureThermodynamicCondition";
}

std::string describeStates(const StateRange &states) {
    std::string text = formatted("First at %.6g kgf/cm2 and %.6g C", states.firstPressure, states.firstTemperature);
    if (!std::isnan(states.firstTime))
        text += formatted(", simulation time %.6g s", states.firstTime);
    if (states.minimumPressure != states.maximumPressure || states.minimumTemperature != states.maximumTemperature)
        text += formatted("; pressure from %.6g to %.6g kgf/cm2, temperature from %.6g to %.6g C",
                          states.minimumPressure, states.maximumPressure, states.minimumTemperature,
                          states.maximumTemperature);
    text.push_back('.');
    return text;
}

std::string describeTime(double time) {
    return std::isnan(time) ? std::string() : formatted(" First at simulation time %.6g s.", time);
}

std::string describeValve(int valve) {
    return valve == 0 ? std::string("Master1 valve") : formatted("production valve %d", valve);
}

} // namespace

void recordFlashError(FlashRoutine routine, int errorCode, double pressure, double temperature, double time) {
    std::lock_guard<std::mutex> lock(registryMutex);
    flashErrors[{routine, errorCode}].add(pressure, temperature, time);
}

void recordStockTankDefault(StockTankDefault kind, double time) {
    std::lock_guard<std::mutex> lock(registryMutex);
    DefaultCount &entry = stockTankDefaults[kind];
    if (entry.count == 0)
        entry.firstTime = time;
    entry.count++;
}

void recordLightMixtureFallback(int errorCode, double pressure, double temperature, double mixtureMolarMass,
                                double time) {
    std::lock_guard<std::mutex> lock(registryMutex);
    LightMixtureFallback &fallback = lightMixtureFallbacks[errorCode];
    if (fallback.states.count == 0) {
        fallback.minimumMolarMass = fallback.maximumMolarMass = mixtureMolarMass;
    } else {
        fallback.minimumMolarMass = std::min(fallback.minimumMolarMass, mixtureMolarMass);
        fallback.maximumMolarMass = std::max(fallback.maximumMolarMass, mixtureMolarMass);
    }
    fallback.states.add(pressure, temperature, time);
}

void recordArtificialGas(const std::string &system, const ArtificialGasAddition &addition, int step, double time,
                         double timeStep) {
    std::lock_guard<std::mutex> lock(registryMutex);
    ArtificialGasTotal &total = artificialGas[{system, addition.valve, addition.cell}];
    if (total.steps == 0)
        total.firstTime = time;
    if (total.steps == 0 || step != total.lastStep + 1)
        total.episodes++;
    total.steps++;
    total.lastStep = step;
    total.lastTime = time + timeStep;
    total.mass += addition.massRate * timeStep;
    total.peakRate = std::max(total.peakRate, addition.massRate);
}

void flushTo(Logger &logger) {
    std::lock_guard<std::mutex> lock(registryMutex);
    for (const auto &[key, states] : flashErrors) {
        const auto &[routine, errorCode] = key;
        logger.log(LOGGER_AVISO, LOG_WARN_RUNTIME_DIAGNOSTIC,
                   formatted(" the %s returned error code %d in %s, and its results were used as returned. ",
                             flashName(routine), errorCode, countOf(states.count, "call").c_str()) +
                       describeStates(states),
                   routineName(routine), formatted("iIER = %d", errorCode));
    }
    for (const auto &[kind, entry] : stockTankDefaults) {
        const bool api = kind == StockTankDefault::ApiGravity;
        logger.log(LOGGER_AVISO, LOG_WARN_RUNTIME_DIAGNOSTIC,
                   formatted(api ? " the stock-tank flash gave no liquid density in %s, and the API gravity was set "
                                   "to 50."
                                 : " the stock-tank flash gave vapor without a gas-oil ratio in %s, and the gas-oil "
                                   "ratio was set to 1e6 m3/m3.",
                             countOf(entry.count, "call").c_str()) +
                       describeTime(entry.firstTime),
                   routineName(FlashRoutine::StockTank),
                   api ? "dStockTankLiquidDensity <= 0.01" : "gas-oil ratio below 1e-15 with vapor");
    }
    for (const auto &[errorCode, fallback] : lightMixtureFallbacks) {
        std::string molarMass = fallback.minimumMolarMass == fallback.maximumMolarMass
                                    ? formatted("%.6g", fallback.minimumMolarMass)
                                    : formatted("from %.6g to %.6g", fallback.minimumMolarMass,
                                                fallback.maximumMolarMass);
        logger.log(LOGGER_AVISO, LOG_WARN_RUNTIME_DIAGNOSTIC,
                   formatted(" the light-mixture fallback replaced the results of %s that returned error code %d "
                             "with vapor of the feed composition: mixture molar mass %s, at most 30. ",
                             countOf(fallback.states.count, "mixture flash", "mixture flashes").c_str(), errorCode, molarMass.c_str()) +
                       describeStates(fallback.states),
                   routineName(FlashRoutine::Mixture), formatted("iIER = %d", errorCode));
    }
    for (const auto &[key, total] : artificialGas) {
        const auto &[system, valve, cell] = key;
        logger.log(LOGGER_AVISO, LOG_WARN_RUNTIME_DIAGNOSTIC,
                   formatted(" the closing-valve time-step control added %.6g kg of artificial gas to the source of "
                             "cell %d, at the %s, in %s: %s in %s, from %.6g s to %.6g s, peak rate %.6g kg/s.",
                             total.mass, cell, describeValve(valve).c_str(), system.c_str(),
                             countOf(total.steps, "accepted time step").c_str(),
                             countOf(total.episodes, "episode").c_str(), total.firstTime, total.lastTime,
                             total.peakRate),
                   "controleDTvalv", "a closing valve with a void fraction below 0.05 in its cell");
    }
    flashErrors.clear();
    stockTankDefaults.clear();
    lightMixtureFallbacks.clear();
    artificialGas.clear();
}

} // namespace diagnostics
