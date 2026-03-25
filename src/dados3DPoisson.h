/*
 * dados1Poisson.h
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */

#ifndef DADOS3DPOISSON_H_
#define DADOS3DPOISSON_H_

#include <math.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <unordered_map>
using namespace std;
//#include <ctime.h>
//#include <ctime.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Log.h"
#include "Vetor.h"
//#include "Vetor.cpp"
#include "Matriz.h"
//#include "Matriz.cpp"
#include "Elem3DPoisson.h"
#include "Malha3DPoisson.h"
#include "estruturasPoisson3D.h"
#include "Geometria.h"
//#include "TrocaCalor.h"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "estruturaUNV.h"

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


class dadosP3D{
	public:

		detTempoPoisson3D temp;
		detPropPoisson3D prop;
		//detCIPoisson3D CI;
		detCCPoisson3D CC;
		//materialPoisson3D* mat;//vetor com os materiais cadastrados no Json
		//cortedutoPoisson3D* corte;//vetor com as secoes transversais cadastradas no json
		//ProFluCol fluc;
		//ProFlu flui;
		//DadosGeo* dutosMRT;
		//double* comp;
		//TransCal* transfer;

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

		//vector<cordaNo> cordas;
		//vector<faceNo> faces;
		//vector<eleNo> elemento;
		//cordaOBJ* cordaO;
		//faceOBJ* faceO;
		eleOBJ* elementoO;

		double tempo;
		int nmaterial;
		int acop;
		double angAcop;
		double* tParede;
		double* tInt;
		//double pInt;
		//double tAmb;
		//double vAmb;
		//double velIni;
		//int amb;

		double* hE;
		double* hI;
		//double* condGlob;
		//double* condLoc;
		//double* qDesacop;
		double* qAcop;
		double* qTotal;
		double* tInt0;
		double dimExt;
		double* diamRef;
		//int* soConvInt;

		int rank;
		int colorido;
		int solverMat;

		string elearq;
		string noarq;
		string polyarq;
		string entrada;

		dadosP3D(string nomeArquivoEntrada);
		dadosP3D();
		dadosP3D(const dadosP3D&);
		dadosP3D& operator=(const dadosP3D&);
		~dadosP3D(){
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
			if(nele>0)delete[] vecsra;
			if(nele>0)delete[] veclm;
			if(nele>0)delete[] vecfnz;
			//if(acop==1 && CC.nAcop>0){
				//delete[] mat;
				//for(int i=0; i<CC.nAcop; i++){
					//delete[] corte[i].diam;
					//delete[] corte[i].indmat;
					//delete[] corte[i].discre;
				//}
				//delete[] corte;
				//delete[] transfer;
				//if(comp!=0)delete[] comp;
				//if(dutosMRT!=0)delete[] dutosMRT;
			//}
		    //if(cordaO!=0)delete [] cordaO;
		    //if(faceO!=0)delete [] faceO;
		    if(elementoO!=0)delete [] elementoO;
		    if(acop==1 && CC.nAcop>0){
		    	if(tParede!=0)delete[] tParede;
		    	if(tInt!=0)delete[] tInt;
		    	if(hE!=0)delete[] hE;
		    	if(hI!=0)delete[] hI;
		    	//delete[] condGlob;
		    	//delete[] condLoc;
		    	//delete[] qDesacop;
		    	if(qAcop!=0)delete[] qAcop;
		    	if(qTotal!=0)delete[] qTotal;
		    	if(tInt0!=0)delete[] tInt0;
		    	if(diamRef!=0)delete[] diamRef;
		    }
		}
		void iniciaVariaveis();
		//void parse_materiais(Value& material_json);
		void parse_corte(Value& corte_json);
		//void parse_fluido_complementar(Value& fluido_json);
		void parse_configIni(Value& configuracaoInicial_json);
		void parse_Prop(Value& prop_json);
		void parse_CI(Value& CI_json);
		void parse_CC(Value& CC_json);
		void parse_malha(Value& malha_json);
		void lerPoisson(string nomeArquivoEntrada);
		void lerPoisson2(string nomeArquivoEntrada,const dadosP3D& aux);

};



#endif /* DADOS3DPOISSON_H_ */
