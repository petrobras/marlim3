#ifndef _FNUM
#define _FNUM

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
#include <stdlib.h>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"
#include "Log.h"

// criar objeto para log da aplicacao

extern Logger logger;

void NumError(const string& t);//aviso de erro em ferramentas numericas
template<class T> T gammln(const T xx);//Retorna o valor de ln(gama(xx)) para xx>0
template<class T> T PoliNewton(const Vcr<T>&, const Vcr<T>&, T);//Interpolacao por Newton
template<class T> T InterLinear(const Vcr<T>&, const Vcr<T>&, T);//Interpolacao linear
template<class T> T InterLinear(const FullMtx<T>&, const Vcr<T>&, const Vcr<T>&, T, T);//Interpolacao Linear para uma superficie
template<class T, T F(T,const T* const)> T NIntegrate(T,T,const T* const,int,int tipo=3);//Integracao, esta usa Euler, tipo=1, Simpson-> tipo =2, Gauss->tipo=3
template<class T> void gauleg(const T x1, const T x2, Vcr<T>& x, Vcr<T>& w);//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Legendre, da os pontos x da quadratura e seus pesos, este sao obtidos a partir de  W=1, x1 e x2 sao os limites de integracao,
//o integrando e do tipo f(x) pura e simplesmente. x-> pontos em que se calculara a  quadratura, w-> os pesos
template<class T> void gaulag(Vcr<T>& x, Vcr<T>& w, const T alf);//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Laguerre, da os pontos x da quadartura e seus pesos, para isto usa o parametro alfa, W=(x^alfa)*(e^-x)
//o limite de integracao vai de zero a infinito, integrando do tipo (x^alfa)*(e^-x)*f(x);
//x-> pontos de x da quadratura, w-> valores do peso, menor abcissa em x[0], maior abcissa em x[n-1]
template<class T> void gauher(Vcr<T>& x, Vcr<T>& w);//subrotina auxiliar para quadratura gaussiana
//usando polinomio de Hermite, da os pontos x da quadartura e seus pesos
//o limite de integracao vai de - infinito a + infinito, integrando do tipo (e^-(x^2))*f(x)
//maior abcissa em x[0], abcissa mais negativa em x[n-1]
template<class T> void gaujac(const T x1, const T x2, Vcr<T>& x, Vcr<T>& w, const T alf, const T bet);//subrotina auxiliar para quadratura gaussiana
//Utiliza o polinomio de Jacobi, o integrando e da forma [(1-x)^alf]*[(1+x)^bet]*f(x); Legendre e um caso especial em que alf=0 e bet=0
//Chebyshev e tambem um caso especial em que alf=bet=-1/2
//o limite de integracao vai de x1 a x2.
template<class T, T F(T,const T* const)> T quadleg(const T x1, const T x2, const T* const,const int n); //quadratura gaussiana usando Legendre, x1-> limite inferior, x2-> limite superior
//n->numero de pontos da quadratura
template<class T, T F(T,const T* const)> T quadjac(const T x1, const T x2, const T* const,const int n, const T alf, const T bet); //quadratura gaussiana usando Jacobi, x1-> limite inferior, x2-> limite superior
//n->numero de pontos da quadratura, alf e bet expoentes da funcao ponderadora-> (x^alfa)*(e^-x),
//integrando do tipo  (x^alfa)*(e^-x)*f(x);
template<class T, T F(T,const T* const)> T quadlag(const T* const,const int n, const T alf); //quadratura gaussiana usando Laguerre
//n->numero de pontos da quadratura, integracao vai de 0 a infinito, alf e o expoente da funcao ponderadora->(x^alfa)*(e^-x)
//integrando do tipo (x^alfa)*(e^-x)*f(x);
template<class T, T F(T,const T* const)> T quadher(const T* const,const int n); //quadratura gaussiana usando Hermite
//n->numero de pontos da quadratura, integracao vai de -infinito a infinito
//integrando do tipo ((e^-x)^2)*f(x);
template<class T, T F(T, const T* const)> void MelhoraChute(T&,T&, const T* const,int dinter=10);//Melhora chute para calculo de zero,
//dinter intervalo em que e feita a pesquisa para que os dois chutes "cerquem" a raiz
template<class T, T F(T, const T* const)> T FalsaCorda(T,T, const T* const,T delta=0.00001 ,T epsn=0.00001,int maxit=100);//Acha zero de funcao

template<class T, T F(T, const T* const)> T Bissecao(T,T, const T* const,T delta=0.00001 ,T epsn=0.00001,int maxit=100);//Acha zero de funcao

template<class T> T SIGN(T a,T b);//retorna um valor com a magnitude de a e sinal de b, ver NR
template<class T, T F(T, const T* const)> T zriddr(T,T, const T* const,T xacc=0.00001, int maxit=60);//Acha zero de funcao ver NR
template<class T, T F(T, const T* const)> T zbrent(T,T, const T* const,T tol=0.00001,T epsn=0.00001, int maxit=100);
template<class T, T F(T)> T zbrent2(T,T, T tol=0.00001,T epsn=0.00001, int maxit=100);
//Acha zero de funcao ver NR

template<class T> class spline{ // classe que trabalha com spline, condicao de contorno natural

	int lenth; //tamanho dos vetores
	T* vx; //vetor de abcissa
        T* vy; //vetor de ordenada
        T* d2vy; //vetor da segunda derivada do spline
        public:
               spline(int n, const T* const abs, const T* const ord); //construtor
               spline(const spline&);//construtor de copia
               ~spline(){delete[] vx;
                      delete[] vy;
                      } //destrutor
                spline& operator=(const spline&);//sobrecarga do operador =
                T operator[](int i)const{return vx[i];}//sobrecarga do operador []
                int size()const{return lenth;}
                ostream& curva(ostream&, T lim1, T lim2,int npontos) const; //saida para montagem de curva
                T Fy(T) const; //valor da funcao
                T Dy(T) const; //derivada do valor da funcao
};

template<class T, T F(T, const T* const)> T derivada(T,const T* const, T dx=0.001); //calcula a derivada de uma funcao
//a partir de um spline, dx e o valor do diferencial de x utilizado no calculo da derivada

#endif
