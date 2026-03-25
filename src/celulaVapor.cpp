/*
 * celulaVapor.cpp
 *
 *  Created on: 21 de mai de 2019
 *      Author: eduardo
 */




#include "celulaVapor.h"

CelVap::CelVap(varGlob1D* Vvg1dSP,const DadosGeo vdutoL,const DadosGeo vduto,
		   const DadosGeo vdutoR,
        //const ProFlu vflui,
		   const ProVap vflui,
        const double vtempL,const double vtemp,
        const double vtempR,const double vtempini,
        const double vpresL,const double vpres,const double vpresR,
        const double vML,const double vMC,const double vMR,
        const double vMliqiniL,const double vMliqini,const double vMliqiniR,
        const double valfL,const double valf,const double valfR,
        const double vdxL,const double vdx,const double vdxR,
        const double vdt, const int vposic,
        const TransCal vcalor,
        const acessorio vacsr,
        acessorio* vacsrL) :
		TL(2), local(2,5) { //construtor default

	vg1dSP=Vvg1dSP;
	dutoL = vdutoL;
	duto = vduto;
	dutoR = vdutoR;
	flui = vflui;
	acsr=vacsr;
	acsrL=vacsrL;
	calor=vcalor;
	tempR = vtempR;
	temp = vtemp;
	tempL = vtempL;
	tempini = vtempini;
	presL=vpresL;
	presLini=vpresL;
	presauxL=vpresL;
	presaux=pres=presini=vpres;
	presauxR=vpresR;
	presR = vpresR;
	entalp=0.;
	uinter=0.;
	dxL = vdxL;
	dx = vdx;
	dxR = vdxR;
	dt = vdt;
	dt2=dt;
	posic = vposic;
	ML = vML;
	MC = vMC;
	MR = vMR;
	MRini = MR;
	alfL=valfL;
	alf=valf;
	alfR=valfR;
	MliqiniL = vMliqiniL;
	Mliqini = vMliqini;
	MliqiniR = vMliqiniR;
	MliqiniR0 = vMliqiniR;// alteracao7
	alfLini=alfL;
	alfini=alf;
	alfRini=alfR;
	massfonteCH=0;
	c0=1.;
	ud=0.;
	c0Spare=c0;
	udSpare=ud;
    transmassL=0.;
    transmassR=0.;
    FonteMudaFase=0.;
    DTransDt1=0.;
    DTransDt0=0.;
    DTransDxR=0.;
    DTransDxL=0.;
    CoefDTR=0.;
    CoefDTL=0.;
    fontedissolv=0.;
    TMModel=2;
    TMModelL=2;
    fontemassLL=0.;
    fontemassGL=0.;
    fontemassLR=0.;
    fontemassGR=0.;
    dpB=0.;
    potB=0;
    cinematico=1.;
    arranjo=1;
    arranjoR=1;
	if(fabsl(ML)<1e-5){
		term1L=0.;
		term2L=0.;
	}
	else{
		term1L=MliqiniL/ML;
		term2L=0.;
	}
	if(fabsl(MC)<1e-5){
		term1=0.;
		term2=0.;
	}
	else{
		term1=Mliqini/MC;
		term2=0.;
	}
	if(fabsl(MR)<1e-5){
		term1R=0.;
		term2R=0.;
	}
	else{
		term1R=MliqiniR/MR;
		term2R=0.;
	}
	double localtinyTemp;
	if(vg1dSP!=0)localtinyTemp=(*vg1dSP).localtiny;
	else localtinyTemp=1e-9;
	if(fabsl(alf)>=localtinyTemp && fabsl(alf)<=1-localtinyTemp)transic=0;
	else if(fabsl(alf)>=localtinyTemp)transic=-1;
	else transic=1;
	double razdx;
	razdx=dx/(dx+dxL);
	double pmed=razdx*pres+(1-razdx)*presL;
	double rl=flui.MasEspLiq(pmed,temp,TEMP);
	double rg=flui.MasEspGas(pmed,temp,TEMP);

	rpL=rpLi=flui.MasEspLiq(presL,tempL,TEMP);
	rpC=rpCi=flui.MasEspLiq(pres,temp,TEMP);
	rpR=rpRi=flui.MasEspLiq(presR,tempR,TEMP);
	double rmisL=rpL;
	double rmisR=rpR;
	double rmisC=rl;

	QL=Mliqini/rmisC;
	QG=(MC-Mliqini)/rg;

	QLL=MliqiniL/rmisL;
	QLR=MliqiniR/rmisR;
	QLRini=MliqiniR/rmisR;// alteracao7

	if(vdutoR.area>0.)VTemper=(QL+QG)/vdutoR.area;
	else VTemper=0.;

	perdaEstratL=0.;
	perdaEstratG=0.;

	estadoPig=0;
	dtPig=dt;
	alfPigE=1.-alf;
	alfPigER=alfPigE;
	alfPigD=alfPigE;
	alfPigEini=1.-alf;
	alfPigDini=alfPigE;
	velPig=0.;
	velPigini=0.;
	razPig=0.;
	razPigini=0.;
	RazAreaPig=0.;//alteracao2
    DelPig=0.;//alteracao2
    VazaPig=0.;//alteracao2
    cdpig=0.;//alteracao2
    indpig=-1;//alteracao2

    DmasschokeG=0.;
    DmasschokeL=0.;

    chutearranjo=-1.;

    ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
    presBuf=pres;
    presLiniBuf=presLini;
    presLBuf=presL;
    presRBuf=presR;
    presauxBuf=presaux;
    MCBuf=MC;
    MRBuf=MR;
    MliqiniRBuf=MliqiniR;
    MRiniBuf=MRini;
    MLBuf=ML;
    MliqiniBuf=Mliqini;
    MliqiniLBuf=MliqiniL;
    //////////////////////////// 05-07-2018 ///////////////////////////////////////////////

    corrigeMassaPres=0.;



}

CelVap::CelVap(const CelVap& vcel) :
	TL(2), local(2, 5) { //construtor por copia
	vg1dSP=vcel.vg1dSP;
	dutoL = vcel.dutoL;
	duto = vcel.duto;
	dutoR = vcel.dutoR;
	flui = vcel.flui;
	acsr=vcel.acsr;
	acsrL=vcel.acsrL;
	calor=vcel.calor;
	tempR = vcel.tempR;
	temp = vcel.temp;
	tempL = vcel.tempL;
	tempini = vcel.tempini;
	presL= vcel.presL;
	presLini=vcel.presLini;
	pres= vcel.pres;
	presini= vcel.presini;
	presauxL= vcel.presauxL;
	presaux= vcel.presaux;
	presauxR= vcel.presauxR;
	presR = vcel.presR;
	entalp=vcel.entalp;
	uinter=vcel.uinter;
	dxL = vcel.dxL;
	dx = vcel.dx;
	dxR = vcel.dxR;
	dt = vcel.dt;
	dt2 = vcel.dt2;
	posic = vcel.posic;
	ML = vcel.ML;
	MC = vcel.MC;
	MR = vcel.MR;
	MRini=vcel.MRini;
	alfL= vcel.alfL;
	alf= vcel.alf;
	alfR= vcel.alfR;
	MliqiniL = vcel.MliqiniL;
	Mliqini = vcel.Mliqini;
	MliqiniR = vcel.MliqiniR;
	MliqiniR0 = vcel.MliqiniR0;// alteracao7
	alfLini= vcel.alfLini;
	alfini= vcel.alfini;
	alfRini= vcel.alfRini;
	TL=vcel.TL;
	local=vcel.local;
	massfonteCH=vcel.massfonteCH;
	c0=vcel.c0;
	ud=vcel.ud;
	c0Spare=vcel.c0Spare;
	udSpare=vcel.udSpare;
    transmassL=vcel.transmassL;
    transmassR=vcel.transmassR;
    FonteMudaFase=vcel.FonteMudaFase;
    DTransDt1=vcel.DTransDt1;
    DTransDt0=vcel.DTransDt0;
    DTransDxR=vcel.DTransDxR;
    DTransDxL=vcel.DTransDxL;
    CoefDTR=vcel.CoefDTR;
    CoefDTL=vcel.CoefDTL;
    fontedissolv=vcel.fontedissolv;
    TMModel=vcel.TMModel;
    TMModelL=vcel.TMModelL;
    fontemassLL=vcel.fontemassLL;
    fontemassGL=vcel.fontemassGL;
    fontemassLR=vcel.fontemassLR;
    fontemassGR=vcel.fontemassGR;
    dpB=vcel.dpB;
    potB=vcel.potB;
	term1L=vcel.term1L;
	term2L=vcel.term2L;
	term1=vcel.term1;
	term2=vcel.term2;
	term1R=vcel.term1R;
	term2R=vcel.term2R;
	transic=vcel.transic;
	QL=vcel.QL;
	QG=vcel.QG;
	QLL=vcel.QLL;
	QLR=vcel.QLR;
	QLRini=vcel.QLRini;// alteracao7
	cinematico=vcel.cinematico;
	arranjo=vcel.arranjo;
	arranjoR=vcel.arranjoR;
	VTemper=vcel.VTemper;

	rpL=vcel.rpL;
	rpC=vcel.rpC;
	rpR=vcel.rpR;

	rpLi=vcel.rpLi;
	rpCi=vcel.rpCi;
	rpRi=vcel.rpRi;

	perdaEstratL=vcel.perdaEstratL;
	perdaEstratG=vcel.perdaEstratG;

	estadoPig=vcel.estadoPig;
	dtPig = vcel.dtPig;
	alfPigE=vcel.alfPigE;
	alfPigER=vcel.alfPigER;
	alfPigD=vcel.alfPigD;
	alfPigEini=vcel.alfPigEini;
	alfPigDini=vcel.alfPigDini;
	velPig=vcel.velPig;
	velPigini=vcel.velPigini;
	razPig=vcel.razPig;
	razPigini=vcel.razPigini;
	RazAreaPig=vcel.RazAreaPig;//alteracao2
    DelPig=vcel.DelPig;//alteracao2
    VazaPig=vcel.VazaPig;//alteracao2
    cdpig=vcel.cdpig;//alteracao2
    indpig=vcel.indpig;//alteracao2

    DmasschokeG=vcel.DmasschokeG;
    DmasschokeL=vcel.DmasschokeL;

    chutearranjo=vcel.chutearranjo;

    ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
    presBuf=vcel.presBuf;
    presLiniBuf=vcel.presLiniBuf;
    presLBuf=vcel.presLBuf;
    presRBuf=vcel.presRBuf;
    presauxBuf=vcel.presauxBuf;
    MCBuf=vcel.MCBuf;
    MRBuf=vcel.MRBuf;
    MliqiniRBuf=vcel.MliqiniRBuf;
    MRiniBuf=vcel.MRiniBuf;
    MLBuf=vcel.MLBuf;
    MliqiniBuf=vcel.MliqiniBuf;
    MliqiniLBuf=vcel.MliqiniLBuf;
    //////////////////////////// 05-07-2018 ///////////////////////////////////////////////

    corrigeMassaPres=0.;

}

CelVap& CelVap::operator =(const CelVap& vcel) {
	if (this != &vcel) {
		vg1dSP=vcel.vg1dSP;
		dutoL = vcel.dutoL;
		duto = vcel.duto;
		dutoR = vcel.dutoR;
		flui = vcel.flui;
		acsr=vcel.acsr;
		acsrL=vcel.acsrL;
		calor=vcel.calor;
		tempR = vcel.tempR;
		temp = vcel.temp;
		tempL = vcel.tempL;
		tempini = vcel.tempini;
		presL= vcel.presL;
		presLini=vcel.presLini;
		pres= vcel.pres;
		presini= vcel.presini;
		presauxL= vcel.presauxL;
		presaux= vcel.presaux;
		presauxR= vcel.presauxR;
		presR = vcel.presR;
		entalp=vcel.entalp;
		uinter=vcel.uinter;
		dxL = vcel.dxL;
		dx = vcel.dx;
		dxR = vcel.dxR;
		dt = vcel.dt;
		dt2 = vcel.dt2;
		posic = vcel.posic;
		ML = vcel.ML;
		MC = vcel.MC;
		MR = vcel.MR;
		MRini=vcel.MRini;
		alfL= vcel.alfL;
		alf= vcel.alf;
		alfR= vcel.alfR;
		MliqiniL = vcel.MliqiniL;
		Mliqini = vcel.Mliqini;
		MliqiniR = vcel.MliqiniR;
		MliqiniR0 = vcel.MliqiniR0;// alteracao7
		alfLini= vcel.alfLini;
		alfini= vcel.alfini;
		alfRini= vcel.alfRini;
		TL=vcel.TL;
		local=vcel.local;
		massfonteCH=vcel.massfonteCH;
		c0=vcel.c0;
		ud=vcel.ud;
	    transmassL=vcel.transmassL;
	    transmassR=vcel.transmassR;
	    FonteMudaFase=vcel.FonteMudaFase;
	    DTransDt1=vcel.DTransDt1;
	    DTransDt0=vcel.DTransDt0;
	    DTransDxR=vcel.DTransDxR;
	    DTransDxL=vcel.DTransDxL;
	    fontedissolv=vcel.fontedissolv;
	    CoefDTR=vcel.CoefDTR;
	    CoefDTL=vcel.CoefDTL;
	      ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
	    presBuf=vcel.presBuf;
	    presLiniBuf=vcel.presLiniBuf;
	    presLBuf=vcel.presLBuf;
	    presRBuf=vcel.presRBuf;
	    presauxBuf=vcel.presauxBuf;
	    MCBuf=vcel.MCBuf;
	    MRBuf=vcel.MRBuf;
	    MliqiniRBuf=vcel.MliqiniRBuf;
	    MRiniBuf=vcel.MRiniBuf;
	    MLBuf=vcel.MLBuf;
	    MliqiniBuf=vcel.MliqiniBuf;
	    MliqiniLBuf=vcel.MliqiniLBuf;
        //////////////////////////// 05-07-2018 /////////////////////////////////////////////
	    TMModel=vcel.TMModel;
	    TMModelL=vcel.TMModelL;
	    fontemassLL=vcel.fontemassLL;
	    fontemassGL=vcel.fontemassGL;
	    fontemassLR=vcel.fontemassLR;
	    fontemassGR=vcel.fontemassGR;
	    dpB=vcel.dpB;
	    potB=vcel.potB;
		term1L=vcel.term1L;
		term2L=vcel.term2L;
		term1=vcel.term1;
		term2=vcel.term2;
		term1R=vcel.term1R;
		term2R=vcel.term2R;
		transic=vcel.transic;
		QL=vcel.QL;
		QG=vcel.QG;
		QLL=vcel.QLL;
		QLR=vcel.QLR;
		QLRini=vcel.QLRini;// alteracao7
		cinematico=vcel.cinematico;
		arranjo=vcel.arranjo;
		arranjoR=vcel.arranjoR;
		VTemper=vcel.VTemper;

		rpL=vcel.rpL;
		rpC=vcel.rpC;
		rpR=vcel.rpR;

		rpLi=vcel.rpLi;
		rpCi=vcel.rpCi;
		rpRi=vcel.rpRi;

		perdaEstratL=vcel.perdaEstratL;
		perdaEstratG=vcel.perdaEstratG;

		estadoPig=vcel.estadoPig;
		dtPig = vcel.dtPig;
		alfPigE=vcel.alfPigE;
		alfPigER=vcel.alfPigER;
		alfPigD=vcel.alfPigD;
		alfPigEini=vcel.alfPigEini;
		alfPigDini=vcel.alfPigDini;
		velPig=vcel.velPig;
		velPigini=vcel.velPigini;
		razPig=vcel.razPig;
		razPigini=vcel.razPigini;
		RazAreaPig=vcel.RazAreaPig;//alteracao2
	    DelPig=vcel.DelPig;//alteracao2
	    VazaPig=vcel.VazaPig;//alteracao2
	    cdpig=vcel.cdpig;//alteracao2
	    indpig=vcel.indpig;

	    DmasschokeG=vcel.DmasschokeG;
	    DmasschokeL=vcel.DmasschokeL;

	    chutearranjo=vcel.chutearranjo;

	    ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
	    presBuf=vcel.presBuf;
	    presLiniBuf=vcel.presLiniBuf;
	    presLBuf=vcel.presLBuf;
	    presRBuf=vcel.presRBuf;
	    presauxBuf=vcel.presauxBuf;
	    MCBuf=vcel.MCBuf;
	    MRBuf=vcel.MRBuf;
	    MliqiniRBuf=vcel.MliqiniRBuf;
	    MRiniBuf=vcel.MRiniBuf;
	    MLBuf=vcel.MLBuf;
	    MliqiniBuf=vcel.MliqiniBuf;
	    MliqiniLBuf=vcel.MliqiniLBuf;
	    //////////////////////////// 05-07-2018 ///////////////////////////////////////////////

	    corrigeMassaPres=0.;

	}
	return *this;
}

double CelVap::Rey(double dia, double vel,
		double rho, double vis) {
	return dia * fabsl(vel) * rho / (vis * 1e-3);
}
double CelVap::fric(double re, double eps) {

/*
 double val;
 if (fabsl(re) > 1e-5) {
		if (re > 2400) {
			val = 6.9 / fabsl(re) + pow(eps / 3.7, 1.11);
			val = -1.8 * (logl(val) / logl(10.));
			val = pow(1 / val,2.);
		} else
			val = 4*16. / fabsl(re);
	} else
		val = 0.;
	return val/4.;*/
	double val;
	   if (fabsl(re) > 1e-5) {
			if (re > 2400) {
				val = 6.9 / fabsl(re) + pow(eps / 3.7, 1.11);
				val = -1.8 * (logl(val) / logl(10.));
				val = pow(1 / val,2.);
				for(int konta=0;konta<2;konta++){
				  val=pow(-2.*logl( 2.51 / fabsl(re*sqrtl(val)) + eps/ 3.7)/logl(10.),2.);
				  val=1./val;
				}
				val/=4.;
			} else
				val = 16. / fabsl(re);
		} else
			val = 0.;
	   return val;
}

 void CelVap::avancalf(int& reinicia, int ncel){

   double convec=0.;
   double term1Mass=0.;



    double rlm=rpC;
    double fator1=1.;
    if(alf<=(*vg1dSP).CritCond&&transmassR<0.){
    	transmassR=0.;
    	FonteMudaFase=0.;
    }
    //if(alf<=1e-2&&transmassR<0. && (*acsrL).tipo==4){
    	//transmassR=0.;
    	//FonteMudaFase=0.;
    //}
    if(posic<ncel){
	   convec=(1/rpC)*MliqiniR/dx-(1/rpC)*Mliqini/dx;
	   term1Mass=(convec-fontemassLR/(dx*rpC)+transmassR/rpC)/duto.area;
   }
   else{
	   //convec=-QL*rpCi*(1.-betI)/(dx*rpC)-QL*rcCi*betI/(dx*rcC);
	   convec=-(1/rpC)*Mliqini/dx;
	   term1Mass=(convec-fontemassLR/(dx*rpC))/duto.area;
	   CoefDTR=0.;
	   CoefDTL=0.;
   }
   if(term1Mass>0 && alfini>1-(*vg1dSP).localtiny)term1Mass=0.;
   if(term1Mass<0 && alfini<(*vg1dSP).localtiny)term1Mass=0.;
   alf=(1-CoefDTL/(rpC*duto.area))*alfini/ (1+CoefDTR/(rpC*duto.area))+
		  ((CoefDTR+CoefDTL)/(rpC*duto.area))/ (1+CoefDTR/(rpC*duto.area))+
		  (term1Mass/ (1+CoefDTR/(rpC*duto.area)))*dt;

   double resto=0.;
   if(((alf<=(*vg1dSP).localtiny)&&(alf>=-(*vg1dSP).localtiny))){
	   alf=fabsl(0.);
   }
   else if(alf<-(*vg1dSP).localtiny){
      double dtaux;
      dtaux=-((1-CoefDTL*fator1/(rlm*duto.area))*alfini/ (1+CoefDTR*fator1/(rlm*duto.area))+
    		((CoefDTR*fator1+CoefDTL*fator1)/(rlm*duto.area))/ (1+CoefDTR*fator1/(rlm*duto.area)))/
    				(term1Mass/ (1+CoefDTR*fator1/(rlm*duto.area)));
      if(dtaux>(*vg1dSP).localtiny){
        dt=dtaux;
        reinicia=-1;
        alf=fabsl(0.);
      }
      else alf=fabsl(0.);
   }
   else if((alf>=(1.-(*vg1dSP).localtiny)&&alf<=(1.+(*vg1dSP).localtiny))){
	   alf=1.;
   }
   else if(alf>(1.+(*vg1dSP).localtiny)){
      double dtaux;
      //dtaux=(1.-alfini)/term1Mass;
      dtaux=(1.-((1-CoefDTL*fator1/(rlm*duto.area))*alfini/ (1+CoefDTR*fator1/(rlm*duto.area))+
    		((CoefDTR*fator1+CoefDTL*fator1)/(rlm*duto.area))/ (1+CoefDTR*fator1/(rlm*duto.area))))/
    		(term1Mass/ (1+CoefDTR*fator1/(rlm*duto.area)));
      if(dtaux>(*vg1dSP).localtiny){
        dt=dtaux;
        reinicia=-1;
        alf=1.;
      }
      else alf=1.;
   }

}



void CelVap::GeraLocal(double presfim, int masChkSup, int ncel, double razareativa,
        double presE, double tempE, double titE) {

	if (posic != 0 && posic != ncel) {

		   if(posic>=95 && posic<=98){
		   	int para;
		   	para=0;
		   }


	  double AL =dutoL.area;
	  double AC =duto.area;
	  double AR =dutoR.area;
      double siL=dutoL.peri;
	  double siC=duto.peri;
	  double siR=dutoR.peri;
	  double rhogL = flui.MasEspGas(presL,tempL,TEMP);
	  double rholL = rpL;
	  double rhogC = flui.MasEspGas(pres,temp,TEMP);
	  double rholC = rpC;
	  double rhogR = flui.MasEspGas(presR,tempR,TEMP);
	  double rholR = rpR;
	  double compres;
	  double dzdp;
	  double dpdrho;
	  double multL=((1/rpL)*rpLi)/(rpLi);
	  double multR=((1/rpL)*rpCi)/(rpCi);

	  int master=1;
	  if(acsrL!=0){
			if(((*acsrL).tipo==5&&(*acsrL).chk.AreaGarg<(1e-3+razareativa)*AL)
				||((*acsrL).tipo==8&&fabsl((*acsrL).bvol.freq)>1 ))master=0;
	  }
	  if(master==1){

    	compres = flui.ZFunc(presL, tempL);
    	dzdp = flui.DZDPFunc(presL, tempL);
    	dpdrho = rhogL* (1 / (presL * 98066.5) - (1 / compres) * dzdp);
    	dpdrho = 1. / dpdrho;

    	if(alfLini<=(*vg1dSP).CritCond&&transmassL<0.)transmassL=0.;
    	//if(alfLini<=1e-2&&transmassL<0. && (*acsrL).tipo==4)transmassL=0.;

        local[0][0] = -((1./rhogL)*(1.-term1L)+multL*term1L)-dxL*(1/rhogL-1/rpL)*term1L*DTransDxL;
    	//local[0][0] = -1./rhogL+term1L/rhogL-term1L/rholL;
	    local[0][1] = dxL*(AL*(alfL/rhogL)/dpdrho)*98066.5 / dt;
	    local[0][2] = (1./rhogL)*(1-term1)+multR*term1-dxL*(1/rhogL-1/rpL)*term1*DTransDxR;
	    //local[0][2] = 1./rhogL-term1/rhogL+term1/rholL;
	    local[0][3] = 0.;
	    local[0][4] = 0.;

	    if(posic==1 && presE>0)fontemassGL+=corrigeMassaPres*dxL;

	    if(TMModelL==1 || TMModelL==2 || TMModelL==3){
	      TL[0] = (dxL*AL*(alfL/rhogL)/dpdrho)*presL*98066.5 / dt+(1/rhogL-multR)*term2-
	    		(1/rhogL-multL)*term2L+
	    		(fontemassGL/rhogL+(fontemassLL/rpL))+dxL*(1/rhogL-1/rpL)*transmassL;
	    }
	    else if(TMModelL==0){
	    TL[0] = (dxL*AL*(alfL/rhogL)/dpdrho)*presL*98066.5 / dt+(1/rhogL-multR)*term2-
	    	    		(1/rhogL-multL)*term2L+
	    	    		(fontemassGL/rhogL+(fontemassLL/rpL))+
						dxL*(1/rhogL-1/rpL)*(DTransDt1*(1.-alfL)/dt+
								               DTransDt0*(1.-alfLini)/dt+term2*DTransDxR+term2L*DTransDxL);
	    }

	    if(acsrL!=0){
	      if((*acsrL).tipo==3){
	    	local[0][1]-=((*acsrL).ipr.deriG/rhogL+((*acsrL).ipr.deriP+(*acsrL).ipr.deriC)/rholL);
	    	TL[0] -=((*acsrL).ipr.deriG/rhogL+((*acsrL).ipr.deriP+(*acsrL).ipr.deriC)/rholL)*presL;
	      }
	    }


      }
      else{
        local[0][0] = 0.;
  	    local[0][1] = 0.;
  	    local[0][2] = 1.;
  	    local[0][3] = 0.;
  	    local[0][4] = 0.;

  	    TL[0] = 0.;
      }

      if((acsr.tipo!=5||acsr.chk.AreaGarg>=(1e-3+razareativa)*AC)&&
    	 (acsr.tipo!=8||fabsl(acsr.bvol.freq)<=1.)){
		double dxmed = 0.5 * (dx + dxR);
		double Amed =(dx*AC+dxR*AR)/(dx + dxR);
		//double Amed =AC;
		//if((MR-MliqiniR)<0.)Amed =AR;


		double ugL=0*((MC-Mliqini)/(rhogC))/AC;
		if(alf>(*vg1dSP).localtiny)ugL/=alf;
		else ugL=0.;
		double ugR=0*((MR-MliqiniR)/(rhogR))/AR;
		if(alfR>(*vg1dSP).localtiny)ugR/=alfR;
		else ugR=0.;
		local[1][0] = 0.;
		local[1][1] = -ugL*(1-term1)/dx;
		local[1][2] = -Amed*98066.5/dxmed;
		local[1][3] = (1-term1R) / dt+ugR*(1-term1R)/dx;
		local[1][4] = Amed*98066.5/dxmed;

		TL[1] = (MR-MliqiniR) / dt+ term2R / dt+(ugR*term2R-ugL*term2)/dx;

		double j1;
		double j2;

		j1=(MliqiniR/rholC+(MR-MliqiniR)/rhogC)/AC;
		j2=(MliqiniR/rholR+(MR-MliqiniR)/rhogR)/AR;
		//j1=((term1R*MR+term2R)/rholC+(MR-(term1R*MR+term2R))/rhogC)/AC;
		//j2=((term1R*MR+term2R)/rholR+(MR-(term1R*MR+term2R))/rhogR)/AR;

		double coef1C=((1/rhogC)+term1R*(rhogC-rholC)/(rhogC*rholC))/AC;
		double coef2C=(term2R*(rhogC-rholC)/(rhogC*rholC))/AC;
		double coef1R=((1/rhogR)+term1R*(rhogR-rholR)/(rhogR*rholR))/AR;
		double coef2R=(term2R*(rhogR-rholR)/(rhogR*rholR))/AR;

		double rhomix1;
		double rhomix2;

		rhomix1=alf*rhogC+(1-alf)*rholC;
		rhomix2=alfR*rhogR+(1-alfR)*rholR;
		//rhomix1=alf*rhogC+(1-alf)*rholC;
		//rhomix2=alfR*rhogR+(1-alfR)*rholR;

		double viscmix1;
		double viscmix2;


		double CorrViscG=1.;
		double CorrViscL=1.;

		//if(arranjoR==3 && alf<1-1e-3)CorrViscL=1.+2.5*alf;
		//else if(arranjoR==2 && alf<1-1e-3){
			//CorrViscG=0.;
			//CorrViscL=1./(1.-alf);
		//}

		viscmix1=alf*flui.ViscGas(pres,temp)*CorrViscG+
				(1-alf)*(flui.ViscLiq(pres,temp))*CorrViscL;

		CorrViscG=1.;
		CorrViscL=1.;

		//if(arranjoR==3 && alfR<1-1e-3)CorrViscL=1.+2.5*alfR;
		//else if(arranjoR==2 && alfR<1-1e-3){
			//CorrViscG=0.;
			//CorrViscL=1./(1.-alfR);
		//}
		viscmix2=alfR*flui.ViscGas(presR,tempR)*CorrViscG+
				(1-alfR)*(flui.ViscLiq(presR,tempR))*CorrViscL;
		//viscmix1=alf*flui.ViscGas(pres,temp)+
				//(1-alf)*((1-bet)*flui.ViscOleo(pres,temp)+bet*fluicol.VisFlu(temp));
		//viscmix2=alfR*flui.ViscGas(presR,tempR)+
				//(1-alfR)*((1-betR)*flui.ViscOleo(presR,tempR)+betR*fluicol.VisFlu(tempR));


		double re1;
		double re2;

		if(dutoL.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
		else{
			double dhid=4* duto.area/duto.peri;
			re1 = Rey(dhid, j1, rhomix1,viscmix1);
		}
		if(dutoR.revest==0)re2 = Rey(dutoR.a, j2, rhomix2,viscmix2);
		else{
			double dhid=4* duto.area/duto.peri;
			re2 = Rey(dhid, j2, rhomix2,viscmix2);
		}
		double f1 = fric(re1, duto.rug / duto.a);
		double f2 = fric(re2, dutoR.rug / dutoR.a);
		if(duto.teta<0.1){
			f1*=1.;
			f2*=1.;
		}
		double razdx = dx / (dxR + dx);
		double coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC * razdx;
		double dpfric=coefTensC*coef2C;
		double coefTensR=0.5 * f2*rhomix2*( fabsl(j2)) * siR * (1.-razdx);
		dpfric+=coefTensR*coef2R;
		double dphidro=9.82 * sinl(duto.teta) *AC*rhomix1* razdx;
		dphidro+=9.82 * sinl(dutoR.teta) *AR*rhomix2* (1.-razdx);
		double estrat1=perdaEstratL*term1R*fabsl(MliqiniR)+perdaEstratG*(1-term1R)*fabsl(MR-MliqiniR);
		double estrat2=(perdaEstratL*fabsl(MliqiniR)-perdaEstratG*fabsl(MR-MliqiniR))*term2R;

		estrat1=0.;
		estrat2=0.;

		dpB=0.;
		potB=0.;
		double coefDpB=0.;
		if(acsr.tipo==4&&acsr.bcs.freqnova>1. && j1>=0.){
		     double vazmix=j1*AC;
		     double alf0=alf;
		     vazmix*=(86400/0.16);
		     //BomCentSub bombavisc=
		     acsr.bcs.NovaVis(viscmix1,rhomix1,vazmix);
		     //dpB=0.3048*bombavisc.Fhead(vazmix)*rhomix1*9.82;
		     //potB=(1.-bombavisc.Fefic(vazmix)/100.+100.*(1.-bombavisc.eficM/100.)/bombavisc.eficM)*
		    	  //bombavisc.Fpower(vazmix)*745.7;
		     dpB=0.3048*acsr.bcs.Hvis*rhomix1*9.82;
		     potB=(1.-acsr.bcs.Evis/100.+100.*(1.-acsr.bcs.eficM/100.)/acsr.bcs.eficM)*
		    		 acsr.bcs.Pvis*745.7;
		     double Cdpb=AC*(86400/0.16);
		     double djdm=(term1R/rholC+(1-term1R)/rhogC)/AC;
		     //double dpdvaz=0.3048*bombavisc.Fhead(vazmix*1.001)*rhomix1*9.82;
		     acsr.bcs.NovaVis(viscmix1,rhomix1,vazmix*1.001);
		     double dpdvaz=(0.3048*acsr.bcs.Hvis*rhomix1*9.82-dpB)/(0.001*vazmix);
		     coefDpB=Cdpb*djdm*dpdvaz;
		}
		else if(acsr.tipo==7) dpB=acsr.delp*98066.5;

		double delpChoke=0.;

		double sinalPig=1.;
		if(velPig<0.)sinalPig=1.;

		local[1][3] +=(coefTensC*coef1C+coefTensR*coef1R+estrat1);
		local[1][3] -=coefDpB*((dx*AC+dxR*AR)/(dx + dxR))/dx;
		TL[1]-=(dpfric+dphidro-(dpB+delpChoke-
				coefDpB*MR-(sinalPig*DelPig*velPig*98066.5))*((dx*AC+dxR*AR)/(dx + dxR))/dx+estrat2);

		cinematico=(fabsl(MR-MRini) / dt)/fabsl((Amed*98066.5/dxmed)*(presR-pres));
      }
      else{
    	compres = flui.ZFunc(pres, temp);
    	dzdp = flui.DZDPFunc(pres, temp);
    	dpdrho = rhogC* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
        dpdrho = 1. / dpdrho;
        multL=((1/rpC)*rpCi)/(rpCi);
        local[1][0] = 0.;
   	    local[1][1] = -((1./rhogC)*(1.-term1)+multL*term1);
   	    local[1][2] =  dx*(AC*(alf/rhogC)/dpdrho)*98066.5 / dt;
   	    local[1][3] = 0.;
   	    local[1][4] = 0.;

   	    TL[1] = (dx*AC*(alf/rhogC)/dpdrho)*pres*98066.5 / dt-(1/rhogC-multL)*term2+
   	    		(fontemassGR/rhogC+(fontemassLR/rpC))+dx*(1/rhogC-1/rpC)*transmassR;
   	    if(acsr.tipo==8){
   	    	double drhodp=1/dpdrho;
   	    	double drhodp1;
   	    	compres = flui.ZFunc(presR, tempR);
   	    	dzdp = flui.DZDTFunc(presR, tempR);
   	    	drhodp1 = rhogR* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
   	    	double correcBVol=-(-(1/rhogR)*drhodp*pres*98066.5+rhogC*drhodp1*presR*98066.5/(rhogR*rhogR));
   	    	local[1][2] +=(acsr.bvol.MultGas/rhogC+(acsr.bvol.MLiqP/rpC))*(1/rhogR)*drhodp*98066.5;
   	    	local[1][4] -= (acsr.bvol.MultGas/rhogC+(acsr.bvol.MLiqP/rpC))*rhogC*drhodp1*98066.5/(rhogR*rhogR);
   	    	TL[1] += (acsr.bvol.MultGas/rhogC+(acsr.bvol.MLiqP/rpC))*correcBVol;
   	    }


      }

   	}
	else if (posic == ncel) {
       if(masChkSup==0){
		 double AL =dutoL.area;
		 double rhogL = flui.MasEspGas(presL,tempL,TEMP);
		 double rholL = rpL;
	     double compres = flui.ZFunc(presL, tempL);
		 double dzdp = flui.DZDPFunc(presL, tempL);
		 double dpdrho = rhogL* (1 / (presL * 98066.5) - (1 / compres) * dzdp);
		 dpdrho = 1. / dpdrho;
		 double multL=((1/rpL)*rpLi)/(rpLi);
		 double multR=((1/rpL)*rpCi)/(rpCi);

		 double coluna=1.;
		 if(duto.teta<0.8*M_PI/2.)coluna=0.;

         local[0][0] = -((1./rhogL)*(1.-term1L)+multL*term1L);
	     local[0][1] = coluna*(dxL*AL*(alfL/rhogL)/dpdrho)*98066.5 / dt;
	     local[0][2] = (1./rhogL)*(1-term1)+multR*term1;
	     local[0][3] = 0.;
	     local[0][4] = 0.;

	     TL[0] = coluna*(dxL*AL*(alfL/rhogL)/dpdrho)*presL*98066.5 / dt+(1/rhogL-multR)*term2-(1/rhogL-multL)*term2L+
	        		(fontemassGL/rhogL+(fontemassLL/rpL)/rholL)+dxL*(1/rhogL-1/rpL)*transmassL;

		  local[1][0] = 0.;
		  local[1][1] = 0.;
		  local[1][2] = 1.;
		  local[1][3] = 0.;
		  local[1][4] = 0.;

		  TL[1] = presfim;

		  ////////////////////////////////////////////////////////////////////////////////////////////////////


	  }
       else{
  		 double AL =dutoL.area;
  		 double rhogL = flui.MasEspGas(presL,tempL,TEMP);
  		 double rholL = rpL;
  	     double compres = flui.ZFunc(presL, tempL);
  		 double dzdp = flui.DZDPFunc(presL, tempL);
  		 double dpdrho = rhogL* (1 / (presL * 98066.5) - (1 / compres) * dzdp);
  		 dpdrho = 1. / dpdrho;
		 double multL=((1/rpL)*rpLi)/(rpLi);
		 double multR=((1/rpL)*rpCi)/(rpCi);

  		 double coluna=1.;
  		 if(duto.teta<0.8*M_PI/2.)coluna=1.;

         local[0][0] = -((1./rhogL)*(1.-term1L)+multL*term1L);
  	     local[0][1] = coluna*(dxL*AL*(alfL/rhogL)/dpdrho)*98066.5 / dt;
  	     local[0][2] = (1./rhogL)*(1-term1)+multR*term1;
  	     local[0][3] = 0.;
  	     local[0][4] = 0.;

  	     TL[0] = coluna*(dxL*AL*(alfL/rhogL)/dpdrho)*presL*98066.5 / dt+(1/rhogL-multR)*term2-(1/rhogL-multL)*term2L+
	        		(fontemassGL/rhogL+(fontemassLL/rpL))+dxL*(1/rhogL-1/rpL)*transmassL;


  	      double dxncel=1.*dx;
  	      double AC =dutoR.area;
  		  double rhog = flui.MasEspGas(pres,temp,TEMP);
  		  double rhol = rpC;
  	      compres = flui.ZFunc(pres, temp);
  		  dzdp = flui.DZDPFunc(pres, temp);
  		  dpdrho = rhog* (1 / (pres * 98066.5) - (1 / compres) * dzdp);
  		  dpdrho = 1. / dpdrho;

  		  coluna=1.;
  		  if(duto.teta<0.8*M_PI/2.)coluna=1.;

  		  multL=((1/rpL)*rpCi)/(rpCi);

  	      local[1][0] = 0.;
  		  local[1][1] = -((1./rhog)*(1.-term1)+multL*term1);
  		  local[1][2] = coluna*(dxncel*AC*(alf/rhog)/dpdrho)*98066.5 / dt-
  				(DmasschokeG/rhog+DmasschokeL/rpC);
  		  local[1][3] = 0.;
  		  local[1][4] = 0.;

  		  TL[1] = coluna*(dxncel*AC*(alf/rhog)/dpdrho)*pres*98066.5 / dt
  				  -(1/rhog-multL)*term2+
	        	  ((fontemassGR-DmasschokeG*pres)/rhog+(fontemassLR-DmasschokeL*pres)/rpC);


       }

	}
	else if (posic == 0) {



		if(presE<0){//alteracao4

		  local[0][0] = 0.;
		  local[0][1] = 0.;
		  local[0][2] =1.;
		  local[0][3] = 0.;
		  local[0][4] = 0;

		  TL[0] =0.;
		}
		else{
			  double AC =duto.area;
			  double AR =dutoR.area;
			  double Amed =0.5*(AC+AR);
			  double siC=duto.peri;
			  double rhogC = flui.MasEspGas(0.5*(presE+pres),tempE, TEMP);
			  double rhogR = flui.MasEspGas(pres,temp,TEMP);
			  double rholC = flui.MasEspLiq(0.5*(presE+pres),tempE,TEMP);

			  double j1=(Mliqini/rholC+(MC-Mliqini)/rhogC)/AC;
			  double coef1C=((1/rhogC)+term1*(rhogC-rholC)/(rhogC*rholC))/AC;
			  double coef2C=(term2*(rhogC-rholC)/(rhogC*rholC))/AC;
			  double rhomix1=titE*rhogC+(1-titE)*rholC;
			  double viscmix1=titE*flui.ViscGas(0.5*(presE+pres),tempE)+
					(1-titE)*(flui.ViscLiq(0.5*(presE+pres),tempE));
			  double re1;
			  if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
			  else{
				  double dhid=4* duto.area/duto.peri;
				  re1 = Rey(dhid, j1, rhomix1,viscmix1);
			  }
			  double f1 = fric(re1, duto.rug / duto.a);
			  double coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC;
			  double dpfric=coefTensC*coef2C;
			  double dphidro=9.82 * sinl(duto.teta) *AC*rhomix1;

			  double ugl=(MC-Mliqini)/rhogC;
			  double ugr=(MR-MliqiniR)/rhogR;
			  double dxm=dx;
			  double coefacelera1L=(-(1-term1)*ugl+0.25*(ugl+ugr)*(1-term1))/dxm;
			  double coefacelar2L=(term2*ugl-0.25*(ugl+ugr)*term2)/dxm;
			  double coefacelera1R=(0.25*(ugl+ugr)*(1-term1R))/dxm;
			  double coefacelar2R=(-0.25*(ugl+ugr)*term2R)/dxm;

			  local[0][0] = 0.;
			  local[0][1] = 0.;
			  local[0][2] =coefTensC*coef1C+(1-term1) / dt;
			  local[0][3] = Amed*98066.5/(dx);
			  local[0][4] = 0;

			  TL[0] = (MC-Mliqini) / dt+ term2 / dt;

			  TL[0] +=(Amed*98066.5*presE/(dx)-(dpfric+dphidro));
		}

		  double AC =duto.area;
		  double AR =dutoR.area;
		  double siC=duto.peri;
		  double siR=dutoR.peri;
		  double rhogC = flui.MasEspGas(pres,temp,TEMP);
		  double rholC = rpC;
		  double rhogR = flui.MasEspGas(presR,tempR,TEMP);
		  double rholR = rpR;

		  double dxmed = 0.5 * (dx + dxR);
		  double Amed =0.5*(AC+AR);

		  local[1][0] = 0.;
		  local[1][1] = 0.;
		  local[1][2] = -Amed*98066.5/dxmed;
		  local[1][3] = (1-term1R) / dt;
		  local[1][4] = Amed*98066.5/dxmed;

		  TL[1] = (MR-MliqiniR) / dt+term2R/dt;

		  double j1;
		  double j2;

		  j1=(MliqiniR/rholC+(MR-MliqiniR)/rhogC)/AC;
		  j2=(MliqiniR/rholR+(MR-MliqiniR)/rhogR)/AR;

		  double coef1C=((1/rhogC)+term1R*(rhogC-rholC)/(rhogC*rholC))/AC;
		  double coef2C=(term2R*(rhogC-rholC)/(rhogC*rholC))/AC;
		  double coef1R=((1/rhogR)+term1R*(rhogR-rholR)/(rhogR*rholR))/AR;
		  double coef2R=(term2R*(rhogR-rholR)/(rhogR*rholR))/AR;

		  double rhomix1;
		  double rhomix2;

		  rhomix1=alf*rhogC+(1-alf)*rholC;
		  rhomix2=alfR*rhogR+(1-alfR)*rholR;

		  double viscmix1;
		  double viscmix2;


		  viscmix1=alf*flui.ViscGas(pres,temp)+
				(1-alf)*(flui.ViscLiq(pres,temp));
		  viscmix2=alfR*flui.ViscGas(presR,tempR)+
				(1-alfR)*(flui.ViscLiq(presR,tempR));
		  //viscmix1=alf*flui.ViscGas(pres,temp)+(1-alf)*flui.ViscOleo(pres,temp);
		  //viscmix2=alfR*flui.ViscGas(presR,tempR)+(1-alfR)*flui.ViscOleo(presR,tempR);

		  double re1;
		  double re2;

		  if(duto.revest==0)re1 = Rey(duto.a, j1, rhomix1,viscmix1);
		  else{
			  double dhid=4* duto.area/duto.peri;
			  re1 = Rey(dhid, j1, rhomix1,viscmix1);
		  }
		  if(dutoR.revest==0)re2 = Rey(dutoR.a, j2, rhomix2,viscmix2);
		  else{
			  double dhid=4* duto.area/duto.peri;
			  re2 = Rey(dhid, j2, rhomix2,viscmix2);
		  }
		  double f1 = fric(re1, duto.rug / duto.a);
		  double f2 = fric(re2, dutoR.rug / dutoR.a);
		  double razdx = dx / (dxR + dx);
		  double coefTensC=0.5 * f1*rhomix1*( fabsl(j1)) * siC * razdx;
		  double dpfric=coefTensC*coef2C;
		  double coefTensR=0.5 * f2*rhomix2*( fabsl(j2)) * siR * (1.-razdx);
		  dpfric+=coefTensR*coef2R;
		  double dphidro=9.82 * sinl(duto.teta) *AC*rhomix1* razdx;
		  dphidro+=9.82 * sinl(dutoR.teta) *AR*rhomix2* (1.-razdx);

		  local[1][3] +=(coefTensC*coef1C+coefTensR*coef1R);
		  TL[1]-=(dpfric+dphidro);


	}

}

 void CelVap::avancPig(int& reinicia){
  //double AC =duto.area;
  //double rhogR = flui.MasEspGas(presR,tempR);
  //double jL=(QL+QG)/AC;
  //double jR=(QLR+((MR-MliqiniR)/rhogR))/AC;
  //velPig=jL*razPig+jR*(1.-razPig);
  razPig=(razPigini*dx+velPig*dt)/dx;

  if(((razPig<=(*vg1dSP).localtiny)&&(razPig>=-(*vg1dSP).localtiny))) razPig=0;
  else if(razPig<-(*vg1dSP).localtiny){
     double dtaux;
     dtaux=-razPigini*dx/velPig;
     if(dtaux>(*vg1dSP).localtiny){
       dtPig=dtaux;
       reinicia=-1;
       razPig=fabsl(0.);
     }
     else razPig=fabsl(0.);
  }
  else if((razPig>=(1.-(*vg1dSP).localtiny)&&razPig<=(1.+(*vg1dSP).localtiny))){
	   razPig=1.;
  }
  else if(razPig>(1.+(*vg1dSP).localtiny)){
     double dtaux;
     dtaux=(1.-razPigini)*dx/velPig;
     if(dtaux>(*vg1dSP).localtiny){
       dtPig=dtaux;
       reinicia=-1;
       razPig=1.;
     }
     else razPig=1.;
  }

}


 void CelVap::avancalfPig(){
   double term1Mass=0.;
   if(alf<=(*vg1dSP).localtiny){
	   alfPigD=0.;
	   alfPigE=0.;
   }
   else if(alf>=(1.-(*vg1dSP).localtiny)){
	   alfPigD=1.;
	   alfPigE=1.;
   }
   else{
	   if(velPig>=0.){

        if(RazAreaPig>(*vg1dSP).localtiny){//alteracao2
    	 double areagarg=RazAreaPig*duto.area;
    	 double rlm=(1.-alfPigEini)*(rpC)+
    			          alfPigEini*flui.MasEspGas(pres,temp,TEMP);
     	 double massica=areagarg*sqrtl(2.*rlm*(DelPig*velPig)*98066.52);
   		 massica=cdpig*massica;
   		 VazaPig=massica/rlm;
        }//alteracao2
        else VazaPig=0.;//alteracao2
        double fontepigL=VazaPig*(1.-alfPigEini)*rpC;//alteracao2



	    if(razPig<=(1.-(*vg1dSP).localtiny)){
	     double rlm=rpC;

	     double fator1=1.;
	     fator1=1./fator1;

	     double razdx1=dxR/(dx+dxR);
	     double tmed1=razdx1*tempR+(1-razdx1)*temp;

	     double convec=fator1*(MliqiniR-rlm*(1-alfPigDini)*velPig*duto.area)/((1.-razPig)*dx)
				 -fator1*(fontepigL/((1.-razPig)*dx));//alteracao2
	     //double convec=0.;

	     if(razPig<0.5)term1Mass=(convec-fator1*(fontemassLR/((1.-razPig)*dx)-transmassR))/(rlm*duto.area);
	     else term1Mass=(convec+fator1*transmassR)/(rlm*duto.area);
	     //term1Mass=(convec-fator1*(fontemassLR/(dx)+(1.-fator2)*fontemassCR/(dx)-transmassR))/(rlm*duto.area);
	     alfPigD=alfPigDini+term1Mass*dt;
	     if(alfPigD<0.)alfPigD=0.;
	     if(alfPigD>1.)alfPigD=1.;
	     alfPigE=(-alfPigD*(1.-razPig)+alf)/razPig;
	     if(alfPigE<0.)alfPigE=0.;
	     if(alfPigE>1.)alfPigE=1.;
	   }
	   else{
		   alfPigD=alfPigDini;
		   alfPigE=alf;
	   }
   }
   else{

       if(RazAreaPig>(*vg1dSP).localtiny){//alteracao2
   	     double areagarg=RazAreaPig*duto.area;
   	     double rlm=(alfPigDini)*(rpC)+
		                  alfPigDini*flui.MasEspGas(pres,temp,TEMP);
    	 double massica=areagarg*sqrtl(2.*rlm*(DelPig*fabsl(velPig))*98066.52);
  		 massica=cdpig*massica;
  		 VazaPig=-massica/rlm;
       }//alteracao2
       else VazaPig=0.;//alteracao2
       double fontepigL=-VazaPig*(1.-alfPigDini)*rpC;//alteracao2

	   if(razPig>=(*vg1dSP).localtiny){

	     double rlm=rpC;

	     double fator1=1.;
	     fator1=1./fator1;

	     double razdx0=dx/(dx+dxL);
	     double tmed0=razdx0*temp+(1-razdx0)*tempL;
	     double convec=fator1*(rlm*(1.-alfPigEini)*velPig*duto.area-Mliqini)/(razPig*dx)
	                     -fator1*(fontepigL/(razPig*dx));//alteracao2
	     //double convec=0.;

	     if(razPig>=0.5)term1Mass=(convec-fator1*(fontemassLR/(razPig*dx)-
	    		                   transmassR))/(rlm*duto.area);
	     else term1Mass=(convec+fator1*transmassR)/(rlm*duto.area);
	     //term1Mass=(convec-fator1*(fontemassLR/(dx)+(1.-fator2)*fontemassCR/(dx)-transmassR))/(rlm*duto.area);
	     alfPigE=alfPigEini+term1Mass*dt;
	     if(alfPigE<0.)alfPigE=0.;
	     if(alfPigE>1.)alfPigE=1.;
	     alfPigD=(-alfPigE*razPig+alf)/(1-razPig);
	     if(alfPigD<0.)alfPigD=0.;
	     if(alfPigD>1.)alfPigD=1.;
     }
	 else{
		   alfPigD=alf;
		   alfPigE=alfPigEini;
	 }
   }
   }
}

//template class CelVap;
