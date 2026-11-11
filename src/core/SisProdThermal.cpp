#include "SisProdTransient.h"
#include "DriftFluxClosure.h"
#include "SisProdSources.h"
#include "SisProdThermalInternal.h"
#include "TransientKineticResponse.h"
#include "TransientTemperatureUpdate.h"

#include "SisProdConstants.h"

#include "Leitura.h"
#include "celula3.h"
#include "solver3DPoisson.h"
#include "solverPoissonAxiSim.h"
#include "dados1PoissonAxiSim.h"
#include "UnitConversions.h"

#include <math.h>
#include <numbers>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <omp.h>

namespace sisprod::thermal {
namespace {

template <typename Axis>
[[nodiscard]] int bracketingIndex(double value, int divisionCount, Axis &&axis,
                                  int descendProbeOffset) {
    int index = 0;
    int searchLow = 1;
    int searchHigh = divisionCount + 1;
    while (searchLow <= searchHigh) {
        const int searchMiddle = (searchLow + searchHigh) / 2;
        if (searchMiddle == 1) {
            index = searchMiddle;
            break;
        } else if (searchMiddle == divisionCount + 1 && axis(searchMiddle) == value) {
            index = searchMiddle - 1;
            break;
        }
        if (axis(searchMiddle) > value && axis(searchMiddle - 1) <= value) {
            index = searchMiddle - 1;
            break;
        }
        if (axis(searchMiddle + descendProbeOffset) < value)
            searchLow = searchMiddle + 1;
        else
            searchHigh = searchMiddle - 1;
    }
    return index;
}

[[nodiscard]] SourceEnthalpy sourceEnthalpyOf(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    double sourceTemperature = cell.temp;
    double sourceGasEnthalpy;
    double sourceLiquidEnthalpy;
    double hcF = 0.;

    if (cell.acsr.tipo == kAccessoryGasInjection) {
        sourceTemperature = cell.acsr.injg.temp;
        sourceGasEnthalpy = cell.acsr.injg.FluidoPro.EntalpGas(cell.pres, sourceTemperature);
        sourceLiquidEnthalpy = 0;
        hcF = 0;
    } else if (cell.acsr.tipo == kAccessoryLiquidInjection) {
        sourceTemperature = cell.acsr.injl.temp;
        sourceGasEnthalpy = cell.acsr.injl.FluidoPro.EntalpGas(cell.pres, sourceTemperature);
        sourceLiquidEnthalpy = cell.acsr.injl.FluidoPro.EntalpLiq(cell.pres, sourceTemperature);
        hcF = cell.acsr.injl.fluidocol.CalorLiq(cell.pres, sourceTemperature) * sourceTemperature;
    } else if (cell.acsr.tipo == kAccessoryInflowPerformance) {
        sourceTemperature = cell.acsr.ipr.Tres;
        sourceGasEnthalpy = cell.acsr.ipr.FluidoPro.EntalpGas(cell.pres, sourceTemperature);
        sourceLiquidEnthalpy = cell.acsr.ipr.FluidoPro.EntalpLiq(cell.pres, sourceTemperature);
        hcF = 0;
    } else if (cell.acsrL != 0) {
        return upstreamSourceEnthalpyOf(state, cellIndex);
    } else {
        sourceGasEnthalpy = 0.;
        sourceLiquidEnthalpy = 0.;
    }
    return SourceEnthalpy{.gasEnthalpy = sourceGasEnthalpy,
                          .liquidEnthalpy = sourceLiquidEnthalpy,
                          .hcF = hcF};
}

[[nodiscard]] bool accessoryImposesPressureJump(const ThermalState &state,
                                                int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    return (cell.acsr.tipo == kAccessoryChoke &&
            cell.acsr.chk.AreaGarg <=
                (1e-3 + state.input.master1.razareaativ) *
                    cell.duto.area) ||
           (cell.acsr.tipo == kAccessoryPump &&
            cell.acsr.bcs.freq > 0) ||
           (cell.acsr.tipo == kAccessoryVolumetricPump &&
            cell.acsr.bvol.freq > 0.) ||
           (cell.acsr.tipo == 7 &&
            fabs(cell.acsr.delp) > 0.) ||
           (cell.acsr.tipo == kAccessoryMultiPump &&
            cell.acsr.multibcs.freq > 0);
}

[[nodiscard]] CellFlowBasis cellFlowBasisOf(const ThermalState &state, int cellIndex) {
    SimulationContext &globals = *state.globals;
    Cel &cell = state.cells[cellIndex];
    const bool cellHasGas = cell.alf > globals.smallValueTolerance;
    const bool cellHasLiquid = cell.alf < 1. - globals.smallValueTolerance;
    double diameter = cell.duto.a;
    double flowArea = 0.25 * std::numbers::pi * diameter * diameter;
    double meanVoidFraction = cell.alf;
    double betmed = cell.bet;
    const bool useCellFlow =
        cellIndex > 0 &&
        (state.cells[cellIndex - 1].acsr.tipo != kAccessoryChoke ||
         state.cells[cellIndex - 1].acsr.chk.AreaGarg >
             (1e-3 + state.input.master1.razareaativ) * state.cells[cellIndex - 1].duto.area);
    const Cel &donor = useCellFlow || cellIndex == state.lastCell
                           ? cell
                           : state.cells[cellIndex + 1];
    const double meanSuperficialGasVelocity = cellHasGas ? donor.QG / flowArea : 0.;
    const double meanSuperficialLiquidVelocity = cellHasLiquid ? donor.QL / flowArea : 0.;
    return CellFlowBasis{.flowArea = flowArea,
                         .voidFraction = meanVoidFraction,
                         .betmed = betmed,
                         .gasSuperficialVelocity = meanSuperficialGasVelocity,
                         .liquidSuperficialVelocity = meanSuperficialLiquidVelocity};
}

[[nodiscard]] TemperatureTransport temperatureTransportOf(const ThermalState &state,
                                                          int cellIndex) {
    const Cel &cell = state.cells[cellIndex];
    const CellFlowBasis flow = cellFlowBasisOf(state, cellIndex);
    const double liquidDensity = (1. - flow.betmed) * cell.rpC + flow.betmed * cell.rcC;
    const double liquidSpecificHeat = (1. - flow.betmed) * cell.flui.CalorLiq(cell.presini, cell.temp) +
                                      flow.betmed * cell.fluicol.CalorLiq(cell.presini, cell.temp);
    const double gasSpecificHeat = cell.flui.CalorGas(cell.presini, cell.temp);
    const double gasSpecificHeatConstantVolume = cell.flui.CalorGasVolMod(cell.presini, cell.temp, cell.rgC);
    const double timeCoefficient = (liquidDensity * (1 - flow.voidFraction) * liquidSpecificHeat +
                                    cell.rgC * flow.voidFraction * gasSpecificHeatConstantVolume) * flow.flowArea;
    const double spatialCoefficient = (liquidDensity * flow.liquidSuperficialVelocity * liquidSpecificHeat +
                                       cell.rgC * flow.gasSuperficialVelocity * gasSpecificHeat) * flow.flowArea;
    return TemperatureTransport{.flow = flow,
                                .liquidDensity = liquidDensity,
                                .liquidSpecificHeat = liquidSpecificHeat,
                                .gasSpecificHeat = gasSpecificHeat,
                                .timeCoefficient = timeCoefficient,
                                .spatialCoefficient = spatialCoefficient};
}


TemperatureBalance prepareTemperatureBalance(const ThermalState &state,
                                             int cellIndex,
                                             int steadyStateMode,
                                             const TemperatureTransport &transport) {
    Cel &cell = state.cells[cellIndex];
    double cellLength = cell.dx;
    double meanCellLength;
    if (cellIndex > 0)
        meanCellLength = 0.5 * (cell.dx + state.cells[cellIndex - 1].dx);
    else
        meanCellLength = 0.5 * cell.dx;
    const CellFlowBasis &basis = transport.flow;
    const double flowArea = basis.flowArea;
    const double meanVoidFraction = basis.voidFraction;
    const double betmed = basis.betmed;
    const double meanSuperficialGasVelocity = basis.gasSuperficialVelocity;
    const double meanSuperficialLiquidVelocity = basis.liquidSuperficialVelocity;
    double referenceMixtureVelocity = fabs(meanSuperficialGasVelocity) + fabs(meanSuperficialLiquidVelocity);
    double producedLiquidDensity = cell.rpC;
    double complementaryDensity = cell.rcC;
    double liquidDensity = transport.liquidDensity;
    double gasDensity = cell.rgC;
    double liquidSpecificHeat = transport.liquidSpecificHeat;
    double gasSpecificHeat = transport.gasSpecificHeat;
    double liquidJouleThomson = (1. - betmed) * cell.flui.JTL(cell.presini, cell.temp) - betmed / complementaryDensity;
    double gasJouleThomson = cell.flui.JTG(cell.presini, cell.temp, cell.rgC);
    double hydrostaticPower = (liquidDensity * meanSuperficialLiquidVelocity + gasDensity * meanSuperficialGasVelocity) * flowArea * kGravity * sin(cell.duto.teta);

    cell.calor.Tint = cell.temp;
    if (cellIndex > 0)
        cell.calor.dtL = cell.temp - state.cells[cellIndex - 1].tempini;
    else
        cell.calor.dtL = 0;
    cell.calor.Vint = meanSuperficialGasVelocity + meanSuperficialLiquidVelocity;
    cell.calor.dt = cell.dt;
    double liquidConductivity = (1. - betmed) * cell.flui.CondLiq(cell.presini, cell.temp) + betmed * cell.fluicol.CondLiq(cell.presini, cell.temp);
    cell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + cell.flui.CondGas(cell.presini, cell.temp) * meanVoidFraction;
    cell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
    cell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
    double liquidViscosity = (1. - betmed) * cell.mipC + betmed * cell.micC;
    cell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + cell.migC * meanVoidFraction * units::kPascalSecondPerCentipoise;
    double temperaturePerturbation = cell.temp * 0.01;
    if (fabs(cell.temp) < 1e-15)
        temperaturePerturbation = 0.1;
    double liquidDensityChange = (1. - betmed) * cell.flui.MasEspLiq(cell.presini, cell.temp + temperaturePerturbation) +
                                 betmed * cell.fluicol.MasEspFlu(cell.presini, cell.temp + temperaturePerturbation) - liquidDensity;
    double gasDensityChange = cell.flui.MasEspGas(cell.presini, cell.temp + temperaturePerturbation) - gasDensity;
    cell.calor.betint = -(1 / cell.calor.rhoint) * (liquidDensityChange * (1 - meanVoidFraction) + gasDensityChange * meanVoidFraction) / (temperaturePerturbation);
    const double heatFlux = temperatureHeatFluxOf(state, cellIndex, steadyStateMode);

    cell.fluxcalmed = heatFlux;

    double pressureTimeCoefficient;
    pressureTimeCoefficient = -cell.flui.CalorGasPresMod(cell.presini, cell.temp) * (gasDensity * meanVoidFraction * flowArea);
    double pressureSpatialCoefficient = (liquidDensity * meanSuperficialLiquidVelocity * liquidJouleThomson + gasDensity * meanSuperficialGasVelocity * gasJouleThomson) * flowArea;

    return TemperatureBalance{
        .cellLength = cellLength,
        .meanCellLength = meanCellLength,
        .flowArea = flowArea,
        .voidFraction = meanVoidFraction,
        .bet = betmed,
        .gasSuperficialVelocity = meanSuperficialGasVelocity,
        .liquidSuperficialVelocity = meanSuperficialLiquidVelocity,
        .referenceMixtureVelocity = referenceMixtureVelocity,
        .rp = producedLiquidDensity,
        .rc = complementaryDensity,
        .liquidDensity = liquidDensity,
        .hydrostaticPower = hydrostaticPower,
        .heatFlux = heatFlux,
        .timeCoefficient = transport.timeCoefficient,
        .pressureTimeCoefficient = pressureTimeCoefficient,
        .temperatureSpatialCoefficient = transport.spatialCoefficient,
        .pressureSpatialCoefficient = pressureSpatialCoefficient,
    };
}

double computeKineticTemperatureTerm(const ThermalState &state, int cellIndex,
                                     TemperatureBalance &balance) {
    Cel &cell = state.cells[cellIndex];
    double kineticTerm = 0;
    double leftFaceMeanGasVelocity = 0;
    double leftFaceMeanLiquidVelocity = 0;
    double rightFaceMeanGasVelocity = 0;
    double rightFaceMeanLiquidVelocity = 0;
    double previousRightFaceMeanGasVelocity = 0;
    double previousRightFaceMeanLiquidVelocity = 0;
    const bool ordinaryAccessories = cellIndex < state.lastCell - 1 &&
                                     cell.acsr.tipo == kAccessoryNone &&
                                     state.cells[cellIndex + 1].acsr.tipo == kAccessoryNone;
    if (!detail::includesTransientKineticWork(cellIndex, state.lastCell, ordinaryAccessories,
                                            state.surfaceChoke.AreaGarg, state.surfaceChoke.AreaTub))
        return kineticTerm;
    Cel &rightCell = state.cells[cellIndex + 1];

    double kineticCellLength = cell.dx;
    double cellDiameter = cell.duto.a;
    double cellFlowArea = 0.25 * std::numbers::pi * cellDiameter * cellDiameter;

    double rpcin = rightCell.rpCi;
    double rccin = rightCell.rcCi;
    double kineticLiquidDensity = (1. - balance.bet) * rpcin + balance.bet * rccin;
    double kineticGasDensity = rightCell.rgCi;

    double rightFaceVoidFraction;
    double previousRightFaceVoidFraction;

    balance.gasSuperficialVelocity = rightCell.QG / balance.flowArea;
    balance.liquidSuperficialVelocity = rightCell.QL / balance.flowArea;

    if (rightCell.QG > 0)
        rightFaceVoidFraction = cell.alf;
    else
        rightFaceVoidFraction = rightCell.alf;

    if (rightCell.QGini > 0)
        previousRightFaceVoidFraction = cell.alfini;
    else
        previousRightFaceVoidFraction = rightCell.alfini;

    const double rightFaceLiquidFraction =
        1. - (rightCell.QL > 0. ? cell.alf : rightCell.alf);
    const double previousRightFaceLiquidFraction =
        1. - (rightCell.QLini > 0. ? cell.alfini : rightCell.alfini);

    double leftFaceVoidFraction;
    if (cell.QG > 0)
        leftFaceVoidFraction = cellIndex > 0 ? state.cells[cellIndex - 1].alf : cell.alfL;
    else
        leftFaceVoidFraction = cell.alf;

    const double leftFaceLiquidFraction =
        1. - (cell.QL > 0.
                  ? (cellIndex > 0 ? state.cells[cellIndex - 1].alf : cell.alfL)
                  : cell.alf);

    if (leftFaceVoidFraction > 1e-3) {
        leftFaceMeanGasVelocity = cell.QG / (cellFlowArea);
        leftFaceMeanGasVelocity /= leftFaceVoidFraction;
    }

    if (leftFaceLiquidFraction > 1e-3) {
        leftFaceMeanLiquidVelocity = cell.QL / (cellFlowArea);
        leftFaceMeanLiquidVelocity /= leftFaceLiquidFraction;
    }

    if (rightFaceVoidFraction > 1e-3) {
        rightFaceMeanGasVelocity = balance.gasSuperficialVelocity;
        rightFaceMeanGasVelocity /= rightFaceVoidFraction;
    }

    if (rightFaceLiquidFraction > 1e-3) {
        rightFaceMeanLiquidVelocity = balance.liquidSuperficialVelocity;
        rightFaceMeanLiquidVelocity /= rightFaceLiquidFraction;
    }

    if (previousRightFaceVoidFraction > 1e-3) {

        previousRightFaceMeanGasVelocity = rightCell.QGini / (cellFlowArea);
        previousRightFaceMeanGasVelocity /= previousRightFaceVoidFraction;
    }

    if (previousRightFaceLiquidFraction > 1e-3) {

        previousRightFaceMeanLiquidVelocity = rightCell.QLini / (cellFlowArea);
        previousRightFaceMeanLiquidVelocity /= previousRightFaceLiquidFraction;
    }

    kineticTerm = kineticLiquidDensity * (1. - rightFaceVoidFraction) * cellFlowArea * (0.5 * (rightFaceMeanLiquidVelocity * rightFaceMeanLiquidVelocity - previousRightFaceMeanLiquidVelocity * previousRightFaceMeanLiquidVelocity)) / cell.dt +
                  kineticGasDensity * rightFaceVoidFraction * cellFlowArea * (0.5 * (rightFaceMeanGasVelocity * rightFaceMeanGasVelocity - previousRightFaceMeanGasVelocity * previousRightFaceMeanGasVelocity)) / cell.dt +
                  ((rightCell.MC - rightCell.Mliqini) * rightFaceMeanGasVelocity * (rightFaceMeanGasVelocity - leftFaceMeanGasVelocity) / kineticCellLength +
                   rightCell.Mliqini * rightFaceMeanLiquidVelocity * (rightFaceMeanLiquidVelocity - leftFaceMeanLiquidVelocity) / kineticCellLength);

    return kineticTerm;
}

void clearSourceSpecificHeats(double &sourceGasSpecificHeat,
                              double &sourceSpecificHeatRatio,
                              double &sourceLiquidSpecificHeat) {
    sourceGasSpecificHeat = 0.;
    sourceSpecificHeatRatio = 1.;
    sourceLiquidSpecificHeat = 0.;
}

TemperatureSourceTerms computeTemperatureSourceTerms(const ThermalState &state,
                                                     int cellIndex,
                                                     double cellLength) {
    Cel &cell = state.cells[cellIndex];
    double sourceTemperature = cell.temp;
    double sourceGasSpecificHeat;
    double sourceLiquidSpecificHeat;
    if (cell.acsr.tipo == kAccessoryGasInjection) {
        sourceTemperature = cell.acsr.injg.temp;
        sourceGasSpecificHeat = cell.acsr.injg.FluidoPro.CalorGas(cell.presini, cell.temp);
        sourceLiquidSpecificHeat = 0.;
    } else if (cell.acsr.tipo == kAccessoryLiquidInjection) {
        sourceTemperature = cell.acsr.injl.temp;
        sourceGasSpecificHeat = cell.acsr.injl.FluidoPro.CalorGas(cell.presini, cell.temp);
        sourceLiquidSpecificHeat = (1. - cell.acsr.injl.bet) * cell.acsr.injl.FluidoPro.CalorLiq(cell.presini, cell.temp) + cell.acsr.injl.bet * cell.acsr.injl.fluidocol.CalorLiq(cell.presini, cell.temp); //(1. - cell.acsr.injl.bet) * cell.acsr.injl.FluidoPro.CalorLiq(cell.pres, cell.temp)
    } else if (cell.acsr.tipo == kAccessoryMultipleSource) {
        sourceTemperature = cell.acsr.injm.temp;
        sourceGasSpecificHeat = cell.acsr.injm.FluidoPro.CalorGas(cell.presini, cell.temp);
        if ((cell.fontemassLR + cell.fontemassCR) > 0.) {
            double titbet = cell.fontemassCR / (cell.fontemassLR + cell.fontemassCR);
            sourceLiquidSpecificHeat = (1. - titbet) * cell.acsr.injl.FluidoPro.CalorLiq(cell.presini, cell.temp) +
                                       titbet * cell.acsr.injl.fluidocol.CalorLiq(cell.presini, cell.temp);
        } else
            sourceLiquidSpecificHeat = 0.;
    } else if (cell.acsr.tipo == kAccessoryInflowPerformance) {
        sourceTemperature = cell.acsr.ipr.Tres;
        sourceGasSpecificHeat = cell.acsr.ipr.FluidoPro.CalorGas(cell.presini, cell.temp);
        sourceLiquidSpecificHeat = cell.acsr.ipr.FluidoPro.CalorLiq(cell.presini, cell.temp);
    } else if (cell.acsr.tipo == kAccessoryRadialPorous) {
        sourceTemperature = cell.acsr.radialPoro.tRes;
        sourceGasSpecificHeat = cell.acsr.radialPoro.flup.CalorGas(cell.presini, cell.temp);
        sourceLiquidSpecificHeat = cell.acsr.radialPoro.flup.CalorLiq(cell.presini, cell.temp);
    } else if (cell.acsr.tipo == kAccessoryLeak) {
        double betM = cell.acsr.fontechk.betISamb;
        double ambientPressure = cell.acsr.fontechk.pamb;
        double ambientTemperature = cell.acsr.fontechk.tamb;
        sourceGasSpecificHeat = cell.acsr.fontechk.fluidoPamb.CalorGas(ambientPressure, ambientTemperature);
        sourceLiquidSpecificHeat = (1. - betM) * cell.acsr.fontechk.fluidoPamb.CalorLiq(ambientPressure, ambientTemperature) + betM * cell.acsr.fontechk.fluidocol.CalorLiq(ambientPressure, ambientTemperature);
        sourceTemperature = cell.acsr.fontechk.tamb;
    } else if (cellIndex > 0 && cell.acsrL != 0 && cellIndex < state.lastCell) {
        Cel &leftCell = state.cells[cellIndex - 1];
        if ((*cell.acsrL).tipo == kAccessoryChoke && (cell.fontemassLR + cell.fontemassCR + cell.fontemassGR) > 0.) {
            if ((*cell.acsrL).chk.AreaGarg < state.input.master1.razareaativ * cell.dutoL.area && (*cell.acsrL).chk.AreaGarg > 1e-5 * cell.dutoL.area) {
                double upstreamTemperature = leftCell.tempini;
                double chokeUpstreamVoidFraction = leftCell.alf;
                double betE = leftCell.bet;

                double upstreamLiquidDensity = leftCell.rpC;
                double rholc = leftCell.rcC;
                double upstreamMixtureLiquidDensity = (1 - betE) * upstreamLiquidDensity + betE * rholc;

                double chokeDownstreamVoidFraction = cell.alf;
                double betJ = cell.bet;
                double downstreamLiquidDensity = cell.rpC;
                double rholcJ = cell.rcC;
                double downstreamMixtureLiquidDensity = (1 - betJ) * downstreamLiquidDensity + betJ * rholcJ;

                double upstreamHydrostaticHead = sin(leftCell.duto.teta) * (0.5 * leftCell.dx) * (upstreamMixtureLiquidDensity * (1 - chokeUpstreamVoidFraction) + chokeUpstreamVoidFraction * leftCell.rgC) * kGravity / units::kPascalPerKgfPerCm2;
                double downstreamHydrostaticHead = sin(cell.duto.teta) * (0.5 * cell.dx) * (downstreamMixtureLiquidDensity * (1 - chokeDownstreamVoidFraction) + chokeDownstreamVoidFraction * cell.rgC) * kGravity / units::kPascalPerKgfPerCm2;
                double upstreamGasMassFraction = chokeUpstreamVoidFraction * leftCell.rgC / (leftCell.rgC * chokeUpstreamVoidFraction + upstreamMixtureLiquidDensity * (1. - chokeUpstreamVoidFraction));
                sourceGasSpecificHeat = leftCell.flui.CalorGas(leftCell.presini, leftCell.tempini);
                sourceLiquidSpecificHeat = (1 - leftCell.bet) * leftCell.flui.CalorLiq(leftCell.presini, leftCell.tempini) + leftCell.bet * leftCell.fluicol.CalorLiq(leftCell.presini, leftCell.tempini);
                double upstreamLiquidJouleThomson = (1. - betE) * leftCell.flui.JTL(leftCell.presini - upstreamHydrostaticHead, leftCell.tempini) - betE / rholcJ; //(1. - betE) * leftCell.flui.JTL(leftCell.pres - upstreamHydrostaticHead, leftCell.temp)
                double upstreamGasJouleThomson = leftCell.flui.JTG(leftCell.presini - upstreamHydrostaticHead, leftCell.tempini);
                sourceTemperature = upstreamTemperature + ((1. - upstreamGasMassFraction) * upstreamLiquidJouleThomson / sourceLiquidSpecificHeat + upstreamGasMassFraction * upstreamGasJouleThomson / sourceGasSpecificHeat) *
                                                              (cell.pres + downstreamHydrostaticHead - leftCell.pres - upstreamHydrostaticHead) * units::kPascalPerKgfPerCm2;


            } else {
                sourceGasSpecificHeat = leftCell.flui.CalorGas(leftCell.presini, leftCell.tempini);
                sourceLiquidSpecificHeat = (1 - leftCell.bet) * leftCell.flui.CalorLiq(leftCell.presini, leftCell.tempini) + leftCell.bet * leftCell.fluicol.CalorLiq(leftCell.presini, leftCell.tempini);
                sourceTemperature = cell.temp;
            }
        } else if ((*cell.acsrL).tipo == kAccessoryVolumetricPump) {
            double betM = leftCell.bet;
            sourceGasSpecificHeat = leftCell.flui.CalorGas(leftCell.presini, leftCell.tempini);
            sourceLiquidSpecificHeat = (1. - betM) * leftCell.flui.CalorLiq(leftCell.presini, leftCell.tempini) + betM * leftCell.fluicol.CalorLiq(leftCell.presini, leftCell.tempini);
            double polytropicExponent = (*cell.acsrL).bvol.npoli;
            double pumpPressureRatio = (cell.pres) / (leftCell.pres);
            sourceTemperature = leftCell.tempini * pow(pumpPressureRatio, (polytropicExponent - 1) / polytropicExponent);
        } else {
            sourceGasSpecificHeat = 0.;
            sourceLiquidSpecificHeat = 0.;
        }
    } else if (cellIndex == state.lastCell && (cell.fontemassLR + cell.fontemassCR + cell.fontemassGR) > 0.) {
        if ((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1)
            sourceTemperature = cell.calor.Textern1;
        else
            sourceTemperature = state.gasSurfaceTemperature;
        sourceGasSpecificHeat = cell.flui.CalorGas(cell.presini, sourceTemperature);
        double betM = 0.;
        if (fabs(cell.fontemassCR) > 1e-15)
            betM = cell.fontemassCR /
                   (cell.fontemassCR + cell.fontemassLR);
        sourceLiquidSpecificHeat = (1. - betM) * cell.flui.CalorLiq(cell.presini, sourceTemperature) + betM * cell.fluicol.CalorLiq(cell.presini, sourceTemperature);
    } else {
        sourceGasSpecificHeat = 0.;
        sourceLiquidSpecificHeat = 0.;
    }

    const double liquidHeatCapacityRate =
        (std::max(0., cell.fontemassLR) + std::max(0., cell.fontemassCR)) *
        sourceLiquidSpecificHeat / cellLength;
    const double gasHeatCapacityRate =
        std::max(0., cell.fontemassGR) * sourceGasSpecificHeat / cellLength;
    const double temperatureDifference = sourceTemperature - cell.temp;
    return TemperatureSourceTerms{
        .gas = gasHeatCapacityRate * temperatureDifference,
        .liquid = liquidHeatCapacityRate * temperatureDifference,
        .heatCapacityRate = gasHeatCapacityRate + liquidHeatCapacityRate,
    };
}

TemperatureSourceTerms computeThermalMassTransferSourceTerms(
    const ThermalState &state, int cellIndex, double cellLength) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double sourceTemperature = cell.temp;
    double sourceGasSpecificHeat;
    double sourceSpecificHeatRatio = 0.;
    double sourceLiquidSpecificHeat;
    if (cell.acsr.tipo == kAccessoryGasInjection) {
        sourceTemperature = cell.acsr.injg.temp;
        sourceGasSpecificHeat = cell.acsr.injg.FluidoPro.CalorGas(cell.pres, cell.temp);
        sourceSpecificHeatRatio = cell.acsr.injg.FluidoPro.ConstAdG(cell.pres, cell.temp);
        sourceLiquidSpecificHeat = 0.;
    } else if (cell.acsr.tipo == kAccessoryLiquidInjection) {
        sourceTemperature = cell.acsr.injl.temp;
        sourceGasSpecificHeat = cell.acsr.injl.FluidoPro.CalorGas(cell.pres, cell.temp);
        sourceSpecificHeatRatio = cell.acsr.injl.FluidoPro.ConstAdG(cell.pres, cell.temp);
        sourceLiquidSpecificHeat = (1. - cell.acsr.injl.bet) * cell.acsr.injl.FluidoPro.CalorLiq(cell.pres, cell.temp) + cell.acsr.injl.bet * cell.acsr.injl.fluidocol.CalorLiq(cell.pres, cell.temp);
    } else if (cell.acsr.tipo == kAccessoryInflowPerformance) {
        sourceTemperature = cell.acsr.ipr.Tres;
        sourceGasSpecificHeat = cell.acsr.ipr.FluidoPro.CalorGas(cell.pres, cell.temp);
        sourceSpecificHeatRatio = cell.acsr.ipr.FluidoPro.ConstAdG(cell.pres, cell.temp);
        sourceLiquidSpecificHeat = cell.acsr.ipr.FluidoPro.CalorLiq(cell.pres, cell.temp);
    } else if (cell.acsrL != 0) {
        if ((*cell.acsrL).tipo == kAccessoryChoke) {
            if ((*cell.acsrL).chk.AreaGarg < state.input.master1.razareaativ * cell.dutoL.area && (*cell.acsrL).chk.AreaGarg > 1e-5 * cell.dutoL.area) {
                double upstreamTemperature = leftCell.temp;
                double chokeUpstreamVoidFraction = leftCell.alf;
                double betE = leftCell.bet;

                double upstreamLiquidDensity = leftCell.flui.MasEspLiq(leftCell.pres, leftCell.temp);
                double rholc = leftCell.fluicol.MasEspFlu(leftCell.pres, leftCell.temp);
                double upstreamMixtureLiquidDensity = (1 - betE) * upstreamLiquidDensity + betE * rholc;

                double chokeDownstreamVoidFraction = cell.alf;
                double betJ = cell.bet;
                double downstreamLiquidDensity = cell.flui.MasEspLiq(cell.pres, cell.temp);
                double rholcJ = cell.fluicol.MasEspFlu(cell.pres, cell.temp);
                double downstreamMixtureLiquidDensity = (1 - betJ) * downstreamLiquidDensity + betJ * rholcJ;

                double upstreamHydrostaticHead = sin(leftCell.duto.teta) * (0.5 * leftCell.dx) * (upstreamMixtureLiquidDensity * (1 - chokeUpstreamVoidFraction) + chokeUpstreamVoidFraction * leftCell.flui.MasEspGas(leftCell.pres, leftCell.temp)) * kGravity / units::kPascalPerKgfPerCm2;
                double downstreamHydrostaticHead = sin(cell.duto.teta) * (0.5 * cell.dx) * (downstreamMixtureLiquidDensity * (1 - chokeDownstreamVoidFraction) + chokeDownstreamVoidFraction * cell.flui.MasEspGas(cell.pres, cell.temp)) * kGravity / units::kPascalPerKgfPerCm2;

                double upstreamGasMassFraction = chokeUpstreamVoidFraction * leftCell.flui.MasEspGas(leftCell.pres, leftCell.temp) / (leftCell.flui.MasEspGas(leftCell.pres, leftCell.temp) * chokeUpstreamVoidFraction + upstreamMixtureLiquidDensity * (1. - chokeUpstreamVoidFraction));

                double upstreamLiquidJouleThomson = (1. - betE) * leftCell.flui.JTL(leftCell.pres - upstreamHydrostaticHead, leftCell.temp) - betE / rholcJ;
                double upstreamGasJouleThomson = leftCell.flui.JTG(leftCell.pres - upstreamHydrostaticHead, leftCell.temp);
                sourceTemperature = upstreamTemperature + ((1. - upstreamGasMassFraction) * upstreamLiquidJouleThomson + upstreamGasMassFraction * upstreamGasJouleThomson) * (cell.pres + downstreamHydrostaticHead - leftCell.pres - upstreamHydrostaticHead);

                sourceGasSpecificHeat = leftCell.flui.CalorGas(leftCell.pres, leftCell.temp);
                sourceLiquidSpecificHeat = (1 - leftCell.bet) * leftCell.flui.CalorLiq(leftCell.pres, leftCell.temp) + leftCell.bet * leftCell.fluicol.CalorLiq(leftCell.pres, leftCell.temp);
                sourceSpecificHeatRatio = leftCell.flui.ConstAdG(leftCell.pres, leftCell.temp);

            } else {
                sourceGasSpecificHeat = leftCell.flui.CalorGas(leftCell.pres, leftCell.temp);
                sourceLiquidSpecificHeat = (1 - leftCell.bet) * leftCell.flui.CalorLiq(leftCell.pres, leftCell.temp) + leftCell.bet * leftCell.fluicol.CalorLiq(leftCell.pres, leftCell.temp);
                sourceTemperature = cell.temp;
                sourceSpecificHeatRatio = leftCell.flui.ConstAdG(leftCell.pres, leftCell.temp);
            }
        } else if ((*cell.acsrL).tipo == kAccessoryVolumetricPump) {
            double betM = leftCell.bet;
            sourceGasSpecificHeat = leftCell.flui.CalorGas(leftCell.pres, leftCell.temp);
            sourceLiquidSpecificHeat = (1. - betM) * leftCell.flui.CalorLiq(leftCell.pres, leftCell.temp) + betM * leftCell.fluicol.CalorLiq(leftCell.pres, leftCell.temp);
            double polytropicExponent = (*cell.acsrL).bvol.npoli;
            double pumpPressureRatio = (cell.pres) / (leftCell.pres);
            sourceTemperature = leftCell.temp * pow(pumpPressureRatio, (polytropicExponent - 1) / polytropicExponent);
        } else {
            clearSourceSpecificHeats(sourceGasSpecificHeat, sourceSpecificHeatRatio,
                                     sourceLiquidSpecificHeat);
        }
    } else {
        clearSourceSpecificHeats(sourceGasSpecificHeat, sourceSpecificHeatRatio,
                                 sourceLiquidSpecificHeat);
    }

    double liquidMassSourceTerm = 0;
    if (cell.fontemassLR > 0.)
        liquidMassSourceTerm = cell.fontemassLR / cellLength;
    if (cell.fontemassCR > 0.)
        liquidMassSourceTerm += cell.fontemassCR / cellLength;
    liquidMassSourceTerm *= (sourceLiquidSpecificHeat) * (sourceTemperature - cell.temp);

    double gasMassSourceTerm = cell.fontemassGR / cellLength;
    if (gasMassSourceTerm > 0.)
        gasMassSourceTerm *= (sourceGasSpecificHeat / sourceSpecificHeatRatio) * (sourceTemperature - cell.temp);
    else
        gasMassSourceTerm = 0;

    return TemperatureSourceTerms{
        .gas = gasMassSourceTerm,
        .liquid = liquidMassSourceTerm,
    };
}

MassTransferFaceState initializeDistributedMassTransferInlet(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex) {
    double inletLiquidDensity;
    double inletOilVolumeFactor;
    double inletSolutionGasRatio;
    double inletSolutionGasPressureDerivative;
    Cel &cell = state.cells[cellIndex];
    state.cells[0].transmassLini = state.cells[0].transmassL;
    state.cells[0].transmassL = 0.;
    sources::renewSourceTerms(dependencies.sources, cellIndex);
    state.cells[1].fontemassLLini = state.cells[1].fontemassLL;
    state.cells[1].fontemassLL = state.cells[0].fontemassLR;
    state.cells[1].fontemassCLini = state.cells[1].fontemassCL;
    state.cells[1].fontemassCL = state.cells[0].fontemassCR;
    state.cells[1].fontemassGLini = state.cells[1].fontemassGL;
    state.cells[1].fontemassGL = state.cells[0].fontemassGR;
    inletLiquidDensity =
        (1 - cell.bet) *
            cell.flui.MasEspLiq(
                cell.pres, cell.temp) +
        cell.bet *
            cell.fluicol.MasEspFlu(
                cell.pres, cell.temp);
    inletOilVolumeFactor = cell.flui.BOFunc(
        cell.pres, cell.temp);
    inletSolutionGasRatio = cell.flui.RS(
        cell.pres, cell.temp);
    if (state.input.flashCompleto != 2 || state.input.miniTabAtraso > 0) {
        double inletOilVolumeFactorAtPerturbedPressure = cell.flui.BOFunc(
            cell.pres * kDerivativePerturbationFactor,
            cell.temp);
        double inletSolutionGasRatioAtPerturbedPressure = cell.flui.RS(
            cell.pres * kDerivativePerturbationFactor,
            cell.temp);
        inletSolutionGasPressureDerivative =
            (inletSolutionGasRatio / inletOilVolumeFactor -
             inletSolutionGasRatioAtPerturbedPressure / inletOilVolumeFactorAtPerturbedPressure) /
            (cell.pres * 0.001);
    } else {
        ProFlu flutemp = cell.flui;
        flutemp.atualizaPropComp(
            cell.pres * kDerivativePerturbationFactor,
            cell.temp, flutemp.dCalculatedBeta,
            flutemp.oCalculatedLiqComposition,
            flutemp.oCalculatedVapComposition, state.input.pocinjec);
        double inletOilVolumeFactorAtPerturbedPressure = flutemp.BOFunc(
            cell.pres * kDerivativePerturbationFactor,
            cell.temp);
        double inletSolutionGasRatioAtPerturbedPressure = flutemp.RS(
            cell.pres * kDerivativePerturbationFactor,
            cell.temp);
        inletSolutionGasPressureDerivative =
            (inletSolutionGasRatio / inletOilVolumeFactor -
             inletSolutionGasRatioAtPerturbedPressure / inletOilVolumeFactorAtPerturbedPressure) /
            (cell.pres * 0.001);
    }
    ProFlu flutemp = cell.flui;
    return MassTransferFaceState{
        .liquidDensity = inletLiquidDensity,
        .oilVolumeFactor = inletOilVolumeFactor,
        .solutionGasRatio = inletSolutionGasRatio,
        .solutionGasPressureDerivative = inletSolutionGasPressureDerivative};
}

DistributedMassTransferProperties prepareDistributedMassTransferProperties(
    const ThermalState &state, int cellIndex, double meanTemperature, ProFlu &leftCellLeftFaceFluid,
    ProFlu &leftFaceFluid) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    const bool useTabulatedPvt =
        state.input.flashCompleto != 2 || state.input.miniTabAtraso > 0;
    const bool cellHasNegativeLiquidMassFlowRate = cell.Mliqini < 0;
    const bool leftCellHasNegativeLiquidMassFlowRate = leftCell.Mliqini < 0;
    const MassTransferWaterCuts waterCuts = prepareMassTransferDonors(
        state, cellIndex, leftCellHasNegativeLiquidMassFlowRate, leftCellLeftFaceFluid, leftFaceFluid);
    const double leftFaceWaterCut = waterCuts.leftFaceWaterCut;
    const double leftCellLeftFaceWaterCut = waterCuts.leftCellLeftFaceWaterCut;
    const double leftCellWaterCut = waterCuts.leftCellWaterCut;

    double liquidDensity;
    double gasDensity;

    // compositional case

    liquidDensity = leftFaceFluid.MasEspLiq(cell.presaux, meanTemperature);

    gasDensity = leftFaceFluid.MasEspGas(cell.presaux, meanTemperature);

    double betL = leftCell.betL;

    double betI = leftCell.betPigD;
    if (cellHasNegativeLiquidMassFlowRate)
        betI = cell.betPigE;
    if (cellIndex > 1)
        betL = state.cells[cellIndex - 2].betPigD;
    if (leftCellHasNegativeLiquidMassFlowRate)
        betL = leftCell.betPigE;

    double mixtureLiquidDensity = (1 - betI) * liquidDensity +
                                  betI * cell.fluicol.MasEspFlu(
                                             cell.presaux, meanTemperature);
    double leftFaceOilVolumeFactor;
    double leftFaceSolutionGasRatio;
    double leftFaceOilVolumeFactorAtPerturbedPressure;
    double leftFaceSolutionGasRatioAtPerturbedPressure;
    if (useTabulatedPvt) {
        leftFaceOilVolumeFactor = leftFaceFluid.BOFunc(cell.presaux, meanTemperature);
        leftFaceSolutionGasRatio = leftFaceFluid.RS(cell.presaux, meanTemperature);
        leftFaceOilVolumeFactorAtPerturbedPressure = leftFaceFluid.BOFunc(cell.presaux * kDerivativePerturbationFactor, meanTemperature);
        leftFaceSolutionGasRatioAtPerturbedPressure = leftFaceFluid.RS(cell.presaux * kDerivativePerturbationFactor, meanTemperature);
    } else {
        leftFaceOilVolumeFactor = leftFaceFluid.BOFunc(cell.presaux, meanTemperature);
        leftFaceSolutionGasRatio = leftFaceFluid.RS(cell.presaux, meanTemperature);
        leftFaceFluid.atualizaPropComp(
            cell.presaux * kDerivativePerturbationFactor, meanTemperature, leftFaceFluid.dCalculatedBeta,
            leftFaceFluid.oCalculatedLiqComposition,
            leftFaceFluid.oCalculatedVapComposition, state.input.pocinjec);
        leftFaceOilVolumeFactorAtPerturbedPressure = leftFaceFluid.BOFunc(cell.presaux * kDerivativePerturbationFactor, meanTemperature);
        leftFaceSolutionGasRatioAtPerturbedPressure = leftFaceFluid.RS(cell.presaux * kDerivativePerturbationFactor, meanTemperature);
    } // compositional case
    double leftFaceSolutionGasPressureDerivative =
        (leftFaceSolutionGasRatio / leftFaceOilVolumeFactor - leftFaceSolutionGasRatioAtPerturbedPressure / leftFaceOilVolumeFactorAtPerturbedPressure) / (cell.presaux * 0.001);
    double leftCellOilVolumeFactor;
    double leftCellSolutionGasRatio;
    double leftCellOilVolumeFactorAtPerturbedPressure;
    double leftCellSolutionGasRatioAtPerturbedPressure;
    if (useTabulatedPvt) {
        leftCellOilVolumeFactor = leftCell.flui.BOFunc(
            leftCell.pres, leftCell.temp);
        leftCellSolutionGasRatio = leftCell.flui.RS(
            leftCell.pres, leftCell.temp);
        leftCellOilVolumeFactorAtPerturbedPressure = leftCell.flui.BOFunc(
            leftCell.pres * kDerivativePerturbationFactor, leftCell.temp);
        leftCellSolutionGasRatioAtPerturbedPressure = leftCell.flui.RS(
            leftCell.pres * kDerivativePerturbationFactor, leftCell.temp);
    } else {
        leftCellOilVolumeFactor = leftCell.flui.BOFunc(
            leftCell.pres, leftCell.temp);
        leftCellSolutionGasRatio = leftCell.flui.RS(
            leftCell.pres, leftCell.temp);
        ProFlu flutemp = leftCell.flui;
        flutemp.atualizaPropComp(
            leftCell.pres * kDerivativePerturbationFactor,
            leftCell.temp, flutemp.dCalculatedBeta,
            flutemp.oCalculatedLiqComposition,
            flutemp.oCalculatedVapComposition, state.input.pocinjec);
        leftCellOilVolumeFactorAtPerturbedPressure = flutemp.BOFunc(
            leftCell.pres * kDerivativePerturbationFactor, leftCell.temp);
        leftCellSolutionGasRatioAtPerturbedPressure = flutemp.RS(
            leftCell.pres * kDerivativePerturbationFactor, leftCell.temp);
    } // compositional case
    double leftCellSolutionGasPressureDerivative =
        (leftCellSolutionGasRatio / leftCellOilVolumeFactor - leftCellSolutionGasRatioAtPerturbedPressure / leftCellOilVolumeFactorAtPerturbedPressure) / (leftCell.pres * 0.001);
    double leftCellSolutionGasTemperatureDerivative = 0;
    if (state.input.cicloAcopTerm == 1) {
        double leftCellOilVolumeFactorAtPerturbedTemperature;
        double leftCellSolutionGasRatioAtPerturbedTemperature;
        if (useTabulatedPvt) {
            leftCellOilVolumeFactorAtPerturbedTemperature = leftCell.flui.BOFunc(
                leftCell.pres,
                leftCell.temp * kDerivativePerturbationFactor);
            leftCellSolutionGasRatioAtPerturbedTemperature = leftCell.flui.RS(
                leftCell.pres,
                leftCell.temp * kDerivativePerturbationFactor);
        } else {
            ProFlu flutemp = leftCell.flui;
            flutemp.atualizaPropComp(
                leftCell.pres,
                leftCell.temp * kDerivativePerturbationFactor,
                flutemp.dCalculatedBeta,
                flutemp.oCalculatedLiqComposition,
                flutemp.oCalculatedVapComposition, state.input.pocinjec);
            leftCellOilVolumeFactorAtPerturbedTemperature = flutemp.BOFunc(
                leftCell.pres,
                leftCell.temp * kDerivativePerturbationFactor);
            leftCellSolutionGasRatioAtPerturbedTemperature = flutemp.RS(
                leftCell.pres,
                leftCell.temp * kDerivativePerturbationFactor);
        } // compositional case
        leftCellSolutionGasTemperatureDerivative = (leftCellSolutionGasRatio / leftCellOilVolumeFactor - leftCellSolutionGasRatioAtPerturbedTemperature / leftCellOilVolumeFactorAtPerturbedTemperature) /
                                                   (leftCell.temp * 0.001);
    }

    return DistributedMassTransferProperties{
        .leftFaceWaterCut = leftFaceWaterCut,
        .leftCellLeftFaceWaterCut = leftCellLeftFaceWaterCut,
        .leftCellWaterCut = leftCellWaterCut,
        .liquidDensity = liquidDensity,
        .gasDensity = gasDensity,
        .leftFaceComposition = betI,
        .leftCellLeftFaceComposition = betL,
        .mixtureLiquidDensity = mixtureLiquidDensity,
        .leftFaceOilVolumeFactor = leftFaceOilVolumeFactor,
        .leftFaceSolutionGasRatio = leftFaceSolutionGasRatio,
        .leftFaceSolutionGasPressureDerivative = leftFaceSolutionGasPressureDerivative,
        .leftCellOilVolumeFactor = leftCellOilVolumeFactor,
        .leftCellSolutionGasRatio = leftCellSolutionGasRatio,
        .leftCellSolutionGasPressureDerivative = leftCellSolutionGasPressureDerivative,
        .leftCellSolutionGasTemperatureDerivative = leftCellSolutionGasTemperatureDerivative,
    };
}

[[nodiscard]] double lastCellSolutionGasPressureDerivative(const ThermalState &state,
                                                           int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    double cellOilVolumeFactor;
    double cellSolutionGasRatio;
    double cellOilVolumeFactorAtPerturbedPressure;
    double cellSolutionGasRatioAtPerturbedPressure;
    if (state.input.flashCompleto != 2 ||
        state.input.miniTabAtraso > 0) {
        cellOilVolumeFactor = cell.flui.BOFunc(
            cell.pres, cell.temp);
        cellSolutionGasRatio = cell.flui.RS(
            cell.pres, cell.temp);
        cellOilVolumeFactorAtPerturbedPressure = cell.flui.BOFunc(
            cell.pres * kDerivativePerturbationFactor, cell.temp);
        cellSolutionGasRatioAtPerturbedPressure = cell.flui.RS(
            cell.pres * kDerivativePerturbationFactor, cell.temp);
    } else {
        cellOilVolumeFactor = cell.flui.BOFunc(
            cell.pres, cell.temp);
        cellSolutionGasRatio = cell.flui.RS(
            cell.pres, cell.temp);
        ProFlu flutemp = cell.flui;
        flutemp.atualizaPropComp(
            cell.pres * kDerivativePerturbationFactor, cell.temp,
            flutemp.dCalculatedBeta,
            flutemp.oCalculatedLiqComposition,
            flutemp.oCalculatedVapComposition, state.input.pocinjec);
        cellOilVolumeFactorAtPerturbedPressure = flutemp.BOFunc(
            cell.pres * kDerivativePerturbationFactor, cell.temp);
        cellSolutionGasRatioAtPerturbedPressure = flutemp.RS(
            cell.pres * kDerivativePerturbationFactor, cell.temp);
    } // compositional case
    double cellSolutionGasPressureDerivative =
        (cellSolutionGasRatio / cellOilVolumeFactor - cellSolutionGasRatioAtPerturbedPressure / cellOilVolumeFactorAtPerturbedPressure) / (cell.pres * 0.001);
    return cellSolutionGasPressureDerivative;
}

DistributedMassTransferCoefficients updateDistributedMassTransferDerivatives(
    const ThermalState &state, int cellIndex, double leftCellWaterCut, const ProFlu &leftFaceFluid,
    double leftCellSolutionGasPressureDerivative, double leftCellSolutionGasTemperatureDerivative) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    const bool isLastCell = cellIndex == state.lastCell;
    double activeDerivative = 1.;
    double pressureLimit = 10;
    if (state.fullModel == 1)
        pressureLimit = 0;
    if (leftCell.pres < pressureLimit ||
        state.massTransferModel != 0)
        activeDerivative = 0.;
    double spatialCoupling = 1.;
    if (cellIndex < 2 || isLastCell)
        spatialCoupling = 0;

    double coefficientFlowArea = cell.dutoL.area;
    leftCell.ativaDeri = activeDerivative;
    leftCell.DTransDtp =
        activeDerivative * coefficientFlowArea * (1. - leftCell.alf) *
        (1. - leftCell.bet) * (1. - leftCellWaterCut) * leftFaceFluid.Deng *
        kAirDensityAtStandardConditions * leftCellSolutionGasPressureDerivative * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    cell.DTransDtpL = leftCell.DTransDtp;
    if (isLastCell) {
        const double cellSolutionGasPressureDerivative =
            lastCellSolutionGasPressureDerivative(state, cellIndex);
        cell.DTransDtp =
            activeDerivative * coefficientFlowArea * (1. - cell.alf) *
            (1. - cell.bet) * (1. - leftCellWaterCut) * leftFaceFluid.Deng *
            kAirDensityAtStandardConditions * cellSolutionGasPressureDerivative * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
    }
    if (state.input.cicloAcopTerm == 1) {
        leftCell.DTransDtT =
            activeDerivative * coefficientFlowArea * (1. - leftCell.alf) *
            (1. - leftCell.bet) * (1. - leftCellWaterCut) * leftFaceFluid.Deng *
            kAirDensityAtStandardConditions * leftCellSolutionGasTemperatureDerivative * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        cell.DTransDtTL = leftCell.DTransDtT;
        if (isLastCell) {

            (void)lastCellSolutionGasPressureDerivative(state, cellIndex);
            cell.DTransDtT =
                activeDerivative * coefficientFlowArea * (1. - cell.alf) *
                (1. - cell.bet) * (1. - leftCellWaterCut) * leftFaceFluid.Deng *
                kAirDensityAtStandardConditions * leftCellSolutionGasTemperatureDerivative * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre);
        }
    }

    return DistributedMassTransferCoefficients{
        .activeDerivative = activeDerivative,
        .spatialCoupling = spatialCoupling,
        .flowArea = coefficientFlowArea,
    };
}

void selectDistributedMassTransferModel(
    const ThermalState &state, int cellIndex, double leftAbsoluteSuperficialVelocity) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];

    leftCell.TMModel = state.massTransferModel;
    if (state.massTransferModel != 3) {
        if ((((leftCell.alf < 0.001) ||
              (leftCell.alf > kDerivativePerturbationFactor) ||
              (leftCell.bet > kDerivativePerturbationFactor &&
               leftCell.alf < kDerivativePerturbationFactor)) &&
             leftAbsoluteSuperficialVelocity < 0.1) ||
            leftCell.flui.RGO >= (*state.globals).maximumGasOilRatio)
            leftCell.TMModel = 3;
        else if (leftCell.estadoPig == 1)
            leftCell.TMModel = 3;
        else if (leftCell.acsr.tipo == kAccessoryLiquidInjection ||
                 leftCell.acsr.tipo == kAccessoryInflowPerformance ||
                 leftCell.acsr.tipo == kAccessoryLeak ||
                 leftCell.acsr.tipo == kAccessoryRadialPorous ||
                 leftCell.acsr.tipo == kAccessoryPorous2D)
            leftCell.TMModel = 3;
        else if (cellIndex >= 2) {
            if (state.cells[cellIndex - 2].acsr.tipo == kAccessoryChoke &&
                (state.cells[cellIndex - 2].acsr.chk.AreaGarg <
                 (1e-3 + state.input.master1.razareaativ) *
                     state.cells[cellIndex - 2].duto.area))
                leftCell.TMModel = 3;
            else if (state.cells[cellIndex - 2].acsr.tipo == kAccessoryPump ||
                     state.cells[cellIndex - 2].acsr.tipo == 7 ||
                     state.cells[cellIndex - 2].acsr.tipo == kAccessoryMultiPump)
                leftCell.TMModel = 0;
        }
        if (state.input.flashCompleto == 2) {
            double candidateMassFraction = leftCell.flui.FracMass(
                leftCell.pres, leftCell.temp);
            if (candidateMassFraction > 1.0 - 1e-2 || candidateMassFraction < 1e-2)
                leftCell.TMModel = 3;
        }
    }
    if (leftCell.TMModel == 0 &&
        leftCell.alf <= (*state.globals).condensationVoidFractionThreshold)
        leftCell.TMModel = 1;
    if (leftCell.TMModel == 0 && cellIndex == state.lastCell)
        leftCell.TMModel = 1;
    if (cellIndex == state.lastCell)
        cell.FonteMudaFase = 0.;
}

void snapshotMassTransferState(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    leftCell.transmassRini = leftCell.transmassR;
    leftCell.FonteMudaFaseini =
        leftCell.FonteMudaFase;
    cell.DTransDxRini = cell.DTransDxR;
    cell.DTransDxLini = cell.DTransDxL;
    cell.DTransDt1ini = cell.DTransDt1;
    cell.DTransDt0ini = cell.DTransDt0;
    cell.DTransDxRpini = cell.DTransDxRp;
    cell.DTransDxLpini = cell.DTransDxLp;
    leftCell.CoefDTLini = leftCell.CoefDTL;
    leftCell.coefTransBetini =
        leftCell.coefTransBet;
    cell.transmassLini = cell.transmassL;
}

void applyDistributedMassTransferModel(
    const ThermalState &state, int cellIndex, double leftAbsoluteSuperficialVelocity, const ProFlu &leftCellLeftFaceFluid, const ProFlu &leftFaceFluid,
    const DistributedMassTransferProperties &properties,
    const DistributedMassTransferCoefficients &coefficients, const MassTransferFaceState &previousFace) {
    const double activeDerivative = coefficients.activeDerivative;
    const double coefficientFlowArea = coefficients.flowArea;
    const double leftCellLeftFaceOilVolumeFactor = previousFace.oilVolumeFactor;
    const double leftCellLeftFaceSolutionGasRatio = previousFace.solutionGasRatio;
    const double leftFaceWaterCut = properties.leftFaceWaterCut;
    const double leftCellLeftFaceWaterCut = properties.leftCellLeftFaceWaterCut;
    const double leftCellWaterCut = properties.leftCellWaterCut;
    const double betI = properties.leftFaceComposition;
    const double betL = properties.leftCellLeftFaceComposition;
    const double leftFaceOilVolumeFactor = properties.leftFaceOilVolumeFactor;
    const double leftFaceSolutionGasRatio = properties.leftFaceSolutionGasRatio;
    const double leftCellOilVolumeFactor = properties.leftCellOilVolumeFactor;
    const double leftCellSolutionGasRatio = properties.leftCellSolutionGasRatio;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    selectDistributedMassTransferModel(state, cellIndex, leftAbsoluteSuperficialVelocity);

    leftCell.fontedissolv = 0.;

    snapshotMassTransferState(state, cellIndex);

    cell.TMModelL = leftCell.TMModel;
    leftCell.FonteMudaFase = 0.;
    if (!(leftCell.TMModel == 0 || leftCell.TMModel == 1))
        return;

    leftCell.transmassR =
        -(cell.QL * (1 - betI) * (leftFaceFluid.rDgD) * leftFaceFluid.Deng *
          kAirDensityAtStandardConditions * (1. - leftFaceWaterCut) * leftFaceSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftFaceOilVolumeFactor) +
        (leftCell.QL * (1 - betL) * (leftCellLeftFaceFluid.rDgD) *
         leftCellLeftFaceFluid.Deng * kAirDensityAtStandardConditions * (1. - leftCellLeftFaceWaterCut) * leftCellLeftFaceSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) /
         leftCellLeftFaceOilVolumeFactor);

    leftCell.transmassR /= leftCell.dx;
    leftCell.transmassR += leftCell.fontedissolv;
    cell.transmassL = leftCell.transmassR;
    leftCell.FonteMudaFase =
        leftCell.transmassR -
        leftCell.DTransDtp * leftCell.d2pdt2 -
        leftCell.DTransDtT * leftCell.dTdtIni;
    if (leftCell.TMModel == 1) {
        leftCell.transmassR -=
            activeDerivative * ((1. - leftCell.bet) * (1. - leftCell.alf) * (1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) * leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor) /
            leftCell.dt;
        leftCell.transmassR +=
            activeDerivative * ((1. - leftCell.betini) * (1. - leftCell.alfini) * (1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) * leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor) /
            leftCell.dt;

        cell.transmassL = leftCell.transmassR;
        leftCell.FonteMudaFase =
            leftCell.transmassR;
    }
    if (leftCell.TMModel == 0) {
        leftCell.FonteMudaFase -=
            activeDerivative * ((1. - leftCell.bet) * (1. - leftCell.alf) * (1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) * leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor) /
            leftCell.dt;
        leftCell.FonteMudaFase +=
            activeDerivative * ((1. - leftCell.betini) * (1. - leftCell.alfini) * (1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) * leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor) /
            leftCell.dt;
    }

    if (leftCell.TMModel == 0) {
        updateEquilibriumMassTransferDerivatives(
            state, cellIndex, leftCellLeftFaceFluid, leftFaceFluid, properties, coefficients, previousFace);
    } else {
        cell.DTransDxR = 0.;
        cell.DTransDxL = 0.;
        cell.DTransDt1 = 0.;
        cell.DTransDxRp = 0.;
        cell.DTransDxLp = 0.;
        if (state.input.desligaDeriTransMassDTemp == 1) {
            leftCell.DTransDtT = 0;
            cell.DTransDtTL = 0.;
        }
        leftCell.CoefDTR = 0.;
        leftCell.CoefDTL = 0.;
        leftCell.coefTransBet = 0.;
    }
}

void clearMassTransferDerivatives(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    cell.DTransDxR = 0.;
    cell.DTransDxL = 0.;
    cell.DTransDt1 = 0.;
    cell.DTransDt0 = 0.;
    cell.DTransDxRp = 0.;
    cell.DTransDxLp = 0.;
    leftCell.CoefDTR = 0.;
    leftCell.CoefDTL = 0.;
    leftCell.coefTransBet = 0.;
    if (state.input.desligaDeriTransMassDTemp == 1) {
        leftCell.DTransDtT = 0;
        cell.DTransDtTL = 0.;
    }
}

[[nodiscard]] UpstreamFaceBasis upstreamFaceBasisOf(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double gasDensity;
    double flowArea;
    if (cell.QG >= 0) {
        flowArea = cell.dutoL.area;
        gasDensity = leftCell.rgCi;
    } else {
        gasDensity = cell.rgCi;
        flowArea = cell.duto.area;
    }
    return UpstreamFaceBasis{.gasDensity = gasDensity,
                             .flowArea = flowArea};
}

void setFlowPartitionTerms(const ThermalState &state, int cellIndex, double term1,
                           int &branchFlag, int flagValue, bool recordInterfaceFraction) {
    state.cells[cellIndex].term1 = term1;
    state.cells[cellIndex].term2 = 0.;
    if (recordInterfaceFraction)
        state.cells[cellIndex].alfI = term1 == 1. ? 0. : 1.;
    branchFlag = flagValue;
}

void clearDriftClosure(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    cell.c0 = 1;
    cell.ud = 0;
    cell.arranjo = 0;
}

void selectAndApplyInteriorFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, double superficialGasVelocity,
    double superficialLiquidVelocity, double leftSuperficialGasVelocity, double leftSuperficialLiquidVelocity, double rightSuperficialGasVelocity, double rightSuperficialLiquidVelocity,
    double gasDensity, double liquidDensity, double flowArea, int explicitMode) {
    SimulationContext &globals = *state.globals;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    Cel &rightCell = state.cells[cellIndex + 1];
    const bool noGasSourceOnEitherFace =
        cell.fontemassGL <= globals.smallValueTolerance * 1e-5 && cell.fontemassGR <= globals.smallValueTolerance * 1e-5;
    const bool noLiquidSourceOnEitherFace =
        (cell.fontemassLL + cell.fontemassCL) <= globals.smallValueTolerance * 1e-5 && (cell.fontemassLR + cell.fontemassCR) <= globals.smallValueTolerance * 1e-5;
    bif[cellIndex] = 1;

    if (leftCell.alfPigD <= globals.smallValueTolerance && cell.alfPigE <= globals.smallValueTolerance && noGasSourceOnEitherFace) {
        setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
    } else if (leftCell.alfPigD >= (1. - globals.smallValueTolerance) && cell.alfPigE >= (1. - globals.smallValueTolerance) && (noLiquidSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
    } else if (leftCell.acsr.tipo == kAccessoryChoke && leftCell.acsr.chk.AreaGarg <= (1e-3)) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
    }

    else if (superficialGasVelocity >= 0 && leftCell.alfPigD <= globals.smallValueTolerance && (noGasSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfPigE > (1. - globals.smallValueTolerance) && superficialLiquidVelocity < 0 && leftSuperficialLiquidVelocity < 0 && cell.duto.teta > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfPigE > (1. - 10 * globals.smallValueTolerance) && cell.duto.teta < 0) {
            setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 1, true);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alf >= rightCell.alf && rightSuperficialGasVelocity < 0) {
            bif[cellIndex] = 1;
        }
    } else if (superficialGasVelocity <= 0 && cell.alfPigE <= globals.smallValueTolerance && (noGasSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
        if (fabs(superficialGasVelocity) <= 1e-15 && leftCell.alfPigD > (1. - globals.smallValueTolerance) && superficialLiquidVelocity > 0 && rightSuperficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfL >= leftCell.alfL && leftSuperficialGasVelocity > 0) {
            bif[cellIndex] = 1;
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && fabs(superficialLiquidVelocity) <= 1e-15 && cell.alf < leftCell.alf && cell.duto.teta > 0) {
            bif[cellIndex] = 1;
        }
    } else if (superficialLiquidVelocity >= 0 && leftCell.alfPigD >= 1. - 1 * globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
        if (fabs(superficialLiquidVelocity) <= 1e-15 && cell.alfPigE < globals.smallValueTolerance && rightSuperficialLiquidVelocity < 0) {
            setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        } else if (fabs(rightSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.alfPigE < globals.smallValueTolerance && cell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
            }
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.alfPigE < globals.smallValueTolerance && cell.duto.teta >= 0) {
                bif[cellIndex] = 1;
            }
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.alfPigE < globals.smallValueTolerance && cell.duto.teta < 0) {
                setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
            }
        } else if ((fabs(superficialLiquidVelocity) < 1e-15 && (rightSuperficialLiquidVelocity < 0 || cell.duto.teta > 0) && ((cell.alfPigE <= (1 - 10 * globals.smallValueTolerance + .0 * cell.alfPigER) && cell.alfPigER < 1 - 1 * globals.smallValueTolerance) || cell.alfPigE <= 0.7)))
            bif[cellIndex] = 1;

    } else if (superficialLiquidVelocity <= 0 && cell.alfPigE >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
        if (fabs(superficialLiquidVelocity) <= globals.smallValueTolerance * 1e-5 && leftCell.alfPigD < globals.smallValueTolerance && leftSuperficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        }

        if (fabs(leftSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftCell.alfPigD < globals.smallValueTolerance && leftCell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
            }
        } else if ((cellIndex > 1 && fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftSuperficialLiquidVelocity > 0 && ((leftCell.alfPigD <= 1.0 * state.cells[cellIndex - 2].alfPigD && state.cells[cellIndex - 2].alfPigD < 0.99) || leftCell.alfPigD < 0.7)) && superficialGasVelocity >= 0.)
            bif[cellIndex] = 1;
        else {
            if ((fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftSuperficialLiquidVelocity > 0 && ((leftCell.alfPigD <= 1.0 * leftCell.alfL) || leftCell.alfPigD < 0.7)) && superficialGasVelocity >= 0)
                bif[cellIndex] = 1;
            else if ((fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.duto.teta >= 0.95 * std::numbers::pi / 2. && ((leftCell.alfPigD <= 1.0 * leftCell.alfL) || leftCell.alfPigD < 0.7)) && superficialGasVelocity > 0)
                bif[cellIndex] = 1;
        }
    }

    if (bif[cellIndex] == 1) {
        applyInteriorDriftPartition(
            state, dependencies, cellIndex, superficialGasVelocity, superficialLiquidVelocity, gasDensity, liquidDensity, flowArea, explicitMode);
    }
}

void updateInteriorFlowPartitionCell(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, Vcr<int> &valv, int explicitMode) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    Cel &rightCell = state.cells[cellIndex + 1];
    const bool hasRightCell = cellIndex < state.lastCell;
    if (hasRightCell) {
        leftCell.alfR = rightCell.alfL = cell.alf;
        leftCell.betR = rightCell.betL = cell.bet;
    } else {
        leftCell.alfR = cell.alf;
        leftCell.betR = cell.bet;
    }
    valv[cellIndex] = 1;
    if (leftCell.acsr.tipo == kAccessoryChoke || leftCell.acsr.tipo == kAccessoryVolumetricPump) {
        if ((*cell.acsrL).tipo == kAccessoryChoke && (*cell.acsrL).chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * leftCell.duto.area)
            valv[cellIndex] = 0;
        if ((*cell.acsrL).tipo == kAccessoryVolumetricPump && fabs((*cell.acsrL).bvol.freq) > 1)
            valv[cellIndex] = 0;
    }
    if (valv[cellIndex] != 1) {
        cell.c0 = 1.;
        cell.ud = 0.;
        cell.term1 = 0.;
        cell.term2 = 0.;
        cell.alfI = 1.;
        cell.term1L = leftCell.term1;
        cell.term2L = leftCell.term2;
        leftCell.term1R = cell.term1;
        leftCell.term2R = cell.term2;
        return;
    }

    double betI;
    double liquidDensity;
    if (cell.QL < 0.) {
        betI = cell.betPigE;
        liquidDensity = (1 - betI) * cell.rpCi + betI * cell.rcCi;
    } else {
        betI = leftCell.betPigD;
        liquidDensity = (1 - betI) * leftCell.rpCi + betI * leftCell.rcCi;
    }
    const UpstreamFaceBasis upstream = upstreamFaceBasisOf(state, cellIndex);
    const double gasDensity = upstream.gasDensity;
    const double flowArea = upstream.flowArea;
    double superficialGasVelocity = cell.QG / (flowArea);
    double superficialLiquidVelocity = cell.QL / (flowArea);

    double leftFlowArea = cell.dutoL.area;
    double betIL;
    if (cellIndex < 2)
        betIL = leftCell.betL;
    else
        betIL = state.cells[cellIndex - 2].betPigD;
    if (leftCell.QL < 0.)
        betIL = leftCell.betPigE;
    double leftGasDensity = cell.rgLi;
    double leftLiquidDensity = (1 - betIL) * cell.rpLi + betIL * cell.rcLi;
    double leftSuperficialGasVelocity = (cell.ML - cell.MliqiniL) / (leftGasDensity * leftFlowArea);
    double leftSuperficialLiquidVelocity = (cell.MliqiniL) / (leftLiquidDensity * leftFlowArea);

    double rightFlowArea = cell.dutoR.area;
    double betIR = cell.betPigD;
    if (cell.QLR < 0.) {
        if (cellIndex > state.lastCell - 2)
            betIR = cell.betR;
        else
            betIR = rightCell.betPigE;
    }
    double rightGasDensity = cell.rgRi;
    double rightLiquidDensity = (1 - betIR) * cell.rpRi + betIR * cell.rcRi;
    double rightSuperficialGasVelocity = (cell.MR - cell.MliqiniR) / (rightGasDensity * rightFlowArea);
    double rightSuperficialLiquidVelocity = (cell.MliqiniR) / (rightLiquidDensity * rightFlowArea);

    selectAndApplyInteriorFlowRegime(
        state, dependencies, cellIndex, bif, superficialGasVelocity, superficialLiquidVelocity, leftSuperficialGasVelocity, leftSuperficialLiquidVelocity, rightSuperficialGasVelocity, rightSuperficialLiquidVelocity,
        gasDensity, liquidDensity, flowArea, explicitMode);
}

void updateOutletBoundaryFlowPartition(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, int explicitMode) {
    SimulationContext &globals = *state.globals;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    const bool noLiquidSourceOnEitherFace =
        (cell.fontemassLL + cell.fontemassCL) <= globals.smallValueTolerance * 1e-5 && (cell.fontemassLR + cell.fontemassCR) <= globals.smallValueTolerance * 1e-5;
    state.cells[state.lastCell - 1].alfR = state.cells[state.lastCell].alf;
    state.cells[state.lastCell].alfR = state.cells[state.lastCell].alf;
    state.cells[state.lastCell - 1].betR = state.cells[state.lastCell].bet;
    state.cells[state.lastCell].betR = state.cells[state.lastCell].bet;

    double lengthRatio = cell.dxL / (cell.dx + cell.dxL);
    double meanPressure = cell.presaux;
    double meanTemperature = cell.temp * lengthRatio + leftCell.temp * (1. - lengthRatio);
    double betI = cell.betL;
    if (cell.QL < 0.)
        betI = cell.bet;
    double gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
    double liquidDensity = (1 - betI) * cell.flui.MasEspLiq(meanPressure, meanTemperature) + betI * cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
    double flowArea = cell.duto.area;
    if (cell.MC >= 0)
        flowArea = cell.dutoL.area;
    double superficialGasVelocity = cell.QG / (flowArea);
    double superficialLiquidVelocity = cell.QL / (flowArea);

    double leftFlowArea = cell.dutoL.area;
    double leftLengthRatio = leftCell.dxL / (leftCell.dx + leftCell.dxL);
    double leftMeanPressure = leftCell.presaux;
    double leftMeanTemperature = leftCell.temp * leftLengthRatio + leftCell.tempL * (1. - leftLengthRatio);
    double betIL = leftCell.betL;
    if (leftCell.QL < 0.)
        betIL = leftCell.bet;
    double leftLiquidDensity = (1 - betIL) * cell.flui.MasEspLiq(leftMeanPressure, leftMeanTemperature) + betIL * cell.fluicol.MasEspFlu(leftMeanPressure, leftMeanTemperature);
    double leftSuperficialLiquidVelocity = (cell.MliqiniL) / (leftLiquidDensity * leftFlowArea);

    double rightFlowArea = cell.dutoR.area;
    double rightLengthRatio = cell.dxR / (cell.dxR + cell.dx);
    double rightMeanPressure = cell.pres;
    double rightMeanTemperature = cell.temp * rightLengthRatio + cell.tempR * (1. - rightLengthRatio);
    double betIR = cell.bet;
    double rightLiquidDensity = (1 - betIR) * cell.flui.MasEspLiq(rightMeanPressure, rightMeanTemperature) + betIR * cell.fluicol.MasEspFlu(rightMeanPressure, rightMeanTemperature);
    double rightSuperficialLiquidVelocity = (cell.MliqiniR) / (rightLiquidDensity * rightFlowArea);

    bif[cellIndex] = 1;

    if (cell.alfL <= globals.smallValueTolerance && cell.alf <= globals.smallValueTolerance && cell.fontemassGL <= 0 && cell.fontemassGR <= 0) {
        setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        cell.c0 = 1. - globals.smallValueTolerance;
        cell.ud = 0.;
        cell.arranjo = 0;
    } else if (cell.alfL >= (1. - globals.smallValueTolerance) && cell.alf >= (1. - globals.smallValueTolerance) && cell.fontemassLL <= 0 && cell.fontemassLR <= 0) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        cell.c0 = 1. - globals.smallValueTolerance;
        cell.ud = 0.;
        cell.arranjo = 0;
    } else if (superficialGasVelocity > 0 && cell.alfL <= globals.smallValueTolerance && (cell.fontemassGL <= 0. && cell.fontemassGR <= 0.)) {
        setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        cell.c0 = 1. - globals.smallValueTolerance;
        cell.ud = 0.;
        cell.arranjo = 0;
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alf > (1. - globals.smallValueTolerance) && superficialLiquidVelocity < 0 && leftSuperficialLiquidVelocity < 0) {
            setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        }
    } else if (superficialGasVelocity < 0 && cell.alf <= globals.smallValueTolerance && (cell.fontemassGL <= 0. && cell.fontemassGR <= 0.)) {
        setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        cell.c0 = 1. - globals.smallValueTolerance;
        cell.ud = 0.;
        cell.arranjo = 0;
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfL > (1. - globals.smallValueTolerance) && superficialLiquidVelocity > 0 && rightSuperficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        }
    } else if (superficialLiquidVelocity >= 0 && leftCell.alf >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
        if (fabs(superficialLiquidVelocity) <= 1e-15 && cell.alf < globals.smallValueTolerance && rightSuperficialLiquidVelocity < 0) {
            setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        } else if (fabs(rightSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.alf < globals.smallValueTolerance && cell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
            }
        } else if (fabs(superficialLiquidVelocity) < 1e-15 && rightSuperficialLiquidVelocity < 0 && cell.alf <= (1 - 1 * globals.smallValueTolerance))
            bif[cellIndex] = 1;
    }

    else if (superficialLiquidVelocity <= 0 && cell.alf >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        setFlowPartitionTerms(state, cellIndex, 0., bif[cellIndex], 0, true);
        clearDriftClosure(state, cellIndex);
        if (fabs(superficialLiquidVelocity) <= globals.smallValueTolerance * 1e-5 && leftCell.alf < globals.smallValueTolerance && leftSuperficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
        }
        if (fabs(leftSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftCell.alf < globals.smallValueTolerance && leftCell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., bif[cellIndex], 0, true);
            }
        } else if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftSuperficialLiquidVelocity > 0 && ((leftCell.alfPigD <= 1.0 * state.cells[cellIndex - 2].alfPigD && state.cells[cellIndex - 2].alfPigD < 0.99) || leftCell.alfPigD < 0.7))
            bif[cellIndex] = 1;
        else if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftSuperficialLiquidVelocity > 0 && ((leftCell.alfPigD <= 1.0 * leftCell.alfL) || leftCell.alfPigD < 0.7))
            bif[cellIndex] = 1;
    }

    if (bif[cellIndex] == 1) {
        const double meanVoidFraction = cell.alfL;
        cell.alfI = meanVoidFraction;
        double c0;
        double ud;
        if (explicitMode == 0)
            driftflux::coefficient::instantaneous(dependencies.closure, cellIndex, c0, ud);
        else {
            c0 = 1.;
            ud = 0.;
        }
        if (state.input.escorregamentoCelulaContorno == 0) {
            c0 = 1.;
            ud = 0.;
        }
        cell.c0 = c0;
        cell.ud = ud;
        double numerator = (1. - meanVoidFraction * c0);
        double denominator = 1. + meanVoidFraction * (gasDensity / liquidDensity) * c0 - meanVoidFraction * c0;
        if (fabs(denominator) > 1e-15) {
            cell.term1 = numerator / denominator;
            cell.term2 = (-flowArea * meanVoidFraction * gasDensity * ud) / denominator;
        } else {
            cell.term1 = meanVoidFraction < 0.5 ? 1. : 0.;
            cell.term2 = 0.;
        }
    }

    cell.term1L = leftCell.term1;
    cell.term2L = leftCell.term2;
    leftCell.term1R = cell.term1;
    leftCell.term2R = cell.term2;
}

void blendNeighbourDriftClosure(const ThermalState &state, int cellIndex,
                                int neighbourIndex, int blendIndex,
                                const Vcr<int> &bif,
                                Vcr<double> &c0V, Vcr<double> &udV) {
    double inclinationAngle;
    double leftInclinationAngle;
    inclinationAngle = state.cells[cellIndex].duto.teta;
    leftInclinationAngle = state.cells[neighbourIndex].duto.teta;
    if (((inclinationAngle != leftInclinationAngle) && bif[blendIndex] != 0) ||
        (state.cells[cellIndex].arranjo != state.cells[blendIndex].arranjo && bif[blendIndex] != 0)) {
        c0V[cellIndex] = (state.cells[cellIndex].dx * state.cells[cellIndex].c0 + state.cells[neighbourIndex].dx * state.cells[neighbourIndex].c0) / (state.cells[cellIndex].dx + state.cells[neighbourIndex].dx);
        if (inclinationAngle * leftInclinationAngle >= 0)
            udV[cellIndex] = (state.cells[cellIndex].dx * state.cells[cellIndex].ud + state.cells[neighbourIndex].dx * state.cells[neighbourIndex].ud) / (state.cells[cellIndex].dx + state.cells[neighbourIndex].dx);
    }
}

void finalizeFlowPartitionTerms(
    const ThermalState &state, const Vcr<int> &bif, const Vcr<int> &valv) {
    int isShutIn = 0;
    for (int cellIndex = 1; cellIndex < state.lastCell; cellIndex++) {
        Cel &cell = state.cells[cellIndex];
        if (cell.acsr.tipo == kAccessoryChoke && cell.acsr.chk.AreaGarg <= 1e-15 * cell.acsr.chk.AreaTub)
            isShutIn = 1;
        else if (state.surfaceChoke.AreaGarg <= 1.e-15 * state.surfaceChoke.AreaTub)
            isShutIn = 1;
    }

    Vcr<double> c0V(state.lastCell, 0.);
    Vcr<double> udV(state.lastCell, 0.);
    for (int cellIndex = 1; cellIndex < state.lastCell; cellIndex++) {
        Cel &cell = state.cells[cellIndex];
        c0V[cellIndex] = cell.c0;
        udV[cellIndex] = cell.ud;
        if (bif[cellIndex] == 1 && valv[cellIndex] == 1 && cellIndex > 2) {
            int neighborIndex = cellIndex - 1;
            int secondNeighborIndex = cellIndex - 2;
            if (cell.QG < 0) {
                neighborIndex = cellIndex + 1;
                secondNeighborIndex = cellIndex;
            }
            if ((state.cells[neighborIndex].acsr.tipo == kAccessoryNone && (state.cells[secondNeighborIndex].acsr.tipo != kAccessoryChoke || state.cells[secondNeighborIndex].acsr.chk.AreaGarg > (1e-3))) &&
                (cell.arranjo != state.cells[neighborIndex].arranjo && bif[neighborIndex] != 0)) {
                c0V[cellIndex] = (cell.dx * cell.c0 + state.cells[neighborIndex].dx * state.cells[neighborIndex].c0) / (cell.dx + state.cells[neighborIndex].dx);
                if (cell.duto.teta * state.cells[neighborIndex].duto.teta >= 0)
                    udV[cellIndex] = (cell.dx * cell.ud + state.cells[neighborIndex].dx * state.cells[neighborIndex].ud) / (cell.dx + state.cells[neighborIndex].dx);
            } else if (cellIndex > 2) {
                if ((state.cells[cellIndex - 1].acsr.tipo == kAccessoryNone && (state.cells[cellIndex - 2].acsr.tipo != kAccessoryChoke || state.cells[cellIndex - 2].acsr.chk.AreaGarg > (1e-3))) &&
                    cell.QG >= 0) {
                    blendNeighbourDriftClosure(state, cellIndex, cellIndex - 1,
                                               neighborIndex, bif, c0V, udV);
                } else if ((state.cells[cellIndex + 1].acsr.tipo == kAccessoryNone && (cell.acsr.tipo != kAccessoryChoke || cell.acsr.chk.AreaGarg > (1e-3))) &&
                           cell.QG < 0) {
                    blendNeighbourDriftClosure(state, cellIndex, cellIndex + 1,
                                               neighborIndex, bif, c0V, udV);
                }
            }
        }
    }

    for (int cellIndex = 1; cellIndex < state.lastCell; cellIndex++) {
        finalizeFlowPartitionCell(state, cellIndex, isShutIn, bif, valv);
    }
    for (int cellIndex = 1; cellIndex <= state.lastCell; cellIndex++) {
        Cel &cell = state.cells[cellIndex];
        Cel &leftCell = state.cells[cellIndex - 1];
        cell.term1L = leftCell.term1;
        cell.term2L = leftCell.term2;
        leftCell.term1R = cell.term1;
        leftCell.term2R = cell.term2;
        leftCell.alfRI = cell.alfI;
        leftCell.c0R = cell.c0;
        leftCell.udR = cell.ud;
    }

    state.cells[state.lastCell].alfRI = state.cells[state.lastCell].alfI;
    state.cells[state.lastCell].c0R = state.cells[state.lastCell].c0;
    state.cells[state.lastCell].udR = state.cells[state.lastCell].ud;
}

void applyNoSlipClosure(const ThermalState &state, int cellIndex, double term1,
                        int &branchFlag, double distributionCoefficient,
                        double driftVelocity, bool recordInterfaceFraction) {
    Cel &cell = state.cells[cellIndex];
    cell.term1 = term1;
    cell.term2 = 0.;
    if (recordInterfaceFraction)
        cell.alfI = term1 == 1. ? 0. : 1.;
    branchFlag = 0;
    cell.c0 = 1 + 0 * distributionCoefficient;
    cell.ud = 0 * driftVelocity;
    cell.arranjo = 0;
}

inline void InletBoundaryRegime::applyInletOverrides(const ThermalState &state, int cellIndex,
                                                     int &branchFlag, double superficialGasVelocity,
                                                     double superficialLiquidVelocity) {
    SimulationContext &globals = *state.globals;
    if (superficialLiquidVelocity >= 0 && state.inletVoidFraction > 1 - globals.smallValueTolerance) {
        setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 0, true);
    }
    if (superficialGasVelocity >= 0 && state.inletVoidFraction < globals.smallValueTolerance) {
        setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0, true);
    }
}

inline void InletBoundaryRegime::updateClosure(const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double &c0,
                                               double &ud) {
    driftflux::coefficient::initialization(dependencies.closure, cellIndex, c0, ud);
    state.cells[cellIndex].c0 = c0;
    state.cells[cellIndex].ud = ud;
}

inline void BufferedInletRegime::applyInletOverrides(const ThermalState &, int, int &, double, double) {}

inline void BufferedInletRegime::updateClosure(const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double &c0,
                                               double &ud) {
    driftflux::coefficient::bufferedInitialization(dependencies.closure, cellIndex, c0, ud);
}

template <typename Regime>
void selectAndApplyInletRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, int &branchFlag, double distributionCoefficient,
    double driftVelocity, double superficialGasVelocity, double superficialLiquidVelocity, double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea, int explicitMode) {
    SimulationContext &globals = *state.globals;
    Cel &cell = state.cells[cellIndex];
    const bool noGasSourceOnEitherFace =
        cell.fontemassGL <= globals.smallValueTolerance * 1e-5 && cell.fontemassGR <= globals.smallValueTolerance * 1e-5;
    const bool noLiquidSourceOnEitherFace =
        (cell.fontemassLL + cell.fontemassCL) <= globals.smallValueTolerance * 1e-5 && (cell.fontemassLR + cell.fontemassCR) <= globals.smallValueTolerance * 1e-5;
    branchFlag = 1;

    if (state.inletVoidFraction < globals.smallValueTolerance && cell.alfPigE <= globals.smallValueTolerance && noGasSourceOnEitherFace) {
        applyNoSlipClosure(state, cellIndex, 1., branchFlag, distributionCoefficient,
                           driftVelocity, Regime::recordsInterfaceFraction);
    } else if (state.inletVoidFraction >= (1. - globals.smallValueTolerance) && cell.alfPigE >= (1. - globals.smallValueTolerance) && (noLiquidSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity, Regime::recordsInterfaceFraction);
    } else if (superficialGasVelocity >= 0 && state.inletVoidFraction <= globals.smallValueTolerance && (noGasSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 1., branchFlag, distributionCoefficient,
                           driftVelocity, Regime::recordsInterfaceFraction);
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfPigE > (1. - globals.smallValueTolerance) && superficialLiquidVelocity < 0 && cell.duto.teta > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 0, Regime::recordsInterfaceFraction);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfPigE > (1. - 1 * globals.smallValueTolerance) && cell.duto.teta < 0 && superficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 1, Regime::recordsInterfaceFraction);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alf >= state.inletVoidFraction && superficialLiquidVelocity < 0) {
            branchFlag = 1;
        }
    } else if (superficialGasVelocity <= 0 && cell.alfPigE <= globals.smallValueTolerance && (noGasSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 1., branchFlag, distributionCoefficient,
                           driftVelocity, Regime::recordsInterfaceFraction);
        if (fabs(superficialGasVelocity) <= 1e-15 && state.inletVoidFraction > (1. - globals.smallValueTolerance) && superficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 0, Regime::recordsInterfaceFraction);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && state.inletVoidFraction > globals.smallValueTolerance && superficialLiquidVelocity > 0) {
            branchFlag = 1;
        }
    } else if (superficialLiquidVelocity >= 0 && state.inletVoidFraction >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity, Regime::recordsInterfaceFraction);
        if (fabs(superficialLiquidVelocity) <= 1e-15 && cell.alfPigE < globals.smallValueTolerance && rightSuperficialLiquidVelocity < 0) {
            setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0, Regime::recordsInterfaceFraction);
        } else if (fabs(rightSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.alfPigE < globals.smallValueTolerance && cell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0, Regime::recordsInterfaceFraction);
            }
        } else if (fabs(superficialLiquidVelocity) < 1e-15 && rightSuperficialLiquidVelocity < 0 && ((cell.alfPigE <= (1 - 1 * globals.smallValueTolerance + .0 * cell.alfPigER) && cell.alfPigER < 1 - 1 * globals.smallValueTolerance) || cell.alfPigE <= 0.7))
            branchFlag = 1;

    } else if (superficialLiquidVelocity <= 0 && cell.alfPigE >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity, Regime::recordsInterfaceFraction);
        if (fabs(superficialLiquidVelocity) <= globals.smallValueTolerance * 1e-5 && state.inletVoidFraction < globals.smallValueTolerance && rightSuperficialLiquidVelocity < 0) {
            setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0, Regime::recordsInterfaceFraction);
        }

        if (fabs(rightSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && state.inletVoidFraction < globals.smallValueTolerance) {
                setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0, Regime::recordsInterfaceFraction);
            }
        } else if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && superficialGasVelocity < 0 && (state.inletVoidFraction < 0.7))
            branchFlag = 1;
    }
    if (superficialLiquidVelocity > 0 && fabs(superficialGasVelocity) <= globals.smallValueTolerance * 1e-5 && state.inletVoidFraction > globals.smallValueTolerance && state.inletVoidFraction < 1 - globals.smallValueTolerance)
        branchFlag = 1;
    if (superficialGasVelocity > 0 && fabs(superficialLiquidVelocity) <= globals.smallValueTolerance * 1e-5 && state.inletVoidFraction > globals.smallValueTolerance && state.inletVoidFraction < 1 - globals.smallValueTolerance)
        branchFlag = 1;
    Regime::applyInletOverrides(state, cellIndex, branchFlag,
                                superficialGasVelocity, superficialLiquidVelocity);

    if (branchFlag == 1) {

        double c0;
        double ud;
        const double meanVoidFraction = superficialGasVelocity < 0 ? cell.alfPigE : state.inletVoidFraction;
        if constexpr (Regime::recordsInterfaceFraction)
            cell.alfI = cell.alfPigE;
        if (explicitMode == 0)
            Regime::updateClosure(state, dependencies, cellIndex, c0, ud);
        else {
            c0 = 1.;
            ud = 0.;
            cell.c0 = c0;
            cell.ud = ud;
        }
        double numerator = (1. - meanVoidFraction * c0);
        double denominator = 1 + c0 * meanVoidFraction * ((gasDensity / liquidDensity) - 1.);
        if (fabs(denominator) > 1e-15) {
            cell.term1 = numerator / denominator;
            cell.term2 = (-flowArea * meanVoidFraction * gasDensity * ud) / denominator;
        } else {
            cell.term1 = meanVoidFraction < 0.5 ? 1. : 0.;
            cell.term2 = 0.;
        }
    }
}

void selectAndApplyInletBoundaryFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bif, double distributionCoefficient,
    double driftVelocity, double superficialGasVelocity, double superficialLiquidVelocity, double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea, int explicitMode) {
    selectAndApplyInletRegime<InletBoundaryRegime>(
        state, dependencies, cellIndex, bif[cellIndex], distributionCoefficient, driftVelocity,
        superficialGasVelocity, superficialLiquidVelocity, rightSuperficialLiquidVelocity,
        gasDensity, liquidDensity, flowArea, explicitMode);
}

void selectAndApplyBufferedOutletFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double distributionCoefficient, double driftVelocity,
    double superficialGasVelocity, double superficialLiquidVelocity, double leftSuperficialGasVelocity, double leftSuperficialLiquidVelocity, double rightSuperficialGasVelocity,
    double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity,
    double flowArea) {
    SimulationContext &globals = *state.globals;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    Cel &rightCell = state.cells[cellIndex + 1];
    const bool noGasSourceOnEitherFace =
        cell.fontemassGL <= globals.smallValueTolerance * 1e-5 && cell.fontemassGR <= globals.smallValueTolerance * 1e-5;
    const bool noLiquidSourceOnEitherFace =
        (cell.fontemassLL + cell.fontemassCL) <= globals.smallValueTolerance * 1e-5 && (cell.fontemassLR + cell.fontemassCR) <= globals.smallValueTolerance * 1e-5;
    int branchFlag = 1;

    if (leftCell.alfPigD <= globals.smallValueTolerance && cell.alfPigE <= globals.smallValueTolerance && noGasSourceOnEitherFace) {
        applyNoSlipClosure(state, cellIndex, 1., branchFlag, distributionCoefficient,
                           driftVelocity);
    } else if (leftCell.alfPigD >= (1. - globals.smallValueTolerance) && cell.alfPigE >= (1. - globals.smallValueTolerance) && (noLiquidSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity);
    } else if (leftCell.acsr.tipo == kAccessoryChoke && leftCell.acsr.chk.AreaGarg <= (1e-3)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity);
    }

    else if (superficialGasVelocity >= 0 && leftCell.alfPigD <= globals.smallValueTolerance && (noGasSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 1., branchFlag, distributionCoefficient,
                           driftVelocity);
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfPigE > (1. - globals.smallValueTolerance) && superficialLiquidVelocity < 0 && leftSuperficialLiquidVelocity < 0 && cell.duto.teta > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 0);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfPigE > (1. - 10 * globals.smallValueTolerance) && cell.duto.teta < 0) {
            setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 1);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alf >= rightCell.alf && rightSuperficialGasVelocity < 0) {
            branchFlag = 1;
        }
    } else if (superficialGasVelocity <= 0 && cell.alfPigE <= globals.smallValueTolerance && (noGasSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 1., branchFlag, distributionCoefficient,
                           driftVelocity);
        if (fabs(superficialGasVelocity) <= 1e-15 && leftCell.alfPigD > (1. - globals.smallValueTolerance) && superficialLiquidVelocity > 0 && rightSuperficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 0., branchFlag, 0);
        }
        if (fabs(superficialGasVelocity) <= 1e-15 && cell.alfL >= leftCell.alfL && leftSuperficialGasVelocity > 0) {
            branchFlag = 1;
        }
    } else if (superficialLiquidVelocity >= 0 && leftCell.alfPigD >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity);
        if (fabs(superficialLiquidVelocity) <= 1e-15 && cell.alfPigE < globals.smallValueTolerance && rightSuperficialLiquidVelocity < 0) {
            setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0);
        } else if (fabs(rightSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && cell.alfPigE < globals.smallValueTolerance && cell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0);
            }
        } else if (fabs(superficialLiquidVelocity) < 1e-15 && rightSuperficialLiquidVelocity < 0 && ((cell.alfPigE <= (1 - 1 * globals.smallValueTolerance + .0 * cell.alfPigER) && cell.alfPigER < 1 - 1 * globals.smallValueTolerance) || cell.alfPigE <= 0.7))
            branchFlag = 1;

    } else if (superficialLiquidVelocity <= 0 && cell.alfPigE >= 1. - globals.smallValueTolerance && (noLiquidSourceOnEitherFace)) {
        applyNoSlipClosure(state, cellIndex, 0., branchFlag, distributionCoefficient,
                           driftVelocity);
        if (fabs(superficialLiquidVelocity) <= globals.smallValueTolerance * 1e-5 && leftCell.alfPigD < globals.smallValueTolerance && leftSuperficialLiquidVelocity > 0) {
            setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0);
        }

        if (fabs(leftSuperficialLiquidVelocity) < globals.smallValueTolerance * 1e-5) {
            if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftCell.alfPigD < globals.smallValueTolerance && leftCell.fontemassGR >= globals.smallValueTolerance * 1e-5) {
                setFlowPartitionTerms(state, cellIndex, 1., branchFlag, 0);
            }
        } else if (cellIndex > 1 && fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftSuperficialLiquidVelocity > 0 && ((leftCell.alfPigD <= 1.0 * state.cells[cellIndex - 2].alfPigD && state.cells[cellIndex - 2].alfPigD < 0.99) || leftCell.alfPigD < 0.7))
            branchFlag = 1;
        else if (fabs(superficialLiquidVelocity) < globals.smallValueTolerance * 1e-5 && leftSuperficialLiquidVelocity > 0 && ((leftCell.alfPigD <= 1.0 * leftCell.alfL) || leftCell.alfPigD < 0.7))
            branchFlag = 1;
    }

    if (superficialGasVelocity < 0 && cell.alf > globals.smallValueTolerance && cell.alf < 1. - globals.smallValueTolerance)
        branchFlag = 1;

    if (branchFlag == 1) {

        double c0;
        double ud;
        const double meanVoidFraction = superficialGasVelocity < 0 ? cell.alfPigE : leftCell.alfPigD;
        driftflux::coefficient::buffered(dependencies.closure, cellIndex, c0, ud);
        if (state.input.escorregamentoCelulaContorno == 0) {
            c0 = 1.;
            ud = 0.;
        }
        double numerator = (1. - meanVoidFraction * c0);
        double denominator = 1 + c0 * meanVoidFraction * ((gasDensity / liquidDensity) - 1.);
        if (fabs(denominator) > 1e-15) {
            cell.term1 = numerator / denominator;
            cell.term2 = (-flowArea * meanVoidFraction * gasDensity * ud) / denominator;
        } else {
            cell.term1 = meanVoidFraction < 0.5 ? 1. : 0.;
            cell.term2 = 0.;
        }
        if (leftCell.acsr.tipo == kAccessoryChoke && leftCell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * leftCell.duto.area) {
            cell.term1 = 0.;
            cell.term2 = 0.;
        }
    }
}

void selectAndApplyBufferedInletFlowRegime(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double distributionCoefficient, double driftVelocity,
    double superficialGasVelocity, double superficialLiquidVelocity, double rightSuperficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea) {
    int branchFlag = 1;
    selectAndApplyInletRegime<BufferedInletRegime>(
        state, dependencies, cellIndex, branchFlag, distributionCoefficient, driftVelocity,
        superficialGasVelocity, superficialLiquidVelocity, rightSuperficialLiquidVelocity,
        gasDensity, liquidDensity, flowArea);
}

void refreshTemperatureCell(const ThermalState &state, int cellIndex) {
    computeCellTemperature(state, cellIndex, 0);
    Cel &cell = state.cells[cellIndex];
    cell.dTdt = (cell.temp - cell.tempini) / state.timeStep;
    cell.dTdtIni = cell.dTdt;
}

void refreshPoissonTemperatures(const ThermalState &state) {
    if (state.poisson2DCellCount <= 0)
        return;
    if (state.poisson2DCellCount == 1) {
        refreshTemperatureCell(state, state.poisson2DCellIndices[0]);
        return;
    }
    const int threadCount = std::max(1, std::min(state.globals->solverThreadCount, state.poisson2DCellCount));
#pragma omp parallel for num_threads(threadCount) default(none) shared(state) schedule(static)
    for (int index = 0; index < state.poisson2DCellCount; ++index)
        refreshTemperatureCell(state, state.poisson2DCellIndices[index]);
}

void refreshRegularTemperatures(const ThermalState &state) {
    const int regularCellCount = state.lastCell + 1 - state.poisson2DCellCount;
    const int threadCount = std::max(1, std::min(state.globals->solverThreadCount, regularCellCount));
#pragma omp parallel for num_threads(threadCount) default(none) shared(state) schedule(static)
    for (int index = 0; index <= state.lastCell; ++index)
        if (state.cells[index].calor.difus2D == 0)
            refreshTemperatureCell(state, index);
}

void refreshTemperatureField(const ThermalState &state) {
    refreshPoissonTemperatures(state);
    refreshRegularTemperatures(state);
}

TemperatureSourceTerms computeSteadySourceTerms(const ThermalState &state,
                                                int cellIndex,
                                                double meanPressure,
                                                double meanTemperature,
                                                double cellLength) {
    Cel &leftCell = state.cells[cellIndex - 1];
    double sourceTemperature = leftCell.temp;
    double sourceGasSpecificHeat;
    double sourceSpecificHeatRatio;
    double sourceLiquidSpecificHeat;

    if (leftCell.acsr.tipo == kAccessoryGasInjection) {
        sourceTemperature = leftCell.acsr.injg.temp;
        sourceGasSpecificHeat = leftCell.acsr.injg.FluidoPro.CalorGas(meanPressure, sourceTemperature);
        sourceSpecificHeatRatio = leftCell.acsr.injg.FluidoPro.ConstAdG(meanPressure, sourceTemperature);
        sourceLiquidSpecificHeat = 0.;
    } else if (leftCell.acsr.tipo == kAccessoryLiquidInjection) {
        sourceTemperature = leftCell.acsr.injl.temp;
        sourceGasSpecificHeat = 0.;
        sourceSpecificHeatRatio = 1.;
        sourceLiquidSpecificHeat = (1. - leftCell.acsr.injl.bet) * leftCell.acsr.injl.FluidoPro.CalorLiq(meanPressure, meanTemperature) + leftCell.acsr.injl.bet * leftCell.acsr.injl.fluidocol.CalorLiq(meanPressure, meanTemperature);
    } else if (leftCell.acsr.tipo == kAccessoryInflowPerformance) {
        sourceTemperature = leftCell.acsr.ipr.Tres;
        sourceGasSpecificHeat = leftCell.acsr.ipr.FluidoPro.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = leftCell.acsr.ipr.FluidoPro.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = leftCell.acsr.ipr.FluidoPro.CalorLiq(meanPressure, meanTemperature);
    } else if (leftCell.acsr.tipo == kAccessoryLeak && leftCell.acsr.fontechk.abertura > 1e-6 &&
               (leftCell.fontemassCR + leftCell.fontemassGR + leftCell.fontemassLR) > 1e-9) {
        sourceTemperature = leftCell.acsr.fontechk.tamb;
        sourceGasSpecificHeat = leftCell.acsr.fontechk.fluidoPamb.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = leftCell.acsr.fontechk.fluidoPamb.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = (1. - leftCell.acsr.fontechk.betISamb) *
                                       leftCell.acsr.fontechk.fluidoPamb.CalorLiq(meanPressure, meanTemperature) +
                                   leftCell.acsr.fontechk.betISamb * leftCell.acsr.fontechk.fluidocol.CalorLiq(meanPressure, meanTemperature);
    } else if (leftCell.acsr.tipo == kAccessoryRadialPorous) {
        sourceTemperature = leftCell.acsr.radialPoro.tRes;
        sourceGasSpecificHeat = leftCell.acsr.radialPoro.flup.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = leftCell.acsr.radialPoro.flup.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = leftCell.acsr.radialPoro.flup.CalorLiq(meanPressure, meanTemperature);
    } else if (leftCell.acsr.tipo == kAccessoryPorous2D) {
        sourceTemperature = leftCell.acsr.poroso2D.dados.tRes;
        sourceGasSpecificHeat = leftCell.acsr.poroso2D.dados.flup.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = leftCell.acsr.poroso2D.dados.flup.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = leftCell.acsr.poroso2D.dados.flup.CalorLiq(meanPressure, meanTemperature);
    } else {
        clearSourceSpecificHeats(sourceGasSpecificHeat, sourceSpecificHeatRatio,
                                 sourceLiquidSpecificHeat);
    }

    double liquidMassSourceTerm = 0;
    if (leftCell.fontemassLR > 0.)
        liquidMassSourceTerm = leftCell.fontemassLR / cellLength;
    if (leftCell.fontemassCR > 0.)
        liquidMassSourceTerm += leftCell.fontemassCR / cellLength;
    liquidMassSourceTerm *= (sourceLiquidSpecificHeat) * (sourceTemperature - leftCell.temp);

    double gasMassSourceTerm = leftCell.fontemassGR / cellLength;
    if (gasMassSourceTerm > 0.)
        gasMassSourceTerm *= (sourceGasSpecificHeat / sourceSpecificHeatRatio) * (sourceTemperature - leftCell.temp);
    else
        gasMassSourceTerm = 0;
    return TemperatureSourceTerms{.gas = gasMassSourceTerm,
                                  .liquid = liquidMassSourceTerm};
}

double applySteadyAnnulusCoupling(const ThermalState &state, int cellIndex,
                                  double interfaceMeanPressure,
                                  double interfaceMeanTemperature,
                                  double gasSpecificHeat,
                                  double gasDensity) {
    Cel &leftCell = state.cells[cellIndex - 1];
    double annulusResistance = 0.;
    // checks whether there is coupling with the annulus:
    if (state.input.lingas == 1 && (cellIndex - 1 <= state.annulusTubingStart && cellIndex - 1 >= state.annulusTubingEnd)) {
        int j = state.annulusTubingStart + state.tubingAnnulusStart - (cellIndex - 1);
        // with coupling:

        if (state.steadyIteration == 0 && (cellIndex - 1) < state.annulusTubingStart) {
            // on the first iteration, the resistance of casing + cement +
            // formation is considered; on the other iterations
            // it is enough to determine the heat exchange between the tubing and the annulus gas
            // in the coupled model, the tubing has no casing+cement+formation wall definition;
            // to obtain this resistance, the heat-exchange model of the
            // annulus has to be used, which is done here:
            // Note: done up to one cell before reaching the master, (cellIndex-1)<ColunaAnulaIni,
            // At the ANM cell only the heat exchange with the gas is considered,
            // from the first iteration on
            state.gasCells[j].calor.Tint = interfaceMeanTemperature;
            state.gasCells[j].calor.Vint = 100;
            state.gasCells[j].calor.kint = leftCell.flui.CondGas(interfaceMeanPressure, interfaceMeanTemperature);
            state.gasCells[j].calor.cpint = gasSpecificHeat;
            state.gasCells[j].calor.rhoint = gasDensity;
            state.gasCells[j].calor.viscint = leftCell.flui.ViscGas(interfaceMeanPressure, interfaceMeanTemperature) * units::kPascalSecondPerCentipoise;
            state.gasCells[j].fluxcal = state.gasCells[j].calor.transperm(); // heat exchange in the annulus
            annulusResistance = state.gasCells[j].calor.resGlob;             // wall resistance
            // The heat flux itself is not of interest here, only the resistance
            // of the walls, from the casing towards the formation.
            leftCell.calor.Vextern1 = 100.;
        } else { // after the first iteration, only the heat exchange between the tubing and the annulus gas is considered,
            // through the tubing wall, of course
            annulusResistance = 0.;
            leftCell.calor.Vextern1 = state.gasCells[j].VGasR / state.gasCells[j].u1L;
        }
        leftCell.calor.kextern1 = state.gasCells[j].calor.kint;
        leftCell.calor.cpextern1 = state.gasCells[j].calor.cpint;
        leftCell.calor.rhoextern1 = state.gasCells[j].calor.rhoint;
        leftCell.calor.viscextern1 = state.gasCells[j].calor.viscint;
        if (state.steadyIteration == 0)
            leftCell.calor.Textern1 = state.gasCells[j].calor.Textern1; // on the first iteration, as the calculation
        // uses the whole well resistance, so the external temperature is the geothermal one
        else
            leftCell.calor.Textern1 = state.gasCells[j].temp; // on the following iterations, the ambient temperature
        // is the gas temperature
    }
    return annulusResistance;
}

double computeSteadyKineticTerm(const ThermalState &state, int cellIndex,
                                double meanSuperficialGasVelocity,
                                double meanSuperficialLiquidVelocity) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double kineticTerm = 0;
    double upstreamMeanGasVelocity = 0;
    double upstreamMeanLiquidVelocity = 0;
    double meanGasVelocity = 0;
    double meanLiquidVelocity = 0;
    if (cell.acsr.tipo == kAccessoryNone && leftCell.acsr.tipo == kAccessoryNone && cellIndex > 2) {
        double kineticCellLength = leftCell.dx;
        double upstreamDiameter = leftCell.duto.a;
        double upstreamFlowArea = 0.25 * std::numbers::pi * upstreamDiameter * upstreamDiameter;

        if (leftCell.alf > 1e-3)
            meanGasVelocity = meanSuperficialGasVelocity / leftCell.alf;
        if (leftCell.alf < (1. - 1e-3))
            meanLiquidVelocity = meanSuperficialLiquidVelocity / (1. - leftCell.alf);

        if (state.cells[cellIndex - 2].alf > 1e-3) {
            upstreamMeanGasVelocity = leftCell.QG / (upstreamFlowArea);
            upstreamMeanGasVelocity /= state.cells[cellIndex - 2].alf;
        }
        if (state.cells[cellIndex - 2].alf < (1. - 1e-3)) {
            upstreamMeanLiquidVelocity = leftCell.QL / (upstreamFlowArea);
            upstreamMeanLiquidVelocity /= (1. - state.cells[cellIndex - 2].alf);
        }

        if (state.input.nCompTotalUnidadesP / kineticCellLength < kKineticTermMaximumLengthRatio)
            kineticTerm = (cell.MC - cell.Mliqini) * meanGasVelocity * (meanGasVelocity - upstreamMeanGasVelocity) / kineticCellLength + cell.Mliqini * meanLiquidVelocity * (meanLiquidVelocity - upstreamMeanLiquidVelocity) / kineticCellLength;
        else
            kineticTerm = 0;
    }
    return kineticTerm;
}

double computeSteadyLatentHeatTerm(const ThermalState &state, int cellIndex,
                                   double meanPressure, double meanTemperature) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double latentHeatTerm = 0.;
    if (isnan(leftCell.FonteMudaFase))
        leftCell.FonteMudaFase = 0.;
    double phaseChangeMassRate = fabs(leftCell.FonteMudaFase);
    double phaseChangeSign = 1.;
    if (phaseChangeMassRate > kPhaseChangeFloor)
        phaseChangeSign = leftCell.FonteMudaFase / phaseChangeMassRate;
    if (state.input.limTransMass < phaseChangeMassRate)
        phaseChangeMassRate = phaseChangeSign * state.input.limTransMass;
    else
        phaseChangeMassRate *= phaseChangeSign;
    if (cell.flui.dVaporMassFraction < (1 - 1e-15) && cell.flui.dVaporMassFraction > (1e-15)) {
        if (leftCell.acsr.tipo == kAccessoryGasInjection || leftCell.acsr.tipo == kAccessoryLiquidInjection || leftCell.acsr.tipo == kAccessoryInflowPerformance || leftCell.acsr.tipo == kAccessoryRadialPorous || leftCell.acsr.tipo == kAccessoryPorous2D) {
            leftCell.FonteMudaFase = 0.;
            phaseChangeMassRate = 0.;
        }
        if (state.latentHeatEnabled > 0 && state.steadyIteration != 0 && state.input.flashCompleto == 0) {
            latentHeatTerm = interpolateLatentHeat(state, meanPressure, meanTemperature) * phaseChangeMassRate;
        } else if ((state.input.flashCompleto == 1 || state.input.flashCompleto == 2) && state.latentHeatEnabled > 0 && state.steadyIteration != 0) {
            latentHeatTerm = (cell.flui.EntalpGas(meanPressure, meanTemperature) -
                              cell.flui.EntalpLiq(meanPressure, meanTemperature)) *
                             phaseChangeMassRate;
        } else
            latentHeatTerm = 0;
    }

    return latentHeatTerm;
}

TemperatureSourceTerms computeReverseSteadySourceTerms(
    const ThermalState &state, int cellIndex, double meanPressure,
    double meanTemperature, double cellLength) {
    Cel &rightCell = state.cells[cellIndex + 1];
    double sourceTemperature = rightCell.temp;
    double sourceGasSpecificHeat;
    double sourceSpecificHeatRatio;
    double sourceLiquidSpecificHeat;

    if (rightCell.acsr.tipo == kAccessoryGasInjection) {
        sourceTemperature = rightCell.acsr.injg.temp;
        sourceGasSpecificHeat = rightCell.acsr.injg.FluidoPro.CalorGas(meanPressure, sourceTemperature);
        sourceSpecificHeatRatio = rightCell.acsr.injg.FluidoPro.ConstAdG(meanPressure, sourceTemperature);
        sourceLiquidSpecificHeat = 0.;
    } else if (rightCell.acsr.tipo == kAccessoryLiquidInjection) {
        sourceTemperature = rightCell.acsr.injl.temp;
        sourceGasSpecificHeat = 0.;
        sourceSpecificHeatRatio = 1.;
        sourceLiquidSpecificHeat = (1. - rightCell.acsr.injl.bet) * rightCell.acsr.injl.FluidoPro.CalorLiq(meanPressure, meanTemperature) + rightCell.acsr.injl.bet * rightCell.acsr.injl.fluidocol.CalorLiq(meanPressure, meanTemperature);
    } else if (rightCell.acsr.tipo == kAccessoryInflowPerformance) {
        sourceTemperature = rightCell.acsr.ipr.Tres;
        sourceGasSpecificHeat = rightCell.acsr.ipr.FluidoPro.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = rightCell.acsr.ipr.FluidoPro.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = rightCell.acsr.ipr.FluidoPro.CalorLiq(meanPressure, meanTemperature);
    } else if (rightCell.acsr.tipo == kAccessoryRadialPorous) {
        sourceTemperature = rightCell.acsr.radialPoro.tRes;
        sourceGasSpecificHeat = rightCell.acsr.radialPoro.flup.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = rightCell.acsr.radialPoro.flup.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = rightCell.acsr.radialPoro.flup.CalorLiq(meanPressure, meanTemperature);
    } else if (rightCell.acsr.tipo == kAccessoryPorous2D) {
        sourceTemperature = rightCell.acsr.poroso2D.dados.tRes;
        sourceGasSpecificHeat = rightCell.acsr.poroso2D.dados.flup.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = rightCell.acsr.poroso2D.dados.flup.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = rightCell.acsr.poroso2D.dados.flup.CalorLiq(meanPressure, meanTemperature);
    } else if (rightCell.acsr.tipo == kAccessoryLeak && rightCell.acsr.fontechk.abertura > 1e-6 &&
               (rightCell.fontemassCR + rightCell.fontemassGR + rightCell.fontemassLR) > 1e-9) {
        sourceTemperature = rightCell.acsr.fontechk.tamb;
        sourceGasSpecificHeat = rightCell.acsr.fontechk.fluidoPamb.CalorGas(meanPressure, meanTemperature);
        sourceSpecificHeatRatio = rightCell.acsr.fontechk.fluidoPamb.ConstAdG(meanPressure, meanTemperature);
        sourceLiquidSpecificHeat = (1. - rightCell.acsr.fontechk.betISamb) *
                                       rightCell.acsr.fontechk.fluidoPamb.CalorLiq(meanPressure, meanTemperature) +
                                   rightCell.acsr.fontechk.betISamb * rightCell.acsr.fontechk.fluidocol.CalorLiq(meanPressure, meanTemperature);
    } else {
        clearSourceSpecificHeats(sourceGasSpecificHeat, sourceSpecificHeatRatio,
                                 sourceLiquidSpecificHeat);
    }

    double liquidMassSourceTerm = 0;
    if (rightCell.fontemassLR > 0.)
        liquidMassSourceTerm = rightCell.fontemassLR / cellLength;
    if (rightCell.fontemassCR > 0.)
        liquidMassSourceTerm += rightCell.fontemassCR / cellLength;
    liquidMassSourceTerm *= (sourceLiquidSpecificHeat) * (sourceTemperature - rightCell.temp);

    double gasMassSourceTerm = rightCell.fontemassGR / cellLength;
    if (gasMassSourceTerm > 0.)
        gasMassSourceTerm *= (sourceGasSpecificHeat / sourceSpecificHeatRatio) * (sourceTemperature - rightCell.temp);
    else
        gasMassSourceTerm = 0;
    return TemperatureSourceTerms{.gas = gasMassSourceTerm,
                                  .liquid = liquidMassSourceTerm};
}

double computeReverseSteadyKineticTerm(
    const ThermalState &state, int cellIndex,
    double meanSuperficialGasVelocity,
    double meanSuperficialLiquidVelocity) {
    Cel &cell = state.cells[cellIndex];
    Cel &rightCell = state.cells[cellIndex + 1];
    double kineticTerm = 0;
    double upstreamMeanGasVelocity = 0;
    double upstreamMeanLiquidVelocity = 0;
    double meanGasVelocity = 0;
    double meanLiquidVelocity = 0;
    if (cell.acsr.tipo == kAccessoryNone && rightCell.acsr.tipo == kAccessoryNone && cellIndex < state.lastCell) {
        double kineticCellLength = rightCell.dx;
        double upstreamDiameter = rightCell.duto.a;
        double upstreamFlowArea = 0.25 * std::numbers::pi * upstreamDiameter * upstreamDiameter;

        if (rightCell.alf > 1e-3)
            meanGasVelocity = meanSuperficialGasVelocity / rightCell.alf;
        if (rightCell.alf < (1. - 1e-3))
            meanLiquidVelocity = meanSuperficialLiquidVelocity / (1. - rightCell.alf);

        if (cell.alf > 1e-3) {
            upstreamMeanGasVelocity = fabs(cell.QG) / (upstreamFlowArea);
            upstreamMeanGasVelocity /= cell.alf;
        }
        if (cell.alf < (1. - 1e-3)) {
            upstreamMeanLiquidVelocity = fabs(cell.QL) / (upstreamFlowArea);
            upstreamMeanLiquidVelocity /= (1. - cell.alf);
        }

        kineticTerm = -fabs(cell.MC - cell.Mliqini) * meanGasVelocity * (meanGasVelocity - upstreamMeanGasVelocity) / kineticCellLength -
                      fabs(cell.Mliqini) * meanLiquidVelocity * (meanLiquidVelocity - upstreamMeanLiquidVelocity) / kineticCellLength;
    }
    return kineticTerm;
}

double computeReverseSteadyLatentHeatTerm(const ThermalState &state, int cellIndex,
                                          double meanPressure, double meanTemperature) {
    Cel &cell = state.cells[cellIndex];
    Cel &rightCell = state.cells[cellIndex + 1];
    double latentHeatTerm = 0.;
    if (cell.flui.dVaporMassFraction < (1 - 1e-15) && cell.flui.dVaporMassFraction > (1e-15)) {
        if (rightCell.acsr.tipo == kAccessoryGasInjection || rightCell.acsr.tipo == kAccessoryLiquidInjection || rightCell.acsr.tipo == kAccessoryInflowPerformance || rightCell.acsr.tipo == kAccessoryRadialPorous || rightCell.acsr.tipo == kAccessoryPorous2D)
            rightCell.FonteMudaFase =
                0.;
        if (state.latentHeatEnabled > 0 && state.steadyIteration != 0 && state.input.flashCompleto == 0) {
            latentHeatTerm = -interpolateLatentHeat(state, meanPressure, meanTemperature) * rightCell.FonteMudaFase;
        } else if ((state.input.flashCompleto == 1 || state.input.flashCompleto == 2) && state.latentHeatEnabled > 0 && state.steadyIteration != 0) {
            latentHeatTerm = -(cell.flui.EntalpGas(meanPressure, meanTemperature) -
                               cell.flui.EntalpLiq(meanPressure, meanTemperature)) *
                             rightCell.FonteMudaFase;
        } else
            latentHeatTerm = 0;
    }
    return latentHeatTerm;
}

double applyReverseSteadyAnnulusCoupling(
    const ThermalState &state, int cellIndex,
    double interfaceMeanPressure, double interfaceMeanTemperature,
    double gasSpecificHeat, double gasDensity) {
    Cel &rightCell = state.cells[cellIndex + 1];
    double annulusResistance = 0.;
    // checks if coupling with the annular space exists:
    if (state.input.lingas == 1 && (cellIndex + 1 <= state.annulusTubingStart && cellIndex + 1 >= state.annulusTubingEnd)) {
        int j = state.annulusTubingStart + state.tubingAnnulusStart - (cellIndex + 1);
        // in case there is coupling:

        if (state.steadyIteration == 0 && (cellIndex + 1) > state.annulusTubingEnd) {
            // On the first iteration, include casing, cement, and formation resistance.
            // Later iterations consider only heat transfer between the tubing and annular gas.
            // For the coupled model, obtain the external resistance from the annulus heat-transfer model.
            // Apply this up to the cell before the master valve to improve the ANM temperature estimate.
            // At the ANM cell, consider only heat transfer with the gas from the first iteration.
            state.gasCells[j].calor.Tint = interfaceMeanTemperature;
            state.gasCells[j].calor.Vint = 100;
            state.gasCells[j].calor.kint = rightCell.flui.CondGas(interfaceMeanPressure, interfaceMeanTemperature);
            state.gasCells[j].calor.cpint = gasSpecificHeat;
            state.gasCells[j].calor.rhoint = gasDensity;
            state.gasCells[j].calor.viscint = rightCell.flui.ViscGas(interfaceMeanPressure, interfaceMeanTemperature) * units::kPascalSecondPerCentipoise;
            state.gasCells[j].fluxcal = state.gasCells[j].calor.transperm(); // heat exchange in the annulus
            annulusResistance = state.gasCells[j].calor.resGlob;             // wall resistance
            // Only the thermal resistance from the casing to the formation is needed, not the actual heat flow.
            rightCell.calor.Vextern1 = 100.;
        } else { // After the first iteration, consider only heat transfer between the tubing and annular gas.
            // Through the tubing wall.
            annulusResistance = 0.;
            rightCell.calor.Vextern1 = state.gasCells[j].VGasR / state.gasCells[j].u1L;
        }
        rightCell.calor.kextern1 = state.gasCells[j].calor.kint;
        rightCell.calor.cpextern1 = state.gasCells[j].calor.cpint;
        rightCell.calor.rhoextern1 = state.gasCells[j].calor.rhoint;
        rightCell.calor.viscextern1 = state.gasCells[j].calor.viscint;
        if (state.steadyIteration == 0)
            rightCell.calor.Textern1 = state.gasCells[j].calor.Textern1; // on the first iteration, as the calculation
        // uses the whole well resistance, so the external temperature is the geothermal one
        else
            rightCell.calor.Textern1 = state.gasCells[j].temp; // on the following iterations, the ambient temperature
        // is the gas temperature
    }
    return annulusResistance;
}

[[nodiscard]] SourceEnthalpy upstreamSourceEnthalpyOf(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double sourceTemperature = cell.temp;
    double sourceGasEnthalpy;
    double sourceLiquidEnthalpy;
    double hcF = 0.;
    if ((*cell.acsrL).tipo == kAccessoryChoke) {
        if ((*cell.acsrL).chk.AreaGarg < state.input.master1.razareaativ * cell.dutoL.area && (*cell.acsrL).chk.AreaGarg > 1e-5 * cell.dutoL.area) {
            double chokeUpstreamTemperature = leftCell.temp;
            double chokeUpstreamVoidFraction = leftCell.alf;
            double betE = leftCell.bet;

            double upstreamLiquidDensity = leftCell.flui.MasEspLiq(leftCell.pres, leftCell.temp);
            double rholc = leftCell.fluicol.MasEspFlu(leftCell.pres, leftCell.temp);
            double upstreamMixtureLiquidDensity = (1 - betE) * upstreamLiquidDensity + betE * rholc;

            double chokeDownstreamVoidFraction = cell.alf;
            double betJ = cell.bet;
            double downstreamLiquidDensity = cell.flui.MasEspLiq(cell.pres, cell.temp);
            double rholcJ = cell.fluicol.MasEspFlu(cell.pres, cell.temp);
            double downstreamMixtureLiquidDensity = (1 - betJ) * downstreamLiquidDensity + betJ * rholcJ;

            const double upstreamGasDensity = leftCell.flui.MasEspGas(leftCell.pres, leftCell.temp);
            double upstreamHydrostaticHead = sin(leftCell.duto.teta) * (0.5 * leftCell.dx) * (upstreamMixtureLiquidDensity * (1 - chokeUpstreamVoidFraction) + chokeUpstreamVoidFraction * upstreamGasDensity) * kGravity / units::kPascalPerKgfPerCm2;
            double downstreamHydrostaticHead = sin(cell.duto.teta) * (0.5 * cell.dx) * (downstreamMixtureLiquidDensity * (1 - chokeDownstreamVoidFraction) + chokeDownstreamVoidFraction * cell.flui.MasEspGas(cell.pres, cell.temp)) * kGravity / units::kPascalPerKgfPerCm2;

            double upstreamGasMassFraction = chokeUpstreamVoidFraction * upstreamGasDensity / (upstreamGasDensity * chokeUpstreamVoidFraction + upstreamMixtureLiquidDensity * (1. - chokeUpstreamVoidFraction));

            double upstreamLiquidJouleThomson = (1. - betE) * leftCell.flui.JTL(leftCell.pres - upstreamHydrostaticHead, leftCell.temp) - betE / rholcJ;
            double upstreamGasJouleThomson = leftCell.flui.JTG(leftCell.pres - upstreamHydrostaticHead, leftCell.temp);
            sourceTemperature = chokeUpstreamTemperature + ((1. - upstreamGasMassFraction) * upstreamLiquidJouleThomson + upstreamGasMassFraction * upstreamGasJouleThomson) * (cell.pres + downstreamHydrostaticHead - leftCell.pres - upstreamHydrostaticHead);

            sourceGasEnthalpy = leftCell.flui.EntalpGas(leftCell.pres, sourceTemperature);
            sourceLiquidEnthalpy = leftCell.flui.EntalpLiq(leftCell.pres, sourceTemperature);
            hcF = leftCell.fluicol.CalorLiq(leftCell.pres, sourceTemperature) * sourceTemperature;

        } else {
            sourceTemperature = cell.temp;
            sourceGasEnthalpy = leftCell.flui.EntalpGas(leftCell.pres, leftCell.temp);
            sourceLiquidEnthalpy = leftCell.flui.EntalpLiq(leftCell.pres, leftCell.temp);
            hcF = leftCell.fluicol.CalorLiq(leftCell.pres, leftCell.temp) * sourceTemperature;
        }
    } else if ((*cell.acsrL).tipo == kAccessoryVolumetricPump) {

        double polytropicExponent = (*cell.acsrL).bvol.npoli;
        double pumpPressureRatio = (cell.pres) / (leftCell.pres);
        sourceTemperature = leftCell.temp * pow(pumpPressureRatio, (polytropicExponent - 1) / polytropicExponent);

        sourceGasEnthalpy = leftCell.flui.EntalpGas(leftCell.pres, sourceTemperature);
        sourceLiquidEnthalpy = leftCell.flui.EntalpLiq(leftCell.pres, sourceTemperature);
        hcF = leftCell.fluicol.CalorLiq(leftCell.pres, sourceTemperature) * sourceTemperature;

    } else {
        sourceGasEnthalpy = 0.;
        sourceLiquidEnthalpy = 0.;
    }
    return SourceEnthalpy{.gasEnthalpy = sourceGasEnthalpy,
                          .liquidEnthalpy = sourceLiquidEnthalpy,
                          .hcF = hcF};
}

int thermalCouplingIndexOf(const ThermalState &state, int cellIndex) {
    for (int couplingIndex = 0; couplingIndex < state.input.nacop; couplingIndex++) {
        if (cellIndex == state.input.celAcop[couplingIndex].indCel)
            return couplingIndex;
    }
    return -1;
}

void updateInletBoundaryFlowPartition(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, Vcr<int> &bifurcationFlags, int explicitMode) {
    Cel &cell = state.cells[cellIndex];
    if (state.input.ConContEntrada == 0) {
        state.cells[1].alfL = state.cells[0].alf;
        state.cells[0].alfL = state.cells[0].alf;
        state.cells[1].betL = state.cells[0].bet;
        state.cells[0].betL = state.cells[0].bet;
        state.cells[0].term1 = 0.;
        state.cells[0].term2 = 0.;
        state.cells[0].term1L = 0.;
        state.cells[0].term2L = 0.;
        cell.alfI = 1.;
    } else {

        double meanPressure;
        meanPressure = state.inletPressure;

        double meanTemperature;
        if (state.cells[0].QL < 0.)
            meanTemperature = cell.temp;
        else
            meanTemperature = state.inletTemperature;

        double gasDensity = state.cells[0].flui.MasEspGas(state.inletPressure, state.inletTemperature);
        double liquidDensity = state.cells[0].flui.MasEspLiq(state.inletPressure, state.inletTemperature);
        double rcis = state.cells[0].fluicol.MasEspFlu(state.inletPressure, state.inletTemperature);

        double liquidMixtureDensity = state.inletComplementaryFraction * rcis + (1 - state.inletComplementaryFraction) * liquidDensity;
        state.inletVoidFraction = (-state.inletQuality * liquidMixtureDensity / (state.inletQuality * gasDensity - gasDensity - state.inletQuality * liquidMixtureDensity)) / (state.cells[0].c0);
        cell.alfI = state.inletVoidFraction;

        double betI;
        double surfaceTension;
        if (cell.QL < 0.) {
            betI = cell.betPigE;
            liquidDensity = (1 - betI) * cell.flui.MasEspLiq(meanPressure, meanTemperature) + betI * cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * cell.flui.TensSuper(meanPressure, meanTemperature) + betI * cell.fluicol.TensSuper(meanPressure, meanTemperature);
        } else {
            betI = state.inletComplementaryFraction;
            liquidDensity = (1 - betI) * (*cell.fluiL).MasEspLiq(meanPressure, meanTemperature) + betI * cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * (*cell.fluiL).TensSuper(meanPressure, meanTemperature) + betI * cell.fluicol.TensSuper(meanPressure, meanTemperature);
        }

        double flowArea;
        if (cell.QG >= 0) {
            flowArea = cell.duto.area;
            gasDensity = (*cell.fluiL).MasEspGas(meanPressure, meanTemperature);
        } else {
            gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
            flowArea = cell.duto.area;
        }
        double superficialGasVelocity = cell.QG / (flowArea);
        double superficialLiquidVelocity = cell.QL / (flowArea);

        double inclinationAngle = cell.duto.teta;
        double inclinationSign = 1.;
        if (inclinationAngle < 0.)
            inclinationSign = -1.;
        double c0 = 2.;
        double ud = inclinationSign * 0.0246 * cos(inclinationAngle) + 1.606 * pow(kGravity * surfaceTension * (liquidDensity - gasDensity) / (liquidDensity * liquidDensity), 0.25) * sin(inclinationAngle);

        double rightFlowArea = cell.dutoR.area;
        double rightLengthRatio = cell.dx / (cell.dxR + cell.dx);
        double rightMeanPressure = cell.presauxR;
        double rightMeanTemperature = cell.temp * rightLengthRatio + cell.tempL * (1. - rightLengthRatio);
        double betIR = cell.betPigD;
        if (cell.QLR < 0.)
            betIR = state.cells[cellIndex + 1].betPigE;

        (void)cell.flui.MasEspGas(rightMeanPressure, rightMeanTemperature);
        double rightLiquidDensity = (1 - betIR) * cell.flui.MasEspLiq(rightMeanPressure, rightMeanTemperature) + betIR * cell.fluicol.MasEspFlu(rightMeanPressure, rightMeanTemperature);
        double rightSuperficialLiquidVelocity = (cell.MliqiniR) / (rightLiquidDensity * rightFlowArea);

        selectAndApplyInletBoundaryFlowRegime(
            state, dependencies, cellIndex, bifurcationFlags, c0, ud, superficialGasVelocity, superficialLiquidVelocity, rightSuperficialLiquidVelocity,
            gasDensity, liquidDensity, flowArea, explicitMode);
        state.cells[1].term1L = cell.term1;
        state.cells[1].term2L = cell.term2;

        state.cells[1].alfL = state.cells[0].alf;
        state.cells[0].alfL = state.inletVoidFraction;
        state.cells[1].betL = state.cells[0].bet;
        state.cells[0].betL = state.inletComplementaryFraction;
    }
}

double axisymmetricHeatFluxOf(const ThermalState &state, int cellIndex,
                             TransCal &heatTransfer, bool useDirichletBoundary) {
    int coupled = -1;
    for (int index = 0; index < state.input.nAxiSim; index++) {
        if (cellIndex == state.input.indAxiSim[index]) {
            coupled = index;
            break;
        }
    }
    if (coupled == -1)
        return heatTransfer.transtrans();

    const double heatFlux = -state.axisymmetricSolver.dados->qAcop[coupled];
    if (useDirichletBoundary && state.axisymmetricSolver.dados->acopDirichlet != 0)
        return heatTransfer.transtrans(1, heatFlux);
    heatTransfer.transtransAxiSim(heatFlux);
    return heatFlux;
}

double temperatureHeatFluxOf(const ThermalState &state, int cellIndex, int steadyStateMode) {
    Cel &cell = state.cells[cellIndex];
    double heatFlux = 0.;
    if ((state.input.modoDifus3D == 0 &&
         (state.input.pocoTermAxiSim == 0 || state.input.nAxiSim == 0 || state.input.anulAxiSim == 1)) ||
        steadyStateMode != 0) {
        if (steadyStateMode == 0)
            heatFlux = cell.calor.transtrans();
        else
            heatFlux = cell.calor.transperm();
        if (state.productionNetworkCoupled == 1 && cellIndex >= state.primarySectionStart && cellIndex <= state.primarySectionEnd) {
            heatFlux -= cell.fluxcalAcopRedeP;
        }
    } else if (state.input.modoDifus3D == 1 ||
               (state.input.pocoTermAxiSim == 1 && state.input.nAxiSim > 0 && state.input.anulAxiSim == 0)) {
        if (state.input.modoDifus3D == 1) {
            const int coupled = thermalCouplingIndexOf(state, cellIndex);
            if (coupled == -1) {
                if (state.input.pocoTermAxiSim == 0 || state.input.nAxiSim == 0 || state.input.anulAxiSim == 1)
                    heatFlux = cell.calor.transtrans();
                else
                    heatFlux = axisymmetricHeatFluxOf(state, cellIndex, cell.calor, true);
            } else {
                int iacop1 = state.coupledCellIndices[coupled];
                heatFlux = -state.input.celAcop[coupled].FE * state.poissonSolver3D.dados.qTotal[iacop1] / cell.dx;
            }
        } else {
            heatFlux = axisymmetricHeatFluxOf(state, cellIndex, cell.calor, false);
        }
    }
    return heatFlux;
}

double temperaturePressureGradientOf(const ThermalState &state, int cellIndex,
                                     const TemperatureBalance &balance) {
    Cel &cell = state.cells[cellIndex];
    double pressureGradient;
    if (cellIndex < state.lastCell - 1)
        pressureGradient = 2. * (state.cells[cellIndex + 1].presaux - cell.pres) * units::kPascalPerKgfPerCm2 / cell.dx;
    else if (cellIndex == state.lastCell - 1 && state.surfaceChoke.AreaGarg > 0.5 * state.cells[state.lastCell - 1].duto.area)
        pressureGradient = 2. * (state.cells[cellIndex + 1].presaux - cell.pres) * units::kPascalPerKgfPerCm2 / cell.dx;
    else
        pressureGradient = 2. * (cell.pres - cell.presaux) * units::kPascalPerKgfPerCm2 / cell.dx;
    if (cellIndex > 0 && cellIndex < state.lastCell && state.cells[cellIndex - 1].acsr.tipo == kAccessoryChoke &&
        state.cells[cellIndex - 1].acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * cell.duto.area)
        pressureGradient = 2. * (state.cells[cellIndex + 1].presaux - cell.pres) * units::kPascalPerKgfPerCm2 / cell.dx;
    else if (cellIndex > 0 && cell.acsr.tipo == kAccessoryChoke &&
             cell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * cell.duto.area) {
        pressureGradient = (cell.pres - state.cells[cellIndex - 1].pres) * units::kPascalPerKgfPerCm2 / balance.meanCellLength;
    } else if (cellIndex == 0)
        pressureGradient = 2. * (state.cells[cellIndex + 1].presaux - cell.pres) * units::kPascalPerKgfPerCm2 / cell.dx;
    return pressureGradient;
}

double temperatureSpatialGradientOf(const ThermalState &state, int cellIndex,
                                    const TemperatureBalance &balance, double &pressureGradient) {
    Cel &cell = state.cells[cellIndex];
    double temperatureGradient = 0.;
    if (cellIndex > 0)
        temperatureGradient = (cell.temp - state.cells[cellIndex - 1].tempini) / balance.meanCellLength;

    if (cellIndex < state.lastCell)
        if (cell.VTemper < 0) {
            const Cel &rightCell = state.cells[cellIndex + 1];
            temperatureGradient = (rightCell.tempini - cell.temp) / (0.5 * (rightCell.dx + cell.dx));
        }
    if (cell.acsr.tipo == kAccessoryChoke && cell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * cell.duto.area && cell.VTemper <= 0)
        temperatureGradient = 0.;
    if (cellIndex > 0 && state.cells[cellIndex - 1].acsr.tipo == kAccessoryChoke &&
        state.cells[cellIndex - 1].acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * state.cells[cellIndex - 1].duto.area && cell.VTemper >= 0) {
        if (cellIndex < state.lastCell) {
            const Cel &rightCell = state.cells[cellIndex + 1];
            temperatureGradient = 0 * (rightCell.tempini - cell.temp) / (0.5 * (rightCell.dx + cell.dx));
        } else {
            temperatureGradient = 0.;
        }
    }
    if ((cellIndex <= 1 && cell.VTemper <= 0) || (cellIndex == state.lastCell && cell.VTemper <= 0))
        temperatureGradient = 0.;
    if (cellIndex > 0 && cell.acsr.tipo == kAccessoryVolumetricPump && cell.acsr.bvol.freq > 1.) {
        pressureGradient = (cell.pres - state.cells[cellIndex - 1].pres) * units::kPascalPerKgfPerCm2 / balance.meanCellLength;
    }
    return temperatureGradient;
}

double computeTemperatureLatentHeatTerm(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    double latentHeatTerm;
    double phaseChangeMassRate = fabs(cell.FonteMudaFase);
    double phaseChangeSign = 1.;
    if (phaseChangeMassRate > kPhaseChangeFloor)
        phaseChangeSign = cell.FonteMudaFase / phaseChangeMassRate;
    if (state.input.limTransMass < phaseChangeMassRate)
        phaseChangeMassRate = phaseChangeSign * state.input.limTransMass;
    else
        phaseChangeMassRate *= phaseChangeSign;
    if (state.latentHeatEnabled > 0 && state.input.flashCompleto == 0) {
        latentHeatTerm = interpolateLatentHeat(state, cell.presini, cell.temp) * phaseChangeMassRate;
    } else if ((state.input.flashCompleto == 1 || state.input.flashCompleto == 2) && state.latentHeatEnabled > 0)
        latentHeatTerm = (cell.flui.EntalpGas(cell.presini, cell.temp) -
                          cell.flui.EntalpLiq(cell.presini, cell.temp)) *
                         phaseChangeMassRate;
    else
        latentHeatTerm = 0;

    if (state.input.latente == 0)
        latentHeatTerm = 0.;
    else if (state.input.condlatente == 0 && latentHeatTerm < 0)
        latentHeatTerm = 0.;
    return latentHeatTerm;
}

double computeThermalMassTransferKineticTerm(
    const ThermalState &state, int cellIndex, double meanSuperficialGasVelocity,
    double meanSuperficialLiquidVelocity) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double kineticTerm = 0;
    double leftCellLeftFaceMeanGasVelocity = 0;
    double leftCellLeftFaceMeanLiquidVelocity = 0;
    double leftFaceMeanGasVelocity = 0;
    double leftFaceMeanLiquidVelocity = 0.;
    if (cell.acsr.tipo == kAccessoryNone && leftCell.acsr.tipo == kAccessoryNone && cellIndex > 1) {

        double kineticCellLength = leftCell.dx;
        double leftCellDiameter = leftCell.duto.a;
        double leftCellFlowArea = 0.25 * std::numbers::pi * leftCellDiameter * leftCellDiameter;

        double leftFaceVoidFraction;
        if (cell.QL > 0)
            leftFaceVoidFraction = leftCell.alf;
        else
            leftFaceVoidFraction = cell.alf;
        double leftCellLeftFaceVoidFraction;
        if (leftCell.QL > 0)
            leftCellLeftFaceVoidFraction = state.cells[cellIndex - 2].alf;
        else
            leftCellLeftFaceVoidFraction = leftCell.alf;

        if (leftCellLeftFaceVoidFraction > 1e-3) {
            leftCellLeftFaceMeanGasVelocity = leftCell.QG / (leftCellFlowArea);
            leftCellLeftFaceMeanGasVelocity /= leftCellLeftFaceVoidFraction;
        }

        if (leftCellLeftFaceVoidFraction < 1. - 1e-3) {
            leftCellLeftFaceMeanLiquidVelocity = leftCell.QL / (leftCellFlowArea);
            leftCellLeftFaceMeanLiquidVelocity /= (1. - leftCellLeftFaceVoidFraction);
        }

        if (leftFaceVoidFraction > 1e-3) {
            leftFaceMeanGasVelocity = meanSuperficialGasVelocity;
            leftFaceMeanGasVelocity /= leftFaceVoidFraction;
        }

        if (leftFaceVoidFraction < 1. - 1e-3) {
            leftFaceMeanLiquidVelocity = meanSuperficialLiquidVelocity;
            leftFaceMeanLiquidVelocity /= (1. - leftFaceVoidFraction);
        }

        kineticTerm = (cell.MC - leftCell.Mliqini) * leftFaceMeanGasVelocity * (leftFaceMeanGasVelocity - leftCellLeftFaceMeanGasVelocity) / kineticCellLength + leftCell.Mliqini * leftFaceMeanLiquidVelocity * (meanSuperficialLiquidVelocity - leftCellLeftFaceMeanLiquidVelocity) / kineticCellLength;
    }
    return kineticTerm;
}

void advanceSteadyTemperatureSubsteps(const ThermalState &state, int cellIndex,
                                      const SteadyTemperatureStep &step, double heatFluxTemperatureGradient) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    const double meanPressure = step.meanPressure;
    const double meanVoidFraction = step.meanVoidFraction;
    const double betmed = step.betmed;
    const double liquidDensity = step.liquidDensity;
    const double gasDensity = step.gasDensity;
    const double liquidSpecificHeat = step.liquidSpecificHeat;
    const double gasSpecificHeat = step.gasSpecificHeat;
    const double meanSuperficialGasVelocity = step.meanSuperficialGasVelocity;
    const double meanSuperficialLiquidVelocity = step.meanSuperficialLiquidVelocity;
    const double meanCellLength = step.meanCellLength;
    const double meanTemperatureGradientCorrection = step.meanTemperatureGradientCorrection;
    const double temperatureSpatialCoefficient = step.temperatureSpatialCoefficient;
    const double annulusResistance = step.annulusResistance;
    const double mixtureFluxSign = step.mixtureFluxSign;
    const double sourceTemperatureGradient = step.sourceTemperatureGradient;
    double liquidConductivity;
    double liquidViscosity;
    double heatFlux;
    int subStepCount;
    double subStepLength;
    double stabilityLength = fabs(temperatureSpatialCoefficient) / meanTemperatureGradientCorrection;
    if (meanCellLength / (leftCell.calor.resGlob + annulusResistance) < (stabilityLength + 0. * 1000.)) { // Thermal resistance is not low.
        subStepCount = 1;
        subStepLength = meanCellLength;
    } else {
        // For low thermal resistance, determine the number of temperature update steps.
        subStepCount = (meanCellLength / (leftCell.calor.resGlob + annulusResistance)) / (stabilityLength + 0 * 1000) + 1;
        subStepLength = meanCellLength / subStepCount; // cell divided into npassos
    }
    double subStepTemperature = subStepLength * (-(-leftCell.temp) / subStepLength + sourceTemperatureGradient + heatFluxTemperatureGradient); // first step
    for (int j = 1; j < subStepCount; j++) {                                                                                                   // next steps
        // Keep all energy terms except heat flow, which is recalculated at each step.
        leftCell.calor.Tint = subStepTemperature;
        leftCell.calor.Vint = meanSuperficialGasVelocity + meanSuperficialLiquidVelocity;
        liquidConductivity = (1. - betmed) * leftCell.flui.CondLiq(meanPressure, subStepTemperature) + betmed * leftCell.fluicol.CondLiq(meanPressure, subStepTemperature);
        leftCell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + leftCell.flui.CondGas(meanPressure, subStepTemperature) * meanVoidFraction;
        leftCell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
        leftCell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
        liquidViscosity = (1. - betmed) * leftCell.flui.ViscOleo(meanPressure, subStepTemperature) + betmed * leftCell.fluicol.VisFlu(meanPressure, subStepTemperature);
        leftCell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + leftCell.flui.ViscGas(meanPressure, subStepTemperature) * meanVoidFraction * units::kPascalSecondPerCentipoise;
        if (state.steadyIteration != 0 && state.input.lingas == 1 && (cellIndex - 1 <= state.annulusTubingStart && cellIndex - 1 >= state.annulusTubingEnd)) {
            int k = state.annulusTubingStart + state.tubingAnnulusStart - (cellIndex - 1);
            double externalTemperatureStep = (state.gasCells[k - 1].temp - state.gasCells[k].temp) / subStepCount;
            leftCell.calor.Textern1 = state.gasCells[k].temp + (j - 1) * externalTemperatureStep;
        }
        heatFlux = mixtureFluxSign * leftCell.calor.transperm(annulusResistance);
        leftCell.fluxcalmed += heatFlux;
        heatFluxTemperatureGradient = meanTemperatureGradientCorrection * (heatFlux) / temperatureSpatialCoefficient;
        subStepTemperature = subStepLength * (-(-subStepTemperature) / subStepLength + sourceTemperatureGradient + heatFluxTemperatureGradient);
    }

    cell.temp = subStepTemperature;
    leftCell.fluxcalmed /= subStepCount;
    leftCell.fluxcalmed = 1. * leftCell.fluxcalmed;

    if (cell.temp < kMinimumTemperatureCelsius)
        cell.temp = kMinimumTemperatureCelsius;
    if (cell.temp > kMaximumTemperatureCelsius)
        cell.temp = kMaximumTemperatureCelsius;
}

void advanceReverseSteadyTemperatureSubsteps(const ThermalState &state, int cellIndex,
                                             const SteadyTemperatureStep &step, double heatFluxTemperatureGradient) {
    Cel &cell = state.cells[cellIndex];
    Cel &rightCell = state.cells[cellIndex + 1];
    const double meanPressure = step.meanPressure;
    const double meanVoidFraction = step.meanVoidFraction;
    const double betmed = step.betmed;
    const double liquidDensity = step.liquidDensity;
    const double gasDensity = step.gasDensity;
    const double liquidSpecificHeat = step.liquidSpecificHeat;
    const double gasSpecificHeat = step.gasSpecificHeat;
    const double meanSuperficialGasVelocity = step.meanSuperficialGasVelocity;
    const double meanSuperficialLiquidVelocity = step.meanSuperficialLiquidVelocity;
    const double meanCellLength = step.meanCellLength;
    const double meanTemperatureGradientCorrection = step.meanTemperatureGradientCorrection;
    const double temperatureSpatialCoefficient = step.temperatureSpatialCoefficient;
    const double annulusResistance = step.annulusResistance;
    const double mixtureFluxSign = step.mixtureFluxSign;
    const double sourceTemperatureGradient = step.sourceTemperatureGradient;
    double liquidConductivity;
    double liquidViscosity;
    double heatFlux;
    int subStepCount;
    double subStepLength;
    double stabilityLength = fabs(temperatureSpatialCoefficient) / meanTemperatureGradientCorrection;
    if (meanCellLength / (rightCell.calor.resGlob + annulusResistance) < (stabilityLength + 0. * 1000.)) {
        subStepCount = 1;
        subStepLength = meanCellLength;
    } else { // small thermal resistance: determines in how many steps
        // the temperature advance is made
        subStepCount = (meanCellLength / (rightCell.calor.resGlob + annulusResistance)) / (stabilityLength + 0 * 1000) + 1;
        subStepLength = meanCellLength / subStepCount; // cell divided into npassos
    }
    double subStepTemperature = subStepLength * (-(-rightCell.temp) / subStepLength + sourceTemperatureGradient + heatFluxTemperatureGradient); // first step
    for (int j = 1; j < subStepCount; j++) {                                                                                                    // following steps
        // the energy terms are kept, except the heat flux, which is
        // re-evaluated at each step:
        rightCell.calor.Tint = subStepTemperature;
        rightCell.calor.Vint = meanSuperficialGasVelocity + meanSuperficialLiquidVelocity;
        liquidConductivity = (1. - betmed) * rightCell.flui.CondLiq(meanPressure, subStepTemperature) + betmed * rightCell.fluicol.CondLiq(meanPressure, subStepTemperature);
        rightCell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + rightCell.flui.CondGas(meanPressure, subStepTemperature) * meanVoidFraction;
        rightCell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
        rightCell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
        liquidViscosity = (1. - betmed) * rightCell.flui.ViscOleo(meanPressure, subStepTemperature) + betmed * rightCell.fluicol.VisFlu(meanPressure, subStepTemperature);
        rightCell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + rightCell.flui.ViscGas(meanPressure, subStepTemperature) * meanVoidFraction * units::kPascalSecondPerCentipoise;
        if (state.steadyIteration != 0 && state.input.lingas == 1 && (cellIndex + 1 <= state.annulusTubingStart && cellIndex + 1 >= state.annulusTubingEnd)) {
            int k = state.annulusTubingStart + state.tubingAnnulusStart - (cellIndex + 1);
            double externalTemperatureStep = (state.gasCells[k].temp - state.gasCells[k - 1].temp) / subStepCount;
            rightCell.calor.Textern1 = state.gasCells[k].temp + (j - 1) * externalTemperatureStep;
        }
        heatFlux = mixtureFluxSign * rightCell.calor.transperm(annulusResistance);
        rightCell.fluxcalmed += heatFlux;
        heatFluxTemperatureGradient = meanTemperatureGradientCorrection * (heatFlux) / temperatureSpatialCoefficient;
        subStepTemperature = subStepLength * (-(-subStepTemperature) / subStepLength + sourceTemperatureGradient + heatFluxTemperatureGradient);
    }

    cell.temp = subStepTemperature;
    rightCell.fluxcalmed /= subStepCount;
    rightCell.fluxcalmed = 1. * rightCell.fluxcalmed;

    if (cell.temp < kMinimumTemperatureCelsius)
        cell.temp = kMinimumTemperatureCelsius;
    if (cell.temp > kMaximumTemperatureCelsius)
        cell.temp = kMaximumTemperatureCelsius;
}

void prepareTransientOutletFlow(const ThermalState &state) {
    if (((*state.globals).networkMode == 0 || state.endNode == 1 || (*state.globals).parallelNetworkMode == 1) && state.input.chkv == 0) {
        if (state.cells[state.lastCell - 1].MliqiniR < 0) {
            state.cells[state.lastCell - 1].MR = state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR;
            state.cells[state.lastCell - 1].MliqiniR = 0;
            state.cells[state.lastCell - 1].term1R = 0;
            state.cells[state.lastCell - 1].term2R = 0;
            state.cells[state.lastCell].MC = state.cells[state.lastCell].MC - state.cells[state.lastCell].Mliqini;
            state.cells[state.lastCell].Mliqini = 0;
            state.cells[state.lastCell].term1 = 0;
            state.cells[state.lastCell].term2 = 0;
            state.cells[state.lastCell - 1].QLR = 0.;
            state.cells[state.lastCell].QL = 0.;
        }
        if (state.cells[state.lastCell - 1].QLR < 0 && (state.surfaceChokeMassFlag == 0 || state.surfaceChokeOpen == 1)) {
            state.cells[state.lastCell - 1].QLR = 0;
            state.cells[state.lastCell].QL = 0;
        }
    } else if (state.input.chkv == 1 && (state.surfaceChokeMassFlag == 0 || state.surfaceChokeMassFlag == 1)) {
        if (state.cells[state.lastCell - 1].MliqiniR < 0) {
            state.cells[state.lastCell - 1].MR = 0.;
            state.cells[state.lastCell - 1].MliqiniR = 0;
            state.cells[state.lastCell - 1].term1R = 0;
            state.cells[state.lastCell - 1].term2R = 0;
            state.cells[state.lastCell].MC = 0.;
            state.cells[state.lastCell].Mliqini = 0;
            state.cells[state.lastCell].term1 = 0;
            state.cells[state.lastCell].term2 = 0;
            state.cells[state.lastCell - 1].QLR = 0.;
            state.cells[state.lastCell].QL = 0.;
            state.cells[state.lastCell].QG = 0.;
        }
        if (state.cells[state.lastCell - 1].QLR < 0 && (state.surfaceChokeMassFlag == 0 || state.surfaceChokeOpen == 1)) {
            state.cells[state.lastCell - 1].QLR = 0;
            state.cells[state.lastCell].QL = 0;
        }
    }
}

void advanceCoupledThermalField(const ThermalState &state, int cycle, int maximumCycle) {
    const auto prepareInterface = [&](int coupled) {
        const int cellIndex = state.input.celAcop[coupled].indCel;
        prepareNonDimensionalHeatDiffusion(state, cellIndex);
        const int nextCouplingIndex = state.coupledCellIndices[coupled];
        state.poissonSolver3D.dados.tInt[nextCouplingIndex] = state.cells[cellIndex].temp;
        const double cellInterfaceCoefficient = state.cells[cellIndex].calor.hInt();
        state.poissonSolver3D.dados.hI[nextCouplingIndex] = cellInterfaceCoefficient;
    };
    const int threadCount = std::max(1, std::min(state.globals->solverThreadCount, state.input.nacop));
    if (threadCount == 1) {
        for (int coupled = 0; coupled < state.input.nacop; ++coupled)
            prepareInterface(coupled);
    } else {
#pragma omp parallel for num_threads(threadCount) default(none) shared(state, prepareInterface) schedule(static)
        for (int coupled = 0; coupled < state.input.nacop; ++coupled)
            prepareInterface(coupled);
    }
    if (cycle < maximumCycle || maximumCycle == 0 || state.minimumCycleTimeStep != state.timeStep) {
        if (cycle == maximumCycle && maximumCycle > 0)
            state.poissonSolver3D.FeiticoDoTempo();
        state.poissonSolver3D.transientePoisson(state.timeStep);
    }
    refreshTemperatureField(state);
}

MassTransferWaterCuts prepareMassTransferDonors(
    const ThermalState &state, int cellIndex, bool leftCellHasNegativeLiquidMassFlowRate,
    ProFlu &leftCellLeftFaceFluid, ProFlu &leftFaceFluid) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double leftFaceWaterCut;
    double leftCellLeftFaceWaterCut;
    double leftCellOilVolumeFactorForWaterCut = leftCell.flui.BOFunc(
        leftCell.pres, leftCell.temp);
    double leftCellWaterVolumeFactorForWaterCut = leftCell.flui.BAFunc(
        leftCell.pres, leftCell.temp);
    double leftCellWaterCut = leftCell.flui.BSW * leftCellWaterVolumeFactorForWaterCut /
                              (leftCellOilVolumeFactorForWaterCut + leftCellWaterVolumeFactorForWaterCut * leftCell.flui.BSW -
                               leftCell.flui.BSW * leftCellOilVolumeFactorForWaterCut);
    if (cell.Mliqini < 0.) {
        leftFaceFluid = cell.flui;
        double leftFaceDonorOilVolumeFactor = leftFaceFluid.BOFunc(cell.pres, cell.temp);
        double leftFaceDonorWaterVolumeFactor = leftFaceFluid.BAFunc(cell.pres, cell.temp);
        leftFaceWaterCut = leftFaceFluid.BSW * leftFaceDonorWaterVolumeFactor / (leftFaceDonorOilVolumeFactor + leftFaceDonorWaterVolumeFactor * leftFaceFluid.BSW - leftFaceFluid.BSW * leftFaceDonorOilVolumeFactor);
    } else {
        leftFaceFluid = leftCell.flui;
        double leftFaceDonorOilVolumeFactor = leftFaceFluid.BOFunc(
            leftCell.pres, leftCell.temp);
        double leftFaceDonorWaterVolumeFactor = leftFaceFluid.BAFunc(
            leftCell.pres, leftCell.temp);
        leftFaceWaterCut = leftFaceFluid.BSW * leftFaceDonorWaterVolumeFactor / (leftFaceDonorOilVolumeFactor + leftFaceDonorWaterVolumeFactor * leftFaceFluid.BSW - leftFaceFluid.BSW * leftFaceDonorOilVolumeFactor);
    }
    if (leftCellHasNegativeLiquidMassFlowRate) {
        leftCellLeftFaceFluid = leftCell.flui;
        double leftCellLeftFaceDonorOilVolumeFactor = leftCellLeftFaceFluid.BOFunc(
            leftCell.pres, leftCell.temp);
        double leftCellLeftFaceDonorWaterVolumeFactor = leftCellLeftFaceFluid.BAFunc(
            leftCell.pres, leftCell.temp);
        leftCellLeftFaceWaterCut = leftCellLeftFaceFluid.BSW * leftCellLeftFaceDonorWaterVolumeFactor / (leftCellLeftFaceDonorOilVolumeFactor + leftCellLeftFaceDonorWaterVolumeFactor * leftCellLeftFaceFluid.BSW - leftCellLeftFaceFluid.BSW * leftCellLeftFaceDonorOilVolumeFactor);

    } else {
        if (cellIndex > 1) {
            leftCellLeftFaceFluid = state.cells[cellIndex - 2].flui;
            double leftCellLeftFaceDonorOilVolumeFactor = leftCellLeftFaceFluid.BOFunc(
                state.cells[cellIndex - 2].pres, state.cells[cellIndex - 2].temp);
            double leftCellLeftFaceDonorWaterVolumeFactor = leftCellLeftFaceFluid.BAFunc(
                state.cells[cellIndex - 2].pres, state.cells[cellIndex - 2].temp);
            leftCellLeftFaceWaterCut = leftCellLeftFaceFluid.BSW * leftCellLeftFaceDonorWaterVolumeFactor /
                                       (leftCellLeftFaceDonorOilVolumeFactor + leftCellLeftFaceDonorWaterVolumeFactor * leftCellLeftFaceFluid.BSW - leftCellLeftFaceFluid.BSW * leftCellLeftFaceDonorOilVolumeFactor);
        } else {
            leftCellLeftFaceFluid = leftCell.flui;
            double leftCellLeftFaceDonorOilVolumeFactor = leftCellLeftFaceFluid.BOFunc(
                leftCell.pres, leftCell.temp);
            double leftCellLeftFaceDonorWaterVolumeFactor = leftCellLeftFaceFluid.BAFunc(
                leftCell.pres, leftCell.temp);
            leftCellLeftFaceWaterCut = leftCellLeftFaceFluid.BSW * leftCellLeftFaceDonorWaterVolumeFactor /
                                       (leftCellLeftFaceDonorOilVolumeFactor + leftCellLeftFaceDonorWaterVolumeFactor * leftCellLeftFaceFluid.BSW - leftCellLeftFaceFluid.BSW * leftCellLeftFaceDonorOilVolumeFactor);
        }
    }
    return MassTransferWaterCuts{leftFaceWaterCut, leftCellLeftFaceWaterCut, leftCellWaterCut};
}

void updateEquilibriumMassTransferDerivatives(
    const ThermalState &state, int cellIndex,
    const ProFlu &leftCellLeftFaceFluid, const ProFlu &leftFaceFluid,
    const DistributedMassTransferProperties &properties,
    const DistributedMassTransferCoefficients &coefficients, const MassTransferFaceState &previousFace) {
    const double activeDerivative = coefficients.activeDerivative;
    const double spatialCoupling = coefficients.spatialCoupling;
    const double coefficientFlowArea = coefficients.flowArea;
    const double leftCellLeftFaceMixtureLiquidDensity = previousFace.liquidDensity;
    const double leftCellLeftFaceOilVolumeFactor = previousFace.oilVolumeFactor;
    const double leftCellLeftFaceSolutionGasRatio = previousFace.solutionGasRatio;
    const double leftCellLeftFaceSolutionGasPressureDerivative = previousFace.solutionGasPressureDerivative;
    const double leftFaceWaterCut = properties.leftFaceWaterCut;
    const double leftCellLeftFaceWaterCut = properties.leftCellLeftFaceWaterCut;
    const double leftCellWaterCut = properties.leftCellWaterCut;
    const double betI = properties.leftFaceComposition;
    const double betL = properties.leftCellLeftFaceComposition;
    const double liquidDensity = properties.mixtureLiquidDensity;
    const double leftFaceOilVolumeFactor = properties.leftFaceOilVolumeFactor;
    const double leftFaceSolutionGasRatio = properties.leftFaceSolutionGasRatio;
    const double leftFaceSolutionGasPressureDerivative =
        properties.leftFaceSolutionGasPressureDerivative;
    const double leftCellOilVolumeFactor = properties.leftCellOilVolumeFactor;
    const double leftCellSolutionGasRatio = properties.leftCellSolutionGasRatio;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    cell.DTransDxR =
        -((1 - betI) * (leftFaceFluid.rDgD) * leftFaceFluid.Deng * kAirDensityAtStandardConditions *
          (1. - leftFaceWaterCut) * leftFaceSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftFaceOilVolumeFactor) /
        (liquidDensity * leftCell.dx);
    cell.DtransDxLinear =
        -spatialCoupling * cell.QL *
            ((1 - betI) * (leftFaceFluid.rDgD) * leftFaceFluid.Deng * kAirDensityAtStandardConditions *
             (1. - leftFaceWaterCut) * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
             (leftFaceSolutionGasPressureDerivative * cell.dpresaux)) /
            (leftCell.dx) +
        spatialCoupling * cell.QL *
            ((1 - betI) * (leftFaceFluid.rDgD) * leftFaceFluid.Deng * kAirDensityAtStandardConditions *
             (1. - leftFaceWaterCut) * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
             (leftFaceSolutionGasPressureDerivative * cell.presaux)) /
            (leftCell.dx);
    cell.DTransDxRp =
        -spatialCoupling * cell.QL *
        ((1 - betI) * (leftFaceFluid.rDgD) * leftFaceFluid.Deng * kAirDensityAtStandardConditions *
         (1. - leftFaceWaterCut) * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) * 0.5 * leftFaceSolutionGasPressureDerivative) /
        (leftCell.dx);
    cell.DTransDxL =
        ((1 - betL) * (leftCellLeftFaceFluid.rDgD) * leftCellLeftFaceFluid.Deng * kAirDensityAtStandardConditions *
         (1. - leftCellLeftFaceWaterCut) * leftCellLeftFaceSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellLeftFaceOilVolumeFactor) /
        (leftCellLeftFaceMixtureLiquidDensity * leftCell.dx);
    cell.DtransDxLinear =
        cell.DtransDxLinear +
        spatialCoupling * leftCell.QL *
            ((1 - betL) * (leftCellLeftFaceFluid.rDgD) * leftCellLeftFaceFluid.Deng * kAirDensityAtStandardConditions *
             (1. - leftCellLeftFaceWaterCut) * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
             (leftCellLeftFaceSolutionGasPressureDerivative * leftCell.dpresaux)) /
            (leftCell.dx) -
        spatialCoupling * leftCell.QL *
            ((1 - betL) * (leftCellLeftFaceFluid.rDgD) * leftCellLeftFaceFluid.Deng * kAirDensityAtStandardConditions *
             (1. - leftCellLeftFaceWaterCut) * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) *
             (leftCellLeftFaceSolutionGasPressureDerivative * leftCell.presaux)) /
            (leftCell.dx);
    cell.DTransDxLp =
        spatialCoupling * leftCell.QL *
        ((1 - betL) * (leftCellLeftFaceFluid.rDgD) * leftCellLeftFaceFluid.Deng * kAirDensityAtStandardConditions *
         (1. - leftCellLeftFaceWaterCut) * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) * 0.5 * leftCellLeftFaceSolutionGasPressureDerivative) /
        (leftCell.dx);
    cell.DTransDt1 =
        -activeDerivative * ((1. - leftCellWaterCut) * coefficientFlowArea *
                             (leftCell.flui.rDgD) *
                             leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio *
                             (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor);
    cell.DTransDt0 = -cell.DTransDt1;

    leftCell.CoefDTR =
        -((1. - leftCell.bet) * (1. - leftCellWaterCut) * coefficientFlowArea *
          (leftCell.flui.rDgD) *
          leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio *
          (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor);
    leftCell.CoefDTL = -leftCell.CoefDTR;
    leftCell.coefTransBet =
        ((1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) *
         leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio *
         (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor);

    cell.transmassL -=
        activeDerivative * ((1. - leftCell.bet) * (1. - leftCell.alf) * (1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) * leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor) /
        leftCell.dt;
    cell.transmassL +=
        activeDerivative * ((1. - leftCell.betini) * (1. - leftCell.alfini) * (1. - leftCellWaterCut) * coefficientFlowArea * (leftCell.flui.rDgD) * leftCell.flui.Deng * kAirDensityAtStandardConditions * leftCellSolutionGasRatio * (units::kBarrelPerCubicMetre / units::kCubicFootPerCubicMetre) / leftCellOilVolumeFactor) /
        leftCell.dt;
}

void updateDistributedMassTransferCell(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, MassTransferFaceState &previousFace) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    Cel &rightCell = state.cells[cellIndex + 1];
    sources::renewSourceTerms(dependencies.sources, cellIndex);

    if (cellIndex < state.lastCell) {
        rightCell.fontemassLLini = cell.fontemassLR;
        rightCell.fontemassCLini = cell.fontemassCR;
        rightCell.fontemassGLini = cell.fontemassGR;
        rightCell.fontemassLL = cell.fontemassLR;
        rightCell.fontemassCL = cell.fontemassCR;
        rightCell.fontemassGL = cell.fontemassGR;
    }
    double meanTemperature = leftCell.temp;
    if (cell.VTemper < 0.)
        meanTemperature = cell.temp;

    double leftAbsoluteSuperficialVelocity = (fabs(leftCell.QG) + fabs(leftCell.QL)) / leftCell.duto.area;

    ProFlu leftCellLeftFaceFluid;
    ProFlu leftFaceFluid;
    DistributedMassTransferProperties properties =
        prepareDistributedMassTransferProperties(
            state, cellIndex, meanTemperature, leftCellLeftFaceFluid, leftFaceFluid);
    double leftCellWaterCut = properties.leftCellWaterCut;

    double mixtureLiquidDensity = properties.mixtureLiquidDensity;
    double leftFaceOilVolumeFactor = properties.leftFaceOilVolumeFactor;
    double leftFaceSolutionGasRatio = properties.leftFaceSolutionGasRatio;
    double leftFaceSolutionGasPressureDerivative =
        properties.leftFaceSolutionGasPressureDerivative;
    double leftCellSolutionGasPressureDerivative = properties.leftCellSolutionGasPressureDerivative;
    double leftCellSolutionGasTemperatureDerivative =
        properties.leftCellSolutionGasTemperatureDerivative;
    DistributedMassTransferCoefficients coefficients =
        updateDistributedMassTransferDerivatives(
            state, cellIndex, leftCellWaterCut, leftFaceFluid, leftCellSolutionGasPressureDerivative, leftCellSolutionGasTemperatureDerivative);

    applyDistributedMassTransferModel(
        state, cellIndex, leftAbsoluteSuperficialVelocity, leftCellLeftFaceFluid, leftFaceFluid, properties,
        coefficients, previousFace);
    finalizeDistributedMassTransferFace(state, cellIndex, properties, coefficients);

    previousFace.liquidDensity = mixtureLiquidDensity;
    previousFace.oilVolumeFactor = leftFaceOilVolumeFactor;
    previousFace.solutionGasRatio = leftFaceSolutionGasRatio;
    previousFace.solutionGasPressureDerivative = leftFaceSolutionGasPressureDerivative;
}

void finalizeDistributedMassTransferFace(
    const ThermalState &state, int cellIndex, const DistributedMassTransferProperties &properties,
    const DistributedMassTransferCoefficients &coefficients) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    const double betI = properties.leftFaceComposition;
    const double gasDensity = properties.gasDensity;
    const double liquidDensity = properties.liquidDensity;
    const double coefficientFlowArea = coefficients.flowArea;
    if (leftCell.TMModel == -2) {
        double hydrateTransportVelocity;
        if (cell.alfL > (*state.globals).smallValueTolerance && betI < (1. - (*state.globals).smallValueTolerance))
            hydrateTransportVelocity = (cell.QG * gasDensity + cell.QL * (1. - betI) * liquidDensity) / (coefficientFlowArea * (cell.alfL * gasDensity + (1. - cell.alfL) * (1. - betI) * liquidDensity));
        else
            hydrateTransportVelocity = 0.;
        double hydrateMassFraction = leftCell.flui.FracMassHidra(leftCell.pres, leftCell.temp);
        double pressurePerturbationRatio = kDerivativePerturbationFactor;
        double hydrateFractionPressureDerivative = (hydrateMassFraction - leftCell.flui.FracMassHidra(leftCell.pres * pressurePerturbationRatio, leftCell.temp)) / ((1 - pressurePerturbationRatio) * leftCell.pres);
        double pressureGradient;
        pressureGradient = (cell.presaux - leftCell.presaux) / cell.dxL;
        cell.transmassL = leftCell.transmassR = 1 * (cell.alfL * gasDensity + (1. - cell.alfL) * (1. - betI) * liquidDensity) * hydrateTransportVelocity * (pressureGradient * hydrateFractionPressureDerivative) * coefficientFlowArea;

        cell.DTransDxR = 0.;
        cell.DTransDxL = 0.;
        cell.DTransDt1 = 0.;
        cell.DTransDt0 = 0.;
        if (state.input.desligaDeriTransMassDTemp == 1) {
            leftCell.DTransDtT = 0;
            cell.DTransDtTL = 0.;
        }
        leftCell.CoefDTR = 0.;
        leftCell.CoefDTL = 0.;
        leftCell.coefTransBet = 0.;
    }
    if (leftCell.TMModel == 3) {
        cell.transmassL = leftCell.transmassR = 0.;
        clearMassTransferDerivatives(state, cellIndex);
    }
    if (leftCell.transmassR > 0 && (cell.alfL > (1. - (*state.globals).smallValueTolerance) || cell.betL > (1. - (*state.globals).smallValueTolerance))) {
        cell.transmassL = leftCell.transmassR = -(*state.globals).smallValueTolerance;
        clearMassTransferDerivatives(state, cellIndex);
    }
    if (leftCell.transmassR < 0 && cell.alfL < (*state.globals).smallValueTolerance) {
        cell.transmassL = leftCell.transmassR = (*state.globals).smallValueTolerance;
        clearMassTransferDerivatives(state, cellIndex);
    }
}

void applyInteriorDriftPartition(
    const ThermalState &state, const ThermalDependencies &dependencies, int cellIndex, double superficialGasVelocity,
    double superficialLiquidVelocity, double gasDensity, double liquidDensity, double flowArea, int explicitMode) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    double c0;
    double ud;
    const double meanVoidFraction = superficialGasVelocity < 0 ? cell.alfPigE : leftCell.alfPigD;
    cell.alfI = meanVoidFraction;
    const double meanDiameter = cell.MC >= 0 ? cell.dutoL.a : cell.duto.a;
    if (fabs(cell.QG / (0.25 * std::numbers::pi * meanDiameter * meanDiameter * meanVoidFraction)) > 100. ||
        fabs(cell.QL / (0.25 * std::numbers::pi * meanDiameter * meanDiameter * (1. - meanVoidFraction))) > 100. || explicitMode == 1) {
        c0 = 1.;
        ud = 0.;
    } else
        driftflux::coefficient::instantaneous(dependencies.closure, cellIndex, c0, ud);
    cell.c0 = c0;
    cell.ud = ud;
    if (cellIndex == state.lastCell) {
        double numerator = (1. - meanVoidFraction * c0);
        double denominator = 1 + c0 * meanVoidFraction * ((gasDensity / liquidDensity) - 1.);
        if (fabs(denominator) > 1e-15) {
            cell.term1 = numerator / denominator;
            cell.term2 = (-flowArea * meanVoidFraction * gasDensity * ud) / denominator;
        } else {
            cell.term1 = meanVoidFraction < 0.5 ? 1. : 0.;
            cell.term2 = 0.;
        }
        double previousLiquidDriftFlux = (superficialGasVelocity - meanVoidFraction * ud) / (meanVoidFraction * c0) - superficialGasVelocity;
        double liquidDriftFlux = (superficialGasVelocity + superficialLiquidVelocity) * (1. - c0 * meanVoidFraction) - meanVoidFraction * ud;
        if ((liquidDriftFlux > 0. || previousLiquidDriftFlux > 0.) && leftCell.alfPigD > 1 - 1e-15) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
        if ((liquidDriftFlux < 0. || previousLiquidDriftFlux < 0.) && cell.alfPigE > 1 - 1e-15) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
        if (leftCell.acsr.tipo == kAccessoryChoke && leftCell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * leftCell.duto.area) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
    }
}

void finalizeFlowPartitionCell(
    const ThermalState &state, int cellIndex, int isShutIn,
    const Vcr<int> &bif, const Vcr<int> &valv) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    if (bif[cellIndex] == 1 && valv[cellIndex] == 1) {
        double betI;
        double liquidDensity;

        if (cell.QL < 0.) {
            betI = cell.betPigE;
            liquidDensity = (1 - betI) * cell.rpCi + betI * cell.rcCi;
        } else {
            betI = leftCell.betPigD;
            liquidDensity = (1 - betI) * leftCell.rpCi + betI * leftCell.rcCi;
        }

        const UpstreamFaceBasis upstream = upstreamFaceBasisOf(state, cellIndex);
        const double gasDensity = upstream.gasDensity;
        const double flowArea = upstream.flowArea;
        double superficialGasVelocity = cell.QG / (flowArea);
        double superficialLiquidVelocity = cell.QL / (flowArea);

        double meanVoidFraction;
        meanVoidFraction = leftCell.alfPigD;
        if (superficialGasVelocity < 0)
            meanVoidFraction = cell.alfPigE;
        double numerator = (1. - meanVoidFraction * cell.c0);
        double denominator = 1 + cell.c0 * meanVoidFraction * ((gasDensity / liquidDensity) - 1.);
        if (fabs(denominator) > 1e-15) {
            cell.term1 = numerator / denominator;
            cell.term2 = (-flowArea * meanVoidFraction * gasDensity * cell.ud) / denominator;
        } else {
            cell.term1 = meanVoidFraction < 0.5 ? 1. : 0.;
            cell.term2 = 0.;
        }
        double previousLiquidDriftFlux;
        if (meanVoidFraction > 1e-15)
            previousLiquidDriftFlux = (superficialGasVelocity - meanVoidFraction * cell.ud) / (meanVoidFraction * cell.c0) - superficialGasVelocity;
        else
            previousLiquidDriftFlux = superficialLiquidVelocity;
        double liquidDriftFlux = (superficialGasVelocity + superficialLiquidVelocity) * (1. - cell.c0 * meanVoidFraction) - meanVoidFraction * cell.ud;
        double candidateLiquidMass = cell.term1 * cell.MC + cell.term2;
        double candidateGasMass = (1 - cell.term1) * cell.MC - cell.term2;
        if ((liquidDriftFlux > 0. || previousLiquidDriftFlux > 0.) && leftCell.alfPigD > 1 - 1e-15) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
        if ((liquidDriftFlux < 0. || previousLiquidDriftFlux < 0.) && cell.alfPigE > 1 - 1e-15) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
        if (leftCell.acsr.tipo == kAccessoryChoke && leftCell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * leftCell.duto.area) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
        if ((liquidDriftFlux < 0. || previousLiquidDriftFlux < 0.) && ((fabs(leftCell.QG / (flowArea)) + fabs(leftCell.QL / (flowArea))) < 0.1) &&
            (isShutIn == 1 && state.input.modoSegrega == 1) && (cell.duto.teta > 0 && leftCell.duto.teta <= 0) &&
            (candidateLiquidMass > 0 && cell.term2 > 0) &&
            ((1. - cell.alfL) < (1. - cell.alf))) {
            cell.term2 = 0.;
        }
        if (cell.duto.teta > 0 && candidateGasMass < 0 && ((isShutIn == 1 && state.input.modoSegrega == 1)) && cell.alfPigE > 1 - 1e-15) {
            cell.term1 = 0.;
            cell.term2 = 0.;
            cell.alfI = 1.;
        }
    }
}


double temperatureInterfacialWorkOf(const ThermalState &state, int cellIndex,
                                    const TemperatureBalance &balance) {
    const Cel &cell = state.cells[cellIndex];
    const bool positiveGasFlow = balance.gasSuperficialVelocity > 0.;
    const double fraction = positiveGasFlow ? cell.alf : cell.alfR;
    const double leftFraction = positiveGasFlow ? cell.alfL : cell.alf;
    double slipVelocity = balance.liquidSuperficialVelocity;
    if (fraction > state.globals->smallValueTolerance && fraction < 1. - state.globals->smallValueTolerance)
        slipVelocity = balance.gasSuperficialVelocity / fraction -
                       balance.liquidSuperficialVelocity / (1. - fraction);
    else if (fraction > state.globals->smallValueTolerance)
        slipVelocity = balance.gasSuperficialVelocity;
    return balance.flowArea * cell.pres * units::kPascalPerKgfPerCm2 *
           slipVelocity * (fraction - leftFraction) / cell.dx;
}

double explicitTemperatureCandidate(const ThermalState &state, int cellIndex, TemperatureBalance &balance) {
    const Cel &cell = state.cells[cellIndex];
    double pressureGradient = temperaturePressureGradientOf(state, cellIndex, balance);
    const double temperatureGradient = temperatureSpatialGradientOf(state, cellIndex, balance, pressureGradient);
    const double kineticTerm = computeKineticTemperatureTerm(state, cellIndex, balance);
    const auto source = computeTemperatureSourceTerms(state, cellIndex, balance.cellLength);
    const double latentHeat = computeTemperatureLatentHeatTerm(state, cellIndex);
    const double interfacialWork = temperatureInterfacialWorkOf(state, cellIndex, balance);
    const double leftThermalPower = cellIndex > 0 ? state.cells[cellIndex - 1].acsr.potTermo : 0.;
    return ((balance.timeCoefficient / cell.dt) * cell.temp -
            (-balance.pressureTimeCoefficient * (cell.pres - cell.presini) * units::kPascalPerKgfPerCm2 / cell.dt) +
            cell.dTdLCor * (-balance.temperatureSpatialCoefficient * temperatureGradient +
                balance.pressureSpatialCoefficient * pressureGradient - kineticTerm -
                (balance.hydrostaticPower - 0. * interfacialWork) +
                (leftThermalPower + cell.fonteCal) / balance.meanCellLength +
                source.liquid + source.gas + balance.heatFlux - latentHeat) -
            (balance.rc - balance.rp) * (1. - balance.voidFraction) * cell.pres * units::kPascalPerKgfPerCm2 *
                balance.flowArea * (cell.bet - cell.betini) / (balance.liquidDensity * cell.dt)) /
           (balance.timeCoefficient / cell.dt);
}

void advanceExplicitCellTemperature(const ThermalState &state, int cellIndex, TemperatureBalance &balance) {
    Cel &cell = state.cells[cellIndex];
    const double candidate = explicitTemperatureCandidate(state, cellIndex, balance);
    const double minimumTemperature = state.input.usaTabela == 1
                                          ? state.input.tabent.tmin + 1.
                                          : kMinimumTemperatureCelsius;
    cell.temp = detail::limitExplicitTemperature(cell.tempini, candidate, cell.dt,
        cell.VTemper, balance.referenceMixtureVelocity, minimumTemperature);
}

void computeCellTemperature(const ThermalState &state, int cellIndex, int steadyMode) {
    Cel &cell = state.cells[cellIndex];
    if (state.thermalSourceDisabled != 0) {
        cell.temp = cell.calor.Textern1;
        return;
    }
    const auto transport = temperatureTransportOf(state, cellIndex);
    auto balance = prepareTemperatureBalance(state, cellIndex, steadyMode, transport);
    cell.VTemper = balance.temperatureSpatialCoefficient / balance.timeCoefficient;
    if ((cellIndex == 0 && cell.VTemper < 0.) ||
        (cellIndex > 0 && (cellIndex < state.lastCell || cell.VTemper >= 0.)) ||
        (cellIndex == state.lastCell && (state.surfaceChokeMassFlag == 1 || state.input.chkv == 1)))
        advanceExplicitCellTemperature(state, cellIndex, balance);
    else if (cellIndex == state.lastCell)
        cell.temp = state.globals->networkMode == 0 || state.endNode == 1 || state.globals->parallelNetworkMode == 1
                        ? cell.calor.Textern1 : state.gasSurfaceTemperature;
}

} // namespace

std::vector<int> buildThermalCouplingIndices(const Ler &input, const solverP3D &solver, int lastCell) {
    const int count = input.nacop;
    if (lastCell < 0 || count <= 0 || static_cast<int>(input.celAcop.size()) != count ||
        solver.dados.CC.nAcop != count || !solver.dados.CC.rotuloAcop)
        throw std::invalid_argument("Inconsistent 3D thermal coupling count");
    std::vector<int> indices(count, -1);
    std::vector<bool> usedCells(lastCell + 1, false), usedDestinations(count, false);
    for (int i = 0; i < count; ++i) {
        const auto &coupling = input.celAcop[i];
        if (coupling.indCel < 0 || coupling.indCel > lastCell || usedCells[coupling.indCel])
            throw std::invalid_argument("Invalid or repeated cell in 3D thermal coupling");
        usedCells[coupling.indCel] = true;
        for (int j = 0; j < count; ++j) {
            if (coupling.rotulo == solver.dados.CC.rotuloAcop[j]) {
                if (indices[i] != -1)
                    throw std::invalid_argument("Repeated 3D thermal boundary label: " + coupling.rotulo);
                indices[i] = j;
            }
        }
        if (indices[i] == -1)
            throw std::invalid_argument("3D thermal boundary label not found: " + coupling.rotulo);
        if (usedDestinations[indices[i]])
            throw std::invalid_argument("Repeated 3D thermal coupling destination: " + coupling.rotulo);
        usedDestinations[indices[i]] = true;
    }
    return indices;
}

double interpolateLatentHeat(const ThermalState &state, double pressure, double temperature) {
    const int divisionCount = state.input.tabent.npont - 1;
    if (pressure < state.latentHeatTable[1][0] || pressure >= state.latentHeatTable[divisionCount + 1][0] || temperature < state.latentHeatTable[0][1] || temperature >= state.latentHeatTable[0][divisionCount + 1])
        return 0.;

    const int pressureIndex = bracketingIndex(
        pressure, divisionCount,
        [&](int index) { return state.latentHeatTable[index][0]; }, 0);
    const int temperatureIndex = bracketingIndex(
        temperature, divisionCount,
        [&](int index) { return state.latentHeatTable[0][index]; }, -1);
    double pressureRatio = (state.latentHeatTable[pressureIndex][0] - pressure) / (state.latentHeatTable[pressureIndex][0] - state.latentHeatTable[pressureIndex + 1][0]);
    double temperatureRatio = (state.latentHeatTable[0][temperatureIndex] - temperature) / (state.latentHeatTable[0][temperatureIndex] - state.latentHeatTable[0][temperatureIndex + 1]);
    double latentHeatAtTemperatureIndex = (1 - pressureRatio) * (state.latentHeatTable[pressureIndex][temperatureIndex]) + pressureRatio * (state.latentHeatTable[pressureIndex + 1][temperatureIndex]);
    double latentHeatAtNextTemperatureIndex = (1 - pressureRatio) * (state.latentHeatTable[pressureIndex][temperatureIndex + 1]) + pressureRatio * (state.latentHeatTable[pressureIndex + 1][temperatureIndex + 1]);
    return (1 - temperatureRatio) * latentHeatAtTemperatureIndex + temperatureRatio * latentHeatAtNextTemperatureIndex;
}

double computeMixtureEnthalpy(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    Cel &rightCell = state.cells[cellIndex + 1];

    double cellLength = cell.dx;
    double diameter = cell.duto.a;
    double flowArea = 0.25 * std::numbers::pi * diameter * diameter;
    double betmed = cell.bet;
    double previousMeanVoidFraction = cell.alfini;
    double betmed0 = cell.betini;
    double meanPressure = cell.pres;
    double previousMeanPressure = cell.presini;
    double meanTemperature = cell.temp;
    double leftSuperficialGasVelocity;
    double leftSuperficialLiquidVelocity;
    double rightSuperficialGasVelocity;
    double rightSuperficialLiquidVelocity;

    leftSuperficialGasVelocity = cell.QG / flowArea;
    leftSuperficialLiquidVelocity = cell.QL / flowArea;
    rightSuperficialGasVelocity = rightCell.QG / flowArea;
    rightSuperficialLiquidVelocity = rightCell.QL / flowArea;

    double betL = cell.bet;
    double betR = cell.bet;
    if (leftSuperficialGasVelocity > 0.)
        betL = leftCell.bet;
    if (leftSuperficialGasVelocity < 0.)
        betL = rightCell.bet;

    double meanSuperficialGasVelocity = 0.5 * (leftSuperficialGasVelocity + rightSuperficialGasVelocity);
    double meanSuperficialLiquidVelocity = 0.5 * (leftSuperficialLiquidVelocity + rightSuperficialLiquidVelocity);

    double leftPressure = cell.presaux;
    double rightPressure = rightCell.presaux;
    if (accessoryImposesPressureJump(state, cellIndex)) {
        rightPressure = cell.pres + (cell.pres - cell.presaux) * 0.5;
    }

    double leftTemperature = cell.tempL;
    if (cell.VTemper < 0.)
        leftTemperature = cell.temp;
    double rightTemperature = cell.temp;
    if (rightCell.VTemper < 0.)
        rightTemperature = cell.tempR;

    double gasDensity = cell.flui.MasEspGas(previousMeanPressure, meanTemperature);
    double liquidDensity = cell.flui.MasEspLiq(previousMeanPressure, meanTemperature);
    double rhoc = cell.fluicol.MasEspFlu(previousMeanPressure, meanTemperature);
    double gasDensityAtCellPressure = cell.flui.MasEspGas(meanPressure, meanTemperature);
    double liquidDensityAtCellPressure = cell.flui.MasEspLiq(meanPressure, meanTemperature);
    double gasEnthalpy = cell.flui.EntalpGas(previousMeanPressure, meanTemperature);
    double liquidEnthalpy = cell.flui.EntalpLiq(previousMeanPressure, meanTemperature);
    double hc = cell.fluicol.CalorLiq(previousMeanPressure, meanTemperature) * meanTemperature;

    double leftGasDensity = cell.flui.MasEspGas(leftPressure, leftTemperature);
    double leftLiquidDensity = cell.flui.MasEspLiq(leftPressure, leftTemperature);
    double rhocL = cell.fluicol.MasEspFlu(leftPressure, leftTemperature);
    double leftGasEnthalpy = cell.flui.EntalpGas(leftPressure, leftTemperature);
    double leftLiquidEnthalpy = cell.flui.EntalpLiq(leftPressure, leftTemperature);
    double hcL = cell.fluicol.CalorLiq(leftPressure, leftTemperature) * leftTemperature;

    double rightGasDensity = cell.flui.MasEspGas(rightPressure, rightTemperature);
    double rightLiquidDensity = cell.flui.MasEspLiq(rightPressure, rightTemperature);
    double rhocR = cell.fluicol.MasEspFlu(rightPressure, rightTemperature);
    double rightGasEnthalpy = cell.flui.EntalpGas(rightPressure, rightTemperature);
    double rightLiquidEnthalpy = cell.flui.EntalpLiq(rightPressure, rightTemperature);
    double hcR = cell.fluicol.CalorLiq(rightPressure, rightTemperature) * rightTemperature;

    double previousMixtureInternalEnergy = gasDensity * previousMeanVoidFraction * (gasEnthalpy - previousMeanPressure * units::kPascalPerKgfPerCm2 / gasDensity) + liquidDensity * (1 - betmed0) * (1. - previousMeanVoidFraction) * (liquidEnthalpy - previousMeanPressure * units::kPascalPerKgfPerCm2 / liquidDensity) +
                                           betmed0 * rhoc * (1. - previousMeanVoidFraction) * (hc - previousMeanPressure * units::kPascalPerKgfPerCm2 / rhoc);
    double leftEnthalpyFlux = leftGasDensity * leftSuperficialGasVelocity * leftGasEnthalpy + leftLiquidDensity * (1. - betL) * leftSuperficialLiquidVelocity * leftLiquidEnthalpy + rhocL * betL * leftSuperficialLiquidVelocity * hcL;
    double rightEnthalpyFlux = rightGasDensity * rightSuperficialGasVelocity * rightGasEnthalpy + rightLiquidDensity * (1. - betR) * rightSuperficialLiquidVelocity * rightLiquidEnthalpy + rhocR * betR * rightSuperficialLiquidVelocity * hcR;
    double enthalpyFluxDivergence = (rightEnthalpyFlux - leftEnthalpyFlux) / cellLength;
    double hydrostaticTerm = (gasDensityAtCellPressure * meanSuperficialGasVelocity + (1 - betmed) * meanSuperficialLiquidVelocity * liquidDensityAtCellPressure + betmed * meanSuperficialLiquidVelocity) * sin(cell.duto.teta) * kGravity;

    const SourceEnthalpy source = sourceEnthalpyOf(state, cellIndex);

    const double sourceGasEnthalpy = source.gasEnthalpy;
    const double sourceLiquidEnthalpy = source.liquidEnthalpy;
    const double hcF = source.hcF;

    double liquidMassSourceTerm = 0;
    double gasMassSourceTerm = 0;
    if (cell.fontemassLR > 0.)
        liquidMassSourceTerm = sourceLiquidEnthalpy * cell.fontemassLR / cellLength;
    if (cell.fontemassCR > 0.)
        liquidMassSourceTerm += hcF * cell.fontemassCR / cellLength;
    if (gasMassSourceTerm > 0.)
        gasMassSourceTerm = sourceGasEnthalpy * cell.fontemassGR / cellLength;

    return previousMixtureInternalEnergy - (enthalpyFluxDivergence + hydrostaticTerm - (gasMassSourceTerm + liquidMassSourceTerm) / flowArea) * cell.dt;
}

double interpolateMixtureEnergy(const ThermalState &state, int cellIndex, int pressureIndex, int temperatureIndex, double pressureRatio) {
    Cel &cell = state.cells[cellIndex];
    int nextPressureIndex = pressureIndex + 1;

    double lowerPressure = cell.flui.rhogF[pressureIndex][0];
    double upperPressure = cell.flui.rhogF[nextPressureIndex][0];
    double temperature = cell.flui.rhogF[0][temperatureIndex];
    double meanVoidFraction = cell.alf;
    double betmed = cell.bet;

    double lowerGasDensity = cell.flui.rhogF[pressureIndex][temperatureIndex];
    double upperGasDensity = cell.flui.rhogF[nextPressureIndex][temperatureIndex];
    double lowerGasEnthalpy = cell.flui.HgF[pressureIndex][temperatureIndex];
    double upperGasEnthalpy = cell.flui.HgF[nextPressureIndex][temperatureIndex];

    double lowerLiquidDensity = cell.flui.rholF[pressureIndex][temperatureIndex];
    double upperLiquidDensity = cell.flui.rholF[nextPressureIndex][temperatureIndex];
    double lowerLiquidEnthalpy = cell.flui.HlF[pressureIndex][temperatureIndex];
    double upperLiquidEnthalpy = cell.flui.HlF[nextPressureIndex][temperatureIndex];

    double rhocp0 = cell.fluicol.MasEspFlu(lowerPressure, temperature);
    double rhocp1 = cell.fluicol.MasEspFlu(upperPressure, temperature);
    double hlc0 = cell.fluicol.CalorLiq(lowerPressure, temperature) * temperature;
    double hlc1 = cell.fluicol.CalorLiq(upperPressure, temperature) * temperature;

    double lowerMixtureEnergy = meanVoidFraction * lowerGasDensity * (lowerGasEnthalpy - lowerPressure * units::kPascalPerKgfPerCm2 / lowerGasDensity) +
                                (1 - meanVoidFraction) * (1 - betmed) * lowerLiquidDensity * (lowerLiquidEnthalpy - lowerPressure * units::kPascalPerKgfPerCm2 / lowerLiquidDensity) +
                                (1 - meanVoidFraction) * (betmed)*rhocp0 * (hlc0 - lowerPressure * units::kPascalPerKgfPerCm2 / rhocp0);

    double upperMixtureEnergy = meanVoidFraction * upperGasDensity * (upperGasEnthalpy - upperPressure * units::kPascalPerKgfPerCm2 / lowerGasDensity) +
                                (1 - meanVoidFraction) * (1 - betmed) * upperLiquidDensity * (upperLiquidEnthalpy - upperPressure * units::kPascalPerKgfPerCm2 / upperLiquidDensity) +
                                (1 - meanVoidFraction) * (betmed)*rhocp1 * (hlc1 - upperPressure * units::kPascalPerKgfPerCm2 / rhocp1);

    return pressureRatio * lowerMixtureEnergy + (1. - pressureRatio) * upperMixtureEnergy;
}

void updateTemperatureFromEnthalpy(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    double pressure = cell.pres;
    double **propertyTable = cell.flui.rholF;

    int divisionCount = cell.flui.npontos - 1;
    if (pressure < propertyTable[1][0] || pressure >= propertyTable[divisionCount + 1][0]) {
        cout << "pressure outside the table bounds";
    }

    const int pressureIndex = bracketingIndex(
        pressure, divisionCount,
        [&](int index) { return propertyTable[index][0]; }, 0);

    double pressureRatio = 1. - (propertyTable[pressureIndex][0] - pressure) / (propertyTable[pressureIndex][0] - propertyTable[pressureIndex + 1][0]);

    double mixtureInternalEnergy = computeMixtureEnthalpy(state, cellIndex);
    int temperatureIndex = 1;
    double lowerEnergy = -1;
    double upperEnergy = 0;
    while (temperatureIndex < divisionCount + 1 || (mixtureInternalEnergy >= lowerEnergy && mixtureInternalEnergy <= upperEnergy) || (mixtureInternalEnergy <= lowerEnergy && mixtureInternalEnergy >= upperEnergy)) {
        lowerEnergy = interpolateMixtureEnergy(state, cellIndex, pressureIndex, temperatureIndex, pressureRatio);
        upperEnergy = interpolateMixtureEnergy(state, cellIndex, pressureIndex, temperatureIndex + 1, pressureRatio);
        temperatureIndex++;
    }
    double temperatureRatio = 1. - (lowerEnergy - mixtureInternalEnergy) / (lowerEnergy - upperEnergy);
    cell.temp = propertyTable[0][temperatureIndex] * temperatureRatio + (1. - temperatureRatio) * propertyTable[0][temperatureIndex + 1];

    if (cell.temp < kMinimumTemperatureCelsius)
        cell.temp = kMinimumTemperatureCelsius;
    if (cell.temp > kMaximumTemperatureCelsius)
        cell.temp = kMaximumTemperatureCelsius;
}

void computeTemperature(const ThermalState &state, int cellIndex, double, int steadyMode) {
    computeCellTemperature(state, cellIndex, steadyMode);
}

void computeThermalMassTransfer(const ThermalState &state, int cellIndex) {
    SimulationContext &globals = *state.globals;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    Cel &rightCell = state.cells[cellIndex + 1];
    const bool hasRightCell = cellIndex < state.lastCell;

    double cellLength = cell.dx;
    double meanCellLength = 0.5 * (cell.dx + leftCell.dx);
    double diameter = cell.duto.a;
    double flowArea = 0.25 * std::numbers::pi * diameter * diameter;
    double meanVoidFraction = cell.alf;
    double betmed = cell.bet;
    double meanSuperficialGasVelocity;

    if (cell.alf > globals.smallValueTolerance)
        meanSuperficialGasVelocity = cell.QG / flowArea;
    else {
        meanSuperficialGasVelocity = 0.;
    }
    double meanSuperficialLiquidVelocity;
    if (cell.alf < 1. - globals.smallValueTolerance)
        meanSuperficialLiquidVelocity = cell.QL / flowArea;
    else {
        meanSuperficialLiquidVelocity = 0.;
    }
    double producedLiquidDensity = cell.flui.MasEspLiq(cell.pres, cell.temp);
    double complementaryDensity = cell.fluicol.MasEspFlu(cell.pres, cell.temp);
    double liquidDensity = (1. - betmed) * producedLiquidDensity + betmed * complementaryDensity;
    double gasDensity = cell.flui.MasEspGas(cell.pres, cell.temp);
    double liquidSpecificHeat = (1. - betmed) * cell.flui.CalorLiq(cell.pres, cell.temp) + betmed * cell.fluicol.CalorLiq(cell.pres, cell.temp);
    double liquidSpecificHeatConstantVolume = liquidSpecificHeat;
    double gasSpecificHeat = cell.flui.CalorGas(cell.pres, cell.temp);
    double gasSpecificHeatConstantVolume = cell.flui.CalorGasVolMod(cell.pres, cell.temp);
    double liquidJouleThomson = (1. - betmed) * cell.flui.JTL(cell.pres, cell.temp) - betmed / complementaryDensity;
    double gasJouleThomson = cell.flui.JTG(cell.pres, cell.temp);
    double hydrostaticPower = (liquidDensity * meanSuperficialLiquidVelocity + gasDensity * meanSuperficialGasVelocity) * flowArea * kGravity * sin(cell.duto.teta);

    cell.calor.Tint = cell.temp;
    cell.calor.Vint = meanSuperficialGasVelocity + meanSuperficialLiquidVelocity;
    cell.calor.dt = cell.dt;
    double liquidConductivity = (1. - betmed) * cell.flui.CondLiq(cell.pres, cell.temp) + betmed * cell.fluicol.CondLiq(cell.pres, cell.temp);
    cell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + cell.flui.CondGas(cell.pres, cell.temp) * meanVoidFraction;
    cell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
    cell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
    double liquidViscosity = (1. - betmed) * cell.flui.ViscOleo(cell.pres, cell.temp) + betmed * cell.fluicol.VisFlu(cell.pres, cell.temp);
    cell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + cell.flui.ViscGas(cell.pres, cell.temp) * meanVoidFraction * units::kPascalSecondPerCentipoise;
    double heatFlux = cell.calor.transtrans();

    double timeCoefficient = (liquidDensity * (1 - meanVoidFraction) * liquidSpecificHeatConstantVolume + gasDensity * meanVoidFraction * gasSpecificHeatConstantVolume) * flowArea;
    double pressureTimeCoefficient;
    pressureTimeCoefficient = -cell.flui.CalorGasPresMod(cell.pres, cell.temp) * (gasDensity * meanVoidFraction * flowArea);
    double temperatureSpatialCoefficient = (liquidDensity * meanSuperficialLiquidVelocity * liquidSpecificHeat + gasDensity * meanSuperficialGasVelocity * gasSpecificHeat) * flowArea;
    double pressureSpatialCoefficient = (liquidDensity * meanSuperficialLiquidVelocity * liquidJouleThomson + gasDensity * meanSuperficialGasVelocity * gasJouleThomson) * flowArea;
    double pressureGradient;
    if (hasRightCell)
        pressureGradient = 2. * (rightCell.presaux - cell.pres) * units::kPascalPerKgfPerCm2 / cell.dx;
    else
        pressureGradient = 2. * (cell.pres - cell.presaux) * units::kPascalPerKgfPerCm2 / cell.dx;
    if (cell.acsr.tipo == kAccessoryChoke && cell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * cell.duto.area)
        pressureGradient = 2. * (cell.pres - cell.presaux) * units::kPascalPerKgfPerCm2 / cell.dx;
    cell.VTemper = temperatureSpatialCoefficient / timeCoefficient;
    double temperatureGradient = (cell.temp - leftCell.temp) / meanCellLength;
    if (hasRightCell)
        if (cell.VTemper < 0)
            temperatureGradient = (rightCell.temp - cell.temp) / (0.5 * (rightCell.dx + cell.dx));
    if (cell.acsr.tipo == kAccessoryChoke && cell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * cell.duto.area && cell.VTemper <= 0)
        temperatureGradient = 0.;
    if (leftCell.acsr.tipo == kAccessoryChoke && leftCell.acsr.chk.AreaGarg <= (1e-3 + state.input.master1.razareaativ) * leftCell.duto.area && cell.VTemper >= 0)
        temperatureGradient = 0.;
    if ((cellIndex == 1 && cell.VTemper <= 0) || (cellIndex == state.lastCell && cell.VTemper <= 0))
        temperatureGradient = 0.;
    if (cell.acsr.tipo == kAccessoryPump && cell.acsr.bcs.freqnova > 1.) {
        pressureGradient = 0.;
    }
    if (cell.acsr.tipo == kAccessoryVolumetricPump && cell.acsr.bvol.freq > 1.) {
        pressureGradient = (cell.pres - leftCell.pres) * units::kPascalPerKgfPerCm2 / meanCellLength;
    }
    if (cell.acsr.tipo == kAccessoryMultiPump && cell.acsr.multibcs.freqnova > 1.) {
        pressureGradient = 0.;
    }

    const double kineticTerm = computeThermalMassTransferKineticTerm(
        state, cellIndex, meanSuperficialGasVelocity, meanSuperficialLiquidVelocity);

    TemperatureSourceTerms sourceTerms =
        computeThermalMassTransferSourceTerms(state, cellIndex, cellLength);

    double latentHeatTerm;
    if (state.latentHeatEnabled > 0 && state.input.flashCompleto == 0) {
        latentHeatTerm = interpolateLatentHeat(state, cell.pres, cell.temp);
    } else if (state.input.flashCompleto == 1)
        latentHeatTerm = (cell.flui.EntalpGas(cell.pres, cell.temp) -
                          cell.flui.EntalpLiq(cell.pres, cell.temp));
    else
        latentHeatTerm = 0;

    double interfaceVoidFraction;
    double leftInterfaceVoidFraction;
    if (meanSuperficialGasVelocity > 0) {
        interfaceVoidFraction = cell.alf;
        leftInterfaceVoidFraction = cell.alfL;
    } else {
        interfaceVoidFraction = cell.alfR;
        leftInterfaceVoidFraction = cell.alf;
    }
    double slipVelocity;
    if (interfaceVoidFraction > globals.smallValueTolerance && interfaceVoidFraction < (1. - globals.smallValueTolerance))
        slipVelocity = meanSuperficialGasVelocity / interfaceVoidFraction - meanSuperficialLiquidVelocity / (1. - interfaceVoidFraction);
    else if (interfaceVoidFraction > globals.smallValueTolerance)
        slipVelocity = meanSuperficialGasVelocity;
    else
        slipVelocity = meanSuperficialLiquidVelocity;
    double interfacialWorkTerm = flowArea * cell.pres * units::kPascalPerKgfPerCm2 * slipVelocity * (interfaceVoidFraction - leftInterfaceVoidFraction) / cell.dx;

    cell.FonteMudaFase = (-(timeCoefficient / cell.dt) * (cell.temp - cell.tempini) - (pressureTimeCoefficient * (cell.pres - cell.presini) * units::kPascalPerKgfPerCm2 / cell.dt) - temperatureSpatialCoefficient * temperatureGradient + pressureSpatialCoefficient * pressureGradient - kineticTerm - (hydrostaticPower - 0. * interfacialWorkTerm) + leftCell.acsr.potB / meanCellLength + sourceTerms.liquid + sourceTerms.gas + heatFlux - (complementaryDensity - producedLiquidDensity) * (1 - meanVoidFraction) * cell.pres * units::kPascalPerKgfPerCm2 * flowArea * (cell.bet - cell.betini) / (liquidDensity * cell.dt));

    cell.FonteMudaFase /= latentHeatTerm;
}

void updateDistributedMassTransfer(const ThermalState &state, const ThermalDependencies &dependencies) {
    if (state.lastCell < 0)
        return;

    sources::prepareRecirculationSourceRates(dependencies.sources);
    MassTransferFaceState previousFace = initializeDistributedMassTransferInlet(state, dependencies, 0);
    for (int cellIndex = 1; cellIndex <= state.lastCell; cellIndex++) {
        updateDistributedMassTransferCell(state, dependencies, cellIndex, previousFace);
    }
}

void advanceAxisymmetricThermalField(const ThermalState &state) {
    if (state.input.pocoTermAxiSim != 1 || state.input.nAxiSim <= 0)
        return;
    auto &solver = state.axisymmetricSolver;
    for (int index = 0; index < solver.dados->ncelPoco; index++) {
        const int cellIndex = state.input.indAxiSim[index];
        TransCal &heatTransfer = state.input.anulAxiSim == 0
                                     ? state.cells[cellIndex].calor
                                     : state.gasCells[cellIndex].calor;
        if (solver.dados->acopDirichlet == 0)
            solver.dados->tInt[index] = state.input.anulAxiSim == 0
                                           ? state.cells[cellIndex].temp
                                           : state.gasCells[cellIndex].temp;
        else {
            const int lastLayer = heatTransfer.geom.ncamadas - 1;
            const int lastSubdivision = heatTransfer.ncamada[lastLayer];
            solver.dados->tInt[index] = heatTransfer.Tcamada[lastLayer][lastSubdivision];
        }
        heatTransfer.calcResGlobAxiSim();
        solver.dados->resGlob[index] = heatTransfer.resGlobAxiSim;
    }
    if (state.globals->simulationTime < 1e-15) {
        std::vector<double> initialTemperatures;
        for (int index = 0; index < solver.dados->ncelPoco; index++) {
            const int cellIndex = state.input.indAxiSim[index];
            if (state.input.anulAxiSim == 0)
                initialTemperatures.push_back(state.cells[cellIndex].calor.Tcamada[0][0]);
            else
                initialTemperatures.push_back(state.gasCells[cellIndex].calor.Tcamada[0][0]);
        }
        solver.inicializaTransientePoisson(initialTemperatures);
        for (int element = 0; element < solver.malha.nele; element++) {
            solver.malha.mlh2d[element].perm = 0;
            solver.malha.mlh2d[element].trans = 1;
        }
        for (int index = 0; index < solver.dados->ncelPoco; index++)
            solver.dados->tInt0[index] = solver.dados->tInt[index];
        solver.kontaTempo = 0;
    }
    if (state.globals->simulationTime < 1e-15) {
        for (int element = 0; element < solver.malha.nele; element++)
            solver.malha.mlh2d[element].calcGradGreen();
    }
    solver.transientePoisson(state.timeStep);
}

void restartAxisymmetricThermalField(const ThermalState &state) {
    if (state.input.pocoTermAxiSim == 1 && state.input.nAxiSim > 0) {
        state.axisymmetricSolver.FeiticoDoTempo();
        state.axisymmetricSolver.transientePoisson(state.timeStep);
    }
}

void finalizeAxisymmetricThermalField(const ThermalState &state, int branchIndex) {
    if (state.input.pocoTermAxiSim == 1 && state.input.nAxiSim > 0)
        state.axisymmetricSolver.finalizaPassoTransiente(state.timeStep, branchIndex);
}

void updateFlowPartitionTerms(const ThermalState &state, const ThermalDependencies &dependencies, int, int explicitMode) {

    Vcr<int> bifurcationFlags(state.lastCell + 1, 0);
    Vcr<int> valveFlags(state.lastCell + 1, 1);
    for (int cellIndex = 0; cellIndex <= state.lastCell; cellIndex++) {
        Cel &cell = state.cells[cellIndex];
        cell.c0ini = cell.c0;
        cell.udini = cell.ud;
        if (cellIndex != 0 && cellIndex != state.lastCell) {
            updateInteriorFlowPartitionCell(state, dependencies, cellIndex, bifurcationFlags, valveFlags, explicitMode);
        } else if (cellIndex == 0) {
            updateInletBoundaryFlowPartition(state, dependencies, cellIndex, bifurcationFlags, explicitMode);

        } else {
            updateOutletBoundaryFlowPartition(state, dependencies, cellIndex, bifurcationFlags, explicitMode);
        }
    }

    finalizeFlowPartitionTerms(state, bifurcationFlags, valveFlags);
}

void updateOutletFlowPartitionTerms(const ThermalState &state, const ThermalDependencies &dependencies) {

    int cellIndex = state.lastCell;
    Cel &cell = state.cells[cellIndex];
    Cel &leftCell = state.cells[cellIndex - 1];
    const bool bufferedGasMassIsPositive = (cell.MCBuf - cell.MliqiniBuf) >= 0;

    double meanPressure = cell.presBuf;
    double meanTemperature = leftCell.temp;
    if (cell.VTemper < 0.)
        meanTemperature = state.gasSurfaceTemperature;
    double betI;
    double liquidDensity;
    double surfaceTension;
    if (cell.MliqiniBuf < 0.) {
        betI = cell.betPigE;
        liquidDensity = (1 - betI) * cell.flui.MasEspLiq(meanPressure, meanTemperature) + betI * cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        surfaceTension = (1 - betI) * cell.flui.TensSuper(meanPressure, meanTemperature) + betI * cell.fluicol.TensSuper(meanPressure, meanTemperature);
    } else {
        betI = leftCell.betPigD;
        liquidDensity = (1 - betI) * leftCell.flui.MasEspLiq(meanPressure, meanTemperature) + betI * leftCell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        surfaceTension = (1 - betI) * leftCell.flui.TensSuper(meanPressure, meanTemperature) + betI * leftCell.fluicol.TensSuper(meanPressure, meanTemperature);
    }
    double gasDensity;
    double flowArea;
    if (bufferedGasMassIsPositive) {
        flowArea = cell.dutoL.area;
        gasDensity = leftCell.flui.MasEspGas(meanPressure, meanTemperature);
    } else {
        gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
        flowArea = cell.duto.area;
    }
    double superficialGasVelocity = (cell.MCBuf - cell.MliqiniBuf) / (gasDensity * flowArea);
    double superficialLiquidVelocity = (cell.MliqiniBuf) / (liquidDensity * flowArea);

    double inclinationAngle = cell.duto.teta;
    if (cellIndex >= 2) {
        if (state.cells[cellIndex - 2].acsr.tipo == kAccessoryChoke && state.cells[cellIndex - 2].acsr.chk.AreaGarg <= (1e-3)) {
            if (bufferedGasMassIsPositive)
                inclinationAngle = cell.duto.teta;
            else
                inclinationAngle = cell.dutoR.teta;
        } else {
            if (bufferedGasMassIsPositive)
                inclinationAngle = cell.dutoL.teta;
            else
                inclinationAngle = cell.duto.teta;
        }
    }
    double inclinationSign = 1.;
    if (inclinationAngle < 0.)
        inclinationSign = -1.;
    double c0 = 2.;
    double ud = inclinationSign * 0.0246 * cos(inclinationAngle) + 1.606 * pow(kGravity * surfaceTension * (liquidDensity - gasDensity) / (liquidDensity * liquidDensity), 0.25) * sin(inclinationAngle);

    double leftFlowArea = cell.dutoL.area;
    double leftLengthRatio = leftCell.dxL / (leftCell.dx + leftCell.dxL);
    double leftMeanPressure = leftCell.presaux;
    double leftMeanTemperature = leftCell.temp * leftLengthRatio + leftCell.tempL * (1. - leftLengthRatio);
    double betIL;
    if (cellIndex < 2)
        betIL = leftCell.betL;
    else
        betIL = state.cells[cellIndex - 2].betPigD;
    if (leftCell.MliqiniBuf < 0.)
        betIL = leftCell.betPigE;
    double leftGasDensity = cell.flui.MasEspGas(leftMeanPressure, leftMeanTemperature);
    double leftLiquidDensity = (1 - betIL) * cell.flui.MasEspLiq(leftMeanPressure, leftMeanTemperature) + betIL * cell.fluicol.MasEspFlu(leftMeanPressure, leftMeanTemperature);
    double leftSuperficialGasVelocity = (cell.MLBuf - cell.MliqiniLBuf) / (leftGasDensity * leftFlowArea);
    double leftSuperficialLiquidVelocity = (cell.MliqiniLBuf) / (leftLiquidDensity * leftFlowArea);

    double rightFlowArea = cell.dutoR.area;
    double rightLengthRatio = cell.dxR / (cell.dxR + cell.dx);
    double rightMeanPressure = cell.presRBuf;
    double rightMeanTemperature = cell.temp * rightLengthRatio + cell.tempR * (1. - rightLengthRatio);
    double betIR = cell.betPigD;
    if (cell.MliqiniRBuf < 0.) {
        if (cellIndex > state.lastCell - 2)
            betIR = cell.betR;
        else
            betIR = state.cells[cellIndex + 1].betPigE;
    }
    double rightGasDensity = cell.flui.MasEspGas(rightMeanPressure, rightMeanTemperature);
    double rightLiquidDensity = (1 - betIR) * cell.flui.MasEspLiq(rightMeanPressure, rightMeanTemperature) + betIR * cell.fluicol.MasEspFlu(rightMeanPressure, rightMeanTemperature);
    double rightSuperficialGasVelocity = (cell.MRBuf - cell.MliqiniRBuf) / (rightGasDensity * rightFlowArea);
    double rightSuperficialLiquidVelocity = (cell.MliqiniRBuf) / (rightLiquidDensity * rightFlowArea);

    selectAndApplyBufferedOutletFlowRegime(
        state, dependencies, cellIndex, c0, ud, superficialGasVelocity, superficialLiquidVelocity, leftSuperficialGasVelocity, leftSuperficialLiquidVelocity, rightSuperficialGasVelocity, rightSuperficialLiquidVelocity,
        gasDensity, liquidDensity, flowArea);
    cell.term1L = leftCell.term1;
    cell.term2L = leftCell.term2;
    leftCell.term1R = cell.term1;
    leftCell.term2R = cell.term2;
}

void updateInletFlowPartitionTerms(const ThermalState &state, const ThermalDependencies &dependencies) {

    int cellIndex = 0;
    Cel &cell = state.cells[cellIndex];

    double meanPressure;
    meanPressure = state.inletPressure;

    double meanTemperature;
    if (state.cells[0].MliqiniBuf < 0.)
        meanTemperature = cell.temp;
    else
        meanTemperature = state.inletTemperature;

    double betI;
    double surfaceTension;
    double gasDensity = state.cells[0].flui.MasEspGas(state.inletPressure, state.inletTemperature);
    double liquidDensity = state.cells[0].flui.MasEspLiq(state.inletPressure, state.inletTemperature);
    double rcis = state.cells[0].fluicol.MasEspFlu(state.inletPressure, state.inletTemperature);

    double liquidMixtureDensity = state.inletComplementaryFraction * rcis + (1 - state.inletComplementaryFraction) * liquidDensity;
    state.inletVoidFraction = (-state.inletQuality * liquidMixtureDensity / (state.inletQuality * gasDensity - gasDensity - state.inletQuality * liquidMixtureDensity)) / (state.cells[0].c0);

    if ((cell.MCBuf - state.cells[0].MliqiniBuf) * 0 + 1 * state.cells[0].MliqiniBuf < 0.) {
        betI = cell.betPigE;
        liquidDensity = (1 - betI) * cell.flui.MasEspLiq(meanPressure, meanTemperature) + betI * cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        surfaceTension = (1 - betI) * cell.flui.TensSuper(meanPressure, meanTemperature) + betI * cell.fluicol.TensSuper(meanPressure, meanTemperature);
    } else {
        betI = state.inletComplementaryFraction;
        liquidDensity = (1 - betI) * (*cell.fluiL).MasEspLiq(meanPressure, meanTemperature) + betI * cell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        surfaceTension = (1 - betI) * (*cell.fluiL).TensSuper(meanPressure, meanTemperature) + betI * cell.fluicol.TensSuper(meanPressure, meanTemperature);
    }
    double flowArea;
    if (cell.MCBuf - state.cells[0].MliqiniBuf >= 0) {
        flowArea = cell.dutoL.area;
        gasDensity = (*cell.fluiL).MasEspGas(meanPressure, meanTemperature);
    } else {
        gasDensity = cell.flui.MasEspGas(meanPressure, meanTemperature);
        flowArea = cell.duto.area;
    }
    double superficialGasVelocity = (cell.MCBuf - cell.MliqiniBuf) / (gasDensity * flowArea);
    double superficialLiquidVelocity = cell.MliqiniBuf / (liquidDensity * flowArea);

    double inclinationAngle = cell.duto.teta;
    double inclinationSign = 1.;
    if (inclinationAngle < 0.)
        inclinationSign = -1.;
    double c0 = 2.;
    double ud = inclinationSign * 0.0246 * cos(inclinationAngle) + 1.606 * pow(kGravity * surfaceTension * (liquidDensity - gasDensity) / (liquidDensity * liquidDensity), 0.25) * sin(inclinationAngle);

    double rightFlowArea = cell.dutoR.area;
    double rightLengthRatio = cell.dxR / (cell.dxR + cell.dx);
    double rightMeanPressure = cell.presRBuf * rightLengthRatio + cell.presBuf * (1. - rightLengthRatio);
    double rightMeanTemperature = cell.temp * rightLengthRatio + cell.tempR * (1. - rightLengthRatio);
    double betIR = cell.betPigD;
    if (cell.QLR < 0.)
        betIR = state.cells[cellIndex + 1].betPigE;

    (void)cell.flui.MasEspGas(rightMeanPressure, rightMeanTemperature);
    double rightLiquidDensity = (1 - betIR) * cell.flui.MasEspLiq(rightMeanPressure, rightMeanTemperature) + betIR * cell.fluicol.MasEspFlu(rightMeanPressure, rightMeanTemperature);
    double rightSuperficialLiquidVelocity = (cell.MliqiniRBuf) / (rightLiquidDensity * rightFlowArea);

    selectAndApplyBufferedInletFlowRegime(
        state, dependencies, cellIndex, c0, ud, superficialGasVelocity, superficialLiquidVelocity, rightSuperficialLiquidVelocity, gasDensity, liquidDensity,
        flowArea);
    state.cells[1].term1L = cell.term1;
    state.cells[1].term2L = cell.term2;
}

void prepareNonDimensionalHeatDiffusion(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    const CellFlowBasis basis = cellFlowBasisOf(state, cellIndex);
    const double meanVoidFraction = basis.voidFraction;
    const double betmed = basis.betmed;
    const double meanSuperficialGasVelocity = basis.gasSuperficialVelocity;
    const double meanSuperficialLiquidVelocity = basis.liquidSuperficialVelocity;
    double rp = cell.rpC;
    double rc = cell.rcC;
    double liquidDensity = (1. - betmed) * rp + betmed * rc;
    double gasDensity = cell.rgC;
    double liquidSpecificHeat = (1. - betmed) * cell.flui.CalorLiq(cell.presini, cell.temp) + betmed * cell.fluicol.CalorLiq(cell.presini, cell.temp);
    double gasSpecificHeat = cell.flui.CalorGas(cell.presini, cell.temp);

    cell.calor.Tint = cell.temp;
    cell.calor.dtL = cellIndex > 0 ? cell.temp - state.cells[cellIndex - 1].tempini : 0.;
    cell.calor.Vint = meanSuperficialGasVelocity + meanSuperficialLiquidVelocity;
    cell.calor.dt = cell.dt;
    double liquidConductivity = (1. - betmed) * cell.flui.CondLiq(cell.presini, cell.temp) + betmed * cell.fluicol.CondLiq(cell.presini, cell.temp);
    cell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + cell.flui.CondGas(cell.presini, cell.temp) * meanVoidFraction;
    cell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
    cell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
    double liquidViscosity = (1. - betmed) * cell.mipC + betmed * cell.micC;
    cell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + cell.migC * meanVoidFraction * units::kPascalSecondPerCentipoise;
    double temperaturePerturbation = cell.temp * 0.01;
    if (fabs(cell.temp) < 1e-15)
        temperaturePerturbation = 0.1;
    double liquidDensityChange = (1. - betmed) * cell.flui.MasEspLiq(cell.presini, cell.temp + temperaturePerturbation) +
                                 betmed * cell.fluicol.MasEspFlu(cell.presini, cell.temp + temperaturePerturbation) - liquidDensity;
    double gasDensityChange = cell.flui.MasEspGas(cell.presini, cell.temp + temperaturePerturbation) - gasDensity;
    cell.calor.betint = -(1 / cell.calor.rhoint) * (liquidDensityChange * (1 - meanVoidFraction) + gasDensityChange * meanVoidFraction) / (temperaturePerturbation);
}

void advanceTransientEnergy(const ThermalState &state, const transient::TransientStepState &transientStep, int cycle, int maximumCycle) {
    prepareTransientOutletFlow(state);

    state.cells[0].tempini = state.cells[0].temp;
    state.cells[0].temp = state.input.ConContEntrada > 0
                              ? state.inletTemperature : state.defaultInletTemperature;
    state.cells[0].dTdt = (state.cells[0].temp - state.cells[0].tempini) / state.timeStep;
    state.cells[0].dTdtL = state.cells[0].dTdt;
    state.cells[1].tempLini = state.cells[1].tempL;
    state.cells[1].tempL = state.cells[0].temp;
    for (int cellIndex = 1; cellIndex <= state.lastCell; cellIndex++) {
        state.cells[cellIndex].tempini = state.cells[cellIndex].temp;
    }
    if (state.input.modoDifus3D == 0) {
        refreshTemperatureField(state);
    } else if (state.input.modoDifus3D == 1) {
        advanceCoupledThermalField(state, cycle, maximumCycle);
    }
    for (int cellIndex = 1; cellIndex <= state.lastCell; cellIndex++) {
        Cel &cell = state.cells[cellIndex];
        Cel &leftCell = state.cells[cellIndex - 1];
        cell.dTdtL = leftCell.dTdt;
        if (cellIndex < state.lastCell) {
            Cel &rightCell = state.cells[cellIndex + 1];
            rightCell.tempLini = rightCell.tempL;
            rightCell.tempL = cell.temp;
        }
        leftCell.tempRini = leftCell.tempR;
        leftCell.tempR = cell.temp;
    }
    if (cycle < maximumCycle) {
        for (int k = 0; k <= state.lastCell; k++) {
            state.cells[k].FeiticoDoTempo();
        }
    } else if (state.input.modoDifus3D == 1)
        state.poissonSolver3D.renova();
    if (state.fullModel == 0) {
        if (cycle < maximumCycle) {
            transient::solvePressureVolumeCoupling(transientStep, 0, cycle);
            transient::updateCells(transientStep, 0);
        }
    }
}

void advanceSteadyTemperature(const ThermalState &state, int cellIndex, int rungeKuttaStage) {
    Cel &cell = state.cells[cellIndex];
    SimulationContext &globals = *state.globals;
    Cel &leftCell = state.cells[cellIndex - 1];
    double cellLength = 0.5 * (cell.dx + leftCell.dx);
    double meanCellLength = 0.5 * (cell.dx + leftCell.dx);
    double meanTemperatureGradientCorrection = (cell.dx * cell.dTdLCor + leftCell.dx * leftCell.dTdLCor) / (cell.dx + leftCell.dx);
    double diameter = leftCell.duto.a;
    double flowArea = 0.25 * std::numbers::pi * diameter * diameter;
    double meanVoidFraction;
    double betmed;
    double meanPressure;
    double meanTemperature;
    if (rungeKuttaStage == 0) {
        meanVoidFraction = leftCell.alf;
        betmed = leftCell.bet;
        meanPressure = leftCell.pres;
        meanTemperature = leftCell.temp;
    } else {
        meanVoidFraction = cell.alf;
        betmed = cell.bet;
        meanPressure = cell.pres;
        meanTemperature = cell.temp;
    }
    double meanSuperficialGasVelocity;
    meanSuperficialGasVelocity = cell.QG / flowArea; // gas superficial velocity
    double meanSuperficialLiquidVelocity;
    meanSuperficialLiquidVelocity = cell.QL / flowArea; // liquid superficial velocity
    double mixtureFluxSign = 1.;
    if (fabs(meanSuperficialGasVelocity + meanSuperficialLiquidVelocity) > 0.05 && state.thermalSourceDisabled == 0) { // Solve heat transport above 0.05 m/s when thermal sources are enabled;
        // at lower velocities the fluid temperature is taken as the ambient temperature
        mixtureFluxSign = (meanSuperficialGasVelocity + meanSuperficialLiquidVelocity) / fabs(meanSuperficialGasVelocity + meanSuperficialLiquidVelocity);
        double interfaceMeanPressure = cell.presaux + 0 * leftCell.acsr.dpB / units::kPascalPerKgfPerCm2;
        double interfaceMeanTemperature;
        if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0) // temperature at the cell's left interface
            // case in which one iteration is considered to have been
            // done already, with the temperature of cell cellIndex known from the previous iteration;
            // this can hinder convergence when convergence acceleration is wanted
            // the temperature at the left boundary is taken as that of the left cell
            interfaceMeanTemperature = (cell.dx * cell.temp + cell.dxL * cell.tempL) / (cell.dx + cell.dxL);
        else
            interfaceMeanTemperature = leftCell.temp;
        double rp = leftCell.flui.MasEspLiq(meanPressure, meanTemperature);
        double rc = leftCell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        double liquidDensity = (1. - betmed) * rp + betmed * rc;
        double gasDensity = leftCell.flui.MasEspGas(meanPressure, meanTemperature);
        double liquidSpecificHeat = (1. - betmed) * leftCell.flui.CalorLiq(interfaceMeanPressure, interfaceMeanTemperature) + betmed * leftCell.fluicol.CalorLiq(interfaceMeanPressure, interfaceMeanTemperature);
        double gasSpecificHeat = leftCell.flui.CalorGas(interfaceMeanPressure, interfaceMeanTemperature);
        // liquidJouleThomson = liquid Joule-Thomson coefficient x cp
        // gasJouleThomson = gas Joule-Thomson coefficient x cp
        double liquidJouleThomson = (1. - betmed) * leftCell.flui.JTL(interfaceMeanPressure, interfaceMeanTemperature) - betmed / rc;
        if (state.input.pocinjec > 0 && state.input.condpocinj.tipoFlui == 2) {
            liquidJouleThomson = -(1 + (interfaceMeanTemperature + units::kZeroCelsiusInKelvin) * leftCell.fluicol.DrhoDtFlu(interfaceMeanPressure, interfaceMeanTemperature) / rc) / rc;
        }
        double gasJouleThomson = leftCell.flui.JTG(interfaceMeanPressure, interfaceMeanTemperature);
        // potential energy:
        double hydrostaticPower = (liquidDensity * meanSuperficialLiquidVelocity + gasDensity * meanSuperficialGasVelocity) * flowArea * kGravity * sin(leftCell.duto.teta);

        // internal tubing parameters for the heat exchange with the surroundings
        leftCell.calor.Tint = interfaceMeanTemperature;
        leftCell.calor.Vint = fabs(meanSuperficialGasVelocity + meanSuperficialLiquidVelocity);
        double liquidConductivity = (1. - betmed) * leftCell.flui.CondLiq(interfaceMeanPressure, interfaceMeanTemperature) + betmed * leftCell.fluicol.CondLiq(interfaceMeanPressure, interfaceMeanTemperature);
        leftCell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + leftCell.flui.CondGas(interfaceMeanPressure, interfaceMeanTemperature) * meanVoidFraction;
        leftCell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
        leftCell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
        double liquidViscosity = (1. - betmed) * leftCell.flui.ViscOleo(interfaceMeanPressure, interfaceMeanTemperature) + betmed * leftCell.fluicol.VisFlu(interfaceMeanPressure, interfaceMeanTemperature);
        leftCell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + leftCell.flui.ViscGas(interfaceMeanPressure, interfaceMeanTemperature) * meanVoidFraction * units::kPascalSecondPerCentipoise;

        double heatFlux;
        double annulusResistance = applySteadyAnnulusCoupling(
            state, cellIndex, interfaceMeanPressure, interfaceMeanTemperature,
            gasSpecificHeat, gasDensity);
        leftCell.fluxcalmed = 0;
        if (state.productionNetworkHeatCoupled == 1 && (cellIndex - 1) >= state.primaryNetworkSectionEnd && (cellIndex - 1) <= state.primaryNetworkSectionStart) {
            heatFlux = mixtureFluxSign * leftCell.calor.transperm(leftCell.resAcopRedeP);
        } else
            heatFlux = mixtureFluxSign * leftCell.calor.transperm(annulusResistance);
        leftCell.fluxcalmed = heatFlux; // heat flux in the tubing

        double temperatureSpatialCoefficient = (liquidDensity * meanSuperficialLiquidVelocity * liquidSpecificHeat + gasDensity * meanSuperficialGasVelocity * gasSpecificHeat) * flowArea; // multiplying term
        // the derivative Dt/Dx
        double cappedMeanSuperficialLiquidVelocity = meanSuperficialLiquidVelocity;
        if (globals.temporaryBlackOilMode == 1 && fabs(meanSuperficialLiquidVelocity) > 5)
            cappedMeanSuperficialLiquidVelocity = 5 * meanSuperficialLiquidVelocity / fabs(meanSuperficialLiquidVelocity);
        double cappedMeanSuperficialGasVelocity = meanSuperficialGasVelocity;
        if (globals.temporaryBlackOilMode == 1 && fabs(meanSuperficialGasVelocity) > 5)
            cappedMeanSuperficialGasVelocity = 5 * meanSuperficialGasVelocity / fabs(meanSuperficialGasVelocity);
        double pressureSpatialCoefficient = 1 * (liquidDensity * cappedMeanSuperficialLiquidVelocity * liquidJouleThomson + gasDensity * cappedMeanSuperficialGasVelocity * gasJouleThomson) * flowArea; // multiplying term
        // the derivative Dp/Dx
        const double pressureGradient = 2. * (cell.presaux - leftCell.pres) * units::kPascalPerKgfPerCm2 / leftCell.dx;
        cell.VTemper = meanSuperficialLiquidVelocity; // transport velocity for subsequent thermal steps

        double kineticTerm = computeSteadyKineticTerm(
            state, cellIndex, meanSuperficialGasVelocity,
            meanSuperficialLiquidVelocity);

        TemperatureSourceTerms steadySources = computeSteadySourceTerms(
            state, cellIndex, meanPressure, meanTemperature, cellLength);
        double gasMassSourceTerm = steadySources.gas;
        double liquidMassSourceTerm = steadySources.liquid;

        // latent heat effect, when requested
        double latentHeatTerm = computeSteadyLatentHeatTerm(state, cellIndex, meanPressure, meanTemperature);

        if (fabs(temperatureSpatialCoefficient) > globals.smallValueTolerance) {
            // Energy terms for boundary work, potential and kinetic energy,
            // mass sources, latent heat, and shaft work.
            if (state.input.latente == 0)
                latentHeatTerm = 0.;
            else if (state.input.condlatente == 0 && latentHeatTerm < 0)
                latentHeatTerm = 0.;
            double sourceTemperatureGradient = meanTemperatureGradientCorrection * (pressureSpatialCoefficient * pressureGradient - kineticTerm - (hydrostaticPower) + (liquidMassSourceTerm + gasMassSourceTerm) - latentHeatTerm + (leftCell.acsr.potTermo + leftCell.fonteCal) / meanCellLength) / temperatureSpatialCoefficient;
            // portion of energy related to heat exchange
            double heatFluxTemperatureGradient = meanTemperatureGradientCorrection * (heatFlux) / temperatureSpatialCoefficient;

            // Check whether heat transfer is too fast for the explicit temperature update.
            // If so, use additional substeps to prevent thermal instability.
            const SteadyTemperatureStep step{
                .meanPressure = meanPressure,
                .meanVoidFraction = meanVoidFraction,
                .betmed = betmed,
                .liquidDensity = liquidDensity,
                .gasDensity = gasDensity,
                .liquidSpecificHeat = liquidSpecificHeat,
                .gasSpecificHeat = gasSpecificHeat,
                .meanSuperficialGasVelocity = meanSuperficialGasVelocity,
                .meanSuperficialLiquidVelocity = meanSuperficialLiquidVelocity,
                .meanCellLength = meanCellLength,
                .meanTemperatureGradientCorrection = meanTemperatureGradientCorrection,
                .temperatureSpatialCoefficient = temperatureSpatialCoefficient,
                .annulusResistance = annulusResistance,
                .mixtureFluxSign = mixtureFluxSign,
                .sourceTemperatureGradient = sourceTemperatureGradient};
            advanceSteadyTemperatureSubsteps(state, cellIndex, step, heatFluxTemperatureGradient);
        } else
            cell.temp = cell.calor.Textern1;
    } else { // case where the mixing speed is too low
        cell.temp = leftCell.calor.Textern1;
        if (state.input.lingas == 1 && (cellIndex - 1 <= state.annulusTubingStart && cellIndex - 1 >= state.annulusTubingEnd)) {
            int j = state.annulusTubingStart + state.tubingAnnulusStart - (cellIndex - 1);
            leftCell.calor.Vextern1 = 100.;
            leftCell.calor.kextern1 = state.gasCells[j].calor.kint;
            leftCell.calor.cpextern1 = state.gasCells[j].calor.cpint;
            leftCell.calor.rhoextern1 = state.gasCells[j].calor.rhoint;
            leftCell.calor.viscextern1 = state.gasCells[j].calor.viscint;
            (void)leftCell.calor.transperm(0);
        }
    }
}

void advanceReverseSteadyTemperature(const ThermalState &state, int cellIndex, int rungeKuttaStage) {
    Cel &cell = state.cells[cellIndex];
    SimulationContext &globals = *state.globals;
    Cel &rightCell = state.cells[cellIndex + 1];
    double cellLength = 0.5 * (cell.dx + rightCell.dx);
    double meanCellLength = 0.5 * (cell.dx + rightCell.dx);
    double meanTemperatureGradientCorrection = (cell.dx * cell.dTdLCor + rightCell.dx * rightCell.dTdLCor) / (cell.dx + rightCell.dx);
    double diameter = rightCell.duto.a;
    double flowArea = 0.25 * std::numbers::pi * diameter * diameter;
    double meanVoidFraction;
    double betmed;
    double meanPressure;
    double meanTemperature;
    if (rungeKuttaStage == 0) {
        meanVoidFraction = rightCell.alf;
        betmed = rightCell.bet;
        meanPressure = rightCell.pres;
        meanTemperature = rightCell.temp;
    } else {
        meanVoidFraction = cell.alf;
        betmed = cell.bet;
        meanPressure = cell.pres;
        meanTemperature = cell.temp;
    }
    double meanSuperficialGasVelocity;
    meanSuperficialGasVelocity = fabs(rightCell.QG) / flowArea; // Superficial gas velocity
    double meanSuperficialLiquidVelocity;
    meanSuperficialLiquidVelocity = fabs(rightCell.QL) / flowArea; // Superficial liquid velocity
    double mixtureFluxSign = 1.;
    if (fabs(meanSuperficialGasVelocity + meanSuperficialLiquidVelocity) > state.slowHeatTransferThreshold) {
        // Solve heat transport above the configured slow-transfer threshold.
        // Otherwise, assume the fluid temperature equals the ambient temperature.
        mixtureFluxSign = (meanSuperficialGasVelocity + meanSuperficialLiquidVelocity) / fabs(meanSuperficialGasVelocity + meanSuperficialLiquidVelocity);
        double interfaceMeanPressure = rightCell.presaux - cell.acsr.dpB / units::kPascalPerKgfPerCm2;
        double interfaceMeanTemperature;
        if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
            // Use the previous-iteration temperature at the right interface.
            interfaceMeanTemperature = (cell.dx * cell.temp + cell.dxR * cell.tempR) / (cell.dx + cell.dxR);
        else
            interfaceMeanTemperature = rightCell.temp;
        double rp = rightCell.flui.MasEspLiq(meanPressure, meanTemperature);
        double rc = rightCell.fluicol.MasEspFlu(meanPressure, meanTemperature);
        double liquidDensity = (1. - betmed) * rp + betmed * rc;
        double gasDensity = rightCell.flui.MasEspGas(meanPressure, meanTemperature);
        double liquidSpecificHeat = (1. - betmed) * rightCell.flui.CalorLiq(interfaceMeanPressure, interfaceMeanTemperature) + betmed * rightCell.fluicol.CalorLiq(interfaceMeanPressure, interfaceMeanTemperature);
        double gasSpecificHeat = rightCell.flui.CalorGas(interfaceMeanPressure, interfaceMeanTemperature);
        // Liquid Joule-Thomson coefficient multiplied by cp.
        // Gas Joule-Thomson coefficient multiplied by cp.
        double liquidJouleThomson = (1. - betmed) * rightCell.flui.JTL(interfaceMeanPressure, interfaceMeanTemperature) - betmed / rc;
        if (state.input.pocinjec > 0 && state.input.condpocinj.tipoFlui == 2) {
            liquidJouleThomson = -(1 + (interfaceMeanTemperature + units::kZeroCelsiusInKelvin) * rightCell.fluicol.DrhoDtFlu(interfaceMeanPressure, interfaceMeanTemperature) / rc) / rc;
        }
        double gasJouleThomson = rightCell.flui.JTG(interfaceMeanPressure, interfaceMeanTemperature);
        // potential energy:
        double hydrostaticPower = -(liquidDensity * meanSuperficialLiquidVelocity + gasDensity * meanSuperficialGasVelocity) * flowArea * kGravity * sin(rightCell.duto.teta);

        // Definition of internal parameters in the piping to achieve heat exchange with the environment
        rightCell.calor.Tint = interfaceMeanTemperature;
        rightCell.calor.Vint = fabs(meanSuperficialGasVelocity + meanSuperficialLiquidVelocity);
        double liquidConductivity = (1. - betmed) * rightCell.flui.CondLiq(interfaceMeanPressure, interfaceMeanTemperature) + betmed * rightCell.fluicol.CondLiq(interfaceMeanPressure, interfaceMeanTemperature);
        rightCell.calor.kint = liquidConductivity * (1 - meanVoidFraction) + rightCell.flui.CondGas(interfaceMeanPressure, interfaceMeanTemperature) * meanVoidFraction;
        rightCell.calor.cpint = liquidSpecificHeat * (1 - meanVoidFraction) + gasSpecificHeat * meanVoidFraction;
        rightCell.calor.rhoint = liquidDensity * (1 - meanVoidFraction) + gasDensity * meanVoidFraction;
        double liquidViscosity = (1. - betmed) * rightCell.flui.ViscOleo(interfaceMeanPressure, interfaceMeanTemperature) + betmed * rightCell.fluicol.VisFlu(interfaceMeanPressure, interfaceMeanTemperature);
        rightCell.calor.viscint = liquidViscosity * (1 - meanVoidFraction) * units::kPascalSecondPerCentipoise + rightCell.flui.ViscGas(interfaceMeanPressure, interfaceMeanTemperature) * meanVoidFraction * units::kPascalSecondPerCentipoise;

        double heatFlux;
        double annulusResistance = applyReverseSteadyAnnulusCoupling(
            state, cellIndex, interfaceMeanPressure, interfaceMeanTemperature,
            gasSpecificHeat, gasDensity);
        rightCell.fluxcalmed = 0;
        heatFlux = mixtureFluxSign * rightCell.calor.transperm(annulusResistance);
        rightCell.fluxcalmed = heatFlux; // heat flux in the tubing

        double temperatureSpatialCoefficient = (liquidDensity * meanSuperficialLiquidVelocity * liquidSpecificHeat + gasDensity * meanSuperficialGasVelocity * gasSpecificHeat) * flowArea; // multiplying term
        // the derivative Dt/Dx
        double pressureSpatialCoefficient = 1. * (liquidDensity * meanSuperficialLiquidVelocity * liquidJouleThomson + gasDensity * meanSuperficialGasVelocity * gasJouleThomson) * flowArea; // multiplying term
        // the derivative Dp/Dx
        double pressureGradient;
        if ((rightCell.acsr.tipo != kAccessoryPump || rightCell.acsr.bcs.freq < 1) && rightCell.acsr.tipo != 7)
            // without an ESP or a pressure increment, the pressure at the left boundary
            // and the pressure at the cell centre are used to compute Dp/Dx
            pressureGradient = 2. * (rightCell.presaux - rightCell.pres) * units::kPascalPerKgfPerCm2 / rightCell.dx;
        else {
            // with an ESP or a pressure increment, the pressure of the cell to the left
            // and the pressure at the cell centre are used to compute Dp/Dx
            pressureGradient = (interfaceMeanPressure - rightCell.pres) * units::kPascalPerKgfPerCm2 / cellLength;
        }
        cell.VTemper = meanSuperficialLiquidVelocity; // transport velocity for subsequent thermal steps

        double kineticTerm = computeReverseSteadyKineticTerm(
            state, cellIndex, meanSuperficialGasVelocity,
            meanSuperficialLiquidVelocity);

        TemperatureSourceTerms reverseSources = computeReverseSteadySourceTerms(
            state, cellIndex, meanPressure, meanTemperature, cellLength);
        double gasMassSourceTerm = reverseSources.gas;
        double liquidMassSourceTerm = reverseSources.liquid;

        // latent heat effect, when requested
        double latentHeatTerm = computeReverseSteadyLatentHeatTerm(state, cellIndex, meanPressure, meanTemperature);

        if (fabs(temperatureSpatialCoefficient) > globals.smallValueTolerance) {
            // energy term from boundary work, potential energy,
            // kinetic energy, mass sources, latent heat and shaft work:
            double sourceTemperatureGradient = meanTemperatureGradientCorrection * (pressureSpatialCoefficient * pressureGradient - kineticTerm - (hydrostaticPower) + (liquidMassSourceTerm + gasMassSourceTerm) - latentHeatTerm - rightCell.acsr.potBT / meanCellLength) / temperatureSpatialCoefficient;
            // energy term from the heat exchange
            double heatFluxTemperatureGradient = meanTemperatureGradientCorrection * (heatFlux) / temperatureSpatialCoefficient;

            // checks whether the heat exchange is happening too fast:
            // as the temperature advance is explicit, this can make the thermal calculation unstable;
            // if the heat exchange is found to be fast, the advance is
            // made in a larger number of steps from one cell to the next
            const SteadyTemperatureStep step{
                .meanPressure = meanPressure,
                .meanVoidFraction = meanVoidFraction,
                .betmed = betmed,
                .liquidDensity = liquidDensity,
                .gasDensity = gasDensity,
                .liquidSpecificHeat = liquidSpecificHeat,
                .gasSpecificHeat = gasSpecificHeat,
                .meanSuperficialGasVelocity = meanSuperficialGasVelocity,
                .meanSuperficialLiquidVelocity = meanSuperficialLiquidVelocity,
                .meanCellLength = meanCellLength,
                .meanTemperatureGradientCorrection = meanTemperatureGradientCorrection,
                .temperatureSpatialCoefficient = temperatureSpatialCoefficient,
                .annulusResistance = annulusResistance,
                .mixtureFluxSign = mixtureFluxSign,
                .sourceTemperatureGradient = sourceTemperatureGradient};
            advanceReverseSteadyTemperatureSubsteps(state, cellIndex, step, heatFluxTemperatureGradient);
        } else
            cell.temp = cell.calor.Textern1;
    } else { // case where the mixture velocity is very low
        cell.temp = rightCell.calor.Textern1;
        if (state.input.lingas == 1 && (cellIndex + 1 <= state.annulusTubingStart && cellIndex + 1 >= state.annulusTubingEnd)) {
            int j = state.annulusTubingStart + state.tubingAnnulusStart - (cellIndex + 1);
            rightCell.calor.Vextern1 = 100.;
            rightCell.calor.kextern1 = state.gasCells[j].calor.kint;
            rightCell.calor.cpextern1 = state.gasCells[j].calor.cpint;
            rightCell.calor.rhoextern1 = state.gasCells[j].calor.rhoint;
            rightCell.calor.viscextern1 = state.gasCells[j].calor.viscint;
            (void)rightCell.calor.transperm(0);
        }
    }
}

void computeGasTemperature(const ThermalState &state, int cellIndex, double, int steadyMode) {
    const bool hasRightGasCell = cellIndex < state.lastGasCell;

    if (state.thermalSourceDisabled == 0) {
        double cellLength = state.gasCells[cellIndex].dx0;
        double meanCellLength = 0.5 * (state.gasCells[cellIndex].dx0 + state.gasCells[cellIndex - 1].dx0);
        double flowArea = state.gasCells[cellIndex].duto.area;
        double meanSuperficialGasVelocity;
        if (hasRightGasCell)
            meanSuperficialGasVelocity = state.gasCells[cellIndex].VGasR / state.gasCells[cellIndex].u1L;
        else {
            meanSuperficialGasVelocity = state.gasCells[cellIndex].VGasL / state.gasCells[cellIndex].u1L;
        }
        double gasDensity = state.gasCells[cellIndex].rg;
        double gasSpecificHeat = state.gasCells[cellIndex].flui.CalorGas(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini);
        double gasSpecificHeatConstantVolume = state.gasCells[cellIndex].flui.CalorGasVolMod(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini);
        double gasJouleThomson = state.gasCells[cellIndex].flui.JTG(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini);
        double hydrostaticPower = (gasDensity * meanSuperficialGasVelocity) * flowArea * kGravity * sin(state.gasCells[cellIndex].duto.teta);

        state.gasCells[cellIndex].calor.Tint = state.gasCells[cellIndex].tempini;
        state.gasCells[cellIndex].calor.dtL = state.gasCells[cellIndex].tempini - state.gasCells[cellIndex - 1].tempini;
        state.gasCells[cellIndex].calor.Vint = meanSuperficialGasVelocity;
        state.gasCells[cellIndex].calor.dt = state.gasCells[cellIndex].dt;
        state.gasCells[cellIndex].calor.kint = state.gasCells[cellIndex].flui.CondGas(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini);
        state.gasCells[cellIndex].calor.cpint = gasSpecificHeat;
        state.gasCells[cellIndex].calor.rhoint = gasDensity;
        state.gasCells[cellIndex].calor.viscint = state.gasCells[cellIndex].flui.ViscGas(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini) * units::kPascalSecondPerCentipoise;
        double temperaturePerturbation = state.gasCells[cellIndex].temp * 0.01;
        if (fabs(state.gasCells[cellIndex].temp) < 1e-15)
            temperaturePerturbation = 0.1;
        double gasDensityChange = state.gasCells[cellIndex].flui.MasEspGas(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini + temperaturePerturbation) - gasDensity;
        state.gasCells[cellIndex].calor.betint = -(1 / state.gasCells[cellIndex].calor.rhoint) * gasDensityChange / (temperaturePerturbation);
        if ((state.input.pocoTermAxiSim == 0 || state.input.nAxiSim == 0 || state.input.anulAxiSim == 0) || steadyMode != 0) {
            if (steadyMode == 0)
                state.gasCells[cellIndex].fluxcal = state.gasCells[cellIndex].calor.transtrans();
            else
                state.gasCells[cellIndex].fluxcal = state.gasCells[cellIndex].calor.transperm();
        } else if (state.input.pocoTermAxiSim == 1 && state.input.nAxiSim >= 1 && state.input.anulAxiSim == 1 && steadyMode == 0) {
            state.gasCells[cellIndex].fluxcal = axisymmetricHeatFluxOf(
                state, cellIndex, state.gasCells[cellIndex].calor, true);
        }
        if (cellIndex >= state.tubingAnnulusStart && cellIndex <= state.tubingAnnulusEnd && state.networkCoupled == 1) {
            int kconecte = cellIndex - state.tubingAnnulusStart;
            int iconecte = state.annulusTubingStart - kconecte;
            state.gasCells[cellIndex].fluxcal -= state.cells[iconecte].calor.fluxFim;
        }

        double lengthRatio;
        if (hasRightGasCell)
            lengthRatio = cellLength / (cellLength + state.gasCells[cellIndex + 1].dx0);
        else
            lengthRatio = cellLength / (cellLength + state.gasCells[cellIndex - 1].dx0);
        double timeCoefficient = gasDensity * gasSpecificHeatConstantVolume * flowArea;
        double pressureTimeCoefficient = state.gasCells[cellIndex].flui.CalorGasPresMod(state.gasCells[cellIndex].presini, state.gasCells[cellIndex].tempini, state.gasCells[cellIndex].rg) *
                                         (gasDensity * flowArea);

        double temperatureSpatialCoefficient = gasDensity * meanSuperficialGasVelocity * gasSpecificHeat * flowArea;
        double pressureSpatialCoefficient = gasDensity * meanSuperficialGasVelocity * gasJouleThomson * flowArea;
        double pressureGradient;
        if (hasRightGasCell)
            pressureGradient = 2. * (((1 - lengthRatio) * state.gasCells[cellIndex + 1].presini + lengthRatio * state.gasCells[cellIndex].presini) - state.gasCells[cellIndex].presini) * units::kPascalPerKgfPerCm2 / cellLength;
        else
            pressureGradient = 2. * (state.gasCells[cellIndex].presini - ((1 - lengthRatio) * state.gasCells[cellIndex - 1].presini + lengthRatio * state.gasCells[cellIndex].presini)) * units::kPascalPerKgfPerCm2 / cellLength;
        double temperatureGradient = (state.gasCells[cellIndex].tempini - state.gasCells[cellIndex - 1].tempini) / meanCellLength;
        if (hasRightGasCell)
            if (meanSuperficialGasVelocity < 0)
                temperatureGradient = (state.gasCells[cellIndex + 1].tempini - state.gasCells[cellIndex].tempini) / meanCellLength;
        if ((cellIndex == 1 && meanSuperficialGasVelocity <= 0) || (cellIndex == state.lastGasCell && meanSuperficialGasVelocity <= 0))
            temperatureGradient = 0.;

        double kineticTerm;
        double mixtureDensity = gasDensity;
        double leftFaceMeanSuperficialGasVelocity = state.gasCells[cellIndex].VGasL / state.gasCells[cellIndex - 1].u1L;
        double mixtureFluxDifference = (meanSuperficialGasVelocity - leftFaceMeanSuperficialGasVelocity) / cellLength;

        kineticTerm = mixtureDensity * flowArea * meanSuperficialGasVelocity * meanSuperficialGasVelocity * mixtureFluxDifference;

        double gasMassSourceTerm = 0.;
        double liquidMassSourceTerm = 0.;

        const double pressureWorkFactor = 1.;

        state.gasCells[cellIndex].temp = ((timeCoefficient / state.gasCells[cellIndex].dt) * state.gasCells[cellIndex].temp - (pressureWorkFactor) * (pressureTimeCoefficient * (state.gasCells[cellIndex].pres - state.gasCells[cellIndex].presini) * units::kPascalPerKgfPerCm2 / state.gasCells[cellIndex].dt) + state.gasCells[cellIndex].dTdLCor * (-temperatureSpatialCoefficient * temperatureGradient + pressureSpatialCoefficient * pressureGradient - kineticTerm - hydrostaticPower + liquidMassSourceTerm + gasMassSourceTerm + state.gasCells[cellIndex].fluxcal)) / (timeCoefficient / state.gasCells[cellIndex].dt);

        if (state.gasCells[cellIndex].temp < kMinimumTemperatureCelsius)
            state.gasCells[cellIndex].temp = kMinimumTemperatureCelsius;
        if (state.gasCells[cellIndex].temp > kMaximumTemperatureCelsius)
            state.gasCells[cellIndex].temp = kMaximumTemperatureCelsius;

    } else {
        state.gasCells[cellIndex].temp = state.gasCells[cellIndex].calor.Textern1;
    }

    if (cellIndex > 0)
        state.gasCells[cellIndex - 1].tempR = state.gasCells[cellIndex].temp;
    if (hasRightGasCell)
        state.gasCells[cellIndex + 1].tempL = state.gasCells[cellIndex].temp;
}

void computeDischargeTemperature(const ThermalState &state, int cellIndex) {
    Cel &leftProductionCell = state.cells[cellIndex - 1];
    double leftHalfLength = 0.5 * state.gasCells[cellIndex].dxL;
    double rightHalfLength = 0.5 * state.gasCells[cellIndex].dx0;
    double rightGasFraction = 0.;
    if (state.gasCells[cellIndex].razInter <= 0.5)
        rightGasFraction = 2 * state.gasCells[cellIndex].razInter;
    double leftGasFraction = 0.;
    if (state.gasCells[cellIndex - 1].razInter >= 0.5)
        leftGasFraction = 2 * (state.gasCells[cellIndex - 1].razInter - 0.5);
    double leftGasLength = leftHalfLength * leftGasFraction;
    double rightGasLength = rightHalfLength * rightGasFraction;
    double leftLiquidLength = leftHalfLength - leftGasLength;
    double rightLiquidLength = rightHalfLength - rightGasLength;
    double totalLength = leftLiquidLength + rightLiquidLength + leftGasLength + rightGasLength;
    double pressure;
    double temperature;

    pressure = state.gasCells[cellIndex - 1].pres;
    temperature = state.gasCells[cellIndex - 1].temp;
    const double completionFluidDensity = state.gasCells[cellIndex].MasEspFlu(pressure, temperature);
    double mixtureDensity = ((leftLiquidLength + rightLiquidLength) * completionFluidDensity + (leftGasLength + rightGasLength) * state.gasCells[cellIndex].flui.MasEspGas(pressure, temperature)) / totalLength;

    double interfaceVelocity = state.gasCells[cellIndex].VGasL / (completionFluidDensity * state.gasCells[cellIndex - 1].duto.area);
    if (state.gasCells[cellIndex].razInter > (*state.globals).smallValueTolerance)
        interfaceVelocity = state.gasCells[cellIndex].VGasL / (state.gasCells[cellIndex].flui.MasEspGas(pressure, temperature) * state.gasCells[cellIndex - 1].duto.area);
    double liquidSpecificHeat = ((leftLiquidLength + rightLiquidLength) * state.gasCells[cellIndex].CalorLiq(pressure, temperature) + (leftGasLength + rightGasLength) * state.gasCells[cellIndex].flui.CalorGas(pressure, temperature)) / totalLength;

    state.gasCells[cellIndex - 1].calor.Tint = temperature;
    state.gasCells[cellIndex - 1].calor.Vint = interfaceVelocity;
    double liquidConductivity = ((leftLiquidLength + rightLiquidLength) * state.gasCells[cellIndex - 1].CondLiq(pressure, temperature) + (leftGasLength + rightGasLength) * state.gasCells[cellIndex].flui.CondGas(pressure, temperature)) / totalLength;
    state.gasCells[cellIndex - 1].calor.kint = liquidConductivity;
    state.gasCells[cellIndex - 1].calor.cpint = liquidSpecificHeat;
    leftProductionCell.calor.rhoint = mixtureDensity;
    double liquidViscosity = (((leftLiquidLength + rightLiquidLength) * state.gasCells[cellIndex].VisFlu(pressure, temperature) + (leftGasLength + rightGasLength) * state.gasCells[cellIndex].flui.ViscGas(pressure, temperature)) * 1e-3) / totalLength;
    leftProductionCell.calor.viscint = liquidViscosity;

    (void)leftProductionCell.calor.transtrans();

    state.gasCells[cellIndex - 1].tempR = state.gasCells[cellIndex].temp;
    state.gasCells[cellIndex].tempL = state.gasCells[cellIndex - 1].temp;
}

double computeGasLiftDischargeTemperature(const ThermalState &state, int valveIndex) {
    int stepCount = floor((state.gasLiftChokes[valveIndex].presEstag - state.gasLiftChokes[valveIndex].presGarg) / 50) + 1;
    double pressureStep = -(state.gasLiftChokes[valveIndex].presEstag - state.gasLiftChokes[valveIndex].presGarg) / stepCount;
    double stageInletPressure = state.gasLiftChokes[valveIndex].presEstag;
    double stageOutletPressure = state.gasLiftChokes[valveIndex].presEstag + pressureStep;
    double stageInletTemperature = state.gasLiftChokes[valveIndex].tempEstag;
    double stageOutletTemperature;
    for (int cellIndex = 0; cellIndex < stepCount; cellIndex++) {
        double gasSpecificHeat = state.gasLiftChokes[valveIndex].flui.CalorGas(stageInletPressure, stageInletTemperature);
        double compressibilityTemperatureDerivative = state.gasLiftChokes[valveIndex].flui.DZDT(stageInletPressure, stageInletTemperature);
        double inletTemperatureKelvin = stageInletTemperature + units::kZeroCelsiusInKelvin;
        stageOutletTemperature = 1.0 / (1.0 / inletTemperatureKelvin - ((286.998 / state.gasLiftChokes[valveIndex].flui.Deng) * compressibilityTemperatureDerivative / gasSpecificHeat) * log((stageOutletPressure) / (stageInletPressure)));
        stageInletPressure = stageOutletPressure;
        stageOutletPressure = stageOutletPressure - pressureStep;
        stageInletTemperature = stageOutletTemperature - units::kZeroCelsiusInKelvin;
    }
    if (stepCount == 0) {
        double gasSpecificHeat = state.gasLiftChokes[valveIndex].flui.CalorGas(stageInletPressure, stageInletTemperature);
        double gasJouleThomson = state.gasLiftChokes[valveIndex].flui.JTG(stageInletPressure, stageInletTemperature) / gasSpecificHeat;
        stageInletTemperature -= gasJouleThomson * (state.gasLiftChokes[valveIndex].presEstag - state.gasLiftChokes[valveIndex].presGarg) * units::kPascalPerKgfPerCm2;
    }
    return stageInletTemperature;
}

void updateProductionTemperaturePeriphery(const ThermalState &state, int cellIndex) {
    Cel &cell = state.cells[cellIndex];
    if (cellIndex > 0)
        state.cells[cellIndex - 1].tempR = cell.temp;
    if (cellIndex < state.lastCell)
        state.cells[cellIndex + 1].tempL = cell.temp;
    cell.tempini = cell.temp;
}

double surfaceChokeOutletTemperature(const ThermalState &state) {
    const Cel &outlet = state.cells[state.lastCell];
    const double mixtureMassFlow = state.cells[state.lastCell - 1].MR;
    if (state.surfaceChoke.AreaGarg <= 1e-3 * state.cells[state.lastCell - 1].duto.area ||
        outlet.pres <= state.gasSurfacePressure || fabs(mixtureMassFlow) <= 1e-15)
        return outlet.temp;

    const double quality = fabs((mixtureMassFlow - state.cells[state.lastCell - 1].MliqiniR) / mixtureMassFlow);
    const double complementaryFraction = outlet.bet;
    const double complementaryDensity = outlet.fluicol.MasEspFlu(outlet.pres, outlet.temp);
    const double liquidSpecificHeat =
        (1. - complementaryFraction) * outlet.flui.CalorLiq(outlet.pres, outlet.temp) +
        complementaryFraction * outlet.fluicol.CalorLiq(outlet.pres, outlet.temp);
    const double liquidJouleThomson =
        (1. - complementaryFraction) * outlet.flui.JTL(outlet.pres, outlet.temp) -
        complementaryFraction / complementaryDensity;
    const double gasSpecificHeat = outlet.flui.CalorGas(outlet.pres, outlet.temp);
    const double gasJouleThomson = outlet.flui.JTG(outlet.pres, outlet.temp);
    return outlet.temp + ((1. - quality) * liquidJouleThomson / liquidSpecificHeat +
                          quality * gasJouleThomson / gasSpecificHeat) *
                             (state.gasSurfacePressure - outlet.pres) * units::kPascalPerKgfPerCm2;
}

void computeOutletTemperature(const ThermalState &state) {

    if (state.input.chokep.abertura[0] <= 0.6 && state.input.chokep.abertura[0] > (*state.globals).smallValueTolerance && state.outletPressure < state.gasSurfacePressure) {
        double inletMassFlowRate = state.cells[state.lastCell - 1].MR;
        double gasMassFlowRate = state.cells[state.lastCell - 1].MR - state.cells[state.lastCell - 1].MliqiniR;
        double rholc = state.cells[state.lastCell].fluicol.MasEspFlu(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp);
        double betEF = state.cells[state.lastCell].bet;
        double quality = fabs(gasMassFlowRate / inletMassFlowRate);

        double liquidJouleThomson = (1. - betEF) * state.cells[state.lastCell].flui.JTL(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp) - betEF / rholc;
        double gasJouleThomson = state.cells[state.lastCell].flui.JTG(state.cells[state.lastCell].pres, state.cells[state.lastCell].temp);
        state.surfaceTemperature = state.cells[state.lastCell].temp + ((1. - quality) * liquidJouleThomson + quality * gasJouleThomson) * (state.cells[state.lastCell].pres - state.cells[state.lastCell].pres);

    } else
        state.surfaceTemperature = state.cells[state.lastCell - 1].temp;
}

} // namespace sisprod::thermal
