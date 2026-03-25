/*
 * variaveisGlobais1D.h
 *
 *  Created on: 10 de out. de 2024
 *      Author: Eduardo
 */

#ifndef VARIAVEISGLOBAIS1D_H_
#define VARIAVEISGLOBAIS1D_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>


/*struct noRede{
	int naflu=0;
	int ncole=0;
	int aflu[20];
	int cole[20];
	double normaP1=1000.;
	double normaP0=1000.;
	double normaMass1=1000.;
	double normaMass0=1000.;
	int cadastrado=0;
};*/


struct varGlob1D{
	int partidaVF;
	double relaxVF;
	double relaxVFExpli;
	double relaxVFPcor;
	double gravVF;
	double vertY;
	int iterPresCor;
	double tempo;
	int nvfHR;
	int nvfHRHol;
	int atrasaHR;
	int nxMH;
	int nyMH;
	int metodoAcopPV;
	double angY;
	double angX;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	int eleYmaxWall;
	double mulFC;
	double corrigeDistor;
	int tipoRes;
	double cflG;
	int temInterface;
	int reiniciaVF;
	int aplicaVOF;
	int correcForcCorp;
	int iliq;
	int igas;

	double centroX;
	double centroY;
	int nmaterialVF;
	int acop;
	double angAcop;
	double tParede;
	double tAmb;
	double vAmb;
	int amb;

	double lixo5;
	int contador;
	double localtiny;
	double CritCond;
	double CritDTMin;
	int contaExit;

	double lixo5R;//alteracao7
	double TmaxR;//alteracao7

	double RGOMax;

	int chaverede;
	int fluidoRede;
	int chaveAnelGL;
	int chaveRedeParalela;
	int iterRede;
	int restartRede;
	double dtrede;//alteracao7
	double somaarea;
	double somavaz;
	double somavazG;
	double relax;
	int chaveredeT;//alteracao7
	int iterRedeT;//alteracao7
	int qualRede;
	int qualTramo;
	int verificaTramo;
	int tipoFluidoRedeGlob;

	double erroRede;
	double norma0;

	double arearef;

	int numthreads;
	int ntrd;
	int ntrdGlob;
	int ntrd3DP;

	int modoTransiente;
	int simulaTransiente;
	int sequenciaAS;

	int narq;

	int tipoModeloDrift;

	int blackOilTemp;


	//vector<noRede> normaEvol;

	varGlob1D(){
		partidaVF=0;
		relaxVF=0.7;
		relaxVFExpli=0.9;
		relaxVFPcor=0.1;
		gravVF=9.82;
		vertY=0;
		iterPresCor=0;
		tempo=0;
		nvfHR=0;
		nvfHRHol=1;
		atrasaHR=1;
		nxMH=0;
		nyMH=0;
		metodoAcopPV=0;
		angY=0.;
		angX=0.;
		xmin=0;
		xmax=0;
		ymin=0;
		ymax=0;
		eleYmaxWall=0;
		mulFC=1.;
		corrigeDistor=0.;
		tipoRes=0;
		cflG=0.5;
		temInterface=0;
		reiniciaVF=0;
		aplicaVOF=1;
		correcForcCorp=1;
		iliq=0;
		igas=0;

		centroX=0;
		centroY=0;
		nmaterialVF=0;
		acop=0;
		angAcop=0;
		tParede=0;
		tAmb=0;
		vAmb=0;
		amb=0;

		lixo5=0;
		contador=0;
		localtiny = 1e-9;
		CritCond = 1e-9;
		CritDTMin = 1e-5;
		contaExit = 0;

		lixo5R=0;//alteracao7
		TmaxR=0;//alteracao7

		RGOMax = 9400000.;

		chaverede = 0;
		fluidoRede=1;
		chaveAnelGL=0;
		chaveRedeParalela=0;
		iterRede = 0;
		restartRede = 1;
		dtrede=0;//alteracao7
		somaarea = 0;
		somavaz = 0;
		somavazG = 0;
		relax = 0.5;
		chaveredeT=0;//alteracao7
		iterRedeT=0;//alteracao7
		qualRede=-1;
		qualTramo=-1;
		verificaTramo=0;
		tipoFluidoRedeGlob=0;

		erroRede=1000;
		norma0=10000;

		double MPI=3.14159265359;

		arearef=0.25*MPI*(4*4)*2.54*2.54/10000;

		numthreads =1;
		ntrd=1;
		ntrdGlob=1;
		ntrd3DP=1;

		modoTransiente=0;
		simulaTransiente=0;
		sequenciaAS=0;
		narq=1;

		tipoModeloDrift=1;

		blackOilTemp=0;
	}
	varGlob1D& operator=(varGlob1D& vtemp){
		if (this != &vtemp) {
			partidaVF=vtemp.partidaVF;
			relaxVF=vtemp.relaxVF;
			relaxVFExpli=vtemp.relaxVFExpli;
			relaxVFPcor=vtemp.relaxVFPcor;
			gravVF=vtemp.gravVF;
			vertY=vtemp.vertY;
			iterPresCor=vtemp.iterPresCor;
			tempo=vtemp.tempo;
			nvfHR=vtemp.nvfHR;
			nvfHRHol=vtemp.nvfHRHol;
			atrasaHR=vtemp.atrasaHR;
			nxMH=vtemp.nxMH;
			nyMH=vtemp.nyMH;
			metodoAcopPV=vtemp.metodoAcopPV;
			angY=vtemp.angY;
			angX=vtemp.angX;
			xmin=vtemp.xmin;
			xmax=vtemp.xmax;
			ymin=vtemp.ymin;
			ymax=vtemp.ymax;
			eleYmaxWall=vtemp.eleYmaxWall;
			mulFC=vtemp.mulFC;
			corrigeDistor=vtemp.corrigeDistor;
			tipoRes=vtemp.tipoRes;
			cflG=vtemp.cflG;
			temInterface=vtemp.temInterface;
			reiniciaVF=vtemp.reiniciaVF;
			aplicaVOF=vtemp.aplicaVOF;
			correcForcCorp=vtemp.correcForcCorp;
			iliq=vtemp.iliq;
			igas=vtemp.igas;

			centroX=vtemp.centroX;
			centroY=vtemp.centroY;
			nmaterialVF=vtemp.nmaterialVF;
			acop=vtemp.acop;
			angAcop=vtemp.angAcop;
			tParede=vtemp.tParede;
			tAmb=vtemp.tAmb;
			vAmb=vtemp.vAmb;
			amb=vtemp.amb;

			lixo5=vtemp.lixo5;
			contador=vtemp.contador;
			localtiny =vtemp.localtiny;
			CritCond =vtemp.CritCond;
			CritDTMin =vtemp.CritDTMin;
			contaExit =vtemp.contaExit;

			lixo5R=vtemp.lixo5R;//alteracao7
			TmaxR=vtemp.TmaxR;//alteracao7

			RGOMax =vtemp.RGOMax;

			chaverede =vtemp.chaverede;
			fluidoRede=vtemp.fluidoRede;
			chaveAnelGL=vtemp.chaveAnelGL;
			chaveRedeParalela=vtemp.chaveRedeParalela;
			iterRede =vtemp.iterRede;
			restartRede =vtemp.restartRede;
			dtrede=vtemp.dtrede;//alteracao7
			somaarea =vtemp.somaarea;
			somavaz =vtemp.somavaz;
			somavazG =vtemp.somavazG;
			relax =vtemp.relax;
			chaveredeT=vtemp.chaveredeT;//alteracao7
			iterRedeT=vtemp.iterRede;//alteracao7
			qualRede=vtemp.qualRede;
			qualTramo=vtemp.qualTramo;
			verificaTramo=vtemp.verificaTramo;
			tipoFluidoRedeGlob=vtemp.tipoFluidoRedeGlob;

			erroRede=vtemp.erroRede;
			norma0=vtemp.norma0;

			arearef=vtemp.arearef;

			numthreads =vtemp.numthreads;
			ntrd=vtemp.ntrd;
			ntrdGlob=vtemp.ntrdGlob;
			ntrd3DP=vtemp.ntrd3DP;

			modoTransiente=vtemp.modoTransiente;
			simulaTransiente=vtemp.simulaTransiente;
			sequenciaAS=vtemp.sequenciaAS;
			narq=vtemp.narq;
			tipoModeloDrift=vtemp.tipoModeloDrift;
			blackOilTemp=vtemp.blackOilTemp;
			//normaEvol.clear();
			//int dim=vtemp.normaEvol.size();
			//for(int i=0; i<dim; i++)normaEvol.push_back(vtemp.normaEvol[i]);
		}
		return *this;
	}
};



#endif /* VARIAVEISGLOBAIS1D_H_ */
