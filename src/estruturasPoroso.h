/*
 * estruturas.h
 *
 *  Created on: 28 de jul. de 2022
 *      Author: eduardo
 */

#ifndef ESTRUTURASPOROSO_H_
#define ESTRUTURASPOROSO_H_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct detTempoPoroso{
	int perm;
	int trans;
	double tmax;
	double relax;
	int parserie;
	double* dtmax;
	double* tempo;

	double* tempoImp;
	int parserieImp;
	detTempoPoroso(){
		perm=0;
		trans=0;
		tmax=0;
		relax=0;
		dtmax=0;
	    tempo=0;
	    parserie=0;
	    tempoImp=0;
	   	parserieImp=0;

	 }
	~detTempoPoroso(){
	   	if(parserie>0){
	    	delete [] dtmax;
	    	delete [] tempo;
	    	parserie=0;
	   	}
	   	if(parserieImp>0){
	   		delete [] tempoImp;
	   		parserieImp=0;
	   	}
	 }
};
struct detPropPoroso{
	int parserie;
	double* xmax;
	double* xmin;
	double* ymax;
	double* ymin;
	/*
	 	double kY;
    	double kX;
    	double poro;
    	double compRoc;
	 */

	double* kY;
	double* kX;
	double* poro;
	double* compRoc;
	detPropPoroso(){
	   	parserie=0;
	    xmax=0;
	    xmin=0;
	    ymax=0;
	    ymin=0;
	    kY=0;
	    kX=0;
	    poro=0;
	    compRoc=0;
	 }
	~detPropPoroso(){
	   	if(parserie>0){
	    	delete [] xmax;
	    	delete [] xmin;
	    	delete [] ymax;
	    	delete [] ymin;
	    	delete [] kY;
	    	delete [] kX;
	    	delete [] poro;
	    	delete [] compRoc;
	    	parserie=0;
	   	}
	 }
};
struct detCIPoroso{
	int parserie;
	double* xmax;
	double* xmin;
	double* ymax;
	double* ymin;
	double* Dxmax;
	double* Dxmin;
	double* Dymax;
	double* Dymin;
	double* val1;
	double* val2;
	detCIPoroso(){
			parserie=0;
		    xmax=0;
		    xmin=0;
		    ymax=0;
		    ymin=0;
		    Dxmax=0;
		    Dxmin=0;
		    Dymax=0;
		    Dymin=0;
		    val1=0;
		    val2=0;
		 }
	~detCIPoroso(){
	   	if(parserie>0){
	    	delete [] xmax;
	    	delete [] xmin;
	    	delete [] ymax;
	    	delete [] ymin;
	    	delete [] Dxmax;
	    	delete [] Dxmin;
	    	delete [] Dymax;
	    	delete [] Dymin;
	    	delete [] val1;
	    	delete [] val2;
	   	}
	 }
};
struct detDiriPoroso{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* valSat;
	double* tempo;
	int nserie;
	detDiriPoroso(){
		rotulo=0;
		rotuloSTR="vazio";
		val=0;
		valSat=0;
		tempo=0;
		nserie=0;
	}
	~detDiriPoroso(){
	   	if(nserie>0){
	    	delete [] val;
	    	delete [] valSat;
	    	delete [] tempo;
	    	nserie=0;
	   	}
	}
	detDiriPoroso& operator=(const detDiriPoroso& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] valSat;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
			   	rotuloSTR=condic.rotuloSTR;
			   	nserie=condic.nserie;
			   	val=new double [nserie];
			   	valSat=new double [nserie];
			   	tempo=new double [nserie];
			   	for(int iCC=0;iCC<nserie;iCC++){
			   		val[iCC]=condic.val[iCC];
			   		valSat[iCC]=condic.valSat[iCC];
			   		tempo[iCC]=condic.tempo[iCC];
			   	}
		 }
		 return *this;
	 }
};
struct detVNPoroso{
	int rotulo;
	string rotuloSTR;
	double* val;
	double* valSat;
	double* tempo;
	int nserie;
	detVNPoroso(){
		rotulo=0;
		rotuloSTR="vazio";
		val=0;
		valSat=0;
		tempo=0;
		nserie=0;
	}
	~detVNPoroso(){
		if(nserie>0){
		   delete [] val;
		   delete [] valSat;
		   delete [] tempo;
		   nserie=0;
		}
	}
	detVNPoroso& operator=(const detVNPoroso& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] val;
			    	delete [] valSat;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
			   	rotuloSTR=condic.rotuloSTR;
			   	nserie=condic.nserie;
			   	val=new double [nserie];
			   	valSat=new double [nserie];
			   	tempo=new double [nserie];
			   	for(int iCC=0;iCC<nserie;iCC++){
			   		val[iCC]=condic.val[iCC];
			   		valSat[iCC]=condic.valSat[iCC];
			   		tempo[iCC]=condic.tempo[iCC];
			   	}
		 }
		 return *this;
	 }
};
struct detRicPoroso{
	int rotulo;
	string rotuloSTR;
	double* valAmb;
	double* hAmb;
	double* valSat;
	double* tempo;
	int nserie;
	detRicPoroso(){
		rotulo=0;
		rotuloSTR="vazio";
		valAmb=0;
		hAmb=0;
		valSat=0;
		tempo=0;
		nserie=0;
	}
	~detRicPoroso(){
	   	if(nserie>0){
	    	delete [] valAmb;
	    	delete [] hAmb;
	    	delete [] valSat;
	    	delete [] tempo;
	    	nserie=0;
	   	}
	}
	detRicPoroso& operator=(const detRicPoroso& condic){
		 if(this != &condic){
			   	if(nserie>0){
			    	delete [] valAmb;
			    	delete [] hAmb;
			    	delete [] valSat;
			    	delete [] tempo;
			   	}
			   	rotulo=condic.rotulo;
			   	rotuloSTR=condic.rotuloSTR;
			   	nserie=condic.nserie;
			   	valAmb=new double [nserie];
			   	hAmb=new double [nserie];
			   	valSat=new double [nserie];
			   	tempo=new double [nserie];
			   	for(int iCC=0;iCC<nserie;iCC++){
			   		valAmb[iCC]=condic.valAmb[iCC];
			   		hAmb[iCC]=condic.hAmb[iCC];
			   		valSat[iCC]=condic.valSat[iCC];
			   		tempo[iCC]=condic.tempo[iCC];
			   	}
		 }
		 return *this;
	 }
};
struct detCCPoroso{
	int nDiri;
	int nRic;
	int nVN;
	int rotuloAcop;
	double satAcop;
	detDiriPoroso* ccDir;
	detRicPoroso* ccRic;
	detVNPoroso* ccVN;
	detCCPoroso(){
		rotuloAcop=0;
		nDiri=0;
		nRic=0;
		nVN=0;
		ccDir=0;
		ccRic=0;
		ccVN=0;
		satAcop=0.;
	}
	~detCCPoroso(){
	   	if(nDiri>0)delete [] ccDir;
	   	if(nRic>0)delete [] ccRic;
	   	if(nVN>0)delete [] ccVN;
	   	nDiri=0;
	   	nRic=0;
	   	nVN=0;
	 }
	detCCPoroso& operator=(const detCCPoroso& condic){
		 if(this != &condic){
			   	if(nDiri>0)delete [] ccDir;
			   	if(nRic>0)delete [] ccRic;
			   	if(nVN>0)delete [] ccVN;

			   	rotuloAcop=condic.rotuloAcop;
			   	satAcop=condic.satAcop;
			   	nDiri=condic.nDiri;
			   	nRic=condic.nRic;
			   	nVN=condic.nVN;
			   	ccDir=new detDiriPoroso [nDiri];
			   	ccRic=new detRicPoroso [nRic];
			   	ccVN=new detVNPoroso [nVN];
			   	for(int iCC=0;iCC<nDiri;iCC++)ccDir[iCC]=condic.ccDir[iCC];
			   	for(int iCC=0;iCC<nRic;iCC++)ccRic[iCC]=condic.ccRic[iCC];
			   	for(int iCC=0;iCC<nVN;iCC++)ccVN[iCC]=condic.ccVN[iCC];
		 }
		 return *this;
	 }
};

//struct relacionada com o detalhamento do material na parede da tubulacao;
struct materialPoroso {
    int id;
    double kY;
    double kX;
    double poro;
    double compRoc;
};

//struct com o detalhamento da secao transversal do duto;
struct cortePoroso {
    int id;
    int ncam;//numero de camadas de material;
    double a;//diametro interno, se anular, menor diametro do anular
    double b;//para o caso anular, maior diametro do anular
    double* diam;//vetor com o maior diemtro de cada camada de material;
    int* indmat;//vetor com o indicador do material, relaciona-se com a struct material
    int* discre;//discretizacao de cada camada de material
    double zDatum0;
    double zDatum1;
    cortePoroso(){
        id=0;
        ncam=0;//numero de camadas de material;
        a=0;//diametro interno, se anular, menor diametro do anular
        b=0;//para o caso anular, maior diametro do anular
        diam=0;//vetor com o maior diemtro de cada camada de material;
        indmat=0;//vetor com o indicador do material, relaciona-se com a struct material
        discre=0;//discretizacao de cada camada de material
        zDatum0=0;
        zDatum1=0;
    }
};

struct detMalhaHomPoroso{
	double xcoor;
	double ycoor;
	double delx;
	double dely;
	int indEle;
	int buraco;
};

//struct com as propriedade do fluido coletadas da tabela PVTSim
struct tabelaFlashPoroso{
    int id;
    int TwoOrThree;//indicador se a tabela e bifpasica ou trifasica
	string arquivo;//nome do arquivo PVTSim
	//double compini;
	//double compfim;
	//int celfim;
	//int celini;
	int visc;//indicador se se deseja usar a viscosidade da tabela PVTsim ou usar um modelo black-oil para o calciulo
	//das viscosidades, 0->tabela, 1->black oil
	//conjunto de tabelas obtidas diretamente do aqruivo PVTSim
    double **rholF;
    double **rhogF;
    double **DrholDpF;
    double **DrhogDpF;
    double **DrholDtF;
    double **DrhogDtF;
    double **DrhowDtF;
    double **RSF;
    double **cplF;
    double **cpgF;
    double **cpwF;
    double **HlF;
    double **HgF;
    double **sigOGF;
    double **sigWGF;
    double **viscO;//alteracao6
    double **viscG;//alteracao6
    double *PBF;
    double *TBF;
};

struct composicionalPoroso{
	string arquivo;
	int npseudo=0;
	int liqModel=0;
	int vapModel=0;
    double *fracMol=0;
    double *masMol=0;
    double *tempCrit=0;
    double *presCrit=0;
    double *fatAcent=0;
    double *parac=0;
    double *TIndepPeneloux=0;
    double *kij=0;
    double *lij=0;
    double *CpIGCoefs=0;
    ~composicionalPoroso(){
    	if(npseudo>0){
    	delete fracMol;
    	delete CpIGCoefs;
    	delete TIndepPeneloux;
    	delete fatAcent;
    	delete kij;
    	delete lij;
    	delete masMol;
    	delete parac;
    	delete presCrit;
    	delete tempCrit;
    	}
    }
};

struct detTabViscPoroso{
	int parserie;
	double* visc;
	double* temp;
};

//struct com os detalhes de tabela para montagem de tabelas de propriedade dos fluidos
struct detTabelaEntalpPoroso{
	int npont;//npumero de pontos da tabela
	double pmax;//poressao maxima kgf/cm2
	double pmin;//pressao minima kgf/cm2
	double tmax;//temperatura maximo C
	double tmin;//temperatura minima C
};

class tabelaPemRelOG{
public:
	int npont;
	double* satG;
	double* permRelG;
	double* permRelOG;
	tabelaPemRelOG(){
		npont=0;
		satG=0;
		permRelG=0;
		permRelOG=0;
	}
	tabelaPemRelOG(const tabelaPemRelOG& tabela){
	   	npont=tabela.npont;
	   	if(npont>0){
	   		satG=new double [npont];
	   		permRelG=new double [npont];
	   		permRelOG=new double [npont];
	   		for(int itab=0;itab<npont;itab++)satG[itab]=tabela.satG[itab];
	   		for(int itab=0;itab<npont;itab++)permRelG[itab]=tabela.permRelG[itab];
	   		for(int itab=0;itab<npont;itab++)permRelOG[itab]=tabela.permRelOG[itab];
	   	}
	   	else{
			satG=0;
			permRelG=0;
			permRelOG=0;
	   	}
	}
	~tabelaPemRelOG(){
		if(npont>0){
		    delete [] satG;
		    delete [] permRelG;
		    delete [] permRelOG;
		    npont=0;
		}
	}

	tabelaPemRelOG& operator=(const tabelaPemRelOG& tabela){
		 if(this != &tabela){
			   	if(npont>0){
			    	delete [] satG;
			    	delete [] permRelG;
			    	delete [] permRelOG;
			    	npont=0;
			   	}

			   	npont=tabela.npont;
			   	if(npont>0){
			   		satG=new double [npont];
			   		permRelG=new double [npont];
			   		permRelOG=new double [npont];
			   		for(int itab=0;itab<npont;itab++)satG[itab]=tabela.satG[itab];
			   		for(int itab=0;itab<npont;itab++)permRelG[itab]=tabela.permRelG[itab];
			   		for(int itab=0;itab<npont;itab++)permRelOG[itab]=tabela.permRelOG[itab];
			   	}
			   	else{
					satG=0;
					permRelG=0;
					permRelOG=0;
					npont=0;
			   	}
		 }
		 return *this;
	 }
};

class tabelaPemRelOA{
public:
	int npont;
	double* satW;
	double* permRelW;
	double* permRelOW;
	tabelaPemRelOA(){
		npont=0;
		satW=0;
		permRelW=0;
		permRelOW=0;
	}
	tabelaPemRelOA(const tabelaPemRelOA& tabela){
			   	npont=tabela.npont;
			   	if(npont>0){
			   		satW=new double [npont];
			   		permRelW=new double [npont];
			   		permRelOW=new double [npont];
			   		for(int itab=0;itab<npont;itab++)satW[itab]=tabela.satW[itab];
			   		for(int itab=0;itab<npont;itab++)permRelW[itab]=tabela.permRelW[itab];
			   		for(int itab=0;itab<npont;itab++)permRelOW[itab]=tabela.permRelOW[itab];
			   	}
			   	else{
					satW=0;
					permRelW=0;
					permRelOW=0;
			   	}
	}
	~tabelaPemRelOA(){
	   	if(npont>0){
	    	delete [] satW;
	    	delete [] permRelW;
	    	delete [] permRelOW;
	    	npont=0;
	   	}
	}

	tabelaPemRelOA& operator=(const tabelaPemRelOA& tabela){
		 if(this != &tabela){
			   	if(npont>0){
			    	delete [] satW;
			    	delete [] permRelW;
			    	delete [] permRelOW;
			    	npont=0;
			   	}

			   	npont=tabela.npont;
			   	if(npont>0){
			   		satW=new double [npont];
			   		permRelW=new double [npont];
			   		permRelOW=new double [npont];
			   		for(int itab=0;itab<npont;itab++)satW[itab]=tabela.satW[itab];
			   		for(int itab=0;itab<npont;itab++)permRelW[itab]=tabela.permRelW[itab];
			   		for(int itab=0;itab<npont;itab++)permRelOW[itab]=tabela.permRelOW[itab];
			   	}
			   	else{
					satW=0;
					permRelW=0;
					permRelOW=0;
			   	}
		 }
		 return *this;
	 }
};

class tabelaPresCapOA{
public:
	int npont;
	double* satW;
	double* presCapOW;
	tabelaPresCapOA(){
		npont=0;
		satW=0;
		presCapOW=0;
	}
	tabelaPresCapOA(const tabelaPresCapOA& tabela){
	   	npont=tabela.npont;
	   	if(npont>0){
	   		satW=new double [npont];
	   		presCapOW=new double [npont];
	   		for(int itab=0;itab<npont;itab++)satW[itab]=tabela.satW[itab];
	   		for(int itab=0;itab<npont;itab++)presCapOW[itab]=tabela.presCapOW[itab];
	   	}
	   	else{
			satW=0;
			presCapOW=0;
	   	}
	}
	~tabelaPresCapOA(){
		if(npont>0){
			delete [] satW;
		    delete [] presCapOW;
		    npont=0;
		 }
	}

	tabelaPresCapOA& operator=(const tabelaPresCapOA& tabela){
		 if(this != &tabela){
			   	if(npont>0){
			    	delete [] satW;
			    	delete [] presCapOW;
			    	npont=0;
			   	}

			   	npont=tabela.npont;
			   	if(npont>0){
			   		satW=new double [npont];
			   		presCapOW=new double [npont];
			   		for(int itab=0;itab<npont;itab++)satW[itab]=tabela.satW[itab];
			   		for(int itab=0;itab<npont;itab++)presCapOW[itab]=tabela.presCapOW[itab];
			   	}
			   	else{
					satW=0;
					presCapOW=0;
					npont=0;
			   	}
		 }
		 return *this;
	 }
};

class tabelaPresCapGO{
public:
	int npont;
	double* satG;
	double* presCapGO;
	tabelaPresCapGO(){
		npont=0;
		satG=0;
		presCapGO=0;
	}
	tabelaPresCapGO(const tabelaPresCapGO& tabela){
	   	npont=tabela.npont;
	   	if(npont>0){
	   		satG=new double [npont];
	   		presCapGO=new double [npont];
	   		for(int itab=0;itab<npont;itab++)satG[itab]=tabela.satG[itab];
	   		for(int itab=0;itab<npont;itab++)presCapGO[itab]=tabela.presCapGO[itab];
	   	}
	   	else{
			satG=0;
			presCapGO=0;
			npont=0;
	   	}
	}
	~tabelaPresCapGO(){
		if(npont>0){
			delete [] satG;
		    delete [] presCapGO;
		    npont=0;
		 }
	}

	tabelaPresCapGO& operator=(const tabelaPresCapGO& tabela){
		 if(this != &tabela){
			   	if(npont>0){
			    	delete [] satG;
			    	delete [] presCapGO;
			    	npont=0;
			   	}

			   	npont=tabela.npont;
			   	if(npont>0){
			   		satG=new double [npont];
			   		presCapGO=new double [npont];
			   		for(int itab=0;itab<npont;itab++)satG[itab]=tabela.satG[itab];
			   		for(int itab=0;itab<npont;itab++)presCapGO[itab]=tabela.presCapGO[itab];
			   	}
			   	else{
					satG=0;
					presCapGO=0;
			   	}
		 }
		 return *this;
	 }
};



#endif /* ESTRUTURASPOROSO_H_ */
