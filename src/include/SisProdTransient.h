#ifndef SISPRODTRANSIENT_H_
#define SISPRODTRANSIENT_H_

// Non-owning state views use forward-declared model types.
class Cel;
class CelG;
class Ler;
class choke;
class ChokeGas;
class ProFlu;
class solverP3D;
struct SimulationContext;
class SProd;
template <class T> class Vcr;
template <class T> class FullMtx;
template <class T> class BandMtx;

#include "RuntimeDiagnostics.h"

#include <chrono>
#include <deque>
#include <string>
#include <vector>

namespace sisprod::thermal { struct ThermalState; struct ThermalDependencies; }
namespace sisprod::gaslift { struct GasLiftState; }

namespace sisprod::fluidtracking { struct FluidTrackingState; }
namespace trendoutput { struct TrendState; }

namespace sisprod {

namespace transient {

/// Invokes the hydrate-envelope solvers of the owning production system.
class HydrateEnvelopeSolver {
  public:
    explicit HydrateEnvelopeSolver(SProd &system) : system_(system) {}
    /// Runs the production and service-line solvers at the current step position.
    void operator()() const;

  private:
    SProd &system_;
};

/// Views of the physical models and trend writer used by transient orchestration.
struct TransientDependencies {
    const thermal::ThermalState &thermal;
    const thermal::ThermalDependencies &thermalDependencies;
    const gaslift::GasLiftState &gasLift;
    const fluidtracking::FluidTrackingState &fluidTracking;
    const trendoutput::TrendState &trends;
    HydrateEnvelopeSolver hydrates;
};

/// Live non-owning bindings used by transient cell and time-step operations.
/// Scalar and pointer references observe updates made by collaborating modules;
/// array pointers address storage owned by the production system.
struct TransientStepState {
    /// SProd::transient.meanMaximumTemperatureRate -- written.
    double &meanMaximumTemperatureRate;

    /// SProd::transient.meanMaximumPressureRate -- written.
    double &meanMaximumPressureRate;

    /// SProd::transient.masterValveState -- written.
    int &masterState;

    /// SProd::transient.surfaceChokeOpen -- written.
    int &surfaceChokeOpen;

    /// SProd::transient.previousSurfaceChokeOpen -- written.
    int &initiallyOpen;

    /// SProd::transient.adjustTimeStep -- written.
    int &timeChanged;

    /// SProd::transient.artificialGasThisStep -- written.
    std::vector<diagnostics::ArtificialGasAddition> &artificialGasThisStep;

    /// SProd::betaE -- written.
    double &inletComplementaryFraction;

    /// SProd::gasLift.interfaceCell -- written.
    int &interfaceCell;

    /// SProd::transient.masterValveCounter -- written.
    int &masterCounter;

    /// SProd::tables.gasSpecificHeatTable -- bound storage, not accessed by this implementation.
    double **const &gasSpecificHeatTable;

    /// SProd::dt -- written.
    double &timeStep;

    /// SProd::transient.meanCflTimeStep -- written.
    double &meanCflTimeStep;

    /// SProd::transient.totalCflTimeStep -- written.
    double &totalCflTimeStep;

    /// SProd::gasLift.interfaceTimeStep -- written.
    double &interfaceTimeStep;

    /// SProd::transient.meanAcceptedTimeStep -- written.
    double &meanSimulationTimeStep;

    /// SProd::transient.totalAcceptedTimeStep -- written.
    double &totalSimulationTimeStep;

    /// SProd::transient.bufferedComplementaryMassSource -- written.
    double &bufferedComplementaryMassSource;

    /// SProd::transient.bufferedGasMassSource -- written.
    double &bufferedGasMassSource;

    /// SProd::transient.bufferedLiquidMassSource -- written.
    double &bufferedLiquidMassSource;

    /// SProd::transient.eventIndex -- written.
    int &eventIndex;

    /// SProd::transient.slugCount -- written.
    int &slugCount;

    /// SProd::transient.restrictedStepsLeft -- written.
    int &timeStepRestrictionCount;

    /// SProd::masChkSup -- written.
    int &surfaceChokeMassFlag;

    /// SProd::transient.fullModel -- written.
    int &fullModel;

    /// SProd::transient.artificialGasMultiplier -- written.
    double &desperationMoment;

    /// SProd::transient.chokeModeChanged -- written.
    bool &chokeModeChanged;

    /// SProd::transient.cflSafetyFactor -- written.
    double &multiplier;

    /// SProd::presfim -- written.
    double &outletPressure;

    /// SProd::transient.rollbackRequested -- written.
    int &restart;

    /// SProd::transient.timeStepRestricted -- written.
    bool &timeStepRestricted;

    /// SProd::transient.activeChokeDelay -- written.
    int &openTime;

    /// SProd::transient.rightHandSide -- written.
    Vcr<double> &productionSolution;

    /// SProd::titE -- written.
    double &inletQuality;

    /// SProd::transient.valveAreaRatiosBeforeUpdate -- written.
    double *valveAreaRatiosBeforeUpdate;

    /// SProd::transient.valveAreaRatiosAfterUpdate -- written.
    double *valveAreaRatiosAfterUpdate;

    /// SProd::transient.masterValveCriticalRatios -- written.
    double *masterCriticalRatio;

    /// SProd::gasLift.interfaceVelocity -- written.
    double &interfaceVelocity;

    /// SProd::transient.movingPigCount -- written.
    int &movingPigCount;

    /// SProd::transient.previousMovingPigCount -- written.
    int &previousMovingPigCount;

    /// SProd::transient.masterValveOpenTimes -- read only.
    std::vector<double> &masterOpenSchedule;

    /// SProd::arq -- mutable access.
    Ler &input;

    /// SProd::gasLift.previousInterfaceCell -- read only.
    const int &initialInterfaceCell;

    /// SProd::celula -- fixed pointer binding with mutable cell access.
    Cel *const &cells;

    /// SProd::gasLift.cells -- fixed pointer binding with mutable cell access.
    CelG *const &gasCells;

    /// SProd::chokeSup -- mutable access.
    choke &surfaceChoke;

    /// SProd::transient.cflTimeSteps -- mutable access.
    std::vector<double> &cflTimeSteps;

    /// SProd::gasLift.previousInterfaceTimeStep -- read only.
    const double &initialInterfaceTimeStep;

    /// SProd::transient.acceptedTimeSteps -- mutable access.
    std::vector<double> &simulationTimeSteps;

    /// SProd::transient.auxiliaryCflTimeStep -- mutable access.
    double &auxiliaryCflTimeStep;

    /// SProd::transient.auxiliaryAcceptedTimeStep -- mutable access.
    double &finalAuxiliaryTimeStep;

    /// SProd::transient.masterValveCloseTimes -- read only.
    std::vector<double> &masterCloseSchedule;

    /// SProd::flut -- read only.
    FullMtx<double> &productionFreeTerms;

    /// SProd::flutG -- read only.
    FullMtx<double> &gasFreeTerms;

    /// SProd::indTramo -- read only.
    const int &branchIndex;

    /// SProd::transient.movingMeanFlux -- mutable access.
    double &movingMeanFlux;

    /// SProd::transient.stepCount -- mutable access.
    int &stepIndex;

    /// SProd::transient.systemMatrix -- mutable access.
    BandMtx<double> &productionMatrix;

    /// SProd::transient.smallestCellLength -- read only.
    const double &smallestCellLength;

    /// The number of scheduled Master1 opening events, SProd::transient.masterValveOpenTimes.size().
    int masterOpenCount;

    /// SProd::ncel -- read only.
    const int &lastCell;

    /// SProd::gasLift.cellCount -- read only.
    const int &lastGasCell;

    /// SProd::trends.productionWallProfileCells -- read only.
    std::vector<int> &productionCrossSectionCount;

    /// The number of scheduled Master1 closing events, SProd::transient.masterValveCloseTimes.size().
    int masterCloseCount;

    /// SProd::noextremo -- read only.
    const int &endNode;

    /// SProd::gasLift.separatorPressure -- mutable access.
    double &gasSurfacePressure;

    /// SProd::presE -- mutable access.
    double &inletPressure;

    /// SProd::transient.movingMeanPressure -- mutable access.
    double &movingMeanPressure;

    /// SProd::transient.movingMeanStartTime -- read only.
    const double &movingMeanStartTime;

    /// SProd::transient.maximumTemperatureRates -- mutable access.
    std::vector<double> &maximumTemperatureRates;

    /// SProd::transient.maximumPressureRates -- mutable access.
    std::vector<double> &maximumPressureRates;

    /// SProd::tempE -- mutable access.
    double &inletTemperature;

    /// SProd::networkCoupling.reverseQuality -- read only.
    const double &reverseQuality;

    /// SProd::gasLift.previousInterfaceVelocity -- read only.
    const double &initialInterfaceVelocity;

    /// SProd::transient.pigReceiverCells -- read only.
    std::vector<int> &pigReceiverCells;

    /// The number of pigs, each with its receiver cell: SProd::transient.pigReceiverCells.size().
    int scheduledPigCount;

    /// SProd::simulationContext -- fixed pointer binding with mutable global-state access.
    SimulationContext *const &globals;

};

/// Updates cells in increasing index order, using the first, interior, and outlet rules.
void updateCells(const TransientStepState &state, int expli = 0);

/// Updates an interior cell and its adjacent faces; i is an interior index.
void updateInteriorCell(const TransientStepState &state, int i, int expli);

/// Updates inlet state, snapshots, and flow properties; i is the inlet index zero.
void updateFirstCell(const TransientStepState &state, int i, int expli);

/// Updates outlet state, pressure correction, and flows; i equals lastCell.
void updateLastCell(const TransientStepState &state, int i, int expli);

/// Refreshes phase flow rates and adjacent faces without taking full cell snapshots.
void updateFlowRates(const TransientStepState &state);

/// Copies solver values into the inlet, first interior, and outlet buffers,
/// including the outlet right face; mass-source buffers are unchanged.
void updateBufferFromSolution(const TransientStepState &state);

/// Copies cell values into the inlet, first interior, and outlet buffers,
/// then stores outlet mass sources; the outlet right-face buffers are unchanged.
void updateBufferFromCells(const TransientStepState &state);

/// Tests whether the throat area is strictly above one thousandth of the pipe area.
[[nodiscard]] bool surfaceChokeIsOpen(const TransientStepState &state);

/// Tests whether the throat area is strictly below one thousandth of the pipe area;

/// equality satisfies neither this predicate nor surfaceChokeIsOpen.
[[nodiscard]] bool surfaceChokeIsShut(const TransientStepState &state);

/// Applies the current-state outlet condition and choke-temperature correction.
/// titRev and alfRev are accepted but unused; betRev supplies the reverse liquid fraction.
void applyOutletPressureCondition(const TransientStepState &state, double titRev, double alfRev, double betRev);

/// Applies the buffered outlet condition with the supplied reverse-flow mixture.
void applyOutletBufferCondition(const TransientStepState &state, double titRev, double alfRev, double betRev);

/// Chooses and propagates the explicit or implicit time step after initial output
/// and porous-flow restrictions; its exact value determines the temporal mesh.
void computeTimeStep(const TransientStepState &state, int vexpli = 0);

/// Limits the time step by the cell wave speeds and propagates cell increments.
void computeExplicitTimeStep(const TransientStepState &state);

/// Applies ordered choke, transport, event, and valve controls to the time step.
void computeImplicitTimeStep(const TransientStepState &state, int explicitScheme = 0);

/// Updates the accepted/CFL histories and their fixed-denominator restriction rule.
void dampMaximumTimeStep(const TransientStepState &state);

/// Assesses block pressure and temperature rates, their histories, and model selection.
void evaluatePressureRateOfChange(const TransientStepState &state, int vexpli);

/// Restricts the inlet-cell increment during valve closing and records artificial gas additions.
void restrictTimeStepByValve(const TransientStepState &state);

/// Captures the current master and accessory valve area ratios in destination,
/// which must provide at least input.nvalv + 1 entries.
void captureValveAreaRatios(const TransientStepState &state, double *destination);

/// Advances interior void and complementary-liquid fractions in parallel, applying
/// each outlet condition after its interior sweep completes, then advances pig fractions.
/// Collects cell restart flags serially after all fraction updates complete.
/// betrev and ciclo are accepted but unused; alfrev supplies the reverse void fraction.
void evolveFractions(const TransientStepState &state, double alfrev = 1., double betrev = 0., int ciclo = 0);

/// Reduces the current global time step using each cell's general, void fraction,
/// complementary liquid and pig movement limits, in that order.
/// Includes the outlet cell and keeps the step unless a limit is strictly smaller.
void restartFractionEvolutionInitial(const TransientStepState &state);

/// Restores void and pig void fractions where cell pressure exceeds -10.
void restartFractionEvolutionSub(const TransientStepState &state);

/// Propagates the reduced increment, advances the restored gas sub-step,
/// and restores void, complementary-liquid, and pig fractions in order.
void restartFractionEvolution(const TransientStepState &state, const gaslift::GasLiftState &gasLiftDependencies, const thermal::ThermalState &thermalDependencies);

/// Snapshots pig movement state, transfers pigs between cells, and prepares face liquid fractions.
void updatePig(const TransientStepState &state);

/// Builds each cell's pressure-volume equations in parallel, writing its two exclusive
/// matrix rows and right-hand-side entries, then solves the band system serially.
/// All cell assemblies complete before the solve. Configuration, fluid tables and
/// shared input data must remain stable throughout assembly; concurrent calls must
/// use distinct cell arrays, matrices and right-hand-side storage.
void solvePressureVolumeCoupling(const TransientStepState &state, int vexpli = 0, int ciclo = 0);

/// Generates fluid mini-tables and initializes the interior-cell mole inventories.
void refreshFluidMiniTable(const TransientStepState &state);

/// Updates active pressure-boundary quality and, for the gas-dominated model, the
/// complementary fraction. Unhandled model combinations retain their current values.
void refreshInletCondition(const TransientStepState &state);

/// Recentres cell and accessory mini-tables and evaluates enabled tables in parallel.
void generateFluidMiniTables(const TransientStepState &state);

/// Evaluates four ordered composition corners and supplies properties for nearly absent phases.
void fillFluidMiniTable(const TransientStepState &state, ProFlu &flui);

/// Copies the three outlet-face mass sources to their previous-time-level fields in every cell.
void storePreviousSources(const TransientStepState &state);

/// Evaluates cell wax deposition and updates outlet wall and heat-transfer properties.
void evaluateWaxDeposition(const TransientStepState &state);

/// Stores previous fractions and pig state, computes pig velocities, and receives pigs.
void storePreviousFractionsAndMovePigs(const TransientStepState &state);

/// Caches cell and upwind-face densities and cell viscosities before serial face propagation.
void cacheCellAndFaceDensities(const TransientStepState &state);

/// Live step bindings and the additional histories and output buffers
/// required to orchestrate one transient solve. References retain their owner storage.
struct TransientSolveState {
    /// State shared with cell, fraction, and time-step operations.
    TransientStepState step;

    /// SProd::temperatura -- written or read by the solve; not promised const.
    double &defaultInletTemperature;

    /// SProd::derivaAnel -- written or read by the solve; not promised const.
    int &annulusDrift;

    /// SProd::saidaSubTextoSis -- read only.
    const char *const *closingSubtitles;

    /// SProd::saidaTextoSis -- read only.
    const char *const *closingTitles;

    /// SProd::transient.compositionalRefreshCounter -- written or read by the solve; not promised const.
    int &compositionalRefreshCounter;

    /// SProd::transient.fluxSamples -- written or read by the solve; not promised const.
    std::deque<double> &fluxHistory;

    /// SProd::trends.gasWallProfileCells -- written or read by the solve; not promised const.
    std::vector<int> &gasCrossSectionCellCounts;

    /// SProd::trends.gasWallProfileTimeIndex -- written or read by the solve; not promised const.
    int &gasCrossSectionProfileTimeCounter;

    /// SProd::trends.gasWallTrendBufferedCounts -- written or read by the solve; not promised const.
    int* &gasCrossSectionTrendBufferedCounts;

    /// SProd::trends.gasWallTrendCounts -- written or read by the solve; not promised const.
    int* &gasCrossSectionTrendCounts;

    /// SProd::trends.gasWallTrendMatrices -- written or read by the solve; not promised const.
    double*** &gasCrossSectionTrendMatrix;

    /// SProd::trends.gasWallTrendResetTimes -- written or read by the solve; not promised const.
    double* &gasCrossSectionTrendResetTimers;

    /// SProd::trends.gasProfileTimeIndex -- written or read by the solve; not promised const.
    int &gasProfileTimeCounter;

    /// SProd::trends.gasTrendBufferedCounts -- written or read by the solve; not promised const.
    int* &gasTrendBufferedCounts;

    /// SProd::trends.gasTrendCounts -- written or read by the solve; not promised const.
    int* &gasTrendCounts;

    /// SProd::trends.gasTrendMatrices -- written or read by the solve; not promised const.
    double*** &gasTrendMatrix;

    /// SProd::trends.gasTrendResetTimes -- written or read by the solve; not promised const.
    double* &gasTrendResetTimers;

    /// SProd::gasLift.injectionPressure -- written or read by the solve; not promised const.
    double &initialGasPressure;

    /// SProd::transient.previousSeparatorPressure -- written or read by the solve; not promised const.
    double &initialGasSurfacePressure;

    /// SProd::gasLift.injectionTemperature -- written or read by the solve; not promised const.
    double &initialGasTemperature;

    /// SProd::transient.previousActiveChokeDelay -- written or read by the solve; not promised const.
    int &initialOpenTime;

    /// SProd::gasLift.injectionChoke -- written or read by the solve; not promised const.
    ChokeGas &injectionChoke;

    /// SProd::tmpLog -- written or read by the solve; not promised const.
    std::string &logBuffer;

    /// SProd::transient.eventLogCount -- written or read by the solve; not promised const.
    int &logCounter;

    /// SProd::transient.massTransferModel -- written or read by the solve; not promised const.
    int &massTransferModel;

    /// SProd::transient.minimumCycleTimeStep -- written or read by the solve; not promised const.
    double &minimumCycleTimeStep;

    /// SProd::transient.movingMeanWindow -- written or read by the solve; not promised const.
    double &movingMeanCounter;

    /// SProd::transient.movingMeanVoidFraction -- written or read by the solve; not promised const.
    double &movingMeanVoidFraction;

    /// SProd::gasLift.annulusThermalCoupling -- written or read by the solve; not promised const.
    bool &networkCoupled;

    /// SProd::coupling3D.poissonSolver3D -- written or read by the solve; not promised const.
    solverP3D &poissonSolver3D;

    /// SProd::transient.pressureSamples -- written or read by the solve; not promised const.
    std::deque<double> &pressureHistory;

    /// SProd::transient.printCounter -- written or read by the solve; not promised const.
    int &printCounter;

    /// SProd::trends.trendOutputCount -- written or read by the solve; not promised const.
    int &printPassCount;

    /// SProd::trends.productionWallProfileTimeIndex -- written or read by the solve; not promised const.
    int &productionCrossSectionProfileTimeCounter;

    /// SProd::trends.productionWallTrendBufferedCounts -- written or read by the solve; not promised const.
    int* &productionCrossSectionTrendBufferedCounts;

    /// SProd::trends.productionWallTrendCounts -- written or read by the solve; not promised const.
    int* &productionCrossSectionTrendCounts;

    /// SProd::trends.productionWallTrendMatrices -- written or read by the solve; not promised const.
    double*** &productionCrossSectionTrendMatrix;

    /// SProd::trends.productionWallTrendResetTimes -- written or read by the solve; not promised const.
    double* &productionCrossSectionTrendResetTimers;

    /// SProd::trends.productionProfileTimeIndex -- written or read by the solve; not promised const.
    int &productionProfileTimeCounter;

    /// SProd::trends.productionTrendBufferedCounts -- written or read by the solve; not promised const.
    int* &productionTrendBufferedCounts;

    /// SProd::trends.productionTrendCounts -- written or read by the solve; not promised const.
    int* &productionTrendCounts;

    /// SProd::trends.productionTrendMatrices -- written or read by the solve; not promised const.
    double*** &productionTrendMatrix;

    /// SProd::trends.productionTrendResetTimes -- written or read by the solve; not promised const.
    double* &productionTrendResetTimers;

    /// SProd::noinicial -- written or read by the solve; not promised const.
    int &startNode;

    /// SProd::transient.timeSamples -- written or read by the solve; not promised const.
    std::deque<double> &timeStepHistory;

    /// SProd::transient.accumulatedFlux -- written or read by the solve; not promised const.
    double &totalFlux;

    /// SProd::transient.accumulatedPressure -- written or read by the solve; not promised const.
    double &totalPressure;

    /// SProd::transient.accumulatedVoidFraction -- written or read by the solve; not promised const.
    double &totalVoidFraction;

    /// SProd::transient.trackGasGravity -- written or read by the solve; not promised const.
    int &trackGasGravity;

    /// SProd::transient.trackGasOilRatio -- written or read by the solve; not promised const.
    int &trackGasOilRatio;

    /// SProd::trends.singleCellOutputTimeIndices -- written or read by the solve; not promised const.
    std::vector<int> &unitCellTimeCounters;

    /// SProd::transient.voidFractionSamples -- written or read by the solve; not promised const.
    std::deque<double> &voidFractionHistory;

};

/// Advances and reports one transient step, preserving the ordered coupling,
/// property, history, and output phases. Standalone lines advance their physics
/// here; network callers supply an advanced state for the common finalization.
/// Trend flushing is selected before synchronizing the network clock. Only
/// standalone lines advance simulationTime at the end of this call.
void solveTransientStep(const TransientSolveState &state, const TransientDependencies &dependencies,
                        double reverseQuality, double reverseVoidFraction, double reverseLiquidFraction,
                        int networkIndex, const ProFlu *reverseFluid);

}  // namespace transient
}  // namespace sisprod

#endif  // SISPRODTRANSIENT_H_
