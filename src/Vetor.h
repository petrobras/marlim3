#ifndef _VETOR
#define _VETOR
using namespace std;
#include <iostream>
#include <omp.h>
//A classe Vcr � uma classe que define um vetor com diversas opera��es �teis para vetor
//tais como adi��o, subtra��o e multiplica��o de vetores, c�lculo de normas e produto interno,
//assim como sa�das de valores

//Bibliografia utilizada para esta classe:
//C++ and Object-Oriented Numeric Computing for Scientists and Engineers, Daoqi Yang

extern int nthrdMatriz;

template<class T> class Vcr{ //Vcr � um template para vetores
	int lenth; //tamanho do vetor
	T* vr; //vetor propriamente dito, tipo T
	public:
		Vcr(int,const T* const); //construtor
		Vcr(int=0 ,T=0 ); //construtor default
		Vcr(const Vcr&); //construtor por c�pia
		~Vcr(){if(vr)delete[] vr;} //destrutor

		int size() const {return lenth;} //mostra o tamanho do vetor
		void resize(int); //redimensiona o vetor

		Vcr& operator=(const Vcr&);//sobrecarga do operador = para vetores
		T& operator[](int i)const{return vr[i];}//sobrecarga do operador []
		//T& operator[](int) const;
		Vcr& operator+=(const Vcr&);//sobrecarga do operador +=
                Vcr& operator+=(T);//sobrecarga do operador +=  com um escalar
		Vcr& operator-=(const Vcr&);//sobrecarga do operador -=
                Vcr& operator-=(T);//sobrecarga do operador -= com escalar
                Vcr operator -(T) const; //subtra��o de vetor por um escalar
		Vcr operator -(const Vcr&) const; //subtra��o de vetores
		Vcr operator -() {return Vcr<T>(lenth)-(*this);}; //mudan�a de sinal
                Vcr operator+(T) const; //soma de vetor com um escalar V+a
		Vcr operator+(const Vcr&) const; //soma de vetores
		Vcr operator/(T) const; //divis�o de um vetor por um escalar v[]/a
		Vcr operator*(T) const; //multiplica��o de um vetor por um escalar v[]*a
		Vcr operator*(const Vcr&) const; //multiplica��o de um vetor por outro
		
		ostream& saida(ostream&,const Vcr&) const; //sa�da de dois vetores em duas colunas
		T maxnorm() const; //calcula norma pelo o maior elemento do vetor
		T onenorm() const; //calcula norma 1
		T twonorm() const; //calcula norma 2
		//fun��es friends
		//template<class S> friend S dot(const Vcr<S>&, const Vcr<S>&); //produto interno
              //template<class S> friend ostream& operator<<(ostream& s, const Vcr<S>&);
					
};



//Operadores adicionais, sobrecarregados, que n�o consegui colocar como friend
/*template<class S> S dot(const Vcr<S>&, const Vcr<S>&); //produto interno
template<class S> S dot(S*, S*, int); //sobrecarga do produto interno para array
template<class S> ostream& operator<<(ostream& s, const Vcr<S>&);//saida de valores de vetor
template<class S> Vcr<S> operator*(S, const Vcr<S>&); // Calculo da multiplica��o de um escalar por um vetor a*v[]*/
double dot(const Vcr<double>&, const Vcr<double>&); //produto interno
double dot(double*, double*, int); //sobrecarga do produto interno para array
ostream& operator<<(ostream& s, const Vcr<double>&);//saida de valores de vetor
Vcr<double> operator*(double, const Vcr<double>&); // Calculo da multiplica��o de um escalar por um vetor a*v[]

#endif
