/*
 * FonteMasVap.cpp
 *
 *  Created on: 21 de mai de 2019
 *      Author: eduardo
 */

#include "FonteMasVap.h"

IPRVap::IPRVap(double vpres, double vtres, double vij){
 Pres=vpres;
 Tres=vtres;
 ij=vij;
 deriP=0.;
 deriG=0.;
}

IPRVap::IPRVap(const IPRVap& vipr){//construtor de c�pia
 Pres=vipr.Pres;
 Tres=vipr.Tres;
 ij=vipr.ij;
 deriP=vipr.deriP;
 deriG=vipr.deriG;
}

IPRVap& IPRVap::operator =(const IPRVap& vipr){
 Pres=vipr.Pres;
 Tres=vipr.Tres;
 ij=vipr.ij;
 deriP=vipr.deriP;
 deriG=vipr.deriG;
 return *this;
}

double IPRVap::VMas(const double& pfundo, const double& tfundo){
//retorna a vaz�o m�ssica para uma dada press�o e temperatura de fundo. kg/s
//Determina o valor da vaz�o m�ssica produzida por um reservat�rio em kg/s
//para uma determinada press�o de fundo (kgf/cm2), o ip � dado em m3/d
//kgf/cm2, a IPR � obtida por Voguel na condi��o de saturado e linear no
//subsaturado, a press�o de reservat�rio em kgf/
//cm2, a temperatura de reservat�rio em C, observe que na IPR � adicionada
//a possibilidade que o po�o beba, neste caso, poder� ocorrer escoamento
//descendente

 double var;

 double Rhogs;
 double Rhols;
 double multiplicador;

   var = ij*(Pres - pfundo);
   deriG=deriP=-ij;
  multiplicador =1./86400;

 deriG=deriP=multiplicador*deriP;
 return multiplicador*var;
}

double IPRVap::MasL(const double& pfundo, const double& tfundo){
//retorna a vaz�o m�ssica de l�quido para uma dada press�o e temperatura de fundo. kg/s
//Determina o valor da vaz�o m�ssica produzida por um reservat�rio em kg/s
//para uma determinada press�o de fundo (kgf/cm2), o ip � dado em m3/d
//kgf/cm2, a IPR � obtida por Voguel na condi��o de saturado e linear no
//subsaturado, a press�o de reservat�rio em kgf/
//cm2, a temperatura de reservat�rio em C, observe que na IPR � adicionada
//a possibilidade que o po�o beba, neste caso, poder� ocorrer escoamento
//descendente

	ProVap flup;
	double tit=flup.Titulo(pfundo,tfundo,TEMP);
	deriP*=(1.-tit);
	return (1.-tit)*VMas(pfundo,tfundo);
}

double IPRVap::MasG(const double& pfundo, const double& tfundo){
//retorna a vaz�o m�ssica de l�quido para uma dada press�o e temperatura de fundo. kg/s
//Determina o valor da vaz�o m�ssica produzida por um reservat�rio em kg/s
//para uma determinada press�o de fundo (kgf/cm2), o ip � dado em m3/d
//kgf/cm2, a IPR � obtida por Voguel na condi��o de saturado e linear no
//subsaturado, a press�o de reservat�rio em kgf/
//cm2, a temperatura de reservat�rio em C, observe que na IPR � adicionada
//a possibilidade que o po�o beba, neste caso, poder� ocorrer escoamento
//descendente

	ProVap flup;
    double tit=flup.Titulo(pfundo,tfundo,TEMP);
	deriG*=tit;
	return tit*VMas(pfundo,tfundo);
}


InjMultVap::InjMultVap(double vmp,double vtemp){
	 Mass=vmp;
	 temp=vtemp;
}

InjMultVap::InjMultVap(const InjMultVap& vinj){//construtor de c�pia
	 Mass=vinj.Mass;
	 temp=vinj.temp;
}

InjMultVap& InjMultVap::operator =(const InjMultVap& vinj){
	 Mass=vinj.Mass;
	 temp=vinj.temp;
  return *this;
}

double InjMultVap::VMas(const double& pres, const double& temp){
//Para o caso de uma fonte de l�quido mais g�s, vaz�o injetada em Sm3/d, vaz�o m�ssica em kg/s

 //double Rhogs = this->FluidoPro.MasEspGas(1, 15);
 //double Rhols = (1000.*141.5/(131.5 + this->FluidoPro[1]))*(1-this->FluidoPro[4])+1000.*this->FluidoPro[5]*this->FluidoPro[4];
 //double multiplicador = (Rhols + this->FluidoPro[2]*Rhogs*(1-this->FluidoPro[4]))/86400.;
 //return (multiplicador*(1.-bet)+bet*fluidocol.MasEspFlu(1.,15))*QLiq/86400.;
	return Mass;
}

//template class IPRVap;
//template class InjMultVap;


