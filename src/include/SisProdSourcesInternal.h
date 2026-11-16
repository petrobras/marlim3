#ifndef SISPRODSOURCESINTERNAL_H_
#define SISPRODSOURCESINTERNAL_H_

#include "SisProdSources.h"

namespace sisprod::sources {
namespace {

/// Reads the water and gas consumed by hydrate formation after 0.01 s for
/// envelope models 2 and 3, and updates BSW from the remaining free-water volume.
/// Inactive models leave both output arguments unchanged.
void consumeHydrateFormationMass(const SourceState &state, double &consumedGasMass,
                                double &consumedWaterMass, int cellIndex);

/// Evaluates the donor fluid's in-situ water fraction at the receiving cell's
/// pressure and temperature, using the receiving fluid's gas-oil-ratio criterion.
double inSituWaterFraction(const Cel &receivingCell, Cel &donorCell);

/// Prepares a leak's fluid and phase quality, then accumulates its calculated
/// rates or the recorded rates for the matching primary parallel connection.
void refreshLeakSource(const SourceState &state, int cellIndex);

/// Applies the well pressure and donor water fraction to the radial reservoir,
/// advances its transient or pseudo-transient state, and assigns phase sources.
void advanceRadialPorousSource(const SourceState &state, int cellIndex);

/// Applies the well pressure and donor water fraction to the 2D reservoir and
/// its transfer region, advances the selected model, and assigns phase sources.
void advancePorous2DSource(const SourceState &state, int cellIndex);

/// Applies dry or wet gas injection, or complementary-fluid injection, using
/// the receiving pressure and temperature captured at source-renewal entry.
void applyGasInjectionSource(const SourceState &state, int cellIndex, double cellPressure, double cellTemperature);

/// Splits injected production liquid and solution gas at the captured receiving
/// pressure and temperature, and accumulates the complementary-liquid source.
void applyLiquidInjectionSource(const SourceState &state, int cellIndex, double cellPressure, double cellTemperature);

/// Applies the IPR production or reverse-inflow rates and their phase derivatives
/// at the receiving pressure and temperature captured at source-renewal entry.
void applyInflowPerformanceSource(const SourceState &state, int cellIndex, double cellPressure, double cellTemperature);

/// Clears the next cell's sources, updates the pump fluids and, at an active
/// frequency, subtracts its phase rates here and adds them to the next cell.
void applyVolumetricPumpSource(const SourceState &state, int cellIndex);

/// Updates source terms for leaks, multiple sources and radial or 2D porous media.
/// Leaks accumulate mass; the other models assign their phase contributions.
/// A primary parallel-network connection may supply its recorded leak rates.
void refreshChokeMultipleAndPorousSources(const SourceState &state, int cellIndex);

}  // namespace
}  // namespace sisprod::sources

#endif  // SISPRODSOURCESINTERNAL_H_
