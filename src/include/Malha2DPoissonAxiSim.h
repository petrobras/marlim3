/*
 * Malha.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef MALHA2DAxiSim_H_
#define MALHA2DAxiSim_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "Elem2DPoissonAxiSim.h"
#include "variaveisGlobais1D.h"

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;


class malha2dAxiSim{
public:
	int nele;
	int acopDir;
	Vcr<double> vecSolv;
	vector<elem2dPoissonAxiSim> mlh2d;
	malha2dAxiSim(varGlob1D* Vvg1dSP=0,double** xcoor=0, int** noEle=0,
			int* tipo=0,double* atributo=0,
			int nVert=0, int neleV=0, int nno=0,
			int vperm=1, int vtrans=0, int vacopD=0);
	malha2dAxiSim(const malha2dAxiSim&);
	malha2dAxiSim& operator=(const malha2dAxiSim&);

	void imprime(double tempo, int indTramo=-1);
};


#endif /* MALHA2DPOISSON_H_ */
