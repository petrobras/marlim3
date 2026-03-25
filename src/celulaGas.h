#ifndef _CELG
#define _CELG

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "Geometria.h"
#include "chokegas.h"
#include "TrocaCalor.h"

class CelG{

    //obs: pressao em kgf/cm2, temperatura em C; vazao massica em kg/s; tempo em seg.; diametro, perimetro,
    //rugosidade em metros, massa especifica em kg/m3, vazao volumetrica em m3/s; angulo em radianos;
    //velocidade em metros; comprimento de celula em metros; volume de leve, pesado e agua em Sm3;
    //fluxo de calor em W/m;
      	public:
    //Solver de Hidratos

		double j_H;
		double j_G;
		double j_W;
		double V_h;
		double V_w; //chris - Hidratos
		double FVHG; //chris - Hidratos
		double agua_consumida;  // acumulada por hidrato //chris - Hidratos
		double massa_hidrato; //chris - Hidratos


	    DadosGeo dutoL;//objeto com as informacoes sobre a geometria do volume a esquerda;
	    DadosGeo duto;//objeto com as informacoes sobre a geometria do volume;
        DadosGeo dutoR;//objeto com as informacoes sobre a geometria do volume a direita;
        ProFlu flui;//objeto com as propriedades do fluido do volume;
        TransCal calor;//objeto de transferencia de calor;
        int posic;//indice do centro de volume, referencia->plataforma;
        int tipoCC;//tipo de condicao de contorno na injecao de gas, 0->pressao, 1->vazao
        double dt;//incremento de tempo
        double dTdt;//variacao da temperatura com o tempo, usado em caso de se desejar usar esta correcao nos modelos
        //de conservacao de massa;
        double tempL;//temperatura no volume a esquerda;
        double temp;//temperatura no volume;
        double tempR;//temperatura no volume a direita;
        double presL;//pressao no volume a esquerda;
        double pres;//pressao no volume;
        double presR;//pressao no volume a direita;
        double VGasL;//vazao massica de gas na fronteira esquerda do volume;
        double VGasR;//vazao massica de gas na fronteira direita do volume;
        double VGasRBuf;//vazao massica de gas na fronteira direita do volume,
        //na iteracao intermediaria para a convergencia da rede transiente;
        double VGasRR;//vazao massica de gas na fronteira direita do volume a direita;
        double dx0;//comprimento do volume;
        double dx1;//comprimento do volume a direita;
        double dxL;//comprimento do volume a esquerda;
        double u1LL;//produto massa especpificaXarea da tubukacao no volume a esquerda;
        double u1L;//produto massa especpificaXarea da tubukacao no volume;
        double u1R;//produto massa especpificaXarea da tubukacao no volume a direita;
        double massfonteCH;//fonte de massa no volume;
        double fluxcal;//fluxo de calor;
        double salinidade;//para o modelo de descarga de GL, salinidade do fluido de completacao contido no volume;
        int labelchk;//indicador de que existe uma Master2 instalada na fronteira direita do volume -> 1;
        int fechamon;//nao utilizado;
        double razInter;//para o modelo de descarga de gas lift, posicao relativa da interface
        //gas-fluido de completacao dentro do volume, 0-> frobteira a escquerda do volume,
        //1->fronteira a direitaa do volume;
        double razInterIni;//;//para o modelo de descarga de gas lift, posicao relativa da interface
        //gas-fluido de completacao dentro do volume na camada de tempo anteiror;
        /*double posicLiq;
        double dtInter;
        double velInter;*/
        int *celInter;//ponteiro da variavel que guarda o indice do volume onde se encontra a a interface
        //gas-fluido de completacao, necessario na descarga de GL para indicar no metodo gera-local se o volume
        //esta na interface gas-fluido de completacao;
        ChokeGas chkcell;//objeto choke de injecao;
        double rpchk;//sem utilidade atualmente;
        Vcr<double> TL;//termo livre do sistema local;
        FullMtx<double> local;//matriz local;

        double fonteM2;//fonte de massa auxiliar para modelo de master2, quando o choke esta ativo nesta valvula;
        //double deriLM2;
        //double deriCM2;
        //double deriRM2;
 ///////////////////////////////////////////////////////////////////////////////////////
        double tempLini;//temperatura no volume a esquerda na camada de tempo anterior;
        double tempini;//temperatura no volume na camada de tempo anterior;
        double tempRini;//temperatura no volume a direita na camada de tempo anterior;
        double presLini;//pressao no volume a esquerda na camada de tempo anterior;
        double presini;//pressao no volume na camada de tempo anterior;
        double presRini;//pressao no volume a direita na camada de tempo anterior;
        double VGasLini;//vazao massica de gas na fronteira esquerda do volume na camada de tempo anterior;
        double VGasRini;//vazao massica de gas na fronteira direita do volume na camada de tempo anterior;
        double VGasRRini;//vazao massica de gas na fronteira direita do volume a direita na camada de tempo anterior;
        double u1LLini;//produto massa especpificaXarea da tubukacao no volume na camada de tempo anterior;
        double u1Lini;//produto massa especpificaXarea da tubukacao no volume na camada de tempo anterior;
        double u1Rini;//produto massa especpificaXarea da tubukacao no volume a direita na camada de tempo anterior;
        double massfonteCHini;//fonte de massa no volume na camada de tempo anterior;
        double fonteM2ini;//fonte de massa auxiliar para modelo de master2,
        // valor da camada de tempo anterior, quando o choke esta ativo nesta valvula;
        int fechamonini;//nao utilizado;
        //double razInterIni0;
        int *celInterini;//ponteiro da variavel que guarda o indice do volume onde se encontra a a interface
        //gas-fluido de completacao (camada de tempo anterior), necessario na descarga de GL para indicar no
        //metodo gera-local se o volume esta na interface gas-fluido de completacao;
        int posicini;//nao utilizado atualmente;
        double rpchkini;//sem utilidade atualmente;
        int vgl;//indicador de que este volume tem uma VGL->1, utilizado na impressao de tendencia para VGL;
        double tEstag;//temperatura a montante da VGL, usada para impressao de tendencia para VGL;
        double pEstag;//pressao a montante da VGL, usada para impressao de tendencia para VGL;
        double tGarg;//temperatura a jusante da VGL, usada para impressao de tendencia para VGL;
        double pGarg;//pressao a jusante da VGL, usada para impressao de tendencia para VGL;
        double qGarg;//vazao na VGL em Stdm3/d, usada para impressao de tendencia para VGL;
        double areaGarg;//area do orificio da VGL;

        double rg;
        double rgR;

        int indGeom;

        double dPdLHidro;
        double dPdLFric;
        double dTdLCor;

        int inddPdLHidro;
        int inddPdLFric;
        int inddTdLCor;

        double termoHidro;//variacao de pressao hidrostatica entre o volume central e o volume a direita
        double termoFric;//variacao de pressao por friccao entre o volume central e o volume a direita
 ///////////////////////////////////////////////////////////////////////////////////////////////


        CelG(const DadosGeo vdutoL=DadosGeo(),
        	const DadosGeo vduto=DadosGeo(),
        	const DadosGeo vdutoR=DadosGeo(),
            const ProFlu vflui=ProFlu(),
            const ChokeGas vchkcell=ChokeGas(),
            const double vtempL=25.,const double vtemp=25., const double vtempR=25.,
            const double vpresL=10.,const double vpres=10., const double vpresR=10.,
            const double vVGasL=0.,const double vVGasR=0.,const double vVGasRR=0.,
            const double vu1L=0.,const double vu1R=0.,const double vu1LL=0.,
            const double vdx0=0.,const double vdx1=0.,const double vdxL=0.,
            const double vdt=-1.,const int vposic=-1,const int vfecham=0, const int vtipoCC=0,
            const TransCal vcalor=TransCal());//construtor default
        CelG(const CelG&); //construtor por c�pia
  	    CelG& operator=(const CelG&);

  	    double Rey(double dia/*diametro da tubulacao*/,
  	    		double vel/*velocidade media de escoamento*/,
				double rho/*massa especifica*/,
				double vis/*viscosidade*/);//Reynolds;
  	    double fric(double re/*numero de Reynolds*/,
  	    		double eps/*rugosidade relativa*/);//metodo para calculo do fator de friccao;
  	    double MasEspFlu(double pres, double temp) const;//metodo que calcula a
  	    //massa especifica do fluido de completacao na linha de servico, usado no caso de descarga de VGL;
  	    double VisFlu(double pres, double temp) const;//metodo que calcula a
  	    //viscosidade do fluido de completacao na linha de servico, usado no caso de descarga de VGL;
  	    double CalorLiq(double pres, double temp) const;//metodo que calcula o
  	    //calor especifico do fluido de completacao na linha de servico, usado no caso de descarga de VGL;
  	    double CondLiq(double pres, double temp) const;//metodo que calcula a
  	    //condutividade do fluido de completacao na linha de servico, usado no caso de descarga de VGL;
  	    double DrhoDtFlu(double pres, double temp) const;//metodo que calcula a
  	    //derivada da massa especifica do fluido de completacao com a temperatura na linha de servico,
  	    //usado no caso de descarga de VGL;
  	    double psia(const double p)const { return (p*0.9678411)*14.69595;} //p->kgf, transforma kgf para psia // alteracao2
  	    double Faren(const double t)const { return 1.8*t + 32;} //t em Celcius, transforma Celcius para Farenheit // alteracao2
  	    void GeraLocal(int ncelGas,double presiniG,double tempiniG,double abertura);//metodo que
  	    //monta a matriz local do i-esimo volume da linha de servico
  	    void FeiticoDoTempo();//metodo que recupera os valores dos atributos do voluem da camada de tempo anterior;
  	    void DeVoltaParaoFuturo();//metodo que salva os valores mais recentes antes de um novo avanco de tempo;
  	    //void avancInter(int& reinicia);

};

#endif

