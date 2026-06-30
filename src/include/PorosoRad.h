/*
 * TrocaCalor.h
 *
 * Created on: April 24, 2014
 *     Author: bwz1
 */

#ifndef POROSORAD_H_
#define POROSORAD_H_

#include "GeometriaPoro.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Vetor.h"
#include "celRad.h"
#include "estruturaTabDin.h"
#include "estruturasPoroso.h"
#include "variaveisGlobais1D.h"

extern detTempoPoroso temp;

extern detDiriPoroso pW;

class PorosRad {
  public:
    DadosGeoPoro geom;
    ProFluCol fluc;
    ProFlu flup;
    celrad *celula;
    int lenth;
    int permanente;
    int *ncamada;
    int nglobal;
    int ncel;
    double *drcamada;
    double Pint;
    double presRes;
    double presRes2;
    double tRes;
    double dt;
    double fluxIni;
    double fluxFim;
    double fluxIniG;
    double fluxFimG;
    double fluxIniA;
    double fluxFimA;
    double DfluxIni;
    double DfluxIniG;
    double DfluxIniA;
    double zD0;
    double zD1;
    double resGlob;
    double resFim;
    double he;
    double hi;

    double dtL;
    double dh;

    int condiTparede;
    double sLRes;
    double sWRes;
    double sWPoc;

    tabelaPemRelOA kRelOA;
    tabelaPemRelOG kRelOG;
    tabelaPresCapOA pcOA;
    tabelaPresCapGO pcGO;

    double satConata;
    double ip;

    int tipoModelOleo;
    double compOleo;
    double rhoRef;
    double pBolha;
    double BSW;

    varGlob1D *vg1dSP;

    PorosRad();

    PorosRad(const DadosGeoPoro vgeom, varGlob1D *Vvg1dSP, const int vpermanente, const int *const vncamada,
             const double *const vdrcamada, double **const vPcamada, const double vPint,
             const double vpresRes, const double vtRes, const double vdt, const double pres, const double vzD0, const double vzD1, const double vdh,
             double **const vsL, double **const vsW, const double vsLRes, const double vsWRes,
             const double vsWPoc,
             const double vsatConata, const double vip,
             tabelaPemRelOA vkRelOA, tabelaPemRelOG vkRelOG,
             tabelaPresCapOA vpcOA, tabelaPresCapGO vpcGO,
             ProFluCol vfluc, ProFlu vflup);

    PorosRad(const PorosRad &); // Copy constructor
    PorosRad &operator=(const PorosRad &);
    ~PorosRad() {
        if (ncamada)
            delete[] ncamada;
        if (drcamada)
            delete[] drcamada;
        if (lenth > 0) {
            delete[] celula;
        }

    } // Destructor

    void iniciaVariaveis();
    void geraCel(double **const vPcamada = 0, double **const vsL = 0, double **const vsW = 0, const double vsLRes = 0, const double vsWRes = 0);
    double interpolaTabela(int nserie, double valx, double *x, double *y);
    double fkO(double satW, double satG);
    double marchaperm(double mastot);
    double transperm(double mastot = -1);
    void renovaPres(int i, double mTot);
    double SIGN(double a, double b);
    double zriddr(double x1, double x2);
    void atualizaIni();
    double transtrans(double espessura);
    void transtransAux(double espessura);
    void FeiticoDoTempo();
    FullMtx<double> perfil();
    double psia(const double p) const { return (p * 0.9678411) * 14.69595; } // Converts pressure from kgf/cm2 to psia
    double Faren(const double t) const { return 1.8 * t + 32; }              // Converts temperature from degrees Celsius to degrees Fahrenheit
    double cel(const double t) const { return (t - 32) / 1.8; }              // Converts temperature from degrees Fahrenheit to degrees Celsius
    double kgf(const double p) const { return p / (14.69595 * 0.9678411); }  // Converts pressure from psia to kgf/cm2
};

#endif /* POROSORAD_H_ */