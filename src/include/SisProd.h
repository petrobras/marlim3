/*
 * SisProd.h
 *
 * Created on: December 21, 2016
 *     Author: Eduardo
 */

#ifndef SISPROD_H_
#define SISPROD_H_

#include "Acidentes2.h"
#include "DriftFluxClosure.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Geometria.h"
#include "GradientCorrelations.h"
#include "Leitura.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "RuntimeDiagnostics.h"
#include "SisProdConstants.h"
#include "Vetor.h"
#include "celula3.h"
#include "celulaGas.h"
#include "chokegas.h"
#include "dados3DPoisson.h"
#include "estrat.h"
#include "estruturaTabDin.h"
#include "mapa.h"
#include "solver3DPoisson.h"
#include "solverPoissonAxiSim.h"
#include "SimulationContext.h"
#include <ctime>
#include <cmath>
#include <deque>
#include <array>
#include <string>
#include <vector>

class SProd;
namespace trendoutput { struct TrendState; }

namespace sisprod {

/// Identifies the valve-area snapshot on either side of a boundary-condition update.
enum class ValveSnapshotPhase { BeforeBoundaryUpdate, AfterBoundaryUpdate };

namespace fluidtracking { struct FluidTrackingState; }
namespace gaslift { struct GasLiftState; }
namespace sources { struct SourceState; }
namespace steady { struct SteadyStateSearchState; struct SteadyStateState;  }
namespace thermal { struct ThermalState; }
namespace transient {
class HydrateEnvelopeSolver;
struct TransientSolveState;

struct TransientStepState;

}  // namespace transient

/// A pressure-temperature table the fluids read through a double **: its rows, and a pointer to each.
struct GridTable {
    std::vector<std::vector<double>> rows;
    std::vector<double *> rowPointers;

    /// rowCount rows of columnCount values each.
    void allocate(int rowCount, int columnCount) {
        rows.assign(rowCount, std::vector<double>(columnCount));
        rowPointers.resize(rowCount);
        for (int i = 0; i < rowCount; i++)
            rowPointers[i] = rows[i].data();
    }
    /// What the fluids are given: the row pointers, null when there is no table.
    double **data() { return rowPointers.data(); }
    double *operator[](int i) { return rowPointers[i]; }
};

/// The fluid-property tables the fluids of a system's cells point at. The black-oil
/// compressibility-factor, specific-heat and liquid-density-derivative tables belong to the
/// input; the latent-heat table and the Livia bubble-point and solution-gas-ratio tables are
/// built and owned by the system, and so are the dynamic tables of a compositional network.
struct PropertyTables {
    /// Black-oil gas-compressibility-factor table.
    double **gasCompressibilityTable = nullptr;
    /// Pressure derivative of the black-oil compressibility-factor table.
    double **gasCompressibilityPressureDerivative = nullptr;
    /// Temperature derivative of the black-oil compressibility-factor table.
    double **gasCompressibilityTemperatureDerivative = nullptr;
    /// Black-oil gas specific-heat table.
    double **gasSpecificHeatTable = nullptr;
    /// Black-oil produced-liquid specific-heat table.
    double **liquidSpecificHeatTable = nullptr;
    /// Temperature derivative of liquid density.
    double **liquidDensityTemperatureDerivative = nullptr;
    /// Black-oil latent-heat table.
    GridTable latentHeatTable;
    /// Bubble-pressure values imported from PVTSim for the Livia solution-gas-ratio correlation. This table is
    /// separate from the full PVTSim fluid-property model.
    std::vector<double> pvtSimBubblePressures;
    /// Bubble-temperature values imported from PVTSim for the Livia solution-gas-ratio correlation. This table is
    /// separate from the full PVTSim fluid-property model.
    std::vector<double> pvtSimBubbleTemperatures;
    /// Precomputed solution-gas-ratio table for the Livia correlation, used to avoid repeating its expensive
    /// calculation during the simulation.
    GridTable liviaSolutionGasRatioTable;
    /// Dynamic fluid-property tables.
    std::vector<tabelaDinamica> dynamicTables;
};

/// The gas-lift line: its cells, the injection choke, the valves and the cells they join, its
/// pressure-velocity system, its surface and initial conditions, the column-annulus thermal
/// coupling, the completion-fluid interface, and the unloading controller's samples and
/// settings.
struct GasLiftLine {
    /// Global pressure-velocity coupling matrix for the gas line, sized for one cell until the system is sized for its
    /// input.
    BandMtx<double> systemMatrix{kGasLineUnknownsPerCell, kGasLineBandwidth, kGasLineBandwidth};
    /// Right-hand side and solution vector for the gas-line pressure-velocity system, sized like systemMatrix.
    Vcr<double> rightHandSide{kGasLineUnknownsPerCell};
    /// Production-line cell indices associated with gas-lift valves.
    std::vector<int> productionValveCells;
    /// Service-line cell indices associated with gas-lift valves.
    std::vector<int> gasValveCells;
    /// Service-line index where column-annulus thermal coupling begins.
    int annulusCouplingStart = 0;
    /// Service-line index where column-annulus thermal coupling ends.
    int annulusCouplingEnd = 0;
    /// Production-column index aligned with annulusCouplingStart.
    int tubingCouplingStart = 0;
    /// Production-column index aligned with annulusCouplingEnd.
    int tubingCouplingEnd = 0;
    /// Indicates whether column-annulus thermal coupling is enabled.
    bool annulusThermalCoupling = false;
    /// Gas-lift valves installed in the system.
    std::vector<ChokeGas> valveChokes;
    /// Time horizon used by the gas-lift unloading controller.
    double unloadingAveragingWindow = kUnloadingAveragingWindow;
    /// Maximum number of flow samples retained by the unloading PI controller.
    int unloadingSampleCapacity = kUnloadingSampleCapacity;
    /// The completion-fluid volume in the unloading controller's window: the sum of its volume samples.
    double accumulatedUnloadingVolume = 0;
    /// The time the unloading controller's window spans: the sum of its time-step samples.
    double unloadingAveragingInterval = 0;
    /// The window's volume samples: each step's largest completion-fluid volume flow through a gas-lift valve,
    /// times the step's time step.
    std::vector<double> maximumUnloadingFlowSamples;
    /// Time-step samples associated with the unloading-flow average.
    std::vector<double> unloadingTimeStepSamples;
    /// Separator pressure or pressure at the inlet of the downstream network section.
    double separatorPressure = 0;
    /// Index of the last control volume of the gas service line, which has cellCount + 1 of them; zero without a gas
    /// line.
    int cellCount = 0;
    /// Gas-injection pressure. Zero until an injection-pressure condition or a sensitivity analysis case sets it; the
    /// steady summary and the restart file record it either way.
    double injectionPressure = 0.;
    /// Gas-injection temperature in the service line. Zero until an injection condition or a sensitivity analysis
    /// case sets it; the restart file records it either way.
    double injectionTemperature = 0.;
    /// Gas-injection choke model.
    ChokeGas injectionChoke;
    /// Gas service-line control volumes, held by gasCells.
    CelG *cells = nullptr;
    /// The gas service-line control volumes that cells points at.
    std::vector<CelG> gasCells;
    /// Current service-line cell containing the completion-fluid/gas interface.
    int interfaceCell = kNoInterfaceCell;
    /// Maximum time step that keeps the unloading interface within one gas-line cell.
    double interfaceTimeStep = 0.;
    /// Current completion-fluid/gas interface velocity.
    double interfaceVelocity = 0.;
    /// Previous-time-level interface cell.
    int previousInterfaceCell = kNoInterfaceCell;
    /// Previous-time-level interface-limited time step.
    double previousInterfaceTimeStep = 0.;
    /// Previous-time-level interface velocity.
    double previousInterfaceVelocity = 0.;
};

/// The state of a transient run: the time step, its history and the restrictions on it, the
/// moving averages and running totals, the Master1 valve state and schedule, the
/// production-line pressure-velocity system, the buffered sources, the event and log
/// counters, the transport switches, the pigs and the cells of the two-dimensional Poisson
/// model.
struct TransientRun {
    /// Enables transport equations for primitive black-oil properties, including API gravity, BSW, gas-oil ratio, and
    /// light/heavy mass fractions.
    int trackGasOilRatio = 0;
    /// Enables transport equations for gas density and the gas-phase CO2 molar fraction.
    int trackGasGravity = 0;
    /// Current Master1 valve state.
    int masterValveState = 0;
    /// Counter used while changing the Master1 state.
    int masterValveCounter = 0;
    /// Cell indices where pigs are received.
    std::vector<int> pigReceiverCells;
    /// Global pressure-velocity coupling matrix for the multiphase production line, sized for one cell until the
    /// system is sized for its input.
    BandMtx<double> systemMatrix{kProductionUnknownsPerCell, kProductionLeftBandwidth, kProductionRightBandwidth};
    /// Times at which Master1 closes.
    std::vector<double> masterValveCloseTimes;
    /// Times at which Master1 opens.
    std::vector<double> masterValveOpenTimes;
    /// Number of events written to the event log.
    int eventLogCount = 0;
    /// Smallest production-line control-volume length.
    double smallestCellLength = 0.;
    /// Simulation time-step counter.
    int stepCount = 0;
    /// Controls event-log output frequency.
    int printCounter = 0;
    /// Index of the next scheduled simulation event.
    int eventIndex = 0;
    /// History of recently accepted time steps.
    std::vector<double> acceptedTimeSteps;
    /// History of time steps proposed by the CFL criterion.
    std::vector<double> cflTimeSteps;
    /// Average time step proposed by the CFL criterion.
    double meanCflTimeStep = 1.;
    /// Average time step actually used by the simulation.
    double meanAcceptedTimeStep = 1.;
    /// Indicates that time-step growth must remain restricted.
    bool timeStepRestricted = false;
    /// Number of remaining steps under the current time-step restriction.
    int restrictedStepsLeft = 0;
    /// Accumulated CFL time steps used to compute meanCflTimeStep.
    double totalCflTimeStep = 0.;
    /// Accumulated accepted time steps used to compute meanAcceptedTimeStep.
    double totalAcceptedTimeStep = 0.;
    /// Counts alternating liquid-flow oscillations near an active surface choke.
    int slugCount = kInitialSlugCount;
    /// Multiplier applied to a small artificial upstream gas flow during difficult Master1 closures. It mitigates a
    /// pressure blind spot when the upstream side contains only liquid and increases if the problem persists.
    double artificialGasMultiplier = 0;
    /// The artificial gas the closing-valve control added in the current time step, kept until the step is accepted,
    /// when it goes to the run's diagnostics.
    std::vector<diagnostics::ArtificialGasAddition> artificialGasThisStep;
    /// Recent maximum pressure-change rates.
    std::vector<double> maximumPressureRates;
    /// Average maximum pressure-change rate.
    double meanMaximumPressureRate = 1.;
    /// Recent maximum temperature-change rates.
    std::vector<double> maximumTemperatureRates;
    /// Average maximum temperature-change rate.
    double meanMaximumTemperatureRate = 1.;
    /// Counter controlling compositional-property refreshes.
    int compositionalRefreshCounter = 0;
    /// Production cells handled by the two-dimensional Poisson model.
    std::vector<int> poisson2DCells;
    /// Signals that the time step must be adjusted.
    int adjustTimeStep = 0;
    /// Requests rollback and time-step reevaluation when at least one control volume produces a holdup or volume
    /// fraction outside the physical [0, 1] range.
    int rollbackRequested = 0;
    /// Master and accessory valve area ratios after the boundary-condition update.
    std::array<double, kMasterRatioHistorySize> valveAreaRatiosAfterUpdate{};
    /// Master and accessory valve area ratios before the boundary-condition update.
    std::array<double, kMasterRatioHistorySize> valveAreaRatiosBeforeUpdate{};
    /// Critical Master1 ratio history used by the switching logic.
    std::array<double, kMasterRatioHistorySize> masterValveCriticalRatios{
        kDefaultMasterCriticalRatio, kDefaultMasterCriticalRatio, kDefaultMasterCriticalRatio,
        kDefaultMasterCriticalRatio, kDefaultMasterCriticalRatio, kDefaultMasterCriticalRatio,
        kDefaultMasterCriticalRatio, kDefaultMasterCriticalRatio, kDefaultMasterCriticalRatio,
        kDefaultMasterCriticalRatio};
    /// Previous-time-level value of gasLift.separatorPressure.
    double previousSeparatorPressure = 0.;
    /// Recorded in the restart file; nothing in the simulation reads it.
    double restartSourceMass = 0.;
    /// CFL safety factor, typically set to 0.8.
    double cflSafetyFactor = 0;
    /// Time-averaged pressure in the final production-line control volume. Used to decide whether the surface choke
    /// behaves as a localized pressure loss or as a discharge-flow model.
    double movingMeanPressure = 0;
    /// Time-averaged mixture volumetric flux in the final production-line control volume. Used by the surface-choke
    /// operating-mode logic.
    double movingMeanFlux = 0;
    /// Time-averaged void fraction in the final production-line control volume, updated once the averaging window
    /// fills. Zero until then; only the restart file reads it.
    double movingMeanVoidFraction = 0.;
    /// Start time of the moving-average window.
    double movingMeanStartTime = 0;
    /// Duration of the moving-average window.
    double movingMeanWindow = 0.;
    /// Accumulated pressure used to compute movingMeanPressure.
    double accumulatedPressure = 0.;
    /// Accumulated mixture flux used to compute movingMeanFlux.
    double accumulatedFlux = 0.;
    /// Accumulated void fraction used to compute movingMeanVoidFraction.
    double accumulatedVoidFraction = 0.;
    /// Pressure samples used by the moving-average calculation.
    std::deque<double> pressureSamples;
    /// Mixture-flux samples used by the moving-average calculation.
    std::deque<double> fluxSamples;
    /// Void-fraction samples used by the moving-average calculation.
    std::deque<double> voidFractionSamples;
    /// Time samples associated with the moving-average window.
    std::deque<double> timeSamples;
    /// Current surface-choke open/closed state.
    int surfaceChokeOpen = 0;
    /// Previous-time-level surface-choke state.
    int previousSurfaceChokeOpen = 0;
    /// Counter that delays transitions out of active-choke mode.
    int activeChokeDelay = 0;
    /// Previous-time-level value of activeChokeDelay.
    int previousActiveChokeDelay = 0;
    /// Signals a surface-choke operating-mode transition.
    bool chokeModeChanged = false;
    /// Selects the interphase mass-transfer model: 0 = complete, 1 = fully explicit, 2 = simplified, and 3 =
    /// disabled.
    int massTransferModel = 0;
    /// Number of pigs currently moving through the line.
    int movingPigCount = 0;
    /// Previous-time-level value of movingPigCount.
    int previousMovingPigCount = 0;
    /// Right-hand side and solution vector for the production-line pressure-velocity system, sized like systemMatrix.
    Vcr<double> rightHandSide{kProductionUnknownsPerCell};
    /// Simulation end time.
    double finalTime = 0;
    /// Current complete-model activation state.
    int fullModel = 1;
    /// Auxiliary CFL time-step accumulator.
    double auxiliaryCflTimeStep = 0.;
    /// Auxiliary accepted-time-step accumulator.
    double auxiliaryAcceptedTimeStep = 0.;
    /// Intermediate production-liquid inflow estimate for a network section.
    double bufferedLiquidMassSource = 0.;
    /// Intermediate complementary-liquid inflow estimate for a network section.
    double bufferedComplementaryMassSource = 0.;
    /// Intermediate gas inflow estimate for a network section.
    double bufferedGasMassSource = 0.;
    /// Minimum time step allowed by the current cycle. Zero until a transient cycle sets it.
    double minimumCycleTimeStep = 0.;
};

/// The storage of one family of trends: for each trend, its rows of samples, and its reset time
/// and sample counts. The solve reads it through the pointers allocate() points at it.
struct TrendSet {
    std::vector<std::vector<std::vector<double>>> values;
    std::vector<std::vector<double *>> rows;
    std::vector<double **> matrices;
    std::vector<double> resetTimers;
    std::vector<int> counts;
    std::vector<int> bufferedCounts;

    /// For each of the count trends, length(i) rows of rowWidth(i) values whose first
    /// sentinelCount(i) hold kTrendSentinel, and the reset timers and sample counts,
    /// zeroed; points matrixPointer, resetTimerPointer, countPointer and bufferedCountPointer
    /// at them.
    template <typename Length, typename RowWidth, typename SentinelCount>
    void allocate(int count, Length length, RowWidth rowWidth, SentinelCount sentinelCount, double ***&matrixPointer,
                  double *&resetTimerPointer, int *&countPointer, int *&bufferedCountPointer) {
        values.resize(count);
        rows.resize(count);
        matrices.resize(count);
        for (int i = 0; i < count; i++) {
            const int n = length(i);
            values[i].assign(n, std::vector<double>(rowWidth(i)));
            rows[i].resize(n);
            for (int j = 0; j < n; j++) {
                rows[i][j] = values[i][j].data();
                for (int k = 0; k < sentinelCount(i); k++)
                    values[i][j][k] = kTrendSentinel;
            }
            matrices[i] = rows[i].data();
        }
        resetTimers.assign(count, 0.);
        counts.assign(count, 0);
        bufferedCounts.assign(count, 0);
        matrixPointer = matrices.data();
        resetTimerPointer = resetTimers.data();
        countPointer = counts.data();
        bufferedCountPointer = bufferedCounts.data();
    }
};

/// The trend and profile output a system writes: the trend matrices of every family
/// (production, gas line, and the wall temperatures of both), their sample counters and reset
/// times, the profile and single-cell output indices, the cells whose radial temperature
/// profiles are written, and the count of output passes.
struct TrendRecorder {
    /// Gas-line cells where radial temperature profiles are written.
    std::vector<int> gasWallProfileCells;
    /// Times at which gas-line trend buffers are reset.
    double *gasTrendResetTimes = nullptr;
    /// Number of gas-line trend samples currently stored.
    int *gasTrendCounts = nullptr;
    /// Number of gas-line trend samples stored before the last flush.
    int *gasTrendBufferedCounts = nullptr;
    /// Buffered gas-line wall-temperature trend data.
    double ***gasWallTrendMatrices = nullptr;
    /// Times at which gas-line wall-temperature buffers are reset.
    double *gasWallTrendResetTimes = nullptr;
    /// Number of gas-line wall-temperature samples currently stored.
    int *gasWallTrendCounts = nullptr;
    /// Number of gas-line wall-temperature samples stored before the last flush.
    int *gasWallTrendBufferedCounts = nullptr;
    /// Production-line cells where radial temperature profiles are written.
    std::vector<int> productionWallProfileCells;
    /// Times at which production-line trend buffers are reset.
    double *productionTrendResetTimes = nullptr;
    /// Number of production-line trend samples stored before the last flush.
    int *productionTrendBufferedCounts = nullptr;
    /// Buffered production-line wall-temperature trend data.
    double ***productionWallTrendMatrices = nullptr;
    /// Times at which production-line wall-temperature buffers are reset.
    double *productionWallTrendResetTimes = nullptr;
    /// Number of production-line wall-temperature samples currently stored.
    int *productionWallTrendCounts = nullptr;
    /// Number of production-line wall-temperature samples stored before the last flush.
    int *productionWallTrendBufferedCounts = nullptr;
    /// Current production-profile output index.
    int productionProfileTimeIndex = 0;
    /// Current gas-line profile output index.
    int gasProfileTimeIndex = 0;
    /// Current production-wall-temperature profile output index.
    int productionWallProfileTimeIndex = 0;
    /// Current gas-line wall-temperature profile output index.
    int gasWallProfileTimeIndex = 0;
    /// For each single-cell output, the index of its next output time.
    std::vector<int> singleCellOutputTimeIndices;
    /// The production-line trends, read through productionTrendMatrices, productionTrendResetTimes,
    /// productionTrendCounts and productionTrendBufferedCounts.
    TrendSet productionTrendSet;
    /// The gas-line trends, read through gasTrendMatrices, gasTrendResetTimes, gasTrendCounts and
    /// gasTrendBufferedCounts.
    TrendSet gasTrendSet;
    /// The production-line wall-temperature trends, read through productionWallTrendMatrices,
    /// productionWallTrendResetTimes, productionWallTrendCounts and productionWallTrendBufferedCounts.
    TrendSet productionWallTrendSet;
    /// The gas-line wall-temperature trends, read through gasWallTrendMatrices, gasWallTrendResetTimes,
    /// gasWallTrendCounts and gasWallTrendBufferedCounts.
    TrendSet gasWallTrendSet;
    /// Buffered gas-line trend data.
    double ***gasTrendMatrices = nullptr;
    /// Buffered production-line trend data.
    double ***productionTrendMatrices = nullptr;
    /// Number of production-line trend samples currently stored.
    int *productionTrendCounts = nullptr;
    /// Trend-output cycle counter; a value of one triggers header output.
    int trendOutputCount = 0;
};

/// What a system keeps as a branch of a network: the inlet and outlet conditions of the
/// previous time level, the reverse-flow state, and the coupling with the other branch of a
/// parallel network.
struct NetworkCoupling {
    /// Inlet pressure stored at the previous time level.
    double previousInletPressure = -1;
    /// Inlet temperature stored at the previous time level.
    double previousInletTemperature = -1;
    /// Inlet gas mass fraction stored at the previous time level.
    double previousInletQuality = -1;
    /// Inlet void fraction stored at the previous time level.
    double previousInletVoidFraction = -1;
    /// Inlet complementary-liquid fraction stored at the previous time level.
    double previousInletLiquidFraction = -1;
    /// Previous-time-level value of presfim.
    double previousOutletPressure = 0;
    /// Gas mass fraction imposed during reverse flow at the last control volume. Used only by transient network
    /// simulations.
    double reverseQuality = 1.;
    /// Previous-time-level value of reverseQuality.
    double previousReverseQuality = 1.;
    /// Complementary-liquid fraction imposed during reverse flow at the last control volume. Used only by transient
    /// network simulations.
    double reverseLiquidFraction = 0.;
    /// Previous-time-level value of reverseLiquidFraction.
    double previousReverseLiquidFraction = 1.;
    /// Previous-time-level downstream or separator temperature.
    double previousSeparatorTemperature = 0.;
    /// Temperature downstream of the surface choke, delivered to the next network section.
    double chokeOutletTemperature = 0;
    /// Previous-time-level value of masChkSup.
    int previousSurfaceChokeActive = 0;
    /// Previous-time-level value of transient.chokeModeChanged.
    bool previousChokeModeChanged = false;
    /// Secondary-line index where parallel-network coupling with the primary line begins.
    int secondaryCouplingStart = 0;
    /// Secondary-line index where parallel-network coupling with the primary line ends.
    int secondaryCouplingEnd = 0;
    /// Primary-line index aligned with secondaryCouplingStart.
    int primaryCouplingStart = 0;
    /// Primary-line index aligned with secondaryCouplingEnd.
    int primaryCouplingEnd = 0;
    /// Indicates thermal coupling on the primary branch of a parallel network.
    bool primaryThermalCoupling = false;
    /// Indicates thermal coupling on the secondary branch of a parallel network.
    bool secondaryThermalCoupling = false;
    /// Initial source indices for parallel-network coupling.
    std::vector<int> parallelSourceCells;
    /// Initial production-liquid sources for the parallel network.
    std::vector<double> parallelSourceProductionLiquid;
    /// Initial complementary-liquid sources for the parallel network.
    std::vector<double> parallelSourceComplementaryLiquid;
    /// Initial gas sources for the parallel network.
    std::vector<double> parallelSourceGas;
    /// Boundary-condition type applied to the secondary parallel-network branch.
    int parallelSecondaryBoundaryCondition = -1;
    /// Primary branch index in a parallel network.
    int parallelPrimaryBranch = -1;
    /// Secondary branch index in a parallel network.
    int parallelSecondaryBranch = -1;

    /// Takes over other's role in a parallel network: the boundary-condition type of the secondary branch and the
    /// indices of both branches.
    void takeParallelRole(const NetworkCoupling &other) {
        parallelSecondaryBoundaryCondition = other.parallelSecondaryBoundaryCondition;
        parallelPrimaryBranch = other.parallelPrimaryBranch;
        parallelSecondaryBranch = other.parallelSecondaryBranch;
    }
};

/// What the steady-state search carries from one iteration to the next: the convergence
/// monitor and its reference, the initial estimates, the reverse flow of a network solution
/// and its fluid and temperature, the number of production fluids and the slow thermal
/// coupling parameter.
struct SteadySearch {
    /// Control parameter for slowly varying thermal coupling.
    double slowHeatTransferThreshold = kSlowHeatTransferThreshold;
    /// Number of production fluids.
    int productionFluidCount = 0;
    /// Initial holdup estimate used by the steady-state solver.
    double holdupGuess = -1.;
    /// Controls the search for an initial steady-state estimate.
    bool initialEstimateSearch = false;
    /// Fluid state received from downstream during reverse network flow.
    ProFlu reverseNetworkFluid;
    /// Temperature associated with reverse network flow.
    double reverseFlowTemperature = 0.;
    /// Indicates reverse flow in the steady-state network solution.
    bool steadyReverseFlow = false;
    /// Current steady-state convergence monitor.
    double convergenceMonitor = kInitialConvergenceMonitor;
    /// Reference value for the steady-state convergence monitor.
    double convergenceMonitorReference = 1.;
};

/// The coupling with the three-dimensional thermal model: its Poisson solver and the index
/// corrections of the coupled sections.
struct Coupling3D {
    /// Index corrections used by coupled thermal sections.
    std::vector<int> coupledIndexCorrections;
    /// Three-dimensional Poisson solver used by the thermal model.
    solverP3D poissonSolver3D;
};

}  // namespace sisprod

/// Models and solves a one-dimensional production system.
///
/// The class owns the production and gas-service-line state, fluid-property tables, boundary conditions,
/// network-coupling data, transient buffers, and steady-state/transient solution procedures.
class SProd {
    friend class sisprod::transient::HydrateEnvelopeSolver;

  public:
    /// What a system built from an already parsed input takes from the system that
    /// input came from: its place in the network and its reverse-flow state. Each field
    /// starts at the declared value of the member it carries.
    struct CarriedState {
        int noextremo = 1;
        int noinicial = 1;
        int derivaAnel = -1;
        int bloq = 0;
        double previousReverseLiquidFraction = 1.;
        double previousReverseQuality = 1.;
        double minimumCycleTimeStep = 0.;
    };

    /// Constructs and initializes a production-system simulation from the input and log files. A
    /// dynamicTableOverride that is not negative replaces the input's choice of dynamic property table.
    SProd(std::string inputFile, std::string logFile, tipoValidacaoJson_t jsonValidation,
          tipoSimulacao_t simulationType, SimulationContext *globals = nullptr, int dynamicTableOverride = -1,
          int blocking = 0, int reversedGeometry = 0, double *extraSourceLengths = nullptr,
          int *extraSourceCells = nullptr, int extraSourceCount = 0, int steadyBranch = 1);

    /// Creates an empty system, which loadSection fills.
    SProd();

    /// Builds a system from an already parsed input, without reading the JSON file
    /// again, with the place in the network and the reverse-flow state in carried.
    SProd(Ler &parsedInput, const CarriedState &carried);

    /// Not copyable nor assignable: the object owns raw arrays.
    SProd(const SProd &) = delete;
    SProd &operator=(const SProd &) = delete;

    /// Fills this empty system with other's section: reads other's input file again, takes over other's place in
    /// the network, its reverse-flow state and its role in a parallel network, and assembles the system. Throws
    /// std::logic_error when this system was already assembled.
    void loadSection(const SProd &other);

    /// The state another system built from this one's input takes over.
    CarriedState carriedState() const;

    /// Loads pressure-velocity results into cell and face state variables.
    void renova(int explicitScheme = 0);
    /// Updates only section-end states during intermediate network convergence.
    void renovaBuffer();
    /// Copies previous states when an active choke bypasses the intermediate network solve.
    void renovaBufferCego();

    /// Updates distributed mass-transfer terms used by void-fraction and mixture-mass equations.
    void renovaTemp();
    /// Computes T1 and T2 used to split mixture mass flow into liquid and gas flows.
    void renovaterm(int inflowMode = 0, int explicitMode = 0);
    /// Computes T1 and T2 at the outlet of an internal network section.
    void renovatermAfluFim();
    /// Computes T1 and T2 at the inlet of an internal network section.
    void renovatermColIni();

    /// Calculates outlet-choke flow from the last-cell and separator pressures.
    void calcCCpres(double reverseQuality = 1., double reverseVoidFraction = 1.,
                    double reverseLiquidFraction = 0.);
    /// Calculates outlet-choke flow for the intermediate network state.
    void calcCCBuffer(double reverseQuality = 1., double reverseVoidFraction = 1.,
                      double reverseLiquidFraction = 0.);

    /// Selects a stable time step from CFL and additional model restrictions.
    void determinaDT(int explicitScheme = 0);
    /// Limits time-step growth when accepted steps remain well below the CFL estimate.
    void atenuaDtMax();
    /// Checks whether liquid-density variation requires the complete formulation.
    void avaliaVariaDpDt(int explicitScheme = 0);
    /// Captures valve area ratios for the selected side of the boundary-condition update.
    void captureValveAreaRatios(sisprod::ValveSnapshotPhase phase);
    /// Restricts the time step during valve-state transitions.
    void restringeDTporValv();

    /// Advances the gas-injection system and prepares its coupling with the production line.
    void solveLinGas();
    /// Advances phase volume fractions.
    void EvoluiFrac(double reverseVoidFraction = 1., double reverseLiquidFraction = 0., int cycle = 0);
    /// Restores the initial fraction state after an invalid update.
    void ReiniEvolFrac0();
    /// Restores previous volume fractions after a nonphysical update.
    void ReiniEvolFrac();
    /// Updates phase fractions in cells affected by a moving pig.
    void AtualizaPig();
    /// Assembles and solves the global pressure-velocity coupling system.
    void SolveAcopPV(int explicitScheme = 0, int cycle = 0);

    /// Advances the transient energy equation.
    void marchaEnergTrans(int cycle = 0, int maximumCycle = 0);
    /// Refreshes local dynamic fluid-property tables.
    void atualizaMiniTab();
    /// Updates the first boundary condition.
    void atualizaCC1();

    /// Advances the complete transient production-system solution and handles output and logging. collectorFluid is
    /// the fluid that flows back from the collector, null when the system has none.
    void SolveTrans(double reverseQuality = 1., double reverseVoidFraction = 1., double reverseLiquidFraction = 0.,
                    int network = -1, const ProFlu *collectorFluid = nullptr);

    /// Appends the buffered rows of a production-line trend file.
    void ImprimeTrendP(int trend, int network = -1);
    /// Writes the column captions of a production-line trend file, truncating it.
    void ImprimeTrendPCab(int trend, int network = -1);
    /// Appends the buffered rows of a service-line trend file.
    void ImprimeTrendG(int trend, int network = -1);
    /// Writes the column captions of a service-line trend file, truncating it.
    void ImprimeTrendGCab(int trend, int network = -1);

    /// Marches the steady production solution using a bottomhole-pressure guess with outlet pressure prescribed.
    double marchaProdPerm1(double pressureGuess);
    /// Brackets and solves the bottomhole-pressure root for marchaProdPerm1.
    double buscaProdPfundoPerm(double guess = -1., int attemptCount = -1);
    /// Brackets and solves the reverse-flow bottomhole-pressure root.
    double buscaProdPfundoPermRev(double guess = -1.);
    /// Brackets and solves the bottomhole-pressure root for marchaProdPerm2.
    double buscaProdPfundoPerm2(double guess = -1., int attemptCount = -1);
    /// Runs a direct march when inlet pressure and flow rate are known.
    double buscaProdPfundoPerm3(double inletPressure);
    /// Marches the steady production solution using a bottomhole mass-flow guess.
    double marchaProdPresPres1(double massFlowRateGuess);
    /// Brackets and solves the mass-flow root for marchaProdPresPres1.
    double buscaProdPresPresPerm(double massFlowRateGuess, double maximumFlowRate = 0., int iterationCount = 0);
    /// Brackets and solves the reverse-flow mass-flow root.
    double buscaProdPresPresPermRev(double massFlowRateGuess, double maximumFlowRate = 0., int iterationCount = 0);
    /// Brackets and solves the mass-flow root for marchaProdPresPres2.
    double buscaProdPresPresPerm2(double massFlowRateGuess, double maximumFlowRate = 0.);
    /// Brackets and solves the mass-flow root for the closed-choke case, chosen
    /// by Num4Main when arq.chokep.abertura[0] <= 1e-15.
    double buscaProdPresPresPerm3(double massFlowRateGuess, double maximumFlowRate = 0.);

    /// Solves injection cases CC1 and CC3.
    double buscaInjPfundoPerm1(double guess = -1.);
    /// Solves injection case CC0.
    double buscaInjPfundoPerm2(double guess = -1.);
    /// Solves injection case CC2.
    double buscaInjPfundoPerm3(double guess = -1.);
    /// Solves injection case CC4.
    double buscaInjPfundoPerm4();
    /// Solves injection case CC5.
    double buscaInjPfundoPerm5(double guess = -1.);

    /// Estimates steady production-network node pressures from hydrostatics.
    double hidroreverso(double liquidHoldup, double liquidFlowRate = 0, double gasFlowRate = 0);
    /// Estimates steady injection-network node pressures from hydrostatics.
    double hidroreversoInj(double liquidHoldup, double liquidFlowRate = 0);
    /// Estimates secondary-branch pressures in a parallel production network.
    double hidroTramoSecundario(double quality);

    /// Calculates temperature downstream from the surface choke for network coupling.
    void calcTempFim();

    /// Returns the outlet temperature after an open surface choke with forward pressure drop.
    /// Returns the last cell temperature when the choke is closed or its mass flow is zero.
    double surfaceChokeOutletTemperature();

    /// Enables latent-heat calculations for black-oil simulations.
    int CalcLat = 0;

    /// Section index when this object belongs to a pipeline network.
    int indTramo = -1;
    /// Number of control volumes in the production line.
    int ncel = 0;

    /// What the steady-state search carries from one iteration to the next.
    sisprod::SteadySearch steadySearch;

    /// Current inlet pressure boundary condition.
    double presE = -1;
    /// Current inlet temperature boundary condition.
    double tempE = -1;
    /// Current inlet gas mass fraction.
    double titE = -1;
    /// Computed inlet void fraction.
    double alfE = -1;
    /// Computed inlet complementary-liquid fraction.
    double betaE = -1;
    /// What this system keeps as a branch of a network.
    sisprod::NetworkCoupling networkCoupling;

    /// Pressure in the last production-line control volume. When the surface choke is open, this value is equal to
    /// the separator pressure.
    double presfim = 0;
    /// Separator temperature or temperature at the inlet of the downstream section.
    double tGSup = 0.;
    /// Production-line inlet temperature when no inlet-pressure boundary condition is imposed.
    double temperatura = 0;

    /// Indicates whether the surface choke is active.
    int masChkSup = 0;
    /// The fluid-property tables the fluids of this system's cells point at.
    sisprod::PropertyTables tables;

    /// Parsed user input and simulation configuration.
    Ler arq;

    /// Buffer used to write gas-line profiles, one row until the system is sized for its input.
    FullMtx<double> flutG{1, sisprod::kGasLineProfileFixedColumns};
    /// Buffer used to write production-line profiles, one row until the system is sized for its input.
    FullMtx<double> flut{1, sisprod::kProductionProfileFixedColumns};
    /// The gas-lift line: its cells, chokes and valves, its pressure-velocity system, and the unloading of its
    /// completion fluid.
    sisprod::GasLiftLine gasLift;
    /// The state of a transient run.
    sisprod::TransientRun transient;

    /// Current time step.
    double dt = 0.;

    /// The trend and profile output this system writes.
    sisprod::TrendRecorder trends;

    /// Event-log file name.
    std::string tmpLog;

    /// Steady-state mode flag: 1 while Num4Main solves a network branch at steady state. The source terms read it
    /// (sisprod::sources::SourceState).
    bool modoPerm = false;

    /// Surface-choke model.
    choke chokeSup;
    /// Multiphase production-line control volumes, held by productionCells.
    Cel *celula = nullptr;

    /// Indicates that the section outlet is not connected to another network section.
    int noextremo = 1;
    /// Indicates that the section inlet is not connected to another network section.
    int noinicial = 1;
    /// Indicates that this section is a branch of a gas-lift ring network.
    int derivaAnel = -1;

    /// Section-blocking state.
    int bloq = 0;

    /// The coupling with the three-dimensional thermal model.
    sisprod::Coupling3D coupling3D;
    solverPAxiSim axisymmetricThermalSolver;
    /// Shared simulation settings and runtime state.
    SimulationContext *simulationContext = nullptr;

  private:
    /// Per-call views shared by steady marches and their boundary searches.
    struct SteadyCallState;
    /// Per-call views shared by the transient solve and its collaborating models.
    struct TransientCallState;
    /// Prepares only the views used by a coupled steady calculation.
    SteadyCallState steadyCallStateOf();
    /// Prepares only the views used by a coupled transient calculation.
    TransientCallState transientCallStateOf();

    /// Binds the current gasLift data without copying simulation state.
    sisprod::gaslift::GasLiftState gasLiftStateOf();
    /// Binds the current steady data without copying simulation state.
    sisprod::steady::SteadyStateState steadyStateOf();
    /// Binds the current search data without copying simulation state.
    sisprod::steady::SteadyStateSearchState searchStateOf();
    /// Binds the current transient data without copying simulation state.
    sisprod::transient::TransientStepState transientStateOf();
    /// Binds the current fluidTracking data without copying simulation state.
    sisprod::fluidtracking::FluidTrackingState fluidTrackingStateOf();
    /// Binds the current transientSolve data without copying simulation state.
    sisprod::transient::TransientSolveState transientSolveStateOf();
    /// Binds the current closure data without copying simulation state.
    driftflux::coefficient::ClosureState closureStateOf();
    /// Binds the current source data without copying simulation state.
    sisprod::sources::SourceState sourceStateOf();
    /// Binds the current thermal data without copying simulation state.
    sisprod::thermal::ThermalState thermalStateOf();
    /// Binds the current trend data without copying simulation state.
    trendoutput::TrendState trendStateOf() const;

    /// Assembles the system from arq: the property tables and the steady-state search as declared, the solution
    /// buffers sized for arq's cells, carried's place in the network, and the production section with the extra gas
    /// sources at extraSourceLengths, whose cell indices it records in extraSourceCells.
    void assemble(const CarriedState &carried, double *extraSourceLengths = nullptr, int *extraSourceCells = nullptr,
                  int extraSourceCount = 0);
    /// Sizes the profile buffers and the pressure-velocity systems of both lines for arq's cells, all zero.
    void allocateSolutionBuffers();

    /// Takes over the place in the network and the reverse-flow state in carried.
    void applyCarriedState(const CarriedState &carried);
    /// Points every cell fluid, and every source fluid it carries, at the
    /// bubble-point tables read from the PVTSim file, and switches them to
    /// saturation model 4.
    void assignPvtSimBubbleTablesToCells();
    /// Reads the bubble-point curve from the PVTSim file, points the cell fluids at
    /// it and writes perfilBolha; with tabRSPB on, computes the reduced solution
    /// gas-oil ratio Rs/Rsb on the file's grid and writes perfilRSLivia.
    void loadPvtSimSaturationTables();
    /// Builds the bubble-point curve and the solution gas-oil ratio table from the
    /// fluid correlations over the input table's pressure-temperature grid, writes
    /// perfilBolha and perfilRSLivia, and points the cell fluids at both.
    void generateSaturationTablesFromCorrelations();
    /// Copies the run configuration into the members, checks that an injection well
    /// has the IPR its boundary condition needs, builds the Cp and JTL tables,
    /// hands the fluid constants to every fluid, generates the pipe and the
    /// production cells, and places every source -- including the extra gas sources
    /// at extraSourceLengths, whose cell indices it records in extraSourceCells.
    void buildProductionCells(double *extraSourceLengths, int *extraSourceCells, int extraSourceCount);
    /// Gives the inlet the sources its boundary condition needs (and the second
    /// cell, under a blockage), then places the accessories -- pumps, volumetric
    /// pumps, pressure-drop requirements, heat sources, the master valve and the
    /// other valves -- and sets up the outlet pressure, the surface and injection
    /// chokes and the pigs.
    void configureInletSourcesAndAccessories(int extraSourceCount);
    /// Builds the gas-lift line when there is one: its cells, the second master
    /// valve, the injection choke, one gas-lift valve choke per valve with its
    /// position on both lines, and each gas cell's share of the annulus above the
    /// discharge cell. Without a gas line, sets the last gas-cell index to zero.
    void buildGasLiftLine();

    /// Initializes the axisymmetric well solver after both pipe geometries are available.
    void buildAxisymmetricThermalCoupling();
    /// Rejects or warns about source and boundary-condition combinations the run
    /// cannot honour (RN-300, RN-301, gas-lift discharge without an IPR, no outlet
    /// pressure, accessories in the last two cells), builds the gas-lift discharge
    /// hydrostatics and the event log, applies the initial state of a production
    /// well, and records the surface temperature and mass flow it starts from.
    void validateSetupAndApplyInitialState();
    /// With the dynamic property table on, splits the pipe into table segments that
    /// end at every source cell, including leaks. Propagates neighbouring
    /// inclinations across horizontal cells in left and right passes.
    void buildDynamicTablesAndInclinations();
    /// Sets the latent-heat switch from the input and, when it is on and
    /// flashCompleto is 0, reads the latent-heat table from the PVTSim file into
    /// tables.latentHeatTable and writes perfilLatente.
    void configureLatentHeat();
    /// Resets or evaluates gas-density corrections for cell fluids and the liquid,
    /// IPR, multiple-source and porous-reservoir fluids. Gas-injection fluids are
    /// excluded from the source traversal. Updates critical properties only for
    /// cell fluids, then copies the inlet source fluid into the first cell.
    void applyDensityCorrectionsAndInletFluid();
    /// Lists the cells with two-dimensional heat diffusion, copies the master-valve opening and closing times, maps
    /// the transient profile positions to global thermal-node indices, and allocates the trend matrices of the
    /// production line, the gas line and the transient trends, sized to the longest simulated time and filled with
    /// kTrendSentinel.
    void allocateEventProfileAndTrendArrays();
    /// Writes the requested initial profiles and initializes their next-time indices.
    /// Profile output is suppressed in AP mode; index initialization is unconditional.
    void initializeProfileOutputState();
    /// Sets the event-log path and appends the events due at initialization,
    /// recording a separate wall-clock timestamp for each event outside injection wells.
    void writeInitialEventLog();
    /// Resets the column-annulus and network coupling flags, locates the
    /// column-annulus coupling range on the gas line, sets the steady and transient
    /// profile counters and their first output times, opens the event log with the
    /// events known at start, finds the smallest cell length, and zeroes the
    /// moving-average and running-total state the transient loop starts from.
    void resetCouplingAndOutputState();
    /// Builds cells, accessories and the gas line, applies the initial state,
    /// configures tables and fluid corrections, then initializes output and
    /// coupling state. Each phase consumes the state established by its predecessors.
    void montasistema(double *extraSourceLengths, int *extraSourceCells, int extraSourceCount);

    /// Initializes the gas service line for gas-lift unloading.
    void HidroDescargaG();
    /// Initializes the production line for gas-lift unloading.
    void HidroDescargaP();

    /// Whether this step computes the hydrate envelopes: asked for in the input, and the simulated time not past the
    /// end of the run.
    bool hydrateEnvelopeDue() const;
    /// Runs the hydrate-envelope solvers for the production and gas lines.
    void solveHydrateEnvelopes();

    /// Number of iterations used to bracket the initial steady-state root.
    int steadyIteration = 0;

    /// The production-line control volumes celula points at.
    std::vector<Cel> productionCells;

    /// Indicates that the thermal source term is disabled.
    bool thermalSourceDisabled = false;

    static constexpr const char *saidaTextoSis[16] = {"                          Post Coitum Omine Animal Triste Est                   ",
                                     "           'Ouca-me. O fim quase nunca esta longe, em nenhum momento!'          ",
                                     "      So nos curamos de um sofrimento depois de o haver suportado ate o fim.    ",
                                     "                   Infeliz e o espirito ansioso pelo futuro.                    ",
                                     "                                    Memento Mori                                ",
                                     " Somente um progresso calmo e constante, livre de precipitacao, conduz ao objetivo.",
                                     "             Paciencia, nove mulheres nao conseguem gerar uma crianca em um mes. ",
                                     "                  A necessidade e a mae da inovacao, mas a paciencia e o pai    ",
                                     "O sucesso nao e uma linha reta, e um jogo de resistencia, e cada tropeco e apenas um degrau a mais para a vitoria!",
                                     "                        Quem vive de navegar, o vento e quem lhe comanda                ",
                                     "    Uma vez me perguntaram o que achava da passagem do tempo, e eu disse: sou contra    ",
                                     "                 Nao importa o quanto voce va devagar, desde que nao pare                ",
                                     "Um simulador que resolve uma parada de producao, comeca avancando pequenos incrementos de tempo",
                                     "                            Nada e permanente, exceto a mudanca                           ",
                                     "                  Uma jornada de mil quilometros comeca com um unico passo                ",
                                     "Seja paciente. Espere ate que a lama assente e a agua fique limpa. Permaneça imovel ate que a acao correta suja por si so"};
    static constexpr const char *saidaSubTextoSis[16] = {
        "                         Galeno de Pergamo do Transiente Longo                          ",
        "                     J. California Cooper depois da simulacao divergir                  ",
        "                                Marcel Proust no CrossFit                               ",
        "                              Seneca do Mindfulness                                     ",
        "                                   Zuleica da Funeraria                                 ",
        "                                       China In Box                                     ",
        "                                      Tiao do Linkedin                                  ",
        "                                      Marcao da Oficina                                 ",
        "                                    Mario Pascal do Insta                               ",
        "                          Seu Pereira na feira de artesanatos numericos                 ",
        "                        Luis Fernando Verissimo das Simulacoes Permanentes              ",
        "                             Confucio vendo a simulacao emperrar                       ",
        "                               Confucio das simulacoes sem fim                         ",
        "          Heraclito de Efeso vendo tudo mudar a cada incremento de tempo                ",
        "    Lao-Tse tomando coragem para simular um caso de parafinacao em dutos de producao    ",
        "            Lao-Tse, vendo o incremento de tempo ficar cada vez menor                   "};
};

#endif /* SISPROD_H_ */
