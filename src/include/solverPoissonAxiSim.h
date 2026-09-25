/*
 * solverPoisson.h
 *
 *  Created on: Aug 30, 2023
 *      Author: Eduardo
 */

#ifndef SOLVERAxiSim_H_
#define SOLVERAxiSim_H_
#define _USE_MATH_DEFINES

//#include "dados1PoissonAxiSim.h"
#include "Elem2DPoissonAxiSim.h"
#include "Log.h"
#include "Malha2DPoissonAxiSim.h"
#include "Matriz.h"
#include "Vetor.h"
#include "estruturasPoisson.h"
#include "variaveisGlobais1D.h"
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
#include <vector>

using namespace std;

class dadosPAxiSim;

class solverPAxiSim {
  public:
	dadosPAxiSim* dados;
    Vcr<double> termolivre;
    SparseMtx<double> matglob;
    malha2dAxiSim malha;
    int kontaTempo;
    double tempProd;
    varGlob1D *vg1dSP;
    int precn;
    solverPAxiSim(varGlob1D *Vvg1dSP, string nomeArquivoEntrada, double* vresGlob = 0,
            double* vTint = 0, double* vdiamI = 0
			, double* dxCel=0, int vncel=0, double* vgeoterm=0, double vtempProd=0.);
    solverPAxiSim();
    solverPAxiSim(const solverPAxiSim &);
    solverPAxiSim &operator=(const solverPAxiSim &);
    ~solverPAxiSim();

    double ResForm(double tempprod, double diaExt,double condform, double rhoform, double cpform);
    void indrazT(int &ind, double &raz);
    void permanentePoisson();
    void inicializaPermanentePoisson();
    void inicializaTransientePoisson(std::vector<double>& tInicial);
    double defineDeltPoisson();
    void transientePoissonDummy(double delt);
    void transientePoisson(double delt);
    void imprimePermanente(int indTramo = -1);
    void finalizaPassoTransiente(double vdelt, int indTramo = -1);
    void FeiticoDoTempo();
};

#endif /* SOLVERPOISSON_H_ */
