#include "SisProdThermal.h"
#include "SisProdGasLift.h"
#include "SisProdGasLiftInternal.h"

#include "Leitura.h"
#include "SisProdConstants.h"
#include "Matriz.h"
#include "Vetor.h"
#include "celulaGas.h"
#include "celula3.h"
#include "chokegas.h"
#include "UnitConversions.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace sisprod::gaslift {

// Unloading the completion fluid through the gas-lift valves.

void computeGasUnloadingHydrostatics(const GasLiftState &state) {
    double meanPressure = unloadingStartPressure(state);
    if (state.input.controDesc == 1)
        state.injectionPressure = state.input.presIniDescG;

    CelG &inletCell = state.gasCells[0];
    CelG &secondCell = state.gasCells[1];
    inletCell.presL = meanPressure;
    inletCell.pres = meanPressure;
    inletCell.presini = meanPressure;
    secondCell.presL = meanPressure;
    double meanTemperature = inletCell.calor.Textern1;
    inletCell.tempL = meanTemperature;
    inletCell.temp = meanTemperature;
    secondCell.tempL = meanTemperature;
    double leftDensity = inletCell.flui.MasEspGas(meanPressure, meanTemperature);
    inletCell.u1L = inletCell.duto.area * leftDensity;
    inletCell.u1LL = inletCell.u1L;
    secondCell.u1LL = inletCell.u1L;
    inletCell.VGasL = 0;
    inletCell.VGasR = 0;
    secondCell.VGasL = 0;
    inletCell.massfonteCH = 0.;
    for (int gasCellIndex = 1; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        CelG &left = state.gasCells[gasCellIndex - 1];
        const double leftHalfLength = 0.5 * cell.dxL;
        const double localHalfLength = 0.5 * cell.dx0;
        meanPressure -= unloadingHalfCellHead(leftDensity, leftHalfLength, left.duto.teta);
        meanTemperature = cell.calor.Textern1;
        const double localDensity = gasCellIndex < state.interfaceCell
                                        ? cell.flui.MasEspGas(meanPressure, meanTemperature)
                                        : cell.MasEspFlu(meanPressure, meanTemperature);
        meanPressure -= unloadingHalfCellHead(localDensity, localHalfLength, cell.duto.teta);
        leftDensity = localDensity;

        cell.pres = meanPressure;
        cell.presini = meanPressure;
        left.presR = meanPressure;
        cell.temp = meanTemperature;
        left.tempR = meanTemperature;
        cell.u1L = cell.duto.area * leftDensity;
        left.u1R = cell.u1L;
        cell.VGasR = 0;
        left.VGasRR = 0;
        cell.massfonteCH = 0.;
        if (gasCellIndex < state.lastGasCell) {
            CelG &right = state.gasCells[gasCellIndex + 1];
            right.presL = meanPressure;
            right.tempL = meanTemperature;
            right.u1LL = cell.u1L;
            right.VGasL = 0;
        }
    }
}

void computeProductionUnloadingHydrostatics(const GasLiftState &state) {
    Cel &inletCell = state.cells[0];
    Cel &secondCell = state.cells[1];
    clearMassSources(inletCell);
    const bool inletBelowInterface = 0 <= state.input.celdescargaP;
    setUnloadingPhase(inletCell, inletBelowInterface);
    copyPhaseToRightNeighbour(inletCell, secondCell);
    double meanPressure = inletCell.acsr.ipr.Pres;
    inletCell.presL = meanPressure;
    inletCell.pres = meanPressure;
    inletCell.presini = meanPressure;
    inletCell.presauxL = meanPressure;
    double meanTemperature = inletCell.calor.Textern1;
    inletCell.tempL = meanTemperature;
    inletCell.temp = meanTemperature;
    inletCell.tempini = meanTemperature;
    double leftDensity = inletBelowInterface ? inletCell.fluicol.MasEspFlu(meanPressure, meanTemperature)
                                             : inletCell.flui.MasEspGas(meanPressure, meanTemperature);
    double localDensity = inletCell.fluicol.MasEspFlu(meanPressure, meanTemperature);

    inletCell.FW = 0;
    inletCell.FWini = 0;
    inletCell.arranjo = 0;
    inletCell.QLL = 0;
    secondCell.QLL = inletCell.QLL;
    inletCell.QL = 0;
    inletCell.QG = 0;
    inletCell.rpL = localDensity;
    inletCell.rpC = localDensity;
    inletCell.rcL = localDensity;
    inletCell.rcC = localDensity;

    inletCell.MC = 0.;
    inletCell.ML = 0.;
    secondCell.ML = inletCell.MC;
    inletCell.Mliqini = inletCell.MC;
    secondCell.MliqiniL = inletCell.MC;
    inletCell.MliqiniL = inletCell.MC;

    inletCell.rpLi = localDensity;
    inletCell.rpCi = localDensity;
    inletCell.rcLi = localDensity;
    inletCell.rcCi = localDensity;

    for (int cellIndex = 1; cellIndex <= state.lastCell; cellIndex++) {
        Cel &cell = state.cells[cellIndex];
        Cel &left = state.cells[cellIndex - 1];
        const bool belowInterface = cellIndex <= state.input.celdescargaP;
        const double leftHalfLength = 0.5 * cell.dxL;
        const double localHalfLength = 0.5 * cell.dx;
        meanPressure -= unloadingHalfCellHead(leftDensity, leftHalfLength, left.duto.teta);
        meanTemperature = cell.calor.Textern1;
        double interpolatedTemperature =
            (leftHalfLength * left.temp + localHalfLength * meanTemperature) / (leftHalfLength + localHalfLength);
        cell.presaux = meanPressure;
        left.presauxR = cell.presaux;
        if (belowInterface)
            localDensity = cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        else
            localDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
        meanPressure -= unloadingHalfCellHead(localDensity, localHalfLength, cell.duto.teta);
        leftDensity = localDensity;

        cell.presL = left.pres;
        cell.pres = meanPressure;
        left.presR = meanPressure;
        cell.presini = meanPressure;
        cell.presLini = left.presini;
        cell.presauxL = left.presaux;
        cell.tempL = left.temp;
        cell.temp = meanTemperature;
        left.tempR = meanTemperature;
        cell.tempini = meanTemperature;

        cell.FW = 0;
        cell.FWini = 0;
        cell.arranjo = 0;
        cell.QL = 0;
        left.QLR = cell.QL;
        cell.QG = 0;
        cell.rpL = localDensity;
        cell.rpC = localDensity;
        left.rpR = cell.rpC;
        cell.rcL = localDensity;
        cell.rcC = localDensity;
        left.rcR = cell.rcC;

        const double interpolatedDensity = belowInterface
                                               ? cell.fluicol.MasEspFlu(cell.presaux, interpolatedTemperature)
                                               : inletCell.flui.MasEspGas(cell.presaux, interpolatedTemperature);
        cell.rpCi = interpolatedDensity;
        left.rpRi = interpolatedDensity;
        cell.rcCi = interpolatedDensity;
        left.rcRi = interpolatedDensity;

        clearMassSources(cell);
        setUnloadingPhase(cell, belowInterface);
        copyPhaseToLeftNeighbour(cell, left);

        if (cellIndex < state.lastCell) {
            Cel &right = state.cells[cellIndex + 1];
            right.QLL = cell.QL;
            right.rpLi = interpolatedDensity;
            right.rcLi = interpolatedDensity;
            copyPhaseToRightNeighbour(cell, right);
        }
    }
}

double searchUnloadingInjectionPressure(const GasLiftState &state) {
    if (state.input.descarga != 1) {
        state.separatorPressure = std::max(state.separatorPressure * kRelaxationDownFactor, state.input.presMinDesc);
        relaxTowards(state.injectionPressure, state.input.presIniDesc);
        return 0.;
    }
    const double simulationTime = state.globals->simulationTime;
    if (simulationTime > state.input.tempoLatenciaDesc) {
        const UnloadingValve valve = selectUnloadingValve(state);
        recordUnloadingSample(state, valve.flowRate);
        const double controlledFlowRate = kCurrentFlowWeight * valve.flowRate + (1. - kCurrentFlowWeight) *
                                                                                    state.accumulatedUnloadingVolume /
                                                                                    state.unloadingAveragingInterval;
        computeMaximumSeparatorPressure(state);
        correctInjectionPressure(state, controlledFlowRate, valve.index);
        if (state.gasValveCellIndices[valve.index] >= state.interfaceCell)
            correctSeparatorPressure(state, controlledFlowRate, valve.index);
        return valve.flowRate;
    }
    // Before the control starts, the separator pressure ramps down to the unloading's minimum, and the injection
    // pressure holds its initial value.
    const double rampEnd = kSeparatorRampLatencyFraction * state.input.tempoLatenciaDesc;
    if (simulationTime < rampEnd) {
        const double columnTopPressure = state.cells[state.lastCell].pres;
        state.separatorPressure = columnTopPressure - (columnTopPressure - state.input.presMinDesc) *
                                                          state.cells[0].dt / (rampEnd - simulationTime);
    } else {
        state.separatorPressure = state.input.presMinDesc;
    }
    state.injectionPressure = state.input.presIniDescG;
    return 0.;
}

UnloadingValve selectUnloadingValve(const GasLiftState &state) {
    UnloadingValve selected{0., 0};
    const int valveCount = state.input.nvalvgas;
    for (int valveIndex = 0; valveIndex < valveCount; valveIndex++) {
        const int gasValveCell = state.gasValveCellIndices[valveIndex];
        if (gasValveCell <= state.interfaceCell)
            continue;
        const CelG &valveCell = state.gasCells[gasValveCell];
        const double completionFluidDensity = valveCell.MasEspFlu(valveCell.pres, valveCell.temp);
        const double massFlowRate = primeValveMassSource<kUnloadingThroughValve>(
            state, valveIndex, valveCell, state.cells[state.productionValveCellIndices[valveIndex]]);
        const double candidateFlowRate = massFlowRate / completionFluidDensity;
        if (candidateFlowRate > selected.flowRate)
            selected = {candidateFlowRate, valveIndex};
    }
    return selected;
}

void recordUnloadingSample(const GasLiftState &state, double maximumFlowRate) {
    if (std::ssize(state.unloadingTimeStepSamples) > state.unloadingSampleCapacity ||
        state.unloadingAveragingInterval > state.unloadingAveragingWindow) {
        state.unloadingAveragingInterval -= state.unloadingTimeStepSamples[0];
        state.unloadingTimeStepSamples.erase(state.unloadingTimeStepSamples.begin());
        state.accumulatedUnloadingVolume -= state.maximumUnloadingFlowSamples[0];
        state.maximumUnloadingFlowSamples.erase(state.maximumUnloadingFlowSamples.begin());
    }

    const double sample = maximumFlowRate * state.timeStep;
    state.maximumUnloadingFlowSamples.push_back(sample);
    state.accumulatedUnloadingVolume += sample;
    state.unloadingTimeStepSamples.push_back(state.timeStep);
    state.unloadingAveragingInterval += state.timeStep;
}

void computeMaximumSeparatorPressure(const GasLiftState &state) {
    double meanPressure = state.separatorPressure;
    double maximumPressure = kUnboundedUnloadingPressure;
    for (int cellIndex = state.lastCell; cellIndex >= 0; cellIndex--) {
        Cel &cell = state.cells[cellIndex];
        const double meanTemperature = cell.temp;
        const double flowArea = cell.duto.area;
        const double perimeter = cell.duto.peri;
        const double cellLength = cell.dx;
        const double gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
        const double productionLiquidDensity = cell.flui.MasEspLiq(meanPressure, meanTemperature);
        const double completionFluidDensity = cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        const double gasViscosity = cell.flui.ViscGas(meanPressure, meanTemperature);
        const double oilViscosity = cell.flui.ViscOleo(meanPressure, meanTemperature);
        const double completionFluidViscosity = cell.fluicol.VisFlu(meanPressure, meanTemperature);
        const double voidFraction = cell.alf;
        const double completionFluidFraction = cell.bet;
        double liquidDensity =
            completionFluidFraction * completionFluidDensity + (1. - completionFluidFraction) * productionLiquidDensity;
        double liquidViscosity =
            completionFluidFraction * completionFluidViscosity + (1. - completionFluidFraction) * oilViscosity;
        const double mixtureDensity = voidFraction * gasDensity + (1. - voidFraction) * liquidDensity;
        const double mixtureViscosity = voidFraction * gasViscosity + (1. - voidFraction) * liquidViscosity;
        const double mixtureVelocity = cell.QL / (flowArea * liquidDensity) + cell.QG / (flowArea * gasDensity);
        double frictionFactor = 0.;
        if (fabs(mixtureVelocity) > kStillMixtureVelocity) {
            const double reynolds = cell.Rey(characteristicDiameter(cell.duto.revest, cell.duto.a, flowArea, perimeter),
                                             mixtureVelocity, mixtureDensity, mixtureViscosity);
            frictionFactor = cell.fric(reynolds, cell.duto.rug / cell.duto.a);
        }
        const double wallShearStress = frictionFactor * mixtureDensity * mixtureVelocity * fabs(mixtureVelocity) / 2.;
        meanPressure -= (-kGravity * mixtureDensity * sin(cell.duto.teta) - wallShearStress * perimeter / flowArea) *
                        cellLength / units::kPascalPerKgfPerCm2;
        if (cell.acsr.tipo == kAccessoryInflowPerformance) {
            const double candidateMaximumPressure = cell.acsr.ipr.Pres - (meanPressure - state.separatorPressure);
            maximumPressure = std::min(maximumPressure, candidateMaximumPressure);
        }
    }
    state.input.presMaxDesc = maximumPressure;
}

void correctInjectionPressure(const GasLiftState &state, double flowRate, int valveIndex) {
    const Ler &input = state.input;
    const bool separatorAtMaximum = state.separatorPressure >= input.presMaxDesc * kReachedMaximumPressureFactor;
    const bool separatorAtMinimum = state.separatorPressure <= input.presMinDesc * kReachedMinimumPressureFactor;
    if (separatorAtMaximum && flowRate > kUnloadingUpperBandEdge * input.vazDescControl &&
        state.gasCells[0].VGasR > 0.) {
        const double pressureCorrection =
            limitedCorrection(unloadingPressureCorrection(state, flowRate, valveIndex, kUnloadingUpperBandEdge, -1),
                              state.injectionPressure, state.cells[0].dt);
        state.injectionPressure = std::max(state.injectionPressure + pressureCorrection, input.presMinDescG);
    } else if (flowRate <= kUnloadingLowerBandEdge * input.vazDescControl && separatorAtMinimum) {
        const double pressureCorrection =
            limitedCorrection(unloadingPressureCorrection(state, flowRate, valveIndex, kUnloadingLowerBandEdge, 1),
                              state.injectionPressure, state.cells[0].dt);
        state.injectionPressure = std::min(state.injectionPressure + pressureCorrection, input.presMaxDescG);
    }
}

void correctSeparatorPressure(const GasLiftState &state, double flowRate, int valveIndex) {
    if (state.cells[state.lastCell - 1].MC > 0.0) {
        if (flowRate > kUnloadingUpperBandEdge * state.input.vazDescControl) {
            const double pressureCorrection = limitedCorrection(
                unloadingPressureCorrection(state, flowRate, valveIndex, kUnloadingUpperBandEdge, -1),
                state.separatorPressure, state.cells[0].dt);
            state.separatorPressure = std::min(state.separatorPressure - pressureCorrection, state.input.presMaxDesc);
        } else if (flowRate <= kUnloadingLowerBandEdge * state.input.vazDescControl) {
            const double pressureCorrection = limitedCorrection(
                unloadingPressureCorrection(state, flowRate, valveIndex, kUnloadingLowerBandEdge, 1),
                state.separatorPressure, state.cells[0].dt);
            state.separatorPressure = std::max(state.separatorPressure - pressureCorrection, state.input.presMinDesc);
        }
    } else {
        state.separatorPressure = std::max(
            state.separatorPressure * (1 - kSeparatorPressureDecayPerSecond * state.timeStep), state.input.presMinDesc);
    }
}

double unloadingPressureCorrection(const GasLiftState &state, double maximumFlowRate, int valveIndex, double factor,
                                   int sign) {
    const int gasValveCell = state.gasValveCellIndices[valveIndex];
    ChokeGas &choke = state.gasLiftChokes[valveIndex];
    const CelG &valveCell = state.gasCells[gasValveCell];
    choke.presEstag = valveCell.pres;
    choke.tempEstag = valveCell.temp;
    const double completionFluidDensity = valveCell.MasEspFlu(choke.presEstag, choke.tempEstag);
    const double massFlowRate = (factor * state.input.vazDescControl - maximumFlowRate) * completionFluidDensity;
    const double throatMassFlux = massFlowRate / choke.areagarg;
    const double pressureCorrection =
        throatMassFlux * throatMassFlux / (2. * completionFluidDensity * units::kPascalPerKgfPerCm2);
    return sign * pressureCorrection;
}

void solveUnloading(const GasLiftState &state, const thermal::ThermalState &dependencies) {
    for (int gasCellIndex = state.interfaceCell; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        CelG &left = state.gasCells[gasCellIndex - 1];
        const double leftHalfLength = 0.5 * cell.dxL;
        const double localHalfLength = 0.5 * cell.dx0;
        const double localGasShare = cell.razInter <= kMidCellInterfaceRatio ? 2 * cell.razInter : 1.;
        const double leftGasShare =
            left.razInter >= kMidCellInterfaceRatio ? 2 * (left.razInter - kMidCellInterfaceRatio) : 0.;
        const double leftGasLength = leftHalfLength * leftGasShare;
        const double localGasLength = localHalfLength * localGasShare;
        const double leftLiquidLength = leftHalfLength - leftGasLength;
        const double localLiquidLength = localHalfLength - localGasLength;
        const double leftTemperature = left.temp;
        const double leftCellPressure = left.pres;
        double liquidDensity = cell.MasEspFlu(leftCellPressure, leftTemperature);
        const double liquidViscosity = cell.VisFlu(leftCellPressure, leftTemperature);
        double gasDensity = cell.flui.MasEspGas(leftCellPressure, leftTemperature);
        const double gasViscosity = cell.flui.ViscGas(leftCellPressure, leftTemperature);
        const double carryingDensity = cell.razInter > state.globals->smallValueTolerance ? gasDensity : liquidDensity;
        const double leftVelocity = cell.VGasL / (carryingDensity * left.duto.area);
        const double localVelocity = cell.VGasL / (carryingDensity * cell.duto.area);
        const double leftDiameter =
            characteristicDiameter(left.duto.revest, left.duto.a, left.duto.area, left.duto.peri);
        const double localDiameter =
            characteristicDiameter(cell.duto.revest, cell.duto.a, cell.duto.area, cell.duto.peri);
        const double leftLiquidReynolds = left.Rey(leftDiameter, leftVelocity, liquidDensity, liquidViscosity);
        const double localLiquidReynolds = cell.Rey(localDiameter, localVelocity, liquidDensity, liquidViscosity);
        const double leftGasReynolds = left.Rey(leftDiameter, leftVelocity, gasDensity, gasViscosity);
        const double localGasReynolds = cell.Rey(localDiameter, localVelocity, gasDensity, gasViscosity);
        const double leftLiquidFriction = left.fric(leftLiquidReynolds, left.duto.rug / left.duto.a) * leftLiquidLength;
        const double localLiquidFriction = cell.fric(localLiquidReynolds, cell.duto.rug / cell.duto.a) * localLiquidLength;
        const double leftLiquidHead = (kGravity * sin(left.duto.teta) * liquidDensity) * leftLiquidLength;
        const double localLiquidHead = (kGravity * sin(cell.duto.teta) * liquidDensity) * localLiquidLength;
        const double leftGasFriction = left.fric(leftGasReynolds, left.duto.rug / left.duto.a) * leftGasLength;
        const double localGasFriction = cell.fric(localGasReynolds, cell.duto.rug / cell.duto.a) * localGasLength;
        const double leftGasHead = (kGravity * sin(left.duto.teta) * gasDensity) * leftGasLength;
        const double localGasHead = (kGravity * sin(cell.duto.teta) * gasDensity) * localGasLength;
        cell.pres = left.pres + (-0.5 * (leftLiquidFriction * liquidDensity + leftGasFriction * gasDensity) *
                                     leftVelocity * fabs(leftVelocity) * left.duto.peri / left.duto.area -
                                 0.5 * (localLiquidFriction * liquidDensity + localGasFriction * gasDensity) *
                                     localVelocity * fabs(localVelocity) * cell.duto.peri / cell.duto.area -
                                 leftLiquidHead - localLiquidHead - leftGasHead - localGasHead) /
                                    units::kPascalPerKgfPerCm2;
        cell.presL = left.pres;
        left.presR = cell.pres;

        thermal::computeDischargeTemperature(dependencies, gasCellIndex);

        cell.u1L = ((1. - cell.razInter) * cell.MasEspFlu(cell.pres, cell.temp) +
                    cell.razInter * cell.flui.MasEspGas(leftCellPressure, leftTemperature)) *
                   cell.duto.area;
        left.u1R = cell.u1L;
        cell.u1LL = left.u1L;
    }

    double totalSourceVolumeFlow = 0.;
    for (int gasCellIndex = state.interfaceCell; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        const CelG &cell = state.gasCells[gasCellIndex];
        const double cellTemperature = cell.temp;
        const double cellPressure = cell.pres;
        const double liquidDensity = cell.MasEspFlu(cellPressure, cellTemperature);
        const double gasDensity = cell.flui.MasEspGas(cellPressure, cellTemperature);
        totalSourceVolumeFlow += sourceVolumeFlowRate(cell, liquidDensity, gasDensity);
    }
    CelG &interfaceGasCell = state.gasCells[state.interfaceCell];
    const double interfaceTemperature = interfaceGasCell.temp;
    const double interfacePressure = interfaceGasCell.pres;
    const double interfaceGasDensity = interfaceGasCell.flui.MasEspGas(interfacePressure, interfaceTemperature);
    interfaceGasCell.VGasL = totalSourceVolumeFlow * interfaceGasDensity;
    state.gasCells[state.interfaceCell - 1].VGasR = interfaceGasCell.VGasL;
    state.gasCells[state.interfaceCell - 2].VGasRR = interfaceGasCell.VGasL;
    for (int gasCellIndex = state.interfaceCell; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        CelG &left = state.gasCells[gasCellIndex - 1];
        const double cellTemperature = cell.temp;
        const double cellPressure = cell.pres;
        const double liquidDensity = cell.MasEspFlu(cellPressure, cellTemperature);
        const double gasDensity = cell.flui.MasEspGas(cellPressure, cellTemperature);
        totalSourceVolumeFlow -= sourceVolumeFlowRate(cell, liquidDensity, gasDensity);
        if (gasCellIndex < state.lastGasCell) {
            CelG &right = state.gasCells[gasCellIndex + 1];
            right.VGasL = totalSourceVolumeFlow * liquidDensity;
            cell.VGasR = right.VGasL;
        } else
            cell.VGasR = 0.;
        left.VGasRR = cell.VGasL;
    }

    const CelG &nextGasCell = state.gasCells[state.interfaceCell + 1];
    state.interfaceVelocity =
        nextGasCell.VGasL / (nextGasCell.MasEspFlu(nextGasCell.pres, nextGasCell.temp) * interfaceGasCell.duto.area);
}

void advanceInterface(const GasLiftState &state) {
    CelG &cell = state.gasCells[state.interfaceCell];
    cell.razInter = (cell.razInterIni * cell.dx0 + state.interfaceVelocity * state.timeStep) / cell.dx0;
    if (state.interfaceCell == (state.lastGasCell - 1) && cell.razInter >= kInterfaceHandoverRatio) {
        handOverInterface(state);
        return;
    }
    if (state.input.descarga != 1)
        return;

    // The ratio snaps to the face it reached; when it crossed the face during the step, the interface time step is
    // shortened to the crossing.
    const double tiny = state.globals->smallValueTolerance;
    if (cell.razInter < -tiny) {
        const double interfaceCrossingTimeStep = -cell.razInterIni * cell.dx0 / state.interfaceVelocity;
        if (interfaceCrossingTimeStep > tiny)
            state.interfaceTimeStep = interfaceCrossingTimeStep;
        cell.razInter = 0.;
    } else if (cell.razInter <= tiny) {
        cell.razInter = 0.;
    } else if (cell.razInter > 1. + tiny) {
        const double interfaceCrossingTimeStep = (1. - cell.razInterIni) * cell.dx0 / state.interfaceVelocity;
        if (interfaceCrossingTimeStep > tiny)
            state.interfaceTimeStep = interfaceCrossingTimeStep;
        cell.razInter = 1.;
    } else if (cell.razInter >= 1. - tiny) {
        cell.razInter = 1.;
    }

    if (fabs(cell.razInter) < tiny && state.interfaceVelocity < -fabs(tiny)) {
        cell.razInter = 0.;
        if (state.interfaceCell > 0) {
            state.interfaceCell--;
            state.gasCells[state.interfaceCell].razInter = 1.;
        }
    } else if (fabs(cell.razInter - 1.) < tiny && state.interfaceVelocity > tiny) {
        cell.razInter = 1.;
        if (state.interfaceCell < state.lastGasCell) {
            state.interfaceCell++;
            state.gasCells[state.interfaceCell].razInter = 0.;
        }
    }
}

void handOverInterface(const GasLiftState &state) {
    CelG &interfaceGasCell = state.gasCells[state.interfaceCell];
    CelG &leftGasCell = state.gasCells[state.interfaceCell - 1];
    // The interface cell is the last but one: its right neighbour is the end of the gas line.
    CelG &rightGasCell = state.gasCells[state.interfaceCell + 1];

    const double interfaceGasDensity = interfaceGasCell.flui.MasEspGas(interfaceGasCell.pres, interfaceGasCell.temp);
    const double interfaceLiquidDensity = interfaceGasCell.MasEspFlu(interfaceGasCell.pres, interfaceGasCell.temp);
    interfaceGasCell.VGasR = interfaceGasCell.VGasR * interfaceGasDensity / interfaceLiquidDensity;
    interfaceGasCell.u1L = interfaceGasCell.duto.area * interfaceGasDensity;
    leftGasCell.u1R = interfaceGasCell.u1L;
    leftGasCell.VGasRR = interfaceGasCell.VGasR;
    rightGasCell.u1LL = interfaceGasCell.u1L;
    rightGasCell.VGasL = interfaceGasCell.VGasR;

    interfaceGasCell.razInter = 1.0;
    rightGasCell.razInter = 1.0;

    rightGasCell.presini = rightGasCell.pres;
    const double outletGasDensity = rightGasCell.flui.MasEspGas(rightGasCell.pres, rightGasCell.temp);
    const double outletLiquidDensity = rightGasCell.MasEspFlu(rightGasCell.pres, rightGasCell.temp);
    rightGasCell.VGasR = 0 * rightGasCell.VGasR * outletGasDensity / outletLiquidDensity;
    rightGasCell.u1L = rightGasCell.duto.area * outletGasDensity;
    rightGasCell.u1R = rightGasCell.duto.area * outletGasDensity;
    interfaceGasCell.u1R = rightGasCell.u1L;
    interfaceGasCell.VGasRR = rightGasCell.VGasR;

    state.interfaceCell = kNoInterfaceCell;
    state.input.descarga = 0;
}

double unloadingStartPressure(const GasLiftState &state) {
    const Ler &input = state.input;
    if (input.controDesc == 1)
        return input.presIniDescG;
    // The configured condition, not the first gas cell's: the cells are being given their initial state here.
    if (input.controDesc == 0 && input.gasinj.tipoCC == kGasInletInjectionPressure)
        return input.gasinj.presinj[0];
    return kDefaultUnloadingStartPressure;
}

double unloadingHalfCellHead(double density, double halfLength, double inclination) {
    return density * kGravityUnloadingVariant * halfLength * sin(inclination) / units::kPascalPerKgfPerCm2;
}

void clearMassSources(Cel &cell) {
    cell.massfonteCH = 0;
    cell.fontemassCL = 0;
    cell.fontemassLL = 0;
    cell.fontemassGL = 0;
    cell.fontemassCR = 0;
    cell.fontemassLR = 0;
    cell.fontemassGR = 0;
}

void setUnloadingPhase(Cel &cell, bool belowInterface) {
    cell.alf = belowInterface ? 0. : 1.;
    cell.bet = belowInterface ? 1. : 0.;
    cell.betI = cell.bet;
    cell.alfini = cell.alf;
    cell.betini = cell.bet;
    cell.alfPigD = cell.alf;
    cell.betPigD = cell.bet;
    cell.alfPigE = cell.alf;
    cell.betPigE = cell.bet;
}

void copyPhaseToLeftNeighbour(const Cel &cell, Cel &left) {
    left.betR = cell.bet;
    left.betRini = cell.bet;
    left.alfR = cell.alf;
    left.alfRini = cell.alf;
}

void copyPhaseToRightNeighbour(const Cel &cell, Cel &right) {
    right.alfL = cell.alf;
    right.betL = cell.bet;
    right.betLI = cell.bet;
    right.alfLini = cell.alf;
    right.betLini = cell.bet;
}

void relaxTowards(double &pressure, double target) {
    if (pressure > target)
        pressure = std::max(pressure * kRelaxationDownFactor, target);
    else if (pressure < target)
        pressure = std::min(pressure * kRelaxationUpFactor, target);
}

double limitedCorrection(double correction, double pressure, double timeStep) {
    const double limit = kMaximumUnloadingCorrectionPerSecond * pressure * timeStep;
    if (fabs(correction) > limit)
        return (fabs(correction) / correction) * limit;
    return correction;
}

double sourceVolumeFlowRate(const CelG &cell, double liquidDensity, double gasDensity) {
    return cell.razInter > kMidCellInterfaceRatio ? cell.massfonteCH / gasDensity : cell.massfonteCH / liquidDensity;
}

// The gas line in the transient.

void solveGasLine(const GasLiftState &state, const thermal::ThermalState &dependencies) {
    if (state.input.lingas <= 0)
        return;

    updateTransientGasValves(state);
    for (int valveIndex = 0; valveIndex < state.input.nvalvgas; valveIndex++) {
        const int productionValveCell = state.productionValveCellIndices[valveIndex];
        const int gasValveCell = state.gasValveCellIndices[valveIndex];
        Cel &productionCell = state.cells[productionValveCell];
        CelG &valveCell = state.gasCells[gasValveCell];
        const ChokeGas &choke = state.gasLiftChokes[valveIndex];
        const bool valveDischargesGas =
            gasValveCell < state.interfaceCell ||
            (gasValveCell == state.interfaceCell && valveCell.razInter > kMidCellInterfaceRatio);
        if (valveDischargesGas) {
            productionCell.acsr.injg.QGas =
                valveCell.massfonteCH * units::kSecondsPerDay /
                valveCell.flui.MasEspGas(kStandardPressureKgfPerCm2, kStandardTemperatureCelsius);
            productionCell.acsr.injg.tipoflu = kInjectedGas;
            productionCell.acsr.injg.temp = injectedGasTemperature(state, dependencies, valveIndex, productionCell);
        } else {
            productionCell.acsr.injg.QGas = valveCell.massfonteCH;
            productionCell.acsr.injg.tipoflu = kInjectedCompletionFluid;
            productionCell.acsr.injg.temp = choke.tempEstag;
        }
        recordValveState(valveCell, choke);
    }
    advanceGasSubStep(state, dependencies);
}

void updateTransientGasValves(const GasLiftState &state) {
    const int valveCount = state.input.nvalvgas;
    for (int gasCellIndex = state.interfaceCell; gasCellIndex <= state.lastGasCell; gasCellIndex++)
        state.gasCells[gasCellIndex].massfonteCH = 0.;
    for (int valveIndex = 0; valveIndex < valveCount; valveIndex++) {
        const int gasValveCell = state.gasValveCellIndices[valveIndex];
        CelG &valveCell = state.gasCells[gasValveCell];
        const Cel &productionCell = state.cells[state.productionValveCellIndices[valveIndex]];
        // Behind the interface the valve discharges completion fluid, ahead of it gas.
        valveCell.massfonteCH =
            gasValveCell >= state.interfaceCell && valveCell.razInter < kMidCellInterfaceRatio
                ? primeValveMassSource<kCompletionFluidThroughValve>(state, valveIndex, valveCell, productionCell)
                : primeValveMassSource<kGasThroughValve>(state, valveIndex, valveCell, productionCell);
    }
    updateMaster2Source(state);
}

void updateMaster2Source(const GasLiftState &state) {
    const int master2CellIndex = state.input.master2.posic;
    CelG &upstreamCell = state.gasCells[master2CellIndex];
    CelG &downstreamCell = state.gasCells[master2CellIndex + 1];
    ChokeGas &choke = upstreamCell.chkcell;
    const double master2FlowArea = std::min(upstreamCell.duto.area, upstreamCell.dutoR.area);
    const bool throttling = choke.areagarg <= kMaster2ThrottlingAreaFraction * master2FlowArea &&
                            choke.areagarg >= kMaster2ClosedAreaFraction * master2FlowArea;
    if (!throttling) {
        upstreamCell.fonteM2 = 0.;
        downstreamCell.fonteM2 = 0.;
        return;
    }
    const double upstreamGasDensity = upstreamCell.flui.MasEspGas(upstreamCell.pres, upstreamCell.temp);
    const double downstreamGasDensity = downstreamCell.flui.MasEspGas(downstreamCell.pres, downstreamCell.temp);
    const double upstreamHead = -0.5 * upstreamGasDensity * units::kStandardGravity * upstreamCell.dx0 *
                                sin(upstreamCell.duto.teta) / units::kPascalPerKgfPerCm2;
    const double downstreamHead = 0.5 * downstreamGasDensity * units::kStandardGravity * downstreamCell.dx0 *
                                  sin(downstreamCell.duto.teta) / units::kPascalPerKgfPerCm2;
    if (downstreamCell.pres < upstreamCell.pres) {
        choke.presEstag = upstreamCell.pres + upstreamHead;
        choke.presGarg = downstreamCell.pres + downstreamHead;
        choke.tempEstag = upstreamCell.temp;
        upstreamCell.fonteM2 = -choke.massica();
    } else {
        choke.presEstag = downstreamCell.pres;
        choke.presGarg = upstreamCell.pres;
        choke.tempEstag = downstreamCell.temp;
        upstreamCell.fonteM2 = choke.massica();
    }
    downstreamCell.fonteM2 = -upstreamCell.fonteM2;
}

void advanceGasSubStep(const GasLiftState &state, const thermal::ThermalState &dependencies) {
    for (int gasCellIndex = 0; gasCellIndex <= state.lastGasCell; gasCellIndex++)
        state.gasCells[gasCellIndex].DeVoltaParaoFuturo();
    state.interfaceTimeStep = state.timeStep;
    if (state.input.descarga == 1) {
        state.previousInterfaceCell = state.interfaceCell;
        state.previousInterfaceTimeStep = state.interfaceTimeStep;
        state.previousInterfaceVelocity = state.interfaceVelocity;
        advanceInterface(state);
    }
    if (state.interfaceTimeStep < state.timeStep) {
        state.timeStep = state.interfaceTimeStep;
        for (int cellIndex = 0; cellIndex <= state.lastCell; cellIndex++) {
            Cel &cell = state.cells[cellIndex];
            cell.dt = state.timeStep;
            cell.dt2 = state.timeStep;
            cell.dtPig = state.timeStep;
        }
    }

    for (int gasCellIndex = 0; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        cell.dt = state.timeStep;
        cell.dTdt = 0;
    }

    // The temperature and density updates below stop before this cell: past the last one without an interface, at
    // the interface cell's left neighbour otherwise.
    const int gasLoopEnd = state.interfaceCell <= state.lastGasCell ? state.interfaceCell - 1 : state.lastGasCell + 1;

    const int maximumThermalCouplingCycles = state.input.cicloAcopTerm;
    for (int thermalCouplingCycle = 0; thermalCouplingCycle <= maximumThermalCouplingCycles; thermalCouplingCycle++) {
        assembleAndSolveGasSystem(state);
        updateGasLine(state);
        updateGasTemperatures(state, dependencies, gasLoopEnd);
        if (thermalCouplingCycle < maximumThermalCouplingCycles)
            for (int renewedCellIndex = 0; renewedCellIndex <= state.lastGasCell; renewedCellIndex++)
                state.gasCells[renewedCellIndex].FeiticoDoTempo();
    }

    updateGasDensities(state, gasLoopEnd);

    for (int gasCellIndex = 0; gasCellIndex <= state.lastGasCell; gasCellIndex++)
        state.gasCells[gasCellIndex].presini = state.gasCells[gasCellIndex].pres;

    if (state.input.descarga == 1)
        solveUnloading(state, dependencies);
}

void assembleAndSolveGasSystem(const GasLiftState &state) {
    const double chokeOpeningFraction = withGasInletCondition(
        state.gasCells[0].tipoCC,
        [&](auto condition) { return injectionChokeOpening(state, condition); });
#pragma omp parallel for num_threads(state.globals->solverThreadCount)
    for (int gasCellIndex = 0; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        cell.GeraLocal(state.lastGasCell, state.injectionPressure, state.injectionTemperature, chokeOpeningFraction);
        const int firstRow = kGasLineUnknownsPerCell * gasCellIndex;
        for (int bandIndex = 0; bandIndex < kGasLineStencilCoefficients; bandIndex++) {
            state.gasSystemMatrix[firstRow][bandIndex - kGasLineUnknownsPerCell] = cell.local[0][bandIndex];
            state.gasSystemMatrix[firstRow + 1][bandIndex - kGasLineUnknownsPerCell - 1] = cell.local[1][bandIndex];
            state.gasSystemMatrix[firstRow + 2][bandIndex - kGasLineUnknownsPerCell - 2] = cell.local[2][bandIndex];
        }
        state.gasRightHandSide[firstRow] = cell.TL[0];
        state.gasRightHandSide[firstRow + 1] = cell.TL[1];
        state.gasRightHandSide[firstRow + 2] = cell.TL[2];
    }
    state.gasSystemMatrix.GaussElimPP(state.gasRightHandSide);
}

void updateGasLine(const GasLiftState &state) {
    // The unknowns of a gas cell in the solved system: its pressure, then the gas mass flow through its right face.
    const auto pressureOf = [&](int cellIndex) { return state.gasRightHandSide[kGasLineUnknownsPerCell * cellIndex]; };
    const auto rightFaceFlowOf = [&](int cellIndex) {
        return state.gasRightHandSide[kGasLineUnknownsPerCell * cellIndex + 1];
    };
    for (int gasCellIndex = 0; gasCellIndex <= state.lastGasCell; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        // The first cell counts as the first even when it is also the last.
        const bool first = gasCellIndex == 0;
        const bool last = !first && gasCellIndex == state.lastGasCell;
        cell.pres = pressureOf(gasCellIndex);
        cell.presL = first ? cell.pres : pressureOf(gasCellIndex - 1);
        cell.presR = last ? cell.pres : pressureOf(gasCellIndex + 1);
        cell.VGasR = rightFaceFlowOf(gasCellIndex);
        if (!first)
            cell.VGasL = rightFaceFlowOf(gasCellIndex - 1);
        if (!last)
            cell.VGasRR = rightFaceFlowOf(gasCellIndex + 1);
    }
}

void updateGasTemperatures(const GasLiftState &state, const thermal::ThermalState &dependencies, int gasLoopEnd) {
    CelG &inletCell = state.gasCells[0];
    CelG &outletCell = state.gasCells[state.lastGasCell];
    inletCell.temp = state.injectionTemperature;
    inletCell.dTdt = (inletCell.temp - inletCell.tempini) / state.timeStep;
    if (outletCell.VGasR < 0.)
        outletCell.temp = kReverseFlowOutletTemperatureCelsius;
#pragma omp parallel for num_threads(state.globals->solverThreadCount)
    for (int gasCellIndex = 1; gasCellIndex < gasLoopEnd; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        thermal::computeGasTemperature(dependencies, gasCellIndex, state.gasCells[gasCellIndex - 1].tempini, 0);
        cell.dTdt = (cell.temp - cell.tempini) / state.timeStep;
    }
}

void updateGasDensities(const GasLiftState &state, int gasLoopEnd) {
#pragma omp parallel for num_threads(state.globals->solverThreadCount)
    for (int gasCellIndex = 0; gasCellIndex < gasLoopEnd; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        cell.rg = cell.flui.MasEspGas(cell.pres, cell.temp);
    }

    for (int gasCellIndex = 0; gasCellIndex < gasLoopEnd; gasCellIndex++) {
        CelG &cell = state.gasCells[gasCellIndex];
        cell.u1L = cell.rg * cell.duto.area;
        // The inlet feeds itself; every other cell is fed by the one upstream.
        cell.u1LL = gasCellIndex == 0 ? cell.u1L : state.gasCells[gasCellIndex - 1].u1L;
        if (gasCellIndex > 0) {
            state.gasCells[gasCellIndex - 1].rgR = cell.rg;
            state.gasCells[gasCellIndex - 1].u1R = cell.u1L;
        }
        if (gasCellIndex == state.lastGasCell) {
            cell.u1R = cell.u1L;
            cell.rgR = cell.rg;
        }
    }
}

void connectTubing(const GasLiftState &state) {
    forEachCoupledCellPair(state, [&](int productionCellIndex, int gasCellIndex) {
        Cel &productionCell = state.cells[productionCellIndex];
        CelG &gasCell = state.gasCells[gasCellIndex];
        productionCell.calor.Textern2 = gasCell.calor.Tcamada[0][0];
        productionCell.calor.betext = gasCell.calor.betint;
        const int layerIndex = productionCell.calor.geom.ncamadas - 1;
        const int discretizationIndex = productionCell.calor.ncamada[layerIndex] - 1;
        gasCell.calor.Tint2 = productionCell.calor.Tcamada[layerIndex][discretizationIndex];
        productionCell.calor.colunaDia = gasCell.duto.dia;
        productionCell.calor.geom.b = gasCell.calor.geom.a;

        productionCell.calor.Textern1 = gasCell.temp;
        productionCell.calor.Vextern1 = gasCell.VGasR / gasCell.u1L;
        // Ahead of the interface the annulus carries gas; behind it, completion fluid.
        const double gasPressure = gasCell.pres;
        const double gasTemperature = gasCell.temp;
        FluidProperties fluid;
        if (gasCellIndex < state.interfaceCell) {
            const double specificHeat = gasCell.flui.CalorGas(gasPressure, gasTemperature);
            const double density = gasCell.flui.MasEspGas(gasPressure, gasTemperature);
            fluid = {gasCell.flui.CondGas(gasPressure, gasTemperature), specificHeat, density,
                     gasCell.flui.ViscGas(gasPressure, gasTemperature)};
        } else {
            const double specificHeat = gasCell.CalorLiq(gasPressure, gasTemperature);
            const double density = gasCell.MasEspFlu(gasPressure, gasTemperature);
            fluid = {gasCell.CondLiq(gasPressure, gasTemperature), specificHeat, density,
                     gasCell.VisFlu(gasPressure, gasTemperature)};
        }
        publishExternalFluid(productionCell, fluid);
    });
}

double injectionChokeOpening(const GasLiftState &state, InjectionPressureCondition) {
    const double chokeOpeningFraction = state.injectionChoke.areagarg / state.gasCells[0].duto.area;
    if (chokeOpeningFraction < kThrottlingChokeOpening) {
        state.injectionChoke.presEstag = state.injectionPressure;
        state.injectionChoke.tempEstag = state.injectionTemperature;
        state.injectionChoke.presGarg = state.gasCells[0].pres;
        state.gasCells[0].massfonteCH = state.injectionChoke.massica();
    }
    return chokeOpeningFraction;
}

double injectionChokeOpening(const GasLiftState &, InjectionFlowRateCondition) {
    return 1.;
}

void recordValveState(CelG &gasCell, const ChokeGas &choke) {
    gasCell.pEstag = choke.presEstag;
    gasCell.tEstag = choke.tempEstag;
    gasCell.pGarg = choke.presGarg;
    gasCell.tGarg = choke.tempGarg;
    gasCell.qGarg = choke.qGarg;
    gasCell.areaGarg = choke.areagarg;
}

// The gas line in the steady state.

void initializeTubingConnectionSteady(const GasLiftState &state) {
    forEachCoupledCellPair(state, [&](int productionCellIndex, int gasCellIndex) {
        Cel &productionCell = state.cells[productionCellIndex];
        const CelG &gasCell = state.gasCells[gasCellIndex];
        productionCell.calor.Textern2 = gasCell.calor.Textern2;
        productionCell.calor.colunaDia = gasCell.duto.dia;

        const double productionPressure = productionCell.pres;
        const double externalTemperature = state.input.celg[gasCellIndex].textern;
        const double gasSpecificHeat = gasCell.flui.CalorGas(productionPressure, externalTemperature);
        const double gasDensity = gasCell.flui.MasEspGas(productionPressure, externalTemperature);
        productionCell.calor.Textern1 = gasCell.calor.Textern1;
        productionCell.calor.Vextern1 = 1.;
        publishExternalFluid(productionCell, {gasCell.flui.CondGas(productionPressure, externalTemperature),
                                              gasSpecificHeat, gasDensity, kInitialAnnulusGasViscosityCentipoise});
    });
}

void connectTubingSteady(const GasLiftState &state) {
    forEachCoupledCellPair(state, [&](int productionCellIndex, int gasCellIndex) {
        Cel &productionCell = state.cells[productionCellIndex];
        const CelG &gasCell = state.gasCells[gasCellIndex];
        productionCell.calor.Textern2 = gasCell.calor.Textern1;
        productionCell.calor.colunaDia = gasCell.duto.dia;

        const double gasPressure = gasCell.pres;
        const double gasTemperature = gasCell.temp;
        const double gasSpecificHeat = gasCell.flui.CalorGas(gasPressure, gasTemperature);
        const double gasDensity = gasCell.flui.MasEspGas(gasPressure, gasTemperature);
        productionCell.calor.Textern1 = gasCell.calor.Textern1;
        productionCell.calor.Vextern1 = gasCell.VGasR / gasCell.u1L;
        publishExternalFluid(productionCell, {gasCell.flui.CondGas(gasPressure, gasTemperature), gasSpecificHeat,
                                              gasDensity, gasCell.flui.ViscGas(gasPressure, gasTemperature)});
    });
}

void updateSteadyGasPressure(const GasLiftState &state, int cellIndex) {
    CelG &cell = state.gasCells[cellIndex];
    CelG &left = state.gasCells[cellIndex - 1];
    const double leftFaceGasMassFlowRate = left.VGasR;

    // The left neighbour's half, at its pressure and temperature; its friction and hydrostatic gradients are recorded
    // on it.
    double meanPressure;
    {
        const double dx = 0.5 * cell.dxL;
        const double diameter = cell.dutoL.a;
        const double area = boreArea(diameter);
        const double perimeter = cell.dutoL.peri;
        const double gasDensity = left.flui.MasEspGas(left.pres, left.temp);
        const double gasVelocity = leftFaceGasMassFlowRate / left.u1L;
        const double gasViscosity = left.flui.ViscGas(left.pres, left.temp);
        const double reynolds = left.Rey(characteristicDiameter(cell.dutoL.revest, cell.dutoL.a, area, perimeter),
                                         gasVelocity, gasDensity, gasViscosity);
        const double frictionFactor = left.fric(reynolds, cell.dutoL.rug / diameter);
        const double frictionGradient = left.dPdLFric * 0.5 * frictionFactor * gasDensity *
                                        (fabs(gasVelocity) * gasVelocity) * perimeter * dx / area;
        const double hydrostaticGradient = left.dPdLHidro * kGravity * sin(cell.dutoL.teta) * gasDensity * dx;
        left.termoFric = frictionGradient / dx;
        left.termoHidro = hydrostaticGradient / dx;
        meanPressure = left.pres - (frictionGradient + hydrostaticGradient) / units::kPascalPerKgfPerCm2;
    }

    // The cell's own half, from the pressure the left half reaches, with the dynamic pressure of the change of flow
    // area between the two cells.
    {
        const double meanTemperature = left.temp;
        const double dx = 0.5 * cell.dx0;
        const double diameter = cell.duto.a;
        const double area = boreArea(diameter);
        const double perimeter = cell.duto.peri;
        const double gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
        const double leftFlowArea = cell.dutoL.area;
        const double flowArea = cell.duto.area;
        const double areaVariation = 1 / (leftFlowArea * leftFlowArea) - 1 / (flowArea * flowArea);
        const double dynamicPressureTerm =
            0.5 * leftFaceGasMassFlowRate * leftFaceGasMassFlowRate * areaVariation / gasDensity;
        const double gasVelocity = leftFaceGasMassFlowRate / (gasDensity * area);
        const double gasViscosity = cell.flui.ViscGas(meanPressure, meanTemperature);
        const double reynolds = cell.Rey(characteristicDiameter(cell.duto.revest, cell.duto.a, area, perimeter),
                                         gasVelocity, gasDensity, gasViscosity);
        const double frictionFactor = cell.fric(reynolds, cell.duto.rug / diameter);
        const double frictionGradient = cell.dPdLFric * 0.5 * frictionFactor * gasDensity *
                                        (fabs(gasVelocity) * gasVelocity) * perimeter * dx / area;
        const double hydrostaticGradient = cell.dPdLHidro * kGravity * sin(cell.duto.teta) * gasDensity * dx;
        cell.pres =
            meanPressure - (frictionGradient + hydrostaticGradient - dynamicPressureTerm) / units::kPascalPerKgfPerCm2;
    }
    left.presR = cell.pres;
    if (cellIndex < state.lastGasCell)
        state.gasCells[cellIndex + 1].presL = cell.pres;
    cell.presini = cell.pres;
}

void updateSteadyGasTemperature(const GasLiftState &state, int cellIndex) {
    if (state.thermalSourceDisabled) {
        CelG &cell = state.gasCells[cellIndex];
        cell.temp = cell.calor.Textern1;
        return;
    }
    const bool insideCoupling = cellIndex >= state.annulusCouplingStart && cellIndex <= state.annulusCouplingEnd;
    if (insideCoupling)
        coupledSteadyGasTemperature(state, cellIndex);
    else
        marchSteadyGasTemperature(state, cellIndex);
}

void marchSteadyGasTemperature(const GasLiftState &state, int cellIndex) {
    CelG &cell = state.gasCells[cellIndex];
    CelG &left = state.gasCells[cellIndex - 1];
    const double dx = cell.dx0;
    const double meanCellLength = 0.5 * (cell.dx0 + left.dx0);
    const double meanTemperatureGradient = (cell.dx0 * cell.dTdLCor + left.dx0 * left.dTdLCor) / (cell.dx0 + left.dx0);
    const double area = cell.duto.area;
    const double gasDensity = left.flui.MasEspGas(left.pres, left.temp);
    const double meanSuperficialGasVelocity = left.VGasR / left.u1L;
    const double gasSpecificHeat = left.flui.CalorGas(left.pres, left.temp);
    const double gasJouleThomson = left.flui.JTG(left.pres, left.temp);
    const double hydrostaticTerm = (gasDensity * meanSuperficialGasVelocity) * area * kGravity * sin(cell.duto.teta);

    publishInternalGas(left, left.temp, meanSuperficialGasVelocity,
                       {left.flui.CondGas(left.pres, left.temp), gasSpecificHeat, gasDensity,
                        left.flui.ViscGas(left.pres, left.temp)});

    const bool marches = meanSuperficialGasVelocity > kMinimumMarchGasVelocity &&
                         cell.duto.a / left.duto.a > kMinimumMarchDiameterRatio &&
                         left.duto.a / cell.duto.a > kMinimumMarchDiameterRatio;
    if (marches) {
        double leftCellHeatFlux = left.fluxcal;
        const double tubingHeatFlux = 0.;

        const double lengthRatio = dx / (dx + left.dx0);
        const double temperatureMarchCoefficient = gasDensity * meanSuperficialGasVelocity * gasSpecificHeat * area;
        const double pressureMarchCoefficient = gasDensity * meanSuperficialGasVelocity * gasJouleThomson * area;
        const double pressureGradient = 2. * (cell.pres - ((1 - lengthRatio) * left.pres + lengthRatio * cell.pres)) *
                                        units::kPascalPerKgfPerCm2 / dx;

        const double leftCellLeftFaceSuperficialGasVelocity = left.VGasL / left.u1L;
        const double superficialVelocityGradient =
            (meanSuperficialGasVelocity - leftCellLeftFaceSuperficialGasVelocity) / dx;

        const double kineticTerm = state.input.nCompTotalUnidadesG / dx < kKineticTermMaximumLengthRatio
                                       ? gasDensity * area * meanSuperficialGasVelocity * meanSuperficialGasVelocity *
                                             superficialVelocityGradient
                                       : 0.;

        const double gasMassSource = 0.;
        const double liquidMassSource = 0.;

        const double workTerm = meanTemperatureGradient *
                                (pressureMarchCoefficient * pressureGradient - kineticTerm - hydrostaticTerm +
                                 liquidMassSource + gasMassSource) /
                                temperatureMarchCoefficient;
        double heatTerm = meanTemperatureGradient * (leftCellHeatFlux + tubingHeatFlux) / temperatureMarchCoefficient;
        const double lengthOverResistance = meanCellLength / left.calor.resGlob;
        int stepCount;
        double stepLength;
        if (lengthOverResistance < kThermalSubstepLength) {
            stepCount = 1;
            stepLength = meanCellLength;
        } else {
            stepCount = static_cast<int>(2 * lengthOverResistance / kThermalSubstepLength + 1);
            stepLength = meanCellLength / stepCount;
        }
        double stepTemperature = stepLength * (left.temp / stepLength + workTerm + heatTerm);
        for (int stepIndex = 1; stepIndex < stepCount; stepIndex++) {
            publishInternalGas(left, stepTemperature, meanSuperficialGasVelocity,
                               {left.flui.CondGas(left.pres, stepTemperature), gasSpecificHeat, gasDensity,
                                left.flui.ViscGas(left.pres, stepTemperature)});
            leftCellHeatFlux = left.fluxcal;
            heatTerm = meanTemperatureGradient * (leftCellHeatFlux + tubingHeatFlux) / temperatureMarchCoefficient;
            stepTemperature = stepLength * (stepTemperature / stepLength + workTerm + heatTerm);
        }

        cell.temp = stepTemperature;
    } else {
        cell.temp = cell.calor.Textern1;
    }

    clampAndPropagateTemperature(state, cellIndex);
}

void coupledSteadyGasTemperature(const GasLiftState &state, int cellIndex) {
    CelG &cell = state.gasCells[cellIndex];
    CelG &left = state.gasCells[cellIndex - 1];
    if (state.annulusThermalCoupling) {
        const int connectionIndex = facingCouplingCell(state, cellIndex);
        const int secondConnectionIndex = connectionIndex == state.lastCell ? connectionIndex - 1 : connectionIndex;

        const Cel &facingCell = state.cells[connectionIndex];
        cell.temp = state.cells[secondConnectionIndex].calor.resGlob * facingCell.calor.fluxFim + facingCell.temp;

        if (state.input.correcaoContracorPerm == 1) {
            const double meanCellLength = 0.5 * (cell.dx0 + left.dx0);
            double gasSpecificHeat = left.flui.CalorGas(left.pres, left.temp);
            double inverseHeatCapacity =
                1. / (facingCell.calor.cpint * facingCell.MC) + 1. / (gasSpecificHeat * left.VGasR);
            const double logTemperature = -exp(-(1. / state.cells[secondConnectionIndex].calor.resGlob) *
                                         (1 + 0 * facingCell.duto.peri) * meanCellLength * inverseHeatCapacity) *
                                        (state.cells[secondConnectionIndex + 1].temp - left.temp) +
                                    state.cells[secondConnectionIndex].temp;

            cell.temp = std::min(cell.temp, logTemperature);
        }
    }

    clampAndPropagateTemperature(state, cellIndex);

    const double meanSuperficialGasVelocity = left.VGasR / left.u1L;
    const double gasDensity = left.flui.MasEspGas(left.pres, left.temp);
    double gasSpecificHeat = left.flui.CalorGas(left.pres, left.temp);

    publishInternalGas(cell, cell.temp, meanSuperficialGasVelocity,
                       {left.flui.CondGas(left.pres, left.temp), gasSpecificHeat, gasDensity,
                        left.flui.ViscGas(left.pres, left.temp)});
}

void computeSteadyGasFlowRate(const GasLiftState &state, const thermal::ThermalState &dependencies, int cellIndex) {
    CelG &cell = state.gasCells[cellIndex];
    const int valveCount = state.input.nvalvgas;
    bool matched = false;
    for (int valveIndex = 0; valveIndex < valveCount; valveIndex++) {
        if (state.gasValveCellIndices[valveIndex] == cellIndex) {
            matched = true;
            Cel &productionCell = state.cells[state.productionValveCellIndices[valveIndex]];
            cell.massfonteCH = primeValveMassSource<kGasThroughValve>(state, valveIndex, cell, productionCell);
            cell.VGasR = cellIndex > 0 ? state.gasCells[cellIndex - 1].VGasR - cell.massfonteCH : cell.massfonteCH;

            productionCell.acsr.injg.QGas =
                cell.massfonteCH * units::kSecondsPerDay / (kAirDensityAtStandardConditions * cell.flui.Deng);
            productionCell.acsr.injg.temp = injectedGasTemperature(state, dependencies, valveIndex, productionCell);
            productionCell.fontemassGR = cell.massfonteCH;

            if (cell.VGasR <= 0. && cellIndex < state.lastGasCell) {
                cell.VGasR = 0.;
                for (int laterValveIndex = valveIndex + 1; laterValveIndex < valveCount; laterValveIndex++) {
                    state.gasCells[state.gasValveCellIndices[laterValveIndex]].massfonteCH = 0;
                    state.cells[state.productionValveCellIndices[laterValveIndex]].fontemassGR = 0.;
                }
            }

            if (cellIndex > 0)
                state.gasCells[cellIndex - 1].VGasRR = cell.VGasR;
            if (cellIndex < state.lastGasCell)
                state.gasCells[cellIndex + 1].VGasL = cell.VGasR;
        }
    }
    if (!matched) {
        CelG &left = state.gasCells[cellIndex - 1];
        cell.VGasR = left.VGasR - 0 * cell.massfonteCH;
        left.VGasRR = cell.VGasR;
        if (cellIndex < state.lastGasCell)
            state.gasCells[cellIndex + 1].VGasL = cell.VGasR;
    }
}

void initializeSteadyValveGasFlowRate(const GasLiftState &state, int cellIndex) {
    const int valveCount = state.input.nvalvgas;
    const double casingTemperature = state.gasCells[0].calor.Textern1;
    for (int valveIndex = 0; valveIndex < valveCount; valveIndex++) {
        const int productionValveCell = state.productionValveCellIndices[valveIndex];
        if (cellIndex != productionValveCell)
            continue;
        Cel &productionCell = state.cells[productionValveCell];
        const CelG &valveCell = state.gasCells[state.gasValveCellIndices[valveIndex]];
        // This site reads the flag the other way round from the rest: "1 is the flow
        // rate, anything else the pressure", where every other reads "0 is the
        // pressure, anything else the flow rate". The readings differ only for a
        // value outside {0, 1}, which the input does not reject.
        withGasInletCondition(state.gasCells[0].tipoCC == kGasInletInjectionFlowRate ? kGasInletInjectionFlowRate
                                                                                     : kGasInletInjectionPressure,
                              [&](auto condition) {
                                  setSteadyValveGasMassSource(state, valveIndex, valveCount, casingTemperature,
                                                              condition);
                              });
        productionCell.acsr.injg.QGas =
            valveCell.massfonteCH * units::kSecondsPerDay /
            valveCell.flui.MasEspGas(kStandardPressureKgfPerCm2, kStandardTemperatureCelsius);
        productionCell.acsr.injg.temp = casingTemperature;
        productionCell.fontemassGR = valveCell.massfonteCH;
    }
}

double steadyGasPressureDrop(const GasLiftState &state, int cellIndex) {
    CelG &cell = state.gasCells[cellIndex];
    CelG &left = state.gasCells[cellIndex - 1];
    const double leftFaceGasMassFlowRate = left.VGasR;

    // The cell's own half, at its pressure and temperature.
    double pressureDrop;
    {
        const double dx = 0.5 * cell.dx0;
        const double diameter = cell.duto.a;
        const double area = boreArea(diameter);
        const double perimeter = cell.duto.peri;
        const double gasDensity = cell.flui.MasEspGas(cell.pres, cell.temp);
        const double gasVelocity = leftFaceGasMassFlowRate / cell.u1L;
        const double gasViscosity = cell.flui.ViscGas(cell.pres, cell.temp);
        const double reynolds = cell.Rey(characteristicDiameter(cell.duto.revest, cell.duto.a, area, perimeter),
                                         gasVelocity, gasDensity, gasViscosity);
        const double frictionFactor = cell.fric(reynolds, cell.duto.rug / diameter);
        const double frictionGradient = cell.dPdLFric * (0.5 * frictionFactor * gasDensity *
                                                         (fabs(gasVelocity) * gasVelocity) * perimeter * dx / area);
        const double hydrostaticGradient = cell.dPdLHidro * (kGravity * sin(cell.duto.teta) * gasDensity * dx);
        pressureDrop = (frictionGradient + hydrostaticGradient) / units::kPascalPerKgfPerCm2;
    }

    // The left neighbour's half, at the pressure the cell's half reaches and the mean temperature of the two cells.
    const double meanPressure = cell.pres + pressureDrop;
    const double meanTemperature = (cell.dx0 * cell.temp + cell.dxL * cell.tempL) / (cell.dx0 + cell.dxL);
    {
        const double dx = 0.5 * cell.dxL;
        const double diameter = cell.dutoL.a;
        const double area = boreArea(diameter);
        const double perimeter = cell.dutoL.peri;
        const double gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
        const double gasVelocity = leftFaceGasMassFlowRate / (gasDensity * area);
        const double gasViscosity = left.flui.ViscGas(meanPressure, meanTemperature);
        const double reynolds = cell.Rey(characteristicDiameter(cell.dutoL.revest, cell.dutoL.a, area, perimeter),
                                         gasVelocity, gasDensity, gasViscosity);
        const double frictionFactor = left.fric(reynolds, cell.dutoL.rug / diameter);
        const double frictionGradient = left.dPdLFric * (0.5 * frictionFactor * gasDensity *
                                                         (fabs(gasVelocity) * gasVelocity) * perimeter * dx / area);
        const double hydrostaticGradient = left.dPdLHidro * (kGravity * sin(cell.dutoL.teta) * gasDensity * dx);
        pressureDrop += (frictionGradient + hydrostaticGradient) / units::kPascalPerKgfPerCm2;
    }
    return pressureDrop;
}

double steadyInjectionPressureDrop(const GasLiftState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &left = state.cells[cellIndex - 1];

    // The cell's own half, at its auxiliary pressure and the mean temperature of the two cells, with the flow from the
    // left neighbour.
    double pressureDrop;
    double fluidVelocity;
    {
        const double dx = 0.5 * cell.dx;
        const double diameter = cell.duto.a;
        const double area = boreArea(diameter);
        const double perimeter = cell.duto.peri;
        const double meanTemperature = (cell.dx * cell.temp + cell.dxL * left.temp) / (cell.dx + cell.dxL);
        const double injectedFluidDensity = cell.fluicol.MasEspFlu(cell.presaux, meanTemperature);
        fluidVelocity = left.QL / area;
        const double fluidViscosity = cell.fluicol.VisFlu(cell.presaux, meanTemperature);
        const double reynolds = cell.Rey(characteristicDiameter(cell.duto.revest, cell.duto.a, area, perimeter),
                                         fluidVelocity, injectedFluidDensity, fluidViscosity);
        const double frictionFactor = cell.fric(reynolds, cell.duto.rug / diameter);
        const double frictionGradient = cell.dPdLFric * 0.5 * frictionFactor * injectedFluidDensity *
                                        (fabs(fluidVelocity) * fluidVelocity) * perimeter * dx / area;
        const double hydrostaticGradient = cell.dPdLHidro * kGravity * sin(cell.duto.teta) * injectedFluidDensity * dx;
        pressureDrop = (frictionGradient + hydrostaticGradient) / units::kPascalPerKgfPerCm2;
    }

    // The left neighbour's half, at its pressure and temperature, with the flow from the cell before it.
    const double meanPressure = left.pres;
    {
        const double dx = 0.5 * cell.dxL;
        const double diameter = cell.dutoL.a;
        const double area = boreArea(diameter);
        const double perimeter = cell.dutoL.peri;
        const double meanTemperature = left.temp;
        const double injectedFluidDensity = cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        // Cell 1 has no cell before its left neighbour: its left half keeps the velocity of its own half.
        const double leftFluidVelocity = cellIndex > 1 ? state.cells[cellIndex - 2].QL / area : fluidVelocity;
        const double fluidViscosity = cell.fluicol.VisFlu(meanPressure, meanTemperature);
        const double reynolds = cell.Rey(characteristicDiameter(cell.dutoL.revest, cell.dutoL.a, area, perimeter),
                                         leftFluidVelocity, injectedFluidDensity, fluidViscosity);
        const double frictionFactor = left.fric(reynolds, cell.dutoL.rug / diameter);
        const double frictionGradient = left.dPdLFric * 0.5 * frictionFactor * injectedFluidDensity *
                                        (fabs(leftFluidVelocity) * leftFluidVelocity) * perimeter * dx / area;
        const double hydrostaticGradient = left.dPdLHidro * kGravity * sin(cell.dutoL.teta) * injectedFluidDensity * dx;
        pressureDrop += (frictionGradient + hydrostaticGradient) / units::kPascalPerKgfPerCm2;
    }
    return pressureDrop;
}

void setSteadyValveGasMassSource(const GasLiftState &state, int valveIndex, int valveCount, double,
                                 InjectionFlowRateCondition) {
    const double valveFlowRate = (state.input.gasinj.vazgas[0] *
                            state.gasCells[0].flui.MasEspGas(kStandardPressureKgfPerCm2, kStandardTemperatureCelsius) /
                            units::kSecondsPerDay) /
                           valveCount;
    state.gasCells[state.gasValveCellIndices[valveIndex]].massfonteCH = valveFlowRate;
}

void setSteadyValveGasMassSource(const GasLiftState &state, int valveIndex, int, double casingTemperature,
                                 InjectionPressureCondition) {
    const int gasValveCell = state.gasValveCellIndices[valveIndex];
    ChokeGas &choke = state.gasLiftChokes[valveIndex];
    const Cel &productionCell = state.cells[state.productionValveCellIndices[valveIndex]];
    const double casingPressure = state.gasCells[gasValveCell].pres;
    choke.presEstag = casingPressure;
    choke.presGarg = productionCell.pres;
    choke.tempEstag = casingTemperature;
    state.gasCells[gasValveCell].massfonteCH = productionCell.pres < casingPressure ? choke.massica() : 0.;
}

void clampAndPropagateTemperature(const GasLiftState &state, int cellIndex) {
    CelG &cell = state.gasCells[cellIndex];
    cell.temp = std::clamp(cell.temp, kMinimumTemperatureCelsius, kMaximumTemperatureCelsius);

    if (cellIndex > 0)
        state.gasCells[cellIndex - 1].tempR = cell.temp;
    if (cellIndex < state.lastGasCell)
        state.gasCells[cellIndex + 1].tempL = cell.temp;
}

void publishInternalGas(CelG &cell, double temperature, double velocity, const FluidProperties &gas) {
    cell.calor.Tint = temperature;
    cell.calor.Vint = velocity;
    cell.calor.kint = gas.conductivity;
    cell.calor.cpint = gas.specificHeat;
    cell.calor.rhoint = gas.density;
    cell.calor.viscint = gas.viscosityCentipoise * units::kPascalSecondPerCentipoise;
    cell.fluxcal = cell.calor.transperm();
}

double boreArea(double diameter) {
    return 0.25 * std::numbers::pi * diameter * diameter;
}

// What every phase uses: the gas-lift valves, the column-annulus coupling and the Reynolds diameter.

template <ValveFluids fluids>
double primeValveMassSource(const GasLiftState &state, int valveIndex, const CelG &gasCell, const Cel &productionCell) {
    ChokeGas &choke = state.gasLiftChokes[valveIndex];
    const double recovery = fluids.recovery == kInjectedGas ? choke.frec : choke.frecliq;
    choke.presEstag = gasCell.pres;
    choke.presGarg = (productionCell.pres - choke.presEstag * recovery) / (1. - recovery);
    choke.tempEstag = gasCell.temp;
    double massSource = 0.;
    if (productionCell.pres < gasCell.pres)
        massSource = fluids.flow == kInjectedGas ? choke.massica()
                                                 : choke.massica(kInjectedCompletionFluid, state.input.salinDescarga);
    if (choke.tipo == kGasLiftCalibratedValve)
        massSource *= calibratedValveOpening(choke);
    return massSource;
}

double calibratedValveOpening(const ChokeGas &choke) {
    return calibratedValveOpeningFraction(choke.pcalib * units::kPsiPerKgfPerCm2, choke.tcalib,
                                          (choke.presEstag - units::kAtmosphereInKgfPerCm2) * units::kPsiPerKgfPerCm2,
                                          (choke.presGarg - units::kAtmosphereInKgfPerCm2) * units::kPsiPerKgfPerCm2,
                                          choke.dextern, choke.areagarg, choke.areagarg / choke.areafole,
                                          units::celsiusToFahrenheit(choke.tempEstag));
}

double calibratedValveOpeningFraction(double calibrationPressure, double calibrationTemperature,
                                      double valveOpeningPressure, double tubingPressure, double externalDiameter,
                                      double throatArea, double valveRatio, double bottomHoleTemperatureFahrenheit) {
    double domePressure = calibrationPressure * (1 - valveRatio);
    const double calibrationTemperatureRankine =
        units::celsiusToFahrenheit(calibrationTemperature) + units::kZeroFahrenheitInRankine;
    domePressure = (domePressure + units::kAtmosphereInPsi) *
                       (kBellowsReferenceTemperatureFahrenheit + units::kZeroFahrenheitInRankine) /
                       calibrationTemperatureRankine -
                   units::kAtmosphereInPsi;
    const double bellowsPressure =
        domePressure * (1 + kBellowsPressureChangePerFahrenheit *
                                (bottomHoleTemperatureFahrenheit - kBellowsReferenceTemperatureFahrenheit));
    const double bellowsArea = throatArea / valveRatio;
    // The spring rate depends only on whether the valve is a large-bore one.
    const double externalDiameterInches = externalDiameter * units::kCentimetrePerMetre / units::kCentimetrePerInch;
    const double bellowsSpringRate =
        (externalDiameterInches > kLargeBoreValveInches ? kLargeBoreSpringRate : kSmallBoreSpringRate) * bellowsArea;
    const double stemTravel = ((valveOpeningPressure - bellowsPressure) * bellowsArea -
                         (valveOpeningPressure - tubingPressure) * throatArea) /
                        bellowsSpringRate;

    const double throatRadius = sqrt(throatArea * 4. / std::numbers::pi) / 2.0;
    const double bellowsRadius = sqrt(bellowsArea / std::numbers::pi);
    // The seat offset: the distance from the stem axis to where the bellows
    // meets the seat.
    const double seatOffset = sqrt(bellowsRadius * bellowsRadius - throatRadius * throatRadius);
    const double openingArea =
        std::numbers::pi * throatRadius * stemTravel * (stemTravel + 2.0 * seatOffset) /
        sqrt((stemTravel + seatOffset) * (stemTravel + seatOffset) + throatRadius * throatRadius);
    return std::min(openingArea, throatArea) / throatArea;
}

double injectedGasTemperature(const GasLiftState &state, const thermal::ThermalState &dependencies, int valveIndex, const Cel &productionCell) {
    const ChokeGas &choke = state.gasLiftChokes[valveIndex];
    const double temperature = choke.presEstag > choke.presGarg
                                   ? thermal::computeGasLiftDischargeTemperature(dependencies, valveIndex)
                                   : productionCell.temp;
    return std::max(temperature, kMinimumTemperatureCelsius);
}

template <typename Visit>
void forEachCoupledCellPair(const GasLiftState &state, Visit &&visit) {
    for (int productionCellIndex = state.tubingCouplingStart; productionCellIndex >= state.tubingCouplingEnd;
         productionCellIndex--)
        visit(productionCellIndex, facingCouplingCell(state, productionCellIndex));
}

int facingCouplingCell(const GasLiftState &state, int cellIndex) {
    return state.tubingCouplingStart + state.annulusCouplingStart - cellIndex;
}

void publishExternalFluid(Cel &productionCell, const FluidProperties &fluid) {
    productionCell.calor.kextern1 = fluid.conductivity;
    productionCell.calor.cpextern1 = fluid.specificHeat;
    productionCell.calor.rhoextern1 = fluid.density;
    productionCell.calor.viscextern1 = fluid.viscosityCentipoise * units::kPascalSecondPerCentipoise;
}

double characteristicDiameter(int cased, double bore, double area, double perimeter) {
    return cased == 0 ? bore : 4 * area / perimeter;
}

}  // namespace sisprod::gaslift
