/*
 * estruturas.h
 *
 *  Created on: 28 de jul. de 2022
 *      Author: eduardo
 */

#ifndef ESTRUTURAS_H_
#define ESTRUTURAS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct detTempo{
	int perm;
	int trans;
	double tmax;
	double relax;
	double relaxP;
	double relaxT;
	int nvfHR;
	int parserie;
	double* dtmax;
	double* tempoDT;

	double* tempoImp;
	int parserieImp;
	int confinado;
	int pRef;

	double tendTemp;

	double errP;
	double errV;
	double errT;
	double erroRes;
	int maxIt;
	detTempo(){
		perm=0;
		trans=0;
		tmax=0;
		relax=0;
		relaxP=0;
		nvfHR=0;
		parserie=0;
		dtmax=0;
		tempoDT=0;

		tempoImp=0;
		parserieImp=0;
		confinado=0;
		pRef=0;

		tendTemp=0;

		errP=0;
		errV=0;
		errT=0;
		erroRes=0;
		maxIt=0;
	}
	detTempo(const detTempo& vtemp){
		perm=vtemp.perm;
		trans=vtemp.trans;
		tmax=vtemp.tmax;
		relax=vtemp.relax;
		relaxP=vtemp.relaxP;
		nvfHR=vtemp.nvfHR;
		parserie=vtemp.parserie;
		parserieImp=vtemp.parserieImp;
		dtmax=new double [parserie];
		tempoDT=new double [parserie];
		tempoImp=new double [parserieImp];
		for(int i=0;i<parserie;i++){
			dtmax[i]=vtemp.dtmax[i];
			tempoDT[i]=vtemp.tempoDT[i];
		}
		for(int i=0;i<parserieImp;i++)tempoImp[i]=vtemp.tempoImp[i];
		confinado=vtemp.confinado;
		pRef=vtemp.pRef;

		tendTemp=vtemp.tendTemp;

		errP=vtemp.errP;
		errV=vtemp.errV;
		errT=vtemp.errT;
		erroRes=vtemp.erroRes;
		maxIt=vtemp.maxIt;
	}
	detTempo& operator=(detTempo& vtemp){
		if (this != &vtemp) {
		   	if(parserie>0){
		    	delete [] dtmax;
		    	delete [] tempoDT;
		   	}
		   	if(parserieImp>0) delete [] tempoImp;
			perm=vtemp.perm;
			trans=vtemp.trans;
			tmax=vtemp.tmax;
			relax=vtemp.relax;
			relaxP=vtemp.relaxP;
			nvfHR=vtemp.nvfHR;
			parserie=vtemp.parserie;
			parserieImp=vtemp.parserieImp;
			dtmax=new double [parserie];
			tempoDT=new double [parserie];
			tempoImp=new double [parserieImp];
			for(int i=0;i<parserie;i++){
				dtmax[i]=vtemp.dtmax[i];
				tempoDT[i]=vtemp.tempoDT[i];
			}
			for(int i=0;i<parserieImp;i++)tempoImp[i]=vtemp.tempoImp[i];
			confinado=vtemp.confinado;
			pRef=vtemp.pRef;

			tendTemp=vtemp.tendTemp;

			errP=vtemp.errP;
			errV=vtemp.errV;
			errT=vtemp.errT;
			erroRes=vtemp.erroRes;
			maxIt=vtemp.maxIt;
		}
		return *this;
	}
	~detTempo(){
	   	if(parserie>0){
	    	delete [] dtmax;
	    	delete [] tempoDT;
	    	parserie=0;
	   	}
	   	if(parserieImp>0){
	   		delete [] tempoImp;
	   		parserieImp=0;
	   	}
	 }
};
struct detProp{
	int parserie;
	double* rho;
	double* visc;
	double* cp;
	double* cond;
	double* beta;
	double* tRef;
	~detProp(){
	   	if(parserie>0){
	    	delete [] rho;
	    	delete [] visc;
	    	delete [] cp;
	    	delete [] cond;
	    	delete [] beta;
	    	delete [] tRef;
	    	parserie=0;
	   	}
	 }
};
struct detMapProp{
	int parserie;
	int* interno;
	int* inferior;
	//double* xcoor;
	//double* ymax;
	//double* ymin;
	int* indProp;
	~detMapProp(){
	   	if(parserie>0){
	    	//delete [] xcoor;
	    	delete [] interno;
	    	//delete [] ymax;
	    	//delete [] ymin;
	    	delete [] indProp;
	    	parserie=0;
	   	}
	 }
};
struct detCI{
	int parserie;
	double* xmax;
	double* xmin;
	double* ymax;
	double* ymin;
	double* temp;
	double* valU;
	double* valV;
	double* pres;
	~detCI(){
	   	if(parserie>0){
	    	delete [] xmax;
	    	delete [] xmin;
	    	delete [] ymax;
	    	delete [] ymin;
	    	delete [] temp;
	    	delete [] valU;
	    	delete [] valV;
	    	delete [] pres;
	    	parserie=0;
	   	}
	 }
};
struct detInl{
	int rotulo;
	string rotuloSTR;
	double* valU;
	double* valV;
	double* tempo;
	int nserie;
	detInl(){
	    rotulo=-1;
	    rotuloSTR="vazio";
		valU=0;
		valV=0;
		tempo=0;
		nserie=0;
	}
	detInl(const detInl& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	    nserie=vinl.nserie;
	    valU=new double [nserie];
		valV=new double [nserie];
		tempo=new double [nserie];
		for(int i=0; i<nserie; i++){
			valU[i]=vinl.valU[i];
			valV[i]=vinl.valV[i];
			tempo[i]=vinl.tempo[i];
		}
	}
	detInl& operator=(detInl& vinl){
		if (this != &vinl) {
		   	if(nserie>0){
		    	delete [] valU;
		    	delete [] valV;
		    	delete [] tempo;
		   	}
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		    nserie=vinl.nserie;
		    valU=new double [nserie];
			valV=new double [nserie];
			tempo=new double [nserie];
			for(int i=0; i<nserie; i++){
				valU[i]=vinl.valU[i];
				valV[i]=vinl.valV[i];
				tempo[i]=vinl.tempo[i];
			}
		}
		return *this;
	}
	~detInl(){
	   	if(nserie>0){
	    	delete [] valU;
	    	delete [] valV;
	    	delete [] tempo;
	    	nserie=0;
	   	}
	}
};
struct detPres{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* tempo;
	int nserie;
	detPres(){
	    rotulo=-1;
	    rotuloSTR="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	detPres(const detPres& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	    nserie=vinl.nserie;
	    val=new double [nserie];
		tempo=new double [nserie];
		for(int i=0; i<nserie; i++){
			val[i]=vinl.val[i];
			tempo[i]=vinl.tempo[i];
		}
	}
	detPres& operator=(detPres& vinl){
		if (this != &vinl) {
		   	if(nserie>0){
		    	delete [] val;
		    	delete [] tempo;
		   	}
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		    nserie=vinl.nserie;
		    val=new double [nserie];
			tempo=new double [nserie];
			for(int i=0; i<nserie; i++){
				val[i]=vinl.val[i];
				tempo[i]=vinl.tempo[i];
			}
		}
		return *this;
	}
	~detPres(){
		if(nserie>0){
		   delete [] val;
		   delete [] tempo;
		   nserie=0;
		}
	}
};
struct detWall{
	int rotulo;
	string rotuloSTR;
	double* velW;
	double* tempo;
	int nserie;
	detWall(){
	    rotulo=-1;
	    rotuloSTR="vazio";
	    velW=0;
		tempo=0;
		nserie=0;
	}
	detWall(const detWall& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	    nserie=vinl.nserie;
	    velW=new double [nserie];
		tempo=new double [nserie];
		for(int i=0; i<nserie; i++){
			velW[i]=vinl.velW[i];
			tempo[i]=vinl.tempo[i];
		}
	}
	detWall& operator=(detWall& vinl){
		if (this != &vinl) {
		   	if(nserie>0){
		    	delete [] velW;
		    	delete [] tempo;
		   	}
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		    nserie=vinl.nserie;
		    velW=new double [nserie];
			tempo=new double [nserie];
			for(int i=0; i<nserie; i++){
				velW[i]=vinl.velW[i];
				tempo[i]=vinl.tempo[i];
			}
		}
		return *this;
	}
	~detWall(){
	   	if(nserie>0){
	    	delete [] velW;
	    	delete [] tempo;
	    	nserie=0;
	   	}
	}
};
struct detSim{
	int rotulo;
	string rotuloSTR;
	detSim(){
	    rotulo=-1;
	    rotuloSTR="vazio";
	}
	detSim(const detSim& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	}
	detSim& operator=(detSim& vinl){
		if (this != &vinl) {
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		}
		return *this;
	}
};
struct detDiri{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* tempo;
	int nserie;
	detDiri(){
	    rotulo=-1;
	    rotuloSTR="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	detDiri(const detDiri& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	    nserie=vinl.nserie;
	    val=new double [nserie];
		tempo=new double [nserie];
		for(int i=0; i<nserie; i++){
			val[i]=vinl.val[i];
			tempo[i]=vinl.tempo[i];
		}
	}
	detDiri& operator=(detDiri& vinl){
		if (this != &vinl) {
		   	if(nserie>0){
		    	delete [] val;
		    	delete [] tempo;
		   	}
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		    nserie=vinl.nserie;
		    val=new double [nserie];
			tempo=new double [nserie];
			for(int i=0; i<nserie; i++){
				val[i]=vinl.val[i];
				tempo[i]=vinl.tempo[i];
			}
		}
		return *this;
	}
	~detDiri(){
	   	if(nserie>0){
	    	delete [] val;
	    	delete [] tempo;
	    	nserie=0;
	   	}
	}
};
struct detVN{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* tempo;
	int nserie;
	detVN(){
	    rotulo=-1;
	    rotuloSTR="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	detVN(const detVN& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	    nserie=vinl.nserie;
	    val=new double [nserie];
		tempo=new double [nserie];
		for(int i=0; i<nserie; i++){
			val[i]=vinl.val[i];
			tempo[i]=vinl.tempo[i];
		}
	}
	detVN& operator=(detVN& vinl){
		if (this != &vinl) {
		   	if(nserie>0){
		    	delete [] val;
		    	delete [] tempo;
		   	}
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		    nserie=vinl.nserie;
		    val=new double [nserie];
			tempo=new double [nserie];
			for(int i=0; i<nserie; i++){
				val[i]=vinl.val[i];
				tempo[i]=vinl.tempo[i];
			}
		}
		return *this;
	}
	~detVN(){
		if(nserie>0){
		   delete [] val;
		   delete [] tempo;
		   nserie=0;
		}
	}
};
struct detRic{
	int rotulo;
	string rotuloSTR;
	double* valAmb;
	double* hAmb;
	double* tempo;
	int nserie;
	detRic(){
	    rotulo=-1;
	    rotuloSTR="vazio";
		valAmb=0;
		hAmb=0;
		tempo=0;
		nserie=0;
	}
	detRic(const detRic& vinl){
	    rotulo=vinl.rotulo;
	    rotuloSTR=vinl.rotuloSTR;
	    nserie=vinl.nserie;
	    valAmb=new double [nserie];
		hAmb=new double [nserie];
		tempo=new double [nserie];
		for(int i=0; i<nserie; i++){
			valAmb[i]=vinl.valAmb[i];
			hAmb[i]=vinl.hAmb[i];
			tempo[i]=vinl.tempo[i];
		}
	}
	detRic& operator=(detRic& vinl){
		if (this != &vinl) {
		   	if(nserie>0){
		    	delete [] valAmb;
		    	delete [] hAmb;
		    	delete [] tempo;
		   	}
		    rotulo=vinl.rotulo;
		    rotuloSTR=vinl.rotuloSTR;
		    nserie=vinl.nserie;
		    valAmb=new double [nserie];
			hAmb=new double [nserie];
			tempo=new double [nserie];
			for(int i=0; i<nserie; i++){
				valAmb[i]=vinl.valAmb[i];
				hAmb[i]=vinl.hAmb[i];
				tempo[i]=vinl.tempo[i];
			}
		}
		return *this;
	}

	~detRic(){
	   	if(nserie>0){
	    	delete [] valAmb;
	    	delete [] hAmb;
	    	delete [] tempo;
	    	nserie=0;
	   	}
	}
};
struct detCC{
	int nInl;
	int nOut;
	int nWall;
	int nSim;
	int nDiri;
	int nRic;
	int nVN;
	detInl* ccInl;
	detPres* ccPres;
	detWall* ccWall;
	detSim* ccSim;
	detDiri* ccDir;
	detRic* ccRic;
	detVN* ccVN;
	int rotuloAcop;

	detCC(){
		nInl=0;
		nOut=0;
		nWall=0;
		nSim=0;
		nDiri=0;
		nRic=0;
		nVN=0;
		ccInl=0;
		ccPres=0;
		ccWall=0;
		ccSim=0;
		ccDir=0;
		ccRic=0;
		ccVN=0;
		rotuloAcop=-1;
	}
	detCC(const detCC& vinl){
	    rotuloAcop=vinl.rotuloAcop;
		nInl=vinl.nInl;
		nOut=vinl.nOut;
		nWall=vinl.nWall;
		nSim=vinl.nSim;
		nDiri=vinl.nDiri;
		nRic=vinl.nRic;
		nVN=vinl.nVN;
		ccInl=new detInl [nInl];
		ccPres=new detPres [nOut];
		ccWall=new detWall [nWall];
		ccSim=new detSim [nSim];
		ccDir=new detDiri [nDiri];
		ccRic=new detRic [nRic];
		ccVN=new detVN [nVN];
		for(int i=0; i<nInl; i++) ccInl[i]=vinl.ccInl[i];
		for(int i=0; i<nOut; i++) ccPres[i]=vinl.ccPres[i];
		for(int i=0; i<nWall; i++) ccWall[i]=vinl.ccWall[i];
		for(int i=0; i<nSim; i++) ccSim[i]=vinl.ccSim[i];
		for(int i=0; i<nDiri; i++) ccDir[i]=vinl.ccDir[i];
		for(int i=0; i<nRic; i++) ccRic[i]=vinl.ccRic[i];
		for(int i=0; i<nVN; i++) ccVN[i]=vinl.ccVN[i];

	}
	detCC& operator=(detCC& vinl){
		if (this != &vinl) {
		   	if(nInl>0) delete [] ccInl;
		   	if(nOut>0) delete [] ccPres;
		   	if(nWall>0) delete [] ccWall;
		   	if(nSim>0) delete [] ccSim;
		   	if(nDiri>0) delete [] ccDir;
		   	if(nRic>0) delete [] ccRic;
		   	if(nVN>0) delete [] ccVN;
		    rotuloAcop=vinl.rotuloAcop;
			nInl=vinl.nInl;
			nOut=vinl.nOut;
			nWall=vinl.nWall;
			nSim=vinl.nSim;
			nDiri=vinl.nDiri;
			nRic=vinl.nRic;
			nVN=vinl.nVN;
			ccInl=new detInl [nInl];
			ccPres=new detPres [nOut];
			ccWall=new detWall [nWall];
			ccSim=new detSim [nSim];
			ccDir=new detDiri [nDiri];
			ccRic=new detRic [nRic];
			ccVN=new detVN [nVN];
			for(int i=0; i<nInl; i++) ccInl[i]=vinl.ccInl[i];
			for(int i=0; i<nOut; i++) ccPres[i]=vinl.ccPres[i];
			for(int i=0; i<nWall; i++) ccWall[i]=vinl.ccWall[i];
			for(int i=0; i<nSim; i++) ccSim[i]=vinl.ccSim[i];
			for(int i=0; i<nDiri; i++) ccDir[i]=vinl.ccDir[i];
			for(int i=0; i<nRic; i++) ccRic[i]=vinl.ccRic[i];
			for(int i=0; i<nVN; i++) ccVN[i]=vinl.ccVN[i];
		}
		return *this;
	}

	~detCC(){
	   	if(nInl>0){
	   		delete [] ccInl;
	   		nInl=0;
	   	}
	   	if(nOut>0){
	   		delete [] ccPres;
	   		nOut=0;
	   	}
	   	if(nWall>0){
	   		delete [] ccWall;
	   		nWall=0;
	   	}
	   	if(nSim>0){
	   		delete [] ccSim;
	   		nSim=0;
	   	}
	   	if(nDiri>0){
	   		delete [] ccDir;
	   		nDiri=0;
	   	}
	   	if(nRic>0){
	   		delete [] ccRic;
	   		nRic=0;
	   	}
	   	if(nVN>0){
	   		delete [] ccVN;
	   		nVN=0;
	   	}
	 }
};

struct detMalhaHom{
	double xcoor;
	double ycoor;
	double delx;
	double dely;
	int indEle;
	int buraco;
};

struct detInterFace{
	double* xcoor;
	double* ycoor;
	int nserie;
	double ymax;
	double ymin;
	double xmax;
	double xmin;
	detInterFace(){
		xcoor=0;
		ycoor=0;
		nserie=0;
		ymax=0;
		ymin=0;
		xmax=0;
		xmin=0;
	}
	~detInterFace(){
		if(nserie>0){
		   delete [] xcoor;
		   delete [] ycoor;
		   nserie=0;
		}
	}
};

//struct relacionada com o detalhamento do material na parede da tubulacao;
struct materialVF {
    int id;
    double cond;//condutividade, W/mK;
    double cp;//calor especifico J/kgK;
    double rho;//massa especifica kg/m3;
    int tipo;//tipo de material, 0-> solido, 1-> fluido (usuario), 2-> agua, 3->ar;
    double visc;//viscosidade, cP, apenas para tipo 1;
    double beta;//expansividade termica, apenas para tipo 1;
    materialVF(){
    	  id=0;
    	  cond=0;//condutividade, W/mK;
    	  cp=0;//calor especifico J/kgK;
    	  rho=0;//massa especifica kg/m3;
    	  tipo=0;//tipo de material, 0-> solido, 1-> fluido (usuario), 2-> agua, 3->ar;
    	  visc=0;//viscosidade, cP, apenas para tipo 1;
    	  beta=0;
    }
};

//struct com o detalhamento da secao transversal do duto;
struct cortedutoVF {
    int id;
    int ncam;//numero de camadas de material;
    int anul;//se regiao de escoamento e ou nao anular
    double a;//diametro interno, se anular, menor diametro do anular
    double b;//para o caso anular, maior diametro do anular
    double rug;//rugosidade absoluta, metros;
    double* diam;//vetor com o maior diemtro de cada camada de material;
    int* indmat;//vetor com o indicador do material, relaciona-se com a struct material
    int* discre;//discretizacao de cada camada de material
    cortedutoVF(){
        id=0;
        ncam=0;//numero de camadas de material;
        anul=0;//se regiao de escoamento e ou nao anular
        a=0;//diametro interno, se anular, menor diametro do anular
        b=0;//para o caso anular, maior diametro do anular
        rug=0;//rugosidade absoluta, metros;
        diam=0;//vetor com o maior diemtro de cada camada de material;
        indmat=0;//vetor com o indicador do material, relaciona-se com a struct material
        discre=0;//discretizacao de cada camada de material
    }
};



#endif /* ESTRUTURAS_H_ */
