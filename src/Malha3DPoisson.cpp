/*
 * Malha.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Malha3DPoisson.h"


malha3d::malha3d(varGlob1D* DGP3DV,eleOBJ* velementoUNV, double** xcoor, int** noEle,int* tipo,
		double* atributo,int nVert, int neleV, int nno):
	vecSolv(neleV){
	DGP3D=DGP3DV;
	nele=neleV;
	double voltot=0.;
	double areatot=0.;
	if(nele>0){
	    for(int i=0; i<neleV;i++){
	    	elem3dPoisson temp(DGP3D,velementoUNV[i],xcoor, noEle, tipo, atributo, nVert, nele, nno, i);
	    	mlh3d.push_back(temp);
	    	voltot+=mlh3d[i].cel3D.vElem;
	    	for(int j=0; j<mlh3d[i].cel3D.nvert;j++){
	    		int indCC=mlh3d[i].eleUNV.faceCCfaceReal[j];
	    		if(mlh3d[i].cel3D.ccFace[j]!="interno")areatot+=mlh3d[i].cel3D.sFaceMod[j];
	    		//if(indCC>=0)cel3D.ccFace[j]=eleUNV.objFace[indCC].condCont;
	    	}
	    }
	    for(int i=0; i<nele;i++){
	    	int nvizi=0;
	    	for(int j=0; j<mlh3d[i].cel3D.nvert;j++){
	    		if(mlh3d[i].cel3D.ccFace[j]=="interno")nvizi++;
	    	}
	    	mlh3d[i].nvizinho=nvizi;
	    	if(mlh3d[i].cel3D.nvert>0){
	    		//mlh3d[i].vizinho=new elementoPoisson3D* [mlh3d[i].cel3D.nvert];
	    		//mlh3d[i].kvizinho=new int [mlh3d[i].cel3D.nvert];
	    		for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
	    			mlh3d[i].kvizinho[k]=-1;
	    			mlh3d[i].vizinho[k]=0;
	    		}
	    		//int j=0;
	    		//while(j<nvizi)
	    		for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
	    			if(mlh3d[i].cel3D.indFace[k]>=0){
	    				int indViz=mlh3d[i].cel3D.indFace[k];
	    				mlh3d[i].kvizinho[k]=k;
	    				mlh3d[i].vizinho[k]=&mlh3d[indViz].cel3D;
	    				//j++;
	    			}
	    		}
	    	}
	    }
	    for(int i=0; i<nele;i++){
	    	mlh3d[i].faceDetalhes();
	    	for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
	    		if(mlh3d[i].kvizinho[k]>=0){
	    			int indViz=mlh3d[i].cel3D.indFace[k];
	    			int match=0;
	    			int k2=0;
	    			while(match==0 && k2<mlh3d[indViz].cel3D.nvert){
	    				if(mlh3d[indViz].cel3D.indFace[k2]==i){
	    					match=1;
	    				}
	    				else k2++;
	    			}
	    			if(k2<mlh3d[indViz].cel3D.nvert){
	    				mlh3d[i].kvizinho[k]=k2;
	    			}
	    		}
	    	}
	    }
	}
}

malha3d::malha3d(const malha3d& vmalha):vecSolv(vmalha.nele){
	nele=vmalha.nele;
	DGP3D=vmalha.DGP3D;
	if(nele>0){
		vecSolv=vmalha.vecSolv;
	    for(int i=0; i<nele;i++){
	    	elem3dPoisson temp(vmalha.mlh3d[i]);
	    	mlh3d.push_back(temp);
	    }
	    for(int i=0; i<nele;i++){
	    	int nvizi=0;
	    	for(int j=0; j<mlh3d[i].cel3D.nvert;j++){
	    		if(mlh3d[i].cel3D.ccFace[j]=="interno")nvizi++;
	    	}
	    	mlh3d[i].nvizinho=nvizi;
	    	if(mlh3d[i].cel3D.nvert>0){
	    		//mlh3d[i].vizinho=new elementoPoisson3D* [mlh3d[i].cel3D.nvert];
	    		//mlh3d[i].kvizinho=new int [mlh3d[i].cel3D.nvert];
	    		for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
	    			mlh3d[i].kvizinho[k]=-1;
	    			mlh3d[i].vizinho[k]=0;
	    		}
	    		//int j=0;
	    		//while(j<nvizi)
	    		for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
	    			if(mlh3d[i].cel3D.indFace[k]>=0){
	    				int indViz=mlh3d[i].cel3D.indFace[k];
	    				mlh3d[i].kvizinho[k]=k;
	    				mlh3d[i].vizinho[k]=&mlh3d[indViz].cel3D;
	    				//j++;
	    			}
	    		}
	    	}
	    }
	    for(int i=0; i<nele;i++){
	    	mlh3d[i].faceDetalhes();
	    	for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
	    		if(mlh3d[i].kvizinho[k]>=0){
	    			int indViz=mlh3d[i].cel3D.indFace[k];
	    			int match=0;
	    			int k2=0;
	    			while(match==0 && k2<mlh3d[indViz].cel3D.nvert){
	    				if(mlh3d[indViz].cel3D.indFace[k2]==i){
	    					match=1;
	    				}
	    				else k2++;
	    			}
	    			if(k2<mlh3d[indViz].cel3D.nvert){
	    				mlh3d[i].kvizinho[k]=k2;
	    			}
	    		}
	    	}
	    }
	}
}

malha3d& malha3d::operator =(const malha3d& vmalha) {
	if (this != &vmalha) {
		if(mlh3d.size()>0)mlh3d.clear();
		DGP3D=vmalha.DGP3D;
		nele=vmalha.nele;
		if(nele>0){
			vecSolv=vmalha.vecSolv;
		    for(int i=0; i<nele;i++){
		    	elem3dPoisson temp(vmalha.mlh3d[i]);
		    	mlh3d.push_back(temp);
		    }
		    for(int i=0; i<nele;i++){
		    	int nvizi=0;
		    	for(int j=0; j<mlh3d[i].cel3D.nvert;j++){
		    		if(mlh3d[i].cel3D.ccFace[j]=="interno")nvizi++;
		    	}
		    	mlh3d[i].nvizinho=nvizi;
		    	if(mlh3d[i].cel3D.nvert>0){
		    		//mlh3d[i].vizinho=new elementoPoisson3D* [mlh3d[i].cel3D.nvert];
		    		//mlh3d[i].kvizinho=new int [mlh3d[i].cel3D.nvert];
		    		for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
		    			mlh3d[i].kvizinho[k]=-1;
		    			mlh3d[i].vizinho[k]=0;
		    		}
		    		//int j=0;
		    		//while(j<nvizi)
		    		for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
		    			if(mlh3d[i].cel3D.indFace[k]>=0){
		    				int indViz=mlh3d[i].cel3D.indFace[k];
		    				mlh3d[i].kvizinho[k]=k;
		    				mlh3d[i].vizinho[k]=&mlh3d[indViz].cel3D;
		    				//j++;
		    			}
		    		}
		    	}
		    }
		    for(int i=0; i<nele;i++){
		    	mlh3d[i].faceDetalhes();
		    	for(int k=0; k<mlh3d[i].cel3D.nvert;k++){
		    		if(mlh3d[i].kvizinho[k]>=0){
		    			int indViz=mlh3d[i].cel3D.indFace[k];
		    			int match=0;
		    			int k2=0;
		    			while(match==0 && k2<mlh3d[indViz].cel3D.nvert){
		    				if(mlh3d[indViz].cel3D.indFace[k2]==i){
		    					match=1;
		    				}
		    				else k2++;
		    			}
		    			if(k2<mlh3d[indViz].cel3D.nvert){
		    				mlh3d[i].kvizinho[k]=k2;
		    			}
		    		}
		    	}
		    }
		}
	}

	return *this;
}

void malha3d::imprime(double tempo){
	FullMtx<double> flut(nele,8);
	FullMtx<double> flut2(nele,6);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh3d[i].cel3D.centroideElem[0];
		flut.mx[i][2]=mlh3d[i].cel3D.centroideElem[1];
		flut.mx[i][3]=mlh3d[i].cel3D.centroideElem[2];
		flut.mx[i][4]=mlh3d[i].cel3D.tempC;
		if(mlh3d[i].cel3D.tempC>3.9 && mlh3d[i].cel3D.tempC<95) flut.mx[i][5]=0;
		else flut.mx[i][5]=-1000;
		flut.mx[i][6]=0;
		flut.mx[i][7]=mlh3d[i].cel3D.tempC;
	}

	for(int i=0;i<nele;i++){
		if(mlh3d[i].eleUNV.RAspec>0*1.+1*5.){
			double vol=0;
			double substitua=0;
			for(int j=0;j<mlh3d[i].cel3D.nvert;j++){
				int viz=mlh3d[i].kvizinho[j];
				if(viz>=0){
					substitua+=(*(mlh3d[i].vizinho[j])).tempC*(*(mlh3d[i].vizinho[j])).vElem;
					vol+=(*(mlh3d[i].vizinho[j])).vElem;
				}
			 }
		     if(vol>0){
					flut.mx[i][4]=substitua/vol;
					flut.mx[i][6]=-2000;
			 }
		}
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PERFIST" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Coordenada Z (m);";
	escreveIni << " Temperatura (C);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();


	for(int i=0;i<nele;i++){
		flut2.mx[i][0]=tempo;
		flut2.mx[i][1]=mlh3d[i].cel3D.centroideElem[0];
		flut2.mx[i][2]=mlh3d[i].cel3D.centroideElem[1];
		flut2.mx[i][3]=mlh3d[i].cel3D.centroideElem[2];
		flut2.mx[i][4]=mlh3d[i].cel3D.tempC;
		if(mlh3d[i].cel3D.tempC>3.9 && mlh3d[i].cel3D.tempC<95) flut2.mx[i][5]=0;
		else flut2.mx[i][5]=-1000;

	}

	ostringstream saidaP2;
	saidaP2 << pathPrefixoArqSaida << "PERFIST2" << "-" << numero
				<< ".dat";
	string tmp2 = saidaP2.str();
	ofstream escreveIni2(tmp2.c_str(), ios_base::out);
	escreveIni2 << " Tempo (s) ;";
	escreveIni2 << " Coordenada X (m);";
	escreveIni2 << " Coordenada Y (m);";
	escreveIni2 << " Coordenada Z (m);";
	escreveIni2 << " Temperatura (C);";
	escreveIni2 << "\n";
	escreveIni2 << flut2;
	escreveIni2.close();
}
