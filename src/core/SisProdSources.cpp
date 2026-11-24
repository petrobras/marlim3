#include "SisProdSources.h"
#include "SisProdSourcesInternal.h"

#include "Leitura.h"
#include "SisProdConstants.h"
#include "celula3.h"
#include "SimulationContext.h"
#include "UnitConversions.h"

#include <algorithm>
#include <math.h>

namespace sisprod::sources {

using enum sisprod::AccessoryKind;
using sisprod::kAirDensityAtStandardConditions;
using sisprod::kGravity;

namespace {

void consumeHydrateFormationMass(const SourceState &state, double &consumedGasMass, double &consumedWaterMass, int cellIndex) {
    const bool hydrateModel2Active = state.input.calculaEnvelope == 1 && state.input.tipoHmodel == 2 && (*state.globals).simulationTime > 0.01;
    const bool hydrateModel3Active = state.input.calculaEnvelope == 1 && state.input.tipoHmodel == 3 && (*state.globals).simulationTime > 0.01;
    if (hydrateModel2Active || hydrateModel3Active) {

        consumedWaterMass = state.cells[cellIndex].waterMassConsumedByHydratesThisStep;

        consumedGasMass = state.cells[cellIndex].gasMassConsumedByHydratesThisStep;

        // Update the BSW
        double crossSectionArea = state.cells[cellIndex].duto.area;
        double cellLength = state.cells[cellIndex].dx;
        double availableFluidVolume = std::max(crossSectionArea * cellLength -
                                              (hydrateModel2Active ? state.cells[cellIndex].V_h : state.cells[cellIndex].V_h_total), 1e-12);

        double waterVolumeFraction = std::max((1-state.cells[cellIndex].alfR)*(1-state.cells[cellIndex].betR)*state.cells[cellIndex].FW, 1e-12);
        double oilVolumeFraction = std::max((1-state.cells[cellIndex].alfR)*(1-state.cells[cellIndex].betR)*(1-state.cells[cellIndex].FW), 1e-12);

        double waterVolume = waterVolumeFraction * availableFluidVolume;
        double oilVolume = oilVolumeFraction * availableFluidVolume;

        double waterDensity = std::max(state.cells[cellIndex].flui.MasEspAgua(state.cells[cellIndex].pres, state.cells[cellIndex].temp), 1e-12);
        double remainingWaterVolume = waterVolume - consumedWaterMass / waterDensity;
        if (remainingWaterVolume < 0.0) remainingWaterVolume = 0.0;

        double remainingProducedLiquidVolume = oilVolume + remainingWaterVolume;
        if (remainingProducedLiquidVolume > 1e-12) {
            state.cells[cellIndex].flui.BSW = remainingWaterVolume / remainingProducedLiquidVolume;
        }

    }
}

double inSituWaterFraction(const Cel &receivingCell, Cel &donorCell) {
    double oilFormationVolumeFactor;
    double waterFormationVolumeFactor;
    if (receivingCell.flui.RGO < 1e7) {
        double solutionGasOilRatio = donorCell.flui.RS(receivingCell.pres, receivingCell.temp);
        oilFormationVolumeFactor = donorCell.flui.BOFunc(receivingCell.pres, receivingCell.temp, solutionGasOilRatio);
        waterFormationVolumeFactor = donorCell.flui.BAFunc(receivingCell.pres, receivingCell.temp);
    } else {
        oilFormationVolumeFactor = 1;
        waterFormationVolumeFactor = 0.;
    }
    return donorCell.flui.BSW * waterFormationVolumeFactor /
           (oilFormationVolumeFactor + waterFormationVolumeFactor * donorCell.flui.BSW - donorCell.flui.BSW * oilFormationVolumeFactor);
}

void refreshLeakSource(const SourceState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.fontechk;
    source.fluidoP = cell.flui;
    source.presT = cell.pres;
    source.tempT = cell.temp;
    double cellPressure = cell.pres;
    double cellTemperature = cell.temp;
    double voidFraction = cell.alf;
    double complementaryLiquidFraction = cell.bet;
    double gasDensity = cell.flui.MasEspGas(cellPressure, cellTemperature);
    double producedLiquidDensity = cell.flui.MasEspLiq(cellPressure, cellTemperature);
    double complementaryLiquidDensity = cell.fluicol.MasEspFlu(cellPressure, cellTemperature);
    source.titT = voidFraction * gasDensity /
                  (voidFraction * gasDensity + (1 - voidFraction) *
                   (complementaryLiquidFraction * complementaryLiquidDensity + (1 - complementaryLiquidFraction) * producedLiquidDensity));
    source.betIST = complementaryLiquidFraction;
    int parallelSourceIndex = -1;
    if (!((*state.globals).parallelNetworkMode == 0 || (*state.globals).networkIteration > 0 || state.parallelSecondaryBranch == 1 || state.parallelSecondaryBoundaryCondition == 1)) {
        int sourceCellCount = state.parallelSourceCells.size();
        for (int sourceIndex = 0; sourceIndex < sourceCellCount; sourceIndex++) {
            if (state.parallelSourceCells[sourceIndex] == cellIndex) {
                parallelSourceIndex = sourceIndex;
                break;
            }
        }
    }
    if (parallelSourceIndex < 0) {
        source.VMas();
        cell.fontemassLR += source.masP;
        cell.fontemassCR += source.masC;
        cell.fontemassGR += source.masG;
    } else {
        cell.fontemassLR += state.parallelSourceProductionLiquid[parallelSourceIndex];
        cell.fontemassCR += state.parallelSourceComplementaryLiquid[parallelSourceIndex];
        cell.fontemassGR += state.parallelSourceGas[parallelSourceIndex];
    }
}

void advanceRadialPorousSource(const SourceState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.radialPoro;
    source.pW.val[0] = cell.pres;
    // Use the next array cell's fluid at this cell's pressure and temperature.
    double waterVolumeFraction = inSituWaterFraction(cell, state.cells[cellIndex + 1]);
    source.sWPoc = waterVolumeFraction * (1. - source.satAconat) + source.satAconat;
    source.Pint = cell.pres;
    if (state.steadyMode == 0)
        source.avancoPressao();
    else
        source.pseudoTrans();
    cell.fontemassLR = source.fluxIni + source.fluxIniA;
    cell.fontemassCR = 0.;
    cell.fontemassGR = source.fluxIniG;
}

void advancePorous2DSource(const SourceState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.poroso2D;
    source.dados.pW.val[0] = cell.pres;
    // Use the next array cell's fluid at this cell's pressure and temperature.
    double waterVolumeFraction = inSituWaterFraction(cell, state.cells[cellIndex + 1]);
    source.sWPoc = waterVolumeFraction * (1. - source.dados.satAconat) + source.dados.satAconat;
    source.dados.transfer.sWPoc = source.sWPoc;
    source.dados.pInt = cell.pres;
    source.dados.transfer.Pint = cell.pres;
    if (state.steadyMode == 0)
        source.avancoPressao();
    else
        source.pseudoTransientePoroso();
    cell.fontemassLR = source.dados.transfer.fluxIni + source.dados.transfer.fluxIniA;
    cell.fontemassCR = 0.;
    cell.fontemassGR = source.dados.transfer.fluxIniG;
}

void refreshChokeMultipleAndPorousSources(const SourceState &state, int cellIndex) {
    if (state.cells[cellIndex].acsr.tipo == kAccessoryLeak) {
        refreshLeakSource(state, cellIndex);
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryMultipleSource) {
        Cel &cell = state.cells[cellIndex];
        if (cell.acsr.injm.condTermo == 1) {
            cell.fontemassLR = cell.acsr.injm.MassP;
            cell.fontemassCR = cell.acsr.injm.MassC;
            cell.fontemassGR = cell.acsr.injm.MassG;
        } else {
            cell.fontemassCR = cell.acsr.injm.MassC;
            double productionFluidMassFlowRate = cell.acsr.injm.MassP + cell.acsr.injm.MassG;
            double gasMassFraction = cell.acsr.injm.FluidoPro.FracMassHidra(cell.pres, cell.temp);
            cell.acsr.injm.MassG = cell.fontemassGR = productionFluidMassFlowRate * gasMassFraction;
            cell.acsr.injm.MassP = cell.fontemassLR = productionFluidMassFlowRate - cell.fontemassGR;
        }
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryRadialPorous) {
        advanceRadialPorousSource(state, cellIndex);
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryPorous2D) {
        advancePorous2DSource(state, cellIndex);
    }
}

void applyGasInjectionSource(const SourceState &state, int cellIndex, double cellPressure, double cellTemperature) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.injg;
    if (source.tipoflu == 0) {
        double gasMassFlowRate = source.VMas(cellPressure, cellTemperature);
        if (fabs(gasMassFlowRate) < (*state.globals).smallValueTolerance)
            gasMassFlowRate = 0.;
        if (source.seco == 1) {
            cell.fontemassGR += gasMassFlowRate;
            cell.fontemassLR = 0.;
            cell.fontemassCR = 0.;
        } else {
            double gasMassFraction;
            if (state.input.flashCompleto != 2)
                gasMassFraction = source.FluidoPro.FracMassHidra(1., 20.);
            else
                gasMassFraction = source.FluidoPro.dStockTankVaporMassFraction;
            double productionFluidMassFlowRate = gasMassFlowRate / gasMassFraction;
            gasMassFraction = source.FluidoPro.FracMassHidra(cellPressure, cellTemperature);
            cell.fontemassGR += productionFluidMassFlowRate * gasMassFraction;
            cell.fontemassLR += productionFluidMassFlowRate * (1. - gasMassFraction);
            double complementaryFluidStandardDensity = source.fluidocol.standardDensity();
            cell.fontemassCR += complementaryFluidStandardDensity * source.razCompGas * source.QGas / units::kSecondsPerDay;
        }
    } else {
        cell.fontemassGR += 0.;
        cell.fontemassLR = 0.;
        cell.fontemassCR = source.QGas;
    }
}

void applyLiquidInjectionSource(const SourceState &state, int cellIndex, double cellPressure, double cellTemperature) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.injl;
    double complementaryFluidStandardDensity = source.fluidocol.standardDensity();
    cell.fontemassCR += complementaryFluidStandardDensity * source.QLiq * source.bet / units::kSecondsPerDay;
    double standardProducedLiquidVolumeFlowRate = source.QLiq * (1. - source.bet) / units::kSecondsPerDay;
    double standardGasDensity = source.FluidoPro.Deng * kAirDensityAtStandardConditions;
    double standardProducedLiquidDensity = (1000 * 141.5 / (131.5 + source.FluidoPro.API)) * (1 - source.FluidoPro.BSW) + 1000. * source.FluidoPro.Denag * source.FluidoPro.BSW;
    double massPerStandardLiquidVolume = (standardProducedLiquidDensity + source.FluidoPro.RGO * standardGasDensity * (1 - source.FluidoPro.BSW));
    double productionFluidMassFlowRate = standardProducedLiquidVolumeFlowRate * massPerStandardLiquidVolume;
    double gasMassFraction = source.FluidoPro.FracMassHidra(cellPressure, cellTemperature);
    cell.fontemassLR += (1. - gasMassFraction) * productionFluidMassFlowRate;
    cell.fontemassGR += gasMassFraction * productionFluidMassFlowRate;
}

void applyInflowPerformanceSource(const SourceState &state, int cellIndex, double cellPressure, double cellTemperature) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.ipr;
    if (source.ICV != 0) {
        double annulusPressure = -1.;
        if (state.globals->simulationTime > 0.)
            annulusPressure = source.presAnul;
        source.VMasICV(cellPressure, cellTemperature, cell.alf, cell.bet,
                       cell.flui, cell.fontemassGR, cell.fontemassLR, annulusPressure);
        return;
    }
    if (cellPressure < source.Pres) {
        cell.fontemassLR += source.MasL(cellPressure, cellTemperature);
        cell.fontemassCR = 0.;
        cell.fontemassGR += source.MasG(cellPressure, cellTemperature);
    } else {
        double gasMassFraction;
        gasMassFraction = cell.alf * cell.flui.MasEspGas(cellPressure, cellTemperature) /
              (cell.alf * cell.flui.MasEspGas(cellPressure, cellTemperature) +
               (1. - cell.alf) * (1. - cell.bet) * cell.flui.MasEspLiq(cellPressure, cellTemperature) +
               (1. - cell.alf) * cell.bet * cell.fluicol.MasEspFlu(cellPressure, cellTemperature));
        cell.fontemassLR += (1. - cell.alf) * (1. - cell.bet) * source.VMas(cellPressure, cellTemperature) * cell.flui.MasEspLiq(cellPressure, cellTemperature);
        cell.fontemassCR += (1. - cell.alf) * cell.bet * source.VMas(cellPressure, cellTemperature) * cell.fluicol.MasEspFlu(cellPressure, cellTemperature);
        cell.fontemassGR += 1 * cell.alf * source.VMas(cellPressure, cellTemperature) * cell.flui.MasEspGas(cellPressure, cellTemperature);
        source.deriP *= (1. - cell.alf) * (1. - cell.bet) * cell.flui.MasEspLiq(cellPressure, cellTemperature);
        source.deriC *= (1. - cell.alf) * cell.bet * cell.fluicol.MasEspFlu(cellPressure, cellTemperature);
        source.deriG *= 1 * cell.alf * cell.flui.MasEspGas(cellPressure, cellTemperature);
    }
}

void refreshRecirculationFlow(const SourceState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &downstream = state.cells[cell.acsr.indJusRecic];
    auto &source = downstream.acsr.fontechk;
    source.fluidoP = downstream.flui;
    source.presT = downstream.pres;
    source.tempT = downstream.temp;
    double pressure = downstream.pres;
    double temperature = downstream.temp;
    double voidFraction = downstream.alf;
    double complementaryFraction = downstream.bet;
    double gasDensity = downstream.flui.MasEspGas(pressure, temperature);
    double producedDensity = downstream.flui.MasEspLiq(pressure, temperature);
    double complementaryDensity = downstream.fluicol.MasEspFlu(pressure, temperature);
    source.pamb = cell.pres;
    source.tamb = cell.temp;
    source.fluidoPamb = cell.flui;
    source.titT = voidFraction * gasDensity /
                  (voidFraction * gasDensity + (1 - voidFraction) *
                   (complementaryFraction * complementaryDensity + (1 - complementaryFraction) * producedDensity));
    source.betIST = complementaryFraction;
    pressure = cell.pres;
    temperature = cell.temp;
    voidFraction = cell.alf;
    complementaryFraction = cell.bet;
    gasDensity = cell.flui.MasEspGas(pressure, temperature);
    producedDensity = cell.flui.MasEspLiq(pressure, temperature);
    complementaryDensity = cell.fluicol.MasEspFlu(pressure, temperature);
    source.titamb = voidFraction * gasDensity /
                    (voidFraction * gasDensity + (1 - voidFraction) *
                     (complementaryFraction * complementaryDensity + (1 - complementaryFraction) * producedDensity));
    source.betISamb = complementaryFraction;
    source.VMas();
}

void applyVolumetricPumpSource(const SourceState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    auto &source = cell.acsr.bvol;
    state.cells[cellIndex + 1].fontemassLR = 0.;
    state.cells[cellIndex + 1].fontemassCR = 0.;
    state.cells[cellIndex + 1].fontemassGR = 0.;
    source.fluido = cell.flui;
    source.fluicol = cell.fluicol;
    if (fabs(source.freq) > 1) {
        double voidFraction = cell.alf;
        double complementaryLiquidFraction = cell.bet;
        double cellPressure = cell.pres;
        double cellTemperature = cell.temp;
        double rightCellPressure = cell.presR;
        double rightCellTemperature = cell.tempR;
        source.vazmass(cellPressure, cellTemperature, rightCellPressure, rightCellTemperature, complementaryLiquidFraction, voidFraction);
        cell.fontemassLR -= source.MLiqP;
        cell.fontemassCR -= source.MLiqC;
        cell.fontemassGR -= source.MGas;
        state.cells[cellIndex + 1].fontemassLR += source.MLiqP;
        state.cells[cellIndex + 1].fontemassCR += source.MLiqC;
        state.cells[cellIndex + 1].fontemassGR += source.MGas;
    }
}

}  // namespace

void addMasterValveFlow(const SourceState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    if (cell.acsr.chk.AreaGarg < (1e-3 + state.input.master1.razareaativ) * cell.duto.area && cell.acsr.chk.AreaGarg > 1e-3 * cell.duto.area) {
        Cel &nextCell = state.cells[cellIndex + 1];
        const double cellTemperature = cell.temp;
        const double cellVoidFraction = cell.alf;
        const double cellComplementaryLiquidFraction = cell.bet;
        const double flowDirectionSign = 1.;

        double cellProducedLiquidDensity = cell.flui.MasEspLiq(cell.pres, cell.temp);
        double cellComplementaryLiquidDensity = cell.fluicol.MasEspFlu(cell.pres, cell.temp);
        double cellMixedLiquidDensity = (1 - cellComplementaryLiquidFraction) * cellProducedLiquidDensity + cellComplementaryLiquidFraction * cellComplementaryLiquidDensity;

        double nextCellVoidFraction = nextCell.alf;
        double nextCellComplementaryLiquidFraction = nextCell.bet;
        double nextCellProducedLiquidDensity = nextCell.flui.MasEspLiq(nextCell.pres, nextCell.temp);
        double nextCellComplementaryLiquidDensity = nextCell.fluicol.MasEspFlu(nextCell.pres, nextCell.temp);
        double nextCellMixedLiquidDensity = (1 - nextCellComplementaryLiquidFraction) * nextCellProducedLiquidDensity + nextCellComplementaryLiquidFraction * nextCellComplementaryLiquidDensity;

        double cellHydrostaticPressureCorrection = sin(cell.duto.teta) * (0.5 * cell.dx) *
            (cellMixedLiquidDensity * (1 - cellVoidFraction) + cellVoidFraction * cell.flui.MasEspGas(cell.pres, cell.temp)) *
            kGravity / units::kPascalPerKgfPerCm2;
        double nextCellHydrostaticPressureCorrection = sin(nextCell.duto.teta) * (0.5 * nextCell.dx) *
            (nextCellMixedLiquidDensity * (1 - nextCellVoidFraction) + nextCellVoidFraction * nextCell.flui.MasEspGas(nextCell.pres, nextCell.temp)) *
            kGravity / units::kPascalPerKgfPerCm2;

        double incomingTotalMassFlowRate = cell.MC;
        double incomingGasMassFlowRate = cell.MC - cell.Mliqini;
        double gasMassFraction;
        if (fabs(incomingTotalMassFlowRate) < 1e-9 && fabs(incomingGasMassFlowRate) < 1e-9) {
            gasMassFraction = cellVoidFraction * cell.flui.MasEspGas(cell.pres, cell.temp) / (cell.flui.MasEspGas(cell.pres, cell.temp) * cellVoidFraction + cellMixedLiquidDensity * (1. - cellVoidFraction));
        } else {
            if ((incomingGasMassFlowRate >= 0 && cell.Mliqini <= 0) || (incomingGasMassFlowRate < 0 && cell.Mliqini == 0))
                gasMassFraction = 1.;
            else if (incomingGasMassFlowRate <= 0 && cell.Mliqini > 0)
                gasMassFraction = 0.;
            else if (incomingTotalMassFlowRate < 0)
                gasMassFraction = 1.;
            else
                gasMassFraction = fabs(incomingGasMassFlowRate / incomingTotalMassFlowRate);
            if (gasMassFraction > 1)
                gasMassFraction = 1;
        }

        const double pressureRatio = (nextCell.pres + nextCellHydrostaticPressureCorrection) / (cell.pres - cellHydrostaticPressureCorrection);
        const double calculatedChokeMassFlowRate = cell.acsr.chk.vazmassSachd(
            pressureRatio, cell.pres - cellHydrostaticPressureCorrection, cellTemperature, cellVoidFraction,
            cellComplementaryLiquidFraction, gasMassFraction, cell.flui, cell.fluicol);
        double chokeMassFlowRate = cell.acsr.chk.vazmaxSachd(
            cell.pres - cellHydrostaticPressureCorrection, cellTemperature, cellVoidFraction,
            cellComplementaryLiquidFraction, gasMassFraction, cell.flui, cell.fluicol);

        if (fabs(pressureRatio) > fabs(cell.acsr.chk.razpres))
            chokeMassFlowRate = calculatedChokeMassFlowRate;

        if (cell.acsr.chk.AreaGarg < (1e-3) * cell.duto.area || (cell.pres < nextCell.pres))
            chokeMassFlowRate = 0.;

        const double chokeLiquidMassFlowRate = flowDirectionSign * chokeMassFlowRate * (1. - gasMassFraction);
        const double chokeGasMassFlowRate = flowDirectionSign * chokeMassFlowRate * gasMassFraction;

        cell.fontemassLR += (-1 * chokeLiquidMassFlowRate * (1 - cellComplementaryLiquidFraction) * cellProducedLiquidDensity / cellMixedLiquidDensity);
        cell.fontemassCR += (-1 * chokeLiquidMassFlowRate * cellComplementaryLiquidFraction * cellComplementaryLiquidDensity / cellMixedLiquidDensity);
        cell.fontemassGR += (-1 * chokeGasMassFlowRate);

        nextCell.fontemassLR += (chokeLiquidMassFlowRate * (1 - cellComplementaryLiquidFraction) * cellProducedLiquidDensity / cellMixedLiquidDensity);
        nextCell.fontemassCR += (chokeLiquidMassFlowRate * cellComplementaryLiquidFraction * cellComplementaryLiquidDensity / cellMixedLiquidDensity);
        nextCell.fontemassGR += (chokeGasMassFlowRate);
    }
}

void prepareRecirculationSourceRates(const SourceState &state) {
    for (int index = 0; index < state.input.nfuro; index++) {
        if (state.input.furo[index].recircula == 1)
            refreshRecirculationFlow(state, state.input.furo[index].indMon);
    }
}

void renewSourceTerms(const SourceState &state, int cellIndex) {

    double cellPressure = state.cells[cellIndex].pres;
    double cellTemperature = state.cells[cellIndex].temp;
    if (cellIndex == 0 ||
        (state.cells[cellIndex - 1].acsr.tipo != kAccessoryChoke &&
         state.cells[cellIndex - 1].acsr.tipo != kAccessoryVolumetricPump &&
         (cellIndex < state.lastCell || state.cells[state.lastCell].acsr.tipo == kAccessoryInflowPerformance))) {
        state.cells[cellIndex].fontemassLR = 0.;
        state.cells[cellIndex].fontemassCR = 0.;
        state.cells[cellIndex].fontemassGR = 0.;
    }

    double consumedWaterMass = 0.;
    double consumedGasMass = 0.;

    consumeHydrateFormationMass(state, consumedGasMass, consumedWaterMass, cellIndex);
    if (state.cells[cellIndex].acsr.tipo == kAccessoryGasInjection) {
        applyGasInjectionSource(state, cellIndex, cellPressure, cellTemperature);
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryLiquidInjection) {
        applyLiquidInjectionSource(state, cellIndex, cellPressure, cellTemperature);
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryInflowPerformance) {
        applyInflowPerformanceSource(state, cellIndex, cellPressure, cellTemperature);
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryChoke) {
        state.cells[cellIndex + 1].fontemassLR = 0.;
        state.cells[cellIndex + 1].fontemassCR = 0.;
        state.cells[cellIndex + 1].fontemassGR = 0.;
        if (state.steadyMode == 0)
            addMasterValveFlow(state, cellIndex);
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryVolumetricPump) {
        applyVolumetricPumpSource(state, cellIndex);
    }
    refreshChokeMultipleAndPorousSources(state, cellIndex);

    if (state.cells[cellIndex].acsr.tipo == kAccessoryRecirculationUpstream) {
        Cel &cell = state.cells[cellIndex];
        const auto &source = state.cells[cell.acsr.indJusRecic].acsr.fontechk;
        cell.fontemassLR -= source.masP;
        cell.fontemassCR -= source.masC;
        cell.fontemassGR -= source.masG;
    }
    if (state.cells[cellIndex].acsr.tipo == kAccessoryRecirculationDownstream) {
        Cel &cell = state.cells[cellIndex];
        const auto &source = cell.acsr.fontechk;
        cell.fontemassLR += source.masP;
        cell.fontemassCR += source.masC;
        cell.fontemassGR += source.masG;
    }

    const bool hydrateModel = state.input.calculaEnvelope == 1 && (state.input.tipoHmodel == 2 || state.input.tipoHmodel == 3);
    if (hydrateModel && state.cells[cellIndex].flui.BSW > 1e-12 && (*state.globals).simulationTime > 0.01) {
        state.cells[cellIndex].fontemassLR -= (consumedWaterMass / (*state.globals).simulationTime);
        state.cells[cellIndex].fontemassGR -= (consumedGasMass / (*state.globals).simulationTime);
    }
}

}  // namespace sisprod::sources
