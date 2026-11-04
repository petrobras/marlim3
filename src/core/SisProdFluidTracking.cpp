#include "SisProdFluidTracking.h"
#include "SisProdFluidTrackingInternal.h"

#include "Leitura.h"
#include "SisProdConstants.h"
#include "celula3.h"
#include "SimulationContext.h"
#include "UnitConversions.h"

#include <math.h>

namespace sisprod::fluidtracking {

namespace {

void transportBlackOilBalances(const FluidTrackingState &state, const BlackOilFace &left,
                               const BlackOilFace &right, const BlackOilCell &cell,
                               const BlackOilSource &source, BlackOilBalance &balance, double dx,
                               double flowArea, int i, Vcr<double> &rgo, Vcr<double> &dg, Vcr<double> &yco2,
                               Vcr<double> &API, Vcr<double> &BSW, Vcr<double> &denag, Vcr<double> &VISCL,
                               Vcr<double> &VISCH, double dt) {
    if (hasMultipleFluidsOrNetwork(state)) {
        balance.MultPe = 0.;
        if (state.cells[i].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultPe = state.cells[i].QL * (1 - left.betI) * (1 - left.bsw) / left.oilVolumeFactor;
        balance.MultPd = 0.;
        if (state.cells[i + 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultPd =
                state.cells[i + 1].QL * (1 - right.betI) * (1 - right.bsw) / right.oilVolumeFactor;
        balance.residuoP = (balance.volpesFim - state.cells[i].VolPesaST) * flowArea / dt +
                           (balance.MultPd - balance.MultPe) / dx - source.deadOil / dx;
        balance.MultAe = 0.;
        if (state.cells[i].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultAe = state.cells[i].QL * (1 - left.betI) * left.bsw / left.oilVolumeFactor;
        balance.MultAd = 0.;
        if (state.cells[i + 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultAd = state.cells[i + 1].QL * (1 - right.betI) * right.bsw / right.oilVolumeFactor;
        balance.residuoA = (balance.volaguaFim - state.cells[i].VolAguaST) * flowArea / dt +
                           (balance.MultAd - balance.MultAe) / dx - source.water / dx;
    }
    if (cell.liquidHoldup > (*state.globals).smallValueTolerance &&
        cell.complementaryFraction < (1. - (*state.globals).smallValueTolerance) &&
        cell.bsw < (1. - (*state.globals).smallValueTolerance)) {
        rgo[i] = (balance.volleveFim - balance.residuo * dt / flowArea) * cell.oilVolumeFactor /
                 (cell.liquidHoldup * (1 - cell.complementaryFraction) * (1 - cell.bsw));
        if (rgo[i] > (*state.globals).maximumGasOilRatio)
            rgo[i] = (*state.globals).maximumGasOilRatio;
    } else if (cell.complementaryFraction >= (1. - (*state.globals).smallValueTolerance) ||
               cell.bsw >= (1. - (*state.globals).smallValueTolerance))
        rgo[i] = 0.;
    else
        rgo[i] = (*state.globals).maximumGasOilRatio;

    if (balance.volleveFim > 1e-5 && state.input.flashCompleto == 0 &&
        ((source.freeGas >= 0 || source.dissolvedGas > 0) ||
         ((balance.MultGd < 0 || balance.MultGe > 0) || (balance.MultOd < 0 || balance.MultOe > 0)))) {
        dg[i] = (dt * (source.dgO * source.dissolvedGas / dx + source.dgG * source.freeGas / dx +
                       1. * cell.dgini * balance.residuo -
                       (right.dgO * balance.MultOd - left.dgO * balance.MultOe) / dx -
                       (right.dgG * balance.MultGd - left.dgG * balance.MultGe) / dx) +
                 cell.dgini * state.cells[i].VolLeveST * flowArea) /
                (balance.volleveFim * flowArea - 0. * balance.residuo * dt);
        yco2[i] = (dt * (source.yco2O * source.dissolvedGas / dx + source.yco2G * source.freeGas / dx +
                         1. * cell.yco2ini * balance.residuo -
                         (right.yco2O * balance.MultOd - left.yco2O * balance.MultOe) / dx -
                         (right.yco2G * balance.MultGd - left.yco2G * balance.MultGe) / dx) +
                   cell.yco2ini * state.cells[i].VolLeveST * flowArea) /
                  (balance.volleveFim * flowArea - 0. * balance.residuo * dt);
        if (yco2[i] < 0.)
            yco2[i] = 0.;
        else if (yco2[i] > 1.)
            yco2[i] = 1.;
    } else {
        dg[i] = cell.dgini;
        yco2[i] = cell.yco2ini;
    }
    if (tracksBlackOilOilProperties(state)) {
        if (balance.volpesFim > 1e-3 && (source.deadOil > 0 || (balance.MultPd < 0 || balance.MultPe > 0))) {
            double denmixSTDF = 141.5 / (131.5 + source.API);
            double denmixSTDini = 141.5 / (131.5 + cell.APIini);
            double denmixSTD1 = 141.5 / (131.5 + right.API);
            double denmixSTD0 = 141.5 / (131.5 + left.API);
            API[i] = (dt * (denmixSTDF * source.deadOil / dx + 1. * denmixSTDini * balance.residuoP -
                            (denmixSTD1 * balance.MultPd - denmixSTD0 * balance.MultPe) / dx) +
                      denmixSTDini * state.cells[i].VolPesaST * flowArea) /
                     (balance.volpesFim * flowArea - 0. * balance.residuoP * dt);
            API[i] = 141.5 / API[i] - 131.5;
            VISCL[i] = (dt * (source.viscL * source.deadOil / dx + 1. * cell.viscLini * balance.residuoP -
                              (right.viscL * balance.MultPd - left.viscL * balance.MultPe) / dx) +
                        cell.viscLini * state.cells[i].VolPesaST * flowArea) /
                       (balance.volpesFim * flowArea - 0. * balance.residuoP * dt);
            VISCH[i] = (dt * (source.viscH * source.deadOil / dx + 1. * cell.viscHini * balance.residuoP -
                              (right.viscH * balance.MultPd - left.viscH * balance.MultPe) / dx) +
                        cell.viscHini * state.cells[i].VolPesaST * flowArea) /
                       (balance.volpesFim * flowArea - 0. * balance.residuoP * dt);
        } else {
            API[i] = cell.APIini;
            VISCL[i] = cell.viscLini;
            VISCH[i] = cell.viscHini;
        }
    }
    if (hasMultipleFluidsOrNetwork(state)) {
        if ((balance.volaguaFim + balance.volpesFim) > 1e-3 &&
            ((source.deadOil > 0 || source.water > 0) ||
             ((balance.MultPd < 0 || balance.MultPe > 0) || (balance.MultAd < 0 || balance.MultAe > 0)))) {
            BSW[i] = (dt * (source.BSW * (source.water + source.deadOil) / dx +
                            1. * cell.BSWini * (balance.residuoA + balance.residuoP) -
                            (right.BSW * (balance.MultAd + balance.MultPd) -
                             left.BSW * (balance.MultAe + balance.MultPe)) /
                                dx) +
                      cell.BSWini * (state.cells[i].VolAguaST + state.cells[i].VolPesaST) * flowArea) /
                     ((balance.volaguaFim + balance.volpesFim) * flowArea -
                      0. * (balance.residuoA + balance.residuoP) * dt);
            denag[i] = (dt * (source.denag * (source.water) / dx + 1. * cell.denagini * (balance.residuoA) -
                              (right.denag * (balance.MultAd) - left.denag * (balance.MultAe)) / dx) +
                        cell.denagini * (state.cells[i].VolAguaST) * flowArea) /
                       ((balance.volaguaFim) * flowArea - 0. * (balance.residuoA) * dt);
            if (BSW[i] < 0.)
                BSW[i] = 0.;
            else if (BSW[i] > 1.)
                BSW[i] = 1.;
            if (denag[i] < 1.)
                denag[i] = 1.;
        } else
            BSW[i] = cell.BSWini;
        denag[i] = cell.denagini;
    }
}

void blackOilSourceCellFluid(const FluidTrackingState &state, BlackOilSource &source, int i,
                             double temperatureHigh, double temperatureLow) {
    if (state.cells[i].acsr.tipo == kAccessoryChoke || state.cells[i].acsr.tipo == kAccessoryVolumetricPump) {
        source.dgO = state.cells[i].flui.Deng;
        source.yco2O = state.cells[i].flui.yco2;
        double sourceGasOilRatio = state.cells[i].flui.RGO;
        source.stockTankQuality = state.cells[i].flui.dStockTankVaporMassFraction;
        double rholiq = (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                        state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
        double rhogas = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
        source.rholST = rholiq + rhogas * sourceGasOilRatio * (1. - state.cells[i].flui.BSW);
        source.rhogST = rhogas;
        double solutionGasRatioSource = state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                                        (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        double dissolvedGasDensityRatio = 1 / state.cells[i].flui.rDgD;
        source.razdgl = 1 / state.cells[i].flui.rDgL;
        source.dissolvedGas =
            (source.dissolvedGas + source.freeGas) * (dissolvedGasDensityRatio * solutionGasRatioSource *
                                                      (1. - state.cells[i].flui.BSW) / source.rholST);
        if (hasMultipleFluidsOrNetwork(state)) {
            double rhoPSTF =
                (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
            source.deadOil *= ((1 - state.cells[i].flui.BSW) / rhoPSTF);
            source.water *= (state.cells[i].flui.BSW / rhoPSTF);
            source.API = state.cells[i].flui.API;
            source.BSW = state.cells[i].flui.BSW;
            source.viscL = state.cells[i].flui.VisOM(temperatureLow);
            source.viscH = state.cells[i].flui.VisOM(temperatureHigh);
        }
    } else if (state.cells[i].acsrL != 0 && ((*state.cells[i].acsrL).tipo == kAccessoryChoke ||
                                             (*state.cells[i].acsrL).tipo == kAccessoryVolumetricPump)) {
        double rholiq;
        double rhogas;

        if (i > 0) {
            source.dgO = state.cells[i - 1].flui.Deng;
            source.yco2O = state.cells[i - 1].flui.yco2;
            double sourceGasOilRatio = state.cells[i - 1].flui.RGO;
            rholiq =
                (1 - state.cells[i - 1].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i - 1].flui.API)) +
                state.cells[i - 1].flui.BSW * 1000 * state.cells[i - 1].flui.Denag;
            rhogas = state.cells[i - 1].flui.Deng * kAirDensityAtStandardConditions;
            source.stockTankQuality = state.cells[i - 1].flui.dStockTankVaporMassFraction;
            source.rholST = rholiq + rhogas * sourceGasOilRatio * (1. - state.cells[i - 1].flui.BSW);
            source.rhogST = rhogas;
            double solutionGasRatioSource =
                state.cells[i - 1].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
            double dissolvedGasDensityRatio = 1 / state.cells[i - 1].flui.rDgD;
            source.razdgl = 1 / state.cells[i - 1].flui.rDgL;
            source.dissolvedGas =
                (source.dissolvedGas + source.freeGas) * (dissolvedGasDensityRatio * solutionGasRatioSource *
                                                          (1. - state.cells[i - 1].flui.BSW) / source.rholST);
            if (hasMultipleFluidsOrNetwork(state)) {
                double rhoPSTF = (1 - state.cells[i - 1].flui.BSW) *
                                     (1000 * 141.5 / (131.5 + state.cells[i - 1].flui.API)) +
                                 state.cells[i - 1].flui.BSW * 1000 * state.cells[i - 1].flui.Denag;
                source.deadOil *= ((1 - state.cells[i - 1].flui.BSW) / rhoPSTF);
                source.water *= (state.cells[i - 1].flui.BSW / rhoPSTF);
                source.API = state.cells[i - 1].flui.API;
                source.BSW = state.cells[i - 1].flui.BSW;
                source.denag = state.cells[i - 1].flui.Denag;
                source.viscL = state.cells[i - 1].flui.VisOM(temperatureLow);
                source.viscH = state.cells[i - 1].flui.VisOM(temperatureHigh);
            }
        } else {
            source.dgO = state.cells[i].flui.Deng;
            source.yco2O = state.cells[i].flui.yco2;
            double sourceGasOilRatio = state.cells[i].flui.RGO;
            rholiq = (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                     state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
            rhogas = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
            source.stockTankQuality = state.cells[i].flui.dStockTankVaporMassFraction;
            source.rholST = rholiq + rhogas * sourceGasOilRatio * (1. - state.cells[i].flui.BSW);
            source.rhogST = rhogas;
            (void)(state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                   (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre));
            double dissolvedGasDensityRatio = 1 / state.cells[i].flui.rDgD;
            source.razdgl = 1 / state.cells[i].flui.rDgL;
            source.dissolvedGas =
                (source.dissolvedGas + source.freeGas) *
                (dissolvedGasDensityRatio * state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                 (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                 (1. - state.cells[i].flui.BSW) / source.rholST);
            if (hasMultipleFluidsOrNetwork(state)) {
                double rhoPSTF =
                    (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                    state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
                source.deadOil *= ((1 - state.cells[i].flui.BSW) / rhoPSTF);
                source.water *= (state.cells[i].flui.BSW / rhoPSTF);
                source.API = state.cells[i].flui.API;
                source.BSW = state.cells[i].flui.BSW;
                source.denag = state.cells[i].flui.Denag;
                source.viscL = state.cells[i].flui.VisOM(temperatureLow);
                source.viscH = state.cells[i].flui.VisOM(temperatureHigh);
            }
        }
    }
}

void blackOilSourcePorous2D(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF, int i,
                            double temperatureHigh, double temperatureLow) {
    if ((state.cells[i].fontemassLR + state.cells[i].fontemassGR) > 1e-15)
        fluF = state.cells[i].acsr.radialPoro.flup;
    else
        fluF = state.cells[i].flui;

    source.stockTankQuality = fluF.dStockTankVaporMassFraction;
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;

    source.rholST =
        (1 - state.cells[i].acsr.poroso2D.dados.transfer.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) +
        state.cells[i].acsr.poroso2D.dados.transfer.BSW * 1000 * fluF.Denag +
        fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio *
            (1. - state.cells[i].acsr.poroso2D.dados.transfer.BSW);
    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    source.dissolvedGas = (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
                          (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                           (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                           (1. - state.cells[i].acsr.poroso2D.dados.transfer.BSW) / source.rholST);
    if (hasMultipleFluidsOrNetwork(state)) {
        convertBlackOilPorousRates(state, source, fluF, state.cells[i].acsr.poroso2D.dados.transfer.BSW,
                                   solutionGasRatioSource, temperatureHigh, temperatureLow);
    }
}

void blackOilSourceRadialPorous(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF, int i,
                                double temperatureHigh, double temperatureLow) {
    if ((state.cells[i].fontemassLR + state.cells[i].fontemassGR) > 1e-15)
        fluF = state.cells[i].acsr.radialPoro.flup;
    else
        fluF = state.cells[i].flui;

    source.stockTankQuality = fluF.dStockTankVaporMassFraction;
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;

    source.rholST = (1 - state.cells[i].acsr.radialPoro.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) +
                    state.cells[i].acsr.radialPoro.BSW * 1000 * fluF.Denag +
                    fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio *
                        (1. - state.cells[i].acsr.radialPoro.BSW);
    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    source.dissolvedGas = (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
                          (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                           (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                           (1. - state.cells[i].acsr.radialPoro.BSW) / source.rholST);
    if (hasMultipleFluidsOrNetwork(state)) {
        convertBlackOilPorousRates(state, source, fluF, state.cells[i].acsr.radialPoro.BSW,
                                   solutionGasRatioSource, temperatureHigh, temperatureLow);
    }
}

void blackOilSourceLeak(const FluidTrackingState &state, BlackOilSource &source, int i,
                        double temperatureHigh, double temperatureLow) {
    ProFlu fluF;
    if (state.cells[i].acsr.fontechk.presT > state.cells[i].acsr.fontechk.pamb) {
        fluF = state.cells[i].acsr.fontechk.fluidoP;
    } else {
        fluF = state.cells[i].acsr.fontechk.fluidoPamb;
    }
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;
    source.stockTankQuality = fluF.dStockTankVaporMassFraction;

    if (fluF.BSW < 1 - (*state.globals).smallValueTolerance)
        source.rholST = (1 - fluF.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) + fluF.BSW * 1000 * fluF.Denag +
                        fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio * (1. - fluF.BSW);
    else
        source.rholST = fluF.BSW * 1000 * fluF.Denag;

    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    if (state.cells[i].acsr.fontechk.ambGas != 1 || (source.dissolvedGas + source.freeGas) < 0.)
        source.dissolvedGas = (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
                              (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                               (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
                               (1. - fluF.BSW) / source.rholST);
    else
        source.dissolvedGas = 0.;
    if (hasMultipleFluidsOrNetwork(state)) {
        double bswaux = fluF.BSW;
        double contrabsw = 1. - bswaux;
        if (contrabsw <= (*state.globals).smallValueTolerance)
            contrabsw = 0.9 * (*state.globals).smallValueTolerance;
        double rhoPSTF;
        if (contrabsw > (*state.globals).smallValueTolerance)
            rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag +
                      fluF.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
        else
            rhoPSTF = 1000 * fluF.Denag;
        if (contrabsw > (*state.globals).smallValueTolerance) {
            source.deadOil *= (1. / rhoPSTF);
            source.water *= ((bswaux / contrabsw) / rhoPSTF);
        } else {
            source.deadOil = 0.;
            source.water *= (1 / (fluF.BSW * 1000 * fluF.Denag));
        }
        source.API = fluF.API;
        source.BSW = fluF.BSW;
        source.denag = fluF.Denag;
        source.viscL = fluF.VisOM(temperatureLow);
        source.viscH = fluF.VisOM(temperatureHigh);
    }
}

void blackOilSourceInflowPerformance(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                     int i, double temperatureHigh, double temperatureLow) {
    if ((state.cells[i].acsr.ipr.Pres) > state.cells[i].pres)
        fluF = state.cells[i].acsr.ipr.FluidoPro;
    else
        fluF = state.cells[i].flui;

    source.stockTankQuality = fluF.dStockTankVaporMassFraction;
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;

    source.rholST = (1 - fluF.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) + fluF.BSW * 1000 * fluF.Denag +
                    fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio * (1. - fluF.BSW);
    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    source.dissolvedGas =
        (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
        (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
         (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) * (1. - fluF.BSW) / source.rholST);
    if (hasMultipleFluidsOrNetwork(state)) {
        convertBlackOilPorousRates(state, source, fluF, fluF.BSW, solutionGasRatioSource, temperatureHigh,
                                   temperatureLow);
    }
}

void blackOilSourceMultipleSource(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                  int i, double temperatureHigh, double temperatureLow) {
    if ((state.cells[i].acsr.injm.MassC + state.cells[i].acsr.injm.MassG + state.cells[i].acsr.injm.MassP) >
        0.)
        fluF = state.cells[i].acsr.injm.FluidoPro;
    else
        fluF = state.cells[i].flui;

    source.stockTankQuality = fluF.dStockTankVaporMassFraction;
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;

    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    source.rholST = (1 - fluF.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) + fluF.BSW * 1000 * fluF.Denag +
                    fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio * (1. - fluF.BSW);
    source.dissolvedGas =
        (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
        (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
         (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) * (1. - fluF.BSW) / source.rholST);

    if (hasMultipleFluidsOrNetwork(state)) {
        convertBlackOilInjectionRates(state, source, fluF, solutionGasRatioSource, temperatureHigh,
                                      temperatureLow);
    }
}

void blackOilSourceLiquidInjection(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                   int i, double temperatureHigh, double temperatureLow) {
    if (state.cells[i].acsr.injl.QLiq > 0.)
        fluF = state.cells[i].acsr.injl.FluidoPro;
    else
        fluF = state.cells[i].flui;

    source.stockTankQuality = fluF.dStockTankVaporMassFraction;
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;

    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    if (fluF.BSW < 1 - (*state.globals).smallValueTolerance)
        source.rholST = (1 - fluF.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) + fluF.BSW * 1000 * fluF.Denag +
                        fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio * (1. - fluF.BSW);
    else
        source.rholST = fluF.BSW * 1000 * fluF.Denag;
    source.dissolvedGas =
        (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
        (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
         (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) * (1. - fluF.BSW) / source.rholST);

    if (hasMultipleFluidsOrNetwork(state)) {
        convertBlackOilInjectionRates(state, source, fluF, solutionGasRatioSource, temperatureHigh,
                                      temperatureLow);
    }
}

void blackOilSourceWetGasInjection(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                   int i, double temperatureHigh, double temperatureLow) {
    if (state.cells[i].acsr.injg.QGas > 0.)
        fluF = state.cells[i].acsr.injg.FluidoPro;
    else
        fluF = state.cells[i].flui;

    source.stockTankQuality = fluF.dStockTankVaporMassFraction;
    source.dgO = fluF.Deng;
    source.yco2O = fluF.yco2;
    double sourceGasOilRatio = fluF.RGO;

    double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                    (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    double dissolvedGasDensityRatio = 1 / fluF.rDgD;
    source.razdgl = 1 / fluF.rDgL;
    if (state.cells[i].acsr.injg.FluidoPro.BSW < 1 - (*state.globals).smallValueTolerance)
        source.rholST = (1 - fluF.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) + fluF.BSW * 1000 * fluF.Denag +
                        fluF.Deng * kAirDensityAtStandardConditions * sourceGasOilRatio * (1. - fluF.BSW);
    else
        source.rholST = fluF.BSW * 1000 * fluF.Denag;
    source.dissolvedGas =
        (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio *
        (fluF.RS(state.cells[i].pres, state.cells[i].temp) *
         (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) * (1. - fluF.BSW) / source.rholST);

    if (hasMultipleFluidsOrNetwork(state)) {
        double bswaux = fluF.BSW;
        double contrabsw = 1. - bswaux;
        if (contrabsw < (*state.globals).smallValueTolerance)
            contrabsw = 0.9 * (*state.globals).smallValueTolerance;
        double rhoPSTF;
        if (contrabsw > (*state.globals).smallValueTolerance)
            rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag +
                      fluF.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
        else
            rhoPSTF = 1000 * fluF.Denag;
        if (contrabsw > (*state.globals).smallValueTolerance) {
            source.deadOil *= (1. / rhoPSTF);
            source.water *= ((bswaux / contrabsw) / rhoPSTF);
        } else {
            source.deadOil = 0.;
            source.water *= (1 / (fluF.BSW * 1000 * fluF.Denag));
        }
        source.API = fluF.API;
        source.BSW = fluF.BSW;
        source.denag = state.cells[i].acsr.injg.FluidoPro.Denag;
        source.viscL = fluF.VisOM(temperatureLow);
        source.viscH = fluF.VisOM(temperatureHigh);
    }
}

void upwindLeftFaceGasProperties(const FluidTrackingState &state, BlackOilFace &left, int i) {
    if ((i > 0 || state.input.ConContEntrada == 1) && state.cells[i].QG > 0) {
        left.rhogST = (*state.cells[i].fluiL).Deng * kAirDensityAtStandardConditions;
        left.dgG = (*state.cells[i].fluiL).Deng;
        left.yco2G = (*state.cells[i].fluiL).yco2;
    } else {
        left.rhogST = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
        left.dgG = state.cells[i].flui.Deng;
        left.yco2G = state.cells[i].flui.yco2;
    }
}

void upwindRightFaceLiquidProperties(const FluidTrackingState &state, BlackOilFace &right, int i,
                                     double temperatureHigh, double temperatureLow) {
    if (state.cells[i + 1].QL < 0.) {
        right.betI = state.cells[i + 1].betPigE;
        prepareBlackOilLiquidFace(right, state.cells[i + 1].flui, state.cells[i + 1].pres,
                                  state.cells[i + 1].temp, temperatureHigh, temperatureLow);
    }
}

void upwindLeftFaceBlackOilLiquid(const FluidTrackingState &state, BlackOilFace &left, int i,
                                  double temperatureHigh, double temperatureLow) {
    if ((i > 0 || state.input.ConContEntrada == 1) && state.cells[i].QL >= 0.) {
        double upstreamPressure;
        double upstreamTemperature;
        if (i > 0) {
            upstreamPressure = state.cells[i - 1].pres;
            upstreamTemperature = state.cells[i - 1].temp;
        } else {
            upstreamPressure = state.inletPressure;
            upstreamTemperature = state.inletTemperature;
        }
        if (state.input.ConContEntrada == 0)
            left.betI = state.cells[i - 1].betPigD;
        else
            left.betI = state.inletComplementaryFraction;
        prepareBlackOilLiquidFace(left, (*state.cells[i].fluiL), upstreamPressure, upstreamTemperature,
                                  temperatureHigh, temperatureLow);
    } else {
        left.betI = state.cells[i].betPigE;
        prepareBlackOilLiquidFace(left, state.cells[i].flui, state.cells[i].pres, state.cells[i].temp,
                                  temperatureHigh, temperatureLow);
    }
}

void transportCellBlackOilProperties(const FluidTrackingState &state, int i, Vcr<double> &rgo,
                                     Vcr<double> &dg, Vcr<double> &yco2, Vcr<double> &API, Vcr<double> &BSW,
                                     Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                     double temperatureHigh, double temperatureLow, double dt) {
    BlackOilBalance balance;
    double flowArea = state.cells[i].duto.area;
    double dx = state.cells[i].dx;
    BlackOilFace left;
    BlackOilFace right;
    prepareBlackOilFaces(state, left, right, i, temperatureHigh, temperatureLow);

    BlackOilCell cell;
    BlackOilSource source;
    prepareBlackOilCellSourceBasis(state, cell, source, i, temperatureHigh, temperatureLow);
    ProFlu fluF;
    prepareBlackOilAccessorySource(state, source, fluF, i, temperatureHigh, temperatureLow);

    prepareBlackOilStockTankBalances(state, left, right, cell, source, balance, dx, flowArea, i, dt);
    transportBlackOilBalances(state, left, right, cell, source, balance, dx, flowArea, i, rgo, dg, yco2, API,
                              BSW, denag, VISCL, VISCH, dt);
    state.cells[i].VolLeveST = balance.volleveFim;
    state.cells[i].VolPesaST = balance.volpesFim;
    state.cells[i].VolAguaST = balance.volaguaFim;
}

void convertBlackOilPorousRates(const FluidTrackingState &state, BlackOilSource &source, const ProFlu &fluF,
                                const double &sourceWaterCut, double solutionGasRatioSource,
                                double temperatureHigh, double temperatureLow) {
    double bswaux = sourceWaterCut;
    double contrabsw = 1. - bswaux;
    if (contrabsw <= (*state.globals).smallValueTolerance)
        contrabsw = 0.9 * (*state.globals).smallValueTolerance;
    double rhoPSTF;
    if (contrabsw > 0)
        rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag +
                  fluF.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
    else
        rhoPSTF = 1000 * fluF.Denag;
    if (contrabsw > (*state.globals).smallValueTolerance) {
        source.deadOil *= (1. / rhoPSTF);
        source.water *= ((bswaux / contrabsw) / rhoPSTF);
    } else {
        source.deadOil = 0.;
        source.water *= (1 / (sourceWaterCut * 1000 * fluF.Denag));
    }
    source.API = fluF.API;
    source.BSW = sourceWaterCut;
    source.denag = fluF.Denag;
    source.viscL = fluF.VisOM(temperatureLow);
    source.viscH = fluF.VisOM(temperatureHigh);
}

void convertBlackOilInjectionRates(const FluidTrackingState &state, BlackOilSource &source,
                                   const ProFlu &fluF, double solutionGasRatioSource, double temperatureHigh,
                                   double temperatureLow) {
    double bswaux = fluF.BSW;
    double contrabsw = 1. - bswaux;
    if (contrabsw < (*state.globals).smallValueTolerance)
        contrabsw = 0.9 * (*state.globals).smallValueTolerance;
    double rhoPSTF;
    if (contrabsw > (*state.globals).smallValueTolerance)
        rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag +
                  fluF.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
    else
        rhoPSTF = 1000 * fluF.Denag;
    if (contrabsw > (*state.globals).smallValueTolerance) {
        source.deadOil *= (1. / rhoPSTF);
        source.water *= ((bswaux / contrabsw) / rhoPSTF);
    } else {
        source.deadOil = 0.;
        source.water *= (1 / (fluF.BSW * 1000 * fluF.Denag));
    }
    source.API = fluF.API;
    source.BSW = fluF.BSW;
    source.denag = fluF.Denag;
    source.viscL = fluF.VisOM(temperatureLow);
    source.viscH = fluF.VisOM(temperatureHigh);
}

void prepareBlackOilLiquidFace(BlackOilFace &face, const ProFlu &fluid, const double &pressure,
                               const double &temperature, double temperatureHigh, double temperatureLow) {
    face.solutionGasRatio = fluid.RS(pressure, temperature);
    face.oilVolumeFactor = fluid.BOFunc(pressure, temperature, face.solutionGasRatio);
    face.waterVolumeFactor = fluid.BAFunc(pressure, temperature);
    face.bsw = fluid.BSW * face.waterVolumeFactor /
               (face.oilVolumeFactor + face.waterVolumeFactor * fluid.BSW - fluid.BSW * face.oilVolumeFactor);
    face.solutionGasRatio =
        face.solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
    face.dgO = fluid.Deng;
    face.razdgd = 1 / fluid.rDgD;
    face.razdgl = 1 / fluid.rDgL;
    face.yco2O = fluid.yco2;
    face.API = fluid.API;
    face.BSW = fluid.BSW;
    face.denag = fluid.Denag;
    face.viscL = fluid.VisOM(temperatureLow);
    face.viscH = fluid.VisOM(temperatureHigh);
}

void preparePhaseLiquidFace(PhaseFace &face, const ProFlu &fluid, const double &pressure,
                            const double &temperature, double temperatureHigh, double temperatureLow) {
    double solutionGasRatio = fluid.RS(pressure, temperature);
    face.oilVolumeFactor = fluid.BOFunc(pressure, temperature, solutionGasRatio);
    face.waterVolumeFactor = fluid.BAFunc(pressure, temperature);
    face.bsw = fluid.BSW * face.waterVolumeFactor /
               (face.oilVolumeFactor + face.waterVolumeFactor * fluid.BSW - fluid.BSW * face.oilVolumeFactor);
    face.BSW = fluid.BSW;
    face.denag = fluid.Denag;
    face.viscL = fluid.VisOM(temperatureLow);
    face.viscH = fluid.VisOM(temperatureHigh);
}

void convertPhaseInjectionRates(const FluidTrackingState &state, PhaseSource &source, const ProFlu &fluid,
                                double solutionGasRatioSource, double temperatureHigh,
                                double temperatureLow) {
    double bswaux = fluid.BSW;
    double contrabsw = 1. - bswaux;
    if (contrabsw < (*state.globals).smallValueTolerance)
        contrabsw = 0.9 * (*state.globals).smallValueTolerance;
    double rhoPSTF;
    if (contrabsw > (*state.globals).smallValueTolerance)
        rhoPSTF = (1000 * 141.5 / (131.5 + fluid.API)) + (bswaux / contrabsw) * 1000 * fluid.Denag +
                  fluid.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
    else
        rhoPSTF = 1000 * fluid.Denag;
    if (contrabsw > (*state.globals).smallValueTolerance) {
        source.deadOil *= (1. / rhoPSTF);
        source.water *= ((bswaux / contrabsw) / rhoPSTF);
    } else {
        source.deadOil = 0.;
        source.water *= (1 / (fluid.BSW * 1000 * fluid.Denag));
    }
    source.BSW = fluid.BSW;
    source.denag = fluid.Denag;
    source.viscL = fluid.VisOM(temperatureLow);
    source.viscH = fluid.VisOM(temperatureHigh);
}

void convertPhasePorousRates(const FluidTrackingState &state, PhaseSource &source, const ProFlu &fluid,
                             const double &sourceWaterCut, double solutionGasRatioSource,
                             double temperatureHigh, double temperatureLow) {
    double bswaux = sourceWaterCut;
    double contrabsw = 1. - bswaux;
    if (contrabsw <= (*state.globals).smallValueTolerance)
        contrabsw = 0.9 * (*state.globals).smallValueTolerance;
    double rhoPSTF;
    if (contrabsw > 0)
        rhoPSTF = (1000 * 141.5 / (131.5 + fluid.API)) + (bswaux / contrabsw) * 1000 * fluid.Denag +
                  fluid.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
    else
        rhoPSTF = 1000 * fluid.Denag;
    if (contrabsw > (*state.globals).smallValueTolerance) {
        source.deadOil *= (1. / rhoPSTF);
        source.water *= ((bswaux / contrabsw) / rhoPSTF);
    } else {
        source.deadOil = 0.;
        source.water *= (1 / (sourceWaterCut * 1000 * fluid.Denag));
    }
    source.BSW = sourceWaterCut;
    source.denag = fluid.Denag;
    source.viscL = fluid.VisOM(temperatureLow);
    source.viscH = fluid.VisOM(temperatureHigh);
}

bool hasMultipleFluidsOrNetwork(const FluidTrackingState &state) {
    return state.input.nfluP > 1 || (*state.globals).networkMode != 0;
}

bool hasProductionFluidOrNetwork(const FluidTrackingState &state) {
    return state.input.nfluP > 0 || (*state.globals).networkMode != 0;
}

bool tracksBlackOilOilProperties(const FluidTrackingState &state) {
    return (state.input.nfluP > 1 && state.input.flashCompleto == 0) || (*state.globals).networkMode != 0;
}

void computePhaseLiquidVolumeBalances(const FluidTrackingState &state, const PhaseFace &left,
                                      const PhaseFace &right, const PhaseSource &source,
                                      PhaseBalance &balance, double dx, double flowArea, int i, double dt) {
    if ((*state.globals).simulationTime < 1e-15 && state.input.snaps != 1) {
        state.cells[i].VolPesaST = balance.volpesFim;
        state.cells[i].VolAguaST = balance.volaguaFim;
    }
    if (hasMultipleFluidsOrNetwork(state)) {
        balance.MultPe = 0.;
        if (state.cells[i].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultPe = state.cells[i].QL * (1 - left.betI) * (1 - left.bsw) / left.oilVolumeFactor;
        balance.MultPd = 0.;
        if (state.cells[i + 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultPd =
                state.cells[i + 1].QL * (1 - right.betI) * (1 - right.bsw) / right.oilVolumeFactor;
        balance.residuoP = (balance.volpesFim - state.cells[i].VolPesaST) * flowArea / dt +
                           (balance.MultPd - balance.MultPe) / dx - source.deadOil / dx;
        balance.MultAe = 0.;
        if (state.cells[i].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultAe = state.cells[i].QL * (1 - left.betI) * left.bsw / left.oilVolumeFactor;
        balance.MultAd = 0.;
        if (state.cells[i + 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
            balance.MultAd = state.cells[i + 1].QL * (1 - right.betI) * right.bsw / right.oilVolumeFactor;
        balance.residuoA = (balance.volaguaFim - state.cells[i].VolAguaST) * flowArea / dt +
                           (balance.MultAd - balance.MultAe) / dx - source.water / dx;
    }
}

void prepareBlackOilFaces(const FluidTrackingState &state, BlackOilFace &left, BlackOilFace &right, int i,
                          double temperatureHigh, double temperatureLow) {
    upwindLeftFaceBlackOilLiquid(state, left, i, temperatureHigh, temperatureLow);
    if (left.oilVolumeFactor < 1e-15)
        left.oilVolumeFactor = 1e-15;

    right.betI = state.cells[i].betPigD;
    right.solutionGasRatio = state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp);
    right.oilVolumeFactor =
        state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp, right.solutionGasRatio);
    right.waterVolumeFactor = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);
    right.bsw = state.cells[i].flui.BSW * right.waterVolumeFactor /
                (right.oilVolumeFactor + right.waterVolumeFactor * state.cells[i].flui.BSW -
                 state.cells[i].flui.BSW * right.oilVolumeFactor);
    right.solutionGasRatio =
        right.solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;
    right.dgO = state.cells[i].flui.Deng;
    right.yco2O = state.cells[i].flui.yco2;
    right.API = state.cells[i].flui.API;
    right.BSW = state.cells[i].flui.BSW;
    right.denag = state.cells[i].flui.Denag;
    right.viscL = state.cells[i].flui.VisOM(temperatureLow);
    right.viscH = state.cells[i].flui.VisOM(temperatureHigh);
    right.razdgd = 1 / state.cells[i].flui.rDgD;
    right.razdgl = 1 / state.cells[i].flui.rDgL;

    upwindRightFaceLiquidProperties(state, right, i, temperatureHigh, temperatureLow);
    if (right.oilVolumeFactor < 1e-15)
        right.oilVolumeFactor = 1e-15;

    upwindLeftFaceGasProperties(state, left, i);

    right.rhogST = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
    right.dgG = state.cells[i].flui.Deng;
    right.yco2G = state.cells[i].flui.yco2;
    if (state.cells[i + 1].QG <= 0.) {
        right.rhogST = state.cells[i + 1].flui.Deng * kAirDensityAtStandardConditions;
        right.dgG = state.cells[i + 1].flui.Deng;
        right.yco2G = state.cells[i + 1].flui.yco2;
    }
}

void prepareBlackOilCellSourceBasis(const FluidTrackingState &state, BlackOilCell &cell,
                                    BlackOilSource &source, int i, double temperatureHigh,
                                    double temperatureLow) {
    cell.liquidHoldup = 1. - state.cells[i].alf;
    cell.complementaryFraction = state.cells[i].bet;
    cell.rholST = (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                  state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
    cell.rhog = state.cells[i].rgC;
    cell.rhogST = state.cells[i].flui.Deng * kAirDensityAtStandardConditions;
    cell.solutionGasRatio = state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp);
    cell.razdgd = 1 / state.cells[i].flui.rDgD;
    cell.razdgl = 1 / state.cells[i].flui.rDgL;
    cell.oilVolumeFactor =
        state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp, cell.solutionGasRatio);
    cell.waterVolumeFactor = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);
    cell.bsw = state.cells[i].flui.BSW * cell.waterVolumeFactor /
               (cell.oilVolumeFactor + cell.waterVolumeFactor * state.cells[i].flui.BSW -
                state.cells[i].flui.BSW * cell.oilVolumeFactor);
    cell.solutionGasRatio =
        cell.solutionGasRatio * units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre;

    cell.dgini = state.cells[i].flui.Deng;
    cell.yco2ini = state.cells[i].flui.yco2;
    source.dissolvedGas = state.cells[i].fontemassLR;
    source.freeGas = state.cells[i].fontemassGR;
    source.deadOil = state.cells[i].fontemassLR;
    source.water = state.cells[i].fontemassLR;
    cell.APIini = state.cells[i].flui.API;
    cell.BSWini = state.cells[i].flui.BSW;
    cell.denagini = state.cells[i].flui.Denag;
    source.API = cell.APIini;
    source.BSW = cell.BSWini;
    source.denag = cell.denagini;
    source.dgO = cell.dgini;
    source.dgG = cell.dgini;
    source.yco2O = cell.yco2ini;
    source.yco2G = cell.yco2ini;
    cell.viscLini = state.cells[i].flui.VisOM(temperatureLow);
    cell.viscHini = state.cells[i].flui.VisOM(temperatureHigh);
    source.viscL = cell.viscLini;
    source.viscH = cell.viscHini;
    source.rholST = cell.rholST;
    source.rhogST = cell.rhogST;
    source.razdgl = 1.;
    source.stockTankQuality = 0.;
}

void blackOilSourceRecirculation(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                 int i, double temperatureHigh, double temperatureLow) {
    const auto &connection = state.cells[state.cells[i].acsr.indJusRecic].acsr.fontechk;
    if (connection.presT > connection.pamb)
        fluF = connection.fluidoP;
    else
        fluF = connection.fluidoPamb;

            source.stockTankQuality = fluF.dStockTankVaporMassFraction;
            source.dgO = fluF.Deng;
            source.yco2O = fluF.yco2;
            double sourceGasOilRatio = fluF.RGO;

            double rsF = fluF.RS(state.cells[i].pres, state.cells[i].temp) * (6.29 / 35.31467);
            double dissolvedGasDensityRatio = 1 / fluF.rDgD;
            source.razdgl = 1 / fluF.rDgL;
            if (fluF.BSW < 1 - (*state.globals).smallValueTolerance)
                source.rholST = (1 - fluF.BSW) * (1000 * 141.5 / (131.5 + fluF.API)) + fluF.BSW * 1000 * fluF.Denag + fluF.Deng * 1.225 * sourceGasOilRatio * (1. - fluF.BSW);
            else
                source.rholST = fluF.BSW * 1000 * fluF.Denag;
            source.dissolvedGas = (source.dissolvedGas + source.freeGas) * dissolvedGasDensityRatio * (fluF.RS(state.cells[i].pres, state.cells[i].temp) * (6.29 / 35.31467) * (1. - fluF.BSW) / source.rholST);

            if (state.input.nfluP > 1 || (*state.globals).networkMode != 0) {
                double bswaux = fluF.BSW;
                double contrabsw = 1. - bswaux;
                if (contrabsw < (*state.globals).smallValueTolerance)
                    contrabsw = 0.9 * (*state.globals).smallValueTolerance;
                double rhoPSTF;
                if (contrabsw > (*state.globals).smallValueTolerance)
                    rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag + fluF.Deng * 1.225 * rsF;
                else
                    rhoPSTF = 1000 * fluF.Denag;
                if (contrabsw > (*state.globals).smallValueTolerance) {
                    source.deadOil *= (1. / rhoPSTF);
                    source.water *= ((bswaux / contrabsw) / rhoPSTF);
                } else {
                    source.deadOil = 0.;
                    source.water *= (1 / (fluF.BSW * 1000 * fluF.Denag));
                }
                source.API = fluF.API;
                source.BSW = fluF.BSW;
                source.denag = fluF.Denag;
                source.viscL = 0 * 30 + 1 * fluF.VisOM(temperatureLow);
                source.viscH = 0 * 20 + 1 * fluF.VisOM(temperatureHigh);
            }
        
}

void prepareBlackOilAccessorySource(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                    int i, double temperatureHigh, double temperatureLow) {
    if (state.cells[i].acsr.tipo == kAccessoryGasInjection && state.cells[i].acsr.injg.seco == 1) {
        if (state.cells[i].acsr.injg.QGas > 0.)
            fluF = state.cells[i].acsr.injg.FluidoPro;
        else
            fluF = state.cells[i].flui;
        source.dgG = fluF.Deng;
        source.yco2G = fluF.yco2;
        source.rhogST = fluF.Deng * kAirDensityAtStandardConditions;
    } else if (state.cells[i].acsr.tipo == kAccessoryGasInjection && state.cells[i].acsr.injg.seco == 0) {
        blackOilSourceWetGasInjection(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryLiquidInjection) {
        blackOilSourceLiquidInjection(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryMultipleSource) {
        blackOilSourceMultipleSource(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryInflowPerformance) {
        blackOilSourceInflowPerformance(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryLeak) {
        blackOilSourceLeak(state, source, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryRadialPorous) {
        blackOilSourceRadialPorous(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryPorous2D) {
        blackOilSourcePorous2D(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if (state.cells[i].acsr.tipo == kAccessoryRecirculationUpstream ||
               state.cells[i].acsr.tipo == kAccessoryRecirculationDownstream) {
        blackOilSourceRecirculation(state, source, fluF, i, temperatureHigh, temperatureLow);
    } else if ((fabs(source.dissolvedGas) > (*state.globals).smallValueTolerance &&
                state.cells[i].acsr.tipo != kAccessoryLiquidInjection &&
                state.cells[i].acsr.tipo != kAccessoryInflowPerformance &&
                state.cells[i].acsr.tipo != kAccessoryLeak &&
                state.cells[i].acsr.tipo != kAccessoryRadialPorous &&
                state.cells[i].acsr.tipo != kAccessoryPorous2D) ||
               (fabs(source.freeGas) > (*state.globals).smallValueTolerance &&
                state.cells[i].acsr.tipo != kAccessoryGasInjection &&
                state.cells[i].acsr.tipo != kAccessoryLiquidInjection &&
                state.cells[i].acsr.tipo != kAccessoryInflowPerformance &&
                state.cells[i].acsr.tipo != kAccessoryLeak &&
                state.cells[i].acsr.tipo != kAccessoryRadialPorous &&
                state.cells[i].acsr.tipo != kAccessoryPorous2D)) {
        blackOilSourceCellFluid(state, source, i, temperatureHigh, temperatureLow);
    }
    source.freeGas *= (source.razdgl / (source.rhogST));

    if (source.stockTankQuality > 1. - 1e-15) {
        source.dissolvedGas = 0.;
        source.deadOil = 0.;
        source.water = 0.;
    }
}

void prepareBlackOilStockTankBalances(const FluidTrackingState &state, const BlackOilFace &left,
                                      const BlackOilFace &right, const BlackOilCell &cell,
                                      const BlackOilSource &source, BlackOilBalance &balance, double dx,
                                      double flowArea, int i, double dt) {
    balance.MultOe = 0.;
    if (state.cells[i].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        balance.MultOe = state.cells[i].QL * (1 - left.betI) * (1 - left.bsw) * left.razdgd *
                         left.solutionGasRatio / left.oilVolumeFactor;
    balance.MultOd = 0.;
    if (state.cells[i + 1].flui.dStockTankVaporMassFraction < 1. - 1e-15)
        balance.MultOd = state.cells[i + 1].QL * (1 - right.betI) * (1 - right.bsw) * right.razdgd *
                         right.solutionGasRatio / right.oilVolumeFactor;
    balance.MultGe = (state.cells[i].MC - state.cells[i].Mliqini) * left.razdgl / (left.rhogST);
    balance.MultGd = (state.cells[i + 1].MC - state.cells[i + 1].Mliqini) * right.razdgl / (right.rhogST);
    balance.volleveFim = (((1 - cell.liquidHoldup) * cell.rhog * cell.razdgl / (cell.rhogST)) +
                          cell.liquidHoldup * (1 - cell.complementaryFraction) * (1. - cell.bsw) *
                              cell.solutionGasRatio * cell.razdgd / (cell.oilVolumeFactor));
    if (balance.volleveFim < 1e-15)
        balance.volleveFim = 0.;
    balance.residuo = (balance.volleveFim - state.cells[i].VolLeveST) * flowArea / dt +
                      (balance.MultOd - balance.MultOe) / dx + (balance.MultGd - balance.MultGe) / dx -
                      (source.dissolvedGas / dx + source.freeGas / dx);
    balance.volpesFim =
        cell.liquidHoldup * (1 - cell.complementaryFraction) * (1 - cell.bsw) / cell.oilVolumeFactor;
    balance.volaguaFim = cell.liquidHoldup * (1 - cell.complementaryFraction) * cell.bsw;
    balance.MultPe = 0.;
    balance.MultPd = 0.;
    balance.residuoP = 0.;
    balance.MultAe = 0.;
    balance.MultAd = 0.;
}

void publishBlackOilCellProperties(const FluidTrackingState &state, int i, Vcr<double> &rgo, Vcr<double> &dg,
                                   Vcr<double> &yco2, Vcr<double> &API, Vcr<double> &BSW, Vcr<double> &denag,
                                   Vcr<double> &VISCL, Vcr<double> &VISCH, double temperatureHigh,
                                   double temperatureLow) {
    if (state.trackGasOilRatio > 0 && state.cells[i].flui.corrSat != 4)
        state.cells[i].flui.RGO = rgo[i];
    if (state.trackGasGravity > 0 && state.input.flashCompleto == 0) {
        state.cells[i].flui.Deng = dg[i];
        if ((state.cells[i].flui.Deng > 5 || state.cells[i].flui.Deng < 0) && i > 0)
            state.cells[i].flui.Deng = state.cells[i - 1].flui.Deng;
        state.cells[i].flui.yco2 = yco2[i];
    }
    if (hasMultipleFluidsOrNetwork(state)) {
        state.cells[i].flui.BSW = BSW[i];
        state.cells[i].flui.Denag = denag[i];
        if (state.input.flashCompleto == 0) {
            state.cells[i].flui.API = API[i];
            state.cells[i].flui.LVisL = VISCL[i];
            state.cells[i].flui.LVisH = VISCH[i];
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
        }
    }
    if (state.input.flashCompleto == 0)
        state.cells[i].flui.RenovaFluido();
    correctGasSpecificGravity(state.cells, state.input, state.steadyIteration, i);
}

void updateBlackOilOutletProperties(const FluidTrackingState &state, const ProFlu &fluiRev, Vcr<double> &rgo,
                                    Vcr<double> &dg, Vcr<double> &yco2, Vcr<double> &API, Vcr<double> &BSW,
                                    Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                    double temperatureHigh, double temperatureLow) {
    if ((*state.globals).networkMode == 0 || state.endNode == 1 ||
        state.cells[state.lastCell].Mliqini > -(*state.globals).smallValueTolerance) {
        if (state.trackGasOilRatio > 0 && state.cells[state.lastCell].flui.corrSat != 4)
            state.cells[state.lastCell].flui.RGO = rgo[state.lastCell - 1];
        if (state.trackGasGravity > 0 && state.input.flashCompleto == 0) {
            state.cells[state.lastCell].flui.Deng = dg[state.lastCell - 1];
            state.cells[state.lastCell].flui.yco2 = yco2[state.lastCell - 1];
        }
        if (hasMultipleFluidsOrNetwork(state)) {
            state.cells[state.lastCell].flui.BSW = BSW[state.lastCell - 1];
            state.cells[state.lastCell].flui.Denag = denag[state.lastCell - 1];
            if (state.input.flashCompleto == 0) {
                state.cells[state.lastCell].flui.API = API[state.lastCell - 1];
                state.cells[state.lastCell].flui.LVisL = VISCL[state.lastCell - 1];
                state.cells[state.lastCell].flui.LVisH = VISCH[state.lastCell - 1];
                state.cells[state.lastCell].flui.TempL = temperatureLow;
                state.cells[state.lastCell].flui.TempH = temperatureHigh;
            }
        }
    } else {
        state.cells[state.lastCell].flui.RGO = fluiRev.RGO;
        state.cells[state.lastCell].flui.Deng = fluiRev.Deng;
        state.cells[state.lastCell].flui.yco2 = fluiRev.yco2;
        state.cells[state.lastCell].flui.API = fluiRev.API;
        state.cells[state.lastCell].flui.BSW = fluiRev.BSW;
        state.cells[state.lastCell].flui.Denag = fluiRev.Denag;
        state.cells[state.lastCell].flui.LVisL = fluiRev.LVisL;
        state.cells[state.lastCell].flui.LVisH = fluiRev.LVisH;
        state.cells[state.lastCell].flui.TempL = fluiRev.TempL;
        state.cells[state.lastCell].flui.TempH = fluiRev.TempH;
    }
    if (state.input.flashCompleto == 0)
        state.cells[state.lastCell].flui.RenovaFluido();
    if (state.input.corrDeng == 0) {
        state.cells[state.lastCell].flui.rDgD = 1.;
        state.cells[state.lastCell].flui.rDgL = 1.;
        state.cells[state.lastCell].flui.PCis = state.cells[state.lastCell].flui.PC;
        state.cells[state.lastCell].flui.TCis = state.cells[state.lastCell].flui.TC;
    } else {
        state.cells[state.lastCell].flui.razDegD(state.cells[state.lastCell].pres,
                                                 state.cells[state.lastCell].temp);
        state.cells[state.lastCell].flui.rzDegL(state.cells[state.lastCell].pres,
                                                state.cells[state.lastCell].temp);
        state.cells[state.lastCell].flui.PcTcIS();
    }
}

void publishPhaseCellProperties(const FluidTrackingState &state, ProFlu *fluC, int i, Vcr<double> &BSW,
                                Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                double temperatureHigh, double temperatureLow) {
    if (hasProductionFluidOrNetwork(state)) {
        state.cells[i].flui = fluC[i];
        if (hasMultipleFluidsOrNetwork(state)) {
            state.cells[i].flui.BSW = BSW[i];
            state.cells[i].flui.Denag = denag[i];
            state.cells[i].flui.LVisL = VISCL[i];
            state.cells[i].flui.LVisH = VISCH[i];
            state.cells[i].flui.TempL = temperatureLow;
            state.cells[i].flui.TempH = temperatureHigh;
        }
    }
}

void updatePhaseOutletProperties(const FluidTrackingState &state, const ProFlu &fluiRev, Vcr<double> &BSW,
                                 Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                 double temperatureHigh, double temperatureLow) {
    if ((*state.globals).networkMode == 0 || state.endNode == 1 ||
        state.cells[state.lastCell].Mliqini > -(*state.globals).smallValueTolerance) {
        if (hasProductionFluidOrNetwork(state)) {
            state.cells[state.lastCell].flui = state.cells[state.lastCell - 1].flui;
            if (hasMultipleFluidsOrNetwork(state)) {
                state.cells[state.lastCell].flui.BSW = BSW[state.lastCell - 1];
                state.cells[state.lastCell].flui.Denag = denag[state.lastCell - 1];
                state.cells[state.lastCell].flui.LVisL = VISCL[state.lastCell - 1];
                state.cells[state.lastCell].flui.LVisH = VISCH[state.lastCell - 1];
                state.cells[state.lastCell].flui.TempL = temperatureLow;
                state.cells[state.lastCell].flui.TempH = temperatureHigh;
            }
        }
    } else {
        state.cells[state.lastCell].flui = fluiRev;
    }
}

void refreshPhaseSourceComposition(const FluidTrackingState &state, ProFlu &fluid, const double &pressure,
                                   const double &temperature) {
    if (fluid.dCalculatedBeta < 0. || fluid.dCalculatedBeta > 1.)
        fluid.atualizaPropComp(pressure, temperature, -1, NULL, NULL, state.input.pocinjec);
    else
        fluid.atualizaPropComp(pressure, temperature, fluid.dCalculatedBeta, fluid.oCalculatedLiqComposition,
                               fluid.oCalculatedVapComposition, state.input.pocinjec);
}

void readPhaseSourceLiquidQuality(const FluidTrackingState &state, PhaseSource &source, const ProFlu &fluF,
                                  int i) {
    double sourceOilVolumeFactor = fluF.BOFunc(state.cells[i].pres, state.cells[i].temp);
    double sourceWaterVolumeFactor = fluF.BAFunc(state.cells[i].pres, state.cells[i].temp);
    double sourceWaterCut =
        fluF.BSW * sourceWaterVolumeFactor /
        (sourceOilVolumeFactor + sourceWaterVolumeFactor * fluF.BSW - fluF.BSW * sourceOilVolumeFactor);
    double sourceOilDensity = fluF.MasEspoleo(state.cells[i].pres, state.cells[i].temp);
    double sourceWaterDensity = fluF.MasEspAgua(state.cells[i].pres, state.cells[i].temp);
    source.tit = (1 - sourceWaterCut) * sourceOilDensity /
                 ((1 - sourceWaterCut) * sourceOilDensity + sourceWaterCut * sourceWaterDensity);
}

void preparePhaseFacesAndCellWeight(const FluidTrackingState &state, PhaseFace &left, PhaseFace &right,
                                    PhaseCell &cell, int i, double temperatureHigh, double temperatureLow,
                                    int ncomp) {
    upwindLeftFacePhaseLiquidProperties(state, left, i, temperatureHigh, temperatureLow);
    if (left.oilVolumeFactor < 1e-15)
        left.oilVolumeFactor = 1e-15;

    right.betI = state.cells[i].betPigD;
    preparePhaseLiquidFace(right, state.cells[i].flui, state.cells[i].pres, state.cells[i].temp,
                           temperatureHigh, temperatureLow);

    if (state.cells[i + 1].QL < 0.) {
        right.betI = state.cells[i + 1].betPigE;
        preparePhaseLiquidFace(right, state.cells[i + 1].flui, state.cells[i + 1].pres,
                               state.cells[i + 1].temp, temperatureHigh, temperatureLow);
    }
    if (right.oilVolumeFactor < 1e-15)
        right.oilVolumeFactor = 1e-15;
    left.vazMasLiq = state.cells[i].Mliqini;
    right.vazMasLiq = state.cells[i + 1].Mliqini;
    left.vazMasGas = state.cells[i].MC - state.cells[i].Mliqini;
    right.vazMasGas = state.cells[i + 1].MC - state.cells[i + 1].Mliqini;
    cell.pesoMol = 0.;
    for (int kfrac = 0; kfrac < ncomp; kfrac++) {
        cell.pesoMol += state.cells[i].flui.masMol[kfrac] * state.cells[i].flui.fracMol[kfrac];
    }
}

void preparePhaseMixtureSource(const FluidTrackingState &state, PhaseSource &source, ProFlu &fluF, int i,
                               int ncomp) {
    source.dissolvedGas = state.cells[i].fontemassLR;
    source.freeGas = state.cells[i].fontemassGR;
    source.deadOil = state.cells[i].fontemassLR;
    source.water = state.cells[i].fontemassLR;

    readPhaseSourceFluid(state, source, fluF, i);

    source.pesoMol = 0;
    for (int j = 0; j < ncomp; j++) {
        source.pesoMol += fluF.masMol[j] * fluF.fracMol[j];
    }
    source.dissolvedGas *= source.tit;
}

void updatePhaseMoleInventory(const FluidTrackingState &state, const PhaseCell &cell, ProFlu *fluC, int i) {
    PhaseTransported transported;
    transported.betIV = state.cells[i].bet;
    transported.solutionGasRatio = state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp);
    transported.oilVolumeFactor =
        state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp, transported.solutionGasRatio);
    transported.waterVolumeFactor = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);
    transported.bswV =
        state.cells[i].flui.BSW * transported.waterVolumeFactor /
        (transported.oilVolumeFactor + transported.waterVolumeFactor * state.cells[i].flui.BSW -
         state.cells[i].flui.BSW * transported.oilVolumeFactor);
    transported.rhoOVol = state.cells[i].flui.MasEspoleo(state.cells[i].pres, state.cells[i].temp);
    transported.rhoWVol = state.cells[i].flui.MasEspAgua(state.cells[i].pres, state.cells[i].temp);
    transported.titVol =
        (1 - transported.bswV) * transported.rhoOVol /
        ((1 - transported.bswV) * transported.rhoOVol + transported.bswV * transported.rhoWVol);
    state.cells[i].nMol = (fluC[i].MasEspLiq(state.cells[i].pres, state.cells[i].temp) *
                               (1. - state.cells[i].alf) * (1. - transported.betIV) * transported.titVol +
                           fluC[i].MasEspGas(state.cells[i].pres, state.cells[i].temp) * state.cells[i].alf) *
                          state.cells[i].duto.area * state.cells[i].dx / cell.pesoMol;
    fluC[i].Pmol = cell.pesoMol;
}

void transportPhaseLiquidProperties(const FluidTrackingState &state, const PhaseFace &left,
                                    const PhaseFace &right, PhaseCell &cell, PhaseSource &source,
                                    ProFlu &fluF, int i, Vcr<double> &BSW, Vcr<double> &denag,
                                    Vcr<double> &VISCL, Vcr<double> &VISCH, double temperatureHigh,
                                    double temperatureLow, double dx, double flowArea, double dt) {
    cell.liquidHoldup = 1. - state.cells[i].alf;
    cell.complementaryFraction = state.cells[i].bet;

    cell.solutionGasRatio = state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp);
    cell.oilVolumeFactor =
        state.cells[i].flui.BOFunc(state.cells[i].pres, state.cells[i].temp, cell.solutionGasRatio);
    cell.waterVolumeFactor = state.cells[i].flui.BAFunc(state.cells[i].pres, state.cells[i].temp);
    cell.bsw = state.cells[i].flui.BSW * cell.waterVolumeFactor /
               (cell.oilVolumeFactor + cell.waterVolumeFactor * state.cells[i].flui.BSW -
                state.cells[i].flui.BSW * cell.oilVolumeFactor);

    cell.BSWini = state.cells[i].flui.BSW;
    source.BSW = state.cells[i].flui.BSW;
    cell.denagini = state.cells[i].flui.Denag;
    source.denag = state.cells[i].flui.Denag;
    cell.viscLini = state.cells[i].flui.VisOM(temperatureLow);
    cell.viscHini = state.cells[i].flui.VisOM(temperatureHigh);
    source.viscL = cell.viscLini;
    source.viscH = cell.viscHini;

    phaseSourceStandardRatesByAccessory(state, source, fluF, i, temperatureHigh, temperatureLow);
    if (source.tit > 1. - 1e-15) {
        source.dissolvedGas = 0.;
        source.deadOil = 0.;
        source.water = 0.;
    }

    PhaseBalance balance;
    balance.volpesFim =
        cell.liquidHoldup * (1 - cell.complementaryFraction) * (1 - cell.bsw) / cell.oilVolumeFactor;
    balance.volaguaFim =
        cell.liquidHoldup * (1 - cell.complementaryFraction) * cell.bsw / cell.oilVolumeFactor;
    balance.residuoP = 0.;
    balance.residuoA = 0.;
    transportPhaseVolumesAndViscosities(state, left, right, cell, source, balance, dx, flowArea, i, BSW,
                                        denag, VISCL, VISCH, dt);
    state.cells[i].VolPesaST = balance.volpesFim - 0. * balance.residuoP * dt / flowArea;
    state.cells[i].VolAguaST = balance.volaguaFim - 0. * balance.residuoA * dt / flowArea;
}

void prepareLiquidFaceTransport(PhaseFace &face, const ProFlu &fluid, const ProFluCol &complementaryFluid,
                                const double &pressure, const double &temperature,
                                const double &liquidVolumeFlow, const double &voidFraction,
                                Vcr<double> &fractions, int ncomp) {
    double waterCutCarried = face.bsw;
    double rhoOV = fluid.MasEspoleo(pressure, temperature);
    double rhoWV = fluid.MasEspAgua(pressure, temperature);
    double titLocal = fluid.FracMass(pressure, temperature);
    face.titV = (1 - waterCutCarried) * rhoOV / ((1 - waterCutCarried) * rhoOV + waterCutCarried * rhoWV);
    face.vazMasLiq -= face.betI * liquidVolumeFlow * complementaryFluid.MasEspFlu(pressure, temperature);
    face.vazMasLiq *= face.titV;
    face.pesoMolO = 0;
    vector<double> fracMolFase(ncomp);
    normalizePhaseFractions(fracMolFase, fluid.oCalculatedLiqComposition, ncomp);
    for (int kfrac = 0; kfrac < ncomp; kfrac++) {
        fractions[kfrac] = fracMolFase[kfrac];
        face.pesoMolO += fluid.masMol[kfrac] * fracMolFase[kfrac];
    }
    if (face.pesoMolO < 1e-3 || titLocal > 1. - 1e-3 || voidFraction > 1. - 1e-3) {
        face.pesoMolO = 0;
        for (int kfrac = 0; kfrac < ncomp; kfrac++) {
            fractions[kfrac] = fluid.fracMol[kfrac];
            face.pesoMolO += fluid.masMol[kfrac] * fluid.fracMol[kfrac];
        }
    }
}

void prepareVapourFaceComposition(PhaseFace &face, const ProFlu &fluid, const double &pressure,
                                  const double &temperature, const double &voidFraction,
                                  Vcr<double> &fractions, int ncomp) {
    face.pesoMolG = 0;
    double titLocal = fluid.FracMass(pressure, temperature);
    vector<double> fracMolFase(ncomp);
    normalizePhaseFractions(fracMolFase, fluid.oCalculatedVapComposition, ncomp);
    for (int kfrac = 0; kfrac < ncomp; kfrac++) {
        fractions[kfrac] = fracMolFase[kfrac];
        face.pesoMolG += fluid.masMol[kfrac] * fracMolFase[kfrac];
    }
    if (face.pesoMolG < 1e-3 || titLocal < 1e-3 || voidFraction < 1e-3) {
        face.pesoMolG = 0;
        for (int kfrac = 0; kfrac < ncomp; kfrac++) {
            fractions[kfrac] = fluid.fracMol[kfrac];
            face.pesoMolG += fluid.masMol[kfrac] * fluid.fracMol[kfrac];
        }
    }
}

void copyMixtureFaceComposition(PhaseFace &face, const ProFlu &fluid, Vcr<double> &fractions, int ncomp) {
    face.pesoMol = 0;
    for (int kfrac = 0; kfrac < ncomp; kfrac++) {
        fractions[kfrac] = fluid.fracMol[kfrac];
        face.pesoMol += fluid.masMol[kfrac] * fluid.fracMol[kfrac];
    }
}

} // namespace

void transportBlackOilProperties(const FluidTrackingState &state, ProFlu fluiRev) {
    Vcr<double> rgo(state.lastCell);
    Vcr<double> dg(state.lastCell);
    Vcr<double> yco2(state.lastCell);
    Vcr<double> API(state.lastCell);
    Vcr<double> BSW(state.lastCell);
    Vcr<double> denag(state.lastCell);
    Vcr<double> VISCL(state.lastCell);
    Vcr<double> VISCH(state.lastCell);
    double dt = state.cells[1].dt;
    double temperatureLow;
    if (state.input.flashCompleto == 1 && (state.input.tabent.tmin - 0) > (*state.globals).smallValueTolerance)
        temperatureLow = state.input.tabent.tmin + 0.1;
    else
        temperatureLow = 0.;
    double temperatureHigh;
    if (state.input.flashCompleto == 1 && (70 - state.input.tabent.tmax) > (*state.globals).smallValueTolerance)
        temperatureHigh = state.input.tabent.tmax - 0.1;
    else
        temperatureHigh = 70.;
    int imin = 1;
    if ((*state.globals).networkMode != 0 &&
        (state.cells[0].acsr.tipo == kAccessoryMultipleSource || state.input.ConContEntrada > 0))
        imin = 0;
    if (state.cells[0].acsr.tipo == kAccessoryRadialPorous &&
        (state.cells[0].fontemassCR + state.cells[0].fontemassGR + state.cells[0].fontemassLR) > 0.)
        state.cells[0].flui.BSW = state.cells[0].acsr.radialPoro.BSW;
    else if (state.cells[0].acsr.tipo == kAccessoryPorous2D &&
             (state.cells[0].fontemassCR + state.cells[0].fontemassGR + state.cells[0].fontemassLR) > 0.)
        state.cells[0].flui.BSW = state.cells[0].acsr.poroso2D.dados.transfer.BSW;
#pragma omp parallel for num_threads((*state.globals).solverThreadCount)
    for (int i = imin; i < state.lastCell; i++) {
        transportCellBlackOilProperties(state, i, rgo, dg, yco2, API, BSW, denag, VISCL, VISCH,
                                        temperatureHigh, temperatureLow, dt);
    }
    for (int i = imin; i <= state.lastCell - 1; i++) {
        publishBlackOilCellProperties(state, i, rgo, dg, yco2, API, BSW, denag, VISCL, VISCH, temperatureHigh,
                                      temperatureLow);
    }

    updateBlackOilOutletProperties(state, fluiRev, rgo, dg, yco2, API, BSW, denag, VISCL, VISCH,
                                   temperatureHigh, temperatureLow);
}

namespace {

void normalizePhaseFractions(std::vector<double> &fracMolFase, double *fracMolOriginal, int npseudo) {
    for (int kfrac = 0; kfrac < npseudo; kfrac++) {
        fracMolFase[kfrac] = fracMolOriginal[kfrac];
    }
    // Find the smallest value
    double menorFracFase = fracMolFase[0];
    for (int kfrac = 1; kfrac < npseudo; kfrac++) {
        if (menorFracFase > fracMolFase[kfrac]) {
            menorFracFase = fracMolFase[kfrac];
        }
    }
    menorFracFase = 0.;

    // Subtract the smallest value
    for (int kfrac = 0; kfrac < npseudo; kfrac++) {
        fracMolFase[kfrac] -= menorFracFase;
    }

    // Compute the total sum
    double fracTotFase = 0.;
    for (int kfrac = 0; kfrac < npseudo; kfrac++) {
        fracTotFase += fracMolFase[kfrac];
    }

    // Normalise
    if (fracTotFase > 0) {
        for (int kfrac = 0; kfrac < npseudo; kfrac++) {
            fracMolFase[kfrac] /= fracTotFase;
        }
    }
}

void transportPhaseVolumesAndViscosities(const FluidTrackingState &state, const PhaseFace &left,
                                         const PhaseFace &right, const PhaseCell &cell,
                                         const PhaseSource &source, PhaseBalance &balance, double dx,
                                         double flowArea, int i, Vcr<double> &BSW, Vcr<double> &denag,
                                         Vcr<double> &VISCL, Vcr<double> &VISCH, double dt) {
    computePhaseLiquidVolumeBalances(state, left, right, source, balance, dx, flowArea, i, dt);
    if ((state.input.nfluP > 1) || (*state.globals).networkMode != 0) {
        if (((balance.volpesFim > 1e-3) &&
             (source.deadOil > 0 || (balance.MultPd < 0 || balance.MultPe > 0))) &&
            state.input.nfluP > 0) {
            VISCL[i] = (dt * (source.viscL * source.deadOil / dx + 1. * cell.viscLini * balance.residuoP -
                              (right.viscL * balance.MultPd - left.viscL * balance.MultPe) / dx) +
                        cell.viscLini * state.cells[i].VolPesaST * flowArea) /
                       (balance.volpesFim * flowArea - 0. * balance.residuoP * dt);
            VISCH[i] = (dt * (source.viscH * source.deadOil / dx + 1. * cell.viscHini * balance.residuoP -
                              (right.viscH * balance.MultPd - left.viscH * balance.MultPe) / dx) +
                        cell.viscHini * state.cells[i].VolPesaST * flowArea) /
                       (balance.volpesFim * flowArea - 0. * balance.residuoP * dt);
        } else {
            VISCL[i] = cell.viscLini;
            VISCH[i] = cell.viscHini;
        }
    }
    if (hasMultipleFluidsOrNetwork(state)) {
        if (((balance.volaguaFim + balance.volpesFim) > 1e-3) &&
            ((source.deadOil > 0 || source.water > 0) ||
             ((balance.MultPd < 0 || balance.MultPe > 0) || (balance.MultAd < 0 || balance.MultAe > 0)))) {
            BSW[i] = (dt * (source.BSW * (source.water + source.deadOil) / dx +
                            1. * cell.BSWini * (balance.residuoA + balance.residuoP) -
                            (right.BSW * (balance.MultAd + balance.MultPd) -
                             left.BSW * (balance.MultAe + balance.MultPe)) /
                                dx) +
                      cell.BSWini * (state.cells[i].VolAguaST + state.cells[i].VolPesaST) * flowArea) /
                     ((balance.volaguaFim + balance.volpesFim) * flowArea -
                      0. * dt * (balance.residuoA + balance.residuoP));
            denag[i] = (dt * (source.denag * (source.water) / dx + 1. * cell.denagini * (balance.residuoA) -
                              (right.denag * (balance.MultAd) - left.denag * (balance.MultAe)) / dx) +
                        cell.denagini * (state.cells[i].VolAguaST) * flowArea) /
                       ((balance.volaguaFim) * flowArea - 0. * dt * (balance.residuoA));
            if (BSW[i] < 0.)
                BSW[i] = 0.;
            else if (BSW[i] > 1.)
                BSW[i] = 1.;
            else if (isnan(BSW[i]))
                BSW[i] = cell.BSWini;
            if (denag[i] < 1.)
                denag[i] = 1.;
            else if (isnan(denag[i]))
                denag[i] = cell.denagini;
        } else
            BSW[i] = cell.BSWini;
        denag[i] = cell.denagini;
    }
}

void phaseSourceStandardRatesByAccessory(const FluidTrackingState &state, PhaseSource &source, ProFlu &fluF,
                                         int i, double temperatureHigh, double temperatureLow) {
    if (state.cells[i].acsr.tipo == kAccessoryLiquidInjection) {
        double solutionGasRatioSource =
            state.cells[i].acsr.injl.FluidoPro.RS(state.cells[i].pres, state.cells[i].temp) *
            (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        if (hasProductionFluidOrNetwork(state)) {
            convertPhaseInjectionRates(state, source, state.cells[i].acsr.injl.FluidoPro,
                                       solutionGasRatioSource, temperatureHigh, temperatureLow);
        }
    } else if (state.cells[i].acsr.tipo == kAccessoryMultipleSource) {
        double solutionGasRatioSource =
            state.cells[i].acsr.injm.FluidoPro.RS(state.cells[i].pres, state.cells[i].temp) *
            (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        if (hasProductionFluidOrNetwork(state)) {
            convertPhaseInjectionRates(state, source, state.cells[i].acsr.injm.FluidoPro,
                                       solutionGasRatioSource, temperatureHigh, temperatureLow);
        }
    } else if (state.cells[i].acsr.tipo == kAccessoryInflowPerformance) {
        double solutionGasRatioSource =
            state.cells[i].acsr.ipr.FluidoPro.RS(state.cells[i].pres, state.cells[i].temp) *
            (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        if (hasProductionFluidOrNetwork(state)) {
            convertPhasePorousRates(state, source, state.cells[i].acsr.ipr.FluidoPro,
                                    state.cells[i].acsr.ipr.FluidoPro.BSW, solutionGasRatioSource,
                                    temperatureHigh, temperatureLow);
        }
    } else if (state.cells[i].acsr.tipo == kAccessoryLeak) {

        double solutionGasRatioSource = fluF.RS(state.cells[i].pres, state.cells[i].temp) *
                                        (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        if (hasProductionFluidOrNetwork(state)) {
            double bswaux = fluF.BSW;
            double contrabsw = 1. - bswaux;
            if (contrabsw <= (*state.globals).smallValueTolerance)
                contrabsw = 0.9 * (*state.globals).smallValueTolerance;
            double rhoPSTF;
            if (contrabsw > (*state.globals).smallValueTolerance)
                rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag +
                          fluF.Deng * kAirDensityAtStandardConditions * solutionGasRatioSource;
            else
                rhoPSTF = 1000 * fluF.Denag;
            if (contrabsw > (*state.globals).smallValueTolerance) {
                source.deadOil *= (1. / rhoPSTF);
                source.water *= ((bswaux / contrabsw) / rhoPSTF);
            } else {
                source.deadOil = 0.;
                source.water *= (1 / (fluF.BSW * 1000 * fluF.Denag));
            }
            source.BSW = fluF.BSW;
            source.denag = fluF.Denag;
            source.viscL = fluF.VisOM(temperatureLow);
            source.viscH = fluF.VisOM(temperatureHigh);
        }
    } else if (state.cells[i].acsr.tipo == kAccessoryRadialPorous) {
        double solutionGasRatioSource =
            state.cells[i].acsr.radialPoro.flup.RS(state.cells[i].pres, state.cells[i].temp) *
            (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        if (hasProductionFluidOrNetwork(state)) {
            convertPhasePorousRates(state, source, state.cells[i].acsr.radialPoro.flup,
                                    state.cells[i].acsr.radialPoro.BSW, solutionGasRatioSource,
                                    temperatureHigh, temperatureLow);
        }
    } else if (state.cells[i].acsr.tipo == kAccessoryPorous2D) {
        double solutionGasRatioSource =
            state.cells[i].acsr.poroso2D.dados.flup.RS(state.cells[i].pres, state.cells[i].temp) *
            (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        if (hasProductionFluidOrNetwork(state)) {
            convertPhasePorousRates(state, source, state.cells[i].acsr.poroso2D.dados.flup,
                                    state.cells[i].acsr.poroso2D.dados.transfer.BSW, solutionGasRatioSource,
                                    temperatureHigh, temperatureLow);
        }
    } else if (state.cells[i].acsr.tipo == kAccessoryRecirculationUpstream ||
               state.cells[i].acsr.tipo == kAccessoryRecirculationDownstream) {

            double rsF = fluF.RS(state.cells[i].pres, state.cells[i].temp) * (6.29 / 35.31467);
            if (state.input.nfluP > 0 || (*state.globals).networkMode != 0) {
                double bswaux = fluF.BSW;
                double contrabsw = 1. - bswaux;
                if (contrabsw <= (*state.globals).smallValueTolerance)
                    contrabsw = 0.9 * (*state.globals).smallValueTolerance;
                double rhoPSTF;
                if (contrabsw > (*state.globals).smallValueTolerance)
                    rhoPSTF = (1000 * 141.5 / (131.5 + fluF.API)) + (bswaux / contrabsw) * 1000 * fluF.Denag + fluF.Deng * 1.225 * rsF;
                else
                    rhoPSTF = 1000 * fluF.Denag;
                if (contrabsw > (*state.globals).smallValueTolerance) {
                    source.deadOil *= (1. / rhoPSTF);
                    source.water *= ((bswaux / contrabsw) / rhoPSTF);
                } else {
                    source.deadOil = 0.;
                    source.water *= (1 / (fluF.BSW * 1000 * fluF.Denag));
                }
                source.BSW = fluF.BSW;
                source.denag = fluF.Denag;
                source.viscL = 0 * 30 + 1 * fluF.VisOM(temperatureLow);
                source.viscH = 0 * 20 + 1 * fluF.VisOM(temperatureHigh);
            }
        
    } else if ((fabs(source.dissolvedGas) > (*state.globals).smallValueTolerance &&
                state.cells[i].acsr.tipo != kAccessoryLiquidInjection &&
                state.cells[i].acsr.tipo != kAccessoryInflowPerformance &&
                state.cells[i].acsr.tipo != kAccessoryLeak &&
                state.cells[i].acsr.tipo != kAccessoryRadialPorous &&
                state.cells[i].acsr.tipo != kAccessoryPorous2D) ||
               (fabs(source.freeGas) > (*state.globals).smallValueTolerance &&
                state.cells[i].acsr.tipo != kAccessoryGasInjection &&
                state.cells[i].acsr.tipo != kAccessoryLiquidInjection &&
                state.cells[i].acsr.tipo != kAccessoryInflowPerformance &&
                state.cells[i].acsr.tipo != kAccessoryLeak &&
                state.cells[i].acsr.tipo != kAccessoryRadialPorous &&
                state.cells[i].acsr.tipo != kAccessoryPorous2D)) {
        if (state.cells[i].acsr.tipo == kAccessoryChoke ||
            state.cells[i].acsr.tipo == kAccessoryVolumetricPump) {
            (void)(state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                   (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre));
            if (hasProductionFluidOrNetwork(state)) {
                double rhoPSTF =
                    (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                    state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
                source.deadOil *= ((1 - state.cells[i].flui.BSW) / rhoPSTF);
                source.water *= (state.cells[i].flui.BSW / rhoPSTF);
                source.BSW = state.cells[i].flui.BSW;
                source.denag = fluF.Denag;
                source.viscL = state.cells[i].flui.VisOM(temperatureLow);
                source.viscH = state.cells[i].flui.VisOM(temperatureHigh);
            }
        } else if (state.cells[i].acsrL != 0 && ((*state.cells[i].acsrL).tipo == kAccessoryChoke ||
                                                 (*state.cells[i].acsrL).tipo == kAccessoryVolumetricPump)) {
            if (i > 0) {
                (void)(state.cells[i - 1].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                       (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre));
                if (hasProductionFluidOrNetwork(state)) {
                    double rhoPSTF = (1 - state.cells[i - 1].flui.BSW) *
                                         (1000 * 141.5 / (131.5 + state.cells[i - 1].flui.API)) +
                                     state.cells[i - 1].flui.BSW * 1000 * state.cells[i - 1].flui.Denag;
                    source.deadOil *= ((1 - state.cells[i - 1].flui.BSW) / rhoPSTF);
                    source.water *= (state.cells[i - 1].flui.BSW / rhoPSTF);
                    source.BSW = state.cells[i - 1].flui.BSW;
                    source.denag = state.cells[i - 1].flui.Denag;
                    source.viscL = state.cells[i - 1].flui.VisOM(temperatureLow);
                    source.viscH = state.cells[i - 1].flui.VisOM(temperatureHigh);
                }
            } else {
                (void)(state.cells[i].flui.RS(state.cells[i].pres, state.cells[i].temp) *
                       (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre));
                if (hasProductionFluidOrNetwork(state)) {
                    double rhoPSTF =
                        (1 - state.cells[i].flui.BSW) * (1000 * 141.5 / (131.5 + state.cells[i].flui.API)) +
                        state.cells[i].flui.BSW * 1000 * state.cells[i].flui.Denag;
                    source.deadOil *= ((1 - state.cells[i].flui.BSW) / rhoPSTF);
                    source.water *= (state.cells[i].flui.BSW / rhoPSTF);
                    source.BSW = state.cells[i].flui.BSW;
                    source.denag = state.cells[i].flui.Denag;
                    source.viscL = state.cells[i].flui.VisOM(temperatureLow);
                    source.viscH = state.cells[i].flui.VisOM(temperatureHigh);
                }
            }
        }
    }
}

void solveCellPhaseMolarFractions(const FluidTrackingState &state, const PhaseFace &left,
                                  const PhaseFace &right, PhaseCell &cell, const PhaseSource &source,
                                  int limCorrige, ProFlu &fluF, int i, Vcr<double> &fracMol0,
                                  Vcr<double> &fracMol1, double dt, ProFlu *fluC, int ncomp) {
    PhaseTransported transported;
    for (int corrige = 0; corrige < limCorrige; corrige++) {

        transported.betIV = state.cells[i].bet;
        transported.solutionGasRatio = fluC[i].RS(state.cells[i].pres, state.cells[i].temp);
        transported.oilVolumeFactor =
            fluC[i].BOFunc(state.cells[i].pres, state.cells[i].temp, transported.solutionGasRatio);
        transported.waterVolumeFactor = fluC[i].BAFunc(state.cells[i].pres, state.cells[i].temp);
        transported.bswV =
            fluC[i].BSW * transported.waterVolumeFactor /
            (transported.oilVolumeFactor + transported.waterVolumeFactor * state.cells[i].flui.BSW -
             state.cells[i].flui.BSW * transported.oilVolumeFactor);
        transported.rhoOVol = fluC[i].MasEspoleo(state.cells[i].pres, state.cells[i].temp);
        transported.rhoWVol = fluC[i].MasEspAgua(state.cells[i].pres, state.cells[i].temp);
        transported.titVol =
            (1 - transported.bswV) * transported.rhoOVol /
            ((1 - transported.bswV) * transported.rhoOVol + transported.bswV * transported.rhoWVol);

        transported.tempMol =
            (fluC[i].MasEspLiq(state.cells[i].pres, state.cells[i].temp) * (1. - state.cells[i].alf) *
                 (1. - transported.betIV) * transported.titVol +
             fluC[i].MasEspGas(state.cells[i].pres, state.cells[i].temp) * state.cells[i].alf) *
            state.cells[i].duto.area * state.cells[i].dx / cell.pesoMol;

        double menorFrac = 0.;
        for (int kfrac = 0; kfrac < ncomp; kfrac++) {

            fluC[i].fracMol[kfrac] =
                (state.cells[i].nMolIni * state.cells[i].flui.fracMol[kfrac] +
                 ((source.dissolvedGas + source.freeGas) * fluF.fracMol[kfrac] / source.pesoMol -
                  (right.vazMasLiq + right.vazMasGas) * fracMol1[kfrac] / right.pesoMol +
                  (left.vazMasLiq + left.vazMasGas) * fracMol0[kfrac] / left.pesoMol) *
                     dt) /
                transported.tempMol;
            if (menorFrac > fluC[i].fracMol[kfrac]) {
                menorFrac = fluC[i].fracMol[kfrac];
            }
        }
        double fracTot = 0.;
        if (menorFrac < 0.) {
            for (int kfrac = 0; kfrac < ncomp; kfrac++)
                fluC[i].fracMol[kfrac] -= menorFrac;
        }
        for (int kfrac = 0; kfrac < ncomp; kfrac++)
            fracTot += fluC[i].fracMol[kfrac];
        for (int kfrac = 0; kfrac < ncomp; kfrac++)
            fluC[i].fracMol[kfrac] /= fracTot;
        cell.pesoMol = 0;
        for (int kfrac = 0; kfrac < ncomp; kfrac++)
            cell.pesoMol += fluC[i].masMol[kfrac] * state.cells[i].flui.fracMol[kfrac];
        if (state.compositionalRefreshCounter == state.input.miniTabAtraso ||
            state.input.miniTabAtraso == 0) {
            fluC[i].atualizaPropCompStandard();
            if (fluC[i].dCalculatedBeta < 0. || fluC[i].dCalculatedBeta > 1.)
                fluC[i].atualizaPropComp(state.cells[i].pres, state.cells[i].temp, -1, NULL, NULL,
                                         state.input.pocinjec);
            else
                fluC[i].atualizaPropComp(state.cells[i].pres, state.cells[i].temp, fluC[i].dCalculatedBeta,
                                         fluC[i].oCalculatedLiqComposition, fluC[i].oCalculatedVapComposition,
                                         state.input.pocinjec);
        }
    }
}

void readPhaseSourceFluid(const FluidTrackingState &state, PhaseSource &source, ProFlu &fluF, int i) {
    if (state.cells[i].acsr.tipo == kAccessoryGasInjection) {
        refreshPhaseSourceComposition(state, state.cells[i].acsr.injg.FluidoPro, state.cells[i].pres,
                                      state.cells[i].temp);
        if (state.cells[i].acsr.injg.QGas > 0.)
            fluF = state.cells[i].acsr.injg.FluidoPro;
        else
            fluF = state.cells[i].flui;
        source.tit = 1.;
    } else if (state.cells[i].acsr.tipo == kAccessoryLiquidInjection) {
        refreshPhaseSourceComposition(state, state.cells[i].acsr.injl.FluidoPro, state.cells[i].pres,
                                      state.cells[i].temp);
        if (state.cells[i].acsr.injl.QLiq > 0.)
            fluF = state.cells[i].acsr.injl.FluidoPro;
        else
            fluF = state.cells[i].flui;
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else if (state.cells[i].acsr.tipo == kAccessoryInflowPerformance) {
        refreshPhaseSourceComposition(state, state.cells[i].acsr.ipr.FluidoPro, state.cells[i].pres,
                                      state.cells[i].temp);
        if ((state.cells[i].acsr.ipr.Pres) > state.cells[i].pres)
            fluF = state.cells[i].acsr.ipr.FluidoPro;
        else
            fluF = state.cells[i].flui;
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else if (state.cells[i].acsr.tipo == kAccessoryMultipleSource) {
        refreshPhaseSourceComposition(state, state.cells[i].acsr.injm.FluidoPro, state.cells[i].pres,
                                      state.cells[i].temp);
        if ((state.cells[i].acsr.injm.MassC + state.cells[i].acsr.injm.MassG +
             state.cells[i].acsr.injm.MassP) > 0.)
            fluF = state.cells[i].acsr.injm.FluidoPro;
        else
            fluF = state.cells[i].flui;
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else if (state.cells[i].acsr.tipo == kAccessoryLeak && state.cells[i].acsr.fontechk.abertura > 1e-6) {
        refreshPhaseSourceComposition(state, state.cells[i].acsr.fontechk.fluidoP, state.cells[i].pres,
                                      state.cells[i].temp);
        refreshPhaseSourceComposition(state, state.cells[i].acsr.fontechk.fluidoPamb, state.cells[i].pres,
                                      state.cells[i].temp);
        if (state.cells[i].acsr.fontechk.presT > state.cells[i].acsr.fontechk.pamb) {
            fluF = state.cells[i].acsr.fontechk.fluidoP;
        } else {
            fluF = state.cells[i].acsr.fontechk.fluidoPamb;
        }
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else if (state.cells[i].acsr.tipo == kAccessoryRadialPorous) {
        double tRes = state.cells[i].acsr.radialPoro.tRes;
        refreshPhaseSourceComposition(state, state.cells[i].acsr.radialPoro.flup, state.cells[i].pres, tRes);
        if ((state.cells[i].fontemassLR + state.cells[i].fontemassGR) > 1e-15)
            fluF = state.cells[i].acsr.radialPoro.flup;
        else
            fluF = state.cells[i].flui;
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else if (state.cells[i].acsr.tipo == kAccessoryPorous2D) {
        double tRes = state.cells[i].acsr.poroso2D.dados.tRes;
        refreshPhaseSourceComposition(state, state.cells[i].acsr.poroso2D.dados.flup, state.cells[i].pres,
                                      tRes);
        if ((state.cells[i].fontemassLR + state.cells[i].fontemassGR) > 1e-15)
            fluF = state.cells[i].acsr.poroso2D.dados.flup;
        else
            fluF = state.cells[i].flui;
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else if (state.cells[i].acsr.tipo == kAccessoryRecirculationUpstream ||
               state.cells[i].acsr.tipo == kAccessoryRecirculationDownstream) {
        const auto &connection = state.cells[state.cells[i].acsr.indJusRecic].acsr.fontechk;
        if (connection.presT > connection.pamb)
            fluF = connection.fluidoP;
        else
            fluF = connection.fluidoPamb;
        refreshPhaseSourceComposition(state, fluF, state.cells[i].pres, state.cells[i].temp);
        readPhaseSourceLiquidQuality(state, source, fluF, i);
    } else {
        fluF = state.cells[i].flui;
        source.tit = 0.;
    }
}

void upwindVapourAndMixtureFractions(const FluidTrackingState &state, PhaseFace &left, PhaseFace &right,
                                     int i, Vcr<double> &fracMol0, Vcr<double> &fracMol1,
                                     Vcr<double> &fracMol0G, Vcr<double> &fracMol1G, int ncomp) {
    if ((i > 0 || state.input.ConContEntrada == 1) && (state.cells[i].MC - state.cells[i].Mliqini) >= 0.) {
        double upstreamPressure;
        double upstreamTemperature;
        if (i > 0) {
            upstreamPressure = state.cells[i - 1].pres;
            upstreamTemperature = state.cells[i - 1].temp;
        } else {
            upstreamPressure = state.inletPressure;
            upstreamTemperature = state.inletTemperature;
        }
        prepareVapourFaceComposition(left, (*state.cells[i].fluiL), upstreamPressure, upstreamTemperature,
                                     state.cells[i].alfL, fracMol0G, ncomp);
    } else {
        prepareVapourFaceComposition(left, state.cells[i].flui, state.cells[i].pres, state.cells[i].temp,
                                     state.cells[i].alf, fracMol0G, ncomp);
    }
    if ((state.cells[i + 1].MC - state.cells[i + 1].Mliqini) >= 0.) {
        prepareVapourFaceComposition(right, state.cells[i].flui, state.cells[i].pres, state.cells[i].temp,
                                     state.cells[i].alf, fracMol1G, ncomp);
    } else {
        prepareVapourFaceComposition(right, state.cells[i + 1].flui, state.cells[i + 1].pres,
                                     state.cells[i + 1].temp, state.cells[i + 1].alf, fracMol1G, ncomp);
    }

    if ((i > 0 || state.input.ConContEntrada == 1) && (left.vazMasLiq + left.vazMasGas) >= 0) {
        copyMixtureFaceComposition(left, (*state.cells[i].fluiL), fracMol0, ncomp);
    } else {
        copyMixtureFaceComposition(left, state.cells[i].flui, fracMol0, ncomp);
    }
    if ((right.vazMasLiq + right.vazMasGas) >= 0) {
        copyMixtureFaceComposition(right, state.cells[i].flui, fracMol1, ncomp);
    } else {
        copyMixtureFaceComposition(right, state.cells[i + 1].flui, fracMol1, ncomp);
    }
}

void upwindLiquidFractions(const FluidTrackingState &state, PhaseFace &left, PhaseFace &right, int i,
                           Vcr<double> &fracMol0O, Vcr<double> &fracMol1O, int ncomp) {
    if ((i > 0 || state.input.ConContEntrada == 1) && state.cells[i].Mliqini >= 0.) {
        double upstreamPressure;
        double upstreamTemperature;
        if (i > 0) {
            upstreamPressure = state.cells[i - 1].pres;
            upstreamTemperature = state.cells[i - 1].temp;
        } else {
            upstreamPressure = state.inletPressure;
            upstreamTemperature = state.inletTemperature;
        }
        prepareLiquidFaceTransport(left, (*state.cells[i].fluiL), state.cells[i].fluicol, upstreamPressure,
                                   upstreamTemperature, state.cells[i].QL, state.cells[i].alfL, fracMol0O,
                                   ncomp);
    } else {
        prepareLiquidFaceTransport(left, state.cells[i].flui, state.cells[i].fluicol, state.cells[i].pres,
                                   state.cells[i].temp, state.cells[i].QL, state.cells[i].alf, fracMol0O,
                                   ncomp);
    }
    if (state.cells[i + 1].Mliqini >= 0.) {
        prepareLiquidFaceTransport(right, state.cells[i].flui, state.cells[i].fluicol, state.cells[i].pres,
                                   state.cells[i].temp, state.cells[i].QL, state.cells[i].alf, fracMol1O,
                                   ncomp);
    } else {
        prepareLiquidFaceTransport(right, state.cells[i + 1].flui, state.cells[i].fluicol,
                                   state.cells[i + 1].pres, state.cells[i + 1].temp, state.cells[i + 1].QL,
                                   state.cells[i + 1].alf, fracMol1O, ncomp);
    }
}

void upwindLeftFacePhaseLiquidProperties(const FluidTrackingState &state, PhaseFace &left, int i,
                                         double temperatureHigh, double temperatureLow) {
    if ((i > 0 || state.input.ConContEntrada == 1) && state.cells[i].QL >= 0.) {
        double upstreamPressure;
        double upstreamTemperature;
        if (i > 0) {
            upstreamPressure = state.cells[i - 1].pres;
            upstreamTemperature = state.cells[i - 1].temp;
        } else {
            upstreamPressure = state.inletPressure;
            upstreamTemperature = state.inletTemperature;
        }
        if (state.input.ConContEntrada == 0)
            left.betI = state.cells[i - 1].betPigD;
        else
            left.betI = state.inletComplementaryFraction;
        preparePhaseLiquidFace(left, (*state.cells[i].fluiL), upstreamPressure, upstreamTemperature,
                               temperatureHigh, temperatureLow);
    } else {
        left.betI = state.cells[i].betPigE;
        preparePhaseLiquidFace(left, state.cells[i].flui, state.cells[i].pres, state.cells[i].temp,
                               temperatureHigh, temperatureLow);
    }
}

void transportCellPhaseMolarFractions(const FluidTrackingState &state, int i, Vcr<double> &BSW,
                                      Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                      Vcr<double> &fracMol0, Vcr<double> &fracMol1, Vcr<double> &fracMol0O,
                                      Vcr<double> &fracMol1O, Vcr<double> &fracMol0G, Vcr<double> &fracMol1G,
                                      double temperatureHigh, double temperatureLow, double dt, ProFlu *fluC,
                                      int ncomp) {

    PhaseFace left;
    PhaseFace right;
    PhaseSource source;
    PhaseCell cell;
    double flowArea = state.cells[i].duto.area;
    double dx = state.cells[i].dx;
    ProFlu fluF;

    preparePhaseFacesAndCellWeight(state, left, right, cell, i, temperatureHigh, temperatureLow, ncomp);

    upwindLiquidFractions(state, left, right, i, fracMol0O, fracMol1O, ncomp);

    upwindVapourAndMixtureFractions(state, left, right, i, fracMol0, fracMol1, fracMol0G, fracMol1G, ncomp);

    preparePhaseMixtureSource(state, source, fluF, i, ncomp);
    fluC[i] = state.cells[i].flui;
    int limCorrige = 1;
    if (state.compositionalRefreshCounter == state.input.miniTabAtraso || state.input.miniTabAtraso == 0)
        limCorrige = 2;
    solveCellPhaseMolarFractions(state, left, right, cell, source, limCorrige, fluF, i, fracMol0, fracMol1,
                                 dt, fluC, ncomp);
    updatePhaseMoleInventory(state, cell, fluC, i);
    transportPhaseLiquidProperties(state, left, right, cell, source, fluF, i, BSW, denag, VISCL, VISCH,
                                   temperatureHigh, temperatureLow, dx, flowArea, dt);
}

} // namespace

void transportPhaseMolarFractions(const FluidTrackingState &state, ProFlu fluiRev) {
    Vcr<double> BSW(state.lastCell);
    Vcr<double> denag(state.lastCell);
    Vcr<double> VISCL(state.lastCell);
    Vcr<double> VISCH(state.lastCell);
    int ncomp = state.input.npseudo;
    ProFlu *fluC;
    fluC = new ProFlu[state.lastCell];
    Vcr<double> fracMol0(ncomp);
    Vcr<double> fracMol1(ncomp);
    Vcr<double> fracMol0O(ncomp);
    Vcr<double> fracMol1O(ncomp);
    Vcr<double> fracMol0G(ncomp);
    Vcr<double> fracMol1G(ncomp);
    Vcr<double> fracMolF(ncomp);
    if (state.cells[0].acsr.tipo == kAccessoryRadialPorous &&
        (state.cells[0].fontemassCR + state.cells[0].fontemassGR + state.cells[0].fontemassLR) > 0.)
        state.cells[0].flui.BSW = state.cells[0].acsr.radialPoro.BSW;
    else if (state.cells[0].acsr.tipo == kAccessoryPorous2D &&
             (state.cells[0].fontemassCR + state.cells[0].fontemassGR + state.cells[0].fontemassLR) > 0.)
        state.cells[0].flui.BSW = state.cells[0].acsr.poroso2D.dados.transfer.BSW;
    double dt = state.cells[1].dt;
    double temperatureLow = 0.;
    double temperatureHigh = 70.;
    if (state.compositionalRefreshCounter == state.input.miniTabAtraso && state.input.miniTabAtraso > 0)
        (*state.globals).transientPropertyMode = 0;
    int imin = 1;
    if ((*state.globals).networkMode != 0 &&
        (state.cells[0].acsr.tipo == kAccessoryMultipleSource || state.input.ConContEntrada > 0))
        imin = 0;

    for (int i = imin; i < state.lastCell; i++) {
        transportCellPhaseMolarFractions(state, i, BSW, denag, VISCL, VISCH, fracMol0, fracMol1, fracMol0O,
                                         fracMol1O, fracMol0G, fracMol1G, temperatureHigh, temperatureLow, dt,
                                         fluC, ncomp);
    }
    for (int i = imin; i <= state.lastCell - 1; i++) {
        publishPhaseCellProperties(state, fluC, i, BSW, denag, VISCL, VISCH, temperatureHigh, temperatureLow);
    }

    updatePhaseOutletProperties(state, fluiRev, BSW, denag, VISCL, VISCH, temperatureHigh, temperatureLow);
    delete[] fluC;
    if (state.input.miniTabAtraso > 0)
        (*state.globals).transientPropertyMode = 1;
}

void correctGasSpecificGravity(Cel *const &cells, const Ler &input, const int &steadyIteration, int i) {
    if (input.corrDeng == 0) { // this switch, for black oil, distinguishes
        // between the density of dissolved gas and of free gas
        // rDgD = ratio between the density of the dissolved gas and the gas at standard conditions
        // rDgL = ratio between the density of the free gas and the gas at standard conditions
        // arq.corrDeng==0 means no distinction between the densities
        cells[i].flui.rDgD = 1.;
        cells[i].flui.rDgL = 1.;
        cells[i].flui.PCis = cells[i].flui.PC;
        cells[i].flui.TCis = cells[i].flui.TC;
        if (cells[i].acsr.tipo == kAccessoryGasInjection && cells[i].acsr.injg.seco == 0) {
            cells[i].acsr.injg.FluidoPro.rDgD = 1.;
            cells[i].acsr.injg.FluidoPro.rDgL = 1.;
        }
        if (cells[i].acsr.tipo == kAccessoryLiquidInjection) {
            cells[i].acsr.injl.FluidoPro.rDgD = 1.;
            cells[i].acsr.injl.FluidoPro.rDgL = 1.;
        } else if (cells[i].acsr.tipo == kAccessoryInflowPerformance) {
            cells[i].acsr.ipr.FluidoPro.rDgD = 1.;
            cells[i].acsr.ipr.FluidoPro.rDgL = 1.;
        } else if (cells[i].acsr.tipo == kAccessoryMultipleSource) {
            cells[i].acsr.injm.FluidoPro.rDgD = 1.;
            cells[i].acsr.injm.FluidoPro.rDgL = 1.;
        }
        if (cells[i].acsr.tipo == kAccessoryRadialPorous) {
            cells[i].acsr.radialPoro.flup.rDgD = 1.;
            cells[i].acsr.radialPoro.flup.rDgL = 1.;
            for (int porousCell = 0; porousCell < cells[i].acsr.radialPoro.ncel; porousCell++) {
                cells[i].acsr.radialPoro.celula[porousCell].flup.rDgD = 1.;
                cells[i].acsr.radialPoro.celula[porousCell].flup.rDgL = 1.;
            }
        }
        if (cells[i].acsr.tipo == kAccessoryPorous2D) {
            cells[i].acsr.poroso2D.dados.flup.rDgD = 1.;
            cells[i].acsr.poroso2D.dados.flup.rDgL = 1.;
            for (int porousCell = 0; porousCell < cells[i].acsr.poroso2D.dados.transfer.ncel; porousCell++) {
                cells[i].acsr.poroso2D.dados.transfer.celula[porousCell].flup.rDgD = 1.;
                cells[i].acsr.poroso2D.dados.transfer.celula[porousCell].flup.rDgL = 1.;
            }
            for (int porousCell = 0; porousCell < cells[i].acsr.poroso2D.malha.nele; porousCell++) {
                cells[i].acsr.poroso2D.malha.mlh2d[porousCell].flup.rDgD = 1.;
                cells[i].acsr.poroso2D.malha.mlh2d[porousCell].flup.rDgL = 1.;
            }
        }
    } else { // case arq.corrDeng==1, which means the densities are distinguished
        int k = i - 1;
        if (steadyIteration != 0)
            k = i;
        cells[i].flui.razDegD(cells[k].pres, cells[k].temp);
        cells[i].flui.rzDegL(cells[k].pres, cells[k].temp);
        cells[i].flui.PcTcIS(); // as the free-gas density changed, its critical
        // pressures must be recomputed
        // if there is a source, computes the sources' in-situ densities
        if (cells[i].acsr.tipo == kAccessoryGasInjection && cells[i].acsr.injg.seco == 0) { // liquid source
            cells[i].acsr.injg.FluidoPro.razDegD(cells[k].pres, cells[k].temp);
            cells[i].acsr.injg.FluidoPro.rzDegL(cells[k].pres, cells[k].temp);
        }
        if (cells[i].acsr.tipo == kAccessoryLiquidInjection) { // liquid source
            cells[i].acsr.injl.FluidoPro.razDegD(cells[k].pres, cells[k].temp);
            cells[i].acsr.injl.FluidoPro.rzDegL(cells[k].pres, cells[k].temp);
        } else if (cells[i].acsr.tipo == kAccessoryInflowPerformance) { // ipr
            cells[i].acsr.ipr.FluidoPro.razDegD(cells[k].pres, cells[k].temp);
            cells[i].acsr.ipr.FluidoPro.rzDegL(cells[k].pres, cells[k].temp);
        } else if (cells[i].acsr.tipo == kAccessoryMultipleSource) { // generic mass source
            cells[i].acsr.injm.FluidoPro.razDegD(cells[k].pres, cells[k].temp);
            cells[i].acsr.injm.FluidoPro.rzDegL(cells[k].pres, cells[k].temp);
        }
        if (cells[i].acsr.tipo == kAccessoryRadialPorous) {
            cells[i].acsr.radialPoro.flup.razDegD(cells[i].pres, cells[i].temp);
            cells[i].acsr.radialPoro.flup.rzDegL(cells[i].pres, cells[i].temp);
            for (int porousCell = 0; porousCell < cells[i].acsr.radialPoro.ncel; porousCell++) {
                double pres = cells[i].acsr.radialPoro.celula[porousCell].Pcamada;
                double temp = cells[i].acsr.radialPoro.tRes;
                cells[i].acsr.radialPoro.celula[porousCell].flup.razDegD(pres, temp);
                cells[i].acsr.radialPoro.celula[porousCell].flup.rzDegL(pres, temp);
            }
        }
        if (cells[i].acsr.tipo == kAccessoryPorous2D) {
            cells[i].acsr.poroso2D.dados.flup.razDegD(cells[i].pres, cells[i].temp);
            cells[i].acsr.poroso2D.dados.flup.rzDegL(cells[i].pres, cells[i].temp);
            for (int porousCell = 0; porousCell < cells[i].acsr.poroso2D.dados.transfer.ncel; porousCell++) {
                double pres = cells[i].acsr.poroso2D.dados.transfer.celula[porousCell].Pcamada;
                double temp = cells[i].acsr.poroso2D.dados.transfer.tRes;
                cells[i].acsr.poroso2D.dados.transfer.celula[porousCell].flup.razDegD(pres, temp);
                cells[i].acsr.poroso2D.dados.transfer.celula[porousCell].flup.rzDegL(pres, temp);
            }
            for (int porousCell = 0; porousCell < cells[i].acsr.poroso2D.malha.nele; porousCell++) {
                double pres = cells[i].acsr.poroso2D.malha.mlh2d[porousCell].cel2D.presC;
                double temp = cells[i].acsr.poroso2D.malha.mlh2d[porousCell].tRes;
                cells[i].acsr.poroso2D.malha.mlh2d[porousCell].flup.razDegD(pres, temp);
                cells[i].acsr.poroso2D.malha.mlh2d[porousCell].flup.rzDegL(pres, temp);
            }
        }
    }
}

} // namespace sisprod::fluidtracking
