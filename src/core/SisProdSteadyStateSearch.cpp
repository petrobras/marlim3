#include "DriftFluxClosure.h"
#include "SisProdSources.h"
#include "SisProdGasLift.h"
#include "SisProdThermal.h"
#include "SisProdSteadyStateSearch.h"
#include "SisProdPressureMarchInternal.h"

#include "RootFindingSolvers.h"

#include "Leitura.h"
#include "SisProdConstants.h"
#include "celula3.h"
#include "celulaGas.h"
#include "chokegas.h"
#include "SimulationContext.h"
#include "UnitConversions.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <locale>
#include <math.h>
#include <sstream>

namespace sisprod::steady {

namespace {

/// Estimates inlet mixture velocity from the current cell properties and a standard source flow rate.
double estimateInletMixtureVelocity(const SteadyStateSearchState &state, double standardFlowRateGuess) {
    double complementaryFraction = state.march.cells[1].bet;
    double voidFraction = state.march.cells[1].alf;
    double firstCellPressure = state.march.cells[0].pres;
    double firstCellTemperature = state.march.cells[0].temp;
    double complementaryDensityAtGuess = state.march.cells[0].fluicol.MasEspFlu(firstCellPressure, firstCellTemperature);
    double liquidDensityAtGuess = state.march.cells[0].flui.MasEspLiq(firstCellPressure, firstCellTemperature);
    double gasDensityAtGuess = state.march.cells[0].flui.MasEspGas(firstCellPressure, firstCellTemperature);
    double rmisL = (1 - complementaryFraction) * liquidDensityAtGuess + complementaryFraction * complementaryDensityAtGuess;
    double rmis = (1 - voidFraction) * rmisL + voidFraction * gasDensityAtGuess;
    double rCst = state.march.cells[0].fluicol.MasEspFlu(1., 20.);
    double rPst = state.march.cells[0].flui.MasEspLiq(1., 20.);
    double rGst = state.march.cells[0].flui.MasEspGas(1., 20.);
    double rmisLst = (1 - complementaryFraction) * rPst + complementaryFraction * rCst;
    double multiplica;
    if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection)
        multiplica = rmisLst;
    else
        multiplica = rGst;
    return standardFlowRateGuess * multiplica / (rmis * state.march.cells[0].duto.area * units::kSecondsPerDay);
}

/// Identifies the steady march selected by the well type and boundary conditions.
enum class SteadyMarch {
    marchaInjPerm1,
    marchaGasPerm2,
    marchaGasPerm3,
    marchaProdPerm1,
    marchaProdPerm1Rev,
    marchaProdPerm2,
    marchaProdPresPres1,
    marchaProdPresPres1Rev,
    marchaProdPresPres2,
};

/// Resolves the four selectors to one row of the table: a pure function of
/// five values.
///
/// productionChokeOpening is the array, not the value, so that the subscript
/// happens only in the branch that needs it: Ler::copia_chokeSup leaves
/// chokep.abertura null when parserie is not positive, and reading it on every
/// dispatch would turn a conditional dereference into an unconditional one.
SteadyMarch selectSteadyMarch(int injectorWell, int prod, int tipoCC, int reverseMarch,
                              const double *productionChokeOpening) {
    if (injectorWell != 0)
        return SteadyMarch::marchaInjPerm1;
    if (prod == 0)
        return tipoCC == 0 ? SteadyMarch::marchaGasPerm2 : SteadyMarch::marchaGasPerm3;
    if (prod == 1) {
        if (tipoCC != 0)
            return SteadyMarch::marchaProdPerm2;
        return reverseMarch == 0 ? SteadyMarch::marchaProdPerm1
                                 : SteadyMarch::marchaProdPerm1Rev;
    }
    if (tipoCC != 0) {
        // The secondary march handles both open and closed surface chokes.
        return productionChokeOpening[0] > 1e-15 ? SteadyMarch::marchaProdPresPres2
                                                 : SteadyMarch::marchaProdPresPres2;
    }
    return reverseMarch == 0 ? SteadyMarch::marchaProdPresPres1
                             : SteadyMarch::marchaProdPresPres1Rev;
}

/// Brackets and solves the root for the reverse search.
///
/// Handles both residual signs using the reverse march boundary conditions.
double bracketReverseRoot(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double aumenta, double reduz, double &guessLowerBound, double &positiveResidualGuess, double &negativeResidualGuess, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute2, double pchute, double chute) {
    if (marchResidual < 0.) { // case where the upstream choke pressure < the pressure of the last cell computed by the
        // march: the pressure guess is high, so now look for
        // a low pressure guess that makes marchResidual positive, to start the
        // root finding
        negativeResidualGuess = pchute; // storing the pressure guess that gives the negative value
        // at each new search in which marchResidual gets closer to zero but is still negative
        // negativeResidualGuess is updated with the last pressure guess
        while (marchResidual < 0) {
            if (fabs(pchute2 - pchute) / pchute < (1. - reduz) / 10. && kontaiter > 100) {
                pchute2 *= 0.5;
            }
            pchuteAux = pchute2;
            pchute2 *= reduz; // Lowering the pressure in search of marchResidual>0
            if (pchute2 <= guessLowerBound)
                pchute2 = 0.5 * (pchuteAux + guessLowerBound); // guessLowerBound is initially
            // zero, but pchute2 may be lowered so far that marchResidual=-1e10
            // (pressure below 0.5 somewhere in the march); then guessLowerBound becomes this value of
            // pchute2, as it is then known that one cannot go below guessLowerBound
            marchResidual = marchReverseProductionSteady(state.march, dependencies, pchute2);
            if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                negativeResidualGuess = pchute2; // updating negativeResidualGuess
            kontaiter++;
            if (kontaiter > 100) { // iteration limit: failed to find the second guess
                // end of the simulation or a failure warning
                if ((*state.march.globals).networkMode == 0) {
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                        NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                    else {
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                } else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            while (marchResidual < -kSteadyMarchSentinelThreshold) { // the pressure reduction was too large and the march could not
                // reach the end without the pressure falling below 0.5 kgf/cm2;
                // the low pressure estimate must be raised
                guessLowerBound = pchute2;
                pchute2 = 0.5 * (pchute2 + pchuteAux); // intermediate value between the lowest pressure
                // that gives marchResidual<0 and the pressure that is too low
                marchResidual = marchReverseProductionSteady(state.march, dependencies, pchute2);
                if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = pchute2;
                kontaiter++;
                if (kontaiter > 100) {
                    if ((*state.march.globals).networkMode == 0) {
                        if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                            NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                        else {
                            cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    } else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
        }
        positiveResidualGuess = pchute2;
    } else if (marchResidual > 0.) { // case where the upstream choke pressure > the pressure of the last cell
        // computed by the
        // march: the pressure guess is low, so now look for
        // a high pressure guess that makes marchResidual negative, to start the
        // root finding
        positiveResidualGuess = pchute; // storing the pressure guess that gives the positive value
        // at each new search in which marchResidual gets closer to zero but is still positive
        // positiveResidualGuess is updated with the last pressure guess
        while (marchResidual > 0) {
            pchuteAux = pchute2;
            pchute2 *= aumenta; // Raising the pressure in search of marchResidual>0
            marchResidual = marchReverseProductionSteady(state.march, dependencies, pchute2);
            if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
                positiveResidualGuess = pchute2; // updating positiveResidualGuess
            kontaiter++;
            if (kontaiter > 100) { // iteration limit: failed to find the second guess
                // end of the simulation or a failure warning
                if ((*state.march.globals).networkMode == 0) {
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                        NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                    else {
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                } else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            while (marchResidual > kSteadyMarchSentinelThreshold) { // the pressure increment was too large and the march could not
                // reach the end without the pressure rising above the static pressure
                // of an IPR along the march, or above the maximum pressure
                // of the PVTSim table, when there is one;
                // the low pressure estimate must be lowered
                guessLowerBound = pchute2;
                pchute2 = 0.5 * (pchute2 + pchuteAux); // intermediate value between the highest pressure
                // that gives marchResidual>0 and the pressure that is too high
                marchResidual = marchReverseProductionSteady(state.march, dependencies, pchute2);
                if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = pchute2;
                kontaiter++;
                if (kontaiter > 100) {
                    if ((*state.march.globals).networkMode == 0) {
                        if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                            NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                        else {
                            cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;

                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    } else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
        }
        negativeResidualGuess = pchute2;
    }

    return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 1, 0); // with the two pressure estimates
    // on opposite-sign sides of the curve, the root finding
    // starts
}

/// Walks the guess until the reverse march stops returning a sentinel.
bool retryUntilReverseMarchCompletes(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double pchuteAux0, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute, double chute, double &abortValue) {
    if ((marchResidual < -kSteadyMarchSentinelThreshold || marchResidual > kSteadyMarchSentinelThreshold) && kontaiter <= 100) {
        double valtemp;
        valtemp = marchReverseProductionSteady(state.march, dependencies, pchuteAux);   // march with pchuteAux
        while (valtemp > kSteadyMarchSentinelThreshold && marchResidual > kSteadyMarchSentinelThreshold) { // bottom-hole pressure estimate still high
            pchuteAux *= 0.99;                     // lowering the estimate
            valtemp = marchReverseProductionSteady(state.march, dependencies, pchuteAux);
            kontaiter++; // at most 50 iterations
        }
        while (valtemp < -kSteadyMarchSentinelThreshold && marchResidual < -kSteadyMarchSentinelThreshold && kontaiter < 100) { // bottom-hole pressure estimate still low
            if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection || state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection || state.march.cells[0].acsr.tipo == kAccessoryMultipleSource)
                pchuteAux *= 1.1;
            else
                pchuteAux *= 1.01; // raising the estimate
            // checking whether this raise exceeds the pressure limit of an IPR at the
            // bottom
            int limpres = 0;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                (state.march.cells[0].acsr.ipr.Pres - pchuteAux) < -0.01 * state.march.cells[0].acsr.ipr.Pres) {

                pchuteAux = (10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchuteAux > 1.01 * state.march.cells[0].acsr.ipr.Pres)
                    pchuteAux = 1.01 * state.march.cells[0].acsr.ipr.Pres;

                limpres = 1; // flag: this pressure step is too large;
                // further steps must be smaller
            }
            // checking whether this raised estimate is above the maximum pressure
            // of a PVTSim table, if there is one
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
            valtemp = marchReverseProductionSteady(state.march, dependencies, pchuteAux); // new attempt
            if (valtemp < -kSteadyMarchSentinelThreshold && limpres == 1) { // still high, and the flag says
                // a smaller pressure step must be used
                int iterpres = 0;
                while (valtemp < -kSteadyMarchSentinelThreshold && iterpres < 10) { // new loop with a smaller step,
                    // at most 10 iterations
                    pchuteAux *= 1.001;
                    valtemp = marchReverseProductionSteady(state.march, dependencies, pchuteAux);
                    iterpres++;
                }
                if (iterpres >= 10) {
                    // case where the maximum number of pressure steps was reached with a
                    // small step: returns a warning that something went wrong, or ends the simulation
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                        // in this case the simulation ends: there is no transient
                        // to run next and this is not a network
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                    else {
                        // only shows a warning
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                        // if in a network iteration, after the first iteration
                        if ((*state.march.globals).networkIteration > 0)
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
            }
            if (kontaiter == 100) {
                pchuteAux = 0.5 * pchuteAux0;
            }
            kontaiter++;
        }
        if (valtemp > -kSteadyMarchSentinelThreshold && valtemp < kSteadyMarchSentinelThreshold) { // the march reached the end
            marchResidual = valtemp;
            pchute = pchuteAux;
        }
    }
    return false;
}

/// Moves the guess according to which sentinel the reverse march returned.
void classifyReverseMarchSentinel(const SteadyStateSearchState &state, double marchResidual, double &pchuteAux, double perdafric) {
    double taux;
    if (marchResidual < -kSteadyMarchSentinelThreshold) { // pressure too low in the march: the guess must be raised
        // a new estimate is made, now assuming a water hydrostatic head,
        // which gives a higher bottom-hole pressure
        pchuteAux = state.march.gasSurfacePressure;
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            double rhol = 1000 + 0 * state.march.cells[i].flui.MasEspLiq(pchuteAux, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchuteAux, taux);
            double alfa = 0.;
            if (state.march.annulusDrift == 0)
                alfa = 1.;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            pchuteAux += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            if (state.march.cells[i - 1].acsr.tipo == 7)
                pchuteAux -= state.march.cells[i - 1].acsr.delp;
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchuteAux) > -0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                pchuteAux = (10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchuteAux > 1.01 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchuteAux = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
        }
        if (pchuteAux > 1100)
            pchuteAux = 1100;  // pchuteAux limit
    } else if (marchResidual > kSteadyMarchSentinelThreshold) { // the pressure somewhere went above a static pressure;
        // the guess must be lowered
        pchuteAux = state.march.gasSurfacePressure;
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchuteAux, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchuteAux, taux);
            // in this case a high void fraction is used for the hydrostatics
            double alfa = 0.8;
            if (state.march.annulusDrift == 0)
                alfa = 1.;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.2 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            pchuteAux += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            if (state.march.cells[i - 1].acsr.tipo == 7)
                pchuteAux -= state.march.cells[i - 1].acsr.delp;
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchuteAux) > -0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                pchuteAux = (10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchuteAux > 1.01 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchuteAux = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
        }
        if (pchuteAux > 1100)
            pchuteAux = 1100;
    }
}

/// Estimates the bottom-hole pressure the reverse search starts from.
void estimateInitialReverseBottomHolePressure(const SteadyStateSearchState &state, double &perdafric, double &pchute, double chute) {
    double j = 0.;
    double rmis = 0.;
    double frictionFactor = 0.;
    double taux;
    double complementaryFractionGuess = 0.;

    if (chute < 0) {

        if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection && fabs(state.march.cells[0].acsr.injl.QLiq) > 0.) {
            // this block estimates the mean pressure loss
            // from the flow rate at the start of the pipe; this is done only
            // if there is a liquid source, celula[0].acsr.tipo == 2
            taux = state.march.input.celp[0].textern; // the temperature of this estimate
            // is the source's; the pressure for the properties
            // is pGSup
            if (taux < state.march.input.tmin)
                taux = state.march.input.tmin;
            /////////// physical properties:
            double complementaryFraction = state.march.cells[0].acsr.injl.bet;
            complementaryFractionGuess = complementaryFraction;
            double visC = state.march.cells[0].acsr.injl.fluidocol.VisFlu(pchute, taux);
            double visP = state.march.cells[0].acsr.injl.FluidoPro.ViscOleo(pchute, taux);
            double visG = state.march.cells[0].acsr.injl.FluidoPro.ViscGas(pchute, taux);
            double visMis = (1 - complementaryFraction) * visP + complementaryFraction * visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(pchute, taux);
            double liquidDensityAtGuess = state.march.cells[0].acsr.injl.FluidoPro.MasEspLiq(pchute, taux);
            double gasDensityAtGuess = state.march.cells[0].acsr.injl.FluidoPro.MasEspGas(pchute, taux);
            rmis = (1 - complementaryFraction) * liquidDensityAtGuess + complementaryFraction * complementaryDensityAtGuess;
            state.march.cells[0].acsr.injl.FluidoPro.MasEspLiq(1., 15.);
            double rlcA = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(1.001, 15.);
            // rough estimate of the complementary-liquid mass flow rate
            double massicC = rlcA * state.march.cells[0].acsr.injl.QLiq * state.march.cells[0].acsr.injl.bet / units::kSecondsPerDay;
            // rough estimate of the produced liquid and gas mass flow rates
            double massic;
            // densities at standard conditions
            double Rhogs = state.march.cells[0].acsr.injl.FluidoPro.Deng * kAirDensityAtStandardConditions; // cel[ind].acsr.injl.FluidoPro.MasEspGas(1, 15);
            double Rhols = (1000 * 141.5 / (131.5 + state.march.cells[0].acsr.injl.FluidoPro.API)) * (1 - state.march.cells[0].acsr.injl.FluidoPro.BSW) + 1000. * state.march.cells[0].acsr.injl.FluidoPro.Denag * state.march.cells[0].acsr.injl.FluidoPro.BSW;
            // multiplier of the standard flow rate giving the produced gas+liquid mass flow rate
            double multiplicador = (Rhols + state.march.cells[0].acsr.injl.FluidoPro.RGO * Rhogs * (1 - state.march.cells[0].acsr.injl.FluidoPro.BSW));
            massic = 1 * multiplicador * state.march.cells[0].acsr.injl.QLiq * (1. - state.march.cells[0].acsr.injl.bet) / units::kSecondsPerDay;
            // gas quality relative to the oil+water+gas mixture
            double fracmasshidra = state.march.cells[0].acsr.injl.FluidoPro.FracMassHidra(pchute, taux);
            double massicP = (1. - fracmasshidra) * massic; // produced liquid mass flow rate
            double massicG = fracmasshidra * massic;        // gas mass flow rate
            /// rough estimate of the mixture velocity
            j = (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess) / state.march.cells[0].duto.area;
            // no-slip void fraction estimate
            double alfmis = (massicG / gasDensityAtGuess) / (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess);
            // mixture properties
            rmis = (1 - alfmis) * rmis + alfmis * gasDensityAtGuess;
            visMis = (1 - alfmis) * visMis + alfmis * visG;
            // Reynolds number estimate
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            // friction factor estimate
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);
        }
        double tmparea = state.march.cells[0].duto.area;
        double tmpperi = state.march.cells[0].duto.peri;
        // estimate of the friction loss in the system; if the accessory at the start of the pipe
        // is not a liquid source, this estimate is zero
        perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * tmpperi / tmparea;
        // if the accessory is an IPR, the estimate is a pressure close to a low flow rate
        // at the bottom of the well
        // if it is not an IPR, the estimate goes on with the pressure loss and the hydrostatics,
        // evaluated in this loop
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            if (taux < state.march.input.tmin)
                taux = state.march.input.tmin;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchute, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchute, taux);
            // if the system is not a main gas-lift ring
            // the pressure estimate uses only the liquid hydrostatics, which gives a
            // very high initial pressure guess
            double alfa = 0.;
            if (complementaryFractionGuess < 0.5) {
                double quality = state.march.cells[i].flui.FracMassHidra(pchute, taux);
                alfa = quality * rhol / (rhog - quality * rhog + quality * rhol);
            }
            if (state.march.annulusDrift == 0)
                alfa = 1.; // if it is the ring, there is only gas
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            // pressure advance by the hydrostatics and the estimated friction loss
            pchute += ((rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2);
            // pressure increment from a constant pressure gain in some cell
            if (state.march.cells[i - 1].acsr.tipo == 7)
                pchute += state.march.cells[i - 1].acsr.delp;
            // if there is an IPR along the pipe, checks whether the estimated pressure is above
            // the static pressure: this solver does not work with negative flow rates;
            // to avoid them, the pressure is corrected to a value close to the
            // static pressure of the cell's IPR
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchute) > -0.01 * state.march.cells[i - 1].acsr.ipr.Pres) && i == 1) {
                pchute = (10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute > 1.1 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 1.1 * state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute < 1.01 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            // a guess above the maximum limit of a table is also avoided
            // when PVTSim is used
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
        }

        if (pchute > 1000)
            pchute = 1000; // maximum pressure guess
    } else
        pchute = chute; // when chute is not negative, the estimate passed in is used
}

/// Raises the guess until the march stops failing on too large an increment.
///
/// Inner loop of bracketFromLowGuess: the outer loop moves the guess, this one
/// backs off when the move overshot and the march returned a sentinel.
bool raisePressureUntilMarchCompletes(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double &positiveResidualGuess, int &kontaiter, double &marchResidual, double pchuteAux, double &pchute2, double chute, int kontaTenta, double &abortValue) {
    // reach the end without the pressure rising above the static pressure
    // of an IPR along the march, or above the maximum pressure
    // of the PVTSim table, when there is one;
    // the low pressure estimate must be lowered
    pchute2 = 0.5 * (pchute2 + pchuteAux); // intermediate value between the highest pressure
    // that gives marchResidual>0 and the pressure that is too high
    marchResidual = marchProductionSteady(state.march, dependencies, pchute2);
    if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
        if (kontaTenta < 0)
            logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                       "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                       "", "");
        cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
        if ((*state.march.globals).networkIteration > 0)
            {
                abortValue = -kSteadyConvergenceFailure;
                return true;
            }
        else
            {
                abortValue = kSteadyConvergenceFailure;
                return true;
            }
    }
    if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
        positiveResidualGuess = pchute2;
    kontaiter++;
    if (kontaiter > 50 * 0.1 / state.march.input.buscaFC) {
        if ((*state.march.globals).networkMode == 0) {
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            } else {
                if (kontaTenta < 0)
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;

                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
        } else {
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
    }
    return false;
}

/// Brackets the root upward when the first march came back positive.
///
/// Applies the upward-search pressure limits and convergence criteria.
bool bracketFromLowGuess(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double &amplifica, double &reduz, int &reversao, double &val0, double &positiveResidualGuess, double &negativeResidualGuess, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute2, double pchute, double chute, int kontaTenta, double &abortValue) {
    // computed by the
    // march: the pressure guess is low, so now look for
    // a high pressure guess that makes marchResidual negative, to start the
    // root finding
    positiveResidualGuess = pchute; // storing the pressure guess that gives the positive value
    // at each new search in which marchResidual gets closer to zero but is still positive
    // positiveResidualGuess is updated with the last pressure guess
    int kontaReverso = 0;
    while (marchResidual > 0) {
        pchuteAux = pchute2;
        pchute2 *= amplifica; // Raising the pressure in search of marchResidual>0
        int limpres = 0;
        if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
            (state.march.cells[0].acsr.ipr.Pres - pchute2) < 0.001 * state.march.cells[0].acsr.ipr.Pres) {
            if (chute < 0) {
                pchute2 = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute2 < 0.999 * state.march.cells[0].acsr.ipr.Pres)
                    pchute2 = 0.999 * state.march.cells[0].acsr.ipr.Pres;
                if (fabs(pchute2 - pchute) < 1e-15) {
                    pchute2 = -(1 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                    if (pchute2 < 0.9999 * state.march.cells[0].acsr.ipr.Pres)
                        pchute2 = 0.9999 * state.march.cells[0].acsr.ipr.Pres;
                }
            } else {
                pchute2 = pchuteAux * 1.0001;
            }

            limpres = 1;
        }
        marchResidual = marchProductionSteady(state.march, dependencies, pchute2);
        if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
            if (kontaTenta < 0)
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        if (marchResidual > 0. && limpres == 1) {
            int iterpres = 0;
            while (marchResidual > 0 && iterpres < 10) {
                pchute2 *= 1.001;
                marchResidual = marchProductionSteady(state.march, dependencies, pchute2);
                if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
                    if (kontaTenta < 0)
                        logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                   "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                   "", "");
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
                iterpres++;
            }
            if (iterpres >= 10) {
                if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                } else {
                    if (kontaTenta < 0)
                        logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                   "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                   "", "");
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            }
        }
        if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold) {
            positiveResidualGuess = pchute2; // updating positiveResidualGuess
            if (marchResidual > val0 && state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance && state.march.input.lingas == 0) {

                kontaReverso++;
                if (kontaReverso == 2) {
                    reversao = 1;
                    marchResidual = -1;
                } else {
                    reduz = 1. + state.march.input.buscaFC;
                    amplifica = 1. - state.march.input.buscaFC;
                    val0 = marchResidual;
                }

            } else
                val0 = marchResidual;
        }
        kontaiter++;
        if (kontaiter > 50 * 0.1 / state.march.input.buscaFC) { // iteration limit: failed to find the second guess
            // end of the simulation or a failure warning
            if ((*state.march.globals).networkMode == 0) {
                if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                } else {
                    if (kontaTenta < 0)
                        logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                   "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                   "", "");
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            } else {
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
        }
        while (marchResidual > kSteadyMarchSentinelThreshold) { // the pressure increment was too large and the march could not
            if (raisePressureUntilMarchCompletes(state, dependencies, positiveResidualGuess, kontaiter, marchResidual, pchuteAux, pchute2, chute, kontaTenta, abortValue))
                return true;
        }
    }
    negativeResidualGuess = pchute2;
    return false;
}

/// Brackets the root downward when the first march came back negative.
///
/// One arm of the sign split that ends searchProductionBottomHolePressure.
bool bracketFromHighGuess(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double &amplifica, double &reduz, int &reversao, double &val0, double &guessLowerBound, double &negativeResidualGuess, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute2, double pchute, double chute, int kontaTenta, double &abortValue) {
    // march: the pressure guess is high, so now look for
    // a low pressure guess that makes marchResidual positive, to start the
    // root finding
    negativeResidualGuess = pchute; // storing the pressure guess that gives the negative value
    // at each new search in which marchResidual gets closer to zero but is still negative
    // negativeResidualGuess is updated with the last pressure guess
    int kontaReverso = 0;
    while (marchResidual < 0) {
        if (fabs(pchute2 - pchute) / pchute < (1. - reduz) / 10. && kontaiter > 50 * 0.1 / state.march.input.buscaFC) {
            pchute2 *= 0.5;
        }
        pchuteAux = pchute2;
        pchute2 *= reduz; // Lowering the pressure in search of marchResidual>0
        if (pchute2 <= guessLowerBound)
            pchute2 = 0.5 * (pchuteAux + guessLowerBound); // guessLowerBound is initially
        // zero, but pchute2 may be lowered so far that marchResidual=-1e10
        // (pressure below 0.5 somewhere in the march); then guessLowerBound becomes this value of
        // pchute2, as it is then known that one cannot go below guessLowerBound
        marchResidual = marchProductionSteady(state.march, dependencies, pchute2);
        if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
            if (kontaTenta < 0)
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold) {
            negativeResidualGuess = pchute2; // updating negativeResidualGuess
            if (marchResidual < val0 && state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance && state.march.input.lingas == 0) {

                kontaReverso++;
                if (kontaReverso == 2) {
                    reversao = 1;
                    marchResidual = 1;
                } else {
                    reduz = 1. + state.march.input.buscaFC;
                    amplifica = 1. - state.march.input.buscaFC;
                    val0 = marchResidual;
                }
            } else
                val0 = marchResidual;
        }
        kontaiter++;
        if (kontaiter > 50 * 0.1 / state.march.input.buscaFC) { // iteration limit: failed to find the second guess
            // end of the simulation or a failure warning
            if ((*state.march.globals).networkMode == 0) {
                if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                } else {
                    if (kontaTenta < 0)
                        logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                   "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                   "", "");
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            } else {
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
        }
        while (marchResidual < -kSteadyMarchSentinelThreshold) { // the pressure reduction was too large and the march could not
            // reach the end without the pressure falling below 0.5 kgf/cm2;
            // the low pressure estimate must be raised
            guessLowerBound = pchute2;
            pchute2 = 0.5 * (pchute2 + pchuteAux); // intermediate value between the lowest pressure
            // that gives marchResidual<0 and the pressure that is too low
            marchResidual = marchProductionSteady(state.march, dependencies, pchute2);
            if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
                if (kontaTenta < 0)
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold) {
                negativeResidualGuess = pchute2;
            }
            kontaiter++;
            if (kontaiter > 50 * 0.1 / state.march.input.buscaFC) {
                if ((*state.march.globals).networkMode == 0) {
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                        NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                        logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                   "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                   "", "");
                    } else {
                        if (kontaTenta < 0)
                            logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                       "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                       "", "");
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                        if ((*state.march.globals).networkIteration > 0)
                            {
                                abortValue = -kSteadyConvergenceFailure;
                                return true;
                            }
                        else
                            {
                                abortValue = kSteadyConvergenceFailure;
                                return true;
                            }
                    }
                } else {
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            }
        }
    }
    return false;
}

/// Walks the guess until the march stops returning a sentinel.
///
/// The march reports failure by returning 1e10 or -1e10 rather than by any other
/// means, so the search has to read the magnitude to know what happened.
bool retryUntilMarchCompletes(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double pchuteAux0, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute, double chute, int kontaTenta, double &abortValue) {
    if ((marchResidual < -kSteadyMarchSentinelThreshold || marchResidual > kSteadyMarchSentinelThreshold) && kontaiter < 50) {
        double valtemp;
        valtemp = marchProductionSteady(state.march, dependencies, pchuteAux); // march with pchuteAux
        if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if (kontaTenta < 0)
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        while (valtemp > kSteadyMarchSentinelThreshold && marchResidual > kSteadyMarchSentinelThreshold) { // bottom-hole pressure estimate still high
            pchuteAux *= 0.99;                     // lowering the estimate
            valtemp = marchProductionSteady(state.march, dependencies, pchuteAux);
            if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if (kontaTenta < 0)
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            kontaiter++; // at most 50 iterations
        }
        while (valtemp < -kSteadyMarchSentinelThreshold && marchResidual < -kSteadyMarchSentinelThreshold && kontaiter <= 50) { // bottom-hole pressure estimate still low
            if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection || state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection || state.march.cells[0].acsr.tipo == kAccessoryMultipleSource)
                pchuteAux *= 1.1;
            else
                pchuteAux *= 1.01; // raising the estimate
            // checking whether this raise exceeds the pressure limit of an IPR at the
            // bottom
            int limpres = 0;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                (state.march.cells[0].acsr.ipr.Pres - pchuteAux) < 0.01 * state.march.cells[0].acsr.ipr.Pres) {

                pchuteAux = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchuteAux < 0.99 * state.march.cells[0].acsr.ipr.Pres)
                    pchuteAux = 0.99 * state.march.cells[0].acsr.ipr.Pres;

                limpres = 1; // flag: this pressure step is too large;
                // further steps must be smaller
            }
            // checking whether this raised estimate is above the maximum pressure
            // of a PVTSim table, if there is one
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
            valtemp = marchProductionSteady(state.march, dependencies, pchuteAux); // new attempt
            if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if (kontaTenta < 0)
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            if (valtemp < -kSteadyMarchSentinelThreshold && limpres == 1) { // still high, and the flag says
                // a smaller pressure step must be used
                int iterpres = 0;
                while (valtemp < -kSteadyMarchSentinelThreshold && iterpres < 10) { // new loop with a smaller step,
                    // at most 10 iterations
                    pchuteAux *= 1.001;
                    valtemp = marchProductionSteady(state.march, dependencies, pchuteAux);
                    if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                        if (kontaTenta < 0)
                            logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                       "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                       "", "");
                        if ((*state.march.globals).networkIteration > 0)
                            {
                                abortValue = -kSteadyConvergenceFailure;
                                return true;
                            }
                        else
                            {
                                abortValue = kSteadyConvergenceFailure;
                                return true;
                            }
                    }
                    iterpres++;
                }
                if (iterpres >= 10) {
                    // case where the maximum number of pressure steps was reached with a
                    // small step: returns a warning that something went wrong, or ends the simulation
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                        // in this case the simulation ends: there is no transient
                        // to run next and this is not a network
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                        logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                   "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                   "", "");
                    } else {
                        // only shows a warning
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                        if (kontaTenta < 0)
                            logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                                       "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                                       "", "");
                        // if in a network iteration, after the first iteration
                        if ((*state.march.globals).networkIteration > 0)
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
            }
            if (kontaiter == 50) {
                pchuteAux = 0.5 * pchuteAux0;
            }
            kontaiter++;
        }
        if (valtemp > -kSteadyMarchSentinelThreshold && valtemp < kSteadyMarchSentinelThreshold) { // the march reached the end
            marchResidual = valtemp;
            pchute = pchuteAux;
        }
    }
    return false;
}

/// Moves the guess according to which sentinel the march returned.
///
/// -1e10 means the pressure fell near zero before the last cell, 1e10 means it
/// rose above a static pressure or above the table's maximum.
void classifyMarchSentinel(const SteadyStateSearchState &state, double marchResidual, double &pchuteAux, double complementaryFractionGuess, double perdafric, double pchute) {
    double taux;
    if (marchResidual < -kSteadyMarchSentinelThreshold) { // pressure too low in the march: the guess must be raised
        // a new estimate is made, now assuming a water hydrostatic head,
        // which gives a higher bottom-hole pressure
        pchuteAux = state.march.gasSurfacePressure;
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            double rhol = 1000 + 0 * state.march.cells[i].flui.MasEspLiq(pchuteAux, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchuteAux, taux);
            double alfa = 0.;
            if (complementaryFractionGuess < 0.5) {
                double quality = state.march.cells[i].flui.FracMassHidra(pchute, taux);
                alfa = quality * rhol / (rhog - quality * rhog + quality * rhol);
                alfa *= 0.5;
            }
            if (state.march.annulusDrift == 0)
                alfa = 1.;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            pchuteAux += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            if (state.march.cells[i - 1].acsr.tipo == 7)
                pchuteAux -= state.march.cells[i - 1].acsr.delp;
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchuteAux) < 0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                pchuteAux = -(10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchuteAux < 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchuteAux = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
        }
        if (pchuteAux > 1000) {
            pchuteAux = 1000; // pchuteAux limit
        }
    } else if (marchResidual > kSteadyMarchSentinelThreshold) { // the pressure somewhere went above a static pressure
        // the guess must be lowered
        pchuteAux = state.march.gasSurfacePressure;
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchuteAux, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchuteAux, taux);
            // in this case a high void fraction is used for the hydrostatics
            double alfa = 0.8;
            if (complementaryFractionGuess < 0.5) {
                double quality = state.march.cells[i].flui.FracMassHidra(pchute, taux);
                alfa = quality * rhol / (rhog - quality * rhog + quality * rhol);
                if (alfa > 0.9999)
                    alfa = 0.9999;
            }
            if (state.march.annulusDrift == 0)
                alfa = 1.;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.2 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            pchuteAux += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            if (state.march.cells[i - 1].acsr.tipo == 7)
                pchuteAux -= state.march.cells[i - 1].acsr.delp;
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchuteAux) < 0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                pchuteAux = -(10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchuteAux < 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchuteAux = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
        }
        if (pchuteAux > 1000) {
            pchuteAux = 1000;
        }
    }
}

/// Estimates the bottom-hole pressure the search starts from.
///
/// With a negative guess the pressure is built from the surface pressure, the head
/// accessory and a friction estimate; otherwise the caller's guess stands.
void estimateInitialBottomHolePressure(const SteadyStateSearchState &state, double &complementaryFractionGuess, double &perdafric, double &pchute, double chute) {
    double j = 0.;
    double rmis = 0.;
    double frictionFactor = 0.;
    double taux;

    if (chute < 0) {

        if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection && fabs(state.march.cells[0].acsr.injl.QLiq) > 0.) {
            complementaryFractionGuess = state.march.cells[0].acsr.injl.bet;
            // this block estimates the mean pressure loss
            // from the flow rate at the start of the pipe; this is done only
            // if there is a liquid source, celula[0].acsr.tipo == 2
            taux = state.march.input.celp[0].textern; // the temperature of this estimate
            // is the source's; the pressure for the properties
            // is pGSup
            if (taux < state.march.input.tmin)
                taux = state.march.input.tmin;
            /////////// physical properties:
            double complementaryFraction = state.march.cells[0].acsr.injl.bet;
            double visC = state.march.cells[0].acsr.injl.fluidocol.VisFlu(pchute, taux);
            double visP = state.march.cells[0].acsr.injl.FluidoPro.ViscOleo(pchute, taux);
            double visG = state.march.cells[0].acsr.injl.FluidoPro.ViscGas(pchute, taux);
            double visMis = (1 - complementaryFraction) * visP + complementaryFraction * visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(pchute, taux);
            double liquidDensityAtGuess = state.march.cells[0].acsr.injl.FluidoPro.MasEspLiq(pchute, taux);
            double gasDensityAtGuess = state.march.cells[0].acsr.injl.FluidoPro.MasEspGas(pchute, taux);
            rmis = (1 - complementaryFraction) * liquidDensityAtGuess + complementaryFraction * complementaryDensityAtGuess;
            double rlcA = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(1.001, 15.);
            // rough estimate of the complementary-liquid mass flow rate
            double massicC = rlcA * state.march.cells[0].acsr.injl.QLiq * state.march.cells[0].acsr.injl.bet / units::kSecondsPerDay;
            // rough estimate of the produced liquid and gas mass flow rates
            double massic;
            // densities at standard conditions
            double Rhogs = state.march.cells[0].acsr.injl.FluidoPro.Deng * kAirDensityAtStandardConditions; // cel[ind].acsr.injl.FluidoPro.MasEspGas(1, 15);
            double Rhols = (1000 * 141.5 / (131.5 + state.march.cells[0].acsr.injl.FluidoPro.API)) * (1 - state.march.cells[0].acsr.injl.FluidoPro.BSW) + 1000. * state.march.cells[0].acsr.injl.FluidoPro.Denag * state.march.cells[0].acsr.injl.FluidoPro.BSW;
            // multiplier of the standard flow rate giving the produced gas+liquid mass flow rate
            double multiplicador = (Rhols + state.march.cells[0].acsr.injl.FluidoPro.RGO * Rhogs * (1 - state.march.cells[0].acsr.injl.FluidoPro.BSW));
            massic = 1 * multiplicador * state.march.cells[0].acsr.injl.QLiq * (1. - state.march.cells[0].acsr.injl.bet) / units::kSecondsPerDay;
            // gas quality relative to the oil+water+gas mixture
            double fracmasshidra = state.march.cells[0].acsr.injl.FluidoPro.FracMassHidra(pchute, taux);
            double massicP = (1. - fracmasshidra) * massic; // produced liquid mass flow rate
            double massicG = fracmasshidra * massic;        // gas mass flow rate
            /// rough estimate of the mixture velocity
            j = (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess) / state.march.cells[0].duto.area;
            // no-slip void fraction estimate
            double alfmis = (massicG / gasDensityAtGuess) / (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess);
            // mixture properties
            rmis = (1 - alfmis) * rmis + alfmis * gasDensityAtGuess;
            visMis = (1 - alfmis) * visMis + alfmis * visG;
            // Reynolds number estimate
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            // friction factor estimate
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);
        }
        double tmparea = state.march.cells[0].duto.area;
        double tmpperi = state.march.cells[0].duto.peri;
        // estimate of the friction loss in the system; if the accessory at the start of the pipe
        // is not a liquid source, this estimate is zero
        perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * tmpperi / tmparea;
        // if the accessory is an IPR, the estimate is a pressure close to a low flow rate
        // at the bottom of the well
        // if it is not an IPR, the estimate goes on with the pressure loss and the hydrostatics,
        // evaluated in this loop
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            if (taux < state.march.input.tmin)
                taux = state.march.input.tmin;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchute, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchute, taux);
            // if the system is not a main gas-lift ring
            // the pressure estimate uses only the liquid hydrostatics, which gives a
            // very high initial pressure guess
            double alfa = 0.;
            if (complementaryFractionGuess < 0.5) {
                double quality = state.march.cells[i].flui.FracMassHidra(pchute, taux);
                alfa = quality * rhol / (rhog - quality * rhog + quality * rhol);
            }
            if (state.march.annulusDrift == 0)
                alfa = 1.; // if it is the ring, there is only gas
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            // pressure advance by the hydrostatics and the estimated friction loss
            pchute += ((rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2);
            if (pchute < 0.8)
                pchute = 0.8;
            // pressure increment from a constant pressure gain in some cell
            if (state.march.cells[i - 1].acsr.tipo == 7)
                pchute -= state.march.cells[i - 1].acsr.delp;
            // if there is an IPR along the pipe, checks whether the estimated pressure is above
            // the static pressure: this solver does not work with negative flow rates;
            // to avoid them, the pressure is corrected to a value close to the
            // static pressure of the cell's IPR
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchute) < 0.01 * state.march.cells[i - 1].acsr.ipr.Pres) && i == 1) {
                double flowRateGuess = 0.15 * state.march.cells[i - 1].duto.area * units::kSecondsPerDay;
                pchute = -(flowRateGuess / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute > 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute < 0.9 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 0.9 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            // a guess above the maximum limit of a table is also avoided
            // when PVTSim is used
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
        }

        if (pchute > 1000) {
            pchute = 1000; // maximum pressure guess
        }
    } else
        pchute = chute; // when chute is not negative, the estimate passed in is used
}

/// Brackets the root when the choke passes less than the column delivers.
bool bracketFromLowGuessSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double amplifica, double &positiveResidualGuess, double &negativeResidualGuess, int &kontaiter, double mult2, double &marchResidual, double &pchuteAux, double &pchute2, double pchute, double chute, double &abortValue) {
    // this means the pressure guess is low, so now look for
    // a high pressure guess that makes marchResidual negative, to start the
    // root finding
    positiveResidualGuess = pchute; // storing the pressure guess that gives the positive value
    // at each new search in which marchResidual gets closer to zero but is still positive
    // positiveResidualGuess is updated with the last pressure guess
    while (marchResidual > 0) {
        pchuteAux = pchute2;
        if ((*state.march.globals).networkMode == 0)
            pchute2 *= amplifica; // Raising the pressure in search of marchResidual>0
        else
            pchute2 *= mult2; // Raising the pressure in search of marchResidual>0
        if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute2) < (*state.march.globals).smallValueTolerance)
            pchute2 = 0.9 * state.march.input.tabent.pmax;
        int limpres = 0;
        if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
            (state.march.cells[0].acsr.ipr.Pres - pchute2) < 0.001 * state.march.cells[0].acsr.ipr.Pres) {
            if (chute < 0) {
                pchute2 = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute2 < 0.999 * state.march.cells[0].acsr.ipr.Pres)
                    pchute2 = 0.999 * state.march.cells[0].acsr.ipr.Pres;
                if (fabs(pchute2 - pchute) < 1e-15) {
                    pchute2 = -(1 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                    if (pchute2 < 0.999 * state.march.cells[0].acsr.ipr.Pres)
                        pchute2 = 0.999 * state.march.cells[0].acsr.ipr.Pres;
                }
            } else {
                pchute2 = pchuteAux * 1.0001;
            }
            limpres = 1;
        }
        marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute2);
        if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        if (marchResidual > 0. && limpres == 1) {
            int iterpres = 0;
            while (marchResidual > 0 && iterpres < 10) {
                pchute2 *= 1.001;
                marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute2);
                if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
                iterpres++;
            }
            if (iterpres >= 10) {
                if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm2 atingiu maximo de iteracoes");
                else {
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            }
        }
        if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
            positiveResidualGuess = pchute2; // updating positiveResidualGuess
        kontaiter++;
        if (kontaiter > 50) { // iteration limit: failed to find the second guess
            // end of the simulation or a failure warning
            if ((*state.march.globals).networkMode == 0) {
                if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm2 atingiu maximo de iteracoes");
                else {
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            } else {
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
        }
        while (marchResidual > kSteadyMarchSentinelThreshold) { // the pressure increment was too large and the march could not
            // reach the end without the pressure rising above the static pressure
            // of an IPR along the march, or above the maximum pressure
            // of the PVTSim table, when there is one;
            // the low pressure estimate must be lowered
            pchute2 = 0.5 * (pchute2 + pchuteAux); // intermediate value between the highest pressure
            // that gives marchResidual>0 and the pressure that is too high
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute2) < (*state.march.globals).smallValueTolerance)
                pchute2 = 0.9 * state.march.input.tabent.pmax;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                (state.march.cells[0].acsr.ipr.Pres - pchute2) < 0.001 * state.march.cells[0].acsr.ipr.Pres) {
                pchute2 = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute2 < 0.999 * state.march.cells[0].acsr.ipr.Pres)
                    pchute2 = 0.999 * state.march.cells[0].acsr.ipr.Pres;
            }
            marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute2);
            if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
                positiveResidualGuess = pchute2;
            kontaiter++;

            if (kontaiter > 50) {
                if ((*state.march.globals).networkMode == 0) {
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm2 atingiu maximo de iteracoes");
                    else {
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                        if ((*state.march.globals).networkIteration > 0)
                            {
                                abortValue = -kSteadyConvergenceFailure;
                                return true;
                            }
                        else
                            {
                                abortValue = kSteadyConvergenceFailure;
                                return true;
                            }
                    }
                } else {
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            }
        }
    }
    negativeResidualGuess = pchute2;
    return false;
}

/// Brackets the root when the pressure guess was too high.
bool bracketFromHighGuessSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double reduz, double &negativeResidualGuess, double &guessLowerBound, int &kontaiter, double mult1, double &marchResidual, double &pchuteAux, double &pchute2, double pchute, double chute, double &abortValue) {
    // choke flow rate > mixture flow rate in the pipe
    // computed by the march: the pressure guess is high, so now look for
    // a low pressure guess that makes marchResidual positive, to start the
    // root finding
    negativeResidualGuess = pchute; // storing the pressure guess that gives the negative value
    // at each new search in which marchResidual gets closer to zero but is still negative
    // negativeResidualGuess is updated with the last pressure guess
    while (marchResidual < 0) {
        if (fabs(pchute2 - pchute) / pchute < (1. - reduz) / 10. && kontaiter > 50) {
            pchute2 *= 0.5;
        }
        pchuteAux = pchute2;
        if ((*state.march.globals).networkMode == 0)
            pchute2 *= reduz; // Lowering the pressure in search of marchResidual>0
        else
            pchute2 *= mult1; // Lowering the pressure in search of marchResidual>0
        if (pchute2 <= guessLowerBound)
            pchute2 = 0.5 * (pchuteAux + guessLowerBound); // guessLowerBound is initially
        // zero, but pchute2 may be lowered so far that marchResidual=-1e10
        // (pressure below 0.5 somewhere in the march); then guessLowerBound becomes this value of
        // pchute2, as it is then known that one cannot go below guessLowerBound
        if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute2) < (*state.march.globals).smallValueTolerance)
            pchute2 = 0.9 * state.march.input.tabent.pmax;
        if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
            (state.march.cells[0].acsr.ipr.Pres - pchute2) < 0.001 * state.march.cells[0].acsr.ipr.Pres) {
            pchute2 = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
            if (pchute2 < 0.999 * state.march.cells[0].acsr.ipr.Pres)
                pchute2 = 0.999 * state.march.cells[0].acsr.ipr.Pres;
        }
        marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute2);
        if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
            negativeResidualGuess = pchute2; // updating negativeResidualGuess
        kontaiter++;
        if (kontaiter > 50) { // iteration limit: failed to find the second guess
            // end of the simulation or a failure warning
            if ((*state.march.globals).networkMode == 0) {
                if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm2 atingiu maximo de iteracoees");
                else {
                    cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            } else {
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
        }
        while (marchResidual < -kSteadyMarchSentinelThreshold) { // the pressure reduction was too large and the march could not
            // reach the end without the pressure falling below 0.5 kgf/cm2;
            // the low pressure estimate must be raised
            guessLowerBound = pchute2;
            pchute2 = 0.5 * (pchute2 + pchuteAux); // intermediate value between the lowest pressure
            // that gives marchResidual<0 and the pressure that is too low
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute2) < (*state.march.globals).smallValueTolerance)
                pchute2 = 0.9 * state.march.input.tabent.pmax;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                (state.march.cells[0].acsr.ipr.Pres - pchute2) < 0.001 * state.march.cells[0].acsr.ipr.Pres) {
                pchute2 = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute2 < 0.999 * state.march.cells[0].acsr.ipr.Pres)
                    pchute2 = 0.999 * state.march.cells[0].acsr.ipr.Pres;
            }
            marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute2);
            if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                negativeResidualGuess = pchute2;
            kontaiter++;
            if (kontaiter > 50) {
                if ((*state.march.globals).networkMode == 0) {
                    if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm2 atingiu maximo de iteracoes");
                    else {
                        cout << "#################PERMANENTE FALHOU EM SUA CONVERGENCIA##############################" << endl;
                        if ((*state.march.globals).networkIteration > 0)
                            {
                                abortValue = -kSteadyConvergenceFailure;
                                return true;
                            }
                        else
                            {
                                abortValue = kSteadyConvergenceFailure;
                                return true;
                            }
                    }
                } else {
                    if ((*state.march.globals).networkIteration > 0)
                        {
                            abortValue = -kSteadyConvergenceFailure;
                            return true;
                        }
                    else
                        {
                            abortValue = kSteadyConvergenceFailure;
                            return true;
                        }
                }
            }
        }
    }
    return false;
}

/// Walks the guess until the march stops returning a sentinel.
bool retryUntilMarchCompletesSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double pchuteAux0, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute, double &abortValue) {
    if (marchResidual < -kSteadyMarchSentinelThreshold || marchResidual > kSteadyMarchSentinelThreshold) {
        double valtemp;
        valtemp = marchProductionSteadySecondary(state.march, dependencies, pchuteAux); // march with pchuteAux
        if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        while (valtemp > kSteadyMarchSentinelThreshold && marchResidual > kSteadyMarchSentinelThreshold && kontaiter < 50) { // bottom-hole pressure estimate still high
            pchuteAux *= 0.99;                                       // lowering the estimate
            valtemp = marchProductionSteadySecondary(state.march, dependencies, pchuteAux);
            if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            kontaiter++; // at most 50 iterations
        }
        while (valtemp < -kSteadyMarchSentinelThreshold && marchResidual < -kSteadyMarchSentinelThreshold && kontaiter <= 50) { // bottom-hole pressure estimate still low
            if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection || state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection || state.march.cells[0].acsr.tipo == kAccessoryMultipleSource)
                pchuteAux *= 1.1;
            else
                pchuteAux *= 1.01; // raising the estimate
            // checking whether this raise exceeds the pressure limit of an IPR at the
            // bottom
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                (state.march.cells[0].acsr.ipr.Pres - pchuteAux) < 0.01 * state.march.cells[0].acsr.ipr.Pres) {
                pchuteAux = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchuteAux < 0.99 * state.march.cells[0].acsr.ipr.Pres)
                    pchuteAux = 0.99 * state.march.cells[0].acsr.ipr.Pres;
            }
            // checking whether this raised estimate is above the maximum pressure
            // of a PVTSim table, if there is one
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
            valtemp = marchProductionSteadySecondary(state.march, dependencies, pchuteAux); // new attempt
            if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    {
                        abortValue = -kSteadyConvergenceFailure;
                        return true;
                    }
                else
                    {
                        abortValue = kSteadyConvergenceFailure;
                        return true;
                    }
            }
            if (kontaiter == 50) {
                pchuteAux = 0.5 * pchuteAux0;
            }
            kontaiter++;
        }
        valtemp = marchProductionSteadySecondary(state.march, dependencies, pchuteAux);
        if (fabs(valtemp) > kSteadyConvergenceFailureThreshold) {
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                {
                    abortValue = -kSteadyConvergenceFailure;
                    return true;
                }
            else
                {
                    abortValue = kSteadyConvergenceFailure;
                    return true;
                }
        }
        if (valtemp > -kSteadyMarchSentinelThreshold && valtemp < kSteadyMarchSentinelThreshold) { // the march reached the end
            marchResidual = valtemp;
            pchute = pchuteAux;
        }
    }
    return false;
}

/// Moves the guess according to which sentinel the march returned.
void classifyMarchSentinelSecondary(const SteadyStateSearchState &state, double marchResidual, double &pchuteAux, double perdafric) {
    double taux;
    if (marchResidual < -kSteadyMarchSentinelThreshold) { // pressure too low in the march: the guess must be raised
        // a new estimate is made, now assuming a water hydrostatic head,
        // which gives a higher bottom-hole pressure
        pchuteAux = state.march.gasSurfacePressure;
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            double rhol = 1000 + 0 * state.march.cells[i].flui.MasEspLiq(pchuteAux, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchuteAux, taux);
            double alfa = 0.;
            if (state.march.annulusDrift == 0)
                alfa = 1.;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            pchuteAux += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchuteAux) < 0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                pchuteAux = -(10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchuteAux < 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchuteAux = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
        }
        if (pchuteAux > 1100)
            pchuteAux = 1100;  // pchuteAux limit
    } else if (marchResidual > kSteadyMarchSentinelThreshold) { // the pressure somewhere went above a static pressure
        // the guess must be lowered
        pchuteAux = state.march.gasSurfacePressure;
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchuteAux, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchuteAux, taux);
            // in this case a high void fraction is used for the hydrostatics
            double alfa = 0.8;
            if (state.march.annulusDrift == 0)
                alfa = 1.;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.2 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            pchuteAux += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchuteAux) < 0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                pchuteAux = -(10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchuteAux < 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchuteAux = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchuteAux) < (*state.march.globals).smallValueTolerance)
                pchuteAux = 0.9 * state.march.input.tabent.pmax;
        }
        if (pchuteAux > 1100)
            pchuteAux = 1100;
    }
}

/// Estimates the bottom-hole pressure the secondary search starts from.
void estimateInitialBottomHolePressureSecondary(const SteadyStateSearchState &state, double &perdafric, double &pchute, double chute) {
    double j = 0.;
    double rmis = 0.;
    double frictionFactor = 0.;
    double taux;
    double complementaryFractionGuess = 0.;

    if (chute < 0) {
        if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection && fabs(state.march.cells[0].acsr.injl.QLiq) > 0.) {
            // this block estimates the mean pressure loss
            // from the flow rate at the start of the pipe; this is done only
            // if there is a liquid source, celula[0].acsr.tipo == 2
            taux = state.march.input.celp[0].textern;
            // the temperature of this estimate
            // is the source's; the pressure for the properties
            // is pGSup
            if (taux < state.march.input.tmin)
                taux = state.march.input.tmin;
            /////////// physical properties:
            double complementaryFraction = state.march.cells[0].acsr.injl.bet;
            complementaryFractionGuess = complementaryFraction;
            double visC = state.march.cells[0].acsr.injl.fluidocol.VisFlu(pchute, taux);
            double visP = state.march.cells[0].acsr.injl.FluidoPro.ViscOleo(pchute, taux);
            double visG = state.march.cells[0].acsr.injl.FluidoPro.ViscGas(pchute, taux);
            double visMis = (1 - complementaryFraction) * visP + complementaryFraction * visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(pchute, taux);
            double liquidDensityAtGuess = state.march.cells[0].acsr.injl.FluidoPro.MasEspLiq(pchute, taux);
            double gasDensityAtGuess = state.march.cells[0].acsr.injl.FluidoPro.MasEspGas(pchute, taux);
            rmis = (1 - complementaryFraction) * liquidDensityAtGuess + complementaryFraction * complementaryDensityAtGuess;
            double rlpA = state.march.cells[0].acsr.injl.FluidoPro.MasEspLiq(1., 15.);
            double rlcA = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(1.001, 15.);
            // rough estimate of the complementary-liquid mass flow rate
            double massicC = rlcA * state.march.cells[0].acsr.injl.QLiq * state.march.cells[0].acsr.injl.bet / units::kSecondsPerDay;
            // rough estimate of the produced liquid and gas mass flow rates
            double massic = rlpA * state.march.cells[0].acsr.injl.QLiq * (1. - state.march.cells[0].acsr.injl.bet) / units::kSecondsPerDay;
            // densities at standard conditions
            double Rhogs = state.march.cells[0].acsr.injl.FluidoPro.Deng * kAirDensityAtStandardConditions; // cel[ind].acsr.injl.FluidoPro.MasEspGas(1, 15);
            double Rhols = (1000 * 141.5 / (131.5 + state.march.cells[0].acsr.injl.FluidoPro.API)) * (1 - state.march.cells[0].acsr.injl.FluidoPro.BSW) + 1000. * state.march.cells[0].acsr.injl.FluidoPro.Denag * state.march.cells[0].acsr.injl.FluidoPro.BSW;
            // multiplier of the standard flow rate giving the produced gas+liquid mass flow rate
            double multiplicador = (Rhols + state.march.cells[0].acsr.injl.FluidoPro.RGO * Rhogs * (1 - state.march.cells[0].acsr.injl.FluidoPro.BSW));
            massic = 1 * multiplicador * state.march.cells[0].acsr.injl.QLiq * (1. - state.march.cells[0].acsr.injl.bet) / units::kSecondsPerDay;
            // gas quality relative to the oil+water+gas mixture
            double fracmasshidra = state.march.cells[0].acsr.injl.FluidoPro.FracMassHidra(pchute, taux);
            double massicP = (1. - fracmasshidra) * massic; // produced liquid mass flow rate
            double massicG = fracmasshidra * massic;        // gas mass flow rate
            /// rough estimate of the mixture velocity
            j = (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess) / state.march.cells[0].duto.area;
            // no-slip void fraction estimate
            double alfmis = (massicG / gasDensityAtGuess) / (massicP / liquidDensityAtGuess + massicC / complementaryDensityAtGuess + massicG / gasDensityAtGuess);
            // mixture properties
            rmis = (1 - alfmis) * rmis + alfmis * gasDensityAtGuess;
            visMis = (1 - alfmis) * visMis + alfmis * visG;
            // Reynolds number estimate
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            // friction factor estimate
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);
        }
        // estimate of the friction loss in the system; if the accessory at the start of the pipe
        // is not a liquid source, this estimate is zero
        perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.march.cells[0].duto.peri / state.march.cells[0].duto.area;
        // if the accessory is an IPR, the estimate is a pressure close to a low flow rate
        // at the bottom of the well
        // if it is not an IPR, the estimate goes on with the pressure loss and the hydrostatics,
        // evaluated in this loop
        for (int i = state.march.lastCell; i > 0; i--) {
            taux = state.march.input.celp[i].textern;
            if (taux < state.march.input.tmin)
                taux = state.march.input.tmin;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchute, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchute, taux);
            // if the system is not a main gas-lift ring
            // the pressure estimate uses only the liquid hydrostatics, which gives a
            // very high initial pressure guess
            double alfa = 0.;
            if (complementaryFractionGuess < 0.5) {
                double quality = state.march.cells[i].flui.FracMassHidra(pchute, taux);
                alfa = quality * rhol / (rhog - quality * rhog + quality * rhol);
            }
            if (state.march.annulusDrift == 0)
                alfa = 1.; // if it is the ring, there is only gas
            else if ((*state.march.globals).networkMode == 0 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess;
            else if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
            // pressure advance by the hydrostatics and the estimated friction loss
            pchute += (rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
            // if there is an IPR along the pipe, checks whether the estimated pressure is above
            // the static pressure: this solver does not work with negative flow rates;
            // to avoid them, the pressure is corrected to a value close to the
            // static pressure of the cell's IPR
            if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                ((state.march.cells[i - 1].acsr.ipr.Pres - pchute) < 0.01 * state.march.cells[i - 1].acsr.ipr.Pres && i == 1)) {
                double flowRateGuess = 0.15 * state.march.cells[i - 1].duto.area * units::kSecondsPerDay;
                pchute = -(flowRateGuess / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute > 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute < 0.9 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 0.9 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            // a guess above the maximum limit of a table is also avoided
            // when PVTSim is used
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
        }

        if (pchute > 1000)
            pchute = 1000.; // maximum pressure guess
    } else
        pchute = chute; // when chute is not negative, the estimate passed in is used
}

/// Walks the column cell by cell for the tertiary search.
///
/// Applies the tertiary search pressure limits and coupled thermal updates.
bool advanceTertiaryCells(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, int &i, double &abortValue) {

                advanceUpstreamSteadyPressure(state.march, i, 0); // march step to get the pressure at the left boundary
                // of cell i
                // checks whether something went wrong:
                if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - state.march.cells[i].presaux) < (*state.march.globals).smallValueTolerance)
                    {
                        abortValue = kSteadyMarchLimit;
                        return true;
                    }
                if (marchPressureTooLow(state.march.cells[i].presaux) ||
                    (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmin - state.march.cells[i].presaux) > (*state.march.globals).smallValueTolerance)) {
                    {
                        abortValue = -kSteadyMarchLimit;
                        return true;
                    }
                }
                refreshUpstreamProductionPeriphery(state.march, i); // update of the left boundary pressure,
                // if there is an ESP or a pressure increment
                if (state.march.input.flashCompleto != 2)
                    advanceSteadyMass(state.march, dependencies, i); // checks whether the previous cell has a source, and so updates
                // the mass flow rates at the left boundary and the fluid properties,
                // gas density, GOR, API, BSW, beta
                else
                    advanceCompositionalSteadyMass(state.march, dependencies, i);
                thermal::advanceSteadyTemperature(dependencies.thermal, i, 0); // advances the temperature from cell i-1 to cell i
                // checks whether the temperature went out of bounds
                // when working with a PVTSim table
                if (isnan(state.march.cells[i].temp))
                    NumError("Temperatrura na linha de producao com valor NaN");
                if (state.march.input.usaTabela == 1 && (state.march.cells[i].temp - state.march.input.tabent.tmin) < (*state.march.globals).smallValueTolerance)
                    state.march.cells[i].temp = state.march.input.tabent.tmin;
                thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i); // just updates the left and right temperature fields
                advanceDownstreamSteadyPressure(state.march, i, 0); // advances the pressure from the left boundary of cell i to
                // its cell centre
                // checks whether something went wrong in this pressure advance to the centre of the
                // cell
                if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - state.march.cells[i].pres) < (*state.march.globals).smallValueTolerance) {
                    {
                        abortValue = kSteadyMarchLimit;
                        return true;
                    }
                }
                if (marchPressureTooLow(state.march.cells[i].pres) ||
                    (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmin - state.march.cells[i].pres) > (*state.march.globals).smallValueTolerance)) {
                    {
                        abortValue = -kSteadyMarchLimit;
                        return true;
                    }
                }
                refreshDownstreamProductionPeriphery(state.march, i); // just updates the fields that hold the pressures
                // of the cells to the left and right
                for (int j = 0; j < state.march.input.nvalvgas; j++) { // re-evaluates the gas-lift valve flow rate, when
                    // the cell has one.

                    if (state.march.productionValveCellIndices[j] == i) {
                        int k = state.march.gasValveCellIndices[j];
                        gaslift::computeSteadyGasFlowRate(dependencies.gasLift, dependencies.thermal, k);
                    }
                }
                if (state.march.input.tipoFluido == 0)
                    advanceSteadyMassTransfer(state.march, i - 1);
                else
                    advanceSteadyGasMassTransfer(state.march, i - 1); // with a PVTSim table, computes the
                // interphase mass transfer rate for the
                // latent heat in the energy equation
                if (state.march.input.ordperm > 1) { // second-order correction
                    double D0presaux = state.march.cells[i].presaux - state.march.cells[i - 1].pres;
                    double D0pres = state.march.cells[i].pres - state.march.cells[i].presaux;
                    double D0temp = state.march.cells[i].temp - state.march.cells[i - 1].temp;
                    advanceUpstreamSteadyPressure(state.march, i, 1);
                    refreshUpstreamProductionPeriphery(state.march, i);
                    advanceSteadyMass(state.march, dependencies, i);
                    thermal::advanceSteadyTemperature(dependencies.thermal, i, 1);
                    advanceDownstreamSteadyPressure(state.march, i, 1);
                    state.march.cells[i].pres = 0.5 * (state.march.cells[i].presaux + D0pres + state.march.cells[i].pres);
                    state.march.cells[i].presaux = 0.5 * (state.march.cells[i - 1].pres + D0presaux + state.march.cells[i].presaux);
                    state.march.cells[i].temp = 0.5 * (state.march.cells[i - 1].temp + D0temp + state.march.cells[i].temp);
                    refreshDownstreamProductionPeriphery(state.march, i);
                    refreshUpstreamProductionPeriphery(state.march, i);
                    thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
                    advanceSteadyMass(state.march, dependencies, i);
                    if (state.march.input.tipoFluido == 0)
                        advanceSteadyMassTransfer(state.march, i - 1);
                    else
                        advanceSteadyGasMassTransfer(state.march, i - 1);
                }

                // once the pressure at the centre of cell i is reached, on the first march iteration
                // checks whether there is a gas-lift valve in i and makes an initial estimate of the gas-lift
                // flow rate (when there is a gas line). Note that this is done only on iteration zero.
                if (state.march.input.lingas > 0 && state.march.input.nvalvgas > 0 && state.march.steadyIteration == 0)
                    gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, i);
                i++;

                // checks whether the cell-centre pressure went above
                // the static pressure of an IPR, if any
                if (marchPressureTooLow(state.march.cells[i - 1].pres) ||
                    (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmin - state.march.cells[i - 1].pres) > (*state.march.globals).smallValueTolerance)) {
                    {
                        abortValue = -kSteadyMarchLimit;
                        return true;
                    }
                } else if ((state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance &&
                            ((state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres) < (*state.march.globals).smallValueTolerance) && i == 1)) {
                    {
                        abortValue = kSteadyMarchLimit;
                        return true;
                    }
                } else if ((state.march.cells[i - 1].acsr.tipo == kAccessoryRadialPorous &&
                            ((state.march.cells[i - 1].acsr.radialPoro.pRes[0] - state.march.cells[i - 1].pres) < (*state.march.globals).smallValueTolerance) && i == 1)) {
                    {
                        abortValue = kSteadyMarchLimit;
                        return true;
                    }
                } else if ((state.march.cells[i - 1].acsr.tipo == kAccessoryPorous2D &&
                            ((state.march.cells[i - 1].acsr.poroso2D.dados.pRes - state.march.cells[i - 1].pres) < (*state.march.globals).smallValueTolerance) && i == 1)) {
                    {
                        abortValue = kSteadyMarchLimit;
                        return true;
                    }
                }
    return false;
}

/// Marches the column to convergence for the tertiary search.
///
/// Not a search: it calls no solver and no march, and inlines a march of its
/// own.
bool marchTertiaryCellsUntilConverged(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, int &guessNeedsCorrection, int &i, double betini, double alfini, double pentrada, double &abortValue) {
    while (guessNeedsCorrection == 1) {
        // Complete the column march or return its failure sentinel.

        // initialising the pressures and volume fractions of the first cells, cell centre
        // and cell boundary
        state.march.cells[0].presauxL = pentrada;
        state.march.cells[0].presLini = pentrada;
        state.march.cells[0].presL = pentrada;
        state.march.cells[0].pres = pentrada;
        state.march.cells[1].presL = pentrada;
        state.march.cells[0].presini = pentrada;
        state.march.cells[1].presLini = pentrada;
        state.march.cells[0].presaux = pentrada;
        state.march.cells[1].presauxL = pentrada;

        state.march.cells[0].alf = alfini;
        state.march.cells[0].alfini = alfini;
        state.march.cells[0].bet = betini;
        state.march.cells[0].betini = betini;
        state.march.cells[1].alfL = state.march.cells[0].alf;
        state.march.cells[1].alfLini = state.march.cells[0].alf;
        state.march.cells[0].alfPigD = state.march.cells[0].alf;
        state.march.cells[0].alfPigDini = state.march.cells[0].alf;
        state.march.cells[0].alfPigE = state.march.cells[0].alf;
        state.march.cells[0].alfPigEini = state.march.cells[0].alf;
        state.march.cells[1].betL = state.march.cells[0].bet;
        state.march.cells[1].betLini = state.march.cells[0].bet;
        state.march.cells[0].betPigD = state.march.cells[0].bet;
        state.march.cells[0].betPigDini = state.march.cells[0].bet;
        state.march.cells[0].betPigE = state.march.cells[0].bet;
        state.march.cells[0].betPigEini = state.march.cells[0].bet;
        state.march.cells[0].betI = state.march.cells[0].bet;
        state.march.cells[1].betLI = state.march.cells[0].bet;

        // checks whether something is already wrong at the start of the march
        if (marchPressureTooLow(state.march.cells[0].pres) ||
            (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmin - state.march.cells[0].pres) > (*state.march.globals).smallValueTolerance))
            {
                abortValue = -kSteadyMarchLimit;
                return true;
            }
        else if ((state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                  (state.march.cells[0].acsr.ipr.Pres - state.march.cells[0].pres) < (*state.march.globals).smallValueTolerance))
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        else if ((state.march.cells[0].acsr.tipo == kAccessoryRadialPorous &&
                  (state.march.cells[0].acsr.radialPoro.pRes[0] - state.march.cells[0].pres) < (*state.march.globals).smallValueTolerance))
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        else if ((state.march.cells[0].acsr.tipo == kAccessoryPorous2D &&
                  (state.march.cells[0].acsr.poroso2D.dados.pRes - state.march.cells[0].pres) < (*state.march.globals).smallValueTolerance))
            {
                abortValue = kSteadyMarchLimit;
                return true;
            }
        // IniciaVazValvGasPerm estimates the flow rate through a gas-lift valve
        // before the gas line has been marched. It receives the
        // production cell index and checks whether that cell has a gas-lift valve; if it has,
        // with an injected-flow-rate condition on the gas line, it divides the injected flow rate by the number of
        // valves and gives that value to the valve of this production cell;
        // with an injection-pressure condition, it estimates the gas-line pressure
        // at the valve by hydrostatics and computes the valve's injection flow rate from it
        if (state.march.input.lingas > 0 && state.march.input.nvalvgas > 0 && state.march.steadyIteration == 0)
            gaslift::initializeSteadyValveGasFlowRate(dependencies.gasLift, 0);
        i = 1;
        // start of the march proper
        while (i <= state.march.lastCell && state.march.cells[i - 1].pres >= kMarchMinimumPressure && fabs(pentrada - state.march.cells[0].pres) < (*state.march.globals).smallValueTolerance) {
            if (advanceTertiaryCells(state, dependencies, i, abortValue))
                return true;
        }
        if (i == state.march.lastCell + 1)
            guessNeedsCorrection = 0; // end of the march
    }
    return false;
}

/// The marches per guess with convergence acceleration on: one more under a
/// pressure condition on the gas line, where it couples less easily with the
/// production line.
int acceleratedMarchesPerGuess(InjectionPressureCondition) {
    return 3;
}

int acceleratedMarchesPerGuess(InjectionFlowRateCondition) {
    return 2;
}

/// The gas line at steady state under its inlet condition, as the tertiary
/// search solves it: marched when the injection choke is not throttling and
/// searched (tertiary) when it is, under a pressure condition; searched
/// (secondary) under a flow-rate condition.
void solveGasLineForSearch(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, InjectionPressureCondition) {
    // march for the injection-pressure case
    if (state.march.input.chokes.abertura[0] >= 0.2) { // injection choke inactive
        marchGasSteady(state.march, dependencies);
    } else
        searchGasPressureSteadyTertiary(state, dependencies); // injection choke active
}

void solveGasLineForSearch(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, InjectionFlowRateCondition) {
    searchGasPressureSteadySecondary(state, dependencies); // gas-line march for the injection-flow-rate case
}

/// Brackets and solves the mass-flow root for the tertiary pressure-to-pressure search.
double bracketTertiaryPressureToPressureRoot(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, int &testaEscoa, double &positiveResidualGuess, double &negativeResidualGuess, double &mchute2, double &mchuteAux, int &kontaiter, double &marchResidual, double mchute, double maximumFlowRate) {
    if (marchResidual < 0.) {
        negativeResidualGuess = mchute;
        while (marchResidual < 0) {
            mchuteAux = mchute2;
            mchute2 *= (1. - state.march.input.buscaFC);
            marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
            if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                negativeResidualGuess = mchute2;
            kontaiter++;
            double velocityGuess = estimateInletMixtureVelocity(state, mchute2);
            if (kontaiter > 200 && velocityGuess > 0.01) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            if (velocityGuess <= 0.01) {
                testaEscoa = 0;
                marchResidual = 1.;
            }
            while (marchResidual < -kSteadyMarchSentinelThreshold) {
                mchute2 = 0.5 * (mchute2 + mchuteAux);
                marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
                if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = mchute2;
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
        }
        positiveResidualGuess = mchute2;
    } else if (marchResidual > 0.) {
        positiveResidualGuess = mchute;
        while (marchResidual > 0) {
            mchuteAux = mchute2;
            mchute2 *= (1. + state.march.input.buscaFC);
            marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
            if ((*state.march.globals).networkMode != 0 && mchute2 > maximumFlowRate && maximumFlowRate > 0) {
                testaEscoa = 0;
                marchResidual = -1.;
            }
            if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
                positiveResidualGuess = mchute2;
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm  atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            while (marchResidual > kSteadyMarchSentinelThreshold) {
                mchute2 = 0.5 * (mchute2 + mchuteAux);
                marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
                if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = mchute2;
                kontaiter++;

                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
        }
        negativeResidualGuess = mchute2;
    }
    if (testaEscoa == 1)
        return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 2, 1);
    else {
        if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection) {
            state.march.cells[0].acsr.injg.QGas = 0.;
        } else if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
            state.march.cells[0].acsr.injl.QLiq = 0.;
        }
        return 0.;
    }
}

/// What the downward walk of a surface-pressure injection search does with a
/// guess below 1 kgf/cm2, the code's atmospheric pressure.
///
/// The second and the fifth injection searches bracket their root the same way
/// except here: the second refuses such a guess -- it steps back to 99 % of the
/// previous one instead of 90 %, and gives up if even that is below
/// atmospheric -- and the fifth passes it to the march.
enum class SubAtmosphericGuess {
    refused,
    marched,
};

/// Brackets and solves the root for the two injection searches whose unknown is
/// the surface pressure: the second (condContorno 0) and the fifth (5).
double bracketInjectionPressureRoot(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double &positiveResidualGuess, double &negativeResidualGuess, int &kontaiter, double &marchResidual, double &pchuteAux, double &pchute2, double pchute, SubAtmosphericGuess subAtmosphericGuess) {
    if (marchResidual < 0.) {
        negativeResidualGuess = pchute;
        while (marchResidual < 0) {
            pchuteAux = pchute2;
            pchute2 *= 0.9;
            if (subAtmosphericGuess == SubAtmosphericGuess::refused && pchute2 < 1.) {
                pchute2 = pchuteAux;
                pchute2 *= 0.99;
                if (pchute2 < 1. && state.march.input.AP == 0)
                    NumError("Pressao de injecao abaixo da pressao atmosferica");
                else if (pchute2 < 1.)
                    return kSteadyConvergenceFailure;
            }
            marchResidual = marchInjectionSteady(state.march, dependencies, pchute2);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm2 atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            while (marchResidual < -kSteadyMarchSentinelThreshold) {
                pchute2 = 0.5 * (pchute2 + pchuteAux);
                marchResidual = marchInjectionSteady(state.march, dependencies, pchute2);
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm2 atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
        }
        positiveResidualGuess = pchute2;
    } else if (marchResidual > 0.) {
        positiveResidualGuess = pchute;
        while (marchResidual > 0) {
            pchuteAux = pchute2;
            pchute2 *= 1.1;
            marchResidual = marchInjectionSteady(state.march, dependencies, pchute2);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm2 atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            while (marchResidual > kSteadyMarchSentinelThreshold) {
                pchute2 = 0.5 * (pchute2 + pchuteAux);
                marchResidual = marchInjectionSteady(state.march, dependencies, pchute2);
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm2 atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
        }
        negativeResidualGuess = pchute2;
    }
    return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 1, 0);
}

}  // namespace

double dispatchMarch(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, int prod, int tipoCC) {
    switch (selectSteadyMarch(state.march.input.pocinjec, prod, tipoCC, state.reverseSteady,
                              state.march.input.chokep.abertura)) {
    case SteadyMarch::marchaGasPerm2:         return marchGasSteadySecondary(state.march, dependencies, chute);
    case SteadyMarch::marchaGasPerm3:         return marchGasSteadyTertiary(state.march, dependencies, chute);
    case SteadyMarch::marchaProdPerm1:        return marchProductionSteady(state.march, dependencies, chute);
    case SteadyMarch::marchaProdPerm1Rev:     return marchReverseProductionSteady(state.march, dependencies, chute);
    case SteadyMarch::marchaProdPerm2:        return marchProductionSteadySecondary(state.march, dependencies, chute);
    case SteadyMarch::marchaProdPresPres1:    return marchProductionPressureToPressure(state.march, dependencies, chute);
    case SteadyMarch::marchaProdPresPres1Rev: return marchReverseProductionPressureToPressure(state.march, dependencies, chute);
    case SteadyMarch::marchaProdPresPres2:    return marchProductionPressureToPressureSecondary(state.march, dependencies, chute);
    // The injection case uses the return after the switch to satisfy GCC 11 return analysis.
    case SteadyMarch::marchaInjPerm1:         break;
    }
    return marchInjectionSteady(state.march, dependencies, chute);
}

double solveSteadyRoot(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double lowerBracket, double upperBracket, int prod, int tipoCC) {
    // Strong coupling sets the minimum root-finding iteration count.
    int minit=0;
    if(state.march.input.acopColAnulPermForte == 1)minit=10;
    return rootfinding::zriddr(
        lowerBracket, upperBracket,
        [&](double guess) { return dispatchMarch(state, dependencies, guess, prod, tipoCC); },
        // Normalize the residual before the root solver evaluates convergence.
        [&](double residual) {
            double normalized = residual / state.march.baseConvergenceMonitor;
            if (prod != 0) {
                state.march.convergenceMonitor = fabs(normalized);
            }
            return normalized;
        },
        state.reverseSteady, minit);
}

double searchGasPressureSteadySecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies) {
    int valveCount = state.march.input.nvalvgas;
    double pchute;
    int maisprof = 0;
    for (int i = 1; i < valveCount; i++)
        if (state.march.productionValveCellIndices[i] < state.march.productionValveCellIndices[maisprof])
            maisprof = i;                      // finds the deepest valve
    pchute = state.march.cells[state.march.productionValveCellIndices[maisprof]].pres; // estimates a pressure for the deepest valve of the line,
    // the pressure in the production tubing at this valve; note that when this method is called,
    // the march along the production line has already been done
    double deepestValveTemperatureGuess;
    if (state.march.steadyIteration == 0) {                              // for the first iteration of the production line-gas line system
        deepestValveTemperatureGuess = state.march.cells[state.march.productionValveCellIndices[maisprof]].temp; // estimate of the temperature at the deepest gas-lift valve
        for (int k = state.march.lastGasCell; k > 0; k--) {           // march from the last cell to the first
            // to estimate the injection pressure
            double dx = 0.5 * (state.march.gasCells[k].dx0 + state.march.gasCells[k].dxL);
            double rhog = state.march.gasCells[k].flui.MasEspGas(pchute, deepestValveTemperatureGuess);
            pchute += rhog * 9.81 * sin(state.march.gasCells[k].duto.teta) * dx / units::kPascalPerKgfPerCm2; // advance by the hydrostatics only
        }
    } else {
        for (int k = state.march.lastGasCell; k > 0; k--)
            pchute += gaslift::steadyGasPressureDrop(dependencies.gasLift, k); // advance using the results of the previous iteration, with the hydrostatics
        // and the friction
    }

    double delmas = marchGasSteadySecondary(state.march, dependencies, pchute); // march with the estimated injection pressure and the defined injection
    // flow rate; returns the difference between the sum of the valve flow rates computed in the march
    // and the defined injection flow rate: negative -> pchute low, positive -> pchute high
    double pchute2 = pchute;
    int kontaiter = 0;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    if (fabs(delmas) < 1e-3)
        return pchute;
    else {
        if (delmas < 0) { // pchute low: new estimate raising pchute, to find another value
            // of delmas that is positive, to start the false chord
            negativeResidualGuess = pchute2;
            while (delmas < 0 && kontaiter < 800) {
                pchute2 *= 1.1;
                delmas = marchGasSteadySecondary(state.march, dependencies, pchute2);
                if (delmas < 0 && delmas > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = pchute2;
                kontaiter++;
            }
            if (kontaiter >= 800) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaGasPresPerm2 atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            positiveResidualGuess = pchute2;
        } else if (delmas > 0) { // pchute high: new estimate lowering pchute, to find another value
            // of delmas that is negative, to start the false chord
            positiveResidualGuess = pchute2;
            while (delmas > 0 && kontaiter < 800) {
                pchute2 *= 0.9;
                delmas = marchGasSteadySecondary(state.march, dependencies, pchute2);
                if (delmas > 0 && delmas < kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = pchute2;
                kontaiter++;
            }
            if (kontaiter >= 800) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaGasPresPerm2 atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            negativeResidualGuess = pchute2;
        }
        return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 0, 0); // root finding
    }
}

double searchGasPressureSteadyTertiary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies) {
    // two pressure guesses upstream of the injection choke; convergence in this case,
    // with an injection choke, is harder, so more guesses are tried:
    double pchute = state.march.injectionChoke.presEstag * 0.8; // upstream choke pressure 20% below the downstream pressure
    double pchute2 = state.march.gasCells[0].pres;         // On the first iteration this value is the upstream pressure itself

    // two marches for the two guesses:
    double delmas;
    delmas = marchGasSteadyTertiary(state.march, dependencies, pchute); // the value returned is the difference between the sum of the valve flow rates
    // and the flow rate computed at the injection choke; negative means the upstream pressure is too low,
    // positive, the upstream pressure is too high
    double delmas2;
    delmas2 = marchGasSteadyTertiary(state.march, dependencies, pchute2);

    int kontaiter = 0;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    if (fabs(delmas) < 1e-3)
        return pchute;
    else {
        if (delmas < 0) { // pressure guess too low, to be raised
            negativeResidualGuess = pchute;
            while (delmas < 0 && kontaiter < 800) {
                pchute *= 1.01;
                if (pchute > state.march.injectionChoke.presEstag)
                    pchute = state.march.injectionChoke.presEstag;
                delmas = marchGasSteadyTertiary(state.march, dependencies, pchute);
                if (delmas < 0 && delmas > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = pchute;
                kontaiter++;
            }
            positiveResidualGuess = pchute;
        } else if (delmas >= 0) { // pressure guess too high, to be lowered
            kontaiter = 0;
            positiveResidualGuess = pchute;
            while (delmas > 0 && kontaiter < 800) {
                pchute *= 0.9;
                delmas = marchGasSteadyTertiary(state.march, dependencies, pchute);
                if (delmas > 0 && delmas < kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = pchute;
                kontaiter++;
            }
            negativeResidualGuess = pchute;
        }
        // in case it does not work with the first guess
        if (delmas2 >= 0 && kontaiter >= 800) { // pressure guess too low, to be raised
            kontaiter = 0;
            positiveResidualGuess = pchute2;
            while (delmas2 > 0 && kontaiter < 800) {
                pchute2 *= 0.9;
                delmas2 = marchGasSteadyTertiary(state.march, dependencies, pchute2);
                if (delmas2 > 0 && delmas2 < kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = pchute2;
                kontaiter++;
            }
            if (kontaiter >= 800) { // failed to find a guess for the false chord
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaGasPresPerm3 atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            negativeResidualGuess = pchute2;
        } else if (delmas2 <= 0 && kontaiter >= 800) { // pressure guess too high, to be lowered
            kontaiter = 0;
            negativeResidualGuess = pchute2;
            while (delmas2 < 0 && kontaiter < 800) {
                pchute2 *= 1.01;
                if (pchute2 > state.march.injectionChoke.presEstag)
                    pchute2 = state.march.injectionChoke.presEstag;
                delmas2 = marchGasSteadyTertiary(state.march, dependencies, pchute2);
                if (delmas2 < 0 && delmas2 > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = pchute2;
                kontaiter++;
            }
            if (kontaiter >= 800) { // failed to find a guess for the false chord
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("Busca de valores iniciais para calculo de zero de funcao em buscaGasPresPerm3 atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            positiveResidualGuess = pchute2;
        }
        return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 0, 1); // root finding
    }
}

double searchReverseProductionBottomHolePressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute) {
    state.reverseSteady = 1;
    state.march.convergenceMonitor = 1000.;
    // search for two initial guesses whose values have opposite signs
    // for marchaProdPerm1, to start the root finding.
    double pchute = state.march.gasSurfacePressure; // initialising pchute with the pressure downstream
    // of the choke; pchute is the guess actually used in the search
    // if chute>0, pchute=chute, otherwise it is estimated
    // of pchute
    double perdafric = 0.;

    estimateInitialReverseBottomHolePressure(state, perdafric, pchute, chute);
    // the method's parameter list
    double pchute2;        // second pressure guess of the search
    double pchuteAux = 0.; // helper in the search for the two pressure guesses
    // needed to start the root-finding method
    double marchResidual; // marchResidual always receives the value the march returns
    // here, the upstream choke pressure minus the pressure of the last cell computed by the
    // march
    marchResidual = marchReverseProductionSteady(state.march, dependencies, pchute);
    state.march.searchOrigin = 1;
    // the march can fail: the pressure somewhere rose above the static pressure of
    // some IPR along the way, returns 1e10;
    // or before reaching the last cell the pressure got close to zero, or the pressure
    // returns -1e10; or, with PVTSim, the pressure rose above the table's maximum,
    // returns 1e10
    classifyReverseMarchSentinel(state, marchResidual, pchuteAux, perdafric);

    // if the first march went wrong, a new attempt is made with pchuteAux
    int kontaiter = 0; // counter for the loop that tries a new estimate
    // for which at least 1e10 or 1e-10 is not returned
    double pchuteAux0 = pchuteAux;
    double abortValue;
    if (retryUntilReverseMarchCompletes(state, dependencies, pchuteAux0, kontaiter, marchResidual, pchuteAux, pchute, chute, abortValue))
        return abortValue;
    if (kontaiter > 100) { // iteration limit reached
        if ((*state.march.globals).networkMode == 0) {
            // in this case the simulation ends: there is no transient
            // to run next and this is not a network
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
            else {
                // only shows a warning
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                // if in a network iteration, after the first iteration
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                // if a transient simulation follows, or this is the first network iteration
                else
                    return kSteadyConvergenceFailure;
            }
        } else {
            // if in a network iteration, after the first iteration
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            // if a transient simulation follows, or this is the first network iteration
            else
                return kSteadyConvergenceFailure;
        }
    }

    // if the previous loop took the estimate from 1e10 to -1e10
    // or from -1e10 to 1e10,
    // that is, from a pressure guess too high to finish the march
    // to one too low to finish it, or the other way round,
    while ((marchResidual < -kSteadyMarchSentinelThreshold || marchResidual > kSteadyMarchSentinelThreshold) && kontaiter < 100) {
        pchute = 0.5 * (pchute + pchuteAux); // a middle value is sought
        // between the too high and the too low
        if ((fabs(pchute - pchuteAux) / pchute) < 0.001 && marchResidual < -kSteadyMarchSentinelThreshold) {
            pchute = 1.05 * pchute;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[0].acsr.ipr.Pres - pchute) > 0.00 * state.march.cells[0].acsr.ipr.Pres) {
                pchute = (10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute > 1.01 * state.march.cells[0].acsr.ipr.Pres)
                    pchute = 1.01 * state.march.cells[0].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
            pchuteAux = pchute;
        }
        marchResidual = marchReverseProductionSteady(state.march, dependencies, pchute);
        kontaiter++;
    }
    if (kontaiter >= 100) {
        if ((*state.march.globals).networkMode == 0) {
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
            else {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        } else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    // if a pressure guess that lets the march finish was found
    kontaiter = 0;
    pchute2 = pchute;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    double guessLowerBound = 0.;
    state.march.input.buscaFC = fabs(state.march.input.buscaFC);
    double reduz = 1. - state.march.input.buscaFC;
    double amplifica = 1. + state.march.input.buscaFC;
    double aumenta = amplifica;
    if (fabs(marchResidual) < 1e-3)
        return pchute;
    else {
        return bracketReverseRoot(state, dependencies, aumenta, reduz, guessLowerBound, positiveResidualGuess, negativeResidualGuess, kontaiter, marchResidual, pchuteAux, pchute2, pchute, chute);
    }
}

double searchProductionBottomHolePressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, int kontaTenta) {
    state.reverseSteady = 0;
    state.march.convergenceMonitor = 1000.;
    // search for two initial guesses whose values have opposite signs
    // for marchaProdPerm1, to start the root finding.
    double pchute = state.march.gasSurfacePressure; // initialising pchute with the pressure downstream
    // of the choke; pchute is the guess actually used in the search
    // if chute>0, pchute=chute, otherwise it is estimated
    // of pchute
    double perdafric = 0.;
    double complementaryFractionGuess = 0.;

    estimateInitialBottomHolePressure(state, complementaryFractionGuess, perdafric, pchute, chute);
    // the method's parameter list
    double pchute2;        // second pressure guess of the search
    double pchuteAux = 0.; // helper in the search for the two pressure guesses
    // needed to start the root-finding method
    double marchResidual; // marchResidual always receives the value the march returns
    // here, the upstream choke pressure minus the pressure of the last cell computed by the
    // march
    marchResidual = marchProductionSteady(state.march, dependencies, pchute);
    state.march.searchOrigin = 1;
    // the march can fail: the pressure somewhere rose above the static pressure of
    // some IPR along the way, returns 1e10;
    // or before reaching the last cell the pressure got close to zero, or the pressure
    // returns -1e10; or, with PVTSim, the pressure rose above the table's maximum,
    // returns 1e10
    classifyMarchSentinel(state, marchResidual, pchuteAux, complementaryFractionGuess, perdafric, pchute);

    // if the first march went wrong, a new attempt is made with pchuteAux
    int kontaiter = 0; // counter for the loop that tries a new estimate
    // for which at least 1e10 or 1e-10 is not returned
    double pchuteAux0 = pchuteAux;
    double abortValue;
    if (retryUntilMarchCompletes(state, dependencies, pchuteAux0, kontaiter, marchResidual, pchuteAux, pchute, chute, kontaTenta, abortValue))
        return abortValue;
    if (kontaiter > 50) { // iteration limit reached
        if ((*state.march.globals).networkMode == 0) {
            // in this case the simulation ends: there is no transient
            // to run next and this is not a network
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            } else {
                // only shows a warning
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if (kontaTenta < 0)
                    logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                               "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                               "", "");
                // if in a network iteration, after the first iteration
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                // if a transient simulation follows, or this is the first network iteration
                else
                    return kSteadyConvergenceFailure;
            }
        } else {
            // if in a network iteration, after the first iteration
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            // if a transient simulation follows, or this is the first network iteration
            else
                return kSteadyConvergenceFailure;
        }
    }

    // if the previous loop took the estimate from 1e10 to -1e10
    // or from -1e10 to 1e10,
    // that is, from a pressure guess too high to finish the march
    // to one too low to finish it, or the other way round,
    while ((marchResidual < -kSteadyMarchSentinelThreshold || marchResidual > kSteadyMarchSentinelThreshold) && kontaiter < 50) {
        pchute = 0.5 * (pchute + pchuteAux); // a middle value is sought
        // between the too high and the too low
        if ((fabs(pchute - pchuteAux) / pchute) < 0.001 && marchResidual < -kSteadyMarchSentinelThreshold) {
            pchute = 1.05 * pchute;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[0].acsr.ipr.Pres - pchute) < 0.00 * state.march.cells[0].acsr.ipr.Pres) {
                pchute = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute < 0.99 * state.march.cells[0].acsr.ipr.Pres)
                    pchute = 0.99 * state.march.cells[0].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
            pchuteAux = pchute;
        }
        marchResidual = marchProductionSteady(state.march, dependencies, pchute);
        if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
            if (kontaTenta < 0)
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
        kontaiter++;
    }
    if (kontaiter >= 50) {
        if ((*state.march.globals).networkMode == 0) {
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0) {
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
                logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
                           "Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes",
                           "", "");
            } else {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        } else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    // if a pressure guess that lets the march finish was found
    kontaiter = 0;
    pchute2 = pchute;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    double guessLowerBound = 0.;
    double val0 = marchResidual;
    int reversao = 0;
    double reduz = 1. - state.march.input.buscaFC;
    double amplifica = 1. + state.march.input.buscaFC;
    if (fabs(marchResidual) < 1e-3)
        return pchute;
    else {
        if (marchResidual < 0.) { // case where the upstream choke pressure < the pressure of the last cell computed by the
            double abortValue;
            if (bracketFromHighGuess(state, dependencies, amplifica, reduz, reversao, val0, guessLowerBound, negativeResidualGuess, kontaiter, marchResidual, pchuteAux, pchute2, pchute, chute, kontaTenta, abortValue))
                return abortValue;
            positiveResidualGuess = pchute2;
        } else if (marchResidual > 0.) { // case where the upstream choke pressure > the pressure of the last cell
            double abortValue;
            if (bracketFromLowGuess(state, dependencies, amplifica, reduz, reversao, val0, positiveResidualGuess, negativeResidualGuess, kontaiter, marchResidual, pchuteAux, pchute2, pchute, chute, kontaTenta, abortValue))
                return abortValue;
        }

        if (reversao == 0)
            return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 1, 0); // with the two pressure estimates
        // on opposite-sign sides of the curve, the root finding
        // starts
        else
            return searchReverseProductionBottomHolePressure(state, dependencies, pchute * 1.);
    }
}

double searchProductionBottomHolePressureSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, int) {
    // search for two initial guesses whose values have opposite signs
    // for marchaProdPerm1, to start the root finding.
    state.reverseSteady = 0;
    state.march.convergenceMonitor = 1000.;
    double pchute = state.march.gasSurfacePressure; // initialising pchute with the pressure downstream
    // of the choke; pchute is the guess actually used in the search
    // if chute>0, pchute=chute, otherwise it is estimated
    // of pchute
    double perdafric = 0.;

    estimateInitialBottomHolePressureSecondary(state, perdafric, pchute, chute);
    // the method's parameter list
    double pchute2;        // second pressure guess of the search
    double pchuteAux = 0.; // helper in the search for the two pressure guesses
    // needed to start the root-finding method
    double marchResidual; // marchResidual always receives the value the march returns
    // here, the upstream choke pressure minus the pressure of the last cell computed by the
    // march
    marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute);
    state.march.searchOrigin = 1;
    // the march can fail: the pressure somewhere rose above the static pressure of
    // some IPR along the way, returns 1e10;
    // or before reaching the last cell the pressure got close to zero, or the pressure
    // returns -1e10; or, with PVTSim, the pressure rose above the table's maximum,
    // returns 1e10
    classifyMarchSentinelSecondary(state, marchResidual, pchuteAux, perdafric);

    double mult1 = 0.9;
    double mult2 = 1.1;

    // if the first march went wrong, a new attempt is made with pchuteAux
    int kontaiter = 0; // counter for the loop that tries a new estimate
    // for which at least 1e10 or 1e-10 is not returned
    double pchuteAux0 = pchuteAux;
    double abortValue;
    if (retryUntilMarchCompletesSecondary(state, dependencies, pchuteAux0, kontaiter, marchResidual, pchuteAux, pchute, abortValue))
        return abortValue;
    if (kontaiter > 50) { // iteration limit reached
        if ((*state.march.globals).networkMode == 0) {
            // in this case the simulation ends: there is no transient
            // to run next and this is not a network
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
            else {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        } else {
            // if in a network iteration, after the first iteration
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            // if a transient simulation follows, or this is the first network iteration
            else
                return kSteadyConvergenceFailure;
        }
    }
    // if the previous loop took the estimate from 1e10 to -1e10
    // or from -1e10 to 1e10,
    // that is, from a pressure guess too high to finish the march
    // to one too low to finish it, or the other way round,
    while ((marchResidual < -kSteadyMarchSentinelThreshold || marchResidual > kSteadyMarchSentinelThreshold) && kontaiter <= 50) {
        pchute = 0.5 * (pchute + pchuteAux); // a middle value is sought
        // between the too high and the too low
        if ((fabs(pchute - pchuteAux) / pchute) < 0.001 && marchResidual < -kSteadyMarchSentinelThreshold) {
            pchute = 1.05 * pchute;
            if (state.march.cells[0].acsr.tipo == kAccessoryInflowPerformance &&
                (state.march.cells[0].acsr.ipr.Pres - pchute) < 0.01 * state.march.cells[0].acsr.ipr.Pres) {
                pchute = -(10 / state.march.cells[0].acsr.ipr.ip) + state.march.cells[0].acsr.ipr.Pres;
                if (pchute < 0.99 * state.march.cells[0].acsr.ipr.Pres)
                    pchute = 0.99 * state.march.cells[0].acsr.ipr.Pres;
            }
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
            pchuteAux = pchute;
        }

        marchResidual = marchProductionSteadySecondary(state.march, dependencies, pchute);
        if (fabs(marchResidual) > kSteadyConvergenceFailureThreshold) {
            cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
        kontaiter++;
    }
    if (kontaiter > 50) {
        if ((*state.march.globals).networkMode == 0) {
            if (state.march.input.transiente == 0 && chute < 0 && state.march.input.AP == 0)
                NumError("Busca de valores iniciais para calculo de zero de funcao em buscaProdPfundoPerm atingiu maximo de iteracoes");
            else {
                cout << "#################PERMANENTE FALHOU EM SUA CONVERGÃŠNCIA##############################" << endl;
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        } else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }

    // if a pressure guess that lets the march finish was found
    kontaiter = 0;
    pchute2 = pchute;
    double guessLowerBound = 0.;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    double reduz = 1. - state.march.input.buscaFC;
    double amplifica = 1. + state.march.input.buscaFC;
    if (fabs(marchResidual) < 1e-3)
        return pchute;
    else {
        if (marchResidual < 0.) { // case where the pressure guess was high
            double abortValue;
            if (bracketFromHighGuessSecondary(state, dependencies, reduz, negativeResidualGuess, guessLowerBound, kontaiter, mult1, marchResidual, pchuteAux, pchute2, pchute, chute, abortValue))
                return abortValue;
            positiveResidualGuess = pchute2;
        } else if (marchResidual > 0.) { // case where choke flow rate < mixture flow rate in the pipe,
            double abortValue;
            if (bracketFromLowGuessSecondary(state, dependencies, amplifica, positiveResidualGuess, negativeResidualGuess, kontaiter, mult2, marchResidual, pchuteAux, pchute2, pchute, chute, abortValue))
                return abortValue;
        }
        return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 1, 1); // with the two pressure estimates
        // on opposite-sign sides of the curve, the root finding
        // starts
    }
}

double searchProductionBottomHolePressureTertiary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double pentrada) {
    state.reverseSteady = 0;
    state.march.convergenceMonitor = 1000.;
    double alfini;
    double betini;

    // void fraction estimate in the first cell of the system
    // mass source at the start of the pipe
    // as with a liquid source, void fraction = no slip
    state.march.cells[0].temp = state.march.cells[0].acsr.injm.temp;
    if (state.march.input.flashCompleto == 2) {
        if (state.march.input.tabelaDinamica == 0)
            state.march.cells[0].flui.atualizaPropComp(pentrada, state.march.cells[0].temp);
        state.march.cells[0].acsr.injm.FluidoPro.atualizaPropComp(pentrada, state.march.cells[0].temp);
    }
    state.march.cells[0].acsr.injm.condTermo = 0;
    state.march.cells[0].pres = pentrada;
    sources::renewSourceTerms(dependencies.sources, 0);
    double qgas = state.march.cells[0].acsr.injm.MassG /
                  state.march.cells[0].acsr.injm.FluidoPro.MasEspGas(pentrada, state.march.cells[0].temp);
    double qliq = state.march.cells[0].acsr.injm.MassP /
                      state.march.cells[0].acsr.injm.FluidoPro.MasEspLiq(pentrada, state.march.cells[0].temp) +
                  state.march.cells[0].acsr.injm.MassC /
                      state.march.cells[0].acsr.injm.fluidocol.MasEspFlu(pentrada, state.march.cells[0].temp);
    state.march.cells[0].acsr.injm.FluidoPro.MasEspLiq(pentrada, state.march.cells[0].temp);
    double qcomp = state.march.cells[0].acsr.injm.MassC /
                   state.march.cells[0].acsr.injm.fluidocol.MasEspFlu(pentrada, state.march.cells[0].temp);
    alfini = qgas / (qliq + qgas);
    if ((fabs(qcomp) + fabs(qliq)) > 1e-15)
        betini = fabs(qcomp) / (fabs(qcomp) + fabs(qliq));
    else
        betini = 0.;

    // this march is for a source at the start of the pipe,
    // so the pipe is taken as closed and a source is placed at the centre of the
    // first cell. The flow rates at the cell's left boundary are therefore 0
    state.march.cells[0].tempL = state.march.cells[0].temp;
    state.march.cells[1].tempL = state.march.cells[0].temp;
    state.march.cells[0].tempini = state.march.cells[0].temp;

    state.march.cells[0].ML = 0.;
    state.march.cells[0].MC = 0.;
    state.march.cells[1].ML = 0.;
    state.march.cells[0].MliqiniL = 0.;
    state.march.cells[0].Mliqini = 0.;
    state.march.cells[1].MliqiniL = 0.;
    state.march.cells[0].MComp = 0.;
    state.march.cells[0].QLL = 0.;
    state.march.cells[0].QL = 0;
    state.march.cells[1].QLL = 0.;
    state.march.cells[0].QG = 0.;
    double masfim = 1.;
    double masfim0 = -10;
    double presteste = 1.;
    double presteste0 = -10;
    state.march.steadyIteration = 0;

    int limIter = 200; // iteration limit when convergence acceleration is off. Turning this option off
    // is not advisable: the gains are small and convergence becomes unstable, especially
    // when the branch is part of a network
    if (state.march.input.AceleraConvergPerm == 1) { // convergence acceleration on
        limIter = 2;                   // usually only two march iterations are made for a given guess
        if (state.march.input.lingas == 1)
            limIter = withGasInletCondition(
                state.march.input.gasinj.tipoCC,
                [](auto condition) { return acceleratedMarchesPerGuess(condition); }); // with

        // A gas injection pressure condition requires an additional coupled march.
    }
    // arq.CriterioConvergPerm is a march convergence criterion; it only makes sense
    // when convergence acceleration is off. Note that this convergence is not
    // the convergence of the problem itself, just a repetition of the march for a given
    // pressure or flow rate guess at the start of the pipe. What convergence really seeks is
    // the bottom-hole pressure or flow rate that satisfies the boundary conditions at the end
    // of the pipe; that is done in the search methods.
    while ((fabs(masfim - masfim0) / fabs(masfim) > state.march.input.CriterioConvergPerm ||
            fabs(presteste - presteste0) / fabs(presteste) > state.march.input.CriterioConvergPerm) &&
           state.march.steadyIteration < limIter) {

        masfim0 = masfim;
        presteste0 = presteste;
        if (state.march.steadyIteration == 0 && state.march.input.lingas > 0 && state.march.input.nvalvgas > 0 && state.march.networkCoupled == 1)
            gaslift::initializeTubingConnectionSteady(dependencies.gasLift); // before the first march iteration,
        // an initial estimate of the thermal coupling between the tubing and the annulus is made,
        // when there is a gas line
        else if (state.march.input.lingas > 0 && state.march.input.nvalvgas > 0 && state.march.networkCoupled == 1)
            gaslift::connectTubingSteady(dependencies.gasLift); // thermal coupling with the pressure and temperature
        // obtained in the first march iteration
        int i;
        int guessNeedsCorrection = 1;
        double abortValue;
        if (marchTertiaryCellsUntilConverged(state, dependencies, guessNeedsCorrection, i, betini, alfini, pentrada, abortValue))
            return abortValue;
        // after the production line march, the gas line is marched,
        // if there is one
        if (pentrada > 0 && state.march.input.lingas > 0 && state.march.input.nvalvgas > 0) {
            withGasInletCondition(state.march.gasCells[0].tipoCC,
                                  [&](auto condition) { solveGasLineForSearch(state, dependencies, condition); });
        }

        if (state.march.input.acopColAnulPermForte > 0 && state.march.input.lingas > 0) {
            refreshProperties(state.march);
            refreshSteadyThermalVelocities(state.march);
            computePseudoTransientTimeStep(state.march);
            gaslift::connectTubing(dependencies.gasLift);
            for (int kontaPseudo = 0; kontaPseudo < state.march.input.acopColAnulPermForte; kontaPseudo++) {
                for (int i = 0; i <= state.march.lastGasCell; i++)
                    state.march.gasCells[i].tempini = state.march.gasCells[i].temp;
                for (int i = 1; i <= state.march.lastGasCell; i++) {
                    thermal::computeGasTemperature(dependencies.thermal, i, state.march.gasCells[i - 1].tempini, 1);
                }
                state.march.cells[0].tempini = state.march.cells[0].temp;
                state.march.cells[1].tempLini = state.march.cells[1].tempL;
                state.march.cells[1].tempL = state.march.cells[0].temp;
                for (int i = 1; i <= state.march.lastCell; i++) {
                    state.march.cells[i].tempini = state.march.cells[i].temp;
                }
                for (int i = 1; i <= state.march.lastCell; i++) {
                    thermal::computeTemperature(dependencies.thermal, i, state.march.cells[i].tempini, 1);
                }
                refreshProperties(state.march);
                computePseudoTransientTimeStep(state.march);
                gaslift::connectTubing(dependencies.gasLift);
            }
        }

        masfim = state.march.cells[state.march.lastCell - 1].MC; // stores the flow rate to compute the error when the
        // convergence acceleration option is off
        presteste = state.march.cells[state.march.lastCell].pres; // stores the pressure to compute the error when the
        // convergence acceleration option is off
        state.march.steadyIteration++; // updates the march iteration
        if (state.march.steadyIteration > 200 && state.march.input.AP == 0)
            NumError("ConvergÃƒÂªncia em marchaProdPerm1 atingiu maximo de iteracoes");
        else if (state.march.steadyIteration > 200)
            return kSteadyConvergenceFailure;
    }

    double corrigePresF = 0.;
    if (((*state.march.globals).networkMode == 0 || state.march.endNode == 1 || (*state.march.globals).parallelNetworkMode == 1) && state.march.input.corrigeContSep == 1)
        corrigePresF = steadyPressureAtLastCell(state.march);

    state.march.gasSurfacePressure = (state.march.cells[state.march.lastCell].pres + corrigePresF);

    return state.march.cells[state.march.lastCell].pres; // if the march reached the last cell,
    // returns the pressure of the last cell computed by the march
}

double searchReverseProductionPressureToPressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, double maximumFlowRate, int kontaiter) {
    state.reverseSteady = 1;
    state.march.convergenceMonitor = 1000.;
    if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection) {
        state.march.cells[0].acsr.injg.FluidoPro = state.reverseNetworkFluid;
    } else if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
        state.march.cells[0].acsr.injl.FluidoPro = state.reverseNetworkFluid;
    } else if (state.march.cells[0].acsr.tipo == kAccessoryMultipleSource) {
        state.march.cells[0].acsr.injm.FluidoPro = state.reverseNetworkFluid;
    }
    double mchute = chute;

    double marchResidual = marchReverseProductionPressureToPressure(state.march, dependencies, mchute);
    state.march.searchOrigin = 1;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
            NumError(
                "Busca de valores iniciais para callculo de zero de funcao em buscaProdPresPresPermRev com problemas, pressao abaixo de 1 kgf/cm2 na marcha reversa");
        else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    double mchute2 = mchute;
    state.march.input.buscaFC = fabs(state.march.input.buscaFC);

    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    int testaEscoa = 1;
    double val0 = marchResidual;
    int reversao = 0;
    if (fabs(marchResidual) < 1e-3)
        return mchute;
    else {
        if (marchResidual < 0.) {
            negativeResidualGuess = mchute;
            while (marchResidual < 0) {
                mchute2 *= (1. + state.march.input.buscaFC);
                marchResidual = marchReverseProductionPressureToPressure(state.march, dependencies, mchute2);
                if (marchResidual > -kSteadyMarchSentinelThreshold) {
                    if (marchResidual < val0) {
                        reversao = 1;
                        marchResidual = 1;
                    } else
                        val0 = marchResidual;
                }
                if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = mchute2;
                kontaiter++;
                double velocityGuess = estimateInletMixtureVelocity(state, mchute2);
                if (kontaiter > 200 && fabs(velocityGuess) > 0.01) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPermRev atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (fabs(velocityGuess) <= 0.01) {
                    testaEscoa = 0;
                    marchResidual = 1.;
                }
            }
            positiveResidualGuess = mchute2;
        } else if (marchResidual > 0.) {
            positiveResidualGuess = mchute;
            while (marchResidual > 0) {
                mchute2 *= (1. - state.march.input.buscaFC);
                marchResidual = marchReverseProductionPressureToPressure(state.march, dependencies, mchute2);
                if (marchResidual > -kSteadyMarchSentinelThreshold) {
                    if (marchResidual > val0) {
                        reversao = 1;
                        marchResidual = -1;
                    } else
                        val0 = marchResidual;
                }
                if (marchResidual > 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = mchute2;
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm  atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
            }
            negativeResidualGuess = mchute2;
        }
        if (testaEscoa == 1 && reversao == 0)
            return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 2, 0);
        else if (reversao == 0) {
            if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection) {
                state.march.cells[0].acsr.injg.QGas = 0.;
            } else if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
                state.march.cells[0].acsr.injl.QLiq = 0.;
            }
            return 0.;
        } else {
            return searchProductionPressureToPressure(state, dependencies, -chute, -maximumFlowRate, kontaiter);
        }
    }
}

double searchProductionPressureToPressure(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, double maximumFlowRate, int kontaiter) {
    struct SearchDiagnostics {
        std::size_t preliminaryEvaluations = 0;
        std::size_t bracketingEvaluations = 0;
        std::size_t completedEvaluations = 0;
        std::size_t pressureLimitedEvaluations = 0;
        std::size_t nonFiniteEvaluations = 0;
        std::size_t positiveResiduals = 0;
        std::size_t negativeResiduals = 0;
        double lastGuess = 0.;
        double lastCompletedGuess = 0.;
        double lastCompletedResidual = 0.;
        double lastAbortedGuess = 0.;
        detail::PressureMarchStop lastStop;
    } diagnostics;
    const auto evaluateMarch = [&](double guess, bool preliminary) {
        detail::PressureMarchStop stop;
        const double residual = detail::marchProductionPressureToPressure(state.march, dependencies, guess, &stop);
        if (preliminary)
            ++diagnostics.preliminaryEvaluations;
        else
            ++diagnostics.bracketingEvaluations;
        diagnostics.lastGuess = guess;
        if (stop.cell >= 0) {
            ++diagnostics.pressureLimitedEvaluations;
            diagnostics.lastAbortedGuess = guess;
            diagnostics.lastStop = stop;
        } else if (std::isfinite(residual)) {
            ++diagnostics.completedEvaluations;
            diagnostics.lastCompletedGuess = guess;
            diagnostics.lastCompletedResidual = residual;
            diagnostics.positiveResiduals += residual > 0.;
            diagnostics.negativeResiduals += residual < 0.;
        } else {
            ++diagnostics.nonFiniteEvaluations;
        }
        return residual;
    };
    const auto failureMessage = [&](const char *message) {
        std::ostringstream output;
        output.imbue(std::locale::classic());
        output << std::setprecision(17) << message
               << "; pressure march diagnostics: preliminary=" << diagnostics.preliminaryEvaluations
               << ", bracketing=" << diagnostics.bracketingEvaluations
               << ", completed=" << diagnostics.completedEvaluations
               << ", pressure-limited=" << diagnostics.pressureLimitedEvaluations
               << ", non-finite=" << diagnostics.nonFiniteEvaluations
               << ", positive-residuals=" << diagnostics.positiveResiduals
               << ", negative-residuals=" << diagnostics.negativeResiduals
               << ", iteration-count=" << kontaiter
               << ", last-flow[m3/day]=" << diagnostics.lastGuess;
        if (diagnostics.completedEvaluations != 0)
            output << ", last-completed-flow[m3/day]=" << diagnostics.lastCompletedGuess
                   << ", last-completed-residual[kgf/cm2]=" << diagnostics.lastCompletedResidual;
        if (diagnostics.pressureLimitedEvaluations != 0)
            output << ", last-aborted-flow[m3/day]=" << diagnostics.lastAbortedGuess
                   << ", stopping-cell=" << diagnostics.lastStop.cell
                   << ", stopping-pressure[kgf/cm2]=" << diagnostics.lastStop.pressure;
        return output.str();
    };

    state.reverseSteady = 0;
    state.march.convergenceMonitor = 1000.;
    double mchute = chute;
    double marchResidual = evaluateMarch(mchute, true);
    state.march.searchOrigin = 1;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        while (marchResidual < -kSteadyMarchSentinelThreshold && kontaiter < 800) {
            mchute *= 0.9;
            marchResidual = evaluateMarch(mchute, true);
            if (mchute < 1e-5) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(failureMessage("Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes"));
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
        marchResidual = evaluateMarch(mchute, true);
        kontaiter++;
    }
    double mchuteAux;
    double mchute2 = mchute;

    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    int testaEscoa = 1;
    double sentido = 1.;
    double val0 = marchResidual;
    int reversao = 0;
    int kontaReverso;
    if (fabs(marchResidual) < 1e-3)
        return mchute;
    else {
        if (marchResidual * sentido < 0.) {
            negativeResidualGuess = mchute;
            kontaReverso = 0;
            while (marchResidual * sentido < 0) {
                mchuteAux = mchute2;
                mchute2 *= (1. - state.march.input.buscaFC);
                marchResidual = evaluateMarch(mchute2, false);
                if (marchResidual > -kSteadyMarchSentinelThreshold) {
                    if (marchResidual < val0) {
                        kontaReverso++;
                        if (kontaReverso == 2) {
                            reversao = 1;
                            marchResidual = 1;
                        } else {
                            state.march.input.buscaFC = -fabs(state.march.input.buscaFC);
                            val0 = marchResidual;
                        }
                    } else
                        val0 = marchResidual;
                }
                if (marchResidual * sentido < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = mchute2;
                kontaiter++;
                double velocityGuess = estimateInletMixtureVelocity(state, mchute2);
                if (kontaiter > 200 && velocityGuess > 0.01) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(failureMessage("Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes"));
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (velocityGuess <= 0.01) {
                    testaEscoa = 0;
                    marchResidual = 1.;
                }
                while (marchResidual < -kSteadyMarchSentinelThreshold) {
                    mchute2 = 0.5 * (mchute2 + mchuteAux);
                    marchResidual = evaluateMarch(mchute2, false);
                    if (marchResidual * sentido < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                        negativeResidualGuess = mchute2;
                    kontaiter++;
                    if (kontaiter > 200) {
                        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                            NumError(failureMessage("Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes"));
                        else {
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    }
                }
            }
            positiveResidualGuess = mchute2;
        } else if (marchResidual * sentido > 0.) {
            positiveResidualGuess = mchute;
            kontaReverso = 0;
            while (marchResidual * sentido > 0) {
                mchuteAux = mchute2;
                mchute2 *= (1. + state.march.input.buscaFC);
                marchResidual = evaluateMarch(mchute2, false);
                if (marchResidual > -kSteadyMarchSentinelThreshold) {
                    if (marchResidual > val0) {
                        kontaReverso++;
                        if (kontaReverso == 2) {
                            reversao = 1;
                            marchResidual = -1;
                        } else {
                            state.march.input.buscaFC = -fabs(state.march.input.buscaFC);
                            val0 = marchResidual;
                        }
                    } else
                        val0 = marchResidual;
                }
                if ((*state.march.globals).networkMode != 0 && mchute2 > maximumFlowRate && maximumFlowRate > 0) {
                    testaEscoa = 0;
                    marchResidual = -1.;
                }
                if (marchResidual * sentido > 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = mchute2;
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(failureMessage("Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm  atingiu maximo de iteracoes"));
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                while (marchResidual < -kSteadyMarchSentinelThreshold) {
                    mchute2 = 0.5 * (mchute2 + mchuteAux);
                    marchResidual = evaluateMarch(mchute2, false);
                    if (marchResidual * sentido > 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                        positiveResidualGuess = mchute2;
                    kontaiter++;

                    if (kontaiter > 200) {
                        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                            NumError(failureMessage("Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes"));
                        else {
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    }
                }
            }
            negativeResidualGuess = mchute2;
        }
        if (testaEscoa == 1 && reversao == 0)
            return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 2, 0);
        else if (reversao == 0) {
            if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection) {
                state.march.cells[0].acsr.injg.QGas = kNoFlowSolutionMarker;
            } else if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
                state.march.cells[0].acsr.injl.QLiq = kNoFlowSolutionMarker;
            }
            return 0.;
        } else {
            return searchReverseProductionPressureToPressure(state, dependencies, -chute, -maximumFlowRate, kontaiter);
        }
    }
}

double searchProductionPressureToPressureSecondary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, double maximumFlowRate) {
    state.reverseSteady = 0;
    state.march.convergenceMonitor = 1000.;
    double mchute = chute;
    double marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute);
    state.march.searchOrigin = 1;
    int kontaiter = 0;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        while (marchResidual < -kSteadyMarchSentinelThreshold && kontaiter < 400) {
            mchute *= 0.9;
            marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute);
            if (mchute < 1e-5) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute);
            kontaiter++;
        }
    }
    double mchuteAux;
    double mchute2 = mchute;
    kontaiter = 0;

    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;

    int testaEscoa = 1;
    if (fabs(marchResidual) < 1e-3)
        return mchute;
    else {
        if (marchResidual < 0.) {
            negativeResidualGuess = mchute;
            while (marchResidual < 0) {
                mchuteAux = mchute2;
                mchute2 *= (1. - state.march.input.buscaFC);
                marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
                if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                    negativeResidualGuess = mchute2;
                kontaiter++;
                double velocityGuess = estimateInletMixtureVelocity(state, mchute2);
                if (kontaiter > 200 && velocityGuess > 0.01) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (velocityGuess <= 0.01) {
                    testaEscoa = 0;
                    marchResidual = 1.;
                }
                while (marchResidual < -kSteadyMarchSentinelThreshold) {
                    mchute2 = 0.5 * (mchute2 + mchuteAux);
                    marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
                    if (marchResidual < 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                        negativeResidualGuess = mchute2;
                    kontaiter++;
                    if (kontaiter > 200) {
                        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                            NumError(
                                "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                        else {
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    }
                }
            }
            positiveResidualGuess = mchute2;
        } else if (marchResidual > 0.) {
            positiveResidualGuess = mchute;
            while (marchResidual > 0) {
                mchuteAux = mchute2;
                mchute2 *= (1. + state.march.input.buscaFC);
                marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
                if ((*state.march.globals).networkMode != 0 && mchute2 > maximumFlowRate) {
                    testaEscoa = 0;
                    marchResidual = -1.;
                }
                if (marchResidual > 0 && marchResidual < kSteadyMarchSentinelThreshold)
                    positiveResidualGuess = mchute2;
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm  atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                while (marchResidual < -kSteadyMarchSentinelThreshold) {
                    mchute2 = 0.5 * (mchute2 + mchuteAux);
                    marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute2);
                    if (marchResidual > 0 && marchResidual > -kSteadyMarchSentinelThreshold)
                        positiveResidualGuess = mchute2;
                    kontaiter++;

                    if (kontaiter > 200) {
                        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                            NumError(
                                "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                        else {
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    }
                }
            }
            negativeResidualGuess = mchute2;
        }
        if (testaEscoa == 1)
            return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 2, 1);
        else {
            if (state.march.cells[0].acsr.tipo == kAccessoryGasInjection) {
                state.march.cells[0].acsr.injg.QGas = 0.;
            } else if (state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
                state.march.cells[0].acsr.injl.QLiq = 0.;
            }
            return 0.;
        }
    }
}

double searchProductionPressureToPressureTertiary(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute, double maximumFlowRate) {

    state.reverseSteady = 0;
    state.march.convergenceMonitor = 1000.;
    double taux;
    double pchute = state.march.input.CCPres.pres[0];
    double totalSourceFlowRate = 0.;
    if (chute < 0.) {
        for (int i = 0; i < state.march.lastCell; i++) {
            taux = state.march.input.celp[i].textern;
            double rhol = state.march.cells[i].flui.MasEspLiq(pchute, taux);
            double rhog = state.march.cells[i].flui.MasEspGas(pchute, taux);
            // if the system is not a main gas-lift ring
            // the pressure estimate uses only the liquid hydrostatics, which gives a
            // very high initial pressure guess
            double alfa = 0.1;
            if ((*state.march.globals).networkMode == 1 && state.holdupGuess < 0.5 && state.holdupGuess > -1e-15)
                alfa = 1. - state.holdupGuess * 2.;
            double rhomix = (1. - alfa) * rhol + alfa * rhog;
            double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i + 1].dx);
            // pressure advance by the hydrostatics and the estimated friction loss
            pchute -= ((rhomix * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed) / units::kPascalPerKgfPerCm2);
            // pressure increment from a constant pressure gain in some cell
            if (state.march.cells[i].acsr.tipo == 7)
                pchute -= state.march.cells[i].acsr.delp;
            // if there is an IPR along the pipe, checks whether the estimated pressure is below
            // the static pressure: this solver does not work with negative flow rates
            // to avoid them, the pressure is corrected to a value close to the
            // static pressure of the cell's IPR
            if (state.march.cells[i].acsr.tipo == kAccessoryInflowPerformance && ((state.march.cells[i].acsr.ipr.Pres - pchute) > 0.0) && i == state.march.lastCell - 1) {

                pchute = -(10 / state.march.cells[i - 1].acsr.ipr.ip) + state.march.cells[i - 1].acsr.ipr.Pres;
                if (pchute < 0.99 * state.march.cells[i - 1].acsr.ipr.Pres)
                    pchute = 0.99 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
            double accessoryFlowRate = 0.;
            if (state.march.cells[i].acsr.tipo == kAccessoryInflowPerformance)
                accessoryFlowRate = -(state.march.cells[i].acsr.ipr.Pres - pchute) * state.march.cells[i].acsr.ipr.ip;
            else if (state.march.cells[i].acsr.tipo == kAccessoryGasInjection && state.march.input.tipoFluido == 1)
                accessoryFlowRate = -state.march.cells[i].acsr.injg.QGas;
            else if (state.march.cells[i].acsr.tipo == kAccessoryLiquidInjection && state.march.input.tipoFluido == 0)
                accessoryFlowRate = -state.march.cells[i].acsr.injl.QLiq;
            else if (state.march.cells[i].acsr.tipo == kAccessoryLeak && state.march.cells[0].acsr.tipo == kAccessoryGasInjection) {
                accessoryFlowRate = 10000.;
            } else if (state.march.cells[i].acsr.tipo == kAccessoryLeak && state.march.cells[0].acsr.tipo == kAccessoryLiquidInjection) {
                accessoryFlowRate = 1000.;
            }

            totalSourceFlowRate += accessoryFlowRate;
            if (totalSourceFlowRate < 0)
                totalSourceFlowRate -= accessoryFlowRate;
            // a guess above the maximum limit of a table is also avoided
            // when PVTSim is used
            if (state.march.input.usaTabela == 1 && (state.march.input.tabent.pmax - pchute) < (*state.march.globals).smallValueTolerance)
                pchute = 0.9 * state.march.input.tabent.pmax;
        }
        chute = totalSourceFlowRate;
    }
    double mchute = chute;
    double marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute);
    state.march.searchOrigin = 1;
    int kontaiter = 0;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        while (marchResidual < -kSteadyMarchSentinelThreshold && kontaiter < 400) {
            mchute *= (1. - state.march.input.buscaFC);
            marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute);
            if (mchute < 1e-5) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Busca de valores iniciais para calculo de zero de funcao em buscaProdPresPresPerm atingiu maximo de iteracoes");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
            marchResidual = marchProductionPressureToPressureSecondary(state.march, dependencies, mchute);
            kontaiter++;
        }
    }
    double mchuteAux;
    double mchute2 = mchute;
    kontaiter = 0;

    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;

    int testaEscoa = 1;
    if (fabs(marchResidual) < 1e-3)
        return mchute;
    else {
        return bracketTertiaryPressureToPressureRoot(state, dependencies, testaEscoa, positiveResidualGuess, negativeResidualGuess, mchute2, mchuteAux, kontaiter, marchResidual, mchute, maximumFlowRate);
    }
}

double searchInjectionBottomHolePressure1(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute) {
    double pavanc = state.march.gasSurfacePressure;
    double mchute = 0;
    double taux = state.march.cells[0].calor.Textern1;
    if (chute < 0) {
        if (state.march.input.flashCompleto < 1) {
            for (int i = 1; i <= state.march.lastCell; i++) {
                double rhol = state.march.cells[i].fluicol.MasEspFlu(pavanc, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                pavanc -= rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed / units::kPascalPerKgfPerCm2;
                if (state.march.cells[i].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i].acsr.ipr.Pres - pavanc) > -(*state.march.globals).smallValueTolerance) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Poco injetor provavelmente produzindo quando se utiliza este valor de pressao na superficie");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i].acsr.tipo == kAccessoryInflowPerformance) {
                    if (state.march.input.condpocinj.tipoFlui < 2)
                        mchute -= state.march.cells[i].acsr.ipr.ij * (state.march.cells[i].acsr.ipr.Pres - pavanc);
                    else
                        mchute -= (state.march.cells[i].acsr.ipr.ij * rhol) * (state.march.cells[i].acsr.ipr.Pres - pavanc) / state.march.cells[i].fluicol.MasEspFlu(1.01, 15.);
                }
                taux = state.march.cells[i].calor.Textern1;
            }
            if (state.march.input.condpocinj.CC == 3) {
                if (state.march.cells[0].acsr.injl.QLiq > 1e-5)
                    mchute = state.march.cells[0].acsr.injl.QLiq;
                else if (state.march.input.condpocinj.tipoFlui < 2)
                    mchute += 100.;
                else
                    mchute += 100. * state.march.cells[state.march.lastCell].fluicol.MasEspFlu(pavanc, taux) / state.march.cells[state.march.lastCell].fluicol.MasEspFlu(1.01, 15.);
            }
        } else {
            for (int i = 1; i <= state.march.lastCell; i++) {
                double rhog = state.march.cells[i].flui.MasEspGas(pavanc, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                pavanc -= rhog * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed / units::kPascalPerKgfPerCm2;
                if (state.march.cells[i].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i].acsr.ipr.Pres - pavanc) > -(*state.march.globals).smallValueTolerance) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Poco injetor provavelmente produzindo quando se utiliza este valor de pressao na superficie");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i].acsr.tipo == kAccessoryInflowPerformance) {
                    if (state.march.input.condpocinj.tipoFlui < 2)
                        mchute -= state.march.cells[i].acsr.ipr.ij * (state.march.cells[i].acsr.ipr.Pres - pavanc);
                    else
                        mchute -= (state.march.cells[i].acsr.ipr.ij * rhog) * (state.march.cells[i].acsr.ipr.Pres - pavanc) / (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions);
                }
                taux = state.march.cells[i].calor.Textern1;
            }
            if (state.march.input.condpocinj.CC == 3) {
                if (state.march.cells[0].acsr.injg.QGas > 1e-5)
                    mchute = state.march.cells[0].acsr.injg.QGas;
                else
                    mchute += 100. * state.march.cells[state.march.lastCell].flui.MasEspGas(pavanc, taux) / (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions);
            }
        }
    } else
        mchute = chute;
    double mchute2;
    double mchuteAux;
    double marchResidual;
    marchResidual = marchInjectionSteady(state.march, dependencies, mchute);
    int kontaiter = 0;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        while (marchResidual < -kSteadyMarchSentinelThreshold) {
            mchute *= 0.9;
            marchResidual = marchInjectionSteady(state.march, dependencies, mchute);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "PoÃ§o injetor com problemas na iteracao na busca de um chute de vazao inicial, perda de carga muito alta");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
    } else if (marchResidual > kSteadyMarchSentinelThreshold) {
        while (marchResidual > kSteadyMarchSentinelThreshold) {
            mchute *= 1.1;
            marchResidual = marchInjectionSteady(state.march, dependencies, mchute);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "PoÃ§o injetor com problemas na iteracao na busca de um chute de vazao inicial, Vazao muito baixa");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
    }
    kontaiter = 0;
    mchute2 = mchute;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    if (fabs(marchResidual) < 1e-15)
        return mchute;
    else {
        if (marchResidual < 0.) {
            negativeResidualGuess = mchute;
            while (marchResidual < 0) {
                mchuteAux = mchute2;
                mchute2 *= 1.1;
                marchResidual = marchInjectionSteady(state.march, dependencies, mchute2);
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm1 atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                while (marchResidual < -kSteadyMarchSentinelThreshold) {
                    mchute2 = 0.5 * (mchute2 + mchuteAux);
                    marchResidual = marchInjectionSteady(state.march, dependencies, mchute2);
                    kontaiter++;
                    if (kontaiter > 200) {
                        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                            NumError(
                                "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm1 atingiu maximo de iteracoes");
                        else {
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    }
                }
            }
            positiveResidualGuess = mchute2;
        } else if (marchResidual > 0.) {
            positiveResidualGuess = mchute;
            while (marchResidual > 0) {
                mchuteAux = mchute2;
                mchute2 *= 0.9;
                marchResidual = marchInjectionSteady(state.march, dependencies, mchute2);
                kontaiter++;
                if (kontaiter > 200) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm1 atingiu maximo de iteracoes");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                while (marchResidual > kSteadyMarchSentinelThreshold) {
                    mchute2 = 0.5 * (mchute2 + mchuteAux);
                    marchResidual = marchInjectionSteady(state.march, dependencies, mchute2);
                    kontaiter++;
                    if (kontaiter > 200) {
                        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                            NumError(
                                "Busca de valores iniciais para calculo de zero de funcao em buscaInjPfundoPerm1 atingiu maximo de iteracoes");
                        else {
                            if ((*state.march.globals).networkIteration > 0)
                                return -kSteadyConvergenceFailure;
                            else
                                return kSteadyConvergenceFailure;
                        }
                    }
                }
            }
            negativeResidualGuess = mchute2;
        }
        return solveSteadyRoot(state, dependencies, negativeResidualGuess, positiveResidualGuess, 1, 0);
    }
}

double searchInjectionBottomHolePressure2(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute) {
    double pchute;
    if (chute < 0) {
        if (state.march.input.flashCompleto < 1) {
            if (state.march.input.condpocinj.tipoFlui < 2)
                pchute = state.march.cells[state.march.lastCell].acsr.ipr.Pres + ((state.march.cells[0].acsr.injl.QLiq) / (state.march.cells[state.march.lastCell].acsr.ipr.ij));
            else
                pchute = state.march.cells[state.march.lastCell].acsr.ipr.Pres + ((state.march.cells[0].acsr.injl.QLiq) * state.march.cells[state.march.lastCell].fluicol.MasEspFlu(1.01, 15.) / (state.march.cells[state.march.lastCell].acsr.ipr.ij * state.march.cells[state.march.lastCell].fluicol.MasEspFlu(state.march.cells[state.march.lastCell].acsr.ipr.Pres, state.march.cells[state.march.lastCell].calor.Textern1)));
            double taux = state.march.cells[0].calor.Textern1;
            double rmis;
            double j;
            double frictionFactor;

            double visC = state.march.cells[0].acsr.injl.fluidocol.VisFlu(pchute, taux);
            double visMis = visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(pchute, taux);
            rmis = complementaryDensityAtGuess;
            double rlcA = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(1.001, 15.);
            double massicC = rlcA * state.march.cells[0].acsr.injl.QLiq / units::kSecondsPerDay;
            j = (massicC / complementaryDensityAtGuess) / state.march.cells[0].duto.area;
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);

            double perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.march.cells[0].duto.peri / state.march.cells[0].duto.area;
            for (int i = state.march.lastCell; i > 0; i--) {
                double rhol = state.march.cells[i].fluicol.MasEspFlu(pchute, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                pchute += (rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - pchute) > -(*state.march.globals).smallValueTolerance)
                    pchute = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
                taux = state.march.cells[i].calor.Textern1;
            }
        } else {
            pchute = state.march.cells[state.march.lastCell].acsr.ipr.Pres + ((state.march.cells[0].acsr.injg.QGas) * (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions) / (state.march.cells[state.march.lastCell].acsr.ipr.ij * state.march.cells[state.march.lastCell].flui.MasEspGas(state.march.cells[state.march.lastCell].acsr.ipr.Pres, state.march.cells[state.march.lastCell].calor.Textern1)));
            double taux = state.march.cells[0].calor.Textern1;
            double rmis;
            double j;
            double frictionFactor;

            double visC = state.march.cells[0].acsr.injg.FluidoPro.ViscGas(pchute, taux);
            double visMis = visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injg.FluidoPro.MasEspGas(pchute, taux);
            rmis = complementaryDensityAtGuess;
            double rlcA = (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions);
            double massicC = rlcA * state.march.cells[0].acsr.injg.QGas / units::kSecondsPerDay;
            j = (massicC / complementaryDensityAtGuess) / state.march.cells[0].duto.area;
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);

            double perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.march.cells[0].duto.peri / state.march.cells[0].duto.area;
            for (int i = state.march.lastCell; i > 0; i--) {
                double rhol = state.march.cells[i].flui.MasEspGas(pchute, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                pchute += (rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - pchute) > -(*state.march.globals).smallValueTolerance)
                    pchute = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
                taux = state.march.cells[i].calor.Textern1;
            }
        }
    } else
        pchute = chute;
    if (pchute <= 1)
        pchute = 1.01;
    double pchute2;
    double pchuteAux = 0.;
    double marchResidual;
    marchResidual = marchInjectionSteady(state.march, dependencies, pchute);
    int kontaiter = 0;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        while (marchResidual < -kSteadyMarchSentinelThreshold) {
            pchute *= 1.1;
            marchResidual = marchInjectionSteady(state.march, dependencies, pchute);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "PoÃ§o injetor com problemas na iteracao na busca de um chute de pressao inicial, perda de carga muito alta");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
    } else if (marchResidual > kSteadyMarchSentinelThreshold) {
        while (marchResidual > kSteadyMarchSentinelThreshold) {
            pchute *= 0.9;
            marchResidual = marchInjectionSteady(state.march, dependencies, pchute);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "PoÃ§o injetor com problemas na iteracao na busca de um chute de vazao inicial, pressao muito alta");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
    }
    kontaiter = 0;
    pchute2 = pchute;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    if (fabs(marchResidual) < 1e-15)
        return pchute;
    else {
        return bracketInjectionPressureRoot(state, dependencies, positiveResidualGuess, negativeResidualGuess, kontaiter, marchResidual, pchuteAux, pchute2, pchute, SubAtmosphericGuess::refused);
    }
}

double searchInjectionBottomHolePressure3(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute) {
    state.march.cells[state.march.lastCell].pres = state.march.input.condpocinj.presfundo;
    if (state.march.cells[state.march.lastCell].pres < state.march.cells[state.march.lastCell].acsr.ipr.Pres) {
        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
            NumError("Valor de pressao de fundo menor que a pressao de reservatÃ³rio");
        else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    double mchute;
    if (chute < 0.) {
        if (state.march.input.flashCompleto < 1) {
            if (state.march.input.condpocinj.tipoFlui < 2)
                mchute = -(state.march.cells[state.march.lastCell].acsr.ipr.ij) * (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres);
            else
                mchute = -(state.march.cells[state.march.lastCell].acsr.ipr.ij * state.march.cells[state.march.lastCell].fluicol.MasEspFlu(state.march.cells[state.march.lastCell].pres,
                                                                                     state.march.cells[state.march.lastCell].calor.Textern1)) *
                         (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres) / state.march.cells[state.march.lastCell].fluicol.MasEspFlu(1.01, 15.);
            double taux = state.march.cells[0].calor.Textern1;
            for (int i = state.march.lastCell; i > 0; i--) {
                double rhol = state.march.cells[i].fluicol.MasEspFlu(state.march.cells[i].pres, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                state.march.cells[i - 1].pres = state.march.cells[i].pres + rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed / units::kPascalPerKgfPerCm2;
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres) > -(*state.march.globals).smallValueTolerance) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError("Valor de pressao de fundo menor que a pressao de reservatÃ³rio");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
                    if (state.march.input.condpocinj.tipoFlui < 2)
                        mchute -= (state.march.cells[i - 1].acsr.ipr.ij) * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                    else
                        mchute -= (state.march.cells[i - 1].acsr.ipr.ij * state.march.cells[i - 1].fluicol.MasEspFlu(state.march.cells[i - 1].pres, 0.) / state.march.cells[state.march.lastCell].fluicol.MasEspFlu(1.01, 15.)) * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                }
                taux = state.march.cells[i].calor.Textern1;
            }
        } else {
            if (state.march.input.condpocinj.tipoFlui < 2)
                mchute = -(state.march.cells[state.march.lastCell].acsr.ipr.ij) * (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres);
            else
                mchute = -(state.march.cells[state.march.lastCell].acsr.ipr.ij * state.march.cells[state.march.lastCell].flui.MasEspGas(state.march.cells[state.march.lastCell].pres,
                                                                                  state.march.cells[state.march.lastCell].calor.Textern1)) *
                         (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres) / (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions);
            double taux = state.march.cells[0].calor.Textern1;
            for (int i = state.march.lastCell; i > 0; i--) {
                double rhol = state.march.cells[i].flui.MasEspGas(state.march.cells[i].pres, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                state.march.cells[i - 1].pres = state.march.cells[i].pres + rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed / units::kPascalPerKgfPerCm2;
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres) > -(*state.march.globals).smallValueTolerance) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError("Valor de pressao de fundo menor que a pressao de reservatÃ³rio");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
                    if (state.march.input.condpocinj.tipoFlui < 2)
                        mchute -= (state.march.cells[i - 1].acsr.ipr.ij) * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                    else
                        mchute -= (state.march.cells[i - 1].acsr.ipr.ij * state.march.cells[i - 1].flui.MasEspGas(state.march.cells[i - 1].pres, 0.) / (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions)) * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                }
                taux = state.march.cells[i].calor.Textern1;
            }
        }
    } else
        mchute = chute;

    state.march.gasSurfacePressure = state.march.cells[0].pres;
    double marchResidual;
    marchResidual = marchInjectionSteady(state.march, dependencies, mchute);
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
            NumError(
                "PoÃ§o injetor com problemas na iteracao na busca de um chute de pressao inicial, perda de carga muito alta");
        else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    if (marchResidual > kSteadyMarchSentinelThreshold) {
        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
            NumError(
                "PoÃ§o injetor com problemas na iteracao na busca de um chute de vazao inicial, pressao muito alta");
        else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    int konta = 0;
    while (fabs(state.march.cells[state.march.lastCell].pres - state.march.input.condpocinj.presfundo) / state.march.input.condpocinj.presfundo > 0.01 / 100 && konta < 200) {
        state.march.cells[state.march.lastCell].pres = state.march.input.condpocinj.presfundo;
        if (state.march.input.flashCompleto < 1) {
            if (state.march.input.condpocinj.tipoFlui < 2)
                mchute = -state.march.cells[state.march.lastCell].acsr.ipr.ij * (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres);
            else
                mchute = -(state.march.cells[state.march.lastCell].acsr.ipr.ij * state.march.cells[state.march.lastCell].fluicol.MasEspFlu(state.march.cells[state.march.lastCell].pres, state.march.cells[state.march.lastCell].temp) / state.march.cells[state.march.lastCell].fluicol.MasEspFlu(1.01, 15.)) * (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres);
            for (int i = state.march.lastCell; i > 0; i--) {
                state.march.cells[i - 1].pres = state.march.cells[i].pres + gaslift::steadyInjectionPressureDrop(dependencies.gasLift, i);
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres) > -(*state.march.globals).smallValueTolerance) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError("Valor de pressao de fundo menor que a pressao de reservatÃ³rio");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
                    if (state.march.input.condpocinj.tipoFlui < 2)
                        mchute -= state.march.cells[i - 1].acsr.ipr.ij * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                    else
                        mchute -= (state.march.cells[i - 1].acsr.ipr.ij * state.march.cells[i - 1].fluicol.MasEspFlu(state.march.cells[i - 1].pres, state.march.cells[i - 1].temp) / state.march.cells[state.march.lastCell].fluicol.MasEspFlu(1.01, 15.)) * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                }
            }
        } else {
            if (state.march.input.condpocinj.tipoFlui < 2)
                mchute = -state.march.cells[state.march.lastCell].acsr.ipr.ij * (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres);
            else
                mchute = -(state.march.cells[state.march.lastCell].acsr.ipr.ij * state.march.cells[state.march.lastCell].flui.MasEspGas(state.march.cells[state.march.lastCell].pres, state.march.cells[state.march.lastCell].temp) / (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions)) * (state.march.cells[state.march.lastCell].acsr.ipr.Pres - state.march.cells[state.march.lastCell].pres);
            for (int i = state.march.lastCell; i > 0; i--) {
                state.march.cells[i - 1].pres = state.march.cells[i].pres + gaslift::steadyInjectionPressureDrop(dependencies.gasLift, i);
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres) > -(*state.march.globals).smallValueTolerance) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError("Valor de pressao de fundo menor que a pressao de reservatÃ³rio");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance) {
                    if (state.march.input.condpocinj.tipoFlui < 2)
                        mchute -= state.march.cells[i - 1].acsr.ipr.ij * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                    else
                        mchute -= (state.march.cells[i - 1].acsr.ipr.ij * state.march.cells[i - 1].flui.MasEspGas(state.march.cells[i - 1].pres, state.march.cells[i - 1].temp) / (state.march.cells[0].flui.Deng * kAirDensityAtStandardConditions)) * (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres);
                }
            }
        }
        state.march.gasSurfacePressure = state.march.cells[0].pres;
        marchResidual = marchInjectionSteady(state.march, dependencies, mchute);
        konta++;
        if (marchResidual < -kSteadyMarchSentinelThreshold) {
            if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                NumError(
                    "PoÃ§o injetor com problemas na iteracao na busca de um chute de pressao inicial, perda de carga muito alta");
            else {
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        }
        if (marchResidual > kSteadyMarchSentinelThreshold) {
            if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                NumError(
                    "PoÃ§o injetor com problemas na iteracao na busca de um chute de pressao inicial, pressao muito alta");
            else {
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        }
    }
    if (konta >= 200) {
        if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
            NumError("PoÃ§o injetor com problemas na iteracao");
        else {
            if ((*state.march.globals).networkIteration > 0)
                return -kSteadyConvergenceFailure;
            else
                return kSteadyConvergenceFailure;
        }
    }
    return (state.march.cells[state.march.lastCell].pres - state.march.input.condpocinj.presfundo);
}

double searchInjectionBottomHolePressure4(const SteadyStateSearchState &state, const SteadyDependencies &dependencies) {

    if (state.march.input.flashCompleto < 1)
        state.march.cells[0].temp = state.march.cells[0].acsr.injl.temp;
    else
        state.march.cells[0].temp = state.march.cells[0].acsr.injg.temp;
    double alfini = 0.;
    double betini = 1.;

    if (state.march.input.flashCompleto >= 1) {
        if (state.march.cells[0].acsr.injg.seco == 1) {
            alfini = 1.;
            betini = 0.;
        } else {
            if (state.march.input.flashCompleto == 2) {
                if (state.march.input.tabelaDinamica == 0)
                    state.march.cells[0].flui.atualizaPropComp(state.march.gasSurfacePressure, state.march.cells[0].acsr.injg.temp, -1, NULL, NULL, state.march.input.pocinjec);
                state.march.cells[0].acsr.injg.FluidoPro.atualizaPropComp(state.march.gasSurfacePressure, state.march.cells[0].acsr.injg.temp, -1, NULL, NULL, state.march.input.pocinjec);
            }
            double masgas = state.march.cells[0].acsr.injg.VMas(state.march.gasSurfacePressure, state.march.cells[0].acsr.injg.temp);
            double quality;
            if (state.march.input.flashCompleto != 2)
                quality = state.march.cells[0].acsr.injg.FluidoPro.FracMassHidra(1., 20.);
            else
                quality = state.march.cells[0].acsr.injg.FluidoPro.dStockTankVaporMassFraction;
            double masT = masgas / quality;
            quality = state.march.cells[0].acsr.injg.FluidoPro.FracMassHidra(state.march.gasSurfacePressure, state.march.cells[0].acsr.injg.temp);
            double qgas = masT * quality /
                          state.march.cells[0].acsr.injg.FluidoPro.MasEspGas(state.march.gasSurfacePressure, state.march.cells[0].acsr.injg.temp);
            double qliq = masT * (1. - quality) /
                          state.march.cells[0].acsr.injg.FluidoPro.MasEspLiq(state.march.gasSurfacePressure, state.march.cells[0].acsr.injg.temp);
            alfini = qgas / (qliq + qgas);
            betini = 0.;
        }
    }

    state.march.cells[0].tempL = state.march.cells[0].temp;
    state.march.cells[1].tempL = state.march.cells[0].temp;
    state.march.cells[0].tempini = state.march.cells[0].temp;
    state.march.cells[0].ML = 0.;
    state.march.cells[0].MC = 0.;
    state.march.cells[1].ML = 0.;
    state.march.cells[0].MliqiniL = 0.;
    state.march.cells[0].Mliqini = 0.;
    state.march.cells[1].MliqiniL = 0.;
    state.march.cells[0].QLL = 0.;
    state.march.cells[0].QL = 0;
    state.march.cells[1].QLL = 0.;
    state.march.cells[0].QG = 0.;
    if (state.march.input.condpocinj.CC == 0)
        state.march.gasSurfacePressure = state.march.input.condpocinj.presinj;
    state.march.steadyIteration = 0;

    double masfim = 0.;

    int i;
    int guessNeedsCorrection = 1;
    while (guessNeedsCorrection == 1) {
        state.march.cells[0].presauxL = state.march.gasSurfacePressure;
        state.march.cells[0].presLini = state.march.gasSurfacePressure;
        state.march.cells[0].presL = state.march.gasSurfacePressure;
        state.march.cells[0].pres = state.march.gasSurfacePressure;
        state.march.cells[1].presL = state.march.gasSurfacePressure;
        state.march.cells[0].presini = state.march.gasSurfacePressure;
        state.march.cells[1].presLini = state.march.gasSurfacePressure;
        state.march.cells[0].presaux = state.march.gasSurfacePressure;
        state.march.cells[1].presauxL = state.march.gasSurfacePressure;
        state.march.cells[0].alf = alfini;
        state.march.cells[0].alfini = alfini;
        state.march.cells[0].bet = betini;
        state.march.cells[0].betini = betini;
        state.march.cells[1].alfL = state.march.cells[0].alf;
        state.march.cells[1].alfLini = state.march.cells[0].alf;
        state.march.cells[0].alfPigD = state.march.cells[0].alf;
        state.march.cells[0].alfPigDini = state.march.cells[0].alf;
        state.march.cells[0].alfPigE = state.march.cells[0].alf;
        state.march.cells[0].alfPigEini = state.march.cells[0].alf;
        state.march.cells[1].betL = state.march.cells[0].bet;
        state.march.cells[1].betLini = state.march.cells[0].bet;
        state.march.cells[0].betPigD = state.march.cells[0].bet;
        state.march.cells[0].betPigDini = state.march.cells[0].bet;
        state.march.cells[0].betPigE = state.march.cells[0].bet;
        state.march.cells[0].betPigEini = state.march.cells[0].bet;
        state.march.cells[0].betI = state.march.cells[0].bet;
        state.march.cells[1].betLI = state.march.cells[0].bet;
        i = 1;
        while (i <= state.march.lastCell && state.march.cells[i - 1].pres >= 1. && masfim >= -(*state.march.globals).smallValueTolerance) {

            advanceUpstreamSteadyPressure(state.march, i, 0);
            refreshUpstreamProductionPeriphery(state.march, i);
            if (state.march.input.flashCompleto != 2)
                advanceSteadyMass(state.march, dependencies, i);
            else
                advanceCompositionalSteadyMass(state.march, dependencies, i);
            thermal::advanceSteadyTemperature(dependencies.thermal, i, 0);
            thermal::updateProductionTemperaturePeriphery(dependencies.thermal, i);
            advanceDownstreamSteadyPressure(state.march, i, 0);
            refreshDownstreamProductionPeriphery(state.march, i);
            advanceSteadyMassTransfer(state.march, i - 1);
            masfim += (state.march.cells[i - 1].fontemassCR + state.march.cells[i - 1].fontemassLR + state.march.cells[i - 1].fontemassGR);
            i++;
            if (state.march.cells[i - 1].pres <= 1) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("pressao menor que 1");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            } else if ((state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - state.march.cells[i - 1].pres) > -(*state.march.globals).smallValueTolerance)) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("Valor de pressao de fundo menor que a pressao de reservatÃ³rio");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            } else if (i < (state.march.lastCell + 1) && masfim < -(*state.march.globals).smallValueTolerance) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError("Vazao massica de injeÃ§Ã£o menor que 0");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
        if (i == state.march.lastCell + 1)
            guessNeedsCorrection = 0;
        else if (!(state.march.cells[i - 1].pres >= 1.)) { // stopped on the pressure
            if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                NumError("pressao menor que 1");
            else {
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        } else { // stopped on the mass flow
            if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                NumError("Vazao massica de injeÃ§Ã£o menor que 0");
            else {
                if ((*state.march.globals).networkIteration > 0)
                    return -kSteadyConvergenceFailure;
                else
                    return kSteadyConvergenceFailure;
            }
        }
    }
    sources::renewSourceTerms(dependencies.sources, state.march.lastCell);
    masfim += (state.march.cells[state.march.lastCell].fontemassCR + state.march.cells[state.march.lastCell].fontemassLR + state.march.cells[state.march.lastCell].fontemassGR);

    return masfim;
}

double searchInjectionBottomHolePressure5(const SteadyStateSearchState &state, const SteadyDependencies &dependencies, double chute) {

    double pchute = state.march.input.condpocinj.presfundo;
    double taux;
    double j = 0.;
    double rmis = 0.;
    double frictionFactor = 0.;
    taux = state.march.input.celp[0].textern;
    if (chute < 0.) {
        if (state.march.input.flashCompleto < 1) {
            double visC = state.march.cells[0].acsr.injl.fluidocol.VisFlu(pchute, taux);
            double visMis = visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(pchute, taux);
            rmis = complementaryDensityAtGuess;
            double rlcA = state.march.cells[0].acsr.injl.fluidocol.MasEspFlu(1.001, 15.);
            double massicC = rlcA * state.march.cells[0].acsr.injl.QLiq / units::kSecondsPerDay;
            j = (massicC / complementaryDensityAtGuess) / state.march.cells[0].duto.area;
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);
            double perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.march.cells[0].duto.peri / state.march.cells[0].duto.area;
            for (int i = state.march.lastCell; i > 0; i--) {
                double rhol = state.march.cells[i].fluicol.MasEspFlu(pchute, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                pchute += (rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
                if (pchute < 0.5) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Poco injetor com problemas na iteracao na busca de um chute de pressao inicial, perda de carga muito alta");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - pchute) > -(*state.march.globals).smallValueTolerance)
                    pchute = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
        } else {
            double visC = state.march.cells[0].acsr.injg.FluidoPro.ViscGas(pchute, taux);
            double visMis = visC;
            double complementaryDensityAtGuess = state.march.cells[0].acsr.injg.FluidoPro.MasEspGas(pchute, taux);
            rmis = complementaryDensityAtGuess;
            double rlcA = (state.march.cells[0].acsr.injg.FluidoPro.Deng * kAirDensityAtStandardConditions);
            double massicC = rlcA * state.march.cells[0].acsr.injg.QGas / units::kSecondsPerDay;
            j = (massicC / complementaryDensityAtGuess) / state.march.cells[0].duto.area;
            double reynolds;
            if (state.march.cells[0].duto.revest == 0)
                reynolds = state.march.cells[0].Rey(state.march.cells[0].duto.a, j, rmis, visMis);
            else {
                double dhid = 4 * state.march.cells[0].duto.area / state.march.cells[0].duto.peri;
                reynolds = state.march.cells[0].Rey(dhid, j, rmis, visMis);
            }
            frictionFactor = state.march.cells[0].fric(reynolds, state.march.cells[0].duto.rug / state.march.cells[0].duto.a);
            double perdafric = (frictionFactor * rmis * j * fabs(j) / 2.) * state.march.cells[0].duto.peri / state.march.cells[0].duto.area;
            for (int i = state.march.lastCell; i > 0; i--) {
                double rhol = state.march.cells[i].flui.MasEspGas(pchute, taux);
                double dxmed = 0.5 * (state.march.cells[i].dx + state.march.cells[i - 1].dx);
                pchute += (rhol * 9.81 * sin(state.march.cells[i].duto.teta) * dxmed + perdafric * dxmed) / units::kPascalPerKgfPerCm2;
                if (pchute < 0.5) {
                    if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                        NumError(
                            "Poco injetor com problemas na iteracao na busca de um chute de pressao inicial, perda de carga muito alta");
                    else {
                        if ((*state.march.globals).networkIteration > 0)
                            return -kSteadyConvergenceFailure;
                        else
                            return kSteadyConvergenceFailure;
                    }
                }
                if (state.march.cells[i - 1].acsr.tipo == kAccessoryInflowPerformance && (state.march.cells[i - 1].acsr.ipr.Pres - pchute) > -(*state.march.globals).smallValueTolerance)
                    pchute = 1.01 * state.march.cells[i - 1].acsr.ipr.Pres;
            }
        }
    } else
        pchute = chute;
    double pchute2;
    double pchuteAux = 0.;
    double marchResidual;
    marchResidual = marchInjectionSteady(state.march, dependencies, pchute);
    int kontaiter = 0;
    if (marchResidual < -kSteadyMarchSentinelThreshold) {
        while (marchResidual < -kSteadyMarchSentinelThreshold) {
            pchute *= 1.1;
            marchResidual = marchInjectionSteady(state.march, dependencies, pchute);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Poco injetor com problemas na iteracao na busca de um chute de pressao inicial, perda de carga muito alta");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
    } else if (marchResidual > kSteadyMarchSentinelThreshold) {
        while (marchResidual > kSteadyMarchSentinelThreshold) {
            pchute *= 0.9;
            marchResidual = marchInjectionSteady(state.march, dependencies, pchute);
            kontaiter++;
            if (kontaiter > 200) {
                if ((*state.march.globals).networkMode == 0 && state.march.input.AP == 0)
                    NumError(
                        "Poco injetor com problemas na iteracao na busca de um chute de vazao inicial, pressao muito alta");
                else {
                    if ((*state.march.globals).networkIteration > 0)
                        return -kSteadyConvergenceFailure;
                    else
                        return kSteadyConvergenceFailure;
                }
            }
        }
    }
    kontaiter = 0;
    pchute2 = pchute;
    double negativeResidualGuess = 0.;
    double positiveResidualGuess = 0.;
    if (fabs(marchResidual) < 1e-15)
        return pchute;
    else {
        return bracketInjectionPressureRoot(state, dependencies, positiveResidualGuess, negativeResidualGuess, kontaiter, marchResidual, pchuteAux, pchute2, pchute, SubAtmosphericGuess::marched);
    }
}

}  // namespace sisprod::steady
