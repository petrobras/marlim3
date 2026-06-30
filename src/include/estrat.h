#ifndef _ESTRAT
#define _ESTRAT
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

/*!
 * Model stratified gas-liquid flow in a circular pipe.
 *
 * The class stores the phase geometry, wetted perimeters, hydraulic
 * diameters, Reynolds numbers, friction factors, wall and interfacial
 * shear stresses, phase properties, flow velocities, and drift-flux
 * parameters used by the stratified-flow correlations.
 *
 * It also provides nonlinear solvers for film height, equilibrium
 * conditions, void fraction, distribution coefficient, and transition
 * drift velocity.
 */
class estratificado {
  public:
    double dia;           // Pipe internal diameter.
    double hfilm;         // Liquid-film height.
    double holliq;        // Liquid holdup.
    double twg;           // Gas-wall shear stress.
    double twl;           // Liquid-wall shear stress.
    double ti;            // Gas-liquid interfacial shear stress.
    double fg;            // Gas friction factor.
    double fl;            // Liquid friction factor.
    double fi;            // Interfacial friction factor.
    double reyL;          // Liquid Reynolds number.
    double reyG;          // Gas Reynolds number.
    double alNd;          // Dimensionless liquid cross-sectional area.
    double agNd;          // Dimensionless gas cross-sectional area.
    double swlNd;         // Dimensionless liquid wetted perimeter.
    double swgNd;         // Dimensionless gas wetted perimeter.
    double siNd;          // Dimensionless gas-liquid interfacial perimeter.
    double uls;           // Superficial liquid velocity.
    double ugs;           // Superficial gas velocity.
    double ang;           // Pipe inclination angle.
    double rl;            // Liquid density.
    double rg;            // Gas density.
    double mil;           // Liquid dynamic viscosity.
    double mig;           // Gas dynamic viscosity.
    double velref;        // Reference velocity.
    double dFdUl;         // Derivative of the equilibrium function with respect to liquid velocity.
    double dFdUg;         // Derivative of the equilibrium function with respect to gas velocity.
    double dFdAl;         // Derivative of the equilibrium function with respect to liquid area or holdup.
    double cine;          // Kinematic contribution or coefficient.
    double dina;          // Dynamic contribution or coefficient.
    double coefC0;        // Drift-flux distribution coefficient.
    double valUd;         // Drift velocity.
    int arr;              // Flow-pattern identifier.
    double multTrans;     // Transition multiplier.
    double fatorperdaLiq; // Liquid-phase pressure-loss factor.
    double fatorperdaGas; // Gas-phase pressure-loss factor.

    /*!
     * Construct the stratified-flow model.
     */
    estratificado(double vdia = 1 * 2.54 / 100., double Ql = 0., double Qg = 0.,
                  double vrl = 10000., double vrg = 1.,
                  double vmil = 1. / 1000., double vmig = 1. / 10000,
                  double hol = 0.5, double vang = 0., double eps = 0.); // Constructor.

    //! Copy constructor.
    estratificado(const estratificado &);

    //! Destructor.
    ~estratificado() {}

    //! Copy-assignment operator.
    estratificado &operator=(const estratificado &);

    /*!
     * Calculate the liquid cross-sectional area.
     * \param var Dimensionless interface-position variable.
     * \param dia Pipe diameter.
     * \return Liquid cross-sectional area.
     */
    double al(const double var, const double dia) {
        return 0.25 * dia * dia * (M_PI - acos(var) + var * sqrt(1 - var * var));
    }

    /*!
     * Calculate the gas cross-sectional area.
     * \param var Dimensionless interface-position variable.
     * \param dia Pipe diameter.
     * \return Gas cross-sectional area.
     */
    double ag(const double var, const double dia) {
        return 0.25 * dia * dia * (acos(var) - var * sqrt(1 - var * var));
    }

    /*!
     * Calculate the liquid wetted perimeter.
     */
    double sl(const double var, const double dia) {
        double peri;
        peri = dia * (M_PI - acos(var));
        if (peri < 4. * 1e-20)
            peri = 4. * 1e-20;
        return peri;
    }

    /*!
     * Calculate the gas wetted perimeter.
     */
    double sg(const double var, const double dia) {
        double peri;
        peri = dia * acos(var);
        if (peri < 4. * 1e-20)
            peri = 4. * 1e-20;
        return peri;
    }

    /*!
     * Calculate the gas-liquid interfacial perimeter.
     */
    double si(const double var, const double dia) {
        double peri;
        peri = dia * sqrt(1 - var * var);
        if (peri < 4. * 1e-20)
            peri = 4. * 1e-20;
        return peri;
    }

    /*!
     * Evaluate the nonlinear equation relating film height and liquid holdup.
     */
    double fraiz(const double hfilm, const double hol,
                 const double dia) {
        double var = 2. * hfilm / dia - 1.;
        return 4. * al(var, dia) / (M_PI * dia * dia) - hol;
    }

    /*!
     * Calculate the liquid-film height using Newton's method.
     *
     * \param x1 Initial film-height estimate.
     * \param hol Liquid holdup.
     * \param dia Pipe diameter.
     * \param tol Function-value tolerance.
     * \param epsn Increment tolerance.
     * \param maxit Maximum number of iterations.
     * \return Calculated liquid-film height.
     */
    double fhfilm(double x1,
                  const double hol,
                  const double dia,
                  double tol = 0.01,
                  double epsn = 0.01,
                  int maxit = 400) {
        if (hol < 0.9999) {
            double rtn = x1;
            for (int j = 0; j < maxit; j++) {
                double f = fraiz(rtn, hol, dia);
                double df = 4. * si(rtn, dia) / (M_PI * dia * dia);
                double dx = f / df;
                rtn -= dx;
                if (fabs(dx) < epsn || fabs(f) < tol || rtn < dia / 50)
                    return rtn;
            }
            throw("M�ximo n�mero de intera��es em Newton");
        } else {
            return hol * dia;
        }
    }

    //! Calculate the liquid hydraulic diameter.
    double dhl(double var, double dia) {
        return 4. * alNd * dia * dia / (swlNd * dia);
    }

    //! Calculate the gas hydraulic diameter.
    double dhg(double var, double dia) {
        return 4. * agNd * dia * dia / (swgNd * dia);
    }

    //! Calculate the liquid Reynolds number.
    double reyl(double var, double dia,
                double Ql) {
        if (alNd < 1e-20)
            alNd = 1e-20;
        return dhl(var, dia) * rl * (fabs(Ql) / (alNd * dia * dia)) / mil;
    }

    //! Calculate the gas Reynolds number.
    double reyg(double var, double dia,
                double Qg) {
        if (agNd < 1e-20) {
            agNd = 1e-20;
        }
        return dhg(var, dia) * rg * (fabs(Qg) / (agNd * dia * dia)) / mig;
    }

    //! Calculate the liquid friction factor.
    double fricl(double dia,
                 double Ql, double eps);

    //! Calculate the gas friction factor.
    double fricg(double dia,
                 double Qg, double eps);

    //! Calculate the liquid-wall shear stress.
    double tall(double vell) {
        return 0.5 * fl * rl * vell * fabs(vell);
    }

    //! Calculate the gas-wall shear stress.
    double talg(double velg) {
        return 0.5 * fg * rg * velg * fabs(velg);
    }

    //! Calculate the gas-liquid interfacial shear stress.
    double tali(double velg, double vell) {
        return 0.5 * fg * rg * (velg - vell) * fabs(velg - vell);
    }

    //! Calculate the gas momentum-source term.
    double fonteQMGas(double dia);

    //! Calculate the liquid momentum-source term.
    double fonteQMliquido(double dia);

    //! Evaluate the stratified-flow map for a specified diameter and void fraction.
    void mapa(double dia, double alf);

    //! Evaluate the secondary flow-map function.
    double mapa2(double chute);

    //! Evaluate the transient or time-dependent flow map.
    void mapaTD(double chute = -1.);

    //! Evaluate the phase-equilibrium equation.
    double equil(double dia);

    //! Evaluate the equilibrium equation using liquid and total flow rates.
    double equil2(double ql, double qt, double eps);

    //! Evaluate the equilibrium equation using holdup and phase flow rates.
    double equil3(double hol, double ql, double qg, double eps);

    //! Calculate the equilibrium liquid flow rate.
    double qlequil(double hol, double ql, double qg, double eps);

    //! Evaluate the stratified-flow stability or transition criterion.
    double criterio(double hol, double ql, double qg, double eps);

    //! Solve for the superficial liquid velocity using Brent's method.
    double zbrentUls(double x1, double x2, double ql, double qg, double eps, double tol = 0.00000000001,
                     double epsn = 0.00000000001, int maxit = 4000);

    //! Determine the void fraction between two holdup bounds.
    double findalf(double hol1, double hol2, double ql, double qg, double eps);

    //! Calculate the drift-flux distribution coefficient.
    double calcC0(double x1, const double qt,
                  double alf, double eps, double tol = 1.0,
                  double epsn = 0.000000001, int maxit = 800);

    //! Solve the distribution-coefficient equation using the false-position method.
    double FalsaCorda(double a, double b,
                      const double qt,
                      double alf, double eps,
                      double delta = 0.00001,
                      double epsn = 0.00001, int maxit = 2000);

    //! Return a value with the magnitude of a and the sign of b.
    double SIGN(double a, double b) { return (b >= 0 ? 1.0 : -1.0) * fabs(a); }

    // Check whether a value with the magnitude of a and the sign of b differs from a;
    // see the Numerical Recipes implementation.
    double zbrent(double x1, double x2, double ql, double qg,
                  double eps, double tol = 0.001,
                  double epsn = 0.001, int maxit = 100);

    //! Calculate the distribution coefficient using the false-position formulation.
    double calcC0FC(const double x1, const double qt,
                    double alf, double eps, double coaux);

    //! Calculate the transient drift velocity.
    double CalcUdTrans(double alfc, double alf0,
                       double alf1, double Ql0, double Ql1,
                       double Qg0, double Qg1,
                       double pL, double pR,
                       double c0aux, double dt, double dx, int verifica);
};

#endif