#ifndef SISPRODGASLIFTINTERNAL_H_
#define SISPRODGASLIFTINTERNAL_H_

#include "SisProdConstants.h"
#include "SisProdGasLift.h"

/// What only the gas-lift module uses: its constants and types, its helpers, and the routines no other module
/// calls. SisProdGasLift.h declares the module's interface.
namespace sisprod::gaslift {

/// The interface ratio at the middle of a cell: once the interface is past it, the cell counts as on the gas side.
inline constexpr double kMidCellInterfaceRatio = 0.5;

/// What a gas-lift valve is primed for: the fluid whose recovery factor turns the stagnation pressure into the
/// throat pressure, and the fluid whose mass flow the choke passes.
struct ValveFluids {
    InjectedFluid recovery;
    InjectedFluid flow;
};

/// Gas through the valve.
inline constexpr ValveFluids kGasThroughValve{kInjectedGas, kInjectedGas};
/// Completion fluid through the valve.
inline constexpr ValveFluids kCompletionFluidThroughValve{kInjectedCompletionFluid, kInjectedCompletionFluid};
/// The unloading search's combination: the gas recovery factor with the completion-fluid mass flow.
inline constexpr ValveFluids kUnloadingThroughValve{kInjectedGas, kInjectedCompletionFluid};

/// The transport properties of a fluid as a cell's heat transfer reads them: the fluid outside a production cell, or
/// the gas inside a gas cell.
struct FluidProperties {
    double conductivity;
    double specificHeat;
    double density;
    double viscosityCentipoise;
};

// Unloading the completion fluid through the gas-lift valves.

/// The unloading controller corrects the injection or the separator pressure when the completion-fluid flow
/// through the selected valve leaves the band from 1 - kUnloadingLowerBandMargin to 1 - kUnloadingUpperBandMargin
/// of the controlled flow rate.
inline constexpr double kUnloadingUpperBandMargin = 0.1;
inline constexpr double kUnloadingLowerBandMargin = 0.4;

/// The two edges of that band, as fractions of the controlled flow rate.
inline constexpr double kUnloadingUpperBandEdge = 1 - kUnloadingUpperBandMargin;
inline constexpr double kUnloadingLowerBandEdge = 1 - kUnloadingLowerBandMargin;

/// The largest pressure correction of one step, as a fraction of the corrected pressure per second of the step.
inline constexpr double kMaximumUnloadingCorrectionPerSecond = 0.01;

/// The separator pressure counts as at the unloading's maximum or minimum within these factors.
inline constexpr double kReachedMaximumPressureFactor = 0.9999999;
inline constexpr double kReachedMinimumPressureFactor = 1.0000001;

/// presMaxDesc before the minimum over the IPR accessories: above any pressure the column reaches, in kgf/cm^2.
inline constexpr double kUnboundedUnloadingPressure = 100000.;

/// The pressure the unloading hydrostatics start the gas line from when nothing else gives it, in kgf/cm^2.
inline constexpr double kDefaultUnloadingStartPressure = 10.;

/// The weight of the step's own flow against the moving average, in the flow the unloading controller corrects.
inline constexpr double kCurrentFlowWeight = 0.5;

/// The fraction of the separator pressure the unloading takes off per second while the column's last cell but
/// one holds no completion fluid.
inline constexpr double kSeparatorPressureDecayPerSecond = 0.01;

/// Before the unloading control starts, the separator pressure is brought down to the unloading's minimum by this
/// fraction of the latency, and held there after it.
inline constexpr double kSeparatorRampLatencyFraction = 0.5;

/// The factors a pressure is relaxed by per step towards its target, after the unloading.
inline constexpr double kRelaxationDownFactor = 0.95;
inline constexpr double kRelaxationUpFactor = 1.05;

/// The ratio of its cell the interface must reach in the last cell but one before it is handed over to the end of
/// the gas line.
inline constexpr double kInterfaceHandoverRatio = 0.99;

/// At or below this mixture velocity, in m/s, the unloading column counts as still and takes no wall friction.
inline constexpr double kStillMixtureVelocity = 1e-15;

/// The valve the unloading control follows: the largest completion-fluid volume flow among the valves behind the
/// interface, and the valve's index.
struct UnloadingValve {
    double flowRate;
    int index;
};

/// Primes each valve behind the interface for the unloading and returns the one with the largest completion-fluid
/// volume flow; zero and the first valve when none flows.
[[nodiscard]] UnloadingValve selectUnloadingValve(const GasLiftState &state);

/// Adds a step's largest unloading flow to the unloading control's moving average, after dropping the oldest sample
/// when the window holds more samples than its capacity or spans more time than the averaging window.
void recordUnloadingSample(const GasLiftState &state, double maximumFlowRate);

/// Marches the mixture pressure from the separator down the production column and sets presMaxDesc, the highest
/// separator pressure the unloading allows: the smallest, over the IPR accessories, of the reservoir pressure less the
/// column's pressure difference from the separator down to the accessory.
void computeMaximumSeparatorPressure(const GasLiftState &state);

/// Moves the injection pressure towards the unloading flow band through the selected valve, within the unloading's
/// injection pressures: down when the flow is above the band with the separator pressure at its maximum and gas
/// entering the gas line, up when the flow is below the band with the separator pressure at its minimum.
void correctInjectionPressure(const GasLiftState &state, double flowRate, int valveIndex);

/// Corrects the separator pressure towards the unloading flow band through the selected valve, within the
/// unloading's pressures, while the column's last cell but one holds completion fluid; once it holds none, lets the
/// separator pressure decay towards the unloading's minimum.
void correctSeparatorPressure(const GasLiftState &state, double flowRate, int valveIndex);

/// The pressure correction the unloading control applies for one valve, in kgf/cm^2 and with the given sign: the
/// dynamic pressure, at the valve's throat, of the completion-fluid flow that separates maximumFlowRate from factor
/// times the controlled unloading flow rate. Primes the valve's choke with its gas cell's pressure and temperature.
[[nodiscard]] double unloadingPressureCorrection(const GasLiftState &state, double maximumFlowRate, int valveIndex,
                                                 double factor, int sign);

/// Solves one unloading step from the interface cell to the end of the gas line: marches the pressure through the
/// friction and the head of each half cell's gas and completion-fluid parts, updates the discharge temperatures, and
/// sets the flows that carry the valves' sources along the line and the interface velocity.
void solveUnloading(const GasLiftState &state, const thermal::ThermalState &dependencies);

/// Advances the unloading interface over the time step. When it crosses a face of its cell, the interface time step
/// is shortened to the crossing and the interface moves to the next or the previous cell; at the end of the gas line
/// it is handed over and the unloading ends (interfaceCell becomes kNoInterfaceCell, input.descarga 0).
void advanceInterface(const GasLiftState &state);

/// Hands the interface over to the end of the gas line, once it is far enough into the last cell but one: that cell
/// and the last one take the gas density, their faces the values that go with it, both ratios 1, and the unloading
/// ends (interfaceCell becomes kNoInterfaceCell, input.descarga 0).
void handOverInterface(const GasLiftState &state);

/// The pressure the unloading hydrostatics start the gas line from: the unloading's own initial pressure when the
/// unloading is controlled; the configured injection pressure when it is not and the gas inlet is under a pressure
/// condition; kDefaultUnloadingStartPressure otherwise.
[[nodiscard]] double unloadingStartPressure(const GasLiftState &state);

/// The hydrostatic head across a half cell of the unloading's initial state, in kgf/cm^2.
[[nodiscard]] double unloadingHalfCellHead(double density, double halfLength, double inclination);

/// Clears a production cell's mass sources.
void clearMassSources(Cel &cell);

/// Gives a production cell the phases of the unloading's initial state, completion fluid below the interface and
/// gas above it, with the copies the march and the pigs start from.
void setUnloadingPhase(Cel &cell, bool belowInterface);

/// Copies a production cell's phases to the face values its neighbours hold of it.
void copyPhaseToLeftNeighbour(const Cel &cell, Cel &left);
void copyPhaseToRightNeighbour(const Cel &cell, Cel &right);

/// Walks a pressure towards a target by kRelaxationDownFactor or kRelaxationUpFactor per step, without overshooting
/// it. The two factors are not each other's inverse.
void relaxTowards(double &pressure, double target);

/// A pressure correction of the unloading control, limited to kMaximumUnloadingCorrectionPerSecond of the pressure
/// it corrects per second of the step, with its sign.
[[nodiscard]] double limitedCorrection(double correction, double pressure, double timeStep);

/// The volume flow of a gas cell's source during the unloading: gas once the
/// interface is past the cell's middle, completion fluid before.
[[nodiscard]] double sourceVolumeFlowRate(const CelG &cell, double liquidDensity, double gasDensity);

// The gas line in the transient.

/// Below this opening the injection choke is throttling and becomes the mass
/// source of the first cell instead of a free boundary.
inline constexpr double kThrottlingChokeOpening = 0.2;

/// The temperature of the gas line's last cell while the flow there runs backwards, in degrees Celsius.
inline constexpr double kReverseFlowOutletTemperatureCelsius = 20.;

/// The gas line's master2 valve throttles while its throat area lies between these fractions of the smaller pipe
/// area around it.
inline constexpr double kMaster2ThrottlingAreaFraction = 0.01;
inline constexpr double kMaster2ClosedAreaFraction = 1e-5;

/// Clears the gas cells' sources from the interface cell on, primes each gas-lift valve's choke and sets the mass
/// source it gives its gas cell (completion fluid behind the interface, gas ahead of it), then the sources of the
/// master2 valve's two cells, which only flow while it throttles.
void updateTransientGasValves(const GasLiftState &state);

/// The mass sources the gas line's master2 valve gives its cell and the next one while it throttles, from the
/// pressure difference across it; none otherwise.
void updateMaster2Source(const GasLiftState &state);

/// Opens the injection choke if it is throttling, assembles the band system from every gas cell and solves it.
void assembleAndSolveGasSystem(const GasLiftState &state);

/// Copies the solved pressure-flow system into the gas cells: each cell's pressure and right-face mass flow, and the
/// values its neighbours' faces hold of them.
void updateGasLine(const GasLiftState &state);

/// The gas temperatures of one thermal coupling cycle: the injection temperature at the inlet, the reverse-flow
/// temperature at the outlet while the flow there runs backwards, and each cell before gasLoopEnd from its left
/// neighbour's previous temperature; with the time derivative of each temperature it sets.
void updateGasTemperatures(const GasLiftState &state, const thermal::ThermalState &dependencies, int gasLoopEnd);

/// The gas density of each cell before gasLoopEnd, at its pressure and temperature, and its density times flow area,
/// with the face values its neighbours hold of both.
void updateGasDensities(const GasLiftState &state, int gasLoopEnd);

/// The opening the injection choke presents to the gas line. Under a pressure
/// condition it is the choke's own, and a choke throttling the line also sets
/// the inlet mass source through it; under a flow-rate condition the source is
/// given and the choke counts as open.
[[nodiscard]] double injectionChokeOpening(const GasLiftState &state, InjectionPressureCondition);
[[nodiscard]] double injectionChokeOpening(const GasLiftState &state, InjectionFlowRateCondition);

/// Copies a gas-lift valve's choke state onto its gas cell: the stagnation and throat pressures and temperatures, the
/// throat flow and the throat area.
void recordValveState(CelG &gasCell, const ChokeGas &choke);

// The gas line in the steady state.

/// Fixed gas viscosity used to prime the steady annulus connection, in cP.
inline constexpr double kInitialAnnulusGasViscosityCentipoise = 0.16;

/// The steady gas temperature marches a cell only above this superficial gas velocity, in m/s, and between
/// diameters within this ratio of each other; elsewhere the cell takes the external temperature.
inline constexpr double kMinimumMarchGasVelocity = 1e-3;
inline constexpr double kMinimumMarchDiameterRatio = 0.5;

/// The steady gas temperature marches a cell in substeps when its length over its global thermal resistance
/// passes this value.
inline constexpr double kThermalSubstepLength = 1000.;

/// The steady gas temperature of a cell outside the column-annulus coupling, marched from its left neighbour.
void marchSteadyGasTemperature(const GasLiftState &state, int cellIndex);

/// The steady gas temperature of a cell inside the column-annulus coupling, and the properties its heat
/// transfer is computed with.
void coupledSteadyGasTemperature(const GasLiftState &state, int cellIndex);

/// One valve's gas mass source at steady state. Under a flow-rate condition,
/// an equal share of the injection flow rate; under a pressure condition, what
/// the valve's choke passes from the casing pressure, if the casing is above
/// the production line.
void setSteadyValveGasMassSource(const GasLiftState &state, int valveIndex, int valveCount, double casingTemperature,
                                 InjectionFlowRateCondition);
void setSteadyValveGasMassSource(const GasLiftState &state, int valveIndex, int valveCount, double casingTemperature,
                                 InjectionPressureCondition);

/// Clamps a gas cell's temperature to the physical range and copies it to the
/// neighbours that face it.
void clampAndPropagateTemperature(const GasLiftState &state, int cellIndex);

/// Gives a gas cell's heat transfer the gas flowing inside it, at this temperature and superficial velocity, and
/// computes the cell's heat flux.
void publishInternalGas(CelG &cell, double temperature, double velocity, const FluidProperties &gas);

/// The flow area of a bore of this diameter.
[[nodiscard]] double boreArea(double diameter);

// What every phase uses: the gas-lift valves, the column-annulus coupling and the Reynolds diameter.

/// Gas-lift valve geometry. The bore threshold and the two spring rates are
/// imperial, as the whole valve correlation is.
inline constexpr double kLargeBoreValveInches = 1.1;
inline constexpr double kLargeBoreSpringRate = 500.0;
inline constexpr double kSmallBoreSpringRate = 1950.0;

/// The bellows of a calibrated valve: the temperature its dome charge is referred to, in degrees Fahrenheit, and
/// the fractional change of the dome pressure per degree Fahrenheit.
inline constexpr double kBellowsReferenceTemperatureFahrenheit = 80.;
inline constexpr double kBellowsPressureChangePerFahrenheit = 0.00215;

/// Primes the choke from the two cells it spans and returns the mass source it
/// delivers, already scaled by the calibrated opening when the valve is one.
template <ValveFluids fluids>
[[nodiscard]] double primeValveMassSource(const GasLiftState &state, int valveIndex, const CelG &gasCell,
                                          const Cel &productionCell);

/// The opening fraction of a calibrated gas-lift valve, from the state already primed on its choke, converted to
/// the units of the valve correlation.
[[nodiscard]] double calibratedValveOpening(const ChokeGas &choke);

/// The opening of a calibrated gas-lift valve as a fraction of its throat area, at most 1; a closed valve is not a
/// special case. The correlation is imperial: the calibration pressure, the valve opening pressure (the casing's) and
/// the tubing pressure are gauge pressures in psi, and the bottom-hole temperature is in degrees Fahrenheit; the
/// calibration temperature comes in degrees Celsius and the external diameter in metres. valveRatio is the throat
/// area over the bellows area.
[[nodiscard]] double calibratedValveOpeningFraction(double calibrationPressure, double calibrationTemperature,
                                                    double valveOpeningPressure, double tubingPressure,
                                                    double externalDiameter, double throatArea, double valveRatio,
                                                    double bottomHoleTemperatureFahrenheit);

/// The temperature of the gas a valve injects into its production cell: the valve's discharge temperature while the
/// valve flows, the cell's own otherwise, and never below kMinimumTemperatureCelsius.
[[nodiscard]] double injectedGasTemperature(const GasLiftState &state, const thermal::ThermalState &dependencies, int valveIndex, const Cel &productionCell);

/// Calls visit(productionCellIndex, gasCellIndex) for each pair of cells the column-annulus thermal coupling joins,
/// from tubingCouplingStart down to tubingCouplingEnd.
template <typename Visit>
void forEachCoupledCellPair(const GasLiftState &state, Visit &&visit);

/// The cell across the column-annulus coupling from this one: the gas cell that faces a production cell, or the
/// production cell that faces a gas cell.
[[nodiscard]] int facingCouplingCell(const GasLiftState &state, int cellIndex);

/// Publishes the transport properties of the gas line's fluid onto the production
/// cell that faces it, as that cell's external fluid.
void publishExternalFluid(Cel &productionCell, const FluidProperties &fluid);

/// The diameter a Reynolds number is formed on: the bore when the segment is
/// bare, the hydraulic diameter when it is cased.
[[nodiscard]] double characteristicDiameter(int cased, double bore, double area, double perimeter);

}  // namespace sisprod::gaslift

#endif  // SISPRODGASLIFTINTERNAL_H_
