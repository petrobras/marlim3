//Petroleo Brasileiro S.A.
#include "GradientCorrelations.h"
#include "CorrelationFits.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include <stdexcept>

using std::vector;


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
- Turbulent regime (Re > 2100): Iterative Serghides approximation for the
  implicit Colebrook-White equation

Variable dictionary:
- reynolds           : Reynolds number (dimensionless) - input
- roughness          : Relative roughness (epsilon/D, dimensionless) - input
- frictionFactorValue: Moody-Darcy friction factor (dimensionless) - output
- normalizedRoughness: Normalized roughness (roughness/3.7)
- reynoldsProduct    : Product of the Reynolds number and normalized roughness
- logRoughness       : Logarithm of the normalized roughness
- termA, termB, termC: Auxiliary terms used in the Serghides approximation
==============================================================================
*/

void frictionFactor(double reynolds, double roughness, double& frictionFactorValue) {
    
    // Flow Regime Constants
    static constexpr double LAMINAR_TRANSITION_REYNOLDS = 2100.0;  // Laminar-turbulent transition
    static constexpr double LAMINAR_FRICTION_COEFFICIENT = 64.0;   // Hagen-Poiseuille coefficient
    
    // Colebrook-White Equation Constants
    static constexpr double COLEBROOK_ROUGHNESS_FACTOR = 3.7;      // Roughness normalization factor
    static constexpr double COLEBROOK_CONSTANT_A = 12.0;           // First iteration constant
    static constexpr double COLEBROOK_CONSTANT_B = 5.02;           // Second iteration constant
    static constexpr double FRICTION_MULTIPLIER = 0.25;            // Friction factor multiplier (1/4)
    
    // Convergence Criteria
    static constexpr double CONVERGENCE_TOLERANCE = 1.0E-06;       // Iteration convergence threshold
    
    // Laminar Flow Regime (Re ≤ 2100)
    // Direct analytical solution: f = 64/Re
    if (reynolds <= LAMINAR_TRANSITION_REYNOLDS) {
        frictionFactorValue = LAMINAR_FRICTION_COEFFICIENT / reynolds;
        return;
    }
    
    // Turbulent Flow Regime (Re > 2100)
    // Serghides approximation for Colebrook-White implicit equation:
    // 1/√f = -2.0 * log10(ε/D/3.7 + 2.51/(Re*√f))
    // Iterative solution using successive substitution with Aitken acceleration
    
    // Step 1: Normalize roughness
    const double normalizedRoughness = roughness / COLEBROOK_ROUGHNESS_FACTOR;
    const double reynoldsProduct = reynolds * normalizedRoughness;
    const double logRoughness = log10(normalizedRoughness);
    
    // Step 2: First approximation (termA)
    const double termA = log10(1.0 + COLEBROOK_CONSTANT_A / reynoldsProduct);
    
    // Step 3: Second approximation (termB)
    const double termB = log10(1.0 - COLEBROOK_CONSTANT_B * (termA + logRoughness) / reynoldsProduct);
    
    // Step 4: Third approximation (termC)
    const double termC = log10(1.0 - COLEBROOK_CONSTANT_B * (termB + logRoughness) / reynoldsProduct);
    
    // Step 5: Aitken acceleration for convergence
    const double termDifference = termA - termB;
    
    // Note: Original algorithm checks (a - b) > tolerance WITHOUT absolute value
    // This is intentional - negative differences use the simpler approximation
    if (termDifference > CONVERGENCE_TOLERANCE) {
        // Aitken's Δ² method: extrapolate for faster convergence
        const double acceleratedTerm = pow(termDifference, 2) / (termA - 2.0 * termB + termC) - termA - logRoughness;
        frictionFactorValue = FRICTION_MULTIPLIER / pow(acceleratedTerm, 2);
    } else {
        // Terms converged or negative difference: use direct approximation
        const double simpleTerm = termA + logRoughness;
        frictionFactorValue = FRICTION_MULTIPLIER / pow(simpleTerm, 2);
    }
}

/*
==============================================================================
accelerateConvergence: Wegstein convergence acceleration for iterative processes

Implements the Wegstein method to accelerate convergence of fixed-point iterations.
This technique is particularly useful for solving implicit equations that arise in
two-phase flow correlations (e.g., liquid holdup calculations).

Wegstein Method:
Given a sequence x_{n+1} = f(x_n), the method uses previous iterations to extrapolate
and accelerate convergence. It maintains a history of the last two function evaluations
to compute an acceleration factor.

References:
    Wegstein, J. H.: "Accelerating Convergence of Iterative Processes,"
    Communications of the ACM, Vol. 1, No. 6 (1958) 9-13.

Parameters:
- currentValue  : Current iteration value (f(x_n)) - updated by the function
- previousValue : Previous iteration value (x_n) - input
- convergeStatus: Convergence state flag - modified by the function
- tolerance     : Convergence tolerance for |f(x) - x|
- auxValueA     : History: previous f(x_{n-1}) value - modified by the function
- auxValueB     : History: previous x_{n-1} value - modified by the function

Convergence States:
- CONVERGE_INIT      (0): First iteration - initialize history
- CONVERGE_ITERATING (1): Actively iterating
- CONVERGE_DONE      (2): Converged within tolerance
==============================================================================
*/
void accelerateConvergence(double& currentValue, double& previousValue, unsigned char& convergeStatus,
                          double tolerance, double& auxValueA, double& auxValueB) {
    
    // Convergence State Constants
    static constexpr unsigned char CONVERGE_INIT = 0;       // First iteration
    static constexpr unsigned char CONVERGE_ITERATING = 1;  // Actively iterating
    static constexpr unsigned char CONVERGE_DONE = 2;       // Converged
    
    // Wegstein Method Constants
    static constexpr double ZERO_DENOMINATOR_THRESHOLD = 1.0E-10;  // Near-singular denominator
    static constexpr double AVERAGE_DIVISOR = 2.0;                 // Simple averaging fallback
    
    // State 1: Initialization
    // First iteration: store initial values and use direct substitution
    if (convergeStatus == CONVERGE_INIT) {
        auxValueA = currentValue;      // Store f(x_0)
        auxValueB = previousValue;     // Store x_0
        currentValue = previousValue;  // Use direct substitution for first step
        convergeStatus = CONVERGE_ITERATING;
        return;
    }
    
    // State 2: Convergence Check
    // Check if iteration has converged: |f(x) - x| < tolerance
    const double iterationError = std::abs(currentValue - previousValue);
    
    if (iterationError < tolerance) {
        currentValue = previousValue;  // Accept converged value
        convergeStatus = CONVERGE_DONE;
        return;
    }
    
    // State 3: Wegstein Acceleration
    // Compute Wegstein acceleration factor using history
    // Formula: x_{n+1} = (auxValueA * x_n - auxValueB * f(x_n)) / deltaSum
    
    const double deltaSum = auxValueA - currentValue + previousValue - auxValueB;
    
    // Check for near-zero denominator (indicates poor conditioning)
    if (std::abs(deltaSum) < ZERO_DENOMINATOR_THRESHOLD) {
        // Fallback to simple averaging when Wegstein becomes unstable
        currentValue = (currentValue + auxValueB) / AVERAGE_DIVISOR;
    } else {
        // Apply Wegstein extrapolation
        const double numerator = auxValueA * previousValue - auxValueB * currentValue;
        const double wegsteinValue = numerator / deltaSum;
        
        // Update history for next iteration
        auxValueA = currentValue;   // Store current f(x_n)
        auxValueB = previousValue;  // Store current x_n
        
        // Use accelerated value for next iteration
        currentValue = wegsteinValue;
    }
}

// FUNÇÃO PARA EXECUÇÃO DE CORRELAÇÕES

/*
==============================================================================
executarCorrelacao: Extracts parameters from the celula structure and executes
                    the flow correlation specified by its numeric identifier.

Parameters:
- celula             : Pointer to the cell array
- i                  : Index of the current cell
- inter              : Flag indicating whether the cell is at an interface
                       (1) or not (0)
- AceleraConvergPerm : Convergence acceleration flag
- correlacao         : Identifier of the correlation to be executed
                       0  = Poettmann-Carpenter
                       1  = Baxendell-Thomas
                       2  = Fancher-Brown
                       3  = Hagedorn-Brown
                       4  = Duns-Ros
                       5  = Orkiszewski
                       6  = Beggs & Brill
                       7  = Mukherjee-Brill
                       8  = Aziz
                       9  = Gray
                       10 = Oliemans
                       11 = Dukler
                       12 = Beggs & Brill with Palmer correction
                       13 = Dukler, Eaton, and Flanigan
                       14 = Dukler and Minami I
                       15 = Dukler and Minami II

==============================================================================
*/


void executarCorrelacao(Cel* celula, int i, int inter, int AceleraConvergPerm,
                       int correlacao,
                       double& holdup, double& frictionGrad, double& gravityGrad, double& totalGrad,
                       double& reynolds, unsigned char& flowType) {
    
    
    long double area;
    long double diameter;
    long double roughness;
    long double alfmed;
    long double betmed;
    long double rhog;
    long double rhol;
    long double ugsmed;
    long double ulsmed;
    long double tmed;
    long double pmed;
    long double j;
    long double rhomix;
    long double gasViscosity;
    long double liquidViscosity;
    long double viscmix;
    long double angle;
    long double velocity;
    long double temperature;  // Temperature - °F
	long double compressibilityFactor;
	long double surfaceTension;
	long double productionRate;
	long double waterFraction;
	long double oilSurfaceTension;
	long double waterSurfaceTension;
    
    if(inter == 1) {
        angle = celula[i].dutoL.teta * 180 / M_PI;
    } else {
        angle = celula[i].duto.teta * 180 / M_PI;
    }
    
   
    if(inter == 1) {
        // Interface
        area = celula[i].dutoL.area;
        diameter = celula[i].dutoL.dia * 100.0 / (2.54*12);
        roughness = celula[i].dutoL.rug / celula[i].dutoL.dia;
        alfmed = celula[i - 1].alf;
        betmed = celula[i - 1].bet;
        tmed = celula[i - 1].temp;
        pmed = celula[i - 1].pres;

        compressibilityFactor=celula[i - 1].flui.Zdran(pmed, tmed);
        surfaceTension=celula[i - 1].flui.TensSuper(pmed, tmed)*1000.;
        productionRate=celula[i - 1].flui.IRGO * (1 - celula[i - 1].flui.BSW);
        waterFraction=celula[i - 1].flui.BSW;
        oilSurfaceTension=celula[i - 1].flui.TensSuperOleo(pmed, tmed)*1000.;
        waterSurfaceTension=celula[i - 1].flui.TensSuperAgua(pmed, tmed)*1000.;

        rhog = celula[i - 1].flui.MasEspGas(pmed, tmed);
        rhol = (1.0 - betmed) * celula[i - 1].flui.MasEspLiq(pmed, tmed)
             + betmed * celula[i - 1].fluicol.MasEspFlu(celula[i - 1].pres, celula[i - 1].temp);
        liquidViscosity = ((1.0 - betmed) * celula[i - 1].flui.ViscOleo(pmed, tmed)
                         + betmed * celula[i - 1].fluicol.VisFlu(pmed, tmed));
        gasViscosity = celula[i].flui.ViscGas(pmed, tmed);
        ugsmed = (celula[i - 1].MC - celula[i - 1].Mliqini) / (area * rhog);
        ulsmed = celula[i - 1].Mliqini / (area * rhol);
        j = ugsmed + ulsmed;
        velocity = 3.28084 * (ugsmed + ulsmed);
        temperature = tmed * 9.0/5.0 + 32.0;     // Conversion °C to °F
    } else {
        area = celula[i].duto.area;
        diameter = celula[i].duto.dia * 100.0 / (2.54*12);
        roughness = celula[i].duto.rug / celula[i].duto.dia;
        if(inter!=0){
        	alfmed = celula[i].alf;
        	betmed = celula[i].bet;
        }
        else{
           	alfmed = celula[i-1].alf;
            betmed = celula[i-1].bet;
        }
        long double razdx = celula[i].dx / (celula[i].dx + celula[i].dxL);
        
        if(AceleraConvergPerm == 0 && inter!=0) {
            tmed = razdx * celula[i].temp + (1.0 - razdx) * celula[i - 1].temp;
        } else {
            tmed = celula[i - 1].temp;
        }
        
        pmed = celula[i].presaux + celula[i - 1].dpB / 98066.5;

        celula[i].flui.PcTcIS();
        compressibilityFactor=celula[i].flui.Zdran(pmed, tmed);
        surfaceTension=celula[i].flui.TensSuper(pmed, tmed)*1000.;
        productionRate=celula[i - 1].flui.IRGO * (1 - celula[i - 1].flui.BSW);
        waterFraction=celula[i].flui.BSW;
        oilSurfaceTension=celula[i - 1].flui.TensSuperOleo(pmed, tmed)*1000.;
        waterSurfaceTension=celula[i - 1].flui.TensSuperAgua(pmed, tmed)*1000.;

        rhog = celula[i].rgCi;
        rhol = (1.0 - betmed) * celula[i].rpCi + betmed * celula[i].rcCi;
        ugsmed = celula[i].QG / area;
        ulsmed = celula[i].QL / area;
        j = ugsmed + ulsmed;
        velocity = 3.28084 * (ugsmed + ulsmed);
        
        long double sinalJ = 1.0;
        if(fabsl(j) > 1e-15) {
            sinalJ = j / fabsl(j);
        }
        
        rhomix = alfmed * rhog + (1.0 - alfmed) * rhol;
        liquidViscosity = ((1.0 - betmed) * celula[i].flui.ViscOleo(pmed, tmed)
                         + betmed * celula[i].fluicol.VisFlu(pmed, tmed));
        gasViscosity = celula[i].flui.ViscGas(pmed, tmed);
        viscmix = alfmed * celula[i].flui.ViscGas(pmed, tmed) + (1.0 - alfmed) * liquidViscosity;
        temperature = tmed * 9.0/5.0 + 32.0;     // Conversion °C to °F
    }
    
    
    pmed*=(0.9678411)*14.69595;
    long double liquidFraction;
    if(fabsl(j)>1e-15)liquidFraction= fabsl(ulsmed / j);
    else liquidFraction=1.-celula[i-1].alf;
    long double gasDensity = 0.06243 * rhog;
    long double liquidDensity = 0.06243 * rhol;
    
    // Additional variables for correlations that return more parameters
    double accelGrad = 0.0;
    unsigned char criticalFlag = 0;
    unsigned char convergeFlag = 0;
    unsigned char flowPattern = 0;
    unsigned char palmerFlag = 0;
    unsigned char correlationFlag = 0;
    unsigned char transitionFlag = 0;
    
    // Initialize holdup
    holdup = 0.0;
    
    if (correlacao == 0) {
        // 0 = Poettmann-Carpenter
        poettmannCarpenter(angle, diameter, velocity, liquidFraction,
                          gasDensity, liquidDensity, gasViscosity, liquidViscosity, roughness,
                          holdup, frictionGrad, gravityGrad, totalGrad, reynolds, flowType);
    }
    else if (correlacao == 1) {
        // 1 = Baxendell-Thomas
        baxendellThomas(angle, diameter, velocity, liquidFraction,
                       gasDensity, liquidDensity, gasViscosity, liquidViscosity, roughness,
                       holdup, frictionGrad, gravityGrad, totalGrad, reynolds, flowType);
    }
    else if (correlacao == 2) {
        // 2 = Fancher-Brown
        fancherBrown(angle, diameter, productionRate, velocity, liquidFraction,
                    gasDensity, liquidDensity, gasViscosity, liquidViscosity, roughness,
                    holdup, frictionGrad, gravityGrad, totalGrad, reynolds, flowType);
    }
    else if (correlacao == 3) {
        // 3 = Hagedorn-Brown
        hagedornBrown(angle, diameter, roughness, pmed, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                     temperature, compressibilityFactor,
                     holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                     reynolds, flowType, criticalFlag, convergeFlag);
    }
    else if (correlacao == 4) {
        // 4 = Duns-Ros
        dunsRos(angle, diameter, roughness, pmed, velocity, liquidFraction,
               gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
               temperature, compressibilityFactor,
               holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
               reynolds, flowType, criticalFlag, convergeFlag);
    }
    else if (correlacao == 5) {
        // 5 = Orkiszewski
        unsigned char isigFlag=0;
        orkiszewski(angle, diameter, roughness, pmed, velocity, liquidFraction,
                   gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                   temperature, compressibilityFactor, waterFraction,
                   holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                   reynolds, flowType, criticalFlag, convergeFlag, isigFlag);
    }
    else if (correlacao == 6) {
        // 6 = Beggs & Brill
        beggsAndBrill(angle, diameter, roughness, pmed, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                     holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                     reynolds, flowType, palmerFlag, criticalFlag);
    }
    else if (correlacao == 7) {
        // 7 = Mukherjee-Brill
        mukherjeeeBrill(angle, diameter, roughness, pmed, velocity, liquidFraction,
                       gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                       surfaceTension, temperature, compressibilityFactor,
                       holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                       reynolds, flowPattern, criticalFlag, convergeFlag);
    }
    else if (correlacao == 8) {
        // 8 = Aziz
        aziz(angle, diameter, roughness, pmed, velocity, liquidFraction,
            gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
            holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
            reynolds, flowPattern, criticalFlag);
    }
    else if (correlacao == 9) {
        // 9 = Gray
        gray(angle, diameter, roughness, pmed, velocity, liquidFraction, waterFraction,
            gasDensity, liquidDensity, gasViscosity, liquidViscosity,
            oilSurfaceTension, waterSurfaceTension,
            holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
            reynolds, flowPattern, criticalFlag);
    }
    else if (correlacao == 10) {
        // 10 = Oliemans
        correlationFlag = 1;
        olie(angle, diameter, roughness, pmed, velocity, liquidFraction,
            gasDensity, liquidDensity, gasViscosity, liquidViscosity,
            surfaceTension, temperature, compressibilityFactor,
            holdup, frictionGrad, gravityGrad, totalGrad,
            reynolds, flowPattern, correlationFlag, criticalFlag);
    }
    else if (correlacao == 11) {
        // 11 = Dukler
        correlationFlag=0;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
    else if (correlacao == 12) {
        // 12 = Beggs & Brill with Palmer's correction
        palmerFlag = 1;
        beggsAndBrill(angle, diameter, roughness, pmed, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity, surfaceTension,
                     holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                     reynolds, flowType, palmerFlag, criticalFlag);
    }
    else if (correlacao == 13) {
        // 13 = Dukler, Eaton e Flanigan
        correlationFlag=1;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
    else if (correlacao == 14) {
        // 14 = Dukler e Minami I
        correlationFlag=2;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
    else if (correlacao == 15) {
        // 15 = Dukler e Minami II
        correlationFlag=3;
        duklerCorrelation(angle, diameter, roughness, pmed, velocity, liquidFraction,
                         gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                         surfaceTension, temperature, compressibilityFactor,
                         holdup, frictionGrad, gravityGrad, accelGrad, totalGrad,
                         reynolds, flowPattern, correlationFlag, transitionFlag, criticalFlag);
    }
}


// FLOW CORRELATIONS

/*
==============================================================================
poettmannCarpenter: Calculates pressure gradients in vertical two-phase flow
                    using the Poettmann and Carpenter correlation.

References:
     1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984), Sections 3-4 through 3-10.
     2.  Poettmann, F. H. and Carpenter, P. G.: "The Multiphase Flow
         of Gas, Oil, and Water Through Vertical Flow Strings with
         Application to the Design of Gas-Lift Installations,"
         Drilling and Production Practice, API (1952), 257-317.

Variable dictionary:
- angle          : Pipe inclination angle (degrees)
- diameter       : Pipe inner diameter (inches)
- velocity       : Average flow velocity (ft/s)
- liquidFraction : Liquid volumetric fraction
- gasDensity     : Gas-phase density (lb/ft3)
- liquidDensity  : Liquid-phase density (lb/ft3)
- gasViscosity   : Gas-phase viscosity (cP)
- liquidViscosity: Liquid-phase viscosity (cP)
- roughness      : Pipe relative roughness (dimensionless)
- frictionGrad   : Frictional pressure gradient (psi/ft)
- gravityGrad    : Gravitational pressure gradient (psi/ft)
- totalGrad      : Total pressure gradient (psi/ft)
- reynolds       : Reynolds number (dimensionless)
- flowType       : Flow-type indicator (1 = liquid, 2 = gas, 3 = mixed)
- logFlowArray   : Array of logarithmic flow values used by the correlation
- frictionArray  : Array of correlated function values
- angleRad       : Angle in radians
- mixtureRho     : Apparent mixture density weighted by the liquid and gas fractions
- frictionFactor : Friction factor
- xParam         : Intermediate parameter defined as the product of density,
                   velocity, and diameter
- logX           : Logarithm of xParam
==============================================================================
*/


void poettmannCarpenter(double angle, double diameter, double velocity, double liquidFraction,
                       double gasDensity, double liquidDensity, double gasViscosity,
                       double liquidViscosity, double roughness, double& holdup,
                       double& frictionGrad, double& gravityGrad, double& totalGrad, double& reynolds,
                       unsigned char& flowType) {
    
    // CORRELATION DATA (replaced by analytical fits in CorrelationFits.h)
    
    // PHYSICAL CONSTANTS
    static constexpr double REYNOLDS_FACTOR = 1488.0;  // Field units conversion (lb/ft³)(ft/s)(in)/(cP)
    static constexpr double PSF_TO_PSI = 144.0;        // Pressure conversion factor (in²/ft²)
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    static constexpr double MIN_VELOCITY_THRESHOLD = 1e-15;  // Near-zero velocity cutoff
    
    // Flow type identifiers
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_TWOPHASE = 3;
    
    // INPUT PROCESSING
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    const bool isStaticFlow = (fabs(velocity) < MIN_VELOCITY_THRESHOLD);
    
    // FLOW REGIME IDENTIFICATION
    double mixtureDensity;
    
    if (liquidFraction >= 1.0) {
        flowType = FLOW_LIQUID;
        mixtureDensity = liquidDensity;
    } else if (liquidFraction <= 0.0) {
        flowType = FLOW_GAS;
        mixtureDensity = gasDensity;
    } else {
        flowType = FLOW_TWOPHASE;
        mixtureDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    }
    
    // FRICTION FACTOR CALCULATION
    double frictionFactorValue = 0.0;
    reynolds = 0.0;
    
    if (!isStaticFlow) {
        if (flowType == FLOW_TWOPHASE) {
            // Two-phase: Poettmann-Carpenter empirical correlation
            const double flowParameter = mixtureDensity * velocity * diameter;
            const double logFlowParameter = log(flowParameter);
            
            const double logFrictionInterpolated = CorrelationFits::poettmannFriction(logFlowParameter);
            
            frictionFactorValue = exp(logFrictionInterpolated);
            
        } else {
            // Single-phase: Moody diagram with Reynolds number
            const double phaseViscosity = (flowType == FLOW_LIQUID) ? liquidViscosity : gasViscosity;
            reynolds = REYNOLDS_FACTOR * mixtureDensity * velocity * diameter / phaseViscosity;
            frictionFactor(reynolds, roughness, frictionFactorValue);
        }
    }
    
    // PRESSURE GRADIENT COMPONENTS
    // Frictional pressure drop (Darcy-Weisbach)
    if (isStaticFlow) {
        frictionGrad = 0.0;
    } else {
        const double velocitySquared = velocity * velocity;
        frictionGrad = (frictionFactorValue * mixtureDensity * velocitySquared) /
                       (2.0 * g_in * diameter * PSF_TO_PSI);
    }
    
    // Gravitational pressure gradient
    gravityGrad = (mixtureDensity * sinAngle) / PSF_TO_PSI;
    
    // Total gradient
    totalGrad = frictionGrad + gravityGrad;
    
    // HOLDUP (NO-SLIP MODEL)
    // Poettmann-Carpenter assumes no slip between phases
    holdup = liquidFraction;
}

/*
==============================================================================
baxendellThomas: Calculates pressure gradients for vertical two-phase flow
                 using the Baxendell and Thomas correlation.

References:
     1.  Baxendell, P. B. and Thomas, R.: "The Calculation of
         Pressure Gradients in High-Rate Flowing Wells," Journal
         of Petroleum Technology (Oct. 1961), 1023-1028.
     2.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984), Sections 3-4 through 3-10.

Variable dictionary:
- angle          : Pipe inclination angle (degrees)
- diameter       : Pipe inner diameter (inches)
- velocity       : Average flow velocity (ft/s)
- liquidFraction : Liquid volumetric fraction
- gasDensity     : Gas density (lb/ft3)
- liquidDensity  : Liquid density (lb/ft3)
- gasViscosity   : Gas viscosity (cP)
- liquidViscosity: Liquid viscosity (cP)
- roughness      : Pipe relative roughness (dimensionless)
- frictionGrad   : Frictional pressure gradient (psi/ft)
- gravityGrad    : Gravitational pressure gradient (psi/ft)
- totalGrad      : Total pressure gradient (psi/ft)
- reynolds       : Reynolds number (dimensionless)
- flowType       : Flow-type indicator (1 = liquid, 2 = gas, 3 = mixed)
- logFlowArray   : Array of logarithmic flow values used by the correlation
                   (internal constant)
- baxendellArray : Array of values from the Baxendell and Thomas correlated
                   function (internal constant)
- angleRad       : Angle in radians
- mixtureRho     : Apparent mixture density weighted by the liquid and gas
                   fractions
- frictionFactor : Friction factor
- xParam         : Intermediate parameter defined as the product of density,
                   velocity, and diameter
- logX           : Logarithm of xParam
==============================================================================
*/

void baxendellThomas(double angle, double diameter, double velocity, double liquidFraction,
                    double gasDensity, double liquidDensity, double gasViscosity,
                    double liquidViscosity, double roughness, double& holdup,
                    double& frictionGrad, double& gravityGrad, double& totalGrad, double& reynolds,
                    unsigned char& flowType) {
    
    // CORRELATION DATA (replaced by analytical fits in CorrelationFits.h)
    
    // PHYSICAL CONSTANTS
    static constexpr double REYNOLDS_FACTOR = 1488.0;  // Field units conversion (lb/ft³)(ft/s)(in)/(cP)
    static constexpr double PSF_TO_PSI = 144.0;        // Pressure conversion factor (in²/ft²)
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    static constexpr double MIN_VELOCITY_THRESHOLD = 1e-15;  // Near-zero velocity cutoff
    
    // Flow type identifiers
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_TWOPHASE = 3;
    
    // INPUT PROCESSING
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    const bool isStaticFlow = (fabs(velocity) < MIN_VELOCITY_THRESHOLD);
    
    // FLOW REGIME IDENTIFICATION
    double mixtureDensity;
    
    if (liquidFraction >= 1.0) {
        flowType = FLOW_LIQUID;
        mixtureDensity = liquidDensity;
    } else if (liquidFraction <= 0.0) {
        flowType = FLOW_GAS;
        mixtureDensity = gasDensity;
    } else {
        flowType = FLOW_TWOPHASE;
        mixtureDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    }
    
    // FRICTION FACTOR CALCULATION
    double frictionFactorValue = 0.0;
    reynolds = 0.0;
    
    if (!isStaticFlow) {
        if (flowType == FLOW_TWOPHASE) {
            // Two-phase: Baxendell-Thomas empirical correlation
            const double flowParameter = mixtureDensity * velocity * diameter;
            const double logFlowParameter = log(flowParameter);
            
            const double logFrictionInterpolated = CorrelationFits::baxendellFriction(logFlowParameter);
            
            frictionFactorValue = exp(logFrictionInterpolated);
            
        } else {
            // Single-phase: Moody diagram with Reynolds number
            const double phaseViscosity = (flowType == FLOW_LIQUID) ? liquidViscosity : gasViscosity;
            reynolds = REYNOLDS_FACTOR * mixtureDensity * velocity * diameter / phaseViscosity;
            frictionFactor(reynolds, roughness, frictionFactorValue);
        }
    }
    
    // PRESSURE GRADIENT COMPONENTS
    // Frictional pressure drop (Darcy-Weisbach)
    if (isStaticFlow) {
        frictionGrad = 0.0;
    } else {
        const double velocitySquared = velocity * velocity;
        frictionGrad = (frictionFactorValue * mixtureDensity * velocitySquared) /
                       (2.0 * g_in * diameter * PSF_TO_PSI);
    }
    
    // Gravitational pressure gradient
    gravityGrad = (mixtureDensity * sinAngle) / PSF_TO_PSI;
    
    // Total gradient
    totalGrad = frictionGrad + gravityGrad;
    
    // HOLDUP (NO-SLIP MODEL)
    // Baxendell-Thomas assumes no slip between phases
    holdup = liquidFraction;
}

/*
==============================================================================
fancherBrown: Calculates pressure gradients for vertical two-phase flow using
              the Fancher-Brown correlation, with input validation.

References:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984), Sections 3-4 through 3-10.
      2.  Fancher, G. H. and Brown, K. E.: "Prediction of Pressure
          Gradients for Multiphase Flow in Tubing," Society of
          Petroleum Engineers Journal (March 1963), 59-69.

Variable dictionary:
- angle          : Pipe inclination angle (degrees)
- diameter       : Pipe inner diameter (inches)
- productionRate : Gas-to-liquid production ratio (SCF/STB)
- velocity       : Average flow velocity (ft/s)
- liquidFraction : Liquid volumetric fraction
- gasDensity     : Gas-phase density (lb/ft3)
- liquidDensity  : Liquid-phase density (lb/ft3)
- gasViscosity   : Gas-phase viscosity (cP)
- liquidViscosity: Liquid-phase viscosity (cP)
- roughness      : Pipe relative roughness (dimensionless)
- frictionGrad   : Frictional pressure gradient (psi/ft)
- gravityGrad    : Gravitational pressure gradient (psi/ft)
- totalGrad      : Total pressure gradient (psi/ft)
- reynolds       : Reynolds number (dimensionless)
- flowType       : Flow-type indicator (1 = liquid, 2 = gas, 3 = mixed)
- rateArray      : Gas-liquid ratio (GLR) data
- logFlowArray   : Array of logarithmic flow values used by the correlations
                   (internal constant)
- fancherMatrix  : Matrix of Fancher-Brown correlation function values
- angleRad       : Angle in radians (calculated internally)
- mixtureRho     : Apparent mixture density weighted by the liquid and gas
                   fractions
- frictionFactor : Friction factor calculated by the correlation or by the
                   frictionFactor function
- xParam         : Intermediate parameter defined as the product of density,
                   velocity, and diameter
- logX           : Logarithm of xParam used by the correlations
- tempArray      : Auxiliary array used for one-dimensional interpolation at
                   the extreme productionRate values
- i, j           : Loop-control variables used to select rows and columns in
                   the fancherMatrix
==============================================================================
*/


void fancherBrown(double angle, double diameter, double productionRate, double velocity,
                 double liquidFraction, double gasDensity, double liquidDensity,
                 double gasViscosity, double liquidViscosity, double roughness,
                 double& holdup, double& frictionGrad, double& gravityGrad, double& totalGrad,
                 double& reynolds, unsigned char& flowType) {
    
    // CORRELATION DATA (replaced by analytical fits in CorrelationFits.h)
    
    // GLR range bounds
    static constexpr double GLR_MIN = 1500.0;
    static constexpr double GLR_MAX = 3000.0;
    
    // PHYSICAL CONSTANTS
    static constexpr double REYNOLDS_FACTOR = 1488.0;  // Field units conversion
    static constexpr double PSF_TO_PSI = 144.0;        // Pressure conversion factor
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    static constexpr double MIN_VELOCITY_THRESHOLD = 1e-15;
    
    // Flow type identifiers
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_TWOPHASE = 3;
    
    // INPUT PROCESSING
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    const bool isStaticFlow = (fabs(velocity) < MIN_VELOCITY_THRESHOLD);
    
    // FLOW REGIME IDENTIFICATION
    double mixtureDensity;
    
    if (liquidFraction >= 1.0) {
        flowType = FLOW_LIQUID;
        mixtureDensity = liquidDensity;
    } else if (liquidFraction <= 0.0) {
        flowType = FLOW_GAS;
        mixtureDensity = gasDensity;
    } else {
        flowType = FLOW_TWOPHASE;
        mixtureDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    }
    
    // FRICTION FACTOR CALCULATION
    double frictionFactorValue = 0.0;
    reynolds = 0.0;
    
    if (!isStaticFlow) {
        if (flowType == FLOW_TWOPHASE) {
            // Two-phase: Fancher-Brown correlation (analytical fit)
            const double flowParameter = mixtureDensity * velocity * diameter;
            const double logFlowParameter = log(flowParameter);
            
            const double logFrictionInterpolated = CorrelationFits::fancherFrictionInterp(logFlowParameter, productionRate);
            
            frictionFactorValue = exp(logFrictionInterpolated);
            
        } else {
            // Single-phase: Moody diagram with Reynolds number
            const double phaseViscosity = (flowType == FLOW_LIQUID) ? liquidViscosity : gasViscosity;
            reynolds = REYNOLDS_FACTOR * mixtureDensity * velocity * diameter / phaseViscosity;
            frictionFactor(reynolds, roughness, frictionFactorValue);
        }
    }
    
    // PRESSURE GRADIENT COMPONENTS
    // Frictional pressure drop (Darcy-Weisbach)
    if (isStaticFlow) {
        frictionGrad = 0.0;
    } else {
        const double velocitySquared = velocity * velocity;
        frictionGrad = (frictionFactorValue * mixtureDensity * velocitySquared) /
                       (2.0 * g_in * diameter * PSF_TO_PSI);
    }
    
    // Gravitational pressure gradient
    gravityGrad = (mixtureDensity * sinAngle) / PSF_TO_PSI;
    
    // Total gradient
    totalGrad = frictionGrad + gravityGrad;
    
    // HOLDUP (NO-SLIP MODEL)
    // Fancher-Brown assumes no slip between phases
    holdup = liquidFraction;
}

/*
==============================================================================
hagedornBrown: Calculates pressure gradients, liquid holdup, and vertical
               two-phase flow properties using the Hagedorn-Brown correlation.

References:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984), Sections 3-11 through 3-19.
      2.  Duns, H., Jr. and Ros, N. C. J.: "Vertical Flow of Gas and
          Liquid Mixtures in Wells," Proc., 6th World Petroleum
          Congress (1963), 451.
      3.  Griffith, P. and Wallis, G. B.: "Two Phase Slug Flow,"
          Journal of Heat Transfer; Trans., ASME (Aug. 1961), 307-320.
      4.  Hagedorn, A. R. and Brown, K. E.: "Experimental Study of
          Pressure Gradients Occurring During Continuous Two Phase
          Flow in Small-Diameter Vertical Conduits," Journal of
          Petroleum Technology (April 1965), 475-484.

Variable dictionary:
- angle           : Pipe inclination angle (degrees)
- diameter        : Pipe inner diameter (inches)
- roughness       : Pipe relative roughness
- pressure        : Pressure (psi)
- velocity        : Average flow velocity (ft/s)
- liquidFraction  : Liquid volumetric fraction
- gasDensity      : Gas-phase density (lb/ft3)
- liquidDensity   : Liquid-phase density (lb/ft3)
- gasViscosity    : Gas-phase viscosity (cP)
- liquidViscosity : Liquid-phase viscosity (cP)
- liquidViscNumber: Parameter related to liquid holdup
- liquidVelNumber : Auxiliary parameter used to calculate liquid holdup
- gasVelNumber    : Auxiliary parameter used to calculate liquid holdup
- diameterNumber  : Auxiliary parameter used to calculate liquid holdup
- holdup          : Calculated liquid holdup
- frictionGrad    : Frictional pressure gradient (psi/ft)
- gravityGrad     : Gravitational pressure gradient (psi/ft)
- accelGrad       : Accelerational pressure gradient (psi/ft)
- totalGrad       : Total pressure gradient (psi/ft)
- reynolds        : Reynolds number
- flowType        : Flow-type indicator
                    (1 = liquid, 2 = gas, 3 = bubble, 4 = intermittent)
- griffithFlag    : Indicates the use of the Griffith-Wallis correlation
                    for bubble flow
- holdupFlag      : Indicates whether the minimum holdup adjustment is applied
- criticalFlag    : Critical-flow indicator
- calcInj         : Global flag that controls the calculation mode
==============================================================================
*/
void hagedornBrown(double angle, double diameter, double roughness, double pressure, double velocity,
                  double liquidFraction, double gasDensity, double liquidDensity,
                  double gasViscosity, double liquidViscosity, double surfaceTension,
                  double temperature, double compressibilityFactor,
                  double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                  double& totalGrad, double& reynolds, unsigned char& flowType,
                  unsigned char& criticalFlag, unsigned char& convergeFlag) {
    
    // CORRELATION DATA (replaced by analytical fits in CorrelationFits.h)
    
    // PHYSICAL CONSTANTS
    static constexpr double REYNOLDS_FACTOR = 1488.0;  // Field units conversion
    static constexpr double PSF_TO_PSI = 144.0;        // Pressure conversion
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    static constexpr double MIN_VELOCITY_THRESHOLD = 1e-15;
    static constexpr double MIN_HOLDUP_THRESHOLD = 1e-15;
    static constexpr double ATM_PRESSURE_PSI = 14.69595;  // Standard atmospheric pressure
    
    // Dimensionless number coefficients (Hagedorn-Brown)
    static constexpr double COEFF_VEL_NUMBER = 1.938;      // Velocity number coefficient
    static constexpr double COEFF_DIAM_NUMBER = 120.872;   // Diameter number coefficient
    static constexpr double COEFF_VISC_NUMBER = 0.15726;   // Viscosity number coefficient
    
    // Griffith-Wallis bubble flow parameters
    static constexpr double GRIFFITH_REFERENCE_VELOCITY = 0.8;  // ft/s
    static constexpr double BUBBLE_LIMIT_BASE = 1.071;
    static constexpr double BUBBLE_LIMIT_COEFF = 0.2218;
    static constexpr double BUBBLE_LIMIT_MIN = 0.13;
    
    // Acceleration parameter limit (critical flow)
    static constexpr double MAX_ACCEL_PARAMETER = 0.95;
    
    // Flow type identifiers
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_BUBBLE = 3;
    static constexpr unsigned char FLOW_SLUG_CHURN = 4;
    
    // Configuration flags
    static constexpr bool USE_GRIFFITH_WALLIS = true;   // Use Griffith-Wallis for bubble flow
    static constexpr bool ENFORCE_NO_SLIP_MINIMUM = true;  // Enforce holdup >= liquid fraction
    
    // INITIALIZATION
    flowType = 0;
    criticalFlag = 0;
    convergeFlag = 0;
    
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    const bool isStaticFlow = (fabs(velocity) < MIN_VELOCITY_THRESHOLD);
    
    // SUPERFICIAL VELOCITIES
    const double liquidSuperficialVel = velocity * liquidFraction;
    const double gasSuperficialVel = velocity * (1.0 - liquidFraction);
    
    // DIMENSIONLESS NUMBERS (Hagedorn-Brown)
    const double densityRatio = liquidDensity / surfaceTension;
    const double sqrtDensityRatio = sqrt(densityRatio);
    const double fourthRootDensityRatio = sqrt(sqrtDensityRatio);
    
    const double liquidVelNumber = COEFF_VEL_NUMBER * fourthRootDensityRatio * liquidSuperficialVel;
    const double gasVelNumber = COEFF_VEL_NUMBER * fourthRootDensityRatio * gasSuperficialVel;
    const double diameterNumber = COEFF_DIAM_NUMBER * diameter * sqrtDensityRatio;
    
    const double viscNumberDenom = pow(liquidDensity * surfaceTension * surfaceTension * surfaceTension, 0.25);
    const double liquidViscNumber = COEFF_VISC_NUMBER * liquidViscosity / viscNumberDenom;
    
    // SINGLE-PHASE FLOW HANDLING
    if (liquidFraction >= 1.0) {
        // Single-phase liquid
        flowType = FLOW_LIQUID;
        holdup = 1.0;
        
        const double noSlipDensity = liquidDensity;
        if (!isStaticFlow) {
            reynolds = REYNOLDS_FACTOR * noSlipDensity * velocity * diameter / liquidViscosity;
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            frictionGrad = (frictionFactorValue * noSlipDensity * velocity * velocity) /
                          (2.0 * g_in * diameter * PSF_TO_PSI);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = noSlipDensity * sinAngle / PSF_TO_PSI;
        accelGrad = 0.0;
        totalGrad = frictionGrad + gravityGrad;
        return;
    }
    
    if (liquidFraction <= 0.0) {
        // Single-phase gas
        flowType = FLOW_GAS;
        holdup = 0.0;
        
        const double noSlipDensity = gasDensity;
        if (!isStaticFlow) {
            reynolds = REYNOLDS_FACTOR * noSlipDensity * velocity * diameter / gasViscosity;
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            frictionGrad = (frictionFactorValue * noSlipDensity * velocity * velocity) /
                          (2.0 * g_in * diameter * PSF_TO_PSI);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = noSlipDensity * sinAngle / PSF_TO_PSI;
        accelGrad = 0.0;
        totalGrad = frictionGrad + gravityGrad;
        return;
    }
    
    // TWO-PHASE FLOW: BUBBLE REGIME CHECK
    const double gasFraction = 1.0 - liquidFraction;
    double bubbleFlowLimit = BUBBLE_LIMIT_BASE - BUBBLE_LIMIT_COEFF * velocity * velocity / diameter;
    if (bubbleFlowLimit < BUBBLE_LIMIT_MIN) {
        bubbleFlowLimit = BUBBLE_LIMIT_MIN;
    }
    
    if (gasFraction <= bubbleFlowLimit && USE_GRIFFITH_WALLIS) {
        // BUBBLE FLOW: Griffith-Wallis Correlation
        flowType = FLOW_BUBBLE;
        
        if (!isStaticFlow) {
            const double refVel = GRIFFITH_REFERENCE_VELOCITY;
            const double velRatio = velocity / refVel;
            const double gasRatio = gasSuperficialVel / refVel;
            const double discriminant = velRatio * velRatio + 2.0 * velRatio * (1.0 - 2.0 * gasRatio) + 1.0;
            
            holdup = 1.0 - 0.5 * (1.0 + velRatio - sqrt(discriminant));
            
            if (holdup < liquidFraction) {
                holdup = liquidFraction;
            }
            
            const double slipDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
            
            if (fabs(holdup) > MIN_HOLDUP_THRESHOLD) {
                const double liquidVelInSlug = liquidSuperficialVel / holdup;
                reynolds = REYNOLDS_FACTOR * liquidDensity * liquidVelInSlug * diameter / liquidViscosity;
                
                double frictionFactorValue;
                frictionFactor(reynolds, roughness, frictionFactorValue);
                
                const double liquidVelSquared = liquidVelInSlug * liquidVelInSlug;
                frictionGrad = (frictionFactorValue * liquidDensity * liquidVelSquared) /
                              (2.0 * g_in * diameter * PSF_TO_PSI);
            } else {
                reynolds = 0.0;
                frictionGrad = 0.0;
            }
            
            gravityGrad = slipDensity * sinAngle / PSF_TO_PSI;
            accelGrad = 0.0;
            totalGrad = frictionGrad + gravityGrad;
        } else {
            holdup = liquidFraction;
            reynolds = 0.0;
            frictionGrad = 0.0;
            gravityGrad = (liquidDensity * holdup + gasDensity * (1.0 - holdup)) * sinAngle / PSF_TO_PSI;
            accelGrad = 0.0;
            totalGrad = gravityGrad;
        }
        
        return;
    }
    
    // TWO-PHASE FLOW: SLUG/CHURN REGIME (Hagedorn-Brown Correlation)
    flowType = FLOW_SLUG_CHURN;
    
    // Calculate liquid holdup using Hagedorn-Brown correlation
    if (gasVelNumber < MIN_VELOCITY_THRESHOLD) {
        // No gas flow - treat as liquid-only
        holdup = 1.0;
    } else {
        // Viscosity correction factor
        const double logViscNumber = log(liquidViscNumber);
        const double logViscFactor = CorrelationFits::hagedornCNL(logViscNumber);
        const double viscosityFactor = exp(logViscFactor);
        
        // Primary holdup parameter
        const double holdupParam = (liquidVelNumber * viscosityFactor) /
                                   (pow(gasVelNumber, 0.575) * diameterNumber) *
                                   pow(pressure / ATM_PRESSURE_PSI, 0.1);
        const double logHoldupParam = log(holdupParam);
        
        // Base holdup from primary correlation
        double baseHoldup = CorrelationFits::hagedornHoldup(logHoldupParam);
        
        // Secondary correction factor
        const double psiParam = gasVelNumber * pow(liquidViscNumber, 0.38) * pow(diameterNumber, -2.14);
        double correctionFactor = CorrelationFits::hagedornPSI(psiParam);
        
        if (correctionFactor < 1.0) {
            correctionFactor = 1.0;
        }
        
        // Apply correction
        holdup = baseHoldup * correctionFactor;
        
        // Physical bounds
        if (holdup < 0.0) holdup = 0.0;
        if (holdup > 1.0) holdup = 1.0;
        
        // Enforce no-slip minimum if enabled
        if (ENFORCE_NO_SLIP_MINIMUM && holdup < liquidFraction) {
            holdup = liquidFraction;
        }
    }
    
    // MIXTURE PROPERTIES
    const double noSlipDensity = liquidDensity * liquidFraction + gasDensity * gasFraction;
    const double slipDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
    
    // Effective viscosity (power-law mixing)
    double effectiveViscosity;
    if (liquidViscosity > 0.0) {
        effectiveViscosity = pow(liquidViscosity, holdup) * pow(gasViscosity, 1.0 - holdup);
    } else {
        effectiveViscosity = liquidViscosity * holdup + gasViscosity * (1.0 - holdup);
    }
    
    // PRESSURE GRADIENT CALCULATIONS
    double frictionFactorValue = 0.0;
    
    if (!isStaticFlow) {
        reynolds = REYNOLDS_FACTOR * noSlipDensity * velocity * diameter / effectiveViscosity;
        frictionFactor(reynolds, roughness, frictionFactorValue);
        
        const double velocitySquared = velocity * velocity;
        frictionGrad = (frictionFactorValue * noSlipDensity * noSlipDensity * velocitySquared) /
                      (2.0 * g_in * diameter * slipDensity * PSF_TO_PSI);
    } else {
        reynolds = 0.0;
        frictionGrad = 0.0;
    }
    
    // Gravity gradient
    gravityGrad = slipDensity * sinAngle / PSF_TO_PSI;
    
    // Acceleration gradient
    double accelParameter = slipDensity * velocity * gasSuperficialVel /
                           (g_in * pressure * PSF_TO_PSI);
    
    // Check for critical flow
    if (accelParameter > MAX_ACCEL_PARAMETER) {
        accelParameter = MAX_ACCEL_PARAMETER;
        criticalFlag = 1;
    }
    
    // Total gradient
    totalGrad = (gravityGrad + frictionGrad) / (1.0 - accelParameter);
    accelGrad = totalGrad * accelParameter;
}

/*
==============================================================================
dunsRos: Calculates pressure gradients, liquid holdup, and vertical two-phase
         flow properties using the Duns-Ros correlation.

References:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984), Sections 3-20 through 3-33.
      2.  Duns, H., Jr. and Ros, N. C. J.: "Vertical Flow of Gas and
          Liquid Mixtures in Wells," Proc., 6th World Petroleum
          Congress (1963), 451.

Variable dictionary:
- angle           : Pipe inclination angle (degrees)
- diameter        : Pipe inner diameter (inches)
- roughness       : Pipe relative roughness
- pressure        : Pressure (psi)
- velocity        : Average flow velocity (ft/s)
- liquidFraction  : Liquid volumetric fraction
- gasDensity      : Gas-phase density (lb/ft3)
- liquidDensity   : Liquid-phase density (lb/ft3)
- gasViscosity    : Gas-phase viscosity (cP)
- liquidViscosity : Liquid-phase viscosity (cP)
- surfaceTension  : Surface tension (dyn/cm)
- liquidViscNumber: Dimensionless liquid viscosity number
- liquidVelNumber : Dimensionless liquid velocity number
- gasVelNumber    : Dimensionless gas velocity number
- diameterNumber  : Dimensionless pipe diameter number
- holdup          : Calculated liquid holdup
- frictionGrad    : Frictional pressure gradient (psi/ft)
- gravityGrad     : Gravitational pressure gradient (psi/ft)
- accelGrad       : Accelerational pressure gradient (psi/ft)
- totalGrad       : Total pressure gradient (psi/ft)
- reynolds        : Liquid Reynolds number
- flowType        : Flow-type indicator
                    (1 = liquid, 2 = gas, 3 = bubble, 4 = slug,
                     5 = mist, 6 = transition)
- criticalFlag    : Critical-flow indicator
- convergeFlag    : Convergence indicator
==============================================================================
*/
void dunsRos(double angle, double diameter, double roughness, double pressure, double velocity,
             double liquidFraction, double gasDensity, double liquidDensity,
             double gasViscosity, double liquidViscosity, double surfaceTension,
             double temperature, double compressibilityFactor,
             double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
             double& totalGrad, double& reynolds, unsigned char& flowType,
             unsigned char& criticalFlag, unsigned char& convergeFlag) {
    
    // DUNS-ROS CORRELATION DATA (replaced by analytical fits in CorrelationFits.h)
    
    // DUNS-ROS CONSTANTS
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_BUBBLE = 3;
    static constexpr unsigned char FLOW_SLUG = 4;
    static constexpr unsigned char FLOW_MIST = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.99999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.00001;
    static constexpr double VELOCITY_TOLERANCE = 1.0e-15;
    
    static constexpr double VEL_NUMBER_SCALE = 1.938;           // Velocity number scaling
    static constexpr double DIAMETER_NUMBER_SCALE = 120.872;    // Diameter number scaling
    static constexpr double VISC_NUMBER_SCALE = 0.15726;        // Viscosity number scaling
    
    static constexpr double SLUG_BASE_LIMIT = 50.0;             // Base slug flow limit
    static constexpr double SLUG_SLOPE = 36.0;                  // Slug limit slope
    static constexpr double MIST_BASE_LIMIT = 75.0;             // Base mist flow limit
    static constexpr double MIST_SLOPE = 84.0;                  // Mist limit slope
    static constexpr double MIST_EXPONENT = 0.75;               // Mist limit exponent
    
    static constexpr double DIAMETER_CORRECTION = 0.029;        // Diameter correction for slug
    static constexpr double SLUG_GAS_EXPONENT = 0.982;          // Gas velocity exponent in slug
    static constexpr double FRICTION_CORRECTION_DENOM = 50.0;   // Friction correction denominator
    static constexpr double DIAMETER_EXPONENT = 0.666;          // Diameter number exponent
    
    static constexpr double WEBBER_SCALE = 453.59;              // Weber number scale
    static constexpr double VISC_PARAM_SCALE = 0.0002048;       // Viscosity parameter scale
    static constexpr double SURFACE_TENSION_SCALE = 0.0749;     // Surface tension scale
    static constexpr double WEBBER_THRESHOLD = 0.005;           // Weber product threshold
    
    static constexpr double ACCEL_CRITICAL_LIMIT = 0.95;        // Critical acceleration limit
    static constexpr double MAX_MIST_ROUGHNESS = 0.5;           // Maximum mist roughness
    static constexpr double HIGH_ROUGHNESS_THRESHOLD = 0.05;    // High roughness threshold
    static constexpr double DEFAULT_MIST_ROUGHNESS = 1.0e-5;    // Default mist roughness
    
    static constexpr double PRESSURE_CONVERSION = 144.0;        // psi to lbf/ft²
    static constexpr double REYNOLDS_SCALE = 1488.0;            // Reynolds number scale
    static constexpr double DEG_TO_RAD = M_PI / 180.0;          // Degrees to radians
    
    // INITIALIZATION
    criticalFlag = 0;
    convergeFlag = 0;
    
    const double sinAngle = sin(angle * DEG_TO_RAD);
    const double liquidSuperficialVel = velocity * liquidFraction;
    const double gasSuperficialVel = velocity - liquidSuperficialVel;
    
    // SINGLE-PHASE LIQUID FLOW
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowType = FLOW_LIQUID;
        holdup = liquidFraction;
        accelGrad = 0.0;
        
        if (fabs(liquidSuperficialVel) > VELOCITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * liquidDensity * liquidSuperficialVel * diameter / liquidViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = liquidSuperficialVel * liquidSuperficialVel;
            frictionGrad = frictionFactorValue * liquidDensity * velocitySquared /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = liquidDensity * sinAngle / PRESSURE_CONVERSION;
        totalGrad = frictionGrad + gravityGrad;
        return;
    }
    
    // SINGLE-PHASE GAS FLOW
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowType = FLOW_GAS;
        holdup = liquidFraction;
        
        if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
            const double gasReynolds = REYNOLDS_SCALE * gasDensity * gasSuperficialVel * diameter / gasViscosity;
            
            double frictionFactorValue;
            frictionFactor(gasReynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = gasSuperficialVel * gasSuperficialVel;
            frictionGrad = frictionFactorValue * gasDensity * velocitySquared /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
            
            double accelParam = gasDensity * velocitySquared / (g_in * pressure * PRESSURE_CONVERSION);
            
            if (accelParam > ACCEL_CRITICAL_LIMIT) {
                accelParam = ACCEL_CRITICAL_LIMIT;
                criticalFlag = 1;
            }
            
            gravityGrad = gasDensity * sinAngle / PRESSURE_CONVERSION;
            totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
            accelGrad = accelParam * totalGrad;
            reynolds = gasReynolds;
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
            gravityGrad = gasDensity * sinAngle / PRESSURE_CONVERSION;
            totalGrad = gravityGrad;
            accelGrad = 0.0;
        }
        return;
    }
    
    // TWO-PHASE FLOW - DIMENSIONLESS NUMBERS
    const double densitySurfaceTensionRoot = pow(liquidDensity / surfaceTension, 0.25);
    const double liquidVelocityNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * liquidSuperficialVel;
    const double gasVelocityNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * gasSuperficialVel;
    const double diameterNumber = DIAMETER_NUMBER_SCALE * diameter * sqrt(liquidDensity / surfaceTension);
    
    const double surfaceTensionCubed = surfaceTension * surfaceTension * surfaceTension;
    const double viscosityNumberRoot = pow(1.0 / (liquidDensity * surfaceTensionCubed), 0.25);
    const double liquidViscosityNumber = VISC_NUMBER_SCALE * liquidViscosity * viscosityNumberRoot;
    
    // FLOW REGIME DETERMINATION
    const double slugFlowLimit = SLUG_BASE_LIMIT + SLUG_SLOPE * liquidVelocityNumber;
    const double mistFlowLimit = MIST_BASE_LIMIT + MIST_SLOPE * pow(liquidVelocityNumber, MIST_EXPONENT);
    
    const double logDiameterNumber = log(diameterNumber);
    const double transitionY1 = CorrelationFits::dunsRosTransitionY1(logDiameterNumber);
    const double transitionY2 = CorrelationFits::dunsRosTransitionY2(logDiameterNumber);
    const double bubbleToSlugLimit = transitionY1 + transitionY2 * liquidVelocityNumber;
    
    bool inTransitionRegion = false;
    double slipNumber = 0.0;
    double slugFrictionGrad = 0.0;
    double slugGravityGrad = 0.0;
    
    // BUBBLE FLOW REGIME
    if (gasVelocityNumber <= bubbleToSlugLimit) {
        flowType = FLOW_BUBBLE;
        
        const double logViscosityNumber = log(liquidViscosityNumber);
        const double f1 = exp(CorrelationFits::dunsRosF1(logViscosityNumber));
        const double f2 = exp(CorrelationFits::dunsRosF2(logViscosityNumber));
        const double f3 = exp(CorrelationFits::dunsRosF3(logViscosityNumber));
        const double f4Raw = CorrelationFits::dunsRosF4(logViscosityNumber);
        const double f4 = f3 - f4Raw / diameterNumber;
        
        const double gasVelRatio = gasVelocityNumber / (1.0 + liquidVelocityNumber);
        slipNumber = f1 + f2 * liquidVelocityNumber + f4 * gasVelRatio * gasVelRatio;
    }
    // SLUG FLOW REGIME (OR TRANSITION TO MIST)
    else if (gasVelocityNumber <= mistFlowLimit) {
        flowType = FLOW_SLUG;
        
        if (gasVelocityNumber > slugFlowLimit) {
            inTransitionRegion = true;
        }
        
        const double logViscosityNumber = log(liquidViscosityNumber);
        const double f5 = exp(CorrelationFits::dunsRosF5(logViscosityNumber));
        const double f6 = CorrelationFits::dunsRosF6(logViscosityNumber);
        const double f7 = exp(CorrelationFits::dunsRosF7(logViscosityNumber));
        
        const double f6Prime = DIAMETER_CORRECTION * diameterNumber + f6;
        const double gasVelPower = pow(gasVelocityNumber, SLUG_GAS_EXPONENT);
        const double liquidVelFactor = pow(1.0 + f7 * liquidVelocityNumber, 2.0);
        
        slipNumber = (1.0 + f5) * (gasVelPower + f6Prime) / liquidVelFactor;
    }
    
    // BUBBLE/SLUG FLOW CALCULATIONS
    if (gasVelocityNumber <= mistFlowLimit) {
        double frictionFactorValue = 0.0;
        
        if (fabs(liquidSuperficialVel) > VELOCITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * liquidDensity * liquidSuperficialVel * diameter / liquidViscosity;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            // Apply two-phase friction correction
            const double velocityRatio = gasSuperficialVel / liquidSuperficialVel;
            const double frictionParam = frictionFactorValue * velocityRatio *
                                        pow(diameterNumber, DIAMETER_EXPONENT);
            const double logFrictionParam = log(frictionParam);
            
            const double ff2 = exp(CorrelationFits::dunsRosFrictionCorrection(logFrictionParam));
            const double ff3 = 1.0 + frictionFactorValue * sqrt(velocityRatio / FRICTION_CORRECTION_DENOM);
            
            frictionFactorValue = frictionFactorValue * ff2 / ff3;
        } else {
            reynolds = 0.0;
        }
        
        frictionGrad = frictionFactorValue * liquidDensity * liquidSuperficialVel * velocity /
                      (2.0 * g_in * diameter * PRESSURE_CONVERSION);
        
        // Calculate slip velocity and holdup
        const double slipVelocity = slipNumber / (VEL_NUMBER_SCALE * densitySurfaceTensionRoot);
        
        if (fabs(slipVelocity) > VELOCITY_TOLERANCE) {
            const double velocityDiff = velocity - slipVelocity;
            const double discriminant = velocityDiff * velocityDiff +
                                       4.0 * liquidSuperficialVel * slipVelocity;
            holdup = (-velocityDiff + sqrt(discriminant)) / (2.0 * slipVelocity);
        } else {
            holdup = liquidFraction;
        }
        
        // Enforce physical constraint
        if (holdup < liquidFraction) {
            holdup = liquidFraction;
        }
        
        const double mixtureDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
        gravityGrad = mixtureDensity * sinAngle / PRESSURE_CONVERSION;
        accelGrad = 0.0;
        
        if (!inTransitionRegion) {
            totalGrad = frictionGrad + gravityGrad;
            return;
        }
        
        // Store slug values for transition blending
        slugFrictionGrad = frictionGrad;
        slugGravityGrad = gravityGrad;
    }
    
    // MIST FLOW REGIME
    flowType = FLOW_MIST;
    
    // Calculate effective roughness for mist flow
    double mistRoughness;
    
    if (roughness > 0.0) {
        const double roughnessDiameter = roughness * diameter;
        const double velocitySquared = gasSuperficialVel * gasSuperficialVel;
        
        const double webberNumber = WEBBER_SCALE * gasDensity * velocitySquared *
                                   roughnessDiameter / surfaceTension;
        const double viscosityParameter = VISC_PARAM_SCALE * liquidViscosity * liquidViscosity /
                                         (liquidDensity * surfaceTension * roughnessDiameter);
        const double parameterProduct = webberNumber * viscosityParameter;
        
        if (parameterProduct > WEBBER_THRESHOLD) {
            const double c2 = 0.3713 * surfaceTension / (gasDensity * velocitySquared * diameter) *
                            pow(parameterProduct, 0.302);
            
            if (c2 > 0.001 && c2 < 3.45) {
                const double zeta = log(c2);
                mistRoughness = exp(-1.1117597 + zeta * (0.42096466 + zeta * (-8.0206350e-2 +
                               zeta * (7.2551561e-3 + zeta * (2.4853674e-3 +
                               zeta * (1.2567230e-4 - 3.2648140e-6 * zeta))))));
            } else if (c2 < 0.001) {
                mistRoughness = c2;
            } else {
                mistRoughness = MAX_MIST_ROUGHNESS;
            }
        } else {
            const double c1 = gasDensity * velocitySquared * diameter /
                            (SURFACE_TENSION_SCALE * surfaceTension);
            const double discriminantHelper = sqrt(-4.0 / (3.0 * c1) + 0.25);
            const double aux = c1 * c1 * discriminantHelper;
            
            double y1;
            if ((0.5 * c1 - aux) > 0.0) {
                y1 = pow(0.5 * c1 + aux, 1.0/3.0) + pow(0.5 * c1 - aux, 1.0/3.0);
            } else {
                y1 = pow(0.5 * c1 + aux, 1.0/3.0) - pow(-0.5 * c1 + aux, 1.0/3.0);
            }
            
            const double sqrtY1 = sqrt(y1);
            mistRoughness = 1.0 - sqrtY1 + 0.5 * sqrt(2.0 * y1 + 2.0 * sqrt(y1 * y1 + 4.0 * c1));
            
            if (mistRoughness > MAX_MIST_ROUGHNESS) {
                mistRoughness = MAX_MIST_ROUGHNESS;
            }
        }
    } else {
        mistRoughness = DEFAULT_MIST_ROUGHNESS;
    }
    
    // Calculate mist flow gradients
    double mistFrictionGrad;
    double gasReynolds;
    
    if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
        const double correctedGasVel = gasSuperficialVel / pow(1.0 - mistRoughness, 2.0);
        gasReynolds = REYNOLDS_SCALE * gasDensity * correctedGasVel * diameter / gasViscosity;
        
        double frictionFactorValue;
        if (mistRoughness >= HIGH_ROUGHNESS_THRESHOLD) {
            const double logTerm = log10(0.27 * mistRoughness);
            frictionFactorValue = 4.0 * (1.0 / pow(4.0 * logTerm, 2.0) +
                                        0.067 * pow(mistRoughness, 1.73));
        } else {
            frictionFactor(gasReynolds, mistRoughness, frictionFactorValue);
        }
        
        const double correctedVelSquared = correctedGasVel * correctedGasVel;
        mistFrictionGrad = frictionFactorValue * gasDensity * correctedVelSquared /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
    } else {
        gasReynolds = 0.0;
        mistFrictionGrad = 0.0;
    }
    
    const double mixtureDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    const double mistGravityGrad = mixtureDensity * sinAngle / PRESSURE_CONVERSION;
    
    double accelParam;
    if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
        const double correctedGasVel = gasSuperficialVel / pow(1.0 - mistRoughness, 2.0);
        accelParam = mixtureDensity * velocity * correctedGasVel /
                    (g_in * pressure * PRESSURE_CONVERSION);
        
        if (accelParam > ACCEL_CRITICAL_LIMIT) {
            accelParam = ACCEL_CRITICAL_LIMIT;
            criticalFlag = 1;
        }
    } else {
        accelParam = 0.0;
    }
    
    if (!inTransitionRegion) {
        frictionGrad = mistFrictionGrad;
        gravityGrad = mistGravityGrad;
        totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
        accelGrad = accelParam * totalGrad;
        reynolds = gasReynolds;
        holdup = liquidFraction;
        return;
    }
    
    // TRANSITION REGIME (SLUG-TO-MIST BLENDING)
    flowType = FLOW_TRANSITION;
    
    const double mistTotalGrad = (mistFrictionGrad + mistGravityGrad * gasVelocityNumber / mistFlowLimit) /
                                (1.0 - accelParam);
    const double mistAccelGrad = accelParam * mistTotalGrad;
    
    const double slugWeight = (mistFlowLimit - gasVelocityNumber) / (mistFlowLimit - slugFlowLimit);
    const double mistWeight = 1.0 - slugWeight;
    
    frictionGrad = slugWeight * slugFrictionGrad + mistWeight * mistFrictionGrad;
    gravityGrad = slugWeight * slugGravityGrad + mistWeight * mistGravityGrad;
    accelGrad = mistWeight * mistAccelGrad;
    totalGrad = frictionGrad + gravityGrad + accelGrad;
    reynolds = gasReynolds;
}

/*
==============================================================================
orkiszewski: Calculates pressure gradients, liquid holdup, and vertical
             two-phase flow properties using the Orkiszewski correlation.

References:
      1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984), Sections 3-33 through 3-42.
      2.  Orkiszewski, J.: "Predicting Two Phase Pressure Drops in
          Vertical Pipes," Journal of Petroleum Technology
          (June 1967), 829-838.

Variable dictionary:
- numEntrCorr    : Number of inputs or correlation scenario
- angle          : Pipe inclination angle (degrees)
- diameter       : Pipe inner diameter (inches)
- roughness       : Pipe relative roughness
- pressure        : Pressure (psi)
- velocity        : Average flow velocity (ft/s)
- liquidFraction  : Liquid volumetric fraction
- gasDensity      : Gas-phase density (lb/ft3)
- liquidDensity   : Liquid-phase density (lb/ft3)
- gasViscosity    : Gas-phase viscosity (cP)
- liquidViscosity : Liquid-phase viscosity (cP)
- surfaceTension  : Surface tension (dyn/cm)
- liquidVelNumber : Dimensionless liquid velocity number
- gasVelNumber    : Dimensionless gas velocity number
- waterFraction   : Water mass fraction in the liquid phase
- holdup          : Calculated liquid holdup
- frictionGrad    : Frictional pressure gradient (psi/ft)
- gravityGrad     : Gravitational pressure gradient (psi/ft)
- accelGrad       : Accelerational pressure gradient (psi/ft)
- totalGrad       : Total pressure gradient (psi/ft)
- reynolds        : Liquid Reynolds number
- liquidDistrib   : Liquid distribution coefficient between phases
- gasSupVel       : Superficial gas velocity
- gasSupVelCorr   : Superficial gas velocity corrected for mist flow
- liquidSupVel    : Superficial liquid velocity
- flowType        : Flow-type indicator
                    (1 = liquid, 2 = gas, 3 = bubble, 4 = slug,
                     5 = mist, 6 = transition)
- criticalFlag    : Critical-flow indicator
- convergeFlag    : Convergence indicator
- isigFlag        : Flag used for liquidDistrib adjustments
==============================================================================
*/
void orkiszewski(double angle, double diameter, double roughness,
                double pressure, double velocity, double liquidFraction, double gasDensity,
                double liquidDensity, double gasViscosity, double liquidViscosity,
                double surfaceTension, double temperature, double compressibilityFactor, double waterFraction,
                double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                double& totalGrad, double& reynolds, unsigned char& flowType,
                unsigned char& criticalFlag, unsigned char& convergeFlag, unsigned char& isigFlag) {
    
    // ORKISZEWSKI CORRELATION DATA (replaced by analytical fits in CorrelationFits.h)
    
    // ORKISZEWSKI CONSTANTS
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_BUBBLE = 3;
    static constexpr unsigned char FLOW_SLUG = 4;
    static constexpr unsigned char FLOW_MIST = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.99999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.00001;
    static constexpr double VELOCITY_TOLERANCE = 1.0e-15;
    
    // Dimensionless number coefficients
    static constexpr double VEL_NUMBER_SCALE = 1.938;
    
    // Flow regime boundaries
    static constexpr double SLUG_BASE_LIMIT = 50.0;
    static constexpr double SLUG_SLOPE = 36.0;
    static constexpr double MIST_BASE_LIMIT = 75.0;
    static constexpr double MIST_SLOPE = 84.0;
    static constexpr double MIST_EXPONENT = 0.75;
    
    // Bubble flow limits (Griffith-Wallis style)
    static constexpr double BUBBLE_LIMIT_BASE = 1.071;
    static constexpr double BUBBLE_LIMIT_COEFF = 0.2218;
    static constexpr double BUBBLE_LIMIT_MIN = 0.13;
    static constexpr double GRIFFITH_REFERENCE_VELOCITY = 0.8;  // ft/s
    
    // Slug flow bubble velocity coefficients
    static constexpr double BUBBLE_VEL_COEFF_C1 = 0.35;        // Base coefficient
    static constexpr double BUBBLE_VEL_TRANS_BASE = 0.251;     // Transition base
    static constexpr double BUBBLE_VEL_TRANS_SLOPE = 8.74e-6;  // Transition slope
    static constexpr double BUBBLE_VEL_VISC_COEFF = 13.59;     // Viscosity coefficient
    static constexpr double BUBBLE_VEL_LOW_RE = 0.546;         // Low Reynolds coefficient
    static constexpr double BUBBLE_VEL_HIGH_RE = 0.350;        // High Reynolds coefficient
    
    // Reynolds number transition ranges
    static constexpr double RE_BUBBLE_LOW_START = 2980.0;
    static constexpr double RE_BUBBLE_LOW_END = 3020.0;
    static constexpr double RE_BUBBLE_HIGH_START = 7980.0;
    static constexpr double RE_BUBBLE_HIGH_END = 8020.0;
    static constexpr double RE_LIQUID_TRANSITION = 6000.0;
    
    // Liquid distribution coefficient calculation
    static constexpr double DISTRIB_VISC_COEFF = 0.01;
    static constexpr double DISTRIB_DIAM_EXP_1 = -1.571;
    static constexpr double DISTRIB_OFFSET_1 = 0.397;
    static constexpr double DISTRIB_DIAM_COEFF_1 = 0.63;
    
    // Water-dominated liquid distribution (case 3,4)
    static constexpr double WATER_THRESHOLD = 0.75;
    
    // Velocity threshold for different liquid distribution correlations
    static constexpr double VEL_DISTRIB_THRESHOLD = 10.0;  // ft/s
    static constexpr double DISTRIB_VEL_SLOPE = -0.065;    // Minimum slope constraint
    
    // Mist flow parameters (same as Duns-Ros)
    static constexpr double WEBBER_SCALE = 453.59;
    static constexpr double VISC_PARAM_SCALE = 0.0002048;
    static constexpr double SURFACE_TENSION_SCALE = 0.0749;
    static constexpr double WEBBER_THRESHOLD = 0.005;
    static constexpr double MAX_MIST_ROUGHNESS = 0.5;
    static constexpr double HIGH_ROUGHNESS_THRESHOLD = 0.05;
    static constexpr double DEFAULT_MIST_ROUGHNESS = 1.0e-5;
    
    // Iteration control
    static constexpr int MAX_BUBBLE_VEL_ITERATIONS = 100;
    static constexpr double BUBBLE_VEL_TOLERANCE = 0.001;    // ft/s
    static constexpr double BUBBLE_VEL_RELAXATION = 0.2;     // Relaxation factor
    
    // Critical flow
    static constexpr double ACCEL_CRITICAL_LIMIT = 0.95;
    static constexpr double MIST_TRANSITION_LIMIT = 1200.0;  // Gas velocity number
    
    // Physical constants
    static constexpr double PRESSURE_CONVERSION = 144.0;     // psi to lbf/ft²
    static constexpr double REYNOLDS_SCALE = 1488.0;         // Reynolds number scale
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    
    // INITIALIZATION
    flowType = 0;
    criticalFlag = 0;
    convergeFlag = 0;
    
    const double sinAngle = sin(angle * DEG_TO_RAD);
    const double liquidSuperficialVel = velocity * liquidFraction;
    const double gasSuperficialVel = velocity * (1.0 - liquidFraction);
    
    // DIMENSIONLESS NUMBERS
    const double densitySurfaceTensionRoot = pow(liquidDensity / surfaceTension, 0.25);
    const double liquidVelocityNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * liquidSuperficialVel;
    const double gasVelocityNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * gasSuperficialVel;
    
    // Calculate mixture Reynolds numbers
    double reynoldsMixture = 0.0;
    double reynoldsGas = 0.0;
    
    if (fabs(velocity) > VELOCITY_TOLERANCE) {
        reynoldsMixture = REYNOLDS_SCALE * liquidDensity * velocity * diameter / liquidViscosity;
    }
    
    if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
        reynoldsGas = REYNOLDS_SCALE * gasDensity * gasSuperficialVel * diameter / gasViscosity;
    }
    
    // SINGLE-PHASE LIQUID FLOW
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowType = FLOW_LIQUID;
        holdup = liquidFraction;
        accelGrad = 0.0;
        
        if (fabs(liquidSuperficialVel) > VELOCITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * liquidDensity * liquidSuperficialVel * diameter / liquidViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = liquidSuperficialVel * liquidSuperficialVel;
            frictionGrad = (frictionFactorValue * liquidDensity * velocitySquared) /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = liquidDensity * sinAngle / PRESSURE_CONVERSION;
        totalGrad = frictionGrad + gravityGrad;
        return;
    }
    
    // SINGLE-PHASE GAS FLOW
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowType = FLOW_GAS;
        holdup = liquidFraction;
        
        if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
            reynoldsGas = REYNOLDS_SCALE * gasDensity * gasSuperficialVel * diameter / gasViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynoldsGas, roughness, frictionFactorValue);
            
            const double velocitySquared = gasSuperficialVel * gasSuperficialVel;
            frictionGrad = (frictionFactorValue * gasDensity * velocitySquared) /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
            
            double accelParam = (gasDensity * velocitySquared) / (g_in * pressure * PRESSURE_CONVERSION);
            
            if (accelParam > ACCEL_CRITICAL_LIMIT) {
                accelParam = ACCEL_CRITICAL_LIMIT;
                criticalFlag = 1;
            }
            
            gravityGrad = gasDensity * sinAngle / PRESSURE_CONVERSION;
            totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
            accelGrad = accelParam * totalGrad;
            reynolds = reynoldsGas;
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
            gravityGrad = gasDensity * sinAngle / PRESSURE_CONVERSION;
            totalGrad = gravityGrad;
            accelGrad = 0.0;
        }
        return;
    }
    
    // TWO-PHASE FLOW - REGIME DETERMINATION
    const double slugFlowLimit = SLUG_BASE_LIMIT + SLUG_SLOPE * liquidVelocityNumber;
    const double mistFlowLimit = MIST_BASE_LIMIT + MIST_SLOPE * pow(liquidVelocityNumber, MIST_EXPONENT);
    
    double gasFraction;
    if (fabs(velocity) > VELOCITY_TOLERANCE) {
        gasFraction = gasSuperficialVel / velocity;
    } else {
        gasFraction = 1.0 - liquidFraction;
    }
    
    // Bubble flow limit (Griffith-Wallis style)
    double bubbleFlowLimit = BUBBLE_LIMIT_BASE - BUBBLE_LIMIT_COEFF * velocity * velocity / diameter;
    if (bubbleFlowLimit < BUBBLE_LIMIT_MIN) {
        bubbleFlowLimit = BUBBLE_LIMIT_MIN;
    }
    
    bool inTransitionRegion = false;
    double slugFrictionGrad = 0.0;
    double slugGravityGrad = 0.0;
    
    // BUBBLE FLOW REGIME (Griffith-Wallis)
    if (gasFraction < bubbleFlowLimit) {
        flowType = FLOW_BUBBLE;
        
        if (fabs(velocity) > VELOCITY_TOLERANCE) {
            const double refVel = GRIFFITH_REFERENCE_VELOCITY;
            const double velRatio = velocity / refVel;
            const double gasRatio = gasSuperficialVel / refVel;
            const double discriminant = (1.0 + velRatio) * (1.0 + velRatio) - 4.0 * gasRatio / refVel;
            
            holdup = 1.0 - 0.5 * (1.0 + velRatio - sqrt(discriminant));
            
            if (holdup < liquidFraction) {
                holdup = liquidFraction;
            }
            
            const double mixtureDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
            gravityGrad = mixtureDensity * sinAngle / PRESSURE_CONVERSION;
            
            if (fabs(holdup) > VELOCITY_TOLERANCE) {
                const double liquidVelInSlug = liquidSuperficialVel / holdup;
                const double reynoldsBubble = REYNOLDS_SCALE * liquidDensity * liquidVelInSlug * diameter / liquidViscosity;
                
                double frictionFactorValue;
                frictionFactor(reynoldsBubble, roughness, frictionFactorValue);
                
                const double liquidVelSquared = liquidVelInSlug * liquidVelInSlug;
                frictionGrad = (frictionFactorValue * liquidDensity * liquidVelSquared) /
                              (2.0 * g_in * diameter * PRESSURE_CONVERSION);
                reynolds = reynoldsBubble;
            } else {
                frictionGrad = 0.0;
                reynolds = 0.0;
            }
            
            accelGrad = 0.0;
            totalGrad = frictionGrad + gravityGrad;
        } else {
            holdup = liquidFraction;
            const double mixtureDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
            
            reynolds = 0.0;
            frictionGrad = 0.0;
            gravityGrad = mixtureDensity * sinAngle / PRESSURE_CONVERSION;
            accelGrad = 0.0;
            totalGrad = gravityGrad;
        }
        
        return;
    }
    
    // SLUG FLOW REGIME
    if (gasVelocityNumber < mistFlowLimit) {
        flowType = FLOW_SLUG;
        
        if (gasVelocityNumber >= slugFlowLimit && gasVelocityNumber <= MIST_TRANSITION_LIMIT) {
            inTransitionRegion = true;
        }
        
        // LIQUID DISTRIBUTION COEFFICIENT
        // Determine continuous phase (oil or water)
        const bool isWaterContinuous = (waterFraction >= WATER_THRESHOLD);
        const bool isHighVelocity = (velocity > VEL_DISTRIB_THRESHOLD);
        
        // Common terms
        const double viscTerm = DISTRIB_VISC_COEFF * log10(liquidViscosity + 1.0) *
                               pow(diameter, DISTRIB_DIAM_EXP_1);
        const double x2Term = viscTerm + DISTRIB_OFFSET_1 + DISTRIB_DIAM_COEFF_1 * log10(diameter);
        const double x3Term = -log10(velocity) * x2Term;
        
        double liquidDistribCoeff = 0.0;
        
        if (!isWaterContinuous && !isHighVelocity) {
            // Case 1: Oil continuous, low velocity
            const double xx2 = 0.0127 * log10(liquidViscosity + 1.0) * pow(diameter, -1.415);
            liquidDistribCoeff = xx2 - 0.284 + 0.167 * log10(velocity) + 0.113 * log10(diameter);
            
        } else if (!isWaterContinuous && isHighVelocity) {
            // Case 2: Oil continuous, high velocity
            if (isigFlag <= 0) {
                const double xx3 = 0.0274 * log10(liquidViscosity + 1.0) * pow(diameter, -1.371);
                liquidDistribCoeff = xx3 + 0.161 + 0.569 * log10(diameter) + x3Term;
            } else {
                const double xx3 = 0.0127 * log10(liquidViscosity + 1.0) * pow(diameter, -1.415);
                liquidDistribCoeff = xx3 - 0.117 + 0.113 * log10(diameter) + x2Term + x3Term;
            }
            
        } else if (isWaterContinuous && !isHighVelocity) {
            // Case 3: Water continuous, low velocity
            const double xx4 = 0.013 * log10(liquidViscosity) * pow(diameter, -1.38);
            liquidDistribCoeff = xx4 - 0.681 + 0.232 * log10(velocity) - 0.428 * log10(diameter);
            
        } else {
            // Case 4: Water continuous, high velocity
            if (isigFlag <= 0) {
                const double xx5 = 0.045 * log10(liquidViscosity) * pow(diameter, -0.799);
                liquidDistribCoeff = xx5 - 0.709 - 0.162 * log10(velocity) - 0.888 * log10(diameter);
            } else {
                const double xx5 = 0.013 * log10(liquidViscosity) * pow(diameter, -1.38);
                liquidDistribCoeff = xx5 - 0.287 - 0.162 * log10(velocity) - 0.428 * log10(diameter);
            }
        }
        
        // Apply constraint for low velocities
        if (velocity < VEL_DISTRIB_THRESHOLD) {
            const double minDistrib = DISTRIB_VEL_SLOPE * velocity;
            if (liquidDistribCoeff < minDistrib) {
                liquidDistribCoeff = minDistrib;
            }
        }
        
        // BUBBLE RISE VELOCITY (Iterative Calculation)
        const double sqrtGravDiam = sqrt(g_in * diameter);
        double bubbleVelocity = 0.5 * sqrtGravDiam;  // Initial guess
        double bubbleVelCalculated = bubbleVelocity;  // Store calculated value
        bool converged = false;
        int iterCount = 0;
        
        while (!converged && iterCount < MAX_BUBBLE_VEL_ITERATIONS) {
            iterCount++;
            
            const double reynoldsBubble = REYNOLDS_SCALE * liquidDensity * bubbleVelocity * diameter / liquidViscosity;
            
            if (reynoldsMixture <= RE_LIQUID_TRANSITION) {
                // Use analytical fit for bubble velocity coefficient
                const double c2Coeff = CorrelationFits::orkiszewskiC2(reynoldsBubble, reynoldsMixture);
                
                bubbleVelCalculated = BUBBLE_VEL_COEFF_C1 * c2Coeff * sqrtGravDiam;
                
            } else {
                // Use transition equations for high Reynolds numbers
                const double transTerm = (BUBBLE_VEL_TRANS_BASE + BUBBLE_VEL_TRANS_SLOPE * reynoldsMixture) * sqrtGravDiam;
                const double viscTerm = (BUBBLE_VEL_VISC_COEFF * liquidViscosity) /
                                       (liquidDensity * sqrt(diameter));
                
                bubbleVelCalculated = 0.5 * (transTerm + sqrt(transTerm * transTerm + viscTerm));
                
                // Handle Reynolds number transition regions
                if (reynoldsBubble <= RE_BUBBLE_LOW_START) {
                    bubbleVelCalculated = (BUBBLE_VEL_LOW_RE + BUBBLE_VEL_TRANS_SLOPE * reynoldsMixture) * sqrtGravDiam;
                    
                } else if (reynoldsBubble >= RE_BUBBLE_HIGH_END) {
                    bubbleVelCalculated = (BUBBLE_VEL_HIGH_RE + BUBBLE_VEL_TRANS_SLOPE * reynoldsMixture) * sqrtGravDiam;
                    
                } else if (reynoldsBubble > RE_BUBBLE_LOW_START && reynoldsBubble < RE_BUBBLE_LOW_END) {
                    // Linear interpolation in low Re transition
                    const double bubbleVel1 = 0.5 * (transTerm + sqrt(transTerm * transTerm + viscTerm));
                    const double bubbleVel2 = (BUBBLE_VEL_LOW_RE + BUBBLE_VEL_TRANS_SLOPE * reynoldsMixture) * sqrtGravDiam;
                    const double weight = (reynoldsBubble - RE_BUBBLE_LOW_START) /
                                         (RE_BUBBLE_LOW_END - RE_BUBBLE_LOW_START);
                    bubbleVelCalculated = bubbleVel2 * (1.0 - weight) + bubbleVel1 * weight;
                    
                } else if (reynoldsBubble > RE_BUBBLE_HIGH_START && reynoldsBubble < RE_BUBBLE_HIGH_END) {
                    // Linear interpolation in high Re transition
                    const double bubbleVel1 = 0.5 * (transTerm + sqrt(transTerm * transTerm + viscTerm));
                    const double bubbleVel2 = (BUBBLE_VEL_HIGH_RE + BUBBLE_VEL_TRANS_SLOPE * reynoldsMixture) * sqrtGravDiam;
                    const double weight = (reynoldsBubble - RE_BUBBLE_HIGH_START) /
                                         (RE_BUBBLE_HIGH_END - RE_BUBBLE_HIGH_START);
                    bubbleVelCalculated = bubbleVel2 * weight + bubbleVel1 * (1.0 - weight);
                }
            }
            
            // Check convergence
            if (fabs(bubbleVelCalculated - bubbleVelocity) < BUBBLE_VEL_TOLERANCE) {
                converged = true;
            } else {
                // Relaxation for stability
                bubbleVelocity += BUBBLE_VEL_RELAXATION * (bubbleVelCalculated - bubbleVelocity);
            }
        }
        
        if (iterCount >= MAX_BUBBLE_VEL_ITERATIONS) {
            convergeFlag = 1;  // Failed to converge
        }
        
        // Use the final calculated value for all subsequent calculations
        const double finalBubbleVel = bubbleVelCalculated;
        
        // MIXTURE DENSITY WITH LIQUID DISTRIBUTION
        double mixtureDensity;
        bool needsDistribAdjustment = true;
        unsigned char ksigFlag = 0;
        
        while (needsDistribAdjustment) {
            if (fabs(velocity + finalBubbleVel) > VELOCITY_TOLERANCE) {
                mixtureDensity = (liquidDensity * (liquidSuperficialVel + finalBubbleVel) +
                                 gasDensity * gasSuperficialVel) / (velocity + finalBubbleVel) +
                                liquidDensity * liquidDistribCoeff;
                
                // Check if adjustment needed for high velocity
                if (isHighVelocity && isigFlag <= 0) {
                    const double minDistribForDensity = -finalBubbleVel * (1.0 - mixtureDensity / liquidDensity) /
                                                        (velocity + finalBubbleVel);
                    
                    if (liquidDistribCoeff <= minDistribForDensity && ksigFlag != 1) {
                        liquidDistribCoeff = minDistribForDensity;
                        ksigFlag = 1;
                    } else {
                        needsDistribAdjustment = false;
                    }
                } else {
                    needsDistribAdjustment = false;
                }
            } else {
                mixtureDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
                needsDistribAdjustment = false;
            }
        }
        
        // Calculate holdup from mixture density
        holdup = (mixtureDensity - gasDensity) / (liquidDensity - gasDensity);
        
        // PRESSURE GRADIENTS FOR SLUG FLOW
        gravityGrad = mixtureDensity * sinAngle / PRESSURE_CONVERSION;
        
        if (fabs(velocity) > VELOCITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * liquidDensity * velocity * diameter / liquidViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            if (fabs(velocity + finalBubbleVel) > VELOCITY_TOLERANCE) {
                const double frictionMultiplier = (liquidSuperficialVel + finalBubbleVel) /
                                                  (velocity + finalBubbleVel) + liquidDistribCoeff;
                frictionGrad = (frictionFactorValue * liquidDensity * velocity * velocity * frictionMultiplier) /
                              (2.0 * g_in * diameter * PRESSURE_CONVERSION);
            } else {
                frictionGrad = 0.0;
            }
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        accelGrad = 0.0;
        
        if (!inTransitionRegion) {
            totalGrad = frictionGrad + gravityGrad;
            return;
        }
        
        // Store slug values for transition blending
        slugFrictionGrad = frictionGrad;
        slugGravityGrad = gravityGrad;
    }
    
    // MIST FLOW REGIME
    flowType = FLOW_MIST;
    
    // Calculate effective roughness for mist flow (same algorithm as Duns-Ros)
    double mistRoughness;
    
    if (roughness > 0.0) {
        const double roughnessDiameter = roughness * diameter;
        const double velocitySquared = gasSuperficialVel * gasSuperficialVel;
        
        const double webberNumber = WEBBER_SCALE * gasDensity * velocitySquared *
                                   roughnessDiameter / surfaceTension;
        const double viscosityParameter = VISC_PARAM_SCALE * liquidViscosity * liquidViscosity /
                                         (liquidDensity * surfaceTension * roughnessDiameter);
        const double parameterProduct = webberNumber * viscosityParameter;
        
        if (parameterProduct > WEBBER_THRESHOLD) {
            const double c2 = 0.3713 * surfaceTension / (gasDensity * velocitySquared * diameter) *
                            pow(parameterProduct, 0.302);
            
            if (c2 > 0.001 && c2 < 3.45) {
                const double zeta = log(c2);
                mistRoughness = exp(-1.1117597 + zeta * (0.42096466 + zeta * (-8.0206350e-2 +
                               zeta * (7.2551561e-3 + zeta * (2.4853674e-3 +
                               zeta * (1.2567230e-4 - 3.2648140e-6 * zeta))))));
            } else if (c2 < 0.001) {
                mistRoughness = c2;
            } else {
                mistRoughness = MAX_MIST_ROUGHNESS;
            }
        } else {
            const double c1 = gasDensity * velocitySquared * diameter /
                            (SURFACE_TENSION_SCALE * surfaceTension);
            const double discriminantHelper = sqrt(-4.0 / (3.0 * c1) + 0.25);
            const double aux = c1 * c1 * discriminantHelper;
            
            double y1;
            if ((0.5 * c1 - aux) > 0.0) {
                y1 = pow(0.5 * c1 + aux, 1.0/3.0) + pow(0.5 * c1 - aux, 1.0/3.0);
            } else {
                y1 = pow(0.5 * c1 + aux, 1.0/3.0) - pow(-0.5 * c1 + aux, 1.0/3.0);
            }
            
            const double sqrtY1 = sqrt(y1);
            mistRoughness = 1.0 - sqrtY1 + 0.5 * sqrt(2.0 * y1 + 2.0 * sqrt(y1 * y1 + 4.0 * c1));
            
            if (mistRoughness > MAX_MIST_ROUGHNESS) {
                mistRoughness = MAX_MIST_ROUGHNESS;
            }
        }
    } else {
        mistRoughness = DEFAULT_MIST_ROUGHNESS;
    }
    
    // Calculate mist flow gradients
    double mistFrictionGrad;
    double correctedGasVel;
    
    if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
        correctedGasVel = gasSuperficialVel / pow(1.0 - mistRoughness, 2.0);
        const double reynoldsGasCorr = REYNOLDS_SCALE * gasDensity * correctedGasVel * diameter / gasViscosity;
        
        double frictionFactorValue;
        if (mistRoughness >= HIGH_ROUGHNESS_THRESHOLD) {
            const double logTerm = log10(0.27 * mistRoughness);
            frictionFactorValue = 4.0 * (1.0 / pow(4.0 * logTerm, 2.0) +
                                        0.067 * pow(mistRoughness, 1.73));
        } else {
            frictionFactor(reynoldsGasCorr, mistRoughness, frictionFactorValue);
        }
        
        const double correctedVelSquared = correctedGasVel * correctedGasVel;
        mistFrictionGrad = (frictionFactorValue * gasDensity * correctedVelSquared) /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
    } else {
        correctedGasVel = 0.0;
        mistFrictionGrad = 0.0;
    }
    
    const double mixtureDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    const double mistGravityGrad = mixtureDensity * sinAngle / PRESSURE_CONVERSION;
    
    double accelParam;
    if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
        accelParam = (mixtureDensity * velocity * correctedGasVel) /
                    (g_in * pressure * PRESSURE_CONVERSION);
        
        if (accelParam > ACCEL_CRITICAL_LIMIT) {
            accelParam = ACCEL_CRITICAL_LIMIT;
            criticalFlag = 1;
        }
    } else {
        accelParam = 0.0;
    }
    
    if (!inTransitionRegion) {
        frictionGrad = mistFrictionGrad;
        gravityGrad = mistGravityGrad;
        totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
        accelGrad = accelParam * totalGrad;
        reynolds = reynoldsMixture;
        holdup = liquidFraction;
        return;
    }
    
    // TRANSITION REGIME (SLUG-TO-MIST BLENDING)
    flowType = FLOW_TRANSITION;
    
    const double mistTotalGrad = (mistFrictionGrad + mistGravityGrad * gasVelocityNumber / mistFlowLimit) /
                                (1.0 - accelParam);
    const double mistAccelGrad = accelParam * mistTotalGrad;
    
    const double slugWeight = (mistFlowLimit - gasVelocityNumber) / (mistFlowLimit - slugFlowLimit);
    const double mistWeight = 1.0 - slugWeight;
    
    frictionGrad = slugWeight * slugFrictionGrad + mistWeight * mistFrictionGrad;
    gravityGrad = slugWeight * slugGravityGrad + mistWeight * mistGravityGrad;
    accelGrad = mistWeight * mistAccelGrad;
    totalGrad = frictionGrad + gravityGrad + accelGrad;
    reynolds = reynoldsMixture;
}

/*
==============================================================================
beggsAndBrill: Calculates pressure gradients, liquid holdup, and two-phase
               flow properties using the Beggs-Brill correlation.

References:
      1.  Beggs, H. D. and Brill, J. P.: "A Study of Two-Phase Flow in
          Inclined Pipes," Journal of Petroleum Technology (May 1973),
          607-617.
      2.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
          (Feb. 1984), Sections 3-53 through 3-64.

Variable dictionary:
- angle          : Pipe inclination angle (degrees)
- diameter       : Pipe inner diameter (inches)
- roughness      : Pipe relative roughness
- pressure       : Pressure (psi)
- velocity       : Total superficial velocity (ft/s)
- liquidFraction : Liquid volumetric fraction
- gasDensity     : Gas-phase density (lb/ft3)
- liquidDensity  : Liquid-phase density (lb/ft3)
- gasViscosity   : Gas-phase viscosity (cP)
- liquidViscosity: Liquid-phase viscosity (cP)
- surfaceTension : Surface tension (dyn/cm)
- holdup         : Calculated liquid holdup
- frictionGrad   : Frictional pressure gradient (psi/ft)
- gravityGrad    : Gravitational pressure gradient (psi/ft)
- accelGrad      : Accelerational pressure gradient (psi/ft)
- totalGrad      : Total pressure gradient (psi/ft)
- reynolds       : Mixture Reynolds number
- flowType       : Flow-pattern indicator
                   (1 = liquid, 2 = gas, 3 = distributed,
                    4 = intermittent, 5 = segregated, 6 = transition)
- palmerFlag     : Indicates whether the Palmer correction factor is used
                   (0 = without correction, 1 = with correction)
- criticalFlag   : Critical-flow indicator (0 = normal, 1 = critical)
==============================================================================
*/
void beggsAndBrill(double angle, double diameter, double roughness, double pressure, double velocity,
                   double liquidFraction, double gasDensity, double liquidDensity,
                   double gasViscosity, double liquidViscosity, double surfaceTension,
                   double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                   double& totalGrad, double& reynolds, unsigned char& flowType,
                   unsigned char palmerFlag, unsigned char& criticalFlag) {
    
    // CONSTANTS
    
    // Single-phase flow thresholds
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.99999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.00001;
    
    // Named flow types
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_DISTRIBUTED = 3;
    static constexpr unsigned char FLOW_INTERMITTENT = 4;
    static constexpr unsigned char FLOW_SEGREGATED = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    // Flow pattern transition boundaries
    static constexpr double TRANSITION_A_COEFF = 316.0;
    static constexpr double TRANSITION_A_EXP = 0.302;
    static constexpr double TRANSITION_B_COEFF = 0.0009252;
    static constexpr double TRANSITION_B_EXP = -2.46842;
    static constexpr double TRANSITION_C_COEFF = 0.1;
    static constexpr double TRANSITION_C_EXP = -1.45155;
    static constexpr double TRANSITION_D_COEFF = 0.5;
    static constexpr double TRANSITION_D_EXP = -6.738;
    static constexpr double LIQUID_FRACTION_LOW = 0.01;
    static constexpr double LIQUID_FRACTION_HIGH = 0.4;
    
    // Horizontal holdup correlations (flowType-specific)
    // Distributed flow (type 3)
    static constexpr double DISTRIBUTED_COEFF = 1.065;
    static constexpr double DISTRIBUTED_LAMBDA_EXP = 0.5824;
    static constexpr double DISTRIBUTED_FROUDE_EXP = -0.0609;
    
    // Intermittent flow (type 4)
    static constexpr double INTERMITTENT_COEFF = 0.845;
    static constexpr double INTERMITTENT_LAMBDA_EXP = 0.5351;
    static constexpr double INTERMITTENT_FROUDE_EXP = -0.0173;
    
    // Segregated flow (type 5)
    static constexpr double SEGREGATED_COEFF = 0.98;
    static constexpr double SEGREGATED_LAMBDA_EXP = 0.4846;
    static constexpr double SEGREGATED_FROUDE_EXP = -0.0868;
    
    // C-factor coefficients (angle correction)
    // Upward/horizontal flow
    static constexpr double C_COEFF_D_DISTRIBUTED = 1.0;
    static constexpr double C_COEFF_E_DISTRIBUTED = 0.0;
    static constexpr double C_COEFF_F_DISTRIBUTED = 0.0;
    static constexpr double C_COEFF_G_DISTRIBUTED = 0.0;
    
    static constexpr double C_COEFF_D_INTERMITTENT = 2.96;
    static constexpr double C_COEFF_E_INTERMITTENT = 0.305;
    static constexpr double C_COEFF_F_INTERMITTENT = -0.4473;
    static constexpr double C_COEFF_G_INTERMITTENT = 0.0978;
    
    static constexpr double C_COEFF_D_SEGREGATED = 0.011;
    static constexpr double C_COEFF_E_SEGREGATED = -3.768;
    static constexpr double C_COEFF_F_SEGREGATED = 3.539;
    static constexpr double C_COEFF_G_SEGREGATED = -1.614;
    
    // Downward flow
    static constexpr double C_COEFF_D_DOWNWARD = 4.7;
    static constexpr double C_COEFF_E_DOWNWARD = -0.3692;
    static constexpr double C_COEFF_F_DOWNWARD = 0.1244;
    static constexpr double C_COEFF_G_DOWNWARD = -0.5056;
    
    // Angle correction factors
    static constexpr double ANGLE_SCALE_FACTOR = 1.8;
    static constexpr double ANGLE_CUBIC_COEFF = 0.333;
    
    // Palmer correction factors
    static constexpr double PALMER_DOWNWARD_FACTOR = 0.685;
    static constexpr double PALMER_UPWARD_FACTOR = 0.924;
    
    // Two-phase friction factor correlation
    static constexpr double FRICTION_S_COEFF_A = -0.0523;
    static constexpr double FRICTION_S_COEFF_B = 3.182;
    static constexpr double FRICTION_S_COEFF_C = -0.8725;
    static constexpr double FRICTION_S_COEFF_D = 0.01853;
    static constexpr double FRICTION_Y_MIN = 1.0;
    static constexpr double FRICTION_Y_MAX = 1.2;
    static constexpr double FRICTION_S_ALT_COEFF = 2.2;
    static constexpr double FRICTION_S_ALT_OFFSET = -1.2;
    
    // Physical constants and conversions
    static constexpr double PRESSURE_CONVERSION = 144.0;     // psi to lbf/ft²
    static constexpr double REYNOLDS_SCALE = 1488.0;         // Reynolds number scale
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    static constexpr double VEL_NUMBER_SCALE = 1.938;
    static constexpr double DENSITY_SURFACE_TENSION_EXP = 0.25;
    
    // Numerical tolerances
    static constexpr double VELOCITY_TOLERANCE = 1.0e-15;
    static constexpr double FROUDE_TOLERANCE = 1.0e-15;
    static constexpr double MIN_HOLDUP = 0.00001;
    
    // Critical flow
    static constexpr double ACCEL_CRITICAL_LIMIT = 0.95;
    
    // INITIALIZATION
    flowType = FLOW_SEGREGATED;  // Default for two-phase
    criticalFlag = 0;
    
    const double sinAngle = sin(angle * DEG_TO_RAD);
    const double liquidSuperficialVel = velocity * liquidFraction;
    const double gasSuperficialVel = velocity * (1.0 - liquidFraction);
    
    // Pre-compute dimensionless numbers
    const double densitySurfaceTensionRoot = pow(liquidDensity / surfaceTension, DENSITY_SURFACE_TENSION_EXP);
    const double liquidVelocityNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * liquidSuperficialVel;
    const double froudeNumber = (velocity * velocity) / (g_in * diameter);
    
    // SINGLE-PHASE LIQUID FLOW
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowType = FLOW_LIQUID;
        holdup = liquidFraction;
        
        if (fabs(liquidSuperficialVel) > VELOCITY_TOLERANCE) {
            const double noSlipViscosity = liquidViscosity * liquidFraction + gasViscosity * (1.0 - liquidFraction);
            const double noSlipDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
            reynolds = REYNOLDS_SCALE * noSlipDensity * velocity * diameter / noSlipViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = velocity * velocity;
            frictionGrad = (frictionFactorValue * noSlipDensity * velocitySquared) /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = liquidDensity * sinAngle / PRESSURE_CONVERSION;
        accelGrad = 0.0;
        totalGrad = frictionGrad + gravityGrad;
        return;
    }
    
    // SINGLE-PHASE GAS FLOW
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowType = FLOW_GAS;
        holdup = liquidFraction;
        
        if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
            const double noSlipViscosity = liquidViscosity * liquidFraction + gasViscosity * (1.0 - liquidFraction);
            const double noSlipDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
            reynolds = REYNOLDS_SCALE * noSlipDensity * velocity * diameter / noSlipViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = velocity * velocity;
            frictionGrad = (frictionFactorValue * noSlipDensity * velocitySquared) /
                          (2.0 * g_in * diameter * PRESSURE_CONVERSION);
            
            double accelParam = (noSlipDensity * velocitySquared) / (g_in * pressure * PRESSURE_CONVERSION);
            
            if (accelParam > ACCEL_CRITICAL_LIMIT) {
                accelParam = ACCEL_CRITICAL_LIMIT;
                criticalFlag = 1;
            }
            
            gravityGrad = gasDensity * sinAngle / PRESSURE_CONVERSION;
            totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
            accelGrad = accelParam * totalGrad;
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
            gravityGrad = gasDensity * sinAngle / PRESSURE_CONVERSION;
            totalGrad = gravityGrad;
            accelGrad = 0.0;
        }
        return;
    }
    
    // TWO-PHASE FLOW - FLOW PATTERN DETERMINATION
    
    // Calculate transition boundaries
    const double transitionBoundary1 = TRANSITION_A_COEFF * pow(liquidFraction, TRANSITION_A_EXP);
    const double transitionBoundary2 = TRANSITION_B_COEFF * pow(liquidFraction, TRANSITION_B_EXP);
    const double transitionBoundary3 = TRANSITION_C_COEFF * pow(liquidFraction, TRANSITION_C_EXP);
    const double transitionBoundary4 = TRANSITION_D_COEFF * pow(liquidFraction, TRANSITION_D_EXP);
    
    // Determine flow pattern boundary
    double flowPatternBoundary = transitionBoundary1;
    if (liquidFraction >= LIQUID_FRACTION_HIGH) {
        flowPatternBoundary = transitionBoundary4;
    }
    
    // Determine initial flow pattern
    bool inTransitionRegion = false;
    
    if (liquidFraction >= LIQUID_FRACTION_LOW) {
        if (froudeNumber >= transitionBoundary2 && froudeNumber < transitionBoundary3) {
            inTransitionRegion = true;
            flowType = FLOW_SEGREGATED;  // Start with segregated for transition calculation
        } else if (froudeNumber >= transitionBoundary3 && froudeNumber < flowPatternBoundary) {
            flowType = FLOW_INTERMITTENT;
        } else if (froudeNumber >= flowPatternBoundary) {
            flowType = FLOW_DISTRIBUTED;
        } else {
            flowType = FLOW_SEGREGATED;
        }
    } else {
        if (froudeNumber >= transitionBoundary1) {
            flowType = FLOW_DISTRIBUTED;
        } else {
            flowType = FLOW_SEGREGATED;
        }
    }
    
    // HORIZONTAL HOLDUP CALCULATION
    
    double horizontalHoldup;
    double cCoeffD, cCoeffE, cCoeffF, cCoeffG;
    
    // Calculate horizontal holdup based on flow pattern
    if (fabs(froudeNumber) > FROUDE_TOLERANCE) {
        if (flowType == FLOW_DISTRIBUTED) {
            horizontalHoldup = DISTRIBUTED_COEFF *
                              pow(liquidFraction, DISTRIBUTED_LAMBDA_EXP) *
                              pow(froudeNumber, DISTRIBUTED_FROUDE_EXP);
            cCoeffD = C_COEFF_D_DISTRIBUTED;
            cCoeffE = C_COEFF_E_DISTRIBUTED;
            cCoeffF = C_COEFF_F_DISTRIBUTED;
            cCoeffG = C_COEFF_G_DISTRIBUTED;
            
        } else if (flowType == FLOW_INTERMITTENT) {
            horizontalHoldup = INTERMITTENT_COEFF *
                              pow(liquidFraction, INTERMITTENT_LAMBDA_EXP) *
                              pow(froudeNumber, INTERMITTENT_FROUDE_EXP);
            cCoeffD = C_COEFF_D_INTERMITTENT;
            cCoeffE = C_COEFF_E_INTERMITTENT;
            cCoeffF = C_COEFF_F_INTERMITTENT;
            cCoeffG = C_COEFF_G_INTERMITTENT;
            
        } else {  // FLOW_SEGREGATED
            horizontalHoldup = SEGREGATED_COEFF *
                              pow(liquidFraction, SEGREGATED_LAMBDA_EXP) *
                              pow(froudeNumber, SEGREGATED_FROUDE_EXP);
            cCoeffD = C_COEFF_D_SEGREGATED;
            cCoeffE = C_COEFF_E_SEGREGATED;
            cCoeffF = C_COEFF_F_SEGREGATED;
            cCoeffG = C_COEFF_G_SEGREGATED;
        }
    } else {
        // Zero Froude number: no-slip holdup
        horizontalHoldup = liquidFraction;
        cCoeffD = C_COEFF_D_DISTRIBUTED;
        cCoeffE = C_COEFF_E_DISTRIBUTED;
        cCoeffF = C_COEFF_F_DISTRIBUTED;
        cCoeffG = C_COEFF_G_DISTRIBUTED;
    }
    
    // Ensure horizontal holdup >= liquid fraction
    if (horizontalHoldup < liquidFraction) {
        horizontalHoldup = liquidFraction;
    }
    
    // INCLINED FLOW CORRECTION
    
    double segregatedHoldup = 0.0;
    double intermittentHoldup = 0.0;
    
    // For horizontal flow, use horizontal holdup directly
    if (fabs(angle) < VELOCITY_TOLERANCE) {
        holdup = horizontalHoldup;
    } else {
        // Inclined flow: apply C-factor correction
        
        // Override C-coefficients for downward flow
        if (angle < 0.0) {
            cCoeffD = C_COEFF_D_DOWNWARD;
            cCoeffE = C_COEFF_E_DOWNWARD;
            cCoeffF = C_COEFF_F_DOWNWARD;
            cCoeffG = C_COEFF_G_DOWNWARD;
        }
        
        // Calculate C-factor
        double cFactor = 0.0;
        if (fabs(froudeNumber) > FROUDE_TOLERANCE) {
            cFactor = (1.0 - liquidFraction) *
                     log(cCoeffD * pow(liquidFraction, cCoeffE) *
                         pow(liquidVelocityNumber, cCoeffF) *
                         pow(froudeNumber, cCoeffG));
            if (cFactor < 0.0) {
                cFactor = 0.0;
            }
        }
        
        // Calculate angle correction factor
        const double angleParameter = sin(ANGLE_SCALE_FACTOR * angle * DEG_TO_RAD);
        const double angleFactor = 1.0 + cFactor *
                                  (angleParameter - ANGLE_CUBIC_COEFF * angleParameter * angleParameter * angleParameter);
        
        // Apply angle correction to horizontal holdup
        if (angleFactor >= 0.0) {
            holdup = horizontalHoldup * angleFactor;
            
            // Clamp holdup to valid range
            if (holdup > 1.0) {
                holdup = 1.0;
            } else if (holdup <= 0.0) {
                holdup = MIN_HOLDUP;
            }
        } else {
            holdup = MIN_HOLDUP;
        }
        
        // Apply Palmer correction if requested
        if (palmerFlag != 0) {
            if (angle < 0.0) {
                holdup *= PALMER_DOWNWARD_FACTOR;
            } else if (angle > 0.0) {
                holdup *= PALMER_UPWARD_FACTOR;
            }
        }
    }
    
    // TRANSITION REGION BLENDING
    
    if (inTransitionRegion) {
        // Store segregated holdup
        segregatedHoldup = holdup;
        
        // Recalculate for intermittent flow
        if (fabs(froudeNumber) > FROUDE_TOLERANCE) {
            horizontalHoldup = INTERMITTENT_COEFF *
                              pow(liquidFraction, INTERMITTENT_LAMBDA_EXP) *
                              pow(froudeNumber, INTERMITTENT_FROUDE_EXP);
        } else {
            horizontalHoldup = liquidFraction;
        }
        
        if (horizontalHoldup < liquidFraction) {
            horizontalHoldup = liquidFraction;
        }
        
        // Apply inclined correction for intermittent
        if (fabs(angle) < VELOCITY_TOLERANCE) {
            intermittentHoldup = horizontalHoldup;
        } else {
            // Use intermittent C-coefficients
            cCoeffD = (angle < 0.0) ? C_COEFF_D_DOWNWARD : C_COEFF_D_INTERMITTENT;
            cCoeffE = (angle < 0.0) ? C_COEFF_E_DOWNWARD : C_COEFF_E_INTERMITTENT;
            cCoeffF = (angle < 0.0) ? C_COEFF_F_DOWNWARD : C_COEFF_F_INTERMITTENT;
            cCoeffG = (angle < 0.0) ? C_COEFF_G_DOWNWARD : C_COEFF_G_INTERMITTENT;
            
            double cFactor = 0.0;
            if (fabs(froudeNumber) > FROUDE_TOLERANCE) {
                cFactor = (1.0 - liquidFraction) *
                         log(cCoeffD * pow(liquidFraction, cCoeffE) *
                             pow(liquidVelocityNumber, cCoeffF) *
                             pow(froudeNumber, cCoeffG));
                if (cFactor < 0.0) {
                    cFactor = 0.0;
                }
            }
            
            const double angleParameter = sin(ANGLE_SCALE_FACTOR * angle * DEG_TO_RAD);
            const double angleFactor = 1.0 + cFactor *
                                      (angleParameter - ANGLE_CUBIC_COEFF * angleParameter * angleParameter * angleParameter);
            
            if (angleFactor >= 0.0) {
                intermittentHoldup = horizontalHoldup * angleFactor;
                if (intermittentHoldup > 1.0) {
                    intermittentHoldup = 1.0;
                } else if (intermittentHoldup <= 0.0) {
                    intermittentHoldup = MIN_HOLDUP;
                }
            } else {
                intermittentHoldup = MIN_HOLDUP;
            }
            
            if (palmerFlag != 0) {
                if (angle < 0.0) {
                    intermittentHoldup *= PALMER_DOWNWARD_FACTOR;
                } else if (angle > 0.0) {
                    intermittentHoldup *= PALMER_UPWARD_FACTOR;
                }
            }
        }
        
        // Blend segregated and intermittent holdups
        const double aWeight = (transitionBoundary3 - froudeNumber) /
                              (transitionBoundary3 - transitionBoundary2);
        const double bWeight = 1.0 - aWeight;
        
        holdup = segregatedHoldup * aWeight + intermittentHoldup * bWeight;
        flowType = FLOW_TRANSITION;
    }
    
    // MIXTURE PROPERTIES
    
    const double noSlipDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    const double slipDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
    const double noSlipViscosity = liquidViscosity * liquidFraction + gasViscosity * (1.0 - liquidFraction);
    
    // REYNOLDS NUMBER AND FRICTION FACTOR
    
    double frictionFactorValue = 0.0;
    
    if (fabs(velocity) > VELOCITY_TOLERANCE) {
        reynolds = REYNOLDS_SCALE * noSlipDensity * velocity * diameter / noSlipViscosity;
        frictionFactor(reynolds, roughness, frictionFactorValue);
        
        // Two-phase friction factor multiplier (S-parameter)
        const double yParam = liquidFraction / (holdup * holdup);
        const double xParam = log(yParam);
        const double xSquared = xParam * xParam;
        const double xQuartic = xSquared * xSquared;
        
        // Calculate S-parameter using polynomial correlation
        double sParam = xParam / (FRICTION_S_COEFF_A +
                                 FRICTION_S_COEFF_B * xParam +
                                 FRICTION_S_COEFF_C * xSquared +
                                 FRICTION_S_COEFF_D * xQuartic);
        
        // Override with special case correlation for narrow range
        if (yParam > FRICTION_Y_MIN && yParam < FRICTION_Y_MAX) {
            sParam = log(FRICTION_S_ALT_COEFF * yParam + FRICTION_S_ALT_OFFSET);
        }
        
        frictionFactorValue *= exp(sParam);
        
    } else {
        reynolds = 0.0;
    }
    
    // PRESSURE GRADIENTS
    
    frictionGrad = 0.0;
    if (fabs(velocity) > VELOCITY_TOLERANCE) {
        const double velocitySquared = velocity * velocity;
        frictionGrad = (frictionFactorValue * noSlipDensity * velocitySquared) /
                      (2.0 * g_in * diameter * PRESSURE_CONVERSION);
    }
    
    gravityGrad = slipDensity * sinAngle / PRESSURE_CONVERSION;
    
    double accelParam = 0.0;
    if (fabs(gasSuperficialVel) > VELOCITY_TOLERANCE) {
        accelParam = (slipDensity * velocity * gasSuperficialVel) /
                    (g_in * pressure * PRESSURE_CONVERSION);
        
        if (accelParam > ACCEL_CRITICAL_LIMIT) {
            accelParam = ACCEL_CRITICAL_LIMIT;
            criticalFlag = 1;
        }
    }
    
    totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
    accelGrad = accelParam * totalGrad;
}

/*
==============================================================================
holdupInterpolation: Auxiliary function used by the Dukler correlation.

This routine performs log-log interpolation between the no-slip liquid holdup
and the Dukler liquid holdup as a function of the Reynolds number.

Variable dictionary:
- columnIndex : Index of the column used in the data matrices (1 to 11)
- inputValue  : Value for which the liquid holdup is interpolated (between 0 and 1)
- outputValue : Interpolated liquid holdup (output)

Data matrices:
- inputMatrix[9][11] : Matrix of X values used for interpolation
                       (each column corresponds to a columnIndex value)
- outputMatrix[9][11]: Matrix of liquid holdup values for each X
                       (each column corresponds to a columnIndex value)

Auxiliary variables:
- interpolationFlag      : Indicates whether the interpolation has already
                           been performed and the search should stop
- arrayIndex             : Zero-based index used to access the matrices
                           (columnIndex - 1)
- rowIndex               : Row index used to iterate through the matrices
- lowerIndex, upperIndex : Indices of the two points used for interpolation
==============================================================================
*/
void holdupInterpolation(int columnIndex, double inputValue, double& outputValue) {
    // Replaced by analytical fits: uses CorrelationFits::duklerHoldup
    int colIdx = columnIndex - 1; // Convert to 0-based index
    outputValue = CorrelationFits::duklerHoldup(inputValue, colIdx);
}

/*
==============================================================================
holdupLiquidFraction: Calculates liquid holdup using the Eaton or Dukler
                      correlation with an iterative solution.

References:
     1.  Eaton, B. A. et al.: "The Prediction of Flow Patterns, Liquid
         Holdup and Pressure Losses Occurring During Continuous Two-Phase
         Flow in Horizontal Pipelines," Journal of Petroleum Technology
         (June 1967), 815-828.
     2.  Dukler, A. E., Wicks, M., and Cleveland, R. G.: "Frictional
         Pressure Drop in Two-Phase Flow: A. A Comparison of Existing
         Correlations for Pressure Loss and Holdup," AIChE Journal,
         Vol. 10, No. 1 (1964), 38-43.

Variable dictionary:
- liquidFraction   : No-slip liquid volumetric fraction (0-1)
- liquidViscosity  : Liquid viscosity (cP)
- liquidDensity    : Liquid density (lb/ft3)
- gasDensity       : Gas density (lb/ft3)
- diameter         : Pipe inner diameter (inches)
- velocity         : Total superficial velocity (ft/s)
- pressure         : Pressure (psi)
- liquidNumber     : Dimensionless liquid velocity number
- liquidViscNumber : Dimensionless liquid viscosity number
- gasVelNumber     : Dimensionless gas velocity number
- tubeNumber       : Dimensionless pipe diameter number
- holdup           : Calculated liquid holdup (output)
- reynoldsMixture  : Mixture Reynolds number (output)
- densityMixture   : Mixture density (output)
- correlationFlag  : Correlation selection flag (0 = Dukler, 1 = Eaton)
==============================================================================
*/
void holdupLiquidFraction(double liquidFraction, double liquidViscosity, double liquidDensity,
                         double gasDensity, double diameter, double velocity, double pressure,
                         double liquidNumber, double liquidViscNumber, double gasVelNumber, double tubeNumber,
                         double& holdup, double& reynoldsMixture, double& densityMixture,
                         unsigned char correlationFlag) {
    
    // Physical Constants
    static constexpr double ATM_PRESSURE_PSI = 14.69595;      // Standard atmospheric pressure
    static constexpr double REYNOLDS_SCALE = 1488.0;          // Reynolds number scaling factor
    
    // Eaton Correlation Constants
    static constexpr double EATON_BNL_COEFFICIENT = 0.00226;  // Baseline viscosity number for Eaton
    static constexpr double EATON_LIQUID_EXPONENT = 0.575;    // Liquid number exponent
    static constexpr double EATON_TUBE_EXPONENT = 0.0277;     // Tube number exponent
    static constexpr double EATON_PRESSURE_EXPONENT = 0.05;   // Pressure ratio exponent
    static constexpr double EATON_VISC_EXPONENT = 0.1;        // Viscosity ratio exponent
    static constexpr double EATON_LOW_MULTIPLIER = 3.0;       // Multiplier for low xe values
    
    // Iteration Control Constants
    static constexpr double HOLDUP_CONVERGENCE_TOLERANCE = 0.001;  // Convergence criterion
    static constexpr int MAX_HOLDUP_ITERATIONS = 40;               // Maximum iteration count
    static constexpr double ZERO_VELOCITY_TOLERANCE = 1e-15;       // Near-zero velocity cutoff
    
    // Correlation Selection Flags
    static constexpr unsigned char CORRELATION_DUKLER = 0;
    static constexpr unsigned char CORRELATION_EATON = 1;
    
    // Eaton Correlation Data (replaced by analytical fits in CorrelationFits.h)
    static constexpr double EATON_LOG_X_MIN = -6.90776;
    static constexpr double EATON_LOW_MULTIPLIER_THRESHOLD = -6.90776;
    
    // Dukler Correlation Data: Reynolds Number Ranges
    static const double REYNOLDS_VALUES[11] = {
        100, 500, 1000, 2500, 5000, 10000, 25000, 50000, 100000, 200000, 10000000
    };
    
    static constexpr int NUM_REYNOLDS_POINTS = 11;
    
    // Initialization
    double holdupGuess = liquidFraction;
    int iterationCount = 0;
    
    // EATON CORRELATION (correlationFlag == 1)
    if (correlationFlag == CORRELATION_EATON) {
        // Handle zero gas velocity case
        if (fabs(gasVelNumber) <= ZERO_VELOCITY_TOLERANCE) {
            holdup = liquidFraction;
            densityMixture = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
            reynoldsMixture = 0.0;
            return;
        }
        
        // Calculate Eaton parameter xe
        const double pressureRatio = pressure / ATM_PRESSURE_PSI;
        const double viscosityRatio = liquidViscNumber / EATON_BNL_COEFFICIENT;
        
        const double xe_value = pow(liquidNumber, EATON_LIQUID_EXPONENT) /
                               (gasVelNumber * pow(tubeNumber, EATON_TUBE_EXPONENT)) *
                               pow(pressureRatio, EATON_PRESSURE_EXPONENT) *
                               pow(viscosityRatio, EATON_VISC_EXPONENT);
        const double xe = log(xe_value);
        
        // Interpolate or extrapolate holdup from Eaton correlation
        if (xe <= EATON_LOG_X_MIN) {
            // Extrapolation for low xe: exponential decay
            holdup = EATON_LOW_MULTIPLIER * exp(xe);
        } else {
            // Analytical fit for Eaton holdup
            holdup = CorrelationFits::eatonHoldup(xe);
            
            // Enforce physical constraint: holdup >= liquid fraction
            if (holdup < liquidFraction) {
                holdup = liquidFraction;
            }
        }
        
        holdupGuess = holdup;
    }
    
    // Calculate Mixture Density
    // Weighted density accounting for slip between phases
    if (holdupGuess == 1.0) {
        // Special case: avoid division by zero
        densityMixture = liquidDensity * liquidFraction * liquidFraction;
    } else {
        densityMixture = liquidDensity * liquidFraction * liquidFraction / holdupGuess +
                        gasDensity * (1.0 - liquidFraction) * (1.0 - liquidFraction) / (1.0 - holdupGuess);
    }
    
    // Calculate mixture Reynolds number
    reynoldsMixture = REYNOLDS_SCALE * diameter * velocity * densityMixture / liquidViscosity;
    
    // DUKLER CORRELATION (correlationFlag == 0)
    // Iterative solution using 2D interpolation tables
    if (correlationFlag == CORRELATION_DUKLER) {
        do {
            double e1, e2;
            
            // Determine Reynolds number range and interpolate holdup
            if (reynoldsMixture <= REYNOLDS_VALUES[0]) {
                // Below minimum Reynolds: use first column
                holdupInterpolation(1, liquidFraction, holdup);
                
            } else if (reynoldsMixture >= REYNOLDS_VALUES[NUM_REYNOLDS_POINTS - 1]) {
                // Above maximum Reynolds: use last column
                holdupInterpolation(NUM_REYNOLDS_POINTS, liquidFraction, holdup);
                
            } else {
                // Within Reynolds range: interpolate between columns
                int i = 0;
                while ((reynoldsMixture > REYNOLDS_VALUES[i]) && (i < NUM_REYNOLDS_POINTS - 1)) {
                    i++;
                }
                int k = i + 1;
                
                if (reynoldsMixture == REYNOLDS_VALUES[k - 1]) {
                    // Exact match at Reynolds grid point
                    holdupInterpolation(k, liquidFraction, holdup);
                    
                } else if (reynoldsMixture < REYNOLDS_VALUES[k - 1]) {
                    // Interpolate between two Reynolds columns
                    holdupInterpolation(k, liquidFraction, e2);
                    
                    int j = k - 1;
                    holdupInterpolation(j, liquidFraction, e1);
                    
                    // Logarithmic interpolation in Reynolds number
                    holdup = e1 + (log(reynoldsMixture) - log(REYNOLDS_VALUES[j - 1])) /
                            (log(REYNOLDS_VALUES[k - 1]) - log(REYNOLDS_VALUES[j - 1])) * (e2 - e1);
                }
            }
            
            // Convergence Check
            if (std::abs(holdup - holdupGuess) <= HOLDUP_CONVERGENCE_TOLERANCE) {
                // Converged: enforce physical constraint and return
                if (holdup > liquidFraction) {
                    return;
                } else {
                    holdup = liquidFraction;
                    densityMixture = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
                    reynoldsMixture = REYNOLDS_SCALE * diameter * velocity * densityMixture / liquidViscosity;
                    return;
                }
            } else {
                // Not converged: update guess and continue iteration
                iterationCount++;
                
                if (iterationCount < MAX_HOLDUP_ITERATIONS) {
                    holdupGuess = holdup;
                    
                    // Recalculate mixture density with updated holdup
                    if (holdupGuess == 1.0) {
                        densityMixture = liquidDensity * liquidFraction * liquidFraction;
                    } else {
                        densityMixture = liquidDensity * liquidFraction * liquidFraction / holdupGuess +
                                        gasDensity * (1.0 - liquidFraction) * (1.0 - liquidFraction) / (1.0 - holdupGuess);
                    }
                    
                    reynoldsMixture = REYNOLDS_SCALE * diameter * velocity * densityMixture / liquidViscosity;
                } else {
                    // Maximum iterations reached: force convergence
                    holdup = 1.0;
                    return;
                }
            }
        } while (true);
    }
}

/*
==============================================================================
holdupMinami: Calculates liquid holdup using the Minami I/II correlations
              with the Beggs-Brill inclination correction.

References:
     1.  Minami, K. et al.: "Correlations for Calculating Liquid
         Holdup in Horizontal and Inclined Two-Phase Flow,"
         PETROBRAS Technical Report (1985).
     2.  Beggs, H. D. and Brill, J. P.: "A Study of Two-Phase Flow
         in Inclined Pipes," Journal of Petroleum Technology
         (May 1973), 607-617.
     3.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984), Sections 4-11 through 4-17.

Variable dictionary:
- angle            : Pipe inclination angle (degrees)
- diameter         : Pipe inner diameter (inches)
- pressure         : Pressure (psi)
- velocity         : Mixture velocity (ft/s)
- liquidFraction   : No-slip liquid volumetric fraction (0-1)
- gasVelNumber     : Dimensionless gas velocity number
- liquidViscNumber : Dimensionless liquid viscosity number
- tubeNumber       : Dimensionless pipe diameter number
- liquidNumber     : Dimensionless liquid velocity number
- correlationFlag  : Correlation flag (2 = Minami I, 3 = Minami II)
- transitionFlag   : Flag indicating conditions outside the correlation's
                     range of applicability (output)
- flowPattern      : Flow pattern (output):
                     1 = liquid, 2 = gas, 3 = distributed,
                     4 = intermittent, 5 = segregated, 6 = transition
- holdup           : Calculated liquid holdup (output)
==============================================================================
*/
void holdupMinami(double angle, double diameter, double pressure, double velocity,
                 double liquidFraction, double gasVelNumber, double liquidViscNumber,
                 double tubeNumber, double liquidNumber, unsigned char correlationFlag,
                 unsigned char& transitionFlag, unsigned char& flowPattern, double& holdup) {
    
    // Physical Constants
    static constexpr double ATM_PRESSURE_PSI = 14.69595;     // Standard atmospheric pressure
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    
    // Flow Regime Thresholds
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.99999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.00001;
    static constexpr double ZERO_VELOCITY_TOLERANCE = 1e-15;
    
    // Flow Pattern Identifiers
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_DISTRIBUTED = 3;
    static constexpr unsigned char FLOW_INTERMITTENT = 4;
    static constexpr unsigned char FLOW_SEGREGATED = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    // Correlation Selection Flags
    static constexpr unsigned char MINAMI_I = 2;
    static constexpr unsigned char MINAMI_II = 3;
    
    // Minami Correlation I Constants (wet gas flow)
    static constexpr double MINAMI_I_LIQUID_EXP = 0.8945;
    static constexpr double MINAMI_I_TUBE_EXP = 0.0796;
    static constexpr double MINAMI_I_VEL_EXP = -0.4076;
    static constexpr double MINAMI_I_Y_MIN = 0.0026;        // Minimum y parameter
    static constexpr double MINAMI_I_Y_MAX = 0.15;          // Maximum y parameter (applicability limit)
    
    // Minami I polynomial coefficients: H = a*y + b*y² + c*y⁴ + d
    static constexpr double MINAMI_I_COEFF_A = 3.698;
    static constexpr double MINAMI_I_COEFF_B = -11.497;
    static constexpr double MINAMI_I_COEFF_C = 65.22;
    static constexpr double MINAMI_I_COEFF_D = -0.00952;
    
    // Minami Correlation II Constants (general flow - Eaton-based)
    static constexpr double MINAMI_II_SCALE = 1.84;
    static constexpr double MINAMI_II_LIQUID_EXP = 0.575;
    static constexpr double MINAMI_II_PRESSURE_EXP = 0.05;
    static constexpr double MINAMI_II_VISC_EXP = 0.1;
    static constexpr double MINAMI_II_TUBE_EXP = 0.0277;
    static constexpr double MINAMI_II_EATON_MIN = 0.0001;   // Minimum Eaton number
    
    // Minami II exponential form coefficients
    static constexpr double MINAMI_II_LOG_OFFSET = 9.21;
    static constexpr double MINAMI_II_LOG_SCALE = 8.7115;
    static constexpr double MINAMI_II_POWER = 4.3374;
    
    // Beggs & Brill Flow Pattern Map Constants
    // Transition boundaries: XL = coeff * λ^exp
    static constexpr double BB_XL1_COEFF = 316.0;
    static constexpr double BB_XL1_EXP = 0.302;
    static constexpr double BB_XL2_COEFF = 0.0009252;
    static constexpr double BB_XL2_EXP = -2.46842;
    static constexpr double BB_XL3_COEFF = 0.1;
    static constexpr double BB_XL3_EXP = -1.45155;
    static constexpr double BB_XL4_COEFF = 0.5;
    static constexpr double BB_XL4_EXP = -6.738;
    
    static constexpr double BB_LIQUID_FRAC_THRESHOLD_LOW = 0.01;
    static constexpr double BB_LIQUID_FRAC_THRESHOLD_HIGH = 0.4;
    
    // Beggs & Brill Inclination Correction Constants
    // Coefficients for C = (1-λ) * ln(d * λ^e * NL^f * NFr^g)
    
    // Distributed flow (upward)
    static constexpr double BB_DIST_D = 1.0;
    static constexpr double BB_DIST_E = 0.0;
    static constexpr double BB_DIST_F = 0.0;
    static constexpr double BB_DIST_G = 0.0;
    
    // Intermittent flow (upward)
    static constexpr double BB_INTER_D = 2.96;
    static constexpr double BB_INTER_E = 0.305;
    static constexpr double BB_INTER_F = -0.4473;
    static constexpr double BB_INTER_G = 0.0978;
    
    // Segregated flow (upward)
    static constexpr double BB_SEG_D = 0.011;
    static constexpr double BB_SEG_E = -3.768;
    static constexpr double BB_SEG_F = 3.539;
    static constexpr double BB_SEG_G = -1.614;
    
    // Downward flow (all patterns)
    static constexpr double BB_DOWN_D = 4.7;
    static constexpr double BB_DOWN_E = -0.3692;
    static constexpr double BB_DOWN_F = 0.1244;
    static constexpr double BB_DOWN_G = -0.5056;
    
    // Inclination factor: ψ = 1 + C * (sin(1.8θ) - 0.333*sin³(1.8θ))
    static constexpr double BB_ANGLE_MULTIPLIER = 1.8;
    static constexpr double BB_SINE_CUBIC_COEFF = 0.333;
    
    // Physical bounds
    static constexpr double MIN_HOLDUP = 0.00001;
    static constexpr double MAX_HOLDUP = 1.0;
    
    // Pre-computed Angle and Froude Number
    const double angleRad = angle * DEG_TO_RAD;
    const double froudeNumber = velocity * velocity / (g_in * diameter);
    
    // Single-Phase Flow Check
    flowPattern = FLOW_SEGREGATED;  // Default pattern
    
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowPattern = FLOW_LIQUID;
        holdup = liquidFraction;
        return;
    }
    
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowPattern = FLOW_GAS;
        holdup = liquidFraction;
        return;
    }
    
    // Two-Phase Flow: Check for Static Conditions
    if (fabs(velocity) <= ZERO_VELOCITY_TOLERANCE) {
        holdup = liquidFraction;
        return;
    }
    
    // Step 1: Calculate Horizontal Holdup Using Selected Minami Correlation
    double horizontalHoldup;
    bool correlationLoop = true;
    
    do {
        correlationLoop = true;
        
        if (correlationFlag == MINAMI_I) {
            // MINAMI CORRELATION I (Wet Gas Flow)
            const double yParam = pow(liquidFraction, MINAMI_I_LIQUID_EXP) *
                                 pow(tubeNumber, MINAMI_I_TUBE_EXP) *
                                 pow(liquidNumber, MINAMI_I_VEL_EXP);
            
            if (yParam < MINAMI_I_Y_MIN) {
                // Below applicability range: no slip
                holdup = liquidFraction;
                return;
            }
            
            if (yParam > MINAMI_I_Y_MAX) {
                // Above applicability range: switch to Minami II
                transitionFlag = 1;
                correlationLoop = false;
                correlationFlag = MINAMI_II;
            } else {
                // Within range: polynomial correlation
                const double ySquared = yParam * yParam;
                const double yFourth = ySquared * ySquared;
                horizontalHoldup = MINAMI_I_COEFF_A * yParam +
                                  MINAMI_I_COEFF_B * ySquared +
                                  MINAMI_I_COEFF_C * yFourth +
                                  MINAMI_I_COEFF_D;
            }
            
        } else {
            // MINAMI CORRELATION II (General Flow - Eaton-based)
            const double pressureRatio = pressure / ATM_PRESSURE_PSI;
            const double eatonNumber = MINAMI_II_SCALE *
                                      pow(liquidNumber, MINAMI_II_LIQUID_EXP) *
                                      pow(pressureRatio, MINAMI_II_PRESSURE_EXP) *
                                      pow(liquidViscNumber, MINAMI_II_VISC_EXP) /
                                      (gasVelNumber * pow(tubeNumber, MINAMI_II_TUBE_EXP));
            
            if (eatonNumber >= MINAMI_II_EATON_MIN) {
                // Exponential correlation
                const double exponentArg = (log10(eatonNumber) + MINAMI_II_LOG_OFFSET) / MINAMI_II_LOG_SCALE;
                horizontalHoldup = 1.0 - exp(-pow(exponentArg, MINAMI_II_POWER));
            } else {
                // Below minimum: no slip
                holdup = liquidFraction;
                return;
            }
        }
        
    } while (!correlationLoop);
    
    // Step 2: Apply Inclination Correction
    if (angle == 0.0) {
        // Horizontal flow: no correction needed
        holdup = horizontalHoldup;
        return;
    }
    
    // Enforce physical constraint
    if (horizontalHoldup < liquidFraction) {
        horizontalHoldup = liquidFraction;
    }
    
    // Step 3: Determine Flow Pattern Using Beggs & Brill Map
    unsigned char transitionType = 0;
    
    const double xl1 = BB_XL1_COEFF * pow(liquidFraction, BB_XL1_EXP);
    const double xl2 = BB_XL2_COEFF * pow(liquidFraction, BB_XL2_EXP);
    const double xl3 = BB_XL3_COEFF * pow(liquidFraction, BB_XL3_EXP);
    const double xl4 = BB_XL4_COEFF * pow(liquidFraction, BB_XL4_EXP);
    
    double froudeTransition = xl1;
    
    if (liquidFraction >= BB_LIQUID_FRAC_THRESHOLD_LOW) {
        if (liquidFraction > BB_LIQUID_FRAC_THRESHOLD_HIGH) {
            froudeTransition = xl4;
        }
        
        // Determine flow pattern
        if ((froudeNumber >= xl2) && (froudeNumber < xl3)) {
            transitionType = 1;  // Transition region
        }
        if ((froudeNumber >= xl3) && (froudeNumber < froudeTransition)) {
            flowPattern = FLOW_INTERMITTENT;
        }
        if (froudeNumber >= froudeTransition) {
            flowPattern = FLOW_DISTRIBUTED;
        }
    } else {
        // Low liquid fraction
        if (froudeNumber >= xl1) {
            flowPattern = FLOW_DISTRIBUTED;
        }
    }
    
    // Step 4: Apply Beggs & Brill Inclination Correction
    double holdupSegregated = 0.0;
    double holdupIntermittent = 0.0;
    bool transitionLoop = true;
    
    do {
        transitionLoop = true;
        
        // Select coefficients based on flow pattern and flow direction
        double coeff_d, coeff_e, coeff_f, coeff_g;
        
        if (angleRad <= 0) {
            // Downward flow: use single set of coefficients
            coeff_d = BB_DOWN_D;
            coeff_e = BB_DOWN_E;
            coeff_f = BB_DOWN_F;
            coeff_g = BB_DOWN_G;
        } else {
            // Upward flow: coefficients depend on pattern
            switch (flowPattern) {
                case FLOW_DISTRIBUTED:
                    coeff_d = BB_DIST_D;
                    coeff_e = BB_DIST_E;
                    coeff_f = BB_DIST_F;
                    coeff_g = BB_DIST_G;
                    break;
                case FLOW_INTERMITTENT:
                    coeff_d = BB_INTER_D;
                    coeff_e = BB_INTER_E;
                    coeff_f = BB_INTER_F;
                    coeff_g = BB_INTER_G;
                    break;
                case FLOW_SEGREGATED:
                default:
                    coeff_d = BB_SEG_D;
                    coeff_e = BB_SEG_E;
                    coeff_f = BB_SEG_F;
                    coeff_g = BB_SEG_G;
                    break;
            }
        }
        
        // Calculate C factor
        const double logArg = coeff_d * pow(liquidFraction, coeff_e) *
                             pow(liquidNumber, coeff_f) *
                             pow(froudeNumber, coeff_g);
        double cFactor = (1.0 - liquidFraction) * log(logArg);
        
        if (cFactor < 0.0) {
            cFactor = 0.0;
        }
        
        // Calculate inclination correction factor: ψ = 1 + C*(sin(1.8θ) - 0.333*sin³(1.8θ))
        const double angleScaled = BB_ANGLE_MULTIPLIER * angleRad;
        const double sineTerm = sin(angleScaled);
        const double sineCubed = sineTerm * sineTerm * sineTerm;
        double correctionFactor = 1.0 + cFactor * (sineTerm - BB_SINE_CUBIC_COEFF * sineCubed);
        
        if (correctionFactor < 0.0) {
            correctionFactor = 0.0;
        }
        
        // Apply correction
        holdup = horizontalHoldup * correctionFactor;
        
        // Enforce physical bounds
        if (holdup > MAX_HOLDUP) {
            holdup = MAX_HOLDUP;
        }
        if (holdup <= 0.0) {
            holdup = MIN_HOLDUP;
        }
        
        // Handle Transition Region (Segregated ↔ Intermittent)
        if (transitionType == 1) {
            if (flowPattern == FLOW_SEGREGATED) {
                // First pass: calculate segregated holdup
                holdupSegregated = holdup;
                flowPattern = FLOW_INTERMITTENT;
                transitionLoop = false;
            } else {
                // Second pass: blend segregated and intermittent holdups
                holdupIntermittent = holdup;
                const double weightSegregated = (xl3 - froudeNumber) / (xl3 - xl2);
                const double weightIntermittent = 1.0 - weightSegregated;
                holdup = holdupSegregated * weightSegregated + holdupIntermittent * weightIntermittent;
                flowPattern = FLOW_TRANSITION;
            }
        }
        
    } while (!transitionLoop);
}

/*
==============================================================================
duklerCorrelation: Calculates liquid holdup and pressure gradients using the
                   Dukler correlation with the Flanigan elevation-gradient
                   correction.

References:
     1.  Brill, J. P. and Beggs, H. D.: Two-Phase Flow in Pipes
         (Feb. 1984), Sections 4-11 through 4-17.
     2.  Dukler, A. E. et al.: "Gas-Liquid Flow in Pipelines, I.
         Research Results," AGA-API Project NX-28 (May 1969).
     3.  Duns, H., Jr. and Ros, N. C. J.: "Vertical Flow of Gas and
         Liquid Mixtures in Wells," Proceedings, Sixth World
         Petroleum Congress (1963), 451.
     4.  Eaton, B. A.: "The Prediction of Flow Patterns, Liquid Holdup
         and Pressure Losses Occurring During Continuous Two-Phase
         Flow in Horizontal Pipelines," Ph.D. Dissertation, The
         University of Texas, Austin (1965).
     5.  Eaton, B. A. et al.: "The Prediction of Flow Patterns, Liquid
         Holdup and Pressure Losses Occurring During Continuous
         Two-Phase Flow in Horizontal Pipelines," Transactions of
         AIME (1967), 815.
     6.  Flanigan, O.: "Effect of Uphill Flow on Pressure Drop in
         Design of Two-Phase Gathering Systems," Oil and Gas Journal
         (March 10, 1958), 56.

Variable dictionary:
- angle             : Pipe inclination angle (degrees)
- diameter          : Pipe inner diameter (inches)
- roughness         : Relative roughness (dimensionless)
- pressure          : Pressure (psi)
- velocity          : Mixture velocity (ft/s)
- liquidFraction    : No-slip liquid volumetric fraction (0-1)
- gasDensity        : Gas density (lb/ft3)
- liquidDensity     : Liquid density (lb/ft3)
- gasViscosity      : Gas viscosity (cP)
- liquidViscosity   : Liquid viscosity (cP)
- liquidNumber      : Dimensionless liquid viscosity number
- liquidViscNumber  : Dimensionless liquid velocity number
- gasVelNumber      : Dimensionless gas velocity number
- tubeNumber        : Dimensionless pipe diameter number
- holdup            : Calculated liquid holdup (output)
- frictionGrad      : Frictional pressure gradient (psi/ft)
- gravityGrad       : Gravitational pressure gradient (psi/ft)
- accelGrad         : Accelerational pressure gradient (psi/ft)
- totalGrad         : Total pressure gradient (psi/ft)
- reynolds          : Reynolds number (output)
- flowPattern       : Flow pattern (1 = liquid, 2 = gas, 3 = two-phase)
- correlationFlag   : Holdup correlation flag
                      (0 = Dukler, 1 = Eaton, 2 = Minami I, 3 = Minami II)
- transitionFlag    : Flag indicating conditions outside the correlation's
                      range of applicability
- criticalFlag      : Critical-flow flag (1 = critical)

Internal arrays:
- phiFlanigan[16]   : Density correction factors for the Flanigan elevation
                      gradient
- vsgFlanigan[16]   : Superficial gas velocities used by the Flanigan
                      correlation
- lambdaValues[15]  : Liquid-fraction values used to calculate the friction
                      factor
- logLambda[15]     : Logarithms of the liquid-fraction values
- frictionRatio[15] : Two-phase-to-single-phase friction-factor ratios
==============================================================================
*/

/*
==============================================================================
duklerCorrelation: Calculates pressure gradients using the Dukler, Eaton, and
                   Flanigan correlations, with selectable subcorrelations for
                   liquid holdup.

References:
     1.  Dukler, A. E., Wicks, M., and Cleveland, R. G.: "Frictional
         Pressure Drop in Two-Phase Flow: A. A Comparison of Existing
         Correlations for Pressure Loss and Holdup," AIChE Journal,
         Vol. 10, No. 1 (1964), 38-43.
     2.  Eaton, B. A. et al.: "The Prediction of Flow Patterns, Liquid
         Holdup and Pressure Losses Occurring During Continuous Two-Phase
         Flow in Horizontal Pipelines," Journal of Petroleum Technology
         (June 1967), 815-828.
     3.  Flanigan, O.: "Effect of Uphill Flow on Pressure Drop in Design
         of Two-Phase Gathering Systems," Oil and Gas Journal
         (March 10, 1958), 132-141.

Variable dictionary:
- angle                 : Pipe inclination angle (degrees)
- diameter              : Pipe inner diameter (inches)
- roughness             : Relative roughness (dimensionless)
- pressure              : Absolute pressure (psia)
- velocity              : Mixture superficial velocity (ft/s)
- liquidFraction        : No-slip liquid volumetric fraction (0-1)
- gasDensity            : Gas density (lb/ft3)
- liquidDensity         : Liquid density (lb/ft3)
- gasViscosity          : Gas viscosity (cP)
- liquidViscosity       : Liquid viscosity (cP)
- surfaceTension        : Surface tension (dyn/cm)
- temperature           : Temperature (degrees F)
- compressibilityFactor : Gas compressibility factor (dimensionless)
- holdup                : Calculated liquid holdup (output)
- frictionGrad          : Frictional pressure gradient (psi/ft) - output
- gravityGrad           : Gravitational pressure gradient (psi/ft) - output
- accelGrad             : Accelerational pressure gradient (psi/ft) - output
- totalGrad             : Total pressure gradient (psi/ft) - output
- reynolds              : Reynolds number (output)
- flowPattern           : Flow-pattern indicator (output):
                          1 = liquid, 2 = gas, 3 = two-phase
- correlationFlag       : Holdup correlation selection flag:
                          0 = Dukler, 1 = Eaton,
                          2 = Minami I, 3 = Minami II
- transitionFlag        : Flow-pattern transition indicator (output)
- criticalFlag          : Critical-flow indicator (output)
==============================================================================
*/
void duklerCorrelation(double angle, double diameter, double roughness, double pressure,
                      double velocity, double liquidFraction, double gasDensity,
                      double liquidDensity, double gasViscosity, double liquidViscosity,
                      double surfaceTension, double temperature, double compressibilityFactor,
                      double& holdup, double& frictionGrad, double& gravityGrad,
                      double& accelGrad, double& totalGrad, double& reynolds, unsigned char& flowPattern,
                      unsigned char correlationFlag, unsigned char& transitionFlag,
                      unsigned char& criticalFlag) {
    
    // Correlation Data (replaced by analytical fits in CorrelationFits.h)
    
    // Physical Constants
    static constexpr double REYNOLDS_SCALE = 1488.0;          // Reynolds number scaling factor
    static constexpr double PSI_CONVERSION = 144.0;           // Pressure conversion (in²/ft²)
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    
    // Dimensionless number coefficients
    static constexpr double VEL_NUMBER_SCALE = 1.938;         // Velocity number coefficient
    static constexpr double DIAM_NUMBER_SCALE = 120.872;      // Diameter number coefficient
    static constexpr double VISC_NUMBER_SCALE = 0.15726;      // Viscosity number coefficient
    
    // Flow regime thresholds
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.99999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.00001;
    static constexpr double MIN_VELOCITY_THRESHOLD = 1e-15;
    
    // Critical flow parameters
    static constexpr double MAX_ACCEL_PARAMETER = 0.95;       // Critical acceleration limit
    
    // Flow type identifiers
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_TWOPHASE = 3;
    
    // Holdup correlation flags
    static constexpr unsigned char HOLDUP_DUKLER = 0;
    static constexpr unsigned char HOLDUP_EATON = 1;
    static constexpr unsigned char HOLDUP_MINAMI_I = 2;
    static constexpr unsigned char HOLDUP_MINAMI_II = 3;
    
    // Blasius friction factor constants
    static constexpr double BLASIUS_INTERCEPT = 0.0014;
    static constexpr double BLASIUS_COEFFICIENT = 0.125;
    static constexpr double BLASIUS_EXPONENT = -0.32;
    static constexpr double BLASIUS_MULTIPLIER = 4.0;
    
    // Initialization
    criticalFlag = 0;
    flowPattern = FLOW_TWOPHASE;  // Default: two-phase flow
    
    // Pre-computed Geometric Properties
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    const double area = 0.25 * M_PI * diameter * diameter;
    
    // Superficial Velocities
    const double liquidSupVel = velocity * liquidFraction;
    const double gasSupVel = velocity - liquidSupVel;
    const double gasVolumeFlow = gasSupVel * area;
    
    // Dimensionless Numbers
    const double densitySurfaceTensionRoot = pow(liquidDensity / surfaceTension, 0.25);
    const double liquidNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * liquidSupVel;
    const double gasVelNumber = VEL_NUMBER_SCALE * densitySurfaceTensionRoot * gasSupVel;
    const double tubeNumber = DIAM_NUMBER_SCALE * diameter * sqrt(liquidDensity / surfaceTension);
    
    const double surfaceTensionCubed = surfaceTension * surfaceTension * surfaceTension;
    const double viscNumberDenom = pow(1.0 / (liquidDensity * surfaceTensionCubed), 0.25);
    const double liquidViscNumber = VISC_NUMBER_SCALE * liquidViscosity * viscNumberDenom;
    
    // CASE 1: Single-Phase Liquid Flow
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowPattern = FLOW_LIQUID;
        holdup = liquidFraction;
        
        double frictionFactorValue;
        if (fabs(liquidSupVel) > MIN_VELOCITY_THRESHOLD) {
            reynolds = REYNOLDS_SCALE * liquidDensity * liquidSupVel * diameter / liquidViscosity;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = liquidSupVel * liquidSupVel;
            frictionGrad = frictionFactorValue * liquidDensity * velocitySquared /
                          (2.0 * g_in * diameter * PSI_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionFactorValue = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = liquidDensity * sinAngle / PSI_CONVERSION;
        
        const double accelerationParam = 0.0;
        totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelerationParam);
        accelGrad = accelerationParam * totalGrad;
        
        return;
    }
    
    // CASE 2: Single-Phase Gas Flow
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowPattern = FLOW_GAS;
        holdup = liquidFraction;
        
        double frictionFactorValue;
        double accelerationParam;
        
        if (fabs(gasSupVel) > MIN_VELOCITY_THRESHOLD) {
            reynolds = REYNOLDS_SCALE * gasDensity * gasSupVel * diameter / gasViscosity;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = gasSupVel * gasSupVel;
            frictionGrad = frictionFactorValue * gasDensity * velocitySquared /
                          (2.0 * g_in * diameter * PSI_CONVERSION);
            
            accelerationParam = gasDensity * velocitySquared /
                               (g_in * pressure * PSI_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionFactorValue = 0.0;
            frictionGrad = 0.0;
            accelerationParam = 0.0;
        }
        
        gravityGrad = gasDensity * sinAngle / PSI_CONVERSION;
        
        // Check for critical flow
        if (accelerationParam > MAX_ACCEL_PARAMETER) {
            accelerationParam = MAX_ACCEL_PARAMETER;
            criticalFlag = 1;
        }
        
        totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelerationParam);
        accelGrad = accelerationParam * totalGrad;
        
        return;
    }
    
    // CASE 3: Two-Phase Flow
    flowPattern = FLOW_TWOPHASE;
    
    // Step 3.1: Calculate Holdup Using Selected Correlation
    const double mixtureViscosity = liquidViscosity * liquidFraction + gasViscosity * (1.0 - liquidFraction);
    double reynoldsMixture, densityMixture;
    
    if ((correlationFlag == HOLDUP_DUKLER) || (correlationFlag == HOLDUP_EATON)) {
        // Use Dukler or Eaton holdup correlation
        holdupLiquidFraction(liquidFraction, mixtureViscosity, liquidDensity, gasDensity,
                           diameter, velocity, pressure, liquidNumber, liquidViscNumber,
                           gasVelNumber, tubeNumber, holdup, reynoldsMixture, densityMixture,
                           correlationFlag);
    } else {
        // Use Minami I or Minami II holdup correlation
        holdupMinami(angle, diameter, pressure, velocity, liquidFraction,
                    gasVelNumber, liquidViscNumber, tubeNumber, liquidNumber,
                    correlationFlag, transitionFlag, flowPattern, holdup);
        
        densityMixture = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
        
        if (fabs(velocity) > MIN_VELOCITY_THRESHOLD) {
            reynoldsMixture = REYNOLDS_SCALE * densityMixture * velocity * diameter / mixtureViscosity;
        } else {
            reynoldsMixture = 0.0;
        }
    }
    
    // Step 3.2: Calculate Friction Gradient
    double frictionFactorValue;
    
    if (fabs(velocity) > MIN_VELOCITY_THRESHOLD) {
        // Analytical fit for two-phase friction factor ratio from Dukler correlation
        const double logLiquidFraction = log(liquidFraction);
        const double frictionFactorRatio = CorrelationFits::duklerFrictionRatio(logLiquidFraction);
        
        // Blasius friction factor for smooth pipe
        const double singlePhaseFriction = BLASIUS_MULTIPLIER *
                                          (BLASIUS_INTERCEPT + BLASIUS_COEFFICIENT * pow(reynoldsMixture, BLASIUS_EXPONENT));
        
        frictionFactorValue = frictionFactorRatio * singlePhaseFriction;
        
        const double velocitySquared = velocity * velocity;
        frictionGrad = frictionFactorValue * densityMixture * velocitySquared /
                      (2.0 * g_in * diameter * PSI_CONVERSION);
    } else {
        frictionFactorValue = 0.0;
        frictionGrad = 0.0;
    }
    
    // Step 3.3: Calculate Gravity Gradient (Flanigan Correlation)
    const double phiFactor = CorrelationFits::flaniganPhi(gasSupVel);
    
    gravityGrad = liquidDensity * sinAngle * phiFactor / PSI_CONVERSION;
    
    // Special case: downward flow uses gas density
    if (angleRad < 0.0) {
        gravityGrad = gasDensity * sinAngle / PSI_CONVERSION;
    }
    
    // Step 3.4: Calculate Acceleration Gradient
    double accelerationParam = densityMixture * velocity * gasSupVel /
                              (g_in * pressure * PSI_CONVERSION);
    
    // Check for critical flow
    if (accelerationParam > MAX_ACCEL_PARAMETER) {
        accelerationParam = MAX_ACCEL_PARAMETER;
        criticalFlag = 1;
    }
    
    // Step 3.5: Total Gradient
    totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelerationParam);
    accelGrad = 0.0;  // Note: Dukler correlation sets this to zero for two-phase
}

/*
==============================================================================
mukherjeeeBrillFlowPattern: Determines the two-phase flow pattern using the
                           Mukherjee-Brill correlation.

Variable dictionary:
- liquidViscNumber : Dimensionless liquid velocity number
- gasVelNumber     : Dimensionless gas velocity number
- liquidNumber     : Dimensionless liquid viscosity number
- angle            : Pipe inclination angle (degrees)
- flowPattern      : Determined flow pattern
                     3 = bubble, 4 = slug, 5 = mist, 6 = stratified

Auxiliary variables:
- angleRad         : Angle in radians
- sinAngle         : Sine of the pipe inclination angle
- absAngle         : Absolute value of the angle
- gasVelMist       : Boundary for mist flow
- liquidVelBubble  : Boundary for bubble flow
- liquidVelStrat   : Boundary for stratified flow
- gasVelBubble     : Boundary between bubble and stratified flow

Flow patterns:
- 3: Bubble flow
- 4: Slug flow
- 5: Mist flow
- 6: Stratified flow
==============================================================================
*/
void mukherjeeeBrillFlowPattern(double liquidViscNumber, double gasVelNumber, double liquidNumber,
                               double angle, unsigned char& flowPattern) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_BUBBLE     = 3;
    static constexpr unsigned char FLOW_SLUG       = 4;
    static constexpr unsigned char FLOW_MIST       = 5;
    static constexpr unsigned char FLOW_STRATIFIED = 6;
    
    // Flow Pattern Boundary Thresholds
    static constexpr double DOWNWARD_ANGLE_THRESHOLD = 30.0;  // degrees
    
    // Pre-computed Angle Properties
    const double angleRad = angle * M_PI / 180.0;
    const double sinAngle = sin(angleRad);
    const double sinAngleSquared = sinAngle * sinAngle;
    const double absoluteAngle = fabs(angle);
    
    // Pre-computed Logarithms
    const double log10GasVelNumber = log10(gasVelNumber);
    const double log10LiquidViscNumber = log10(liquidViscNumber);
    const double log10GasVelNumberSquared = log10GasVelNumber * log10GasVelNumber;
    const double log10LiquidViscNumberSquared = log10LiquidViscNumber * log10LiquidViscNumber;
    
    // Flow Pattern Boundary Calculations
    
    // Mist flow boundary: gasVelNumber vs liquidNumber
    const double mistBoundaryExponent = 1.400575
                                       - 2.694 * liquidNumber
                                       + 0.521084 * pow(liquidViscNumber, 0.329066);
    const double mistFlowBoundary = pow(10.0, mistBoundaryExponent);
    
    // Bubble flow boundary: liquidViscNumber vs other parameters (upward flow)
    const double bubbleBoundaryExponent = 0.94
                                         + 0.074 * sinAngle
                                         - 0.855 * sinAngleSquared
                                         + 3.695 * liquidNumber
                                         + log10GasVelNumber;
    const double bubbleFlowBoundary = pow(10.0, bubbleBoundaryExponent);
    
    // Stratified flow boundary: liquidViscNumber vs other parameters
    const double stratifiedBoundaryExponent = 0.321
                                             - 0.017 * gasVelNumber
                                             - 4.267 * sinAngle
                                             - 2.972 * liquidNumber
                                             - 0.033 * log10GasVelNumberSquared
                                             - 3.925 * sinAngleSquared;
    const double stratifiedFlowBoundary = pow(10.0, stratifiedBoundaryExponent);
    
    // Bubble vs stratified boundary: gasVelNumber threshold (downward flow)
    const double bubbleVsStratifiedExponent = 0.431
                                             + 1.132 * sinAngle
                                             - 3.003 * liquidNumber
                                             - 1.138 * log10LiquidViscNumber * sinAngle
                                             - 0.429 * log10LiquidViscNumberSquared * sinAngle;
    const double bubbleVsStratifiedBoundary = pow(10.0, bubbleVsStratifiedExponent);
    
    // Flow Pattern Determination
    
    // Check mist flow first (applies to all angles)
    if (gasVelNumber >= mistFlowBoundary) {
        flowPattern = FLOW_MIST;
        return;
    }
    
    // Upward flow (angle > 0)
    if (angle > 0.0) {
        if (liquidViscNumber > bubbleFlowBoundary) {
            flowPattern = FLOW_BUBBLE;
        } else {
            flowPattern = FLOW_SLUG;
        }
        return;
    }
    
    // Downward flow (angle < -30 degrees)
    if (absoluteAngle > DOWNWARD_ANGLE_THRESHOLD) {
        if (gasVelNumber <= bubbleVsStratifiedBoundary) {
            flowPattern = FLOW_BUBBLE;
        } else if (liquidViscNumber > stratifiedFlowBoundary) {
            flowPattern = FLOW_SLUG;
        } else {
            flowPattern = FLOW_STRATIFIED;
        }
        return;
    }
    
    // Horizontal/near-horizontal flow (|angle| <= 30 degrees)
    if (liquidViscNumber > stratifiedFlowBoundary) {
        if (gasVelNumber > bubbleVsStratifiedBoundary) {
            flowPattern = FLOW_SLUG;
        } else {
            flowPattern = FLOW_BUBBLE;
        }
    } else {
        flowPattern = FLOW_STRATIFIED;
    }
}

/*
==============================================================================
mukherjeeeBrillHoldup: Calculates liquid holdup using the Mukherjee-Brill
                      correlation.

Variable dictionary:
- liquidViscNumber : Dimensionless liquid velocity number
- gasVelNumber     : Dimensionless gas velocity number
- liquidNumber     : Dimensionless liquid viscosity number
- angle            : Pipe inclination angle (degrees)
- flowPattern      : Flow pattern
                     (3 = bubble, 4 = slug, 5 = mist, 6 = stratified)
- holdup           : Calculated liquid holdup (output)

Auxiliary variables:
- angleRad         : Angle in radians
- sinAngle         : Sine of the pipe inclination angle
- exponentExp      : Exponent used in the liquid holdup expression
- exponentResult   : Result of the exponential expression
==============================================================================
*/
void mukherjeeeBrillHoldup(double liquidViscNumber, double gasVelNumber, double liquidNumber,
                          double angle, unsigned char flowPattern, double& holdup) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_STRATIFIED = 6;
    
    // Holdup Correlation Coefficients
    
    // Upward/horizontal flow coefficients
    static constexpr double UPWARD_C0 = -0.380113;
    static constexpr double UPWARD_C1 =  0.129875;
    static constexpr double UPWARD_C2 = -0.119788;
    static constexpr double UPWARD_C3 =  2.343227;
    static constexpr double UPWARD_EXP_GAS = 0.475686;
    static constexpr double UPWARD_EXP_LIQ = -0.288657;
    
    // Downward stratified flow coefficients
    static constexpr double STRAT_C0 = -1.330282;
    static constexpr double STRAT_C1 =  4.808139;
    static constexpr double STRAT_C2 =  4.171584;
    static constexpr double STRAT_C3 = 56.262268;
    static constexpr double STRAT_EXP_GAS = 0.079951;
    static constexpr double STRAT_EXP_LIQ = -0.504887;
    
    // Downward non-stratified flow coefficients (bubble/slug/mist)
    static constexpr double DOWNWARD_C0 = -0.516644;
    static constexpr double DOWNWARD_C1 =  0.789805;
    static constexpr double DOWNWARD_C2 =  0.551627;
    static constexpr double DOWNWARD_C3 = 15.519214;
    static constexpr double DOWNWARD_EXP_GAS = 0.371771;
    static constexpr double DOWNWARD_EXP_LIQ = -0.393952;
    
    // Pre-computed Angle Properties
    const double angleRad = angle * M_PI / 180.0;
    const double sinAngle = sin(angleRad);
    const double sinAngleSquared = sinAngle * sinAngle;
    const double liquidNumberSquared = liquidNumber * liquidNumber;
    
    // Pre-computed Power Terms
    const double gasVelPowerUpward = pow(gasVelNumber, UPWARD_EXP_GAS);
    const double liquidViscPowerUpward = pow(liquidViscNumber, UPWARD_EXP_LIQ);
    const double gasVelPowerStrat = pow(gasVelNumber, STRAT_EXP_GAS);
    const double liquidViscPowerStrat = pow(liquidViscNumber, STRAT_EXP_LIQ);
    const double gasVelPowerDownward = pow(gasVelNumber, DOWNWARD_EXP_GAS);
    const double liquidViscPowerDownward = pow(liquidViscNumber, DOWNWARD_EXP_LIQ);
    
    // Holdup Calculation
    
    double exponentBase;
    double exponentMultiplier;
    
    // Upward or horizontal flow (all patterns use same correlation)
    if (angle >= 0.0) {
        exponentBase = UPWARD_C0
                      + UPWARD_C1 * sinAngle
                      + UPWARD_C2 * sinAngleSquared
                      + UPWARD_C3 * liquidNumberSquared;
        exponentMultiplier = gasVelPowerUpward * liquidViscPowerUpward;
        holdup = exp(exponentBase * exponentMultiplier);
        return;
    }
    
    // Downward flow: stratified pattern
    if (flowPattern == FLOW_STRATIFIED) {
        exponentBase = STRAT_C0
                      + STRAT_C1 * sinAngle
                      + STRAT_C2 * sinAngleSquared
                      + STRAT_C3 * liquidNumberSquared;
        exponentMultiplier = gasVelPowerStrat * liquidViscPowerStrat;
        holdup = exp(exponentBase * exponentMultiplier);
        return;
    }
    
    // Downward flow: bubble, slug, or mist patterns
    exponentBase = DOWNWARD_C0
                  + DOWNWARD_C1 * sinAngle
                  + DOWNWARD_C2 * sinAngleSquared
                  + DOWNWARD_C3 * liquidNumberSquared;
    exponentMultiplier = gasVelPowerDownward * liquidViscPowerDownward;
    holdup = exp(exponentBase * exponentMultiplier);
}

/*
==============================================================================
mukherjeeeBrillStratifiedFlow: Calculates pressure gradients for stratified
                              flow using the Mukherjee-Brill correlation.

Variable dictionary:
- angleRad            : Pipe inclination angle (radians)
- velocity            : Total superficial velocity (ft/s)
- liquidFraction      : No-slip liquid volumetric fraction (0-1)
- holdup              : Calculated liquid holdup (0-1)
- diameter            : Pipe inner diameter (inches)
- roughness           : Relative roughness (dimensionless)
- gasDensity          : Gas density (lb/ft3)
- gasViscosity        : Gas viscosity (cP)
- frictionGrad        : Frictional pressure gradient (psi/ft) - output
- gravityGrad         : Gravitational pressure gradient (psi/ft) - output
- totalGrad           : Total pressure gradient (psi/ft) - output
- convergeFlag        : Convergence flag
                        (0 = converged, 1 = did not converge) - output

Auxiliary variables:
- convergeStatus      : Iteration convergence status
- chordAngle          : Angle subtended by the liquid chord
- iterations          : Iteration counter
- gamma               : Auxiliary variable used by the iterative solution
- auxiliarX, auxiliarY: Auxiliary variables used to accelerate convergence
- hydraulicDiameter   : Gas-phase hydraulic diameter
- effectiveDiameter   : Effective hydraulic diameter
- liquidVelocity      : Superficial liquid velocity
- gasVelocity         : Superficial gas velocity
- realGasVelocity     : Actual gas velocity
- reynolds            : Gas-phase Reynolds number
- frictionFactorValue : Moody friction factor
==============================================================================
*/
void mukherjeeeBrillStratifiedFlow(double angleRad, double velocity, double liquidFraction,
                                 double holdup, double diameter, double roughness, double gasDensity,
                                 double gasViscosity, double& frictionGrad, double& gravityGrad,
                                 double& totalGrad, unsigned char& convergeFlag) {
    
    // Iteration Control Constants
    static constexpr unsigned char MAX_ITERATIONS = 20;
    static constexpr double CONVERGENCE_TOLERANCE = 0.01;
    static constexpr double INITIAL_CHORD_ANGLE = 0.001;
    static constexpr double ZERO_TOLERANCE = 1e-15;
    
    // Physical Constants
    static constexpr double TWO_PI = 6.283186;                 // 2π
    static constexpr double REYNOLDS_FACTOR = 1488.0;          // ft-lbm/cp-psi conversion
    static constexpr double GRAVITY_ACCEL = 64.4;              // ft/s²
    static constexpr double PSI_CONVERSION = 144.0;            // in²/ft²
    
    // Iterative Solution for Chord Angle
    unsigned char convergeStatus = 0;
    double chordAngle = INITIAL_CHORD_ANGLE;
    unsigned char iterations = 0;
    double gamma, auxiliarX, auxiliarY;
    
    do {
        gamma = TWO_PI * holdup + sin(chordAngle);
        accelerateConvergence(chordAngle, gamma, convergeStatus, CONVERGENCE_TOLERANCE,
                            auxiliarX, auxiliarY);
        iterations++;
    } while ((iterations <= MAX_ITERATIONS) && (convergeStatus <= 1));
    
    // Set convergence flag (0=converged, 1=did not converge)
    convergeFlag = (iterations > MAX_ITERATIONS) ? 1 : 0;
    
    // Hydraulic Diameter Calculation
    
    // Calculate wetted perimeter terms
    const double gammaMinusSinGamma = gamma - sin(gamma);
    const double halfGamma = gamma / 2.0;
    const double sinHalfGamma = sin(halfGamma);
    
    // Hydraulic diameter ratio for gas phase
    const double numerator = TWO_PI - gammaMinusSinGamma;
    const double denominator = TWO_PI - gamma + 2.0 * sinHalfGamma;
    const double hydraulicDiameterRatio = numerator / denominator;
    const double effectiveDiameter = hydraulicDiameterRatio * diameter;
    
    // Superficial Velocities
    const double liquidVelocity = velocity * liquidFraction;
    const double gasVelocity = velocity - liquidVelocity;
    const double gasHoldup = 1.0 - holdup;
    
    // Friction Gradient (Gas Phase Based)
    
    if ((fabs(gasHoldup) > ZERO_TOLERANCE) && (fabs(gasVelocity) > ZERO_TOLERANCE)) {
        // Real gas velocity through available cross-section
        const double realGasVelocity = gasVelocity / gasHoldup;
        
        // Reynolds number for gas phase
        const double reynolds = REYNOLDS_FACTOR * effectiveDiameter * realGasVelocity
                               * gasDensity / gasViscosity;
        
        // Moody friction factor
        double frictionFactorValue;
        frictionFactor(reynolds, roughness, frictionFactorValue);
        
        // Friction pressure gradient (psi/ft)
        frictionGrad = frictionFactorValue * realGasVelocity * realGasVelocity * gasDensity
                      / (GRAVITY_ACCEL * effectiveDiameter * PSI_CONVERSION);
    } else {
        frictionGrad = 0.0;
    }
    
    // Gravity Gradient
    gravityGrad = gasDensity * sin(angleRad) / PSI_CONVERSION;
    
    // Total Gradient
    totalGrad = frictionGrad + gravityGrad;
}

/*
==============================================================================
mukherjeeeBrill: Calculates liquid holdup, flow pattern, and pressure gradients
                 using the complete Mukherjee-Brill correlation.

References:
     1.  Mukherjee, H. and Brill, J. P.: "Empirical Equations to Predict
         Flow Patterns in Two Phase Inclined Flow," International Journal
         of Multiphase Flow, Vol. 11, No. 3 (May-June 1985), 299-315.
     2.  Mukherjee, H.: "An Experimental Study of Inclined Two Phase Flow,"
         Ph.D. Dissertation, The University of Tulsa (1979).
     3.  Mukherjee, H. and Brill, J. P.: "Liquid Holdup Correlations for
         Inclined Two Phase Flow," Journal of Petroleum Technology
         (May 1983), 1003-1008.
     4.  Mukherjee, H. and Brill, J. P.: "Pressure Drop Correlations for
         Inclined Two Phase Flow," Journal of Energy Resources Technology,
         Vol. 107 (Dec. 1985), 549-554.

Variable dictionary:
- angle             : Pipe inclination angle (degrees)
- diameter          : Pipe inner diameter (inches)
- roughness         : Relative roughness (dimensionless)
- pressure          : Pressure (psi)
- velocity          : Mixture velocity (ft/s)
- liquidFraction    : No-slip liquid volumetric fraction (0-1)
- gasDensity        : Gas density (lb/ft3)
- liquidDensity     : Liquid density (lb/ft3)
- gasViscosity      : Gas viscosity (cP)
- liquidViscosity   : Liquid viscosity (cP)
- liquidViscNumber  : Dimensionless liquid velocity number
- gasVelNumber      : Dimensionless gas velocity number
- liquidNumber      : Dimensionless liquid viscosity number
- holdup            : Calculated liquid holdup (output)
- frictionGrad      : Frictional pressure gradient (psi/ft)
- gravityGrad       : Gravitational pressure gradient (psi/ft)
- accelGrad         : Accelerational pressure gradient (psi/ft)
- totalGrad         : Total pressure gradient (psi/ft)
- reynolds          : Reynolds number (output)
- flowPattern       : Flow pattern (output)
- criticalFlag      : Critical-flow flag (output)
- convergeFlag      : Convergence flag (output)

Data arrays:
- frictionRatio[9]  : Friction-factor ratio for mist flow
- holdupRatio[9]    : Liquid-holdup ratio for mist flow

Subroutines used:
- mukherjeeeBrillFlowPattern    : Determines the flow pattern
- mukherjeeeBrillHoldup         : Calculates liquid holdup
- mukherjeeeBrillStratifiedFlow : Calculates pressure gradients for
                                  stratified flow
==============================================================================
*/
void mukherjeeeBrill(double angle, double diameter, double roughness, double pressure,
                    double velocity, double liquidFraction, double gasDensity,
                    double liquidDensity, double gasViscosity, double liquidViscosity,
                    double surfaceTension, double temperature, double compressibilityFactor,
                    double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
                    double& totalGrad, double& reynolds, unsigned char& flowPattern,
                    unsigned char& criticalFlag, unsigned char& convergeFlag) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS    = 2;
    static constexpr unsigned char FLOW_MIST   = 5;
    static constexpr unsigned char FLOW_STRATIFIED = 6;
    
    // Physical Constants
    static constexpr double DIMENSIONLESS_VEL_FACTOR = 1.938;     // Velocity dimensionless number factor
    static constexpr double DIMENSIONLESS_VISC_FACTOR = 0.15726;  // Viscosity dimensionless number factor
    static constexpr double REYNOLDS_FACTOR = 1488.0;             // ft-lbm/cp-psi conversion
    static constexpr double GRAVITY_ACCEL = 64.4;                 // ft/s²
    static constexpr double PSI_CONVERSION = 144.0;               // in²/ft²
    
    // Flow Regime Thresholds
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.9999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.0001;
    static constexpr double ZERO_VELOCITY_TOLERANCE = 1e-15;
    static constexpr double ZERO_HOLDUP_TOLERANCE = 1e-15;
    static constexpr double MIST_HOLDUP_THRESHOLD = 0.60;
    static constexpr double CRITICAL_FLOW_LIMIT = 0.95;
    
    // Mist Flow Correlation (replaced by analytical fits in CorrelationFits.h)
    
    // Pre-computed Values
    const double angleRad = angle * M_PI / 180.0;
    const double sinAngle = sin(angleRad);
    const double velocitySquared = velocity * velocity;
    
    // Superficial velocities
    const double liquidSupVel = velocity * liquidFraction;
    const double gasSupVel = velocity - liquidSupVel;
    
    // Dimensionless surface tension term
    const double densitySurfaceTensionRoot = pow(liquidDensity / surfaceTension, 0.25);
    
    // Dimensionless numbers for flow pattern and holdup correlations
    const double liquidViscNumber = DIMENSIONLESS_VEL_FACTOR * densitySurfaceTensionRoot * liquidSupVel;
    const double gasVelNumber = DIMENSIONLESS_VEL_FACTOR * densitySurfaceTensionRoot * gasSupVel;
    const double surfaceTensionCubed = surfaceTension * surfaceTension * surfaceTension;
    const double liquidNumber = DIMENSIONLESS_VISC_FACTOR * liquidViscosity
                               * pow(1.0 / (liquidDensity * surfaceTensionCubed), 0.25);
    
    // Initialize output flags
    criticalFlag = 0;
    convergeFlag = 0;
    
    // Single-Phase Flow Check
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowPattern = FLOW_LIQUID;
        holdup = liquidFraction;
        // For single-phase liquid, other gradients would be calculated elsewhere
        return;
    }
    
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowPattern = FLOW_GAS;
        holdup = liquidFraction;
        // For single-phase gas, other gradients would be calculated elsewhere
        return;
    }
    
    // Two-Phase Flow Pattern and Holdup
    
    // Check for zero velocity
    if (fabs(velocity) <= ZERO_VELOCITY_TOLERANCE) {
        flowPattern = FLOW_MIST;  // Default pattern
        holdup = liquidFraction;
    } else {
        // Determine flow pattern using dimensionless numbers
        mukherjeeeBrillFlowPattern(liquidViscNumber, gasVelNumber, liquidNumber, angle,
                                  flowPattern);
        
        // Calculate liquid holdup
        mukherjeeeBrillHoldup(liquidViscNumber, gasVelNumber, liquidNumber, angle,
                             flowPattern, holdup);
        
        // For upward flow, holdup cannot be less than no-slip liquid fraction
        if ((angle >= 0.0) && (holdup < liquidFraction)) {
            holdup = liquidFraction;
        }
        
        // Limit holdup to physical range
        if (holdup > 1.0) {
            holdup = liquidFraction;
        }
        
        // Stratified Flow Special Treatment
        if (flowPattern == FLOW_STRATIFIED) {
            mukherjeeeBrillStratifiedFlow(angleRad, velocity, liquidFraction, holdup,
                                         diameter, roughness, gasDensity, gasViscosity,
                                         frictionGrad, gravityGrad, totalGrad, convergeFlag);
            // Acceleration gradient is zero for stratified flow in this model
            accelGrad = 0.0;
            reynolds = 0.0;  // Reynolds number calculated within stratified flow function
            return;
        }
    }
    
    // Mixture Properties
    const double mixtureNoSlipDensity = liquidDensity * liquidFraction
                                       + gasDensity * (1.0 - liquidFraction);
    const double mixtureSlipDensity = liquidDensity * holdup
                                     + gasDensity * (1.0 - holdup);
    const double mixtureNoSlipViscosity = liquidViscosity * liquidFraction
                                         + gasViscosity * (1.0 - liquidFraction);
    
    // Friction Gradient (Non-Stratified Flows)
    
    if (fabs(velocity) > ZERO_VELOCITY_TOLERANCE) {
        // Reynolds number for mixture
        reynolds = REYNOLDS_FACTOR * diameter * mixtureNoSlipDensity * velocity
                  / mixtureNoSlipViscosity;
        
        // Moody friction factor
        double frictionFactorValue;
        frictionFactor(reynolds, roughness, frictionFactorValue);
        
        // Mist flow with low holdup requires special friction correlation
        if ((holdup < MIST_HOLDUP_THRESHOLD) && (flowPattern == FLOW_MIST)) {
            if (fabs(holdup) > ZERO_HOLDUP_TOLERANCE) {
                // Calculate holdup ratio and interpolate friction multiplier
                const double holdupRatioValue = log10(liquidFraction / holdup);
                const double frictionMultiplier = CorrelationFits::mukherjeeFrictionRatio(holdupRatioValue);
                frictionGrad = frictionMultiplier * frictionFactorValue * velocitySquared
                              * mixtureNoSlipDensity / (GRAVITY_ACCEL * diameter);
            } else {
                frictionGrad = 0.0;
            }
        } else {
            // Standard friction gradient for bubble and slug flows
            frictionGrad = frictionFactorValue * velocitySquared * mixtureSlipDensity
                          / (GRAVITY_ACCEL * diameter);
        }
    } else {
        reynolds = 0.0;
        frictionGrad = 0.0;
    }
    
    // Convert friction gradient to psi/ft
    frictionGrad = frictionGrad / PSI_CONVERSION;
    
    // Gravity Gradient
    gravityGrad = mixtureSlipDensity * sinAngle / PSI_CONVERSION;
    
    // Acceleration Effects
    double accelerationParam = mixtureSlipDensity * velocity * gasSupVel
                              / (g_in * pressure * PSI_CONVERSION);
    
    // Check for critical flow conditions
    if (accelerationParam > CRITICAL_FLOW_LIMIT) {
        accelerationParam = CRITICAL_FLOW_LIMIT;
        criticalFlag = 1;
    }
    
    // Total Gradient
    totalGrad = (gravityGrad + frictionGrad) / (1.0 - accelerationParam);
    accelGrad = accelerationParam * totalGrad;
}


/*
==============================================================================
azizRegimeMap: Predicts the flow pattern using the Govier, Redford, and Dunn
               flow map for the Aziz et al. correlation.

Variable dictionary:
- liquidSupVel             : Superficial liquid velocity (ft/s)
- gasSupVel                : Superficial gas velocity (ft/s)
- liquidDensity            : Liquid density (lbm/ft3)
- gasDensity               : Gas density (lbm/ft3)
- surfaceTension           : Surface tension (lbm/s2)
- liquidFraction           : No-slip liquid fraction
- surfaceTensionWaterAir   : Water-air surface tension (lbm/s2)
- bubbleToSlugBoundary     : Bubble-to-slug flow boundary
- slugToTransitionBoundary : Slug-to-transition flow boundary
- transitionToMistBoundary : Transition-to-mist flow boundary
- flowMapCoordinateX       : X-coordinate in the flow-pattern map
- flowMapCoordinateY       : Y-coordinate in the flow-pattern map
- flowPattern              : Flow-pattern indicator
                             1 = liquid, 2 = gas, 3 = bubble,
                             4 = slug, 5 = mist, 6 = transition
==============================================================================
*/
void azizRegimeMap(double liquidSupVel, double gasSupVel, double liquidDensity, double gasDensity,
                  double surfaceTension, double liquidFraction,
                  double& bubbleToSlugBoundary, double& slugToTransitionBoundary, double& transitionToMistBoundary,
                  double& flowMapCoordinateX, double& flowMapCoordinateY, unsigned char& flowPattern) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_BUBBLE = 3;
    static constexpr unsigned char FLOW_SLUG = 4;
    static constexpr unsigned char FLOW_MIST = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    // Reference Constants (Govier-Redford-Dunn Map)
    static constexpr double WATER_AIR_SURFACE_TENSION = 0.162;  // lbm/s² at standard conditions
    static constexpr double WATER_DENSITY_REFERENCE = 62.4;     // lbm/ft³ at standard conditions
    static constexpr double AIR_DENSITY_REFERENCE = 0.0806;     // lbm/ft³ at standard conditions
    
    // Flow regime boundaries
    static constexpr double BUBBLE_BOUNDARY_COEFF = 0.51;
    static constexpr double BUBBLE_BOUNDARY_EXPONENT = 0.172;
    static constexpr double BUBBLE_SCALE_FACTOR = 100.0;
    
    static constexpr double HIGH_LIQUID_VEL_THRESHOLD = 4.0;
    static constexpr double MIST_BOUNDARY_HIGH_LIQ = 26.5;
    
    static constexpr double SLUG_TRANSITION_SLOPE = 1.0 / 0.263;  // ~3.8023
    static constexpr double SLUG_TRANSITION_INTERCEPT = 8.6;
    
    static constexpr double TRANSITION_MIST_COEFF = 70.0;
    static constexpr double TRANSITION_MIST_EXPONENT = -0.152;
    
    static constexpr double MIN_FLOW_MAP_X = 0.1;  // Minimum coordinate value
    
    // Single-phase thresholds
    static constexpr double SINGLE_PHASE_LIQUID_THRESHOLD = 0.99999;
    static constexpr double SINGLE_PHASE_GAS_THRESHOLD = 0.00001;
    
    // Initialization
    flowMapCoordinateX = 0.0;
    flowMapCoordinateY = 0.0;
    bubbleToSlugBoundary = 0.0;
    slugToTransitionBoundary = 0.0;
    transitionToMistBoundary = 0.0;
    
    // Single-Phase Flow Check
    if (liquidFraction > SINGLE_PHASE_LIQUID_THRESHOLD) {
        flowPattern = FLOW_LIQUID;
        return;
    }
    
    if (liquidFraction < SINGLE_PHASE_GAS_THRESHOLD) {
        flowPattern = FLOW_GAS;
        return;
    }
    
    // Two-Phase Flow: Govier-Redford-Dunn Map Coordinates
    
    // Dimensionless density scaling factors
    const double densityRatio = liquidDensity * WATER_AIR_SURFACE_TENSION /
                               (WATER_DENSITY_REFERENCE * surfaceTension);
    const double fourthRootDensityRatio = pow(densityRatio, 0.25);
    
    const double gasDensityRatio = gasDensity / AIR_DENSITY_REFERENCE;
    const double cubeRootGasDensityRatio = pow(gasDensityRatio, 1.0 / 3.0);
    
    // Scale factors for map coordinates
    const double liquidScaleFactor = fourthRootDensityRatio;
    const double gasScaleFactor = fourthRootDensityRatio * cubeRootGasDensityRatio;
    
    // Flow map coordinates
    flowMapCoordinateY = liquidScaleFactor * liquidSupVel;
    flowMapCoordinateX = gasScaleFactor * gasSupVel;
    
    // Enforce minimum coordinate value
    if (flowMapCoordinateX < MIN_FLOW_MAP_X) {
        flowMapCoordinateX = MIN_FLOW_MAP_X;
    }
    
    // Flow Pattern Determination
    
    // Bubble-to-slug boundary
    bubbleToSlugBoundary = BUBBLE_BOUNDARY_COEFF *
                          pow(BUBBLE_SCALE_FACTOR * flowMapCoordinateY, BUBBLE_BOUNDARY_EXPONENT);
    
    // Check bubble flow first
    if (flowMapCoordinateX <= bubbleToSlugBoundary) {
        flowPattern = FLOW_BUBBLE;
        return;
    }
    
    // High liquid velocity regime (simplified boundaries)
    if (flowMapCoordinateY > HIGH_LIQUID_VEL_THRESHOLD) {
        if (flowMapCoordinateX > MIST_BOUNDARY_HIGH_LIQ) {
            flowPattern = FLOW_MIST;
        } else {
            flowPattern = FLOW_SLUG;
        }
        return;
    }
    
    // Low liquid velocity regime (requires transition check)
    slugToTransitionBoundary = SLUG_TRANSITION_SLOPE * flowMapCoordinateY + SLUG_TRANSITION_INTERCEPT;
    
    if (flowMapCoordinateX <= slugToTransitionBoundary) {
        flowPattern = FLOW_SLUG;
        return;
    }
    
    // Check transition-to-mist boundary
    transitionToMistBoundary = TRANSITION_MIST_COEFF *
                               pow(BUBBLE_SCALE_FACTOR * flowMapCoordinateY, TRANSITION_MIST_EXPONENT);
    
    if (flowMapCoordinateX <= transitionToMistBoundary) {
        flowPattern = FLOW_TRANSITION;
    } else {
        flowPattern = FLOW_MIST;
    }
}

/*
==============================================================================
azizDensity: Calculates liquid holdup and slip density for the Aziz, Govier,
             and Fogarasi correlation.

Variable dictionary:
- gasSupVel               : Superficial gas velocity (ft/s)
- velocity                : Mixture superficial velocity (ft/s)
- surfaceTension          : Surface tension (code units)
- liquidDensity           : Liquid density (lbm/ft3)
- gasDensity              : Gas density (lbm/ft3)
- diameter                : Pipe inner diameter (ft)
- liquidViscosity         : Liquid viscosity (cP)
- transitionToMistBoundary: X-limit for the transition-to-mist boundary
- flowMapCoordinateX      : X-coordinate in the flow-pattern map
- slugToTransitionBoundary: X-limit for the slug-to-transition boundary
- flowPattern             : Flow-pattern indicator
                            1 = liquid, 2 = gas, 3 = bubble,
                            4 = slug, 5 = mist, 6 = transition

Output and return variables:
- holdup            : Liquid holdup fraction
- bubbleLength      : Bubble length (ft)
- slugLength        : Slug length (ft)
- slipDensity       : Slip density (lbm/ft3)
- slugLiquidDensity : Liquid-slug slip density (lbm/ft3)
==============================================================================
*/
void azizDensity(double gasSupVel, double velocity, double surfaceTension,
                 double liquidDensity, double gasDensity, double diameter, double liquidViscosity,
                 double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary, unsigned char flowPattern,
                 double& holdup, double& bubbleLength, double& slugLength,
                 double& slipDensity, double& slugLiquidDensity) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_BUBBLE = 3;
    static constexpr unsigned char FLOW_SLUG = 4;
    static constexpr unsigned char FLOW_MIST = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    // Bubble Flow Constants
    static constexpr double BUBBLE_RISE_COEFF = 1.41;        // Bubble rise velocity coefficient
    static constexpr double BUBBLE_RISE_EXPONENT = 0.25;     // Quarter power for buoyancy
    static constexpr double EFFECTIVE_VEL_MULTIPLIER = 1.25; // Drift velocity multiplier
    
    // Slug Flow Constants
    static constexpr double SLUG_NUMBER_SCALE = 1488.0;      // Aziz slug number scaling
    static constexpr double SLUG_LENGTH_FACTOR = 10.0;       // Slug length = 10 * diameter
    static constexpr double EFFECTIVE_SLUG_VEL_MULT = 1.2;   // Slug velocity multiplier
    
    // Bubble rise velocity correlation constants
    static constexpr double BUBBLE_RISE_BASE = 0.345;
    static constexpr double SLUG_NUM_THRESHOLD_LOW = 18.0;
    static constexpr double SLUG_NUM_THRESHOLD_HIGH = 250.0;
    static constexpr double M_PARAM_LOW = 25.0;
    static constexpr double M_PARAM_HIGH = 10.0;
    static constexpr double M_PARAM_COEFF = 69.0;
    static constexpr double M_PARAM_EXPONENT = -0.35;
    
    // Exponential saturation limit
    static constexpr double EXP_SATURATION_LIMIT = -10.0;
    static constexpr double EXP_COEFF1_DENOM = 0.345;
    static constexpr double EXP_COEFF1_NUMER = -0.01;
    static constexpr double EOTVOS_REFERENCE = 3.37;
    
    // Slug unit geometry constants
    static constexpr double GAS_FRACTION_EXPONENT = 1.8;
    static constexpr double BUBBLE_LENGTH_COEFF = 7.5;
    static constexpr double BUBBLE_LENGTH_OFFSET = 0.526;
    static constexpr double GAS_FRACTION_SINGULARITY = 0.913;
    
    // Tolerances
    static constexpr double ZERO_VELOCITY_TOLERANCE = 1e-15;
    static constexpr double ZERO_DIFFERENCE_TOLERANCE = 1e-15;
    static constexpr double DEFAULT_HOLDUP_EQUAL_DENSITY = 0.5;
    
    // Initialization
    bubbleLength = 0.0;
    slipDensity = 0.0;
    slugLiquidDensity = 0.0;
    holdup = 0.0;
    slugLength = 0.0;
    
    // Single-Phase Flow Patterns
    if (flowPattern == FLOW_LIQUID) {
        slipDensity = liquidDensity;
        holdup = 1.0;
        return;
    }
    
    if (flowPattern == FLOW_GAS) {
        slipDensity = gasDensity;
        holdup = 0.0;
        return;
    }
    
    // Bubble Flow Pattern
    if (flowPattern == FLOW_BUBBLE) {
        // Bubble rise velocity in stagnant liquid (Harmathy correlation)
        const double densityDifference = liquidDensity - gasDensity;
        const double buoyancyTerm = (surfaceTension * g_in * densityDifference) /
                                   (liquidDensity * liquidDensity);
        const double bubbleRiseVelocity = BUBBLE_RISE_COEFF * pow(buoyancyTerm, BUBBLE_RISE_EXPONENT);
        
        // Effective bubble rise velocity in flowing liquid (drift flux)
        const double effectiveBubbleVelocity = EFFECTIVE_VEL_MULTIPLIER * velocity + bubbleRiseVelocity;
        
        // Gas fraction and liquid holdup
        if (fabs(effectiveBubbleVelocity) > ZERO_VELOCITY_TOLERANCE) {
            const double gasFraction = gasSupVel / effectiveBubbleVelocity;
            holdup = 1.0 - gasFraction;
            slipDensity = holdup * liquidDensity + gasFraction * gasDensity;
        } else {
            holdup = 1.0;
            slipDensity = liquidDensity;
        }
        return;
    }
    
    // Slug or Transition Flow Patterns
    if (flowPattern == FLOW_SLUG || flowPattern == FLOW_TRANSITION) {
        // Dimensionless Numbers for Slug Flow
        const double densityDifference = liquidDensity - gasDensity;
        
        // Aziz slug number (modified Froude number)
        // Note: Compute inline to match original precision exactly
        const double azizSlugNumber = SLUG_NUMBER_SCALE *
                                     sqrt(pow(diameter, 3) * g_in * (liquidDensity - gasDensity) * liquidDensity) /
                                     liquidViscosity;
        
        // Eötvös number (ratio of buoyancy to surface tension forces)
        // Note: Compute inline to match original precision exactly
        const double eotvosNumber = g_in * (liquidDensity - gasDensity) * (diameter * diameter) / surfaceTension;
        
        // Parameter M (empirical correlation based on slug number)
        double parameterM;
        if (azizSlugNumber <= SLUG_NUM_THRESHOLD_LOW) {
            parameterM = M_PARAM_LOW;
        } else if (azizSlugNumber <= SLUG_NUM_THRESHOLD_HIGH) {
            parameterM = M_PARAM_COEFF * pow(azizSlugNumber, M_PARAM_EXPONENT);
        } else {
            parameterM = M_PARAM_HIGH;
        }
        
        // Bubble Rise Velocity Correction Factors
        
        // Coefficient 1: viscosity effect (saturates for high slug numbers)
        const double expArg1 = EXP_COEFF1_NUMER * azizSlugNumber / EXP_COEFF1_DENOM;
        const double coeff1 = (expArg1 < EXP_SATURATION_LIMIT) ? 1.0 : (1.0 - exp(expArg1));
        
        // Coefficient 2: diameter and surface tension effect
        const double expArg2 = (EOTVOS_REFERENCE - eotvosNumber) / parameterM;
        const double coeff2 = (expArg2 < EXP_SATURATION_LIMIT) ? 1.0 : (1.0 - exp(expArg2));
        
        // Combined correction factor
        const double correctionFactor = BUBBLE_RISE_BASE * coeff1 * coeff2;
        
        // Bubble Rise and Effective Velocities
        // Note: Compute inline to match original precision exactly
        const double bubbleRiseVelocity = correctionFactor *
                                         sqrt(g_in * diameter * (liquidDensity - gasDensity) / liquidDensity);
        const double effectiveBubbleVelocity = EFFECTIVE_SLUG_VEL_MULT * velocity + bubbleRiseVelocity;
        
        // Slug Unit Geometry
        double gasFraction = 0.0;
        double gasFractionInSlug = 0.0;
        
        if (fabs(effectiveBubbleVelocity) > ZERO_VELOCITY_TOLERANCE) {
            gasFraction = gasSupVel / effectiveBubbleVelocity;
            gasFractionInSlug = pow(gasFraction, GAS_FRACTION_EXPONENT);
            
            // Bubble length (length of gas pockets between slugs)
            const double bubbleLengthTerm = BUBBLE_LENGTH_COEFF * (gasFractionInSlug - gasFraction) -
                                           BUBBLE_LENGTH_OFFSET;
            const double fractionDenominator = gasFraction - GAS_FRACTION_SINGULARITY;
            
            if (fabs(fractionDenominator) > ZERO_DIFFERENCE_TOLERANCE) {
                bubbleLength = diameter * bubbleLengthTerm / fractionDenominator;
            } else {
                bubbleLength = 0.0;
            }
            
            // Slug liquid body density (density of liquid slug with entrained gas)
            slugLiquidDensity = gasFractionInSlug * gasDensity +
                               (1.0 - gasFractionInSlug) * liquidDensity;
        } else {
            gasFraction = 0.0;
            gasFractionInSlug = 0.0;
            bubbleLength = 0.0;
            slugLiquidDensity = liquidDensity;
        }
        
        // Slug Unit Average Density
        slugLength = SLUG_LENGTH_FACTOR * diameter;
        const double totalUnitLength = bubbleLength + slugLength;
        
        double slugMixtureDensity;
        if (fabs(totalUnitLength) > ZERO_DIFFERENCE_TOLERANCE) {
            slugMixtureDensity = (gasDensity * bubbleLength + slugLiquidDensity * slugLength) /
                                totalUnitLength;
        } else {
            slugMixtureDensity = liquidDensity;
        }
        
        // Transition vs. Pure Slug Flow
        if (flowPattern == FLOW_TRANSITION) {
            // Mist flow density (no slip between phases)
            double mistGasFraction;
            if (fabs(gasSupVel) > ZERO_VELOCITY_TOLERANCE) {
                mistGasFraction = 1.0 / (1.0 + (velocity - gasSupVel) / gasSupVel);
            } else {
                mistGasFraction = 0.0;
            }
            
            const double mistMixtureDensity = (1.0 - mistGasFraction) * liquidDensity +
                                             mistGasFraction * gasDensity;
            
            // Weighted average between slug and mist densities
            // Note: Divide each term separately to match original precision exactly
            const double transitionRange = transitionToMistBoundary - slugToTransitionBoundary;
            if (fabs(transitionRange) > ZERO_DIFFERENCE_TOLERANCE) {
                slipDensity = (transitionToMistBoundary - flowMapCoordinateX) * slugMixtureDensity / transitionRange
                            + (flowMapCoordinateX - slugToTransitionBoundary) * mistMixtureDensity / transitionRange;
            } else {
                slipDensity = slugMixtureDensity;
            }
        } else {
            // Pure slug flow
            slipDensity = slugMixtureDensity;
        }
        
        // Back-calculate holdup from slip density
        const double densityRange = liquidDensity - gasDensity;
        if (fabs(densityRange) > ZERO_DIFFERENCE_TOLERANCE) {
            holdup = (slipDensity - gasDensity) / densityRange;
        } else {
            holdup = DEFAULT_HOLDUP_EQUAL_DENSITY;
        }
        
        return;
    }
    
    // Mist Flow Pattern
    if (flowPattern == FLOW_MIST) {
        // Gas fraction (no slip between phases)
        double gasFraction;
        if (fabs(gasSupVel) > ZERO_VELOCITY_TOLERANCE) {
            gasFraction = 1.0 / (1.0 + (velocity - gasSupVel) / gasSupVel);
        } else {
            gasFraction = 0.0;
        }
        
        holdup = 1.0 - gasFraction;
        slipDensity = holdup * liquidDensity + gasFraction * gasDensity;
        return;
    }
}

/*
==============================================================================
azizFriction: Calculates the frictional pressure gradient using the Aziz et al.
              correlation.

Variable dictionary:
- diameter                 : Pipe inner diameter (ft)
- velocity                 : Mixture superficial velocity (ft/s)
- gasSupVel                : Superficial gas velocity (ft/s)
- liquidDensity            : Liquid density (lbm/ft3)
- gasDensity               : Gas density (lbm/ft3)
- liquidViscosity          : Liquid viscosity (cP)
- gasViscosity             : Gas viscosity (cP)
- surfaceTension           : Surface tension (code units)
- roughness                : Relative roughness (dimensionless)
- mixtureDensity           : No-slip mixture density (lbm/ft3)
- slugMixtureDensity       : Average mixture density in the slug region
                             (lbm/ft3)
- bubbleLength             : Bubble length (ft)
- slugLength               : Slug length (ft)
- transitionToMistBoundary : X-coordinate of the slug-to-mist transition
                             boundary
- flowMapCoordinateX       : X-coordinate used in the flow-pattern map
                             (dimensionless)
- slugToTransitionBoundary : X-coordinate of the slug-to-transition boundary
- reynolds                 : Calculated Reynolds number (returned by reference)
- flowPattern              : Flow-pattern indicator
                             (1 = liquid, 2 = gas, 3 = bubble,
                              4 = slug, 5 = mist, 6 = transition)

Output parameters:
- frictionGrad             : Frictional pressure gradient (psi/ft)
- reynolds                 : Calculated Reynolds number (returned by reference)

Main local variables:
- frictionFactorValue      : Darcy-Moody friction factor (dimensionless)
- reynoldsGas              : Gas Reynolds number, when applicable
- mistRoughnessParameter   : Auxiliary parameter used to calculate the
                             effective roughness for mist flow
- effectiveRoughness       : Effective relative roughness used to calculate
                             the friction factor (dimensionless)
- slugFrictionTerm         : Weighted-average frictional pressure gradient
                             for slug flow (psi/ft)
- mistFrictionTerm         : Weighted-average frictional pressure gradient
                             for mist flow (psi/ft)
==============================================================================
*/
void azizFriction(double diameter, double velocity, double gasSupVel,
                  double liquidDensity, double gasDensity, double liquidViscosity,
                  double gasViscosity, double surfaceTension, double roughness,
                  double mixtureDensity, double slugMixtureDensity,
                  double bubbleLength, double slugLength,
                  double transitionToMistBoundary, double flowMapCoordinateX, double slugToTransitionBoundary,
                  double& reynolds, unsigned char flowPattern,
                  double& frictionGrad) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_BUBBLE = 3;
    static constexpr unsigned char FLOW_SLUG = 4;
    static constexpr unsigned char FLOW_MIST = 5;
    static constexpr unsigned char FLOW_TRANSITION = 6;
    
    // Physical Constants
    static constexpr double REYNOLDS_SCALE = 1488.0;         // Reynolds number scaling factor
    static constexpr double GRAVITY_ACCEL = 32.174;          // ft/s²
    static constexpr double PSI_CONVERSION = 144.0;          // in²/ft²
    
    // Mist flow roughness correlation constants
    static constexpr double MIST_ROUGHNESS_SCALE = 4.52e-7;
    static constexpr double MIST_ROUGHNESS_THRESHOLD = 0.005;
    static constexpr double MIST_ROUGHNESS_LOW_COEFF = 34.0;
    static constexpr double MIST_ROUGHNESS_HIGH_COEFF = 174.8;
    static constexpr double MIST_ROUGHNESS_HIGH_EXP = 0.302;
    static constexpr double MIST_ROUGHNESS_MIN = 0.001;
    static constexpr double MIST_ROUGHNESS_MAX = 0.5;
    
    // Tolerances
    static constexpr double ZERO_VELOCITY_TOLERANCE = 1e-15;
    static constexpr double ZERO_LENGTH_TOLERANCE = 1e-15;
    
    // Initialization
    frictionGrad = 0.0;
    reynolds = 0.0;
    
    // Liquid or Bubble Flow
    if (flowPattern == FLOW_LIQUID || flowPattern == FLOW_BUBBLE) {
        if (fabs(velocity) <= ZERO_VELOCITY_TOLERANCE) {
            return;
        }
        
        reynolds = REYNOLDS_SCALE * liquidDensity * diameter * velocity / liquidViscosity;
        
        double frictionFactorValue;
        frictionFactor(reynolds, roughness, frictionFactorValue);
        
        const double velocitySquared = velocity * velocity;
        frictionGrad = (0.5 * frictionFactorValue * velocitySquared * mixtureDensity) /
                      (g_in * diameter * PSI_CONVERSION);
        return;
    }
    
    // Slug or Transition Flow
    if (flowPattern == FLOW_SLUG || flowPattern == FLOW_TRANSITION) {
        // Slug component friction
        double slugFrictionGrad = 0.0;
        
        if (fabs(velocity) > ZERO_VELOCITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * liquidDensity * diameter * velocity / liquidViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double totalUnitLength = bubbleLength + slugLength;
            if (fabs(totalUnitLength) > ZERO_LENGTH_TOLERANCE) {
                const double velocitySquared = velocity * velocity;
                slugFrictionGrad = (0.5 * frictionFactorValue * velocitySquared *
                                  slugMixtureDensity * slugLength) /
                                  (g_in * diameter * totalUnitLength * PSI_CONVERSION);
            }
        }
        
        // Pure slug flow: return slug friction only
        if (flowPattern == FLOW_SLUG) {
            frictionGrad = slugFrictionGrad;
            return;
        }
        
        // Transition flow: blend slug and mist friction
        double mistFrictionGrad = 0.0;
        
        if (fabs(gasSupVel) > ZERO_VELOCITY_TOLERANCE) {
            const double reynoldsGas = REYNOLDS_SCALE * gasDensity * diameter * gasSupVel / gasViscosity;
            
            // Calculate effective roughness for mist flow
            // Note: Compute inline to match original precision exactly
            const double mistRoughnessParam = MIST_ROUGHNESS_SCALE *
                                             (gasSupVel * liquidViscosity / surfaceTension) *
                                             (gasSupVel * liquidViscosity / surfaceTension) *
                                             gasDensity / liquidDensity;
            
            double effectiveRoughness;
            const double denominator = gasDensity * diameter * gasSupVel * gasSupVel;
            
            if (mistRoughnessParam <= MIST_ROUGHNESS_THRESHOLD) {
                effectiveRoughness = MIST_ROUGHNESS_LOW_COEFF * surfaceTension / denominator;
            } else {
                effectiveRoughness = MIST_ROUGHNESS_HIGH_COEFF * surfaceTension *
                                   pow(mistRoughnessParam, MIST_ROUGHNESS_HIGH_EXP) / denominator;
            }
            
            // Enforce roughness bounds
            if (effectiveRoughness < MIST_ROUGHNESS_MIN) effectiveRoughness = MIST_ROUGHNESS_MIN;
            if (effectiveRoughness > MIST_ROUGHNESS_MAX) effectiveRoughness = MIST_ROUGHNESS_MAX;
            
            double frictionFactorValue;
            frictionFactor(reynoldsGas, effectiveRoughness, frictionFactorValue);
            
            mistFrictionGrad = 0.5 * frictionFactorValue * gasDensity * gasSupVel * gasSupVel /
                              (g_in * diameter * PSI_CONVERSION);
        }
        
        // Weighted average between slug and mist friction
        // Note: Divide each term separately to match original precision exactly
        const double transitionRange = transitionToMistBoundary - slugToTransitionBoundary;
        if (fabs(transitionRange) > ZERO_LENGTH_TOLERANCE) {
            frictionGrad = (transitionToMistBoundary - flowMapCoordinateX) * slugFrictionGrad / transitionRange
                         + (flowMapCoordinateX - slugToTransitionBoundary) * mistFrictionGrad / transitionRange;
        } else {
            frictionGrad = slugFrictionGrad;
        }
        
        return;
    }
    
    // Mist Flow
    if (flowPattern == FLOW_MIST) {
        if (fabs(gasSupVel) <= ZERO_VELOCITY_TOLERANCE) {
            return;
        }
        
        const double reynoldsGas = REYNOLDS_SCALE * gasDensity * diameter * gasSupVel / gasViscosity;
        
        // Calculate effective roughness for mist flow
        // Note: Compute inline to match original precision exactly
        const double mistRoughnessParam = MIST_ROUGHNESS_SCALE *
                                         (gasSupVel * liquidViscosity / surfaceTension) *
                                         (gasSupVel * liquidViscosity / surfaceTension) *
                                         gasDensity / liquidDensity;
        
        double effectiveRoughness;
        const double denominator = gasDensity * diameter * gasSupVel * gasSupVel;
        
        if (mistRoughnessParam <= MIST_ROUGHNESS_THRESHOLD) {
            effectiveRoughness = MIST_ROUGHNESS_LOW_COEFF * surfaceTension / denominator;
        } else {
            effectiveRoughness = MIST_ROUGHNESS_HIGH_COEFF * surfaceTension *
                               pow(mistRoughnessParam, MIST_ROUGHNESS_HIGH_EXP) / denominator;
        }
        
        // Enforce roughness bounds
        if (effectiveRoughness < MIST_ROUGHNESS_MIN) effectiveRoughness = MIST_ROUGHNESS_MIN;
        if (effectiveRoughness > MIST_ROUGHNESS_MAX) effectiveRoughness = MIST_ROUGHNESS_MAX;
        
        double frictionFactorValue;
        frictionFactor(reynoldsGas, effectiveRoughness, frictionFactorValue);
        
        frictionGrad = 0.5 * frictionFactorValue * gasDensity * gasSupVel * gasSupVel /
                      (g_in * diameter * PSI_CONVERSION);
        return;
    }
    
    // Single-Phase Gas Flow
    if (flowPattern == FLOW_GAS) {
        if (fabs(gasSupVel) <= ZERO_VELOCITY_TOLERANCE) {
            return;
        }
        
        const double reynoldsGas = REYNOLDS_SCALE * gasDensity * gasSupVel * diameter / gasViscosity;
        
        double frictionFactorValue;
        frictionFactor(reynoldsGas, roughness, frictionFactorValue);
        
        frictionGrad = 0.5 * frictionFactorValue * gasDensity * gasSupVel * gasSupVel /
                      (g_in * diameter * PSI_CONVERSION);
        return;
    }
}

/*
==============================================================================
aziz: Implements the main logic of the Aziz, Govier, and Fogarasi correlation.

Variable dictionary:
- angle            : Pipe inclination angle (degrees)
- diameter         : Pipe inner diameter (ft)
- roughness        : Relative roughness (dimensionless)
- pressure         : Pressure (psia)
- velocity         : Mixture superficial velocity (ft/s)
- liquidFraction   : No-slip liquid fraction (ENS)
- gasDensity       : Gas density (lbm/ft3)
- liquidDensity    : Liquid density (lbm/ft3)
- gasViscosity     : Gas viscosity (cP)
- liquidViscosity  : Liquid viscosity (cP)
- surfaceTension   : Gas-liquid surface tension
- holdup           : Calculated liquid holdup fraction
- frictionGrad     : Frictional pressure gradient
- gravityGrad      : Gravitational pressure gradient
- accelerationGrad : Accelerational pressure gradient
- totalGrad        : Total pressure gradient
- reynolds         : Calculated Reynolds number
- flowPattern      : Flow pattern
- criticalFlag     : Critical-flow indicator
==============================================================================
*/
void aziz(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity, double surfaceTension,
          double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad, double& totalGrad,
          double& reynolds, unsigned char& flowPattern, unsigned char& criticalFlag) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_MIST = 5;
    
    // Physical Constants
    static constexpr double SURFACE_TENSION_CONVERSION = 2.2046e-3;  // Convert dyn/cm to lbm/s²
    static constexpr double PSI_CONVERSION = 144.0;                  // in²/ft²
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    static constexpr double ACCEL_CRITICAL_LIMIT = 0.95;
    
    // Initialization
    criticalFlag = 0;
    
    // Pre-computed Values
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    
    // Convert surface tension to field units (dyn/cm → lbm/s²)
    const double surfaceTensionConverted = surfaceTension * SURFACE_TENSION_CONVERSION;
    
    // Superficial velocities
    const double liquidSupVel = velocity * liquidFraction;
    const double gasSupVel = velocity - liquidSupVel;
    
    // Flow Regime Determination
    double bubbleToSlugBoundary, slugToTransitionBoundary, transitionToMistBoundary;
    double flowMapCoordinateX, flowMapCoordinateY;
    
    azizRegimeMap(liquidSupVel, gasSupVel, liquidDensity, gasDensity,
                  surfaceTensionConverted, liquidFraction,
                  bubbleToSlugBoundary, slugToTransitionBoundary, transitionToMistBoundary,
                  flowMapCoordinateX, flowMapCoordinateY, flowPattern);
    
    // Holdup and Slip Density Calculation
    double bubbleLength, slugLength, slipDensity, slugLiquidDensity;
    
    azizDensity(gasSupVel, velocity, surfaceTensionConverted, liquidDensity, gasDensity,
                diameter, liquidViscosity,
                transitionToMistBoundary, flowMapCoordinateX, slugToTransitionBoundary, flowPattern,
                holdup, bubbleLength, slugLength, slipDensity, slugLiquidDensity);
    
    // Gravity Gradient
    gravityGrad = slipDensity * sinAngle / PSI_CONVERSION;
    
    // Friction Gradient
    azizFriction(diameter, velocity, gasSupVel, liquidDensity, gasDensity, liquidViscosity,
                 gasViscosity, surfaceTensionConverted, roughness, slipDensity, slugLiquidDensity,
                 bubbleLength, slugLength, transitionToMistBoundary, flowMapCoordinateX,
                 slugToTransitionBoundary,
                 reynolds, flowPattern, frictionGrad);
    
    // Acceleration Effects (Mist Flow Only)
    double accelParam = 0.0;
    
    if (flowPattern == FLOW_MIST) {
        accelParam = slipDensity * velocity * gasSupVel / (g_in * pressure * PSI_CONVERSION);
        
        if (accelParam > ACCEL_CRITICAL_LIMIT) {
            accelParam = ACCEL_CRITICAL_LIMIT;
            criticalFlag = 1;
        }
    }
    
    // Total Gradient
    totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParam);
    accelGrad = accelParam * totalGrad;
}

/*
==============================================================================
olie: Calculates pressure gradients using the Oliemans correlation with a
      slip correction based on the Dukler correlation.

References:
     1.  Oliemans, R. V. A.: "Two-Phase Flow in Gas-Transmission Pipelines,"
         ASME Paper 76-Pet-25, presented at the ASME Petroleum Division
         Conference, Mexico City (Sept. 1976).
     2.  Oliemans, R. V. A. et al.: "Modelling of Annular Dispersed Two-Phase
         Flow in Vertical Pipes," International Journal of Multiphase Flow,
         Vol. 12, No. 5 (1986), 711-732.

Variable dictionary:
- angle                 : Pipe inclination angle (degrees)
- diameter              : Pipe inner diameter (inches)
- roughness             : Relative roughness (dimensionless)
- pressure              : Absolute pressure (psia)
- velocity              : Mixture superficial velocity (ft/s)
- liquidFraction        : No-slip liquid volumetric fraction (0-1)
- gasDensity            : Gas density (lb/ft3)
- liquidDensity         : Liquid density (lb/ft3)
- gasViscosity          : Gas viscosity (cP)
- liquidViscosity       : Liquid viscosity (cP)
- surfaceTension        : Surface tension (dyn/cm)
- temperature           : Temperature (degrees F)
- compressibilityFactor : Gas compressibility factor (dimensionless)
- holdup                : Calculated liquid holdup (output)
- frictionGrad          : Frictional pressure gradient (psi/ft) - output
- gravityGrad           : Gravitational pressure gradient (psi/ft) - output
- totalGrad             : Total pressure gradient (psi/ft) - output
- reynolds              : Two-phase Reynolds number (output)
- flowPattern           : Flow-pattern indicator (output)
- correlationFlag       : Holdup correlation selection flag
                          (0 = Dukler, 1 = Eaton,
                           2 = Minami I, 3 = Minami II)
- criticalFlag          : Critical-flow indicator (output)

Auxiliary variables:
- area                  : Pipe cross-sectional area (in2)
- liquidSupVel          : Superficial liquid velocity (ft/s)
- gasSupVel             : Superficial gas velocity (ft/s)
- massFlowRate          : Total mass flow rate (lbm/s)
- betaL                 : Slip parameter (holdup - liquidFraction)
- slipDensity           : Slip-corrected density (lb/ft3)
- massFlux              : Slip-corrected mass flux (lbm/(s*ft2))
- effectiveDiameter     : Slip-corrected effective diameter (inches)
- noSlipDensity         : Slip-corrected density (lb/ft3)
- noSlipViscosity       : Slip-corrected viscosity (cP)
- reynoldsTp            : Two-phase Reynolds number (dimensionless)
- effectiveRoughness    : Effective relative roughness (dimensionless)
==============================================================================
*/
void olie(double angle, double diameter, double roughness, double pressure, double velocity, double liquidFraction,
          double gasDensity, double liquidDensity, double gasViscosity, double liquidViscosity,
          double surfaceTension, double temperature, double compressibilityFactor,
          double& holdup, double& frictionGrad, double& gravityGrad,
          double& totalGrad, double& reynolds, unsigned char& flowPattern, unsigned char correlationFlag,
          unsigned char& criticalFlag) {
    
    // Physical Constants
    static constexpr double REYNOLDS_SCALE = 1488.0;      // Reynolds number scaling factor
    static constexpr double ZERO_REYNOLDS_TOLERANCE = 1e-15;  // Near-zero Reynolds cutoff
    
    // Step 1: Pre-computed Geometric and Flow Properties
    
    criticalFlag = 0;
    
    // Cross-sectional area
    const double area = 0.25 * M_PI * diameter * diameter;
    
    // Superficial velocities
    const double liquidSupVel = velocity * liquidFraction;
    const double gasSupVel = velocity - liquidSupVel;
    
    // Total mass flow rate
    const double massFlowRate = (liquidSupVel * liquidDensity + gasSupVel * gasDensity) * area;
    
    // Step 2: Baseline Calculation Using Dukler Correlation
    // Dukler provides initial holdup, gravity gradient, and flow pattern
    
    unsigned char transitionFlag = 0;
    double accelGrad;
    
    duklerCorrelation(angle, diameter, roughness, pressure, velocity, liquidFraction,
                     gasDensity, liquidDensity, gasViscosity, liquidViscosity,
                     surfaceTension, temperature, compressibilityFactor,
                     holdup, frictionGrad, gravityGrad, accelGrad,
                     totalGrad, reynolds, flowPattern, correlationFlag,
                     transitionFlag, criticalFlag);
    
    // Step 3: Oliemans Slip Correction Parameters
    // Calculate slip parameter and corrected properties
    
    const double betaL = holdup - liquidFraction;
    const double slipDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
    const double massFlux = massFlowRate / (1.0 - betaL) / area;
    const double effectiveDiameter = diameter * std::sqrt(1.0 - betaL);
    const double noSlipDensity = (liquidDensity * liquidFraction + gasDensity * (1.0 - holdup)) / (1.0 - betaL);
    const double noSlipViscosity = (liquidViscosity * liquidFraction + gasViscosity * (1.0 - holdup)) / (1.0 - betaL);
    
    // Step 4: Two-Phase Reynolds Number
    
    const double reynoldsTp = REYNOLDS_SCALE * diameter * massFlowRate /
                             (area * noSlipViscosity * std::sqrt(1.0 - betaL));
    
    // Step 5: Effective Roughness
    
    const double effectiveRoughness = roughness * diameter / effectiveDiameter;
    
    // Step 6: Friction Gradient Calculation
    
    if (fabs(reynoldsTp) > ZERO_REYNOLDS_TOLERANCE) {
        // Moody friction factor with corrected Reynolds and roughness
        double frictionFactorValue;
        frictionFactor(reynoldsTp, effectiveRoughness, frictionFactorValue);
        
        // Oliemans friction gradient formula
        frictionGrad = (frictionFactorValue * massFlux * massFlux) /
                      (2.0 * 144.0 * effectiveDiameter * noSlipDensity * g_in);
    } else {
        frictionGrad = 0.0;
    }
    
    // Step 7: Total Gradient
    
    totalGrad = frictionGrad + gravityGrad;
}

/*
==============================================================================
gray: Gray correlation for gas wells with liquid production.

References:
      1. Gray, H. E.: "Vertical Flow Correlation in Gas Wells,"
         User's Manual for API 14B, Subsurface Controlled Safety Valve
         Sizing Computer Program, Appendix B (June 1974).

Variable dictionary:
- angle              : Flow angle relative to the horizontal (degrees)
- diameter           : Pipe inner diameter (inches)
- roughness          : Pipe relative roughness (dimensionless)
- pressure           : Absolute pressure (psia)
- velocity           : Mixture superficial velocity (ft/s)
- liquidFraction     : No-slip liquid holdup fraction (dimensionless, 0-1)
- waterFraction      : Water mass fraction in the liquid phase
                       (dimensionless, 0-1)
- gasDensity         : Gas density (lb/ft3)
- liquidDensity      : Liquid density (lb/ft3)
- gasViscosity       : Gas viscosity (cP)
- liquidViscosity    : Liquid viscosity (cP)
- oilSurfaceTension  : Oil surface tension (dyn/cm)
- waterSurfaceTension: Water surface tension (dyn/cm)
- holdup             : Calculated liquid holdup fraction
                       (dimensionless, 0-1). Output.
- frictionGrad       : Frictional pressure gradient (psi/ft). Output.
- gravityGrad        : Gravitational pressure gradient (psi/ft). Output.
- accelerationGrad   : Accelerational pressure gradient (psi/ft). Output.
- totalGrad          : Total pressure gradient (psi/ft). Output.
- reynolds           : Reynolds number (dimensionless). Output.
- flowPattern        : Flow-pattern indicator. Output.
                       1 = single-phase liquid, 2 = single-phase gas,
                       3 = two-phase flow
- criticalFlag       : Critical-flow indicator. Output.
                       0 if accelParam <= 0.95,
                       1 if accelParam > 0.95

Internal variables:
- angleRad           : Flow angle relative to the horizontal (radians)
- liquidSupVel       : Superficial liquid velocity (ft/s)
- gasSupVel          : Superficial gas velocity (ft/s)
- noSlipDensity      : No-slip mixture density (lb/ft3)
- slipDensity        : Mixture density accounting for slip (lb/ft3)
- liquidSurfTension  : Liquid surface tension (dyn/cm)
- effectiveRoughness : Effective roughness for two-phase flow (dimensionless)
- velocityNumber     : Dimensionless velocity number (Nv)
- pipeNumber         : Dimensionless pipe number (Np)
- liquidGasRatio     : Liquid-to-gas velocity ratio (R)
- exponentB          : Exponent B used by the holdup correlation
- pseudoRoughness    : Surface-tension-based pseudo-roughness (ft)
- transitionRoughness: Calculated transition roughness (ft)
- effectiveViscosity : Effective mixture viscosity (cP)
- frictionFactor     : Moody friction factor (dimensionless)
- accelParam         : Kinetic-energy/acceleration term (dimensionless)
==============================================================================
*/

/*
==============================================================================
gray: Calculates pressure gradients for two-phase flow using the Gray
      correlation.

References:
      1.  Gray, H. E.: "Vertical Flow Correlation in Gas Wells,"
          User's Manual for API 14B, Subsurface Controlled Safety Valve
          Sizing Computer Program, Appendix B (June 1974).

Variable dictionary:
- angle               : Pipe inclination angle (degrees)
- diameter            : Pipe inner diameter (inches)
- roughness           : Pipe relative roughness (dimensionless)
- pressure            : Pressure (psia)
- velocity            : Mixture superficial velocity (ft/s)
- liquidFraction      : Liquid volumetric fraction
- waterFraction       : Water mass fraction in the liquid phase
- gasDensity          : Gas density (lb/ft3)
- liquidDensity       : Liquid density (lb/ft3)
- gasViscosity        : Gas viscosity (cP)
- liquidViscosity     : Liquid viscosity (cP)
- oilSurfaceTension   : Oil-gas surface tension (dyn/cm)
- waterSurfaceTension : Water-gas surface tension (dyn/cm)
- holdup              : Calculated liquid holdup
- frictionGrad        : Frictional pressure gradient (psi/ft)
- gravityGrad         : Gravitational pressure gradient (psi/ft)
- accelGrad           : Accelerational pressure gradient (psi/ft)
- totalGrad           : Total pressure gradient (psi/ft)
- reynolds            : Calculated Reynolds number
- flowPattern         : Flow-pattern indicator
                        (1 = liquid, 2 = gas, 3 = two-phase)
- criticalFlag        : Critical-flow indicator
==============================================================================
*/
void gray(double angle, double diameter, double roughness, double pressure, double velocity,
          double liquidFraction, double waterFraction, double gasDensity, double liquidDensity,
          double gasViscosity, double liquidViscosity, double oilSurfaceTension, double waterSurfaceTension,
          double& holdup, double& frictionGrad, double& gravityGrad, double& accelGrad,
          double& totalGrad, double& reynolds, unsigned char& flowPattern, unsigned char& criticalFlag) {
    
    // Named Flow Pattern Constants
    static constexpr unsigned char FLOW_LIQUID = 1;
    static constexpr unsigned char FLOW_GAS = 2;
    static constexpr unsigned char FLOW_TWOPHASE = 3;
    
    // Physical Constants
    static constexpr double REYNOLDS_SCALE = 1488.0;        // Reynolds number scaling factor
    static constexpr double PSI_CONVERSION = 144.0;         // in²/ft²
    static constexpr double DEG_TO_RAD = M_PI / 180.0;
    
    // Surface tension blending coefficients
    static constexpr double WATER_TENSION_COEFF = 0.617;    // Water surface tension coefficient
    static constexpr double WATER_FRACTION_DENOM = 0.383;   // Water fraction denominator correction
    
    // Gray correlation dimensionless number coefficients
    static constexpr double VELOCITY_NUMBER_COEFF = 1.4093264e+01;   // Velocity number coefficient
    static constexpr double PIPE_NUMBER_COEFF = 1.4598885e+04;       // Pipe number coefficient
    
    // Holdup correlation parameters
    static constexpr double HOLDUP_BASE_EXPONENT = -2.314;           // Base exponential coefficient
    static constexpr double EXPONENT_B_SCALE = 0.0814;               // Exponent B scaling factor
    static constexpr double EXPONENT_B_LOG_COEFF = 0.0554;           // Log term coefficient
    static constexpr double EXPONENT_B_LOG_SCALE = 730.0;            // Log scale factor
    static constexpr double PIPE_NUMBER_CORRECTION = 205.0;          // Pipe number correction
    
    // Pseudo-roughness calculation
    static constexpr double PSEUDO_ROUGHNESS_COEFF = 6.283175e-02;   // 2*pi/100, conversion factor
    static constexpr double LIQUID_GAS_RATIO_THRESHOLD = 0.007;      // L/G ratio transition threshold
    static constexpr double MIN_ROUGHNESS_LIMIT = 2.308e-06;         // Minimum roughness (ft)
    
    // Acceleration parameter limit
    static constexpr double MAX_ACCEL_PARAMETER = 0.95;              // Critical flow acceleration limit
    
    // Tolerances
    static constexpr double ZERO_VELOCITY_TOLERANCE = 1e-15;
    static constexpr double ZERO_VISCOSITY_TOLERANCE = 1e-15;
    
    // Initialization
    criticalFlag = 0;
    flowPattern = FLOW_TWOPHASE;
    
    // Pre-computed Values
    const double angleRad = angle * DEG_TO_RAD;
    const double sinAngle = sin(angleRad);
    
    // Superficial velocities
    const double liquidSuperficialVel = velocity * liquidFraction;
    const double gasSuperficialVel = velocity - liquidSuperficialVel;
    
    // No-slip mixture density
    double noSlipDensity = liquidDensity * liquidFraction + gasDensity * (1.0 - liquidFraction);
    
    // Liquid Surface Tension Calculation
    // Blends oil and water surface tensions based on water fraction
    // Accounts for non-ideal mixing behavior
    const double liquidSurfaceTension = (oilSurfaceTension * (1.0 - waterFraction) +
                                        WATER_TENSION_COEFF * waterSurfaceTension * waterFraction) /
                                       (1.0 - WATER_FRACTION_DENOM * waterFraction);
    
    // Single-Phase Flow Handling
    if (liquidFraction >= 1.0) {
        // Single-phase liquid flow
        flowPattern = FLOW_LIQUID;
        holdup = 1.0;
        noSlipDensity = liquidDensity;
        
        const double slipDensity = liquidDensity;
        
        if (fabs(velocity) > ZERO_VELOCITY_TOLERANCE &&
            fabs(liquidViscosity) > ZERO_VISCOSITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * noSlipDensity * velocity * diameter / liquidViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = velocity * velocity;
            frictionGrad = (frictionFactorValue * noSlipDensity * velocitySquared) /
                          (2.0 * g_in * diameter * PSI_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = slipDensity * sinAngle / PSI_CONVERSION;
        accelGrad = 0.0;
        totalGrad = frictionGrad + gravityGrad;
        return;
    }
    
    if (liquidFraction <= 0.0) {
        // Single-phase gas flow
        flowPattern = FLOW_GAS;
        holdup = 0.0;
        noSlipDensity = gasDensity;
        
        const double slipDensity = gasDensity;
        
        if (fabs(velocity) > ZERO_VELOCITY_TOLERANCE &&
            fabs(gasViscosity) > ZERO_VISCOSITY_TOLERANCE) {
            reynolds = REYNOLDS_SCALE * noSlipDensity * velocity * diameter / gasViscosity;
            
            double frictionFactorValue;
            frictionFactor(reynolds, roughness, frictionFactorValue);
            
            const double velocitySquared = velocity * velocity;
            frictionGrad = (frictionFactorValue * noSlipDensity * velocitySquared) /
                          (2.0 * g_in * diameter * PSI_CONVERSION);
        } else {
            reynolds = 0.0;
            frictionGrad = 0.0;
        }
        
        gravityGrad = slipDensity * sinAngle / PSI_CONVERSION;
        
        // Gas flow acceleration effect
        double accelParameter = 0.0;
        if (fabs(pressure) > ZERO_VELOCITY_TOLERANCE) {
            accelParameter = slipDensity * velocity * gasSuperficialVel /
                           (g_in * pressure * PSI_CONVERSION);
            
            if (accelParameter > MAX_ACCEL_PARAMETER) {
                accelParameter = MAX_ACCEL_PARAMETER;
                criticalFlag = 1;
            }
        }
        
        totalGrad = (frictionGrad + gravityGrad) / (1.0 - accelParameter);
        accelGrad = totalGrad * accelParameter;
        return;
    }
    
    // Two-Phase Flow: Gray Correlation
    flowPattern = FLOW_TWOPHASE;
    
    // Dimensionless Numbers
    const double densityDifference = liquidDensity - gasDensity;
    const double velocitySquared = velocity * velocity;
    
    // Gray velocity number: characterizes flow inertia relative to buoyancy
    const double velocityNumber = VELOCITY_NUMBER_COEFF *
                                 (noSlipDensity * velocitySquared * noSlipDensity * velocitySquared) /
                                 (densityDifference * liquidSurfaceTension);
    
    // Gray pipe number: characterizes pipe geometry relative to surface tension
    const double pipeNumber = PIPE_NUMBER_COEFF * densityDifference *
                             diameter * diameter / liquidSurfaceTension;
    
    // Liquid-Gas Ratio and Holdup Calculation
    double liquidGasRatio;
    
    if (fabs(gasSuperficialVel) > ZERO_VELOCITY_TOLERANCE) {
        liquidGasRatio = liquidSuperficialVel / gasSuperficialVel;
        
        // Exponent B: empirical correlation based on liquid-gas ratio
        const double logTerm = log(1.0 + EXPONENT_B_LOG_SCALE * liquidGasRatio /
                                  (1.0 + liquidGasRatio));
        const double exponentB = EXPONENT_B_SCALE * (1.0 - EXPONENT_B_LOG_COEFF * logTerm);
        
        // Holdup correlation: exponential decay with modified velocity number
        const double modifiedVelocityNumber = velocityNumber * (1.0 + PIPE_NUMBER_CORRECTION / pipeNumber);
        const double exponentialTerm = exp(HOLDUP_BASE_EXPONENT * pow(modifiedVelocityNumber, exponentB));
        
        holdup = 1.0 - (1.0 - exponentialTerm) / (1.0 + liquidGasRatio);
        
    } else {
        // No gas flow: liquid-only with no-slip holdup
        liquidGasRatio = 0.0;
        holdup = liquidFraction;
    }
    
    // Effective Roughness Calculation
    // Two-phase flow roughness depends on surface tension effects
    
    double pseudoRoughness;
    if (fabs(velocity) > ZERO_VELOCITY_TOLERANCE) {
        // Pseudo-roughness: characteristic length scale based on surface tension
        pseudoRoughness = PSEUDO_ROUGHNESS_COEFF * liquidSurfaceTension /
                         (noSlipDensity * velocitySquared);
    } else {
        pseudoRoughness = roughness * diameter;
    }
    
    // Transition between physical and pseudo-roughness
    double transitionRoughness;
    const double physicalRoughness = roughness * diameter;
    
    if (liquidGasRatio < LIQUID_GAS_RATIO_THRESHOLD) {
        // Linear interpolation for low liquid-gas ratios
        const double weight = liquidGasRatio / LIQUID_GAS_RATIO_THRESHOLD;
        transitionRoughness = physicalRoughness + weight * (pseudoRoughness - physicalRoughness);
    } else {
        // Use pseudo-roughness for higher liquid-gas ratios
        transitionRoughness = pseudoRoughness;
    }
    
    // Enforce minimum roughness limit
    if (transitionRoughness < MIN_ROUGHNESS_LIMIT) {
        transitionRoughness = MIN_ROUGHNESS_LIMIT;
    }
    
    const double effectiveRoughness = transitionRoughness / diameter;
    
    // Mixture Properties
    const double slipDensity = liquidDensity * holdup + gasDensity * (1.0 - holdup);
    const double effectiveViscosity = liquidViscosity * holdup + gasViscosity * (1.0 - holdup);
    
    // Pressure Gradient Calculations
    
    // Friction gradient
    if (fabs(effectiveViscosity) > ZERO_VISCOSITY_TOLERANCE &&
        fabs(velocity) > ZERO_VELOCITY_TOLERANCE) {
        
        reynolds = REYNOLDS_SCALE * noSlipDensity * velocity * diameter / effectiveViscosity;
        
        double frictionFactorValue;
        frictionFactor(reynolds, effectiveRoughness, frictionFactorValue);
        
        frictionGrad = (frictionFactorValue * noSlipDensity * velocitySquared) /
                      (2.0 * g_in * diameter * PSI_CONVERSION);
    } else {
        reynolds = 0.0;
        frictionGrad = 0.0;
    }
    
    // Gravity gradient
    gravityGrad = slipDensity * sinAngle / PSI_CONVERSION;
    
    // Acceleration gradient (kinetic energy change)
    double accelParameter = slipDensity * velocity * gasSuperficialVel /
                           (g_in * pressure * PSI_CONVERSION);
    
    // Check for critical flow condition
    if (accelParameter > MAX_ACCEL_PARAMETER) {
        accelParameter = MAX_ACCEL_PARAMETER;
        criticalFlag = 1;
    }
    
    // Total gradient
    totalGrad = (gravityGrad + frictionGrad) / (1.0 - accelParameter);
    accelGrad = totalGrad * accelParameter;
}