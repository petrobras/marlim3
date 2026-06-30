// ============================================================================
// CorrelationFits.h - Analytical polynomial fits replacing digitized tables
//
// All fits are independent polynomial approximations derived from graphical
// data published in the following references:
//
//   [1] Brill, J.P. and Beggs, H.D.: "Two-Phase Flow in Pipes",
//       University of Tulsa, Feb. 1984.
//   [2] Poettmann, F.H. & Carpenter, P.G., API Drilling and Production
//       Practice (1952) 257-317.
//   [3] Baxendell, P.B. & Thomas, R., JPT (Oct. 1961) 1023-1028.
//   [4] Fancher, G.H. & Brown, K.E., SPEJ (March 1963) 59-69.
//   [5] Hagedorn, A.R. & Brown, K.E., JPT (April 1965) 475-484.
//   [6] Duns, H. & Ros, N.C.J., Proc. 6th World Petroleum Congress (1963).
//   [7] Orkiszewski, J., JPT (June 1967) 829-838.
//   [8] Eaton, B.A. et al., JPT (Oct. 1967) 815-828.
//   [9] Flanigan, O., Oil & Gas Journal (March 1958).
//  [10] Dukler, A.E. et al., AIChE J. 10(1) (1964) 44-51.
//  [11] Mukherjee, H. & Brill, J.P., JPT (May 1985) 1003-1008.
//
// Polynomial evaluations use Horner's method for numerical stability.
// Boundary values use constant extrapolation (clamped).
// (c) Petrobras S.A. - Open source under [LICENSE]
// ============================================================================
#pragma once

#include <cmath>

namespace CorrelationFits {

// ============================================================================
//    POETTMANN-CARPENTER [2]: log(friction factor) vs log(rho*v*d)
// ============================================================================
// Returns log(friction factor) given log(rho*v*d)
// Polynomial degree 6, max rel error: 4.39e-03, max abs error: 1.43e-02
inline double poettmannFriction(double x) {
    if (x <= 1.09861000000000009e+00)
        return 2.07943999999999996e+00;
    if (x >= 4.78749000000000002e+00)
        return -5.33914000000000044e+00;
    return ((((((1.65208798486174147e-02) * x + (-3.25036025550743291e-01)) * x + (2.59722151407582702e+00)) * x + (-1.08126559855598021e+01)) * x + (2.49638716305749995e+01)) * x + (-3.24998145822781339e+01)) * x + (1.86988920262817828e+01);
}

// ============================================================================
//    BAXENDELL-THOMAS [3]: log(friction factor) vs log(rho*v*d)
// ============================================================================
// Returns log(friction factor) given log(rho*v*d)
// Polynomial degree 7, max rel error: 4.67e-03, max abs error: 1.69e-02
inline double baxendellFriction(double x) {
    if (x <= 1.09861000000000009e+00)
        return 2.07943999999999996e+00;
    if (x >= 4.78749000000000002e+00)
        return -3.97389999999999999e+00;
    return (((((((2.41574649203832419e-02) * x + (-5.13562718841930499e-01)) * x + (4.47416411907003653e+00)) * x + (-2.05373406503674403e+01)) * x + (5.30944751631520830e+01)) * x + (-7.58695173498199864e+01)) * x + (5.13501781299410069e+01)) * x + (-9.55231573946761792e+00);
}

// ============================================================================
//    FANCHER-BROWN [4]: log(friction factor) vs log(rho*v*d) at different GLR
// ============================================================================
// GLR = 1500 SCF/STB
// Polynomial degree 8, max rel error: 4.43e-03, max abs error: 1.65e-02
inline double fancherFriction1500(double x) {
    if (x <= 1.09861000000000009e+00)
        return 4.69999999999999973e-01;
    if (x >= 4.78749000000000002e+00)
        return -4.60517000000000021e+00;
    return ((((((((1.15786036220708349e-01) * x + (-2.86745211554150581e+00)) * x + (3.03622777753017097e+01)) * x + (-1.79103392213295308e+02)) * x + (6.41963901742389794e+02)) * x + (-1.42717714198671138e+03)) * x + (1.91516952088315293e+03)) * x + (-1.41467467915465841e+03)) * x + (4.38907451396179738e+02);
}

// GLR = 2250 SCF/STB
// Polynomial degree 7, max rel error: 3.80e-03, max abs error: 1.53e-02
inline double fancherFriction2250(double x) {
    if (x <= 1.09861000000000009e+00)
        return -9.67579999999999996e-01;
    if (x >= 4.78749000000000002e+00)
        return -5.29832000000000036e+00;
    return (((((((-2.13137853332794891e-02) * x + (4.46709041132507811e-01)) * x + (-3.88076351844119660e+00)) * x + (1.80424138097538425e+01)) * x + (-4.82717761876063776e+01)) * x + (7.39986193556185583e+01)) * x + (-6.11282640666158557e+01)) * x + (2.00665900116670421e+01);
}

// GLR = 3000 SCF/STB
// Polynomial degree 9, max rel error: 4.31e-10, max abs error: 2.64e-09
inline double fancherFriction3000(double x) {
    if (x <= 1.09861000000000009e+00)
        return -1.71480000000000010e+00;
    if (x >= 4.78749000000000002e+00)
        return -6.11929999999999996e+00;
    return (((((((((5.34471834082678510e-01) * x + (-1.51703835120990522e+01)) * x + (1.87963214689538148e+02)) * x + (-1.33193419121867419e+03)) * x + (5.93626565424978344e+03)) * x + (-1.72138236652284140e+04)) * x + (3.23808847796981718e+04)) * x + (-3.79690383764604485e+04)) * x + (2.50783462991562883e+04)) * x + (-7.07721198586622995e+03);
}

// Linear interpolation between GLR curves
inline double fancherFrictionInterp(double logFlowParam, double glr) {
    if (glr <= 1500.0)
        return fancherFriction1500(logFlowParam);
    if (glr >= 3000.0)
        return fancherFriction3000(logFlowParam);
    double f1500 = fancherFriction1500(logFlowParam);
    double f2250 = fancherFriction2250(logFlowParam);
    double f3000 = fancherFriction3000(logFlowParam);
    if (glr <= 2250.0) {
        double t = (glr - 1500.0) / 750.0;
        return f1500 + t * (f2250 - f1500);
    } else {
        double t = (glr - 2250.0) / 750.0;
        return f2250 + t * (f3000 - f2250);
    }
}

// ============================================================================
//    HAGEDORN-BROWN [5]: Holdup and correction factors
// ============================================================================
// Base holdup vs log(dimensionless parameter H)
// Polynomial degree 11, max rel error: 2.59e-06, max abs error: 1.03e-07
inline double hagedornHoldup(double x) {
    if (x <= -1.31223630000000000e+01)
        return 4.00000000000000008e-02;
    if (x >= -4.60517000000000021e+00)
        return 1.00000000000000000e+00;
    return (((((((((((2.15625867347281413e-06) * x + (2.07537591922282011e-04)) * x + (8.97986027217116158e-03)) * x + (2.30484428880244036e-01)) * x + (3.89785497863357211e+00)) * x + (4.55908275499481022e+01)) * x + (3.76229377606499781e+02)) * x + (2.19004533256444847e+03)) * x + (8.81109325216121033e+03)) * x + (2.33314880506111913e+04)) * x + (3.65935606880937630e+04)) * x + (2.57542961917948269e+04);
}

// log(CNL viscosity correction) vs log(liquid viscosity number)
// Polynomial degree 5, max rel error: 2.83e-03, max abs error: 1.33e-02
inline double hagedornCNL(double x) {
    if (x <= -6.21460800000000013e+00)
        return -6.26590100000000039e+00;
    if (x >= -9.16290999999999967e-01)
        return -4.46540800000000004e+00;
    return (((((1.32983488959416874e-03) * x + (1.37249274880853738e-02)) * x + (3.89298538389172945e-03)) * x + (-2.79603282829537447e-01)) * x + (-2.94397614015148468e-01)) * x + (-4.50827341121471914e+00);
}

// PSI correction factor vs dimensionless group
// Polynomial degree 8, max rel error: 4.42e-03, max abs error: 7.07e-03
inline double hagedornPSI(double x) {
    if (x <= 1.00000000000000002e-02)
        return 1.00000000000000000e+00;
    if (x >= 8.99999999999999967e-02)
        return 1.83000000000000007e+00;
    return ((((((((-4.05071894750086792e+11) * x + (1.62990906996931580e+11)) * x + (-2.73611007300596771e+10)) * x + (2.47892129108125305e+09)) * x + (-1.30806226402397037e+08)) * x + (4.03866666605098778e+06)) * x + (-6.95733445184202574e+04)) * x + (6.16292915777828739e+02)) * x + (-1.15824515843067233e+00);
}

// ============================================================================
//    DUNS-ROS [6]: Bubble and Slug flow coefficients
//    F1, F2, F4 have constant-value regions at low viscosity numbers;
//    these use piecewise fits (constant + polynomial).
//    Bubble flow coeff F1 vs log(NL) [6]
//    Piecewise: constant for x <= -4.605170, then degree-11 polynomial
//    Max rel error in polynomial segment: 1.27e-08
inline double dunsRosF1(double x) {
    if (x <= -4.60517000000000021e+00)
        return 2.23143499999999995e-01;
    if (x >= 4.05465000000000020e-01)
        return -5.12933000000000000e-02;
    return (((((((((((-1.13593075314565826e-02) * x + (-2.86649128626714600e-01)) * x + (-3.12450802688379392e+00)) * x + (-1.92189838069128953e+01)) * x + (-7.30258412179811955e+01)) * x + (-1.75834920094656752e+02)) * x + (-2.62772917356859921e+02)) * x + (-2.22762538073217542e+02)) * x + (-7.57445747237909757e+01)) * x + (2.09314223639305332e+01)) * x + (2.00120589826370079e+01)) * x + (3.27082834298374570e+00);
}

// Bubble flow coeff F2 vs log(NL) [6]
// Piecewise: constant for x <= -4.199705, then degree-10 polynomial
// Max rel error in polynomial segment: 1.19e-09
inline double dunsRosF2(double x) {
    if (x <= -4.19970499999999980e+00)
        return -1.42711600000000005e+00;
    if (x >= 4.05465000000000020e-01)
        return -2.87681999999999993e-01;
    return ((((((((((-9.03215923879787351e-03) * x + (-1.87517277089296469e-01)) * x + (-1.64363093049912545e+00)) * x + (-7.88589180466469752e+00)) * x + (-2.23933288523770351e+01)) * x + (-3.77507977393594913e+01)) * x + (-3.51004895006296351e+01)) * x + (-1.31986851299078722e+01)) * x + (2.94365140979805773e+00)) * x + (3.13805078982186858e+00)) * x + (3.13191159183137557e-01);
}

// Bubble flow coeff F3 vs log(NL) [6]
// Polynomial degree 12, max rel error: 3.42e-02, max abs error: 6.98e-03
inline double dunsRosF3(double x) {
    if (x <= -5.80914299999999972e+00)
        return -1.86329999999999996e-01;
    if (x >= 4.05465000000000020e-01)
        return 1.41098699999999999e+00;
    return ((((((((((((1.68249913963469307e-05) * x + (5.87831909472457661e-04)) * x + (8.84872678030818732e-03)) * x + (7.51041882889903478e-02)) * x + (3.94537481625514552e-01)) * x + (1.32267471483750665e+00)) * x + (2.80267164262294433e+00)) * x + (3.54296228147979031e+00)) * x + (2.24056641025709613e+00)) * x + (1.85238273421764799e-01)) * x + (-4.65959259833950923e-01)) * x + (-7.46416206731042586e-02)) * x + (1.39097592154039895e+00);
}

// Bubble flow coeff F4 vs log(NL) [6]
// Two-piece: polynomial for x < -2.525729, constant 55.0 for x >= -2.525729
// Polynomial segment: degree 7, max abs error: 1.07e-08
inline double dunsRosF4(double x) {
    if (x <= -5.80914299999999972e+00)
        return -5.00000000000000000e+00;
    if (x >= -2.99573199999999984e+00)
        return 5.50000000000000000e+01;
    return (((((((-1.64255922519966724e+01) * x + (-5.08091156332995354e+02)) * x + (-6.68502420013167375e+03)) * x + (-4.84874298507204949e+04)) * x + (-2.09345363266149332e+05)) * x + (-5.37930407982715406e+05)) * x + (-7.61563123093672912e+05)) * x + (-4.58092357449975156e+05);
}

// Slug flow coeff F5 vs log(NL) [6]
// Two-piece polynomial: split at x = -1.897120
// Left piece: degree 8, max err: 1.24e-02
// Right piece: degree 3, max err: 3.46e-03
inline double dunsRosF5(double x) {
    if (x <= -5.80914299999999972e+00)
        return -1.52326000000000006e+00;
    if (x >= 4.05465000000000020e-01)
        return -2.25379500000000021e+00;
    if (x <= -1.89711999999999992e+00)
        return ((((((((7.86799789007605957e-03) * x + (2.54417571767320994e-01)) * x + (3.55700144420578956e+00)) * x + (2.80756888712694348e+01)) * x + (1.36761798529998913e+02)) * x + (4.20540878915131941e+02)) * x + (7.95471388292733081e+02)) * x + (8.42678306335644947e+02)) * x + (3.77773924251247934e+02);
    else
        return (((-1.02488802903618725e-01) * x + (-1.92218293366647169e-01)) * x + (3.88168764172915126e-01)) * x + (-2.37232124841677594e+00);
}

// Slug flow coeff F6 vs log(NL) [6]
// Two-piece polynomial: split at x = -2.995732
// Left piece: degree 7, max err: 4.27e-10
// Right piece: degree 7, max err: 3.46e-13
inline double dunsRosF6(double x) {
    if (x <= -5.80914299999999972e+00)
        return 4.79999999999999982e-01;
    if (x >= 4.05465000000000020e-01)
        return 1.73999999999999999e+00;
    if (x <= -2.99573199999999984e+00)
        return (((((((-3.48455557265343385e-01) * x + (-1.10342233277054156e+01)) * x + (-1.48702058612298828e+02)) * x + (-1.10521982468265628e+03)) * x + (-4.89115849590539165e+03)) * x + (-1.28831379137648692e+04)) * x + (-1.86903761371082182e+04)) * x + (-1.15131083313997551e+04);
    else
        return (((((((-2.28622025496525272e-01) * x + (-2.00529844277488190e+00)) * x + (-6.53235283202945194e+00)) * x + (-9.61772248585279854e+00)) * x + (-5.70828939827403392e+00)) * x + (2.19154123202269790e-01)) * x + (1.17593359427583111e+00)) * x + (1.94853803378990320e+00);
}

// Slug flow coeff F7 vs log(NL) [6]
// Polynomial degree 9, max rel error: 4.33e-03, max abs error: 1.36e-02
inline double dunsRosF7(double x) {
    if (x <= -5.80914299999999972e+00)
        return -2.07944199999999979e+00;
    if (x >= 4.05465000000000020e-01)
        return -3.72970200000000007e+00;
    return (((((((((-8.50716528657631727e-05) * x + (-2.13334660021798550e-03)) * x + (-2.18310096162851013e-02)) * x + (-1.16774452889603780e-01)) * x + (-3.47066158400594849e-01)) * x + (-5.62209383918115924e-01)) * x + (-4.45509942665859748e-01)) * x + (-5.75790684209300727e-02)) * x + (-2.86329574447428230e-02)) * x + (-3.65919454744752892e+00);
}

// Friction correction factor vs log(friction param) [6]
// Two-piece polynomial: split at x = -1.203973
// Left piece: degree 3, max err: 6.94e-17
// Right piece: degree 7, max err: 4.29e-14
inline double dunsRosFrictionCorrection(double x) {
    if (x <= -6.90775499999999987e+00)
        return 0.00000000000000000e+00;
    if (x >= 6.21460800000000013e+00)
        return -1.57021699999999997e+00;
    if (x <= -1.20397299999999996e+00)
        return (((8.45694171015253388e-05) * x + (2.03447461029604296e-03)) * x + (2.35038980413764011e-02)) * x + (9.31555762208741284e-02);
    else
        return (((((((3.70754805673244125e-05) * x + (-8.01624043181155938e-04)) * x + (6.41700125067087759e-03)) * x + (-2.34843718214933807e-02)) * x + (4.24222424022711250e-02)) * x + (-1.48838459754024599e-02)) * x + (-3.98586632637663252e-01)) * x + (-2.48460999999996546e-01);
}

// Transition factor Y1 vs log(Nd) [6]
// Drops then plateaus at 1.0
// Polynomial degree 5, max abs error: 1.84e-11
inline double dunsRosTransitionY1(double x) {
    if (x <= 2.99573199999999984e+00)
        return 2.00000000000000000e+00;
    if (x >= 4.24849500000000013e+00)
        return 1.00000000000000000e+00;
    return (((((6.52938688140051138e+00) * x + (-1.19691594423791926e+02)) * x + (8.76071926572838152e+02)) * x + (-3.20051921040981733e+03)) * x + (5.83470891919353380e+03)) * x + (-4.24300359609827319e+03);
}

// Transition factor Y2 vs log(Nd) [6]
// Rises then plateaus at 1.1
// Polynomial degree 3, max abs error: 4.32e-03
inline double dunsRosTransitionY2(double x) {
    if (x <= 2.99573199999999984e+00)
        return 5.50000000000000044e-01;
    if (x >= 4.24849500000000013e+00)
        return 1.10000000000000009e+00;
    return (((-2.22447213527426857e-01) * x + (2.32165576559761933e+00)) * x + (-7.53831973651234133e+00)) * x + (8.27725851140069047e+00);
}

// ============================================================================
//    ORKISZEWSKI [7]: Bubble velocity coefficient C2
// ============================================================================
// C2 at ReL=2900 vs bubble Reynolds
// Polynomial degree 4, max rel error: 2.96e-14, max abs error: 3.20e-14
inline double orkiszewskiC2_ReL2900(double x) {
    if (x <= 3.00000000000000000e+03)
        return 1.50000000000000000e+00;
    if (x >= 8.00000000000000000e+03)
        return 1.08000000000000007e+00;
    return ((((2.75000000000013185e-15) * x + (-6.86666666666691567e-11)) * x + (6.42250000000014264e-07)) * x + (-2.69633333333335872e-03)) * x + (5.43999999999999506e+00);
}

// C2 at ReL=6000 vs bubble Reynolds
// Polynomial degree 4, max rel error: 6.27e-14, max abs error: 7.31e-14
inline double orkiszewskiC2_ReL6000(double x) {
    if (x <= 3.00000000000000000e+03)
        return 1.70999999999999996e+00;
    if (x >= 8.00000000000000000e+03)
        return 1.16500000000000004e+00;
    return ((((-6.62500000000023841e-15) * x + (1.39250000000004870e-10)) * x + (-1.02337500000003130e-06)) * x + (2.96075000000007443e-03)) * x + (-1.18500000000003580e+00);
}

// 2D interpolation for C2(ReB, ReL)
inline double orkiszewskiC2(double reB, double reL) {
    if (reL <= 0.0)
        return 1.0;
    double c2_2900 = orkiszewskiC2_ReL2900(reB);
    double c2_6000 = orkiszewskiC2_ReL6000(reB);
    if (reL <= 2900.0)
        return 1.0 + (reL / 2900.0) * (c2_2900 - 1.0);
    if (reL >= 6000.0)
        return c2_6000;
    return c2_2900 + ((reL - 2900.0) / 3100.0) * (c2_6000 - c2_2900);
}

// ============================================================================
//    EATON [8]: Liquid holdup correlation
// ============================================================================
// Eaton liquid holdup vs log(dimensionless param) [8]
// Two-piece polynomial: split at x = 0.000000
// Left piece: degree 12, max err: 3.33e-03
// Right piece: degree 3, max err: 2.14e-03
inline double eatonHoldup(double x) {
    if (x <= -6.90775999999999968e+00)
        return 3.00000000000000006e-03;
    if (x >= 4.60517000000000021e+00)
        return 1.00000000000000000e+00;
    if (x <= 0.00000000000000000e+00)
        return ((((((((((((1.62549533228562073e-06) * x + (5.93889312097176138e-05)) * x + (9.51297088088857968e-04)) * x + (8.78253375144138516e-03)) * x + (5.16307918190999221e-02)) * x + (2.01286218487717772e-01)) * x + (5.24363155116798763e-01)) * x + (8.93217160816801248e-01)) * x + (9.31818976346808436e-01)) * x + (4.92037313884328253e-01)) * x + (3.97319798331640314e-02)) * x + (1.78941384551947941e-01)) * x + (7.41940705923420096e-01);
    else
        return (((3.55446637727444090e-03) * x + (-4.37864451652546227e-02)) * x + (1.82118329375998950e-01)) * x + (7.42195256001284775e-01);
}

// ============================================================================
//    FLANIGAN [9]: Elevation gradient correction phi factor
// ============================================================================
// Flanigan phi factor vs superficial gas velocity (ft/s) [9]
// Two-piece polynomial: split at x = 8.000000
// Left piece: degree 5, max err: 1.66e-03
// Right piece: degree 6, max err: 2.07e-03
inline double flaniganPhi(double x) {
    if (x <= 0.00000000000000000e+00)
        return 1.00000000000000000e+00;
    if (x >= 4.80000000000000000e+01)
        return 5.99999999999999978e-02;
    if (x <= 8.00000000000000000e+00)
        return (((((-1.35864513130370035e-04) * x + (2.85869090627388807e-03)) * x + (-2.28675333981216769e-02)) * x + (9.68050215142329112e-02)) * x + (-3.07455284896367809e-01)) * x + (1.00014447284839858e+00);
    else
        return ((((((3.20585740014670868e-09) * x + (-4.51881725271688946e-07)) * x + (2.50171526843196362e-05)) * x + (-7.12461158024695605e-04)) * x + (1.16668911277511625e-02)) * x + (-1.17802356100569092e-01)) * x + (7.56774525341857740e-01);
}

// ============================================================================
//    DUKLER [10]: Friction factor ratio f_TP/f_k vs log(lambda)
// ============================================================================
// Friction factor ratio vs log(input liquid fraction) [10]
// Polynomial degree 8, max rel error: 4.74e-03, max abs error: 1.23e-02
inline double duklerFrictionRatio(double x) {
    if (x <= -9.21034000000000042e+00)
        return 1.35000000000000009e+00;
    if (x >= 0.00000000000000000e+00)
        return 1.07000000000000006e+00;
    return ((((((((-7.73476313969206176e-06) * x + (-2.68184159684643579e-04)) * x + (-3.76440138214676270e-03)) * x + (-2.76331771372627447e-02)) * x + (-1.14797192456252389e-01)) * x + (-2.81766806705029571e-01)) * x + (-5.33223630662621573e-01)) * x + (-1.15494059277228178e+00)) * x + (1.06953480597439299e+00);
}

// ============================================================================
//    DUKLER HOLDUP [10]: Replaces A[9][11] and R[9][11] matrices
// For each Reynolds number column: log(Dukler_holdup) = f(log(no_slip_holdup))
// ============================================================================
// Re column 0 (Re=100): log(holdup) vs log(no-slip) [10]
// Polynomial degree 5, max rel error: 1.66e-02, max abs error: 1.22e-02
inline double duklerHoldupCol0(double x) {
    if (x <= -9.21034037197618183e+00)
        return -1.89711998488588129e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return (((((-2.59628918470756565e-04) * x + (-5.22989717514588375e-03)) * x + (-3.02474328340132173e-02)) * x + (-3.99765178375105709e-02)) * x + (1.85822973408632353e-01)) * x + (-1.30871318967014840e-05);
}

// Re column 1 (Re=500): log(holdup) vs log(no-slip) [10]
// Polynomial degree 5, max rel error: 1.81e-02, max abs error: 1.15e-02
inline double duklerHoldupCol1(double x) {
    if (x <= -9.21034037197618183e+00)
        return -3.50655789731998180e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return (((((-2.79098509793027051e-04) * x + (-5.50772825568096071e-03)) * x + (-2.92385687134256583e-02)) * x + (-2.88833230984973342e-02)) * x + (3.00276668860198881e-01)) * x + (9.43895948952426012e-04);
}

// Re column 2 (Re=1000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 5, max rel error: 1.20e-02, max abs error: 1.61e-02
inline double duklerHoldupCol2(double x) {
    if (x <= -9.21034037197618183e+00)
        return -4.46540824361293254e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return (((((-4.70536954578005043e-04) * x + (-8.93632137993703550e-03)) * x + (-4.68088793935096809e-02)) * x + (-5.38710956137520430e-02)) * x + (3.63512506659725676e-01)) * x + (9.46852196040737226e-04);
}

// Re column 3 (Re=2500): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 4.90e-03, max abs error: 8.89e-03
inline double duklerHoldupCol3(double x) {
    if (x <= -9.21034037197618183e+00)
        return -5.29831736654803631e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-2.93879164170658170e-04) * x + (-8.36089612528134740e-03)) * x + (-8.82758154045238247e-02)) * x + (-4.14721320188771114e-01)) * x + (-8.26777664306759807e-01)) * x + (-1.41572949522773911e-01)) * x + (-9.10260767881254651e-05);
}

// Re column 4 (Re=5000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 3.47e-03, max abs error: 6.13e-03
inline double duklerHoldupCol4(double x) {
    if (x <= -9.21034037197618183e+00)
        return -6.26590139280974245e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-1.65089284084632220e-04) * x + (-4.98963673836792451e-03)) * x + (-5.50870791748192534e-02)) * x + (-2.62460001250011066e-01)) * x + (-5.20491135736724231e-01)) * x + (7.49118958543802255e-02)) * x + (8.88948334202647497e-06);
}

// Re column 5 (Re=10000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 3.41e-03, max abs error: 9.06e-03
inline double duklerHoldupCol5(double x) {
    if (x <= -9.11503019217185795e+00)
        return -6.90775527898213682e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-2.20205305382305581e-04) * x + (-6.27307250006365474e-03)) * x + (-6.52878154117255971e-02)) * x + (-2.91277775616893053e-01)) * x + (-5.25518384463224630e-01)) * x + (1.72088488785785643e-01)) * x + (-3.73978283031798712e-05);
}

// Re column 6 (Re=25000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 5.14e-03, max abs error: 1.30e-02
inline double duklerHoldupCol6(double x) {
    if (x <= -8.80487526386801811e+00)
        return -6.90775527898213682e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-3.05199941212679982e-04) * x + (-8.05726306806374762e-03)) * x + (-7.76546585758117330e-02)) * x + (-3.21125008412290136e-01)) * x + (-5.27700757315488156e-01)) * x + (3.01318513254467624e-01)) * x + (-2.81131065785681755e-04);
}

// Re column 7 (Re=50000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 2.19e-03, max abs error: 5.27e-03
inline double duklerHoldupCol7(double x) {
    if (x <= -8.51719319141623821e+00)
        return -6.90775527898213682e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-2.70493627884146328e-04) * x + (-7.06695713871086529e-03)) * x + (-6.73110229232816437e-02)) * x + (-2.74731708736632996e-01)) * x + (-4.43107610895837722e-01)) * x + (4.43541891675321487e-01)) * x + (3.62846320930051077e-05);
}

// Re column 8 (Re=100000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 8.88e-03, max abs error: 1.88e-02
inline double duklerHoldupCol8(double x) {
    if (x <= -8.18072095479502437e+00)
        return -6.90775527898213682e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-4.16310181287708617e-04) * x + (-1.02736241589344631e-02)) * x + (-9.37041894673498466e-02)) * x + (-3.79229525370981035e-01)) * x + (-6.42819988491725502e-01)) * x + (4.23637653181304585e-01)) * x + (-2.07389641384262774e-04);
}

// Re column 9 (Re=200000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 6, max rel error: 3.39e-03, max abs error: 6.15e-03
inline double duklerHoldupCol9(double x) {
    if (x <= -7.41858090274812820e+00)
        return -6.90775527898213682e+00;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((((((-3.26968478373733655e-04) * x + (-8.04673999994433113e-03)) * x + (-7.43203622755870263e-02)) * x + (-3.11323409054068467e-01)) * x + (-5.60483320069777746e-01)) * x + (5.88721541179178742e-01)) * x + (-3.62129835259850097e-04);
}

// Re column 10 (Re=10000000): log(holdup) vs log(no-slip) [10]
// Polynomial degree 2, max rel error: 7.41e-16, max abs error: 2.66e-15
inline double duklerHoldupCol10(double x) {
    if (x <= -1.38155105579642736e+01)
        return -1.38155105579642736e+01;
    if (x >= 0.00000000000000000e+00)
        return 0.00000000000000000e+00;
    return ((-8.84166796619271842e-17) * x + (9.99999999999998779e-01)) * x + (-8.88178419700125232e-16);
}

// Select Dukler holdup column by Reynolds number index (0-10)
// Input: logNoSlipHoldup = log(no-slip liquid holdup)
// Output: log(Dukler liquid holdup)
inline double duklerHoldupByColumn(double logNoSlipHoldup, int colIndex) {
    switch (colIndex) {
    case 0:
        return duklerHoldupCol0(logNoSlipHoldup);
    case 1:
        return duklerHoldupCol1(logNoSlipHoldup);
    case 2:
        return duklerHoldupCol2(logNoSlipHoldup);
    case 3:
        return duklerHoldupCol3(logNoSlipHoldup);
    case 4:
        return duklerHoldupCol4(logNoSlipHoldup);
    case 5:
        return duklerHoldupCol5(logNoSlipHoldup);
    case 6:
        return duklerHoldupCol6(logNoSlipHoldup);
    case 7:
        return duklerHoldupCol7(logNoSlipHoldup);
    case 8:
        return duklerHoldupCol8(logNoSlipHoldup);
    case 9:
        return duklerHoldupCol9(logNoSlipHoldup);
    case 10:
        return duklerHoldupCol10(logNoSlipHoldup);
    default:
        return logNoSlipHoldup;
    }
}

// Full Dukler holdup: input no-slip holdup (0-1) and column index
// Returns Dukler holdup (0-1)
inline double duklerHoldup(double noSlipHoldup, int colIndex) {
    if (noSlipHoldup <= 0.0)
        return 0.0;
    if (noSlipHoldup >= 1.0)
        return 1.0;
    double logResult = duklerHoldupByColumn(log(noSlipHoldup), colIndex);
    double result = exp(logResult);
    if (result < 0.0)
        result = 0.0;
    if (result > 1.0)
        result = 1.0;
    return result;
}

// ============================================================================
//    MUKHERJEE-BRILL [11]: Mist flow correction factors
// ============================================================================
// Friction factor ratio for mist flow (index 1-9) [11]
// Polynomial degree 8, max rel error: 9.45e-11, max abs error: 9.45e-11
inline double mukherjeeFrictionRatio(double x) {
    if (x <= -3.00000000000000000e+00)
        return 1.00000000000000000e+00;
    if (x >= 1.00000000000000000e+00)
        return 1.00000000000000000e+00;
    return ((((((((-1.31921314648925527e+01) * x + (-7.38288890745069750e+01)) * x + (-1.08529877050545622e+02)) * x + (1.20332286099896990e+01)) * x + (1.08772252122997998e+02)) * x + (6.57136095046523678e+01)) * x + (1.02466261673417893e+01)) * x + (-1.21481881504504075e+00)) * x + (1.00000000000121259e+00);
}

// Holdup correction for mist flow (index 1-9) [11]
// Polynomial degree 2, max rel error: 1.79e-15, max abs error: 4.44e-16
inline double mukherjeeHoldupRatio(double x) {
    if (x <= -3.00000000000000000e+00)
        return -3.00000000000000000e+00;
    if (x >= 1.00000000000000000e+00)
        return 1.00000000000000000e+00;
    return ((1.68266556585015865e-16) * x + (1.00000000000000044e+00)) * x + (-2.22044604925031308e-16);
}

} // namespace CorrelationFits