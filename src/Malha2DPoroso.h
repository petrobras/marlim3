/*
 * Malha.h
 *
 *  Created on: 22 de jun. de 2022
 *      Author: Eduardo
 */

#ifndef MALHA2DPOROSO_H_
#define MALHA2DPOROSO_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>
#include <algorithm>
#include <fstream>
#include <complex>
#include <vector>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "celRad.h"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "estruturasPoroso.h"
#include "estruturaTabDin.h"
#include "dados1Poroso.h"
#include "Elem2DPoroso.h"
#include "variaveisGlobais1D.h"

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;


class malha2dPoro{
public:
	int nele;
	varGlob1D* vg1dSP;
	Vcr<double> vecSolv;
	vector<elem2dPoroso> mlh2d;
	malha2dPoro();
	malha2dPoro(varGlob1D* Vvg1dSP,double** xcoor, int** noEle, int* tipo,double* atributo,int nVert, int neleV, int nno,
			int vperm, int vtrans,ProFlu vflup,tabelaPemRelOA vkRelOA,tabelaPemRelOG vkRelOG,
			tabelaPresCapOA vpcOA,tabelaPresCapGO vpcGO);
	malha2dPoro(const malha2dPoro&);
	malha2dPoro& operator=(const malha2dPoro&);

	void imprime(double tempo);
};


#endif /* MALHA2DPOROSO_H_ */
