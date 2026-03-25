/*
 * Elem2D.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef ELEM2D_H_
#define ELEM2D_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "estruturas.h"
#include "variaveisGlobais1D.h"
#include "PropFluColVF.h"

extern detTempo tempVF;
extern detCC CC;
/*extern int partidaVF;
extern double relaxVF;
extern double relaxVFPcor;
extern int nvfHR;
extern int nvfHRHol;
extern int atrasaHR;
extern double gravVF;

extern double tempo;
extern double vertY;
extern int iterPresCor;
extern int metodoAcopPV;

extern double angY;
extern double angX;

extern double mulFC;
extern double corrigeDistor;

extern int temInterface;
extern int aplicaVOF;
extern int correcForcCorp;
extern double cflG;

extern int reiniciaVF;

extern int iliq;
extern int igas;*/

struct elemento{
    int nvert;//numero de vertices
    int dim;//dimensao do problema
    int indEle;//indice do elemento
    int nele;//numero de elementos da discretizacao
    int* noElem;//vetor com o indice dos nos
    int* indFace;//vetor com o indice do elemento vizinho para uma das faces do elemento
    int* ccFace;//vetor indicando a condicao de contorno de uma face
    vector<int> indVizCres;
    double* orientFace;//vetor corrigindo a orientacao da face de um elemento,
    //para garantir que sempre esteja apontada para fora
    double* centroElem;//coordenadas do centro geométrico do elemento
    double* centroideElem;//coordenadas do centro de massa do elemento
    double** centroideFace;//coordenadas com o centro de massa da face
    double** coordVert;//coordenadas de cada vertice do elemnto
    double** dCF;//vetor apontando do centroide do elemnto para o centro da face
    double* dCFMod;//modulo do vetor
    double vElem;//volume/area do elemnto
    double *subVol;
    int** noFace;
    double** sFace;//vetor da área da face
    double* sFaceMod;//modulo do vetor
    double* ownFace;//vetor indicando se o elemento é ou não "dono" do vetor area
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double** vecE;//vetor unitario na orientacao entre o centroide de do elemento e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elemento e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementos
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementos e a face do elemento
    double** fIfC;//vetor entre a intersecao e o centroide da face

	double* difuPres;
	double* difuPresRC;
	double** vecSDif;
	double* vecSDifMod;
	double* HcU;
	double* HcV;
	double* HcP;
	double* HcT;

	double presC;
	double presCEx;
	double presCI;
	double presC0;
	double presCcor;
	double presCIcor;
	double uC;
	double uCI;
	double uC0;
	double uCEx;
	double vC;
	double vCI;
	double vC0;
	double vCEx;
	double tempC;
	double tempCI;
	double tempC0;
	double tCEx;
	double holC;
	double holCI;
	double holC0;
	double taxaDeform;
	double* gradGreenPres;
	double* gradGreenPresCor;
	double* gradGreenU;
	double* gradGreenV;
	double* gradGreenTemp;
	double* gradGreenHol;
	double* gradGreenPresI;
	double* gradGreenPresCorI;
	double* gradGreenUI;
	double* gradGreenVI;
	double* gradGreenTempI;
	double* gradGreenHolI;
	double cond;
	double cp;
	double rho;
	double visc;
	double beta;
	double tempRef;
	double a0U;
	double a0V;
	double *B2Med;
	double **BMedF;
};

class elem2d{
	public:
	elemento cel2D;
	int nvizinho;
	elemento** vizinho;
	int* kvizinho;
	int* confinado;

	double deriFonteMass;
	double FonteMass;
	double* presF;
	double* presFcor;
	double* uF;
	double* vF;
	double* tempF;
	double* holF;
	double* tUpw;
	double* uUpw;
	double* vUpw;
	double* holUpw;
	double* tMed;
	double* uMed;
	double* vMed;
	double* holMed;
	double* tDow;
	double* uDow;
	double* vDow;
	double* holDow;
	double* tK;
	double* uK;
	double* vK;
	double* holK;
	double* tL;
	double* uL;
	double* vL;
	double* holL;
	double* uRC;
	double* vRC;
	double* uRC0;
	double* vRC0;
	double* massF;
	double* coefUHRC;
	double* coefUHRV;
	double* fonteUHR;
	double* coefVHRC;
	double* coefVHRV;
	double* fonteVHR;
	double* coefTHRC;
	double* coefTHRV;
	double* fonteTHR;
	double fluxCal;
	double dt;
	double** gradPresface;
	double** gradPresfaceCor;
	double** gradUface;
	double** gradVface;
	double* taxadeformFace;
	double* viscFace;
	int perm;
	int trans;
	double* ccInU;
	double* ccInV;
	double* ccWvel;
	double* ccPres;
	double* ccWall;
	double massTot;
	///////////////////////////////////////////////////////////////////////////
	double* ccTD;
	double* ccTVN;
	double* ccHR;
	double* ccTambR;
	double DCCN;
	double extrapSuaveCCN;

	double residuoU;
	double residuoV;
	double denoResU;
	double denoResV;
	double residuoT;
	double denoResT;

	double* CFL;
	double* dtCFL;
	double CFLC;

	ProFluColVF flucVF;

    Vcr<double> TLUV;//vetor livre da matriz local;
    FullMtx<double> localUV;//matriz local;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vcr<double> TLU;//vetor livre da matriz local;
    FullMtx<double> localU;//matriz local;
    Vcr<double> TLV;//vetor livre da matriz local;
    FullMtx<double> localV;//matriz local;
    Vcr<double> TLPCor;//vetor livre da matriz local;
    FullMtx<double> localPCor;//matriz local;

	Vcr<double> TLT;//vetor livre da matriz local;
    FullMtx<double> localT;//matriz local;

    varGlob1D* vg1dSP;

    /*detTempo temp;
    detCC CC;
    int partidaVF;
    double relax;
    double relaxPcor;
    int nvfHR;
    int nvfHRHol;
    int atrasaHR;
    double grav;*/


	elem2d(varGlob1D* Vvg1dSP=0,double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int nele=0, int nno=0, int i=0,
			double vdt=0., int vperm=1, int vtrans=0,ProFluColVF vflucVF=ProFluColVF(),
			double vpres=0.,double vu=0.,double vv=0.,double vt=0.,double vfluxCal=0.,double vcond=0.,double vcp=0.,
			double vrho=0.,double vvisc=0.,double vbeta=0./*,int vpartidaVF=0,double vrelax=0,double vrelaxPcor=0,
			int vnvfHR=0,int vnvfHRHol=0,int vatrasaHR=0,double vgrav=0.,detTempo vtemp=detTempo(),detCC vCC=detCC()*/);
	elem2d(const elem2d&); //construtor por copia
	elem2d& operator=(const elem2d&);

	~elem2d(){
		if(cel2D.nvert>0 && cel2D.nele>0){
			delete[] cel2D.noElem;
			delete[] cel2D.indFace;
			delete[] cel2D.ccFace;
			delete[] cel2D.orientFace;
			delete[] cel2D.centroElem;
			delete[] cel2D.centroideElem;
			delete[] cel2D.dCFMod;
			delete[] cel2D.sFaceMod;
			delete[] cel2D.subVol;
			delete[] cel2D.ownFace;
			delete[] cel2D.vecSDifMod;
			delete[] cel2D.HcU;
			delete[] cel2D.HcV;
			delete[] cel2D.HcP;
			delete[] cel2D.HcT;
			delete[] ccInU;
			delete[] ccInV;
			delete[] ccWvel;
			delete[] ccPres;
			delete[] ccWall;
			/////////////////////////////////////////////////////////////////////////////////////////////////
			delete[] ccTD;
			delete[] ccTVN;
			delete[] ccHR;
			delete[] ccTambR;
			///////////////////////////////////////////////////////////////////////////////////////////////////
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
				delete[] gradPresface[i];
				delete[] gradPresfaceCor[i];
				delete[] gradUface[i];
				delete[] gradVface[i];
				///////////////////////////////////////////////////////////
				delete[] cel2D.vecSDif[i];
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
			delete[] cel2D.gradGreenPres;
			delete[] cel2D.gradGreenPresCor;
			delete[] cel2D.gradGreenU;
			delete[] cel2D.gradGreenV;
			delete[] cel2D.gradGreenTemp;
			delete[] cel2D.gradGreenHol;

			delete[] cel2D.gradGreenPresI;
			delete[] cel2D.gradGreenPresCorI;
			delete[] cel2D.gradGreenUI;
			delete[] cel2D.gradGreenVI;
			delete[] cel2D.gradGreenTempI;
			delete[] cel2D.gradGreenHolI;
			//////////////////////////////////////////////////////////////////////////////////
			delete[] presF;
			delete[] presFcor;
			delete[] uF;
			delete[] vF;
			delete[] holF;
			delete[] tempF;
			delete[] tUpw;
			delete[] uUpw;
			delete[] vUpw;
			delete[] holUpw;
			delete[] tMed;
			delete[] uMed;
			delete[] vMed;
			delete[] holMed;
			delete[] tDow;
			delete[] uDow;
			delete[] vDow;
			delete[] holDow;
			delete[] tK;
			delete[] uK;
			delete[] vK;
			delete[] holK;
			delete[] tL;
			delete[] uL;
			delete[] vL;
			delete[] holL;
			delete[] uRC;
			delete[] vRC;
			delete[] uRC0;
			delete[] vRC0;
			delete[] massF;
			delete[] taxadeformFace;
			delete[] viscFace;
			delete[] gradPresface;
			delete[] gradPresfaceCor;
			delete[] gradUface;
			delete[] gradVface;

			delete[] coefUHRC;
			delete[] coefUHRV;
			delete[] fonteUHR;
			delete[] coefVHRC;
			delete[] coefVHRV;
			delete[] fonteVHR;
			delete[] coefTHRC;
			delete[] coefTHRV;
			delete[] fonteTHR;

			delete[] cel2D.difuPres;
			delete[] cel2D.difuPresRC;
			delete[] cel2D.vecSDif;
			for(int i=0;i<cel2D.nvert;i++){
				delete[] cel2D.BMedF[i];
			}
			delete[] cel2D.B2Med;
			delete[] cel2D.BMedF;
			delete[] confinado;
			delete[] dtCFL;
			delete[] CFL;
		}
		/*if(nvizinho>0){
			delete[] vizinho;
			delete[] kvizinho;
		}*/
	}

	void buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle);
	void menorIndViz();
	void faceDetalhes();
	void vazMass(double rlx,int noRelax=0);
	void calcVarFaceUV(int face);
	void calcVarFaceTemp(int face);
	void calcVarFacePres(int face);
	void calcVarFacePresCor(int face);
	void calcVarFaceHol(int face);
	void impliHR(int face);
	void expliHR(int face);
	void impliHRT(int face);
	void expliHRT(int face);
	void expliHRHol(int face);
	void impliCoefHRT(int face);
	void expliCoefHRT(int face);
	void impliCoefHR(int face);
	void expliCoefHR(int face);
	void tipoCCTemp(int i,int& diri, int& vn, int& rich, int& acoplado,int& kcc);
	void tipoCC(int i,int& inl, int& out, int& wall, int& sim,int& kcc);
	void atualizaCCTemp(int i);
	void atualizaCC(int i);
	void calcForcCorp();
	void calcGradGreenUV();
	void calcGradGreenPres();
	void calcGradGreenTemp(int inicia=0);
	void calcGradGreenPresCor();
	void calcGradGreenHol(int inicia=0);
	double escalar(double* v1, double* v2, int dimV);
	int achaInd(int i);
	void calSupDifu();
	void explicitUV();
	void GeraLocalUV(double rlx);
	void GeraLocalU();
	void GeraLocalV();
	void GeraLocalPcor();
	void GeraLocalT(double rlx);
	void explicitHol();
	void atualizaCor();
	void indraz(int& ind/*valor retornado do momento na serie de tempo logo anterior ao momento que se quer avaliar*/,
			double& raz/*posicao relativa de tempo, entre a posicao ind e ind+1*/,
			double tempo/*momento que se quer avaliar na serie de tempo para se fazer a interpolacao*/,
			int parserie/*tamanho da serie de tempo*/,
			double* serietemp/*vetor com os tempos da serie de tempo*/);//metodo que obtem o valor interpolado
	//de uma variavel de uma serie de tempo



};

#endif /* ELEM2D_H_ */
