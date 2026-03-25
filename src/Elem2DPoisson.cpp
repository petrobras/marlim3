/*
 * elem2dPoisson.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Elem2DPoisson.h"



elem2dPoisson::elem2dPoisson(varGlob1D* Vvg1dSP,double** xcoor, int** noEle,int* tipo,double* atributo,int nVert, int nele, int nno, int vperm,
		int vtrans,int i,double vtemp,double vfluxCal,double vcond,double vcp,double vrho,double vdt) :
				TL(1), local(1,nVert+1) {
	vg1dSP=Vvg1dSP;
	vizinho=0;
	kvizinho=0;
	nvizinho=0;
	cel2D.tempC=vtemp;
	cel2D.tempC0=vtemp;
	cel2D.tempCI=vtemp;
	deriFonteT=0.;
	FonteT=0.;
	fluxCal=vfluxCal;
	cel2D.cond=vcond;
	cel2D.rho=vrho;
	cel2D.cp=vcp;
	dt=vdt;
	perm=vperm;
	trans=vtrans;
	ccTD=0;
	ccTVN=0;
	ccHR=0;
	ccTambR=0;
	CC=detCCPoisson();
	if(nele>0){
		cel2D.noFace=0;
    	cel2D.dim=2;
    	cel2D.nvert=3;
    	cel2D.indEle=i;
    	cel2D.nele=nele;
    	tempF=new double [cel2D.nvert];
    	gradTface=new double* [cel2D.nvert];
    	ccTD=new double [cel2D.nvert];
    	ccTVN=new double [cel2D.nvert];
    	ccHR=new double [cel2D.nvert];
    	ccTambR=new double [cel2D.nvert];
    	for(int j=0; j<cel2D.nvert;j++) gradTface[j]=new double [cel2D.dim];
    	cel2D.gradGreenT=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenT[j]=0.;
    	cel2D.gradGreenTI=new double [cel2D.dim];
    	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenTI[j]=0.;
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

    	vizinho=new elementoPoisson* [cel2D.nvert];
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
		cel2D.gradGreenT=0;
		cel2D.gradGreenTI=0;
		tempF=0;
		gradTface=0;
    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;
	}
}
elem2dPoisson::elem2dPoisson(const elem2dPoisson& velem) :
						TL(1), local(1,velem.cel2D.nvert+1) {
	vg1dSP=velem.vg1dSP;
	cel2D.nvert=velem.cel2D.nvert;
	cel2D.dim=velem.cel2D.dim;
	cel2D.indEle=velem.cel2D.indEle;
	cel2D.nele=velem.cel2D.nele;
	nvizinho=velem.nvizinho;
	cel2D.tempC=velem.cel2D.tempC;
	cel2D.tempC0=velem.cel2D.tempC0;
	cel2D.tempCI=velem.cel2D.tempCI;
	deriFonteT=velem.deriFonteT;
	FonteT=velem.FonteT;
	fluxCal=velem.fluxCal;
	cel2D.cond=velem.cel2D.cond;
	cel2D.rho=velem.cel2D.rho;
	cel2D.cp=velem.cel2D.cp;
	dt=velem.dt;
	perm=velem.perm;
	trans=velem.trans;
	CC=velem.CC;
	//ccTD=velem.ccTD;
	//ccTVN=velem.ccTVN;
	//ccHR=velem.ccHR;
	//ccTambR=velem.ccTambR;
	if(cel2D.nvert>0){
    	vizinho=new elementoPoisson* [cel2D.nvert];
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
    	tempF=new double [cel2D.nvert];
    	gradTface=new double* [cel2D.nvert];
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
    	for(int j=0; j<cel2D.nvert;j++) gradTface[j]=new double [cel2D.dim];
    	cel2D.gradGreenT=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenT[j]=velem.cel2D.gradGreenT[j];
    	cel2D.gradGreenTI=new double [cel2D.dim];
    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenTI[j]=velem.cel2D.gradGreenTI[j];
    	for(int i=0; i<cel2D.nvert;i++){
        	tempF[i]=velem.tempF[i];
        	ccTD[i]=velem.ccTD[i];
        	ccTVN[i]=velem.ccTVN[i];
        	ccHR[i]=velem.ccHR[i];
        	ccTambR[i]=velem.ccTambR[i];
        	for(int j=0;j<cel2D.dim;j++) gradTface[i][j]=velem.gradTface[i][j];
    	}
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
		//////////////////////////////////////////////////////////////////////////////////////
        cel2D.vecE=0;
        cel2D.modE=0;
        cel2D.vecT=0;
        cel2D.modT=0;
        cel2D.fatG=0;
        cel2D.angES=0;
        cel2D.fInter=0;
        cel2D.fIfC=0;
        cel2D.gradGreenT=0;
        cel2D.gradGreenTI=0;
        tempF=0;
        gradTface=0;
    	ccTD=0;
    	ccTVN=0;
    	ccHR=0;
    	ccTambR=0;

    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;
	}
}

elem2dPoisson& elem2dPoisson::operator =(const elem2dPoisson& velem) {
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
			delete[] ccTD;
			delete[] ccTVN;
			delete[] ccHR;
			delete[] ccTambR;
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
				delete[] gradTface[i];
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
			delete[] cel2D.gradGreenT;
			delete[] cel2D.gradGreenTI;
			//////////////////////////////////////////////////////////////////////////////////
			delete[] tempF;
			delete[] gradTface;

			delete[] coefTHRC;
			delete[] coefTHRV;
			delete[] fonteTHR;
		}
		/*if(nvizinho>0){
			delete[] vizinho;
			delete[] kvizinho;
		}*/
		vg1dSP=velem.vg1dSP;
		TL=velem.TL;
		local=velem.local;
		cel2D.nvert=velem.cel2D.nvert;
		cel2D.dim=velem.cel2D.dim;
		cel2D.indEle=velem.cel2D.indEle;
		cel2D.nele=velem.cel2D.nele;
		nvizinho=velem.nvizinho;
		cel2D.tempC=velem.cel2D.tempC;
		cel2D.tempC0=velem.cel2D.tempC0;
		cel2D.tempCI=velem.cel2D.tempCI;
		deriFonteT=velem.deriFonteT;
		FonteT=velem.FonteT;
		fluxCal=velem.fluxCal;
		cel2D.cond=velem.cel2D.cond;
		cel2D.rho=velem.cel2D.rho;
		cel2D.cp=velem.cel2D.cp;
		dt=velem.dt;
		perm=velem.perm;
		trans=velem.trans;
		ccTD=velem.ccTD;
		ccTVN=velem.ccTVN;
		ccHR=velem.ccHR;
		ccTambR=velem.ccTambR;
		CC=velem.CC;
		if(cel2D.nvert>0){
	    	vizinho=new elementoPoisson* [cel2D.nvert];
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
	    	tempF=new double [cel2D.nvert];
	    	gradTface=new double* [cel2D.nvert];
	    	ccTD=new double [cel2D.nvert];
	    	ccTVN=new double [cel2D.nvert];
	    	ccHR=new double [cel2D.nvert];
	    	ccTambR=new double [cel2D.nvert];
	    	for(int j=0; j<cel2D.nvert;j++) gradTface[j]=new double [cel2D.dim];
	    	cel2D.gradGreenT=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenT[j]=velem.cel2D.gradGreenT[j];
	    	cel2D.gradGreenTI=new double [cel2D.dim];
	    	for(int j=0;j<cel2D.dim;j++) cel2D.gradGreenTI[j]=velem.cel2D.gradGreenTI[j];
	    	for(int i=0; i<cel2D.nvert;i++){
	        	tempF[i]=velem.tempF[i];
	        	ccTD[i]=velem.ccTD[i];
	        	ccTVN[i]=velem.ccTVN[i];
	        	ccHR[i]=velem.ccHR[i];
	        	ccTambR[i]=velem.ccTambR[i];
	        	for(int j=0;j<cel2D.dim;j++) gradTface[i][j]=velem.gradTface[i][j];
	    	}
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
			//////////////////////////////////////////////////////////////////////////////////////
	        cel2D.vecE=0;
	        cel2D.modE=0;
	        cel2D.vecT=0;
	        cel2D.modT=0;
	        cel2D.fatG=0;
	        cel2D.angES=0;
	        cel2D.fInter=0;
	        cel2D.fIfC=0;
	        cel2D.gradGreenT=0;
	        cel2D.gradGreenTI=0;
	        tempF=0;
	        gradTface=0;
	    	ccTD=0;
	    	ccTVN=0;
	    	ccHR=0;
	    	ccTambR=0;

	    	coefTHRC=0;
	    	coefTHRV=0;
	    	fonteTHR=0;
		}
	}
	return *this;
}

void elem2dPoisson::buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle){
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

void elem2dPoisson::menorIndViz(){
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

double elem2dPoisson::escalar(double* v1, double* v2, int vDim){
	double tot=0.;
	for(int i=0;i<vDim;i++)tot+=v1[i]*v2[i];
	return tot;
}

void elem2dPoisson::indraz(int& ind, double& raz,
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

void elem2dPoisson::faceDetalhes(){

	/*
	double** vecE;//vetor unitario na orientacao entre o centroide de do elementoPoisson e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elementoPoisson e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;//distancia entre o centroide da face e a intersecao da face
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementoPoissons
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementoPoissons e a face do elementoPoisson
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

void elem2dPoisson::tipoCC(int i,int& diri, int& vn, int& rich, int& acoplado, int& kcc){
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

void elem2dPoisson::atualizaCC(int i){
	int diri=0;
	int rich=0;
	int vn=0;
	int kcc=0;
	int acoplado=0;
	tipoCC(i, diri, vn, rich,acoplado,kcc);
    int ind=0;
    double raz;
	if(diri==1){

		indraz(ind, raz, (*vg1dSP).lixo5, CC.ccDir[kcc].nserie, CC.ccDir[kcc].tempo);
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

		indraz(ind, raz, (*vg1dSP).lixo5, CC.ccVN[kcc].nserie, CC.ccVN[kcc].tempo);
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

		indraz(ind, raz, (*vg1dSP).lixo5, CC.ccRic[kcc].nserie, CC.ccRic[kcc].tempo);
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

void elem2dPoisson::calcGradGreen(){
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			double tinter=cel2D.fatG[i]*cel2D.tempC+(1.-cel2D.fatG[i])*(*vizinho[i]).tempC;
			tempF[i]=0.;
				//esta abordagem não parece corrigir bem a distorção pelo fator da interpolação nção estar no centro da superfície:

			//for(int j=0; j<cel2D.dim;j++)
			//tempF[i]+=cel2D.fatG[i]*cel2D.gradGreenT[j]*(cel2D.centroideFace[i][j]-cel2D.centroideElem[j])+
			//(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenT[j]*(cel2D.centroideFace[i][j]-(*vizinho[i]).centroideElem[j]);

				//esta abordagem, aparentemente. levou a resultados melhores:

			//for(int j=0; j<cel2D.dim;j++)
			//tempF[i]+=cel2D.fatG[i]*cel2D.gradGreenT[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j])+
				//	  (1-cel2D.fatG[i])*(*vizinho[i]).gradGreenT[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			//tempF[i]+=tinter;


			double gradMed [cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenTI[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenTI[j];
			}
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double deri=-(cel2D.tempC-(*vizinho[i]).tempC)/cel2D.modE[i];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel2D.vecE[i][j];
			}
			double corDistorc=1.;
			if(fabsl(cel2D.angES[i])<0.9)corDistorc=0.;
			for(int j=0; j<cel2D.dim;j++)
			tempF[i]+=corDistorc*gradMed[j]*(cel2D.centroideFace[i][j]-cel2D.fInter[i][j]);
			tempF[i]+=tinter;
		}
		else{
			int diri=0;
			int rich=0;
			int vn=0;
			int kcc=0;
			int acoplado=0;
			tipoCC(i, diri, vn, rich,acoplado,kcc);
			if(diri==1){
				tempF[i]=ccTD[i];
			}
			else if(vn==1){
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
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				coefTHRC[i]=1.;
				fonteTHR[i]=termoCorda*cel2D.modE[i];
				tempF[i]=cel2D.tempC+fonteTHR[i];
			}
			else if(rich==1 || acoplado==1){
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

				coefTHRC[i]=(cel2D.cond*cordaArea/cel2D.modE[i])/denom;
				fonteTHR[i]=(ccHR[i]*cel2D.sFaceMod[i]*ccTambR[i]-cel2D.cond*correrT)/denom;
				tempF[i]=num/denom;
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
	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenT[j]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenT[j]+=tempF[i]*cel2D.sFace[i][j];
	}
	for(int j=0; j<cel2D.dim;j++)cel2D.gradGreenT[j]/=cel2D.vElem;
}

int elem2dPoisson::achaInd(int i){
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

void elem2dPoisson::GeraLocal(){
	TL[0]=0.;
	int diag=achaInd(cel2D.indEle);
	local.mx[0][diag]=0.;
	for(int i=0;i<cel2D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel2D.indFace[i]);
			double gradMed [cel2D.dim];
			for(int j=0; j<cel2D.dim;j++){
				gradMed[j]=cel2D.fatG[i]*cel2D.gradGreenT[j]+(1-cel2D.fatG[i])*(*vizinho[i]).gradGreenT[j];
			}
			double condHarm;//=2*cel2D.cond*(*vizinho[i]).cond/(cel2D.cond+(*vizinho[i]).cond);
			condHarm=cel2D.fatG[i]/cel2D.cond+(1-cel2D.fatG[i])/(*vizinho[i]).cond;
			condHarm=1./condHarm;
			double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
			double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
			double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
			TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
			double termMat=(condHarm*cordaArea/cel2D.modE[i]);
			local.mx[0][col]=-termMat;
			local.mx[0][diag]+=termMat;
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
			if(diri==1){

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
			else if(rich==1 || acoplado==1){

				double gradMed [cel2D.dim];
				for(int j=0; j<cel2D.dim;j++){
					gradMed[j]=cel2D.gradGreenT[j];
				}
				double condHarm=cel2D.cond;
				double termoCorda=escalar(gradMed,cel2D.vecE[i],cel2D.dim);
				double escalGradArea=escalar(gradMed,cel2D.sFace[i],cel2D.dim);
				double cordaArea=escalar(cel2D.vecE[i],cel2D.sFace[i],cel2D.dim);
				double termMat=(condHarm*cordaArea/cel2D.modE[i]);

				TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				TL[0]+=termMat*fonteTHR[i];
				local.mx[0][diag]+=termMat*(1.-coefTHRC[i]);
			}
			else if(vn==1){
				TL[0]+=ccTVN[i]*cel2D.sFaceMod[i];
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
	if(perm==0 && trans==1){
		TL[0]+=cel2D.vElem*cel2D.rho*cel2D.cp*cel2D.tempC0/dt+FonteT*cel2D.vElem;
		local.mx[0][diag]+=cel2D.vElem*(cel2D.rho*cel2D.cp/dt-deriFonteT);
	}
}

