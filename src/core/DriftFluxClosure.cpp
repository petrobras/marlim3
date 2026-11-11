#include "DriftFluxClosure.h"

// <math.h> is load-bearing: the Colebrook loop below calls abs() on double
// operands, and picking up int abs(int) instead would truncate the iterate and
// the convergence delta, ending the loop early with badly wrong values. The
// static_assert fails the build if the overload ever stops being the
// floating-point one.
#include <math.h>
#include <numbers>

#include <type_traits>

// Needed only by driftflux::coefficient below: the correlations part above
// reads nothing but its arguments. estrat.h and mapa.h bring the two
// flow-pattern map classes the variants construct on the stack.
#include "Leitura.h"
#include "SisProdConstants.h"
#include "celula3.h"
#include "estrat.h"
#include "mapa.h"
#include "SimulationContext.h"

static_assert(std::is_same<decltype(abs(1.5)), double>::value,
              "abs() must resolve to the double overload; see the Colebrook "
              "loop in bhagwatGhajarCore");

namespace driftflux {
namespace correlations {
namespace {

/// Sign carried by the duct inclination, negative for downward flow.
///
/// Choi, Hibiki Ishii and Franca Lahey all derive it the same way. It is a
/// plain comparison rather than copysign, which would return -1 for negative
/// zero where this returns 1.
inline double inclinationSignOf(double inclinationAngle) {
    return inclinationAngle < 0. ? -1. : 1.;
}

/// Cross sectional area of a circular duct.
inline double ductArea(double diameter) {
    return std::numbers::pi * diameter * diameter / 4.;
}

/// Forces the drift velocity to point along the inclination when the flow is
/// nearly stagnant.
///
/// Below a combined superficial velocity of 0.01 m/s the correlations can
/// return a drift velocity whose sign contradicts the duct inclination, which
/// is unphysical: buoyancy drives gas upward. All five correlations apply it.
///
/// @param gasFlowRate      Volumetric gas flow rate.
/// @param liquidFlowRate   Volumetric liquid flow rate.
/// @param flowArea         Duct cross sectional area.
/// @param inclinationAngle Duct inclination in radians, negative downward.
/// @param ud               Drift velocity, corrected in place.
inline void alignDriftWithInclination(double gasFlowRate, double liquidFlowRate, double flowArea,
                                      double inclinationAngle, double &ud) {
    const double superficialVelocity = fabs(gasFlowRate / flowArea) + fabs(liquidFlowRate / flowArea);
    if (superficialVelocity < 0.01 && inclinationAngle > 0. && ud < 0.)
        ud = fabs(ud);
    else if (superficialVelocity < 0.01 && inclinationAngle < 0. && ud > 0.)
        ud = -fabs(ud);
}

/// Darcy friction factor, Haaland estimate refined by Colebrook iteration.
///
/// @param relativeRoughness Roughness divided by diameter.
/// @param reynolds          Reynolds number, already floored away from zero.
/// @return Darcy friction factor.
///
/// The fixed point converges in at most two iterations over a grid far wider
/// than anything the engine produces, Reynolds from 1e-9 to 1e12 and roughness
/// from smooth to 1e-2, so the bound below is insurance and never binds.
///
/// A stall is not the real hazard anyway: if the iterate turns into NaN the
/// convergence delta does too, and a NaN comparison is false, so the loop
/// exits on its own.
double darcyFrictionFactor(double relativeRoughness, double reynolds) {
    if (reynolds > 2400) { // turbulent flow regime
        double frictionFactorEstimate =
            (1 / (-18e-1 * log10(pow((relativeRoughness / (3.7)), 1.11) + (69e-1 / (reynolds + 1e-15)))));
        frictionFactorEstimate *= frictionFactorEstimate; // Haaland.
        // Never reached in practice, see the note above.
        constexpr int kMaxColebrookIterations = 100;
        int iterations = 0;
        double frictionFactor;
        double convergenceDelta;
        do {
            const double colebrookDenominator =
                -2 * log10(((relativeRoughness) / 3.7) + 2.51 / ((reynolds + 1e-15) * sqrt(abs(frictionFactorEstimate))));
            frictionFactor = 1 / (colebrookDenominator * colebrookDenominator); // Colebrook.
            convergenceDelta = abs(frictionFactor - frictionFactorEstimate);
            frictionFactorEstimate = frictionFactor;
        } while (convergenceDelta >= 1e-3 && ++iterations < kMaxColebrookIterations);
        return frictionFactor;
    }
    return 64. / (reynolds); // 16.
}

/// Shared implementation of the two Bhagwat and Ghajar variants.
///
/// Bhagwat, S. M. and Ghajar, A. J. (2014), "A flow pattern independent drift
/// flux model based void fraction correlation for a wide range of gas liquid
/// two phase flow", International Journal of Multiphase Flow, vol. 59.
///
/// The published correlation builds the distribution parameter from three
/// additive terms and the drift velocity from an inclination factor, a
/// buoyancy scale and three correction factors. The two exported variants run
/// exactly this algorithm and differ only in which Reynolds number feeds the
/// friction factor and the Reynolds dependent terms.
///
/// @param reynolds Reynolds number the variant selects, mixture or liquid.
///
/// @note This variant carries no flow direction sign, unlike Choi, Hibiki Ishii
///       and Franca Lahey. Taking -1 for downward flow, as they do, is not a
///       fix: it drives the Froude radicand negative, and although the resulting
///       NaN never reaches c0 or ud, it makes both guards below permanently
///       false, so the duct shape term would stop being zeroed and
///       downwardFlowSign would stop flipping for downward low Froude flow.
///       Direction is handled by downwardFlowSign, factor C4 of the published
///       model.
void bhagwatGhajarCore(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                       double reynolds, double gasFlowRate, double liquidFlowRate, double diameter,
                       double roughness, double inclinationAngle, double &c0, double &ud,
                       double horizontalDriftSign) {
    const double flowArea = ductArea(diameter);
    const double mixtureDensity = voidFraction * gasDensity + (1. - voidFraction) * liquidDensity;


    const double froudeNumber = sqrt(gasDensity / (liquidDensity - gasDensity)) * ((gasFlowRate) / flowArea) / sqrt(9.81 * diameter * cos(inclinationAngle));
    const double massQuality = (gasDensity * fabs(gasFlowRate) / flowArea) / ((gasDensity * fabs(gasFlowRate) / flowArea) + (liquidDensity * fabs(liquidFlowRate) / flowArea));
    const double noSlipGasFraction = (fabs(gasFlowRate) / flowArea) / ((fabs(gasFlowRate) / flowArea) + (fabs(liquidFlowRate) / flowArea));

    const double relativeRoughness = roughness / diameter;

    if (reynolds < 0.0000001)
        reynolds = 0.0000001;
    const double frictionFactor = darcyFrictionFactor(relativeRoughness, reynolds);

    double densityRatioSquared = gasDensity / liquidDensity;
    densityRatioSquared *= densityRatioSquared;
    double scaledReynoldsSquared = reynolds / 1000;
    scaledReynoldsSquared *= scaledReynoldsSquared;
    const double distributionTerm1 = (2 - densityRatioSquared) / (1 + scaledReynoldsSquared);
    const double distributionTerm2 = (pow(((1 + densityRatioSquared * cos(inclinationAngle)) / (1 + cos(inclinationAngle))), (1 - voidFraction) / 5.)) /
             (1 + 1 / scaledReynoldsSquared);
    const double ductShapeCoefficient = 0.2; // circular or annular duct; a rectangular one would be 0.4.
    double ductShapeTerm = (ductShapeCoefficient - ductShapeCoefficient * sqrt(gasDensity / liquidDensity)) * (pow((2.6 - noSlipGasFraction), 0.15) - sqrt(frictionFactor)) * pow((1 - massQuality), 1.5);
    if (gasFlowRate * liquidFlowRate < 0.)
        ductShapeTerm = 0;
    if (inclinationAngle >= -50 * std::numbers::pi / 180. && inclinationAngle <= 0 && froudeNumber <= 0.1)
        ductShapeTerm = 0.0;
    c0 = distributionTerm1 + distributionTerm2 + ductShapeTerm; // Distribution parameter.

    const double mixtureViscosity = diameter * (fabs(gasFlowRate / flowArea) + fabs(liquidFlowRate / flowArea)) * mixtureDensity / reynolds;
    const double inclinationFactor = (0.35 * sin(inclinationAngle) + 0.45 * cos(inclinationAngle));
    const double buoyancyVelocityScale = sqrt((9.81 * diameter * (liquidDensity - gasDensity) / liquidDensity)) * sqrt(1 - voidFraction);
    const double viscosityCorrection =
        (mixtureViscosity / 0.001 > 10) ? pow((0.434 / (log10(mixtureViscosity / 0.001))), 0.15) : 1.0;
    const double laplaceNumber = sqrt(surfaceTension / (9.81 * (liquidDensity - gasDensity))) / diameter;
    const double laplaceCorrection =
        (laplaceNumber < 0.025) ? pow((laplaceNumber / 0.025), 0.90) : 1.0;
    const double downwardFlowSign =
        (inclinationAngle >= -(50 * std::numbers::pi / 180.) && inclinationAngle < 0 && froudeNumber <= 0.1) ? -1.0 : 1.0;
    ud = horizontalDriftSign * inclinationFactor * buoyancyVelocityScale * viscosityCorrection * laplaceCorrection * downwardFlowSign; // Drift velocity.
    alignDriftWithInclination(gasFlowRate, liquidFlowRate, flowArea, inclinationAngle, ud);
}

} // namespace

void BhagwatGhajar(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                   double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                   double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                   double horizontalDriftSign) {
    bhagwatGhajarCore(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                      gasFlowRate, liquidFlowRate, diameter, roughness, inclinationAngle, c0, ud,
                      horizontalDriftSign);
}

void BhagwatGhajarMod(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                      double mixtureReynolds, double liquidReynolds, double gasFlowRate,
                      double liquidFlowRate, double diameter, double roughness, double inclinationAngle,
                      double &c0, double &ud, double horizontalDriftSign) {
    bhagwatGhajarCore(liquidDensity, gasDensity, surfaceTension, voidFraction, liquidReynolds,
                      gasFlowRate, liquidFlowRate, diameter, roughness, inclinationAngle, c0, ud,
                      horizontalDriftSign);
}

void Choi(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
          double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
          double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
          double horizontalDriftSign) {
    const double inclinationSign = inclinationSignOf(inclinationAngle);
    const double flowArea = ductArea(diameter);
    ud = horizontalDriftSign * inclinationSign * 0.0246 * cos(inclinationAngle) + 1.606 * pow(sisprod::kGravity * surfaceTension * (liquidDensity - gasDensity) / (liquidDensity * liquidDensity), 0.25) * sin(inclinationAngle);
    c0 = 2. / (1 + pow(mixtureReynolds / 1000., 2.)) + (1.2 - 0.2 * sqrt(gasDensity / liquidDensity) * (1 - exp(-18 * voidFraction))) / (1 + pow(1000. / mixtureReynolds, 2.));
    alignDriftWithInclination(gasFlowRate, liquidFlowRate, flowArea, inclinationAngle, ud);
}

void HibikiIshii(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                 double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                 double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                 double horizontalDriftSign) {
    const double inclinationSign = inclinationSignOf(inclinationAngle);
    const double flowArea = ductArea(diameter);
    c0 = 1. + (1. - voidFraction) / (voidFraction + 4. * sqrt(gasDensity / liquidDensity));
    ud = (horizontalDriftSign * inclinationSign * (1. - voidFraction) / (voidFraction + 4. * sqrt(gasDensity / liquidDensity))) * sqrt(sisprod::kGravity * fabs(sin(inclinationAngle)) * diameter * (liquidDensity - gasDensity) * (1. - voidFraction) / (0.015 * liquidDensity));
    alignDriftWithInclination(gasFlowRate, liquidFlowRate, flowArea, inclinationAngle, ud);
}

void FrancaLahey(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                 double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                 double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                 double horizontalDriftSign) {
    const double inclinationSign = inclinationSignOf(inclinationAngle);
    c0 = 1.04;
    ud = horizontalDriftSign * inclinationSign * 0.466;
    const double flowArea = ductArea(diameter);
    alignDriftWithInclination(gasFlowRate, liquidFlowRate, flowArea, inclinationAngle, ud);
}

namespace {

/// Blends Bhagwat Ghajar and Choi across the inclination band from 5 to 20
/// degrees, which is selector 5 in all three regimes.
///
/// @warning Two properties here are load bearing. BhagwatGhajarMod runs before
///          Choi because both write c0 and ud, so the order decides which
///          result the temporaries hold. And the interpolation keeps the form
///          ratio*x + (1 - ratio)*xMod, because rearranging it to
///          xMod + ratio*(x - xMod) is algebraically identical and rounds
///          differently.
void blendAcrossInclination(double liquidDensity, double gasDensity, double surfaceTension,
                            double voidFraction, double mixtureReynolds, double liquidReynolds,
                            double gasFlowRate, double liquidFlowRate, double diameter,
                            double roughness, double inclinationAngle, double &c0, double &ud,
                            double horizontalDriftSign) {
    if (fabs(inclinationAngle) < 5 * std::numbers::pi / 180.) {
        BhagwatGhajar(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                      liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness,
                      inclinationAngle, c0, ud, horizontalDriftSign);
    } else if (fabs(inclinationAngle) > 20 * std::numbers::pi / 180.) {
        Choi(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
             liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness, inclinationAngle,
             c0, ud, horizontalDriftSign);
    } else {
        const double blendRatio = (fabs(inclinationAngle) - 5 * std::numbers::pi / 180.) / (15 * std::numbers::pi / 180.);
        BhagwatGhajarMod(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                         liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness,
                         inclinationAngle, c0, ud, horizontalDriftSign);
        const double c0BhagwatGhajarMod = c0;
        const double udBhagwatGhajarMod = ud;
        Choi(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
             liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness, inclinationAngle,
             c0, ud, horizontalDriftSign);
        c0 = blendRatio * c0 + (1. - blendRatio) * c0BhagwatGhajarMod;
        ud = blendRatio * ud + (1. - blendRatio) * udBhagwatGhajarMod;
    }
}

/// Applies the selectors that every flow regime accepts.
///
/// Anything outside this set falls through and leaves c0 and ud exactly as the
/// caller passed them. That fall-through is observable behaviour, which is also
/// why the three regimes keep separate case sets instead of sharing one table.
void applyCommonCorrelation(int correlationIndex, double liquidDensity, double gasDensity,
                            double surfaceTension, double voidFraction, double mixtureReynolds,
                            double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                            double diameter, double roughness, double inclinationAngle, double &c0,
                            double &ud, double horizontalDriftSign) {
    switch (correlationIndex) {
    case 0:
        Choi(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
             liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness, inclinationAngle,
             c0, ud, horizontalDriftSign);
        break;
    case 1:
        BhagwatGhajar(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                      liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness,
                      inclinationAngle, c0, ud, horizontalDriftSign);
        break;
    case 4:
        BhagwatGhajarMod(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                         liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness,
                         inclinationAngle, c0, ud, horizontalDriftSign);
        break;
    case 5:
        blendAcrossInclination(liquidDensity, gasDensity, surfaceTension, voidFraction,
                               mixtureReynolds, liquidReynolds, gasFlowRate, liquidFlowRate,
                               diameter, roughness, inclinationAngle, c0, ud, horizontalDriftSign);
        break;
    }
}

} // namespace

void C0UdDisperso(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                  double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                  double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                  double horizontalDriftSign, int estabCol, int correlationIndex) {
    applyCommonCorrelation(correlationIndex, liquidDensity, gasDensity, surfaceTension, voidFraction,
                           mixtureReynolds, liquidReynolds, gasFlowRate, liquidFlowRate, diameter,
                           roughness, inclinationAngle, c0, ud, horizontalDriftSign);
}

void C0UdAnularChurn(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                     double mixtureReynolds, double liquidReynolds, double gasFlowRate,
                     double liquidFlowRate, double diameter, double roughness, double inclinationAngle,
                     double &c0, double &ud, double horizontalDriftSign, int estabCol, int correlationIndex) {
    // Hibiki Ishii is accepted in this regime and in no other.
    if (correlationIndex == 3) {
        HibikiIshii(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                    liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness,
                    inclinationAngle, c0, ud, horizontalDriftSign);
        return;
    }
    applyCommonCorrelation(correlationIndex, liquidDensity, gasDensity, surfaceTension, voidFraction,
                           mixtureReynolds, liquidReynolds, gasFlowRate, liquidFlowRate, diameter,
                           roughness, inclinationAngle, c0, ud, horizontalDriftSign);
}

void C0UdEstratificado(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                       double mixtureReynolds, double liquidReynolds, double gasFlowRate,
                       double liquidFlowRate, double diameter, double roughness, double inclinationAngle,
                       double &c0, double &ud, double horizontalDriftSign, int estabCol,
                       int correlationIndex) {
    // Franca Lahey is accepted in this regime and in no other.
    if (correlationIndex == 2) {
        FrancaLahey(liquidDensity, gasDensity, surfaceTension, voidFraction, mixtureReynolds,
                    liquidReynolds, gasFlowRate, liquidFlowRate, diameter, roughness,
                    inclinationAngle, c0, ud, horizontalDriftSign);
        return;
    }
    applyCommonCorrelation(correlationIndex, liquidDensity, gasDensity, surfaceTension, voidFraction,
                           mixtureReynolds, liquidReynolds, gasFlowRate, liquidFlowRate, diameter,
                           roughness, inclinationAngle, c0, ud, horizontalDriftSign);
}

}  // namespace correlations

namespace coefficient {

using enum sisprod::AccessoryKind;

/*
 * The five variants disagree about which cell's accessory the horizontal
 * correction reads, whether the flow-pattern map runs at all, whether the
 * transition counter is kept, and which of arq.escorregaTran and
 * arq.escorregaPerm ends the calculation, so each keeps its own control flow
 * and shares only the blocks they have exactly in common.
 *
 * The `// beta doubt` markers sit on the unconditional assignments of betI.
 */

namespace {

/*
 * Data sources.
 *
 * The five variants read three sources: CalcC0Ud and CalcC0UdIni read exactly
 * the same fields, and so do CalcC0UdBuf and CalcC0UdIniBuf -- the
 * initialisation variants differ from the others in control flow, not in
 * source. What varies is instantaneous versus buffered versus steady state.
 *
 * The hooks are called AT THE POINT OF USE, never hoisted into a local at the
 * top of a body: a value read once and reused would turn a conditional read
 * into an unconditional one. gasForSign and gasFlowRate are separate hooks
 * because they are separate expressions in the instantaneous variants: the
 * sign tests read QG, while the flow rate is MC - Mliqini.
 *
 * Stateless structs with static members, resolved at compile time, defined in
 * the same translation unit as their only callers: no indirect call survives.
 */

/// Instantaneous state -- instantaneous and initialization.
struct InstantaneousSource {
    /// Value whose SIGN the variant tests. Not the flow rate: these variants
    /// branch on QG but divide MC - Mliqini by the density.
    static double gasForSign(const Cel *cells, int index) { return cells[index].QG; }
    /// Gas mass flow: total minus its liquid part.
    static double gasFlowRate(const Cel *cells, int index) {
        return cells[index].MC - cells[index].Mliqini;
    }
    /// Liquid mass flow.
    static double liquidFlowRate(const Cel *cells, int index) { return cells[index].Mliqini; }
};

/// Buffered network state -- buffered and bufferedInitialization.
struct BufferedSource {
    /// Value whose SIGN the variant tests. Here it is the same expression as the
    /// flow rate, unlike the instantaneous source; both hooks are kept so the
    /// two roles stay distinguishable at the call sites.
    static double gasForSign(const Cel *cells, int index) {
        return cells[index].MCBuf - cells[index].MliqiniBuf;
    }
    /// Buffered gas mass flow: total minus its liquid part.
    static double gasFlowRate(const Cel *cells, int index) {
        return cells[index].MCBuf - cells[index].MliqiniBuf;
    }
    /// Buffered liquid mass flow.
    static double liquidFlowRate(const Cel *cells, int index) { return cells[index].MliqiniBuf; }
};

/// Steady state -- steadyState. It takes the magnitude of both rates and
/// never tests their sign, so it has no gasForSign.
struct SteadyStateSource {
    /// Gas mass flow, magnitude only -- the steady-state variant never branches
    /// on direction, which is why this source has no gasForSign.
    static double gasFlowRate(const Cel *cells, int index) {
        return fabs(cells[index].MC - cells[index].Mliqini);
    }
    /// Liquid mass flow, magnitude only.
    static double liquidFlowRate(const Cel *cells, int index) {
        return fabs(cells[index].Mliqini);
    }
};

/// Liquid holdup at the face, for the transient variants.
///
/// Shared by CalcC0Ud and CalcC0UdBuf: gasForSign supplies the gas rate the
/// sign tests read, QG or MCBuf - MliqiniBuf. It is called at each test rather
/// than read once into a local, so a value tested twice is read twice.
///
/// Of the two localtiny guards, the first replaces a holdup derived from a
/// vanishing gas rate, the second rejects one that has collapsed onto either
/// end of its range.
template <typename Source>
double transientLiquidHoldup(const ClosureState &state, int cellIndex) {
    double liquidHoldup;
    if (cellIndex > 0)
        liquidHoldup = 1. - state.cells[cellIndex - 1].alfPigD;
    else
        liquidHoldup = 1. - state.cells[cellIndex].alf;
    if (Source::gasForSign(state.cells, cellIndex) < 0)
        liquidHoldup = 1. - state.cells[cellIndex].alfPigE;
    if (fabs(Source::gasForSign(state.cells, cellIndex)) < (*state.globals).smallValueTolerance * 1e-5) {
        if (fabs(state.cells[cellIndex].alfPigE) < (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex].alfPigE) < (*state.globals).smallValueTolerance && fabs(state.cells[cellIndex - 1].alfPigD) > (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex - 1].alfPigD) > (*state.globals).smallValueTolerance)
            liquidHoldup = 1. - state.cells[cellIndex - 1].alfPigD;
        else if (fabs(state.cells[cellIndex - 1].alfPigD) < (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex - 1].alfPigD) < (*state.globals).smallValueTolerance && fabs(state.cells[cellIndex].alfPigE) > (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex].alfPigE) > (*state.globals).smallValueTolerance)
            liquidHoldup = 1. - state.cells[cellIndex].alfPigE;
        else
            liquidHoldup = 0.5 * (1. - state.cells[cellIndex].alfPigE + 1. - state.cells[cellIndex - 1].alfPigD);
    }
    if (liquidHoldup < (*state.globals).smallValueTolerance || liquidHoldup > 1. - (*state.globals).smallValueTolerance)
        liquidHoldup = 0.5 * (1. - state.cells[cellIndex].alfPigE + 1. - state.cells[cellIndex - 1].alfPigD);
    return liquidHoldup;
}

/// Duct inclination seen by the face, for the transient variants.
///
/// The length-weighted mean of the two duct inclinations, overridden two cells
/// downstream of a shut choke by whichever single duct the flow actually comes
/// from. Shared by CalcC0Ud and CalcC0UdBuf.
///
/// The weighting reads cellLength against dutoL and leftCellLength against
/// duto -- crossed, which is the usual interpolation. CalcC0UdPerm weights it
/// the other way round and therefore does not use this function.
template <typename Source>
double transientInclinationAngle(const ClosureState &state, int cellIndex) {
    const double totalLength = state.cells[cellIndex].dxL + state.cells[cellIndex].dx;
    const double cellLength = state.cells[cellIndex].dx;
    const double leftCellLength = state.cells[cellIndex].dxL;
    double inclinationAngle = (cellLength * state.cells[cellIndex].dutoL.teta + leftCellLength * state.cells[cellIndex].duto.teta) / totalLength;
    if (cellIndex >= 2) {
        if (state.cells[cellIndex - 2].acsr.tipo == kAccessoryChoke && state.cells[cellIndex - 2].acsr.chk.AreaGarg <= (1e-3)) {
            if (Source::gasForSign(state.cells, cellIndex) >= 0)
                inclinationAngle = state.cells[cellIndex].duto.teta;
            else
                inclinationAngle = state.cells[cellIndex].dutoR.teta;
        } else {
            if (Source::gasForSign(state.cells, cellIndex) >= 0)
                inclinationAngle = state.cells[cellIndex].dutoL.teta;
            else
                inclinationAngle = state.cells[cellIndex].duto.teta;
        }
    }
    return inclinationAngle;
}

/// Sign correction applied to the drift term on a horizontal face.
///
/// The four transient variants differ in which cell's accessory opens the
/// guard: CalcC0Ud reads the upstream one, the other three the face's own, so
/// the accessory cell is a parameter.
///
/// The two arms differ in more than the accessory: the first requires both
/// junction angles to agree in sign, the second reads the downstream angle
/// alone. So the guard is not decorative, and a call site that passed the wrong
/// cell would change results wherever the two cells carry different accessories.
double horizontalDriftSignOf(const ClosureState &state, int cellIndex, int accessoryCellIndex) {
    double horizontalDriftSign = 1.;
    if (fabs(state.cells[cellIndex].duto.teta) < 1e-10) {
        if (state.cells[accessoryCellIndex].acsr.tipo != kAccessoryChoke || state.cells[accessoryCellIndex].acsr.chk.AreaGarg > 1e-10) {
            if (state.cells[cellIndex].angEsq < 0 && state.cells[cellIndex].angDir < 0)
                horizontalDriftSign = -1.;
            else if (state.cells[cellIndex].angEsq > 0 && state.cells[cellIndex].angDir > 0)
                horizontalDriftSign = 1.;
        } else {
            if (state.cells[cellIndex].angDir < 0)
                horizontalDriftSign = -1.;
            else if (state.cells[cellIndex].angDir > 0)
                horizontalDriftSign = 1.;
        }
    }
    return horizontalDriftSign;
}

/// Liquid holdup at the face, for the initialisation variants.
///
/// Same shape as the transient one, reading the inlet void fraction instead of
/// the neighbouring cell's. Shared by CalcC0UdIni and CalcC0UdIniBuf.
///
/// One guard mixes the two: the second condition of the else-if tests
/// cells[cellIndex - 1].alfPigD while everything around it reads the inlet
/// fraction.
template <typename Source>
double inletLiquidHoldup(const ClosureState &state, int cellIndex) {
    double liquidHoldup = 1 - state.inletVoidFraction;
    if (Source::gasForSign(state.cells, cellIndex) < 0)
        liquidHoldup = 1. - state.cells[cellIndex].alfPigE;
    if (fabs(Source::gasForSign(state.cells, cellIndex)) < (*state.globals).smallValueTolerance * 1e-5) {
        if (fabs(state.cells[cellIndex].alfPigE) < (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex].alfPigE) < (*state.globals).smallValueTolerance && fabs(state.inletVoidFraction) > (*state.globals).smallValueTolerance && fabs(1. - state.inletVoidFraction) > (*state.globals).smallValueTolerance)
            liquidHoldup = 1 - state.inletVoidFraction;
        else if (fabs(state.inletVoidFraction) < (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex - 1].alfPigD) < (*state.globals).smallValueTolerance && fabs(state.cells[cellIndex].alfPigE) > (*state.globals).smallValueTolerance && fabs(1. - state.cells[cellIndex].alfPigE) > (*state.globals).smallValueTolerance)
            liquidHoldup = 1. - state.cells[cellIndex].alfPigE;
        else
            liquidHoldup = 0.5 * (1. - state.cells[cellIndex].alfPigE + 1. - state.inletVoidFraction);
    }
    if (liquidHoldup < (*state.globals).smallValueTolerance || liquidHoldup > 1. - (*state.globals).smallValueTolerance)
        liquidHoldup = 0.5 * (1. - state.cells[cellIndex].alfPigE + 1. - state.inletVoidFraction);
    return liquidHoldup;
}

/// No slip while a pig occupies the face: the drift terms are forced off and the
/// flow pattern is pinned to slug.
///
/// The body of both arms of the pig guard in the two transient variants: the
/// conditions differ, what they do does not.
void applyPigOverride(const ClosureState &state, int cellIndex, double &c0, double &ud) {
    c0 = 1.;
    ud = 0.;
    state.cells[cellIndex].arranjo = 1.;
    state.cells[cellIndex - 1].arranjoR = 1.;
    state.cells[cellIndex - 1].perdaEstratL = 0.;
    state.cells[cellIndex - 1].perdaEstratG = 0.;
}

/// The phase properties the flow scales are built from.
///
/// Named for the same reason as MixtureProperties: five adjacent doubles are
/// five chances to transpose a pair with nothing to catch it.
struct PhaseProperties {
    double liquidDensity;
    double gasDensity;
    double liquidViscosity;
    double gasViscosity;
    double liquidHoldup;
};

/// The scalars the closure helpers below read, named instead of counted.
///
/// Built once per variant with designated initializers, immediately before the
/// Reynolds guard that gates both helpers, so a transposed pair of doubles
/// shows on the line where it happens. The field order is the one the
/// correlation signatures use.
///
/// The fields are REFERENCES, for the reason ClosureState holds references:
/// several of these locals are assigned again further down, and a copy taken
/// here would freeze the value at construction rather than at use.
struct MixtureProperties {
    const double &liquidDensity;
    const double &gasDensity;
    const double &surfaceTension;
    const double &voidFraction;
    const double &gasFlowRate;     ///< A volumetric rate.
    const double &liquidFlowRate;  ///< A volumetric rate.
    const double &diameter;
    const double &flowArea;
    const double &mixtureReynolds;
    const double &liquidReynolds;
    const double &inclinationAngle;
    const double &horizontalDriftSign;
};

/// Pressure and temperature the property model is evaluated at, for CalcC0Ud.
struct MeanConditions {
    double pressure;
    double temperature;
};

MeanConditions instantaneousMeanConditions(const ClosureState &state, int cellIndex) {
    const double meanPressure = cellIndex == state.lastCell ? state.cells[cellIndex].pres : state.cells[cellIndex].presaux;
    const double meanTemperature = cellIndex < state.lastCell ? state.cells[cellIndex].temp : state.gasSurfaceTemperature;
    return MeanConditions{meanPressure, meanTemperature};
}

/// Phase properties at the face, from the instantaneous state.
///
/// The only variant that consults the cached densities rpCi, rcCi and rgCi: at
/// the two ends of the line it calls the property model, and in between it
/// takes the cache. The other four always call the model.
PhaseProperties instantaneousPhaseProperties(const ClosureState &state, int cellIndex, double betI,
                                             double meanPressure, double meanTemperature,
                                             double liquidHoldup, double &surfaceTension) {
    double liquidDensity;
    double liquidViscosity;
    if (state.cells[cellIndex].QL < 0.) {
        if (cellIndex == 0 || cellIndex == state.lastCell)
            liquidDensity = (1 - betI) * state.cells[cellIndex].flui.MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);
        else
            liquidDensity = (1 - betI) * state.cells[cellIndex].rpCi + betI * state.cells[cellIndex].rcCi;
        liquidViscosity = (1 - betI) * state.cells[cellIndex].flui.ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(meanPressure, meanTemperature);
        surfaceTension = (1 - betI) * state.cells[cellIndex].flui.TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);
    } else {
        if (cellIndex == 0 || cellIndex == state.lastCell)
            liquidDensity = (1 - betI) * state.cells[cellIndex - 1].flui.MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex - 1].fluicol.MasEspFlu(meanPressure, meanTemperature);
        else
            liquidDensity = (1 - betI) * state.cells[cellIndex].rpCi + betI * state.cells[cellIndex - 1].rcCi;
        liquidViscosity = (1 - betI) * state.cells[cellIndex - 1].flui.ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex - 1].fluicol.VisFlu(meanPressure, meanTemperature);
        surfaceTension = (1 - betI) * state.cells[cellIndex - 1].flui.TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex - 1].fluicol.TensSuper(meanPressure, meanTemperature);
    }

    double gasDensity;
    double gasViscosity;
    if (state.cells[cellIndex].QG < 0.) {
        if (cellIndex == 0 || cellIndex == state.lastCell)
            gasDensity = state.cells[cellIndex].flui.MasEspGas(meanPressure, meanTemperature);
        else
            gasDensity = state.cells[cellIndex].rgCi;
        gasViscosity = state.cells[cellIndex].flui.ViscGas(meanPressure, meanTemperature);
    } else {
        if (cellIndex == 0 || cellIndex == state.lastCell)
            gasDensity = state.cells[cellIndex - 1].flui.MasEspGas(meanPressure, meanTemperature);
        else
            gasDensity = state.cells[cellIndex].rgCi;
        gasViscosity = state.cells[cellIndex - 1].flui.ViscGas(meanPressure, meanTemperature);
    }
    return PhaseProperties{liquidDensity, gasDensity, liquidViscosity, gasViscosity, liquidHoldup};
}

/// The dispersed and stratified closures, evaluated as a pair and then
/// blended: one helper fills it, the other reads it.
struct FlowPatternPair {
    double dispersedC0;
    double dispersedUd;
    double stratifiedC0;
    double stratifiedUd;
};

/// The flow rates and the two Reynolds numbers built from them.
///
/// The five call sites take these apart with a structured binding, so THE ORDER
/// OF THESE FIELDS IS LOAD-BEARING: reordering them silently rebinds every call
/// site to the wrong values.
struct FlowScales {
    double gasVolumetricFlowRate;
    double liquidVolumetricFlowRate;
    double diameter;
    double area;
    double mixture;
    double liquid;
};

/// Flow rates, duct size and Reynolds numbers, the same in all five variants
/// apart from where the rates come from.
///
/// The duct diameter is chosen INSIDE this function, not passed in, because
/// cells[cellIndex - 1].duto.a is read only when cellIndex > 0 and the gas
/// volumetric flow rate is not negative. Taking it as an argument would make
/// that read unconditional.
template <typename Source>
FlowScales flowScalesOf(const ClosureState &state, int cellIndex, const PhaseProperties &phases) {
    double gasVolumetricFlowRate = Source::gasFlowRate(state.cells, cellIndex) / phases.gasDensity;
    double liquidVolumetricFlowRate = Source::liquidFlowRate(state.cells, cellIndex) / phases.liquidDensity;
    double diameter = state.cells[cellIndex].duto.a;
    if (cellIndex > 0 && gasVolumetricFlowRate >= 0)
        diameter = state.cells[cellIndex - 1].duto.a;
    double flowArea = std::numbers::pi * diameter * diameter / 4.;

    double mixtureDensity = phases.liquidHoldup * phases.liquidDensity + (1 - phases.liquidHoldup) * phases.gasDensity;
    double mixtureViscosity = (phases.liquidHoldup * phases.liquidViscosity + (1 - phases.liquidHoldup) * phases.gasViscosity) / pow(10., 3.);
    double mixtureReynolds = diameter * mixtureDensity * (fabs(gasVolumetricFlowRate) / flowArea + fabs(liquidVolumetricFlowRate) / flowArea) / mixtureViscosity;
    double liquidReynolds = diameter * phases.liquidDensity * (fabs(gasVolumetricFlowRate) / flowArea + fabs(liquidVolumetricFlowRate) / flowArea) / (phases.liquidViscosity / 1000.);
    return FlowScales{gasVolumetricFlowRate, liquidVolumetricFlowRate, diameter, flowArea, mixtureReynolds, liquidReynolds};
}

/// Dispersed and stratified closure, evaluated as a pair; the same in all five
/// variants.
void evaluateFlowPatternPair(const ClosureState &state, int cellIndex, const MixtureProperties &mix,
                             FlowPatternPair &pair) {
    driftflux::correlations::C0UdDisperso(mix.liquidDensity, mix.gasDensity, mix.surfaceTension, mix.voidFraction, mix.mixtureReynolds, mix.liquidReynolds, mix.gasFlowRate, mix.liquidFlowRate, mix.diameter,
                                          state.cells[cellIndex].duto.rug, mix.inclinationAngle, pair.dispersedC0, pair.dispersedUd, mix.horizontalDriftSign,
                                          state.cells[cellIndex].estabCol, state.selectors.dispersed);
    driftflux::correlations::C0UdEstratificado(mix.liquidDensity, mix.gasDensity, mix.surfaceTension, mix.voidFraction, mix.mixtureReynolds, mix.liquidReynolds, mix.gasFlowRate, mix.liquidFlowRate, mix.diameter,
                                               state.cells[cellIndex].duto.rug, mix.inclinationAngle, pair.stratifiedC0, pair.stratifiedUd, mix.horizontalDriftSign,
                                               state.cells[cellIndex].estabCol, state.selectors.stratified);
}

/// Blends the dispersed and stratified results by superficial velocity, with a
/// linear ramp between the two limits.
///
/// The ramp is written (1. - blendRatio) * stratified + blendRatio * dispersed
/// and MUST stay that way: the algebraically equal
/// dispersed + (1. - blendRatio) * (stratified - dispersed) rounds differently.
void blendBySuperficialVelocity(const MixtureProperties &mix, const FlowPatternPair &pair,
                                double &c0, double &ud) {
    double maxSuperficialVelocity = 0.05;
    double minSuperficialVelocity = 0.005;
    if ((fabs(mix.gasFlowRate) + fabs(mix.liquidFlowRate)) / mix.flowArea > maxSuperficialVelocity) {
        c0 = pair.stratifiedC0;
        ud = pair.stratifiedUd;
    } else if ((fabs(mix.gasFlowRate) + fabs(mix.liquidFlowRate)) / mix.flowArea < minSuperficialVelocity) {
        c0 = pair.dispersedC0;
        ud = pair.dispersedUd;
    } else {
        double blendRatio = (maxSuperficialVelocity - (fabs(mix.gasFlowRate) + fabs(mix.liquidFlowRate)) / mix.flowArea) / (maxSuperficialVelocity - minSuperficialVelocity);
        c0 = ((1. - blendRatio) * pair.stratifiedC0 + blendRatio * pair.dispersedC0);
        ud = ((1. - blendRatio) * pair.stratifiedUd + blendRatio * pair.dispersedUd);
    }
}

/// Dispersed closure, upgraded to annular/churn when the pattern says so.
void evaluateDispersedOrAnnular(const ClosureState &state, int cellIndex, const MixtureProperties &mix,
                                int flowPattern, double &c0, double &ud) {
    driftflux::correlations::C0UdDisperso(mix.liquidDensity, mix.gasDensity, mix.surfaceTension, mix.voidFraction, mix.mixtureReynolds, mix.liquidReynolds, mix.gasFlowRate, mix.liquidFlowRate, mix.diameter,
                                          state.cells[cellIndex].duto.rug, mix.inclinationAngle, c0, ud, mix.horizontalDriftSign,
                                          state.cells[cellIndex].estabCol, state.selectors.dispersed);
    if (flowPattern == -2) {
        driftflux::correlations::C0UdAnularChurn(mix.liquidDensity, mix.gasDensity, mix.surfaceTension, mix.voidFraction, mix.mixtureReynolds, mix.liquidReynolds, mix.gasFlowRate, mix.liquidFlowRate,
                                                 mix.diameter, state.cells[cellIndex].duto.rug, mix.inclinationAngle, c0, ud,
                                                 mix.horizontalDriftSign, state.cells[cellIndex].estabCol,
                                                 state.selectors.annularChurn);
    }
}

/// Discards the computed slip when the deck disables it: the field read is
/// escorregaTran in the four transient variants and escorregaPerm in the
/// steady-state one.
void applyNoSlipOverride(int slipEnabled, double &c0, double &ud) {
    if (slipEnabled == 0) {
        c0 = 1.;
        ud = 0.;
    }
}

}  // namespace

void instantaneous(const ClosureState &state, int cellIndex, double &c0, double &ud) {
    state.cells[cellIndex].transic0 = state.cells[cellIndex].transic;
    c0 = 1.;
    ud = 0.;
    if (state.cells[cellIndex - 1].velPig > 0 && state.cells[cellIndex - 1].estadoPig == 1) {
        applyPigOverride(state, cellIndex, c0, ud);
    } else if (state.cells[cellIndex].velPig < 0 && state.cells[cellIndex].estadoPig == 1) {
        applyPigOverride(state, cellIndex, c0, ud);
    } else if ((state.cells[cellIndex].acsr.tipo != kAccessoryPump || state.cells[cellIndex].acsr.bcs.freqnova <= 1.)) {
        const double liquidHoldup = transientLiquidHoldup<InstantaneousSource>(state, cellIndex);
        const double voidFraction = 1 - liquidHoldup;

        double betI = state.cells[cellIndex].betL;
        if (cellIndex > 0)
            betI = state.cells[cellIndex - 1].betPigD;
        if (((0. * state.cells[cellIndex].QG + 1 * state.cells[cellIndex].QL) < 0.))
            betI = state.cells[cellIndex].betPigE;

        const MeanConditions conditions = instantaneousMeanConditions(state, cellIndex);
        const double meanPressure = conditions.pressure;
        const double meanTemperature = conditions.temperature;

        const double horizontalDriftSign = horizontalDriftSignOf(state, cellIndex, cellIndex - 1);

        double surfaceTension;
        const PhaseProperties phases = instantaneousPhaseProperties(
            state, cellIndex, betI, meanPressure, meanTemperature, liquidHoldup, surfaceTension);
        const double liquidDensity = phases.liquidDensity;
        const double gasDensity = phases.gasDensity;
        const double liquidViscosity = phases.liquidViscosity;
        const double gasViscosity = phases.gasViscosity;

        auto [gasFlowRate, liquidFlowRate, diameter, flowArea, mixtureReynolds, liquidReynolds] =
            flowScalesOf<InstantaneousSource>(state, cellIndex,
                                {.liquidDensity = liquidDensity,
                                 .gasDensity = gasDensity,
                                 .liquidViscosity = liquidViscosity,
                                 .gasViscosity = gasViscosity,
                                 .liquidHoldup = liquidHoldup});

        int flowPattern = 1;
        const double inclinationAngle = transientInclinationAngle<InstantaneousSource>(state, cellIndex);

        double transitionWindow = 20;

        const MixtureProperties mix{
            .liquidDensity = liquidDensity,
            .gasDensity = gasDensity,
            .surfaceTension = surfaceTension,
            .voidFraction = voidFraction,
            .gasFlowRate = gasFlowRate,
            .liquidFlowRate = liquidFlowRate,
            .diameter = diameter,
            .flowArea = flowArea,
            .mixtureReynolds = mixtureReynolds,
            .liquidReynolds = liquidReynolds,
            .inclinationAngle = inclinationAngle,
            .horizontalDriftSign = horizontalDriftSign,
        };
        if (mixtureReynolds > 1e-30) {
            if (fabs(0 * inclinationAngle + 1 * state.cells[cellIndex].duto.teta) < 45. * std::numbers::pi / 180. && liquidHoldup < 0.99 && liquidHoldup > 0.01 && cellIndex < state.lastCell - 1) {
                gasFlowRate = (state.cells[cellIndex].MC - state.cells[cellIndex].Mliqini) / gasDensity;
                liquidFlowRate = state.cells[cellIndex].Mliqini / liquidDensity;

                estratificado stratifiedMap(diameter, liquidFlowRate, gasFlowRate, liquidDensity, gasDensity, liquidViscosity / pow(10., 3.), gasViscosity / pow(10., 3.), liquidHoldup,
                                        state.cells[cellIndex].duto.teta, state.cells[cellIndex].duto.rug / diameter);

                if (state.selectors.stratified == 2)
                    stratifiedMap.mapaTD();
                else
                    stratifiedMap.mapaTD(1);

                flowPattern = stratifiedMap.arr;

                if (flowPattern == -1) {
                    if (state.cells[cellIndex].arranjo != 0) {
                        if (((state.cells[cellIndex].arranjo != flowPattern) || state.cells[cellIndex].transic > 0)) {
                            if ((state.cells[cellIndex].arranjo != flowPattern) && state.cells[cellIndex].transic > 0)
                                state.cells[cellIndex].transic = 0;
                            state.cells[cellIndex].transic++;
                            if (state.cells[cellIndex].transic > transitionWindow - 1)
                                state.cells[cellIndex].transic = 0;
                        } else
                            state.cells[cellIndex].transic = 0;
                    }
                    state.cells[cellIndex].arranjo = flowPattern = stratifiedMap.arr;
                    state.cells[cellIndex - 1].arranjoR = stratifiedMap.arr;
                    state.cells[cellIndex - 1].perdaEstratL = stratifiedMap.fatorperdaLiq;
                    state.cells[cellIndex - 1].perdaEstratG = stratifiedMap.fatorperdaGas;
                    FlowPatternPair pair;
                    evaluateFlowPatternPair(state, cellIndex, mix, pair);

                    blendBySuperficialVelocity(mix, pair, c0, ud);

                    if (state.cells[cellIndex].transic > 0) {
                        c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                        ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    }
                }
            }
            if (flowPattern == 1) {

                arranjo flowPatternMap(diameter, liquidFlowRate / flowArea, gasFlowRate / flowArea, liquidDensity, gasDensity, liquidViscosity / pow(10., 3.), gasViscosity / pow(10., 3.), liquidHoldup,
                                   state.cells[cellIndex].duto.teta, surfaceTension, state.input.mapaArranjo, state.globals);
                flowPattern = flowPatternMap.verificaArr();

                evaluateDispersedOrAnnular(state, cellIndex, mix, flowPattern, c0, ud);
                if (fabs(gasFlowRate / state.cells[cellIndex].duto.area) > 5. && voidFraction >= 0.75) {
                    transitionWindow = 20;
                    if (state.selectors.annularChurn == 3 && state.selectors.dispersed == 1)
                        transitionWindow = 200;
                }

                if (state.cells[cellIndex].arranjo != 0) {
                    if ((flowPattern != state.cells[cellIndex].arranjo || state.cells[cellIndex].transic > 0)) {
                        if (flowPattern != state.cells[cellIndex].arranjo && state.cells[cellIndex].transic > 0)
                            state.cells[cellIndex].transic = 0;
                        state.cells[cellIndex].transic++;
                        if (state.cells[cellIndex].transic > transitionWindow - 1)
                            state.cells[cellIndex].transic = 0;
                    } else
                        state.cells[cellIndex].transic = 0;
                }
                if (state.cells[cellIndex].transic > 0) {
                    c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                }
                state.cells[cellIndex].arranjo = flowPattern;
                state.cells[cellIndex - 1].arranjoR = flowPattern;
            }
            state.cells[cellIndex].c0Spare = c0;
            state.cells[cellIndex].udSpare = ud;
        }
    }
    applyNoSlipOverride(state.input.escorregaTran, c0, ud);
}

void buffered(const ClosureState &state, int cellIndex, double &c0, double &ud) {
    c0 = 1.;
    ud = 0.;
    if (state.cells[cellIndex - 1].velPig > 0 && state.cells[cellIndex - 1].estadoPig == 1) {
        applyPigOverride(state, cellIndex, c0, ud);
    } else if (state.cells[cellIndex].velPig < 0 && state.cells[cellIndex].estadoPig == 1) {
        applyPigOverride(state, cellIndex, c0, ud);
    } else if ((state.cells[cellIndex].acsr.tipo != kAccessoryPump || state.cells[cellIndex].acsr.bcs.freqnova <= 1.)) {
        const double lengthRatio = state.cells[cellIndex].dxL / (state.cells[cellIndex].dx + state.cells[cellIndex].dxL);
        const double liquidHoldup = transientLiquidHoldup<BufferedSource>(state, cellIndex);
        const double voidFraction = 1 - liquidHoldup;

        const double betI = state.cells[cellIndex].betPigE; // beta doubt

        double meanPressure;

        meanPressure = lengthRatio * state.cells[cellIndex].presBuf + (1 - lengthRatio) * state.cells[cellIndex].presLBuf;
        if (cellIndex == state.lastCell)
            meanPressure = state.cells[cellIndex].presBuf;
        double meanTemperature = state.cells[cellIndex].tempL;
        if (state.cells[cellIndex].VTemper < 0.) {
            if (cellIndex < state.lastCell)
                meanTemperature = state.cells[cellIndex].temp;
            else
                meanTemperature = state.gasSurfaceTemperature;
        }

        const double horizontalDriftSign = horizontalDriftSignOf(state, cellIndex, cellIndex);

        double liquidDensity;
        double liquidViscosity;
        double surfaceTension;
        if ((state.cells[cellIndex].MliqiniBuf) < 0.) {
            liquidDensity = (1 - betI) * state.cells[cellIndex].flui.MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);
            liquidViscosity = (1 - betI) * state.cells[cellIndex].flui.ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * state.cells[cellIndex].flui.TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);
        } else {
            liquidDensity = (1 - betI) * state.cells[cellIndex - 1].flui.MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex - 1].fluicol.MasEspFlu(meanPressure, meanTemperature);
            liquidViscosity = (1 - betI) * state.cells[cellIndex - 1].flui.ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex - 1].fluicol.VisFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * state.cells[cellIndex - 1].flui.TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex - 1].fluicol.TensSuper(meanPressure, meanTemperature);
        }

        double gasDensity;
        double gasViscosity;
        if ((state.cells[cellIndex].MCBuf - state.cells[cellIndex].MliqiniBuf) < 0.) {
            gasDensity = state.cells[cellIndex].flui.MasEspGas(meanPressure, meanTemperature);
            gasViscosity = state.cells[cellIndex].flui.ViscGas(meanPressure, meanTemperature);
        } else {
            gasDensity = state.cells[cellIndex - 1].flui.MasEspGas(meanPressure, meanTemperature);
            gasViscosity = state.cells[cellIndex - 1].flui.ViscGas(meanPressure, meanTemperature);
        }

        auto [gasFlowRate, liquidFlowRate, diameter, flowArea, mixtureReynolds, liquidReynolds] =
            flowScalesOf<BufferedSource>(state, cellIndex,
                                {.liquidDensity = liquidDensity,
                                 .gasDensity = gasDensity,
                                 .liquidViscosity = liquidViscosity,
                                 .gasViscosity = gasViscosity,
                                 .liquidHoldup = liquidHoldup});

        int flowPattern = 1;
        const double inclinationAngle = transientInclinationAngle<BufferedSource>(state, cellIndex);
        double transitionWindow = 20;
        const MixtureProperties mix{
            .liquidDensity = liquidDensity,
            .gasDensity = gasDensity,
            .surfaceTension = surfaceTension,
            .voidFraction = voidFraction,
            .gasFlowRate = gasFlowRate,
            .liquidFlowRate = liquidFlowRate,
            .diameter = diameter,
            .flowArea = flowArea,
            .mixtureReynolds = mixtureReynolds,
            .liquidReynolds = liquidReynolds,
            .inclinationAngle = inclinationAngle,
            .horizontalDriftSign = horizontalDriftSign,
        };
        if (mixtureReynolds > 1e-30) {
            if (fabs(0 * inclinationAngle + 1 * state.cells[cellIndex].duto.teta) < 45. * std::numbers::pi / 180. && liquidHoldup < 0.99 && liquidHoldup > 0.01 && cellIndex < state.lastCell - 1) {
                gasFlowRate = (state.cells[cellIndex].MCBuf - state.cells[cellIndex].MliqiniBuf) / gasDensity;
                liquidFlowRate = (state.cells[cellIndex].MliqiniBuf) / liquidDensity;

                flowPattern = state.cells[cellIndex].arranjo;
                if (flowPattern == -1) {

                    FlowPatternPair pair;
                    evaluateFlowPatternPair(state, cellIndex, mix, pair);

                    blendBySuperficialVelocity(mix, pair, c0, ud);

                    if (state.cells[cellIndex].transic > 0) {
                        c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                        ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    }
                }
            }
            if (flowPattern != -1) {

                evaluateDispersedOrAnnular(state, cellIndex, mix, flowPattern, c0, ud);
                if (fabs(gasFlowRate / state.cells[cellIndex].duto.area) > 5. && voidFraction >= 0.75) {
                    transitionWindow = 20;
                    if (state.selectors.annularChurn == 3 && state.selectors.dispersed == 1)
                        transitionWindow = 200;
                }

                if (state.cells[cellIndex].transic > 0) {
                    c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                }
                state.cells[cellIndex].arranjo = flowPattern;
                state.cells[cellIndex - 1].arranjoR = flowPattern;
            }
            state.cells[cellIndex].c0Spare = c0;
            state.cells[cellIndex].udSpare = ud;
        }
    }
    applyNoSlipOverride(state.input.escorregaTran, c0, ud);
}

void initialization(const ClosureState &state, int cellIndex, double &c0, double &ud) {
    c0 = 1.;
    ud = 0.;
    if (state.cells[cellIndex].velPig < 0 && state.cells[cellIndex].estadoPig == 1) {
        c0 = 1.;
        ud = 0.;
        state.cells[cellIndex].arranjo = 1.;

    } else if ((state.cells[cellIndex].acsr.tipo != kAccessoryPump || state.cells[cellIndex].acsr.bcs.freqnova <= 1.)) {
        const double liquidHoldup = inletLiquidHoldup<InstantaneousSource>(state, cellIndex);
        const double voidFraction = 1 - liquidHoldup;

        const double betI = state.cells[cellIndex].betPigE; // beta doubt

        const double meanPressure = cellIndex == state.lastCell ? state.cells[cellIndex].pres : state.inletPressure;
        const double meanTemperature = state.inletTemperature;

        const double horizontalDriftSign = horizontalDriftSignOf(state, cellIndex, cellIndex);

        double liquidDensity;
        double liquidViscosity;
        double surfaceTension;
        if (state.cells[cellIndex].QL < 0.) {
            liquidDensity = (1 - betI) * state.cells[cellIndex].flui.MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);
            liquidViscosity = (1 - betI) * state.cells[cellIndex].flui.ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * state.cells[cellIndex].flui.TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);
        } else {
            liquidDensity = (1 - betI) * (*state.cells[cellIndex].fluiL).MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);
            liquidViscosity = (1 - betI) * (*state.cells[cellIndex].fluiL).ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * (*state.cells[cellIndex].fluiL).TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);
        }

        double gasDensity;
        double gasViscosity;
        if (state.cells[cellIndex].QG < 0.) {
            gasDensity = state.cells[cellIndex].flui.MasEspGas(meanPressure, meanTemperature);
            gasViscosity = state.cells[cellIndex].flui.ViscGas(meanPressure, meanTemperature);
        } else {
            gasDensity = (*state.cells[cellIndex].fluiL).MasEspGas(meanPressure, meanTemperature);
            gasViscosity = (*state.cells[cellIndex].fluiL).ViscGas(meanPressure, meanTemperature);
        }

        auto [gasFlowRate, liquidFlowRate, diameter, flowArea, mixtureReynolds, liquidReynolds] =
            flowScalesOf<InstantaneousSource>(state, cellIndex,
                                {.liquidDensity = liquidDensity,
                                 .gasDensity = gasDensity,
                                 .liquidViscosity = liquidViscosity,
                                 .gasViscosity = gasViscosity,
                                 .liquidHoldup = liquidHoldup});

        int flowPattern = 1;
        double totalLength = state.cells[cellIndex].dxL + state.cells[cellIndex].dx;
        double leftCellLength = state.cells[cellIndex].dxL;
        double cellLength = state.cells[cellIndex].dx;
        double inclinationAngle = (cellLength * state.cells[cellIndex].dutoL.teta + leftCellLength * state.cells[cellIndex].duto.teta) / totalLength;
        double transitionWindow = 20.;
        const MixtureProperties mix{
            .liquidDensity = liquidDensity,
            .gasDensity = gasDensity,
            .surfaceTension = surfaceTension,
            .voidFraction = voidFraction,
            .gasFlowRate = gasFlowRate,
            .liquidFlowRate = liquidFlowRate,
            .diameter = diameter,
            .flowArea = flowArea,
            .mixtureReynolds = mixtureReynolds,
            .liquidReynolds = liquidReynolds,
            .inclinationAngle = inclinationAngle,
            .horizontalDriftSign = horizontalDriftSign,
        };
        if (mixtureReynolds > 1e-30) {
            if (fabs(0 * inclinationAngle + 1 * state.cells[cellIndex].duto.teta) < 45. * std::numbers::pi / 180. && liquidHoldup < 0.99 && liquidHoldup > 0.01 && cellIndex < state.lastCell - 1) {
                gasFlowRate = (state.cells[cellIndex].MC - state.cells[cellIndex].Mliqini) / gasDensity;
                liquidFlowRate = state.cells[cellIndex].Mliqini / liquidDensity;

                estratificado stratifiedMap(diameter, liquidFlowRate, gasFlowRate, liquidDensity, gasDensity, liquidViscosity / pow(10., 3.), gasViscosity / pow(10., 3.), liquidHoldup,
                                        state.cells[cellIndex].duto.teta, state.cells[cellIndex].duto.rug / diameter);

                stratifiedMap.mapaTD();
                flowPattern = stratifiedMap.arr;
                if (flowPattern == -1) {
                    if (state.cells[cellIndex].arranjo != 0) {
                        if (((state.cells[cellIndex].arranjo != flowPattern) || state.cells[cellIndex].transic > 0)) {
                            if ((state.cells[cellIndex].arranjo != flowPattern) && state.cells[cellIndex].transic > 0)
                                state.cells[cellIndex].transic = 0;
                            state.cells[cellIndex].transic++;
                            if (state.cells[cellIndex].transic > 19)
                                state.cells[cellIndex].transic = 0;
                        }
                    } else
                        state.cells[cellIndex].transic = 0;
                    state.cells[cellIndex].arranjo = flowPattern = stratifiedMap.arr;
                    FlowPatternPair pair;
                    evaluateFlowPatternPair(state, cellIndex, mix, pair);

                    blendBySuperficialVelocity(mix, pair, c0, ud);

                    if (state.cells[cellIndex].transic > 0) {
                        c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                        ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    }
                }
            }
            if (flowPattern == 1) {

                arranjo flowPatternMap(diameter, liquidFlowRate / flowArea, gasFlowRate / flowArea, liquidDensity, gasDensity, liquidViscosity / pow(10., 3.), gasViscosity / pow(10., 3.), liquidHoldup,
                                   state.cells[cellIndex].duto.teta, surfaceTension, state.input.mapaArranjo, state.globals);
                flowPattern = flowPatternMap.verificaArr();

                evaluateDispersedOrAnnular(state, cellIndex, mix, flowPattern, c0, ud);

                if (fabs(gasFlowRate / state.cells[cellIndex].duto.area) > 5. && voidFraction >= 0.75) {
                    transitionWindow = 20;
                    if (state.selectors.annularChurn == 3 && state.selectors.dispersed == 1)
                        transitionWindow = 200;
                }
                if (state.cells[cellIndex].arranjo != 0) {
                    if ((flowPattern != state.cells[cellIndex].arranjo || state.cells[cellIndex].transic > 0)) {
                        if (flowPattern != state.cells[cellIndex].arranjo && state.cells[cellIndex].transic > 0)
                            state.cells[cellIndex].transic = 0;
                        state.cells[cellIndex].transic++;
                        if (state.cells[cellIndex].transic > transitionWindow - 1)
                            state.cells[cellIndex].transic = 0;
                    }
                } else
                    state.cells[cellIndex].transic = 0;
                if (state.cells[cellIndex].transic > 0) {
                    c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                }
                state.cells[cellIndex].arranjo = flowPattern;
            }
            state.cells[cellIndex].c0Spare = c0;
            state.cells[cellIndex].udSpare = ud;
        }
    }
    applyNoSlipOverride(state.input.escorregaTran, c0, ud);
}

void bufferedInitialization(const ClosureState &state, int cellIndex, double &c0, double &ud) {
    c0 = 1.;
    ud = 0.;
    if (state.cells[cellIndex].velPig < 0 && state.cells[cellIndex].estadoPig == 1) {
        c0 = 1.;
        ud = 0.;
        state.cells[cellIndex].arranjo = 1.;

    } else if ((state.cells[cellIndex].acsr.tipo != kAccessoryPump || state.cells[cellIndex].acsr.bcs.freqnova <= 1.)) {
        const double liquidHoldup = inletLiquidHoldup<BufferedSource>(state, cellIndex);
        const double voidFraction = 1 - liquidHoldup;

        const double betI = state.cells[cellIndex].betPigE; // beta doubt

        const double meanPressure = state.inletPressure;
        const double meanTemperature = state.inletTemperature;

        const double horizontalDriftSign = horizontalDriftSignOf(state, cellIndex, cellIndex);

        double liquidDensity;
        double liquidViscosity;
        double surfaceTension;
        if (state.cells[cellIndex].MliqiniBuf < 0.) {
            liquidDensity = (1 - betI) * state.cells[cellIndex].flui.MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);
            liquidViscosity = (1 - betI) * state.cells[cellIndex].flui.ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * state.cells[cellIndex].flui.TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);
        } else {
            liquidDensity = (1 - betI) * (*state.cells[cellIndex].fluiL).MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);
            liquidViscosity = (1 - betI) * (*state.cells[cellIndex].fluiL).ViscOleo(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(meanPressure, meanTemperature);
            surfaceTension = (1 - betI) * (*state.cells[cellIndex].fluiL).TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);
        }

        double gasDensity;
        double gasViscosity;
        if ((state.cells[cellIndex].MCBuf - state.cells[cellIndex].MliqiniBuf) < 0.) {
            gasDensity = state.cells[cellIndex].flui.MasEspGas(meanPressure, meanTemperature);
            gasViscosity = state.cells[cellIndex].flui.ViscGas(meanPressure, meanTemperature);
        } else {
            gasDensity = (*state.cells[cellIndex].fluiL).MasEspGas(meanPressure, meanTemperature);
            gasViscosity = (*state.cells[cellIndex].fluiL).ViscGas(meanPressure, meanTemperature);
        }

        auto [gasFlowRate, liquidFlowRate, diameter, flowArea, mixtureReynolds, liquidReynolds] =
            flowScalesOf<BufferedSource>(state, cellIndex,
                                {.liquidDensity = liquidDensity,
                                 .gasDensity = gasDensity,
                                 .liquidViscosity = liquidViscosity,
                                 .gasViscosity = gasViscosity,
                                 .liquidHoldup = liquidHoldup});

        int flowPattern = 1;
        double totalLength = state.cells[cellIndex].dxL + state.cells[cellIndex].dx;
        double leftCellLength = state.cells[cellIndex].dxL;
        double cellLength = state.cells[cellIndex].dx;
        double inclinationAngle = (cellLength * state.cells[cellIndex].dutoL.teta + leftCellLength * state.cells[cellIndex].duto.teta) / totalLength;
        double transitionWindow = 20.;
        const MixtureProperties mix{
            .liquidDensity = liquidDensity,
            .gasDensity = gasDensity,
            .surfaceTension = surfaceTension,
            .voidFraction = voidFraction,
            .gasFlowRate = gasFlowRate,
            .liquidFlowRate = liquidFlowRate,
            .diameter = diameter,
            .flowArea = flowArea,
            .mixtureReynolds = mixtureReynolds,
            .liquidReynolds = liquidReynolds,
            .inclinationAngle = inclinationAngle,
            .horizontalDriftSign = horizontalDriftSign,
        };
        if (mixtureReynolds > 1e-30) {
            if (fabs(0 * inclinationAngle + 1 * state.cells[cellIndex].duto.teta) < 45. * std::numbers::pi / 180. && liquidHoldup < 0.99 && liquidHoldup > 0.01 && cellIndex < state.lastCell - 1) {
                gasFlowRate = (state.cells[cellIndex].MCBuf - state.cells[cellIndex].MliqiniBuf) / gasDensity;
                liquidFlowRate = state.cells[cellIndex].MliqiniBuf / liquidDensity;

                flowPattern = state.cells[cellIndex].arranjo;
                if (flowPattern == -1) {

                    FlowPatternPair pair;
                    evaluateFlowPatternPair(state, cellIndex, mix, pair);

                    blendBySuperficialVelocity(mix, pair, c0, ud);

                    if (state.cells[cellIndex].transic > 0) {
                        c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                        ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    }
                }
            }
            if (flowPattern != -1) {

                evaluateDispersedOrAnnular(state, cellIndex, mix, flowPattern, c0, ud);
                if (fabs(gasFlowRate / state.cells[cellIndex].duto.area) > 5. && voidFraction >= 0.75) {
                    transitionWindow = 20;
                    if (state.selectors.annularChurn == 3 && state.selectors.dispersed == 1)
                        transitionWindow = 200;
                }

                if (state.cells[cellIndex].transic > 0) {
                    c0 = (c0 * state.cells[cellIndex].transic + state.cells[cellIndex].c0 * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                    ud = (ud * state.cells[cellIndex].transic + state.cells[cellIndex].ud * (transitionWindow - state.cells[cellIndex].transic)) / transitionWindow;
                }
                state.cells[cellIndex].arranjo = flowPattern;
            }
            state.cells[cellIndex].c0Spare = c0;
            state.cells[cellIndex].udSpare = ud;
        }
    }
    applyNoSlipOverride(state.input.escorregaTran, c0, ud);
}

void steadyState(const ClosureState &state, int cellIndex, double &c0, double &ud) {

    c0 = 1.;
    ud = 0.;
    if (state.cells[cellIndex].acsr.tipo != kAccessoryPump || state.cells[cellIndex].acsr.bcs.freqnova <= 1.) {
        double lengthRatio = state.cells[cellIndex].dx / (state.cells[cellIndex].dx + state.cells[cellIndex].dxL);
        double upstreamLengthRatio;
        if (cellIndex > 0)
            upstreamLengthRatio = state.cells[cellIndex - 1].dx / (state.cells[cellIndex - 1].dx + state.cells[cellIndex - 1].dxL);
        else
            upstreamLengthRatio = lengthRatio;
        const double liquidHoldup = 1. - state.cells[cellIndex].alf;
        const double voidFraction = 1 - liquidHoldup;

        const double betI = state.cells[cellIndex].betL;

        double meanPressure;
        double upstreamMeanPressure = 0.;

        meanPressure = state.cells[cellIndex].presaux;
        if (cellIndex > 0)
            upstreamMeanPressure = state.cells[cellIndex - 1].presaux;
        else
            upstreamMeanPressure = state.cells[cellIndex].presaux;
        double meanTemperature;
        if (state.steadyIteration != 0 && state.input.AceleraConvergPerm == 0)
            meanTemperature = lengthRatio * state.cells[cellIndex].temp + (1 - lengthRatio) * state.cells[cellIndex].tempL;
        else
            meanTemperature = state.cells[cellIndex - 1].temp;
        double upstreamMeanTemperature;
        if (cellIndex > 0 && state.input.AceleraConvergPerm == 0)
            upstreamMeanTemperature = upstreamLengthRatio * state.cells[cellIndex - 1].temp + (1 - upstreamLengthRatio) * state.cells[cellIndex - 1].tempL;
        else
            upstreamMeanTemperature = meanTemperature;

        double horizontalDriftSign = 1.;
        if (fabs(state.cells[cellIndex].duto.teta) < 1e-10) {
            if (state.cells[cellIndex].angEsq < 0 && state.cells[cellIndex].angDir < 0)
                horizontalDriftSign = -1.;
            else if (state.cells[cellIndex].angEsq > 0 && state.cells[cellIndex].angDir > 0)
                horizontalDriftSign = 1.;
        }

        double liquidDensity;
        double liquidViscosity;
        double surfaceTension;
        if (cellIndex > 0)
            liquidDensity = (1 - betI) * state.cells[cellIndex - 1].flui.MasEspLiq(upstreamMeanPressure, upstreamMeanTemperature) + betI * state.cells[cellIndex - 1].fluicol.MasEspFlu(upstreamMeanPressure, upstreamMeanTemperature);
        else
            liquidDensity = (1 - betI) * (*state.cells[cellIndex].fluiL).MasEspLiq(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.MasEspFlu(meanPressure, meanTemperature);

        liquidViscosity = (1 - betI) * (*state.cells[cellIndex].fluiL).ViscOleo(upstreamMeanPressure, upstreamMeanTemperature) + betI * state.cells[cellIndex].fluicol.VisFlu(upstreamMeanPressure, upstreamMeanTemperature);
        surfaceTension = (1 - betI) * (*state.cells[cellIndex].fluiL).TensSuper(meanPressure, meanTemperature) + betI * state.cells[cellIndex].fluicol.TensSuper(meanPressure, meanTemperature);

        double gasDensity;
        double gasViscosity;
        if (cellIndex > 0)
            gasDensity = state.cells[cellIndex - 1].flui.MasEspGas(upstreamMeanPressure, upstreamMeanTemperature);
        else
            gasDensity = (*state.cells[cellIndex].fluiL).MasEspGas(meanPressure, meanTemperature);
        gasViscosity = (*state.cells[cellIndex].fluiL).ViscGas(meanPressure, meanTemperature);

        auto [gasFlowRate, liquidFlowRate, diameter, flowArea, mixtureReynolds, liquidReynolds] =
            flowScalesOf<SteadyStateSource>(state, cellIndex,
                                {.liquidDensity = liquidDensity,
                                 .gasDensity = gasDensity,
                                 .liquidViscosity = liquidViscosity,
                                 .gasViscosity = gasViscosity,
                                 .liquidHoldup = liquidHoldup});

        int flowPattern = 1;
        double totalLength = state.cells[cellIndex].dxL + state.cells[cellIndex].dx;
        double leftCellLength = state.cells[cellIndex].dxL;
        double cellLength = state.cells[cellIndex].dx;
        double inclinationAngle = (leftCellLength * state.cells[cellIndex].dutoL.teta + cellLength * state.cells[cellIndex].duto.teta) / totalLength;
        double inclinationSign = 1.;
        if (fabs(state.cells[cellIndex].MC) > 1e-15)
            inclinationSign = state.cells[cellIndex].MC / fabs(state.cells[cellIndex].MC);
        if (gasDensity < 0.9 * liquidDensity) {
        const MixtureProperties mix{
            .liquidDensity = liquidDensity,
            .gasDensity = gasDensity,
            .surfaceTension = surfaceTension,
            .voidFraction = voidFraction,
            .gasFlowRate = gasFlowRate,
            .liquidFlowRate = liquidFlowRate,
            .diameter = diameter,
            .flowArea = flowArea,
            .mixtureReynolds = mixtureReynolds,
            .liquidReynolds = liquidReynolds,
            .inclinationAngle = inclinationAngle,
            .horizontalDriftSign = horizontalDriftSign,
        };
            if (mixtureReynolds > 1e-30) {
                if (fabs(0 * inclinationAngle + inclinationSign * state.cells[cellIndex].duto.teta) < 45. * std::numbers::pi / 180. && liquidHoldup < 0.99 && liquidHoldup > 0.01 && cellIndex < state.lastCell - 1) {
                    gasFlowRate = fabs(state.cells[cellIndex].MC - state.cells[cellIndex].Mliqini) / gasDensity;
                    liquidFlowRate = fabs(state.cells[cellIndex].Mliqini) / liquidDensity;

                    estratificado stratifiedMap(diameter, liquidFlowRate, gasFlowRate, liquidDensity, gasDensity, liquidViscosity / pow(10., 3.), gasViscosity / pow(10., 3.), liquidHoldup,
                                            inclinationSign * state.cells[cellIndex].duto.teta, state.cells[cellIndex].duto.rug / diameter);

                    stratifiedMap.mapaTD();
                    flowPattern = stratifiedMap.arr;
                    if (flowPattern == -1) {
                        if (((state.cells[cellIndex].arranjo != flowPattern) || state.cells[cellIndex].transic > 0)) {
                            if ((state.cells[cellIndex].arranjo != flowPattern) && state.cells[cellIndex].transic > 0)
                                state.cells[cellIndex].transic = 0;
                            state.cells[cellIndex].transic++;
                            if (state.cells[cellIndex].transic > 19)
                                state.cells[cellIndex].transic = 0;
                        }
                        state.cells[cellIndex].arranjo = flowPattern = stratifiedMap.arr;
                        if (cellIndex > 0) {
                            state.cells[cellIndex - 1].arranjoR = stratifiedMap.arr;
                            state.cells[cellIndex - 1].perdaEstratL = stratifiedMap.fatorperdaLiq;
                            state.cells[cellIndex - 1].perdaEstratG = stratifiedMap.fatorperdaGas;
                        }

                        FlowPatternPair pair;
                        evaluateFlowPatternPair(state, cellIndex, mix, pair);

                        blendBySuperficialVelocity(mix, pair, c0, ud);
                    }
                }
                if (flowPattern == 1) {

                    arranjo flowPatternMap(diameter, liquidFlowRate / flowArea, gasFlowRate / flowArea, liquidDensity, gasDensity, liquidViscosity / pow(10., 3.), gasViscosity / pow(10., 3.), liquidHoldup,
                                       inclinationSign * state.cells[cellIndex].duto.teta, surfaceTension, state.input.mapaArranjo, state.globals);
                    flowPattern = flowPatternMap.verificaArr();

                    evaluateDispersedOrAnnular(state, cellIndex, mix, flowPattern, c0, ud);
                    state.cells[cellIndex].arranjo = flowPattern;
                    if (cellIndex > 0)
                        state.cells[cellIndex - 1].arranjoR = flowPattern;
                }
                state.cells[cellIndex].c0Spare = c0;
                state.cells[cellIndex].udSpare = ud;
            }
        } else {
            c0 = 1.;
            ud = 0.;
            state.cells[cellIndex].arranjo = 1;
            if (cellIndex > 0)
                state.cells[cellIndex - 1].arranjoR = 1;
            state.cells[cellIndex].c0Spare = c0;
            state.cells[cellIndex].udSpare = ud;
        }
    }
    applyNoSlipOverride(state.input.escorregaPerm, c0, ud);
}

}  // namespace coefficient
}  // namespace driftflux
