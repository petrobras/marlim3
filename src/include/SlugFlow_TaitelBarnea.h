#ifndef SLUGFLOW_TAITELBARNEA_H
#define SLUGFLOW_TAITELBARNEA_H

// ==============================================================================
// SlugFlow_TaitelBarnea.h
// ==============================================================================

#include <vector>
#include <string>
#include <cmath>

// ==============================================================================
// ESTRUTURAS DE DADOS
// ==============================================================================

/**
 * Perfil do filme ao longo de z (saída da integração da Eq. 21)
 */
struct FilmProfile {
    std::vector<double> z;       // Posição axial desde o tail da golfada [m]
    std::vector<double> hf;      // Nível (estratificado) ou espessura δ (anular) [m]
    std::vector<double> Rf;      // Holdup do filme [-]
    std::vector<double> uf;      // Velocidade do líquido no filme [m/s]
    std::vector<double> uG;      // Velocidade do gás [m/s]
    double filmLength;           // lf [m]
    bool converged;
};

/**
 * Resultados completos do modelo de slug flow
 */
struct SlugFlowResults {
    // Variáveis auxiliares (Seção II.E)
    double C;                    // Parâmetro de distribuição [-]
    double ud;                   // Velocidade de drift [m/s]
    double ut;                   // Velocidade translacional [m/s] (Eq. 37)
    double ub;                   // Velocidade bolhas dispersas [m/s] (Eq. 38)
    double Rs;                   // Holdup na golfada [-] (Eq. 58)
    double us;                   // Velocidade da mistura [m/s]
    double uL;                   // Velocidade do líquido na golfada [m/s]

    // Geometria do slug unit
    double ls;                   // Comprimento da golfada [m]
    double lf;                   // Comprimento do filme [m]
    double lu;                   // Comprimento do slug unit [m]
    double freq;                 // Frequência da golfada [Hz]

    // Filme (valores finais / equilíbrio)
    double hfe;                  // Nível no final do filme [m]
    double Rfe;                  // Holdup no final [-]
    double ufe;                  // Velocidade filme final [m/s]
    double hE;                   // Nível de equilíbrio [m]

    // Fração de vazio
    double alpha_u;              // Fração de vazio média do slug unit [-] (Eq. 9)
    double alpha_s;              // Fração de vazio na golfada [-]

    // Perfil detalhado
    FilmProfile profile;

    // Info
    std::string flowGeometry;    // "Vertical (annular)" ou "Stratified"
};

// ==============================================================================
// FUNÇÃO PRINCIPAL
// ==============================================================================

void calculateSlugFlow(
    double angle,       // β [graus] (0=horizontal, 90=vertical)
    double diameter,    // D [m]
    double uLs,         // Vel. superficial líquido [m/s]
    double uGs,         // Vel. superficial gás [m/s]
    double rhoL,        // Densidade líquido [kg/m³]
    double rhoG,        // Densidade gás [kg/m³]
    double muL,         // Viscosidade líquido [Pa·s]
    double muG,         // Viscosidade gás [Pa·s]
    double sigma,       // Tensão superficial [N/m]
    double roughness,   // Rugosidade relativa ε/D [-]
    double gravity,     // g [m/s²]
    SlugFlowResults& results
);

#endif // SLUGFLOW_TAITELBARNEA_H
