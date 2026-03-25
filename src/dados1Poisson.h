/*
 * dados1Poisson.h
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */

#ifndef DADOS1POISSON_H_
#define DADOS1POISSON_H_
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;
//#include <ctime.h>
//#include <ctime.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "Log.h"
#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "Elem2DPoisson.h"
#include "Malha2DPoisson.h"
#include "estruturasPoisson.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/pt_BR.h"


// criar relatorio dos arquivos de dados de saida da simulacao
extern ofstream arqRelatorioPerfis;

// criar objeto de logger
extern Logger logger;

// criar string do path do arquivo de entrada
extern string pathArqEntrada;

// criar string do path dos arquivos de entrada PVTSIM e SnapShot
extern string pathArqExtEntrada;

// criar string do path e prefixo dos arquivos de saida para POCO_INJETOR
extern string pathPrefixoArqSaida;

//diretorio contendo os arquivos de saida
extern string diretorioSaida;


class dadosP{
	public:

		detTempoPoisson temp;
		detPropPoisson prop;
		detCIPoisson CI;
		detCCPoisson CC;

		int nno;//vari�vel inteira indicando o n�mero de n�s da malha
		int nele;
		int noZero;
		int** noEle;
		double* atributo;
		int* tipo;
		double* vecsra;
		int* veclm;
		int* vecfnz;
		//double* vecsra;
		//int* veclm;
		//int* vecfnz;
		double** xcoor;

		double tempo;
		int acop;
		double angAcop;
		double tParede;
		double tInt;
		double tAmb;

		double hE;
		double hI;
		double condGlob;
		double condLoc;
		double qDesacop;
		double qAcop;
		double qTotal;
		double tInt0=tInt;
		double dimExt;

		double diamInt;
		double diamExt;

		//double centroX;
		//double centroY;
		//int nxMH;
		//int nyMH;
		double xmin;
		double xmax;
		double ymin;
		double ymax;
		//double dxH;
		//double dyH;

		int unv;
		string elearq;
		string noarq;
		string polyarq;
		string entrada;

		int nthrdLocal;

		int rankLU;
		int colore;
		int solverMat;

		dadosP(string nomeArquivoEntrada, double vCondGlob=0.,double vCondLoc=0., double vhE=0., double vhInt=0.,
				double vTint=0., double vTamb=0.,double vdiamI=0., double vdiamE=0.);
		dadosP();
		dadosP(const dadosP&);
		dadosP& operator=(const dadosP&);
		~dadosP(){
			if(nele>0){
				for(int i=0;i<nele;i++) delete[] noEle[i];
				delete[] noEle;
			}
			if(nno>0){
				for(int i=0;i<nno;i++) delete[] xcoor[i];
				delete[] xcoor;
		    	delete[] atributo;
		    	delete[] tipo;
			}
			if(noZero>0)delete[] vecsra;
			if(noZero>0)delete[] veclm;
			if(noZero>0)delete[] vecfnz;
		}
		void iniciaVariaveis();
		void parse_configIni(Value& configuracaoInicial_json);
		void parse_Prop(Value& prop_json);
		void parse_CI(Value& CI_json);
		void parse_CC(Value& CC_json);
		void parse_malha(Value& malha_json);
		void lerPoisson(string nomeArquivoEntrada, double vCondGlob=0.,double vCondLoc=0., double vhE=0., double vhInt=0.,
				double vTint=0., double vTamb=0.,double vdiamI=0., double vdiamE=0.);

};



#endif /* DADOS1POISSON_H_ */
