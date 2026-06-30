/*
 * solver3DPoisson.h
 *
 *  Created on: Aug 30, 2023
 *      Author: Eduardo
 */

#ifndef SOLVER3DPOISSON_H_
#define SOLVER3DPOISSON_H_

#include "Elem3DPoisson.h"
#include "Geometria.h"
#include "Log.h"
#include "Malha3DPoisson.h"
#include "Matriz.h"
#include "Vetor.h"
#include "dados3DPoisson.h"
#include "estruturasPoisson3D.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <mutex>
#include <omp.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <utility> // Provides std::pair
#include <vector>

using namespace std;

class solverP3D {
  public:
    dadosP3D dados;
    malha3d malha;
    int nacop;
    double ki;
    double cpi;
    double rhoi;
    double visci;
    int kontaTempo;
    int itera;
    int penalizaDt;
    varGlob1D *DGP3D;
    int rank;
    int precn;
    SparseMtx<double> matglob;
    Vcr<double> termolivre;
    solverP3D(string nomeArquivoEntrada, varGlob1D *DGP3DV, int nacopV, vector<DadosGeo> &dutoAux,
              double hi, double he, double ti);
    solverP3D();
    solverP3D(const solverP3D &);
    solverP3D &operator=(const solverP3D &);
    void indrazT(int &ind, double &raz);
    int permanentePoisson();
    void inicializaPermanentePoisson();
    void inicializaTransientePoisson();
    double defineDeltPoisson();
    void transientePoisson(double delt);
    void renova();
    void transientePoissonDummy(double delt, int konta);
    void FeiticoDoTempo();
};

#endif /* SOLVER3DPOISSON_H_ */