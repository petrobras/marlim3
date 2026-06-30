/*
 * solverPoroso.h
 *
 *  Created on: Aug 30, 2023
 *      Author: Eduardo
 */

#ifndef SOLVERPOROSO_H_
#define SOLVERPOROSO_H_

#include "Elem2DPoroso.h"
#include "GeometriaPoro.h"
#include "Log.h"
#include "Malha2DPoroso.h"
#include "Matriz.h"
#include "PorosoRad.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Vetor.h"
#include "celRad.h"
#include "dados1Poroso.h"
#include "estruturaTabDin.h"
#include "estruturasPoroso.h"
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

class solverPoro {
  public:
    dadosPoro dados;
    detMalhaHomPoroso **malhaH;
    malha2dPoro malha;
    int kontaTempo;
    double sWPoc;
    SparseMtx<double> matglob;
    Vcr<double> termolivre;
    int tipoTabelaDinamica;
    vector<tabelaDinamica> tabDin;
    int ntabDin;
    int reinicia;
    double lAcopTot;
    double dt;
    double posicMarlim;
    varGlob1D *vg1dSP;
    int precn;
    solverPoro();
    solverPoro(varGlob1D *Vvg1dSP, string nomeArquivoEntrada);
    solverPoro(const solverPoro &);
    solverPoro &operator=(const solverPoro &);
    ~solverPoro() {
        if (dados.nxMH > 0) {
            for (int i = 0; i < dados.nxMH; i++)
                delete[] malhaH[i];
            delete[] malhaH;
        }
    }
    void indrazT(int &ind, double &raz);
    void indrazPW(int &ind, double &raz);
    void indraz(int &ind, double &raz, double *serieTempo, int nserie);
    void malhaRegular(double &dx, double &dy);
    void imprimeMalhaRegular(double minP);
    void imprimeMalhaRegularSW();
    void imprimeMalhaRegularAlf();
    void imprimeMalhaRegularAux(double minP);
    void imprimeMalhaRegularSWAux();
    void imprimeMalhaRegularAlfAux();
    void preparaTabDinLaco12D();
    void preparaTabDinLaco1Radial();
    void preparaTabDin();
    void auxMiniTab(ProFlu &flu);
    void geraMiniTabFlu();
    void atualizaCel2D(int i);
    void atualizaCelTransfer(int i);
    void inicializaTransientePoroso();
    double defineDeltPoroso();
    void defineDT(int perm = 0);
    void transientePoroso(double &delt);
    void imprimePseudo();
    void pseudoTransientePoroso();
    void dVazdPw();
    void avancoSW(double delt);
    void reavaliaDT(double &delt);
    void reiniciaEvoluiSW(double delt);
    void avancoSWcorrec();
    void avancoPressao();
    void FeiticoDoTempo();
    void FeiticoDoTempoSW();
    void FeiticoDoTempoPQ();
};

#endif /* SOLVERPOROSO_H_ */