/*
 * multiBCS.cpp
 *
 *  Created on: 6 de abr. de 2026
 *      Author: Eduardo
 */

#include "multiBCS.h"

multiBomCentSub::multiBomCentSub(int vnBCS ,BomCentSub* BCStemp ,int* nestagParcFabtemp ,
		int* nestagParctemp, double Vfreq, double VeficM,
			    double vfreqMinima, int vcorrecHI,int vequilTerm,double vfracTermMotorEfic,
	            ProFlu vflui, ProFluCol vfluiCol){//construtor

	nBCS=vnBCS;
	equilTerm=vequilTerm;
	if(nBCS>0){
		BCSinterno= new BomCentSub[nBCS];
		nestagParcFab=new int[nBCS];
		nestagParc= new int[nBCS];
		nestag=0;
		flui=vflui;
		fluicol=vfluiCol;
		for(int i=0; i<nBCS;i++){
			nestagParcFab[i]=nestagParcFabtemp[i];
			nestagParc[i]=nestagParctemp[i];
			BCSinterno[i]=BCStemp[i];
			nestag+=nestagParc[i];
		}
        freq=Vfreq;
        freqnova=freq;
        eficM=VeficM;
        freqMinima=vfreqMinima;
        correcHI=vcorrecHI;
        fracTermMotorEfic=vfracTermMotorEfic;

    	dpB=0.;
    	potB=0.;
    	potBT=0.;
    	potTermo=0.;
       }
       else{
    	nBCS=0;
    	BCSinterno=0;
    	nestagParcFab=0;
    	nestagParc=0;
    	nestag=0;
        freq=0;
        freqnova=freq;
        freqMinima=0.;
        eficM=0.;
        correcHI=0;
        fracTermMotorEfic=0.;

    	dpB=0.;
    	potB=0.;
    	potBT=0.;
    	potTermo=0.;
       }
}
multiBomCentSub::multiBomCentSub(const multiBomCentSub& bombaantiga){
//construtor de c�pia
	nBCS=bombaantiga.nBCS;
	equilTerm=bombaantiga.equilTerm;
	if(nBCS>0){
		BCSinterno= new BomCentSub[nBCS];
		nestagParcFab=new int[nBCS];
		nestagParc= new int[nBCS];
		nestag=bombaantiga.nestag;
		flui=bombaantiga.flui;
		fluicol=bombaantiga.fluicol;
		for(int i=0; i<nBCS;i++){
			nestagParcFab[i]=bombaantiga.nestagParcFab[i];
			nestagParc[i]=bombaantiga.nestagParc[i];
			BCSinterno[i]=bombaantiga.BCSinterno[i];
		}
        freq=bombaantiga.freq;
        freqnova=bombaantiga.freqnova;
        eficM=bombaantiga.eficM;
        freqMinima=bombaantiga.freqMinima;
        correcHI=bombaantiga.correcHI;
        fracTermMotorEfic=bombaantiga.fracTermMotorEfic;

    	dpB=bombaantiga.dpB;
    	potB=bombaantiga.potB;
    	potBT=bombaantiga.dpB;
    	potTermo=bombaantiga.potTermo;
       }
       else{
    	nBCS=0;
    	BCSinterno=0;
    	nestagParcFab=0;
    	nestagParc=0;
    	nestag=0;
        freq=0;
        freqnova=freq;
        freqMinima=0.;
        eficM=0.;
        correcHI=0;
        fracTermMotorEfic=0.;

       	dpB=0.;
        potB=0.;
        potBT=0.;
        potTermo=0.;
       }

}

multiBomCentSub& multiBomCentSub::operator =(const multiBomCentSub& bombaantiga){
	if(this!=&bombaantiga){
			  if(nBCS>0 && nestagParcFab!=0)delete[] nestagParcFab;
			  if(nBCS>0 && nestagParc!=0)delete[] nestagParc;
			  if(nBCS>0 && BCSinterno!=0)delete[] BCSinterno;
			  nBCS=bombaantiga.nBCS;
			  equilTerm=bombaantiga.equilTerm;
			  if(nBCS>0){
					BCSinterno= new BomCentSub[nBCS];
					nestagParcFab=new int[nBCS];
					nestagParc= new int[nBCS];
					nestag=bombaantiga.nestag;
					flui=bombaantiga.flui;
					fluicol=bombaantiga.fluicol;
					for(int i=0; i<nBCS;i++){
						nestagParcFab[i]=bombaantiga.nestagParcFab[i];
						nestagParc[i]=bombaantiga.nestagParc[i];
						BCSinterno[i]=bombaantiga.BCSinterno[i];
					}
			        freq=bombaantiga.freq;
			        freqnova=bombaantiga.freqnova;
			        eficM=bombaantiga.eficM;
			        freqMinima=bombaantiga.freqMinima;
			        correcHI=bombaantiga.correcHI;
			        fracTermMotorEfic=bombaantiga.fracTermMotorEfic;

			    	dpB=bombaantiga.dpB;
			    	potB=bombaantiga.potB;
			    	potBT=bombaantiga.dpB;
			    	potTermo=bombaantiga.potTermo;
			  }
			  else{
			    	nBCS=0;
			    	BCSinterno=0;
			    	nestagParcFab=0;
			    	nestagParc=0;
			    	nestag=0;
			        freq=0;
			        freqnova=freq;
			        freqMinima=0.;
			        eficM=0.;
			        correcHI=0;
			        fracTermMotorEfic=0.;

			       	dpB=0.;
			        potB=0.;
			        potBT=0.;
			        potTermo=0.;
			   }
	}
    return *this;
}


void multiBomCentSub::marchaMultiBcs(double vazG, double vazL, double pres, double temp, double alfa, double beta){
	double xvaz = (86400 / 0.1589876)*(vazG+vazL);
	double cpl;
	double rhol;
	double cpg;
	double rhog;
    double rhomis;
    double vismis;
    dpB=0.;
    double vpotB=0.;
    potBT=0.;
    double coefdxT;
    double vazLIni=vazL;
    double vazGIni=vazG;
    double tempIni=temp;
    double presIni=pres;
    double betaIni=beta;

    ProFlu fluiL=flui;
    ProFlu fluiG=flui;

    double titTermo= fluiL.FracMassHidra(pres, temp);

	double cplIni = (1. - beta) * fluiL.CalorLiq(pres, temp)
    		  + beta * fluicol.CalorLiq(pres, temp);
	double cpgIni = fluiG.CalorGas(pres, temp);

	double rhocIni=fluicol.MasEspFlu(pres, temp);
	double rholIni=(1 - beta) *fluiL.MasEspLiq(pres, temp)
                    + beta * rhocIni;
    double rhogIni=  fluiG.MasEspGas(pres, temp);

	double rhomisIni = (alfa * fluiG.MasEspGas(pres, temp)
			+ (1 - alfa)* ((1 - beta) *fluiL.MasEspLiq(pres, temp)
            + beta * fluicol.MasEspFlu(pres, temp)));

	double vismisIni = alfa * fluiG.ViscGas(pres, temp)
    		+ (1 - alfa)* ((1. - beta) * fluiL.ViscOleo(pres, temp)
            + beta * fluicol.VisFlu(pres, temp));

	double titulo=alfa*rhogIni/(rhomisIni);

	double boRef;
	if(fluiL.RGO<1e6)boRef= fluiL.BOFunc(pres, temp);
    else boRef=1.;
    double baRef= fluiL.BAFunc(pres, temp);
    double fwRef = fluiL.BSW*baRef / (boRef + baRef*fluiL.BSW - fluiL.BSW * boRef);

    if(equilTerm==0 && flui.flashCompleto!=1 && flui.flashCompleto!=2){
    	double rgoNovo=flui.RS(pres, temp)*6.29/35.31467;
    	flui.rzDegL(pres, temp);
    	flui.razDegD(pres, temp);
    	double razDengL=flui.rDgL;
    	double razDengD=flui.rDgD;
    	fluiG=ProFlu(flui.vg1dSP, 60, 10000000000000.0, razDengL*flui.Deng, 0., 1., 10., 10.,
				50., 50.,0, 0, 0,0, 0, 0,flui.yco2,
				flui.corrC);
    	fluiL=flui;
    	fluiL.tab=0;
    	fluiL.RGO=rgoNovo;
    	fluiL.Deng=razDengD*flui.Deng;
    	fluiL.RenovaFluido();
    }

	cpl=cplIni;
	cpg=cpgIni;
	rhol=rholIni;
	rhog=rhogIni;
	rhomis=rhomisIni;
	vismis=vismisIni;

	double delTit;
	double novoTit;

    for(int iCurva=0; iCurva<nBCS; iCurva++){
    	for(int iEstag=0; iEstag<nestagParc[iCurva]; iEstag++){

    		BCSinterno[iCurva].NovaVis(vismis, rhomis, xvaz);
    	    dpB = 0.3048 * BCSinterno[iCurva].Hvis * rhomis * 9.82;

    	    pres+=dpB/98066.5;

    	    vpotB = BCSinterno[iCurva].Pvis * 745.7;
    	    potB+=vpotB;
    	    coefdxT = (rhol * vazL * cpl + rhog * vazG * cpg);

    	    temp+=(1.-BCSinterno[iCurva].Evis/100.)*vpotB/coefdxT;

    	    if(flui.flashCompleto==2 && equilTerm==1)
    	    	flui.atualizaPropComp(pres, temp,flui.dCalculatedBeta,flui.oCalculatedLiqComposition,
    	    						flui.oCalculatedVapComposition,0);

    	    double tit0= fluiL.FracMassHidra(pres, temp);

    		cpl = (1. - beta) * fluiL.CalorLiq(pres, temp)
    	    		  + beta * fluicol.CalorLiq(pres, temp);
    		cpg = fluiG.CalorGas(pres, temp);

    		double rhoOA=fluiL.MasEspLiq(pres, temp);
    		double rhoc=fluicol.MasEspFlu(pres, temp);
            rhog=  fluiG.MasEspGas(pres, temp);


            double boI;
            if(fluiL.RGO<1e6)boI= fluiL.BOFunc(pres, temp);
            else boI=1.;
            double baI= fluiL.BAFunc(pres, temp);
            double fwI = fluiL.BSW*baI / (boI + baI*fluiL.BSW - fluiL.BSW * boI);
            double qo = vazLIni * (1 - fwRef) * (1 - betaIni) * boRef / boI;
            double qw;
            if (fwI < (1 - 1e-10)) qw = fwI * qo / (1 - fwI);
            else qw = vazLIni * (1 - betaIni);
            double qc;
            if(fluiL.RGO<1e7)
           	 qc= vazLIni * (betaIni) * rhocIni/ rhoc;
            else qc=0.;

            if((fabs(qo) + fabs(qw) + fabs(qc))>0)
           	 beta = fabs(qc) / (fabs(qo) + fabs(qw) + fabs(qc));
            else beta =0.;

            if(flui.flashCompleto==1 || equilTerm==1){
            	delTit=tit0-titTermo;
            	novoTit=titulo+delTit*(titulo/titTermo);
            }
            else{
            	novoTit=titulo;
            }
    	    alfa = novoTit*(rhoOA*(1.-beta)+rhoc*beta)/(rhog*(1.-novoTit)+
    	    		novoTit*(rhoOA*(1.-beta)+rhoc*beta));


    		rhol=(1 - beta) *rhoOA + beta * rhoc;
    	    rhomis = (alfa * rhog+ (1 - alfa)* ((1 - beta) *rhoOA + beta * rhoc));

    		vismis = alfa * fluiG.ViscGas(pres, temp)
            		+ (1 - alfa)* ((1. - beta) * fluiL.ViscOleo(pres, temp)
                    + beta * fluicol.VisFlu(pres, temp));
    		vazL=qo+qw+qc;
    		vazG=(vazLIni*rholIni+vazGIni*rhogIni-vazL*rhol)/rhog;
    		xvaz = (86400 / 0.1589876)*(vazG+vazL);
    	}
    }
    dpB=pres-presIni;
    coefdxT = (rholIni * vazLIni * cplIni + rhogIni * vazGIni * cpgIni);
    potTermo=1*(temp-tempIni)*coefdxT;
    if(eficM>0.)
    potBT = (1. + 100. * (1. - eficM / 100.) / eficM) * potB;
    else potBT = 0.;
}
