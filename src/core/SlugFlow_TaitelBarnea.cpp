// ============================================================================== 
// SlugFlow_TaitelBarnea.cpp  
//
// Referência: Taitel & Barnea, "Two-Phase Slug Flow",
// Advances in Heat Transfer, Vol. 20, pp. 83-132, 1990.
// ============================================================================== 

#include "SlugFlow_TaitelBarnea.h"
#include <cmath>
#include <algorithm>
#include <vector>

static const double PI = 3.14159265358979323846;

// ============================================================================== 
// 1. GEOMETRIA DA SEÇÃO TRANSVERSAL
// ==============================================================================

/**
 * Geometria estratificada (horizontal / inclinado) 
 *
 * @param D     Diâmetro [m]
 * @param hf    Nível de líquido [m] (0 < hf < D)
 * @param Rf    Holdup do filme (Eq. 39) [saída]
 * @param Af    Área líquido [saída]
 * @param AG    Área gás [saída]
 * @param Sf    Perímetro molhado [saída]
 * @param SG    Perímetro gás [saída]
 * @param Si    Perímetro interface [saída]
 */
static void stratifiedGeometry(
    double D, double hf,
    double& Rf, double& Af, double& AG,
    double& Sf, double& SG, double& Si
) {
    double eta = 2.0 * hf / D - 1.0;               
    eta = std::max(-0.999, std::min(0.999, eta));    

    double theta = acos(eta);                        
    double sqterm = sqrt(1.0 - eta * eta);

    
    Rf = (1.0 / PI) * (PI - theta + eta * sqterm);

    double A = PI * D * D / 4.0;
    Af = Rf * A;
    AG = (1.0 - Rf) * A;

    
    Sf = D * (PI - theta);
    SG = D * theta;

    
    Si = D * sqterm;
}

/**
 * Derivada analítica dRf/dhf para geometria estratificada 
 */
static double dRf_dhf_stratified(double D, double hf) {
    double eta = 2.0 * hf / D - 1.0;
    eta = std::max(-0.999, std::min(0.999, eta));
    return (4.0 / (PI * D)) * sqrt(1.0 - eta * eta);
}

/**
 * Geometria anular (vertical / off-vertical) - filme simétrico na parede
 *
 * @param D     Diâmetro [m]
 * @param delta Espessura do filme δ [m]
 */
static void annularGeometry(
    double D, double delta,
    double& Rf, double& Af, double& AG,
    double& Sf, double& Si
) {
    double ro = D / 2.0;
    double ri = ro - delta;
    if (ri < 0.0) ri = 0.0;

    Af = PI * (ro * ro - ri * ri);
    AG = PI * ri * ri;

    double A = PI * ro * ro;
    Rf = Af / A;                 

    Sf = PI * D;                  
    Si = 2.0 * PI * ri;          
}

/**
 * Derivada dRf/dδ para geometria anular
 */
static double dRf_ddelta_annular(double D, double delta) {
    return (4.0 / D) * (1.0 - 2.0 * delta / D);
}

// ==============================================================================
// 2. FATORES DE ATRITO 
// ==============================================================================


static double frictionFactor(double Re) {
    if (Re < 1.0) Re = 1.0;
    if (Re <= 2100.0) {
        return 16.0 / Re;          // Cf=16, n=-1
    } else {
        return 0.046 * pow(Re, -0.2);  // Cf=0.046, n=-0.2
    }
}

/**
 * Fator de atrito interfacial fi
 */
static double interfacialFriction(double angle, double delta_or_hf, double D) {
    if (angle >= 80.0) {
        // Vertical 
        return 0.005 * (1.0 + 300.0 * delta_or_hf / D);
    } else {
        // Horizontal/inclinado - constante
        return 0.014;
    }
}

// ==============================================================================
// 3. TENSÕES DE CISALHAMENTO 
// ==============================================================================

// τf = ff · ρL |uf| uf / 2                                          
// τG = fG · ρG |uG| uG / 2                                          
// τi = fi · ρG |uG - uf| (uG - uf) / 2                              

// Implementadas inline nas funções que as usam.

// ==============================================================================
// 4. CORRELAÇÕES - VARIÁVEIS AUXILIARES 
// ==============================================================================


static double driftVelocity(double angle, double D, double g) {
    double sqgD = sqrt(g * D);
    double ud_v = 0.35 * sqgD;     
    double ud_h = 0.542 * sqgD;    

    if (angle >= 80.0) return ud_v;
    if (angle <= 5.0) return ud_h;

    
    double beta = angle * PI / 180.0;
    return ud_h * cos(beta) + ud_v * sin(beta);
}


static double distributionParameter() {
    return 1.2;
}

static double slugHoldup(
    double us, double D, double rhoL, double rhoG,
    double muL, double sigma, double gravity
) {
    
    double Re_s = rhoL * fabs(us) * D / muL;
    double fs = frictionFactor(Re_s);
    double drho = rhoL - rhoG;
    double term1 = 2.0 * sqrt(0.4 * sigma / (drho * gravity));
    double term2 = pow((2.0 * fs / D) * us * us * us, 0.4);   
    double term3 = pow(rhoL / sigma, 0.6);                     

    double inner = term1 * term2 * term3 - 0.725;

    double alpha_s;
    if (inner <= 0.0) {
        alpha_s = 0.0;  
    } else {
        alpha_s = 0.058 * inner * inner;
    }

  
    double Rs = 1.0 - alpha_s;
    return std::max(0.48, std::min(1.0, Rs));
}


static double dispersedBubbleVelocity(
    double angle, double us,
    double sigma, double rhoL, double rhoG, double g
) {
    double beta = angle * PI / 180.0;

    
    double B;
    if (angle >= 80.0) {
        B = 1.2;
    } else if (angle <= 5.0) {
        B = 1.0;
    } else {
        B = 1.0 + 0.2 * sin(beta);  // interpolação suave
    }

    double u_inf = 1.54 * pow(sigma * g * (rhoL - rhoG) / (rhoL * rhoL), 0.25);
    double u0 = u_inf * sin(beta);

    return B * us + u0;
}


static double slugLength(double angle, double D) {
    double beta = angle * PI / 180.0;
    double ls_v = 20.0 * D;
    double ls_h = 30.0 * D;

    if (angle >= 80.0) return ls_v;
    if (angle <= 5.0) return ls_h;

    return ls_h + (ls_v - ls_h) * sin(beta);
}

// ==============================================================================
// 5. DERIVADA dhf/dz 
// ==============================================================================

/**
 * Calcula dhf/dz ou dδ/dz usando Eq. 21 de Taitel & Barnea
 */
static double filmDerivative_Eq21(
    double hf_or_delta,
    double angle,
    double D,
    double ut,
    double uL,
    double ub,
    double us,
    double Rs,
    double rhoL,
    double rhoG,
    double muL,
    double muG,
    double roughness,
    double gravity
) {
    double beta = angle * PI / 180.0;
    double sinB = sin(beta);
    double cosB = cos(beta);
    double alpha_s = 1.0 - Rs;

    // --- Geometria ---
    double Rf, Af, AG, Sf, SG, Si;
    double dRf_dh;
    bool isVertical = (angle >= 80.0);

    if (isVertical) {
        double delta = hf_or_delta;
        annularGeometry(D, delta, Rf, Af, AG, Sf, Si);
        SG = Si;  // para anular, SG = Si
        dRf_dh = dRf_ddelta_annular(D, delta);
    } else {
        double hf = hf_or_delta;
        stratifiedGeometry(D, hf, Rf, Af, AG, Sf, SG, Si);
        dRf_dh = dRf_dhf_stratified(D, hf);
    }

    double af = 1.0 - Rf;

    // Limites de segurança
    if (Rf < 0.001 || Rf > 0.999 || af < 0.001 || Af < 1e-12 || AG < 1e-12) {
        return 0.0;
    }

    // --- Velocidades no filme ---
    double vf = (ut - uL) * Rs / Rf;
    double uf = ut - vf;

    double vG = (ut - ub) * alpha_s / af;
    double uG_val = ut - vG;


    // --- Números de Reynolds ---
    double Dh_L = 4.0 * Af / Sf;
    double ReL = rhoL * fabs(uf) * Dh_L / muL;

    double Dh_G;
    if (isVertical) {
        Dh_G = 4.0 * AG / Si;
    } else {
        Dh_G = 4.0 * AG / (SG + Si);
    }
    double ReG = rhoG * fabs(uG_val) * Dh_G / muG;

    // --- Fatores de atrito ---
    double ff = frictionFactor(ReL);
    double fG = frictionFactor(ReG);
    double fi = interfacialFriction(angle, hf_or_delta, D);

    // --- Tensões de cisalhamento  ---
    double tau_f = ff * rhoL * fabs(uf) * uf / 2.0;         
    double tau_G = fG * rhoG * fabs(uG_val) * uG_val / 2.0; 
    double tau_i = fi * rhoG * fabs(uG_val - uf) * (uG_val - uf) / 2.0; 

    // --- Numerador da Eq. 21 ---
    double num = tau_f * Sf / Af
               - tau_G * SG / AG
               - tau_i * Si * (1.0 / Af + 1.0 / AG)
               + (rhoL - rhoG) * gravity * sinB;

    // --- Denominador da Eq. 21 ---
    double term_L = rhoL * vf * (ut - uL) * Rs / (Rf * Rf) * dRf_dh;
    double term_G = rhoG * vG * (ut - ub) * alpha_s / (af * af) * dRf_dh;

    double den = (rhoL - rhoG) * gravity * cosB - term_L - term_G;

    // Evitar denominador nulo
    if (fabs(den) < 1e-12) {
        return 0.0;
    }

    return num / den;
}

// ==============================================================================
// 6. NÍVEL CRÍTICO hc e NÍVEL DE EQUILÍBRIO hE
// ==============================================================================

/**
 * Encontra nível de equilíbrio hE (numerador de Eq. 21 = 0, i.e. Eq. 24)
 * Resolve por bisseção
 */
static double findEquilibriumLevel(
    double angle, double D, double ut, double uL, double ub,
    double us, double Rs, double rhoL, double rhoG,
    double muL, double muG, double roughness, double gravity
) {
    double lo = 0.01 * D;
    double hi = (angle >= 80.0) ? 0.49 * D : 0.99 * D;

    // Bisseção: encontrar onde filmDerivative numerador = 0
    for (int iter = 0; iter < 100; iter++) {
        double mid = (lo + hi) / 2.0;
        double deriv = filmDerivative_Eq21(mid, angle, D, ut, uL, ub, us, Rs,
                                           rhoL, rhoG, muL, muG, roughness, gravity);
        // O numerador muda de sinal quando hf passa por hE
        // Para simplificar, usamos o valor da derivada completa
        if (fabs(deriv) < 1e-10 || (hi - lo) < 1e-8 * D) break;

        // Teste: para vertical, dhf/dz > 0 quando hf < hE (filme afina → equilíbrio)
        double deriv_lo = filmDerivative_Eq21(lo, angle, D, ut, uL, ub, us, Rs,
                                              rhoL, rhoG, muL, muG, roughness, gravity);
        if (deriv * deriv_lo > 0) {
            lo = mid;
        } else {
            hi = mid;
        }
    }
    return (lo + hi) / 2.0;
}

/**
 * Encontra nível crítico hc (denominador de Eq. 21 = 0)
 */
static double findCriticalLevel(
    double angle, double D, double ut, double uL, double ub,
    double Rs, double rhoL, double rhoG, double gravity
) {
    if (angle >= 80.0) return -1.0;  // Vertical: sem nível crítico

    double beta = angle * PI / 180.0;
    double cosB = cos(beta);
    double alpha_s = 1.0 - Rs;

    auto denomFunc = [&](double hf) -> double {
        double Rf, Af, AG, Sf, SG, Si;
        stratifiedGeometry(D, hf, Rf, Af, AG, Sf, SG, Si);
        double af = 1.0 - Rf;
        if (Rf < 0.001 || af < 0.001) return 1.0;

        double vf = (ut - uL) * Rs / Rf;
        double vG = (ut - ub) * alpha_s / af;
        double dRf = dRf_dhf_stratified(D, hf);

        double term_L = rhoL * vf * (ut - uL) * Rs / (Rf * Rf) * dRf;
        double term_G = rhoG * vG * (ut - ub) * alpha_s / (af * af) * dRf;

        return (rhoL - rhoG) * gravity * cosB - term_L - term_G;
    };

    // Varre o intervalo procurando todas as trocas de sinal
    double hf_min = 0.01 * D;
    double hf_max = 0.99 * D;
    int n_steps = 200;
    double step = (hf_max - hf_min) / n_steps;
    std::vector<double> roots;
    double prev_hf = hf_min;
    double prev_val = denomFunc(prev_hf);
    for (int i = 1; i <= n_steps; ++i) {
        double curr_hf = hf_min + i * step;
        double curr_val = denomFunc(curr_hf);
        if (prev_val * curr_val < 0.0) {
            // Troca de sinal: refina raiz por bisseção
            double lo = prev_hf, hi = curr_hf;
            double val_lo = prev_val, val_hi = curr_val;
            for (int iter = 0; iter < 100; ++iter) {
                double mid = 0.5 * (lo + hi);
                double val_mid = denomFunc(mid);
                if (fabs(val_mid) < 1e-12 || (hi - lo) < 1e-8 * D) {
                    roots.push_back(mid);
                    break;
                }
                if (val_mid * val_lo > 0) {
                    lo = mid;
                    val_lo = val_mid;
                } else {
                    hi = mid;
                    val_hi = val_mid;
                }
            }
        }
        prev_hf = curr_hf;
        prev_val = curr_val;
    }
    // Seleciona a menor raiz positiva (física)
    double hc = -1.0;
    for (double r : roots) {
        if (r > 0.0 && (hc < 0.0 || r < hc)) hc = r;
    }
    return hc;
}


static double levelFromHoldup(double Rs, double D, bool isVertical) {
    if (isVertical) {
        // Anular
        return (D / 2.0) * (1.0 - sqrt(1.0 - Rs));
    }

    // Estratificado
    double lo = 0.0, hi = D;
    for (int iter = 0; iter < 100; iter++) {
        double mid = (lo + hi) / 2.0;
        double eta = 2.0 * mid / D - 1.0;
        eta = std::max(-0.999, std::min(0.999, eta));
        double theta = acos(eta);
        double Rf_test = (1.0 / PI) * (PI - theta + eta * sqrt(1.0 - eta * eta));

        if (fabs(Rf_test - Rs) < 1e-10 || (hi - lo) < 1e-10 * D) {
            return mid;
        }

        if (Rf_test < Rs) {
            lo = mid;
        } else {
            hi = mid;
        }
    }
    return (lo + hi) / 2.0;
}

// ==============================================================================
// 7. INTEGRAÇÃO DO PERFIL DO FILME - Runge-Kutta 4ª Ordem
// ==============================================================================

static void integrateFilmProfile(
    double angle, double D, double ut, double uL, double ub,
    double us, double Rs, double uLs,
    double rhoL, double rhoG, double muL, double muG,
    double roughness, double gravity,
    double ls, FilmProfile& profile
) {
    profile.z.clear();
    profile.hf.clear();
    profile.Rf.clear();
    profile.uf.clear();
    profile.uG.clear();
    profile.converged = false;

    bool isVertical = (angle >= 80.0);
    double A = PI * D * D / 4.0;

    // Nível correspondente a Rs (= nível na cauda do slug)
    double hs = levelFromHoldup(Rs, D, isVertical);

    // -------------------------------------------------------------------------
    // Determinação do ponto de partida hfi
    // -------------------------------------------------------------------------
    double hfi;

    if (isVertical) {
        // Vertical: sem nível crítico, iniciar em hs
        hfi = hs;
    } else {
        // Horizontal/inclinado: verificar nível crítico hc
        double hc = findCriticalLevel(angle, D, ut, uL, ub, Rs,
                                      rhoL, rhoG, gravity);

        if (hc > 0.0 && hc < hs) {
            // hc existe e é menor que hs → salto hidráulico na cauda do slug
            // Integração começa em hc 
            hfi = hc;
        } else {
            // hc não existe ou hc >= hs → sem singularidade, iniciar em hs
            hfi = hs;
        }

        // Verificar se hfi < hE → filme já está em equilíbrio 
        double hE = findEquilibriumLevel(angle, D, ut, uL, ub, us, Rs,
                                         rhoL, rhoG, muL, muG, roughness, gravity);
        if (hfi < hE) {
            hfi = hE;
        }
    }

    // Limites
    double hf_min = 0.001 * D;
    double hf_max = isVertical ? 0.49 * D : 0.99 * D;
    hfi = std::max(hf_min, std::min(hf_max, hfi));

    // Passo de integração
    double dz = D / 100.0;
    double z = 0.0;
    double hf = hfi;
    double z_max = 2000.0 * D;
    double Rf_integral = 0.0;

    // Integração RK4
    while (z < z_max) {
        // Armazenar estado
        double Rf_cur, Af_cur, AG_cur, Sf_cur, SG_cur, Si_cur;
        if (isVertical) {
            annularGeometry(D, hf, Rf_cur, Af_cur, AG_cur, Sf_cur, Si_cur);
        } else {
            stratifiedGeometry(D, hf, Rf_cur, Af_cur, AG_cur, Sf_cur, SG_cur, Si_cur);
        }

        double alpha_s = 1.0 - Rs;
        double af_cur = 1.0 - Rf_cur;
        double uf_cur = ut - (ut - uL) * Rs / Rf_cur;
        double uG_cur = (af_cur > 0.001) ? (us - uf_cur * Rf_cur) / af_cur : us;

        profile.z.push_back(z);
        profile.hf.push_back(hf);
        profile.Rf.push_back(Rf_cur);
        profile.uf.push_back(uf_cur);
        profile.uG.push_back(uG_cur);

        // Acumular ∫Rf dz (trapezoidal)
        if (profile.Rf.size() > 1) {
            size_t n = profile.Rf.size();
            Rf_integral += 0.5 * (profile.Rf[n - 1] + profile.Rf[n - 2]) * dz;
        }

        // Verificar balanço de massa 
        double lf_current = z;
        double lu_current = ls + lf_current;
        double rhs = uL * Rs * ls + ut * Rf_integral - (ut - uL) * Rs * lf_current;
        double lhs = uLs * lu_current;

        if (lf_current > dz && lhs >= rhs) {
            profile.converged = true;
            profile.filmLength = lf_current;
            return;
        }

        // RK4
        auto F = [&](double h) {
            return filmDerivative_Eq21(h, angle, D, ut, uL, ub, us, Rs,
                                       rhoL, rhoG, muL, muG, roughness, gravity);
        };

        double k1 = F(hf);
        double k2 = F(std::max(hf_min, std::min(hf_max, hf + k1 * dz / 2.0)));
        double k3 = F(std::max(hf_min, std::min(hf_max, hf + k2 * dz / 2.0)));
        double k4 = F(std::max(hf_min, std::min(hf_max, hf + k3 * dz)));

        double dhf = (k1 + 2.0 * k2 + 2.0 * k3 + k4) * dz / 6.0;
        hf += dhf;
        hf = std::max(hf_min, std::min(hf_max, hf));

        z += dz;

        // Verificar se atingiu equilíbrio
        if (fabs(dhf) < 1e-8 * D && z > 2.0 * dz) {
            // Filme em equilíbrio: calcular lf restante analiticamente
            double Rf_eq = Rf_cur;
            double uf_eq = uf_cur;

           
            double denom_43 = uLs - uf_eq * Rf_eq;
            if (fabs(denom_43) > 1e-12) {
                double lu_eq = ls * (uL * Rs - uf_eq * Rf_eq) / denom_43;
                double lf_eq = lu_eq - ls;
                if (lf_eq > z) {
                    // Preencher restante com equilíbrio
                    double z_remaining = lf_eq - z;
                    int n_steps = (int)(z_remaining / dz) + 1;
                    for (int i = 0; i < n_steps && z < lf_eq; i++) {
                        z += dz;
                        profile.z.push_back(z);
                        profile.hf.push_back(hf);
                        profile.Rf.push_back(Rf_eq);
                        profile.uf.push_back(uf_eq);
                        profile.uG.push_back(uG_cur);
                    }
                    profile.converged = true;
                    profile.filmLength = lf_eq;
                    return;
                }
            }

            profile.converged = true;
            profile.filmLength = z;
            return;
        }
    }

    // Atingiu z_max sem convergir
    profile.converged = false;
    profile.filmLength = z;
}

// ==============================================================================
// 8. FUNÇÃO PRINCIPAL
// ==============================================================================

void calculateSlugFlow(
    double angle,
    double diameter,
    double uLs,
    double uGs,
    double rhoL,
    double rhoG,
    double muL,
    double muG,
    double sigma,
    double roughness,
    double gravity,
    SlugFlowResults& results
) {
    double D = diameter;
    double beta = angle * PI / 180.0;
    bool isVertical = (angle >= 80.0);

    // ==========================================================================
    // PASSO 1: Variáveis auxiliares 
    // ==========================================================================

    // us - velocidade da mistura
    results.us = uLs + uGs;
    double us = results.us;

    // C e ud - velocidade translacional 
    results.C = distributionParameter();
    results.ud = driftVelocity(angle, D, gravity);
    results.ut = results.C * us + results.ud;
    double ut = results.ut;

    // Rs - holdup na golfada 
    results.Rs = slugHoldup(us, D, rhoL, rhoG, muL, sigma, gravity);
    double Rs = results.Rs;
    results.alpha_s = 1.0 - Rs;
    double alpha_s = results.alpha_s;

    // ub - velocidade das bolhas dispersas 
    results.ub = dispersedBubbleVelocity(angle, us, sigma, rhoL, rhoG, gravity);
    double ub = results.ub;

    // uL - velocidade do líquido na golfada
    results.uL = (us - ub * alpha_s) / Rs;
    double uL = results.uL;

    // ls - comprimento da golfada
    results.ls = slugLength(angle, D);
    double ls = results.ls;

    // ==========================================================================
    // PASSO 2: Fração de vazio média 
    // ==========================================================================
    results.alpha_u = (uGs - ub * alpha_s + ut * alpha_s) / ut;
    results.alpha_u = std::max(0.0, std::min(1.0, results.alpha_u));

    // ==========================================================================
    // PASSO 3: Integração do perfil do filme 
    // ==========================================================================
    integrateFilmProfile(angle, D, ut, uL, ub, us, Rs, uLs,
                         rhoL, rhoG, muL, muG, roughness, gravity,
                         ls, results.profile);

    results.lf = results.profile.filmLength;
    results.lu = ls + results.lf;
    results.freq = (results.lu > 0.0) ? results.ut / results.lu : 0.0;

    // Propriedades no final do filme
    if (!results.profile.hf.empty()) {
        results.hfe = results.profile.hf.back();
        results.Rfe = results.profile.Rf.back();
        results.ufe = results.profile.uf.back();
    }

    // Nível de equilíbrio (para referência)
    results.hE = findEquilibriumLevel(angle, D, ut, uL, ub, us, Rs,
                                      rhoL, rhoG, muL, muG, roughness, gravity);

    // Info
    results.flowGeometry = isVertical ? "Vertical (annular)" : "Stratified";
}
