#ifndef _CEL
#define _CEL

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>

using namespace std;
#include "acessorios.h"
#include "Geometria.h"
#include "Vetor.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "TrocaCalor.h"
#include "variaveisGlobais1D.h"

/*extern double localtiny;//pequeno valor, usado como criterio para transicao entre multifasico e monofasico
extern double CritCond;//variavel global, indica o menor valor de fracao de vazio ou holdup em que a
//transferencia de massa e considerada em uma simulacao transiente
extern double CritDTMin;
extern double lixo5;// variavel global, tempo na simulacao para um so tramo;

extern int chaverede;//variavel global, indica se se esta simulando um tramo solteiro ou pertencente a uma malha de dutos;
extern int chaveredeT;*/

struct dadosParafina;

class Cel{


      	public:
      	
	    //Solver de Hidratos

		double j_H;
		double j_G;
		double j_W;
		double V_h;
		double V_w; //chris - Hidratos
		double FVH; //chris - Hidratos
		double agua_consumida;  // acumulada por hidrato //chris - Hidratos
		double massa_hidrato; //chris - Hidratos
		//double Vcel_hyd; //chris - Hidratos
		//double VolLiq;  //chris - Hidratos
	    
	    DadosGeo dutoL;//objeto com as informacoes sobre a geometria do volume a esquerda;
        DadosGeo duto;//objeto com as informacoes sobre a geometria do volume;
        DadosGeo dutoR;//objeto com as informacoes sobre a geometria do volume a direita;
        acessorio acsr;//objeto onde o acessorio do volume e armazenado->fontes, BCS,bomba volumetrica, valvula;
        acessorio* acsrL;//ponteiro com o endereco do acessorio do volume a esquerda;
        ProFlu flui;//objeto com as propriedades do fluido do volume;
        ProFlu* fluiL;//ponteiro com o endereco do obejto que armazena as propriuedades do fluido do volume a esquerda;
        ProFlu* fluiR;//ponteiro com o endereco do obejto que armazena as propriuedades do fluido do volume a direita;
        ProFluCol fluicol;//objeto com as propriedades do fluido complementar;
        int fonteCompartilhada;
        double multiFComp;

        //obs: pressao em kgf/cm2, temperatura em C; vazao massica em kg/s; tempo em seg.; diametro, perimetro,
        //rugosidade em metros, massa especifica em kg/m3, vazao volumetrica em m3/s; angulo em radianos;
        //velocidade em metros; comprimento de celula em metros; volume de leve, pesado e agua em Sm3;
        //fluxo de calor em W/m;

        int correlacaoMR2;
        int posic;//indice do centro de volume, referencia->fundo de poco;
        double dt;//incremento de tempo, em geral, igual ao incremento de tempo global, mas pode ser diferente
        double dt1;
        //caso neste volume seja necessario um ajuste de tempo para corrigir problemas na fracao volumetrica do volume;
        double dt2;//incremento de tempo relacionado com a correcao necessaria, quando o fator beta e desrespeitado
        //nos seus limites, quando isto ocorre, calcula-se o valor de incremento de tempo para que isto nao ocorra e
        //e armazenado em dt2;
        double dTdt;//variacao da temperatura com o tempo, usado em caso de se desejar usar esta correcao nos modelos
        //de conservacao de massa;
        double dTdtIni;
        double dTdtL;//variacao da temperatura com o tempo no volume a esquerda, usado em caso de se desejar
        //usar esta correcao nos modelos de conservacao de massa;
        double tempR;//temperatura no volume a direita;
        double tempRini;//temperatura no volume a direita na camada de tempo anterior;
        double temp;//temperatura no volume;
        double tempini;//temperatura no volume na camada de tempo anterior;
        double tempL;//temperatura no volume a esquerda;
        double tempLini;//temperatura no volume a esquerda na camada de tempo anterior;
        double presL;//pressao no volume a esquerda;
        double presLini;//pressao no volume a esquerda na camada de tempo anterior;
        double pres;//pressao no volume;
        double presini;//pressao no volume na camada de tempo anterior;
        double presauxL;//pressao na fronteira esquerda do volume a esquerda;
        double presauxLini;//pressao na fronteira esquerda no volume a esquerda
        //na camada de tempo anterior;
        double presaux;//pressao na frointeira esquerda do volume;
        double dpresaux;//desvio medio da pressao na fronteira a esquerda do volume em relacao as medias
        //das pressoes dos volumes vizinhos, devido as correcoes friccao e hidrostatica;
        double presauxini;//pressao na frointeira esquerda do volume, na camada de tempo anterior;
        double presauxR;//pressao na fronteira esquerda do volume a direita;
        double presauxRini;//pressao na fronteira esquerda no volume a direita
        //na camada de tempo anterior;
        double presR;//pressao no volume a direita;
        double presRini;//pressao no volume a direita na camada de tempo anterior;
        double ML;//vazao massica da mistura na fronteira esquerda do volume a esquerda;
        double MLini;//vazao massica da mistura na fronteira esquerda do volume a esquerda
        // na camada de tempo anterior;
        double MC;//vazao massica da mistura na fronteira esquerda do volume;
        double MCini;//vazao massica da mistura na fronteira esquerda do volume
        // na camada de tempo anterior;
        double MR;//vazao massica da mistura na fronteira esquerda do volume a direita;
        double MRini;//vazao massica da mistura na fronteira esquerda do volume a direita
        // na camada de tempo anterior;
        double MliqiniR;//vazao massica de liquido na fronteira esquerda do volume a direita;
        double MliqiniR0;//vazao massica de liquido na fronteira esquerda do volume a direita
        // na camada de tempo anterior;
        double Mliqini;//vazao massica de liquido na fronteira esquerda do volume;
        double Mliqini0;//vazao massica de liquido na fronteira esquerda do volume
        // na camada de tempo anterior;
        double MliqiniL;//vazao massica de liquido na fronteira esquerda do volume a esquerda;
        double MliqiniL0;//vazao massica de liquido na fronteira esquerda do volume a esquerda
        // na camada de tempo anterior;
        double MComp;//vazao massica de fluido complementar, valor auxiliar, so usado no permanente em condicao de escoamento reverso
        ///////////////////////////// 05-07-2018 //////////////////////////////////////////////
        double presBuf;//pressao no volume na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double presLiniBuf;//pressao no volume a esquerda na iteracao intermediaria no processo de resolucao de rede
        //de escoamento, variavel da camada de tempo anterior (nao utilizado);
        double presLBuf;//pressao no volume a esquerda na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double presRBuf;//pressao no volume a direita na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double presauxBuf;//pressao na fronteira a esquerda do volume na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double MCBuf;//vazao da mistura na fronteira a esquerda do volume na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double MRBuf;//vazao da mistura na fronteira a esquerda do volume a direita na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double MliqiniRBuf;//vazao da mistura na fronteira a esquerda do volume a direita na iteracao intermediaria
        //no processo de resolucao de rede de escoamento;
        double MRiniBuf;//nao utilizado;
        double MLBuf;//vazao da mistura na fronteira a esquerda do volume a esquerda na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double MliqiniBuf;//vazao de liquido na fronteira a esquerda do volume na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        double MliqiniLBuf;//vazao de liquido na fronteira a esquerda do volume a esquerda na iteracao intermediaria no processo de resolucao de rede
        //de escoamento;
        //////////////////////////// 05-07-2018 ///////////////////////////////////////////////
        double alfL;//fracao de vazio no volume a esquerda;
        double alfLini;//fracao de vazio no volume a esquerda na camada de tempo anterior;
        double alfR;//fracao de vazio no volume a direita;
        double alfRini;//fracao de vazio no volume a direita na camada de tempo anterior;
        double alf;//fracao de vazio no volume;
        double alfIter;
        double alfini;//fracao de vazio no volume, na camada de tempo anterior;
        double betL;//beta no volume a esquerda;
        double betLini;//beta no volume a esquerda, na camada de tempo anterior;
        double betR;//beta no volume a direita;
        double betRini;//beta no volume a direita, na camada de tempo anterior;
        double bet;//beta no volume;
        double betini;//beta no volume, na camada de tempo anterior;
        double betLI;//fracao beta na fronteira esquerda do volume a esquerda;
        double betLIini;
        double betI;//fracao beta na fronteira esquerda do volume;
        double betIini;
        double betRI;//fracao beta na fronteira esquerda do volume a direita;
        double betRIini;
        double FW;//BSW in-situ do volume;
        double FWini;//BSW in-situ do volume na camada de tempo anterior;
        double dxL;//comprimento do volume a esquerda;
        double dx;//comprimento do volume;
        double dxR;//comprimento do volume a direita;
        double razdxTM;//sem utilizacao;
        double razdxTM0;//sem utilizacao;
        double massfonteCH;//sem utilizacao;
        //T1 e T2 do modelo:
        double term1L;//T1 da fronteira esquerda do volume a esquerda;
        double term2L;//T2 da fronteira esquerda do volume a esquerda;
        double term1;//T1 da fronteira esquerda do volume;
        double term2;//T2 da fronteira esquerda do volume;
        double term1R;//T1 da fronteira esquerda do volume a direita;
        double term2R;//T2 da fronteira esquerda do volume a direita;
        double c0;//parametro de distribuicao, fronteira esquerda do volume;
        double ud;//parametro de distribuicao, fronteira esquerda do volume;
        double c0ini;
        double udini;
        double c0Spare;//atributo usado quando ha mudanca de arranjo entre o volume
        //e o volume a esquerda, ou quando ha mudanca de inclinacao, guarda o parametro de distribuicao
        //indicado pelo arranjo no volume anterior e faz uma media com o parametro de distribuicao do volume
        double udSpare;//atributo usado quando ha mudanca de arranjo entre o volume;
        //e o volume a esquerda, ou quando ha mudanca de inclinacao, guarda a velocidade de escorregamento
        //indicado pelo arranjo no volume anterior e faz uma media com a velocidade de escorregamento do volume;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        //variaveis relacionadoas com termos de transferencia de massa usados nas equacoes de conservacao de massa:
        double transmassL;
        double transmassLini;
        double transmassR;
        double transmassRini;
        double FonteMudaFase;
        double FonteMudaFaseini;
        double DTransDt1;
        double DTransDt1ini;
        double DTransDt0;
        double DTransDt0ini;
        double DTransDxR;
        double DTransDxRini;
        double DTransDxL;
        double DTransDxLini;
        double DTransDxLp;
        double DTransDxRp;
        double DTransDxLpini;
        double DTransDxRpini;
        double DTransDx;
        double DTransDtp;
        double DTransDtpL;
        double DTransDtpini;
        double DTransDtT;
        double DTransDtTL;
        double DtransDxLinear;
        double coefTransBetini;
        double coefTransBet;
        double CoefDTRini;
        double CoefDTR;
        double CoefDTLini;
        double CoefDTL;
        double fontedissolv;
        double ativaDeri;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int TMModel;//modelo de transferencia de massa usado no volume; quando zero, modelo completo,
        //quando 1 modelo expicito, quando 2, utiliza-se um modelo simplificado de baixa relevancia e
        //, finalmente, quando 3, elimina-se a transferencia de massa;
        int TMModelL;//modelo de transferencia de massa usado no volume a esquerda;
        double fontemassLL;//fonte de massa de liquido, hidrocarboneto, no volume a esquerda;
        double fontemassLLini;//fonte de massa de liquido, hidrocarboneto, no volume a esquerda
        //camada de tempo anetrior;
        double fontemassGL;//fonte de massa de gas, no volume a esquerda;
        double fontemassGLini;//fonte de massa de gas, no volume a esquerda
        //camada de tempo anetrior;
        double fontemassCL;//fonte de massa de liquido complementar, hidrocarboneto, no volume a esquerda;
        double fontemassCLini;//fonte de massa de liquido complementar, no volume a esquerda
        //camada de tempo anetrior;
        double fontemassLR;//fonte de massa de liquido, hidrocarboneto, no volume;
        double fontemassLRini;//fonte de massa de liquido, hidrocarboneto, no volume
        //camada de tempo anetrior;
        double fontemassGR;//fonte de massa de gas, no volume;
        double fontemassGRini;//fonte de massa de gas, no volume
        //camada de tempo anetrior;
        double fontemassCR;//fonte de massa de liquido complementar, no volume;
        double fontemassCRini;//fonte de massa de liquido complementar, no volume
        //camada de tempo anetrior;
        double dpB;//ganho de pressao na fronteira direita do volume devido a acao de uma BCS;
        double potB;//potencia aplicada ao fluido devido a uma BCS ou um incremento de pressao;
        double potBT;//potencia aplicada a um fluido devido a um equipamento de bombeio considerando a
        //eficiencia do motor
        double dpBL;
        double potBL;
        double potBTL;
        int transic;//contador de passo de tempo para fazer uma transicao suave dos parametros de escorregamento,
        //quando a variavel arranjo modifica de um passo de tempo para outro, nao e feita uma mudanca automatica
        //nos parametros de escorregamento devido a mudanca de arranjo, isto causaria problemas na simulacao;
        int transic0;
        int arranjo;//indicador do arranjo do escoamento na fronteira esquerda do volume;
        int arranjoR;//indicador de arranjo na fronteira direita do arranjo;
        double chutearranjo;//sem utilizacao
        double QLL;//vazao volumetrica da mistura de liquido na fronteira esuqerda do volume esquerdo;
        double QLLini;//vazao volumetrica da mistura de liquido na fronteira esuqerda do volume esquerdo
        //camada de tempo anterior;
        double QL;//vazao volumetrica da mistura de liquido na fronteira esuqerda do volume;
        double QLini;//vazao volumetrica da mistura de liquido na fronteira esuqerda do volume
        //camada de tempo anterior;
        double QLR;//vazao volumetrica da mistura de liquido na fronteira esuqerda do volume direito;
        double QLRini;//vazao volumetrica da mistura de liquido na fronteira esuqerda do volume direito
        //camada de tempo anterior;
        double QG;//vazao volumetrica de gas na fronteira esuqerda do volume;
        double QGini;//vazao volumetrica de gas na fronteira esuqerda do volume
        //camada de tempo anterior;
        double rpL;//massa especifica de lpiquido, hidrocarboneto, volume a esquerda;
        double rpC;//massa especifica de lpiquido, hidrocarboneto, volume central;
        double rpR;//massa especifica de lpiquido, hidrocarboneto, volume a direita;
        double rcL;//massa especifica de lpiquido, complementar, volume a esquerda;
        double rcC;//massa especifica de lpiquido, complementar, volume cehtral;
        double rcR;//massa especifica de lpiquido, complementar, volume a direita;
        double rgL;//massa especifica de gas, volume a esquerda;
        double rgC;//massa especifica de gas, volume central;
        double rgR;//massa especifica de gas, volume a direita;
        double rpLi;//massa especifica de lpiquido, hidrocarboneto,fronteira a esquerda do volume a esquerda;
        double rpCi;//massa especifica de lpiquido, hidrocarboneto,fronteira a esquerda do volume central;
        double rpRi;//massa especifica de lpiquido, hidrocarboneto,fronteira a esquerda do volume a direita;
        double rcLi;//massa especifica de lpiquido, complementar,fronteira a esquerda do volume a esquerda;
        double rcCi;//massa especifica de lpiquido, complementar,fronteira a esquerda do volume central;
        double rcRi;//massa especifica de lpiquido, complementar,fronteira a esquerda do volume a direita;
        double rgLi;//massa especifica de gas,fronteira a esquerda do volume a esquerda;
        double rgCi;//massa especifica de gas,fronteira a esquerda do volume central;
        double rgRi;//massa especifica de gas,fronteira a esquerda do volume a direita;
        double drhogdt;//derivada da massa especifica de gas com o tempo no volume central;
        double mipC;
        double micC;
        double migC;
        double mipR;
        double micR;
        double migR;
        Vcr<double> TL;//vetor livre da matriz local;
        FullMtx<double> local;//matriz local;
        double perdaEstratL;//nao utilizado;
        double perdaEstratG;//nao utilizado;
        double cinematico;//nao utilizado;
        double VTemper;//velocidade de adveccao da temperatura, fronteira a esquerda do volume;
        double VTemperini;//velocidade de adveccao da temperatura, fronteira a esquerda do volume
        //camada de tempo anterior;
        TransCal calor;//objeto de transferencia de calor;
        double fluxcalmed;//fluxo de calor;

        int estadoPig;//indicador de que existe um pig viajando dentro do volume;
        int estadoPigini;
        double dtPig;//incremento de tempo relacionado com a correcao necessaria,
        //quando o pig viaja de um volume para outro em um passo de tempo
        //, quando isto ocorre, calcula-se o valor de incremento de tempo para que o pig viaje ate a fronteira, este tempo
        //e armazenado em dtPig;
        double alfPigE;//fracao de vazio a esquerda do pig, caso este esteja no volume central;
        double alfPigER;//fracao de vazio a esquerda do pig, caso este esteja no volume a direita;
        double alfPigEini;//fracao de vazio a esquerda do pig, caso este esteja no volume central
        //na camada de tempo anterior;
        double alfPigERini;
        double alfPigD;//fracao de vazio a direita do pig, caso este esteja no volume central;
        double alfPigDini;//fracao de vazio a direita do pig, caso este esteja no volume central
        //na camada de tempo anterior;
        double betPigE;//beta a esquerda do pig, caso este esteja no volume central;
        double betPigEini;//beta a esquerda do pig, caso este esteja no volume central
        //na camada de tempo anterior;
        double betPigD;//beta a direita do pig, caso este esteja no volume central;
        double betPigDini;//beta a direita do pig, caso este esteja no volume central
        //na camada de tempo anterior;
        double velPig;//velocidade do pig;
        double velPigini;//velocidade do pig na camada de tempo anterior;
        double razPig;//posicao relativa do pig no volume central, 0=fronteira esquerda, 1=fronteira direita;
        double razPigini;//posicao relativa do pig no volume central, 0=fronteira esquerda, 1=fronteira direita
        //na camada de tempo anterior;
        double RazAreaPig;//nao utilizado;
        double DelPig;//nao utilizado;
        double VazaPig;//nao utilizado;
        double cdpig;//nao utilizado;
        int indpig;//indice que determinada a posicao do pig na fila temporal de lancamento;
        int indpigini;

        double VolLeveST;//volume de componente leve no volume central
        double VolPesaST;//volume de componente pesado no volume central
        double VolAguaST;//volume de agua no volume central;

        double DmasschokeG;//derivada da vazao massica de gas passando pelo choke de superficie com a pressao
        //utilizada para aumentar a implicitude do modelo de choke, so utilizada na ultima celula do sistema;
        double DmasschokeL;//derivada da vazao massica de liquido - hidrocarboneto - passando pelo choke de superficie com a pressao
        //utilizada para aumentar a implicitude do modelo de choke, so utilizada na ultima celula do sistema;
        double DmasschokeC;//derivada da vazao massica de liquido complementar passando pelo choke de superficie com a pressao
        //utilizada para aumentar a implicitude do modelo de choke, so utilizada na ultima celula do sistema;

        double corrigeMassaPres;//nao utilizado;
        int mudaDT;
        int mudaDTL;
        double dpdt;
        double dpdtIni;
        double m2d;
        double m2dL;
        double d2pdt2;
        ///////////////////////////////////
        ////novas saidas
        double termoHidro;//variacao de pressao hidrostatica entre o volume central e o volume a direita
        double termoFric;//variacao de pressao por friccao entre o volume central e o volume a direita

        double termRug;//parcela do calculo de fatpor de friccao relacionada com a rugosidade relativa da tubulacao;

        double angEsq;//valor do ultimo angulo a esquerda do volume com modulo > zero,
        //variavel utilziada na decisao de qual sentido a velocidade de escorregamento tera em um duto horizontal
        //se positiva ou negativa
        double angDir;//valor do ultimo angulo a volume do volume com modulo > zero,
        //variavel utilziada na decisao de qual sentido a velocidade de escorregamento tera em um duto horizontal
        //se positiva ou negativa

        int estabCol;
        int mudaArea;

        double nMol;
        double nMolIni;

        double dR;

        int indGeom;

        double dPdLHidro;
        double dPdLFric;
        double dTdLCor;

        int inddPdLHidro;
        int inddPdLFric;
        int inddTdLCor;

        int reiniciaAlf;
        int reiniciaBet;
        int reiniciaPig;

        int correrGlobHol;

        int miniTabAtraso;

        varGlob1D* vg1dSP;

        int modoParafina;
        int parafinado;
        double deltaPar;

        double fluxCal2D;

        double C2;
        double C3;
        double diamInterIni;
        double porosoPar;
        double MW_wax;
        double rhoWaxLiq;
        double Sum_dCwaxdT;

        double fluxcalAcopRedeP;
        double resAcopRedeP;

        Cel(varGlob1D* Vvg1dSP=0,const DadosGeo vdutoL=DadosGeo(),
        	const DadosGeo vduto=DadosGeo(),
        	const DadosGeo vdutoR=DadosGeo(),
            const ProFlu vflui=ProFlu(),
            const ProFluCol vfluiCol=ProFluCol(),
            const double vtempL=25.,const double vtemp=25.,
            const double vtempR=25.,const double vtempini=25.,
            const double vpresL=10.,const double vpres=10.,const double vpresR=10.,
            const double vML=0.,const double vMC=0.,const double vMR=0.,
            const double vMliqiniL=0.,const double vMliqini=0.,const double vMliqiniR=0.,
            const double valfL=1.,const double valf=1.,const double valfR=1.,
            const double vbetL=1.,const double vbet=1.,const double vbetR=1.,
            const double vdxL=1.,const double vdx=1.,const double vdxR=1.,
            const double vdt=-1.,
            const int vposic=-1,
            const TransCal vcalor=TransCal(),
            const acessorio vacsr=acessorio(),
            acessorio* vacsrL=0);//construtor default
        Cel(const Cel&); //construtor por copia
  	    Cel& operator=(const Cel&);

  	    double Rey(double dia/*diametro*/,double vel/*velocidade*/,
  	    		double rho/*massa especifica*/,double vis/*viscosidade, cP*/);//calcula o npumero de Reynolds
  	    double fric(double re/*Reynolds*/,double eps/*rugosidade relativa*/,
  	    		int tipo=1/*indica se o termo de rugosidade do fator de friccao deve ser recalculado ou
				se deve ser utilizado o termo a priori, se 1, usa o termo a priori*/);  //calcula o fator de friccao
  	    void avancalf(int& reinicia/*indicador se sera necessario corrigir o incremento de tempo*/,
  	    		int ncel/*numero de celulas do sistema*/);//metodo da evolucao da fracao de vazio
  	    void avancbet(int& reinicia/*indicador se sera necessario corrigir o incremento de tempo*/,
  	    		int ncel/*numero de celulas do sistema*/);//metodo da evolucao de beta
  	    double delpJus(double presfim);
  	    void GeraLocal(double presfim/*pressao do separador*/,
  	    		int masChkSup/*indicador de que o choke de superficie esta ativo ou inativo, 0=inativo*/,
				int ncel/*numero de celulas do sistema*/,
				double razareativa/*avisa que a master1 esta em uma condicao de abertura em que se
				 deve considerar a sua resistencia no modelo*/,
                double presE/*para a condicao de contorno pressao na entrada, valor da pressao,
                se presE<0, condicao fechada*/,
				double tempE/*para a condicao de contorno pressao na entrada, valor da temperatura*/,
				double titE=-1/*para a condicao de contorno pressao na entrada, valor do titulo*/,
				double betE=-1/*para a condicao de contorno pressao na entrada, valor de beta*/,
				int ciclo=0/*contador de iteracao para se incluir o termo dTdtL na conservacao de massa da mistura
				em ciclo 0, este termo nao e adicionado ao modelo, so apos a segunda iteracao ciclo=1
				(caso se deseje fazer a iteracao)*/,
				int modelo=1, int noextremo=0, int corrigeContSep=1, double areaChoke=1., int vexpi=0);//metodo onde e feito acoplamento pressao-velocidade

  	    void avancPig(int& reinicia);//metodo em que se calcula a nova posicao relativa do pig no volume, se maior que 1,
  	    //corrigisse o incremento de tempo para ficar =1;
  	    void avancalfPig();//metodo em que se calcula a fracao de vazio a esquerda e direita do pig, dentro do volume;
  	    void avancbetPig();//metodo em que se calcula o beta a esquerda e direita do pig, dentro do volume;

  	    void avancalfPigExpli();//metodo em que se calcula a fracao de vazio a esquerda e direita do pig, dentro do volume;
  	    void avancbetPigExpli();//metodo em que se calcula o beta a esquerda e direita do pig, dentro do volume;

  	    void FeiticoDoTempo();//metodo especial para iteracoes, apos uma evcolucao de tempo,
  	    //retorna os valores calculador para os valores originais da camada de tempo anteriuor

  	    void FeiticoDoTempo2();//metodo especial para iteracoes, apos uma evcolucao de tempo,
  	    //retorna os valores calculador para os valores originais da camada de tempo anteriuor

  	    void FeiticoDoTempo3();//metodo especial para iteracoes, apos uma evcolucao de tempo,
  	    //retorna os valores calculador para os valores originais da camada de tempo anteriuor
  	    double SIGN(double a,double b);//metodo auxiliar a zriddr;
        double zriddrRhop(double x1,double x2, double rp);

  	    void expiAlfaBetaPres(int& reinicia,int ncel);
  	    void expiMassMist(double presfim,int masChkSup,int ncel,double razareativa,
                double presE,
				double tempE,
				double titE=-1,
				double betE=-1,
				int noextremo=0,double areaChoke=1.);
  	    double somVel();
  	    double termAdSomVel();
  	    //void WaxDeposition(int BooleanPi, double C2_star, double C3_star, int BooleanFi,
  	    		           //double ConstFi, double MultipDil, double rug, double MultipVisc, int alteraViscFlu, int ncel);
  	    void WaxDeposition(dadosParafina& detalParafina, int ncel);

  	    FullMtx<double> Jacobiana();
  	    Vcr<double> autoVec(int ind, FullMtx<double> jac);
  	    Vcr<double> flutua();
};

#endif

