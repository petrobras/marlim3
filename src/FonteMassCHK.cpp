/*
 * FonteMassCHK.cpp
 *
 *  Created on: 13/07/2017
 *      Author: bwz1
 */
#include "FonteMassCHK.h"

fontemaschk::fontemaschk(double vmasP,double vmasC,double vmasG,double vtitT,double vtitamb,
		   double vcd, double vabertura, double varea, double vbet,
		   double vbetIST,double vbetISamb, double vtempT, double vpresT,
		   double vtamb, double vpamb,ProFlu vfluidoP, ProFlu vfluidoPamb,ProFluCol vfluidoC, int vcheck,
		   int vambGas){

 masP=vmasP;
 masC=vmasC;
 masG=vmasG;
 titT=vtitT;
 titamb=vtitamb;
 abertura=vabertura;
 area=varea;
 tempT=vtempT;
 presT=vpresT;
 tamb=vtamb;
 pamb=vpamb;
 bet=vbet;
 betIST=vbetIST;
 betISamb=vbetISamb;
 fluidoP=vfluidoP;
 fluidoPamb=vfluidoPamb;
 fluidocol=vfluidoC;
 cd=vcd;
 chk=choke(area, abertura*area);
 chk.cdchk=cd;
 check=vcheck;
 ambGas=vambGas;
}

fontemaschk::fontemaschk(){

 masP=0.;
 masC=0.;
 masG=0.;
 titT=0.;
 titamb=0.;
 abertura=0.;
 area=0.;
 tempT=0.;
 presT=0.;
 tamb=0.;
 pamb=0.;
 bet=0.;
 betIST=0.;
 betISamb=0.;
 cd=0.84;
 fluidoP=ProFlu();
 fluidoPamb=ProFlu();
 fluidocol=ProFluCol();
 chk=choke(1.,1.);
 check=0;
 ambGas=0;
}

fontemaschk::fontemaschk(const fontemaschk& vinj){//construtor de c�pia
	 masP=vinj.masP;
	 masC=vinj.masC;
	 masG=vinj.masG;
	 titT=vinj.titT;
	 titamb=vinj.titamb;
	 abertura=vinj.abertura;
	 area=vinj.area;
	 tempT=vinj.tempT;
	 presT=vinj.presT;
	 tamb=vinj.tamb;
	 pamb=vinj.pamb;
	 bet=vinj.bet;
	 betIST=vinj.betIST;
	 betISamb=vinj.betISamb;
	 cd=vinj.cd;
	 fluidoP=vinj.fluidoP;
	 fluidoPamb=vinj.fluidoPamb;
	 fluidocol=vinj.fluidocol;
	 chk=vinj.chk;
	 check=vinj.check;
	 ambGas=vinj.ambGas;
}

fontemaschk& fontemaschk::operator =(const fontemaschk& vinj){
	 masP=vinj.masP;
	 masC=vinj.masC;
	 masG=vinj.masG;
	 titT=vinj.titT;
	 titamb=vinj.titamb;
	 abertura=vinj.abertura;
	 area=vinj.area;
	 tempT=vinj.tempT;
	 presT=vinj.presT;
	 tamb=vinj.tamb;
	 pamb=vinj.pamb;
	 bet=vinj.bet;
	 betIST=vinj.betIST;
	 betISamb=vinj.betISamb;
	 fluidoP=vinj.fluidoP;
	 fluidoPamb=vinj.fluidoPamb;
	 fluidocol=vinj.fluidocol;
	 chk=vinj.chk;
	 check=vinj.check;
	 ambGas=vinj.ambGas;

  return *this;
}

void fontemaschk::VMas(){
	double ypres;
	double tfonte;
	double pfonte;
	double titfonte;
	double betfonte;
	ProFlu fluidoPfonte;
	double maxSup;
	double sinal;
	if(presT>pamb){
		ypres=pamb/presT;
		tfonte=tempT;
		pfonte=presT;
		titfonte=titT;
		betfonte=betIST;
		fluidoPfonte=fluidoP;
		sinal=-1.;
		if(check==1)sinal=0;
	}
	else{
		ypres=presT/pamb;
		tfonte=tamb;
		pfonte=pamb;
		titfonte=titamb;
		if(ambGas==1)titfonte=1.;
		betfonte=betISamb;
		fluidoPfonte=fluidoPamb;
		sinal=1.;
		if(check==-1)sinal=0;
	}

	if(sinal!=0){
		double masChk=chk.vazmassSachd(ypres,pfonte, tfonte,titfonte,betfonte,titfonte, fluidoPfonte,fluidocol);
		maxSup=chk.vazmaxSachd(pfonte, tfonte, titfonte,betfonte,titfonte, fluidoPfonte,fluidocol);
		int fluxcri=1;
		if(titfonte<=0.01 || fabsl(ypres)>fabsl(chk.razpres)){
	         	 fluxcri=0;
	         	 maxSup=masChk;
		}
		if(chk.AreaGarg<(1e-5)*area ) maxSup=0.;
		double masliq=maxSup*(1.-titfonte);
		masG=sinal*maxSup*titfonte;
		double rholP=fluidoPfonte.MasEspLiq(pfonte,tfonte);
		double rholC=fluidocol.MasEspFlu(pfonte,tfonte);
		double rholmix=betfonte*rholC+(1.-betfonte)*rholP;
		double qlinsit=masliq/rholmix;
		masP=sinal*(1.-betfonte)*rholP*qlinsit;
		masC=sinal*betfonte*rholC*qlinsit;
	}
	else{
		masG=0.;
		masP=0.;
		masC=0.;
	}


}

//template class fontemaschk;

