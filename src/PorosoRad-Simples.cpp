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
#include "PorosoRad-Simples.h"


PorosRadSimp::PorosRadSimp(){
	  iniciaVariaveis();
}

PorosRadSimp::PorosRadSimp(string nomeArquivoEntrada,varGlob1D* Vvg1dSP){
	  iniciaVariaveis();
	  lerDados(nomeArquivoEntrada);
	  ////////////////////////////////////////////////////////////////////////
	  double* vkx=0;
	  double* vky=0;
	  double* vporo=0;
	  double* vcompRoc=0;
	  vg1dSP=Vvg1dSP;
	  if(corte.ncam>0){
		  int cam = corte.ncam;
		  vkx = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vkx[j] = mat[corte.indmat[j]].kX;
		  vky = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vky[j] = mat[corte.indmat[j]].kX;
		  vporo = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vporo[j] = mat[corte.indmat[j]].poro;
		  vcompRoc = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vcompRoc[j] = mat[corte.indmat[j]].compRoc;
	  }
    //DadosGeoPoro(double va=0 ,double vb=0,int vn=0, double* vkX=0, double* vdiamC=0,double* vkY=0,
     		 //double* vporo=0, double* vcompRoc=0)

	  dutosMRT = DadosGeoPoro(corte.a, corte.b,	corte.ncam, vkx, corte.diam, vky, vporo, vcompRoc);
	  //////////////////////////////////////////////////////////////////////////////////////////
	  int* vncamada=0;
	  double* vdrcamada=0;
	  double** vPcamada=0;
	  double** vsO=0;
	  double** vsW=0;
	  if(dutosMRT.ncamadas>0){
		vncamada = new int[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) vncamada[i] = corte.discre[i];
		vdrcamada = new double[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			if (i > 0)
				vdrcamada[i] = 0.5 * (dutosMRT.diamC[i] - dutosMRT.diamC[i - 1])
						/ vncamada[i];
			else
				vdrcamada[0] = 0.5 * (dutosMRT.diamC[0] - dutosMRT.a) / vncamada[0];
		}
		vPcamada = new double*[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			vPcamada[i] = new double[vncamada[i] + 1];
		}

		int equilterm=1;

		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			for (int j = 0; j <= vncamada[i]; j++)
				vPcamada[i][j] = pRes[0];
		}
		vsO = new double*[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			vsO[i] = new double[vncamada[i] + 1];
		}


		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			for (int j = 0; j <= vncamada[i]; j++)
				vsO[i][j] = satLRes[0];
		}
		vsW = new double*[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			vsW[i] = new double[vncamada[i] + 1];
		}


		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			for (int j = 0; j <= vncamada[i]; j++)
				vsW[i][j] = satARes[0];
		}
	  }

		int perm = -1;

		pInt=pW.val[0];

		/*
		 * 	PorosRadSimp(vgeom,vpermanente, vncamada,vdrcamada,vPcamada,
		 * 	vPint,vTextern1,vtRes,vdt,pres,vdh=1,vfluc)
		 */

		/*transfer = PorosRadSimp(dutosMRT, perm, vncamada, vdrcamada, vPcamada,
				pInt,pRes2, pRes[0], tRes,temp.dtmax[0],pW.val[0],corte.zDatum0-zDatumRef,
				corte.zDatum1-zDatumRef,espessuraRes,
				vsO,vsW,satLRes[0],satARes[0],satAPoc[0],satAconat,ippoco,
				kRelOA,kRelOG,pcOA,pcGO,fluc,flup); // alteracao2 // alteracao0*/



		geom=dutosMRT;
		permanente=perm;
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

		zD0=corte.zDatum0-zDatumRef;
		zD1=corte.zDatum1-zDatumRef;


		dh=espessuraRes;
		dtL=0.;

		sLRes=satLRes[0];
		sWRes=satARes[0];
		sWPoc=satAPoc[0];

		satConata=satAconat;
		ip=ippoco;
		pBolha=1000.;

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

			Pint=pW.val[0];
			presRes=pRes[0];
			presRes2=pRes2;
			dt=temp.dtmax[0];

			geraCel(vPcamada,vsO,vsW,satLRes[0],satARes[0]);
		}
	    if(corte.ncam>0){
	  	  delete[] vkx;
	  	  delete[] vky;
	  	  delete[] vporo;
	  	  delete[] vcompRoc;
	    }

	    if(dutosMRT.ncamadas>0){
	    	delete[] vncamada;
	    	delete[] vdrcamada;
	    	for (int i = 0; i < dutosMRT.ncamadas; i++)
	    		delete[] vPcamada[i];
	    	delete[] vPcamada;
	    	delete[] vsO;
	    	delete[] vsW;
	    }
}


/*PorosRadSimp::PorosRadSimp(const DadosGeoPoro vgeom,const int vpermanente, const int* const vncamada,
		           const double* const vdrcamada,double** const vPcamada,const double vPint,
					const double vpresRes2,const double vpresRes,const double vtRes, const double vdt,
		           const double pres, const double vzD0, const double vzD1, const double vdh,
				   double** const vsL,double** const vsW,const double vsLRes,const double vsWRes,
					const double vsWPoc,const double vsatConata,const double vip,
					tabelaPemRelOA vkRelOA,tabelaPemRelOG vkRelOG,tabelaPresCapOA vpcOA,tabelaPresCapGO vpcGO,
					ProFluCol vfluc, ProFlu vflup){

	iniciaVariaveis();
	geom=vgeom;
	permanente=vpermanente;
	erroPermanente=1000.;
	acessorio=1;
	DfluxIni=0.;
	DfluxIniG=0.;
	DfluxIniA=0.;
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

	kontaTempoImp=0;
	posicMarlim=0;

	kontaRenovaComp=0;

	tipoTabelaDinamica=0;

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
	  presRes2=vpresRes2;
      dt=vdt;

      geraCel(vPcamada,vsL,vsW,vsLRes,vsWRes);


	}
	else{
		lenth=0;
		ncamada=0;
		drcamada=0;
		Pint=0;
		presRes=0;
		tRes=0;
	    dt=0;
	    nglobal=0;
	    celula=0;

		mat=0;
		pRes=0;
		tempoPRes=0;
		nPRes=0;

		npontos=0;
		zdranP=0;
		dzdpP=0;
		dzdtP=0;
		nvecEmul=0;
		BSWVec=0;
		emulVec=0;
		corrDeng=0;

		satARes=0;
		satLRes=0;
		satAPoc=0;
		tempoSatARes=0;
		(*vg1dSP).tempoSatLRes=0;
		(*vg1dSP).tempoSatAPoc=0;

		nsatARes=0;
		nsatLRes=0;
		nsatAPoc=0;

		reinicia=0;

		kontaTempoImp=0;
		posicMarlim=0;
	}


}*/


PorosRadSimp::PorosRadSimp(const PorosRadSimp& vPorosRadSimp){
	  iniciaVariaveis();
	  vg1dSP=vPorosRadSimp.vg1dSP;
	  lerDados(vPorosRadSimp.entrada);
	  ////////////////////////////////////////////////////////////////////////
	  double* vkx=0;
	  double* vky=0;
	  double* vporo=0;
	  double* vcompRoc=0;
	  if(corte.ncam>0){
		  int cam = corte.ncam;
		  vkx = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vkx[j] = mat[corte.indmat[j]].kX;
		  vky = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vky[j] = mat[corte.indmat[j]].kX;
		  vporo = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vporo[j] = mat[corte.indmat[j]].poro;
		  vcompRoc = new double[cam];
		  for (int j = 0; j < cam; j++)
			  vcompRoc[j] = mat[corte.indmat[j]].compRoc;
	  }
  //DadosGeoPoro(double va=0 ,double vb=0,int vn=0, double* vkX=0, double* vdiamC=0,double* vkY=0,
   		 //double* vporo=0, double* vcompRoc=0)

	  dutosMRT = DadosGeoPoro(corte.a, corte.b,	corte.ncam, vkx, corte.diam, vky, vporo, vcompRoc);
	  //////////////////////////////////////////////////////////////////////////////////////////
	  int* vncamada=0;
	  double* vdrcamada=0;
	  double** vPcamada=0;
	  double** vsO=0;
	  double** vsW=0;
	  if(dutosMRT.ncamadas>0){
		vncamada = new int[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) vncamada[i] = corte.discre[i];
		vdrcamada = new double[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			if (i > 0)
				vdrcamada[i] = 0.5 * (dutosMRT.diamC[i] - dutosMRT.diamC[i - 1])
						/ vncamada[i];
			else
				vdrcamada[0] = 0.5 * (dutosMRT.diamC[0] - dutosMRT.a) / vncamada[0];
		}
		vPcamada = new double*[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			vPcamada[i] = new double[vncamada[i] + 1];
		}

		int equilterm=1;

		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			for (int j = 0; j <= vncamada[i]; j++)
				vPcamada[i][j] = pRes[0];
		}
		vsO = new double*[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			vsO[i] = new double[vncamada[i] + 1];
		}


		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			for (int j = 0; j <= vncamada[i]; j++)
				vsO[i][j] = satLRes[0];
		}
		vsW = new double*[dutosMRT.ncamadas];
		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			vsW[i] = new double[vncamada[i] + 1];
		}


		for (int i = 0; i < dutosMRT.ncamadas; i++) {
			for (int j = 0; j <= vncamada[i]; j++)
				vsW[i][j] = satARes[0];
		}
	  }

		int perm = -1;

		pInt=pW.val[0];

		/*
		 * 	PorosRadSimp(vgeom,vpermanente, vncamada,vdrcamada,vPcamada,
		 * 	vPint,vTextern1,vtRes,vdt,pres,vdh=1,vfluc)
		 */

		/*transfer = PorosRadSimp(dutosMRT, perm, vncamada, vdrcamada, vPcamada,
				pInt,pRes2, pRes[0], tRes,temp.dtmax[0],pW.val[0],corte.zDatum0-zDatumRef,
				corte.zDatum1-zDatumRef,espessuraRes,
				vsO,vsW,satLRes[0],satARes[0],satAPoc[0],satAconat,ippoco,
				kRelOA,kRelOG,pcOA,pcGO,fluc,flup); // alteracao2 // alteracao0*/



		geom=dutosMRT;
		permanente=perm;
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

		zD0=corte.zDatum0-zDatumRef;
		zD1=corte.zDatum1-zDatumRef;


		dh=espessuraRes;
		dtL=0.;

		sLRes=satLRes[0];
		sWRes=satARes[0];
		sWPoc=satAPoc[0];

		satConata=satAconat;
		ip=ippoco;

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

			Pint=pW.val[0];
			presRes=pRes[0];
			presRes2=pRes2;
			dt=temp.dtmax[0];

			geraCel(vPcamada,vsO,vsW,satLRes[0],satARes[0]);
		}
	    if(corte.ncam>0){
	  	  delete[] vkx;
	  	  delete[] vky;
	  	  delete[] vporo;
	  	  delete[] vcompRoc;
	    }

	    if(dutosMRT.ncamadas>0){
	    	delete[] vncamada;
	    	delete[] vdrcamada;
	    	for (int i = 0; i < dutosMRT.ncamadas; i++)
	    		delete[] vPcamada[i];
	    	delete[] vPcamada;
	    	delete[] vsO;
	    	delete[] vsW;
	    }
	    pBolha=1000.;
}

/*PorosRadSimp::PorosRadSimp(const PorosRadSimp& vPorosRadSimp){

	ncel=vPorosRadSimp.ncel;
	geom=vPorosRadSimp.geom;
	permanente=vPorosRadSimp.permanente;
	lenth=geom.ncamadas;

	resFim=vPorosRadSimp.resFim;

	condiTparede=vPorosRadSimp.condiTparede;

	dh=vPorosRadSimp.dh;
	dtL=vPorosRadSimp.dtL;

	he=vPorosRadSimp.he;
	hi=vPorosRadSimp.hi;

	fluc=vPorosRadSimp.fluc;
	flup=vPorosRadSimp.flup;

    zD0=vPorosRadSimp.zD0;
    zD1=vPorosRadSimp.zD1;

	tRes=vPorosRadSimp.tRes;
	sLRes=vPorosRadSimp.sLRes;
	sWRes=vPorosRadSimp.sWRes;
	sWPoc=vPorosRadSimp.sWPoc;

	kRelOA=vPorosRadSimp.kRelOA;
	kRelOG=vPorosRadSimp.kRelOG;
	pcOA=vPorosRadSimp.pcOA;
	pcGO=vPorosRadSimp.pcGO;

	satConata=vPorosRadSimp.satConata;
	ip=vPorosRadSimp.ip;

	if(lenth>0){
		ncamada= new int [lenth];
		for(int i=0;i<lenth;i++)ncamada[i]=vPorosRadSimp.ncamada[i];
		drcamada=new double [lenth];
		for(int i=0;i<lenth;i++)drcamada[i]=vPorosRadSimp.drcamada[i];

		 nglobal=1;
		 for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

		Pint=vPorosRadSimp.Pint;
		presRes=vPorosRadSimp.presRes;
		presRes2=vPorosRadSimp.presRes2;
		tRes=vPorosRadSimp.tRes;
	    dt=vPorosRadSimp.dt;

		fluxIni=vPorosRadSimp.fluxIni;
		fluxFim=vPorosRadSimp.fluxFim;
		fluxIniA=vPorosRadSimp.fluxIniA;
		fluxFimA=vPorosRadSimp.fluxFimA;
		fluxIniG=vPorosRadSimp.fluxIniG;
		fluxFimG=vPorosRadSimp.fluxFimG;
		resGlob=vPorosRadSimp.resGlob;

		celula=new celradSimp [ncel];
		for(int i=0;i<ncel;i++)celula[i]=vPorosRadSimp.celula[i];

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
		resGlob=0.;
		celula=0;

		mat=0;
		pRes=0;
		tempoPRes=0;
		nPRes=0;

		npontos=0;
		zdranP=0;
		dzdpP=0;
		dzdtP=0;
		nvecEmul=0;
		BSWVec=0;
		emulVec=0;
		corrDeng=0;

		satARes=0;
		satLRes=0;
		satAPoc=0;
		tempoSatARes=0;
		(*vg1dSP).tempoSatLRes=0;
		(*vg1dSP).tempoSatAPoc=0;

		nsatARes=0;
		nsatLRes=0;
		nsatAPoc=0;

		reinicia=0;

	}

}*/


PorosRadSimp& PorosRadSimp::operator =(const PorosRadSimp& vPorosRadSimp){

	if (this != &vPorosRadSimp && vPorosRadSimp.geom.ncamadas>0) {
	    if(tabp>0 && npontos>0){
			  for(int i=0; i<npontos+2;i++) delete[] zdranP[i];
			  delete [] zdranP;
			  for(int i=0; i<npontos+2;i++) delete[] dzdpP[i];
			  delete [] dzdpP;
			  for(int i=0; i<npontos+2;i++) delete[] dzdtP[i];
			  delete [] dzdtP;
	     }

	    if(flashCompleto==1 && tabent.npont>0){
		   for(int j=0; j<tabent.npont+1;j++) delete [] flash.rholF[j];
		   delete [] flash.rholF;
		   for(int j=0; j<tabent.npont+1;j++) delete[] flash.rhogF[j];
		   delete [] flash.rhogF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDpF[j];
	       delete [] flash.DrholDpF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDpF[j];
	       delete [] flash.DrhogDpF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDtF[j];
	       delete [] flash.DrholDtF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDtF[j];
	       delete [] flash.DrhogDtF;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhowDtF[j];
	       delete [] flash.DrhowDtF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.RSF[j];
	       delete [] flash.RSF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cplF[j];
	       delete [] flash.cplF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpgF[j];
	       delete [] flash.cpgF;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpwF[j];
	       delete [] flash.cpwF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HlF[j];
	       delete [] flash.HlF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HgF[j];
	       delete [] flash.HgF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigOGF[j];
	       delete [] flash.sigOGF;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigWGF[j];
	       delete [] flash.sigWGF;
	       for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscO[j];//alteracao6
	       delete [] flash.viscO;
           for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscG[j];//alteracao6
	       delete [] flash.viscG;

	       delete [] flash.PBF;
	       delete [] flash.TBF;
	   }

	   if(nvecEmul>0){
	    	  delete [] BSWVec;
	    	  delete [] emulVec;
	    }
		if(flup.corrOM==7){
			if(tabVisc.parserie>0){
				delete [] tabVisc.visc;
				delete [] tabVisc.temp;
			}
		}
		if(nsatARes>0){
			delete [] satARes;
			delete [] tempoSatARes;
		}
		if(nsatLRes>0){
			delete [] satLRes;
			delete [] tempoSatLRes;
		}
		if(nsatAPoc>0){
			delete [] satAPoc;
			delete [] tempoSatAPoc;
		}
		if(nPRes>0){
			delete [] pRes;
			delete [] tempoPRes;
		}


        if(/*ncamada &&*/ nglobal>0) delete[] ncamada;
        if(/*drcamada*/ nglobal>0) delete[] drcamada;
        if(ncel>0){
        		delete [] celula;
        }

  	  iniciaVariaveis();
  	  vg1dSP=vPorosRadSimp.vg1dSP;
  	  lerDados(vPorosRadSimp.entrada);
  	  ////////////////////////////////////////////////////////////////////////
  	  double* vkx=0;
  	  double* vky=0;
  	  double* vporo=0;
  	  double* vcompRoc=0;
  	  if(corte.ncam>0){
  		  int cam = corte.ncam;
  		  vkx = new double[cam];
  		  for (int j = 0; j < cam; j++)
  			  vkx[j] = mat[corte.indmat[j]].kX;
  		  vky = new double[cam];
  		  for (int j = 0; j < cam; j++)
  			  vky[j] = mat[corte.indmat[j]].kX;
  		  vporo = new double[cam];
  		  for (int j = 0; j < cam; j++)
  			  vporo[j] = mat[corte.indmat[j]].poro;
  		  vcompRoc = new double[cam];
  		  for (int j = 0; j < cam; j++)
  			  vcompRoc[j] = mat[corte.indmat[j]].compRoc;
  	  }
    //DadosGeoPoro(double va=0 ,double vb=0,int vn=0, double* vkX=0, double* vdiamC=0,double* vkY=0,
     		 //double* vporo=0, double* vcompRoc=0)

  	  dutosMRT = DadosGeoPoro(corte.a, corte.b,	corte.ncam, vkx, corte.diam, vky, vporo, vcompRoc);
  	  //////////////////////////////////////////////////////////////////////////////////////////
  	  int* vncamada=0;
  	  double* vdrcamada=0;
  	  double** vPcamada=0;
  	  double** vsO=0;
  	  double** vsW=0;
  	  if(dutosMRT.ncamadas>0){
  		vncamada = new int[dutosMRT.ncamadas];
  		for (int i = 0; i < dutosMRT.ncamadas; i++) vncamada[i] = corte.discre[i];
  		vdrcamada = new double[dutosMRT.ncamadas];
  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			if (i > 0)
  				vdrcamada[i] = 0.5 * (dutosMRT.diamC[i] - dutosMRT.diamC[i - 1])
  						/ vncamada[i];
  			else
  				vdrcamada[0] = 0.5 * (dutosMRT.diamC[0] - dutosMRT.a) / vncamada[0];
  		}
  		vPcamada = new double*[dutosMRT.ncamadas];
  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			vPcamada[i] = new double[vncamada[i] + 1];
  		}

  		int equilterm=1;

  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			for (int j = 0; j <= vncamada[i]; j++)
  				vPcamada[i][j] = pRes[0];
  		}
  		vsO = new double*[dutosMRT.ncamadas];
  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			vsO[i] = new double[vncamada[i] + 1];
  		}


  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			for (int j = 0; j <= vncamada[i]; j++)
  				vsO[i][j] = satLRes[0];
  		}
  		vsW = new double*[dutosMRT.ncamadas];
  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			vsW[i] = new double[vncamada[i] + 1];
  		}


  		for (int i = 0; i < dutosMRT.ncamadas; i++) {
  			for (int j = 0; j <= vncamada[i]; j++)
  				vsW[i][j] = satARes[0];
  		}
  	  }

  		int perm = -1;

  		pInt=pW.val[0];

  		/*
  		 * 	PorosRadSimp(vgeom,vpermanente, vncamada,vdrcamada,vPcamada,
  		 * 	vPint,vTextern1,vtRes,vdt,pres,vdh=1,vfluc)
  		 */

  		/*transfer = PorosRadSimp(dutosMRT, perm, vncamada, vdrcamada, vPcamada,
  				pInt,pRes2, pRes[0], tRes,temp.dtmax[0],pW.val[0],corte.zDatum0-zDatumRef,
  				corte.zDatum1-zDatumRef,espessuraRes,
  				vsO,vsW,satLRes[0],satARes[0],satAPoc[0],satAconat,ippoco,
  				kRelOA,kRelOG,pcOA,pcGO,fluc,flup); // alteracao2 // alteracao0*/



  		geom=dutosMRT;
  		permanente=perm;
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

  		zD0=corte.zDatum0-zDatumRef;
  		zD1=corte.zDatum1-zDatumRef;


  		dh=espessuraRes;
  		dtL=0.;

  		sLRes=satLRes[0];
  		sWRes=satARes[0];
  		sWPoc=satAPoc[0];

  		satConata=satAconat;
  		ip=ippoco;

  		BSW=0;

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

  			Pint=pW.val[0];
  			presRes=pRes[0];
  			presRes2=pRes2;
  			dt=temp.dtmax[0];

  			geraCel(vPcamada,vsO,vsW,satLRes[0],satARes[0]);
  		}
  	    if(corte.ncam>0){
  	  	  delete[] vkx;
  	  	  delete[] vky;
  	  	  delete[] vporo;
  	  	  delete[] vcompRoc;
  	    }

  	    if(dutosMRT.ncamadas>0){
  	    	delete[] vncamada;
  	    	delete[] vdrcamada;
  	    	for (int i = 0; i < dutosMRT.ncamadas; i++)
  	    		delete[] vPcamada[i];
  	    	delete[] vPcamada;
  	    	delete[] vsO;
  	    	delete[] vsW;
  	    }

	}

	return *this;
}

/*PorosRadSimp& PorosRadSimp::operator =(const PorosRadSimp& vPorosRadSimp){

	if(lenth>0){
	  if(this!=&vPorosRadSimp){
		 ncel=vPorosRadSimp.ncel;
	     geom=vPorosRadSimp.geom;
	     permanente=vPorosRadSimp.permanente;
	 	 resFim=vPorosRadSimp.resFim;

	 	condiTparede=vPorosRadSimp.condiTparede;
		he=vPorosRadSimp.he;
		hi=vPorosRadSimp.hi;
		fluc=vPorosRadSimp.fluc;
		flup=vPorosRadSimp.flup;

	    zD0=vPorosRadSimp.zD0;
	    zD1=vPorosRadSimp.zD1;


	 	dh=vPorosRadSimp.dh;
	 	dtL=vPorosRadSimp.dtL;

		tRes=vPorosRadSimp.tRes;
		sLRes=vPorosRadSimp.sLRes;
		sWRes=vPorosRadSimp.sWRes;
		sWPoc=vPorosRadSimp.sWPoc;

		kRelOA=vPorosRadSimp.kRelOA;
		kRelOG=vPorosRadSimp.kRelOG;
		pcOA=vPorosRadSimp.pcOA;
		pcGO=vPorosRadSimp.pcGO;

		satConata=vPorosRadSimp.satConata;
		ip=vPorosRadSimp.ip;

	     if(lenth!=vPorosRadSimp.lenth)cout<<"dimensoes de vetores diferem em número de camadas, impossivel igualar na PorosRadSimp";
		 for(int i=0;i<lenth;i++)drcamada[i]=vPorosRadSimp.drcamada[i];
		 for(int i=0; i<lenth; i++){
	       if(ncamada[i]>0){
	    	  if(ncamada[i]!=vPorosRadSimp.ncamada[i])cout<<"dimensoes de vetores diferem em número de discretizações, impossivel igualar na PorosRadSimp";
		    }
		 }
		 for(int i=0; i<lenth; i++){
	       if(ncamada[i]>0){
	    	  if(ncamada[i]!=vPorosRadSimp.ncamada[i])cout<<"dimensoes de vetores diferem em número de discretizações, impossivel igualar na PorosRadSimp";
		    }
		 }

		 nglobal=1;
		 for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

		 Pint=vPorosRadSimp.Pint;
		 presRes=vPorosRadSimp.presRes;
		 presRes2=vPorosRadSimp.presRes2;
	     tRes=vPorosRadSimp.tRes;
	     dt=vPorosRadSimp.dt;

		 for(int i=0;i<ncel;i++)celula[i]=vPorosRadSimp.celula[i];


      }
	}
	else if(lenth==0){
		if(this!=&vPorosRadSimp){
		  ncel=vPorosRadSimp.ncel;
		  geom=vPorosRadSimp.geom;
		  permanente=vPorosRadSimp.permanente;
		  lenth=geom.ncamadas;
		  resFim=vPorosRadSimp.resFim;

		  condiTparede=vPorosRadSimp.condiTparede;



		  dh=vPorosRadSimp.dh;
	      dtL=vPorosRadSimp.dtL;
	      he=vPorosRadSimp.he;
	      hi=vPorosRadSimp.hi;
	      fluc=vPorosRadSimp.fluc;
	      flup=vPorosRadSimp.flup;

	      zD0=vPorosRadSimp.zD0;
	      zD1=vPorosRadSimp.zD1;

	      tRes=vPorosRadSimp.tRes;
	      sLRes=vPorosRadSimp.sLRes;
	      sWRes=vPorosRadSimp.sWRes;
	      sWPoc=vPorosRadSimp.sWPoc;

	  	  kRelOA=vPorosRadSimp.kRelOA;
	  	  kRelOG=vPorosRadSimp.kRelOG;
	  	  pcOA=vPorosRadSimp.pcOA;
	  	  pcGO=vPorosRadSimp.pcGO;

	  	  satConata=vPorosRadSimp.satConata;
	  	  ip=vPorosRadSimp.ip;

		  if(lenth>0){
			 ncamada= new int [lenth];
			 for(int i=0;i<lenth;i++)ncamada[i]=vPorosRadSimp.ncamada[i];
			 drcamada=new double [lenth];
			 for(int i=0;i<lenth;i++)drcamada[i]=vPorosRadSimp.drcamada[i];


		    nglobal=1;
		    for(int i=0;i<geom.ncamadas;i++)nglobal+=ncamada[i];

			Pint=vPorosRadSimp.Pint;
			presRes=vPorosRadSimp.presRes;
			presRes2=vPorosRadSimp.presRes2;
			tRes=vPorosRadSimp.tRes;
			dt=vPorosRadSimp.dt;
			fluxIni=vPorosRadSimp.fluxIni;
			fluxFim=vPorosRadSimp.fluxFim;
			fluxIniA=vPorosRadSimp.fluxIniA;
			fluxFimA=vPorosRadSimp.fluxFimA;
			fluxIniG=vPorosRadSimp.fluxIniG;
			fluxFimG=vPorosRadSimp.fluxFimG;
			resGlob=vPorosRadSimp.resGlob;

			celula=new celradSimp [ncel];
			for(int i=0;i<ncel;i++)celula[i]=vPorosRadSimp.celula[i];
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
			resGlob=0.;
			celula=0;

			mat=0;
			pRes=0;
			tempoPRes=0;
			nPRes=0;

			npontos=0;
			zdranP=0;
			dzdpP=0;
			dzdtP=0;
			nvecEmul=0;
			BSWVec=0;
			emulVec=0;
			corrDeng=0;

			satARes=0;
			satLRes=0;
			satAPoc=0;
			tempoSatARes=0;
			(*vg1dSP).tempoSatLRes=0;
			(*vg1dSP).tempoSatAPoc=0;

			nsatARes=0;
			nsatLRes=0;
			nsatAPoc=0;

			reinicia=0;
		}
	}
  }

 return *this;
}*/

void PorosRadSimp::iniciaVariaveis(){
    satConata=0;
    ip=0;
    condiTparede=0;
    condLoc=0;
    tabp=0;
    resFim=0;
    sWRes=0;
    sWPoc=0;
    pRes2=0;
    condGlob=0;
    he=0;
    hi=0;
    permanente=0;
    zD0=0;
    zDatumRef=0;
    espessuraRes=0;
    dtL=0;
    pInt=0;
	ippoco=0;
	zD1=0;
	qTotal=0;
	satAconat=0;
	sLRes=0;
	dh=0;

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
	celula=0;

	mat=0;
	nmaterialPoroso=0;
	pRes=0;
	tempoPRes=0;
	nPRes=0;

	npontos=0;
	zdranP=0;
	dzdpP=0;
	dzdtP=0;
	nvecEmul=0;
	BSWVec=0;
	emulVec=0;
	corrDeng=0;

	satARes=0;
	satLRes=0;
	satAPoc=0;
	tempoSatARes=0;
	tempoSatLRes=0;
	tempoSatAPoc=0;

	nsatARes=0;
	nsatLRes=0;
	nsatAPoc=0;

	reinicia=0;
	erroPermanente=1000.;
	acessorio=1;

	DfluxIni=0.;
	DfluxIniG=0.;
	DfluxIniA=0.;

	kontaTempoImp=0;

	posicMarlim=0;

	kontaRenovaComp=0;
	tipoTabelaDinamica=0;

	celula=0;
	ncel=0;
	nPRes=0;

	geom=DadosGeoPoro();
	fluc=ProFluCol();
	flup=ProFlu();

	dutosMRT=DadosGeoPoro();
	corte=cortePoroso();
	pW=detDiriPoroso();
	temp=detTempoPoroso();

	kRelOA=tabelaPemRelOA();
	kRelOG=tabelaPemRelOG();
	pcOA=tabelaPresCapOA();
	pcGO=tabelaPresCapGO();

	ntabDin=0;
	tipoTabelaDinamica=0;
	kontaRenovaComp=0;
	posicMarlim=0;
	miniTabDt=0;
	miniTabDp=0;
	miniTabAtraso=0;
	//tabDin=vector<tabelaDinamica>(0);

    flash.rholF=0;
    flash.rhogF=0;
    flash.DrholDpF=0;
    flash.DrhogDpF=0;
    flash.DrholDtF=0;
    flash.DrhogDtF=0;
    flash.DrhowDtF=0;
    flash.RSF=0;
    flash.cplF=0;
    flash.cpgF=0;
    flash.cpwF=0;
    flash.HlF=0;
    flash.HgF=0;
    flash.sigOGF=0;
    flash.sigWGF=0;
    flash.viscO=0;
    flash.viscG=0;
    flash.PBF=0;
    flash.TBF=0;

    pInt0=pInt;

    flashCompleto=0;
    usaTabela=0;

    temp.parserie=0;
    temp.parserieImp=0;
    corte.ncam=0;

    BSW=0;
    pBolha=1000.;
	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;

	vg1dSP=0;
}


void PorosRadSimp::lerDados(string nomeArquivoEntrada){

	 entrada=nomeArquivoEntrada;
	 FILE *fp = fopen(nomeArquivoEntrada.c_str(), "r");
		//FILE *fp = fopen("/home/bwz1/teste2.json", "r");
	  char readBuffer[125536];
	  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	  Document elementoRaiz;
	  elementoRaiz.ParseStream(is);
	  fclose(fp);


	  assert(elementoRaiz.HasMember("tempo"));

	  if (elementoRaiz.HasMember("tempo")){
				parse_tempo(elementoRaiz["tempo"]);
	  } else {
				exit(-1);
	  }

	  parse_materiais(elementoRaiz["material"]);

	  parse_kRelOA(elementoRaiz["kRelOA"]);
	  parse_kRelOG(elementoRaiz["kRelOG"]);
	  parse_pcOA(elementoRaiz["pcOA"]);
	  //parse_pcGO(elementoRaiz["pcGO"]);

	  if(elementoRaiz.HasMember("fluidoComplementar"))parse_fluido_complementar(elementoRaiz["fluidoComplementar"]);
	  else{
			int tipoflup=0;
		    double masesp=0.;
			double compP = 0.;
			double compT = 0.;
			double tensup = 0.;
			double calesp = 0.;
			double condut = 0.;
			double templ = 0.;
			double lvisl = 0.;
			double temph = 0.;
			double lvish = 0.;
			double sal =0.;
			fluc = ProFluCol(masesp, compP, compT, tensup, calesp,
						condut, sal, templ, lvisl, temph, lvish,0, tipoflup);
			fluc.npontos = 0;
	  }
	  if(tabp>0){
			// realizar o parse da chave tabela
			parse_tabela(elementoRaiz["tabelaZ"]);
			zdranP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
					zdranP[i] = new double[npontos + 2];
			dzdpP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
					dzdpP[i] = new double[npontos + 2];
			dzdtP = new double*[npontos + 2];
			for (int i = 0; i < npontos + 2; i++)
					dzdtP[i] = new double[npontos + 2];

	  }
	  parse_fluido_producao(elementoRaiz["fluidosProducao"]);
	  pBolha=flup.PB(1., tRes)/(0.9678411*14.69595);

	  if(tabp>0){
			double TCMin = 0;  //15-06-2018
			double TCMax = 0;  //15-06-2018
			double PCMin = 0;  //15-06-2018
			double PCMax = 0;  //15-06-2018
			if(corrDeng==0){
				TCMin = flup.TC;  //15-06-2018
				TCMax = flup.TC;  //15-06-2018
				PCMin = flup.PC;  //15-06-2018
				PCMax = flup.PC;  //15-06-2018
			}
			else{
				double PC= 708.75 - 57.5* 0.5538;
				double TC= 169 + 314.0* 0.5538;
				TCMin = TC;
				TCMax = TC;
				PCMin = PC;
				PCMax = PC;
			}
			if (flup.TC < TCMin)
							TCMin = flup.TC;
			if (flup.TC > TCMax)
							TCMax = flup.TC;
			if (flup.PC < PCMin)
							PCMin = flup.PC;
			if (flup.PC > PCMax)
							PCMax = flup.PC;
			tabentCrit.npont = tabent.npont;  //15-06-2018
			tabentCrit.tmin = (tabent.tmin + 273.15)
							/ ((TCMax - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
			tabentCrit.tmax = (tabent.tmax + 273.15)
							/ ((TCMin - 460. - 32.) / 1.8 + 273.15);  //15-06-2018
			tabentCrit.pmin = tabent.pmin * 0.9678411 * 14.69595 / PCMax; //15-06-2018
			tabentCrit.pmax = tabent.pmax * 0.9678411 * 14.69595 / PCMin; //15-06-2018

			flup.tabela(tabentCrit.pmax, tabentCrit.tmax, tabentCrit.pmin,
							tabentCrit.tmin);
			flup.tabelaDZDP(tabentCrit.pmax, tabentCrit.tmax,
							tabentCrit.pmin, tabentCrit.tmin);
			flup.tabelaDZDT(tabentCrit.pmax, tabentCrit.tmax,
							tabentCrit.pmin, tabentCrit.tmin);
	  }

	  if(tipoTabelaDinamica==1){
		  int minNPontos=0;
		  ntabDin=1;
		  tabelaDinamica temp;
		  temp.npontosP=minNPontos;
		  temp.npontosT=minNPontos;
		  temp.celIni=0;
		  temp.celFim=ncel-1;
		  temp.rhogF=0;
		  temp.rholF=0;
		  temp.DrhogDpF=0;
		  temp.DrhogDtF=0;
		  temp.DrholDpF=0;
		  temp.DrholDtF=0;
		  temp.valBO=0;
		  temp.HgF=0;
		  temp.HlF=0;
		  temp.cpgF=0;
		  temp.cplF=0;
		  temp.valZ=0;
		  temp.valdZdT=0;
		  temp.valdZdP=0;
		  temp.tit=0;
		  temp.rs=0;
		  temp.viscG=0;
		  temp.viscO=0;
		  temp.TBF=0;
		  temp.PBF=0;
		  tabDin.push_back(temp);
	  }

	  if(tipoModelOleo==0){
		  rhoRef=flup.MasEspoleo(pRes2, tRes);
		  double dpPres=0.01*pRes2;
		  compOleo=(1/rhoRef)*(flup.MasEspoleo(pRes2+dpPres, tRes)-rhoRef)/dpPres;
	  }

	  parse_corte(elementoRaiz["secaoTransversal"]);
	  if (elementoRaiz.HasMember("pW")){
		  parse_PW(elementoRaiz["pW"]);
	  }
	  else{
			pW.nserie=1;
			pW.tempo=0;
			pW.val=0;
			pW.valSat=0;
			pW.tempo=new double [pW.nserie];
			pW.val=new double [pW.nserie];
			pW.valSat=new double [pW.nserie];
	  }
}


void PorosRadSimp::parse_tempo(Value& tempo_json) {
	string chaveJson("#/tempo");
/*
 * 	int perm;
	int trans;
	double relax;
	int parserie;
	double* dtmax;
	double* (*vg1dSP).tempo;
 */

	temp.perm=1;
	temp.trans=0;
	temp.relax=1.;
	temp.parserie=0;
	temp.dtmax=0;
	temp.tempo=0;
	temp.tmax=0.;
	temp.tempoImp=0;
	temp.parserieImp=0;
	zDatumRef=0.;
	flashCompleto =0;
	tabp=0;
	miniTabAtraso=10;
	miniTabDp=10.;
	miniTabDt=10.;
	satAconat=0.;
	ippoco=1.;

	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;

	if (tempo_json.HasMember("modeloOleo"))
		tipoModelOleo =
				tempo_json["modeloOleo"].GetInt();

	if (tempo_json.HasMember("modeloFluidoTabelaFlash"))
		flashCompleto =
				tempo_json["modeloFluidoTabelaFlash"].GetInt();
	// caso simulacao com tabela flash - modeloFluidoTabelaFlash: true
	if (flashCompleto == 0) {
		int verifComp=0;
		if (tempo_json.HasMember("modeloFluidoComposicional"))
			verifComp=tempo_json["modeloFluidoComposicional"].GetInt();
		flashCompleto+=2*verifComp;
	}
	if(flashCompleto == 0){
		if (tempo_json.HasMember("tabP"))
			tabp = tempo_json["tabP"].GetInt();
		if (tempo_json.HasMember("correcaoDenGasLivreBlackOil"))
			corrDeng = tempo_json["correcaoDenGasLivreBlackOil"].GetInt();
	}
	if(flashCompleto==2){
	    if (tempo_json.HasMember("miniTabDinAtraso"))
	    	miniTabAtraso=tempo_json["miniTabDinAtraso"].GetInt();
	    if (tempo_json.HasMember("miniTabDinDp"))
	    	miniTabDp=tempo_json["miniTabDinDp"].GetInt();
	    if (tempo_json.HasMember("miniTabDinDt"))
	    	miniTabDt=tempo_json["miniTabDinDt"].GetInt();
	}
	if(flashCompleto==1 || flashCompleto==2){
		if (tempo_json.HasMember("pvtsimArq"))
			pvtsimarq = pathArqExtEntrada
					+ string(tempo_json["pvtsimArq"].GetString());
		else{
			logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'pvtsimArq' inexistente",
					chaveJson, "Condicoes de tempo");
		}
	}
	if(flashCompleto==2){
		if (tempo_json.HasMember("modeloTabelaDinamica"))
			tipoTabelaDinamica=tempo_json["modeloTabelaDinamica"].GetInt();
		else tipoTabelaDinamica=0;
	}

	satAconat = tempo_json["aguaConata"].GetDouble();
	ippoco = tempo_json["ipLocal"].GetDouble();
	tRes = tempo_json["temperaturaReservatorio"].GetDouble();
	espessuraRes = tempo_json["espessuraReservatorio"].GetDouble();

	zDatumRef = tempo_json["datumReferencia"].GetDouble();

	pRes2 = tempo_json["pressaoReferencia"].GetDouble();

	if (tempo_json.HasMember("tmax"))
		temp.tmax =
				tempo_json["tmax"].GetDouble();
	if (tempo_json.HasMember("perm"))
		temp.perm =
				tempo_json["perm"].GetBool();
	if (tempo_json.HasMember("trans"))
		temp.trans =
				tempo_json["trans"].GetBool();
	if (tempo_json.HasMember("relax"))
		temp.relax =
				tempo_json["relax"].GetDouble();

	if (tempo_json.HasMember("dtMax"))
		temp.parserie =
				tempo_json["dtMax"].Size();
	else if(temp.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'dtMax' inexistente",chaveJson, "condicao de tempo");
	if(tempo_json["dtMax"].Size()!=tempo_json["tempo"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves 'dtMax', 'tempo' com dimensoes diferentes",
				chaveJson, "Condicoes de tempo");
	}
	temp.dtmax=new double [temp.parserie];
	temp.tempo=new double [temp.parserie];
	for(int i=0; i<temp.parserie; i++){
		temp.dtmax[i]=tempo_json["dtMax"][i].GetDouble();
		temp.tempo[i]=tempo_json["tempo"][i].GetDouble();
	}


	if (tempo_json.HasMember("tempoImp"))
		temp.parserieImp =
				tempo_json["tempoImp"].Size();
	else if(temp.trans==1)logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempoImp' inexistente para impressão do processo transiente",chaveJson, "condicao de tempo");
	temp.tempoImp=new double [temp.parserieImp];
	for(int i=0; i<temp.parserieImp; i++){
		temp.tempoImp[i]=tempo_json["tempoImp"][i].GetDouble();
	}


	if (tempo_json.HasMember("tempoSatARes"))
		nsatARes =
				tempo_json["tempoSatARes"].Size();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempoSatARes' inexistente",chaveJson, "condicao de tempo");
	if(tempo_json["tempoSatARes"].Size()!=tempo_json["satARes"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves 'tempoSatARes', 'satARes' com dimensoes diferentes",
				chaveJson, "Condicoes de tempo");
	}
	satARes=new double [nsatARes];
	tempoSatARes=new double [nsatARes];
	for(int i=0; i<nsatARes; i++){
		satARes[i]=tempo_json["satARes"][i].GetDouble();
		tempoSatARes[i]=tempo_json["tempoSatARes"][i].GetDouble();
	}

	/*if (tempo_json.HasMember("tempoSatLRes"))
		nsatLRes =
				tempo_json["tempoSatLRes"].Size();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave '(*vg1dSP).tempoSatLRes' inexistente",chaveJson, "condicao de (*vg1dSP).tempo");
	if(tempo_json["(*vg1dSP).tempoSatLRes"].Size()!=tempo_json["satLRes"].Size()){
		logger.log(LOGGER_FALHA,
		LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
				"Chaves '(*vg1dSP).tempoSatLRes', 'satLRes' com dimensoes diferentes",
				chaveJson, "Condicoes de (*vg1dSP).tempo");
	}
	satLRes=new double [nsatLRes];
	(*vg1dSP).tempoSatLRes=new double [nsatLRes];
	for(int i=0; i<nsatLRes; i++){
		satLRes[i]=tempo_json["satLRes"][i].GetDouble();
		(*vg1dSP).tempoSatLRes[i]=tempo_json["(*vg1dSP).tempoSatLRes"][i].GetDouble();
	}*/
	nsatLRes =1.;
	satLRes=new double [nsatLRes];
	tempoSatLRes=new double [nsatLRes];
	for(int i=0; i<nsatLRes; i++){
		satLRes[i]=1.;
		tempoSatLRes[i]=0;
	}

	if (tempo_json.HasMember("tempoSatAPoc"))
		nsatAPoc =
				tempo_json["tempoSatAPoc"].Size();
	else nsatAPoc=1;
	if(tempo_json.HasMember("tempoSatAPoc")){
		if(tempo_json["tempoSatAPoc"].Size()!=tempo_json["satAPoc"].Size()){
			logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'tempoSatAPoc', 'satAPoc' com dimensoes diferentes",
					chaveJson, "Condicoes de tempo");
		}
		satAPoc=new double [nsatAPoc];
		tempoSatAPoc=new double [nsatAPoc];
		for(int i=0; i<nsatAPoc; i++){
			satAPoc[i]=tempo_json["satAPoc"][i].GetDouble();
			tempoSatAPoc[i]=tempo_json["tempoSatAPoc"][i].GetDouble();
		}
	}
	else{
		satAPoc=new double [nsatAPoc];
		tempoSatAPoc=new double [nsatAPoc];
		for(int i=0; i<nsatAPoc; i++){
			satAPoc[i]=satAconat;
			tempoSatAPoc[i]=0.;
		}
	}

	if (tempo_json.HasMember("pressaoReservatorio"))
		nPRes =
				tempo_json["tempoPReservatorio"].Size();
	else nPRes=1;
	if(tempo_json.HasMember("tempoPReservatorio")){
		if(tempo_json["tempoPReservatorio"].Size()!=tempo_json["pressaoReservatorio"].Size()){
			logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chaves 'tempoPReservatorio', 'pressaoReservatorio' com dimensoes diferentes",
					chaveJson, "Condicoes de tempo");
		}
		pRes=new double [nPRes];
		tempoPRes=new double [nPRes];
		for(int i=0; i<nPRes; i++){
			pRes[i]=tempo_json["pressaoReservatorio"][i].GetDouble();
			tempoPRes[i]=tempo_json["tempoPReservatorio"][i].GetDouble();
		}
	}
	else{
		pRes=new double [nsatAPoc];
		tempoPRes=new double [nsatAPoc];
		for(int i=0; i<nPRes; i++){
			pRes[i]=pRes2;
			tempoPRes[i]=0.;
		}
	}


}

void PorosRadSimp::parse_tabela(Value& tabela_json) {
	/*
	 * "tabela": { "ativo": false, "nPontos": 21, "pressaoMaxima": 600, "pressaominima": 2, "temperaturaMaxima": 200, "temperaturaMinima": -100 }
	 */

		// de-para do elemento "tabela" obtido do json para o struct correspondente
		// de-para do elemento "tabela" obtido do json para o struct correspondente
		tabent.npont = tabela_json["nPontos"].GetInt();
		tabent.pmax = tabela_json["pressaoMaxima"].GetDouble();
		tabent.pmin = tabela_json["pressaoMinima"].GetDouble();
		tabent.tmax = tabela_json["temperaturaMaxima"].GetDouble();
		tabent.tmin = tabela_json["temperaturaMinima"].GetDouble();
		npontos = tabent.npont;
}

void PorosRadSimp::parse_corte(Value& corte_json) {
			// de-para do elementoPoisson do json para o struct
	corte.a = corte_json["diametroInterno"].GetDouble();
	corte.b = corte.a;
	corte.ncam = corte_json["camadas"].Size();
	corte.zDatum1 = corte_json["zInterno"].GetDouble();
	corte.zDatum0 = corte_json["zPoco"].GetDouble();
	corte.diam = new double[corte.ncam];
	corte.indmat = new int[corte.ncam];
	corte.discre = new int[corte.ncam];
			// percorrer a lista de camadas da seção transversal
	for (int j = 0; j < corte.ncam; j++) {
		double esp=corte_json["camadas"][j]["espessura"].GetDouble();
		if(j==0)corte.diam[j]=corte.a+2.*esp;
		else corte.diam[j]=corte.diam[j-1]+2.*esp;

		corte.discre[j] =
				corte_json["camadas"][j]["discretizacao"].GetInt();
		corte.indmat[j] =
				corte_json["camadas"][j]["idMaterial"].GetInt();
	}
}

void PorosRadSimp::parse_materiais(Value& materialPoroso_json) {
	// obter o tamanho do elementoPoisson do Json
		nmaterialPoroso = materialPoroso_json.Size();
	// caso o tamanho do elementoPoisson seja maior que zero
		if (nmaterialPoroso > 0) {
			// vetor de materiais
			mat = new materialPoroso[nmaterialPoroso];
			// loop para carga do vetor do JSON
			for (int i = 0; i < nmaterialPoroso; i++) {

				mat[i].kX =0;
				mat[i].kY =0;
				mat[i].poro =0;
				mat[i].compRoc =0;

				mat[i].kX = materialPoroso_json[i]["permeabilidadeY"].GetDouble()*(9.869233/(1e16));
				mat[i].kY = materialPoroso_json[i]["permeabilidadeY"].GetDouble()*(9.869233/(1e16));
				mat[i].poro = materialPoroso_json[i]["porosidade"].GetDouble();
				mat[i].compRoc = materialPoroso_json[i]["compressibilidadeRocha"].GetDouble();
			}
		}
}

void PorosRadSimp::parse_kRelOA(Value& kRelOA_json) {
	kRelOA.npont= kRelOA_json["satA"].Size();
	kRelOA.permRelOW=new double[kRelOA.npont];
	kRelOA.permRelW=new double[kRelOA.npont];
	kRelOA.satW=new double[kRelOA.npont];
	for(int i=0;i<kRelOA.npont;i++){
		kRelOA.satW[i]=kRelOA_json["satA"][i].GetDouble();
		kRelOA.permRelW[i]=kRelOA_json["kRelA"][i].GetDouble();
		kRelOA.permRelOW[i]=kRelOA_json["kRelOA"][i].GetDouble();
	}

	/*kRelOG.npont= 1;
		kRelOG.permRelG=new double[kRelOG.npont];
		kRelOG.permRelOG=new double[kRelOG.npont];
		kRelOG.satG=new double[kRelOG.npont];
		for(int i=0;i<kRelOG.npont;i++){
			kRelOG.satG[i]=0.;
			kRelOG.permRelG[i]=0.;
			kRelOG.permRelOG[i]=0.;
		}*/
}

void PorosRadSimp::parse_kRelOG(Value& kRelOG_json) {
	kRelOG.npont= kRelOG_json["satG"].Size();
	kRelOG.permRelG=new double[kRelOG.npont];
	kRelOG.permRelOG=new double[kRelOG.npont];
	kRelOG.satG=new double[kRelOG.npont];
	for(int i=0;i<kRelOG.npont;i++){
		kRelOG.satG[i]=kRelOG_json["satG"][i].GetDouble();
		kRelOG.permRelG[i]=kRelOG_json["kRelG"][i].GetDouble();
		kRelOG.permRelOG[i]=kRelOG_json["kRelOG"][i].GetDouble();
	}
	/*kRelOG.npont= 1;
		kRelOG.permRelG=new double[kRelOG.npont];
		kRelOG.permRelOG=new double[kRelOG.npont];
		kRelOG.satG=new double[kRelOG.npont];
		for(int i=0;i<kRelOG.npont;i++){
			kRelOG.satG[i]=0.;
			kRelOG.permRelG[i]=0.;
			kRelOG.permRelOG[i]=0.;
		}*/
}

void PorosRadSimp::parse_pcOA(Value& pcOA_json) {
	//psi
	pcOA.npont= pcOA_json["satA"].Size();
	pcOA.satW=new double[pcOA.npont];
	pcOA.presCapOW=new double[pcOA.npont];
	for(int i=0;i<pcOA.npont;i++){
		pcOA.satW[i]=pcOA_json["satA"][i].GetDouble();
		pcOA.presCapOW[i]=pcOA_json["pcOA"][i].GetDouble()/(14.69595*0.9678411);
	}

	pcGO.npont= 1;
	    pcGO.satG=new double[pcGO.npont];
		pcGO.presCapGO=new double[pcGO.npont];
		for(int i=0;i<pcGO.npont;i++){
			pcGO.satG[i]=0.;
			pcGO.presCapGO[i]=0.;
		}
}

void PorosRadSimp::geraTabFlash(int var) {

	string pvtfile;
	pvtfile = pvtsimarq;
	string dadosMR = pvtfile;
	ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
	string chave;
	char* tenta;
	//tenta = new char[400];
	double testatok;
	int ndiv = tabent.npont - 1;

	Vcr<double> presPVTSim(ndiv + 1);
	Vcr<double> tempPVTSim(ndiv + 1);

	FullMtx<double> VarTemp(ndiv + 2, ndiv + 2);

	char line[4000];
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	while (tenta != nullptr && strcmp(tenta, "PHASE") != 0) {
		tenta = strtok(NULL, " ,()=");
	}
	tenta = strtok(NULL, " ,()=");
	int lacoleitura;

	int threeOrtwo = 0;
	if (strcmp(tenta, "THREE") == 0) {
		threeOrtwo = 1;
		if (var == 1)
			lacoleitura = 1;  //rhog
		else if (var == 2)
			lacoleitura = 2;  //rhol
		else if (var == 3)
			lacoleitura = 4;  //drhogdp
		else if (var == 4)
			lacoleitura = 5;  //drholdp
		else if (var == 5)
			lacoleitura = 7;  //dhhogdt
		else if (var == 6)
			lacoleitura = 8;  //drholdt
		else if (var == 7)
			lacoleitura = 9;  //drhowdt
		else if (var == 8)
			lacoleitura = 10;  //RS
		else if (var == 9)
			lacoleitura = 15;  //Cpg
		else if (var == 10)
			lacoleitura = 16;  //Cpl
		else if (var == 11)
			lacoleitura = 17;  //Cpw
		else if (var == 12)
			lacoleitura = 18;  //Hg
		else if (var == 13)
			lacoleitura = 19;  //Hl
		else if (var == 14)
			lacoleitura = 24;  //Sig g-l
		else if (var == 15)
			lacoleitura = 25;  //Sig g-w
		else if (var == 16)
			lacoleitura = 13;  //visc oleo//alteracao6
		else if (var == 17)
			lacoleitura = 12;  //visc gas//alteracao6
	} else {
		if (var == 1)
			lacoleitura = 1;  //rhog
		else if (var == 2)
			lacoleitura = 2;  //rhol
		else if (var == 3)
			lacoleitura = 3;  //drhogdp
		else if (var == 4)
			lacoleitura = 4;  //drholdp
		else if (var == 5)
			lacoleitura = 5;  //dhhogdt
		else if (var == 6)
			lacoleitura = 6;  //drholdt
		else if (var == 8)
			lacoleitura = 7;  //RS
		else if (var == 9)
			lacoleitura = 10;  //Cpg
		else if (var == 10)
			lacoleitura = 11;  //Cpl
		else if (var == 12)
			lacoleitura = 12;  //Hg
		else if (var == 13)
			lacoleitura = 13;  //Hl
		else if (var == 14)
			lacoleitura = 16;  //Sig g-l
		else if (var == 16)
			lacoleitura = 9;  //visc oleo//alteracao6
		else if (var == 17)
			lacoleitura = 8;  //visc gas//alteracao6
	}

	while (chave != "PRESSURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	presPVTSim[0] = atof(tenta) * 1.01971621e-5;
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		presPVTSim[kontaPVT] = atof(tenta) * 1.01971621e-5;
	}
	while (chave != "TEMPERATURE")
		lendoPVTSim >> chave;
	lendoPVTSim.get(line, 4000);
	tenta = strtok(line, " ,()=");
	tempPVTSim[0] = atof(tenta);
	for (int kontaPVT = 1; kontaPVT <= ndiv; kontaPVT++) {
		tenta = strtok(NULL, " ,");
		tempPVTSim[kontaPVT] = atof(tenta);
	}
	if(var==1){
		tabent.pmin=presPVTSim[0];
		tabent.pmax=presPVTSim[ndiv];
		tabent.tmin=tempPVTSim[0];
		tabent.tmax=tempPVTSim[ndiv];
	}

	for (int i = 1; i <= ndiv + 1; i++) {
		VarTemp[i][0] = presPVTSim[i - 1];
		for (int j = 1; j <= ndiv + 1; j++) {
			VarTemp[0][j] = tempPVTSim[j - 1];
			//HLatTemp[i][j]=arq.flup.EntalpGasIsoT(pteste,tteste)-arq.flup.EntalpLiqIsoT(pteste,tteste);
			while (chave != "POINT")
				lendoPVTSim >> chave;
			lendoPVTSim.get(line, 4000);
			tenta = strtok(line, " ,()=");
			for (int kontaPVT = 0; kontaPVT < lacoleitura; kontaPVT++) {
				tenta = strtok(NULL, " ,");
				testatok = atof(tenta);
			}
			tenta = strtok(NULL, " ,");
			testatok = atof(tenta);
			VarTemp[i][j] = testatok;
			//tenta = strtok(NULL, " ,");
			//testatok = atof(tenta);
			//VarTemp[i][j] -= testatok;

			if (i == ndiv + 1 && j == ndiv + 1)
				break;
			while (chave != "PVTTABLE")
				lendoPVTSim >> chave;
		}

	}
	lendoPVTSim.close();
	for (int i = 0; i < ndiv + 2; i++) {
		for (int j = 0; j < ndiv + 2; j++) {
			if (threeOrtwo == 1) {
				if (var == 1)
					flash.rhogF[i][j] = VarTemp[i][j];  //rhog
				else if (var == 2)
					flash.rholF[i][j] = VarTemp[i][j];  //rhol
				else if (var == 3)
					flash.DrhogDpF[i][j] = VarTemp[i][j]/**98066.14*/; //drhogdp
				else if (var == 4)
					flash.DrholDpF[i][j] = VarTemp[i][j]/**98066.14*/; //drholdp
				else if (var == 5)
					flash.DrhogDtF[i][j] = VarTemp[i][j];  //dhhogdt
				else if (var == 6)
					flash.DrholDtF[i][j] = VarTemp[i][j];  //drholdt
				else if (var == 7)
					flash.DrhowDtF[i][j] = VarTemp[i][j];  //drhowdt
				else if (var == 8)
					flash.RSF[i][j] = VarTemp[i][j];  //RS
				else if (var == 9)
					flash.cpgF[i][j] = VarTemp[i][j];  //Cpg
				else if (var == 10)
					flash.cplF[i][j] = VarTemp[i][j];  //Cpl
				else if (var == 11)
					flash.cpwF[i][j] = VarTemp[i][j];  //Cpw
				else if (var == 12)
					flash.HgF[i][j] = VarTemp[i][j];  //Hg
				else if (var == 13)
					flash.HlF[i][j] = VarTemp[i][j];  //Hl
				else if (var == 14)
					flash.sigOGF[i][j] = VarTemp[i][j];  //Sig g-l
				else if (var == 15)
					flash.sigWGF[i][j] = VarTemp[i][j];  //Sig g-w
				else if (var == 16)
					flash.viscO[i][j] = VarTemp[i][j]; //visc oleo //alteracao6
				else if (var == 17)
					flash.viscG[i][j] = VarTemp[i][j]; //visc gas //alteracao6

				if (var == 8 && i > 0 && j > 0) {
					double rhostd = 141.5 * 1000.
							/ (131.5 + flup.API);
					double rgstd = 1.225 * flup.Deng;
					flash.RSF[i][j] = (flup.RGO
							- (rhostd + rgstd * flup.RGO)
									* flash.RSF[i][j] / rgstd) * 35.31467
							/ 6.29;
					if(flash.RSF[i][j]<0)flash.RSF[i][j]=0.;
					//falcatrua-teste de consistencia de RS do PVTSIM
					//if(i>2 && flash.RSF[i][j]<flash.RSF[i-1][j])flash.RSF[i][j]=flash.RSF[i-1][j];
					VarTemp[i][j] = flash.RSF[i][j] * 6.29 / 35.31467;
				}
				if ((var == 16 || var == 17) && i > 0 && j > 0) {  //alteracao6
					if (var == 16)
						flash.viscO[i][j] *= 1000.;
					else
						flash.viscG[i][j] *= 1000.;
					VarTemp[i][j] *= 1000.;
				}
			} else {
				if (var == 1)
					flash.rhogF[i][j] = VarTemp[i][j];  //rhog
				else if (var == 2)
					flash.rholF[i][j] = VarTemp[i][j];  //rhol
				else if (var == 3)
					flash.DrhogDpF[i][j] = VarTemp[i][j]/**98066.5*/; //drhogdp
				else if (var == 4)
					flash.DrholDpF[i][j] = VarTemp[i][j]/**98066.5*/; //drholdp
				else if (var == 5)
					flash.DrhogDtF[i][j] = VarTemp[i][j];  //dhhogdt
				else if (var == 6)
					flash.DrholDtF[i][j] = VarTemp[i][j];  //drholdt
				else if (var == 8)
					flash.RSF[i][j] = VarTemp[i][j];  //RS
				else if (var == 9)
					flash.cpgF[i][j] = VarTemp[i][j];  //Cpg
				else if (var == 10)
					flash.cplF[i][j] = VarTemp[i][j];  //Cpl
				else if (var == 12)
					flash.HgF[i][j] = VarTemp[i][j];  //Hg
				else if (var == 13)
					flash.HlF[i][j] = VarTemp[i][j];  //Hl
				else if (var == 14)
					flash.sigOGF[i][j] = VarTemp[i][j];  //Sig g-l
				else if (var == 16)
					flash.viscO[i][j] = VarTemp[i][j]; //visc oleo //alteracao6
				else if (var == 17)
					flash.viscG[i][j] = VarTemp[i][j]; //visc gas //alteracao6

				if (var == 8 && i > 0 && j > 0) {
					double rhostd = 141.5 * 1000.
							/ (131.5 + flup.API);
					double rgstd = 1.225 * flup.Deng;
					flash.RSF[i][j] = (flup.RGO
							- (rhostd + rgstd * flup.RGO)
									* flash.RSF[i][j] / rgstd) * 35.31467
							/ 6.29;
					if(flash.RSF[i][j]<0)flash.RSF[i][j]=0.;
					//falcatrua-teste de consistencia de RS do PVTSIM
					//if(i>2 && flash.RSF[i][j]<flash.RSF[i-1][j])flash.RSF[i][j]=flash.RSF[i-1][j];
					VarTemp[i][j] = flash.RSF[i][j] * 6.29 / 35.31467;
				}
				if ((var == 16 || var == 17) && i > 0 && j > 0) {  //alteracao6
					if (var == 16)
						flash.viscO[i][j] *= 1000.;
					else
						flash.viscG[i][j] *= 1000.;
					VarTemp[i][j] *= 1000.;
				}
			}
		}

	}
	/*double del2e;
	 double del1e;
	 double del1d;
	 double del2d;
	 for (int i = 3; i < ndiv-1; i++) {
	 for (int j = 3; j < ndiv-1; j++){

	 if(var==1){
	 del2e=flash.rhogF[i-1][j]-flash.rhogF[i-2][j];
	 del1e=flash.rhogF[i][j]-flash.rhogF[i-1][j];
	 del1d=flash.rhogF[i+1][j]-flash.rhogF[i][j];
	 del2d=flash.rhogF[i+2][j]-flash.rhogF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.rhogF[i][j]=flash.rhogF[i-1][j];
	 }
	 else if(var==2){
	 //flash.rholF[i][j]= VarTemp[i][j];
	 del2e=flash.rholF[i-1][j]-flash.rholF[i-2][j];
	 del1e=flash.rholF[i][j]-flash.rholF[i-1][j];
	 del1d=flash.rholF[i+1][j]-flash.rholF[i][j];
	 del2d=flash.rholF[i+2][j]-flash.rholF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.rholF[i][j]=flash.rholF[i-1][j];
	 }
	 else if(var==3){
	 //flash.DrhogDpF[i][j]= VarTemp[i][j];
	 del2e=flash.DrhogDpF[i-1][j]-flash.DrhogDpF[i-2][j];
	 del1e=flash.DrhogDpF[i][j]-flash.DrhogDpF[i-1][j];
	 del1d=flash.DrhogDpF[i+1][j]-flash.DrhogDpF[i][j];
	 del2d=flash.DrhogDpF[i+2][j]-flash.DrhogDpF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrhogDpF[i][j]=flash.DrhogDpF[i-1][j];
	 }
	 else if(var==4){
	 //flash.DrholDpF[i][j]= VarTemp[i][j];
	 del2e=flash.DrholDpF[i-1][j]-flash.DrholDpF[i-2][j];
	 del1e=flash.DrholDpF[i][j]-flash.DrholDpF[i-1][j];
	 del1d=flash.DrholDpF[i+1][j]-flash.DrholDpF[i][j];
	 del2d=flash.DrholDpF[i+2][j]-flash.DrholDpF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrholDpF[i][j]=flash.DrholDpF[i-1][j];
	 }
	 else if(var==5){
	 //flash.DrhogDtF[i][j]= VarTemp[i][j];
	 del2e=flash.DrhogDtF[i-1][j]-flash.DrhogDtF[i-2][j];
	 del1e=flash.DrhogDtF[i][j]-flash.DrhogDtF[i-1][j];
	 del1d=flash.DrhogDtF[i+1][j]-flash.DrhogDtF[i][j];
	 del2d=flash.DrhogDtF[i+2][j]-flash.DrhogDtF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrhogDtF[i][j]=flash.DrhogDtF[i-1][j];
	 }
	 else if(var==6){
	 //flash.DrholDtF[i][j]= VarTemp[i][j];
	 del2e=flash.rhogF[i-1][j]-flash.rhogF[i-2][j];
	 del1e=flash.rhogF[i][j]-flash.rhogF[i-1][j];
	 del1d=flash.rhogF[i+1][j]-flash.rhogF[i][j];
	 del2d=flash.rhogF[i+2][j]-flash.rhogF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.rhogF[i][j]=flash.rhogF[i-1][j];
	 }
	 else if(var==7){
	 //flash.DrhowDtF[i][j]= VarTemp[i][j];
	 del2e=flash.DrhowDtF[i-1][j]-flash.DrhowDtF[i-2][j];
	 del1e=flash.DrhowDtF[i][j]-flash.DrhowDtF[i-1][j];
	 del1d=flash.DrhowDtF[i+1][j]-flash.DrhowDtF[i][j];
	 del2d=flash.DrhowDtF[i+2][j]-flash.DrhowDtF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.DrhowDtF[i][j]=flash.DrhowDtF[i-1][j];
	 }
	 else if(var==8){
	 //flash.RSF[i][j]= VarTemp[i][j];
	 del2e=flash.RSF[i-1][j]-flash.RSF[i-2][j];
	 del1e=flash.RSF[i][j]-flash.RSF[i-1][j];
	 del1d=flash.RSF[i+1][j]-flash.RSF[i][j];
	 del2d=flash.RSF[i+2][j]-flash.RSF[i+1][j];
	 if(del2e*del2d*del1e*del1d<0)flash.RSF[i][j]=flash.RSF[i-1][j];
	 }


	 }
	 }*/

	string variavel;
	if (var == 1)
		variavel = "rhog";
	else if (var == 2)
		variavel = "rhol";
	else if (var == 3)
		variavel = "drhogdp";
	else if (var == 4)
		variavel = "drholdp";
	else if (var == 5)
		variavel = "dhhogdt";
	else if (var == 6)
		variavel = "drholdt";
	else if (var == 7)
		variavel = "drhowdt";
	else if (var == 8)
		variavel = "RS";
	else if (var == 9)
		variavel = "Cpg";
	else if (var == 10)
		variavel = "Cpl";
	else if (var == 11)
		variavel = "Cpw";
	else if (var == 12)
		variavel = "Hg";
	else if (var == 13)
		variavel = "Hl";
	else if (var == 14)
		variavel = "Sig-g-l";
	else if (var == 15)
		variavel = "Sig-g-w";
	else if (var == 16)
		variavel = "visc oleo";  //alteracao6
	else if (var == 17)
		variavel = "visc gas";  //alteracao6
	ostringstream saidaLatente;
	saidaLatente << pathPrefixoArqSaida << "perfiFlash-" << "-var-"
			<< variavel << ".dat";
	string tmp = saidaLatente.str();
	ofstream escreveMass(tmp.c_str(), ios_base::out);
	escreveMass << VarTemp;
	escreveMass.close();
	//delete [] tenta;
}


void PorosRadSimp::parse_fluido_producao(
		Value& fluidos_producao_json) {
	// criar variavel para o nome da propriedade json em processo de parse
	string chaveJson("#/fluidosProducao");

	double api = 20;
	double rgo = 0;
	double deng = 1;
	double bsw = 0;
	double denag = 1;
	double templ = 10;
	double lvisl = 2;
	double temph = 20;
	double lvish = 1;
	int tipoemul = 0;
	double bswCorte = 1;
	double yco2 = 0;
	int corrC = 0;
	int corrSat = 0;  //alteracao4
	double aemul = 0.;  //alteracao4
	double bemul = 0.;  //alteracao4
	int vcorrOM = 0;
	int vcorrOV = 0;
	int vcorrOS = 0;
	int corrDeng=0;
	int fracMolarUsuario=0;
	double PHI100=0.765;
	int nvecBSW=0;

			tabVisc.parserie=0;
			tabVisc.temp=0;
			tabVisc.visc=0;
			if (flashCompleto == 0) {
				// loop para carga do vetor do JSON
					// de-para do elemento "fluidosProducao" do json para o struct
					api = fluidos_producao_json["api"].GetDouble();
					rgo = fluidos_producao_json["rgo"].GetDouble();
					deng =
							fluidos_producao_json["densidadeGas"].GetDouble();
					/*bsw = fluidos_producao_json["bsw"].GetDouble();
					if(bsw>1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"BSW > 1", chaveJson,
								"BSW>1");
					}*/
					denag = 1;
					if (fluidos_producao_json.HasMember("densidadeAgua"))
						denag =
								fluidos_producao_json["densidadeAgua"].GetDouble();
					if(denag<1.0)denag=1.0;

					tipoemul = 0;
					if (fluidos_producao_json.HasMember("tipoEmul")) {
						tipoemul =
								fluidos_producao_json["tipoEmul"].GetDouble();
						// caso modelo exponencial
						if (tipoemul == 4) {
							if (!fluidos_producao_json.HasMember("coefAModeloExp")
									|| !fluidos_producao_json.HasMember("coefBModeloExp")) {
								// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
										chaveJson, "tipoEmul=4");
							} else {
								aemul =
										fluidos_producao_json["coefAModeloExp"].GetDouble();
								bemul =
										fluidos_producao_json["coefBModeloExp"].GetDouble();
							}
						}
						else if(tipoemul == 5 && fluidos_producao_json.HasMember("PHI100"))
							PHI100=fluidos_producao_json["PHI100"].GetDouble();
						else if(tipoemul == 6 && nvecBSW==0){
							nvecBSW=1;
							if(!fluidos_producao_json.HasMember("BSWVec") ||
							   !fluidos_producao_json.HasMember("emulVec")){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' requeridas",
										chaveJson, "tipoEmul=6");
							}
							else{
								nvecEmul =fluidos_producao_json["BSWVec"].Size();
								if(nvecEmul!=fluidos_producao_json["emulVec"].Size()){
									logger.log(LOGGER_FALHA,
									LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
											"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
											chaveJson, "tipoEmul=6");
								}
								else{
									BSWVec = new double[nvecEmul];
									emulVec = new double[nvecEmul];
									for (int i = 0; i < nvecEmul; i++) {
										BSWVec[i] = fluidos_producao_json["BSWVec"][i].GetDouble();
										emulVec[i] = fluidos_producao_json["emulVec"][i].GetDouble();
									}
								}
							}
						}
					}
					bswCorte = 1.;
					if (fluidos_producao_json.HasMember("bswCorte"))
						bswCorte =
								fluidos_producao_json["bswCorte"].GetDouble();
					yco2 =
							fluidos_producao_json["fracCO2"].GetDouble();
					corrC =
							fluidos_producao_json["correlacaoCritica"].GetInt();

					if (fluidos_producao_json.HasMember("modeloRsPb")) {
						corrSat =
								fluidos_producao_json["modeloRsPb"].GetInt(); //alteracao4
						// caso modelo RsPb da Livia
						/*if (corrSat == 4) {
							// TODO: CORRIGIR ESSE TRECHO DE CÓDIGO NO FUTURO
							corrSat = -4;
							if (pvtsimarq.length() == 0) {
								// RN-042: Chave "pvtsimArq" requerida em caso de "latente" ou "modeloCp" ou "modeloJTL"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"arquivo pvtsim requerido", chaveJson,
										"modeloRsPb=4");
								// Caso o arquivo pvtsim inexista
							} else if (!ifstream(pvtsimarq)) {
								// criar variavel para o nome da propriedade json em processo de parse
								string chavePvtsimArq(chaveJson + "/pvtsimArq");
								// RN-043: Arquivo "pvtsimArq" inexistente
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Arquivo inexistente", chavePvtsimArq,
										pvtsimarq);
							}
						}*/
					}

					vcorrOM = 0;
					// caso modelo oleo morto preenchido
					if (fluidos_producao_json.HasMember("modeloOleoMorto")) {
						//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
						vcorrOM =
								fluidos_producao_json["modeloOleoMorto"].GetInt();
					}
					if (vcorrOM == 0) {
						if (!fluidos_producao_json.HasMember("temp1")
								|| !fluidos_producao_json.HasMember("visc1")
								|| !fluidos_producao_json.HasMember("temp2")
								|| !fluidos_producao_json.HasMember("visc2")) {
							// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
									chaveJson,
									"modeloOleoMorto = 0 ou ausente");
						} else {
							templ =
									fluidos_producao_json["temp1"].GetDouble();
							lvisl =
									fluidos_producao_json["visc1"].GetDouble();
							temph =
									fluidos_producao_json["temp2"].GetDouble();
							lvish =
									fluidos_producao_json["visc2"].GetDouble();
						}
					}
					else if (vcorrOM == 7) {
						if (!fluidos_producao_json.HasMember("tempOleoMorto")
								|| !fluidos_producao_json.HasMember("viscOleoMorto")) {
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'tempOleoMorto', 'viscOleoMorto' requeridas",
									chaveJson,
									"modeloOleoMorto = 7 ou ausente");
						} else {
							tabVisc.parserie=fluidos_producao_json["tempOleoMorto"].Size();
							tabVisc.temp=new double[tabVisc.parserie];
							tabVisc.visc=new double[tabVisc.parserie];
							for(int itab=0;itab<tabVisc.parserie;itab++){
								tabVisc.temp[itab]=fluidos_producao_json["tempOleoMorto"][itab].GetDouble();
								tabVisc.visc[itab]=fluidos_producao_json["viscOleoMorto"][itab].GetDouble();
							}
						}
					}

					// caso modelo oleo vivo preenchido
					if (fluidos_producao_json.HasMember("modeloOleoVivo"))
						//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
						vcorrOV =
								fluidos_producao_json["modeloOleoVivo"].GetInt();

					// caso modelo oleo subsaturado preenchido
					if (fluidos_producao_json.HasMember("modeloOleoSubSaturado"))
						//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
						vcorrOS =
								fluidos_producao_json["modeloOleoSubSaturado"].GetInt();

					if (fluidos_producao_json.HasMember("correcaoDenGas"))
						corrDeng =
								fluidos_producao_json["correcaoDenGas"].GetInt();

					flup = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul, PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, 0); //alteracao4
					flup.zdranP = zdranP;  //alteracao2
					flup.dzdpP = dzdpP;  //alteracao2
					flup.dzdtP = dzdtP;  //alteracao2
					flup.npontos = npontos;
					flup.viscBlackOil = 1;
					flup.modelaAgua = 1;
					flup.corrDeng=corrDeng;
					flup.nvecEmul=nvecEmul;
					flup.BSWVec=BSWVec;
					flup.emulVec=emulVec;
					flup.dStockTankVaporMassFraction=flup.FracMass(1.01, 20.);
					flup.parserie=tabVisc.parserie;
					flup.viscTab=tabVisc.visc;
					flup.tempTab=tabVisc.temp;
			} else if(flashCompleto==1){
				///////////////////////////////////////////////////////////

				if (fluidos_producao_json.HasMember("pvtsimArq"))
					pvtsimarq = pathArqExtEntrada
							+ string(fluidos_producao_json["pvtsimArq"].GetString());

				string dadosMRPre;
				dadosMRPre = pvtsimarq;
				ifstream lendoPVTSimPre(dadosMRPre.c_str(), ios_base::in);
				string chave;
				char* tenta;
				//tenta = new char[400];
				double testatok;
				char line[4000];
				lendoPVTSimPre.get(line, 4000);
				tenta = strtok(line, " ,()=");
				lendoPVTSimPre >> chave;
				while (chave != "BUBBLEPRESSURES") {
					lendoPVTSimPre >> chave;
				}

				lendoPVTSimPre.get(line, 4000);
				int ntab=0;
				tenta = strtok(line, " ,()=");
				while(tenta != nullptr && std::string(tenta)!="Pa"){
					ntab++;
					tenta = strtok(NULL, " ,)/");
				}
				lendoPVTSimPre.close();
				tabent.npont=ntab;
				//////////////////////////////////////////////////
					flash.arquivo = pvtsimarq;

					/*bsw = fluidos_producao_json["bsw"].GetDouble();
					if(bsw>1){
						logger.log(LOGGER_FALHA,
						LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
								"BSW > 1", chaveJson,
								"BSW>1");
					}*/
					denag = 1;
					if (fluidos_producao_json.HasMember("densidadeAgua"))
						denag =
								fluidos_producao_json["densidadeAgua"].GetDouble();
					if(denag<1.0)denag=1.0;

					int modelagua=1;
					if (fluidos_producao_json.HasMember("modeloAguaBlackOil"))  //alteracao6
						modelagua =
								fluidos_producao_json["modeloAguaBlackOil"].GetInt();

					flash.visc = 0;  // modelo viscosidade tabela flash
					if (fluidos_producao_json.HasMember("modeloViscBlackOil"))  //alteracao6
						flash.visc =
								fluidos_producao_json["modeloViscBlackOil"].GetInt(); //alteracao6
					if (flash.visc == 1) {  //modelo viscosidade black oil
						vcorrOM = 0;
						// caso modelo oleo morto preenchido
						if (fluidos_producao_json.HasMember("modeloOleoMorto")) {
							//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
							vcorrOM =
									fluidos_producao_json["modeloOleoMorto"].GetInt();
						}
						if (vcorrOM == 0) {
							if (!fluidos_producao_json.HasMember("temp1")
									|| !fluidos_producao_json.HasMember("visc1")
									|| !fluidos_producao_json.HasMember("temp2")
									|| !fluidos_producao_json.HasMember("visc2")) {
								// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
										chaveJson,
										"modeloOleoMorto = 0 ou ausente");
							} else {
								templ =
										fluidos_producao_json["temp1"].GetDouble();
								lvisl =
										fluidos_producao_json["visc1"].GetDouble();
								temph =
										fluidos_producao_json["temp2"].GetDouble();
								lvish =
										fluidos_producao_json["visc2"].GetDouble();
							}
						}

						if (fluidos_producao_json.HasMember("modeloOleoVivo"))
							//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
							vcorrOV =
									fluidos_producao_json["modeloOleoVivo"].GetInt();
						if (fluidos_producao_json.HasMember("modeloOleoSubSaturado"))
							//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
							vcorrOS =
									fluidos_producao_json["modeloOleoSubSaturado"].GetInt();
					} else {
						vcorrOM = 4;
						vcorrOV = 0;
						vcorrOS = 0;
					}

					tipoemul = 0;
					if (fluidos_producao_json.HasMember("tipoEmul"))
						tipoemul =
								fluidos_producao_json["tipoEmul"].GetInt();
					bswCorte = 1.;
					if (fluidos_producao_json.HasMember("bswCorte"))
						bswCorte =
								fluidos_producao_json["bswCorte"].GetDouble();

					if (tipoemul == 4) {
						if (!fluidos_producao_json.HasMember("coefAModeloExp")
								|| !fluidos_producao_json.HasMember("coefBModeloExp")) {
							// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
									chaveJson, "tipoEmul=4");
						} else {
							aemul =
									fluidos_producao_json["coefAModeloExp"].GetDouble();
							bemul =
									fluidos_producao_json["coefBModeloExp"].GetDouble();
						}
					}
					else if(tipoemul == 5 && fluidos_producao_json.HasMember("PHI100"))
						PHI100=fluidos_producao_json["PHI100"].GetDouble();
					else if(tipoemul == 6 && nvecBSW==0){
						nvecBSW=1;
						if(!fluidos_producao_json.HasMember("BSWVec") ||
						   !fluidos_producao_json.HasMember("emulVec")){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'BSWVec', 'emulVec' requeridas",
									chaveJson, "tipoEmul=6");
						}
						else{
							nvecEmul =fluidos_producao_json["BSWVec"].Size();
							if(nvecEmul!=fluidos_producao_json["emulVec"].Size()){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
										chaveJson, "tipoEmul=6");
							}
							else{
								BSWVec = new double[nvecEmul];
								emulVec = new double[nvecEmul];
								for (int i = 0; i < nvecEmul; i++) {
									BSWVec[i] = fluidos_producao_json["BSWVec"][i].GetDouble();
									emulVec[i] = fluidos_producao_json["emulVec"][i].GetDouble();
								}
							}
						}
					}

					flash.id =0;
					flash.rholF = new double*[tabent.npont + 1];
					flash.rhogF = new double*[tabent.npont + 1];
					flash.DrholDpF = new double*[tabent.npont + 1];
					flash.DrhogDpF = new double*[tabent.npont + 1];
					flash.DrholDtF = new double*[tabent.npont + 1];
					flash.DrhogDtF = new double*[tabent.npont + 1];
					flash.DrhowDtF = new double*[tabent.npont + 1];
					flash.RSF = new double*[tabent.npont + 1];
					flash.cplF = new double*[tabent.npont + 1];
					flash.cpgF = new double*[tabent.npont + 1];
					flash.cpwF = new double*[tabent.npont + 1];
					flash.HlF = new double*[tabent.npont + 1];
					flash.HgF = new double*[tabent.npont + 1];
					flash.sigOGF = new double*[tabent.npont + 1];
					flash.sigWGF = new double*[tabent.npont + 1];
					flash.viscO = new double*[tabent.npont + 1]; //alteracao6
					flash.viscG = new double*[tabent.npont + 1]; //alteracao6
					flash.PBF = new double[tabent.npont];
					flash.TBF = new double[tabent.npont];

					for (int k = 0; k < tabent.npont + 1; k++) {
						flash.rholF[k] = new double[tabent.npont + 1];
						flash.rhogF[k] = new double[tabent.npont + 1];
						flash.DrholDpF[k] =
								new double[tabent.npont + 1];
						flash.DrhogDpF[k] =
								new double[tabent.npont + 1];
						flash.DrholDtF[k] =
								new double[tabent.npont + 1];
						flash.DrhogDtF[k] =
								new double[tabent.npont + 1];
						flash.DrhowDtF[k] =
								new double[tabent.npont + 1];
						flash.RSF[k] = new double[tabent.npont + 1];
						flash.cplF[k] = new double[tabent.npont + 1];
						flash.cpgF[k] = new double[tabent.npont + 1];
						flash.cpwF[k] = new double[tabent.npont + 1];
						flash.HlF[k] = new double[tabent.npont + 1];
						flash.HgF[k] = new double[tabent.npont + 1];
						flash.sigOGF[k] = new double[tabent.npont + 1];
						flash.sigWGF[k] = new double[tabent.npont + 1];
						flash.viscO[k] = new double[tabent.npont + 1]; //alteracao6
						flash.viscG[k] = new double[tabent.npont + 1]; //alteracao6
					}

					for (int k = 0; k < tabent.npont + 1; k++) {
						for (int j = 0; j < tabent.npont + 1; j++) {
							flash.rholF[k][j] = 0;
							flash.rhogF[k][j] = 0;
							flash.DrholDpF[k][j] = 0;
							flash.DrhogDpF[k][j] = 0;
							flash.DrholDtF[k][j] = 0;
							flash.DrhogDtF[k][j] = 0;
							flash.DrhowDtF[k][j] = 0;
							flash.RSF[k][j] = 0;
							flash.cplF[k][j] = 0;
							flash.cpgF[k][j] = 0;
							flash.cpwF[k][j] = 0;
							flash.HlF[k][j] = 0;
							flash.HgF[k][j] = 0;
							flash.sigOGF[k][j] = 0;
							flash.sigWGF[k][j] = 0;
							flash.viscO[k][j] = 0;  //alteracao6
							flash.viscG[k][j] = 0;  //alteracao6
						}
					}
					for (int k = 0; k < tabent.npont; k++) {
						flash.PBF[k] = 0;
						flash.TBF[k] = 0;
					}

					string dadosMR;
					dadosMR= pvtsimarq;
					ifstream lendoPVTSim(dadosMR.c_str(), ios_base::in);
					//string chave;
					//char* tenta;
					//tenta = new char[400];
					//double testatok;
					//char line[4000];
					lendoPVTSim.get(line, 4000);
					tenta = strtok(line, " ,()=");
					while (strcmp(tenta, "PHASE") != 0) {
						tenta = strtok(NULL, " ,()=");
					}
					tenta = strtok(NULL, " ,()=");
					int threeOrtwo = 0;
					flash.TwoOrThree = 0;
					if (strcmp(tenta, "THREE") == 0) {
						threeOrtwo = 1;
						flash.TwoOrThree = 1;
					}

					lendoPVTSim >> chave;
					while (chave != "GOR")
						lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					rgo = stof(chave);

					/*if (threeOrtwo == 1) {
						while (chave != "WC")
							lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						bsw = stof(chave);
					} else
						bsw = 0.;*/

					while (chave != "STDGASDENSITY")
						lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					deng = stof(chave);
					deng /= 1.225;

					while (chave != "STDOILDENSITY")
						lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					lendoPVTSim >> chave;
					api = stof(chave);
					api /= 1000.;
					api = 141.5 / api - 131.5;

					/*if (threeOrtwo == 1) {
						while (chave != "STDWATDENSITY")
							lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						lendoPVTSim >> chave;
						denag = stof(chave);
						denag /= 1000.;
						if (denag == 0)
							denag = 1.;
					} else
						denag = 1.;*/

					//double rgoSpare = fluidos_producao_json[indAtivo].rgo();
					//if(rgoSpare>rgo) rgo=rgoSpare;

					flup = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul,PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, 0);
					flup.indiceFlash = 0;
					//flup.viscBlackOil = 1 - flash.visc;
					flup.viscBlackOil = flash.visc;
					flup.modelaAgua=modelagua;

					int ndiv = tabent.npont - 1;
					flup.npontos=tabent.npont;
					lendoPVTSim >> chave;
					int lacoleitura = ndiv;
					double valor;

					while (chave != "BUBBLEPRESSURES") {
						lendoPVTSim >> chave;
					}
					lendoPVTSim.get(line, 4000);
					tenta = strtok(line, " ,()=");
					flash.PBF[0] = atof(tenta) * 0.00014503773800722;
					for (int kontaPVT = 1; kontaPVT <= lacoleitura;
							kontaPVT++) {
						tenta = strtok(NULL, " ,");
						testatok = atof(tenta);
						flash.PBF[kontaPVT] = testatok * 0.00014503773800722;
					}
					while (chave != "BUBBLETEMPERATURES") {
						lendoPVTSim >> chave;
					}
					lendoPVTSim.get(line, 4000);
					tenta = strtok(line, " ,()=");
					flash.TBF[0] = atof(tenta);
					for (int kontaPVT = 1; kontaPVT <= lacoleitura;
							kontaPVT++) {
						tenta = strtok(NULL, " ,");
						testatok = atof(tenta);
						flash.TBF[kontaPVT] = testatok;
					}

					FullMtx<double> BolhaTemp(ndiv + 2, 2);
					for (int k = 0; k <= ndiv; k++) {
						BolhaTemp[k][0] = flash.PBF[k];
						BolhaTemp[k][1] = flash.TBF[k];
					}
					ostringstream saidaBolha;
					saidaBolha << pathPrefixoArqSaida << "perfilBolha.dat";
					string tmp = saidaBolha.str();
					ofstream escreveMass(tmp.c_str(), ios_base::out);
					escreveMass << BolhaTemp;
					escreveMass.close();
					lendoPVTSim.close();

					geraTabFlash(1);
					geraTabFlash(2);
					geraTabFlash(3);
					geraTabFlash(4);
					geraTabFlash(5);
					geraTabFlash(6);
					geraTabFlash(8);
					geraTabFlash(9);
					geraTabFlash(10);
					geraTabFlash(12);
					geraTabFlash(13);
					geraTabFlash(14);
					geraTabFlash(16);  //alteracao6
					geraTabFlash(17);  //alteracao6
					if (threeOrtwo == 1) {
						geraTabFlash(7);
						geraTabFlash(11);
						geraTabFlash(15);
					}
					flup.rhogF = flash.rhogF;

					int erroP=0;
					int erroT=0;
					vector<double> dP;
					vector<double> pchange;
					vector<int> indp;
					int ndp=0;
					vector<double> dT;
					vector<double> tchange;
					vector<int> indt;
					int ndt=0;
					double dp0=0.;
					double dp1;
					double dt0=0.;
					double dt1;

					int imin=1;
					while(flup.rhogF[imin][0]<=1.0332274497825)imin++;
					flup.indMinEquP=imin+1;
					int iant=imin+1;
					flup.pMinEqu=flup.rhogF[imin+1][0];
					flup.delpTab=flup.rhogF[imin+2][0]-flup.rhogF[imin+1][0];
					dp0=(flup.rhogF[ndiv+1][0]-flup.rhogF[imin+1][0])/(ndiv-imin);
					for(int j=imin+1;j<ndiv;j++){
						dp0=flup.rhogF[j+1][0]-flup.rhogF[j][0];
						dp1=flup.rhogF[j+2][0]-flup.rhogF[j+1][0];
						if(fabsl((dp0-dp1)/dp0)>1e-3){
							ndp++;
							pchange.push_back(flup.rhogF[j+1][0]);
							indp.push_back(j+1);
							dP.push_back((flup.rhogF[j+1][0]-flup.rhogF[iant][0])/(j+1-iant));
							iant=j+1;
							if(ndp>3)erroP=1;
						}
						else if(j==ndiv-1 && ndp>0){
							ndp++;
							pchange.push_back(flup.rhogF[ndiv+1][0]);
							indp.push_back(ndiv+1);
							dP.push_back((flup.rhogF[ndiv+1][0]-flup.rhogF[iant][0])/(ndiv+1-iant));
							if(ndp>3)erroP=1;

						}
					}
                    if(ndp==0){
                    	ndp++;
                    	pchange.push_back(flup.rhogF[ndiv+1][0]);
                    	indp.push_back(ndiv+1);
                    	dP.push_back((flup.rhogF[ndiv+1][0]-flup.rhogF[imin+1][0])/(ndiv-imin));
                    }
                    if(erroP==0){
                      flup.pchange=pchange;
                      flup.dpchange=dP;
                      flup.ipchange=indp;
                    }
					flup.delpTab=ndp;
					flup.erroP=erroP;

					imin=1;
					while(flup.rhogF[0][imin]<=16)imin++;
					flup.indMinEquT=imin+1;
					iant=imin+1;
					flup.tMinEqu=flup.rhogF[0][imin+1];
					flup.deltTab=flup.rhogF[0][imin+2]-flup.rhogF[0][imin+1];
					dt0=(flup.rhogF[0][ndiv+1]-flup.rhogF[0][imin+1])/(ndiv-imin);
					for(int j=imin+1;j<ndiv;j++){
						dt0=flup.rhogF[0][j+1]-flup.rhogF[0][j];
						dt1=flup.rhogF[0][j+2]-flup.rhogF[0][j+1];
						if(fabsl((dt0-dt1)/dt0)>1e-3){
							ndt++;
							tchange.push_back(flup.rhogF[0][j+1]);
							indt.push_back(j+1);
							dT.push_back((flup.rhogF[0][j+1]-flup.rhogF[0][iant])/(j+1-iant));
							iant=j+1;
							if(ndt>3)erroT=1;
						}
						else if(j==ndiv-1 && ndt>0){
							ndt++;
							tchange.push_back(flup.rhogF[0][ndiv+1]);
							indt.push_back(ndiv+1);
							dT.push_back((flup.rhogF[0][ndiv+1]-flup.rhogF[0][iant])/(ndiv+1-iant));
							if(ndt>3)erroT=1;

						}
					}
                    if(ndt==0){
                    	ndt++;
                    	tchange.push_back(flup.rhogF[0][ndiv+1]);
                    	indt.push_back(ndiv+1);
                    	dT.push_back((flup.rhogF[0][ndiv+1]-flup.rhogF[0][imin+1])/(ndiv-imin));
                    }
                    if(erroT==0){
                      flup.tchange=tchange;
                      flup.dtchange=dT;
                      flup.itchange=indt;
                    }
					flup.deltTab=ndt;
					flup.erroT=erroT;

					flup.rholF = flash.rholF;
					flup.DrhogDpF = flash.DrhogDpF;
					flup.DrholDpF = flash.DrholDpF;
					flup.DrhogDtF = flash.DrhogDtF;
					flup.DrholDtF = flash.DrholDtF;
					flup.DrhowDtF = flash.DrhowDtF;
                    flup.RSF = flash.RSF;
					flup.cpgF = flash.cpgF;
					flup.cplF = flash.cplF;
					flup.cpwF = flash.cpwF;
					flup.HgF = flash.HgF;
					flup.HlF = flash.HlF;
					flup.sigOGF = flash.sigOGF;
					flup.sigWGF = flash.sigWGF;
					flup.viscO = flash.viscO;
					flup.viscG = flash.viscG;
					flup.PBPVTSim = flash.PBF;
					flup.TBPVTSim = flash.TBF;

					/*for(int j=1;j>ndiv;j++){
						for(int k=2;k<ndiv+2;k++){
							if(flup.RSF[j][k]<0.){
								//flup.rholF[j][k]=1000*141.5/(131.5+api);
								flup.rholF[j][k]=flup.rholF[j][k-1];
								flup.DrholDtF[j][k]=0.;
								flup.DrholDpF[j][k]=0.;
							}
						}
					}*/

					flup.nvecEmul=nvecEmul;
					flup.BSWVec=BSWVec;
					flup.emulVec=emulVec;
					flup.parserie=tabVisc.parserie;
					flup.viscTab=tabVisc.visc;
					flup.tempTab=tabVisc.temp;

					rgo = fluidos_producao_json["rgo"].GetDouble();
					if(rgo>flup.RGO){
						flup.RGO=rgo;
						flup.IRGO=flup.RGO*35.31467/6.29;
					}

					if(flash.rhogF[0][1]<20 && flash.rhogF[1][0]<1.01)
						//flup.dStockTankVaporDensity=flup.FracMass(1.01, 20.);
						flup.dStockTankVaporMassFraction=flup.FracMass(1.01, 20.);
					else
						//flup.dStockTankVaporDensity=0.5;
						flup.dStockTankVaporMassFraction=0.5;
					//delete [] tenta;
			}
			else{

				/*api = fluidos_producao_json[indAtivo].api();
				rgo = fluidos_producao_json[indAtivo].rgo();
				deng =
						fluidos_producao_json[indAtivo].densidadeGas();
				bsw = fluidos_producao_json[indAtivo].bsw();
				if(bsw>1){
					logger.log(LOGGER_FALHA,
					LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
							"BSW > 1", chaveJson,
							"BSW>1");
				}
				denag = 1;
				if (fluidos_producao_json[indAtivo].densidadeAgua().exists())
					denag =
							fluidos_producao_json[indAtivo].densidadeAgua();
				if(denag<1.0)denag=1.0;*/

				string dadosMRPre;
				dadosMRPre = pvtsimarq;
				int npseudo=0;
				int erro=0;
                GetMixtureComponentNumberFromExternalFile(dadosMRPre.c_str(),dadosMRPre.size(),0,
                		    &npseudo,&erro);
                if(erro!=0)logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
						"Leitura CTM", chaveJson, "Erro na leitura do CTM");
                double fractemp[npseudo];

                	compDet.fracMol = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.fracMol[inpseudo]=0.;
                	compDet.TIndepPeneloux = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.TIndepPeneloux[inpseudo]=0.;
                	compDet.fatAcent = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.fatAcent[inpseudo]=0.;
                	compDet.masMol = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.masMol[inpseudo]=0.;
                	compDet.parac = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.parac[inpseudo]=0.;
                	compDet.presCrit = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.presCrit[inpseudo]=0.;
                	compDet.tempCrit = new double [npseudo];
                	for(int inpseudo=0;inpseudo<npseudo;inpseudo++)compDet.tempCrit[inpseudo]=0.;
                	compDet.CpIGCoefs = new double [4*npseudo];
                	for(int inpseudo=0;inpseudo<4*npseudo;inpseudo++)compDet.CpIGCoefs[inpseudo]=0.;
                	compDet.lij = new double [npseudo*npseudo];
                	for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet.lij[inpseudo]=0.;
                	compDet.kij = new double [npseudo*npseudo];
                	for(int inpseudo=0;inpseudo<npseudo*npseudo;inpseudo++)compDet.kij[inpseudo]=0.;

                	/*compDet.fracMol = new double [npseudo];
                	compDet.TIndepPeneloux = new double [npseudo];
                	compDet.fatAcent = new double [npseudo];
                	compDet.masMol = new double [npseudo];
                	compDet.parac = new double [npseudo];
                	compDet.presCrit = new double [npseudo];
                	compDet.tempCrit = new double [npseudo];
                	compDet.CpIGCoefs = new double [4*npseudo];
                	compDet.lij = new double [npseudo*npseudo];
                	compDet.kij = new double [npseudo*npseudo];*/

                	InputForViscosity modeloVisc;

                		ReadMixtureComponentsFromExternalFile(dadosMRPre.c_str(), dadosMRPre.size(), 0,
                				&npseudo, &fractemp[0], compDet.masMol,
								compDet.tempCrit, compDet.presCrit, compDet.fatAcent,
								compDet.parac,compDet.kij,compDet.lij,compDet.TIndepPeneloux,
								&compDet.liqModel, compDet.CpIGCoefs,&modeloVisc, &erro);
            		/*ReadMixtureComponentsFromExternalFile(dadosMRPre.c_str(), dadosMRPre.size(), 0,
            				&npseudo, &fractemp[0], compDet.masMol,
							compDet.tempCrit, compDet.presCrit, compDet.fatAcent,
							compDet.parac,compDet.kij,compDet.lij,compDet.TIndepPeneloux,
							&compDet.liqModel, compDet.CpIGCoefs,&modeloVisc, &erro);*/
              		int fracusu=0;
    				if (fluidos_producao_json.HasMember("fracMolarUsuario"))
    						fracusu =fluidos_producao_json["fracMolarUsuario"].GetDouble();
    				if(fracusu==1){
    					int nusu=fluidos_producao_json["fracMolar"].Size();
    					if(nusu!=npseudo)logger.log(LOGGER_FALHA, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    								"Dimensao da composicao diferente da dimensao da composicao dada no CTM",
									chaveJson, "Erro no preenchimento da Fracao Molar");
    					for(int j=0;j<npseudo;j++)
    							compDet.fracMol[j]=fluidos_producao_json["fracMolar"][j].GetDouble()/100.;
    					/*for(int j=0;j<npseudo;j++)
    					    	compDet.fracMol[j]=fluidos_producao_json[indAtivo].fracMolar()[j];*/

    			    }
    				else /*memcpy(compDet.fracMol,&fractemp[0],npseudo*sizeof(double));*/
    					memcpy(compDet.fracMol,&fractemp[0],npseudo*sizeof(double));
    				double totFrac=0.;
    				for(int j=0;j<npseudo;j++)totFrac+=compDet.fracMol[j];
    				if(totFrac>1e-15)for(int j=0;j<npseudo;j++)compDet.fracMol[j]/=totFrac;
    				/*bsw = fluidos_producao_json["bsw"].GetDouble();
    				if(bsw>1){
    							logger.log(LOGGER_FALHA,
    							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    							"BSW > 1", chaveJson,
    							"BSW>1");
    					   	}*/
    				denag = 1;
    				if (fluidos_producao_json.HasMember("densidadeAgua"))
    							denag =	fluidos_producao_json["densidadeAgua"].GetDouble();
    				if(denag<1.0)denag=1.0;

					yco2 =
							fluidos_producao_json["fracCO2"].GetDouble();
					corrC =
							fluidos_producao_json["correlacaoCritica"].GetInt();


					int modelovisc = 1;  // modelo viscosidade tabela flash
					if (fluidos_producao_json.HasMember("modeloViscBlackOil"))  //alteracao6
						modelovisc =
								fluidos_producao_json["modeloViscBlackOil"].GetInt(); //alteracao6
					if (modelovisc == 1) {  //modelo viscosidade black oil
						vcorrOM = 0;
						// caso modelo oleo morto preenchido
						if (fluidos_producao_json.HasMember("modeloOleoMorto")) {
							//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
							vcorrOM =
									fluidos_producao_json["modeloOleoMorto"].GetInt();
						}
						if (vcorrOM == 0) {
							if (!fluidos_producao_json.HasMember("temp1")
									|| !fluidos_producao_json.HasMember("visc1")
									|| !fluidos_producao_json.HasMember("temp2")
									|| !fluidos_producao_json.HasMember("visc2")) {
								// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
								// incluir falha
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
										chaveJson,
										"modeloOleoMorto = 0 ou ausente");
							} else {
								templ =
										fluidos_producao_json["temp1"].GetDouble();
								lvisl =
										fluidos_producao_json["visc1"].GetDouble();
								temph =
										fluidos_producao_json["temp2"].GetDouble();
								lvish =
										fluidos_producao_json["visc2"].GetDouble();
							}
						}

						if (fluidos_producao_json.HasMember("modeloOleoVivo"))
							//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
							vcorrOV =
									fluidos_producao_json["modeloOleoVivo"].GetInt();
						if (fluidos_producao_json.HasMember("modeloOleoSubSaturado"))
							//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
							vcorrOS =
									fluidos_producao_json["modeloOleoSubSaturado"].GetInt();
					} else {
						vcorrOM = 4;
						vcorrOV = 0;
						vcorrOS = 0;
					}

					tipoemul = 0;
					if (fluidos_producao_json.HasMember("tipoEmul"))
						tipoemul =
								fluidos_producao_json["tipoEmul"].GetInt();
					bswCorte = 1.;
					if (fluidos_producao_json.HasMember("bswCorte"))
						bswCorte =
								fluidos_producao_json["bswCorte"].GetDouble();

					if (tipoemul == 4) {
						if (!fluidos_producao_json.HasMember("coefAModeloExp")
								|| !fluidos_producao_json.HasMember("coefBModeloExp")) {
							// RN-041: Chaves "coefAModeloExp", "coefBModeloExp" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'coefAModeloExp', 'coefBModeloExp' requeridas",
									chaveJson, "tipoEmul=4");
						} else {
							aemul =
									fluidos_producao_json["coefAModeloExp"].GetDouble();
							bemul =
									fluidos_producao_json["coefBModeloExp"].GetDouble();
						}
					}
					else if(tipoemul == 5 && fluidos_producao_json.HasMember("PHI100"))
						PHI100=fluidos_producao_json["PHI100"].GetDouble();
					else if(tipoemul == 6 && nvecBSW==0){
						nvecBSW=1;
						if(!fluidos_producao_json.HasMember("BSWVec") ||
						   !fluidos_producao_json.HasMember("emulVec")){
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'BSWVec', 'emulVec' requeridas",
									chaveJson, "tipoEmul=6");
						}
						else{
							nvecEmul =fluidos_producao_json["BSWVec"].Size();
							if(nvecEmul!=fluidos_producao_json["emulVec"].Size()){
								logger.log(LOGGER_FALHA,
								LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
										"Chaves 'BSWVec', 'emulVec' com dimensoes diferentes",
										chaveJson, "tipoEmul=6");
							}
							else{
								BSWVec = new double[nvecEmul];
								emulVec = new double[nvecEmul];
								for (int i = 0; i < nvecEmul; i++) {
									BSWVec[i] = fluidos_producao_json["BSWVec"][i].GetDouble();
									emulVec[i] = fluidos_producao_json["emulVec"][i].GetDouble();
								}
							}
						}
					}


					/*vcorrOM = 0;
					// caso modelo oleo morto preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoMorto().exists()) {
						//0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 4->Petrosky_Farshad 6->Beal
						vcorrOM =
								fluidos_producao_json[indAtivo].modeloOleoMorto();
					}
					if (vcorrOM == 0) {
						if (!fluidos_producao_json[indAtivo].temp1().exists()
								|| !fluidos_producao_json[indAtivo].visc1().exists()
								|| !fluidos_producao_json[indAtivo].temp2().exists()
								|| !fluidos_producao_json[indAtivo].visc2().exists()) {
							// RN-031: Chaves "temp1", "visc1", "temp2", "visc2" requeridas em caso de "modeloOleoMorto=0"
							// incluir falha
							logger.log(LOGGER_FALHA,
							LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
									"Chaves 'temp1', 'visc1', 'temp2', 'visc2' requeridas",
									chaveJson,
									"modeloOleoMorto = 0 ou ausente");
						} else {
							templ =
									fluidos_producao_json[indAtivo].temp1();
							lvisl =
									fluidos_producao_json[indAtivo].visc1();
							temph =
									fluidos_producao_json[indAtivo].temp2();
							lvish =
									fluidos_producao_json[indAtivo].visc2();
						}
					}

					// caso modelo oleo vivo preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoVivo().exists())
						//0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
						vcorrOV =
								fluidos_producao_json[indAtivo].modeloOleoVivo();

					// caso modelo oleo subsaturado preenchido
					if (fluidos_producao_json[indAtivo].modeloOleoSubSaturado().exists())
						//0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
						vcorrOS =
								fluidos_producao_json[indAtivo].modeloOleoSubSaturado();*/

					flup = ProFlu(vg1dSP, api, rgo, deng, bsw,
							denag, templ, lvisl, temph, lvish, tipoemul, aemul,
							bemul, PHI100, bswCorte, tabp, yco2, corrC, corrSat,
							vcorrOM, vcorrOV, vcorrOS, flashCompleto, 0,npseudo);
					flup.viscBlackOil = modelovisc;
					flup.modelaAgua=1;
					tabp=0;
					flup.tabelaDinamica=tipoTabelaDinamica;

					flup.TIndepPeneloux=compDet.TIndepPeneloux;
					flup.fatAcent=compDet.fatAcent;
					flup.masMol=compDet.masMol;
					flup.parac=compDet.parac;
					flup.presCrit=compDet.presCrit;
					flup.tempCrit=compDet.tempCrit;
					flup.CpIGCoefs=compDet.CpIGCoefs;
					flup.lij=compDet.lij;
					flup.kij=compDet.kij;
					flup.liqModel=compDet.liqModel;
					memcpy(flup.fracMol,compDet.fracMol, npseudo*sizeof(double));
					flup.Pmol=0.;
					for(int j=0;j<flup.npseudo;j++)flup.Pmol+=flup.fracMol[j]*flup.masMol[j];
					flup.atualizaPropCompStandard();

					flup.parserie=tabVisc.parserie;
					flup.viscTab=tabVisc.visc;
					flup.tempTab=tabVisc.temp;
					//flup.atualizaPropComp(90,10);
					/*FullMtx<double> fatores(3,flup.npseudo);
					for(int ipseu=0;ipseu<flup.npseudo;ipseu++){
						fatores[0][ipseu]=flup.fatAcent[ipseu];
						fatores[1][ipseu]=flup.masMol[ipseu];
						fatores[2][ipseu]=flup.masMol[ipseu];
					}
					cout<<fatores;
					cout<<dadosMRPre<<" "<<flup.iIER<<"   "<<flup.dStockTankVaporDensity<<"  "<<flup.dStockTankVaporMassFraction<<"\n";*/
					//flup.atualizaPropComp(50,50);
					/*flup.TIndepPeneloux=compDet.TIndepPeneloux;
					flup.fatAcent=compDet.fatAcent;
					flup.masMol=compDet.masMol;
					flup.parac=compDet.parac;
					flup.presCrit=compDet.presCrit;
					flup.tempCrit=compDet.tempCrit;
					flup.CpIGCoefs=compDet.CpIGCoefs;
					flup.lij=compDet.lij;
					flup.kij=compDet.kij;
					flup.liqModel=compDet.liqModel;
					memcpy(flup.fracMol,compDet.fracMol, npseudo*sizeof(double));*/
					//flup.atualizaPropCompStandard();
					//flup.atualizaPropComp(50,50);
					//double teste=flup.DZDT(50, 50);
					//teste=flup.BOFunc(50, 50);

			}
}




void PorosRadSimp::parse_fluido_complementar(Value& fluido_json) {
	// caso a propriedade "ativo" esteja habilitada
		// de-para do elementoPoisson "fluido_complementar" do json para o struct fluc
	int tipoflup=0;
	double masesp=0.;
	double compP = 0.;
	double compT = 0.;
	double tensup = 0.;
	double calesp = 0.;
	double condut = 0.;
	double templ = 0.;
	double lvisl = 0.;
	double temph = 0.;
	double lvish = 0.;
	double sal=0.;

	masesp =fluido_json["massaEspecifica"].GetDouble();
			//compP = fluido_json["compP"].GetDouble();
	compP = fluido_json["compP"].GetDouble();
			//tensup = fluido_json["tensup"].GetDouble();
	templ = fluido_json["temp1"].GetDouble();
	lvisl = fluido_json["visc1"].GetDouble();
	temph = fluido_json["temp2"].GetDouble();
	lvish = fluido_json["visc2"].GetDouble();

	fluc = ProFluCol(masesp, compP, compT, tensup, calesp,
				condut, sal, templ, lvisl, temph, lvish,0, tipoflup);
	fluc.npontos = 0;
}

void PorosRadSimp::parse_PW(Value& pW_json) {
	string chaveJson("#/pW");


	pW.nserie=1;
	//pW.rotulo=;
	pW.tempo=0;
	pW.val=0;
	pW.valSat=0;

	if (pW_json.HasMember("tempo"))
		pW.nserie =
				pW_json["tempo"].Size();
	else logger.log(LOGGER_FALHA,
			LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
					"Chave 'tempo' inexistente",chaveJson, "pW");

	pW.tempo=new double [pW.nserie];
	pW.val=new double [pW.nserie];
	pW.valSat=new double [pW.nserie];
	for(int i=0; i<pW.nserie; i++){
		pW.tempo[i]=pW_json["tempo"][i].GetDouble();
		pW.val[i]=pW_json["valor"][i].GetDouble();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//alteracao2

double PorosRadSimp::interpolaTabela(int nserie, double valx, double* x, double* y){
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

double PorosRadSimp::fkO(double satW, double satG){
	double krw=interpolaTabela(kRelOA.npont, satW, kRelOA.satW, kRelOA.permRelW);
	double krow=interpolaTabela(kRelOA.npont, satW, kRelOA.satW, kRelOA.permRelOW);
	double krowC=interpolaTabela(kRelOA.npont, satConata, kRelOA.satW, kRelOA.permRelOW);

	double val=krowC*(((krow/krowC)+krw)-(krw));

	return val;
}



void PorosRadSimp::geraCel(double** const vPcamada,double** const vsL,double** const vsW,
		const double vsLRes,const double vsWRes){
/*
 * 	celradSimp(int vncel=0,int vicel=0,double vr0=0., double vr1=0., double vrm=0.,double vKXL=0.,double vKXR=0.,
			double vKYL=0.,double vKYR=0.,double vporo=0.,double vcompresPoro=0.,double vPcamada=0,double vPint=0.,
			double vpresRes=0.,double vQL=0., double vQR=0, double vtRes=0, double vdt=0, double vzD=0,
			double vdh=1,ProFluCol vfluc=ProFluCol(), celradSimp* vcelL=0, celradSimp* vcelR=0);

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
    celradSimp* vcelL=0;
    celradSimp* vcelR=0;
    celula=new celradSimp [ncel];
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
    celula[konta]=celradSimp(vg1dSP,ncel,konta,r0, r2, r1,kxl,kxr,
			kyl,kyr,geom.poro[0],geom.compRoc[0],pcamada,Pint,
			presRes2,presRes,ql, qr, tRes, dt,zdKm, zdK,zdK2,
			dh,sL,sW,sLRes,sWPoc,sWRes,
			satConata,ip,vkRelOA,vkRelOG,vpcOA,vpcGO,
			fluc, flup/*,vcelL, vcelR*/);
    celula[konta].pBolha=pBolha;
    celula[konta].tipoModelOleo=tipoModelOleo;
    celula[konta].compOleo=compOleo;
    celula[konta].rhoRef=rhoRef;
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
			    celula[konta]=celradSimp(vg1dSP,ncel,konta,r0, r2, r1,kxl,kxr,
						kyl,kyr,geom.poro[icam],geom.compRoc[icam],pcamada,Pint,
						presRes2,presRes,ql, qr, tRes, dt,zdKm, zdK,zdK2,
						dh,sL,sW,sLRes,sWPoc,sWRes,
						satConata,ip,vkRelOA,vkRelOG,vpcOA,vpcGO,
						fluc, flup/*,vcelL, vcelR*/);
			    celula[konta].pBolha=pBolha;
			    celula[konta].tipoModelOleo=tipoModelOleo;
			    celula[konta].compOleo=compOleo;
			    celula[konta].rhoRef=rhoRef;
			    konta++;

			  }
		 }
	 }

	/* for(int i=0;i<ncel;i++){
		 if(i>0)celula[i].celL=&celula[i-1];
		 else celula[i].celL=&celula[i];
		 if(i<ncel-1)celula[i].celR=&celula[i+1];
		 else celula[i].celR=&celula[i];
	 }*/
}

double PorosRadSimp::marchaperm(double mastot){

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

double PorosRadSimp::transperm(double mastot){
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

void PorosRadSimp::renovaPres(int i, double mTot){

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



double PorosRadSimp::SIGN(double a, double b) {
  return (b >= 0 ? 1.0 : -1.0) * fabsl(a);
}


double  PorosRadSimp::zriddr(double x1,double x2){
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

void PorosRadSimp::indrazT(int& ind, double& raz) {
	for (int i = 0; i <= temp.parserie - 1; i++) {
		if (i < temp.parserie - 1) {
			if ((*vg1dSP).tempo >= temp.tempo[i] && (*vg1dSP).tempo < temp.tempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).tempo - temp.tempo[i])
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

void PorosRadSimp::indrazPW(int& ind, double& raz) {
	for (int i = 0; i <= pW.nserie - 1; i++) {
		if (i < pW.nserie - 1) {
			if ((*vg1dSP).tempo >= pW.tempo[i] && (*vg1dSP).tempo < pW.tempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).tempo - pW.tempo[i])
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

void PorosRadSimp::indraz(int& ind, double& raz, double* serieTempo, int nserie) {
	for (int i = 0; i <= nserie - 1; i++) {
		if (i < nserie - 1) {
			if ((*vg1dSP).tempo >= serieTempo[i] && (*vg1dSP).tempo < serieTempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).tempo - serieTempo[i])
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

void PorosRadSimp::atualizaCel(int i){
	for(int j=0; j<celula[i].flup.npseudo; j++)
		  celula[i].flup.fracMol[j]=celula[i-1].flup.fracMol[j];
	celula[i].flup.iCalculatedStockTankThermodynamicCondition=
		celula[i-1].flup.iCalculatedStockTankThermodynamicCondition;
	celula[i].flup.dStockTankVaporMassFraction=
		celula[i-1].flup.dStockTankVaporMassFraction;
	celula[i].flup.dStockTankLiquidDensity=
		celula[i-1].flup.dStockTankLiquidDensity;
	celula[i].flup.dStockTankVaporDensity=
		celula[i-1].flup.dStockTankVaporDensity;

	if(celula[i].flup.dStockTankLiquidDensity>0.01){
		celula[i].flup.API=141.5/(celula[i].flup.dStockTankLiquidDensity/1000.)-131.5;
	}
	else celula[i].flup.API=50;
	celula[i].flup.Deng=celula[i].flup.dStockTankVaporDensity/1.225;
	celula[i].flup.RGO=celula[i-1].flup.RGO;
	celula[i].flup.IRGO=celula[i-1].flup.IRGO;

	double tmed = tRes;
	double pmed = celula[i].pmed1;

	int veriI=i-1;
	while((celula[veriI].flup.dCalculatedBeta>1-(0.0+1e-15) ||
			celula[veriI].flup.dCalculatedBeta<(0.0+1e-15))
			&& (veriI>0 ))veriI--;
	if((veriI==0) && (celula[veriI].flup.dCalculatedBeta>1-(0.0+1e-15)
			|| celula[veriI].flup.dCalculatedBeta<(0.0+1e-15)))veriI=i-1;
	celula[i].flup.atualizaPropComp(pmed, tmed,celula[veriI].flup.dCalculatedBeta,
			celula[veriI].flup.oCalculatedLiqComposition,
			celula[veriI].flup.oCalculatedVapComposition,0);
}

void PorosRadSimp::preparaTabDin(){
	for(int i=0;i<ntabDin;i++){
		int i0=tabDin[i].celIni;
		int i1=tabDin[i].celFim;
		tabDin[i].pmax=celula[i0].Pcamada;
		tabDin[i].pmin=celula[i0].Pcamada;
		tabDin[i].tmax=celula[i0].tRes;
		tabDin[i].tmin=celula[i0].tRes;
		for(int j=i0+1;j<=i1;j++){
			if(celula[j].Pcamada<tabDin[i].pmin)tabDin[i].pmin=celula[j].Pcamada;
			else if(celula[j].Pcamada>tabDin[i].pmax)tabDin[i].pmax=celula[j].Pcamada;
			if(celula[j].tRes<tabDin[i].tmin)tabDin[i].tmin=celula[j].tRes;
			else if(celula[j].tRes>tabDin[i].tmax)tabDin[i].tmax=celula[j].tRes;
		}
		tabDin[i].pmax*=1.2;
		tabDin[i].pmin*=0.8;
		//tabDin[i].tmax=tabDin[i].tmax;
		//tabDin[i].tmin=tabDin[i].tmin;
		if(tabDin[i].tmin<-30)tabDin[i].tmin=-30;
		tabDin[i].delP=5.;
		tabDin[i].delT=5.;
		tabDin[i].npontosT=floor((tabDin[i].tmax-tabDin[i].tmin)/(tabDin[i].delT))+1;
		tabDin[i].npontosP=floor((tabDin[i].pmax-tabDin[i].pmin)/(tabDin[i].delP))+1;
		if(tabDin[i].npontosP<=3)tabDin[i].npontosP=4;
		tabDin[i].delP=(tabDin[i].pmax-tabDin[i].pmin)/
				               (tabDin[i].npontosP-1);
		tabDin[i].delT=(tabDin[i].tmax-tabDin[i].tmin)/
	               (tabDin[i].npontosT-1);

	}
	int i0=tabDin[0].celIni;
	int i1=tabDin[0].celFim;
	celula[i0].flup.atualizaPropCompStandard();
	double pmed = celula[i0].pmed1;
	double tmed=celula[i0].tRes;
	celula[i0].flup.atualizaPropComp(pmed, tmed,-1,NULL,NULL,0);
	for(int i=i0+1;i<=i1;i++){
		atualizaCel(i);
		//celula[i].flup.atualizaPropCompStandard();
	}

	for(int j=1;j<ntabDin;j++){
		int i0=tabDin[j].celIni;
		int i1=tabDin[j].celFim;
		celula[i0].flup.atualizaPropCompStandard();
		tmed = tRes;
		pmed = celula[i0].pmed1;
		//celula[i0].flup.atualizaPropComp(pmed, tmed,-1,NULL,NULL,0);
		celula[i0].flup.atualizaPropComp(pmed, tmed,celula[i0-1].flup.dCalculatedBeta,
					celula[i0-1].flup.oCalculatedLiqComposition,
					celula[i0-1].flup.oCalculatedVapComposition,0);
		for(int i=i0+1;i<=i1;i++){
			for(int k=0; k<celula[i].flup.npseudo;k++)
				celula[i].flup.fracMol[k]=celula[i-1].flup.fracMol[k];
			atualizaCel(i);
			//celula[i].flup.atualizaPropCompStandard();
		}
	}
	for(int i=0;i<ntabDin;i++){
		//tabDin[i].delT=(tabDin[i].tmax-tabDin[i].tmin)/(tabDin[i].npontosT-1.);
		//tabDin[i].delP=(tabDin[i].pmax-tabDin[i].pmin)/(tabDin[i].npontosP-1.);
		tabDin[i].rholF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].rhogF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].DrholDpF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].DrhogDpF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].DrholDtF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].DrhogDtF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].valBO=new double*[tabDin[i].npontosP + 1];
		tabDin[i].tit=new double*[tabDin[i].npontosP + 1];
		tabDin[i].rs=new double*[tabDin[i].npontosP + 1];
		tabDin[i].cplF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].cpgF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].valZ=new double*[tabDin[i].npontosP + 1];
		tabDin[i].HlF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].HgF=new double*[tabDin[i].npontosP + 1];
		tabDin[i].valdZdT=new double*[tabDin[i].npontosP + 1];
		tabDin[i].valdZdP=new double*[tabDin[i].npontosP + 1];
		tabDin[i].viscO=new double*[tabDin[i].npontosP + 1];
		tabDin[i].viscG=new double*[tabDin[i].npontosP + 1];
		tabDin[i].PBF=new double[tabDin[i].npontosT];
		tabDin[i].TBF=new double[tabDin[i].npontosT];
		for(int k=0;k<tabDin[i].npontosP+1;k++){
			tabDin[i].rholF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].rhogF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].DrholDpF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].DrhogDpF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].DrholDtF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].DrhogDtF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].valBO[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].tit[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].rs[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].cplF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].cpgF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].valZ[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].HlF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].HgF[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].valdZdT[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].valdZdP[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].viscO[k]=new double[tabDin[i].npontosT + 1];
			tabDin[i].viscG[k]=new double[tabDin[i].npontosT + 1];
		}
		for (int k = 0; k < tabDin[i].npontosP + 1; k++) {
			for (int j = 0; j < tabDin[i].npontosT + 1; j++) {
				tabDin[i].rholF[k][j] = 0;
				tabDin[i].rhogF[k][j] = 0;
				tabDin[i].DrholDpF[k][j] = 0;
				tabDin[i].DrhogDpF[k][j] = 0;
				tabDin[i].DrholDtF[k][j] = 0;
				tabDin[i].DrhogDtF[k][j] = 0;
				tabDin[i].valBO[k][j] = 0;
				tabDin[i].tit[k][j] = 0;
				tabDin[i].rs[k][j] = 0;
				tabDin[i].cplF[k][j] = 0;
				tabDin[i].cpgF[k][j] = 0;
				tabDin[i].valZ[k][j] = 0;
				tabDin[i].HlF[k][j] = 0;
				tabDin[i].HgF[k][j] = 0;
				tabDin[i].valdZdT[k][j] = 0;
				tabDin[i].valdZdP[k][j] = 0;
				tabDin[i].viscO[k][j] = 0;
				tabDin[i].viscG[k][j] = 0;
			}
		}
		for (int k = 0; k < tabDin[i].npontosT; k++) {
			tabDin[i].PBF[k] = 0;
			tabDin[i].TBF[k] = 0;
		}
		tabDin[i].TBF[0] = tabDin[i].tmin;
		tabDin[i].rholF[0][1] = tabDin[i].tmin;
		tabDin[i].rhogF[0][1] = tabDin[i].tmin;
		tabDin[i].DrholDpF[0][1] = tabDin[i].tmin;
		tabDin[i].DrhogDpF[0][1] = tabDin[i].tmin;
		tabDin[i].DrholDtF[0][1] = tabDin[i].tmin;
		tabDin[i].DrhogDtF[0][1] = tabDin[i].tmin;
		tabDin[i].valBO[0][1] = tabDin[i].tmin;
		tabDin[i].tit[0][1] = tabDin[i].tmin;
		tabDin[i].rs[0][1] = tabDin[i].tmin;
		tabDin[i].cplF[0][1] = tabDin[i].tmin;
		tabDin[i].cpgF[0][1] = tabDin[i].tmin;
		tabDin[i].valZ[0][1] = tabDin[i].tmin;
		tabDin[i].HlF[0][1] = tabDin[i].tmin;
		tabDin[i].HgF[0][1] = tabDin[i].tmin;
		tabDin[i].valdZdT[0][1] = tabDin[i].tmin;
		tabDin[i].valdZdP[0][1] = tabDin[i].tmin;
		tabDin[i].viscO[0][1] = tabDin[i].tmin;
		tabDin[i].viscG[0][1] = tabDin[i].tmin;
		tabDin[i].rholF[1][0] = tabDin[i].pmin;
		tabDin[i].rhogF[1][0] = tabDin[i].pmin;
		tabDin[i].DrholDpF[1][0] = tabDin[i].pmin;
		tabDin[i].DrhogDpF[1][0] = tabDin[i].pmin;
		tabDin[i].DrholDtF[1][0] = tabDin[i].pmin;
		tabDin[i].DrhogDtF[1][0] = tabDin[i].pmin;
		tabDin[i].valBO[1][0] = tabDin[i].pmin;
		tabDin[i].tit[1][0] = tabDin[i].pmin;
		tabDin[i].rs[1][0] = tabDin[i].pmin;
		tabDin[i].cplF[1][0] = tabDin[i].pmin;
		tabDin[i].cpgF[1][0] = tabDin[i].pmin;
		tabDin[i].valZ[1][0] = tabDin[i].pmin;
		tabDin[i].HlF[1][0] = tabDin[i].pmin;
		tabDin[i].HgF[1][0] = tabDin[i].pmin;
		tabDin[i].valdZdT[1][0] = tabDin[i].pmin;
		tabDin[i].valdZdP[1][0] = tabDin[i].pmin;
		tabDin[i].viscO[1][0] = tabDin[i].pmin;
		tabDin[i].viscG[1][0] = tabDin[i].pmin;
		for (int k = 1; k < tabDin[i].npontosT; k++){
			tabDin[i].TBF[k] = tabDin[i].TBF[k-1]+tabDin[i].delT;
			tabDin[i].rholF[0][k+1] = tabDin[i].rholF[0][k]+tabDin[i].delT;
			tabDin[i].rhogF[0][k+1] = tabDin[i].rhogF[0][k]+tabDin[i].delT;
			tabDin[i].DrholDpF[0][k+1] = tabDin[i].DrholDpF[0][k]+tabDin[i].delT;
			tabDin[i].DrhogDpF[0][k+1] = tabDin[i].DrhogDpF[0][k]+tabDin[i].delT;
			tabDin[i].DrholDtF[0][k+1] = tabDin[i].DrholDtF[0][k]+tabDin[i].delT;
			tabDin[i].DrhogDtF[0][k+1] = tabDin[i].DrhogDtF[0][k]+tabDin[i].delT;
			tabDin[i].valBO[0][k+1] = tabDin[i].valBO[0][k]+tabDin[i].delT;
			tabDin[i].tit[0][k+1] = tabDin[i].tit[0][k]+tabDin[i].delT;
			tabDin[i].rs[0][k+1] = tabDin[i].rs[0][k]+tabDin[i].delT;
			tabDin[i].cplF[0][k+1] = tabDin[i].cplF[0][k]+tabDin[i].delT;
			tabDin[i].cpgF[0][k+1] = tabDin[i].cpgF[0][k]+tabDin[i].delT;
			tabDin[i].valZ[0][k+1] = tabDin[i].valZ[0][k]+tabDin[i].delT;
			tabDin[i].HlF[0][k+1] = tabDin[i].HlF[0][k]+tabDin[i].delT;
			tabDin[i].HgF[0][k+1] = tabDin[i].HgF[0][k]+tabDin[i].delT;
			tabDin[i].valdZdT[0][k+1] = tabDin[i].valdZdT[0][k]+tabDin[i].delT;
			tabDin[i].valdZdP[0][k+1] = tabDin[i].valdZdP[0][k]+tabDin[i].delT;
			tabDin[i].viscO[0][k+1] = tabDin[i].viscO[0][k]+tabDin[i].delT;
			tabDin[i].viscG[0][k+1] = tabDin[i].viscG[0][k]+tabDin[i].delT;
		}
		for (int k = 1; k < tabDin[i].npontosP; k++){
			tabDin[i].rholF[k+1][0] = tabDin[i].rholF[k][0]+tabDin[i].delP;
			tabDin[i].rhogF[k+1][0] = tabDin[i].rhogF[k][0]+tabDin[i].delP;
			tabDin[i].DrholDpF[k+1][0] = tabDin[i].DrholDpF[k][0]+tabDin[i].delP;
			tabDin[i].DrhogDpF[k+1][0] = tabDin[i].DrhogDpF[k][0]+tabDin[i].delP;
			tabDin[i].DrholDtF[k+1][0] = tabDin[i].DrholDtF[k][0]+tabDin[i].delP;
			tabDin[i].DrhogDtF[k+1][0] = tabDin[i].DrhogDtF[k][0]+tabDin[i].delP;
			tabDin[i].valBO[k+1][0] = tabDin[i].valBO[k][0]+tabDin[i].delP;
			tabDin[i].tit[k+1][0] = tabDin[i].tit[k][0]+tabDin[i].delP;
			tabDin[i].rs[k+1][0] = tabDin[i].rs[k][0]+tabDin[i].delP;
			tabDin[i].cplF[k+1][0] = tabDin[i].cplF[k][0]+tabDin[i].delP;
			tabDin[i].cpgF[k+1][0] = tabDin[i].cpgF[k][0]+tabDin[i].delP;
			tabDin[i].valZ[k+1][0] = tabDin[i].valZ[k][0]+tabDin[i].delP;
			tabDin[i].HlF[k+1][0] = tabDin[i].HlF[k][0]+tabDin[i].delP;
			tabDin[i].HgF[k+1][0] = tabDin[i].HgF[k][0]+tabDin[i].delP;
			tabDin[i].valdZdT[k+1][0] = tabDin[i].valdZdT[k][0]+tabDin[i].delP;
			tabDin[i].valdZdP[k+1][0] = tabDin[i].valdZdP[k][0]+tabDin[i].delP;
			tabDin[i].viscO[k+1][0] = tabDin[i].viscO[k][0]+tabDin[i].delP;
			tabDin[i].viscG[k+1][0] = tabDin[i].viscG[k][0]+tabDin[i].delP;
		}
        //#pragma omp parallel for num_threads(ntrd)
		for (int k = 1; k < tabDin[i].npontosP+1; k++){
			ProFlu fluF;
			for (int j = 1; j < tabDin[i].npontosT+1; j++){
				double pres=tabDin[i].rholF[k][0];
				double temp=tabDin[i].rholF[0][j];
				int indIni=tabDin[i].celIni;
				fluF=celula[indIni].flup;
				if(/*k==1 &&*/ j==1)fluF.atualizaPropComp(pres, temp,-1,NULL,NULL,0);
				else
					fluF.atualizaPropComp(pres, temp,fluF.dCalculatedBeta,fluF.oCalculatedLiqComposition,
							fluF.oCalculatedVapComposition,0);
				tabDin[i].rholF[k][j] = fluF.MasEspoleo(pres, temp);
				tabDin[i].rhogF[k][j] = fluF.MasEspGas(pres, temp);
				tabDin[i].DrhogDpF[k][j] = fluF.drhodp(pres, temp);
				tabDin[i].DrholDtF[k][j] = fluF.DrholDT(pres, temp);
				tabDin[i].DrhogDtF[k][j] = fluF.drhodt(pres, temp);
				tabDin[i].valBO[k][j] = fluF.BOFunc(pres, temp);
				tabDin[i].valZ[k][j] = fluF.Zdran(pres, temp);
				tabDin[i].valdZdT[k][j] = fluF.DZDT(pres, temp);
				tabDin[i].valdZdP[k][j] = fluF.DZDP(pres, temp);
				tabDin[i].tit[k][j] = fluF.FracMass(pres, temp);
				tabDin[i].rs[k][j] = fluF.RS(pres, temp);
				tabDin[i].cplF[k][j] = fluF.CalorLiq(pres, temp);
				tabDin[i].cpgF[k][j] = fluF.CalorGas(pres, temp);
				tabDin[i].HlF[k][j] = fluF.EntalpLiq(pres,temp);
				tabDin[i].HgF[k][j] = fluF.EntalpGas(pres,temp);
				tabDin[i].viscO[k][j] = fluF.ViscOleo(pres,temp);
				if(k==tabDin[i].npontosP)tabDin[i].PBF[j-1]  =fluF.PB(pres, temp);
			}
		}
	}

	for(int i=0;i<ntabDin;i++){
		int i0=tabDin[i].celIni;
		int i1=tabDin[i].celFim;
		for(int j=i0;j<=i1;j++){
			celula[j].flup.tabDin.TBF=tabDin[i].TBF;
			celula[j].flup.tabDin.PBF=tabDin[i].PBF;
			celula[j].flup.tabDin.rholF=tabDin[i].rholF;
			celula[j].flup.tabDin.rhogF=tabDin[i].rhogF;
			celula[j].flup.tabDin.DrholDpF=tabDin[i].DrholDpF;
			celula[j].flup.tabDin.DrhogDpF=tabDin[i].DrhogDpF;
			celula[j].flup.tabDin.DrholDtF=tabDin[i].DrholDtF;
			celula[j].flup.tabDin.DrhogDtF=tabDin[i].DrhogDtF;
			celula[j].flup.tabDin.valBO=tabDin[i].valBO;
			celula[j].flup.tabDin.tit=tabDin[i].tit;
			celula[j].flup.tabDin.rs=tabDin[i].rs;
			celula[j].flup.tabDin.cplF=tabDin[i].cplF;
			celula[j].flup.tabDin.cpgF=tabDin[i].cpgF;
			celula[j].flup.tabDin.valZ=tabDin[i].valZ;
			celula[j].flup.tabDin.HlF=tabDin[i].HlF;
			celula[j].flup.tabDin.HgF=tabDin[i].HgF;
			celula[j].flup.tabDin.valdZdT=tabDin[i].valdZdT;
			celula[j].flup.tabDin.valdZdP=tabDin[i].valdZdP;
			celula[j].flup.tabDin.viscO=tabDin[i].viscO;
			celula[j].flup.tabDin.viscG=tabDin[i].viscG;
			celula[j].flup.tabDin.delP=tabDin[i].delP;
			celula[j].flup.tabDin.delT=tabDin[i].delT;
			celula[j].flup.tabDin.npontosP=tabDin[i].npontosP;
			celula[j].flup.tabDin.npontosT=tabDin[i].npontosT;
			celula[j].flup.tabDin.pmax=tabDin[i].pmax;
			celula[j].flup.tabDin.pmin=tabDin[i].pmin;
			celula[j].flup.tabDin.tmax=tabDin[i].tmax;
			celula[j].flup.tabDin.tmin=tabDin[i].tmin;
		}
	}

	for(int i=0; i<=ncel;i++)celula[i].flup.tabelaDinamica=1;
}


void PorosRadSimp::auxMiniTab(ProFlu& flui) {
	ProFlu fluC;
	fluC=flui;
	fluC.atualizaPropCompStandard();
	if(fluC.dCalculatedBeta>0. && fluC.dCalculatedBeta<1.)
	fluC.atualizaPropComp(flui.miniTabDin.pmin, flui.miniTabDin.tmin,
						  fluC.dCalculatedBeta, fluC.oCalculatedLiqComposition,
						  fluC.oCalculatedVapComposition,0);
	else
	fluC.atualizaPropComp(flui.miniTabDin.pmin, flui.miniTabDin.tmin,-1,NULL,NULL,0);
	//fluC.atualizaPropComp(celula[i].flui.miniTabDin.pmin, celula[i].flui.miniTabDin.tmin,
			//-1,NULL,NULL,arq.pocinjec);
	flui.miniTabDin.rholF[0][0] =
			fluC.MasEspoleo(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.rhogF[0][0] =
			fluC.MasEspGas(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.DrhogDpF[0][0] =
			fluC.drhodp(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.DrholDtF[0][0] =
			fluC.DrholDT(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.DrhogDtF[0][0] =
			fluC.drhodt(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.valBO[0][0] =
			fluC.BOFunc(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.valZ[0][0] =
			fluC.Zdran(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.valdZdT[0][0] =
			fluC.DZDT(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.valdZdP[0][0] =
			fluC.DZDP(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.tit[0][0] =
			fluC.FracMass(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.rs[0][0] =
			fluC.RS(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.cplF[0][0] =
			fluC.CalorLiq(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.cpgF[0][0] =
			fluC.CalorGas(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.HlF[0][0] =
			fluC.EntalpLiq(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.HgF[0][0] =
			fluC.EntalpGas(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	flui.miniTabDin.PBF[0]  =
			fluC.PB(flui.miniTabDin.pmin, flui.miniTabDin.tmin);
	///////////////////////////////////////////////////////////////////////////////////////////////
	if(fluC.dCalculatedBeta>0. && fluC.dCalculatedBeta<1.)
	fluC.atualizaPropComp(flui.miniTabDin.pmin, flui.miniTabDin.tmax,
						  fluC.dCalculatedBeta, fluC.oCalculatedLiqComposition,
						  fluC.oCalculatedVapComposition,0);
	else
	fluC.atualizaPropComp(flui.miniTabDin.pmin, flui.miniTabDin.tmax,-1,NULL,NULL,0);
	flui.miniTabDin.rholF[0][1] =
			fluC.MasEspoleo(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.rhogF[0][1] =
			fluC.MasEspGas(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.DrhogDpF[0][1] =
			fluC.drhodp(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.DrholDtF[0][1] =
			fluC.DrholDT(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.DrhogDtF[0][1] =
			fluC.drhodt(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.valBO[0][1] =
			fluC.BOFunc(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.valZ[0][1] =
			fluC.Zdran(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.valdZdT[0][1] =
			fluC.DZDT(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.valdZdP[0][1] =
			fluC.DZDP(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.tit[0][1] =
			fluC.FracMass(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.rs[0][1] =
			fluC.RS(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.cplF[0][1] =
			fluC.CalorLiq(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.cpgF[0][1] =
			fluC.CalorGas(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.HlF[0][1] =
			fluC.EntalpLiq(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.HgF[0][1] =
			fluC.EntalpGas(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	flui.miniTabDin.PBF[1]  =
			fluC.PB(flui.miniTabDin.pmin, flui.miniTabDin.tmax);
	///////////////////////////////////////////////////////////////////////////////////////////////
	if(fluC.dCalculatedBeta>0. && fluC.dCalculatedBeta<1.)
	fluC.atualizaPropComp(flui.miniTabDin.pmax, flui.miniTabDin.tmin,
						  fluC.dCalculatedBeta, fluC.oCalculatedLiqComposition,
						  fluC.oCalculatedVapComposition,0);
	else
	fluC.atualizaPropComp(flui.miniTabDin.pmax, flui.miniTabDin.tmin,-1,NULL,NULL,0);
	flui.miniTabDin.rholF[1][0] =
			fluC.MasEspoleo(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.rhogF[1][0] =
			fluC.MasEspGas(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.DrhogDpF[1][0] =
			fluC.drhodp(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.DrholDtF[1][0] =
			fluC.DrholDT(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.DrhogDtF[1][0] =
			fluC.drhodt(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.valBO[1][0] =
			fluC.BOFunc(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.valZ[1][0] =
			fluC.Zdran(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.valdZdT[1][0] =
			fluC.DZDT(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.valdZdP[1][0] =
			fluC.DZDP(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.tit[1][0] =
			fluC.FracMass(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.rs[1][0] =
			fluC.RS(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.cplF[1][0] =
			fluC.CalorLiq(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.cpgF[1][0] =
			fluC.CalorGas(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.HlF[1][0] =
			fluC.EntalpLiq(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	flui.miniTabDin.HgF[1][0] =
			fluC.EntalpGas(flui.miniTabDin.pmax, flui.miniTabDin.tmin);
	///////////////////////////////////////////////////////////////////////////////////////////////
	if(fluC.dCalculatedBeta>0. && fluC.dCalculatedBeta<1.)
	fluC.atualizaPropComp(flui.miniTabDin.pmax, flui.miniTabDin.tmax,
						  fluC.dCalculatedBeta, fluC.oCalculatedLiqComposition,
						  fluC.oCalculatedVapComposition,0);
	else
	fluC.atualizaPropComp(flui.miniTabDin.pmax, flui.miniTabDin.tmax,-1,NULL,NULL,0);
	flui.miniTabDin.rholF[1][1] =
			fluC.MasEspoleo(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.rhogF[1][1] =
			fluC.MasEspGas(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.DrhogDpF[1][1] =
			fluC.drhodp(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.DrholDtF[1][1] =
			fluC.DrholDT(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.DrhogDtF[1][1] =
			fluC.drhodt(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.valBO[1][1] =
			fluC.BOFunc(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.valZ[1][1] =
			fluC.Zdran(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.valdZdT[1][1] =
			fluC.DZDT(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.valdZdP[1][1] =
			fluC.DZDP(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.tit[1][1] =
			fluC.FracMass(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.rs[1][1] =
			fluC.RS(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.cplF[1][1] =
			fluC.CalorLiq(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.cpgF[1][1] =
			fluC.CalorGas(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.HlF[1][1] =
			fluC.EntalpLiq(flui.miniTabDin.pmax, flui.miniTabDin.tmax);
	flui.miniTabDin.HgF[1][1] =
			fluC.EntalpGas(flui.miniTabDin.pmax,flui.miniTabDin.tmax);
}

void PorosRadSimp::geraMiniTabFlu(){
	flup.miniTabDin.pmax=pInt+10;
	flup.miniTabDin.pmin=pInt-10;
	flup.miniTabDin.tmax=tRes+1;
	flup.miniTabDin.tmin=tRes-1;
	flup.atualizaPropCompStandard();
	if(flup.dCalculatedBeta>0. && flup.dCalculatedBeta<1.)
	flup.atualizaPropComp(pInt, tRes,
			flup.dCalculatedBeta, flup.oCalculatedLiqComposition,
			flup.oCalculatedVapComposition,0);
	else
	flup.atualizaPropComp(pInt, tRes,-1,NULL,NULL,0);
	auxMiniTab(flup);
	int ncelRad=ncel;
	for(int j=0; j<ncelRad;j++){
		celula[j].flup.miniTabDin.pmax=celula[j].Pcamada+10;
		celula[j].flup.miniTabDin.pmin=celula[j].Pcamada-10;
		celula[j].flup.miniTabDin.tmax=tRes+1;
		celula[j].flup.miniTabDin.tmin=tRes-1;
		celula[j].flup.atualizaPropCompStandard();
		if(celula[j].flup.dCalculatedBeta>0. &&
				celula[j].flup.dCalculatedBeta<1.)
		celula[j].flup.atualizaPropComp(celula[j].Pcamada,
				celula[j].tRes,
				celula[j].flup.dCalculatedBeta,
				celula[j].flup.oCalculatedLiqComposition,
				celula[j].flup.oCalculatedVapComposition,0);
		else
		celula[j].flup.atualizaPropComp(celula[j].Pcamada,
				celula[j].tRes,-1,NULL,NULL,0);
		auxMiniTab(celula[j].flup);
	}
}

void PorosRadSimp::defineDT(int perm){
	  int ind=0;
	  double raz;
	  double delt;
	  if(perm==0){
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
	  }
	  else{
		  dt=2*temp.dtmax[0];
	  }

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
}

void PorosRadSimp::atualizaIni(){
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


double PorosRadSimp::transtrans(int perm){

 int nglob=0;
 for(int i=0; i<ncel;i++){
	celula[i].Pint=Pint;
	celula[i].sWPoc=sWPoc;
 }
 for(int i=0;i<geom.ncamadas;i++)nglob+=ncamada[i];
 nglob=4*(nglob+1);
 Vcr<double> vetliv(nglob);
 BandMtx<double> matglob(nglob, 3, 4);
 int konta;
 int idisc;
 double erro=100.;
 int iterPres=0;
 while(erro>1e-5 && iterPres<20){
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

  double pAux;
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


  tit=flup.FracMassHidra(Pint, tRes);
  rhogP=flup.MasEspGas(Pint, tRes);
  rhoP=flup.MasEspoleo(Pint, tRes);
  alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
  fluxIni=2.*M_PI*vetliv[1]*(1-alf)*espessuraRes*rhoP;
  fluxIniG=2.*M_PI*vetliv[1]*alf*espessuraRes*rhogP;
  fluxIniA=2.*M_PI*vetliv[3]*espessuraRes*celula[0].flup.MasEspAgua(Pint, tRes);
  konta-=4;
  tit=flup.FracMassHidra(celula[ncel-1].Pcamada, tRes);
  rhogP=flup.MasEspGas(celula[ncel-1].Pcamada, tRes);
  rhoP=flup.MasEspoleo(celula[ncel-1].Pcamada, tRes);
  alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
  fluxFim=2.*M_PI*vetliv[konta+1]*(1-alf)*espessuraRes*rhoP;
  fluxFimG=2.*M_PI*vetliv[konta+1]*alf*espessuraRes*rhogP;
  fluxFimA=2.*M_PI*vetliv[konta+3]*espessuraRes*
		  celula[0].flup.MasEspAgua(celula[ncel-1].Pcamada, tRes);

  if(perm==0 && acessorio==0){
	  cout << "  erro= "<<erro<<";"<<" iteracoes= "<<iterPres;
	  cout << endl;
  }

  return fluxIni;

}

void PorosRadSimp::pseudoTrans(int partida){

	Pint = pW.val[0];

	sLRes = satLRes[0];

	sWRes = satARes[0];

	sWPoc = satAPoc[0];

	presRes = pRes[0];

	if(partida==1){
		for(int i=0; i<ncel;i++){
			celula[i].Pint=Pint;
			celula[i].presRes=presRes;
			celula[i].sLRes=sLRes;
			celula[i].sWRes=sWRes;
			celula[i].sWPoc=sWPoc;
		}

		/*for(int i=0; i<ncel;i++){
			double rs;
			double bo;
			double ba;
			if(celula[i].flup.RGO<1e7){
				double pAux=celula[i].Pcamada;
				  rs = celula[i].flup.RS(pAux, celula[i].tRes);
				  bo = celula[i].flup.BOFunc(pAux, celula[i].tRes,rs);
				  ba = celula[i].flup.BAFunc(pAux, celula[i].tRes);
				  rs = rs * 6.29 / 35.31467;
			  }
			  else{
				  bo=1;
				  rs=0;
				  ba=0.;
			  }
			 double vfw = celula[ncel-2].BSW*ba / (bo + ba*celula[ncel-2].BSW -
					             celula[ncel-2].BSW * bo);
			  celula[i].sW=vfw*(1.-celula[i].satConata)+celula[i].satConata;
			  //celula[i].Pcamada=celula[i].presRes;
			  if(i>0){
				  celula[i-1].sWR=celula[i].sW;
				  //celula[i-1].PcamadaR=celula[i].Pcamada;
			  }
			  if(i<ncel-1){
				  celula[i+1].sWL=celula[i].sW;
				  //celula[i+1].PcamadaL=celula[i].Pcamada;
			  }

		}*/
	}
	erroPermanente=1000.;
	double erroPermanenteSW=1000.;
	int kontaItera=0;
	while((erroPermanente>Pint/10000. /*|| erroPermanenteSW>(1./10.)*/) || kontaItera<3){
		/*if(Pint<230){
			int para;
			para=0;
		}*/

		int ciclo=0;
		reinicia=0;
		defineDT(1);
		for(int i=0;i<ncel;i++){
			celula[i].dPdT=0.;
			//celula[i].calcVazOWG();
		}
		while(ciclo<1){
			for(int i=0;i<ncel;i++){
				if(i>=ncel-1 && celula[ncel-2].QcamadaL>0.){
					celula[i].sL=sLRes;
					celula[i-1].sLR=sLRes;
					celula[i].sW=sWRes;
					celula[i-1].sWR=sWRes;
					celula[i].sLR=sLRes;
					celula[i].sWR=sWRes;
					celula[i].reiniciaSL=0;
					celula[i].reiniciaSW=0;
				}
				else if(i==0 && celula[1].QcamadaR<0.){
					celula[i].sL=sLRes;
					//dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
					celula[i].sW=sWPoc;
					//dados.transfer.celula[i-1].sWR=dados.transfer.sWPoc;
					celula[i].sLR=sLRes;
					celula[i].sWR=sWPoc;
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
			transtrans(1);
			BSW=celula[2].BSW;
			dVazdPw();
			if(flashCompleto==2){
				for(int i=0;i<ncel;i++){
		 	        if(celula[i].flup.dCalculatedBeta<0. || celula[i].flup.dCalculatedBeta>1.)
		 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada,  tRes,-1,NULL,NULL,0);
		 	        else
		 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada, tRes,celula[i].flup.dCalculatedBeta,
		 	        			celula[i].flup.oCalculatedLiqComposition,
								celula[i].flup.oCalculatedVapComposition,0);
				}
			}
			ciclo++;
		}
		erroPermanente=0.;
		erroPermanenteSW=0.;
		for(int i=0;i<ncel;i++){
			erroPermanente+=fabsl(celula[i].Pcamada-celula[i].Pini);
			//erroPermanenteSW+=fabsl(celula[i].sW-celula[i].sWini);
		}
		/*for(int i=3;i<ncel;i++){
			erroPermanenteSW+=fabsl(celula[i].BSW-celula[2].BSW);
		}*/
		erroPermanenteSW=fabsl(celula[ncel-2].BSW-celula[2].BSW);
		erroPermanente/=ncel;
		atualizaIni();
		kontaItera++;
	}
}

void PorosRadSimp::pseudoTrans2(int partida){

	Pint = pW.val[0];

	sLRes = satLRes[0];

	sWRes = satARes[0];

	sWPoc = satAPoc[0];

	presRes = pRes[0];

	if(partida==1){
		for(int i=0; i<ncel;i++){
			celula[i].Pint=Pint;
			celula[i].presRes=presRes;
			celula[i].sLRes=sLRes;
			celula[i].sWRes=sWRes;
			celula[i].sWPoc=sWPoc;
		}

		/*for(int i=0; i<ncel;i++){
			double rs;
			double bo;
			double ba;
			if(celula[i].flup.RGO<1e7){
				double pAux=celula[i].Pcamada;
				  rs = celula[i].flup.RS(pAux, celula[i].tRes);
				  bo = celula[i].flup.BOFunc(pAux, celula[i].tRes,rs);
				  ba = celula[i].flup.BAFunc(pAux, celula[i].tRes);
				  rs = rs * 6.29 / 35.31467;
			  }
			  else{
				  bo=1;
				  rs=0;
				  ba=0.;
			  }
			 double vfw = celula[ncel-2].BSW*ba / (bo + ba*celula[ncel-2].BSW -
					             celula[ncel-2].BSW * bo);
			  celula[i].sW=vfw*(1.-celula[i].satConata)+celula[i].satConata;
			  //celula[i].Pcamada=celula[i].presRes;
			  if(i>0){
				  celula[i-1].sWR=celula[i].sW;
				  //celula[i-1].PcamadaR=celula[i].Pcamada;
			  }
			  if(i<ncel-1){
				  celula[i+1].sWL=celula[i].sW;
				  //celula[i+1].PcamadaL=celula[i].Pcamada;
			  }

		}*/
	}
	erroPermanente=1000.;
	double erroPermanenteSW=1000.;
	int kontaItera=0;
	while((erroPermanente>Pint/10000. || erroPermanenteSW>(1./100.)) || kontaItera<3){
		/*if(Pint<230){
			int para;
			para=0;
		}*/

		int ciclo=0;
		reinicia=0;
		defineDT(1);
		for(int i=0;i<ncel;i++){
			celula[i].dPdT=0.;
			//celula[i].calcVazOWG();
		}
		while(ciclo<1){
			for(int i=0;i<ncel;i++){
				if(i>=ncel-1 && celula[ncel-2].QcamadaL>0.){
					celula[i].sL=sLRes;
					celula[i-1].sLR=sLRes;
					celula[i].sW=sWRes;
					celula[i-1].sWR=sWRes;
					celula[i].sLR=sLRes;
					celula[i].sWR=sWRes;
					celula[i].reiniciaSL=0;
					celula[i].reiniciaSW=0;
				}
				else if(i==0 && celula[1].QcamadaR<0.){
					celula[i].sL=sLRes;
					//dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
					celula[i].sW=sWPoc;
					//dados.transfer.celula[i-1].sWR=dados.transfer.sWPoc;
					celula[i].sLR=sLRes;
					celula[i].sWR=sWPoc;
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
			transtrans(1);
			BSW=celula[0].BSW;
			dVazdPw();
			if(flashCompleto==2){
				for(int i=0;i<ncel;i++){
		 	        if(celula[i].flup.dCalculatedBeta<0. || celula[i].flup.dCalculatedBeta>1.)
		 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada,  tRes,-1,NULL,NULL,0);
		 	        else
		 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada, tRes,celula[i].flup.dCalculatedBeta,
		 	        			celula[i].flup.oCalculatedLiqComposition,
								celula[i].flup.oCalculatedVapComposition,0);
				}
			}
			ciclo++;
		}
		erroPermanente=0.;
		erroPermanenteSW=0.;
		double erroTemp=0;
		for(int i=0;i<ncel;i++){
			erroPermanente+=fabsl(celula[i].Pcamada-celula[i].Pini);
		}
		for(int i=2;i<ncel-1;i++){
			erroTemp=fabsl(celula[i].BSW-celula[ncel-2].BSW);
			if(erroTemp>erroPermanenteSW)erroPermanenteSW=erroTemp;
		}
		/*for(int i=3;i<ncel;i++){
			erroPermanenteSW+=fabsl(celula[i].BSW-celula[2].BSW);
		}*/
		//erroPermanenteSW=fabsl(celula[ncel-2].BSW-celula[2].BSW);
		erroPermanente/=ncel;
		atualizaIni();
		kontaItera++;
	}
}

void PorosRadSimp::dVazdPw(){

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
	 celula[idisc].transcel(idisc);
	 for(int kLin=0;kLin<4;kLin++){
		 vetliv[kLin]=celula[idisc].localvet[kLin];
		 int ini;
		 if(kLin==0)ini=kLin;
		 else ini=kLin;
		 for(int kCol=ini;kCol<8+kLin;kCol++){
			 matglob[kLin][kCol-kLin-3]=celula[idisc].localmat[kLin][kCol];
		 }
	 }
	 konta=4;
	 while(idisc<ncel-1){
			 idisc++;
			 celula[idisc].transcel(idisc);
			 for(int kLin=0;kLin<4;kLin++){
				 vetliv[kLin+konta]=celula[idisc].localvet[kLin];
				 for(int kCol=kLin;kCol<8;kCol++){
					 matglob[kLin+konta][kCol-kLin-3]=celula[idisc].localmat[kLin][kCol];
				 }
			 }
			 konta+=4;
	 }
	 matglob.GaussElimPP(vetliv);


	 /*idisc=0;

	 celula[idisc].Pcamada=vetliv[0];
	 celula[idisc].PcamadaL=vetliv[0];
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
			 celula[i].Pcamada=vetliv[konta];
			 celula[i-1].PcamadaR=vetliv[konta];
			 celula[i].PcamadaL=celula[i-1].Pcamada;
			 double verifica;
			 verifica=vetliv[konta+1];

			 celula[i].QocamadaR=2.*M_PI*vetliv[konta+1];
			 celula[i].QocamadaL=celula[i-1].QocamadaR;
			 celula[i].QgcamadaR=2.*M_PI*vetliv[konta+2];
			 celula[i].QgcamadaL=celula[i-1].QgcamadaR;
			 celula[i].QwcamadaR=2.*M_PI*vetliv[konta+3];
			 celula[i].QwcamadaL=celula[i-1].QwcamadaR;
			 celula[i].QcamadaR=celula[i].QocamadaR+celula[i].QgcamadaR+celula[i].QwcamadaR;
			 celula[i].QcamadaL=celula[i-1].QcamadaR;
			 konta+=4;
	 }
	 celula[ncel-1].PcamadaR=celula[ncel-1].Pcamada;*/

  Pint-=DPint;
  double tit=flup.FracMassHidra(Pint+DPint, tRes);
  double rhogP=flup.MasEspGas(Pint+DPint, tRes);
  double rhoP=flup.MasEspoleo(Pint+DPint, tRes);
  double alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
  DfluxIni=(2.*M_PI*vetliv[1]*(1-alf)*rhoP*espessuraRes-
		    fluxIni)/DPint;
  DfluxIniG=(2.*M_PI*vetliv[1]*alf*rhogP*espessuraRes-
		     fluxIniG)/DPint;
  DfluxIniA=(2.*M_PI*vetliv[3]*celula[0].flup.MasEspAgua(Pint+DPint, tRes)*espessuraRes-
		     fluxIniA)/DPint;
}

void PorosRadSimp::solveTrans(){
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
	if((*vg1dSP).tempo>=43400){
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

	indraz(ind, raz,tempoSatAPoc,nsatAPoc);
	inf = satAPoc[ind];
	if (ind < nsatAPoc - 1){
		sup = satAPoc[ind + 1];
		sWPoc = (inf * raz + (1 - raz) * sup);
	}
	else
		sWPoc = satAPoc[ind];

	indraz(ind, raz,tempoPRes,nPRes);
	inf = pRes[ind];
	if (ind < nPRes - 1){
		sup = pRes[ind + 1];
		presRes = (inf * raz + (1 - raz) * sup);
	}
	else
		presRes = pRes[ind];

	for(int i=0; i<ncel;i++){
		celula[i].Pint=Pint;
		celula[i].presRes=presRes;
		celula[i].sLRes=sLRes;
		celula[i].sWRes=sWRes;
		celula[i].sWPoc=sWPoc;
	}

	int ciclo=0;
	reinicia=0;
	for(int i=0;i<ncel;i++){
		celula[i].dPdT=0.;
		//celula[i].calcVazOWG();
	}
	while(ciclo<1){
		for(int i=0;i<ncel;i++){
			/*if(i>=ncel-1){
				celula[i].sL=sLRes;
				celula[i-1].sLR=sLRes;
				celula[i].sW=sWRes;
				celula[i-1].sWR=sWRes;
				celula[i].sLR=sLRes;
				celula[i].sWR=sWRes;
				celula[i].reiniciaSL=0;
				celula[i].reiniciaSW=0;
			}*/
			if(i>=ncel-1 && celula[ncel-2].QcamadaL>0.){
				celula[i].sL=sLRes;
				celula[i-1].sLR=sLRes;
				celula[i].sW=sWRes;
				celula[i-1].sWR=sWRes;
				celula[i].sLR=sLRes;
				celula[i].sWR=sWRes;
				celula[i].reiniciaSL=0;
				celula[i].reiniciaSW=0;
			}
			else if(i==0 && celula[1].QcamadaR<0.){
				celula[i].sL=sLRes;
				//dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
				celula[i].sW=sWPoc;
				//dados.transfer.celula[i-1].sWR=dados.transfer.sWPoc;
				celula[i].sLR=sLRes;
				celula[i].sWR=sWPoc;
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
		dVazdPw();
		if(flashCompleto==2){
			for(int i=0;i<ncel;i++){
	 	        if(celula[i].flup.dCalculatedBeta<0. || celula[i].flup.dCalculatedBeta>1.)
	 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada,  tRes,-1,NULL,NULL,0);
	 	        else
	 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada, tRes,celula[i].flup.dCalculatedBeta,
	 	        			celula[i].flup.oCalculatedLiqComposition,
							celula[i].flup.oCalculatedVapComposition,0);
			}
		}
		/*if(ciclo==0){
			for(int i=0;i<ncel;i++)
				celula[i].dPdT=(celula[i].Pcamada-celula[i].Pini)/dt;
			FeiticoDoTempo();
		}*/
		ciclo++;
	}
	atualizaIni();
	(*vg1dSP).tempo+=dt;
}

void PorosRadSimp::avancoSW(double Ndt){


    dt=Ndt;
	for(int i=0;i<ncel;i++){
		  celula[i].dt=dt;
		  celula[i].dtSL=dt;
		  celula[i].dtSW=dt;
	}
	int ind=0.;
	double raz;
	double inf;
	double sup;

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

	indraz(ind, raz,tempoPRes,nPRes);
	inf = pRes[ind];
	if (ind < nPRes - 1){
		sup = pRes[ind + 1];
		presRes = (inf * raz + (1 - raz) * sup);
	}
	else
		presRes = pRes[ind];

	for(int i=0; i<ncel;i++){
		celula[i].Pint=Pint;
		celula[i].presRes=presRes;
		celula[i].sLRes=sLRes;
		celula[i].sWRes=sWRes;
		celula[i].sWPoc=sWPoc;
	}

	int ciclo=0;
	reinicia=0;
	for(int i=0;i<ncel;i++){
		celula[i].dPdT=0.;
	}
	for(int i=0;i<ncel;i++){
		if(i>=ncel-1 && celula[ncel-2].QcamadaL>0.){
			celula[i].sL=sLRes;
			celula[i-1].sLR=sLRes;
			celula[i].sW=sWRes;
			celula[i-1].sWR=sWRes;
			celula[i].sLR=sLRes;
			celula[i].sWR=sWRes;
			celula[i].reiniciaSL=0;
			celula[i].reiniciaSW=0;
		}
		else if(i==0 && celula[1].QcamadaR<0.){
			celula[i].sL=sLRes;
			celula[i].sW=sWPoc;
			celula[i].sLR=sLRes;
			celula[i].sWR=sWPoc;
			celula[i].reiniciaSL=0;
			celula[i].reiniciaSW=0;
		}
		else{
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
			 celula[i].reiniciaSW=0;
	}
}

void PorosRadSimp::reavaliaDT(double& Ndt){


    dt=Ndt;
	for(int i=0;i<ncel;i++){
		//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
		if(dt>celula[i].dtSW)dt=celula[i].dtSW;
	}
	Ndt=dt;
/*	for(int i=0;i<ncel;i++){
		celula[i].dt=dt;
		celula[i].dtSL=dt;
		celula[i].dtSW=dt;
	}*/
}

void PorosRadSimp::reiniciaEvoluiSW(double Ndt){
	dt=Ndt;
	for(int i=0;i<ncel;i++){
		celula[i].dt=dt;
		celula[i].dtSL=dt;
		celula[i].dtSW=dt;
	}
}

void PorosRadSimp::avancoSWcorrec(){
	int ciclo=0;
	for(int i=0;i<ncel;i++){

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

void PorosRadSimp::avancoPressao(){
	transtrans();
	BSW=celula[2].BSW;
	dVazdPw();
	if(flashCompleto==2){
		for(int i=0;i<ncel;i++){
 	        if(celula[i].flup.dCalculatedBeta<0. || celula[i].flup.dCalculatedBeta>1.)
 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada,  tRes,-1,NULL,NULL,0);
 	        else
 	        	celula[i].flup.atualizaPropComp(celula[i].Pcamada, tRes,celula[i].flup.dCalculatedBeta,
 	        			celula[i].flup.oCalculatedLiqComposition,
						celula[i].flup.oCalculatedVapComposition,0);
		}
	}
	atualizaIni();
	//(*vg1dSP).tempo+=dt;
	(*vg1dSP).tempo=(*vg1dSP).lixo5;

	  if(temp.tempoImp[kontaTempoImp]>=(*vg1dSP).tempo-dt && temp.tempoImp[kontaTempoImp]<=(*vg1dSP).tempo){
		  	  FullMtx<double> matrizsaida(nglobal,11);
		  	  matrizsaida=perfil();
		  	  ostringstream saida;
		  	  int numero = roundl((*vg1dSP).tempo);
		  	  //saida << pathPrefixoArqSaida << "PerfisPocoRadial" << "-" << kontaTempoImp
						//<<"-"<<posicMarlim<< ".dat";
		  	  saida << pathPrefixoArqSaida << "PerfisPocoRadial" << "-" << (*vg1dSP).tempo
		  							<<"-"<<posicMarlim<< ".dat";
		  	  string tmp = saida.str();
		  	  ofstream escreveIni(tmp.c_str(), ios_base::out);
		  	  escreveIni<<"tempo = "<< (*vg1dSP).tempo<<endl;
		  	  escreveIni<<" raio (m) ;"<< " raio (pol.) ;"<<" pressao (kgf/cm2) ;"<<" vazao total (sm3/d) ;"<<
		  			  " vazao de oleo (sm3/d) ;"<<" vazao de gas (sm3/d) ;"<<" vazao de agua (sm3/d) ;"<<
					  "saturacao de liquido (-) ;"<<" saturacao de agua (-) ;"<<" fracao volumetrica de gas homogeneo (-) ;"
					  <<" BSW (-) ;"<<endl;
		  	  escreveIni << matrizsaida;
		  	  escreveIni.close();
		  	  kontaTempoImp++;
	  }
}

void PorosRadSimp::FeiticoDoTempo(){
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

void PorosRadSimp::FeiticoDoTempoPQ(){
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
	 for(int i=0;i<ncel;i++)celula[i].FeiticoDoTempoPQ();
	 celula[0].PcamadaL=celula[0].Pcamada;
	 celula[0].QwcamadaL=celula[0].QwcamadaR;
	 celula[0].QocamadaL=celula[0].QocamadaR;
	 celula[0].QgcamadaL=celula[0].QgcamadaR;
	 for(int i=1;i<ncel;i++){
		 celula[i].PcamadaL=celula[i-1].Pcamada;
		 celula[i-1].PcamadaR=celula[i].Pcamada;
		 celula[i].QwcamadaL=celula[i-1].QwcamadaR;
		 celula[i].QocamadaL=celula[i-1].QocamadaR;
		 celula[i].QgcamadaL=celula[i-1].QgcamadaR;
	 }
	 celula[ncel-1].PcamadaR=celula[ncel-1].Pcamada;
}

void PorosRadSimp::FeiticoDoTempoSW(){


	   for(int j=0;j<ncel;j++){
		   celula[j].sW=celula[j].sWini;
	   }
	   celula[0].sWL=celula[0].sW;
	   for(int j=1;j<ncel;j++){
		   celula[j].sWL=celula[j-1].sW;
		   celula[j-1].sWR=celula[j].sW;
	   }
	   celula[ncel-1].sWR=celula[ncel-1].sW;
}

 FullMtx<double> PorosRadSimp::perfil(){
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
	 matrizsaida[0][3]=celula[idisc].QcamadaR*86400*espessuraRes;
	 matrizsaida[0][4]=celula[idisc].QocamadaR*(1-celula[idisc].alf)*86400*espessuraRes;
	 matrizsaida[0][5]=celula[idisc].QocamadaR*celula[idisc].alf*86400*espessuraRes;
	 matrizsaida[0][6]=celula[idisc].QwcamadaR*86400*espessuraRes;
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
	 			matrizsaida[konta][3]=celula[idisc].QcamadaR*86400*espessuraRes;
	 			matrizsaida[konta][4]=celula[idisc].QocamadaR*(1-celula[idisc].alf)*86400*espessuraRes;
	 			matrizsaida[konta][5]=celula[idisc].QocamadaR*celula[idisc].alf*86400*espessuraRes;
	 			matrizsaida[konta][6]=celula[idisc].QwcamadaR*86400*espessuraRes;
	 			matrizsaida[konta][7]=celula[idisc].sL;
	 			if(idisc>0 && idisc< ncel-1) matrizsaida[konta][8]=celula[idisc].sW;
	 			else if(idisc==0){
	 				if(celula[idisc+1].QcamadaR>0) matrizsaida[konta][8]=celula[idisc+1].sW;
	 				else matrizsaida[konta][8]=celula[idisc].sW;
	 			}
	 			else{
	 				if(celula[idisc-1].QcamadaR<0) matrizsaida[konta][8]=celula[idisc-1].sW;
	 				else matrizsaida[konta][8]=celula[idisc].sW;
	 			}
	 			matrizsaida[konta][9]=celula[idisc].alf;
	 			matrizsaida[konta][10]=celula[idisc].BSW;
	 			konta+=1;
	 	 }
	  }
	  return matrizsaida;
}

//template class PorosRadSimp;
