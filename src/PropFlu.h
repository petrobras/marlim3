#ifndef _PROFL
#define _PROFL

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
#include <iostream>
#include "Vetor.h"
#include "Matriz.h"
#include "Log.h"
#include "MarlimComposicional.h"
#include "variaveisGlobais1D.h"
/*extern "C" void Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions(int iNComp, double* oZ, double* oMW, double* oTc, double* oPc, double* oW,
		double* oPeneloux, double* oKij, double* oLij,
        int iLiqPhaseModel, int iVapPhaseModel, int iLiqDensityCalculationMethod, int iVapDensityCalculationMethod, double dStockTankPressure,
        double dStockTankTemperature,
		double dGivenInitialBeta, double* oGivenInitialLiqComposition, double* oGivenInitialVapComposition,
		int* iCalculatedStockTankThermodynamicCondition, double* dStockTankVaporMassFraction,
		double* dStockTankLiquidDensity,
        double* dStockTankVaporDensity, double* dStockTankCompositionalGOR, int* iIER);
extern "C" void Marlim_CalculateMixtureThermodynamicCondition(double dFlashPressure, double dFlashTemperature, int iNComp, double* oMW, double* oZ, double* oTc,
		double* oPc, double* oW, double* oPeneloux, double* oKij, double* oLij, int iLiqPhaseModel, int iVapPhaseModel,
		double dGivenInitialBeta, double* oGivenInitialLiqComposition, double* oGivenInitialVapComposition,
		double* dCalculatedBubbleT, int* iIER_BubbleT, double* dCalculatedDewT, int* iIER_DewT, double* dCalculatedBeta,
		double* oCalculatedLiqComposition, double* oCalculatedVapComposition, int* iIER_Flash, double* dCalculatedBubbleP,
		int* iIER_BubbleP, int* iCalculatedThermodynamicCondition, double* dLiquidPhaseMW, double* dVaporPhaseMW, double* dVaporMassFraction, int* iIER);
*/
extern int contaExit;
//extern double** zdranP;
//extern double** zdranG;
//extern double** dzdpP;
//extern double** dzdpG;
//extern double** dzdtP;
//extern double** dzdtG;
//extern int ModelCp;
//extern int Modeljtl;
//extern double** cpg;
//extern double** cpl;
//extern double** drholdT;
//extern int ndiv;
//extern int npontos;
//extern int trackRGO;
//extern int trackDeng;
//extern int ninjgas;
//extern int lingas;
//extern int nfluP;
//extern double dengG;
//extern double dengP;

// relatorio dos arquivos de dados de saida da simulacao
extern ofstream arqRelatorioPerfis;

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;

// criar objeto para log da aplicacao
extern Logger logger;

extern int modoTransiente;

extern int simulaTransiente;

constexpr double CoefGopal[] = { 1.6643,-2.2114,-0.3647, 1.4385, 0.5220,-0.8511,
                              -0.0364, 1.0490, 0.1391,-0.2988, 0.0007, 0.9969,
                              0.0295,-0.0825, 0.0009, 0.9967,-1.3570, 1.4942,
                              4.6315,-4.7009, 0.1717,-0.3232, 0.5869, 0.1229,
                              0.0984,-0.2053, 0.0621, 0.8580, 0.0211,-0.0527,
                              0.0127, 0.9549,-0.3278, 0.4752, 1.8223,-1.9036,
                              -0.2521, 0.3871, 1.6087,-1.6635,-0.0284, 0.0625,
                              0.4714,-0.0011, 0.0041, 0.0039, 0.0607, 0.7927};

struct tabelaDina{
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
};

struct miniTabelaDina{
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
    double *PBF;
    double *TBF;
    double pmax;
    double pmin;
    double tmax;
    double tmin;
};

/*struct miniTabelaDina{
    double rholF[2][2];
    double rhogF[2][2];
    double DrholDpF[2][2];
    double DrhogDpF[2][2];
    double DrholDtF[2][2];
    double DrhogDtF[2][2];
    double tit[2][2];
    double rs[2][2];
    double cplF[2][2];
    double cpgF[2][2];
    double HlF[2][2];
    double HgF[2][2];
    double valBO[2][2];
    double valZ[2][2];
    double valdZdT[2][2];
    double valdZdP[2][2];
    double PBF[2];
    double TBF[2];
    double pmax;
    double pmin;
    double tmax;
    double tmin;
};*/

class ProFlu{
        public:
	    int id;
	    int npseudo;//numero de pseudocomponentes do caso composicional
	    int posic;
        double mascor;//1->vaz�o m�ssica da corrente bif�sica, kg/s
        double API;//2 ->grau api do �leo da corrente bif�sica
        double RGO;//3 ->RGO da corrente bif�sica m3/m3
        double rDgD;//razao entre a densidade do gas dissolvido e a densidade do gas no separador
        double rDgL;//razao entre a densidade do gas livre e a densidade do gas no separador
        double Deng;//4 ->densidade do g�s
        double BSW;//5 ->BSW da corrente, m3/m3, varia de 0 a 1
        double Denag;//6 ->densidade da �gua
        double TempL;//7 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, temperatura C de um dos pontos do ASTM
        double LVisL;//8 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtempl,vlvisl)
        double TempH;//9 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, temperatura C de um dos pontos do ASTM
        double LVisH;//10 -> para o caso de c�lculo de viscosidade do l�quido com o aux�lio do ASTM, viscosidade cP de um dos pontos do ASTM, (vtemph,vlvish)
        int tipoemul;//indicador de correlacao de emulsao, 0->voiscosidade de mistura oleo agua,
        //ponderada pelo titulo de agua, 1->Woeflin fraco, 2->Woeflin medio, 3->Woeflin forte, 4-> exponencial,
        //5->Pal Rhoades, 6-> par, BSWXmultiplicador, informado pelo usuario, 7-> viscosiddae de oleo abaixo
        //do BSW de saturacao
        double bswCorte;//BSW de saturacao da emulsao

        int nvecEmul;//numero de elementos dos pares BSWXmultiplicador para o modelo de emulsao 6
        double* BSWVec;//vetor com o BSW informado pelo usuario no modelo de emulao 6
        double* emulVec;//vetor com o multiplicador de emulsao informado pelo usuario no modelo de emulao 6

        //////////////////////////////////////////////////////////////////////
        //parametros para o modelo de RS de Begs e Vazquez
        double FAC;
        double SG100vb;
        double Avb;
        double Bvb;
        double Cvb;
        ///////////////////////////////////////////////////////////////////////
        double IRGO;//RGO no sistema ingles
        double PC;//pressao critica, psia, para o gas livre com a densidade nas condicoes standard separador
        double TC;//Temperatura critica, R, para o gas livre com a densidade nas condicoes standard separador
        double PCis;//pressao critica, psia, para o gas livre com a densidade nas condicoes in-situ
        double TCis;//Temperatura critica, R, para o gas livre com a densidade nas condicoes in situ
        //os termos a seguir representam termos auxiliares na aproxima��o de queda de press�o admitindo escoamento homog�neo:
        double term1;//nao utilizado
        double term2;//nao utilizado
        double term3;//nao utilizado
        double term4;//nao utilizado
        double term5;//nao utilizado
        double PCF;//nao utilizado //kgf/cm2
        double TCF;//nao utilizado//Celcius
        double yco2;//fracao molar de CO2
        int corrC;//indicador da correlacao para o calculo da pressao e temperatura criticas do gas,
        //3 correlacoes, retoiradas diretamente do Marlim2, 1 e 2 -> mais adequadas para casos com CO2

       // int npontos;
        int tab;//atributo que indica, para o caso black oil, se o fator de compressibilidade do gas e suas derivadas serao obtidas
        //por meio de interpolacao de tabela ou nao, 0->nao usar tabela, calculo direto, 1-> interpolacao
        //FullMtx<double> zdran;
        //FullMtx<double> dzdp;
        //FullMtx<double> dzdt;
        int npontos;//numero de pontos de tabela
        int npontosB;//numero de pontos de tabela
        int nfluP;//sem uso
    	int ModelCp;//indica se, para o caso black oil, vai ser utilizado um modelo de correlacao ou
    	//dados de uma tabela flash, 0->correlacao direta, 1-> tabela
    	int Modeljtl;//indica se, para o caso black oil, vai ser utilizado um modelo de correlacao ou
    	//dados de uma tabela flash, 0->correlacao direta, 1-> tabela
    	int CalcLat;//nao utilizado
    	int trackRGO;//nao utilizado
    	int trackDeng;//nao utilizado
    	int corrDeng;//nao utilizado
    	int ninjgas;//nao utilizado
    	int lingas;//nao utilizado
        double** zdranP;//Tabela de fator de compressibilidade, para o caso black oil, se o uso de
        //tabela for indicado
        double** dzdpP;//Tabela da derivada com a pressao reduzida do fator de compressibilidade, para o caso black oil, se o uso de
        //tabela for indicado
        double** dzdtP;//Tabela da derivada com tempertura reduzida do fator de compressibilidade, para o caso black oil, se o uso de
        //tabela for indicado
        double** cpg;//Tabela do calor especpifico de gas, para o caso black oil, se o uso de
        //tabela for indicado
        double** cpl;//Tabela do calor especifico de liquido, para o caso black oil, se o uso de
        //tabela for indicado
        double** drholdT;//Tabela da derivada da massa especifica de liquido com a temperatura, para o caso black oil, se o uso de
        //tabela for indicado

        ////////////////////////////////////////////////////////////////////////////////////
        /////vetores auxiliares para a correlacao de Lasater de calculo de RS, definidos no construtor
        /////seguindo a programacao existente no Marlim 2
        /////////////////////////////////////////////////////////////////////////////////////
        Vcr<double> BPPF;//alteracao4
        Vcr<double> GMF;//alteracao4
        Vcr<double> APIEMW;//alteracao4
        Vcr<double> EMW;//alteracao4
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        int corrSat;//Correlacao de RS, Stft3/bpd, 0->VAZQUEZ E BEGGS, 1-> LASATER, 2-> STANDING, 3->GLASO, 4->Livia
        int corrOM;//Correlacao de viscosidade de oleo morto, cP, 0->ASTM 1->Beggs&Robinson 2->Beggs & Robinson Modificado 3-> Glaso 4-> Kartoatmodjo_Schmidt 5->Petrosky_Farshad 6->Beal
        int corrOV;//Correlacao da viscodiadede de oleo vivo, cP, 0->Beggs_Robinson 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad
        int corrOS;//Correlacao de oleo subsaturado, cP, 0->VAZQUEZ E BEGGS 1->Kartoatmodjo_Schmidt 2->Petrosky_Farshad 3-> Beal 4-> Khan
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///parametros usados no cpalculo da correlacao de RS de Livia
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        double A0Liv;
        double A1Liv;
        double A2Liv;
        double A3Liv;
        double A4Liv;
        double B0Liv;
        double B1Liv;
        double B2Liv;
        double B3Liv;
        double B4Liv;
        double C0Liv;
        double C1Liv;
        double C2Liv;
        double C3Liv;
        double C4Liv;
        double D0Liv;
        double D1Liv;
        double D2Liv;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        double *PBPVTSim;//vetor com os valores de pressao de bolha obtidos de um arquivo PVTSim
        double *TBPVTSim;//vetor com os valores de temperatura de bolha obtidos de um arquivo PVTSim
        double **TabRSLivia;//tabela com valores de RS, apenas para o caso black oil, caso se deseja usar valores
        //interpolados da correlacao, em vez do calculo direto
        double aemul;//parametro a do modelo exponencial de emulsao
        double bemul;//parametro b do modelo exponencial de emulsao
        double PHI100;//parametro para o multiplicador de emulsao do modelo Pal Rhodes
        int tabRSPB;//indicador de que se deseja trabalhar com tabela de RS no caso de modelo black oil,
        //emn vez de se utilizar o calculo direto da correlacao, se 1-> tabela
        int flashCompleto;//indica que se deseja trabalhar com um modelo de tabela PVTSim, 1->tabela PVTSim
        //se for =2, indica que se utilizará o modelo composicional
        int tabelaDinamica;
        int viscBlackOil;//indica se, para o caso de tabelas OVTSim, ainda assim se deseja trabalhar com
        //valores de viscosidade obtidos de um modelo black oil
        int indiceFlash;//nao utilizado

        /////////////////////////////////////////////////////////////////////////////////////////////
        //////tabelas de propriedades obtidas de um arquivo PVTSim/////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        double **rholF;
        double **rhogF;
        double **DrholDpF;
        double **DrhogDpF;
        double **DrholDtF;
        double **DrhogDtF;
        double **DrhowDtF;
        double **RSF;
        double **cplF;
        double **cpgF;
        double **cpwF;
        double **HlF;
        double **HgF;
        double **sigOGF;
        double **sigWGF;
        double **viscO;
        double **viscG;

        int* itempAnt;
        int* ipresAnt;

        //Caso composicional//////////////////////////////
        double *fracMol;
        double *masMol;
        double *tempCrit;
        double *presCrit;
        double *fatAcent;
        double *parac;
        double *TIndepPeneloux;
        double *kij;
        double *lij;
        double *CpIGCoefs;
        int liqModel;
        int iIER_BubbleT;
        int iIER_DewT;
        int iIER_Flash;
        int iIER_BubbleP;
        int iIER;
        double Pmol;

   	 	double dCalculatedBubbleT;
   	 	double dCalculatedDewT;
   	 	double dCalculatedBeta;
   	 	double* oCalculatedLiqComposition;
   	 	double* oCalculatedVapComposition;
   	 	double dCalculatedBubbleP;
   	 	int iCalculatedThermodynamicCondition;
   	    int iCalculatedStockTankThermodynamicCondition;
   	 	double dLiquidPhaseMW;
   	 	double dVaporPhaseMW;
   	 	double dVaporMassFraction;
   	 	double dStockTankVaporMassFraction;
   	 	double dStockTankLiquidDensity;
   	 	double dStockTankVaporDensity;

   	 	int modoBlackTemp;

   	    tabelaDina tabDin;
   	    miniTabelaDina miniTabDin;

       	int parserie;
        double* viscTab;
        double* tempTab;


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int modelaAgua;//para o caso de uso de tabela PVTSim, indica se se deseja trabalhar com uma correlacao interna
        //para se calcular o Joule Thomson da agua ou se se deve usar os valores de propriedades da tabela PVTSim
        //double *PBF;
        //double *TBF;
        double pMinEqu;//nao utilizado
        double tMinEqu;//nao utilizado
        int delpTab;//nao utilizado
        //atributos auxiliares para a interpolacao das tabelas PVTSim, aceleram o processo de
        //busca dos intervalos de pressao e temperatura da tabela onde se deve fazer a interpolacao
        //estes parametros sao originalmente obtidos na leituda da tabela na classe Leitura;
        //as tabelas PVTSim, geralmente, tem intervalos equidistantes de pressao e temperatura
        //so que em lotes. Geralmente, para pressao e temperatura abaixo dos valores de
        //condicao standard, se admite que os intervalos nao sao equidistantes, os indices que inicam onde se
        //esta abaixo da pressao e temperatura standard sao dados por indMinEquP e indMinEquT
        //acima destes indices, os intervalos de pressao e temperatura sao equidistantes em lotes,
        //sao construidos vetores emn que se armazena estes Dp e DT para cada intervalo de indice das tabelas
        //estes vetores auxiliares sao pchange (pressao onde acima dela um novo delp se esbelece),
        //dpchange (vetor com os delps),ipchange (indice de pressao onde acima dele um novo delp se esbelece),
        //tchange (temperatura onde acima dela um novo delp se esbelece),
        //dtchange (vetor com os delTs),itchange (indice de temperatura onde acima dele um novo delp se esbelece),
        vector<double> pchange;
        vector<double> dpchange;
        vector<int> ipchange;
        int indMinEquP;

        int deltTab;
        vector<double> tchange;
        vector<double> dtchange;
        vector<int> itchange;
        int indMinEquT;
        int erroP;//indica se se deseja ou nao usar o acelerador de interpocao indicado acima para a pressao
        //1->usa
        int erroT;//indica se se deseja ou nao usar o acelerador de interpocao indicado acima para a temperatura
        //1->usa

        double bASTM1;//valores auxiliares para o calculo da viscosiudade de oleo morto com o
        //metodo ASTM
        double bASTM2;//valores auxiliares para o calculo da viscosiudade de oleo morto com o
        //metodo ASTM
        varGlob1D* vg1dSP;
        
        int modoParafina;
    	double* oPressurePoints;
    	double* oCloudPointTemperatures;
    	double* oStructurePressures;
    	double* oStructureTemperatures;
    	double* oStructureDensities;
    	double* oStructureLiqMW;
    	double* oStructureCPWax;
    	double* oStructureThermCond;
    	double* oStructureWaxConcs;
    	double* oMolecularWeightsOfWaxComponents;
    	double* oLiquidDensitiesOfWaxComponents;

    	int npseudoWax;
    	int npseudoWaxOut;
    	int iPresWax;
    	int iTempWax;
    	//////////////////////////////
    	int iIERW;
    	int iWaxComponentCountOutput;
    	double dCloudPointTOutput;
    	double dInterpolatedCPWaxOutput;
    	double dInterpolatedThermCondOutput;
    	double dInterpolatedDensOutput;
    	double dInterpolatedLiqMWOutput;
    	double* oLiquidDensitiesOfWaxComponentsOutput;
    	///////////////////////////////
    	double* oInterpolatedWaxConcsTDerivOutput;
    	double* oInterpolatedWaxConcs;
    	double* oMolecularWeightsOfWaxComponentsOut;

    	double coefViscWax;

    	double JTLiquidoSimple;


        /*static double CoefGopal[] = { 1.6643,-2.2114,-0.3647, 1.4385, 0.5220,-0.8511,
                            -0.0364, 1.0490, 0.1391,-0.2988, 0.0007, 0.9969,
                             0.0295,-0.0825, 0.0009, 0.9967,-1.3570, 1.4942,
                             4.6315,-4.7009, 0.1717,-0.3232, 0.5869, 0.1229,
                             0.0984,-0.2053, 0.0621, 0.8580, 0.0211,-0.0527,
                             0.0127, 0.9549,-0.3278, 0.4752, 1.8223,-1.9036,
                            -0.2521, 0.3871, 1.6087,-1.6635,-0.0284, 0.0625,
                             0.4714,-0.0011, 0.0041, 0.0039, 0.0607, 0.7927}; */
        //static double CoefGopal[48];
     
        double FalsaCorda(double ,double ,double ,double ,double delta=0.00001 ,double epsn=0.00001,int maxit=100)const;//Calculo de zero de fun��o apenas para calculo de zero na FNDran
 //       void gauleg(const double x1, const double x2, Vcr<double>& x, Vcr<double>& w)const;//subrotina auxiliar para quadratura gaussiana
//usando polin�mio de Legendre, d� os pontos x da quadratura e seus pesos, este s�o obtidos a partir de  W=1, x1 e x2 s�o os limites de integra��o,
//o integrando � do tipo f(x) pura e simplesmente. x-> pontos em que se calcular� a  quadratura, w-> os pesos
       void gauleg(const double x1, const double x2, double* const x, double* const w, int size)const;//subrotina auxiliar para quadratura gaussiana
//usando polin�mio de Legendre, d� os pontos x da quadratura e seus pesos, este s�o obtidos a partir de  W=1, x1 e x2 s�o os limites de integra��o,
//o integrando � do tipo f(x) pura e simplesmente. x-> pontos em que se calcular� a  quadratura, w-> os pesos
        double quadleg(const double x1, const double x2, int tipo,int fase,const double par,const int n)const; //quadratura gaussiana usando Legendre, x1-> limite inferior, x2-> limite superior
//n->n�mero de pontos da quadratura. tipo->integra na press�o (1); integra na temperatura (2). fase->g�s->1; l�quido->2, par�metro-> press�o se tipo=2 e temperatura se tipo=1

	//public:
        ProFlu(varGlob1D* Vvg1dSP=0,double vapi=1,double vrgo=1,double vdeng=1,double vbsw=1,
        		double vdenag=1,double vtempl=10,double vlvisl=1,
             double vtemph=10,double vlvish=1,int vtipoemul=0, double vaemul=0, double vbemul=0,
			 double vPHI100=0.765, double vbswCorte=0, int vtab=0,double vyco2=0, int vcorrC=0,
			 int vcorrSat=0,int vcorrOM=0,int vcorrOV=0,int vcorrOS=0,
			   int vflash=0, int id=0, int vnpseudo=0, double vpcf=0,double vtcf=0); //construtor default//alteracao4

                ProFlu(varGlob1D* Vvg1dSP, const double* const fluido, int vtipoemul,
                        double vaemul, double vbemul,double vPHI100, double vbswCorte,
                        int vtab, double vyco2, int vcorrC,int vcorrSat,int vcorrOM,int vcorrOV,int vcorrOS,
						int vflash,
						int vid, int vnpseudo=0);//alteracao4

                ProFlu(varGlob1D* Vvg1dSP, const double* const fluido, const double* const astm, int vtipoemul
                        , double vaemul, double vbemul,double vPHI100,
                         double vbswCorte,int vtab, double vyco2, int vcorrC,int vcorrSat,
						 int vcorrOM,int vcorrOV,int vcorrOS,int vflash, int vid, int vnpseudo);//alteracao4
       ProFlu(const ProFlu&); //construtor por c�pia

                //double Primitivas(int) const;
//#include <iostream>
		ProFlu& operator=(const ProFlu&);
		double operator[](int i)const;
 		ProFlu operator+(const ProFlu&) const;

		~ProFlu(){
 			if(flashCompleto==2 && npseudo>0)delete [] fracMol;
 			if(flashCompleto==2 && npseudo>0)delete [] oCalculatedLiqComposition;
 			if(flashCompleto==2 && npseudo>0)delete [] oCalculatedVapComposition;
 			if(flashCompleto==2 && npseudo>0 && (*vg1dSP).simulaTransiente==1){
    			for(int j=0; j<2;j++){
    				delete[] miniTabDin.rhogF[j];
    				delete[] miniTabDin.rholF[j];
    				delete[] miniTabDin.DrhogDpF[j];
    				delete[] miniTabDin.DrhogDtF[j];
    				delete[] miniTabDin.DrholDpF[j];
    				delete[] miniTabDin.DrholDtF[j];
    				delete[] miniTabDin.valBO[j];
    				delete[] miniTabDin.HgF[j];
    				delete[] miniTabDin.HlF[j];
    				delete[] miniTabDin.cpgF[j];
    				delete[] miniTabDin.cplF[j];
    				delete[] miniTabDin.valZ[j];
    				delete[] miniTabDin.valdZdT[j];
    				delete[] miniTabDin.valdZdP[j];
    				delete[] miniTabDin.tit[j];
    				delete[] miniTabDin.rs[j];
    			}
    			delete[] miniTabDin.rhogF;
    			delete[] miniTabDin.rholF;
    			delete[] miniTabDin.DrhogDpF;
    			delete[] miniTabDin.DrhogDtF;
    			delete[] miniTabDin.DrholDpF;
    			delete[] miniTabDin.DrholDtF;
    			delete[] miniTabDin.valBO;
    			delete[] miniTabDin.HgF;
    			delete[] miniTabDin.HlF;
    			delete[] miniTabDin.cpgF;
    			delete[] miniTabDin.cplF;
    			delete[] miniTabDin.valZ;
    			delete[] miniTabDin.valdZdT;
    			delete[] miniTabDin.valdZdP;
    			delete[] miniTabDin.tit;
    			delete[] miniTabDin.rs;
    			delete[] miniTabDin.TBF;
    			delete[] miniTabDin.PBF;
 			}
 			if(modoParafina==1 && npseudoWax>0){
				delete[] oMolecularWeightsOfWaxComponentsOut;
				delete[] oInterpolatedWaxConcsTDerivOutput;
				delete[] oInterpolatedWaxConcs;
				delete[] oLiquidDensitiesOfWaxComponentsOutput;
 			}
 			//delete itempAnt;
 			//delete ipresAnt;
 		}

        void MudaVazMas(double NovaVaz){mascor=NovaVaz;}//Modifica a vaz�o m�ssica da corrente
                double* TermAux(double* saida)const{saida[0]=term1;saida[1]=term2;saida[2]=term3;saida[3]=term4;saida[4]=term5; return saida;} //retorna os termos auxiliares, para beggsebrill

                //double PreRog(double ,double ,double ,double ,double delta=0.00001 ,double epsn=0.00001,int maxit=100) const;

        double PreRog(double a/*chute de pressao*/,
        		           double b/*chute de pressao*/,double rog/*massa especifica de gas*/,
						   double temp/*temperatura*/,double delta=0.00000000001 ,
        		           double epsn=0.00000000001,int maxit=300) const;//retorna o valor de pressao, kgf/cm2
        //para um dado valor de massa especifica de gas, kg/m3, e de temperatura C
        void RenovaFluido();//renova parametros de fluido, tais como parametros de black oil,
        //temperatura pressao critica de gas quando deteminadas propriedades sao mudadas com grau API, densidade de gas,
        //fracao molar de CO2, RGO de separador
        void PcTcIS();//para modelos black oil, calcula a pressao e temperatura critica do gas livre nas condicoes in-situ
        double VisAgua(double temp/*temperatura em C*/) const;//calcula a viscosidade da agua
        double VisOM(double temp /*temperatura C*/) const;//calcula a viscxosidade de oleo morto
        double interpolaLas1( double var) const;//funcao de interpolacao, modelo de RS de Lasater
        double interpolaLas2( double var) const;//funcao de interpolacao, modelo de RS de Lasater
        double interpolaLas3( double var) const;//funcao de interpolacao, modelo de RS de Lasater
        double interpolaPB( double var) const;//interpola a pressao de bolha obtida da tabela PVTSim
        double interpolaRS( double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/) const;//interpola o valor de RS, scf/bpd, obtido de um modelo black oil
        double RS(double pres /*pressao kgf/cm2*/, double temp /*C*/,
        		double varPb=-1./*valor da pressao de bolha, psia, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//retorna o valor de RS, scf/bpd
        double PB(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//retona a pressao de bolha, psia
        double ViscOleo(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,int SemEmul=0) const;//retorna a viscosidade,cP,
        //do oleo saturado com a mistura de agua
        double ViscGas(double pres/*pressao kgf/cm2*/,double temp/*temperatura C*/) const;//retorna a viscosidade
        //do gas, cP
        double emul(double pres/*pressao kgf/cm2*/,double temp/*temperatura C*/) const;//retorna o multiuplicador
        //de emulsao
        double ZGopal(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//fator de compressibilidade
        //calculado pela correlacao de Gopal
        double FNdran(double presR/*pressao reduzida*/, double temp/*temperatura reduzida*/,
        		double denR/*massa especifica reduzida*/) const;//funcao auxiliar para o calculo do fator de compressibilidade
        //de Dranchuk
        double ZdranOriginal(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		int cordg=-1/*correcao de densidade de gas, caso se faca a distincao entre densidade de gas
        		dissolvido e livre*/, double=0.9, double=10, int=1) const;//funcao que calcula o fator de compressibilidade
        //por Dranchuk
        double Zdran(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/, int cordg=-1/*correcao de densidade de gas, caso se faca a distincao entre densidade de gas
        		dissolvido e livre*/, double masesp =-1) const;//metodo que retorna o valor do fator de compressibilidade, internamente ele decide
        //se Z sera obtido por Dranchuk ou interpolacao de tabela
        double DZDTOriginal(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		int cordg=-1/*correcao de densidade de gas, caso se faca a distincao entre densidade de gas
        		dissolvido e livre*/) const;//funcao que calcula a derivacada do fator de compressibilidade com temperatura
        //por Dranchuk
        double DZDT(double, double, double masesp =-1) const;//metodo que retorna o valor a derivada do fator de compressibilidade
        //com a temperatura, internamente ele decide se DZ/DT sera obtido por Dranchuk ou interpolacao de tabela
        double DZDPOriginal(double pres/*pressao kgf/cm2*/ , double temp/*temperatura C*/,
        		int cordg=-1/*correcao de densidade de gas, caso se faca a distincao entre densidade de gas
        		dissolvido e livre*/) const;//funcao que calcula a derivacada do fator de compressibilidade com pressao
        //por Dranchuk
        double DZDP(double pres/*pressao kgf/cm2*/ , double temp/*temperatura C*/, double masesp =-1) const;//metodo que retorna o valor a derivada do fator de compressibilidade
        //com a pressao, internamente ele decide se DZ/DP sera obtido por Dranchuk ou interpolacao de tabela
        void tabela(double pmax/*pressao reduzida maxima da tabela*/,
        		double tmax/*temperatura reduzida maxima da tabela*/,
                double pmin/*pressao reduzida minima da tabela*/,
				double tmin/*temperatura reduzida minima da tabela*/) const;//tabela com os valores do fator de compressibilidade, caso black oil
        void tabelaDZDP(double pmax/*pressao reduzida maxima da tabela*/,
        		double tmax/*temperatura reduzida maxima da tabela*/,
                double pmin/*pressao reduzida minima da tabela*/,
				double tmin/*temperatura reduzida minima da tabela*/) const;//tabela com os valores da derivada com a pressao do fator de compressibilidade, caso black oil
        void tabelaDZDT(double pmax/*pressao reduzida maxima da tabela*/,
        		double tmax/*temperatura reduzida maxima da tabela*/,
                double pmin/*pressao reduzida minima da tabela*/,
				double tmin/*temperatura reduzida minima da tabela*/) const;//tabela com os valores da derivada com a temperatura do fator de compressibilidade, caso black oil
        double interpolaVarProd(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/, double** Var/*tabela de propriedade obtida do PVTSim*//*,
				int renovaInd=0*/) const;//metodo
        //que interpola para P e T o valor de uma determinada propriedade obtida do arquivo PVTSim
        double BOFunc(double pres/*pressao kgf/cm2*/,double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//calcula o fator volume de formacao
        //do oleo
        double BAFunc(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//calcula o fator volume de formacao
        //da agua
        double MasEspoleo(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//calcula a massa especifica de oleo kg/m3
        double MasEspOleoComp(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//calcula a massa especifica de oleo kg/m3
        void razDegD(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/);//retorna a razao
        //da densidade de gas dissolvido pela densidade de gas nas condicoes standard
        void rzDegL(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/);//retorna a razao
        //da densidade de gas livre pela densidade de gas nas condicoes standard
        double MasEspGas(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*//*,int renovaInd=0*/) const;//calcula a
        //massa especifica do gas, kg/m3
        double MasEspGasComp(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*//*,int renovaInd=0*/) const;//calcula a
        //massa especifica do gas, kg/m3
        double DMasEspAgua(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;
        double MasEspAgua(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//calcula a
        //massa especifica de agua, kg/m3
        double MasEspLiq(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//calcula a massa especifica da mistura
        //oleo + agua, kg/m3
        double MasEspLiqComp(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//calcula a massa especifica da mistura
         //oleo + agua, kg/m3
        double MasOleo(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;// Massa de oleo((kg)) para 1 m^3\de oleo morto
        double MasOleoHidra(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;// Massa de oleo((kg)) para 1 m^3\de mistura (oleo morto+agua)
        double MasLiq(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;// Massa de Liquido((kg)) para 1 m^3\de mistura (oleo morto+agua)
        double MasAgua(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;// Massa de agua((kg)) para 1 m^3\de mistura (oleo morto+agua)
        double MasGasLivre(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//Massa de gas livre(kg) para 1 m^3 de oleo morto
        double MasGasLivreHidra(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//Massa de gas livre(kg) para 1 m^3 de mistura (oleo morto+agua)
        double FracMass(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//Titulo de gas livre (oleo +gas)
        double FracMassHidra(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/) const;//Titulo de gas livre (oleo +gas+agua)
        double FracAguaInSitu(double pres/*pressao kgf/cm2*/, double temp/*temperatura C*/,
        		double varRS=-1/*valor de RS, scf/bpd, caso ja tenha sido calculada, senao,
        		o valor negativo indicara que deve-se ser calculado no metodo*/) const;//BSW in-situ
        double interpola(Vcr<double> abs, Vcr<double> coord, double var);//nao utilizado
        double TensSuper(double pres/*pressao kgf/cm2*/,double temp/*temperatura C*/,
        		double bet=0./*fracao beta*/,
				double tensupBet=0./*tensao superficial do liquido beta*/);//tensao superficial N/m
        double TensSuperOleo(double pres/*pressao kgf/cm2*/,double temp/*temperatura C*/);//tensao superficial Oleo N/m
        double TensSuperAgua(double pres/*pressao kgf/cm2*/,double temp/*temperatura C*/);//tensao superficial Oleo N/m
        double CalorLiqOriginal(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/) const;//calor especifico do liquido, black oil, J/kgK
        double CalorGasOriginal(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/) const;//calor especifico do gas, black oil, J/kgK
        double DrholDTOriginal(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/) const;//derivada da massa especifica do liquido com a temperatura,
        //caso blackoil
        double interpolaCpl(double pres/*pressao kgf/cm2*/
        		, double temp/*temperatura C*/) const;//metodo que interpola o cp do liquido, J/kgK,
        //modelo black-oil
        double interpolaCpg(double pres/*pressao kgf/cm2*/
        		, double temp/*temperatura C*/) const;//metodo que interpola o cp do gas, J/kgK,
        //modelo black-oil
        double interpolaDrholDT(double pres/*pressao kgf/cm2*/
        		, double temp/*temperatura C*/) const;//metodo que interpola a derivada da
        //massa especifica do liquido com a temperatura a partir de uma tabela, modelo black-oil
        double CalorLiq(double pres/*pressao kgf/cm2*/
        		, double temp/*temperatura C*/) const;//calcula o calor especifico do liquido, interpolado ou
        //direto de uma correlacao, J/kgK
        double CalorGas(double pres/*pressao kgf/cm2*/
        		, double temp/*temperatura C*/) const;//calcula o calor especifico do gas, interpolado ou
        //direto de uma correlacao, J/kgK
        double drhodp(double pres/*pressao kgf/cm2*/
        		,double temp/*temperatura C*/) const;//calcula a derivada da massa especpifica de gas com a pressao
        double drhodt(double pres/*pressao kgf/cm2*/
        		,double temp/*temperatura C*/) const;//calcula a derivada da massa especpifica de gas com a temperatura
        double DrholDT(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/) const;//calcula a derivada da massa especpifica de gas com a
        //temperatura ou a partir de correlacao ou a partir de tabela
        double CalorGasVolMod(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/,double rhogini=-1.) const;//calcula a relacao modifiucada de calor especifico
        //a volume constante do gas, ver relatorio transiente, interpolado ou direto de uma correlacao, J/kgK
        double CalorGasVolMod2(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/,double rhogini=-1.) const;//calcula a relacao modifiucada de calor especifico do gas
        //a volume constante ver relatorio transiente, interpolado ou direto de uma correlacao, J/kgK;
        double CalorGasPresMod(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/,double rhogini=-1.) const;//calcula a relacao modifiucada de calor especifico do gas
        //a pressao constante, ver relatorio transiente, interpolado ou direto de uma correlacao, J/kgK
        double ConstAdG(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/,double rhogini=-1.) const;//calcula a constante adiabatica
        double CondLiq(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/)const;//calcula a condutividade do liquido, W/mK
        double CondOleo(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/)const;//calcula a condutividade do oleo, W/mK
        double CondGas(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/)const;//calcula a condutividade do gas, W/mK
        double EntalpLiq(double,double) const;
        double EntalpLiqIsoB(double,double) const;//nao utilizado
        double EntalpLiqIsoT(double,double) const;//nao utilizado
        double JTL(double pres/*pressao kgf/cm2*/,
        		double temp/*temperatura C*/) const;
        double EntalpGas(double,double) const;
        double EntalpGasIsoB(double,double) const;//nao utilizado
        double EntalpGasIsoT(double,double) const;//nao utilizado
        double JTG(double pres/*pressao kgf/cm2*/
        		,double temp/*temperatura C*/,double rhog=-1) const;
        double dRSdP(double,double) const;//nao utilizado
        double dBOSatdP(double,double) const;//nao utilizado
        double retornaR()const {return 8.0465*1000/(Deng*28.9625);}//retorna a constante do gas

        double psia(const double p)const { return (p*0.9678411)*14.69595;} //p->kgf, transforma kgf para psia
        double Faren(const double t)const { return 1.8*t + 32;} //t em Celcius, transforma Celcius para Farenheit
        double cel(const double t)const {return (t - 32)/1.8;} //t em Farenheit, transforma Farenheit para Celcius
        double kgf(const double p)const { return p/(14.69595*0.9678411);}//p em psia, transforma psia para kgf/cm2
        double ft3bbl() const {return RGO*35.31467/6.29;} //RGO em m3/m3, transforma m3/m3 para ft3/bbl
        double m3m() const {return RGO*6.29/35.31467;} //RGO em ft3/bbl, transforma ft3/bbl para m3/m3

        //funcao de atualizacao de variaveis do modelo composicional para pressao e temperatura

       void atualizaPropCompStandard(double GivenInitialBeta=-1.0, double* GivenInitialLiqComposition=NULL,
    		   double* GivenInitialVapComposition=NULL){
    	   double pres=1.03322;
    	   double temp=20.;
    	   double tempRGO=100.;
    	   /*iCalculatedStockTankThermodynamicCondition=0;
    	   dStockTankVaporMassFraction=0;
    	   dStockTankLiquidDensity=1000.;
    	   dStockTankVaporDensity=1.;
    	   iIER=0;*/
    	   int nuloL=0;
    	   if(GivenInitialLiqComposition==NULL){
    		   GivenInitialLiqComposition = new double [npseudo];
    		   for(int k=0;k<npseudo;k++)GivenInitialLiqComposition[k]=0;
    		   nuloL=1;
    	   }
    	   int nuloV=0;
    	   if(GivenInitialVapComposition==NULL){
    		   GivenInitialVapComposition = new double [npseudo];
    		   for(int k=0;k<npseudo;k++)GivenInitialVapComposition[k]=0;
    		   nuloV=1;
    	   }
     	   Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions(npseudo, fracMol, masMol, tempCrit,
     			  presCrit,fatAcent,
    			  TIndepPeneloux, kij, lij,
    			  liqModel, liqModel,liqModel, liqModel, pres,temp,
				  GivenInitialBeta, GivenInitialLiqComposition, GivenInitialVapComposition,
				  &iCalculatedStockTankThermodynamicCondition,
    			  &dStockTankVaporMassFraction,&dStockTankLiquidDensity,&dStockTankVaporDensity,
    			  &tempRGO, &iIER);

     	   if(dStockTankLiquidDensity>0.01){
     		  API=141.5/(dStockTankLiquidDensity/1000.)-131.5;
     	   }
     	   else API=50;
     	   Deng=dStockTankVaporDensity/1.225;
     	   //yco2=0.;
     	   RGO=tempRGO;
     	   if(dStockTankVaporMassFraction>0 && RGO<1e-15)RGO=1e6;
     	   IRGO=RGO*35.31467/6.29;
     	   if(nuloL==1)delete GivenInitialLiqComposition;
     	   if(nuloV==1)delete GivenInitialVapComposition;
       }

        void atualizaPropComp(double pres, double temp,double GivenInitialBeta=-1.0,
        		double* GivenInitialLiqComposition=NULL,double* GivenInitialVapComposition=NULL, int monoGas=0){
            /*double itemp=Faren(temp)+460;
            double pbtemp = kgf(pow(Bvb*IRGO/(SG100vb*pow(10,Cvb*API/itemp)),1/Avb));*/


        	double dMixtureMW=0.;
        	/*iIER_BubbleT=0;
     	    iIER_DewT=0;
     	    iIER_Flash=0;
     	    iIER_BubbleP=0;
     	    iIER=0;
     	    iCalculatedThermodynamicCondition=0;
     	    dCalculatedBubbleT=0.;
     	    dCalculatedDewT=0.;
     	    dCalculatedBubbleP=50.;
     	    dLiquidPhaseMW=0.;
     	    dVaporPhaseMW=0.;
     	    dVaporMassFraction=0.;*/

        	int nuloL=0;
        	if(GivenInitialLiqComposition==NULL){
        		GivenInitialLiqComposition = new double [npseudo];
        		for(int k=0;k<npseudo;k++)GivenInitialLiqComposition[k]=0;
        		nuloL=1;
        	}
        	int nuloV=0;
        	if(GivenInitialVapComposition==NULL){
        		GivenInitialVapComposition = new double [npseudo];
        		for(int k=0;k<npseudo;k++)GivenInitialVapComposition[k]=0;
        		nuloV=1;
        	}

        	for(int i=0; i<npseudo;i++){
        		dMixtureMW+=fracMol[i]*masMol[i];
        	}
        	if(monoGas==1 /*|| dMixtureMW<=(30.+1e-15)*/){
        		iCalculatedThermodynamicCondition=4;
        		dVaporMassFraction=1.;
        		for(int i=0; i<npseudo;i++){
        			oCalculatedVapComposition[i]=fracMol[i];
        			oCalculatedLiqComposition[i]=0.;
        			dCalculatedBeta=1.;
        			iIER_Flash=0;
        			iIER=0;
        			dVaporPhaseMW=dMixtureMW;

        		}
        	}
        	else{
        		dCalculatedBubbleP=50.;

        		Marlim_CalculateMixtureThermodynamicCondition(pres, temp, npseudo,
        				masMol,fracMol, tempCrit,presCrit, fatAcent, TIndepPeneloux, kij, lij, liqModel,
						liqModel,GivenInitialBeta, GivenInitialLiqComposition, GivenInitialVapComposition,
						&dCalculatedBubbleT, &iIER_BubbleT, &dCalculatedDewT, &iIER_DewT,
						&dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, &iIER_Flash,
						&dCalculatedBubbleP,&iIER_BubbleP, &iCalculatedThermodynamicCondition, &dLiquidPhaseMW,
						&dVaporPhaseMW,&dVaporMassFraction, &iIER);
        	}
        	if(monoGas==0 && dMixtureMW<=(30.+1e-15) && iIER>0){
        		iCalculatedThermodynamicCondition=4;
        		dVaporMassFraction=1.;
        		for(int i=0; i<npseudo;i++){
        			oCalculatedVapComposition[i]=fracMol[i];
        			oCalculatedLiqComposition[i]=0.;
        			dCalculatedBeta=1.;
        			iIER_Flash=0;
        			iIER=0;
        			dVaporPhaseMW=dMixtureMW;

        		}
        	}
        	if(nuloL==1)delete GivenInitialLiqComposition;
        	if(nuloV==1)delete GivenInitialVapComposition;

        }

        void atualizaPropParafina(double pres, double temp){

        	Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation(pres, temp, npseudoWax/*numero de componentes*/,
        			iPresWax,
        			iTempWax,oPressurePoints, oCloudPointTemperatures, oStructurePressures,
        			oStructureTemperatures, oStructureDensities, oStructureLiqMW, oStructureCPWax,
        			oStructureThermCond, oStructureWaxConcs,
					oMolecularWeightsOfWaxComponents/*massa molecular-parafina*/,
        			oLiquidDensitiesOfWaxComponents/*massa específica liq de cada componente*/,
					&iIERW, &iWaxComponentCountOutput, oMolecularWeightsOfWaxComponentsOut,
        			oLiquidDensitiesOfWaxComponentsOutput, &dCloudPointTOutput,
					&dInterpolatedCPWaxOutput/*calor especifico a p & T da parafina*/,
        			&dInterpolatedThermCondOutput/*condutividade da parafina*/,
					&dInterpolatedDensOutput/*densidade da parafina*/,
					&dInterpolatedLiqMWOutput/*massa molecular da fase l[iquida*/,
        	        oInterpolatedWaxConcsTDerivOutput/*curva de solubilidade*/,
					oInterpolatedWaxConcs);
        }

};

ostream& operator<<(ostream& s, const ProFlu&);//saida de valores de propriedade de fluidos


#endif

