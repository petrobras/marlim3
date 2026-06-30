/*
 * mapa.h
 *
 *  Created on: 19 de abr de 2016
 *      Author: bwz1
 */

#ifndef MAPA_H_
#define MAPA_H_

#include <iostream>
#include "variaveisGlobais1D.h"
extern double localtiny;

class arranjo{
public:
   double dia;
   double fg;
   double fl;
   double reyL;
   double reyG;
   double reyM;
   double uls;
   double ugs;
   double ang;
   double hol;
   double rl;
   double rg;
   double mil;
   double mig;
   double dina;
   double dpg;
   double dpl;
   double hidro;
   int arr;
   int map;
   varGlob1D* vg1dSP;


   arranjo(double vdia=1*2.54/100., double vuls=0., double vugs=0.,
                 double vrl=10000., double vrg=1.,
                 double vmil=1./1000.,double vmig=1./10000,
                 double vhol=0.5,double vang=0., double vdina=0., int vmap=0,varGlob1D* Vvg1dSP=0); //construtor
   arranjo(const arranjo&); //construtor por c�pia
   ~arranjo(){} //destrutor

   arranjo& operator=(const arranjo&);

   double fricl();
   double fricg();
   double fricm();
   double SIGN(double a,double b){return (b>=0?1.0:-1.0)*fabs(a);}
   double zbrent(double x1,double x2,
                      double tol=0.001,
                      double epsn=0.001,int maxit=100);
   double funcarranjo(double alfl);

   int verificaArr();
};



#endif /* MAPA_H_ */
