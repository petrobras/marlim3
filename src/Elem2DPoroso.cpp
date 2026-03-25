/*
 * elem2dPoroso.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Elem2DPoroso.h"



elem2dPoroso::elem2dPoroso(varGlob1D* Vvg1dSP,double** xcoor, int** noEle, int* tipo,double* atributo,int nVert, int nele, int nno,
		int vperm, int vtrans,int i,ProFlu vflup,tabelaPemRelOA vkRelOA,tabelaPemRelOG vkRelOG,
		tabelaPresCapOA vpcOA,tabelaPresCapGO vpcGO/*,double vfluxCal,double vdt,double vKX,
		double vKY,double vporo,double vcompresPoro,double vPcentro,double vPint,
		double vpresRes,double vQL, double vQR, double vtRes, double vzD,
		double vdh,double vsW,double vsWRes,
		double vsatConata,const double vip,tabelaPemRelOA vkRelOA,tabelaPemRelOG vkRelOG,
		tabelaPresCapOA vpcOA,tabelaPresCapGO vpcGO,
		ProFluCol vfluc, ProFlu vflup*/) :
				TL(1), local(1,nVert+1) {

	/*
	 * ,double vfluxCal=0.,double vdt=0.,double vKX=0.,
				double vKY=0.,double vporo=0.,double vcompresPoro=0.,double vPcentro=0,double vPint=0.,
				double vpresRes=0.,double vQL=0., double vQR=0, double vtRes=0,double vzD=0,
				double vdh=1,double vsW=0,double vsWRes=0,
				double vsatConata=0,const double vip=1.,tabelaPemRelOA vkRelOA=tabelaPemRelOA(),tabelaPemRelOG vkRelOG=tabelaPemRelOG(),
				tabelaPresCapOA vpcOA=tabelaPresCapOA(),tabelaPresCapGO vpcGO=tabelaPresCapGO(),
				ProFluCol vfluc=ProFluCol(), ProFlu vflup=ProFlu()
	 */

	vg1dSP=Vvg1dSP;

	double vfluxCal=0.;
	double vdt=0.;
	double vKX=0.;
	double vKY=0.;
	double vporo=0.;
	double vcompresPoro=0.;
	double vPcentro=0;
	double vPint=0.;
	double vpresRes=0.;
	double vQL=0.;
	double vQR=0.;
	double vtRes=0;
	double vzD=0;
	double vdh=1;
	double vsW=0;
	double vsWRes=0;
	double vsatConata=0;
	const double vip=1.;
	//tabelaPemRelOA vkRelOA=tabelaPemRelOA();
	//tabelaPemRelOG vkRelOG=tabelaPemRelOG();
	//tabelaPresCapOA vpcOA=tabelaPresCapOA();
	//tabelaPresCapGO vpcGO=tabelaPresCapGO();
	ProFluCol vfluc=ProFluCol();
	//ProFlu vflup=ProFlu();




	vizinho=0;
	kvizinho=0;
	nvizinho=0;
	cel2D.presC=vPcentro;
	cel2D.presC0=vPcentro;
	cel2D.presCI=vPcentro;
	cel2D.sLC=1.;
	cel2D.sLI=1.;
	cel2D.sWC=vsW;
	cel2D.sWI=vsW;
	dh=vdh;
	deriFonteT=0.;
	FonteT=0.;
	fluxCal=vfluxCal;
	cel2D.kX=vKX;
	cel2D.kY=vKY;
	cel2D.kabsol=sqrtl(vKX*vKY);
	cel2D.poro=vporo;
	cel2D.compRoc=vcompresPoro;
	cel2D.zD=vzD;

	cel2D.pBolha=1000.;
	cel2D.tipoModelOleo=1;
	cel2D.compOleo=1e-5;
	cel2D.rhoRef=900;

	satConata=vsatConata;
	ip=vip;
	tRes=vtRes;
	sLRes=1.;
	sWRes=vsWRes;
	pRes=vpresRes;

	dt=vdt;
    dtSL=vdt;
    dtSW=vdt;
	reiniciaSL=0;
	reiniciaSW=0;

	perm=vperm;
	trans=vtrans;
	ccPD=0;
	ccSat=0;
	ccPVN=0;
	ccHR=0;
	ccPambR=0;
	fluc=vfluc;
	flup=vflup;
	kRelOACel=vkRelOA;
	kRelOGCel=vkRelOG;
	pcOACel=vpcOA;
	pcGOCel=vpcGO;
	CC=detCCPoroso();
	if(nele>0){

		if((cel2D.presC+fabsl(tRes)+flup.API)>1.){
			cel2D.vbo=flup.BOFunc(cel2D.presC, tRes);
			cel2D.rhostd=1000.*141.5/(131.5+flup.API);
			cel2D.rhogstd=flup.MasEspGas(1.033, 20.);
			cel2D.rhoastd=flup.MasEspAgua(1.033, 20.);
			cel2D.rhoP=flup.MasEspoleo(cel2D.presC, tRes);
			cel2D.rhogP=flup.MasEspGas(cel2D.presC, tRes);
			cel2D.rhoaP=flup.MasEspAgua(cel2D.presC, tRes);
			double tit=flup.FracMassHidra(cel2D.presC, tRes);
			cel2D.alfC=(tit/cel2D.rhogP)/((tit/cel2D.rhogP)+((1.-tit)/cel2D.rhoP));

			cel2D.rhoP0=cel2D.rhoP;
			cel2D.rhogP0=cel2D.rhogP;
			cel2D.rhoaP0=cel2D.rhoaP;
			cel2D.alfC0=cel2D.alfC;
		}
		else{
			cel2D.vbo=1.;
			cel2D.rhostd=1.;
			cel2D.rhogstd=1.;
			cel2D.rhoastd=1.;
			cel2D.rhoP=1.;
			cel2D.rhogP=1.;
			cel2D.rhoaP=1.;
			cel2D.alfC=0.;

			cel2D.rhoP0=cel2D.rhoP;
			cel2D.rhogP0=cel2D.rhogP;
			cel2D.rhoaP0=cel2D.rhoaP;
			cel2D.alfC0=cel2D.alfC;
		}


		cel2D.noFace=0;
    	cel2D.dim=2;
    	cel2D.nvert=3;
    	cel2D.indEle=i;
    	cel2D.nele=nele;

    	presF=new double [cel2D.nvert];
    	swF=new double [cel2D.nvert];
    	alfF=new double [cel2D.nvert];
    	Qtotal=new double [cel2D.nvert];
    	Qw=new double [cel2D.nvert];
    	Qoleo=new double [cel2D.nvert];
    	Qgas=new double [cel2D.nvert];
    	rhoF=new double [cel2D.nvert];
    	rhogF=new double [cel2D.nvert];
    	rhoaF=new double [cel2D.nvert];
    	rhoZF=new double [cel2D.nvert];
    	rhoaZF=new double [cel2D.nvert];
    	mioF=new double [cel2D.nvert];
    	migF=new double [cel2D.nvert];
    	miaF=new double [cel2D.nvert];
    	kmedF=new double [cel2D.nvert];
    	kmedGF=new double [cel2D.nvert];
    	kmedAF=new double [cel2D.nvert];
    	darcyOF=new double [cel2D.nvert];
    	darcyGF=new double [cel2D.nvert];
    	darcyAF=new double [cel2D.nvert];
    	pcAOF=new double [cel2D.nvert];

    	ccPD=new double [cel2D.nvert];
    	ccSat=new double [cel2D.nvert];
    	ccPVN=new double [cel2D.nvert];
    	ccHR=new double [cel2D.nvert];
    	ccPambR=new double [cel2D.nvert];

    	gradPface=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) gradPface[j]=new double [cel2D.dim];
    	gradZface=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) gradZface[j]=new double [cel2D.dim];
    	gradPcOGface=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) gradPcOGface[j]=new double [cel2D.dim];
    	gradPcAOface=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) gradPcAOface[j]=new double [cel2D.dim];

    	cel2D.gradGreenP=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenP[j]=0.;
    	cel2D.gradGreenPI=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPI[j]=0.;
    	cel2D.gradGreenZdatum=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenZdatum[j]=0.;
    	cel2D.gradGreenZdatumI=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenZdatumI[j]=0.;
    	cel2D.gradGreenAZdatum=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenAZdatum[j]=0.;
    	cel2D.gradGreenAZdatumI=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenAZdatumI[j]=0.;
    	cel2D.gradGreenPcOG=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPcOG[j]=0.;
    	cel2D.gradGreenPcAO=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPcAO[j]=0.;
    	cel2D.gradGreenPcAOI=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPcAOI[j]=0.;
    	cel2D.gradGreenSw=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenSw[j]=0.;
    	cel2D.gradGreenSwI=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenSwI[j]=0.;
    	coefTHRC=new double [cel2D.nvert];
    	coefTHRV=new double [cel2D.nvert];
    	fonteTHR=new double [cel2D.nvert];
    	cel2D.noElem=new int [cel2D.nvert];
    	cel2D.indFace=new int [cel2D.nvert];
    	cel2D.ccFace=new int [cel2D.nvert];
    	cel2D.orientFace=new double [cel2D.nvert];
    	cel2D.ownFace=new double [cel2D.nvert];
    	cel2D.centroElem=new double [cel2D.dim];
    	cel2D.centroideElem=new double [cel2D.dim];
    	cel2D.centroideFace=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.centroideFace[j]=new double [cel2D.dim];
    	cel2D.coordVert=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.coordVert[j]=new double [cel2D.dim];
    	cel2D.dCF=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.dCF[j]=new double [cel2D.dim];
    	cel2D.dCFMod=new double [cel2D.nvert];
    	cel2D.sFace=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.sFace[j]=new double [cel2D.dim];
    	cel2D.sFaceMod=new double [cel2D.nvert];

    	cel2D.vecSDif=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.vecSDif[j]=new double [cel2D.dim];
    	cel2D.vecSDifMod=new double [cel2D.nvert];

    	///////////////////////////////////////////////////////////////////////////////////////////
        cel2D.vecE=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.vecE[j]=new double [cel2D.dim];
        cel2D.modE=new double [cel2D.nvert];
        cel2D.vecT=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.vecT[j]=new double [cel2D.dim];
        cel2D.modT=new double [cel2D.nvert];
        cel2D.fatG=new double [cel2D.nvert];
        cel2D.angES=new double [cel2D.nvert];
        cel2D.fInter=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.fInter[j]=new double [cel2D.dim];
        cel2D.fIfC=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.fIfC[j]=new double [cel2D.dim];

    	vizinho=new elementoPoroso* [cel2D.nvert];
    	kvizinho=new int [cel2D.nvert];
		for(int k=0; k<cel2D.nvert;k++){
			kvizinho[k]=-1;
			vizinho[k]=0;
		}


    	buscaVizinho(noEle,cel2D.indFace,i,cel2D.nvert, nele);
    	for(int j=0; j<cel2D.nvert;j++){
    		if(i>cel2D.indFace[j] && cel2D.indFace[j]>=0)cel2D.ownFace[j]=-1.;
    		else cel2D.ownFace[j]=1.;
    	}

    	menorIndViz();

    	for(int j=0; j<cel2D.nvert;j++){
    		cel2D.noElem[j]=noEle[i][j];
    		int iNo=cel2D.noElem[j];
    		if(j+1<cel2D.nvert){
    			//int iNoP=cel2D.noElem[j+1];
    			int iNoP=noEle[i][j+1];
    			if(cel2D.indFace[j]>=0)cel2D.ccFace[j]=0;
    			else if(tipo[iNo]==tipo[iNoP])cel2D.ccFace[j]=tipo[iNo];
    			else if(tipo[iNo]==0 || tipo[iNoP]==0)cel2D.ccFace[j]=0;
    			else if(fabsl(atributo[iNo]-1.)<1e-15)cel2D.ccFace[j]=tipo[iNoP];
    			else if(fabsl(atributo[iNoP]-1.)<1e-15)cel2D.ccFace[j]=tipo[iNo];
    			else cel2D.ccFace[j]=0;
    		}
    		else{
    			int iNoM=cel2D.noElem[0];
    			if(cel2D.indFace[j]>=0)cel2D.ccFace[j]=0;
    			else if(tipo[iNo]==tipo[iNoM])cel2D.ccFace[j]=tipo[iNo];
    			else if(tipo[iNo]==0 || tipo[iNoM]==0)cel2D.ccFace[j]=0;
    			else if(fabsl(atributo[iNo]-1.)<1e-15)cel2D.ccFace[j]=tipo[iNoM];
    			else if(fabsl(atributo[iNoM]-1.)<1e-15)cel2D.ccFace[j]=tipo[iNo];
    			else cel2D.ccFace[j]=0;
    		}
    		for(int k=0; k<cel2D.dim; k++){
    			cel2D.coordVert[j][k]=xcoor[iNo][k];
    		}

    	}
    	for(int j=0; j<cel2D.dim;j++){
    		cel2D.centroElem[j]=0.;
    		for(int k=0; k<cel2D.nvert; k++){
    			cel2D.centroElem[j]+=cel2D.coordVert[k][j];
    		}
    		cel2D.centroElem[j]/=cel2D.nvert;
    	}
    	if(cel2D.dim==2 && cel2D.nvert==3){
    		for(int j=0; j<cel2D.dim;j++)cel2D.centroideElem[j]=cel2D.centroElem[j];
    		cel2D.vElem=0.5*fabsl((cel2D.coordVert[1][0]-cel2D.coordVert[0][0])*
					(cel2D.coordVert[2][1]-cel2D.coordVert[0][1])-(cel2D.coordVert[2][0]-cel2D.coordVert[0][0])*
					(cel2D.coordVert[1][1]-cel2D.coordVert[0][1]));
    	}
    	else{
    		double sT[cel2D.nvert];
    		double centroT[cel2D.nvert][2];
    		cel2D.vElem=0.;
    		for(int k=0; k<cel2D.nvert; k++){
    			if(k<cel2D.nvert-1){
    				sT[k]=0.5*fabsl((cel2D.coordVert[k][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k+1][1]-cel2D.centroElem[1])-(cel2D.coordVert[k+1][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k][1]-cel2D.centroElem[1]));
    				centroT[k][0]=(cel2D.coordVert[k][0]+cel2D.coordVert[k+1][0]+cel2D.centroElem[0])/3.;
    				centroT[k][1]=(cel2D.coordVert[k][1]+cel2D.coordVert[k+1][1]+cel2D.centroElem[1])/3.;
    			}
    			else{
    				sT[k]=0.5*fabsl((cel2D.coordVert[k][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[0][1]-cel2D.centroElem[1])-(cel2D.coordVert[0][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k][1]-cel2D.centroElem[1]));
    				centroT[k][0]=(cel2D.coordVert[k][0]+cel2D.coordVert[0][0]+cel2D.centroElem[0])/3.;
    				centroT[k][1]=(cel2D.coordVert[k][1]+cel2D.coordVert[0][1]+cel2D.centroElem[1])/3.;
    			}
    			cel2D.vElem+=sT[k];
    		}
        	for(int j=0; j<cel2D.dim;j++){
        		cel2D.centroideElem[j]=0.;
        		for(int k=0; k<cel2D.nvert; k++){
        			cel2D.centroideElem[j]+=centroT[k][j]*sT[k];
        		}
        		cel2D.centroideElem[j]/=cel2D.vElem;
        	}
    	}
    	for(int k=0; k<cel2D.nvert; k++){
    		double val;
    		if(k<cel2D.nvert-1){
    			val=(cel2D.coordVert[k][0]-cel2D.centroideElem[0])*
    					(cel2D.coordVert[k+1][1]-cel2D.coordVert[k][1])-(cel2D.coordVert[k+1][0]-cel2D.coordVert[k][0])*
    					(cel2D.coordVert[k][1]-cel2D.centroideElem[1]);
    			if(val>0)cel2D.orientFace[k]=1.;
    			else cel2D.orientFace[k]=-1.;
    			cel2D.sFace[k][0]=cel2D.orientFace[k]*(cel2D.coordVert[k+1][1]-cel2D.coordVert[k][1]);
    			cel2D.sFace[k][1]=-cel2D.orientFace[k]*(cel2D.coordVert[k+1][0]-cel2D.coordVert[k][0]);
    		}
    		else{
    			val=(cel2D.coordVert[k][0]-cel2D.centroideElem[0])*
    					(cel2D.coordVert[0][1]-cel2D.coordVert[k][1])-(cel2D.coordVert[0][0]-cel2D.coordVert[k][0])*
    					(cel2D.coordVert[k][1]-cel2D.centroideElem[1]);
    			if(val>0)cel2D.orientFace[k]=1.;
    			else cel2D.orientFace[k]=-1.;
    			cel2D.sFace[k][0]=cel2D.orientFace[k]*(cel2D.coordVert[0][1]-cel2D.coordVert[k][1]);
    			cel2D.sFace[k][1]=-cel2D.orientFace[k]*(cel2D.coordVert[0][0]-cel2D.coordVert[k][0]);
    		}

    		cel2D.vecSDif[k][0]=cel2D.sFace[k][0];
    		cel2D.vecSDif[k][1]=cel2D.sFace[k][1];
    	}
    	for(int k=0; k<cel2D.nvert; k++){
    		cel2D.sFaceMod[k]=0.;
    		for(int j=0; j<cel2D.dim;j++)cel2D.sFaceMod[k]+=cel2D.sFace[k][j]*cel2D.sFace[k][j];
    		cel2D.sFaceMod[k]=sqrtl(cel2D.sFaceMod[k]);
    	}
    	for(int k=0; k<cel2D.nvert; k++){
    		if(k<cel2D.nvert-1){
    			for(int j=0; j<cel2D.dim;j++)cel2D.centroideFace[k][j]=0.5*(cel2D.coordVert[k+1][j]+cel2D.coordVert[k][j]);
    		}
    		else{
    			for(int j=0; j<cel2D.dim;j++)cel2D.centroideFace[k][j]=0.5*(cel2D.coordVert[0][j]+cel2D.coordVert[k][j]);
    		}
    	}
    	for(int k=0; k<cel2D.nvert; k++){
    		for(int j=0; j<cel2D.dim;j++)cel2D.dCF[k][j]=cel2D.centroideFace[k][j]-cel2D.centroideElem[j];
    		cel2D.dCFMod[k]=0.;
    		for(int j=0; j<cel2D.dim;j++)cel2D.dCFMod[k]+=cel2D.dCF[k][j]*cel2D.dCF[k][j];
    		cel2D.dCFMod[k]=sqrtl(cel2D.dCFMod[k]);
    	}
	}
	else{
		cel2D.nvert=0;
		cel2D.dim=0;
		cel2D.indEle=0;
		cel2D.nele=0;
		cel2D.noElem=0;
		cel2D.indFace=0;
		cel2D.ccFace=0;
		cel2D.orientFace=0;
		cel2D.centroElem=0;
		cel2D.centroideElem=0;
		cel2D.centroideFace=0;
		cel2D.coordVert=0;
		cel2D.dCF=0;
		cel2D.dCFMod=0;
		cel2D.vElem=0;
		cel2D.noFace=0;
		cel2D.sFace=0;
		cel2D.sFaceMod=0;
		cel2D.ownFace=0;
		cel2D.gradGreenP=0;
		cel2D.gradGreenPI=0;
		cel2D.gradGreenZdatum=0;
		cel2D.gradGreenZdatumI=0;
		cel2D.gradGreenAZdatum=0;
		cel2D.gradGreenAZdatumI=0;
		cel2D.gradGreenPcOG=0;
		cel2D.gradGreenPcAO=0;
		cel2D.gradGreenPcAOI=0;
		cel2D.gradGreenSw=0;
		cel2D.gradGreenSwI=0;
		presF=0;
		swF=0;
		alfF=0;
		Qtotal=0;
		Qw=0;
		Qoleo=0;
		Qgas=0;
		rhoF=0;
		rhogF=0;
		rhoaF=0;
    	rhoZF=0;
    	rhoaZF=0;
		mioF=0;
		migF=0;
		miaF=0;
		kmedF=0;
		kmedGF=0;
		kmedAF=0;
		darcyOF=0;
		darcyGF=0;
		darcyAF=0;
		pcAOF=0;
		gradPface=0;
		gradZface=0;
		gradPcOGface=0;
		gradPcAOface=0;
    	ccPD=0;
    	ccSat=0;
    	ccPVN=0;
    	ccHR=0;
    	ccPambR=0;
    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;

		cel2D.alfC=0;
		cel2D.vbo=0;
		cel2D.rhostd=0;
		cel2D.rhogstd=0;
		cel2D.rhoastd=0;
		cel2D.rhoP=0;
		cel2D.rhogP=0;
		cel2D.rhoaP=0;

		cel2D.rhoP0=cel2D.rhoP;
		cel2D.rhogP0=cel2D.rhogP;
		cel2D.rhoaP0=cel2D.rhoaP;
		cel2D.alfC0=cel2D.alfC;

		cel2D.vecSDif=0;
		cel2D.vecSDifMod=0;

    	vizinho=0;
    	kvizinho=0;

    	kRelOACel=tabelaPemRelOA();
    	kRelOGCel=tabelaPemRelOG();
    	pcOACel=tabelaPresCapOA();
    	pcGOCel=tabelaPresCapGO();
    	fluc=ProFluCol();
    	flup=ProFlu();
	}
}
elem2dPoroso::elem2dPoroso(const elem2dPoroso& velem) :
						TL(1), local(1,velem.cel2D.nvert+1) {
	vg1dSP=velem.vg1dSP;
	cel2D.nvert=velem.cel2D.nvert;
	cel2D.dim=velem.cel2D.dim;
	cel2D.indEle=velem.cel2D.indEle;
	cel2D.nele=velem.cel2D.nele;
	nvizinho=velem.nvizinho;
	cel2D.presC=velem.cel2D.presC;
	cel2D.presC0=velem.cel2D.presC0;
	cel2D.presCI=velem.cel2D.presCI;
	cel2D.sLC=velem.cel2D.sLC;
	cel2D.sLI=velem.cel2D.sLI;
	cel2D.sWC=velem.cel2D.sWC;
	cel2D.sWI=velem.cel2D.sWI;
	dh=velem.dh;
	deriFonteT=velem.deriFonteT;
	FonteT=velem.FonteT;
	fluxCal=velem.fluxCal;
	cel2D.kX=velem.cel2D.kX;
	cel2D.kY=velem.cel2D.kY;
	cel2D.kabsol=velem.cel2D.kabsol;
	cel2D.poro=velem.cel2D.poro;
	cel2D.compRoc=velem.cel2D.compRoc;
	cel2D.zD=velem.cel2D.zD;

	cel2D.pBolha=velem.cel2D.pBolha;
	cel2D.tipoModelOleo=velem.cel2D.tipoModelOleo;
	cel2D.compOleo=velem.cel2D.compOleo;
	cel2D.rhoRef=velem.cel2D.rhoRef;

	satConata=velem.satConata;
	ip=velem.ip;
	tRes=velem.tRes;
	sLRes=velem.sLRes;
	sWRes=velem.sWRes;
	pRes=velem.pRes;

	dt=velem.dt;
    dtSL=velem.dtSL;
    dtSW=velem.dtSW;
	reiniciaSL=velem.reiniciaSL;
	reiniciaSW=velem.reiniciaSW;
	deriFonteT=velem.deriFonteT;
	FonteT=velem.FonteT;
	fluxCal=velem.fluxCal;

	perm=velem.perm;
	trans=velem.trans;
	fluc=velem.fluc;
	flup=velem.flup;
	kRelOACel=velem.kRelOACel;
	kRelOGCel=velem.kRelOGCel;
	pcOACel=velem.pcOACel;
	pcGOCel=velem.pcGOCel;
	CC=velem.CC;
	//ccTD=velem.ccTD;
	//ccTVN=velem.ccTVN;
	//ccHR=velem.ccHR;
	//ccTambR=velem.ccTambR;
	if(cel2D.nvert>0){
    	vizinho=new elementoPoroso* [cel2D.nvert];
    	kvizinho=new int [cel2D.nvert];
		for(int k=0; k<cel2D.nvert;k++){
			kvizinho[k]=velem.kvizinho[k];
			vizinho[k]=velem.vizinho[k];
		}
	}
	else{
		vizinho=0;
		kvizinho=0;
	}
	if(cel2D.nele>0){
		TL=velem.TL;
		local=velem.local;
		cel2D.noFace=0;

		cel2D.alfC;
		cel2D.vbo=velem.cel2D.vbo;
		cel2D.rhostd=velem.cel2D.rhogstd;
		cel2D.rhogstd=velem.cel2D.rhogstd;
		cel2D.rhoastd=velem.cel2D.rhoastd;
		cel2D.rhoP=velem.cel2D.rhoP;
		cel2D.rhogP=velem.cel2D.rhogP;
		cel2D.rhoaP=velem.cel2D.rhoaP;

		cel2D.rhoP0=velem.cel2D.rhoP0;
		cel2D.rhogP0=velem.cel2D.rhogP0;
		cel2D.rhoaP0=velem.cel2D.rhoaP0;
		cel2D.alfC0=velem.cel2D.alfC0;

    	presF=new double [cel2D.nvert];
    	swF=new double [cel2D.nvert];
    	alfF=new double [cel2D.nvert];
    	Qtotal=new double [cel2D.nvert];
    	Qw=new double [cel2D.nvert];
    	Qoleo=new double [cel2D.nvert];
    	Qgas=new double [cel2D.nvert];
    	rhoF=new double [cel2D.nvert];
    	rhogF=new double [cel2D.nvert];
    	rhoaF=new double [cel2D.nvert];
    	rhoZF=new double [cel2D.nvert];
    	rhoaZF=new double [cel2D.nvert];
    	mioF=new double [cel2D.nvert];
    	migF=new double [cel2D.nvert];
    	miaF=new double [cel2D.nvert];
    	kmedF=new double [cel2D.nvert];
    	kmedGF=new double [cel2D.nvert];
    	kmedAF=new double [cel2D.nvert];
    	darcyOF=new double [cel2D.nvert];
    	darcyGF=new double [cel2D.nvert];
    	darcyAF=new double [cel2D.nvert];
    	pcAOF=new double [cel2D.nvert];
    	ccPD=new double [cel2D.nvert];
    	ccSat=new double [cel2D.nvert];
    	ccPVN=new double [cel2D.nvert];
    	ccHR=new double [cel2D.nvert];
    	ccPambR=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++){
        	ccPD[j]=-1;
        	ccSat[j]=-1;
        	ccPVN[j]=-1;
        	ccHR[j]=-1;
        	ccPambR[j]=-1;
    	}

    	gradPface=new double* [cel2D.nvert];
    	gradZface=new double* [cel2D.nvert];
    	gradPcOGface=new double* [cel2D.nvert];
    	gradPcAOface=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) gradPface[j]=new double [cel2D.dim];
    	for(int j=0; j<cel2D.nvert;j++) gradZface[j]=new double [cel2D.dim];
    	for(int j=0; j<cel2D.nvert;j++) gradPcOGface[j]=new double [cel2D.dim];
    	for(int j=0; j<cel2D.nvert;j++) gradPcAOface[j]=new double [cel2D.dim];

    	cel2D.gradGreenP=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenP[j]=velem.cel2D.gradGreenP[j];
    	cel2D.gradGreenPI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPI[j]=velem.cel2D.gradGreenPI[j];
    	cel2D.gradGreenZdatum=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenZdatum[j]=velem.cel2D.gradGreenZdatum[j];
    	cel2D.gradGreenZdatumI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenZdatumI[j]=velem.cel2D.gradGreenZdatumI[j];
    	cel2D.gradGreenAZdatum=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenAZdatum[j]=velem.cel2D.gradGreenAZdatum[j];
    	cel2D.gradGreenAZdatumI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenAZdatumI[j]=velem.cel2D.gradGreenAZdatumI[j];
    	cel2D.gradGreenPcOG=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPcOG[j]=velem.cel2D.gradGreenPcOG[j];
    	cel2D.gradGreenPcAO=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPcAO[j]=velem.cel2D.gradGreenPcAO[j];
    	cel2D.gradGreenPcAOI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPcAOI[j]=velem.cel2D.gradGreenPcAOI[j];
    	cel2D.gradGreenSw=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenSw[j]=velem.cel2D.gradGreenSw[j];
    	cel2D.gradGreenSwI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenSwI[j]=velem.cel2D.gradGreenSwI[j];

    	cel2D.vecSDif=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.vecSDif[j]=new double [cel2D.dim];
    	cel2D.vecSDifMod=new double [cel2D.nvert];

    	for(int i=0; i<cel2D.nvert;i++){
        	presF[i]=velem.presF[i];
        	swF[i]=velem.swF[i];
        	alfF[i]=velem.alfF[i];
        	Qtotal[i]=velem.Qtotal[i];
        	Qw[i]=velem.Qw[i];
        	Qoleo[i]=velem.Qoleo[i];
        	Qgas[i]=velem.Qgas[i];
        	rhoF[i]=velem.rhoF[i];
        	rhogF[i]=velem.rhogF[i];
        	rhoaF[i]=velem.rhoaF[i];
        	rhoZF[i]=velem.rhoZF[i];
        	rhoaZF[i]=velem.rhoaZF[i];
        	mioF[i]=velem.mioF[i];
        	migF[i]=velem.migF[i];
        	miaF[i]=velem.miaF[i];
        	kmedF[i]=velem.kmedF[i];
        	kmedGF[i]=velem.kmedGF[i];
        	kmedAF[i]=velem.kmedAF[i];
        	darcyOF[i]=velem.darcyOF[i];
        	darcyGF[i]=velem.darcyGF[i];
        	darcyAF[i]=velem.darcyAF[i];
        	pcAOF[i]=velem.pcAOF[i];
        	ccPD[i]=velem.ccPD[i];
        	ccSat[i]=velem.ccSat[i];
        	ccPVN[i]=velem.ccPVN[i];
        	ccHR[i]=velem.ccHR[i];
        	ccPambR[i]=velem.ccPambR[i];
        	for(int j=0;j<cel2D.dim;j++) gradPface[i][j]=velem.gradPface[i][j];
        	for(int j=0;j<cel2D.dim;j++) gradZface[i][j]=velem.gradZface[i][j];
        	for(int j=0;j<cel2D.dim;j++) gradPcOGface[i][j]=velem.gradPcOGface[i][j];
        	for(int j=0;j<cel2D.dim;j++) gradPcAOface[i][j]=velem.gradPcAOface[i][j];

        	for(int j=0;j<cel2D.dim;j++) cel2D.vecSDif[i][j]=velem.cel2D.vecSDif[i][j];

    	}

    	for(int j=0;j<cel2D.nvert;j++) cel2D.vecSDifMod[j]=velem.cel2D.vecSDifMod[j];

    	coefTHRC=new double [cel2D.nvert];
    	coefTHRV=new double [cel2D.nvert];
    	fonteTHR=new double [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++){
    		coefTHRC[i]=velem.coefTHRC[i];
    		coefTHRV[i]=velem.coefTHRV[i];
    		fonteTHR[i]=velem.fonteTHR[i];
    	}
    	cel2D.noElem=new int [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.noElem[i]=velem.cel2D.noElem[i];
    	cel2D.indFace=new int [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.indFace[i]=velem.cel2D.indFace[i];
    	int size=velem.cel2D.indVizCres.size();
    	for(int i=0; i<size;i++)cel2D.indVizCres.push_back(velem.cel2D.indVizCres[i]);
    	cel2D.ccFace=new int [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.ccFace[i]=velem.cel2D.ccFace[i];
    	cel2D.orientFace=new double [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.orientFace[i]=velem.cel2D.orientFace[i];
    	cel2D.ownFace=new double [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.ownFace[i]=velem.cel2D.ownFace[i];
    	cel2D.centroElem=new double [cel2D.dim];
    	for(int i=0; i<cel2D.dim;i++)cel2D.centroElem[i]=velem.cel2D.centroElem[i];
    	cel2D.centroideElem=new double [cel2D.dim];
    	for(int i=0; i<cel2D.dim;i++)cel2D.centroideElem[i]=velem.cel2D.centroideElem[i];
    	cel2D.centroideFace=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.centroideFace[j]=new double [cel2D.dim];
    	for(int i=0; i<cel2D.nvert;i++)
    		for(int j=0; j<cel2D.dim;j++)cel2D.centroideFace[i][j]=velem.cel2D.centroideFace[i][j];
    	cel2D.coordVert=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.coordVert[j]=new double [cel2D.dim];
    	for(int i=0; i<cel2D.nvert;i++)
    		for(int j=0; j<cel2D.dim;j++)cel2D.coordVert[i][j]=velem.cel2D.coordVert[i][j];
    	cel2D.dCF=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.dCF[j]=new double [cel2D.dim];
       	for(int i=0; i<cel2D.nvert;i++)
        	for(int j=0; j<cel2D.dim;j++)cel2D.dCF[i][j]=velem.cel2D.dCF[i][j];
    	cel2D.dCFMod=new double [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.dCFMod[i]=velem.cel2D.dCFMod[i];
    	cel2D.sFace=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.sFace[j]=new double [cel2D.dim];
       	for(int i=0; i<cel2D.nvert;i++)
        	for(int j=0; j<cel2D.dim;j++)cel2D.sFace[i][j]=velem.cel2D.sFace[i][j];
    	cel2D.sFaceMod=new double [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.sFaceMod[i]=velem.cel2D.sFaceMod[i];

    	///////////////////////////////////////////////////////////////////////////////////////////
        cel2D.vecE=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.vecE[j]=new double [cel2D.dim];
    	for(int i=0; i<cel2D.nvert;i++)
    		for(int j=0; j<cel2D.dim;j++)cel2D.vecE[i][j]=velem.cel2D.vecE[i][j];
        cel2D.modE=new double [cel2D.nvert];
        for(int i=0; i<cel2D.nvert;i++)cel2D.modE[i]=velem.cel2D.modE[i];
        cel2D.vecT=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.vecT[j]=new double [cel2D.dim];
    	for(int i=0; i<cel2D.nvert;i++)
    		for(int j=0; j<cel2D.dim;j++)cel2D.vecT[i][j]=velem.cel2D.vecT[i][j];
        cel2D.modT=new double [cel2D.nvert];
        for(int i=0; i<cel2D.nvert;i++)cel2D.modT[i]=velem.cel2D.modT[i];
        cel2D.fatG=new double [cel2D.nvert];
        for(int i=0; i<cel2D.nvert;i++)cel2D.fatG[i]=velem.cel2D.fatG[i];
        cel2D.angES=new double [cel2D.nvert];
        for(int i=0; i<cel2D.nvert;i++)cel2D.angES[i]=velem.cel2D.angES[i];
        cel2D.fInter=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.fInter[j]=new double [cel2D.dim];
    	for(int i=0; i<cel2D.nvert;i++)
    		for(int j=0; j<cel2D.dim;j++)cel2D.fInter[i][j]=velem.cel2D.fInter[i][j];
        cel2D.fIfC=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++) cel2D.fIfC[j]=new double [cel2D.dim];
    	for(int i=0; i<cel2D.nvert;i++)
    		for(int j=0; j<cel2D.dim;j++)cel2D.fIfC[i][j]=velem.cel2D.fIfC[i][j];
    	cel2D.vElem=velem.cel2D.vElem;
	}
	else{
		cel2D.nvert=0;
		cel2D.dim=0;
		cel2D.indEle=0;
		cel2D.nele=0;
		cel2D.noElem=0;
		cel2D.indFace=0;
		cel2D.ccFace=0;
		cel2D.orientFace=0;
		cel2D.centroElem=0;
		cel2D.centroideElem=0;
		cel2D.centroideFace=0;
		cel2D.coordVert=0;
		cel2D.dCF=0;
		cel2D.dCFMod=0;
		cel2D.vElem=0;
		cel2D.noFace=0;
		cel2D.sFace=0;
		cel2D.sFaceMod=0;
		cel2D.ownFace=0;

		cel2D.gradGreenP=0;
		cel2D.gradGreenPI=0;
		cel2D.gradGreenZdatum=0;
		cel2D.gradGreenZdatumI=0;
		cel2D.gradGreenAZdatum=0;
		cel2D.gradGreenAZdatumI=0;
		cel2D.gradGreenPcOG=0;
		cel2D.gradGreenPcAO=0;
		cel2D.gradGreenPcAOI=0;
		cel2D.gradGreenSw=0;
		cel2D.gradGreenSwI=0;

		presF=0;
		swF=0;
		alfF=0;
		Qtotal=0;
		Qw=0;
		Qoleo=0;
		Qgas=0;
		rhoF=0;
		rhogF=0;
		rhoaF=0;
		rhoZF=0;
		rhoaZF=0;
		mioF=0;
		migF=0;
		miaF=0;
		kmedF=0;
		kmedGF=0;
		kmedAF=0;
		pcAOF=0;
		darcyOF=0;
		darcyGF=0;
		darcyAF=0;

		gradPface=0;
		gradZface=0;
		gradPcOGface=0;
		gradPcAOface=0;

    	ccPD=0;
    	ccSat=0;
    	ccPVN=0;
    	ccHR=0;
		coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;

		cel2D.alfC=0;
		cel2D.vbo=0;
		cel2D.rhostd=0;
		cel2D.rhogstd=0;
		cel2D.rhoastd=0;
		cel2D.rhoP=0;
		cel2D.rhogP=0;
		cel2D.rhoaP=0;

		cel2D.rhoP0=0;
		cel2D.rhogP0=0;
		cel2D.rhoaP0=0;
		cel2D.alfC0=0;

    	cel2D.vecSDif=0;
    	cel2D.vecSDifMod=0;

    	vizinho=0;
    	kvizinho=0;

    	kRelOACel=tabelaPemRelOA();
    	kRelOGCel=tabelaPemRelOG();
    	pcOACel=tabelaPresCapOA();
    	pcGOCel=tabelaPresCapGO();
    	fluc=ProFluCol();
    	flup=ProFlu();
	}
}

elem2dPoroso& elem2dPoroso::operator =(const elem2dPoroso& velem) {
	if (this != &velem) {
		if(cel2D.nvert>0){
			delete[] cel2D.noElem;
			delete[] cel2D.indFace;
			delete[] cel2D.ccFace;
			delete[] cel2D.orientFace;
			delete[] cel2D.centroElem;
			delete[] cel2D.centroideElem;
			delete[] cel2D.dCFMod;
			delete[] cel2D.sFaceMod;
			delete[] cel2D.ownFace;
			delete[] ccPD;
			delete[] ccSat;
			delete[] ccPVN;
			delete[] ccHR;
			delete[] ccPambR;
			delete[] cel2D.vecSDifMod;
			for(int i=0;i<cel2D.nvert;i++){
				delete[] cel2D.centroideFace[i];
				delete[] cel2D.coordVert[i];
				delete[] cel2D.dCF[i];
				delete[] cel2D.sFace[i];
				///////////////////////////////////////////////////////////
				delete[] cel2D.vecE[i];
				delete[] cel2D.vecT[i];
				delete[] cel2D.fInter[i];
				delete[] cel2D.fIfC[i];
				///////////////////////////////////////////////////////////
				delete[] gradPface[i];
				delete[] gradZface[i];
				delete[] gradPcOGface[i];
				delete[] gradPcAOface[i];

				delete[] cel2D.vecSDif[i];

			}
			delete[] cel2D.centroideFace;
			delete[] cel2D.coordVert;
			delete[] cel2D.dCF;
			delete[] cel2D.sFace;
			delete[] vizinho;
			delete[] kvizinho;
			///////////////////////////////////////////////////////////////////////////////
			delete[] cel2D.vecE;
			delete[] cel2D.modE;
			delete[] cel2D.vecT;
			delete[] cel2D.modT;
			delete[] cel2D.fatG;
			delete[] cel2D.angES;
			delete[] cel2D.fInter;
			delete[] cel2D.fIfC;

			delete[] cel2D.gradGreenP;
			delete[] cel2D.gradGreenPI;
			delete[] cel2D.gradGreenZdatum;
			delete[] cel2D.gradGreenZdatumI;
			delete[] cel2D.gradGreenAZdatum;
			delete[] cel2D.gradGreenAZdatumI;
			delete[] cel2D.gradGreenPcOG;
			delete[] cel2D.gradGreenPcAO;
			delete[] cel2D.gradGreenPcAOI;
			delete[] cel2D.gradGreenSw;
			delete[] cel2D.gradGreenSwI;
			//////////////////////////////////////////////////////////////////////////////////
			delete[] presF;
			delete[] swF;
			delete[] alfF;
			delete[] Qtotal;
			delete[] Qw;
			delete[] Qoleo;
			delete[] Qgas;
			delete[] rhoF;
			delete[] rhogF;
			delete[] rhoaF;
			delete[] rhoZF;
			delete[] rhoaZF;
			delete[] mioF;
			delete[] migF;
			delete[] miaF;
			delete[] kmedF;
			delete[] kmedGF;
			delete[] kmedAF;
			delete[] darcyOF;
			delete[] darcyGF;
			delete[] darcyAF;
			delete[] pcAOF;

			delete[] gradPface;
			delete[] gradZface;
			delete[] gradPcOGface;
			delete[] gradPcAOface;


			delete[] coefTHRC;
			delete[] coefTHRV;
			delete[] fonteTHR;

			delete[] cel2D.vecSDif;
		}
		/*if(nvizinho>0){
			delete[] vizinho;
			delete[] kvizinho;
		}*/
		vg1dSP=velem.vg1dSP;
		cel2D.nvert=velem.cel2D.nvert;
		cel2D.dim=velem.cel2D.dim;
		cel2D.indEle=velem.cel2D.indEle;
		cel2D.nele=velem.cel2D.nele;
		nvizinho=velem.nvizinho;
		cel2D.presC=velem.cel2D.presC;
		cel2D.presC0=velem.cel2D.presC0;
		cel2D.presCI=velem.cel2D.presCI;
		cel2D.sLC=velem.cel2D.sLC;
		cel2D.sLI=velem.cel2D.sLI;
		cel2D.sWC=velem.cel2D.sWC;
		cel2D.sWI=velem.cel2D.sWI;
		dh=velem.dh;
		deriFonteT=velem.deriFonteT;
		FonteT=velem.FonteT;
		fluxCal=velem.fluxCal;
		cel2D.kX=velem.cel2D.kX;
		cel2D.kY=velem.cel2D.kY;
		cel2D.kabsol=velem.cel2D.kabsol;
		cel2D.poro=velem.cel2D.poro;
		cel2D.compRoc=velem.cel2D.compRoc;
		cel2D.zD=velem.cel2D.zD;

		cel2D.pBolha=velem.cel2D.pBolha;
		cel2D.tipoModelOleo=velem.cel2D.tipoModelOleo;
		cel2D.compOleo=velem.cel2D.compOleo;
		cel2D.rhoRef=velem.cel2D.rhoRef;

		satConata=velem.satConata;
		ip=velem.ip;
		tRes=velem.tRes;
		sLRes=velem.sLRes;
		sWRes=velem.sWRes;
		pRes=velem.pRes;

		dt=velem.dt;
	    dtSL=velem.dtSL;
	    dtSW=velem.dtSW;
		reiniciaSL=velem.reiniciaSL;
		reiniciaSW=velem.reiniciaSW;
		deriFonteT=velem.deriFonteT;
		FonteT=velem.FonteT;
		fluxCal=velem.fluxCal;

		perm=velem.perm;
		trans=velem.trans;
		fluc=velem.fluc;
		flup=velem.flup;
		kRelOACel=velem.kRelOACel;
		kRelOGCel=velem.kRelOGCel;
		pcOACel=velem.pcOACel;
		pcGOCel=velem.pcGOCel;
		CC=velem.CC;
		//ccTD=velem.ccTD;
		//ccTVN=velem.ccTVN;
		//ccHR=velem.ccHR;
		//ccTambR=velem.ccTambR;
		if(cel2D.nvert>0){
	    	vizinho=new elementoPoroso* [cel2D.nvert];
	    	kvizinho=new int [cel2D.nvert];
			for(int k=0; k<cel2D.nvert;k++){
				kvizinho[k]=velem.kvizinho[k];
				vizinho[k]=velem.vizinho[k];
			}
		}
		else{
			vizinho=0;
			kvizinho=0;
		}
		if(cel2D.nele>0){
			TL=velem.TL;
			local=velem.local;
			cel2D.noFace=0;

			cel2D.alfC;
			cel2D.vbo=velem.cel2D.vbo;
			cel2D.rhostd=velem.cel2D.rhogstd;
			cel2D.rhogstd=velem.cel2D.rhogstd;
			cel2D.rhoastd=velem.cel2D.rhoastd;
			cel2D.rhoP=velem.cel2D.rhoP;
			cel2D.rhogP=velem.cel2D.rhogP;
			cel2D.rhoaP=velem.cel2D.rhoaP;

			cel2D.rhoP0=velem.cel2D.rhoP0;
			cel2D.rhogP0=velem.cel2D.rhogP0;
			cel2D.rhoaP0=velem.cel2D.rhoaP0;
			cel2D.alfC0=velem.cel2D.alfC0;

	    	presF=new double [cel2D.nvert];
	    	swF=new double [cel2D.nvert];
	    	alfF=new double [cel2D.nvert];
	    	Qtotal=new double [cel2D.nvert];
	    	Qw=new double [cel2D.nvert];
	    	Qoleo=new double [cel2D.nvert];
	    	Qgas=new double [cel2D.nvert];
	    	rhoF=new double [cel2D.nvert];
	    	rhogF=new double [cel2D.nvert];
	    	rhoaF=new double [cel2D.nvert];
	    	rhoZF=new double [cel2D.nvert];
	    	rhoaZF=new double [cel2D.nvert];
	    	mioF=new double [cel2D.nvert];
	    	migF=new double [cel2D.nvert];
	    	miaF=new double [cel2D.nvert];
	    	kmedF=new double [cel2D.nvert];
	    	kmedGF=new double [cel2D.nvert];
	    	kmedAF=new double [cel2D.nvert];
	    	darcyOF=new double [cel2D.nvert];
	    	darcyGF=new double [cel2D.nvert];
	    	darcyAF=new double [cel2D.nvert];
	    	pcAOF=new double [cel2D.nvert];

	    	ccPD=new double [cel2D.nvert];
	    	ccSat=new double [cel2D.nvert];
	    	ccPVN=new double [cel2D.nvert];
	    	ccHR=new double [cel2D.nvert];
	    	ccPambR=new double [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++){
	        	ccPD[j]=-1;
	        	ccSat[j]=-1;
	        	ccPVN[j]=-1;
	        	ccHR[j]=-1;
	        	ccPambR[j]=-1;
	    	}

	    	gradPface=new double* [cel2D.nvert];
	    	gradZface=new double* [cel2D.nvert];
	    	gradPcOGface=new double* [cel2D.nvert];
	    	gradPcAOface=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) gradPface[j]=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.nvert;j++) gradZface[j]=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.nvert;j++) gradPcOGface[j]=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.nvert;j++) gradPcAOface[j]=new double [cel2D.dim];

	    	cel2D.gradGreenP=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenP[j]=velem.cel2D.gradGreenP[j];
	    	cel2D.gradGreenPI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPI[j]=velem.cel2D.gradGreenPI[j];
	    	cel2D.gradGreenZdatum=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenZdatum[j]=velem.cel2D.gradGreenZdatum[j];
	    	cel2D.gradGreenZdatumI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenZdatumI[j]=velem.cel2D.gradGreenZdatumI[j];
	    	cel2D.gradGreenAZdatum=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenAZdatum[j]=velem.cel2D.gradGreenAZdatum[j];
	    	cel2D.gradGreenAZdatumI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenAZdatumI[j]=velem.cel2D.gradGreenAZdatumI[j];
	    	cel2D.gradGreenPcOG=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPcOG[j]=velem.cel2D.gradGreenPcOG[j];
	    	cel2D.gradGreenPcAO=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPcAO[j]=velem.cel2D.gradGreenPcAO[j];
	    	cel2D.gradGreenPcAOI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPcAOI[j]=velem.cel2D.gradGreenPcAOI[j];
	    	cel2D.gradGreenSw=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenSw[j]=velem.cel2D.gradGreenSw[j];
	    	cel2D.gradGreenSwI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenSwI[j]=velem.cel2D.gradGreenSwI[j];

	    	cel2D.vecSDif=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) cel2D.vecSDif[j]=new double [cel2D.dim];
	    	cel2D.vecSDifMod=new double [cel2D.nvert];

	    	for(int i=0; i<cel2D.nvert;i++){

	        	presF[i]=velem.presF[i];
	        	swF[i]=velem.swF[i];
	        	alfF[i]=velem.alfF[i];
	        	Qtotal[i]=velem.Qtotal[i];
	        	Qw[i]=velem.Qw[i];
	        	Qoleo[i]=velem.Qoleo[i];
	        	Qgas[i]=velem.Qgas[i];
	        	rhoF[i]=velem.rhoF[i];
	        	rhogF[i]=velem.rhogF[i];
	        	rhoaF[i]=velem.rhoaF[i];
	        	rhoZF[i]=velem.rhoZF[i];
	        	rhoaZF[i]=velem.rhoaZF[i];
	        	mioF[i]=velem.mioF[i];
	        	migF[i]=velem.migF[i];
	        	miaF[i]=velem.miaF[i];
	        	kmedF[i]=velem.kmedF[i];
	        	kmedGF[i]=velem.kmedGF[i];
	        	kmedAF[i]=velem.kmedAF[i];
	        	darcyOF[i]=velem.darcyOF[i];
	        	darcyGF[i]=velem.darcyGF[i];
	        	darcyAF[i]=velem.darcyAF[i];
	        	pcAOF[i]=velem.pcAOF[i];

	        	ccPD[i]=velem.ccPD[i];
	        	ccSat[i]=velem.ccSat[i];
	        	ccPVN[i]=velem.ccPVN[i];
	        	ccHR[i]=velem.ccHR[i];
	        	ccPambR[i]=velem.ccPambR[i];

	        	for(int j=0;j<cel2D.dim;j++) gradPface[i][j]=velem.gradPface[i][j];
	        	for(int j=0;j<cel2D.dim;j++) gradZface[i][j]=velem.gradZface[i][j];
	        	for(int j=0;j<cel2D.dim;j++) gradPcOGface[i][j]=velem.gradPcOGface[i][j];
	        	for(int j=0;j<cel2D.dim;j++) gradPcAOface[i][j]=velem.gradPcAOface[i][j];

	        	for(int j=0;j<cel2D.dim;j++) cel2D.vecSDif[i][j]=velem.cel2D.vecSDif[i][j];

	    	}

	    	for(int j=0;j<cel2D.nvert;j++) cel2D.vecSDifMod[j]=velem.cel2D.vecSDifMod[j];

	    	coefTHRC=new double [cel2D.nvert];
	    	coefTHRV=new double [cel2D.nvert];
	    	fonteTHR=new double [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++){
	    		coefTHRC[i]=velem.coefTHRC[i];
	    		coefTHRV[i]=velem.coefTHRV[i];
	    		fonteTHR[i]=velem.fonteTHR[i];
	    	}
	    	cel2D.noElem=new int [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.noElem[i]=velem.cel2D.noElem[i];
	    	cel2D.indFace=new int [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.indFace[i]=velem.cel2D.indFace[i];
	    	int size=velem.cel2D.indVizCres.size();
	    	for(int i=0; i<size;i++)cel2D.indVizCres.push_back(velem.cel2D.indVizCres[i]);
	    	cel2D.ccFace=new int [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.ccFace[i]=velem.cel2D.ccFace[i];
	    	cel2D.orientFace=new double [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.orientFace[i]=velem.cel2D.orientFace[i];
	    	cel2D.ownFace=new double [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.ownFace[i]=velem.cel2D.ownFace[i];
	    	cel2D.centroElem=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.dim;i++)cel2D.centroElem[i]=velem.cel2D.centroElem[i];
	    	cel2D.centroideElem=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.dim;i++)cel2D.centroideElem[i]=velem.cel2D.centroideElem[i];
	    	cel2D.centroideFace=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) cel2D.centroideFace[j]=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.nvert;i++)
	    		for(int j=0; j<cel2D.dim;j++)cel2D.centroideFace[i][j]=velem.cel2D.centroideFace[i][j];
	    	cel2D.coordVert=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) cel2D.coordVert[j]=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.nvert;i++)
	    		for(int j=0; j<cel2D.dim;j++)cel2D.coordVert[i][j]=velem.cel2D.coordVert[i][j];
	    	cel2D.dCF=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) cel2D.dCF[j]=new double [cel2D.dim];
	       	for(int i=0; i<cel2D.nvert;i++)
	        	for(int j=0; j<cel2D.dim;j++)cel2D.dCF[i][j]=velem.cel2D.dCF[i][j];
	    	cel2D.dCFMod=new double [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.dCFMod[i]=velem.cel2D.dCFMod[i];
	    	cel2D.sFace=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) cel2D.sFace[j]=new double [cel2D.dim];
	       	for(int i=0; i<cel2D.nvert;i++)
	        	for(int j=0; j<cel2D.dim;j++)cel2D.sFace[i][j]=velem.cel2D.sFace[i][j];
	    	cel2D.sFaceMod=new double [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.sFaceMod[i]=velem.cel2D.sFaceMod[i];
	    	///////////////////////////////////////////////////////////////////////////////////////////
	        cel2D.vecE=new double* [cel2D.nvert];
	        for(int j=0; j<cel2D.nvert;j++) cel2D.vecE[j]=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.nvert;i++)
	    		for(int j=0; j<cel2D.dim;j++)cel2D.vecE[i][j]=velem.cel2D.vecE[i][j];
	        cel2D.modE=new double [cel2D.nvert];
	        for(int i=0; i<cel2D.nvert;i++)cel2D.modE[i]=velem.cel2D.modE[i];
	        cel2D.vecT=new double* [cel2D.nvert];
	        for(int j=0; j<cel2D.nvert;j++) cel2D.vecT[j]=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.nvert;i++)
	    		for(int j=0; j<cel2D.dim;j++)cel2D.vecT[i][j]=velem.cel2D.vecT[i][j];
	        cel2D.modT=new double [cel2D.nvert];
	        for(int i=0; i<cel2D.nvert;i++)cel2D.modT[i]=velem.cel2D.modT[i];
	        cel2D.fatG=new double [cel2D.nvert];
	        for(int i=0; i<cel2D.nvert;i++)cel2D.fatG[i]=velem.cel2D.fatG[i];
	        cel2D.angES=new double [cel2D.nvert];
	        for(int i=0; i<cel2D.nvert;i++)cel2D.angES[i]=velem.cel2D.angES[i];
	        cel2D.fInter=new double* [cel2D.nvert];
	        for(int j=0; j<cel2D.nvert;j++) cel2D.fInter[j]=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.nvert;i++)
	    		for(int j=0; j<cel2D.dim;j++)cel2D.fInter[i][j]=velem.cel2D.fInter[i][j];
	        cel2D.fIfC=new double* [cel2D.nvert];
	        for(int j=0; j<cel2D.nvert;j++) cel2D.fIfC[j]=new double [cel2D.dim];
	    	for(int i=0; i<cel2D.nvert;i++)
	    		for(int j=0; j<cel2D.dim;j++)cel2D.fIfC[i][j]=velem.cel2D.fIfC[i][j];
	    	cel2D.vElem=velem.cel2D.vElem;
		}
	}
	return *this;
}

void elem2dPoroso::buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle){
	for(int i=0;i<nVert;i++){
		face[i]=-1;
		int v1=noEle[elem][i];
		int v2;
		if(i<nVert-1) v2=noEle[elem][i+1];
		else v2=noEle[elem][0];
		int match=0;
		for(int j=0;j<nEle;j++){
			if(j!=elem){
				for(int k=0;k<nVert;k++){
					int v1V=noEle[j][k];
					int v2V;
					if(k<nVert-1) v2V=noEle[j][k+1];
					else v2V=noEle[j][0];
					if((v1V==v1 || v1V==v2) && (v2V==v1 || v2V==v2)){
						match=1;
						face[i]=j;
					}
					if(match==1)break;
				}
			}
			if(match==1)break;
		}
	}
}

void elem2dPoroso::menorIndViz(){
	/*int rank=0;
	if(kvizinho[iviz]>=0){
		for(int i=0;i<cel2D.nvert;i++){
			if(kvizinho[i]>=0 && cel2D.indFace[iviz]>cel2D.indFace[i])rank++;
		}
	}
	else rank=-1;
	return rank;*/

	cel2D.indVizCres.push_back(cel2D.indEle);
	for(int i=0;i<cel2D.nvert;i++)if(cel2D.indFace[i]>=0)cel2D.indVizCres.push_back(cel2D.indFace[i]);
	sort(cel2D.indVizCres.begin(), cel2D.indVizCres.end());
}

double elem2dPoroso::escalar(double* v1, double* v2, int vDim){
	double tot=0.;
	for(int i=0;i<vDim;i++)tot+=v1[i]*v2[i];
	return tot;
}

void elem2dPoroso::indraz(int& ind, double& raz,
		double tempo, int parserie, double* serietemp) {

	for (int i = 0; i <= parserie - 1; i++) {
		if (i < parserie - 1) {
			if (tempo >= serietemp[i] && tempo < serietemp[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - serietemp[i])
								/ (serietemp[i + 1] - serietemp[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void elem2dPoroso::faceDetalhes(){

	/*
	double** vecE;//vetor unitario na orientacao entre o centroide de do elementoPoroso e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elementoPoroso e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;//distancia entre o centroide da face e a intersecao da face
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementoPorosos
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementoPorosos e a face do elementoPoroso
    double** fIfC;//vetor entre a intersecao e o centroide da face
	 */
	for(int i=0;i<cel2D.nvert;i++){
		cel2D.modE[i]=0.;
		for(int j=0;j<cel2D.dim;j++){
			if(kvizinho[i]>=0){
				cel2D.vecE[i][j]=(*vizinho[i]).centroideElem[j]-cel2D.centroideElem[j];
			}
			else{
				cel2D.vecE[i][j]=cel2D.centroideFace[i][j]-cel2D.centroideElem[j];
			}
			cel2D.modE[i]+=cel2D.vecE[i][j]*cel2D.vecE[i][j];
		}
		cel2D.modE[i]=sqrtl(cel2D.modE[i]);
		for(int j=0;j<cel2D.dim;j++)cel2D.vecE[i][j]=cel2D.vecE[i][j]/cel2D.modE[i];
		cel2D.angES[i]=0.;
		for(int j=0;j<cel2D.dim;j++){
			cel2D.angES[i]+=(cel2D.sFace[i][j]/cel2D.sFaceMod[i])*cel2D.vecE[i][j];
		}
		double denf=0.;
		double numf=0.;
		for(int j=0;j<cel2D.dim;j++){
			numf+=(cel2D.centroideFace[i][j]-cel2D.centroideElem[j])*cel2D.sFace[i][j];
			denf+=cel2D.sFace[i][j]*cel2D.vecE[i][j];
		}
		double kf=numf/denf;
		for(int j=0;j<cel2D.dim;j++){
			//cel2D.fInter[i][j]=
					//(cel2D.centroideFace[i][j]-cel2D.centroideElem[j])*cel2D.sFace[i][j]/(cel2D.sFace[i][j]*cel2D.vecE[i][j])+
					//cel2D.centroideElem[j];
			cel2D.fInter[i][j]=kf*cel2D.vecE[i][j]+cel2D.centroideElem[j];
		}

		cel2D.fatG[i]=0;
		if(kvizinho[i]>=0){
			for(int j=0;j<cel2D.dim;j++){
				double del=(*vizinho[i]).centroideElem[j]-cel2D.fInter[i][j];
				cel2D.fatG[i]+=del*del;
			}
			cel2D.fatG[i]=sqrtl(cel2D.fatG[i]);
			cel2D.fatG[i]/=cel2D.modE[i];
		}
		else cel2D.fatG[i]=0.;

		cel2D.modT[i]=0.;
		for(int j=0;j<cel2D.dim;j++){
			cel2D.vecT[i][j]=cel2D.centroideFace[i][j]-cel2D.fInter[i][j];
			cel2D.modT[i]+=cel2D.vecT[i][j]*cel2D.vecT[i][j];
		}
		cel2D.modT[i]=sqrtl(cel2D.modT[i]);

	}

}

double elem2dPoroso::interpolaTabela(int nserie, double valx, double* x, double* y){
	double val;
	int ind=0;
	int min=1;
	double raz;
	for (int i = 0; i <= nserie - 1; i++) {
		if (i < nserie - 1) {
			if(valx<x[0]){
				ind=0;
				min=-1;
				break;
			}
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
	  if (ind < nserie - 1 && min>-1){
		  sup = y[ind + 1];
		  val = (inf * raz + (1 - raz) * sup);
	  }
	  else{
		  val = y[ind];
	  }

	return val;
}

double elem2dPoroso::fkO(double satW, double satG){
	/*double krw=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelW);
	double krow=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC=interpolaTabela(kRelOACel.npont, satConata, kRelOACel.satW, kRelOACel.permRelOW);

	double val=krowC*(((krow/krowC)+krw)-(krw));*/

	/*double krw0=interpolaTabela(kRelOACel.npont, 0.49, kRelOACel.satW, kRelOACel.permRelW);
	double krow0=interpolaTabela(kRelOACel.npont, 0.49, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC0=interpolaTabela(kRelOACel.npont, 0.13, kRelOACel.satW, kRelOACel.permRelOW);
	double krg0=interpolaTabela(kRelOGCel.npont, 0.195, kRelOGCel.satG, kRelOGCel.permRelG);
	double krog0=interpolaTabela(kRelOGCel.npont, 0.195, kRelOGCel.satG, kRelOGCel.permRelOG);

	double val0=krowC0*(((krow0/krowC0)+krw0)*((krog0/krowC0)+krg0)-(krw0+krg0));*/

	double krw=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelW);
	double krow=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC=interpolaTabela(kRelOACel.npont, satConata, kRelOACel.satW, kRelOACel.permRelOW);
	double krg=interpolaTabela(kRelOGCel.npont, satG, kRelOGCel.satG, kRelOGCel.permRelG);
	double krog=interpolaTabela(kRelOGCel.npont, satG, kRelOGCel.satG, kRelOGCel.permRelOG);

	double val=krowC*(((krow/krowC)+krw)*((krog/krowC)+krg)-(krw+krg));
	if(val<0)val=0.;

	return val;
}

void elem2dPoroso::tipoCC(int i,int& diri, int& vn, int& rich, int& acoplado, int& kcc){
	while(kcc<CC.nDiri && cel2D.ccFace[i]!=CC.ccDir[kcc].rotulo) kcc++;
	if(kcc<CC.nDiri && cel2D.ccFace[i]==CC.ccDir[kcc].rotulo)diri=1;
	else{
		kcc=0;
		while(kcc<CC.nRic && cel2D.ccFace[i]!=CC.ccRic[kcc].rotulo) kcc++;
		if(kcc<CC.nRic && cel2D.ccFace[i]==CC.ccRic[kcc].rotulo)rich=1;
		else{
			kcc=0;
			while(kcc<CC.nVN && cel2D.ccFace[i]!=CC.ccVN[kcc].rotulo) kcc++;
			if(kcc<CC.nVN && cel2D.ccFace[i]==CC.ccVN[kcc].rotulo)vn=1;
			else{
				kcc=0;
				while(kcc<1 && cel2D.ccFace[i]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && cel2D.ccFace[i]==CC.rotuloAcop)acoplado=1;
			}
		}
	}
}

void elem2dPoroso::atualizaCC(int i){
	int diri=0;
	int rich=0;
	int vn=0;
	int kcc=0;
	int acoplado=0;
	tipoCC(i, diri, vn, rich,acoplado,kcc);
    int ind=0;
    double raz;
	if(diri==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccDir[kcc].nserie, CC.ccDir[kcc].tempo);
		double inf = CC.ccDir[kcc].val[ind];
		double sup;
		double infSat = CC.ccDir[kcc].valSat[ind];
		double supSat;
		if (ind < CC.ccDir[kcc].nserie - 1){
			sup = CC.ccDir[kcc].val[ind + 1];
			supSat= CC.ccDir[kcc].valSat[ind+1];
			ccPD[i] = (inf * raz + (1 - raz) * sup);
			ccSat[i] = (infSat * raz + (1 - raz) * supSat);
		}
		else{
			ccPD[i] = CC.ccDir[kcc].val[ind];
			ccSat[i] = CC.ccDir[kcc].valSat[ind];
		}
	}
	else if(vn==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccVN[kcc].nserie, CC.ccVN[kcc].tempo);
		double inf = CC.ccVN[kcc].val[ind];
		double sup;
		double infSat = CC.ccVN[kcc].valSat[ind];
		double supSat;
		if (ind < CC.ccVN[kcc].nserie - 1){
			sup = CC.ccVN[kcc].val[ind + 1];
			supSat= CC.ccDir[kcc].valSat[ind+1];
			ccPVN[i] = (inf * raz + (1 - raz) * sup);
			ccSat[i] = (infSat * raz + (1 - raz) * supSat);
		}
		else{
			ccPVN[i] = CC.ccVN[kcc].val[ind];
			ccSat[i] = CC.ccVN[kcc].valSat[ind];
		}
	}
	else if(rich==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccRic[kcc].nserie, CC.ccRic[kcc].tempo);
		double inf = CC.ccRic[kcc].valAmb[ind];
		double sup;
		double infSat = CC.ccRic[kcc].valSat[ind];
		double supSat;
		if (ind < CC.ccRic[kcc].nserie - 1){
			sup = CC.ccRic[kcc].valAmb[ind + 1];
			supSat= CC.ccDir[kcc].valSat[ind+1];
			ccPambR[i] = (inf * raz + (1 - raz) * sup);
			ccSat[i] = (infSat * raz + (1 - raz) * supSat);
		}
		else{
			ccPambR[i] = CC.ccRic[kcc].valAmb[ind];
			ccSat[i] = CC.ccRic[kcc].valSat[ind];
		}


		inf = CC.ccRic[kcc].hAmb[ind];
		if (ind < CC.ccRic[kcc].nserie - 1){
			sup = CC.ccRic[kcc].hAmb[ind + 1];
			ccHR[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccHR[i] = CC.ccRic[kcc].hAmb[ind];

	}
}

void elem2dPoroso::calSupDifu(){
	double Dmed[2];
	double perm[2];
	double permV[2];
	perm[0]=cel2D.kX;
	perm[1]=cel2D.kY;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			permV[0]=(*vizinho[i]).kX;
			permV[1]=(*vizinho[i]).kY;
			for(int j=0; j<cel2D.dim;j++){
				Dmed[j]=cel2D.fatG[i]/perm[j]+(1-cel2D.fatG[i])/permV[j];
				Dmed[j]=1./Dmed[j];
			}
		}
		else{
			for(int j=0; j<cel2D.dim;j++){
				Dmed[j]=perm[j];
			}
		}
		for(int j=0; j<cel2D.dim;j++)cel2D.vecSDif[i][j]=Dmed[j]*cel2D.sFace[i][j];
		cel2D.vecSDifMod[i]=0.;
		for(int j=0; j<cel2D.dim;j++)cel2D.vecSDifMod[i]+=(cel2D.vecSDif[i][j]*cel2D.vecSDif[i][j]);
		cel2D.vecSDifMod[i]=sqrtl(cel2D.vecSDifMod[i]);
	}
}

void elem2dPoroso::calcGradGreen(){
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			double pinter=cel2D.fatG[i]*cel2D.presC+(1.-cel2D.fatG[i])*(*vizinho[i]).presC;
			double swinter=cel2D.fatG[i]*cel2D.sWC+(1.-cel2D.fatG[i])*(*vizinho[i]).sWC;
			double zDatumC=cel2D.zD;
			double zDatumV=(*vizinho[i]).zD;
			/*double rhoZinter=
					(9.82/98066.22)*(cel2D.fatG[i](cel2D.alfC*cel2D.rhogP+(1.-cel2D.alfC)*cel2D.rhoP)+
							(1.-cel2D.fatG[i])*((*vizinho[i]).alfC*(*vizinho[i]).rhogP+(1.-(*vizinho[i]).alfC)*(*vizinho[i]).rhoP));
			double rhoAZinter=(9.82/98066.22)*(cel2D.fatG[i]*cel2D.rhoaP+(1.-cel2D.fatG[i])*(*vizinho[i]).rhoaP);*/
			double rhoZinter=cel2D.fatG[i]*cel2D.zD+(1.-cel2D.fatG[i])*(*vizinho[i]).zD;
			presF[i]=0.;
			swF[i]=0.;
			alfF[i]=0;
			rhoF[i]=0.;
			rhogF[i]=0.;
			rhoaF[i]=0.;
			mioF[i]=0.;
			migF[i]=0.;
			miaF[i]=0.;
			kmedF[i]=0.;
			kmedGF[i]=0.;
			kmedAF[i]=0.;
			darcyOF[i]=0.;
			darcyGF[i]=0.;
			darcyAF[i]=0.;
			pcAOF[i]=0.;
			rhoZF[i]=0.;
			rhoaZF[i]=0.;
				//esta abordagem não parece corrigir bem a distorção pelo fator da interpolação nção estar no centro da superfície:

			//for(int j=0; j<cel2D.dim;j++)
			//tempF[i]+=cel2D.fatG[i]*cel2D.gradGreenT[j]*(cel2D.centroideFace[i][j]-cel2D.centroideElem[j])+
			//(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenT[j]*(cel2D.centroideFace[i][j]-(*vizinho[i]).centroideElem[j]);

				//esta abordagem, aparentemente. levou a resultados melhores:

			//for(int j=0; j<cel2D.dim;j++)
			//tempF[i]+=cel2D.fatG[i]*cel2D.gradGreenT[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j])+
				//	  (1-cel2D.fatG[i])*(*vizinho[i]).gradGreenT[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			//tempF[i]+=tinter;
			double corDistorc=1.;
			if(fabsl(cel2D.angES[i])<0.9)corDistorc=0.;

			double gradMed [cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenPI[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenPI[j];
			}
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double deri=-(cel2D.presC-(*vizinho[i]).presC)/cel2D.modE[i];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
			}
			for(int j=0; j<cel2D.dim;j++)
			presF[i]+=corDistorc*gradMed[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			presF[i]+=pinter;
			Qoleo[i]=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			Qoleo[i]=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
			Qw[i]=Qoleo[i];

			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenSwI[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenSwI[j];
			}
			termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			deri=-(cel2D.sWC-(*vizinho[i]).sWC)/cel2D.modE[i];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
			}
			for(int j=0; j<cel2D.dim;j++)
			swF[i]+=corDistorc*gradMed[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			swF[i]+=swinter;

			if(cel2D.tipoModelOleo==1)
				rhoF[i]=flup.MasEspoleo(presF[i], tRes);
			else
				rhoF[i]=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-presF[i]));
			rhogF[i]=flup.MasEspGas(presF[i], tRes);
			rhoaF[i]=flup.MasEspAgua(presF[i], tRes);
			if(cel2D.tipoModelOleo==1){
				double tit=flup.FracMassHidra(presF[i], tRes);
				alfF[i]=(tit/rhogF[i])/((tit/rhogF[i])+((1.-tit)/rhoF[i]));
			}
			else alfF[i]=0.;
			//if(presF[i]<cel2D.pBolha-0.01)
				mioF[i]=flup.ViscOleo(presF[i], tRes)/1000.;
			//else mioF[i]=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;
			migF[i]=flup.ViscGas(presF[i], tRes)/1000.;
			miaF[i]=flup.VisAgua(tRes)/1000.;
			rhoF[i]=(1.-alfF[i])*rhoF[i]+alfF[i]*rhogF[i];
			kmedF[i]=fkO(swF[i], 1*alfF[i]*(1.-swF[i]));
			kmedAF[i]=interpolaTabela(kRelOACel.npont, swF[i], kRelOACel.satW, kRelOACel.permRelW);
			//double kabsolHarm=cel2D.fatG[i]/cel2D.kabsol+(1-cel2D.fatG[i])/(*vizinho[i]).kabsol;
			//kabsolHarm=1./kabsolHarm;
			double kabsolHarm=1.;
			darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-alfF[i])*mioF[i]+alfF[i]*migF[i]);
			//darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-tit)*mioF[i]+tit*migF[i]);
			darcyAF[i]=kmedAF[i]*kabsolHarm*98066.22/miaF[i];

			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenPcAOI[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenPcAOI[j];
			}
			termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			deri=-(cel2D.pcAO-(*vizinho[i]).pcAO)/cel2D.modE[i];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
			}
			//Qw[i]-=1*escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			Qw[i]-=1*escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
			pcAOF[i]=interpolaTabela(pcOACel.npont, swF[i], pcOACel.satW, pcOACel.presCapOW);

			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenZdatumI[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenZdatumI[j];
			}
			termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			deri=-(cel2D.zD-(*vizinho[i]).zD)/cel2D.modE[i];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
			}
			//Qoleo[i]-=1*(rhoF[i]*(9.82/98066.22))*escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			Qoleo[i]-=1*(rhoF[i]*(9.82/98066.22))*escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
			for(int j=0; j<cel2D.dim;j++)
			rhoZF[i]+=corDistorc*gradMed[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			rhoZF[i]+=rhoZinter;
			//rhoZF[i]*=(9.82/98066.22);

			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenAZdatumI[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenAZdatumI[j];
			}
			termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			deri=-(cel2D.zD-(*vizinho[i]).zD)/cel2D.modE[i];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
			}
			//Qw[i]-=1*(rhoaF[i]*(9.82/98066.22))*escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			Qw[i]-=1*(rhoaF[i]*(9.82/98066.22))*escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
			for(int j=0; j<cel2D.dim;j++)
			rhoaZF[i]+=corDistorc*gradMed[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			rhoaZF[i]+=rhoZinter;
			//rhoaZF[i]*=(9.82/98066.22);
			Qoleo[i]*=darcyOF[i];
			Qw[i]*=darcyAF[i];
			Qgas[i]=alfF[i]*Qoleo[i];
			Qoleo[i]*=(1.-alfF[i]);
			/*if(contaExit>9){
				int para;
				para=0;
			}*/

		}
		else{
			int diri=0;
			int rich=0;
			int vn=0;
			int kcc=0;
			int acoplado=0;
			tipoCC(i, diri, vn, rich,acoplado,kcc);
			if(diri==1){
				presF[i]=ccPD[i];
				if(Qoleo[i]>=0)	swF[i]=ccSat[i];
				else swF[i]=cel2D.sWC;

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenPI[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double deri=-(cel2D.presC-presF[i])/cel2D.modE[i];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
				}
				//Qoleo[i]=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				Qoleo[i]=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
				Qw[i]=Qoleo[i];

				if(cel2D.tipoModelOleo==1)
					rhoF[i]=flup.MasEspoleo(presF[i], tRes);
				else
					rhoF[i]=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-presF[i]));
				rhogF[i]=flup.MasEspGas(presF[i], tRes);
				rhoaF[i]=flup.MasEspAgua(presF[i], tRes);
				if(cel2D.tipoModelOleo==1){
					double tit=flup.FracMassHidra(presF[i], tRes);
					alfF[i]=(tit/rhogF[i])/((tit/rhogF[i])+((1.-tit)/rhoF[i]));
				}
				else alfF[i]=0.;
				//if(presF[i]<cel2D.pBolha-0.01)
					mioF[i]=flup.ViscOleo(presF[i], tRes)/1000.;
				//else mioF[i]=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;
				migF[i]=flup.ViscGas(presF[i], tRes)/1000.;
				miaF[i]=flup.VisAgua(tRes)/1000.;
				rhoF[i]=(1.-alfF[i])*rhoF[i]+alfF[i]*rhogF[i];
				kmedF[i]=fkO(swF[i], 1*alfF[i]*(1.-swF[i]));
				kmedAF[i]=interpolaTabela(kRelOACel.npont, swF[i], kRelOACel.satW, kRelOACel.permRelW);
				//double kabsolHarm=cel2D.kabsol;
				double kabsolHarm=1.;
				darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-alfF[i])*mioF[i]+alfF[i]*migF[i]);
				//darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-tit)*mioF[i]+tit*migF[i]);
				darcyAF[i]=kmedAF[i]*kabsolHarm*98066.22/miaF[i];
				pcAOF[i]=interpolaTabela(pcOACel.npont, cel2D.sWC, pcOACel.satW, pcOACel.presCapOW);

				rhoZF[i]=cel2D.zD;
				rhoaZF[i]=cel2D.zD;

				Qoleo[i]*=darcyOF[i];
				Qw[i]*=darcyAF[i];
				Qgas[i]=alfF[i]*Qoleo[i];
				Qoleo[i]*=(1.-alfF[i]);

			}
			else if(vn==1 || acoplado==1){
				/*double condHarm=cel2D.cond;
				//double gradareaB=-ccTVN[i]/condHarm;???????
				double gradareaB=ccTVN[i]/condHarm;//fluxo negativo, saindo do sólido, grad negativo

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				//tempF[i]=cel2D.tempC+(gradareaB-escalGradArea+cel2D.sFaceMod[i]*termoCorda)*cel2D.modE[i]/cel2D.sFaceMod[i];
				coefTHRC[i]=1.;
				fonteTHR[i]=(gradareaB-escalGradArea+termoCorda*cordaArea)*cel2D.modE[i]/cordaArea;
				tempF[i]=cel2D.tempC+fonteTHR[i];*/
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenPI[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				coefTHRC[i]=1.;
				fonteTHR[i]=termoCorda*cel2D.modE[i];
				presF[i]=cel2D.presC+1*fonteTHR[i];

				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenSwI[j];
				}
				termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double fontesw=termoCorda*cel2D.modE[i];
				if(ccPVN[i]>=0){
					if(vn==1)swF[i]=ccSat[i];
					else swF[i]=CC.satAcop;
				}
				else swF[i]=cel2D.sWC+1*fontesw;

				if(cel2D.tipoModelOleo==1)
					rhoF[i]=flup.MasEspoleo(presF[i], tRes);
				else
					rhoF[i]=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-presF[i]));
				rhogF[i]=flup.MasEspGas(presF[i], tRes);
				rhoaF[i]=flup.MasEspAgua(presF[i], tRes);
				if(cel2D.tipoModelOleo==1){
					double tit=flup.FracMassHidra(presF[i], tRes);
					alfF[i]=(tit/rhogF[i])/((tit/rhogF[i])+((1.-tit)/rhoF[i]));
				}
				else alfF[i]=0.;
				//if(presF[i]<cel2D.pBolha-0.01)
					mioF[i]=flup.ViscOleo(presF[i], tRes)/1000.;
				//else mioF[i]=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;
				migF[i]=flup.ViscGas(presF[i], tRes)/1000.;
				miaF[i]=flup.VisAgua(tRes)/1000.;
				rhoF[i]=(1.-alfF[i])*rhoF[i]+alfF[i]*rhogF[i];
				kmedF[i]=fkO(swF[i], 1*alfF[i]*(1.-swF[i]));
				kmedAF[i]=interpolaTabela(kRelOACel.npont, swF[i], kRelOACel.satW, kRelOACel.permRelW);
				//double kabsolHarm=cel2D.kabsol;
				double kabsolHarm=1.;
				darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-alfF[i])*mioF[i]+alfF[i]*migF[i]);
				//darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-tit)*mioF[i]+tit*migF[i]);
				darcyAF[i]=kmedAF[i]*kabsolHarm*98066.22/miaF[i];
				pcAOF[i]=interpolaTabela(pcOACel.npont, cel2D.sWC, pcOACel.satW, pcOACel.presCapOW);

				rhoZF[i]=cel2D.zD;
				rhoaZF[i]=cel2D.zD;

				//double rhomist= (kmedF[i]*rhoF[i]+kmedAF[i]*rhoaF[i])/(kmedF[i]+kmedAF[i]);
				double rhomist= (darcyOF[i]*rhoF[i]+darcyAF[i]*rhoaF[i])/(darcyOF[i]+darcyAF[i]);
				double Qvol=ccPVN[i]*cel2D.sFaceMod[i]/rhomist;

				Qoleo[i]=(darcyOF[i]/(darcyOF[i]+darcyAF[i]))*Qvol;
				Qw[i]=(darcyAF[i]/(darcyOF[i]+darcyAF[i]))*Qvol;

				Qgas[i]=alfF[i]*Qoleo[i];
				Qoleo[i]*=(1.-alfF[i]);
			}
			else if(rich==1){
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenPI[j];
				}

				double mio;
				//if(presF[i]<cel2D.pBolha-0.01)
					mio=flup.ViscOleo(cel2D.presC, tRes)/1000.;
				//else mio=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;
				double mig=flup.ViscGas(cel2D.presC, tRes)/1000.;
				double mia=flup.VisAgua(tRes)/1000.;
				double kO=fkO(cel2D.sWC, cel2D.alfC);
				double kA=interpolaTabela(kRelOACel.npont, cel2D.sWC, kRelOACel.satW, kRelOACel.permRelW);
				double darcyO=kO*1*98066.22/((1.-cel2D.alfC)*mio+cel2D.alfC*mig);
				double darcyA=kA*1*98066.22/mia;

				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double denom=((darcyO+darcyA)*cordaArea/cel2D.modE[i])+ccHR[i]*cel2D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double correrP=escalGradArea-termoCorda*cordaArea;
				double num=ccHR[i]*cel2D.sFaceMod[i]*ccPambR[i]+
						cel2D.kabsol*(darcyO+darcyA)*cordaArea*cel2D.presC/cel2D.modE[i]-cel2D.kabsol*(darcyO+darcyA)*correrP;

				coefTHRC[i]=(cel2D.kabsol*(darcyO+darcyA)*cordaArea/cel2D.modE[i])/denom;
				fonteTHR[i]=(ccHR[i]*cel2D.sFaceMod[i]*ccPambR[i]-cel2D.kabsol*(darcyO+darcyA)*correrP)/denom;
				presF[i]=num/denom;

				if(Qoleo[i]>=0)	swF[i]=ccSat[i];
				else swF[i]=cel2D.sWC;

				termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double deri=-(cel2D.presC-presF[i])/cel2D.modE[i];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
				}
				Qoleo[i]=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
				Qw[i]=Qoleo[i];

				if(cel2D.tipoModelOleo==1)
					rhoF[i]=flup.MasEspoleo(presF[i], tRes);
				else
					rhoF[i]=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-presF[i]));
				rhogF[i]=flup.MasEspGas(presF[i], tRes);
				rhoaF[i]=flup.MasEspAgua(presF[i], tRes);
				if(cel2D.tipoModelOleo==1){
					double tit=flup.FracMassHidra(presF[i], tRes);
					alfF[i]=(tit/rhogF[i])/((tit/rhogF[i])+((1.-tit)/rhoF[i]));
				}
				else alfF[i]=0.;
				//alfF[i]=0.;
				//if(presF[i]<cel2D.pBolha-0.01)
					mioF[i]=flup.ViscOleo(presF[i], tRes)/1000.;
				//else mioF[i]=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;
				migF[i]=flup.ViscGas(presF[i], tRes)/1000.;
				miaF[i]=flup.VisAgua(tRes)/1000.;
				rhoF[i]=(1.-alfF[i])*rhoF[i]+alfF[i]*rhogF[i];
				kmedF[i]=fkO(swF[i], 1*alfF[i]*(1.-swF[i]));
				kmedAF[i]=interpolaTabela(kRelOACel.npont, swF[i], kRelOACel.satW, kRelOACel.permRelW);
				//double kabsolHarm=cel2D.kabsol;
				double kabsolHarm=cel2D.kabsol;
				darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-alfF[i])*mioF[i]+alfF[i]*migF[i]);
				//darcyOF[i]=kmedF[i]*kabsolHarm*98066.22/((1.-tit)*mioF[i]+tit*migF[i]);
				darcyAF[i]=kmedAF[i]*kabsolHarm*98066.22/miaF[i];
				pcAOF[i]=interpolaTabela(pcOACel.npont, cel2D.sWC, pcOACel.satW, pcOACel.presCapOW);

				rhoZF[i]=cel2D.zD;
				rhoaZF[i]=cel2D.zD;

				Qoleo[i]*=darcyOF[i];
				Qw[i]*=darcyAF[i];
				Qgas[i]=alfF[i]*Qoleo[i];
				Qoleo[i]*=(1.-alfF[i]);
			}
			/*else if(vn==1){
				double condHarm=cel2D.cond;
				double gradareaB=-ccTVN[i]/condHarm;

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				//tempF[i]=cel2D.tempC+(gradareaB-escalGradArea+cel2D.sFaceMod[i]*termoCorda)*cel2D.modE[i]/cel2D.sFaceMod[i];
				tempF[i]=cel2D.tempC+(gradareaB-escalGradArea+termoCorda*cordaArea)*cel2D.modE[i]/cordaArea;
			}
			else if(rich==1){
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double denom=(cel2D.cond*cordaArea/cel2D.modE[i])+ccHR[i]*cel2D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double correrT=escalGradArea-termoCorda*cordaArea;
				double num=ccHR[i]*cel2D.sFaceMod[i]*ccTambR[i]+
						cel2D.cond*cordaArea*cel2D.tempC/cel2D.modE[i]-cel2D.cond*correrT;
				tempF[i]=num/denom;
			}*/
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenP[j]=0.;
		cel2D.gradGreenZdatum[j]=0.;
		cel2D.gradGreenAZdatum[j]=0.;
		cel2D.gradGreenPcAO[j]=0.;
	}
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++){
			cel2D.gradGreenP[j]+=presF[i]*cel2D.sFace[i][j];
			cel2D.gradGreenZdatum[j]+=(rhoZF[i])*cel2D.sFace[i][j];
			cel2D.gradGreenAZdatum[j]+=(rhoaZF[i])*cel2D.sFace[i][j];
			cel2D.gradGreenPcAO[j]+=pcAOF[i]*cel2D.sFace[i][j];

		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenP[j]/=cel2D.vElem;
		cel2D.gradGreenZdatum[j]/=cel2D.vElem;
		cel2D.gradGreenAZdatum[j]/=cel2D.vElem;
		cel2D.gradGreenPcAO[j]/=cel2D.vElem;
	}
}

double elem2dPoroso::cflA(){
	double vazTot=0.;
	for(int i=0;i<cel2D.nvert;i++){
		   if(Qw[i]<0.)vazTot+=fabsl(Qw[i]);
	}
	double porod=cel2D.poro*(1-cel2D.compRoc*(cel2D.presC-pRes));
	double massCel=(cel2D.sWC)*porod*cel2D.vElem;
	if(vazTot>1e-15)return massCel/vazTot;
	else return 10000000;
}

void elem2dPoroso::evoluiSW(int& reinicia){

	dtSW=dt;
	double poro0=cel2D.poro*(1-cel2D.compRoc*(cel2D.presC-pRes));
	//double area=rm*drQ;//dividido por 2*Pi
	double area=cel2D.vElem;
	//if(icel==1)area*=2.;
	double multTA=poro0*area*cel2D.rhoaP;

	double massTot=0;
	for(int i=0;i<cel2D.nvert;i++){
		   massTot+=rhoaF[i]*Qw[i];
			if(kvizinho[i]>=0){
				int para;
				para=0;
			}
	}

	double balanco=(1./multTA)*massTot;

	double val=cel2D.sWI+balanco*dt;
	if(val<satConata && cel2D.sWI==satConata)val=satConata;
	else if(val>1. && cel2D.sWI==1.)val=1.;
    double alfx=1.-val;
    double localtiny=1e-5;

	if(((val<=1.+localtiny)&&(val>=1.-localtiny))){
		   //alf=fabsl(0.);
		cel2D.sWC=1.;
	}
	else if(val>1.+localtiny){
	      double dtaux;
	      if(fabsl(balanco)>1e-15)
	      dtaux=(1.-cel2D.sWI)/(balanco);
	      else{
	    	  dtaux=0.;
	      }
	      //if(dtaux<dt*CritDTMin)dtaux=dt*CritDTMin;
	      if(reinicia==0 && dtaux>1e-5){
	        dtSW=dtaux;
	        //reinicia=-1;
	        reiniciaSW=-1;
	        cel2D.sWC=1.;
	      }
	      else{
	    	  cel2D.sWC=1.;
	    	  dtSW=dt;
	      }
	}
	else if((val<=(satConata+localtiny)&&val>=(satConata-localtiny))){
		cel2D.sWC=fabsl(satConata);
	}
	else if(val<(satConata-localtiny)){
	      double dtaux;
	      if(fabsl(balanco)>1e-15)
	      dtaux=(satConata-cel2D.sWI)/(balanco);
	      else dtaux=0.;
	      if(reinicia==0 && dtaux>1e-5){
	        dtSW=dtaux;
	        //reinicia=-1;
	        reiniciaSW=-1;
	        cel2D.sWC=fabsl(satConata);
	      }
	      else{
	    	  cel2D.sWC=fabsl(satConata);
	    	  dtSW=dt;
	      }
	}
	else cel2D.sWC=val;

	if(reiniciaSW==-1){
		int para;
		para=0;
	}

	//sW=0.13;
	//dtSW=dt;
	//reiniciaSW=0;


}


int elem2dPoroso::achaInd(int i){
	int siz=cel2D.indVizCres.size();
	int saida=-1;
	for(int j=0;j<siz;j++){
		if(i==cel2D.indVizCres[j]){
			saida=j;
			break;
		}
	}
	return saida;
}

void elem2dPoroso::GeraLocal(double relax){
	TL[0]=0.;
	int diag=achaInd(cel2D.indEle);
	local.mx[0][diag]=0.;
	/*if(cel2D.indEle==1){
		int para;
		para=0;
	}*/
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			double gradMedOZ [cel2D.dim];
			double gradMedAZ [cel2D.dim];
			double gradMedPCAO [cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenP[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenP[j];
				gradMedOZ[j]=cel2D.fatG[i]*cel2D.gradGreenZdatum[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenZdatum[j];
				gradMedAZ[j]=cel2D.fatG[i]*cel2D.gradGreenAZdatum[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenAZdatum[j];
				gradMedPCAO[j]=cel2D.fatG[i]*cel2D.gradGreenPcAO[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenPcAO[j];
			}
			double condHarm;//=2*cel2D.cond*(*vizinho[i]).cond/(cel2D.cond+(*vizinho[i]).cond);
			//condHarm=cel2D.fatG[i]/cel2D.kabsol+(1-cel2D.fatG[i])/(*vizinho[i]).kabsol;
			condHarm=(rhoF[i]*darcyOF[i]+rhoaF[i]*darcyAF[i]);
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			//double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double escalGradArea=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
			//double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
			TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
			double termMat=(condHarm*cordaArea/cel2D.modE[i]);
			local.mx[0][col]=-termMat;
			local.mx[0][diag]+=termMat;

			termoCorda=escalar(gradMedOZ,cel2D.vecE[i],cel2D.dim);
			//escalGradArea=escalar(gradMedOZ,cel2D.sFace[i],cel2D.dim);
			escalGradArea=escalar(gradMedOZ,cel2D.vecSDif[i],cel2D.dim);
			double zDatumC=cel2D.zD;
			double zDatumV=(*vizinho[i]).zD;
			termMat=(cordaArea/cel2D.modE[i])*(zDatumV-zDatumC);
			TL[0]-=1*rhoF[i]*darcyOF[i]*(rhoF[i]*(9.82/98066.22))*((escalGradArea-termoCorda*cordaArea)+termMat);

			termoCorda=escalar(gradMedAZ,cel2D.vecE[i],cel2D.dim);
			//escalGradArea=escalar(gradMedAZ,cel2D.sFace[i],cel2D.dim);
			escalGradArea=escalar(gradMedAZ,cel2D.vecSDif[i],cel2D.dim);
			double zDatumAC=cel2D.zD;
			double zDatumAV=(*vizinho[i]).zD;
			termMat=(cordaArea/cel2D.modE[i])*(zDatumAV-zDatumAC);
			TL[0]-=1*rhoaF[i]*darcyAF[i]*(rhoaF[i]*9.82/98066.22)*((escalGradArea-termoCorda*cordaArea)+termMat);

			termoCorda=escalar(gradMedPCAO,cel2D.vecE[i],cel2D.dim);
			//escalGradArea=escalar(gradMedPCAO,cel2D.sFace[i],cel2D.dim);
			escalGradArea=escalar(gradMedPCAO,cel2D.vecSDif[i],cel2D.dim);
			termMat=(cordaArea/cel2D.modE[i])*((*vizinho[i]).pcAO-cel2D.pcAO);
			TL[0]-=1*rhoaF[i]*darcyAF[i]*((escalGradArea-termoCorda*cordaArea)+termMat);
		}
		else{
			int diri=0;
			int rich=0;
			int vn=0;
			int kcc=0;
			int acoplado;
			tipoCC(i, diri, vn, rich,acoplado,kcc);
		    int ind;
		    double raz;
			double mio;
			//if(presF[i]<cel2D.pBolha-0.01)
				mio=flup.ViscOleo(cel2D.presC, tRes)/1000.;
			//else mio=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;
			double mig=flup.ViscGas(cel2D.presC, tRes)/1000.;
			double mia=flup.VisAgua(tRes)/1000.;
			double kO=fkO(cel2D.sWC, cel2D.alfC);
			double kA=interpolaTabela(kRelOACel.npont, cel2D.sWC, kRelOACel.satW, kRelOACel.permRelW);
			//double darcyO=kO*cel2D.kabsol*98066.22/((1.-cel2D.alfC)*mio+cel2D.alfC*mig);
			double darcyO=kO*98066.22/((1.-cel2D.alfC)*mio+cel2D.alfC*mig);
			//double darcyA=kA*cel2D.kabsol*98066.22/mia;
			double darcyA=kA*98066.22/mia;
			double condHarm=(((1.-cel2D.alfC)*cel2D.rhoP+cel2D.alfC*cel2D.rhogP)*darcyO+cel2D.rhoaP*darcyA);
			if(diri==1){

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenP[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				//double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
				//double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
				TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=(condHarm*cordaArea/cel2D.modE[i]);
				TL[0]+=termMat*ccPD[i];
				local.mx[0][diag]+=termMat;
			}
			else if(rich==1){

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenP[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termMat=(condHarm*cordaArea/cel2D.modE[i]);

				TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				TL[0]+=termMat*fonteTHR[i];
				local.mx[0][diag]+=termMat*(1.-coefTHRC[i]);
			}
			else if(vn==1 || acoplado==1){
				TL[0]+=ccPVN[i]*cel2D.sFaceMod[i];
			}
			/*if(diri==1){

				indraz(ind, raz, tempo, CC.ccDir[kcc].nserie, CC.ccDir[kcc].tempo);
				double inf = CC.ccDir[kcc].val[ind];
				double sup;
				if (ind < CC.ccDir[kcc].nserie - 1)
					sup = CC.ccDir[kcc].val[ind + 1];
				else
					sup = CC.ccDir[kcc].val[ind];
				ccTD[i] = (inf * raz + (1 - raz) * sup);


				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double condHarm=cel2D.cond;
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=(condHarm*cordaArea/cel2D.modE[i]);
				TL[0]+=termMat*ccTD[i];
				local.mx[0][diag]+=termMat;
			}
			else if(vn==1){

				indraz(ind, raz, tempo, CC.ccVN[kcc].nserie, CC.ccVN[kcc].tempo);
				double inf = CC.ccVN[kcc].val[ind];
				double sup;
				if (ind < CC.ccVN[kcc].nserie - 1)
					sup = CC.ccVN[kcc].val[ind + 1];
				else
					sup = CC.ccVN[kcc].val[ind];
				ccTVN[i] = (inf * raz + (1 - raz) * sup);

				TL[0]+=ccTVN[i];
			}
			else if(rich==1){

				indraz(ind, raz, tempo, CC.ccRic[kcc].nserie, CC.ccRic[kcc].tempo);
				double inf = CC.ccRic[kcc].valAmb[ind];
				double sup;
				if (ind < CC.ccRic[kcc].nserie - 1)
					sup = CC.ccRic[kcc].valAmb[ind + 1];
				else
					sup = CC.ccRic[kcc].valAmb[ind];
				ccTambR[i] = (inf * raz + (1 - raz) * sup);

				inf = CC.ccRic[kcc].hAmb[ind];
				if (ind < CC.ccRic[kcc].nserie - 1)
					sup = CC.ccRic[kcc].hAmb[ind + 1];
				else
					sup = CC.ccRic[kcc].hAmb[ind];
				ccHR[i] = (inf * raz + (1 - raz) * sup);

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double num=ccHR[i]*(cel2D.cond*cordaArea/cel2D.modE[i])*cel2D.sFaceMod[i];
				double denom=(cel2D.cond*cordaArea/cel2D.modE[i])+ccHR[i]*cel2D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double termLiv=-(ccHR[i]*cel2D.cond*cel2D.sFaceMod[i]*(escalGradArea-termoCorda*cordaArea)+num*ccTambR[i])/denom;
				TL[0]+=termLiv;
				local.mx[0][diag]-=num/denom;
			}*/
		}
	}
	if(/*perm==0 &&*/ trans==1){


		  double poro0=cel2D.poro*(1-cel2D.compRoc*(cel2D.presC0-pRes));
		  double poro=cel2D.poro*(1-cel2D.compRoc*(cel2D.presCI-pRes));
		  double multTot;
		  if(fabsl((cel2D.presCI-cel2D.presC0)/cel2D.presC0)<1e-3){
			  double dporo=-cel2D.poro*cel2D.compRoc;
			  double porod=cel2D.poro*(1-cel2D.compRoc*(cel2D.presC0*1.0001-pRes));
			  double dPoroRhoo=0;
			  double dPoroRhow=0;
			  double dPoroRhog=0;
			  double pmedT=0.5*(cel2D.presC0+cel2D.presC0);

				/*
				if(cel2D.tipoModelOleo==1)
					rhoF[i]=flup.MasEspoleo(presF[i], tRes);
				else
					rhoF[i]=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-presF[i]));
				rhogF[i]=flup.MasEspGas(presF[i], tRes);
				rhoaF[i]=flup.MasEspAgua(presF[i], tRes);
				if(cel2D.tipoModelOleo==1){
					double tit=flup.FracMassHidra(presF[i], tRes);
					alfF[i]=(tit/rhogF[i])/((tit/rhogF[i])+((1.-tit)/rhoF[i]));
				}
				else alfF[i]=0.;
				if(presF[i]<cel2D.pBolha-0.01)mioF[i]=flup.ViscOleo(presF[i], tRes)/1000.;
				else mioF[i]=flup.ViscOleo(cel2D.pBolha-0.01, tRes)/1000.;

				*/


			  double rhood;
			  if(cel2D.tipoModelOleo==1)
					rhood=flup.MasEspoleo(pmedT*1.00001, tRes);
			  else
					rhood=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-pmedT*1.00001));
			  double rhogd=flup.MasEspGas(pmedT*1.00001, tRes);
			  double titd;
			  double alfd;
			  if(cel2D.tipoModelOleo==1){
					titd=flup.FracMassHidra(pmedT*1.00001, tRes);
					alfd=(titd/rhogd)/((titd/rhogd)+((1.-titd)/rhood));
			  }
			  else alfd=0.;
			  double rhooe=flup.MasEspoleo(pmedT*(1-0.00001), tRes);
			  if(cel2D.tipoModelOleo==1)
					rhooe=flup.MasEspoleo(pmedT*(1-0.00001), tRes);
			  else
					rhooe=cel2D.rhoRef*(1-cel2D.compOleo*(pRes-pmedT*(1-0.00001)));
			  double rhoge=flup.MasEspGas(pmedT*(1-0.00001), tRes);
			  double tite;
			  double alfe;
			  if(cel2D.tipoModelOleo==1){
				  tite=flup.FracMassHidra(pmedT*(1-0.00001), tRes);
				  alfe=(tite/rhoge)/((tite/rhoge)+((1.-tite)/rhooe));
			  }
			  else alfe=0;

			  dPoroRhoo=dporo*((1.-cel2D.alfC)*cel2D.rhoP+cel2D.alfC*cel2D.rhogP)+
					    poro0*(((1-alfd)*rhood+alfd*rhogd)-((1.-alfe)*rhooe+alfe*rhoge))/(pmedT*0.00002);
			  dPoroRhog=(porod*rhogd-poro0*cel2D.rhogP)/(cel2D.presC0*0.0001);
			  dPoroRhog=dporo*(cel2D.rhogP)+
					    poro0*((rhogd)-(cel2D.rhogP))/(cel2D.presC0*0.0001);

			  dPoroRhow=dporo*(cel2D.rhoaP)+
					    poro0*flup.DMasEspAgua(pmedT, tRes);

			  multTot=-((cel2D.sLC-cel2D.sWC-0*satConata)*dPoroRhoo+(1.-cel2D.sLC)*dPoroRhog+
			  				  	  	  	  (cel2D.sWC-0.*satConata)*dPoroRhow)*cel2D.vElem/dt;
		  }
		  else{
			  double poro0B=poro*(1-cel2D.compRoc*(cel2D.presC0-pRes));
			  double poro1B=poro*(1-cel2D.compRoc*(cel2D.presCI-pRes));
			  double rho0B=cel2D.rhoP0;
			  double rhog0B=cel2D.rhogP0;
			  double rhoa0B=cel2D.rhoaP0;
			  double alf0B=cel2D.alfC0;
			  multTot=-((cel2D.sLC-cel2D.sWC-0*satConata)*(poro1B*((1.-cel2D.alfC)*cel2D.rhoP+cel2D.alfC*cel2D.rhogP)-
					  poro0B*((1.-alf0B)*rho0B+alf0B*rhog0B))/(cel2D.presCI-cel2D.presC0)+
					  (cel2D.sWC-0.*satConata)*(poro1B*cel2D.rhoaP-poro0B*rhoa0B)/(cel2D.presCI-cel2D.presC0))*cel2D.vElem/dt;
		  }



		  /*double poro0=cel2D.poro*(1-cel2D.compRoc*(cel2D.presC0-pRes));
		  double dporo=-cel2D.poro*cel2D.compRoc;
		  double porod=cel2D.poro*(1-cel2D.compRoc*(cel2D.presC0*1.0001-pRes));
		  double dPoroRhoo=0;
		  double dPoroRhow=0;
		  double dPoroRhog=0;
		  double pmedT=0.5*(cel2D.presC0+cel2D.presC0);
		  double rhood=flup.MasEspoleo(pmedT*1.00001, tRes);
		  double rhogd=flup.MasEspGas(pmedT*1.00001, tRes);
		  double titd=flup.FracMassHidra(pmedT*1.00001, tRes);
		  double alfd=(titd/rhogd)/((titd/rhogd)+((1.-titd)/rhood));
		  double rhooe=flup.MasEspoleo(pmedT*(1-0.00001), tRes);
		  double rhoge=flup.MasEspGas(pmedT*(1-0.00001), tRes);
		  double tite=flup.FracMassHidra(pmedT*(1-0.00001), tRes);
		  double alfe=(tite/rhoge)/((tite/rhoge)+((1.-tite)/rhooe));

		  dPoroRhoo=dporo*((1.-cel2D.alfC)*cel2D.rhoP+cel2D.alfC*cel2D.rhogP)+
				    poro0*(((1-alfd)*rhood+alfd*rhogd)-((1.-alfe)*rhooe+alfe*rhoge))/(pmedT*0.00002);
		  dPoroRhog=(porod*rhogd-poro0*cel2D.rhogP)/(cel2D.presC0*0.0001);
		  dPoroRhog=dporo*(cel2D.rhogP)+
				    poro0*((rhogd)-(cel2D.rhogP))/(cel2D.presC0*0.0001);
		  dPoroRhow=dporo*(cel2D.rhoaP)+
				    poro0*flup.DMasEspAgua(pmedT, tRes);
		  double multTot=-((cel2D.sLC-cel2D.sWC-0*satConata)*dPoroRhoo+(1.-cel2D.sLC)*dPoroRhog+
				  	  	  	  (cel2D.sWC-0.*satConata)*dPoroRhow)*cel2D.vElem/dt;*/

		  double multTO=((1.-cel2D.alfC)*cel2D.rhoP+cel2D.alfC*cel2D.rhogP)*(cel2D.sLC-cel2D.sWC-cel2D.sLI+cel2D.sWI);
		  double multTG=cel2D.rhogP*(-cel2D.sLC+cel2D.sLI);
		  double multTA=cel2D.rhoaP*(cel2D.sWC-cel2D.sWI);
		  double termoSat=-1*poro0*(multTO+multTG+multTA)*cel2D.vElem/dt;


		TL[0]+=(multTot*cel2D.presC0+FonteT*cel2D.vElem-termoSat);
		local.mx[0][diag]+=(multTot-cel2D.vElem*deriFonteT);
	}
	double acPres=local.mx[0][diag];
	local.mx[0][diag]=acPres/relax;
	TL[0]+=((1-relax)/relax)*acPres*cel2D.presCI;
	/*if(isnan(TL[0])){
		int para;
		para=0;
	}*/
}

