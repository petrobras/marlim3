/*
 * Leitura.h
 *
 *  Created on: 18/03/2015
 *      Author: bwz1
 */

#ifndef LEITURA_H_
#define LEITURA_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility> // para std::pair
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace std;

#include "Acidentes2.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Geometria.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Vetor.h"
#include "Matriz.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "acessorios.h"
#include "celula3.h"
#include "TrocaCalor.h"
#include "dados1Poisson.h"
#include "solverPoisson.h"
#include "chokegas.h"
#include "celulaGas.h"
#include "Log.h"
//////////////////////////////////////////
#include "PorosoRad-Simples.h"
#include "solverPoroso.h"
///////////////////////////////////////
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/pt_BR.h"
#include "JSON_entrada.h"
#include "MarlimComposicional.h"
#include "variaveisGlobais1D.h"
#include "criterioIntermiSevera.h"

//#define VERSAO_SCHEMA_JSON "1.3.9"
//#define ARQUIVO_SCHEMA_JSON "schema_1_3_9.json"

#ifdef linux
  // barra para linux
  #define BARRA "/"
#elif LINUX
  // barra para linux
  #define BARRA "/"
#elif Linux
  // barra para linux
  #define BARRA "/"
#elif UNIX
  // barra para linux
  #define BARRA "/"
#elif defined WIN32
  // barra para windows
  #define BARRA "\\"
#elif defined Win32
  // barra para windows
  #define BARRA "\\"
#elif defined win32
  // barra para windows
  #define BARRA "\\"
#else
  // barra para outros
  #define BARRA "/\\"
#endif

using namespace rapidjson;
//extern double** zdranP;
//extern double** zdranG;
//extern double** dzdpP;
//extern double** dzdpG;
//extern double** dzdtP;
//extern double** dzdtG;
//extern double** cpg;
//extern double** cpl;
//extern double** drholdT;
//extern int npontos;
//extern int ndiv;
/*extern int contador;//variavel global, contador de passos de tempo, o mesmo que o atributo kSP;
extern int chaverede;//variavel global, indica se se esta simulando um tramo solteiro ou pertencente a uma malha de dutos;
extern int chaveAnelGL;//variavel global, indica se o tramo faz parte de um anel de GL;
extern double CritDTMin;
extern int chaverede;
extern int chaveredeT;
extern int simulaTransiente;

extern int tipoFluidoRedeGlob;

extern int sequenciaAS;*/
//extern double** RhoInj;
//extern double** ViscInj;
//extern double** CondInj;
//extern double** CpInj;
//extern double** DrhoDtInj;

// relatorio dos arquivos de dados de saida da simulacao
extern ofstream arqRelatorioPerfis;

// criar objeto para log da aplicacao
extern Logger logger;

// obter string do path dos arquivos de entrada para o simulador
extern string pathArqExtEntrada;

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;

extern int nthrdMatriz;

extern string versao;


typedef enum { none, json, schema, rules, off } tipoValidacaoJson_t;
typedef enum { transiente, poco_injetor, rede, convecNatural } tipoSimulacao_t;
//typedef enum { plataforma, poco } origemGeometria_t;
typedef enum { jusante, montante } origemGeometria_t;
typedef enum { multifasico, injetor, oleoduto } sistemaSimulacao_t;

//struct relacionada com a informacao de incremento de tempo alimanteda no Json;
struct detTMAX{
	int parserie;//comprimento da serie temporal;
	double* tempo;//vetor os valores de tempo onde ocorrem as mudancas de valores, segundos;
	double* valor;//vetor com os incrementos de tempo maximo, segundos;
};

struct detRGO{
	int parserie;
	double* tempo;
	double* valor;
};

//struct relacionada com o detalhamento do material na parede da tubulacao;
struct material {
    int id;
    double cond;//condutividade, W/mK;
    double cp;//calor especifico J/kgK;
    double rho;//massa especifica kg/m3;
    int tipo;//tipo de material, 0-> solido, 1-> fluido (usuario), 2-> agua, 3->ar;
    double visc;//viscosidade, cP, apenas para tipo 1;
    double beta;//expansividade termica, apenas para tipo 1;
};

//struct com o detalhamento da secao transversal do duto;
struct corteduto {
    int id;
    int ncam;//numero de camadas de material;
    int anul;//se regiao de escoamento e ou nao anular
    double a;//diametro interno, se anular, menor diametro do anular
    double b;//para o caso anular, maior diametro do anular
    double rug;//rugosidade absoluta, metros;
    double* diam;//vetor com o maior diemtro de cada camada de material;
    int* indmat;//vetor com o indicador do material, relaciona-se com a struct material
    int* discre;//discretizacao de cada camada de material
};

//struct com informacoes genericas de uma unidade de escoamento;
struct detduto{
    double ang;//angulo da tubulacao em radiano
    int indcorte;//inidce da secao transversal da unidade, relaciona-se com corteduto
    int servico;//indica se e linha de producao ou servico
};

//struct com as informacoes completas de uma unidade de escoamento, linha de producao
struct detalhaP{
	int ind;
	int duto;//indice do duto, relaciona-se com detduto
	int ncel;//numero de discretizacoes da unidade
	double comp;//comprimento da unidade
	double Lini;//posicao, tomando como referencia o fundo do poco, onde se inicia a unidade
	int dirconv;//indicador de como e a conveccao no ambiente externo, se atacando o duto, zero, se paralelo ao duto, 1
	int acopcol;//indica se esta unidade tem acoplamento termico com uma unidade da linha de servico
	int acopcolRede;
	int formacDuto;//indica que o ambiente externo ao duto e uma formacao
    int lito;//indica a litologia da formacao, relaciona-se com a struct detFormacao
	int ambext;//indica o tipo de ambiente externo -> 0 ambiente informadao pelo usuario, 1-> maritimo, 2-> aereo
	int nponts;//numero de pontos em que se e informado dados de condicoes ambiente e de condicoes iniciais, se for o caso
	double* dxVar;//posicoes relativas em que se informa valores de ambiente eterno e condicoes iniciais
	double** var;//matriz com as informacoes de condicoes ambientes e condicoes iniciais informadas
	double dPdLHidro;
	double dPdLFric;
	double dTdL;
	int inddPdLHidro;
	int inddPdLFric;
	int inddTdL;

	int difusTerm2D;
	string difusTerm2DJSON;

	//vector<int> difusTerm3D;
	//vector<string> difusTerm3DAcop;

	int* difusTerm3D;
	double* difusTerm3DFE;
	string* difusTerm3DAcop;

	//pelo usuario
	//var[0][ind] temp;
	//var[1][ind] pres;
	//var[2][ind] hol;
	//var[3][ind] bet;
	//var[4][ind] uls;
	//var[5][ind] ugs;
	//var[6][ind] textern;
	//var[7][ind] vextern;
	//var[8][ind] kextern;
	//var[9][ind] cpextern;
	//var[10][ind] rhoextern;
	//var[11][ind] viscextern;
	int agrupaDiscre;//indica se a leitura da discretizacao será feita por agrupamentos de discretizacao
	//ous se cada tamanho de celula sera definida no sistema
	double *dx;//vetor com o comprimento de cada discretizacao
    //vector<pair<int,double> > dx;
	int correlacaoMR2;
};

//struct com o detalhamento de uma celula, nao entra no parse com o json, e feito apos o parse com a unidade de producao
struct detcelp{
    double dx;//comprimento da celula
    int duto;//indice do duto, relaciona-se com detduto
    double temp;//temperatura da condicao inicial, caso o permanente nao seja calculado
    double pres;//pressao da condicao inicial, caso o permanente nao seja calculado
    double hol;//holdup da condicao inicial, caso o permanente nao seja calculado
    double bet;//beta da condicao inicial, caso o permanente nao seja calculado
    double uls;//velocidade superficial de liquido da condicao inicial, caso o permanente nao seja calculado
    double ugs;//velocidade superficial de gas da condicao inicial, caso o permanente nao seja calculado
    double textern;//temperatura ambiente
    double vextern;//velocidade da corrente externa, caso nao seja formacao ou acoplamento com linha de servico
    double kextern;//condutividade ambeinet, caso nao seja formacao, acoplamento com linha de servico,
    //ou ambiente maritimo ou aereo
    double cpextern;//calor especifico ambeinet, caso nao seja formacao, acoplamento com linha de servico,
    //ou ambiente maritimo ou aereo
    double rhoextern;//massa especifica ambeinet, caso nao seja formacao, acoplamento com linha de servico,
    //ou ambiente maritimo ou aereo
    double viscextern;//viscosidade ambeinet, caso nao seja formacao, acoplamento com linha de servico,
    //ou ambiente maritimo ou aereo
    int ambext;//indica o tipo de ambiente externo -> 0 ambiente informadao pelo usuario, 1-> maritimo, 2-> aereo
    int dirconv;//indicador de como e a conveccao no ambiente externo, se atacando o duto, zero, se paralelo ao duto, 1
    int acopcol;//indica se esta unidade tem acoplamento termico com uma unidade da linha de servico
    int acopcolRede;
    int formacCel;//indica que o ambiente externo ao duto e uma formacao
    int lito;//indica a litologia da formacao, relaciona-se com a struct detFormacao
    double profundiM;//profundidade no centro da celula, tomando com referencia a plataforma,
    //usado para um eventual calculo hidrostatico, caso alguma camada de duto seja ar;
    double profundiF;//profundidade da fronteira da celula, tomando com referencia a plataforma,
    //usado para um eventual calculo hidrostatico, caso alguma camada de duto seja ar;
	double dPdLHidro;//multiplicador de variacao de pressao por hidrostatica que sera utilizado,default=1
	double dPdLFric;//multiplicador de variacao de pressao por friccao que  sera utilizado,default=1
	double dTdL;//multiplicador de variacao de temperatura que sera utilizado,default=1
	int inddPdLHidro;//indice que indica qual multiplicador de variacao de pressao por hidrostatica sera utilizado
	int inddPdLFric;//indice que indica qual multiplicador de variacao de pressao por friccao sera utilizado
	int inddTdL;//indice que indica qual multiplicador de variacao de temperaturasera utilizado

	int difusTerm2D;
	string difusTerm2DJSON;

	int difusTerm3D;
	double difusTerm3DFE;
	string difusTerm3DAcop;

	int correlacaoMR2;
};

//struct com as informacoes completas de uma unidade de escoamento, lnha de servico
struct detalhaG{
	int ind;
	int duto;//indice do duto, relaciona-se com detduto
	int ncel;//numero de discretizacoes da unidade
	double comp;//comprimento da unidade
	double Lini;//posicao, tomando como referencia o fundo do poco, onde se inicia a unidade
	int acopcol;//indica se esta unidade tem acoplamento termico com uma unidade da linha de producao
	int formacDuto;//indica que o ambiente externo ao duto e uma formacao
    int lito;//indica a litologia da formacao, relaciona-se com a struct detFormacao
	int ambext;//indica o tipo de ambiente externo -> 0 ambiente informadao pelo usuario, 1-> maritimo, 2-> aereo
	int nponts;//numero de pontos em que se e informado dados de condicoes ambiente e de condicoes iniciais, se for o caso
	double* dxVar;//posicoes relativas em que se informa valores de ambiente eterno e condicoes iniciais
	double** var;//matriz com as informacoes de condicoes ambientes e condicoes iniciais informadas
	//var[0][ind] temp;
	//var[1][ind] pres;
	//var[2][ind] mg;
	//var[3][ind] textern;
	//var[4][ind] vextern;
	//var[5][ind] kextern;
	//var[6][ind] cpextern;
	//var[7][ind] rhoextern;
	//var[8][ind] viscextern;
	int agrupaDiscre;//indica se a leitura da discretizacao será feita por agrupamentos de discretizacao
	//ous se cada tamanho de celula sera definida no sistema
	double *dx;//vetor com o comprimento de cada discretizacao
    //vector<pair<int,double> > dx;
	double dPdLHidro;
	double dPdLFric;
	double dTdL;
	int inddPdLHidro;
	int inddPdLFric;
	int inddTdL;
};

//struct com o detalhamento de uma celula, nao entra no parse com o json, e feito apos o parse com a linha de servico
struct detcelg{
    double dx;//comprimento da celula
    int duto;//indice do duto, relaciona-se com detduto
    double temp;//temperatura da condicao inicial, caso o permanente nao seja calculado
    double pres;//pressao da condicao inicial, caso o permanente nao seja calculado
    double mg;//vazao massica da condicao inicial, caso o permanente nao seja calculado
    double textern;//temperatura ambiente
    double vextern;//velocidade da corrente externa, caso nao seja formacao ou acoplamento com linha de producao
    double kextern;//condutividade ambeinet, caso nao seja formacao, acoplamento com linha de producao,
    //ou ambiente maritimo ou aereo
    double cpextern;//calor especifico ambeinet, caso nao seja formacao, acoplamento com linha de producao,
    //ou ambiente maritimo ou aereo
    double rhoextern;//massa especifica ambeinet, caso nao seja formacao, acoplamento com linha de producao,
    //ou ambiente maritimo ou aereo
    double viscextern;//viscosidade ambeinet, caso nao seja formacao, acoplamento com linha de producao,
    //ou ambiente maritimo ou aereo
    int acopcol;//indica se esta unidade tem acoplamento termico com uma unidade da linha de producao
    int formacCel;//indica que o ambiente externo ao duto e uma formacao
    int ambext;//indica o tipo de ambiente externo -> 0 ambiente informadao pelo usuario, 1-> maritimo, 2-> aereo
    int lito;//indica a litologia da formacao, relaciona-se com a struct detFormacao
    double profundiM;//profundidade no centro da celula, tomando com referencia a plataforma,
    //usado para um eventual calculo hidrostatico, caso alguma camada de duto seja ar;
    double profundiF;//profundidade da fronteira da celula, tomando com referencia a plataforma,
    //usado para um eventual calculo hidrostatico, caso alguma camada de duto seja ar;
	double dPdLHidro;
	double dPdLFric;
	double dTdL;
	int inddPdLHidro;
	int inddPdLFric;
	int inddTdL;
};

//struct com o detalhamento de IPR para fazer o parse com Json
struct detIPR{
	int indcel;//indice da celula
	double comp;//posicao em metros onde a fonte IPR se localiza, referencia fundo de poco
	int tipoIPR;//modelo de IPR: 0-> IPR linear; 1->Vogel combinada; 2-> Vogel
	int seriep;//numero de elementos da se�riede tempo de pressao estatica
	double* pres;//serie de pressoes
	double* tpres;//tempos relacionados com as pressoes da serie
	int seriet;//numero de elementos da se�riede tempo de temperatura de resrvatorio
	double* temp;//serie de temperatura
	double* ttemp;//tempos relacionados com as temperaturas da serie
	int serieip;//numero de elementos da se�riede tempo de IP de resrvatorio
	double* ip;//serie de IPs stdm3/d/kgf/cm2 (nao valido para Vogel puro, tipo 2)
	double* tip;//tempos relacionados com os IPs da serie
	int seriejp;//numero de elementos da seriede tempo de IJ de resrvatorio
	double* jp;//serie de JPs stdm3/d/kgf/cm2
	double* tjp;//tempos relacionados com os JPs da serie
	int serieqm;//numero de elementos da seriede tempo de vazao maxima (vogel puro) de resrvatorio
	double* qMax;//serie de vazoes maximas stdm3/d
	double* tqMax;//tempos relacionados com as vazoes maximas da serie
	int indfluP;//indicador de qual fluido de producao do vetor de fluidos de producao sera utilizado na IPR
};

//struct com as definicoes da injecao de gas-lift na linha de servico
struct detGASINJ{
	int tipoCC;//tipo de condicao de contorno, 0->pressao de injecao, 1-> vazao de injecao
	int parserie;//numero de elementpos da serie temporal de vazao ou pressao de injecao
	int chuteVaz; //para o  calculo permanente para o caso de pressao de injecao, opcional,
	//cghute de vazao de injecao para facilitar a conevrgencia
	double* temperatura;//vetor com as temperaturas de injecao da serie de tempo
	double* presinj;//vetor com as pressoes de injecao da serie temporal, se for este o caso
	double* vazgas;//vetor com as vazoes de injecao da serie temporal, se for este o caso
	double* tempo;//vetor com os tempos da serie de tempo
};

//struct com as informacoes da condicao de contorno de pressao na entrada do duto
struct detPresEnt{//alteracao4
	int parserie;//nuymeros de elementos da serie temporal
	int indFluido;
	double* temperatura;//vetor com a temperatura de entrada no duto para a serie de tempo
	double* pres;//vetor com a pressao de entrada no duto para a serie de tempo
	double* tit;//vetor com o titulo de entrada no duto para a serie de tempo
	double* bet;//vetor com obeta de entrada no duto para a serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
};//alteracao4

//struct com as informacoes da condicao de contorno de pressao e vazao na entrada do duto
struct detVazPresEnt{//alteracao4
	int parserie;//nuymeros de elementos da serie temporal
	double* temperatura;//vetor com a temperatura de entrada no duto para a serie de tempo
	double* pres;//vetor com a pressao de entrada no duto para a serie de tempo
	double* mass;//vetor com a vazao massica kg/s de entrada no duto para a serie de tempo
	double* bet;//vetor com obeta de entrada no duto para a serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
};//alteracao4

//struct com as informacoes da valvula de gas-lift
struct detVALVGL{
  int posicP;//indice da celula na linha de producao onde a VGL se posiciona
  int posicG;//indice da celula da linha de servico onde a VGL se posiciona
  double compP;//comprimento na linha de producao onde a VGL se posiciona, referencia fundo do poco
  double compG;//comprimento na linha de servico onde a VGL se posiciona, referencia plataforma
  int tipo;//tipo de VGL, 0-> orifpicio, 1-> pressao, 2-> Venturi
  double diagarg;//diametro do orificio metros
  double frec;//nao utilizado
  double frecLiq;//nao utilizado
  double pcali;//pressao de calibracao PSI
  double tcali;//temperatura de calibracao F
  double diaexter;//diametro da porta metros
  double cd;//coeficiente de de descarga
  double cdLiq;//coeficiente de descarga, caso esteja escoando liquido, caso de descarga de GL
  double razarea;//razao de area, valvula de pressao
};

//struct com o detalhamento de fonte de gas
struct detFONGAS{
	int posicP;//indice da celula onde a fointe se encontra no sistema de producao
	double comp;//comprimento onde a fointe se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	int seco;
	int indfluP;
	double* temp;//vetor com a temperatura da fonte
	double* vazgas;//vetor com a vazao da fonte, sm3/d
	double* vazcomp;//vetor com a vazao da fonte, sm3/d
	double* tempo;//vetor com os tempos da serie de tempo
};

//struct com o detalhamento de fonte meio poroso radial
struct detPoroRad{
	int posicP;//indice da celula onde a fointe se encontra no sistema de producao
	double comp;//comprimento onde a fointe se encontra no sistema de producao, referencia->fundo de poco
	string nomeArquivoEntrada;
};

//struct com o detalhamento de fonte meio poroso 2D
struct detPoro2D{
	int posicP;//indice da celula onde a fointe se encontra no sistema de producao
	double comp;//comprimento onde a fointe se encontra no sistema de producao, referencia->fundo de poco
	string nomeArquivoEntrada;
};

//struct detalhamento CV
struct detCV{
	double x1;
	double x2;
	double cv1;
	double cv2;
};

//struct com o detalhamento de valvula
struct detValv{
	int posicP;//indice da celula onde a valvula se encontra no sistema de producao
	double comp;//comprimento onde a valvula se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	double* abertura;//vetor com a abertura da valvula (razao com a area da tubulacao)
	double* tempo;//vetor com os tempos da serie de tempo
	double cd;//cd da valvula
	int curvaCV;
	int curvaDinamic;
	int ncv;
	double xini;
	detCV* cvCurv;
	/*~detValv(){
		if(ncv>0)delete [] cvCurv;
		if(parserie>0){
		  delete [] abertura;
		  delete [] tempo;
		}
	}*/
};

//struct com o detalhamento de fonte de liquido
struct detFONLIQ{
	int posicP;//indice da celula onde a fointe se encontra no sistema de producao
	double comp;//comprimento onde a fointe se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	double* temp;//vetor com a temperatura da fonte
	double* bet;//vetor com o valor beta da fonte
	double* vazliq;//vetor com a vazao de liquido da fonte, sm3/d
	double* tempo;//vetor com os tempos da serie de tempo
	int indfluP;//indicador de qual fluido de producao do vetor de fluidos de producao sera utilizado na fonte
};

//struct com o detalhamento de fonte de massa
struct detFONMASS{
	int posicP;//indice da celula onde a fointe se encontra no sistema de producao
	double comp;//comprimento onde a fointe se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	int condiTermo;//se 0 equilíbrio termodinamico
	double* temp;//vetor com a temperatura da fonte
	double* vazMasT;//vetor com a vazao total da fonte, kg/s
	double* vazMasC;//vetor com a vazao de liquido compĺementar da fonte, kg/s
	double* vazMasG;//vetor com a vazao de gás da fonte, kg/s
	double* tempo;//vetor com os tempos da serie de tempo
	int indfluP;//indicador de qual fluido de producao do vetor de fluidos de producao sera utilizado na fonte
};

//struct com o detalhamento de fonte que ocorre devido a uma derivacao do tipo vazamento
struct detFURO{
	int posicP;//indice da celula onde a fointe se encontra no sistema de producao
	double comp;//comprimento onde a fointe se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	int TipoAbertura;//indica se o tamanho do furo sera dado pelo seu diametro (tipo=1)
	//ou pela razao entre a area do furo e da tubulacao tipo=0
	double pres;//pressao extrerna a tubulacao
	double temp;//temperatura externa a tubulacao
	double beta;//beta externa a tubulacao
	double titAmb;
	double cd;//coeficiente de descarga do furo
	double* abertura;//vetor com os valores da abertura do furo para a serie temporal
	double* tempo;//vetor com os tempos da serie de tempo
	int tipoFlu;//se 1-> fluido de producao utilizado sera o primeiro, senao,
	//pergunta-se qual o indice do fluido de producao
	int indFlu;//indice do fluido de producao externo a tubulacao
	int* check;
	double* tempoChk;
	int parserieChk;
	int ambGas;
};

//struct com o detalhamento da BCS
struct detBCS{
	int posicP;//indice da celula onde a BCS se encontra no sistema de producao
	double comp;//comprimento onde a BCS se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
	double* freq;//vetor com as frequencias, Hertz, da serie de tempo da BCS
	int ncurva;//numero de pontos do detalhamento da BCS
	double* vaz;//vetor com os valores da vazao da curva de BCS, BPD
	double* head;//vetor com os valores de head da curva de BCS, ft
	double* power;//vetor com os valores de potencia da curva de BCS, hp
	double* efic;//vetor com os valores de eficiencia da curva de BCS
	double freqref;//frequencia de referencia em que a curva foi levantada
	int nestag;//numero de estagios da bomba
	double eficM;//eficiecia do motor
	double freqMinima;//frequencia mpinima de operacao da BCS
	int correcHI;
};

//struct com o detalhamento da Bomba Volumetrica
struct detBVOL{
	int posicP;//indice da celula onde a bomba se encontra no sistema de producao
	double comp;//comprimento onde a bomba se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
	double* freq;//vetor com as frequencias, Hertz, da serie de tempo da bomba
    double capacidade;//capacidade volumetrica da bomba, m3
    double npoli;//fator politropico
};

//struct com o detalhamento de incremento de pressao localizado
struct detDPREQ{
	int posicP;//indice da celula onde o incremento de pressao se encontra no sistema de producao
	double comp;//comprimento onde o incremento de pressao se encontra no sistema de producao, referencia->fundo de poco
	int parserie;//numero de elementos da serie de tempo
	double* dp;//vetor com os deltas pressao, kgf/cm2, da serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
	//alteracao 5/02/2021
	int tipoCompGas;//indica qual sera o processo de compressao do gas, se 0->adiabatico,
	//se 1->politrópico, 2->isotérmico
	double fatPoli;//caso tipoCompGas==1 fatPoli sera o fator politropico, senao, constante adiabatica
	double eficLiq;//eficiencia de bombeio da fase liquida, se não for informada, admite-se 100%
	double eficGas;//eficiencia de compressao da fase gas, se não for informada, admite-se 100%
};

//struct com o detalhamento da Master1
struct detMASTER1{
	int parserie;//numero de elementos da serie de tempo
	int posic;//indice da celula onde a master1 se encontra no sistema de producao
	double comp;//comprimento onde a master1 se encontra no sistema de producao, referencia->fundo de poco
	double* abertura;//vetor com as aberturas relativas da Master1, em relacao a area da tubulacao, da serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
	double razareaativ;//valor da area relativa onde comeca a se entender a master1 atuando como um choke
	int curvaCV;
	int ncv;
	detCV* cvCurv;
	/*~detMASTER1(){
		if(ncv>0)delete [] cvCurv;
		if(parserie>0){
		  delete [] abertura;
		  delete [] tempo;
		}
	}*/
};

//struct com o detalhamento da pressao a jusante da linha, pressao do separador
struct detPSEP{
	int parserie;//numero de elementos da serie de tempo
	double* pres;//vetor com os valores de pressao do separador/jusante, kgf/cm2, da serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
};

struct detCorrec{
	int parserieHidro;
	int parserieFric;
	int parserieDT;
	double* dPdLHidro;
	double* dPdLFric;
	double* dTdL;
};

//struct com o detalhamento do choke de superficie
struct detCHOKESUP{
	int parserie;//numero de elementos da serie de tempo
	double cd;//coeficiente de de descarga
	int modelo;//modelo do choke multifasico, 0-> sachdeva
	double* abertura;//vetor com as aberturas relativas do choke, em relacao a area da tubulacao, da serie de tempo
	double* tempo;//vetor com os tempos da serie de tempo
	int curvaCV;
	int curvaDinamic;
	int ncv;
	double xini;
	detCV* cvCurv;
	/*~detCHOKESUP(){
		if(ncv>0)delete [] cvCurv;
		if(parserie>0){
		  delete [] abertura;
		  delete [] tempo;
		}
	}*/
};

//struct com o detalhamento do pig
struct detPig{
  int lanca;//indice da celula onde a pig e lancado no sistema de producao
  int receb;//indice da celula onde a pig e recebido no sistema de producao
  double razarea;//nao usado
  double delpres;//nao usado
  double cdPig;//nao usado
  double compL;//comprimento onde o pig e lancado no sistema de producao, referencia->fundo de poco
  double compR;//comprimento onde o pig e recebido no sistema de producao, referencia->fundo de poco
  double tempo;//momento em que o pig e lancado
};

//struct com informacoes sobre testes de intermitencia severa;
struct detIntermi{
	int indInicioTrechoAcumula;
	int indFimTrechoAcumula;
	int indFimTrechoColuna;
	int criterio;
	double fracaoVazioPenetracao;
};

//struct com a informacao referentes a gravacao de perfis na linha de producao
struct detPROFP{
	int n;//numero de perfis que serao gravados
	int pres;//indicador de que se deseja gvar o perfil de pressao
	int temp;//indicador de que se deseja gvar o perfil de temperatura
	int hol;//indicador de que se deseja gvar o perfil de holdup
	int FVH;//indicador de que se deseja gvar o perfil de Fração Volumétrica de Hidrato (FVH) - solver de Hidratos - chris
	int bet;//indicador de que se deseja gvar o perfil de beta
	int ugs;//indicador de que se deseja gvar o perfil de velocidade superficial de gas
	int uls;//indicador de que se deseja gvar o perfil de velocidade superficial de liquido
	int ug;//indicador de que se deseja gvar o perfil de velocidade media de gas
	int ul;//indicador de que se deseja gvar o perfil de velocidade media de liquido
	int arra;//indicador de que se deseja gvar o perfil de arranjo
	int yco2;//indicador de que se deseja gvar o perfil de fracao de CO2
	int viscl;//indicador de que se deseja gvar o perfil de viscosidADE DE liquido
	int viscg;//indicador de que se deseja gvar o perfil de viscosidade de gas
	int rhog;//indicador de que se deseja gvar o perfil de massa especifica de gas
	int rhol;//indicador de que se deseja gvar o perfil de massa especifica de liquido
	int rhoo;//indicador de que se deseja gvar o perfil de massa especifica do oleo
	int rhoa;//indicador de que se deseja gvar o perfil de massa especifica da agua
	int rhoMix;
	int RS;
	int masg;//indicador de que se deseja gvar o perfil de vazao massica de gas
	int masl;//indicador de que se deseja gvar o perfil de vazao massica de liquido
	int qlst;////indicador de que se deseja gvar o perfil de vazao de fracao pesada standard
	int qlwst;//indicador de que se deseja gvar o perfil de fracao pesada + agua standard
	int qgst;//indicador de que se deseja gvar o perfil de vazao de leve standard
	int qlstTot;//indicador de que se deseja gvar o perfil de vazao total de liquido standard
	int c0;//indicador de que se deseja gvar o perfil de parametro de distribuicao
	int ud;//indicador de que se deseja gvar o perfil de velocidade de escorregamento
	int RGO;//indicador de que se deseja gvar o perfil de RGO local
	int deng;//indicador de que se deseja gvar o perfil de desnsidade de gas
	int calor;//indicador de que se deseja gvar o perfil de fluxo de calor
	int masstrans;//indicador de que se deseja gvar o perfil de transferencia de massa
	int cpg;//indicador de que se deseja gvar o perfil de calor especifico de gas
	int cpl;//indicador de que se deseja gvar o perfil de calor especifico de liquido
	int api;//indicador de que se deseja gvar o perfil de API
	int bsw;//indicador de que se deseja gvar o perfil de BSW
	int hidro;//indicador de que se deseja gvar o perfil de termo hidrostatico
	int fric;//indicador de que se deseja gvar o perfil de termo de friccao
	int Term1;
	int Term2;
	int dengD;////indicador de que se deseja gvar o perfil de densidade de gas dissolvido
	int dengL;//indicador de que se deseja gvar o perfil de densidade de gas livre

	int reyi;//indicador de que se deseja gvar o perfil de Reynolds interno
	int reye;//indicador de que se deseja gvar o perfil de Reynolds ambiente
	int Fr;//indicador de que se deseja gvar o perfil de Froude
	int grashi;//indicador de que se deseja gvar o perfil de Grashof interno
	int grashe;//indicador de que se deseja gvar o perfil de Grashof externo
	int nusi;//indicador de que se deseja gvar o perfil de Nusselt interno
	int nuse;//indicador de que se deseja gvar o perfil de Nussel externo
	int hi;//indicador de que se deseja gvar o perfil de coefeiciente de pelicula interno
	int he;//indicador de que se deseja gvar o perfil de coeficiente de pelicula externo
	int pri;//indicador de que se deseja gvar o perfil de prandtl interno
	int pre;//indicador de que se deseja gvar o perfil de Prandtl externo

	int Rs;//indicador de que se deseja gvar o perfil de RS
	int Bo;//indicador de que se deseja gvar o perfil de Bo

	int tAmb;//indicador de temperatura ambiente
	int PrG;//prandtl de gas, escoamento interno
	int PrL;//prandtl Liquido. escoamento interno

	int pseudoL;
	int pseudoG;
	int pseudoM;

	int TResi;
	int redAtrito;

	int angulo;
	int diamInt;
	int TempParede;

	int dadosParafina;

	int subResfria;

	double* tempo;//vetor com os tempos em que serao gravados os perfis
};

//struct com a informacao referentes a gravacao de perfis na linha de servico
struct detPROFG{
	int n;//numero de perfis que serao gravados
	int pres;//indicador de que se deseja gvar o perfil de pressao
	int temp;//indicador de que se deseja gvar o perfil de temperatura
	int ugs;//indicador de que se deseja gvar o perfil de velocidade media de gas
	int ug;//indicador de que se deseja gvar o perfil de velocidade media de gas
	int tens;//indicador de que se deseja gvar o perfil de tensao de cisalhamento
	int viscg;//indicador de que se deseja gvar o perfil de viscosidade de gas
	int rhog;//indicador de que se deseja gvar o perfil de massa especifica de gas
	int masg;//indicador de que se deseja gvar o perfil de vazao massica de gas
	int masl;//indicador de que se deseja gvar o perfil de vazao massica de liquido
	int qgst;//indicador de que se deseja gvar o perfil de vazao de gas standard
	int hidro;//indicador de que se deseja gvar o perfil de termo hidrostatico
	int fric;//indicador de que se deseja gvar o perfil de termo de friccao
	int FVHG;//indicador de que se deseja gvar o perfil de Fração Volumétrica de Hidrato na linha de Servico - chris - Hidratos
	int calor;//indicador de que se deseja gvar o perfil de fluxo de calor

	int reyi;//indicador de que se deseja gvar o perfil de Reynolds interno
	int reye;//indicador de que se deseja gvar o perfil de Reynolds ambiente
	int grashi;//indicador de que se deseja gvar o perfil de Grashof interno
	int grashe;//indicador de que se deseja gvar o perfil de Grashof externo
	int nusi;//indicador de que se deseja gvar o perfil de Nusselt interno
	int nuse;//indicador de que se deseja gvar o perfil de Nussel externo
	int hi;//indicador de que se deseja gvar o perfil de coefeiciente de pelicula interno
	int he;//indicador de que se deseja gvar o perfil de coeficiente de pelicula externo
	int pri;//indicador de que se deseja gvar o perfil de prandtl interno
	int pre;//indicador de que se deseja gvar o perfil de Prandtl externo
	int tAmb;//indicador de temperatura ambiente

	int angulo;
	int diamInt;
	int TempParede;

	int subResfria;

	double* tempo;//vetor com os tempos em que serao gravados os perfis
};

//struct com a informacao referentes a gravacao de perfis termicos na secao transversal na linha de producao
struct detPROFTRANS{
	int n;//nuymero de tempos emse gravra os perfis
	int* posic;//vetor com os indices da secao transversal em que serao gravdos os perfis termicos
	double* tempo;//vetor de tempo onde os perfis serao gravados
};

//struct com a informacao referentes a gravacao de tendencia na linha de producao
struct detTRENDP{
	double dt;//periodo de tempo em que a tendencia
	int posic;//indice da celua onde se obtem a tendencia
	int pres;//indicador de que se deseja gvar a tendencia de pressao
	int temp;//indicador de que se deseja gvar a tendencia de temperatura
	int hol;//indicador de que se deseja gvar a tendencia de holdup
	int FVH;//indicador de que se deseja gvar a tendencia de Fração Volumétrica de Hidrato (FVH) - solver de Hidratos - chris
	int bet;//indicador de que se deseja gvar a tendencia de beta
	int ugs;//indicador de que se deseja gvar a tendencia de velocidade superficial de gas
	int uls;//indicador de que se deseja gvar a tendencia de velocidade superficial de liquido
	int ug;//indicador de que se deseja gvar a tendencia de velocidade media de gas
	int ul;//indicador de que se deseja gvar a tendencia de velocidade media de liquido
	int arra;//indicador de que se deseja gvar a tendencia de arranjo
	int yco2;//indicador de que se deseja gvar a tendencia de fracao de CO2
	int viscl;//indicador de que se deseja gvar a tendencia de viscosidADE DE liquido
	int viscg;//indicador de que se deseja gvar a tendencia de viscosidade de gas
	int rhog;//indicador de que se deseja gvar a tendencia de massa especifica de gas
	int rhol;//indicador de que se deseja gvar a tendencia de massa especifica de liquido
	int rhoMix;
	int masg;//indicador de que se deseja gvar a tendencia de vazao massica de gas
	int masl;//indicador de que se deseja gvar a tendencia de vazao massica de liquido
	int c0;//indicador de que se deseja gvar a tendencia de parametro de distribuicao
	int ud;//indicador de que se deseja gvar a tendencia de velocidade de escorregamento
	int RGO;//indicador de que se deseja gvar a tendencia de RGO local
	int deng;//indicador de que se deseja gvar a tendencia de densidade de gas no separador
	int calor;//indicador de que se deseja gvar a tendencia de fluxo de calor
	int masstrans;//indicador de que se deseja gvar a tendencia de transferencia de massa
	int qlst;////indicador de que se deseja gvar a tendencia de vazao de fracao pesada standard
	int qlwst;//indicador de que se deseja gvar a tendencia de fracao pesada + agua standard
	int qlstTot;//indicador de que se deseja gvar a tendencia de vazao total de liquido standard
	int qgst;//indicador de que se deseja gvar a tendencia de vazao de leve standard
	int api;//indicador de que se deseja gvar a tendencia de API
	int bsw;//indicador de que se deseja gvar a tendencia de BSW
	int hidro;//indicador de que se deseja gvar a tendencia de termo hidrostatico
	int fric;//indicador de que se deseja gvar a tendencia de termo de friccao
	int dengD;////indicador de que se deseja gvar a tendencia de densidade de gas dissolvido
	int dengL;//indicador de que se deseja gvar a tendencia de densidade de gas livre

	int mlFonte;//indicador de que se deseja gvar a tendencia da fonte de massa hidrocarboneto + agua
	int mgFonte;//indicador de que se deseja gvar a tendencia da fonte de massa gas
	int mcFonte;//indicador de que se deseja gvar a tendencia da fonte de massa liquido complementar

	int dpB;//indicador de incremento de pressao devido a acao de uma bomba
	int potB;//indicador de potencia de bombeio

	int tempChokeJus;//indicador de que se deseja gvar a tendencia da tempetratura a jusante do choke

	int reyi;//indicador de que se deseja gvar a tendencia de Reynolds interno
	int reye;//indicador de que se deseja gvar a tendencia de Reynolds ambiente
	int Fr;//indicador de que se deseja gvar a tendencia de Froude
	int grashi;//indicador de que se deseja gvar a tendencia de Grashof interno
	int grashe;//indicador de que se deseja gvar a tendencia de Grashof externo
	int nusi;//indicador de que se deseja gvar a tendencia de Nusselt interno
	int nuse;//indicador de que se deseja gvar a tendencia de Nussel externo
	int hi;//indicador de que se deseja gvar a tendencia de coefeiciente de pelicula interno
	int he;//indicador de que se deseja gvar a tendencia de coeficiente de pelicula externo
	int pri;//indicador de que se deseja gvar a tendencia de prandtl interno
	int pre;//indicador de que se deseja gvar a tendencia de Prandtl externo

	int Rs;//indicador de que se deseja gvar a tendencia de RS
	int Bo;//indicador de que se deseja gvar a tendencia de Bo

	int volMonM1PT;//Volume de líquido a P&T a montante da M1
	int volJusM1PT;//Volume de líquido a P&T a jusante da M1
	int volMonM1ST;//Volume de líquido standard a montante da M1
    int volJusM1ST;//Volume de líquido standard a jusante da M1

    int autoVal;
    int autoVel;
    int flutuacao;

	int comp;//comprimento onde a tendencia e gravada, referencia fundo de poco
	int diamInt;
	int TempParede;

	int dadosParafina;

	int inventarioGas;
	int inventarioLiq;

	int subResfria;

	string rotulo;//rotulo
};

//struct com informacoes sobre as variaveis que serao ap�resentadas na tela do computador enquanto a simulacao ocorre
struct detTela{
	int col;//indica se a variavel e da linha de producao ou de servico, =1->linha de producao
	int posic;//indice da celula onde sera obtido o valor da variavel
	int var;//variavel:
	//producao-> 1-pressao; 2-temperatura, 3-alfa, 4- USG, 5- USL
	//servico-> 1-pressao; 2-temperatura, 3- USG

	/*int pres;
	int temp;
	int alf;
	int bet;
	int ugs;
	int uls;*/
};

//struct com a informacao referentes a gravacao de tendencia na linha de servico
struct detTRENDG{
	double dt;//periodo de tempo em que a tendencia
	int posic;//indice da celua onde se obtem a tendencia
	int pres;//indicador de que se deseja gvar a tendencia de pressao
	int temp;//indicador de que se deseja gvar a tendencia de temperatura
	int ugs;//indicador de que se deseja gvar a tendencia de velocidade media de gas
	int ug;//indicador de que se deseja gvar a tendencia de velocidade media de gas
	int tens;//indicador de que se deseja gvar a tendencia de tensao de cisalhamento
	int viscg;//indicador de que se deseja gvar a tendencia de viscosidade de gas
	int rhog;//indicador de que se deseja gvar a tendencia de massa especifica de gas
	int masg;//indicador de que se deseja gvar a tendencia de vazao massica de gas
	int masl;//indicador de que se deseja gvar a tendencia de vazao massica de liquido
	int hidro;//indicador de que se deseja gvar a tendencia de termo hidrostatico
	int fric;//indicador de que se deseja gvar a tendencia de termo de friccao
	int FVHG;//indicador de que se deseja gravar a fracao volumetrica de hidrato na linha de servico - chris - Hidratos
	int calor;//indicador de que se deseja gvar a tendencia de fluxo de calor
	int qgst;//indicador de que se deseja gvar a tendencia de vazao de gas standard
	int velgarg;//indicador de que se deseja gvar a tendencia de velocidade de gas no orificio da VGL
	int pEstagVGL;//indicador de que se deseja gvar a tendencia de pressao a montante da VGL
	int tEstagVGL;//indicador de que se deseja gvar a tendencia de temperatura a montante da VGL
	int pGargVGL;//indicador de que se deseja gvar a tendencia de pressao a jusante da VGL
	int tGargVGL;//indicador de que se deseja gvar a tendencia de temperatura a jusante da VGL
	int qVGL;//indicador de que se deseja gvar a tendencia de vazao de gas na VGL

	int reyi;//indicador de que se deseja gvar a tendencia de Reynolds interno
	int reye;//indicador de que se deseja gvar a tendencia de Reynolds ambiente
	int grashi;//indicador de que se deseja gvar a tendencia de Grashof interno
	int grashe;//indicador de que se deseja gvar a tendencia de Grashof externo
	int nusi;//indicador de que se deseja gvar a tendencia de Nusselt interno
	int nuse;//indicador de que se deseja gvar a tendencia de Nussel externo
	int hi;//indicador de que se deseja gvar a tendencia de coefeiciente de pelicula interno
	int he;//indicador de que se deseja gvar a tendencia de coeficiente de pelicula externo
	int pri;//indicador de que se deseja gvar a tendencia de prandtl interno
	int pre;//indicador de que se deseja gvar a tendencia de Prandtl externo
	int comp;//comprimento onde a tendencia e gravada, referencia fundo de poco
	int diamInt;
	int TempParede;

	int subResfria;
	string rotulo;//rotulo
};

//struct com a tendencia de temperatura em um determinado ponto de uma secao transversal
struct detTRENDTrans{
	double dt;//periodicidade em que a tendencia e gravada
	int posic;//celula em que a tendencia e gravada
	int camada;//camada da secao em que a tendencia e gravada
	int discre;//posicao da discretizacao da camada em que a tendencia e gravada
	int comp;//posicao em metros onde a cpelula se encontra
	string rotulo;//rotulo
};

//struct com os detalhes de tabela para montagem de tabelas de propriedade dos fluidos
struct detTabelaEntalp{
	int npont;//npumero de pontos da tabela
	int npontB;//npumero de pontos da tabela
	double pmax;//poressao maxima kgf/cm2
	double pmin;//pressao minima kgf/cm2
	double tmax;//temperatura maximo C
	double tmin;//temperatura minima C
};

//struct auxiliar no registro de eventos no arquivo de Log, indica qual evento, o inpicio e fim do evento
//e uqais estados foram mudados no evento
struct LogEvento{
	double instante;//momento emque se inicia o evento
	double duracao;//duracao do evento
	string descricao;//descricao do evento
	double estIni;//estado antes do inpicio do evento
	double estFim;//estado depois do evento
	bool operator<(const LogEvento& e) const{
		if(this->instante<e.instante) return true;
		else return false;
	}
};

//struct para o caso de simulacao de poco injetor, detalhando condicao de contorno, se o fluido e agua, co2
//ou um liquido definido pelo usuario. Observacao, o modelo de injecao nao trata de condicoes permanentes
struct detCondConInjec{
	int CC;//tipo de condicao de contorno:
	//(vazao de injecao e IPR no fundo, CC0)
	//(pressao de injecao e IPR no fundo; CC1)
	//(pressao de fundo e IPR no fundo, CC2)
	//(pressao de injecao e pressao de fundo; CC3)
	//(vazao de injecao e pressao de injecao, CC4)
	//(vazao de injecao e pressao de fundo, CC5)
	int tipoFlui;//0->usuario, 1-> agua, 2-> tabela
	double salin;//salinidade da agua
	double tempinj;//temperatura de injecao C
	double vazinj;//vazao de injecao Sm3/d
	double presinj;//pressao de injecao kgf/cm2
	double presfundo;//pressao de fundo kgf/cm2
	string pvtsimarqInj;//nome do arquivo pvtsim para injecao de gas rico em CO2
};

//struct com delahes temicos de uma camada litologica do poco
struct detFormacao{
  int id;
	double tempo;//tempo de producao do poco, dias
	double cond;//condutividade da formacao,  W/mK
	double cp;//calor especifico da formacao, J/kgK
	double rho;//massa especifica da formacao, kg/m3
};

//struct com as propriedade do fluido coletadas da tabela PVTSim
struct tabelaFlash{
    int id;
    int TwoOrThree;//indicador se a tabela e bifpasica ou trifasica
	string arquivo;//nome do arquivo PVTSim
	//double compini;
	//double compfim;
	//int celfim;
	//int celini;
	int visc;//indicador se se deseja usar a viscosidade da tabela PVTsim ou usar um modelo black-oil para o calciulo
	//das viscosidades, 0->tabela, 1->black oil
	//conjunto de tabelas obtidas diretamente do aqruivo PVTSim
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
    double **viscO;//alteracao6
    double **viscG;//alteracao6
    double *PBF;
    double *TBF;
};

struct composicional{
	string arquivo;
	int npseudo;
	int liqModel;
	int vapModel;
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
    ~composicional(){
    	if(npseudo>0){
    	delete [] fracMol;
    	delete [] CpIGCoefs;
    	delete [] TIndepPeneloux;
    	delete [] fatAcent;
    	delete [] kij;
    	delete [] lij;
    	delete [] masMol;
    	delete [] parac;
    	delete [] presCrit;
    	delete [] tempCrit;
    	}
    }
};

struct detTabVisc{
	int parserie;
	double* visc;
	double* temp;
};

struct dadosParafina{
	string arquivo;
	int poroRey;
	double valRey;
	int C2C3;
	double valC2;
	double valC3;
	int difus;
	int alteraViscFlu;
	double multDifus;
	double rug;
	double multVis;
	double DViscWax;
	double EViscWax;
	double FViscWax;
	double DmultipWax; // Samuel - 10/10/25
	double EmultipWax; // Samuel - 10/10/25
	double FmultipWax; // Samuel - 10/10/25
};

struct acopDuto3D{
	int indCel;
	string rotulo;
	double FE;
	double comp;
	//DadosGeo duto;
};

class Ler{
    public:
    
	//*** solver hidratos - chris
   	double MMH, MMG, MMW, Whamm; //chris - Hidratos
   	//double Khamm //chris - Hidratos
   	//double Methanol, MMEG; chris - Hidratos
    //double rhoH; //vem do Marlim 3 --> , rhoG, rhoW, //chris - Hidratos
	double coefEsteq;
  	/*double estruturaHidratosIk1, estruturaHidratosIk2;
   	double estruturaHidratosIIk1, estruturaHidratosIIk2;*/
	string inibidor;
	double fracFWcarregada;
	string estruturaHidratos;
   	double rd, rp;

	int saidaClassica;//atributo que indica como sera a chamada de fim de simulacao, atributo meramente de perfumaria
	int cicloAcopTerm;//define se se deseja fazer uma iteracao a mais para se considerar o termo dT/dt
	//nas equacoes de conservacao de massa 1-> dT/dt
	int flashCompleto;//atributo que indica se vai ser usada a tabela de PVTSim ou Black oil 1-> PVTSim
	int tabelaDinamica;
	int usaTabela;//atributo que indica que se est�pa usando uma tabela, seja no caso black oil ou no caso PVTSim
	int ConContEntrada;//indica qual a condicao de contorno na entrada do sistema de prosucao, se 0->fechado,
	//se 1->pressao, se 2 vazão e pressão na entrada
        string impfile;//arquivo Json
	string pvtsimarq;//arquivo PVTSim
	int modelcp;//se 1, indica que, mesmo usando um modelo blackoil, se utilizara os valores de Cp de uma tabela PVTSim
	int modelJTL;//se 1, indica que, mesmo usando um modelo blackoil, se utilizara os valores de Drhol/dT de uma tabela PVTSim
	int nmaterial;//numero de materiais cadastrados no Json
	int ncorte;//numero de secoes transversais cadastradas no Json
	int nduto;//Numero de dutos cadastrados no Json
	int nunidadep;//numero de unidades de producapo cadastradas no Json
	int ncelp;//numero de celulas do sistema de producao
	int lingas;//se 1, indica que se utilizara uma linha de servico acoplada ao sistema de producao
	int nunidadeg;//numero de unidades da linha de servico cadastradas no Json
	int ncelg;//numero de celulas do sistema de gas
	int nipr;//nuemro de IPR cadastradas no Json
	int nvalvgas;//numero de VGLs cadastradas no Json
	int nvalv;//numero de valvulas cadastradas no Json
	int nfontechk;//nao usada
	int ninjgas;//nemro de fontes de gas cadastradas no Json
	int ninjliq;//numero de fontes de liquido cadastradas no Json
	int ninjmass;//numero de fontes de massa cadastradas no Json
	int nPoroRad;//numero de fontes de meio poroso radial cadastradas no Json
	int nPoro2D;//numero de fontes de meio poroso 2D cadastradas no Json
	int nfuro;//numero de fontes de pressao cadastradas no Json
	int nbcs;//npumero de BCSs cadastradas no Json
	int nbvol;//numero de bombas volumetricas cadastradas no Json
	int ndpreq;//numero de incrementpos de pressao cadastrados no Json
	int npig;//npumero de pigs cadastrados no Json
	int nperfisp;//numero de perfis de producao que serao impressos
	int nperfisg;//numero de perfis de linha de servico que serao impressos
	int nperfistransp;//numero de perfis termicos da secao transversalc que serao impressos
	int nperfistransg;//nuemro de perfis termicos da secao transversal da linha de gas que serao impressos
	int ntendp;//numero de arquivos de tendencia na linha de producao que serao impressos
	int ntendg;//numero de arquivos de tendencia na linha de gas que serao impressos
	int ntendtransp;//numero de arquivos de tendencia de temperatura
	//na secao transversal aa linha de producao que serao impressos
	int ntendtransg;//numero de arquivos de tendencia de temperatura
	//na secao transversal da linha de gas que serao impressos
	int ntela;//numpro de variaveis que serao impressa na tela do computador enquanto se processa a simulacao
	int equilterm;//se 1, indica que caso a condicao inicial seja dada pelo usuario, se admitira que o
	//perfil termico da parede da tubulacao se encontra em equilibrio termico com o ambiente
	int latente;//considera o efeito dao calopr latente na simulacao, quando se utiliza um modelo black oil,
	//para tanto, deve-se tambem informar o arquivo PVTSim onde se obtera as entalpias de liquido e gas
	int condlatente;
	int tabp;//indica, para o caso black oil, que se montara uma tabela de fator de compresibilidade e suas derivadas
	//antes de se iniciar a simulacao, linha de poroducao
	int tabg;//indica, para o caso black oil, que se montara uma tabela de fator de compresibilidade e suas derivadas
	//antes de se iniciar a simulacao, linha de gas
	int transmass;//indica qual o tipo de modelo de transferencia se utilizara, quando zero, modelo completo,
    //quando 1 modelo explicito, quando 2, utiliza-se um modelo simplificado de baixa relevancia e
    //, finalmente, quando 3, elimina-se a transferencia de massa;
	int trackRGO;//se 1, indica que se deseja fazer um tracking de RGO local, alem de outras variaveis como API e BSW
	detRGO varRGO;//sem uso atualmente
	int trackDeng;//se 1, indica que se deseja fazer um tracking da densidade de gas,
	//alem de outras variaveis como a fracao de CO2
    int perm;//se 1, indica que se deseja calcular a solucao permanente antes de se iniciar a simulacao
    int snaps;//se 1-> indica que a condicao inicial e dada por um arquivo de dados
    origemGeometria_t origemGeometria;//indica como a linha de producao e preenchida no Json, se tomando como referencia
    //a plataforma ou fundo do poco. OBS: internamente, a referencia e sempre fundo do poco
    sistemaSimulacao_t sistemaSimulacao;//indica se e um sistema de producao ou de injecao
    bool sentidoGeometriaSegueEscoamento;//sem uso atualmente
    int descarga;//se 1-> indica que deseja-se fazer uma simulacao de descarga de gas-lift
    double salinDescarga;//salinidade do fluido de completacao em uma smulacao de descarga de gas-lift
    double compInterDesc;//posicao. em comprimento, tomando como referencia a plataforma,
    //inicial em que se encontra a interface gas/fluido de completacao
    //na linha de servico, em uma simulacao de descarga de gas lift
    double compInterDescP;//posicao. em comprimento, tomando como referencia o fundo de poco,
    //inicial em que se encontra a interface gas/fluido de completacao
    //na linha de producao, em uma simulacao de descarga de gas lift
    int celdescarga;//indice da celula, tomando como referencia a plataforma,
    //inicial em que se encontra a interface gas/fluido de completacao
    //na linha de servico, em uma simulacao de descarga de gas lift
    int celdescargaP;//indice da celula, tomando como referencia o fundo de poco,
    //inicial em que se encontra a interface gas/fluido de completacao
    //na linha de producao, em uma simulacao de descarga de gas lift

    int controDesc;//para o modo de descarga de gas lift, quando 1, indica que se deseja fazer um controle automatizado da descarga,
    //tentando definir como deve ser feita a injecao de gas para que se impeca velocidades erosionais nas VGLs durante a descraga
    double vazDescControl;//limite maximo de vazao de liquido na valvula, m3/d
    double presMaxDesc;//pressao maxima a jusante do choke de produycao durante a descarga
    double presMinDesc;//pressao minima a jusante do choke de produycao durante a descarga
    double presIniDesc;//pressao a jusante do choke de producao em que se inicia a descarga
    double tempDescarga;//temperatura em que e feita a descarga
    double presMaxDescG;//pressao maxima de injecao durante a decarga
    double presMinDescG;//pressao mpiunima de injecao durante a descraga
    double presIniDescG;//pressao inicial na linha de ga, quando se inicia a descrag
    double tempoLatenciaDesc;//tempo entre a espera da estabilizacao da imulacao e o inicio do processo de descarga

    string snapshotArqIn;//nome do arquivo  de entrada de dados, quando a condicao inicial e feita por arquivo
    int ordperm;//ordem da solucao permenente, primeira ou segunda orde, 1 ou 2, rrespectivamente
    int transiente;//se 1, indica que se desja fazer a simulacao permanente
	int imaxDG;//sem uso atualmente
	int iminDG;//sem uso atualmente
	int pocinjec;//se 1-> se fara a simulacao de poco injetor
	double tfinal;//tempo total da simulacao
	double dtmax;//incremento de tempo indicado pelo usuario no momento da partida da simulacao
	double anmP;//posicao em comprimento da anm na linha de produclao, referencia no fundo do poco
	double anmG;//posicao em comprimento da anm na linha de servico, referencia na plataforma
	double mono;//menor valor de fracao de vazio o holdup onde se admite que o problema se torna monofasico
	double critcond;//menor valor de fracao de vazio o holdup onde se desliga o modelo de transferencia de massa
	int corrDeng;//se 1, indica que, no modelo black oil, a desnidade do gas dissolvido devera ser diferenciada
	//da densidade do gas livre
    vector<double> evento;//vetor com os tempos em que os eventos ocorrem
	vector<LogEvento> logevento;//vetor com todos os eventos, inclusive impressao de perfis, com suas descricoes e
	//alteracoes de estado
	vector<LogEvento> logeventoEstat;//vetor apenas com eventos relacionados com acessorios, sem impressao de perfis,
	//com suas descricoes e alteracoes de estado

    detTMAX dtmaxserie;//objeto com a serie de tempo dos limites maximo de incremeto de tempo
    int nfluP;//numero de fluidos de producao
    int indfluPIni;//indice do fluido de prpoducao usado para se iniciar a simulacao, quando nao se solicita
    //a solucao permanente
    int chkv;//indicado, se =1, da existencia de uma cvalvula check na superficie
    int* indFlup;
    ProFlu* flup;//vetor com os fluidos de producao
    ProFluCol fluc;//objeto com o fluido complementar
    ProFlu flug;//objeto com o fluido gas da linha de servico ou da fonte de gas
    detTabelaEntalp tabent;//tabela com os limites de pressao e temepratura para a tabela de PVTSim
    detTabelaEntalp tabentCrit;//tabela com os limites de pressao reduzida e temperatura reduzida para o
    //fator de compressibilidade do gas e suas derivadas
    material* mat;//vetor com os materiais cadastrados no Json
    corteduto* corte;//vetor com as secoes transversais cadastradas no json
    detduto* duto;//vetor com as informacoes de dutos cadastrados no json
    detalhaP* unidadeP;//vetor com as unidades de producao cadastradas no Json
    detcelp* celp;//vetor com as structs de celulas de producao
    detalhaG* unidadeG;//vetor com as unidades de servico cadastradas no Json
    detcelg* celg;//vetor com as structs de celulas de servico
    detIPR* IPRS;//vetor com as structs de IPRs cadastradas no Json
    detGASINJ gasinj;//objeto com as informacoes do sistema de injecao de gas lift, condicao de contorno
    detVALVGL* valvgl;//vetor com o detalhamento das VGL cadastradas no Json
    detValv* valv;//vetor com o detalhamento das valvulas cadastradas no Json
    detFONGAS* fonteg;//vetor com o detalhamento das fontes de gas cadastradas no Json
    detFONLIQ* fontel;//vetor com o detalhamento das fontes de liquido cadastradas no Json
    detFONMASS* fontem;//vetor com o detalhamento das fontes de massa cadastradas no Json
    detPoroRad* porosoRad;//vetor com o detalhamento das fontes de meio poroso radial cadastradas no Json
    detPoro2D* poroso2D;//vetor com o detalhamento das fontes de meio poroso 2D cadastradas no Json
    detFURO* furo;//vetor com o detalhamento das fontes de pressao cadastradas no Json
    detBCS* bcs;//vetor com o detalhamento das BCS cadastradas no Json
    detBVOL* bvol;//vetor com o detalhamento das bombas volumetricas cadastradas no Json
    detDPREQ* dpreq;//vetor com o detalhamento dos incrementos de pressao cadastrados no Json
    detMASTER1 master1;//objeto com detalhamento de serie temporal da master1
    detMASTER1 master2;//objeto com detalhamento de serie temporal da master2
    detPSEP psep;//objeto com detalhamento de serie temporal da pressao na plataforma
    detCorrec correcao;
    detCHOKESUP chokep;//objeto com detalhamento de serie temporal do choke de superficie
    detCHOKESUP* fontechk;//sem uso
    detCHOKESUP chokes;//objeto com detalhamento de serie temporal do choke de injecao de gas
    detPig* pig;//vetor com os tempos e posicoes de lancamenbto dos pigs cadastrados no Json
    detPROFP profp;//obejto com as informacoes de impressao de perfis de producao
    int nvarprofp;//numero de variaveis que tem perfis de producao impressos
    detPROFG profg;//obejto com as informacoes de impressao de perfis de servico
    int nvarprofg;//numero de variaveis que tem perfis de servico impressos
    detPROFTRANS proftransp;//obejto com as informacoes de impressao de perfis de temperatura em secoes transversais
    //de sistemas de producao
    detPROFTRANS proftransg;//obejto com as informacoes de impressao de perfis de temperatura em secoes transversais
    //de sistemas de injecao
    detTRENDP* trendp;//vetor com as variaveis e posicao da impressao de tendencias de producao
    int* nvartrendp;//vetor com o numero de variaveis que sao impressas em cada tendencia de prosucao
    detTRENDG* trendg;//vetor com as variaveis e posicao da impressao de tendencias de servico
    int* nvartrendg;//vetor com o numero de variaveis que sao impressas em cada tendencia de servico
    detTRENDTrans* trendtransp;//vetor com as posicoes da impressao de tendencias de temperatura
    //de secoes transversais no sistema de producao producao
    detTRENDTrans* trendtransg;//vetor com as posicoes da impressao de tendencias de temperatura
    //de secoes transversais no sistema de producao producao
    detTela* tela;//vetor com as variaveis a serem apresentadas na tela do computador
    detCondConInjec condpocinj;//objeto com as condicoes de contorno para o caso de poco de injecao
    int nform; // numero de litologias cadastradas no Json
    detFormacao* formacPoc;// vetor com as informacoes tpermicas das litologias cadastradas no Json
    detPresEnt CCPres;//objeto com as informacoes de condicao de contorno de pressao na entrada da tubulacao
    detVazPresEnt CCVPres;//objeto com as informacoes de condicao de contorno de pressao e vazão na entrada da tubulacao
    tabelaFlash* flash;//objeto com as tabelas de propriedade dos fluidos obtidos de um aqruivo PVTSim
    composicional* compDet;
    //composicional compDet[5];
    int MedSimpPresFront;//indicador  de como as pressoe na fronteira sao calculadas, por um metodo
    //mais completo ou por um metodo mais simples; quando zero, calcula-se da maneira mais completa,
    //quando 1, usa os valores medios
    double limTransMass;//limite de pressao em que abaixo deste valor, se utiliza um modelo de transferencia de massa
    //mais simples, porem mais estavel (em pressoes muito baixas, em shutins, o simulador tem dificuldades com o modelo
    //de transferencia de massa)
    int RelaxaDTChoke;//indiador de penalizacao do incremento de tempo, quando se observa oscilacoes na vazao de
    //lpiquido entrando e saindo do choke de superficie
    double CriterioConvergPerm;//critep�rio de convergencia permanente quando se usa um modelo mais
    //rigoroso de convergencia atualmente pouco utilizado, devido ao criterio AceleraConvergPerm
    int AceleraConvergPerm;//indicador do uso de uma simplificacao no solver permnanente que acelera
    //a convergencia do solevre alem de ser mais estavel, atualmente, a opcao padao do simulador
    //1-> usa o acelardor
    int escorregamentoCelulaContorno;//indicador usado para se saber se se deseja usar um modelo de scxorregamento
    //no ulyimo volume do sistema de producao, eventualmente neste ultimo volume, pode ocorrer osculacoes
    //devido a mudancas de alta freque^ncia no arrranjo de escoamento. Para atenuar este problema, se
    // desliga o escorregamento entre as fases
    int correcaoContracorPerm;//utilizado no solver permanente, quando=1, se faz uma cporrecao no calculo da temperatura
    //do gas que entra no anular usando um modelo aproximado de troca termica em contracorrente
    //com o fluido escoando na coluna (observacao, no modelo transiente, este modelo contracorrente e feito com precisao)

    // comprimento total da linha de producao
    double nCompTotalUnidadesP;
    // comprimento total da linha de servico
    double nCompTotalUnidadesG;

    DadosGeo* dutosMRT;//vetor com objetos de dutos ja tarnsformado para ser usado no objetos celulas
    //da classe celula3

    int eventoabre;//numero de eventos em que se abre a Master1, isto e utilizado para monitoramento da abertura
    //de master1 e fazer restricao de tempo nestes momentos
    double* Tevento;//vetor com os tempos em que se iniciam a abertura de Master1
    int eventofecha;//numero de eventos em que se fecha a Master1, isto e utilizado para monitoramento da fechamento
    //de master1 e fazer restricao de tempo nestes momentos
    double* Teventof;//vetor com os tempos em que se iniciam o fechamento de Master1
    int eventoabrePXO;//sem uso
    double* TeventoPXO;//sem uso
    double areagargPXO;//sem uso

    int npontos;//numero de pontos de qualquer tabela-> para black oil e PVTSim
    double** zdranP;//tabela de fator de compressibilidade, black oil (pressao reduzida X temperatura reduzida)
    double** dzdpP;//tabela com a derivada do Z com a pressao, black oil (pressao reduzida X temperatura reduzida)
    double** dzdtP;//tabela com a derivada do Z com a temperatura, black oil (pressao reduzida X temperatura reduzida)
    double** cpg;//tabela com calor expecifico de gas com a temperatura, black oil
    double** cpl;//tabela com calor expecifico de liquido com a temperatura, black oil
    double** drholdT;//tabela com a derivada de massa especifica de liquido com a temperatura, black oil

    //(pressao em kgf/cm2 X temperatura C)
    double** RhoInj;//tabela com a massa especifica, caso poco de injecao,
    //obtido do arquivo PVTSim, propriedade do gas, kg/m3
    double** ViscInj;//tabela com a viscosidade, caso poco de injecao,
    //obtido do arquivo PVTSim, propriedade do gas,Pas
    double** CondInj;//tabela com a condutvidade termica, caso poco de injecao,
    //obtido do arquivo PVTSim, propriedade do gas W/mK
    double** CpInj;//tabela com calor especifico, caso poco de injecao,
    //obtido do arquivo PVTSim, propriedade do gas j/kgK
    double** DrhoDtInj;//tabela com a derivada da massa especifica com a temperatura, caso poco de injecao,
    //obtido do arquivo PVTSim, propriedade do gas,kg/m3/K

    int nvecEmul;//para o caso em que o modelo de emulsao e dado pelo usuario por meio de pares BSWXmultiplicadore,
    //nvecEmul tamanho do vetor
    double* BSWVec;//vetor com os valores de emulsao fornecido pelo usuario
    double* emulVec;//vetor com os valores do multiuplicadores de viscosidade fornecido pelo usupario

    int escorregaPerm;//indicador que determina se o modelo sera ou nao com escorregamento, se ->1, poadrao,
    //indica que se ira usar modelo de escorregamento no solver permanente
    int escorregaTran;//indicador que determina se o modelo sera ou nao com escorregamento, se ->1, poadrao,
    //indica que se ira usar modelo de escorregamento no solver transiente

    int tabRSPB;//indica se se deseja montar uma tabela de Razao de Solubilidade usando um modelo black oil
    // antes de iniciar a simulacao, para melhorar o desempenho, util proincipalmente no modelo RS de Livia, que
    //consome muito tempo de computacao

    //indicador de correlacoes de escorregamento
    //0->Choi et al.;
    //1->Bhagwat & Ghajar Modificado;
    //2->Franca & Lahey;
    //3->Hibiki & Ishii;
    //4->Bhagwat & Ghajar;
    int CorreEstrat;//pode ser 0,1,2,4
    int CorreDisper;//pode ser 0, 1, 4
    int CorreAnular;//pode ser 0,1,3,4

    int mapaArranjo;
    //indicador de tipo de mapa
    //0-> Barnea simplificado
    //1-> Barnea Completo

    int saidaTela;//atributo que define como vai ser a saida na tela do computador durante a simulacao, se apenas
    //indicando a porcentagm simulada ou se usando uma saida com mais informacoes, tempo, passo de tempo e variaveis,
    //0-> saida com porcentagem, caso padrao

	double valTempChokeJus;//valor onde e armazenado a temperatura a jusante do choke de superfpicie,
    //valor meramente para gravacao em graficos de tendencia, quando solicitado

	int nsnp; //número de gravações(fotografias) de momentos da simulação
	//caso o usuário queira que um conjunto de momentos sejam gravados
	double* tempsnp; //vetor com os valores do momento indicados pelo usuário para gravação

	int nTcorrecaoMassaEspLiq;
	double* TcorrecaoMassaEspLiq;
	int* VcorrecaoMassaEspLiq;
	int nTsonico;
	double* Tempsonico;
	int* Vsonico;
	int correcaoMassaEspLiq;
	int desligaDeriTransMassDTemp;
	int corrigeContSep;
	double taxaDespre;
	int acopColAnulPermForte;
	int tipoFluido;
	int mudaArea;
	int npseudo;



	double* compLinServ;//composicao do gas na linha de servico, caso em que na linha de producao
	//se tem um modelo composicional, este vetor guarda a composicao do gas na linha de servico ou
	//em uma fonte de gas para se ter a composicao da mistura na linha de producao quando este gas
	//e injetado nesta linha

	int miniTabAtraso;
	double miniTabDp;
	double miniTabDt;

	double tempReves;
	double razCompGasReves;

	int AS;
	int paralelAS;

	detTabVisc *tabVisc;

	int nthrd;

	double buscaFC;

	double chutePerm;

	int HISEP;

	int reverso;

	int modoSegrega;
	double* vecTSegrega;
	int* vecSegrega;
	int nsegrega;

	int estabCol;

	varGlob1D* vg1dSP;

	int modoXY;
	double xProd0;
	double yProd0;
	double xServ0;
	double yServ0;

	int modoParafina;
	dadosParafina detalParafina;

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
	double* oInterpolatedWaxConcs;

	int npseudoWax;
	int iPresWax;
	int iTempWax;

	int modoDifus3D;
	int threadP3D;
	string modoDifus3DJson;
	int nacop;
	vector<acopDuto3D> celAcop;
	vector<DadosGeo> geoAcop;

	int desligaPenalizaDT;
	int controleDTvalv;

	int indicaBeta;

	int conINIAmbP;
	int conINIAmbS;

	int redeperm;

	double** envelopeHidra;
	int existeEnvelope;
	int calculaEnvelope;
	int nparEnvelope;

	double pmin;
	double tmin;

	int tipoModeloDrift;
	double JTLiquidoSimple;

	int nintermi;
	detIntermi* intermi;

	/*double* vecTestabCol;
	int* vecestabCol;
	int nestabCol;*/

	//int reduzAtrito;

    /*
     * Default Constructor
     */
    Ler();
    /*
     * Fields Constructor
     */
    Ler(const string IMPFILE, const string ARQUIVO_LOG, const tipoValidacaoJson_t VALIDACAO_MRT,
    		const tipoSimulacao_t SIMULACAO_MRT, int vreverso=0,varGlob1D* Vvg1dSP=0, int vredeperm=1);
    /*
     * Copy Constructor
     */
    Ler(const Ler&);
    /*
     * Assignment Operator
     */
    Ler& operator=(const Ler&);
    /*
     * Destructor
     */
	~Ler(){
		//delete [] dtmaxserie.tempo;
		//delete [] dtmaxserie.valor;
		//delete [] flup;

		if (dtmaxserie.parserie > 0) {
			delete[] dtmaxserie.tempo;
			delete[] dtmaxserie.valor;
		}

		if (nfluP > 0){
			delete[] flup;
			delete[] indFlup;
		}

		if(nform>0) delete [] formacPoc; // 03-04-2018

		//delete [] varRGO.tempo;
		//delete [] varRGO.valor;

		//if(mat) delete [] mat;
		if (nmaterial > 0)
			delete[] mat;

		//if(corte){
		  //for(int i=0; i<this->ncorte;i++){
			  //delete [] corte[i].diam;
			  //delete [] corte[i].indmat;
			  //delete [] corte[i].discre;
		  //}
		  //delete [] corte;
		//}
		if (ncorte > 0) {
			for (int i = 0; i < this->ncorte; i++) {
				delete[] corte[i].diam;
				delete[] corte[i].indmat;
				delete[] corte[i].discre;
			}
			delete[] corte;
		}

		//if(duto) delete [] duto;
		if (nduto > 0)
			delete[] duto;


		if(nunidadep>0){
			for(int i=0; i<nunidadep;i++){
				delete [] unidadeP[i].dx;
				delete [] unidadeP[i].dxVar;
				for(int j=0;j<12;j++)delete [] unidadeP[i].var[j];
				delete [] unidadeP[i].var;
				if(modoDifus3D==1){
					delete [] unidadeP[i].difusTerm3D;
					delete [] unidadeP[i].difusTerm3DFE;
					delete [] unidadeP[i].difusTerm3DAcop;
				}
			}
			delete [] unidadeP;
		}

		if(nunidadeg>0 && lingas>0){
			for(int i=0; i<nunidadeg;i++){
				delete [] unidadeG[i].dx;
				delete [] unidadeG[i].dxVar;
				for(int j=0;j<9;j++)delete [] unidadeG[i].var[j];
				delete [] unidadeG[i].var;
			}
			delete [] unidadeG;
		}

		if(ncelp>0) delete [] celp;

		if(ncelg>0) delete [] celg;

		if(nparEnvelope>0){
		  for(int i=0; i<nparEnvelope;i++){
			  delete [] envelopeHidra[i];
		  }
		  delete [] envelopeHidra;
		}

		if(nipr>0){
		  for(int i=0; i<this->nipr;i++){
			  delete [] IPRS[i].pres;
			  delete [] IPRS[i].tpres;
			  delete [] IPRS[i].temp;
			  delete [] IPRS[i].ttemp;
			  if(IPRS[i].tipoIPR==0 || IPRS[i].tipoIPR==1){
				  delete [] IPRS[i].ip;
				  delete [] IPRS[i].tip;
			  }
			  else{
				  delete [] IPRS[i].qMax;
				  delete [] IPRS[i].tqMax;
			  }
			  delete [] IPRS[i].jp;
			  delete [] IPRS[i].tjp;
		  }
		  delete [] IPRS;
		}
		if( ConContEntrada==1){
	        delete [] CCPres.temperatura;
	        delete [] CCPres.pres;
	        delete [] CCPres.tit;
	        delete [] CCPres.bet;
	        delete [] CCPres.tempo;
		}
		else if( ConContEntrada==2){
	        delete [] CCVPres.temperatura;
	        delete [] CCVPres.pres;
	        delete [] CCVPres.mass;
	        delete [] CCVPres.bet;
	        delete [] CCVPres.tempo;
		}

		if(lingas>0 && controDesc==0){
			delete [] gasinj.tempo;
			if(gasinj.tipoCC==1)delete [] gasinj.vazgas;
			else if(gasinj.tipoCC==0){//mudancaChute
				delete [] gasinj.presinj;
				//if(gasinj.chuteVaz==1)
				delete [] gasinj.vazgas;
			}//mudancaChute
			delete [] gasinj.temperatura;

			if(chokes.parserie>0){
			  delete [] chokes.tempo;
			  delete [] chokes.abertura;
			}
		}

		if(nvalvgas>0 && lingas>0) delete [] valvgl;

		if(nvalv>0){
		  for(int i=0; i<this->nvalv;i++){
			  if(valv[i].ncv>0)delete [] valv[i].cvCurv;
			  delete [] valv[i].abertura;
			  delete [] valv[i].tempo;
		  }
		  delete [] valv;
		}

		if(nfontechk>0){
			for(int i=0; i<this->nfontechk;i++){
				delete [] fontechk[i].tempo;
				delete [] fontechk[i].abertura;
			}
			delete [] fontechk;
		}

		if(ninjgas>0){
		  for(int i=0; i<this->ninjgas;i++){
			  delete [] fonteg[i].temp;
			  delete [] fonteg[i].vazgas;
			  delete [] fonteg[i].vazcomp;
			  delete [] fonteg[i].tempo;
		  }
		  delete [] fonteg;
		}

		if(ninjliq>0){
		  for(int i=0; i<this->ninjliq;i++){
			  delete [] fontel[i].temp;
			  delete [] fontel[i].bet;
			  delete [] fontel[i].vazliq;
			  delete [] fontel[i].tempo;
		  }
		  delete [] fontel;
		}

		if(ninjmass>0){
		  for(int i=0; i<this->ninjmass;i++){
			  delete [] fontem[i].temp;
			  delete [] fontem[i].vazMasT;
			  delete [] fontem[i].vazMasC;
			  delete [] fontem[i].vazMasG;
			  delete [] fontem[i].tempo;
		  }
		  delete [] fontem;
		}

		if(nPoroRad>0){
		  delete [] porosoRad;
		}

		if(nPoro2D>0){
		  delete [] poroso2D;
		}

		if(nfuro>0){
		  for(int i=0; i<this->nfuro;i++){
			  delete [] furo[i].abertura;
			  delete [] furo[i].tempo;

			  delete [] furo[i].check;
			  delete [] furo[i].tempoChk;
		  }
		  delete [] furo;
		}

		if(nbcs>0){
		  for(int i=0; i<this->nbcs;i++){
			  delete [] bcs[i].tempo;
			  delete [] bcs[i].freq;
			  delete [] bcs[i].vaz;
			  delete [] bcs[i].head;
			  delete [] bcs[i].power;
			  delete [] bcs[i].efic;
		  }
		  delete [] bcs;
		}

		if(nbvol>0){
		  for(int i=0; i<this->nbvol;i++){
			  delete [] bvol[i].tempo;
			  delete [] bvol[i].freq;
		  }
		  delete [] bvol;
		}

		if(ndpreq>0){
		  for(int i=0; i<this->ndpreq;i++){
			  delete [] dpreq[i].dp;
			  delete [] dpreq[i].tempo;
		  }
		  delete [] dpreq;
		}

		if(npig>0) delete [] pig;

		int nevento=master1.parserie;
		delete [] master1.tempo;
		delete [] master1.abertura;
		if(master1.ncv>0)delete [] master1.cvCurv;

		if(lingas>0){
			delete [] master2.tempo;
			delete [] master2.abertura;
		}

		if(psep.parserie>0){
		  delete [] psep.tempo;
		  delete [] psep.pres;
		}

		if(correcao.parserieHidro>0) delete [] correcao.dPdLHidro;
		if(correcao.parserieFric>0) delete [] correcao.dPdLFric;
		if(correcao.parserieDT>0) delete [] correcao.dTdL;


		if(chokep.parserie>0){
		  delete [] chokep.tempo;
		  delete [] chokep.abertura;
		  if(chokep.ncv>0)delete [] chokep.cvCurv;
		}

		if(nperfisp>0)	delete [] profp.tempo;
		if(nperfisg>0)	delete [] profg.tempo;
		if(nperfistransp>0){
			delete [] proftransp.posic;
			delete [] proftransp.tempo;
		}
		if(nperfistransg>0 && lingas>0){
			delete [] proftransg.posic;
			delete [] proftransg.tempo;
		}
		if(ntendp>0){
			delete [] trendp;
		    delete [] nvartrendp;
		}
		if(ntendg>0){
			delete [] trendg;
			  delete [] nvartrendg;
		}
		if(ntendtransp>0)	delete [] trendtransp;
		if(ntendtransg>0)	delete [] trendtransg;
		if(nduto>0) delete [] dutosMRT;
		//if(nacop>0) delete [] dutosAcop;

		if(nevento>0) delete [] Tevento;
		if(nevento>0) delete [] Teventof;
		//if(TeventoPXO) delete [] TeventoPXO;

		if(ntela>0) delete [] tela;

	    if(tabp>0||tabg>0){
			  for(int i=0; i<npontos+2;i++) delete[] zdranP[i];
			  delete [] zdranP;
			  for(int i=0; i<npontos+2;i++) delete[] dzdpP[i];
			  delete [] dzdpP;
			  for(int i=0; i<npontos+2;i++) delete[] dzdtP[i];
			  delete [] dzdtP;
	     }
	     int ndiv = tabent.npont - 1;
	      if(modelcp>0 && cpg!=0 && cpl!=0){
			  for(int i=0; i<ndiv+2;i++){
				  delete[] cpg[i];
				  delete[] cpl[i];
			  }
			  delete [] cpg;
			  delete [] cpl;
	      }
	      if(modelJTL==1 && drholdT!=0){
			  for(int i=0; i<ndiv+2;i++){
				  delete[] drholdT[i];
		  }
		  delete [] drholdT;
      }
	  if(flashCompleto==1){
		    for(int i=0;i<nfluP;i++){
			   for(int j=0; j<tabent.npont+1;j++) delete [] flash[i].rholF[j];
			   delete [] flash[i].rholF;
			   for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].rhogF[j];
			   delete [] flash[i].rhogF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].DrholDpF[j];
		       delete [] flash[i].DrholDpF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].DrhogDpF[j];
		       delete [] flash[i].DrhogDpF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].DrholDtF[j];
		       delete [] flash[i].DrholDtF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].DrhogDtF[j];
		       delete [] flash[i].DrhogDtF;
               for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].DrhowDtF[j];
		       delete [] flash[i].DrhowDtF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].RSF[j];
		       delete [] flash[i].RSF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].cplF[j];
		       delete [] flash[i].cplF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].cpgF[j];
		       delete [] flash[i].cpgF;
               for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].cpwF[j];
		       delete [] flash[i].cpwF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].HlF[j];
		       delete [] flash[i].HlF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].HgF[j];
		       delete [] flash[i].HgF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].sigOGF[j];
		       delete [] flash[i].sigOGF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].sigWGF[j];
		       delete [] flash[i].sigWGF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].viscO[j];//alteracao6
		       delete [] flash[i].viscO;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash[i].viscG[j];//alteracao6
		       delete [] flash[i].viscG;

		       delete [] flash[i].PBF;
		       delete [] flash[i].TBF;
		  }
		  delete [] flash;
	  }
	  if(flashCompleto==2){
		    /*for(int i=0;i<nfluP;i++){
		    	delete[] compDet[i].fracMol;
		    	delete[] compDet[i].CpIGCoefs;
		    	delete[] compDet[i].TIndepPeneloux;
		    	delete[] compDet[i].fatAcent;
		    	delete[] compDet[i].kij;
		    	delete[] compDet[i].lij;
		    	delete[] compDet[i].masMol;
		    	delete[] compDet[i].parac;
		    	delete[] compDet[i].presCrit;
		    	delete[] compDet[i].tempCrit;
		    }*/
		    if(nfluP>0)delete[] compDet;
	    	/*delete  compDet.fracMol;
	    	delete  compDet.CpIGCoefs;
	    	delete  compDet.TIndepPeneloux;
	    	delete  compDet.fatAcent;
	    	delete  compDet.kij;
	    	delete  compDet.lij;
	    	delete  compDet.masMol;
	    	delete  compDet.parac;
	    	delete  compDet.presCrit;
	    	delete  compDet.tempCrit;*/
		    if(pocinjec == 0 && (lingas==1 || ninjgas>0)) delete[] compLinServ;
	  }
      if(pocinjec>0 && condpocinj.tipoFlui==2){
 		  if(RhoInj){
 			  for(int i=0; i<ndiv+2;i++) delete[] RhoInj[i];
 		      delete [] RhoInj;
	 		  }
	 		  if(ViscInj){
	 		      for(int i=0; i<ndiv+2;i++) delete[] ViscInj[i];
	 		      delete [] ViscInj;
	 		  }
	 		  if(CondInj){
	 			  for(int i=0; i<ndiv+2;i++) delete[] CondInj[i];
	 			  delete [] CondInj;
	 		  }
	 		  if(CpInj){
	 			  for(int i=0; i<ndiv+2;i++) delete[] CpInj[i];
	 		      delete [] CpInj;
	 		  }
	 		  if(DrhoDtInj){
	 			  for(int i=0; i<ndiv+2;i++) delete[] DrhoDtInj[i];
	 		      delete [] DrhoDtInj;
	 		  }
	      }
      if(nvecEmul>0){
    	  delete [] BSWVec;
    	  delete [] emulVec;
      }
      if(nsnp>0) delete [] tempsnp;
      if(nTcorrecaoMassaEspLiq>0){
    	  delete [] VcorrecaoMassaEspLiq;
      	  delete [] TcorrecaoMassaEspLiq;
      }
      if(nTsonico>0){
     	  delete [] Vsonico;
       	  delete [] Tempsonico;
       }

		if(nfluP>0){
			for(int i=0; i<nfluP;i++){
				if(flup[i].corrOM==7){
					if(tabVisc[i].parserie>0){
						delete [] tabVisc[i].visc;
						delete [] tabVisc[i].temp;
					}
				}
			}
			delete [] tabVisc;
		}
		if(nsegrega>0){
			delete [] vecTSegrega;
			delete [] vecSegrega;
		}

		if(oPressurePoints!=0) delete [] oPressurePoints;
		if(oCloudPointTemperatures!=0) delete [] oCloudPointTemperatures;
		if(oStructurePressures!=0) delete [] oStructurePressures;
		if(oStructureTemperatures!=0) delete [] oStructureTemperatures;
		/*if(oStructureTemperatures!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureTemperatures[i];
			delete [] oStructureTemperatures;
		}*/
		if(oStructureDensities!=0) delete [] oStructureDensities;
		/*if(oStructureDensities!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureDensities[i];
			delete [] oStructureDensities;
		}*/
		if(oStructureLiqMW!=0) delete [] oStructureLiqMW;
		/*if(oStructureLiqMW!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureLiqMW[i];
			delete [] oStructureLiqMW;
		}*/
		if(oStructureCPWax!=0) delete [] oStructureCPWax;
		/*if(oStructureCPWax!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureCPWax[i];
			delete [] oStructureCPWax;
		}*/
		if(oStructureThermCond!=0) delete [] oStructureThermCond;
		/*if(oStructureThermCond!=0){
			for(int i=0; i<iPresWax;i++) delete [] oStructureThermCond[i];
			delete [] oStructureThermCond;
		}*/
		if(oStructureWaxConcs!=0) delete [] oStructureWaxConcs;
		/*if(oStructureWaxConcs!=0){
			for(int i=0; i<iPresWax;i++){
				for(int j=0; i< iTempWax; i++) delete [] oStructureWaxConcs[i][j];
				delete [] oStructureWaxConcs[i];
			}
			delete [] oStructureWaxConcs;
		}*/
		if(oMolecularWeightsOfWaxComponents!=0) delete [] oMolecularWeightsOfWaxComponents;
		if(oLiquidDensitiesOfWaxComponents!=0) delete [] oLiquidDensitiesOfWaxComponents;
		if(oInterpolatedWaxConcs!=0) delete [] oInterpolatedWaxConcs;

		/*if(nestabCol>0){
			delete [] vecTestabCol;
			delete [] vecestabCol;
		}*/
		if(nintermi>0){
		  delete [] intermi;
		}
	}


	    void copiaSemJson(Ler&);
		void lerArq();//metodo onde sao organizados os parses de leitura de json e e feito o vetor com a struct de celulas
		//que depois, na montagem do objeto de SisProd auxiliara na construcao de objetos de celula3
		void copiaArq(Ler& arqAntigo);

		void geraduto();//metodo que transforma a struct do tipo detduto no objeto DadosGeo, que e o objeto usado em celula3

		void novatrans(TransCal& transfer/*objeto de trasnferencia de calor, e montado neste metodo*/,
				  DadosGeo& geom/*objeto com dados de geometria*/,
				  int* discre/*vetor com o numero de discretizacoes de cada camada da parede, dado obtido
				  de um objeto do tipo corte duto*/, double tint/*temperatura no interior do duto*/,
				  double text/*temperatura no ambiente externo*/,
				  double vext/*velocidade do ambiente externo*/,
				  double vint/*velocidade de escoamento interno,usado apenas para dar partida na construcao do objeto*/,
				  long dircon/*direcao da conveccao externa, se atacando o duto, ou oaralelo, informaclao retirada de
				  um objeto da struct decelp ou detcelg*/,
				  double dt/*valor de incremento de tempo, usado apenas para dar partida na construcao do objeto*/,
				  double ki/*condutividade do fluido interno, usado apenas para dar partida na construcao do objeto*/, double cpi, double rhoi, double visci,
				  double ke/*condutividade do ambiente externo*/,
				  double cpe/*calor especifico do ambiente externo*/,
				  double rhoe/*massa especifica do ambiente externo*/,
				  double visce/*viscosidade do ambiente externo*/,
				  int formac/*indicador se existe troca termica com formacao*/,
				  int litologia/*indice que indica qual litologia utlizar no vetor formacPoc*/,
				  int ambext/*indicador de qual e o ambiente externo, usuario, aereo, maritmo*/,
				  double pres/*pressao interno,usado apenas para dar partida na construcao do objeto*/,
				  double dx/*comprimento da celula vinculada ao objeto de transcal*/);//metodo que monta o objeto do tipo
		//TransCal que sera carregado nos objetos do tipo celula

		void geracelp(Cel* celula);//metodo qiue gera o vetor de objetos celulas de producao que sera usado em SisProd
		void geracelg(CelG* celula);//metodo qiue gera o vetor de objetos celulas de gas que sera usado em SisProd
		int coluanulini();//metodo que define onde se inicia na linha de producao o acoplamento teprmico com
		//a linha de servico
		int coluanulfim();//metodo que define onde termina na linha de producao o acoplamento teprmico com
		//a linha de servico
		int anulcoluini();//metodo que define onde se inicia na linha de servico o acoplamento teprmico com
		//a linha de producao
		int anulcolufim();//metodo que define onde termina na linha de servico o acoplamento teprmico com
		//a linha de producao

		int acopPriRedeParalelaini();//metodo que define onde se inicia nas linhas primarias o acoplamento teprmico com
		//as linha secundarias-redes paralelas
		int acopPriRedeParalelafim();//metodo que define onde termina nas linha primarias o acoplamento teprmico com
		//a linha secundarias-redes paralelas
		int acopSecRedeParalelaini();//metodo que define onde se inicia nas linhas primarias o acoplamento teprmico com
		//as linha secundarias-redes paralelas
		int acopSecRedeParalelafim();//metodo que define onde termina nas linha primarias o acoplamento teprmico com
		//a linha secundarias-redes paralelas

		void geraipr(Cel* celula);//gera os objetos fointe do tipo IPR para o SisProd
		void gerafgasVGL(Cel* celula);//gera os objetos fonte de gas devido a VGLs na linha de producao e
		//na linha de servico e os modelos de VGL no SispRod
		void gerafgasFonte(Cel* celula);//metodo que gera os objetos fonte de gas na linha de producao em SisProd
		void gerafliqFonte(Cel* celula);//metodo que gera os objetos fonte de liquido na linha de producao em SisProd
		void gerafmassFonte(Cel* celula);//metodo que gera os objetos fonte de massa na linha de producao em SisProd
		void gerafPoroRadFonte(Cel* celula);//metodo que gera os objetos fonte de meio Poroso Radial na linha de producao em SisProd
		void gerafPoro2DFonte(Cel* celula);//metodo que gera os objetos fonte de meio Poroso 2D na linha de producao em SisProd
		void geraFuro(Cel* celula);//metodo que gera os objetos de vazamento na linha de producao em SisProd
		void gerafBCS(Cel* celula);//metodo que gera os objetos BCSs na linha de producao em SisProd
		void gerafBVOL(Cel* celula);//metodo que gera os objetos de bomba volumetrica na linha de producao em SisProd
		void geraDPReq(Cel* celula);//metodo que gera os objetos de delta pressao na linha de producao em SisProd
		void funcRazCV(double abertura, detCV* cvCurv,int ncurvaCV,double cdchk,
				            double AreaTub, double& razarea);
		void geraValv(Cel* celula);//metodo que gera os objetos de valvulas na linha de producao em SisProd
		void geraMaster1(Cel* celula);//metodo que gera o objeto valvula Master1 na linha de producao em SisProd
		void geraMaster2(CelG* celula);//metodo que gvecArq[iSeq]=Ler(era o objeto valvula Master2 na linha de servico em SisProd
		void gerapresfim(double& presfim/*pressao a montante do choke de superficia*/,
				double& pGSup/*pressao a jusante do choke*/);//metodo que inicializa as pressoes a montente
		//e a jusante do choke
		void gerachokesup(choke& chokeSup);//metodo que cria o objeto choke de superficie no SisProd
		void gerachokeinj(ChokeGas& chokeInj);//metodo que cria o objeto choke de injecao no SisProd
		void geraIntermitencia(Cel* celula);//metodo que gera os objetos de avaliacao de intermitencia
		void indraz(int& ind/*valor retornado do momento na serie de tempo logo anterior ao momento que se quer avaliar*/,
				double& raz/*posicao relativa de tempo, entre a posicao ind e ind+1*/,
				double tempo/*momento que se quer avaliar na serie de tempo para se fazer a interpolacao*/,
				int parserie/*tamanho da serie de tempo*/,
				double* serietemp/*vetor com os tempos da serie de tempo*/);//metodo que obtem o valor interpolado
		//de uma variavel de uma serie de tempo, metodo usado internamente no metodo atualiza
		/*void atualiza(double& presfim, choke& chokeSup,
				      Cel* celula, CelG* celulaG, double& pGSup,
				      double& temperatura, double& presiniG,
				      double& tempiniG,double tempo);*/
		void atualiza(int inicio/*indica se o inicio do tramo e de conexao com outrpo tramo ou sem conexao*/,
				      int extrem/*indica se o final do duto tem conexao com outro duto de uma rede ou nao*/,
					  int anel/*indica se a linha de gas esta conecatada a um anel de GL*/,
					  choke& chokeSup/*objeto choke de superficie, e neste metodo que sua abertura e atualizada*/,
				      ChokeGas& chokeInj/*objeto choke de injecao, e neste metodo que sua abertura e atualizada*/,
				      Cel* celula/*vetor com as celulas da linha de producao*/,
					  CelG* celulaG/*vetor com as celulas da linha de gas*/,
					  double& pGSup/*pressao a jusante do choke de superficie*/,
				      double& temperatura/*temperatura no inicio da linha de producao,
				      caso condicao de contorno fechada*/, double& presiniG/*pressao de injecao de gas*/,
				      double& tempiniG/*temperatura de injecao de gas*/,
					  double& presE/*pressao de entrada do sistema de producao, condicao de contorno de pressao na entrada*/,
					  double& tempE/*temperatura de entrada do sistema de producao, condicao de contorno de pressao na entrada*/,
					  double& titE/*titulo de entrada do sistema de producao, condicao de contorno de pressao na entrada*/,
					  double& betaE/*beta de entrada do sistema de producao, condicao de contorno de pressao na entrada*/,
					  double tempo/*momento da simulacao*/, double dt=0);//metodo responsavel por atualizar
		//os estados de varios acessorios e condicoes de contorno do sistema de producao e de injecao
		void atualizaSonico(double tempo,int& vExpli);


//////////////////////////////////////////////////////////////////////////////////////////////////
//////Metodos que fazem a impressao de dados durante a simulacao////////////////////////////////////
		void listaevento(int inic=1,int extrem=1);
		/*bool comparaevento(LogEvento* e1, LogEvento* e2){
			if(e1->instante<e2->instante) return true;
			else return false;
		}*/
		//Vcr& operator=(const Vcr&);
		void geraevento(int inic=1,int extrem=1);


		double fqlst(Cel* const celula,int i, double tempo);
		double fqlstTot(Cel* const celula,int i, double tempo);
		double fqlwst(Cel* const celula,int i, double tempo);
		double fqgst(Cel* const celula,int i, double tempo);
		void imprimeProfile(Cel* const celula,  FullMtx<double>& flut,
				            double tempo, int indTramo, int nrede=-1);
		void imprimeProfileG(CelG* const celula,  FullMtx<double>& flut,
				             double tempo, int indTramo, int nrede=-1);
		void imprimeProfileTrans(Cel* const celula, int* length,
				                 double tempo, int indTramo, int nrede=-1);
		void imprimeProfileTransG(CelG* const celula, int* length,
				                  double tempo, int indTramo,int nrede=-1);
		void imprimeTrend(Cel* const celula,  double** flut, double tempo,
				          int trend, int linha);
		void imprimeTrendG(CelG* const celula,  double** flut, double tempo,
				           int trend, int linha, double velgaslift=0.);//aletracao5
		void imprimeTrendTrans(Cel* const celula,
				               double tempo,int trend);
		void resumoPermanente(Cel* const celula, CelG* const celulaG, double pGsup, double presiniG,int indTramo, int nrede=-1);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////Os metodos a seguir sao usados em lerArq no momento da construcao do vetor de structs de celulas de
		///producao e de gas a partir das unidades cadastradas no json e sua discretizacao, os metodos buscam
		///os indices de cada celula a partir das informacoes do comprimento da unidade mais a soma de sua discretiuzacao
		int buscaIndiceMeioP(double lverif);
		int buscaIndiceMeioG(double lverif);
		int buscaIndiceFrontP(double lverif);
		int buscaIndiceFrontG(double lverif);

		int buscaIndiceUnidade(int iniSeg, int nuni, double comp);
		int buscaIndiceUnidadeG(int iniSeg, int nuni, double comp);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void geraTabCp();//metodo em que a tabela de calor especifico em um model black oil usando dados do PVTSim
		//e montada
		void geraTabDrholDt();//metodo em que a tabela de derivada de massa especifica de liquido com a temperatura
		//em um model black oil usando dados do PVTSim e montada
		void geraTabInjCO2();//metodo em que as tabelas de propriedade do poco injetor, quando o fluido e um gas,
		//e montada
		void geraTabFlash(int flu,int var);//metodo que gera as tabelas de propriedades do fluido do PVTSim

		double interpolaTempEnvelope(double pres);


		std::string ler_linha_continua(std::ifstream& in);

		// definir o tipo da simulacao: POCO_INJETOR, TRANSIENTE, REDE
    tipoSimulacao_t tipoSimulacao;

  private:
    string arquivoLog;
    tipoValidacaoJson_t validacaoJson;

    Document parseSchema();
    JSON_entrada parseEntrada();

    void validateVsSchema(Document *schemaMrtDoc, Document *mrtDoc);

    void iniciarVariaveis();
    void iniciarVariaveisConstrutorDefault();

    //bool verificarUnicidade(int *identificadores, int tamanhoVetor, int maiorElemento);
    bool verificarUnicidade(const std::vector<int>& identificadores);
    int identificarMaterial(int identificador);
    int identificarCorte(int identificador);
    int identificarFormacao(int identificador);
    int identificarFluidoProducao(int identificador);
    int determinarLinhaErro(int posicaoErro);
    int determinarLinhaErro(const char* chaveJson);

    // Ma�todos para parse do arquivo de entrada MRT
    void parse_configuracao_inicial(JSON_entrada_configuracaoInicial& configuracao_inicial_json);
    void parse_condcont_pocinjec(JSON_entrada_CondicaoContPocInjec& condcont_json, int temFluido=0);
    void parse_tempo(JSON_entrada_tempo& tempo_json);
    //void parse_gravaMomento(JSON_entrada_gravaMomento& gravaMomento_json);
    //void parse_gravaMomento(Value& gravaMomento_json);
    void parse_materiais(JSON_entrada_material& material_json);
    void parse_corte(JSON_entrada_secaoTransversal& corte_json);
    void parse_tabela(JSON_entrada_tabela& tabela_json);
    void parse_parafina(JSON_entrada_parafina& parafina_json);
    void parse_fluidos_producao(JSON_entrada_fluidosProducao& fluidos_producao_json);
    void parse_fluido_complementar(JSON_entrada_fluidoComplementar& fluido_complementar_json);
    void parse_fluido_gas(JSON_entrada_fluidoGas& fluido_gas_json);

    void parse_unidades_producao(JSON_entrada_dutosProducao& duto_producao_json);
    void parse_unidades_producaoAmb(JSON_entrada_dutosProducao& duto_producao_json);
    void parse_unidades_servico(JSON_entrada_dutosServico& duto_servico_json);
    void parse_unidades_servicoAmb(JSON_entrada_dutosServico& duto_servico_json);
    void parse_hidrato(JSON_entrada_hidrato& hidrato_json);
    void parse_ipr(JSON_entrada_ipr& ipr_json);
    void parse_separador(JSON_entrada_separador& separador_json);
    void parse_correcao(JSON_entrada_correcao& correcao_json);
    void parse_chokeSup(JSON_entrada_chokeSup& chokeSup_json);
    void parse_chokeInj(JSON_entrada_chokeInj& chokeInj_json);
    void parse_master1(JSON_entrada_master1& master1_json);
    void parse_master2(JSON_entrada_master2& master2_json);
    void parse_valv(JSON_entrada_valvula& valvula_json);
    void parse_gasInj(JSON_entrada_gasInj& gasInj_json);
    void parse_fonte_gas(JSON_entrada_fonteGas& fonte_gas_json);
    void parse_fonte_liquido(JSON_entrada_fonteLiquido& fonte_liquido_json);
    void parse_fonte_PoroRadial(JSON_entrada_fontePoroRadial& fonte_poroRadial_json);
    void parse_fonte_Poro2D(JSON_entrada_fontePoro2D& fonte_poro2D_json);
    void parse_fonte_massa(JSON_entrada_fonteMassa& fonte_massa_json);
    //void parse_furo(Value& furo_json);
    void parse_furo(JSON_entrada_fontePressao& fontePressao_json);
    void parse_fontechk(JSON_entrada_fonteChoke& fontechk_json);
    void parse_pig(JSON_entrada_pig& pig_json);
    void parse_bcs(JSON_entrada_bcs& bcs_json);
    void parse_bomba_volumetrica(JSON_entrada_bombaVolumetrica& bomba_volumetrica_json);
    void parse_delta_pressao(JSON_entrada_deltaPressao& delta_pressao_json);
    void troca_gaslift(detVALVGL& valv1, detVALVGL& valv2);//alteracao2
    void parse_fonte_gaslift(JSON_entrada_fonteGasLift& fonte_gaslift_json);
    void parse_intermitencia(JSON_entrada_intermitenciaSevera& intermitencia_json);

    void parse_perfil_producao(JSON_entrada_perfilProducao& perfis_producao_json);
    void parse_perfil_servico(JSON_entrada_perfilServico& perfis_servico_json);
    void parse_perfis_trans_producao(JSON_entrada_perfisTransP& perfis_trans_producao_json);
    void parse_perfis_trans_servico(JSON_entrada_perfisTransS& perfis_trans_servico_json);

    void parse_tendencia_producao(JSON_entrada_tendP& tendencia_producao_json);
    void parse_tendencia_servico(JSON_entrada_tendS& tendencia_servico_json);
    void parse_tendencia_trans_producao(JSON_entrada_tendTransP& tendencia_trans_producao_json);
    void parse_tendencia_trans_servico(JSON_entrada_tendTransS& tendencia_trans_servico_json);

    void parse_tela(JSON_entrada_tela& tela_json);


    void copia_configuracao_inicial(Ler& arqAntigo);
    void copia_condcont_pocinjec(Ler& arqAntigo);
    void copia_tempo(Ler& arqAntigo);
    void copia_materiais(Ler& arqAntigo);
    void copia_corte(Ler& arqAntigo);
    void copia_tabela(Ler& arqAntigo);
    void copia_parafina(Ler& arqAntigo);
    void copia_fluidos_producao(Ler& arqAntigo);
    void copia_fluido_complementar(Ler& arqAntigo);
    void copia_fluido_gas(Ler& arqAntigo);
    void copia_unidades_producao(Ler& arqAntigo);
    void copia_unidades_servico(Ler& arqAntigo);
    void copia_hidrato(Ler& arqAntigo);
    void copia_ipr(Ler& arqAntigo);
    void copia_separador(Ler& arqAntigo);
    void copia_correcao(Ler& arqAntigo);
    void copia_chokeSup(Ler& arqAntigo);
    void copia_chokeInj(Ler& arqAntigo);
    void copia_master1(Ler& arqAntigo);
    void copia_master2(Ler& arqAntigo);
    void copia_valv(Ler& arqAntigo);
    void copia_gasInj(Ler& arqAntigo);
    void copia_fonte_gas(Ler& arqAntigo);
    void copia_fonte_liquido(Ler& arqAntigo);
    void copia_fonte_PoroRadial(Ler& arqAntigo);
    void copia_fonte_Poro2D(Ler& arqAntigo);
    void copia_fonte_massa(Ler& arqAntigo);
    void copia_furo(Ler& arqAntigo);
    void copia_fontechk(Ler& arqAntigo);
    void copia_pig(Ler& arqAntigo);
    void copia_bcs(Ler& arqAntigo);
    void copia_bomba_volumetrica(Ler& arqAntigo);
    void copia_delta_pressao(Ler& arqAntigo);
    void copia_fonte_gaslift(Ler& arqAntigo);
    void copia_intermitencia(Ler& arqAntigo);
    void copia_perfil_producao(Ler& arqAntigo);
    void copia_perfil_servico(Ler& arqAntigo);
    void copia_perfis_trans_producao(Ler& arqAntigo);
    void copia_perfis_trans_servico(Ler& arqAntigo);
    void copia_tendencia_producao(Ler& arqAntigo);
    void copia_tendencia_servico(Ler& arqAntigo);
    void copia_tendencia_trans_producao(Ler& arqAntigo);
    void copia_tendencia_trans_servico(Ler& arqAntigo);
    void copia_tela(Ler& arqAntigo);
    ////////////////////////////////////////////////////////////////////////////////

};




#endif /* LEITURA_H_ */
