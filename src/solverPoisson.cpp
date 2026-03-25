/*
 * solverPoisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "solverPoisson.h"

solverP::solverP(varGlob1D* Vvg1dSP,string nomeArquivoEntrada, double vCondGlob,double vCondLoc, double vhE, double vhInt,
		double vTint, double vTamb,double vdiamI, double vdiamE,int vindcel):
dados(nomeArquivoEntrada,vCondGlob,vCondLoc,vhE,vhInt,vTint,vTamb,vdiamI,vdiamE),
termolivre(dados.nele),matglob(dados.nele,dados.noZero,dados.vecsra,dados.veclm,dados.vecfnz){
	//, double vCondGlob,double vCondLoc, double vhE, double vhInt,
	//double vTint, double vTamb,double vdiamI, double vdiamE
    //lixo5=0.;
	vg1dSP=Vvg1dSP;
	indCel=vindcel;
    malha=malha2d(vg1dSP,dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno,dados.temp.perm,dados.temp.trans);
    for(int i=0;i<malha.nele;i++){
    	malha.mlh2d[i].CC=dados.CC;
    	for(int j=0; j<dados.prop.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.prop.xmax[j] && xc>=dados.prop.xmin[j])&&(yc<=dados.prop.ymax[j] && yc>=dados.prop.ymin[j])){
    			malha.mlh2d[i].cel2D.cond=dados.prop.cond[j];
    			malha.mlh2d[i].cel2D.cp=dados.prop.cp[j];
    			malha.mlh2d[i].cel2D.rho=dados.prop.rho[j];
    			break;
    		}
    	}
    	for(int j=0; j<dados.CI.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.CI.xmax[j] && xc>=dados.CI.xmin[j])&&(yc<=dados.CI.ymax[j] && yc>=dados.CI.ymin[j])){
    			malha.mlh2d[i].cel2D.tempC=dados.CI.temp[j];
    			malha.mlh2d[i].cel2D.tempC0=dados.CI.temp[j];
    			break;
    		}
    	}
    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
    		if(malha.mlh2d[i].kvizinho[j]<0){
    			malha.mlh2d[i].atualizaCC(j);
    		}
    	}
    }

    int indsra=0;
    for(int i=0;i<malha.nele;i++){
    	int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
    	//vecfnz[i]=malha.mlh2d[i].cel2D.indVizCres[0];
    	matglob.fnz[i]=indsra;
    	for(int j=0;j<dimNoZero;j++){
    		int ind=malha.mlh2d[i].cel2D.indVizCres[j];
    		matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
    		matglob.clm[indsra+j]=ind;
    	}
    	indsra+=dimNoZero;
    	termolivre[i]=malha.mlh2d[i].TL[0];
    }
    matglob.fnz[malha.nele]=dados.noZero;

    if(dados.rankLU>=0){
  	  matglob.rank=dados.rankLU;
  	  if(dados.rankLU>0){
  	    	matglob.geraILUrank();
  	    	matglob.sraILU=new double[matglob.noZeroIlu];
  	  }
  	  matglob.colorido=dados.colore;
  	  if(dados.colore)matglob.geraILUcor();
    }
  	precn=1;
  	if(dados.rankLU>=0)precn=4;

    if(dados.acop==1){
    	dados.hE=vhE;
    	dados.hI=vhInt;
    	dados.condGlob=vCondGlob;
    	dados.condLoc=vCondLoc;
    	dados.tAmb=vTamb;
    	dados.tInt=vTint;
    	dados.diamInt=vdiamI;
    	dados.diamExt=vdiamE;
    	//tParede=
    	dados.qDesacop=((2*M_PI-dados.angAcop)*(dados.tInt-dados.tAmb))/(1./dados.condGlob+1./(0.5*dados.diamInt*dados.hI)+
    			1./(0.5*dados.diamExt*dados.hE));
    	dados.qAcop=0.;
    	for(int i=0; i<dados.CC.nRic; i++){
    		for(int j=0; j<dados.CC.ccRic[i].nserie; j++){
    			dados.CC.ccRic[i].valAmb[j]=dados.tAmb;
    			dados.CC.ccRic[i].hAmb[j]=dados.hE;
    		}
    	}
		for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						malha.mlh2d[i].ccHR[j]=dados.condLoc;
						//qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(tInt-malha.mlh2d[i].tempF[j])/(1/condLoc+1./(hI));
						dados.qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(dados.tInt-malha.mlh2d[i].cel2D.tempC)/(1/dados.condLoc+1./(dados.hI));
					}
	    		}
	    	}
		}
		dados.qTotal=dados.qAcop+dados.qDesacop;
		dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.diamInt*dados.hI);
		for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						malha.mlh2d[i].ccTambR[j]=dados.tParede;
					}
	    		}
	    	}
		}

    }
    //malhaRegular( dados.dxH, dados.dyH);
	kontaTempo=0;
}

solverP::solverP():
		dados(),termolivre(1),matglob(1,1){
	    vg1dSP=0;
		indCel=0;
		//malhaH=0;
		kontaTempo=0;
		malha=malha2d();
		precn=1;
}

solverP::solverP(const solverP& vPoisson):
		dados(vPoisson.dados.entrada,vPoisson.dados.condGlob,vPoisson.dados.condLoc,vPoisson.dados.hE,vPoisson.dados.hI,
				vPoisson.dados.tInt,vPoisson.dados.tAmb,vPoisson.dados.diamInt,vPoisson.dados.diamExt),
		termolivre(dados.nele),matglob(dados.nele,dados.noZero,dados.vecsra,dados.veclm,dados.vecfnz){
    //tempo=0.;
	vg1dSP=vPoisson.vg1dSP;
    malha=malha2d(vg1dSP,dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno,dados.temp.perm,dados.temp.trans);
    indCel=vPoisson.indCel;
    for(int i=0;i<malha.nele;i++){
    	malha.mlh2d[i].CC=dados.CC;
    	for(int j=0; j<dados.prop.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.prop.xmax[j] && xc>=dados.prop.xmin[j])&&(yc<=dados.prop.ymax[j] && yc>=dados.prop.ymin[j])){
    			malha.mlh2d[i].cel2D.cond=dados.prop.cond[j];
    			malha.mlh2d[i].cel2D.cp=dados.prop.cp[j];
    			malha.mlh2d[i].cel2D.rho=dados.prop.rho[j];
    			break;
    		}
    	}
    	for(int j=0; j<dados.CI.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.CI.xmax[j] && xc>=dados.CI.xmin[j])&&(yc<=dados.CI.ymax[j] && yc>=dados.CI.ymin[j])){
    			malha.mlh2d[i].cel2D.tempC=dados.CI.temp[j];
    			malha.mlh2d[i].cel2D.tempC0=dados.CI.temp[j];
    			break;
    		}
    	}
    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
    		if(malha.mlh2d[i].kvizinho[j]<0){
    			malha.mlh2d[i].atualizaCC(j);
    		}
    	}
    }

    int indsra=0;
    for(int i=0;i<malha.nele;i++){
    	int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
    	//vecfnz[i]=malha.mlh2d[i].cel2D.indVizCres[0];
    	matglob.fnz[i]=indsra;
    	for(int j=0;j<dimNoZero;j++){
    		int ind=malha.mlh2d[i].cel2D.indVizCres[j];
    		matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
    		matglob.clm[indsra+j]=ind;
    	}
    	indsra+=dimNoZero;
    	termolivre[i]=malha.mlh2d[i].TL[0];
    }
    matglob.fnz[malha.nele]=dados.noZero;

    if(dados.rankLU>=0){
  	  matglob.rank=dados.rankLU;
  	  if(dados.rankLU>0){
  	    	matglob.geraILUrank();
  	    	matglob.sraILU=new double[matglob.noZeroIlu];
  	  }
  	  matglob.colorido=dados.colore;
  	  if(dados.colore)matglob.geraILUcor();
    }
  	precn=1;
  	if(dados.rankLU>=0)precn=4;


    if(dados.acop==1){
    	//dados.hE=dados.transfer.hExt();
    	//dados.hI=dados.transfer.hInt();
    	//dados.condGlob=dados.transfer.condParede();
    	//dados.condLoc=dados.transfer.condParedeLocal();
    	//tParede=
    	dados.qDesacop=((2*M_PI-dados.angAcop)*(dados.tInt-dados.tAmb))/(1./dados.condGlob+1./(0.5*dados.diamInt*dados.hI)+
    			1./(0.5*dados.diamExt*dados.hE));
    	dados.qAcop=0.;
    	for(int i=0; i<dados.CC.nRic; i++){
    		for(int j=0; j<dados.CC.ccRic[i].nserie; j++){
    			dados.CC.ccRic[i].valAmb[j]=dados.tAmb;
    			dados.CC.ccRic[i].hAmb[j]=dados.hE;
    		}
    	}
		for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						malha.mlh2d[i].ccHR[j]=dados.condLoc;
						//qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(tInt-malha.mlh2d[i].tempF[j])/(1/condLoc+1./(hI));
						dados.qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(dados.tInt-malha.mlh2d[i].cel2D.tempC)/(1/dados.condLoc+1./(dados.hI));
					}
	    		}
	    	}
		}
		dados.qTotal=dados.qAcop+dados.qDesacop;
		dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.diamInt*dados.hI);
		for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						malha.mlh2d[i].ccTambR[j]=dados.tParede;
					}
	    		}
	    	}
		}

    }
    //malhaRegular( dados.dxH, dados.dyH);
	kontaTempo=vPoisson.kontaTempo;
}

solverP& solverP::operator =(const solverP& vPoisson) {
	if (this != &vPoisson) {
		/*if(dados.nxMH>0){
			for(int i=0;i<dados.nxMH;i++) delete[] malhaH[i];
			delete[] malhaH;
		}*/
		vg1dSP=vPoisson.vg1dSP;
		indCel=vPoisson.indCel;
		dados=vPoisson.dados;
		termolivre=vPoisson.termolivre;
		matglob=vPoisson.matglob;

	    //tempo=0.;
	    malha=malha2d(vg1dSP,dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno,dados.temp.perm,dados.temp.trans);
	    for(int i=0;i<malha.nele;i++){
	    	malha.mlh2d[i].CC=dados.CC;
	    	for(int j=0; j<dados.prop.parserie;j++){
	    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
	    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
	    		if((xc<=dados.prop.xmax[j] && xc>=dados.prop.xmin[j])&&(yc<=dados.prop.ymax[j] && yc>=dados.prop.ymin[j])){
	    			malha.mlh2d[i].cel2D.cond=dados.prop.cond[j];
	    			malha.mlh2d[i].cel2D.cp=dados.prop.cp[j];
	    			malha.mlh2d[i].cel2D.rho=dados.prop.rho[j];
	    			break;
	    		}
	    	}
	    	for(int j=0; j<dados.CI.parserie;j++){
	    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
	    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
	    		if((xc<=dados.CI.xmax[j] && xc>=dados.CI.xmin[j])&&(yc<=dados.CI.ymax[j] && yc>=dados.CI.ymin[j])){
	    			malha.mlh2d[i].cel2D.tempC=dados.CI.temp[j];
	    			malha.mlh2d[i].cel2D.tempC0=dados.CI.temp[j];
	    			break;
	    		}
	    	}
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
	    			malha.mlh2d[i].atualizaCC(j);
	    		}
	    	}
	    }

	    int indsra=0;
	    for(int i=0;i<malha.nele;i++){
	    	int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
	    	//vecfnz[i]=malha.mlh2d[i].cel2D.indVizCres[0];
	    	matglob.fnz[i]=indsra;
	    	for(int j=0;j<dimNoZero;j++){
	    		int ind=malha.mlh2d[i].cel2D.indVizCres[j];
	    		matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
	    		matglob.clm[indsra+j]=ind;
	    	}
	    	indsra+=dimNoZero;
	    	termolivre[i]=malha.mlh2d[i].TL[0];
	    }
	    if(malha.nele>0)matglob.fnz[malha.nele]=dados.noZero;

	    if(dados.rankLU>=0){
	  	  matglob.rank=dados.rankLU;
	  	  if(dados.rankLU>0){
	  	    	matglob.geraILUrank();
	  	    	matglob.sraILU=new double[matglob.noZeroIlu];
	  	  }
	  	  matglob.colorido=dados.colore;
	  	  if(dados.colore)matglob.geraILUcor();
	    }
	  	precn=1;
	  	if(dados.rankLU>=0)precn=4;


	    if(dados.acop==1){
	    	//dados.hE=dados.transfer.hExt();
	    	//dados.hI=dados.transfer.hInt();
	    	//dados.condGlob=dados.transfer.condParede();
	    	//dados.condLoc=dados.transfer.condParedeLocal();
	    	//tParede=
	    	dados.qDesacop=((2*M_PI-dados.angAcop)*(dados.tInt-dados.tAmb))/(1./dados.condGlob+1./(0.5*dados.diamInt*dados.hI)+
	    			1./(0.5*dados.diamExt*dados.hE));
	    	dados.qAcop=0.;
	    	for(int i=0; i<dados.CC.nRic; i++){
	    		for(int j=0; j<dados.CC.ccRic[i].nserie; j++){
	    			dados.CC.ccRic[i].valAmb[j]=dados.tAmb;
	    			dados.CC.ccRic[i].hAmb[j]=dados.hE;
	    		}
	    	}
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
						int kcc=0;
						while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
						if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
							malha.mlh2d[i].ccHR[j]=dados.condLoc;
							//qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(tInt-malha.mlh2d[i].tempF[j])/(1/condLoc+1./(hI));
							dados.qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(dados.tInt-malha.mlh2d[i].cel2D.tempC)/(1/dados.condLoc+1./(dados.hI));
						}
		    		}
		    	}
			}
			dados.qTotal=dados.qAcop+dados.qDesacop;
			dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.diamInt*dados.hI);
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
						int kcc=0;
						while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
						if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
							malha.mlh2d[i].ccTambR[j]=dados.tParede;
						}
		    		}
		    	}
			}

	    }
	    //if(malha.nele>0)malhaRegular( dados.dxH, dados.dyH);

		kontaTempo=vPoisson.kontaTempo;
	}

	return *this;
}

void solverP::indrazT(int& ind, double& raz) {

	for (int i = 0; i <= dados.temp.parserie - 1; i++) {
		if (i < dados.temp.parserie - 1) {
			if ((*vg1dSP).lixo5 >= dados.temp.tempo[i] && (*vg1dSP).lixo5 < dados.temp.tempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).lixo5 - dados.temp.tempo[i])
								/ (dados.temp.tempo[i + 1] - dados.temp.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == dados.temp.parserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

/*void solverP::malhaRegular(double& dx, double& dy){
	dados.xmin=malha.mlh2d[0].cel2D.centroideElem[0];
	dados.xmax=malha.mlh2d[0].cel2D.centroideElem[0];
	dados.ymin=malha.mlh2d[0].cel2D.centroideElem[1];
	dados.ymax=malha.mlh2d[0].cel2D.centroideElem[1];
	int nvert=3;
	for(int i=1;i<malha.nele;i++){
		for(int k=0;k<nvert;k++){
			if(malha.mlh2d[i].cel2D.coordVert[k][0]<dados.xmin)dados.xmin=malha.mlh2d[i].cel2D.coordVert[k][0];
			if(malha.mlh2d[i].cel2D.coordVert[k][0]>dados.xmax)dados.xmax=malha.mlh2d[i].cel2D.coordVert[k][0];
			if(malha.mlh2d[i].cel2D.coordVert[k][1]<dados.ymin)dados.ymin=malha.mlh2d[i].cel2D.coordVert[k][1];
			if(malha.mlh2d[i].cel2D.coordVert[k][1]>dados.ymax)dados.ymax=malha.mlh2d[i].cel2D.coordVert[k][1];
		}
	}
	//dados.nxMH=floor((dados.xmax-dados.xmin)/dx);
	//dados.nyMH=floor((dados.ymax-dados.ymin)/dy);
	//malhaH=new detMalhaHomPoisson* [dados.nxMH];
	//for(int i=0; i<dados.nxMH;i++)malhaH[i]=new detMalhaHomPoisson [dados.nyMH];
	//for(int i=0; i<dados.nxMH;i++)
		//for(int j=0; j<dados.nyMH;j++)malhaH[i][j].buraco=1;
	//dx=(dados.xmax-dados.xmin)/dados.nxMH;
	//dy=(dados.ymax-dados.ymin)/dados.nyMH;
	double xcoor=dados.xmin;
	double ycoor=dados.ymin;
	int interior=-1;
	int iele=0;
	double ylim[nvert];
	double xlim[nvert];
	for(int ix=0; ix<dados.nxMH; ix++){
		ycoor=dados.ymin;
		for(int iy=0; iy<dados.nyMH; iy++){
			iele=0;
			interior=-1;
			while(interior==-1 && iele<malha.nele){
				int xinterno[3];
				int yinterno[3];
				for(int ivert=0;ivert<nvert;ivert++){
					xinterno[ivert]=0;
					yinterno[ivert]=0;
					if(ivert<nvert-1){
						double delx=malha.mlh2d[iele].cel2D.coordVert[ivert+1][0]-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dely=malha.mlh2d[iele].cel2D.coordVert[ivert+1][1]-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						double dxteste=xcoor-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dyteste=ycoor-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						if(fabsl(delx)>1e-15)
							ylim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][1]+(dely/delx)*dxteste;
						else ylim[ivert]=1e20;
						if(fabsl(dely)>1e-15)
							xlim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][0]+(delx/dely)*dyteste;
						else xlim[ivert]=1e20;
						if((ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert+1][1]) ||
						   (ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert+1][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][1]))
							yinterno[ivert]=1;
						if((xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert+1][0]) ||
						   (xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert+1][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][0]))
							xinterno[ivert]=1;
					}
					else{
						double delx=malha.mlh2d[iele].cel2D.coordVert[0][0]-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dely=malha.mlh2d[iele].cel2D.coordVert[0][1]-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						double dxteste=xcoor-malha.mlh2d[iele].cel2D.coordVert[ivert][0];
						double dyteste=ycoor-malha.mlh2d[iele].cel2D.coordVert[ivert][1];
						if(fabsl(dely)>1e-15)
							ylim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][1]+(dely/delx)*dxteste;
						else ylim[ivert]=1e20;
						if(fabsl(dely)>1e-15)
							xlim[ivert]=malha.mlh2d[iele].cel2D.coordVert[ivert][0]+(delx/dely)*dyteste;
						else xlim[ivert]=1e20;
						if((ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[0][1]) ||
						   (ylim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[0][1] && ylim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][1]))
							yinterno[ivert]=1;
						if((xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[ivert][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[0][0]) ||
						   (xlim[ivert]<=malha.mlh2d[iele].cel2D.coordVert[0][0] && xlim[ivert]>=malha.mlh2d[iele].cel2D.coordVert[ivert][0]))
							xinterno[ivert]=1;
					}
				}
				int totalxyinterno=0;
				for(int ivert=0;ivert<nvert;ivert++){
					totalxyinterno+=(yinterno[ivert]+xinterno[ivert]);

				}
				if(totalxyinterno==4){
					malhaH[ix][iy].buraco=0;
					malhaH[ix][iy].indEle=iele;
					malhaH[ix][iy].xcoor=xcoor;
					malhaH[ix][iy].ycoor=ycoor;
					malhaH[ix][iy].delx=xcoor-malha.mlh2d[iele].cel2D.centroideElem[0];
					malhaH[ix][iy].dely=ycoor-malha.mlh2d[iele].cel2D.centroideElem[1];
					interior=0;
				}
				iele++;
				if(iele==malha.nele){
					int para;
					para=0;
				}
			}
			ycoor+=dy;
		}
		xcoor+=dx;
	}
	for(int i=0; i<dados.nxMH;i++){
		for(int j=0; j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==1){
				if((dados.xmin+i*dx)<(dados.centroX+dados.diamInt) && (dados.xmin+i*dx)>(dados.centroX-dados.diamInt) &&
				   (dados.ymin+j*dy)<(dados.centroY+dados.diamInt) && (dados.ymin+j*dy)>(dados.centroY-dados.diamInt)){
					malhaH[i][j].buraco=2;
				}
				else if((dados.xmin+i*dx)<(dados.centroX+dados.diamExt) &&
						(dados.xmin+i*dx)>(dados.centroX-dados.diamExt) &&
						(dados.ymin+j*dy)<(dados.centroY+dados.diamExt) &&
						(dados.ymin+j*dy)>(dados.centroY-dados.diamExt)){
					malhaH[i][j].buraco=3;
				}
			}
		}
	}

	//for(int ix=0;ix<nx;ix++){
		//for(int iy=0;iy<ny;iy++){

		//}
	//}
}*/

/*void solverP::imprimeMalhaRegular(int indTramo){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).lixo5);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.tempC+
					malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenT[0]+
					malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenT[1];
			}
			else if(malhaH[i][j].buraco==2){
				flut[i][j]=dados.tInt;
			}
			else if(malhaH[i][j].buraco==3){
				flut[i][j]=dados.tParede;
			}
			else flut[i][j]=0.;
		}
	}
	ostringstream saida;
	if(indTramo<0)
	saida << pathPrefixoArqSaida << "PerfisTempRegular2D-"<<indCel << "-" << numero
					<< ".dat";
	else
		saida << pathPrefixoArqSaida << "PerfisTempRegular2D-"<<indCel <<"-Tramo-"<<indTramo<< "-" << numero
						<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}*/

void solverP::permanentePoisson(){
    double norma=1e5;
    int itera=0;
    int indsra;
   // Vcr<double> termolivre(malha.nele);
    int diverge=0;
    double norma0=norma;
    while(norma>1e-5 && diverge==0){
    	for(int i=0; i<malha.nele;i++){
    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
    			malha.mlh2d[i].cel2D.gradGreenTI[j]=malha.mlh2d[i].cel2D.gradGreenT[j];
    		}
    	}
    	for(int i=0; i<malha.nele;i++){
    		malha.mlh2d[i].calcGradGreen();
    	}
    	for(int i=0; i<malha.nele;i++){
    		malha.mlh2d[i].GeraLocal();
    	}
    	for(int i=0;i<malha.nele;i++)
    	    		malha.mlh2d[i].cel2D.tempC0=malha.mlh2d[i].cel2D.tempCI=malha.mlh2d[i].cel2D.tempC;
    	indsra=0;
    	for(int i=0;i<malha.nele;i++){
    		int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
    		for(int j=0;j<dimNoZero;j++)
    			matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
    		indsra+=dimNoZero;
    		termolivre[i]=malha.mlh2d[i].TL[0];
    	}
    	double eps=0.001;
    	int iter=malha.nele;
    	int retorna;
    	for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.tempC;
    	retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
    	//if(retorna==1)break;
    	/////////////////////////////////////////////////////////////////////////////////////////////////
    	///////////////////////////////////////////////////////////////////////////////////////////////////

    	norma=0.;
    	for(int i=0;i<malha.nele;i++){
    		//malha.mlh2d[i].cel2D.tempC0=malha.mlh2d[i].cel2D.tempC;
    		malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
    		norma+=fabsl(malha.mlh2d[i].cel2D.tempC-malha.mlh2d[i].cel2D.tempC0);
    	}
    	norma/=malha.nele;
    	if(norma>=norma0)diverge=1;
    	else norma0=norma;

	    if(dados.acop==1){
	    	//hE=transfer.hExt();
	    	//dados.transfer.Tint=dados.tInt;
	    	//dados.transfer.kint=dados.fluc.CondLiq(dados.pInt, dados.tInt);
	    	//dados.transfer.cpint=dados.fluc.CalorLiq(dados.pInt, dados.tInt);
	    	//dados.transfer.rhoint=dados.fluc.MasEspFlu(dados.pInt, dados.tInt);
	    	//dados.transfer.viscint=dados.fluc.VisFlu(dados.pInt, dados.tInt);
	    	//dados.hI=dados.transfer.hInt();
	    	dados.qDesacop=((2*M_PI-dados.angAcop)*(dados.tInt-dados.tAmb))/(1./dados.condGlob+1./(0.5*dados.diamInt*dados.hI)+
	    			  1./(0.5*dados.diamExt*dados.hE));
	    	dados.qAcop=0.;
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
						int kcc=0;
						while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
						if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
							dados.qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(dados.tInt-malha.mlh2d[i].tempF[j])/(1/dados.condLoc+1./(dados.hI));
						}
		    		}
		    	}
			}
			dados.qTotal=dados.qAcop+dados.qDesacop;
			dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.diamInt*dados.hI);
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
						int kcc=0;
						while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
						if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
							malha.mlh2d[i].ccTambR[j]=dados.tParede;
						}
		    		}
		    	}
			}

	    }
	    itera++;
    }
}

void solverP::imprimePermanente(int indTramo){
    malha.imprime(0.,indCel,indTramo);
    //imprimeMalhaRegular(indTramo);
}

void solverP::inicializaPermanentePoisson(){
    for(int i=0;i<malha.nele;i++){
    	malha.mlh2d[i].perm=1;
    	malha.mlh2d[i].trans=0;
    }
	dados.tInt0=dados.tInt;
}

void solverP::inicializaTransientePoisson(){
    if(dados.temp.perm==1 && dados.temp.trans==1){
    	for(int i=0;i<malha.nele;i++){
    		malha.mlh2d[i].perm=0;
    		malha.mlh2d[i].trans=1;
    	}
    }

	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].perm=0;
		malha.mlh2d[i].trans=1;
	}
	kontaTempo=0;
	//cpi=dados.fluc.CalorLiq(dados.pInt, dados.tInt);
	//rhoi=dados.fluc.MasEspFlu(dados.pInt, dados.tInt);
	dados.tInt0=dados.tInt;
}

void solverP::transientePoissonDummy(double vdelt){
	double delt=vdelt;
	double norma;
	int itera;
	int indsra;

			norma=1e5;

			for(int i=0; i<malha.nele;i++)malha.mlh2d[i].dt=delt;
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
		    			malha.mlh2d[i].atualizaCC(j);
		    		}
		    	}
			}
			itera=0;
			while(norma>1.e-5){
				#pragma omp parallel for num_threads(dados.nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
		    			malha.mlh2d[i].cel2D.gradGreenTI[j]=malha.mlh2d[i].cel2D.gradGreenT[j];
		    		}
		    	}
				#pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcGradGreen();
				#pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocal();
				indsra=0;
				for(int i=0;i<malha.nele;i++){
					int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
					for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
					indsra+=dimNoZero;
					termolivre[i]=malha.mlh2d[i].TL[0];
				}
				double eps=0.00001;
				int iter=malha.nele;
				int retorna;
				for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.tempC;
				retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				//if(retorna==1)break;
				norma=0.;
				for(int i=0;i<malha.nele;i++){
					malha.mlh2d[i].cel2D.tempCI=malha.mlh2d[i].cel2D.tempC;
					malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
					norma+=fabsl(malha.mlh2d[i].cel2D.tempC-malha.mlh2d[i].cel2D.tempCI);
				}
				norma/=malha.nele;

			    if(dados.acop==1){
			    	//hE=transfer.hExt();
			    	//dados.transfer.Tint=dados.tInt=dados.tInt0-dados.qTotal*delt/(cpi*rhoi*dados.dutosMRT.area);
			    	//dados.transfer.kint=ki=dados.fluc.CondLiq(dados.pInt, dados.tInt);
			    	//dados.transfer.cpint=cpi=dados.fluc.CalorLiq(dados.pInt, dados.tInt);
			    	//dados.transfer.rhoint=rhoi=dados.fluc.MasEspFlu(dados.pInt, dados.tInt);
			    	//dados.transfer.viscint=visci=dados.fluc.VisFlu(dados.pInt, dados.tInt);
			    	//dados.transfer.Vint=0.;
			    	//dados.hI=dados.transfer.hInt();
			    	dados.qDesacop=((2*M_PI-dados.angAcop)*(dados.tInt-dados.tAmb))/(1./dados.condGlob+1./(0.5*dados.diamInt*dados.hI)+
			    			  1./(0.5*dados.diamExt*dados.hE));
			    	dados.qAcop=0.;
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(dados.tInt-malha.mlh2d[i].tempF[j])/
											(1/dados.condLoc+1./(dados.hI));
								}
				    		}
				    	}
					}
					dados.qTotal=dados.qAcop+dados.qDesacop;
					dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.diamInt*dados.hI);
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									malha.mlh2d[i].ccTambR[j]=dados.tParede;
								}
				    		}
				    	}
					}

			    }
			    itera++;

			}
			if(dados.acop==1)dados.tInt0=dados.tInt;
			for(int i=0;i<malha.nele;i++){
				malha.mlh2d[i].cel2D.tempC0=malha.mlh2d[i].cel2D.tempC;
				//malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
			}
}

double solverP::defineDeltPoisson(){
    int ind=0;
	double raz;
	double delt;

	indrazT(ind, raz);
	double inf = dados.temp.dtmax[ind];
	double sup;
	if (ind < dados.temp.parserie - 1){
		sup = dados.temp.dtmax[ind + 1];
		delt = (inf * raz + (1 - raz) * sup);
	}
	else
		delt = dados.temp.dtmax[ind];
	return delt;
}

void solverP::transientePoisson(double vdelt){
	double delt=vdelt;
	double norma;
	int itera;
	int indsra;
	//if(dados.temp.trans==1){
		//while(tempo<=dados.temp.tmax){
			norma=1e5;
			/*indrazT(ind, raz);
			double inf = dados.temp.dtmax[ind];
			double sup;
			if (ind < dados.temp.parserie - 1){
				sup = dados.temp.dtmax[ind + 1];
				delt = (inf * raz + (1 - raz) * sup);
			}
			else
				delt = dados.temp.dtmax[ind];*/
			for(int i=0; i<malha.nele;i++)malha.mlh2d[i].dt=delt;
			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
		    			malha.mlh2d[i].atualizaCC(j);
		    		}
		    	}
			}
			itera=0;
			while(norma>1.e-5){
                #pragma omp parallel for num_threads(dados.nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
		    			malha.mlh2d[i].cel2D.gradGreenTI[j]=malha.mlh2d[i].cel2D.gradGreenT[j];
		    		}
		    	}
				#pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].calcGradGreen();
				#pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocal();
				indsra=0;
				for(int i=0;i<malha.nele;i++){
					int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
					for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
					indsra+=dimNoZero;
					termolivre[i]=malha.mlh2d[i].TL[0];
				}
				double eps=0.00001;
				int iter=malha.nele;
				int retorna;
				for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.tempC;
				if(dados.solverMat==0)retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				else if(dados.solverMat==1)retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				else retorna=matglob.BiCGStab(malha.vecSolv,termolivre,eps,iter,precn);
				//if(retorna==1)break;
				norma=0.;
				for(int i=0;i<malha.nele;i++){
					malha.mlh2d[i].cel2D.tempCI=malha.mlh2d[i].cel2D.tempC;
					malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
					norma+=fabsl(malha.mlh2d[i].cel2D.tempC-malha.mlh2d[i].cel2D.tempCI);
				}
				norma/=malha.nele;

			    if(dados.acop==1){
			    	//hE=transfer.hExt();
			    	//dados.transfer.Tint=dados.tInt=dados.tInt0-dados.qTotal*delt/(cpi*rhoi*dados.dutosMRT.area);
			    	//dados.transfer.kint=ki=dados.fluc.CondLiq(dados.pInt, dados.tInt);
			    	//dados.transfer.cpint=cpi=dados.fluc.CalorLiq(dados.pInt, dados.tInt);
			    	//dados.transfer.rhoint=rhoi=dados.fluc.MasEspFlu(dados.pInt, dados.tInt);
			    	//dados.transfer.viscint=visci=dados.fluc.VisFlu(dados.pInt, dados.tInt);
			    	//dados.transfer.Vint=0.;
			    	//dados.hI=dados.transfer.hInt();
			    	dados.qDesacop=((2*M_PI-dados.angAcop)*(dados.tInt-dados.tAmb))/(1./dados.condGlob+1./(0.5*dados.diamInt*dados.hI)+
			    			  1./(0.5*dados.diamExt*dados.hE));
			    	dados.qAcop=0.;
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.qAcop+=malha.mlh2d[i].cel2D.sFaceMod[j]*(dados.tInt-malha.mlh2d[i].tempF[j])/
											(1/dados.condLoc+1./(dados.hI));
								}
				    		}
				    	}
					}
					dados.qTotal=dados.qAcop+dados.qDesacop;
					dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.diamInt*dados.hI);
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									malha.mlh2d[i].ccTambR[j]=dados.tParede;
								}
				    		}
				    	}
					}

			    }
			    itera++;

			}
		//}
	//}
}

void solverP::finalizaPassoTransiente(double vdelt,int indTramo){
	double delt=vdelt;
	if(dados.acop==1)dados.tInt0=dados.tInt;
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].cel2D.tempC0=malha.mlh2d[i].cel2D.tempC;
		//malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
	}

	//cout << "Tempo= " << lixo5<<" norma= "<< norma<<" iteracoes= "<<itera;
	//cout << endl;

	//tempo+=delt;

	if (((*vg1dSP).lixo5 <= dados.temp.tempoImp[kontaTempo] && (*vg1dSP).lixo5 + delt >= dados.temp.tempoImp[kontaTempo])) {
		malha.imprime((*vg1dSP).lixo5,indCel,indTramo);
		//imprimeMalhaRegular(indTramo);
		kontaTempo++;
		if (kontaTempo >= dados.temp.parserieImp) kontaTempo--;
	}
}

void solverP::FeiticoDoTempo(){
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].cel2D.tempC=malha.mlh2d[i].cel2D.tempC0;
	}

}
