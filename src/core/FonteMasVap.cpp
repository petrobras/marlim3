/*
 * FonteMasVap.cpp
 *
 *  Created on: 21 de mai de 2019
 *      Author: eduardo
 */

#include "FonteMasVap.h"

IPRVap::IPRVap(double vpres, double vtres, double vij) {
    Pres = vpres;
    Tres = vtres;
    ij = vij;
    deriP = 0.;
    deriG = 0.;
}

IPRVap::IPRVap(const IPRVap &vipr) { // construtor de c�pia
    Pres = vipr.Pres;
    Tres = vipr.Tres;
    ij = vipr.ij;
    deriP = vipr.deriP;
    deriG = vipr.deriG;
}

IPRVap &IPRVap::operator=(const IPRVap &vipr) {
    Pres = vipr.Pres;
    Tres = vipr.Tres;
    ij = vipr.ij;
    deriP = vipr.deriP;
    deriG = vipr.deriG;
    return *this;
}

double IPRVap::VMas(const double &pfundo, const double &tfundo) {
    /**
     * Calculates the mass flow rate produced by the reservoir for a given
     * bottom-hole pressure and reservoir temperature.
     *
     * The inflow performance relationship (IPR) is calculated using Vogel's
     * correlation under saturated conditions and a linear relationship under
     * undersaturated conditions.
     *
     * The model also allows fluid injection into the reservoir. Therefore, the
     * resulting mass flow rate may represent downward flow through the well.
     *
     * Units:
     * - Mass flow rate: kg/s
     * - Bottom-hole pressure: kgf/cm2
     * - Reservoir pressure: kgf/cm2
     * - Productivity index: m3/day per kgf/cm2
     * - Reservoir temperature: degrees Celsius
     *
     * @return Mass flow rate in kg/s.
     */

    double var;
    double multiplicador;

    var = ij * (Pres - pfundo);
    deriG = deriP = -ij;
    multiplicador = 1. / 86400;

    deriG = deriP = multiplicador * deriP;
    return multiplicador * var;
}

double IPRVap::MasL(const double &pfundo, const double &tfundo) {
    /**
     * Calculates the liquid mass flow rate produced by the reservoir for a given
     * bottom-hole pressure and reservoir temperature.
     *
     * The inflow performance relationship (IPR) is calculated using Vogel's
     * correlation under saturated conditions and a linear relationship under
     * undersaturated conditions.
     *
     * The model also supports fluid injection into the reservoir. Therefore, the
     * resulting liquid mass flow rate may represent downward flow through the well.
     *
     * Units:
     * - Liquid mass flow rate: kg/s
     * - Bottom-hole pressure: kgf/cm2
     * - Reservoir pressure: kgf/cm2
     * - Productivity index: m3/day per kgf/cm2
     * - Reservoir temperature: degrees Celsius
     *
     * @return Liquid mass flow rate in kg/s.
     */

    ProVap flup;
    double tit = flup.Titulo(pfundo, tfundo, TEMP);
    deriP *= (1. - tit);
    return (1. - tit) * VMas(pfundo, tfundo);
}

double IPRVap::MasG(const double &pfundo, const double &tfundo) {
    /**
     * Calculates the liquid mass flow rate produced by the reservoir for a given
     * bottom-hole pressure and reservoir temperature.
     *
     * The inflow performance relationship (IPR) is determined using Vogel's
     * correlation for saturated conditions and a linear relationship for
     * undersaturated conditions.
     *
     * The model also allows reverse flow into the reservoir, in which case the
     * resulting liquid mass flow rate may represent downward flow in the well.
     *
     * Units:
     * - Liquid mass flow rate: kg/s
     * - Bottom-hole pressure: kgf/cm2
     * - Reservoir pressure: kgf/cm2
     * - Productivity index: m3/day per kgf/cm2
     * - Reservoir temperature: degrees Celsius
     *
     * @return Liquid mass flow rate in kg/s.
     */

    ProVap flup;
    double tit = flup.Titulo(pfundo, tfundo, TEMP);
    deriG *= tit;
    return tit * VMas(pfundo, tfundo);
}

InjMultVap::InjMultVap(double vmp, double vtemp) {
    Mass = vmp;
    temp = vtemp;
}

InjMultVap::InjMultVap(const InjMultVap &vinj) { // construtor de c�pia
    Mass = vinj.Mass;
    temp = vinj.temp;
}

InjMultVap &InjMultVap::operator=(const InjMultVap &vinj) {
    Mass = vinj.Mass;
    temp = vinj.temp;
    return *this;
}

double InjMultVap::VMas(const double &pres, const double &temp) {
    // NOTE: For a gas-lift source, the injected flow rate is expressed in Sm3/day, while the mass flow rate is expressed in kg/s.

    return Mass;
}