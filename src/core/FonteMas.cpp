#include "FonteMas.h"

IPR::IPR(double vpres, double vtres, double vip, double vij, double vqm, ProFlu vfluido, int tipoip,
		int vICV,int vncv, choke vchoke) {
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
    fluidocol=ProFluCol();
    Psat = vfluido.PB(Pres, Tres) / (0.9678411 * 14.69595);
    ICV=vICV;
    chokeICV=vchoke;
    presAnul=-1.;
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
    fluidocol=ProFluCol();
    Psat = 0.;
    ICV=0;
    chokeICV=choke();
    presAnul=-1.;
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
    fluidocol=ProFluCol();
    Psat = vipr.Psat;
    chokeICV=vipr.chokeICV;
    ICV=vipr.ICV;
    presAnul=vipr.presAnul;
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
    fluidocol=ProFluCol();
    Psat = vipr.Psat;
    chokeICV=vipr.chokeICV;
    ICV=vipr.ICV;
    presAnul=vipr.presAnul;
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
        if (pfundo <= Pres) {
        	var = qMax * (1 - 0.2 * (pfundo / Pres) - 0.8 * (pfundo / Pres) * (pfundo / Pres));
        	deriG = deriP = -qMax * (0.2 / Pres + 1.6 * pfundo / (Pres * Pres));
        	deriC = 0.;
        	Rhogs = this->FluidoPro.Deng * 1.225; // this->FluidoPro.MasEspGas(1, 15);
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

double IPR::SIGN(double a, double b) {
    return (b >= 0 ? 1.0 : -1.0) * fabs(a);
}


double IPR::preparaChoke(double presM, double presJ, double tempJ, double alfa, double beta, double& massG, double& massL){
	double tE = Tres;
	double sense = 1.;

	double maxSup = 0.;

	double titM=FluidoPro.FracMassHidra(presM, tE);
	double titJ=FluidoPro.FracMassHidra(presJ, tempJ);
	double tit;


	double masChk;

	double ypres = presJ / presM;
	if (ypres < 1.) {
	    tit=titM;
	    masChk = chokeICV.vazmassSachd(ypres, presM, tE, tit,0., tit, FluidoPro, fluidocol);
	    maxSup = chokeICV.vazmaxSachd(presM, tE, tit,0, tit, FluidoPro, fluidocol);
	} else {
	    ypres = 1. / ypres;
	    sense = -1;
	    tit=titJ;
	    tit = alfa * FluidoPro.MasEspGas(presJ, tempJ) /
	    		(FluidoPro.MasEspGas(presJ, tempJ) * alfa + FluidoPro.MasEspLiq(presJ, tempJ) * (1. - alfa));
	    masChk = sense *chokeICV.vazmassSachd(ypres, presJ, tempJ, tit, 0., tit, FluidoPro,fluidocol);
	    maxSup = sense *chokeICV.vazmaxSachd(presJ, tempJ, tit, 0., tit, FluidoPro,fluidocol);
	}

	if (fabs(ypres) > fabs(chokeICV.razpres))
	    maxSup = masChk;

	if (chokeICV.AreaGarg < (1e-3) * chokeICV.AreaTub)
	    maxSup = 0.;

	massG=tit* maxSup;
	massL=(1.-tit)* maxSup;
	return  maxSup;
}

double IPR::buscaRaiz(double pfundo, double tfundo, double alfa, double beta,
		double& massG, double& massL, double& massTot, double vpresAnul){
	double temp=Tres;
	if(vpresAnul<0.)
		presAnul=0.5*(Pres+pfundo);
	else presAnul=vpresAnul;
	double massICV;
	if (chokeICV.AreaGarg < 1e-3 * chokeICV.AreaTub){
		presAnul=Pres;
		massICV=0.;
	}
	if (chokeICV.AreaGarg >= (1e-3 + 0.5) * chokeICV.AreaTub)presAnul=pfundo;
	double massIPR=VMas(presAnul, temp);
	double rhomix=1.;
	if(pfundo>Pres){
		rhomix=(1.-alfa)*FluidoPro.MasEspLiq(pfundo, tfundo)+alfa*FluidoPro.MasEspGas(pfundo, tfundo);
		massIPR*=rhomix;
	}
	if (chokeICV.AreaGarg < (1e-3 + 0.5) * chokeICV.AreaTub && chokeICV.AreaGarg > 1e-3 * chokeICV.AreaTub)
		massICV=preparaChoke(presAnul, pfundo, tfundo,alfa,beta, massG, massL);
	else massICV=massIPR;

	massTot=massIPR;

	double raiz=massIPR-massICV;
	double presAnulNeg;
	double presAnulPos;
	if(fabs(raiz)<1e-5){
		return presAnul;
	}
	else{
		if(raiz>0.){
			while(raiz>0.){
				presAnulPos=presAnul;
				presAnul*=1.01;
				if(presAnul>Pres && pfundo<Pres)presAnul=Pres;
				if(presAnul>pfundo && pfundo>Pres)presAnul=pfundo;
				massIPR=VMas(presAnul, temp);
				massICV=preparaChoke(presAnul, pfundo, tfundo,alfa,beta, massG, massL);
				raiz=massIPR-massICV;
			}
			presAnulNeg=presAnul;
		}
		else{
			while(raiz<0.){
				presAnulNeg=presAnul;
				presAnul*=0.99;
				if(presAnul<pfundo && pfundo<Pres)presAnul=pfundo;
				if(presAnul<Pres && pfundo>Pres)presAnul=Pres;
				massIPR=VMas(presAnul, temp);
				if(pfundo>Pres){
					massIPR*=rhomix;
				}
				massICV=preparaChoke(presAnul, pfundo, tfundo,alfa,beta, massG, massL);
				raiz=massIPR-massICV;
			}
			presAnulPos=presAnul;
		}
		presAnul=zriddr(presAnulNeg, presAnulPos,pfundo, tfundo,alfa,beta,rhomix, massG, massL);

		massTot=massG+massL;

		return presAnul;
	}

}

double IPR::VMasICV(double pfundo, double tfundo, double alfa, double beta,ProFlu fluidoJ,
		double& massG, double& massL, double vpresAnul){
	ProFlu fluTemp;
	double massTot;
	fluTemp=FluidoPro;
	if(Pres<=pfundo){
		FluidoPro=fluidoJ;
	}
	double presAnultemp=buscaRaiz(pfundo, tfundo,alfa,beta, massG, massL,massTot, vpresAnul);
	if (chokeICV.AreaGarg < (1e-3 + 0.5) * chokeICV.AreaTub){
		double massGTemp=massG;
		double massLTemp=massL;
		double pderi=pfundo*1.001;
		if(pderi>presAnultemp && pfundo<presAnultemp)pderi=pfundo*0.999;
		buscaRaiz(pfundo*1.001, tfundo,alfa,beta, massGTemp, massLTemp,massTot, presAnultemp);
		presAnul=presAnultemp;
		deriG = (massGTemp-massG)/(0.001*pfundo);
		deriP = (massLTemp-massL)/(0.001*pfundo);
		deriC =0.;
		FluidoPro=fluTemp;
		return massG+massL;
	}
	else{
		presAnul=pfundo;
		return massTot;
	}
}

double IPR::zriddr(double x1, double x2,double pfundo, double tfundo, double alfa, double beta, double rhomix, double& massG, double& massL) {
    double xacc = 1e-5;
    int maxit = 100;
    double fmin;
    double xmin;
    double fl;
    double fh;
    fl = rhomix*VMas(x1, Tres)-preparaChoke(x1, pfundo, tfundo,alfa,beta, massG, massL);
    fh = rhomix*VMas(x2, Tres)-preparaChoke(x2, pfundo, tfundo,alfa,beta, massG, massL);
    if (fabs(fl) > 1e9 || fabs(fh) > 1e9)
        return 1e10;
     if (fabs(fh) < fabs(fl)) {
        fmin = fh;
        xmin = x2;
    } else {
        fmin = fl;
        xmin = x1;
    }
    if ((fl > 0.0 && fh < 0.0) || (fl < 0.0 && fh > 0.0)) {
        double xl = x1;
        double xh = x2;
        double ans = -1.e20;
        for (int j = 0; j < maxit; j++) {
            double xm = 0.5 * (xl + xh);
            double fm = rhomix*VMas(xm, Tres)-preparaChoke(xm, pfundo, tfundo,alfa,beta, massG, massL);
            if (fabs(fm) < fabs(fmin)) {
                fmin = fm;
                xmin = xm;
            }
            double s = sqrt(fm * fm - fl * fh);
            if (s == 0.0) {
                fmin = rhomix*VMas(xmin, Tres)-preparaChoke(xmin, pfundo, tfundo,alfa,beta, massG, massL);
                return xmin;
            }
            double xnew = xm + (xm - xl) * ((fl >= fh ? 1.0 : -1.0) * fm / s);
            if (fabs(xnew - ans) <= xacc) {
                fmin = rhomix*VMas(xmin, Tres)-preparaChoke(xmin, pfundo, tfundo,alfa,beta, massG, massL);
                return xmin;
            }
            ans = xnew;
            double fnew = rhomix*VMas(ans, Tres)-preparaChoke(ans, pfundo, tfundo,alfa,beta, massG, massL);
            if (fabs(fnew) < fabs(fmin)) {
                fmin = fnew;
                xmin = ans;
            }
            if (fabs(fnew) <= xacc) {
                fmin = rhomix*VMas(xmin, Tres)-preparaChoke(xmin, pfundo, tfundo,alfa,beta, massG, massL);
                return xmin;
            }
            if (SIGN(fm, fnew) != fm) {
                xl = xm;
                fl = fm;
                xh = ans;
                fh = fnew;
            } else if (SIGN(fl, fnew) != fl) {
                xh = ans;
                fh = fnew;
            } else if (SIGN(fh, fnew) != fh) {
                xl = ans;
                fl = fnew;
            } else
                return -1.e10;
            if (fabs(xh - xl) <= xacc) {
                fmin = rhomix*VMas(xmin, Tres)-preparaChoke(xmin, pfundo, tfundo,alfa,beta, massG, massL);
                return xmin;
            }
        }
        return 1.e10;
    } else {
        if (fabs(fl) <= xacc) {
            return x1;
        }
        if (fabs(fh) <= xacc) {
            return x2;
        }
        return -1e10;
    }
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
