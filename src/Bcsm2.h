#ifndef _FBCS
#define _FBCS

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>
using namespace std;
#include "Vetor.h"
#include "Matriz.h"

class BomCentSub{ // BCS � uma  classe que trabalha com spline,
//a princ�pio ter�amos apenas dois vetores para a interpola��o, mas iremos trabalhar com quatro vetores, onde 3 fun��es
//s�o interpoladas pelo primeiro vetor
        public:
		int lenth; //tamanho dos vetores
		double* vaz; //vetor de vaz�o  em BPD
        double* head; //vetor de head em ft
        double* power; //vetor de pot�ncia em hp
        double* efic; //vetor de efici�ncia
        //double* d1head; //vetor de primeira derivada de head
        //double* d1power; //vetor de primeira derivada de pot�ncia
        //double* d1efic; //vetor de primeira derivada de efici�ncia
        double* d2head; //vetor de segunda derivada de head
        double* d2power; //vetor de segunda derivada de pot�ncia
        double* d2efic; //vetor de segunda derivada de efici�ncia
        double BEP ;//ponto de m�xima efici�ncia
        double Qzero;//ponto em que o head na bomba � m�ximo
        double freq;//freq��ncia da bomba em Hertz
        double freqnova;
        double eficM;
        double freqMinima;
        int nestag;//n�mero de est�gios
        //double d3head;
        //double d3power;
        //double d3efic;
        double Hvis;
        double Pvis;
        double Evis;
        int correcHI;

        double FalsaCorda(int,double ,double , double delta=0.000001 ,double epsn=0.000001,int maxit=100)const;//Calculo de zero de fun��o apenas para calculo de zero na FNDran

               BomCentSub(int n=0, const double* const Vvaz=0, const double* const Vhead=0,
                       const double* const Vpower=0,const double* const Vefic=0, double Vfreq=0,int nestag=1, double VeficM=100,
					    double freqcMinima=0., int vcorrecHI=1); //construtor
               BomCentSub(const BomCentSub&);//construtor de c�pia
               ~BomCentSub(){delete[] vaz;
                      delete[] head;
                      delete[] power;
                      delete[] efic;
                      //delete[] d1head;
                      //delete[] d1power;
                      //delete[] d1efic;
                      delete[] d2head;
                      delete[] d2power;
                      delete[] d2efic;} //destrutor
                BomCentSub& operator=(const BomCentSub&);//sobrecarga do operador =
                double operator[](int i)const{return vaz[i];}//sobrecarga do operador []
                double MelhorVaz()const{return BEP;}
                double MaxVaz()const{return Qzero;}
                int size()const{return lenth;}
                ostream& curva(ostream&, double Vfreq, double estag, int npontos) const; //sa�da para montagem de curva
                double Fhead(double) const;
                double Fpower(double) const;
                double Fefic(double) const;
                double Defic(double) const;
                double segundaH(int i)const{return d2head[i];}
                double segundaP(int i)const{return d2power[i];}
                double segundaE(int i)const{return d2efic[i];}
                void NovaVis(double,double,double);
};

ostream& operator<<(ostream& s, const BomCentSub&);//saida de valores da bomba */

#endif
 
