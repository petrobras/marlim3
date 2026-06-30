/*
 * SisProdVap.h
 *
 *  Created on: May 22, 2019
 *      Author: eduardo
 */

#ifndef SISPRODVAP_H_
#define SISPRODVAP_H_
#define _USE_MATH_DEFINES // Enables access to the M_PI constant

#include "Acidentes2.h"
#include "Bcsm2.h"
#include "FerramentasNumericas.h"
#include "FonteMasVap.h"
#include "Geometria.h"
#include "LeituraVapor.h"
#include "Log.h"
#include "Matriz.h"
#include "PropVapor.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "acessorios.h"
#include "celulaVapor.h"
#include "chokegas.h"
#include "estrat.h"
#include "mapa.h"
#include "variaveisGlobais1D.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;
extern double lixo5;
extern double lixo5R;
extern int contador;
extern double localtiny;
extern double CritCond;
extern int contaExit;
extern int chaverede;
extern int iterRede;
extern int iterRedeT;

// Retrieves the output file path prefix used for POCO_INJETOR
extern string pathPrefixoArqSaida;

// Output stream used to generate simulation data file reports
extern ofstream arqRelatorioPerfis;

// Logger instance used by the application
extern Logger logger;

class SProdVap {
  public:
    int indTramo;
    int ncel;
    int reinicia;
    double presE;
    double tempE;
    double titE;
    double alfE;
    double presfim;
    double titRev;
    double pGSup;
    double tGSup;
    double temperatura;
    double masSup;
    double tempSup;
    double mult;
    double presMedMov;
    double jMedMov;
    double alfMedMov;
    double tMedMov;
    int aberto;
    int tempoaberto;
    int EstadoMaster1;
    int contaMaster1;
    int masChkSup;
    int TransMassModel;
    int indpigP;
    int npig;
    int *receb;
    LerVap arq;
    FullMtx<double> flutG;
    FullMtx<double> flut;
    BandMtx<double> matglobG;
    Vcr<double> termolivreG;
    BandMtx<double> matglobP;
    Vcr<double> termolivreP;
    double dt;
    double dtini;
    double tfinal;
    int nabreM1;
    int nfechaM1;
    double *fechaM1;
    double *abreM1;
    int *ncelperftransp;
    int *TrendLengthP;
    double ***MatTrendP;
    double *resettrend;
    int *ntrend;
    int *TrendLengthTransP;
    double ***MatTrendTransP;
    double *resettrendtrans;
    int *ntrendtrans;
    int kontaTempoProf;
    int kontaTempoTransProf;
    ostringstream saidaLog;
    string tmpLog;
    int contaLog;
    double menorDx;
    int iterperm;
    int kSP;
    int KontaImprime;
    int indevento;
    int modoPerm;
    double ktMedMov;
    double pTotal;
    double jTotal;
    double alfTotal;
    vector<double> presVet;
    vector<double> jVet;
    vector<double> alfVet;
    vector<double> tVet;
    choke chokeSup;

    double tempMedContDesc;
    double maxVecContDesc;
    double velmedDesc;
    double tempmedDEsc;
    vector<double> velmaxMedDesc;
    vector<double> dtDesc;

    int noextremo;
    int noinicial;
    double fontemassPRBuf;
    double fontemassCRBuf;
    double fontemassGRBuf;
    CelVap *celula;
    varGlob1D *vg1dSP;

    SProdVap(string nomeArquivoEntrada, string nomeArquivoLog, varGlob1D *Vvg1dSP);
    SProdVap();
    ~SProdVap() { // Destructor

        if (nabreM1 > 0)
            delete[] abreM1;
        if (nfechaM1 > 0)
            delete[] fechaM1;
        if (arq.nperfistransp > 0)
            delete[] ncelperftransp;

        if (arq.ntendp > 0) {
            for (int i = 0; i < arq.ntendp; i++) {
                for (int j = 0; j < TrendLengthP[i]; j++)
                    if (MatTrendP[i][j])
                        delete[] MatTrendP[i][j];
                if (MatTrendP[i])
                    delete[] MatTrendP[i];
            }
            if (MatTrendP)
                delete[] MatTrendP;
            if (TrendLengthP)
                delete[] TrendLengthP;
            if (resettrend)
                delete[] resettrend;
            if (ntrend)
                delete[] ntrend;
        }

        if (arq.ntendtransp > 0) {
            for (int i = 0; i < arq.ntendtransp; i++) {
                for (int j = 0; j < TrendLengthTransP[i]; j++)
                    if (MatTrendTransP[i][j])
                        delete[] MatTrendTransP[i][j];
                if (MatTrendTransP[i])
                    delete[] MatTrendTransP[i];
            }
            if (MatTrendTransP)
                delete[] MatTrendTransP;
            if (TrendLengthTransP)
                delete[] TrendLengthTransP;
            if (resettrendtrans)
                delete[] resettrendtrans;
            if (ntrendtrans)
                delete[] ntrendtrans;
        }

        if (ncel > 0)
            delete[] celula;
        if (npig > 0)
            delete[] receb;
    }

    SProdVap &operator=(const SProdVap &);
    void montasistema();
    void calctemp(int i, double tempantiga);
    double calcHmix(int i, double tempantiga);
    int sign(double var);
    double calcTempEntalp(int i, double tempantiga);
    void calcTransMassTermo(int i);
    void FonteValv(int ind);
    void renovaFonte(int ind);
    void renovaalbetini();
    void renovaMasEsp();
    void CalcC0Ud(int ind, double &c0, double &ud);
    void CalcC0UdBuf(int ind, double &c0, double &ud);
    void CalcC0UdIni(int ind, double &c0, double &ud);
    void CalcC0UdIniBuf(int ind, double &c0, double &ud);
    void renova();
    void renovaBuffer();
    void renovaBufferCego();
    void renovaTemp();
    void renovaterm(int aflu = 0);
    void renovatermAfluFim();
    void renovatermColIni();
    void calcCCpres(double titRev = 1., double alfRev = 1.);
    void calcCCBuffer(double titRev = 1., double alfRev = 1.);
    void calcCCpres();
    void determinaDT();
    void EvoluiFrac(double alfrev);
    void ReiniEvolFrac0();
    void ReiniEvolFrac();
    void AtualizaPig();
    void SolveAcopPV();
    void SolveTrans(double titRev = 1., double alfRev = 1.);
    void ImprimeTrend();
    double marchaProdPerm1(double pchute);
    double marchaProdPerm2(double pchute);
    double buscaProdPfundoPerm(double chute = -1.);
    double buscaProdPfundoPerm2(double chute = -1.);
    double marchaProdPresPres1(double mchute);
    double buscaProdPresPresPerm(double mchute);
    double marchaProdPresPres2(double mchute);
    double buscaProdPresPresPerm2(double mchute);
    void RenovaPresPermMon(int i, int RK);
    void RenovaPresPermJus(int i, int RK);
    void RenovaMassPerm(int i);
    void CalcC0UdPerm(int ind, double &c0, double &ud);
    double calcHmixPerm(int i);
    double calcTempEntalpPerm(int i);
    void RenovaTransMassPerm(int i);
    void atualizaPeriPmonProd(int i);
    void atualizaPeriPjusProd(int i);
    void atualizaPeriTempProd(int i);
    double multMarcha(double chute, int prod, int tipoCC);
    double SIGN(double a, double b);
    double zbrent(double, double, int prod, int tipoCC, double tol = 0.00001, double epsn = 0.00001, int maxit = 100);
    double falsacorda(double a, double b, int prod, int tipoCC);
    double zriddr(double x1, double x2, int prod, int tipoCC);
    double hidroreverso(double hol, double vaz = 0);
    void calcTempFim();
};

#endif /* SISPRODVAP_H_ */