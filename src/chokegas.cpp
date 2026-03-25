#include "chokegas.h"

ChokeGas::ChokeGas(const ProFlu vflui,const double vareagarg,const double vdextern,
		const double vcd, const double vpresEstag,const double vpresGarg,const double vtempEstag,
		const double vfrec, int vtipo, double vareafole, double vpcalib, double vtcalib,
		const double vcdliq,const double vfrecliq) { //construtor default//alteracao2

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


}//alteracao2

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
}//alteracao2

ChokeGas& ChokeGas::operator =(const ChokeGas& vchok) {//alteracao2
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
}//alteracao2

double  ChokeGas::MasEspFlu(double pres, double temper, double salin) const{// alteracao2

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

}// alteracao2

double ChokeGas::massica(int fluido, double salin) {//alteracao2

	double massica=0.;
	double sens=1.;
	if(presEstag<presGarg){
		//double aux=presEstag;
		//presEstag=presGarg;
		//presGarg=aux;
		sens=0.;
	}

	if(fluido==0 && sens>0.){
	  double max;
	  double kad=flui.ConstAdG(presEstag,tempEstag);
	  double rho0=flui.MasEspGas(presEstag,tempEstag);
	  double rhoG=flui.MasEspGas(presGarg,tempGarg);
	  if(tipo==0 || tipo==1){//alteracao3
	    double rt=presEstag*98066.52/(rho0*flui.Zdran(presEstag,tempEstag));
	    double razcrit=pow((kad+1)/2.,-kad/(kad-1));
	    if(presGarg/presEstag>razcrit){
	      massica=sqrtl(2.*kad/(kad-1.)*pow(presGarg/presEstag,2./kad)*
			    (1.-pow(presGarg/presEstag,(kad-1.)/kad)));
	      massica*=(areagarg*presEstag*98066.52/sqrtl(rt));
		  tempGarg=pow(presGarg/presEstag,(kad-1)/kad)*tempEstag;
	    }
	    else{

		      max=sqrtl(2.*kad/(kad-1.)*pow(razcrit,2./kad)*
				    (1.-pow(razcrit,(kad-1.)/kad)));
		      max*=(areagarg*presEstag*98066.52/sqrtl(rt));
			  tempGarg=pow(razcrit,(kad-1)/kad)*tempEstag;

	      massica=max;
	    }
	  }//alteracao3
	  else{//alteracao3
		  double SGG=flui.Deng;
		  double PBAR=0.980665*presEstag;
		  double TC=tempEstag;
		  double agmm=areagarg*(1000*1000);
		  double CR;

		  int JDG = floor(10.0*(SGG - 0.55)); // Densidade
		  if (JDG < 0) JDG = 0;
		  if (JDG > 3) JDG = 3;

		  int JPR = floor(0.02*PBAR);         // Pressão
		  if (JPR > 7) JPR = 7;

		  int JTE = floor(0.05*TC)-1;               // Temperatura
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

		  double QGcrt = (8640.0*agmm*CR*PBAR/Rhostd)/sqrtl(Rg*(TC+273.15));

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

	  }//alteracao3
	  double rho1=flui.MasEspGas(presGarg,tempGarg);
	  qGarg=cd*sens*massica/rho1;
	  return cd*sens*massica;
	}
	else{
		double rho0=MasEspFlu(presEstag,tempEstag,salin);
		if(presEstag>presGarg)
		massica=areagarg*sqrtl(2.*rho0*(presEstag-presGarg)*98066.52);
		else massica=0.;
		tempGarg=tempEstag;
		double rho1=MasEspFlu(presGarg,tempGarg,salin);
		qGarg=cd*sens*massica/rho1;
		return cdliq*sens*massica;

	}

}//alteracao2

///alteração para válvula de orifício, Thornhill-Craver


/*double ChokeGas::massica(int fluido, double salin) {//alteracao2

	double massica;
	double sens=1.;
	if(presEstag<presGarg){
		//double aux=presEstag;
		//presEstag=presGarg;
		//presGarg=aux;
		sens=0.;
	}

	if(fluido==0){
	  double max;
	  double kad=1.3;
	  double rho0=flui.MasEspGas(1.0332,20.);
	  double rt=(pow(flui.Deng,1)*(tempEstag+273.23)*flui.Zdran(presEstag,tempEstag));
	  //double rt=presEstag*0.98665/(flui.MasEspGas(presEstag,tempEstag));
	  double razcrit=pow((kad+1)/2.,-kad/(kad-1));
	  double razpres=presGarg/presEstag;
	  if(presGarg/presEstag>razcrit){
	    massica=sqrtl(2.*(kad/(kad-1.))*(pow(razpres,2/kad)-pow(razpres,(kad+1)/kad))/rt);
	    massica*=423.6*(areagarg*1e6*presEstag*0.980665)*rho0/86400;
	  }
	  else{
	    massica=sqrtl(2.*(kad/(kad-1.))*(pow(razcrit,2/kad)-pow(razcrit,(kad+1)/kad))/rt);
		massica*=423.6*(areagarg*1e6*presEstag*0.98665)*rho0/86400;
	  }
	  return cd*sens*massica;
	}
	else{
		double rho0=MasEspFlu(presEstag,tempEstag,salin);
		if(presEstag>presGarg)
		massica=areagarg*sqrtl(2.*rho0*(presEstag-presGarg)*98066.52);
		else massica=0.;
		return cdliq*sens*massica;

	}

}//alteracao2*/

/*double ChokeGas::massica(int fluido, double salin) {

	double massica;
	double sens=1.;
	if(presEstag<presGarg){
		//double aux=presEstag;
		//presEstag=presGarg;
		//presGarg=aux;
		sens=0.;
	}

	  double max;
	  double kad=flui.ConstAdG(presEstag,tempEstag);
	  //double kad=1.26;
	  double rho0=flui.MasEspGas(presEstag,tempEstag);
	  double rt=presEstag*98066.5/(rho0*flui.Zdran(presEstag,tempEstag));
	 // max=sqrtl(kad)*pow(2./(kad+1.),0.5*(kad+1.)/(kad-1.))*areagarg*rho0*sqrtl(rt);
	  massica=sqrtl(2.*kad/(kad-1.)*pow(presGarg/presEstag,2./kad)*
			  (1.-pow(presGarg/presEstag,(kad-1.)/kad)));
	  massica*=(areagarg*presEstag*98066.5/sqrtl(rt));
	  double razcrit=pow((kad+1)/2.,-kad/(kad-1));
	  max=sqrtl((2.*kad/(kad-1.))*pow(razcrit,2./kad)*(1.-pow(razcrit,(kad-1.)/kad)));
	  max*=(areagarg*presEstag*98066.5/sqrtl(rt));
	  //if(massica>max)massica=max;
	  if(presGarg/presEstag<=razcrit)massica=max;
	return cd*sens*massica;
}*/


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
	return pow((mass*sqrtl(rt)/(areagarg*presEstag*98066.5)),2.)-(2*kad/(kad-1.))*rp*rp*(1.-pow(rp,kad-1.));
}
double ChokeGas::derraiz(double kad, double rp) {
	return -(2.*kad/(kad-1.))*(2.*rp*(1.-pow(rp,kad-1.))-(kad-1.)*rp*rp*pow(rp,kad-2.));
}
double ChokeGas::newton(double kad, double rt, double mass, double rp) {
	double errox=100.;
	double errof=fraiz(kad,rt,mass,rp);

	int konta=0;
	while((fabsl(errox)>1e-5 || fabsl(errof)>1e-5) && konta<100){
	 double deri=derraiz(kad,rp);
	 errox=-errof/deri;
	 rp=rp+errox;
	/* if(rp<0.){
		 errox=0.;
		 errof=0.;
		 rp=0.0001;
	 }
	 else{*/
		 errof=fraiz(kad,rt,mass,rp);
	     konta++;
	 //}
	}
	return rp;
}
double ChokeGas::razpres(double mass, double rp) {
	double kad=flui.ConstAdG(presEstag,tempEstag);
	double rho0=flui.MasEspGas(presEstag,tempEstag);
	double rt=presEstag*98066.5/(rho0*flui.Zdran(presEstag,tempEstag));
	//double max=sqrtl(kad)*pow(2./(kad+1.),0.5*(kad+1.)/(kad-1.))*areagarg*rho0*sqrtl(rt);
	double rpcrit=razcrit();
	//rp=rpcrit;
	rp=razpresSimples(mass,rp);
	double aux=newton(kad,rt,mass,pow(rp,1./kad));
	aux=pow(fabsl(aux),kad);
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
	double vsom=sqrtl((2.*kad/(kad+1.))*rt);
	double Mach=vgarg/vsom;
	double rpcrit=razcrit();
	//rp=rpcrit;
	double aux=pow(1.+0.5*(kad-1.)*Mach*Mach,kad/(kad-1.));
	//aux=1./aux;
	double rpS=1./aux;
	if(rpS>1.)rpS=1.;
	if(rpS<rpcrit)rpS=rpcrit;
	return rpS;
}

double ChokeGas::FMachVenturi(double Mach, double kad){
	//double kad=flui.ConstAdG(presEstag,tempEstag);
	double razarea=areafole/areagarg;
    return razarea-pow((1.+0.5*(kad-1)*Mach*Mach)/(0.5*(kad+1.)),0.5*(kad+1.)/(kad-1.))/Mach;
}

double ChokeGas::DeriFMachVenturi(double Mach, double kad){
	//double kad=flui.ConstAdG(presEstag,tempEstag);
	double razarea=areafole/areagarg;
	double expre1=(1.+0.5*(kad-1)*Mach*Mach)/(0.5*(kad+1.));
    return (1/(Mach*Mach))*pow(expre1,0.5*(kad+1.)/(kad-1.))-pow(expre1,(-0.5*kad+1.5)/(kad-1.));
}

double ChokeGas::newtonMachVenturi(double kad) {
	double errox=100.;
	double MachChute=0.05;
	double errof=FMachVenturi(MachChute, kad);
	double raiz;

	int konta=0;
	while((fabsl(errox)>1e-5 || fabsl(errof)>1e-5) && konta<100){
	 double deri=DeriFMachVenturi(MachChute, kad);
	 errox=-errof/deri;
	 MachChute=MachChute+errox;
	/* if(rp<0.){
		 errox=0.;
		 errof=0.;
		 rp=0.0001;
	 }
	 else{*/
		 errof=FMachVenturi(MachChute, kad);
	     konta++;
	 //}
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
		//double aux=presEstag;
		//presEstag=presGarg;
		//presGarg=aux;
		sens=0.;
	}

	  double max;
	  double kad=flui.ConstAdG(presEstag,tempEstag);
	  //double kad=1.26;
	  double rho0=flui.MasEspGas(presEstag,tempEstag);
	  double rt=presEstag*98066.5/(rho0*flui.Zdran(presEstag,tempEstag));
	  //max=sqrtl(kad)*pow(2./(kad+1.),0.5*(kad+1.)/(kad-1.))*areagarg*rho0*sqrtl(rt);
	  massica=sqrtl((2.*kad/(kad-1.))*pow(presGarg/presEstag,2./kad)*(1.-pow(presGarg/presEstag,(kad-1.)/kad)));
	  massica*=(areafole*presEstag*98066.5/sqrtl(rt));
	  double razcrit=razpresVenturi();
	  //razcrit=pow((kad+1)/2.,-kad/(kad-1));
	  max=sqrtl((2.*kad/(kad-1.))*pow(razcrit,2./kad)*(1.-pow(razcrit,(kad-1.)/kad)));
	  max*=(areafole*presEstag*98066.5/sqrtl(rt));
	  if(presGarg/presEstag<=razcrit)massica=max;
	  //if(massica>max)massica=max;
	return cd*sens*massica;
}

//template class ChokeGas;
