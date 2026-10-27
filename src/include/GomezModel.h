// ==============================================================================
// GomezModel.h
// ==============================================================================
#pragma once
#include <cmath>
// ------------------------------------------------------------------------------
// Estruturas de entrada
// ------------------------------------------------------------------------------
struct GomezInput {
    double vSL;       // velocidade superficial do líquido [m/s]
    double vSG;       // velocidade superficial do gás [m/s]
    double rhoL;      // massa específica do líquido [kg/m³]
    double rhoG;      // massa específica do gás [kg/m³]
    double muL;       // viscosidade do líquido [Pa·s]
    double muG;       // viscosidade do gás [Pa·s]
    double sigma;     // tensão superficial [N/m]
    double diameter;  // diâmetro interno do tubo [m]
    double roughness; // rugosidade absoluta da parede [m]
    double angle;     // ângulo de inclinação [graus, 0=horizontal, 90=vertical]
    double gravity;   // aceleração da gravidade [m/s²]
    double pressure;  // pressão absoluta [Pa]
};
// ------------------------------------------------------------------------------
// Padrões de escoamento
// ------------------------------------------------------------------------------
enum class FlowPattern {
    StratifiedSmooth,
    StratifiedWavy,
    Slug,
    AnnularMist,
    Bubble,
    DispersedBubble
};
// ------------------------------------------------------------------------------
// Estruturas de resultado por padrão
// ------------------------------------------------------------------------------
struct StratifiedResult {
    double hL_over_d = 0.0;
    double HL        = 0.0;
    double vL        = 0.0;
    double vG        = 0.0;
    double dpdL_grav = 0.0;  // componente gravitacional [Pa/m]
    double dpdL_fric = 0.0;  // componente friccional    [Pa/m]
    bool   isWavy    = false;
    bool   converged = false;
};
struct SlugResult {

    // --- Relações de fechamento ---
    double LS    = 0.0;  // comprimento do pistão [m]
    double HL_LS = 0.0;  // holdup no pistão [-]
    double vTB   = 0.0;  // velocidade translacional da bolha de Taylor [m/s]
    double vGLS  = 0.0;  // velocidade do gás no pistão [m/s]
    double vLLS  = 0.0;  // velocidade do líquido no pistão [m/s]

    // --- Filme da bolha de Taylor ---
    double HL_TB = 0.0;  // holdup no filme [-]
    double vLTB  = 0.0;  // velocidade do líquido no filme [m/s]
    double vGTB  = 0.0;  // velocidade do gás no filme [m/s]

    // Geometria do filme — apenas um dos dois estará ativo,
    // dependendo de filmIsSymmetric.
    bool   filmIsSymmetric             = false; // true se θ ≥ 86°
    double symmetricFilmThicknessRatio = 0.0;   // espessura normalizada do filme anular (δ/d)
    double stratifiedFilmLiquidLevel   = 0.0;   // nível normalizado do líquido no filme (hLTB/d)

    // --- Slug unit ---
    double LU   = 0.0;  // comprimento da slug unit [m]
    double LF   = 0.0;  // comprimento do filme [m]
    double HL_U = 0.0;  // holdup médio da slug unit [-]

    
    //   β < 0        → Bolhas ou Bolhas dispersas (padrão degenerado)
    //   β > 1        → Anular (padrão degenerado)
    //   0 < β < 1    → Golfadas (regime esperado)
    double beta = 0.0;

    // --- Resultado ---
    double dpdL_grav = 0.0;  // componente gravitacional [Pa/m]
    double dpdL_fric = 0.0;  // componente friccional    [Pa/m]
    bool   converged = false;
};
struct AnnularResult {
    double delta_over_d = 0.0;
    double HL           = 0.0;
    double E            = 0.0;
    double dpdL_grav    = 0.0;  // componente gravitacional [Pa/m]
    double dpdL_fric    = 0.0;  // componente friccional    [Pa/m]
    bool   converged    = false;
};
struct BubbleResult {
    double HL        = 0.0;
    double dpdL_grav = 0.0;  // componente gravitacional [Pa/m]
    double dpdL_fric = 0.0;  // componente friccional    [Pa/m]
    bool   converged = false;
};
// ------------------------------------------------------------------------------
// Resultado consolidado
// ------------------------------------------------------------------------------
struct GomezResult {
    FlowPattern      pattern = FlowPattern::Slug;
    double           HL      = 0.0;
    double           dpdL_grav = 0.0;  // componente gravitacional [Pa/m]
    double           dpdL_fric = 0.0;  // componente friccional    [Pa/m]
    StratifiedResult stratified;
    SlugResult       slug;
    AnnularResult    annular;
    BubbleResult     bubble;
};
// ------------------------------------------------------------------------------
// Função principal
// ------------------------------------------------------------------------------
GomezResult calculateGomez(const GomezInput& input);