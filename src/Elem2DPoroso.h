/*
 * elem2dPoroso.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef elem2dPorosoPOISSON_H_
#define elem2dPorosoPOISSON_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "celRad.h"
#include "estruturasPoroso.h"
#include "estruturaTabDin.h"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "variaveisGlobais1D.h"

//extern detTempoPoroso temp;
//extern detCCPoroso CC;

//extern double tempo;

struct elementoPoroso{
    int nvert;//numero de vertices
    int dim;//dimensao do problema
    int indEle;//indice do elementoPoroso
    int nele;//numero de elementoPorosos da discretizacao
    int* noElem;//vetor com o indice dos nos
    int* indFace;//vetor com o indice do elementoPoroso vizinho para uma das faces do elementoPoroso
    int* ccFace;//vetor indicando a condicao de contorno de uma face
    vector<int> indVizCres;
    double* orientFace;//vetor corrigindo a orientacao da face de um elementoPoroso,
    //para garantir que sempre esteja apontada para fora
    double* centroElem;//coordenadas do centro geométrico do elementoPoroso
    double* centroideElem;//coordenadas do centro de massa do elementoPoroso
    double** centroideFace;//coordenadas com o centro de massa da face
    double** coordVert;//coordenadas de cada vertice do elemnto
    double** dCF;//vetor apontando do centroide do elemnto para o centro da face
    double* dCFMod;//modulo do vetor
    double vElem;//volume/area do elemnto
    int** noFace;
    double** sFace;//vetor da área da face
    double* sFaceMod;//modulo do vetor
    double* ownFace;//vetor indicando se o elementoPoroso é ou não "dono" do vetor area
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double** vecE;//vetor unitario na orientacao entre o centroide de do elementoPoroso e um dos seus vizinhos
    double* modE;//distancia entre o centroide do elementoPoroso e um dos seus vizinho
    double** vecT;//vetor unitario normal a vecE
    double* modT;
    double* fatG;//razao de distancia entre a intersecao da face e o centroide e a
    //distancia de dois centroides de elementoPorosos
    double* angES;//angulo entre a corda dos dois centroides e o vetor da area da face
    double** fInter;//coordenadas da intersecao da corda entre os centroides dos elementoPorosos e a face do elementoPoroso
    double** fIfC;//vetor entre a intersecao e o centroide da face

	double** vecSDif;
	double* vecSDifMod;

    double presC;
	double presCI;
	double presC0;

	double* gradGreenP;
	double* gradGreenPI;
	double* gradGreenZdatum;
	double* gradGreenZdatumI;
	double* gradGreenAZdatum;
	double* gradGreenAZdatumI;
	double* gradGreenPcOG;
	double* gradGreenPcAO;
	double* gradGreenPcAOI;
	double* gradGreenSw;
	double* gradGreenSwI;

	double kY;
	double kX;
	double poro;
	double compRoc;

	double sLC;
	double sLI;
	double sWC;
	double sWI;

	double alfC;
	double alfC0;

	double vbo;
	double rhostd;
	double rhogstd;
	double rhoastd;

	double rhoP;
	double rhogP;
	double rhoaP;

	double rhoP0;
	double rhogP0;
	double rhoaP0;

	double pcOG;
	double pcAO;

	double kabsol;
	double zD;

	int tipoModelOleo;
	double compOleo;
	double rhoRef;
	double pBolha;

	elementoPoroso(){
	    nvert=0;//numero de vertices
	    dim=0;//dimensao do problema
	    indEle=0;//indice do elementoPoroso
	    nele=0;//numero de elementoPorosos da discretizacao
	    noElem=0;//vetor com o indice dos nos
	    indFace=0;//vetor com o indice do elementoPoroso vizinho para uma das faces do elementoPoroso
	    ccFace=0;//vetor indicando a condicao de contorno de uma face
	    orientFace=0;//vetor corrigindo a orientacao da face de um elementoPoroso,
	    //para garantir que sempre esteja apontada para fora
	    centroElem=0;//coordenadas do centro geométrico do elementoPoroso
	    centroideElem=0;//coordenadas do centro de massa do elementoPoroso
	    indVizCres=vector<int>(0);
	    centroideFace=0;//coordenadas com o centro de massa da face
	    coordVert=0;//coordenadas de cada vertice do elemnto
	    dCF=0;//vetor apontando do centroide do elemnto para o centro da face
	    dCFMod=0;//modulo do vetor
	    vElem=0;//volume/area do elemnto
	    noFace=0;
	    sFace=0;//vetor da área da face
	    sFaceMod=0;//modulo do vetor
	    ownFace=0;//vetor indicando se o elementoPoroso é ou não "dono" do vetor area
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    vecE=0;//vetor unitario na orientacao entre o centroide de do elementoPoroso e um dos seus vizinhos
	    modE=0;//distancia entre o centroide do elementoPoroso e um dos seus vizinho
	    vecT=0;//vetor unitario normal a vecE
	    modT=0;
	    fatG=0;//razao de distancia entre a intersecao da face e o centroide e a
	    //distancia de dois centroides de elementoPorosos
	    angES=0;//angulo entre a corda dos dois centroides e o vetor da area da face
	    fInter=0;//coordenadas da intersecao da corda entre os centroides dos elementoPorosos e a face do elementoPoroso
	    fIfC=0;//vetor entre a intersecao e o centroide da face

		vecSDif=0;
		vecSDifMod=0;

	    presC=0;
		presCI=0;
		presC0=0;

		gradGreenP=0;
		gradGreenPI=0;
		gradGreenZdatum=0;
		gradGreenZdatumI=0;
		gradGreenAZdatum=0;
		gradGreenAZdatumI=0;
		gradGreenPcOG=0;
		gradGreenPcAO=0;
		gradGreenPcAOI=0;
		gradGreenSw=0;
		gradGreenSwI=0;

		kY=0;
		kX=0;
		poro=0;
		compRoc=0;

		sLC=0;
		sLI=0;
		sWC=0;
		sWI=0;

		alfC=0;
		alfC0=0;

		vbo=0;
		rhostd=0;
		rhogstd=0;
		rhoastd=0;

		rhoP=0;
		rhogP=0;
		rhoaP=0;

		rhoP0=0;
		rhogP0=0;
		rhoaP0=0;

		pcOG=0;
		pcAO=0;

		kabsol=0;
		zD=0;

		pBolha=1000.;
		tipoModelOleo=1;
		compOleo=1e-5;
		rhoRef=900;
	}

};

class elem2dPoroso{
	public:
	detCCPoroso CC;
	elementoPoroso cel2D;
	int nvizinho;
	elementoPoroso** vizinho;
	int* kvizinho;
	varGlob1D* vg1dSP;
	/*elem2dPoroso(double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int nele=0, int nno=0,
			int vperm=1, int vtrans=0,int i=0,double vfluxCal=0.,double vdt=0.,double vKX=0.,
			double vKY=0.,double vporo=0.,double vcompresPoro=0.,double vPcentro=0,double vPint=0.,
			double vpresRes=0.,double vQL=0., double vQR=0, double vtRes=0,double vzD=0,
			double vdh=1,double vsW=0,double vsWRes=0,
			double vsatConata=0,const double vip=1.,tabelaPemRelOA vkRelOA=tabelaPemRelOA(),tabelaPemRelOG vkRelOG=tabelaPemRelOG(),
			tabelaPresCapOA vpcOA=tabelaPresCapOA(),tabelaPresCapGO vpcGO=tabelaPresCapGO(),
			ProFluCol vfluc=ProFluCol(), ProFlu vflup=ProFlu());*/
	elem2dPoroso(varGlob1D* Vvg1dSP=0,double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int nele=0, int nno=0,
				int vperm=0, int vtrans=1,int i=0,ProFlu vflup=ProFlu(),tabelaPemRelOA vkRelOA=tabelaPemRelOA(),tabelaPemRelOG vkRelOG=tabelaPemRelOG(),
				tabelaPresCapOA vpcOA=tabelaPresCapOA(),tabelaPresCapGO vpcGO=tabelaPresCapGO());
	elem2dPoroso(const elem2dPoroso&); //construtor por copia
	elem2dPoroso& operator=(const elem2dPoroso&);

	double deriFonteT;
	double FonteT;

	double* presF;
	double* swF;
	double* alfF;
	double* Qtotal;
	double* Qw;
	double* Qoleo;
	double* Qgas;
	double* rhoF;
	double* rhogF;
	double* rhoaF;
	double* rhoZF;
	double* rhoaZF;
	double* mioF;
	double* migF;
	double* miaF;
	double* kmedF;
	double* kmedGF;
	double* kmedAF;
	double* darcyOF;
	double* darcyGF;
	double* darcyAF;
	double* pcAOF;

	double fluxCal;
	double dt;
    double dtSL;
    double dtSW;
	double dh;
	int reiniciaSL;
	int reiniciaSW;

	double** gradPface;
	double** gradZface;
	double** gradPcOGface;
	double** gradPcAOface;

	int perm;
	int trans;
	double* ccPD;
	double* ccSat;
	double* ccPVN;
	double* ccHR;
	double* ccPambR;
	double* coefTHRC;
	double* coefTHRV;
	double* fonteTHR;
	ProFluCol fluc;
	ProFlu flup;
	tabelaPemRelOA kRelOACel;
	tabelaPemRelOG kRelOGCel;
	tabelaPresCapOA pcOACel;
	tabelaPresCapGO pcGOCel;

	double satConata;
	double ip;
	double tRes;
	double sLRes;
	double sWRes;
	double pRes;

    Vcr<double> TL;//vetor livre da matriz local;
    FullMtx<double> local;//matriz local;

	~elem2dPoroso(){
		if(cel2D.nvert>0){
			//if(cel2D.indEle==909){
		//		int para;
		//		para=0;
			//}
			delete[] cel2D.noElem;
			delete[] cel2D.indFace;
			delete[] cel2D.ccFace;
			delete[] cel2D.orientFace;
			delete[] cel2D.centroElem;
			delete[] cel2D.centroideElem;
			delete[] cel2D.dCFMod;
			delete[] cel2D.sFaceMod;
			delete[] cel2D.ownFace;
			delete[] ccPD;
			delete[] ccSat;
			delete[] ccPVN;
			delete[] ccHR;
			delete[] ccPambR;
			delete[] cel2D.vecSDifMod;
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
				delete[] gradPface[i];
				delete[] gradZface[i];
				delete[] gradPcOGface[i];
				delete[] gradPcAOface[i];

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
			delete[] cel2D.gradGreenP;
			delete[] cel2D.gradGreenPI;
			delete[] cel2D.gradGreenZdatum;
			delete[] cel2D.gradGreenZdatumI;
			delete[] cel2D.gradGreenAZdatum;
			delete[] cel2D.gradGreenAZdatumI;
			delete[] cel2D.gradGreenPcOG;
			delete[] cel2D.gradGreenPcAO;
			delete[] cel2D.gradGreenPcAOI;
			delete[] cel2D.gradGreenSw;
			delete[] cel2D.gradGreenSwI;
			//////////////////////////////////////////////////////////////////////////////////
			delete[] presF;
			delete[] swF;
			delete[] alfF;
			delete[] Qtotal;
			delete[] Qw;
			delete[] Qoleo;
			delete[] Qgas;
			delete[] rhoF;
			delete[] rhogF;
			delete[] rhoaF;
			delete[] rhoZF;
			delete[] rhoaZF;
			delete[] mioF;
			delete[] migF;
			delete[] miaF;
			delete[] kmedF;
			delete[] kmedGF;
			delete[] kmedAF;
			delete[] darcyOF;
			delete[] darcyGF;
			delete[] darcyAF;
			delete[] pcAOF;
			delete[] gradPface;
			delete[] gradZface;
			delete[] gradPcOGface;
			delete[] gradPcAOface;


			delete[] coefTHRC;
			delete[] coefTHRV;
			delete[] fonteTHR;

			delete[] cel2D.vecSDif;
		}
		/*if(nvizinho>0){
			delete[] vizinho;
			delete[] kvizinho;
		}*/
	}

	void buscaVizinho(int** noEle,int* face,int elem,int nVert, int nEle);
	void menorIndViz();
	void faceDetalhes();
	double interpolaTabela(int nserie, double valx, double* x, double* y);
	double fkO(double satW, double satG);
	void tipoCC(int i,int& diri, int& vn, int& rich, int& acoplado,int& kcc);
	void atualizaCC(int i);
	void calcGradGreen();
	double cflA();
	void evoluiSW(int& reinicia);
	double escalar(double* v1, double* v2, int dimV);
	int achaInd(int i);
	void calSupDifu();
	void GeraLocal(double relax);
	void indraz(int& ind/*valor retornado do momento na serie de tempo logo anterior ao momento que se quer avaliar*/,
			double& raz/*posicao relativa de tempo, entre a posicao ind e ind+1*/,
			double tempo/*momento que se quer avaliar na serie de tempo para se fazer a interpolacao*/,
			int parserie/*tamanho da serie de tempo*/,
			double* serietemp/*vetor com os tempos da serie de tempo*/);//metodo que obtem o valor interpolado
	//de uma variavel de uma serie de tempo



};

#endif /* elem2dPorosoPOISSON_H_ */
