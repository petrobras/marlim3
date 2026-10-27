/*
 * solverPoisson.cpp
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */
#include "dados1PoissonAxiSim.h"
#include "solverPoissonAxiSim.h"

solverPAxiSim::solverPAxiSim(varGlob1D *Vvg1dSP, string nomeArquivoEntrada, double* vresGlob,
                 double* vTint,double* vdiamI
				 , double* dxCel, int vncel, double* vgeoterm, double vtempProd) :
				 //dados(nomeArquivoEntrada, vresGlob, vTint, vdiamI,dxCel, vncel,vgeoterm),
                 //termolivre(dados->nele), matglob(dados->nele, dados->noZero, dados->vecsra, dados->veclm, dados->vecfnz)
				 termolivre(1), matglob(1, 1){

	dados = new dadosPAxiSim(nomeArquivoEntrada, vresGlob, vTint, vdiamI, dxCel, vncel, vgeoterm);
	termolivre=Vcr<double>(dados->nele);
	matglob=SparseMtx<double>(dados->nele, dados->noZero, dados->vecsra, dados->veclm, dados->vecfnz);

    vg1dSP = Vvg1dSP;
    malha = malha2dAxiSim(vg1dSP, dados->xcoor, dados->noEle, dados->tipo, dados->atributo, dados->nVert,
    		dados->nele, dados->nno, dados->temp.perm, dados->temp.trans,dados->acopDirichlet);
    tempProd=vtempProd;
    int kontaEle=0;
    int kontaZona=0;
    for (int i = 0; i < dados->ncelPoco; i++){
    	for(int j=0;j<dados->ndR;j++){
            double xc = 2*malha.mlh2d[kontaEle].cel2D.centroideElem[0];
            double yc = malha.mlh2d[kontaEle].cel2D.centroideElem[1];
            int indx;
            int indy;
    		for(int iD=0;iD<dados->ncorte; iD++){
    			indx=iD;
    			double dmenor;
    			double dmaior=dados->corte[0].diam[iD];
    			if(iD==0)dmenor=dados->corte[0].a;
    			else dmenor=dados->corte[0].diam[iD-1];
    			if(xc>=dmenor && xc<dmaior)break;
    		}
    		for(int iP=0;iP<dados->nzonas; iP++){
    			indy=iP;
    			double hmenor;
    			double hmaior=dados->zonaProf[iP];
    			if(iP==0)hmenor=0;
    			else hmenor=dados->zonaProf[iP-1];
    			if(yc>=hmenor && yc<hmaior)break;
    		}
    		malha.mlh2d[kontaEle].CC = dados->CC;
    		int imat=dados->corte[indy].indmat[indx];
            malha.mlh2d[kontaEle].cel2D.cond = dados->mat[imat].cond;
            malha.mlh2d[kontaEle].cel2D.cp =  dados->mat[imat].cp;
            malha.mlh2d[kontaEle].cel2D.rho =  dados->mat[imat].rho;
            malha.mlh2d[kontaEle].cel2D.tempC=dados->perfiGeoterm[i];
            malha.mlh2d[kontaEle].cel2D.tempC0=dados->perfiGeoterm[i];
            for(int iface=0; iface<dados->nVert; iface++){
            	if( malha.mlh2d[kontaEle].cel2D.indFace[iface]<0){
            		if(malha.mlh2d[kontaEle].cel2D.ccFace[iface]==3){
            			malha.mlh2d[kontaEle].CC.ccDir[0].val[0]=dados->CC.ccDir[0].val[0]=dados->perfiGeoterm[i];
            		}
            		else if(malha.mlh2d[kontaEle].cel2D.ccFace[iface]==1){
            			malha.mlh2d[kontaEle].CC.ccRic[0].valAmb[0]=dados->CC.ccRic[0].valAmb[0]=dados->tAmb;
            			malha.mlh2d[kontaEle].CC.ccRic[0].hAmb[0]=dados->CC.ccRic[0].hAmb[0]=dados->hE;
            		}
            		else if( malha.mlh2d[kontaEle].cel2D.ccFace[iface]==2){
            			malha.mlh2d[kontaEle].CC.ccRic[0].valAmb[0]=dados->CC.ccRic[1].valAmb[0]=dados->perfiGeoterm[i];
            			int camFim=dados->corte[0].ncam-1;
            			imat=dados->corte[indy].indmat[camFim];
            			double resFim=ResForm(tempProd, dados->corte[0].diam[camFim],
            					dados->mat[imat].cond, dados->mat[imat].rho, dados->mat[imat].cp);
            			malha.mlh2d[kontaEle].CC.ccRic[0].hAmb[0]=dados->CC.ccRic[1].hAmb[0]=1 / (resFim);
            		}
            	}
            }
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[kontaEle].kvizinho[j] < 0) {
                    malha.mlh2d[kontaEle].atualizaCC(j);
                }
            }
            kontaEle++;
    	}
    }
    /*for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }*/

    int indsra = 0;
    for (int i = 0; i < malha.nele; i++) {
        int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
        matglob.fnz[i] = indsra;
        for (int j = 0; j < dimNoZero; j++) {
            int ind = malha.mlh2d[i].cel2D.indVizCres[j];
            matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            matglob.clm[indsra + j] = ind;
        }
        indsra += dimNoZero;
        termolivre[i] = malha.mlh2d[i].TL[0];
    }
    matglob.fnz[malha.nele] = dados->noZero;

    if (dados->rankLU >= 0) {
        matglob.rank = dados->rankLU;
        if (dados->rankLU > 0) {
            matglob.geraILUrank();
            matglob.sraILU = new double[matglob.noZeroIlu];
        }
        matglob.colorido = dados->colore;
        if (dados->colore)
            matglob.geraILUcor();
    }
    precn = 1;
    if (dados->rankLU >= 0)
        precn = 4;

    if (dados->acop == 1) {
        for(int iPoc=0;iPoc<dados->ncelPoco;iPoc++){
        	//dados->tInt[iPoc] = dados->perfiGeoterm[iPoc];
        	dados->qAcop[iPoc] = 0.;
        }
        	for (int i = 0; i < dados->CC.nRic; i++) {
        		for (int j = 0; j < dados->CC.ccRic[i].nserie; j++) {
        			dados->CC.ccRic[i].valAmb[j] = dados->tAmb;
        			dados->CC.ccRic[i].hAmb[j] = dados->hE;
        		}
        	}
        	for (int i = 0; i < malha.nele; i++) {
        		for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
        			if (malha.mlh2d[i].kvizinho[j] < 0) {
        				int kcc = 0;
        				while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
        					kcc++;
        				if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					malha.mlh2d[i].ccHR[j] = 1 / dados->resGlob[iPoc];
        					double raio;
        					if(j==0 || j==3)raio=malha.mlh2d[i].cel2D.coordVert[0][0];
        					else if(j==1 || j==2)raio=malha.mlh2d[i].cel2D.coordVert[1][0];
        					double delRaio=malha.mlh2d[i].cel2D.sFace[j][1];
        					double dProf=malha.mlh2d[i].cel2D.sFace[j][0];
        					double areaR=M_PI*((raio+delRaio)*(raio+delRaio)-raio*raio);
        					double areaP=2.*M_PI*raio*dProf;
        					double areaMod=fabs(areaR)+fabs(areaP);
        					//malha.mlh2d[i].ccHR[j]/=areaMod;
        					malha.mlh2d[i].ccHR[j]/=(2.*M_PI*raio);
        					if(dados->acopDirichlet==0)
        					dados->qAcop[iPoc] = (dados->tInt[iPoc] - malha.mlh2d[i].cel2D.tempC) / (dados->resGlob[iPoc]);
        					else dados->qAcop[iPoc] =malha.mlh2d[i].fluxTface[j][0]/fabs(dProf);
        				}
        			}
        		}
        	}
        	for (int i = 0; i < malha.nele; i++) {
        		for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
        			if (malha.mlh2d[i].kvizinho[j] < 0) {
        				int kcc = 0;
        				while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
        					kcc++;
        				if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					if(dados->acopDirichlet==0)
        					malha.mlh2d[i].ccTambR[j] = dados->tInt[iPoc];
        					else malha.mlh2d[i].ccTD[j]=dados->tInt[iPoc];
        				}
        			}
        		}
        	}
    }
    kontaTempo = 0;
}

solverPAxiSim::solverPAxiSim() :
		//dados(),
				termolivre(1), matglob(1, 1) {
	dados = new dadosPAxiSim();
    vg1dSP = 0;
    kontaTempo = 0;
    tempProd=0.;
    malha = malha2dAxiSim();
    precn = 1;
}

solverPAxiSim::solverPAxiSim(const solverPAxiSim &vPoisson) :
		termolivre(1), matglob(1, 1)
		/*dados(vPoisson.dados->entrada, vPoisson.dados->resGlob,
             vPoisson.dados->tInt, vPoisson.dados->diamInt,
			 vPoisson.dados->dxPoco,vPoisson.dados->ncelPoco, vPoisson.dados->perfiGeoterm),
             termolivre(dados->nele),
			 matglob(dados->nele, dados->noZero, dados->vecsra, dados->veclm, dados->vecfnz) */{
	dados= new dadosPAxiSim(vPoisson.dados->entrada, vPoisson.dados->resGlob,
	             vPoisson.dados->tInt, vPoisson.dados->diamInt,
				 vPoisson.dados->dxPoco,vPoisson.dados->ncelPoco, vPoisson.dados->perfiGeoterm);
	termolivre=Vcr<double>(dados->nele);
	matglob=SparseMtx<double>(dados->nele, dados->noZero, dados->vecsra, dados->veclm, dados->vecfnz);
	vg1dSP = vPoisson.vg1dSP;
    malha = malha2dAxiSim(vg1dSP, dados->xcoor, dados->noEle, dados->tipo, dados->atributo, dados->nVert,
    		dados->nele, dados->nno, dados->temp.perm, dados->temp.trans, dados->acopDirichlet);

    tempProd=vPoisson.tempProd;
    int kontaEle=0;
    int kontaZona=0;
    for (int i = 0; i < dados->ncelPoco; i++){
    	for(int j=0;j<dados->ndR;j++){
            double xc = 2*malha.mlh2d[kontaEle].cel2D.centroideElem[0];
            double yc = malha.mlh2d[kontaEle].cel2D.centroideElem[1];
            int indx;
            int indy;
    		for(int iD=0;iD<dados->ncorte; iD++){
    			indx=iD;
    			double dmenor;
    			double dmaior=dados->corte[0].diam[iD];
    			if(iD==0)dmenor=dados->corte[0].a;
    			else dmenor=dados->corte[0].diam[iD-1];
    			if(xc>=dmenor && xc<dmaior)break;
    		}
    		for(int iP=0;iP<dados->nzonas; iP++){
    			indy=iP;
    			double hmenor;
    			double hmaior=dados->zonaProf[iP];
    			if(iP==0)hmenor=0;
    			else hmenor=dados->zonaProf[iP-1];
    			if(yc>=hmenor && yc<hmaior)break;
    		}
    		malha.mlh2d[kontaEle].CC = dados->CC;
    		int imat=dados->corte[indy].indmat[indx];
            malha.mlh2d[kontaEle].cel2D.cond = dados->mat[imat].cond;
            malha.mlh2d[kontaEle].cel2D.cp =  dados->mat[imat].cp;
            malha.mlh2d[kontaEle].cel2D.rho =  dados->mat[imat].rho;
            malha.mlh2d[kontaEle].cel2D.tempC=dados->perfiGeoterm[i];
            malha.mlh2d[kontaEle].cel2D.tempC0=dados->perfiGeoterm[i];
            for(int iface=0; iface<dados->nVert; iface++){
            	if( malha.mlh2d[kontaEle].cel2D.indFace[iface]<0){
            		if(malha.mlh2d[kontaEle].cel2D.ccFace[iface]==3){
            			malha.mlh2d[kontaEle].CC.ccDir[0].val[0]=dados->CC.ccDir[0].val[0]=dados->perfiGeoterm[i];
            		}
            		else if(malha.mlh2d[kontaEle].cel2D.ccFace[iface]==1){
            			malha.mlh2d[kontaEle].CC.ccRic[0].valAmb[0]=dados->CC.ccRic[0].valAmb[0]=dados->tAmb;
            			malha.mlh2d[kontaEle].CC.ccRic[0].hAmb[0]=dados->CC.ccRic[0].hAmb[0]=dados->hE;
            		}
            		else if( malha.mlh2d[kontaEle].cel2D.ccFace[iface]==2){
            			malha.mlh2d[kontaEle].CC.ccRic[0].valAmb[0]=dados->CC.ccRic[1].valAmb[0]=dados->perfiGeoterm[i];
            			int camFim=dados->corte[0].ncam-1;
            			imat=dados->corte[indy].indmat[camFim];
            			double resFim=ResForm(tempProd, dados->corte[0].diam[camFim],
            					dados->mat[imat].cond, dados->mat[imat].rho, dados->mat[imat].cp);
            			malha.mlh2d[kontaEle].CC.ccRic[0].hAmb[0]=dados->CC.ccRic[1].hAmb[0]=1 / (resFim);
            		}
            	}
            }
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[kontaEle].kvizinho[j] < 0) {
                    malha.mlh2d[kontaEle].atualizaCC(j);
                }
            }
            kontaEle++;
    	}
    }
    /*for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }*/

    int indsra = 0;
    for (int i = 0; i < malha.nele; i++) {
        int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
        matglob.fnz[i] = indsra;
        for (int j = 0; j < dimNoZero; j++) {
            int ind = malha.mlh2d[i].cel2D.indVizCres[j];
            matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            matglob.clm[indsra + j] = ind;
        }
        indsra += dimNoZero;
        termolivre[i] = malha.mlh2d[i].TL[0];
    }
    matglob.fnz[malha.nele] = dados->noZero;

    if (dados->rankLU >= 0) {
        matglob.rank = dados->rankLU;
        if (dados->rankLU > 0) {
            matglob.geraILUrank();
            matglob.sraILU = new double[matglob.noZeroIlu];
        }
        matglob.colorido = dados->colore;
        if (dados->colore)
            matglob.geraILUcor();
    }
    precn = 1;
    if (dados->rankLU >= 0)
        precn = 4;

    if (dados->acop == 1) {

        for(int iPoc=0;iPoc<dados->ncelPoco;iPoc++){
        	dados->qAcop[iPoc] = 0.;
        }
        for (int i = 0; i < dados->CC.nRic; i++) {
            for (int j = 0; j < dados->CC.ccRic[i].nserie; j++) {
                dados->CC.ccRic[i].valAmb[j] = dados->tAmb;
                dados->CC.ccRic[i].hAmb[j] = dados->hE;
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    int kcc = 0;
                    while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                        kcc++;
                    if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
    					int iPoc=floor(i/dados->ndR);
    					malha.mlh2d[i].ccHR[j] = 1 / dados->resGlob[iPoc];
    					double raio;
    					if(j==0 || j==3)raio=malha.mlh2d[i].cel2D.coordVert[0][0];
    					else if(j==1 || j==2)raio=malha.mlh2d[i].cel2D.coordVert[1][0];
    					double delRaio=malha.mlh2d[i].cel2D.sFace[j][1];
    					double dProf=malha.mlh2d[i].cel2D.sFace[j][0];
    					double areaR=M_PI*((raio+delRaio)*(raio+delRaio)-raio*raio);
    					double areaP=2.*M_PI*raio*dProf;
    					double areaMod=fabs(areaR)+fabs(areaP);
    					malha.mlh2d[i].ccHR[j]/=areaMod;
    					if(dados->acopDirichlet==0)
    					dados->qAcop[iPoc] = (dados->tInt[iPoc] - malha.mlh2d[i].cel2D.tempC) / (dados->resGlob[iPoc]);
    					else dados->qAcop[iPoc] =malha.mlh2d[i].fluxTface[j][0]/fabs(dProf);
                    }
                }
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    int kcc = 0;
                    while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                        kcc++;
                    if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
    					int iPoc=floor(i/dados->ndR);
    					if(dados->acopDirichlet==0)
    					malha.mlh2d[i].ccTambR[j] = dados->tInt[iPoc];
    					else malha.mlh2d[i].ccTD[j]=dados->tInt[iPoc];
                    }
                }
            }
        }
    }
    kontaTempo = vPoisson.kontaTempo;
}

solverPAxiSim &solverPAxiSim::operator=(const solverPAxiSim &vPoisson) {
    if (this != &vPoisson && vPoisson.malha.nele>0) {
        vg1dSP = vPoisson.vg1dSP;
        delete dados;
        //dados = vPoisson.dados;
        dados = new dadosPAxiSim(vPoisson.dados->entrada, vPoisson.dados->resGlob,
	             vPoisson.dados->tInt, vPoisson.dados->diamInt,
				 vPoisson.dados->dxPoco,vPoisson.dados->ncelPoco, vPoisson.dados->perfiGeoterm);
        termolivre = vPoisson.termolivre;
        matglob = vPoisson.matglob;

        malha = malha2dAxiSim(vg1dSP, dados->xcoor, dados->noEle, dados->tipo, dados->atributo,
        		dados->nVert, dados->nele, dados->nno, dados->temp.perm, dados->temp.trans,
				dados->acopDirichlet);
        tempProd=vPoisson.tempProd;
        int kontaEle=0;
        int kontaZona=0;
        for (int i = 0; i < dados->ncelPoco; i++){
        	for(int j=0;j<dados->ndR;j++){
                double xc = 2*malha.mlh2d[kontaEle].cel2D.centroideElem[0];
                double yc = malha.mlh2d[kontaEle].cel2D.centroideElem[1];
                int indx;
                int indy;
        		for(int iD=0;iD<dados->ncorte; iD++){
        			indx=iD;
        			double dmenor;
        			double dmaior=dados->corte[0].diam[iD];
        			if(iD==0)dmenor=dados->corte[0].a;
        			else dmenor=dados->corte[0].diam[iD-1];
        			if(xc>=dmenor && xc<dmaior)break;
        		}
        		for(int iP=0;iP<dados->nzonas; iP++){
        			indy=iP;
        			double hmenor;
        			double hmaior=dados->zonaProf[iP];
        			if(iP==0)hmenor=0;
        			else hmenor=dados->zonaProf[iP-1];
        			if(yc>=hmenor && yc<hmaior)break;
        		}
        		malha.mlh2d[kontaEle].CC = dados->CC;
        		int imat=dados->corte[indy].indmat[indx];
                malha.mlh2d[kontaEle].cel2D.cond = dados->mat[imat].cond;
                malha.mlh2d[kontaEle].cel2D.cp =  dados->mat[imat].cp;
                malha.mlh2d[kontaEle].cel2D.rho =  dados->mat[imat].rho;
                malha.mlh2d[kontaEle].cel2D.tempC=dados->perfiGeoterm[i];
                malha.mlh2d[kontaEle].cel2D.tempC0=dados->perfiGeoterm[i];
                for(int iface=0; iface<dados->nVert; iface++){
                	if( malha.mlh2d[kontaEle].cel2D.indFace[iface]<0){
                		if(malha.mlh2d[kontaEle].cel2D.ccFace[iface]==3){
                			malha.mlh2d[kontaEle].CC.ccDir[0].val[0]=dados->CC.ccDir[0].val[0]=dados->perfiGeoterm[i];
                		}
                		else if(malha.mlh2d[kontaEle].cel2D.ccFace[iface]==1){
                			malha.mlh2d[kontaEle].CC.ccRic[0].valAmb[0]=dados->CC.ccRic[0].valAmb[0]=dados->tAmb;
                			malha.mlh2d[kontaEle].CC.ccRic[0].hAmb[0]=dados->CC.ccRic[0].hAmb[0]=dados->hE;
                		}
                		else if( malha.mlh2d[kontaEle].cel2D.ccFace[iface]==2){
                			malha.mlh2d[kontaEle].CC.ccRic[0].valAmb[0]=dados->CC.ccRic[1].valAmb[0]=dados->perfiGeoterm[i];
                			int camFim=dados->corte[0].ncam-1;
                			imat=dados->corte[indy].indmat[camFim];
                			double resFim=ResForm(tempProd, dados->corte[0].diam[camFim],
                					dados->mat[imat].cond, dados->mat[imat].rho, dados->mat[imat].cp);
                			malha.mlh2d[kontaEle].CC.ccRic[0].hAmb[0]=dados->CC.ccRic[1].hAmb[0]=1 / (resFim);
                		}
                	}
                }
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[kontaEle].kvizinho[j] < 0) {
                        malha.mlh2d[kontaEle].atualizaCC(j);
                    }
                }
                kontaEle++;
        	}
        }
        /*for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                if (malha.mlh2d[i].kvizinho[j] < 0) {
                    malha.mlh2d[i].atualizaCC(j);
                }
            }
        }*/

        int indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            matglob.fnz[i] = indsra;
            for (int j = 0; j < dimNoZero; j++) {
                int ind = malha.mlh2d[i].cel2D.indVizCres[j];
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
                matglob.clm[indsra + j] = ind;
            }
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        if (malha.nele > 0)
            matglob.fnz[malha.nele] = dados->noZero;

        if (dados->rankLU >= 0) {
            matglob.rank = dados->rankLU;
            if (dados->rankLU > 0) {
                matglob.geraILUrank();
                matglob.sraILU = new double[matglob.noZeroIlu];
            }
            matglob.colorido = dados->colore;
            if (dados->colore)
                matglob.geraILUcor();
        }
        precn = 1;
        if (dados->rankLU >= 0)
            precn = 4;

        if (dados->acop == 1) {

            //dados->qAcop = 0.;
            for (int i = 0; i < dados->CC.nRic; i++) {
                for (int j = 0; j < dados->CC.ccRic[i].nserie; j++) {
                    dados->CC.ccRic[i].valAmb[j] = dados->tAmb;
                    dados->CC.ccRic[i].hAmb[j] = dados->hE;
                }
            }
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					malha.mlh2d[i].ccHR[j] = 1 / dados->resGlob[iPoc];
        					double raio;
        					if(j==0 || j==3)raio=malha.mlh2d[i].cel2D.coordVert[0][0];
        					else if(j==1 || j==2)raio=malha.mlh2d[i].cel2D.coordVert[1][0];
        					double delRaio=malha.mlh2d[i].cel2D.sFace[j][1];
        					double dProf=malha.mlh2d[i].cel2D.sFace[j][0];
        					double areaR=M_PI*((raio+delRaio)*(raio+delRaio)-raio*raio);
        					double areaP=2.*M_PI*raio*dProf;
        					double areaMod=fabs(areaR)+fabs(areaP);
        					malha.mlh2d[i].ccHR[j]/=areaMod;
        					if(dados->acopDirichlet==0)
        					dados->qAcop[iPoc] = (dados->tInt[iPoc] - malha.mlh2d[i].cel2D.tempC) / (dados->resGlob[iPoc]);
        					else dados->qAcop[iPoc] =malha.mlh2d[i].fluxTface[j][0]/fabs(dProf);
                        }
                    }
                }
            }
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					if(dados->acopDirichlet==0)
        					malha.mlh2d[i].ccTambR[j] = dados->tInt[iPoc];
        					else malha.mlh2d[i].ccTD[j]=dados->tInt[iPoc];
                        }
                    }
                }
            }
        }
        kontaTempo = vPoisson.kontaTempo;
    }

    return *this;
}


solverPAxiSim::~solverPAxiSim() {
    delete dados;
}

double solverPAxiSim::ResForm(double tempprod, double diaExt,double condform, double rhoform, double cpform) {

    double difusForm = condform / (rhoform * cpform);
    double tadim = 4. * tempprod * 86400 * difusForm / (diaExt * diaExt);
    double temperadim;
    if (tadim <= 1.)
        temperadim = 1.1281 * sqrt(tadim) * (1. - 0.3 * sqrt(tadim));
    else if (tadim <= 4.5)
        temperadim = 0.432 + 0.4792 * tadim - 0.127 * tadim * tadim + 0.0201 * tadim * tadim * tadim - 0.0013 * pow(tadim, 4.);
    else
        temperadim = (0.4063 + 0.5 * log(tadim)) * (1. + 0.6 / tadim);

    return temperadim / (2. * M_PI * condform);
}

void solverPAxiSim::indrazT(int &ind, double &raz) {

    for (int i = 0; i <= dados->temp.parserie - 1; i++) {
        if (i < dados->temp.parserie - 1) {
            if ((*vg1dSP).lixo5 >= dados->temp.tempo[i] && (*vg1dSP).lixo5 < dados->temp.tempo[i + 1]) {
                ind = i;
                raz = 1 - ((*vg1dSP).lixo5 - dados->temp.tempo[i]) / (dados->temp.tempo[i + 1] - dados->temp.tempo[i]);
                break;
            }
        } else if (i == dados->temp.parserie - 1) {
            ind = i;
            raz = 1;
        }
    }
}

void solverPAxiSim::permanentePoisson() {
    double norma = 1e5;
    int itera = 0;
    int indsra;
    int diverge = 0;
    double norma0 = norma;
    while (norma > 1e-5 && diverge == 0) {
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.dim; j++) {
                malha.mlh2d[i].cel2D.gradGreenTI[j] = malha.mlh2d[i].cel2D.gradGreenT[j];
            }
        }
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].calcGradGreen();
        }
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].GeraLocal();
        }
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].cel2D.tempC0 = malha.mlh2d[i].cel2D.tempCI = malha.mlh2d[i].cel2D.tempC;
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh2d[i].cel2D.tempC;
		double eps=0.00001;
		int iter=malha.nele;
		int retorna;
		if (dados->solverMat == 0)
			retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,1,40);
		else if (dados->solverMat == 1)
			retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,1,40);
		else
			retorna=matglob.BiCGStab(malha.vecSolv, termolivre, eps, iter, precn);
        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].cel2D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh2d[i].cel2D.tempC - malha.mlh2d[i].cel2D.tempC0);
        }
        norma /= malha.nele;
        if (norma >= norma0)
            diverge = 1;
        else
            norma0 = norma;

        if (dados->acop == 1) {

            //dados->qAcop = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					malha.mlh2d[i].ccHR[j] = 1 / dados->resGlob[iPoc];
        					double raio;
        					if(j==0 || j==3)raio=malha.mlh2d[i].cel2D.coordVert[0][0];
        					else if(j==1 || j==2)raio=malha.mlh2d[i].cel2D.coordVert[1][0];
        					double delRaio=malha.mlh2d[i].cel2D.sFace[j][1];
        					double dProf=malha.mlh2d[i].cel2D.sFace[j][0];
        					double areaR=M_PI*((raio+delRaio)*(raio+delRaio)-raio*raio);
        					double areaP=2.*M_PI*raio*dProf;
        					double areaMod=fabs(areaR)+fabs(areaP);
        					//malha.mlh2d[i].ccHR[j]/=areaMod;
        					malha.mlh2d[i].ccHR[j]/=(2.*M_PI*raio);
        					if(dados->acopDirichlet==0)
        					dados->qAcop[iPoc] = (dados->tInt[iPoc] - malha.mlh2d[i].tempF[j]) / (dados->resGlob[iPoc]);
        					else dados->qAcop[iPoc] =malha.mlh2d[i].fluxTface[j][0]/fabs(dProf);
                        }
                    }
                }
            }
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					if(dados->acopDirichlet==0)
        					malha.mlh2d[i].ccTambR[j] = dados->tInt[iPoc];
        					else malha.mlh2d[i].ccTD[j]=dados->tInt[iPoc];
                        }
                    }
                }
            }
        }
        itera++;
    }
}

void solverPAxiSim::imprimePermanente(int indTramo) {
    malha.imprime(0.,indTramo);
}

void solverPAxiSim::inicializaPermanentePoisson() {
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].perm = 1;
        malha.mlh2d[i].trans = 0;
    }
    for(int i=0; i<dados->ncelPoco;i++)
    dados->tInt0[i] = dados->tInt[i];
}

void solverPAxiSim::inicializaTransientePoisson(std::vector<double>& tInicial) {

	int nLinha=tInicial.size();

    TransCal *vecTransfer = nullptr;
    vecTransfer = new TransCal[nLinha];
    DadosGeo *dutosMRT;
    dutosMRT=new DadosGeo[dados->nzonas];
	double* cond;
	double* cp;
	double* rho;
	double* beta;
	double* visc;
	int* tipomat;
	int* indmat;
	cond=new double[dados->corte[0].ncam];
	cp=new double[dados->corte[0].ncam];
	rho=new double[dados->corte[0].ncam];
	visc=new double[dados->corte[0].ncam];
	beta=new double[dados->corte[0].ncam];
	tipomat=new int[dados->corte[0].ncam];
	indmat=new int[dados->corte[0].ncam];
    for(int iD=0;iD<dados->nzonas;iD++){

    	for(int iC=0;iC<dados->corte[0].ncam;iC++){
    		int iMat=dados->corte[iD].indmat[iC];
    		cond[iC]=dados->mat[iMat].cond;
    		cp[iC]=dados->mat[iMat].cp;
    		rho[iC]=dados->mat[iMat].rho;
    		visc[iC]=0.;
    		beta[iC]=0.;
    		tipomat[iC]=0;
    		indmat[iC]=iMat;
    	}
    	dutosMRT[iD]=DadosGeo(dados->corte[iD].a, dados->corte[iD].a, 0, 0., 0, dados->corte[iD].ncam,
                cond, dados->corte[iD].diam, cp,rho, visc, beta, tipomat, indmat);
    }
	int* vncamada;
	vncamada = new int[dutosMRT[0].ncamadas];
	for (int i = 0; i < dutosMRT[0].ncamadas; i++) {
		if (dutosMRT[0].tipomat[i] != 0)
			vncamada[i] = 1;
		else
			vncamada[i] = dados->corte[0].discre[i];
	}
	double* vdrcamada;
	vdrcamada = new double[dutosMRT[0].ncamadas];
	for (int i = 0; i < dutosMRT[0].ncamadas; i++) {
		if (i > 0)
			vdrcamada[i] = 0.5 * (dutosMRT[0].diamC[i] - dutosMRT[0].diamC[i - 1])
					/ vncamada[i];
		else
			vdrcamada[0] = 0.5 * (dutosMRT[0].diamC[0] - dutosMRT[0].a) / vncamada[0];
	}
	double** vTcamada;
	vTcamada = new double*[dutosMRT[0].ncamadas];
	for (int i = 0; i < dutosMRT[0].ncamadas; i++) {
		vTcamada[i] = new double[vncamada[i] + 1];
	}

	for (int i = 0; i < dutosMRT[0].ncamadas; i++) {
		for (int j = 0; j <= vncamada[i]; j++)
			vTcamada[i][j] = dados->perfiGeoterm[0];
	}

    TransCal transfer(vg1dSP, dutosMRT[0], 1, dados->corte[0].discre, vdrcamada, vTcamada,
    		tInicial[0], dados->perfiGeoterm[0], 0., 1000., 1000., 0, 1., 0.6, 4186., 1000., 1., 1.,
                        0., 0., 0., 1, 1, 365,dutosMRT[0].cond[dutosMRT[0].ncamadas-1], dutosMRT[0].cp[dutosMRT[0].ncamadas-1],
						dutosMRT[0].rhoC[dutosMRT[0].ncamadas-1], 1.);

    double prof=0.;
    int iele=0;
    for (int i = 0; i < nLinha; i++) {
            vecTransfer[i] = transfer;
            vecTransfer[i].Tint=tInicial[i];
            vecTransfer[i].Textern1=dados->perfiGeoterm[i];
            int indFor=0;
            for(int iF=0;iF<dados->nzonas-1;iF++){
            	if(prof<=dados->zonaProf[iF]){
            		indFor=iF;
            		break;
            	}
            }
            vecTransfer[i].geom=dutosMRT[indFor];
            vecTransfer[i].condform=dutosMRT[indFor].cond[dutosMRT[indFor].ncamadas-1];
            vecTransfer[i].cpform=dutosMRT[indFor].cp[dutosMRT[indFor].ncamadas-1];
            vecTransfer[i].rhoform=dutosMRT[indFor].rhoC[dutosMRT[indFor].ncamadas-1];
            vecTransfer[i].Textern1=dados->perfiGeoterm[i];
            vecTransfer[i].Textern2=dados->perfiGeoterm[i];
            vecTransfer[i].Tint=tInicial[i];
            prof+=dados->dxPoco[i];
            vecTransfer[i].transperm();
            for(int iC=0; iC<dutosMRT[0].ncamadas; iC++){
            	for(int iD=0;iD<vecTransfer[i].ncamada[iC];iD++){
            		malha.mlh2d[iele].cel2D.tempC=vecTransfer[i].Tcamada[iC][iD];
            		malha.mlh2d[iele].cel2D.tempC0=vecTransfer[i].Tcamada[iC][iD];
            		malha.mlh2d[iele].cel2D.tempCI=vecTransfer[i].Tcamada[iC][iD];
            		iele++;
            	}
            }

     }

    /*for(int itempo=0;itempo<100; itempo++){
    	vecTransfer[0].transtrans(1,-1000.);
    }*/

    delete [] vecTransfer;
	for (int i = 0; i < dutosMRT[0].ncamadas; i++) delete[] vTcamada[i];
	delete[] vTcamada;
    delete [] dutosMRT;
    delete [] cond;
    delete [] cp;
    delete [] rho;
    delete [] beta;
    delete [] visc;
    delete [] tipomat;
    delete [] indmat;
    delete [] vncamada;
    delete [] vdrcamada;


	if (dados->temp.perm == 1 && dados->temp.trans == 1) {
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].perm = 0;
            malha.mlh2d[i].trans = 1;
        }
    }

    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].perm = 0;
        malha.mlh2d[i].trans = 1;
    }
    kontaTempo = 0;
    for(int i=0; i<dados->ncelPoco;i++)
    dados->tInt0[i] = dados->tInt[i];
}

void solverPAxiSim::transientePoissonDummy(double vdelt) {
    double delt = vdelt;
    double norma;
    int itera;
    int indsra;

    norma = 1e5;

    for (int i = 0; i < malha.nele; i++)
        malha.mlh2d[i].dt = delt;
    for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }
    itera = 0;
    while (norma > 1.e-5) {
#pragma omp parallel for num_threads(dados->nthrdLocal)
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.dim; j++) {
                malha.mlh2d[i].cel2D.gradGreenTI[j] = malha.mlh2d[i].cel2D.gradGreenT[j];
            }
        }
#pragma omp parallel for num_threads(dados->nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].calcGradGreen();
#pragma omp parallel for num_threads(dados->nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].GeraLocal();
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh2d[i].cel2D.tempC;
		double eps=0.00001;
		int iter=malha.nele;
		int retorna;
		if (dados->solverMat == 0)
			retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,1,40);
		else if (dados->solverMat == 1)
			retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,1,40);
		else
			retorna=matglob.BiCGStab(malha.vecSolv, termolivre, eps, iter, precn);
        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].cel2D.tempCI = malha.mlh2d[i].cel2D.tempC;
            malha.mlh2d[i].cel2D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh2d[i].cel2D.tempC - malha.mlh2d[i].cel2D.tempCI);
        }
        norma /= malha.nele;

        if (dados->acop == 1) {
            //dados->qAcop = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					malha.mlh2d[i].ccHR[j] = 1 / dados->resGlob[iPoc];
        					double raio;
        					if(j==0 || j==3)raio=malha.mlh2d[i].cel2D.coordVert[0][0];
        					else if(j==1 || j==2)raio=malha.mlh2d[i].cel2D.coordVert[1][0];
        					double delRaio=malha.mlh2d[i].cel2D.sFace[j][1];
        					double dProf=malha.mlh2d[i].cel2D.sFace[j][0];
        					double areaR=M_PI*((raio+delRaio)*(raio+delRaio)-raio*raio);
        					double areaP=2.*M_PI*raio*dProf;
        					double areaMod=fabs(areaR)+fabs(areaP);
        					//malha.mlh2d[i].ccHR[j]/=areaMod;
        					malha.mlh2d[i].ccHR[j]/=(2.*M_PI*raio);
        					if(dados->acopDirichlet==0)
        					dados->qAcop[iPoc] = (dados->tInt[iPoc] - malha.mlh2d[i].tempF[j]) / (dados->resGlob[iPoc]);
        					else dados->qAcop[iPoc] =malha.mlh2d[i].fluxTface[j][0]/fabs(dProf);
                        }
                    }
                }
            }
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					if(dados->acopDirichlet==0)
        					malha.mlh2d[i].ccTambR[j] = dados->tInt[iPoc];
        					else malha.mlh2d[i].ccTD[j]=dados->tInt[iPoc];
                        }
                    }
                }
            }
        }
        itera++;
    }
    if (dados->acop == 1){
        for(int i=0; i<dados->ncelPoco;i++)
        dados->tInt0[i] = dados->tInt[i];
    }
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].cel2D.tempC0 = malha.mlh2d[i].cel2D.tempC;
    }
}

double solverPAxiSim::defineDeltPoisson() {
    int ind = 0;
    double raz;
    double delt;

    indrazT(ind, raz);
    double inf = dados->temp.dtmax[ind];
    double sup;
    if (ind < dados->temp.parserie - 1) {
        sup = dados->temp.dtmax[ind + 1];
        delt = (inf * raz + (1 - raz) * sup);
    } else
        delt = dados->temp.dtmax[ind];
    return delt;
}

void solverPAxiSim::transientePoisson(double vdelt) {
    double delt = vdelt;
    double norma;
    int itera;
    int indsra;
    norma = 1e5;
    for (int i = 0; i < malha.nele; i++)
        malha.mlh2d[i].dt = delt;
    for (int i = 0; i < malha.nele; i++) {
        for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
            if (malha.mlh2d[i].kvizinho[j] < 0) {
                malha.mlh2d[i].atualizaCC(j);
            }
        }
    }
    itera = 0;
    while (norma > 1.e-5) {
#pragma omp parallel for num_threads(dados->nthrdLocal)
        for (int i = 0; i < malha.nele; i++) {
            for (int j = 0; j < malha.mlh2d[i].cel2D.dim; j++) {
                malha.mlh2d[i].cel2D.gradGreenTI[j] = malha.mlh2d[i].cel2D.gradGreenT[j];
            }
        }
#pragma omp parallel for num_threads(dados->nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].calcGradGreen();
#pragma omp parallel for num_threads(dados->nthrdLocal)
        for (int i = 0; i < malha.nele; i++)
            malha.mlh2d[i].GeraLocal();
        indsra = 0;
        for (int i = 0; i < malha.nele; i++) {
            int dimNoZero = malha.mlh2d[i].cel2D.indVizCres.size();
            for (int j = 0; j < dimNoZero; j++)
                matglob.sra[indsra + j] = malha.mlh2d[i].local.mx[0][j];
            indsra += dimNoZero;
            termolivre[i] = malha.mlh2d[i].TL[0];
        }
        for (int i = 0; i < malha.nele; i++)
            malha.vecSolv[i] = malha.mlh2d[i].cel2D.tempC;
		double eps=0.00001;
		int iter=malha.nele;
		int retorna;
		if (dados->solverMat == 0)
			retorna=matglob.GMRES(malha.vecSolv,termolivre,eps,iter,1,40);
		else if (dados->solverMat == 1)
			retorna=matglob.FGMRES(malha.vecSolv,termolivre,eps,iter,1,40);
		else
			retorna=matglob.BiCGStab(malha.vecSolv, termolivre, eps, iter, precn);
        norma = 0.;
        for (int i = 0; i < malha.nele; i++) {
            malha.mlh2d[i].cel2D.tempCI = malha.mlh2d[i].cel2D.tempC;
            malha.mlh2d[i].cel2D.tempC = malha.vecSolv[i];
            norma += fabs(malha.mlh2d[i].cel2D.tempC - malha.mlh2d[i].cel2D.tempCI);
        }
        norma /= malha.nele;

        if (dados->acop == 1) {

            //dados->qAcop = 0.;
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					malha.mlh2d[i].ccHR[j] = 1 / dados->resGlob[iPoc];
        					double raio;
        					if(j==0 || j==3)raio=malha.mlh2d[i].cel2D.coordVert[0][0];
        					else if(j==1 || j==2)raio=malha.mlh2d[i].cel2D.coordVert[1][0];
        					double delRaio=malha.mlh2d[i].cel2D.sFace[j][1];
        					double dProf=malha.mlh2d[i].cel2D.sFace[j][0];
        					double areaR=M_PI*((raio+delRaio)*(raio+delRaio)-raio*raio);
        					double areaP=2.*M_PI*raio*dProf;
        					double areaMod=fabs(areaR)+fabs(areaP);
        					//malha.mlh2d[i].ccHR[j]/=areaMod;
        					malha.mlh2d[i].ccHR[j]/=(2.*M_PI*raio);
        					if(dados->acopDirichlet==0)
        					dados->qAcop[iPoc] = (dados->tInt[iPoc] - malha.mlh2d[i].tempF[j]) / (dados->resGlob[iPoc]);
        					else dados->qAcop[iPoc] =malha.mlh2d[i].fluxTface[j][0]/fabs(dProf);
                        }
                    }
                }
            }
            for (int i = 0; i < malha.nele; i++) {
                for (int j = 0; j < malha.mlh2d[i].cel2D.nvert; j++) {
                    if (malha.mlh2d[i].kvizinho[j] < 0) {
                        int kcc = 0;
                        while (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] != dados->CC.rotuloAcop)
                            kcc++;
                        if (kcc < 1 && malha.mlh2d[i].cel2D.ccFace[j] == dados->CC.rotuloAcop) {
        					int iPoc=floor(i/dados->ndR);
        					if(dados->acopDirichlet==0)
        					malha.mlh2d[i].ccTambR[j] = dados->tInt[iPoc];
        					else malha.mlh2d[i].ccTD[j]=dados->tInt[iPoc];
                        }
                    }
                }
            }
        }
        itera++;
    }
}

void solverPAxiSim::finalizaPassoTransiente(double vdelt, int indTramo) {
    double delt = vdelt;
    if (dados->acop == 1){
        for(int i=0; i<dados->ncelPoco;i++)
        dados->tInt0[i] = dados->tInt[i];
    }
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].cel2D.tempC0 = malha.mlh2d[i].cel2D.tempC;
    }

    if (((*vg1dSP).lixo5 <= dados->temp.tempoImp[kontaTempo] && (*vg1dSP).lixo5 + delt >= dados->temp.tempoImp[kontaTempo])) {
        malha.imprime((*vg1dSP).lixo5, indTramo);
        kontaTempo++;
        if (kontaTempo >= dados->temp.parserieImp)
            kontaTempo--;
    }
}

void solverPAxiSim::FeiticoDoTempo() {
    for (int i = 0; i < malha.nele; i++) {
        malha.mlh2d[i].cel2D.tempC = malha.mlh2d[i].cel2D.tempC0;
    }
}
