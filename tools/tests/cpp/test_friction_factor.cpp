// ==============================================================================
// test_friction_factor.cpp
// Unit test for the single-phase Moody-Darcy friction factor.
//
// Verifies frictionFactor() against a converged Colebrook-White root across the
// Moody chart, plus the laminar branch.
//
// This function feeds the frictional pressure gradient of every black-box
// correlation (correlacaoMR2 = 0..16, used when tipoModeloDrift = false), so an
// error here propagates directly into steady-state pressure profiles.
//
// Build (opt-in; the default engine build is unaffected):
//     cmake -S . -B build -DMARLIM_BUILD_TESTS=ON
//     cmake --build build --target test_friction_factor
//     ctest --test-dir build --output-on-failure
// ==============================================================================
#include "GradientCorrelations.h"

#include <cmath>
#include <cstdio>
#include <vector>

namespace {

// Reference: iterate 1/sqrt(f) = -2 log10(eps/3.7 + 2.51/(Re sqrt(f))) to
// convergence. This is the exact implicit root that the explicit
// approximations are meant to reproduce.
double colebrookRoot(double reynolds, double relRoughness) {
    double f = 0.02;
    for (int i = 0; i < 500; ++i) {
        const double rhs =
            -2.0 * std::log10(relRoughness / 3.7 + 2.51 / (reynolds * std::sqrt(f)));
        const double next = 1.0 / (rhs * rhs);
        if (std::fabs(next - f) < 1.0e-15) {
            return next;
        }
        f = next;
    }
    return f;
}

int failures = 0;

void check(bool condition, const char *what, double reynolds, double relRoughness,
           double got, double expected) {
    if (condition) {
        return;
    }
    ++failures;
    std::printf("FAIL  %-11s Re=%.3e  eps/D=%.3e  got=%.8f  expected=%.8f  (%+.3f%%)\n",
                what, reynolds, relRoughness, got, expected,
                (got - expected) / expected * 100.0);
}

} // namespace

int main() {
    // Serghides' published accuracy is ~0.0025% across the Moody chart. A 0.05%
    // tolerance absorbs compiler-dependent rounding while still catching a
    // regression to the previous formulation, which erred by up to 11.2%.
    const double TOLERANCE_PERCENT = 0.05;

    const std::vector<double> reynolds = {5.0e3, 1.0e4, 3.0e4, 1.0e5,
                                          3.0e5, 1.0e6, 3.0e6, 1.0e7};
    const std::vector<double> relRoughness = {1.0e-6, 1.0e-5, 1.0e-4,
                                              1.0e-3, 1.0e-2, 5.0e-2};

    double worst = 0.0;
    for (std::size_t i = 0; i < reynolds.size(); ++i) {
        for (std::size_t j = 0; j < relRoughness.size(); ++j) {
            const double re = reynolds[i];
            const double eps = relRoughness[j];

            double value = 0.0;
            frictionFactor(re, eps, value);

            const double expected = colebrookRoot(re, eps);
            const double deviation = std::fabs(value - expected) / expected * 100.0;
            if (deviation > worst) {
                worst = deviation;
            }
            check(deviation <= TOLERANCE_PERCENT, "turbulent", re, eps, value, expected);
        }
    }

    // Laminar branch: f = 64/Re for Re <= 2100 (Hagen-Poiseuille).
    const double laminar[] = {500.0, 1000.0, 2000.0, 2100.0};
    for (std::size_t i = 0; i < sizeof(laminar) / sizeof(laminar[0]); ++i) {
        double value = 0.0;
        frictionFactor(laminar[i], 1.0e-4, value);
        const double expected = 64.0 / laminar[i];
        check(std::fabs(value - expected) < 1.0e-12, "laminar", laminar[i], 1.0e-4,
              value, expected);
    }

    // The result must be the Moody-Darcy factor, not Fanning (4x smaller).
    // Cel::fric in celula3.cpp returns Fanning, so confusing the two is a
    // plausible regression; pin the magnitude at a well-known point.
    {
        double value = 0.0;
        frictionFactor(1.0e5, 1.0e-4, value);
        const double expected = colebrookRoot(1.0e5, 1.0e-4);
        check(std::fabs(value - expected / 4.0) > 1.0e-6, "not-fanning", 1.0e5, 1.0e-4,
              value, expected);
    }

    std::printf("worst deviation from the Colebrook root: %.5f%% (tolerance %.3f%%)\n",
                worst, TOLERANCE_PERCENT);
    if (failures == 0) {
        std::printf("PASSED\n");
        return 0;
    }
    std::printf("FAILED: %d check(s)\n", failures);
    return 1;
}
