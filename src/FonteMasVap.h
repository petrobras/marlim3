/*
 * FonteMasVap.h
 *
 *  Created on: 21 de mai de 2019
 *      Author: eduardo
 */

#ifndef FONTEMASVAP_H_
#define FONTEMASVAP_H_

#include <math.h>

#include <algorithm>
#include <fstream>
#include <complex>
using namespace std;
#include "PropVapor.h"



class IPRVap{//classe para fonte do tipo IPR, herda a interface de AbsFonte
	public:
        double Pres; //press�o de reservat�rio kgf/cm2
		double Tres; //temperatura de reservat�rio C
        double ij;//�ndice de injetividade m3/d/kgf/cm2
        double deriP;
        double deriG;
		IPRVap(double=0, double=0, double=0); //construtor default
		IPRVap(const IPRVap&);//construtor de c�pia
        IPRVap& operator=(const IPRVap&);//sobrecarga do operador =

        double VMas(const double&, const double&);//retorna a vaz�o m�ssica para uma dada press�o e temperatura de fundo. kg/s
        double MasL(const double&, const double&);//retorna a vaz�o m�ssica de L�quido para uma dada press�o e temperatura de fundo. kg/s
        double MasG(const double&, const double&);//retorna a vaz�o m�ssica de G�s para uma dada press�o e temperatura de fundo. kg/s
};

class InjMultVap{//classe para fonte do tipo inje��o de l�quido, herda a interface de AbsFonte
//alteracao7
        public:
        double Mass;
        double temp;
		InjMultVap(double=0,double=0); //construtor default
		InjMultVap(const InjMultVap&);//construtor de c�pia
        InjMultVap& operator=(const InjMultVap&);//sobrecarga do operador =
        double VMas(const double& pres, const double& temp);//retorna a vaz�o m�ssica kg/s
};




#endif /* FONTEMASVAP_H_ */
