/*
 * elem3dPoisson.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Elem3DPoisson.h"



elem3dPoisson::elem3dPoisson(varGlob1D* DGP3DV,
		eleOBJ velementoUNV, double** xcoor, int** noEle,int* tipo,double* atributo,int nVert, int nele, int nno,
		int i,double vtemp,double vfluxCal,double vcond,double vcp,double vrho,double vdt) :
				TL(1), local(1,nVert+1) {
	DGP3D=DGP3DV;
	vizinho=0;
	kvizinho=0;
	nvizinho=0;
	cel3D.tempC=vtemp;
	cel3D.tempC0=vtemp;
	cel3D.tempCI=vtemp;
	deriFonteT=0.;
	FonteT=0.;
	fluxCal=vfluxCal;
	cel3D.cond=vcond;
	cel3D.rho=vrho;
	cel3D.cp=vcp;
	dt=vdt;
	//perm=vperm;
	//trans=vtrans;
	ccTD=0;
	ccTVN=0;
	ccHR=0;
	ccTambR=0;
	CC=0;//detCCPoisson3D();
	if(nele>0){
		eleUNV=velementoUNV;
		cel3D.noFace=0;
    	cel3D.dim=3;
    	cel3D.nvert=4;
    	cel3D.indEle=i;
    	cel3D.nele=nele;
    	v1=new double [cel3D.dim];
    	v2=new double [cel3D.dim];
    	v3=new double [cel3D.dim];
    	tempF=new double [cel3D.nvert];
    	gradTface=new double* [cel3D.nvert];
    	ccTD=new double [cel3D.nvert];
    	ccTVN=new double [cel3D.nvert];
    	ccHR=new double [cel3D.nvert];
    	ccTambR=new double [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) gradTface[j]=new double [cel3D.dim];
    	cel3D.gradGreenT=new double [cel3D.dim];
    	for(int j=0; j<cel3D.dim;j++)cel3D.gradGreenT[j]=0.;
    	cel3D.gradGreenTI=new double [cel3D.dim];
    	for(int j=0; j<cel3D.dim;j++)cel3D.gradGreenTI[j]=0.;
    	cel3D.gradGreenT0=new double [cel3D.dim];
    	for(int j=0; j<cel3D.dim;j++)cel3D.gradGreenT0[j]=0.;
    	coefTHRC=new double [cel3D.nvert];
    	coefTHRV=new double [cel3D.nvert];
    	fonteTHR=new double [cel3D.nvert];
    	cel3D.noElem=new int [cel3D.nvert];
    	cel3D.indFace=new int [cel3D.nvert];
    	cel3D.ccFace=new string [cel3D.nvert];
    	cel3D.orientFace=new double [cel3D.nvert];
    	cel3D.ownFace=new double [cel3D.nvert];
    	cel3D.centroElem=new double [cel3D.dim];
    	cel3D.centroideElem=new double [cel3D.dim];
    	cel3D.centroideFace=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.centroideFace[j]=new double [cel3D.dim];
    	cel3D.coordVert=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.coordVert[j]=new double [cel3D.dim];
    	cel3D.dCF=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.dCF[j]=new double [cel3D.dim];
    	cel3D.dCFMod=new double [cel3D.nvert];
    	cel3D.sFace=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.sFace[j]=new double [cel3D.dim];
    	cel3D.sFaceMod=new double [cel3D.nvert];
    	///////////////////////////////////////////////////////////////////////////////////////////
        cel3D.vecE=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.vecE[j]=new double [cel3D.dim];
        cel3D.modE=new double [cel3D.nvert];
        cel3D.vecT=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.vecT[j]=new double [cel3D.dim];
        cel3D.modT=new double [cel3D.nvert];
        cel3D.fatG=new double [cel3D.nvert];
        cel3D.angES=new double [cel3D.nvert];
        cel3D.fInter=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.fInter[j]=new double [cel3D.dim];
        cel3D.fIfC=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.fIfC[j]=new double [cel3D.dim];

    	vizinho=new elementoPoisson3D* [cel3D.nvert];
    	kvizinho=new int [cel3D.nvert];
		for(int k=0; k<cel3D.nvert;k++){
			kvizinho[k]=-1;
			vizinho[k]=0;
		}


    	buscaVizinho(noEle,cel3D.indFace,i,cel3D.nvert, nele);
    	for(int j=0; j<cel3D.nvert;j++){
    		if(i>cel3D.indFace[j] && cel3D.indFace[j]>=0)cel3D.ownFace[j]=-1.;
    		else cel3D.ownFace[j]=1.;
    	}

    	menorIndViz();

    	for(int j=0; j<cel3D.nvert;j++){
    		cel3D.noElem[j]=eleUNV.noGlobal[j];
    		int indCC=eleUNV.faceCCfaceReal[j];
    		if(indCC>=0)cel3D.ccFace[j]=eleUNV.objFace[indCC].condCont;
    		else cel3D.ccFace[j]="interno";
    		for(int k=0; k<cel3D.dim; k++){
    			cel3D.coordVert[j][k]=eleUNV.objNo[j].x[k];
    		}
    	}
    	for(int j=0; j<cel3D.dim;j++){
    		cel3D.centroElem[j]=0.;
    		for(int k=0; k<cel3D.nvert; k++){
    			cel3D.centroElem[j]+=cel3D.coordVert[k][j];
    		}
    		cel3D.centroElem[j]/=cel3D.nvert;
    	}
		for(int j=0; j<cel3D.dim;j++)cel3D.centroideElem[j]=cel3D.centroElem[j];

    	for(int j=0; j<cel3D.dim;j++){
    		v1[j]=cel3D.coordVert[1][j]-cel3D.coordVert[0][j];
    		v2[j]=cel3D.coordVert[2][j]-cel3D.coordVert[0][j];
    		v3[j]=cel3D.coordVert[3][j]-cel3D.coordVert[0][j];
    	}
    	double prodVec[3];
    	prodVec[0]=v1[1]*v2[2]-v1[2]*v2[1];
    	prodVec[1]=v1[2]*v2[0]-v1[0]*v2[2];
    	prodVec[2]=v1[0]*v2[1]-v1[1]*v2[0];
    	cel3D.vElem=0.;
    	for(int j=0; j<cel3D.dim;j++)cel3D.vElem+=v3[j]*prodVec[j];
    	cel3D.vElem/=6.;
    	cel3D.vElem=fabsl(cel3D.vElem);

       	for(int k=0; k<cel3D.nvert; k++){
        		double vetor1[3];
        		double vetor2[3];
        		double val;
        		if(xcoor[eleUNV.indFaceNo[k][0]][2]==xcoor[eleUNV.indFaceNo[k][1]][2] &&
        				xcoor[eleUNV.indFaceNo[k][0]][2]==xcoor[eleUNV.indFaceNo[k][2]][2]){
        			int para;
        			para=0;
        		}
        		for(int j=0; j<cel3D.dim;j++)cel3D.centroideFace[k][j]=0.;
        		for(int j=0; j<cel3D.dim;j++){
        			for(int kV=0; kV<cel3D.nvert-1; kV++){
        				int indNo=eleUNV.indFaceNo[k][kV];
        				cel3D.centroideFace[k][j]+=xcoor[indNo][j];//cel3D.centroideFace[k][j]+=eleUNV.objFace[k].objNo[kV].x[j];
        			}
        			cel3D.centroideFace[k][j]/=(cel3D.nvert-1);
        		}
        		for(int j=0; j<cel3D.dim;j++){
        			int indNo0=eleUNV.indFaceNo[k][0];
        			int indNo1=eleUNV.indFaceNo[k][1];
        			int indNo2=eleUNV.indFaceNo[k][2];
        			vetor1[j]=xcoor[indNo1][j]-xcoor[indNo0][j];//eleUNV.objFace[k].objNo[1].x[j]-eleUNV.objFace[k].objNo[0].x[j];
        			vetor2[j]=xcoor[indNo2][j]-xcoor[indNo0][j];
        		}
            	prodVec[0]=vetor1[1]*vetor2[2]-vetor1[2]*vetor2[1];
            	prodVec[1]=vetor1[2]*vetor2[0]-vetor1[0]*vetor2[2];
            	prodVec[2]=vetor1[0]*vetor2[1]-vetor1[1]*vetor2[0];
            	val=prodVec[0]*(cel3D.centroideFace[k][0]-cel3D.centroElem[0])+
            		prodVec[1]*(cel3D.centroideFace[k][1]-cel3D.centroElem[1])+
    				prodVec[2]*(cel3D.centroideFace[k][2]-cel3D.centroElem[2]);
    			if(val>0)cel3D.orientFace[k]=1.;
    			else cel3D.orientFace[k]=-1.;
    			for(int j=0; j<cel3D.dim;j++)cel3D.sFace[k][j]=0.5*cel3D.orientFace[k]*prodVec[j];
        }
    	for(int k=0; k<cel3D.nvert; k++){
    		cel3D.sFaceMod[k]=0.;
    		for(int j=0; j<cel3D.dim;j++)cel3D.sFaceMod[k]+=cel3D.sFace[k][j]*cel3D.sFace[k][j];
    		cel3D.sFaceMod[k]=sqrtl(cel3D.sFaceMod[k]);
    	}
    	for(int k=0; k<cel3D.nvert; k++){
    		for(int j=0; j<cel3D.dim;j++)cel3D.dCF[k][j]=cel3D.centroideFace[k][j]-cel3D.centroideElem[j];
    		cel3D.dCFMod[k]=0.;
    		for(int j=0; j<cel3D.dim;j++)cel3D.dCFMod[k]+=cel3D.dCF[k][j]*cel3D.dCF[k][j];
    		cel3D.dCFMod[k]=sqrtl(cel3D.dCFMod[k]);
    	}
	}
	else{
		cel3D.nvert=0;
		cel3D.dim=0;
		cel3D.indEle=0;
		cel3D.nele=0;
		cel3D.noElem=0;
		cel3D.indFace=0;
		cel3D.ccFace=0;
		cel3D.orientFace=0;
		cel3D.centroElem=0;
		cel3D.centroideElem=0;
		cel3D.centroideFace=0;
		cel3D.coordVert=0;
		cel3D.dCF=0;
		cel3D.dCFMod=0;
		cel3D.vElem=0;
		cel3D.noFace=0;
		cel3D.sFace=0;
		cel3D.sFaceMod=0;
		cel3D.ownFace=0;
		cel3D.gradGreenT=0;
		cel3D.gradGreenTI=0;
		cel3D.gradGreenT0=0;
		tempF=0;
		v1=0;
		v2=0;
		v3=0;
		gradTface=0;
    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;
    	eleUNV=eleOBJ();
	}
}
elem3dPoisson::elem3dPoisson(const elem3dPoisson& velem) :
						TL(1), local(1,velem.cel3D.nvert+1) {
	DGP3D=velem.DGP3D;
	cel3D.nvert=velem.cel3D.nvert;
	cel3D.dim=velem.cel3D.dim;
	cel3D.indEle=velem.cel3D.indEle;
	cel3D.nele=velem.cel3D.nele;
	nvizinho=velem.nvizinho;
	cel3D.tempC=velem.cel3D.tempC;
	cel3D.tempC0=velem.cel3D.tempC0;
	cel3D.tempCI=velem.cel3D.tempCI;
	deriFonteT=velem.deriFonteT;
	FonteT=velem.FonteT;
	fluxCal=velem.fluxCal;
	cel3D.cond=velem.cel3D.cond;
	cel3D.rho=velem.cel3D.rho;
	cel3D.cp=velem.cel3D.cp;
	dt=velem.dt;
	//perm=velem.perm;
	//trans=velem.trans;
	CC=velem.CC;
	//ccTD=velem.ccTD;
	//ccTVN=velem.ccTVN;
	//ccHR=velem.ccHR;
	//ccTambR=velem.ccTambR;
	if(cel3D.nvert>0){
    	vizinho=new elementoPoisson3D* [cel3D.nvert];
    	kvizinho=new int [cel3D.nvert];
		for(int k=0; k<cel3D.nvert;k++){
			kvizinho[k]=velem.kvizinho[k];
			vizinho[k]=velem.vizinho[k];
		}
	}
	else{
		vizinho=0;
		kvizinho=0;
	}
	if(cel3D.nele>0){
		eleUNV=velem.eleUNV;
		TL=velem.TL;
		local=velem.local;
		cel3D.noFace=0;
    	tempF=new double [cel3D.nvert];
    	gradTface=new double* [cel3D.nvert];
    	ccTD=new double [cel3D.nvert];
    	ccTVN=new double [cel3D.nvert];
    	ccHR=new double [cel3D.nvert];
    	ccTambR=new double [cel3D.nvert];
    	v1=new double [cel3D.dim];
    	v2=new double [cel3D.dim];
    	v3=new double [cel3D.dim];
    	for(int j=0; j<cel3D.nvert;j++){
        	ccTD[j]=-1;
        	ccTVN[j]=-1;
        	ccHR[j]=-1;
        	ccTambR[j]=-1;
    	}
    	for(int j=0; j<cel3D.nvert;j++) gradTface[j]=new double [cel3D.dim];
    	cel3D.gradGreenT=new double [cel3D.dim];
    	for(int j=0;j<cel3D.dim;j++) cel3D.gradGreenT[j]=velem.cel3D.gradGreenT[j];
    	cel3D.gradGreenTI=new double [cel3D.dim];
    	for(int j=0;j<cel3D.dim;j++) cel3D.gradGreenTI[j]=velem.cel3D.gradGreenTI[j];
    	cel3D.gradGreenT0=new double [cel3D.dim];
    	for(int j=0;j<cel3D.dim;j++) cel3D.gradGreenT0[j]=velem.cel3D.gradGreenT0[j];
    	for(int i=0; i<cel3D.nvert;i++){
        	tempF[i]=velem.tempF[i];
        	ccTD[i]=velem.ccTD[i];
        	ccTVN[i]=velem.ccTVN[i];
        	ccHR[i]=velem.ccHR[i];
        	ccTambR[i]=velem.ccTambR[i];
        	for(int j=0;j<cel3D.dim;j++) gradTface[i][j]=velem.gradTface[i][j];
    	}
    	coefTHRC=new double [cel3D.nvert];
    	coefTHRV=new double [cel3D.nvert];
    	fonteTHR=new double [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++){
    		coefTHRC[i]=velem.coefTHRC[i];
    		coefTHRV[i]=velem.coefTHRV[i];
    		fonteTHR[i]=velem.fonteTHR[i];
    	}
    	cel3D.noElem=new int [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.noElem[i]=velem.cel3D.noElem[i];
    	cel3D.indFace=new int [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.indFace[i]=velem.cel3D.indFace[i];
    	int size=velem.cel3D.indVizCres.size();
    	for(int i=0; i<size;i++)cel3D.indVizCres.push_back(velem.cel3D.indVizCres[i]);
    	cel3D.ccFace=new string [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.ccFace[i]=velem.cel3D.ccFace[i];
    	cel3D.orientFace=new double [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.orientFace[i]=velem.cel3D.orientFace[i];
    	cel3D.ownFace=new double [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.ownFace[i]=velem.cel3D.ownFace[i];
    	cel3D.centroElem=new double [cel3D.dim];
    	for(int i=0; i<cel3D.dim;i++)cel3D.centroElem[i]=velem.cel3D.centroElem[i];
    	cel3D.centroideElem=new double [cel3D.dim];
    	for(int i=0; i<cel3D.dim;i++)cel3D.centroideElem[i]=velem.cel3D.centroideElem[i];
    	cel3D.centroideFace=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.centroideFace[j]=new double [cel3D.dim];
    	for(int i=0; i<cel3D.nvert;i++)
    		for(int j=0; j<cel3D.dim;j++)cel3D.centroideFace[i][j]=velem.cel3D.centroideFace[i][j];
    	cel3D.coordVert=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.coordVert[j]=new double [cel3D.dim];
    	for(int i=0; i<cel3D.nvert;i++)
    		for(int j=0; j<cel3D.dim;j++)cel3D.coordVert[i][j]=velem.cel3D.coordVert[i][j];
    	for(int j=0; j<cel3D.dim;j++){
    		v1[j]=velem.v1[j];
    		v2[j]=velem.v2[j];
    		v3[j]=velem.v3[j];
    	}
    	cel3D.dCF=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.dCF[j]=new double [cel3D.dim];
       	for(int i=0; i<cel3D.nvert;i++)
        	for(int j=0; j<cel3D.dim;j++)cel3D.dCF[i][j]=velem.cel3D.dCF[i][j];
    	cel3D.dCFMod=new double [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.dCFMod[i]=velem.cel3D.dCFMod[i];
    	cel3D.sFace=new double* [cel3D.nvert];
    	for(int j=0; j<cel3D.nvert;j++) cel3D.sFace[j]=new double [cel3D.dim];
       	for(int i=0; i<cel3D.nvert;i++)
        	for(int j=0; j<cel3D.dim;j++)cel3D.sFace[i][j]=velem.cel3D.sFace[i][j];
    	cel3D.sFaceMod=new double [cel3D.nvert];
    	for(int i=0; i<cel3D.nvert;i++)cel3D.sFaceMod[i]=velem.cel3D.sFaceMod[i];
    	///////////////////////////////////////////////////////////////////////////////////////////
        cel3D.vecE=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.vecE[j]=new double [cel3D.dim];
    	for(int i=0; i<cel3D.nvert;i++)
    		for(int j=0; j<cel3D.dim;j++)cel3D.vecE[i][j]=velem.cel3D.vecE[i][j];
        cel3D.modE=new double [cel3D.nvert];
        for(int i=0; i<cel3D.nvert;i++)cel3D.modE[i]=velem.cel3D.modE[i];
        cel3D.vecT=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.vecT[j]=new double [cel3D.dim];
    	for(int i=0; i<cel3D.nvert;i++)
    		for(int j=0; j<cel3D.dim;j++)cel3D.vecT[i][j]=velem.cel3D.vecT[i][j];
        cel3D.modT=new double [cel3D.nvert];
        for(int i=0; i<cel3D.nvert;i++)cel3D.modT[i]=velem.cel3D.modT[i];
        cel3D.fatG=new double [cel3D.nvert];
        for(int i=0; i<cel3D.nvert;i++)cel3D.fatG[i]=velem.cel3D.fatG[i];
        cel3D.angES=new double [cel3D.nvert];
        for(int i=0; i<cel3D.nvert;i++)cel3D.angES[i]=velem.cel3D.angES[i];
        cel3D.fInter=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.fInter[j]=new double [cel3D.dim];
    	for(int i=0; i<cel3D.nvert;i++)
    		for(int j=0; j<cel3D.dim;j++)cel3D.fInter[i][j]=velem.cel3D.fInter[i][j];
        cel3D.fIfC=new double* [cel3D.nvert];
        for(int j=0; j<cel3D.nvert;j++) cel3D.fIfC[j]=new double [cel3D.dim];
    	for(int i=0; i<cel3D.nvert;i++)
    		for(int j=0; j<cel3D.dim;j++)cel3D.fIfC[i][j]=velem.cel3D.fIfC[i][j];
    	cel3D.vElem=velem.cel3D.vElem;
	}
	else{
		cel3D.noElem=0;
		cel3D.indFace=0;
		cel3D.ccFace=0;
		cel3D.orientFace=0;
		cel3D.centroElem=0;
		cel3D.centroideElem=0;
		cel3D.centroideFace=0;
		cel3D.coordVert=0;
		cel3D.dCF=0;
		cel3D.dCFMod=0;
		cel3D.vElem=0;
		cel3D.noFace=0;
		cel3D.sFace=0;
		cel3D.sFaceMod=0;
		cel3D.ownFace=0;
		//////////////////////////////////////////////////////////////////////////////////////
        cel3D.vecE=0;
        cel3D.modE=0;
        cel3D.vecT=0;
        cel3D.modT=0;
        cel3D.fatG=0;
        cel3D.angES=0;
        cel3D.fInter=0;
        cel3D.fIfC=0;
        cel3D.gradGreenT=0;
        cel3D.gradGreenTI=0;
        cel3D.gradGreenT0=0;
        tempF=0;
        gradTface=0;
    	ccTD=0;
    	ccTVN=0;
    	ccHR=0;
    	ccTambR=0;

    	coefTHRC=0;
    	coefTHRV=0;
    	fonteTHR=0;

		vizinho=0;
		kvizinho=0;

		v1=0;
		v2=0;
		v3=0;
		eleUNV=eleOBJ();
	}
}

elem3dPoisson& elem3dPoisson::operator =(const elem3dPoisson& velem) {
	if (this != &velem) {
		if(cel3D.nvert>0){
			delete[] cel3D.noElem;
			delete[] cel3D.indFace;
			delete[] cel3D.ccFace;
			delete[] cel3D.orientFace;
			delete[] cel3D.centroElem;
			delete[] cel3D.centroideElem;
			delete[] cel3D.dCFMod;
			delete[] cel3D.sFaceMod;
			delete[] cel3D.ownFace;
			delete[] ccTD;
			delete[] ccTVN;
			delete[] ccHR;
			delete[] ccTambR;
			for(int i=0;i<cel3D.nvert;i++){
				delete[] cel3D.centroideFace[i];
				delete[] cel3D.coordVert[i];
				delete[] cel3D.dCF[i];
				delete[] cel3D.sFace[i];
				///////////////////////////////////////////////////////////
				delete[] cel3D.vecE[i];
				delete[] cel3D.vecT[i];
				delete[] cel3D.fInter[i];
				delete[] cel3D.fIfC[i];
				///////////////////////////////////////////////////////////
				delete[] gradTface[i];
			}
			delete[] cel3D.centroideFace;
			delete[] cel3D.coordVert;
			delete[] cel3D.dCF;
			delete[] cel3D.sFace;
			delete[] vizinho;
			delete[] kvizinho;
			///////////////////////////////////////////////////////////////////////////////
			delete[] cel3D.vecE;
			delete[] cel3D.modE;
			delete[] cel3D.vecT;
			delete[] cel3D.modT;
			delete[] cel3D.fatG;
			delete[] cel3D.angES;
			delete[] cel3D.fInter;
			delete[] cel3D.fIfC;
			delete[] cel3D.gradGreenT;
			delete[] cel3D.gradGreenTI;
			delete[] cel3D.gradGreenT0;
			//////////////////////////////////////////////////////////////////////////////////
			delete[] tempF;
			delete[] gradTface;

			delete[] coefTHRC;
			delete[] coefTHRV;
			delete[] fonteTHR;

			delete[] v1;
			delete[] v2;
			delete[] v3;
		}
		/*if(nvizinho>0){
			delete[] vizinho;
			delete[] kvizinho;
		}*/
		DGP3D=velem.DGP3D;
		TL=velem.TL;
		local=velem.local;
		cel3D.nvert=velem.cel3D.nvert;
		cel3D.dim=velem.cel3D.dim;
		cel3D.indEle=velem.cel3D.indEle;
		cel3D.nele=velem.cel3D.nele;
		nvizinho=velem.nvizinho;
		cel3D.tempC=velem.cel3D.tempC;
		cel3D.tempC0=velem.cel3D.tempC0;
		cel3D.tempCI=velem.cel3D.tempCI;
		deriFonteT=velem.deriFonteT;
		FonteT=velem.FonteT;
		fluxCal=velem.fluxCal;
		cel3D.cond=velem.cel3D.cond;
		cel3D.rho=velem.cel3D.rho;
		cel3D.cp=velem.cel3D.cp;
		dt=velem.dt;
		//perm=velem.perm;
		//trans=velem.trans;
		ccTD=velem.ccTD;
		ccTVN=velem.ccTVN;
		ccHR=velem.ccHR;
		ccTambR=velem.ccTambR;
		CC=velem.CC;
		if(cel3D.nvert>0){
	    	vizinho=new elementoPoisson3D* [cel3D.nvert];
	    	kvizinho=new int [cel3D.nvert];
			for(int k=0; k<cel3D.nvert;k++){
				kvizinho[k]=velem.kvizinho[k];
				vizinho[k]=velem.vizinho[k];
			}
		}
		else{
			vizinho=0;
			kvizinho=0;
		}
    	v1=new double [cel3D.dim];
    	v2=new double [cel3D.dim];
    	v3=new double [cel3D.dim];
		if(cel3D.nele>0){
			eleUNV=velem.eleUNV;
			cel3D.noFace=0;
	    	tempF=new double [cel3D.nvert];
	    	gradTface=new double* [cel3D.nvert];
	    	ccTD=new double [cel3D.nvert];
	    	ccTVN=new double [cel3D.nvert];
	    	ccHR=new double [cel3D.nvert];
	    	ccTambR=new double [cel3D.nvert];
	    	for(int j=0; j<cel3D.nvert;j++) gradTface[j]=new double [cel3D.dim];
	    	cel3D.gradGreenT=new double [cel3D.dim];
	    	for(int j=0;j<cel3D.dim;j++) cel3D.gradGreenT[j]=velem.cel3D.gradGreenT[j];
	    	cel3D.gradGreenTI=new double [cel3D.dim];
	    	for(int j=0;j<cel3D.dim;j++) cel3D.gradGreenTI[j]=velem.cel3D.gradGreenTI[j];
	    	cel3D.gradGreenT0=new double [cel3D.dim];
	    	for(int j=0;j<cel3D.dim;j++) cel3D.gradGreenT0[j]=velem.cel3D.gradGreenT0[j];
	    	for(int i=0; i<cel3D.nvert;i++){
	        	tempF[i]=velem.tempF[i];
	        	ccTD[i]=velem.ccTD[i];
	        	ccTVN[i]=velem.ccTVN[i];
	        	ccHR[i]=velem.ccHR[i];
	        	ccTambR[i]=velem.ccTambR[i];
	        	for(int j=0;j<cel3D.dim;j++) gradTface[i][j]=velem.gradTface[i][j];
	    	}
	    	coefTHRC=new double [cel3D.nvert];
	    	coefTHRV=new double [cel3D.nvert];
	    	fonteTHR=new double [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++){
	        	coefTHRC[i]=velem.coefTHRC[i];
	        	coefTHRV[i]=velem.coefTHRV[i];
	        	fonteTHR[i]=velem.fonteTHR[i];
	    	}
	    	for(int j=0; j<cel3D.dim;j++){
	    		v1[j]=velem.v1[j];
	    		v2[j]=velem.v2[j];
	    		v3[j]=velem.v3[j];
	    	}
	    	cel3D.noElem=new int [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.noElem[i]=velem.cel3D.noElem[i];
	    	cel3D.indFace=new int [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.indFace[i]=velem.cel3D.indFace[i];
	    	int size=velem.cel3D.indVizCres.size();
	    	cel3D.indVizCres.clear();
	    	for(int i=0; i<size;i++)cel3D.indVizCres.push_back(velem.cel3D.indVizCres[i]);
	    	cel3D.ccFace=new string [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.ccFace[i]=velem.cel3D.ccFace[i];
	    	cel3D.orientFace=new double [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.orientFace[i]=velem.cel3D.orientFace[i];
	    	cel3D.ownFace=new double [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.ownFace[i]=velem.cel3D.ownFace[i];
	    	cel3D.centroElem=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.dim;i++)cel3D.centroElem[i]=velem.cel3D.centroElem[i];
	    	cel3D.centroideElem=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.dim;i++)cel3D.centroideElem[i]=velem.cel3D.centroideElem[i];
	    	cel3D.centroideFace=new double* [cel3D.nvert];
	    	for(int j=0; j<cel3D.nvert;j++) cel3D.centroideFace[j]=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.nvert;i++)
	    		for(int j=0; j<cel3D.dim;j++)cel3D.centroideFace[i][j]=velem.cel3D.centroideFace[i][j];
	    	cel3D.coordVert=new double* [cel3D.nvert];
	    	for(int j=0; j<cel3D.nvert;j++) cel3D.coordVert[j]=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.nvert;i++)
	    		for(int j=0; j<cel3D.dim;j++)cel3D.coordVert[i][j]=velem.cel3D.coordVert[i][j];
	    	cel3D.dCF=new double* [cel3D.nvert];
	    	for(int j=0; j<cel3D.nvert;j++) cel3D.dCF[j]=new double [cel3D.dim];
	       	for(int i=0; i<cel3D.nvert;i++)
	        	for(int j=0; j<cel3D.dim;j++)cel3D.dCF[i][j]=velem.cel3D.dCF[i][j];
	    	cel3D.dCFMod=new double [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.dCFMod[i]=velem.cel3D.dCFMod[i];
	    	cel3D.sFace=new double* [cel3D.nvert];
	    	for(int j=0; j<cel3D.nvert;j++) cel3D.sFace[j]=new double [cel3D.dim];
	       	for(int i=0; i<cel3D.nvert;i++)
	        	for(int j=0; j<cel3D.dim;j++)cel3D.sFace[i][j]=velem.cel3D.sFace[i][j];
	    	cel3D.sFaceMod=new double [cel3D.nvert];
	    	for(int i=0; i<cel3D.nvert;i++)cel3D.sFaceMod[i]=velem.cel3D.sFaceMod[i];
	    	///////////////////////////////////////////////////////////////////////////////////////////
	        cel3D.vecE=new double* [cel3D.nvert];
	        for(int j=0; j<cel3D.nvert;j++) cel3D.vecE[j]=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.nvert;i++)
	    		for(int j=0; j<cel3D.dim;j++)cel3D.vecE[i][j]=velem.cel3D.vecE[i][j];
	        cel3D.modE=new double [cel3D.nvert];
	        for(int i=0; i<cel3D.nvert;i++)cel3D.modE[i]=velem.cel3D.modE[i];
	        cel3D.vecT=new double* [cel3D.nvert];
	        for(int j=0; j<cel3D.nvert;j++) cel3D.vecT[j]=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.nvert;i++)
	    		for(int j=0; j<cel3D.dim;j++)cel3D.vecT[i][j]=velem.cel3D.vecT[i][j];
	        cel3D.modT=new double [cel3D.nvert];
	        for(int i=0; i<cel3D.nvert;i++)cel3D.modT[i]=velem.cel3D.modT[i];
	        cel3D.fatG=new double [cel3D.nvert];
	        for(int i=0; i<cel3D.nvert;i++)cel3D.fatG[i]=velem.cel3D.fatG[i];
	        cel3D.angES=new double [cel3D.nvert];
	        for(int i=0; i<cel3D.nvert;i++)cel3D.angES[i]=velem.cel3D.angES[i];
	        cel3D.fInter=new double* [cel3D.nvert];
	        for(int j=0; j<cel3D.nvert;j++) cel3D.fInter[j]=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.nvert;i++)
	    		for(int j=0; j<cel3D.dim;j++)cel3D.fInter[i][j]=velem.cel3D.fInter[i][j];
	        cel3D.fIfC=new double* [cel3D.nvert];
	        for(int j=0; j<cel3D.nvert;j++) cel3D.fIfC[j]=new double [cel3D.dim];
	    	for(int i=0; i<cel3D.nvert;i++)
	    		for(int j=0; j<cel3D.dim;j++)cel3D.fIfC[i][j]=velem.cel3D.fIfC[i][j];
	    	cel3D.vElem=velem.cel3D.vElem;
		}
		else{
			cel3D.noElem=0;
			cel3D.indFace=0;
			cel3D.ccFace=0;
			cel3D.orientFace=0;
			cel3D.centroElem=0;
			cel3D.centroideElem=0;
			cel3D.centroideFace=0;
			cel3D.coordVert=0;
			cel3D.dCF=0;
			cel3D.dCFMod=0;
			cel3D.vElem=0;
			cel3D.noFace=0;
			cel3D.sFace=0;
			cel3D.sFaceMod=0;
			cel3D.ownFace=0;
			//////////////////////////////////////////////////////////////////////////////////////
	        cel3D.vecE=0;
	        cel3D.modE=0;
	        cel3D.vecT=0;
	        cel3D.modT=0;
	        cel3D.fatG=0;
	        cel3D.angES=0;
	        cel3D.fInter=0;
	        cel3D.fIfC=0;
	        cel3D.gradGreenT=0;
	        cel3D.gradGreenTI=0;
	        cel3D.gradGreenT0=0;
	        tempF=0;
	        gradTface=0;
	    	ccTD=0;
	    	ccTVN=0;
	    	ccHR=0;
	    	ccTambR=0;

	    	coefTHRC=0;
	    	coefTHRV=0;
	    	fonteTHR=0;

			vizinho=0;
			kvizinho=0;

			v1=0;
			v2=0;
			v3=0;
			eleUNV=eleOBJ();
		}
	}
	return *this;
}

void elem3dPoisson::buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle){
	for(int i=0;i<nVert;i++){
		face[i]=eleUNV.indViz[i];
	}
}

void elem3dPoisson::menorIndViz(){
	/*int rank=0;
	if(kvizinho[iviz]>=0){
		for(int i=0;i<cel3D.nvert;i++){
			if(kvizinho[i]>=0 && cel3D.indFace[iviz]>cel3D.indFace[i])rank++;
		}
	}
	else rank=-1;
	return rank;*/

	cel3D.indVizCres.push_back(cel3D.indEle);
	for(int i=0;i<cel3D.nvert;i++)if(cel3D.indFace[i]>=0)cel3D.indVizCres.push_back(cel3D.indFace[i]);
	sort(cel3D.indVizCres.begin(), cel3D.indVizCres.end());
}

double elem3dPoisson::escalar(double* v1, double* v2, int vDim){
	double tot=0.;
	for(int i=0;i<vDim;i++)tot+=v1[i]*v2[i];
	return tot;
}

void elem3dPoisson::indraz(int& ind, double& raz,
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

void elem3dPoisson::faceDetalhes(){

	/*
	double** vecE;//vetor unitario na orientacao entre o centroide de do elementoPoisson3D e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elementoPoisson3D e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;//distancia entre o centroide da face e a intersecao da face
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementoPoisson3Ds
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementoPoisson3Ds e a face do elementoPoisson3D
    double** fIfC;//vetor entre a intersecao e o centroide da face
	 */
	for(int i=0;i<cel3D.nvert;i++){
		cel3D.modE[i]=0.;
		for(int j=0;j<cel3D.dim;j++){
			if(kvizinho[i]>=0){
				cel3D.vecE[i][j]=(*vizinho[i]).centroideElem[j]-cel3D.centroideElem[j];
			}
			else{
				cel3D.vecE[i][j]=cel3D.centroideFace[i][j]-cel3D.centroideElem[j];
			}
			cel3D.modE[i]+=cel3D.vecE[i][j]*cel3D.vecE[i][j];
		}
		cel3D.modE[i]=sqrtl(cel3D.modE[i]);
		for(int j=0;j<cel3D.dim;j++)cel3D.vecE[i][j]=cel3D.vecE[i][j]/cel3D.modE[i];
		cel3D.angES[i]=0.;
		for(int j=0;j<cel3D.dim;j++){
			cel3D.angES[i]+=(cel3D.sFace[i][j]/cel3D.sFaceMod[i])*cel3D.vecE[i][j];
		}
		double denf=0.;
		double numf=0.;
		for(int j=0;j<cel3D.dim;j++){
			numf+=(cel3D.centroideFace[i][j]-cel3D.centroideElem[j])*cel3D.sFace[i][j];
			denf+=cel3D.sFace[i][j]*cel3D.vecE[i][j];
		}
		double kf=numf/denf;
		for(int j=0;j<cel3D.dim;j++){
			//cel3D.fInter[i][j]=
					//(cel3D.centroideFace[i][j]-cel3D.centroideElem[j])*cel3D.sFace[i][j]/(cel3D.sFace[i][j]*cel3D.vecE[i][j])+
					//cel3D.centroideElem[j];
			cel3D.fInter[i][j]=kf*cel3D.vecE[i][j]+cel3D.centroideElem[j];
		}

		cel3D.fatG[i]=0;
		if(kvizinho[i]>=0){
			for(int j=0;j<cel3D.dim;j++){
				double del=(*vizinho[i]).centroideElem[j]-cel3D.fInter[i][j];
				cel3D.fatG[i]+=del*del;
			}
			cel3D.fatG[i]=sqrtl(cel3D.fatG[i]);
			cel3D.fatG[i]/=cel3D.modE[i];
		}
		else cel3D.fatG[i]=0.;

		cel3D.modT[i]=0.;
		for(int j=0;j<cel3D.dim;j++){
			cel3D.vecT[i][j]=cel3D.centroideFace[i][j]-cel3D.fInter[i][j];
			cel3D.modT[i]+=cel3D.vecT[i][j]*cel3D.vecT[i][j];
		}
		cel3D.modT[i]=sqrtl(cel3D.modT[i]);

	}

}

void elem3dPoisson::tipoCC(int i,int& diri, int& vn, int& rich, int& acoplado, int& kcc){
	while(kcc<(*CC).nDiri && cel3D.ccFace[i]!=(*CC).ccDir[kcc].rotulo) kcc++;
	if(kcc<(*CC).nDiri && cel3D.ccFace[i]==(*CC).ccDir[kcc].rotulo)diri=1;
    else{
		kcc=0;
		while(kcc<(*CC).nVN && cel3D.ccFace[i]!=(*CC).ccVN[kcc].rotulo) kcc++;
		if(kcc<(*CC).nVN && cel3D.ccFace[i]==(*CC).ccVN[kcc].rotulo)vn=1;
		else{
			kcc=0;
			while(kcc<(*CC).nRic && cel3D.ccFace[i]!=(*CC).ccRic[kcc].rotulo) kcc++;
			if(kcc<(*CC).nRic && cel3D.ccFace[i]==(*CC).ccRic[kcc].rotulo)rich=1;
			else{
				kcc=0;
				//while(kcc<1 && cel3D.ccFace[i]!=(*CC).rotuloAcop) kcc++;
				//if(kcc<1 && cel3D.ccFace[i]==(*CC).rotuloAcop)acoplado=1;
				while(kcc<(*CC).nAcop && cel3D.ccFace[i]!=(*CC).rotuloAcop[kcc]) kcc++;
				if(kcc<(*CC).nAcop && cel3D.ccFace[i]==(*CC).rotuloAcop[kcc])acoplado=1;
			}
		}
	}
}

void elem3dPoisson::atualizaCC(int i){
	int diri=0;
	int rich=0;
	int vn=0;
	int kcc=0;
	int acoplado=0;
	tipoCC(i, diri, vn, rich,acoplado,kcc);
    int ind=0;
    double raz;
	if(diri==1){

		indraz(ind, raz, (*DGP3D).tempo, (*CC).ccDir[kcc].nserie, (*CC).ccDir[kcc].tempo);
		double inf = (*CC).ccDir[kcc].val[ind];
		double sup;
		if (ind < (*CC).ccDir[kcc].nserie - 1){
			sup = (*CC).ccDir[kcc].val[ind + 1];
			ccTD[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccTD[i] = (*CC).ccDir[kcc].val[ind];
	}
	else if(vn==1){

		indraz(ind, raz, (*DGP3D).tempo, (*CC).ccVN[kcc].nserie, (*CC).ccVN[kcc].tempo);
		double inf = (*CC).ccVN[kcc].val[ind];
		double sup;
		if (ind < (*CC).ccVN[kcc].nserie - 1){
			sup = (*CC).ccVN[kcc].val[ind + 1];
			ccTVN[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccTVN[i] = (*CC).ccVN[kcc].val[ind];
	}
	else if(rich==1){

		indraz(ind, raz, (*DGP3D).tempo, (*CC).ccRic[kcc].nserie, (*CC).ccRic[kcc].tempo);
		double inf = (*CC).ccRic[kcc].valAmb[ind];
		double sup;
		if (ind < (*CC).ccRic[kcc].nserie - 1){
			sup = (*CC).ccRic[kcc].valAmb[ind + 1];
			ccTambR[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccTambR[i] = (*CC).ccRic[kcc].valAmb[ind];



		inf = (*CC).ccRic[kcc].hAmb[ind];
		if (ind < (*CC).ccRic[kcc].nserie - 1){
			sup = (*CC).ccRic[kcc].hAmb[ind + 1];
			ccHR[i] = (inf * raz + (1 - raz) * sup);
		}
		else
			ccHR[i] = (*CC).ccRic[kcc].hAmb[ind];

	}
}

void elem3dPoisson::calcGradGreen(){
	for(int i=0;i<cel3D.nvert;i++){
		if(kvizinho[i]>=0){
			double tinter=cel3D.fatG[i]*cel3D.tempC+(1.-cel3D.fatG[i])*(*vizinho[i]).tempC;
			tempF[i]=0.;
				//esta abordagem não parece corrigir bem a distorção pelo fator da interpolação nção estar no centro da superfície:

			//for(int j=0; j<cel3D.dim;j++)
			//tempF[i]+=cel3D.fatG[i]*cel3D.gradGreenT[j]*(cel3D.centroideFace[i][j]-cel3D.centroideElem[j])+
			//(1-cel3D.fatG[i])*(*vizinho[i]).gradGreenT[j]*(cel3D.centroideFace[i][j]-(*vizinho[i]).centroideElem[j]);

				//esta abordagem, aparentemente. levou a resultados melhores:

			//for(int j=0; j<cel3D.dim;j++)
			//tempF[i]+=cel3D.fatG[i]*cel3D.gradGreenT[j]*(cel3D.centroideFace[i][j]-cel3D.fInter[i][j])+
				//	  (1-cel3D.fatG[i])*(*vizinho[i]).gradGreenT[j]*(cel3D.centroideFace[i][j]-cel3D.fInter[i][j]);
			//tempF[i]+=tinter;


			double gradMed [cel3D.dim];
			for(int j=0; j<cel3D.dim;j++){
				gradMed[j]=cel3D.fatG[i]*cel3D.gradGreenTI[j]+(1-cel3D.fatG[i])*(*vizinho[i]).gradGreenTI[j];
			}
			double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
			double deri=-(cel3D.tempC-(*vizinho[i]).tempC)/cel3D.modE[i];
			for(int j=0; j<cel3D.dim;j++){
				gradMed[j]=gradMed[j]+(deri-termoCorda)*cel3D.vecE[i][j];
			}
			double corDistorc=1.;
			if(fabsl(cel3D.angES[i])<0*1+1*0.866)corDistorc=0.;
			else if(eleUNV.RAspec>0*1+1*2.)corDistorc=0.;
			for(int j=0; j<cel3D.dim;j++)
			tempF[i]+=corDistorc*gradMed[j]*(cel3D.centroideFace[i][j]-cel3D.fInter[i][j]);
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
				/*double condHarm=cel3D.cond;
				//double gradareaB=-ccTVN[i]/condHarm;???????
				double gradareaB=ccTVN[i]/condHarm;//fluxo negativo, saindo do sólido, grad negativo

				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenT[j];
				}
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				//tempF[i]=cel3D.tempC+(gradareaB-escalGradArea+cel3D.sFaceMod[i]*termoCorda)*cel3D.modE[i]/cel3D.sFaceMod[i];
				coefTHRC[i]=1.;
				fonteTHR[i]=(gradareaB-escalGradArea+termoCorda*cordaArea)*cel3D.modE[i]/cordaArea;
				tempF[i]=cel3D.tempC+fonteTHR[i];*/
				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenTI[j];
				}
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				coefTHRC[i]=1.;
				fonteTHR[i]=termoCorda*cel3D.modE[i];
				tempF[i]=cel3D.tempC+fonteTHR[i];
			}
			else if(rich==1 || acoplado==1){
				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenTI[j];
				}
				double distorc=1.;
				if(eleUNV.RAspec>0*1.+1*2.)distorc=0.;
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				double denom=(cel3D.cond*cordaArea/cel3D.modE[i])+ccHR[i]*cel3D.sFaceMod[i];
				//double denom=distorc*(cel3D.cond*cordaArea/cel3D.modE[i])+ccHR[i]*cel3D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				double correrT=escalGradArea-termoCorda*cordaArea;
				//double correrT=escalGradArea-distorc*termoCorda*cordaArea;
				double num=ccHR[i]*cel3D.sFaceMod[i]*ccTambR[i]+
						cel3D.cond*cordaArea*cel3D.tempC/cel3D.modE[i]-distorc*cel3D.cond*correrT;
				//double num=ccHR[i]*cel3D.sFaceMod[i]*ccTambR[i]+
						    //distorc*cel3D.cond*cordaArea*cel3D.tempC/cel3D.modE[i]-cel3D.cond*correrT;

				coefTHRC[i]=(cel3D.cond*cordaArea/cel3D.modE[i])/denom;
				fonteTHR[i]=(ccHR[i]*cel3D.sFaceMod[i]*ccTambR[i]-distorc*cel3D.cond*correrT)/denom;
				//coefTHRC[i]=distorc*(cel3D.cond*cordaArea/cel3D.modE[i])/denom;
				//fonteTHR[i]=(ccHR[i]*cel3D.sFaceMod[i]*ccTambR[i]-cel3D.cond*correrT)/denom;
				tempF[i]=num/denom;
				//if(eleUNV.RAspec>5)tempF[i]=ccTambR[i];
			}
			/*else if(vn==1){
				double condHarm=cel3D.cond;
				double gradareaB=-ccTVN[i]/condHarm;

				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenT[j];
				}
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				//tempF[i]=cel3D.tempC+(gradareaB-escalGradArea+cel3D.sFaceMod[i]*termoCorda)*cel3D.modE[i]/cel3D.sFaceMod[i];
				tempF[i]=cel3D.tempC+(gradareaB-escalGradArea+termoCorda*cordaArea)*cel3D.modE[i]/cordaArea;
			}
			else if(rich==1){
				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenT[j];
				}
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				double denom=(cel3D.cond*cordaArea/cel3D.modE[i])+ccHR[i]*cel3D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				double correrT=escalGradArea-termoCorda*cordaArea;
				double num=ccHR[i]*cel3D.sFaceMod[i]*ccTambR[i]+
						cel3D.cond*cordaArea*cel3D.tempC/cel3D.modE[i]-cel3D.cond*correrT;
				tempF[i]=num/denom;
			}*/
		}
	}
	for(int j=0; j<cel3D.dim;j++)cel3D.gradGreenT[j]=0.;
	for(int i=0;i<cel3D.nvert;i++){
		for(int j=0; j<cel3D.dim;j++){
			cel3D.gradGreenT[j]+=tempF[i]*cel3D.sFace[i][j];
		}
	}
	for(int j=0; j<cel3D.dim;j++)cel3D.gradGreenT[j]/=cel3D.vElem;
}

int elem3dPoisson::achaInd(int i){
	int siz=cel3D.indVizCres.size();
	int saida=-1;
	for(int j=0;j<siz;j++){
		if(i==cel3D.indVizCres[j]){
			saida=j;
			break;
		}
	}
	return saida;
}

void elem3dPoisson::GeraLocal(double relax){
	TL[0]=0.;
	int diag=achaInd(cel3D.indEle);
	local.mx[0][diag]=0.;
	for(int i=0;i<cel3D.nvert;i++){
		if(kvizinho[i]>=0){
			int col=achaInd(cel3D.indFace[i]);
			double gradMed [cel3D.dim];
			for(int j=0; j<cel3D.dim;j++){
				gradMed[j]=cel3D.fatG[i]*cel3D.gradGreenT[j]+(1-cel3D.fatG[i])*(*vizinho[i]).gradGreenT[j];
			}
			double condHarm;//=2*cel3D.cond*(*vizinho[i]).cond/(cel3D.cond+(*vizinho[i]).cond);
			condHarm=cel3D.fatG[i]/cel3D.cond+(1-cel3D.fatG[i])/(*vizinho[i]).cond;
			condHarm=1./condHarm;
			double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
			double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
			double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
			TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
			double termMat=(condHarm*cordaArea/cel3D.modE[i]);
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

				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenT[j];
				}
				double condHarm=cel3D.cond;
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=(condHarm*cordaArea/cel3D.modE[i]);
				TL[0]+=termMat*ccTD[i];
				local.mx[0][diag]+=termMat;
			}
			else if(rich==1 || acoplado==1){
				if(eleUNV.RAspec<0*1.+1*200.){
					double gradMed [cel3D.dim];
					for(int j=0; j<cel3D.dim;j++){
						gradMed[j]=cel3D.gradGreenT[j];
					}
					double condHarm=cel3D.cond;
					double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
					double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
					double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
					double termMat=(condHarm*cordaArea/cel3D.modE[i]);
					//if(eleUNV.RAspec<0*1.+1*4.){
					TL[0]+=1*condHarm*(escalGradArea-termoCorda*cordaArea);
					TL[0]+=termMat*fonteTHR[i];
					local.mx[0][diag]+=termMat*(1.-coefTHRC[i]);
					//}
					//else TL[0]+=1*condHarm*(escalGradArea);
				}
				else{
					double condHarm=cel3D.cond;
					double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
					double termMat=(condHarm*cordaArea/cel3D.modE[i]);
					TL[0]+=termMat*fonteTHR[i];
					local.mx[0][diag]+=ccHR[i]*cel3D.sFaceMod[i]*coefTHRC[i];
				}
			}
			else if(vn==1){
				TL[0]+=ccTVN[i]*cel3D.sFaceMod[i];
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


				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenT[j];
				}
				double condHarm=cel3D.cond;
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				TL[0]+=condHarm*(escalGradArea-termoCorda*cordaArea);
				double termMat=(condHarm*cordaArea/cel3D.modE[i]);
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

				double gradMed [cel3D.dim];
				for(int j=0; j<cel3D.dim;j++){
					gradMed[j]=cel3D.gradGreenT[j];
				}
				double cordaArea=escalar(cel3D.vecE[i],cel3D.sFace[i],cel3D.dim);
				double num=ccHR[i]*(cel3D.cond*cordaArea/cel3D.modE[i])*cel3D.sFaceMod[i];
				double denom=(cel3D.cond*cordaArea/cel3D.modE[i])+ccHR[i]*cel3D.sFaceMod[i];
				double termoCorda=escalar(gradMed,cel3D.vecE[i],cel3D.dim);
				double escalGradArea=escalar(gradMed,cel3D.sFace[i],cel3D.dim);
				double termLiv=-(ccHR[i]*cel3D.cond*cel3D.sFaceMod[i]*(escalGradArea-termoCorda*cordaArea)+num*ccTambR[i])/denom;
				TL[0]+=termLiv;
				local.mx[0][diag]-=num/denom;
			}*/
		}
	}
	//if(perm==0 && trans==1){
		TL[0]+=cel3D.vElem*cel3D.rho*cel3D.cp*cel3D.tempC0/dt+FonteT*cel3D.vElem;
		local.mx[0][diag]+=cel3D.vElem*(cel3D.rho*cel3D.cp/dt-deriFonteT);
	//}
	double acTemp=local.mx[0][diag];
	local.mx[0][diag]=acTemp/relax;
	TL[0]+=((1-relax)/relax)*acTemp*cel3D.tempCI;
}

