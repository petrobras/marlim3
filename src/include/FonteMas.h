#ifndef _FMAS
#define _FMAS
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "PropFlu.h"
#include "PropFluCol.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>

using namespace std;

/*!
 * Define the common interface for mass-source models.
 *
 * Derived classes implement the total mass-flow calculation as a function
 * of pressure and temperature. The interface supports source models based
 * on inflow-performance relationships and prescribed injection conditions.
 */
class AbsFonte {
  public:
    /*!
     * Calculate the total mass flow rate for the specified state.
     *
     * \param pressure Pressure at the source evaluation point.
     * \param temperature Temperature at the source evaluation point.
     * \return Total mass flow rate.
     */
    virtual double VMas(const double &, const double &) = 0;
};

/*!
 * Represent a source based on an inflow-performance relationship (IPR).
 *
 * The model stores reservoir conditions, productivity and injectivity
 * indices, saturation pressure, fluid properties, and derivatives used by
 * the production or injection formulation.
 */
class IPR : public AbsFonte {
  public:
    double Pres;      // Reservoir pressure in kgf/cm2.
    double Tres;      // Reservoir temperature in degrees Celsius.
    double ip;        // Productivity index in m3/(day.kgf/cm2).
    double qMax;      // Maximum flow rate.
    double ij;        // Injectivity index in m3/(day.kgf/cm2).
    double Psat;      // Fluid saturation pressure at reservoir temperature in kgf/cm2.
    double deriP;     // Pressure-related derivative.
    double deriG;     // Gas-related derivative.
    double deriC;     // Additional model derivative or coefficient.
    int tipoIPR;      // IPR model type.
    ProFlu FluidoPro; // Production-fluid properties.

    //! Construct an IPR source using explicit fluid properties.
    IPR(double, double, double, double, double, ProFlu, int tipoip = 1);

    //! Construct an IPR source using default fluid properties.
    IPR(double = 0, double = 0, double = 0, double = 0, double = 0, int tipoip = 1);

    //! Copy constructor.
    IPR(const IPR &);

    //! Copy-assignment operator.
    IPR &operator=(const IPR &);

    /*!
     * Replace the source-fluid properties and update the saturation pressure.
     *
     * \param vfluido New production-fluid properties.
     */
    void MudaFlu(ProFlu &vfluido) {
        this->FluidoPro = vfluido;
        Psat = vfluido.PB(Pres, Tres) / (0.9678411 * 14.69595);
    }

    /*!
     * Calculate the total mass flow rate at the specified bottom-hole state.
     *
     * \return Total mass flow rate in kg/s.
     */
    double VMas(const double &, const double &);

    /*!
     * Calculate the liquid mass flow rate at the specified bottom-hole state.
     *
     * \return Liquid mass flow rate in kg/s.
     */
    double MasL(const double &, const double &);

    /*!
     * Calculate the gas mass flow rate at the specified bottom-hole state.
     *
     * \return Gas mass flow rate in kg/s.
     */
    double MasG(const double &, const double &);
};

/*!
 * Represent a prescribed gas-injection source.
 *
 * The model converts the gas flow rate specified at standard conditions into
 * mass flow using the selected production or complementary-fluid properties.
 */
class InjGas : public AbsFonte {
  public:
    double QGas;         // Gas flow rate at standard conditions in Sm3/day.
    double razCompGas;   // Complementary-gas fraction or ratio.
    double temp;         // Injection temperature.
    int tipoflu;         // Fluid-type selector.
    int seco;            // Dry-gas condition flag.
    ProFlu FluidoPro;    // Production-fluid properties.
    ProFluCol fluidocol; // Complementary-fluid properties.

    //! Construct a gas-injection source using explicit fluid properties.
    InjGas(double, double, ProFlu, ProFluCol = ProFluCol(), double vqcomp = 0.);

    //! Construct a gas-injection source using default fluid properties.
    InjGas(double = 0, double = 25, double vqcomp = 0.);

    //! Copy constructor.
    InjGas(const InjGas &);

    //! Copy-assignment operator.
    InjGas &operator=(const InjGas &);

    /*!
     * Replace the production-fluid properties used by the source.
     *
     * \param vfluido New production-fluid properties.
     */
    void MudaFlu(ProFlu &vfluido) { this->FluidoPro = vfluido; }

    /*!
     * Calculate the injected gas mass flow rate.
     *
     * \param pres Pressure at the source evaluation point.
     * \param temp Temperature at the source evaluation point.
     * \return Gas mass flow rate.
     */
    double VMas(const double &pres, const double &temp);
};

/*!
 * Represent a prescribed liquid-injection source.
 */
class InjLiq : public AbsFonte {

  public:
    ProFluCol fluidocol; // Complementary-fluid properties.
    double QLiq;         // Liquid flow rate at standard conditions in Sm3/day.
    double bet;          // Liquid-source coefficient or fraction.
    double temp;         // Injection temperature.
    ProFlu FluidoPro;    // Production-fluid properties.

    //! Construct a liquid-injection source using explicit fluid properties.
    InjLiq(double, double, double, ProFlu, ProFluCol);

    //! Construct a liquid-injection source using default fluid properties.
    InjLiq(double = 0, double = 0, double = 0);

    //! Copy constructor.
    InjLiq(const InjLiq &);

    //! Copy-assignment operator.
    InjLiq &operator=(const InjLiq &);

    /*!
     * Replace the production-fluid properties used by the source.
     *
     * \param vfluido New production-fluid properties.
     */
    void MudaFlu(ProFlu &vfluido) { this->FluidoPro = vfluido; }

    /*!
     * Calculate the injected liquid mass flow rate.
     *
     * \param pres Pressure at the source evaluation point.
     * \param temp Temperature at the source evaluation point.
     * \return Liquid mass flow rate in kg/s.
     */
    double VMas(const double &pres, const double &temp);
};

/*!
 * Represent a prescribed multiphase injection source.
 *
 * The source stores separate mass-flow contributions for the production
 * liquid, complementary liquid, and gas phases.
 */
class InjMult : public AbsFonte {
  public:
    ProFluCol fluidocol; // Complementary-fluid properties.
    double MassP;        // Production-liquid mass-flow contribution.
    double MassC;        // Complementary-liquid mass-flow contribution.
    double MassG;        // Gas mass-flow contribution.
    double temp;         // Injection temperature.
    int condTermo;       // Thermal-condition selector.
    ProFlu FluidoPro;    // Production-fluid properties.

    //! Construct a multiphase injection source using explicit fluid properties.
    InjMult(double, double, double, double, ProFlu, ProFluCol);

    //! Construct a multiphase injection source using default fluid properties.
    InjMult(double = 0, double = 0, double = 0, double = 0);

    //! Copy constructor.
    InjMult(const InjMult &);

    //! Copy-assignment operator.
    InjMult &operator=(const InjMult &);

    /*!
     * Replace the production-fluid properties used by the source.
     *
     * \param vfluido New production-fluid properties.
     */
    void MudaFlu(ProFlu &vfluido) { this->FluidoPro = vfluido; }

    /*!
     * Calculate the total injected multiphase mass flow rate.
     *
     * \param pres Pressure at the source evaluation point.
     * \param temp Temperature at the source evaluation point.
     * \return Total mass flow rate in kg/s.
     */
    double VMas(const double &pres, const double &temp);
};

#endif