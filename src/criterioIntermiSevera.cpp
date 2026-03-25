/*
 * criterioIntermiSevera.cpp
 *
 *  Created on: 5 de jan. de 2026
 *      Author: Eduardo
 */


#include "criterioIntermiSevera.h"

critInterSev::critInterSev() {
	criterio=-1;
	indAcumulaIni=0;
	indAcumulaFim=0;
	alfPenetra=0.1;

	compAcumula=0.;
	volumeAcumula=0.;
	cotaAcumulo=0.;
	zMed=0.;
	alfMedAcumula=0.;
	area0=0;
	tempMedAcumula=0.;
	RGas=((28.9625)*98066.5)/(8.0465*1000);
	rhog0=0;
	rhol0=0;
	rholR=0;
	rhogMedAcumula=0.;
	rholMedAcumula=0.;
	pMedAcumula=0.;
	pJus=0.;
	ugs0=0.;
	uls0=0.;
	massLiq0=0.;
	massGas0=0.;
	uslCol=0.;
	qualidadeArranjo=0.;
	cotaColuna=0.;
	indFim=0;
}

critInterSev::critInterSev(Cel* celula, int vini, int vfim, int fimcota, int vtipoCrit, double valfPenetra) {

	criterio=vtipoCrit;
	indAcumulaIni=vini;
	indAcumulaFim=vfim;
	indFim=fimcota;
	alfPenetra=valfPenetra;

	compAcumula=0.;
	volumeAcumula=0.;
	cotaAcumulo=0.;
	zMed=0.;
	alfMedAcumula=0.;
	area0=celula[vini].duto.area;
	tempMedAcumula=0.;
	RGas=((celula[vini].flui.rDgL*celula[vini].flui.Deng*28.9625)*98066.5)/(8.0465*1000);
	rhog0=celula[vini].flui.MasEspGas(celula[vini].pres, celula[vini].temp);
	rhol0=celula[vini].flui.MasEspLiq(celula[vini].pres, celula[vini].temp);
	rholR=celula[vfim].flui.MasEspLiq(celula[vfim].pres, celula[vfim].temp);
	rhogMedAcumula=0.;
	rholMedAcumula=0.;
	pMedAcumula=0.;
	pJus=celula[vfim].pres;
	ugs0=celula[vini].QG/celula[vini].duto.area;
	uls0=celula[vini].QL/celula[vini].duto.area;
	massLiq0=celula[vini].Mliqini;
	massGas0=celula[vini].MC-celula[vini].Mliqini;
	uslCol=celula[vfim].QL/celula[vini].duto.area;
	qualidadeArranjo=0.;
	for(int i=vini; i<=vfim;i++){
		compAcumula+=celula[i].dx;
		volumeAcumula+=celula[i].dx*celula[i].duto.area;
		cotaAcumulo+=celula[i].dx*sinl(celula[i].duto.teta);
		double pres=celula[i].pres;
		double temp=celula[i].temp;
		zMed+=celula[i].flui.Zdran(pres, temp)*celula[i].dx*celula[i].duto.area;
		alfMedAcumula+=celula[i].alf*celula[i].dx*celula[i].duto.area;
		rhogMedAcumula+=celula[i].flui.MasEspGas(pres, temp)*celula[i].dx*celula[i].duto.area;
		rholMedAcumula+=celula[i].flui.MasEspLiq(pres, temp)*celula[i].dx*celula[i].duto.area;
		pMedAcumula+=celula[i].pres*celula[i].dx*celula[i].duto.area;
		tempMedAcumula+=celula[i].temp*celula[i].dx*celula[i].duto.area;
		if(celula[i].arranjo==-1)qualidadeArranjo+=celula[i].dx;
	}
	zMed/=volumeAcumula;
	alfMedAcumula/=volumeAcumula;
	rhogMedAcumula/=volumeAcumula;
	rholMedAcumula/=volumeAcumula;
	pMedAcumula/=volumeAcumula;
	tempMedAcumula/=volumeAcumula;
	qualidadeArranjo/=compAcumula;
	cotaColuna=0.;
	for(int i=vfim; i<indFim;i++){
		cotaColuna+=celula[i].dx*sinl(celula[i].duto.teta);
	}
}

critInterSev::critInterSev(const critInterSev& temp) {
	criterio=temp.criterio;
	indAcumulaIni=temp.indAcumulaIni;
	indAcumulaFim=temp.indAcumulaFim;
	indFim=temp.indFim;
	alfPenetra=temp.alfPenetra;

	compAcumula=temp.compAcumula;
	volumeAcumula=temp.volumeAcumula;
	cotaAcumulo=temp.cotaAcumulo;
	zMed=temp.zMed;
	alfMedAcumula=temp.alfMedAcumula;
	area0=temp.area0;
	tempMedAcumula=temp.tempMedAcumula;
	RGas=temp.RGas;
	rhog0=temp.rhog0;
	rhol0=temp.rhol0;
	rholR=temp.rholR;
	rhogMedAcumula=temp.rhogMedAcumula;
	rholMedAcumula=temp.rholMedAcumula;
	pMedAcumula=temp.pMedAcumula;
	pJus=temp.pJus;
	ugs0=temp.ugs0;
	uls0=temp.uls0;
	massLiq0=temp.massLiq0;
	massGas0=temp.massGas0;
	uslCol=temp.uslCol;
	qualidadeArranjo=temp.qualidadeArranjo;
	cotaColuna=temp.cotaColuna;
}

critInterSev& critInterSev::operator=(const critInterSev& temp) {
	if (this != &temp) {
		criterio=temp.criterio;
		indAcumulaIni=temp.indAcumulaIni;
		indAcumulaFim=temp.indAcumulaFim;
		indFim=temp.indFim;
		alfPenetra=temp.alfPenetra;

		compAcumula=temp.compAcumula;
		volumeAcumula=temp.volumeAcumula;
		cotaAcumulo=temp.cotaAcumulo;
		zMed=temp.zMed;
		alfMedAcumula=temp.alfMedAcumula;
		area0=temp.area0;
		tempMedAcumula=temp.tempMedAcumula;
		RGas=temp.RGas;
		rhog0=temp.rhog0;
		rhol0=temp.rhol0;
		rholR=temp.rholR;
		rhogMedAcumula=temp.rhogMedAcumula;
		rholMedAcumula=temp.rholMedAcumula;
		pMedAcumula=temp.pMedAcumula;
		pJus=temp.pJus;
		ugs0=temp.ugs0;
		uls0=temp.uls0;
		massLiq0=temp.massLiq0;
		massGas0=temp.massGas0;
		uslCol=temp.uslCol;
		qualidadeArranjo=temp.qualidadeArranjo;
		cotaColuna=temp.cotaColuna;
	}

	return *this;
}

void critInterSev::atualiza(Cel* celula) {
	int vini=indAcumulaIni;
	int vfim=indAcumulaFim;
	zMed=0.;
	alfMedAcumula=0.;
	tempMedAcumula=0.;
	RGas=((celula[vini].flui.rDgL*celula[vini].flui.Deng*28.9625)*98066.5)/(8.0465*1000);
	rhog0=celula[vini].flui.MasEspGas(celula[vini].pres, celula[vini].temp);
	rhol0=celula[vini].flui.MasEspLiq(celula[vini].pres, celula[vini].temp);
	rholR=celula[vfim].flui.MasEspLiq(celula[vfim].pres, celula[vfim].temp);
	rhogMedAcumula=0.;
	rholMedAcumula=0.;
	pMedAcumula=0.;
	pJus=celula[vfim].pres;
	ugs0=celula[vini].QG/celula[vini].duto.area;
	uls0=celula[vini].QL/celula[vini].duto.area;
	massLiq0=celula[vini].Mliqini;
	massGas0=celula[vini].MC-celula[vini].Mliqini;
	uslCol=celula[vfim].QL/celula[vini].duto.area;
	qualidadeArranjo=0.;
	for(int i=vini; i<=vfim;i++){
		double pres=celula[i].pres;
		double temp=celula[i].temp;
		zMed+=celula[i].flui.Zdran(pres, temp)*celula[i].dx*celula[i].duto.area;
		alfMedAcumula+=celula[i].alf*celula[i].dx*celula[i].duto.area;
		rhogMedAcumula+=celula[i].flui.MasEspGas(pres, temp)*celula[i].dx*celula[i].duto.area;
		rholMedAcumula+=celula[i].flui.MasEspLiq(pres, temp)*celula[i].dx*celula[i].duto.area;
		pMedAcumula+=celula[i].pres*celula[i].dx*celula[i].duto.area;
		tempMedAcumula+=celula[i].temp*celula[i].dx*celula[i].duto.area;
		if(celula[i].arranjo==-1)qualidadeArranjo+=celula[i].dx;
	}
	zMed/=volumeAcumula;
	alfMedAcumula/=volumeAcumula;
	rhogMedAcumula/=volumeAcumula;
	rholMedAcumula/=volumeAcumula;
	pMedAcumula/=volumeAcumula;
	tempMedAcumula/=volumeAcumula;
	qualidadeArranjo/=compAcumula;
}


double critInterSev::uls0Boe(Cel* celula){
	return  uls0-(celula[indAcumulaFim].duto.area/area0)*rhog0*area0*zMed*RGas*tempMedAcumula*ugs0/(rhol0*9.82*alfMedAcumula*volumeAcumula);
}

double critInterSev::difPresTeB(Cel* celula){
	return celula[indFim].pres* 98066.52-(alfMedAcumula*volumeAcumula/(alfPenetra*celula[indAcumulaFim].duto.area)-cotaColuna)*rholR*9.82;
}

double critInterSev::testaCriterio(int tipo, Cel* celula){
	//if(tipo==0)return uls0Boe(celula);
	//else
    switch ( tipo )
    {
       case 0:
    	  return uls0Boe(celula);
          break;
       case 1:
    	  return uls0Boe(celula);
          break;
       default:
    	   logger.log(LOGGER_AVISO, LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION,
    	       	      "#################Criterio de Intermitencia Inexistente##############################",
    	       	      	    							     			      		     "", "");
    	   return 0;
    }
}
