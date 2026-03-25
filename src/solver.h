/*
 * solver.h
 *
 *  Created on: 3 de dez. de 2023
 *      Author: Eduardo
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <chrono>
#include <unordered_map>
using namespace std;
//#include <ctime.h>
//#include <ctime.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <algorithm>
#include "FerramentasNumericas.h"
#include "Log.h"
#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "Elem2D.h"
#include "Malha2D.h"
#include "estruturas.h"
#include "Geometria.h"
#include "TrocaCalor.h"
#include "PropFluColVF.h"
#include "variaveisGlobais1D.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/pt_BR.h"

//---------------------------------------------------------------------------
// criar relatorio dos arquivos de dados de saida da simulacao
extern ofstream arqRelatorioPerfis;

// criar objeto de logger
extern Logger logger;

// criar string do path do arquivo de entrada
extern string pathArqEntrada;

// criar string do path dos arquivos de entrada PVTSIM e SnapShot
extern string pathArqExtEntrada;

// criar string do path e prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;

// criar string do arquivo de saida para o Snapshot, incluindo o path
extern string arqSaidaSnapShot;

//diretorio contendo os arquivos de saida
extern string diretorioSaida;

//extern string elearq;
//extern string noarq;
//extern string polyarq;

/*extern int partidaVF;
extern double relaxVF;
extern double relaxVFExpli;
extern double relaxVFPcor;
extern double gravVF;
extern double vertY;
extern int iterPresCor;
extern double tempo;
extern int nvfHR;
extern int nvfHRHol;
extern int atrasaHR;
extern int nxMH;
extern int nyMH;
extern int metodoAcopPV;
extern double angY;
extern double angX;
extern double xmin;
extern double xmax;
extern double ymin;
extern double ymax;
extern int eleYmaxWall;
extern double mulFC;
extern double corrigeDistor;
extern int tipoRes;
extern double cflG;
extern int temInterface;
extern int reiniciaVF;
extern int aplicaVOF;
extern int correcForcCorp;
extern int iliq;
extern int igas;

extern double centroX;
extern double centroY;
extern int nmaterialVF;
extern int acop;
extern double angAcop;
extern double tParede;
extern double tAmb;
extern double vAmb;
extern int amb;
extern int ntrd;*/

extern detTempo tempVF;
extern detProp prop;
extern detMapProp mapprop;
extern detCI CI;
extern detCC CC;

extern int nthrdMatriz;
//extern detMalhaHom** malhaH;
//extern detInterFace interface;
//extern materialVF* matVF;//vetor com os materiais cadastrados no Json
//extern cortedutoVF corteVF;//vetor com as secoes transversais cadastradas no json
//extern ProFluColVF flucVF;
//extern DadosGeo dutosMRT;
//extern TransCal transfer;
//extern TransCal transfer2;

class solv2D{
	public:
	ProFluColVF flucVF;
	cortedutoVF corteVF;
	materialVF* matVF;
	detMalhaHom** malhaH;
	string nomeArquivoEntrada;
	string nomeArquivoLog;
	detInterFace interface;
	int unv;
	int nthrdLocal;
	varGlob1D* vg1dSP;
	int solverMat;
	int rankLU;
	int colore;
	double dtSegur;
	int cicloSegur;
	int nPrime;
	int equilterm;
	int impliAcopTerm;
	int iteraTerm;
	solv2D(){
		nomeArquivoEntrada="";
		nomeArquivoLog="";
		flucVF=ProFluColVF();
		corteVF=cortedutoVF();
		interface=detInterFace();
		matVF=0;
		malhaH=0;
		unv=0;
		vg1dSP=0;
		nthrdLocal=1;
		rankLU=-1;
		colore=0;
		dtSegur=0.01;
		cicloSegur=100;
		nPrime=0;
		equilterm=0;
		solverMat=0;
		impliAcopTerm=0;
		iteraTerm=200;
	};
	solv2D(string vnomeArquivoEntrada,string vnomeArquivoLog,varGlob1D* Vvg1dSP=0){
			nomeArquivoEntrada=vnomeArquivoEntrada;
			nomeArquivoLog=vnomeArquivoLog;
			flucVF=ProFluColVF();
			corteVF=cortedutoVF();
			interface=detInterFace();
			matVF=0;
			malhaH=0;
			unv=0;
			vg1dSP=Vvg1dSP;
			nthrdLocal=1;
			rankLU=-1;
			colore=0;
			dtSegur=0.01;
			cicloSegur=100;
			nPrime=0;
			equilterm=0;
			solverMat=0;
			impliAcopTerm=0;
			iteraTerm=200;
	};
	~solv2D(){
	    if((*vg1dSP).acop==1){
	    	delete[] matVF;
			delete[] corteVF.diam;
			delete[] corteVF.indmat;
			delete[] corteVF.discre;
	    }
	    if((*vg1dSP).nxMH>0){
	    	for(int i=0;i<(*vg1dSP).nxMH;i++) delete[] malhaH[i];
	    	delete[] malhaH;
	    }
	}
	void parse_materiais(Value& material_json);
	void parse_corte(Value& corte_json);
	void parse_fluido_complementar(Value& fluido_json);
	void parse_tempo(Value& tempo_json);
	void parse_Prop(Value& prop_json);
	void parse_CI(Value& CI_json);
	void parse_CC(Value& CC_json);
	void parse_malha(Value& malha_json, double& dx, double& dy,string& elearq,string& noarq,string& polyarq);
	void parse_Interface(Value& Interface_json);
	void calcYInter(double x, double* y);
	double calcYInterUno(double x,double ymed);
	void parse_mapProp(Value& mapProp_json);
	double SIGN(double a, double b);
	double  zriddr(double x1,double x2,double a,double b,double ymed);
	void iniciaProp(malha2dVF& malha);
	void indrazT(int& ind, double& raz);
	void malhaRegular(double& dx, double& dy, malha2dVF& malha);
	void imprimeMalhaRegular(malha2dVF& malha, int var);
	double tempMed(malha2dVF& malha);
	double fluxMed(malha2dVF& malha);
	void paredeContorno(malha2dVF& malha, unordered_map<int, int>& indPar, unordered_map<int, int>& indPar2, int& nPar);
	double viscMed(malha2dVF& malha);
	double taxaMed(malha2dVF& malha);
	double velMed(malha2dVF& malha);
	double tempMedParede(malha2dVF& malha);
	double areaMedParede(malha2dVF& malha, double& condMed);
	void resolve();
};



#endif /* SOLVER_H_ */
