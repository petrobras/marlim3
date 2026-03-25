/*
 * Malha.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef MALHA3DPOISSON_H_
#define MALHA3DPOISSON_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "Elem3DPoisson.h"
#include "estruturaUNV.h"
#include "variaveisGlobais1D.h"

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;


class malha3d{
public:
	int nele;
	Vcr<double> vecSolv;
	vector<elem3dPoisson> mlh3d;
	varGlob1D* DGP3D;
	malha3d(varGlob1D* DGP3DV=0,eleOBJ* velementoUNV=0, double** xcoor=0, int** noEle=0, int* tipo=0,
			double* atributo=0,int nVert=0, int neleV=0, int nno=0);
	malha3d(const malha3d&);
	malha3d& operator=(const malha3d&);

	void imprime(double tempo);
};


#endif /* MALHA3DPOISSON_H_ */
