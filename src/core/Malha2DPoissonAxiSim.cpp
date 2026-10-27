/*
 * Malha.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Malha2DPoissonAxiSim.h"


malha2dAxiSim::malha2dAxiSim(varGlob1D* Vvg1dSP,double** xcoor, int** noEle,int* tipo,
		double* atributo,int nVert, int neleV, int nno, int vperm, int vtrans, int vacopD):
	vecSolv(neleV){
	nele=neleV;
	acopDir=vacopD;
	if(nele>0){
	    for(int i=0; i<neleV;i++){
	    	elem2dPoissonAxiSim temp(Vvg1dSP,xcoor, noEle, tipo, atributo, nVert, nele,
	    			nno,vperm, vtrans, i,acopDir);
	    	mlh2d.push_back(temp);
	    }
	    for(int i=0; i<nele;i++){
	    	int nvizi=0;
	    	for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
	    		if(mlh2d[i].cel2D.ccFace[j]==0)nvizi++;
	    	}
	    	mlh2d[i].nvizinho=nvizi;
	    	if(mlh2d[i].cel2D.nvert>0){
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			mlh2d[i].kvizinho[k]=-1;
	    			mlh2d[i].vizinho[k]=0;
	    		}
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			if(mlh2d[i].cel2D.indFace[k]>=0){
	    				int indViz=mlh2d[i].cel2D.indFace[k];
	    				mlh2d[i].kvizinho[k]=k;
	    				mlh2d[i].vizinho[k]=&mlh2d[indViz].cel2D;
	    			}
	    		}
	    	}
	    }
	    for(int i=0; i<nele;i++){
	    	mlh2d[i].faceDetalhes();
	    	for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    		if(mlh2d[i].kvizinho[k]>=0){
	    			int indViz=mlh2d[i].cel2D.indFace[k];
	    			int match=0;
	    			int k2=0;
	    			while(match==0 && k2<mlh2d[indViz].cel2D.nvert){
	    				if(mlh2d[indViz].cel2D.indFace[k2]==i){
	    					match=1;
	    				}
	    				else k2++;
	    			}
	    			if(k2<mlh2d[indViz].cel2D.nvert){
	    				mlh2d[i].kvizinho[k]=k2;
	    			}
	    		}
	    	}
	    }
	}
}

malha2dAxiSim::malha2dAxiSim(const malha2dAxiSim& vmalha):vecSolv(vmalha.nele){
	nele=vmalha.nele;
	acopDir=vmalha.acopDir;
	if(nele>0){
		vecSolv=vmalha.vecSolv;
	    for(int i=0; i<nele;i++){
	    	elem2dPoissonAxiSim temp(vmalha.mlh2d[i]);
	    	mlh2d.push_back(temp);
	    }
	    for(int i=0; i<nele;i++){
	    	int nvizi=0;
	    	for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
	    		if(mlh2d[i].cel2D.ccFace[j]==0)nvizi++;
	    	}
	    	mlh2d[i].nvizinho=nvizi;
	    	if(mlh2d[i].cel2D.nvert>0){
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			mlh2d[i].kvizinho[k]=-1;
	    			mlh2d[i].vizinho[k]=0;
	    		}
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			if(mlh2d[i].cel2D.indFace[k]>=0){
	    				int indViz=mlh2d[i].cel2D.indFace[k];
	    				mlh2d[i].kvizinho[k]=k;
	    				mlh2d[i].vizinho[k]=&mlh2d[indViz].cel2D;
	    			}
	    		}
	    	}
	    }
	    for(int i=0; i<nele;i++){
	    	mlh2d[i].faceDetalhes();
	    	for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    		if(mlh2d[i].kvizinho[k]>=0){
	    			int indViz=mlh2d[i].cel2D.indFace[k];
	    			int match=0;
	    			int k2=0;
	    			while(match==0 && k2<mlh2d[indViz].cel2D.nvert){
	    				if(mlh2d[indViz].cel2D.indFace[k2]==i){
	    					match=1;
	    				}
	    				else k2++;
	    			}
	    			if(k2<mlh2d[indViz].cel2D.nvert){
	    				mlh2d[i].kvizinho[k]=k2;
	    			}
	    		}
	    	}
	    }
	}
}

malha2dAxiSim& malha2dAxiSim::operator =(const malha2dAxiSim& vmalha) {
	if (this != &vmalha) {
		mlh2d.clear();
		nele=vmalha.nele;
		if(nele>0){
			vecSolv=vmalha.vecSolv;
		    for(int i=0; i<nele;i++){
		    	elem2dPoissonAxiSim temp(vmalha.mlh2d[i]);
		    	mlh2d.push_back(temp);
		    }
		    for(int i=0; i<nele;i++){
		    	int nvizi=0;
		    	for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
		    		if(mlh2d[i].cel2D.ccFace[j]==0)nvizi++;
		    	}
		    	mlh2d[i].nvizinho=nvizi;
		    	if(mlh2d[i].cel2D.nvert>0){
		    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
		    			mlh2d[i].kvizinho[k]=-1;
		    			mlh2d[i].vizinho[k]=0;
		    		}
		    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
		    			if(mlh2d[i].cel2D.indFace[k]>=0){
		    				int indViz=mlh2d[i].cel2D.indFace[k];
		    				mlh2d[i].kvizinho[k]=k;
		    				mlh2d[i].vizinho[k]=&mlh2d[indViz].cel2D;
		    			}
		    		}
		    	}
		    }
		    for(int i=0; i<nele;i++){
		    	mlh2d[i].faceDetalhes();
		    	for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
		    		if(mlh2d[i].kvizinho[k]>=0){
		    			int indViz=mlh2d[i].cel2D.indFace[k];
		    			int match=0;
		    			int k2=0;
		    			while(match==0 && k2<mlh2d[indViz].cel2D.nvert){
		    				if(mlh2d[indViz].cel2D.indFace[k2]==i){
		    					match=1;
		    				}
		    				else k2++;
		    			}
		    			if(k2<mlh2d[indViz].cel2D.nvert){
		    				mlh2d[i].kvizinho[k]=k2;
		    			}
		    		}
		    	}
		    }
		}
	}

	return *this;
}

void malha2dAxiSim::imprime(double tempo, int indTramo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.tempC;
	}

	ostringstream saidaP;
	int numero = round(tempo);
	if(indTramo<0)
		saidaP << pathPrefixoArqSaida << "PERFIST2DAxiSim-"<< "-" << numero
				<< ".dat";
	else
		saidaP << pathPrefixoArqSaida << "PERFIST2DAxiSim-"<<"-Tramo-"<<indTramo<< "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Temperatura (C);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}
