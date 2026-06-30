/*
 * solver.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Eduardo
 */

#ifndef SOLVER_H_
#define SOLVER_H_
#define _USE_MATH_DEFINES // Enables access to the M_PI constant

#include "Elem2D.h"
#include "FerramentasNumericas.h"
#include "Geometria.h"
#include "Log.h"
#include "Malha2D.h"
#include "Matriz.h"
#include "PropFluColVF.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "estruturas.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;

// Output stream used to generate simulation data file reports
extern ofstream arqRelatorioPerfis;

// Logger instance used by the simulation
extern Logger logger;

// Path to the primary input file
extern string pathArqEntrada;

// Path to the PVTSIM and Snapshot input files
extern string pathArqExtEntrada;

// Output file path and prefix used for POCO_INJETOR
extern string pathPrefixoArqSaida;

// Full path to the Snapshot output file
extern string arqSaidaSnapShot;

// Directory containing the simulation output files
extern string diretorioSaida;

extern detTempo tempVF;
extern detProp prop;
extern detMapProp mapprop;
extern detCI CI;
extern detCC CC;
extern int nthrdMatriz;

class solv2D {
  public:
    ProFluColVF flucVF;
    cortedutoVF corteVF;
    materialVF *matVF;
    detMalhaHom **malhaH;
    string nomeArquivoEntrada;
    string nomeArquivoLog;
    detInterFace interface;
    int unv;
    int nthrdLocal;
    varGlob1D *vg1dSP;
    int solverMat;
    int rankLU;
    int colore;
    double dtSegur;
    int cicloSegur;
    int nPrime;
    int equilterm;
    int impliAcopTerm;
    int iteraTerm;
    solv2D() {
        nomeArquivoEntrada = "";
        nomeArquivoLog = "";
        flucVF = ProFluColVF();
        corteVF = cortedutoVF();
        interface = detInterFace();
        matVF = 0;
        malhaH = 0;
        unv = 0;
        vg1dSP = 0;
        nthrdLocal = 1;
        rankLU = -1;
        colore = 0;
        dtSegur = 0.01;
        cicloSegur = 100;
        nPrime = 0;
        equilterm = 0;
        solverMat = 0;
        impliAcopTerm = 0;
        iteraTerm = 200;
    };
    solv2D(string vnomeArquivoEntrada, string vnomeArquivoLog, varGlob1D *Vvg1dSP = 0) {
        nomeArquivoEntrada = vnomeArquivoEntrada;
        nomeArquivoLog = vnomeArquivoLog;
        flucVF = ProFluColVF();
        corteVF = cortedutoVF();
        interface = detInterFace();
        matVF = 0;
        malhaH = 0;
        unv = 0;
        vg1dSP = Vvg1dSP;
        nthrdLocal = 1;
        rankLU = -1;
        colore = 0;
        dtSegur = 0.01;
        cicloSegur = 100;
        nPrime = 0;
        equilterm = 0;
        solverMat = 0;
        impliAcopTerm = 0;
        iteraTerm = 200;
    };
    ~solv2D() {
        if ((*vg1dSP).acop == 1) {
            delete[] matVF;
            delete[] corteVF.diam;
            delete[] corteVF.indmat;
            delete[] corteVF.discre;
        }
        if ((*vg1dSP).nxMH > 0) {
            for (int i = 0; i < (*vg1dSP).nxMH; i++)
                delete[] malhaH[i];
            delete[] malhaH;
        }
    }
    void parse_materiais(Value &material_json);
    void parse_corte(Value &corte_json);
    void parse_fluido_complementar(Value &fluido_json);
    void parse_tempo(Value &tempo_json);
    void parse_Prop(Value &prop_json);
    void parse_CI(Value &CI_json);
    void parse_CC(Value &CC_json);
    void parse_malha(Value &malha_json, double &dx, double &dy, string &elearq, string &noarq, string &polyarq);
    void parse_Interface(Value &Interface_json);
    void calcYInter(double x, double *y);
    double calcYInterUno(double x, double ymed);
    void parse_mapProp(Value &mapProp_json);
    double SIGN(double a, double b);
    double zriddr(double x1, double x2, double a, double b, double ymed);
    void iniciaProp(malha2dVF &malha);
    void indrazT(int &ind, double &raz);
    void malhaRegular(double &dx, double &dy, malha2dVF &malha);
    void imprimeMalhaRegular(malha2dVF &malha, int var);
    double tempMed(malha2dVF &malha);
    double fluxMed(malha2dVF &malha);
    void paredeContorno(malha2dVF &malha, unordered_map<int, int> &indPar, unordered_map<int, int> &indPar2, int &nPar);
    double viscMed(malha2dVF &malha);
    double taxaMed(malha2dVF &malha);
    double velMed(malha2dVF &malha);
    double tempMedParede(malha2dVF &malha);
    double areaMedParede(malha2dVF &malha, double &condMed);
    void resolve();
};

#endif /* SOLVER_H_ */