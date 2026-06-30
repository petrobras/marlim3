/*
 * BombaVolVap.cpp
 *
 *  Created on: 24 de mai de 2019
 *      Author: eduardo
 */

#include "BombaVolVap.h"

BomVolVapor::BomVolVapor(double vcap, double vfreq, double vnpoli,
		                        double VMG, double VMP){
	capacidade=vcap;
	freq=vfreq;
	npoli=vnpoli;
	MGas=VMG;
	MLiqP=VMP;
	MultGas=VMG;
	MultLiqP=VMP;
}

BomVolVapor::BomVolVapor(const BomVolVapor& bombaantiga){
	capacidade=bombaantiga.capacidade;
	freq=bombaantiga.freq;
	npoli=bombaantiga.npoli;
	MGas=bombaantiga.MGas;
	MLiqP=bombaantiga.MLiqP;
	MultGas=bombaantiga.MultGas;
	MultLiqP=bombaantiga.MultLiqP;
}

BomVolVapor& BomVolVapor::operator =(const BomVolVapor& bombaantiga)
{
	if (this != &bombaantiga) {
	   capacidade=bombaantiga.capacidade;
       freq=bombaantiga.freq;
       npoli=bombaantiga.npoli;
	   MGas=bombaantiga.MGas;
	   MLiqP=bombaantiga.MLiqP;
	   MultGas=bombaantiga.MultGas;
	   MultLiqP=bombaantiga.MultLiqP;
	}
	return *this;
}

void BomVolVapor::vazmass(double pres, double pres1,double temp,
		              double temp1, double alf){
    ProVap fluido;
	if(freq<=1.)freq=0.;
	double rhoO=fluido.MasEspLiq(pres, temp,TEMP);
	double rhoG=fluido.MasEspGas(pres,temp,TEMP);
	double rhoG1=fluido.MasEspGas(pres1,temp1,TEMP);
	double qvol=capacidade*(1-alf+alf*rhoG/rhoG1)*freq;
    MGas=alf*qvol*rhoG;
    MLiqP=(1.-alf)*qvol*rhoO;
    MultGas=alf*capacidade*freq*rhoG;
    MultLiqP=(1.-alf)*capacidade*freq*rhoO;
}


