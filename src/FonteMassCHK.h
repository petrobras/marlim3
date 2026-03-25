/*
 * FonteMassCHK.h
 *
 *  Created on: 13/07/2017
 *      Author: bwz1
 */

#ifndef FONTEMASSCHK_H_
#define FONTEMASSCHK_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
using namespace std;
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Acidentes2.h"


class fontemaschk{ //classe para fonte do tipo injecao de liquido, herda a interface de AbsFonte

        public:
	    ProFlu fluidoP;
	    ProFlu fluidoPamb;
	    ProFluCol fluidocol;
	    choke chk;
	    double masP;
	    double masC;
	    double masG;
        double titT;
        double titamb;
        double cd;
        double abertura;
        double area;
        double bet;
        double betIST;
        double betISamb;
        double tempT;
        double tamb;
        double presT;
        double pamb;
        int check;
        int ambGas;
        fontemaschk(double vmasP,double vmasC,double vmasG,double vtitT,double vtitamb,
        	   double vcd, double vabertura, double varea, double vbet,
			   double vbetIS,double vbetISamb, double vtempT, double vpresT,
			   double vtamb, double vpamb,ProFlu vfluidoP,ProFlu vfluidoPamb, ProFluCol vfluidoC, int vcheck, int vambGas); //construtor
        fontemaschk();
        fontemaschk(const fontemaschk&);//construtor de copia
        fontemaschk& operator=(const fontemaschk&);//sobrecarga do operador =
        void MudaFlu(ProFlu& vfluido){this->fluidoP=vfluido;}//muda o fluido da fonte
        void VMas();//retorna a vazao massica kg/s
};



#endif /* FONTEMASSCHK_H_ */
