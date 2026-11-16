#ifndef SISPRODTHERMALINTERNAL_H_
#define SISPRODTHERMALINTERNAL_H_

#include "SisProdThermal.h"

class ProFlu;
template <typename T>
class Vcr;

namespace sisprod::thermal {
namespace {

/// Fluid properties and transport coefficients held fixed during one steady cell advance.
struct SteadyTemperatureStep {
    double meanPressure;
    double meanVoidFraction;
    double betmed;
    double liquidDensity;
    double gasDensity;
    double liquidSpecificHeat;
    double gasSpecificHeat;
    double meanSuperficialGasVelocity;
    double meanSuperficialLiquidVelocity;
    double meanCellLength;
    double meanTemperatureGradientCorrection;
    double temperatureSpatialCoefficient;
    double annulusResistance;
    double mixtureFluxSign;
    double sourceTemperatureGradient;
};

struct SourceEnthalpy {
    double gasEnthalpy;
    double liquidEnthalpy;
    /// Complementary-fluid enthalpy approximated as cp(P, T) * T.
    double hcF;
};
struct CellFlowBasis {
    double flowArea;
    double voidFraction;
    double betmed;
    double gasSuperficialVelocity;
    double liquidSuperficialVelocity;
};

/// Phase heat capacities and advection coefficients evaluated without advancing the wall.
struct TemperatureTransport {
    CellFlowBasis flow;
    double liquidDensity;
    double liquidSpecificHeat;
    double gasSpecificHeat;
    double timeCoefficient;
    double spatialCoefficient;
};

struct TemperatureBalance {
    double cellLength;
    double meanCellLength;
    double flowArea;
    double voidFraction;
    double bet;
    double gasSuperficialVelocity;
    double liquidSuperficialVelocity;
    double referenceMixtureVelocity;
    double rp;
    double rc;
    double liquidDensity;
    double hydrostaticPower;
    double heatFlux;
    double timeCoefficient;
    double pressureTimeCoefficient;
    double temperatureSpatialCoefficient;
    double pressureSpatialCoefficient;
};

struct TemperatureSourceTerms {
    double gas;
    double liquid;
    /// Positive mass-inflow heat-capacity rate per unit length.
    double heatCapacityRate = 0.;
};

/// Water cuts evaluated while copying the two liquid-flow donors.
struct MassTransferWaterCuts {
    double leftFaceWaterCut;
    double leftCellLeftFaceWaterCut;
    double leftCellWaterCut;
};

struct DistributedMassTransferProperties {
    double leftFaceWaterCut;
    double leftCellLeftFaceWaterCut;
    double leftCellWaterCut;
    double liquidDensity;
    double gasDensity;
    double leftFaceComposition;
    double leftCellLeftFaceComposition;
    double mixtureLiquidDensity;
    double leftFaceOilVolumeFactor;
    double leftFaceSolutionGasRatio;
    double leftFaceSolutionGasPressureDerivative;
    double leftCellOilVolumeFactor;
    double leftCellSolutionGasRatio;
    double leftCellSolutionGasPressureDerivative;
    double leftCellSolutionGasTemperatureDerivative;
};
/// Mixture liquid density and solution-gas state carried from the previous face.
struct MassTransferFaceState {
    double liquidDensity;
    double oilVolumeFactor;
    double solutionGasRatio;
    double solutionGasPressureDerivative;
};

struct DistributedMassTransferCoefficients {
    double activeDerivative;
    double spatialCoupling;
    double flowArea;
};
struct UpstreamFaceBasis {
    double gasDensity;
    double flowArea;
};

/// Inlet closure policy for the instantaneous pass.
struct InletBoundaryRegime {
    static constexpr bool recordsInterfaceFraction = true;
    /// Applies the two ordered inlet boundary overrides.
    static void applyInletOverrides(const ThermalState &state, int cellIndex,
                                    int &branchFlag, double superficialGasVelocity,
                                    double superficialLiquidVelocity);

    /// Evaluates and stores the instantaneous inlet closure.
    static void updateClosure(const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double &c0,
                              double &ud);
};

/// Inlet closure policy for the buffered pass.
struct BufferedInletRegime {
    static constexpr bool recordsInterfaceFraction = false;
    /// Leaves the buffered branch flag unchanged.
    static void applyInletOverrides(const ThermalState &, int, int &, double, double);

    /// Evaluates the buffered inlet closure without storing it in the cell.
    static void updateClosure(const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double &c0,
                              double &ud);
};

/// Bisects an increasing table axis and returns the lower bracketing index.
/// The probe offset is 0 for pressure and -1 for temperature; equality and
/// endpoint tests select the table interval, and an unsuccessful search returns 0.
template <typename Axis>
[[nodiscard]] int bracketingIndex(double value, int divisionCount, Axis &&axis,
                                  int descendProbeOffset);

/// Evaluates the enthalpies carried by the cell accessory or its upstream accessory.
/// Property evaluations use the corresponding source fluid and pressure.
[[nodiscard]] SourceEnthalpy sourceEnthalpyOf(const ThermalState &state, int cellIndex);

/// Tests whether the accessory requires extrapolating the downstream pressure.
[[nodiscard]] bool accessoryImposesPressureJump(const ThermalState &state,
                                                int cellIndex);

/// Selects this cell or its downstream neighbour as the flow donor.
/// Phase presence and flow area always belong to this cell.
[[nodiscard]] CellFlowBasis cellFlowBasisOf(const ThermalState &state, int cellIndex);

/// Evaluates phase heat capacities and transport coefficients at the stored cell temperature.
/// Uses previous pressure and current flow data; does not update heat-transfer state.
[[nodiscard]] TemperatureTransport temperatureTransportOf(const ThermalState &state,
                                                          int cellIndex);

/// Prepares heat-transfer state and energy coefficients using previous pressure
/// and current temperature, reusing transport properties at that temperature.
/// Advances the wall heat-transfer state once and publishes the mean cell heat flux.
TemperatureBalance prepareTemperatureBalance(const ThermalState &state,
                                             int cellIndex,
                                             int steadyStateMode,
                                             const TemperatureTransport &transport);

void computeCellTemperature(const ThermalState &state, int cellIndex, int steadyStateMode);

/// Computes the transient kinetic-energy term where both cells have no accessory.
/// Also replaces the balance velocities with downstream face velocities for the
/// subsequent interfacial-work calculation. Each phase uses its own flow sign
/// to select its face fraction, including the saved liquid velocity. At cell
/// zero, incoming phases use the inlet face rather than a left control volume.
double computeKineticTemperatureTerm(const ThermalState &state, int cellIndex,
                                     TemperatureBalance &balance);

/// Sets inert source properties: zero specific heats and unit adiabatic ratio.
void clearSourceSpecificHeats(double &sourceGasSpecificHeat,
                              double &sourceSpecificHeatRatio,
                              double &sourceLiquidSpecificHeat);

/// Computes incoming enthalpy differences, linearized with cp at previous
/// pressure and the source-specific temperature and accessory rules. Withdrawals
/// carry the local fluid enthalpy and do not add a temperature relaxation term.
TemperatureSourceTerms computeTemperatureSourceTerms(const ThermalState &state,
                                                     int cellIndex,
                                                     double cellLength);

/// Computes source heating for thermal mass transfer at current pressure.
/// The volumetric-pump path retains a zero source specific-heat ratio.
TemperatureSourceTerms computeThermalMassTransferSourceTerms(
    const ThermalState &state, int cellIndex, double cellLength);

/// Refreshes inlet sources, saves their previous face values, and seeds the
/// liquid density and solution-gas properties for the distributed sweep.
MassTransferFaceState initializeDistributedMassTransferInlet(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex);

/// Selects liquid-flow donors for a face with cellIndex >= 1 and prepares
/// water cuts, densities, and pressure/temperature solution-gas derivatives.
/// Compositional pressure perturbation updates leftFaceFluid in place.
DistributedMassTransferProperties prepareDistributedMassTransferProperties(
    const ThermalState &state, int cellIndex, double meanTemperature, ProFlu &leftCellLeftFaceFluid,
    ProFlu &leftFaceFluid);

/// Evaluates the last cell pressure derivative using its fluid and, when needed,
/// a compositional copy. Fluid-property calls and copy lifetime are significant.
[[nodiscard]] double lastCellSolutionGasPressureDerivative(const ThermalState &state,
                                                           int cellIndex);

/// Publishes pressure and temperature derivatives on both sides of a face.
/// At the last cell the thermal path also evaluates the pressure derivative,
/// then uses the supplied temperature derivative in DTransDtT.
DistributedMassTransferCoefficients updateDistributedMassTransferDerivatives(
    const ThermalState &state, int cellIndex, double leftCellWaterCut, const ProFlu &leftFaceFluid,
    double leftCellSolutionGasPressureDerivative, double leftCellSolutionGasTemperatureDerivative);

/// Applies ordered model overrides for the upstream cell and clears the
/// last cell phase-change source.
void selectDistributedMassTransferModel(
    const ThermalState &state, int cellIndex, double leftAbsoluteSuperficialVelocity);

/// Saves face mass-transfer terms and derivatives in their previous-iteration fields.
void snapshotMassTransferState(const ThermalState &state, int cellIndex);

/// Selects and applies the upstream cell phase-change model with its
/// model-specific derivative resets and previous-face dependencies.
void applyDistributedMassTransferModel(
    const ThermalState &state, int cellIndex, double leftAbsoluteSuperficialVelocity, const ProFlu &leftCellLeftFaceFluid, const ProFlu &leftFaceFluid,
    const DistributedMassTransferProperties &properties,
    const DistributedMassTransferCoefficients &coefficients, const MassTransferFaceState &previousFace);

/// Zeros face mass-transfer derivatives. Temperature derivatives are reset
/// only when their disabling flag is active; the mass source is left to the caller.
void clearMassTransferDerivatives(const ThermalState &state, int cellIndex);

/// Selects upstream gas density and area by the sign of the gas flow rate.
[[nodiscard]] UpstreamFaceBasis upstreamFaceBasisOf(const ThermalState &state, int cellIndex);

/// Sets the liquid partition, zeros its drift term, and assigns the branch flag.
void setFlowPartitionTerms(const ThermalState &state, int cellIndex, double term1,
                           int &branchFlag, int flagValue, bool recordInterfaceFraction = false);

/// Sets unit distribution, zero drift velocity, and no flow-pattern label.
void clearDriftClosure(const ThermalState &state, int cellIndex);

/// Classifies an interior face, applies ordered regime overrides, then computes
/// the closure and partition terms for the surviving branch.
void selectAndApplyInteriorFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, double superficialGasVelocity,
    double superficialLiquidVelocity, double leftSuperficialGasVelocity, double leftSuperficialLiquidVelocity, double rightSuperficialGasVelocity, double rightSuperficialLiquidVelocity,
    double gasDensity, double liquidDensity, double flowArea, int explicitMode = 0);

/// Prepares both face properties, propagates left-face state, and applies
/// the interior regime unless the valve flag blocks it.
void updateInteriorFlowPartitionCell(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, Vcr<int> &valv, int explicitMode = 0);

/// Applies outlet-specific regime rules and propagates its closure to the left face.
void updateOutletBoundaryFlowPartition(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, int explicitMode = 0);

/// Blends closure values into the supplied buffers according to inclination
/// and flow-pattern differences; blendIndex identifies the destination entry.
void blendNeighbourDriftClosure(const ThermalState &state, int cellIndex,
                                int neighbourIndex, int blendIndex,
                                const Vcr<int> &bif,
                                Vcr<double> &c0V, Vcr<double> &udV);

/// Prepares neighbour closure buffers, corrects flow signs, and publishes
/// mixture partition terms using the cell closure state.
void finalizeFlowPartitionTerms(
    const ThermalState &state, const Vcr<int> &bif, const Vcr<int> &valv);

/// Applies the no-slip partition while retaining zero products with the supplied
/// closure values, including their non-finite floating-point behavior.
void applyNoSlipClosure(const ThermalState &state, int cellIndex, double term1,
                        int &branchFlag, double distributionCoefficient,
                        double driftVelocity, bool recordInterfaceFraction = false);

/// Classifies the inlet, applies the selected inlet policy, and computes
/// the partition after all branch-flag overrides.
template <typename Regime>
void selectAndApplyInletRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, int &branchFlag, double distributionCoefficient,
    double driftVelocity, double superficialGasVelocity, double superficialLiquidVelocity, double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea, int explicitMode = 0);

/// Applies the instantaneous inlet policy using the cell branch-flag entry.
void selectAndApplyInletBoundaryFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, double distributionCoefficient,
    double driftVelocity, double superficialGasVelocity, double superficialLiquidVelocity, double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea, int explicitMode = 0);

/// Applies the buffered outlet rules, closure, and final choke override.
void selectAndApplyBufferedOutletFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double distributionCoefficient, double driftVelocity,
    double superficialGasVelocity, double superficialLiquidVelocity, double leftSuperficialGasVelocity, double leftSuperficialLiquidVelocity, double rightSuperficialGasVelocity,
    double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity,
    double flowArea);

/// Applies the buffered inlet policy with a local branch flag.
void selectAndApplyBufferedInletFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double distributionCoefficient, double driftVelocity,
    double superficialGasVelocity, double superficialLiquidVelocity, double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea);

/// Prepares Poisson-2D cells and the remaining column using one outer team.
/// A single 2D wall runs on the caller so its solver can distribute its own mesh.
void refreshTemperatureField(const ThermalState &state);

/// Computes steady source heating from the left cell accessory.
TemperatureSourceTerms computeSteadySourceTerms(const ThermalState &state,
                                                int cellIndex,
                                                double meanPressure,
                                                double meanTemperature,
                                                double cellLength);

/// Prepares the left cell external thermal state and returns annulus wall
/// resistance, including casing and formation on the initial steady iteration.
double applySteadyAnnulusCoupling(const ThermalState &state, int cellIndex,
                                  double interfaceMeanPressure,
                                  double interfaceMeanTemperature,
                                  double gasSpecificHeat,
                                  double gasDensity);

/// Computes the direct steady kinetic term with its phase and cell-length guards.
double computeSteadyKineticTerm(const ThermalState &state, int cellIndex,
                                double meanSuperficialGasVelocity,
                                double meanSuperficialLiquidVelocity);

/// Sanitizes and limits the left cell phase-change source, clears it at
/// pressure-jump accessories, and evaluates its latent-heat contribution.
double computeSteadyLatentHeatTerm(const ThermalState &state, int cellIndex,
                                   double meanPressure, double meanTemperature);

/// Computes reverse steady source heating from the right cell accessory.
TemperatureSourceTerms computeReverseSteadySourceTerms(
    const ThermalState &state, int cellIndex, double meanPressure,
    double meanTemperature, double cellLength);

/// Computes the reverse kinetic term using absolute phase velocities.
double computeReverseSteadyKineticTerm(
    const ThermalState &state, int cellIndex,
    double meanSuperficialGasVelocity,
    double meanSuperficialLiquidVelocity);

/// Evaluates reverse latent heat from the right cell source without applying
/// the direct march source sanitation or mass-transfer limit.
double computeReverseSteadyLatentHeatTerm(const ThermalState &state, int cellIndex,
                                          double meanPressure, double meanTemperature);

/// Prepares the right cell external thermal state and returns annulus wall
/// resistance using reverse-march coupling bounds.
double applyReverseSteadyAnnulusCoupling(
    const ThermalState &state, int cellIndex,
    double interfaceMeanPressure, double interfaceMeanTemperature,
    double gasSpecificHeat, double gasDensity);

/// Evaluates source enthalpy for a non-null upstream accessory, including
/// choke expansion and volumetric-pump heating.
[[nodiscard]] SourceEnthalpy upstreamSourceEnthalpyOf(const ThermalState &state, int cellIndex);

/// Returns the first configured 3D thermal coupling for a cell, or -1.
/// The linear search preserves the order of repeated cell entries.
int thermalCouplingIndexOf(const ThermalState &state, int cellIndex);

/// Prepares the inlet face and partition terms for cellIndex == 0.
/// The inlet void fraction uses c0 on entry, before the closure callback updates it.
void updateInletBoundaryFlowPartition(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bifurcationFlags, int explicitMode = 0);

/// Evaluates transient, steady, or 3D-coupled heat flux after the cell heat-transfer
/// state is prepared. Unsupported diffusion modes retain zero heat flux.
double temperatureHeatFluxOf(const ThermalState &state, int cellIndex, int steadyStateMode);

/// Computes the temperature-balance pressure gradient with ordered boundary
/// and pressure-jump accessory overrides.
double temperaturePressureGradientOf(const ThermalState &state, int cellIndex,
                                     const TemperatureBalance &balance);

/// Computes the upwind temperature gradient and the subsequent volumetric-pump
/// pressure override. Negative transport uses the saved right temperature,
/// including at cells zero and one. Choke and outlet rules take precedence.
double temperatureSpatialGradientOf(const ThermalState &state, int cellIndex,
                                    const TemperatureBalance &balance, double &pressureGradient);

/// Limits the phase-change mass rate locally and evaluates the temperature
/// balance latent heat using previous pressure and the configured latent switches.
double computeTemperatureLatentHeatTerm(const ThermalState &state, int cellIndex);

/// Computes thermal mass-transfer kinetic work using the left cell face velocities.
/// This calculation uses current-pressure mass-transfer data.
double computeThermalMassTransferKineticTerm(
    const ThermalState &state, int cellIndex, double meanSuperficialGasVelocity,
    double meanSuperficialLiquidVelocity);

/// Advances the direct march over thermally stable substeps, recalculating
/// heat flow and the leftCell external annulus temperature at each step.
/// Publishes the cell temperature and average wall heat flux in evaluation order.
void advanceSteadyTemperatureSubsteps(const ThermalState &state, int cellIndex,
                                      const SteadyTemperatureStep &step, double heatFluxTemperatureGradient);

/// Advances the reverse march over thermally stable substeps, recalculating
/// heat flow and the rightCell external annulus temperature at each step.
/// Publishes the cell temperature and average wall heat flux in evaluation order.
void advanceReverseSteadyTemperatureSubsteps(const ThermalState &state, int cellIndex,
                                             const SteadyTemperatureStep &step, double heatFluxTemperatureGradient);

/// Applies outlet backflow rules before advancing thermal state.
/// The two boundary modes retain their distinct mixture mass-flow assignments.
/// A prohibited phase has zero mass and volume flow before the energy balance.
void prepareTransientOutletFlow(const ThermalState &state);

/// Prepares coupled-cell interface data in parallel, advances the 3D Poisson
/// thermal solver when the cycle requires it, and refreshes the temperature field.
void advanceCoupledThermalField(const ThermalState &state, int cycle, int maximumCycle);

/// Copies the two donors selected by liquid-flow direction and evaluates their
/// water cuts and the upstream cell water cut in property-call order.
MassTransferWaterCuts prepareMassTransferDonors(
    const ThermalState &state, int cellIndex, bool leftCellHasNegativeLiquidMassFlowRate,
    ProFlu &leftCellLeftFaceFluid, ProFlu &leftFaceFluid);

/// Updates model-0 spatial and time derivatives and its corrected left mass source.
/// Model-1 and hydrate derivative resets are handled by their own branches.
void updateEquilibriumMassTransferDerivatives(
    const ThermalState &state, int cellIndex,
    const ProFlu &leftCellLeftFaceFluid, const ProFlu &leftFaceFluid,
    const DistributedMassTransferProperties &properties,
    const DistributedMassTransferCoefficients &coefficients, const MassTransferFaceState &previousFace);

/// Refreshes sources, prepares one face, updates derivatives and model terms,
/// then publishes the state carried to the next face before fluid copies are destroyed.
void updateDistributedMassTransferCell(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, MassTransferFaceState &previousFace);

/// Applies the hydrate model, disabled-model reset, and ordered phase-availability
/// limits without sharing reset sets between distinct models.
void finalizeDistributedMassTransferFace(
    const ThermalState &state, int cellIndex, const DistributedMassTransferProperties &properties,
    const DistributedMassTransferCoefficients &coefficients);

/// Evaluates the interior drift closure after regime selection and applies
/// its velocity cap and outlet-only partition corrections in their evaluation order.
void applyInteriorDriftPartition(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double superficialGasVelocity,
    double superficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea, int explicitMode = 0);

/// Corrects one interior partition using cell closure values. Candidate phase
/// masses are evaluated before drift and shut-in corrections and retain that
/// time level throughout the ordered tests.
void finalizeFlowPartitionCell(
    const ThermalState &state, int cellIndex, int isShutIn,
    const Vcr<int> &bif, const Vcr<int> &valv);

} // namespace
} // namespace sisprod::thermal

#endif // SISPRODTHERMALINTERNAL_H_
