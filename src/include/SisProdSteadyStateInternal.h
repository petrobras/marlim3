#ifndef SISPRODSTEADYSTATEINTERNAL_H_
#define SISPRODSTEADYSTATEINTERNAL_H_

#include "SisProdSteadyState.h"

namespace sisprod::steady {
namespace {

/// Initial gas void fraction and complementary-liquid fraction of the first cell.
struct InitialPhaseFractions {
    double voidFraction;
    double complementaryLiquidFraction;
};

/// Refreshes the cell flash when untabulated, then the source flash, in compositional mode.
/// The source gas-mode flag is passed through unchanged.
void refreshSeedCompositionalProperties(const SteadyStateState &state, double pressureGuess,
                                        ProFlu &sourceFluid, int gasMode = 0);

/// Converts the first wet-gas source's reference and in-situ flow rates to initial fractions.
InitialPhaseFractions wetGasSeedFractions(const SteadyStateState &state, double pressureGuess);

/// Sets the first-cell temperature and source state, then estimates its phase fractions.
/// Porous sources publish their water cut after their flow rates have been evaluated.
InitialPhaseFractions seedFirstCellVoidFraction(const SteadyStateState &state,
                                                const SteadyDependencies &dependencies,
                                                double pressureGuess);

/// Initializes temperature fields and zero upstream flow rates for a source in a closed first cell.
/// Called once per pressure-guess evaluation, before any march pass.
void initializeClosedSourceBoundary(const SteadyStateState &state);

/// Restores the pressure guess and fractions in the first cells, faces and initial-time fields.
/// Called at the start of every column-march attempt.
void resetProductionMarchInlet(const SteadyStateState &state, double pressureGuess,
                               const InitialPhaseFractions &fractions);

}  // namespace
}  // namespace sisprod::steady

#endif  // SISPRODSTEADYSTATEINTERNAL_H_
