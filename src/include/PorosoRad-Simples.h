/*
 * TrocaCalor.h
 *
 * Created on: April 24, 2014
 *     Author: bwz1
 */

#ifndef POROSORAD_SIMPLES_H_
#define POROSORAD_SIMPLES_H_

#include "GeometriaPoro.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Vetor.h"
#include "celRad-Simples.h"
#include "estruturaTabDin.h"
#include "estruturasPoroso.h"
#include "variaveisGlobais1D.h"

// Simulation output report stream
extern ofstream arqRelatorioPerfis;

// Application logger object
extern Logger logger;

// Path to the simulation input file
extern string pathArqEntrada;

// Path to the PVTSIM and snapshot input files
extern string pathArqExtEntrada;

// Path and filename prefix for POCO_INJETOR output files
extern string pathPrefixoArqSaida;

// Directory containing the simulation output files
extern string diretorioSaida;

class PorosRadSimp {
  public:
    DadosGeoPoro geom;
    ProFluCol fluc;
    ProFlu flup;
    celradSimp *celula;
    int lenth;
    int permanente;
    int acessorio;
    double erroPermanente;
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

    int tipoModelOleo;
    double compOleo;
    double rhoRef;
    double pBolha;

    tabelaPemRelOA kRelOA;
    tabelaPemRelOG kRelOG;
    tabelaPresCapOA pcOA;
    tabelaPresCapGO pcGO;

    double satConata;
    double ip;

    detTempoPoroso temp;
    materialPoroso *mat; // Array containing the materials defined in the JSON file
    cortePoroso corte;   // Cross-section data defined in the JSON file
    DadosGeoPoro dutosMRT;
    detDiriPoroso pW;

    double satAconat;
    double ippoco;
    int nmaterialPoroso;
    double pInt;
    double pRes2;
    double *pRes;
    double *tempoPRes;
    int nPRes;
    double zDatumRef;

    double condGlob;
    double condLoc;
    double qTotal;
    double pInt0;
    double espessuraRes;

    int flashCompleto;       // Selects the fluid-property model: 1 -> PVTSim table; otherwise, Black Oil
    int usaTabela;           // Indicates whether a property table is used for either the Black Oil or PVTSim model
    string pvtsimarq;        // PVTSim input filename
    int tabp;                // For the Black Oil model, indicates that a compressibility-factor table and its derivatives will be generated
                             // before the production-line simulation starts
    tabelaFlashPoroso flash; // Fluid-property tables loaded from a PVTSim file
    int npontos;             // Number of points in each Black Oil or PVTSim table
    double **zdranP;         // Black Oil compressibility-factor table indexed by reduced pressure and reduced temperature
    double **dzdpP;          // Black Oil table of the Z-factor derivative with respect to pressure
    double **dzdtP;          // Black Oil table of the Z-factor derivative with respect to temperature
    int nvecEmul;            // Number of entries when the user defines the emulsion model through BSW/viscosity-multiplier pairs
    // nvecEmul stores the array size
    double *BSWVec;  // User-provided emulsion values
    double *emulVec; // User-provided viscosity multipliers
    int miniTabAtraso;
    double miniTabDp;
    double miniTabDt;
    detTabViscPoroso tabVisc;
    detTabelaEntalpPoroso tabent;     // Pressure and temperature limits for the PVTSim table
    detTabelaEntalpPoroso tabentCrit; // Reduced-pressure and reduced-temperature limits used to calculate the
    // gas compressibility factor and its derivatives
    composicionalPoroso compDet;
    int corrDeng;

    double *satARes;
    double *satLRes;
    double *satAPoc;
    double *tempoSatARes;
    double *tempoSatLRes;
    double *tempoSatAPoc;

    int nsatARes;
    int nsatLRes;
    int nsatAPoc;

    int reinicia;
    string entrada;

    int kontaTempoImp;

    int posicMarlim;

    int kontaRenovaComp;

    int tipoTabelaDinamica;
    vector<tabelaDinamica> tabDin;
    int ntabDin;

    double BSW;
    varGlob1D *vg1dSP;

    PorosRadSimp();
    PorosRadSimp(string nomeArquivoEntrada, varGlob1D *Vvg1dSP = 0);
    PorosRadSimp(const PorosRadSimp &); // Copy constructor
    PorosRadSimp &operator=(const PorosRadSimp &);
    ~PorosRadSimp() {
        if (tabp > 0) {
            for (int i = 0; i < npontos + 2; i++)
                delete[] zdranP[i];
            delete[] zdranP;
            for (int i = 0; i < npontos + 2; i++)
                delete[] dzdpP[i];
            delete[] dzdpP;
            for (int i = 0; i < npontos + 2; i++)
                delete[] dzdtP[i];
            delete[] dzdtP;
        }

        if (flashCompleto == 1) {
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.rholF[j];
            delete[] flash.rholF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.rhogF[j];
            delete[] flash.rhogF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrholDpF[j];
            delete[] flash.DrholDpF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrhogDpF[j];
            delete[] flash.DrhogDpF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrholDtF[j];
            delete[] flash.DrholDtF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrhogDtF[j];
            delete[] flash.DrhogDtF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrhowDtF[j];
            delete[] flash.DrhowDtF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.RSF[j];
            delete[] flash.RSF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.cplF[j];
            delete[] flash.cplF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.cpgF[j];
            delete[] flash.cpgF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.cpwF[j];
            delete[] flash.cpwF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.HlF[j];
            delete[] flash.HlF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.HgF[j];
            delete[] flash.HgF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.sigOGF[j];
            delete[] flash.sigOGF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.sigWGF[j];
            delete[] flash.sigWGF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.viscO[j];
            delete[] flash.viscO;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.viscG[j];
            delete[] flash.viscG;
            delete[] flash.PBF;
            delete[] flash.TBF;
        }

        if (nvecEmul > 0) {
            delete[] BSWVec;
            delete[] emulVec;
        }
        if (flup.corrOM == 7) {
            if (tabVisc.parserie > 0) {
                delete[] tabVisc.visc;
                delete[] tabVisc.temp;
            }
        }
        if (nsatARes > 0) {
            delete[] satARes;
            delete[] tempoSatARes;
        }
        if (nsatLRes > 0) {
            delete[] satLRes;
            delete[] tempoSatLRes;
        }
        if (nsatAPoc > 0) {
            delete[] satAPoc;
            delete[] tempoSatAPoc;
        }
        if (nPRes > 0) {
            delete[] pRes;
            delete[] tempoPRes;
        }

        if (ncamada)
            delete[] ncamada;
        if (drcamada)
            delete[] drcamada;
        if (lenth > 0) {
            delete[] celula;
        }

    } // Destructor

    void parse_tempo(Value &tempo_json);
    void parse_tabela(Value &tabela_json);
    void parse_corte(Value &corte_json);
    void parse_materiais(Value &materialPoroso_json);
    void parse_kRelOA(Value &kRelOA_json);
    void parse_kRelOG(Value &kRelOG_json);
    void parse_pcOA(Value &pcOA_json);
    void geraTabFlash(int var);
    void parse_fluido_producao(Value &fluidos_producao_json);
    void parse_fluido_complementar(Value &fluido_json);
    void parse_PW(Value &pW_json);
    void iniciaVariaveis();
    void lerDados(string nomeArquivoEntrada);
    void atualizaCel(int i);
    void preparaTabDin();
    void auxMiniTab(ProFlu &flu);
    void geraMiniTabFlu();
    void geraCel(double **const vPcamada = 0, double **const vsL = 0, double **const vsW = 0, const double vsLRes = 0, const double vsWRes = 0);
    double interpolaTabela(int nserie, double valx, double *x, double *y);
    double fkO(double satW, double satG);
    double marchaperm(double mastot);
    double transperm(double mastot = -1);
    void renovaPres(int i, double mTot);
    double SIGN(double a, double b);
    double zriddr(double x1, double x2);
    void indrazT(int &ind, double &raz);
    void indrazPW(int &ind, double &raz);
    void indraz(int &ind, double &raz, double *serieTempo, int nserie);
    void defineDT(int perm = 0);
    void atualizaIni();
    double transtrans(int perm = 0);
    void pseudoTrans(int partida = 1);
    void pseudoTrans2(int partida = 1);
    void dVazdPw();
    void solveTrans();
    void avancoSW(double Ndt);
    void reavaliaDT(double &Ndt);
    void reiniciaEvoluiSW(double Ndt);
    void avancoSWcorrec();
    void avancoPressao();
    void FeiticoDoTempo();
    void FeiticoDoTempoSW();
    void FeiticoDoTempoPQ();
    FullMtx<double> perfil();
    double psia(const double p) const { return (p * 0.9678411) * 14.69595; } // Converts pressure from kgf/cm2 to psia
    double Faren(const double t) const { return 1.8 * t + 32; }              // Converts temperature from degrees Celsius to degrees Fahrenheit
    double cel(const double t) const { return (t - 32) / 1.8; }              // Converts temperature from degrees Fahrenheit to degrees Celsius
    double kgf(const double p) const { return p / (14.69595 * 0.9678411); }  // Converts pressure from psia to kgf/cm2
};

#endif /* POROSORAD_SIMPLES_H_ */