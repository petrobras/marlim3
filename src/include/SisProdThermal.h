#ifndef SISPRODTHERMAL_H_
#define SISPRODTHERMAL_H_

#include <vector>

class Cel;
class CelG;
class ChokeGas;
class Ler;
class SProd;
namespace driftflux::coefficient { struct ClosureState; }
namespace sisprod::sources { struct SourceState; }

namespace sisprod::transient { struct TransientStepState; }
class choke;
class solverP3D;
class solverPAxiSim;
struct SimulationContext;

namespace sisprod::thermal {

/// Source and drift-closure views used by coupled thermal operations.
struct ThermalDependencies {
    const sources::SourceState &sources;
    const driftflux::coefficient::ClosureState &closure;
};

/// Live data read and updated by the thermal and phase-transfer steps.
struct ThermalState {
    Cel *const &cells;
    CelG *const &gasCells;
    const Ler &input;
    /// The latent heat table's row pointers -- SProd::tables.latentHeatTable.
    const std::vector<double *> &latentHeatTable;
    SimulationContext *const &globals;
    const bool &thermalSourceDisabled;
    const bool &productionNetworkCoupled;
    const int &primarySectionStart;
    const int &primarySectionEnd;
    const std::vector<int> &coupledCellIndices;
    solverP3D &poissonSolver3D;
    solverPAxiSim &axisymmetricSolver;
    const int &lastCell;
    const choke &surfaceChoke;
    const int &surfaceChokeMassFlag;
    const int &endNode;
    const double &gasSurfaceTemperature;
    const int &latentHeatEnabled;
    const int &fullModel;
    const int &massTransferModel;
    const double &inletPressure;
    const double &inletTemperature;
    const double &inletQuality;
    double &inletVoidFraction;
    const double &inletComplementaryFraction;
    const int &surfaceChokeOpen;
    const double &defaultInletTemperature;
    const double &timeStep;
    const double &minimumCycleTimeStep;
    const std::vector<int> &poisson2DCellIndices;
    int poisson2DCellCount;
    const int &steadyIteration;
    const double &slowHeatTransferThreshold;
    const int &annulusTubingStart;
    const int &annulusTubingEnd;
    const int &tubingAnnulusStart;
    const bool &productionNetworkHeatCoupled;
    const int &primaryNetworkSectionEnd;
    const int &primaryNetworkSectionStart;
    const int &lastGasCell;
    std::vector<ChokeGas> &gasLiftChokes;
    const double &gasSurfacePressure;
    const double &outletPressure;
    /// Written, not just read -- computeOutletTemperature assigns it.
    double &surfaceTemperature;
    const bool &networkCoupled;
    const int &tubingAnnulusEnd;
};

/// Builds an exclusive cell-to-boundary mapping for the 3D thermal interface.
/// Throws invalid_argument for inconsistent counts, labels, or cell indices.
std::vector<int> buildThermalCouplingIndices(const Ler &input, const solverP3D &solver, int lastCell);

/// Advances the axisymmetric well field from the production or annulus interface state.
/// Initializes its transient history at the first simulation step.
void advanceAxisymmetricThermalField(const ThermalState &state);

/// Restores and advances the axisymmetric field after a reduced time step.
void restartAxisymmetricThermalField(const ThermalState &state);

/// Accepts the axisymmetric thermal step and writes its configured output.
void finalizeAxisymmetricThermalField(const ThermalState &state, int branchIndex);

/// Interpolates latent heat in the pressure-temperature table.
double interpolateLatentHeat(const ThermalState &state, double pressure,
                             double temperature);

/// Computes the mixture enthalpy balance for one control volume.
double computeMixtureEnthalpy(const ThermalState &state, int cellIndex);

/// Interpolates mixture energy between two pressure rows of a property table.
double interpolateMixtureEnergy(const ThermalState &state, int cellIndex,
                                int pressureIndex, int temperatureIndex,
                                double pressureRatio);

/// Updates one control-volume temperature from tabulated mixture enthalpy.
void updateTemperatureFromEnthalpy(const ThermalState &state, int cellIndex);

/// Computes the thermal phase-change mass source for one cell.
void computeThermalMassTransfer(const ThermalState &state, int cellIndex);

/// Updates one control-volume temperature from the thermal energy balance.
/// previousTemperature does not participate in the calculation.
/// Uses previous pressure, current temperature, and prior face temperatures.
void computeTemperature(const ThermalState &state, int cellIndex,
                        double previousTemperature, int steadyStateMode = 0);

/// Renews distributed phase-change mass transfer along the production cells.
void updateDistributedMassTransfer(const ThermalState &state, const ThermalDependencies &dependencies);

/// Computes the mixture-flow partition terms along the production cells.
/// inflowMode does not affect the sweep, which uses the outlet boundary.
void updateFlowPartitionTerms(const ThermalState &state, const ThermalDependencies &dependencies, int inflowMode, int explicitMode = 0);

/// Computes mixture-flow partition terms at an internal-section outlet.
void updateOutletFlowPartitionTerms(const ThermalState &state, const ThermalDependencies &dependencies);

/// Computes mixture-flow partition terms at an internal-section inlet.
void updateInletFlowPartitionTerms(const ThermalState &state, const ThermalDependencies &dependencies);

/// Prepares the non-dimensional heat-diffusion properties for one cell.
/// Cell zero has no left control volume and uses a zero left temperature offset.
void prepareNonDimensionalHeatDiffusion(const ThermalState &state,
                                        int cellIndex);

/// Advances the transient thermal-energy solution by one coupling cycle.
void advanceTransientEnergy(const ThermalState &state, const transient::TransientStepState &transientStep, int cycle,
                            int maximumCycle);

/// Advances the steady-state temperature march in the direct direction.
void advanceSteadyTemperature(const ThermalState &state, int cellIndex,
                              int rungeKuttaStage);

/// Advances the reverse steady temperature march with its directional source
/// and coupling rules.
void advanceReverseSteadyTemperature(const ThermalState &state, int cellIndex,
                                     int rungeKuttaStage);

/// Solves the gas-line temperature for one cell and publishes face temperatures.
/// previousTemperature does not participate in the calculation.
void computeGasTemperature(const ThermalState &state, int cellIndex,
                           double previousTemperature, int steadyMode);

/// Temperature at the discharge of an accessory.
void computeDischargeTemperature(const ThermalState &state, int cellIndex);

/// Discharge temperature of a gas-lift valve.
double computeGasLiftDischargeTemperature(const ThermalState &state,
                                          int valveIndex);

/// Propagates the cell temperature to its neighbours' interface fields.
void updateProductionTemperaturePeriphery(const ThermalState &state,
                                          int cellIndex);

/// Returns temperature downstream of the surface choke from its Joule-Thomson pressure drop.
/// Returns the outlet cell temperature for a closed choke, nonpositive pressure drop, or zero flow.
double surfaceChokeOutletTemperature(const ThermalState &state);

/// Surface temperature at the end of the production line.
void computeOutletTemperature(const ThermalState &state);

} // namespace sisprod::thermal

#endif // SISPRODTHERMAL_H_
