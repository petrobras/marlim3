/*
 * FonteMasVap.h
 *
 * Created on: May 21, 2019
 * Author: Eduardo
 *
 * Mass-source models used by the vapor-flow formulation.
 */

#ifndef FONTEMASVAP_H_
#define FONTEMASVAP_H_

#include "PropVapor.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>

using namespace std;

/*!
 * Represent a vapor source based on an inflow-performance relationship.
 *
 * The model stores the reservoir pressure and temperature, injectivity
 * index, and derivatives used by the vapor-flow formulation.
 */
class IPRVap {
  public:
    double Pres;  // Reservoir pressure in kgf/cm2.
    double Tres;  // Reservoir temperature in degrees Celsius.
    double ij;    // Injectivity index in m3/(day.kgf/cm2).
    double deriP; // Pressure-related derivative.
    double deriG; // Vapor- or gas-related derivative.

    //! Construct an IPR vapor source using default or specified parameters.
    IPRVap(double = 0, double = 0, double = 0);

    //! Copy constructor.
    IPRVap(const IPRVap &);

    //! Copy-assignment operator.
    IPRVap &operator=(const IPRVap &);

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
     * Calculate the vapor or gas mass flow rate at the specified bottom-hole state.
     *
     * \return Vapor or gas mass flow rate in kg/s.
     */
    double MasG(const double &, const double &);
};

/*!
 * Represent a prescribed vapor multiphase-injection source.
 *
 * The model stores the injected mass flow rate and injection temperature.
 */
class InjMultVap {
  public:
    double Mass; // Prescribed injected mass flow rate.
    double temp; // Injection temperature.

    //! Construct a vapor-injection source using default or specified values.
    InjMultVap(double = 0, double = 0);

    //! Copy constructor.
    InjMultVap(const InjMultVap &);

    //! Copy-assignment operator.
    InjMultVap &operator=(const InjMultVap &);

    /*!
     * Calculate the injected mass flow rate.
     *
     * \param pres Pressure at the source evaluation point.
     * \param temp Temperature at the source evaluation point.
     * \return Injected mass flow rate in kg/s.
     */
    double VMas(const double &pres, const double &temp);
};

#endif /* FONTEMASVAP_H_ */