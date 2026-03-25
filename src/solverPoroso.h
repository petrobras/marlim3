/*
 * solverPorooroso.h
 *
 *  Created on: 30 de ago. de 2023
 *      Author: Eduardo
 */

#ifndef SOLVERPOROSO_H_
#define SOLVERPOROSO_H_

#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;
//#include <ctime.h>
//#include <ctime.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "Log.h"
#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "Elem2DPoroso.h"
#include "Malha2DPoroso.h"
#include "estruturasPoroso.h"
#include "estruturaTabDin.h"
#include "GeometriaPoro.h"
#include "celRad.h"
#include "PorosoRad.h"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "dados1Poroso.h"
#include "variaveisGlobais1D.h"

//extern double tempo;
//extern double lixo5;

class solverPoro{
	public:

		dadosPoro dados;
		detMalhaHomPoroso** malhaH;
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

	    varGlob1D* vg1dSP;

	    int precn;

	    solverPoro();
	    solverPoro(varGlob1D* Vvg1dSP,string nomeArquivoEntrada);
		solverPoro(const solverPoro&);
		solverPoro& operator=(const solverPoro&);
		~solverPoro(){
			if(dados.nxMH>0){
				for(int i=0;i<dados.nxMH;i++) delete[] malhaH[i];
				delete[] malhaH;
			}
		}
		void indrazT(int& ind, double& raz);
		void indrazPW(int& ind, double& raz);
		void indraz(int& ind, double& raz, double* serieTempo, int nserie);
		void malhaRegular(double& dx, double& dy);
		void imprimeMalhaRegular(double minP);
		void imprimeMalhaRegularSW();
		void imprimeMalhaRegularAlf();

		void imprimeMalhaRegularAux(double minP);
		void imprimeMalhaRegularSWAux();
		void imprimeMalhaRegularAlfAux();

		void preparaTabDinLaco12D();
		void preparaTabDinLaco1Radial();
		void preparaTabDin();
		void auxMiniTab(ProFlu& flu);
		void geraMiniTabFlu();
		void atualizaCel2D(int i);
		void atualizaCelTransfer(int i);

		void inicializaTransientePoroso();
		double defineDeltPoroso();
		void defineDT(int perm=0);
		void transientePoroso(double& delt);
		void imprimePseudo();
		void pseudoTransientePoroso();
		void dVazdPw();
		void avancoSW(double delt);
		void reavaliaDT(double& delt);
		void reiniciaEvoluiSW(double delt);
		void avancoSWcorrec();
		void avancoPressao();
		void FeiticoDoTempo();
		void FeiticoDoTempoSW();
		void FeiticoDoTempoPQ();
};



#endif /* SOLVERPOROSO_H_ */
