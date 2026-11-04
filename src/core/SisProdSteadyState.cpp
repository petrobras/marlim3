#include "SisProdSteadyStateSearch.h"
#include "DriftFluxClosure.h"
#include "SisProdSources.h"
#include "SisProdGasLift.h"
#include "SisProdThermal.h"
#include "SisProdFluidTracking.h"
#include "SisProdSteadyState.h"
#include "SisProdSteadyStateInternal.h"
#include "SisProdPressureMarchInternal.h"

#include "GradientCorrelations.h"
#include "Leitura.h"
#include "SisProdConstants.h"
#include "celula3.h"
#include "celulaGas.h"
#include "chokegas.h"
#include "SimulationContext.h"
#include "UnitConversions.h"

#include <cmath>
#include <math.h>
#include <numbers>

namespace sisprod::steady {

namespace {

/// Publishes cell complementary-liquid fraction to face, pig and initial transient values.
void publishSteadyComplementaryFraction(const SteadyStateState &state, int i) {
    if (i < state.lastCell) {
        state.cells[i + 1].betL = state.cells[i].bet;
        state.cells[i + 1].betLini = state.cells[i].bet;
        state.cells[i + 1].betLI = state.cells[i].bet;
    }
    state.cells[i].betini = state.cells[i].bet;
    state.cells[i - 1].betR = state.cells[i].bet;
    state.cells[i - 1].betRini = state.cells[i].bet;
    state.cells[i].betPigD = state.cells[i].bet;
    state.cells[i].betPigDini = state.cells[i].bet;
    state.cells[i].betPigE = state.cells[i].bet;
    state.cells[i].betPigEini = state.cells[i].bet;
    state.cells[i].betI = state.cells[i].bet;
    state.cells[i - 1].betRI = state.cells[i].bet;
}

/// Publishes cell void fraction to face, pig and initial transient values.
void publishSteadyVoidFraction(const SteadyStateState &state, int i) {
    state.cells[i].alfini = state.cells[i].alf;
    state.cells[i - 1].alfR = state.cells[i].alf;
    state.cells[i - 1].alfRini = state.cells[i].alf;
    if (i < state.lastCell)
        state.cells[i + 1].alfL = state.cells[i].alf;
    if (i < state.lastCell)
        state.cells[i + 1].alfLini = state.cells[i].alf;
    state.cells[i].alfPigD = state.cells[i].alf;
    state.cells[i].alfPigDini = state.cells[i].alf;
    state.cells[i].alfPigE = state.cells[i].alf;
    state.cells[i].alfPigEini = state.cells[i].alf;
}

/// Resolves the phase fractions of cell i for the two reverse mass marches.
///
/// Four regimes -- no flow, liquid only, gas only, two-phase -- and the drift
/// closure the two-phase arm needs, shared by advanceReverseSteadyMass and
/// advanceReverseCompositionalSteadyMass.
///
/// After publication, the reverse black-oil march zeroes the complementary-liquid
/// residence time; the reverse compositional march propagates it from the upstream cell.
void applyReverseSteadyPhaseFractions(const SteadyStateState &state, const SteadyDependencies &dependencies, int i, double rhol, double rhog) {
    if (fabs(state.cells[i].QG + state.cells[i].QL) < (*state.globals).smallValueTolerance) {
        if (state.input.tipoFluido == 1) {
            state.cells[i].alf = 0.;
        } else {
            state.cells[i].alf = 1.;
        }
        publishSteadyVoidFraction(state, i);
        publishSteadyComplementaryFraction(state, i);
    } else if (fabs(state.cells[i].QG) < (*state.globals).smallValueTolerance) { // liquid only:
        state.cells[i].alf = 0.;
        publishSteadyVoidFraction(state, i);
        publishSteadyComplementaryFraction(state, i);
    } else if (fabs(state.cells[i].QL) < (*state.globals).smallValueTolerance * 1e-6) { // gas only:
        state.cells[i].alf = 1.;
        publishSteadyVoidFraction(state, i);
        state.cells[i].c0 = 1.;
        state.cells[i].ud = 0.;
        publishSteadyComplementaryFraction(state, i);
    } else { // two-phase
        double c0 = 1.;
        double ud = 0.;
        if (fabs(state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-6) {
            if (state.steadyIteration == 0) { // first estimate, first iteration
                // the no-slip void fraction is used, since the correlation that gives the
                // void fraction depends on the void fraction itself
                if ((fabs(state.cells[i].QG) + fabs(state.cells[i].QL)) > (*state.globals).smallValueTolerance) {
                    state.cells[i].alf = fabs(state.cells[i].QG) /
                                    (fabs(state.cells[i].QG) + fabs(state.cells[i].QL));
                    driftflux::coefficient::steadyState(dependencies.closure, i, c0, ud);
                } else
                    state.cells[i].alf = 0.;
                publishSteadyVoidFraction(state, i);
            }
            if (fabs(rhog) / rhol > 0.9) {
                c0 = 1.;
                ud = 0.;
            }

            // at steady state, the void fraction comes from the slip relations,
            // so this is where Co and Ud are obtained:
            else if (fabs(state.cells[i].QG) > (*state.globals).smallValueTolerance && fabs(state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-6)
                driftflux::coefficient::steadyState(dependencies.closure, i, c0, ud);
            state.cells[i].c0 = c0;
            state.cells[i].ud = ud;
            double area = state.cells[i].duto.area;
            if (fabs(state.cells[i].QG + state.cells[i].QL) > (*state.globals).smallValueTolerance) {
                // void fraction with slip:
                state.cells[i].alf = state.cells[i].QG / (c0 * (state.cells[i].QG + state.cells[i].QL) + ud * area);
                double alfHomo = state.cells[i].QG / (state.cells[i].QG + state.cells[i].QL);
                if (state.cells[i].alf > 1. - 1e-15 || state.cells[i].alf < 1e-15)
                    state.cells[i].alf = alfHomo;

            } else
                state.cells[i].alf = 0.;
            if (state.cells[i].alf > (1 - (*state.globals).smallValueTolerance))
                state.cells[i].alf = state.cells[i].QG / (state.cells[i].QG + state.cells[i].QL);
        } else {
            state.cells[i].alf = 1.;
        }
        if (state.cells[i].alf < 0.)
            state.cells[i].alf = 0.;
        else if (state.cells[i].alf > 1.)
            state.cells[i].alf = 1.;
        // updates the volume fractions of cell i kept in
        // other cells, including the "previous time" values, which do not
        // matter to the steady problem but do if the steady result
        // starts the transient solution:
        publishSteadyVoidFraction(state, i);
        publishSteadyComplementaryFraction(state, i);
    }
}

/// Refreshes the compositional flash of cell i, seeded from the nearest cell
/// upstream or downstream that has a usable calculated beta. Shared by
/// advanceReverseCompositionalSteadyMass and advanceCompositionalSteadyMass.
void refreshCompositionalFlashFromNeighbour(const SteadyStateState &state, int i, double pmed, double tmed) {
    if (((state.steadyIteration == 0 && i <= 1 && state.searchOrigin == 0) && ((*state.globals).networkMode == 0 || (*state.globals).networkIteration == 1)) && state.input.tabelaDinamica == 0) {
        state.cells[i].flui.atualizaPropComp(pmed, tmed, -1, NULL, NULL, state.input.pocinjec);
    } else if (state.input.tabelaDinamica == 0) {
        if ((state.steadyIteration == 0 && state.searchOrigin == 0) && ((*state.globals).networkMode == 0 || (*state.globals).networkIteration == 1)) {
            int veriI = i - 1;
            while ((state.cells[veriI].flui.dCalculatedBeta > 1 - (0.0 + 1e-15) || state.cells[veriI].flui.dCalculatedBeta < (0.0 + 1e-15)) && (veriI > 0))
                veriI--;
            if ((veriI == 0) && (state.cells[veriI].flui.dCalculatedBeta > 1 - (0.0 + 1e-15) || state.cells[veriI].flui.dCalculatedBeta < (0.0 + 1e-15)))
                state.cells[i].flui.atualizaPropComp(pmed, tmed, -1, NULL, NULL, state.input.pocinjec);
            else
                state.cells[i].flui.atualizaPropComp(pmed, tmed, state.cells[veriI].flui.dCalculatedBeta, state.cells[veriI].flui.oCalculatedLiqComposition,
                                                state.cells[veriI].flui.oCalculatedVapComposition, state.input.pocinjec);
        } else {
            int veriI;
            int veriIinf = i;
            while ((state.cells[veriIinf].flui.dCalculatedBeta > 1 - 1e-15 ||
                    state.cells[veriIinf].flui.dCalculatedBeta < 1e-15) &&
                   veriIinf > 0)
                veriIinf--;
            int veriIsup = i;
            while ((state.cells[veriIsup].flui.dCalculatedBeta > 1 - 1e-15 ||
                    state.cells[veriIsup].flui.dCalculatedBeta < 1e-15) &&
                   veriIsup < state.lastCell)
                veriIsup++;
            if ((state.cells[veriIinf].flui.dCalculatedBeta > 1 - 1e-15 ||
                 state.cells[veriIinf].flui.dCalculatedBeta < 1e-15) &&
                (state.cells[veriIsup].flui.dCalculatedBeta < 1. &&
                 state.cells[veriIsup].flui.dCalculatedBeta > 0.))
                veriI = veriIsup;
            else if ((state.cells[veriIsup].flui.dCalculatedBeta > 1 - 1e-15 || state.cells[veriIsup].flui.dCalculatedBeta < 1e-15) &&
                     (state.cells[veriIinf].flui.dCalculatedBeta < 1. && state.cells[veriIinf].flui.dCalculatedBeta > 0.))
                veriI = veriIinf;
            else if (fabs(i - veriIsup) < fabs(i - veriIinf))
                veriI = veriIsup;
            else
                veriI = veriIinf;
            if ((state.cells[veriI].flui.dCalculatedBeta > 1 - (0.0 + 1e-15) || state.cells[veriI].flui.dCalculatedBeta < (0.0 + 1e-15)))
                state.cells[i].flui.atualizaPropComp(pmed, tmed, -1, NULL, NULL, state.input.pocinjec);
            else
                state.cells[i].flui.atualizaPropComp(pmed, tmed, state.cells[veriI].flui.dCalculatedBeta, state.cells[veriI].flui.oCalculatedLiqComposition,
                                                state.cells[veriI].flui.oCalculatedVapComposition, state.input.pocinjec);
        }
    }
}

/// Two-phase branch of the compositional phase-regime chain: one arm of the
/// four-way chain in applyCompositionalPhaseRegime, with no other caller.
void applyCompositionalTwoPhaseRegime(const SteadyStateState &state, const SteadyDependencies &dependencies, int i, double rhol, double rhog) {
    double c0 = 1.;
    double ud = 0.;
    if (fabs(state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-5) {
        if (state.steadyIteration == 0) { // first estimate, first iteration
            // the no-slip void fraction is used, since the correlation that gives the
            // void fraction depends on the void fraction itself
            if ((fabs(state.cells[i].QG) + fabs(state.cells[i].QL)) > (*state.globals).smallValueTolerance) {
                if (state.convergenceMonitor > 0.01)
                    state.cells[i].alf = fabs(state.cells[i].QG) /
                                    (fabs(state.cells[i].QG) + fabs(state.cells[i].QL));
                driftflux::coefficient::steadyState(dependencies.closure, i, c0, ud);
            } else
                state.cells[i].alf = 0.;
            publishSteadyVoidFraction(state, i);
        }
        if (state.input.tipoModeloDrift == 1) {
            if (fabs(rhog) / rhol > 0.9) {
                c0 = 1.;
                ud = 0.;
            }
            // at steady state, the void fraction comes from the slip relations,
            // so this is where Co and Ud are obtained:
            else if (fabs(state.cells[i].QG) > (*state.globals).smallValueTolerance * 1e-5 && fabs(state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-5)
                driftflux::coefficient::steadyState(dependencies.closure, i, c0, ud);
            state.cells[i].c0 = c0;
            state.cells[i].ud = ud;
            double area = state.cells[i].duto.area;
            if (fabs(state.cells[i].QG + state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-5) {
                // void fraction with slip:
                state.cells[i].alf = state.cells[i].QG / (c0 * (state.cells[i].QG + state.cells[i].QL) + ud * area);
                double alfHomo = state.cells[i].QG / (state.cells[i].QG + state.cells[i].QL);
                if (state.cells[i].alf > 1. - 1e-15 || state.cells[i].alf < 1e-15)
                    state.cells[i].alf = alfHomo;
            } else
                state.cells[i].alf = 0.;
            if (state.cells[i].alf > (1 - (*state.globals).smallValueTolerance) && fabs(state.cells[i].QG + state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-5)
                state.cells[i].alf = state.cells[i].QG / (state.cells[i].QG + state.cells[i].QL);
            else if (state.cells[i].alf > (1 - (*state.globals).smallValueTolerance))
                state.cells[i].alf = 1.;
        } else {
            state.cells[i].alf = 1.;
        }
    } else {
        double holdup;
        double frictionGrad;
        double gravityGrad;
        double totalGrad;
        double reynolds;
        unsigned char flowType;
        executarCorrelacao(state.cells, i, 0, state.input.AceleraConvergPerm,
                           state.cells[i - 1].correlacaoMR2,
                           holdup, frictionGrad, gravityGrad, totalGrad,
                           reynolds, flowType);
        if(flowType==1 || flowType==2)state.cells[i].arranjo=0;
        if(flowType==3)state.cells[i].arranjo=1;
        if(flowType==4)state.cells[i].arranjo=2;
        if(flowType==6)state.cells[i].arranjo=-1;
        if(flowType==5)state.cells[i].arranjo=-2;
        state.cells[i].alf = 1. - holdup;
    }
    if (state.cells[i].alf < 0.)
        state.cells[i].alf = 0.;
    else if (state.cells[i].alf > 1.)
        state.cells[i].alf = 1.;
    // updates the volume fractions of cell i kept in
    // other cells, including the "previous time" values, which do not
    // matter to the steady problem but do if the steady result
    // starts the transient solution:
    publishSteadyVoidFraction(state, i);
    publishSteadyComplementaryFraction(state, i);

}

/// Resolves the phase fractions of cell i for the compositional mass march.
///
/// Uses localtiny * 1e-5 for the no-flow and phase-presence thresholds.
void applyCompositionalPhaseRegime(const SteadyStateState &state, const SteadyDependencies &dependencies, int i, double rhol, double rhog) {
    if (fabs(state.cells[i].QG + state.cells[i].QL) < (*state.globals).smallValueTolerance * 1e-5) {
        if (state.input.tipoFluido == 1) {
            state.cells[i].alf = 0.;
        } else {
            state.cells[i].alf = 1.;
        }
        publishSteadyVoidFraction(state, i);
        publishSteadyComplementaryFraction(state, i);
    } else if (fabs(state.cells[i].QG) < (*state.globals).smallValueTolerance * 1e-5) { // liquid only:
        state.cells[i].alf = 0.;
        publishSteadyVoidFraction(state, i);
        publishSteadyComplementaryFraction(state, i);
    } else if (fabs(state.cells[i].QL) < (*state.globals).smallValueTolerance * 1e-5) { // gas only:
        state.cells[i].alf = 1.;
        publishSteadyVoidFraction(state, i);
        state.cells[i].c0 = 1.;
        state.cells[i].ud = 0.;
        publishSteadyComplementaryFraction(state, i);
    } else { // two-phase
        applyCompositionalTwoPhaseRegime(state, dependencies, i, rhol, rhog);
    }
}

/// Source quality and residence time used by compositional mixing and transport.
struct SteadySource {
    double tit;
    double residenceTime;
};

/// Reads the accessory fluid, quality and residence time for compositional mixing.
void readCompositionalSourceProperties(const SteadyStateState &state, int i, ProFlu &fluF, SteadySource &source) {
    double oilVolumeFactorSource = 1.;
    double waterVolumeFactorSource = 1.;
    double waterCutSource = 1.;
    double rhoOF;
    double rhoWF = 1000.;
    if (state.cells[i - 1].acsr.tipo == kAccessoryGasInjection) {
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.injg.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, state.cells[i - 1].flui.dCalculatedBeta,
                                                               state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.injg.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, -1, NULL, NULL, state.input.pocinjec);
        fluF = state.cells[i - 1].acsr.injg.FluidoPro;
        waterCutSource = 0.;
        source.tit = 1.;
        source.residenceTime = state.cells[i - 1].acsr.injg.fluidocol.TR;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryLiquidInjection) {
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.injl.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, state.cells[i - 1].flui.dCalculatedBeta,
                                                               state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.injl.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, -1, NULL, NULL, state.input.pocinjec);
        fluF = state.cells[i - 1].acsr.injl.FluidoPro;
        oilVolumeFactorSource = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorSource = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutSource = fluF.BSW * waterVolumeFactorSource / (oilVolumeFactorSource + waterVolumeFactorSource * fluF.BSW - fluF.BSW * oilVolumeFactorSource);
        rhoOF = fluF.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
        rhoWF = fluF.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
        source.tit = (1 - waterCutSource) * rhoOF / ((1 - waterCutSource) * rhoOF + waterCutSource * rhoWF);
        source.residenceTime = state.cells[i - 1].acsr.injl.fluidocol.TR;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.ipr.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, state.cells[i - 1].flui.dCalculatedBeta,
                                                              state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.ipr.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, -1, NULL, NULL, state.input.pocinjec);
        fluF = state.cells[i - 1].acsr.ipr.FluidoPro;
        oilVolumeFactorSource = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorSource = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutSource = fluF.BSW * waterVolumeFactorSource / (oilVolumeFactorSource + waterVolumeFactorSource * fluF.BSW - fluF.BSW * oilVolumeFactorSource);
        rhoOF = fluF.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
        rhoWF = fluF.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
        source.tit = (1 - waterCutSource) * rhoOF / ((1 - waterCutSource) * rhoOF + waterCutSource * rhoWF);
        source.residenceTime = 0.;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryMultipleSource) {
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.injm.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, state.cells[i - 1].flui.dCalculatedBeta,
                                                               state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.injm.FluidoPro.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, -1, NULL, NULL, state.input.pocinjec);
        fluF = state.cells[i - 1].acsr.injm.FluidoPro;
        oilVolumeFactorSource = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorSource = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutSource = fluF.BSW * waterVolumeFactorSource / (oilVolumeFactorSource + waterVolumeFactorSource * fluF.BSW - fluF.BSW * oilVolumeFactorSource);
        rhoOF = fluF.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
        rhoWF = fluF.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
        source.tit = (1 - waterCutSource) * rhoOF / ((1 - waterCutSource) * rhoOF + waterCutSource * rhoWF);
        source.residenceTime = state.cells[i - 1].acsr.injm.fluidocol.TR;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryLeak && state.cells[i - 1].acsr.fontechk.abertura > 1e-6) {

        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.fontechk.fluidoP.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, state.cells[i - 1].flui.dCalculatedBeta,
                                                                 state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.fontechk.fluidoP.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, -1, NULL, NULL, state.input.pocinjec);
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.fontechk.fluidoPamb.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, state.cells[i - 1].flui.dCalculatedBeta,
                                                                    state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.fontechk.fluidoPamb.atualizaPropComp(state.cells[i - 1].pres, state.cells[i - 1].temp, -1, NULL, NULL, state.input.pocinjec);
        if (state.cells[i - 1].acsr.fontechk.presT > state.cells[i - 1].acsr.fontechk.pamb) {
            fluF = state.cells[i - 1].acsr.fontechk.fluidoP;
        } else {
            fluF = state.cells[i - 1].acsr.fontechk.fluidoPamb;
        }
        oilVolumeFactorSource = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorSource = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutSource = fluF.BSW * waterVolumeFactorSource / (oilVolumeFactorSource + waterVolumeFactorSource * fluF.BSW - fluF.BSW * oilVolumeFactorSource);
        rhoOF = fluF.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
        rhoWF = fluF.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
        source.tit = (1 - waterCutSource) * rhoOF / ((1 - waterCutSource) * rhoOF + waterCutSource * rhoWF);
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryRadialPorous) {
        double tRes = state.cells[i - 1].acsr.radialPoro.tRes;
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.radialPoro.flup.atualizaPropComp(state.cells[i - 1].pres, tRes,
                                                                state.cells[i - 1].flui.dCalculatedBeta,
                                                                state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.radialPoro.flup.atualizaPropComp(state.cells[i - 1].pres, tRes, -1, NULL, NULL, state.input.pocinjec);
        fluF = state.cells[i - 1].acsr.radialPoro.flup;
        oilVolumeFactorSource = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorSource = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutSource = fluF.BSW * waterVolumeFactorSource / (oilVolumeFactorSource + waterVolumeFactorSource * fluF.BSW - fluF.BSW * oilVolumeFactorSource);
        rhoOF = fluF.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
        rhoWF = fluF.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
        source.tit = (1 - waterCutSource) * rhoOF / ((1 - waterCutSource) * rhoOF + waterCutSource * rhoWF);
        source.residenceTime = 0.;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryPorous2D) {
        double tRes = state.cells[i - 1].acsr.poroso2D.dados.transfer.tRes;
        if (i > 0 && (state.cells[i - 1].flui.dCalculatedBeta > 0. && state.cells[i - 1].flui.dCalculatedBeta < 1.))
            state.cells[i - 1].acsr.poroso2D.dados.flup.atualizaPropComp(state.cells[i - 1].pres, tRes,
                                                                    state.cells[i - 1].flui.dCalculatedBeta,
                                                                    state.cells[i - 1].flui.oCalculatedLiqComposition, state.cells[i - 1].flui.oCalculatedVapComposition, state.input.pocinjec);
        else
            state.cells[i - 1].acsr.poroso2D.dados.flup.atualizaPropComp(state.cells[i - 1].pres, tRes, -1, NULL, NULL, state.input.pocinjec);
        fluF = state.cells[i - 1].acsr.poroso2D.dados.flup;
        oilVolumeFactorSource = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorSource = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutSource = fluF.BSW * waterVolumeFactorSource / (oilVolumeFactorSource + waterVolumeFactorSource * fluF.BSW - fluF.BSW * oilVolumeFactorSource);
        rhoOF = fluF.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
        rhoWF = fluF.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
        source.tit = (1 - waterCutSource) * rhoOF / ((1 - waterCutSource) * rhoOF + waterCutSource * rhoWF);
        source.residenceTime = 0.;
    }
}

/// Cell i's left face in the compositional steady-state mass step: the upstream
/// cell's properties there, the mass flow rates that cross it (vazMas*) and that
/// its source adds (fonteMas*), and the complementary and hydrocarbon mass flow
/// rates.
struct SteadyFace {
    double tmed;
    double titV;
    double solutionGasRatio;
    double waterVolumeFactor;
    double oilVolumeFactor;
    double vazMasLiq;
    double vazMasGas;
    double fonteMasLiq;
    double fonteMasGas;
    double mComp;
    double mHidro;
};

/// Cell i-1 holds a source: the stream it passes to cell i mixes its own flow
/// with the source's, weighted by the standard oil and water rates before and
/// from the source, and the complementary-liquid fraction is re-evaluated.
void mixUpstreamSourceIntoCell(const SteadyStateState &state, SteadyFace &left, const SteadySource &source, int i, double temperatureHigh, double temperatureLow, ProFlu &fluF) {
    double waterCutCarried = state.cells[i - 1].FW;
    double rhoOV = state.cells[i - 1].flui.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhoWV = state.cells[i - 1].flui.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
    left.titV = (1 - waterCutCarried) * rhoOV / ((1 - waterCutCarried) * rhoOV + waterCutCarried * rhoWV);
    left.vazMasLiq *= left.titV;
    left.fonteMasLiq *= source.tit;
    if (state.input.tabelaDinamica == 0) {
        double pesoMolV = 0;
        double pesoMolF = 0;
        for (int j = 0; j < state.cells[i].flui.npseudo; j++) {
            pesoMolV += state.cells[i - 1].flui.masMol[j] * state.cells[i - 1].flui.fracMol[j];
            pesoMolF += fluF.masMol[j] * fluF.fracMol[j];
        }
        double vazMolV = (left.vazMasLiq + left.vazMasGas) / pesoMolV;
        double vazMolF = (left.fonteMasLiq + left.fonteMasGas) / pesoMolF;
        double razMolV = 1.;
        if (fabs(vazMolV + vazMolF) > 1e-15)
            razMolV = vazMolV / (vazMolV + vazMolF);
        if (vazMolF > 0.) {
            for (int j = 0; j < state.cells[i].flui.npseudo; j++) {
                state.cells[i].flui.fracMol[j] = razMolV * state.cells[i - 1].flui.fracMol[j] +
                                            (1. - razMolV) * fluF.fracMol[j];
            }
        } else {
            for (int j = 0; j < state.cells[i].flui.npseudo; j++)
                state.cells[i].flui.fracMol[j] = state.cells[i - 1].flui.fracMol[j];
        }
        state.cells[i].flui.Pmol = 0.;
        for (int j = 0; j < state.cells[i].flui.npseudo; j++)
            state.cells[i].flui.Pmol += state.cells[i].flui.fracMol[j] * state.cells[i].flui.masMol[j];

        state.cells[i].flui.atualizaPropCompStandard();
    }

    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boinjl;
    double bainjl;
    double fwinjl;
    if (fabs(left.fonteMasLiq + left.fonteMasGas) > 1e-15) {
        rholisF = fluF.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boinjl = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        bainjl = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwinjl = fluF.BSW * bainjl / (boinjl + bainjl * fluF.BSW - fluF.BSW * boinjl);
    } else {
        rholisF = rholis;
        boinjl = left.oilVolumeFactor;
        bainjl = left.waterVolumeFactor;
        fwinjl = state.cells[i - 1].FW;
    }
    // standard oil flow rate before the source:
    double qostd1;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (left.oilVolumeFactor * rholis);
    else
        qostd1 = 0.;

    // standard oil flow rate of the source:
    double qostd2;
    if (fluF.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwinjl) / (boinjl * rholisF);
    else
        qostd2 = 0.;

    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * left.solutionGasRatio / left.oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;
    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance))
            state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * left.oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
        if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio && state.cells[i].flui.RGO < 1e6)
            state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    }

    double waterFlowRate1;
    if ((1. - state.cells[i - 1].flui.BSW) > 0)
        waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
    else
        waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
    double waterFlowRate2;
    if ((1. - fluF.BSW) > 0)
        waterFlowRate2 = qostd2 * fluF.BSW / (1. - fluF.BSW);
    else
        waterFlowRate2 = state.cells[i - 1].fontemassLR * fwinjl / (1000. * fluF.Denag);

    if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15 && fabs(waterFlowRate2 + qostd2) > 1e-15 && state.cells[i - 1].fontemassLR > 1e-15)
        state.cells[i].flui.BSW = (waterFlowRate1 + waterFlowRate2) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
    else
        state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;

    if (fabs(waterFlowRate1 + waterFlowRate2) > 1e-15 && state.cells[i - 1].fontemassLR > 1e-15)
        state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                waterFlowRate2 * fluF.Denag) /
                               (waterFlowRate1 + waterFlowRate2);
    else
        state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;

    if (fabs(qostd1 + qostd2) > 1e-15 && fabs(qostd2) > 1e-15 && state.cells[i - 1].fontemassLR > 1e-15) { // liquid flow rate > 0
        state.cells[i].flui.TempL = temperatureLow;
        state.cells[i].flui.TempH = temperatureHigh;
        state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * fluF.VisOM(temperatureLow)) / (qostd1 + qostd2);
        state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * fluF.VisOM(temperatureHigh)) / (qostd1 + qostd2);
    } else {
        state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
        state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
        state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
        state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
    }

    if (state.input.tipoFluido == 0) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the liquid source can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour /
                     (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        double oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, left.tmed);
        double waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, left.tmed);
        double waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu /
                     (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / fluF.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = state.cells[i - 1].fontemassCR / state.cells[i].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) /
                      (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / left.oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance * 1e-5))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, left.tmed);
        if (fabs(oilFlowRate + waterFlowRate + complementaryFlowRate) > 1e-15)
            state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
        else
            state.cells[i].bet = state.cells[i - 1].bet;
    } else {
        left.mHidro = state.cells[i].MC - left.mComp;
    }
}

/// Cell i-1 holds no source: the separator gas-oil ratio, BSW, API, gas density
/// and the rest pass unchanged into cell i, and the volume of light components is
/// refreshed, because the transient reads it.
void carryUpstreamCompositionIntoCell(const SteadyStateState &state, SteadyFace &left, int i) {
    // here variables such as the separator GOR, BSW, API, gas density and others do not change; they equal
    // the values of cell i-1
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // the volume of light components is updated here,
    // following the equations in the report; it is irrelevant to the steady state, but
    // must be computed, as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * left.solutionGasRatio / left.oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;

    state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;

    double waterCutCarried = state.cells[i - 1].FW;
    double rhoOV = state.cells[i - 1].flui.MasEspoleo(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhoWV = state.cells[i - 1].flui.MasEspAgua(state.cells[i - 1].pres, state.cells[i - 1].temp);
    left.titV = (1 - waterCutCarried) * rhoOV / ((1 - waterCutCarried) * rhoOV + waterCutCarried * rhoWV);

    for (int j = 0; j < state.cells[i].flui.npseudo; j++)
        state.cells[i].flui.fracMol[j] = state.cells[i - 1].flui.fracMol[j];
    if (i > 1) {
        state.cells[i].flui.iCalculatedStockTankThermodynamicCondition = state.cells[i - 1].flui.iCalculatedStockTankThermodynamicCondition;
        state.cells[i].flui.dStockTankVaporMassFraction = state.cells[i - 1].flui.dStockTankVaporMassFraction;
        state.cells[i].flui.dStockTankLiquidDensity = state.cells[i - 1].flui.dStockTankLiquidDensity;
        state.cells[i].flui.dStockTankVaporDensity = state.cells[i - 1].flui.dStockTankVaporDensity;

        if (state.cells[i].flui.dStockTankLiquidDensity > 0.01) {
            state.cells[i].flui.API = 141.5 / (state.cells[i].flui.dStockTankLiquidDensity / 1000.) - 131.5;
        } else
            state.cells[i].flui.API = 50;
        state.cells[i].flui.Deng = state.cells[i].flui.dStockTankVaporDensity / kAirDensityAtStandardConditions;
        state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
        state.cells[i].flui.IRGO = state.cells[i - 1].flui.IRGO;
    } else {
        state.cells[i].flui.atualizaPropCompStandard();
    }

    if (state.input.tipoFluido == 0) { // re-evaluates the complementary-liquid volume fraction;
        // even without a source it can change, because the produced liquid shrinks
        double oilVolumeFactorInSitu;
        double waterVolumeFactorInSitu;
        if (state.cells[i].flui.RGO < 1e6)
            oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, left.tmed);
        else
            oilVolumeFactorInSitu = 1.;
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, left.tmed);
        double waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double oilFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].FW) * (1 - state.cells[i - 1].bet) * oilVolumeFactorInSitu / left.oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance * 1e-5))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet);
        double complementaryFlowRate;
        complementaryFlowRate = state.cells[i - 1].QL * (state.cells[i - 1].bet) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, left.tmed);

        if (fabs(oilFlowRate + waterFlowRate + complementaryFlowRate) > 1e-15)
            state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
        else
            state.cells[i].bet = state.cells[i - 1].bet;
    } else {
        left.mHidro = state.cells[i].MC - left.mComp;
    }
}

// ------------------------------------------------- mass march helpers ----
//
// Nothing outside advanceSteadyMass calls these fourteen, so they have internal
// linkage rather than a line in the header.
//
// Nine are the arms of the accessory dispatch, one per kind attached to the
// upstream cell. Five close the march once the sources are known, chosen by
// which phases are actually flowing.
void applySteadyMassWithoutSource(const SteadyStateState &state, int i, double oilVolumeFactor, double solutionGasRatio, double tmed, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    // here variables such as the separator GOR, BSW, API, gas density and others do not change; they equal
    // the values of cell i-1
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // the volume of light components is updated here,
    // following the equations in the report; it is irrelevant to the steady state, but
    // must be computed, as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;

    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance))
            state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
        if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio && state.cells[i].flui.RGO < 1e7)
            state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    } else
        state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;

    state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;

    if (state.input.flashCompleto == 0) { // this switch loads into cell i
        // the variables that matter to the black-oil model
        state.cells[i].flui.Deng = state.cells[i - 1].flui.Deng;
        state.cells[i].flui.yco2 = state.cells[i - 1].flui.yco2;
        if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6) { // re-evaluates the complementary-liquid volume fraction;
        // even without a source it can change, because the produced liquid shrinks
        if (state.cells[i].flui.RGO < 1e6)
            oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        else
            oilVolumeFactorInSitu = 1.;
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double oilFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].FW) * (1 - state.cells[i - 1].bet) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet);
        double complementaryFlowRate;
        if (state.cells[i].flui.RGO < 1e7)
            complementaryFlowRate = state.cells[i - 1].QL * (state.cells[i - 1].bet) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        else
            complementaryFlowRate = 0.;

        if ((fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate)) > 0)
            state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
        else
            state.cells[i].bet = 0.;
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassDryGasInjection(const SteadyStateState &state, int i, double oilVolumeFactor, double solutionGasRatio, double tmed, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    waterCutInSitu = 0.;
    // standard oil flow rate
    double qostd;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    else
        qostd = 0.;
    // new standard gas flow rate, adding the gas source:
    double qgstd = qostd * state.cells[i - 1].flui.RGO + state.cells[i - 1].acsr.injg.QGas / units::kSecondsPerDay;
    double deng;
    double yco2;
    // balance that sets the gas density and the CO2 fraction from the gas source
    if (fabs(qgstd) > (*state.globals).smallValueTolerance && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) {
        deng = (qostd * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + state.cells[i - 1].acsr.injg.QGas * state.cells[i - 1].acsr.injg.FluidoPro.Deng / units::kSecondsPerDay) / qgstd;
        yco2 = (qostd * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + state.cells[i - 1].acsr.injg.QGas * state.cells[i - 1].acsr.injg.FluidoPro.yco2 / units::kSecondsPerDay) / qgstd;
    } else {
        deng = state.cells[i - 1].flui.Deng;
        yco2 = state.cells[i - 1].flui.yco2;
    }

    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;
    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance))
            state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
        if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio && state.cells[i].flui.RGO < 1e7)
            state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    }
    double rgo;
    // new separator GOR, without slip, from the standard gas and oil flow rates
    if (qostd > (*state.globals).smallValueTolerance)
        rgo = qgstd / qostd;
    else
        rgo = state.cells[i - 1].flui.RGO;
    state.cells[i].flui.RGO = rgo;

    state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW; // the BSW does not change with a gas source

    if (state.input.flashCompleto == 0) { // this switch loads into cell i
        // the variables that matter to the black-oil model
        state.cells[i].flui.Deng = deng;
        state.cells[i].flui.yco2 = yco2;
        if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }

    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 && state.cells[i - 1].alf < 1. - (*state.globals).smallValueTolerance * 1e-6 && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        // even without a liquid source it can change,
        // because the produced liquid shrinks
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double oilFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].FW) * (1 - state.cells[i - 1].bet) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet);
        double complementaryFlowRate = state.cells[i - 1].QL * (state.cells[i - 1].bet) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassWetGasInjection(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double waterVolumeFactor, double solutionGasRatio, double tmed, double &residenceTimeSource, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boinjl;
    double bainjl;
    double fwinjl;
    residenceTimeSource = state.cells[i - 1].acsr.injg.fluidocol.TR;
    if (fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) {
        rholisF = state.cells[i - 1].acsr.injg.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boinjl = state.cells[i - 1].acsr.injg.FluidoPro.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        bainjl = state.cells[i - 1].acsr.injg.FluidoPro.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwinjl = state.cells[i - 1].acsr.injg.FluidoPro.BSW * bainjl / (boinjl + bainjl * state.cells[i - 1].acsr.injg.FluidoPro.BSW - state.cells[i - 1].acsr.injg.FluidoPro.BSW * boinjl);
    } else {
        rholisF = rholis;
        boinjl = oilVolumeFactor;
        bainjl = waterVolumeFactor;
        fwinjl = state.cells[i - 1].FW;
    }
    // standard oil flow rate
    double qostd = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) *
                (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (state.cells[i - 1].acsr.injg.FluidoPro.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwinjl) / (boinjl * rholisF);
    // new standard gas flow rate, adding the gas source:
    double qgstd = qostd * state.cells[i - 1].flui.RGO + state.cells[i - 1].acsr.injg.QGas / units::kSecondsPerDay;
    double deng;
    double yco2;
    // balance that sets the gas density and the CO2 fraction from the gas source
    if (fabs(qgstd) > (*state.globals).smallValueTolerance * 1e-10 && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) {
        deng = (qostd * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + state.cells[i - 1].acsr.injg.QGas * state.cells[i - 1].acsr.injg.FluidoPro.Deng / units::kSecondsPerDay) / qgstd;
        yco2 = (qostd * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + state.cells[i - 1].acsr.injg.QGas * state.cells[i - 1].acsr.injg.FluidoPro.yco2 / units::kSecondsPerDay) / qgstd;
    } else {
        deng = state.cells[i - 1].flui.Deng;
        yco2 = state.cells[i - 1].flui.yco2;
    }

    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;
    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance))
            state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
        if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio && state.cells[i].flui.RGO < 1e7)
            state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    }

    // new separator GOR, changed by the liquid source
    if (fabs(qostd + qostd2) > (*state.globals).smallValueTolerance && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15)
        state.cells[i].flui.RGO = qgstd / (qostd + qostd2);
    else if (fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15)
        state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    else
        state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;

    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd + qostd2) > 1e-15 && state.input.flashCompleto == 0 && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd + (141.5 / (131.5 + state.cells[i].flui.API)) * qostd2;
            denmixSTD /= (qostd + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - state.cells[i - 1].acsr.injg.FluidoPro.BSW) > 0)
            waterFlowRate2 = qostd2 * state.cells[i - 1].acsr.injg.FluidoPro.BSW / (1. - state.cells[i - 1].acsr.injg.FluidoPro.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwinjl / (1000. * state.cells[i - 1].acsr.injg.FluidoPro.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd + qostd2) > 1e-15 && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15)
            state.cells[i].flui.BSW = (waterFlowRate1 + waterFlowRate2) / (waterFlowRate1 + waterFlowRate2 + qostd + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;
        if (fabs(waterFlowRate1 + waterFlowRate2) > 1e-15)
            state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                    waterFlowRate2 * state.cells[i - 1].acsr.injg.FluidoPro.Denag) /
                                   (waterFlowRate1 + waterFlowRate2);
        else
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
        // re-evaluates the pairs the dead-oil viscosity model needs
        // (ASTM). This is always done with the same maximum and
        // minimum temperatures to build the pairs
        if (fabs(qostd + qostd2) > 1e-15 && state.input.flashCompleto == 0 && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) { // liquid flow rate > 0
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * state.cells[i - 1].acsr.injg.FluidoPro.VisOM(temperatureLow)) / (qostd + qostd2);
            state.cells[i].flui.LVisH = (qostd * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * state.cells[i - 1].acsr.injg.FluidoPro.VisOM(temperatureHigh)) / (qostd + qostd2);
        } else if (state.input.flashCompleto == 0 || state.cells[i - 1].acsr.injg.QGas <= 0.) { // without liquid flow
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }

    if (state.input.flashCompleto == 0) { // this switch loads into cell i
        // the variables that matter to the black-oil model
        state.cells[i].flui.Deng = deng;
        state.cells[i].flui.yco2 = yco2;
        if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }

    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance && state.cells[i - 1].alf < 1. - (*state.globals).smallValueTolerance * 1e-10 && fabs(state.cells[i - 1].acsr.injg.QGas) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / state.cells[i - 1].acsr.injg.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = state.cells[i - 1].fontemassCR / state.cells[i - 1].acsr.injg.fluidocol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp);

        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;

        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;

        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);

        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassLiquidInjection(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double waterVolumeFactor, double solutionGasRatio, double tmed, double &residenceTimeSource, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boinjl;
    double bainjl;
    double fwinjl;
    residenceTimeSource = state.cells[i - 1].acsr.injl.fluidocol.TR;
    if (fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15) {
        rholisF = state.cells[i - 1].acsr.injl.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boinjl = state.cells[i - 1].acsr.injl.FluidoPro.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        bainjl = state.cells[i - 1].acsr.injl.FluidoPro.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwinjl = state.cells[i - 1].acsr.injl.FluidoPro.BSW * bainjl / (boinjl + bainjl * state.cells[i - 1].acsr.injl.FluidoPro.BSW - state.cells[i - 1].acsr.injl.FluidoPro.BSW * boinjl);
    } else {
        rholisF = rholis;
        boinjl = oilVolumeFactor;
        bainjl = waterVolumeFactor;
        fwinjl = state.cells[i - 1].FW;
    }
    // standard oil flow rate before the source:
    double qostd1 = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (state.cells[i - 1].acsr.injl.FluidoPro.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwinjl) / (boinjl * rholisF);
    // total gas flow rate at standard conditions: the sum of the gas carried
    // across the left boundary
    // of the cell and the gas associated with the liquid source
    double qgstd = qostd1 * state.cells[i - 1].flui.RGO + qostd2 * state.cells[i - 1].acsr.injl.FluidoPro.RGO;
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;

    if (fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15) {
        if (state.input.trackRGO == -1) {
            // Accounts for dissolved gas in the gas-oil ratio.
            if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance))
                state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
            if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio)
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else {
            // new separator GOR, changed by the liquid source
            if (fabs(qostd1 + qostd2) > (*state.globals).smallValueTolerance)
                state.cells[i].flui.RGO = qgstd / (qostd1 + qostd2);
            else if (i == 1 && state.cells[0].MC == 0. && state.cells[0].Mliqini == 0. &&
                     state.cells[0].MComp == 0. && state.cells[0].acsr.injl.QLiq > 0. &&
                     qostd1 == 0. && qostd2 > 0. && std::isfinite(qostd2) &&
                     state.cells[0].acsr.injl.FluidoPro.RGO >= 0. &&
                     std::isfinite(state.cells[0].acsr.injl.FluidoPro.RGO))
                state.cells[i].flui.RGO = state.cells[0].acsr.injl.FluidoPro.RGO;
            else
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        }
    } else
        state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    // new gas density at standard conditions, changed by the liquid source
    if (fabs(qgstd) > (*state.globals).smallValueTolerance && state.input.flashCompleto == 0 && fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15 &&
        fabs(qgstd) > 1e-15)
        state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + qostd2 * state.cells[i - 1].acsr.injl.FluidoPro.RGO * state.cells[i - 1].acsr.injl.FluidoPro.Deng) / qgstd;
    else if (fabs(state.cells[i - 1].acsr.injl.QLiq) <= 1e-15)
        state.cells[i].flui.Deng = state.cells[i - 1].flui.Deng;
    // new CO2 fraction, changed by the liquid source
    if (fabs(qgstd) > (*state.globals).smallValueTolerance && state.input.flashCompleto == 0 && fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15 &&
        fabs(qgstd) > 1e-15)
        state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + qostd2 * state.cells[i - 1].acsr.injl.FluidoPro.RGO * state.cells[i - 1].acsr.injl.FluidoPro.yco2) / qgstd;
    else if (fabs(state.cells[i - 1].acsr.injl.QLiq) <= 1e-15)
        state.cells[i].flui.yco2 = state.cells[i - 1].flui.yco2;
    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd1 +
                               (141.5 / (131.5 + state.cells[i - 1].acsr.injl.FluidoPro.API)) * qostd2;
            denmixSTD /= (qostd1 + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0 ||
                   fabs(state.cells[i - 1].acsr.injl.QLiq) <= 1e-15)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - state.cells[i - 1].acsr.injl.FluidoPro.BSW) > 0)
            waterFlowRate2 = qostd2 * state.cells[i - 1].acsr.injl.FluidoPro.BSW / (1. - state.cells[i - 1].acsr.injl.FluidoPro.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwinjl / (1000. * state.cells[i - 1].acsr.injl.FluidoPro.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15 &&
            fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15)
            state.cells[i].flui.BSW = (waterFlowRate1 + waterFlowRate2) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;
        if (fabs(waterFlowRate1 + waterFlowRate2) > 1e-15)
            state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                    waterFlowRate2 * state.cells[i - 1].acsr.injl.FluidoPro.Denag) /
                                   (waterFlowRate1 + waterFlowRate2);
        else
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
        // re-evaluates the pairs the dead-oil viscosity model needs
        // (ASTM). This is always done with the same maximum and
        // minimum temperatures to build the pairs
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15) { // liquid flow rate > 0
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * state.cells[i - 1].acsr.injl.FluidoPro.VisOM(temperatureLow)) / (qostd1 + qostd2);
            state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * state.cells[i - 1].acsr.injl.FluidoPro.VisOM(temperatureHigh)) / (qostd1 + qostd2);
        } else if (state.input.flashCompleto == 0) { // without liquid flow
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }
    if (state.input.flashCompleto == 0) {
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if ((state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 || fabs(state.cells[i - 1].fontemassCR) > (*state.globals).smallValueTolerance * 1e-6) &&
        fabs(state.cells[i - 1].acsr.injl.QLiq) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the liquid source can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / state.cells[i - 1].acsr.injl.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = state.cells[i - 1].fontemassCR / state.cells[i - 1].acsr.injl.fluidocol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassInflowPerformance(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double waterVolumeFactor, double solutionGasRatio, double tmed, double &residenceTimeSource, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boipr;
    double baipr;
    double fwipr;
    residenceTimeSource = 0.;
    if (fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
        rholisF = state.cells[i - 1].acsr.ipr.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boipr = state.cells[i - 1].acsr.ipr.FluidoPro.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        baipr = state.cells[i - 1].acsr.ipr.FluidoPro.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwipr = state.cells[i - 1].acsr.ipr.FluidoPro.BSW * baipr / (boipr + baipr * state.cells[i - 1].acsr.ipr.FluidoPro.BSW - state.cells[i - 1].acsr.ipr.FluidoPro.BSW * boipr);
    } else {
        rholisF = rholis;
        boipr = oilVolumeFactor;
        baipr = waterVolumeFactor;
        fwipr = state.cells[i - 1].FW;
    }
    // standard oil flow rate before the source:
    double qostd1 = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (state.cells[i - 1].acsr.ipr.FluidoPro.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwipr) / (boipr * rholisF);
    // total gas flow rate at standard conditions: the sum of the gas carried
    // across the left boundary
    // of the cell and the gas associated with the IPR
    double qgstd = qostd1 * state.cells[i - 1].flui.RGO + qostd2 * state.cells[i - 1].acsr.ipr.FluidoPro.RGO;
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;
    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if ((state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15) {
            if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance) &&
                fabs(state.cells[i - 1].fontemassLR) > 1e-15)
                state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
            if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio)
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;

    } else { // new separator GOR, changed by the IPR
        if ((state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15) {
            if (fabs(qostd1 + qostd2) > (*state.globals).smallValueTolerance * 1e-10)
                state.cells[i].flui.RGO = qgstd / (qostd1 + qostd2);
            else
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    }
    // new gas density at standard conditions, changed by the IPR
    if (state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15 &&
        fabs(qgstd) > 1e-15)
        state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + qostd2 * state.cells[i - 1].acsr.ipr.FluidoPro.RGO * state.cells[i - 1].acsr.ipr.FluidoPro.Deng) / qgstd;
    else
        state.cells[i].flui.Deng = state.cells[i - 1].flui.Deng;
    // new CO2 fraction, changed by the liquid source
    if (state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15 && fabs(qgstd) > 1e-15)
        state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + qostd2 * state.cells[i - 1].acsr.ipr.FluidoPro.RGO * state.cells[i - 1].acsr.ipr.FluidoPro.yco2) / qgstd;
    else
        state.cells[i].flui.yco2 = state.cells[i - 1].flui.yco2;
    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            (state.cells[i - 1].fontemassLR) > 1e-15) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd1 +
                               (141.5 / (131.5 + state.cells[i - 1].acsr.ipr.FluidoPro.API)) * qostd2;
            denmixSTD /= (qostd1 + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0 || (state.cells[i - 1].fontemassLR) <= 1e-15)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - state.cells[i - 1].acsr.ipr.FluidoPro.BSW) > 0)
            waterFlowRate2 = qostd2 * state.cells[i - 1].acsr.ipr.FluidoPro.BSW / (1. - state.cells[i - 1].acsr.ipr.FluidoPro.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwipr / (1000. * state.cells[i - 1].acsr.ipr.FluidoPro.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15 &&
            (state.cells[i - 1].fontemassLR) > 1e-15)
            state.cells[i].flui.BSW = ((waterFlowRate1 + qostd1) * state.cells[i - 1].flui.BSW + (waterFlowRate2 + qostd2) * state.cells[i - 1].acsr.ipr.FluidoPro.BSW) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;
        if ((waterFlowRate1 + waterFlowRate2) > 1e-15)
            state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                    waterFlowRate2 * state.cells[i - 1].acsr.ipr.FluidoPro.Denag) /
                                   (waterFlowRate1 + waterFlowRate2);
        else
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
        if ((qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15) {
            // re-evaluates the pairs the dead-oil viscosity model needs
            // (ASTM). This is always done with the same maximum and
            // minimum temperatures to build the pairs
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * state.cells[i - 1].acsr.ipr.FluidoPro.VisOM(temperatureLow)) / (qostd1 + qostd2);
            state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * state.cells[i - 1].acsr.ipr.FluidoPro.VisOM(temperatureHigh)) / (qostd1 + qostd2);
        } else if (state.input.flashCompleto == 0 || fabs(state.cells[i - 1].fontemassLR) <= 1e-15) { // without liquid flow
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }
    if (state.input.flashCompleto == 0) {
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 && fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the IPR can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / state.cells[i - 1].acsr.ipr.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = 0.;
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassMultipleSource(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double waterVolumeFactor, double solutionGasRatio, double tmed, double &residenceTimeSource, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boinjl;
    double bainjl;
    double fwinjl;
    residenceTimeSource = state.cells[i - 1].acsr.injm.fluidocol.TR;
    if (fabs(state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15) {
        rholisF = state.cells[i - 1].acsr.injm.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boinjl = state.cells[i - 1].acsr.injm.FluidoPro.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        bainjl = state.cells[i - 1].acsr.injm.FluidoPro.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwinjl = state.cells[i - 1].acsr.injm.FluidoPro.BSW * bainjl / (boinjl + bainjl * state.cells[i - 1].acsr.injm.FluidoPro.BSW - state.cells[i - 1].acsr.injm.FluidoPro.BSW * boinjl);
    } else {
        rholisF = rholis;
        boinjl = oilVolumeFactor;
        bainjl = waterVolumeFactor;
        fwinjl = state.cells[i - 1].FW;
    }
    // standard oil flow rate before the source:
    double qostd1 = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (state.cells[i - 1].acsr.injm.FluidoPro.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwinjl) / (boinjl * rholisF);
    // total gas flow rate at standard conditions: the sum of the gas carried
    // across the left boundary
    // of the cell and the gas associated with the liquid source
    double qgstd = qostd1 * state.cells[i - 1].flui.RGO + qostd2 * state.cells[i - 1].acsr.injm.FluidoPro.RGO;
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;

    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if ((state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15) {
            if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance))
                state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
            if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio)
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    } else {
        // new separator GOR, changed by the liquid source
        if ((state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15) {
            if (fabs(qostd1 + qostd2) > (*state.globals).smallValueTolerance)
                state.cells[i].flui.RGO = qgstd / (qostd1 + qostd2);
            else
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    }
    // new gas density at standard conditions, changed by the liquid source
    if (fabs(qgstd) > (*state.globals).smallValueTolerance * 1e-10 && state.input.flashCompleto == 0 &&
        (state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15)
        state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + qostd2 * state.cells[i - 1].acsr.injm.FluidoPro.RGO * state.cells[i - 1].acsr.injm.FluidoPro.Deng) / qgstd;
    else
        state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    // new CO2 fraction, changed by the liquid source
    if (fabs(qgstd) > (*state.globals).smallValueTolerance * 1e-10 && state.input.flashCompleto == 0 &&
        (state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15)
        state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + qostd2 * state.cells[i - 1].acsr.injm.FluidoPro.RGO * state.cells[i - 1].acsr.injm.FluidoPro.yco2) / qgstd;
    else
        state.cells[i].flui.yco2 = state.cells[i - 1].flui.yco2;
    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            fabs(state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd1 +
                               (141.5 / (131.5 + state.cells[i - 1].acsr.injm.FluidoPro.API)) * qostd2;
            denmixSTD /= (qostd1 + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0 ||
                   fabs(state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) <= 1e-15)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - state.cells[i - 1].acsr.injm.FluidoPro.BSW) > 0)
            waterFlowRate2 = qostd2 * state.cells[i - 1].acsr.injm.FluidoPro.BSW / (1. - state.cells[i - 1].acsr.injm.FluidoPro.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwinjl / (1000. * state.cells[i - 1].acsr.injm.FluidoPro.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15 &&
            (state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15)
            state.cells[i].flui.BSW = (waterFlowRate1 + waterFlowRate2) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;
        if (fabs(waterFlowRate1 + waterFlowRate2) > 1e-15 &&
            (state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15)
            state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                    waterFlowRate2 * state.cells[i - 1].acsr.injm.FluidoPro.Denag) /
                                   (waterFlowRate1 + waterFlowRate2);
        else
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
        // re-evaluates the pairs the dead-oil viscosity model needs
        // (ASTM). This is always done with the same maximum and
        // minimum temperatures to build the pairs
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            (state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) > 1e-15) { // liquid flow rate > 0
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * state.cells[i - 1].acsr.injm.FluidoPro.VisOM(temperatureLow)) / (qostd1 + qostd2);
            state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * state.cells[i - 1].acsr.injm.FluidoPro.VisOM(temperatureHigh)) / (qostd1 + qostd2);
        } else if (state.input.flashCompleto == 0 ||
                   fabs(state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP) <= 1e-15) { // without liquid flow
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }
    if (state.input.flashCompleto == 0) {
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if ((state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 || fabs(state.cells[i - 1].fontemassCR) > (*state.globals).smallValueTolerance) &&
        fabs(state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP + state.cells[i - 1].acsr.injm.MassC) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the liquid source can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / state.cells[i - 1].acsr.injm.FluidoPro.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = state.cells[i - 1].fontemassCR / state.cells[i - 1].acsr.injm.fluidocol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassLeakSource(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double solutionGasRatio, double tmed, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    ProFlu fluF;
    // decides the source's fluid: if the ambient pressure is below the
    // pipe pressure, the pipe's fluid; otherwise the fluid defined as
    // ambient fluid
    if (state.cells[i - 1].acsr.fontechk.presT > state.cells[i - 1].acsr.fontechk.pamb) {
        fluF = state.cells[i - 1].acsr.fontechk.fluidoP;
    } else {
        fluF = state.cells[i - 1].acsr.fontechk.fluidoPamb;
    }
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF = fluF.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double boinjl = fluF.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double bainjl = fluF.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double fwinjl = fluF.BSW * bainjl / (boinjl + bainjl * fluF.BSW - fluF.BSW * boinjl);
    // standard oil flow rate before the source:
    double qostd1 = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (fluF.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwinjl) / (boinjl * rholisF);
    // total gas flow rate at standard conditions: the sum of the gas carried
    // across the left boundary
    // of the cell and the gas associated with the leak
    double qgstd;
    if (fabs(qostd2) > 0.)
        qgstd = qostd1 * state.cells[i - 1].flui.RGO + qostd2 * fluF.RGO;
    else {
        qgstd = qostd1 * state.cells[i - 1].flui.RGO + state.cells[i - 1].fontemassGR / (fluF.Deng * kAirDensityAtStandardConditions);
    }
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;

    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance * 1e-6) && bsw < (1. - (*state.globals).smallValueTolerance))
            state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
        if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio)
            state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    } else { // new separator GOR, changed by the IPR
        if (fabs(qostd1 + qostd2) > (*state.globals).smallValueTolerance * 1e-6)
            state.cells[i].flui.RGO = qgstd / (qostd1 + qostd2);
        else
            state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
    }
    // new gas density at standard conditions, changed by the leak
    if (fabs(qgstd) > (*state.globals).smallValueTolerance && state.input.flashCompleto == 0) {
        if (fabs(qostd2) > 0)
            state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + qostd2 * fluF.RGO * fluF.Deng) / qgstd;
        else
            state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + state.cells[i - 1].fontemassGR / (fluF.Deng * kAirDensityAtStandardConditions)) / qgstd;
    }
    // new CO2 fraction, changed by the leak
    if (fabs(qgstd) > (*state.globals).smallValueTolerance && state.input.flashCompleto == 0) {
        if (fabs(qostd2) > 0)
            state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + qostd2 * fluF.RGO * fluF.yco2) / qgstd;
        else
            state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + fluF.yco2 * state.cells[i - 1].fontemassGR / (fluF.Deng * kAirDensityAtStandardConditions)) / qgstd;
    }
    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd1 +
                               (141.5 / (131.5 + fluF.API)) * qostd2;
            denmixSTD /= (qostd1 + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - fluF.BSW) > 0)
            waterFlowRate2 = qostd2 * fluF.BSW / (1. - fluF.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwinjl / (1000. * fluF.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15)
            state.cells[i].flui.BSW = (waterFlowRate1 + waterFlowRate2) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;

        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0) {
            // re-evaluates the pairs the dead-oil viscosity model needs
            // (ASTM). This is always done with the same maximum and
            // minimum temperatures to build the pairs
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * fluF.VisOM(temperatureLow)) / (qostd1 + qostd2);
            state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * fluF.VisOM(temperatureHigh)) / (qostd1 + qostd2);
        } else if (state.input.flashCompleto == 0) {
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }
    if (state.input.flashCompleto == 0) {
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 || fabs(state.cells[i - 1].fontemassCR) > (*state.globals).smallValueTolerance * 1e-6) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the leak can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / fluF.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = state.cells[i - 1].fontemassCR / state.cells[i - 1].acsr.fontechk.fluidocol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassRadialPorous(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double waterVolumeFactor, double solutionGasRatio, double tmed, double &residenceTimeSource, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boipr;
    double baipr;
    double fwipr;
    residenceTimeSource = 0.;
    if (fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
        rholisF = state.cells[i - 1].acsr.radialPoro.flup.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boipr = state.cells[i - 1].acsr.radialPoro.flup.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        baipr = state.cells[i - 1].acsr.radialPoro.flup.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwipr = state.cells[i - 1].acsr.radialPoro.BSW * baipr / (boipr + baipr * state.cells[i - 1].acsr.radialPoro.BSW - state.cells[i - 1].acsr.radialPoro.BSW * boipr);
    } else {
        rholisF = rholis;
        boipr = oilVolumeFactor;
        baipr = waterVolumeFactor;
        fwipr = state.cells[i - 1].FW;
    }
    // standard oil flow rate before the source:
    double qostd1 = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (state.cells[i - 1].acsr.radialPoro.flup.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwipr) / (boipr * rholisF);
    // total gas flow rate at standard conditions: the sum of the gas carried
    // across the left boundary
    // of the cell and the gas associated with the IPR
    double qgstd = qostd1 * state.cells[i - 1].flui.RGO + qostd2 * state.cells[i - 1].acsr.radialPoro.flup.RGO;
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;
    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
            if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance) &&
                fabs(state.cells[i - 1].fontemassLR) > 1e-15)
                state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
            if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio)
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;

    } else { // new separator GOR, changed by the IPR
        if ((state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15) {
            if (fabs(qostd1 + qostd2) > (*state.globals).smallValueTolerance * 1e-10)
                state.cells[i].flui.RGO = qgstd / (qostd1 + qostd2);
            else
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    }
    // new gas density at standard conditions, changed by the IPR
    if (state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15 &&
        fabs(qgstd) > 1e-15)
        state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + qostd2 * state.cells[i - 1].acsr.radialPoro.flup.RGO * state.cells[i - 1].acsr.radialPoro.flup.Deng) / qgstd;
    else
        state.cells[i].flui.Deng = state.cells[i - 1].flui.Deng;
    // new CO2 fraction, changed by the liquid source
    if (state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15 && fabs(qgstd) > 1e-15)
        state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + qostd2 * state.cells[i - 1].acsr.radialPoro.flup.RGO * state.cells[i - 1].acsr.radialPoro.flup.yco2) / qgstd;
    else
        state.cells[i].flui.yco2 = state.cells[i - 1].flui.yco2;
    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            (state.cells[i - 1].fontemassLR) > 1e-15) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd1 +
                               (141.5 / (131.5 + state.cells[i - 1].acsr.radialPoro.flup.API)) * qostd2;
            denmixSTD /= (qostd1 + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0 || fabs(state.cells[i - 1].fontemassLR) <= 1e-15)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - state.cells[i - 1].acsr.radialPoro.BSW) > 0)
            waterFlowRate2 = qostd2 * state.cells[i - 1].acsr.radialPoro.BSW / (1. - state.cells[i - 1].acsr.radialPoro.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwipr / (1000. * state.cells[i - 1].acsr.radialPoro.flup.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15 &&
            fabs(state.cells[i - 1].fontemassLR) > 1e-15)
            state.cells[i].flui.BSW = ((waterFlowRate1 + qostd1) * state.cells[i - 1].flui.BSW + (waterFlowRate2 + qostd2) * state.cells[i - 1].acsr.radialPoro.BSW) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;
        if ((waterFlowRate1 + waterFlowRate2) > 1e-15 && (state.cells[i - 1].fontemassLR) > 1e-15)
            state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                    waterFlowRate2 * state.cells[i - 1].acsr.radialPoro.flup.Denag) /
                                   (waterFlowRate1 + waterFlowRate2);
        else
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15) {
            // re-evaluates the pairs the dead-oil viscosity model needs
            // (ASTM). This is always done with the same maximum and
            // minimum temperatures to build the pairs
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * state.cells[i - 1].acsr.radialPoro.flup.VisOM(temperatureLow)) / (qostd1 + qostd2);
            state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * state.cells[i - 1].acsr.radialPoro.flup.VisOM(temperatureHigh)) / (qostd1 + qostd2);
        } else if (state.input.flashCompleto == 0 || fabs(state.cells[i - 1].fontemassLR) <= 1e-15) { // without liquid flow
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }
    if (state.input.flashCompleto == 0) {
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 && fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the IPR can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / state.cells[i - 1].acsr.radialPoro.flup.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = 0.;
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void applySteadyMassPorous2D(const SteadyStateState &state, int i, double temperatureLow, double temperatureHigh, double oilVolumeFactor, double waterVolumeFactor, double solutionGasRatio, double tmed, double &residenceTimeSource, double &oilVolumeFactorInSitu, double &waterVolumeFactorInSitu, double &waterCutInSitu) {
    double rholis = state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rholisF;
    double boipr;
    double baipr;
    double fwipr;
    residenceTimeSource = 0.;
    if (fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
        rholisF = state.cells[i - 1].acsr.poroso2D.dados.flup.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        boipr = state.cells[i - 1].acsr.poroso2D.dados.flup.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        baipr = state.cells[i - 1].acsr.poroso2D.dados.flup.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        fwipr = state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW * baipr / (boipr + baipr * state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW - state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW * boipr);
    } else {
        rholisF = rholis;
        boipr = oilVolumeFactor;
        baipr = waterVolumeFactor;
        fwipr = state.cells[i - 1].FW;
    }
    // standard oil flow rate before the source:
    double qostd1 = 0.;
    if (state.cells[i - 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd1 = state.cells[i - 1].Mliqini * (1. - state.cells[i - 1].FW) * (1. - state.cells[i - 1].bet) / (oilVolumeFactor * rholis);
    // standard oil flow rate of the source:
    double qostd2 = 0.;
    if (state.cells[i - 1].acsr.poroso2D.dados.flup.dStockTankVaporMassFraction < 1. - 1e-15)
        qostd2 = state.cells[i - 1].fontemassLR * (1. - fwipr) / (boipr * rholisF);
    // total gas flow rate at standard conditions: the sum of the gas carried
    // across the left boundary
    // of the cell and the gas associated with the IPR
    double qgstd = qostd1 * state.cells[i - 1].flui.RGO + qostd2 * state.cells[i - 1].acsr.poroso2D.dados.flup.RGO;
    double liquidHoldup = 1 - state.cells[i - 1].alf;
    double complementaryFraction = state.cells[i - 1].bet;
    double bsw = state.cells[i - 1].FW;
    double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
    double rhogST = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
    // volume of light components in cell i, following the equations in the report;
    // irrelevant to the steady state, but it must be computed,
    // as the transient takes it as input
    state.cells[i - 1].VolLeveST = (((1 - liquidHoldup) * rhog / rhogST) + liquidHoldup * (1 - complementaryFraction) * (1. - bsw) * solutionGasRatio / oilVolumeFactor);
    if (state.cells[i - 1].VolLeveST < 1e-15)
        state.cells[i - 1].VolLeveST = 0.;
    if (state.input.trackRGO == -1) {
        // Accounts for dissolved gas in the gas-oil ratio.
        if (fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
            if (liquidHoldup > (*state.globals).smallValueTolerance && complementaryFraction < (1. - (*state.globals).smallValueTolerance) && bsw < (1. - (*state.globals).smallValueTolerance) &&
                fabs(state.cells[i - 1].fontemassLR) > 1e-15)
                state.cells[i].flui.RGO = state.cells[i - 1].VolLeveST * oilVolumeFactor / (liquidHoldup * (1 - complementaryFraction) * (1 - bsw));
            if (state.cells[i].flui.RGO > (*state.globals).maximumGasOilRatio)
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;

    } else { // new separator GOR, changed by the IPR
        if ((state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15) {
            if (fabs(qostd1 + qostd2) > (*state.globals).smallValueTolerance * 1e-10)
                state.cells[i].flui.RGO = qgstd / (qostd1 + qostd2);
            else
                state.cells[i].flui.RGO = (*state.globals).maximumGasOilRatio;
        } else
            state.cells[i].flui.RGO = state.cells[i - 1].flui.RGO;
    }
    // new gas density at standard conditions, changed by the IPR
    if (state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15 &&
        fabs(qgstd) > 1e-15)
        state.cells[i].flui.Deng = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.Deng + qostd2 * state.cells[i - 1].acsr.poroso2D.dados.flup.RGO * state.cells[i - 1].acsr.poroso2D.dados.flup.Deng) / qgstd;
    else
        state.cells[i].flui.Deng = state.cells[i - 1].flui.Deng;
    // new CO2 fraction, changed by the liquid source
    if (state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15 && (state.cells[i - 1].fontemassGR) > 1e-15 && fabs(qgstd) > 1e-15)
        state.cells[i].flui.yco2 = (qostd1 * state.cells[i - 1].flui.RGO * state.cells[i - 1].flui.yco2 + qostd2 * state.cells[i - 1].acsr.poroso2D.dados.flup.RGO * state.cells[i - 1].acsr.poroso2D.dados.flup.yco2) / qgstd;
    else
        state.cells[i].flui.yco2 = state.cells[i - 1].flui.yco2;
    if (state.productionFluidCount > 1 || (*state.globals).networkMode == 1) {
        // when the model is ASTM. Note that this only makes sense with more than one
        // production fluid in the JSON, or when the simulation is part of a network
        // with several branches feeding others with fluids of different properties
        // in each branch
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 &&
            (state.cells[i - 1].fontemassLR) > 1e-15) {
            double denmixSTD = (141.5 / (131.5 + state.cells[i - 1].flui.API)) * qostd1 +
                               (141.5 / (131.5 + state.cells[i - 1].acsr.poroso2D.dados.flup.API)) * qostd2;
            denmixSTD /= (qostd1 + qostd2);
            state.cells[i].flui.API = 141.5 / denmixSTD - 131.5;
        } else if (state.input.flashCompleto == 0 || (state.cells[i - 1].fontemassLR) <= 1e-15)
            state.cells[i].flui.API = state.cells[i - 1].flui.API;
        double waterFlowRate1;
        if ((1. - state.cells[i - 1].flui.BSW) > 0)
            waterFlowRate1 = qostd1 * state.cells[i - 1].flui.BSW / (1. - state.cells[i - 1].flui.BSW);
        else
            waterFlowRate1 = state.cells[i - 1].Mliqini * state.cells[i - 1].FW * (1. - state.cells[i - 1].bet) / (1000. * state.cells[i - 1].flui.Denag);
        double waterFlowRate2;
        if ((1. - state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW) > 0)
            waterFlowRate2 = qostd2 * state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW / (1. - state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW);
        else
            waterFlowRate2 = state.cells[i - 1].fontemassLR * fwipr / (1000. * state.cells[i - 1].acsr.poroso2D.dados.flup.Denag);

        if (fabs(waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2) > 1e-15 &&
            (state.cells[i - 1].fontemassLR) > 1e-15)
            state.cells[i].flui.BSW = ((waterFlowRate1 + qostd1) * state.cells[i - 1].flui.BSW + (waterFlowRate2 + qostd2) * state.cells[i - 1].acsr.poroso2D.dados.transfer.BSW) / (waterFlowRate1 + waterFlowRate2 + qostd1 + qostd2);
        else
            state.cells[i].flui.BSW = state.cells[i - 1].flui.BSW;
        if (fabs(waterFlowRate1 + waterFlowRate2) > 1e-15 &&
            (state.cells[i - 1].fontemassLR) > 1e-15)
            state.cells[i].flui.Denag = (waterFlowRate1 * state.cells[i - 1].flui.Denag +
                                    waterFlowRate2 * state.cells[i - 1].acsr.poroso2D.dados.flup.Denag) /
                                   (waterFlowRate1 + waterFlowRate2);
        else
            state.cells[i].flui.Denag = state.cells[i - 1].flui.Denag;
        if (fabs(qostd1 + qostd2) > 1e-15 && state.input.flashCompleto == 0 && (state.cells[i - 1].fontemassLR) > 1e-15) {
            // re-evaluates the pairs the dead-oil viscosity model needs
            // (ASTM). This is always done with the same maximum and
            // minimum temperatures to build the pairs
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
            state.cells[i].flui.LVisL = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureLow) + qostd2 * state.cells[i - 1].acsr.poroso2D.dados.flup.VisOM(temperatureLow)) / (qostd1 + qostd2);
            state.cells[i].flui.LVisH = (qostd1 * state.cells[i - 1].flui.VisOM(temperatureHigh) + qostd2 * state.cells[i - 1].acsr.poroso2D.dados.flup.VisOM(temperatureHigh)) / (qostd1 + qostd2);
        } else if (state.input.flashCompleto == 0 || fabs(state.cells[i - 1].fontemassLR) <= 1e-15) { // without liquid flow
            state.cells[i].flui.TempL = state.cells[i - 1].flui.TempL;
            state.cells[i].flui.TempH = state.cells[i - 1].flui.TempH;
            state.cells[i].flui.LVisL = state.cells[i - 1].flui.LVisL;
            state.cells[i].flui.LVisH = state.cells[i - 1].flui.LVisH;
        }
    }
    if (state.input.flashCompleto == 0) {
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if (state.cells[i - 1].bet > (*state.globals).smallValueTolerance * 1e-6 && fabs(state.cells[i - 1].fontemassLR) > 1e-15) {
        // re-evaluates the complementary-liquid volume fraction;
        // note that the IPR can have a complementary-liquid fraction
        // different from the one at the left of cell i-1
        double oilVolumeFactorNeighbour = state.cells[i].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterVolumeFactorNeighbour = state.cells[i].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double waterCutNeighbour = state.cells[i].flui.BSW * waterVolumeFactorNeighbour / (oilVolumeFactorNeighbour + waterVolumeFactorNeighbour * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorNeighbour);
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
        waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
        waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
        double qlpF = state.cells[i - 1].fontemassLR / state.cells[i - 1].acsr.poroso2D.dados.flup.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp);
        double qlcF = 0.;
        double betN = (state.cells[i - 1].QL * state.cells[i - 1].bet + qlcF) / (state.cells[i - 1].QL + qlpF + qlcF);
        double oilFlowRate = (state.cells[i - 1].QL + qlpF + qlcF) * (1 - waterCutNeighbour) * (1 - betN) * oilVolumeFactorInSitu / oilVolumeFactor;
        double waterFlowRate;
        if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
            waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
        else
            waterFlowRate = state.cells[i - 1].QL * (1 - state.cells[i - 1].bet) + qlpF;
        double complementaryFlowRate = (state.cells[i - 1].QL * (state.cells[i - 1].bet) + qlcF) * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp) / state.cells[i].fluicol.MasEspFlu(state.cells[i].presaux, tmed);
        state.cells[i].bet = fabs(complementaryFlowRate) / (fabs(oilFlowRate) + fabs(waterFlowRate) + fabs(complementaryFlowRate));
    } else
        state.cells[i].bet = state.cells[i - 1].bet;
}

void finalizeSteadyMassWithLiquid(const SteadyStateState &state, int i, double waterCutInSitu, double tmed, double pmed, double &rhog, double &rhol) {
    double oilFlowRate = 0.;
    if (state.input.tipoFluido == 0) {
        double solutionGasRatio = state.cells[i].flui.RS(pmed, tmed);
        double oilVolumeFactor = state.cells[i].flui.BOFunc(pmed, tmed, solutionGasRatio);
        solutionGasRatio = solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
        double rhogstd = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
        double rhololeostd = 1000. * 141.5 / (131.5 + state.cells[i].flui.API);
        double rhoa = state.cells[i].flui.Denag * 1000.;

        double denom = state.cells[i].flui.RGO * rhogstd + rhololeostd;
        if (fabs(1 - state.cells[i].flui.BSW) > (*state.globals).smallValueTolerance) {
            denom += rhoa * state.cells[i].flui.BSW / (1 - state.cells[i].flui.BSW);
            if (fabs(1 - state.cells[i].bet) > (*state.globals).smallValueTolerance * 1e-6 && fabs(state.cells[i].bet) > (*state.globals).smallValueTolerance * 1e-10) {
                denom += (state.cells[i].fluicol.MasEspFlu(pmed, tmed) * state.cells[i].bet / (1 - state.cells[i].bet)) * oilVolumeFactor * (1. + waterCutInSitu / (1 - waterCutInSitu));
            }
        }
        if (fabs(1 - state.cells[i].bet) > (*state.globals).smallValueTolerance * 1e-6 && fabs(1 - state.cells[i].flui.BSW) > (*state.globals).smallValueTolerance)
            oilFlowRate = state.cells[i].MC / denom;

        if (fabs(1 - state.cells[i].bet) > (*state.globals).smallValueTolerance * 1e-6 && fabs(1 - state.cells[i].flui.BSW) > (*state.globals).smallValueTolerance) {
            if ((*state.globals).networkFluidBalanceMode == 0)
                state.cells[i].Mliqini = state.cells[i].MC - (state.cells[i].flui.RGO - solutionGasRatio) * rhogstd * oilFlowRate;
            else {
                double masoleo = oilFlowRate * oilVolumeFactor * state.cells[i].flui.MasEspoleo(pmed, tmed, solutionGasRatio) + oilFlowRate * rhoa * state.cells[i].flui.BSW / (1 - state.cells[i].flui.BSW);
                state.cells[i].Mliqini = masoleo + state.cells[i].MComp;
            }
        } else
            state.cells[i].Mliqini = state.cells[i].MC - (state.cells[i - 1].MC - state.cells[i - 1].Mliqini) - state.cells[i - 1].fontemassGR;
    } else {
        double quality = state.cells[i].flui.FracMass(pmed, tmed);
        state.cells[i].Mliqini = state.cells[i].MC * (1. - quality);
    }
    state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;
    if (i < state.lastCell)
        state.cells[i + 1].MliqiniL = state.cells[i].Mliqini;
    pmed = state.cells[i].presaux + state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2;
    rhog = state.cells[i].rgCi = state.cells[i].flui.MasEspGas(pmed, tmed);

    // densities at the cell's left interface
    state.cells[i].rpCi = state.cells[i].flui.MasEspLiq(pmed, tmed);
    state.cells[i].rcCi = state.cells[i].fluicol.MasEspFlu(pmed, tmed);
    rhol = (1 - state.cells[i].bet) * state.cells[i].rpCi + state.cells[i].bet * state.cells[i].rcCi;
    // volumetric flow rates:
    state.cells[i].QL = state.cells[i].Mliqini / rhol;
    if (i < state.lastCell)
        state.cells[i + 1].QLL = state.cells[i].QL;
    state.cells[i - 1].QLR = state.cells[i].QL;
    state.cells[i].QG = (state.cells[i].MC - state.cells[i].Mliqini) / rhog;
}

void finalizeSteadyMassNoFlow(const SteadyStateState &state, int i) {
    if (state.input.tipoFluido == 1) {
        state.cells[i].alf = 0.;
    } else {
        state.cells[i].alf = 1.;
    }
    publishSteadyVoidFraction(state, i);
    publishSteadyComplementaryFraction(state, i);
}

void finalizeSteadyMassLiquidOnly(const SteadyStateState &state, int i) {
    state.cells[i].alf = 0.;
    publishSteadyVoidFraction(state, i);
    publishSteadyComplementaryFraction(state, i);
}

void finalizeSteadyMassGasOnly(const SteadyStateState &state, int i) {
    state.cells[i].alf = 1.;
    publishSteadyVoidFraction(state, i);
    state.cells[i].c0 = 1.;
    state.cells[i].ud = 0.;
    publishSteadyComplementaryFraction(state, i);
}

void finalizeSteadyMassTwoPhase(const SteadyStateState &state, const SteadyDependencies &dependencies, int i, double rhog, double rhol) {
    double c0 = 1.;
    double ud = 0.;
    if (fabs(state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-6) {
        if (state.input.tipoModeloDrift == 1) {
            if (state.steadyIteration == 0) { // first estimate, first iteration
                // the no-slip void fraction is used, since the correlation that gives the
                // void fraction depends on the void fraction itself
                if ((fabs(state.cells[i].QG) + fabs(state.cells[i].QL)) > (*state.globals).smallValueTolerance) {
                    if (state.convergenceMonitor > 0.01)
                        state.cells[i].alf = fabs(state.cells[i].QG) /
                                        (fabs(state.cells[i].QG) + fabs(state.cells[i].QL));
                    driftflux::coefficient::steadyState(dependencies.closure, i, c0, ud);
                } else
                    state.cells[i].alf = 0.;
                publishSteadyVoidFraction(state, i);
            }
            if (fabs(rhog) / rhol > 0.9) {
                c0 = 1.;
                ud = 0.;
            }

            // at steady state, the void fraction comes from the slip relations,
            // so this is where Co and Ud are obtained:
            else if (fabs(state.cells[i].QG) > (*state.globals).smallValueTolerance && fabs(state.cells[i].QL) > (*state.globals).smallValueTolerance * 1e-6)
                driftflux::coefficient::steadyState(dependencies.closure, i, c0, ud);
            state.cells[i].c0 = c0;
            state.cells[i].ud = ud;
            double area = state.cells[i].duto.area;
            if (fabs(state.cells[i].QG + state.cells[i].QL) > (*state.globals).smallValueTolerance) {
                // void fraction with slip:
                state.cells[i].alf = state.cells[i].QG / (c0 * (state.cells[i].QG + state.cells[i].QL) + ud * area);
                double alfHomo = state.cells[i].QG / (state.cells[i].QG + state.cells[i].QL);
                if (state.cells[i].alf > 1. - 1e-15 || state.cells[i].alf < 1e-15)
                    state.cells[i].alf = alfHomo;

            } else
                state.cells[i].alf = 0.;
            if (state.cells[i].alf > (1 - (*state.globals).smallValueTolerance) && fabs(state.cells[i].QG + state.cells[i].QL) > (*state.globals).smallValueTolerance)
                state.cells[i].alf = fabs(state.cells[i].QG) / fabs(state.cells[i].QG + state.cells[i].QL);
            else if (state.cells[i].alf > (1 - (*state.globals).smallValueTolerance))
                state.cells[i].alf = 1.;
        } else {
            double holdup;
            double frictionGrad;
            double gravityGrad;
            double totalGrad;
            double reynolds;
            unsigned char flowType;
            executarCorrelacao(state.cells, i, 0, state.input.AceleraConvergPerm,
                               state.cells[i - 1].correlacaoMR2,
                               holdup, frictionGrad, gravityGrad, totalGrad,
                               reynolds, flowType);
           if(state.cells[i - 1].correlacaoMR2==16){
            if(flowType=='1' || flowType=='2')state.cells[i].arranjo=0;
            if(flowType=='3')state.cells[i].arranjo=1;
            if(flowType=='4')state.cells[i].arranjo=2;
            if(flowType=='6')state.cells[i].arranjo=-1;
            if(flowType=='5')state.cells[i].arranjo=-2;
           }
           else{
        	   if(flowType=='1')state.cells[i].arranjo=1;
        	   if(flowType=='2')state.cells[i].arranjo=2;
               if(flowType=='3')state.cells[i].arranjo=3;
               if(flowType=='4')state.cells[i].arranjo=4;
               if(flowType=='6')state.cells[i].arranjo=5;
               if(flowType=='5')state.cells[i].arranjo=6;
           }
            state.cells[i].alf = 1. - holdup;
        }
    } else {
        state.cells[i].alf = 1.;
    }
    if (state.cells[i].alf < 0.)
        state.cells[i].alf = 0.;
    else if (state.cells[i].alf > 1.)
        state.cells[i].alf = 1.;
    // updates the volume fractions of cell i kept in
    // other cells, including the "previous time" values, which do not
    // matter to the steady problem but do if the steady result
    // starts the transient solution:
    publishSteadyVoidFraction(state, i);
    publishSteadyComplementaryFraction(state, i);
}

void refreshSeedCompositionalProperties(const SteadyStateState &state, double pressureGuess,
                                        ProFlu &sourceFluid, int gasMode) {
    if (state.input.flashCompleto != 2)
        return;
    if (state.input.tabelaDinamica == 0)
        state.cells[0].flui.atualizaPropComp(pressureGuess, state.cells[0].temp);
    sourceFluid.atualizaPropComp(pressureGuess, state.cells[0].temp, -1, nullptr, nullptr, gasMode);
}

InitialPhaseFractions wetGasSeedFractions(const SteadyStateState &state, double pressureGuess) {
    auto &source = state.cells[0].acsr.injg;
    InitialPhaseFractions fractions;
    const double gasMassFlowRate = source.VMas(pressureGuess, state.cells[0].temp);
    double quality;
    if (state.input.flashCompleto != 2)
        quality = source.FluidoPro.FracMassHidra(1., 20.);
    else
        quality = source.FluidoPro.dStockTankVaporMassFraction;
    const double totalMassFlowRate = gasMassFlowRate / quality;
    quality = source.FluidoPro.FracMassHidra(pressureGuess, state.cells[0].temp);
    const double gasVolumeFlowRate = totalMassFlowRate * quality /
                  source.FluidoPro.MasEspGas(pressureGuess, state.cells[0].temp);
    const double liquidVolumeFlowRate = totalMassFlowRate * (1. - quality) /
                  source.FluidoPro.MasEspLiq(pressureGuess, state.cells[0].temp);
    double complementaryVolumeFlowRate = source.razCompGas *
                   source.QGas * source.fluidocol.standardDensity() /
                   source.fluidocol.MasEspFlu(pressureGuess, state.cells[0].temp);
    complementaryVolumeFlowRate /= units::kSecondsPerDay;
    fractions.voidFraction = gasVolumeFlowRate / (liquidVolumeFlowRate + complementaryVolumeFlowRate + gasVolumeFlowRate);
    if ((fabs(complementaryVolumeFlowRate) + fabs(liquidVolumeFlowRate)) > 1e-15)
        fractions.complementaryLiquidFraction = fabs(complementaryVolumeFlowRate) / (fabs(complementaryVolumeFlowRate) + fabs(liquidVolumeFlowRate));
    else
        fractions.complementaryLiquidFraction = 0.;
    return fractions;
}

InitialPhaseFractions seedFirstCellVoidFraction(const SteadyStateState &state,
                                                const SteadyDependencies &dependencies,
                                                double pressureGuess) {
    switch (state.cells[0].acsr.tipo) {
    case kAccessoryNone: {
        InitialPhaseFractions fractions;
        state.cells[0].temp = state.input.celp[0].textern;
        fractions.voidFraction = 1.;
        fractions.complementaryLiquidFraction = 0.;
        if (state.input.flashCompleto == 2 && state.input.tabelaDinamica == 0)
            state.cells[0].flui.atualizaPropComp(pressureGuess, state.cells[0].temp);
        return fractions;
    }
    case kAccessoryGasInjection: {
        auto &source = state.cells[0].acsr.injg;
        state.cells[0].temp = source.temp;
        refreshSeedCompositionalProperties(state, pressureGuess, source.FluidoPro,
                                            source.seco);
        if (source.seco == 1)
            return {1., 0.};
        return wetGasSeedFractions(state, pressureGuess);
    }
    case kAccessoryLiquidInjection: {
        auto &source = state.cells[0].acsr.injl;
        InitialPhaseFractions fractions;
        state.cells[0].temp = source.temp;
        refreshSeedCompositionalProperties(state, pressureGuess, source.FluidoPro);
        const double gasVolumeFlowRate = source.QLiq * (1 - source.bet) *
                      (1. - source.FluidoPro.BSW) *
                      (source.FluidoPro.RGO -
                       source.FluidoPro.rDgD * source.FluidoPro.RS(pressureGuess, state.cells[0].temp) * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                      source.FluidoPro.Deng * kAirDensityAtStandardConditions / source.FluidoPro.MasEspGas(pressureGuess, state.cells[0].temp);
        const double liquidVolumeFlowRate = source.QLiq * (1 - source.bet) *
                          (1. - source.FluidoPro.BSW) * source.FluidoPro.BOFunc(pressureGuess, state.cells[0].temp) +
                      source.QLiq * (1 - source.bet) *
                          source.FluidoPro.BSW * source.FluidoPro.BAFunc(pressureGuess, state.cells[0].temp) +
                      source.QLiq * source.bet;
        fractions.voidFraction = gasVolumeFlowRate / (liquidVolumeFlowRate + gasVolumeFlowRate);
        fractions.complementaryLiquidFraction = source.bet;
        return fractions;
    }
    case kAccessoryInflowPerformance: {
        auto &source = state.cells[0].acsr.ipr;
        InitialPhaseFractions fractions;
        state.cells[0].temp = source.Tres;
        refreshSeedCompositionalProperties(state, pressureGuess, source.FluidoPro);
        const double gasVolumeFlowRate = source.MasG(pressureGuess, state.cells[0].temp) /
                      source.FluidoPro.MasEspGas(pressureGuess, state.cells[0].temp);
        const double liquidVolumeFlowRate = source.MasL(pressureGuess, state.cells[0].temp) /
                      source.FluidoPro.MasEspLiq(pressureGuess, state.cells[0].temp);
        fractions.voidFraction = gasVolumeFlowRate / (liquidVolumeFlowRate + gasVolumeFlowRate);
        fractions.complementaryLiquidFraction = 0.;
        return fractions;
    }
    case kAccessoryRadialPorous: {
        auto &source = state.cells[0].acsr.radialPoro;
        InitialPhaseFractions fractions;
        state.cells[0].temp = source.tRes;
        state.cells[0].pres = pressureGuess;
        refreshSeedCompositionalProperties(state, pressureGuess, source.flup);
        sources::renewSourceTerms(dependencies.sources, 0);
        const double gasVolumeFlowRate = source.fluxIniG /
                      source.flup.MasEspGas(pressureGuess, state.cells[0].temp);
        const double liquidVolumeFlowRate = (source.fluxIni + source.fluxIniA) /
                      source.flup.MasEspLiq(pressureGuess, state.cells[0].temp);
        fractions.voidFraction = gasVolumeFlowRate / (liquidVolumeFlowRate + gasVolumeFlowRate);
        fractions.complementaryLiquidFraction = 0.;
        state.cells[0].flui.BSW = source.BSW;
        return fractions;
    }
    case kAccessoryPorous2D: {
        auto &source = state.cells[0].acsr.poroso2D;
        InitialPhaseFractions fractions;
        state.cells[0].temp = source.dados.tRes;
        state.cells[0].pres = pressureGuess;
        refreshSeedCompositionalProperties(state, pressureGuess, source.dados.flup);
        sources::renewSourceTerms(dependencies.sources, 0);
        const double gasVolumeFlowRate = source.dados.transfer.fluxIniG /
                      source.dados.flup.MasEspGas(pressureGuess, state.cells[0].temp);
        const double liquidVolumeFlowRate = (source.dados.transfer.fluxIni + source.dados.transfer.fluxIniA) /
                      source.dados.flup.MasEspLiq(pressureGuess, state.cells[0].temp);
        fractions.voidFraction = gasVolumeFlowRate / (liquidVolumeFlowRate + gasVolumeFlowRate);
        fractions.complementaryLiquidFraction = 0.;
        state.cells[0].flui.BSW = source.dados.transfer.BSW;
        return fractions;
    }
    case kAccessoryMultipleSource: {
        auto &source = state.cells[0].acsr.injm;
        InitialPhaseFractions fractions;
        state.cells[0].temp = source.temp;
        refreshSeedCompositionalProperties(state, pressureGuess, source.FluidoPro);
        if (source.condTermo == 0) {
            state.cells[0].pres = pressureGuess;
            sources::renewSourceTerms(dependencies.sources, 0);
        }
        const double gasVolumeFlowRate = source.MassG /
                      source.FluidoPro.MasEspGas(pressureGuess, state.cells[0].temp);
        const double liquidVolumeFlowRate = source.MassP /
                          source.FluidoPro.MasEspLiq(pressureGuess, state.cells[0].temp) +
                      source.MassC /
                          source.fluidocol.MasEspFlu(pressureGuess, state.cells[0].temp);
        fractions.voidFraction = gasVolumeFlowRate / (liquidVolumeFlowRate + gasVolumeFlowRate);
        fractions.complementaryLiquidFraction = 0.;
        return fractions;
    }
    default: {
        InitialPhaseFractions fractions;
        state.cells[0].temp = state.input.celp[0].textern;
        if (state.input.flashCompleto == 2 && state.input.tabelaDinamica == 0)
            state.cells[0].flui.atualizaPropComp(pressureGuess, state.cells[0].temp);
        fractions.voidFraction = 1.;
        fractions.complementaryLiquidFraction = 0.;
        return fractions;
    }
    }
}

void initializeClosedSourceBoundary(const SteadyStateState &state) {
    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;
}

void resetProductionMarchInlet(const SteadyStateState &state, double pressureGuess,
                               const InitialPhaseFractions &fractions) {
    state.cells[0].presauxL = pressureGuess;
    state.cells[0].presLini = pressureGuess;
    state.cells[0].presL = pressureGuess;
    state.cells[0].pres = pressureGuess;
    state.cells[1].presL = pressureGuess;
    state.cells[0].presini = pressureGuess;
    state.cells[1].presLini = pressureGuess;
    state.cells[0].presaux = pressureGuess;
    state.cells[1].presauxL = pressureGuess;

    state.cells[0].alf = fractions.voidFraction;
    state.cells[0].alfini = fractions.voidFraction;
    state.cells[0].bet = fractions.complementaryLiquidFraction;
    state.cells[0].betini = fractions.complementaryLiquidFraction;
    state.cells[1].alfL = state.cells[0].alf;
    state.cells[1].alfLini = state.cells[0].alf;
    state.cells[0].alfPigD = state.cells[0].alf;
    state.cells[0].alfPigDini = state.cells[0].alf;
    state.cells[0].alfPigE = state.cells[0].alf;
    state.cells[0].alfPigEini = state.cells[0].alf;
    state.cells[1].betL = state.cells[0].bet;
    state.cells[1].betLini = state.cells[0].bet;
    state.cells[0].betPigD = state.cells[0].bet;
    state.cells[0].betPigDini = state.cells[0].bet;
    state.cells[0].betPigE = state.cells[0].bet;
    state.cells[0].betPigEini = state.cells[0].bet;
    state.cells[0].betI = state.cells[0].bet;
    state.cells[1].betLI = state.cells[0].bet;
}

/// Solves the gas line at steady state under its inlet condition. With the
/// injection pressure given, marches the line when the injection choke is not
/// throttling, and searches its pressure (tertiary) when it is. With the flow
/// rate given, searches its pressure (secondary).
void solveGasLineSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, InjectionPressureCondition) {
    // march for the injection-pressure case
    if (state.input.chokes.abertura[0] >= 0.2) { // injection choke inactive
        marchGasSteady(state, dependencies);
    } else
        steady::searchGasPressureSteadyTertiary(dependencies.search, dependencies); // injection choke active
}

void solveGasLineSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, InjectionFlowRateCondition) {
    steady::searchGasPressureSteadySecondary(dependencies.search, dependencies); // gas-line march for the injection-flow-rate case
}

/// Before the first march, a pressure condition lets the gas-line pressure at
/// the valves be estimated by hydrostatics, and the valves' injection computed
/// from it. Under a flow-rate condition there is nothing to estimate: the valves
/// share the given rate.
void estimateValveGasPressure(const SteadyStateState &state, InjectionPressureCondition) {
    gasLineHydrostatic(state);
}

void estimateValveGasPressure(const SteadyStateState &, InjectionFlowRateCondition) {}

void marchGasLineAndCoupleAnnulus(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute) {
    if (pchute > 0 && state.input.lingas > 0 && state.input.nvalvgas > 0) {
        withGasInletCondition(state.gasCells[0].tipoCC,
                              [&](auto condition) { solveGasLineSteady(state, dependencies, condition); });
    }

    if (state.input.acopColAnulPermForte > 0 && state.input.lingas > 0 && state.thermalSourceDisabled == 0) {
        refreshProperties(state);
        refreshSteadyThermalVelocities(state);
        computePseudoTransientTimeStep(state);
        gaslift::connectTubing(dependencies.gasLift);
        for (int kontaPseudo = 0; kontaPseudo < state.input.acopColAnulPermForte; kontaPseudo++) {
            for (int iterm = 0; iterm <= state.lastGasCell; iterm++)
                state.gasCells[iterm].tempini = state.gasCells[iterm].temp;
            for (int iterm = 1; iterm <= state.lastGasCell; iterm++) {
                thermal::computeGasTemperature(dependencies.thermal, iterm, state.gasCells[iterm - 1].tempini, 1);
            }
            state.cells[0].tempini = state.cells[0].temp;
            state.cells[1].tempLini = state.cells[1].tempL;
            state.cells[1].tempL = state.cells[0].temp;
            for (int iterm = 1; iterm <= state.lastCell; iterm++) {
                state.cells[iterm].tempini = state.cells[iterm].temp;
            }
            for (int iterm = 1; iterm <= state.lastCell; iterm++) {
                thermal::computeTemperature(dependencies.thermal, iterm, state.cells[iterm].tempini, 1);
            }
            refreshProperties(state);
            computePseudoTransientTimeStep(state);
            gaslift::connectTubing(dependencies.gasLift);
        }
    }
}

bool advanceProductionCells(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute, int &i, double &abortValue) {
    while (i <= state.lastCell && state.cells[i - 1].pres >= kMarchMinimumPressure && fabs(pchute - state.cells[0].pres) < (*state.globals).smallValueTolerance) {

        advanceUpstreamSteadyPressure(state, i, 0); // march step to get the pressure at the left boundary
        // of cell i
        // checks whether something went wrong:
        if (state.input.usaTabela == 1 && (state.input.tabent.pmax - state.cells[i].presaux) < (*state.globals).smallValueTolerance) {
            abortValue = kSteadyMarchLimit;
            return true;
        }
        if (marchPressureTooLow(state.cells[i].presaux) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i].presaux) > (*state.globals).smallValueTolerance)) {
            abortValue = -kSteadyMarchLimit;
            return true;
        }
        refreshUpstreamProductionPeriphery(state, i); // update of the left boundary pressure,
        // if there is an ESP or a pressure increment
        if (state.input.flashCompleto != 2)
            advanceSteadyMass(state, dependencies, i); // checks whether the previous cell has a source, and so updates
        // the mass flow rates at the left boundary and the fluid properties,
        // gas density, GOR, API, BSW, beta
        else
            advanceCompositionalSteadyMass(state, dependencies, i);

        if (state.input.acopColAnulPermForte == 0 || state.input.lingas == 0 || state.convergenceMonitor > 0.3)
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 0); // advances the temperature from cell i-1 to cell i
        // checks whether the temperature went out of bounds
        // when working with a PVTSim table
        if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
            state.cells[i].temp = state.input.tabent.tmin;
        thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i); // just updates the left and right temperature fields
        advanceDownstreamSteadyPressure(state, i, 0); // advances the pressure from the left boundary of cell i to
        // its cell centre
        // checks whether something went wrong in this pressure advance to the centre of the
        // cell
        if (state.input.usaTabela == 1 && (state.input.tabent.pmax - state.cells[i].pres) < (*state.globals).smallValueTolerance) {
            abortValue = kSteadyMarchLimit;
            return true;
        }
        if (marchPressureTooLow(state.cells[i].pres) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i].pres) > (*state.globals).smallValueTolerance)) {
            abortValue = -kSteadyMarchLimit;
            return true;
        }
        refreshDownstreamProductionPeriphery(state, i); // just updates the fields that hold the pressures
        // of the cells to the left and right
        for (int j = 0; j < state.input.nvalvgas; j++) { // re-evaluates the gas-lift valve flow rate, when
            // the cell has one.

            if (state.productionValveCellIndices[j] == i) {
                int k = state.gasValveCellIndices[j];
                gaslift::computeSteadyGasFlowRate(dependencies.gasLift, dependencies.thermal, k);
            }
        }
        if (state.input.tipoFluido == 0)
            advanceSteadyMassTransfer(state, i - 1);
        else
            advanceSteadyGasMassTransfer(state, i - 1); // with a PVTSim table, computes the
        // interphase mass transfer rate for the
        // latent heat in the energy equation
        if (state.input.ordperm > 1) { // second-order correction
            double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
            double D0pres = state.cells[i].pres - state.cells[i].presaux;
            double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
            advanceUpstreamSteadyPressure(state, i, 1);
            refreshUpstreamProductionPeriphery(state, i);
            advanceSteadyMass(state, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
            if (isnan(state.cells[i].temp)) {
                if (state.input.transiente == 0 && state.input.AP == 0)
                    // in this case the simulation ends: there is no transient
                    // to run next and this is not a network
                    NumError(
                        "Temperatrura na linha de producao com valor NaN em marchaProdPerm1");
                else {
                    // only shows a warning
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    // if in a network iteration, after the first iteration
                    if ((*state.globals).networkIteration > 0) {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                    // if a transient simulation follows, or this is the first network iteration
                    else {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
                }
            }
            advanceDownstreamSteadyPressure(state, i, 1);
            state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
            state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
            state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
            refreshDownstreamProductionPeriphery(state, i);
            refreshUpstreamProductionPeriphery(state, i);
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceSteadyMass(state, dependencies, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
        }

        // once the pressure at the centre of cell i is reached, on the first march iteration
        // checks whether there is a gas-lift valve in i and makes an initial estimate of the gas-lift
        // flow rate (when there is a gas line). Note that this is done only on iteration zero.
        if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.steadyIteration == 0 && state.convergenceMonitor > 0.1)
            gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, i);
        i++;

        if (isnan(state.cells[i - 1].pres) || isnan(state.cells[i - 1].temp) || isnan(state.cells[i - 1].alf)) {
            abortValue = kSteadyMarchLimit;
            return true;
        }

        // checks whether the cell-centre pressure went above
        // the static pressure of an IPR, if any
        if (marchPressureTooLow(state.cells[i - 1].pres) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i - 1].pres) > (*state.globals).smallValueTolerance)) {
            abortValue = -kSteadyMarchLimit;
            return true;
        } else if ((state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                    ((state.cells[i - 1].acsr.ipr.Pres - state.cells[i - 1].pres) < (*state.globals).smallValueTolerance) && i == 1)) {
            abortValue = kSteadyMarchLimit;
            return true;
        }
    }
    return false;
}

bool advanceReverseProductionCells(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute, int &i, double &abortValue) {
    while (i <= state.lastCell && state.cells[i - 1].pres >= kMarchMinimumPressure && fabs(pchute - state.cells[0].pres) < (*state.globals).smallValueTolerance) {

        advanceUpstreamSteadyPressure(state, i, 0); // march step to get the pressure at the left boundary
        // of cell i
        // checks whether something went wrong:
        if (state.input.usaTabela == 1 && (state.input.tabent.pmax - state.cells[i].presaux) < (*state.globals).smallValueTolerance)
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        if (marchPressureTooLow(state.cells[i].presaux) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i].presaux) > (*state.globals).smallValueTolerance)) {
            {
                abortValue = -kSteadyMarchLimit;
                return true;
            }
        }
        refreshUpstreamProductionPeriphery(state, i); // update of the left boundary pressure,
        // if there is an ESP or a pressure increment
        if (state.input.flashCompleto != 2)
            advanceSteadyMass(state, dependencies, i); // checks whether the previous cell has a source, and so updates
        // the mass flow rates at the left boundary and the fluid properties,
        // gas density, GOR, API, BSW, beta
        else
            advanceCompositionalSteadyMass(state, dependencies, i);
        // checks whether the temperature went out of bounds
        // when working with a PVTSim table
        if (isnan(state.cells[i].temp))
            NumError("Temperatrura na linha de producao com valor NaN");
        if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
            state.cells[i].temp = state.input.tabent.tmin;
        thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i); // just updates the left and right temperature fields
        advanceDownstreamSteadyPressure(state, i, 0); // advances the pressure from the left boundary of cell i to
        // its cell centre
        // checks whether something went wrong in this pressure advance to the centre of the
        // cell
        if (state.input.usaTabela == 1 && (state.input.tabent.pmax - state.cells[i].pres) < (*state.globals).smallValueTolerance) {
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        }
        if (marchPressureTooLow(state.cells[i].pres) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i].pres) > (*state.globals).smallValueTolerance)) {
            {
                abortValue = -kSteadyMarchLimit;
                return true;
            }
        }
        refreshDownstreamProductionPeriphery(state, i); // just updates the fields that hold the pressures
        // of the cells to the left and right
        if (state.input.tipoFluido == 0)
            advanceSteadyMassTransfer(state, i - 1);
        else
            advanceSteadyGasMassTransfer(state, i - 1); // with a PVTSim table, computes the
        // interphase mass transfer rate for the
        // latent heat in the energy equation
        if (state.input.ordperm > 1) { // second-order correction
            double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
            double D0pres = state.cells[i].pres - state.cells[i].presaux;
            double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
            advanceUpstreamSteadyPressure(state, i, 1);
            refreshUpstreamProductionPeriphery(state, i);
            advanceSteadyMass(state, dependencies, i);
            advanceDownstreamSteadyPressure(state, i, 1);
            state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
            state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
            state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
            refreshDownstreamProductionPeriphery(state, i);
            refreshUpstreamProductionPeriphery(state, i);
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceSteadyMass(state, dependencies, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
        }

        i++;

        if (marchPressureTooLow(state.cells[i - 1].pres) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i - 1].pres) > (*state.globals).smallValueTolerance)) {
            {
                abortValue = -kSteadyMarchLimit;
                return true;
            }
        }
    }
    return false;
}

bool advanceProductionCellsSecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute, int &i, double &abortValue) {
    while (i <= state.lastCell && state.cells[i - 1].pres >= kMarchMinimumPressure && fabs(pchute - state.cells[0].pres) < (*state.globals).smallValueTolerance) {

        advanceUpstreamSteadyPressure(state, i, 0); // march step to get the pressure at the left boundary
        // of cell i
        // checks whether something went wrong:
        if (state.input.usaTabela == 1 && (state.input.tabent.pmax - state.cells[i].presaux) < (*state.globals).smallValueTolerance)
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        refreshUpstreamProductionPeriphery(state, i); // update of the left boundary pressure,
        // if there is an ESP or a pressure increment
        if (state.input.flashCompleto != 2)
            advanceSteadyMass(state, dependencies, i); // checks whether the previous cell has a source, and so updates
        // the mass flow rates at the left boundary and the fluid properties,
        // gas density, GOR, API, BSW, beta
        else
            advanceCompositionalSteadyMass(state, dependencies, i);

        if (state.input.acopColAnulPermForte == 0 || state.input.lingas == 0 || state.convergenceMonitor > 0.3)
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 0); // advances the temperature from cell i-1 to cell i
        // checks whether the temperature went out of bounds
        // when working with a PVTSim table
        if (isnan(state.cells[i].temp)) {
            if (state.input.transiente == 0 && state.input.AP == 0)
                // in this case the simulation ends: there is no transient
                // to run next and this is not a network
                NumError(
                    "Temperatrura na linha de producao com valor NaN em marchaProdPerm2");
            else {
                // only shows a warning
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                // if in a network iteration, after the first iteration
                if ((*state.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                // if a transient simulation follows, or this is the first network iteration
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
        }
        if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
            state.cells[i].temp = state.input.tabent.tmin;
        thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i); // just updates the left and right temperature fields
        advanceDownstreamSteadyPressure(state, i, 0); // advances the pressure from the left boundary of cell i to
        // its cell centre
        // checks whether something went wrong in this pressure advance to the centre of the
        // cell
        if (state.input.usaTabela == 1 && (state.input.tabent.pmax - state.cells[i].pres) < (*state.globals).smallValueTolerance)
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        refreshDownstreamProductionPeriphery(state, i); // just updates the fields that hold the pressures
        // of the cells to the left and right
        for (int j = 0; j < state.input.nvalvgas; j++) { // re-evaluates the gas-lift valve flow rate, when
            // the cell has one.

            if (state.productionValveCellIndices[j] == i) {
                int k = state.gasValveCellIndices[j];
                gaslift::computeSteadyGasFlowRate(dependencies.gasLift, dependencies.thermal, k);
            }
        }
        if (state.input.tipoFluido == 0)
            advanceSteadyMassTransfer(state, i - 1);
        else
            advanceSteadyGasMassTransfer(state, i - 1); // with a PVTSim table, computes the
        // interphase mass transfer rate for the
        // latent heat in the energy equation
        if (state.input.ordperm > 1) { // second-order correction
            double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
            double D0pres = state.cells[i].pres - state.cells[i].presaux;
            double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
            advanceUpstreamSteadyPressure(state, i, 1);
            refreshUpstreamProductionPeriphery(state, i);
            advanceSteadyMass(state, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
            advanceDownstreamSteadyPressure(state, i, 1);
            state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
            state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
            state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
            refreshDownstreamProductionPeriphery(state, i);
            refreshUpstreamProductionPeriphery(state, i);
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceSteadyMass(state, dependencies, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
        }
        // once the pressure at the centre of cell i is reached, on the first march iteration
        // checks whether there is a gas-lift valve in i and makes an initial estimate of the gas-lift
        // flow rate (when there is a gas line). Note that this is done only on iteration zero.
        if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.steadyIteration == 0 && state.convergenceMonitor > 0.1)
            gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, i);
        i++;
        // checks whether the cell-centre pressure went above
        // the static pressure of an IPR, if any, or fell too low
        if (marchPressureTooLow(state.cells[i - 1].pres) ||
            (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[i - 1].pres) > (*state.globals).smallValueTolerance))
            {
                abortValue = -kSteadyMarchLimit;
                return true;
            }
        else if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && ((state.cells[i - 1].acsr.ipr.Pres - state.cells[i - 1].pres) < (*state.globals).smallValueTolerance && i == 1))
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        else if (state.cells[i - 1].acsr.tipo == kAccessoryRadialPorous && ((state.cells[i - 1].acsr.radialPoro.pRes[0] - state.cells[i - 1].pres) < (*state.globals).smallValueTolerance && i == 1))
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        else if (state.cells[i - 1].acsr.tipo == kAccessoryPorous2D && ((state.cells[i - 1].acsr.poroso2D.dados.pRes - state.cells[i - 1].pres) < (*state.globals).smallValueTolerance && i == 1))
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        else if (state.input.usaTabela == 1) {
            if ((state.input.tabent.pmax - state.cells[i - 1].pres) < (*state.globals).smallValueTolerance)
                {
                    abortValue = kSteadyMarchLimit;
                    return true;
                }
        }
    }
    return false;
}

double surfaceChokeMassFlowRate(const SteadyStateState &state, const thermal::ThermalState &thermalState) {
    double maxSup;
    if (state.annulusDrift != 0 && state.cells[state.lastCell].pres > state.gasSurfacePressure) { // for the gas-lift ring, the flow rate at the end must be zero
        double tESup = state.cells[state.lastCell].temp;
        double alfSup = state.cells[state.lastCell].alf;
        double betSup = state.cells[state.lastCell].bet;

        double masentrada = state.cells[state.lastCell - 1].MR;
        double massgas = state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR;

        double quality;
        state.outletPressure = state.cells[state.lastCell].pres;
        state.cells[state.lastCell].flui.MasEspLiq(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp);
        quality = fabs(massgas / masentrada);

        double masChk;

        double ypres = state.gasSurfacePressure / state.outletPressure;
        masChk = state.surfaceChoke.vazmassSachd(ypres, state.outletPressure, tESup, alfSup, betSup, quality, state.cells[state.lastCell - 1].flui,
                                       state.cells[state.lastCell - 1].fluicol);
        maxSup = state.surfaceChoke.vazmaxSachd(state.outletPressure, tESup, alfSup, betSup, quality, state.cells[state.lastCell - 1].flui, state.cells[state.lastCell - 1].fluicol);
        if (fabs(ypres) > fabs(state.surfaceChoke.razpres))
            maxSup = masChk;
        // maxSup is the total flow rate through the choke

        if (state.surfaceChoke.AreaGarg > (1e-3) * state.cells[state.lastCell - 1].duto.area && ypres < 1.) {
            state.input.valTempChokeJus = thermal::surfaceChokeOutletTemperature(thermalState);
        }

    } else {
        maxSup = 0.;
        state.input.valTempChokeJus = state.cells[state.lastCell].temp;
    }

    return maxSup;
}

void seedFirstCellFromFlowRateGuess(const SteadyStateState &state, double mchute, double &alfini, double &betini) {
    if (state.cells[0].acsr.tipo == kAccessoryGasInjection) {
        state.cells[0].temp = state.cells[0].acsr.injg.temp;
        if (state.input.flashCompleto == 2) {
            if (state.input.tabelaDinamica == 0)
                state.cells[0].flui.atualizaPropComp(state.cells[0].pres, state.cells[0].temp);
            state.cells[0].acsr.injg.FluidoPro.atualizaPropComp(state.cells[0].pres, state.cells[0].temp, -1, NULL, NULL, state.cells[0].acsr.injg.seco);
        }
        state.cells[0].acsr.injg.QGas = mchute;
    } else if (state.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
        state.cells[0].temp = state.cells[0].acsr.injl.temp;
        if (state.input.flashCompleto == 2) {
            if (state.input.tabelaDinamica == 0)
                state.cells[0].flui.atualizaPropComp(state.cells[0].pres, state.cells[0].temp);
            state.cells[0].acsr.injl.FluidoPro.atualizaPropComp(state.cells[0].pres, state.cells[0].temp);
        }
        state.cells[0].acsr.injl.QLiq = mchute;
    }

    if (state.cells[0].acsr.tipo == kAccessoryGasInjection) {
        if (state.cells[0].acsr.injg.seco == 1) {
            alfini = 1.;
            betini = 0.;
        } else {
            double masgas = state.cells[0].acsr.injg.VMas(state.cells[0].pres, state.cells[0].temp);
            double quality;
            if (state.input.flashCompleto != 2)
                quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(1., 20.);
            else
                quality = state.cells[0].acsr.injg.FluidoPro.dStockTankVaporMassFraction;
            double masT = masgas / quality;
            quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(state.cells[0].pres, state.cells[0].temp);
            double qgas = masT * quality /
                          state.cells[0].acsr.injg.FluidoPro.MasEspGas(state.cells[0].pres, state.cells[0].temp);
            double qliq = masT * (1. - quality) /
                          state.cells[0].acsr.injg.FluidoPro.MasEspLiq(state.cells[0].pres, state.cells[0].temp);
            double qcomp = state.cells[0].acsr.injg.razCompGas *
                           state.cells[0].acsr.injg.QGas * state.cells[0].acsr.injg.fluidocol.standardDensity() /
                           state.cells[0].acsr.injg.fluidocol.MasEspFlu(state.cells[0].pres, state.cells[0].temp);
            qcomp /= units::kSecondsPerDay;
            alfini = qgas / (qliq + qcomp + qgas);
            if ((fabs(qcomp) + fabs(qliq)) > 1e-15)
                betini = fabs(qcomp) / (fabs(qcomp) + fabs(qliq));
            else
                betini = 0.;
        }
    } else if (state.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
        double qgas = state.cells[0].acsr.injl.QLiq * (1 - state.cells[0].acsr.injl.bet) *
                      (1. - state.cells[0].acsr.injl.FluidoPro.BSW) *
                      (state.cells[0].acsr.injl.FluidoPro.RGO -
                       state.cells[0].acsr.injl.FluidoPro.rDgD * state.cells[0].acsr.injl.FluidoPro.RS(state.cells[0].pres, state.cells[0].temp) * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                      state.cells[0].acsr.injl.FluidoPro.Deng * kAirDensityAtStandardConditions / state.cells[0].acsr.injl.FluidoPro.MasEspGas(state.cells[0].pres, state.cells[0].temp);
        double qliq = state.cells[0].acsr.injl.QLiq * (1 - state.cells[0].acsr.injl.bet) *
                          (1. - state.cells[0].acsr.injl.FluidoPro.BSW) * state.cells[0].acsr.injl.FluidoPro.BOFunc(state.cells[0].pres, state.cells[0].temp) +
                      state.cells[0].acsr.injl.QLiq * (1 - state.cells[0].acsr.injl.bet) *
                          state.cells[0].acsr.injl.FluidoPro.BSW * state.cells[0].acsr.injl.FluidoPro.BAFunc(state.cells[0].pres, state.cells[0].temp) +
                      state.cells[0].acsr.injl.QLiq * state.cells[0].acsr.injl.bet;
        alfini = qgas / (qliq + qgas);
        betini = state.cells[0].acsr.injl.bet;
    }
}

void advanceProductionCellsPressureToPressureSecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, int &i) {
    while (i <= state.lastCell && state.cells[i - 1].pres >= 1.) {

        advanceUpstreamSteadyPressure(state, i, 0);
        refreshUpstreamProductionPeriphery(state, i);
        if (state.input.flashCompleto != 2)
            advanceSteadyMass(state, dependencies, i);
        else
            advanceCompositionalSteadyMass(state, dependencies, i);
        thermal::advanceSteadyTemperature(dependencies.thermal, i, 0);
        if (isnan(state.cells[i].temp))
            NumError("Temperatrura na linha de producao com valor NaN");
        if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
            state.cells[i].temp = state.input.tabent.tmin;
        thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
        advanceDownstreamSteadyPressure(state, i, 0);
        refreshDownstreamProductionPeriphery(state, i);
        if (state.input.tipoFluido == 0)
            advanceSteadyMassTransfer(state, i - 1);
        else
            advanceSteadyGasMassTransfer(state, i - 1);
        if (state.input.ordperm > 1) {
            double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
            double D0pres = state.cells[i].pres - state.cells[i].presaux;
            double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
            advanceUpstreamSteadyPressure(state, i, 1);
            refreshUpstreamProductionPeriphery(state, i);
            advanceSteadyMass(state, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
            advanceDownstreamSteadyPressure(state, i, 1);
            state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
            state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
            state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
            refreshDownstreamProductionPeriphery(state, i);
            refreshUpstreamProductionPeriphery(state, i);
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceSteadyMass(state, dependencies, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
        }

        i++;
    }
}

}  // namespace


void advanceReverseSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int i) {
    ProFlu fluF;
    double residenceTimeSource = 0.;
    if (i == 1) {
        sources::renewSourceTerms(dependencies.sources, i - 1); // checks whether the cell has a source and computes the
        // mass flow rates of produced liquid (oil+water), gas and complementary liquid
        // relation between the source at the left and at the right of a cell:
        state.cells[i].fontemassLL = state.cells[i - 1].fontemassLR;
        state.cells[i].fontemassCL = state.cells[i - 1].fontemassCR;
        state.cells[i].fontemassGL = state.cells[i - 1].fontemassGR;

        state.cells[i].MC = state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassGR;

        state.cells[i - 1].MR = state.cells[i].MC;
        if (i < state.lastCell)
            state.cells[i + 1].ML = state.cells[i].MC;
        state.cells[i - 1].MRini = state.cells[i - 1].MR;
        state.cells[i].MComp = state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR;

        if (state.cells[i - 1].acsr.tipo == kAccessoryGasInjection) {
            fluF = state.cells[i - 1].acsr.injg.FluidoPro;
            residenceTimeSource = state.cells[i - 1].acsr.injg.fluidocol.TR;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryLiquidInjection) {
            fluF = state.cells[i - 1].acsr.injl.FluidoPro;
            residenceTimeSource = state.cells[i - 1].acsr.injl.fluidocol.TR;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
            fluF = state.cells[i - 1].acsr.ipr.FluidoPro;
            residenceTimeSource = 0.;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryRadialPorous) {
            fluF = state.cells[i - 1].acsr.radialPoro.flup;
            residenceTimeSource = 0.;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryPorous2D) {
            fluF = state.cells[i - 1].acsr.poroso2D.dados.flup;
            residenceTimeSource = 0.;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryMultipleSource) {
            fluF = state.cells[i - 1].acsr.injm.FluidoPro;
            residenceTimeSource = state.cells[i - 1].acsr.injm.fluidocol.TR;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryLeak && state.cells[i - 1].acsr.fontechk.abertura > 1e-6) {
            if (state.cells[i - 1].acsr.fontechk.presT > state.cells[i - 1].acsr.fontechk.pamb) {
                fluF = state.cells[i - 1].acsr.fontechk.fluidoP;
            } else {
                fluF = state.cells[i - 1].acsr.fontechk.fluidoPamb;
            }
        }
        state.cells[i].flui = fluF;
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    } else {
        state.cells[i - 1].fontemassLR = 0.;
        state.cells[i - 1].fontemassCR = 0.;
        state.cells[i - 1].fontemassGR = 0.;

        state.cells[i].fontemassLL = state.cells[i - 1].fontemassLR;
        state.cells[i].fontemassCL = state.cells[i - 1].fontemassCR;
        state.cells[i].fontemassGL = state.cells[i - 1].fontemassGR;

        state.cells[i].MC = state.cells[i - 1].MC;

        state.cells[i - 1].MR = state.cells[i].MC;
        if (i < state.lastCell)
            state.cells[i + 1].ML = state.cells[i].MC;
        state.cells[i - 1].MRini = state.cells[i - 1].MR;

        state.cells[i].MComp = state.cells[i - 1].MComp;

        state.cells[i].flui = state.cells[i - 1].flui;
        state.cells[i].flui.RenovaFluido();
        fluidtracking::correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
    }
    if (i == 0) {
        if (state.cells[0].acsr.tipo == kAccessoryRadialPorous)
            state.cells[0].flui.BSW = state.cells[0].acsr.radialPoro.BSW;
        else if (state.cells[0].acsr.tipo == kAccessoryPorous2D)
            state.cells[0].flui.BSW = state.cells[0].acsr.poroso2D.dados.transfer.BSW;
    }

    double tmed;
    // temperature at the boundary between cells i-1 and i: from the second iteration on, one could
    // use the temperature of cell i, already computed, but this can complicate
    // convergence; it is safer to keep the criterion in every iteration, so the temperature at the
    // boundary is taken equal to the temperature of cell i-1
    if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
        tmed = (state.cells[i].dx * state.cells[i].temp + state.cells[i].dxR * state.cells[i].tempR) / (state.cells[i].dx + state.cells[i].dxR);
    else
        tmed = state.cells[i - 1].temp;

    double pmed = state.cells[i].presaux;

    state.cells[i].rpCi = state.cells[i].flui.MasEspLiq(pmed, tmed);
    state.cells[i].rcCi = state.cells[i].fluicol.MasEspFlu(pmed, tmed);
    state.cells[i].rgCi = state.cells[i].flui.MasEspGas(pmed, tmed);

    double MasCarb = state.cells[i].MC - state.cells[i].MComp;
    double MasGas = MasCarb * state.cells[i].flui.FracMassHidra(state.cells[i].presaux, tmed);
    double MasLiqProd = MasCarb - MasGas;
    double oilVolumeFactorInSitu;
    double waterVolumeFactorInSitu;
    if (state.cells[i].flui.RGO < 1e6)
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
    else
        oilVolumeFactorInSitu = 1.;
    waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
    double waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
    double oilFlowRate = MasLiqProd * (1 - state.cells[i - 1].FW) / state.cells[i].rpCi;
    double waterFlowRate;
    if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
        waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
    else
        waterFlowRate = MasLiqProd / state.cells[i].flui.MasEspAgua(pmed, tmed);
    double complementaryFlowRate;
    complementaryFlowRate = state.cells[i].MComp / state.cells[i].rcCi;

    state.cells[i].bet = complementaryFlowRate / (oilFlowRate + waterFlowRate + complementaryFlowRate);

    state.cells[i].Mliqini = MasLiqProd;
    state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;
    if (i < state.lastCell)
        state.cells[i + 1].MliqiniL = state.cells[i].Mliqini;

    double rhol = (1 - state.cells[i].bet) * state.cells[i].rpCi + state.cells[i].bet * state.cells[i].rcCi;
    double rhog = state.cells[i].rgCi;
    // volumetric flow rates:
    state.cells[i].QL = state.cells[i].Mliqini / rhol;
    if (i < state.lastCell)
        state.cells[i + 1].QLL = state.cells[i].QL;
    state.cells[i - 1].QLR = state.cells[i].QL;
    state.cells[i].QG = (state.cells[i].MC - state.cells[i].Mliqini) / rhog;

    // maximum and minimum temperatures for a possible re-evaluation of the
    // ASTM method when fluids mix and the dead-oil viscosity model, if it is
    // the ASTM one working with a pair of temperatures, is to be updated
    double temperatureLow;
    if (state.input.flashCompleto == 1 && (state.input.tabent.tmin - 0) > (*state.globals).smallValueTolerance)
        temperatureLow = state.input.tabent.tmin + 0.1;
    else
        temperatureLow = 0.;
    double temperatureHigh;
    if (state.input.flashCompleto == 1 && (70 - state.input.tabent.tmax) < (*state.globals).smallValueTolerance)
        temperatureHigh = state.input.tabent.tmax - 0.1;
    else
        temperatureHigh = 70.;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double solutionGasRatio;
    // computes RS, Bo and Ba in the cell before cell i
    if (state.cells[i - 1].flui.RGO < 1e7) {
        solutionGasRatio = state.cells[i - 1].flui.RS(state.cells[i - 1].pres, state.cells[i - 1].temp);
        oilVolumeFactor = state.cells[i - 1].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp, solutionGasRatio);
        waterVolumeFactor = state.cells[i - 1].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        solutionGasRatio = solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
    } else {
        oilVolumeFactor = 1;
        solutionGasRatio = 0;
        waterVolumeFactor = 0.;
    }
    // in-situ BSW of the previous cell (in the march, cell i)
    state.cells[i - 1].FW = state.cells[i - 1].flui.BSW * waterVolumeFactor / (oilVolumeFactor + waterVolumeFactor * state.cells[i - 1].flui.BSW - state.cells[i - 1].flui.BSW * oilVolumeFactor);
    state.cells[i - 1].FWini = state.cells[i - 1].FW;
    // Volume fractions:
    applyReverseSteadyPhaseFractions(state, dependencies, i, rhol, rhog);

    if (fabs(state.cells[i].QL) > 1e-15 && fabs(state.cells[i].MComp) > 1e-15) {
        double hol0 = 1. - state.cells[i - 1].alf;
        double hol1 = 1. - state.cells[i].alf;
        state.cells[i].fluicol.TR = (state.cells[i - 1].fluicol.TR * state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR * residenceTimeSource) / state.cells[i].MComp;
        state.cells[i].fluicol.TR = state.cells[i].fluicol.TR + (0.5 * state.cells[i - 1].duto.area * state.cells[i - 1].dx * hol0 +
                                                       0.5 * state.cells[i].duto.area * state.cells[i].dx * hol1) /
                                                          state.cells[i].QL;
    } else
        state.cells[i].fluicol.TR = 0.;
}

void advanceReverseCompositionalSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int i) {
    ProFlu fluF;
    double residenceTimeSource = 0.;
    if (i == 1) {
        sources::renewSourceTerms(dependencies.sources, i - 1); // checks whether the cell has a source and computes the
        // mass flow rates of produced liquid (oil+water), gas and complementary liquid
        // relation between the source at the left and at the right of a cell:
        state.cells[i].fontemassLL = state.cells[i - 1].fontemassLR;
        state.cells[i].fontemassCL = state.cells[i - 1].fontemassCR;
        state.cells[i].fontemassGL = state.cells[i - 1].fontemassGR;

        state.cells[i].MC = state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassGR;

        state.cells[i - 1].MR = state.cells[i].MC;
        if (i < state.lastCell)
            state.cells[i + 1].ML = state.cells[i].MC;
        state.cells[i - 1].MRini = state.cells[i - 1].MR;
        state.cells[i].MComp = state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR;

        if (state.cells[i - 1].acsr.tipo == kAccessoryGasInjection) {
            state.cells[i - 1].acsr.injg.FluidoPro.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            fluF = state.cells[i - 1].acsr.injg.FluidoPro;
            residenceTimeSource = state.cells[i - 1].acsr.injg.fluidocol.TR;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryLiquidInjection) {
            state.cells[i - 1].acsr.injl.FluidoPro.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            fluF = state.cells[i - 1].acsr.injl.FluidoPro;
            residenceTimeSource = state.cells[i - 1].acsr.injl.fluidocol.TR;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
            state.cells[i - 1].acsr.ipr.FluidoPro.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            fluF = state.cells[i - 1].acsr.ipr.FluidoPro;
            residenceTimeSource = 0.;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryMultipleSource) {
            state.cells[i - 1].acsr.injm.FluidoPro.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            fluF = state.cells[i - 1].acsr.injm.FluidoPro;
            residenceTimeSource = state.cells[i - 1].acsr.injm.fluidocol.TR;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryLeak && state.cells[i - 1].acsr.fontechk.abertura > 1e-6) {
            state.cells[i - 1].acsr.fontechk.fluidoP.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            state.cells[i - 1].acsr.fontechk.fluidoPamb.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            if (state.cells[i - 1].acsr.fontechk.presT > state.cells[i - 1].acsr.fontechk.pamb) {
                fluF = state.cells[i - 1].acsr.fontechk.fluidoP;
            } else {
                fluF = state.cells[i - 1].acsr.fontechk.fluidoPamb;
            }
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryRadialPorous) {
            state.cells[i - 1].acsr.radialPoro.flup.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            fluF = state.cells[i - 1].acsr.radialPoro.flup;
            residenceTimeSource = 0.;
        } else if (state.cells[i - 1].acsr.tipo == kAccessoryPorous2D) {
            state.cells[i - 1].acsr.poroso2D.dados.flup.atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL, state.input.pocinjec);
            fluF = state.cells[i - 1].acsr.poroso2D.dados.flup;
            residenceTimeSource = 0.;
        }
        state.cells[i].flui = fluF;

    } else {
        state.cells[i - 1].fontemassLR = 0.;
        state.cells[i - 1].fontemassCR = 0.;
        state.cells[i - 1].fontemassGR = 0.;

        state.cells[i].fontemassLL = state.cells[i - 1].fontemassLR;
        state.cells[i].fontemassCL = state.cells[i - 1].fontemassCR;
        state.cells[i].fontemassGL = state.cells[i - 1].fontemassGR;

        state.cells[i].MC = state.cells[i - 1].MC;

        state.cells[i - 1].MR = state.cells[i].MC;
        if (i < state.lastCell)
            state.cells[i + 1].ML = state.cells[i].MC;
        state.cells[i - 1].MRini = state.cells[i - 1].MR;

        state.cells[i].MComp = state.cells[i - 1].MComp;

        state.cells[i].flui = state.cells[i - 1].flui;
    }
    if (i == 0) {
        if (state.cells[0].acsr.tipo == kAccessoryRadialPorous)
            state.cells[0].flui.BSW = state.cells[0].acsr.radialPoro.BSW;
        else if (state.cells[0].acsr.tipo == kAccessoryPorous2D)
            state.cells[0].flui.BSW = state.cells[0].acsr.poroso2D.dados.transfer.BSW;
    }

    double tmed;
    // temperature at the boundary between cells i-1 and i: from the second iteration on, one could
    // use the temperature of cell i, already computed, but this can complicate
    // convergence; it is safer to keep the criterion in every iteration, so the temperature at the
    // boundary is taken equal to the temperature of cell i-1
    if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
        tmed = (state.cells[i].dx * state.cells[i].temp + state.cells[i].dxR * state.cells[i].tempR) / (state.cells[i].dx + state.cells[i].dxR);
    else
        tmed = state.cells[i - 1].temp;

    double pmed = state.cells[i].presaux;

    refreshCompositionalFlashFromNeighbour(state, i, pmed, tmed);

    double titulo = state.cells[i].flui.dVaporMassFraction;

    state.cells[i].rpCi = state.cells[i].flui.MasEspLiq(pmed, tmed);
    state.cells[i].rcCi = state.cells[i].fluicol.MasEspFlu(pmed, tmed);
    state.cells[i].rgCi = state.cells[i].flui.MasEspGas(pmed, tmed);

    double MasCarb = state.cells[i].MC - state.cells[i].MComp;
    double MasGas = MasCarb * state.cells[i].flui.FracMassHidra(state.cells[i].presaux, tmed);
    double MasLiqProd = MasCarb - MasGas;
    double oilVolumeFactorInSitu;
    double waterVolumeFactorInSitu;
    if (titulo < 1 - 1e-15)
        oilVolumeFactorInSitu = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed);
    else
        oilVolumeFactorInSitu = 1.;
    waterVolumeFactorInSitu = state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed);
    double waterCutInSitu = state.cells[i].flui.BSW * waterVolumeFactorInSitu / (oilVolumeFactorInSitu + waterVolumeFactorInSitu * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorInSitu);
    double oilFlowRate = MasLiqProd * (1 - state.cells[i - 1].FW) / state.cells[i].rpCi;
    double waterFlowRate;
    if (waterCutInSitu < (1 - (*state.globals).smallValueTolerance))
        waterFlowRate = waterCutInSitu * oilFlowRate / (1 - waterCutInSitu);
    else
        waterFlowRate = MasLiqProd / state.cells[i].flui.MasEspAgua(pmed, tmed);
    double complementaryFlowRate;
    complementaryFlowRate = state.cells[i].MComp / state.cells[i].rcCi;

    state.cells[i].bet = complementaryFlowRate / (oilFlowRate + waterFlowRate + complementaryFlowRate);

    state.cells[i].Mliqini = MasLiqProd;
    state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;
    if (i < state.lastCell)
        state.cells[i + 1].MliqiniL = state.cells[i].Mliqini;

    double rhol = (1 - state.cells[i].bet) * state.cells[i].rpCi + state.cells[i].bet * state.cells[i].rcCi;
    double rhog = state.cells[i].rgCi;
    // volumetric flow rates:
    state.cells[i].QL = state.cells[i].Mliqini / rhol;
    if (i < state.lastCell)
        state.cells[i + 1].QLL = state.cells[i].QL;
    state.cells[i - 1].QLR = state.cells[i].QL;
    state.cells[i].QG = (state.cells[i].MC - state.cells[i].Mliqini) / rhog;

    // maximum and minimum temperatures for a possible re-evaluation of the
    // ASTM method when fluids mix and the dead-oil viscosity model, if it is
    // the ASTM one working with a pair of temperatures, is to be updated
    double temperatureLow;
    if (state.input.flashCompleto == 1 && (state.input.tabent.tmin - 0) > (*state.globals).smallValueTolerance)
        temperatureLow = state.input.tabent.tmin + 0.1;
    else
        temperatureLow = 0.;
    double temperatureHigh;
    if (state.input.flashCompleto == 1 && (70 - state.input.tabent.tmax) < (*state.globals).smallValueTolerance)
        temperatureHigh = state.input.tabent.tmax - 0.1;
    else
        temperatureHigh = 70.;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double solutionGasRatio;
    // computes RS, Bo and Ba in the cell before cell i
    if (titulo < 1 - 1e-15) {
        solutionGasRatio = state.cells[i - 1].flui.RS(state.cells[i - 1].pres, state.cells[i - 1].temp);
        oilVolumeFactor = state.cells[i - 1].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp, solutionGasRatio);
        waterVolumeFactor = state.cells[i - 1].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        solutionGasRatio = solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
    } else {
        oilVolumeFactor = 1;
        solutionGasRatio = 0;
        waterVolumeFactor = 0.;
    }
    // in-situ BSW of the previous cell (in the march, cell i)
    state.cells[i - 1].FW = state.cells[i - 1].flui.BSW * waterVolumeFactor / (oilVolumeFactor + waterVolumeFactor * state.cells[i - 1].flui.BSW - state.cells[i - 1].flui.BSW * oilVolumeFactor);
    state.cells[i - 1].FWini = state.cells[i - 1].FW;
    // Volume fractions:
    applyReverseSteadyPhaseFractions(state, dependencies, i, rhol, rhog);
    if (fabs(state.cells[i].QL) > 1e-15 && fabs(state.cells[i].MComp) > 1e-15) {
        double hol0 = 1. - state.cells[i - 1].alf;
        double hol1 = 1. - state.cells[i].alf;
        state.cells[i].fluicol.TR = (state.cells[i - 1].fluicol.TR * state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR * residenceTimeSource) / state.cells[i].MComp;
        state.cells[i].fluicol.TR = state.cells[i].fluicol.TR + (0.5 * state.cells[i - 1].duto.area * state.cells[i - 1].dx * hol0 +
                                                       0.5 * state.cells[i].duto.area * state.cells[i].dx * hol1) /
                                                          state.cells[i].QL;
    } else
        state.cells[i].fluicol.TR = state.cells[i - 1].fluicol.TR;
}

void advanceCompositionalSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int i) {
    SteadySource source;
    source.tit = 0.;
    ProFlu fluF;

    SteadyFace left;
    left.mHidro = 0.;
    source.residenceTime = 0.;

    readCompositionalSourceProperties(state, i, fluF, source);

    sources::renewSourceTerms(dependencies.sources, i - 1); // checks whether the cell has a source and computes the
    // mass flow rates of produced liquid (oil+water), gas and complementary liquid
    // relation between the source at the left and at the right of a cell:
    state.cells[i].fontemassLL = state.cells[i - 1].fontemassLR;
    state.cells[i].fontemassCL = state.cells[i - 1].fontemassCR;
    state.cells[i].fontemassGL = state.cells[i - 1].fontemassGR;
    left.mComp = state.cells[i].MComp = state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR;
    state.cells[i].MC = state.cells[i - 1].MC + state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassGR;
    if (i == 0) {
        if (state.cells[0].acsr.tipo == kAccessoryRadialPorous)
            state.cells[0].flui.BSW = state.cells[0].acsr.radialPoro.BSW;
        else if (state.cells[0].acsr.tipo == kAccessoryPorous2D)
            state.cells[0].flui.BSW = state.cells[0].acsr.poroso2D.dados.transfer.BSW;
    }

    left.fonteMasLiq = state.cells[i].fontemassLL;
    left.fonteMasGas = state.cells[i].fontemassGL;
    left.vazMasLiq = state.cells[i].MliqiniL;
    left.vazMasGas = state.cells[i].ML - state.cells[i].MliqiniL;

    // maximum and minimum temperatures for a possible re-evaluation of the
    // ASTM method when fluids mix and the dead-oil viscosity model, if it is
    // the ASTM one working with a pair of temperatures, is to be updated
    double temperatureLow = 0;
    double temperatureHigh = 70.;

    left.titV = 0.;
    // computes RS, Bo and Ba in the cell before cell i
    if (state.cells[i - 1].flui.RGO < 1e7) {
        left.solutionGasRatio = state.cells[i - 1].flui.RS(state.cells[i - 1].pres, state.cells[i - 1].temp);
        left.oilVolumeFactor = state.cells[i - 1].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp, left.solutionGasRatio);
        left.waterVolumeFactor = state.cells[i - 1].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        left.solutionGasRatio = left.solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
    } else {
        left.oilVolumeFactor = 1;
        left.solutionGasRatio = 0;
        left.waterVolumeFactor = 0.;
    }
    // in-situ BSW of the previous cell (in the march, cell i)
    state.cells[i - 1].FW = state.cells[i - 1].flui.BSW * left.waterVolumeFactor / (left.oilVolumeFactor + left.waterVolumeFactor * state.cells[i - 1].flui.BSW - state.cells[i - 1].flui.BSW * left.oilVolumeFactor);
    state.cells[i - 1].FWini = state.cells[i - 1].FW;
    // temperature at the boundary between cells i-1 and i: from the second iteration on, one could
    // use the temperature of cell i, already computed, but this can complicate
    // convergence; it is safer to keep the criterion in every iteration, so the temperature at the
    // boundary is taken equal to the temperature of cell i-1
    if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
        left.tmed = (state.cells[i].dx * state.cells[i].temp + state.cells[i].dxL * state.cells[i].tempL) / (state.cells[i].dx + state.cells[i].dxL);
    else
        left.tmed = state.cells[i - 1].temp;
    // first test: no sources in cell i-1:
    if (state.cells[i - 1].acsr.tipo != kAccessoryGasInjection && state.cells[i - 1].acsr.tipo != kAccessoryLiquidInjection && state.cells[i - 1].acsr.tipo != kAccessoryInflowPerformance && state.cells[i - 1].acsr.tipo != kAccessoryMultipleSource && (state.cells[i - 1].acsr.tipo != kAccessoryLeak || (state.cells[i - 1].acsr.tipo == kAccessoryLeak && state.cells[i - 1].acsr.fontechk.abertura <= 1e-6)) &&
        state.cells[i - 1].acsr.tipo != kAccessoryRadialPorous && state.cells[i - 1].acsr.tipo != kAccessoryPorous2D) {
        carryUpstreamCompositionIntoCell(state, left, i);
    } else {
        mixUpstreamSourceIntoCell(state, left, source, i, temperatureHigh, temperatureLow, fluF);
    }
    // mixture mass flow rate update:
    state.cells[i].MC = state.cells[i - 1].MC + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassGR;
    state.cells[i - 1].MR = state.cells[i].MC;
    if (i < state.lastCell)
        state.cells[i + 1].ML = state.cells[i].MC;
    state.cells[i - 1].MRini = state.cells[i - 1].MR;

    double pmed = state.cells[i].presaux + state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2;
    double rhog;
    double rhol;
    // liquid mass flow rate and volumetric flow rates:

    refreshCompositionalFlashFromNeighbour(state, i, pmed, left.tmed);
    double titulo = state.cells[i].flui.dVaporMassFraction;

    ProFlu fluI;
    if (state.cells[i].MC >= 0 && i > 0) {
        fluI = state.cells[i - 1].flui;
    } else {
        fluI = state.cells[i].flui;
    }
    if (state.input.tipoFluido == 0) {
        left.solutionGasRatio = state.cells[i].flui.RS(pmed, left.tmed);
        left.oilVolumeFactor = state.cells[i].flui.BOFunc(pmed, left.tmed, left.solutionGasRatio);
        left.solutionGasRatio = left.solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
        left.waterVolumeFactor = state.cells[i].flui.BAFunc(pmed, left.tmed);

        if (titulo < 1. - 1e-15)
            state.cells[i].FW = state.cells[i].flui.BSW * left.waterVolumeFactor / (left.oilVolumeFactor + left.waterVolumeFactor * state.cells[i].flui.BSW - state.cells[i].flui.BSW * left.oilVolumeFactor);
        else
            state.cells[i].FW = 1.;
        double oilDensity = state.cells[i].flui.MasEspoleo(pmed, left.tmed);
        double waterDensity = state.cells[i].flui.MasEspAgua(pmed, left.tmed);
        double complementaryDensity = state.cells[i].fluicol.MasEspFlu(pmed, left.tmed);
        double denom = complementaryDensity * state.cells[i].bet + (1 - state.cells[i].bet) * state.cells[i].FW * waterDensity;
        if (titulo < 1. - 1e-15) {
            denom += (1 - state.cells[i].bet) * (1. - state.cells[i].FW) * oilDensity / (1. - titulo);
            state.cells[i].QL = state.cells[i].MC / denom;

            state.cells[i].Mliqini = state.cells[i].QL * (state.cells[i].bet * complementaryDensity + (1 - state.cells[i].bet) * (state.cells[i].FW * waterDensity +
                                                                                            (1. - state.cells[i].FW) * oilDensity));
        } else if (state.cells[i].flui.BSW > 1.e-15 || state.cells[i - 1].betI > 1.e-15) {
            state.cells[i].Mliqini = state.cells[i - 1].QL * state.cells[i - 1].rpCi * (1. - state.cells[i - 1].betI) * (1. - left.titV) +
                                state.cells[i - 1].fontemassLR * (1. - source.tit) +
                                state.cells[i - 1].QL * state.cells[i - 1].betI * state.cells[i - 1].rcCi + state.cells[i - 1].fontemassCR;
            state.cells[i].QL = state.cells[i].Mliqini / denom;
        } else {
            state.cells[i].Mliqini = 0.;
            state.cells[i].QL = 0.;
        }
    } else {
        double oilDensity = state.cells[i].flui.MasEspoleo(pmed, left.tmed);
        double complementaryDensity = state.cells[i].fluicol.MasEspFlu(pmed, left.tmed);
        double oilFlowRate = left.mHidro * (1. - titulo) / oilDensity;
        double complementaryFlowRate = left.mComp / complementaryDensity;
        if (fabs(oilFlowRate + complementaryFlowRate) > 1e-15)
            state.cells[i].bet = complementaryFlowRate / (oilFlowRate + complementaryFlowRate);
        else
            state.cells[i].bet = state.cells[i - 1].bet;
        double complementaryFraction = state.cells[i].bet;
        double denom;
        if (titulo < 1. - 1e-15) {
            denom = complementaryDensity * complementaryFraction + (1. - complementaryFraction) * oilDensity;
            state.cells[i].QL = (left.mHidro * (1. - titulo) + left.mComp) / denom;
            state.cells[i].Mliqini = state.cells[i].QL * (state.cells[i].bet * complementaryDensity + (1 - state.cells[i].bet) * oilDensity);
        } else if (complementaryFraction > (1 - 1e-15)) {
            state.cells[i].Mliqini = state.cells[i - 1].QL * state.cells[i - 1].betI * state.cells[i - 1].rcCi + state.cells[i - 1].fontemassCR;
            state.cells[i].QL = state.cells[i].Mliqini / (complementaryDensity * complementaryFraction + (1. - complementaryFraction) * oilDensity);
        } else {
            state.cells[i].Mliqini = 0.;
            state.cells[i].QL = 0.;
        }
    }

    state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;
    if (i < state.lastCell)
        state.cells[i + 1].MliqiniL = state.cells[i].Mliqini;
    rhog = state.cells[i].rgCi = state.cells[i].flui.MasEspGas(pmed, left.tmed);

    // densities at the cell's left interface
    state.cells[i].rpCi = state.cells[i].flui.MasEspLiq(pmed, left.tmed);
    state.cells[i].rcCi = state.cells[i].fluicol.MasEspFlu(pmed, left.tmed);
    rhol = (1 - state.cells[i].bet) * state.cells[i].rpCi + state.cells[i].bet * state.cells[i].rcCi;
    // volumetric flow rates:
    if (i < state.lastCell)
        state.cells[i + 1].QLL = state.cells[i].QL;
    state.cells[i - 1].QLR = state.cells[i].QL;
    state.cells[i].QG = (state.cells[i].MC - state.cells[i].Mliqini) / rhog;
    // Volume fractions:
    applyCompositionalPhaseRegime(state, dependencies, i, rhol, rhog);
    if (fabs(state.cells[i].QL) > 1e-15 && fabs(state.cells[i].MComp) > 1e-15) {
        double hol0 = 1. - state.cells[i - 1].alf;
        double hol1 = 1. - state.cells[i].alf;
        state.cells[i].fluicol.TR = (state.cells[i - 1].fluicol.TR * state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR * source.residenceTime) / state.cells[i].MComp;
        state.cells[i].fluicol.TR = state.cells[i].fluicol.TR + (0.5 * state.cells[i - 1].duto.area * state.cells[i - 1].dx * hol0 +
                                                       0.5 * state.cells[i].duto.area * state.cells[i].dx * hol1) /
                                                          state.cells[i].QL;
    } else
        state.cells[i].fluicol.TR = state.cells[i - 1].fluicol.TR;
}

void advanceSteadyMass(const SteadyStateState &state, const SteadyDependencies &dependencies, int i) {

    if (state.cells[i - 1].acsr.tipo == kAccessoryGasInjection && state.cells[i - 1].acsr.injg.QGas < 0) {
        state.cells[i - 1].acsr.injg.FluidoPro = state.cells[i - 1].flui;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryLiquidInjection && state.cells[i - 1].acsr.injl.QLiq < 0) {
        state.cells[i - 1].acsr.injl.FluidoPro = state.cells[i - 1].flui;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryMultipleSource && state.cells[i - 1].acsr.injm.MassC + state.cells[i - 1].acsr.injm.MassG + state.cells[i - 1].acsr.injm.MassP < 0) {
        state.cells[i - 1].acsr.injm.FluidoPro = state.cells[i - 1].flui;
    }

    sources::renewSourceTerms(dependencies.sources, i - 1); // checks whether the cell has a source and computes the
    // mass flow rates of produced liquid (oil+water), gas and complementary liquid
    // relation between the source at the left and at the right of a cell:
    state.cells[i].fontemassLL = state.cells[i - 1].fontemassLR;
    state.cells[i].fontemassCL = state.cells[i - 1].fontemassCR;
    state.cells[i].fontemassGL = state.cells[i - 1].fontemassGR;
    state.cells[i].MComp = state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR;

    double residenceTimeSource = 0.;
    double sinalQ = 1.;
    if (i > 1) {
        if (fabs(state.cells[i - 1].QL + state.cells[i - 1].QG) > 1e-15)
            sinalQ = (state.cells[i - 1].QL + state.cells[i - 1].QG) / fabs(state.cells[i - 1].QL + state.cells[i - 1].QG);
    } else {
        if (fabs(state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassGR) > 1e-15)
            sinalQ = (state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassGR) /
                     fabs(state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassGR);
    }
    double oilVolumeFactorInSitu;
    double waterVolumeFactorInSitu;
    double waterCutInSitu = 0;
    // maximum and minimum temperatures for a possible re-evaluation of the
    // ASTM method when fluids mix and the dead-oil viscosity model, if it is
    // the ASTM one working with a pair of temperatures, is to be updated
    double temperatureLow;
    if (state.input.flashCompleto == 1 && (state.input.tabent.tmin - 0) > (*state.globals).smallValueTolerance)
        temperatureLow = state.input.tabent.tmin + 0.1;
    else
        temperatureLow = 0.;
    double temperatureHigh;
    if (state.input.flashCompleto == 1 && (70 - state.input.tabent.tmax) < (*state.globals).smallValueTolerance)
        temperatureHigh = state.input.tabent.tmax - 0.1;
    else
        temperatureHigh = 70.;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double solutionGasRatio;
    // computes RS, Bo and Ba in the cell before cell i
    if (state.cells[i - 1].flui.RGO < 1e7) {
        solutionGasRatio = state.cells[i - 1].flui.RS(state.cells[i - 1].pres, state.cells[i - 1].temp);
        oilVolumeFactor = state.cells[i - 1].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp, solutionGasRatio);
        waterVolumeFactor = state.cells[i - 1].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        solutionGasRatio = solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
    } else {
        oilVolumeFactor = 1;
        solutionGasRatio = 0;
        waterVolumeFactor = 0.;
    }
    // in-situ BSW of the previous cell (in the march, cell i)
    state.cells[i - 1].FW = state.cells[i - 1].flui.BSW * waterVolumeFactor / (oilVolumeFactor + waterVolumeFactor * state.cells[i - 1].flui.BSW - state.cells[i - 1].flui.BSW * oilVolumeFactor);
    state.cells[i - 1].FWini = state.cells[i - 1].FW;
    double tmed;
    // temperature at the boundary between cells i-1 and i: from the second iteration on, one could
    // use the temperature of cell i, already computed, but this can complicate
    // convergence; it is safer to keep the criterion in every iteration, so the temperature at the
    // boundary is taken equal to the temperature of cell i-1
    if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
        tmed = (state.cells[i].dx * state.cells[i].temp + state.cells[i].dxL * state.cells[i].tempL) / (state.cells[i].dx + state.cells[i].dxL);
    else
        tmed = state.cells[i - 1].temp;
    // first test: no sources in cell i-1:
    if (state.cells[i - 1].acsr.tipo != kAccessoryGasInjection && state.cells[i - 1].acsr.tipo != kAccessoryLiquidInjection && state.cells[i - 1].acsr.tipo != kAccessoryInflowPerformance && state.cells[i - 1].acsr.tipo != kAccessoryMultipleSource && (state.cells[i - 1].acsr.tipo != kAccessoryLeak || (state.cells[i - 1].acsr.tipo == kAccessoryLeak && state.cells[i - 1].acsr.fontechk.abertura <= 1e-6)) && state.cells[i - 1].acsr.tipo != kAccessoryRadialPorous && state.cells[i - 1].acsr.tipo != kAccessoryPorous2D) {
        applySteadyMassWithoutSource(state, i, oilVolumeFactor, solutionGasRatio, tmed, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    }
    // a gas source in cell i-1, which changes the GOR and the gas density in i
    else if (state.cells[i - 1].acsr.tipo == kAccessoryGasInjection && state.cells[i - 1].acsr.injg.seco == 1) {
        applySteadyMassDryGasInjection(state, i, oilVolumeFactor, solutionGasRatio, tmed, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryGasInjection && state.cells[i - 1].acsr.injg.seco == 0) {
        applySteadyMassWetGasInjection(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, waterVolumeFactor, solutionGasRatio, tmed, residenceTimeSource, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    }
    // a liquid source in cell i-1:
    else if (state.cells[i - 1].acsr.tipo == kAccessoryLiquidInjection) {
        applySteadyMassLiquidInjection(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, waterVolumeFactor, solutionGasRatio, tmed, residenceTimeSource, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    }
    // an IPR in cell i-1:
    else if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
        applySteadyMassInflowPerformance(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, waterVolumeFactor, solutionGasRatio, tmed, residenceTimeSource, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    }
    // a mass source in cell i-1:
    else if (state.cells[i - 1].acsr.tipo == kAccessoryMultipleSource) {
        applySteadyMassMultipleSource(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, waterVolumeFactor, solutionGasRatio, tmed, residenceTimeSource, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    }
    // special case, a leak:
    else if (state.cells[i - 1].acsr.tipo == kAccessoryLeak && state.cells[i - 1].acsr.fontechk.abertura > 1e-6) {
        applySteadyMassLeakSource(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, solutionGasRatio, tmed, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryRadialPorous) {
        applySteadyMassRadialPorous(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, waterVolumeFactor, solutionGasRatio, tmed, residenceTimeSource, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryPorous2D) {
        applySteadyMassPorous2D(state, i, temperatureLow, temperatureHigh, oilVolumeFactor, waterVolumeFactor, solutionGasRatio, tmed, residenceTimeSource, oilVolumeFactorInSitu, waterVolumeFactorInSitu, waterCutInSitu);
    }

    // mixture mass flow rate update:
    state.cells[i].MC = state.cells[i - 1].MC + state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassGR;
    state.cells[i - 1].MR = state.cells[i].MC;
    if (i < state.lastCell)
        state.cells[i + 1].ML = state.cells[i].MC;
    state.cells[i - 1].MRini = state.cells[i - 1].MR;
    double pmed = state.cells[i].presaux;
    double rhog;
    double rhol = 1000.;
    // liquid mass flow rate and volumetric flow rates:
    if (state.cells[i].flui.RGO < 1e7) { // With liquid
        finalizeSteadyMassWithLiquid(state, i, waterCutInSitu, tmed, pmed, rhog, rhol);
    } else {
        state.cells[i].Mliqini = 0.;
        state.cells[i - 1].MliqiniR = state.cells[i].Mliqini;
        if (i < state.lastCell)
            state.cells[i + 1].MliqiniL = state.cells[i].Mliqini;
        state.cells[i].QL = 0.;
        if (i < state.lastCell)
            state.cells[i + 1].QLL = state.cells[i].QL;
        state.cells[i - 1].QLR = state.cells[i].QL;
        pmed = state.cells[i].presaux + state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2;
        state.cells[i].rpCi = state.cells[i].flui.MasEspLiq(pmed, tmed);
        state.cells[i].rcCi = state.cells[i].fluicol.MasEspFlu(pmed, tmed);
        rhog = state.cells[i].rgCi = state.cells[i].flui.MasEspGas(pmed, tmed);
        state.cells[i].QG = (state.cells[i].MC) / rhog;
    }
    // Volume fractions:
    if (fabs(state.cells[i].QG + state.cells[i].QL) < (*state.globals).smallValueTolerance) {
        finalizeSteadyMassNoFlow(state, i);
    } else if (fabs(state.cells[i].QG) < (*state.globals).smallValueTolerance) { // liquid only:
        finalizeSteadyMassLiquidOnly(state, i);
    } else if (fabs(state.cells[i].QL) < (*state.globals).smallValueTolerance * 1e-6) { // gas only:
        finalizeSteadyMassGasOnly(state, i);
    } else { // two-phase
        finalizeSteadyMassTwoPhase(state, dependencies, i, rhog, rhol);
    }

    if (fabs(state.cells[i].QL) > 1e-15 && fabs(state.cells[i].MComp) > 1e-15) {
        double hol0 = 1. - state.cells[i - 1].alf;
        double hol1 = 1. - state.cells[i].alf;
        state.cells[i].fluicol.TR = (state.cells[i - 1].fluicol.TR * state.cells[i - 1].MComp + state.cells[i - 1].fontemassCR * residenceTimeSource) / state.cells[i].MComp;
        state.cells[i].fluicol.TR = state.cells[i].fluicol.TR + (0.5 * state.cells[i - 1].duto.area * state.cells[i - 1].dx * hol0 +
                                                       0.5 * state.cells[i].duto.area * state.cells[i].dx * hol1) /
                                                          state.cells[i].QL;
    } else
        state.cells[i].fluicol.TR = state.cells[i - 1].fluicol.TR;
}

double areaChangePressureDrop(const SteadyStateState &state, int i, double rhomix, double reynolds, double jmix) {
    double dpArea = 0.;
    if ((state.cells[i].duto.area != state.cells[i].dutoR.area && reynolds > 2400) && (state.cells[i].mudaArea == 1 && (fabs(jmix) >= 0.1))) {
        double areaMenor;
        double areaMaior;
        double bernou;
        double perdLoc;
        bernou = 0.5 * (state.cells[i].MR * state.cells[i].MR / (state.cells[i].duto.area * state.cells[i].duto.area * rhomix)) *
                 (1. - pow(state.cells[i].duto.area / state.cells[i].dutoR.area, 2.));
        if (state.cells[i].duto.area > state.cells[i].dutoR.area) {
            areaMenor = state.cells[i].dutoR.area;
            areaMaior = state.cells[i].duto.area;
            if (state.cells[i].MR > 0.) {
                perdLoc = 0.42 * 0.5 * (state.cells[i].MR * state.cells[i].MR / (state.cells[i].duto.area * state.cells[i].duto.area * rhomix)) *
                          (1. - pow(state.cells[i].duto.area / state.cells[i].dutoR.area, 2.));
            } else {
                double expanse = (1. - pow(areaMenor / areaMaior, 2.));
                perdLoc = 0.5 * (state.cells[i].MR * state.cells[i].MR / (areaMenor * areaMenor * rhomix)) * expanse * expanse;
            }
        } else {
            areaMenor = state.cells[i].duto.area;
            areaMaior = state.cells[i].dutoR.area;
            if (state.cells[i].MR < 0.) {
                perdLoc = 0.42 * 0.5 * (state.cells[i].MR * state.cells[i].MR / (state.cells[i].duto.area * state.cells[i].duto.area * rhomix)) *
                          (1. - pow(state.cells[i].duto.area / state.cells[i].dutoR.area, 2.));
            } else {
                double expanse = (1. - pow(areaMenor / areaMaior, 2.));
                perdLoc = -0.5 * (state.cells[i].MR * state.cells[i].MR / (areaMenor * areaMenor * rhomix)) * expanse * expanse;
            }
        }
        dpArea = bernou + perdLoc;
    }
    return dpArea / units::kPascalPerKgfPerCm2;
}

double steadyPressureAtLastCell(const SteadyStateState &state) {

    double dx = 0.5 * state.cells[state.lastCell].dx;
    double diameter = state.cells[state.lastCell].duto.a;
    double area = 0.25 * std::numbers::pi * diameter * diameter;
    double perimeter = state.cells[state.lastCell].duto.peri;
    double alfmed = state.cells[state.lastCell].alf;
    double betmed = state.cells[state.lastCell].bet;
    double rhog = state.cells[state.lastCell].flui.MasEspGas(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp);
    double rhol = (1. - betmed) * state.cells[state.lastCell].flui.MasEspLiq(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp) + betmed * state.cells[state.lastCell].fluicol.MasEspFlu(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp);
    double ugsmed = (state.cells[state.lastCell].MC - state.cells[state.lastCell].Mliqini) / (area * rhog);
    double ulsmed = state.cells[state.lastCell].Mliqini / (area * rhol);

    double j = ugsmed + ulsmed;
    double sinalJ = 1.;
    if (fabs(j) > 1e-15)
        sinalJ = j / fabs(j);

    double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
    double viscmix = alfmed * state.cells[state.lastCell].flui.ViscGas(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp) + (1 - alfmed) * ((1. - betmed) * state.cells[state.lastCell].flui.ViscOleo(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp) + betmed * state.cells[state.lastCell].fluicol.VisFlu(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp));

    double mixtureReynolds;
    if (state.cells[state.lastCell].duto.revest == 0)
        mixtureReynolds = state.cells[state.lastCell].Rey(state.cells[state.lastCell].duto.a, j, rhomix, viscmix);
    else {
        double dhid = 4 * area / perimeter;
        mixtureReynolds = state.cells[state.lastCell].Rey(dhid, j, rhomix, viscmix);
    }
    double frictionFactor = state.cells[state.lastCell].fric(mixtureReynolds, state.cells[state.lastCell].duto.rug / diameter);
    double gradfric = state.cells[state.lastCell].dPdLFric * (0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area);
    double gradhidro = state.cells[state.lastCell].dPdLHidro * (kGravity * sin(state.cells[state.lastCell].duto.teta) * rhomix * dx);
    return -(1. * gradfric + gradhidro) / units::kPascalPerKgfPerCm2;
}

void advanceUpstreamSteadyPressure(const SteadyStateState &state, int i, int rungeKuttaStage) {

    if (state.input.tipoModeloDrift == 1) {
        double dx = 0.5 * state.cells[i].dxL;
        double diameter = state.cells[i].dutoL.a;
        double area = 0.25 * std::numbers::pi * diameter * diameter;
        double perimeter = state.cells[i].dutoL.peri;
        if (rungeKuttaStage == 0) {
            double alfmed = state.cells[i - 1].alf;
            double betmed = state.cells[i - 1].bet;
            double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i - 1].pres, state.cells[i - 1].temp);
            double rhol = (1. - betmed) * state.cells[i - 1].flui.MasEspLiq(state.cells[i - 1].pres, state.cells[i - 1].temp) + betmed * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i - 1].pres, state.cells[i - 1].temp);
            double ugsmed = (state.cells[i - 1].MC - state.cells[i - 1].Mliqini) / (area * rhog);
            double ulsmed = state.cells[i - 1].Mliqini / (area * rhol);
            double j = ugsmed + ulsmed;
            double sinalJ = 1.;
            if (fabs(j) > 1e-15)
                sinalJ = j / fabs(j);

            double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
            double visl = ((1. - betmed) * state.cells[i - 1].flui.ViscOleo(state.cells[i - 1].pres, state.cells[i - 1].temp) + betmed * state.cells[i - 1].fluicol.VisFlu(state.cells[i - 1].pres, state.cells[i - 1].temp));
            double viscmix = alfmed * state.cells[i - 1].flui.ViscGas(state.cells[i - 1].pres, state.cells[i - 1].temp) + (1 - alfmed) * visl;

            double mixtureReynolds;
            if (state.cells[i].dutoL.revest == 0)
                mixtureReynolds = state.cells[i - 1].Rey(state.cells[i].dutoL.a, j, rhomix, viscmix);
            else {
                double dhid = 4 * area / perimeter;
                mixtureReynolds = state.cells[i - 1].Rey(dhid, j, rhomix, viscmix);
            }
            double frictionFactor = state.cells[i - 1].fric(mixtureReynolds, state.cells[i].dutoL.rug / diameter);
            if (i > 1 && state.cells[i - 1].fluicol.tipoF == 2) {
                frictionFactor *= (1 - state.cells[i - 1].dR);
            }
            double gradfric = state.cells[i - 1].dPdLFric * (0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area);
            double gradhidro = state.cells[i - 1].dPdLHidro * (kGravity * sin(state.cells[i].dutoL.teta) * rhomix * dx);
            state.cells[i].presaux = state.cells[i - 1].pres - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2;

            state.cells[i].termoHidro = sinalJ * gradhidro / dx;
            state.cells[i].termoFric = gradfric / dx;
        } else {
            double alfmed = state.cells[i].alf;
            double betmed = state.cells[i].bet;
            double razdx = state.cells[i].dx / (state.cells[i].dx + state.cells[i].dxL);
            double tmed = razdx * state.cells[i].temp + (1. - razdx) * state.cells[i - 1].temp;
            double pmed = state.cells[i].presaux;
            double rhog = state.cells[i - 1].flui.MasEspGas(pmed, tmed);
            double rhol = (1. - betmed) * state.cells[i - 1].flui.MasEspLiq(pmed, tmed) + betmed * state.cells[i - 1].fluicol.MasEspFlu(pmed, tmed);
            double ugsmed = (state.cells[i].QG) / (area);
            double ulsmed = state.cells[i].QL / (area);
            double j = ugsmed + ulsmed;
            double sinalJ = 1.;
            if (fabs(j) > 1e-15)
                sinalJ = j / fabs(j);

            double rhomix = alfmed * rhog + (1 - alfmed) * rhol;
            double viscmix = alfmed * state.cells[i - 1].flui.ViscGas(pmed, tmed) + (1 - alfmed) * ((1. - betmed) * state.cells[i - 1].flui.ViscOleo(pmed, tmed) + betmed * state.cells[i - 1].fluicol.VisFlu(pmed, tmed));

            double mixtureReynolds;
            if (state.cells[i].dutoL.revest == 0)
                mixtureReynolds = state.cells[i - 1].Rey(state.cells[i].dutoL.a, j, rhomix, viscmix);
            else {
                double dhid = 4 * area / perimeter;
                mixtureReynolds = state.cells[i - 1].Rey(dhid, j, rhomix, viscmix);
            }
            double frictionFactor = state.cells[i - 1].fric(mixtureReynolds, state.cells[i].dutoL.rug / diameter);
            double gradfric = state.cells[i - 1].dPdLFric * (0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area);
            double gradhidro = state.cells[i - 1].dPdLHidro * (kGravity * sin(state.cells[i].dutoL.teta) * rhomix * dx);
            state.cells[i].presaux = state.cells[i - 1].pres - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2;

            state.cells[i].termoHidro = sinalJ * gradhidro / dx;
            state.cells[i].termoFric = gradfric / dx;
        }
    } else {
        double holdup;
        double frictionGrad;
        double gravityGrad;
        double totalGrad;
        double reynolds;
        unsigned char flowType;

        double dx = 0.5 * state.cells[i].dxL;
        executarCorrelacao(state.cells, i, 1, state.input.AceleraConvergPerm,
                           state.cells[i - 1].correlacaoMR2,
                           holdup, frictionGrad, gravityGrad, totalGrad,
                           reynolds, flowType);

        double gradfric = state.cells[i - 1].dPdLFric * frictionGrad * 22620.6 * dx;
        double gradhidro = state.cells[i - 1].dPdLHidro * gravityGrad * 22620.6 * dx;
        state.cells[i].presaux = state.cells[i - 1].pres - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2;

        state.cells[i].termoHidro = gradhidro / dx;
        state.cells[i].termoFric = gradfric / dx;
    }
}

void advanceDownstreamSteadyPressure(const SteadyStateState &state, int i, int rungeKuttaStage) {

    double dx;
    double diameter;
    double area;
    double perimeter;
    double alfmed;
    double betmed;
    double rhog;
    double rhol;
    double ugsmed;
    double ulsmed;
    double j;

    double rhomix;
    double viscmix;
    double mixtureReynolds;
    double frictionFactor;
    double gradfric;
    double gradhidro;

    double tmed;

    double dpArea = 0.;

    dx = 0.5 * state.cells[i].dx;
    diameter = state.cells[i].duto.a;
    area = 0.25 * std::numbers::pi * diameter * diameter;
    perimeter = state.cells[i].duto.peri;

    if (state.input.tipoModeloDrift == 1) {
        if (rungeKuttaStage == 0) {
            alfmed = state.cells[i].alf;
            betmed = state.cells[i].bet;
            double razdx = state.cells[i].dx / (state.cells[i].dx + state.cells[i].dxL);
            if (state.input.AceleraConvergPerm == 0)
                tmed = razdx * state.cells[i].temp + (1. - razdx) * state.cells[i - 1].temp;
            else
                tmed = state.cells[i - 1].temp;
            double pmed = state.cells[i].presaux + state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2;
            rhog = state.cells[i].rgCi;
            rhol = (1. - betmed) * state.cells[i].rpCi + betmed * state.cells[i].rcCi;
            ugsmed = (state.cells[i].QG) / (area);
            ulsmed = state.cells[i].QL / (area);
            j = ugsmed + ulsmed;
            double sinalJ = 1.;
            if (fabs(j) > 1e-15)
                sinalJ = j / fabs(j);

            rhomix = alfmed * rhog + (1 - alfmed) * rhol;
            double visl = ((1. - betmed) * state.cells[i].flui.ViscOleo(pmed, tmed) + betmed * state.cells[i].fluicol.VisFlu(pmed, tmed));
            viscmix = alfmed * state.cells[i].flui.ViscGas(pmed, tmed) + (1 - alfmed) * visl;

            if (state.cells[i].duto.revest == 0)
                mixtureReynolds = state.cells[i].Rey(state.cells[i].duto.a, j, rhomix, viscmix);
            else {
                double dhid = 4 * area / perimeter;
                mixtureReynolds = state.cells[i].Rey(dhid, j, rhomix, viscmix);
            }
            frictionFactor = state.cells[i].fric(mixtureReynolds, state.cells[i].duto.rug / diameter);
            if (state.cells[i].fluicol.tipoF == 2) {
                double liquidReynolds;
                if (fabs(state.cells[i].MComp) > 1e-15) {
                    double ulmed;
                    if (alfmed < 1 - 1e-15)
                        ulmed = ulsmed / (1 - alfmed);
                    else
                        ulmed = 0.;
                    if (state.cells[i].duto.revest == 0)
                        liquidReynolds = state.cells[i].Rey(state.cells[i].duto.a, ulmed, rhol, visl);
                    else {
                        double dhid = 4 * area / perimeter;
                        liquidReynolds = state.cells[i].Rey(dhid, ulmed, rhol, visl);
                    }
                    state.cells[i].dR = state.cells[i].fluicol.calcDR(liquidReynolds);
                } else
                    state.cells[i].dR = 0.;
                frictionFactor *= (1 - state.cells[i].dR);
            }
            gradfric = state.cells[i].dPdLFric * (0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area);
            gradhidro = state.cells[i].dPdLHidro * (kGravity * sin(state.cells[i].duto.teta) * rhomix * dx);
            if (state.cells[i].mudaArea == 1)
                dpArea = areaChangePressureDrop(state, i - 1, rhomix, mixtureReynolds, fabs(j));

            state.cells[i].pres = pmed - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2 + dpArea;
        } else {
            alfmed = state.cells[i].alf;
            betmed = state.cells[i].bet;
            tmed = state.cells[i].temp;
            double pmed = state.cells[i].pres;
            rhog = state.cells[i].flui.MasEspGas(pmed, tmed);
            rhol = (1. - betmed) * state.cells[i].flui.MasEspLiq(pmed, tmed) + betmed * state.cells[i].fluicol.MasEspFlu(pmed, tmed);
            ugsmed = (state.cells[i].MC - state.cells[i].Mliqini) / (area * rhog);
            ulsmed = state.cells[i].Mliqini / (area * rhol);
            j = ugsmed + ulsmed;
            double sinalJ = 1.;
            if (fabs(j) > 1e-15)
                sinalJ = j / fabs(j);

            rhomix = alfmed * rhog + (1 - alfmed) * rhol;
            viscmix = alfmed * state.cells[i].flui.ViscGas(pmed, tmed) + (1 - alfmed) * ((1. - betmed) * state.cells[i].flui.ViscOleo(pmed, tmed) + betmed * state.cells[i].fluicol.VisFlu(pmed, tmed));

            if (state.cells[i].duto.revest == 0)
                mixtureReynolds = state.cells[i].Rey(state.cells[i].duto.a, j, rhomix, viscmix);
            else {
                double dhid = 4 * area / perimeter;
                mixtureReynolds = state.cells[i].Rey(dhid, j, rhomix, viscmix);
            }
            frictionFactor = state.cells[i].fric(mixtureReynolds, state.cells[i].duto.rug / diameter);
            gradfric = state.cells[i].dPdLFric * (0.5 * frictionFactor * rhomix * (fabs(j) * j) * perimeter * dx / area);
            gradhidro = state.cells[i].dPdLHidro * (kGravity * sin(state.cells[i].duto.teta) * rhomix * dx);
            if (state.cells[i].mudaArea == 1)
                dpArea = areaChangePressureDrop(state, i - 1, rhomix, mixtureReynolds, fabs(j));

            state.cells[i].pres = state.cells[i].presaux + state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2 - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2 + dpArea;
        }
    } else {
        double holdup;
        double frictionGrad;
        double gravityGrad;
        double totalGrad;
        double reynolds;
        unsigned char flowType;
        executarCorrelacao(state.cells, i, 2, state.input.AceleraConvergPerm,
                           state.cells[i - 1].correlacaoMR2,
                           holdup, frictionGrad, gravityGrad, totalGrad,
                           reynolds, flowType);

        double gradfric = state.cells[i - 1].dPdLFric * frictionGrad * 22620.6 * dx;
        double gradhidro = state.cells[i - 1].dPdLHidro * gravityGrad * 22620.6 * dx;
        state.cells[i].pres = state.cells[i].presaux + state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2 - (gradfric + gradhidro) / units::kPascalPerKgfPerCm2;

        state.cells[i].termoHidro = gradhidro / dx;
        state.cells[i].termoFric = gradfric / dx;
    }
}

void advanceSteadyMassTransfer(const SteadyStateState &state, int i) {

    double waterCutLocal;
    double waterCutLeftCell;

    double razdx = state.cells[i].dxR / (state.cells[i].dx + state.cells[i].dxR);
    double razdxL = state.cells[i].dx / (state.cells[i].dx + state.cells[i].dxL);
    double tmed;
    tmed = razdx * state.cells[i + 1].temp + (1 - razdx) * state.cells[i].temp;
    double tmed0;
    if (i > 1) {
        tmed0 = razdxL * state.cells[i].temp + (1 - razdxL) * state.cells[i - 1].temp;
    } else
        tmed0 = state.cells[i].temp;

    double oilVolumeFactorLocal = state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp);
    double waterVolumeFactorLocal = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);
    waterCutLocal = state.cells[i].flui.BSW * waterVolumeFactorLocal / (oilVolumeFactorLocal + waterVolumeFactorLocal * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorLocal);
    double solutionGasRatioRightFace = state.cells[i].flui.RS(state.cells[i + 1].presaux, tmed);
    double oilVolumeFactorRightFace = state.cells[i].flui.BOFunc(state.cells[i + 1].presaux, tmed);
    double oilVolumeFactorLeftCell;
    double waterVolumeFactorLeftCell;
    double solutionGasRatioLeftFace;
    double oilVolumeFactorLeftFace;
    double dengD = state.cells[i].flui.Deng;
    double dengE;
    double dissolvedGasGravityRatio = state.cells[i].flui.rDgD;
    double freeGasGravityRatio;
    if (i > 0) {
        oilVolumeFactorLeftCell = state.cells[i - 1].flui.BOFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterVolumeFactorLeftCell = state.cells[i - 1].flui.BAFunc(state.cells[i - 1].pres, state.cells[i - 1].temp);
        waterCutLeftCell = state.cells[i - 1].flui.BSW * waterVolumeFactorLeftCell / (oilVolumeFactorLeftCell + waterVolumeFactorLeftCell * state.cells[i - 1].flui.BSW - state.cells[i - 1].flui.BSW * oilVolumeFactorLeftCell);
        solutionGasRatioLeftFace = state.cells[i - 1].flui.RS(state.cells[i].presaux, tmed0);
        oilVolumeFactorLeftFace = state.cells[i - 1].flui.BOFunc(state.cells[i].presaux, tmed0);
        state.cells[i - 1].flui.BAFunc(state.cells[i].presaux, tmed0);
        dengE = state.cells[i - 1].flui.Deng;
        freeGasGravityRatio = state.cells[i - 1].flui.rDgD;
    } else {
        oilVolumeFactorLeftCell = state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp);
        waterVolumeFactorLeftCell = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);

        waterCutLeftCell = state.cells[i].flui.BSW * waterVolumeFactorLeftCell / (oilVolumeFactorLeftCell + waterVolumeFactorLeftCell * state.cells[i].flui.BSW - state.cells[i].flui.BSW * oilVolumeFactorLeftCell);
        solutionGasRatioLeftFace = state.cells[i].flui.RS(state.cells[i].presaux, tmed0);
        oilVolumeFactorLeftFace = state.cells[i].flui.BOFunc(state.cells[i].presaux, tmed0);
        state.cells[i].flui.BAFunc(state.cells[i].presaux, tmed0);
        dengE = state.cells[i].flui.Deng;
        freeGasGravityRatio = state.cells[i].flui.rDgD;
    }
    double betI;
    double betL;
    if (i < state.lastCell) {
        betI = state.cells[i].betPigD;
        if (state.cells[i + 1].Mliqini < 0)
            betI = state.cells[i + 1].betPigE;
    } else
        betI = state.cells[i].betPigD;
    if (i > 0) {
        betL = state.cells[i - 1].betPigD;
        if (state.cells[i].Mliqini < 0)
            betL = state.cells[i].betPigE;
    } else
        betL = state.cells[i].betPigE;

    if (state.cells[i].acsr.tipo == kAccessoryNone)
        state.cells[i].transmassR = (-(state.cells[i + 1].QL * (1. - betI) * dissolvedGasGravityRatio * dengD * kAirDensityAtStandardConditions * (1. - waterCutLocal) * solutionGasRatioRightFace * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / oilVolumeFactorRightFace) + (state.cells[i].QL * (1. - betL) * freeGasGravityRatio * dengE * kAirDensityAtStandardConditions * (1. - waterCutLeftCell) * solutionGasRatioLeftFace * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / oilVolumeFactorLeftFace));
    else
        state.cells[i].transmassR = 0.;

    state.cells[i].transmassR /= state.cells[i].dx;
    state.cells[i + 1].transmassL = state.cells[i].transmassR;
    state.cells[i].FonteMudaFase = state.cells[i].transmassR;
    if (i == state.lastCell - 1) {
        state.cells[i + 1].transmassR = state.cells[i].transmassR;
        state.cells[i + 1].FonteMudaFase = state.cells[i].transmassR;
    }
}

void advanceSteadyGasMassTransfer(const SteadyStateState &state, int i) {

    if (state.cells[i].acsr.tipo == kAccessoryNone && fabs(state.cells[i + 1].flui.dVaporMassFraction - state.cells[i].flui.dVaporMassFraction) < 0.2) {
        state.cells[i].transmassR = ((state.cells[i + 1].MC - state.cells[i + 1].Mliqini) -
                                (state.cells[i].MC - state.cells[i].Mliqini));
    } else
        state.cells[i].transmassR = 0.;

    state.cells[i].transmassR /= state.cells[i].dx;
    state.cells[i + 1].transmassL = state.cells[i].transmassR;
    state.cells[i].FonteMudaFase = state.cells[i].transmassR;
    if (i == state.lastCell - 1) {
        state.cells[i + 1].transmassR = state.cells[i].transmassR;
        state.cells[i + 1].FonteMudaFase = state.cells[i].transmassR;
    }
}

void refreshProperties(const SteadyStateState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        state.cells[i].rpC = state.cells[i].rpCi =
            state.cells[i].flui.MasEspLiq(state.cells[i].pres, state.cells[i].temp);
        state.cells[i].rgC = state.cells[i].rgCi =
            state.cells[i].flui.MasEspGas(state.cells[i].pres, state.cells[i].temp);
        state.cells[i].rcC = state.cells[i].rcCi =
            state.cells[i].fluicol.MasEspFlu(state.cells[i].pres, state.cells[i].temp);
        state.cells[i].rpL = state.cells[i].rpLi =
            state.cells[i].flui.MasEspLiq(state.cells[i].presL, state.cells[i].tempL);
        state.cells[i].rgL = state.cells[i].rgLi =
            state.cells[i].flui.MasEspGas(state.cells[i].presL, state.cells[i].tempL);
        state.cells[i].rcL = state.cells[i].rcLi =
            state.cells[i].fluicol.MasEspFlu(state.cells[i].presL, state.cells[i].tempL);

        state.cells[i].mipC = state.cells[i].flui.ViscOleo(state.cells[i].pres, state.cells[i].temp);
        state.cells[i].migC = state.cells[i].flui.ViscGas(state.cells[i].pres, state.cells[i].temp);
        state.cells[i].micC = state.cells[i].fluicol.VisFlu(state.cells[i].pres, state.cells[i].temp);

        if (i > 0) {
            state.cells[i - 1].rpR = state.cells[i - 1].rpRi = state.cells[i].rpC;
            state.cells[i - 1].rgR = state.cells[i - 1].rgRi = state.cells[i].rgC;
            state.cells[i - 1].rcR = state.cells[i - 1].rcRi = state.cells[i].rcC;

            state.cells[i - 1].mipR = state.cells[i].mipC;
            state.cells[i - 1].migR = state.cells[i].migC;
            state.cells[i - 1].micR = state.cells[i].micC;
        }
        if (i == state.lastCell) {
            state.cells[i].rpR = state.cells[i].rpRi = state.cells[i].rpC;
            state.cells[i].rgR = state.cells[i].rgRi = state.cells[i].rgC;
            state.cells[i].rcR = state.cells[i].rcRi = state.cells[i].rcC;

            state.cells[i].mipR = state.cells[i].mipC;
            state.cells[i].migR = state.cells[i].migC;
            state.cells[i].micR = state.cells[i].micC;
        }
    }
}

void refreshSteadyThermalVelocities(const SteadyStateState &state) {
    for (int i = 0; i <= state.lastCell; i++) {
        double diameter = state.cells[i].duto.a;
        double area = 0.25 * std::numbers::pi * diameter * diameter;
        double alfmed = state.cells[i].alf;
        double betmed = state.cells[i].bet;
        double rhol = (1. - betmed) * state.cells[i].rpC + betmed * state.cells[i].rcC;
        double rhog = state.cells[i].rgC;
        double liquidSpecificHeat = (1. - betmed) * state.cells[i].flui.CalorLiq(state.cells[i].presini, state.cells[i].temp) + betmed * state.cells[i].fluicol.CalorLiq(state.cells[i].presini, state.cells[i].temp);
        double liquidSpecificHeatVolume = liquidSpecificHeat;
        double gasSpecificHeat = state.cells[i].flui.CalorGas(state.cells[i].presini, state.cells[i].temp);
        double gasSpecificHeatVolume = state.cells[i].flui.CalorGasVolMod(state.cells[i].presini, state.cells[i].temp, state.cells[i].rgC);
        double coefTempo = (rhol * (1 - alfmed) * liquidSpecificHeatVolume + rhog * alfmed * gasSpecificHeatVolume) * area;
        double ugsmed = state.cells[i].QG / area;
        double ulsmed = state.cells[i].QL / area;
        double coefdxT = (rhol * ulsmed * liquidSpecificHeat + rhog * ugsmed * gasSpecificHeat) * area;
        state.cells[i].VTemper = coefdxT / coefTempo;
    }
}

void computePseudoTransientTimeStep(const SteadyStateState &state) {
    state.timeStep = 100.;
    for (int i = 0; i <= state.lastCell; i++) {
        double jmix = 0.;
        double dtaux;
        if (fabs(state.cells[i].VTemper) > jmix)
            jmix = fabs(state.cells[i].VTemper);
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
    state.timeStep = 0.8 * state.timeStep;
    for (int i = 0; i <= state.lastCell; i++)
        state.cells[i].dt = state.timeStep;
    if (state.input.lingas == 1) {
        for (int i = 0; i <= state.lastGasCell; i++)
            state.gasCells[i].dt = state.timeStep;
    }
}

void refreshUpstreamProductionPeriphery(const SteadyStateState &state, int i) {
    if (state.cells[i].presaux < 0.5)
        state.cells[i].presaux = 0.5;
    state.cells[i - 1].presauxR = state.cells[i].presaux;
    if (i < state.lastCell)
        state.cells[i + 1].presauxL = state.cells[i].presaux;

    state.cells[i - 1].acsr.dpB = 0.;
    state.cells[i - 1].acsr.potB = 0.;
    state.cells[i - 1].acsr.potBT = 0.;
    double tmed;
    if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
        tmed = (state.cells[i].dx * state.cells[i].temp + state.cells[i].dxL * state.cells[i].tempL) / (state.cells[i].dx + state.cells[i].dxL);
    else
        tmed = state.cells[i - 1].temp;
    double sinalQ = 1.;
    if (fabs(state.cells[i - 1].QL + state.cells[i - 1].QG) > 1e-15)
        sinalQ = fabs(state.cells[i - 1].QL + state.cells[i - 1].QG) / (state.cells[i - 1].QL + state.cells[i - 1].QG);
    if (state.cells[i - 1].acsr.tipo == kAccessoryPump && state.cells[i - 1].acsr.bcs.freqnova > 1.) {
        double vazmix = fabs(state.cells[i - 1].QL + state.cells[i - 1].QG);
        double alf0 = state.cells[i - 1].alf;
        double bet0 = state.cells[i - 1].bet;
        double rhomis = (alf0 * state.cells[i - 1].flui.MasEspGas(state.cells[i].presaux, tmed) + (1 - alf0) * ((1 - bet0) * state.cells[i - 1].flui.MasEspLiq(state.cells[i].presaux, tmed) + bet0 * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i].presaux, tmed)));
        double vismis = alf0 * state.cells[i - 1].flui.ViscGas(state.cells[i].presaux, tmed) + (1 - alf0) * ((1. - bet0) * state.cells[i - 1].flui.ViscOleo(state.cells[i].presaux, tmed) + bet0 * state.cells[i].fluicol.VisFlu(state.cells[i].presaux, tmed));
        vazmix *= (units::kSecondsPerDay / units::kCubicMetrePerBarrel);
        state.cells[i - 1].acsr.bcs.NovaVis(vismis, rhomis, vazmix);
        state.cells[i - 1].acsr.dpB = sinalQ * 0.3048 * state.cells[i - 1].acsr.bcs.Hvis * rhomis * kGravity;
        state.cells[i - 1].acsr.potB = state.cells[i - 1].acsr.bcs.Pvis * 745.7;
        state.cells[i - 1].acsr.potTermo = (1. - state.cells[i - 1].acsr.bcs.Evis / 100.) * state.cells[i - 1].acsr.potB;
        if (state.cells[i - 1].acsr.bcs.eficM > 0.)
            state.cells[i - 1].acsr.potBT = (1. + 100. * (1. - state.cells[i - 1].acsr.bcs.eficM / 100.) / state.cells[i - 1].acsr.bcs.eficM) * state.cells[i - 1].acsr.potB;
        else
            state.cells[i - 1].acsr.potBT = 0.;
        state.cells[i - 1].acsr.potTermo += state.cells[i - 1].acsr.potBT * (1. - state.cells[i - 1].acsr.bcs.eficM / 100.) * state.cells[i - 1].acsr.bcs.fracTermMotorEfic;

    } else if (state.cells[i - 1].acsr.tipo == 7) {
        state.cells[i - 1].acsr.dpB = sinalQ * state.cells[i - 1].acsr.delp * units::kPascalPerKgfPerCm2;
        double bet0 = state.cells[i - 1].bet;
        double rhog = state.cells[i - 1].flui.MasEspGas(state.cells[i].presaux, tmed);
        double rhol = ((1 - bet0) * state.cells[i - 1].flui.MasEspLiq(state.cells[i].presaux, tmed) + bet0 * state.cells[i - 1].fluicol.MasEspFlu(state.cells[i].presaux, tmed));
        double qgMon = (state.cells[i - 1].MC - state.cells[i - 1].Mliqini) / rhog;
        double qlmon = state.cells[i - 1].Mliqini / rhol;
        double npoli = 1.;
        if (state.cells[i - 1].acsr.tipoCompGas == 0) {
            npoli = state.cells[i - 1].flui.ConstAdG(state.cells[i - 1].pres, state.cells[i - 1].temp);
            if ((npoli - 1.05) < 1e-2)
                npoli = 1.05;
        } else if (state.cells[i - 1].acsr.tipoCompGas == 1)
            npoli = state.cells[i - 1].acsr.fatPoli;
        double Wcomp;
        double Wbomb;
        Wbomb = (100. / state.cells[i - 1].acsr.eficLiq) * state.cells[i - 1].acsr.dpB * qlmon;
        double wcompiso = -state.cells[i].presaux * units::kPascalPerKgfPerCm2 * qgMon * log(state.cells[i].presaux / (state.cells[i].presaux + state.cells[i - 1].acsr.delp));
        if (state.cells[i - 1].acsr.tipoCompGas != 2)
            Wcomp = -(state.cells[i].presaux * units::kPascalPerKgfPerCm2 * qgMon / (1. - npoli)) *
                    (pow(1 + sinalQ * state.cells[i - 1].acsr.delp / state.cells[i].presaux, (npoli - 1.) / npoli) - 1.);
        else
            Wcomp = wcompiso;
        Wcomp *= (100. / state.cells[i - 1].acsr.eficGas);
        state.cells[i - 1].acsr.potB = sinalQ * (Wcomp + Wbomb);
        state.cells[i - 1].acsr.potTermo = sinalQ * ((1. - state.cells[i - 1].acsr.eficLiq / 100.) * Wbomb + (1. - state.cells[i - 1].acsr.eficGas / 100.) * Wcomp);
        state.cells[i - 1].acsr.potBT = state.cells[i - 1].acsr.potB;
    } else if (state.cells[i - 1].acsr.tipo == kAccessoryMultiPump && state.cells[i - 1].acsr.multibcs.freqnova > 1.) {
        double alf0 = state.cells[i - 1].alf;
        double bet0 = state.cells[i - 1].bet;
        state.cells[i - 1].acsr.multibcs.flui = state.cells[i - 1].flui;
        state.cells[i - 1].acsr.multibcs.fluicol = state.cells[i - 1].fluicol;
        state.cells[i - 1].acsr.multibcs.marchaMultiBcs(state.cells[i - 1].QG, state.cells[i - 1].QL, state.cells[i].presaux, tmed, alf0, bet0);
        state.cells[i - 1].acsr.dpB = state.cells[i - 1].acsr.multibcs.dpB * units::kPascalPerKgfPerCm2;
        state.cells[i - 1].acsr.potB = state.cells[i - 1].acsr.multibcs.potBT;
        state.cells[i - 1].acsr.potBT = state.cells[i - 1].acsr.multibcs.potBT;
        state.cells[i - 1].acsr.potTermo = state.cells[i - 1].acsr.multibcs.potTermo;
        state.cells[i - 1].acsr.potTermo += state.cells[i - 1].acsr.potBT * (1. - state.cells[i - 1].acsr.multibcs.eficM / 100.) * state.cells[i - 1].acsr.multibcs.fracTermMotorEfic;
    }
}

void refreshDownstreamProductionPeriphery(const SteadyStateState &state, int i) {
    if (state.cells[i].pres < kMarchMinimumPressure)
        state.cells[i].pres = kMarchMinimumPressure;
    state.cells[i - 1].presR = state.cells[i].pres;
    if (i < state.lastCell) {
        state.cells[i + 1].presL = state.cells[i].pres;
        state.cells[i + 1].presLini = state.cells[i + 1].presL;
    }
    state.cells[i].presini = state.cells[i].pres;
}

void gasLineHydrostatic(const SteadyStateState &state) {
    double pchute = state.input.gasinj.presinj[0];
    state.gasCells[0].pres = pchute;
    double taux;
    taux = state.input.celg[0].textern;
    state.cells[0].temp = taux;
    state.gasCells[0].flui.MasEspGas(state.gasCells[0].pres, taux);
    for (int i = 0; i < state.lastGasCell; i++) {
        taux = state.input.celg[i].textern;
        double rhog = state.gasCells[i].flui.MasEspGas(pchute, taux);
        double dxmed = 0.5 * (state.gasCells[i].dx0 + state.gasCells[i + 1].dx0);
        pchute -= ((rhog * 9.81 * sinl(state.gasCells[i].duto.teta) * dxmed) / units::kPascalPerKgfPerCm2);
        state.gasCells[i + 1].pres = pchute;
        state.gasCells[i + 1].temp = taux;
    }
}

double marchGasSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double chutemass) {
    int valveCount = state.input.nvalvgas;
    double massGas = 0.;
    double erro = 10.;
    double erro1 = 10.;
    if (chutemass < 0) {
        for (int j = 0; j < valveCount; j++)
            massGas += state.gasCells[state.gasValveCellIndices[j]].massfonteCH; // without a mass flow rate guess
        // at the gas-line inlet, the flow rates of the valves are summed
    } else
        massGas = chutemass;
    int itera = 0;
    double relaxa = 0.5; // relaxation of the total injection flow rate estimated at each iteration
    double presteste = -10;
    double presteste0 = -10;
    int injectionFlowRateIsLow = 0;
    while (((erro > 0.00001 && injectionFlowRateIsLow == 0) || erro1 > 0.00001) && itera < 600) { // convergence iteration
        presteste0 = presteste;
        if (itera > 100)
            relaxa = 0.01;
        else if (itera > 50)
            relaxa = 0.05;
        else if (itera > 20)
            relaxa = 0.1;

        state.gasCells[0].presL = state.initialGasPressure;
        state.gasCells[0].pres = state.initialGasPressure;
        state.gasCells[0].presini = state.initialGasPressure;
        state.gasCells[1].presL = state.initialGasPressure;
        state.gasCells[0].tempL = state.initialGasTemperature;
        state.gasCells[0].temp = state.initialGasTemperature;
        state.gasCells[1].tempL = state.initialGasTemperature;
        state.gasCells[0].rg = state.gasCells[0].flui.MasEspGas(state.initialGasPressure, state.initialGasTemperature);
        state.gasCells[0].u1L = state.gasCells[0].duto.area * state.gasCells[0].rg;
        state.gasCells[0].u1LL = state.gasCells[0].u1L;
        state.gasCells[1].u1LL = state.gasCells[0].u1L;
        state.gasCells[0].VGasL = 0;
        state.gasCells[0].massfonteCH = massGas;
        state.gasCells[0].VGasR = massGas;
        state.gasCells[1].VGasL = massGas;
        for (int i = 1; i <= state.lastGasCell; i++) { // march
            gaslift::updateSteadyGasPressure(dependencies.gasLift, i);            // pressure advance from one cell to the next, at the cell centre
            gaslift::updateSteadyGasTemperature(dependencies.gasLift, i);            // temperature advance from one cell to the next, cell centre
            if (isnan(state.gasCells[i].temp))
                NumError("Temperatrura na linha de servico com valor NaN");
            gaslift::computeSteadyGasFlowRate(dependencies.gasLift, dependencies.thermal, i); // if this cell's centre has a gas-lift valve, computes its flow rate
            // and takes it from the line's total flow rate
            state.gasCells[i].rg = state.gasCells[i].flui.MasEspGas(state.gasCells[i].pres, state.gasCells[i].temp);
            state.gasCells[i - 1].rgR = state.gasCells[i].rg;
            state.gasCells[i].u1L = state.gasCells[i].duto.area * state.gasCells[i].rg;
            state.gasCells[i - 1].u1R = state.gasCells[i].u1L;
            if (i < state.lastGasCell)
                state.gasCells[i + 1].u1LL = state.gasCells[i].u1L;
        }
        state.gasCells[state.lastGasCell].rgR = state.gasCells[state.lastGasCell].rg;
        double massGas2 = massGas; // keeps the previous gas injection value
        massGas = 0;
        for (int j = 0; j < valveCount; j++)
            massGas += state.gasCells[state.gasValveCellIndices[j]].massfonteCH; // updates the injection flow rate from
        // the flow rates computed at the valves
        itera++;
        massGas = (relaxa * massGas + (1. - relaxa) * massGas2); // relaxation of the injection flow rate estimate
        if (0.05 * state.gasCells[0].duto.area * state.gasCells[0].rg > massGas)
            injectionFlowRateIsLow = 1;
        else
            injectionFlowRateIsLow = 0;
        if (fabs(massGas) > 1e-15)
            erro = fabs(massGas - massGas2) / fabs(massGas); // error in the flow rate estimate
        // from one iteration to the next
        else if (fabs(massGas2) > 1e-15)
            erro = fabs(massGas - massGas2) / fabs(massGas2); // erro = erro/2.;
        else
            erro = 0.;
        presteste = state.gasCells[state.lastGasCell].pres;
        erro1 = fabs(presteste - presteste0) / presteste0; // error in the pressure of the last cell
    }
    if (itera >= 600) {
        if ((*state.globals).networkMode == 0 && state.input.AP == 0) {
            NumError("Busca de valores iniciais para calculo de zero de funcao em marchaGasPerm1 atingiu maximo de iteracoes");
            return 0.0;
        } else {
            if ((*state.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    } else
        return erro1;
}

double marchProductionSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double pressureGuess) {
    auto initialFractions = seedFirstCellVoidFraction(state, dependencies, pressureGuess);
    constexpr double initialPhaseFractionCutoff = 1e-6;
    if (fabs(initialFractions.voidFraction) < initialPhaseFractionCutoff)
        initialFractions.voidFraction = 0.;
    if (fabs(initialFractions.complementaryLiquidFraction) < initialPhaseFractionCutoff)
        initialFractions.complementaryLiquidFraction = 0.;

    initializeClosedSourceBoundary(state);
    double monitoredMassFlowRate = 1.;
    double previousMonitoredMassFlowRate = -10;
    double monitoredOutletPressure = 1.;
    double previousMonitoredOutletPressure = -10;
    state.steadyIteration = 0;
    const int maximumMarchPasses = state.input.AceleraConvergPerm == 1 ? 1 : 2;

    while ((fabs(monitoredMassFlowRate - previousMonitoredMassFlowRate) / fabs(monitoredMassFlowRate) > state.input.CriterioConvergPerm ||
            fabs(monitoredOutletPressure - previousMonitoredOutletPressure) / fabs(monitoredOutletPressure) > state.input.CriterioConvergPerm) &&
           state.steadyIteration < maximumMarchPasses) {
        previousMonitoredMassFlowRate = monitoredMassFlowRate;
        previousMonitoredOutletPressure = monitoredOutletPressure;
        if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.networkCoupled == 1) {
            if (state.steadyIteration == 0)
                gaslift::initializeTubingConnectionSteady(dependencies.gasLift);
            else
                gaslift::connectTubingSteady(dependencies.gasLift);
        }

        int nextCell;
        do {
            resetProductionMarchInlet(state, pressureGuess, initialFractions);
            if (marchPressureTooLow(state.cells[0].pres) ||
                (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[0].pres) > (*state.globals).smallValueTolerance))
                return -kSteadyMarchLimit;
            else if ((state.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                      (state.cells[0].acsr.ipr.Pres - state.cells[0].pres) < (*state.globals).smallValueTolerance))
                return kSteadyMarchLimit;

            if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.steadyIteration == 0 && state.convergenceMonitor > 0.1) {
                withGasInletCondition(state.gasCells[0].tipoCC,
                                      [&](auto condition) { estimateValveGasPressure(state, condition); });
                gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, 0);
            }
            nextCell = 1;
            double abortValue;
            if (advanceProductionCells(state, dependencies, pressureGuess, nextCell, abortValue))
                return abortValue;
        } while (nextCell != state.lastCell + 1);

        marchGasLineAndCoupleAnnulus(state, dependencies, pressureGuess);
        monitoredMassFlowRate = state.cells[state.lastCell - 1].MC;
        monitoredOutletPressure = state.cells[state.lastCell].pres;
        state.steadyIteration++;
    }

    double outletPressureCorrection = 0.;
    if (((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1) && state.input.corrigeContSep == 1)
        outletPressureCorrection = steadyPressureAtLastCell(state);

    state.baseConvergenceMonitor = state.gasSurfacePressure;
    return state.gasSurfacePressure - (state.cells[state.lastCell].pres + outletPressureCorrection);
}

double marchReverseProductionSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute) {

    double alfini = 0.;
    double betini = 0.;
    state.slowHeatTransferThreshold = 0.1;

    const auto initialFractions = seedFirstCellVoidFraction(state, dependencies, pchute);
    alfini = initialFractions.voidFraction;
    betini = initialFractions.complementaryLiquidFraction;
    // this march is for a source at the start of the pipe,
    // so the pipe is taken as closed and a source is placed at the centre of the
    // first cell. The flow rates at the cell's left boundary are therefore 0
    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;
    double masfim = 1.;
    double masfim0 = -10;
    double presteste = 1.;
    double presteste0 = -10;
    double tempteste = state.cells[0].temp;
    double tempteste0 = -1000;
    state.steadyIteration = 0;

    int limIter = 2; // iteration limit when convergence acceleration is off. Turning this option off
    // is not advisable: the gains are small and convergence becomes unstable, especially
    // when the branch is part of a network
    if (state.input.AceleraConvergPerm == 1) { // convergence acceleration on
        limIter = 1;                   // usually only two march iterations are made for a given guess

    }
    // arq.CriterioConvergPerm is a march convergence criterion; it only makes sense
    // when convergence acceleration is off. Note that this convergence is not
    // the convergence of the problem itself, just a repetition of the march for a given
    // pressure or flow rate guess at the start of the pipe. What convergence really seeks is
    // the bottom-hole pressure or flow rate that satisfies the boundary conditions at the end
    // of the pipe; that is done in the search methods.
    while ((fabs(masfim - masfim0) / fabs(masfim) > state.input.CriterioConvergPerm ||
            fabs(presteste - presteste0) / fabs(presteste) > state.input.CriterioConvergPerm) &&
           (state.steadyIteration < limIter || fabs(tempteste - tempteste0) / ((tempteste) + units::kZeroCelsiusInKelvin) > 0.001)) {

        masfim0 = masfim;
        presteste0 = presteste;
        tempteste0 = tempteste;
        if (state.steadyIteration == 0 && state.input.lingas > 0 && state.input.nvalvgas > 0 && state.networkCoupled == 1)
            gaslift::initializeTubingConnectionSteady(dependencies.gasLift); // before the first march iteration,
        // an initial estimate of the thermal coupling between the tubing and the annulus is made,
        // when there is a gas line
        else if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.networkCoupled == 1)
            gaslift::connectTubingSteady(dependencies.gasLift); // thermal coupling with the pressure and temperature
        // obtained in the first march iteration
        int i;
        int guessNeedsCorrection = 1;
        while (guessNeedsCorrection == 1) {
            // Complete the column march or return its failure sentinel.

            // initialising the pressures and volume fractions of the first cells, cell centre
            // and cell boundary
            state.cells[0].presauxL = pchute;
            state.cells[0].presLini = pchute;
            state.cells[0].presL = pchute;
            state.cells[0].pres = pchute;
            state.cells[1].presL = pchute;
            state.cells[0].presini = pchute;
            state.cells[1].presLini = pchute;
            state.cells[0].presaux = pchute;
            state.cells[1].presauxL = pchute;

            state.cells[0].alf = alfini;
            state.cells[0].alfini = alfini;
            state.cells[0].bet = betini;
            state.cells[0].betini = betini;
            state.cells[1].alfL = state.cells[0].alf;
            state.cells[1].alfLini = state.cells[0].alf;
            state.cells[0].alfPigD = state.cells[0].alf;
            state.cells[0].alfPigDini = state.cells[0].alf;
            state.cells[0].alfPigE = state.cells[0].alf;
            state.cells[0].alfPigEini = state.cells[0].alf;
            state.cells[1].betL = state.cells[0].bet;
            state.cells[1].betLini = state.cells[0].bet;
            state.cells[0].betPigD = state.cells[0].bet;
            state.cells[0].betPigDini = state.cells[0].bet;
            state.cells[0].betPigE = state.cells[0].bet;
            state.cells[0].betPigEini = state.cells[0].bet;
            state.cells[0].betI = state.cells[0].bet;
            state.cells[1].betLI = state.cells[0].bet;
            // checks whether something is already wrong at the start of the march
            if (marchPressureTooLow(state.cells[0].pres) ||
                (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[0].pres) > (*state.globals).smallValueTolerance))
                return -kSteadyMarchLimit;

            if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.steadyIteration == 0)
                gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, 0);
            i = 1;
            // start of the march proper
            double abortValue;
            if (advanceReverseProductionCells(state, dependencies, pchute, i, abortValue))
                return abortValue;
            if (i == state.lastCell + 1)
                guessNeedsCorrection = 0; // end of the march
        }

        masfim = state.cells[state.lastCell - 1].MC; // stores the flow rate to compute the error when the
        // convergence acceleration option is off
        presteste = state.cells[state.lastCell].pres; // stores the pressure to compute the error when the
        // convergence acceleration option is off
        state.steadyIteration++; // updates the march iteration
        if (state.steadyIteration > 200 && state.input.AP == 0)
            NumError("ConvergÃƒÂªncia em marchaProdPerm1 atingiu maximo de iteracoes");
        else if (state.steadyIteration > 200)
            return kSteadyConvergenceFailure;
        if (state.input.tipoFluido != 10000) {
            state.cells[state.lastCell].temp = state.casingTemperature;
            if (state.steadyIteration < 100) {
                int lento = 0;
                double media = 0.;
                for (int ktemp = state.lastCell - 1; ktemp >= 1; ktemp--) {

                    double area = state.cells[ktemp].duto.area;
                    double ugsmed;
                    ugsmed = fabs(state.cells[ktemp].QG) / area; // gas superficial velocity
                    double ulsmed;
                    ulsmed = fabs(state.cells[ktemp].QL) / area; // liquid superficial velocity
                    if (fabs(ugsmed + ulsmed) <= state.slowHeatTransferThreshold)
                        lento += 1;
                    media += fabs(ugsmed + ulsmed);
                }
                media /= (state.lastCell - 1);
                if (lento > 0 && lento < state.lastCell) {
                    state.slowHeatTransferThreshold = 100.;
                }
                for (int ktemp = state.lastCell - 1; ktemp >= 0; ktemp--) {
                    thermal::advanceReverseSteadyTemperature(dependencies.thermal, ktemp, 0); // advances the temperature from cell i-1 to cell i
                    // checks whether the temperature went out of bounds
                    // when working with a PVTSim table
                    if (state.input.usaTabela == 1 && (state.cells[ktemp].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
                        state.cells[ktemp].temp = state.input.tabent.tmin;
                    thermal::updateProductionTemperaturePeriphery(dependencies.thermal, ktemp); // just updates the left and right temperature fields
                }
            }
            tempteste = state.cells[0].temp;
        } else {
            refreshProperties(state);
            refreshSteadyThermalVelocities(state);
            computePseudoTransientTimeStep(state);
            for (int kontaPseudo = 0; kontaPseudo < 20; kontaPseudo++) {
                state.cells[0].tempini = state.cells[0].temp;
                state.cells[1].tempLini = state.cells[1].tempL;
                state.cells[1].tempL = state.cells[0].temp;
                state.cells[state.lastCell].temp = state.casingTemperature;
                for (int i = 0; i < state.lastCell; i++) {
                    state.cells[i].tempini = state.cells[i].temp;
                }
                for (int itemp = 1; itemp <= state.lastCell; itemp++) {
                    thermal::computeTemperature(dependencies.thermal, itemp, state.cells[itemp].tempini, 1);
                }
                refreshProperties(state);
                computePseudoTransientTimeStep(state);
            }
        }
    }

    double corrigePresF = 0.;
    if (((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1) && state.input.corrigeContSep == 1)
        corrigePresF = steadyPressureAtLastCell(state);

    state.baseConvergenceMonitor = state.gasSurfacePressure;
    return state.gasSurfacePressure - (state.cells[state.lastCell].pres + corrigePresF); // if the march reached the last cell,
    // returns the difference between the upstream choke pressure and the pressure of the last cell
    // computed by the march
}

double marchProductionSteadySecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute) {

    double alfini = 0.;
    double betini = 0.;

    const auto initialFractions = seedFirstCellVoidFraction(state, dependencies, pchute);
    alfini = initialFractions.voidFraction;
    betini = initialFractions.complementaryLiquidFraction;

    // this march is for a source at the start of the pipe,
    // so the pipe is taken as closed and a source is placed at the centre of the
    // first cell. The flow rates at the cell's left boundary are therefore 0
    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;
    double masfim = 1.;
    double masfim0 = -10;
    double presteste = 1.;
    double presteste0 = -10;
    state.steadyIteration = 0;

    int limIter = 2; // iteration limit when convergence acceleration is off. Turning this option off
    // is not advisable: the gains are small and convergence becomes unstable, especially
    // when the branch is part of a network
    if (state.input.AceleraConvergPerm == 1) { // convergence acceleration on
        limIter = 1;                   // usually only two march iterations are made for a given guess

    }
    // arq.CriterioConvergPerm is a march convergence criterion; it only makes sense
    // when convergence acceleration is off. Note that this convergence is not
    // the convergence of the problem itself, just a repetition of the march for a given
    // pressure or flow rate guess at the start of the pipe. What convergence really seeks is
    // the bottom-hole pressure or flow rate that satisfies the boundary conditions at the end
    // of the pipe; that is done in the search methods.
    while ((fabs(masfim - masfim0) / fabs(masfim) > state.input.CriterioConvergPerm ||
            fabs(presteste - presteste0) / fabs(presteste) > state.input.CriterioConvergPerm) &&
           (state.steadyIteration < limIter)) {
        masfim0 = masfim;
        presteste0 = presteste;
        if (state.steadyIteration == 0 && state.input.lingas > 0 && state.input.nvalvgas > 0 && state.networkCoupled == 1)
            gaslift::initializeTubingConnectionSteady(dependencies.gasLift); // before the first march iteration,
        // an initial estimate of the thermal coupling between the tubing and the annulus is made,
        // when there is a gas line
        else if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.networkCoupled == 1)
            gaslift::connectTubingSteady(dependencies.gasLift); // thermal coupling with the pressure and temperature
        // obtained in the first march iteration
        int i;
        int guessNeedsCorrection = 1;
        while (guessNeedsCorrection == 1) {
            // Complete the column march or return its failure sentinel.

            // initialising the pressures and volume fractions of the first cells, cell centre
            // and cell boundary
            state.cells[0].presauxL = pchute;
            state.cells[0].presLini = pchute;
            state.cells[0].presL = pchute;
            state.cells[0].pres = pchute;
            state.cells[1].presL = pchute;
            state.cells[0].presini = pchute;
            state.cells[1].presLini = pchute;
            state.cells[0].presaux = pchute;
            state.cells[1].presauxL = pchute;

            state.cells[0].alf = alfini;
            state.cells[0].alfini = alfini;
            state.cells[0].bet = betini;
            state.cells[0].betini = betini;
            state.cells[1].alfL = state.cells[0].alf;
            state.cells[1].alfLini = state.cells[0].alf;
            state.cells[0].alfPigD = state.cells[0].alf;
            state.cells[0].alfPigDini = state.cells[0].alf;
            state.cells[0].alfPigE = state.cells[0].alf;
            state.cells[0].alfPigEini = state.cells[0].alf;
            state.cells[1].betL = state.cells[0].bet;
            state.cells[1].betLini = state.cells[0].bet;
            state.cells[0].betPigD = state.cells[0].bet;
            state.cells[0].betPigDini = state.cells[0].bet;
            state.cells[0].betPigE = state.cells[0].bet;
            state.cells[0].betPigEini = state.cells[0].bet;
            state.cells[0].betI = state.cells[0].bet;
            state.cells[1].betLI = state.cells[0].bet;

            // checks whether something is already wrong at the start of the march
            if (marchPressureTooLow(state.cells[0].pres) ||
                (state.input.usaTabela == 1 && (state.input.tabent.pmin - state.cells[0].presaux) > (*state.globals).smallValueTolerance))
                return -kSteadyMarchLimit;
            else if ((state.cells[0].acsr.tipo == kAccessoryInflowPerformance && (state.cells[0].acsr.ipr.Pres - state.cells[0].pres) < (*state.globals).smallValueTolerance)) {
                return kSteadyMarchLimit;
            } else if ((state.cells[0].acsr.tipo == kAccessoryRadialPorous && (state.cells[0].acsr.radialPoro.pRes[0] - state.cells[0].pres) < (*state.globals).smallValueTolerance)) {
                return kSteadyMarchLimit;
            } else if ((state.cells[0].acsr.tipo == kAccessoryPorous2D && (state.cells[0].acsr.poroso2D.dados.pRes - state.cells[0].pres) < (*state.globals).smallValueTolerance)) {
                return kSteadyMarchLimit;
            }
            // IniciaVazValvGasPerm estimates the flow rate through a gas-lift valve
            // before the gas line has been marched. It receives the
            // production cell index and checks whether that cell has a gas-lift valve; if it has,
            // with an injected-flow-rate condition on the gas line, it divides the injected flow rate by the number of
            // valves and gives that value to the valve of this production cell;
            // with an injection-pressure condition, it estimates the gas-line pressure
            // at the valve by hydrostatics and computes the valve's injection flow rate from it
            if (state.input.lingas > 0 && state.input.nvalvgas > 0 && state.steadyIteration == 0 && state.convergenceMonitor > 0.1) {
                withGasInletCondition(state.gasCells[0].tipoCC,
                                      [&](auto condition) { estimateValveGasPressure(state, condition); });
                gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, 0);
            }
            i = 1;
            // start of the march proper
            double abortValue;
            if (advanceProductionCellsSecondary(state, dependencies, pchute, i, abortValue))
                return abortValue;
            if (i == state.lastCell + 1)
                guessNeedsCorrection = 0; // end of the march
        }
        // after the production line march, the gas line is marched,
        // if there is one
        marchGasLineAndCoupleAnnulus(state, dependencies, pchute);

        masfim = state.cells[state.lastCell - 1].MC; // stores the flow rate to compute the error when the
        // convergence acceleration option is off
        presteste = state.cells[state.lastCell].pres; // stores the pressure to compute the error when the
        // convergence acceleration option is off
        state.steadyIteration++; // updates the march iteration
        if (state.steadyIteration > 200 && state.input.AP == 0)
            NumError("Convergencia em marchaProdPerm2 atingiu maximo de iteracoes");
        else if (state.steadyIteration > 200)
            return kSteadyMarchLimit;
    }

    // in this march the condition at the end of the pipe is not the upstream choke pressure
    // but the downstream choke pressure. So the condition to converge
    // is the flow rate through the choke, given by the difference between the pressure
    // in the last cell and the downstream choke pressure. So first
    // the flow rate through the choke is computed and compared with the total mass flow rate
    // in the last cell
    double maxSup = surfaceChokeMassFlowRate(state, dependencies.thermal);
    if (fabs(masfim) > 1e-15)
        state.baseConvergenceMonitor = fabs(masfim);
    else if (fabs(maxSup) > 1e-15)
        state.baseConvergenceMonitor = fabs(maxSup);
    else
        state.baseConvergenceMonitor = 1.;
    return (masfim - maxSup); // difference between the total flow rate at the left boundary of the
    // second-last cell and the flow rate through the choke. If the pressure guess is high,
    // maxSup>masfim, it returns a negative value;
    // if the bottom-hole pressure estimate is low, maxSup<masfim, it returns a positive value
}

double marchProductionPressureToPressure(const SteadyStateState &state, const SteadyDependencies &dependencies, double mchute) {
    return detail::marchProductionPressureToPressure(state, dependencies, mchute, nullptr);
}

double detail::marchProductionPressureToPressure(const SteadyStateState &state, const SteadyDependencies &dependencies,
                                                double mchute, PressureMarchStop *stop) {
    if (stop != nullptr)
        *stop = {};

    double alfini = 0.;
    double betini = 0.;

    seedFirstCellFromFlowRateGuess(state, mchute, alfini, betini);

    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;

    int i;

    state.cells[0].presauxL = state.cells[0].pres;
    state.cells[0].presLini = state.cells[0].pres;
    state.cells[0].presL = state.cells[0].pres;
    state.cells[1].presL = state.cells[0].pres;
    state.cells[0].presini = state.cells[0].pres;
    state.cells[1].presLini = state.cells[0].pres;
    state.cells[0].presaux = state.cells[0].pres;
    state.cells[1].presauxL = state.cells[0].pres;

    state.cells[0].alf = alfini;
    state.cells[0].alfini = alfini;
    state.cells[0].bet = betini;
    state.cells[0].betini = betini;
    state.cells[1].alfL = state.cells[0].alf;
    state.cells[1].alfLini = state.cells[0].alf;
    state.cells[0].alfPigD = state.cells[0].alf;
    state.cells[0].alfPigDini = state.cells[0].alf;
    state.cells[0].alfPigE = state.cells[0].alf;
    state.cells[0].alfPigEini = state.cells[0].alf;
    state.cells[1].betL = state.cells[0].bet;
    state.cells[1].betLini = state.cells[0].bet;
    state.cells[0].betPigD = state.cells[0].bet;
    state.cells[0].betPigDini = state.cells[0].bet;
    state.cells[0].betPigE = state.cells[0].bet;
    state.cells[0].betPigEini = state.cells[0].bet;
    state.cells[0].betI = state.cells[0].bet;
    state.cells[1].betLI = state.cells[0].bet;

    state.steadyIteration = 0;
    while (state.steadyIteration < 3) {
        i = 1;
        while (i <= state.lastCell && state.cells[i - 1].pres >= 1.) {

            advanceUpstreamSteadyPressure(state, i, 0);
            refreshUpstreamProductionPeriphery(state, i);
            if (state.input.flashCompleto != 2)
                advanceSteadyMass(state, dependencies, i);
            else
                advanceCompositionalSteadyMass(state, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 0);
            if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
                state.cells[i].temp = state.input.tabent.tmin;
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceDownstreamSteadyPressure(state, i, 0);
            refreshDownstreamProductionPeriphery(state, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
            if (state.input.ordperm > 1) {
                double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
                double D0pres = state.cells[i].pres - state.cells[i].presaux;
                double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
                advanceUpstreamSteadyPressure(state, i, 1);
                refreshUpstreamProductionPeriphery(state, i);
                advanceSteadyMass(state, dependencies, i);
                thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
                if (isnan(state.cells[i].temp))
                    NumError("Temperatrura na linha de producao com valor NaN");
                advanceDownstreamSteadyPressure(state, i, 1);
                state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
                state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
                state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
                refreshDownstreamProductionPeriphery(state, i);
                refreshUpstreamProductionPeriphery(state, i);
                thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
                advanceSteadyMass(state, dependencies, i);
                if (state.input.tipoFluido == 0)
                    advanceSteadyMassTransfer(state, i - 1);
                else
                    advanceSteadyGasMassTransfer(state, i - 1);
            }
            i++;
            if (state.cells[i - 1].pres <= 1) {
                if (stop != nullptr)
                    *stop = {i - 1, state.cells[i - 1].pres};
                return -kSteadyMarchLimit;
            }
        }

        state.steadyIteration++;
    }

    double corrigePresF = 0.;
    if ((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1)
        corrigePresF = steadyPressureAtLastCell(state);

    state.baseConvergenceMonitor = state.gasSurfacePressure;
    return state.cells[state.lastCell].pres + corrigePresF - state.gasSurfacePressure;
}

double marchReverseProductionPressureToPressure(const SteadyStateState &state, const SteadyDependencies &dependencies, double mchute) {

    double alfini = 0.;
    double betini = 0.;
    state.slowHeatTransferThreshold = 0.05;

    seedFirstCellFromFlowRateGuess(state, mchute, alfini, betini);

    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;

    int i;

    state.cells[0].presauxL = state.cells[0].pres;
    state.cells[0].presLini = state.cells[0].pres;
    state.cells[0].presL = state.cells[0].pres;
    state.cells[1].presL = state.cells[0].pres;
    state.cells[0].presini = state.cells[0].pres;
    state.cells[1].presLini = state.cells[0].pres;
    state.cells[0].presaux = state.cells[0].pres;
    state.cells[1].presauxL = state.cells[0].pres;

    state.cells[0].alf = alfini;
    state.cells[0].alfini = alfini;
    state.cells[0].bet = betini;
    state.cells[0].betini = betini;
    state.cells[1].alfL = state.cells[0].alf;
    state.cells[1].alfLini = state.cells[0].alf;
    state.cells[0].alfPigD = state.cells[0].alf;
    state.cells[0].alfPigDini = state.cells[0].alf;
    state.cells[0].alfPigE = state.cells[0].alf;
    state.cells[0].alfPigEini = state.cells[0].alf;
    state.cells[1].betL = state.cells[0].bet;
    state.cells[1].betLini = state.cells[0].bet;
    state.cells[0].betPigD = state.cells[0].bet;
    state.cells[0].betPigDini = state.cells[0].bet;
    state.cells[0].betPigE = state.cells[0].bet;
    state.cells[0].betPigEini = state.cells[0].bet;
    state.cells[0].betI = state.cells[0].bet;
    state.cells[1].betLI = state.cells[0].bet;

    state.steadyIteration = 0;
    double tempteste = state.cells[0].temp;
    double tempteste0 = -1000;
    while (state.steadyIteration < 3 || fabs(tempteste - tempteste0) / ((tempteste) + units::kZeroCelsiusInKelvin) > 0.001) {
        i = 1;
        tempteste0 = tempteste;
        while (i <= state.lastCell && state.cells[i - 1].pres >= 1.) {

            advanceUpstreamSteadyPressure(state, i, 0);
            refreshUpstreamProductionPeriphery(state, i);
            if (state.input.flashCompleto != 2)
                advanceSteadyMass(state, dependencies, i);
            else
                advanceCompositionalSteadyMass(state, dependencies, i);
            if (isnan(state.cells[i].temp))
                NumError("Temperatrura na linha de producao com valor NaN");
            if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
                state.cells[i].temp = state.input.tabent.tmin;
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceDownstreamSteadyPressure(state, i, 0);
            refreshDownstreamProductionPeriphery(state, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
            if (state.input.ordperm > 1) {
                double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
                double D0pres = state.cells[i].pres - state.cells[i].presaux;
                double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
                advanceUpstreamSteadyPressure(state, i, 1);
                refreshUpstreamProductionPeriphery(state, i);
                advanceSteadyMass(state, dependencies, i);
                advanceDownstreamSteadyPressure(state, i, 1);
                state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
                state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
                state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
                refreshDownstreamProductionPeriphery(state, i);
                refreshUpstreamProductionPeriphery(state, i);
                thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
                advanceSteadyMass(state, dependencies, i);
                if (state.input.tipoFluido == 0)
                    advanceSteadyMassTransfer(state, i - 1);
                else
                    advanceSteadyGasMassTransfer(state, i - 1);
            }
            i++;
            if (state.cells[i - 1].pres <= 1)
                return -kSteadyMarchLimit;
        }

        state.steadyIteration++;
        if (state.steadyIteration > 200 && state.input.AP == 0)
            NumError("ConvergÃƒÂªncia em marchaProdPerm1 atingiu maximo de iteracoes");
        else if (state.steadyIteration > 200)
            return kSteadyConvergenceFailure;
        if (state.input.tipoFluido != 10000) {
            state.cells[state.lastCell].temp = state.casingTemperature;
            if (state.steadyIteration < 100) {
                int lento = 0;
                state.slowHeatTransferThreshold = 0;
                for (int ktemp = state.lastCell - 1; ktemp >= 0; ktemp--) {

                    double area = state.cells[ktemp].duto.area;
                    double ugsmed;
                    ugsmed = fabs(state.cells[ktemp].QG) / area; // gas superficial velocity
                    double ulsmed;
                    ulsmed = fabs(state.cells[ktemp].QL) / area; // liquid superficial velocity
                    if (fabs(ugsmed + ulsmed) <= 0.1)
                        lento += 1;
                }
                if (state.steadyIteration > 10 && state.slowHeatTransferThreshold < 0.01) {
                    state.slowHeatTransferThreshold -= 0.01;
                    state.steadyIteration = 0;
                }
                for (int ktemp = state.lastCell - 1; ktemp >= 0; ktemp--) {
                    thermal::advanceReverseSteadyTemperature(dependencies.thermal, ktemp, 0);
                    if (state.input.usaTabela == 1 && (state.cells[ktemp].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
                        state.cells[ktemp].temp = state.input.tabent.tmin;
                    thermal::updateProductionTemperaturePeriphery(dependencies.thermal, ktemp); // just updates the left and right temperature fields
                }
            }
            tempteste = state.cells[0].temp;
        } else {
            refreshProperties(state);
            refreshSteadyThermalVelocities(state);
            computePseudoTransientTimeStep(state);
            for (int kontaPseudo = 0; kontaPseudo < 20; kontaPseudo++) {
                state.cells[0].tempini = state.cells[0].temp;
                state.cells[1].tempLini = state.cells[1].tempL;
                state.cells[1].tempL = state.cells[0].temp;
                state.cells[state.lastCell].temp = state.casingTemperature;
                for (int i = 0; i < state.lastCell; i++) {
                    state.cells[i].tempini = state.cells[i].temp;
                }
                for (int itemp = 1; itemp <= state.lastCell; itemp++) {
                    thermal::computeTemperature(dependencies.thermal, itemp, state.cells[itemp].tempini, 1);
                }
                refreshProperties(state);
                computePseudoTransientTimeStep(state);
            }
        }
    }
    double corrigePresF = 0.;
    if ((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1)
        corrigePresF = steadyPressureAtLastCell(state);
    state.baseConvergenceMonitor = state.gasSurfacePressure;
    return state.cells[state.lastCell].pres + corrigePresF - state.gasSurfacePressure;
}

double marchProductionPressureToPressureSecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double mchute) {

    double alfini = 0.;
    double betini = 0.;

    if (state.cells[0].acsr.tipo == kAccessoryGasInjection) {
        state.cells[0].temp = state.cells[0].acsr.injg.temp;
        if (state.input.flashCompleto == 2) {
            if (state.input.tabelaDinamica == 0)
                state.cells[0].flui.atualizaPropComp(state.cells[0].pres, state.cells[0].temp);
            state.cells[0].acsr.injg.FluidoPro.atualizaPropComp(state.cells[0].pres, state.cells[0].temp, -1, NULL, NULL, state.cells[0].acsr.injg.seco);
        }
        state.cells[0].acsr.injg.QGas = mchute;
    } else if (state.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
        state.cells[0].temp = state.cells[0].acsr.injl.temp;
        if (state.input.flashCompleto == 2) {
            if (state.input.tabelaDinamica == 0)
                state.cells[0].flui.atualizaPropComp(state.cells[0].pres, state.cells[0].temp);
            state.cells[0].acsr.injl.FluidoPro.atualizaPropComp(state.cells[0].pres, state.cells[0].temp);
        }
        state.cells[0].acsr.injl.QLiq = mchute;
    }

    if (state.cells[0].acsr.tipo == kAccessoryGasInjection) {
        if (state.cells[0].acsr.injg.seco == 1) {
            alfini = 1.;
            betini = 0.;
        } else {
            double masgas = state.cells[0].acsr.injg.VMas(state.cells[0].pres, state.cells[0].temp);
            double quality;
            double masT;
            if (state.input.ConContEntrada != 1) {
                if (state.input.flashCompleto != 2)
                    quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(1., 20.);
                else
                    quality = state.cells[0].acsr.injg.FluidoPro.dStockTankVaporMassFraction;
                masT = masgas / quality;
                quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(state.cells[0].pres, state.cells[0].temp);
            } else {
                quality = state.inletQuality;
                masT = masgas / quality;
            }
            double qgas = masT * quality /
                          state.cells[0].acsr.injg.FluidoPro.MasEspGas(state.cells[0].pres, state.cells[0].temp);
            double qliq = masT * (1. - quality) /
                          state.cells[0].acsr.injg.FluidoPro.MasEspLiq(state.cells[0].pres, state.cells[0].temp);
            double qcomp = state.cells[0].acsr.injg.razCompGas *
                           state.cells[0].acsr.injg.QGas * state.cells[0].acsr.injg.fluidocol.standardDensity() /
                           state.cells[0].acsr.injg.fluidocol.MasEspFlu(state.cells[0].pres, state.cells[0].temp);
            qcomp /= units::kSecondsPerDay;
            if (fabs(qliq + qgas) > 1e-15)
                alfini = qgas / (qliq + qcomp + qgas);
            else
                alfini = state.inletQuality;
            if ((fabs(qcomp) + fabs(qliq)) > 1e-15)
                betini = fabs(qcomp) / (fabs(qcomp) + fabs(qliq));
            else
                betini = 0.;
        }
    } else if (state.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
        double qgas = state.cells[0].acsr.injl.QLiq * (1 - state.cells[0].acsr.injl.bet) *
                      (1. - state.cells[0].acsr.injl.FluidoPro.BSW) *
                      (state.cells[0].acsr.injl.FluidoPro.RGO -
                       state.cells[0].acsr.injl.FluidoPro.rDgD * state.cells[0].acsr.injl.FluidoPro.RS(state.cells[0].pres, state.cells[0].temp) * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                      state.cells[0].acsr.injl.FluidoPro.Deng * kAirDensityAtStandardConditions / state.cells[0].acsr.injl.FluidoPro.MasEspGas(state.cells[0].pres, state.cells[0].temp);
        double qliq = state.cells[0].acsr.injl.QLiq * (1 - state.cells[0].acsr.injl.bet) *
                          (1. - state.cells[0].acsr.injl.FluidoPro.BSW) * state.cells[0].acsr.injl.FluidoPro.BOFunc(state.cells[0].pres, state.cells[0].temp) +
                      state.cells[0].acsr.injl.QLiq * (1 - state.cells[0].acsr.injl.bet) *
                          state.cells[0].acsr.injl.FluidoPro.BSW * state.cells[0].acsr.injl.FluidoPro.BAFunc(state.cells[0].pres, state.cells[0].temp) +
                      state.cells[0].acsr.injl.QLiq * state.cells[0].acsr.injl.bet;
        if (fabs(qliq + qgas) > 1e-15)
            alfini = fabs(qgas / (qliq + qgas));
        else
            alfini = state.inletQuality;
        betini = state.cells[0].acsr.injl.bet;
    }

    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;

    int i;

    state.cells[0].presauxL = state.cells[0].pres;
    state.cells[0].presLini = state.cells[0].pres;
    state.cells[0].presL = state.cells[0].pres;
    state.cells[1].presL = state.cells[0].pres;
    state.cells[0].presini = state.cells[0].pres;
    state.cells[1].presLini = state.cells[0].pres;
    state.cells[0].presaux = state.cells[0].pres;
    state.cells[1].presauxL = state.cells[0].pres;

    state.cells[0].alf = alfini;
    state.cells[0].alfini = alfini;
    state.cells[0].bet = betini;
    state.cells[0].betini = betini;
    state.cells[1].alfL = state.cells[0].alf;
    state.cells[1].alfLini = state.cells[0].alf;
    state.cells[0].alfPigD = state.cells[0].alf;
    state.cells[0].alfPigDini = state.cells[0].alf;
    state.cells[0].alfPigE = state.cells[0].alf;
    state.cells[0].alfPigEini = state.cells[0].alf;
    state.cells[1].betL = state.cells[0].bet;
    state.cells[1].betLini = state.cells[0].bet;
    state.cells[0].betPigD = state.cells[0].bet;
    state.cells[0].betPigDini = state.cells[0].bet;
    state.cells[0].betPigE = state.cells[0].bet;
    state.cells[0].betPigEini = state.cells[0].bet;
    state.cells[0].betI = state.cells[0].bet;
    state.cells[1].betLI = state.cells[0].bet;

    state.steadyIteration = 0;
    while (state.steadyIteration < 3) {
        i = 1;
        advanceProductionCellsPressureToPressureSecondary(state, dependencies, i);
        state.steadyIteration++;
    }

    double maxSup;
    if (state.cells[state.lastCell].pres >= state.gasSurfacePressure) {
        double tESup = state.cells[state.lastCell].temp;
        double alfSup = state.cells[state.lastCell].alf;
        double betSup = state.cells[state.lastCell].bet;

        double masentrada = state.cells[state.lastCell - 1].MR;
        double massgas = state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR;
        double quality;
        state.outletPressure = state.cells[state.lastCell].pres;
        state.cells[state.lastCell].flui.MasEspLiq(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp);
        quality = fabs(massgas / masentrada);

        double masChk;

        double ypres = state.gasSurfacePressure / state.outletPressure;
        masChk = state.surfaceChoke.vazmassSachd(ypres, state.outletPressure, tESup, alfSup, betSup, quality, state.cells[state.lastCell - 1].flui,
                                       state.cells[state.lastCell - 1].fluicol);
        maxSup = state.surfaceChoke.vazmaxSachd(state.outletPressure, tESup, alfSup, betSup, quality, state.cells[state.lastCell - 1].flui, state.cells[state.lastCell - 1].fluicol);
        if (fabs(ypres) > fabs(state.surfaceChoke.razpres))
            maxSup = masChk;

        if (state.surfaceChoke.AreaGarg > (1e-3) * state.cells[state.lastCell - 1].duto.area && ypres < 1.) {
            state.input.valTempChokeJus = thermal::surfaceChokeOutletTemperature(dependencies.thermal);
        }
    } else {
        maxSup = 0.;
        state.input.valTempChokeJus = state.cells[state.lastCell].temp;
    }

    if (fabs(maxSup) > 1e-15)
        state.baseConvergenceMonitor = fabs(maxSup);
    else
        state.baseConvergenceMonitor = 1.;
    return maxSup - state.cells[state.lastCell - 1].MR;
}

double marchProductionPressureToPressureTertiary(const SteadyStateState &state, const SteadyDependencies &dependencies, double mchute) {

    double alfini = 0.;
    double betini = 0.;

    seedFirstCellFromFlowRateGuess(state, mchute, alfini, betini);

    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].MComp = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;

    int i;

    state.cells[0].presauxL = state.cells[0].pres;
    state.cells[0].presLini = state.cells[0].pres;
    state.cells[0].presL = state.cells[0].pres;
    state.cells[1].presL = state.cells[0].pres;
    state.cells[0].presini = state.cells[0].pres;
    state.cells[1].presLini = state.cells[0].pres;
    state.cells[0].presaux = state.cells[0].pres;
    state.cells[1].presauxL = state.cells[0].pres;

    state.cells[0].alf = alfini;
    state.cells[0].alfini = alfini;
    state.cells[0].bet = betini;
    state.cells[0].betini = betini;
    state.cells[1].alfL = state.cells[0].alf;
    state.cells[1].alfLini = state.cells[0].alf;
    state.cells[0].alfPigD = state.cells[0].alf;
    state.cells[0].alfPigDini = state.cells[0].alf;
    state.cells[0].alfPigE = state.cells[0].alf;
    state.cells[0].alfPigEini = state.cells[0].alf;
    state.cells[1].betL = state.cells[0].bet;
    state.cells[1].betLini = state.cells[0].bet;
    state.cells[0].betPigD = state.cells[0].bet;
    state.cells[0].betPigDini = state.cells[0].bet;
    state.cells[0].betPigE = state.cells[0].bet;
    state.cells[0].betPigEini = state.cells[0].bet;
    state.cells[0].betI = state.cells[0].bet;
    state.cells[1].betLI = state.cells[0].bet;

    state.steadyIteration = 0;
    while (state.steadyIteration < 3) {
        i = 1;
        while (i <= state.lastCell && state.cells[i - 1].pres >= 1.) {

            advanceUpstreamSteadyPressure(state, i, 0);
            refreshUpstreamProductionPeriphery(state, i);
            if (state.input.flashCompleto != 2)
                advanceSteadyMass(state, dependencies, i);
            else
                advanceCompositionalSteadyMass(state, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 0);
            if (isnan(state.cells[i].temp))
                NumError("Temperatrura na linha de producao com valor NaN");
            if (state.input.usaTabela == 1 && (state.cells[i].temp - state.input.tabent.tmin) < (*state.globals).smallValueTolerance)
                state.cells[i].temp = state.input.tabent.tmin;
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceDownstreamSteadyPressure(state, i, 0);
            refreshDownstreamProductionPeriphery(state, i);
            if (state.input.tipoFluido == 0)
                advanceSteadyMassTransfer(state, i - 1);
            else
                advanceSteadyGasMassTransfer(state, i - 1);
            if (state.input.ordperm > 1) {
                double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
                double D0pres = state.cells[i].pres - state.cells[i].presaux;
                double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
                advanceUpstreamSteadyPressure(state, i, 1);
                refreshUpstreamProductionPeriphery(state, i);
                advanceSteadyMass(state, dependencies, i);
                thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
                advanceDownstreamSteadyPressure(state, i, 1);
                state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
                state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
                state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
                refreshDownstreamProductionPeriphery(state, i);
                refreshUpstreamProductionPeriphery(state, i);
                thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
                advanceSteadyMass(state, dependencies, i);
                if (state.input.tipoFluido == 0)
                    advanceSteadyMassTransfer(state, i - 1);
                else
                    advanceSteadyGasMassTransfer(state, i - 1);
            }

            i++;
            if (state.cells[i - 1].pres <= state.gasSurfacePressure)
                return -kSteadyMarchLimit;
        }
        state.steadyIteration++;
    }

    state.baseConvergenceMonitor = 1.;
    return 0. - state.cells[state.lastCell - 1].MR;
}

double marchGasSteadySecondary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute, double chutemass) {
    state.gasCells[0].presL = pchute;
    state.gasCells[0].pres = pchute;
    state.gasCells[0].presini = pchute;
    state.gasCells[1].presL = pchute;
    state.gasCells[0].tempL = state.initialGasTemperature;
    state.gasCells[0].temp = state.initialGasTemperature;
    state.gasCells[1].tempL = state.initialGasTemperature;
    state.gasCells[0].rg = state.gasCells[0].flui.MasEspGas(pchute, state.initialGasTemperature);
    state.gasCells[0].u1L = state.gasCells[0].duto.area * state.gasCells[0].rg;
    state.gasCells[0].u1LL = state.gasCells[0].u1L;
    state.gasCells[1].u1LL = state.gasCells[0].u1L;
    state.gasCells[0].VGasL = 0.;
    if (chutemass < 0) // if no chutemass is given in the parameter list,
                       // the JSON's gas injection value is used
        state.gasCells[0].massfonteCH = state.input.gasinj.vazgas[0] * state.gasCells[0].flui.MasEspGas(kStandardPressureKgfPerCm2, kStandardTemperatureCelsius) / units::kSecondsPerDay;
    else
        state.gasCells[0].massfonteCH = chutemass * state.gasCells[0].flui.MasEspGas(kStandardPressureKgfPerCm2, kStandardTemperatureCelsius) / units::kSecondsPerDay;
    state.gasCells[0].VGasR = state.gasCells[0].massfonteCH;
    state.gasCells[1].VGasL = state.gasCells[0].massfonteCH;

    for (int i = 1; i <= state.lastGasCell; i++) { // march along the service line
        gaslift::updateSteadyGasPressure(dependencies.gasLift, i);            // pressure advance from one cell to the next, at the cell centre
        gaslift::updateSteadyGasTemperature(dependencies.gasLift, i);            // temperature advance from one cell to the next, cell centre
        if (isnan(state.gasCells[i].temp))
            NumError("Temperatrura na linha de servico com valor NaN");
        gaslift::computeSteadyGasFlowRate(dependencies.gasLift, dependencies.thermal, i); // if this cell's centre has a gas-lift valve, computes its flow rate
        // and takes it from the line's total flow rate
        state.gasCells[i].rg = state.gasCells[i].flui.MasEspGas(state.gasCells[i].pres, state.gasCells[i].temp);
        state.gasCells[i - 1].rgR = state.gasCells[i].rg;
        state.gasCells[i].u1L = state.gasCells[i].duto.area * state.gasCells[i].rg;
        state.gasCells[i - 1].u1R = state.gasCells[i].u1L;
        if (i < state.lastGasCell)
            state.gasCells[i + 1].u1LL = state.gasCells[i].u1L;
    }
    state.gasCells[state.lastGasCell].rgR = state.gasCells[state.lastGasCell].rg;

    int valveCount = state.input.nvalvgas;
    double mastot = 0.;
    for (int j = 0; j < valveCount; j++)
        mastot += state.gasCells[state.gasValveCellIndices[j]].massfonteCH;
    return mastot - state.gasCells[0].massfonteCH; // difference between the sum of the valve flow rates
    // and the line's injection flow rate
}

double marchGasSteadyTertiary(const SteadyStateState &state, const SteadyDependencies &dependencies, double pchute) {
    state.gasCells[0].presL = pchute; // pressure downstream of the injection choke
    state.gasCells[0].pres = pchute;
    state.gasCells[0].presini = pchute;
    state.gasCells[1].presL = pchute;
    state.gasCells[0].tempL = state.initialGasTemperature;
    state.gasCells[0].temp = state.initialGasTemperature;
    state.gasCells[1].tempL = state.initialGasTemperature;
    state.gasCells[0].rg = state.gasCells[0].flui.MasEspGas(pchute, state.initialGasTemperature);
    state.gasCells[0].u1L = state.gasCells[0].duto.area * state.gasCells[0].rg;
    state.gasCells[0].u1LL = state.gasCells[0].u1L;
    state.gasCells[1].u1LL = state.gasCells[0].u1L;
    state.gasCells[0].VGasL = 0.;
    state.injectionChoke.presGarg = pchute;
    double chutemass = state.injectionChoke.massica(); // line injection flow rate obtained from the
    // injection choke's mass flow rate
    state.gasCells[0].massfonteCH = chutemass;
    state.gasCells[0].VGasR = state.gasCells[0].massfonteCH;
    state.gasCells[1].VGasL = state.gasCells[0].massfonteCH;

    for (int i = 1; i <= state.lastGasCell; i++) { // march along the gas line
        gaslift::updateSteadyGasPressure(dependencies.gasLift, i);            // pressure advance from one cell to the next, at the cell centre
        gaslift::updateSteadyGasTemperature(dependencies.gasLift, i);            // temperature advance from one cell to the next, cell centre
        if (isnan(state.gasCells[i].temp))
            NumError("Temperatura na linha de servico com valor NaN");
        gaslift::computeSteadyGasFlowRate(dependencies.gasLift, dependencies.thermal, i); // if this cell's centre has a gas-lift valve, computes its flow rate
        // and takes it from the line's total flow rate
        state.gasCells[i].rg = state.gasCells[i].flui.MasEspGas(state.gasCells[i].pres, state.gasCells[i].temp);
        state.gasCells[i - 1].rgR = state.gasCells[i].rg;
        state.gasCells[i].u1L = state.gasCells[i].duto.area * state.gasCells[i].rg;
        state.gasCells[i - 1].u1R = state.gasCells[i].u1L;
        if (i < state.lastGasCell)
            state.gasCells[i + 1].u1LL = state.gasCells[i].u1L;
    }
    state.gasCells[state.lastGasCell].rgR = state.gasCells[state.lastGasCell].rg;

    int valveCount = state.input.nvalvgas;
    double mastot = 0.;
    for (int j = 0; j < valveCount; j++)
        mastot += state.gasCells[state.gasValveCellIndices[j]].massfonteCH;
    return mastot - chutemass; // difference between the sum of the valve flow rates
    // and the line's injection flow rate
}

double marchInjectionSteady(const SteadyStateState &state, const SteadyDependencies &dependencies, double chute) {

    if (state.input.flashCompleto < 1)
        state.cells[0].temp = state.cells[0].acsr.injl.temp;
    else
        state.cells[0].temp = state.cells[0].acsr.injg.temp;
    double alfini = 0.;
    double betini = 1.;
    double delp = 0.;
    if (state.input.condpocinj.CC == 1 || state.input.condpocinj.CC == 2 || state.input.condpocinj.CC == 3) {
        if ((state.surfaceChoke.AreaGarg / state.surfaceChoke.AreaTub) >= 0.6) {
            if (state.input.flashCompleto < 1) {
                state.cells[0].acsr.injl.QLiq = chute;
            } else {
                state.cells[0].acsr.injg.QGas = chute;
                if (state.cells[0].acsr.injg.seco == 1) {
                    alfini = 1.;
                    betini = 0.;
                } else {
                    if (state.input.flashCompleto == 2) {
                        if (state.input.tabelaDinamica == 0)
                            state.cells[0].flui.atualizaPropComp(state.gasSurfacePressure, state.cells[0].acsr.injg.temp, -1, NULL, NULL, state.input.pocinjec);
                        state.cells[0].acsr.injg.FluidoPro.atualizaPropComp(state.gasSurfacePressure, state.cells[0].acsr.injg.temp, -1, NULL, NULL, state.input.pocinjec);
                    }
                    double masgas = state.cells[0].acsr.injg.VMas(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    double quality;
                    if (state.input.flashCompleto != 2)
                        quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(1., 20.);
                    else
                        quality = state.cells[0].acsr.injg.FluidoPro.dStockTankVaporMassFraction;
                    double masT = masgas / quality;
                    quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    double qgas = masT * quality /
                                  state.cells[0].acsr.injg.FluidoPro.MasEspGas(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    double qliq = masT * (1. - quality) /
                                  state.cells[0].acsr.injg.FluidoPro.MasEspLiq(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    alfini = qgas / (qliq + qgas);
                    betini = 0.;
                }
            }
        } else {
            if (state.input.flashCompleto < 1) {
                state.cells[0].acsr.injl.QLiq = chute;
                delp = (1 / (state.cells[0].fluicol.MasEspFlu(state.gasSurfacePressure, state.cells[0].acsr.injl.temp) * state.surfaceChoke.cdchk * 2.)) * pow((chute * state.cells[0].fluicol.standardDensity() / units::kSecondsPerDay) / (state.surfaceChoke.AreaGarg), 2.) / units::kPascalPerKgfPerCm2;
            } else {
                state.cells[0].acsr.injg.QGas = chute;
                if (state.input.flashCompleto == 2) {
                    if (state.input.tabelaDinamica == 0)
                        state.cells[0].flui.atualizaPropComp(state.gasSurfacePressure, state.cells[0].acsr.injg.temp, -1, NULL, NULL, state.input.pocinjec);
                    state.cells[0].acsr.injg.FluidoPro.atualizaPropComp(state.gasSurfacePressure, state.cells[0].acsr.injg.temp, -1, NULL, NULL, state.input.pocinjec);
                }
                double rhogstd = state.cells[0].flui.Deng * kAirDensityAtStandardConditions;
                delp = (1 / (state.cells[0].flui.MasEspGas(state.gasSurfacePressure, state.cells[0].acsr.injg.temp) * state.surfaceChoke.cdchk * 2.)) * pow((chute * rhogstd / units::kSecondsPerDay) / (state.surfaceChoke.AreaGarg), 2.) / units::kPascalPerKgfPerCm2;
                if (state.cells[0].acsr.injg.seco == 1) {
                    alfini = 1.;
                    betini = 0.;
                } else {
                    double masgas = state.cells[0].acsr.injg.VMas(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    double quality;
                    if (state.input.flashCompleto != 2)
                        quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(1., 20.);
                    else
                        quality = state.cells[0].acsr.injg.FluidoPro.dStockTankVaporMassFraction;
                    double masT = masgas / quality;
                    quality = state.cells[0].acsr.injg.FluidoPro.FracMassHidra(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    double qgas = masT * quality /
                                  state.cells[0].acsr.injg.FluidoPro.MasEspGas(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    double qliq = masT * (1. - quality) /
                                  state.cells[0].acsr.injg.FluidoPro.MasEspLiq(state.gasSurfacePressure, state.cells[0].acsr.injg.temp);
                    alfini = qgas / (qliq + qgas);
                    betini = 0.;
                }
            }
        }
    }
    state.cells[0].tempL = state.cells[0].temp;
    state.cells[1].tempL = state.cells[0].temp;
    state.cells[0].tempini = state.cells[0].temp;

    state.cells[0].ML = 0.;
    state.cells[0].MC = 0.;
    state.cells[1].ML = 0.;
    state.cells[0].MliqiniL = 0.;
    state.cells[0].Mliqini = 0.;
    state.cells[1].MliqiniL = 0.;
    state.cells[0].QLL = 0.;
    state.cells[0].QL = 0;
    state.cells[1].QLL = 0.;
    state.cells[0].QG = 0.;
    if (state.input.condpocinj.CC == 0 || state.input.condpocinj.CC == 5) {
        state.gasSurfacePressure = chute;
    }
    state.steadyIteration = 0;

    double masfim = 0.;

    int i;
    int guessNeedsCorrection = 1;
    while (guessNeedsCorrection == 1) {
        state.cells[0].presauxL = state.gasSurfacePressure - delp;
        state.cells[0].presLini = state.gasSurfacePressure - delp;
        state.cells[0].presL = state.gasSurfacePressure - delp;
        state.cells[0].pres = state.gasSurfacePressure - delp;
        state.cells[1].presL = state.gasSurfacePressure - delp;
        state.cells[0].presini = state.gasSurfacePressure - delp;
        state.cells[1].presLini = state.gasSurfacePressure - delp;
        state.cells[0].presaux = state.gasSurfacePressure - delp;
        state.cells[1].presauxL = state.gasSurfacePressure - delp;
        state.cells[0].alf = alfini;
        state.cells[0].alfini = alfini;
        state.cells[0].bet = betini;
        state.cells[0].betini = betini;
        state.cells[1].alfL = state.cells[0].alf;
        state.cells[1].alfLini = state.cells[0].alf;
        state.cells[0].alfPigD = state.cells[0].alf;
        state.cells[0].alfPigDini = state.cells[0].alf;
        state.cells[0].alfPigE = state.cells[0].alf;
        state.cells[0].alfPigEini = state.cells[0].alf;
        state.cells[1].betL = state.cells[0].bet;
        state.cells[1].betLini = state.cells[0].bet;
        state.cells[0].betPigD = state.cells[0].bet;
        state.cells[0].betPigDini = state.cells[0].bet;
        state.cells[0].betPigE = state.cells[0].bet;
        state.cells[0].betPigEini = state.cells[0].bet;
        state.cells[0].betI = state.cells[0].bet;
        state.cells[1].betLI = state.cells[0].bet;
        i = 1;
        while (i <= state.lastCell && state.cells[i - 1].pres >= 1. && masfim >= -(*state.globals).smallValueTolerance) {

            advanceUpstreamSteadyPressure(state, i, 0);
            refreshUpstreamProductionPeriphery(state, i);
            if (state.input.flashCompleto != 2)
                advanceSteadyMass(state, dependencies, i);
            else
                advanceCompositionalSteadyMass(state, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 0);
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceDownstreamSteadyPressure(state, i, 0);
            refreshDownstreamProductionPeriphery(state, i);
            advanceSteadyMassTransfer(state, i - 1);
            if (state.input.ordperm > 1) { // second-order correction
                double D0presaux = state.cells[i].presaux - state.cells[i - 1].pres;
                double D0pres = state.cells[i].pres - state.cells[i].presaux;
                double D0temp = state.cells[i].temp - state.cells[i - 1].temp;
                advanceUpstreamSteadyPressure(state, i, 1);
                refreshUpstreamProductionPeriphery(state, i);
                advanceSteadyMass(state, dependencies, i);
                thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
                advanceDownstreamSteadyPressure(state, i, 1);
                state.cells[i].pres = 0.5 * (state.cells[i].presaux + D0pres + state.cells[i].pres);
                state.cells[i].presaux = 0.5 * (state.cells[i - 1].pres + D0presaux + state.cells[i].presaux);
                state.cells[i].temp = 0.5 * (state.cells[i - 1].temp + D0temp + state.cells[i].temp);
                refreshDownstreamProductionPeriphery(state, i);
                refreshUpstreamProductionPeriphery(state, i);
                thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
                advanceSteadyMass(state, dependencies, i);
                advanceSteadyMassTransfer(state, i - 1);
            }
            masfim += (state.cells[i - 1].fontemassCR + state.cells[i - 1].fontemassLR + state.cells[i - 1].fontemassGR);
            i++;
            if (state.cells[i - 1].pres <= 1 || (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.cells[i - 1].acsr.ipr.Pres - state.cells[i - 1].pres) > -(*state.globals).smallValueTolerance))
                return -kSteadyMarchLimit;
            else if (i < (state.lastCell + 1) && masfim < -(*state.globals).smallValueTolerance)
                return kSteadyMarchLimit;
        }
        if (i == state.lastCell + 1)
            guessNeedsCorrection = 0;
        else if (!(state.cells[i - 1].pres >= 1.))
            return -kSteadyMarchLimit; // stopped on the pressure, as the march's own pressure test
        else
            return kSteadyMarchLimit; // stopped on the mass flow, as the march's own flow test
    }
    sources::renewSourceTerms(dependencies.sources, state.lastCell);
    masfim += (state.cells[state.lastCell].fontemassCR + state.cells[state.lastCell].fontemassLR + state.cells[state.lastCell].fontemassGR);

    if (state.input.condpocinj.CC != 3 && state.input.condpocinj.CC != 5)
        return masfim;
    else
        return state.input.condpocinj.presfundo - state.cells[state.lastCell].pres;
}

double reverseHydrostatic(const SteadyStateState &state, double liquidHoldup, double liquidFlowRate, double gasFlowRate) {
    double pchute = state.gasSurfacePressure;
    double taux;
    state.cells[state.lastCell].pres = pchute;
    double j = 0.;
    double rmis = 0.;
    double frictionFactor = 0.;
    for (int i = state.lastCell; i > 0; i--) {
        if (liquidFlowRate > 0. || gasFlowRate > 0.) {
            taux = state.input.celp[0].textern;
            double complementaryFraction = 0.;
            double visC = state.cells[i].fluicol.VisFlu(pchute, taux);
            double visP = state.cells[i].flui.ViscOleo(pchute, taux);
            double visG = state.cells[i].flui.ViscGas(pchute, taux);
            double visMis = (1 - complementaryFraction) * visP + complementaryFraction * visC;
            double complementaryDensityAtGuess = state.cells[i].fluicol.MasEspFlu(pchute, taux);
            double liquidDensityAtGuess = state.cells[i].flui.MasEspLiq(pchute, taux);
            double gasDensityAtGuess = state.cells[i].flui.MasEspGas(pchute, taux);
            rmis = (1 - complementaryFraction) * liquidDensityAtGuess + complementaryFraction * complementaryDensityAtGuess;
            double rlpA = state.cells[i].flui.MasEspLiq(1., 15.);
            double rlcA = state.cells[i].fluicol.MasEspFlu(1.001, 15.);
            double massicC = rlcA * liquidFlowRate * complementaryFraction;
            double massic = rlpA * liquidFlowRate * (1. - complementaryFraction);
            double Rhogs = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
            double Rhols = (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) * (1 - state.cells[i].flui.BSW) + 1000. * state.cells[i].flui.Denag * state.cells[i].flui.BSW;
            double multiplicador = (Rhols + state.cells[i].flui.RGO * Rhogs * (1 - state.cells[i].flui.BSW));
            massic = 1 * liquidFlowRate * (1. - complementaryFraction) * multiplicador;
            double fracmasshidra = state.cells[i].flui.FracMassHidra(pchute, taux);
            double massicP = (1. - fracmasshidra) * massic;
            double massicG = fracmasshidra * massic + gasFlowRate * Rhogs;
            j = (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess) / state.cells[i].duto.area;
            double alfmis = (massicG / gasDensityAtGuess) / (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess);
            rmis = (1 - alfmis) * rmis + alfmis * gasDensityAtGuess;
            visMis = (1 - alfmis) * visMis + alfmis * visG;
            double mixtureReynolds;
            if (state.cells[i].duto.revest == 0)
                mixtureReynolds = state.cells[0].Rey(state.cells[i].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.cells[i].duto.area / state.cells[0].duto.peri;
                mixtureReynolds = state.cells[i].Rey(dhid, j, rmis, visMis);
            }
            frictionFactor = state.cells[i].fric(mixtureReynolds, state.cells[0].duto.rug / state.cells[0].duto.a);
        }
        double perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.cells[i].duto.peri / state.cells[i].duto.area;
        taux = state.input.celp[i].textern;
        double rhol = state.cells[i].flui.MasEspLiq(pchute, taux);
        double rhog = state.cells[i].flui.MasEspGas(pchute, taux);
        double alfa = 1. - liquidHoldup;
        double rhomix = (1. - alfa) * rhol + alfa * rhog;
        double dxmed = 0.5 * (state.cells[i].dx + state.cells[i - 1].dx);
        pchute += ((rhomix * 9.81 * sin(state.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2);
        if (state.cells[i - 1].acsr.tipo == 7)
            pchute -= state.cells[i - 1].acsr.delp;
        if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.cells[i - 1].acsr.ipr.Pres - pchute) < (*state.globals).smallValueTolerance)
            pchute = 0.99 * state.cells[i - 1].acsr.ipr.Pres;
        if (state.cells[i - 1].acsr.tipo == kAccessoryRadialPorous && (state.cells[i - 1].acsr.radialPoro.pRes[0] - pchute) < (*state.globals).smallValueTolerance)
            pchute = 0.99 * state.cells[i - 1].acsr.radialPoro.pRes[0];
        if (state.cells[i - 1].acsr.tipo == kAccessoryPorous2D && (state.cells[i - 1].acsr.poroso2D.dados.pRes - pchute) < (*state.globals).smallValueTolerance)
            pchute = 0.99 * state.cells[i - 1].acsr.poroso2D.dados.pRes;

        state.cells[i - 1].acsr.dpB = 0.;
        if (state.cells[i - 1].acsr.tipo == kAccessoryPump && state.cells[i - 1].acsr.bcs.freqnova > 1. && liquidFlowRate >= 0.) {
            double vazmix = j * state.cells[i - 1].dutoL.area;
            double rhomis = state.cells[i - 1].flui.MasEspLiq(pchute, taux);
            double vismis = state.cells[i - 1].flui.ViscOleo(pchute, taux);
            vazmix *= (units::kSecondsPerDay / units::kCubicMetrePerBarrel);
            state.cells[i - 1].acsr.bcs.NovaVis(vismis, rhomis, vazmix);
            state.cells[i - 1].acsr.dpB = 0.3048 * state.cells[i - 1].acsr.bcs.Hvis * rhomis * kGravity;
        }
        if (state.cells[i - 1].acsr.tipo == kAccessoryMultiPump && state.cells[i - 1].acsr.multibcs.freqnova > 1. && liquidFlowRate >= 0.) {
            double alf0 = state.cells[i - 1].alf;
            double bet0 = state.cells[i - 1].bet;
            state.cells[i - 1].acsr.multibcs.flui = state.cells[i - 1].flui;
            state.cells[i - 1].acsr.multibcs.fluicol = state.cells[i - 1].fluicol;
            state.cells[i - 1].acsr.multibcs.marchaMultiBcs(state.cells[i - 1].QG, state.cells[i - 1].QL,
                                                       pchute, taux, alf0, bet0);
            state.cells[i - 1].acsr.dpB = state.cells[i - 1].acsr.multibcs.dpB * units::kPascalPerKgfPerCm2;
        }
        pchute -= state.cells[i - 1].acsr.dpB / units::kPascalPerKgfPerCm2;
        state.cells[i - 1].pres = pchute;
    }
    return pchute;
}

double reverseInjectionHydrostatic(const SteadyStateState &state, double liquidHoldup, double liquidFlowRate) {
    double pchute = 0.;
    if (state.input.condpocinj.presfundo > 1e-5)
        pchute = state.input.condpocinj.presfundo;
    else if (state.cells[state.lastCell].acsr.tipo == kAccessoryInflowPerformance)
        pchute = state.cells[state.lastCell].acsr.ipr.Pres;
    else
        NumError("Sem pressao no fim do tramo e sem IPR-metodo hidroreversoInj");
    double taux;
    double rmis = 0.;
    double j = 0.;
    double frictionFactor = 0.;

    if (liquidFlowRate > 0.) {
        taux = state.input.celp[0].textern;
        state.cells[state.lastCell].pres = pchute;
        double visC = state.cells[0].acsr.injl.fluidocol.VisFlu(pchute, taux);
        double visMis = visC;
        double complementaryDensityAtGuess = state.cells[0].acsr.injl.fluidocol.MasEspFlu(pchute, taux);
        rmis = complementaryDensityAtGuess;
        double rlcA = state.cells[0].acsr.injl.fluidocol.MasEspFlu(1.001, 15.);
        double massicC = rlcA * liquidFlowRate;
        j = (massicC / complementaryDensityAtGuess) / state.cells[0].duto.area;
        double mixtureReynolds;
        if (state.cells[0].duto.revest == 0)
            mixtureReynolds = state.cells[0].Rey(state.cells[0].duto.a, j, rmis, visMis);
        else {
            double dhid = 4 * state.cells[0].duto.area / state.cells[0].duto.peri;
            mixtureReynolds = state.cells[0].Rey(dhid, j, rmis, visMis);
        }
        frictionFactor = state.cells[0].fric(mixtureReynolds, state.cells[0].duto.rug / state.cells[0].duto.a);
    }
    double perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.cells[0].duto.peri / state.cells[0].duto.area;
    for (int i = state.lastCell; i > 0; i--) {
        taux = state.input.celp[i].textern;
        double rhol = state.cells[i].fluicol.MasEspFlu(pchute, taux);
        double rhog = 0.;
        double alfa = 1. - liquidHoldup;
        double rhomix = (1. - alfa) * rhol + alfa * rhog;
        double dxmed = 0.5 * (state.cells[i].dx + state.cells[i - 1].dx);
        pchute += ((rhomix * 9.81 * sin(state.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2);
        if (state.cells[i - 1].acsr.tipo == 7)
            pchute -= state.cells[i - 1].acsr.delp;
        if (state.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.cells[i - 1].acsr.ipr.Pres - pchute) < (*state.globals).smallValueTolerance)
            pchute = 0.99 * state.cells[i - 1].acsr.ipr.Pres;
        state.cells[i - 1].pres = pchute;
    }
    return pchute;
}

double secondaryBranchHydrostatic(const SteadyStateState &state, double titulo) {
    double pchute;
    if (state.input.ConContEntrada == 1)
        pchute = state.input.CCPres.pres[0];
    else
        pchute = state.cells[0].pres;
    double taux;
    double complementaryFraction;
    if (state.input.ConContEntrada == 1)
        complementaryFraction = state.input.CCPres.bet[0];
    else
        complementaryFraction = state.cells[0].bet;
    state.cells[0].bet = complementaryFraction;
    taux = state.input.celp[0].textern;
    double titRef = state.cells[0].flui.FracMassHidra(pchute, taux);
    if (state.input.ConContEntrada == 1)
        state.cells[0].pres = state.input.CCPres.pres[0];
    state.cells[0].temp = taux;
    double rhol = state.cells[0].flui.MasEspLiq(state.cells[0].pres, taux);
    double rhoc = state.cells[0].fluicol.MasEspFlu(state.cells[0].pres, taux);
    double rhog = state.cells[0].flui.MasEspGas(state.cells[0].pres, taux);
    double titEntra;
    if (state.input.ConContEntrada == 1)
        titEntra = state.input.CCPres.tit[0];
    else
        titEntra = titulo;
    state.cells[0].alf = titEntra * (rhol * (1. - complementaryFraction) + rhoc * complementaryFraction) / (rhog * (1. - titEntra) + titEntra * (rhol * (1. - complementaryFraction) + rhoc * complementaryFraction));
    for (int i = 0; i < state.lastCell; i++) {
        taux = state.input.celp[i].textern;
        rhol = state.cells[i].flui.MasEspLiq(pchute, taux);
        rhoc = state.cells[i].fluicol.MasEspFlu(pchute, taux);
        rhog = state.cells[i].flui.MasEspGas(pchute, taux);
        double tit0 = state.cells[i].flui.FracMassHidra(pchute, taux);
        double delTit = tit0 - titRef;
        double novoTit = titulo + delTit * (titulo / titRef);
        double alfa = novoTit * (rhol * (1. - complementaryFraction) + rhoc * complementaryFraction) / (rhog * (1. - novoTit) + novoTit * (rhol * (1. - complementaryFraction) + rhoc * complementaryFraction));
        state.cells[i + 1].alf = alfa;
        state.cells[i + 1].bet = complementaryFraction;

        double rhomix = (1. - alfa) * ((1. - complementaryFraction) * rhol + complementaryFraction * rhoc) + alfa * rhog;
        double dxmed = 0.5 * (state.cells[i].dx + state.cells[i + 1].dx);
        pchute -= ((rhomix * 9.81 * sin(state.cells[i].duto.teta) * dxmed) / units::kPascalPerKgfPerCm2);
        state.cells[i + 1].pres = pchute;
        state.cells[i + 1].temp = taux;
    }
    return pchute;
}

}  // namespace sisprod::steady
