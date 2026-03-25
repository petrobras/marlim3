/*
 * celRad.h
 *
 *  Created on: 14 de out. de 2023
 *      Author: Eduardo
 */

#ifndef CELRAD_SIMPLES_H_
#define CELRAD_SIMPLES_H_
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <fstream>
using namespace std;
#include <stdlib.h>
#include "estruturasPoroso.h"
#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "variaveisGlobais1D.h"

//extern double tempo;

class celradSimp{
public:
	ProFluCol fluc;
	ProFlu flup;
	double drcamada;
	double PcamadaR;
	double Pcamada;
	double PcamadaL;
	double QcamadaR;
	double QcamadaL;

	double QwcamadaR;
	double QwcamadaL;
	double QocamadaR;
	double QocamadaL;
	double QgcamadaR;
	double QgcamadaL;
	double Qwini;
	double Qoini;
	double Qgini;

	double volCel;
	double cflCel;

	double Piter;
	double Pint;
	double presRes;
	double presRes2;
	double Pini;
	double QiniR;
	double QiniL;
	double dt;
	double zD1;
	double zD;
	double zD0;
	double r1;
	double r0;
	double rm;
	double tRes;
	double kXL;
	double kXR;
	double kYL;
	double kYR;
	double poro;
	double compresPoro;
	int icel;
	Vcr<double> localvet;
	FullMtx<double> localmat;
    //celradSimp* celL;
    //celradSimp* celR;

    double dtSL;
    double dtSW;
	double dh;

	int condiTparede;
	int ncel;

	double sLR;
	double sL;
	double sLL;
	double sLini;
	double sLiter;
	double sWR;
	double sW;
	double sWL;
	double sWini;
	double sWiter;
	double sLRes;
	double sWRes;
	double sWPoc;



	double pmed1;
	double slmed1;
	double samed1;
	double pmed0;
	double slmed0;
	double samed0;

	double vbo;
	double vboIni;
	double rhostd;
	double rhogstd;
	double rhoa;

	double rhoP1;
	double rhoP;
	double rhoPIni;
	double rhoP0;

	double rhogP1;
	double rhogP;
	double rhogPIni;
	double rhogP0;

	double rhog1;
	double rhog0;

	double rho1;
	double rho0;

	double rhoa1;
	double rhoa0;

	double rhoPa1;
	double rhoPa;
	double rhoPaIni;
	double rhoPa0;

	double mio1;
	double mio0;
	double mig1;
	double mig0;
	double mia1;
	double mia0;

	double kmed1;
	double kmed0;
	double kmedG1;
	double kmedG0;
	double kmedA1;
	double kmedA0;
	double pcOG1;
	double pcOGm;
	double pcOG0;
	double pcAO1;
	double pcAOm;
	double pcAO0;

	double kabsol1;
	double kabsol0;

	double darcyO1;
	double darcyO0;
	double darcyG1;
	double darcyG0;
	double darcyA1;
	double darcyA0;

	double rQcamadaL;
	double rQcamadaR;
	double drQ;
	double drP1;
	double drP0;

	double dPdT;

	tabelaPemRelOA kRelOACel;
	tabelaPemRelOG kRelOGCel;
	tabelaPresCapOA pcOACel;
	tabelaPresCapGO pcGOCel;

	double satConata;
	double ip;

	int reiniciaSL;
	int reiniciaSW;

	double alf;
	double alf1;
	double alf0;
	double alfmed1;
	double alfmed0;

	double BSW;

	int tipoModelOleo;
	double compOleo;
	double rhoRef;
	double pBolha;

	varGlob1D* vg1dSP;

	celradSimp(varGlob1D* Vvg1dSP=0,int vncel=0,int vicel=0,double vr0=0., double vr1=0., double vrm=0.,double vKXL=0.,double vKXR=0.,
			double vKYL=0.,double vKYR=0.,double vporo=0.,double vcompresPoro=0.,double vPcamada=0,double vPint=0.,
			double vpresRes2=0.,double vpresRes=0.,double vQL=0., double vQR=0, double vtRes=0, double vdt=0,
			double vzD0=0, double vzD=0,
			double vzD1=0,double vdh=1,double vsL=0,double vsW=0,double vsLRes=0,
			double vsWPoc=0,double vsWRes=0,
			double vsatConata=0,const double vip=1.,tabelaPemRelOA vkRelOA=tabelaPemRelOA(),tabelaPemRelOG vkRelOG=tabelaPemRelOG(),
			tabelaPresCapOA vpcOA=tabelaPresCapOA(),tabelaPresCapGO vpcGO=tabelaPresCapGO(),
			ProFluCol vfluc=ProFluCol(), ProFlu vflup=ProFlu()/*, celradSimp* vcelL=0, celradSimp* vcelR=0*/);
	//celradSimp();

	celradSimp(const celradSimp&); //construtor por c�pia
	celradSimp& operator=(const celradSimp&);
	double interpolaTabela(int nserie, double valx, double* x, double* y);
	double fkO(double satW, double satG);
	double dFW(double satW, double satG);
	void calcVazOWG();
	double cflO();
	double cflA();
	void evoluiSL(int& reinicia,int ciclo,double dpdt=0.);
	void evoluiSW(int& reinicia,int ciclo,double dpdt=0);
	void transcel(int idisc);
	void FeiticoDoTempo();
	void FeiticoDoTempoPQ();
};



#endif /* CELRAD_SIMPLES_H_ */
