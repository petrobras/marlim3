#ifndef GRADIENTCORRELATIONS_H
#define GRADIENTCORRELATIONS_H
#define _USE_MATH_DEFINES
#include <math.h>
#include "celula3.h"
#include "Log.h"
#include "FerramentasNumericas.h"

// ==============================================================================
// GradientCorrelations.h
// Arquivo header para correlações de gradiente de pressão em escoamento multifásico
// ==============================================================================

// ------------------------------------------------------------------------------
// Variáveis Globais
// ------------------------------------------------------------------------------

// Controle do modo de cálculo para injeção
//extern bool calcInj;

// Constantes físicas e matemáticas
//extern const double pi;              // Constante matemática π
const double g_in=32.174;            // Aceleração da gravidade em ft/s² (sistema inglês)

// ------------------------------------------------------------------------------
// Funções Auxiliares
// ------------------------------------------------------------------------------

// Cálculo do fator de atrito usando diagrama de Moody
void frictionFactor(double reynolds, double roughness, double& frictionFactorValue);

long double interpolate(const std::vector<double>& xValues, const std::vector<double>& yValues,
                   double xTarget, int degree, int numPoints);

long double interpolate2D(const std::vector<double>& verticalPoints,
                     const std::vector<double>& horizontalPoints,
                     const std::vector<double>& matrixValues,
                     int numVertical, int numHorizontal,
                     int verticalDegree, int horizontalDegree,
                     double verticalTarget, double horizontalTarget);


// Aceleração de convergência para cálculos iterativos
void accelerateConvergence(double& currentValue, double& previousValue, unsigned char& convergeStatus, 
                          double tolerance, double& auxValueA, double& auxValueB);

// ------------------------------------------------------------------------------
// Função para Execução de Correlações
// ------------------------------------------------------------------------------

// Função que extrai parâmetros da estrutura celula e executa correlação selecionada
void executarCorrelacao(Cel* celula, int i, int inter, int AceleraConvergPerm,
                       int correlacao,  // Número da correlação: 0=Poettmann, 1=Baxendell, 2=Fancher, etc.
                       double& holdup, double& frictionGrad, double& gravityGrad, double& totalGrad,
                       double& reynolds, unsigned char& flowType);

// ------------------------------------------------------------------------------
// Correlações de Gradiente de Pressão
// ------------------------------------------------------------------------------

// Correlação de Poettmann-Carpenter
void poettmannCarpenter(double angle, double diameter, double velocity, double liquidFraction,
                       double gasDensity, double liquidDensity, double gasViscosity,
                       double liquidViscosity, double roughness, double& holdup,
                       double& frictionGrad, double& gravityGrad, double& totalGrad, double& reynolds,
                       unsigned char& flowType);

// Correlação de Baxendell-Thomas
void baxendellThomas(double angle, double diameter, double velocity, double liquidFraction, 
                    double gasDensity, double liquidDensity, double gasViscosity, 
                    double liquidViscosity, double roughness, double& holdup,
                    double& frictionGrad, double& gravityGrad, double& totalGrad, double& reynolds, 
                    unsigned char& flowType);

// Correlação de Fancher-Brown
void fancherBrown(double angle, double diameter, double productionRate, double velocity, 
                 double liquidFraction, double gasDensity, double liquidDensity, 
                 double gasViscosity, double liquidViscosity, double roughness, 
                 double& holdup, double& frictionGrad, double& gravityGrad, double& totalGrad, 
                 double& reynolds, unsigned char& flowType);

// Correlação de Hagedorn-Brown
void hagedornBrown(double angle, double diameter, double roughness, double pressure, double velocity, 
                  double liquidFraction, double gasDensity, double liquidDensity, 
                  double gasViscosity, double liquidViscosity, double surfaceTension, 
                  double temperature, double compressibilityFactor,
                  double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                  double& totalGrad, double& reynolds, unsigned char& flowType, 
                  unsigned char& criticalFlag, unsigned char& convergeFlag);

// Correlação de Duns-Ros
void dunsRos(double angle, double diameter, double roughness, double pressure, double velocity, 
             double liquidFraction, double gasDensity, double liquidDensity, 
             double gasViscosity, double liquidViscosity, double surfaceTension,
             double temperature, double compressibilityFactor,
             double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
             double& totalGrad, double& reynolds, unsigned char& flowType, 
             unsigned char& criticalFlag, unsigned char& convergeFlag);

// Correlação de Orkiszewski
void orkiszewski(double angle, double diameter, double roughness, 
                double pressure, double velocity, double liquidFraction, double gasDensity, 
                double liquidDensity, double gasViscosity, double liquidViscosity, 
                double surfaceTension, double temperature, double compressibilityFactor, double waterFraction,
                double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                double& totalGrad, double& reynolds, unsigned char& flowType, 
                unsigned char& criticalFlag, unsigned char& convergeFlag, unsigned char& isigFlag);

// Correlação de Beggs e Brill
void beggsAndBrill(double angle, double diameter, double roughness, double pressure, double velocity, 
                   double liquidFraction, double gasDensity, double liquidDensity, 
                   double gasViscosity, double liquidViscosity, double surfaceTension,
                   double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                   double& totalGrad, double& reynolds, unsigned char& flowType, 
                   unsigned char palmerFlag, unsigned char& criticalFlag);

// Correlação de Mukherjee-Brill
void mukherjeeeBrill(double angle, double diameter, double roughness, double pressure, 
                    double velocity, double liquidFraction, double gasDensity, 
                    double liquidDensity, double gasViscosity, double liquidViscosity,
                    double surfaceTension, double temperature, double compressibilityFactor,
                    double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                    double& totalGrad, double& reynolds, unsigned char& flowPattern,
                    unsigned char& criticalFlag, unsigned char& convergeFlag);

// Correlação de Aziz
void aziz(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity, double surfaceTension,
          double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad, double& totalGrad,
          double& reynolds, unsigned char& flowPattern, unsigned char& criticalFlag);

// Correlação de Gray
void gray(double angle, double diameter, double roughness, double pressure, double velocity, 
          double liquidFraction, double waterFraction, double gasDensity, double liquidDensity, 
          double gasViscosity, double liquidViscosity, double oilSurfaceTension, double waterSurfaceTension,
          double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
          double& totalGrad, double& reynolds, unsigned char& flowPattern, unsigned char& criticalFlag);

// Correlação de Oliemans
void olie(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity,
          double surfaceTension, double temperature, double compressibilityFactor,
          double& holdup, double& frictionGrad, double& gravityGrad,
          double& totalGrad, double& reynolds, unsigned char& flowPattern, unsigned char correlationFlag,
          unsigned char& criticalFlag);

// Correlação de Dukler
void duklerCorrelation(double angle, double diameter, double roughness, double pressure, 
                      double velocity, double liquidFraction, double gasDensity, 
                      double liquidDensity, double gasViscosity, double liquidViscosity,
                      double surfaceTension, double temperature, double compressibilityFactor,
                      double& holdup, double& frictionGrad, double& gravityGrad,
                      double& accelGrad, double& totalGrad, double& reynolds, unsigned char& flowPattern,
                      unsigned char correlationFlag, unsigned char& transitionFlag, 
                      unsigned char& criticalFlag);

// ------------------------------------------------------------------------------
// Funções Auxiliares para Correlações Específicas
// ------------------------------------------------------------------------------

// Interpolação de holdup 
void holdupInterpolation(int columnIndex, double inputValue, double& outputValue, char* errorMsg, unsigned char& errorFlag);

// Cálculo de holdup para fração de líquido 
void holdupLiquidFraction(double liquidFraction, double liquidViscosity, double liquidDensity, 
                         double gasDensity, double diameter, double velocity, double pressure,
                         double liquidNumber, double liquidViscNumber, double gasVelNumber, double tubeNumber,
                         double& holdup, double& reynoldsMixture, double& densityMixture, 
                         unsigned char correlationFlag);

// Correlação de holdup de Minami 
void holdupMinami(double angle, double diameter, double pressure, double velocity, 
                 double liquidFraction, double gasVelNumber, double liquidViscNumber, 
                 double tubeNumber, double liquidNumber, unsigned char correlationFlag,
                 unsigned char& transitionFlag, unsigned char& flowPattern, double& holdup);

// Determinação do padrão de escoamento de Mukherjee-Brill
void mukherjeeeBrillFlowPattern(double liquidViscNumber, double gasVelNumber, double liquidNumber, 
                               double angle, unsigned char& flowPattern);

// Cálculo de holdup de Mukherjee-Brill
void mukherjeeeBrillHoldup(double liquidViscNumber, double gasVelNumber, double liquidNumber, 
                          double angle, unsigned char flowPattern, double& holdup);

// Cálculo de escoamento estratificado de Mukherjee-Brill
void mukherjeeeBrillStratifiedFlow(double angleRad, double velocity, double liquidFraction, 
                                 double holdup, double diameter, double roughness, double gasDensity, 
                                 double gasViscosity, double& frictionGrad, double& gravityGrad, 
                                 double& totalGrad, unsigned char& convergeFlag);

// Determinação do mapa de regime de Aziz
void azizRegimeMap(double liquidSupVel, double gasSupVel, double liquidDensity, double gasDensity, 
                  double surfaceTension, double liquidFraction,
                  double& bubbleToSlugBoundary, double& slugToTransitionBoundary, double& transitionToMistBoundary,
                  double& flowMapCoordinateX, double& flowMapCoordinateY, unsigned char& flowPattern);

// Cálculo de densidade de Aziz
void azizDensity(double gasSupVel, double velocity, double surfaceTension,
                 double liquidDensity, double gasDensity, double diameter, double liquidViscosity,
                 double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary, unsigned char flowPattern,
                 double& holdup, double& bubbleLength, double& slugLength,
                 double& slipDensity, double& slugLiquidDensity);

// Cálculo de atrito de Aziz
void azizFriction(double diameter, double velocity, double gasSupVel,
                  double liquidDensity, double gasDensity, double liquidViscosity,
                  double gasViscosity, double surfaceTension, double roughness,
                  double mixtureDensity, double slugMixtureDensity,
                  double bubbleLength, double slugLength,
                  double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary,
                  double& reynolds, unsigned char flowPattern,
                  double& frictionGrad);

#endif // GRADIENTCORRELATIONS_H
