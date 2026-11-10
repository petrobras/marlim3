// ==============================================================================
// test_tuffp_runtime_binding.cpp
// Unit test for the runtime-loaded TUFFP Unified gas-liquid model adapter
// (ssFlowModel / correlacaoMR2 = 51).
//
// TUFFP is an external library that is not distributed with Marlim3. The test
// exits with code 77 (reported as "skipped" by CTest) when the library cannot
// be loaded. Point MARLIM3_TUFFP_LIB to the TUFFP shared library, or place it
// next to the test executable, to run it:
//     cmake -S . -B build -DMARLIM_BUILD_TESTS=ON
//     cmake --build build --target test_tuffp_runtime_binding
//     set MARLIM3_TUFFP_LIB=C:\path\to\tuffp_v2026.0.dll
//     ctest --test-dir build -R tuffp_runtime_binding --output-on-failure
// ==============================================================================
#include "tuffp_runtime_binding.h"

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

// Layouts must match the Fortran derived types (natural alignment, int32).
static_assert(sizeof(tuffp::Loc1) == 12, "Loc1 layout");
static_assert(sizeof(tuffp::FilmM) == 40, "FilmM layout");
static_assert(offsetof(tuffp::FilmM, fm) == 8, "FilmM layout");
static_assert(sizeof(tuffp::SlugM) == 28, "SlugM layout");
static_assert(sizeof(tuffp::Inf) == 32, "Inf layout");
static_assert(offsetof(tuffp::Inf, nm) == 8, "Inf layout");
static_assert(sizeof(tuffp::FilmN) == 288, "FilmN layout");
static_assert(sizeof(tuffp::Prg) == 56, "Prg layout");
static_assert(sizeof(tuffp::Lay) == 64, "Lay layout");
static_assert(sizeof(tuffp::Film) == 208, "Film layout");

namespace {

constexpr int SKIP = 77;
constexpr double G = 9.80665;
int failures = 0;

void check(bool condition, const std::string& what) {
    if (!condition) {
        ++failures;
        std::printf("FAIL  %s\n", what.c_str());
    }
}

TuffpInput airWater(double vsg, double vsl, double angleDeg, double diameter = 0.05) {
    TuffpInput in{};
    in.vSG = vsg;
    in.vSL = vsl;
    in.rhoG = 1.225;
    in.rhoL = 998.0;
    in.muG = 1.8e-5;
    in.muL = 1.0e-3;
    in.sigma = 0.072;
    in.diameter = diameter;
    in.roughness = 0.0;
    in.angle = angleDeg;
    return in;
}

void print(const char* label, const TuffpResult& r) {
    std::printf("%-28s fpc=%3d HL=%.5f grav=%11.4f fric=%11.4f acc=%9.4f tot=%11.4f err=%d %s\n",
                label, r.patternCode, r.HL, r.dpdL_grav, r.dpdL_fric, r.dpdL_acc, r.dpdL_tot,
                r.error ? 1 : 0, r.errorReport.c_str());
}

bool finiteResult(const TuffpResult& r) {
    return std::isfinite(r.HL) && std::isfinite(r.dpdL_grav) && std::isfinite(r.dpdL_fric) &&
           std::isfinite(r.dpdL_acc) && std::isfinite(r.dpdL_tot);
}

void checkComponentsSum(const char* label, const TuffpResult& r) {
    const double sum = r.dpdL_grav + r.dpdL_fric + r.dpdL_acc;
    check(std::fabs(sum - r.dpdL_tot) <= 1e-6 * std::fmax(1.0, std::fabs(r.dpdL_tot)),
          std::string(label) + ": grav + fric + acc == tot");
}

} // namespace

int main() {
    std::string message;
    if (!tuffpLoad(message)) {
        std::printf("SKIP  %s\n", message.c_str());
        return SKIP;
    }
    std::printf("TUFFP library: %s\n", tuffpLibraryPath().c_str());

    // Vendor example (TUFFP_v2026.cpp): horizontal air-water, D = 0.05 m.
    const TuffpResult horizontal = calculateTuffp(airWater(1.0, 1.0, 0.0));
    print("horizontal vsg=vsl=1", horizontal);
    check(finiteResult(horizontal), "horizontal: finite results");
    check(!horizontal.error, "horizontal: no TUFFP error flags");
    check(horizontal.patternCode == static_cast<int32_t>(tuffp::Pattern::Slug),
          "horizontal: slug flow expected");
    check(horizontal.HL > 0.0 && horizontal.HL < 1.0, "horizontal: 0 < HL < 1");
    check(std::fabs(horizontal.dpdL_grav) < 1e-6, "horizontal: no gravity gradient");
    check(horizontal.dpdL_fric > 0.0, "horizontal: positive friction gradient");
    checkComponentsSum("horizontal", horizontal);

    // Vertical upward: gravity gradient positive and consistent with the holdup.
    const TuffpResult vertical = calculateTuffp(airWater(1.0, 1.0, 90.0));
    print("vertical vsg=vsl=1", vertical);
    check(finiteResult(vertical), "vertical: finite results");
    check(vertical.HL > 0.0 && vertical.HL < 1.0, "vertical: 0 < HL < 1");
    const double rhoMix = vertical.HL * 998.0 + (1.0 - vertical.HL) * 1.225;
    check(std::fabs(vertical.dpdL_grav - rhoMix * G) < 0.05 * rhoMix * G,
          "vertical: gravity gradient ~ rho_mix * g");
    checkComponentsSum("vertical", vertical);

    // Annular: high gas rate, low liquid rate, vertical.
    const TuffpResult annular = calculateTuffp(airWater(30.0, 0.01, 90.0));
    print("vertical annular", annular);
    check(finiteResult(annular), "annular: finite results");
    check(annular.patternCode == static_cast<int32_t>(tuffp::Pattern::Annular) ||
          annular.patternCode == static_cast<int32_t>(tuffp::Pattern::AnnularTwoFilms),
          "annular: annular flow expected");

    // Single-phase limits.
    const TuffpResult liquid = calculateTuffp(airWater(0.0, 1.0, 30.0));
    print("single-phase liquid", liquid);
    check(liquid.patternCode == static_cast<int32_t>(tuffp::Pattern::SinglePhaseLiquid),
          "liquid: single-phase liquid pattern");
    check(std::fabs(liquid.HL - 1.0) < 1e-9, "liquid: HL == 1");

    const TuffpResult gas = calculateTuffp(airWater(5.0, 0.0, 30.0));
    print("single-phase gas", gas);
    check(gas.patternCode == static_cast<int32_t>(tuffp::Pattern::SinglePhaseGas),
          "gas: single-phase gas pattern");
    check(std::fabs(gas.HL) < 1e-9, "gas: HL == 0");

    // Absolute roughness is converted to relative roughness: rough > smooth friction.
    TuffpInput roughIn = airWater(1.0, 1.0, 0.0);
    roughIn.roughness = 5.0e-4;
    const TuffpResult rough = calculateTuffp(roughIn);
    print("horizontal rough", rough);
    check(rough.dpdL_fric > horizontal.dpdL_fric, "rough: larger friction than smooth");

    // No flow: hydrostatic column with the caller-provided holdup (TUFFP itself yields NaN).
    TuffpInput staticIn = airWater(0.0, 0.0, 30.0);
    staticIn.noFlowHoldup = 0.4;
    const TuffpResult noFlow = calculateTuffp(staticIn);
    print("no flow", noFlow);
    check(finiteResult(noFlow), "no flow: finite results");
    check(!noFlow.error, "no flow: no error");
    check(std::fabs(noFlow.HL - 0.4) < 1e-12, "no flow: HL == noFlowHoldup");
    check(noFlow.dpdL_fric == 0.0 && noFlow.dpdL_acc == 0.0, "no flow: no friction/acceleration");
    check(std::fabs(noFlow.dpdL_grav - (0.4 * 998.0 + 0.6 * 1.225) * G * 0.5) < 1e-9,
          "no flow: hydrostatic gravity gradient");
    checkComponentsSum("no flow", noFlow);

    // Reverse co-current flow: mirror of the forward problem, gradients in the pipe frame.
    const TuffpResult forwardDown = calculateTuffp(airWater(1.0, 0.5, -20.0));
    const TuffpResult reverse = calculateTuffp(airWater(-1.0, -0.5, 20.0));
    print("reverse co-current", reverse);
    check(finiteResult(reverse), "reverse: finite results");
    check(!reverse.error, "reverse: no error");
    check(reverse.patternCode == forwardDown.patternCode && reverse.HL == forwardDown.HL,
          "reverse: same pattern and holdup as the mirrored problem");
    check(reverse.dpdL_fric == -forwardDown.dpdL_fric && reverse.dpdL_grav == -forwardDown.dpdL_grav,
          "reverse: gradients negated");
    check(reverse.dpdL_fric < 0.0, "reverse: friction opposes the (negative) flow");
    check(reverse.dpdL_grav > 0.0, "reverse: gravity positive for an upward pipe");
    checkComponentsSum("reverse", reverse);

    // Counter-current flow: outside model scope, flagged but finite.
    const TuffpResult counter = calculateTuffp(airWater(1.0, -0.2, 90.0));
    print("counter-current", counter);
    check(finiteResult(counter), "counter-current: finite results");
    check(counter.error, "counter-current: flagged");
    check(counter.patternCode == static_cast<int32_t>(tuffp::Pattern::SinglePhaseGas),
          "counter-current: opposing liquid neglected");

    // Concurrent calls (Marlim3 uses OpenMP) must reproduce the serial results.
    std::vector<TuffpInput> inputs;
    for (int k = 0; k < 64; ++k) {
        inputs.push_back(airWater(0.2 + 0.3 * (k % 8), 0.05 + 0.2 * (k / 8), -10.0 + 1.5 * k));
    }
    std::vector<TuffpResult> serial;
    for (const auto& in : inputs) serial.push_back(calculateTuffp(in));
    std::vector<TuffpResult> parallel(inputs.size());
    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back([&, t] {
            for (size_t k = static_cast<size_t>(t); k < inputs.size(); k += 8) {
                parallel[k] = calculateTuffp(inputs[k]);
            }
        });
    }
    for (auto& th : threads) th.join();
    bool same = true;
    for (size_t k = 0; k < inputs.size(); ++k) {
        same = same && serial[k].patternCode == parallel[k].patternCode &&
               serial[k].HL == parallel[k].HL && serial[k].dpdL_tot == parallel[k].dpdL_tot;
    }
    check(same, "concurrent calls match serial results");

    if (failures == 0) {
        std::printf("PASS  all TUFFP adapter checks\n");
        return 0;
    }
    std::printf("%d check(s) failed\n", failures);
    return 1;
}
