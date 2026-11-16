#ifndef RUNTIMEDIAGNOSTICS_H_
#define RUNTIMEDIAGNOSTICS_H_

/// Warnings the solution gathers while it runs, for conditions that can recur at
/// every cell of every time step. One log entry per occurrence would flood the
/// log, so each recorded key keeps counts and state or time information and
/// becomes one entry when the log is written.
///
/// Recording is thread-safe. Callers test the triggering conditions before
/// recording; the recording functions aggregate the occurrences they receive.
/// The registry is shared by all callers and loggers. Entries reach the logger
/// only through flushTo.

#include <string>

class Logger;

namespace diagnostics {

/// The compositional flash routines whose general error code, iIER, is checked.
enum class FlashRoutine {
    /// Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions, for API, gas
    /// density and gas-oil ratio at the stock tank.
    StockTank,
    /// Marlim_CalculateMixtureThermodynamicCondition, at a given pressure and
    /// temperature.
    Mixture,
};

/// A flash returned a nonzero general error code, and the caller used its results
/// as returned. Groups calls by routine and errorCode, retaining the first
/// recorded state and the pressure and temperature ranges. Pressure in kgf/cm2,
/// temperature in C, simulation time in s. A NaN first time is omitted from the
/// message, even if later calls provide a known time.
void recordFlashError(FlashRoutine routine, int errorCode, double pressure, double temperature, double time);

/// What the caller of the stock-tank flash puts in place of a result it cannot use.
enum class StockTankDefault {
    /// No stock-tank liquid density: the API gravity is set to 50.
    ApiGravity,
    /// Vapor without a gas-oil ratio: the gas-oil ratio is set to 1e6.
    GasOilRatio,
};

/// The stock-tank flash's caller applied `kind` at simulation time `time`.
/// Groups calls by kind and retains the first recorded time, in s. A NaN first
/// time is omitted from the message.
void recordStockTankDefault(StockTankDefault kind, double time);

/// A mixture flash failed with a positive code and, the mixture being light, the
/// caller replaced its results with vapor of the feed composition.
/// Groups calls by errorCode, retaining the first recorded state and the ranges
/// of pressure, temperature and mixture molar mass. Pressure in kgf/cm2,
/// temperature in C, simulation time in s. A NaN first time is omitted from the
/// message. The caller determines when the fallback applies.
void recordLightMixtureFallback(int errorCode, double pressure, double temperature, double mixtureMolarMass,
                                double time);

/// Gas the closing-valve time-step control adds to the source of the valve's
/// cell, when a closing valve holds almost no gas. The time step that uses it
/// may still be shortened, so a step keeps its additions until it is accepted.
struct ArtificialGasAddition {
    /// 0 for the Master1 valve, k for the production valve k.
    int valve;
    int cell;
    /// kg/s.
    double massRate;
};

/// An accepted time step, `step` of `system`'s run, that started at `time` and
/// lasted `timeStep` seconds with `addition` in its sources.
/// Groups additions by system, valve and cell. Each call increments the step
/// count. An episode starts at the first call or when step does not immediately
/// follow the last recorded step; repeated step numbers are counted separately.
/// Retains the first recorded start time and the last recorded time + timeStep,
/// accumulates addition.massRate * timeStep in kg, and tracks the peak mass rate.
/// The caller records additions only after accepting the time step.
void recordArtificialGas(const std::string &system, const ArtificialGasAddition &addition, int step, double time,
                         double timeStep);

/// Writes one warning per recorded key to logger: flash errors, stock-tank
/// defaults, light-mixture fallbacks, then artificial gas, in ascending key order
/// within each category. Counts use the singular only for one occurrence;
/// otherwise the plural appends s unless a specific plural is supplied.
/// Holds the registry lock through logging and clears all entries after every
/// logger call completes successfully. A second call without new records adds
/// nothing. If logging throws, the exception propagates and the registry is
/// retained; entries already written to logger are not rolled back.
/// The caller must invoke this on the thread writing the log, outside parallel
/// regions, and prevent concurrent access to the same logger.
void flushTo(Logger &logger);

} // namespace diagnostics

#endif
