/*
 * estruturas.h
 *
 *  Created on: 28 de jul. de 2022
 *      Author: eduardo
 */

#ifndef ESTRUTURASPOISSON_H_
#define ESTRUTURASPOISSON_H_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct detTempoPoisson{
	int perm;
	int trans;
	double tmax;
	double relax;
	int parserie;
	double* dtmax;
	double* tempo;

	double* tempoImp;
	int parserieImp;
	detTempoPoisson(){
		perm=0;
		trans=0;
		tmax=0;
		relax=0;
		parserie=0;
		dtmax=0;
		tempo=0;
		tempoImp=0;
		parserieImp=0;
	}
	~detTempoPoisson(){
	   	if(parserie>0){
	    	delete [] dtmax;
	    	delete [] tempo;
	   	}
	   	if(parserieImp>0) delete [] tempoImp;
	 }
};
struct detPropPoisson{
	int parserie;
	double* xmax;
	double* xmin;
	double* ymax;
	double* ymin;
	double* rho;
	double* cp;
	double* cond;
	detPropPoisson(){
		parserie=0;
		xmax=0;
		xmin=0;
		ymax=0;
		ymin=0;
		rho=0;
		cp=0;
		cond=0;
	}
	~detPropPoisson(){
	   	if(parserie>0){
	    	delete [] xmax;
	    	delete [] xmin;
	    	delete [] ymax;
	    	delete [] ymin;
	    	delete [] rho;
	    	delete [] cp;
	    	delete [] cond;
	   	}
	 }
};
struct detCIPoisson{
	int parserie;
	double* xmax;
	double* xmin;
	double* ymax;
	double* ymin;
	double* temp;
	detCIPoisson(){
		parserie=0;
		xmax=0;
		xmin=0;
		ymax=0;
		ymin=0;
		temp=0;
	}
	~detCIPoisson(){
	   	if(parserie>0){
	    	delete [] xmax;
	    	delete [] xmin;
	    	delete [] ymax;
	    	delete [] ymin;
	    	delete [] temp;
	   	}
	 }
};
struct detDiriPoisson{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* tempo;
	int nserie;
	detDiriPoisson(){
		rotulo=0;
		rotuloSTR="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	detDiriPoisson(const detDiriPoisson& condic){
	   	rotulo=condic.rotulo;
	   	rotuloSTR=condic.rotuloSTR;
	   	nserie=condic.nserie;
	   	val=new double [nserie];
	   	tempo=new double [nserie];
	   	for(int iCC=0;iCC<nserie;iCC++){
	   		val[iCC]=condic.val[iCC];
	   		tempo[iCC]=condic.tempo[iCC];
	   	}
	}
	~detDiriPoisson(){
	   	if(nserie>0){
	    	delete [] val;
	    	delete [] tempo;
	   	}
	}
	detDiriPoisson& operator=(const detDiriPoisson& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
			   	rotuloSTR=condic.rotuloSTR;
			   	nserie=condic.nserie;
			   	val=new double [nserie];
			   	tempo=new double [nserie];
			   	for(int iCC=0;iCC<nserie;iCC++){
			   		val[iCC]=condic.val[iCC];
			   		tempo[iCC]=condic.tempo[iCC];
			   	}
		 }
		 return *this;
	 }
};
struct detVNPoisson{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* tempo;
	int nserie;
	detVNPoisson(){
		rotulo=0;
		rotuloSTR="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	detVNPoisson(const detVNPoisson& condic){
	   	rotulo=condic.rotulo;
	   	rotuloSTR=condic.rotuloSTR;
	   	nserie=condic.nserie;
	   	val=new double [nserie];
	   	tempo=new double [nserie];
	   	for(int iCC=0;iCC<nserie;iCC++){
	   		val[iCC]=condic.val[iCC];
	   		tempo[iCC]=condic.tempo[iCC];
	   	}
	}
	~detVNPoisson(){
		if(nserie>0){
		   delete [] val;
		   delete [] tempo;
		}
	}
	detVNPoisson& operator=(const detVNPoisson& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
			   	rotuloSTR=condic.rotuloSTR;
			   	nserie=condic.nserie;
			   	val=new double [nserie];
			   	tempo=new double [nserie];
			   	for(int iCC=0;iCC<nserie;iCC++){
			   		val[iCC]=condic.val[iCC];
			   		tempo[iCC]=condic.tempo[iCC];
			   	}
		 }
		 return *this;
	 }
};
struct detRicPoisson{
	int rotulo;
	string rotuloSTR;
	double* valAmb;
	double* hAmb;
	double* tempo;
	int nserie;
	detRicPoisson(){
		rotulo=0;
		rotuloSTR="vazio";
		valAmb=0;
		hAmb=0;
		tempo=0;
		nserie=0;
	}
	detRicPoisson(const detRicPoisson& condic){
	   	rotulo=condic.rotulo;
	   	rotuloSTR=condic.rotuloSTR;
	   	nserie=condic.nserie;
	   	valAmb=new double [nserie];
	   	hAmb=new double [nserie];
	   	tempo=new double [nserie];
	   	for(int iCC=0;iCC<nserie;iCC++){
	   		valAmb[iCC]=condic.valAmb[iCC];
	   		hAmb[iCC]=condic.hAmb[iCC];
	   		tempo[iCC]=condic.tempo[iCC];
	   	}
	}
	~detRicPoisson(){
	   	if(nserie>0){
	    	delete [] valAmb;
	    	delete [] hAmb;
	    	delete [] tempo;
	   	}
	}
	detRicPoisson& operator=(const detRicPoisson& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] valAmb;
			    	delete [] hAmb;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
			   	rotuloSTR=condic.rotuloSTR;
			   	nserie=condic.nserie;
			   	valAmb=new double [nserie];
			   	hAmb=new double [nserie];
			   	tempo=new double [nserie];
			   	for(int iCC=0;iCC<nserie;iCC++){
			   		valAmb[iCC]=condic.valAmb[iCC];
			   		hAmb[iCC]=condic.hAmb[iCC];
			   		tempo[iCC]=condic.tempo[iCC];
			   	}
		 }
		 return *this;
	 }
};
struct detCCPoisson{
	int nDiri;
	int nRic;
	int nVN;
	int rotuloAcop;
	detDiriPoisson* ccDir;
	detRicPoisson* ccRic;
	detVNPoisson* ccVN;
	detCCPoisson(){
		rotuloAcop=0;
		nDiri=0;
		nRic=0;
		nVN=0;
		ccDir=0;
		ccRic=0;
		ccVN=0;
	}
	detCCPoisson(const detCCPoisson& condic){
	   	rotuloAcop=condic.rotuloAcop;
	   	nDiri=condic.nDiri;
	   	nRic=condic.nRic;
	   	nVN=condic.nVN;
	   	ccDir=new detDiriPoisson [nDiri];
	   	ccRic=new detRicPoisson [nRic];
	   	ccVN=new detVNPoisson [nVN];
	   	for(int iCC=0;iCC<nDiri;iCC++)ccDir[iCC]=condic.ccDir[iCC];
	   	for(int iCC=0;iCC<nRic;iCC++)ccRic[iCC]=condic.ccRic[iCC];
	   	for(int iCC=0;iCC<nVN;iCC++)ccVN[iCC]=condic.ccVN[iCC];
	}
	~detCCPoisson(){
	   	if(nDiri>0)delete [] ccDir;
	   	if(nRic>0)delete [] ccRic;
	   	if(nVN>0)delete [] ccVN;
	 }
	detCCPoisson& operator=(const detCCPoisson& condic){
		 if(this != &condic){
			   	if(nDiri>0)delete [] ccDir;
			   	if(nRic>0)delete [] ccRic;
			   	if(nVN>0)delete [] ccVN;

			   	rotuloAcop=condic.rotuloAcop;
			   	nDiri=condic.nDiri;
			   	nRic=condic.nRic;
			   	nVN=condic.nVN;
			   	ccDir=new detDiriPoisson [nDiri];
			   	ccRic=new detRicPoisson [nRic];
			   	ccVN=new detVNPoisson [nVN];
			   	for(int iCC=0;iCC<nDiri;iCC++)ccDir[iCC]=condic.ccDir[iCC];
			   	for(int iCC=0;iCC<nRic;iCC++)ccRic[iCC]=condic.ccRic[iCC];
			   	for(int iCC=0;iCC<nVN;iCC++)ccVN[iCC]=condic.ccVN[iCC];
		 }
		 return *this;
	 }
};

//struct relacionada com o detalhamento do material na parede da tubulacao;
struct materialP {
    int id;
    double cond;//condutividade, W/mK;
    double cp;//calor especifico J/kgK;
    double rho;//massa especifica kg/m3;
    int tipo;//tipo de material, 0-> solido, 1-> fluido (usuario), 2-> agua, 3->ar;
    double visc;//viscosidade, cP, apenas para tipo 1;
    double beta;//expansividade termica, apenas para tipo 1;
};

//struct com o detalhamento da secao transversal do duto;
struct cortedutoP {
    int id;
    int ncam;//numero de camadas de material;
    int anul;//se regiao de escoamento e ou nao anular
    double a;//diametro interno, se anular, menor diametro do anular
    double b;//para o caso anular, maior diametro do anular
    double rug;//rugosidade absoluta, metros;
    double* diam;//vetor com o maior diemtro de cada camada de material;
    int* indmat;//vetor com o indicador do material, relaciona-se com a struct material
    int* discre;//discretizacao de cada camada de material
};

struct detMalhaHomPoisson{
	double xcoor;
	double ycoor;
	double delx;
	double dely;
	int indEle;
	int buraco;
};



#endif /* ESTRUTURASPOISSON_H_ */
