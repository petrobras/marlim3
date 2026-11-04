#include "SisProdTrendOutput.h"
#include "SisProdFluidTracking.h"
#include "SisProdGasLift.h"
#include "SisProdThermal.h"
#include "SisProdTransient.h"
#include "SisProdTransientInternal.h"

#include "Acidentes2.h"
#include "Leitura.h"
#include "SisProdConstants.h"
#include "Matriz.h"
#include "Vetor.h"
#include "solver3DPoisson.h"
#include "celula3.h"
#include "celulaGas.h"
#include "SimulationContext.h"
#include "RunInfo.h"
#include "UnitConversions.h"

#include <math.h>
#include <numbers>

namespace sisprod::transient {

namespace {

const ProFlu &reverseFlowFluid(const ProFlu *collectorFluid) {
    static const ProFlu defaultFluid;
    return collectorFluid != nullptr ? *collectorFluid : defaultFluid;
}

double flowQuality(double gasMassFlow, double totalMassFlow) {
    return fabs(gasMassFlow / totalMassFlow);
}

double inSituGasMassFraction(const Cel &cell, double voidFraction, double pressure, double mixtureDensity) {
    return voidFraction * cell.flui.MasEspGas(pressure, cell.temp) / mixtureDensity;
}

void updateValveBoxStates(const TransientStepState &state) {
    for (int boxIndex = 0; boxIndex < state.input.vecCaixa.size(); boxIndex++) {
        const int cellIndex = state.input.vecCaixa[boxIndex].posic;
        state.input.vecCaixa[boxIndex].estadotermo(state.cells[cellIndex], state.cells[cellIndex + 1]);
    }
}

void writeLiquidInventory(const TransientStepState &state, std::ostream &output) {
    int masterCell = state.input.master1.posic;
    double liquidVolume = 0.;
    for (int j = 0; j < masterCell; j++) {
        liquidVolume += (1. - state.cells[j].alf) * state.cells[j].duto.area * state.cells[j].dx;
    }
    output << "Volume de liquido a montante da Master1 a P&T= " << liquidVolume << " (m3); " << endl;
    liquidVolume = 0.;
    for (int j = masterCell; j < state.input.ncelp; j++) {
        liquidVolume += (1. - state.cells[j].alf) * state.cells[j].duto.area * state.cells[j].dx;
    }
    output << "Volume de liquido a jusante da Master1 a P&T= " << liquidVolume << " (m3); " << endl;
    liquidVolume = 0.;
    for (int j = 0; j < masterCell; j++) {
        double pressure = state.cells[j].pres;
        double temperature = state.cells[j].temp;
        double oilVolumeFactor = state.cells[j].flui.BOFunc(pressure, temperature);
        double waterVolumeFactor = state.cells[j].flui.BAFunc(pressure, temperature);
        double waterCut = state.cells[j].FW;
        double complementaryFraction = state.cells[j].bet;
        double complementaryDensity = state.cells[j].fluicol.MasEspFlu(pressure, temperature);
        double standardComplementaryDensity = state.cells[j].fluicol.standardDensity();
        double liquidHoldup = (1. - state.cells[j].alf);
        double producedOilVolume = (1 - complementaryFraction) * (1 - waterCut) * liquidHoldup / oilVolumeFactor;
        double complementaryVolume =
            complementaryFraction * liquidHoldup * complementaryDensity / standardComplementaryDensity;
        double producedWaterVolume = (1 - complementaryFraction) * (waterCut)*liquidHoldup / waterVolumeFactor;
        liquidVolume += (producedOilVolume + complementaryVolume + producedWaterVolume) *
                        state.cells[j].duto.area * state.cells[j].dx;
    }
    output << "Volume de liquido a montante da Master1 'standard'= " << liquidVolume << " (Sm3); " << endl;
    liquidVolume = 0.;
    for (int j = masterCell; j < state.input.ncelp; j++) {
        double pressure = state.cells[j].pres;
        double temperature = state.cells[j].temp;
        double oilVolumeFactor = state.cells[j].flui.BOFunc(pressure, temperature);
        double waterVolumeFactor = state.cells[j].flui.BAFunc(pressure, temperature);
        double waterCut = state.cells[j].FW;
        double complementaryFraction = state.cells[j].bet;
        double complementaryDensity = state.cells[j].fluicol.MasEspFlu(pressure, temperature);
        double standardComplementaryDensity = state.cells[j].fluicol.standardDensity();
        double liquidHoldup = (1. - state.cells[j].alf);
        double producedOilVolume = (1 - complementaryFraction) * (1 - waterCut) * liquidHoldup / oilVolumeFactor;
        double complementaryVolume =
            complementaryFraction * liquidHoldup * complementaryDensity / standardComplementaryDensity;
        double producedWaterVolume = (1 - complementaryFraction) * (waterCut)*liquidHoldup / waterVolumeFactor;
        liquidVolume += (producedOilVolume + complementaryVolume + producedWaterVolume) *
                        state.cells[j].duto.area * state.cells[j].dx;
    }
    output << "Volume de liquido a jusante da Master1 'standard'= " << liquidVolume << " (Sm3); " << endl;
}

void reportAcceptedArtificialGas(const TransientStepState &state) {
    for (const diagnostics::ArtificialGasAddition &addition : state.artificialGasThisStep)
        diagnostics::recordArtificialGas(state.input.impfile, addition, state.stepIndex, (*state.globals).simulationTime,
                                         state.timeStep);
    state.artificialGasThisStep.clear();
}

void advanceCouplingIteration(const TransientSolveState &state, const TransientDependencies &dependencies, int kontaAcop, int celpos, int vExpli, int ciclomax, double titRev, double alfRev, double betRev) {
    prepareCouplingModelFlags(state, celpos);
    evolveFractions(state.step, alfRev, betRev, kontaAcop);
    for (int i = 0; i <= state.step.lastCell; i++) {
        if (state.step.cells[i].acsr.tipo == kAccessoryRadialPorous) {
            state.step.cells[i].acsr.radialPoro.avancoSW(state.step.timeStep);
            if (state.step.cells[i].acsr.radialPoro.reinicia == -1) {
                if (state.step.restart > -1)
                    state.step.restart = -1;
            }
        } else if (state.step.cells[i].acsr.tipo == kAccessoryPorous2D) {
            state.step.cells[i].acsr.poroso2D.avancoSW(state.step.timeStep);
            if (state.step.cells[i].acsr.poroso2D.reinicia == -1) {
                if (state.step.restart > -1)
                    state.step.restart = -1;
            }
        }
    }

    TimeStepPolicies::apply<TimeStepHook::CouplingIterationStart>(
        {state.step, kontaAcop, vExpli});
    restartCouplingFractions(state, dependencies, kontaAcop, alfRev, betRev);
    updatePig(state.step);

    if (kontaAcop == 0)
        state.minimumCycleTimeStep = state.step.timeStep;

    TimeStepPolicies::apply<TimeStepHook::AfterPigUpdate>({state.step, kontaAcop, vExpli});

    double gasMassSource = 0.;
    double liquidMassSource = 0.;
    double complementaryMassSource = 0.;
    if (state.step.fullModel == 1) {
        complementaryMassSource = state.step.cells[state.step.lastCell].fontemassCR;
        liquidMassSource = state.step.cells[state.step.lastCell].fontemassLR;
        gasMassSource = state.step.cells[state.step.lastCell].fontemassGR;
    }

    applyOutletPressureCondition(state.step, titRev, alfRev, betRev);
    thermal::updateFlowPartitionTerms(dependencies.thermal, dependencies.thermalDependencies, 0, vExpli);

    if (state.step.cells[state.step.lastCell].alf < 0.05 && state.step.surfaceChokeMassFlag == 1)
        state.step.cells[state.step.lastCell].alf = 0.05;

    for (int j = 0; j <= state.step.input.nvalv; j++) {
        int celposAux;
        if (j > 0)
            celposAux = state.step.input.valv[j - 1].posicP;
        else
            celposAux = celpos;
        if (state.step.cells[celposAux].alf < 0.05 && state.step.valveAreaRatiosAfterUpdate[j] <= state.step.input.master1.razareaativ)
            state.step.cells[celposAux].alf = 0.05;
    }

    solvePressureVolumeCoupling(state.step, vExpli);

    if (kontaAcop < 1 * state.step.fullModel) {
        for (int i = 0; i <= state.step.lastCell; i++) {
            state.step.cells[i].dpdt = 1 * (state.step.productionSolution[2 * i + 1] - state.step.cells[i].pres) / state.step.cells[i].dt;
            state.step.cells[i].dpdtIni = state.step.cells[i].dpdt;
        }
    }
    if (kontaAcop == 1 * state.step.fullModel || state.step.input.cicloAcopTerm == 1) {
        updateCells(state.step, vExpli);
    }
    if (state.step.input.cicloAcopTerm == 1 && state.step.fullModel == 1) {
        if (kontaAcop < 1 * state.step.fullModel)
            for (int i = 0; i <= state.step.lastCell; i++)
                state.step.cells[i].dpdt = state.step.cells[i].d2pdt2;
        thermal::advanceTransientEnergy(dependencies.thermal, state.step, kontaAcop, ciclomax);
    }
    restoreIntermediateCouplingState(state, kontaAcop, gasMassSource, liquidMassSource, complementaryMassSource);

}

void writeProfiles(const TransientSolveState &state, int nrede) {
    if (state.step.input.nperfisp > 0) {
        if (profileIsDue(state.step, state.step.input.profp.tempo, state.productionProfileTimeCounter)) {
            state.step.input.imprimeProfile(state.step.cells, state.step.productionFreeTerms, (*state.step.globals).simulationTime, state.step.branchIndex, nrede);
            state.step.input.profp.tempo[state.productionProfileTimeCounter] = (*state.step.globals).simulationTime;
            advanceProfileIndex(state.productionProfileTimeCounter, state.step.input.profp.n);
        }
    }
    if (state.step.input.nperfisg > 0 && state.step.input.lingas > 0) {
        if (profileIsDue(state.step, state.step.input.profg.tempo, state.gasProfileTimeCounter)) {
            state.step.input.imprimeProfileG(state.step.gasCells, state.step.gasFreeTerms, (*state.step.globals).simulationTime, state.step.branchIndex, nrede);
            state.step.input.profg.tempo[state.gasProfileTimeCounter] = (*state.step.globals).simulationTime;
            advanceProfileIndex(state.gasProfileTimeCounter, state.step.input.profg.n);
        }
    }
    if (state.step.input.nperfistransp > 0) {
        if (profileIsDue(state.step, state.step.input.proftransp.tempo, state.productionCrossSectionProfileTimeCounter)) {
            state.step.input.imprimeProfileTrans(state.step.cells, state.step.productionCrossSectionCount.data(), (*state.step.globals).simulationTime, state.step.branchIndex, nrede);
            state.step.input.proftransp.tempo[state.productionCrossSectionProfileTimeCounter] = (*state.step.globals).simulationTime;
            advanceProfileIndex(state.productionCrossSectionProfileTimeCounter, state.step.input.proftransp.n);
        }
    }
    if (state.step.input.nperfistransg > 0 && state.step.input.lingas > 0) {
        if (profileIsDue(state.step, state.step.input.proftransg.tempo, state.gasCrossSectionProfileTimeCounter)) {
            state.step.input.imprimeProfileTransG(state.step.gasCells, state.gasCrossSectionCellCounts.data(), (*state.step.globals).simulationTime, state.step.branchIndex, nrede);
            state.step.input.proftransg.tempo[state.gasCrossSectionProfileTimeCounter] = (*state.step.globals).simulationTime;
            advanceProfileIndex(state.gasCrossSectionProfileTimeCounter, state.step.input.proftransg.n);
        }
    }
    if (state.step.input.nCelUnit>0) {
        for(int iCelU=0;iCelU<state.step.input.nCelUnit;iCelU++){
            if (profileIsDue(state.step, state.step.input.celUnit[iCelU].tempo, state.unitCellTimeCounters[iCelU])) {
                state.step.input.relatorioCelulaUnitaria(state.step.cells,state.step.input.celUnit[iCelU].posicP, state.step.branchIndex,nrede);
                state.step.input.celUnit[iCelU].tempo[state.unitCellTimeCounters[iCelU]] = (*state.step.globals).simulationTime;
                advanceProfileIndex(state.unitCellTimeCounters[iCelU], state.step.input.celUnit[iCelU].parserie);
            }
        }
    }
}

void writeTrends(const TransientSolveState &state, const TransientDependencies &dependencies, int ordemImpT, double velmaxdesc, int nrede) {
    if (state.step.input.ntendp > 0) {
        for (int i = 0; i < state.step.input.ntendp; i++) {
            if (state.productionTrendResetTimers[i] == 0) {
                if ((*state.step.globals).simulationTime < 1e-15)
                    trendoutput::writeProductionTrendHeader(dependencies.trends, i, nrede);
                if ((*state.step.globals).simulationTime > 1e-15)
                    state.step.input.imprimeTrend(state.step.cells, state.productionTrendMatrix[i], (*state.step.globals).simulationTime, i, state.productionTrendCounts[i]);
                state.productionTrendCounts[i]++;
            }
            if (ordemImpT == 1) {
                trendoutput::writeProductionTrendRows(dependencies.trends, i, nrede);
                state.productionTrendBufferedCounts[i] = state.productionTrendCounts[i];
            }
            state.productionTrendResetTimers[i] += state.step.timeStep;
            if (state.productionTrendResetTimers[i] > state.step.input.trendp[i].dt)
                state.productionTrendResetTimers[i] = 0;
        }
    }
    if (state.step.input.ntendg > 0 && state.step.input.lingas > 0) {
        for (int i = 0; i < state.step.input.ntendg; i++) {
            if (state.gasTrendResetTimers[i] == 0 || (*state.step.globals).simulationTime < 1e-15) {
                if ((*state.step.globals).simulationTime < 1e-15)
                    trendoutput::writeGasLineTrendHeader(dependencies.trends, i, nrede);
                state.step.input.imprimeTrendG(state.step.gasCells, state.gasTrendMatrix[i], (*state.step.globals).simulationTime, i, state.gasTrendCounts[i], velmaxdesc);
                state.gasTrendCounts[i]++;
            }
            if (ordemImpT == 1) {
                trendoutput::writeGasLineTrendRows(dependencies.trends, i, nrede);
                state.gasTrendBufferedCounts[i] = state.gasTrendCounts[i];
            }
            state.gasTrendResetTimers[i] += state.step.timeStep;
            if (state.gasTrendResetTimers[i] > state.step.input.trendg[i].dt)
                state.gasTrendResetTimers[i] = 0;
        }
    }
    if (state.step.input.ntendtransp > 0) {
        for (int i = 0; i < state.step.input.ntendtransp; i++) {
            if (state.productionCrossSectionTrendResetTimers[i] == 0 || (*state.step.globals).simulationTime < 1e-15) {
                if ((*state.step.globals).simulationTime < 1e-15)
                    trendoutput::writeProductionCrossSectionTrendHeader(dependencies.trends, i);
                state.productionCrossSectionTrendMatrix[i][state.productionCrossSectionTrendCounts[i]][0] = (*state.step.globals).simulationTime;
                int poscel = state.step.input.trendtransp[i].posic;
                int poscam = state.step.input.trendtransp[i].camada - 1;
                int posdiscre = state.step.input.trendtransp[i].discre - 1;
                state.productionCrossSectionTrendMatrix[i][state.productionCrossSectionTrendCounts[i]][1] = state.step.cells[poscel].calor.Tcamada[poscam][posdiscre];
                state.productionCrossSectionTrendCounts[i]++;
            }
            if (ordemImpT == 1) {
                trendoutput::writeProductionCrossSectionTrendRows(dependencies.trends, i);
                state.productionCrossSectionTrendBufferedCounts[i] = state.productionCrossSectionTrendCounts[i];
            }
            state.productionCrossSectionTrendResetTimers[i] += state.step.timeStep;
            if (state.productionCrossSectionTrendResetTimers[i] > state.step.input.trendtransp[i].dt)
                state.productionCrossSectionTrendResetTimers[i] = 0;
        }
    }
    if (state.step.input.ntendtransg > 0 && state.step.input.lingas > 0) {
        for (int i = 0; i < state.step.input.ntendtransg; i++) {
            if (state.gasCrossSectionTrendResetTimers[i] == 0 || (*state.step.globals).simulationTime < 1e-15) {
                if ((*state.step.globals).simulationTime < 1e-15)
                    trendoutput::writeGasLineCrossSectionTrendHeader(dependencies.trends, i);
                state.gasCrossSectionTrendMatrix[i][state.gasCrossSectionTrendCounts[i]][0] = (*state.step.globals).simulationTime;
                int poscel = state.step.input.trendtransg[i].posic;
                int poscam = state.step.input.trendtransg[i].camada - 1;
                int posdiscre = state.step.input.trendtransg[i].discre - 1;
                state.gasCrossSectionTrendMatrix[i][state.gasCrossSectionTrendCounts[i]][1] = state.step.gasCells[poscel].calor.Tcamada[poscam][posdiscre];
                state.gasCrossSectionTrendCounts[i]++;
            }
            if (ordemImpT == 1) {
                trendoutput::writeGasLineCrossSectionTrendRows(dependencies.trends, i);
                state.gasCrossSectionTrendBufferedCounts[i] = state.gasCrossSectionTrendCounts[i];
            }
            state.gasCrossSectionTrendResetTimers[i] += state.step.timeStep;
            if (state.gasCrossSectionTrendResetTimers[i] > state.step.input.trendtransg[i].dt)
                state.gasCrossSectionTrendResetTimers[i] = 0;
        }
    }
}

void writeScreenOutput(const TransientSolveState &state, const chrono::steady_clock::time_point &begin, const chrono::steady_clock::time_point &end) {
    if (state.step.input.saidaTela == 1) {
        cout << state.step.stepIndex << "  " << (*state.step.globals).simulationTime << " " << state.step.timeStep;
        for (int i = 0; i < state.step.input.ntela; i++) {
            int posic = state.step.input.tela[i].posic;
            if (state.step.input.tela[i].col == 1) {
                switch (state.step.input.tela[i].var) {
                case 1:
                    cout << " " << state.step.fullModel;
                    break;
                case 2:
                    cout << " " << state.step.cells[posic].temp;
                    break;
                case 3:
                    cout << " " << state.step.cells[posic].alf;
                    break;
                case 4:
                    cout << " " << state.step.cells[posic].bet;
                    break;
                case 5:
                    cout << " " << (state.step.cells[posic].QG / state.step.cells[posic].duto.area);
                    break;
                case 6:
                    cout << " " << (state.step.cells[posic].QL / state.step.cells[posic].duto.area);
                    break;
                }
            } else {
                switch (state.step.input.tela[i].var) {
                case 1:
                    cout << " " << state.step.gasCells[posic].pres;
                    break;
                case 2:
                    cout << " " << state.step.gasCells[posic].temp;
                    break;
                case 3:
                    cout << " " << (state.step.gasCells[posic].VGasR / state.step.gasCells[posic].duto.area);
                    break;
                }
            }
        }
        cout << " " << chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        cout << endl;
    }
}

void writeEventLog(const TransientSolveState &state, int maxEvento) {
    if (state.logCounter < maxEvento) {
        while (fabs(state.step.input.logevento[state.logCounter].instante - (*state.step.globals).simulationTime) < state.step.timeStep) {
            time_t now = time(0);
            tm *ltm = localtime(&now);
            ostringstream saidaT;
            appendLogPath(saidaT, state);
            string tmp = saidaT.str();
            ofstream escreveIni(tmp.c_str(), ios_base::app);
            escreveIni << "************************************************************************************************"
                       << endl;
            escreveIni << "Evento Externo = ";
            escreveIni << state.step.input.logevento[state.logCounter].instante << " ; ";
            escreveIni << state.step.input.logevento[state.logCounter].duracao << " ; ";
            escreveIni << state.step.input.logevento[state.logCounter].estIni << " ; ";
            escreveIni << state.step.input.logevento[state.logCounter].estFim << " ; ";
            escreveIni << state.step.input.logevento[state.logCounter].descricao << " ; ";
            writeLogTimestamp(escreveIni, ltm);
            state.logCounter++;

            escreveIni.close();
        }
    }
}

void writeProgressReport(const TransientSolveState &state, int MaxKontaImpres) {
    if (progressReportIsDue(state, MaxKontaImpres)) {
        if (state.step.input.saidaTela == 0)
            cout << (*state.step.globals).simulationTime * (100.) / state.step.input.tfinal << " % da simulacao alcancado" << endl;
        state.printCounter = 0;
        ostringstream saidaT;
        appendLogPath(saidaT, state);
        string tmp = saidaT.str();
        ofstream escreveIni(tmp.c_str(), ios_base::app);
        escreveIni << "************************************************************************************************"
                   << endl;
        escreveIni << "Percentual alcancado = " << (*state.step.globals).simulationTime * (100.) / state.step.input.tfinal << " % da simulacao alcancado" << endl;
        escreveIni << "| Passo de Tempo = " << state.step.stepIndex << "| Tempo (s) = " << (*state.step.globals).simulationTime << "| Incremento de Tempo (s) = " << state.step.timeStep
                   << " |" << " Incremento de Tempo Medio CFL (s) = "
                   << state.step.meanCflTimeStep << "| Incremento de Tempo Medio Simulado (s) = " << state.step.meanSimulationTimeStep
                   << " |" << endl;
        if (state.step.input.imprimeInventario == 1)
            writeLiquidInventory(state.step, escreveIni);
        for (int i = 0; i < state.step.input.ntela; i++) {
            int posic = state.step.input.tela[i].posic;
            if (state.step.input.tela[i].col == 1) {
                switch (state.step.input.tela[i].var) {
                case 1:
                    escreveIni << " Pressao na Linha de Producao (kgf/cm2), Celula " << posic << " = " << state.step.cells[posic].pres
                               << endl;
                    break;
                case 2:
                    escreveIni << " Temperatura na Linha de Producao (C), Celula " << posic << " = " << state.step.cells[posic].temp
                               << endl;
                    break;
                case 3:
                    escreveIni << " Fracao de Vazio na Linha de Producao (-), Celula " << posic << " = " << state.step.cells[posic].alf
                               << endl;
                    break;
                case 4:
                    escreveIni << " Fracao Beta na Linha de Producao (-), Celula " << posic << " = " << state.step.cells[posic].bet
                               << endl;
                    break;
                case 5:
                    escreveIni << " Velocidade Superficial de Gas na Linha de Producao (m/s), Celula " << posic << " = "
                               << (state.step.cells[posic].QG / state.step.cells[posic].duto.area) << endl;
                    break;
                case 6:
                    escreveIni << " Velocidade Superficial de Liquido na Linha de Producao (m/s), Celula " << posic << " = "
                               << (state.step.cells[posic].QL / state.step.cells[posic].duto.area) << endl;
                    break;
                }
            } else {
                switch (state.step.input.tela[i].var) {
                case 1:
                    escreveIni << " Pressao na Linha de Servico (kgf/cm2), Celula " << posic << " = " << state.step.gasCells[posic].pres
                               << endl;
                    break;
                case 2:
                    escreveIni << " Temperatura na Linha de Servico (C), Ceula " << posic << " = " << state.step.gasCells[posic].temp
                               << endl;
                    break;
                case 3:
                    escreveIni << " Velocidade de Gas na Linha de Servico (m/s), Celula " << posic << " = "
                               << (state.step.gasCells[posic].VGasR / state.step.gasCells[posic].duto.area) << endl;
                    break;
                }
            }
        }
        writeRunCompletionReport(state, escreveIni);

        time_t now = time(0);
        tm *ltm = localtime(&now);
        writeLogTimestamp(escreveIni, ltm);

        escreveIni.close();
    }
}

void fillMiniTableCornersAtMinPressure(const TransientStepState &state, ProFlu &fluC, ProFlu &flui) {
    fillMiniTableCorner(state, fluC, flui, 0, 0);
    flui.miniTabDin.PBF[0] =
        fluC.PB(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
    fillMiniTableCorner(state, fluC, flui, 0, 1);
    flui.miniTabDin.PBF[1] =
        fluC.PB(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
}

void fillMiniTableCornersAtMaxPressure(const TransientStepState &state, ProFlu &fluC, ProFlu &flui) {
    fillMiniTableCorner(state, fluC, flui, 1, 0);
    fillMiniTableCorner(state, fluC, flui, 1, 1);
}

void assignCellTimeSteps(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].dt = state.timeStep;
        state.cells[i].dt1 = state.timeStep;
        state.cells[i].dt2 = state.timeStep;
        state.cells[i].dtPig = state.timeStep;
    }
}

bool profileIsDue(const TransientStepState &state, double *const &times, const int &index) {
    return (*state.globals).simulationTime > (*state.globals).smallValueTolerance &&
           (*state.globals).simulationTime <= times[index] &&
           (*state.globals).simulationTime + state.timeStep >= times[index];
}

void advanceProfileIndex(int &index, const int &limit) {
    index++;
    if (index >= limit)
        index--;
}

void appendLogPath(std::ostringstream &saidaT, const TransientSolveState &state) {
    if (state.step.branchIndex < 0) {
        saidaT << state.logBuffer;
    } else {
        saidaT << "Tramo" << state.step.branchIndex << "-" << state.logBuffer;
    }
}

void writeLogTimestamp(std::ostream &escreveIni, const std::tm *ltm) {
    escreveIni << "datahora = ";
    escreveIni << ltm->tm_mday << "/";
    escreveIni << 1 + ltm->tm_mon << "/";
    escreveIni << 1900 + ltm->tm_year << " ";
    escreveIni << 0 + ltm->tm_hour << ":";
    escreveIni << 0 + ltm->tm_min << ":";
    escreveIni << 0 + ltm->tm_sec;
    escreveIni << endl;
}

void writeRunCompletionReport(const TransientSolveState &state, std::ostream &escreveIni) {
    if (fabs((*state.step.globals).simulationTime - state.step.input.tfinal) <= state.step.timeStep) {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        int diaFim = (ltm->tm_mday);
        int horaFim;
        if (diaFim == diaIni)
            horaFim = ltm->tm_hour;
        else
            horaFim = ltm->tm_hour + 24;
        horaFim *= 3600;
        int minutoFim = 60 * ltm->tm_min;
        int segundoFim = ltm->tm_sec;
        int totalFim = horaFim + minutoFim + segundoFim;
        int totalIni = horaIni * 3600 + minutoIni * 60 + segundoIni;
        writeLiquidInventory(state.step, escreveIni);
        escreveIni << "     DURACAO    " << totalFim - totalIni << " segundos " << endl;
        escreveIni << "     Versao    " << versao << endl;
        if (state.step.input.saidaClassica == 1) {
            srand(time(NULL));
            int frase = rand() % 16;
            escreveIni << "*******************************************************************************" << endl;
            escreveIni << "                                  UFA!!!!!!!!                                  " << endl;
            escreveIni << state.closingTitles[frase] << endl;
            escreveIni << state.closingSubtitles[frase] << endl;
            escreveIni << "*******************************************************************************" << endl;
        } else
            escreveIni << "                                 FIM                                  " << endl;
    }
}

bool progressReportIsDue(const TransientSolveState &state, int MaxKontaImpres) {
    return (fabs((*state.step.globals).simulationTime * (100. / 5.) / state.step.input.tfinal - round((*state.step.globals).simulationTime * (100. / 5.) / state.step.input.tfinal)) < 0.5 * state.step.timeStep * (100 / 5.) / state.step.input.tfinal) || state.printCounter > MaxKontaImpres || ((*state.step.globals).simulationTime + state.step.timeStep >= state.step.input.tfinal);
}

double updateInteriorAuxiliaryPressure(const TransientStepState &state, int i) {
    Cel &cell = state.cells[i];
    Cel &upstreamCell = state.cells[i - 1];
    double dx = 0.5 * cell.dx;
    double diameter = cell.duto.a;
    double area = 0.25 * std::numbers::pi * diameter * diameter;
    double perimeter = cell.duto.peri;
    double alfmed = cell.alf;
    double rhog = cell.flui.MasEspGas(cell.pres, cell.temp);
    double rhol = (1 - cell.bet) * cell.flui.MasEspLiq(cell.pres, cell.temp) + cell.bet * cell.fluicol.MasEspFlu(cell.pres, cell.temp);
    double ugsmed = (cell.MC - cell.Mliqini) / (area * rhog);
    double ulsmed = cell.Mliqini / (area * rhol);
    double j = ugsmed + ulsmed;

    double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    double viscmix = alfmed * cell.flui.ViscGas(cell.pres, cell.temp) + (1 - alfmed) * ((1 - cell.bet) * cell.flui.ViscOleo(cell.pres, cell.temp) + cell.bet * cell.fluicol.VisFlu(cell.pres, cell.temp));

    double reynolds;
    if (cell.duto.revest == 0)
        reynolds = cell.Rey(cell.duto.a, j, rhomix, viscmix);
    else {
        double dhid = 4 * area / perimeter;
        reynolds = cell.Rey(dhid, j, rhomix, viscmix);
    }
    double frictionFactor = cell.fric(reynolds, cell.duto.rug / diameter);
    double medpres;
    if (state.input.MedSimpPresFront == 0) {
        if (cell.presaux <= 10)
            medpres = 1;
        else
            medpres = 0.;
    } else
        medpres = 1;
    double gradfric = (1 - medpres) * 0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area;
    double gradhidro = (1 - medpres) * kGravity * sin(cell.duto.teta) * rhomix * dx;
    cell.presauxini = cell.presaux;
    cell.presaux = cell.pres + (gradfric + gradhidro - upstreamCell.acsr.dpB) / units::kPascalPerKgfPerCm2;
    cell.dpresaux = 0.5 * (gradfric + gradhidro - upstreamCell.acsr.dpB) / units::kPascalPerKgfPerCm2;
    dx = 0.5 * cell.dxL;
    diameter = upstreamCell.duto.a;
    area = 0.25 * std::numbers::pi * diameter * diameter;
    perimeter = upstreamCell.duto.peri;
    alfmed = upstreamCell.alf;
    rhog = upstreamCell.flui.MasEspGas(upstreamCell.pres, upstreamCell.temp);
    rhol = (1 - upstreamCell.bet) * upstreamCell.flui.MasEspLiq(upstreamCell.pres, upstreamCell.temp) + upstreamCell.bet * upstreamCell.fluicol.MasEspFlu(upstreamCell.pres, upstreamCell.temp);
    ugsmed = (upstreamCell.MC - upstreamCell.Mliqini) / (area * rhog);
    ulsmed = upstreamCell.Mliqini / (area * rhol);
    j = ugsmed + ulsmed;

    rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    viscmix = alfmed * upstreamCell.flui.ViscGas(upstreamCell.pres, upstreamCell.temp) + (1 - alfmed) * ((1 - upstreamCell.bet) * upstreamCell.flui.ViscOleo(upstreamCell.pres, upstreamCell.temp) + upstreamCell.bet * upstreamCell.fluicol.VisFlu(upstreamCell.pres, upstreamCell.temp));

    if (upstreamCell.duto.revest == 0)
        reynolds = upstreamCell.Rey(upstreamCell.duto.a, j, rhomix, viscmix);
    else {
        double dhid = 4 * area / perimeter;
        reynolds = upstreamCell.Rey(dhid, j, rhomix, viscmix);
    }
    frictionFactor = upstreamCell.fric(reynolds, upstreamCell.duto.rug / diameter);

    gradfric = (1 - medpres) * 0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area;
    gradhidro = (1 - medpres) * kGravity * sin(upstreamCell.duto.teta) * rhomix * dx;

    if (upstreamCell.acsr.tipo != kAccessoryChoke || upstreamCell.acsr.chk.AreaGarg > upstreamCell.acsr.chk.AreaTub * 0.5)
        cell.presaux = 0.5 * (cell.presaux) +
                            0.5 * (upstreamCell.pres - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2);
    cell.dpresaux -= 0.5 * (gradfric + gradhidro) / units::kPascalPerKgfPerCm2;
    upstreamCell.presauxRini = upstreamCell.presauxR;
    upstreamCell.presauxR = cell.presaux;
    if (i < state.lastCell) {
        state.cells[i + 1].presauxLini = state.cells[i + 1].presauxL;
        state.cells[i + 1].presauxL = cell.presaux;
    }
    return area;
}

void updateInteriorVolumeFlows(const TransientStepState &state, int i, double area) {
    Cel &cell = state.cells[i];
    Cel &upstreamCell = state.cells[i - 1];
    double tmed = upstreamCell.temp;
    if (cell.VTemper < 0.)
        tmed = cell.temp;

    const ProFlu &flud = cell.Mliqini < 0. ? cell.flui : upstreamCell.flui;

    double betI;
    if (((cell.MC - cell.Mliqini) * 0 + 1 * cell.Mliqini) < 0)
        betI = cell.bet;
    else
        betI = cell.betL;

    const FacePhaseDensities densities = facePhaseDensitiesOf(state, i, flud, tmed, betI);
    double rhol = densities.liquid;
    double gasDensity = densities.gas;
    double vLiqTest = 1 + 0 * fabs(cell.Mliqini / (rhol * area));
    double vGasTest = 1 + 0 * fabs((cell.MC - cell.Mliqini) / (gasDensity * area));

    cell.QLini = cell.QL;
    if (vLiqTest > 1e-3)
        cell.QL = cell.Mliqini / rhol;
    else {
        cell.QL = 0.;
        cell.MC = (cell.MC - cell.Mliqini);
        cell.Mliqini = 0;
        state.cells[i + 1].ML = upstreamCell.MR = cell.MC;
        state.cells[i + 1].MliqiniL = upstreamCell.MliqiniR = cell.Mliqini;
    }
    cell.QGini = cell.QG;
    if (vGasTest > 1e-3)
        cell.QG = (cell.MC - cell.Mliqini) / gasDensity;
    else {
        cell.QG = 0;
        cell.MC = cell.Mliqini;
        state.cells[i + 1].ML = upstreamCell.MR = cell.MC;
    }
    upstreamCell.QLRini = upstreamCell.QLR;
    upstreamCell.QLR = cell.QL;
    if (i < state.lastCell) {
        state.cells[i + 1].QLLini = state.cells[i + 1].QLL;
        state.cells[i + 1].QLL = cell.QL;
    }
}

void updateOutletAuxiliaryPressure(const TransientStepState &state, int i) {
    Cel &cell = state.cells[i];
    Cel &upstreamCell = state.cells[i - 1];
    if (state.input.MedSimpPresFront != 0) {
        cell.presauxini = cell.presaux;
        cell.presaux = cell.pres - upstreamCell.acsr.dpB / units::kPascalPerKgfPerCm2;
        cell.presaux = 0.5 * cell.presaux + 0.5 * upstreamCell.pres;
        cell.dpresaux = -0.5 * upstreamCell.acsr.dpB / units::kPascalPerKgfPerCm2;
        upstreamCell.presauxRini = upstreamCell.presauxR;
        upstreamCell.presauxR = cell.presaux;
        if (i < state.lastCell) {
            state.cells[i + 1].presauxLini = state.cells[i + 1].presauxL;
            state.cells[i + 1].presauxL = cell.presaux;
        }
        return;
    }
    double dx = 0.5 * cell.dx;
    double diameter = cell.duto.a;
    double area = 0.25 * std::numbers::pi * diameter * diameter;
    double perimeter = cell.duto.peri;
    double alfmed = cell.alf;
    double rhog = cell.flui.MasEspGas(cell.pres, cell.temp);
    double rhol = (1 - cell.bet) * cell.flui.MasEspLiq(cell.pres, cell.temp) + cell.bet * cell.fluicol.MasEspFlu(cell.pres, cell.temp);
    double ugsmed = (cell.QG) / (area);
    double ulsmed = cell.QL / (area);
    double j = ugsmed + ulsmed;

    double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    double viscmix = alfmed * cell.flui.ViscGas(cell.pres, cell.temp) + (1 - alfmed) * ((1 - cell.bet) * cell.flui.ViscOleo(cell.pres, cell.temp) + cell.bet * cell.fluicol.VisFlu(cell.pres, cell.temp));

    double reynolds;
    if (cell.duto.revest == 0)
        reynolds = cell.Rey(cell.duto.a, j, rhomix, viscmix);
    else {
        double dhid = 4 * area / perimeter;
        reynolds = cell.Rey(dhid, j, rhomix, viscmix);
    }
    double frictionFactor = j == 0. ? 0. : cell.fric(reynolds, cell.duto.rug / diameter);
    double medpres = 0.;
    double gradfric = (1 - medpres) * 0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area;
    double gradhidro = (1 - medpres) * kGravity * sin(cell.duto.teta) * rhomix * dx;

    cell.presauxini = cell.presaux;
    cell.presaux = cell.pres + (gradfric + gradhidro - upstreamCell.acsr.dpB) / units::kPascalPerKgfPerCm2;
    cell.dpresaux = 0.5 * (gradfric + gradhidro - upstreamCell.acsr.dpB) / units::kPascalPerKgfPerCm2;
    dx = 0.5 * cell.dxL;
    diameter = upstreamCell.duto.a;
    area = 0.25 * std::numbers::pi * diameter * diameter;
    perimeter = upstreamCell.duto.peri;
    alfmed = upstreamCell.alf;
    rhog = upstreamCell.flui.MasEspGas(upstreamCell.pres, upstreamCell.temp);
    rhol = (1 - upstreamCell.bet) * upstreamCell.flui.MasEspLiq(upstreamCell.pres, upstreamCell.temp) + upstreamCell.bet * upstreamCell.fluicol.MasEspFlu(upstreamCell.pres, upstreamCell.temp);
    ugsmed = (upstreamCell.MC - upstreamCell.Mliqini) / (area * rhog);
    ulsmed = upstreamCell.Mliqini / (area * rhol);
    j = ugsmed + ulsmed;

    rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    viscmix = alfmed * upstreamCell.flui.ViscGas(upstreamCell.pres, upstreamCell.temp) + (1 - alfmed) * ((1 - upstreamCell.bet) * upstreamCell.flui.ViscOleo(upstreamCell.pres, upstreamCell.temp) + upstreamCell.bet * upstreamCell.fluicol.VisFlu(upstreamCell.pres, upstreamCell.temp));

    if (upstreamCell.duto.revest == 0)
        reynolds = upstreamCell.Rey(upstreamCell.duto.a, j, rhomix, viscmix);
    else {
        double dhid = 4 * area / perimeter;
        reynolds = upstreamCell.Rey(dhid, j, rhomix, viscmix);
    }
    frictionFactor = j == 0. ? 0. : upstreamCell.fric(reynolds, upstreamCell.duto.rug / diameter);
    gradfric = (1 - medpres) * 0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area;
    gradhidro = (1 - medpres) * kGravity * sin(upstreamCell.duto.teta) * rhomix * dx;

    cell.presaux = 0.5 * (cell.presaux) +
                        0.5 * (upstreamCell.pres - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2);
    cell.dpresaux -= 0.5 * (gradfric + gradhidro) / units::kPascalPerKgfPerCm2;
    upstreamCell.presauxRini = upstreamCell.presauxR;
    upstreamCell.presauxR = cell.presaux;
    if (i < state.lastCell) {
        state.cells[i + 1].presauxLini = state.cells[i + 1].presauxL;
        state.cells[i + 1].presauxL = cell.presaux;
    }
}

void updateOutletVolumeFlows(const TransientStepState &state, int i) {
    Cel &cell = state.cells[i];
    Cel &upstreamCell = state.cells[i - 1];
    double tmed = upstreamCell.temp;
    if (cell.VTemper < 0.)
        tmed = cell.temp;

    const ProFlu &flud = cell.Mliqini < 0. ? cell.flui : upstreamCell.flui;

    double betI;
    if (((cell.MC - cell.Mliqini) * 0.99 + 0.01 * cell.Mliqini) < 0)
        betI = cell.bet;
    else
        betI = cell.betL;

    const FacePhaseDensities densities = facePhaseDensitiesOf(state, i, flud, tmed, betI);
    double rhol = densities.liquid;
    double gasDensity = densities.gas;

    cell.QL = cell.Mliqini / rhol;
    cell.QG = (cell.MC - cell.Mliqini) / gasDensity;
    upstreamCell.QLR = cell.QL;
}

FacePhaseDensities facePhaseDensitiesOf(const TransientStepState &state, int i,
                                             const ProFlu &flud, double tmed, double betI) {
    double liquidDensity = flud.MasEspLiq(state.cells[i].presaux, tmed);
    double rhol = (1 - betI) * liquidDensity + betI * state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
    double gasDensity = flud.MasEspGas(state.cells[i].presaux, tmed);
    return {rhol, gasDensity};
}

void applyChokeTimeStepControls(const TransientStepState &state) {
    constexpr int multChoke = 1;
    double mgas = state.cells[state.lastCell].MC - state.cells[state.lastCell].Mliqini;
    double mgas0 = state.cells[state.lastCell].MCini - state.cells[state.lastCell].Mliqini0;
    if (state.input.RelaxaDTChoke == 0 &&
        (((*state.globals).simulationTime > 1e-15 && state.surfaceChokeMassFlag == 1 && state.surfaceChoke.AreaGarg < 0.6 * state.cells[state.lastCell - 1].duto.area &&
          state.surfaceChoke.AreaGarg > 1e-3 * state.cells[state.lastCell - 1].duto.area &&
          ((state.cells[state.lastCell].Mliqini >= 0 && state.cells[state.lastCell].Mliqini0 < 0 && mgas > 0) || (state.cells[state.lastCell].Mliqini >= 0 && mgas < 0 && mgas0 > 0))) ||
         state.slugCount < multChoke * 200)) {

        if (state.slugCount > multChoke * 200) {
            state.slugCount = 0;
        }
        if (((state.cells[state.lastCell].Mliqini >= 0 && state.cells[state.lastCell].Mliqini0 < 0 && mgas > 0) ||
             (state.cells[state.lastCell].Mliqini >= 0 && mgas < 0 && mgas0 > 0)) &&
            state.slugCount > multChoke * 100)
            state.slugCount = multChoke * 100;
        double progres = 1.;
        if (state.slugCount > multChoke * 100) {
            progres = state.slugCount - multChoke * 100;
        }
        state.timeStep *= (progres / (multChoke * 100.));
        state.slugCount++;
    }
    if (state.surfaceChoke.AreaGarg >= 0.6 * state.cells[state.lastCell - 1].duto.area)
        state.surfaceChokeOpen = 1;
    if (fabs(state.outletPressure - state.gasSurfacePressure) / state.outletPressure < 0.05 && state.surfaceChokeOpen == 0 && state.timeStep > 1. && state.surfaceChoke.AreaGarg < 0.6 * state.cells[state.lastCell - 1].duto.area &&
        state.surfaceChoke.AreaGarg > 1e-15 * state.cells[state.lastCell - 1].duto.area)
        state.timeStep =
            1.;
    for (int i = 0; i < state.input.eventoabre; i++) {
        if ((*state.globals).simulationTime > state.input.Tevento[i] - state.input.dtmax && (*state.globals).simulationTime < state.input.Tevento[i] + 30) {
            if (state.timeStep > state.smallestCellLength / 100.) {
                state.timeStep = state.smallestCellLength / 100.;
            }
        }
    }
    state.multiplier = 0.8;
    if ((((*state.globals).simulationTime - 2 * state.input.dtmax) > state.movingMeanStartTime && (fabs(state.movingMeanPressure - state.outletPressure) / state.outletPressure > 0.4)) || (state.outletPressure < state.gasSurfacePressure && state.surfaceChokeMassFlag == 1 && state.surfaceChoke.AreaGarg > (1e-3) * state.cells[state.lastCell - 1].duto.area) || (state.cells[state.lastCell].alf <= 0.1 && state.surfaceChoke.AreaGarg < 0.6 * state.cells[state.lastCell - 1].duto.area && state.surfaceChoke.AreaGarg > 1e-3 * state.cells[state.lastCell - 1].duto.area && state.surfaceChokeOpen == 0)) {
        double denominador = 10.;
        double progres = 0.95;

        if ((state.outletPressure < state.gasSurfacePressure && state.surfaceChokeMassFlag == 1)) {
            denominador = 100.;
            progres = 0.5;
            state.multiplier = pow(progres, 10. * fabs(state.gasSurfacePressure - state.outletPressure) / state.outletPressure) * state.multiplier;
        } else if ((state.cells[state.lastCell].alf <= 0.1 && state.surfaceChoke.AreaGarg < 0.6 * state.cells[state.lastCell - 1].duto.area)) {
            denominador = 1000.;
            state.multiplier = (state.cells[state.lastCell].alf + 1e-5) * state.multiplier;
        } else {
            state.multiplier = pow(progres, 10 * fabs(state.movingMeanPressure - state.outletPressure) / state.outletPressure) * state.multiplier;
        }
        if (state.input.dtmax * state.multiplier < state.smallestCellLength / denominador)
            state.multiplier = (state.smallestCellLength / denominador) / state.input.dtmax;
        if (state.multiplier > 0.8)
            state.multiplier = 0.8;
    } else {
        if (state.cells[state.lastCell].alf >= 0.11 || state.surfaceChoke.AreaGarg >= 0.6 * state.cells[state.lastCell - 1].duto.area ||
            state.surfaceChoke.AreaGarg <= 1e-3 * state.cells[state.lastCell - 1].duto.area || state.surfaceChokeOpen == 1) {
            state.multiplier = state.multiplier / 0.95;
            if (state.multiplier > 0.8)
                state.multiplier = 0.8;
        }
    }
}

void restrictTimeStepByTransport(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        double jmix = 0.;
        double pipeArea = state.cells[i].duto.area;
        double dtaux;
        double alfteste = state.cells[i].alf;
        if (i > 0 && (state.cells[i].MC - state.cells[i].Mliqini) > 0)
            alfteste = state.cells[i - 1].alf;
        if (alfteste > 1e-9)
            jmix += fabs(
                (state.cells[i].MC - state.cells[i].Mliqini) / (pipeArea * state.cells[i].flui.MasEspGas(state.cells[i].pres, state.cells[i].temp) * (0 + 1 * alfteste)));
        if (fabs(state.cells[i].VTemper) > jmix)
            jmix = fabs(state.cells[i].VTemper);

        double jmixL = 0.;
        if (alfteste < 1 - 1e-5)
            jmixL = fabs(
                state.cells[i].Mliqini / (pipeArea * ((1. - state.cells[i].bet) * state.cells[i].rpC + state.cells[i].bet * state.cells[i].rcC) * (1. - 0 * alfteste)));
        if (fabs(jmixL) > jmix)
            jmix = fabs(jmixL);
        if (fabs(jmix) > 1e-5)
            dtaux = 1.0 * state.cells[i].dx / jmix;
        else
            dtaux = state.timeStep;
        if (dtaux < state.timeStep)
            state.timeStep = dtaux;
    }
    if (state.input.lingas == 1) {
        for (int i = 0; i <= state.lastGasCell; i++) {
            double dtaux;
            double jmix = fabs(state.gasCells[i].VGasR / state.gasCells[i].u1L);
            if (fabs(jmix) > 1e-5)
                dtaux = state.gasCells[i].dx0 / jmix;
            else
                dtaux = state.timeStep;
            if (dtaux < state.timeStep)
                state.timeStep = dtaux;
        }
    }
}


void applyScheduledTimeStepEvents(const TransientStepState &state) {
    if (state.input.evento.size() > state.eventIndex) {
        if ((*state.globals).simulationTime < state.input.evento[state.eventIndex] && ((*state.globals).simulationTime + state.timeStep) > (state.input.evento[state.eventIndex] + 0.1)) {
            state.timeStep = state.input.evento[state.eventIndex] - (*state.globals).simulationTime;
            state.eventIndex++;
        } else if ((*state.globals).simulationTime < state.input.evento[state.eventIndex] && ((*state.globals).simulationTime + state.timeStep) >= (state.input.evento[state.eventIndex]))
            state.eventIndex++;
    }

    state.masterState = 1;
    for (int i = 0; i < state.masterCloseCount; i++) {
        if ((*state.globals).simulationTime > state.masterCloseSchedule[i]) {
            for (int j = 0; j < state.masterOpenCount; j++) {
                if ((*state.globals).simulationTime < state.masterOpenSchedule[j]) {
                    state.masterState = 0;
                    state.masterCounter++;
                    if (state.masterCounter > 19)
                        state.masterCounter = 20;
                    if ((*state.globals).simulationTime >= state.masterOpenSchedule[j] - state.timeStep)
                        state.masterCounter = 0;
                    break;
                }
            }
        }
    }
}

void penalizeOscillatingFractions(const TransientStepState &state) {
    int parada = 0;
    for (int i = 1; i < state.lastCell; i++) {
        if (state.cells[i].acsr.tipo == kAccessoryChoke && state.cells[i].acsr.chk.AreaGarg <= 1e-15 * state.cells[i].acsr.chk.AreaTub)
            parada = 1;
        else if (state.surfaceChoke.AreaGarg <= 1.e-15 * state.surfaceChoke.AreaTub)
            parada = 1;
    }
    if (parada == 1) {
        Vcr<int> oscila(state.lastCell, 0);
        for (int i = 0; i <= state.lastCell; i++) {
            if (i > 0 && i < state.lastCell) {
                if (fabs(state.cells[i - 1].alf - state.cells[i + 1].alf) < fabs(state.cells[i].alf - state.cells[i - 1].alf)) {
                    double area = state.cells[i].duto.area;
                    double vLiqTest = fabs(state.cells[i].QL / (area));
                    double vGasTest = fabs(state.cells[i].QG / (area));
                    if ((vLiqTest + vGasTest) > 0.1)
                        oscila[i] = 1;
                }
            }
        }
        int alarmOscila = 0;
        for (int i = 1; i <= state.lastCell - 4; i++) {
            int kontaOsc = 0;
            int multOsc = 0;
            while (kontaOsc < 3) {
                multOsc += oscila[i + kontaOsc];
                kontaOsc++;
            }
            if (multOsc == 3)
                alarmOscila = 1;
        }

        if (alarmOscila == 1 && state.input.desligaPenalizaDT == 0)
            state.timeStep /= 10.;
    }
}

double valveRestrictedTimeStep(const TransientStepState &state, int i, double dtaux) {
    int celpos;
    double valveTimeStep = state.cells[0].dt;
    if (i == 0)
        celpos = state.input.master1.posic;
    else
        celpos = state.input.valv[i - 1].posicP;
    if (state.valveAreaRatiosAfterUpdate[i] < state.valveAreaRatiosBeforeUpdate[i] && (state.valveAreaRatiosAfterUpdate[i] <= 1.1 * state.input.master1.razareaativ && state.valveAreaRatiosAfterUpdate[i] >= 1e-3 * state.input.master1.razareaativ)) {
        if (state.cells[celpos].alf < 0.05) {
            if (state.cells[celpos].alf < 0.01)
                state.desperationMoment += 1.5;
            else
                state.desperationMoment = 1;
            if (state.desperationMoment < 1.)
                state.desperationMoment = 1.;
            double artificialGasRate = state.desperationMoment * 10000 * state.cells[celpos].flui.Deng * kAirDensityAtStandardConditions / units::kSecondsPerDay;
            state.cells[celpos].fontemassGL += artificialGasRate;
            state.cells[celpos - 1].fontemassGR = state.cells[celpos].fontemassGL;
            state.artificialGasThisStep.push_back({i, celpos, artificialGasRate});
        }
        if (state.cells[celpos].alf < 0.5)
            state.masterCriticalRatio[i] = 0.01;
    } else
        state.desperationMoment = 0.;
    if (state.valveAreaRatiosAfterUpdate[i] < state.valveAreaRatiosBeforeUpdate[i] && (state.valveAreaRatiosAfterUpdate[i] <= 1.1 * state.input.master1.razareaativ && state.valveAreaRatiosAfterUpdate[i] >= state.masterCriticalRatio[i] * state.input.master1.razareaativ)) {
        double timeStepDivisor = 20.;
        if (state.cells[celpos].alf < 0.5)
            timeStepDivisor = 40.;
        valveTimeStep = dtaux;
        if (valveTimeStep > 1)
            valveTimeStep = 1.;
        if (state.cells[celpos].alf < 0.5)
            valveTimeStep = 0.1 + 0.9 * (state.cells[celpos].alf) / 0.5;
        valveTimeStep /= timeStepDivisor;
        state.restart = -1;
    }
    return valveTimeStep;
}

void updateOutletVoidFraction(const TransientStepState &state, int i, double alfrev) {
    if (((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1)) {
        if (state.surfaceChokeMassFlag == 0 || state.cells[state.lastCell].Mliqini > 0)
            state.cells[i].alf = state.cells[i - 1].alf;
        else
            state.cells[i].avancalf(state.restart, state.lastCell);
    } else {
        if (state.cells[state.lastCell].Mliqini > 0)
            state.cells[i].alf = state.cells[i - 1].alf;
        else if (state.surfaceChokeMassFlag == 0 && state.input.chkv == 0)
            state.cells[i].alf = alfrev;
        else
            state.cells[i].avancalf(state.restart, state.lastCell);
    }
}

void updateOutletComplementaryFraction(const TransientStepState &state, int i) {
    if (((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1)) {
        if (state.surfaceChokeMassFlag == 0 || state.cells[state.lastCell].Mliqini > 0)
            state.cells[i].bet = state.cells[i - 1].bet;
        else
            state.cells[i].avancbet(state.restart, state.lastCell);
    } else {
        if (state.surfaceChokeMassFlag == 1)
            state.cells[i].avancbet(state.restart, state.lastCell);
        else if (state.input.chkv == 1)
            state.cells[i].bet = state.cells[i - 1].bet;
    }
}

void preparePigFaceFractions(const TransientStepState &state) {
    if (state.input.ConContEntrada == 0) {
        state.cells[0].betI = state.cells[0].bet;
        state.cells[0].betLI = state.cells[0].bet;
    } else {
        state.cells[0].betI = state.inletComplementaryFraction;
        if ((state.cells[0].MC - state.cells[0].Mliqini) * 0 + state.cells[0].Mliqini < 0.)
            state.cells[0].betI = state.cells[0].betPigE;
        state.cells[0].betLI = state.cells[0].betI;
    }
    for (int i = 1; i <= state.lastCell; i++) {
        state.cells[i].betI = state.cells[i].betPigE;
        if (state.cells[i].QL > 0.)
            state.cells[i].betI = state.cells[i - 1].betPigD;
        state.cells[i - 1].betRI = state.cells[i].betI;
        if (i < state.lastCell)
            state.cells[i + 1].betLI = state.cells[i].betI;
    }
}

double currentOutletGasMassFraction(const TransientStepState &state, double alfSup,
                                    double massgas, double masentrada, double romix) {
    double chokeGasMassFraction;
    if ((massgas >= 0 && state.cells[state.lastCell - 1].MliqiniR <= 0) || (massgas < 0 && state.cells[state.lastCell - 1].MliqiniR == 0))
        chokeGasMassFraction = 1.;
    else if (massgas <= 0 && state.cells[state.lastCell - 1].MliqiniR > 0)
        chokeGasMassFraction = 0.;
    else if (masentrada < 0) {
        chokeGasMassFraction = 1.;
    } else if (fabs(masentrada) < 1e-15)
        chokeGasMassFraction = inSituGasMassFraction(state.cells[state.lastCell], alfSup, state.cells[state.lastCell].pres, romix);
    else
        chokeGasMassFraction = flowQuality(massgas, masentrada);
    if (chokeGasMassFraction > 1)
        chokeGasMassFraction = 1;
    if (state.outletPressure < state.gasSurfacePressure) {
        chokeGasMassFraction = 1.;
    }

    if (chokeGasMassFraction == 0 && alfSup > 0.05)
        chokeGasMassFraction = inSituGasMassFraction(state.cells[state.lastCell], alfSup, state.cells[state.lastCell].pres, romix);
    return chokeGasMassFraction;
}

bool currentOutletUsesPressureDrop(const TransientStepState &state, double chokeGasMassFraction,
                                   double delp, double masentrada, double maxSup) {
    double difdelp = state.outletPressure - state.gasSurfacePressure;
    return ((chokeGasMassFraction < 1e-7 && surfaceChokeIsOpen(state)) ||
         (chokeGasMassFraction < 0.01 && surfaceChokeIsOpen(state) &&
          fabs(difdelp) / delp < 1.2 && fabs(difdelp) / delp > 0.8 &&
          ((fabs(maxSup) > 0 && fabs((masentrada - maxSup) / maxSup) < 0.2) ||
           (fabs(masentrada) > 0 && fabs((masentrada - maxSup) / masentrada) < 0.2))));
}

bool currentOutletCanRemainOpen(const TransientStepState &state, double delp) {
    return surfaceChokeIsOpen(state) &&
            ((((*state.globals).simulationTime - 2 * state.input.dtmax > state.movingMeanStartTime || state.input.perm == 2) &&
              fabs(state.movingMeanPressure - state.gasSurfacePressure) / state.movingMeanPressure < 0.05 && fabs(state.movingMeanFlux) < 0.5) ||
             (((*state.globals).simulationTime - 2 * state.input.dtmax > state.movingMeanStartTime || state.input.perm == 2) &&
              fabs(state.movingMeanPressure - state.gasSurfacePressure) < (0.05 * state.gasSurfacePressure) && fabs(state.movingMeanFlux) < 5. && delp < 0.01 * state.gasSurfacePressure) ||
             (((*state.globals).simulationTime - 2 * state.input.dtmax > state.movingMeanStartTime || state.input.perm == 2) && (state.gasSurfacePressure - state.movingMeanPressure) / state.movingMeanPressure > 0.001 && state.input.chkv == 0) || (fabs(delp) < 0.1 && (state.outletPressure - state.gasSurfacePressure) / state.outletPressure < 0.05 && state.input.chkv == 0));
}

bool bufferedOutletUsesPressureDrop(const TransientStepState &state, double chokeGasMassFraction,
                                    double delp, double masentrada, double maxSup) {
    double difdelp = fabs(fabs(state.cells[state.lastCell].presBuf - state.gasSurfacePressure) - delp);
    return ((chokeGasMassFraction < 1e-7 && surfaceChokeIsOpen(state)) ||
         (chokeGasMassFraction < 0.01 && surfaceChokeIsOpen(state) &&
          difdelp / delp < 0.2 &&
          ((fabs(maxSup) > 0 && fabs((masentrada - maxSup) / maxSup) < 0.2) ||
           (fabs(masentrada) > 0 && fabs((masentrada - maxSup) / masentrada) < 0.2))));
}

void updateBufferedNetworkSources(const TransientStepState &state, double betRev) {
    double sinal;
    if (state.input.chkv == 0 || state.cells[state.lastCell - 1].MRBuf > 0.)
        sinal = 1.;
    else
        sinal = 0.;
    double betloc;
    if ((state.cells[state.lastCell - 1].MRBuf - state.cells[state.lastCell - 1].MliqiniRBuf) * 0 + 1 * state.cells[state.lastCell - 1].MliqiniRBuf > 0.)
        betloc = state.cells[state.lastCell - 1].bet;
    else
        betloc = betRev;
    double rhomistBuf = betloc *
                            state.cells[state.lastCell - 1].fluicol.MasEspFlu(state.cells[state.lastCell].presBuf, state.cells[state.lastCell].temp) +
                        (1. - betloc) * state.cells[state.lastCell - 1].flui.MasEspLiq(state.cells[state.lastCell].presBuf, state.cells[state.lastCell].temp);
    double QLbuf = state.cells[state.lastCell - 1].MliqiniRBuf / rhomistBuf;

    state.bufferedComplementaryMassSource = -sinal * QLbuf * (betloc)*state.cells[state.lastCell - 1].rcC;
    state.bufferedLiquidMassSource = -sinal * (state.cells[state.lastCell - 1].MliqiniRBuf + state.bufferedComplementaryMassSource);
    state.bufferedGasMassSource = -sinal * (state.cells[state.lastCell - 1].MRBuf - state.cells[state.lastCell - 1].MliqiniRBuf);
}

void prepareCouplingModelFlags(const TransientSolveState &state, int celpos) {
    if (state.step.fullModel == 0) {
        for (int i = 0; i <= state.step.lastCell; i++)
            state.step.cells[i].m2d = 0.;
    } else {
        for (int i = 0; i <= state.step.lastCell; i++) {
            double razDp = 0.1;
            double razDT = 1;
            if ((fabs(state.step.cells[i].dpdtIni) / state.step.cells[i].pres < razDp) && fabs(state.step.cells[i].dTdtIni) < razDT) {
                if (state.massTransferModel == 0)
                    state.step.cells[i].m2d = 1.;
                else
                    state.step.cells[i].m2d = 0.;
                state.step.cells[i].mudaDT = 1.;
            } else {
                state.step.cells[i].m2d = 0.;
                state.step.cells[i].mudaDT = 0.;
            }
        }
    }
    if (state.step.input.estabCol == 1) {
        for (int i = 0; i <= celpos; i++) {
            state.step.cells[i].m2d = 0.;
            state.step.cells[i].mudaDT = 0.;
            state.step.cells[i].estabCol = 1;
        }
    }
}

void restartCouplingFractions(const TransientSolveState &state, const TransientDependencies &dependencies, int kontaAcop,
                              double alfRev, double betRev) {
    if (state.step.restart == -1) {
        restartFractionEvolutionInitial(state.step);
        for (int i = 0; i <= state.step.lastCell; i++) {
            if (state.step.cells[i].acsr.tipo == kAccessoryRadialPorous) {
                state.step.cells[i].acsr.radialPoro.reavaliaDT(state.step.timeStep);
            } else if (state.step.cells[i].acsr.tipo == kAccessoryPorous2D) {
                state.step.cells[i].acsr.poroso2D.reavaliaDT(state.step.timeStep);
            }
        }
        for (int i = 0; i <= state.step.lastCell; i++) {
            if (state.step.cells[i].acsr.tipo == kAccessoryRadialPorous) {
                state.step.cells[i].acsr.radialPoro.reiniciaEvoluiSW(state.step.timeStep);
            }
            if (state.step.cells[i].acsr.tipo == kAccessoryPorous2D) {
                state.step.cells[i].acsr.poroso2D.reiniciaEvoluiSW(state.step.timeStep);
            }
        }
        state.step.finalAuxiliaryTimeStep = state.step.timeStep;
        restartFractionEvolution(state.step, dependencies.gasLift, dependencies.thermal);
        evolveFractions(state.step, alfRev, betRev, kontaAcop);
        state.step.restart = 0;
        for (int i = 0; i <= state.step.lastCell; i++) {
            if (state.step.cells[i].acsr.tipo == kAccessoryRadialPorous) {
                state.step.cells[i].acsr.radialPoro.avancoSWcorrec();
            } else if (state.step.cells[i].acsr.tipo == kAccessoryPorous2D) {
                state.step.cells[i].acsr.poroso2D.avancoSWcorrec();
            }
        }
        thermal::restartAxisymmetricThermalField(dependencies.thermal);
    }
}

void restoreIntermediateCouplingState(const TransientSolveState &state, int kontaAcop,
                                      double gasMassSource, double liquidMassSource,
                                      double complementaryMassSource) {
    if (kontaAcop != 1 * state.step.fullModel) {
        for (int i = 0; i <= state.step.lastCell; i++) {
            state.step.cells[i].FeiticoDoTempo2();
            if (state.step.cells[i].acsr.tipo == kAccessoryRadialPorous) {
                state.step.cells[i].acsr.radialPoro.FeiticoDoTempoSW();
            } else if (state.step.cells[i].acsr.tipo == kAccessoryPorous2D) {
                state.step.cells[i].acsr.poroso2D.FeiticoDoTempoSW();
            }
        }

        state.step.cells[state.step.lastCell].fontemassCR = complementaryMassSource;
        state.step.cells[state.step.lastCell].fontemassLR = liquidMassSource;
        state.step.cells[state.step.lastCell].fontemassGR = gasMassSource;

        state.step.surfaceChokeOpen = state.step.initiallyOpen;
        state.step.openTime = state.initialOpenTime;
    }
}

int prepareCouplingMode(const TransientStepState &state, int explicitScheme) {
    for (int i = 0; i <= state.input.nvalv; i++)
        state.masterCriticalRatio[i] = 0.5;
    captureValveAreaRatios(state, state.valveAreaRatiosAfterUpdate);
    for (int i = 0; i <= state.input.nvalv; i++)
        if (state.valveAreaRatiosAfterUpdate[i] != state.valveAreaRatiosBeforeUpdate[i])
            state.fullModel = 0;
    TimeStepPolicies::apply<TimeStepHook::AfterValveOpenings>(
        {state, kOutsideCouplingLoop, explicitScheme});
    if (state.fullModel == 0)
        state.input.cicloAcopTerm = 0;
    else
        state.input.cicloAcopTerm = 1;
    return state.input.cicloAcopTerm;
}

double advanceStandaloneTransientStep(const TransientSolveState &state,
                                      const TransientDependencies &dependencies,
                                      double reverseQuality, double reverseVoidFraction,
                                      double reverseLiquidFraction) {
    dependencies.hydrates();

    if (state.step.input.flashCompleto == 2 && (*state.step.globals).simulationTime < 1e-15 && state.step.input.miniTabAtraso > 0) {
        refreshFluidMiniTable(state.step);
    }

    if ((*state.step.globals).simulationTime < 1e-15) {
        updateValveBoxStates(state.step);
        for (int i = 0; i < state.step.input.nCelUnit; i++)
            state.unitCellTimeCounters[i] = 1;
        for (int i = 0; i < state.step.input.ntendp; i++) {
            state.step.input.imprimeTrend(state.step.cells, state.productionTrendMatrix[i], (*state.step.globals).simulationTime, i, state.productionTrendCounts[i]);
        }
        thermal::updateDistributedMassTransfer(dependencies.thermal, dependencies.thermalDependencies);
    }

    int explicitScheme = 0;
    state.step.fullModel = state.step.input.correcaoMassaEspLiq;
    state.step.input.atualizaSonico((*state.step.globals).simulationTime, explicitScheme);
    computeTimeStep(state.step, explicitScheme);
    state.step.auxiliaryCflTimeStep = state.step.timeStep;
    state.step.finalAuxiliaryTimeStep = state.step.timeStep;

    state.step.restart = 0;
    const int masterValveCell = state.step.input.master1.posic;
    captureValveAreaRatios(state.step, state.step.valveAreaRatiosBeforeUpdate);

    double maximumUnloadingFlowRate = 0.;
    if (state.step.input.controDesc == 1)
        maximumUnloadingFlowRate = gaslift::searchUnloadingInjectionPressure(dependencies.gasLift);
    thermal::advanceAxisymmetricThermalField(dependencies.thermal);
    gaslift::solveGasLine(dependencies.gasLift, dependencies.thermal);
    state.initialGasSurfacePressure = state.step.gasSurfacePressure;
    state.step.input.atualiza(state.startNode, state.step.endNode, state.annulusDrift, state.step.surfaceChoke, state.injectionChoke, state.step.cells, state.step.gasCells, state.step.gasSurfacePressure,
                 state.defaultInletTemperature, state.initialGasPressure, state.initialGasTemperature,
                 state.step.inletPressure, state.step.inletTemperature, state.step.inletQuality, state.step.inletComplementaryFraction, (*state.step.globals).simulationTime, state.step.timeStep);
    refreshInletCondition(state.step);

    const int maximumThermalCouplingCycle = prepareCouplingMode(state.step, explicitScheme);
    state.step.initiallyOpen = state.step.surfaceChokeOpen;
    state.initialOpenTime = state.step.openTime;
    for (int couplingIteration = 0; couplingIteration <= state.step.fullModel; couplingIteration++) {
        advanceCouplingIteration(state, dependencies, couplingIteration, masterValveCell, explicitScheme,
                                 maximumThermalCouplingCycle, reverseQuality, reverseVoidFraction,
                                 reverseLiquidFraction);
    }

    if (state.step.fullModel == 0 || state.step.input.cicloAcopTerm == 0) {
        for (int cycle = 0; cycle <= maximumThermalCouplingCycle; cycle++) {
            thermal::advanceTransientEnergy(dependencies.thermal, state.step, cycle, maximumThermalCouplingCycle);
        }
    }
    return maximumUnloadingFlowRate;
}

void finalizeBuriedPipeConduction(const TransientStepState &state,
                                 const std::vector<int> &cellIndices) {
    for (int cellIndex : cellIndices)
        state.cells[cellIndex].calor.poisson2D.finalizaPassoTransiente(state.timeStep, state.branchIndex);
}

void finalizeTransientCellState(const TransientSolveState &state, const TransientDependencies &dependencies, const ProFlu *fluiRev) {
    if (state.poissonSolver3D.itera > 7) {
        state.poissonSolver3D.penalizaDt = 20;
    }

    for (int i = 1; i <= state.step.lastCell; i++) {
        state.step.cells[i].dTdt = 0.;
        state.step.cells[i].dTdtL = 0.;
        if (state.step.fullModel == 0 || state.step.cells[i].estabCol == 1) {
            state.step.cells[i].dTdtIni = 0.;
            state.step.cells[i].d2pdt2 = 0.;
        }
    }

    if (state.step.input.modoParafina == 1)
        evaluateWaxDeposition(state.step);

    storePreviousSources(state.step);
    thermal::updateDistributedMassTransfer(dependencies.thermal, dependencies.thermalDependencies);

    state.step.outletPressure = state.step.cells[state.step.lastCell].pres;
    if (state.step.input.lingas > 0 && state.networkCoupled == 1)
        gaslift::connectTubing(dependencies.gasLift);

    if (state.step.input.flashCompleto == 2) {
        for (int i = 0; i < state.step.lastCell; i++) {
            state.step.cells[i].nMolIni = state.step.cells[i].nMol;
        }
    }
    storePreviousFractionsAndMovePigs(state.step);
    if ((state.trackGasOilRatio > 0 || state.trackGasGravity > 0) && state.step.input.flashCompleto != 2)
        fluidtracking::transportBlackOilProperties(dependencies.fluidTracking, reverseFlowFluid(fluiRev));
    if (state.step.input.flashCompleto == 2) {
        fluidtracking::transportPhaseMolarFractions(dependencies.fluidTracking, reverseFlowFluid(fluiRev));
        state.compositionalRefreshCounter++;
        if (state.compositionalRefreshCounter == state.step.input.miniTabAtraso + 1 && state.step.input.miniTabAtraso > 0) {
            generateFluidMiniTables(state.step);
            state.compositionalRefreshCounter = 0;
        }
    }
    cacheCellAndFaceDensities(state.step);
    updateValveBoxStates(state.step);
}

void updateTransientMovingMeans(const TransientSolveState &state) {
    state.timeStepHistory.push_back(state.step.timeStep);
    state.pressureHistory.push_back(state.step.outletPressure * state.step.timeStep);
    double jtemporario = state.step.cells[state.step.lastCell - 1].Mliqini / (state.step.cells[state.step.lastCell - 1].duto.area * ((1. - state.step.cells[state.step.lastCell - 1].bet) * state.step.cells[state.step.lastCell - 1].rpC + state.step.cells[state.step.lastCell - 1].bet * state.step.cells[state.step.lastCell - 1].rcC));
    jtemporario += (state.step.cells[state.step.lastCell - 1].MC - state.step.cells[state.step.lastCell - 1].Mliqini) / (state.step.cells[state.step.lastCell - 1].duto.area * state.step.cells[state.step.lastCell - 1].flui.MasEspGas(state.step.cells[state.step.lastCell - 1].pres, state.step.cells[state.step.lastCell - 1].temp));
    state.fluxHistory.push_back(jtemporario * state.step.timeStep);
    state.voidFractionHistory.push_back(state.step.cells[state.step.lastCell - 1].alf * state.step.timeStep);
    state.movingMeanCounter += state.step.timeStep;
    state.totalPressure += state.step.outletPressure * state.step.timeStep;
    state.totalFlux += jtemporario * state.step.timeStep;
    state.totalVoidFraction += state.step.cells[state.step.lastCell - 1].alf * state.step.timeStep;
    if ((*state.step.globals).simulationTime > state.step.movingMeanStartTime) {
        state.totalPressure -= state.pressureHistory.front();
        state.pressureHistory.pop_front();
        state.totalFlux -= state.fluxHistory.front();
        state.fluxHistory.pop_front();
        state.totalVoidFraction -= state.voidFractionHistory.front();
        state.voidFractionHistory.pop_front();
        state.movingMeanCounter -= state.timeStepHistory.front();
        state.timeStepHistory.pop_front();
        state.step.movingMeanPressure = state.totalPressure / state.movingMeanCounter;
        state.step.movingMeanFlux = state.totalFlux / state.movingMeanCounter;
        state.movingMeanVoidFraction = state.totalVoidFraction / state.movingMeanCounter;
    }
}

void updateActivePigState(const TransientStepState &state, int i) {
    state.movingPigCount++;
    int ipig = state.cells[i].indpig;
    state.cells[i].DelPig = state.input.pig[ipig].delpres;
    state.cells[i].RazAreaPig = state.input.pig[ipig].razarea;
    state.cells[i].cdpig = state.input.pig[ipig].cdPig;
    double flowArea = state.cells[i].duto.area;
    double superficialMixtureVelocityLeft = (state.cells[i].QL + state.cells[i].QG) / flowArea;
    double superficialMixtureVelocityRight;
    if (i < state.lastCell)
        superficialMixtureVelocityRight = (state.cells[i + 1].QL + state.cells[i + 1].QG) / flowArea;
    else
        superficialMixtureVelocityRight = (state.cells[i].QL + state.cells[i].QG) / flowArea;
    state.cells[i].velPigini = state.cells[i].velPig;
    state.cells[i].velPig = superficialMixtureVelocityLeft * state.cells[i].razPig + superficialMixtureVelocityRight * (1. - state.cells[i].razPig) - state.cells[i].VazaPig / flowArea;
    for (int j = 0; j < state.scheduledPigCount; j++) {
        if (state.pigReceiverCells[j] == i) {
            state.cells[i].velPig = 0.;
            state.cells[i].estadoPig = 0;
            state.cells[i].razPig = 0.;
            state.cells[i].razPigini = 0.;
            state.cells[i].alfPigDini = state.cells[i].alf;
            state.cells[i].betPigDini = state.cells[i].bet;
            state.cells[i].alfPigEini = state.cells[i].alf;
            state.cells[i].betPigEini = state.cells[i].bet;
            state.cells[i].alfPigD = state.cells[i].alf;
            state.cells[i].betPigD = state.cells[i].bet;
            state.cells[i].alfPigE = state.cells[i].alf;
            state.cells[i].betPigE = state.cells[i].bet;
            if (i > 0)
                state.cells[i - 1].alfPigER = state.cells[i].alfPigE;
            if (i == state.lastCell)
                state.cells[i].alfPigER = state.cells[i].alf;

            state.movingPigCount--;
        }
    }
}

void fillMiniTableCorner(const TransientStepState &state, ProFlu &fluC, ProFlu &flui,
                         int pressureIndex, int temperatureIndex) {
    const double &pressure = pressureIndex == 0 ? flui.miniTabDin.pmin : flui.miniTabDin.pmax;
    const double &temperature = temperatureIndex == 0 ? flui.miniTabDin.tmin : flui.miniTabDin.tmax;
    if (fluC.dCalculatedBeta > 0. && fluC.dCalculatedBeta < 1.)
        fluC.atualizaPropComp(pressure, temperature,
                              fluC.dCalculatedBeta, fluC.oCalculatedLiqComposition,
                              fluC.oCalculatedVapComposition, state.input.pocinjec);
    else
        fluC.atualizaPropComp(pressure, temperature, -1, NULL, NULL, state.input.pocinjec);
    flui.miniTabDin.rholF[pressureIndex][temperatureIndex] =
        fluC.MasEspoleo(pressure, temperature);
    flui.miniTabDin.rhogF[pressureIndex][temperatureIndex] =
        fluC.MasEspGas(pressure, temperature);
    flui.miniTabDin.DrhogDpF[pressureIndex][temperatureIndex] =
        fluC.drhodp(pressure, temperature);
    flui.miniTabDin.DrholDtF[pressureIndex][temperatureIndex] =
        fluC.DrholDT(pressure, temperature);
    flui.miniTabDin.DrhogDtF[pressureIndex][temperatureIndex] =
        fluC.drhodt(pressure, temperature);
    flui.miniTabDin.valBO[pressureIndex][temperatureIndex] =
        fluC.BOFunc(pressure, temperature);
    flui.miniTabDin.valZ[pressureIndex][temperatureIndex] =
        fluC.Zdran(pressure, temperature);
    flui.miniTabDin.valdZdT[pressureIndex][temperatureIndex] =
        fluC.DZDT(pressure, temperature);
    flui.miniTabDin.valdZdP[pressureIndex][temperatureIndex] =
        fluC.DZDP(pressure, temperature);
    flui.miniTabDin.tit[pressureIndex][temperatureIndex] =
        fluC.FracMass(pressure, temperature);
    flui.miniTabDin.rs[pressureIndex][temperatureIndex] =
        fluC.RS(pressure, temperature);
    flui.miniTabDin.cplF[pressureIndex][temperatureIndex] =
        fluC.CalorLiq(pressure, temperature);
    flui.miniTabDin.cpgF[pressureIndex][temperatureIndex] =
        fluC.CalorGas(pressure, temperature);
    flui.miniTabDin.HlF[pressureIndex][temperatureIndex] =
        fluC.EntalpLiq(pressure, temperature);
    flui.miniTabDin.HgF[pressureIndex][temperatureIndex] =
        fluC.EntalpGas(pressure, temperature);
}

void supplyAbsentGasProperties(ProFlu &flui, const std::pair<double, int> (&titVec)[4]) {
    if(titVec[0].first<1e-3){
        int busca=1;
        while(busca<4 && titVec[busca].first<1e-3)busca++;
        if(busca<4){
            const int jtroca = titVec[busca].second / 2;
            const int ktroca = titVec[busca].second % 2;
            for(int j=0;j<2;j++){
                for(int k=0;k<2;k++){
                    if(flui.miniTabDin.tit[j][k]<1e-3){
                        flui.miniTabDin.rhogF[j][k]=flui.miniTabDin.rhogF[jtroca][ktroca];
                        flui.miniTabDin.DrhogDpF[j][k]=flui.miniTabDin.DrhogDpF[jtroca][ktroca];
                        flui.miniTabDin.DrhogDtF[j][k]=flui.miniTabDin.DrhogDtF[jtroca][ktroca];
                        flui.miniTabDin.valZ[j][k]=flui.miniTabDin.valZ[jtroca][ktroca];
                        flui.miniTabDin.valdZdT[j][k]=flui.miniTabDin.valdZdT[jtroca][ktroca];
                        flui.miniTabDin.valdZdP[j][k]=flui.miniTabDin.valdZdP[jtroca][ktroca];
                        flui.miniTabDin.cpgF[j][k]=flui.miniTabDin.cpgF[jtroca][ktroca];
                        flui.miniTabDin.HgF[j][k]=flui.miniTabDin.HgF[jtroca][ktroca];
                    }
                }
            }
        }
    }
}

void supplyAbsentLiquidProperties(ProFlu &flui, const std::pair<double, int> (&titVec)[4]) {
    if(titVec[3].first>1.-1e-3){
        int busca=2;
        while(busca>=0 && titVec[busca].first>1.-1e-3)busca--;
        if(busca>=0){
            const int jtroca = titVec[busca].second / 2;
            const int ktroca = titVec[busca].second % 2;
            for(int j=0;j<2;j++){
                for(int k=0;k<2;k++){
                    if(flui.miniTabDin.tit[j][k]>1.-1e-3){
                        flui.miniTabDin.rholF[j][k]=flui.miniTabDin.rholF[jtroca][ktroca];
                        flui.miniTabDin.valBO[j][k]=flui.miniTabDin.valBO[jtroca][ktroca];
                        flui.miniTabDin.DrholDtF[j][k]=flui.miniTabDin.DrholDtF[jtroca][ktroca];
                        flui.miniTabDin.rs[j][k]=flui.miniTabDin.rs[jtroca][ktroca];
                        flui.miniTabDin.cplF[j][k]=flui.miniTabDin.cplF[jtroca][ktroca];
                        flui.miniTabDin.HlF[j][k]=flui.miniTabDin.HlF[jtroca][ktroca];
                    }
                }
            }
        }
    }
}

void refreshAccessoryFluidMiniTable(const TransientStepState &state, int i, ProFlu &accessoryFluid) {
    accessoryFluid.miniTabDin.pmax = state.cells[i].flui.miniTabDin.pmax;
    accessoryFluid.miniTabDin.pmin = state.cells[i].flui.miniTabDin.pmin;
    accessoryFluid.miniTabDin.tmax = state.cells[i].flui.miniTabDin.tmax;
    accessoryFluid.miniTabDin.tmin = state.cells[i].flui.miniTabDin.tmin;
    accessoryFluid.atualizaPropCompStandard();
    if (accessoryFluid.dCalculatedBeta > 0. && accessoryFluid.dCalculatedBeta < 1.)
        accessoryFluid.atualizaPropComp(state.cells[i].pres, state.cells[i].temp,
                                                       accessoryFluid.dCalculatedBeta, accessoryFluid.oCalculatedLiqComposition,
                                                       accessoryFluid.oCalculatedVapComposition, state.input.pocinjec);
    else
        accessoryFluid.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
    if(state.input.miniTabAtraso > 0)fillFluidMiniTable(state, accessoryFluid);
}

void refreshCompositionalInletQuality(const TransientStepState &state) {
    const Cel &inletCell = state.cells[0];
    (void)inletCell.flui.MasEspGas(state.inletPressure, state.inletTemperature);
    const double liquidDensity = inletCell.flui.MasEspLiq(state.inletPressure, state.inletTemperature);
    const double baseFluidGasMassFraction = inletCell.flui.FracMassHidra(state.inletPressure, state.inletTemperature);
    (void)inletCell.fluicol.standardDensity();
    const double complementaryDensity = inletCell.fluicol.MasEspFlu(state.inletPressure, state.inletTemperature);
    const double mixtureLiquidDensity =
        state.inletComplementaryFraction * complementaryDensity +
        (1. - state.inletComplementaryFraction) * liquidDensity;
    const double gasMassPerLiquidVolume =
        ((1. - state.inletComplementaryFraction) * liquidDensity * baseFluidGasMassFraction /
         (1. - baseFluidGasMassFraction));
    state.inletQuality = gasMassPerLiquidVolume / (mixtureLiquidDensity + gasMassPerLiquidVolume);
}

void refreshGasInletFractions(const TransientStepState &state) {
    const Cel &inletCell = state.cells[0];
    const double standardGasDensity = inletCell.flui.Deng * kAirDensityAtStandardConditions;
    const double gasDensity = inletCell.flui.MasEspGas(state.inletPressure, state.inletTemperature);
    const double oilDensity = inletCell.flui.MasEspoleo(state.inletPressure, state.inletTemperature);
    const double hydrocarbonGasMassFraction = inletCell.flui.FracMass(state.inletPressure, state.inletTemperature);
    const double standardComplementaryDensity = inletCell.fluicol.standardDensity();
    const double complementaryDensity = inletCell.fluicol.MasEspFlu(state.inletPressure, state.inletTemperature);
    const double complementaryRatioTerm =
        (standardComplementaryDensity / complementaryDensity) * (gasDensity / standardGasDensity) *
        state.input.CCPres.bet[0] / hydrocarbonGasMassFraction;
    const double liquidToGasVolumeRatio =
        (gasDensity / oilDensity) * (1 - hydrocarbonGasMassFraction) / hydrocarbonGasMassFraction;
    const double updatedInletQuality =
        gasDensity / (((1. - hydrocarbonGasMassFraction) / hydrocarbonGasMassFraction) * (gasDensity / oilDensity) +
                      gasDensity + complementaryRatioTerm);
    state.inletQuality = updatedInletQuality;
    state.inletComplementaryFraction = complementaryRatioTerm / (liquidToGasVolumeRatio + complementaryRatioTerm);
}

}  // namespace

void updateInteriorCell(const TransientStepState &state, int i, int expli) {
    state.cells[i].presini = state.cells[i].pres;
    state.cells[i].pres = state.productionSolution[2 * i + 1];
    if (isnan(state.cells[i].pres))
        NumError("Pressao na linha com valor NaN");
    state.cells[i].d2pdt2 = state.cells[i].dpdt;
    state.cells[i].dpdt = 0. * (state.cells[i].pres - state.cells[i].presini) / state.cells[i].dt;
    state.cells[i].MCini = state.cells[i].MC;
    state.cells[i].MC = state.productionSolution[2 * i];
    if (isnan(state.cells[i].MC))
        NumError("Vazao massica da mistura na linha com valor NaN");
    state.cells[i + 1].presLini = state.cells[i + 1].presL;
    state.cells[i - 1].presRini = state.cells[i - 1].presR;
    state.cells[i + 1].presL = state.cells[i - 1].presR = state.cells[i].pres;
    state.cells[i + 1].MLini = state.cells[i + 1].ML;
    state.cells[i - 1].MRini = state.cells[i - 1].MR;
    state.cells[i + 1].ML = state.cells[i - 1].MR = state.cells[i].MC;
    if (i <= state.lastCell - 2) {
        state.cells[i + 1].presLL = state.cells[i - 1].pres;
        state.cells[i + 1].MLL = state.cells[i - 1].MC;
    }

    state.cells[i].Mliqini0 = state.cells[i].Mliqini;
    state.cells[i].Mliqini = state.cells[i].term1 * state.cells[i].MC + state.cells[i].term2;
    state.cells[i + 1].MliqiniL0 = state.cells[i + 1].MliqiniL;
    state.cells[i - 1].MliqiniR0 = state.cells[i - 1].MliqiniR;
    state.cells[i + 1].MliqiniL = state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;

    double area = updateInteriorAuxiliaryPressure(state, i);

    updateInteriorVolumeFlows(state, i, area);

}

void updateFirstCell(const TransientStepState &state, int i, int expli) {
    state.cells[0].presini = state.cells[0].pres;
    state.cells[0].pres = state.productionSolution[1];
    state.cells[0].presLL = state.cells[0].pres;
    state.cells[0].ML = state.cells[0].MC;
    state.cells[0].MLL = state.cells[0].MC;
    state.cells[i].d2pdt2 = state.cells[i].dpdt;
    state.cells[i].dpdt = 0 * (state.cells[i].pres - state.cells[i].presini) / state.cells[i].dt;
    state.cells[0].presauxini = state.cells[0].presaux;
    state.cells[0].presaux = state.cells[0].pres;
    state.cells[0].dpresaux = 0.;
    state.cells[0].MCini = state.cells[0].MC;
    state.cells[0].MC = state.productionSolution[0];
    state.cells[1].presLini = state.cells[1].presL;
    state.cells[1].presL = state.cells[0].pres;
    state.cells[1].presLL = state.cells[0].pres;
    state.cells[1].MLini = state.cells[1].ML;
    state.cells[1].ML = state.cells[0].MC;
    state.cells[1].MLL = state.cells[0].MC;
    state.cells[0].Mliqini0 = state.cells[0].Mliqini;
    state.cells[0].Mliqini = state.cells[i].term1 * state.cells[i].MC + state.cells[i].term2;
    state.cells[1].MliqiniL0 = state.cells[1].MliqiniL;
    state.cells[1].MliqiniL = state.cells[0].Mliqini;

    state.cells[0].QLini = state.cells[0].QL;
    state.cells[0].QL = 0.;
    state.cells[1].QLLini = state.cells[1].QLL;
    state.cells[1].QLL = state.cells[0].QL;
    state.cells[0].QGini = state.cells[0].QG;
    state.cells[0].QG = 0.;

    if (state.input.ConContEntrada > 0) {
        double rhogC = state.cells[i].flui.MasEspGas(state.inletPressure, state.inletTemperature);
        double rhopC = state.cells[i].flui.MasEspLiq(state.inletPressure, state.inletTemperature);
        double rhocC = state.cells[i].fluicol.MasEspFlu(state.inletPressure, state.inletTemperature);
        double rholC = rhopC * (1 - state.inletComplementaryFraction) + rhocC * state.inletComplementaryFraction;
        state.cells[0].QL = state.cells[0].Mliqini / rholC;
        state.cells[1].QLL = state.cells[0].QL;
        state.cells[0].QG = (state.cells[0].MC - state.cells[0].Mliqini) / rhogC;
    }
}

void updateLastCell(const TransientStepState &state, int i, int expli) {
    state.cells[i].presini = state.cells[i].pres;
    state.cells[state.lastCell].pres = state.productionSolution[2 * state.lastCell + 1];
    state.cells[i].d2pdt2 = state.cells[i].dpdt;
    state.cells[i].dpdt = 0 * (state.cells[i].pres - state.cells[i].presini) / state.cells[i].dt;
    state.cells[i].d2pdt2 = (state.cells[i].dpdt - state.cells[i].d2pdt2) / state.cells[i].dt;
    state.cells[state.lastCell].MCini = state.cells[state.lastCell].MC;
    state.cells[state.lastCell].MC = state.productionSolution[2 * state.lastCell];
    state.cells[state.lastCell - 1].presRini = state.cells[state.lastCell - 1].presR;
    state.cells[state.lastCell - 1].presR = state.cells[state.lastCell].pres;
    state.cells[state.lastCell].presR = state.cells[state.lastCell].pres;
    state.cells[state.lastCell - 1].MRini = state.cells[state.lastCell - 1].MR;
    state.cells[state.lastCell - 1].MR = state.cells[state.lastCell].MC;
    state.cells[state.lastCell].MRini = state.cells[state.lastCell].MR;
    state.cells[state.lastCell].MR = state.cells[state.lastCell].MC;

    state.cells[state.lastCell].Mliqini0 = state.cells[state.lastCell].Mliqini;
    state.cells[state.lastCell].Mliqini = state.cells[state.lastCell].term1 * state.cells[state.lastCell].MC + state.cells[state.lastCell].term2;
    state.cells[i - 1].MliqiniR0 = state.cells[i - 1].MliqiniR;
    state.cells[state.lastCell - 1].MliqiniR = state.cells[state.lastCell].Mliqini;
    state.cells[state.lastCell].MliqiniR0 = state.cells[state.lastCell].MliqiniR;
    state.cells[state.lastCell].MliqiniR = state.cells[state.lastCell].Mliqini;

    state.cells[state.lastCell].QLini = state.cells[state.lastCell].QL;
    state.cells[state.lastCell].QGini = state.cells[state.lastCell].QG;

    updateOutletAuxiliaryPressure(state, i);

    updateOutletVolumeFlows(state, i);

}

void updateCells(const TransientStepState &state, int expli) {
    for (int i = 0; i <= state.lastCell; i++) {
        if (i != 0 && i != state.lastCell) {
            updateInteriorCell(state, i, expli);
        } else if (i == 0) {
            updateFirstCell(state, i, expli);
        } else {
            updateLastCell(state, i, expli);
        }
    }
    for (int i = 0; i < state.lastCell; i++) {
        state.cells[i].MRR = state.cells[i + 1].MR;
        state.cells[i].MliqiniRR = state.cells[i + 1].MliqiniR;
        state.cells[i].presRR = state.cells[i + 1].presR;
    }
    state.cells[state.lastCell].MRR = state.cells[state.lastCell].MR;
    state.cells[state.lastCell].MliqiniRR = state.cells[state.lastCell].MliqiniR;
    state.cells[state.lastCell].presRR = state.cells[state.lastCell].presR;
}

void updateFlowRates(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        if (i != 0 && i != state.lastCell) {
            state.cells[i].Mliqini = state.cells[i].term1 * state.cells[i].MC + state.cells[i].term2;
            state.cells[i + 1].MliqiniL = state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;

            double tmed = state.cells[i - 1].temp;
            if (state.cells[i].VTemper < 0.)
                tmed = state.cells[i].temp;
            const ProFlu &flud = state.cells[i].Mliqini < 0.
                                     ? state.cells[i].flui : state.cells[i - 1].flui;

            double betI;
            if (((state.cells[i].MC - state.cells[i].Mliqini) * 0 + 1 * state.cells[i].Mliqini) < 0)
                betI = state.cells[i].bet;
            else
                betI = state.cells[i].betL;

            const FacePhaseDensities densities = facePhaseDensitiesOf(state, i, flud, tmed, betI);
            double rhol = densities.liquid;
            double gasDensity = densities.gas;
            state.cells[i].QL = state.cells[i].Mliqini / rhol;
            state.cells[i].QG = (state.cells[i].MC - state.cells[i].Mliqini) / gasDensity;
            state.cells[i - 1].QLR = state.cells[i].QL;
            if (i < state.lastCell) {
                state.cells[i + 1].QLL = state.cells[i].QL;
            }

        } else if (i == 0) {

            state.cells[0].Mliqini = state.cells[i].term1 * state.cells[i].MC + state.cells[i].term2;
            state.cells[1].MliqiniL = state.cells[0].Mliqini;

            if (state.input.ConContEntrada > 0) {
                double rhogC = state.cells[i].flui.MasEspGas(state.inletPressure, state.inletTemperature);
                double rhopC = state.cells[i].flui.MasEspLiq(state.inletPressure, state.inletTemperature);
                double rhocC = state.cells[i].fluicol.MasEspFlu(state.inletPressure, state.inletTemperature);
                double rholC = rhopC * (1 - state.inletComplementaryFraction) + rhocC * state.inletComplementaryFraction;
                state.cells[0].QL = state.cells[0].Mliqini / rholC;
                state.cells[1].QLL = state.cells[0].QL;
                state.cells[0].QG = (state.cells[0].MC - state.cells[0].Mliqini) / rhogC;
            }
        } else {

            state.cells[state.lastCell].Mliqini = state.cells[state.lastCell].term1 * state.cells[state.lastCell].MC + state.cells[state.lastCell].term2;
            state.cells[state.lastCell - 1].MliqiniR = state.cells[state.lastCell].Mliqini;
            state.cells[state.lastCell].MliqiniR = state.cells[state.lastCell].Mliqini;

            double tmed = state.cells[i - 1].temp;
            if (state.cells[i].VTemper < 0.)
                tmed = state.cells[i].temp;

            const ProFlu &flud = state.cells[i].Mliqini < 0.
                                     ? state.cells[i].flui : state.cells[i - 1].flui;

            double betI;
            if (((state.cells[i].MC - state.cells[i].Mliqini) * 0.99 + 0.01 * state.cells[i].Mliqini) < 0)
                betI = state.cells[i].bet;
            else
                betI = state.cells[i].betL;

            const FacePhaseDensities densities = facePhaseDensitiesOf(state, i, flud, tmed, betI);
            double rhol = densities.liquid;
            double gasDensity = densities.gas;

            state.cells[i].QL = state.cells[i].Mliqini / rhol;
            state.cells[i].QG = (state.cells[i].MC - state.cells[i].Mliqini) / gasDensity;
            state.cells[i - 1].QLR = state.cells[i].QL;
        }
    }
}

void updateBufferFromSolution(const TransientStepState &state) {

    state.cells[0].presBuf = state.productionSolution[1];
    state.cells[0].presauxBuf = state.cells[0].presBuf;
    state.cells[0].MCBuf = state.productionSolution[0];
    state.cells[1].presLiniBuf = state.cells[1].presLBuf;
    state.cells[1].presLBuf = state.cells[0].presBuf;
    state.cells[1].MLBuf = state.cells[0].MCBuf;
    state.cells[0].MliqiniBuf = state.cells[0].term1 * state.cells[0].MCBuf + state.cells[0].term2;
    state.cells[1].MliqiniLBuf = state.cells[0].MliqiniBuf;

    int i = 1;
    state.cells[i].presBuf = state.productionSolution[2 * i + 1];
    state.cells[i].MCBuf = state.productionSolution[2 * i];
    state.cells[i + 1].presLiniBuf = state.cells[i + 1].presLBuf;
    state.cells[i + 1].presLBuf = state.cells[i - 1].presRBuf = state.cells[i].presBuf;
    state.cells[i - 1].MRiniBuf = state.cells[i - 1].MRBuf;
    state.cells[i + 1].MLBuf = state.cells[i - 1].MRBuf = state.cells[i].MCBuf;

    state.cells[i].MliqiniBuf = state.cells[i].term1 * state.cells[i].MCBuf + state.cells[i].term2;
    state.cells[i + 1].MliqiniLBuf = state.cells[i - 1].MliqiniRBuf = state.cells[i].MliqiniBuf;

    state.cells[state.lastCell].presBuf = state.productionSolution[2 * state.lastCell + 1];
    state.cells[state.lastCell].MCBuf = state.productionSolution[2 * state.lastCell];
    state.cells[state.lastCell - 1].presRBuf = state.cells[state.lastCell].presBuf;
    state.cells[state.lastCell - 1].MRBuf = state.cells[state.lastCell].MCBuf;
    state.cells[state.lastCell].MRBuf = state.cells[state.lastCell].MCBuf;

    state.cells[state.lastCell].MliqiniBuf = state.cells[state.lastCell].term1 * state.cells[state.lastCell].MCBuf + state.cells[state.lastCell].term2;
    state.cells[state.lastCell - 1].MliqiniRBuf = state.cells[state.lastCell].MliqiniBuf;
    state.cells[state.lastCell].MliqiniRBuf = state.cells[state.lastCell].MliqiniBuf;
}

void updateBufferFromCells(const TransientStepState &state) {

    state.cells[0].presBuf = state.cells[0].pres;
    state.cells[0].presauxBuf = state.cells[0].presaux;
    state.cells[0].MCBuf = state.cells[0].MC;
    state.cells[1].presLiniBuf = state.cells[1].presLini;
    state.cells[1].presLBuf = state.cells[1].presL;
    state.cells[1].MLBuf = state.cells[1].ML;
    state.cells[0].MliqiniBuf = state.cells[0].Mliqini;
    state.cells[1].MliqiniLBuf = state.cells[1].MliqiniL;

    int i = 1;
    state.cells[i].presBuf = state.cells[i].pres;
    state.cells[i].MCBuf = state.cells[i].MC;
    state.cells[i + 1].presLiniBuf = state.cells[i + 1].presLini;
    state.cells[i + 1].presLBuf = state.cells[i - 1].presRBuf = state.cells[i].pres;
    state.cells[i - 1].MRiniBuf = state.cells[i - 1].MR;
    state.cells[i + 1].MLBuf = state.cells[i - 1].MRBuf = state.cells[i].MC;

    state.cells[i].MliqiniBuf = state.cells[i].Mliqini;
    state.cells[i + 1].MliqiniLBuf = state.cells[i - 1].MliqiniRBuf = state.cells[i].Mliqini;

    state.cells[state.lastCell].presBuf = state.cells[state.lastCell].pres;
    state.cells[state.lastCell].MCBuf = state.cells[state.lastCell].MC;
    state.cells[state.lastCell - 1].presRBuf = state.cells[state.lastCell].pres;
    state.cells[state.lastCell - 1].MRBuf = state.cells[state.lastCell].MC;

    state.cells[state.lastCell].MliqiniBuf = state.cells[state.lastCell].Mliqini;
    state.cells[state.lastCell - 1].MliqiniRBuf = state.cells[state.lastCell].Mliqini;

    state.bufferedComplementaryMassSource = state.cells[state.lastCell].fontemassCR;

    state.bufferedLiquidMassSource = state.cells[state.lastCell].fontemassLR;

    state.bufferedGasMassSource = state.cells[state.lastCell].fontemassGR;
}

bool surfaceChokeIsOpen(const TransientStepState &state) {
    return state.surfaceChoke.AreaGarg > (1e-3) * state.cells[state.lastCell - 1].duto.area;
}

bool surfaceChokeIsShut(const TransientStepState &state) {
    return state.surfaceChoke.AreaGarg < (1e-3) * state.cells[state.lastCell - 1].duto.area;
}

void applyOutletPressureCondition(const TransientStepState &state, [[maybe_unused]] double titRev, [[maybe_unused]] double alfRev, double betRev) {

    double tESup = state.cells[state.lastCell].temp;
    double alfSup = state.cells[state.lastCell].alf;
    double betSup = state.cells[state.lastCell].bet;

    double masentrada = state.cells[state.lastCell - 1].MR;
    double massgas = state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR;

    double rholp = state.cells[state.lastCell].rpC;
    double rholc = state.cells[state.lastCell].rcC;
    double rholmix = (1 - betSup) * rholp + betSup * rholc;
    double romix = alfSup * state.cells[state.lastCell].rgC + (1 - alfSup) * rholmix;

    double chokeGasMassFraction = currentOutletGasMassFraction(state, alfSup, massgas, masentrada, romix);

    romix = chokeGasMassFraction * (1. / state.cells[state.lastCell].rgC) + (1 - chokeGasMassFraction) * (1. / rholmix);
    romix = 1 / romix;

    double masChk;

    double sinal = 1.;
    double pmon = state.outletPressure;

    double ypres = state.gasSurfacePressure / state.outletPressure;
    if (surfaceChokeIsOpen(state) && ypres < 1.) {
        double cplM = (1. - betSup) * state.cells[state.lastCell].flui.CalorLiq(state.outletPressure, tESup) +
                      betSup * state.cells[state.lastCell].fluicol.CalorLiq(state.outletPressure, tESup);
        double jtlM = (1. - betSup) * state.cells[state.lastCell].flui.JTL(state.outletPressure, tESup) - betSup / rholc;
        double gasSpecificHeat = state.cells[state.lastCell].flui.CalorGas(state.outletPressure, tESup);
        double jtgM = state.cells[state.lastCell].flui.JTG(state.outletPressure, tESup);
        state.input.valTempChokeJus = tESup + ((1. - chokeGasMassFraction) * jtlM / cplM + chokeGasMassFraction * jtgM / gasSpecificHeat) * (state.gasSurfacePressure - state.outletPressure) * units::kPascalPerKgfPerCm2;
    }
    if (ypres > 1.) {
        if (state.input.chkv == 0)
            sinal = -1.;
        else
            sinal = 0.;
        chokeGasMassFraction = 1.;
        pmon = state.gasSurfacePressure;
        ypres = 1. / ypres;
    }

    masChk = state.surfaceChoke.vazmassSachd(ypres, pmon, tESup, alfSup, betSup, chokeGasMassFraction, state.cells[state.lastCell - 1].flui,
                                   state.cells[state.lastCell - 1].fluicol);
    double maxSup = state.surfaceChoke.vazmaxSachd(pmon, tESup, alfSup, betSup, chokeGasMassFraction, state.cells[state.lastCell - 1].flui, state.cells[state.lastCell - 1].fluicol);

    if (chokeGasMassFraction <= 0.01 || fabs(ypres) > fabs(state.surfaceChoke.razpres)) {
        maxSup = masChk;
    }
    if (surfaceChokeIsShut(state))
        maxSup = 0.;

    double sinal2 = 1.;
    pmon = state.outletPressure * 1.0001;
    ypres = state.gasSurfacePressure / pmon;
    if (ypres > 1.) {
        chokeGasMassFraction = 1;
        pmon = state.gasSurfacePressure;
        ypres = 1. / ypres;
        if (state.input.chkv == 0)
            sinal2 = -1.;
        else
            sinal2 = 0.;
    }

    double masChk2 = state.surfaceChoke.vazmassSachd(ypres, pmon, tESup, alfSup, betSup, chokeGasMassFraction, state.cells[state.lastCell - 1].flui,
                                           state.cells[state.lastCell - 1].fluicol);
    double maxSup2 = state.surfaceChoke.vazmaxSachd(pmon, tESup, alfSup, betSup, chokeGasMassFraction, state.cells[state.lastCell - 1].flui,
                                          state.cells[state.lastCell - 1].fluicol);

    if (chokeGasMassFraction <= 0.01 || fabs(ypres) > fabs(state.surfaceChoke.razpres)) {
        maxSup2 = masChk2;
    }
    if (surfaceChokeIsShut(state))
        maxSup2 = 0.;
    double dmaxsup = (sinal2 * maxSup2 - sinal * maxSup) / (state.outletPressure * 0.0001);

    double masliq;
    double masgas;
    int abertoini = state.surfaceChokeOpen;
    double delp;
    if (surfaceChokeIsOpen(state))
        delp = (0.5 / units::kPascalPerKgfPerCm2) * (1 / romix) * (1 / (state.surfaceChoke.AreaGarg * state.surfaceChoke.AreaGarg * state.surfaceChoke.cdchk * state.surfaceChoke.cdchk)) * masentrada * masentrada;
    else
        delp = 0;

    int masChkSup0 = state.surfaceChokeMassFlag;
    state.chokeModeChanged = 0;

    if (currentOutletUsesPressureDrop(state, chokeGasMassFraction, delp, masentrada, maxSup)) {
        state.surfaceChokeOpen = 1;
        double sens = 1.;
        if (masentrada < 0. && state.cells[state.lastCell - 1].MliqiniR <= 0)
            sens = 0.;
        state.outletPressure = state.gasSurfacePressure + sens * delp;
        state.surfaceChokeMassFlag = 0;
        if (state.surfaceChokeMassFlag != masChkSup0)
            state.chokeModeChanged = 1;

    } else {
        if (currentOutletCanRemainOpen(state, delp)) {
            state.surfaceChokeOpen = 1;
            if (abertoini != state.surfaceChokeOpen)
                state.openTime = 1;
        } else {
            state.surfaceChokeOpen = 0;
            if (state.openTime > 60)
                state.openTime = 0;
        }
        if ((((chokeGasMassFraction > -0.01 && state.cells[state.lastCell].alf > -0.01) || surfaceChokeIsShut(state)) && state.surfaceChoke.AreaGarg < 0.6 * state.cells[state.lastCell - 1].duto.area && (state.surfaceChokeOpen == 0 && (state.openTime == 0 || state.openTime > 60)))) {

            state.surfaceChokeOpen = 0;
            state.openTime = 0;
            masliq = sinal * maxSup * (1. - chokeGasMassFraction);
            masgas = sinal * maxSup * chokeGasMassFraction;

            state.cells[state.lastCell].DmasschokeG = -1 * chokeGasMassFraction * dmaxsup;
            state.cells[state.lastCell].DmasschokeL = -1 * (1. - chokeGasMassFraction) * ((1 - betSup) * rholp / rholmix) * dmaxsup;
            state.cells[state.lastCell].DmasschokeC = -1 * (1. - chokeGasMassFraction) * (betSup * rholc / rholmix) * dmaxsup;

            state.surfaceChokeMassFlag = 1;
            if (state.surfaceChokeMassFlag != masChkSup0)
                state.chokeModeChanged = 1;
            state.cells[state.lastCell].fontemassLR = -masliq * (1 - betSup) * rholp / rholmix;
            state.cells[state.lastCell].fontemassCR = -masliq * betSup * rholc / rholmix;
            state.cells[state.lastCell].fontemassGR = -masgas;

        } else {
            if (state.openTime != 0)
                state.openTime++;
            state.outletPressure = state.gasSurfacePressure;
            state.surfaceChokeOpen = 1;
            state.surfaceChokeMassFlag = 0;
            if (state.surfaceChokeMassFlag != masChkSup0)
                state.chokeModeChanged = 1;
        }
    }

    if (state.surfaceChokeMassFlag == 0 && (*state.globals).networkMode == 1) {
        double betloc;
        if ((state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR) * 0 + 1 * state.cells[state.lastCell - 1].MliqiniR > 0.)
            betloc = state.cells[state.lastCell - 1].bet;
        else
            betloc = betRev;

        if (state.input.chkv == 0 || state.cells[state.lastCell - 1].MR > 0.)
            sinal = 1.;
        else
            sinal = 0.;

        state.cells[state.lastCell].fontemassCR = -sinal * state.cells[state.lastCell - 1].QLR * (betloc)*state.cells[state.lastCell - 1].rcC;
        state.cells[state.lastCell].fontemassLR = -sinal * (state.cells[state.lastCell - 1].MliqiniR + state.cells[state.lastCell].fontemassCR);
        state.cells[state.lastCell].fontemassGR = -sinal * (state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR);
    }
}

void applyOutletBufferCondition(const TransientStepState &state, double titRev, double alfRev, double betRev) {

    double tESup = state.cells[state.lastCell].temp;
    double alfSup = state.cells[state.lastCell].alf;
    double betSup = state.cells[state.lastCell].bet;

    double masentrada = state.cells[state.lastCell - 1].MRBuf;
    double massgas = state.cells[state.lastCell - 1].MRBuf - state.cells[state.lastCell - 1].MliqiniRBuf;

    double rholp = state.cells[state.lastCell].flui.MasEspLiq(state.cells[state.lastCell].presBuf, state.cells[state.lastCell].temp);
    double rholc = state.cells[state.lastCell].fluicol.MasEspFlu(state.cells[state.lastCell].presBuf, state.cells[state.lastCell].temp);
    double rholmix = (1 - betSup) * rholp + betSup * rholc;
    double romix = alfSup * state.cells[state.lastCell].flui.MasEspGas(state.cells[state.lastCell].presBuf, state.cells[state.lastCell].temp) + (1 - alfSup) * rholmix;

    double chokeGasMassFraction;
    if (massgas > 0 && state.cells[state.lastCell - 1].MliqiniRBuf < 0)
        chokeGasMassFraction = 1.;
    else if (massgas <= 0 && state.cells[state.lastCell - 1].MliqiniRBuf >= 0)
        chokeGasMassFraction = 0.;
    else if (masentrada < 0) {
        if ((*state.globals).networkMode == 0 || state.endNode == 1)
            chokeGasMassFraction = 1.;
        else {
            chokeGasMassFraction = titRev;
            alfSup = alfRev;
            betSup = betRev;
        }
    } else if (fabs(masentrada) < 1e-15)
        chokeGasMassFraction = inSituGasMassFraction(state.cells[state.lastCell], alfSup, state.cells[state.lastCell].presBuf, romix);
    else
        chokeGasMassFraction = flowQuality(massgas, masentrada);
    if (chokeGasMassFraction > 1)
        chokeGasMassFraction = 1;
    if (state.cells[state.lastCell].presBuf < state.gasSurfacePressure) {
        if ((*state.globals).networkMode == 0 || state.endNode == 1)
            chokeGasMassFraction = 1.;
        else {
            chokeGasMassFraction = titRev;
            alfSup = alfRev;
            betSup = betRev;
        }
    }

    if (chokeGasMassFraction == 0 && alfSup > 0.05)
        chokeGasMassFraction = inSituGasMassFraction(state.cells[state.lastCell], alfSup, state.cells[state.lastCell].presBuf, romix);

    romix = chokeGasMassFraction * (1. / state.cells[state.lastCell].rgC) + (1 - chokeGasMassFraction) * (1. / rholmix);
    romix = 1 / romix;

    double masChk;

    double sinal = 1.;
    double pmon = state.cells[state.lastCell].presBuf;

    double ypres = state.gasSurfacePressure / state.cells[state.lastCell].presBuf;
    if (ypres > 1.) {
        if (state.input.chkv == 0)
            sinal = -1.;
        else
            sinal = 0.;
        if ((*state.globals).networkMode == 0 || state.endNode == 1)
            chokeGasMassFraction = 1.;
        else {
            chokeGasMassFraction = titRev;
            alfSup = alfRev;
            betSup = betRev;
        }
        pmon = state.gasSurfacePressure;
        ypres = 1. / ypres;
    }

    masChk = state.surfaceChoke.vazmassSachd(ypres, pmon, tESup, alfSup, betSup, chokeGasMassFraction, state.cells[state.lastCell - 1].flui,
                                   state.cells[state.lastCell - 1].fluicol);
    double maxSup = state.surfaceChoke.vazmaxSachd(pmon, tESup, alfSup, betSup, chokeGasMassFraction, state.cells[state.lastCell - 1].flui,
                                  state.cells[state.lastCell - 1].fluicol);

    if (chokeGasMassFraction <= 0.01 || fabs(ypres) > fabs(state.surfaceChoke.razpres)) {
        maxSup = masChk;
    }
    if (surfaceChokeIsShut(state))
        maxSup = 0.;

    double masliq;
    double masgas;

    double delp;
    if (surfaceChokeIsOpen(state))
        delp = (0.5 / units::kPascalPerKgfPerCm2) * (1 / romix) *
               (1 / (state.surfaceChoke.AreaGarg * state.surfaceChoke.AreaGarg * state.surfaceChoke.cdchk * state.surfaceChoke.cdchk)) * masentrada * masentrada;
    else
        delp = 0.;

    if (bufferedOutletUsesPressureDrop(state, chokeGasMassFraction, delp, masentrada, maxSup)) {
        state.surfaceChokeOpen = 1;
        updateBufferedNetworkSources(state, betRev);
    } else {
        if (((chokeGasMassFraction > -0.01 && state.cells[state.lastCell].alf > -0.01) || surfaceChokeIsShut(state)) &&
            state.surfaceChoke.AreaGarg < 0.6 * state.cells[state.lastCell - 1].duto.area && (state.surfaceChokeOpen == 0 && (state.openTime == 0 || state.openTime > 60))) {

            masliq = sinal * maxSup * (1. - chokeGasMassFraction);
            masgas = sinal * maxSup * chokeGasMassFraction;
            state.bufferedLiquidMassSource = -masliq * (1 - betSup) * rholp / rholmix;
            state.bufferedComplementaryMassSource = -masliq * betSup * rholc / rholmix;
            state.bufferedGasMassSource = -masgas;
        }
        if (state.surfaceChokeMassFlag == 1 && (*state.globals).networkMode == 1) {
            state.cells[state.lastCell].fontemassCR = state.bufferedComplementaryMassSource;
            state.cells[state.lastCell].fontemassLR = state.bufferedLiquidMassSource;
            state.cells[state.lastCell].fontemassGR = state.bufferedGasMassSource;

            state.cells[state.lastCell].DmasschokeG = 0.;
            state.cells[state.lastCell].DmasschokeL = 0.;
            state.cells[state.lastCell].DmasschokeC = 0.;
        } else if (state.surfaceChokeMassFlag == 0 && (*state.globals).networkMode == 1) {
            updateBufferedNetworkSources(state, betRev);
        }
    }
}

void computeImplicitTimeStep(const TransientStepState &state, int explicitScheme) {
    applyChokeTimeStepControls(state);
    restrictTimeStepByTransport(state);

    if (state.timeStepRestricted == 1 && state.input.desligaPenalizaDT == 0) {
        if (state.timeChanged < 2)
            state.timeStep /= 10.;
        state.timeChanged++;
        if (state.timeChanged > 10)
            state.timeChanged = 0;
    }

    state.timeStep = state.multiplier * state.timeStep;
    if (state.timeStep > state.input.dtmax)
        state.timeStep = state.input.dtmax;

    applyScheduledTimeStepEvents(state);

    if (explicitScheme != 0)
        computeExplicitTimeStep(state);

    assignCellTimeSteps(state);

    for (int i = 0; i <= state.lastCell; i++) {
        if (state.cells[i].acsr.tipo == kAccessoryRadialPorous) {
            state.cells[i].acsr.radialPoro.dt = state.timeStep;
        } else if (state.cells[i].acsr.tipo == kAccessoryPorous2D) {
            state.cells[i].acsr.poroso2D.dt = state.timeStep;
        }
    }
}

void computeExplicitTimeStep(const TransientStepState &state) {

    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].relacoesSonicas();
        double maximumPropagationSpeed = fabs(state.cells[i].autval1);
        if (fabs(state.cells[i].autval1) < fabs(state.cells[i].autval2))
            maximumPropagationSpeed = fabs(state.cells[i].autval2);
        double dtaux = 0.8 * state.cells[i].dx / maximumPropagationSpeed;
        if (dtaux < state.timeStep)
            state.timeStep = dtaux;
    }
}

void computeTimeStep(const TransientStepState &state, int vexpli) {

    if ((*state.globals).simulationTime < (*state.globals).smallValueTolerance && (*state.globals).networkMode == 0) {
        state.input.imprimeProfile(state.cells, state.productionFreeTerms, (*state.globals).simulationTime, state.branchIndex);
        if (state.input.lingas > 0 && state.input.nvalvgas > 0)
            state.input.imprimeProfileG(state.gasCells, state.gasFreeTerms, (*state.globals).simulationTime, state.branchIndex);
        state.input.imprimeProfileTrans(state.cells, state.productionCrossSectionCount.data(), (*state.globals).simulationTime, state.branchIndex);
    }
    state.timeStep = state.input.dtmax;

    penalizeOscillatingFractions(state);

    for (int i = 0; i <= state.lastCell; i++) {
        if (state.cells[i].acsr.tipo == kAccessoryRadialPorous) {
            state.cells[i].acsr.radialPoro.defineDT(0);
            if (state.cells[i].acsr.radialPoro.dt < state.timeStep)
                state.timeStep = state.cells[i].acsr.radialPoro.dt;
        }
        if (state.cells[i].acsr.tipo == kAccessoryPorous2D) {
            state.cells[i].acsr.poroso2D.defineDT(0);
            if (state.cells[i].acsr.poroso2D.dt < state.timeStep)
                state.timeStep = state.cells[i].acsr.poroso2D.dt;
        }
    }

    computeImplicitTimeStep(state, vexpli);
}

void captureValveAreaRatios(const TransientStepState &state, double *destination) {
    int celpos = state.input.master1.posic;
    destination[0] = state.cells[celpos].acsr.chk.AreaGarg / state.cells[celpos].duto.area;
    for (int i = 1; i <= state.input.nvalv; i++) {
        celpos = state.input.valv[i - 1].posicP;
        destination[i] = state.cells[celpos].acsr.chk.AreaGarg / state.cells[celpos].duto.area;
    }
}

void dampMaximumTimeStep(const TransientStepState &state) {

    state.cflTimeSteps.push_back(state.auxiliaryCflTimeStep);
    state.simulationTimeSteps.push_back(state.finalAuxiliaryTimeStep);
    state.totalCflTimeStep += state.auxiliaryCflTimeStep;
    state.totalSimulationTimeStep += state.finalAuxiliaryTimeStep;
    state.timeStepRestrictionCount++;
    if (state.stepIndex > 10) {
        state.totalCflTimeStep -= state.cflTimeSteps.front();
        state.cflTimeSteps.erase(state.cflTimeSteps.begin());
        state.totalSimulationTimeStep -= state.simulationTimeSteps.front();
        state.simulationTimeSteps.erase(state.simulationTimeSteps.begin());
        if (state.timeStepRestrictionCount > 10 && state.timeStepRestricted == 1) {
            state.timeStepRestricted = 0;
            state.timeStepRestrictionCount = 0;
        }
    }
    state.meanCflTimeStep = state.totalCflTimeStep / 10;
    state.meanSimulationTimeStep = state.totalSimulationTimeStep / 10.;
    if ((state.meanSimulationTimeStep < state.meanCflTimeStep / 2 && state.timeStepRestrictionCount > 10)) {
        state.timeStepRestricted = 1;
        state.timeStepRestrictionCount = 0;
    }
}

void evaluatePressureRateOfChange(const TransientStepState &state, int vexpli) {
    double dpdtRef = 2 * state.input.taxaDespre;
    double dpdtMax = 0.;
    double dTdtMax = 0.;
    if (state.fullModel == 1) {
        state.fullModel = 0;
        int firstCell = 0;
        while (firstCell < state.lastCell) {
            double modDpDt = 0.;
            double modDTDt = 0.;
            int nBloco = 10;
            if (state.lastCell - firstCell < nBloco)
                nBloco = state.lastCell - firstCell;
            for (int j = 0; j < nBloco; j++) {
                modDpDt += fabs(state.cells[firstCell + j].pres - state.cells[firstCell + j].presini) / (state.timeStep);
                modDTDt += fabs(state.cells[firstCell + j].temp - state.cells[firstCell + j].tempini) / (state.timeStep);
            }
            firstCell += nBloco;
            modDpDt /= nBloco;
            modDTDt /= nBloco;
            if (modDpDt > dpdtMax)
                dpdtMax = modDpDt;
            if (modDTDt > dTdtMax)
                dTdtMax = modDTDt;
            if (modDpDt > dpdtRef)
                state.fullModel = 1;
        }
        state.maximumPressureRates.push_back(dpdtMax);
        state.maximumTemperatureRates.push_back(dTdtMax);
        if (state.maximumPressureRates.size() > 10)
            state.maximumPressureRates.erase(state.maximumPressureRates.begin());
        if (state.maximumTemperatureRates.size() > 10)
            state.maximumTemperatureRates.erase(state.maximumTemperatureRates.begin());
        int nvec = state.maximumPressureRates.size();
        int nvecT = state.maximumTemperatureRates.size();
        state.meanMaximumPressureRate = 0.;
        state.meanMaximumTemperatureRate = 0.;
        for (int i = 0; i < nvec; i++)
            state.meanMaximumPressureRate += state.maximumPressureRates[i];
        for (int i = 0; i < nvecT; i++)
            state.meanMaximumTemperatureRate += state.maximumTemperatureRates[i];
        state.meanMaximumPressureRate /= nvec;
        state.meanMaximumTemperatureRate /= nvecT;
        if (state.meanMaximumPressureRate > dpdtRef)
            state.fullModel = 1;
        if (state.surfaceChoke.AreaGarg / state.cells[state.lastCell - 1].duto.area < 1e-3 &&
            (state.meanMaximumPressureRate > state.input.taxaDespre / 10. || state.meanMaximumTemperatureRate > 0.001))
            state.fullModel = 1;
        int linAberta = 1;
        for (int i = 0; i <= state.input.nvalv; i++)
            if (state.valveAreaRatiosAfterUpdate[i] <= 1e-3)
                linAberta = 0;
        if ((linAberta == 1 && state.surfaceChoke.AreaGarg / state.cells[state.lastCell - 1].duto.area > 1e-3))
            state.fullModel = 1;
    }
    for (int i = 0; i <= state.input.nvalv; i++)
        if (state.valveAreaRatiosAfterUpdate[i] != state.valveAreaRatiosBeforeUpdate[i])
            state.fullModel = 0;
    if ((state.meanMaximumPressureRate > 10 || state.meanMaximumTemperatureRate > 1) && state.fullModel == 1 && vexpli == 0) {
        state.fullModel = 0;
    }
}

void restrictTimeStepByValve(const TransientStepState &state) {

    if (state.input.ConContEntrada == 0 && (*state.globals).parallelNetworkMode == 0) {
        double dtaux = state.cells[0].dt;
        if (state.restart == -1) {
            for (int i = 1; i <= state.lastCell; i++)
                if (dtaux > state.cells[i].dt)
                    dtaux = state.cells[i].dt;
        }
        double minimumTimeStep = valveRestrictedTimeStep(state, 0, dtaux);
        for (int i = 1; i <= state.input.nvalv; i++) {
            double candidate = valveRestrictedTimeStep(state, i, dtaux);
            if (candidate < minimumTimeStep)
                minimumTimeStep = candidate;
        }
        state.cells[0].dt = minimumTimeStep;
    }
}

void restartFractionEvolutionInitial(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        const Cel &cell = state.cells[i];
        if (cell.dt < state.timeStep)
            state.timeStep = cell.dt;
        if (cell.dt1 < state.timeStep)
            state.timeStep = cell.dt1;
        if (cell.dt2 < state.timeStep)
            state.timeStep = cell.dt2;
        if (cell.dtPig < state.timeStep)
            state.timeStep = cell.dtPig;
    }
}

void restartFractionEvolutionSub(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        if (state.cells[i].pres > -10.) {
            state.cells[i].alf = state.cells[i].alfini;
            state.cells[i].alfPigE = state.cells[i].alfPigEini;
            state.cells[i].alfPigD = state.cells[i].alfPigDini;
        }
    }
}

void restartFractionEvolution(const TransientStepState &state, const gaslift::GasLiftState &gasLiftDependencies, const thermal::ThermalState &thermalDependencies) {
    assignCellTimeSteps(state);
    if (state.input.lingas > 0) {
        for (int i = 0; i <= state.lastGasCell; i++)
            state.gasCells[i].FeiticoDoTempo();
        if (state.input.descarga == 1) {
            state.interfaceCell = state.initialInterfaceCell;
            state.interfaceTimeStep = state.initialInterfaceTimeStep;
            state.interfaceVelocity = state.initialInterfaceVelocity;
        }
        gaslift::advanceGasSubStep(gasLiftDependencies, thermalDependencies);
    }
    restartFractionEvolutionSub(state);
    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].bet = state.cells[i].betini;
        state.cells[i].razPig = state.cells[i].razPigini;
        state.cells[i].betPigE = state.cells[i].betPigEini;
        state.cells[i].betPigD = state.cells[i].betPigDini;
    }
}

void evolveFractions(const TransientStepState &state, double alfrev, [[maybe_unused]] double betrev, [[maybe_unused]] int ciclo) {

#pragma omp parallel for num_threads((*state.globals).solverThreadCount)
    for (int i = 0; i < state.lastCell; i++) {
        state.cells[i].avancalf(state.restart, state.lastCell);
    }
    if (state.lastCell >= 0) {
        updateOutletVoidFraction(state, state.lastCell, alfrev);
    }
#pragma omp parallel for num_threads((*state.globals).solverThreadCount)
    for (int i = 0; i < state.lastCell; i++) {
        state.cells[i].avancbet(state.restart, state.lastCell);
    }
    if (state.lastCell >= 0) {
        updateOutletComplementaryFraction(state, state.lastCell);
    }
#pragma omp parallel for num_threads((*state.globals).solverThreadCount)
    for (int i = 0; i <= state.lastCell; i++) {
        if (i < state.lastCell && state.cells[i].estadoPig == 1) {
            state.cells[i].avancPig(state.restart);
            state.cells[i].avancalfPig();
            state.cells[i].avancbetPig();
        } else {
            state.cells[i].alfPigE = state.cells[i].alf;
            state.cells[i].betPigE = state.cells[i].bet;
            state.cells[i].alfPigD = state.cells[i].alf;
            state.cells[i].betPigD = state.cells[i].bet;
        }
    }
    for (int i = 0; i <= state.lastCell; i++) {
        if (state.cells[i].correrGlobHol == 1) {
            if (state.cells[i].reiniciaAlf < 0 || state.cells[i].reiniciaBet < 0 || state.cells[i].reiniciaPig < 0)
                state.restart = -1;
            state.cells[i].reiniciaAlf = 0;
            state.cells[i].reiniciaBet = 0;
            state.cells[i].reiniciaPig = 0;
        }
    }
}

void updatePig(const TransientStepState &state) {
    for (int i = 1; i <= state.lastCell; i++) {
        state.cells[i].velPigini = state.cells[i].velPig;
        state.cells[i].estadoPigini = state.cells[i].estadoPig;
        state.cells[i].indpigini = state.cells[i].indpig;
    }
    for (int i = 1; i <= state.lastCell; i++) {
        if (i < state.lastCell && state.cells[i].estadoPig == 1) {
            if (state.cells[i].velPig >= 0) {
                if (state.cells[i].razPig >= 1. - (*state.globals).smallValueTolerance) {
                    state.cells[i].estadoPig = 0;
                    state.cells[i + 1].estadoPig = 1;

                    state.cells[i].razPig = 0.;
                    state.cells[i + 1].razPig = 0.;
                    state.cells[i + 1].indpig = state.cells[i].indpig;
                    state.cells[i].indpig = -1;
                    state.cells[i + 1].velPig = state.cells[i].velPig;
                    state.cells[i + 1].alfPigE = state.cells[i].alf;
                    state.cells[i + 1].betPigE = state.cells[i].bet;
                }
            } else {
                if (state.cells[i].razPig <= (*state.globals).smallValueTolerance) {
                    state.cells[i].estadoPig = 0;
                    state.cells[i - 1].estadoPig = 1;
                    state.cells[i].razPig = 0.;
                    state.cells[i - 1].razPig = 1.;
                    state.cells[i - 1].indpig = state.cells[i].indpig;
                    state.cells[i].indpig = -1;
                    state.cells[i - 1].velPig = state.cells[i].velPig;
                    state.cells[i - 1].alfPigD = state.cells[i].alf;
                    state.cells[i - 1].betPigD = state.cells[i].bet;
                }
            }
        }
    }
    preparePigFaceFractions(state);

}

void solvePressureVolumeCoupling(const TransientStepState &state, int vexpli, int ciclo) {
#pragma omp parallel for default(none) shared(state) firstprivate(ciclo, vexpli) schedule(static) num_threads((*state.globals).solverThreadCount)
    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].GeraLocal(state.outletPressure, state.surfaceChokeMassFlag, state.lastCell, state.input.master1.razareaativ, state.inletPressure, state.inletTemperature, state.inletQuality, state.inletComplementaryFraction, ciclo,
                            state.fullModel, state.endNode, state.input.corrigeContSep, state.surfaceChoke.AreaGarg, vexpli);
        for (int j = 0; j < 6; j++) {
            state.productionMatrix[2 * i][j - 3] = state.cells[i].local[0][j];
            state.productionMatrix[2 * i + 1][j - 3] = state.cells[i].local[1][j];
        }
        state.productionSolution[2 * i] = state.cells[i].TL[0];
        state.productionSolution[2 * i + 1] = state.cells[i].TL[1];
    }

    state.productionMatrix.GaussElimPP(state.productionSolution);
}

void refreshFluidMiniTable(const TransientStepState &state) {
    generateFluidMiniTables(state);
    for (int i = 0; i < state.lastCell; i++) {

        double pres = state.cells[i].pres;
        double temp = state.cells[i].temp;

        double betIV = state.cells[i].bet;
        double solutionGasRatioInSitu = state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp);
        double oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp, solutionGasRatioInSitu);
        double waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);
        double bswV = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double rhoOVol = state.cells[i].flui.MasEspoleo(state.cells[i].pres, state.cells[i].temp);
        double rhoWVol = state.cells[i].flui.MasEspAgua(state.cells[i].pres, state.cells[i].temp);
        double titVol = (1 - bswV) * rhoOVol / ((1 - bswV) * rhoOVol + bswV * rhoWVol);

        (void)state.cells[i].flui.MasEspGas(state.cells[i].pres, state.cells[i].temp);
        (void)state.cells[i].flui.Zdran(state.cells[i].pres, state.cells[i].temp);
        (void)state.cells[i].flui.FracMassHidra(state.cells[i].pres, state.cells[i].temp);
        (void)state.cells[i].flui.PB(state.cells[i].pres, state.cells[i].temp);
        state.cells[i].nMol = (state.cells[i].flui.MasEspLiq(pres, temp) * (1. - state.cells[i].alf) * (1. - betIV) * titVol +
                          state.cells[i].rgC * state.cells[i].alf) *
                         state.cells[i].duto.area * state.cells[i].dx / state.cells[i].flui.Pmol;
        state.cells[i].nMolIni = state.cells[i].nMol;
    }
}

void refreshInletCondition(const TransientStepState &state) {
    if (state.input.ConContEntrada != 1)
        return;

    if (state.input.tipoFluido == 0 && state.input.flashCompleto == 2) {
        refreshCompositionalInletQuality(state);
    } else if (state.input.tipoFluido == 1) {
        refreshGasInletFractions(state);
    }
}

void solveTransientStep(const TransientSolveState &state, const TransientDependencies &dependencies,
                        double reverseQuality, double reverseVoidFraction, double reverseLiquidFraction,
                        int networkIndex, const ProFlu *reverseFluid) {
    const bool screenOutputEnabled = state.step.input.saidaTela == 1;
    const auto begin = screenOutputEnabled ? chrono::steady_clock::now() : chrono::steady_clock::time_point{};
    double maximumUnloadingFlowRate = 0.;
    if ((*state.step.globals).networkMode == 0)
        maximumUnloadingFlowRate = advanceStandaloneTransientStep(
            state, dependencies, reverseQuality, reverseVoidFraction, reverseLiquidFraction);

    finalizeTransientCellState(state, dependencies, reverseFluid);
    updateTransientMovingMeans(state);
    finalizeBuriedPipeConduction(state.step, dependencies.thermal.poisson2DCellIndices);
    thermal::finalizeAxisymmetricThermalField(dependencies.thermal, state.step.branchIndex);

    constexpr int maximumProgressReportInterval = 1000;
    int trendWriteOrder = 0;
    if ((fabs(state.step.input.logevento[state.logCounter].instante - (*state.step.globals).simulationTime) < state.step.timeStep) ||
        progressReportIsDue(state, maximumProgressReportInterval) ||
        (*state.step.globals).simulationTime < 1e-15) {
        state.printPassCount++;
        trendWriteOrder = 1;
    }

    if ((*state.step.globals).networkMode != 0)
        (*state.step.globals).simulationTime = (*state.step.globals).networkSimulationTime;
    writeProfiles(state, networkIndex);
    writeTrends(state, dependencies, trendWriteOrder, maximumUnloadingFlowRate, networkIndex);
    const auto end = screenOutputEnabled ? chrono::steady_clock::now() : chrono::steady_clock::time_point{};
    (*state.step.globals).timeStepIndex = state.step.stepIndex;
    writeScreenOutput(state, begin, end);

    const int eventCount = state.step.input.logevento.size();
    writeEventLog(state, eventCount);
    writeProgressReport(state, maximumProgressReportInterval);

    reportAcceptedArtificialGas(state.step);
    state.step.stepIndex++;
    state.printCounter++;
    if ((*state.step.globals).networkMode == 0)
        (*state.step.globals).simulationTime += state.step.timeStep;
}

void fillFluidMiniTable(const TransientStepState &state, ProFlu &flui) {
    ProFlu fluC;
    fluC = flui;
    fluC.atualizaPropCompStandard();
    fillMiniTableCornersAtMinPressure(state, fluC, flui);
    fillMiniTableCornersAtMaxPressure(state, fluC, flui);

    std::pair<double, int> titVec[4];

    for(int j=0;j<2;j++){
        for(int k=0;k<2;k++){
            titVec[2*j+k]={flui.miniTabDin.tit[j][k],2*j+k};
        }
    }
    std::sort(titVec, titVec + 4);
    supplyAbsentGasProperties(flui, titVec);
    supplyAbsentLiquidProperties(flui, titVec);
}

void generateFluidMiniTables(const TransientStepState &state) {
    (*state.globals).transientPropertyMode = 0;
#pragma omp parallel for num_threads((*state.globals).solverThreadCount)
    for (int i = 0; i <= state.lastCell; i++) {
        double delp;
        double delt;
        delp = 0.5 * state.cells[i].pres;
        if (delp > state.input.miniTabDp)
            delp = state.input.miniTabDp;
        if (delp < 5) {
            state.cells[i].flui.miniTabDin.pmax = state.cells[i].pres + 5.;
            state.cells[i].flui.miniTabDin.pmin = state.cells[i].pres - delp;
            if (state.cells[i].flui.miniTabDin.pmin < 0.9)
                state.cells[i].flui.miniTabDin.pmin = 0.9;
        } else {
            state.cells[i].flui.miniTabDin.pmax = state.cells[i].pres + delp;
            state.cells[i].flui.miniTabDin.pmin = state.cells[i].pres - delp;
        }
        delt = state.input.miniTabDt;
        state.cells[i].flui.miniTabDin.tmax = state.cells[i].temp + delt;
        state.cells[i].flui.miniTabDin.tmin = state.cells[i].temp - delt;
        if(state.input.miniTabAtraso > 0)fillFluidMiniTable(state, state.cells[i].flui);
        if (state.cells[i].acsr.tipo == kAccessoryGasInjection) {
            refreshAccessoryFluidMiniTable(state, i, state.cells[i].acsr.injg.FluidoPro);
        } else if (state.cells[i].acsr.tipo == kAccessoryLiquidInjection) {
            refreshAccessoryFluidMiniTable(state, i, state.cells[i].acsr.injl.FluidoPro);
        } else if (state.cells[i].acsr.tipo == kAccessoryInflowPerformance) {
            refreshAccessoryFluidMiniTable(state, i, state.cells[i].acsr.ipr.FluidoPro);
        } else if (state.cells[i].acsr.tipo == kAccessoryRadialPorous) {
            if(state.input.miniTabAtraso > 0)state.cells[i].acsr.radialPoro.geraMiniTabFlu();
        } else if (state.cells[i].acsr.tipo == kAccessoryPorous2D) {
            if(state.input.miniTabAtraso > 0)state.cells[i].acsr.poroso2D.geraMiniTabFlu();
        } else if (state.cells[i].acsr.tipo == kAccessoryLeak) {
            refreshAccessoryFluidMiniTable(state, i, state.cells[i].acsr.fontechk.fluidoP);
        } else if (state.cells[i].acsr.tipo == kAccessoryMultipleSource) {
            refreshAccessoryFluidMiniTable(state, i, state.cells[i].acsr.injm.FluidoPro);
        }
    }
    (*state.globals).transientPropertyMode = 1;
}

void storePreviousSources(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].fontemassLRini = state.cells[i].fontemassLR;
        state.cells[i].fontemassCRini = state.cells[i].fontemassCR;
        state.cells[i].fontemassGRini = state.cells[i].fontemassGR;
    }
}

void evaluateWaxDeposition(const TransientStepState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        if (i < state.lastCell)
            state.cells[i].WaxDeposition(state.input.detalParafina, state.lastCell);
        else {
            if (state.surfaceChokeMassFlag == 0 || state.cells[state.lastCell].Mliqini > 0) {
                double cpDep = state.cells[i - 1].duto.cp[0];
                double kDep = state.cells[i - 1].duto.cond[0];
                double rhoDep = state.cells[i - 1].duto.rhoC[0];
                state.cells[i].deltaPar = state.cells[i - 1].deltaPar;
                if (state.cells[i].parafinado == 0 && state.cells[i].deltaPar > 0.) {
                    state.cells[i].duto.atualizaCamada(state.cells[i].deltaPar, state.input.detalParafina.rug, cpDep, kDep, rhoDep);
                    state.cells[i].calor.atualiza(state.cells[i].duto, 1);
                    state.cells[i].parafinado = 1;
                } else if (state.cells[i].deltaPar > 0.) {
                    state.cells[i].duto.atualizaCamada2(state.cells[i].deltaPar, cpDep, kDep, rhoDep);
                    state.cells[i].calor.atualiza2(state.cells[i].duto);
                }
            } else
                state.cells[i].WaxDeposition(state.input.detalParafina, state.lastCell);
        }
    }
}

void storePreviousFractionsAndMovePigs(const TransientStepState &state) {

    state.cells[0].alfini = state.cells[0].alf;
    state.cells[0].betini = state.cells[0].bet;

    if (state.input.ConContEntrada == 0) {
        state.cells[0].alfLini = state.cells[0].alfini;
        state.cells[0].betLini = state.cells[0].betini;
    } else {
        state.cells[0].alfLini = state.inletQuality;
        state.cells[0].betLini = state.inletComplementaryFraction;
    }

    state.previousMovingPigCount = state.movingPigCount;
    state.movingPigCount = 0;

    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].estadoPigini = state.cells[i].estadoPig;
        state.cells[i].alfini = state.cells[i].alf;
        if (i > 0)
            state.cells[i - 1].alfRini = state.cells[i].alfini;
        if (i < state.lastCell)
            state.cells[i + 1].alfLini = state.cells[i].alfini;
        state.cells[i].betini = state.cells[i].bet;
        if (i > 0)
            state.cells[i - 1].betRini = state.cells[i].betini;
        if (i < state.lastCell)
            state.cells[i + 1].betLini = state.cells[i].betini;

        state.cells[i].alfPigDini = state.cells[i].alfPigD;
        state.cells[i].betPigDini = state.cells[i].betPigD;
        state.cells[i].alfPigEini = state.cells[i].alfPigE;
        state.cells[i].betPigEini = state.cells[i].betPigE;

        state.cells[i].alfPigERini = state.cells[i].alfPigER;
        state.cells[i].betIini = state.cells[i].betI;
        state.cells[i].betRIini = state.cells[i].betRI;
        state.cells[i].betLIini = state.cells[i].betLI;

        if (i > 0)
            state.cells[i - 1].alfPigER = state.cells[i].alfPigE;
        if (i == state.lastCell)
            state.cells[i].alfPigER = state.cells[i].alf;
        state.cells[i].DelPig = 0.;
        state.cells[i].RazAreaPig = 0.;
        state.cells[i].cdpig = 1.;
        if (state.cells[i].estadoPig == 1) {
            updateActivePigState(state, i);
        }
        state.cells[i].razPigini = state.cells[i].razPig;
    }
    state.cells[state.lastCell].alfRini = state.cells[state.lastCell].alfini;
    state.cells[state.lastCell].betRini = state.cells[state.lastCell].betini;
}

void cacheCellAndFaceDensities(const TransientStepState &state) {

    state.cells[0].rpC = state.cells[0].flui.MasEspLiq(state.cells[0].pres, state.cells[0].temp);
    state.cells[0].rgC = state.cells[0].flui.MasEspGas(state.cells[0].pres, state.cells[0].temp);
    state.cells[0].rcC = state.cells[0].fluicol.MasEspFlu(state.cells[0].pres, state.cells[0].temp);
    state.cells[0].rpL = state.cells[0].rpC;
    state.cells[0].rgL = state.cells[0].rgC;
    state.cells[0].rcL = state.cells[0].rcC;

    state.cells[0].rpCi = state.cells[0].rpC;
    state.cells[0].rgCi = state.cells[0].rgC;
    state.cells[0].rcCi = state.cells[0].rcC;
    state.cells[0].rpLi = state.cells[0].rpCi;
    state.cells[0].rgLi = state.cells[0].rgCi;
    state.cells[0].rcLi = state.cells[0].rcCi;

    state.cells[0].mipC = state.cells[0].flui.ViscOleo(state.cells[0].pres, state.cells[0].temp);
    state.cells[0].migC = state.cells[0].flui.ViscGas(state.cells[0].pres, state.cells[0].temp);
    state.cells[0].micC = state.cells[0].fluicol.VisFlu(state.cells[0].pres, state.cells[0].temp);

#pragma omp parallel for num_threads((*state.globals).solverThreadCount)
    for (int i = 1; i <= state.lastCell; i++) {
        double pressure;
        double temperature;
        pressure = state.cells[i].pres;
        temperature = state.cells[i].temp;
        state.cells[i].rpC = state.cells[i].flui.MasEspLiq(pressure, temperature);
        state.cells[i].rgC = state.cells[i].flui.MasEspGas(pressure, temperature);
        state.cells[i].rcC = state.cells[i].fluicol.MasEspFlu(pressure, temperature);

        state.cells[i].mipC = state.cells[i].flui.ViscOleo(pressure, temperature);
        state.cells[i].migC = state.cells[i].flui.ViscGas(pressure, temperature);
        state.cells[i].micC = state.cells[i].fluicol.VisFlu(pressure, temperature);

        double tmed = state.cells[i - 1].temp;
        if (state.cells[i].VTemper < 0.)
            tmed = state.cells[i].temp;
        const ProFlu &upwindFluid = state.cells[i].QL < 0.
                                       ? state.cells[i].flui : state.cells[i - 1].flui;
        state.cells[i].rpCi = upwindFluid.MasEspLiq(state.cells[i].presaux, tmed);
        state.cells[i].rgCi = upwindFluid.MasEspGas(state.cells[i].presaux, tmed);
        state.cells[i].rcCi = state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
    }
    for (int i = 1; i <= state.lastCell; i++) {

        state.cells[i - 1].rpR = state.cells[i].rpC;
        state.cells[i - 1].rgR = state.cells[i].rgC;
        state.cells[i - 1].rcR = state.cells[i].rcC;
        state.cells[i].rpL = state.cells[i - 1].rpC;
        state.cells[i].rgL = state.cells[i - 1].rgC;
        state.cells[i].rcL = state.cells[i - 1].rcC;

        state.cells[i - 1].mipR = state.cells[i].mipC;
        state.cells[i - 1].migR = state.cells[i].migC;
        state.cells[i - 1].micR = state.cells[i].micC;

        state.cells[i - 1].rpRi = state.cells[i].rpCi;
        state.cells[i - 1].rgRi = state.cells[i].rgCi;
        state.cells[i - 1].rcRi = state.cells[i].rcCi;
        state.cells[i].rpLi = state.cells[i - 1].rpCi;
        state.cells[i].rgLi = state.cells[i - 1].rgCi;
        state.cells[i].rcLi = state.cells[i - 1].rcCi;
    }
    state.cells[state.lastCell].rpR = state.cells[state.lastCell].rpC;
    state.cells[state.lastCell].rgR = state.cells[state.lastCell].rgC;
    state.cells[state.lastCell].rcR = state.cells[state.lastCell].rcC;

    state.cells[state.lastCell].mipR = state.cells[state.lastCell].mipC;
    state.cells[state.lastCell].migR = state.cells[state.lastCell].migC;
    state.cells[state.lastCell].micR = state.cells[state.lastCell].micC;
}

}  // namespace sisprod::transient
