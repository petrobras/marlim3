// Petroleo Brasileiro S.A.
#ifndef FRICTIONFACTOR_H_
#define FRICTIONFACTOR_H_

#include <cmath>

/*
==============================================================================
frictionFactor: Calculates the single-phase Moody friction factor using the
                Colebrook-White equation, with treatment for laminar and
                turbulent flow regimes.

References:
     1.  Moody, L. F.: "Friction Factors for Pipe Flow," Transactions of the ASME,
         Vol. 66, No. 8 (1944) 671-684.
     2.  Colebrook, C. F.: "Turbulent Flow in Pipes, with Particular Reference to
         the Transition Region Between the Smooth and Rough Pipe Laws," Journal of
         the Institution of Civil Engineers, Vol. 11, No. 4 (1939) 133-156.

Method:
- Laminar regime (Re <= 2100): f = 64/Re (Hagen-Poiseuille equation)
- Turbulent regime (Re > 2100): Explicit Serghides approximation for the
  implicit Colebrook-White equation

Variable dictionary:
- reynolds           : Reynolds number (dimensionless) - input
- roughness          : Relative roughness (epsilon/D, dimensionless) - input
- frictionFactorValue: Moody-Darcy friction factor (dimensionless) - output
- relRoughness       : Normalized roughness (roughness/3.7)
- invSqrtF           : Approximation to the inverse square root of the factor
- termA, termB, termC: Auxiliary terms used in the Serghides approximation
==============================================================================
*/

/*!
 * Calculate the Darcy friction factor using a Moody-diagram formulation.
 *
 * \param reynolds Reynolds number.
 * \param roughness Relative roughness.
 * \param frictionFactorValue Calculated friction-factor value.
 */
inline void frictionFactor(double reynolds, double roughness, double& frictionFactorValue) {

    // Flow Regime Constants
    static constexpr double LAMINAR_TRANSITION_REYNOLDS = 2100.0;  // Laminar-turbulent transition
    static constexpr double LAMINAR_FRICTION_COEFFICIENT = 64.0;   // Hagen-Poiseuille coefficient

    // Colebrook-White / Serghides constants
    static constexpr double COLEBROOK_ROUGHNESS_FACTOR = 3.7;      // eps/D normalization factor
    static constexpr double SERGHIDES_A_COEFFICIENT = 12.0;        // Reynolds term, first estimate
    static constexpr double SERGHIDES_BC_COEFFICIENT = 2.51;       // Colebrook Reynolds constant

    // Laminar Flow Regime (Re <= 2100): f = 64/Re (Hagen-Poiseuille)
    if (reynolds <= LAMINAR_TRANSITION_REYNOLDS) {
        frictionFactorValue = LAMINAR_FRICTION_COEFFICIENT / reynolds;
        return;
    }

    // Turbulent Flow Regime (Re > 2100)
    // Serghides' explicit three-point approximation to the Colebrook-White
    // implicit equation  1/sqrt(f) = -2 log10(eps/D/3.7 + 2.51/(Re*sqrt(f))):
    //     A = -2 log10(eps/D/3.7 + 12/Re)
    //     B = -2 log10(eps/D/3.7 + 2.51*A/Re)
    //     C = -2 log10(eps/D/3.7 + 2.51*B/Re)
    //     1/sqrt(f) = A - (B - A)^2 / (C - 2B + A)
    // Ref: Serghides, T.K. (1984), "Estimate friction factor accurately,"
    //   Chemical Engineering 91(5):63-64.
    // Explicit (no iteration); returns the Moody-Darcy friction factor.
    const double relRoughness = roughness / COLEBROOK_ROUGHNESS_FACTOR;
    const double termA = -2.0 * std::log10(relRoughness + SERGHIDES_A_COEFFICIENT / reynolds);
    const double termB = -2.0 * std::log10(relRoughness + SERGHIDES_BC_COEFFICIENT * termA / reynolds);
    const double termC = -2.0 * std::log10(relRoughness + SERGHIDES_BC_COEFFICIENT * termB / reynolds);
    const double invSqrtF = termA - (termB - termA) * (termB - termA) / (termC - 2.0 * termB + termA);
    frictionFactorValue = 1.0 / (invSqrtF * invSqrtF);
}

#endif  // FRICTIONFACTOR_H_
