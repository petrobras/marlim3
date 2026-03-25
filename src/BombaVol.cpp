/*
 * BombaVol.cpp
 *
 *  Created on: 19 de set de 2016
 *      Author: BWZ1
 */
#include "BombaVol.h"

BomVol::BomVol(double vcap, double vfreq, double vnpoli, double VMG, double VMP, double VMC,
		                            const ProFlu vflui,const ProFluCol vfluiC){
	capacidade=vcap;
	freq=vfreq;
	npoli=vnpoli;
	fluido=vflui;
	fluicol=vfluiC;
	MGas=VMG;
	MLiqP=VMP;
	MLiqC=VMC;
	MultGas=VMG;
	MultLiqP=VMP;
	MultLiqC=VMC;
}

BomVol::BomVol(const BomVol& bombaantiga){
	capacidade=bombaantiga.capacidade;
	freq=bombaantiga.freq;
	npoli=bombaantiga.npoli;
	fluido=bombaantiga.fluido;
	MGas=bombaantiga.MGas;
	MLiqP=bombaantiga.MLiqP;
	MLiqC=bombaantiga.MLiqC;
	MultGas=bombaantiga.MultGas;
	MultLiqP=bombaantiga.MultLiqP;
	MultLiqC=bombaantiga.MultLiqC;
}

BomVol& BomVol::operator =(const BomVol& bombaantiga)
{
	if (this != &bombaantiga) {
	   capacidade=bombaantiga.capacidade;
       freq=bombaantiga.freq;
       npoli=bombaantiga.npoli;
	   fluido=bombaantiga.fluido;
	   MGas=bombaantiga.MGas;
	   MLiqP=bombaantiga.MLiqP;
	   MLiqC=bombaantiga.MLiqC;
	   MultGas=bombaantiga.MultGas;
	   MultLiqP=bombaantiga.MultLiqP;
	   MultLiqC=bombaantiga.MultLiqC;
	}
	return *this;
}

void BomVol::vazmass(double pres, double pres1,double temp,
		          double temp1,double bet, double alf, int tipo){
	if(freq<=1.)freq=0.;
	if(tipo==0){
	   double rhoO=fluido.MasEspLiq(pres, temp);
	   double rhoC=fluicol.MasEspFlu(pres, temp);
	   double rhoG=fluido.MasEspGas(pres,temp);
	   double rhoG1=fluido.MasEspGas(pres1,temp1);
	   double qvol=capacidade*(1-alf+alf*rhoG/rhoG1)*freq;
       MGas=alf*qvol*rhoG;
       MLiqP=(1.-alf)*(1.-bet)*qvol*rhoO;
       MLiqC=(1.-alf)*bet*qvol*rhoC;
       MultGas=alf*capacidade*freq*rhoG;
       MultLiqP=(1.-alf)*(1.-bet)*capacidade*freq*rhoO;
       MultLiqC=(1.-alf)*bet*capacidade*freq*rhoC;
	}
	else{
	   ProVap fluidoV;
	   double rhoO=fluidoV.MasEspLiq(pres, temp);
	   double rhoG=fluidoV.MasEspGas(pres,temp);
	   double rhoG1=fluidoV.MasEspGas(pres1,temp1);
	   double qvol=capacidade*(1-alf+alf*rhoG/rhoG1)*freq;
       MGas=alf*qvol*rhoG;
       MLiqP=(1.-alf)*qvol*rhoO;
       MLiqC=0.;
       MultGas=alf*capacidade*freq*rhoG;
       MultLiqP=(1.-alf)*capacidade*freq*rhoO;
       MultLiqC=0.;
	}

}

//template class BomVol;
