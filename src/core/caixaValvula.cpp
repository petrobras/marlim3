/*
 * caixaValvula.cpp
 *
 *  Created on: 15 de set. de 2026
 *      Author: Eduardo
 */


#include "caixaValvula.h"

caixaValv::caixaValv(int vpos,double vlCaixa, int vnMon,int vnJus, int vnfonte, double vtempF, int vformac, int vlito,
		DadosGeo vsecaoTrans,TransCal vtransfer, varGlob1D *Vvg1dSP){

	posic=vpos;
	lCaixa=vlCaixa;
	formac=vformac;
	lito=vlito;
	secaoTrans=vsecaoTrans;
	transfer=vtransfer;
	nMon=vnMon;
	if(nMon>0){
		serieAberturaMon=new double [nMon];
		tempMon=new double [nMon];
		for(int i=0;i<nMon;i++){
			serieAberturaMon[i]=0.;
			tempMon[i]=0.;
		}
	}
	nJus=vnJus;
	if(nJus>0){
		serieAberturaJus=new double [nJus];
		tempJus=new double [nJus];
		for(int i=0;i<nJus;i++){
			serieAberturaJus[i]=0.;
			tempJus[i]=0.;
		}
	}
	abreMon=-1.;
	abreJus=-1.;
	alf=-1.;
	bet=-1.;
	tit=-1.;
	pres=-1.;
	temp=-1.;
	tempoResIni=-1.;
	tempoRes=0.;

	massLiqP=0.;
	massLiqC=0.;
	massGas=0.;

	nfonte=vnfonte;
	if(nfonte==0){
		fonteMP=0;
		fonteMC=0;
		fonteMG=0;
		tempFonte=0;
	}
	else{
		fonteMP=new double[nfonte];
		fonteMG=new double[nfonte];
		fonteMC=new double[nfonte];
		tempFonte=new double[nfonte];
	}
	temperaturaFonte=vtempF;

	FluidoPro=ProFlu();
	fluidocol=ProFluCol();
	vg1dSP=Vvg1dSP;
}

caixaValv::caixaValv(const caixaValv& vcxV){

	posic=vcxV.posic;
	lCaixa=vcxV.lCaixa;
	formac=vcxV.formac;
	lito=vcxV.lito;
	secaoTrans=vcxV.secaoTrans;
	transfer=vcxV.transfer;
	nMon=vcxV.nMon;
	if(nMon>0){
		serieAberturaMon=new double [nMon];
		tempMon=new double [nMon];
		for(int i=0;i<nMon;i++){
			serieAberturaMon[i]=vcxV.serieAberturaMon[i];
			tempMon[i]=vcxV.tempMon[i];
		}
	}
	nJus=vcxV.nJus;
	if(nJus>0){
		serieAberturaJus=new double [nJus];
		tempJus=new double [nJus];
		for(int i=0;i<nJus;i++){
			serieAberturaJus[i]=vcxV.serieAberturaJus[i];
			tempJus[i]=vcxV.tempJus[i];
		}
	}
	nfonte=vcxV.nfonte;
	temperaturaFonte=vcxV.temperaturaFonte;
	if(nfonte>0){
		fonteMP=new double [nfonte];
		fonteMC=new double [nfonte];
		fonteMG=new double [nfonte];
		tempFonte=new double [nfonte];
		for(int i=0;i<nfonte;i++){
			fonteMP[i]=vcxV.fonteMP[i];
			fonteMC[i]=vcxV.fonteMC[i];
			fonteMG[i]=vcxV.fonteMG[i];
			tempFonte[i]=vcxV.tempFonte[i];
		}
	}
	abreMon=vcxV.abreMon;
	abreJus=vcxV.abreJus;
	alf=vcxV.alf;
	bet=vcxV.bet;
	tit=vcxV.tit;
	pres=vcxV.pres;
	temp=vcxV.temp;
	tempoResIni=vcxV.tempoResIni;
	tempoRes=vcxV.tempoRes;

	massLiqP=vcxV.massLiqP;
	massLiqC=vcxV.massLiqC;
	massGas=vcxV.massGas;

	FluidoPro=vcxV.FluidoPro;
	fluidocol=vcxV.fluidocol;
	vg1dSP=vcxV.vg1dSP;
}

caixaValv::caixaValv(caixaValv&& vcxV) noexcept
{

     posic = vcxV.posic;
     lCaixa = vcxV.lCaixa;
 	 formac=vcxV.formac;
 	 lito=vcxV.lito;
     secaoTrans = vcxV.secaoTrans;
     transfer= vcxV.transfer;
     nMon = vcxV.nMon;
     serieAberturaMon = vcxV.serieAberturaMon;
     tempMon = vcxV.tempMon;
     abreMon= vcxV.abreMon;
     nJus= vcxV.nJus;
     serieAberturaJus = vcxV.serieAberturaJus;
     tempJus= vcxV.tempJus;
     abreJus= vcxV.abreJus;
     alf = vcxV.alf;
     bet = vcxV.bet;
     tit = vcxV.tit;
     pres= vcxV.pres;
     temp= vcxV.temp;
     tempoResIni = vcxV.tempoResIni;
     tempoRes= vcxV.tempoRes;
     massLiqP= vcxV.massLiqP;
     massGas= vcxV.massGas;
     massLiqC= vcxV.massLiqC;
     vg1dSP= vcxV.vg1dSP;
     FluidoPro= vcxV.FluidoPro;
     fluidocol= vcxV.fluidocol;
	 nfonte=vcxV.nfonte;
	 temperaturaFonte=vcxV.temperaturaFonte;
	 fonteMP=vcxV.fonteMP;
	 fonteMC=vcxV.fonteMC;
	 fonteMG=vcxV.fonteMG;
	 tempFonte=vcxV.tempFonte;

    // deixa a origem em estado válido, mas vazio
    vcxV.serieAberturaMon = nullptr;
    vcxV.tempMon          = nullptr;
    vcxV.serieAberturaJus = nullptr;
    vcxV.tempJus          = nullptr;
    vcxV.nMon             = 0;
    vcxV.nJus             = 0;
    vcxV.vg1dSP           = nullptr;
	vcxV.nfonte=0;
	vcxV.fonteMP= nullptr;
	vcxV.fonteMC= nullptr;
	vcxV.fonteMG= nullptr;
	vcxV.tempFonte= nullptr;
}


caixaValv& caixaValv::operator =(const caixaValv& vcxV){
	if (this != &vcxV) {

		if(nMon>0){
			delete[] serieAberturaMon;
			delete[] tempMon;
		}
		if(nJus>0){
			delete[] serieAberturaJus;
			delete[] tempJus;
		}
		if(nfonte>0){
			delete[] fonteMP;
			delete[] fonteMC;
			delete[] fonteMG;
			delete[] tempFonte;
		}

		lCaixa=vcxV.lCaixa;
		secaoTrans=vcxV.secaoTrans;
		formac=vcxV.formac;
		lito=vcxV.lito;
		transfer=vcxV.transfer;
		nMon=vcxV.nMon;
		if(nMon>0){
			serieAberturaMon=new double [nMon];
			tempMon=new double [nMon];
			for(int i=0;i<nMon;i++){
				serieAberturaMon[i]=vcxV.serieAberturaMon[i];
				tempMon[i]=vcxV.tempMon[i];
			}
		}
		nJus=vcxV.nJus;
		if(nJus>0){
			serieAberturaJus=new double [nJus];
			tempJus=new double [nJus];
			for(int i=0;i<nJus;i++){
				serieAberturaJus[i]=vcxV.serieAberturaJus[i];
				tempJus[i]=vcxV.tempJus[i];
			}
		}

		nfonte=vcxV.nfonte;
		temperaturaFonte=vcxV.temperaturaFonte;
		if(nfonte>0){
			fonteMP=new double [nfonte];
			fonteMC=new double [nfonte];
			fonteMG=new double [nfonte];
			tempFonte=new double [nfonte];
			for(int i=0;i<nfonte;i++){
				fonteMP[i]=vcxV.fonteMP[i];
				fonteMC[i]=vcxV.fonteMC[i];
				fonteMG[i]=vcxV.fonteMG[i];
				tempFonte[i]=vcxV.tempFonte[i];
			}
		}

		posic=vcxV.posic;
		abreMon=vcxV.abreMon;
		abreJus=vcxV.abreJus;
		alf=vcxV.alf;
		bet=vcxV.bet;
		tit=vcxV.tit;
		pres=vcxV.pres;
		temp=vcxV.temp;
		tempoResIni=vcxV.tempoResIni;
		tempoRes=vcxV.tempoRes;
		massLiqP=vcxV.massLiqP;
		massLiqC=vcxV.massLiqC;
		massGas=vcxV.massGas;
		FluidoPro=vcxV.FluidoPro;
		fluidocol=vcxV.fluidocol;
		vg1dSP=vcxV.vg1dSP;
	}
	return *this;
}

caixaValv& caixaValv::operator=(caixaValv&& vcxV) noexcept {
    if (this != &vcxV) {

        // 1) libera recursos atuais do *this*
        delete[] serieAberturaMon;
        delete[] tempMon;
        delete[] serieAberturaJus;
		delete[] fonteMP;
		delete[] fonteMC;
		delete[] fonteMG;
		delete[] tempFonte;

        // 2) rouba os recursos de vcxV
        posic = vcxV.posic;
        lCaixa  = vcxV.lCaixa;
    	formac=vcxV.formac;
    	lito=vcxV.lito;
        secaoTrans = vcxV.secaoTrans;
        transfer = vcxV.transfer;
        nMon= vcxV.nMon;
        serieAberturaMon = vcxV.serieAberturaMon;
        tempMon= vcxV.tempMon;
        abreMon = vcxV.abreMon;
        nJus= vcxV.nJus;
        serieAberturaJus = vcxV.serieAberturaJus;
        tempJus= vcxV.tempJus;
        abreJus= vcxV.abreJus;
        alf = vcxV.alf;
        bet= vcxV.bet;
        tit = vcxV.tit;
        pres = vcxV.pres;
        temp= vcxV.temp;
        tempoResIni= vcxV.tempoResIni;
        tempoRes = vcxV.tempoRes;
        massLiqP = vcxV.massLiqP;
        massGas = vcxV.massGas;
        massLiqC= vcxV.massLiqC;
        vg1dSP= vcxV.vg1dSP;
        FluidoPro= vcxV.FluidoPro;
        fluidocol= vcxV.fluidocol;
		nfonte=vcxV.nfonte;
		temperaturaFonte=vcxV.temperaturaFonte;
		fonteMP=vcxV.fonteMP;
		fonteMC=vcxV.fonteMC;
		fonteMG=vcxV.fonteMG;
		tempFonte=vcxV.tempFonte;

        // 3) deixa vcxV em estado válido, mas vazio
        vcxV.serieAberturaMon = nullptr;
        vcxV.tempMon = nullptr;
        vcxV.serieAberturaJus = nullptr;
        vcxV.tempJus= nullptr;
        vcxV.nMon = 0;
        vcxV.nJus = 0;
        vcxV.vg1dSP = nullptr;
		vcxV.nfonte=0;
		vcxV.fonteMP= nullptr;
		vcxV.fonteMC= nullptr;
		vcxV.fonteMG= nullptr;
		vcxV.tempFonte= nullptr;
    }
    return *this;
}

void caixaValv::propDeri(double p, double t, double& FW, double& bo, double& rs, double& termoRS, double& rhop,double& rhoc){

    double ba;
	if (FluidoPro.RGO < 1e7) {
        rs = FluidoPro.RS(p, t);
        bo = FluidoPro.BOFunc(p, t, rs);
        ba = FluidoPro.BAFunc(p, t);
        rs = rs * 6.29 / 35.31467;
    } else {
        bo = 1;
        rs = 0;
        ba = 0.;
    }
    // BSW in-situ da celula anterior, na marcha, a i-esima celula
    FW = FluidoPro.BSW * ba / (bo + ba * FluidoPro.BSW - FluidoPro.BSW * bo);
    termoRS=(1-FW)*rs*FluidoPro.Deng*1.225/bo;
    rhop=FluidoPro.MasEspLiq(p, t, rs);
    rhoc=fluidocol.MasEspFlu(p, t);
}


void caixaValv::indraz(int &ind, double &raz, double tempo, int parserie , double *serietemp) {

	for (int i = 0; i <= parserie - 1; i++) {
		if (i < parserie - 1) {
			if (tempo >= serietemp[i] && tempo < serietemp[i + 1]) {
				ind = i;
				raz = 1
						- (tempo - serietemp[i])
								/ (serietemp[i + 1] - serietemp[i]);
				break;
			}
		} else if (i == parserie - 1) {
			ind = i;
			raz = 1;
		}
	}


}

void caixaValv::atualizaAberturaCaixa(){

    int ind;
	double raz;

	indraz(ind, raz, (*vg1dSP).lixo5, nMon, tempMon);
	double aberinf = serieAberturaMon[ind];
	double abersup;
	if (ind < nMon - 1)
		abersup = serieAberturaMon[ind + 1];
	else
		abersup = serieAberturaMon[ind];
    abreMon=(aberinf * raz + (1 - raz) * abersup);

	indraz(ind, raz, (*vg1dSP).lixo5, nJus, tempJus);
	aberinf = serieAberturaJus[ind];
	if (ind < nJus - 1)
		abersup = serieAberturaJus[ind + 1];
	else
		abersup = serieAberturaJus[ind];
    abreJus=(aberinf * raz + (1 - raz) * abersup);
}

void caixaValv::atualizaFontesCaixa(double& Mass, double *serieM){

    int ind;
	double raz;

	indraz(ind, raz, (*vg1dSP).lixo5, nfonte, tempFonte);
	double minf = serieM[ind];
	double msup;
	if (ind < nfonte - 1)
		msup = serieM[ind + 1];
	else
		msup = serieM[ind];
    Mass=(minf * raz + (1 - raz) * msup);
}

double caixaValv::calcAbertura(){

	atualizaAberturaCaixa();
	if(abreMon<1e-15 || abreJus<1e-15) return 0.;
	else{
		if(abreMon<abreJus)return abreMon;
		else return abreJus;
	}
}

void caixaValv::estadotermo(double pmon, double tmon, double alfmon, double betmon, double massmon, double massLmon, ProFlu fluimon, ProFluCol fluiCmon,
			double pjus, double tjus, double alfjus, double betjus, double massjus, double massLjus, ProFlu fluijus, ProFluCol fluiCjus, double dt){

	double vazMass;
	double vazMassL;
	atualizaAberturaCaixa();
	if(abreMon>1e-15 && abreJus>1e-15){
		tempoResIni=-1.;
		if(massmon>0. && massjus>0.){
			FluidoPro=fluimon;
			fluidocol=fluiCmon;
			alf=alfmon;
			bet=betmon;
			pres=pmon;
			temp=tmon;
			vazMass=massmon;
			vazMassL=massLmon;
		}
		else if(massmon<0. && massjus<0.){
			FluidoPro=fluijus;
			fluidocol=fluiCjus;
			alf=alfjus;
			bet=betjus;
			pres=pjus;
			temp=tjus;
			vazMass=massjus;
			vazMassL=massLjus;
		}
		else if(fabs(massmon)>=fabs(massjus)){
			FluidoPro=fluimon;
			fluidocol=fluiCmon;
			alf=alfmon;
			bet=betmon;
			pres=pmon;
			temp=tmon;
			vazMass=massmon;
			vazMassL=massLmon;
		}
		else{
			FluidoPro=fluijus;
			fluidocol=fluiCjus;
			alf=alfjus;
			bet=betjus;
			pres=pjus;
			temp=tjus;
			vazMass=massjus;
			vazMassL=massLjus;
		}
		double rhop=FluidoPro.MasEspLiq(pres, temp);
		double rhoc=fluidocol.MasEspFlu(pres, temp);
		double rholmix=bet*rhoc+(1.-bet)*rhop;
		double rhog=FluidoPro.MasEspGas(pres, temp);
		double rhomix=alf*rhog+(1.-alf)*rholmix;
		tit=alf*rhog/(rhomix);

		double cpp=FluidoPro.CalorLiq(pres, temp);
		double cpc=fluidocol.CalorLiq(pres, temp);
		double cplmix=bet*cpc+(1.-bet)*cpp;
		double cpg=FluidoPro.CalorGas(pres, temp);
		double cpmix=alf*cpg+(1.-alf)*cplmix;

		double kpp=FluidoPro.CalorLiq(pres, temp);
		double kpc=fluidocol.CalorLiq(pres, temp);
		double kplmix=bet*kpc+(1.-bet)*kpp;
		double kpg=FluidoPro.CalorGas(pres, temp);
		double kpmix=alf*kpg+(1.-alf)*kplmix;

		double usl=vazMassL/(rholmix*secaoTrans.area);
		double usg=(vazMass-vazMassL)/(rhog*secaoTrans.area);
		transfer.Vint=usl+usg;
		transfer.Tint=temp;
		transfer.cpint=cpmix;
		transfer.kint=kpmix;
		transfer.rhoint=rhomix;
		transfer.dt=dt;
		if((*vg1dSP).lixo5<1e-15)transfer.transperm();
		else transfer.transtrans();

		massLiqP=rhop*(1.-alf)*(1.-bet)*lCaixa*secaoTrans.area;
		massLiqC=rhoc*(1.-alf)*bet*lCaixa*secaoTrans.area;
		massGas=rhog*alf*lCaixa*secaoTrans.area;

	}
	else if(abreMon>1e-15){
		tempoResIni=-1.;
		FluidoPro=fluimon;
		fluidocol=fluiCmon;
		alf=alfmon;
		bet=betmon;
		pres=pmon;
		temp=tmon;
		vazMass=massmon;
		vazMassL=massLmon;

		double rhop=FluidoPro.MasEspLiq(pres, temp);
		double rhoc=fluidocol.MasEspFlu(pres, temp);
		double rholmix=bet*rhoc+(1.-bet)*rhop;
		double rhog=FluidoPro.MasEspGas(pres, temp);
		double rhomix=alf*rhog+(1.-alf)*rholmix;
		tit=alf*rhog/(rhomix);

		double cpp=FluidoPro.CalorLiq(pres, temp);
		double cpc=fluidocol.CalorLiq(pres, temp);
		double cplmix=bet*cpc+(1.-bet)*cpp;
		double cpg=FluidoPro.CalorGas(pres, temp);
		double cpmix=alf*cpg+(1.-alf)*cplmix;

		double kpp=FluidoPro.CalorLiq(pres, temp);
		double kpc=fluidocol.CalorLiq(pres, temp);
		double kplmix=bet*kpc+(1.-bet)*kpp;
		double kpg=FluidoPro.CalorGas(pres, temp);
		double kpmix=alf*kpg+(1.-alf)*kplmix;

		double usl=vazMassL/(rholmix*secaoTrans.area);
		double usg=(vazMass-vazMassL)/(rhog*secaoTrans.area);
		transfer.Vint=usl+usg;
		transfer.Tint=temp;
		transfer.cpint=cpmix;
		transfer.kint=kpmix;
		transfer.rhoint=rhomix;
		transfer.dt=dt;
		transfer.transtrans();

		massLiqP=rhop*(1.-alf)*(1.-bet)*lCaixa*secaoTrans.area;
		massLiqC=rhoc*(1.-alf)*bet*lCaixa*secaoTrans.area;
		massGas=rhog*alf*lCaixa*secaoTrans.area;
	}
	else if(abreJus>1e-15){
		tempoResIni=-1.;
		FluidoPro=fluijus;
		fluidocol=fluiCjus;
		alf=alfjus;
		bet=betjus;
		pres=pjus;
		temp=tjus;
		vazMass=massjus;
		vazMassL=massLjus;

		double rhop=FluidoPro.MasEspLiq(pres, temp);
		double rhoc=fluidocol.MasEspFlu(pres, temp);
		double rholmix=bet*rhoc+(1.-bet)*rhop;
		double rhog=FluidoPro.MasEspGas(pres, temp);
		double rhomix=alf*rhog+(1.-alf)*rholmix;
		tit=alf*rhog/(rhomix);

		double cpp=FluidoPro.CalorLiq(pres, temp);
		double cpc=fluidocol.CalorLiq(pres, temp);
		double cplmix=bet*cpc+(1.-bet)*cpp;
		double cpg=FluidoPro.CalorGas(pres, temp);
		double cpmix=alf*cpg+(1.-alf)*cplmix;

		double kpp=FluidoPro.CalorLiq(pres, temp);
		double kpc=fluidocol.CalorLiq(pres, temp);
		double kplmix=bet*kpc+(1.-bet)*kpp;
		double kpg=FluidoPro.CalorGas(pres, temp);
		double kpmix=alf*kpg+(1.-alf)*kplmix;

		double usl=vazMassL/(rholmix*secaoTrans.area);
		double usg=(vazMass-vazMassL)/(rhog*secaoTrans.area);
		transfer.Vint=usl+usg;
		transfer.Tint=temp;
		transfer.cpint=cpmix;
		transfer.kint=kpmix;
		transfer.rhoint=rhomix;
		transfer.dt=dt;
		transfer.transtrans();


		massLiqP=rhop*(1.-alf)*(1.-bet)*lCaixa*secaoTrans.area;
		massLiqC=rhoc*(1.-alf)*bet*lCaixa*secaoTrans.area;
		massGas=rhog*alf*lCaixa*secaoTrans.area;
	}
	else{
		if(tempoResIni==-1)tempoResIni=(*vg1dSP).lixo5;
		vazMass=0.;
		vazMassL=0.;

		double rhop=FluidoPro.MasEspLiq(pres, temp);
		double rhoc=fluidocol.MasEspFlu(pres, temp);
		double rholmix=bet*rhoc+(1.-bet)*rhop;
		double rhog=FluidoPro.MasEspGas(pres, temp);
		double rhomix=alf*rhog+(1.-alf)*rholmix;

		double cpp=FluidoPro.CalorLiq(pres, temp);
		double cpc=fluidocol.CalorLiq(pres, temp);
		double cplmix=bet*cpc+(1.-bet)*cpp;
		double cpg=FluidoPro.CalorGas(pres, temp);
		double cpmix=alf*cpg+(1.-alf)*cplmix;

		double kpp=FluidoPro.CalorLiq(pres, temp);
		double kpc=fluidocol.CalorLiq(pres, temp);
		double kplmix=bet*kpc+(1.-bet)*kpp;
		double kpg=FluidoPro.CalorGas(pres, temp);
		double kpmix=alf*kpg+(1.-alf)*kplmix;

		double usl=vazMassL/(rholmix*secaoTrans.area);
		double usg=(vazMass-vazMassL)/(rhog*secaoTrans.area);
		transfer.Vint=usl+usg;
		transfer.Tint=temp;
		transfer.cpint=cpmix;
		transfer.kint=kpmix;
		transfer.rhoint=rhomix;
		transfer.dt=dt;
		double qw;
		qw=transfer.transtrans();
		double FW0;
		double bo0;
		double ba0;
		double rs0;
		double termoRS0;
		double rhop0;
		double rhoc0;
		double rhog0=rhog;
		propDeri(pres, temp, FW0, bo0, rs0, termoRS0, rhop0, rhoc0);
		double rholMix0=(1.-bet)*rhop0+bet*rhoc0;
		double drhogdT=FluidoPro.drhodt(pres, temp);
		double drhogdp;
        double compres = FluidoPro.Zdran(pres, temp);
        double dzdp = FluidoPro.DZDP(pres, temp);
        double dzdt= FluidoPro.DZDT(pres, temp);
        drhogdp = rhog * (1 / (pres * 98066.5) - (1 / compres) * dzdp);

        double cvgMod=FluidoPro.CalorGasVolMod2(pres, temp,rhog);
        double cvl=(1. - bet) * FluidoPro.CalorLiq(pres, temp) +
        		bet * fluidocol.CalorLiq(pres, temp);
        double latente = (FluidoPro.EntalpGas(pres, temp) -FluidoPro.EntalpLiq(pres, temp));

		double coefalf=-1.+termoRS0*(1-bet)/rhop0;

		double pres1;
		double alf1=alf;
		double bet1=bet;
		double area=secaoTrans.area;
		double multdttemp=(alf*rhog*cvgMod+(1.-alf)*rholMix0*cvl);
		double multdptemp=(alf*rhog*cvgMod+(1.-alf)*rholMix0*cvl);
		double temp1=temp+dt*qw/(multdttemp*area);
		double pre1=pres;


		double FW1;
		double bo1;
		double ba1;
		double rs1;
		double termoRS1;
		double rhop1;
		double rhoc1;
		double rholMix1;
		double rhog1=rhog;

		double dalfdt;
		double dbetdt;
		double drhopdt;
		double drhocdt;
		double drholMixdt;
		double dpdt;
		double dTdt;
		double dtermoRSdt;

		propDeri(pres1, temp1, FW1, bo1, rs1, termoRS1, rhop1, rhoc1);
		dTdt=(temp1-temp)/dt;
		double coefPresTempo = -FluidoPro.CalorGasPresMod(pres, temp) * (rhog0 * alf);
		rholMix1=(1.-bet)*rhop1+bet*rhoc1;

		double erro=1000.;
		int iter=0;
		double pres0;
		double temp0;
		pres1=pres;
		temp1=temp;
		double DrhocVP=(fluidocol.MasEspFlu(pres*1.001, temp)-rhoc)/(pres*0.001);
		double DrhocVT=(fluidocol.MasEspFlu(pres, temp*1.001)-rhoc)/(temp*0.001);
		double DrhopVP=(FluidoPro.MasEspLiq(pres*1.001, temp)-rhoc)/(pres*0.001);
		double DrhopVT=(FluidoPro.MasEspLiq(pres, temp*1.001)-rhoc)/(temp*0.001);

		double fonteP=0.;
		double fonteC=0.;
		double fonteG=0.;
		if(nfonte>0){
			atualizaFontesCaixa(fonteP, fonteMP);
			atualizaFontesCaixa(fonteC, fonteMC);
			atualizaFontesCaixa(fonteG, fonteMG);
		}
		double razcpF = FluidoPro.ConstAdG(pres, temp);

		double multP=1.;
		if(fonteP<0.)multP=0.;
		double multG=1.;
		if(fonteG<0.)multG=0.;
		double multC=1.;
		if(fonteC<0.)multC=0.;

		while( erro>1e-5 && iter<100){
			drhopdt=(rhop1-rhop0)/dt;
			drhocdt=(rhoc1-rhoc0)/dt;
			dtermoRSdt=(termoRS1-termoRS0)/dt;
			dalfdt=((1.-alf)*(1.-bet)*drhopdt/rhop0+(1.-alf)*bet*drhocdt/rhoc0-termoRS0*((1.-alf)*bet/rhoc0)*drhocdt-
					((1.-alf)*(1.-bet)/rhop0)*dtermoRSdt-(1.-termoRS0/rhop0)*fonteC/(rhoc0*area*lCaixa)-fonteP/(rhop0*area*lCaixa))/coefalf;
			alf1=alf+dalfdt*dt;
			if(alf1>1.)alf1=1.;
			else if(alf1<0.)alf1=0.;

			dbetdt=-(bet/rhoc0)*drhocdt+bet*dalfdt/(1.-alf)+fonteC/((1.-alf)*rhoc0*area*lCaixa);
			bet1=bet+dbetdt*dt;
			if(bet1>1.)bet1=1.;
			else if(bet1<0.)bet1=0.;
			double coefdp=(alf/rhog0)*drhogdp+((1.-alf)*(bet)/rhoc0)*DrhocVP;
			if(alf<1e-15){
				coefdp+=((1.-alf)*(1.-bet)/rhop0)*DrhopVP;
				dpdt=(-((1.-alf)*(1.-bet)/rhop0)*DrhopVT*dTdt-((1.-alf)*(bet)/rhoc0)*DrhocVT*dTdt-((alf/rhog0)*drhogdT)*dTdt-
						(1/rhog0-1/rhop0)*((1-alf)*(1-bet)*dtermoRSdt+termoRS0*((1.-alf1)*(1.-bet1)-(1.-alf)*(1.-bet))/dt)+
						fonteC/(rhoc0*area*lCaixa)+fonteP/(rhop0*area*lCaixa)+fonteG/(rhog0*area*lCaixa))/coefdp;
			}
			else dpdt=(-((1.-alf)*(1.-bet)/rhop0)*drhopdt-((1.-alf)*(bet)/rhoc0)*DrhocVT*dTdt-((alf/rhog0)*drhogdT)*dTdt-
					(1/rhog0-1/rhop0)*((1-alf)*(1-bet)*dtermoRSdt+termoRS0*((1.-alf1)*(1.-bet1)-(1.-alf)*(1.-bet))/dt)+
					fonteC/(rhoc0*area*lCaixa)+fonteP/(rhop0*area*lCaixa)+fonteG/(rhog0*area*lCaixa))/coefdp;
			pres0=pres1;
			pres1=pres+dpdt*dt/98066.5;

			double transmass=(termoRS1*(1.-bet)*dalfdt-(1.-alf)*(1.-bet)*dtermoRSdt-termoRS0*(1.-alf)*(bet/rhoc0)*drhocdt+
					termoRS0*fonteC/(rhoc0*area*lCaixa));
			dTdt=(coefPresTempo*dpdt-(pres * 98066.5*(1.-alf)/rholMix0)*(rholMix1-rholMix0)/dt-latente*transmass-
					  (rhoc0 - rhop0) * (1 - alf) * pres * 98066.5 * dbetdt / rholmix+qw/area+
					  multP*cpp*(temperaturaFonte-temp)*fonteP/(area*lCaixa)+multG*(cpg/razcpF)*(temperaturaFonte-temp)*fonteG/(area*lCaixa)+
					  multC*cpc*(temperaturaFonte-temp)*fonteC/(area*lCaixa))/multdttemp;

			temp0=temp1;
			temp1=temp+dt*dTdt;
			propDeri(pres1, temp1, FW1, bo1, rs1, termoRS1, rhop1, rhoc1);
			rholMix1=(1.-bet1)*rhop1+bet1*rhoc1;
			rhog1=FluidoPro.MasEspGas(pres1, temp1);
			erro=fabs(pres1-pres0)+fabs(temp1-temp0);
			iter++;
		}

		alf=alf1;
		bet=bet1;
		pres=pres1;
		temp=temp1;

		massLiqP=rhop1*(1.-alf)*(1.-bet)*lCaixa*area;
		massLiqC=rhoc1*(1.-alf)*bet*lCaixa*area;
		massGas=rhog1*alf*lCaixa*area;

	}

}

