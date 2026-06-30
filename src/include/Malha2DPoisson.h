/*
 * Malha.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef MALHA2DPOISSON_H_
#define MALHA2DPOISSON_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "Elem2DPoisson.h"
#include "variaveisGlobais1D.h"

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;


class malha2d{
public:
	int nele;
	Vcr<double> vecSolv;
	vector<elem2dPoisson> mlh2d;
	malha2d(varGlob1D* Vvg1dSP=0,double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int neleV=0, int nno=0,
			int vperm=1, int vtrans=0);
	malha2d(const malha2d&);
	malha2d& operator=(const malha2d&);

	void imprime(double tempo, int indCel, int indTramo=-1);
};


#endif /* MALHA2DPOISSON_H_ */
