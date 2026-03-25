/*
 * elem3dPoisson.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef elem3dPoissonPOISSON_H_
#define elem3dPoissonPOISSON_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "estruturasPoisson3D.h"
#include "estruturaUNV.h"
#include "variaveisGlobais1D.h"

//extern detTempoPoisson3D temp;
//extern detCCPoisson3D CC;

//extern double tempo;

struct elementoPoisson3D{
    int nvert;//numero de vertices
    int dim;//dimensao do problema
    int indEle;//indice do elementoPoisson3D
    int nele;//numero de elementoPoisson3Ds da discretizacao
    int* noElem;//vetor com o indice dos nos
    int* indFace;//vetor com o indice do elementoPoisson3D vizinho para uma das faces do elementoPoisson3D
    string* ccFace;//vetor indicando a condicao de contorno de uma face
    vector<int> indVizCres;
    double* orientFace;//vetor corrigindo a orientacao da face de um elementoPoisson3D,
    //para garantir que sempre esteja apontada para fora
    double* centroElem;//coordenadas do centro geométrico do elementoPoisson3D
    double* centroideElem;//coordenadas do centro de massa do elementoPoisson3D
    double** centroideFace;//coordenadas com o centro de massa da face
    double** coordVert;//coordenadas de cada vertice do elemnto
    double** dCF;//vetor apontando do centroide do elemnto para o centro da face
    double* dCFMod;//modulo do vetor
    double vElem;//volume/area do elemnto
    int** noFace;
    double** sFace;//vetor da área da face
    double* sFaceMod;//modulo do vetor
    double* ownFace;//vetor indicando se o elementoPoisson3D é ou não "dono" do vetor area
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double** vecE;//vetor unitario na orientacao entre o centroide de do elementoPoisson3D e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elementoPoisson3D e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementoPoisson3Ds
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementoPoisson3Ds e a face do elementoPoisson3D
    double** fIfC;//vetor entre a intersecao e o centroide da face

	double tempC;
	double tempCI;
	double tempC0;
	double* gradGreenT;
	double* gradGreenTI;
	double* gradGreenT0;
	double cond;
	double cp;
	double rho;
};

class elem3dPoisson{
	public:
	eleOBJ eleUNV;
	detCCPoisson3D* CC;
	elementoPoisson3D cel3D;
	int nvizinho;
	elementoPoisson3D** vizinho;
	int* kvizinho;

	double deriFonteT;
	double FonteT;
	double* tempF;
	double fluxCal;
	double dt;
	double** gradTface;
	//int perm;
	//int trans;
	double* ccTD;
	double* ccTVN;
	double* ccHR;
	double* ccTambR;
	double* coefTHRC;
	double* coefTHRV;
	double* fonteTHR;

	double* v1;
	double* v2;
	double* v3;

	varGlob1D* DGP3D;

    Vcr<double> TL;//vetor livre da matriz local;
    FullMtx<double> local;//matriz local;


	elem3dPoisson(varGlob1D* DGP3DV=0,eleOBJ velementoUNV=eleOBJ(),double** xcoor=0, int** noEle=0, int* tipo=0,
			double* atributo=0,int nVert=0, int nele=0, int nno=0,
			int i=0,double vtemp=0.,double vfluxCal=0.,double vcond=0.,double vcp=0.,
			double vrho=0.,double vdt=0.);
	elem3dPoisson(const elem3dPoisson&); //construtor por copia
	elem3dPoisson& operator=(const elem3dPoisson&);

	~elem3dPoisson(){
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
	}

	void buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle);
	void menorIndViz();
	void faceDetalhes();
	void tipoCC(int i,int& diri, int& vn, int& rich, int& acoplado,int& kcc);
	void atualizaCC(int i);
	void calcGradGreen();
	double escalar(double* v1, double* v2, int dimV);
	int achaInd(int i);
	void GeraLocal(double relax);
	void indraz(int& ind/*valor retornado do momento na serie de tempo logo anterior ao momento que se quer avaliar*/,
			double& raz/*posicao relativa de tempo, entre a posicao ind e ind+1*/,
			double tempo/*momento que se quer avaliar na serie de tempo para se fazer a interpolacao*/,
			int parserie/*tamanho da serie de tempo*/,
			double* serietemp/*vetor com os tempos da serie de tempo*/);//metodo que obtem o valor interpolado
	//de uma variavel de uma serie de tempo



};

#endif /* elem3dPoissonPOISSON_H_ */
