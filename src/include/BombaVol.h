/*
 * BombaVol.h
 *
 *  Created on: 19 de set de 2016
 *      Author: BWZ1
 */

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#ifndef BOMBAVOL_H_
#define BOMBAVOL_H_
#include <algorithm>
#include <fstream>
#include <stdlib.h>
using namespace std;
#include "PropFlu.h"
#include "PropFluCol.h"
#include "PropVapor.h"

class BomVol{ // BCS � uma  classe que trabalha com spline,
//a princ�pio ter�amos apenas dois vetores para a interpola��o, mas iremos trabalhar com quatro vetores, onde 3 fun��es
//s�o interpoladas pelo primeiro vetor
    public:

	double capacidade;
	double freq;
	double MGas;
	double MLiqP;
	double MLiqC;
	double MultGas;
	double MultLiqP;
	double MultLiqC;
	double npoli;
	ProFlu fluido;
	ProFluCol fluicol;

    BomVol(double vcap=0., double vfreq=0.,double vnpoli=1.2, double VMG=0., double VMP=0.,
    		double VMC=0.,
    		const ProFlu vflui=ProFlu(),
    		const ProFluCol vfluiC=ProFluCol()); //construtor
    BomVol(const BomVol&);//construtor de c�pia
    BomVol& operator=(const BomVol&);//sobrecarga do operador =
    void vazmass(double pres,double pres1, double temp,double temp1,
    		double bet, double alf,int tipo=0);

};




#endif /* BOMBAVOL_H_ */
