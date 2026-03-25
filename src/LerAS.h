/*
 * LerRede.h
 *
 *  Created on: 23 de jun de 2017
 *      Author: Eduardo
 */


#ifndef LERAS_H_
#define LERAS_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
#include <stdio.h>
#include <string.h>
#include <vector>
#include "Acidentes2.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Geometria.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "acessorios.h"
#include "celula3.h"
#include "celulaGas.h"
#include "Leitura.h"
#include "MarlimComposicional.h"
#include "variaveisGlobais1D.h"
/*#include "Acidentes2.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Geometria.h"
#include "FonteMas.h"
#include "Vetor.h"
#include "Matriz.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "acessorios.h"
#include "celula3.h"
#include "TrocaCalor.h"
#include "chokegas.h"
#include "celulaGash.h"*/
//#include "SisProd.h"
#include "Log.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/pt_BR.h"
using namespace rapidjson;

#define ARQUIVO_SCHEMA_AS_JSON "schema_AS_1_0_0.json"

extern int chaverede;

// criar objeto para log da aplicacao
extern Logger logger;


struct detIPRAS{
	int parseriePres;
	int parserieTemp;
	int parserieIP;
	int parserieJP;
	int parserieqMax;
	int parserieFlup;
	int indIPR;
	//double* pres;
	//double* temp;
	//double* ip;
	//double* jp;
	//double* qMax;
	//int* indfluP;
	vector< double >  pres;
	vector< double >  temp;
	vector< double >  ip;
	vector< double > jp;
	vector< double > qMax;
	vector< int > indfluP;
};


struct detGASINJAS{
	int parserieTemp;
	int parseriePresInj;
	int parserieVazGas;
	//double* temperatura;
	//double* presinj;
	//double* vazgas;
	vector< double > temperatura;
	vector< double > presinj;
	vector< double > vazgas;
};

struct detPresEntAS{
	int parserieTemp;
	int parseriePres;
	int parserieTit;
	int parserieBet;
	//double* temperatura;
	//double* pres;
	//double* tit;
	//double* bet;

	vector< double > temperatura;
	vector< double > pres;
	vector< double > tit;
	vector< double > bet;

};

struct detVazPresEntAS{
	int parserieTemp;
	int parseriePres;
	int parserieMass;
	int parserieBet;
	//double* temperatura;
	//double* pres;
	//double* mass;
	//double* bet;
	vector< double > temperatura;
	vector< double > pres;
	vector< double > mass;
	vector< double > bet;
};

//struct com o detalhamento de fonte de gas
struct detFONGASAS{
	int seco;
	int parserieTemp;
	int parserieVazG;
	int parserieVazC;
	int indFG;
	//double* temp;
	//double* vazgas;
	//double* vazcomp;
	//int* indfluP;
	vector< double > temp;
	vector< double > vazgas;
	vector< double > vazcomp;
};

//struct com o detalhamento de valvula
struct detValvAS{
	int indValv;
	int parserieAbre;
	int parserieCD;
	int indV;
	//double* abertura;
	//int *cd;
	vector< double > abertura;
	vector< double > cd;
};

//struct com o detalhamento de fonte de liquido
struct detFONLIQAS{
	int parserieTemp;
	int parserieBet;
	int parserieVL;
	int parserieFlu;
	int indFL;
	//double* temp;
	//double* bet;
	//double* vazliq;
	//int* indfluP;

	vector< double > temp;
	vector< double > bet;
	vector< double > vazliq;
	vector< int > indfluP;
};

//struct com o detalhamento de fonte de massa
struct detFONMASSAS{
	int parserieTemp;
	int parserieMP;
	int parserieMC;
	int parserieMG;
	int parserieFlu;
	int indFM;
	//double* temp;
	//double* vazMasT;
	//double* vazMasC;
	//double* vazMasG;
	//int* indfluP;
	vector< double > temp;
	vector< double > vazMasP;
	vector< double > vazMasC;
	vector< double > vazMasG;
	vector< int > indfluP;
};

//struct com o detalhamento de fonte que ocorre devido a uma derivacao do tipo vazamento
struct detFUROAS{
	int parseriePres;
	int parserieTemp;
	int parserieBeta;
	int parserieCD;
	int parserieAbre;
	int parserieFlu;
	int indFuro;
	//double* pres;
	//double* temp;
	//double* beta;
	//double* cd;
	//double* abertura;
	//int* indFlu;
	vector< double > pres;
	vector< double > temp;
	vector< double > beta;
	vector< double > cd;
	vector< double > abertura;
	vector< int > indFlu;
};

struct detBCSAS{
	int indBCS;
	int parserieFreq;
	int parserieEstag;
	//double* freq;
	//int* nestag;
	vector< double > freq;
	vector< int > nestag;
};

//struct com o detalhamento da Bomba Volumetrica
struct detBVOLAS{
	int parserieFreq;
	int parserieCap;
	int parserieNPoli;
	int indBV;
	//double* freq;
    //double* capacidade;
    //double* npoli;
	vector< double > freq;
	vector< double > capacidade;
	vector< double > npoli;
};

//struct com o detalhamento de incremento de pressao localizado
struct detDPREQAS{
	int parserieDP;
	int indDP;
	//double* dp;
	vector< double > dp;
};

//struct com o detalhamento da pressao a jusante da linha, pressao do separador
struct detPSEPAS{
	int parseriePres;
	//double* pres;
	vector< double > pres;
};

//struct com o detalhamento da RGO somente para o fluido zero
struct detRGOAS{
	int parserieRGO;
	//double* pres;
	vector< double > RGO;
};

//struct com o detalhamento da RGO somente para o fluido zero
struct detBSWAS{
	int parserieBSW;
	//double* pres;
	vector< double > BSW;
};

//struct com o detalhamento da correcao prda de carga hidrostatica
struct detDPHidro{
	int indDPHidro;
	int parserieDPHidro;
	//double* pres;
	vector< double > dPHidro;
};

//struct com o detalhamento da correcao prda de carga friccao
struct detDPFric{
	int indDPFric;
	int parserieDPFric;
	//double* pres;
	vector< double > dPFric;
};

//struct com o detalhamento da correcao temperatura
struct detDT{
	int indDT;
	int parserieDT;
	//double* pres;
	vector< double > dT;
};

//struct com o detalhamento do choke de superficie
struct detCHOKESUPAS{
	int parserieCD;
	int parserieAbre;
	//double* cd;
	//double* abertura;
	vector< double > cd;
	vector< double > abertura;
};

struct detCondConInjecAS{
    int parserieTemp;
    int parserieVaz;
    int parseriePresI;
    int parseriePresFundo;
	//double* tempinj;
	//double* vazinj;
	//double* presinj;
	//double* presfundo;
    vector< double > tempinj;
    vector< double > vazinj;
    vector< double > presinj;
    vector< double > presfundo;
};

struct detDiamRug{
	int parserieA;
	int parserieB;
	int parserieRug;
	int indGeom;
	vector< double >  dA;
	vector< double >  dB;
	vector< double >  rug;
};

struct detCondEquiv{
	int parserieK;
	int indMat;
	vector< double >  cond;
};

struct variaveis{
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

struct casoVEC{
	vector< int >  IPRpres;
	vector< int >  IPRtemp;
	vector< int >  IPRip;
	vector< int > IPRjp;
	vector< int > IPRqMax;
	vector< int > IPRindfluP;
	///////////////////////////////////
	vector< int > FGtemp;
	vector< int > FGvazgas;
	vector< int > FGvazcomp;
	///////////////////////////////////
	vector< int > FLtemp;
	vector< int > FLbet;
	vector< int > FLvazliq;
	vector< int > FLindfluP;
	///////////////////////////////////
	vector< int > FMtemp;
	vector< int > FMvazMasP;
	vector< int > FMvazMasC;
	vector< int > FMvazMasG;
	vector< int > FMindfluP;
	//////////////////////////////////
	vector< int > BCSfreq;
	vector< int > BCSnestag;
	/////////////////////////////////
	vector< int > BVOLfreq;
	vector< int > BVOLcapacidade;
	vector< int > BVOLnpoli;
	/////////////////////////////////
	vector< int > DPdp;
	/////////////////////////////////
	vector< int > DPdLH;
	/////////////////////////////////
	vector< int > DPdLF;
	/////////////////////////////////
	vector< int > DTdL;
	/////////////////////////////////
	vector< int > VALVabertura;
	vector< int > VALVcd;
	/////////////////////////////////
	vector< int > FUROpres;
	vector< int > FUROtemp;
	vector< int > FURObeta;
	vector< int > FUROcd;
	vector< int > FUROabertura;
	vector< int > FUROindFlu;
	////////////////////////////////
	vector< int >  GeomdA;
	vector< int >  GeomdB;
	vector< int >  GeomRug;
	////////////////////////////////
	vector< int >  condEqui;
	////////////////////////////////
	int INJGtemperatura;
	int INJGpresinj;
	int INJGvazgas;
	////////////////////////////////
	int PSEPpres;
	////////////////////////////////
	int RGO;
	////////////////////////////////
	int BSW;
	////////////////////////////////
	int PEtemperatura;
	int PEpres;
	int PEtit;
	int PEbet;
	////////////////////////////////
	int VPEtemperatura;
    int VPEpres;
	int VPEmass;
	int VPEbet;
	////////////////////////////////
	int CHKcd;
	int CHKabertura;
	///////////////////////////////
    int PItempinj;
    int PIvazinj;
    int PIpresinj;
    int PIpresfundo;
};

struct genericoVEC{
	vector< int >  generico;
};

class ASens{
    public:
	int tipoAS;
	string entrada;
	int dim;
	int nVariaveis;
	variaveis listaV;
	int nASIPR;
	detIPRAS* ASIPR;
	int nASFG;
	detFONGASAS* ASFonGas;
	int nASV;
	detValvAS* ASValv;
	int nASFL;
	detFONLIQAS* ASFonLiq;
	int nASFM;
	detFONMASSAS* ASFonMas;
	int nASFuro;
	detFUROAS* ASFuro;
	int nASBCS;
	detBCSAS* ASBCS;
	int nASBV;
	detBVOLAS* ASBVOL;
	int nASDP;
	detDPREQAS* ASDP;
	int nASdPdLH;
	detDPHidro* ASdPdLH;
	int nASdPdLF;
	detDPFric* ASdPdLF;
	int nASdTdL;
	detDT* ASdTdL;
	int nASGeom;
	detDiamRug* ASGeom;
	int nASCondEquiv;
	detCondEquiv* ASCondEquiv;



	detGASINJAS ASGasInj;
	detPresEntAS ASpEntrada;
	detVazPresEntAS ASvpEntrada;
	detPSEPAS ASPsep;
	detRGOAS ASRGO;
	detBSWAS ASBSW;
	detCHOKESUPAS ASCHK;
	detCondConInjecAS ASPInj;

	casoVEC* sequenciaAS;
	genericoVEC* genericoAS;

	double resultadosVec[21+1];

	double** saidaBHP;
	double** saidaVazLiq;
	int nthrdAS;



	int* vecParSerie;


	int varSeq[22];

	int vfp;

	varGlob1D* vg1dSP;

	int ncel;
	detcelp* celp;

	//int indFlup;
	ProFlu* flup; //alteracao chris VFPs novo
	detBCS* bcs; //alteracao chris VFPs novo
	detFONGAS* fonteg; //alteracao chris VFPs novo


    ASens(varGlob1D* Vvg1dSP,const string IMPFILE,int vncel, detcelp* vcelp, ProFlu* vflup=0, detBCS* vbcs=0, detFONGAS* vfonteg=0);
    ASens(const ASens&); //construtor por copia
	ASens& operator=(const ASens&);
	~ASens(){//destrutor

		/*if(listaV.vgasinj==1){
			  if(ASGasInj.parseriePresInj>0)delete [] ASGasInj.presinj;
			  if(ASGasInj.parserieVazGas>0)delete [] ASGasInj.vazgas;
			  if(ASGasInj.parserieTemp>0)delete [] ASGasInj.temperatura;
		}

		if(listaV.vpresent==1){
			  if(ASpEntrada.parseriePres>0)delete [] ASpEntrada.pres;
			  if(ASpEntrada.parserieTit>0)delete [] ASpEntrada.tit;
			  if(ASpEntrada.parserieTemp>0)delete [] ASpEntrada.temperatura;
			  if(ASpEntrada.parserieBet>0)delete [] ASpEntrada.bet;
		}

		if(listaV.vvazpresent==1){
			  if(ASvpEntrada.parseriePres>0)delete [] ASvpEntrada.pres;
			  if(ASvpEntrada.parserieMass>0)delete [] ASvpEntrada.mass;
			  if(ASvpEntrada.parserieTemp>0)delete [] ASvpEntrada.temperatura;
			  if(ASvpEntrada.parserieBet>0)delete [] ASvpEntrada.bet;
		}

		if(listaV.vpsep==1){
			  if(ASPsep.parseriePres>0)delete [] ASPsep.pres;
		}

		if(listaV.vchk==1){
			  if(ASCHK.parserieAbre>0)delete [] ASCHK.abertura;
			  if(ASCHK.parserieCD>0)delete [] ASCHK.cd;
		}

		if(listaV.vpocinj==1){
			  if(ASPInj.parseriePresFundo>0)delete [] ASPInj.presfundo;
			  if(ASPInj.parseriePresI>0)delete [] ASPInj.presinj;
			  if(ASPInj.parserieTemp>0)delete [] ASPInj.tempinj;
			  if(ASPInj.parserieVaz>0)delete [] ASPInj.vazinj;
		}*/


		if(listaV.vbcs==1){
		    /*for(int i=0; i<this->nASBCS;i++){
			  if(ASBCS[i].parserieEstag>0)delete [] ASBCS[i].nestag;
			  if(ASBCS[i].parserieFreq>0)delete [] ASBCS[i].freq;
		    }*/
			delete[] ASBCS;
		}

		if(listaV.vipr ==1){
		    /*for(int i=0; i<this->nASIPR;i++){
			  if(ASIPR[i].parserieFlup>0)delete [] ASIPR[i].indfluP;
			  if(ASIPR[i].parserieIP>0)delete [] ASIPR[i].ip;
			  if(ASIPR[i].parserieJP>0)delete [] ASIPR[i].jp;
			  if(ASIPR[i].parseriePres>0)delete [] ASIPR[i].pres;
			  if(ASIPR[i].parserieTemp>0)delete [] ASIPR[i].temp;
			  if(ASIPR[i].parserieqMax>0)delete [] ASIPR[i].qMax;
		    }*/
			delete[] ASIPR;
		}

		if(listaV.vfonliq ==1){
		    /*for(int i=0; i<this->nASFL;i++){
			  if(ASFonLiq[i].parserieFlu>0)delete [] ASFonLiq[i].indfluP;
			  if(ASFonLiq[i].parserieBet>0)delete [] ASFonLiq[i].bet;
			  if(ASFonLiq[i].parserieTemp>0)delete [] ASFonLiq[i].temp;
			  if(ASFonLiq[i].parserieVL>0)delete [] ASFonLiq[i].vazliq;
		    }*/
			delete[] ASFonLiq;
		}

		if(listaV.vfongas ==1){
		    /*for(int i=0; i<this->nASFG;i++){
			  if(ASFonGas[i].parserieFluP>0)delete [] ASFonGas[i].indfluP;
			  if(ASFonGas[i].parserieTemp>0)delete [] ASFonGas[i].temp;
			  if(ASFonGas[i].parserieVazG>0)delete [] ASFonGas[i].vazgas;
			  if(ASFonGas[i].parserieVazC>0)delete [] ASFonGas[i].vazcomp;
		    }*/
			delete [] ASFonGas;
		}

		if(listaV.vfonmas ==1){
		    /*for(int i=0; i<this->nASFG;i++){
			  if(ASFonMas[i].parserieFlu>0)delete [] ASFonMas[i].indfluP;
			  if(ASFonMas[i].parserieMC>0)delete [] ASFonMas[i].vazMasC;
			  if(ASFonMas[i].parserieMG>0)delete [] ASFonMas[i].vazMasG;
			  if(ASFonMas[i].parserieMT>0)delete [] ASFonMas[i].vazMasT;
			  if(ASFonMas[i].parserieTemp>0)delete [] ASFonMas[i].temp;
		    }*/
			delete [] ASFonMas;
		}

		if(listaV.vvalv ==1){
		    /*for(int i=0; i<this->nASV;i++){
			  if(ASValv[i].parserieAbre>0)delete [] ASValv[i].abertura;
			  if(ASValv[i].parserieCD>0)delete [] ASValv[i].cd;
		    }*/
			delete [] ASValv;
		}

		if(listaV.vbvol ==1){
		    /*for(int i=0; i<this->nASBV;i++){
			  if(ASBVOL[i].parserieFreq>0)delete [] ASBVOL[i].freq;
			  if(ASBVOL[i].parserieCap>0)delete [] ASBVOL[i].capacidade;
			  if(ASBVOL[i].parserieNPoli>0)delete [] ASBVOL[i].npoli;
		    }*/
			delete [] ASBVOL;
		}

		if(listaV.vdp ==1){
		   /* for(int i=0; i<this->nASDP;i++){
			  if(ASDP[i].parserieDP>0)delete [] ASDP[i].dp;
		    }*/
			delete [] ASDP;
		}

		if(listaV.vdpH ==1){
			delete [] ASdPdLH;
		}

		if(listaV.vdpF ==1){
			delete [] ASdPdLF;
		}

		if(listaV.vdt ==1){
			delete [] ASdTdL;
		}

		if(listaV.vfuro ==1){
		    /*for(int i=0; i<this->nASFuro;i++){
			  if(ASFuro[i].parserieAbre>0)delete [] ASFuro[i].abertura;
			  if(ASFuro[i].parserieBeta>0)delete [] ASFuro[i].beta;
			  if(ASFuro[i].parserieCD>0)delete [] ASFuro[i].cd;
			  if(ASFuro[i].parserieFlu>0)delete [] ASFuro[i].indFlu;
			  if(ASFuro[i].parseriePres>0)delete [] ASFuro[i].pres;
			  if(ASFuro[i].parserieTemp>0)delete [] ASFuro[i].temp;
		    }*/
			delete [] ASFuro;
		}

		if(listaV.diam ==1){
			delete[] ASGeom;
		}

		if(listaV.kequiv ==1){
			delete[] ASCondEquiv;
		}

		if(nVariaveis>1){
			delete [] sequenciaAS;
			delete [] genericoAS;
			delete [] vecParSerie;
		}

		if(nVariaveis>1 && dim>0 && tipoAS==1){
			for(int i=0;i<nVariaveis;i++){
				delete [] saidaBHP[i];
				delete [] saidaVazLiq[i];
			}
			delete [] saidaBHP;
			delete [] saidaVazLiq;
		}
	}

	Document parseEntrada();
	void parse_variaveis(Value& variaveis_json);
	void parse_IPR(Value& IPR_json);
	void parse_FonLiq(Value& FonLiq_json);
	void parse_FonGas(Value& FonGas_json);
	void parse_FonMas(Value& FonMas_json);
	void parse_BCS(Value& BCS_json);
	void parse_BVol(Value& BVol_json);
	void parse_DP(Value& DP_json);
	void parse_dPdLH(Value& dPdLHidro_json);
	void parse_dPdLF(Value& dPdLFric_json);
	void parse_dTdL(Value& dTdL_json);
	void parse_Valv(Value& Valv_json);
	void parse_Furo(Value& Furo_json);
	void parse_Diam(Value& Diam_json);
	void parse_CondEquiv(Value& CondEquiv_json);
	void parse_Psep(Value& Psep_json);
	void parse_RGO(Value& RGO_json);
	void parse_BSW(Value& BSW_json);
	void parse_GasInj(Value& GasInj_json);
	void parse_CHK(Value& CHK_json);
	void parse_Pent(Value& Pent_json);
	void parse_VPent(Value& VPent_json);
	void parse_injecPoco(Value& injecP_json);
	void lerArq();

    void constroiVecParSerie();
    void constroiVecParSerieImex();
    void traduzSeq();
    void traduzSeqImex();
	int inicializaSequen(int iSeq=0,int tipo=0);
	void atualizaGeom(int ncelG,  Cel* celula, CelG* celulaG, int iduto,int tipovar, double val);
	void atualizaMat(int ncelG,  Cel* celula, CelG* celulaG, int imat, double val);
	void atualizaCompRGO(double rgo, ProFlu& flui);
	void atualizaRGO(Cel* celula, double val);
	void atualizaBSW(Cel* celula, double val);
	void atualizaCorrecao(int ncelG,  Cel* celula, CelG* celulaG, double* vdPdLH,
			double* vdPdLF, double* vdTdL);
	void cabecalhoAS(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq);
	void cabecalhoASImex(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq);
	void imprimeVarInteresseAS(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq, int seq);
	void selecaoAS(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
			double& pGSup,  double& temperatura,
				double& presiniG, double& tempiniG,double& vazgasG, double& presE, double& tempE,
				double& titE, double& betaE, double& vazE, int seq, int& indCHK, double* vdPdLH,
				double* vdPdLF, double* vdTdL,int imprime=1);
	void selecaoASsemImpre(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
			double& pGSup,  double& temperatura,
				double& presiniG, double& tempiniG,double& vazgasG, double& presE, double& tempE,
				double& titE, double& betaE, double& vazE, int seq, int& indCHK, double* vdPdLH,
				double* vdPdLF, double* vdTdL);
	void imprimeVarInteresseASImex(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq, int seq);
	void selecaoASImex(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
			double& pGSup,  double& temperatura,
				double& presiniG, double& tempiniG,double& vazgasG, double& presE, double& tempE,
				double& titE, double& betaE, double& vazE, int seq, int& indCHK, double* vdPdLH,
				double* vdPdLF, double* vdTdL,int imprime=1);
	void selecaoASImexsemImpre(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
			double& pGSup,  double& temperatura,
				double& presiniG, double& tempiniG,double& vazgasG, double& presE, double& tempE,
				double& titE, double& betaE, double& vazE, int seq, int& indCHK, double* vdPdLH,
				double* vdPdLF, double* vdTdL);
	void tabelaGenericaCabecalho();
	void tabelaGenerica(int ncelG,choke& chokeSup, Cel* celula, CelG* celulaG, ProFlu* flup,
		    detIPR* IPRS,detValv* valv,detFONGAS* fonteg,detFONLIQ* fontel,
			detFONMASS* fontem,detFURO* furo,detBCS* bcs,detBVOL* bvol,detDPREQ* dpreq,
			double& pGSup,  double& temperatura,
				double& presiniG, double& tempiniG,double& vazgasG, double& presE, double& tempE,
				double& titE, double& betaE, double& vazE, int seq, int& indCHK, double* vdPdLH,
				double* vdPdLF, double* vdTdL,double BHP);

   /* private:
  tipoValidacaoJson_t validacaoJson;
  tipoSimulacao_t tipoSimulacao;

  void writeSchemaRede();
  Document parseSchema();
  Document parseEntrada();
  void validateVsSchema(Document *schemaMrtDoc, Document *mrtDoc);

  void parse_configuracao_inicial(Value& configuracao_inicial_json);
  void parse_arquivos(Value& arquivos_json);
  void parse_conexao(Value& conexao_json);*/
};

#endif /* LERAS_H_ */
