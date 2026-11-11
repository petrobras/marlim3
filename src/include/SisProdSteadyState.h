#ifndef SISPRODSTEADYSTATE_H_
#define SISPRODSTEADYSTATE_H_

#include <vector>

// Cell, input and accessory types are referenced without requiring their definitions.
class Cel;
class CelG;
class ChokeGas;
class Ler;
class ProFlu;
class choke;
struct SimulationContext;

namespace driftflux::coefficient { struct ClosureState; }
namespace sisprod::sources { struct SourceState; }
namespace sisprod::thermal { struct ThermalState; struct ThermalDependencies; }
namespace sisprod::gaslift { struct GasLiftState; }

namespace sisprod::steady {

struct SteadyStateSearchState;

/// Physical-model views used by steady marches and boundary searches.
struct SteadyDependencies {
    const sources::SourceState &sources;
    const driftflux::coefficient::ClosureState &closure;
    const thermal::ThermalState &thermal;
    const thermal::ThermalDependencies &thermalDependencies;
    const gaslift::GasLiftState &gasLift;
    const SteadyStateSearchState &search;
};

/// References to the cells, configuration and mutable state used by the steady marches.
/// Scalars remain references so that values are read only when a march needs them.
/// Const references identify inputs that these routines do not modify.
struct SteadyStateState {
    /// Production cells -- SProd::celula. Written.
    Cel *const &cells;
    /// Gas-line cells -- SProd::gasLift.cells. Written.
    CelG *const &gasCells;
    /// Input deck -- SProd::arq. NOT const: the march writes back into it.
    Ler &input;
    /// Shared simulation context -- SProd::simulationContext. Read only.
    const SimulationContext *const &globals;

    /// Index of the last production cell -- SProd::ncel.
    const int &lastCell;
    /// Index of the last gas-line cell -- SProd::gasLift.cellCount.
    const int &lastGasCell;

    /// Injection choke -- SProd::gasLift.injectionChoke. Written.
    ChokeGas &injectionChoke;
    /// Surface choke -- SProd::chokeSup. Not const: marchProductionSteadySecondary
    /// calls vazmassSachd and vazmaxSachd on it, and neither is const-qualified.
    choke &surfaceChoke;
    /// Gas-line and production cell index of each gas-lift valve --
    /// SProd::gasLift.gasValveCells and posicVGLP.
    const std::vector<int> &gasValveCellIndices;
    const std::vector<int> &productionValveCellIndices;

    /// Steady-state iteration counter -- SProd::steadyIteration. Written.
    int &steadyIteration;
    /// Which boundary the search started from -- SProd::steadySearch.initialEstimateSearch. Written.
    bool &searchOrigin;
    /// Convergence monitors -- SProd::steadySearch.convergenceMonitor and convergenceMonitorReference.
    /// Both written.
    double &convergenceMonitor;
    double &baseConvergenceMonitor;

    /// Annulus drift flag -- SProd::derivaAnel. Read only.
    const int &annulusDrift;
    /// Network coupling flag -- SProd::gasLift.annulusThermalCoupling. Read only.
    const bool &networkCoupled;
    /// End-node flag -- SProd::noextremo. Read only.
    const int &endNode;
    /// Thermal source switch -- SProd::thermalSourceDisabled. Read only.
    const bool &thermalSourceDisabled;
    /// Slow-heat-transfer switch -- SProd::steadySearch.slowHeatTransferThreshold. Written.
    double &slowHeatTransferThreshold;

    /// Surface gas pressure -- SProd::gasLift.separatorPressure. Written.
    double &gasSurfacePressure;
    /// Previous-step gas pressure and temperature -- SProd::gasLift.injectionPressure and
    /// tempiniG. Read only here; the gas line owns them.
    const double &initialGasPressure;
    const double &initialGasTemperature;
    /// Pressure the march ends on -- SProd::presfim. Written.
    double &outletPressure;
    /// Time step -- SProd::dt. Written: the pseudo-transient step shortens it.
    double &timeStep;

    /// Casing temperature -- SProd::steadySearch.reverseFlowTemperature. Read only.
    const double &casingTemperature;
    /// Inlet quality -- SProd::titE. Read only.
    const double &inletQuality;
    /// Number of production fluids -- SProd::steadySearch.productionFluidCount. Read only.
    const int &productionFluidCount;

};

/// Advances the forward black-oil mass balance and publishes cell and face fractions.
void advanceSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int cellIndex);

/// Advances the reverse black-oil mass balance and publishes cell and face fractions.
void advanceReverseSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int cellIndex);

/// Advances the forward compositional mass balance, including source mixing and flash properties.
void advanceCompositionalSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int cellIndex);

/// Advances the reverse compositional mass balance and publishes cell and face fractions.
void advanceReverseCompositionalSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int cellIndex);

/// Updates phase mass transfer at the upstream cell of a steady mass step.
void advanceSteadyMassTransfer(const SteadyStateState &state, int cellIndex);

/// Updates phase mass transfer at the upstream gas cell of a steady mass step.
void advanceSteadyGasMassTransfer(const SteadyStateState &state, int cellIndex);

/// Advances pressure to the upstream face using the specified Runge-Kutta stage.
void advanceUpstreamSteadyPressure(const SteadyStateState &state, int cellIndex, int rungeKuttaStage);

/// Advances pressure from the upstream face to the cell centre at the specified Runge-Kutta stage.
void advanceDownstreamSteadyPressure(const SteadyStateState &state, int cellIndex, int rungeKuttaStage);

/// Returns the pressure contribution of an area change using the supplied mixture properties.
[[nodiscard]] double areaChangePressureDrop(const SteadyStateState &state, int cellIndex,
                                            double mixtureDensity, double reynolds, double mixtureFlux);

/// Returns the last-cell pressure with the outlet half-cell pressure contribution.
[[nodiscard]] double steadyPressureAtLastCell(const SteadyStateState &state);

/// Marches the production column from a bottom-hole pressure guess in kgf/cm2.
/// Initializes a closed upstream face with a source in the first cell. Each
/// admitted pass restores inlet pressure and fractions before marching the column
/// and gas line. The current convergence test admits at most one accelerated
/// pass or two unaccelerated passes; the pressure search controls final convergence.
/// Returns surface gas pressure minus corrected outlet pressure, or a march failure sentinel.
[[nodiscard]] double marchProductionSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double pressureGuess);

/// Marches the production column in reverse from a bottom-hole pressure guess in kgf/cm2.
/// Returns surface gas pressure minus corrected outlet pressure, or a march failure sentinel.
[[nodiscard]] double marchReverseProductionSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double pressureGuess);

/// Marches the production column with a surface choke from a pressure guess in kgf/cm2.
/// Returns outlet mass flow minus the choke capacity, or a march failure sentinel.
[[nodiscard]] double marchProductionSteadySecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pressureGuess);

/// Marches between fixed pressures from a standard source flow guess in m3/day.
/// Returns corrected outlet pressure minus surface gas pressure, or a march failure sentinel.
[[nodiscard]] double marchProductionPressureToPressure(const SteadyStateState &state, const SteadyDependencies &dependencies, double massFlowRateGuess);

/// Marches in reverse between fixed pressures from a standard source flow guess in m3/day.
/// Returns corrected outlet pressure minus surface gas pressure, or a march failure sentinel.
[[nodiscard]] double marchReverseProductionPressureToPressure(const SteadyStateState &state, const SteadyDependencies &dependencies, double massFlowRateGuess);

/// Marches between fixed pressures with a surface choke and a standard source flow guess in m3/day.
/// Returns choke capacity minus outlet mass flow, or a march failure sentinel.
[[nodiscard]] double marchProductionPressureToPressureSecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double massFlowRateGuess);

/// Marches between fixed pressures with a closed outlet and a standard source flow guess in m3/day.
/// Returns the negative outlet mass flow, or a march failure sentinel.
[[nodiscard]] double marchProductionPressureToPressureTertiary(const SteadyStateState &state, const SteadyDependencies &dependencies, double massFlowRateGuess);

/// Marches the gas line and returns the relative outlet-pressure residual.
/// A nonnegative massGuess is in kg/s; a negative value derives the flow from the gas-lift valves.
/// Updates gas-line cells and may return a march failure sentinel.
double marchGasSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double massGuess = -1);

/// Marches the gas line from a pressure guess in kgf/cm2 and returns the injection mass imbalance.
/// A nonnegative standardFlowRateGuess is in m3/day; a negative value uses the configured gas rate.
[[nodiscard]] double marchGasSteadySecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pressureGuess,
                                             double standardFlowRateGuess = -1);

/// Marches the gas line from a pressure guess in kgf/cm2 with an active injection choke.
/// Returns the difference between valve flow and choke flow, or a march failure sentinel.
[[nodiscard]] double marchGasSteadyTertiary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pressureGuess);

/// Marches the injection column from the boundary-condition guess and returns its residual.
[[nodiscard]] double marchInjectionSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double guess);

/// Estimates bottom-hole pressure by a reverse hydrostatic march with the supplied holdup.
/// Optional liquid and gas rates are standard volumetric rates in m3/day.
[[nodiscard]] double reverseHydrostatic(const SteadyStateState &state, double liquidHoldup,
                                        double liquidFlowRate = 0, double gasFlowRate = 0);

/// Estimates injection bottom-hole pressure by a reverse hydrostatic march.
/// The optional liquid rate is a standard volumetric rate in m3/day.
[[nodiscard]] double reverseInjectionHydrostatic(const SteadyStateState &state, double liquidHoldup,
                                                 double liquidFlowRate = 0);

/// Returns the hydrostatic pressure estimate for a secondary branch with the supplied quality.
[[nodiscard]] double secondaryBranchHydrostatic(const SteadyStateState &state, double quality);

/// Initializes gas-line pressures and temperatures from the prescribed injection conditions.
void gasLineHydrostatic(const SteadyStateState &state);

/// Restricts the pseudo-transient time step using the steady cell velocities.
void computePseudoTransientTimeStep(const SteadyStateState &state);

/// Refreshes production-cell fluid properties for a steady iteration.
void refreshProperties(const SteadyStateState &state);

/// Updates the velocities used by the steady thermal balances.
void refreshSteadyThermalVelocities(const SteadyStateState &state);

/// Publishes the upstream pressure and temperature of a production cell.
void refreshUpstreamProductionPeriphery(const SteadyStateState &state, int cellIndex);

/// Publishes the downstream pressure and temperature of a production cell.
void refreshDownstreamProductionPeriphery(const SteadyStateState &state, int cellIndex);

}  // namespace sisprod::steady

#endif  // SISPRODSTEADYSTATE_H_
