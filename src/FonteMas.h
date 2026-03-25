#ifndef _FMAS
#define _FMAS

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
using namespace std;
#include "PropFlu.h"
#include "PropFluCol.h"

class AbsFonte{ //classe abstrata para fonte de massa, algumas fun��es virtuais ser�o colocadas nesta classe,
	//esta servir� de interface para outras classes de fonte com IPR e sem IPR
       //	protected://as vari�veis declaradas aqui n�o ser�o privadas para classes derivadas
               // ProFlu FluidoPro; //Fluido que eventualmente ser� produzido pela fonte de massa
        public:
                //ProFlu FluidoPro; //Fluido que eventualmente ser� produzido pela fonte de massa
                virtual double VMas(const double&, const double&) =0;//retorna o valor da vaz�o m�ssica total para uma dada press�o e temperatura
                //ProFlu FonProp()const{return FluidoPro;}//retorna as propriedades do fluido da fonte
                //virtual void MudaFlu(ProFlu& vfluido)=0;//muda o fluido da fonte
};

class IPR: public AbsFonte{//classe para fonte do tipo IPR, herda a interface de AbsFonte
	public:
        double Pres; //press�o de reservat�rio kgf/cm2
		double Tres; //temperatura de reservat�rio C
        double ip;//�ndice de produtividade m3/d/kgf/cm2
        double qMax;
        double ij;//�ndice de injetividade m3/d/kgf/cm2
        double Psat;//press�o de satura��o do fluido na temperatura de reservat�rio kgf/cm2
        double deriP;
        double deriG;
        double deriC;
        int tipoIPR;
        ProFlu FluidoPro;
		IPR(double, double, double,double,double,ProFlu, int tipoip=1); //construtor
		IPR(double=0, double=0, double=0,double=0, double=0, int tipoip=1); //construtor default
		IPR(const IPR&);//construtor de c�pia
        IPR& operator=(const IPR&);//sobrecarga do operador =
        void MudaFlu(ProFlu& vfluido){this->FluidoPro=vfluido; Psat=vfluido.PB(Pres, Tres)/(0.9678411*14.69595);}//muda o fluido da fonte
        //void MudaVazFlu(double& vaz){this->FluidoPro.MudaVazMas(vaz);}//muda vaz�o armazenado no fluido de ipr
        double VMas(const double&, const double&);//retorna a vaz�o m�ssica para uma dada press�o e temperatura de fundo. kg/s
        double MasL(const double&, const double&);//retorna a vaz�o m�ssica de L�quido para uma dada press�o e temperatura de fundo. kg/s
        double MasG(const double&, const double&);//retorna a vaz�o m�ssica de G�s para uma dada press�o e temperatura de fundo. kg/s
        //double* contorno(const double&, const double&);//retorna os coeficientes para serem utilizados em uma eventual condi��o de contorno
};

class InjGas: public AbsFonte{//classe para fonte do tipo inje��o de g�s, herda a interface de AbsFonte
        public:
        double QGas; //vaz�o de g�s em condi��es normais  sm3/d
        double razCompGas;
        double temp;
        int tipoflu;
        int seco;
        ProFlu FluidoPro;
        ProFluCol fluidocol;
		InjGas(double,double, ProFlu, ProFluCol=ProFluCol(),double vqcomp=0.); //construtor
		InjGas(double=0,double=25, double vqcomp=0.); //construtor default
		InjGas(const InjGas&);//construtor de c�pia
        InjGas& operator=(const InjGas&);//sobrecarga do operador =
        void MudaFlu(ProFlu& vfluido){this->FluidoPro=vfluido;}//muda o fluido da fonte
        double VMas(const double& pres, const double& temp);//retorna a vaz�o m�ssica
};


class InjLiq: public AbsFonte{//classe para fonte do tipo inje��o de l�quido, herda a interface de AbsFonte

        public:
	    ProFluCol fluidocol;
        double QLiq; //vaz�o de g�s em condi��es normais  sm3/d
        double bet;
        double temp;
        ProFlu FluidoPro;
		InjLiq(double, double, double, ProFlu, ProFluCol); //construtor
		InjLiq(double=0, double=0, double=0); //construtor default
		InjLiq(const InjLiq&);//construtor de c�pia
        InjLiq& operator=(const InjLiq&);//sobrecarga do operador =
        void MudaFlu(ProFlu& vfluido){this->FluidoPro=vfluido;}//muda o fluido da fonte
        double VMas(const double& pres, const double& temp);//retorna a vaz�o m�ssica kg/s
};

class InjMult: public AbsFonte{//classe para fonte do tipo inje��o de l�quido, herda a interface de AbsFonte
//alteracao7
        public:
	    ProFluCol fluidocol;
        double MassP;
        double MassC;
        double MassG;
        double temp;
        int condTermo;
        ProFlu FluidoPro;
		InjMult(double,double, double, double, ProFlu, ProFluCol); //construtor
		InjMult(double=0,double=0, double=0, double=0); //construtor default
		InjMult(const InjMult&);//construtor de c�pia
        InjMult& operator=(const InjMult&);//sobrecarga do operador =
        void MudaFlu(ProFlu& vfluido){this->FluidoPro=vfluido;}//muda o fluido da fonte
        double VMas(const double& pres, const double& temp);//retorna a vaz�o m�ssica kg/s
};





#endif
