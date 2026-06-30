#include "chokegas.h"

ChokeGas::ChokeGas(const ProFlu vflui,const double vareagarg,const double vdextern,
		const double vcd, const double vpresEstag,const double vpresGarg,const double vtempEstag,
		const double vfrec, int vtipo, double vareafole, double vpcalib, double vtcalib,
		const double vcdliq,const double vfrecliq) { //construtor default

	flui = vflui;
	areagarg=vareagarg;
	cd=vcd;
	cdliq=vcdliq;
	pcalib=vpcalib;
	tcalib=vtcalib;
	dextern=vdextern;
	presEstag = vpresEstag;
	presGarg = vpresGarg;
	tempEstag = vtempEstag;
	tipo=vtipo;
	areafole=vareafole;
	frec=vfrec;
	frecliq=vfrecliq;
	qGarg=0.;
	tempGarg=tempEstag;


}

ChokeGas::ChokeGas(const ChokeGas& vchok) { //construtor por c�pia//alteracao2
	flui = vchok.flui;
	areagarg=vchok.areagarg;
	cd=vchok.cd;
	cdliq=vchok.cdliq;
	pcalib=vchok.pcalib;
	tcalib=vchok.tcalib;
	dextern=vchok.dextern;
	presEstag = vchok.presEstag;
	presGarg = vchok.presGarg;
	tempEstag = vchok.tempEstag;
	tipo=vchok.tipo;
	areafole=vchok.areafole;
	frec=vchok.frec;
	frecliq=vchok.frecliq;
	qGarg=vchok.qGarg;
	tempGarg=vchok.tempGarg;
}

ChokeGas& ChokeGas::operator =(const ChokeGas& vchok) {
	if (this != &vchok) {
		flui = vchok.flui;
		areagarg=vchok.areagarg;
		cd=vchok.cd;
		cdliq=vchok.cdliq;
		pcalib=vchok.pcalib;
		tcalib=vchok.tcalib;
		dextern=vchok.dextern;
		presEstag = vchok.presEstag;
		presGarg = vchok.presGarg;
		tempEstag = vchok.tempEstag;
		tipo=vchok.tipo;
		areafole=vchok.areafole;
		frec=vchok.frec;
		frecliq=vchok.frecliq;
        qGarg=vchok.qGarg;
        tempGarg=vchok.tempGarg;
	}
	return *this;
}

double  ChokeGas::MasEspFlu(double pres, double temper, double salin) const{

	  double tfarAmb=Faren(20.);
	  double ppsiAmb=psia(1.);
	  double tfar=Faren(temper);
	  double ppsi=psia(pres);
	  double bwAmb = 1.0 + 1.2e-04*(tfarAmb - 60.0) + 1.0e-06*pow(tfarAmb - 60.0,2.) - 3.33e-06*ppsiAmb;
	  double bw = 1.0 + 1.2e-04*(tfar - 60.0) + 1.0e-06*pow(tfar - 60.0,2.) - 3.33e-06*ppsi;

	  double rholw=(1000./bw)*bwAmb;
	  double rhosal=2160.;
	  double x=salin/1000.;
	  double rhomist=(1-x)/rholw+x/rhosal;
	  return 1./rhomist;

}

double ChokeGas::massica(int fluido, double salin) {

	double massica=0.;
	double sens=1.;
	if(presEstag<presGarg){
		sens=0.;
	}

	if(fluido==0 && sens>0.){
	  double max;
	  double kad=flui.ConstAdG(presEstag,tempEstag);
	  double rho0=flui.MasEspGas(presEstag,tempEstag);
	  if(tipo==0 || tipo==1){
	    double rt=presEstag*98066.52/(rho0*flui.Zdran(presEstag,tempEstag));
	    double razcrit=pow((kad+1)/2.,-kad/(kad-1));
	    if(presGarg/presEstag>razcrit){
	      massica=sqrt(2.*kad/(kad-1.)*pow(presGarg/presEstag,2./kad)*
			    (1.-pow(presGarg/presEstag,(kad-1.)/kad)));
	      massica*=(areagarg*presEstag*98066.52/sqrt(rt));
		  tempGarg=pow(presGarg/presEstag,(kad-1)/kad)*tempEstag;
	    }
	    else{

		      max=sqrt(2.*kad/(kad-1.)*pow(razcrit,2./kad)*
				    (1.-pow(razcrit,(kad-1.)/kad)));
		      max*=(areagarg*presEstag*98066.52/sqrt(rt));
			  tempGarg=pow(razcrit,(kad-1)/kad)*tempEstag;

	      massica=max;
	    }
	  }
	  else{
		  double SGG=flui.Deng;
		  double PBAR=0.980665*presEstag;
		  double TC=tempEstag;
		  double agmm=areagarg*(1000*1000);
		  double CR;

		  int JDG = floor(10.0*(SGG - 0.55));  // Densidade
		  if (JDG < 0) JDG = 0;
		  if (JDG > 3) JDG = 3;

		  int JPR = floor(0.02*PBAR);  // Pressão
		  if (JPR > 7) JPR = 7;

		  int JTE = floor(0.05*TC)-1;  // Temperatura
		  if (JTE < 0) JTE = 0;
		  if (JTE >8) JTE = 8;

		  double CRPant=(0.05*TC-1 - JTE)*(ventCR[JDG][JPR][JTE+1] - ventCR[JDG][JPR][JTE]) + ventCR[JDG][JPR][JTE];
		  double CRPpos=(0.05*TC-1 - JTE)*(ventCR[JDG][JPR+1][JTE+1] - ventCR[JDG][JPR+1][JTE]) + ventCR[JDG][JPR+1][JTE];
		  double CRDant=(0.02*PBAR - JPR)*(CRPpos - CRPant) + CRPant;

		  CRPant=(0.05*TC-1 - JTE)*(ventCR[JDG+1][JPR][JTE+1] - ventCR[JDG+1][JPR][JTE]) + ventCR[JDG+1][JPR][JTE];
		  CRPpos=(0.05*TC-1 - JTE)*(ventCR[JDG+1][JPR+1][JTE+1] - ventCR[JDG+1][JPR+1][JTE]) + ventCR[JDG+1][JPR+1][JTE];
		  double CRDpos=(0.02*PBAR  - JPR)*(CRPpos - CRPant) + CRPant;

		  if (JDG == 1)
		   CR = ((SGG - 0.5538)/(0.65 - 0.5538))*(CRDpos - CRDant) + CRDant;
		  else
		   CR = (10.0*(SGG - 0.55) - JDG)*(CRDpos - CRDant) + CRDant;
		  double Rhostd=flui.MasEspGas(1.03322745,20.);

		  double Rg=(8.0465*1000*1E5)/((flui.Deng*28.9625)*100000.);

		  double QGcrt = (8640.0*agmm*CR*PBAR/Rhostd)/sqrt(Rg*(TC+273.15));

		  double PSIC = 1.1788*pow(agmm,-0.0634);
		  if (PSIC > 0.97) PSIC = 0.97;
		  if (PSIC < 0.54) PSIC = 0.54;

		  double P3P1 = presGarg/presEstag;
		  int ICRIT = 0;
		  if (P3P1 < PSIC) ICRIT = 1;

		  double QG;
		  if (ICRIT == 1) // escoamento crítico
		   QG = QGcrt;
		  else                // escoamento sub-crítico (usa fórmula empírica)
		   QG = QGcrt*(1.0 + pow(P3P1 - PSIC,3))*(1.0 - pow((P3P1 - PSIC)/(1.0 - PSIC),2.5));

		  if(P3P1 < PSIC) tempGarg=pow(PSIC,(kad-1)/kad)*tempEstag;
		  else tempGarg=pow(P3P1,(kad-1)/kad)*tempEstag;

		  massica=QG*Rhostd/86400;

	  }
	  double rho1=flui.MasEspGas(presGarg,tempGarg);
	  qGarg=cd*sens*massica/rho1;
	  return cd*sens*massica;
	}
	else{
		double rho0=MasEspFlu(presEstag,tempEstag,salin);
		if(presEstag>presGarg)
		massica=areagarg*sqrt(2.*rho0*(presEstag-presGarg)*98066.52);
		else massica=0.;
		tempGarg=tempEstag;
		double rho1=MasEspFlu(presGarg,tempGarg,salin);
		qGarg=cd*sens*massica/rho1;
		return cdliq*sens*massica;

	}

}

double ChokeGas::tgarg() {
	double kad=flui.ConstAdG(presEstag,tempEstag);
	double raz=pow(presEstag/presGarg,(kad-1)/kad);
	return (tempEstag+273.1)/raz-273.1;
}

double ChokeGas::razcrit() {
	double kad=flui.ConstAdG(presEstag,tempEstag);
	return pow(1+0.5*(kad-1.),-kad/(kad-1.));
}

double ChokeGas::fraiz(double kad, double rt, double mass, double rp) {
	return pow((mass*sqrt(rt)/(areagarg*presEstag*98066.5)),2.)-(2*kad/(kad-1.))*rp*rp*(1.-pow(rp,kad-1.));
}

double ChokeGas::derraiz(double kad, double rp) {
	return -(2.*kad/(kad-1.))*(2.*rp*(1.-pow(rp,kad-1.))-(kad-1.)*rp*rp*pow(rp,kad-2.));
}

double ChokeGas::newton(double kad, double rt, double mass, double rp) {
    double errox = 100.;
    double errof = fraiz(kad, rt, mass, rp);

    int konta = 0;
    while ((fabs(errox) > 1e-5 || fabs(errof) > 1e-5) && konta < 100) {
        double deri = derraiz(kad, rp);
        errox = -errof / deri;
        rp = rp + errox;
        errof = fraiz(kad, rt, mass, rp);
        konta++;
    }
    return rp;
}

double ChokeGas::razpres(double mass, double rp) {
	double kad=flui.ConstAdG(presEstag,tempEstag);
	double rho0=flui.MasEspGas(presEstag,tempEstag);
	double rt=presEstag*98066.5/(rho0*flui.Zdran(presEstag,tempEstag));
	double rpcrit=razcrit();
	rp=razpresSimples(mass,rp);
	double aux=newton(kad,rt,mass,pow(rp,1./kad));
	aux=pow(fabs(aux),kad);
	rp=aux;
	if(rp>1.)rp=1.;
	if(rp<rpcrit)rp=rpcrit;
	return rp;
}

double ChokeGas::razpresSimples(double mass, double rp) {
	double kad=flui.ConstAdG(presEstag,tempEstag);
	double rho0=flui.MasEspGas(presEstag,tempEstag);
	double rhoG=flui.MasEspGas(presGarg,tempEstag);
	double rt=presEstag*98066.5/(rho0*flui.Zdran(presEstag,tempEstag));
	double cd=0.885;
	double vgarg=mass/(rhoG*areagarg*cd);
	double vsom=sqrt((2.*kad/(kad+1.))*rt);
	double Mach=vgarg/vsom;
	double rpcrit=razcrit();
	double aux=pow(1.+0.5*(kad-1.)*Mach*Mach,kad/(kad-1.));
	double rpS=1./aux;
	if(rpS>1.)rpS=1.;
	if(rpS<rpcrit)rpS=rpcrit;
	return rpS;
}

double ChokeGas::FMachVenturi(double Mach, double kad){
	double razarea=areafole/areagarg;
    return razarea-pow((1.+0.5*(kad-1)*Mach*Mach)/(0.5*(kad+1.)),0.5*(kad+1.)/(kad-1.))/Mach;
}

double ChokeGas::DeriFMachVenturi(double Mach, double kad){
	double expre1=(1.+0.5*(kad-1)*Mach*Mach)/(0.5*(kad+1.));
    return (1/(Mach*Mach))*pow(expre1,0.5*(kad+1.)/(kad-1.))-pow(expre1,(-0.5*kad+1.5)/(kad-1.));
}

double ChokeGas::newtonMachVenturi(double kad) {
	double errox=100.;
	double MachChute=0.05;
	double errof=FMachVenturi(MachChute, kad);

	int konta=0;
	while((fabs(errox)>1e-5 || fabs(errof)>1e-5) && konta<100){
	 double deri=DeriFMachVenturi(MachChute, kad);
	 errox=-errof/deri;
	 MachChute=MachChute+errox;
		 errof=FMachVenturi(MachChute, kad);
	     konta++;
	}
	return MachChute;
}

double ChokeGas::razpresVenturi(){
	double kad=flui.ConstAdG(presEstag,tempEstag);
	double Ma=newtonMachVenturi(kad);
	return 1./pow(1.+0.5*(kad-1.)*Ma*Ma, kad/(kad-1.));
}

double ChokeGas::massicaVenturi() {

	double massica;
	double sens=1.;
	if(presEstag<presGarg){
		sens=0.;
	}

	  double max;
	  double kad=flui.ConstAdG(presEstag,tempEstag);
	  double rho0=flui.MasEspGas(presEstag,tempEstag);
	  double rt=presEstag*98066.5/(rho0*flui.Zdran(presEstag,tempEstag));
	  massica=sqrt((2.*kad/(kad-1.))*pow(presGarg/presEstag,2./kad)*(1.-pow(presGarg/presEstag,(kad-1.)/kad)));
	  massica*=(areafole*presEstag*98066.5/sqrt(rt));
	  double razcrit=razpresVenturi();
	  max=sqrt((2.*kad/(kad-1.))*pow(razcrit,2./kad)*(1.-pow(razcrit,(kad-1.)/kad)));
	  max*=(areafole*presEstag*98066.5/sqrt(rt));
	  if(presGarg/presEstag<=razcrit)massica=max;
	return cd*sens*massica;
}