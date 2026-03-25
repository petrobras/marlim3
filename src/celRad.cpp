/*
 * celRad.cpp
 *
 *  Created on: 14 de out. de 2023
 *      Author: Eduardo
 */
/*#include <math.h>

#include <algorithm>
#include <fstream>
using namespace std;
#include <stdlib.h>
#include "Vetor.h"
#include "Matriz.h"*/
#include "celRad.h"

celrad::celrad():
		localvet(4),localmat(4,11){
	vg1dSP=0;
	fluc=ProFluCol();
	flup=ProFlu();
	drcamada=0;
	PcamadaR=0;
	Pcamada=0;
	PcamadaL=0;
	QcamadaR=0;
	QcamadaL=0;

	QwcamadaR=0;
	QwcamadaL=0;
	QocamadaR=0;
	QocamadaL=0;
	QgcamadaR=0;
	QgcamadaL=0;
	Qwini=0;
	Qoini=0;
	Qgini=0;

	volCel=0;
	cflCel=0;

	Piter=0;
	Pint=0;
	presRes=0;
	presRes2=0;
	Pini=0;
	QiniR=0;
	QiniL=0;
	dt=0;
	zD1=0;
	zD=0;
	zD0=0;
	r1=0;
	r0=0;
	rm=0;
	tRes=0;
	kXL=0;
	kXR=0;
	kYL=0;
	kYR=0;
	poro=0;
	compresPoro=0;
	icel=0;

    dtSL=0;
    dtSW=0;
	dh=0;

	condiTparede=0;
	ncel=0;

	sLR=0;
	sL=0;
	sLL=0;
	sLini=0;
	sLiter=0;
	sWR=0;
	sW=0;
	sWL=0;
	sWini=0;
	sWiter=0;
	sLRes=0;
	sWRes=0;
	sWPoc=0;

	pmed1=0;
	slmed1=0;
	samed1=0;
	pmed0=0;
	slmed0=0;
	samed0=0;

	vbo=0;
	vboIni=0;
	rhostd=0;
	rhogstd=0;
	rhoa=0;

	rhoP1=0;
	rhoP=0;
	rhoPIni=0;
	rhoP0=0;

	rhogP1=0;
	rhogP=0;
	rhogPIni=0;
	rhogP0=0;

	rhog1=0;
	rhog0=0;

	rho1=0;
	rho0=0;

	rhoa1=0;
	rhoa0=0;

	rhoPa1=0;
	rhoPa=0;
	rhoPaIni=0;
	rhoPa0=0;

	mio1=0;
	mio0=0;
	mig1=0;
	mig0=0;
	mia1=0;
	mia0=0;

	kmed1=0;
	kmed0=0;
	kmedG1=0;
	kmedG0=0;
	kmedA1=0;
	kmedA0=0;
	pcOG1=0;
	pcOGm=0;
	pcOG0=0;
	pcAO1=0;
	pcAOm=0;
	pcAO0=0;

	kabsol1=0;
	kabsol0=0;

	darcyO1=0;
	darcyO0=0;
	darcyG1=0;
	darcyG0=0;
	darcyA1=0;
	darcyA0=0;

	rQcamadaL=0;
	rQcamadaR=0;
	drQ=0;
	drP1=0;
	drP0=0;
	dPdT=0;

	kRelOACel=tabelaPemRelOA();
	kRelOGCel=tabelaPemRelOG();
	pcOACel=tabelaPresCapOA();
	pcGOCel=tabelaPresCapGO();

	satConata=0;
	ip=0;

	reiniciaSL=0;
	reiniciaSW=0;

	alf=0;
	alf1=0;
	alf0=0;
	alfmed1=0;
	alfmed0=0;

	pBolha=1000.;
    BSW=0;
	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;
}


celrad::celrad(int vncel,int vicel,double vr0, double vr1, double vrm,double vkXL,double vkXR,
		double vkYL,double vkYR,double vporo,double vcompresPoro,double vPcamada,double vPint,
		double vpresRes,double vQL, double vQR, double vtRes, double vdt, double vzD0, double vzD,
		double vzD1,double vdh,double vsL,double vsW,double vsLRes,double vsWRes,double vsWPoc,
		double vsatConata,const double vip,tabelaPemRelOA vkRelOA,tabelaPemRelOG vkRelOG,tabelaPresCapOA vpcOA,tabelaPresCapGO vpcGO,
		ProFluCol vfluc, ProFlu vflup,celrad* vcelL, celrad* vcelR,varGlob1D* Vvg1dSP):
		localvet(4),localmat(4,11){

	vg1dSP=Vvg1dSP;

	ncel=vncel;
	icel=vicel;

    condiTparede=0;

    zD1=vzD1;
    zD=vzD;
    zD0=vzD0;
    r0=vr0;
    r1=vr1;
    rm=vrm;
    drcamada=r1-r0;

    kXL=vkXL;
    kXR=vkXR;
    kYL=vkYL;
    kYR=vkYR;
    poro=vporo;
    compresPoro=vcompresPoro;


    PcamadaR=vPcamada;
    Pcamada=vPcamada;
    PcamadaL=vPcamada;
    Pini=Pcamada;
    Piter=vPcamada;
    Pint=vPint;
    presRes=vpresRes;
    presRes2=vpresRes;
    QcamadaL=vQL;
    QcamadaR=vQR;
    QiniL=vQL;
    QiniR=vQR;
    tRes=vtRes;
    dt=vdt;
    dh=vdh;
    dPdT=0.;

	fluc=vfluc;
	flup=vflup;

	//celL=vcelL;
	//celR=vcelR;

	condiTparede=0;
	dtSL=dt;
	dtSW=dt;

	sLR=vsL;
	sL=vsL;
	sLL=vsL;
	sLini=vsL;
	sLiter=vsL;
	sWR=vsW;
	sW=vsW;
	sWL=vsW;
	sWini=vsW;
	sWiter=vsW;
	sLRes=vsLRes;
	sWRes=vsWRes;
	sWPoc=vsWPoc;

	kRelOACel=vkRelOA;
	kRelOGCel=vkRelOG;
	pcOACel=vpcOA;
	pcGOCel=vpcGO;

	satConata=vsatConata;
	ip=vip;

	QwcamadaR=0;
	QwcamadaL=0;
	QocamadaR=0;
	QocamadaL=0;
	QgcamadaR=0;
	QgcamadaL=0;
	Qwini=0;
	Qoini=0;
	Qgini=0;

	double rq1=0.5*(r1+rm);
	double rq0=0.5*(rm+r0);
	volCel=M_PI*(rq1*rq1-rq0*rq0);
	cflCel=1.;

	rQcamadaL=0.5*(rm+r0);
	rQcamadaR=0.5*(r1+rm);
	drQ=rQcamadaR-rQcamadaL;
	drP1=r1-rm;
	drP0=rm-r0;

	pmed1=0.5*(PcamadaR+Pcamada);
	slmed1=0.5*(sLR+sL);
	samed1=0.5*(sWR+sW);
	pmed0=0.5*(Pcamada+PcamadaL);
	slmed0=0.5*(sL+sLL);
	samed0=0.5*(sW+sWL);

	vbo=flup.BOFunc(Pcamada, tRes);
	vboIni=vbo;
	rhostd=1000.*141.5/(131.5+flup.API);
	rhogstd=flup.MasEspGas(1.033, 20.);
	rhoa=flup.MasEspAgua(Pcamada, tRes);

	rhoP1=flup.MasEspoleo(PcamadaR, tRes);
	rhoP=flup.MasEspoleo(Pcamada, tRes);
	rhoPIni=rhoP;
	rhoP0=flup.MasEspoleo(PcamadaL, tRes);

	rhogP1=flup.MasEspGas(PcamadaR, tRes);
	rhogP=flup.MasEspGas(Pcamada, tRes);
	rhogPIni=rhogP;
	rhogP0=flup.MasEspGas(PcamadaL, tRes);

	rhog1=flup.MasEspGas(pmed1, tRes);
	rhog0=flup.MasEspGas(pmed0, tRes);

	rho1=flup.MasEspoleo(pmed1, tRes);
	rho0=flup.MasEspoleo(pmed0, tRes);

	rhoa1=flup.MasEspAgua(pmed1, tRes);
	rhoa0=flup.MasEspAgua(pmed0, tRes);

	rhoPa1=flup.MasEspAgua(PcamadaR, tRes);
	rhoPa=rhoa;
	rhoPaIni=rhoPa;
	rhoPa0=flup.MasEspAgua(PcamadaL, tRes);

	mio1=flup.ViscOleo(pmed1, tRes)/1000.;
	mio0=flup.ViscOleo(pmed0, tRes)/1000.;
	mig1=flup.ViscGas(pmed1, tRes)/1000.;
	mig0=flup.ViscGas(pmed0, tRes)/1000.;
	mia1=flup.VisAgua(tRes)/1000.;
	mia0=flup.VisAgua(tRes)/1000.;

	double tit=flup.FracMassHidra(Pcamada, tRes);
	alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
	tit=flup.FracMassHidra(PcamadaR, tRes);
	alf1=0*tit+1*(tit/rhogP1)/((tit/rhogP1)+((1.-tit)/rhoP1));
	tit=flup.FracMassHidra(PcamadaL, tRes);
	alf0=0*tit+1*(tit/rhogP0)/((tit/rhogP0)+((1.-tit)/rhoP0));
	tit=flup.FracMassHidra(pmed1, tRes);
	alfmed1=0*tit+1*(tit/rhog1)/((tit/rhog1)+((1.-tit)/rho1));
	tit=flup.FracMassHidra(pmed1, tRes);
	alfmed0=0*tit+1*(tit/rhog0)/((tit/rhog0)+((1.-tit)/rho0));

	if(kRelOGCel.npont>0 && kRelOACel.npont>0 && pcGOCel.npont>0 && pcOACel.npont>0){
		kmed1=fkO(samed1, 1.-slmed1);
		kmed0=fkO(samed0, 1.-slmed0);
		kmedG1=interpolaTabela(kRelOGCel.npont, 1.-slmed1, kRelOGCel.satG, kRelOGCel.permRelG);
		kmedG0=interpolaTabela(kRelOGCel.npont, 1.-slmed0, kRelOGCel.satG, kRelOGCel.permRelG);
		kmedA1=interpolaTabela(kRelOACel.npont, samed1, kRelOACel.satW, kRelOACel.permRelW);
		kmedA0=interpolaTabela(kRelOACel.npont, samed0, kRelOACel.satW, kRelOACel.permRelW);
		pcOG1=interpolaTabela(pcGOCel.npont, 1.-sLR, pcGOCel.satG, pcGOCel.presCapGO);
		pcOGm=interpolaTabela(pcGOCel.npont, 1.-sL, pcGOCel.satG, pcGOCel.presCapGO);
		pcOG0=interpolaTabela(pcGOCel.npont, 1.-sLL, pcGOCel.satG, pcGOCel.presCapGO);
		pcAO1=interpolaTabela(pcOACel.npont, sWR, pcOACel.satW, pcOACel.presCapOW);
		pcAOm=interpolaTabela(pcOACel.npont, sW, pcOACel.satW, pcOACel.presCapOW);
		pcAO0=interpolaTabela(pcOACel.npont, sWL, pcOACel.satW, pcOACel.presCapOW);
	}
	else{
		kmed1=0;
		kmed0=0;
		kmedG1=0;
		kmedG0=0;
		kmedA1=0;
		kmedA0=0;
		pcOG1=0;
		pcOGm=0;
		pcOG0=0;
		pcAO1=0;
		pcAOm=0;
		pcAO0=0;
	}

	kabsol1=sqrtl(kXR*kYR);
	kabsol0=sqrtl(kXL*kYL);

	darcyO1=kmed1*kabsol1*98066.22/((1.-alfmed1)*mio1+alfmed1*mig1);
	darcyO0=kmed0*kabsol0*98066.22/((1.-alfmed0)*mio0+alfmed0*mig0);
	darcyG1=kmedG1*kabsol1*98066.22/mig1;
	darcyG0=kmedG0*kabsol0*98066.22/mig0;
	darcyA1=kmedA1*kabsol1*98066.22/mia1;
	darcyA0=kmedA0*kabsol0*98066.22/mia0;

	reiniciaSL=0;
	reiniciaSW=0;

	pBolha=1000.;
    BSW=0;
	tipoModelOleo=1;
	compOleo=1e-5;
	rhoRef=900;

}

celrad::celrad(const celrad& vcel):
		localvet(4),localmat(4,11){

	vg1dSP=vcel.vg1dSP;
	ncel=vcel.ncel;
	icel=vcel.icel;

    condiTparede=0;

    zD1=vcel.zD1;
    zD=vcel.zD;
    zD0=vcel.zD0;
    r0=vcel.r0;
    r1=vcel.r1;
    rm=vcel.rm;
    drcamada=r1-r0;

    kXL=vcel.kXL;
    kXR=vcel.kXR;
    kYL=vcel.kYL;
    kYR=vcel.kYR;
    poro=vcel.poro;
    compresPoro=vcel.compresPoro;

    PcamadaR=vcel.PcamadaR;
    Pcamada=vcel.Pcamada;
    PcamadaL=vcel.PcamadaL;
    Pini=vcel.Pini;
    Piter=vcel.Piter;
    Pint=vcel.Pint;
    presRes=vcel.presRes;
    presRes2=vcel.presRes2;
    QcamadaL=vcel.QcamadaL;
    QcamadaR=vcel.QcamadaR;
    QiniL=vcel.QiniL;
    QiniR=vcel.QiniR;
    tRes=vcel.tRes;
    dt=vcel.dt;
    dh=vcel.dh;
    dPdT=vcel.dPdT;

	fluc=vcel.fluc;
	flup=vcel.flup;

	//celL=vcel.celL;
	//celR=vcel.celR;

	condiTparede=0;
	dtSL=vcel.dtSL;
	dtSW=vcel.dtSW;

	sLR=vcel.sL;
	sL=vcel.sL;
	sLL=vcel.sL;
	sLini=vcel.sL;
	sLiter=vcel.sL;
	sWR=vcel.sW;
	sW=vcel.sW;
	sWL=vcel.sW;
	sWini=vcel.sW;
	sWiter=vcel.sW;
	sLRes=vcel.sLRes;
	sWRes=vcel.sWRes;
	sWPoc=vcel.sWPoc;

	kRelOACel=vcel.kRelOACel;
	kRelOGCel=vcel.kRelOGCel;
	pcOACel=vcel.pcOACel;
	pcGOCel=vcel.pcGOCel;

	satConata=vcel.satConata;
	ip=vcel.ip;

	QwcamadaR=vcel.QwcamadaR;
	QwcamadaL=vcel.QwcamadaL;
	QocamadaR=vcel.QocamadaR;
	QocamadaL=vcel.QocamadaL;
	QgcamadaR=vcel.QgcamadaR;
	QgcamadaL=vcel.QgcamadaL;
	Qwini=vcel.Qwini;
	Qoini=vcel.Qoini;
	Qgini=vcel.Qgini;

	volCel=vcel.volCel;
	cflCel=vcel.cflCel;

	rQcamadaL=vcel.rQcamadaL;
	rQcamadaR=vcel.rQcamadaR;
	drQ=vcel.drQ;
	drP1=vcel.drP1;
	drP0=vcel.drP0;

	pmed1=vcel.pmed1;
	slmed1=vcel.slmed1;
	samed1=vcel.samed1;
	pmed0=vcel.pmed0;
	slmed0=vcel.slmed0;
	samed0=vcel.samed0;

	vbo=vcel.vbo;
	vboIni=vcel.vboIni;
	rhostd=vcel.rhostd;
	rhogstd=vcel.rhogstd;
	rhoa=vcel.rhoa;

	rhoP1=vcel.rhoP1;
	rhoP=vcel.rhoP;
	rhoPIni=vcel.rhoPIni;
	rhoP0=vcel.rhoP0;

	rhogP1=vcel.rhogP1;
	rhogP=vcel.rhogP;
	rhogPIni=vcel.rhogPIni;
	rhogP0=vcel.rhogP0;

	rhog1=vcel.rhog1;
	rhog0=vcel.rhog0;

	rho1=vcel.rho1;
	rho0=vcel.rho0;

	rhoa1=vcel.rhoa1;
	rhoa0=vcel.rhoa0;

	rhoPa1=vcel.rhoPa1;
	rhoPa=rhoa;
	rhoPaIni=vcel.rhoPaIni;
	rhoPa0=vcel.rhoPa0;

	mio1=vcel.mio1;
	mio0=vcel.mio0;
	mig1=vcel.mig1;
	mig0=vcel.mig0;
	mia1=vcel.mia1;
	mia0=vcel.mia0;

	kmed1=vcel.kmed1;
	kmed0=vcel.kmed0;
	kmedG1=vcel.kmedG1;
	kmedG0=vcel.kmedG0;
	kmedA1=vcel.kmedA1;
	kmedA0=vcel.kmedA0;
	pcOG1=vcel.pcOG1;
	pcOGm=vcel.pcOGm;
	pcOG0=vcel.pcOG0;
	pcAO1=vcel.pcAO1;
	pcAOm=vcel.pcAOm;
	pcAO0=vcel.pcAO0;

	kabsol1=vcel.kabsol1;
	kabsol0=vcel.kabsol0;

	darcyO1=vcel.darcyO1;
	darcyO0=vcel.darcyO0;
	darcyG1=vcel.darcyG1;
	darcyG0=vcel.darcyG0;
	darcyA1=vcel.darcyA1;
	darcyA0=vcel.darcyA0;

	reiniciaSL=vcel.reiniciaSL;
	reiniciaSW=vcel.reiniciaSW;

	alf=vcel.alf;
	alf1=vcel.alf1;
	alf0=vcel.alf0;
	alfmed1=vcel.alfmed1;
	alfmed0=vcel.alfmed0;

	pBolha=vcel.pBolha;
    BSW=vcel.BSW;
	tipoModelOleo=vcel.tipoModelOleo;
	compOleo=vcel.compOleo;
	rhoRef=vcel.rhoRef;

}

celrad& celrad::operator =(const celrad& vcel){
	if (this != &vcel){

		vg1dSP=vcel.vg1dSP;
		ncel=vcel.ncel;
		icel=vcel.icel;

	    condiTparede=0;

	    zD1=vcel.zD1;
	    zD=vcel.zD;
	    zD0=vcel.zD0;
	    r0=vcel.r0;
	    r1=vcel.r1;
	    rm=vcel.rm;
	    drcamada=r1-r0;

	    kXL=vcel.kXL;
	    kXR=vcel.kXR;
	    kYL=vcel.kYL;
	    kYR=vcel.kYR;
	    poro=vcel.poro;
	    compresPoro=vcel.compresPoro;


	    PcamadaR=vcel.PcamadaR;
	    Pcamada=vcel.Pcamada;
	    PcamadaL=vcel.PcamadaL;
	    Pini=vcel.Pini;
	    Piter=vcel.Piter;
	    Pint=vcel.Pint;
	    presRes=vcel.presRes;
	    presRes2=vcel.presRes2;
	    QcamadaL=vcel.QcamadaL;
	    QcamadaR=vcel.QcamadaR;
	    QiniL=vcel.QiniL;
	    QiniR=vcel.QiniR;
	    tRes=vcel.tRes;
	    dt=vcel.dt;
	    dh=vcel.dh;
	    dPdT=vcel.dPdT;

		fluc=vcel.fluc;
		flup=vcel.flup;

		//celL=vcel.celL;
		//celR=vcel.celR;

		condiTparede=0;
		dtSL=vcel.dtSL;
		dtSW=vcel.dtSW;

		sLR=vcel.sL;
		sL=vcel.sL;
		sLL=vcel.sL;
		sLini=vcel.sL;
		sLiter=vcel.sL;
		sWR=vcel.sW;
		sW=vcel.sW;
		sWL=vcel.sW;
		sWini=vcel.sW;
		sWiter=vcel.sW;
		sLRes=vcel.sLRes;
		sWRes=vcel.sWRes;
		sWPoc=vcel.sWPoc;

		kRelOACel=vcel.kRelOACel;
		kRelOGCel=vcel.kRelOGCel;
		pcOACel=vcel.pcOACel;
		pcGOCel=vcel.pcGOCel;

		satConata=vcel.satConata;
		ip=vcel.ip;

		QwcamadaR=vcel.QwcamadaR;
		QwcamadaL=vcel.QwcamadaL;
		QocamadaR=vcel.QocamadaR;
		QocamadaL=vcel.QocamadaL;
		QgcamadaR=vcel.QgcamadaR;
		QgcamadaL=vcel.QgcamadaL;
		Qwini=vcel.Qwini;
		Qoini=vcel.Qoini;
		Qgini=vcel.Qgini;

		volCel=vcel.volCel;
		cflCel=vcel.cflCel;

		rQcamadaL=vcel.rQcamadaL;
		rQcamadaR=vcel.rQcamadaR;
		drQ=vcel.drQ;
		drP1=vcel.drP1;
		drP0=vcel.drP0;

		pmed1=vcel.pmed1;
		slmed1=vcel.slmed1;
		samed1=vcel.samed1;
		pmed0=vcel.pmed0;
		slmed0=vcel.slmed0;
		samed0=vcel.samed0;

		vbo=vcel.vbo;
		vboIni=vcel.vboIni;
		rhostd=vcel.rhostd;
		rhogstd=vcel.rhogstd;
		rhoa=vcel.rhoa;

		rhoP1=vcel.rhoP1;
		rhoP=vcel.rhoP;
		rhoPIni=vcel.rhoPIni;
		rhoP0=vcel.rhoP0;

		rhogP1=vcel.rhogP1;
		rhogP=vcel.rhogP;
		rhogPIni=vcel.rhogPIni;
		rhogP0=vcel.rhogP0;

		rhog1=vcel.rhog1;
		rhog0=vcel.rhog0;

		rho1=vcel.rho1;
		rho0=vcel.rho0;

		rhoa1=vcel.rhoa1;
		rhoa0=vcel.rhoa0;

		rhoPa1=vcel.rhoPa1;
		rhoPa=rhoa;
		rhoPaIni=vcel.rhoPaIni;
		rhoPa0=vcel.rhoPa0;

		mio1=vcel.mio1;
		mio0=vcel.mio0;
		mig1=vcel.mig1;
		mig0=vcel.mig0;
		mia1=vcel.mia1;
		mia0=vcel.mia0;

		kmed1=vcel.kmed1;
		kmed0=vcel.kmed0;
		kmedG1=vcel.kmedG1;
		kmedG0=vcel.kmedG0;
		kmedA1=vcel.kmedA1;
		kmedA0=vcel.kmedA0;
		pcOG1=vcel.pcOG1;
		pcOGm=vcel.pcOGm;
		pcOG0=vcel.pcOG0;
		pcAO1=vcel.pcAO1;
		pcAOm=vcel.pcAOm;
		pcAO0=vcel.pcAO0;

		kabsol1=vcel.kabsol1;
		kabsol0=vcel.kabsol0;

		darcyO1=vcel.darcyO1;
		darcyO0=vcel.darcyO0;
		darcyG1=vcel.darcyG1;
		darcyG0=vcel.darcyG0;
		darcyA1=vcel.darcyA1;
		darcyA0=vcel.darcyA0;

		reiniciaSL=vcel.reiniciaSL;
		reiniciaSW=vcel.reiniciaSW;

		alf=vcel.alf;
		alf1=vcel.alf1;
		alf0=vcel.alf0;
		alfmed1=vcel.alfmed1;
		alfmed0=vcel.alfmed0;

		pBolha=vcel.pBolha;
	    BSW=vcel.BSW;
		tipoModelOleo=vcel.tipoModelOleo;
		compOleo=vcel.compOleo;
		rhoRef=vcel.rhoRef;
	}
	return *this;
}

double celrad::interpolaTabela(int nserie, double valx, double* x, double* y){
	double val;
	int ind=0;
	int min=1;
	double raz;
	for (int i = 0; i <= nserie - 1; i++) {
		if (i < nserie - 1) {
			if(valx<x[0]){
				ind=0;
				min=-1;
				break;
			}
			if (valx >= x[i] && valx < x[i + 1]) {
				ind = i;
				raz = 1
						- (valx - x[i])
								/ (x[i + 1] - x[i]);
				break;
			}
			//else if(i==parserie-2){
			//ind=i+1;
			//raz=1;
			//}
		} else if (i == nserie - 1) {
			ind = i;
			raz = 1;
		}
	}

	  double inf = y[ind];
	  double sup;
	  if (ind < nserie - 1 && min>-1){
		  sup = y[ind + 1];
		  val = (inf * raz + (1 - raz) * sup);
	  }
	  else{
		  val = y[ind];
	  }

	return val;
}

double celrad::fkO(double satW, double satG){
	/*double krw=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelW);
	double krow=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC=interpolaTabela(kRelOACel.npont, satConata, kRelOACel.satW, kRelOACel.permRelOW);

	double val=krowC*(((krow/krowC)+krw)-(krw));*/

	/*double krw0=interpolaTabela(kRelOACel.npont, 0.49, kRelOACel.satW, kRelOACel.permRelW);
	double krow0=interpolaTabela(kRelOACel.npont, 0.49, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC0=interpolaTabela(kRelOACel.npont, 0.13, kRelOACel.satW, kRelOACel.permRelOW);
	double krg0=interpolaTabela(kRelOGCel.npont, 0.195, kRelOGCel.satG, kRelOGCel.permRelG);
	double krog0=interpolaTabela(kRelOGCel.npont, 0.195, kRelOGCel.satG, kRelOGCel.permRelOG);

	double val0=krowC0*(((krow0/krowC0)+krw0)*((krog0/krowC0)+krg0)-(krw0+krg0));*/

	double krw=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelW);
	double krow=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC=interpolaTabela(kRelOACel.npont, satConata, kRelOACel.satW, kRelOACel.permRelOW);
	double krg=interpolaTabela(kRelOGCel.npont, satG, kRelOGCel.satG, kRelOGCel.permRelG);
	double krog=interpolaTabela(kRelOGCel.npont, satG, kRelOGCel.satG, kRelOGCel.permRelOG);

	double val=krowC*(((krow/krowC)+krw)*((krog/krowC)+krg)-(krw+krg));
	if(val<0)val=0.;

	return val;
}

double celrad::dFW(double satW, double satG){
	double krw=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelW);
	double krow=interpolaTabela(kRelOACel.npont, satW, kRelOACel.satW, kRelOACel.permRelOW);
	double krowC=interpolaTabela(kRelOACel.npont, satConata, kRelOACel.satW, kRelOACel.permRelOW);
	double krg=interpolaTabela(kRelOGCel.npont, satG, kRelOGCel.satG, kRelOGCel.permRelG);
	double krog=interpolaTabela(kRelOGCel.npont, satG, kRelOGCel.satG, kRelOGCel.permRelOG);
	double ko=krowC*(((krow/krowC)+krw)*(1.+0*(krog/krowC)+0*krg)-(krw+0*krg));

	double fracO=ko/(ko+krw+krg);
	double fracA=krw/(ko+krw+krg);
	double fracG=0*krg/(ko+krow+krg);

	double satwD=satW*1.001;
	if(satwD>1)satwD=satW*0.999;
	double dsatW=satwD-satW;
	krw=interpolaTabela(kRelOACel.npont, satwD, kRelOACel.satW, kRelOACel.permRelW);
	krow=interpolaTabela(kRelOACel.npont, satwD, kRelOACel.satW, kRelOACel.permRelOW);
	ko=krowC*(((krow/krowC)+krw)*((krog/krowC)+krg)-(krw+krg));

	double fracOD=ko/(ko+krw+krg);
	double fracAD=krw/(ko+krw+krg);
	double fracGD=krg/(ko+krow+krg);

	double val=(fracAD-fracA)/dsatW;



	return val;
}

void celrad::calcVazOWG(){

	pmed1=0.5*(PcamadaR+Pcamada);
	slmed1=sLR;//0.5*(sLR+sL);
	if((QwcamadaR+QocamadaR)<0)slmed1=sL;
	samed1=sWR;//0.5*(sWR+sW);
	//if(dFW(sWR ,1.-slmed1)*(QwcamadaR)<0)samed1=sW;
	if((QwcamadaR+QocamadaR)<0)samed1=sW;
	pmed0=0.5*(Pcamada+PcamadaL);
	slmed0=sL;//0.5*(sL+sLL);
	if((QwcamadaL+QocamadaL)<0)slmed0=sLL;
	samed0=sW;//0.5*(sW+sWL);
	//if((dFW(sW ,1.-slmed0)*QwcamadaL)<0)samed0=sWL;
	if((QwcamadaL+QocamadaL)<0)samed0=sWL;

	vbo=flup.BOFunc(Pcamada, tRes);
	rhoa=flup.MasEspAgua(Pcamada, tRes);

	if(tipoModelOleo==1){
		rhoP1=flup.MasEspoleo(PcamadaR, tRes);
		rhoP=flup.MasEspoleo(Pcamada, tRes);
		rhoP0=flup.MasEspoleo(PcamadaL, tRes);
	}
	else{
		rhoP1=rhoRef*(1-compOleo*(presRes-PcamadaR));
		rhoP=rhoRef*(1-compOleo*(presRes-Pcamada));
		rhoP0=rhoRef*(1-compOleo*(presRes-PcamadaL));
	}

	rhogP1=flup.MasEspGas(PcamadaR, tRes);
	rhogP=flup.MasEspGas(Pcamada, tRes);
	rhogP0=flup.MasEspGas(PcamadaL, tRes);

	rhog1=flup.MasEspGas(pmed1, tRes);
	rhog0=flup.MasEspGas(pmed0, tRes);

	if(tipoModelOleo==1){
		rho1=flup.MasEspoleo(pmed1, tRes);
		rho0=flup.MasEspoleo(pmed0, tRes);
	}
	else{
		rho1=rhoRef*(1-compOleo*(presRes-pmed1));
		rho0=rhoRef*(1-compOleo*(presRes-pmed0));
	}

	rhoa1=flup.MasEspAgua(pmed1, tRes);
	rhoa0=flup.MasEspAgua(pmed0, tRes);

	rhoPa1=flup.MasEspAgua(PcamadaR, tRes);
	rhoPa=rhoa;
	rhoPa0=flup.MasEspAgua(PcamadaL, tRes);

	if(pmed1<pBolha-0.01) mio1=flup.ViscOleo(pmed1, tRes)/1000.;
	else mio1=flup.ViscOleo(pBolha-0.01, tRes)/1000.;
	if(pmed0<pBolha-0.01) mio0=flup.ViscOleo(pmed0, tRes)/1000.;
	else mio0=flup.ViscOleo(pBolha-0.01, tRes)/1000.;
	mig1=flup.ViscGas(pmed1, tRes)/1000.;
	mig0=flup.ViscGas(pmed0, tRes)/1000.;
	mia1=flup.VisAgua(tRes)/1000.;
	mia0=flup.VisAgua(tRes)/1000.;

	if(tipoModelOleo==1){
		double tit=flup.FracMassHidra(Pcamada, tRes);
		alf=0*tit+1*(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
		tit=flup.FracMassHidra(PcamadaR, tRes);
		alf1=0*tit+1*(tit/rhogP1)/((tit/rhogP1)+((1.-tit)/rhoP1));
		tit=flup.FracMassHidra(PcamadaL, tRes);
		alf0=0*tit+1*(tit/rhogP0)/((tit/rhogP0)+((1.-tit)/rhoP0));
		tit=flup.FracMassHidra(pmed1, tRes);
		alfmed1=0*tit+1*(tit/rhog1)/((tit/rhog1)+((1.-tit)/rho1));
		tit=flup.FracMassHidra(pmed1, tRes);
		alfmed0=0*tit+1*(tit/rhog0)/((tit/rhog0)+((1.-tit)/rho0));
	}
	else{
		alf=0;
		alf1=0;
		alf0=0;
		alfmed1=0;
		alfmed0=0;
	}

	kmed1=fkO(samed1, 1*alfmed1*(1.-samed1));
	kmed0=fkO(samed0, 1*alfmed0*(1.-samed0));
	kmedG1=0;
	kmedG0=0;
	kmedA1=interpolaTabela(kRelOACel.npont, samed1, kRelOACel.satW, kRelOACel.permRelW);
	kmedA0=interpolaTabela(kRelOACel.npont, samed0, kRelOACel.satW, kRelOACel.permRelW);
	pcOG1=0;
	pcOGm=0;
	pcOG0=0;
	pcAO1=interpolaTabela(pcOACel.npont, sWR, pcOACel.satW, pcOACel.presCapOW);
	pcAOm=interpolaTabela(pcOACel.npont, sW, pcOACel.satW, pcOACel.presCapOW);
	pcAO0=interpolaTabela(pcOACel.npont, sWL, pcOACel.satW, pcOACel.presCapOW);

	darcyO1=kmed1*kabsol1*98066.22/((1.-alfmed1)*mio1+alfmed1*mig1);
	darcyO0=kmed0*kabsol0*98066.22/((1.-alfmed0)*mio0+alfmed0*mig0);
	darcyG1=0;
	darcyG0=0;
	darcyA1=kmedA1*kabsol1*98066.22/mia1;
	darcyA0=kmedA0*kabsol0*98066.22/mia0;

	/*double grav=9.82/98066.22;

	double mo1=-darcyO1*rQcamadaR*(PcamadaR-Pcamada-zD1*rhoP1*grav+zD*rhoP*grav)/drP1;
	double mo0=-darcyO0*rQcamadaL*(Pcamada-PcamadaL-zD*rhoP*grav+zD0*rhoP0*grav)/drP0;

	double mg1=-darcyG1*rQcamadaR*(PcamadaR+pcOG1-Pcamada-pcOGm-zD1*rhogP1*grav+zD*rhogP*grav)/drP1;
	double mg0=-darcyG0*rQcamadaL*(Pcamada+pcOGm-PcamadaL-pcOG0-zD*rhogP*grav+zD0*rhogP0*grav)/drP0;

	double ma1=-darcyA1*rQcamadaR*(PcamadaR-pcAO1-Pcamada+pcAOm-zD1*rhoPa1*grav+zD*rhoPa*grav)/drP1;
	double ma0=-darcyA0*rQcamadaL*(Pcamada-pcAOm-PcamadaL+pcAO0-zD*rhoPa*grav+zD0*rhoPa0*grav)/drP0;

	QocamadaR=mo1;
	QocamadaL=mo0;
	QgcamadaR=mg1;
	QgcamadaL=mg0;
	QwcamadaR=ma1;
	QwcamadaL=ma0;*/
}

double celrad::cflO(){
   double vazTot=0.;
   if((QocamadaR+QwcamadaR)<0.)vazTot+=fabsl((QocamadaR+QwcamadaR)/(2.*M_PI));
   if((QocamadaL+QwcamadaL)>0.)vazTot+=fabsl((QocamadaL+QwcamadaL)/(2.*M_PI));
   double porod=poro*(1-compresPoro*(Pcamada-presRes));
   double massCel=porod*(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
   return massCel/vazTot;
}

double celrad::cflA(){
   double vazTot=0.;
   if(QwcamadaR<0.)vazTot+=fabsl(QwcamadaR/(2.*M_PI));
   if(QwcamadaL>0.)vazTot+=fabsl(QwcamadaL/(2.*M_PI));
   double porod=poro*(1-compresPoro*(Pcamada-presRes));
   double massCel=(sW)*porod*(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
   if(vazTot>1e-15)return massCel/vazTot;
   else return 10000000;
}

void celrad::evoluiSL(int& reinicia,int ciclo,double dpdt){

	double poro0=poro*(1-compresPoro*(Pini-presRes));
	//double area=rm*drQ;//dividido por 2*Pi
	double area=(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
	double multTO=poro0*rhostd*area/vbo;
	double multTA=poro0*area*rhoa;

	double rs1=flup.RS(pmed1, tRes)* 6.29 / 35.31467;
	double rs0=flup.RS(pmed0, tRes)* 6.29 / 35.31467;

	double dPoroRhosBO=0;
	double dPoroRhow=0;
	if(ciclo>0){
		double vbod=flup.BOFunc(Pini*1.0001, tRes);
		double porod=poro*(1-compresPoro*(Pini*1.0001-presRes));
		dPoroRhosBO=rhostd*(porod/vbod-poro0/vbo)/(Pini*0.0001);
		double rhowd=flup.MasEspAgua(Pini*1.0001, tRes);
		dPoroRhow=(porod*rhowd-poro0*rhoa)/(Pini*0.0001);
	}


	double bo1=flup.BOFunc(pmed1, tRes);
	double bo0=flup.BOFunc(pmed0, tRes);

	double multR1;

	multR1=(1.-rhogstd*rs1/(bo1*rho1))*rho1;

	double multR0;
	multR0=(1.-rhogstd*rs0/(bo0*rho0))*rho0;


	double mo1=QocamadaR/(2.*M_PI);
	double mo0=QocamadaL/(2.*M_PI);
	double ma1=QwcamadaR/(2.*M_PI);
	double ma0=QwcamadaL/(2.*M_PI);

	double balanco=(1./multTO)*(multR1*mo1-multR0*mo0)+(1./multTA)*(rhoa1*ma1-rhoa0*ma0)-
			vbo*(sLini-sWini)*dPoroRhosBO*dpdt/(poro0*rhostd)-sWini*dPoroRhow*dpdt/(poro0*rhoa);

	double val=sLini+balanco*dt;
	if(val<satConata && sLini==satConata)val=satConata;
	else if(val>1. && sLini==1.)val=1.;
    double alfx=1.-val;
    double localtiny=1e-5;

	if(((alfx<=localtiny)&&(alfx>=-localtiny))){
		   sL=1.;
	}
	else if(alfx<-localtiny){
	      double dtaux;
	      dtaux=(1.-sLini)/(balanco);
	      //if(dtaux<dt*CritDTMin)dtaux=dt*CritDTMin;
	      if(reinicia==0){
	        dtSL=dtaux;
	        //reinicia=-1;
	        reiniciaSL=-1;
	        sL=1.;
	      }
	      else sL=1.;
	}
	else if((val<=(satConata+localtiny)&&val>=(satConata-localtiny))){
		   sL=fabsl(satConata);
	}
	else if(val<(satConata-localtiny)){
	      double dtaux;
	      dtaux=(satConata-sLini)/(balanco);
	      if(reinicia==0){
	        dtSL=dtaux;
	        //reinicia=-1;
	        reiniciaSL=-1;
	        sL=fabsl(satConata);
	      }
	      else sL=fabsl(satConata);
	}
	else sL=val;


}

void celrad::evoluiSW(int& reinicia,int ciclo,double dpdt){

	dtSW=dt;
	double poro0=poro*(1-compresPoro*(Pini-presRes));
	//double area=rm*drQ;//dividido por 2*Pi
	double area=(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
	//if(icel==1)area*=2.;
	double multTA=poro0*area*rhoPaIni;

	double dPoroRhow=0;
	if(ciclo>0){
		double porod=poro*(1-compresPoro*(Pini*1.0001-presRes));
		double rhowd=flup.MasEspAgua(Pini*1.0001, tRes);
		dPoroRhow=(porod*rhowd-poro0*rhoa)/(Pini*0.0001);
	}

	double ma1;
	double ma0;

	ma1=QwcamadaR/(2.*M_PI);
	ma0=QwcamadaL/(2.*M_PI);

	double balanco=(1./multTA)*(rhoa1*ma1-rhoa0*ma0)-0*(sWini-1.*satConata)*dPoroRhow*dpdt/(poro0*rhoPaIni);

	double val=sWini+balanco*dt;
	if(val<satConata && sWini==satConata)val=satConata;
	else if(val>1. && sWini==1.)val=1.;
    double alfx=1.-val;
    double localtiny=1e-5;

	if(((val<=sL+localtiny)&&(val>=sL-localtiny))){
		   //alf=fabsl(0.);
		   sW=sL;
	}
	else if(val>sL+localtiny){
	      double dtaux;
	      if(fabsl(balanco)>1e-15)
	      dtaux=(sL-sWini)/(balanco);
	      else{
	    	  dtaux=0.;
	      }
	      //if(dtaux<dt*CritDTMin)dtaux=dt*CritDTMin;
	      if(reinicia==0 && dtaux>1e-5){
	        dtSW=dtaux;
	        //reinicia=-1;
	        reiniciaSW=-1;
	        sW=sL;
	      }
	      else{
	    	  sW=1.;
	    	  dtSW=dt;
	      }
	}
	else if((val<=(satConata+localtiny)&&val>=(satConata-localtiny))){
		   sW=fabsl(satConata);
	}
	else if(val<(satConata-localtiny)){
	      double dtaux;
	      if(fabsl(balanco)>1e-15)
	      dtaux=(satConata-sWini)/(balanco);
	      else dtaux=0.;
	      if(reinicia==0 && dtaux>1e-5){
	        dtSW=dtaux;
	        //reinicia=-1;
	        reiniciaSW=-1;
	        sW=fabsl(satConata);
	      }
	      else{
	    	  sW=fabsl(satConata);
	    	  dtSW=dt;
	      }
	}
	else sW=val;

	if(reiniciaSW==-1){
		int para;
		para=0;
	}

	//sW=0.13;
	//dtSW=dt;
	//reiniciaSW=0;


}

void celrad::transcel(int idisc){

  double cond;
  double h1;
  double hi;
  double vr0;
  double vr1;
  double vr2;
  double grav=9.82/98066.22;

  calcVazOWG();


  /*double area=(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
  double poro0=poro*(1-compresPoro*(Pcamada-presRes));
  double multTot;
  if(fabsl((Pcamada-Pini)/Pini)<1e-3){
	  double dporo=-poro*compresPoro;
	  double porod=poro*(1-compresPoro*(Pcamada*1.0001-presRes));
	  double dPoroRhoo=0;
	  double dPoroRhow=0;
	  double dPoroRhog=0;
	  double pmedT=0.5*(Pcamada+Pini);
	  double rhood=flup.MasEspoleo(pmedT*1.00001, tRes);
	  double rhogd=flup.MasEspGas(pmedT*1.00001, tRes);
	  double titd=flup.FracMassHidra(pmedT*1.00001, tRes);
	  double alfd=0*titd+1*(titd/rhogd)/((titd/rhogd)+((1.-titd)/rhood));
	  double rhooe=flup.MasEspoleo(pmedT*(1-0.00001), tRes);
	  double rhoge=flup.MasEspGas(pmedT*(1-0.00001), tRes);
	  double tite=flup.FracMassHidra(pmedT*(1-0.00001), tRes);
	  double alfe=0*titd+1*(tite/rhoge)/((tite/rhoge)+((1.-tite)/rhooe));
	  //dPoroRhoo=(porod*((1-alfd)*rhood+alfd*rhogd)-poro0*((1.-alf)*rhoP+alf*rhogP))/(Pcamada*0.001);
	  dPoroRhoo=dporo*((1.-alf)*rhoP+alf*rhogP)+
		    poro0*(((1-alfd)*rhood+alfd*rhogd)-((1.-alfe)*rhooe+alfe*rhoge))/(pmedT*0.00002);

	  dPoroRhog=(porod*rhogd-poro0*rhogP)/(Pcamada*0.0001);
	  dPoroRhog=dporo*(rhogP)+
		    poro0*((rhogd)-(rhogP))/(Pcamada*0.0001);

	  //double rhowd=flup.MasEspAgua(Pcamada*1.000001, tRes);
	  //porod=poro*(1-compresPoro*(Pcamada*1.0001-presRes));
	  //dPoroRhow=(porod*rhowd-poro0*rhoa)/(Pcamada*0.0001);
	  //dPoroRhow=dporo*(rhoa)+
		    //poro0*(((rhowd)-(rhoa))/(Pcamada*0.000001));
	  dPoroRhow=dporo*(rhoa)+
		    poro0*flup.DMasEspAgua(pmedT, tRes);
	  multTot=((sL-sW-0*satConata)*dPoroRhoo+(1.-sL)*dPoroRhog+(sW-0.*satConata)*dPoroRhow)*area;
  }
  else{
	  double poro0B=poro*(1-compresPoro*(Pini-presRes));
	  double poro1B=poro*(1-compresPoro*(Pcamada-presRes));
	  double tit0B=flup.FracMassHidra(Pini, tRes);
	  double rho0B=flup.MasEspoleo(Pini, tRes);
	  double rhog0B=flup.MasEspGas(Pini, tRes);
	  double rhoa0B=flup.MasEspAgua(Pini, tRes);
	  double alf0B=(tit0B/rhog0B)/((tit0B/rhog0B)+((1.-tit0B)/rho0B));
	  multTot=((sL-sW-0*satConata)*(poro1B*((1.-alf)*rhoP+alf*rhogP)-poro0B*((1.-alf0B)*rho0B+alf0B*rhog0B))/(Pcamada-Pini)+
			  (sW-0.*satConata)*(poro1B*rhoa-poro0B*rhoa0B)/(Pcamada-Pini))*area;
  }*/



  double area=(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
  double poro0=poro*(1-compresPoro*(Pcamada-presRes));
  double multTot;
  if(fabsl((Pcamada-Pini)/Pini)<1e-3){
	  double dporo=-poro*compresPoro;
	  double porod=poro*(1-compresPoro*(Pcamada*1.0001-presRes));
	  double dPoroRhoo=0;
	  double dPoroRhow=0;
	  double dPoroRhog=0;
	  double pmedT=0.5*(Pcamada+Pini);
	  double rhood;
	  double rhogd;
	  double titd;
	  double alfd;
	  double rhooe;
	  double rhoge;
	  double tite;
	  double alfe;
	  if(tipoModelOleo==1){
		  rhood=flup.MasEspoleo(pmedT*1.00001, tRes);
		  rhogd=flup.MasEspGas(pmedT*1.00001, tRes);
		  titd=flup.FracMassHidra(pmedT*1.00001, tRes);
		  alfd=0*titd+1*(titd/rhogd)/((titd/rhogd)+((1.-titd)/rhood));
		  rhooe=flup.MasEspoleo(pmedT*(1-0.00001), tRes);
		  rhoge=flup.MasEspGas(pmedT*(1-0.00001), tRes);
		  tite=flup.FracMassHidra(pmedT*(1-0.00001), tRes);
		  alfe=0*titd+1*(tite/rhoge)/((tite/rhoge)+((1.-tite)/rhooe));
	  }
	  else{
		  rhood=rhoRef*(1-compOleo*(presRes-pmedT*1.00001));
		  rhogd=flup.MasEspGas(pmedT*1.00001, tRes);
		  alfd=0;
		  rhooe=rhoRef*(1-compOleo*(presRes-pmedT*(1-0.00001)));
		  rhoge=flup.MasEspGas(pmedT*(1-0.00001), tRes);
		  alfe=0;
	  }
	  //dPoroRhoo=(porod*((1-alfd)*rhood+alfd*rhogd)-poro0*((1.-alf)*rhoP+alf*rhogP))/(Pcamada*0.001);
	  dPoroRhoo=dporo*((1.-alf)*rhoP+alf*rhogP)+
		    poro0*(((1-alfd)*rhood+alfd*rhogd)-((1.-alfe)*rhooe+alfe*rhoge))/(pmedT*0.00002);

	  dPoroRhog=(porod*rhogd-poro0*rhogP)/(Pcamada*0.0001);
	  dPoroRhog=dporo*(rhogP)+
		    poro0*((rhogd)-(rhogP))/(Pcamada*0.0001);

	  //double rhowd=flup.MasEspAgua(Pcamada*1.000001, tRes);
	  //porod=poro*(1-compresPoro*(Pcamada*1.0001-presRes));
	  //dPoroRhow=(porod*rhowd-poro0*rhoa)/(Pcamada*0.0001);
	  //dPoroRhow=dporo*(rhoa)+
		    //poro0*(((rhowd)-(rhoa))/(Pcamada*0.000001));
	  dPoroRhow=dporo*(rhoa)+
		    poro0*flup.DMasEspAgua(pmedT, tRes);
	  multTot=((sL-sW-0*satConata)*dPoroRhoo+(1.-sL)*dPoroRhog+(sW-1.*satConata)*dPoroRhow)*area;
  }
  else{
	  double poro0B=poro*(1-compresPoro*(Pini-presRes));
	  double poro1B=poro*(1-compresPoro*(Pcamada-presRes));
	  double tit0B;
	  double rho0B;
	  double rhog0B;
	  double rhoa0B;
	  double alf0B;
	  if(tipoModelOleo==1){
		  tit0B=flup.FracMassHidra(Pini, tRes);
		  rho0B=flup.MasEspoleo(Pini, tRes);
		  rhog0B=flup.MasEspGas(Pini, tRes);
		  rhoa0B=flup.MasEspAgua(Pini, tRes);
		  alf0B=(tit0B/rhog0B)/((tit0B/rhog0B)+((1.-tit0B)/rho0B));
	  }
	  else{
		  rho0B=rhoRef*(1-compOleo*(presRes-Pini));
		  rhog0B=flup.MasEspGas(Pini, tRes);
		  rhoa0B=flup.MasEspAgua(Pini, tRes);
		  alf0B=0;
	  }
	  multTot=((sL-sW-0*satConata)*(poro1B*((1.-alf)*rhoP+alf*rhogP)-poro0B*((1.-alf0B)*rho0B+alf0B*rhog0B))/(Pcamada-Pini)+
			  (sW-1.*satConata)*(poro1B*rhoa-poro0B*rhoa0B)/(Pcamada-Pini))*area;
  }

  /*double poro0=poro*(1-compresPoro*(Pcamada-presRes));
  double dporo=-poro*compresPoro;
  double porod=poro*(1-compresPoro*(Pcamada*1.0001-presRes));
  double dPoroRhoo=0;
  double dPoroRhow=0;
  double dPoroRhog=0;
  double pmedT=0.5*(Pcamada+Pini);
  double rhood=flup.MasEspoleo(pmedT*1.00001, tRes);
  double rhogd=flup.MasEspGas(pmedT*1.00001, tRes);
  double titd=flup.FracMassHidra(pmedT*1.00001, tRes);
  double alfd=0*titd+1*(titd/rhogd)/((titd/rhogd)+((1.-titd)/rhood));
  double rhooe=flup.MasEspoleo(pmedT*(1-0.00001), tRes);
  double rhoge=flup.MasEspGas(pmedT*(1-0.00001), tRes);
  double tite=flup.FracMassHidra(pmedT*(1-0.00001), tRes);
  double alfe=0*titd+1*(tite/rhoge)/((tite/rhoge)+((1.-tite)/rhooe));
  dPoroRhoo=dporo*((1.-alf)*rhoP+alf*rhogP)+
		    poro0*(((1-alfd)*rhood+alfd*rhogd)-((1.-alfe)*rhooe+alfe*rhoge))/(pmedT*0.00002);
  dPoroRhog=(porod*rhogd-poro0*rhogP)/(Pcamada*0.0001);
  dPoroRhog=dporo*(rhogP)+
		    poro0*((rhogd)-(rhogP))/(Pcamada*0.0001);
  dPoroRhow=dporo*(rhoa)+
		    poro0*flup.DMasEspAgua(pmedT, tRes);
  double multTot=((sL-sW-0*satConata)*dPoroRhoo+(1.-sL)*dPoroRhog+(sW-0.*satConata)*dPoroRhow)*area;*/

  double multTO=((1.-alf)*rhoP+alf*rhogP)*(sL-sW-sLini+sWini);
  double multTG=rhogP*(-sL+sLini);
  double multTA=rhoa*(sW-sWini);
  //double area=rm*(0.5*(r1-r0));//dividido por 2*Pi
  //double area=(rQcamadaR*rQcamadaR-rQcamadaL*rQcamadaL)/2.;
  double termoSat=1*poro0*(multTO+multTG+multTA)*area/dt;
  //double invert=dt/multTot;

  for(int i=0;i<4;i++){
	  localvet[i]=0.;
	  for(int j=0;j<11;j++)localmat[i][j]=0.;
  }

  vr0=r0;
  vr1=rm;
  vr2=r1;

  if(idisc==0 || idisc==ncel-1){
	  cond=kabsol1*fluc.rholStd*98066.22/fluc.VisFlu(1., tRes);
	  hi=h1=1000000000.*cond*vr1/drcamada;
  }

  if(idisc>0){
	  if(idisc<ncel-1){
  	  //localmat[0][1]=(1/vr1)/(0.5*(vr2-vr0));
  	  localmat[0][0]=((1.-alfmed0)*rho0+alfmed0*rhog0);
  	  localmat[0][1]=rhog0;
  	  localmat[0][2]=rhoa0;
  	  //localmat[0][2]=(rho*cp)/dt;
  	  localmat[0][3]=multTot/dt;
  	  //localmat[0][3]=1.;
  	  //localmat[0][3]=-(1/vr1)/(0.5*(vr2-vr0));
  	  localmat[0][4]=-((1.-alfmed1)*rho1+alfmed1*rhog1);
  	  localmat[0][5]=-rhog1;
  	  localmat[0][6]=-rhoa1;
  	  //localvet[0]=Pcamada[icam][idisc]*(rho*cp)/dt;
  	  localvet[0]=Pini*multTot/dt-termoSat;
	  }
	  else{
	  	  //localmat[0][1]=(1/vr1)/(0.5*(vr2-vr0));
	  	  localmat[0][0]=0;
	  	  localmat[0][1]=0;
	  	  localmat[0][2]=0;
	  	  //localmat[0][2]=(rho*cp)/dt;
	  	  localmat[0][3]=1;
	  	  //localmat[0][3]=-(1/vr1)/(0.5*(vr2-vr0));
	  	  localmat[0][4]=0;
	  	  localmat[0][5]=0;
	  	  localmat[0][6]=0;
	  	  //localvet[0]=Pcamada[icam][idisc]*(rho*cp)/dt;
	  	  localvet[0]=presRes2;
	  }
	  if(idisc<ncel-1){
		  /*
double ma1=-darcyA1*(PcamadaR-pcAO1-Pcamada+pcAOm-zD1*rhoPa1*grav+zD*rhoPa*grav)/drP1;
		   */
		  double grav=9.82/98066.22;
		  double ciL;
		  ciL=darcyO1*rQcamadaR/drP1;
		  localmat[1][3]=ciL;
		  localmat[1][4]=1.;
		  localmat[1][7]=-ciL;
		  localvet[1]=-ciL*0.5*((1.-alf1)*rhoP1+alf1*rhogP1+(1.-alf)*rhoP+rhogP*alf)*grav*(zD1-zD);

		  ciL=0*darcyG1*rQcamadaR/drP1;
		  localmat[2][3]=ciL;
		  localmat[2][5]=1.;
		  localmat[2][7]=-ciL;
		  localvet[2]=-ciL*(-pcOG1+pcOGm+0.5*(rhogP1+rhogP)*grav*(zD1-zD));


		  ciL=darcyA1*rQcamadaR/drP1;
		  localmat[3][3]=ciL;
		  localmat[3][6]=1.;
		  localmat[3][7]=-ciL;
		  localvet[3]=-ciL*(pcAO1-pcAOm+0.5*(rhoPa1+rhoPa)*grav*(zD1-zD));
	  }
	  else{
		 //double tit=flup.FracMass(presRes, tRes);
		 //double rhost = 141.5/(131.5+flup.API);
		 //double FW = flup.BSW*flup.Denag / ((1.-flup.BSW)*rhost+flup.BSW*flup.Denag);
		 //double fracO=(sLRes-sWRes)/(1.-satConata);
		 //double fracA=(sWRes-satConata)/(1.-satConata);
		 //double fracG=(1.-sLRes)/(1.-satConata);
		  double fracO;
		  double fracA;
		  double fracG;
		  /*if(fabsl(presRes-Pcamada)>1e-15){
			  double fatTot=fabsl((vr1/(drcamada*h1))*(Pcamada-PcamadaL)/(presRes-Pcamada));
			  fracO=darcyO1*fatTot;
			  fracA=darcyA1*fatTot;
			  fracG=darcyG1*fatTot;
		  }
		  else{
			  fracO=(sLRes-sWRes)/(1.-satConata);
			  fracA=(sWRes-satConata)/(1.-satConata);
			  fracG=(1.-sLRes)/(1.-satConata);
		  }*/
		  fracO=darcyO1/(darcyO1+darcyA1+0*darcyG1);
		  fracA=darcyA1/(darcyO1+darcyA1+0*darcyG1);
		  fracG=0*darcyG1/(darcyO1+darcyA1+0*darcyG1);
		 /*if(flup.BSW<1.-1e-15){
			 if(tit<1.-1e-15){
				 double tot=1.+tit/(1.-tit)+FW/(1.-FW);
				 fracO=1./tot;
				 fracG=(tit/(1.-tit))/tot;
				 fracA=(FW/(1.-FW))/tot;
			 }
			 else fracG=1.;
		 }
		 else fracA=1.;*/


		  /*localmat[0][3]=-((1.+0*fracO+0*fracA+0*fracG)*h1*rm+(darcyO1+darcyG1+darcyA1)*rm/drcamada);
		  localmat[0][7]=(darcyO1+darcyG1+darcyA1)*rm/drcamada;
		  localvet[0]=-(1+0*fracO+0*fracA+0*fracG)*h1*rm*presRes+(darcyO1*rm/drcamada)*(zD1*rhoP1*grav-zD*rhoP*grav)+
				  1*(darcyG1*rm/drcamada)*(-pcOG1+pcOGm+zD1*rhogP1*grav-zD*rhogP*grav)+
				  1*(darcyA1*rm/drcamada)*(pcAO1-pcAOm+zD1*rhoPa1*grav-zD*rhoPa*grav);*/
		  h1=(darcyO0+0*darcyG0+darcyA0)/drcamada;
		 localmat[1][3]=fracO*h1*vr2;
		 localmat[1][4]=1.;
		 localvet[1]=fracO*h1*vr2*presRes2;

		 localmat[2][3]=0*fracG*h1*vr2;
		 localmat[2][5]=1.;
		 localvet[2]=0*fracG*h1*vr2*presRes2;

		 localmat[3][3]=fracA*h1*vr2;
		 localmat[3][6]=1.;
		 localvet[3]=fracA*h1*vr2*presRes2;
	  }
  }
  else{
	  //double tit=flup.FracMass(Pint, tRes);
	  //double rhost = 141.5/(131.5+flup.API);
	  //double FW = flup.BSW*flup.Denag / ((1.-flup.BSW)*rhost+flup.BSW*flup.Denag);
	  //double fracO=(sL-sW)/(1.-satConata);
	  //double fracA=(sW-satConata)/(1.-satConata);
	  //double fracG=(1.-sL)/(1.-satConata);
	  double fracO;
	  double fracA;
	  double fracG;
	 /* if(fabsl(Pcamada-Pint)>1e-15){
		  double fatTot=fabsl((rm/(drcamada*h1))*(PcamadaR-Pcamada)/(Pcamada-Pint));
		  fracO=darcyO1*fatTot;
		  fracA=darcyA1*fatTot;
		  fracG=darcyG1*fatTot;
	  }
	  else{
		  fracO=(sL-sW)/(1.-satConata);
		  fracA=(sW-satConata)/(1.-satConata);
		  fracG=(1.-sL)/(1.-satConata);
	  }*/

		double ko;
		double ka;
		double dO;
		double dG;
		double dA;
		if((QwcamadaR+QocamadaR+QgcamadaR)>=0.){
			ko=fkO(sWR,alf);
			ka=interpolaTabela(kRelOACel.npont, sWR, kRelOACel.satW, kRelOACel.permRelW);

			double mio;
			double mia;
			double mig;
			if( PcamadaR<pBolha-0.01) mio=flup.ViscOleo( PcamadaR, tRes)/1000.;
			else mio=flup.ViscOleo(pBolha-0.01, tRes)/1000.;
			mig=flup.ViscGas( PcamadaR, tRes)/1000.;
			mia=flup.VisAgua(tRes)/1000.;
			double alfC;
			if(tipoModelOleo==1){
				double tit=flup.FracMassHidra(PcamadaR, tRes);
				alfC=(tit/rhogP1)/((tit/rhogP1)+((1.-tit)/rhoP1));
			}
			else{
				alfC=0;
			}

			dO=ko/((1.-alfC)*mio+alfC*mig);
			dG=0.;
			dA=ka/mia;
		}
		else{
			ko=fkO(sW,alf);
			ka=interpolaTabela(kRelOACel.npont, sW, kRelOACel.satW, kRelOACel.permRelW);

			double mio;
			double mia;
			double mig;
			if( Pcamada<pBolha-0.01) mio=flup.ViscOleo( Pcamada, tRes)/1000.;
			else mio=flup.ViscOleo(pBolha-0.01, tRes)/1000.;
			mig=flup.ViscGas( Pcamada, tRes)/1000.;
			mia=flup.VisAgua(tRes)/1000.;
			double alfC;
			if(tipoModelOleo==1){
				double tit=flup.FracMassHidra(Pcamada, tRes);
				alfC=(tit/rhogP)/((tit/rhogP)+((1.-tit)/rhoP));
			}
			else{
				alfC=0;
			}

			dO=ko/((1.-alfC)*mio+alfC*mig);
			dG=0.;
			dA=ka/mia;
		}

		 fracO=dO/(dO+dA);
		 fracA=dA/(dO+dA);
		 fracG=0.;

	  //fracO=ko/(ko+ka);
	  //fracA=ka/(ko+ka);
	  //fracG=0;
	  /*if(flup.BSW<1.-1e-15){
			 if(tit<1.-1e-15){
				 double tot=1.+tit/(1.-tit)+FW/(1.-FW);
				 fracO=1./tot;
				 fracG=(tit/(1.-tit))/tot;
				 fracA=(FW/(1.-FW))/tot;
			 }
			 else fracG=1.;
	  }
	  else fracA=1.;*/
	  h1=ip/(2*M_PI*rm*86400);


	  localmat[0][3]=-((1.+0*fracO+0*fracA+0*fracG)*h1*rm+(dO+0*dG+dA)*rQcamadaR/drcamada);
	  localmat[0][7]=(dO+0*dG+dA)*rQcamadaR/drcamada;
	  localvet[0]=-(1+0*fracO+0*fracA+0*fracG)*h1*rm*Pint+0.*(dO*rm/drcamada)*(zD1*rhoP1*grav-zD*rhoP*grav)+
			  0*(dG*rm/drcamada)*(-pcOG1+pcOGm+zD1*rhogP1*grav-zD*rhogP*grav)+
			  0*(dA*rm/drcamada)*(pcAO1-pcAOm+zD1*rhoPa1*grav-zD*rhoPa*grav);
	  localmat[1][3]=-fracO*h1*rm;
	  localmat[1][4]=1.;
	  localvet[1]=-fracO*h1*rm*Pint;
	  localmat[2][3]=-fracG*h1*rm;
	  localmat[2][5]=1.;
	  localvet[2]=-fracG*h1*rm*Pint;
	  localmat[3][3]=-fracA*h1*rm;
	  localmat[3][6]=1.;
	  localvet[3]=-fracA*h1*rm*Pint;
	  //localmat[0][3]=1.;
	  //localvet[0]=Pint;
	  /*localmat[1][8]=1.;
	  localmat[1][4]=-1.;
	  localmat[2][9]=1;
	  localmat[2][5]=-1.;
	  localmat[3][10]=1;
	  localmat[3][6]=-1.;*/
  }


}

void celrad::FeiticoDoTempo(){
	 Pcamada=Pini;
	 QcamadaR=QiniR;
	 QcamadaL=QiniL;
	 QwcamadaR=Qwini;
	 QocamadaR=Qoini;
	 QgcamadaR=Qgini;
	 sL=sLini;
	 sW=sWini;
}

void celrad::FeiticoDoTempoPQ(){
	 Pcamada=Pini;
	 QcamadaR=QiniR;
	 QcamadaL=QiniL;
	 QwcamadaR=Qwini;
	 QocamadaR=Qoini;
	 QgcamadaR=Qgini;
}


