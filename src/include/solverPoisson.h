/*
 * solverPoisson.h
 *
 *  Created on: Aug 30, 2023
 *      Author: Eduardo
 */

#ifndef SOLVERPOISSON_H_
#define SOLVERPOISSON_H_
#define _USE_MATH_DEFINES

#include "Elem2DPoisson.h"
#include "Log.h"
#include "Malha2DPoisson.h"
#include "Matriz.h"
#include "Vetor.h"
#include "dados1Poisson.h"
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

class solverP {
  public:
    dadosP dados;
    Vcr<double> termolivre;
    SparseMtx<double> matglob;
    malha2d malha;
    int kontaTempo;
    int indCel;
    varGlob1D *vg1dSP;
    int precn;
    solverP(varGlob1D *Vvg1dSP, string nomeArquivoEntrada, double vCondGlob = 0., double vCondLoc = 0., double vhE = 0., double vhInt = 0.,
            double vTint = 0., double vTamb = 0., double vdiamI = 0., double vdiamE = 0., int indcel = 0);
    solverP();
    solverP(const solverP &);
    solverP &operator=(const solverP &);
    void indrazT(int &ind, double &raz);
    void permanentePoisson();
    void inicializaPermanentePoisson();
    void inicializaTransientePoisson();
    double defineDeltPoisson();
    void transientePoissonDummy(double delt);
    void transientePoisson(double delt);
    void imprimePermanente(int indTramo = -1);
    void finalizaPassoTransiente(double vdelt, int indTramo = -1);
    void FeiticoDoTempo();
};

#endif /* SOLVERPOISSON_H_ */