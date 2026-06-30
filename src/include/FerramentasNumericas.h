#ifndef _FNUM
#define _FNUM
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "Log.h"
#include "Matriz.h"
#include "Vetor.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>
#include <stdlib.h>

using namespace std;

// Application logger used by the numerical utilities.
extern Logger logger;

/*!
 * Report an error raised by a numerical routine.
 *
 * \param t Error message.
 */
void NumError(const string &t);

/*!
 * Calculate the natural logarithm of the gamma function.
 *
 * \tparam T Numeric type.
 * \param xx Function argument; must be greater than zero.
 * \return Value of ln(Gamma(xx)).
 */
template <class T>
T gammln(const T xx);

/*!
 * Interpolate tabulated data using Newton's polynomial.
 *
 * \tparam T Numeric type.
 * \return Interpolated value.
 */
template <class T>
T PoliNewton(const Vcr<T> &, const Vcr<T> &, T);

/*!
 * Perform one-dimensional linear interpolation.
 *
 * \tparam T Numeric type.
 * \return Interpolated value.
 */
template <class T>
T InterLinear(const Vcr<T> &, const Vcr<T> &, T);

/*!
 * Perform bilinear interpolation over a tabulated surface.
 *
 * \tparam T Numeric type.
 * \return Interpolated surface value.
 */
template <class T>
T InterLinear(const FullMtx<T> &, const Vcr<T> &, const Vcr<T> &, T, T);

/*!
 * Numerically integrate a function over a finite interval.
 *
 * \tparam T Numeric type.
 * \tparam F Function to integrate.
 * \param tipo Integration method: 1 for Euler, 2 for Simpson, and 3 for
 *             Gaussian quadrature.
 * \return Numerical approximation of the integral.
 */
template <class T, T F(T, const T *const)>
T NIntegrate(T, T, const T *const, int, int tipo = 3);

/*!
 * Calculate Gauss-Legendre quadrature nodes and weights.
 *
 * The weighting function is W(x) = 1 over the finite interval [x1, x2].
 * The resulting abscissas are stored in x and the corresponding weights in w.
 *
 * \tparam T Numeric type.
 * \param x1 Lower integration limit.
 * \param x2 Upper integration limit.
 * \param x Quadrature abscissas.
 * \param w Quadrature weights.
 */
template <class T>
void gauleg(const T x1, const T x2, Vcr<T> &x, Vcr<T> &w);

/*!
 * Calculate Gauss-Laguerre quadrature nodes and weights.
 *
 * The integration interval is [0, infinity), with weighting function
 * W(x) = x^alf * exp(-x). The smallest abscissa is stored in x[0] and the
 * largest in x[n - 1].
 *
 * \tparam T Numeric type.
 * \param x Quadrature abscissas.
 * \param w Quadrature weights.
 * \param alf Exponent of the weighting function.
 */
template <class T>
void gaulag(Vcr<T> &x, Vcr<T> &w, const T alf);

/*!
 * Calculate Gauss-Hermite quadrature nodes and weights.
 *
 * The integration interval is (-infinity, infinity), with weighting function
 * W(x) = exp(-x^2). The largest abscissa is stored in x[0] and the most
 * negative abscissa in x[n - 1].
 *
 * \tparam T Numeric type.
 * \param x Quadrature abscissas.
 * \param w Quadrature weights.
 */
template <class T>
void gauher(Vcr<T> &x, Vcr<T> &w);

/*!
 * Calculate Gauss-Jacobi quadrature nodes and weights.
 *
 * The weighting function is
 * W(x) = (1 - x)^alf * (1 + x)^bet over [x1, x2].
 * Legendre quadrature is obtained with alf = 0 and bet = 0, while a
 * Chebyshev special case is obtained with alf = bet = -1/2.
 *
 * \tparam T Numeric type.
 * \param x1 Lower integration limit.
 * \param x2 Upper integration limit.
 * \param x Quadrature abscissas.
 * \param w Quadrature weights.
 * \param alf Exponent associated with (1 - x).
 * \param bet Exponent associated with (1 + x).
 */
template <class T>
void gaujac(const T x1, const T x2, Vcr<T> &x, Vcr<T> &w, const T alf, const T bet);

/*!
 * Integrate a function using Gauss-Legendre quadrature.
 *
 * \tparam T Numeric type.
 * \tparam F Function to integrate.
 * \param x1 Lower integration limit.
 * \param x2 Upper integration limit.
 * \param n Number of quadrature points.
 * \return Numerical approximation of the integral.
 */
template <class T, T F(T, const T *const)>
T quadleg(const T x1, const T x2, const T *const, const int n);

/*!
 * Integrate a function using Gauss-Jacobi quadrature.
 *
 * \tparam T Numeric type.
 * \tparam F Function to integrate.
 * \param x1 Lower integration limit.
 * \param x2 Upper integration limit.
 * \param n Number of quadrature points.
 * \param alf Exponent associated with (1 - x).
 * \param bet Exponent associated with (1 + x).
 * \return Numerical approximation of the integral.
 */
template <class T, T F(T, const T *const)>
T quadjac(const T x1, const T x2, const T *const, const int n, const T alf, const T bet);

/*!
 * Integrate a function using Gauss-Laguerre quadrature.
 *
 * The integration interval is [0, infinity), with weighting function
 * W(x) = x^alf * exp(-x).
 *
 * \tparam T Numeric type.
 * \tparam F Function to integrate.
 * \param n Number of quadrature points.
 * \param alf Exponent of the weighting function.
 * \return Numerical approximation of the integral.
 */
template <class T, T F(T, const T *const)>
T quadlag(const T *const, const int n, const T alf);

/*!
 * Integrate a function using Gauss-Hermite quadrature.
 *
 * The integration interval is (-infinity, infinity), with weighting function
 * W(x) = exp(-x^2).
 *
 * \tparam T Numeric type.
 * \tparam F Function to integrate.
 * \param n Number of quadrature points.
 * \return Numerical approximation of the integral.
 */
template <class T, T F(T, const T *const)>
T quadher(const T *const, const int n);

/*!
 * Improve two initial estimates until they bracket a function root.
 *
 * \tparam T Numeric type.
 * \tparam F Function whose root is being sought.
 * \param dinter Search interval used to expand or adjust the estimates.
 */
template <class T, T F(T, const T *const)>
void MelhoraChute(T &, T &, const T *const, int dinter = 10);

/*!
 * Find a function root using the false-position method.
 *
 * \tparam T Numeric type.
 * \tparam F Function whose root is being sought.
 * \param delta Function-value tolerance.
 * \param epsn Root-position tolerance.
 * \param maxit Maximum number of iterations.
 * \return Estimated root.
 */
template <class T, T F(T, const T *const)>
T FalsaCorda(T, T, const T *const, T delta = 0.00001, T epsn = 0.00001, int maxit = 100);

/*!
 * Find a function root using the bisection method.
 *
 * \tparam T Numeric type.
 * \tparam F Function whose root is being sought.
 * \param delta Function-value tolerance.
 * \param epsn Root-position tolerance.
 * \param maxit Maximum number of iterations.
 * \return Estimated root.
 */
template <class T, T F(T, const T *const)>
T Bissecao(T, T, const T *const, T delta = 0.00001, T epsn = 0.00001, int maxit = 100);

/*!
 * Return a value with the magnitude of a and the sign of b.
 *
 * \tparam T Numeric type.
 * \param a Value providing the magnitude.
 * \param b Value providing the sign.
 * \return Value with magnitude |a| and the sign of b.
 */
template <class T>
T SIGN(T a, T b);

/*!
 * Find a function root using Ridder's method.
 *
 * \tparam T Numeric type.
 * \tparam F Function whose root is being sought.
 * \param xacc Requested root accuracy.
 * \param maxit Maximum number of iterations.
 * \return Estimated root.
 */
template <class T, T F(T, const T *const)>
T zriddr(T, T, const T *const, T xacc = 0.00001, int maxit = 60);

/*!
 * Find a function root using Brent's method.
 *
 * \tparam T Numeric type.
 * \tparam F Function whose root is being sought.
 * \param tol Function-value tolerance.
 * \param epsn Root-position tolerance.
 * \param maxit Maximum number of iterations.
 * \return Estimated root.
 */
template <class T, T F(T, const T *const)>
T zbrent(T, T, const T *const, T tol = 0.00001, T epsn = 0.00001, int maxit = 100);

/*!
 * Find a function root using Brent's method for a single-argument function.
 *
 * \tparam T Numeric type.
 * \tparam F Function whose root is being sought.
 * \param tol Function-value tolerance.
 * \param epsn Root-position tolerance.
 * \param maxit Maximum number of iterations.
 * \return Estimated root.
 */
template <class T, T F(T)>
T zbrent2(T, T, T tol = 0.00001, T epsn = 0.00001, int maxit = 100);

/*!
 * Represent a natural cubic spline interpolation.
 *
 * The class stores the input abscissas, ordinates, and spline second
 * derivatives. Natural boundary conditions are used.
 *
 * \tparam T Numeric type.
 */
template <class T>
class spline {

    int lenth; // Number of tabulated points.
    T *vx;     // Abscissa values.
    T *vy;     // Ordinate values.
    T *d2vy;   // Spline second derivatives.

  public:
    /*!
     * Construct a spline from tabulated data.
     *
     * \param n Number of tabulated points.
     * \param abs Abscissa values.
     * \param ord Ordinate values.
     */
    spline(int n, const T *const abs, const T *const ord);

    //! Copy constructor.
    spline(const spline &);

    //! Release the owned abscissa and ordinate arrays.
    ~spline() {
        delete[] vx;
        delete[] vy;
    }

    //! Copy-assignment operator.
    spline &operator=(const spline &);

    //! Return the abscissa stored at the specified index.
    T operator[](int i) const { return vx[i]; }

    //! Return the number of tabulated points.
    int size() const { return lenth; }

    /*!
     * Write sampled spline values for curve construction.
     *
     * \param lim1 Lower sampling limit.
     * \param lim2 Upper sampling limit.
     * \param npontos Number of sampled points.
     * \return Output stream.
     */
    ostream &curva(ostream &, T lim1, T lim2, int npontos) const;

    //! Evaluate the spline function.
    T Fy(T) const;

    //! Evaluate the derivative of the spline function.
    T Dy(T) const;
};

/*!
 * Numerically calculate the derivative of a function.
 *
 * The derivative is evaluated using the specified x increment.
 *
 * \tparam T Numeric type.
 * \tparam F Function to differentiate.
 * \param dx Increment used in the derivative calculation.
 * \return Numerical approximation of the derivative.
 */
template <class T, T F(T, const T *const)>
T derivada(T, const T *const, T dx = 0.001);

#endif