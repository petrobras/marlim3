/*
 * SisProd.h
 *
 *  Created on: 21 de dez de 2016
 *      Author: Eduardo
 */

#ifndef SISPROD_H_
#define SISPROD_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

//#include "FA_Hidratos.h"  // InclusÃ£o do solver de hidratos // chris

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;
//#include <ctime.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include "Vetor.h"
#include "Matriz.h"
#include "FerramentasNumericas.h"
#include "chokegas.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Geometria.h"
#include "celula3.h"
#include "estrat.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "Acidentes2.h"
#include "acessorios.h"
#include "celulaGas.h"
#include "TrocaCalor.h"
#include "Leitura.h"
#include "mapa.h"
#include "Log.h"
#include "estruturaTabDin.h"
#include "variaveisGlobais1D.h"
#include "dados3DPoisson.h"
#include "solver3DPoisson.h"
#include "GradientCorrelations.h"
#include "criterioIntermiSevera.h"

/*extern double lixo5;// variavel global, tempo na simulacao para um so tramo;
extern double lixo5R;//variavel global, tempo na simulacao geral para todos os tramos de uma rede
extern int contador;//variavel global, contador de passos de tempo, o mesmo que o atributo kSP;
extern double localtiny;//pequeno valor, usado como criterio para transicao entre multifasico e monofasico
extern double CritCond;//variavel global, indica o menor valor de fracao de vazio ou holdup em que a
//transferencia de massa e considerada em uma simulacao transiente
//extern double momentoDesesp;
extern double CritDTMin;
extern int contaExit;//variavel global, atualmente sem funcao nesta classe;
extern int chaverede;//variavel global, indica se se esta simulando um tramo solteiro ou pertencente a uma malha de dutos;
extern int fluidoRede;
extern int chaveAnelGL;//variavel global, indica se o tramo faz parte de um anel de GL;
extern int iterRede;//variavel global, indica o numero de iteracoes no processo de convergencia permanente de rede;
//retorna um valor indicativo de falha de converegencia para o tramo, em vez de parar a simulacao;
extern int iterRedeT;//sem funcao nesta classe
extern int qualRede;
extern int qualTramo;
extern int verificaTramo;
extern int chaveredeT;//sem funcao nesta classe
extern double TmaxR;//variavel global, indica o tempo maximo de simulacao definido pelo usuario
//quando preenchendo um arquivo de rede;
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
//extern double** HLat;
//extern double** drholdT;
//extern int CalcLat;
//extern int ndiv;
//extern int npontos;
//extern int trackRGO;
//extern int trackDeng;
//extern int ninjgas;
//extern int lingas;
//extern int nfluP;
//extern double dengG;
//extern double dengP;
extern double RGOMax;//variavel global, indica o valor de RGO maxima utilizada para se evitar
//eventuais valores infinitos de RGO local quando o holdup fica igual a zero em uma simulacao transiente
//extern int injPoc;
//extern double** RhoInj;
//extern double** ViscInj;
//extern double** CondInj;
//extern double** CpInj;

// obter string do prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;

// relatorio dos arquivos de dados de saida da simulacao
extern ofstream arqRelatorioPerfis;

// criar objeto para log da aplicacao
extern Logger logger;

extern int numthreads;
extern int ntrd;

extern int simulaTransiente;
extern int tipoFluidoRedeGlob;

extern int sequenciaAS;

extern int modoTransiente;*/

//extern char* saidaTexto;
//extern char* saidaSubTexto;

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

extern string versao;

extern time_t nowGlobIni;
extern tm *ltmGlobIni;

extern int diaIni;
extern int horaIni;
extern int minutoIni;
extern int segundoIni;

extern time_t nowGlobFim;
extern tm *ltmGlobFim;

class SProd {
  public:

  int ModelCp;//indica qual modelo de calor especifico usar quando em um modelo black-oil, se do proprio black oil
  //ou de uma tabela PVT. Esta indicacao e feita na leitura do json e depois repassada para este atributo
  int Modeljtl;//indicaqual modelo de JouleThomson de liquido usar em um modelo black-oil, se do proprio black oil
  //ou de uma tabela PVT. Esta indicacao e feita na leitura do json e depois repassada para este atributo
  int CalcLat;//Tambem para o caso black oil, indica se se deseja fazer o calculo de calor latente
  int trackRGO;//indica se se deseja fazer o rastreamento das propriedades primitivas de um modelo black oil usando
  //equacoes de transporte para variaveis como API, BSW, RGO massa da fracao leve e da pesada;
  int trackDeng;//como a anterior, aplica uma equacao de transporte para a densidade do gas e sua fracao de CO2;
  int ninjgas;//numero de fontes de gas, na verdade uma variavel redundante, pois no objeto arq (classe de leitura) ja
  //existe um ninjgas=>arq.ninjgas;
  int lingas;//indicador se existe uma linha de gas acoplada ao sistema
  int injPoc;//indicador de que a simulacao e de poco de injecao

  int indTramo;//indice de tramo para o caso do objeto tramo montado a partir desta
  //classe estar inserido em uma malha de dutos
  int ncel;//numero de volumes do sistema de producao
  int reinicia;//indicador da necessidade de se reavaliar o incremento de tempo devido a
  //algum volume com holup maior que 1 ou menor que zero;
  //int extrem;

  double trocaTermicaLenta;

  double vRazMast1[10];
  double vRazMast0[10];
  double vRazMastCrit[10];

  double presE;//pressao de entrada para condicao de contorno pressao na entrada do duto;
  double tempE;//temperatura de entrada para condicao de contorno pressao na entrada do duto;
  double titE;//titulo de entrada para condicao de contorno pressao na entrada do duto;
  double alfE;//fracao de vazio de entrada para condicao de contorno pressao na entrada do duto, calculado;
  double betaE;//beta de entrada de entrada para condicao de contorno pressao na entrada do duto, calculado;
  double presEini;//pressao de entrada para condicao de contorno pressao na entrada do duto;
  double tempEini;//temperatura de entrada para condicao de contorno pressao na entrada do duto;
  double titEini;//titulo de entrada para condicao de contorno pressao na entrada do duto;
  double alfEini;//fracao de vazio de entrada para condicao de contorno pressao na entrada do duto, calculado;
  double betaEini;//beta de entrada de entrada para condicao de contorno pressao na entrada do duto, calculado;
  double presfim;//pressao no ultimo volume, quando choke de superficie esta aberto, esta pressao
  //e igual a pressao do separador;
  double presfimini;
  double titRev;//valor do titulo quando ocorre retorno de escoamento no ultimo volume,
  //so utilizado em problemas transientes de rede;
  double titRevini;
  double betaRev;//valor de beta quando ocorre retorno de escoamento no ultimo volume,
  //so utilizado em problemas transientes de rede;
  double betaRevini;

  double pGSup;//pressao do separador ou pressao no inicio de um tramo a jusante do tramo em questao
  //caso de rede;
  double pGSupIni;
  double tGSupIni;
  double tGSup;//temperatura do separador ou temperatura do inicio do tramo a jusante, caso de rede;
  double temperatura;//temperatura na entrada do duto, quando nao se tem
  //uma condicao de contorno de pressao na entrada;

  double masSup;//atributo sem uso nesta classe neste momento
  double tempSup;//atributo sem uso nesta classe neste momento

  int ncelGas;//numero de volumes na linha de gas
  double presiniG;//pressao de injecao de gas;
  double tempiniG;//temperatura em que o gas e injetado na linha de servico
  double massfonte;//momentaneamente sem uso;

  double mult;//multiplicador de seguranca do CFL, atualmente mult=0.8;
  double presMedMov;//media temporal da pressao no ultimo volume do sistema,
  //usada para definir se o choke deve ser tratado como uma perda localizada ou
  //uma funcao que retorna a vazao de descarga;
  double jMedMov;//media temporal do fluxo volumetrico da mistura no ultimo volume do sistema,
  //usada para definir se o choke deve ser tratado como uma perda localizada ou
  //uma funcao que retorna a vazao de descarga;
  double alfMedMov;//media temporal da fracao de vazio no ultimo volume do sistema,
  //usada para definir se o choke deve ser tratado como uma perda localizada ou
  //uma funcao que retorna a vazao de descarga;
  double tMedMov;//tempo em que se inicia o calculo das medias temnporais no ultimo volume do sistema,
  //usada para definir se o choke deve ser tratado como uma perda localizada ou
  //uma funcao que retorna a vazao de descarga;
  double ktMedMov;//intervalo de tempo em que se faz o calculo das medias temnporais no ultimo volume do sistema,
  //usada para definir se o choke deve ser tratado como uma perda localizada ou
  //uma funcao que retorna a vazao de descarga;
  double pTotal;//variavel auxiliar para o calculo de presMedMov
  double jTotal;//variavel auxiliar para o calculo de jMedMov
  double alfTotal;//variavel auxiliar para o calculo de alfMedMov
  vector<double> presVet;//vetor auxiliar para o calculo de presMedMov
  vector<double> jVet;//vetor auxiliar para o calculo de jMedMov
  vector<double> alfVet;//vetor auxiliar para o calculo de alfMedMov
  vector<double> tVet;//vetor auxiliar para o calculo de ktMedMov

  int aberto;//indicador de que o choke de superficiue esta aberto;
  int abertoini;
  int tempoaberto;//contador de controle para sair de um modo choke ativo para inativo, so apos 60 passos de tempo;
  int tempoabertoini;
  int EstadoMaster1;//indicador se o choke master1 esta ativo ou nao;
  int contaMaster1;//contador de controle de mudanca de estado, quando a master1 fecha;
  int masChkSup;//indicador de que o choke de superficie esta ativo;
  int masChkSupini;
  int mudaModoChk;//alarme de mudanca de estado do choke de estado do choke de superficie;
  int mudaModoChkini;
  int TransMassModel;//modelo de transferencia de massa 0-> modelo completo; 1-> modelo totalmente explicito
  //2-modelo simplificado; 3->sem transferencia de massa;
  int indpigP;//numero de pigs se deslocando na linha;
  int indpigPini;
  int npig;//numero de pigs previstos para serem lancados na linha;
  int* receb;//posicao, indice, em que o pig e recebido;

  int npontos;//numero de pontos de tabela de propriedade de fluidos;
  int nfluP;//numero de fluidos de producao;
  double** zdranP;//tabela com valores do fator de compressibilidade, para modelo black oil;
  //double** zdranG;//alteracao2
  double** dzdpP;//tabela com valores da derivada em pressao do fator de compressibilidade, para modelo black oil;
  //double** dzdpG;//alteracao2
  double** dzdtP;//tabela com valores da derivada em temperatura do fator de compressibilidade, para modelo black oil;
  //double** dzdtG;//alteracao2
  //double dengG;//alteracao2
  //double dengP;//alteracao2
  double** cpg;//tabela com valores do calor especifico de gas, para modelo black oil;
  double** cpl;//tabela com valores do calor especifico de liquido produzido, para modelo black oil;
  double** drholdT;//tabela com valores da derivada da massa especifica de liquido com a temperatura,
  //para modelo black oil;
  double** HLat;//tabela de calor latente, para modelo black oil;

  Ler arq;//objeto produzido a partir da classe leitura com as informacoes do usuario;
  FullMtx<double> flutG;//matriz buffer para gravacao de perfis de grandezas fisicas da linha de gas em arquivos txt;
  FullMtx<double> flut;//matriz buffer para gravacao de perfis de grandezas fisicas da
  //linha de producao em arquivos txt;
  BandMtx<double> matglobG;//matriz global acompclamento pressao - velocidade linha de gas;
  Vcr<double> termolivreG;//vetor livre/solucao do sistema linear do acoplamento pressao-vel lin gas;
  BandMtx<double> matglobP;//matriz global acompclamento pressao - velocidade linha multifasica;
  Vcr<double> termolivreP;//vetor livre/solucao do sistema linear do acoplamento pressao-vel lin multifasica;
  double dt;//incremento de tempo;
  double dtini;//incremeto de tempo da camada de tempo anterior ao avanco atual;
  double tfinal;//tempo final de simulacao;
  int* posicVGLP;//vetor com as posicoes, indices, das VGLs no sistema de producao;
  int* posicVGLG;//vetor com as posicoes, indices, das VGLs na linha de servico;
  int nabreM1;//numero de momentos em que a master1 abre na simulacao(e feito um controle de tempo nestas abverturas);
  int nfechaM1;//numero de momentos em que a master1 fecha na simulacao(e feito um controle de tempo nesttesfechamentos);
  double *fechaM1;//vetor com os momentos em que a master1 fecha;
  double *abreM1;//vetor com os momentos em que a master1 abre;
  int* ncelperftransg;//posicoes onde os perfis de temperatura radiais sao gravados;
  int* TrendLengthG;//vetor com o tamanho maximo das matrizes de tendencias na linha de servico;
  double*** MatTrendG;//conjunto de matrizes com os resultados de tendencia na linha de gas,
  //funcionam como um buffer de dados para serem gravados;
  double* resettrendg;//contador de tempo em que o arquivo buffer e zerado para uma nova batelada de informacao;
  int* ntrendg;//vetor com o numero de pontos de tendencia de gas gravados
  int* ntrendgB;//vetor com o numero de pontos de tendencia de gas gravados antes da ultima gravacao
  int* TrendLengthTransG;//vetor com o tamanho maximo das matrizes de tendencias na linha de servico;
  double*** MatTrendTransG;//conjunto de matrizes com os resultados de tendencia da temperatura de parede em uma posicao da linha de gas,
  //funcionam como um buffer de dados para serem gravados;
  double* resettrendtransg;//contador de tempo em que o arquivo buffer e zerado para uma nova batelada de informacao;
  int* ntrendtransg;//vetor com o numero de pontos de tendencia de temperatura em um ponto da parede de tubulacao da linha de gas gravados
  int* ntrendtransgB;//vetor com o numero de pontos de tendencia gravados antes da ultima gravacao
  int* ncelperftransp;//posicoes onde os perfis de temperatura radiais sao gravados, linha de producao;
  int* TrendLengthP;//vetor com o tamanho maximo das matrizes de tendencias na linha de producao;
  double*** MatTrendP;//conjunto de matrizes com os resultados de tendencia na linha de producao,
  //funcionam como um buffer de dados para serem gravados;
  double* resettrend;//contador de tempo em que o arquivo buffer e zerado para uma nova batelada de informacao;
  int* ntrend;//vetor com o numero de pontos de tendencia de producao gravados
  int* ntrendB;//vetor com o numero de pontos de tendencia de producao gravados antes da ultima gravacao
  int* TrendLengthTransP;//vetor com o tamanho maximo das matrizes de tendencias na linha de producao;
  double*** MatTrendTransP;//conjunto de matrizes com os resultados de tendencia da temperatura de parede em uma posicao da linha de producao,
  //funcionam como um buffer de dados para serem gravados;
  double* resettrendtrans;//contador de tempo em que o arquivo buffer e zerado para uma nova batelada de informacao;
  int* ntrendtrans;//vetor com o numero de pontos de tendencia de temperatura em um ponto da parede de tubulacao da linha de producao gravados
  int* ntrendtransB;//vetor com o numero de pontos de tendencia gravados antes da ultima gravacao;
  int AnulaColunaIni;//indice na linha de servico em que se inicia o acoplamento termico com a coluna;
  int AnulaColunaFim;//indice na linha de servico em que termina o acoplamento termico com a coluna;
  int ColunaAnulaIni;//indice na coluna na posicao coincidente com AnulaColunaIni;
  int ColunaAnulaFim;//indice na coluna na posicao coincidente com AnulaColunaFim;
  int verificaAcop;//indicador de que existe acoplamento termico no modelo;
  int SecPrimIniRedeP;//indice na linha secundaria em que se inicia o acoplamento termico com a primaria em rede paralela;
  int SecPrimFimRedeP;//indice na linha secundaria em que termina o acoplamento termico com a primaria em rede paralela;
  int PrimSecIniRedeP;//indice na primaria na posicao coincidente com SecPrimIni em rede paralela;
  int PrimSecFimRedeP;//indice na primaria na posicao coincidente com SecPrimFim em rede paralela;
  int verificaAcopRedeP;//indicador de que existe acoplamento termico no modelo em rede paralela;
  int verificaAcopRedeS;
  int kontaTempoProf;//contador que indica qual perfil de producao que sera impresso em um determinado momento;
  int kontaTempoProfG;//contador que indica qual perfil de gas que sera impresso em um determinado momento;
  int kontaTempoTransProf;//contador que indica qual perfil de temperatura na secao sera impresso em um determinado momento;
  int kontaTempoTransProfG;//contador que indica qual perfil de temperatura na secao sera impresso em um determinado momento;
  ostringstream saidaLog;//arquivo de saida para o arquivo Log;
  string tmpLog;//nome do arquivo Log;
  int contaLog;//contador de eventos para impressao no arquivo LogEventos, quando atinge o numero maximo de eventos, para de imprimir;
  //double masCL;
  //double masCG;
  //double gasinj;
  double menorDx;//menor comprimento de volume do sistema de producao;
  int iterperm;//numero de iteracoes no algoritmo de busca de chutes para iniciar
  //o metodo de calculo de zero de funcao do solver permanente;
  int kSP;//numero de passos de tempo;
  int KontaImprime;//contador utilizado para determinar a frequencia de impressao no arquivo Log;
  int indevento;//contador usado no vetor que contem os tempos em que os eventos ocorrem na simulacao
  //quando o tempo se aproxima de algum evento, indicado por este contador,
  //o incremento de tempo e ajustado para que nao passe muito do inicio do evento;
  int modoPerm;//variavel atualmente nao utilizada no simulador;
  int modeloCompleto;
  int modeloCompleto0;
  int kontaMudaModelo;


  vector<double> dtSim;//vetor com uma sequencia dos incrementos de tempo mais recentes;
  vector<double> dtCFL;//vetor com os incrementos de tempo indicados pelo criterio CFL,
  //nao necessariamente o incremento de tempo utilizado;
  double dtCFLMed;//o valor medio do incremento de tempo se fosse utilizado apenas o criterio CFL;
  double dtSimMed;//valor medio do incremento de tempo de fato utilizado no simulador;
  int restriDt;//indicador da necessidade de restricao do aumento de incremento de tempo, quando este valormedio
  //difere muito do valor medio CFL;
  int kontarestriDt;//contador de passo de tempo inicando por quantos passos de tempo o imncremento deve se manter restringido;
  int kontarestriSegrega;
  double dtCFLTotal;//variavel auxiliar para o calculo de dtCFLMed e de dtSimMed;
  double dtSimTotal;//variavel auxiliar para o calculo de dtCFLMed e de dtSimMed;
  double dtauxCFL;//variavel auxiliar para o calculo de dtCFLMed e de dtSimMed;
  double dtauxFinal;//variavel auxiliar para o calculo de dtCFLMed e de dtSimMed;
  int kontaGolfada;//contador de passos de tempo quando se observa osciulacoes de vazao de liquido no ultimo volume,
  // quando este se encontra com um choke ativo. Estas oscilacoes ficam mudando de positivo para negativo de um passo de tempo para outro;

  choke chokeSup;//objeto choke de superfpicie;
  ChokeGas chokeInj;//objeto choke de injecao na linha de gas;
  ChokeGas* chokeVGL;//vetor com as VGLs existentes no sistema;
  CelG* celulaG; //vetor com os volumes/celulas da linha de gas, celulas em sequencia, representando o sistema de linha de gas;

  Cel* celula;//vetor com os volumes/celulas da linha de producao, celulas em sequencia, representando o sistema de linha de producao;
  double *PBPVTSim;//tabela com a pressao de bolha obtida pelo PVTSim,
  //uso exclusivo para quando a correlacao de Livia e utilizada para o calculo de RS,
  //nao confundir com o uso puro da tabela de propriedades do PVTSim;
  double *TBPVTSim;//tabela com a temperatura de bolha obtida pelo PVTSim,
  //uso exclusivo para quando a correlacao de Livia e utilizada para o calculo de RS,
  //nao confundir com o uso puro da tabela de propriedades do PVTSim;
  double **RSLivia;//tabela com as razoes de solubilidade para a correlacao de Livia,
  //esta correlacao e calculada internamente, mas como exige um grande esforco computacional,
  //monta-se uma tabela para acelerar o desempenho da simulacao;
  int LerPB;//indicador que mesmo utilizando um modelo black oil, se lera uma tabela de pressao e temperatura de bolhas;
  int lerRS;//indicador que mesmo utilizando um modelo black oil, se lera uma tabela de razao de solubilidade;


  int celInter;//para o caso de descarga de GL,
  //celula da linha de servico onde se encontra a interface fluido de completacao/gas;
  double dtInter;//para o caso de descarga de GL,
  //incremento de tempo manimo para que a interface fluido de completacao/gas nao ultrapasse uma celula da linha de gas;
  double velInter;//para o caso de descarga de GL,
  //velocidade de deslocamento da interface fluido de completacao/gas;
  int celInterIni;//para o caso de descarga de GL,
  //celula inicial (inicio da simulacao) da linha de servico onde se encontra a interface fluido de completacao/gas;
  double dtInterIni;//para o caso de descarga de GL,
  //incremento de tempo manimo para que a interface fluido de completacao/gas nao ultrapasse uma celula da linha de gas;
  //valor da camada de tempo anterior, utilizada quando por algum motivo e necessario reiniciar a simulacao;
  double velInterIni;//para o caso de descarga de GL,
  //velocidade de deslocamento da interface fluido de completacao/gas na camada de tempo anterior, utilizado quando
  //e necessario reibiciar a simulacao;
  double tempMedContDesc;//parametro de controle do processo de descarga
 // para impedir que a velocidade erosional na vgl seja atingida;
  double maxVecContDesc;//tamanho maximo do vetor de vazoes que sao guardados para se fazer
  //a integracao da variavel no controle PI para se evitar a condicao de velocidade erosional na VGL durante a descarga;
  double vazmedDesc;//valor medio da maior vazao massica de fluido de completacao passando por uma das VGLs ;
  double tempmedDEsc;//intervalo de tempo em que e feito o calculo da vazao massica media passando por uma VGL;
  vector<double> vazmaxMedDesc;//vetor em que se e guardada as vazoes massicas na VGL para o calculo de seu valor medio;
  vector<double> dtDesc;//vetor com os incrementos de tempo nos passos de tempo em que se faz o calculo dos valores medios de descarga;

  int noextremo;//indicador, para o caso de simulacao de rede de que o ultimo volume do tramo nao se conecta com outro tramo
  int noinicial;//indicador, para o caso de simulacao de rede de que o primeiro volume do tramo nao se conecta com outro tramo
  int derivaAnel;//indicador de que o tramo em questao faz parte de uma rede de anel de GL e e uma das derivacoes deste anel;

  //o calculo de uma rede transiente e feita em duas iteracoes, na primeira para um tramo interno,
  //e feita uma estimativa das fontes de massa que alimentam o inicio do tramo, esta estimativa intermnediaria e dada
  //por:
  double fontemassPRBuf;//estimativa da vazao do liquido de producao alimentando um tramo interno em uma rede
  double fontemassCRBuf;//estimativa da vazao do liquido de complementar alimentando um tramo interno em uma rede
  double fontemassGRBuf;////estimativa da vazao de gas alimentando um tramo interno em uma rede

  int redeTemporario;//variavel atualmente nao utilizada, sempre com valor zero;
  //double massliqRev;//alteracao8

  double kimpT;//contador dos ciclos de impressao dos arquivos de tendencia, se kimpT=1, antes de imprimir
  //os dados da tendencia, imprime o cabecalho do arquivo;

  double momentoDesesp;//multiplicador especial para fechamento da Master1. O simulador tem limitacoes de modelo em
  //condicoes de valvula fechada, quando a montante tem apenas liquido, e uma especie de "ponto cego" de pressao
  // para evitar esta situacao em fechamento de Master1, quando se verifica este risco, o incremento de tempo e restringido
  //e uma vazao de gas artificial, pequena, e imposta a montante da master1, se o problema se mantem,
  //um multiplicador para esta vazao de gas e aplicado e aumenta a cada passo de tempo, este atributo e o multiplicador;

  vector<double> taxaDpMax;
  double DpMaxMed;

  vector<double> taxaDTMax;
  double DTMaxMed;

  double chuteHol;

  int buscaIni;

  vector<tabelaDinamica> tabDin;
  int ntabDin;

  int kontaRenovaComp;

  int bloq;

  ProFlu fluiRevRede;
  double tempRev;
  int revPerm;
  vector<int> acertaIndAcop;
  varGlob1D* vg1dSP;

  solverP3D poisson3D;

  double dtCicMin;

  vector<int> indCelPoisson2D;
  int nCelulaPoisson2D;

  int semTermo;
  double monitConvPerm;
  double monitConvPermBase;

  int alteraTempo;

  vector<int> indFonteRedeParalelaIni;
  vector<double> fonteMpRedeParalelaIni;
  vector<double> fonteMcRedeParalelaIni;
  vector<double> fonteMgRedeParalelaIni;
  int redeParalelaCCsecundario;
  int redeParalelaP;
  int redeParalelaS;

  const char* saidaTextoSis[12]={"                          Post Coitum Omine Animal Triste Est                   ",
			"           'Ouca-me. O fim quase nunca esta longe, em nenhum momento!'          ",
			"      So nos curamos de um sofrimento depois de o haver suportado ate o fim.    ",
			"                   Infeliz e o espirito ansioso pelo futuro.                    ",
			"                                    Memento Mori                                ",
			" Somente um progresso calmo e constante, livre de precipitacao, conduz ao objetivo.",
			"             Paciencia, nove mulheres nao conseguem gerar uma crianca em um mes. ",
	        "                  A necessidade e a mae da inovacao, mas a paciencia e o pai    ",/*, dando tempo e banana, qualquer macaco faz um projeto"*/
	"O sucesso nao e uma linha reta, e um jogo de resistencia, e cada tropeco e apenas um degrau a mais para a vitoria!",
	"                        Quem vive de navegar, o vento e quem lhe comanda                ",
	"    Uma vez me perguntaram o que achava da passagem do tempo, e eu disse: sou contra    ",
	"                 Nao importa o quanto voce va devagar, desde que nao pare                "};
  const char* saidaSubTextoSis[12]={
		  "                         Galeno de Pergamo do Transiente Longo                          ",
		  "                     J. California Cooper depois da simulacao divergir                  ",
		  "                                Marcel Proust no CrossFit                               ",
		  "                              Seneca do Mindfulness                                     ",
		  "                                   Zuleica da Funeraria                                 ",
		  "                                       China In Box                                     ",
		  "                                      Tiao do Linkedin                                  ",
		  "                                      Marcao da Oficina                                 ",
		  "                                    Mario Pascal do Insta                               ",
		  "                          Seu Pereira na feira de artesanatos numericos                 ",
		  "                        Luis Fernando Verissimo das Simulacoes Permanentes              ",
		  "                             Confucio vendo a simulacao emperrar                       "};


  SProd(string nomeArquivoEntrada, string nomeArquivoLog, tipoValidacaoJson_t validacaoJson,
      tipoSimulacao_t tipoSimulacao,varGlob1D* Vvg1dSP=0, int TD=-1, int vbloq=0, int temporario=0/*variavel muda*/,
	  int reverso=0,
	  /*variaveis usadas apenas em anel de GL, quando o duto e o anel ->*/
	  double* compfonte=0 /*vetor de posicoes das derivacoes do anel, valor de entrada*/,
	  int* posicfonte=0/*vetor de indice das derivacoes do anel, valor retornado*/,
	  int nfontes=0/*numero de derivacoes, valor de entrada*/,
	  int redeperm=1);
  SProd();
  ~SProd() {    //destrutor
    if(arq.lingas>0) delete [] celulaG;
    if(chokeVGL && arq.lingas>0) delete [] chokeVGL;
    if(posicVGLP && arq.lingas>0) delete [] posicVGLP;
    if(posicVGLG && arq.lingas>0) delete [] posicVGLG;
    if(nabreM1>0) delete [] abreM1;
    if(nfechaM1>0) delete [] fechaM1;

    if(arq.nperfistransp>0) delete [] ncelperftransp;
    if(arq.nperfistransg>0 && arq.lingas>0) delete [] ncelperftransg;

   if(arq.ntendp>0 && redeTemporario==0) {
      for(int i=0;i<arq.ntendp;i++) {
        for(int j=0; j<TrendLengthP[i]; j++) //if(MatTrendP[i][j]) delete [] MatTrendP[i][j];
        	if(MatTrendP)delete [] MatTrendP[i][j];
        //if(MatTrendP[i])delete [] MatTrendP[i];
        if(MatTrendP)delete [] MatTrendP[i];
      }
      //if(MatTrendP)delete [] MatTrendP;
      if(MatTrendP)delete [] MatTrendP;
      //if(TrendLengthP) delete [] TrendLengthP;
      if(TrendLengthP)delete [] TrendLengthP;
      //if(resettrend) delete [] resettrend;
      if(resettrend)delete [] resettrend;
      //if(ntrend) delete [] ntrend;
      if(ntrend)delete [] ntrend;
      if(ntrendB)delete [] ntrendB;
    }

    if(arq.ntendg>0 && arq.lingas>0 && redeTemporario==0) {
      for(int i=0;i<arq.ntendg;i++) {
        for(int j=0; j<TrendLengthG[i]; j++) //if(MatTrendG[i][j]) delete [] MatTrendG[i][j];
        	if(MatTrendG)delete [] MatTrendG[i][j];
             //if(MatTrendG[i])delete [] MatTrendG[i];
            if(MatTrendG)delete [] MatTrendG[i];
      }
      //if(MatTrendG)delete [] MatTrendG;
      if(MatTrendG)delete [] MatTrendG;
      //if(TrendLengthG) delete [] TrendLengthG;
      if(TrendLengthG)delete [] TrendLengthG;
      //if(resettrendg) delete [] resettrendg;
      if(resettrendg)delete [] resettrendg;
      //if(ntrendg) delete [] ntrendg;
      if(ntrendg)delete [] ntrendg;
      if(ntrendgB)delete [] ntrendgB;
    }
    if(arq.ntendtransp>0 && redeTemporario==0) {
      for(int i=0;i<arq.ntendtransp;i++) {
        for(int j=0; j<TrendLengthTransP[i]; j++)
        //if(MatTrendTransP[i][j]) delete [] MatTrendTransP[i][j];
        //if(MatTrendTransP[i])delete [] MatTrendTransP[i];
        	if(MatTrendTransP)delete [] MatTrendTransP[i][j];
            if(MatTrendTransP)delete [] MatTrendTransP[i];
      }
      //if(MatTrendTransP)delete [] MatTrendTransP;
      if(MatTrendTransP)delete [] MatTrendTransP;
      //if(TrendLengthTransP) delete [] TrendLengthTransP;
      if(TrendLengthTransP)delete [] TrendLengthTransP;
      //if(resettrendtrans) delete [] resettrendtrans;
      if(resettrendtrans)delete [] resettrendtrans;
      //if(ntrendtrans) delete [] ntrendtrans;
      if(ntrendtrans)delete [] ntrendtrans;
      if(ntrendtransB)delete [] ntrendtransB;
    }

    if(arq.ntendtransg>0 && redeTemporario==0) {
      for(int i=0;i<arq.ntendtransg;i++) {
        for(int j=0; j<TrendLengthTransG[i]; j++)
        //if(MatTrendTransP[i][j]) delete [] MatTrendTransP[i][j];
        //if(MatTrendTransP[i])delete [] MatTrendTransP[i];
        	if(MatTrendTransG)delete [] MatTrendTransG[i][j];
            if(MatTrendTransG)delete [] MatTrendTransG[i];
      }
      //if(MatTrendTransP)delete [] MatTrendTransP;
      if(MatTrendTransG)delete [] MatTrendTransG;
      //if(TrendLengthTransP) delete [] TrendLengthTransP;
      if(TrendLengthTransG)delete [] TrendLengthTransG;
      //if(resettrendtrans) delete [] resettrendtrans;
      if(resettrendtransg)delete [] resettrendtransg;
      //if(ntrendtrans) delete [] ntrendtrans;
      if(ntrendtransg)delete [] ntrendtransg;
      if(ntrendtransgB)delete [] ntrendtransgB;
    }

    int ndiv = arq.tabent.npont - 1;
    if(CalcLat>0 && arq.flashCompleto==0) {
      for(int i=0; i<ndiv+2;i++) delete[] HLat[i];
      delete [] HLat;
    }
    if(LerPB>0) {
       delete[] PBPVTSim;
       delete[] TBPVTSim;
       if(lerRS==1){
         for(int i=0; i<ndiv+2;i++) delete[] RSLivia[i];
         delete [] RSLivia;
       }
    }


    if(ncel>0)delete [] celula;
    if(npig>0) delete[] receb;
    if(arq.tabelaDinamica==1){
    	/*for(int i=0;i<ntabDin;i++){
    		if(tabDin[i].npontosP>0){
    			for(int j=0; j<tabDin[i].npontosP+1;j++){
    				if(tabDin[i].rhogF!=0)delete[] tabDin[i].rhogF[j];
    				if(tabDin[i].rholF!=0)delete[] tabDin[i].rholF[j];
    				if(tabDin[i].DrhogDpF!=0)delete[] tabDin[i].DrhogDpF[j];
    				if(tabDin[i].DrhogDtF!=0)delete[] tabDin[i].DrhogDtF[j];
    				if(tabDin[i].DrholDpF!=0)delete[] tabDin[i].DrholDpF[j];
    				if(tabDin[i].DrholDtF!=0)delete[] tabDin[i].DrholDtF[j];
    				if(tabDin[i].valBO!=0)delete[] tabDin[i].valBO[j];
    				if(tabDin[i].HgF!=0)delete[] tabDin[i].HgF[j];
    				if(tabDin[i].HlF!=0)delete[] tabDin[i].HlF[j];
    				if(tabDin[i].cpgF!=0)delete[] tabDin[i].cpgF[j];
    				if(tabDin[i].cplF!=0)delete[] tabDin[i].cplF[j];
    				if(tabDin[i].valZ!=0)delete[] tabDin[i].valZ[j];
    				if(tabDin[i].valdZdT!=0)delete[] tabDin[i].valdZdT[j];
    				if(tabDin[i].valdZdP!=0)delete[] tabDin[i].valdZdP[j];
    				if(tabDin[i].tit!=0)delete[] tabDin[i].tit[j];
    				if(tabDin[i].rs!=0)delete[] tabDin[i].rs[j];
    				if(tabDin[i].viscG!=0)delete[] tabDin[i].viscG[j];
    				if(tabDin[i].viscO!=0)delete[] tabDin[i].viscO[j];
    			}
    			if(tabDin[i].rhogF!=0)delete [] tabDin[i].rhogF;
    			if(tabDin[i].rholF!=0)delete[] tabDin[i].rholF;
    			if(tabDin[i].DrhogDpF!=0)delete[] tabDin[i].DrhogDpF;
    			if(tabDin[i].DrhogDtF!=0)delete[] tabDin[i].DrhogDtF;
    			if(tabDin[i].DrholDpF!=0)delete[] tabDin[i].DrholDpF;
    			if(tabDin[i].DrholDtF!=0)delete[] tabDin[i].DrholDtF;
    			if(tabDin[i].valBO!=0)delete[] tabDin[i].valBO;
    			if(tabDin[i].HgF!=0)delete[] tabDin[i].HgF;
    			if(tabDin[i].HlF!=0)delete[] tabDin[i].HlF;
    			if(tabDin[i].cpgF!=0)delete[] tabDin[i].cpgF;
    			if(tabDin[i].cplF!=0)delete[] tabDin[i].cplF;
    			if(tabDin[i].valZ!=0)delete[] tabDin[i].valZ;
    			if(tabDin[i].valdZdT!=0)delete[] tabDin[i].valdZdT;
    			if(tabDin[i].valdZdP!=0)delete[] tabDin[i].valdZdP;
    			if(tabDin[i].tit!=0)delete[] tabDin[i].tit;
    			if(tabDin[i].rs!=0)delete[] tabDin[i].rs;
    			if(tabDin[i].viscG!=0)delete[] tabDin[i].viscG;
    			if(tabDin[i].viscO!=0)delete[] tabDin[i].viscO;
    			if(tabDin[i].TBF!=0)delete [] tabDin[i].TBF;
    			if(tabDin[i].PBF!=0)delete [] tabDin[i].PBF;
    		}
    	}*/
    	tabDin.clear();
    }
  }

  SProd& operator=(const SProd&);
  void copiaSemJson(Ler&, int vnoextremo, int vnoinicial, int vderivaAnel, int vbloq, double vbetaRev,
		  double vbetaRevini, double vtitRev, double vtitRevini, double vdtCicMin);
  //metodo que faz a preparacao do tramo, apos ter sido feita a leitura do aqruivo->
  void montasistema(
		  /*variaveis usadas apenas em anel de GL, quando o duto e o anel ->*/
		  double* compfonte=0/*vetor de posicoes das derivacoes do anel, valor de entrada*/,
		  int* posicfonte=0/*vetor de indice das derivacoes do anel, valor retornado*/,
		  int nfontes=0/*numero de derivacoes, valor de entrada*/);
  void HidroDescargaG();//metodo que faz a inicializacao da linha de gas, quando em modo descarga de gas-lift;
  void HidroDescargaP();//metodo que faz a inicializacao da linha de producao, quando em modo descarga de gas-lift;
  /*metodo que faz uma estimativa da correcao da pressao de injecao de gas-lift em um processo de descarga de GL,
   quando a vazao de fluido de completacao se aproxima do seu limite erosional em alguma valvula*/
  double prescordesc(double velmax/*vazao mais alta do conjunto de valvulas de GL*/,
		  int ivalv/*indice da valvula de GL*/, double fator/*fator de seguranca para a vazao erosional maxima*/,
		  int sinal/*indica para onde a pressao de injecao deve ser corrigida se aumentada ou diminuida*/); //alteracao5
  double CalcPresValvDesc(double velGarg/*vazao na valvula de GL*/,
		  int ivalv/*indice da valvula*/);//corrige a pressao de injecao, tomando como referencia a estimativa de prescordesc
  double BuscaPresInjDesc();//controle da pressao de injecao e da pressao a montante do choke a partir das vazoes
  //nas valvulas de gas-lift;
  void renovaGas();//renova as grandezas da linha de gas apos a soulcao do sistema linear de acoplamento pressao-velocidade
  void renovaGasBuf();//so usado em situacoes de rede, iteracao itermediaria,
  //primeira estimativa do acoplamento pressao velocidade do tramo na rede,
  //calculo de grandezas intermediarias na convergencia da rede;
  double areaValvCali(double PCal,double TCal,double PVO, double PT,
       double dextern,double areagarg,double Rvalv, double Temp);//calculo da abertura da valvula de GL por pressao;
  void calctempGas(int i/*indice do volume*/,double tempantiga/*temperatura antiga do volume*/, int modoPerm=0);//calculo da evolucao da temperatura do volume i na linha de gas;
  void resolveDescarga();//resolve o perfil de pressao e vazao na linha de gas em um processo de descarga de GL
  //na regiao onde esta o fluido de completacao;
  void tempDescarga(int i);//calculo da temperatura de um volume de gas em um processo de descarga
  //na regiao com fluido de completacao
  void avancInter();//calculo do deslocamento da interface gas-fluido de completacao na linha de gas em uma descarga de GL;
  double TempDescGL(int igl/*indice da valvula*/);//calcula por JT a temperatura do gas que passa pela valvula de GL, modelo retirado do Marlim 2;
  void ValvGasTrans();//verifica as posicoes na linha de gas onde se encontram as valvulas de gas-lift;
  void subtempoGas();//metodo que faz a resolucao geral do sistema de gas, evoca o acoplamento P-V
  //e o loop de resolucao da tem,peratiura de gas;
  void subtempoGasBuf();//metodo que faz a resolucao geral do sistema de gas no processo iterativo intermediario da resolucao de rede,
  //este metodo evoca o acoplamento P-V e o loop de resolucao da tem,peratiura de gas, mas resolve variaveis de iteracao intermediaria, buffer;
  void conectaColuna();//metodo que faz o compartilhamento das informacoes de transferencia de calor
  //entre a coluna e o anular para se fazer o acoplamento termico destes dois sistemas no modelo transiente;
  double interpolaHLatente(double pres, double temp);//calcula o calor latente a partir de
  //tabelas de entalpia, metodo so utiulizado em casos especiais com modelagem predominantemente black-oil;
  void calctemp(int i/*indice do volume*/,double tempantiga/*temperatura do volume antes da evolucao*/, int modoPerm=0);//calcula a temperatura do i-esimo volume
  //de um sistema de producao em uma evolucao transiente;
  double calcHmix(int i);//metodo nao utilizado
  double energmix(int i,int jp0, int jt, double razp);//metodo nao utilizado
  void calcTempEntalp(int i);//metodo nao utilizado
  void calcTransMassTermo(int i);//metodo nao utilizado
  void FonteValv(int ind);//metodo para o modelo de choke da Master1,
  //calcula a vazao que passa pela Master 1 quando o choke esta atuando;
  void salvaFonte();//metodo que salva os valores de fonte de massa no sistema de producao do tempo anterior,
  //caso em um processo iteratico seja necessario voltar a estes valores;
  void renovaFonte(int ind);//metodo que atualiza as fontes de massa no sistema de producao,
  //fontes de massa como IPR, fonte de gas, liquido, vazamento, VGL;
  void renovaalbetini();//metodo que grava os valores de fracao de vazio e beta da camada de tempo anterior,
  //antes da evolucao destas grandezas. Alem disto, o metodo atualiza a velocidade de deslocamento de um eventual PIG
  //e verifica se algum PIG chegou no seu recebedor;
  void renovaMasEsp();//metodo que grava os valores de massa especifica no volume e na sua fronteira esquerda
  // estas massas especificas sao invocadas frequentemente e esta acao evita o calculo redundante destes valores;

  //os metodos a seguir calculam os parametros C0 e Ud de escorregamento para diversas correlacoes, a lista de parametro e
  //rhol massa especifica da mistura de liquido na fronteira, rhog massa especifica de gas na fronteira,
  //tensup tensao superficial na fronteir, alf fracao de vazio na fronteira,
  // reymix Reynolds da mistura na fronteira, reymixL Reynolds da fase liquida, ug1 velocidade superficial de gas
  // ul1 velocidade superficial de liquido, dia diametro, rug rugosidade absoluta, tet angulo
  // C0 parametro de distribuicao, ud velocidade de escorregamento, correcHor correcao para angulo =0
  // esta correcao define qual sentido a velocidade de escorregamento deve tomar em um duto horizontal
  void BhagwatGhajar(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		    double ug1, double ul1, double dia,double rug, double tet, double& c0,
			double& ud, double correcHor=1.);
  void BhagwatGhajarMod(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		    double ug1, double ul1, double dia,double rug, double tet, double& c0,
			double& ud, double correcHor=1.);
  void Choi(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		  double ug1, double ul1, double dia,double rug, double tet, double& c0,
		  double& ud, double correcHor=1.);
  void HibikiIshii(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		  double ug1, double ul1, double dia,double rug, double tet, double& c0,
		  double& ud, double correcHor=1.);
  void FrancaLahey(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		  double ug1, double ul1, double dia,double rug, double tet, double& c0,
		  double& ud, double correcHor=1.);
  void C0UdDisperso(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		  double ug1, double ul1, double dia,double rug, double tet, double& c0,
		  double& ud, double correcHor=1.,int estabCol=0);
  void C0UdAnularChurn(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		  double ug1, double ul1,double dia,double rug, double tet, double& c0,
		  double& ud, double correcHor=1.,int estabCol=0);
  void C0UdEstratificado(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
		  double ug1, double ul1, double dia,double rug, double tet, double& c0,
		  double& ud, double correcHor=1.,int estabCol=0);
  void CalcC0Ud(int ind, double& c0, double& ud);//metodo em que as correlacoes de escorregamento sao chamadas
  void CalcC0UdBuf(int ind, double& c0, double& ud);//metodo para o caso de rede,
  //calcula os parametros de escorregamento no passo intermediario da convergencia da rede
  void CalcC0UdIni(int ind, double& c0, double& ud);//metodo para calculo dos parametros de escorregamento em
  //tramos internos de rede na primeira fronteira, quando esta e entendida como condicao de contorno pressao
  void CalcC0UdIniBuf(int ind, double& c0, double& ud);//metodo para calculo dos parametros de escorregamento em
  //tramos internos de rede na primeira fronteira, passo intermediario da convergencia da rede,
  //quando esta e entendida como condicao de contorno pressao
  void correcHidroFric(int i, double& hidro, double& fric);//metodo nao utilizado
  void auxMiniTab(ProFlu& flu);
  void geraMiniTabFlu();
  void renova(int expli=0);//metodo onde os resultados do acoplamento pressao-velocidade do sistema de producao sao
  //carregados nos atributos de pressao no centro de volume e vazzao massica na fronteira de volume,
  //alem de fazer o calculo da pressao na fronteira de volume;
  void renovaVaz();
  void renovaBuffer();// metodo onde os resultados do acoplamento pressao-velocidade intermediario na solucao de rede
  //do sistema de producao sao carregados nos atributos de pressao no centro de volume e vazzao massica na fronteira de volume,
  //alem de fazer o calculo da pressao na fronteira de volume, a atualizacao e feita apenas nos volumes extremos do tramo;
  void renovaBufferCego();//para um tramo com choke ativo, nao e necessario fazer o passo iterativo para a rede
  //neste caso, neste caso, nao e feito o acoplamento P-V intermediario para este tramo
  //e as variaveis de iteracao itermediaria deste tramo repetem os valores da camada de tempo antes da evolucao;
  int sign(double var);//metodo nao utilizado
  void renovaTemp();//metodo que trata do termo de transferencia de massa e tambem faz o
  //calculo de termos da equacao de transferencia de massa que sao aplicados de maneira distribuidas
  //nas equacoes de evolucao da fracao de vazio e na equacao de conservacao de massa da mistura;
  //void renovaRGOdgYco2();
  void avaliaParafina();
  void renovaRGOdgYco2(ProFlu fluiRev=ProFlu()/*fluido de retorno no fim de um tramo incluido em uma rede*/);
  //metodo que trata do rasteramento de variaveis como RGO, API e densidade de gas;
  void renovaFracMol(ProFlu fluiRev=ProFlu()/*fluido de retorno no fim de um tramo incluido em uma rede*/);
    //metodo que trata do rasteramento de variaveis como RGO, API e densidade de gas;
  void renovaFracMol2(ProFlu fluiRev=ProFlu()/*fluido de retorno no fim de um tramo incluido em uma rede*/);
  void renovaterm(int aflu=0);//metodo responsavel pelo calculo dos termos  T_1  e T_2  que definem
  // as vazoes massicas de liquido e de gas na fronteira do volume a partir da vazao massica da mistura;
  void renovatermAfluFim();//metodo da iteracao intermediaria em uma rede, responsavel pelo calculo dos termos
  //T_1  e T_2  que definem as vazoes massicas de liquido e de gas na fronteira do volume final do tramo -
  //tramo interno narede -a partir da vazao massica da mistura;
  void renovatermColIni();//metodo da iteracao intermediaria em uma rede, responsavel pelo calculo dos termos
  //T_1  e T_2  que definem as vazoes massicas de liquido e de gas na fronteira do volume inicial do tramo -
  //tramo interno narede - a partir da vazao massica da mistura;
  void calcCCpres(double titRev=1./*titulo de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*/, double alfRev=1./*fracao de vazio de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*/, double betRev=0./*beta de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*/);
  //metodo que calcula a vazao que passa pelo choke no ultimo volume a
  //partir da pressao da ultima celula e a pressao do separador;
  void calcCCBuffer(double titRev=1., double alfRev=1., double betRev=0.);//metodo que calcula a vazao que
  //passa pelo choke no ultimo volume a partir da pressao da ultima celula e a pressao do separador
  //em um processo intermediario de conevrgencia de uma rede;
  //void calcCCpres();
  void determinaDT(int vexpli=0);//metodo que determina qual sera o incremento de tempo a partir da condicao
  //CFL ou de outros criterios necessarios para garantir a estabilidade da simulacao;
  void determinaDTExpli();
  void atenuaDtMax();//metodo que define uma restricao especial ao incremento de tempo quando se verifica
  //que por algum motivo se esta precisando ytrabalhar com incremento medio de tempo muito menor do que
  //o incremento medio definido pelo CFL;
  //grupo auxiliar, ainda em teste, para se considerar a variacao da massa especifica do liquido com o tempo
  void avaliaVariaDpDt(double razMast=0, double razMast0=0, int vexpli=0);//metodo que avalia se é necessario manter um critério de modelagem completa,
  //considerando a variação da massa especifica de liquido com tempo, isto é feito a partir da ordem de grandeza da derivada Dp/Dt
  void aberturaVal1();
  void aberturaVal0();
  void restringeDTporValv();
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  void solveLinGas();//metodo onde efetivamente ocorre a evolucao das grandezas fisicas relacionadas com
  //o sistema de injecao de gas e prepara este sistema para o seu acoplamento com o sistema de producao;
  void EvoluiFrac(double alfrev=1./*fracao de vazio de reversao de fluxo recebido de outro tramo
		  a jusante em um esquema de rede*/,double betrev=0./*beta de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*/, int ciclo=0);//metodo que trata da evolucao das fracoes volumetricas;
  void ReiniEvolFrac0();//metodo onde e feita a correcao de tempo quando se observa que algum volume
  //esta com fracao volumetrica fora do limite correto;
  void SubReiniEvolFrac();//metodo auxiliar para ReiniEvolFrac, salvando apenas as frações volumétricas;
  void ReiniEvolFrac();//metodo que, quando se observa que algum volume
  //esta com fracao volumetrica fora do limite correto, retorna os valores antigos de fracoes volumetricas,
  //antes da evolucao de tempo;
  void AtualizaPig();//metodo que trata do calculo das fracoes volumetricas dentro de um volume devido a
  //um eventual �pig� no sistema;
  void SolveAcopPV(int vexpli=0,int ciclo =0/*contador de ciclos iterativos, caso se trabalhe com um modelo mais completo
  nas equacoes de conservacao de massa*/);//metodo onde e montada a matriz global do
  //acoplamento pressao-velocidade e e resolvido o sistema linear refernte a esta matriz global;
  //void SolveTrans();
  void prepDifusCalorND(int i);
  void marchaEnergTrans(int ciclo=0, int ciclomax=0); //faz a marcha da equação de energia no processo transiente
  void atualizaMiniTab();
  void atualizaCC1();
  void SolveTrans(double titRev=1./*titulo de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*/, double alfRev=1./*fracao de vazio de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*/, double betRev=0./*beta de reversao de fluxo recebido de outro tramo
  a jusante em um esquema de rede*//*,
		  ProFlu fluiRev=ProFlu()*/,int nrede=-1,ProFlu fluiRev=ProFlu());//metodo faz a gestao do avanco de tempo do sistema de producao
  //ao fim dos comandos deste metodo, o sistema tera todas variaveis fisicas representativas do
  //sistema de escoamento atualizadas. Alem disto, dentro deste metodo e feita a decisao de impressao de
  //resultados e de atualizacao do arquivo de �Log� do tramo;
  void ImprimeTrendP(int i/*i-esima tendencia definida no Json*/, int nrede=-1);//metodo onde e gravada a tendencia do sistema de producao
  //gravacao feita em lotes a medida em que a simulacao avanca;
  void ImprimeTrendPCab(int i/*i-esima tendencia definida no Json*/, int nrede=-1);//impressao de cabecalho, tendencia producao;
  void ImprimeTrendG(int i/*i-esima tendencia definida no Json*/, int nrede=-1);//metodo onde e gravada a tendencia do sistema de gas
  //gravacao feita em lotes a medida em que a simulacao avanca;
  void ImprimeTrendGCab(int i/*i-esima tendencia definida no Json*/, int nrede=-1);//impressao de cabecalho, tendencia gas;
  void ImprimeTrendTransP(int i/*i-esima tendencia definida no Json*/);//metodo onde e gravada a tendencia de temperatura
  //de parede do sistema de producao gravacao feita em lotes a medida em que a simulacao avanca;
  void ImprimeTrendTransPCab(int i/*i-esima tendencia definida no Json*/);//impressao de cabecalho, tendencia parede producao;
  void ImprimeTrendTransG(int i/*i-esima tendencia definida no Json*/);//metodo onde e gravada a tendencia de temperatura
  //de parede do sistema de producao gravacao feita em lotes a medida em que a simulacao avanca;
  void ImprimeTrendTransGCab(int i/*i-esima tendencia definida no Json*/);//impressao de cabecalho, tendencia gas;
  ////////////////////////////////////////////////////////////////////////////////////////////////
  double marchaProdPerm1(double pchute);//marcha para solver permanente, obtem perfil de pressao, temperatura,
  //vazao e fracao de vazio para um chute inicial de pressao de fundo pchute, marcha feita para condicao de pressao no fim
  //do duto e vazao no inicio;
  double marchaProdPerm1Rev(double pchute);
  double marchaProdPerm2(double pchute);//marcha para solver permanente, obtem perfil de pressao, temperatura,
  //vazao e fracao de vazio para um chute inicial de pressao de fundo pchute, marcha feita para condicao de choke no fim
  //do duto e vazao no inicoio;
  double buscaProdPfundoPerm(double chute=-1.,int kontaTenta=-1);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaProdPerm1 e assim iniciar o prpocesso de calculo de ero de funcao. chute=chute inicial de pressao de fundo;
  double buscaProdPfundoPermRev(double chute=-1.);
  double buscaProdPfundoPerm2(double chute=-1.,int kontaTenta=-1);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaProdPerm2 e assim iniciar o prpocesso de calculo de ero de funcao;
  double buscaProdPfundoPerm3(double pentrada);//Marcha simples quando a vazao e a pressao no inicio da tubulacao sao conhecidas;
  double marchaProdPresPres1(double mchute);//marcha para solver permanente, obtem perfil de pressao, temperatura,
  //vazao e fracao de vazio para um chute inicial de vazao massica de fundo mchute, marcha feita para condicao de
  //pressao no fim do duto e vazao no inicio;
  double marchaProdPresPres1Rev(double mchute);
  double buscaProdPresPresPerm(double mchute, double maxvaz=0.,int kontaiter=0);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaProdPresPres1 e assim iniciar o prpocesso de calculo de ero de funcao. chute=chute inicial de vazao de fundo;
  double buscaProdPresPresPermRev(double mchute, double maxvaz=0.,int kontaiter=0);
  double marchaProdPresPres2(double mchute);//marcha para solver permanente, obtem perfil de pressao, temperatura,
  //vazao e fracao de vazio para um chute inicial de vazao massica de fundo mchute, marcha feita para condicao de
  //pressao no fim do duto e vazao no inicio;
  double buscaProdPresPresPerm2(double mchute,double maxvaz=0.);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaProdPresPres2 e assim iniciar o prpocesso de calculo de ero de funcao. chute=chute inicial de vazao de fundo;
  double marchaProdPresPres3(double mchute);//marcha para solver permanente, obtem perfil de pressao, temperatura,
  //vazao e fracao de vazio para um chute inicial de vazao massica de fundo mchute, marcha feita para condicao de
  //pressao no fim do duto e vazao no inicio;
  double buscaProdPresPresPerm3(double mchute,double maxvaz=0.);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaProdPresPres2 e assim iniciar o prpocesso de calculo de ero de funcao. chute=chute inicial de vazao de fundo;
  double marchaGasPerm1(double chutemass=-1);//marcha permanente na linha de gas, condicao de contorno
  //pressao de injecao com chute  de vazao chutemass, metodo que trabalha acoplado a marcha permanente do sistema permanente
  //ao fim da marcha obtem-se um perfil de pressao e temperatura na linha de gas;
  double marchaGasPerm2(double pchute,double chutemass=-1);//marcha permanente na linha de gas, condicao de contorno
  //vazao de injecao com chute  de pressao pchute, metodo que trabalha acoplado a marcha permanente do sistema permanente
  //ao fim da marcha obtem-se um perfil de pressao e temperatura na linha de gas;
  double marchaGasPerm3(double pchute);//marcha permanente na linha de gas, condicao de contorno
  //pressao de injecao a montante do choke de injecao e com chute  de pressao a jusante do choke pchute,
  //metodo que trabalha acoplado a marcha permanente do sistema permanente
  //ao fim da marcha obtem-se um perfil de pressao e temperatura na linha de gas;
  double buscaGasPresPerm2();//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaGasPerm2 e assim iniciar o prpocesso de calculo de ero de funcao;
  double buscaGasPresPerm3();//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaGasPerm3 e assim iniciar o prpocesso de calculo de ero de funcao;
  void RenovaPresPermMon(int i, int RK/*ordem da marcha*/);//metodo onde se faz a marcha da pressao do
  //centro do volume i-1 para a pressao na fronteira a jusante
  double RenovaPresPermNcel();//metodo onde se faz a marcha da pressao do
  //centro do volume ncel para a pressao na fronteira a jusante
  double calcDpArea(int i,double rhomix, double rey, double jmix);
  void RenovaPresPermJus(int i, int RK/*ordem da marcha*/);//metodo onde se faz a marcha da pressao na fronteira a
  //montante do volume i para a pressao no centro do volume i;
  void corrDeng(int i);
  void RenovaMassPerm(int i);//metodo onde se faz a atualizacao da vazao massica na fronteira a jusante do volume i
  //devido a alguma fonte de massa, com a devida atualizacao das propriedades do fluido no volume e da fracao de vazio;
  void RenovaMassPermRev(int i);
  void RenovaMassPermComp(int i);//metodo onde se faz a atualizacao da vazao massica na fronteira a jusante do volume i
  //devido a alguma fonte de massa, com a devida atualizacao das propriedades do fluido no volume e da fracao de vazio
  //e mudança na composição da corrente de pseudocomponentes;
  void RenovaMassPermCompRev(int i);
  void CalcC0UdPerm(int ind, double& c0, double& ud);//metodo em que e feito o calculo dos parametros de
  //escorregamento, c0 e ud, na fronteira a jusante do volume ind em regime permanente;
  void RenovaTransMassPerm(int i);//metodo em que e feito o calculo da taxa de transferencia de massa entre fases
  //no regime permanente;
  void RenovaTransMassPermGas(int i);
  void RenovaTempPerm(int i, int RK);//metodo em que e feita a marcha da temperatura em regime permanente da celula i-1
  //para a celula i;
  void RenovaTempPermRev(int i, int RK);
  void atualizaPeriPmonProd(int i);//metodo em que e adicionado o ganho de pressao na fronteira a montante do volume i
  //devido a acao de uma BCS;
  void atualizaPeriPjusProd(int i);//metodo que faz meramente uma atualizacao da informacao de pressao dos
  //volumes a esquerda e a direita do volume i, apos o calculo da pressao no centro deste volume;
  void atualizaPeriTempProd(int i);//metodo que faz meramente uma atualizacao da informacao de temperatura dos
  //volumes a esquerda e a direita do volume i, apos o calculo da temperatura no centro deste volume;
  void RenovaPresGasPerm(int i);//metodo onde se faz a marcha da pressao na linha de gas do
  //centro do volume i-1 para o centro de colume i-1;
  double delpGasPerm(int i);//metodo que faz uma estimativa da variacao de pressao na linha de gas
  //entre o volume i-1 para o volume i, nao calcula de fato a marcha de pressao, e so uma estimativa,
  //usada no metodo buscaGasPresPerm2;
  double delpInjPerm(int i);//metodo que faz uma estimativa da variacao de pressao em um sistema de poco injetor
  //entre o volume i-1 para o volume i, nao calcula de fato a marcha de pressao, e so uma estimativa,
  //usada no metodo buscaInjPfundoPerm3;
  void calcVazGasPerm(int i);//metodo usado na marcha da linha de gas, verifica se o volume i contem
  //uma VGL e calcula a vazao de gas passando por esta VGL a partir da pressao no volume de gas
  // e da pressao no volume correlato do sistema de producao;
  void IniciaVazValvGasPerm(int i);//metodo que faz uma estimativa inicial da vazao na VGL quando se inicia uma marcha no
  //sistema de p�roducao, se a condicao for vazao injetada, a estimativa e a vazao de injecao dividida pelo numero de
  //VGLs se for pressao de injecao, a estimativa e feita a partir de uma pressao aproximada por hidrostatica de gas;
  void RenovaTempGasPerm(int i);//metodo em que e feita a marcha da temperatura em regime permanente na linha de gas
  // da celula i-1 para a celula i;
  void conectaColunaPerm();//metodo que faz o compartilhamento das informacoes de transferencia de calor
  //entre a coluna e o anular para se fazer o acoplamento termico destes dois sistemas no modelo permanente;
  //void conectaColunaPerm(int i);
  void IniciaconectaColunaPerm();//metodo que faz o compartilhamento de informacoes estimadas de transferencia de calor
  //, antes de se iniciar a marcha, entre a coluna e o anular para se fazer o acoplamento termico destes
  //dois sistemas no modelo permanente;
  void atualizaProp();
  void atualizaVelTermPerm();
  void calcDTPseudoTrans();
  ////////////////////////////////////////////////////////////////////////////////////////////////
  double marchaInjPerm1(double chute);//metodo marchante para o caso de poco injetor, chute
  //pode ser a pressao de injecao ou a vazao de injecao, a depender da condicao de contorno;
  double buscaInjPfundoPerm1(double chute=-1.);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaInjPerm1 e assim iniciar o prpocesso de calculo de erro de funcao.
  //caso de poco injetor condicoes de contorno (pressao de injecao e IPR no fundo; CC1) ou
  //(pressao de injecao e pressao de fundo; CC3);
  double buscaInjPfundoPerm2(double chute=-1.);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaInjPerm1 e assim iniciar o prpocesso de calculo de erro de funcao.
  //caso de poco injetor condicoes de contorno (vazao de injecao e IPR no fundo, CC0);
  double buscaInjPfundoPerm3(double chute=-1.);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaInjPerm1 e assim iniciar o prpocesso de calculo de erro de funcao.
  //caso de poco injetor condicoes de contorno (pressao de fundo e IPR no fundo, CC2);
  double buscaInjPfundoPerm4();//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaInjPerm1 e assim iniciar o prpocesso de calculo de erro de funcao.
  //caso de poco injetor condicoes de contorno (vazao de injecao e pressao de injecao, CC4);
  double buscaInjPfundoPerm5(double chute=-1.);//busca de dois chutes iniciais com valores com sinais opostos
  //para marchaInjPerm1 e assim iniciar o prpocesso de calculo de erro de funcao.
  //caso de poco injetor condicoes de contorno (vazao de injecao e pressao de fundo, CC5);
  //////////////////////////////////////////////////////////////////////////////////////////////////////


  double multMarcha(double chute/*chute da busca, que pode ser uma pressao ou vazao*/,
		  int prod/*indica se a marcha e de linha de producao ou de linha de gas,
		  //a injecao e defindia por um atributo de leitura.h*/,
		  int tipoCC/*na linha de gas, se 0->vazao de injecao, senao-> choke de injecao
		  Na linha de producao, se 0-> condicao na plataforma de pressao
		  senao condicao na plataforma de choke*/);//metodo responsavel por chamar os
  //diversos metodos de busca de zero de funcao, metodos de busca de zero de funcao para siatema de producao,
  //linha de gas ou sistema de injecao;
  double SIGN(double a,double b);//metodo auxiliar a zriddr;
  double zbrent(double,double,int prod, int tipoCC,
		  double tol=0.00001,double epsn=0.00001, int maxit=100);//metodo nao usado
  double falsacorda(double a, double b,int prod, int tipoCC);//metodo nao usado
  double zriddr(double x1,double x2,int prod, int tipoCC);//metodo de calculo de zero de funcao,
  //precisa de dois chutes, x1 e x2

  double hidroreverso(double hol/*hodup medio na linha*/,
		  double vaz=0/*vazao media na linha*/, double vazG=0);//metodo auxiliar para redes de producao
  //permanente, faz uma estimativa das pressoes nos nos da rede a partir de um calculo simples de hidrostatrica;
  double hidroreversoInj(double hol, double vaz=0);//metodo auxiliar para redes de injecao
  //permanente, faz uma estimativa das pressoes nos nos da rede a partir de um calculo simples de hidrostatrica;
  double hidroTramoSecundario(double titulo/*titulo medio na linha*/);//metodo auxiliar para redes de producao
  //permanente, paralelas, faz uma estimativa das pressoes nos nos do tramo secundario da rede paralela
  //partir de um calculo simples de hidrostatrica;
  double buscaTramoSecVazPerm(double pPartida, int indPartida);
  double marchaTramoSecVaz(double pchute,double chutemass=-1);

  void calcTempFim();//calcula a temperatura a jusante do choke de superfisie, utilizada no calculo de rede;
};

#endif /* SISPROD_H_ */
