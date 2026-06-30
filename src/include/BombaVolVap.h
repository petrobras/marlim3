/*
 * BombaVolVap.h
 *
 *  Created on: 24 de mai de 2019
 *      Author: eduardo
 */

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#ifndef BOMBAVOLVAP_H_
#define BOMBAVOLVAP_H_
#include <algorithm>
#include <fstream>
#include <stdlib.h>
using namespace std;
#include "PropVapor.h"

class BomVolVapor{ // BCS � uma  classe que trabalha com spline,
//a princ�pio ter�amos apenas dois vetores para a interpola��o, mas iremos trabalhar com quatro vetores, onde 3 fun��es
//s�o interpoladas pelo primeiro vetor
    public:

	double capacidade;
	double freq;
	double MGas;
	double MLiqP;
	double MultGas;
	double MultLiqP;
	double npoli;


    BomVolVapor(double vcap=0., double vfreq=0.,double vnpoli=1.2,
    		  double VMG=0., double VMP=0.); //construtor
    BomVolVapor(const BomVolVapor&);//construtor de c�pia
    BomVolVapor& operator=(const BomVolVapor&);//sobrecarga do operador =
    void vazmass(double pres,double pres1, double temp,double temp1,
    		     double alf);

};




#endif /* BOMBAVOLVAP_H_ */
