/*
 * TrocaCalor.cpp
 *
 *  Created on: 24/04/2014
 *      Author: bwz1
 */

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
using namespace std;
#include <stdlib.h>
#include "Vetor.h"
#include "Matriz.h"
#include "PorosoRad.h"

PorosRad::PorosRad(){

		iniciaVariaveis();
}

PorosRad::PorosRad(const DadosGeoPoro vgeom,varGlob1D* Vvg1dSP,const int vpermanente, const int* const vncamada,
		                                const double* const vdrcamada,double** const vPcamada,const double vPint,
		                                const double vpresRes,const double vtRes, const double vdt,
		                                const double pres, const double vzD0, const double vzD1, const double vdh,
										double** const vsL,double** const vsW,const double vsLRes,const double vsWRes,
										const double vsWPoc,
										const double vsatConata,const double vip,
										tabelaPemRelOA vkRelOA,tabelaPemRelOG vkRelOG,tabelaPresCapOA vpcOA,tabelaPresCapGO vpcGO,
										ProFluCol vfluc, ProFlu vflup){

	iniciaVariaveis();

	vg1dSP=Vvg1dSP;
	geom=vgeom;
	permanente=vpermanente;
	lenth=geom.ncamadas;
	he=0.;
	hi=0.;

	fluxIni=0.;
	fluxFim=0.;
	fluxIniA=0.;
	fluxFimA=0.;
	fluxIniG=0.;
	fluxFimG=0.;
    resGlob=0.;
    resFim=0.000001;

    condiTparede=0;

    zD0=vzD0;
    zD1=vzD1;


	dh=vdh;
	dtL=0.;

	fluc=vfluc;
	flup=vflup;

	tRes=vtRes;
	sLRes=vsLRes;
	sWRes=vsWRes;
	sWPoc=vsWPoc;

	kRelOA=vkRelOA;
	kRelOG=vkRelOG;
	pcOA=vpcOA;
	pcGO=vpcGO;

	satConata=vsatConata;
	ip=vip;

	if(lenth>0){
	  ncamada= new int [lenth];
	  for(int i=0;i<lenth;i++)ncamada[i]=vncamada[i];
	  drcamada=new double [lenth];
	  for(int i=0;i<lenth;i++)drcamada[i]=vdrcamada[i];

	  ncel=1;
	  for(int icam=0;icam<geom.ncamadas;icam++){
		  for(int idisc=1;idisc<=ncamada[icam];idisc++){
			  ncel++;
		  }
	  }

	  nglobal=1;
	  for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

	  Pint=vPint;
	  presRes=vpresRes;
	  presRes2=presRes;
      dt=vdt;

      geraCel(vPcamada,vsL,vsW,vsLRes,vsWRes);


	}
	else{
		lenth=0;
		ncamada=0;
		drcamada=0;
		Pint=0;
		presRes=0;
		presRes2=0.;
		tRes=0;
	    dt=0;
	    nglobal=0;
	    celula=0;
	}


}

PorosRad::PorosRad(const PorosRad& vPorosRad){

	iniciaVariaveis();
	vg1dSP=vPorosRad.vg1dSP;
	ncel=vPorosRad.ncel;
	geom=vPorosRad.geom;
	permanente=vPorosRad.permanente;
	lenth=geom.ncamadas;

	resFim=vPorosRad.resFim;

	condiTparede=vPorosRad.condiTparede;

	dh=vPorosRad.dh;
	dtL=vPorosRad.dtL;

	he=vPorosRad.he;
	hi=vPorosRad.hi;

	fluc=vPorosRad.fluc;
	flup=vPorosRad.flup;

    zD0=vPorosRad.zD0;
    zD1=vPorosRad.zD1;

	tRes=vPorosRad.tRes;
	sLRes=vPorosRad.sLRes;
	sWRes=vPorosRad.sWRes;
	sWPoc=vPorosRad.sWPoc;

	kRelOA=vPorosRad.kRelOA;
	kRelOG=vPorosRad.kRelOG;
	pcOA=vPorosRad.pcOA;
	pcGO=vPorosRad.pcGO;
	BSW=vPorosRad.BSW;

	satConata=vPorosRad.satConata;
	ip=vPorosRad.ip;

	if(lenth>0){
		ncamada= new int [lenth];
		for(int i=0;i<lenth;i++)ncamada[i]=vPorosRad.ncamada[i];
		drcamada=new double [lenth];
		for(int i=0;i<lenth;i++)drcamada[i]=vPorosRad.drcamada[i];

		 nglobal=1;
		 for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

		Pint=vPorosRad.Pint;
		presRes=vPorosRad.presRes;
		presRes2=vPorosRad.presRes2;
		tRes=vPorosRad.tRes;
	    dt=vPorosRad.dt;

		fluxIni=vPorosRad.fluxIni;
		fluxFim=vPorosRad.fluxFim;
		fluxIniA=vPorosRad.fluxIniA;
		fluxFimA=vPorosRad.fluxFimA;
		fluxIniG=vPorosRad.fluxIniG;
		fluxFimG=vPorosRad.fluxFimG;
		resGlob=vPorosRad.resGlob;

		DfluxIni=vPorosRad.DfluxIni;
		DfluxIniA=vPorosRad.DfluxIniA;
		DfluxIniG=vPorosRad.DfluxIniG;

		pBolha=vPorosRad.pBolha;
		tipoModelOleo=vPorosRad.tipoModelOleo;
		compOleo=vPorosRad.compOleo;
		rhoRef=vPorosRad.rhoRef;

		celula=new celrad [ncel];
		for(int i=0;i<ncel;i++)celula[i]=vPorosRad.celula[i];

	}
	else{
		lenth=0;
		ncamada=0;
		drcamada=0;
		Pint=0;
		presRes=0;
		presRes2=0.;
		tRes=0;
		dt=0;
		nglobal=0;

		fluxIni=0.;
		fluxFim=0.;
		fluxIniA=0.;
		fluxFimA=0.;
		fluxIniG=0.;
		fluxFimG=0.;
		resGlob=0.;
		celula=0;

		DfluxIni=0;
		DfluxIniA=0;
		DfluxIniG=0;

	}

}

PorosRad& PorosRad::operator =(const PorosRad& vPorosRad){

	if(lenth>0){
	  if(this!=&vPorosRad){
		 vg1dSP=vPorosRad.vg1dSP;
		 ncel=vPorosRad.ncel;
	     geom=vPorosRad.geom;
	     permanente=vPorosRad.permanente;
	 	 resFim=vPorosRad.resFim;

	 	condiTparede=vPorosRad.condiTparede;
		he=vPorosRad.he;
		hi=vPorosRad.hi;
		fluc=vPorosRad.fluc;
		flup=vPorosRad.flup;

	    zD0=vPorosRad.zD0;
	    zD1=vPorosRad.zD1;


	 	dh=vPorosRad.dh;
	 	dtL=vPorosRad.dtL;

		tRes=vPorosRad.tRes;
		sLRes=vPorosRad.sLRes;
		sWRes=vPorosRad.sWRes;
		sWPoc=vPorosRad.sWPoc;

		kRelOA=vPorosRad.kRelOA;
		kRelOG=vPorosRad.kRelOG;
		pcOA=vPorosRad.pcOA;
		pcGO=vPorosRad.pcGO;

		satConata=vPorosRad.satConata;
		ip=vPorosRad.ip;

	     if(lenth!=vPorosRad.lenth)cout<<"dimensoes de vetores diferem em número de camadas, impossivel igualar na PorosRad";
		 for(int i=0;i<lenth;i++)drcamada[i]=vPorosRad.drcamada[i];
		 for(int i=0; i<lenth; i++){
	       if(ncamada[i]>0){
	    	  if(ncamada[i]!=vPorosRad.ncamada[i])cout<<"dimensoes de vetores diferem em número de discretizações, impossivel igualar na PorosRad";
		    }
		 }
		 for(int i=0; i<lenth; i++){
	       if(ncamada[i]>0){
	    	  if(ncamada[i]!=vPorosRad.ncamada[i])cout<<"dimensoes de vetores diferem em número de discretizações, impossivel igualar na PorosRad";
		    }
		 }

		 nglobal=1;
		 for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

		 Pint=vPorosRad.Pint;
		 presRes=vPorosRad.presRes;
		 presRes2=vPorosRad.presRes2;
	     tRes=vPorosRad.tRes;
	     dt=vPorosRad.dt;

	     pBolha=vPorosRad.pBolha;
	     tipoModelOleo=vPorosRad.tipoModelOleo;
	     compOleo=vPorosRad.compOleo;
	     rhoRef=vPorosRad.rhoRef;
	     BSW=vPorosRad.BSW;

		 for(int i=0;i<ncel;i++)celula[i]=vPorosRad.celula[i];


      }
	}
	else if(lenth==0){
		if(this!=&vPorosRad){
		  vg1dSP=vPorosRad.vg1dSP;
		  ncel=vPorosRad.ncel;
		  geom=vPorosRad.geom;
		  permanente=vPorosRad.permanente;
		  lenth=geom.ncamadas;
		  resFim=vPorosRad.resFim;

		  condiTparede=vPorosRad.condiTparede;



		  dh=vPorosRad.dh;
	      dtL=vPorosRad.dtL;
	      he=vPorosRad.he;
	      hi=vPorosRad.hi;
	      fluc=vPorosRad.fluc;
	      flup=vPorosRad.flup;

	      zD0=vPorosRad.zD0;
	      zD1=vPorosRad.zD1;

	      tRes=vPorosRad.tRes;
	      sLRes=vPorosRad.sLRes;
	      sWRes=vPorosRad.sWRes;
	      sWPoc=vPorosRad.sWPoc;

	  	  kRelOA=vPorosRad.kRelOA;
	  	  kRelOG=vPorosRad.kRelOG;
	  	  pcOA=vPorosRad.pcOA;
	  	  pcGO=vPorosRad.pcGO;

	  	  satConata=vPorosRad.satConata;
	  	  ip=vPorosRad.ip;

	  	  pBolha=vPorosRad.pBolha;
	  	  tipoModelOleo=vPorosRad.tipoModelOleo;
	  	  compOleo=vPorosRad.compOleo;
	  	  rhoRef=vPorosRad.rhoRef;
	  	  BSW=vPorosRad.BSW;

		  if(lenth>0){
			 ncamada= new int [lenth];
			 for(int i=0;i<lenth;i++)ncamada[i]=vPorosRad.ncamada[i];
			 drcamada=new double [lenth];
			 for(int i=0;i<lenth;i++)drcamada[i]=vPorosRad.drcamada[i];


		    nglobal=1;
		    for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

			Pint=vPorosRad.Pint;
			presRes=vPorosRad.presRes;
			presRes2=vPorosRad.presRes2;
			tRes=vPorosRad.tRes;
			dt=vPorosRad.dt;
			fluxIni=vPorosRad.fluxIni;
			fluxFim=vPorosRad.fluxFim;
			fluxIniA=vPorosRad.fluxIniA;
			fluxFimA=vPorosRad.fluxFimA;
			fluxIniG=vPorosRad.fluxIniG;
			fluxFimG=vPorosRad.fluxFimG;
			resGlob=vPorosRad.resGlob;

			DfluxIni=vPorosRad.DfluxIni;
			DfluxIniA=vPorosRad.DfluxIniA;
			DfluxIniG=vPorosRad.DfluxIniG;

			celula=new celrad [ncel];
			for(int i=0;i<ncel;i++)celula[i]=vPorosRad.celula[i];
		}
		else{
			lenth=0;
			ncamada=0;
			drcamada=0;
			Pint=0;
			presRes=0;
			presRes2=0;
            tRes=0;
			dt=0;
			nglobal=0;

			fluxIni=0.;
			fluxFim=0.;
			fluxIniA=0.;
			fluxFimA=0.;
			fluxIniG=0.;
			fluxFimG=0.;
			DfluxIni=0;
			DfluxIniA=0;
			DfluxIniG=0;
			resGlob=0.;
			celula=0;
		}
	}
  }

 return *this;
}


void PorosRad::iniciaVariaveis(){
	lenth=0;
	ncamada=0;
	drcamada=0;
	Pint=0;
	presRes=0;
	presRes2=0;
    tRes=0;
	dt=0;
	nglobal=0;

	fluxIni=0.;
	fluxFim=0.;
	fluxIniA=0.;
	fluxFimA=0.;
	fluxIniG=0.;
	fluxFimG=0.;
	resGlob=0.;

	DfluxIni=0.;
	DfluxIniG=0.;
	DfluxIniA=0.;

	celula=0;
	ncel=0;

	geom=DadosGeoPoro();
	fluc=ProFluCol();
	flup=ProFlu();

	permanente=0;

	zD0=0;
	zD1=0;
	resFim=0;
	he=0;
	hi=0;

	dtL=0;
	dh=0;


	condiTparede=0;
	sLRes=0;
	sWRes=0;
	sWPoc=0;
	satConata=0;
	ip=0;

	kRelOA=tabelaPemRelOA();
	kRelOG=tabelaPemRelOG();
	pcOA=tabelaPresCapOA();
	pcGO=tabelaPresCapGO();

	pBolha=1000.;
    BSW=0;
	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;

	vg1dSP=0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//alteracao2

double PorosRad::interpolaTabela(int nserie, double valx, double* x, double* y){
	double val;
	int ind=0;
	double raz;
	for (int i = 0; i <= nserie - 1; i++) {
		if (i < nserie - 1) {
			if (valx >= x[i] && valx < x[i + 1]) {
				ind = i;
				raz = 1
						- (valx - x[i])
								/ (x[i + 1] - x[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == nserie - 1) {
			ind = i;
			raz = 1;
		}
	}

	  double inf = y[ind];
	  double sup;
	  if (ind < nserie - 1){
		  sup = y[ind + 1];
		  val = (inf * raz + (1 - raz) * sup);
	  }
	  else
		val = y[ind];

	return val;
}

double PorosRad::fkO(double satW, double satG){
	double krw=interpolaTabela(kRelOA.npont, satW, kRelOA.satW, kRelOA.permRelW);
	double krow=interpolaTabela(kRelOA.npont, satW, kRelOA.satW, kRelOA.permRelOW);
	double krowC=interpolaTabela(kRelOA.npont, satConata, kRelOA.satW, kRelOA.permRelOW);

	double val=krowC*(((krow/krowC)+krw)-(krw));

	return val;
}



void PorosRad::geraCel(double** const vPcamada,double** const vsL,double** const vsW,
		const double vsLRes,const double vsWRes){
/*
 * 	celrad(int vncel=0,int vicel=0,double vr0=0., double vr1=0., double vrm=0.,double vKXL=0.,double vKXR=0.,
			double vKYL=0.,double vKYR=0.,double vporo=0.,double vcompresPoro=0.,double vPcamada=0,double vPint=0.,
			double vpresRes=0.,double vQL=0., double vQR=0, double vtRes=0, double vdt=0, double vzD=0,
			double vdh=1,ProFluCol vfluc=ProFluCol(), celrad* vcelL=0, celrad* vcelR=0);

			,
			double** const vsL=0,double** const vsW=0,const double vsLRes=0,const double vsWRes=0,
			const double vsatConata=0,
			tabelaPemRelOA vkRelOA=tabelaPemRelOA(),tabelaPemRelOG vkRelOG=tabelaPemRelOG(),
			tabelaPresCapOA vpcOA=tabelaPresCapOA(),tabelaPresCapGO vpcGO=tabelaPresCapGO(),
			ProFluCol vfluc=ProFluCol(), ProFlu vflup=ProFlu()
 */
	double dzdr=(zD1-zD0)/(geom.diamC[geom.ncamadas-1]-geom.a);
	double r0;
	double r1;
	double r2;
	double zdK2;
	double zdK;
	double zdKm;
	double kxl;
	double kxr;
	double kyl;
	double kyr;
	double ql=0.;
	double qr=0.;
	double pcamada;
    celrad* vcelL=0;
    celrad* vcelR=0;
    celula=new celrad [ncel];
	int konta=0;
	r0=geom.a/2.;
	r1=r0;
	r2=geom.a/2.+drcamada[0];
	zdK2=dzdr*(2.*r2-geom.a)+zD0;
	zdK=dzdr*(2.*r1-geom.a)+zD0;
	zdKm=dzdr*(2.*r0-geom.a)+zD0;
	kxl=geom.kX[0];
	kyl=geom.kY[0];
	kxr=geom.kX[0];
	kyr=geom.kY[0];
	pcamada=vPcamada[0][0];
	double sL=vsL[0][0];
	double sW=vsW[0][0];
	tabelaPemRelOA vkRelOA;
	tabelaPemRelOG vkRelOG;
	tabelaPresCapOA vpcOA;
	tabelaPresCapGO vpcGO;
	vkRelOA=kRelOA;
	vkRelOG=kRelOG;
	vpcOA=pcOA;
	vpcGO=pcGO;
    celula[konta]=celrad(ncel,konta,r0, r2, r1,kxl,kxr,
			kyl,kyr,geom.poro[0],geom.compRoc[0],pcamada,Pint,
			presRes,ql, qr, tRes, dt,zdKm, zdK,zdK2,
			dh,sL,sW,sLRes,sWRes,sWPoc,
			satConata,ip,vkRelOA,vkRelOG,vpcOA,vpcGO,
			fluc, flup,vcelL, vcelR,vg1dSP);
    konta++;
	for(int icam=0;icam<geom.ncamadas;icam++){
		 for(int idisc=1;idisc<=ncamada[icam];idisc++){
			  if(icam>0||(icam==0 && idisc>0)){

			    if(icam>0){
				    r0=geom.diamC[icam-1]/2.+(idisc-1)*drcamada[icam];
				    r1=geom.diamC[icam-1]/2.+idisc*drcamada[icam];
				    if(idisc<ncamada[icam])r2=geom.diamC[icam-1]/2.+(idisc+1)*drcamada[icam];
				    else if(icam<geom.ncamadas-1) r2=geom.diamC[icam]/2.+drcamada[icam+1];
				    else r2=r1;
			    }
			    else{
				    r0=geom.a/2.+(idisc-1)*drcamada[0];
				    r1=geom.a/2.+idisc*drcamada[0];
				    if(idisc<ncamada[0])r2=geom.a/2.+(idisc+1)*drcamada[0];
				    else if(icam<geom.ncamadas-1) r2=geom.diamC[0]/2.+drcamada[1];
				    else r2=r1;
			    }
			    zdK2=dzdr*(2.*r2-geom.a)+zD0;
			    zdK=dzdr*(2.*r1-geom.a)+zD0;
			    zdKm=dzdr*(2.*r0-geom.a)+zD0;
			    if(idisc>0 || icam==0){
			    	kxl=geom.kX[icam];
			    	kyl=geom.kY[icam];
			    }
			    else{
			    	kxl=geom.kX[icam-1];
			    	kyl=geom.kY[icam-1];
			    }
			    if(idisc<ncamada[icam] || icam==geom.ncamadas-1){
			    	kxr=geom.kX[icam];
			    	kyr=geom.kY[icam];
			    }
			    else{
			    	kxr=geom.kX[icam+1];
			    	kyr=geom.kY[icam+1];
			    }

			    pcamada=vPcamada[icam][idisc];
				sL=vsL[icam][idisc];
				sW=vsW[icam][idisc];
				vkRelOA=kRelOA;
				vkRelOG=kRelOG;
				vpcOA=pcOA;
				vpcGO=pcGO;
			    celula[konta]=celrad(ncel,konta,r0, r2, r1,kxl,kxr,
						kyl,kyr,geom.poro[icam],geom.compRoc[icam],pcamada,Pint,
						presRes,ql, qr, tRes, dt,zdKm, zdK,zdK2,
						dh,sL,sW,sLRes,sWRes,sWPoc,
						satConata,ip,vkRelOA,vkRelOG,vpcOA,vpcGO,
						fluc, flup,vcelL, vcelR,vg1dSP);
			    konta++;

			  }
		 }
	 }

	 /*for(int i=0;i<ncel;i++){
		 if(i>0)celula[i].celL=&celula[i-1];
		 else celula[i].celL=&celula[i];
		 if(i<ncel-1)celula[i].celR=&celula[i+1];
		 else celula[i].celR=&celula[i];
	 }*/
}

double PorosRad::marchaperm(double mastot){

     double h1;
	 double h2;
	 double h3;
	 double cal;


     hi=h1=10000000.*10000./geom.a;
	 h3=50000.;
	 he=h3;

	 for(int i=ncel-1;i>=0;i--){
		 renovaPres(i,mastot);
	 }
	 return celula[0].Pcamada-celula[0].Pint;
}

double PorosRad::transperm(double mastot){
	double cal;
	if(mastot<0){
	  double keq=celula[0].kabsol1;
	  double visc=flup.ViscOleo(1., tRes)/1000.;
	  double ip=98066.52*(keq/(visc))/
			         (logl(celula[ncel-1].rm/celula[0].rm)+0.);
	  mastot=ip*(celula[0].presRes-celula[0].Pint)*1000.;
	}
	double val=marchaperm(mastot);
	double chutePos=0.;
	double chuteNeg;
	if(val>0.){
		chutePos=mastot;
		while(val>0.){
			mastot*=1.1;
			val=marchaperm(mastot);
			if(val>0.)chutePos=mastot;
			else chuteNeg=mastot;
		}
	}
	else{
		chuteNeg=mastot;
		while(val<0.){
			mastot*=0.95;
			val=marchaperm(mastot);
			if(val<0.)chuteNeg=mastot;
			else chutePos=mastot;
		}
	}

	cal=zriddr(chutePos,chuteNeg);


   fluxIni=cal;
   fluxFim=cal;

   return cal;

}

void PorosRad::renovaPres(int i, double mTot){

	double rhoa=celula[ncel-1].flup.MasEspAgua(presRes, tRes);
	double rhoo=celula[ncel-1].flup.MasEspoleo(presRes, tRes);
	double rhog=celula[ncel-1].flup.MasEspGas(presRes, tRes);
	double multA=rhoa*(sWRes-satConata)/(1.-satConata);
	double multO=rhoo*(sLRes-sWRes)/(1.-satConata);
	double multG=rhog*(1.-sLRes)/(1.-satConata);
	double qtot=mTot/(multA+multO+multG);
	double massA=multA*qtot;
	double massO=multO*qtot;
	double qOstd=(massO/rhoo)/celula[ncel-1].flup.BOFunc(presRes, tRes);
	double massOstd=celula[ncel-1].rhostd*qOstd;
	double massG=multG*qtot;
	double massgDisolv=qOstd*celula[ncel-1].flup.RS(presRes, tRes)*(6.29 / 35.31467)*celula[ncel-1].rhogstd;
	double massgTot=massG+massgDisolv;

	if(i==ncel-1){
		celula[i].Pcamada=celula[i].presRes;
		celula[i].sL=celula[i].sLRes;
		celula[i].sW=celula[i].sWRes;
	}
	else{
		double rhomed;
		double rhogmed;
		double rhoamed;
		double miomed;
		double migmed;
		double miamed;
		double kO;
		double kG;
		double kA;
		double dO;
		double dG;
		double dA;
		double grav=9.82/98066.22;

		celula[i+1].pcOGm=celula[i+1].interpolaTabela(celula[i+1].pcGOCel.npont, 1.-celula[i+1].sL, celula[i+1].pcGOCel.satG,
				celula[i+1].pcGOCel.presCapGO);
		celula[i].pcOGm=celula[i].interpolaTabela(celula[i].pcGOCel.npont, 1.-celula[i+1].sL, celula[i].pcGOCel.satG, celula[i].pcGOCel.presCapGO);
		celula[i+1].pcAOm=celula[i+1].interpolaTabela(celula[i+1].pcOACel.npont, celula[i+1].sW, celula[i+1].pcOACel.satW,
				celula[i+1].pcOACel.presCapOW);
		celula[i].pcAOm=celula[i].interpolaTabela(celula[i].pcOACel.npont, celula[i+1].sW, celula[i].pcOACel.satW, celula[i].pcOACel.presCapOW);



		rhomed=celula[i+1].rhoP=celula[i+1].flup.MasEspoleo(celula[i+1].Pcamada, tRes);
		rhogmed=celula[i+1].rhogP=celula[i+1].flup.MasEspGas(celula[i+1].Pcamada, tRes);
		rhoamed=celula[i+1].rhoPa=celula[i+1].flup.MasEspAgua(celula[i+1].Pcamada, tRes);

		miomed=celula[i+1].mio0=celula[i+1].flup.ViscOleo(celula[i+1].Pcamada, tRes)/1000.;
		migmed=celula[i+1].mig0=celula[i+1].flup.ViscGas(celula[i+1].Pcamada, tRes)/1000.;
		miamed=celula[i+1].mia0=celula[i+1].flup.VisAgua(tRes)/1000.;

		kO=celula[i+1].kmed0=celula[i+1].fkO(celula[i+1].sW, 1.-celula[i+1].sL);
		kG=celula[i+1].kmedG0=celula[i+1].interpolaTabela(celula[i+1].kRelOGCel.npont, 1.-celula[i+1].sL,
			celula[i+1].kRelOGCel.satG, celula[i+1].kRelOGCel.permRelG);
		kA=celula[i+1].kmedA0=celula[i+1].interpolaTabela(celula[i+1].kRelOACel.npont, celula[i+1].sW,
			celula[i+1].kRelOACel.satW, celula[i+1].kRelOACel.permRelW);

		double erro=10000.;
		celula[i].sL=celula[i+1].sL;
		celula[i].sW=celula[i+1].sW;

		while(erro>1e-5){
			dO=celula[i+1].darcyO0=celula[i+1].kmed0*celula[i+1].kabsol0*98066.22/miomed;
			dG=celula[i+1].darcyG0=celula[i+1].kmedG0*celula[i+1].kabsol0*98066.22/migmed;
			dA=celula[i+1].darcyA0=celula[i+1].kmedA0*celula[i+1].kabsol0*98066.22/miamed;

			double mTotGeom=mTot*celula[i].drP1/celula[i].rQcamadaR;
			double termoPR=rhomed*dO+rhogmed*dG+rhoamed*dA;
			double termoGrav=rhomed*dO*(celula[i+1].zD*rhomed*grav-celula[i+1].zD0*rhomed*grav)+
					rhoamed*dA*(celula[i+1].zD*rhoamed*grav-celula[i+1].zD0*rhoamed*grav)+
					rhogmed*dG*(celula[i+1].zD*rhogmed*grav-celula[i+1].zD0*rhogmed*grav);
			double termoPresCap=rhogmed*dG*(-celula[i+1].pcOGm+celula[i].pcOGm)+
					rhoamed*dA*(celula[i+1].pcAOm-celula[i].pcAOm);

			celula[i].Pcamada=celula[i+1].Pcamada-mTotGeom/termoPR-1.*termoGrav/termoPR-1.*termoPresCap/termoPR;
			celula[i+1].pmed0=celula[i].pmed1=0.5*(celula[i].Pcamada+celula[i+1].Pcamada);

			celula[i].rhoP=celula[i].flup.MasEspoleo(celula[i].Pcamada, tRes);
			celula[i].rhogP=celula[i].flup.MasEspGas(celula[i].Pcamada, tRes);
			celula[i].rhoPa=celula[i].flup.MasEspAgua(celula[i].Pcamada, tRes);

			rhomed=celula[i+1].flup.MasEspoleo(celula[i+1].pmed0, tRes);
			rhogmed=celula[i+1].flup.MasEspGas(celula[i+1].pmed0, tRes);
			rhoamed=celula[i+1].flup.MasEspAgua(celula[i+1].pmed0, tRes);
			miomed=celula[i+1].flup.ViscOleo(celula[i+1].pmed0, tRes)/1000.;
			migmed=celula[i+1].flup.ViscGas(celula[i+1].pmed0, tRes)/1000.;
			miamed=celula[i+1].flup.VisAgua(tRes)/1000.;

			double vazoleo=dO*(celula[i].rQcamadaR/celula[i].drP1)*(celula[i+1].Pcamada-celula[i].Pcamada-
					1*rhomed*(celula[i+1].zD*grav-celula[i+1].zD0*grav));
			double vazoleoSTD=vazoleo/celula[i+1].flup.BOFunc(celula[i].pmed1, tRes);
			double vazgas=dG*(celula[i].rQcamadaR/celula[i].drP1)*(celula[i+1].Pcamada-celula[i].Pcamada-
					1*rhogmed*(celula[i+1].zD*grav-celula[i+1].zD0*grav-celula[i+1].pcOGm+celula[i].pcOGm));
			double vazagua=dA*(celula[i].rQcamadaR/celula[i].drP1)*(celula[i+1].Pcamada-celula[i].Pcamada-
					1*rhoamed*(celula[i+1].zD*grav-celula[i+1].zD0*grav+celula[i+1].pcAOm-celula[i].pcAOm));
			double rs1=celula[i].flup.RS(celula[i].pmed1, tRes) * 6.29 / 35.31467;
			double rs0=celula[i].flup.RS(celula[i].Pcamada, tRes) * 6.29 / 35.31467;
			double maslib=vazoleoSTD*(rs1-rs0)*celula[i+1].rhogstd;
			vazoleo=(vazoleo*rhomed-maslib)/celula[i].rhoP;
			vazgas=(vazgas*rhomed+maslib)/celula[i].rhogP;
			vazagua=vazagua*rhoamed/celula[i].rhoPa;

			double sLiter=celula[i].sL;
			double sWiter=celula[i].sW;
			celula[i].sL=1.-vazgas*(1-celula[i].satConata)/(vazgas+vazoleo+vazagua);
			if(celula[i].sL>1.-1e-15){
				celula[i].sL=1.;
				if(vazgas<0)vazgas=0.;
			}
			else if(celula[i].sL<1e-15)celula[i].sL=0.;
			celula[i].sW=(vazagua+celula[i].satConata*(vazgas+vazoleo))/(vazgas+vazoleo+vazagua);
			if(celula[i].sW>celula[i].sL-1e-15)celula[i].sW=celula[i].sL;
			else if(celula[i].sW<celula[i].satConata-1e-15)celula[i].sW=celula[i].satConata;

			erro=(fabsl(celula[i].sL-sLiter)+fabsl(celula[i].sW-sWiter));

			kO=celula[i+1].kmed0=celula[i+1].fkO(0.5*(celula[i+1].sW+celula[i].sW), 1.-0.5*(celula[i+1].sL+celula[i].sL));
			kG=celula[i+1].kmedG0=celula[i+1].interpolaTabela(celula[i+1].kRelOGCel.npont, 1.-0.5*(celula[i+1].sL+celula[i].sL),
					celula[i+1].kRelOGCel.satG, celula[i+1].kRelOGCel.permRelG);
			kA=celula[i+1].kmedA0=celula[i+1].interpolaTabela(celula[i+1].kRelOACel.npont, 0.5*(celula[i+1].sW+celula[i].sW),
					celula[i+1].kRelOACel.satW, celula[i+1].kRelOACel.permRelW);

			celula[i].pcOGm=celula[i].interpolaTabela(celula[i].pcGOCel.npont, 1.-celula[i].sL, celula[i].pcGOCel.satG,
					celula[i].pcGOCel.presCapGO);
			celula[i].pcAOm=celula[i].interpolaTabela(celula[i].pcOACel.npont, celula[i].sW, celula[i].pcOACel.satW, celula[i].pcOACel.presCapOW);
		}
	}


}



double PorosRad::SIGN(double a, double b) {
  return (b >= 0 ? 1.0 : -1.0) * fabsl(a);
}


double  PorosRad::zriddr(double x1,double x2){
	double xacc=1e-5;
	int maxit=100;
	double fmin;
	double xmin;
	double fl=marchaperm(x1);
	double fh=marchaperm(x2);
	if(fabsl(fh)<fabsl(fl)){
		fmin=fh;
		xmin=x2;
	}
	else{
		fmin=fl;
		xmin=x1;
	}
	if((fl>0.0 && fh<0.0) || (fl<0.0 && fh>0.0)){
		double xl=x1;
		double xh=x2;
		double ans=-1.e20;
		for(int j=0;j<maxit;j++){
			double xm=0.5*(xl+xh);
			double fm=marchaperm(xm);
			if(fabsl(fm)<fabsl(fmin)){
				fmin=fm;
				xmin=xm;
			}
			double s=sqrtl(fm*fm-fl*fh);
			if(s==0.0){
				fmin=marchaperm(xmin);
				return xmin;
			}
			double xnew=xm+(xm-xl)*((fl>=fh ? 1.0 : -1.0)*fm/s);
			if(fabsl(xnew-ans)<=xacc ){
				fmin=marchaperm(xmin);
				return xmin;
			}
			ans=xnew;
			double fnew=marchaperm(ans);
			if(fabsl(fnew)<fabsl(fmin)){
				fmin=fnew;
				xmin=ans;
			}
			if(fabsl(fnew)<=xacc ){
				fmin=marchaperm(xmin);
				return xmin;
			}
			if(SIGN(fm, fnew)!=fm){
				xl=xm;
				fl=fm;
				xh=ans;
				fh=fnew;
			}
			else if(SIGN(fl,fnew)!=fl){
				xh=ans;
				fh=fnew;
			}
			else if(SIGN(fh,fnew)!=fh){
				xl=ans;
				fl=fnew;
			}
			else return -1.e10;
			if(fabsl(xh-xl)<=xacc ){
				fmin=marchaperm(xmin);
				return xmin;
			}

		}
		 return 1.e10;
	}
	else {
		if(fabsl(fl)<=xacc) return x1;
		if(fabsl(fh)<=xacc) return x2;
		return -1e10;
	}
}

/*void PorosRad::indrazT(int& ind, double& raz) {
	for (int i = 0; i <= temp.parserie - 1; i++) {
		if (i < temp.parserie - 1) {
			if (tempo >= temp.tempo[i] && tempo < temp.tempo[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - temp.tempo[i])
								/ (temp.tempo[i + 1] - temp.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == temp.parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void PorosRad::indrazPW(int& ind, double& raz) {
	for (int i = 0; i <= pW.nserie - 1; i++) {
		if (i < pW.nserie - 1) {
			if (tempo >= pW.tempo[i] && tempo < pW.tempo[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - pW.tempo[i])
								/ (pW.tempo[i + 1] - pW.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == pW.nserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void PorosRad::indraz(int& ind, double& raz, double* serieTempo, int nserie) {
	for (int i = 0; i <= nserie - 1; i++) {
		if (i < nserie - 1) {
			if (tempo >= serieTempo[i] && tempo < serieTempo[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - serieTempo[i])
								/ (serieTempo[i + 1] - serieTempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == nserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void PorosRad::defineDT(){
	  int ind;
	  double raz;
	  double delt;
	  indrazT(ind, raz);
	  double inf = temp.dtmax[ind];
	  double sup;
	  if (ind < temp.parserie - 1){
		  sup = temp.dtmax[ind + 1];
		  delt = (inf * raz + (1 - raz) * sup);
	  }
	  else
		  delt = temp.dtmax[ind];

	  dt=delt;

	  for(int i=0;i<ncel;i++){
		  //delt=0.5*celula[i].cflO();
		  //if(dt>delt)dt=delt;
		  delt=0.8*celula[i].cflA();
		  if(dt>delt)dt=delt;
	  }

	  for(int i=0;i<ncel;i++){
		  celula[i].dt=dt;
		  celula[i].dtSL=dt;
		  celula[i].dtSW=dt;
	  }
}*/

void PorosRad::atualizaIni(){
	 int idisc=0;
	  //Pini[0][0]=Pcamada[0][0];
	  celula[idisc].Pini=celula[idisc].Pcamada;
	  //Qini[0][0]=Qcamada[0][0];
	  celula[idisc].QiniR=celula[idisc].QcamadaR;
	  celula[idisc].Qoini=celula[idisc].QocamadaR;
	  celula[idisc].Qgini=celula[idisc].QgcamadaR;
	  celula[idisc].Qwini=celula[idisc].QwcamadaR;
	  celula[idisc].QiniL=celula[idisc].QcamadaL;
	  celula[idisc].sWini=celula[idisc].sW;
	  celula[idisc].sLini=celula[idisc].sL;
	  celula[idisc].vboIni=celula[idisc].vbo;
	  celula[idisc].rhoPIni=celula[idisc].rhoP;
	  celula[idisc].rhogPIni=celula[idisc].rhogP;
	  celula[idisc].rhoPaIni=celula[idisc].rhoPa;
	  for(int i=1;i<ncel;i++){
	  //for(int i=0;i<geom.ncamadas;i++){
		//for(int j=1;j<=ncamada[i];j++){
			//idisc++;
				 //Pini[i][j]=Pcamada[i][j];
			celula[i].Pini=celula[i].Pcamada;
				 //Qini[i][j]=Qcamada[i][j];
			celula[i].QiniR=celula[i].QcamadaR;
			celula[i].Qoini=celula[i].QocamadaR;
			celula[i].Qgini=celula[i].QgcamadaR;
			celula[i].Qwini=celula[i].QwcamadaR;
			celula[i].QiniL=celula[i].QcamadaL;
			celula[i].sWini=celula[i].sW;
			celula[i].sLini=celula[i].sL;
			celula[idisc].vboIni=celula[idisc].vbo;
			celula[idisc].rhoPIni=celula[idisc].rhoP;
			celula[idisc].rhogPIni=celula[idisc].rhogP;
			celula[idisc].rhoPaIni=celula[idisc].rhoPa;
		//}
		/*if(i<geom.ncamadas-1){
				 Pini[i+1][0]=Pcamada[i+1][0];
				 Qini[i+1][0]=Qcamada[i+1][0];
		}*/
	  }
}


double PorosRad::transtrans(double espessura){

 for(int i=0; i<ncel;i++){
	celula[i].Pint=Pint;
	celula[i].sWPoc=sWPoc;
 }
 int nglob=0;
 for(int i=0;i<geom.ncamadas;i++)nglob+=ncamada[i];
 nglob=4*(nglob+1);
 Vcr<double> vetliv(nglob);
 BandMtx<double> matglob(nglob, 3, 4);
 int konta;
 int idisc;
 double erro=100.;
 int iterPres=0;
 while(erro>1e-6 && iterPres<20){
	 idisc=0;
	 erro=0.;
	 //transcel(0, 0);
	 celula[idisc].transcel(idisc);
	 /*vetliv[0]=celula[idisc].localvet[0];
	 vetliv[1]=celula[idisc].localvet[1];
	 vetliv[2]=celula[idisc].localvet[2];
	 vetliv[3]=celula[idisc].localvet[3];*/
	 for(int kLin=0;kLin<4;kLin++){
		 vetliv[kLin]=celula[idisc].localvet[kLin];
		 int ini;
		 if(kLin==0)ini=kLin;
		 else ini=kLin;
		 for(int kCol=ini;kCol<8+kLin;kCol++){
			 matglob[kLin][kCol-kLin-3]=celula[idisc].localmat[kLin][kCol];
		 }
	 }
	 /*matglob[0][-3]=celula[idisc].localmat[0][0];
	 matglob[0][-2]=celula[idisc].localmat[0][1];
	 matglob[0][-1]=celula[idisc].localmat[0][2];
	 matglob[0][0]=celula[idisc].localmat[0][3];
	 matglob[0][1]=celula[idisc].localmat[0][4];
	 matglob[0][2]=celula[idisc].localmat[0][5];
	 matglob[0][3]=celula[idisc].localmat[0][6];
	 matglob[1][-3]=celula[idisc].localmat[1][1];
	 matglob[1][-2]=celula[idisc].localmat[1][2];
	 matglob[1][-1]=celula[idisc].localmat[1][3];
	 matglob[1][0]=celula[idisc].localmat[1][4];
	 matglob[1][1]=celula[idisc].localmat[1][5];
	 matglob[1][2]=celula[idisc].localmat[1][6];
	 matglob[1][3]=celula[idisc].localmat[1][7];
	 matglob[2][-3]=celula[idisc].localmat[2][2];
	 matglob[2][-2]=celula[idisc].localmat[2][3];
	 matglob[2][-1]=celula[idisc].localmat[2][4];
	 matglob[2][0]=celula[idisc].localmat[2][5];
	 matglob[2][1]=celula[idisc].localmat[2][6];
	 matglob[2][2]=celula[idisc].localmat[2][7];
	 matglob[3][-3]=celula[idisc].localmat[3][3];
	 matglob[3][-2]=celula[idisc].localmat[3][4];
	 matglob[3][-1]=celula[idisc].localmat[3][5];
	 matglob[3][0]=celula[idisc].localmat[3][6];
	 matglob[3][1]=celula[idisc].localmat[3][7];*/
	 konta=4;
	 while(idisc<ncel-1){
	 //for(int i=0;i<geom.ncamadas;i++){
		 //for(int j=1;j<=ncamada[i];j++){
			 idisc++;
			 //transcel(i,j);
			 celula[idisc].transcel(idisc);
			 for(int kLin=0;kLin<4;kLin++){
				 vetliv[kLin+konta]=celula[idisc].localvet[kLin];
				 for(int kCol=kLin;kCol<8;kCol++){
					 matglob[kLin+konta][kCol-kLin-3]=celula[idisc].localmat[kLin][kCol];
				 }
			 }
			 /*vetliv[konta]=celula[idisc].localvet[0];
			 vetliv[konta+1]=celula[idisc].localvet[1];
			 matglob[konta][-1]=celula[idisc].localmat[0][1];
			 matglob[konta][0]=celula[idisc].localmat[0][2];
			 matglob[konta][1]=celula[idisc].localmat[0][3];
			 matglob[konta+1][-3]=celula[idisc].localmat[1][0];
			 matglob[konta+1][-1]=celula[idisc].localmat[1][2];
			 matglob[konta+1][0]=celula[idisc].localmat[1][3];
			 if(konta+1<=nglob-1)matglob[konta+1][1]=celula[idisc].localmat[1][4];*/
			 konta+=4;
		 //}
	 //}
	 }
	 matglob.GaussElimPP(vetliv);


	// erro+=fabsl(vetliv[0]-Pcamada[0][0]);
	 idisc=0;
	 erro+=fabsl(vetliv[0]-celula[idisc].Pcamada);
	 //Pcamada[0][0]=vetliv[0];
	 celula[idisc].Pcamada=vetliv[0];
	 celula[idisc].PcamadaL=vetliv[0];
	 //Qcamada[0][0]=2.*M_PI*vetliv[1];
	 celula[idisc].QocamadaR=2.*M_PI*vetliv[1];
	 celula[idisc].QocamadaL=celula[idisc].QocamadaR;
	 celula[idisc].QgcamadaR=2.*M_PI*vetliv[2];
	 celula[idisc].QgcamadaL=celula[idisc].QgcamadaR;
	 celula[idisc].QwcamadaR=2.*M_PI*vetliv[3];
	 celula[idisc].QwcamadaL=celula[idisc].QwcamadaR;
	 celula[idisc].QcamadaR=celula[idisc].QocamadaR+celula[idisc].QgcamadaR+celula[idisc].QwcamadaR;
	 celula[idisc].QcamadaL=celula[idisc].QcamadaR;
	 konta=4;
	 for(int i=1;i<ncel;i++){
	 //for(int i=0;i<geom.ncamadas;i++){
		 //for(int j=1;j<=ncamada[i];j++){
			 //idisc++;
			 //erro+=fabsl(vetliv[konta]-Pcamada[i][j]);
			 erro+=fabsl(vetliv[konta]-celula[i].Pcamada);
			 //Pcamada[i][j]=vetliv[konta];
			 celula[i].Pcamada=vetliv[konta];
			 celula[i-1].PcamadaR=vetliv[konta];
			 celula[i].PcamadaL=celula[i-1].Pcamada;
			 double verifica;
			 verifica=vetliv[konta+1];

			 //Qcamada[i][j]=2.*M_PI*vetliv[konta+1];
			 celula[i].QocamadaR=2.*M_PI*vetliv[konta+1];
			 celula[i].QocamadaL=celula[i-1].QocamadaR;
			 celula[i].QgcamadaR=2.*M_PI*vetliv[konta+2];
			 celula[i].QgcamadaL=celula[i-1].QgcamadaR;
			 celula[i].QwcamadaR=2.*M_PI*vetliv[konta+3];
			 celula[i].QwcamadaL=celula[i-1].QwcamadaR;
			 celula[i].QcamadaR=celula[i].QocamadaR+celula[i].QgcamadaR+celula[i].QwcamadaR;
			 celula[i].QcamadaL=celula[i-1].QcamadaR;
			 konta+=4;
		 //}
		 /*if(i<geom.ncamadas-1){
			 Pini[i+1][0]=Pcamada[i+1][0];
			 Pcamada[i+1][0]=Pcamada[i][ncamada[i]];
			 Qini[i+1][0]=Qcamada[i+1][0];
			 Qcamada[i+1][0]=Qcamada[i][ncamada[i]];
		 }*/
	 //}
	 }
	 celula[ncel-1].PcamadaR=celula[ncel-1].Pcamada;
	 erro/=ncel;
	 iterPres++;
  }

 double tit=flup.FracMassHidra(Pint, tRes);
 double rhogP=flup.MasEspGas(Pint, tRes);
 double rhoP=flup.MasEspoleo(Pint, tRes);
 double alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
 double qaSTD=vetliv[3]*celula[0].flup.MasEspAgua(Pint, tRes)/(flup.Denag*1000.);
 double qoSTD;
 qoSTD=vetliv[1]*(1-alf)/flup.BOFunc(Pint, tRes);
 if((fabsl(qaSTD)+fabsl(qoSTD))>1e-15) celula[0].BSW=BSW=fabsl(qaSTD)/(fabsl(qaSTD)+fabsl(qoSTD));

 double pAux=0.;
 double qaSTD2;
 double qoSTD2;
 for(int i=1;i<ncel;i++){
	  pAux=celula[i].Pcamada;
	  double tit=flup.FracMassHidra(pAux, tRes);
	  double rhogP=flup.MasEspGas(pAux, tRes);
	  double rhoP=flup.MasEspoleo(pAux, tRes);
	  double alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
	  qaSTD2=celula[i].QwcamadaR*celula[i].flup.MasEspAgua(pAux, tRes)/(flup.Denag*1000.);
	  qoSTD2=celula[i].QocamadaR*(1-alf)/celula[i].flup.BOFunc(pAux, tRes);
	  if((fabsl(qaSTD2)+fabsl(qoSTD2))>1e-15){
		  celula[i].BSW=fabsl(qaSTD2)/(fabsl(qaSTD2)+fabsl(qoSTD2));
	  }
 }
 BSW=celula[2].BSW;


  /*double qaSTD=vetliv[3]*celula[0].flup.MasEspAgua(Pint, tRes)/(flup.Denag*1000.);
  double qoSTD=vetliv[1]*(1-celula[0].alf)/flup.BOFunc(Pint, tRes);
  if(fabsl(qaSTD+qoSTD)>1e-15) flup.BSW=qaSTD/(qaSTD+qoSTD);*/
  /*fluxIni=2.*M_PI*vetliv[1]*(1-celula[0].alf)*celula[0].flup.MasEspoleo(Pint, tRes)*espessura;
  fluxIniG=2.*M_PI*vetliv[1]*celula[0].alf*celula[0].flup.MasEspGas(Pint, tRes)*espessura;
  fluxIniA=2.*M_PI*vetliv[3]*celula[0].flup.MasEspAgua(Pint, tRes)*espessura;*/
  fluxIni=2.*M_PI*vetliv[5]*(1-celula[1].alf)*celula[1].flup.MasEspoleo(pAux, tRes)*espessura;
  fluxIniG=2.*M_PI*vetliv[5]*celula[1].alf*celula[1].flup.MasEspGas(pAux, tRes)*espessura;
  fluxIniA=2.*M_PI*vetliv[7]*celula[1].flup.MasEspAgua(pAux, tRes)*espessura;
  konta-=4;
  fluxFim=2.*M_PI*vetliv[konta+1]*(1-celula[ncel-1].alf)*
		  celula[0].flup.MasEspoleo(celula[ncel-1].Pcamada, tRes)*espessura;
  fluxFimG=2.*M_PI*vetliv[konta+1]*celula[ncel-1].alf*
		  celula[0].flup.MasEspGas(celula[ncel-1].Pcamada, tRes)*espessura;
  fluxFimA=2.*M_PI*vetliv[konta+3]*
		  celula[0].flup.MasEspAgua(celula[ncel-1].Pcamada, tRes)*espessura;

  //cout << "  erro= "<<erro<<";"<<" iteracoes= "<<iterPres;
  //cout << endl;

  return erro;

}

void PorosRad::transtransAux(double espessura){

 int nglob=0;
 for(int i=0;i<geom.ncamadas;i++)nglob+=ncamada[i];
 nglob=4*(nglob+1);
 Vcr<double> vetliv(nglob);
 BandMtx<double> matglob(nglob, 3, 4);
 int konta;
 int idisc;
 double DPint=0.01*Pint;
 Pint+=DPint;
	 idisc=0;
	 //transcel(0, 0);
	 celula[idisc].transcel(idisc);
	 /*vetliv[0]=celula[idisc].localvet[0];
	 vetliv[1]=celula[idisc].localvet[1];
	 vetliv[2]=celula[idisc].localvet[2];
	 vetliv[3]=celula[idisc].localvet[3];*/
	 for(int kLin=0;kLin<4;kLin++){
		 vetliv[kLin]=celula[idisc].localvet[kLin];
		 int ini;
		 if(kLin==0)ini=kLin;
		 else ini=kLin;
		 for(int kCol=ini;kCol<8+kLin;kCol++){
			 matglob[kLin][kCol-kLin-3]=celula[idisc].localmat[kLin][kCol];
		 }
	 }
	 /*matglob[0][-3]=celula[idisc].localmat[0][0];
	 matglob[0][-2]=celula[idisc].localmat[0][1];
	 matglob[0][-1]=celula[idisc].localmat[0][2];
	 matglob[0][0]=celula[idisc].localmat[0][3];
	 matglob[0][1]=celula[idisc].localmat[0][4];
	 matglob[0][2]=celula[idisc].localmat[0][5];
	 matglob[0][3]=celula[idisc].localmat[0][6];
	 matglob[1][-3]=celula[idisc].localmat[1][1];
	 matglob[1][-2]=celula[idisc].localmat[1][2];
	 matglob[1][-1]=celula[idisc].localmat[1][3];
	 matglob[1][0]=celula[idisc].localmat[1][4];
	 matglob[1][1]=celula[idisc].localmat[1][5];
	 matglob[1][2]=celula[idisc].localmat[1][6];
	 matglob[1][3]=celula[idisc].localmat[1][7];
	 matglob[2][-3]=celula[idisc].localmat[2][2];
	 matglob[2][-2]=celula[idisc].localmat[2][3];
	 matglob[2][-1]=celula[idisc].localmat[2][4];
	 matglob[2][0]=celula[idisc].localmat[2][5];
	 matglob[2][1]=celula[idisc].localmat[2][6];
	 matglob[2][2]=celula[idisc].localmat[2][7];
	 matglob[3][-3]=celula[idisc].localmat[3][3];
	 matglob[3][-2]=celula[idisc].localmat[3][4];
	 matglob[3][-1]=celula[idisc].localmat[3][5];
	 matglob[3][0]=celula[idisc].localmat[3][6];
	 matglob[3][1]=celula[idisc].localmat[3][7];*/
	 konta=4;
	 while(idisc<ncel-1){
	 //for(int i=0;i<geom.ncamadas;i++){
		 //for(int j=1;j<=ncamada[i];j++){
			 idisc++;
			 //transcel(i,j);
			 celula[idisc].transcel(idisc);
			 for(int kLin=0;kLin<4;kLin++){
				 vetliv[kLin+konta]=celula[idisc].localvet[kLin];
				 for(int kCol=kLin;kCol<8;kCol++){
					 matglob[kLin+konta][kCol-kLin-3]=celula[idisc].localmat[kLin][kCol];
				 }
			 }
			 /*vetliv[konta]=celula[idisc].localvet[0];
			 vetliv[konta+1]=celula[idisc].localvet[1];
			 matglob[konta][-1]=celula[idisc].localmat[0][1];
			 matglob[konta][0]=celula[idisc].localmat[0][2];
			 matglob[konta][1]=celula[idisc].localmat[0][3];
			 matglob[konta+1][-3]=celula[idisc].localmat[1][0];
			 matglob[konta+1][-1]=celula[idisc].localmat[1][2];
			 matglob[konta+1][0]=celula[idisc].localmat[1][3];
			 if(konta+1<=nglob-1)matglob[konta+1][1]=celula[idisc].localmat[1][4];*/
			 konta+=4;
		 //}
	 //}
	 }
	 matglob.GaussElimPP(vetliv);
  //}


	  Pint-=DPint;
	  DfluxIni=(2.*M_PI*vetliv[1]*(1-celula[0].alf)*celula[0].flup.MasEspoleo(Pint+DPint, tRes)*espessura-
			    fluxIni)/DPint;
	  DfluxIniG=(2.*M_PI*vetliv[1]*celula[0].alf*celula[0].flup.MasEspGas(Pint+DPint, tRes)*espessura-
			     fluxIniG)/DPint;
	  DfluxIniA=(2.*M_PI*vetliv[3]*celula[0].flup.MasEspAgua(Pint+DPint, tRes)*espessura-
			     fluxIniA)/DPint;

}

/*void PorosRad::solveTrans(){
	defineDT();
	int ind;
	double raz;
	double inf;
	double sup;

	indrazPW(ind, raz);
	inf = pW.val[ind];
	if (ind < pW.nserie - 1){
		sup = pW.val[ind + 1];
		Pint = (inf * raz + (1 - raz) * sup);
	}
	else
		Pint = pW.val[ind];
	if(Pint<338){
		int para;
		para=0;
	}
	if(tempo>=43400){
		int para;
		para=0;
	}

	indraz(ind, raz,tempoSatLRes,nsatLRes);
	inf = satLRes[ind];
	if (ind < nsatLRes - 1){
		sup = satLRes[ind + 1];
		sLRes = (inf * raz + (1 - raz) * sup);
	}
	else
		sLRes = satLRes[ind];

	indraz(ind, raz,tempoSatARes,nsatARes);
	inf = satARes[ind];
	if (ind < nsatARes - 1){
		sup = satARes[ind + 1];
		sWRes = (inf * raz + (1 - raz) * sup);
	}
	else
		sWRes = satARes[ind];

	for(int i=0; i<ncel;i++){
		celula[i].Pint=Pint;
		celula[i].presRes=presRes;
		celula[i].presRes2=presRes2;
		celula[i].sLRes=sLRes;
		celula[i].sWRes=sWRes;
	}

	int ciclo=0;
	reinicia=0;
	for(int i=0;i<ncel;i++){
		celula[i].dPdT=0.;
		//celula[i].calcVazOWG();
	}
	while(ciclo<1){
		for(int i=0;i<ncel;i++){
			if(i>=ncel-1){
				celula[i].sL=sLRes;
				celula[i-1].sLR=sLRes;
				celula[i].sW=sWRes;
				celula[i-1].sWR=sWRes;
				celula[i].sLR=sLRes;
				celula[i].sWR=sWRes;
				celula[i].reiniciaSL=0;
				celula[i].reiniciaSW=0;
			}
			else{
				//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
				celula[i].evoluiSW(reinicia, ciclo, celula[i].dPdT);
				if(i>0){
					celula[i-1].sLR=celula[i].sL;
					celula[i-1].sWR=celula[i].sW;
					celula[i].sLL=celula[i-1].sL;
					celula[i].sWL=celula[i-1].sW;
				}
				else{
					celula[i].sLL=celula[i].sL;
					celula[i].sWL=celula[i].sW;
				}
			}
		}
		for(int i=0;i<ncel;i++){
			  if(celula[i].reiniciaSL<0 || celula[i].reiniciaSW<0){
				  reinicia=-1;
			  }
			  //celula[i].reiniciaSL=0;
			  celula[i].reiniciaSW=0;
		}
		if(reinicia<0){
			for(int i=0;i<ncel;i++){
				//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
				if(dt>celula[i].dtSW)dt=celula[i].dtSW;
			}
			for(int i=0;i<ncel;i++){
				celula[i].dt=dt;
				celula[i].dtSL=dt;
				celula[i].dtSW=dt;
			}
			for(int i=0;i<ncel;i++){
				//celula[i].sL=celula[i].sLini;
				//celula[i].sW=celula[i].sWini;
				//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
				//celula[i].evoluiSW(reinicia, ciclo, celula[i].dPdT);
				if(i==ncel-1){
					celula[i].sL=sLRes;
					celula[i-1].sLL=sLRes;
					celula[i].sW=sWRes;
					celula[i-1].sWL=sWRes;
					celula[i].sLR=celula[i].sL;
					celula[i].sWR=celula[i].sW;
					celula[i].reiniciaSL=0;
					celula[i].reiniciaSW=0;
				}
				else{
					//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
					celula[i].evoluiSW(reinicia, ciclo, celula[i].dPdT);
					if(i>0){
						celula[i-1].sLR=celula[i].sL;
						celula[i-1].sWR=celula[i].sW;
						celula[i].sLL=celula[i-1].sL;
						celula[i].sWL=celula[i-1].sW;
					}
					else{
						celula[i].sLL=celula[i].sL;
						celula[i].sWL=celula[i].sW;
					}
				}
			}
			reinicia=0;
		}
		transtrans();

		ciclo++;
	}
	atualizaIni();
	tempo+=dt;
}*/

void PorosRad::FeiticoDoTempo(){
	 /*Pcamada[0][0]=Pini[0][0];
	 Qcamada[0][0]=Qini[0][0];
	 int konta=2;
	  for(int i=0;i<geom.ncamadas;i++){
	 	 for(int j=1;j<=ncamada[i];j++){
	 		 Pcamada[i][j]=Pini[i][j];
	 		 Qcamada[i][j]=Qini[i][j];
	 		 konta+=2;
	 	 }
	 	if(i<geom.ncamadas-1){
	 		Pcamada[i+1][0]=Pini[i+1][0];
	 		Qcamada[i+1][0]=Qini[i+1][0];
	 	}
	  }*/
	 for(int i=0;i<ncel;i++)celula[i].FeiticoDoTempo();
	 celula[0].PcamadaL=celula[0].Pcamada;
	 celula[0].sLL=celula[0].sL;
	 celula[0].sWL=celula[0].sW;
	 celula[0].QwcamadaL=celula[0].QwcamadaR;
	 celula[0].QocamadaL=celula[0].QocamadaR;
	 celula[0].QgcamadaL=celula[0].QgcamadaR;
	 for(int i=1;i<ncel;i++){
		 celula[i].PcamadaL=celula[i-1].Pcamada;
		 celula[i-1].PcamadaR=celula[i].Pcamada;
		 celula[i].sLL=celula[i-1].sL;
		 celula[i].sWL=celula[i-1].sW;
		 celula[i-1].sLR=celula[i].sL;
		 celula[i-1].sWR=celula[i].sW;
		 celula[i].QwcamadaL=celula[i-1].QwcamadaR;
		 celula[i].QocamadaL=celula[i-1].QocamadaR;
		 celula[i].QgcamadaL=celula[i-1].QgcamadaR;
	 }
	 celula[ncel-1].PcamadaR=celula[ncel-1].Pcamada;
	 celula[ncel-1].sLR=celula[ncel-1].sL;
	 celula[ncel-1].sWR=celula[ncel-1].sW;
}

 FullMtx<double> PorosRad::perfil(){
	 //int nglob=0;
	 //for(int i=0;i<geom.ncamadas;i++)nglob+=ncamada[i];
	FullMtx<double> matrizsaida(nglobal,11);
	 int konta=1;
	 matrizsaida[0][0]=geom.a/2.;
	 matrizsaida[0][1]=matrizsaida[0][0]*100./2.54;
	 int idisc=0;
	 //matrizsaida[0][2]=Pcamada[0][0];
	 matrizsaida[0][2]=celula[idisc].Pcamada;
	 //matrizsaida[0][3]=Qcamada[0][0];
	 matrizsaida[0][3]=celula[idisc].QcamadaR*86400;
	 matrizsaida[0][4]=celula[idisc].QocamadaR*(1-celula[idisc].alf)*86400;
	 matrizsaida[0][5]=celula[idisc].QocamadaR*celula[idisc].alf*86400;
	 matrizsaida[0][6]=celula[idisc].QwcamadaR*86400;
	 matrizsaida[0][7]=celula[idisc].sL;
	 matrizsaida[0][8]=celula[idisc].sW;
	 matrizsaida[0][9]=celula[idisc].alf;
	 matrizsaida[0][10]=celula[idisc].BSW;
	  for(int i=0;i<geom.ncamadas;i++){
	 	 for(int j=1;j<=ncamada[i];j++){
	 		 	idisc++;
	 			if(i>0)matrizsaida[konta][0]=geom.diamC[i-1]/2.+(j)*drcamada[i];
	 			else matrizsaida[konta][0]=geom.a/2.+(j)*drcamada[i];
	 			matrizsaida[konta][1]=matrizsaida[konta][0]*100./2.54;
	 		//matrizsaida[konta][2]=Pcamada[i][j];
	 			matrizsaida[konta][2]=celula[idisc].Pcamada;
	 		//matrizsaida[konta][3]=Qcamada[i][j];
	 			matrizsaida[konta][3]=celula[idisc].QcamadaR*86400;
	 			matrizsaida[konta][4]=celula[idisc].QocamadaR*(1-celula[idisc].alf)*86400;
	 			matrizsaida[konta][5]=celula[idisc].QocamadaR*celula[idisc].alf*86400;
	 			matrizsaida[konta][6]=celula[idisc].QwcamadaR*86400;
	 			matrizsaida[konta][7]=celula[idisc].sL;
	 			matrizsaida[konta][8]=celula[idisc].sW;
	 			matrizsaida[konta][9]=celula[idisc].alf;
	 			matrizsaida[konta][10]=celula[idisc].BSW;
	 			konta+=1;
	 	 }
	  }
	  return matrizsaida;
}

//template class PorosRad;
