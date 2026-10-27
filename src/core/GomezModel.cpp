// ==============================================================================
// GomezModel.cpp
//
// Unified Mechanistic Model for Steady-State Two-Phase Flow
// Horizontal to Vertical Upward Flow
//
// Referencia: Gomez, Shoham, Schmidt, Chokshi & Northug (2000)
//   SPE Journal, Vol. 5, No. 3, pp. 339-350
//
// Outras Referências:
//   Vieira, Rinaldo (2026). Notas de Aula: Escoamento Multifásico. Universidade Petrobras.
//   Xiao, Shoham & Brill (1990). A comprehensive mechanistic model for two-phase flow in pipelines.
//   Barnea (1987). A Unified Model for Predicting Flow-Pattern Transitions for the Whole Range of Pipe Inclinations.
// ==============================================================================

#include "GomezModel.h"
#include <cmath>
#include <algorithm>
#include <functional>
#include <limits>

static const double PI = 3.14159265358979323846;
static constexpr double kAngleSymmetricFilmDeg = 86.0;

// ==============================================================================
// 1. UTILITÁRIOS — FATORES DE ATRITO
// ==============================================================================

static double fanningFriction(double Re, double eps_over_d = 0.0) {
    if (Re < 1.0) Re = 1.0;
    if (Re <= 2300.0)
        return 16.0 / Re;
    double term = pow(2.0e4 * eps_over_d + 1.0e6 / Re, 1.0 / 3.0);
    return 0.001375 * (1.0 + term);
}

static double ouyangAzizFriction(double ReL, double vSG, double vSL) {
    if (ReL < 1.0e-10) ReL = 1.0e-10;
    if (vSL < 1.0e-10) vSL = 1.0e-10;
    return 1.6291 / pow(ReL, 0.5161) * pow(vSG / vSL, 0.0926);
}


static double xiaoColebrookFriction(double Re, double eps_over_d) {
    if (Re < 1.0) Re = 1.0;
    if (Re <= 2000.0)
        return 16.0 / Re;                          
    double f = fanningFriction(Re, eps_over_d);   
    for (int k = 0; k < 20; ++k) {
        double rhs   = 3.48 - 4.0 * log10(2.0 * eps_over_d
                                         + 9.35 / (Re * sqrt(f)));
        double f_new = 1.0 / (rhs * rhs);
        if (fabs(f_new - f) < 1.0e-12) { f = f_new; break; }
        f = f_new;
    }
    return f;
}
// ==============================================================================
// 2. GEOMETRIA ESTRATIFICADA
// ==============================================================================

struct StratifiedGeometry {
    double HL, AL, AG, SL, SG, SI, dAL_dhL;
};

static StratifiedGeometry computeStratifiedGeometry(double hL_over_d, double d) {
    StratifiedGeometry g;
    double hLd    = std::max(0.001, std::min(0.999, hL_over_d));
    double eta    = std::max(-0.999, std::min(0.999, 2.0 * hLd - 1.0));
    double theta  = acos(eta);
    double sqterm = sqrt(1.0 - eta * eta);

    g.HL      = (PI - theta + eta * sqterm) / PI;
    double A  = PI * d * d / 4.0;
    g.AL      = g.HL * A;
    g.AG      = (1.0 - g.HL) * A;
    g.SL      = d * (PI - theta);
    g.SG      = d * theta;
    g.SI      = d * sqterm;
    g.dAL_dhL = d * sqterm;
    return g;
}

// ==============================================================================
// 3. GEOMETRIA ANULAR
// ==============================================================================

struct AnnularGeometry { double AF, AC, SF, SI; };

static AnnularGeometry computeAnnularGeometry(double delta_over_d, double d) {
    double dod = std::max(1.0e-6, std::min(0.499, delta_over_d));
    double ri  = d / 2.0 - dod * d;
    if (ri < 0.0) ri = 0.0;
    return { PI/4.0*d*d - PI*ri*ri, PI*ri*ri, PI*d, 2.0*PI*ri };
}

// ==============================================================================
// 4. GEOMETRIA DO FILME VERTICAL DA GOLFADA (simétrico)
// ==============================================================================


struct VerticalFilmGeom {
    double delta, di, AF, AC, SF, SI, HLTB;
};

static VerticalFilmGeom verticalFilmGeometry(double delta_over_d, double d) {
    VerticalFilmGeom g;
    double dod = std::max(1.0e-6, std::min(0.499, delta_over_d));
    g.delta    = dod * d;
    g.di       = std::max(0.0, d - 2.0 * g.delta);
    double A   = PI * d * d / 4.0;
    g.AC       = PI * g.di * g.di / 4.0;
    g.AF       = A - g.AC;
    g.SF       = PI * d;
    g.SI       = PI * g.di;
    g.HLTB     = g.AF / A;
    return g;
}

// ==============================================================================
// 5. FATOR DE ATRITO INTERFACIAL — ESTRATIFICADO (Baker et al. via Xiao)
// ==============================================================================

static double stratifiedInterfacialFriction(
    double d, double hL, double vL, double vG, double vSG,
    double rhoG, double rhoL, double muL, double sigma,
    double pressure, double eps, double ReG, double fG, bool isWavy
) {
    if (!isWavy) return fG;

    if (d <= 0.127) {
        double vSG_t = 5.0 * sqrt(101325.0 / std::max(pressure, 1.0));
        if (vSG <= vSG_t) return fG;
        return fG * (1.0 + 15.0 * sqrt(hL / d) * (vSG / vSG_t - 1.0));
    }

    double eps_I  = 0.01 * d;
    double vL_abs = std::max(fabs(vL), 1.0e-6);
    for (int iter = 0; iter < 50; iter++) {
        eps_I = std::max(eps_I, 1.0e-10);
        double Nwe     = rhoG * vL_abs * vL_abs * eps_I / sigma;
        double Nmu     = muL * muL / (rhoL * sigma * eps_I);
        double product = Nwe * Nmu;
        double eps_new = (product <= 0.005)
                       ? 34.0  * sigma / (rhoG * vL_abs * vL_abs)
                       : 170.0 * sigma / (rhoG * vL_abs * vL_abs) * pow(product, 0.3);
        eps_new = std::max(eps, std::min(0.25 * hL, eps_new));
        if (fabs(eps_new - eps_I) < 1.0e-10 * d) { eps_I = eps_new; break; }
        eps_I = eps_new;
    }
    return xiaoColebrookFriction(ReG, eps_I / d);
}

// ==============================================================================
// 6. MODELO ESTRATIFICADO (B1)
// ==============================================================================

static double stratifiedResidual(
    double hL_over_d, double d, double vSL, double vSG,
    double rhoL, double rhoG, double muL, double muG,
    double sigma, double eps, double pressure, double g,
    double angle_rad, double fI_override,
    StratifiedGeometry& geom_out, bool& isWavy_out,
    double& vL_out, double& vG_out
) {
    StratifiedGeometry geom = computeStratifiedGeometry(hL_over_d, d);
    geom_out = geom;
    if (geom.HL < 1e-6 || geom.HL > 1.0-1e-6 ||
        geom.AL < 1e-12 || geom.AG < 1e-12) {
        vL_out = vG_out = 0; isWavy_out = false; return 1e10;
    }

    double vL = vSL / geom.HL;
    double vG = vSG / (1.0 - geom.HL);
    vL_out = vL; vG_out = vG;

    double cosA       = cos(angle_rad);
    double vG_crit_sq = 4.0 * muL * (rhoL - rhoG) * g * cosA
                      / (0.06 * rhoL * rhoG * std::max(fabs(vL), 1e-10));
    isWavy_out = (vG * vG >= vG_crit_sq && vG_crit_sq > 0);
    // Ondas geradas pela gravidade em escoamento descendente,
    // independentemente da velocidade do gás.
    if (!isWavy_out && angle_rad < 0.0) {
        double hL = hL_over_d * d;
        if (hL > 1.0e-6) {
            double Fr_E = fabs(vL) / sqrt(g * hL);
            if (Fr_E >= 1.5) isWavy_out = true;
        }
    }
    double dL  = 4.0 * geom.AL / geom.SL;
    double dG  = 4.0 * geom.AG / (geom.SG + geom.SI);
    double ReL = rhoL * fabs(vL) * dL / muL;
    double ReG = rhoG * fabs(vG) * dG / muG;
    double fG  = fanningFriction(ReG, eps / d);
    double fL  = ouyangAzizFriction(ReL, vSG, vSL);
    double fI  = (fI_override > 0) ? fI_override
               : stratifiedInterfacialFriction(d, hL_over_d*d, vL, vG, vSG,
                     rhoG, rhoL, muL, sigma, pressure, eps, ReG, fG, isWavy_out);

    double sinA  = sin(angle_rad);
    double tauWL = fL * rhoL * vL * fabs(vL) / 2.0;
    double tauWG = fG * rhoG * vG * fabs(vG) / 2.0;
    double tauI  = fI * rhoG * fabs(vG-vL) * (vG-vL) / 2.0;

    return tauWL * geom.SL / geom.AL
         - tauWG * geom.SG / geom.AG
         - tauI  * geom.SI * (1.0/geom.AL + 1.0/geom.AG)
         + (rhoL - rhoG) * g * sinA;
}

static StratifiedResult solveStratified(const GomezInput& in, double fI_override = -1.0) {
    StratifiedResult res;
    res.converged    = false;
    double angle_rad = in.angle * PI / 180.0;
    double lo = 0.001, hi = 0.999;
    StratifiedGeometry geom; bool isWavy; double vL, vG;

    double hL_sol = -1.0;
    double prev   = stratifiedResidual(lo, in.diameter, in.vSL, in.vSG,
        in.rhoL, in.rhoG, in.muL, in.muG, in.sigma, in.roughness,
        in.pressure, in.gravity, angle_rad, fI_override, geom, isWavy, vL, vG);

    for (int i = 1; i <= 500; i++) {
        double hLd = lo + (hi - lo) * i / 500;
        double val = stratifiedResidual(hLd, in.diameter, in.vSL, in.vSG,
            in.rhoL, in.rhoG, in.muL, in.muG, in.sigma, in.roughness,
            in.pressure, in.gravity, angle_rad, fI_override, geom, isWavy, vL, vG);
        if (prev * val < 0) {
            double a = lo + (hi - lo) * (i-1) / 500, b = hLd;
            for (int j = 0; j < 60; j++) {
                double mid = (a+b)/2.0;
                double mv  = stratifiedResidual(mid, in.diameter, in.vSL, in.vSG,
                    in.rhoL, in.rhoG, in.muL, in.muG, in.sigma, in.roughness,
                    in.pressure, in.gravity, angle_rad, fI_override, geom, isWavy, vL, vG);
                if (fabs(mv) < 1e-12) { a = mid; break; }
                if (mv * prev < 0) b = mid; else { a = mid; prev = mv; }
            }
            hL_sol = (a+b)/2.0; break;
        }
        prev = val;
    }
    if (hL_sol < 0) hL_sol = 0.5;

    stratifiedResidual(hL_sol, in.diameter, in.vSL, in.vSG,
        in.rhoL, in.rhoG, in.muL, in.muG, in.sigma, in.roughness,
        in.pressure, in.gravity, angle_rad, fI_override, geom, isWavy, vL, vG);

    res.hL_over_d = hL_sol;
    res.HL = geom.HL; res.vL = vL; res.vG = vG;
    res.isWavy = isWavy; res.converged = true;

    double sinA  = sin(angle_rad);
    double dG    = 4.0 * geom.AG / (geom.SG + geom.SI);
    double ReG   = in.rhoG * fabs(vG) * dG / in.muG;
    double fG    = fanningFriction(ReG, in.roughness / in.diameter);
    double fI_v  = (fI_override > 0) ? fI_override
                 : stratifiedInterfacialFriction(in.diameter, hL_sol*in.diameter,
                       vL, vG, in.vSG, in.rhoG, in.rhoL, in.muL, in.sigma,
                       in.pressure, in.roughness, ReG, fG, isWavy);
    double fL    = ouyangAzizFriction(in.rhoL*fabs(vL)*4.0*geom.AL/geom.SL/in.muL,
                                      in.vSG, in.vSL);
    double tauWL = fL * in.rhoL * vL * fabs(vL) / 2.0;
    double tauWG = fG * in.rhoG * vG * fabs(vG) / 2.0;
    double A     = PI * in.diameter * in.diameter / 4.0;

    res.dpdL_fric = (tauWL * geom.SL + tauWG * geom.SG) / A;
    res.dpdL_grav = (geom.HL * in.rhoL + (1.0 - geom.HL) * in.rhoG) * in.gravity * sinA;
    return res;
}

// ==============================================================================
// 7. PREDIÇÃO DO PADRÃO DE ESCOAMENTO (PARTE A)
// ==============================================================================

static bool testTransitionA1(const GomezInput& in, const StratifiedResult& strat) {
    double cosA = cos(in.angle * PI / 180.0);
    if (cosA < 1e-10) return true;
    double F2   = (in.rhoG / (in.rhoL - in.rhoG))
                * (in.vSG * in.vSG) / (in.diameter * in.gravity * cosA);
    StratifiedGeometry geom = computeStratifiedGeometry(strat.hL_over_d, in.diameter);
    double one_minus = 1.0 - strat.hL_over_d;
    if (one_minus < 1e-6) return true;
    double lhs = F2 * (strat.vG / in.vSG) * (strat.vG / in.vSG)
               * (geom.dAL_dhL / in.diameter)
               / (geom.AG / (in.diameter * in.diameter) * one_minus * one_minus);
    return (lhs >= 1.0);
}

static bool testTransitionA2(const GomezInput& in) {
    double vM    = in.vSL + in.vSG;
    double alpha = in.vSG / vM;
    if (alpha >= 0.52) return false;
    double alphaNS = in.vSG / vM;   
    double rhoM    = alphaNS * in.rhoG + (1.0 - alphaNS) * in.rhoL;
    double muM     = alphaNS * in.muG  + (1.0 - alphaNS) * in.muL;
    double fM      = fanningFriction(rhoM * vM * in.diameter / muM,
                                     in.roughness / in.diameter);
    double diss = 2.0 * fM * vM * vM * vM / in.diameter;
    if (diss < 1e-20) return false;
    double dmax = (4.15 * sqrt(in.vSG / vM) + 0.725)
                * pow(in.sigma / in.rhoL, 0.6)
                * pow(diss, -0.4);
    double dCD  = 2.0 * sqrt(0.4 * in.sigma / ((in.rhoL - in.rhoG) * in.gravity));
    double cosA = cos(in.angle * PI / 180.0);
    double dCB = (fabs(in.angle) <= 10.0 && cosA > 1e-6)
           ? (3.0/8.0) * in.rhoL / (in.rhoL - in.rhoG)
             * fM * vM * vM / (in.gravity * cosA)
           : 1e10;
    return (dmax < dCD && dmax < dCB);
}

static bool testTransitionA3_notAnnular(const GomezInput& in,
                                        const StratifiedResult& strat) {
    double fSL    = fanningFriction(in.rhoL * in.vSL * in.diameter / in.muL,
                                    in.roughness / in.diameter);
    double fSG    = fanningFriction(in.rhoG * in.vSG * in.diameter / in.muG,
                                    in.roughness / in.diameter);
    double dpdLSL = 2.0 * fSL * in.rhoL * in.vSL * in.vSL / in.diameter;
    double dpdLSG = 2.0 * fSG * in.rhoG * in.vSG * in.vSG / in.diameter;
    if (dpdLSG < 1e-20) return true;

    double X2 = dpdLSL / dpdLSG;
    double Y  = (in.rhoL - in.rhoG) * in.gravity
              * sin(in.angle * PI / 180.0) / dpdLSG;

    
    if (strat.HL >= 0.24) return true;

    double HL = strat.HL;
    
    if (HL < 1e-6) return false;
    double pow25  = pow(1.0 - HL, 2.5);
    double Y_eq8  = (1.0 + 75.0 * HL) / (pow25 * HL)
                  - X2 / (HL * HL * HL);

    if (Y < Y_eq8) return false;

    double denom9 = HL * HL * HL * (1.0 - 1.5 * HL);
    if (fabs(denom9) < 1e-14) return false;

    return (Y > (2.0 - 1.5 * HL) / denom9 * X2);
}

static int testTransitionA4(const GomezInput& in) {
    double vM   = in.vSL + in.vSG;
    double sinA = sin(in.angle * PI / 180.0);
    double v0   = 1.53 * pow(in.gravity * in.sigma * (in.rhoL - in.rhoG)
                             / (in.rhoL * in.rhoL), 0.25);

    double vSL_crit = (0.75 / 0.25) * in.vSG - v0 * sqrt(0.75) * sinA;

    double alpha = in.vSG / vM;
    if (alpha >= 0.25) return 0;   
    if (in.vSL <= vSL_crit) return 0;  // Slug


    double d_crit = 19.0 * sqrt((in.rhoL - in.rhoG) * in.sigma
                               / (in.rhoL * in.rhoL * in.gravity));
    if (in.diameter > d_crit && in.angle >= 60.0) return 1;  // Bubble

    return 0;  // Slug
}


static FlowPattern predictFlowPattern(const GomezInput& in,
                                      StratifiedResult& stratResult) {

    // ------------------------------------------------------------------
    // Passo 1 — Bolhas Dispersas 
    // ------------------------------------------------------------------
    if (testTransitionA2(in))
        return FlowPattern::DispersedBubble;

    // ------------------------------------------------------------------
    // Passo 2 — Estratificado → Não-estratificado 
    // ------------------------------------------------------------------
    stratResult       = solveStratified(in);
    const bool unstable = testTransitionA1(in, stratResult);

    if (!unstable)
        return stratResult.isWavy ? FlowPattern::StratifiedWavy
                                  : FlowPattern::StratifiedSmooth;

    // ------------------------------------------------------------------
    // Passo 3 — Anular → Intermitente 
    // ------------------------------------------------------------------
    if (!testTransitionA3_notAnnular(in, stratResult))
        return FlowPattern::AnnularMist;

    // ------------------------------------------------------------------
    // Passo 4 — Bolhas vs Golfadas
    // ------------------------------------------------------------------
    const int a4 = testTransitionA4(in);
    if (a4 == 1) return FlowPattern::Bubble;

    return FlowPattern::Slug;
}

// ==============================================================================
// 8. MODELO SLUG — FILME VERTICAL (θ ≥ 86°)
// ==============================================================================

struct VerticalFilmResult {
    double delta_over_d;  // espessura normalizada do filme (δ/d)
    double HLTB, vLTB, vGTB, tauWF;
    bool   converged;
};

static double verticalFilmResidual(
    double dod, double vTB, double vLLS, double HLLS,
    const GomezInput& in, double sinA, double cosA
) {
    VerticalFilmGeom g = verticalFilmGeometry(dod, in.diameter);
    if (g.HLTB < 1e-6 || g.HLTB > 1.0 - 1e-6) return 1e10;

    double vM   = in.vSL + in.vSG;
    double vLTB = vTB - (vTB - vLLS) * HLLS / g.HLTB;    // Eq. 27
    double vGTB = (vM - vLTB * g.HLTB) / (1.0 - g.HLTB); // Eq. 29

    double dH_F = std::max(4.0 * g.AF / g.SF, 1e-6);
    double fF   = fanningFriction(in.rhoL * fabs(vLTB) * dH_F / in.muL,
                                  in.roughness / in.diameter);

    
    double fSC  = fanningFriction(in.rhoG * fabs(in.vSG) * in.diameter / in.muG,
                                  in.roughness / in.diameter);
    double ReSL = in.rhoL * in.vSL * in.diameter / in.muL;
    double ReSG = in.rhoG * in.vSG * in.diameter / in.muG;
    double FA   = pow( pow(0.707 * sqrt(ReSL), 2.5)
                     + pow(0.0379 * pow(ReSL, 0.9), 2.5), 0.4 )
                / pow(ReSG, 0.9)
                * fabs(vLTB) / (fabs(vGTB) + 1e-20)
                * sqrt(in.rhoL / in.rhoG);
    double fI   = fSC * ((1.0 + 850.0 * FA) * cosA * cosA
                       + (1.0 + 300.0 * dod) * sinA * sinA);

    double tauWF = fF * in.rhoL * fabs(vLTB) * vLTB / 2.0;
    double dv    = vGTB - vLTB;
    double tauI  = fI * in.rhoG * fabs(dv) * dv / 2.0;

    // Eq. 32 (forma anular)
    return tauWF * (g.SF / g.AF)
         - tauI  *  g.SI * (1.0 / g.AF + 1.0 / g.AC)
         + (in.rhoL - in.rhoG) * in.gravity * sinA;
}

static VerticalFilmResult solveSlugFilmVertical(
    double vTB, double vLLS, double HLLS,
    const GomezInput& in, double sinA, double cosA
) {
    VerticalFilmResult res; res.converged = false;
    double lo = 1e-4, hi = 0.499, dod_sol = 0.05;
    auto   f  = [&](double d){ return verticalFilmResidual(d,vTB,vLLS,HLLS,in,sinA,cosA); };
    double prev = f(lo);
    for (int i = 1; i <= 300; i++) {
        double dod = lo + (hi - lo) * i / 300;
        double val = f(dod);
        if (prev * val < 0.0) {
            double a = lo + (hi - lo) * (i-1) / 300, b = dod;
            for (int j = 0; j < 60; j++) {
                double mid = (a+b)/2.0, mv = f(mid);
                if (fabs(mv) < 1e-12) { a = mid; break; }
                if (mv * prev < 0.0) b = mid; else { a = mid; prev = mv; }
            }
            dod_sol = (a+b)/2.0; res.converged = true; break;
        }
        prev = val;
    }
    VerticalFilmGeom g = verticalFilmGeometry(dod_sol, in.diameter);
    res.delta_over_d   = dod_sol;
    res.HLTB           = g.HLTB;
    double vM          = in.vSL + in.vSG;
    res.vLTB           = vTB - (vTB - vLLS) * HLLS / g.HLTB;
    res.vGTB           = (vM - res.vLTB * g.HLTB) / (1.0 - g.HLTB);
    double dH_F        = std::max(4.0 * g.AF / g.SF, 1e-6);
    double fF          = fanningFriction(in.rhoL * fabs(res.vLTB) * dH_F / in.muL,
                                         in.roughness / in.diameter);
    res.tauWF          = fF * in.rhoL * fabs(res.vLTB) * res.vLTB / 2.0;
    return res;
}

// ==============================================================================
// 9. MODELO SLUG — FILME HORIZONTAL (θ < 86°)
// ==============================================================================

struct HorizontalFilmResult {
    double hLTB_over_d;  // nível normalizado do líquido no filme (hLTB/d)
    double HLTB, vLTB, vGTB, tauWF, tauWG;
    bool   converged;
};

static double horizontalFilmResidual(
    double hLTBd, double vTB, double vLLS, double HLLS,
    const GomezInput& in, double sinA
) {
    StratifiedGeometry g = computeStratifiedGeometry(hLTBd, in.diameter);
    if (g.HL < 1e-6 || g.HL > 1.0 - 1e-6 ||
        g.AL < 1e-12 || g.AG < 1e-12) return 1e10;

    double vM   = in.vSL + in.vSG;
    double vLTB = vTB - (vTB - vLLS) * HLLS / g.HL;       // Eq. 27
    double vGTB = (vM - vLTB * g.HL) / (1.0 - g.HL);      // Eq. 29

    double dL    = 4.0 * g.AL / g.SL;
    double dG    = 4.0 * g.AG / (g.SG + g.SI);
    double ReL   = in.rhoL * fabs(vLTB) * dL / in.muL;
    double ReG   = in.rhoG * fabs(vGTB) * dG / in.muG;
    double fG    = fanningFriction(ReG, in.roughness / in.diameter);
    double vSLf  = g.HL * fabs(vLTB);
    double vSGf  = (1.0 - g.HL) * fabs(vGTB);
    double fL    = (vSLf > 1e-10) ? ouyangAzizFriction(ReL, vSGf, vSLf)
                                   : fanningFriction(ReL, in.roughness / in.diameter);
    double tauWF = fL * in.rhoL * vLTB * fabs(vLTB) / 2.0;
    double tauWG = fG * in.rhoG * vGTB * fabs(vGTB) / 2.0;
    double dv    = vGTB - vLTB;
    double tauI  = fG * in.rhoG * fabs(dv) * dv / 2.0;  // fI = fG (interface lisa)

    // Eq. 32 (forma estratificada)
    return tauWF * (g.SL / g.AL)
         - tauWG * (g.SG / g.AG)
         - tauI  *  g.SI * (1.0 / g.AL + 1.0 / g.AG)
         + (in.rhoL - in.rhoG) * in.gravity * sinA;
}

static HorizontalFilmResult solveSlugFilmHorizontal(
    double vTB, double vLLS, double HLLS,
    const GomezInput& in, double sinA
) {
    HorizontalFilmResult res; res.converged = false;
    double lo = 0.001, hi = 0.999, hLTBd_sol = 0.1;
    auto   f  = [&](double h){ return horizontalFilmResidual(h,vTB,vLLS,HLLS,in,sinA); };
    double prev = f(lo);
    for (int i = 1; i <= 200; i++) {
        double hLTBd = lo + (hi - lo) * i / 200;
        double val   = f(hLTBd);
        if (prev * val < 0.0) {
            double a = lo + (hi - lo) * (i-1) / 200, b = hLTBd;
            for (int j = 0; j < 60; j++) {
                double mid = (a+b)/2.0, mv = f(mid);
                if (fabs(mv) < 1e-12) { a = mid; break; }
                if (mv * prev < 0.0) b = mid; else { a = mid; prev = mv; }
            }
            hLTBd_sol = (a+b)/2.0; res.converged = true; break;
        }
        prev = val;
    }
    StratifiedGeometry g = computeStratifiedGeometry(hLTBd_sol, in.diameter);
    res.hLTB_over_d = hLTBd_sol;
    res.HLTB        = g.HL;
    double vM       = in.vSL + in.vSG;
    res.vLTB        = vTB - (vTB - vLLS) * HLLS / g.HL;
    res.vGTB        = (vM - res.vLTB * g.HL) / (1.0 - g.HL);
    double dL       = 4.0 * g.AL / g.SL;
    double dG       = 4.0 * g.AG / (g.SG + g.SI);
    double vSLf     = g.HL * fabs(res.vLTB);
    double vSGf     = (1.0 - g.HL) * fabs(res.vGTB);
    double ReL      = in.rhoL * fabs(res.vLTB) * dL / in.muL;
    double ReG      = in.rhoG * fabs(res.vGTB) * dG / in.muG;
    double fG       = fanningFriction(ReG, in.roughness / in.diameter);
    double fL       = (vSLf > 1e-10) ? ouyangAzizFriction(ReL, vSGf, vSLf)
                                      : fanningFriction(ReL, in.roughness / in.diameter);
    res.tauWF       = fL * in.rhoL * res.vLTB * fabs(res.vLTB) / 2.0;
    res.tauWG       = fG * in.rhoG * res.vGTB * fabs(res.vGTB) / 2.0;
    return res;
}

// ==============================================================================
// 10. MODELO SLUG — com bifurcação horizontal/vertical
// ==============================================================================

static SlugResult solveSlug(const GomezInput& in) {
    SlugResult res; res.converged = false;
    double d         = in.diameter;
    double vM        = in.vSL + in.vSG;
    double angle_rad = in.angle * PI / 180.0;
    double sinA      = sin(angle_rad);
    double cosA      = cos(angle_rad);
    double A         = PI * d * d / 4.0;

  
    double LS;
    double d_inch = d / 0.0254;
    if (fabs(in.angle) <= 1.0 && d_inch > 2.0) {
        LS = exp(-25.4 + 28.5 * pow(log(d_inch), 0.1)) * 0.3048;
    } else {
        double LS_h = 30.0 * d, LS_v = 20.0 * d;
        LS = (in.angle >= 85.0) ? LS_v
           : (in.angle <=  5.0) ? LS_h
           : LS_h + (LS_v - LS_h) * sinA;
    }
    res.LS = LS;

    double ReSL = in.rhoL * vM * d / in.muL;
    double HLLS = exp(-(7.85e-3 * std::max(in.angle, 0.0) + 2.48e-6 * ReSL));
    res.HL_LS = HLLS;

   
    double vTB = 1.2 * vM
               + 0.542 * sqrt(in.gravity * d) * cosA
               + 0.351 * sqrt(in.gravity * d) * sinA;
    res.vTB = vTB;

   
    double v0p  = 1.53 * pow(in.gravity * in.sigma * (in.rhoL - in.rhoG)
                             / (in.rhoL * in.rhoL), 0.25);
    double vGLS = 1.15 * vM + v0p * sinA * sqrt(HLLS);
    res.vGLS    = vGLS;

  
    double vLLS = (vM - vGLS * (1.0 - HLLS)) / HLLS;
    res.vLLS    = vLLS;


    const bool useSymmetricFilm = (in.angle >= kAngleSymmetricFilmDeg);
    double HLTB, vLTB, vGTB, tauWF_film = 0.0, tauWG_film = 0.0;

    if (useSymmetricFilm) {
        VerticalFilmResult vf = solveSlugFilmVertical(vTB, vLLS, HLLS, in, sinA, cosA);
        HLTB       = vf.HLTB;
        vLTB       = vf.vLTB;
        vGTB       = vf.vGTB;
        tauWF_film = vf.tauWF;
        tauWG_film = 0.0;  // gás não toca a parede no modelo anular
        res.filmIsSymmetric             = true;
        res.symmetricFilmThicknessRatio = vf.delta_over_d;
        res.converged                   = vf.converged;
    } else {
        HorizontalFilmResult hf = solveSlugFilmHorizontal(vTB, vLLS, HLLS, in, sinA);
        HLTB       = hf.HLTB;
        vLTB       = hf.vLTB;
        vGTB       = hf.vGTB;
        tauWF_film = hf.tauWF;
        tauWG_film = hf.tauWG;
        res.filmIsSymmetric           = false;
        res.stratifiedFilmLiquidLevel = hf.hLTB_over_d;
        res.converged                 = hf.converged;
    }

    res.HL_TB = HLTB;
    res.vLTB  = vLTB;
    res.vGTB  = vGTB;


    double denom34 = in.vSL - vLTB * HLTB;
    if (fabs(denom34) < 1e-12) denom34 = 1e-12;
    double LU = LS * (vLLS * HLLS - vLTB * HLTB) / denom34;

    //   Vieira (2026)
    //   beta < 0  → Bolhas ou Bolhas dispersas
    //   beta > 1  → Anular
    //   0 < beta < 1 → Golfadas 
    double denom_beta = HLTB * vLTB - HLLS * vLLS;
    res.beta = (std::fabs(denom_beta) > 1.0e-14)
             ? (in.vSL - HLLS * vLLS) / denom_beta
             : std::numeric_limits<double>::quiet_NaN();

    if (std::isnan(LU)) LU = LS;
    res.LU = LU;
    res.LF = LU - LS;

    res.HL_U = std::max(0.0, std::min(1.0,
               (vTB * HLLS + vGLS * (1.0 - HLLS) - in.vSG) / vTB));

    double rhoU  = res.HL_U * in.rhoL + (1.0 - res.HL_U) * in.rhoG;
    double rhoS  = HLLS * in.rhoL + (1.0 - HLLS) * in.rhoG;
    double muS   = HLLS * in.muL  + (1.0 - HLLS) * in.muG;
    double tauS  = fanningFriction(rhoS * vM * d / muS, in.roughness / d)
                 * rhoS * vM * fabs(vM) / 2.0;

    double film_fric;
    if (useSymmetricFilm) {
        film_fric = tauWF_film * (PI * d / A) * (res.LF / LU);
    } else {
        StratifiedGeometry gFilm = computeStratifiedGeometry(
            res.stratifiedFilmLiquidLevel, d);
        film_fric = (tauWF_film * gFilm.SL + tauWG_film * gFilm.SG)
                  / A * (res.LF / LU);
    }

    res.dpdL_grav = rhoU * in.gravity * sinA;
    res.dpdL_fric = tauS * (PI * d / A) * (LS / LU)
                  + film_fric;

    return res;
}

// ==============================================================================
// 11. MODELO ANNULAR 
// ==============================================================================

static AnnularResult solveAnnular(const GomezInput& in) {
    AnnularResult res; res.converged = false;
    double d        = in.diameter;
    double angle_rad = in.angle * PI / 180.0;
    double sinA     = sin(angle_rad);
    double cosA_loc = cos(angle_rad);
    double A        = PI * d * d / 4.0;

    double phi = 1e4 * in.vSG * in.muG / in.sigma * sqrt(in.rhoG / in.rhoL);
    double E   = std::max(0.0, std::min(0.99, 1.0 - exp(-0.125 * (phi - 1.5))));
    res.E      = E;

    double alphaC = in.vSG / (in.vSG + in.vSL * E + 1e-20);
    double rhoC   = in.rhoG * alphaC + in.rhoL * (1.0 - alphaC);
    double muC    = in.muG  * alphaC + in.muL  * (1.0 - alphaC);
    double fSC    = fanningFriction(rhoC * (in.vSG + E * in.vSL) * d / muC,
                                    in.roughness / d);
    double ReSL   = in.rhoL * in.vSL * d / in.muL;
    double ReSG   = in.rhoG * in.vSG * d / in.muG;

    auto annRes = [&](double dod) -> double {
        AnnularGeometry geom = computeAnnularGeometry(dod, d);
        if (geom.AF < 1e-12 || geom.AC < 1e-12) return 1e10;
        double delta = dod * d;
        double vF = in.vSL * (1.0 - E) * d * d / (4.0 * delta * (d - delta));
        double vC = (in.vSG + in.vSL * E) * d * d
                  / ((d - 2.0 * delta) * (d - 2.0 * delta));
        double fF = fanningFriction(
            in.rhoL * fabs(vF) * 4.0*delta*(d-delta)/d / in.muL,
            in.roughness / d);
        double FA = pow(pow(0.707*sqrt(ReSL),2.5) + pow(0.0379*pow(ReSL,0.9),2.5), 0.4)
                  / pow(ReSG, 0.9) * fabs(vF) / (fabs(vC)+1e-20)
                  * sqrt(in.rhoL / in.rhoG);
        double fI = fSC * ((1.0 + 850.0 * FA) * cosA_loc * cosA_loc
                         + (1.0 + 300.0 * dod) * sinA * sinA);
        double tauWF = fF  * in.rhoL * fabs(vF) * vF / 2.0;
        double dv    = vC - vF;
        double tauI  = fI  * rhoC * fabs(dv) * dv / 2.0;
        return tauWF * (geom.SF / geom.AF)
             - tauI  *  geom.SI * (1.0 / geom.AF + 1.0 / geom.AC)
             + (in.rhoL - rhoC) * in.gravity * sinA;
    };

    double lo = 1.0e-5, hi = 0.49, dod_sol = 0.05, prev = annRes(lo);
    double bestDod = lo, bestAbs = fabs(prev);
    for (int i = 1; i <= 600; i++) {
        double dod = lo + (hi - lo) * i / 600, val = annRes(dod);
        if (fabs(val) < bestAbs) { bestAbs = fabs(val); bestDod = dod; }
        if (prev * val < 0) {
            double a = lo + (hi-lo)*(i-1)/600, b = dod;
            for (int j = 0; j < 60; j++) {
                double mid = (a+b)/2.0, mv = annRes(mid);
                if (fabs(mv) < 1e-12) { a = mid; break; }
                if (mv * prev < 0) b = mid; else { a = mid; prev = mv; }
            }
            dod_sol = (a+b)/2.0; res.converged = true; break;
        }
        prev = val;
    }

    if (!res.converged) dod_sol = bestDod;

    res.delta_over_d = dod_sol;
    res.HL = 1.0 - (1.0 - 2.0*dod_sol) * (1.0 - 2.0*dod_sol) * alphaC;

    double delta = dod_sol * d;
    double vF    = in.vSL * (1.0 - E) * d * d / (4.0 * delta * (d - delta));
    double vC    = (in.vSG + in.vSL * E) * d * d / ((d-2.0*delta)*(d-2.0*delta));
    AnnularGeometry geom = computeAnnularGeometry(dod_sol, d);
    double fF    = fanningFriction(
        in.rhoL * fabs(vF) * 4.0*delta*(d-delta)/d / in.muL,
        in.roughness / d);
    double tauWF = fF * in.rhoL * fabs(vF) * vF / 2.0;
    double rhoMix = (geom.AF * in.rhoL + geom.AC * rhoC) / A;
    res.dpdL_fric = tauWF * geom.SF / A;
    res.dpdL_grav = rhoMix * in.gravity * sinA;
    
    return res;
}

// ==============================================================================
// 12. MODELO BUBBLE 
// ==============================================================================

static BubbleResult solveBubble(const GomezInput& in) {
    BubbleResult res; res.converged = false;
    double vM   = in.vSL + in.vSG;
    double sinA = sin(in.angle * PI / 180.0);
    double v0p  = 1.53 * pow(in.gravity * in.sigma * (in.rhoL - in.rhoG)
                             / (in.rhoL * in.rhoL), 0.25);

    double lo = 0.01, hi = 0.999, HL = 0.5;
    auto f = [&](double H) -> double {
        if (H >= 1.0) return -1e10;
        if (H <= 0.0) return  1e10;
        return in.vSG / (1.0 - H) - (1.15 * vM + v0p * sinA * sqrt(H));
    };
    double prev = f(lo);
    for (int i = 0; i < 80; i++) {
        double mid = (lo + hi) / 2.0, val = f(mid);
        if (fabs(val) < 1e-12) { HL = mid; break; }
        if (val * prev > 0) { lo = mid; prev = val; } else hi = mid;
        HL = (lo + hi) / 2.0;
    }
    res.HL = HL; res.converged = true;
    double rhoM = HL * in.rhoL + (1.0 - HL) * in.rhoG;
    double muM  = HL * in.muL  + (1.0 - HL) * in.muG;
    double fM   = fanningFriction(rhoM * vM * in.diameter / muM,
                                  in.roughness / in.diameter);

    res.dpdL_fric = 2.0 * fM * rhoM * vM * fabs(vM) / in.diameter;
    res.dpdL_grav = rhoM * in.gravity * sinA;
    return res;
}

// ==============================================================================
// 13. MODELO DISPERSED BUBBLE 
// ==============================================================================

static BubbleResult solveDispersedBubble(const GomezInput& in) {
    BubbleResult res;
    double vM   = in.vSL + in.vSG;
    res.HL      = in.vSL / vM;
    double rhoM = res.HL * in.rhoL + (1.0 - res.HL) * in.rhoG;
    double muM  = res.HL * in.muL  + (1.0 - res.HL) * in.muG;
    double fM   = fanningFriction(rhoM * vM * in.diameter / muM,
                                  in.roughness / in.diameter);

    res.dpdL_fric = 2.0 * fM * rhoM * vM * fabs(vM) / in.diameter;
    res.dpdL_grav = rhoM * in.gravity * sin(in.angle * PI / 180.0);
    res.converged = true;
    return res;
}

// ==============================================================================
// 14. A5: SUAVIZAÇÃO DE TRANSIÇÕES
// ==============================================================================

static bool applySmoothing(const GomezInput& in, FlowPattern& pattern,
                            GomezResult& res) {
    if (pattern != FlowPattern::Slug) return false;

    // ------------------------------------------------------------------
    // Suavização 1 — Eq. 14: LF <= 1.2d  (fronteira slug ↔ bolhas)
    // ------------------------------------------------------------------
    if (res.slug.LF <= 1.2 * in.diameter) {
        if (in.vSL < 0.6) {
            pattern    = FlowPattern::Bubble;
            res.bubble = solveBubble(in);
        } else {
            pattern    = FlowPattern::DispersedBubble;
            res.bubble = solveDispersedBubble(in);
        }
        res.HL   = res.bubble.HL;
        res.dpdL_grav = res.bubble.dpdL_grav;
        res.dpdL_fric = res.bubble.dpdL_fric;
        return true;
    }

    // ------------------------------------------------------------------
    // Suavização 2 — Eq. 15: zona de transição slug → anular
    // ------------------------------------------------------------------

    double sinA = sin(in.angle * PI / 180.0);
    if (sinA <= 0.01) return false;

    // Limite inferior da zona de transição (Eq. 15 de Gomez)
    double vSG_crit = 3.1 * pow(in.sigma * in.gravity * sinA
                                * (in.rhoL - in.rhoG)
                                / (in.rhoG * in.rhoG), 0.25);

    if (in.vSG <= vSG_crit) return false;

    // ------------------------------------------------------------------
    // Localização de vSG_barnea por BISSECÇÃO
    // ------------------------------------------------------------------
    double vSG_barnea = -1.0;

    double lo_vsg = vSG_crit * 1.001;   // limite inferior: logo após vSG_crit
    double hi_vsg = in.vSG   * 20.0;    // limite superior: amplo por segurança

    // Verifica se a fronteira existe dentro do intervalo de busca
    {
        GomezInput inTest = in;
        inTest.vSG = hi_vsg;
        StratifiedResult stTest = solveStratified(inTest);
        bool upperIsAnnular = !testTransitionA3_notAnnular(inTest, stTest);

        if (upperIsAnnular) {
            // A fronteira existe: bissecção para localizá-la com precisão
            for (int j = 0; j < 50; j++) {
                double mid_vsg = (lo_vsg + hi_vsg) / 2.0;
                inTest.vSG = mid_vsg;
                stTest = solveStratified(inTest);
                if (testTransitionA3_notAnnular(inTest, stTest))
                    lo_vsg = mid_vsg;   // ainda intermitente → sobe o limite inferior
                else
                    hi_vsg = mid_vsg;   // já anular       → baixa o limite superior
            }
            vSG_barnea = (lo_vsg + hi_vsg) / 2.0;
        }
    }

    // Fronteira não encontrada: sem zona de transição
    if (vSG_barnea <= vSG_crit) return false;

    // ------------------------------------------------------------------
    // Cálculo dos gradientes nos dois extremos da zona de transição
    //   - slugC: modelo de golfadas em vSG = vSG_crit  
    //   - annB:  modelo anular    em vSG = vSG_barnea 
    // ------------------------------------------------------------------
    GomezInput inSlug = in; inSlug.vSG = vSG_crit;
    GomezInput inAnn  = in; inAnn.vSG  = vSG_barnea;

    SlugResult    slugC = solveSlug(inSlug);
    AnnularResult annB  = solveAnnular(inAnn);

    if (annB.converged && slugC.converged) {
        double frac = std::max(0.0, std::min(1.0,
                      (in.vSG - vSG_crit) / (vSG_barnea - vSG_crit)));

        
        res.dpdL_grav = (1.0 - frac) * slugC.dpdL_grav + frac * annB.dpdL_grav;
        res.dpdL_fric = (1.0 - frac) * slugC.dpdL_fric + frac * annB.dpdL_fric;
        res.HL   = (1.0 - frac) * slugC.HL_U  + frac * annB.HL;
    }

    return false;
}

// ==============================================================================
// 15. FUNÇÃO PRINCIPAL
// ==============================================================================

GomezResult calculateGomez(const GomezInput& input) {
    GomezResult res;

    StratifiedResult stratForPattern;
    FlowPattern pattern = predictFlowPattern(input, stratForPattern);

    switch (pattern) {
        case FlowPattern::StratifiedSmooth:
        case FlowPattern::StratifiedWavy: {
            res.stratified = solveStratified(input);
            res.HL   = res.stratified.HL;
            res.dpdL_grav = res.stratified.dpdL_grav;
            res.dpdL_fric = res.stratified.dpdL_fric;
            break;
        }
        case FlowPattern::Slug: {
            res.slug = solveSlug(input);
            if (!std::isnan(res.slug.beta)) {
                if (res.slug.beta >= 1.0) {
                    res.annular = solveAnnular(input);
                    res.HL      = res.annular.HL;
                    res.dpdL_grav = res.annular.dpdL_grav;
                    res.dpdL_fric = res.annular.dpdL_fric;
                    pattern     = FlowPattern::AnnularMist;
                    break;
                }
                if (res.slug.beta <= 0.0) {
                    if (input.vSL < 0.6) {
                        res.bubble = solveBubble(input);
                        pattern    = FlowPattern::Bubble;
                    } else {
                        res.bubble = solveDispersedBubble(input);
                        pattern    = FlowPattern::DispersedBubble;
                    }
                    res.HL   = res.bubble.HL;
                    res.dpdL_grav = res.bubble.dpdL_grav;
                    res.dpdL_fric = res.bubble.dpdL_fric;
                    break;
                }
            }
            res.HL   = res.slug.HL_U;
            res.dpdL_grav = res.slug.dpdL_grav;
            res.dpdL_fric = res.slug.dpdL_fric;
            break;
        }
        case FlowPattern::AnnularMist: {
            res.annular = solveAnnular(input);
            res.HL      = res.annular.HL;
            res.dpdL_grav = res.annular.dpdL_grav;
            res.dpdL_fric = res.annular.dpdL_fric;
            break;
        }
        case FlowPattern::Bubble: {
            res.bubble = solveBubble(input);
            res.HL     = res.bubble.HL;
            res.dpdL_grav = res.bubble.dpdL_grav;
            res.dpdL_fric = res.bubble.dpdL_fric;
            break;
        }
        case FlowPattern::DispersedBubble: {
            res.bubble = solveDispersedBubble(input);
            res.HL     = res.bubble.HL;
            res.dpdL_grav = res.bubble.dpdL_grav;
            res.dpdL_fric = res.bubble.dpdL_fric;
            break;
        }
    }

    applySmoothing(input, pattern, res);
    res.pattern = pattern;
    return res;
}
