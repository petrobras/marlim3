/*
 * estruturas.h
 *
 *  Created on: 28 de jul. de 2022
 *      Author: eduardo
 */

#ifndef ESTRUTURASPOISSON3D_H_
#define ESTRUTURASPOISSON3D_H_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct detTempoPoisson3D{
	//int perm;
	//int trans;
	//double tmax;
	double relax;
	int parserie;
	double* dtmax;
	double* tempo;

	double* tempoImp;
	int parserieImp;
	detTempoPoisson3D(){
		relax=1;
		parserie=0;
		dtmax=0;
		tempo=0;
		tempoImp=0;
		parserieImp=0;
	}
	~detTempoPoisson3D(){
	   	if(parserie>0){
	    	delete [] dtmax;
	    	delete [] tempo;
	   	}
	   	if(parserieImp>0) delete [] tempoImp;
	 }
};
struct detPropPoisson3D{
	int parserie;
	string* regiao;
	double* rho;
	double* cp;
	double* cond;
	detPropPoisson3D(){
		parserie=0;
		regiao=0;
		rho=0;
		cp=0;
		cond=0;
	}
	~detPropPoisson3D(){
	   	if(parserie>0){
	   		delete [] regiao;
	    	delete [] rho;
	    	delete [] cp;
	    	delete [] cond;
	   	}
	 }
};
struct detCIPoisson3D{
	int parserie;
	string* regiao;
	double* temp;
	~detCIPoisson3D(){
	   	if(parserie>0){
	   		delete [] regiao;
	    	delete [] temp;
	   	}
	 }
};
struct detDiriPoisson3D{
	string rotulo;
	double* val;
	double* tempo;
	int nserie;
	detDiriPoisson3D(){
		rotulo="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	~detDiriPoisson3D(){
	   	if(nserie>0){
	    	delete [] val;
	    	delete [] tempo;
	   	}
	}
	detDiriPoisson3D& operator=(const detDiriPoisson3D& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
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
	detDiriPoisson3D& operator=(detDiriPoisson3D&& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] tempo;
			   	}
			   	//rotulo=condic.rotulo;
			   	rotulo=std::move(condic.rotulo);
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
struct detVNPoisson3D{
	string rotulo;
	double* val;
	double* tempo;
	int nserie;
	detVNPoisson3D(){
		rotulo="vazio";
		val=0;
		tempo=0;
		nserie=0;
	}
	~detVNPoisson3D(){
		if(nserie>0){
		   delete [] val;
		   delete [] tempo;
		}
	}
	detVNPoisson3D& operator=(const detVNPoisson3D& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
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

	detVNPoisson3D& operator=(detVNPoisson3D&& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] tempo;
			   	}
			   	//rotulo=condic.rotulo;
			   	rotulo = std::move(condic.rotulo);
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
struct detRicPoisson3D{
	string rotulo;
	double* valAmb;
	double* hAmb;
	double* tempo;
	int nserie;
	detRicPoisson3D(){
		rotulo="vazio";
		valAmb=0;
		hAmb=0;
		tempo=0;
		nserie=0;
	}
	~detRicPoisson3D(){
	   	if(nserie>0){
	    	delete [] valAmb;
	    	delete [] hAmb;
	    	delete [] tempo;
	   	}
	}
	detRicPoisson3D& operator=(const detRicPoisson3D& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] valAmb;
			    	delete [] hAmb;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
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
	detRicPoisson3D& operator=(detRicPoisson3D&& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] valAmb;
			    	delete [] hAmb;
			    	delete [] tempo;
			   	}
			   	//rotulo=condic.rotulo;
			   	rotulo = std::move(condic.rotulo);
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

struct detMudaRegiao3D{
	string rotulo;
	detMudaRegiao3D(){
		rotulo="vazio";
	}
	detMudaRegiao3D& operator=(const detMudaRegiao3D& condic){
		 if(this != &condic){
			   	rotulo=condic.rotulo;
		 }
		 return *this;
	 }
	detMudaRegiao3D& operator=(detMudaRegiao3D&& condic) noexcept {
			if (this != &condic) {
				rotulo = std::move(condic.rotulo);
			}
			return *this;
	}
};
struct detCCPoisson3D{
	int nDiri;
	int nRic;
	int nVN;
	int nMuda;
	int nAcop;
	string* rotuloAcop;
	detDiriPoisson3D* ccDir;
	detRicPoisson3D* ccRic;
	detVNPoisson3D* ccVN;
	detMudaRegiao3D* mudaRegiao;
	detCCPoisson3D(){
		nAcop=0;
		rotuloAcop=0;
		nDiri=0;
		nRic=0;
		nVN=0;
		nMuda=0;
		ccDir=0;
		ccRic=0;
		ccVN=0;
		mudaRegiao=0;
	}
	~detCCPoisson3D(){
		if(nAcop>0)delete [] rotuloAcop;
	   	if(nDiri>0)delete [] ccDir;
	   	if(nRic>0)delete [] ccRic;
	   	if(nVN>0)delete [] ccVN;
	   	if(nMuda>0)delete [] mudaRegiao;
	 }
	detCCPoisson3D& operator=(const detCCPoisson3D& condic){
		 if(this != &condic){
			 	if(nAcop>0)delete [] rotuloAcop;
			   	if(nDiri>0)delete [] ccDir;
			   	if(nRic>0)delete [] ccRic;
			   	if(nVN>0)delete [] ccVN;
			   	if(nMuda>0)delete [] mudaRegiao;

			   	nAcop=condic.nAcop;
			   	nDiri=condic.nDiri;
			   	nRic=condic.nRic;
			   	nVN=condic.nVN;
			   	nMuda=condic.nMuda;
			   	rotuloAcop=new string [nAcop];
			   	ccDir=new detDiriPoisson3D [nDiri];
			   	ccRic=new detRicPoisson3D [nRic];
			   	ccVN=new detVNPoisson3D [nVN];
			   	mudaRegiao=new detMudaRegiao3D [nMuda];
			   	for(int iCC=0;iCC<nAcop;iCC++)rotuloAcop[iCC]=condic.rotuloAcop[iCC];
			   	for(int iCC=0;iCC<nDiri;iCC++)ccDir[iCC]=condic.ccDir[iCC];
			   	for(int iCC=0;iCC<nRic;iCC++)ccRic[iCC]=condic.ccRic[iCC];
			   	for(int iCC=0;iCC<nVN;iCC++)ccVN[iCC]=condic.ccVN[iCC];
				for(int iCC=0;iCC<nMuda;iCC++)mudaRegiao[iCC]=condic.mudaRegiao[iCC];
		 }
		 return *this;
	 }

	detCCPoisson3D& operator=(detCCPoisson3D&& condic){
		 if(this != &condic){
			 	if(nAcop>0)delete [] rotuloAcop;
			   	if(nDiri>0)delete [] ccDir;
			   	if(nRic>0)delete [] ccRic;
			   	if(nVN>0)delete [] ccVN;
			   	if(nMuda>0)delete [] mudaRegiao;

			   	nAcop=condic.nAcop;
			   	nDiri=condic.nDiri;
			   	nRic=condic.nRic;
			   	nVN=condic.nVN;
			   	nMuda=condic.nMuda;
			   	rotuloAcop=new string [nAcop];
			   	ccDir=new detDiriPoisson3D [nDiri];
			   	ccRic=new detRicPoisson3D [nRic];
			   	ccVN=new detVNPoisson3D [nVN];
			   	mudaRegiao=new detMudaRegiao3D [nMuda];
			   	//for(int iCC=0;iCC<nAcop;iCC++)rotuloAcop[iCC]=condic.rotuloAcop[iCC];
			   	for(int iCC=0;iCC<nAcop;iCC++)rotuloAcop[iCC]=std::move(condic.rotuloAcop[iCC]);
			   	for(int iCC=0;iCC<nDiri;iCC++)ccDir[iCC]=condic.ccDir[iCC];
			   	for(int iCC=0;iCC<nRic;iCC++)ccRic[iCC]=condic.ccRic[iCC];
			   	for(int iCC=0;iCC<nVN;iCC++)ccVN[iCC]=condic.ccVN[iCC];
				for(int iCC=0;iCC<nMuda;iCC++)mudaRegiao[iCC]=condic.mudaRegiao[iCC];
		 }
		 return *this;
	 }
};

//struct relacionada com o detalhamento do materialPoisson3D na parede da tubulacao;
struct materialPoisson3D {
    int id;
    double cond;//condutividade, W/mK;
    double cp;//calor especifico J/kgK;
    double rho;//massa especifica kg/m3;
    int tipo;//tipo de materialPoisson3D, 0-> solido, 1-> fluido (usuario), 2-> agua, 3->ar;
    double visc;//viscosidade, cP, apenas para tipo 1;
    double beta;//expansividade termica, apenas para tipo 1;
};

//struct com o detalhamento da secao transversal do duto;
struct cortedutoPoisson3D {
    int id;
    int ncam;//numero de camadas de materialPoisson3D;
    int anul;//se regiao de escoamento e ou nao anular
    double a;//diametro interno, se anular, menor diametro do anular
    double b;//para o caso anular, maior diametro do anular
    double rug;//rugosidade absoluta, metros;
    double* diam;//vetor com o maior diemtro de cada camada de materialPoisson3D;
    int* indmat;//vetor com o indicador do materialPoisson3D, relaciona-se com a struct materialPoisson3D
    int* discre;//discretizacao de cada camada de materialPoisson3D
    double comp;
};


#endif /* ESTRUTURASPOISSON3D_H_ */
