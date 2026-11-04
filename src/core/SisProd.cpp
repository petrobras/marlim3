#include "DriftFluxClosure.h"
#include "SisProd.h"
#include "FA_Hidratos.h"
#include "FA_Hidratos_Servico.h"
#include "SisProdConstants.h"
#include "SisProdGasLift.h"
#include "SisProdSources.h"
#include "SisProdSteadyState.h"
#include "SisProdSteadyStateSearch.h"
#include "SisProdTransient.h"
#include "SisProdFluidTracking.h"
#include "SisProdThermal.h"
#include "SisProdTrendOutput.h"
#include <stdexcept>

using namespace sisprod;

struct SProd::SteadyCallState {
    driftflux::coefficient::ClosureState closure;
    sources::SourceState sources;
    thermal::ThermalState thermal;
    gaslift::GasLiftState gasLift;
    steady::SteadyStateSearchState search;

    thermal::ThermalDependencies thermalDependencies() const { return {sources, closure}; }
    steady::SteadyDependencies dependencies(const thermal::ThermalDependencies &thermalModels) const {
        return {sources, closure, thermal, thermalModels, gasLift, search};
    }
};

struct SProd::TransientCallState {
    driftflux::coefficient::ClosureState closure;
    sources::SourceState sources;
    thermal::ThermalState thermal;
    gaslift::GasLiftState gasLift;
    fluidtracking::FluidTrackingState fluidTracking;
    trendoutput::TrendState trends;
    transient::TransientSolveState solve;

    thermal::ThermalDependencies thermalDependencies() const { return {sources, closure}; }
    transient::TransientDependencies dependencies(const thermal::ThermalDependencies &thermalModels,
                                                 transient::HydrateEnvelopeSolver hydrates) const {
        return {thermal, thermalModels, gasLift, fluidTracking, trends, hydrates};
    }
};

SProd::SProd(string inputFile, string logFile, tipoValidacaoJson_t jsonValidation,
             tipoSimulacao_t simulationType, SimulationContext *globals, int dynamicTableOverride, int blocking,
             int reversedGeometry, double *extraSourceLengths, int *extraSourceCells, int extraSourceCount,
             int steadyBranch)
    : arq(inputFile, logFile, jsonValidation, simulationType, reversedGeometry, globals, steadyBranch),
      simulationContext(globals) {
    if (dynamicTableOverride >= 0)
        arq.tabelaDinamica = dynamicTableOverride;
    assemble({.bloq = blocking, .previousReverseLiquidFraction = 0.}, extraSourceLengths, extraSourceCells,
             extraSourceCount);
}

SProd::SProd() = default;

SProd::SProd(Ler &parsedInput, const CarriedState &carried) : simulationContext(parsedInput.simulationContext) {
    arq.copiaSemJson(parsedInput);
    assemble(carried);
}

void SProd::loadSection(const SProd &other) {
    if (celula != nullptr)
        throw std::logic_error("SProd::loadSection: this system was already assembled");
    arq = other.arq;
    simulationContext = other.simulationContext;
    networkCoupling.takeParallelRole(other.networkCoupling);
    assemble(other.carriedState());
}

SProd::CarriedState SProd::carriedState() const {
    return {.noextremo = noextremo,
            .noinicial = noinicial,
            .derivaAnel = derivaAnel,
            .bloq = bloq,
            .previousReverseLiquidFraction = networkCoupling.previousReverseLiquidFraction,
            .previousReverseQuality = networkCoupling.previousReverseQuality,
            .minimumCycleTimeStep = transient.minimumCycleTimeStep};
}

void SProd::renova(int explicitScheme) {
    const auto transientStep = transientStateOf();
    transient::updateCells(transientStep, explicitScheme);
}

void SProd::renovaBuffer() {
    const auto transientStep = transientStateOf();
    transient::updateBufferFromSolution(transientStep);
}

void SProd::renovaBufferCego() {
    const auto transientStep = transientStateOf();
    transient::updateBufferFromCells(transientStep);
}

void SProd::renovaTemp() {
    const auto state = thermalStateOf();
    const auto sources = sourceStateOf();
    const auto closure = closureStateOf();
    const thermal::ThermalDependencies dependencies{sources, closure};
    thermal::updateDistributedMassTransfer(state, dependencies);
}

void SProd::renovaterm(int inflowMode, int explicitMode) {
    const auto state = thermalStateOf();
    const auto sources = sourceStateOf();
    const auto closure = closureStateOf();
    const thermal::ThermalDependencies dependencies{sources, closure};
    thermal::updateFlowPartitionTerms(state, dependencies, inflowMode, explicitMode);
}

void SProd::renovatermAfluFim() {
    const auto state = thermalStateOf();
    const auto sources = sourceStateOf();
    const auto closure = closureStateOf();
    const thermal::ThermalDependencies dependencies{sources, closure};
    thermal::updateOutletFlowPartitionTerms(state, dependencies);
}

void SProd::renovatermColIni() {
    const auto state = thermalStateOf();
    const auto sources = sourceStateOf();
    const auto closure = closureStateOf();
    const thermal::ThermalDependencies dependencies{sources, closure};
    thermal::updateInletFlowPartitionTerms(state, dependencies);
}

void SProd::calcCCpres(double reverseQuality, double reverseVoidFraction, double reverseLiquidFraction) {
    const auto transientStep = transientStateOf();
    transient::applyOutletPressureCondition(transientStep, reverseQuality, reverseVoidFraction,
                                            reverseLiquidFraction);
}

void SProd::calcCCBuffer(double reverseQuality, double reverseVoidFraction, double reverseLiquidFraction) {
    const auto transientStep = transientStateOf();
    transient::applyOutletBufferCondition(transientStep, reverseQuality, reverseVoidFraction,
                                          reverseLiquidFraction);
}

void SProd::determinaDT(int explicitScheme) {
    const auto transientStep = transientStateOf();
    transient::computeTimeStep(transientStep, explicitScheme);
}

void SProd::atenuaDtMax() {
    const auto transientStep = transientStateOf();
    transient::dampMaximumTimeStep(transientStep);
}

void SProd::avaliaVariaDpDt(int explicitScheme) {
    const auto transientStep = transientStateOf();
    transient::evaluatePressureRateOfChange(transientStep, explicitScheme);
}

void SProd::captureValveAreaRatios(sisprod::ValveSnapshotPhase phase) {
    const auto transientStep = transientStateOf();
    double *destination = phase == ValveSnapshotPhase::BeforeBoundaryUpdate
                              ? transientStep.valveAreaRatiosBeforeUpdate
                              : transientStep.valveAreaRatiosAfterUpdate;
    transient::captureValveAreaRatios(transientStep, destination);
}

void SProd::restringeDTporValv() {
    const auto transientStep = transientStateOf();
    transient::restrictTimeStepByValve(transientStep);
}

void SProd::solveLinGas() {
    const auto gasLift = gasLiftStateOf();
    const auto thermal = thermalStateOf();
    gaslift::solveGasLine(gasLift, thermal);
}

void SProd::EvoluiFrac(double reverseVoidFraction, double reverseLiquidFraction, int cycle) {
    const auto transientStep = transientStateOf();
    transient::evolveFractions(transientStep, reverseVoidFraction, reverseLiquidFraction, cycle);
}

void SProd::ReiniEvolFrac0() {
    const auto transientStep = transientStateOf();
    transient::restartFractionEvolutionInitial(transientStep);
}

void SProd::ReiniEvolFrac() {
    const auto state = transientStateOf();
    const auto gasLift = gasLiftStateOf();
    const auto thermal = thermalStateOf();
    transient::restartFractionEvolution(state, gasLift, thermal);
}

void SProd::AtualizaPig() {
    const auto transientStep = transientStateOf();
    transient::updatePig(transientStep);
}

void SProd::SolveAcopPV(int explicitScheme, int cycle) {
    const auto transientStep = transientStateOf();
    transient::solvePressureVolumeCoupling(transientStep, explicitScheme, cycle);
}

void SProd::marchaEnergTrans(int cycle, int maximumCycle) {
    const auto state = thermalStateOf();
    const auto step = transientStateOf();
    thermal::advanceTransientEnergy(state, step, cycle, maximumCycle);
}

void SProd::atualizaMiniTab() {
    const auto transientStep = transientStateOf();
    transient::refreshFluidMiniTable(transientStep);
}

void SProd::atualizaCC1() {
    const auto transientStep = transientStateOf();
    transient::refreshInletCondition(transientStep);
}

void SProd::SolveTrans(double reverseQuality, double reverseVoidFraction, double reverseLiquidFraction, int network,
                       const ProFlu *collectorFluid) {
    const auto models = transientCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies, transient::HydrateEnvelopeSolver{*this});
    transient::solveTransientStep(models.solve, dependencies, reverseQuality, reverseVoidFraction, reverseLiquidFraction,
                                  network, collectorFluid);
}

void SProd::ImprimeTrendP(int trend, int network) {
    const auto trends = trendStateOf();
    trendoutput::writeProductionTrendRows(trends, trend, network);
}

void SProd::ImprimeTrendPCab(int trend, int network) {
    const auto trends = trendStateOf();
    trendoutput::writeProductionTrendHeader(trends, trend, network);
}

void SProd::ImprimeTrendG(int trend, int network) {
    const auto trends = trendStateOf();
    trendoutput::writeGasLineTrendRows(trends, trend, network);
}

void SProd::ImprimeTrendGCab(int trend, int network) {
    const auto trends = trendStateOf();
    trendoutput::writeGasLineTrendHeader(trends, trend, network);
}

double SProd::marchaProdPerm1(double pressureGuess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::marchProductionSteady(models.search.march, dependencies, pressureGuess);
}

double SProd::buscaProdPfundoPerm(double guess, int attemptCount) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchProductionBottomHolePressure(models.search, dependencies, guess, attemptCount);
}

double SProd::buscaProdPfundoPermRev(double guess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchReverseProductionBottomHolePressure(models.search, dependencies, guess);
}

double SProd::buscaProdPfundoPerm2(double guess, int attemptCount) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchProductionBottomHolePressureSecondary(models.search, dependencies, guess, attemptCount);
}

double SProd::buscaProdPfundoPerm3(double inletPressure) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchProductionBottomHolePressureTertiary(models.search, dependencies, inletPressure);
}

double SProd::marchaProdPresPres1(double massFlowRateGuess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::marchProductionPressureToPressure(models.search.march, dependencies, massFlowRateGuess);
}

double SProd::buscaProdPresPresPerm(double massFlowRateGuess, double maximumFlowRate, int iterationCount) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchProductionPressureToPressure(models.search, dependencies, massFlowRateGuess, maximumFlowRate,
                                                      iterationCount);
}

double SProd::buscaProdPresPresPermRev(double massFlowRateGuess, double maximumFlowRate, int iterationCount) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchReverseProductionPressureToPressure(models.search, dependencies, massFlowRateGuess, maximumFlowRate,
                                                             iterationCount);
}

double SProd::buscaProdPresPresPerm2(double massFlowRateGuess, double maximumFlowRate) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchProductionPressureToPressureSecondary(models.search, dependencies, massFlowRateGuess, maximumFlowRate);
}

double SProd::buscaProdPresPresPerm3(double massFlowRateGuess, double maximumFlowRate) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchProductionPressureToPressureTertiary(models.search, dependencies, massFlowRateGuess, maximumFlowRate);
}

double SProd::buscaInjPfundoPerm1(double guess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchInjectionBottomHolePressure1(models.search, dependencies, guess);
}

double SProd::buscaInjPfundoPerm2(double guess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchInjectionBottomHolePressure2(models.search, dependencies, guess);
}

double SProd::buscaInjPfundoPerm3(double guess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchInjectionBottomHolePressure3(models.search, dependencies, guess);
}

double SProd::buscaInjPfundoPerm4() {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchInjectionBottomHolePressure4(models.search, dependencies);
}

double SProd::buscaInjPfundoPerm5(double guess) {
    const auto models = steadyCallStateOf();
    const auto thermalDependencies = models.thermalDependencies();
    const auto dependencies = models.dependencies(thermalDependencies);
    return steady::searchInjectionBottomHolePressure5(models.search, dependencies, guess);
}

double SProd::hidroreverso(double liquidHoldup, double liquidFlowRate, double gasFlowRate) {
    const auto steady = steadyStateOf();
    return steady::reverseHydrostatic(steady, liquidHoldup, liquidFlowRate, gasFlowRate);
}

double SProd::hidroreversoInj(double liquidHoldup, double liquidFlowRate) {
    const auto steady = steadyStateOf();
    return steady::reverseInjectionHydrostatic(steady, liquidHoldup, liquidFlowRate);
}

double SProd::hidroTramoSecundario(double quality) {
    const auto steady = steadyStateOf();
    return steady::secondaryBranchHydrostatic(steady, quality);
}

double SProd::surfaceChokeOutletTemperature() {
    return thermal::surfaceChokeOutletTemperature(thermalStateOf());
}

void SProd::calcTempFim() {
    const auto thermal = thermalStateOf();
    thermal::computeOutletTemperature(thermal);
}

void SProd::assemble(const CarriedState &carried, double *extraSourceLengths, int *extraSourceCells,
                     int extraSourceCount) {
    tables = PropertyTables();
    steadySearch = SteadySearch();
    allocateSolutionBuffers();
    applyCarriedState(carried);
    montasistema(extraSourceLengths, extraSourceCells, extraSourceCount);
}

void SProd::allocateSolutionBuffers() {
    flut = FullMtx<double>(arq.ncelp, arq.nvarprofp + kProductionProfileFixedColumns);
    flutG = FullMtx<double>(arq.ncelg, arq.nvarprofg + kGasLineProfileFixedColumns);
    transient.systemMatrix =
        BandMtx<double>(kProductionUnknownsPerCell * arq.ncelp, kProductionLeftBandwidth, kProductionRightBandwidth);
    transient.rightHandSide = Vcr<double>(kProductionUnknownsPerCell * arq.ncelp);
    gasLift.systemMatrix = BandMtx<double>(kGasLineUnknownsPerCell * arq.ncelg, kGasLineBandwidth, kGasLineBandwidth);
    gasLift.rightHandSide = Vcr<double>(kGasLineUnknownsPerCell * arq.ncelg);
}

void SProd::applyCarriedState(const CarriedState &carried) {
    noextremo = carried.noextremo;
    noinicial = carried.noinicial;
    derivaAnel = carried.derivaAnel;
    bloq = carried.bloq;
    networkCoupling.previousReverseLiquidFraction = carried.previousReverseLiquidFraction;
    networkCoupling.previousReverseQuality = carried.previousReverseQuality;
    transient.minimumCycleTimeStep = carried.minimumCycleTimeStep;
}

void SProd::HidroDescargaG() {
    const auto gasLift = gasLiftStateOf();
    gaslift::computeGasUnloadingHydrostatics(gasLift);
}

void SProd::HidroDescargaP() {
    const auto gasLift = gasLiftStateOf();
    gaslift::computeProductionUnloadingHydrostatics(gasLift);
}

bool SProd::hydrateEnvelopeDue() const {
    return arq.calculaEnvelope == 1 && simulationContext->simulationTime <= arq.tfinal;
}

void SProd::solveHydrateEnvelopes() {
    if (!hydrateEnvelopeDue())
        return;
    FA_Hidrato(*this).solverHidrato();
    if (arq.lingas > 0)
        FA_Hidrato_Servico(*this).solverHidratoG();
}

namespace {
// Some views bind a `const T *const &` field to a `T *` member. C++20 binds such
// a reference to the member itself; a temporary there would leave the view
// with a copy that dangles.
constexpr bool bindsToTheMember() {
    int *member = nullptr;
    const int *const &field = member;
    return &field == &member;
}
static_assert(bindsToTheMember());

}  // namespace

sisprod::gaslift::GasLiftState SProd::gasLiftStateOf() {
    return sisprod::gaslift::GasLiftState{
        .gasCells = gasLift.cells,
        .cells = celula,
        .input = arq,
        .globals = simulationContext,
        .lastGasCell = gasLift.cellCount,
        .lastCell = ncel,
        .gasLiftChokes = gasLift.valveChokes,
        .injectionChoke = gasLift.injectionChoke,
        .gasValveCellIndices = gasLift.gasValveCells,
        .productionValveCellIndices = gasLift.productionValveCells,
        .gasSystemMatrix = gasLift.systemMatrix,
        .gasRightHandSide = gasLift.rightHandSide,
        .tubingCouplingStart = gasLift.tubingCouplingStart,
        .tubingCouplingEnd = gasLift.tubingCouplingEnd,
        .annulusCouplingStart = gasLift.annulusCouplingStart,
        .annulusCouplingEnd = gasLift.annulusCouplingEnd,
        .annulusThermalCoupling = gasLift.annulusThermalCoupling,
        .thermalSourceDisabled = thermalSourceDisabled,
        .injectionPressure = gasLift.injectionPressure,
        .injectionTemperature = gasLift.injectionTemperature,
        .separatorPressure = gasLift.separatorPressure,
        .timeStep = dt,
        .interfaceCell = gasLift.interfaceCell,
        .interfaceVelocity = gasLift.interfaceVelocity,
        .interfaceTimeStep = gasLift.interfaceTimeStep,
        .previousInterfaceCell = gasLift.previousInterfaceCell,
        .previousInterfaceVelocity = gasLift.previousInterfaceVelocity,
        .previousInterfaceTimeStep = gasLift.previousInterfaceTimeStep,
        .accumulatedUnloadingVolume = gasLift.accumulatedUnloadingVolume,
        .unloadingAveragingInterval = gasLift.unloadingAveragingInterval,
        .maximumUnloadingFlowSamples = gasLift.maximumUnloadingFlowSamples,
        .unloadingTimeStepSamples = gasLift.unloadingTimeStepSamples,
        .unloadingAveragingWindow = gasLift.unloadingAveragingWindow,
        .unloadingSampleCapacity = gasLift.unloadingSampleCapacity,
    };
}

sisprod::steady::SteadyStateState SProd::steadyStateOf() {
    return sisprod::steady::SteadyStateState{
        .cells = celula,
        .gasCells = gasLift.cells,
        .input = arq,
        .globals = simulationContext,
        .lastCell = ncel,
        .lastGasCell = gasLift.cellCount,
        .injectionChoke = gasLift.injectionChoke,
        .surfaceChoke = chokeSup,
        .gasValveCellIndices = gasLift.gasValveCells,
        .productionValveCellIndices = gasLift.productionValveCells,
        .steadyIteration = steadyIteration,
        .searchOrigin = steadySearch.initialEstimateSearch,
        .convergenceMonitor = steadySearch.convergenceMonitor,
        .baseConvergenceMonitor = steadySearch.convergenceMonitorReference,
        .annulusDrift = derivaAnel,
        .networkCoupled = gasLift.annulusThermalCoupling,
        .endNode = noextremo,
        .thermalSourceDisabled = thermalSourceDisabled,
        .slowHeatTransferThreshold = steadySearch.slowHeatTransferThreshold,
        .gasSurfacePressure = gasLift.separatorPressure,
        .initialGasPressure = gasLift.injectionPressure,
        .initialGasTemperature = gasLift.injectionTemperature,
        .outletPressure = presfim,
        .timeStep = dt,
        .casingTemperature = steadySearch.reverseFlowTemperature,
        .inletQuality = titE,
        .productionFluidCount = steadySearch.productionFluidCount,
    };
}

sisprod::steady::SteadyStateSearchState SProd::searchStateOf() {
    return sisprod::steady::SteadyStateSearchState{
        .march = steadyStateOf(),
        .holdupGuess = steadySearch.holdupGuess,
        .reverseNetworkFluid = steadySearch.reverseNetworkFluid,
        .reverseSteady = steadySearch.steadyReverseFlow,
    };
}

sisprod::transient::TransientStepState SProd::transientStateOf() {
    return sisprod::transient::TransientStepState{
        .meanMaximumTemperatureRate = transient.meanMaximumTemperatureRate,
        .meanMaximumPressureRate = transient.meanMaximumPressureRate,
        .masterState = transient.masterValveState,
        .surfaceChokeOpen = transient.surfaceChokeOpen,
        .initiallyOpen = transient.previousSurfaceChokeOpen,
        .timeChanged = transient.adjustTimeStep,
        .artificialGasThisStep = transient.artificialGasThisStep,
        .inletComplementaryFraction = betaE,
        .interfaceCell = gasLift.interfaceCell,
        .masterCounter = transient.masterValveCounter,
        .gasSpecificHeatTable = tables.gasSpecificHeatTable,
        .timeStep = dt,
        .meanCflTimeStep = transient.meanCflTimeStep,
        .totalCflTimeStep = transient.totalCflTimeStep,
        .interfaceTimeStep = gasLift.interfaceTimeStep,
        .meanSimulationTimeStep = transient.meanAcceptedTimeStep,
        .totalSimulationTimeStep = transient.totalAcceptedTimeStep,
        .bufferedComplementaryMassSource = transient.bufferedComplementaryMassSource,
        .bufferedGasMassSource = transient.bufferedGasMassSource,
        .bufferedLiquidMassSource = transient.bufferedLiquidMassSource,
        .eventIndex = transient.eventIndex,
        .slugCount = transient.slugCount,
        .timeStepRestrictionCount = transient.restrictedStepsLeft,
        .surfaceChokeMassFlag = masChkSup,
        .fullModel = transient.fullModel,
        .desperationMoment = transient.artificialGasMultiplier,
        .chokeModeChanged = transient.chokeModeChanged,
        .multiplier = transient.cflSafetyFactor,
        .outletPressure = presfim,
        .restart = transient.rollbackRequested,
        .timeStepRestricted = transient.timeStepRestricted,
        .openTime = transient.activeChokeDelay,
        .productionSolution = transient.rightHandSide,
        .inletQuality = titE,
        .valveAreaRatiosBeforeUpdate = transient.valveAreaRatiosBeforeUpdate.data(),
        .valveAreaRatiosAfterUpdate = transient.valveAreaRatiosAfterUpdate.data(),
        .masterCriticalRatio = transient.masterValveCriticalRatios.data(),
        .interfaceVelocity = gasLift.interfaceVelocity,
        .movingPigCount = transient.movingPigCount,
        .previousMovingPigCount = transient.previousMovingPigCount,
        .masterOpenSchedule = transient.masterValveOpenTimes,
        .input = arq,
        .initialInterfaceCell = gasLift.previousInterfaceCell,
        .cells = celula,
        .gasCells = gasLift.cells,
        .surfaceChoke = chokeSup,
        .cflTimeSteps = transient.cflTimeSteps,
        .initialInterfaceTimeStep = gasLift.previousInterfaceTimeStep,
        .simulationTimeSteps = transient.acceptedTimeSteps,
        .auxiliaryCflTimeStep = transient.auxiliaryCflTimeStep,
        .finalAuxiliaryTimeStep = transient.auxiliaryAcceptedTimeStep,
        .masterCloseSchedule = transient.masterValveCloseTimes,
        .productionFreeTerms = flut,
        .gasFreeTerms = flutG,
        .branchIndex = indTramo,
        .movingMeanFlux = transient.movingMeanFlux,
        .stepIndex = transient.stepCount,
        .productionMatrix = transient.systemMatrix,
        .smallestCellLength = transient.smallestCellLength,
        .masterOpenCount = static_cast<int>(transient.masterValveOpenTimes.size()),
        .lastCell = ncel,
        .lastGasCell = gasLift.cellCount,
        .productionCrossSectionCount = trends.productionWallProfileCells,
        .masterCloseCount = static_cast<int>(transient.masterValveCloseTimes.size()),
        .endNode = noextremo,
        .gasSurfacePressure = gasLift.separatorPressure,
        .inletPressure = presE,
        .movingMeanPressure = transient.movingMeanPressure,
        .movingMeanStartTime = transient.movingMeanStartTime,
        .maximumTemperatureRates = transient.maximumTemperatureRates,
        .maximumPressureRates = transient.maximumPressureRates,
        .inletTemperature = tempE,
        .reverseQuality = networkCoupling.reverseQuality,
        .initialInterfaceVelocity = gasLift.previousInterfaceVelocity,
        .pigReceiverCells = transient.pigReceiverCells,
        .scheduledPigCount = static_cast<int>(transient.pigReceiverCells.size()),
        .globals = simulationContext,
    };
}

sisprod::fluidtracking::FluidTrackingState SProd::fluidTrackingStateOf() {
    return sisprod::fluidtracking::FluidTrackingState{
        .cells = celula,
        .lastCell = ncel,
        .input = arq,
        .globals = simulationContext,
        .endNode = noextremo,
        .inletPressure = presE,
        .inletTemperature = tempE,
        .inletComplementaryFraction = betaE,
        .trackGasOilRatio = transient.trackGasOilRatio,
        .trackGasGravity = transient.trackGasGravity,
        .compositionalRefreshCounter = transient.compositionalRefreshCounter,
        .steadyIteration = steadyIteration,
    };
}

sisprod::transient::TransientSolveState SProd::transientSolveStateOf() {
    return sisprod::transient::TransientSolveState{
        .step = transientStateOf(),
        .defaultInletTemperature = temperatura,
        .annulusDrift = derivaAnel,
        .closingSubtitles = saidaSubTextoSis,
        .closingTitles = saidaTextoSis,
        .compositionalRefreshCounter = transient.compositionalRefreshCounter,
        .fluxHistory = transient.fluxSamples,
        .gasCrossSectionCellCounts = trends.gasWallProfileCells,
        .gasCrossSectionProfileTimeCounter = trends.gasWallProfileTimeIndex,
        .gasCrossSectionTrendBufferedCounts = trends.gasWallTrendBufferedCounts,
        .gasCrossSectionTrendCounts = trends.gasWallTrendCounts,
        .gasCrossSectionTrendMatrix = trends.gasWallTrendMatrices,
        .gasCrossSectionTrendResetTimers = trends.gasWallTrendResetTimes,
        .gasProfileTimeCounter = trends.gasProfileTimeIndex,
        .gasTrendBufferedCounts = trends.gasTrendBufferedCounts,
        .gasTrendCounts = trends.gasTrendCounts,
        .gasTrendMatrix = trends.gasTrendMatrices,
        .gasTrendResetTimers = trends.gasTrendResetTimes,
        .initialGasPressure = gasLift.injectionPressure,
        .initialGasSurfacePressure = transient.previousSeparatorPressure,
        .initialGasTemperature = gasLift.injectionTemperature,
        .initialOpenTime = transient.previousActiveChokeDelay,
        .injectionChoke = gasLift.injectionChoke,
        .logBuffer = tmpLog,
        .logCounter = transient.eventLogCount,
        .massTransferModel = transient.massTransferModel,
        .minimumCycleTimeStep = transient.minimumCycleTimeStep,
        .movingMeanCounter = transient.movingMeanWindow,
        .movingMeanVoidFraction = transient.movingMeanVoidFraction,
        .networkCoupled = gasLift.annulusThermalCoupling,
        .poissonSolver3D = coupling3D.poissonSolver3D,
        .pressureHistory = transient.pressureSamples,
        .printCounter = transient.printCounter,
        .printPassCount = trends.trendOutputCount,
        .productionCrossSectionProfileTimeCounter = trends.productionWallProfileTimeIndex,
        .productionCrossSectionTrendBufferedCounts = trends.productionWallTrendBufferedCounts,
        .productionCrossSectionTrendCounts = trends.productionWallTrendCounts,
        .productionCrossSectionTrendMatrix = trends.productionWallTrendMatrices,
        .productionCrossSectionTrendResetTimers = trends.productionWallTrendResetTimes,
        .productionProfileTimeCounter = trends.productionProfileTimeIndex,
        .productionTrendBufferedCounts = trends.productionTrendBufferedCounts,
        .productionTrendCounts = trends.productionTrendCounts,
        .productionTrendMatrix = trends.productionTrendMatrices,
        .productionTrendResetTimers = trends.productionTrendResetTimes,
        .startNode = noinicial,
        .timeStepHistory = transient.timeSamples,
        .totalFlux = transient.accumulatedFlux,
        .totalPressure = transient.accumulatedPressure,
        .totalVoidFraction = transient.accumulatedVoidFraction,
        .trackGasGravity = transient.trackGasGravity,
        .trackGasOilRatio = transient.trackGasOilRatio,
        .unitCellTimeCounters = trends.singleCellOutputTimeIndices,
        .voidFractionHistory = transient.voidFractionSamples,
    };
}

driftflux::coefficient::ClosureState SProd::closureStateOf() {
    return driftflux::coefficient::ClosureState{
        .cells = celula,
        .lastCell = ncel,
        .globals = simulationContext,
        .input = arq,
        .selectors = {arq.CorreDisper, arq.CorreAnular, arq.CorreEstrat},
        .gasSurfaceTemperature = tGSup,
        .inletVoidFraction = alfE,
        .inletComplementaryFraction = betaE,
        .inletPressure = presE,
        .inletTemperature = tempE,
        .steadyIteration = steadyIteration,
    };
}

sisprod::sources::SourceState SProd::sourceStateOf() {
    return sisprod::sources::SourceState{
        .cells = celula,
        .input = arq,
        .globals = simulationContext,
        .lastCell = ncel,
        .steadyMode = modoPerm,
        .parallelSecondaryBranch = networkCoupling.parallelSecondaryBranch,
        .parallelSecondaryBoundaryCondition = networkCoupling.parallelSecondaryBoundaryCondition,
        .parallelSourceCells = networkCoupling.parallelSourceCells,
        .parallelSourceProductionLiquid = networkCoupling.parallelSourceProductionLiquid,
        .parallelSourceComplementaryLiquid = networkCoupling.parallelSourceComplementaryLiquid,
        .parallelSourceGas = networkCoupling.parallelSourceGas,
    };
}

sisprod::thermal::ThermalState SProd::thermalStateOf() {
    return sisprod::thermal::ThermalState{
        .cells = celula,
        .gasCells = gasLift.cells,
        .input = arq,
        .latentHeatTable = tables.latentHeatTable.rowPointers,
        .globals = simulationContext,
        .thermalSourceDisabled = thermalSourceDisabled,
        .productionNetworkCoupled = networkCoupling.secondaryThermalCoupling,
        .primarySectionStart = networkCoupling.secondaryCouplingStart,
        .primarySectionEnd = networkCoupling.secondaryCouplingEnd,
        .coupledCellIndices = coupling3D.coupledIndexCorrections,
        .poissonSolver3D = coupling3D.poissonSolver3D,
        .axisymmetricSolver = axisymmetricThermalSolver,
        .lastCell = ncel,
        .surfaceChoke = chokeSup,
        .surfaceChokeMassFlag = masChkSup,
        .endNode = noextremo,
        .gasSurfaceTemperature = tGSup,
        .latentHeatEnabled = CalcLat,
        .fullModel = transient.fullModel,
        .massTransferModel = transient.massTransferModel,
        .inletPressure = presE,
        .inletTemperature = tempE,
        .inletQuality = titE,
        .inletVoidFraction = alfE,
        .inletComplementaryFraction = betaE,
        .surfaceChokeOpen = transient.surfaceChokeOpen,
        .defaultInletTemperature = temperatura,
        .timeStep = dt,
        .minimumCycleTimeStep = transient.minimumCycleTimeStep,
        .poisson2DCellIndices = transient.poisson2DCells,
        .poisson2DCellCount = static_cast<int>(transient.poisson2DCells.size()),
        .steadyIteration = steadyIteration,
        .slowHeatTransferThreshold = steadySearch.slowHeatTransferThreshold,
        .annulusTubingStart = gasLift.tubingCouplingStart,
        .annulusTubingEnd = gasLift.tubingCouplingEnd,
        .tubingAnnulusStart = gasLift.annulusCouplingStart,
        .productionNetworkHeatCoupled = networkCoupling.primaryThermalCoupling,
        .primaryNetworkSectionEnd = networkCoupling.primaryCouplingEnd,
        .primaryNetworkSectionStart = networkCoupling.primaryCouplingStart,
        .lastGasCell = gasLift.cellCount,
        .gasLiftChokes = gasLift.valveChokes,
        .gasSurfacePressure = gasLift.separatorPressure,
        .outletPressure = presfim,
        .surfaceTemperature = networkCoupling.chokeOutletTemperature,
        .networkCoupled = gasLift.annulusThermalCoupling,
        .tubingAnnulusEnd = gasLift.annulusCouplingEnd,
    };
}

trendoutput::TrendState SProd::trendStateOf() const {
    return trendoutput::TrendState{
        .input = arq,
        .engineVersion = versao,
        .globals = simulationContext,
        .branchIndex = indTramo,
        .printPassCount = trends.trendOutputCount,
        .production = {trends.productionTrendMatrices, trends.productionTrendCounts,
                       trends.productionTrendBufferedCounts},
        .gasLine = {trends.gasTrendMatrices, trends.gasTrendCounts, trends.gasTrendBufferedCounts},
        .productionCrossSection = {trends.productionWallTrendMatrices, trends.productionWallTrendCounts,
                                   trends.productionWallTrendBufferedCounts},
        .gasLineCrossSection = {trends.gasWallTrendMatrices, trends.gasWallTrendCounts,
                                trends.gasWallTrendBufferedCounts}};
}

SProd::SteadyCallState SProd::steadyCallStateOf() {
    return {closureStateOf(), sourceStateOf(), thermalStateOf(), gasLiftStateOf(), searchStateOf()};
}

SProd::TransientCallState SProd::transientCallStateOf() {
    return {closureStateOf(), sourceStateOf(), thermalStateOf(), gasLiftStateOf(), fluidTrackingStateOf(),
            trendStateOf(), transientSolveStateOf()};
}

void transient::HydrateEnvelopeSolver::operator()() const {
    system_.solveHydrateEnvelopes();
}
