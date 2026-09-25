/*
 * caixaValvula.h
 *
 *  Created on: 15 de set. de 2026
 *      Author: Eduardo
 */

#ifndef CAIXAVALVULA_H_
#define CAIXAVALVULA_H_

#include "Geometria.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "variaveisGlobais1D.h"
#include "TrocaCalor.h"

class caixaValv {
  public:
	int posic;
	double lCaixa;
	int formac;
	int lito;
	DadosGeo secaoTrans;
	TransCal transfer;
	int nMon;
	double* serieAberturaMon;
	double* tempMon;
	double abreMon;
	int nJus;
	double* serieAberturaJus;
	double* tempJus;
	double abreJus;
	double alf;
	double bet;
	double tit;
	double pres;
	double temp;
	double tempoResIni;
	double tempoRes;
	double massLiqP;
	double massGas;
	double massLiqC;
	double* fonteMP;
	double* fonteMC;
	double* fonteMG;
	double* tempFonte;
	double temperaturaFonte;
	int nfonte;
	varGlob1D *vg1dSP;
	ProFlu FluidoPro;
	ProFluCol fluidocol;

	caixaValv(int vpos=0,double vlCaixa=0., int vnMon=0,
			  int vnJus=0, int vnfonte=0, double vtempF=0, int vformac=0, int vlito=-1,DadosGeo vsecaoTrans=DadosGeo(),	TransCal vtransfer=TransCal(),varGlob1D *Vvg1dSP = 0);
	caixaValv(const caixaValv& vcxV);
	caixaValv(caixaValv&& vcxV) noexcept;

	caixaValv &operator=(const caixaValv& vcxV);

	caixaValv &operator=(caixaValv &&vcxV) noexcept;

	~caixaValv() {
		if(nMon>0){
			delete[] serieAberturaMon;
			delete[] tempMon;
		}
		if(nJus>0){
			delete[] serieAberturaJus;
			delete[] tempJus;
		}
		if(nfonte>0){
			delete[] fonteMP;
			delete[] fonteMC;
			delete[] fonteMG;
			delete[] tempFonte;
		}
	}

    void propDeri(double p, double t, double& FW, double& bo, double& rs, double& termoRS, double& rhop,double& rhoc);
	void indraz(int &ind, double &raz, double tempo, int parserie , double *serietemp);
    double calcAbertura();
    void atualizaAberturaCaixa();
    void atualizaFontesCaixa(double& Mass, double *serieM);
	void estadotermo(double pmon, double tmon, double alfmon, double betmon, double massmon, double massLmon, ProFlu fluimon, ProFluCol fluiCmon,
			double pjus, double tjus, double alfjus, double betjus, double massjus, double massLjus, ProFlu fluijus, ProFluCol fluiCjus, double dt=0.);
};



#endif /* CAIXAVALVULA_H_ */
