/*
 * Malha.cpp
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */
#include "Malha2D.h"


malha2dVF::malha2dVF(double** xcoor, int** noEle,int* tipo,double* atributo,int nVert, int neleV, int nno,double vdt, int vperm, int vtrans,
		varGlob1D* Vvg1dSP,ProFluColVF vflucVF):
	vecSolv(neleV){
	nele=neleV;
	vg1dSP=Vvg1dSP;
	flucVF=vflucVF;
	if(nele>0){
	    for(int i=0; i<neleV;i++){
	    	/*
	    	 * 	elem2d(double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int nele=0, int nno=0, int i=0,
			double vpres=0.,double vu=0.,double vv=0.,double vt=0.,double vfluxCal=0.,double vcond=0.,double vcp=0.,
			double vrho=0.,double vvisc=0.,double vbeta=0.,double vdt=0., int vperm=1, int vtrans=0);
	    	 */
	    	elem2d temp(vg1dSP,xcoor, noEle, tipo, atributo, nVert, nele, nno, i,vdt,vperm, vtrans,flucVF);
	    	mlh2d.push_back(temp);
	    }
	    for(int i=0; i<nele;i++){
	    	int nvizi=0;
	    	for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
	    		if(mlh2d[i].cel2D.ccFace[j]==0)nvizi++;
	    	}
	    	mlh2d[i].nvizinho=nvizi;
	    	if(mlh2d[i].cel2D.nvert>0){
	    		//mlh2d[i].vizinho=new elemento* [mlh2d[i].cel2D.nvert];
	    		//mlh2d[i].kvizinho=new int [mlh2d[i].cel2D.nvert];
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			mlh2d[i].kvizinho[k]=-1;
	    			mlh2d[i].vizinho[k]=0;
	    		}
	    		//int j=0;
	    		//while(j<nvizi)
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			if(mlh2d[i].cel2D.indFace[k]>=0){
	    				int indViz=mlh2d[i].cel2D.indFace[k];
	    				mlh2d[i].kvizinho[k]=k;
	    				mlh2d[i].vizinho[k]=&mlh2d[indViz].cel2D;
	    				//j++;
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

malha2dVF::malha2dVF(const malha2dVF& vmalha):vecSolv(vmalha.nele){
	nele=vmalha.nele;
	vg1dSP=vmalha.vg1dSP;
	flucVF=vmalha.flucVF;
	if(nele>0){
		vecSolv=vmalha.vecSolv;
	    for(int i=0; i<nele;i++){
	    	elem2d temp(vmalha.mlh2d[i]);
	    	mlh2d.push_back(temp);
	    }
	    for(int i=0; i<nele;i++){
	    	int nvizi=0;
	    	for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
	    		if(mlh2d[i].cel2D.ccFace[j]==0)nvizi++;
	    	}
	    	mlh2d[i].nvizinho=nvizi;
	    	if(mlh2d[i].cel2D.nvert>0){
	    		//mlh2d[i].vizinho=new elemento* [mlh2d[i].cel2D.nvert];
	    		//mlh2d[i].kvizinho=new int [mlh2d[i].cel2D.nvert];
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			mlh2d[i].kvizinho[k]=-1;
	    			mlh2d[i].vizinho[k]=0;
	    		}
	    		//int j=0;
	    		//while(j<nvizi)
	    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
	    			if(mlh2d[i].cel2D.indFace[k]>=0){
	    				int indViz=mlh2d[i].cel2D.indFace[k];
	    				mlh2d[i].kvizinho[k]=k;
	    				mlh2d[i].vizinho[k]=&mlh2d[indViz].cel2D;
	    				//j++;
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

malha2dVF& malha2dVF::operator =(const malha2dVF& vmalha) {
	if (this != &vmalha) {
		mlh2d.clear();
		nele=vmalha.nele;
		vg1dSP=vmalha.vg1dSP;
		flucVF=vmalha.flucVF;
		if(nele>0){
			vecSolv=vmalha.vecSolv;
		    for(int i=0; i<nele;i++){
		    	elem2d temp(vmalha.mlh2d[i]);
		    	mlh2d.push_back(temp);
		    }
		    for(int i=0; i<nele;i++){
		    	int nvizi=0;
		    	for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
		    		if(mlh2d[i].cel2D.ccFace[j]==0)nvizi++;
		    	}
		    	mlh2d[i].nvizinho=nvizi;
		    	if(mlh2d[i].cel2D.nvert>0){
		    		//mlh2d[i].vizinho=new elemento* [mlh2d[i].cel2D.nvert];
		    		//mlh2d[i].kvizinho=new int [mlh2d[i].cel2D.nvert];
		    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
		    			mlh2d[i].kvizinho[k]=-1;
		    			mlh2d[i].vizinho[k]=0;
		    		}
		    		//int j=0;
		    		//while(j<nvizi)
		    		for(int k=0; k<mlh2d[i].cel2D.nvert;k++){
		    			if(mlh2d[i].cel2D.indFace[k]>=0){
		    				int indViz=mlh2d[i].cel2D.indFace[k];
		    				mlh2d[i].kvizinho[k]=k;
		    				mlh2d[i].vizinho[k]=&mlh2d[indViz].cel2D;
		    				//j++;
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

void malha2dVF::imprimeU(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.uC;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisU" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Velocidade U (m/s);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeV(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.vC;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisV" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Velocidade V (m/s);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeP(double tempo){
	FullMtx<double> flut(nele,4);
	double ymaxWall=mlh2d[(*vg1dSP).eleYmaxWall].cel2D.centroideElem[1];

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		double dh=ymaxWall-mlh2d[i].cel2D.centroideElem[1];
		double forcaCorpo=dh*(mlh2d[i].cel2D.rho)*(*vg1dSP).gravVF*sinl((*vg1dSP).angY);
		flut.mx[i][3]=(mlh2d[i].cel2D.presC+(1.-(*vg1dSP).mulFC)*forcaCorpo)/98066.52;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisP" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Pressao (kgf/cm2);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeT(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.tempC;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisT" << "-" << numero
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

void malha2dVF::imprimePCor(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.presCcor;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisPCor" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Correcao de Pressao (Pa);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeHol(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.holC;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisHol" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Holdup (-);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeTaxaDeform(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.taxaDeform;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisTaxa" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Taxa de Deformacao (1/s);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeVisc(double tempo){
	FullMtx<double> flut(nele,4);

	for(int i=0;i<nele;i++){
		flut.mx[i][0]=tempo;
		flut.mx[i][1]=mlh2d[i].cel2D.centroideElem[0];
		flut.mx[i][2]=mlh2d[i].cel2D.centroideElem[1];
		flut.mx[i][3]=mlh2d[i].cel2D.visc;
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "PerfisVisc" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Coordenada X (m);";
	escreveIni << " Coordenada Y (m);";
	escreveIni << " Viscosidade (Pa.s);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeTcont(double tempo, int nPar){
	FullMtx<double> flut(nPar,4);

	int iCont=0;
	for(int i=0; i<nele;i++){
		for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
			if(mlh2d[i].kvizinho[j]<0){
				int kcc=0;
				while(kcc<1 && mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
					flut.mx[iCont][0]=tempo;
					flut.mx[iCont][1]=i;
					flut.mx[iCont][2]=mlh2d[i].tempF[j];
					flut.mx[iCont][3]=mlh2d[i].cel2D.tempC;
					iCont++;
				}
			}
		}
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "temperaturaContorno" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Indice do Elemento;";
	escreveIni << " Temperatura na Fronteira (C);";
	escreveIni << " Temperatura no Volume (C);";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}

void malha2dVF::imprimeQcont(double tempo, int nPar){
	FullMtx<double> flut(nPar,3);

	int iCont=0;
	for(int i=0; i<nele;i++){
		for(int j=0; j<mlh2d[i].cel2D.nvert;j++){
			if(mlh2d[i].kvizinho[j]<0){
				int kcc=0;
				while(kcc<1 && mlh2d[i].cel2D.ccFace[j]!=CC.rotuloAcop) kcc++;
				if(kcc<1 && mlh2d[i].cel2D.ccFace[j]==CC.rotuloAcop){
					flut.mx[iCont][0]=tempo;
					flut.mx[iCont][1]=i;
					flut.mx[iCont][2]=mlh2d[i].ccTVN[j];
					iCont++;
				}
			}
		}
	}

	ostringstream saidaP;
	int numero = roundl(tempo);
		saidaP << pathPrefixoArqSaida << "FluxoCalorContorno" << "-" << numero
				<< ".dat";
	string tmp = saidaP.str();
	ofstream escreveIni(tmp.c_str(), ios_base::out);
	escreveIni << " Tempo (s) ;";
	escreveIni << " Indice do Elemento;";
	escreveIni << " Fluxo de Calor Contorno (J/(s*m2));";
	escreveIni << "\n";
	escreveIni << flut;
	escreveIni.close();
}
