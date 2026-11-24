#ifndef SISPRODSTEADYSTATESEARCH_H_
#define SISPRODSTEADYSTATESEARCH_H_

#include "SisProdSteadyState.h"

// Searches compose the march state and receive coupled-model views explicitly.

namespace sisprod::steady {

/// March state and additional inputs used by boundary-condition searches.
struct SteadyStateSearchState {
    /// Shared state passed to the selected steady march.
    SteadyStateState march;

    /// Holdup guess carried between search attempts -- SProd::steadySearch.holdupGuess.
    /// Read only.
    const double &holdupGuess;
    /// Reverse-flow network fluid -- SProd::steadySearch.reverseNetworkFluid. Read only.
    const ProFlu &reverseNetworkFluid;
    /// Whether this steady solve runs in reverse -- SProd::steadySearch.steadyReverseFlow. Written.
    bool &reverseSteady;
};

/// Runs the march selected by well type, boundary conditions and reverse-flow state.
/// The production selector is 0 for gas, 1 for bottom-hole pressure and otherwise pressure-to-pressure.
[[nodiscard]] double dispatchMarch(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double guess,
                                   int isProduction, int boundaryConditionKind);

/// Solves the bracketed march residual with zriddr.
/// Normalizes residuals by the reference convergence monitor and updates the production monitor.
/// Strong tubing-annulus coupling requires at least ten root iterations.
[[nodiscard]] double solveSteadyRoot(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double x1, double x2,
                                     int isProduction, int boundaryConditionKind);

/// Searches bottom-hole pressure in kgf/cm2 for the production balance.
/// Any negative guess requests an automatic estimate; a negative attemptCount identifies the initial attempt.
[[nodiscard]] double searchProductionBottomHolePressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                        double guess = -1., int attemptCount = -1);

/// Searches bottom-hole pressure in kgf/cm2 using the reverse production march.
/// Any negative guess requests an automatic estimate.
[[nodiscard]] double searchReverseProductionBottomHolePressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                               double guess = -1.);

/// Searches bottom-hole pressure in kgf/cm2 with a surface choke.
/// Any negative guess requests an automatic estimate; attemptCount does not affect this search.
[[nodiscard]] double searchProductionBottomHolePressureSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                                 double guess = -1., int attemptCount = -1);

/// Iterates the production and gas-line marches from the supplied inlet pressure in kgf/cm2.
/// Updates the corrected surface pressure and returns the last-cell pressure before that correction.
[[nodiscard]] double searchProductionBottomHolePressureTertiary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                                double inletPressure);

/// Searches the standard source flow rate in m3/day between fixed production pressures.
/// Uses maximumFlowRate when positive and carries the network iteration count.
/// Standalone failures report completed residuals separately from pressure-limited marches.
[[nodiscard]] double searchProductionPressureToPressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double massFlowRateGuess,
                                                        double maximumFlowRate = 0., int iterationCount = 0);

/// Searches the standard source flow rate in m3/day between fixed pressures using the reverse march.
[[nodiscard]] double searchReverseProductionPressureToPressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                               double massFlowRateGuess, double maximumFlowRate = 0.,
                                                               int iterationCount = 0);

/// Searches the standard source flow rate in m3/day between fixed pressures with a surface choke.
[[nodiscard]] double searchProductionPressureToPressureSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                                 double massFlowRateGuess, double maximumFlowRate = 0.);

/// Searches the standard source flow rate in m3/day between fixed pressures with a closed outlet.
[[nodiscard]] double searchProductionPressureToPressureTertiary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies,
                                                                double massFlowRateGuess, double maximumFlowRate = 0.);

/// Searches gas-line pressure in kgf/cm2 for a prescribed injection flow rate.
/// Updates gas-line state and returns the pressure or a convergence failure sentinel.
double searchGasPressureSteadySecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies);

/// Searches gas-line pressure in kgf/cm2 with an active injection choke.
/// Updates gas-line state and returns the pressure or a convergence failure sentinel.
double searchGasPressureSteadyTertiary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies);

/// Searches the injection flow-rate guess that closes the prescribed pressure balance.
[[nodiscard]] double searchInjectionBottomHolePressure1(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double guess = -1.);

/// Searches injection surface pressure in kgf/cm2 for the prescribed flow rate.
/// A negative guess requests an automatic hydrostatic estimate.
[[nodiscard]] double searchInjectionBottomHolePressure2(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double guess = -1.);

/// Iterates the injection flow and pressure boundary to a fixed point.
[[nodiscard]] double searchInjectionBottomHolePressure3(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double guess = -1.);

/// Marches the injection column and returns its total mass flow.
[[nodiscard]] double searchInjectionBottomHolePressure4(const SteadyStateSearchState &state, const SteadyDependencies &dependencies);

/// Searches injection surface pressure in kgf/cm2 with the fifth boundary-condition variant.
/// A negative guess requests an automatic hydrostatic estimate.
[[nodiscard]] double searchInjectionBottomHolePressure5(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double guess = -1.);

}  // namespace sisprod::steady

#endif  // SISPRODSTEADYSTATESEARCH_H_
