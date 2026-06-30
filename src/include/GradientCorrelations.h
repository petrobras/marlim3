// ==============================================================================
// GradientCorrelations.h
// Pressure-gradient correlations for multiphase flow.
// ==============================================================================
#ifndef GRADIENTCORRELATIONS_H
#define GRADIENTCORRELATIONS_H
#define _USE_MATH_DEFINES

#include "FerramentasNumericas.h"
#include "Log.h"
#include "celula3.h"
#include <math.h>

// ------------------------------------------------------------------------------
// Physical and Mathematical Constants
// ------------------------------------------------------------------------------
const double g_in = 32.174; // Gravitational acceleration in ft/s2.

// ------------------------------------------------------------------------------
// General Auxiliary Functions
// ------------------------------------------------------------------------------
/*!
 * Calculate the Darcy friction factor using a Moody-diagram formulation.
 *
 * \param reynolds Reynolds number.
 * \param roughness Relative roughness.
 * \param frictionFactorValue Calculated friction-factor value.
 */
void frictionFactor(double reynolds, double roughness, double &frictionFactorValue);

/*!
 * Accelerate convergence of an iterative calculation.
 *
 * \param currentValue Current iterative value.
 * \param previousValue Value from the previous iteration.
 * \param convergeStatus Convergence-state flag.
 * \param tolerance Convergence tolerance.
 * \param auxValueA First auxiliary convergence value.
 * \param auxValueB Second auxiliary convergence value.
 */
void accelerateConvergence(double &currentValue, double &previousValue, unsigned char &convergeStatus,
                           double tolerance, double &auxValueA, double &auxValueB);

// ------------------------------------------------------------------------------
// Correlation Dispatcher
// ------------------------------------------------------------------------------

/*!
 * Extract the required data from a cell and execute the selected
 * pressure-gradient correlation.
 *
 * \param celula Array of production-system cells.
 * \param i Index of the cell being evaluated.
 * \param inter Iteration index or iterative-control value.
 * \param AceleraConvergPerm Convergence-acceleration configuration.
 * \param correlacao Correlation identifier: 0 = Poettmann-Carpenter,
 *                   1 = Baxendell-Thomas, 2 = Fancher-Brown, and so on.
 * \param holdup Calculated liquid holdup.
 * \param frictionGrad Frictional pressure gradient.
 * \param gravityGrad Gravitational pressure gradient.
 * \param totalGrad Total pressure gradient.
 * \param reynolds Calculated Reynolds number.
 * \param flowType Calculated flow-pattern or flow-type identifier.
 */
void executarCorrelacao(Cel *celula, int i, int inter, int AceleraConvergPerm,
                        int correlacao, // Correlation identifier: 0 = Poettmann, 1 = Baxendell, 2 = Fancher, etc.
                        double &holdup, double &frictionGrad, double &gravityGrad, double &totalGrad,
                        double &reynolds, unsigned char &flowType);

// ------------------------------------------------------------------------------
// Pressure-Gradient Correlations
// ------------------------------------------------------------------------------
/*!
 * Calculate the pressure gradient using the Poettmann-Carpenter correlation.
 */
void poettmannCarpenter(double angle, double diameter, double velocity, double liquidFraction,
                        double gasDensity, double liquidDensity, double gasViscosity,
                        double liquidViscosity, double roughness, double &holdup,
                        double &frictionGrad, double &gravityGrad, double &totalGrad, double &reynolds,
                        unsigned char &flowType);

/*!
 * Calculate the pressure gradient using the Baxendell-Thomas correlation.
 */
void baxendellThomas(double angle, double diameter, double velocity, double liquidFraction,
                     double gasDensity, double liquidDensity, double gasViscosity,
                     double liquidViscosity, double roughness, double &holdup,
                     double &frictionGrad, double &gravityGrad, double &totalGrad, double &reynolds,
                     unsigned char &flowType);

/*!
 * Calculate the pressure gradient using the Fancher-Brown correlation.
 */
void fancherBrown(double angle, double diameter, double productionRate, double velocity,
                  double liquidFraction, double gasDensity, double liquidDensity,
                  double gasViscosity, double liquidViscosity, double roughness,
                  double &holdup, double &frictionGrad, double &gravityGrad, double &totalGrad,
                  double &reynolds, unsigned char &flowType);

/*!
 * Calculate the pressure gradient using the Hagedorn-Brown correlation.
 */
void hagedornBrown(double angle, double diameter, double roughness, double pressure, double velocity,
                   double liquidFraction, double gasDensity, double liquidDensity,
                   double gasViscosity, double liquidViscosity, double surfaceTension,
                   double temperature, double compressibilityFactor,
                   double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad,
                   double &totalGrad, double &reynolds, unsigned char &flowType,
                   unsigned char &criticalFlag, unsigned char &convergeFlag);

/*!
 * Calculate the pressure gradient using the Duns-Ros correlation.
 */
void dunsRos(double angle, double diameter, double roughness, double pressure, double velocity,
             double liquidFraction, double gasDensity, double liquidDensity,
             double gasViscosity, double liquidViscosity, double surfaceTension,
             double temperature, double compressibilityFactor,
             double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad,
             double &totalGrad, double &reynolds, unsigned char &flowType,
             unsigned char &criticalFlag, unsigned char &convergeFlag);

/*!
 * Calculate the pressure gradient using the Orkiszewski correlation.
 */
void orkiszewski(double angle, double diameter, double roughness,
                 double pressure, double velocity, double liquidFraction, double gasDensity,
                 double liquidDensity, double gasViscosity, double liquidViscosity,
                 double surfaceTension, double temperature, double compressibilityFactor, double waterFraction,
                 double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad,
                 double &totalGrad, double &reynolds, unsigned char &flowType,
                 unsigned char &criticalFlag, unsigned char &convergeFlag, unsigned char &isigFlag);

/*!
 * Calculate the pressure gradient using the Beggs-Brill correlation.
 */
void beggsAndBrill(double angle, double diameter, double roughness, double pressure, double velocity,
                   double liquidFraction, double gasDensity, double liquidDensity,
                   double gasViscosity, double liquidViscosity, double surfaceTension,
                   double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad,
                   double &totalGrad, double &reynolds, unsigned char &flowType,
                   unsigned char palmerFlag, unsigned char &criticalFlag);

/*!
 * Calculate the pressure gradient using the Mukherjee-Brill correlation.
 */
void mukherjeeeBrill(double angle, double diameter, double roughness, double pressure,
                     double velocity, double liquidFraction, double gasDensity,
                     double liquidDensity, double gasViscosity, double liquidViscosity,
                     double surfaceTension, double temperature, double compressibilityFactor,
                     double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad,
                     double &totalGrad, double &reynolds, unsigned char &flowPattern,
                     unsigned char &criticalFlag, unsigned char &convergeFlag);

/*!
 * Calculate the pressure gradient using the Aziz correlation.
 */
void aziz(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity, double surfaceTension,
          double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad, double &totalGrad,
          double &reynolds, unsigned char &flowPattern, unsigned char &criticalFlag);

/*!
 * Calculate the pressure gradient using the Gray correlation.
 */
void gray(double angle, double diameter, double roughness, double pressure, double velocity,
          double liquidFraction, double waterFraction, double gasDensity, double liquidDensity,
          double gasViscosity, double liquidViscosity, double oilSurfaceTension, double waterSurfaceTension,
          double &holdup, double &frictionGrad, double &gravityGrad, double &accelGrad,
          double &totalGrad, double &reynolds, unsigned char &flowPattern, unsigned char &criticalFlag);

/*!
 * Calculate the pressure gradient using the Oliemans correlation.
 */
void olie(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity,
          double surfaceTension, double temperature, double compressibilityFactor,
          double &holdup, double &frictionGrad, double &gravityGrad,
          double &totalGrad, double &reynolds, unsigned char &flowPattern, unsigned char correlationFlag,
          unsigned char &criticalFlag);

/*!
 * Calculate the pressure gradient using the Dukler correlation.
 */
void duklerCorrelation(double angle, double diameter, double roughness, double pressure,
                       double velocity, double liquidFraction, double gasDensity,
                       double liquidDensity, double gasViscosity, double liquidViscosity,
                       double surfaceTension, double temperature, double compressibilityFactor,
                       double &holdup, double &frictionGrad, double &gravityGrad,
                       double &accelGrad, double &totalGrad, double &reynolds, unsigned char &flowPattern,
                       unsigned char correlationFlag, unsigned char &transitionFlag,
                       unsigned char &criticalFlag);

// ------------------------------------------------------------------------------
// Correlation-Specific Auxiliary Functions
// ------------------------------------------------------------------------------

/*!
 * Interpolate a liquid-holdup value from a correlation table.
 *
 * \param columnIndex Table-column index.
 * \param inputValue Independent-variable value.
 * \param outputValue Interpolated holdup value.
 * \param errorMsg Buffer receiving an error message.
 * \param errorFlag Error-state flag.
 */
void holdupInterpolation(int columnIndex, double inputValue, double &outputValue, char *errorMsg, unsigned char &errorFlag);

/*!
 * Calculate liquid holdup and mixture properties from the liquid fraction
 * and dimensionless correlation groups.
 *
 * \param liquidFraction No-slip liquid fraction.
 * \param liquidViscosity Liquid viscosity.
 * \param liquidDensity Liquid density.
 * \param gasDensity Gas density.
 * \param diameter Internal pipe diameter.
 * \param velocity Mixture velocity.
 * \param pressure Absolute pressure.
 * \param liquidNumber Liquid number.
 * \param liquidViscNumber Liquid-viscosity number.
 * \param gasVelNumber Gas-velocity number.
 * \param tubeNumber Tube-diameter number.
 * \param holdup Calculated liquid holdup.
 * \param reynoldsMixture Calculated mixture Reynolds number.
 * \param densityMixture Calculated mixture density.
 * \param correlationFlag Correlation-selection flag.
 */
void holdupLiquidFraction(double liquidFraction, double liquidViscosity, double liquidDensity,
                          double gasDensity, double diameter, double velocity, double pressure,
                          double liquidNumber, double liquidViscNumber, double gasVelNumber, double tubeNumber,
                          double &holdup, double &reynoldsMixture, double &densityMixture,
                          unsigned char correlationFlag);

/*!
 * Calculate liquid holdup using the Minami correlation.
 */
void holdupMinami(double angle, double diameter, double pressure, double velocity,
                  double liquidFraction, double gasVelNumber, double liquidViscNumber,
                  double tubeNumber, double liquidNumber, unsigned char correlationFlag,
                  unsigned char &transitionFlag, unsigned char &flowPattern, double &holdup);

/*!
 * Determine the Mukherjee-Brill flow pattern.
 */
void mukherjeeeBrillFlowPattern(double liquidViscNumber, double gasVelNumber, double liquidNumber,
                                double angle, unsigned char &flowPattern);

/*!
 * Calculate liquid holdup using the Mukherjee-Brill correlation.
 */
void mukherjeeeBrillHoldup(double liquidViscNumber, double gasVelNumber, double liquidNumber,
                           double angle, unsigned char flowPattern, double &holdup);

/*!
 * Calculate the stratified-flow pressure gradient used by the
 * Mukherjee-Brill correlation.
 */
void mukherjeeeBrillStratifiedFlow(double angleRad, double velocity, double liquidFraction,
                                   double holdup, double diameter, double roughness, double gasDensity,
                                   double gasViscosity, double &frictionGrad, double &gravityGrad,
                                   double &totalGrad, unsigned char &convergeFlag);

/*!
 * Determine the Aziz flow-regime map and transition boundaries.
 */
void azizRegimeMap(double liquidSupVel, double gasSupVel, double liquidDensity, double gasDensity,
                   double surfaceTension, double liquidFraction,
                   double &bubbleToSlugBoundary, double &slugToTransitionBoundary, double &transitionToMistBoundary,
                   double &flowMapCoordinateX, double &flowMapCoordinateY, unsigned char &flowPattern);

/*!
 * Calculate the density and geometric quantities required by the Aziz
 * correlation.
 */
void azizDensity(double gasSupVel, double velocity, double surfaceTension,
                 double liquidDensity, double gasDensity, double diameter, double liquidViscosity,
                 double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary, unsigned char flowPattern,
                 double &holdup, double &bubbleLength, double &slugLength,
                 double &slipDensity, double &slugLiquidDensity);

/*!
 * Calculate the frictional pressure gradient used by the Aziz correlation.
 */
void azizFriction(double diameter, double velocity, double gasSupVel,
                  double liquidDensity, double gasDensity, double liquidViscosity,
                  double gasViscosity, double surfaceTension, double roughness,
                  double mixtureDensity, double slugMixtureDensity,
                  double bubbleLength, double slugLength,
                  double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary,
                  double &reynolds, unsigned char flowPattern,
                  double &frictionGrad);

#endif // GRADIENTCORRELATIONS_H