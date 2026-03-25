/*
 * Malha.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef MALHA2D_H_
#define MALHA2D_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "Elem2D.h"
#include "variaveisGlobais1D.h"
#include "PropFluColVF.h"

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;
extern int eleYmaxWall;
extern double angY;
extern double mulFC;
extern double gravVF;

class malha2dVF{
public:
	int nele;
	ProFluColVF flucVF;
	varGlob1D* vg1dSP;
	Vcr<double> vecSolv;
	vector<elem2d> mlh2d;
	malha2dVF(double** xcoor=0, int** noEle=0, int* tipo=0,double* atributo=0,int nVert=0, int neleV=0, int nno=0,
			double vdt=0.,int vperm=1, int vtrans=0,varGlob1D* Vvg1dSP=0,ProFluColVF vflucVF=ProFluColVF());
	malha2dVF(const malha2dVF&);
	malha2dVF& operator=(const malha2dVF&);

	void imprimeU(double tempo);
	void imprimeV(double tempo);
	void imprimeP(double tempo);
	void imprimeT(double tempo);
	void imprimePCor(double tempo);
	void imprimeHol(double tempo);
	void imprimeTaxaDeform(double tempo);
	void imprimeVisc(double tempo);
	void imprimeTcont(double tempo,int nPar);
	void imprimeQcont(double tempo, int nPar);
};


#endif /* MALHA2D_H_ */
