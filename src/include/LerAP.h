/*
 * LerAS.h
 *
 * Created on: June 23, 2017
 *     Author: Eduardo
 */

#ifndef LERAP_H_
#define LERAP_H_
#define _USE_MATH_DEFINES // Required for M_PI
#define ARQUIVO_SCHEMA_AS_JSON "schema_AS_1_0_0.json"

#include "Acidentes2.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Geometria.h"
#include "Leitura.h"
#include "Log.h"
#include "MarlimComposicional.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "acessorios.h"
#include "celula3.h"
#include "celulaGas.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

using namespace rapidjson;

extern int chaverede;

// Application logger object
extern Logger logger;

struct detIPRAP {
    int parseriePres;
    int parserieTemp;
    int parserieIP;
    int parserieJP;
    int parserieqMax;
    int parserieFlup;
    int indIPR;
    vector<double> pres;
    vector<double> temp;
    vector<double> ip;
    vector<double> jp;
    vector<double> qMax;
    vector<int> indfluP;
};

struct detGASINJAP {
    int parserieTemp;
    int parseriePresInj;
    int parserieVazGas;
    vector<double> temperatura;
    vector<double> presinj;
    vector<double> vazgas;
};

struct detPresEntAP {
    int parserieTemp;
    int parseriePres;
    int parserieTit;
    int parserieBet;

    vector<double> temperatura;
    vector<double> pres;
    vector<double> tit;
    vector<double> bet;
};

struct detVazPresEntAP {
    int parserieTemp;
    int parseriePres;
    int parserieMass;
    int parserieBet;
    vector<double> temperatura;
    vector<double> pres;
    vector<double> mass;
    vector<double> bet;
};

// Gas-source configuration details
struct detFONGASAP {
    int seco;
    int parserieTemp;
    int parserieVazG;
    int parserieVazC;
    int indFG;
    vector<double> temp;
    vector<double> vazgas;
    vector<double> vazcomp;
};

// Valve configuration details
struct detValvAP {
    int indValv;
    int parserieAbre;
    int parserieCD;
    int indV;
    vector<double> abertura;
    vector<double> cd;
};

// Liquid-source configuration details
struct detFONLIQAP {
    int parserieTemp;
    int parserieBet;
    int parserieVL;
    int parserieFlu;
    int indFL;

    vector<double> temp;
    vector<double> bet;
    vector<double> vazliq;
    vector<int> indfluP;
};

// Mass-source configuration details
struct detFONMASSAP {
    int parserieTemp;
    int parserieMP;
    int parserieMC;
    int parserieMG;
    int parserieFlu;
    int indFM;
    vector<double> temp;
    vector<double> vazMasP;
    vector<double> vazMasC;
    vector<double> vazMasG;
    vector<int> indfluP;
};

// Leak-source configuration for a branching connection
struct detFUROAP {
    int parseriePres;
    int parserieTemp;
    int parserieBeta;
    int parserieCD;
    int parserieAbre;
    int parserieFlu;
    int indFuro;
    vector<double> pres;
    vector<double> temp;
    vector<double> beta;
    vector<double> cd;
    vector<double> abertura;
    vector<int> indFlu;
};

struct detBCSAP {
    int indBCS;
    int parserieFreq;
    int parserieEstag;
    vector<double> freq;
    vector<int> nestag;
};

// Positive-displacement pump configuration details
struct detBVOLAP {
    int parserieFreq;
    int parserieCap;
    int parserieNPoli;
    int indBV;
    vector<double> freq;
    vector<double> capacidade;
    vector<double> npoli;
};

// Localized pressure-increase configuration details
struct detDPREQAP {
    int parserieDP;
    int indDP;
    vector<double> dp;
};

// Downstream line-pressure configuration, corresponding to separator pressure
struct detPSEPAP {
    int parseriePres;
    vector<double> pres;
};

// Gas-oil ratio configuration for fluid zero only
struct detRGOAP {
    int parserieRGO;
    vector<double> RGO;
};

// Basic sediment and water configuration for fluid zero only
struct detBSWAP {
    int parserieBSW;
    vector<double> BSW;
};

// Hydrostatic pressure-loss correction details
struct detDPHidro {
    int indDPHidro;
    int parserieDPHidro;
    vector<double> dPHidro;
};

// Frictional pressure-loss correction details
struct detDPFric {
    int indDPFric;
    int parserieDPFric;
    vector<double> dPFric;
};

// Temperature-correction details
struct detDT {
    int indDT;
    int parserieDT;
    vector<double> dT;
};

// Surface-choke configuration details
struct detCHOKESUPAP {
    int parserieCD;
    int parserieAbre;
    vector<double> cd;
    vector<double> abertura;
};

struct detCondConInjecAP {
    int parserieTemp;
    int parserieVaz;
    int parseriePresI;
    int parseriePresFundo;
    vector<double> tempinj;
    vector<double> vazinj;
    vector<double> presinj;
    vector<double> presfundo;
};

struct detDiamRug {
    int parserieA;
    int parserieB;
    int parserieRug;
    int indGeom;
    vector<double> dA;
    vector<double> dB;
    vector<double> rug;
};

struct detCondEquiv {
    int parserieK;
    int indMat;
    vector<double> cond;
};

struct variaveis {
    int vpsep;
    int vRGO;
    int vBSW;
    int vfonliq;
    int vfongas;
    int vipr;
    int vfonmas;
    int vbcs;
    int vdp;
    int vbvol;
    int vvalv;
    int vfuro;
    int vgasinj;
    int vpresent;
    int vvazpresent;
    int vchk;
    int vpocinj;
    int diam;
    int kequiv;
    int vdpH;
    int vdpF;
    int vdt;
};

struct casoVEC {
    vector<int> IPRpres;
    vector<int> IPRtemp;
    vector<int> IPRip;
    vector<int> IPRjp;
    vector<int> IPRqMax;
    vector<int> IPRindfluP;
    vector<int> FGtemp;
    vector<int> FGvazgas;
    vector<int> FGvazcomp;
    vector<int> FLtemp;
    vector<int> FLbet;
    vector<int> FLvazliq;
    vector<int> FLindfluP;
    vector<int> FMtemp;
    vector<int> FMvazMasP;
    vector<int> FMvazMasC;
    vector<int> FMvazMasG;
    vector<int> FMindfluP;
    vector<int> BCSfreq;
    vector<int> BCSnestag;
    vector<int> BVOLfreq;
    vector<int> BVOLcapacidade;
    vector<int> BVOLnpoli;
    vector<int> DPdp;
    vector<int> DPdLH;
    vector<int> DPdLF;
    vector<int> DTdL;
    vector<int> VALVabertura;
    vector<int> VALVcd;
    vector<int> FUROpres;
    vector<int> FUROtemp;
    vector<int> FURObeta;
    vector<int> FUROcd;
    vector<int> FUROabertura;
    vector<int> FUROindFlu;
    vector<int> GeomdA;
    vector<int> GeomdB;
    vector<int> GeomRug;
    vector<int> condEqui;
    int INJGtemperatura;
    int INJGpresinj;
    int INJGvazgas;
    int PSEPpres;
    int RGO;
    int BSW;
    int PEtemperatura;
    int PEpres;
    int PEtit;
    int PEbet;
    int VPEtemperatura;
    int VPEpres;
    int VPEmass;
    int VPEbet;
    int CHKcd;
    int CHKabertura;
    int PItempinj;
    int PIvazinj;
    int PIpresinj;
    int PIpresfundo;
};

struct genericoVEC {
    vector<int> generico;
};

class APara {
  public:
    int tipoAP;
    string entrada;
    int dim;
    int nVariaveis;
    variaveis listaV;
    int nAPIPR;
    detIPRAP *APIPR;
    int nAPFG;
    detFONGASAP *APFonGas;
    int nAPV;
    detValvAP *APValv;
    int nAPFL;
    detFONLIQAP *APFonLiq;
    int nAPFM;
    detFONMASSAP *APFonMas;
    int nAPFuro;
    detFUROAP *APFuro;
    int nAPBCS;
    detBCSAP *APBCS;
    int nAPBV;
    detBVOLAP *APBVOL;
    int nAPDP;
    detDPREQAP *APDP;
    int nAPdPdLH;
    detDPHidro *APdPdLH;
    int nAPdPdLF;
    detDPFric *APdPdLF;
    int nAPdTdL;
    detDT *APdTdL;
    int nAPGeom;
    detDiamRug *APGeom;
    int nAPCondEquiv;
    detCondEquiv *APCondEquiv;

    detGASINJAP APGasInj;
    detPresEntAP APpEntrada;
    detVazPresEntAP APvpEntrada;
    detPSEPAP APPsep;
    detRGOAP APRGO;
    detBSWAP APBSW;
    detCHOKESUPAP APCHK;
    detCondConInjecAP APPInj;
    casoVEC *sequenciaAP;
    genericoVEC *genericoAP;

    double resultadosVec[21 + 1];
    double **saidaBHP;
    double **saidaVazLiq;
    int nthrdAP;
    int *vecParSerie;
    int varSeq[22];
    int vfp;
    varGlob1D *vg1dSP;
    int ncel;
    detcelp *celp;
    ProFlu *flup;
    detBCS *bcs;
    detFONGAS *fonteg;

    APara(varGlob1D *Vvg1dSP, const string IMPFILE, int vncel, detcelp *vcelp, ProFlu *vflup = 0, detBCS *vbcs = 0, detFONGAS *vfonteg = 0);
    APara(const APara &); // Copy constructor
    APara &operator=(const APara &);
    ~APara() { // Destructor

        if (listaV.vbcs == 1) {
            delete[] APBCS;
        }

        if (listaV.vipr == 1) {
            delete[] APIPR;
        }

        if (listaV.vfonliq == 1) {
            delete[] APFonLiq;
        }

        if (listaV.vfongas == 1) {
            delete[] APFonGas;
        }

        if (listaV.vfonmas == 1) {
            delete[] APFonMas;
        }

        if (listaV.vvalv == 1) {
            delete[] APValv;
        }

        if (listaV.vbvol == 1) {
            delete[] APBVOL;
        }

        if (listaV.vdp == 1) {
            delete[] APDP;
        }

        if (listaV.vdpH == 1) {
            delete[] APdPdLH;
        }

        if (listaV.vdpF == 1) {
            delete[] APdPdLF;
        }

        if (listaV.vdt == 1) {
            delete[] APdTdL;
        }

        if (listaV.vfuro == 1) {
            delete[] APFuro;
        }

        if (listaV.diam == 1) {
            delete[] APGeom;
        }

        if (listaV.kequiv == 1) {
            delete[] APCondEquiv;
        }

        if (nVariaveis > 1) {
            delete[] sequenciaAP;
            delete[] genericoAP;
            delete[] vecParSerie;
        }

        if (nVariaveis > 1 && dim > 0 && tipoAP == 1) {
            for (int i = 0; i < nVariaveis; i++) {
                delete[] saidaBHP[i];
                delete[] saidaVazLiq[i];
            }
            delete[] saidaBHP;
            delete[] saidaVazLiq;
        }
    }

    Document parseEntrada();
    void parse_variaveis(Value &variaveis_json);
    void parse_IPR(Value &IPR_json);
    void parse_FonLiq(Value &FonLiq_json);
    void parse_FonGas(Value &FonGas_json);
    void parse_FonMas(Value &FonMas_json);
    void parse_BCS(Value &BCS_json);
    void parse_BVol(Value &BVol_json);
    void parse_DP(Value &DP_json);
    void parse_dPdLH(Value &dPdLHidro_json);
    void parse_dPdLF(Value &dPdLFric_json);
    void parse_dTdL(Value &dTdL_json);
    void parse_Valv(Value &Valv_json);
    void parse_Furo(Value &Furo_json);
    void parse_Diam(Value &Diam_json);
    void parse_CondEquiv(Value &CondEquiv_json);
    void parse_Psep(Value &Psep_json);
    void parse_RGO(Value &RGO_json);
    void parse_BSW(Value &BSW_json);
    void parse_GasInj(Value &GasInj_json);
    void parse_CHK(Value &CHK_json);
    void parse_Pent(Value &Pent_json);
    void parse_VPent(Value &VPent_json);
    void parse_injecPoco(Value &injecP_json);
    void lerArq();
    void constroiVecParSerie();
    void constroiVecParSerieImex();
    void traduzSeq();
    void traduzSeqImex();
    int inicializaSequen(int iSeq = 0, int tipo = 0);
    void atualizaGeom(int ncelG, Cel *celula, CelG *celulaG, int iduto, int tipovar, double val);
    void atualizaMat(int ncelG, Cel *celula, CelG *celulaG, int imat, double val);
    void atualizaCompRGO(double rgo, ProFlu &flui);
    void atualizaRGO(Cel *celula, double val);
    void atualizaBSW(Cel *celula, double val);
    void atualizaCorrecao(int ncelG, Cel *celula, CelG *celulaG, double *vdPdLH,
                          double *vdPdLF, double *vdTdL);
    void cabecalhoAP(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                     detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                     detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq);
    void cabecalhoAPImex(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                         detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                         detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq);
    void imprimeVarInteresseAP(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                               detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                               detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq, int seq);
    void selecaoAP(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                   detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                   detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                   double &pGSup, double &temperatura,
                   double &presiniG, double &tempiniG, double &vazgasG, double &presE, double &tempE,
                   double &titE, double &betaE, double &vazE, int seq, int &indCHK, double *vdPdLH,
                   double *vdPdLF, double *vdTdL, int imprime = 1);
    void selecaoAPsemImpre(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                           detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                           detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                           double &pGSup, double &temperatura,
                           double &presiniG, double &tempiniG, double &vazgasG, double &presE, double &tempE,
                           double &titE, double &betaE, double &vazE, int seq, int &indCHK, double *vdPdLH,
                           double *vdPdLF, double *vdTdL);
    void imprimeVarInteresseAPImex(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                                   detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                                   detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq, int seq);
    void selecaoAPImex(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                       detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                       detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                       double &pGSup, double &temperatura,
                       double &presiniG, double &tempiniG, double &vazgasG, double &presE, double &tempE,
                       double &titE, double &betaE, double &vazE, int seq, int &indCHK, double *vdPdLH,
                       double *vdPdLF, double *vdTdL, int imprime = 1);
    void selecaoAPImexsemImpre(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                               detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                               detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                               double &pGSup, double &temperatura,
                               double &presiniG, double &tempiniG, double &vazgasG, double &presE, double &tempE,
                               double &titE, double &betaE, double &vazE, int seq, int &indCHK, double *vdPdLH,
                               double *vdPdLF, double *vdTdL);
    void tabelaGenericaCabecalho();
    void tabelaGenerica(int ncelG, choke &chokeSup, Cel *celula, CelG *celulaG, ProFlu *flup,
                        detIPR *IPRS, detValv *valv, detFONGAS *fonteg, detFONLIQ *fontel,
                        detFONMASS *fontem, detFURO *furo, detBCS *bcs, detBVOL *bvol, detDPREQ *dpreq,
                        double &pGSup, double &temperatura,
                        double &presiniG, double &tempiniG, double &vazgasG, double &presE, double &tempE,
                        double &titE, double &betaE, double &vazE, int seq, int &indCHK, double *vdPdLH,
                        double *vdPdLF, double *vdTdL, double BHP);
};

#endif /* LERAP_H_ */
