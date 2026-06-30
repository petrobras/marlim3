/*
 * LeituraVapor.h
 *
 * Created on: May 20, 2019
 *     Author: eduardo
 */

#define _USE_MATH_DEFINES
#ifndef LEITURAVAPOR_H_
#define LEITURAVAPOR_H_
#define _USE_MATH_DEFINES // Required for M_PI
#define ARQUIVO_SCHEMA_JSON "schema_1_3_8.json"

#include "Acidentes2.h"
#include "Bcsm2.h"
#include "FonteMasVap.h"
#include "Geometria.h"
#include "Log.h"
#include "Matriz.h"
#include "PropVapor.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "acessorios.h"
#include "celulaVapor.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
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
#include <time.h>
#include <vector>

using namespace std;

#ifdef linux
// Path separator for Linux
#define BARRA "/"
#elif LINUX
// Path separator for Linux
#define BARRA "/"
#elif Linux
// Path separator for Linux
#define BARRA "/"
#elif UNIX
// Path separator for Linux
#define BARRA "/"
#elif defined WIN32
// Path separator for Windows
#define BARRA "\\"
#elif defined Win32
// Path separator for Windows
#define BARRA "\\"
#elif defined win32
// Path separator for Windows
#define BARRA "\\"
#else
// Path separator for other platforms
#define BARRA "/\\"
#endif

using namespace rapidjson;

// Report file for simulation output data
extern ofstream arqRelatorioPerfis;

// Application logger object
extern Logger logger;

// Path containing the simulator input files
extern string pathArqExtEntrada;

// Output-file prefix for injector-well simulations
extern string pathPrefixoArqSaida;

struct detTMAXVap {
    int parserie;
    double *tempo;
    double *valor;
};

struct materialVap {
    int id;
    double cond;
    double cp;
    double rho;
    int tipo;
    double visc;
};

struct cortedutoVap {
    int id;
    int ncam;
    int anul;
    double a;
    double b;
    double rug;
    double *diam;
    int *indmat;
    int *discre;
};

struct detdutoVap {
    double ang;
    int indcorte;
    int servico;
};

struct detalhaPVap {
    int ind;
    int duto;
    int ncel;
    double comp;
    double Lini;
    int dirconv;
    int acopcol;
    int nponts;
    int formacDuto;
    int lito;
    int ambext;
    double *dxVar;
    double **var;
    double *dx;
};

struct detcelpVap {
    double dx;
    int duto;
    double temp;
    double pres;
    double hol;
    double uls;
    double ugs;
    double textern;
    double vextern;
    double kextern;
    double cpextern;
    double rhoextern;
    double viscextern;
    int ambext;
    int dirconv;
    int acopcol;
    int formacCel;
    int lito;
    double profundiM;
    double profundiF;
};

struct detIPRVap {
    int indcel;
    double comp;
    int seriep;
    double *pres;
    double *tpres;
    int seriet;
    double *temp;
    double *ttemp;
    int seriejp;
    double *jp;
    double *tjp;
};

struct detPresEntVap {
    int parserie;
    double *temperatura;
    double *pres;
    double *tit;
    double *tempo;
};

struct detFONMAS {
    int posicP;
    double comp;
    int parserie;
    double *temp;
    double *vazmas;
    double *tempo;
};

struct detBCSVap {
    int posicP;
    double comp;
    int parserie;
    double *tempo;
    double *freq;
    int ncurva;
    double *vaz;
    double *head;
    double *power;
    double *efic;
    double freqref;
    int nestag;
    double eficM;
    double freqMinima;
};

struct detBVOLVap {
    int posicP;
    double comp;
    int parserie;
    double *tempo;
    double *freq;
    double capacidade;
    double npoli;
};

struct detDPREQVap {
    int posicP;
    double comp;
    int parserie;
    double *dp;
    double *tempo;
};

struct detMASTER1Vap {
    int parserie;
    int posic;
    double comp;
    double *abertura;
    double *tempo;
    double razareaativ;
};

struct detPSEPVap {
    int parserie;
    double *pres;
    double *tempo;
};

struct detCHOKESUPVap {
    int parserie;
    double cd;
    int modelo;
    double *abertura;
    double *tempo;
};

struct detPigVap {
    int lanca;
    int receb;
    double razarea;
    double delpres;
    double cdPig;
    double compL;
    double compR;
    double tempo;
};

struct detPROFPVap {
    int n;
    int pres;
    int temp;
    int hol;
    int ugs;
    int uls;
    int ug;
    int ul;
    int arra;
    int viscl;
    int viscg;
    int rhog;
    int rhol;
    int masg;
    int masl;
    int c0;
    int ud;
    int RGO;
    int deng;
    int calor;
    int masstrans;
    int cpg;
    int cpl;
    double *tempo;
};

struct detPROFTRANSVap {
    int n;
    int *posic;
    double *tempo;
};

struct detTRENDPVap {
    double dt;
    int posic;
    int pres;
    int temp;
    int hol;
    int ugs;
    int uls;
    int ug;
    int ul;
    int arra;
    int yco2;
    int viscl;
    int viscg;
    int rhog;
    int rhol;
    int masg;
    int masl;
    int c0;
    int ud;
    int calor;
    int masstrans;
    string rotulo;
};

struct detTelaVap {
    int col;
    int posic;
    int var;
};

struct detTRENDTransVap {
    double dt;
    int posic;
    int camada;
    int discre;
    string rotulo;
};

struct LogEventoVap {
    double instante;
    double duracao;
    string descricao;
    double estIni;
    double estFim;

    bool operator<(const LogEventoVap &e) const {
        if (this->instante < e.instante)
            return true;
        else
            return false;
    }
};

struct detFormacaoVap {
    int id;
    double tempo;
    double cond;
    double cp;
    double rho;
};

class LerVap {
  public:
    int ConContEntrada;
    string impfile;
    int nmaterial;
    int ncorte;
    int nduto;
    int nunidadep;
    int ncelp;
    int nipr;
    int ninjliq;
    int nbcs;
    int nbvol;
    int ndpreq;
    int npig;
    int nperfisp;
    int nperfistransp;
    int ntendp;
    int ntendtransp;
    int ntela;
    int equilterm;
    int transmass;
    int perm;
    int snaps;
    int referencia;

    string snapshotArqIn;
    int ordperm;
    int transiente;
    double tfinal;
    double dtmax;
    double anmP;
    double mono;
    double critcond;
    vector<double> evento;
    vector<LogEventoVap> logevento;
    vector<LogEventoVap> logeventoEstat;

    detTMAXVap dtmaxserie;

    int chkv;
    materialVap *mat;
    cortedutoVap *corte;
    detdutoVap *duto;
    detalhaPVap *unidadeP;
    detcelpVap *celp;
    detIPRVap *IPRS;
    detFONMAS *fontel;
    detBCSVap *bcs;
    detBVOLVap *bvol;
    detDPREQVap *dpreq;
    detMASTER1Vap master1;
    detPSEPVap psep;
    detCHOKESUPVap chokep;
    detCHOKESUPVap *fontechk;
    detPigVap *pig;
    detPROFPVap profp;
    int nvarprofp;
    detPROFTRANSVap proftransp;
    detTRENDPVap *trendp;
    int *nvartrendp;
    detTRENDTransVap *trendtransp;
    detTelaVap *tela;
    int nform;
    detFormacaoVap *formacPoc;
    detPresEntVap CCPres;

    // Total length of the production line
    double nCompTotalUnidadesP;
    double compTotal;
    DadosGeo *dutosMRT;

    int eventoabre;
    double *Tevento;
    int eventofecha;
    double *Teventof;
    int eventoabrePXO;
    double *TeventoPXO;
    double areagargPXO;

    int escorregaPerm;
    int escorregaTran;
    int tabRSPB;

    varGlob1D *vg1dSP;

    /*
     * Default constructor
     */
    LerVap();

    /*
     * Parameterized constructor
     */
    LerVap(const string IMPFILE, varGlob1D *Vvg1dSP);

    /*
     * Copy constructor
     */
    LerVap(const LerVap &);

    /*
     * Assignment operator
     */
    LerVap &operator=(const LerVap &);

    /*
     * Destructor
     */
    ~LerVap() {
        delete[] dtmaxserie.tempo;
        delete[] dtmaxserie.valor;

        if (nform > 0)
            delete[] formacPoc;

        if (mat)
            delete[] mat;

        if (corte) {
            for (int i = 0; i < this->ncorte; i++) {
                delete[] corte[i].diam;
                delete[] corte[i].indmat;
                delete[] corte[i].discre;
            }
            delete[] corte;
        }

        if (duto)
            delete[] duto;

        if (nunidadep > 0) {
            for (int i = 0; i < nunidadep; i++) {
                delete[] unidadeP[i].dx;
                delete[] unidadeP[i].dxVar;
                for (int j = 0; j < 11; j++)
                    delete[] unidadeP[i].var[j];
                delete[] unidadeP[i].var;
            }
            delete[] unidadeP;
        }

        if (ncelp > 0)
            delete[] celp;

        if (nipr > 0) {
            for (int i = 0; i < this->nipr; i++) {
                delete[] IPRS[i].pres;
                delete[] IPRS[i].tpres;
                delete[] IPRS[i].temp;
                delete[] IPRS[i].ttemp;
                delete[] IPRS[i].jp;
                delete[] IPRS[i].tjp;
            }
            delete[] IPRS;
        }

        if (ConContEntrada == 1) {
            delete[] CCPres.temperatura;
            delete[] CCPres.pres;
            delete[] CCPres.tit;
            delete[] CCPres.tempo;
        }

        if (ninjliq > 0) {
            for (int i = 0; i < this->ninjliq; i++) {
                delete[] fontel[i].temp;
                delete[] fontel[i].vazmas;
                delete[] fontel[i].tempo;
            }
            delete[] fontel;
        }

        if (nbcs > 0) {
            for (int i = 0; i < this->nbcs; i++) {
                delete[] bcs[i].tempo;
                delete[] bcs[i].freq;
                delete[] bcs[i].vaz;
                delete[] bcs[i].head;
                delete[] bcs[i].power;
                delete[] bcs[i].efic;
            }
            delete[] bcs;
        }

        if (nbvol > 0) {
            for (int i = 0; i < this->nbvol; i++) {
                delete[] bvol[i].tempo;
                delete[] bvol[i].freq;
            }
            delete[] bvol;
        }

        if (ndpreq > 0) {
            for (int i = 0; i < this->ndpreq; i++) {
                delete[] dpreq[i].dp;
                delete[] dpreq[i].tempo;
            }
            delete[] dpreq;
        }

        if (npig > 0)
            delete[] pig;

        int nevento = master1.parserie;
        delete[] master1.tempo;
        delete[] master1.abertura;

        if (psep.parserie > 0) {
            delete[] psep.tempo;
            delete[] psep.pres;
        }

        if (chokep.parserie) {
            delete[] chokep.tempo;
            delete[] chokep.abertura;
        }

        if (nperfisp > 0)
            delete[] profp.tempo;

        if (nperfistransp > 0) {
            delete[] proftransp.posic;
            delete[] proftransp.tempo;
        }

        if (ntendp > 0) {
            delete[] trendp;
            delete[] nvartrendp;
        }

        if (ntendtransp > 0)
            delete[] trendtransp;

        if (nduto > 0)
            delete[] dutosMRT;

        if (nevento > 0)
            delete[] Tevento;

        if (nevento > 0)
            delete[] Teventof;

        if (ntela > 0)
            delete[] tela;
    }

    void lerArq();
    void geraduto();

    void novatrans(TransCal &transfer, DadosGeo &geom, int *discre, double tint,
                   double text, double vext, double vint, long dircon, double dt,
                   double ki, double cpi, double rhoi, double visci,
                   double ke, double cpe, double rhoe, double visce, int formac,
                   int litologia, int ambext, double pres);

    void geracelp(CelVap *celula);
    void geraipr(CelVap *celula);
    void gerafliqFonte(CelVap *celula);
    void gerafBCS(CelVap *celula);
    void gerafBVOL(CelVap *celula);
    void geraDPReq(CelVap *celula);
    void geraMaster1(CelVap *celula);
    void gerapresfim(double &presfim, double &pGSup);
    void gerachokesup(choke &chokeSup);
    void indraz(int &ind, double &raz, double tempo, int parserie, double *serietemp);

    void atualiza(int inicio, int extrem, choke &chokeSup,
                  CelVap *celula, double &pGSup,
                  double &temperatura,
                  double &presE, double &tempE,
                  double &titE, double tempo);

    void listaevento(int inic = 1, int extrem = 1);
    void geraevento(int inic = 1, int extrem = 1);

    void imprimeProfile(CelVap *const celula, FullMtx<double> &flut,
                        double tempo, int indTramo);

    void imprimeProfileTrans(CelVap *const celula, int *length,
                             double tempo, int indTramo);

    void imprimeTrend(CelVap *const celula, double **flut, double tempo,
                      int trend, int linha);

    void imprimeTrendTrans(CelVap *const celula,
                           double tempo, int trend);

    int buscaIndiceMeioP(double lverif);
    int buscaIndiceFrontP(double lverif);
    int buscaIndiceUnidade(int iniSeg, int nuni, double comp);

  private:
    string arquivoLog;

    Document parseSchema();
    Document parseEntrada();

    void writeSchemaTransiente();
    void writeSchemaInjecao();
    void validateVsSchema(Document *schemaMrtDoc, Document *mrtDoc);

    void iniciarVariaveis();
    void iniciarVariaveisConstrutorDefault();

    bool is_ativo(Value &propriedade_json);
    bool contemChaveAtiva(Document &mrtDoc, string chave);
    bool contem_ativos(Value &propriedade_json);
    bool isArrayPositivoCrescente(Value &propriedade_json);

    bool verificarUnicidade(int *identificadores, int tamanhoVetor,
                            int maiorElemento);

    int identificarMaterial(int identificador);
    int identificarCorte(int identificador);
    int identificarFormacao(int identificador);
    int determinarLinhaErro(int posicaoErro);
    int determinarLinhaErro(const char *chaveJson);

    // Methods for parsing the MRT input file
    void parse_configuracao_inicial(Value &configuracao_inicial_json);
    void parse_tempo(Value &tempo_json);
    void parse_materiais(Value &material_json);
    void parse_corte(Value &corte_json);
    void parse_unidades_producao(Value &duto_producao_json);
    void parse_ipr(Value &ipr_json);
    void parse_separador(Value &separador_json);
    void parse_chokeSup(Value &chokeSup_json);
    void parse_master1(Value &master1_json);
    void parse_fonte_liquido(Value &fonte_liquido_json);
    void parse_fontechk(Value &fontechk_json);
    void parse_pig(Value &pig_json);
    void parse_bcs(Value &bcs_json);
    void parse_bomba_volumetrica(Value &bomba_volumetrica_json);
    void parse_delta_pressao(Value &delta_pressao_json);
    void parse_perfis_producao(Value &perfis_producao_json);
    void parse_perfis_trans_producao(Value &perfis_trans_producao_json);
    void parse_tendencia_producao(Value &tendencia_producao_json);
    void parse_tendencia_trans_producao(Value &tendencia_trans_producao_json);
    void parse_tela(Value &tela_json);
};

#endif /* LEITURAVAPOR_H_ */