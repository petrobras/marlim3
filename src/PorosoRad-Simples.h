/*
 * TrocaCalor.h
 *
 *  Created on: 24/04/2014
 *      Author: bwz1
 */

#ifndef POROSORAD_SIMPLES_H_
#define POROSORAD_SIMPLES_H_

#include "celRad-Simples.h"
#include "estruturasPoroso.h"
#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "GeometriaPoro.h"
#include "estruturaTabDin.h"
#include "variaveisGlobais1D.h"

// criar relatorio dos arquivos de dados de saida da simulacao
extern ofstream arqRelatorioPerfis;

// criar objeto de logger
extern Logger logger;

// criar string do path do arquivo de entrada
extern string pathArqEntrada;

// criar string do path dos arquivos de entrada PVTSIM e SnapShot
extern string pathArqExtEntrada;

// criar string do path e prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;

//diretorio contendo os arquivos de saida
extern string diretorioSaida;

//extern int contaExit;

//extern int modoTransiente;

//extern int simulaTransiente;

/*extern int reinicia;
extern double tempo;
extern detTempoPoroso temp;
extern detDiriPoroso pW;

extern double* satARes;
extern double* satLRes;
extern double* satAPoc;
extern double* tempoSatARes;
extern double* tempoSatAPoc;
extern double* tempoSatLRes;
extern int nsatARes;
extern int nsatAPoc;
extern int nsatLRes;
extern double* pRes;
extern double* tempoPRes;
extern int nPRes;*/
//extern double lixo5;
//extern double tempo;

//struct com as propriedade do fluido cpara o caso de tabela dinâmica
/*struct tabelaDinamica{
    int id;
    int TwoOrThree;//indicador se a tabela e bifpasica ou trifasica
    double **rholF;
    double **rhogF;
    double **DrholDpF;
    double **DrhogDpF;
    double **DrholDtF;
    double **DrhogDtF;
    double **tit;
    double **rs;
    double **cplF;
    double **cpgF;
    double **HlF;
    double **HgF;
    double **valBO;
    double **valZ;
    double **valdZdT;
    double **valdZdP;
    double **viscO;
    double **viscG;
    double *PBF;
    double *TBF;
    double pmax;
    double pmin;
    double tmax;
    double tmin;
    double delP;
    double delT;
    int npontosT;
    int npontosP;
    int celIni;
    int celFim;
    tabelaDinamica(){
        id=0;
        TwoOrThree=-1;//indicador se a tabela e bifpasica ou trifasica
        rholF=0;
        rhogF=0;
        DrholDpF=0;
        DrhogDpF=0;
        DrholDtF=0;
        DrhogDtF=0;
        tit=0;
        rs=0;
        cplF=0;
        cpgF=0;
        HlF=0;
        HgF=0;
        valBO=0;
        valZ=0;
        valdZdT=0;
        valdZdP=0;
        viscO=0;
        viscG=0;
        PBF=0;
        TBF=0;
        pmax=0.;
        pmin=0.;
        tmax=0.;
        tmin=0.;
        delP=0.;
        delT=0.;
        npontosT=0;
        npontosP=0;
        celIni=0;
        celFim=0;
    }
    ~tabelaDinamica(){
		for(int j=0; j<npontosP+1;j++){
			if(rhogF!=0)delete[] rhogF[j];
			if(rholF!=0)delete[] rholF[j];
			if(DrhogDpF!=0)delete[] DrhogDpF[j];
			if(DrhogDtF!=0)delete[] DrhogDtF[j];
			if(DrholDpF!=0)delete[] DrholDpF[j];
			if(DrholDtF!=0)delete[] DrholDtF[j];
			if(valBO!=0)delete[] valBO[j];
			if(HgF!=0)delete[] HgF[j];
			if(HlF!=0)delete[] HlF[j];
			if(cpgF!=0)delete[] cpgF[j];
			if(cplF!=0)delete[] cplF[j];
			if(valZ!=0)delete[] valZ[j];
			if(valdZdT!=0)delete[] valdZdT[j];
			if(valdZdP!=0)delete[] valdZdP[j];
			if(tit!=0)delete[] tit[j];
			if(rs!=0)delete[] rs[j];
			if(viscG!=0)delete[] viscG[j];
			if(viscO!=0)delete[] viscO[j];
		}
		if(rhogF!=0)delete [] rhogF;
		if(rholF!=0)delete[] rholF;
		if(DrhogDpF!=0)delete[] DrhogDpF;
		if(DrhogDtF!=0)delete[] DrhogDtF;
		if(DrholDpF!=0)delete[] DrholDpF;
		if(DrholDtF!=0)delete[] DrholDtF;
		if(valBO!=0)delete[] valBO;
		if(HgF!=0)delete[] HgF;
		if(HlF!=0)delete[] HlF;
		if(cpgF!=0)delete[] cpgF;
		if(cplF!=0)delete[] cplF;
		if(valZ!=0)delete[] valZ;
		if(valdZdT!=0)delete[] valdZdT;
		if(valdZdP!=0)delete[] valdZdP;
		if(tit!=0)delete[] tit;
		if(rs!=0)delete[] rs;
		if(viscG!=0)delete[] viscG;
		if(viscO!=0)delete[] viscO;
		if(TBF!=0)delete [] TBF;
		if(PBF!=0)delete [] PBF;
    }
};*/

class PorosRadSimp{
	public:
	DadosGeoPoro geom;
	ProFluCol fluc;
	ProFlu flup;
	celradSimp* celula;
	int lenth;
	int permanente;
	int acessorio;
	double erroPermanente;
	int* ncamada;
	int nglobal;
	int ncel;
	double* drcamada;
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

	////////////////////////////////////////////////////////////////////////////
	detTempoPoroso temp;
	materialPoroso* mat;//vetor com os materiais cadastrados no Json
	cortePoroso corte;//vetor com as secoes transversais cadastradas no json
	DadosGeoPoro dutosMRT;
	detDiriPoroso pW;

	double satAconat;
	double ippoco;
	int nmaterialPoroso;
	double pInt;
	double pRes2;
	double* pRes;
	double* tempoPRes;
	int nPRes;
	double zDatumRef;

	double condGlob;
	double condLoc;
	double qTotal;
	double pInt0;

	double espessuraRes;

	int flashCompleto;//atributo que indica se vai ser usada a tabela de PVTSim ou Black oil 1-> PVTSim
	//int tabelaDinamica;
	int usaTabela;//atributo que indica que se est�pa usando uma tabela, seja no caso black oil ou no caso PVTSim
	string pvtsimarq;//arquivo PVTSim
	int tabp;//indica, para o caso black oil, que se montara uma tabela de fator de compresibilidade e suas derivadas
		//antes de se iniciar a simulacao, linha de poroducao
	tabelaFlashPoroso flash;//objeto com as tabelas de propriedade dos fluidos obtidos de um aqruivo PVTSim
	int npontos;//numero de pontos de qualquer tabela-> para black oil e PVTSim
	double** zdranP;//tabela de fator de compressibilidade, black oil (pressao reduzida X temperatura reduzida)
	double** dzdpP;//tabela com a derivada do Z com a pressao, black oil (pressao reduzida X temperatura reduzida)
	double** dzdtP;//tabela com a derivada do Z com a temperatura, black oil (pressao reduzida X temperatura reduzida)
	int nvecEmul;//para o caso em que o modelo de emulsao e dado pelo usuario por meio de pares BSWXmultiplicadore,
	//nvecEmul tamanho do vetor
	double* BSWVec;//vetor com os valores de emulsao fornecido pelo usuario
	double* emulVec;//vetor com os valores do multiuplicadores de viscosidade fornecido pelo usupario
	int miniTabAtraso;
	double miniTabDp;
	double miniTabDt;
	detTabViscPoroso tabVisc;
	detTabelaEntalpPoroso tabent;//tabela com os limites de pressao e temepratura para a tabela de PVTSim
	detTabelaEntalpPoroso tabentCrit;//tabela com os limites de pressao reduzida e temperatura reduzida para o
	//fator de compressibilidade do gas e suas derivadas
	composicionalPoroso compDet;
	int corrDeng;

	double* satARes;
	double* satLRes;
	double* satAPoc;
	double* tempoSatARes;
	double* tempoSatLRes;
	double* tempoSatAPoc;

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
    varGlob1D* vg1dSP;
	//////////////////////////////////////////////////////////////////////////////////////////////


	/*PorosRadSimp(const DadosGeoPoro vgeom=DadosGeoPoro(),const int vpermanente=-1, const int* const vncamada=0,
			const double* const vdrcamada=0,double** const vPcamada=0,const double vPint=0.,
			const double vpresRes2=0.,const double vpresRes=0.,const double vtRes=0, const double vdt=0
			,const double pres=0, const double vzD0=0, const double vzD1=0, const double vdh=1,
			double** const vsL=0,double** const vsW=0,const double vsLRes=0,const double vsWRes=0,
			const double vsWPoc=0,const double vsatConata=0,const double vip=1.,
			tabelaPemRelOA vkRelOA=tabelaPemRelOA(),tabelaPemRelOG vkRelOG=tabelaPemRelOG(),
			tabelaPresCapOA vpcOA=tabelaPresCapOA(),tabelaPresCapGO vpcGO=tabelaPresCapGO(),
			ProFluCol vfluc=ProFluCol(), ProFlu vflup=ProFlu());//alteracao2*/

    PorosRadSimp();

    PorosRadSimp(string nomeArquivoEntrada,varGlob1D* Vvg1dSP=0);

	PorosRadSimp(const PorosRadSimp&); //construtor por c�pia
	PorosRadSimp& operator=(const PorosRadSimp&);
	~PorosRadSimp(){

		    if(tabp>0){
				  for(int i=0; i<npontos+2;i++) delete[] zdranP[i];
				  delete [] zdranP;
				  for(int i=0; i<npontos+2;i++) delete[] dzdpP[i];
				  delete [] dzdpP;
				  for(int i=0; i<npontos+2;i++) delete[] dzdtP[i];
				  delete [] dzdtP;
		     }

		    if(flashCompleto==1){
			   for(int j=0; j<tabent.npont+1;j++) delete [] flash.rholF[j];
			   delete [] flash.rholF;
			   for(int j=0; j<tabent.npont+1;j++) delete[] flash.rhogF[j];
			   delete [] flash.rhogF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDpF[j];
		       delete [] flash.DrholDpF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDpF[j];
		       delete [] flash.DrhogDpF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDtF[j];
		       delete [] flash.DrholDtF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDtF[j];
		       delete [] flash.DrhogDtF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhowDtF[j];
		       delete [] flash.DrhowDtF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.RSF[j];
		       delete [] flash.RSF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cplF[j];
		       delete [] flash.cplF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpgF[j];
		       delete [] flash.cpgF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpwF[j];
		       delete [] flash.cpwF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HlF[j];
		       delete [] flash.HlF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HgF[j];
		       delete [] flash.HgF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigOGF[j];
		       delete [] flash.sigOGF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigWGF[j];
		       delete [] flash.sigWGF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscO[j];//alteracao6
		       delete [] flash.viscO;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscG[j];//alteracao6
		       delete [] flash.viscG;

		       delete [] flash.PBF;
		       delete [] flash.TBF;
		   }

		   if(nvecEmul>0){
		    	  delete [] BSWVec;
		    	  delete [] emulVec;
		    }
			if(flup.corrOM==7){
				if(tabVisc.parserie>0){
					delete [] tabVisc.visc;
					delete [] tabVisc.temp;
				}
			}
			if(nsatARes>0){
				delete [] satARes;
				delete [] tempoSatARes;
			}
			if(nsatLRes>0){
				delete [] satLRes;
				delete [] tempoSatLRes;
			}
			if(nsatAPoc>0){
				delete [] satAPoc;
				delete [] tempoSatAPoc;
			}
			if(nPRes>0){
				delete [] pRes;
				delete [] tempoPRes;
			}


	        if(ncamada) delete[] ncamada;
	        if(drcamada) delete[] drcamada;
	        if(lenth>0){
	        		delete [] celula;
	        }

	} //destrutor


		/*kRelOA=vkRelOA;
		kRelOG=vkRelOG;
		pcOA=vpcOA;
		pcGO=vpcGO;*/

  void parse_tempo(Value& tempo_json);
  void parse_tabela(Value& tabela_json);
  void parse_corte(Value& corte_json);
  void parse_materiais(Value& materialPoroso_json);
  void parse_kRelOA(Value& kRelOA_json);
  void parse_kRelOG(Value& kRelOG_json);
  void parse_pcOA(Value& pcOA_json);
  void geraTabFlash(int var);
  void parse_fluido_producao(Value& fluidos_producao_json);
  void parse_fluido_complementar(Value& fluido_json);
  void parse_PW(Value& pW_json);
  void iniciaVariaveis();
  void lerDados(string nomeArquivoEntrada);
  void atualizaCel(int i);
  void preparaTabDin();
  void auxMiniTab(ProFlu& flu);
  void geraMiniTabFlu();


  void geraCel(double** const vPcamada=0,double** const vsL=0,double** const vsW=0,const double vsLRes=0,const double vsWRes=0);
  double interpolaTabela(int nserie, double valx, double* x, double* y);
  double fkO(double satW, double satG);
  double marchaperm(double mastot);
  double transperm(double mastot=-1);
  void renovaPres(int i, double mTot);
  double SIGN(double a, double b);
  double zriddr(double x1,double x2);
  void indrazT(int& ind, double& raz);
  void indrazPW(int& ind, double& raz);
  void indraz(int& ind, double& raz, double* serieTempo, int nserie);
  void defineDT(int perm=0);
  void atualizaIni();
  double transtrans(int perm=0);
  void pseudoTrans(int partida=1);
  void pseudoTrans2(int partida=1);
  void dVazdPw();
  void solveTrans();
  void avancoSW(double Ndt);
  void reavaliaDT(double& Ndt);
  void reiniciaEvoluiSW(double Ndt);
  void avancoSWcorrec();
  void avancoPressao();
  void FeiticoDoTempo();
  void FeiticoDoTempoSW();
  void FeiticoDoTempoPQ();
  FullMtx<double> perfil();
  double psia(const double p)const { return (p*0.9678411)*14.69595;} //p->kgf, transforma kgf para psia // alteracao0
  double Faren(const double t)const { return 1.8*t + 32;} //t em Celcius, transforma Celcius para Farenheit // alteracao0
  double cel(const double t)const {return (t - 32)/1.8;} //t em Farenheit, transforma Farenheit para Celcius //alteracao2
  double kgf(const double p)const { return p/(14.69595*0.9678411);}//p em psia, transforma psia para kgf/cm2 //alteracao2

};


#endif /* POROSORAD_SIMPLES_H_ */
