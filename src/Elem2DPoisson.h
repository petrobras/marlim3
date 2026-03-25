/*
 * elem2dPoisson.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef elem2dPoissonPOISSON_H_
#define elem2dPoissonPOISSON_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "estruturasPoisson.h"
#include "variaveisGlobais1D.h"

//extern detTempoPoisson temp;
//extern detCCPoisson CC;

//extern double lixo5;

struct elementoPoisson{
    int nvert;//numero de vertices
    int dim;//dimensao do problema
    int indEle;//indice do elementoPoisson
    int nele;//numero de elementoPoissons da discretizacao
    int* noElem;//vetor com o indice dos nos
    int* indFace;//vetor com o indice do elementoPoisson vizinho para uma das faces do elementoPoisson
    int* ccFace;//vetor indicando a condicao de contorno de uma face
    vector<int> indVizCres;
    double* orientFace;//vetor corrigindo a orientacao da face de um elementoPoisson,
    //para garantir que sempre esteja apontada para fora
    double* centroElem;//coordenadas do centro geométrico do elementoPoisson
    double* centroideElem;//coordenadas do centro de massa do elementoPoisson
    double** centroideFace;//coordenadas com o centro de massa da face
    double** coordVert;//coordenadas de cada vertice do elemnto
    double** dCF;//vetor apontando do centroide do elemnto para o centro da face
    double* dCFMod;//modulo do vetor
    double vElem;//volume/area do elemnto
    int** noFace;
    double** sFace;//vetor da área da face
    double* sFaceMod;//modulo do vetor
    double* ownFace;//vetor indicando se o elementoPoisson é ou não "dono" do vetor area
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double** vecE;//vetor unitario na orientacao entre o centroide de do elementoPoisson e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elementoPoisson e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementoPoissons
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementoPoissons e a face do elementoPoisson
    double** fIfC;//vetor entre a intersecao e o centroide da face

	double tempC;
	double tempCI;
	double tempC0;
	double* gradGreenT;
	double* gradGreenTI;
	double cond;
	double cp;
	double rho;
};

class elem2dPoisson{
	public:
	detCCPoisson CC;
	elementoPoisson cel2D;
	int nvizinho;
	elementoPoisson** vizinho;
	int* kvizinho;
	varGlob1D* vg1dSP;
	elem2dPoisson(varGlob1D* Vvg1dSP=0,double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int nele=0, int nno=0,
			int vperm=1, int vtrans=0,int i=0,double vtemp=0.,double vfluxCal=0.,double vcond=0.,double vcp=0.,
			double vrho=0.,double vdt=0.);
	elem2dPoisson(const elem2dPoisson&); //construtor por copia
	elem2dPoisson& operator=(const elem2dPoisson&);

	double deriFonteT;
	double FonteT;
	double* tempF;
	double fluxCal;
	double dt;
	double** gradTface;
	int perm;
	int trans;
	double* ccTD;
	double* ccTVN;
	double* ccHR;
	double* ccTambR;
	double* coefTHRC;
	double* coefTHRV;
	double* fonteTHR;

    Vcr<double> TL;//vetor livre da matriz local;
    FullMtx<double> local;//matriz local;

	~elem2dPoisson(){
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
	}

	void buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle);
	void menorIndViz();
	void faceDetalhes();
	void tipoCC(int i,int& diri, int& vn, int& rich, int& acoplado,int& kcc);
	void atualizaCC(int i);
	void calcGradGreen();
	double escalar(double* v1, double* v2, int dimV);
	int achaInd(int i);
	void GeraLocal();
	void indraz(int& ind/*valor retornado do momento na serie de tempo logo anterior ao momento que se quer avaliar*/,
			double& raz/*posicao relativa de tempo, entre a posicao ind e ind+1*/,
			double tempo/*momento que se quer avaliar na serie de tempo para se fazer a interpolacao*/,
			int parserie/*tamanho da serie de tempo*/,
			double* serietemp/*vetor com os tempos da serie de tempo*/);//metodo que obtem o valor interpolado
	//de uma variavel de uma serie de tempo



};

#endif /* elem2dPoissonPOISSON_H_ */
