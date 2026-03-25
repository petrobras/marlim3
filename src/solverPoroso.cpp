/*
 * solverPoroso.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "solverPoroso.h"

solverPoro::solverPoro():
		dados(),termolivre(1),matglob(1,1){
		malhaH=0;
		kontaTempo=0;
		sWPoc=0.;
		tipoTabelaDinamica=0;
		malha=malha2dPoro();

		ntabDin=0;
		reinicia=0;
		lAcopTot=0;
		dt=0;
		posicMarlim=0.;
		vg1dSP=0;

		precn=1;
		/*if(tipoTabelaDinamica==1){
			  int minNPontos=0;
			  if(dados.acop==1) ntabDin=2;
			  else  ntabDin=1;
			  for(int i=0;i<ntabDin;i++){
				  tabelaDinamica temp;
				  temp.npontosP=minNPontos;
				  temp.npontosT=minNPontos;
				  temp.celIni=0;
				  if(i==0 && dados.acop==1) temp.celFim=dados.transfer.ncel-1;
				  else temp.celFim=malha.nele-1;
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
		}*/
		//else tabDin=vector<tabelaDinamica>(0);
}


solverPoro::solverPoro(varGlob1D* Vvg1dSP,string nomeArquivoEntrada):
dados(Vvg1dSP,nomeArquivoEntrada),termolivre(dados.nele),matglob(dados.nele,dados.noZero,dados.vecsra,dados.veclm,dados.vecfnz){
	vg1dSP=Vvg1dSP;
	(*vg1dSP).tempo=0.;
    malha=malha2dPoro(vg1dSP,dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno,dados.temp.perm,dados.temp.trans,dados.flup,
    		dados.kRelOA,dados.kRelOG,dados.pcOA,dados.pcGO);
    for(int i=0;i<malha.nele;i++){
    	malha.mlh2d[i].cel2D.pBolha=dados.pBolha;
    	malha.mlh2d[i].cel2D.compOleo=dados.compOleo;
    	malha.mlh2d[i].cel2D.rhoRef=dados.rhoRef;
    	malha.mlh2d[i].cel2D.tipoModelOleo=dados.tipoModelOleo;
    	malha.mlh2d[i].CC=dados.CC;
		malha.mlh2d[i].fluc=dados.fluc;
		//malha.mlh2d[i].flup=dados.flup;
		//malha.mlh2d[i].kRelOACel=dados.kRelOA;
		//malha.mlh2d[i].kRelOGCel=dados.kRelOG;
		//malha.mlh2d[i].pcGOCel=dados.pcGO;
		//malha.mlh2d[i].pcOACel=dados.pcOA;
		malha.mlh2d[i].satConata=dados.satAconat;
		malha.mlh2d[i].pRes=dados.pRes;
		malha.mlh2d[i].tRes=dados.tRes;

		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
			malha.mlh2d[i].cel2D.gradGreenP[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenZdatum[j]=0;
			malha.mlh2d[i].cel2D.gradGreenZdatumI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenAZdatum[j]=0;
			malha.mlh2d[i].cel2D.gradGreenAZdatumI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPcOG[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPcAO[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPcAOI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenSw[j]=0;
			malha.mlh2d[i].cel2D.gradGreenSwI[j]=0;
		}

    	for(int j=0; j<dados.prop.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.prop.xmax[j] && xc>=dados.prop.xmin[j])&&(yc<=dados.prop.ymax[j] && yc>=dados.prop.ymin[j])){
    			malha.mlh2d[i].cel2D.kX=dados.prop.kX[j];
    			malha.mlh2d[i].cel2D.kY=dados.prop.kY[j];
    			malha.mlh2d[i].cel2D.kabsol=sqrtl(malha.mlh2d[i].cel2D.kX*malha.mlh2d[i].cel2D.kY);
    			malha.mlh2d[i].cel2D.poro=dados.prop.poro[j];
    			malha.mlh2d[i].cel2D.compRoc=dados.prop.compRoc[j];
    			break;
    		}
    	}
    	for(int j=0; j<dados.CI.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.CI.xmax[j] && xc>=dados.CI.xmin[j])&&(yc<=dados.CI.ymax[j] && yc>=dados.CI.ymin[j])){
    			malha.mlh2d[i].cel2D.presC=dados.CI.val1[j];
    			malha.mlh2d[i].cel2D.presC0=dados.CI.val1[j];
    			malha.mlh2d[i].cel2D.presCI=dados.CI.val1[j];
    			malha.mlh2d[i].cel2D.sWC=dados.CI.val2[j];
    			malha.mlh2d[i].cel2D.sWI=dados.CI.val2[j];
    			double dZdX=(dados.CI.Dxmax[j]-dados.CI.Dxmin[j])/(dados.CI.xmax[j]-dados.CI.xmin[j]);
    			double dZdY=(dados.CI.Dymax[j]-dados.CI.Dymin[j])/(dados.CI.ymax[j]-dados.CI.ymin[j]);
    			malha.mlh2d[i].cel2D.zD=dados.CI.Dxmin[j]+dZdX*(xc-dados.CI.xmin[j])+dZdY*(yc-dados.CI.ymin[j]);

    			malha.mlh2d[i].cel2D.vbo=malha.mlh2d[i].flup.BOFunc(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			malha.mlh2d[i].cel2D.rhostd=1000.*141.5/(131.5+malha.mlh2d[i].flup.API);
    			malha.mlh2d[i].cel2D.rhogstd=malha.mlh2d[i].flup.MasEspGas(1.033, 20.);
    			malha.mlh2d[i].cel2D.rhoastd=malha.mlh2d[i].flup.MasEspAgua(1.033, 20.);
    			//malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			if(malha.mlh2d[i].cel2D.tipoModelOleo==1)
    				malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			else
    				malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].cel2D.rhoRef*(1-malha.mlh2d[i].cel2D.compOleo*(dados.pRes-malha.mlh2d[i].cel2D.presC));
    			malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			double tit;
    			if(malha.mlh2d[i].cel2D.tipoModelOleo==1){
    				tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    				malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
    			}
    			else malha.mlh2d[i].cel2D.alfC=0.;
    			for(int k=0; k<malha.mlh2d[i].cel2D.nvert;k++){
    				malha.mlh2d[i].presF[j]=malha.mlh2d[i].cel2D.presC;
    				malha.mlh2d[i].rhoF[k]=malha.mlh2d[i].cel2D.rhoP;
    				malha.mlh2d[i].rhogF[k]=malha.mlh2d[i].cel2D.rhogP;
    				malha.mlh2d[i].rhoaF[k]=malha.mlh2d[i].cel2D.rhoaP;
					//tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					//malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
    				if(malha.mlh2d[i].cel2D.tipoModelOleo==1){
    					tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
    					malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
    				}
    				else malha.mlh2d[i].alfF[j]=0.;
					//malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					//if(malha.mlh2d[i].presF[j]<malha.mlh2d[i].cel2D.pBolha-0.01)
						malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					//else malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].cel2D.pBolha-0.01, malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].migF[j]=malha.mlh2d[i].flup.ViscGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].miaF[j]=malha.mlh2d[i].flup.VisAgua(malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].rhoF[j]=(1.-malha.mlh2d[i].alfF[j])*malha.mlh2d[i].rhoF[j]+malha.mlh2d[i].alfF[j]*malha.mlh2d[i].rhogF[j];
    			}

    			break;
    		}
    	}
    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
    		if(malha.mlh2d[i].kvizinho[j]<0){
    			malha.mlh2d[i].atualizaCC(j);
    		}
    		malha.mlh2d[i].Qtotal[j]=0;
    		malha.mlh2d[i].Qw[j]=0;
    		malha.mlh2d[i].Qoleo[j]=0;
    		malha.mlh2d[i].Qgas[j]=0;
    	}
    }

	for(int i=0; i<malha.nele;i++)
		malha.mlh2d[i].calSupDifu();

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
    	dados.hE=0;
    	dados.hI=0;
    	dados.condGlob=0;
    	dados.condLoc=0;
    	//tParede=
    	dados.qDesacop=0.;
    	dados.qAcop=0.;
		/*for(int i=0;i<malha.nele;i++){
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
		dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.dutosMRT.a*dados.hI);*/
		/*for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						malha.mlh2d[i].ccTambR[j]=dados.tParede;
					}
	    		}
	    	}
		}*/

    }
    malhaRegular( dados.dxH, dados.dyH);

	kontaTempo=0;

	sWPoc=dados.satAPoc[0];

	ntabDin=0;
	reinicia=0;
	lAcopTot=0;
	dt=0;
	posicMarlim=0.;
	if(tipoTabelaDinamica==1){
		  int minNPontos=0;
		  if(dados.acop==1) ntabDin=2;
		  else  ntabDin=1;
		  for(int i=0;i<ntabDin;i++){
			  tabelaDinamica temp;
			  temp.npontosP=minNPontos;
			  temp.npontosT=minNPontos;
			  temp.celIni=0;
			  if(i==0 && dados.acop==1) temp.celFim=dados.transfer.ncel-1;
			  else temp.celFim=malha.nele-1;
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
	}

}


solverPoro::solverPoro(const solverPoro& vPoroso):
		dados(vPoroso.vg1dSP,vPoroso.dados.entrada),termolivre(dados.nele),matglob(dados.nele,dados.noZero,dados.vecsra,dados.veclm,dados.vecfnz){
	vg1dSP=vPoroso.vg1dSP;
	(*vg1dSP).tempo=0.;
    malha=malha2dPoro(vg1dSP,dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno,dados.temp.perm,dados.temp.trans,dados.flup,
    		dados.kRelOA,dados.kRelOG,dados.pcOA,dados.pcGO);
    for(int i=0;i<malha.nele;i++){

       	malha.mlh2d[i].cel2D.pBolha=dados.pBolha;
        malha.mlh2d[i].cel2D.compOleo=dados.compOleo;
        malha.mlh2d[i].cel2D.rhoRef=dados.rhoRef;
        malha.mlh2d[i].cel2D.tipoModelOleo=dados.tipoModelOleo;
    	malha.mlh2d[i].CC=dados.CC;
		malha.mlh2d[i].fluc=dados.fluc;
		//malha.mlh2d[i].flup=dados.flup;
		//malha.mlh2d[i].kRelOACel=dados.kRelOA;
		//malha.mlh2d[i].kRelOGCel=dados.kRelOG;
		//malha.mlh2d[i].pcGOCel=dados.pcGO;
		//malha.mlh2d[i].pcOACel=dados.pcOA;
		malha.mlh2d[i].satConata=dados.satAconat;
		malha.mlh2d[i].pRes=dados.pRes;
		malha.mlh2d[i].tRes=dados.tRes;
		malha.mlh2d[i].dh=dados.espessuraRes;

		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
			malha.mlh2d[i].cel2D.gradGreenP[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenZdatum[j]=0;
			malha.mlh2d[i].cel2D.gradGreenZdatumI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenAZdatum[j]=0;
			malha.mlh2d[i].cel2D.gradGreenAZdatumI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPcOG[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPcAO[j]=0;
			malha.mlh2d[i].cel2D.gradGreenPcAOI[j]=0;
			malha.mlh2d[i].cel2D.gradGreenSw[j]=0;
			malha.mlh2d[i].cel2D.gradGreenSwI[j]=0;
		}

    	for(int j=0; j<dados.prop.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.prop.xmax[j] && xc>=dados.prop.xmin[j])&&(yc<=dados.prop.ymax[j] && yc>=dados.prop.ymin[j])){
    			malha.mlh2d[i].cel2D.kX=dados.prop.kX[j];
    			malha.mlh2d[i].cel2D.kY=dados.prop.kY[j];
    			malha.mlh2d[i].cel2D.kabsol=sqrtl(malha.mlh2d[i].cel2D.kX*malha.mlh2d[i].cel2D.kY);
    			malha.mlh2d[i].cel2D.poro=dados.prop.poro[j];
    			malha.mlh2d[i].cel2D.compRoc=dados.prop.compRoc[j];
    			break;
    		}
    	}
    	for(int j=0; j<dados.CI.parserie;j++){
    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
    		if((xc<=dados.CI.xmax[j] && xc>=dados.CI.xmin[j])&&(yc<=dados.CI.ymax[j] && yc>=dados.CI.ymin[j])){
    			malha.mlh2d[i].cel2D.presC=dados.CI.val1[j];
    			malha.mlh2d[i].cel2D.presC0=dados.CI.val1[j];
    			malha.mlh2d[i].cel2D.sWC=dados.CI.val2[j];
    			malha.mlh2d[i].cel2D.sWI=dados.CI.val2[j];
    			double dZdX=(dados.CI.Dxmax[j]-dados.CI.Dxmin[j])/(dados.CI.xmax[j]-dados.CI.xmin[j]);
    			double dZdY=(dados.CI.Dymax[j]-dados.CI.Dymin[j])/(dados.CI.ymax[j]-dados.CI.ymin[j]);
    			malha.mlh2d[i].cel2D.zD=dados.CI.Dxmin[j]+dZdX*(xc-dados.CI.xmin[j])+dZdY*(yc-dados.CI.ymin[j]);

    			malha.mlh2d[i].cel2D.vbo=malha.mlh2d[i].flup.BOFunc(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			malha.mlh2d[i].cel2D.rhostd=1000.*141.5/(131.5+malha.mlh2d[i].flup.API);
    			malha.mlh2d[i].cel2D.rhogstd=malha.mlh2d[i].flup.MasEspGas(1.033, 20.);
    			malha.mlh2d[i].cel2D.rhoastd=malha.mlh2d[i].flup.MasEspAgua(1.033, 20.);
    			//malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			if(malha.mlh2d[i].cel2D.tipoModelOleo==1)
    				malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			else
    				malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].cel2D.rhoRef*(1-malha.mlh2d[i].cel2D.compOleo*(dados.pRes-malha.mlh2d[i].cel2D.presC));
    			malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    			double tit;
    			if(malha.mlh2d[i].cel2D.tipoModelOleo==1){
    				tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
    				malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
    			}
    			else malha.mlh2d[i].cel2D.alfC=0.;
    			for(int k=0; k<malha.mlh2d[i].cel2D.nvert;k++){
    				malha.mlh2d[i].presF[j]=malha.mlh2d[i].cel2D.presC;
    				malha.mlh2d[i].rhoF[k]=malha.mlh2d[i].cel2D.rhoP;
    				malha.mlh2d[i].rhogF[k]=malha.mlh2d[i].cel2D.rhogP;
    				malha.mlh2d[i].rhoaF[k]=malha.mlh2d[i].cel2D.rhoaP;
					//tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					//malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
    				if(malha.mlh2d[i].cel2D.tipoModelOleo==1){
    					tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
    					malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
    				}
    				else malha.mlh2d[i].alfF[j]=0.;
					//malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					//if(malha.mlh2d[i].presF[j]<malha.mlh2d[i].cel2D.pBolha-0.01)
						malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					//else malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].cel2D.pBolha-0.01, malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].migF[j]=malha.mlh2d[i].flup.ViscGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].miaF[j]=malha.mlh2d[i].flup.VisAgua(malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].rhoF[j]=(1.-malha.mlh2d[i].alfF[j])*malha.mlh2d[i].rhoF[j]+malha.mlh2d[i].alfF[j]*malha.mlh2d[i].rhogF[j];
    			}
    			break;
    		}
    	}
    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
    		if(malha.mlh2d[i].kvizinho[j]<0){
    			malha.mlh2d[i].atualizaCC(j);
    		}
    		malha.mlh2d[i].Qtotal[j]=0;
    		malha.mlh2d[i].Qw[j]=0;
    		malha.mlh2d[i].Qoleo[j]=0;
    		malha.mlh2d[i].Qgas[j]=0;
    		malha.mlh2d[i].presF[j]=malha.mlh2d[i].cel2D.presC;
    		malha.mlh2d[i].swF[j]=malha.mlh2d[i].cel2D.sWC;
    	}
    }

	for(int i=0; i<malha.nele;i++)
		malha.mlh2d[i].calSupDifu();

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
    	dados.hE=0.;
    	dados.hI=0.;
    	dados.condGlob=0.;
    	dados.condLoc=0.;
    	//tParede=
    	dados.qDesacop=0.;
    	dados.qAcop=0.;
    	/*for(int i=0; i<dados.CC.nRic; i++){
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
		dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.dutosMRT.a*dados.hI);
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
		}*/

    }
    malhaRegular( dados.dxH, dados.dyH);

	kontaTempo=vPoroso.kontaTempo;
	sWPoc=vPoroso.sWPoc;

	ntabDin=0;
	reinicia=0;
	lAcopTot=0;
	dt=0.;
	posicMarlim=vPoroso.posicMarlim;
	if(tipoTabelaDinamica==1){
		  int minNPontos=0;
		  if(dados.acop==1) ntabDin=2;
		  else  ntabDin=1;
		  for(int i=0;i<ntabDin;i++){
			  tabelaDinamica temp;
			  temp.npontosP=minNPontos;
			  temp.npontosT=minNPontos;
			  temp.celIni=0;
			  if(i==0 && dados.acop==1) temp.celFim=dados.transfer.ncel-1;
			  else temp.celFim=malha.nele-1;
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
	}
}

solverPoro& solverPoro::operator =(const solverPoro& vPoroso) {
	if (this != &vPoroso && vPoroso.malha.nele>0) {
		if(dados.nxMH>0){
			for(int i=0;i<dados.nxMH;i++) delete[] malhaH[i];
			delete[] malhaH;
			dados.nxMH=0;
		}
		vg1dSP=vPoroso.vg1dSP;
		dados=vPoroso.dados;
		termolivre=vPoroso.termolivre;
		matglob=vPoroso.matglob;

		(*vg1dSP).tempo=0.;
	    malha=malha2dPoro(vg1dSP,dados.xcoor, dados.noEle, dados.tipo, dados.atributo, 3, dados.nele, dados.nno,dados.temp.perm,dados.temp.trans,dados.flup,
	    		dados.kRelOA,dados.kRelOG,dados.pcOA,dados.pcGO);
	    for(int i=0;i<malha.nele;i++){
	       	malha.mlh2d[i].cel2D.pBolha=dados.pBolha;
	        malha.mlh2d[i].cel2D.compOleo=dados.compOleo;
	        malha.mlh2d[i].cel2D.rhoRef=dados.rhoRef;
	        malha.mlh2d[i].cel2D.tipoModelOleo=dados.tipoModelOleo;
	    	malha.mlh2d[i].CC=dados.CC;
			malha.mlh2d[i].fluc=dados.fluc;
			//malha.mlh2d[i].flup=dados.flup;
			//malha.mlh2d[i].kRelOACel=dados.kRelOA;
			//malha.mlh2d[i].kRelOGCel=dados.kRelOG;
			//malha.mlh2d[i].pcGOCel=dados.pcGO;
			//malha.mlh2d[i].pcOACel=dados.pcOA;
			malha.mlh2d[i].satConata=dados.satAconat;
			malha.mlh2d[i].pRes=dados.pRes;
			malha.mlh2d[i].tRes=dados.tRes;

			for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
				malha.mlh2d[i].cel2D.gradGreenP[j]=0;
				malha.mlh2d[i].cel2D.gradGreenPI[j]=0;
				malha.mlh2d[i].cel2D.gradGreenZdatum[j]=0;
				malha.mlh2d[i].cel2D.gradGreenZdatumI[j]=0;
				malha.mlh2d[i].cel2D.gradGreenAZdatum[j]=0;
				malha.mlh2d[i].cel2D.gradGreenAZdatumI[j]=0;
				malha.mlh2d[i].cel2D.gradGreenPcOG[j]=0;
				malha.mlh2d[i].cel2D.gradGreenPcAO[j]=0;
				malha.mlh2d[i].cel2D.gradGreenPcAOI[j]=0;
				malha.mlh2d[i].cel2D.gradGreenSw[j]=0;
				malha.mlh2d[i].cel2D.gradGreenSwI[j]=0;
			}

	    	for(int j=0; j<dados.prop.parserie;j++){
	    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
	    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
	    		if((xc<=dados.prop.xmax[j] && xc>=dados.prop.xmin[j])&&(yc<=dados.prop.ymax[j] && yc>=dados.prop.ymin[j])){
	    			malha.mlh2d[i].cel2D.kX=dados.prop.kX[j];
	    			malha.mlh2d[i].cel2D.kY=dados.prop.kY[j];
	    			malha.mlh2d[i].cel2D.kabsol=sqrtl(malha.mlh2d[i].cel2D.kX*malha.mlh2d[i].cel2D.kY);
	    			malha.mlh2d[i].cel2D.poro=dados.prop.poro[j];
	    			malha.mlh2d[i].cel2D.compRoc=dados.prop.compRoc[j];
	    			break;
	    		}
	    	}
	    	for(int j=0; j<dados.CI.parserie;j++){
	    		double xc=malha.mlh2d[i].cel2D.centroideElem[0];
	    		double yc=malha.mlh2d[i].cel2D.centroideElem[1];
	    		if((xc<=dados.CI.xmax[j] && xc>=dados.CI.xmin[j])&&(yc<=dados.CI.ymax[j] && yc>=dados.CI.ymin[j])){
	    			malha.mlh2d[i].cel2D.presC=dados.CI.val1[j];
	    			malha.mlh2d[i].cel2D.presC0=dados.CI.val1[j];
	    			malha.mlh2d[i].cel2D.sWC=dados.CI.val2[j];
	    			malha.mlh2d[i].cel2D.sWI=dados.CI.val2[j];
	    			double dZdX=(dados.CI.Dxmax[j]-dados.CI.Dxmin[j])/(dados.CI.xmax[j]-dados.CI.xmin[j]);
	    			double dZdY=(dados.CI.Dymax[j]-dados.CI.Dymin[j])/(dados.CI.ymax[j]-dados.CI.ymin[j]);
	    			malha.mlh2d[i].cel2D.zD=dados.CI.Dxmin[j]+dZdX*(xc-dados.CI.xmin[j])+dZdY*(yc-dados.CI.ymin[j]);

	    			malha.mlh2d[i].cel2D.vbo=malha.mlh2d[i].flup.BOFunc(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    			malha.mlh2d[i].cel2D.rhostd=1000.*141.5/(131.5+malha.mlh2d[i].flup.API);
	    			malha.mlh2d[i].cel2D.rhogstd=malha.mlh2d[i].flup.MasEspGas(1.033, 20.);
	    			malha.mlh2d[i].cel2D.rhoastd=malha.mlh2d[i].flup.MasEspAgua(1.033, 20.);
	    			//malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    			if(malha.mlh2d[i].cel2D.tipoModelOleo==1)
	    				malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    			else
	    				malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].cel2D.rhoRef*(1-malha.mlh2d[i].cel2D.compOleo*(dados.pRes-malha.mlh2d[i].cel2D.presC));
	    			malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    			malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    			//double tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    			//malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
	    			double tit;
	    			if(malha.mlh2d[i].cel2D.tipoModelOleo==1){
	    				tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
	    				malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
	    			}
	    			else malha.mlh2d[i].cel2D.alfC=0.;
	    			for(int k=0; k<malha.mlh2d[i].cel2D.nvert;k++){
	    				malha.mlh2d[i].presF[j]=malha.mlh2d[i].cel2D.presC;
	    				malha.mlh2d[i].rhoF[k]=malha.mlh2d[i].cel2D.rhoP;
	    				malha.mlh2d[i].rhogF[k]=malha.mlh2d[i].cel2D.rhogP;
	    				malha.mlh2d[i].rhoaF[k]=malha.mlh2d[i].cel2D.rhoaP;
	    				if(malha.mlh2d[i].cel2D.tipoModelOleo==1){
	    					tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
	    					malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
	    				}
	    				else malha.mlh2d[i].alfF[j]=0.;
						//malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
						//if(malha.mlh2d[i].presF[j]<malha.mlh2d[i].cel2D.pBolha-0.01)
							malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
						//else malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].cel2D.pBolha-0.01, malha.mlh2d[i].tRes)/1000.;
						malha.mlh2d[i].migF[j]=malha.mlh2d[i].flup.ViscGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
						malha.mlh2d[i].miaF[j]=malha.mlh2d[i].flup.VisAgua(malha.mlh2d[i].tRes)/1000.;
						malha.mlh2d[i].rhoF[j]=(1.-malha.mlh2d[i].alfF[j])*malha.mlh2d[i].rhoF[j]+malha.mlh2d[i].alfF[j]*malha.mlh2d[i].rhogF[j];
	    			}

	    			break;
	    		}
	    	}
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
	    			malha.mlh2d[i].atualizaCC(j);
	    		}
	    		malha.mlh2d[i].Qtotal[j]=0;
	    		malha.mlh2d[i].Qw[j]=0;
	    		malha.mlh2d[i].Qoleo[j]=0;
	    		malha.mlh2d[i].Qgas[j]=0;
	    	}
	    }

		for(int i=0; i<malha.nele;i++)
			malha.mlh2d[i].calSupDifu();

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
	    	dados.hE=0.;
	    	dados.hI=0.;
	    	dados.condGlob=0.;
	    	dados.condLoc=0.;
	    	//tParede=
	    	dados.qDesacop=0.;
	    	dados.qAcop=0.;
	    	/*for(int i=0; i<dados.CC.nRic; i++){
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
			dados.tParede=dados.tInt-dados.qTotal/(M_PI*dados.dutosMRT.a*dados.hI);
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
			}*/

	    }
	    malhaRegular( dados.dxH, dados.dyH);


		kontaTempo=vPoroso.kontaTempo;
		sWPoc=vPoroso.sWPoc;
		posicMarlim=vPoroso.posicMarlim;

		ntabDin=0;
		reinicia=0;
		lAcopTot=0;
		dt=0.;
		if(tipoTabelaDinamica==1){
			  int minNPontos=0;
			  if(dados.acop==1) ntabDin=2;
			  else  ntabDin=1;
			  for(int i=0;i<ntabDin;i++){
				  tabelaDinamica temp;
				  temp.npontosP=minNPontos;
				  temp.npontosT=minNPontos;
				  temp.celIni=0;
				  if(i==0 && dados.acop==1) temp.celFim=dados.transfer.ncel-1;
				  else temp.celFim=malha.nele-1;
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
		}
	}

	return *this;
}

void solverPoro::indrazPW(int& ind, double& raz) {
	for (int i = 0; i <= dados.pW.nserie - 1; i++) {
		if (i < dados.pW.nserie - 1) {
			if ((*vg1dSP).tempo >= dados.pW.tempo[i] && (*vg1dSP).tempo < dados.pW.tempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).tempo - dados.pW.tempo[i])
								/ (dados.pW.tempo[i + 1] - dados.pW.tempo[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == dados.pW.nserie - 1) {
			ind = i;
			raz = 1;
		}
	}
}

void solverPoro::indrazT(int& ind, double& raz) {

	for (int i = 0; i <= dados.temp.parserie - 1; i++) {
		if (i < dados.temp.parserie - 1) {
			if ((*vg1dSP).tempo >= dados.temp.tempo[i] && (*vg1dSP).tempo < dados.temp.tempo[i + 1]) {
				ind = i;
				raz = 1
						- ((*vg1dSP).tempo - dados.temp.tempo[i])
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

void solverPoro::indraz(int& ind, double& raz, double* serieTempo, int nserie) {
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

void solverPoro::malhaRegular(double& dx, double& dy){
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
	dados.nxMH=floor((dados.xmax-dados.xmin)/dx);
	dados.nyMH=floor((dados.ymax-dados.ymin)/dy);
	malhaH=new detMalhaHomPoroso* [dados.nxMH];
	for(int i=0; i<dados.nxMH;i++)malhaH[i]=new detMalhaHomPoroso [dados.nyMH];
	for(int i=0; i<dados.nxMH;i++)
		for(int j=0; j<dados.nyMH;j++)malhaH[i][j].buraco=1;
	dx=(dados.xmax-dados.xmin)/dados.nxMH;
	dy=(dados.ymax-dados.ymin)/dados.nyMH;
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
	if(dados.acop==1){
	for(int i=0; i<dados.nxMH;i++){
		for(int j=0; j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==1){
				if((dados.xmin+i*dx)<(dados.centroX+dados.dutosMRT.a) && (dados.xmin+i*dx)>(dados.centroX-dados.dutosMRT.a) &&
				   (dados.ymin+j*dy)<(dados.centroY+dados.dutosMRT.a) && (dados.ymin+j*dy)>(dados.centroY-dados.dutosMRT.a)){
					malhaH[i][j].buraco=2;
				}
				else if((dados.xmin+i*dx)<(dados.centroX+dados.dutosMRT.diamC[dados.dutosMRT.ncamadas-1]) &&
						(dados.xmin+i*dx)>(dados.centroX-dados.dutosMRT.diamC[dados.dutosMRT.ncamadas-1]) &&
						(dados.ymin+j*dy)<(dados.centroY+dados.dutosMRT.diamC[dados.dutosMRT.ncamadas-1]) &&
						(dados.ymin+j*dy)>(dados.centroY-dados.dutosMRT.diamC[dados.dutosMRT.ncamadas-1])){
					malhaH[i][j].buraco=3;
					malhaH[i][j].delx=dx;
					malhaH[i][j].dely=dy;
					malhaH[i][j].xcoor=dados.xmin+i*dx;
					malhaH[i][j].ycoor=dados.ymin+j*dy;
					double raio=sqrtl((malhaH[i][j].xcoor-dados.centroX)*(malhaH[i][j].xcoor-dados.centroX)+
							(malhaH[i][j].ycoor-dados.centroY)*(malhaH[i][j].ycoor-dados.centroY));
					malhaH[i][j].indEle=0;
					while(raio>(dados.transfer.celula[malhaH[i][j].indEle].r1+1e-5))malhaH[i][j].indEle++;
				}
			}
		}
	}
	}

	//for(int ix=0;ix<nx;ix++){
		//for(int iy=0;iy<ny;iy++){

		//}
	//}
}

void solverPoro::imprimeMalhaRegular(double minP){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).tempo);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.presC+
					0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenP[0]+
					0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenP[1];
				//if(flut[i][j]<200)flut[i][j]=200.;
			}
			else if(dados.acop==1){
				if(malhaH[i][j].buraco==2){
					flut[i][j]=dados.transfer.Pint;
				}
				else if(malhaH[i][j].buraco==3){
					int ncel=dados.transfer.ncel;
					flut[i][j]=dados.transfer.celula[malhaH[i][j].indEle].Pcamada;
				}
				else flut[i][j]=minP;
			}
			else flut[i][j]=minP;
		}
	}
	ostringstream saida;
	saida << pathPrefixoArqSaida << "PerfisPresRegular-Poroso2D" << "-" << numero<<"-posic-"<<posicMarlim
					<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}

void solverPoro::imprimeMalhaRegularSW(){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).tempo);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.sWC+
					0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenSw[0]+
					0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenSw[1];
			}
			else if(dados.acop==1){
				if(malhaH[i][j].buraco==2){
					flut[i][j]=dados.transfer.celula[0].sWR;
				}
				else if(malhaH[i][j].buraco==3){
					int ncel=dados.transfer.ncel;
					flut[i][j]=dados.transfer.celula[malhaH[i][j].indEle].sWL;
				}
			}
			else flut[i][j]=0.;
		}
	}
	ostringstream saida;
	saida << pathPrefixoArqSaida << "PerfisSWRegular-Poroso2D" << "-" << numero<<"-posic-"<<posicMarlim
					<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}

void solverPoro::imprimeMalhaRegularAlf(){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).tempo);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.alfC;
			}
			else if(dados.acop==1){
				if(malhaH[i][j].buraco==2){
					flut[i][j]=dados.transfer.celula[0].alf;
				}
				else if(malhaH[i][j].buraco==3){
					int ncel=dados.transfer.ncel;
					flut[i][j]=dados.transfer.celula[malhaH[i][j].indEle].alf;
				}
			}
			else flut[i][j]=0.;
		}
	}
	ostringstream saida;
	saida << pathPrefixoArqSaida << "PerfisAlfRegular-Poroso2D" << "-" << numero<<"-posic-"<<posicMarlim
					<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}

void solverPoro::imprimeMalhaRegularAux(double minP){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).tempo);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.presC+
					0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenP[0]+
					0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenP[1];
				//if(flut[i][j]<200)flut[i][j]=200.;
			}
			else if(dados.acop==1){
				if(malhaH[i][j].buraco==2){
					flut[i][j]=dados.transfer.Pint;
				}
				else if(malhaH[i][j].buraco==3){
					int ncel=dados.transfer.ncel;
					flut[i][j]=dados.transfer.celula[malhaH[i][j].indEle].Pcamada;
				}
				else flut[i][j]=minP;
			}
			else flut[i][j]=minP;
		}
	}
	ostringstream saida;
	saida << pathPrefixoArqSaida << "PerfisPresRegular-Poroso2D" << "-" << "Aux"<<"-posic-"<<posicMarlim
					<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}

void solverPoro::imprimeMalhaRegularSWAux(){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).tempo);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.sWC+
					0*malhaH[i][j].delx*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenSw[0]+
					0*malhaH[i][j].dely*malha.mlh2d[malhaH[i][j].indEle].cel2D.gradGreenSw[1];
			}
			else if(dados.acop==1){
				if(malhaH[i][j].buraco==2){
					if(malhaH[i][j].indEle==0 && dados.transfer.celula[1].QcamadaR>=0)
						flut[i][j]=dados.transfer.celula[1].sW;
					else flut[i][j]=dados.transfer.celula[0].sW;
				}
				else if(malhaH[i][j].buraco==3){
					int ncel=dados.transfer.ncel;
					if(malhaH[i][j].indEle==ncel-1 && dados.transfer.celula[ncel-2].QcamadaL<=0)flut[i][j]=dados.transfer.celula[ncel-2].sWL;
					else{
						if(malhaH[i][j].indEle==0 && dados.transfer.celula[1].QcamadaR>=0)
							flut[i][j]=dados.transfer.celula[1].sW;
						else flut[i][j]=dados.transfer.celula[malhaH[i][j].indEle].sWL;
					}
				}
			}
			else flut[i][j]=0.;
		}
	}
	ostringstream saida;
	saida << pathPrefixoArqSaida << "PerfisSWRegular-Poroso2D" << "-" << "Aux"<<"-posic-"<<posicMarlim
					<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}

void solverPoro::imprimeMalhaRegularAlfAux(){
	FullMtx<double> flut(dados.nxMH,dados.nyMH);
	int numero = roundl((*vg1dSP).tempo);
	for(int i=0;i<dados.nxMH;i++){
		for(int j=0;j<dados.nyMH;j++){
			if(malhaH[i][j].buraco==0){
				flut[i][j]=malha.mlh2d[malhaH[i][j].indEle].cel2D.alfC;
			}
			else if(dados.acop==1){
				if(malhaH[i][j].buraco==2){
					flut[i][j]=dados.transfer.celula[0].alf;
				}
				else if(malhaH[i][j].buraco==3){
					int ncel=dados.transfer.ncel;
					flut[i][j]=dados.transfer.celula[malhaH[i][j].indEle].alf;
				}
			}
			else flut[i][j]=0.;
		}
	}
	ostringstream saida;
	saida << pathPrefixoArqSaida << "PerfisAlfRegular-Poroso2D" << "-" << "Aux"<<"-posic-"<<posicMarlim
					<< ".dat";
	string tmp = saida.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << flut;
	escreveIni.close();
}

void solverPoro::atualizaCel2D(int i){
	for(int j=0; j<malha.mlh2d[i].flup.npseudo; j++)
		malha.mlh2d[i].flup.fracMol[j]=malha.mlh2d[i-1].flup.fracMol[j];
		malha.mlh2d[i].flup.iCalculatedStockTankThermodynamicCondition=
				malha.mlh2d[i-1].flup.iCalculatedStockTankThermodynamicCondition;
		malha.mlh2d[i].flup.dStockTankVaporMassFraction=
				malha.mlh2d[i-1].flup.dStockTankVaporMassFraction;
		malha.mlh2d[i].flup.dStockTankLiquidDensity=
				malha.mlh2d[i-1].flup.dStockTankLiquidDensity;
		malha.mlh2d[i].flup.dStockTankVaporDensity=
				malha.mlh2d[i-1].flup.dStockTankVaporDensity;

	if(malha.mlh2d[i].flup.dStockTankLiquidDensity>0.01){
		malha.mlh2d[i].flup.API=141.5/(malha.mlh2d[i].flup.dStockTankLiquidDensity/1000.)-131.5;
	}
	else malha.mlh2d[i].flup.API=50;
	malha.mlh2d[i].flup.Deng=malha.mlh2d[i].flup.dStockTankVaporDensity/1.225;
	malha.mlh2d[i].flup.RGO=malha.mlh2d[i-1].flup.RGO;
	malha.mlh2d[i].flup.IRGO=malha.mlh2d[i-1].flup.IRGO;

	double tmed = dados.tRes;
	double pmed =malha.mlh2d[i].cel2D.presC;

	int veriI=i-1;
	while((malha.mlh2d[veriI].flup.dCalculatedBeta>1-(0.0+1e-15) ||
			malha.mlh2d[veriI].flup.dCalculatedBeta<(0.0+1e-15))
			&& (veriI>0 ))veriI--;
	if((veriI==0) && (malha.mlh2d[veriI].flup.dCalculatedBeta>1-(0.0+1e-15)
			|| malha.mlh2d[veriI].flup.dCalculatedBeta<(0.0+1e-15)))veriI=i-1;
		malha.mlh2d[i].flup.atualizaPropComp(pmed, tmed,malha.mlh2d[veriI].flup.dCalculatedBeta,
				malha.mlh2d[veriI].flup.oCalculatedLiqComposition,
				malha.mlh2d[veriI].flup.oCalculatedVapComposition,0);
}


void solverPoro::atualizaCelTransfer(int i){
	for(int j=0; j<dados.transfer.celula[i].flup.npseudo; j++)
		dados.transfer.celula[i].flup.fracMol[j]=dados.transfer.celula[i-1].flup.fracMol[j];
	dados.transfer.celula[i].flup.iCalculatedStockTankThermodynamicCondition=
			dados.transfer.celula[i-1].flup.iCalculatedStockTankThermodynamicCondition;
	dados.transfer.celula[i].flup.dStockTankVaporMassFraction=
			dados.transfer.celula[i-1].flup.dStockTankVaporMassFraction;
	dados.transfer.celula[i].flup.dStockTankLiquidDensity=
			dados.transfer.celula[i-1].flup.dStockTankLiquidDensity;
	dados.transfer.celula[i].flup.dStockTankVaporDensity=
			dados.transfer.celula[i-1].flup.dStockTankVaporDensity;

	if(dados.transfer.celula[i].flup.dStockTankLiquidDensity>0.01){
		dados.transfer.celula[i].flup.API=141.5/(dados.transfer.celula[i].flup.dStockTankLiquidDensity/1000.)-131.5;
	}
	else dados.transfer.celula[i].flup.API=50;
	dados.transfer.celula[i].flup.Deng=dados.transfer.celula[i].flup.dStockTankVaporDensity/1.225;
	dados.transfer.celula[i].flup.RGO=dados.transfer.celula[i-1].flup.RGO;
	dados.transfer.celula[i].flup.IRGO=dados.transfer.celula[i-1].flup.IRGO;

	double tmed = dados.tRes;
	double pmed = dados.transfer.celula[i].pmed1;

	int veriI=i-1;
	while((dados.transfer.celula[veriI].flup.dCalculatedBeta>1-(0.0+1e-15) ||
			dados.transfer.celula[veriI].flup.dCalculatedBeta<(0.0+1e-15))
			&& (veriI>0 ))veriI--;
	if((veriI==0) && (dados.transfer.celula[veriI].flup.dCalculatedBeta>1-(0.0+1e-15)
			|| dados.transfer.celula[veriI].flup.dCalculatedBeta<(0.0+1e-15)))veriI=i-1;
	dados.transfer.celula[i].flup.atualizaPropComp(pmed, tmed,dados.transfer.celula[veriI].flup.dCalculatedBeta,
			dados.transfer.celula[veriI].flup.oCalculatedLiqComposition,
			dados.transfer.celula[veriI].flup.oCalculatedVapComposition,0);
}

void solverPoro::preparaTabDin(){
	for(int i=0;i<ntabDin;i++){
		int i0=tabDin[i].celIni;
		int i1=tabDin[i].celFim;
		if(i<ntabDin-1){
			tabDin[i].pmax=dados.transfer.celula[i0].Pcamada;
			tabDin[i].pmin=dados.transfer.celula[i0].Pcamada;
			tabDin[i].tmax=dados.transfer.celula[i0].tRes;
			tabDin[i].tmin=dados.transfer.celula[i0].tRes;
			for(int j=i0+1;j<=i1;j++){
				if(dados.transfer.celula[j].Pcamada<tabDin[i].pmin)tabDin[i].pmin=dados.transfer.celula[j].Pcamada;
				else if(dados.transfer.celula[j].Pcamada>tabDin[i].pmax)tabDin[i].pmax=dados.transfer.celula[j].Pcamada;
				if(dados.transfer.celula[j].tRes<tabDin[i].tmin)tabDin[i].tmin=dados.transfer.celula[j].tRes;
				else if(dados.transfer.celula[j].tRes>tabDin[i].tmax)tabDin[i].tmax=dados.transfer.celula[j].tRes;
			}
		}
		else{
			tabDin[i].pmax=malha.mlh2d[i0].cel2D.presC;
			tabDin[i].pmin=malha.mlh2d[i0].cel2D.presC;
			tabDin[i].tmax=malha.mlh2d[i0].tRes;
			tabDin[i].tmin=malha.mlh2d[i0].tRes;
			for(int j=i0+1;j<=i1;j++){
				if(malha.mlh2d[j].cel2D.presC<tabDin[i].pmin)tabDin[i].pmin=malha.mlh2d[j].cel2D.presC;
				else if(malha.mlh2d[j].cel2D.presC>tabDin[i].pmax)tabDin[i].pmax=malha.mlh2d[j].cel2D.presC;
				if(malha.mlh2d[j].tRes<tabDin[i].tmin)tabDin[i].tmin=malha.mlh2d[j].tRes;
				else if(malha.mlh2d[j].tRes>tabDin[i].tmax)tabDin[i].tmax=malha.mlh2d[j].tRes;
			}
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
	double pmed;
	double tmed;
	if(dados.acop==1){
		int i0=tabDin[0].celIni;
		int i1=tabDin[0].celFim;
		dados.transfer.celula[i0].flup.atualizaPropCompStandard();
		pmed = dados.transfer.celula[i0].pmed1;
		tmed=dados.transfer.celula[i0].tRes;
		dados.transfer.celula[i0].flup.atualizaPropComp(pmed, tmed,-1,NULL,NULL,0);
		for(int i=i0+1;i<=i1;i++){
			atualizaCelTransfer(i);
		//celula[i].flup.atualizaPropCompStandard();
		}
	}
	else{
		int i0=tabDin[0].celIni;
		int i1=tabDin[0].celFim;
		malha.mlh2d[i0].flup.atualizaPropCompStandard();
		pmed = malha.mlh2d[i0].cel2D.presC;
		tmed = malha.mlh2d[i0].tRes;
		malha.mlh2d[i0].flup.atualizaPropComp(pmed, tmed,-1,NULL,NULL,0);
		for(int i=i0+1;i<=i1;i++){
			atualizaCel2D(i);
		//celula[i].flup.atualizaPropCompStandard();
		}
	}


	if(dados.acop==1){
		for(int j=1;j<ntabDin;j++){
			int i0=tabDin[j].celIni;
			int i1=tabDin[j].celFim;
			malha.mlh2d[i0].flup.atualizaPropCompStandard();
			pmed = malha.mlh2d[i0].cel2D.presC;
			tmed = malha.mlh2d[i0].tRes;
			//celula[i0].flup.atualizaPropComp(pmed, tmed,-1,NULL,NULL,0);
			malha.mlh2d[i0].flup.atualizaPropComp(pmed, tmed,malha.mlh2d[i0-1].flup.dCalculatedBeta,
					malha.mlh2d[i0-1].flup.oCalculatedLiqComposition,
					malha.mlh2d[i0-1].flup.oCalculatedVapComposition,0);
			for(int i=i0+1;i<=i1;i++){
				for(int k=0; k<malha.mlh2d[i].flup.npseudo;k++)
					malha.mlh2d[i].flup.fracMol[k]=malha.mlh2d[i-1].flup.fracMol[k];
			atualizaCel2D(i);
			//celula[i].flup.atualizaPropCompStandard();
			}
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
				if(i<ntabDin-1)	fluF=dados.transfer.celula[indIni].flup;
				else fluF=malha.mlh2d[indIni].flup;
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
			dados.transfer.celula[j].flup.tabDin.TBF=tabDin[i].TBF;
			dados.transfer.celula[j].flup.tabDin.PBF=tabDin[i].PBF;
			dados.transfer.celula[j].flup.tabDin.rholF=tabDin[i].rholF;
			dados.transfer.celula[j].flup.tabDin.rhogF=tabDin[i].rhogF;
			dados.transfer.celula[j].flup.tabDin.DrholDpF=tabDin[i].DrholDpF;
			dados.transfer.celula[j].flup.tabDin.DrhogDpF=tabDin[i].DrhogDpF;
			dados.transfer.celula[j].flup.tabDin.DrholDtF=tabDin[i].DrholDtF;
			dados.transfer.celula[j].flup.tabDin.DrhogDtF=tabDin[i].DrhogDtF;
			dados.transfer.celula[j].flup.tabDin.valBO=tabDin[i].valBO;
			dados.transfer.celula[j].flup.tabDin.tit=tabDin[i].tit;
			dados.transfer.celula[j].flup.tabDin.rs=tabDin[i].rs;
			dados.transfer.celula[j].flup.tabDin.cplF=tabDin[i].cplF;
			dados.transfer.celula[j].flup.tabDin.cpgF=tabDin[i].cpgF;
			dados.transfer.celula[j].flup.tabDin.valZ=tabDin[i].valZ;
			dados.transfer.celula[j].flup.tabDin.HlF=tabDin[i].HlF;
			dados.transfer.celula[j].flup.tabDin.HgF=tabDin[i].HgF;
			dados.transfer.celula[j].flup.tabDin.valdZdT=tabDin[i].valdZdT;
			dados.transfer.celula[j].flup.tabDin.valdZdP=tabDin[i].valdZdP;
			dados.transfer.celula[j].flup.tabDin.viscO=tabDin[i].viscO;
			dados.transfer.celula[j].flup.tabDin.viscG=tabDin[i].viscG;
			dados.transfer.celula[j].flup.tabDin.delP=tabDin[i].delP;
			dados.transfer.celula[j].flup.tabDin.delT=tabDin[i].delT;
			dados.transfer.celula[j].flup.tabDin.npontosP=tabDin[i].npontosP;
			dados.transfer.celula[j].flup.tabDin.npontosT=tabDin[i].npontosT;
			dados.transfer.celula[j].flup.tabDin.pmax=tabDin[i].pmax;
			dados.transfer.celula[j].flup.tabDin.pmin=tabDin[i].pmin;
			dados.transfer.celula[j].flup.tabDin.tmax=tabDin[i].tmax;
			dados.transfer.celula[j].flup.tabDin.tmin=tabDin[i].tmin;
		}
	}

	if(dados.acop==1) for(int i=0; i<=dados.transfer.ncel;i++)dados.transfer.celula[i].flup.tabelaDinamica=1;
	for(int i=0; i<=malha.nele;i++)malha.mlh2d[i].flup.tabelaDinamica=1;
}


void solverPoro::auxMiniTab(ProFlu& flui) {
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

void solverPoro::geraMiniTabFlu(){
	dados.flup.miniTabDin.pmax=dados.pInt+10;
	dados.flup.miniTabDin.pmin=dados.pInt-10;
	dados.flup.miniTabDin.tmax=dados.tRes+1;
	dados.flup.miniTabDin.tmin=dados.tRes-1;
	dados.flup.atualizaPropCompStandard();
	if(dados.flup.dCalculatedBeta>0. && dados.flup.dCalculatedBeta<1.)
		dados.flup.atualizaPropComp(dados.pInt, dados.tRes,
				dados.flup.dCalculatedBeta, dados.flup.oCalculatedLiqComposition,
				dados.flup.oCalculatedVapComposition,0);
	else
		dados.flup.atualizaPropComp(dados.pInt, dados.tRes,-1,NULL,NULL,0);
	auxMiniTab(dados.flup);
	int ncelRad;
	if(dados.acop==1){
		ncelRad=dados.transfer.ncel;
		for(int j=0; j<ncelRad;j++){
			dados.transfer.celula[j].flup.miniTabDin.pmax=dados.transfer.celula[j].Pcamada+10;
			dados.transfer.celula[j].flup.miniTabDin.pmin=dados.transfer.celula[j].Pcamada-10;
			dados.transfer.celula[j].flup.miniTabDin.tmax=dados.tRes+1;
			dados.transfer.celula[j].flup.miniTabDin.tmin=dados.tRes-1;
			dados.transfer.celula[j].flup.atualizaPropCompStandard();
			if(dados.transfer.celula[j].flup.dCalculatedBeta>0. &&
				dados.transfer.celula[j].flup.dCalculatedBeta<1.)
				dados.transfer.celula[j].flup.atualizaPropComp(dados.transfer.celula[j].Pcamada,
					dados.transfer.celula[j].tRes,
					dados.transfer.celula[j].flup.dCalculatedBeta,
					dados.transfer.celula[j].flup.oCalculatedLiqComposition,
					dados.transfer.celula[j].flup.oCalculatedVapComposition,0);
			else
				dados.transfer.celula[j].flup.atualizaPropComp(dados.transfer.celula[j].Pcamada,
					dados.transfer.celula[j].tRes,-1,NULL,NULL,0);
			auxMiniTab(dados.transfer.celula[j].flup);
		}
	}
	ncelRad=malha.nele;
	for(int j=0; j<ncelRad;j++){
		malha.mlh2d[j].flup.miniTabDin.pmax=malha.mlh2d[j].cel2D.presC+10;
		malha.mlh2d[j].flup.miniTabDin.pmin=malha.mlh2d[j].cel2D.presC-10;
		malha.mlh2d[j].flup.miniTabDin.tmax=dados.tRes+1;
		malha.mlh2d[j].flup.miniTabDin.tmin=dados.tRes-1;
		malha.mlh2d[j].flup.atualizaPropCompStandard();
		if(malha.mlh2d[j].flup.dCalculatedBeta>0. &&
				malha.mlh2d[j].flup.dCalculatedBeta<1.)
			malha.mlh2d[j].flup.atualizaPropComp(malha.mlh2d[j].cel2D.presC,
					malha.mlh2d[j].tRes,
					malha.mlh2d[j].flup.dCalculatedBeta,
					malha.mlh2d[j].flup.oCalculatedLiqComposition,
					malha.mlh2d[j].flup.oCalculatedVapComposition,0);
		else
			malha.mlh2d[j].flup.atualizaPropComp(malha.mlh2d[j].cel2D.presC,
					malha.mlh2d[j].tRes,-1,NULL,NULL,0);
		auxMiniTab(malha.mlh2d[j].flup);
	}

}

void solverPoro::inicializaTransientePoroso(){
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
}

double solverPoro::defineDeltPoroso(){
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

void solverPoro::defineDT(int perm){
    int ind=0;
	double raz;
	double delt;
	if(perm==0){
		indrazT(ind, raz);
		double inf = dados.temp.dtmax[ind];
		double sup;
		if (ind < dados.temp.parserie - 1){
			sup = dados.temp.dtmax[ind + 1];
			delt = (inf * raz + (1 - raz) * sup);
		}
		else
			delt = dados.temp.dtmax[ind];
	}
	else{
		delt=2*dados.temp.dtmax[0];
	}
	for(int i=0; i<malha.nele;i++){
		double dtaux=0.8*malha.mlh2d[i].cflA();
		if(delt>dtaux)delt=dtaux;
	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++){
			double dtaux=0.8*dados.transfer.celula[i].cflA();
			if(delt>dtaux)delt=dtaux;
		}
	}
	for(int i=0; i<malha.nele;i++)malha.mlh2d[i].dt=delt;
	if(dados.acop==1){
		dados.transfer.dt=delt;
		for(int i=0;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].dt=delt;
			dados.transfer.celula[i].dtSL=delt;
			dados.transfer.celula[i].dtSW=delt;
		}
	}
	dt=delt;

}

void solverPoro::transientePoroso(double& delt){
	//double delt=vdelt;
	double norma;
	int itera;
	int indsra;
	reinicia=0;
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

			int ind=0;
			double raz;
			double inf;
			double sup;

			lAcopTot=0.;
			if(dados.acop==1){
				for(int i=0;i<malha.nele;i++){
			    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    		if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
								lAcopTot+=malha.mlh2d[i].cel2D.sFaceMod[j];
							}
			    		}
			    	}
				}

				indrazPW(ind, raz);
				inf = dados.pW.val[ind];
				if (ind < dados.pW.nserie - 1){
					sup = dados.pW.val[ind + 1];
					dados.transfer.Pint = (inf * raz + (1 - raz) * sup);
				}
				else
					dados.transfer.Pint = dados.pW.val[ind];
				for(int i=0;i<dados.transfer.ncel;i++){
					dados.transfer.celula[i].Pint=dados.transfer.Pint;
				}

				indraz(ind, raz,dados.tempoSatAPoc,dados.nsatAPoc);
				inf = dados.satAPoc[ind];
				if (ind < dados.nsatAPoc - 1){
					sup = dados.satAPoc[ind + 1];
					sWPoc = (inf * raz + (1 - raz) * sup);
				}
				else
					sWPoc = dados.satAPoc[ind];
				dados.transfer.sWPoc=sWPoc;
				for(int i=0; i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWPoc=sWPoc;
			}

			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
		    			malha.mlh2d[i].atualizaCC(j);
		    		}
		    	}
			}
			if(dados.acop==1){
		    	dados.qTotalW=-dados.transfer.celula[dados.transfer.ncel-2].QwcamadaR;
		    	dados.qTotalO=-dados.transfer.celula[dados.transfer.ncel-2].QocamadaR;
		    	dados.qTotalG=-dados.transfer.celula[dados.transfer.ncel-2].QgcamadaR;
		    	dados.qTotal=(dados.qTotalW+dados.qTotalO+dados.qTotalG);
				for(int i=0;i<malha.nele;i++){
			    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    		if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
								malha.mlh2d[i].ccPVN[j]=1.*(malha.mlh2d[i].rhoF[j]*(dados.qTotalO+dados.qTotalG)+
										0.*malha.mlh2d[i].rhogF[j]*dados.qTotalG+malha.mlh2d[i].rhoaF[j]*dados.qTotalW)/lAcopTot;
								malha.mlh2d[i].CC.satAcop=dados.transfer.celula[dados.transfer.ncel-2].sW;
							}
			    		}
			    	}
				}
			}

			delt=defineDeltPoroso();

			for(int i=0; i<malha.nele;i++){
				double dtaux=0.8*malha.mlh2d[i].cflA();
				if(delt>dtaux)delt=dtaux;
			}
			if(dados.acop==1){
				for(int i=0;i<dados.transfer.ncel;i++){
				  //delt=0.5*celula[i].cflO();
				  //if(dt>delt)dt=delt;
					double dtaux=0.8*dados.transfer.celula[i].cflA();
					if(delt>dtaux)delt=dtaux;
				}
			}
			for(int i=0; i<malha.nele;i++)malha.mlh2d[i].dt=delt;
			if(dados.acop==1){
				dados.transfer.dt=delt;
				for(int i=0;i<dados.transfer.ncel;i++){
					dados.transfer.celula[i].dt=delt;
					dados.transfer.celula[i].dtSL=delt;
					dados.transfer.celula[i].dtSW=delt;
				}
			}


			for(int i=0; i<malha.nele;i++){
				malha.mlh2d[i].evoluiSW(reinicia);
			}
			if(dados.acop==1){
				dados.transfer.sWRes=0.;
				for(int i=0;i<malha.nele;i++){
			    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    		if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
								dados.transfer.sWRes+=malha.mlh2d[i].swF[j]*malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot;
							}
			    		}
			    	}
				}
				for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWRes=dados.transfer.sWRes;
				for(int i=0;i<dados.transfer.ncel;i++){
					if(i>=dados.transfer.ncel-1 && dados.transfer.celula[dados.transfer.ncel-2].QcamadaL>0.){
						dados.transfer.celula[i].sL=dados.transfer.sLRes;
						dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sW=dados.transfer.sWRes;
						dados.transfer.celula[i-1].sWR=dados.transfer.sWRes;
						dados.transfer.celula[i].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sWR=dados.transfer.sWRes;
						dados.transfer.celula[i].reiniciaSL=0;
						dados.transfer.celula[i].reiniciaSW=0;
					}
					else if(i==0 && dados.transfer.celula[1].QcamadaR<0.){
						dados.transfer.celula[i].sL=dados.transfer.sLRes;
						//dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sW=dados.transfer.sWPoc;
						//dados.transfer.celula[i-1].sWR=dados.transfer.sWPoc;
						dados.transfer.celula[i].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sWR=dados.transfer.sWPoc;
						dados.transfer.celula[i].reiniciaSL=0;
						dados.transfer.celula[i].reiniciaSW=0;
					}
					else{
						//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
						dados.transfer.celula[i].evoluiSW(reinicia, 0, 0);
						if(i>0){
							dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
							dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
							dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
							dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
						}
						else{
							dados.transfer.celula[i].sLL=dados.transfer.celula[i].sL;
							dados.transfer.celula[i].sWL=dados.transfer.celula[i].sW;
						}
					}
				}
			}

			for(int i=0;i<malha.nele;i++){
				  if(malha.mlh2d[i].reiniciaSW<0){
					  reinicia=-1;
				  }
				  //celula[i].reiniciaSL=0;
				  malha.mlh2d[i].reiniciaSW=0;
			}
			if(dados.acop==1){
				for(int i=0;i<dados.transfer.ncel;i++){
					if(dados.transfer.celula[i].reiniciaSL<0 || dados.transfer.celula[i].reiniciaSW<0){
						reinicia=-1;
					}
				  //celula[i].reiniciaSL=0;
					dados.transfer.celula[i].reiniciaSW=0;
				}
			}
			if(reinicia<0){
				for(int i=0;i<malha.nele;i++){
					//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
					if(delt>malha.mlh2d[i].dtSW)delt=malha.mlh2d[i].dtSW;
				}
				if(dados.acop==1){
					for(int i=0;i<dados.transfer.ncel;i++){
						//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
						if(delt>dados.transfer.celula[i].dtSW)delt=dados.transfer.celula[i].dtSW;
					}
				}
				for(int i=0;i<malha.nele;i++){
					malha.mlh2d[i].dt=delt;
					malha.mlh2d[i].dtSW=delt;
				}
				if(dados.acop==1){
					dados.transfer.dt=delt;
					for(int i=0;i<dados.transfer.ncel;i++){
						dados.transfer.celula[i].dt=delt;
						dados.transfer.celula[i].dtSL=delt;
						dados.transfer.celula[i].dtSW=delt;
					}
				}

				for(int i=0; i<malha.nele;i++){
					malha.mlh2d[i].evoluiSW(reinicia);
				}
				if(dados.acop==1){
					dados.transfer.sWRes=0.;
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.transfer.sWRes+=malha.mlh2d[i].swF[j]*malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot;
								}
				    		}
				    	}
					}
					for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWRes=dados.transfer.sWRes;
					for(int i=0;i<dados.transfer.ncel;i++){
						if(i>=dados.transfer.ncel-1){
							dados.transfer.celula[i].sL=dados.transfer.sLRes;
							dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
							dados.transfer.celula[i].sW=dados.transfer.sWRes;
							dados.transfer.celula[i-1].sWR=dados.transfer.sWRes;
							dados.transfer.celula[i].sLR=dados.transfer.sLRes;
							dados.transfer.celula[i].sWR=dados.transfer.sWRes;
							dados.transfer.celula[i].reiniciaSL=0;
							dados.transfer.celula[i].reiniciaSW=0;
						}
						else{
							//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
							dados.transfer.celula[i].evoluiSW(reinicia, 0, 0);
							if(i>0){
								dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
								dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
								dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
								dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
							}
							else{
								dados.transfer.celula[i].sLL=dados.transfer.celula[i].sL;
								dados.transfer.celula[i].sWL=dados.transfer.celula[i].sW;
							}
						}
					}
				}
			}

			itera=0;
			double minP=100000000000.;
			while((norma>(1.e-5*dados.temp.relax) || itera<3) && (itera<80)){
                #pragma omp parallel for num_threads(dados.nthrdLocal)
		    	for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
						/*if(i==1795){
							int para;
							para=0;
						}*/
		    			malha.mlh2d[i].cel2D.gradGreenPI[j]=malha.mlh2d[i].cel2D.gradGreenP[j];
		    			malha.mlh2d[i].cel2D.gradGreenSwI[j]=malha.mlh2d[i].cel2D.gradGreenSw[j];
		    			malha.mlh2d[i].cel2D.gradGreenZdatumI[j]=malha.mlh2d[i].cel2D.gradGreenZdatum[j];
		    			malha.mlh2d[i].cel2D.gradGreenAZdatumI[j]=malha.mlh2d[i].cel2D.gradGreenAZdatum[j];
		    			malha.mlh2d[i].cel2D.gradGreenPcAOI[j]=malha.mlh2d[i].cel2D.gradGreenPcAO[j];
		    		}
		    	}
                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++){
					/*if(i==80 || i==100 || i==120 || i==140 || i==160 || i==200 || i==276 || i==300 || i==400 || i==500 || i==600 || i==700){
						int para;
						para=0;
					}*/
					malha.mlh2d[i].calcGradGreen();
				}
                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocal(dados.temp.relax);
				indsra=0;
				for(int i=0;i<malha.nele;i++){
					int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
					for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
					indsra+=dimNoZero;
					termolivre[i]=malha.mlh2d[i].TL[0];
				}
				double eps=0.000001;
				int iter=malha.nele;
				int retorna;
				for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presC;
				if(dados.solverMat==0)retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				else if(dados.solverMat==1)retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				else retorna=matglob.BiCGStab(malha.vecSolv,termolivre,eps,iter,precn);
				//retorna=matglob.CG(malha.vecSolv,termolivre,eps,iter,1);
				//if(retorna==1)break;
				norma=0.;
				double val;
				double val2;
				int val3;
				for(int i=0;i<malha.nele;i++){
					malha.mlh2d[i].cel2D.presCI=malha.mlh2d[i].cel2D.presC;
					malha.mlh2d[i].cel2D.presC=malha.vecSolv[i];
					if(minP>malha.mlh2d[i].cel2D.presC)minP=malha.mlh2d[i].cel2D.presC;
					norma+=fabsl(malha.mlh2d[i].cel2D.presC-malha.mlh2d[i].cel2D.presCI);///malha.mlh2d[i].cel2D.presCI;
					for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
						if(malha.mlh2d[i].kvizinho[j]<0){
							int diri=0;
							int rich=0;
							int vn=0;
							int kcc=0;
							int acoplado=0;
							malha.mlh2d[i].tipoCC(j, diri, vn, rich, acoplado, kcc);
							if(diri==1){
								val=malha.mlh2d[i].cel2D.presC;
								val2=malha.mlh2d[i].Qtotal[j];
								val3=0;
							}
						}
					}
				}
				norma/=malha.nele;
			    if(dados.acop==1){

			    	double presresAux=dados.transfer.presRes2;
			    	dados.transfer.presRes2=0.;
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.transfer.presRes2+=malha.mlh2d[i].presF[j]*malha.mlh2d[i].cel2D.sFaceMod[j];
									malha.mlh2d[i].CC.satAcop=dados.transfer.celula[dados.transfer.ncel-2].sW;
								}
				    		}
				    	}
					}
					dados.transfer.presRes2/=lAcopTot;
					//dados.transfer.presRes2=0.5*(dados.transfer.presRes2+presresAux);
					for(int i=0;i<dados.transfer.ncel;i++){
						dados.transfer.celula[i].presRes2=dados.transfer.presRes2;
					}
					double erro=dados.transfer.transtrans(dados.espessuraRes);
					dados.BSW=dados.transfer.celula[2].BSW;
					norma+=fabsl(erro);

			    	dados.qTotalW=-dados.transfer.celula[dados.transfer.ncel-2].QwcamadaR;
			    	dados.qTotalO=-dados.transfer.celula[dados.transfer.ncel-2].QocamadaR;
			    	dados.qTotalG=-dados.transfer.celula[dados.transfer.ncel-2].QgcamadaR;
			    	dados.qTotal=(dados.qTotalW+dados.qTotalO+dados.qTotalG);
					/*for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									malha.mlh2d[i].Qtotal[j]=dados.qTotal;
									malha.mlh2d[i].Qw[j]=dados.qTotalW;
									malha.mlh2d[i].Qoleo[j]=dados.qTotalO;
									malha.mlh2d[i].Qgas[j]=dados.qTotalG;
								}
				    		}
				    	}
					}
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									malha.mlh2d[i].Qtotal[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
									malha.mlh2d[i].Qw[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
									malha.mlh2d[i].Qoleo[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
									malha.mlh2d[i].Qgas[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
								}
				    		}
				    	}
					}*/
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									//malha.mlh2d[i].ccPVN[j]=dados.qTotal/lAcopTot;
									malha.mlh2d[i].ccPVN[j]=1.*(malha.mlh2d[i].rhoF[j]*(dados.qTotalO+dados.qTotalG)+
											0.*malha.mlh2d[i].rhogF[j]*dados.qTotalG+malha.mlh2d[i].rhoaF[j]*dados.qTotalW)/lAcopTot;
								}
				    		}
				    	}
					}

			    }

				if((*vg1dSP).tempo>=4040){
						int para;
						para=0;

						//imprimeMalhaRegularAux(minP);
						//imprimeMalhaRegularSWAux();
						//imprimeMalhaRegularAlfAux();
				}

                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0;i<malha.nele;i++){
					//malha.mlh2d[i].cel2D.vbo;
					if(dados.tipoModelOleo==1){
						malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					}
					else{
						malha.mlh2d[i].cel2D.rhoP=dados.rhoRef*(1-dados.compOleo*(dados.pRes-malha.mlh2d[i].cel2D.presC));
					}
					malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					if(dados.tipoModelOleo==1){
						double tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
						malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
					}
					else{
						malha.mlh2d[i].cel2D.alfC=0.;
					}
					malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					malha.mlh2d[i].cel2D.pcAO=malha.mlh2d[i].interpolaTabela(malha.mlh2d[i].pcOACel.npont, malha.mlh2d[i].cel2D.sWC, malha.mlh2d[i].pcOACel.satW,
							malha.mlh2d[i].pcOACel.presCapOW);
				}

			    itera++;

			}
			if(dados.acop==1)dados.transfer.atualizaIni();
            #pragma omp parallel for num_threads(dados.nthrdLocal)
			for(int i=0;i<malha.nele;i++){
				malha.mlh2d[i].cel2D.presC0=malha.mlh2d[i].cel2D.presC;
				malha.mlh2d[i].cel2D.sWI=malha.mlh2d[i].cel2D.sWC;

				malha.mlh2d[i].cel2D.rhoP0=malha.mlh2d[i].cel2D.rhoP;
				malha.mlh2d[i].cel2D.rhogP0=malha.mlh2d[i].cel2D.rhogP;
				malha.mlh2d[i].cel2D.rhoaP0=malha.mlh2d[i].cel2D.rhoaP;
				malha.mlh2d[i].cel2D.alfC0=malha.mlh2d[i].cel2D.alfC;

				//malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
				//malha.mlh2d[i].cel2D.vbo;
				/*malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				double tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
				malha.mlh2d[i].cel2D.pcAO=malha.mlh2d[i].interpolaTabela(malha.mlh2d[i].pcOACel.npont, malha.mlh2d[i].cel2D.sWC, malha.mlh2d[i].pcOACel.satW,
						malha.mlh2d[i].pcOACel.presCapOW);*/

				/*for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					malha.mlh2d[i].rhoF[j]=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					malha.mlh2d[i].rhogF[j]=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					malha.mlh2d[i].rhoaF[j]=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
					malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].migF[j]=malha.mlh2d[i].flup.ViscGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].miaF[j]=malha.mlh2d[i].flup.VisAgua(malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].rhoF[j]=(1.-malha.mlh2d[i].alfF[j])*malha.mlh2d[i].rhoF[j]+malha.mlh2d[i].alfF[j]*malha.mlh2d[i].rhogF[j];
				}*/

			}


			cout << "Tempo= " << (*vg1dSP).tempo<<" norma= "<< norma<<" iteracoes= "<<itera;
			cout << endl;

			//tempo+=delt;

			if (((*vg1dSP).tempo > 1e-15 && (*vg1dSP).tempo <= dados.temp.tempoImp[kontaTempo] && (*vg1dSP).tempo + delt >= dados.temp.tempoImp[kontaTempo])) {
				malha.imprime((*vg1dSP).tempo);
				imprimeMalhaRegular(minP);
				imprimeMalhaRegularSW();
				imprimeMalhaRegularAlf();
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			  	  FullMtx<double> matrizsaida(dados.transfer.nglobal,11);
			  	  matrizsaida=dados.transfer.perfil();
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

				///////////////////////////////////////////////////////////////////////////////////////////////////////////////
				kontaTempo++;
				if (kontaTempo >= dados.temp.parserieImp) kontaTempo--;
			}
		//}
	//}
}

void solverPoro::imprimePseudo(){

	double minP=1000000000.;
	for(int i=0;i<malha.nele;i++){
		if(minP>malha.mlh2d[i].cel2D.presC)minP=malha.mlh2d[i].cel2D.presC;
	}

	imprimeMalhaRegular(minP);
	imprimeMalhaRegularSW();
	imprimeMalhaRegularAlf();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  	  FullMtx<double> matrizsaida(dados.transfer.nglobal,11);
  	  matrizsaida=dados.transfer.perfil();
  	  ostringstream saida;
  	  int numero = roundl(0);
  	  //saida << pathPrefixoArqSaida << "PerfisPocoRadial" << "-" << kontaTempoImp
				//<<"-"<<posicMarlim<< ".dat";
  	  saida << pathPrefixoArqSaida << "PerfisPocoRadial" << "-" << "0"
  							<<"-"<<posicMarlim<< ".dat";
  	  string tmp = saida.str();
  	  ofstream escreveIni(tmp.c_str(), ios_base::out);
  	  escreveIni<<"tempo = "<< 0<<endl;
  	  escreveIni<<" raio (m) ;"<< " raio (pol.) ;"<<" pressao (kgf/cm2) ;"<<" vazao total (sm3/d) ;"<<
  			  " vazao de oleo (sm3/d) ;"<<" vazao de gas (sm3/d) ;"<<" vazao de agua (sm3/d) ;"<<
			  "saturacao de liquido (-) ;"<<" saturacao de agua (-) ;"<<" fracao volumetrica de gas homogeneo (-) ;"
			  <<" BSW (-) ;"<<endl;
  	  escreveIni << matrizsaida;
  	  escreveIni.close();
}

void solverPoro::pseudoTransientePoroso(){
	//double delt=vdelt;
	double norma;
	int itera;
	int indsra;
	reinicia=0;
	double delt;
	delt=0;
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].cel2D.presCI=malha.mlh2d[i].cel2D.presC;
		malha.mlh2d[i].cel2D.presC0=malha.mlh2d[i].cel2D.presC;
	}
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

			int ind;
			double raz;
			double inf;
			double sup;

			lAcopTot=0.;
			if(dados.acop==1){
				for(int i=0;i<malha.nele;i++){
			    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    		if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
								lAcopTot+=malha.mlh2d[i].cel2D.sFaceMod[j];
							}
			    		}
			    	}
				}

				dados.transfer.Pint = dados.pW.val[0];
				sWPoc = dados.satAPoc[0];
				dados.transfer.sWPoc=sWPoc;
				for(int i=0; i<dados.transfer.ncel;i++){
					dados.transfer.celula[i].Pint=dados.transfer.Pint;
					dados.transfer.celula[i].sWPoc=sWPoc;
				}
			}

			for(int i=0;i<malha.nele;i++){
		    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    		if(malha.mlh2d[i].kvizinho[j]<0){
		    			malha.mlh2d[i].atualizaCC(j);
		    		}
		    	}
			}



			double erroPermanente=1000.;
			double erroPermanenteSW=1000.;
			int kontaItera=0;
			double minP;
			double razTemp=10.;
			while((erroPermanente>(dados.transfer.Pint/1000000.) /*&& erroPermanenteSW>(1./1000.)*/) || kontaItera<3){
			if(dados.acop==1){
		    	dados.qTotalW=-dados.transfer.celula[dados.transfer.ncel-2].QwcamadaR;
		    	dados.qTotalO=-dados.transfer.celula[dados.transfer.ncel-2].QocamadaR;
		    	dados.qTotalG=-dados.transfer.celula[dados.transfer.ncel-2].QgcamadaR;
		    	dados.qTotal=(dados.qTotalW+dados.qTotalO+dados.qTotalG);
				for(int i=0;i<malha.nele;i++){
			    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    		if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
								malha.mlh2d[i].ccPVN[j]=1.*(malha.mlh2d[i].rhoF[j]*(dados.qTotalO+dados.qTotalG)+
										0.*malha.mlh2d[i].rhogF[j]*dados.qTotalG+malha.mlh2d[i].rhoaF[j]*dados.qTotalW)/lAcopTot;
								malha.mlh2d[i].CC.satAcop=dados.transfer.celula[dados.transfer.ncel-2].sW;
							}
			    		}
			    	}
				}
			}

			delt=dados.temp.dtmax[0]/razTemp;
			if( delt>1.){
				delt=1.;
				razTemp=dados.temp.dtmax[0];
			}

			for(int i=0; i<malha.nele;i++){
				double dtaux=0.8*malha.mlh2d[i].cflA();
				if(delt>dtaux)delt=dtaux;
			}
			if(dados.acop==1){
				for(int i=0;i<dados.transfer.ncel;i++){
				  //delt=0.5*celula[i].cflO();
				  //if(dt>delt)dt=delt;
					double dtaux=0.8*dados.transfer.celula[i].cflA();
					if(delt>dtaux)delt=dtaux;
				}
			}
			for(int i=0; i<malha.nele;i++)malha.mlh2d[i].dt=delt;
			if(dados.acop==1){
				dados.transfer.dt=delt;
				for(int i=0;i<dados.transfer.ncel;i++){
					dados.transfer.celula[i].dt=delt;
					dados.transfer.celula[i].dtSL=delt;
					dados.transfer.celula[i].dtSW=delt;
				}
			}


			for(int i=0; i<malha.nele;i++){
				malha.mlh2d[i].evoluiSW(reinicia);
			}
			if(dados.acop==1){
				dados.transfer.sWRes=0.;
				for(int i=0;i<malha.nele;i++){
			    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    		if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
								dados.transfer.sWRes+=malha.mlh2d[i].swF[j]*malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot;
							}
			    		}
			    	}
				}
				for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWRes=dados.transfer.sWRes;
				for(int i=0;i<dados.transfer.ncel;i++){
					if(i>=dados.transfer.ncel-1 && dados.transfer.celula[dados.transfer.ncel-2].QcamadaL>0.){
						dados.transfer.celula[i].sL=dados.transfer.sLRes;
						dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sW=dados.transfer.sWRes;
						dados.transfer.celula[i-1].sWR=dados.transfer.sWRes;
						dados.transfer.celula[i].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sWR=dados.transfer.sWRes;
						dados.transfer.celula[i].reiniciaSL=0;
						dados.transfer.celula[i].reiniciaSW=0;
					}
					else if(i==0 && dados.transfer.celula[1].QcamadaR<0.){
						dados.transfer.celula[i].sL=dados.transfer.sLRes;
						//dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sW=dados.transfer.sWPoc;
						//dados.transfer.celula[i-1].sWR=dados.transfer.sWPoc;
						dados.transfer.celula[i].sLR=dados.transfer.sLRes;
						dados.transfer.celula[i].sWR=dados.transfer.sWPoc;
						dados.transfer.celula[i].reiniciaSL=0;
						dados.transfer.celula[i].reiniciaSW=0;
					}
					else{
						//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
						dados.transfer.celula[i].evoluiSW(reinicia, 0, 0);
						if(i>0){
							dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
							dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
							dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
							dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
						}
						else{
							dados.transfer.celula[i].sLL=dados.transfer.celula[i].sL;
							dados.transfer.celula[i].sWL=dados.transfer.celula[i].sW;
						}
					}
				}
			}

			for(int i=0;i<malha.nele;i++){
				  if(malha.mlh2d[i].reiniciaSW<0){
					  reinicia=-1;
				  }
				  //celula[i].reiniciaSL=0;
				  malha.mlh2d[i].reiniciaSW=0;
			}
			if(dados.acop==1){
				for(int i=0;i<dados.transfer.ncel;i++){
					if(dados.transfer.celula[i].reiniciaSL<0 || dados.transfer.celula[i].reiniciaSW<0){
						reinicia=-1;
					}
				  //celula[i].reiniciaSL=0;
					dados.transfer.celula[i].reiniciaSW=0;
				}
			}
			if(reinicia<0){
				for(int i=0;i<malha.nele;i++){
					//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
					if(delt>malha.mlh2d[i].dtSW)delt=malha.mlh2d[i].dtSW;
				}
				if(dados.acop==1){
					for(int i=0;i<dados.transfer.ncel;i++){
						//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
						if(delt>dados.transfer.celula[i].dtSW)delt=dados.transfer.celula[i].dtSW;
					}
				}
				for(int i=0;i<malha.nele;i++){
					malha.mlh2d[i].dt=delt;
					malha.mlh2d[i].dtSW=delt;
				}
				if(dados.acop==1){
					dados.transfer.dt=delt;
					for(int i=0;i<dados.transfer.ncel;i++){
						dados.transfer.celula[i].dt=delt;
						dados.transfer.celula[i].dtSL=delt;
						dados.transfer.celula[i].dtSW=delt;
					}
				}

				for(int i=0; i<malha.nele;i++){
					malha.mlh2d[i].evoluiSW(reinicia);
				}
				if(dados.acop==1){
					dados.transfer.sWRes=0.;
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.transfer.sWRes+=malha.mlh2d[i].swF[j]*malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot;
								}
				    		}
				    	}
					}
					for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWRes=dados.transfer.sWRes;
					for(int i=0;i<dados.transfer.ncel;i++){
						if(i>=dados.transfer.ncel-1){
							dados.transfer.celula[i].sL=dados.transfer.sLRes;
							dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
							dados.transfer.celula[i].sW=dados.transfer.sWRes;
							dados.transfer.celula[i-1].sWR=dados.transfer.sWRes;
							dados.transfer.celula[i].sLR=dados.transfer.sLRes;
							dados.transfer.celula[i].sWR=dados.transfer.sWRes;
							dados.transfer.celula[i].reiniciaSL=0;
							dados.transfer.celula[i].reiniciaSW=0;
						}
						else{
							//celula[i].evoluiSL(reinicia, ciclo, celula[i].dPdT);
							dados.transfer.celula[i].evoluiSW(reinicia, 0, 0);
							if(i>0){
								dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
								dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
								dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
								dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
							}
							else{
								dados.transfer.celula[i].sLL=dados.transfer.celula[i].sL;
								dados.transfer.celula[i].sWL=dados.transfer.celula[i].sW;
							}
						}
					}
				}
			}

			itera=0;
			minP=100000000000.;
			while(/*(norma>(1.e-1*dados.temp.relax) || itera<3) && (itera<80)*/itera<1){
                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++){
		    		for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
						/*if(i==1795){
							int para;
							para=0;
						}*/
		    			malha.mlh2d[i].cel2D.gradGreenPI[j]=malha.mlh2d[i].cel2D.gradGreenP[j];
		    			malha.mlh2d[i].cel2D.gradGreenSwI[j]=malha.mlh2d[i].cel2D.gradGreenSw[j];
		    			malha.mlh2d[i].cel2D.gradGreenZdatumI[j]=malha.mlh2d[i].cel2D.gradGreenZdatum[j];
		    			malha.mlh2d[i].cel2D.gradGreenAZdatumI[j]=malha.mlh2d[i].cel2D.gradGreenAZdatum[j];
		    			malha.mlh2d[i].cel2D.gradGreenPcAOI[j]=malha.mlh2d[i].cel2D.gradGreenPcAO[j];
		    		}
		    	}
                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++){
					/*if(i==80 || i==100 || i==120 || i==140 || i==160 || i==200 || i==276 || i==300 || i==400 || i==500 || i==600 || i==700){
						int para;
						para=0;
					}*/
					malha.mlh2d[i].calcGradGreen();
				}
                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocal(dados.temp.relax);
				indsra=0;
				for(int i=0;i<malha.nele;i++){
					int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
					for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
					indsra+=dimNoZero;
					termolivre[i]=malha.mlh2d[i].TL[0];
				}
				double eps=0.000001;
				int iter=malha.nele;
				int retorna;
				for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presC;
				retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
				//retorna=matglob.CG(malha.vecSolv,termolivre,eps,iter,1);
				//if(retorna==1)break;
				norma=0.;
				double val;
				double val2;
				int val3;
				for(int i=0;i<malha.nele;i++){
					malha.mlh2d[i].cel2D.presCI=malha.mlh2d[i].cel2D.presC;
					malha.mlh2d[i].cel2D.presC=malha.vecSolv[i];
					if(minP>malha.mlh2d[i].cel2D.presC)minP=malha.mlh2d[i].cel2D.presC;
					norma+=fabsl(malha.mlh2d[i].cel2D.presC-malha.mlh2d[i].cel2D.presCI);///malha.mlh2d[i].cel2D.presCI;
					for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
						if(malha.mlh2d[i].kvizinho[j]<0){
							int diri=0;
							int rich=0;
							int vn=0;
							int kcc=0;
							int acoplado=0;
							malha.mlh2d[i].tipoCC(j, diri, vn, rich, acoplado, kcc);
							if(diri==1){
								val=malha.mlh2d[i].cel2D.presC;
								val2=malha.mlh2d[i].Qtotal[j];
								val3=0;
							}
						}
					}
				}
				norma/=malha.nele;
			    if(dados.acop==1){

			    	double presresAux=dados.transfer.presRes2;
			    	dados.transfer.presRes2=0.;
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.transfer.presRes2+=malha.mlh2d[i].presF[j]*malha.mlh2d[i].cel2D.sFaceMod[j];
									malha.mlh2d[i].CC.satAcop=dados.transfer.celula[dados.transfer.ncel-2].sW;
								}
				    		}
				    	}
					}
					dados.transfer.presRes2/=lAcopTot;
					//dados.transfer.presRes2=0.5*(dados.transfer.presRes2+presresAux);
					for(int i=0;i<dados.transfer.ncel;i++){
						dados.transfer.celula[i].presRes2=dados.transfer.presRes2;
					}
					double erro=dados.transfer.transtrans(dados.espessuraRes);
					dados.BSW=dados.transfer.celula[2].BSW;
					norma+=fabsl(erro);

			    	dados.qTotalW=-dados.transfer.celula[dados.transfer.ncel-2].QwcamadaR;
			    	dados.qTotalO=-dados.transfer.celula[dados.transfer.ncel-2].QocamadaR;
			    	dados.qTotalG=-dados.transfer.celula[dados.transfer.ncel-2].QgcamadaR;
			    	dados.qTotal=(dados.qTotalW+dados.qTotalO+dados.qTotalG);
					/*for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									malha.mlh2d[i].Qtotal[j]=dados.qTotal;
									malha.mlh2d[i].Qw[j]=dados.qTotalW;
									malha.mlh2d[i].Qoleo[j]=dados.qTotalO;
									malha.mlh2d[i].Qgas[j]=dados.qTotalG;
								}
				    		}
				    	}
					}
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									malha.mlh2d[i].Qtotal[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
									malha.mlh2d[i].Qw[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
									malha.mlh2d[i].Qoleo[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
									malha.mlh2d[i].Qgas[j]*=(malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot);
								}
				    		}
				    	}
					}*/
					for(int i=0;i<malha.nele;i++){
				    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    		if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									//malha.mlh2d[i].ccPVN[j]=dados.qTotal/lAcopTot;
									malha.mlh2d[i].ccPVN[j]=1.*(malha.mlh2d[i].rhoF[j]*(dados.qTotalO+dados.qTotalG)+
											0.*malha.mlh2d[i].rhogF[j]*dados.qTotalG+malha.mlh2d[i].rhoaF[j]*dados.qTotalW)/lAcopTot;
								}
				    		}
				    	}
					}

			    }

				if((*vg1dSP).tempo>=4040){
						int para;
						para=0;

						//imprimeMalhaRegularAux(minP);
						//imprimeMalhaRegularSWAux();
						//imprimeMalhaRegularAlfAux();
				}
                #pragma omp parallel for num_threads(dados.nthrdLocal)
				for(int i=0;i<malha.nele;i++){
					//malha.mlh2d[i].cel2D.vbo;
					if(dados.tipoModelOleo==1){
						malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					}
					else{
						malha.mlh2d[i].cel2D.rhoP=dados.rhoRef*(1-dados.compOleo*(dados.pRes-malha.mlh2d[i].cel2D.presC));
					}
					malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					if(dados.tipoModelOleo==1){
						double tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
						malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
					}
					else{
						malha.mlh2d[i].cel2D.alfC=0.;
					}
					malha.mlh2d[i].cel2D.pcAO=malha.mlh2d[i].interpolaTabela(malha.mlh2d[i].pcOACel.npont, malha.mlh2d[i].cel2D.sWC, malha.mlh2d[i].pcOACel.satW,
							malha.mlh2d[i].pcOACel.presCapOW);
				}

			    itera++;

			}

			/*dados.transfer.fluxIni*=dados.espessuraRes;
			dados.transfer.fluxIniG*=dados.espessuraRes;
			dados.transfer.fluxIniA*=dados.espessuraRes;
			dados.transfer.fluxFim*=dados.espessuraRes;
			dados.transfer.fluxFimG*=dados.espessuraRes;
			dados.transfer.fluxFimA*=dados.espessuraRes;*/

			kontaItera++;
			erroPermanente=0.;
			erroPermanenteSW=0.;
			if(dados.acop==1){
				for(int i=0;i<dados.transfer.ncel;i++){
					erroPermanente+=fabsl(dados.transfer.celula[i].Pcamada-dados.transfer.celula[i].Pini);
					erroPermanenteSW+=fabsl(dados.transfer.celula[i].sW-dados.transfer.celula[i].sWini);
				}
				erroPermanente/=dados.transfer.ncel;
			}
			for(int i=0;i<malha.nele;i++){
				erroPermanente+=fabsl(malha.mlh2d[i].cel2D.presC-malha.mlh2d[i].cel2D.presC0);
				erroPermanenteSW+=fabsl(malha.mlh2d[i].cel2D.sWC-malha.mlh2d[i].cel2D.sWI);
			}
			erroPermanente/=malha.nele;
			erroPermanenteSW/=malha.nele;

			if(dados.acop==1)dados.transfer.atualizaIni();
            #pragma omp parallel for num_threads(dados.nthrdLocal)
			for(int i=0;i<malha.nele;i++){
				malha.mlh2d[i].cel2D.presC0=malha.mlh2d[i].cel2D.presC;
				malha.mlh2d[i].cel2D.sWI=malha.mlh2d[i].cel2D.sWC;

				malha.mlh2d[i].cel2D.rhoP0=malha.mlh2d[i].cel2D.rhoP;
				malha.mlh2d[i].cel2D.rhogP0=malha.mlh2d[i].cel2D.rhogP;
				malha.mlh2d[i].cel2D.rhoaP0=malha.mlh2d[i].cel2D.rhoaP;
				malha.mlh2d[i].cel2D.alfC0=malha.mlh2d[i].cel2D.alfC;

				//malha.mlh2d[i].cel2D.tempC=malha.vecSolv[i];
				//malha.mlh2d[i].cel2D.vbo;
				/*malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				double tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
				malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
				malha.mlh2d[i].cel2D.pcAO=malha.mlh2d[i].interpolaTabela(malha.mlh2d[i].pcOACel.npont, malha.mlh2d[i].cel2D.sWC, malha.mlh2d[i].pcOACel.satW,
						malha.mlh2d[i].pcOACel.presCapOW);*/

				/*for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					malha.mlh2d[i].rhoF[j]=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					malha.mlh2d[i].rhogF[j]=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					malha.mlh2d[i].rhoaF[j]=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes);
					malha.mlh2d[i].alfF[j]=(tit/malha.mlh2d[i].rhogF[j])/((tit/malha.mlh2d[i].rhogF[j])+((1.-tit)/malha.mlh2d[i].rhoF[j]));
					malha.mlh2d[i].mioF[j]=malha.mlh2d[i].flup.ViscOleo(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].migF[j]=malha.mlh2d[i].flup.ViscGas(malha.mlh2d[i].presF[j], malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].miaF[j]=malha.mlh2d[i].flup.VisAgua(malha.mlh2d[i].tRes)/1000.;
					malha.mlh2d[i].rhoF[j]=(1.-malha.mlh2d[i].alfF[j])*malha.mlh2d[i].rhoF[j]+malha.mlh2d[i].alfF[j]*malha.mlh2d[i].rhogF[j];
				}*/

			}

			  //cout << "  erro= "<<erroPermanente<<";"<<" iteracoes= "<<kontaItera;
			  //cout << endl;

			}


			//cout << "Tempo= " << tempo<<" norma= "<< norma<<" iteracoes= "<<kontaItera;
			//cout << endl;

			//tempo+=delt;

				//malha.imprime(tempo);
/*
				imprimeMalhaRegular(minP);
				imprimeMalhaRegularSW();
				imprimeMalhaRegularAlf();

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			  	  FullMtx<double> matrizsaida(dados.transfer.nglobal,11);
			  	  matrizsaida=dados.transfer.perfil();
			  	  ostringstream saida;
			  	  int numero = roundl(0);
			  	  //saida << pathPrefixoArqSaida << "PerfisPocoRadial" << "-" << kontaTempoImp
							//<<"-"<<posicMarlim<< ".dat";
			  	  saida << pathPrefixoArqSaida << "PerfisPocoRadial" << "-" << "0"
			  							<<"-"<<posicMarlim<< ".dat";
			  	  string tmp = saida.str();
			  	  ofstream escreveIni(tmp.c_str(), ios_base::out);
			  	  escreveIni<<"tempo = "<< 0<<endl;
			  	  escreveIni<<" raio (m) ;"<< " raio (pol.) ;"<<" pressao (kgf/cm2) ;"<<" vazao total (sm3/d) ;"<<
			  			  " vazao de oleo (sm3/d) ;"<<" vazao de gas (sm3/d) ;"<<" vazao de agua (sm3/d) ;"<<
						  "saturacao de liquido (-) ;"<<" saturacao de agua (-) ;"<<" fracao volumetrica de gas homogeneo (-) ;"
						  <<" BSW (-) ;"<<endl;
			  	  escreveIni << matrizsaida;
			  	  escreveIni.close();

				///////////////////////////////////////////////////////////////////////////////////////////////////////////////

				 */

		//}
	//}
}

void solverPoro::avancoSW(double delt){
	int indsra;
	reinicia=0;

	int ind;
	double raz;
	double inf;
	double sup;

	lAcopTot=0.;
	if(dados.acop==1){
		for(int i=0;i<malha.nele;i++){
			   for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						lAcopTot+=malha.mlh2d[i].cel2D.sFaceMod[j];
					}
			    }
			   }
		}

		/*indrazPW(ind, raz);
		inf = dados.pW.val[ind];
		if (ind < dados.pW.nserie - 1){
			sup = dados.pW.val[ind + 1];
			dados.transfer.Pint = (inf * raz + (1 - raz) * sup);
		}
		else
			dados.transfer.Pint = dados.pW.val[ind];
		for(int i=0;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].Pint=dados.transfer.Pint;
		}

		indraz(ind, raz,dados.tempoSatAPoc,dados.nsatAPoc);
		inf = dados.satAPoc[ind];
		if (ind < dados.nsatAPoc - 1){
			sup = dados.satAPoc[ind + 1];
			sWPoc = (inf * raz + (1 - raz) * sup);
		}
		else
			sWPoc = dados.satAPoc[ind];
		dados.transfer.sWPoc=sWPoc;
		for(int i=0; i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWPoc=sWPoc;*/
	}

	for(int i=0;i<malha.nele;i++){
		 for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
		    if(malha.mlh2d[i].kvizinho[j]<0){
		    	malha.mlh2d[i].atualizaCC(j);
		    }
		 }
	}
	if(dados.acop==1){
		   dados.qTotalW=-dados.transfer.celula[dados.transfer.ncel-2].QwcamadaR;
		   dados.qTotalO=-dados.transfer.celula[dados.transfer.ncel-2].QocamadaR;
		   dados.qTotalG=-dados.transfer.celula[dados.transfer.ncel-2].QgcamadaR;
		   dados.qTotal=(dados.qTotalW+dados.qTotalO+dados.qTotalG);
		   for(int i=0;i<malha.nele;i++){
			   for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						malha.mlh2d[i].ccPVN[j]=1.*(malha.mlh2d[i].rhoF[j]*(dados.qTotalO+dados.qTotalG)+
								0.*malha.mlh2d[i].rhogF[j]*dados.qTotalG+malha.mlh2d[i].rhoaF[j]*dados.qTotalW)/lAcopTot;
						malha.mlh2d[i].CC.satAcop=dados.transfer.celula[dados.transfer.ncel-2].sW;
					}
			    }
			   }
		}
	}

/*	delt=defineDeltPoroso();

	for(int i=0; i<malha.nele;i++){
		double dtaux=0.8*malha.mlh2d[i].cflA();
		if(delt>dtaux)delt=dtaux;
	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++){
			double dtaux=0.8*dados.transfer.celula[i].cflA();
			if(delt>dtaux)delt=dtaux;
		}
	}*/
	for(int i=0; i<malha.nele;i++)malha.mlh2d[i].dt=delt;
	if(dados.acop==1){
		dados.transfer.dt=delt;
		for(int i=0;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].dt=delt;
			dados.transfer.celula[i].dtSL=delt;
			dados.transfer.celula[i].dtSW=delt;
		}
	}


	for(int i=0; i<malha.nele;i++){
		malha.mlh2d[i].evoluiSW(reinicia);
	}
	if(dados.acop==1){
		dados.transfer.sWRes=0.;
		for(int i=0;i<malha.nele;i++){
			   for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
			    if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						dados.transfer.sWRes+=malha.mlh2d[i].swF[j]*malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot;
					}
			    }
			   }
		}
		for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWRes=dados.transfer.sWRes;
		for(int i=0;i<dados.transfer.ncel;i++){
			if(i>=dados.transfer.ncel-1 && dados.transfer.celula[dados.transfer.ncel-2].QcamadaL>0.){
				dados.transfer.celula[i].sL=dados.transfer.sLRes;
				dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
				dados.transfer.celula[i].sW=dados.transfer.sWRes;
				dados.transfer.celula[i-1].sWR=dados.transfer.sWRes;
				dados.transfer.celula[i].sLR=dados.transfer.sLRes;
				dados.transfer.celula[i].sWR=dados.transfer.sWRes;
				dados.transfer.celula[i].reiniciaSL=0;
				dados.transfer.celula[i].reiniciaSW=0;
			}
			else if(i==0 && dados.transfer.celula[1].QcamadaR<0.){
				dados.transfer.celula[i].sL=dados.transfer.sLRes;
				dados.transfer.celula[i].sW=dados.transfer.sWPoc;
				dados.transfer.celula[i].sLR=dados.transfer.sLRes;
				dados.transfer.celula[i].sWR=dados.transfer.sWPoc;
				dados.transfer.celula[i].reiniciaSL=0;
				dados.transfer.celula[i].reiniciaSW=0;
			}
			else{
				dados.transfer.celula[i].evoluiSW(reinicia, 0, 0);
				if(i>0){
					dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
					dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
					dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
					dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
				}
				else{
					dados.transfer.celula[i].sLL=dados.transfer.celula[i].sL;
					dados.transfer.celula[i].sWL=dados.transfer.celula[i].sW;
				}
			}
		}
	}

	for(int i=0;i<malha.nele;i++){
		if(malha.mlh2d[i].reiniciaSW<0){
			reinicia=-1;
		}
		malha.mlh2d[i].reiniciaSW=0;
	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++){
			if(dados.transfer.celula[i].reiniciaSL<0 || dados.transfer.celula[i].reiniciaSW<0){
				reinicia=-1;
			}
			dados.transfer.celula[i].reiniciaSW=0;
		}
	}
}

void solverPoro::reavaliaDT(double& delt){
	for(int i=0;i<malha.nele;i++){
			if(delt>malha.mlh2d[i].dtSW)delt=malha.mlh2d[i].dtSW;
	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++){
			if(delt>dados.transfer.celula[i].dtSW)delt=dados.transfer.celula[i].dtSW;
		}
	}
	/*for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].dt=delt;
		malha.mlh2d[i].dtSW=delt;
	}
	if(dados.acop==1){
		dados.transfer.dt=delt;
		for(int i=0;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].dt=delt;
			dados.transfer.celula[i].dtSL=delt;
			dados.transfer.celula[i].dtSW=delt;
		}
	}
	dt=delt;*/
}

void solverPoro::reiniciaEvoluiSW(double delt){
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].dt=delt;
		malha.mlh2d[i].dtSW=delt;
	}
	if(dados.acop==1){
		dados.transfer.dt=delt;
		for(int i=0;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].dt=delt;
			dados.transfer.celula[i].dtSL=delt;
			dados.transfer.celula[i].dtSW=delt;
		}
	}
	dt=delt;
}

void solverPoro::avancoSWcorrec(){
	/*for(int i=0;i<malha.nele;i++){
		//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
		if(delt>malha.mlh2d[i].dtSW)delt=malha.mlh2d[i].dtSW;
	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++){
			//if(dt>celula[i].dtSL)dt=celula[i].dtSL;
			if(delt>dados.transfer.celula[i].dtSW)delt=dados.transfer.celula[i].dtSW;
		}
	}
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].dt=delt;
		malha.mlh2d[i].dtSW=delt;
	}
	if(dados.acop==1){
		dados.transfer.dt=delt;
		for(int i=0;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].dt=delt;
			dados.transfer.celula[i].dtSL=delt;
			dados.transfer.celula[i].dtSW=delt;
		}
	}*/

	for(int i=0; i<malha.nele;i++){
		malha.mlh2d[i].evoluiSW(reinicia);
	}
	if(dados.acop==1){
		dados.transfer.sWRes=0.;
		for(int i=0;i<malha.nele;i++){
	    	for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
	    		if(malha.mlh2d[i].kvizinho[j]<0){
					int kcc=0;
					while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
					if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
						dados.transfer.sWRes+=malha.mlh2d[i].swF[j]*malha.mlh2d[i].cel2D.sFaceMod[j]/lAcopTot;
					}
	    		}
	    	}
		}
		for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].sWRes=dados.transfer.sWRes;
		for(int i=0;i<dados.transfer.ncel;i++){
			if(i>=dados.transfer.ncel-1){
				dados.transfer.celula[i].sL=dados.transfer.sLRes;
				dados.transfer.celula[i-1].sLR=dados.transfer.sLRes;
				dados.transfer.celula[i].sW=dados.transfer.sWRes;
				dados.transfer.celula[i-1].sWR=dados.transfer.sWRes;
				dados.transfer.celula[i].sLR=dados.transfer.sLRes;
				dados.transfer.celula[i].sWR=dados.transfer.sWRes;
				dados.transfer.celula[i].reiniciaSL=0;
				dados.transfer.celula[i].reiniciaSW=0;
			}
			else{
				dados.transfer.celula[i].evoluiSW(reinicia, 0, 0);
				if(i>0){
					dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
					dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
					dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
					dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
				}
				else{
					dados.transfer.celula[i].sLL=dados.transfer.celula[i].sL;
					dados.transfer.celula[i].sWL=dados.transfer.celula[i].sW;
				}
			}
		}
	}
}

void solverPoro::avancoPressao(){
	//double delt=vdelt;
	double norma;
	norma=1e5;
	int itera;
	int indsra;
	itera=0;
	double minP=100000000000.;
	while((norma>(1.e-5*dados.temp.relax) || itera<3) && (itera<80)){
#pragma omp parallel for num_threads(dados.nthrdLocal)
		   for(int i=0; i<malha.nele;i++){
			   for(int j=0; j<malha.mlh2d[i].cel2D.dim;j++){
		    			malha.mlh2d[i].cel2D.gradGreenPI[j]=malha.mlh2d[i].cel2D.gradGreenP[j];
		    			malha.mlh2d[i].cel2D.gradGreenSwI[j]=malha.mlh2d[i].cel2D.gradGreenSw[j];
		    			malha.mlh2d[i].cel2D.gradGreenZdatumI[j]=malha.mlh2d[i].cel2D.gradGreenZdatum[j];
		    			malha.mlh2d[i].cel2D.gradGreenAZdatumI[j]=malha.mlh2d[i].cel2D.gradGreenAZdatum[j];
		    			malha.mlh2d[i].cel2D.gradGreenPcAOI[j]=malha.mlh2d[i].cel2D.gradGreenPcAO[j];
		    		}
		    }
#pragma omp parallel for num_threads(dados.nthrdLocal)
			for(int i=0; i<malha.nele;i++){
					malha.mlh2d[i].calcGradGreen();
			}
#pragma omp parallel for num_threads(dados.nthrdLocal)
			for(int i=0; i<malha.nele;i++) malha.mlh2d[i].GeraLocal(dados.temp.relax);
			indsra=0;
			for(int i=0;i<malha.nele;i++){
				int dimNoZero=malha.mlh2d[i].cel2D.indVizCres.size();
				for(int j=0;j<dimNoZero;j++)
						matglob.sra[indsra+j]=malha.mlh2d[i].local.mx[0][j];
				indsra+=dimNoZero;
				termolivre[i]=malha.mlh2d[i].TL[0];
			}
			double eps=0.000001;
			int iter=malha.nele;
			int retorna;
#pragma omp parallel for num_threads(dados.nthrdLocal)
			for(int i=0;i<malha.nele;i++)malha.vecSolv[i]=malha.mlh2d[i].cel2D.presC;
			if(dados.solverMat==0)retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
			else if(dados.solverMat==1)retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,precn,40+0*malha.nele-1);
			else retorna=matglob.BiCGStab(malha.vecSolv,termolivre,eps,iter,precn);
			//if(retorna==1)break;
			norma=0.;
			double val;
			double val2;
			int val3;
			for(int i=0;i<malha.nele;i++){
				malha.mlh2d[i].cel2D.presCI=malha.mlh2d[i].cel2D.presC;
				malha.mlh2d[i].cel2D.presC=malha.vecSolv[i];
				if(minP>malha.mlh2d[i].cel2D.presC)minP=malha.mlh2d[i].cel2D.presC;
				norma+=fabsl(malha.mlh2d[i].cel2D.presC-malha.mlh2d[i].cel2D.presCI);///malha.mlh2d[i].cel2D.presCI;
				for(int j=0;j<malha.mlh2d[i].cel2D.nvert;j++){
					if(malha.mlh2d[i].kvizinho[j]<0){
						int diri=0;
						int rich=0;
						int vn=0;
						int kcc=0;
						int acoplado=0;
						malha.mlh2d[i].tipoCC(j, diri, vn, rich, acoplado, kcc);
						if(diri==1){
							val=malha.mlh2d[i].cel2D.presC;
							val2=malha.mlh2d[i].Qtotal[j];
							val3=0;
						}
					}
				}
			}
			norma/=malha.nele;
			if(dados.acop==1){

			    double presresAux=dados.transfer.presRes2;
			    dados.transfer.presRes2=0.;
				for(int i=0;i<malha.nele;i++){
				    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    	if(malha.mlh2d[i].kvizinho[j]<0){
							int kcc=0;
							while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
							if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									dados.transfer.presRes2+=malha.mlh2d[i].presF[j]*malha.mlh2d[i].cel2D.sFaceMod[j];
									malha.mlh2d[i].CC.satAcop=dados.transfer.celula[dados.transfer.ncel-2].sW;
							}
				    	}
				    }
				}
				dados.transfer.presRes2/=lAcopTot;
				for(int i=0;i<dados.transfer.ncel;i++){
						dados.transfer.celula[i].presRes2=dados.transfer.presRes2;
				}
				double erro=dados.transfer.transtrans(dados.espessuraRes);
				dados.flup.BSW=dados.transfer.flup.BSW;
				norma+=fabsl(erro);

			    dados.qTotalW=-dados.transfer.celula[dados.transfer.ncel-2].QwcamadaR;
			    dados.qTotalO=-dados.transfer.celula[dados.transfer.ncel-2].QocamadaR;
			    dados.qTotalG=-dados.transfer.celula[dados.transfer.ncel-2].QgcamadaR;
			    dados.qTotal=(dados.qTotalW+dados.qTotalO+dados.qTotalG);
				for(int i=0;i<malha.nele;i++){
				    for(int j=0; j<malha.mlh2d[i].cel2D.nvert;j++){
				    	if(malha.mlh2d[i].kvizinho[j]<0){
								int kcc=0;
								while(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]!=dados.CC.rotuloAcop) kcc++;
								if(kcc<1 && malha.mlh2d[i].cel2D.ccFace[j]==dados.CC.rotuloAcop){
									//malha.mlh2d[i].ccPVN[j]=dados.qTotal/lAcopTot;
									malha.mlh2d[i].ccPVN[j]=1.*(malha.mlh2d[i].rhoF[j]*(dados.qTotalO+dados.qTotalG)+
											0.*malha.mlh2d[i].rhogF[j]*dados.qTotalG+malha.mlh2d[i].rhoaF[j]*dados.qTotalW)/lAcopTot;
								}
				    		}
				    }
				}

			  }
#pragma omp parallel for num_threads(dados.nthrdLocal)
			for(int i=0;i<malha.nele;i++){
					//malha.mlh2d[i].cel2D.vbo;
					malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].flup.MasEspoleo(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].flup.MasEspGas(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].flup.MasEspAgua(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					double tit=malha.mlh2d[i].flup.FracMassHidra(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes);
					malha.mlh2d[i].cel2D.alfC=(tit/malha.mlh2d[i].cel2D.rhogP)/((tit/malha.mlh2d[i].cel2D.rhogP)+((1.-tit)/malha.mlh2d[i].cel2D.rhoP));
					malha.mlh2d[i].cel2D.pcAO=malha.mlh2d[i].interpolaTabela(malha.mlh2d[i].pcOACel.npont, malha.mlh2d[i].cel2D.sWC, malha.mlh2d[i].pcOACel.satW,
							malha.mlh2d[i].pcOACel.presCapOW);
			}

			   itera++;

		}

		if(dados.acop==1){
			dados.transfer.transtransAux(dados.espessuraRes);
		}
		if(dados.flashCompleto==2){
			for(int i=0;i<malha.nele;i++){
				if(malha.mlh2d[i].flup.dCalculatedBeta<0. || malha.mlh2d[i].flup.dCalculatedBeta>1.)
					malha.mlh2d[i].flup.atualizaPropComp(malha.mlh2d[i].cel2D.presC,  malha.mlh2d[i].tRes,-1,NULL,NULL,0);
				else
					malha.mlh2d[i].flup.atualizaPropComp(malha.mlh2d[i].cel2D.presC, malha.mlh2d[i].tRes,
							malha.mlh2d[i].flup.dCalculatedBeta,malha.mlh2d[i].flup.oCalculatedLiqComposition,
							malha.mlh2d[i].flup.oCalculatedVapComposition,0);
			}
			if(dados.acop==1){
				for(int i=0;i<dados.transfer.ncel;i++){
					if(dados.transfer.celula[i].flup.dCalculatedBeta<0. || dados.transfer.celula[i].flup.dCalculatedBeta>1.)
						dados.transfer.celula[i].flup.atualizaPropComp(dados.transfer.celula[i].Pcamada,  dados.transfer.tRes,-1,NULL,NULL,0);
					else
						dados.transfer.celula[i].flup.atualizaPropComp(dados.transfer.celula[i].Pcamada, dados.transfer.tRes,
								dados.transfer.celula[i].flup.dCalculatedBeta,dados.transfer.celula[i].flup.oCalculatedLiqComposition,
								dados.transfer.celula[i].flup.oCalculatedVapComposition,0);
				}
			}
		}

		if(dados.acop==1)dados.transfer.atualizaIni();
#pragma omp parallel for num_threads(dados.nthrdLocal)
		for(int i=0;i<malha.nele;i++){
			malha.mlh2d[i].cel2D.presC0=malha.mlh2d[i].cel2D.presC;
			malha.mlh2d[i].cel2D.sWI=malha.mlh2d[i].cel2D.sWC;

			malha.mlh2d[i].cel2D.rhoP0=malha.mlh2d[i].cel2D.rhoP;
			malha.mlh2d[i].cel2D.rhogP0=malha.mlh2d[i].cel2D.rhogP;
			malha.mlh2d[i].cel2D.rhoaP0=malha.mlh2d[i].cel2D.rhoaP;
			malha.mlh2d[i].cel2D.alfC0=malha.mlh2d[i].cel2D.alfC;


		}

		//parta colocar no MR3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :
		(*vg1dSP).tempo=(*vg1dSP).lixo5;
		if (((*vg1dSP).tempo > 1e-15 && (*vg1dSP).tempo <= dados.temp.tempoImp[kontaTempo] && (*vg1dSP).tempo + dt >= dados.temp.tempoImp[kontaTempo])) {
				malha.imprime((*vg1dSP).tempo);
				imprimeMalhaRegular(minP);
				imprimeMalhaRegularSW();
				imprimeMalhaRegularAlf();
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			  	  FullMtx<double> matrizsaida(dados.transfer.nglobal,11);
			  	  matrizsaida=dados.transfer.perfil();
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

				///////////////////////////////////////////////////////////////////////////////////////////////////////////////
				kontaTempo++;
				if (kontaTempo >= dados.temp.parserieImp) kontaTempo--;
		}
}

void solverPoro::FeiticoDoTempo(){
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].cel2D.presC=malha.mlh2d[i].cel2D.presC0;
		malha.mlh2d[i].cel2D.sWC=malha.mlh2d[i].cel2D.sWI;

		malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].cel2D.rhoP0;
		malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].cel2D.rhogP0;
		malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].cel2D.rhoaP0;
		malha.mlh2d[i].cel2D.alfC=malha.mlh2d[i].cel2D.alfC0;

	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].FeiticoDoTempo();
		dados.transfer.celula[0].PcamadaL=dados.transfer.celula[0].Pcamada;
		dados.transfer.celula[0].sLL=dados.transfer.celula[0].sL;
		dados.transfer.celula[0].sWL=dados.transfer.celula[0].sW;
		dados.transfer.celula[0].QwcamadaL=dados.transfer.celula[0].QwcamadaR;
		dados.transfer.celula[0].QocamadaL=dados.transfer.celula[0].QocamadaR;
		dados.transfer.celula[0].QgcamadaL=dados.transfer.celula[0].QgcamadaR;
		for(int i=1;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].PcamadaL=dados.transfer.celula[i-1].Pcamada;
			dados.transfer.celula[i-1].PcamadaR=dados.transfer.celula[i].Pcamada;
			dados.transfer.celula[i].sLL=dados.transfer.celula[i-1].sL;
			dados.transfer.celula[i].sWL=dados.transfer.celula[i-1].sW;
			dados.transfer.celula[i-1].sLR=dados.transfer.celula[i].sL;
			dados.transfer.celula[i-1].sWR=dados.transfer.celula[i].sW;
			dados.transfer.celula[i].QwcamadaL=dados.transfer.celula[i-1].QwcamadaR;
			dados.transfer.celula[i].QocamadaL=dados.transfer.celula[i-1].QocamadaR;
			dados.transfer.celula[i].QgcamadaL=dados.transfer.celula[i-1].QgcamadaR;
		}
		dados.transfer.celula[dados.transfer.ncel-1].PcamadaR=dados.transfer.celula[dados.transfer.ncel-1].Pcamada;
		dados.transfer.celula[dados.transfer.ncel-1].sLR=dados.transfer.celula[dados.transfer.ncel-1].sL;
		dados.transfer.celula[dados.transfer.ncel-1].sWR=dados.transfer.celula[dados.transfer.ncel-1].sW;
	}
}

void solverPoro::FeiticoDoTempoSW(){

	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].cel2D.sWC=malha.mlh2d[i].cel2D.sWI;
	}
	if(dados.acop==1){
		for(int j=0;j<dados.transfer.ncel;j++){
			dados.transfer.celula[j].sW=dados.transfer.celula[j].sWini;
		}
		dados.transfer.celula[0].sWL=dados.transfer.celula[0].sW;
		for(int j=1;j<dados.transfer.ncel;j++){
			dados.transfer.celula[j].sWL=dados.transfer.celula[j-1].sW;
			dados.transfer.celula[j-1].sWR=dados.transfer.celula[j].sW;
		}
		dados.transfer.celula[dados.transfer.ncel-1].sWR=dados.transfer.celula[dados.transfer.ncel-1].sW;
	}
}

void solverPoro::FeiticoDoTempoPQ(){
	for(int i=0;i<malha.nele;i++){
		malha.mlh2d[i].cel2D.presC=malha.mlh2d[i].cel2D.presC0;
		malha.mlh2d[i].cel2D.rhoP=malha.mlh2d[i].cel2D.rhoP0;
		malha.mlh2d[i].cel2D.rhogP=malha.mlh2d[i].cel2D.rhogP0;
		malha.mlh2d[i].cel2D.rhoaP=malha.mlh2d[i].cel2D.rhoaP0;
		malha.mlh2d[i].cel2D.alfC=malha.mlh2d[i].cel2D.alfC0;

	}
	if(dados.acop==1){
		for(int i=0;i<dados.transfer.ncel;i++)dados.transfer.celula[i].FeiticoDoTempoPQ();
		dados.transfer.celula[0].PcamadaL=dados.transfer.celula[0].Pcamada;
		dados.transfer.celula[0].QwcamadaL=dados.transfer.celula[0].QwcamadaR;
		dados.transfer.celula[0].QocamadaL=dados.transfer.celula[0].QocamadaR;
		dados.transfer.celula[0].QgcamadaL=dados.transfer.celula[0].QgcamadaR;
		for(int i=1;i<dados.transfer.ncel;i++){
			dados.transfer.celula[i].PcamadaL=dados.transfer.celula[i-1].Pcamada;
			dados.transfer.celula[i-1].PcamadaR=dados.transfer.celula[i].Pcamada;
			dados.transfer.celula[i].QwcamadaL=dados.transfer.celula[i-1].QwcamadaR;
			dados.transfer.celula[i].QocamadaL=dados.transfer.celula[i-1].QocamadaR;
			dados.transfer.celula[i].QgcamadaL=dados.transfer.celula[i-1].QgcamadaR;
		}
		dados.transfer.celula[dados.transfer.ncel-1].PcamadaR=dados.transfer.celula[dados.transfer.ncel-1].Pcamada;
	}
}


