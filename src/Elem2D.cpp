/*
 * Elem2D.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Elem2D.h"



elem2d::elem2d(varGlob1D* Vvg1dSP,double** xcoor, int** noEle,int* tipo,double* atributo,int nVert, int nele, int nno, int i
		,double vdt, int vperm, int vtrans,ProFluColVF vflucVF,
		double vpres,double vu,double vv,double vt,double vfluxCal,double vcond,
		double vcp,double vrho,double vvisc,double vbeta/*,int vpartidaVF,double vrelax,double vrelaxPcor,
		int vnvfHR,int vnvfHRHol,int vatrasaHR,double vgrav,detTempo vtemp,detCC vCC*/) :
				TLUV(1), localUV(2,nVert+1),TLU(1), localU(1,nVert+1),TLV(1), localV(1,nVert+1),TLPCor(1), localPCor(1,nVert+1),
				TLT(1), localT(1,nVert+1) {
	vizinho=0;
	kvizinho=0;
	nvizinho=0;
	cel2D.presC=vpres;
	cel2D.presCEx=vpres;
	cel2D.presCcor=0.;
	cel2D.presC0=vpres;
	cel2D.presCI=vpres;
	cel2D.presCIcor=0.;
	cel2D.uC=vu;
	cel2D.uCEx=vu;
	cel2D.uC0=vu;
	cel2D.uCI=vu;
	cel2D.vC=vv;
	cel2D.vCEx=vv;
	cel2D.vC0=vv;
	cel2D.vCI=vv;
	cel2D.tempC=vt;
	cel2D.tempCI=vt;
	cel2D.tempC0=vt;
	cel2D.tCEx=vt;
	cel2D.a0U=0.;
	cel2D.a0V=0.;
	deriFonteMass=0.;
	FonteMass=0.;
	fluxCal=vfluxCal;
	cel2D.cond=vcond;
	cel2D.rho=vrho;
	cel2D.cp=vcp;
	cel2D.visc=vvisc;
	cel2D.beta=vbeta;
	cel2D.tempRef=0.;
	cel2D.taxaDeform=0.;
	//cel2D.B2Med=grav*cel2D.rho;
	dt=vdt;
	perm=vperm;
	trans=vtrans;
	ccInU=0;
	ccInV=0;
	ccWvel=0;
	ccPres=0;
	ccWall=0;
	//////////////////////////////////////////////////////////////////
	ccTD=0;
	ccTVN=0;
	ccHR=0;
	ccTambR=0;
	DCCN=0.;
	extrapSuaveCCN=0.;
	/////////////////////////////////////////////////////////////////
	massTot=0.;

	residuoU=0.;
	residuoV=0.;
	denoResU=1.;
	denoResV=1.;
	residuoT=0.;
	denoResT=1.;
	dtCFL=0;
	CFL=0;
	double CFLCTemp=0.;
	if(Vvg1dSP!=0)CFLCTemp=(*Vvg1dSP).cflG;
	CFLC=CFLCTemp;
	vg1dSP=Vvg1dSP;
	flucVF=vflucVF;

    /*temp=vtemp;
    CC=vCC;
    partidaVF=vpartidaVF;
    relax=vrelax;
    relaxPcor=vrelaxPcor;
    nvfHR=vnvfHR;
    nvfHRHol=vnvfHRHol;
    atrasaHR=vatrasaHR;
    grav=vgrav;*/

	if(nele>0){
		cel2D.noFace=0;
    	cel2D.dim=2;
    	cel2D.nvert=3;
    	cel2D.indEle=i;
    	cel2D.nele=nele;
    	dtCFL=new double [cel2D.nvert];
    	CFL=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)CFL[j]=(*vg1dSP).cflG;
    	confinado=new int [cel2D.nvert];
    	presF=new double [cel2D.nvert];
    	presFcor=new double [cel2D.nvert];
    	gradPresface=new double* [cel2D.nvert];
    	gradPresfaceCor=new double* [cel2D.nvert];
    	uF=new double [cel2D.nvert];
    	gradUface=new double* [cel2D.nvert];
    	vF=new double [cel2D.nvert];
    	tempF=new double [cel2D.nvert];
    	holF=new double [cel2D.nvert];
    	tUpw=new double [cel2D.nvert];
    	uUpw=new double [cel2D.nvert];
    	vUpw=new double [cel2D.nvert];
    	holUpw=new double [cel2D.nvert];
    	tMed=new double [cel2D.nvert];
    	uMed=new double [cel2D.nvert];
    	vMed=new double [cel2D.nvert];
    	holMed=new double [cel2D.nvert];
    	tDow=new double [cel2D.nvert];
    	uDow=new double [cel2D.nvert];
    	vDow=new double [cel2D.nvert];
    	holDow=new double [cel2D.nvert];
    	tK=new double [cel2D.nvert];
    	uK=new double [cel2D.nvert];
    	vK=new double [cel2D.nvert];
    	holK=new double [cel2D.nvert];
    	tL=new double [cel2D.nvert];
    	uL=new double [cel2D.nvert];
    	vL=new double [cel2D.nvert];
    	holL=new double [cel2D.nvert];
    	uRC=new double [cel2D.nvert];
    	vRC=new double [cel2D.nvert];
    	uRC0=new double [cel2D.nvert];
    	vRC0=new double [cel2D.nvert];
    	gradVface=new double* [cel2D.nvert];
    	massF=new double [cel2D.nvert];
    	taxadeformFace=new double [cel2D.nvert];
    	viscFace=new double [cel2D.nvert];
    	coefUHRC=new double [cel2D.nvert];
    	coefUHRV=new double [cel2D.nvert];
    	fonteUHR=new double [cel2D.nvert];
    	coefVHRC=new double [cel2D.nvert];
    	coefVHRV=new double [cel2D.nvert];
    	fonteVHR=new double [cel2D.nvert];
    	coefTHRC=new double [cel2D.nvert];
    	coefTHRV=new double [cel2D.nvert];
    	fonteTHR=new double [cel2D.nvert];
    	cel2D.difuPres=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.difuPres[j]=0.;
    	cel2D.difuPresRC=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.difuPresRC[j]=0.;
    	cel2D.vecSDif=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.vecSDif[j]=new double [cel2D.dim];
    	cel2D.vecSDifMod=new double [cel2D.nvert];
    	cel2D.HcU=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcU[j]=0.;
    	cel2D.HcV=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcV[j]=0.;
    	cel2D.HcP=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcP[j]=0.;
    	cel2D.HcT=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcT[j]=0.;
    	for(int j=0; j<cel2D.nvert;j++)confinado[j]=0.;
    	ccInU=new double [cel2D.nvert];
    	ccInV=new double [cel2D.nvert];
    	ccWvel=new double [cel2D.nvert];
    	ccPres=new double [cel2D.nvert];
    	ccWall=new double [cel2D.nvert];
    	////////////////////////////////////////////////////////////////////////////
    	ccTD=new double [cel2D.nvert];
    	ccTVN=new double [cel2D.nvert];
    	ccHR=new double [cel2D.nvert];
    	ccTambR=new double [cel2D.nvert];
    	////////////////////////////////////////////////////////////////////////////
    	for(int j=0; j<cel2D.nvert;j++) gradPresface[j]=new double [cel2D.dim];
    	for(int j=0; j<cel2D.nvert;j++) gradPresfaceCor[j]=new double [cel2D.dim];
    	cel2D.gradGreenPres=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPres[j]=0.;
    	cel2D.gradGreenPresCor=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPresCor[j]=0.;
    	for(int j=0; j<cel2D.nvert;j++) gradVface[j]=new double [cel2D.dim];
    	cel2D.gradGreenV=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenV[j]=0.;
    	for(int j=0; j<cel2D.nvert;j++) gradUface[j]=new double [cel2D.dim];
    	cel2D.gradGreenU=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenU[j]=0.;
    	cel2D.gradGreenTemp=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTemp[j]=0.;
    	cel2D.gradGreenHol=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenHol[j]=0.;

       	cel2D.gradGreenPresI=new double [cel2D.dim];
        for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPresI[j]=0.;
        cel2D.gradGreenPresCorI=new double [cel2D.dim];
        for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenPresCorI[j]=0.;
        cel2D.gradGreenVI=new double [cel2D.dim];
        for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenVI[j]=0.;
        cel2D.gradGreenUI=new double [cel2D.dim];
        for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenUI[j]=0.;
        cel2D.gradGreenTempI=new double [cel2D.dim];
        for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTempI[j]=0.;
        cel2D.gradGreenHolI=new double [cel2D.dim];
        for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenHolI[j]=0.;
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
    	cel2D.subVol=new double [cel2D.nvert];
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
        cel2D.B2Med=new double [cel2D.dim];
        cel2D.B2Med[1]=sinl((*vg1dSP).angY)*(*vg1dSP).gravVF*cel2D.rho;
        cel2D.B2Med[0]=sinl((*vg1dSP).angX)*(*vg1dSP).gravVF*cel2D.rho;
        cel2D.BMedF=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++){
        	cel2D.BMedF[j]=new double [cel2D.dim];
        	cel2D.BMedF[j][0]=cel2D.B2Med[0];
			cel2D.BMedF[j][1]=cel2D.B2Med[1];
        }
    	vizinho=new elemento* [cel2D.nvert];
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
    		for(int k=0; k<cel2D.nvert; k++){
    			if(k<cel2D.nvert-1){
    				cel2D.subVol[k]=0.5*fabsl((cel2D.coordVert[k][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k+1][1]-cel2D.centroElem[1])-(cel2D.coordVert[k+1][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k][1]-cel2D.centroElem[1]));
    			}
    			else{
    				cel2D.subVol[k]=0.5*fabsl((cel2D.coordVert[k][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[0][1]-cel2D.centroElem[1])-(cel2D.coordVert[0][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k][1]-cel2D.centroElem[1]));
    			}
    		}
    	}
    	else{
    		double sT[cel2D.nvert];
    		double centroT[cel2D.nvert][2];
    		cel2D.vElem=0.;
    		for(int k=0; k<cel2D.nvert; k++){
    			if(k<cel2D.nvert-1){
    				cel2D.subVol[k]=sT[k]=0.5*fabsl((cel2D.coordVert[k][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k+1][1]-cel2D.centroElem[1])-(cel2D.coordVert[k+1][0]-cel2D.centroElem[0])*
    					(cel2D.coordVert[k][1]-cel2D.centroElem[1]));
    				centroT[k][0]=(cel2D.coordVert[k][0]+cel2D.coordVert[k+1][0]+cel2D.centroElem[0])/3.;
    				centroT[k][1]=(cel2D.coordVert[k][1]+cel2D.coordVert[k+1][1]+cel2D.centroElem[1])/3.;
    			}
    			else{
    				cel2D.subVol[k]=sT[k]=0.5*fabsl((cel2D.coordVert[k][0]-cel2D.centroElem[0])*
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
		cel2D.subVol=0;
		cel2D.ownFace=0;
		cel2D.gradGreenPres=0;
		cel2D.gradGreenPresCor=0;
		cel2D.gradGreenU=0;
		cel2D.gradGreenV=0;
		cel2D.gradGreenTemp=0;
		cel2D.gradGreenHol=0;
		cel2D.gradGreenPresI=0;
		cel2D.gradGreenPresCorI=0;
		cel2D.gradGreenUI=0;
		cel2D.gradGreenVI=0;
		cel2D.gradGreenTempI=0;
		cel2D.gradGreenHolI=0;
		presF=0;
		presFcor=0;
		gradPresface=0;
		gradPresfaceCor=0;
		uF=0;
		gradUface=0;
		vF=0;
		tempF=0;
		holF=0;
		tUpw=0;
		uUpw=0;
		vUpw=0;
		holUpw=0;
		tMed=0;
		uMed=0;
		vMed=0;
		holMed=0;
		tDow=0;
		uDow=0;
		vDow=0;
		holDow=0;
		tK=0;
		uK=0;
		vK=0;
		holK=0;
		tL=0;
		uL=0;
		vL=0;
		holL=0;
		uRC=0;
		vRC=0;
		uRC0=0;
		vRC0=0;
		massF=0;
		taxadeformFace=0;
		viscFace=0;
    	coefUHRC=0;
    	coefUHRV=0;
    	fonteUHR=0;
    	coefVHRC=0;
    	coefVHRV=0;
    	fonteVHR=0;
    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;
		gradVface=0;
		cel2D.difuPres=0;
		cel2D.difuPresRC=0;
		cel2D.vecSDif=0;
		cel2D.vecSDifMod=0;
		cel2D.HcU=0;
		cel2D.HcV=0;
		cel2D.HcP=0;
		cel2D.HcT=0;
		cel2D.B2Med=0;
		cel2D.BMedF=0;
		confinado=0;
		dtCFL=0;
		CFL=0;
	}
}
elem2d::elem2d(const elem2d& velem) :
			TLUV(1), localUV(2,velem.cel2D.nvert+1),
			TLU(1), localU(1,velem.cel2D.nvert+1),
			TLV(1), localV(1,velem.cel2D.nvert+1),
			TLPCor(1), localPCor(1,velem.cel2D.nvert+1),
			TLT(1), localT(1,velem.cel2D.nvert+1){
	cel2D.nvert=velem.cel2D.nvert;
	cel2D.dim=velem.cel2D.dim;
	cel2D.indEle=velem.cel2D.indEle;
	cel2D.nele=velem.cel2D.nele;
	nvizinho=velem.nvizinho;
	cel2D.presC=velem.cel2D.presC;
	cel2D.presCEx=velem.cel2D.presCEx;
	cel2D.presCcor=velem.cel2D.presCcor;
	cel2D.presC0=velem.cel2D.presC0;
	cel2D.presCI=velem.cel2D.presCI;
	cel2D.presCIcor=velem.cel2D.presCIcor;
	cel2D.uC=velem.cel2D.uC;
	cel2D.uCEx=velem.cel2D.uCEx;
	cel2D.uC0=velem.cel2D.uC0;
	cel2D.uCI=velem.cel2D.uCI;
	cel2D.vC=velem.cel2D.vC;
	cel2D.vCEx=velem.cel2D.vCEx;
	cel2D.vC0=velem.cel2D.vC0;
	cel2D.vCI=velem.cel2D.vCI;
	cel2D.tempC=velem.cel2D.tempC;
	cel2D.tempC0=velem.cel2D.tempC0;
	cel2D.tempCI=velem.cel2D.tempCI;
	cel2D.tCEx=velem.cel2D.tCEx;
	deriFonteMass=velem.deriFonteMass;
	FonteMass=velem.FonteMass;
	fluxCal=velem.fluxCal;
	cel2D.cond=velem.cel2D.cond;
	cel2D.rho=velem.cel2D.rho;
	cel2D.cp=velem.cel2D.cp;
	cel2D.visc=velem.cel2D.visc;
	cel2D.beta=velem.cel2D.beta;
	cel2D.tempRef=velem.cel2D.tempRef;
	cel2D.a0U=velem.cel2D.a0U;
	cel2D.a0V=velem.cel2D.a0V;
	cel2D.taxaDeform=velem.cel2D.taxaDeform;
	//cel2D.B2Med=velem.cel2D.B2Med;
	dt=velem.dt;
	perm=velem.perm;
	trans=velem.trans;
	massTot=velem.massTot;
	//ccTD=velem.ccTD;
	//ccTVN=velem.ccTVN;
	//ccHR=velem.ccHR;
	//ccTambR=velem.ccTambR;
	residuoU=velem.residuoU;
	residuoV=velem.residuoV;
	denoResU=velem.denoResU;
	denoResV=velem.denoResV;
	residuoT=velem.residuoT;
	denoResT=velem.denoResT;
	CFLC=velem.CFLC;
	vg1dSP=velem.vg1dSP;
	flucVF=velem.flucVF;

	DCCN=velem.DCCN;
	extrapSuaveCCN=velem.extrapSuaveCCN;

    /*temp=velem.temp;
    CC=velem.CC;
    partidaVF=velem.partidaVF;
    relax=velem.relax;
    relaxPcor=velem.relaxPcor;
    nvfHR=velem.nvfHR;
    nvfHRHol=velem.nvfHRHol;
    atrasaHR=velem.atrasaHR;
    grav=velem.grav;*/

	if(cel2D.nvert>0){
    	vizinho=new elemento* [cel2D.nvert];
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
		TLUV=velem.TLUV;
		localUV=velem.localUV;
		TLU=velem.TLU;
		localU=velem.localU;
		TLV=velem.TLV;
		localV=velem.localV;
		TLT=velem.TLT;
		localT=velem.localT;
		cel2D.noFace=0;
		confinado=new int [cel2D.nvert];
		dtCFL=new double [cel2D.nvert];
		CFL=new double [cel2D.nvert];
    	presF=new double [cel2D.nvert];
    	presFcor=new double [cel2D.nvert];
    	gradPresface=new double* [cel2D.nvert];
    	gradPresfaceCor=new double* [cel2D.nvert];
    	uF=new double [cel2D.nvert];
    	gradUface=new double* [cel2D.nvert];
    	vF=new double [cel2D.nvert];
    	tempF=new double [cel2D.nvert];
    	holF=new double [cel2D.nvert];
    	tUpw=new double [cel2D.nvert];
    	uUpw=new double [cel2D.nvert];
    	vUpw=new double [cel2D.nvert];
    	holUpw=new double [cel2D.nvert];
    	tMed=new double [cel2D.nvert];
    	uMed=new double [cel2D.nvert];
    	vMed=new double [cel2D.nvert];
    	holMed=new double [cel2D.nvert];
    	tDow=new double [cel2D.nvert];
    	uDow=new double [cel2D.nvert];
    	vDow=new double [cel2D.nvert];
    	holDow=new double [cel2D.nvert];
    	tK=new double [cel2D.nvert];
    	uK=new double [cel2D.nvert];
    	vK=new double [cel2D.nvert];
    	holK=new double [cel2D.nvert];
    	tL=new double [cel2D.nvert];
    	uL=new double [cel2D.nvert];
    	vL=new double [cel2D.nvert];
    	holL=new double [cel2D.nvert];
    	uRC=new double [cel2D.nvert];
    	vRC=new double [cel2D.nvert];
    	uRC0=new double [cel2D.nvert];
    	vRC0=new double [cel2D.nvert];
    	gradVface=new double* [cel2D.nvert];
    	massF=new double [cel2D.nvert];
    	taxadeformFace=new double [cel2D.nvert];
    	viscFace=new double [cel2D.nvert];
    	coefUHRC=new double [cel2D.nvert];
    	coefUHRV=new double [cel2D.nvert];
    	fonteUHR=new double [cel2D.nvert];
    	coefVHRC=new double [cel2D.nvert];
    	coefVHRV=new double [cel2D.nvert];
    	fonteVHR=new double [cel2D.nvert];
    	coefTHRC=new double [cel2D.nvert];
    	coefTHRV=new double [cel2D.nvert];
    	fonteTHR=new double [cel2D.nvert];
    	cel2D.difuPres=new double [cel2D.dim];
    	cel2D.difuPresRC=new double [cel2D.dim];
    	cel2D.vecSDif=new double* [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) cel2D.vecSDif[j]=new double [cel2D.dim];
    	cel2D.vecSDifMod=new double [cel2D.nvert];
    	cel2D.HcU=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcU[j]=0.;
    	cel2D.HcV=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcV[j]=0.;
    	cel2D.HcP=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcP[j]=0.;
    	cel2D.HcT=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcT[j]=0.;
    	for(int j=0; j<cel2D.nvert;j++)confinado[j]=velem.confinado[j];
    	ccInU=new double [cel2D.nvert];
    	ccInV=new double [cel2D.nvert];
    	ccWvel=new double [cel2D.nvert];
    	ccPres=new double [cel2D.nvert];
    	ccWall=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++){
        	ccInU[j]=-1;
        	ccInV[j]=-1;
        	ccWvel[j]=-1;
        	ccPres[j]=-1;
        	ccWall[j]=-1;
    	}
    	///////////////////////////////////////////////////////////////////////////////////////
    	ccTD=new double [cel2D.nvert];
    	ccTVN=new double [cel2D.nvert];
    	ccHR=new double [cel2D.nvert];
    	ccTambR=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++){
    		ccTD[j]=-1;
    		ccTVN[j]=-1;
    		ccHR[j]=-1;
    		ccTambR[j]=-1;
    	}
    	/////////////////////////////////////////////////////////////////////////////////////////
    	for(int j=0; j<cel2D.nvert;j++) gradPresface[j]=new double [cel2D.dim];
    	for(int j=0; j<cel2D.nvert;j++) gradPresfaceCor[j]=new double [cel2D.dim];
    	cel2D.gradGreenPres=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPres[j]=velem.cel2D.gradGreenPres[j];
    	cel2D.gradGreenPresCor=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPresCor[j]=velem.cel2D.gradGreenPresCor[j];
    	for(int j=0; j<cel2D.nvert;j++) gradUface[j]=new double [cel2D.dim];
    	cel2D.gradGreenU=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenU[j]=velem.cel2D.gradGreenU[j];
    	for(int j=0; j<cel2D.nvert;j++) gradVface[j]=new double [cel2D.dim];
    	cel2D.gradGreenV=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenV[j]=velem.cel2D.gradGreenV[j];
    	cel2D.gradGreenTemp=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTemp[j]=velem.cel2D.gradGreenTemp[j];
    	cel2D.gradGreenHol=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenHol[j]=velem.cel2D.gradGreenHol[j];

    	cel2D.gradGreenPresI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPresI[j]=velem.cel2D.gradGreenPresI[j];
    	cel2D.gradGreenPresCorI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPresCorI[j]=velem.cel2D.gradGreenPresCorI[j];
    	cel2D.gradGreenUI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenUI[j]=velem.cel2D.gradGreenUI[j];
    	cel2D.gradGreenVI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenVI[j]=velem.cel2D.gradGreenVI[j];
    	cel2D.gradGreenTempI=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTempI[j]=velem.cel2D.gradGreenTempI[j];
    	cel2D.gradGreenHolI=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenHolI[j]=velem.cel2D.gradGreenHolI[j];
    	cel2D.B2Med=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++)  cel2D.B2Med[j]=velem.cel2D.B2Med[j];
        cel2D.BMedF=new double* [cel2D.nvert];
        for(int j=0; j<cel2D.nvert;j++){
        	cel2D.BMedF[j]=new double [cel2D.dim];
        	for(int k=0;k<cel2D.dim;k++)cel2D.BMedF[j][k]=velem.cel2D.BMedF[j][k];
        }
    	for(int i=0; i<cel2D.nvert;i++){
    		dtCFL[i]=velem.dtCFL[i];
    		CFL[i]=velem.CFL[i];
    		presF[i]=velem.presF[i];
        	presFcor[i]=velem.presFcor[i];
        	uF[i]=velem.uF[i];
        	vF[i]=velem.vF[i];
        	tempF[i]=velem.tempF[i];
        	holF[i]=velem.holF[i];
        	tUpw[i]=velem.tUpw[i];
        	uUpw[i]=velem.uUpw[i];
        	vUpw[i]=velem.vUpw[i];
        	holUpw[i]=velem.holUpw[i];
        	tMed[i]=velem.tMed[i];
        	uMed[i]=velem.uMed[i];
        	vMed[i]=velem.vMed[i];
        	holMed[i]=velem.holMed[i];
        	tDow[i]=velem.tDow[i];
        	uDow[i]=velem.uDow[i];
        	vDow[i]=velem.vDow[i];
        	holDow[i]=velem.holDow[i];
        	tK[i]=velem.tK[i];
        	uK[i]=velem.uK[i];
        	vK[i]=velem.vK[i];
        	holK[i]=velem.holK[i];
        	tL[i]=velem.tL[i];
        	uL[i]=velem.uL[i];
        	vL[i]=velem.vL[i];
        	holL[i]=velem.holL[i];
           	uRC[i]=velem.uRC[i];
            vRC[i]=velem.vRC[i];
           	uRC0[i]=velem.uRC0[i];
            vRC0[i]=velem.vRC0[i];
        	massF[i]=velem.massF[i];
        	taxadeformFace[i]=velem.taxadeformFace[i];
        	viscFace[i]=velem.viscFace[i];
        	coefUHRC[i]=velem.coefUHRC[i];
        	coefUHRV[i]=velem.coefUHRV[i];
        	fonteUHR[i]=velem.fonteUHR[i];
        	coefVHRC[i]=velem.coefVHRC[i];
        	coefVHRV[i]=velem.coefVHRV[i];
        	fonteVHR[i]=velem.fonteVHR[i];
        	coefTHRC[i]=velem.coefTHRC[i];
        	coefTHRV[i]=velem.coefTHRV[i];
        	fonteTHR[i]=velem.fonteTHR[i];
        	ccInU[i]=velem.ccInU[i];
        	ccInV[i]=velem.ccInV[i];
        	ccWvel[i]=velem.ccWvel[i];
        	ccPres[i]=velem.ccPres[i];
        	ccWall[i]=velem.ccWall[i];
        	////////////////////////////////////////////////////////////////////////////////
        	ccTD[i]=velem.ccTD[i];
        	ccTVN[i]=velem.ccTVN[i];
        	ccHR[i]=velem.ccHR[i];
        	ccTambR[i]=velem.ccTambR[i];
        	////////////////////////////////////////////////////////////////////////////////
        	for(int j=0;j<cel2D.dim;j++) gradPresface[i][j]=velem.gradPresface[i][j];
        	for(int j=0;j<cel2D.dim;j++) gradPresfaceCor[i][j]=velem.gradPresfaceCor[i][j];
        	for(int j=0;j<cel2D.dim;j++) gradUface[i][j]=velem.gradUface[i][j];
        	for(int j=0;j<cel2D.dim;j++) gradVface[i][j]=velem.gradVface[i][j];
        	for(int j=0;j<cel2D.dim;j++) cel2D.vecSDif[i][j]=velem.cel2D.vecSDif[i][j];
    	}
    	for(int j=0;j<cel2D.dim;j++)cel2D.difuPres[j]=velem.cel2D.difuPres[j];
    	for(int j=0;j<cel2D.dim;j++)cel2D.difuPresRC[j]=velem.cel2D.difuPresRC[j];
    	for(int j=0;j<cel2D.nvert;j++) cel2D.vecSDifMod[j]=velem.cel2D.vecSDifMod[j];
    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcU[j]=velem.cel2D.HcU[j];
    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcV[j]=velem.cel2D.HcV[j];
    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcP[j]=velem.cel2D.HcP[j];
    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcT[j]=velem.cel2D.HcT[j];
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
    	cel2D.subVol=new double [cel2D.nvert];
    	for(int i=0; i<cel2D.nvert;i++)cel2D.subVol[i]=velem.cel2D.subVol[i];
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
		cel2D.subVol=0;
		cel2D.ownFace=0;
		//////////////////////////////////////////////////////////////////////////////////////
        cel2D.vecE=0;
        cel2D.modE=0;
        cel2D.vecT=0;
        cel2D.modT=0;
        cel2D.fatG=0;
        cel2D.angES=0;
        cel2D.fInter=0;
        cel2D.fIfC=0;
        cel2D.gradGreenPres=0;
        cel2D.gradGreenPresCor=0;
        presF=0;
        presFcor=0;
        gradPresface=0;
        gradPresfaceCor=0;
        cel2D.gradGreenU=0;
        uF=0;
        gradUface=0;
        cel2D.gradGreenV=0;
        vF=0;
    	cel2D.gradGreenTemp=0;
    	tempF=0;
    	cel2D.gradGreenHol=0;

        cel2D.gradGreenPresI=0;
        cel2D.gradGreenPresCorI=0;
        cel2D.gradGreenUI=0;
        cel2D.gradGreenVI=0;
        cel2D.gradGreenTempI=0;
        cel2D.gradGreenHolI=0;

    	holF=0;
		tUpw=0;
		uUpw=0;
		vUpw=0;
		holUpw=0;
		tMed=0;
		uMed=0;
		vMed=0;
		holMed=0;
		tDow=0;
		uDow=0;
		vDow=0;
		holDow=0;
		tK=0;
		uK=0;
		vK=0;
		holK=0;
		tL=0;
		uL=0;
		vL=0;
		holL=0;
		uRC=0;
		vRC=0;
		uRC0=0;
		vRC0=0;
        massF=0;
        taxadeformFace=0;
        viscFace=0;
    	coefUHRC=0;
    	coefUHRV=0;
    	fonteUHR=0;
    	coefVHRC=0;
    	coefVHRV=0;
    	fonteVHR=0;
    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;
        gradVface=0;
    	ccInU=0;
    	ccInV=0;
    	ccWvel=0;
    	ccPres=0;
    	ccWall=0;
    	///////////////////////////////////////////////////////////////////////////
    	ccTD=0;
    	ccTVN=0;
    	ccHR=0;
    	ccTambR=0;
    	//////////////////////////////////////////////////////////////////////////
    	cel2D.difuPres=0;
    	cel2D.difuPresRC=0;
    	cel2D.vecSDif=0;
    	cel2D.vecSDifMod=0;
    	cel2D.HcU=0;
    	cel2D.HcV=0;
    	cel2D.HcP=0;
    	cel2D.HcT=0;
    	cel2D.B2Med=0;
    	cel2D.BMedF=0;
    	confinado=0;
    	dtCFL=0;
    	CFL=0;
	}
}

elem2d& elem2d::operator =(const elem2d& velem) {
	if (this != &velem) {
		if(cel2D.nvert>0 && cel2D.nele>0){
			delete[] cel2D.noElem;
			delete[] cel2D.indFace;
			delete[] cel2D.ccFace;
			delete[] cel2D.orientFace;
			delete[] cel2D.centroElem;
			delete[] cel2D.centroideElem;
			delete[] cel2D.dCFMod;
			delete[] cel2D.sFaceMod;
			delete[] cel2D.subVol;
			delete[] cel2D.ownFace;
			delete[] cel2D.vecSDifMod;
			delete[] cel2D.HcU;
			delete[] cel2D.HcV;
			delete[] cel2D.HcP;
			delete[] cel2D.HcT;
			delete[] ccInU;
			delete[] ccInV;
			delete[] ccWvel;
			delete[] ccPres;
			delete[] ccWall;
			/////////////////////////////////////////////////////////////////////////////////////////////////
			delete[] ccTD;
			delete[] ccTVN;
			delete[] ccHR;
			delete[] ccTambR;
			///////////////////////////////////////////////////////////////////////////////////////////////////
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
				delete[] gradPresface[i];
				delete[] gradPresfaceCor[i];
				delete[] gradUface[i];
				delete[] gradVface[i];
				///////////////////////////////////////////////////////////
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
			delete[] cel2D.gradGreenPres;
			delete[] cel2D.gradGreenPresCor;
			delete[] cel2D.gradGreenU;
			delete[] cel2D.gradGreenV;
			delete[] cel2D.gradGreenTemp;
			delete[] cel2D.gradGreenHol;

			delete[] cel2D.gradGreenPresI;
			delete[] cel2D.gradGreenPresCorI;
			delete[] cel2D.gradGreenUI;
			delete[] cel2D.gradGreenVI;
			delete[] cel2D.gradGreenTempI;
			delete[] cel2D.gradGreenHolI;

			//////////////////////////////////////////////////////////////////////////////////
			delete[] presF;
			delete[] presFcor;
			delete[] gradPresface;
			delete[] gradPresfaceCor;
			delete[] uF;
			delete[] gradUface;
			delete[] vF;
			delete[] tempF;
			delete[] holF;
			delete[] tUpw;
			delete[] uUpw;
			delete[] vUpw;
			delete[] holUpw;
			delete[] tMed;
			delete[] uMed;
			delete[] vMed;
			delete[] holMed;
			delete[] tDow;
			delete[] uDow;
			delete[] vDow;
			delete[] holDow;
			delete[] tK;
			delete[] uK;
			delete[] vK;
			delete[] holK;
			delete[] tL;
			delete[] uL;
			delete[] vL;
			delete[] holL;
			delete[] uRC;
			delete[] vRC;
			delete[] uRC0;
			delete[] vRC0;
			delete[] massF;
			delete[] taxadeformFace;
			delete[] viscFace;
			delete[] gradVface;

			delete[] coefUHRC;
			delete[] coefUHRV;
			delete[] fonteUHR;
			delete[] coefVHRC;
			delete[] coefVHRV;
			delete[] fonteVHR;
			delete[] coefTHRC;
			delete[] coefTHRV;
			delete[] fonteTHR;

			delete[] cel2D.difuPres;
			delete[] cel2D.difuPresRC;
			delete[] cel2D.vecSDif;
			for(int i=0;i<cel2D.nvert;i++){
				delete[] cel2D.BMedF[i];
			}
			delete[] cel2D.B2Med;
			delete[] cel2D.BMedF;
			delete[] confinado;
			delete[] dtCFL;
			delete[] CFL;
		}
		/*if(nvizinho>0){
			delete[] vizinho;
			delete[] kvizinho;
		}*/
		TLUV=velem.TLUV;
		localUV=velem.localUV;
		TLU=velem.TLU;
		localU=velem.localU;
		TLV=velem.TLV;
		localV=velem.localV;
		TLPCor=velem.TLPCor;
		localPCor=velem.localPCor;
		TLT=velem.TLT;
		localT=velem.localT;
		cel2D.nvert=velem.cel2D.nvert;
		cel2D.dim=velem.cel2D.dim;
		cel2D.indEle=velem.cel2D.indEle;
		cel2D.nele=velem.cel2D.nele;
		nvizinho=velem.nvizinho;
		cel2D.presC=velem.cel2D.presC;
		cel2D.presCEx=velem.cel2D.presCEx;
		cel2D.presCcor=velem.cel2D.presCcor;
		cel2D.presC0=velem.cel2D.presC0;
		cel2D.presCI=velem.cel2D.presCI;
		cel2D.presCIcor=velem.cel2D.presCIcor;
		cel2D.uC=velem.cel2D.uC;
		cel2D.uCEx=velem.cel2D.uCEx;
		cel2D.uC0=velem.cel2D.uC0;
		cel2D.uCI=velem.cel2D.uCI;
		cel2D.vC=velem.cel2D.vC;
		cel2D.vCEx=velem.cel2D.vCEx;
		cel2D.vC0=velem.cel2D.vC0;
		cel2D.vCI=velem.cel2D.vCI;
		cel2D.tempC=velem.cel2D.tempC;
		cel2D.tempC0=velem.cel2D.tempC0;
		cel2D.tempCI=velem.cel2D.tempCI;
		cel2D.tCEx=velem.cel2D.tCEx;
		deriFonteMass=velem.deriFonteMass;
		FonteMass=velem.FonteMass;
		fluxCal=velem.fluxCal;
		cel2D.cond=velem.cel2D.cond;
		cel2D.rho=velem.cel2D.rho;
		cel2D.cp=velem.cel2D.cp;
		cel2D.visc=velem.cel2D.visc;
		cel2D.beta=velem.cel2D.beta;
		cel2D.tempRef=velem.cel2D.tempRef;
		cel2D.a0U=velem.cel2D.a0U;
		cel2D.a0V=velem.cel2D.a0V;
		cel2D.taxaDeform=velem.cel2D.taxaDeform;
		//cel2D.B2Med=velem.cel2D.B2Med;
		dt=velem.dt;
		perm=velem.perm;
		trans=velem.trans;
		/*ccInU=velem.ccInU;
		ccInV=velem.ccInV;
		ccWvel=velem.ccWvel;
		ccPres=velem.ccPres;
		ccWall=velem.ccWall;*/
		massTot=velem.massTot;

		residuoU=velem.residuoU;
		residuoV=velem.residuoV;
		denoResU=velem.denoResU;
		denoResV=velem.denoResV;
		residuoT=velem.residuoT;
		denoResT=velem.denoResT;

		vg1dSP=velem.vg1dSP;
		CFLC=(*vg1dSP).cflG;
		flucVF=velem.flucVF;

		DCCN=velem.DCCN;
		extrapSuaveCCN=velem.extrapSuaveCCN;

	    /*temp=velem.temp;
	    CC=velem.CC;
	    partidaVF=velem.partidaVF;
	    relax=velem.relax;
	    relaxPcor=velem.relaxPcor;
	    nvfHR=velem.nvfHR;
	    nvfHRHol=velem.nvfHRHol;
	    atrasaHR=velem.atrasaHR;
	    grav=velem.grav;*/

		if(cel2D.nvert>0){
	    	vizinho=new elemento* [cel2D.nvert];
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
			cel2D.noFace=0;
			confinado=new int [cel2D.nvert];
			dtCFL=new double [cel2D.nvert];
			CFL=new double [cel2D.nvert];
	    	presF=new double [cel2D.nvert];
	    	presFcor=new double [cel2D.nvert];
	    	gradPresface=new double* [cel2D.nvert];
	    	gradPresfaceCor=new double* [cel2D.nvert];
	    	uF=new double [cel2D.nvert];
	    	gradUface=new double* [cel2D.nvert];
	    	vF=new double [cel2D.nvert];
	    	tempF=new double [cel2D.nvert];
	    	holF=new double [cel2D.nvert];
	    	gradVface=new double* [cel2D.nvert];
	    	tUpw=new double [cel2D.nvert];
	    	uUpw=new double [cel2D.nvert];
	    	vUpw=new double [cel2D.nvert];
	    	holUpw=new double [cel2D.nvert];
	    	tMed=new double [cel2D.nvert];
	    	uMed=new double [cel2D.nvert];
	    	vMed=new double [cel2D.nvert];
	    	holMed=new double [cel2D.nvert];
	    	tDow=new double [cel2D.nvert];
	    	uDow=new double [cel2D.nvert];
	    	vDow=new double [cel2D.nvert];
	    	holDow=new double [cel2D.nvert];
	    	tK=new double [cel2D.nvert];
	    	uK=new double [cel2D.nvert];
	    	vK=new double [cel2D.nvert];
	    	holK=new double [cel2D.nvert];
	    	tL=new double [cel2D.nvert];
	    	uL=new double [cel2D.nvert];
	    	vL=new double [cel2D.nvert];
	    	holL=new double [cel2D.nvert];
	    	uRC=new double [cel2D.nvert];
	    	vRC=new double [cel2D.nvert];
	    	uRC0=new double [cel2D.nvert];
	    	vRC0=new double [cel2D.nvert];
	    	massF=new double [cel2D.nvert];
	    	taxadeformFace=new double [cel2D.nvert];
	    	viscFace=new double [cel2D.nvert];
	    	coefUHRC=new double [cel2D.nvert];
	    	coefUHRV=new double [cel2D.nvert];
	    	fonteUHR=new double [cel2D.nvert];
	    	coefVHRC=new double [cel2D.nvert];
	    	coefVHRV=new double [cel2D.nvert];
	    	fonteVHR=new double [cel2D.nvert];
	    	coefTHRC=new double [cel2D.nvert];
	    	coefTHRV=new double [cel2D.nvert];
	    	fonteTHR=new double [cel2D.nvert];
	    	cel2D.difuPres=new double [cel2D.dim];
	    	cel2D.difuPresRC=new double [cel2D.dim];
	    	cel2D.vecSDif=new double* [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) cel2D.vecSDif[j]=new double [cel2D.dim];
	    	cel2D.vecSDifMod=new double [cel2D.nvert];
	    	cel2D.HcU=new double [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcU[j]=0.;
	    	cel2D.HcV=new double [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcV[j]=0.;
	    	cel2D.HcP=new double [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcP[j]=0.;
	    	cel2D.HcT=new double [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++)cel2D.HcT[j]=0.;
	    	for(int j=0; j<cel2D.nvert;j++)confinado[j]=velem.confinado[j];
	    	ccInU=new double [cel2D.nvert];
	    	ccInV=new double [cel2D.nvert];
	    	ccWvel=new double [cel2D.nvert];
	    	ccPres=new double [cel2D.nvert];
	    	ccWall=new double [cel2D.nvert];
	    	///////////////////////////////////////////////////////////////////////////////////////
	    	ccTD=new double [cel2D.nvert];
	    	ccTVN=new double [cel2D.nvert];
	    	ccHR=new double [cel2D.nvert];
	    	ccTambR=new double [cel2D.nvert];
	    	/////////////////////////////////////////////////////////////////////////////////////////
	    	for(int j=0; j<cel2D.nvert;j++) gradPresface[j]=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.nvert;j++) gradPresfaceCor[j]=new double [cel2D.dim];
	    	cel2D.gradGreenPres=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPres[j]=velem.cel2D.gradGreenPres[j];
	    	cel2D.gradGreenPresCor=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPresCor[j]=velem.cel2D.gradGreenPresCor[j];
	    	for(int j=0; j<cel2D.nvert;j++) gradUface[j]=new double [cel2D.dim];
	    	cel2D.gradGreenU=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenU[j]=velem.cel2D.gradGreenU[j];
	    	for(int j=0; j<cel2D.nvert;j++) gradVface[j]=new double [cel2D.dim];
	    	cel2D.gradGreenV=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenV[j]=velem.cel2D.gradGreenV[j];
	    	cel2D.gradGreenTemp=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTemp[j]=velem.cel2D.gradGreenTemp[j];
	    	cel2D.gradGreenHol=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenHol[j]=velem.cel2D.gradGreenHol[j];

	    	cel2D.gradGreenPresI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPresI[j]=velem.cel2D.gradGreenPresI[j];
	    	cel2D.gradGreenPresCorI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenPresCorI[j]=velem.cel2D.gradGreenPresCorI[j];
	    	cel2D.gradGreenUI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenUI[j]=velem.cel2D.gradGreenUI[j];
	    	cel2D.gradGreenVI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenVI[j]=velem.cel2D.gradGreenVI[j];
	    	cel2D.gradGreenTempI=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTempI[j]=velem.cel2D.gradGreenTempI[j];
	    	cel2D.gradGreenHolI=new double [cel2D.dim];
	    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenHolI[j]=velem.cel2D.gradGreenHolI[j];

	    	cel2D.B2Med=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++)  cel2D.B2Med[j]=velem.cel2D.B2Med[j];
	        cel2D.BMedF=new double* [cel2D.nvert];
	        for(int j=0; j<cel2D.nvert;j++){
	        	cel2D.BMedF[j]=new double [cel2D.dim];
	        	for(int k=0;k<cel2D.dim;k++)cel2D.BMedF[j][k]=velem.cel2D.BMedF[j][k];
	        }
	    	for(int i=0; i<cel2D.nvert;i++){
	    		dtCFL[i]=velem.dtCFL[i];
	    		CFL[i]=velem.CFL[i];
	        	presF[i]=velem.presF[i];
	        	presFcor[i]=velem.presFcor[i];
	        	uF[i]=velem.uF[i];
	        	vF[i]=velem.vF[i];
	        	tempF[i]=velem.tempF[i];
	        	holF[i]=velem.holF[i];
	        	tUpw[i]=velem.tUpw[i];
	        	uUpw[i]=velem.uUpw[i];
	        	vUpw[i]=velem.vUpw[i];
	        	holUpw[i]=velem.holUpw[i];
	        	tMed[i]=velem.tMed[i];
	        	uMed[i]=velem.uMed[i];
	        	vMed[i]=velem.vMed[i];
	        	holMed[i]=velem.holMed[i];
	        	tDow[i]=velem.tDow[i];
	        	uDow[i]=velem.uDow[i];
	        	vDow[i]=velem.vDow[i];
	        	holDow[i]=velem.holDow[i];
	        	tK[i]=velem.tK[i];
	        	uK[i]=velem.uK[i];
	        	vK[i]=velem.vK[i];
	        	holK[i]=velem.holK[i];
	        	tL[i]=velem.tL[i];
	        	uL[i]=velem.uL[i];
	        	vL[i]=velem.vL[i];
	        	holL[i]=velem.holL[i];
	        	uRC[i]=velem.uRC[i];
	        	vRC[i]=velem.vRC[i];
	        	uRC0[i]=velem.uRC0[i];
	        	vRC0[i]=velem.vRC0[i];
	        	massF[i]=velem.massF[i];
	        	taxadeformFace[i]=velem.taxadeformFace[i];
	        	viscFace[i]=velem.viscFace[i];
	        	coefUHRC[i]=velem.coefUHRC[i];
	        	coefUHRV[i]=velem.coefUHRV[i];
	        	fonteUHR[i]=velem.fonteUHR[i];
	        	coefVHRC[i]=velem.coefVHRC[i];
	        	coefVHRV[i]=velem.coefVHRV[i];
	        	fonteVHR[i]=velem.fonteVHR[i];
	        	coefTHRC[i]=velem.coefTHRC[i];
	        	coefTHRV[i]=velem.coefTHRV[i];
	        	fonteTHR[i]=velem.fonteTHR[i];
	        	ccInU[i]=velem.ccInU[i];
	        	ccInV[i]=velem.ccInV[i];
	        	ccWvel[i]=velem.ccWvel[i];
	        	ccPres[i]=velem.ccPres[i];
	        	ccWall[i]=velem.ccWall[i];
	        	////////////////////////////////////////////////////////////////////////////////
	        	ccTD[i]=velem.ccTD[i];
	        	ccTVN[i]=velem.ccTVN[i];
	        	ccHR[i]=velem.ccHR[i];
	        	ccTambR[i]=velem.ccTambR[i];
	        	///////////////////////////////////////////////////////////////////////////////
	        	for(int j=0;j<cel2D.dim;j++) gradPresface[i][j]=velem.gradPresface[i][j];
	        	for(int j=0;j<cel2D.dim;j++) gradPresfaceCor[i][j]=velem.gradPresfaceCor[i][j];
	        	for(int j=0;j<cel2D.dim;j++) gradUface[i][j]=velem.gradUface[i][j];
	        	for(int j=0;j<cel2D.dim;j++) gradVface[i][j]=velem.gradVface[i][j];
	        	for(int j=0;j<cel2D.dim;j++) cel2D.vecSDif[i][j]=velem.cel2D.vecSDif[i][j];
	    	}
	    	for(int j=0;j<cel2D.dim;j++)cel2D.difuPres[j]=velem.cel2D.difuPres[j];
	    	for(int j=0;j<cel2D.dim;j++)cel2D.difuPresRC[j]=velem.cel2D.difuPresRC[j];
	    	for(int j=0;j<cel2D.nvert;j++) cel2D.vecSDifMod[j]=velem.cel2D.vecSDifMod[j];
	    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcU[j]=velem.cel2D.HcU[j];
	    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcV[j]=velem.cel2D.HcV[j];
	    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcP[j]=velem.cel2D.HcP[j];
	    	for(int j=0;j<cel2D.nvert;j++)cel2D.HcT[j]=velem.cel2D.HcT[j];
	    	cel2D.noElem=new int [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.noElem[i]=velem.cel2D.noElem[i];
	    	cel2D.indFace=new int [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.indFace[i]=velem.cel2D.indFace[i];
	    	int size=velem.cel2D.indVizCres.size();
	    	cel2D.indVizCres.clear();
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
	    	cel2D.dCF=new double* [cel2D.nvert];for(int j=0; j<cel2D.nvert;j++) gradPresface[j]=new double [cel2D.dim];
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
	    	cel2D.subVol=new double [cel2D.nvert];
	    	for(int i=0; i<cel2D.nvert;i++)cel2D.subVol[i]=velem.cel2D.subVol[i];
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
				cel2D.subVol=0;
				cel2D.ownFace=0;
				//////////////////////////////////////////////////////////////////////////////////////
		        cel2D.vecE=0;
		        cel2D.modE=0;
		        cel2D.vecT=0;
		        cel2D.modT=0;
		        cel2D.fatG=0;
		        cel2D.angES=0;
		        cel2D.fInter=0;
		        cel2D.fIfC=0;
		        cel2D.gradGreenPres=0;
		        cel2D.gradGreenPresCor=0;
		        presF=0;
		        presFcor=0;
		        gradPresface=0;
		        gradPresfaceCor=0;
		        cel2D.gradGreenU=0;
		        uF=0;
		        gradUface=0;
		        cel2D.gradGreenV=0;
		        vF=0;
		    	cel2D.gradGreenTemp=0;
		    	tempF=0;
		    	cel2D.gradGreenHol=0;

		        cel2D.gradGreenPresI=0;
		        cel2D.gradGreenPresCorI=0;
		        cel2D.gradGreenUI=0;
		        cel2D.gradGreenVI=0;
		        cel2D.gradGreenTempI=0;
		        cel2D.gradGreenHolI=0;

		    	holF=0;
				tUpw=0;
				uUpw=0;
				vUpw=0;
				holUpw=0;
				tMed=0;
				uMed=0;
				vMed=0;
				holMed=0;
				tDow=0;
				uDow=0;
				vDow=0;
				holDow=0;
				tK=0;
				uK=0;
				vK=0;
				holK=0;
				tL=0;
				uL=0;
				vL=0;
				holL=0;
				uRC=0;
				vRC=0;
				uRC0=0;
				vRC0=0;
		        massF=0;
		        taxadeformFace=0;
		        viscFace=0;
		    	coefUHRC=0;
		    	coefUHRV=0;
		    	fonteUHR=0;
		    	coefVHRC=0;
		    	coefVHRV=0;
		    	fonteVHR=0;
		    	coefTHRC=0;
		    	coefTHRV=0;
		    	fonteTHR=0;
		        gradVface=0;
		    	ccInU=0;
		    	ccInV=0;
		    	ccWvel=0;
		    	ccPres=0;
		    	ccWall=0;
		    	///////////////////////////////////////////////////////////////////////////
		    	ccTD=0;
		    	ccTVN=0;
		    	ccHR=0;
		    	ccTambR=0;
		    	//////////////////////////////////////////////////////////////////////////
		    	cel2D.difuPres=0;
		    	cel2D.difuPresRC=0;
		    	cel2D.vecSDif=0;
		    	cel2D.vecSDifMod=0;
		    	cel2D.HcU=0;
		    	cel2D.HcV=0;
		    	cel2D.HcP=0;
		    	cel2D.HcT=0;
		    	cel2D.B2Med=0;
		    	cel2D.BMedF=0;
		    	confinado=0;
		    	dtCFL=0;
		    	CFL=0;
			}
	}
	return *this;
}

void elem2d::buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle){
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

void elem2d::menorIndViz(){
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

double elem2d::escalar(double* v1, double* v2, int vDim){
	double tot=0.;
	for(int i=0;i<vDim;i++)tot+=v1[i]*v2[i];
	return tot;
}

void elem2d::indraz(int& ind, double& raz,
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

void elem2d::faceDetalhes(){

	/*
	double** vecE;//vetor unitario na orientacao entre o centroide de do elemento e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elemento e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;//distancia entre o centroide da face e a intersecao da face
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementos
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementos e a face do elemento
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

		double ortogS [cel2D.dim];
		ortogS[0]=-cel2D.sFace[i][1]/cel2D.sFaceMod[i];
		ortogS[1]=cel2D.sFace[i][0]/cel2D.sFaceMod[i];
		double denf=0.;
		double numf=0.;
		//denf=cel2D.vecE[i][0]*ortogS[1]-cel2D.vecE[i][1]*ortogS[0];
		//numf=(cel2D.centroideFace[i][0]-cel2D.centroideElem[0])*ortogS[1]-
				//(cel2D.centroideFace[i][1]-cel2D.centroideElem[1])*ortogS[0];
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

void elem2d::vazMass(double rlx,int noRelax){
	double masTemp;
	double difGradMed [cel2D.dim];
	massTot=0.;
	if(fabsl(cel2D.centroElem[1]-6.0806626583835642 )<1e-5){
		int para;
		para=0;
	}
	//int inl=0;
	//int out=0;
	//int wall=0;
	//int sim=0;
	//int kcc=0;
	for(int face=0;face<cel2D.nvert;face++){
		if(kvizinho[face]>=0){
			for(int j=0; j<cel2D.dim;j++){
				difGradMed[j]=cel2D.fatG[face]*cel2D.gradGreenPres[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenPres[j];
			}
			double termoCorda=escalar(difGradMed,cel2D.vecE[face],cel2D.dim);
			double deri=-(cel2D.presC-(*vizinho[face]).presC)/cel2D.modE[face];
			for(int j=0; j<cel2D.dim;j++){
				difGradMed[j]=(deri-termoCorda)*cel2D.vecE[face][j];
			}
			double difU=cel2D.fatG[face]*cel2D.difuPresRC[0]+
					(1-cel2D.fatG[face])*(*vizinho[face]).difuPresRC[0];
			double difV=cel2D.fatG[face]*cel2D.difuPresRC[1]+
					(1-cel2D.fatG[face])*(*vizinho[face]).difuPresRC[1];
			double corRelaxU=1*(1-rlx)*(uRC[face]-
					(cel2D.fatG[face]*cel2D.uCI+(1-cel2D.fatG[face])*(*vizinho[face]).uCI));
			double corRelaxV=1*(1-rlx)*(vRC[face]-
					(cel2D.fatG[face]*cel2D.vCI+(1-cel2D.fatG[face])*(*vizinho[face]).vCI));
			uRC[face]=cel2D.fatG[face]*cel2D.uC+(1-cel2D.fatG[face])*(*vizinho[face]).uC;
			vRC[face]=cel2D.fatG[face]*cel2D.vC+(1-cel2D.fatG[face])*(*vizinho[face]).vC;
			uRC[face]-=difGradMed[0]*difU;
			vRC[face]-=difGradMed[1]*difV;
			if(noRelax==0){
				uRC[face]+=corRelaxU;
				vRC[face]+=corRelaxV;
			}
			double bmed[cel2D.dim];
			bmed[0]=0.;
			bmed[1]=(cel2D.fatG[face]*cel2D.rho+(1-cel2D.fatG[face])*(*vizinho[face]).rho)*(*vg1dSP).gravVF*(*vg1dSP).vertY;
			double bmed3[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++)
				bmed3[j]=(cel2D.fatG[face]*cel2D.B2Med[j]+(1-cel2D.fatG[face])*(*vizinho[face]).B2Med[j]);
			///// Moukaled
			if((*vg1dSP).correcForcCorp==0){
			//uRC[face]+=1.*(cel2D.BMedF[face][0]-bmed3[0])*difU;
				vRC[face]+=1.*(cel2D.BMedF[face][1]-bmed3[1])*difV;
			}
			else{
			/////eu-primeira ordem
				double deltaRho=cel2D.rho*(1*(*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-
						(*vizinho[face]).rho*(1*(*vg1dSP).mulFC-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
				double rhoMed=cel2D.fatG[face]*cel2D.rho*(1*(*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
						(1-cel2D.fatG[face])*(*vizinho[face]).rho*(1*(*vg1dSP).mulFC-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
				vRC[face]+=(cel2D.rho*(1*(*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-
						0.5*deltaRho-rhoMed)*(*vg1dSP).gravVF*cel2D.vecE[face][1]*cel2D.vecE[face][1]*difV;
			}
			//eu-segunda ordem
			//double gCf=1.-cel2D.fatG[face];
			//double gfV=1.-gCf;
			//double rhof=cel2D.rho*(1*mulFC-cel2D.beta*(tempF[face]-cel2D.tempRef));
			//double deltaRho=cel2D.rho*(1*mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-
					//(*vizinho[face]).rho*(1*mulFC-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
			//double deltaRho2=gCf*((*vizinho[face]).rho*(1*mulFC-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef))-rhof)-gfV*
					//(rhof-cel2D.rho*(1*mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef)));
			//double rhoMed=cel2D.fatG[face]*cel2D.rho*(1*mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
								//(1-cel2D.fatG[face])*(*vizinho[face]).rho*(1*mulFC-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
			//double difPotencial=rhof+0.5*deltaRho*(gfV*gfV-gCf*gCf)+(deltaRho2/(12.*gCf*gfV))*(gfV*gfV*gfV+gCf*gCf*gCf);
			//vRC[face]+=(difPotencial-rhoMed)*grav*cel2D.vecE[face][1]*cel2D.vecE[face][1]*difV;



			if(perm==0 && trans==1){
				double a0Difus;
				double RCmed;
				a0Difus=cel2D.fatG[face]*(cel2D.a0U)+
				(1-cel2D.fatG[face])*((*vizinho[face]).a0U);
				RCmed=cel2D.fatG[face]*cel2D.uC0+(1-cel2D.fatG[face])*((*vizinho[face]).uC0);
				uRC[face]+=1*a0Difus*(uRC0[face]-RCmed);

				a0Difus=cel2D.fatG[face]*(cel2D.a0V)+
				(1-cel2D.fatG[face])*((*vizinho[face]).a0V);
				RCmed=cel2D.fatG[face]*cel2D.vC0+(1-cel2D.fatG[face])*((*vizinho[face]).vC0);
				vRC[face]+=1*a0Difus*(vRC0[face]-RCmed);
			}
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(face, inl, out, wall, sim, kcc);
			if(inl==1){
				uRC[face]=ccInU[face];
				vRC[face]=ccInV[face];
			}
			/*else if(out==1){
				uRC[face]=cel2D.uC;
				vRC[face]=cel2D.vC;
			}*/
			else if(wall==1){
				uRC[face]=ccWall[face]*fabsl(-cel2D.sFace[face][1]/cel2D.sFaceMod[face]);
				vRC[face]=ccWall[face]*fabsl(cel2D.sFace[face][0]/cel2D.sFaceMod[face]);
			}
			else if(sim==1){
				/*double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenU[j];
				}
				double cordaArea=escalar(cel2D.vecE[face],cel2D.sFace[face],cel2D.dim);
				double termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[face],cel2D.dim);*/
				coefUHRC[face]=1;
				fonteUHR[face]=-(cel2D.uC*cel2D.sFace[face][0]+cel2D.vC*cel2D.sFace[face][1])*cel2D.sFace[face][0]/
						(cel2D.sFaceMod[face]*cel2D.sFaceMod[face]);
				uRC[face]=cel2D.uC+fonteUHR[face];
				//////////////////////////////////////////////////////////////////////////////////////////////////
				coefVHRC[face]=1;
				fonteVHR[face]=-(cel2D.uC*cel2D.sFace[face][0]+cel2D.vC*cel2D.sFace[face][1])*cel2D.sFace[face][1]/
						(cel2D.sFaceMod[face]*cel2D.sFaceMod[face]);
				vRC[face]=cel2D.vC+fonteVHR[face];
			}
			else if(out==1){
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenU[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				gradMed[0]-=termoCorda*cel2D.vecE[face][0];
				gradMed[1]-=termoCorda*cel2D.vecE[face][1];
				termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				coefUHRC[face]=1;
				fonteUHR[face]=-1*termoCorda*cel2D.modE[face];//dúvida, se é para zerar o gradiente, não devceria ser negativo
				//ao contrário do indicado em Moukalled?
				uRC[face]=cel2D.uC+fonteUHR[face];
				//////////////////////////////////////////////////////////////////////////////////////////////////
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenV[j];
				}
				termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				gradMed[0]-=termoCorda*cel2D.vecE[face][0];
				gradMed[1]-=termoCorda*cel2D.vecE[face][1];
				termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				coefVHRC[face]=1;
				fonteVHR[face]=-1*termoCorda*cel2D.modE[face];
				vRC[face]=cel2D.vC+fonteVHR[face];
			}
		}
	}
	for(int face=0;face<cel2D.nvert;face++){
		if(kvizinho[face]>=0){
			double rhoHarm;//=2*cel2D.visc*(*vizinho[face]).visc/(cel2D.visc+(*vizinho[face]).visc);
			//Moukaled
			if((*vg1dSP).correcForcCorp==0){
				rhoHarm=cel2D.fatG[face]/(cel2D.rho*(1.-0.*cel2D.beta*(cel2D.tempC-cel2D.tempRef)))+
						(1-cel2D.fatG[face])/((*vizinho[face]).rho*(1.-0.*(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef)));
				rhoHarm=1./rhoHarm;
			}
			else{
			//eu- primeira ordem
				/*
				 double deltaRho=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-
						(*vizinho[face]).rho*(1.-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
				rhoHarm=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-deltaRho*(1.-cel2D.fatG[face]);
				*/
				rhoHarm=cel2D.fatG[face]*cel2D.rho*(1.-0.*cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
								(1.-cel2D.fatG[face])*(*vizinho[face]).rho*(1.-0.*(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
			}
			//eu- segunda ordem
			//rhoHarm=cel2D.rho*(1.-cel2D.beta*(tempF[face]-cel2D.tempRef));
			masTemp=cel2D.sFace[face][0]*uRC[face]+cel2D.sFace[face][1]*vRC[face];
			massF[face]=rhoHarm*masTemp;
			massTot+=massF[face];
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(face, inl, out, wall, sim, kcc);
			if(inl==1 || out==1){
				double rhoHarm;//=2*cel2D.visc*(*vizinho[face]).visc/(cel2D.visc+(*vizinho[face]).visc);
				rhoHarm=cel2D.rho*(1.-0.*cel2D.beta*(cel2D.tempC-cel2D.tempRef));
				masTemp=cel2D.sFace[face][0]*uRC[face]+cel2D.sFace[face][1]*vRC[face];
				massF[face]=rhoHarm*masTemp;
				massTot+=1*massF[face];
			}
			else massF[face]=0.;
		}
	}
}

void elem2d::atualizaCor(){
	double difGradMed [cel2D.dim];
	massTot=0.;
	double masTemp;
	cel2D.uC-=cel2D.difuPres[0]*cel2D.gradGreenPresCor[0];
	cel2D.vC-=cel2D.difuPres[1]*cel2D.gradGreenPresCor[1];
	//cel2D.presC+=(1.-relax)*cel2D.presCcor;
	//cel2D.presC+=(relax)*cel2D.presCcor;
	cel2D.presC+=((*vg1dSP).relaxVFPcor)*cel2D.presCcor;
	for(int face=0;face<cel2D.nvert;face++){
		if(kvizinho[face]>=0){
			for(int j=0; j<cel2D.dim;j++){
				difGradMed[j]=cel2D.fatG[face]*cel2D.gradGreenPresCor[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenPresCor[j];
			}
			double termoCorda=escalar(difGradMed,cel2D.vecE[face],cel2D.dim);
			double deri=-(cel2D.presCcor-(*vizinho[face]).presCcor)/cel2D.modE[face];
			for(int j=0; j<cel2D.dim;j++){
				difGradMed[j]=difGradMed[j]+(deri-termoCorda)*cel2D.vecE[face][j];
			}
			double rhoHarm;
			//Moukaled
			if((*vg1dSP).correcForcCorp==0){
				rhoHarm=cel2D.fatG[face]/cel2D.rho+(1-cel2D.fatG[face])/(*vizinho[face]).rho;
				rhoHarm=1./rhoHarm;
			}
			else{
			//////eu-primeira ordem
				/*double deltaRho=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-
						(*vizinho[face]).rho*(1.-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
				rhoHarm=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-deltaRho*(1.-cel2D.fatG[face]);*/
				rhoHarm=cel2D.fatG[face]*cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
							(1.-cel2D.fatG[face])*(*vizinho[face]).rho*(1.-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef));
			}
			//////eu-segunda ordem
			//rhoHarm=cel2D.rho*(1.-cel2D.beta*(tempF[face]-cel2D.tempRef));
			double difU=cel2D.fatG[face]*cel2D.difuPres[0]+
					(1-cel2D.fatG[face])*(*vizinho[face]).difuPres[0];
			double difV=cel2D.fatG[face]*cel2D.difuPres[1]+
					(1-cel2D.fatG[face])*(*vizinho[face]).difuPres[1];
			//uRC[face]=uRC[face]-cel2D.difuPres[0]*cel2D.gradGreenPresCor[0];
			//vRC[face]=vRC[face]-cel2D.difuPres[1]*cel2D.gradGreenPresCor[1];
			//uRC[face]-=difU*difGradMed[0];
			//vRC[face]-=difV*difGradMed[1];
			masTemp=(cel2D.sFace[face][0]*difU*difGradMed[0]+cel2D.sFace[face][1]*difV*difGradMed[1]);
			massF[face]=massF[face]-rhoHarm*masTemp;
			massTot+=massF[face];
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(face, inl, out, wall, sim, kcc);
			if(out==1){
				for(int j=0; j<cel2D.dim;j++){
					difGradMed[j]=cel2D.gradGreenPresCor[j];
				}
				double termoCorda=escalar(difGradMed,cel2D.vecE[face],cel2D.dim);
				double deri=-(cel2D.presCcor)/cel2D.modE[face];
				for(int j=0; j<cel2D.dim;j++){
					difGradMed[j]=0.*difGradMed[j]+(deri-0.*termoCorda)*cel2D.vecE[face][j];
				}
				double rhoHarm;
				//rhoHarm=cel2D.rho;
				rhoHarm=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef));
				double difU=cel2D.difuPres[0];
				double difV=cel2D.difuPres[1];
				//uRC[face]=uRC[face]-cel2D.difuPres[0]*cel2D.gradGreenPresCor[0];
				//vRC[face]=vRC[face]-cel2D.difuPres[1]*cel2D.gradGreenPresCor[1];
				//uRC[face]-=difU*difGradMed[0];
				//vRC[face]-=difV*difGradMed[1];
				masTemp=-(cel2D.sFace[face][0]*difU*difGradMed[0]+cel2D.sFace[face][1]*difV*difGradMed[1]);
				massF[face]=massF[face]+rhoHarm*masTemp;
				//double gradMed [cel2D.dim];
				//for(int j=0; j<cel2D.dim;j++){
					//gradMed[j]=cel2D.gradGreenU[j];
				//}
				//double termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				//gradMed[0]-=termoCorda*cel2D.vecE[face][0];
				//gradMed[1]-=termoCorda*cel2D.vecE[face][1];
				//termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				//coefUHRC[face]=1;
				//fonteUHR[face]=-termoCorda*cel2D.modE[face];//dúvida, se é para zerar o gradiente, não devceria ser negativo
				//ao contrário do indicado em Moukalled?
				//uRC[face]=cel2D.uC+fonteUHR[face];
				//////////////////////////////////////////////////////////////////////////////////////////////////
				//for(int j=0; j<cel2D.dim;j++){
					//gradMed[j]=cel2D.gradGreenV[j];
				//}
				//termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				//gradMed[0]-=termoCorda*cel2D.vecE[face][0];
				//gradMed[1]-=termoCorda*cel2D.vecE[face][1];
				//termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
				//coefVHRC[face]=1;
				//fonteVHR[face]=-termoCorda*cel2D.modE[face];
				//vRC[face]=cel2D.vC+fonteVHR[face];
				massTot+=1*massF[face];
			}
			else if(inl==1)massTot+=1*massF[face];
		}
	}
}

void elem2d::impliHR(int face){
	double phi=0.;
	double lLoc=0.;
	double kLoc=0.;

	for(int vari=1;vari<3;vari++){
		//if(vari==0)phi=(tMed[face]-tUpw[face])/(tDow[face]-tUpw[face]);
		//else
		if(vari==1){
			if(fabsl(uDow[face]-uUpw[face])>1e-15)
				phi=(uMed[face]-uUpw[face])/(uDow[face]-uUpw[face]);
			else phi=1.1;
		}
		else if(vari==2){
			if(fabsl(vDow[face]-vUpw[face])>1e-15)
				phi=(vMed[face]-vUpw[face])/(vDow[face]-vUpw[face]);
			else phi=1.1;
		}
		/*if(phi>0. && phi<1./6.){
			lLoc=4.;
			kLoc=0.;
		}
		else if(phi>=1./6. && phi<5./6.){
			lLoc=3./4.;
			kLoc=3./8.;
		}
		else if(phi>=5./6. && phi<1.){
			lLoc=0.;
			kLoc=1.;
		}
		else{
			lLoc=1.;
			kLoc=0.;
		}*/
		switch((*vg1dSP).nvfHR){
			case 0://upwind
				{
					lLoc=1.;
					kLoc=0.;
				}
				break;

			case 1://smart
				if(phi>0. && phi<1./6.){
					lLoc=4.;
					kLoc=0.;
				}
				else if(phi>=1./6. && phi<5./6.){
					lLoc=3./4.;
					kLoc=3./8.;
				}
				else if(phi>=5./6. && phi<1.){
					lLoc=0.;
					kLoc=1.;
				}
				else{
					lLoc=1.;
					kLoc=0.;
				}
				break;
			case 2://minmod
				if(phi>0. && phi<1./2.){
					lLoc=3./2.;
					kLoc=0.;
				}
				else if(phi>=1./2. && phi<1.){
					lLoc=1./2.;
					kLoc=1./2.;
				}
				else{
					lLoc=1.;
					kLoc=0.;
				}
				break;
			case 3://osher
				if(phi>0. && phi<2./3.){
					lLoc=3./2.;
					kLoc=0.;
				}
				else if(phi>=2./3. && phi<1.){
					lLoc=0.;
					kLoc=1.;
				}
				else{
					lLoc=1.;
					kLoc=0.;
				}
				break;
			case 4://muscl
				if(phi>0. && phi<1./4.){
					lLoc=2.;
					kLoc=0.;
				}
				else if(phi>=1./4. && phi<3./4.){
					lLoc=1.;
					kLoc=1./4.;
				}
				else if(phi>=3./4. && phi<1.){
					lLoc=0.;
					kLoc=1.;
				}
				else{
					lLoc=1.;
					kLoc=0.;
				}
				break;
		}
		/*if(vari==0){
			tL[face]=lLoc;
			tK[face]=kLoc;
		}
		else*/
		if(vari==1){
			//lLoc=1.;
			//kLoc=0.;
			uL[face]=lLoc;
			uK[face]=kLoc;
		}
		else if(vari==2){
			//lLoc=1.;
			//kLoc=0.;
			vL[face]=lLoc;
			vK[face]=kLoc;
		}
	}
}

void elem2d::expliHR(int face){
	double phi=0.;
	double lLoc=0.;
	double mLoc=0.;

	for(int vari=1;vari<3;vari++){
		//if(vari==0)phi=(tMed[face]-tUpw[face])/(tDow[face]-tUpw[face]);
		//else
		if(vari==1){
			if(fabsl(uDow[face]-uUpw[face])>1e-15)phi=(uMed[face]-uUpw[face])/(uDow[face]-uUpw[face]);
			else phi=1.1;
		}
		else if(vari==2){
			if(fabsl(vDow[face]-vUpw[face])>1e-15)phi=(vMed[face]-vUpw[face])/(vDow[face]-vUpw[face]);
			else phi=1.1;
		}
		/*if(phi>0. && phi<1./6.){
			lLoc=4.;
			kLoc=0.;
		}
		else if(phi>=1./6. && phi<5./6.){
			lLoc=3./4.;
			kLoc=3./8.;
		}
		else if(phi>=5./6. && phi<1.){
			lLoc=0.;
			kLoc=1.;
		}
		else{
			lLoc=1.;
			kLoc=0.;
		}*/
		switch((*vg1dSP).nvfHR){
			case 0://upwind
				{
					lLoc=0.;
					mLoc=1.;
				}
				break;

			case 1://modified smart
				if(phi>0. && phi<1./6.){
					lLoc=0.;
					mLoc=3.;
				}
				else if(phi>=1./6. && phi<7./10.){
					lLoc=3./8.;
					mLoc=3./4.;
				}
				else if(phi>=7./10. && phi<1.){
					lLoc=2./3.;
					mLoc=1./3.;
				}
				else{
					lLoc=0.;
					mLoc=1.;
				}
				break;
			case 2://minmod
				if(phi>0. && phi<1./2.){
					lLoc=0.;
					mLoc=3./2.;
				}
				else if(phi>=1./2. && phi<1.){
					lLoc=1./2.;
					mLoc=1./2.;
				}
				else{
					lLoc=0.;
					mLoc=1.;
				}
				break;
			case 3://osher
				if(phi>0. && phi<2./3.){
					lLoc=0.;
					mLoc=3./2.;
				}
				else if(phi>=2./3. && phi<1.){
					lLoc=1.;
					mLoc=0.;
				}
				else{
					lLoc=0.;
					mLoc=1.;
				}
				break;
			case 4://muscl
				if(phi>0. && phi<1./4.){
					lLoc=0.;
					mLoc=2.;
				}
				else if(phi>=1./4. && phi<3./4.){
					lLoc=1./4.;
					mLoc=1.;
				}
				else if(phi>=3./4. && phi<1.){
					lLoc=1.;
					mLoc=0.;
				}
				else{
					lLoc=0.;
					mLoc=1.;
				}
				break;
		}
		/*if(vari==0){
			tL[face]=lLoc;
			tK[face]=mLoc;
		}
		else*/
		if(vari==1){
			//lLoc=1.;
			//kLoc=0.;
			uL[face]=lLoc;
			uK[face]=mLoc;
		}
		else if(vari==2){
			//lLoc=1.;
			//kLoc=0.;
			vL[face]=lLoc;
			vK[face]=mLoc;
		}
	}
}

void elem2d::impliHRT(int face){
	double phi=0.;
	double lLoc=0.;
	double kLoc=0.;

	if(fabsl(tDow[face]-tUpw[face])>1e-15)phi=(tMed[face]-tUpw[face])/(tDow[face]-tUpw[face]);
	else phi=1.1;
	switch((*vg1dSP).nvfHR){
		case 0://upwind
			{
				lLoc=1.;
				kLoc=0.;
			}
			break;

		case 1://smart
			if(phi>0. && phi<1./6.){
				lLoc=4.;
				kLoc=0.;
			}
			else if(phi>=1./6. && phi<5./6.){
				lLoc=3./4.;
				kLoc=3./8.;
			}
			else if(phi>=5./6. && phi<1.){
				lLoc=0.;
				kLoc=1.;
			}
			else{
				lLoc=1.;
				kLoc=0.;
			}
			break;
		case 2://minmod
			if(phi>0. && phi<1./2.){
				lLoc=3./2.;
				kLoc=0.;
			}
			else if(phi>=1./2. && phi<1.){
				lLoc=1./2.;
				kLoc=1./2.;
			}
			else{
				lLoc=1.;
				kLoc=0.;
			}
			break;
		case 3://osher
			if(phi>0. && phi<2./3.){
				lLoc=3./2.;
				kLoc=0.;
			}
			else if(phi>=2./3. && phi<1.){
				lLoc=0.;
				kLoc=1.;
			}
			else{
				lLoc=1.;
				kLoc=0.;
			}
			break;
		case 4://muscl
			if(phi>0. && phi<1./4.){
				lLoc=2.;
				kLoc=0.;
			}
			else if(phi>=1./4. && phi<3./4.){
				lLoc=1.;
				kLoc=1./4.;
			}
			else if(phi>=3./4. && phi<1.){
				lLoc=0.;
				kLoc=1.;
			}
			else{
				lLoc=1.;
				kLoc=0.;
			}
			break;
	}
	tL[face]=lLoc;
	tK[face]=kLoc;
}

void elem2d::expliHRT(int face){
	double phi=0.;
	double lLoc=0.;
	double mLoc=0.;

	//phi=(tMed[face]-tUpw[face])/(tDow[face]-tUpw[face]);
	if(fabsl(tDow[face]-tUpw[face])>1e-15)phi=(tMed[face]-tUpw[face])/(tDow[face]-tUpw[face]);
	else phi=1.1;
	switch((*vg1dSP).nvfHR){
		case 0://upwind
			{
				lLoc=0.;
				mLoc=1.;
			}
			break;

		case 1://modified smart
			if(phi>0. && phi<1./6.){
				lLoc=0.;
				mLoc=3.;
			}
			else if(phi>=1./6. && phi<7./10.){
				lLoc=3./8.;
				mLoc=3./4.;
			}
			else if(phi>=7./10. && phi<1.){
				lLoc=2./3.;
				mLoc=1./3.;
			}
			else{
				lLoc=0.;
				mLoc=1.;
			}
			break;
		case 2://minmod
			if(phi>0. && phi<1./2.){
				lLoc=0.;
				mLoc=3./2.;
			}
			else if(phi>=1./2. && phi<1.){
				lLoc=1./2.;
				mLoc=1./2.;
			}
			else{
				lLoc=0.;
				mLoc=1.;
			}
			break;
		case 3://osher
			if(phi>0. && phi<2./3.){
				lLoc=0.;
				mLoc=3./2.;
			}
			else if(phi>=2./3. && phi<1.){
				lLoc=1.;
				mLoc=0.;
			}
			else{
				lLoc=0.;
				mLoc=1.;
			}
			break;
		case 4://muscl
			if(phi>0. && phi<1./4.){
				lLoc=0.;
				mLoc=2.;
			}
			else if(phi>=1./4. && phi<3./4.){
				lLoc=1./4.;
				mLoc=1.;
			}
			else if(phi>=3./4. && phi<1.){
				lLoc=1.;
				mLoc=0.;
			}
			else{
				lLoc=0.;
				mLoc=1.;
			}
			break;
	}
	tL[face]=lLoc;
	tK[face]=mLoc;
}

void elem2d::expliHRHol(int face){
	double phi=0.;
	double lLoc=0.;
	double mLoc=1.;

	if(fabsl(holDow[face]-holUpw[face])>1e-15 && fabsl(CFLC)>1e-15){
		phi=(holMed[face]-holUpw[face])/(holDow[face]-holUpw[face]);
		if((*vg1dSP).nvfHRHol==0){//upwind
				lLoc=0.;
				mLoc=1.;
		}
		else if((*vg1dSP).nvfHRHol==1){//cicsam
			double lCBC;
			double mCBC;
			double phiCBC;
			if(phi>0. && phi<1.){
				if(phi/CFLC<1.){
					lCBC=0.;
					mCBC=1./CFLC;
				}
				else{
					lCBC=1.;
					mCBC=0.;
				}
				phiCBC=mCBC*phi+lCBC;
				double gam;
				double nInter[cel2D.dim];
				double modGrad=0;
				for(int k=0; k<cel2D.dim;k++)modGrad+=cel2D.gradGreenHolI[k]*cel2D.gradGreenHolI[k];
				modGrad=sqrtl(modGrad);
				for(int k=0; k<cel2D.dim;k++)nInter[k]=-cel2D.gradGreenHolI[k]/modGrad;
				double cos=0;
				for(int k=0; k<cel2D.dim;k++)cos+=cel2D.vecE[face][k]*nInter[k];
				double tet=fabsl(acosl(cos));
				gam=(1.+cosl(2.*tet))/2.;
				if(gam>1.)gam=1.;
				double lCicsam;
				double mCicsam;
				mCicsam=CFLC+(1.-CFLC)*6./8.;
				lCicsam=(1.-CFLC)*3./8.;
				double phiCicsam=mCicsam*phi+lCicsam;
				//if((mCicsam*phi+lCicsam)<phi/CFLC){
				if(phiCicsam<phiCBC){
					lLoc=lCicsam;
					mLoc=mCicsam;
				}
				else{
					lLoc=lCBC;
					mLoc=mCBC;
				}
			}
			else{
				lLoc=0.;
				mLoc=1.;
			}
		}
	}
	else{
		lLoc=0.;
		mLoc=1.;
	}
	holL[face]=lLoc;
	holK[face]=mLoc;
}

void elem2d::impliCoefHRT(int face){
	if(massF[face]>=0.){
		coefTHRC[face]=tL[face];
		coefTHRV[face]=tK[face];
		fonteTHR[face]=(tDow[face]-2*escalar(cel2D.gradGreenTempI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face])*(1-tL[face]-tK[face]);
	}
	else{
		coefTHRV[face]=tL[face];
		coefTHRC[face]=tK[face];
		fonteTHR[face]=(tDow[face]+2*escalar(cel2D.gradGreenTempI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face])*(1-tL[face]-tK[face]);
	}
}

void elem2d::expliCoefHRT(int face){
	if(massF[face]>=0.){
		coefTHRC[face]=1.;
		coefTHRV[face]=0.;
		fonteTHR[face]=tMed[face]*(tK[face]-1.)+tUpw[face]*(1.-tK[face]-tL[face])+tDow[face]*tL[face];
	}
	else{
		coefTHRV[face]=1.;
		coefTHRC[face]=0.;
		fonteTHR[face]=tMed[face]*(tK[face]-1.)+tUpw[face]*(1.-tK[face]-tL[face])+tDow[face]*tL[face];
	}
}

void elem2d::impliCoefHR(int face){
	if(massF[face]>=0.){
		coefUHRC[face]=uL[face];
		coefUHRV[face]=uK[face];
		fonteUHR[face]=(uDow[face]-2*escalar(cel2D.gradGreenUI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face])*(1-uL[face]-uK[face]);
		coefVHRC[face]=vL[face];
		coefVHRV[face]=vK[face];
		fonteVHR[face]=(vDow[face]-2*escalar(cel2D.gradGreenVI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face])*(1-vL[face]-vK[face]);
	}
	else{
		coefUHRV[face]=uL[face];
		coefUHRC[face]=uK[face];
		fonteUHR[face]=(uDow[face]+2*escalar(cel2D.gradGreenUI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face])*(1-uL[face]-uK[face]);
		coefVHRV[face]=vL[face];
		coefVHRC[face]=vK[face];
		fonteVHR[face]=(vDow[face]+2*escalar(cel2D.gradGreenVI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face])*(1-vL[face]-vK[face]);
	}
}

void elem2d::expliCoefHR(int face){
	if(massF[face]>=0.){
		coefUHRC[face]=1.;
		coefUHRV[face]=0.;
		fonteUHR[face]=uMed[face]*(uK[face]-1.)+uUpw[face]*(1.-uK[face]-uL[face])+uDow[face]*uL[face];
		coefVHRC[face]=1.;
		coefVHRV[face]=0.;
		fonteVHR[face]=vMed[face]*(vK[face]-1.)+vUpw[face]*(1.-vK[face]-vL[face])+vDow[face]*vL[face];
	}
	else{
		coefUHRV[face]=1.;
		coefUHRC[face]=0.;
		fonteUHR[face]=uMed[face]*(uK[face]-1.)+uUpw[face]*(1.-uK[face]-uL[face])+uDow[face]*uL[face];
		coefVHRV[face]=1.;
		coefVHRC[face]=0.;
		fonteVHR[face]=vMed[face]*(vK[face]-1.)+vUpw[face]*(1.-vK[face]-vL[face])+vDow[face]*vL[face];
	}
}

void elem2d::calcVarFaceUV(int face){
	if(massF[face]>=0.){
		uMed[face]=cel2D.uC;
		uDow[face]=(*vizinho[face]).uC;
		uUpw[face]=uDow[face]-2*escalar(cel2D.gradGreenUI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
		vMed[face]=cel2D.vC;
		vDow[face]=(*vizinho[face]).vC;
		vUpw[face]=vDow[face]-2*escalar(cel2D.gradGreenVI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];

	}
	else{
		uMed[face]=(*vizinho[face]).uC;
		uDow[face]=cel2D.uC;
		uUpw[face]=uDow[face]+2*escalar((*vizinho[face]).gradGreenUI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
		vMed[face]=(*vizinho[face]).vC;
		vDow[face]=cel2D.vC;
		vUpw[face]=vDow[face]+2*escalar((*vizinho[face]).gradGreenVI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}
	if((*vg1dSP).atrasaHR==0)impliHR(face);
	else expliHR(face);
	double gradMedU [cel2D.dim];
	double gradMedV [cel2D.dim];

	for(int j=0; j<cel2D.dim;j++){
		gradMedU[j]=cel2D.fatG[face]*cel2D.gradGreenUI[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenUI[j];
	}
	double termoCorda=escalar(gradMedU,cel2D.vecE[face],cel2D.dim);
	double deri=-(cel2D.uC-(*vizinho[face]).uC)/cel2D.modE[face];
	for(int j=0; j<cel2D.dim;j++){
		gradMedU[j]=gradMedU[j]+(deri-termoCorda)*cel2D.vecE[face][j];
	}

	for(int j=0; j<cel2D.dim;j++){
		gradMedV[j]=cel2D.fatG[face]*cel2D.gradGreenVI[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenVI[j];
	}
	termoCorda=escalar(gradMedV,cel2D.vecE[face],cel2D.dim);
	deri=-(cel2D.vC-(*vizinho[face]).vC)/cel2D.modE[face];
	for(int j=0; j<cel2D.dim;j++){
		gradMedV[j]=gradMedV[j]+(deri-termoCorda)*cel2D.vecE[face][j];
	}

	/*if(massF[face]>=0.){
		coefUHRC[face]=uL[face];
		coefUHRV[face]=uK[face]+(1-uL[face]-uK[face]);
		fonteUHR[face]=-2*escalar(cel2D.gradGreenU,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face]*(1-uL[face]-uK[face]);
		coefVHRC[face]=vL[face];
		coefVHRV[face]=vK[face]+(1-vL[face]-vK[face]);
		fonteVHR[face]=-2*escalar(cel2D.gradGreenV,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face]*(1-vL[face]-vK[face]);
	}
	else{
		coefUHRV[face]=uL[face];
		coefUHRC[face]=uK[face]+(1-uL[face]-uK[face]);
		fonteUHR[face]=2*escalar(cel2D.gradGreenU,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face]*(1-uL[face]-uK[face]);
		coefVHRV[face]=vL[face];
		coefVHRC[face]=vK[face]+(1-vL[face]-vK[face]);
		fonteVHR[face]=2*escalar(cel2D.gradGreenV,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face]*(1-vL[face]-vK[face]);
	}*/
	if((*vg1dSP).atrasaHR==0)impliCoefHR(face);
	else expliCoefHR(face);
	for(int j=0; j<cel2D.dim;j++){
		fonteUHR[face]+=(*vg1dSP).corrigeDistor*gradMedU[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
		fonteVHR[face]+=(*vg1dSP).corrigeDistor*gradMedV[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
	}
	coefUHRC[face]*=massF[face];
	coefUHRV[face]*=massF[face];
	fonteUHR[face]*=massF[face];
	coefVHRC[face]*=massF[face];
	coefVHRV[face]*=massF[face];
	fonteVHR[face]*=massF[face];

	uF[face]=0.;
	vF[face]=0.;

	for(int j=0; j<cel2D.dim;j++){
		uF[face]+=(*vg1dSP).corrigeDistor*gradMedU[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
		vF[face]+=(*vg1dSP).corrigeDistor*gradMedV[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
	}

	if((*vg1dSP).atrasaHR==0){
		uF[face]+=uL[face]*uMed[face]+uK[face]*uDow[face]+(1-uL[face]-uK[face])*uUpw[face];
		vF[face]+=vL[face]*vMed[face]+vK[face]*vDow[face]+(1-vL[face]-vK[face])*vUpw[face];
	}
	else{
		uF[face]+=uK[face]*uMed[face]+uL[face]*uDow[face]+(1-uL[face]-uK[face])*uUpw[face];
		vF[face]+=vK[face]*vMed[face]+vL[face]*vDow[face]+(1-vL[face]-vK[face])*vUpw[face];
	}
}

void elem2d::calcVarFaceTemp(int face){
	if(massF[face]>=0.){
		tMed[face]=cel2D.tempC;
		tDow[face]=(*vizinho[face]).tempC;
		tUpw[face]=tDow[face]-2*escalar(cel2D.gradGreenTempI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}
	else{
		tMed[face]=(*vizinho[face]).tempC;
		tDow[face]=cel2D.tempC;
		tUpw[face]=tDow[face]+2*escalar((*vizinho[face]).gradGreenTempI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}

	tempF[face]=0.;


	if((*vg1dSP).atrasaHR==0)impliHRT(face);
	else expliHRT(face);
	double gradInterT [cel2D.dim];
	double gradMedT [cel2D.dim];

	for(int j=0; j<cel2D.dim;j++){
		gradInterT[j]=cel2D.fatG[face]*cel2D.gradGreenTempI[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenTempI[j];
		gradMedT[j]=0.5*(cel2D.gradGreenTempI[j]+(*vizinho[face]).gradGreenTempI[j]);
	}
	double termoCorda=escalar(gradInterT,cel2D.vecE[face],cel2D.dim);
	double deri=-(cel2D.tempC-(*vizinho[face]).tempC)/cel2D.modE[face];
	for(int j=0; j<cel2D.dim;j++){
		gradMedT[j]=gradInterT[j]+(deri-termoCorda)*cel2D.vecE[face][j];
	}

	double corDistorc2=1.;
	if(fabsl(cel2D.angES[face])<0.9)corDistorc2=0.;

	if((*vg1dSP).atrasaHR==0)impliCoefHRT(face);
	else expliCoefHRT(face);
	for(int j=0; j<cel2D.dim;j++){
		fonteTHR[face]+=corDistorc2*(*vg1dSP).corrigeDistor*gradMedT[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
	}
	coefTHRC[face]*=massF[face];
	coefTHRV[face]*=massF[face];
	fonteTHR[face]*=massF[face];

	for(int j=0; j<cel2D.dim;j++){
		tempF[face]+=corDistorc2*(*vg1dSP).corrigeDistor*gradMedT[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
	}

	if((*vg1dSP).atrasaHR==0){
		tempF[face]+=tL[face]*tMed[face]+tK[face]*tDow[face]+(1-tL[face]-tK[face])*tUpw[face];
	}
	else{
		tempF[face]+=tK[face]*tMed[face]+tL[face]*tDow[face]+(1-tL[face]-tK[face])*tUpw[face];
	}
}

void elem2d::calcVarFacePres(int face){
	/*if(massF[face]>=0.){
		tMed[face]=cel2D.presC;
		tDow[face]=(*vizinho[face]).presC;
		tUpw[face]=tDow[face]-2*escalar(cel2D.gradGreenPres,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}
	else{
		tMed[face]=(*vizinho[face]).presC;
		tDow[face]=cel2D.presC;
		tUpw[face]=tDow[face]+2*escalar((*vizinho[face]).gradGreenPres,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}*/

	presF[face]=0.;

	double corDistorc2=1.;
	if(fabsl(cel2D.angES[face])<0.9)corDistorc2=0.;


	double gradMed [cel2D.dim];
	for(int j=0; j<cel2D.dim;j++){
		gradMed[j]=cel2D.fatG[face]*cel2D.gradGreenPresI[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenPresI[j];
	}

	double termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
	double deri=-(cel2D.presC-(*vizinho[face]).presC)/cel2D.modE[face];
	for(int j=0; j<cel2D.dim;j++){
		gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[face][j];
	}
	for(int j=0; j<cel2D.dim;j++)
	presF[face]+=corDistorc2*(*vg1dSP).corrigeDistor*gradMed[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);


	presF[face]+=cel2D.fatG[face]*cel2D.presC+(1.-cel2D.fatG[face])*(*vizinho[face]).presC;
	double forcCorp=-(cel2D.fatG[face]*cel2D.rho*((*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
				(1.-cel2D.fatG[face])*(*vizinho[face]).rho*((*vg1dSP).mulFC-
						(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef)))*(*vg1dSP).gravVF*sinl((*vg1dSP).angY);
	/*presF[face]-=cel2D.rho*(mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))*grav*sinl(angY)*cel2D.vecE[face][1]*cel2D.modE[face]*(1.-cel2D.fatG[face])/2;
	presF[face]+=(*vizinho[face]).rho*(mulFC-(*vizinho[face]).beta*((*vizinho[face]).tempC-(*vizinho[face]).tempRef))*
			grav*sinl(angY)*cel2D.vecE[face][1]*cel2D.modE[face]*cel2D.fatG[face]/2;
	presF[face]+=(cel2D.presC+(*vizinho[face]).presC)/2;*/

	for(int j=0; j<cel2D.dim;j++){
		presF[face]+=(1.-corDistorc2*(*vg1dSP).corrigeDistor)*(0.*cel2D.BMedF[face][1]+1.*forcCorp)*(cel2D.centroideFace[face][1]-cel2D.fInter[face][1]);
	}
}

void elem2d::calcVarFacePresCor(int face){

	presFcor[face]=0.;

	double gradMed [cel2D.dim];
	for(int j=0; j<cel2D.dim;j++){
		gradMed[j]=cel2D.fatG[face]*cel2D.gradGreenPresCorI[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenPresCorI[j];
	}
	double termoCorda=escalar(gradMed,cel2D.vecE[face],cel2D.dim);
	double deri=-(cel2D.presCcor-(*vizinho[face]).presCcor)/cel2D.modE[face];
	for(int j=0; j<cel2D.dim;j++){
		gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[face][j];
	}
	double corDistorc2=1.;
	if(fabsl(cel2D.angES[face])<0.9)corDistorc2=0.;
	for(int j=0; j<cel2D.dim;j++)
	presFcor[face]+=corDistorc2*(*vg1dSP).corrigeDistor*gradMed[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
	presFcor[face]+=cel2D.fatG[face]*cel2D.presCcor+(1.-cel2D.fatG[face])*(*vizinho[face]).presCcor;
}

void elem2d::calcVarFaceHol(int face){
	if(massF[face]>=0.){
		holMed[face]=cel2D.holC;
		holDow[face]=(*vizinho[face]).holC;
		holUpw[face]=holDow[face]-2*escalar(cel2D.gradGreenHolI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}
	else{
		holMed[face]=(*vizinho[face]).holC;
		holDow[face]=cel2D.holC;
		holUpw[face]=holDow[face]+2*escalar((*vizinho[face]).gradGreenHolI,cel2D.vecE[face],cel2D.dim)*cel2D.modE[face];
	}

	holF[face]=0.;


	expliHRHol(face);
	double gradMedHol [cel2D.dim];

	for(int j=0; j<cel2D.dim;j++){
		gradMedHol[j]=cel2D.fatG[face]*cel2D.gradGreenHolI[j]+(1-cel2D.fatG[face])*(*vizinho[face]).gradGreenHolI[j];
	}
	double termoCorda=escalar(gradMedHol,cel2D.vecE[face],cel2D.dim);
	double deri=-(cel2D.holC-(*vizinho[face]).holC)/cel2D.modE[face];
	for(int j=0; j<cel2D.dim;j++){
		gradMedHol[j]=gradMedHol[j]+(deri-termoCorda)*cel2D.vecE[face][j];
	}

	//for(int j=0; j<cel2D.dim;j++){
		//holF[face]+=1.*gradMedHol[j]*(cel2D.centroideFace[face][j]-cel2D.fInter[face][j]);
	//}

	holF[face]+=holK[face]*holMed[face]+holL[face]*holDow[face]+(1-holL[face]-holK[face])*holUpw[face];
}

void elem2d::tipoCCTemp(int i,int& diri, int& vn, int& rich, int& acoplado, int& kcc){
	while(kcc<CC.nDiri && cel2D.ccFace[i]!=CC.ccDir[kcc].rotulo) kcc++;
	if(kcc<CC.nDiri && cel2D.ccFace[i]==CC.ccDir[kcc].rotulo)diri=1;
	else{
		kcc=0;
		while(kcc<CC.nVN && cel2D.ccFace[i]!=CC.ccVN[kcc].rotulo) kcc++;
		if(kcc<CC.nVN && cel2D.ccFace[i]==CC.ccVN[kcc].rotulo)vn=1;
		else{
			kcc=0;
			while(kcc<CC.nRic && cel2D.ccFace[i]!=CC.ccRic[kcc].rotulo) kcc++;
			if(kcc<CC.nRic && cel2D.ccFace[i]==CC.ccRic[kcc].rotulo)rich=1;
			else{
				kcc=0;
				while(kcc<1 && cel2D.ccFace[i]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && cel2D.ccFace[i]==CC.rotuloAcop)acoplado=1;
			}
		}
	}
}

void elem2d::tipoCC(int i,int& inl, int& out, int& wall, int& sim, int& kcc){
	while(kcc<CC.nInl && cel2D.ccFace[i]!=CC.ccInl[kcc].rotulo) kcc++;
	if(kcc<CC.nInl && cel2D.ccFace[i]==CC.ccInl[kcc].rotulo)inl=1;
	else{
		kcc=0;
		while(kcc<CC.nOut && cel2D.ccFace[i]!=CC.ccPres[kcc].rotulo) kcc++;
		if(kcc<CC.nOut && cel2D.ccFace[i]==CC.ccPres[kcc].rotulo)out=1;
		else{
			kcc=0;
			while(kcc<CC.nWall && cel2D.ccFace[i]!=CC.ccWall[kcc].rotulo){
				kcc++;
			}
			if(kcc<CC.nWall && cel2D.ccFace[i]==CC.ccWall[kcc].rotulo){
				wall=1;
			}
			else{
				kcc=0;
				while(kcc<CC.nSim && cel2D.ccFace[i]!=CC.ccSim[kcc].rotulo) kcc++;
				if(kcc<CC.nSim && cel2D.ccFace[i]==CC.ccSim[kcc].rotulo)sim=1;
			}
		}
	}
}

void elem2d::atualizaCCTemp(int i){
	int diri=0;
	int rich=0;
	int vn=0;
	int kcc=0;
	int acoplado=0;
	tipoCCTemp(i, diri, vn, rich,acoplado,kcc);
    int ind=0;
    double raz;
	if(diri==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccDir[kcc].nserie, CC.ccDir[kcc].tempo);
		double inf = CC.ccDir[kcc].val[ind];
		double sup;
		if (ind < CC.ccDir[kcc].nserie - 1){
			sup = CC.ccDir[kcc].val[ind + 1];
			ccTD[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccTD[i] = CC.ccDir[kcc].val[ind];
	}
	else if(vn==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccVN[kcc].nserie, CC.ccVN[kcc].tempo);
		double inf = CC.ccVN[kcc].val[ind];
		double sup;
		if (ind < CC.ccVN[kcc].nserie - 1){
			sup = CC.ccVN[kcc].val[ind + 1];
			ccTVN[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccTVN[i] = CC.ccVN[kcc].val[ind];
	}
	else if(rich==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccRic[kcc].nserie, CC.ccRic[kcc].tempo);
		double inf = CC.ccRic[kcc].valAmb[ind];
		double sup;
		if (ind < CC.ccRic[kcc].nserie - 1){
			sup = CC.ccRic[kcc].valAmb[ind + 1];
			ccTambR[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccTambR[i] = CC.ccRic[kcc].valAmb[ind];


		inf = CC.ccRic[kcc].hAmb[ind];
		if (ind < CC.ccRic[kcc].nserie - 1){
			sup = CC.ccRic[kcc].hAmb[ind + 1];
			ccHR[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccHR[i] = CC.ccRic[kcc].hAmb[ind];

	}
}


void elem2d::atualizaCC(int i){
	int inl=0;
	int out=0;
	int wall=0;
	int sim=0;
	int kcc=0;
	tipoCC(i, inl, out, wall, sim, kcc);
    int ind=0;
    double raz;
	//if(fabsl(cel2D.centroElem[1]-0.03818283448917122)<1e-5){
		//int para;
		//para=0;
	//}
	if(inl==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccInl[kcc].nserie, CC.ccInl[kcc].tempo);
		double infU = CC.ccInl[kcc].valU[ind];
		double infV = CC.ccInl[kcc].valV[ind];
		double supU;
		double supV;
		if (ind < CC.ccInl[kcc].nserie - 1){
			supU = CC.ccInl[kcc].valU[ind + 1];
			supV = CC.ccInl[kcc].valV[ind + 1];
			ccInU[i] = (infU * raz + (1 - raz) * supU);
			ccInV[i] = (infV * raz + (1 - raz) * supV);
		}
		else{
			ccInU[i] = CC.ccInl[kcc].valU[ind];
			ccInV[i] = CC.ccInl[kcc].valV[ind];
		}
	}
	else if(out==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccPres[kcc].nserie, CC.ccPres[kcc].tempo);
		double inf = CC.ccPres[kcc].val[ind];
		double sup;
		if (ind < CC.ccPres[kcc].nserie - 1){
			sup = CC.ccPres[kcc].val[ind + 1];
			ccPres[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccPres[i] = CC.ccPres[kcc].val[ind];
	}
	else if(wall==1){

		indraz(ind, raz, (*vg1dSP).tempo, CC.ccWall[kcc].nserie, CC.ccWall[kcc].tempo);
		double inf = CC.ccWall[kcc].velW[ind];
		double sup;
		if (ind < CC.ccWall[kcc].nserie - 1){
			sup = CC.ccWall[kcc].velW[ind + 1];
			ccWall[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccWall[i] = CC.ccWall[kcc].velW[ind];
	}
}

/*void elem2d::calcForcCorp(){
	cel2D.B2Med[0]=0.;
	cel2D.B2Med[1]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			cel2D.BMedF[i][0]=-(cel2D.fatG[i]*cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
					(1-cel2D.fatG[i])*(*vizinho[i]).rho*(1.-(*vizinho[i]).beta*((*vizinho[i]).tempC-(*vizinho[i]).tempRef)))*grav*sinl(angX);
			cel2D.BMedF[i][1]=-(cel2D.fatG[i]*cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
					(1-cel2D.fatG[i])*(*vizinho[i]).rho*(1.-(*vizinho[i]).beta*((*vizinho[i]).tempC-(*vizinho[i]).tempRef)))*grav*sinl(angY);
		}
		else{
			cel2D.BMedF[i][0]=-(cel2D.rho*(1.-cel2D.beta*(tempF[i]-cel2D.tempRef)))*grav*sinl(angX);
			cel2D.BMedF[i][1]=-(cel2D.rho*(1.-cel2D.beta*(tempF[i]-cel2D.tempRef)))*grav*sinl(angY);
			//cel2D.BMedF[i][0]=-(cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*grav*sinl(angX);
			//cel2D.BMedF[i][1]=-(cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*grav*sinl(angY);
		}
		for(int j=0;j<cel2D.dim;j++){
			if(kvizinho[i]>=0){
				cel2D.B2Med[j]+=cel2D.fatG[i]*cel2D.BMedF[i][j]*cel2D.vecE[i][j]*cel2D.modE[i]*cel2D.sFace[i][j]/cel2D.vElem;
			}
			else{
				//cel2D.B2Med[j]+=cel2D.BMedF[i][j]*cel2D.vecE[i][j]*cel2D.modE[i]*cel2D.sFace[i][j]/cel2D.vElem;
				if(j==1)cel2D.B2Med[j]-=(cel2D.rho*(1.-cel2D.beta*(tempF[i]-cel2D.tempRef)))*grav*sinl(angY)*
						cel2D.vecE[i][j]*cel2D.modE[i]*cel2D.sFace[i][j]/cel2D.vElem;
			}
		}
		//cel2D.B2Med[0]=-cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))*grav*sinl(angX);
		//cel2D.B2Med[1]=-cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))*grav*sinl(angY);
	}
}*/

void elem2d::calcForcCorp(){
	cel2D.B2Med[0]=0.;
	cel2D.B2Med[1]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			cel2D.BMedF[i][1]=-(cel2D.fatG[i]*cel2D.rho*(1*(*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
					(1-cel2D.fatG[i])*(*vizinho[i]).rho*(1*(*vg1dSP).mulFC-
							(*vizinho[i]).beta*((*vizinho[i]).tempC-(*vizinho[i]).tempRef)))*(*vg1dSP).gravVF*sinl((*vg1dSP).angY);
		}
		else{
			cel2D.BMedF[i][1]=-(cel2D.rho*(1*(*vg1dSP).mulFC-cel2D.beta*(tempF[i]-cel2D.tempRef)))*(*vg1dSP).gravVF*sinl((*vg1dSP).angY);
			//cel2D.BMedF[i][0]=-(cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*grav*sinl(angX);
			//cel2D.BMedF[i][1]=-(cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*grav*sinl(angY);
		}
		if(kvizinho[i]>=0){
			cel2D.B2Med[1]+=cel2D.fatG[i]*cel2D.BMedF[i][1]*cel2D.vecE[i][1]*cel2D.modE[i]*cel2D.sFace[i][1]/cel2D.vElem;
			//cel2D.B2Med[j]+=cel2D.BMedF[i][j]*cel2D.subVol[i]/cel2D.vElem;
		}
		else{
			//if(j==1)cel2D.B2Med[j]-=(cel2D.rho*(1*mulFC-cel2D.beta*(tempF[i]-cel2D.tempRef)))*grav*sinl(angY)*
						//cel2D.subVol[i]/cel2D.vElem;
			cel2D.B2Med[1]-=(cel2D.rho*(1*(*vg1dSP).mulFC-cel2D.beta*(tempF[i]-cel2D.tempRef)))*(*vg1dSP).gravVF*sinl((*vg1dSP).angY)*
						cel2D.vecE[i][1]*cel2D.modE[i]*cel2D.sFace[i][1]/cel2D.vElem;
			//if(j==1)cel2D.B2Med[j]-=1*(cel2D.rho*(mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*grav*sinl(angY)*
										//cel2D.vecE[i][j]*cel2D.modE[i]*cel2D.sFace[i][j]/cel2D.vElem;
		}
		//cel2D.B2Med[1]+=(-mulFC*cel2D.rho*grav*sinl(angY));
		//cel2D.B2Med[0]=-cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))*grav*sinl(angX);
		//cel2D.B2Med[1]=-cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))*grav*sinl(angY);
		//for(int i=0;i<cel2D.nvert;i++){
			//cel2D.BMedF[i][1]+=(-mulFC*cel2D.rho*grav*sinl(angY));
		//}
	}
}

void elem2d::calcGradGreenUV(){
	//if(iterPresCor==0)
		//vazMass();
	if(fabsl(cel2D.centroElem[1]-6.0806626583835642 )<1e-5){
		int para;
		para=0;
	}
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			calcVarFaceUV(i);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			uF[i]=0.;
			vF[i]=0.;
			if(inl==1){
				uF[i]=ccInU[i];
				vF[i]=ccInV[i];
			}
			else if(wall==1){
				//////////////////////////////////////////////////////////////////////////////////////////////////
				uF[i]=ccWall[i]*fabsl(-cel2D.sFace[i][1]/cel2D.sFaceMod[i]);
				vF[i]=ccWall[i]*fabsl(cel2D.sFace[i][0]/cel2D.sFaceMod[i]);
			}
			else if(sim==1 || out==1){
				if(out==1){
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenU[j];
					}
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					gradMed[0]-=termoCorda*cel2D.vecE[i][0];
					gradMed[1]-=termoCorda*cel2D.vecE[i][1];
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					 //dúvida, se é para zerar o gradiente, não devceria ser negativo
					 //ao contrário do indicado em Moukalled?
					//if(iterPresCor!=0){
						coefUHRC[i]=1;
						fonteUHR[i]=+termoCorda*cel2D.modE[i];
					//}
					uF[i]=cel2D.uC+termoCorda*cel2D.modE[i];
					//////////////////////////////////////////////////////////////////////////////////////////////////
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenV[j];
					}
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					gradMed[0]-=termoCorda*cel2D.vecE[i][0];
					gradMed[1]-=termoCorda*cel2D.vecE[i][1];
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					//if(iterPresCor!=0){
						coefVHRC[i]=1;
						fonteVHR[i]=+termoCorda*cel2D.modE[i];
					//}
					vF[i]=cel2D.vC+termoCorda*cel2D.modE[i];
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
				else if(sim==1){
					double gradMed [cel2D.dim];
					double cordaArea;
					double termoCorda;
					double escalGradArea;
					/*for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenU[j];
					}
					double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
					uF[i]=cel2D.uC+(-escalGradArea+termoCorda*cordaArea)*cel2D.modE[i]/cordaArea;*/
					uF[i]=cel2D.uC-(cel2D.uC*cel2D.sFace[i][0]+cel2D.vC*cel2D.sFace[i][1])*cel2D.sFace[i][0]/
							(cel2D.sFaceMod[i]*cel2D.sFaceMod[i]);
					//////////////////////////////////////////////////////////////////////////////////////////////////
					/*for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenV[j];
					}
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);*/
					vF[i]=cel2D.vC-(cel2D.uC*cel2D.sFace[i][0]+cel2D.vC*cel2D.sFace[i][1])*cel2D.sFace[i][1]/
							(cel2D.sFaceMod[i]*cel2D.sFaceMod[i]);
				}
			}
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenU[j]=0.;
		cel2D.gradGreenV[j]=0.;
	}
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++){
			cel2D.gradGreenU[j]+=uF[i]*cel2D.sFace[i][j];
			cel2D.gradGreenV[j]+=vF[i]*cel2D.sFace[i][j];
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenU[j]/=cel2D.vElem;
		cel2D.gradGreenV[j]/=cel2D.vElem;
	}

	cel2D.taxaDeform=sqrtl(2.*cel2D.gradGreenU[0]*cel2D.gradGreenU[0]+2.*cel2D.gradGreenV[1]*cel2D.gradGreenV[1]+
			(cel2D.gradGreenU[1]+cel2D.gradGreenV[0])*(cel2D.gradGreenU[1]+cel2D.gradGreenV[0]));
}

void elem2d::calcGradGreenPres(){
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			calcVarFacePres(i);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			presF[i]=0.;
			double forcCorp=-1*(cel2D.rho*((*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*(*vg1dSP).gravVF*sinl((*vg1dSP).angY);
			if(inl==1){
				if((*vg1dSP).partidaVF>=0){
					/*double vazCentro=(cel2D.uC*cel2D.sFace[i][0]+cel2D.vC*cel2D.sFace[i][1])*cel2D.rho;
					double vazContor=(uF[i]*cel2D.sFace[i][0]+vF[i]*cel2D.sFace[i][1])*cel2D.rho;
					double cordaAreaDif=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenPres[j];
					}
					double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
					presF[i]=cel2D.presC+termoCorda*cel2D.modE[i];
					presF[i]+=(vazCentro-vazContor)*cel2D.modE[i]/cordaAreaDif;*/
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenPres[j];
					}
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					presF[i]=cel2D.presC+(*vg1dSP).corrigeDistor*termoCorda*cel2D.modE[i];
				}
				else{
					presF[i]=cel2D.presC;
				}
			}
			else if(wall==1){
				//////////////////////////////////////////////////////////////////////////////////////////////////
				if((*vg1dSP).partidaVF>=0){
					/*double vazCentro=(cel2D.uC*cel2D.sFace[i][0]+cel2D.vC*cel2D.sFace[i][1])*cel2D.rho;
					double cordaAreaDif=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenPres[j];
					}
					double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
					presF[i]=cel2D.presC+termoCorda*cel2D.modE[i];
					presF[i]+=vazCentro*cel2D.modE[i]/cordaAreaDif;*/

					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenPres[j];
					}
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					//if(confinado[i]==0)
						presF[i]=cel2D.presC+(*vg1dSP).corrigeDistor*termoCorda*cel2D.modE[i]+
								(1.-(*vg1dSP).corrigeDistor)*(0.*cel2D.B2Med[1]+1.*forcCorp)*(cel2D.centroideFace[i][1]-cel2D.centroideElem[1]);
					//else presF[i]=10.*98066.52;
				}
				else{
					//if(confinado[i]==0)
						presF[i]=cel2D.presC+
								(1.-(*vg1dSP).corrigeDistor)*(0.*cel2D.B2Med[1]+1.*forcCorp)*(cel2D.centroideFace[i][1]-cel2D.centroideElem[1]);
					//else presF[i]=10.*98066.52;
				}
			}
			else if(sim==1 || out==1){
				if(out==1){
					presF[i]=ccPres[i];
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
				else if(sim==1){
					double gradMed [cel2D.dim];
					double cordaArea;
					double termoCorda;
					double escalGradArea;
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenPres[j];
					}
					escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
					gradMed[0]-=escalGradArea*cel2D.sFace[i][0]/(cel2D.sFaceMod[i]*cel2D.sFaceMod[i]);
					gradMed[1]-=escalGradArea*cel2D.sFace[i][1]/(cel2D.sFaceMod[i]*cel2D.sFaceMod[i]);
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					presF[i]=cel2D.presC+0*termoCorda*cel2D.modE[i];
				}
			}
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenPres[j]=0.;
	}
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++){
			cel2D.gradGreenPres[j]+=presF[i]*cel2D.sFace[i][j];
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenPres[j]/=cel2D.vElem;
	}
}

void elem2d::calcGradGreenTemp(int inicia){
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			if(inicia==0)calcVarFaceTemp(i);
		}
		else{
			int diri=0;
			int rich=0;
			int vn=0;
			int kcc=0;
			int acoplado=0;
			tipoCCTemp(i, diri, vn, rich,acoplado,kcc);
			if(diri==1){
				tempF[i]=ccTD[i];
			}
			else if(vn==1 || acoplado==1){
				double condHarm=cel2D.cond;
				//double gradareaB=-ccTVN[i]/condHarm;
				double gradareaB=ccTVN[i]*cel2D.sFaceMod[i]/condHarm;

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				extrapSuaveCCN=(termoCorda)*cel2D.modE[i];
				//tempF[i]=cel2D.tempC+(gradareaB-escalGradArea+cel2D.sFaceMod[i]*termoCorda)*cel2D.modE[i]/cel2D.sFaceMod[i];
				coefTHRC[i]=1.;
				fonteTHR[i]=(gradareaB-escalGradArea+termoCorda*cordaArea)*(cel2D.modE[i]/cordaArea);
				tempF[i]=cel2D.tempC+fonteTHR[i];

				/*double condHarm=cel2D.cond;
				//double gradareaB=-ccTVN[i]/condHarm;???????
				double gradareaB=ccTVN[i]/condHarm;//fluxo negativo, saindo do sólido, grad negativo

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				//tempF[i]=cel2D.tempC+(gradareaB-escalGradArea+cel2D.sFaceMod[i]*termoCorda)*cel2D.modE[i]/cel2D.sFaceMod[i];
				coefTHRC[i]=1.;
				fonteTHR[i]=(gradareaB-escalGradArea+termoCorda*cordaArea)*cel2D.modE[i]/cordaArea;*/
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/*double condHarm=cel2D.cond;
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				coefTHRC[i]=1.;
				fonteTHR[i]=(termoCorda)*cel2D.modE[i];
				//fonteTHR[i]=ccTVN[i]*cel2D.modE[i];
				tempF[i]=(cel2D.tempC+0*fonteTHR[i]);
				if((*vg1dSP).tempo>40){
					int para;
					para=0;
				}
				tempF[i]=cel2D.tempC+ccTVN[i]*cel2D.modE[i]/condHarm;*/
				/*
				if((*vg1dSP).tempo<10)
				tempF[i]=(cel2D.tempC+fonteTHR[i]);
				else if((*vg1dSP).tempo<20){
					double raz=(20.-(*vg1dSP).tempo)/10.;
					tempF[i]=cel2D.tempC+(1.-raz)*ccTVN[i]*cel2D.modE[i]/condHarm+raz*fonteTHR[i];
				}
				else
				tempF[i]=cel2D.tempC+ccTVN[i]*cel2D.modE[i]/condHarm;*/
			}
			else if(rich==1){
				/*double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double denom=(cel2D.cond*cordaArea/cel2D.modE[i])+ccHR[i]*cel2D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double correrT=escalGradArea-termoCorda*cordaArea;
				double num=ccHR[i]*cel2D.sFaceMod[i]*ccTambR[i]+
						cel2D.cond*cordaArea*cel2D.tempC/cel2D.modE[i]-cel2D.cond*correrT;

				coefTHRC[i]=(cel2D.cond*cordaArea/cel2D.modE[i])/denom;
				fonteTHR[i]=(ccHR[i]*cel2D.sFaceMod[i]*ccTambR[i]-cel2D.cond*correrT)/denom;
				tempF[i]=num/denom;*/

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double denom=(cel2D.cond*cordaArea/cel2D.modE[i])+ccHR[i]*cel2D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double correrT=escalGradArea-termoCorda*cordaArea;
				double num=ccHR[i]*cel2D.sFaceMod[i]*ccTambR[i]+
						cel2D.cond*cordaArea*cel2D.tempC/cel2D.modE[i]-cel2D.cond*correrT;

				coefTHRC[i]=(cel2D.cond*cordaArea/cel2D.modE[i])/denom;
				fonteTHR[i]=(ccHR[i]*cel2D.sFaceMod[i]*ccTambR[i]-cel2D.cond*correrT)/denom;
				tempF[i]=num/denom;
			}
		}
	}
	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTemp[j]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTemp[j]+=tempF[i]*cel2D.sFace[i][j];
	}
	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTemp[j]/=cel2D.vElem;
}

void elem2d::calcGradGreenPresCor(){
	//vazMass();
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			calcVarFacePresCor(i);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			presFcor[i]=0.;
			if(wall==1 || inl==1 || sim==1){
				/*if(partidaVF>0){
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenPresCor[j];
					}
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					presFcor[i]=cel2D.presCcor+termoCorda*cel2D.modE[i];
				}
				else{
					presFcor[i]=cel2D.presCcor;
				}*/
				presFcor[i]=cel2D.presCcor;
			}
			else presFcor[i]=0.;
		}
	}
	for(int j=0; j<cel2D.dim;j++) cel2D.gradGreenPresCor[j]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++) cel2D.gradGreenPresCor[j]+=presFcor[i]*cel2D.sFace[i][j];
	}
	for(int j=0; j<cel2D.dim;j++) cel2D.gradGreenPresCor[j]/=cel2D.vElem;
}

void elem2d::calcGradGreenHol(int inicia){
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			if(inicia==0)calcVarFaceHol(i);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			holF[i]=0.;
			if(inl==1){
				holF[i]=1.;//a ser modificado, seria o valor de entrada
			}
			else if(wall==1){
				//////////////////////////////////////////////////////////////////////////////////////////////////
				if((*vg1dSP).partidaVF>=0){
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenHol[j];
					}
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
						//holF[i]=cel2D.holC+termoCorda*cel2D.modE[i];
					holF[i]=cel2D.holC;
				}
				else{
						holF[i]=cel2D.holC;
				}
			}
			else if(sim==1 || out==1){
				if(out==1){
					double gradMed [cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenHol[j];
					}
					double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					gradMed[0]-=termoCorda*cel2D.vecE[i][0];
					gradMed[1]-=termoCorda*cel2D.vecE[i][1];
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					 //dúvida, se é para zerar o gradiente, não devceria ser negativo
					 //ao contrário do indicado em Moukalled?
					//holF[i]=cel2D.holC+termoCorda*cel2D.modE[i];
					holF[i]=cel2D.holC;
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
				else if(sim==1){
					double gradMed [cel2D.dim];
					double cordaArea;
					double termoCorda;
					double escalGradArea;
					for(int j=0; j<cel2D.dim;j++){
						gradMed[j]=cel2D.gradGreenHol[j];
					}
					escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
					gradMed[0]-=escalGradArea*cel2D.sFace[i][0]/(cel2D.sFaceMod[i]*cel2D.sFaceMod[i]);
					gradMed[1]-=escalGradArea*cel2D.sFace[i][1]/(cel2D.sFaceMod[i]*cel2D.sFaceMod[i]);
					termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
					//holF[i]=cel2D.holC+termoCorda*cel2D.modE[i];
					holF[i]=cel2D.holC;
				}
			}
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenHol[j]=0.;
	}
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++){
			cel2D.gradGreenHol[j]+=holF[i]*cel2D.sFace[i][j];
		}
	}
	for(int j=0; j<cel2D.dim;j++){
		cel2D.gradGreenHol[j]/=cel2D.vElem;
	}
}

int elem2d::achaInd(int i){
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

void elem2d::calSupDifu(){
	double Dmed[2];
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			for(int j=0; j<cel2D.dim;j++){
				Dmed[j]=cel2D.fatG[i]*cel2D.difuPres[j]+(1-cel2D.fatG[i])*(*vizinho[i]).difuPres[j];
			}
		}
		else{
			for(int j=0; j<cel2D.dim;j++){
				Dmed[j]=cel2D.difuPres[j];
			}
		}
		for(int j=0; j<cel2D.dim;j++)cel2D.vecSDif[i][j]=Dmed[j]*cel2D.sFace[i][j];
		cel2D.vecSDifMod[i]=0.;
		for(int j=0; j<cel2D.dim;j++)cel2D.vecSDifMod[i]+=(cel2D.vecSDif[i][j]*cel2D.vecSDif[i][j]);
		cel2D.vecSDifMod[i]=sqrtl(cel2D.vecSDifMod[i]);
	}
}

void elem2d::explicitUV(){
	TLU[0]=0.;
	TLV[0]=0.;
	int diag=achaInd(cel2D.indEle);
	localU.mx[0][diag]=0.;
	localV.mx[0][diag]=0.;
	if(fabsl(cel2D.centroElem[1]-6.0806626583835642 )<1e-5){
		int para;
		para=0;
	}
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMedU [cel2D.dim];
			double gradMedV [cel2D.dim];
			//double gradMedTransU[cel2D.dim];
			//double gradMedTransV[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMedU[j]=cel2D.fatG[i]*cel2D.gradGreenU[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenU[j];
				gradMedV[j]=cel2D.fatG[i]*cel2D.gradGreenV[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenV[j];
			}
			//gradMedTransU[0]=gradMedU[0];
			//gradMedTransU[1]=gradMedV[0];
			//gradMedTransV[0]=gradMedU[1];
			//gradMedTransV[1]=gradMedV[1];
			double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
			double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
			//gradMedTransU[0]=gradMedTransU[0]+(((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][0];
			//gradMedTransU[1]=gradMedTransU[1]+(((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][0];
			//gradMedTransV[0]=gradMedTransV[0]+(((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][1];
			//gradMedTransV[1]=gradMedTransV[1]+(((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][1];
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			viscHarm=1./viscHarm;
			double escalGradAreaU=escalar(gradMedU,cel2D.sFace[i],cel2D.dim);
			double escalGradAreaV=escalar(gradMedV,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			//double escalGradAreaTransU=escalar(gradMedTransU,cel2D.sFace[i],cel2D.dim);
			//double escalGradAreaTransV=escalar(gradMedTransV,cel2D.sFace[i],cel2D.dim);
			TLU[0]+=viscHarm*(escalGradAreaU-termoCordaU*cordaArea/*+escalGradAreaTransU*/);
			TLU[0]-=(fonteUHR[i]/*+cel2D.gradGreenPres[0]*cel2D.vElem*/);
			TLV[0]+=viscHarm*(escalGradAreaV-termoCordaV*cordaArea/*+escalGradAreaTransV*/);
			TLV[0]-=(fonteVHR[i]/*+cel2D.gradGreenPres[1]*cel2D.vElem*/);
			double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
			TLU[0]-=(-termMat+coefUHRV[i])*(*vizinho[i]).uCI;
			localU.mx[0][diag]+=(termMat+coefUHRC[i]);
			TLV[0]-=(-termMat+coefVHRV[i])*(*vizinho[i]).vCI;
			localV.mx[0][diag]+=(termMat+coefVHRC[i]);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
		    //int ind;
		    //double raz;
			//int col=achaInd(cel2D.indFace[i]);
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			//viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			//viscHarm=1./viscHarm;
			viscHarm=cel2D.visc;
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			if(wall!=1){
				if(inl==1){

					double gradMedU [cel2D.dim];
					double gradMedV [cel2D.dim];
					//double gradMedTransU[cel2D.dim];
					//double gradMedTransV[cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]=cel2D.gradGreenU[j];
						gradMedV[j]=cel2D.gradGreenV[j];
					}
					double escalGradAreaU=escalar(gradMedU,cel2D.sFace[i],cel2D.dim);
					double escalGradAreaV=escalar(gradMedV,cel2D.sFace[i],cel2D.dim);
					//gradMedTransU[0]=gradMedU[0];
					//gradMedTransU[1]=gradMedV[0];
					//gradMedTransV[0]=gradMedU[1];
					//gradMedTransV[1]=gradMedV[1];
					double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
					double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
					//gradMedTransU[0]=gradMedTransU[0]+((uRC[i]-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][0];
					//gradMedTransU[1]=gradMedTransU[1]+((vRC[i]-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][0];
					//gradMedTransV[0]=gradMedTransV[0]+((uRC[i]-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][1];
					//gradMedTransV[1]=gradMedTransV[1]+((vRC[i]-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][1];
					//double escalGradAreaTransU=escalar(gradMedTransU,cel2D.sFace[i],cel2D.dim);
					//double escalGradAreaTransV=escalar(gradMedTransV,cel2D.sFace[i],cel2D.dim);

					TLU[0]+=viscHarm*(escalGradAreaU-termoCordaU*cordaArea/*+escalGradAreaTransU*/);
					TLU[0]-=(massF[i]*uRC[i]/*+cel2D.gradGreenPres[0]*cel2D.vElem*/);
					double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
					TLU[0]+=termMat*uRC[i];
					localU.mx[0][diag]+=(termMat);

					TLV[0]+=viscHarm*(escalGradAreaV-termoCordaV*cordaArea/*+escalGradAreaTransV*/);
					TLV[0]-=(massF[i]*vRC[i]/*+cel2D.gradGreenPres[1]*cel2D.vElem*/);
					TLV[0]+=termMat*vRC[i];
					localV.mx[0][diag]+=(termMat);
				}
				else if(sim==1){
					cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]/cel2D.sFaceMod[i];
					double mod2=cel2D.sFaceMod[i]*cel2D.sFaceMod[i];

					TLU[0]-=(2*viscHarm*cel2D.sFaceMod[i]*cel2D.vC/cordaArea)*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2;
					localU.mx[0][diag]+=(2*viscHarm*cel2D.sFaceMod[i]/cordaArea)*cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2;

					TLV[0]-=(2*viscHarm*cel2D.sFaceMod[i]*cel2D.uC/cordaArea)*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2;
					localV.mx[0][diag]+=(2*viscHarm*cel2D.sFaceMod[i]/cordaArea)*cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2;

				}
				else if(out==1){

					double gradMedU [cel2D.dim];
					double gradMedV [cel2D.dim];

					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]=cel2D.gradGreenU[j];
						gradMedV[j]=cel2D.gradGreenV[j];
					}
					double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
					double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]-=termoCordaU*cel2D.vecE[i][j];
						gradMedV[j]-=termoCordaV*cel2D.vecE[i][j];
					}

					double escalGradAreaU=escalar(gradMedU,cel2D.sFace[i],cel2D.dim);
					double escalGradAreaV=escalar(gradMedV,cel2D.sFace[i],cel2D.dim);


					TLU[0]-=(massF[i]*fonteUHR[i]-0*viscHarm*escalGradAreaU/*+cel2D.gradGreenPres[0]*cel2D.vElem*/);
					localU.mx[0][diag]+=(massF[i]*coefUHRC[i]);

					TLV[0]-=(massF[i]*fonteVHR[i]-0*viscHarm*escalGradAreaV/*+cel2D.gradGreenPres[1]*cel2D.vElem*/);
					localV.mx[0][diag]+=(massF[i]*coefVHRC[i]);
				}
			}
			else{
				//cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]/cel2D.sFaceMod[i];
				double mod2=cel2D.sFaceMod[i]*cel2D.sFaceMod[i];
				cordaArea=mod2/(escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]);

				TLU[0]+=viscHarm*cordaArea*(uRC[i]*(1-cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2)+
						(cel2D.vC-vRC[i])*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				/*TLU[0]-=cel2D.gradGreenPres[0]*cel2D.vElem;*/
				localU.mx[0][diag]+=viscHarm*cordaArea*(1-cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2);

				TLV[0]+=viscHarm*cordaArea*(vRC[i]*(1-cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2)+
						(cel2D.uC-uRC[i])*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				/*TLV[0]-=cel2D.gradGreenPres[1]*cel2D.vElem;*/
				localV.mx[0][diag]+=viscHarm*cordaArea*(1-cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2);
			}
		}
	}
	TLU[0]-=cel2D.gradGreenPres[0]*cel2D.vElem;
	TLV[0]-=cel2D.gradGreenPres[1]*cel2D.vElem;
	TLU[0]+=cel2D.B2Med[0]*cel2D.vElem;
	TLV[0]+=cel2D.B2Med[1]*cel2D.vElem;
	if(perm==0 && trans==1){
		cel2D.a0U=cel2D.vElem*(cel2D.rho/dt);
		TLU[0]+=cel2D.a0U*cel2D.uC0;
		localU.mx[0][diag]+=cel2D.a0U;
		//cel2D.a0U/=localU.mx[0][diag];
		cel2D.a0V=cel2D.vElem*(cel2D.rho/dt);
		TLV[0]+=cel2D.a0V*cel2D.vC0;
		localV.mx[0][diag]+=cel2D.a0V;
		//cel2D.a0V/=localV.mx[0][diag];
	}
	//cel2D.difuPres[0]=cel2D.vElem/localU.mx[0][diag];
	//cel2D.difuPres[1]=cel2D.vElem/localV.mx[0][diag];
	double acTemp=localU.mx[0][diag];
	localU.mx[0][diag]=acTemp/(*vg1dSP).relaxVF;
	TLU[0]+=((1-(*vg1dSP).relaxVF)/(*vg1dSP).relaxVF)*acTemp*cel2D.uCI;
	acTemp=localV.mx[0][diag];
	localV.mx[0][diag]=acTemp/(*vg1dSP).relaxVF;
	TLV[0]+=((1-(*vg1dSP).relaxVF)/(*vg1dSP).relaxVF)*acTemp*cel2D.vCI;
	cel2D.difuPresRC[0]=cel2D.difuPres[0]=cel2D.vElem/localU.mx[0][diag];
	cel2D.difuPresRC[1]=cel2D.difuPres[1]=cel2D.vElem/localV.mx[0][diag];
	cel2D.a0U=cel2D.a0U/localU.mx[0][diag];
	cel2D.a0V=cel2D.a0V/localV.mx[0][diag];
	cel2D.uC=TLU[0]/localU.mx[0][diag];
	cel2D.vC=TLV[0]/localV.mx[0][diag];
}


void elem2d::GeraLocalUV(double rlx){
	TLU[0]=0.;
	TLV[0]=0.;
	cel2D.uCEx=0.;
	cel2D.vCEx=0.;
	int diag=achaInd(cel2D.indEle);
	localU.mx[0][diag]=0.;
	localV.mx[0][diag]=0.;
	if(fabsl(cel2D.centroElem[1]-6.0806626583835642 )<1e-5){
		int para;
		para=0;
	}
	for(int i=0;i<cel2D.nvert;i++){
		cel2D.HcU[i]=0.;
		cel2D.HcV[i]=0.;
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMedU [cel2D.dim];
			double gradMedV [cel2D.dim];
			double gradFaceTU [cel2D.dim];
			double gradFaceTV [cel2D.dim];
			//double gradMedTransU[cel2D.dim];
			//double gradMedTransV[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMedU[j]=cel2D.fatG[i]*cel2D.gradGreenU[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenU[j];
				gradMedV[j]=cel2D.fatG[i]*cel2D.gradGreenV[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenV[j];
			}
			//gradMedTransU[0]=gradMedU[0];
			//gradMedTransU[1]=gradMedV[0];
			//gradMedTransV[0]=gradMedU[1];
			//gradMedTransV[1]=gradMedV[1];
			double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
			double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);

			double dUdE=((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i];
			double dVdE=((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i];

			for(int j=0; j<cel2D.dim;j++){
				gradUface[i][j]=gradMedU[j]+(dUdE-termoCordaU)*cel2D.vecE[i][j];
				gradVface[i][j]=gradMedV[j]+(dVdE-termoCordaV)*cel2D.vecE[i][j];
			}
			gradFaceTU[0]=gradUface[i][0];
			gradFaceTU[1]=gradVface[i][0];
			double forcTransU=escalar(gradFaceTU,cel2D.sFace[i],cel2D.dim);
			gradFaceTV[0]=gradUface[i][1];
			gradFaceTV[1]=gradVface[i][1];
			double forcTransV=escalar(gradFaceTV,cel2D.sFace[i],cel2D.dim);
			taxadeformFace[i]=sqrtl(2.*gradUface[i][0]*gradUface[i][0]+2.*gradVface[i][1]*gradVface[i][1]+
					(gradUface[i][1]+gradVface[i][0])*(gradUface[i][1]+gradVface[i][0]));

			//gradMedTransU[0]=gradMedTransU[0]+(((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][0];
			//gradMedTransU[1]=gradMedTransU[1]+(((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][0];
			//gradMedTransV[0]=gradMedTransV[0]+(((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][1];
			//gradMedTransV[1]=gradMedTransV[1]+(((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][1];
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			if((*vg1dSP).acop!=1){
				viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
				viscHarm=viscFace[i]=1./viscHarm;
			}
			else{
				double pres=presF[i];
				double temp=tempF[i];
				double taxaDef=taxadeformFace[i];
				viscHarm=viscFace[i]=holF[i]*flucVF.VisFlu(pres, temp,taxaDef)/1000.+
					(1.-holF[i])*flucVF.VisGas(pres, temp)/1000.;
			}
			//viscHarm=(*vizinho[i]).visc;
			double escalGradAreaU=escalar(gradMedU,cel2D.sFace[i],cel2D.dim);
			double escalGradAreaV=escalar(gradMedV,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			//double escalGradAreaTransU=escalar(gradMedTransU,cel2D.sFace[i],cel2D.dim);
			//double escalGradAreaTransV=escalar(gradMedTransV,cel2D.sFace[i],cel2D.dim);
			TLU[0]+=viscHarm*(escalGradAreaU-termoCordaU*cordaArea/*+escalGradAreaTransU*/);
			TLU[0]-=(fonteUHR[i]/*+cel2D.gradGreenPres[0]*cel2D.vElem*/);
			TLU[0]+=viscHarm*forcTransU;
			TLV[0]+=viscHarm*(escalGradAreaV-termoCordaV*cordaArea/*+escalGradAreaTransV*/);
			TLV[0]-=(fonteVHR[i]/*+cel2D.gradGreenPres[1]*cel2D.vElem*/);
			TLV[0]+=viscHarm*forcTransV;
			double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
			localU.mx[0][col]=-termMat+coefUHRV[i];
			localU.mx[0][diag]+=(termMat+coefUHRC[i]);
			localV.mx[0][col]=-termMat+coefVHRV[i];
			localV.mx[0][diag]+=(termMat+coefVHRC[i]);
			cel2D.HcU[i]=localU.mx[0][col];
			cel2D.HcV[i]=localV.mx[0][col];
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
		    //int ind;
		    //double raz;
			//int col=achaInd(cel2D.indFace[i]);
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			//viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			//viscHarm=1./viscHarm;
			viscHarm=viscFace[i]=cel2D.visc;
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			if(wall!=1){
				if(inl==1){

					double gradMedU [cel2D.dim];
					double gradMedV [cel2D.dim];
					//double gradMedTransU[cel2D.dim];
					//double gradMedTransV[cel2D.dim];
					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]=cel2D.gradGreenU[j];
						gradMedV[j]=cel2D.gradGreenV[j];
					}
					double escalGradAreaU=escalar(gradMedU,cel2D.sFace[i],cel2D.dim);
					double escalGradAreaV=escalar(gradMedV,cel2D.sFace[i],cel2D.dim);
					//gradMedTransU[0]=gradMedU[0];
					//gradMedTransU[1]=gradMedV[0];
					//gradMedTransV[0]=gradMedU[1];
					//gradMedTransV[1]=gradMedV[1];
					double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
					double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);

					double dUdE=(uRC[i]-cel2D.uC)/cel2D.modE[i];
					double dVdE=(vRC[i]-cel2D.vC)/cel2D.modE[i];

					for(int j=0; j<cel2D.dim;j++){
						gradUface[i][j]=gradMedU[j]+(dUdE-termoCordaU)*cel2D.vecE[i][j];
						gradVface[i][j]=gradMedV[j]+(dVdE-termoCordaV)*cel2D.vecE[i][j];
					}
					taxadeformFace[i]=sqrtl(2.*gradUface[i][0]*gradUface[i][0]+2.*gradVface[i][1]*gradVface[i][1]+
										(gradUface[i][1]+gradVface[i][0])*(gradUface[i][1]+gradVface[i][0]));
					if((*vg1dSP).acop==1){
						double pres=presF[i];
						double temp=tempF[i];
						double taxaDef=taxadeformFace[i];
						viscHarm=viscFace[i]=holF[i]*flucVF.VisFlu(pres, temp,taxaDef)/1000.+
							(1.-holF[i])*flucVF.VisGas(pres, temp)/1000.;
					}
					//gradMedTransU[0]=gradMedTransU[0]+((uRC[i]-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][0];
					//gradMedTransU[1]=gradMedTransU[1]+((vRC[i]-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][0];
					//gradMedTransV[0]=gradMedTransV[0]+((uRC[i]-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][1];
					//gradMedTransV[1]=gradMedTransV[1]+((vRC[i]-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][1];
					//double escalGradAreaTransU=escalar(gradMedTransU,cel2D.sFace[i],cel2D.dim);
					//double escalGradAreaTransV=escalar(gradMedTransV,cel2D.sFace[i],cel2D.dim);

					TLU[0]+=viscHarm*(escalGradAreaU-termoCordaU*cordaArea/*+escalGradAreaTransU*/);
					TLU[0]-=(massF[i]*uRC[i]/*+cel2D.gradGreenPres[0]*cel2D.vElem*/);
					double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
					TLU[0]+=termMat*uRC[i];
					localU.mx[0][diag]+=(termMat);

					TLV[0]+=viscHarm*(escalGradAreaV-termoCordaV*cordaArea/*+escalGradAreaTransV*/);
					TLV[0]-=(massF[i]*vRC[i]/*+cel2D.gradGreenPres[1]*cel2D.vElem*/);
					TLV[0]+=termMat*vRC[i];
					localV.mx[0][diag]+=(termMat);
				}
				else if(sim==1){

					double gradMedU [cel2D.dim];
					double gradMedV [cel2D.dim];

					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]=cel2D.gradGreenU[j];
						gradMedV[j]=cel2D.gradGreenV[j];
					}
					double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
					double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]-=termoCordaU*cel2D.vecE[i][j];
						gradUface[i][j]=gradMedU[j];
						gradMedV[j]-=termoCordaV*cel2D.vecE[i][j];
						gradVface[i][j]=gradMedV[j];
					}
					taxadeformFace[i]=sqrtl(2.*gradUface[i][0]*gradUface[i][0]+2.*gradVface[i][1]*gradVface[i][1]+
										(gradUface[i][1]+gradVface[i][0])*(gradUface[i][1]+gradVface[i][0]));
					if((*vg1dSP).acop==1){
						double pres=presF[i];
						double temp=tempF[i];
						double taxaDef=taxadeformFace[i];
						viscHarm=viscFace[i]=holF[i]*flucVF.VisFlu(pres, temp,taxaDef)/1000.+
							(1.-holF[i])*flucVF.VisGas(pres, temp)/1000.;
					}

					cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]/cel2D.sFaceMod[i];
					double mod2=cel2D.sFaceMod[i]*cel2D.sFaceMod[i];

					TLU[0]-=(2*viscHarm*cel2D.sFaceMod[i]*cel2D.vC/cordaArea)*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2;
					localU.mx[0][diag]+=(2*viscHarm*cel2D.sFaceMod[i]/cordaArea)*cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2;

					TLV[0]-=(2*viscHarm*cel2D.sFaceMod[i]*cel2D.uC/cordaArea)*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2;
					localV.mx[0][diag]+=(2*viscHarm*cel2D.sFaceMod[i]/cordaArea)*cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2;

					/*double forcBU=-(2.*viscHarm*cel2D.sFaceMod[i]/cel2D.modE[i])*cel2D.sFace[i][0]/cel2D.sFaceMod[i];
					TLU[0]-=(massF[i]*fonteUHR[i]+forcBU*cel2D.vC*cel2D.sFace[i][1]/cel2D.sFaceMod[i]);
					localU.mx[0][diag]+=(massF[i]*coefUHRC[i]);
					localU.mx[0][diag]-=forcBU*cel2D.sFace[i][0]/cel2D.sFaceMod[i];

					double forcBV=-(2.*viscHarm*cel2D.sFaceMod[i]/cel2D.modE[i])*cel2D.sFace[i][1]/cel2D.sFaceMod[i];
					TLV[0]-=(massF[i]*fonteVHR[i]+forcBV*cel2D.vC*cel2D.sFace[i][0]/cel2D.sFaceMod[i]);
					localV.mx[0][diag]+=(massF[i]*coefVHRC[i]);
					localV.mx[0][diag]-=forcBV*cel2D.sFace[i][1]/cel2D.sFaceMod[i];*/
				}
				else if(out==1){

					double gradMedU [cel2D.dim];
					double gradMedV [cel2D.dim];

					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]=cel2D.gradGreenU[j];
						gradMedV[j]=cel2D.gradGreenV[j];
					}
					double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
					double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
					for(int j=0; j<cel2D.dim;j++){
						gradMedU[j]-=termoCordaU*cel2D.vecE[i][j];
						gradUface[i][j]=gradMedU[j];
						gradMedV[j]-=termoCordaV*cel2D.vecE[i][j];
						gradVface[i][j]=gradMedV[j];
					}
					taxadeformFace[i]=sqrtl(2.*gradUface[i][0]*gradUface[i][0]+2.*gradVface[i][1]*gradVface[i][1]+
										(gradUface[i][1]+gradVface[i][0])*(gradUface[i][1]+gradVface[i][0]));
					//if((*vg1dSP).acop==1){
						//double pres=presF[i];
						//double temp=tempF[i];
						//double taxaDef=taxadeformFace[i];
						//viscHarm=viscFace[i]=holF[i]*flucVF.VisFlu(pres, temp,taxaDef)/1000.+
						//	(1.-holF[i])*flucVF.VisGas(pres, temp)/1000.;
					//}

					double escalGradAreaU=escalar(gradMedU,cel2D.sFace[i],cel2D.dim);
					double escalGradAreaV=escalar(gradMedV,cel2D.sFace[i],cel2D.dim);


					TLU[0]-=(massF[i]*fonteUHR[i]-0*viscHarm*escalGradAreaU/*+cel2D.gradGreenPres[0]*cel2D.vElem*/);
					localU.mx[0][diag]+=(massF[i]*coefUHRC[i]);

					TLV[0]-=(massF[i]*fonteVHR[i]-0*viscHarm*escalGradAreaV/*+cel2D.gradGreenPres[1]*cel2D.vElem*/);
					localV.mx[0][diag]+=(massF[i]*coefVHRC[i]);
				}
			}
			else{

				double gradMedU [cel2D.dim];
				double gradMedV [cel2D.dim];
				//double gradMedTransU[cel2D.dim];
				//double gradMedTransV[cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMedU[j]=cel2D.gradGreenU[j];
					gradMedV[j]=cel2D.gradGreenV[j];
				}
				//gradMedTransU[0]=gradMedU[0];
				//gradMedTransU[1]=gradMedV[0];
				//gradMedTransV[0]=gradMedU[1];
				//gradMedTransV[1]=gradMedV[1];
				double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
				double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);

				double dUdE=(uRC[i]-cel2D.uC)/cel2D.modE[i];
				double dVdE=(vRC[i]-cel2D.vC)/cel2D.modE[i];

				for(int j=0; j<cel2D.dim;j++){
					gradUface[i][j]=gradMedU[j]+(dUdE-termoCordaU)*cel2D.vecE[i][j];
					gradVface[i][j]=gradMedV[j]+(dVdE-termoCordaV)*cel2D.vecE[i][j];
				}
				taxadeformFace[i]=sqrtl(2.*gradUface[i][0]*gradUface[i][0]+2.*gradVface[i][1]*gradVface[i][1]+
									(gradUface[i][1]+gradVface[i][0])*(gradUface[i][1]+gradVface[i][0]));


				if((*vg1dSP).acop==1){
					double pres=presF[i];
					double temp=tempF[i];
					double taxaDef=taxadeformFace[i];
					viscHarm=viscFace[i]=holF[i]*flucVF.VisFlu(pres, temp,taxaDef)/1000.+
						(1.-holF[i])*flucVF.VisGas(pres, temp)/1000.;
				}

				//cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]/cel2D.sFaceMod[i];
				double mod2=cel2D.sFaceMod[i]*cel2D.sFaceMod[i];
				cordaArea=mod2/(escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]);

				TLU[0]+=viscHarm*cordaArea*(uRC[i]*(1-cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2)+
						(cel2D.vC-vRC[i])*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				TLU[0]-=2.*viscHarm*cordaArea*(cel2D.uC*cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2+
						cel2D.vC*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				/*TLU[0]-=cel2D.gradGreenPres[0]*cel2D.vElem;*/
				localU.mx[0][diag]+=viscHarm*cordaArea*(1-cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2);

				TLV[0]+=viscHarm*cordaArea*(vRC[i]*(1-cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2)+
						(cel2D.uC-uRC[i])*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				TLU[0]-=2.*viscHarm*cordaArea*(cel2D.uC*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2+
						cel2D.vC*cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2);
				/*TLV[0]-=cel2D.gradGreenPres[1]*cel2D.vElem;*/
				localV.mx[0][diag]+=viscHarm*cordaArea*(1-cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2);
			}
		}
	}
	TLU[0]-=cel2D.gradGreenPres[0]*cel2D.vElem;
	TLV[0]-=cel2D.gradGreenPres[1]*cel2D.vElem;
	TLU[0]+=cel2D.B2Med[0]*cel2D.vElem;
	TLV[0]+=(0.*cel2D.B2Med[1]*cel2D.vElem-
			1.*(cel2D.rho*((*vg1dSP).mulFC-cel2D.beta*(cel2D.tempC-cel2D.tempRef)))*(*vg1dSP).gravVF*sinl((*vg1dSP).angY)*cel2D.vElem);
	double rhoC=cel2D.rho*(1.-0*cel2D.beta*(cel2D.tempC-cel2D.tempRef));
	if(perm==0 && trans==1){
		//cel2D.a0U=cel2D.vElem*(cel2D.rho/dt);
		cel2D.a0U=cel2D.vElem*(rhoC/dt);
		TLU[0]+=cel2D.a0U*cel2D.uC0;
		localU.mx[0][diag]+=cel2D.a0U;
		//cel2D.a0U/=localU.mx[0][diag];
		//cel2D.a0V=cel2D.vElem*(cel2D.rho/dt);
		cel2D.a0V=cel2D.vElem*(rhoC/dt);
		TLV[0]+=cel2D.a0V*cel2D.vC0;
		localV.mx[0][diag]+=cel2D.a0V;
		//cel2D.a0V/=localV.mx[0][diag];
	}
	//cel2D.uCEx=TLU[0];
	//cel2D.vCEx=TLV[0];
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			cel2D.uCEx-=cel2D.HcU[i]*(*vizinho[i]).uC;
			cel2D.vCEx-=cel2D.HcV[i]*(*vizinho[i]).vC;
		}
	}
	residuoU=cel2D.uCEx;
	residuoV=cel2D.vCEx;
	residuoU+=TLU[0];
	residuoV+=TLV[0];
	denoResU=localU.mx[0][diag]*cel2D.uC;
	denoResV=localV.mx[0][diag]*cel2D.vC;
	residuoU-=denoResU;
	residuoV-=denoResV;

	//cel2D.uCEx/=localU.mx[0][diag];
	//cel2D.vCEx/=localV.mx[0][diag];
	//cel2D.difuPres[0]=cel2D.vElem/localU.mx[0][diag];
	//cel2D.difuPres[1]=cel2D.vElem/localV.mx[0][diag];
	double acTemp=localU.mx[0][diag];
	localU.mx[0][diag]=acTemp/rlx;
	TLU[0]+=((1-rlx)/rlx)*acTemp*cel2D.uCI;
	cel2D.difuPresRC[0]=cel2D.difuPres[0]=cel2D.vElem/localU.mx[0][diag];
	acTemp=localV.mx[0][diag];
	localV.mx[0][diag]=acTemp/rlx;
	TLV[0]+=((1-rlx)/rlx)*acTemp*cel2D.vCI;

	cel2D.uCEx+=TLU[0];
	cel2D.vCEx+=TLV[0];

	cel2D.uCEx/=localU.mx[0][diag];
	cel2D.vCEx/=localV.mx[0][diag];

	cel2D.difuPresRC[1]=cel2D.difuPres[1]=cel2D.vElem/localV.mx[0][diag];
	cel2D.a0U=cel2D.a0U/localU.mx[0][diag];
	cel2D.a0V=cel2D.a0V/localV.mx[0][diag];
	if((*vg1dSP).metodoAcopPV==1 || (*vg1dSP).metodoAcopPV==4){
		double modHcU=0.;
		double modHcV=0.;
		for(int i=0;i<cel2D.nvert;i++){
			cel2D.HcU[i]/=localU.mx[0][diag];
			cel2D.HcV[i]/=localV.mx[0][diag];
			modHcU+=cel2D.HcU[i];
			modHcV+=cel2D.HcV[i];
		}
		cel2D.difuPres[0]/=(1.+modHcU);
		cel2D.difuPres[1]/=(1.+modHcV);
	}
	//cel2D.a0U=cel2D.a0U*cel2D.difuPresRC[0]/cel2D.vElem;
	//cel2D.a0V=cel2D.a0V*cel2D.difuPresRC[1]/cel2D.vElem;
}

void elem2d::GeraLocalU(){
	TLU[0]=0.;
	int diag=achaInd(cel2D.indEle);
	localU.mx[0][diag]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			double gradMedV [cel2D.dim];
			double gradMedTrans[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenU[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenU[j];
				gradMedV[j]=cel2D.fatG[i]*cel2D.gradGreenV[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenV[j];
			}
			gradMedTrans[0]=gradMed[0];
			gradMedTrans[1]=gradMedV[0];
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
			gradMedTrans[0]=gradMedTrans[0]+(((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i]-termoCorda)*cel2D.vecE[i][0];
			gradMedTrans[1]=gradMedTrans[1]+(((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][0];
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			viscHarm=1./viscHarm;
			double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			double escalGradAreaTrans=escalar(gradMedTrans,cel2D.sFace[i],cel2D.dim);
			TLU[0]+=viscHarm*(escalGradArea-termoCorda*cordaArea+escalGradAreaTrans);
			TLU[0]-=(fonteUHR[i]+cel2D.gradGreenPres[0]*cel2D.vElem);
			double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
			localU.mx[0][col]=-termMat+coefUHRV[i];
			localU.mx[0][diag]+=(termMat+coefUHRC[i]);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			double gradMedV [cel2D.dim];
			double gradMedTrans[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.gradGreenU[j];
				gradMedV[j]=cel2D.gradGreenV[j];
			}
			gradMedTrans[0]=gradMed[0];
			gradMedTrans[1]=gradMedV[0];
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double termoCordaV=escalar(gradMedV,cel2D.vecE[i],cel2D.dim);
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			viscHarm=1./viscHarm;
			double escalGradAreaTrans;
			double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			if(wall!=1){
				if(inl==1){
					gradMedTrans[0]=gradMedTrans[0]+((uRC[i]-cel2D.uC)/cel2D.modE[i]-termoCorda)*cel2D.vecE[i][0];
					gradMedTrans[1]=gradMedTrans[1]+((vRC[i]-cel2D.vC)/cel2D.modE[i]-termoCordaV)*cel2D.vecE[i][0];
					escalGradAreaTrans=escalar(gradMedTrans,cel2D.sFace[i],cel2D.dim);
					TLU[0]+=viscHarm*(escalGradArea-termoCorda*cordaArea+escalGradAreaTrans);
					TLU[0]-=(massF[i]*uRC[i]+cel2D.gradGreenPres[0]*cel2D.vElem);
					double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
					TLU[0]+=termMat*uRC[i];
					localU.mx[0][diag]+=(termMat);
				}
				else if(sim==1){
					double forcB=-(2.*viscHarm*cel2D.sFaceMod[i]/cel2D.modE[i])*cel2D.sFace[i][0]/cel2D.sFaceMod[i];
					TLU[0]-=(massF[i]*fonteUHR[i]+forcB*cel2D.vC*cel2D.sFace[i][1]/cel2D.sFaceMod[i]+
							cel2D.gradGreenPres[0]*cel2D.vElem);
					localU.mx[0][diag]+=(massF[i]*coefUHRC[i]);
					localU.mx[0][diag]-=forcB*cel2D.sFace[i][0]/cel2D.sFaceMod[i];
				}
				else if(out==1){
					TLU[0]-=(massF[i]*fonteUHR[i]+cel2D.gradGreenPres[0]*cel2D.vElem);
					localU.mx[0][diag]+=(massF[i]*coefUHRC[i]);
				}
			}
			else{
				cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]/cel2D.sFaceMod[i];
				double mod2=cel2D.sFaceMod[i]*cel2D.sFaceMod[i];
				TLU[0]+=viscHarm*cordaArea*(uRC[i]*(1-cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2)+
						(cel2D.vC-vRC[i])*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				TLU[0]-=cel2D.gradGreenPres[0]*cel2D.vElem;
				localU.mx[0][diag]+=viscHarm*cordaArea*(1-cel2D.sFace[i][0]*cel2D.sFace[i][0]/mod2);
			}
		}
	}
	if(perm==0 && trans==1){
		TLU[0]+=cel2D.vElem*cel2D.rho*cel2D.uC0/dt;
		localU.mx[0][diag]+=cel2D.vElem*(cel2D.rho/dt);
	}
	cel2D.difuPres[0]=cel2D.vElem/localU.mx[0][diag];
	double acTemp=localU.mx[0][diag];
	localU.mx[0][diag]=acTemp/(*vg1dSP).relaxVF;
	TLU[0]+=((1-(*vg1dSP).relaxVF)/(*vg1dSP).relaxVF)*acTemp*cel2D.uCI;
}

void elem2d::GeraLocalV(){
	TLV[0]=0.;
	int diag=achaInd(cel2D.indEle);
	localV.mx[0][diag]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			double gradMedU [cel2D.dim];
			double gradMedTrans[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenV[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenV[j];
				gradMedU[j]=cel2D.fatG[i]*cel2D.gradGreenU[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenU[j];
			}
			gradMedTrans[1]=gradMed[1];
			gradMedTrans[0]=gradMedU[1];
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
			gradMedTrans[0]=gradMedTrans[0]+(((*vizinho[i]).uC-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][1];
			gradMedTrans[1]=gradMedTrans[1]+(((*vizinho[i]).vC-cel2D.vC)/cel2D.modE[i]-termoCorda)*cel2D.vecE[i][1];
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			viscHarm=1./viscHarm;
			double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double escalGradAreaTrans=escalar(gradMedTrans,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			TLV[0]+=viscHarm*(escalGradArea-termoCorda*cordaArea+escalGradAreaTrans);
			TLV[0]-=(fonteVHR[i]+cel2D.gradGreenPres[1]*cel2D.vElem);
			double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
			localV.mx[0][col]=-termMat+coefVHRV[i];
			localV.mx[0][diag]+=(termMat+coefVHRC[i]);
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			double gradMedU [cel2D.dim];
			double gradMedTrans[cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				//gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenV[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenV[j];
				gradMed[j]=cel2D.gradGreenV[j];
				gradMedU[j]=cel2D.gradGreenU[j];
			}
			gradMedTrans[1]=gradMed[1];
			gradMedTrans[0]=gradMedU[1];
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double termoCordaU=escalar(gradMedU,cel2D.vecE[i],cel2D.dim);
			gradMedTrans[0]=gradMedTrans[0]+((uRC[i]-cel2D.uC)/cel2D.modE[i]-termoCordaU)*cel2D.vecE[i][1];
			gradMedTrans[1]=gradMedTrans[1]+((vRC[i]-cel2D.vC)/cel2D.modE[i]-termoCorda)*cel2D.vecE[i][1];
			double viscHarm;//=2*cel2D.visc*(*vizinho[i]).visc/(cel2D.visc+(*vizinho[i]).visc);
			viscHarm=cel2D.fatG[i]/cel2D.visc+(1-cel2D.fatG[i])/(*vizinho[i]).visc;
			viscHarm=1./viscHarm;
			double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double escalGradAreaTrans=escalar(gradMedTrans,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			if(wall!=1){
				if(inl==1){
					TLV[0]+=viscHarm*(escalGradArea-termoCorda*cordaArea+escalGradAreaTrans);
					TLV[0]-=(massF[i]*vRC[i]+cel2D.gradGreenPres[1]*cel2D.vElem);
					double termMat=(viscHarm*cordaArea/cel2D.modE[i]);
					TLV[0]+=termMat*vRC[i];
					localV.mx[0][diag]+=(termMat);
				}
				else if(sim==1){
					double forcB=-(2.*viscHarm*cel2D.sFaceMod[i]/cel2D.modE[i])*cel2D.sFace[i][1]/cel2D.sFaceMod[i];
					TLV[0]-=(massF[i]*fonteVHR[i]+forcB*cel2D.vC*cel2D.sFace[i][0]/cel2D.sFaceMod[i]+
							cel2D.gradGreenPres[1]*cel2D.vElem);
					localV.mx[0][diag]+=(massF[i]*coefVHRC[i]);
					localV.mx[0][diag]-=forcB*cel2D.sFace[i][1]/cel2D.sFaceMod[i];
				}
				else if(out==1){
					TLV[0]-=(massF[i]*fonteVHR[i]+cel2D.gradGreenPres[1]*cel2D.vElem);
					localV.mx[0][diag]+=(massF[i]*coefVHRC[i]);
				}
			}
			else{
				cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim)*cel2D.modE[i]/cel2D.sFaceMod[i];
				double mod2=cel2D.sFaceMod[i]*cel2D.sFaceMod[i];
				TLV[0]+=viscHarm*cordaArea*(vRC[i]*(1-cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2)+
						(cel2D.uC-uRC[i])*cel2D.sFace[i][0]*cel2D.sFace[i][1]/mod2);
				TLV[0]-=cel2D.gradGreenPres[0]*cel2D.vElem;
				localV.mx[0][diag]+=viscHarm*cordaArea*(1-cel2D.sFace[i][1]*cel2D.sFace[i][1]/mod2);
			}
		}
	}
	if(perm==0 && trans==1){
		TLV[0]+=cel2D.vElem*cel2D.rho*cel2D.vC0/dt;
		localV.mx[0][diag]+=cel2D.vElem*(cel2D.rho/dt);
	}
	cel2D.difuPres[1]=cel2D.vElem/localV.mx[0][diag];
	double acTemp=localV.mx[0][diag];
	localV.mx[0][diag]=acTemp/(*vg1dSP).relaxVF;
	TLV[0]+=((1-(*vg1dSP).relaxVF)/(*vg1dSP).relaxVF)*acTemp*cel2D.vCI;
}

void elem2d::GeraLocalPcor(){
	TLPCor[0]=0.;
	int diag=achaInd(cel2D.indEle);
	localPCor.mx[0][diag]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double rhoHarm;
			////Moukaled
			if((*vg1dSP).correcForcCorp==0){
				rhoHarm=cel2D.fatG[i]/cel2D.rho+(1-cel2D.fatG[i])/(*vizinho[i]).rho;
				rhoHarm=1./rhoHarm;
			}
			else{
			//// Eu-primeira ordem
				/*double deltaRho=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-
						(*vizinho[i]).rho*(1.-(*vizinho[i]).beta*((*vizinho[i]).tempC-(*vizinho[i]).tempRef));
				rhoHarm=cel2D.rho*(1.-cel2D.beta*(cel2D.tempC-cel2D.tempRef))-deltaRho*(1.-cel2D.fatG[i]);*/
				rhoHarm=cel2D.fatG[i]*cel2D.rho*(1.-0*cel2D.beta*(cel2D.tempC-cel2D.tempRef))+
							(1.-cel2D.fatG[i])*(*vizinho[i]).rho*(1.-0*(*vizinho[i]).beta*((*vizinho[i]).tempC-(*vizinho[i]).tempRef));
			}
			///////eu-segunda ordem
			//rhoHarm=cel2D.rho*(1.-cel2D.beta*(tempF[i]-cel2D.tempRef));
			double gradMed [cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenPresCor[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenPresCor[j];
			}
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double escalGradArea=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
			TLPCor[0]+=rhoHarm*(escalGradArea-termoCorda*cordaArea);
			double termMat=rhoHarm*(cordaArea/cel2D.modE[i]);
			localPCor.mx[0][col]=-termMat;
			localPCor.mx[0][diag]+=(termMat);
			cel2D.HcP[i]=localPCor.mx[0][col];
			//TLPCor[0]-=massTot;
		}
		else{
			int inl=0;
			int out=0;
			int wall=0;
			int sim=0;
			int kcc=0;
			tipoCC(i, inl, out, wall, sim, kcc);
			//int col=achaInd(cel2D.indFace[i]);
			/*if(inl==1 || sim==1 || wall==1){
				double rhoHarm;
				rhoHarm=cel2D.rho;
				double difGradMed [cel2D.dim];
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					difGradMed[j]=gradMed[j]=cel2D.gradGreenPresCor[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				/////////////////////////////////////////////////////////////////////////////////////////
				//double deri=(pb-cel2D.presCcor)/cel2D.modE[i];
				//(deri)*cel2D.vecE[i][j]
				for(int j=0; j<cel2D.dim;j++){
					difGradMed[j]=difGradMed[j]-termoCorda*cel2D.vecE[i][j];
				}
				double rhoHarm;
				rhoHarm=cel2D.rho;
				double difU=cel2D.difuPres[0];
				double difV=cel2D.difuPres[1];
				double masTempFonte=-(cel2D.sFace[i][0]*difU*difGradMed[0]+cel2D.sFace[i][1]*difV*difGradMed[1]);
				double coef0=-(cel2D.sFace[i][0]*difU*cel2D.vecE[i][0]+cel2D.sFace[i][1]*difU*cel2D.vecE[i][1])/cel2D.modE[i];
				masTempFonte=-masTempFonte/coef0;
				////////////////////////////////////////////////////////////////////////////////////////
				double escalGradArea=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
				TLPCor[0]+=rhoHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=rhoHarm*(cordaArea/cel2D.modE[i]);
				//localPCor.mx[0][col]=-termMat;
				//localPCor.mx[0][diag]+=(termMat);
				TLPCor[0]+=termMat*masTempFonte;
				//TLPCor[0]-=massTot;
			}
	  else*/if(out==1){
				double rhoHarm;
				//rhoHarm=cel2D.rho;
				rhoHarm=cel2D.rho*(1.-0*cel2D.beta*(cel2D.tempC-cel2D.tempRef));
				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenPresCor[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.vecSDif[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.vecSDif[i],cel2D.dim);
				//TLPCor[0]+=rhoHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=rhoHarm*(cordaArea/cel2D.modE[i]);
				//localPCor.mx[0][col]=-termMat;
				localPCor.mx[0][diag]+=(termMat);
				//TLPCor[0]-=massTot;
			}
		}
	}
	TLPCor[0]-=massTot;
	cel2D.presCEx=TLPCor[0];
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			cel2D.presCEx-=cel2D.HcP[i]*(*vizinho[i]).presCcor;
		}
	}
	cel2D.presCEx/=localPCor.mx[0][diag];
}


void elem2d::GeraLocalT(double rlx){
	TLT[0]=0.;
	int diag=achaInd(cel2D.indEle);
	localT.mx[0][diag]=0.;
	cel2D.tCEx=0.;
	for(int i=0;i<cel2D.nvert;i++){
		cel2D.HcT[i]=0.;
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenTemp[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenTemp[j];
			}
			double condHarm;//=2*cel2D.cond*(*vizinho[i]).cond/(cel2D.cond+(*vizinho[i]).cond);
			condHarm=cel2D.fatG[i]/cel2D.cond+(1-cel2D.fatG[i])/(*vizinho[i]).cond;
			condHarm=1./condHarm;
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			TLT[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
			TLT[0]-=(cel2D.cp*fonteTHR[i]);
			double termMat=(condHarm*cordaArea/cel2D.modE[i]);
			localT.mx[0][col]=-termMat+cel2D.cp*coefTHRV[i];
			localT.mx[0][diag]+=(termMat+cel2D.cp*coefTHRC[i]);
			cel2D.HcT[i]=localT.mx[0][col];
		}
		else{
			int diri=0;
			int rich=0;
			int vn=0;
			int kcc=0;
			int acoplado;
			tipoCCTemp(i, diri, vn, rich,acoplado,kcc);
		    int ind;
		    double raz;
			if(diri==1){

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double condHarm=cel2D.cond;
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				TLT[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=(condHarm*cordaArea/cel2D.modE[i]);
				TLT[0]+=termMat*tempF[i];
				TLT[0]-=(cel2D.cp*massF[i]*tempF[i]);
				localT.mx[0][diag]+=termMat;
			}
			else if(rich==1){

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenTemp[j];
				}
				double condHarm=cel2D.cond;
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termMat=(condHarm*cordaArea/cel2D.modE[i]);

				TLT[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				TLT[0]+=termMat*fonteTHR[i];
				TLT[0]-=(cel2D.cp*massF[i]*fonteTHR[i]);
				localT.mx[0][diag]+=(cel2D.cp*massF[i]*coefTHRC[i]);
				localT.mx[0][diag]-=termMat*coefTHRC[i];
				localT.mx[0][diag]+=termMat;
			}
			else if(vn==1 || acoplado==1){
				TLT[0]+=(ccTVN[i]-DCCN*cel2D.tempC)*cel2D.sFaceMod[i];
				TLT[0]-=1*(cel2D.cp*massF[i]*fonteTHR[i]);
				localT.mx[0][diag]+=(-DCCN*cel2D.sFaceMod[i]+1*cel2D.cp*massF[i]*coefTHRC[i]);
			}
		}
	}
	double rhoC=cel2D.rho*(1.-0*cel2D.beta*(cel2D.tempC-cel2D.tempRef));
	////dissipacao viscosa/////////////////////////////////////////////////////
	double term1=2*(cel2D.gradGreenU[0]*cel2D.gradGreenU[0]+cel2D.gradGreenV[1]*cel2D.gradGreenV[1]);
	double term2=cel2D.gradGreenU[1]+cel2D.gradGreenV[0];
	term2*=term2;
	double dissipa=cel2D.visc*(term1+term2)*cel2D.vElem;
	TLT[0]+=dissipa;
	//////////////////////////////////////////////////////////////////////////
	if(perm==0 && trans==1){
		TLT[0]+=cel2D.vElem*rhoC*cel2D.cp*cel2D.tempC0/dt;
		localT.mx[0][diag]+=cel2D.vElem*(rhoC*cel2D.cp/dt);
	}

	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			cel2D.tCEx-=cel2D.HcT[i]*(*vizinho[i]).tempC;
		}
	}
	residuoT=cel2D.tCEx;
	residuoT+=TLT[0];
	denoResT=localT.mx[0][diag]*cel2D.tempC;
	residuoT-=denoResT;

	double acTemp=localT.mx[0][diag];
	localT.mx[0][diag]=acTemp/rlx;
	TLT[0]+=((1-rlx)/rlx)*acTemp*cel2D.tempCI;

	cel2D.tCEx+=TLT[0];

	cel2D.tCEx/=localT.mx[0][diag];

}

void elem2d::explicitHol(){
	int mono=1;
	double hol=cel2D.holC0;

	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			if(cel2D.holC0!=(*vizinho[i]).holC0)mono=0;
			else if(fabsl(cel2D.holC0-(*vizinho[i]).holC0)<1e-15 && cel2D.holC0>1e-15 && cel2D.holC0<(1.-1e-15))mono=0;
		}
		else if(fabsl(cel2D.holC0-holF[i])<1e-15)mono=0;
		else if(fabsl(cel2D.holC0-holF[i])<1e-15 && cel2D.holC0>1e-15 && cel2D.holC0<(1.-1e-15))mono=0;
	}
	if(mono==1)cel2D.holC=hol;
	else{
		double fluxTot=0.;
		for(int i=0;i<cel2D.nvert;i++){
			fluxTot+=(cel2D.sFace[i][0]*uRC[i]+cel2D.sFace[i][1]*vRC[i])*holF[i];
		}
		double holVol0=cel2D.vElem*cel2D.holC0/dt;
		hol=cel2D.holC0-(fluxTot)*dt/cel2D.vElem;

		if(hol>1.-1e-5){
			if(hol>1.+1e-5){
				//if(holF[0]>1.-1e-5 && holF[1]>1.-1e-5 && holF[2]>1.-1e-5){
					//int erro=0;
					//erro++;
				//}
				double dtTemp;
				dtTemp=-(1.-cel2D.holC0)*cel2D.vElem/fluxTot;
				if(dtTemp>1e-15){
					(*vg1dSP).reiniciaVF=1;
					dt=dtTemp;
				}
				else cel2D.holC=1.;
			}
			else cel2D.holC=1.;
		}
		else if(hol<1e-5){
			if(hol<0.-1e-5){
				double dtTemp;
				dtTemp=cel2D.holC0*cel2D.vElem/fluxTot;
				if(dtTemp>1e-15){
					(*vg1dSP).reiniciaVF=1;
					dt=dtTemp;
				}
				else cel2D.holC=0.;
			}
			else cel2D.holC=0.;
		}
		else cel2D.holC=hol;
	}
}

