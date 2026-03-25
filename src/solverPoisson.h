/*
 * solverPoisson.h
 *
 *  Created on: 30 de ago. de 2023
 *      Author: Eduardo
 */

#ifndef SOLVERPOISSON_H_
#define SOLVERPOISSON_H_
#define _USE_MATH_DEFINES
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
#include "Elem2DPoisson.h"
#include "Malha2DPoisson.h"
#include "estruturasPoisson.h"
//#include "Geometria.h"
//#include "TrocaCalor.h"
//#include "PropFluCol.h"
//#include "PropFlu.h"
#include "dados1Poisson.h"
#include "variaveisGlobais1D.h"

//extern double lixo5;

class solverP{
	public:

		dadosP dados;
		Vcr<double> termolivre;
		SparseMtx<double> matglob;
		//detMalhaHomPoisson** malhaH;
		malha2d malha;

		int kontaTempo;
		int indCel;

		varGlob1D* vg1dSP;

		int precn;


		solverP(varGlob1D* Vvg1dSP,string nomeArquivoEntrada, double vCondGlob=0.,double vCondLoc=0., double vhE=0., double vhInt=0.,
				double vTint=0., double vTamb=0.,double vdiamI=0., double vdiamE=0.,int indcel=0);
		solverP();
		solverP(const solverP&);
		solverP& operator=(const solverP&);
		/*~solverP(){
			if(dados.nxMH>0){
				for(int i=0;i<dados.nxMH;i++) delete[] malhaH[i];
				delete[] malhaH;
			}
		}*/
		void indrazT(int& ind, double& raz);
		//void malhaRegular(double& dx, double& dy);
		//void imprimeMalhaRegular(int indTramo=-1);
		void permanentePoisson();
		void inicializaPermanentePoisson();
		void inicializaTransientePoisson();
		double defineDeltPoisson();
		void transientePoissonDummy(double delt);
		void transientePoisson(double delt);
		void imprimePermanente(int indTramo=-1);
		void finalizaPassoTransiente(double vdelt,int indTramo=-1);
		void FeiticoDoTempo();

};



#endif /* SOLVERPOISSON_H_ */
