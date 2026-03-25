/*
 * celulaVapor.h
 *
 *  Created on: 21 de mai de 2019
 *      Author: eduardo
 */

#ifndef CELULAVAPOR_H_
#define CELULAVAPOR_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>

using namespace std;
#include "acessorios.h"
#include "Geometria.h"
#include "Vetor.h"
#include "Matriz.h"
#include "PropVapor.h"
#include "TrocaCalor.h"
#include "variaveisGlobais1D.h"

/*extern double localtiny;
extern double CritCond;*/

class CelVap{


      	public:
	    DadosGeo dutoL;
        DadosGeo duto;
        DadosGeo dutoR;
        acessorio acsr;
        acessorio* acsrL;
        ProVap flui;

        int posic;
        double dt;
        double dt2;
        double tempR;
        double temp;
        double tempL;
        double tempini;
        double presL;
        double presLini;
        double pres;
        double presini;
        double presauxL;
        double presaux;
        double presauxR;
        double presR;
        double entalp;
        double uinter;
        double ML;
        double MC;
        double MR;
        double MRini;
        double MliqiniR;
        double Mliqini;
        double MliqiniL;
        double MliqiniR0;// alteracao7
        ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
        double presBuf;
        double presLiniBuf;
        double presLBuf;
        double presRBuf;
        double presauxBuf;
        double MCBuf;
        double MRBuf;
        double MliqiniRBuf;
        double MRiniBuf;
        double MLBuf;
        double MliqiniBuf;
        double MliqiniLBuf;
        //////////////////////////// 05-07-2018 ///////////////////////////////////////////////
        double alfL;
        double alfLini;
        double alfR;
        double alfRini;
        double alf;
        double alfini;
        double dxL;
        double dx;
        double dxR;
        double massfonteCH;
        double term1L;
        double term2L;
        double term1;
        double term2;
        double term1R;
        double term2R;
        double c0;
        double ud;
        double c0Spare;
        double udSpare;
        double transmassL;
        double transmassR;
        double FonteMudaFase;
        double DTransDt1;
        double DTransDt0;
        double DTransDxR;
        double DTransDxL;
        double CoefDTR;
        double CoefDTL;
        double fontedissolv;
        int TMModel;
        int TMModelL;
        double fontemassLL;
        double fontemassGL;
        double fontemassLR;
        double fontemassGR;
        double dpB;
        double potB;
        int transic;
        int arranjo;
        int arranjoR;
        double chutearranjo;
        double QLL;
        double QL;
        double QLR;
        double QLRini;// alteracao7
        double QG;
        double rpL;
        double rpR;
        double rpC;
        double rpLi;
        double rpCi;
        double rpRi;
        Vcr<double> TL;
        FullMtx<double> local;
        double perdaEstratL;
        double perdaEstratG;
        double cinematico;
        double VTemper;
        TransCal calor;

        int estadoPig;
        double dtPig;
        double alfPigE;
        double alfPigER;
        double alfPigEini;
        double alfPigD;
        double alfPigDini;

        double velPig;
        double velPigini;
        double razPig;
        double razPigini;
        double RazAreaPig;//alteracao2
        double DelPig;//alteracao2
        double VazaPig;//alteracao2
        double cdpig;//alteracao2
        int indpig;//alteracao2

        double DmasschokeG;
        double DmasschokeL;

        double corrigeMassaPres;

        varGlob1D* vg1dSP;


        CelVap(varGlob1D* Vvg1dSP=0,const DadosGeo vdutoL=DadosGeo(),
        	const DadosGeo vduto=DadosGeo(),
        	const DadosGeo vdutoR=DadosGeo(),
            //const ProFlu vflui=ProFlu(),
			const ProVap vflui=ProVap(),
            const double vtempL=25.,const double vtemp=25.,
            const double vtempR=25.,const double vtempini=25.,
            const double vpresL=10.,const double vpres=10.,const double vpresR=10.,
            const double vML=0.,const double vMC=0.,const double vMR=0.,
            const double vMliqiniL=0.,const double vMliqini=0.,const double vMliqiniR=0.,
            const double valfL=1.,const double valf=1.,const double valfR=1.,
            const double vdxL=1.,const double vdx=1.,const double vdxR=1.,
            const double vdt=-1.,
            const int vposic=-1,
            const TransCal vcalor=TransCal(),
            const acessorio vacsr=acessorio(),
            acessorio* vacsrL=0);//construtor default
        CelVap(const CelVap&); //construtor por copia
  	    CelVap& operator=(const CelVap&);

  	    double Rey(double dia,double vel,double rho,double vis);
  	    double fric(double re,double eps);
  	    void avancalf(int& reinicia, int ncel);
  	    void GeraLocal(double presfim, int masChkSup, int ncel, double razareativa,
                double presE, double tempE, double titE=-1);

  	    void avancPig(int& reinicia);
  	    void avancalfPig();




};





#endif /* CELULAVAPOR_H_ */
