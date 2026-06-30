/*
 * multiBCS.h
 *
 *  Created on: 6 de abr. de 2026
 *      Author: Eduardo
 */

#ifndef MULTIBCS_H_
#define MULTIBCS_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "Bcsm2.h"
#include "PropFlu.h"
#include "PropFluCol.h"

class multiBomCentSub{ // BCS � uma  classe que trabalha com spline,
//a princ�pio ter�amos apenas dois vetores para a interpola��o, mas iremos trabalhar com quatro vetores, onde 3 fun��es
//s�o interpoladas pelo primeiro vetor
        public:
        double freq;//freq��ncia da bomba em Hertz
        double freqnova;
        double eficM;
        double freqMinima;
        int correcHI;
        int equilTerm;

        ProFlu flui;//objeto com as propriedades do fluido do volume;
        ProFluCol fluicol;//objeto com as propriedades do fluido complementar;

    	int nestag;//numero de estagios da bomba
    	int nBCS;
    	int* nestagParcFab;
    	int* nestagParc;
    	BomCentSub* BCSinterno;

    	double dpB;
    	double potB;
    	double potBT;
    	double potTermo;
    	double fracTermMotorEfic;

        multiBomCentSub(int vnBCS=0 ,BomCentSub* BCStemp=0 ,int* nestagParcFabtemp=0 ,
        		int* nestagParctemp=0, double Vfreq=0, double VeficM=100,
					    double vfreqcMinima=0., int vcorrecHI=1,int vequilTerm=1, double vfracTermMotorEfic=0.,
			            ProFlu vflui=ProFlu(),
			            ProFluCol vfluiCol=ProFluCol()); //construtor
        multiBomCentSub(const multiBomCentSub&);//construtor de c�pia
        ~multiBomCentSub(){
        			  if(nBCS>0 && nestagParcFab!=0)delete[] nestagParcFab;
        			  if(nBCS>0 && nestagParc!=0)delete[] nestagParc;
        			  if(nBCS>0 && BCSinterno!=0)delete[] BCSinterno;} //destrutor
        multiBomCentSub& operator=(const multiBomCentSub&);//sobrecarga do operador =
        void marchaMultiBcs(double vazG, double vazL, double pres, double temp, double alfa, double beta);
};



#endif /* MULTIBCS_H_ */
