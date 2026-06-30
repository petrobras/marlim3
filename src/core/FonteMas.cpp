#include "FonteMas.h"

IPR::IPR(double vpres, double vtres, double vip, double vij, double vqm, ProFlu vfluido, int tipoip) {
    Pres = vpres;
    Tres = vtres;
    ip = vip;
    qMax = vqm;
    ij = vij;
    deriP = 0.;
    deriG = 0.;
    deriC = 0.;
    tipoIPR = tipoip;
    this->FluidoPro = vfluido;
    Psat = vfluido.PB(Pres, Tres) / (0.9678411 * 14.69595);
}

IPR::IPR(double vpres, double vtres, double vip, double vij, double vqm, int tipoip) {
    Pres = vpres;
    Tres = vtres;
    ip = vip;
    qMax = vqm;
    ij = vij;
    Psat = 0;
    deriP = 0.;
    deriG = 0.;
    deriC = 0.;
    tipoIPR = tipoip;
    this->FluidoPro = ProFlu();
    Psat = 0.;
}

IPR::IPR(const IPR &vipr) { // construtor de c�pia
    Pres = vipr.Pres;
    Tres = vipr.Tres;
    ip = vipr.ip;
    qMax = vipr.qMax;
    ij = vipr.ij;
    deriP = vipr.deriP;
    deriG = vipr.deriG;
    deriC = vipr.deriC;
    tipoIPR = vipr.tipoIPR;
    this->FluidoPro = vipr.FluidoPro;
    Psat = vipr.Psat;
}

IPR &IPR::operator=(const IPR &vipr) {
    Pres = vipr.Pres;
    Tres = vipr.Tres;
    ip = vipr.ip;
    qMax = vipr.qMax;
    ij = vipr.ij;
    deriP = vipr.deriP;
    deriG = vipr.deriG;
    deriC = vipr.deriC;
    tipoIPR = vipr.tipoIPR;
    this->FluidoPro = vipr.FluidoPro;
    Psat = vipr.Psat;
    return *this;
}

double IPR::VMas(const double &pfundo, const double &tfundo) {
    /**
     * Returns the mass flow rate produced by the reservoir.
     *
     * Calculates the reservoir mass flow rate for a given bottom-hole pressure
     * and reservoir temperature.
     *
     * The inflow performance relationship (IPR) is calculated using Vogel's
     * correlation for saturated conditions and a linear relationship for
     * undersaturated conditions.
     *
     * The model also allows reverse inflow into the reservoir, which may result
     * in downward flow through the well.
     *
     * Units:
     * - Mass flow rate: kg/s
     * - Bottom-hole pressure: kgf/cm2
     * - Reservoir pressure: kgf/cm2
     * - Productivity index: m3/day per kgf/cm2
     * - Reservoir temperature: degrees Celsius
     *
     * @return Reservoir mass flow rate in kg/s.
     */

    double var = 0.;
    double multiplicador = 0.;
    if (tipoIPR == 1) {
        Psat = this->FluidoPro.PB(Pres, Tres) / (0.9678411 * 14.69595);
        double Psat2 = Psat;
        if (Psat > Pres)
            Psat2 = Pres;
        double qsat = ip * (Pres - Psat2);
        if (qsat < 0) {
            qsat = 0;
            Psat2 = Pres;
        }
        double qmax = ip / (1.8 / Psat2) + qsat;

        double Rhogs;
        double Rhols;
        double bsw = 0 * this->FluidoPro.BSW;

        if (pfundo <= Pres) {
            if (pfundo > Psat2) {
                var = ip * (Pres - pfundo);
                deriG = deriP = -ip;
                deriC = 0.;
            } else {
                var = (1 - bsw) * ((qmax - qsat) * (1 - 0.2 * (pfundo / Psat2) -
                                                    0.8 * (pfundo / Psat2) * (pfundo / Psat2)) +
                                   qsat) +
                      bsw * ip * (Pres - pfundo);
                deriG = deriP = -(1 - bsw) * (qmax - qsat) * (0.2 / Psat2 + 1.6 * pfundo / (Psat2 * Psat2)) -
                                bsw * ip;
                deriC = 0.;
            }
            Rhogs = this->FluidoPro.Deng * 1.225;
            Rhols = ((1000 * 141.5 / (131.5 + this->FluidoPro[1])) * (1 - this->FluidoPro[4]) +
                     1000. * this->FluidoPro[5] * this->FluidoPro[4]);
            multiplicador = (Rhols + this->FluidoPro[2] * Rhogs * (1 - this->FluidoPro[4])) / 86400;
        } else {
            var = ij * (Pres - pfundo);
            deriC = deriG = deriP = -ij;
            Rhols = (1000 * 141.5 / (131.5 + this->FluidoPro[1])) * (1 - this->FluidoPro[4]) +
                    1000. * this->FluidoPro[5] * this->FluidoPro[4];
            multiplicador = 1. / 86400;
        }
    } else if (tipoIPR == 0) {
        double Rhogs;
        double Rhols;
        if (pfundo <= Pres) {
            var = ip * (Pres - pfundo);
            deriG = deriP = -ip;
            deriC = 0.;
            Rhogs = this->FluidoPro.Deng * 1.225;
            Rhols = ((1000 * 141.5 / (131.5 + this->FluidoPro[1])) * (1 - this->FluidoPro[4]) +
                     1000. * this->FluidoPro[5] * this->FluidoPro[4]);
            multiplicador = (Rhols + this->FluidoPro[2] * Rhogs * (1 - this->FluidoPro[4])) / 86400;
        } else {
            var = ij * (Pres - pfundo);
            deriC = deriG = deriP = -ij;
            Rhols = (1000 * 141.5 / (131.5 + this->FluidoPro[1])) * (1 - this->FluidoPro[4]) +
                    1000. * this->FluidoPro[5] * this->FluidoPro[4];
            multiplicador = 1. / 86400;
        }
    } else if (tipoIPR == 2) {
        double Rhogs;
        double Rhols;
        var = qMax * (1 - 0.2 * (pfundo / Pres) - 0.8 * (pfundo / Pres) * (pfundo / Pres));
        deriG = deriP = -qMax * (0.2 / Pres + 1.6 * pfundo / (Pres * Pres));
        deriC = 0.;
        Rhogs = this->FluidoPro.Deng * 1.225; // this->FluidoPro.MasEspGas(1, 15);
        Rhols = ((1000 * 141.5 / (131.5 + this->FluidoPro[1])) * (1 - this->FluidoPro[4]) +
                 1000. * this->FluidoPro[5] * this->FluidoPro[4]);
        multiplicador = (Rhols + this->FluidoPro[2] * Rhogs * (1 - this->FluidoPro[4])) / 86400;
    }

    deriG = deriP = multiplicador * deriP;
    deriC *= multiplicador;
    return multiplicador * var;
}

double IPR::MasL(const double &pfundo, const double &tfundo) {
    /**
     * Calculates the liquid mass flow rate produced by the reservoir for a given
     * bottom-hole pressure and reservoir temperature.
     *
     * The inflow performance relationship (IPR) is calculated using Vogel's
     * correlation under saturated conditions and a linear relationship under
     * undersaturated conditions.
     *
     * The model also allows the well to inject fluid into the reservoir. In this
     * case, the calculated flow rate may represent downward flow.
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

    // FIXME: This calculation appears to be incorrect.
    // The derivatives of the phase mass flow rates with respect to pressure
    // may be calculated incorrectly. The total mass flow rate should likely
    // be calculated first, followed by its derivative with respect to pressure.
    double vazmass = VMas(pfundo, tfundo);
    deriP *= (1. - this->FluidoPro.FracMassHidra(pfundo, tfundo));
    deriC = 0;
    return (1. - this->FluidoPro.FracMassHidra(pfundo, tfundo)) * vazmass;
}

double IPR::MasG(const double &pfundo, const double &tfundo) {
    /**
     * Calculates the liquid mass flow rate produced by the reservoir for a given
     * bottom-hole pressure and reservoir temperature.
     *
     * The inflow performance relationship (IPR) is calculated using Vogel's
     * correlation for saturated reservoir conditions and a linear relationship
     * for undersaturated conditions.
     *
     * The model also allows fluid injection into the reservoir. Therefore, the
     * resulting mass flow rate may represent downward flow through the well.
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

    // FIXME: This calculation appears to be incorrect.
    // The derivatives of the phase mass flow rates with respect to pressure
    // may be calculated incorrectly. The total mass flow rate should likely
    // be calculated first, followed by its derivative with respect to pressure.
    double vazmass = VMas(pfundo, tfundo);
    deriG *= this->FluidoPro.FracMassHidra(pfundo, tfundo);
    return this->FluidoPro.FracMassHidra(pfundo, tfundo) * vazmass;
}

InjGas::InjGas(double qgij, double vtemp, ProFlu vfluido, ProFluCol vfluicol, double vqcomp) {
    QGas = qgij;
    razCompGas = vqcomp;
    temp = vtemp;
    tipoflu = 0;
    seco = 1;
    this->FluidoPro = vfluido;
    this->fluidocol = vfluicol;
    this->FluidoPro.MudaVazMas(VMas(1, 15));
}

InjGas::InjGas(double qgij, double vtemp, double vqcomp) {
    QGas = qgij;
    razCompGas = vqcomp;
    temp = vtemp;
    tipoflu = 0;
    seco = 1;
    this->FluidoPro = ProFlu();
    this->fluidocol = ProFluCol();
}

InjGas::InjGas(const InjGas &vinj) { // construtor de c�pia
    QGas = vinj.QGas;
    razCompGas = vinj.razCompGas;
    temp = vinj.temp;
    tipoflu = 0;
    seco = vinj.seco;
    this->FluidoPro = vinj.FluidoPro;
    this->fluidocol = vinj.fluidocol;
}

InjGas &InjGas::operator=(const InjGas &vinj) {
    QGas = vinj.QGas;
    razCompGas = vinj.razCompGas;
    temp = vinj.temp;
    tipoflu = 0;
    seco = vinj.seco;
    this->FluidoPro = vinj.FluidoPro;
    this->fluidocol = vinj.fluidocol;
    return *this;
}

double InjGas::VMas(const double &pres, const double &temp) {
    // NOTE: For a gas-lift source, the injected flow rate is expressed in Sm3/day, while the mass flow rate is expressed in kg/s.
    double multiplicador = this->FluidoPro.Deng * 1.225 / 86400;
    return multiplicador * QGas;
}

InjLiq::InjLiq(double qlij, double vtemp, double vbet, ProFlu vfluido, ProFluCol vfluidoC) {
    QLiq = qlij;
    temp = vtemp;
    bet = vbet;
    this->FluidoPro = vfluido;
    fluidocol = vfluidoC;
}

InjLiq::InjLiq(double qlij, double vtemp, double vbet) {
    QLiq = qlij;
    temp = vtemp;
    bet = vbet;
    this->FluidoPro = ProFlu();
    fluidocol = ProFluCol();
}

InjLiq::InjLiq(const InjLiq &vinj) { // construtor de c�pia
    QLiq = vinj.QLiq;
    temp = vinj.temp;
    bet = vinj.bet;
    this->FluidoPro = vinj.FluidoPro;
}

InjLiq &InjLiq::operator=(const InjLiq &vinj) {
    QLiq = vinj.QLiq;
    temp = vinj.temp;
    bet = vinj.bet;
    this->FluidoPro = vinj.FluidoPro;
    fluidocol = vinj.fluidocol;
    return *this;
}

double InjLiq::VMas(const double &pres, const double &temp) {
    // NOTE: For a gas-lift source, the injected flow rate is expressed in Sm3/day, while the mass flow rate is expressed in kg/s.
    double Rhols = (1000. * 141.5 / (131.5 + this->FluidoPro[1])) * (1 - this->FluidoPro[4]) + 1000. * this->FluidoPro[5] * this->FluidoPro[4];
    return (Rhols * (1. - bet) + bet * fluidocol.MasEspFlu(1., 15)) * QLiq / 86400.;
}

InjMult::InjMult(double vmp, double vmc, double vmg, double vtemp,
                 ProFlu vfluido, ProFluCol vfluidoC) {
    MassP = vmp;
    MassC = vmc;
    MassG = vmg;
    temp = vtemp;
    condTermo = 1;
    this->FluidoPro = vfluido;
    fluidocol = vfluidoC;
}

InjMult::InjMult(double vmp, double vmc, double vmg, double vtemp) {
    MassP = vmp;
    MassC = vmc;
    MassG = vmg;
    temp = vtemp;
    condTermo = 1;
    this->FluidoPro = ProFlu();
    fluidocol = ProFluCol();
}

InjMult::InjMult(const InjMult &vinj) { // construtor de c�pia
    MassP = vinj.MassP;
    MassC = vinj.MassC;
    MassG = vinj.MassG;
    temp = vinj.temp;
    condTermo = vinj.condTermo;
    this->FluidoPro = vinj.FluidoPro;
}

InjMult &InjMult::operator=(const InjMult &vinj) {
    MassP = vinj.MassP;
    MassC = vinj.MassC;
    MassG = vinj.MassG;
    temp = vinj.temp;
    condTermo = vinj.condTermo;
    this->FluidoPro = vinj.FluidoPro;
    fluidocol = vinj.fluidocol;
    return *this;
}

double InjMult::VMas(const double &pres, const double &temp) {
    // NOTE: For a gas-lift source, the injected flow rate is expressed in Sm3/day, while the mass flow rate is expressed in kg/s.

    return MassP + MassC + MassG;
}