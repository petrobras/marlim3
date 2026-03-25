/*
 * dados1Poroso.h
 *
 *  Created on: 31 de ago. de 2023
 *      Author: Eduardo
 */

#ifndef DADOS1POROSO_H_
#define DADOS1POROSO_H_

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
#include "Elem2DPoroso.h"
#include "Malha2DPoroso.h"
#include "estruturasPoroso.h"
#include "estruturaTabDin.h"
#include "GeometriaPoro.h"
#include "celRad.h"
#include "PorosoRad.h"
#include "PropFluCol.h"
#include "PropFlu.h"
#include "MarlimComposicional.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/schema.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/pt_BR.h"

#include "variaveisGlobais1D.h"

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

/*extern int contaExit;

extern int modoTransiente;

extern int simulaTransiente;*/


class dadosPoro{
	public:

		detTempoPoroso temp;
		detPropPoroso prop;
		detCIPoroso CI;
		detDiriPoroso pW;
		detCCPoroso CC;
		materialPoroso* mat;//vetor com os materiais cadastrados no Json
		cortePoroso corte;//vetor com as secoes transversais cadastradas no json
		DadosGeoPoro dutosMRT;
		PorosRad transfer;

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

		double pInt;
		double pRes;
		double zDatumRef;

		double condGlob;
		double condLoc;
		double qTotal;
		double pInt0=pInt;

		double tRes;
		double espessuraRes;

		double tempo;
		int nmaterial;
		int acop;
		double angAcop;
		double pParede;

		double hE;
		double hI;

		double qDesacop;
		double qAcop;
		double qTotalW;
		double qTotalO;
		double qTotalG;

		double centroX;
		double centroY;
		int nxMH;
		int nyMH;
		double xmin;
		double xmax;
		double ymin;
		double ymax;
		double dxH;
		double dyH;

		int unv;
		string elearq;
		string noarq;
		string polyarq;
		string entrada;


		ProFluCol fluc;
		int flashCompleto;//atributo que indica se vai ser usada a tabela de PVTSim ou Black oil 1-> PVTSim
		int tabelaDinamica;
		int usaTabela;//atributo que indica que se est�pa usando uma tabela, seja no caso black oil ou no caso PVTSim
		string pvtsimarq;//arquivo PVTSim
		int tabp;//indica, para o caso black oil, que se montara uma tabela de fator de compresibilidade e suas derivadas
			//antes de se iniciar a simulacao, linha de poroducao
		ProFlu flup;//vetor com os fluidos de producao
		tabelaFlashPoroso flash;//objeto com as tabelas de propriedade dos fluidos obtidos de um aqruivo PVTSim
		int npontos=0;//numero de pontos de qualquer tabela-> para black oil e PVTSim
		double** zdranP;//tabela de fator de compressibilidade, black oil (pressao reduzida X temperatura reduzida)
		double** dzdpP;//tabela com a derivada do Z com a pressao, black oil (pressao reduzida X temperatura reduzida)
		double** dzdtP;//tabela com a derivada do Z com a temperatura, black oil (pressao reduzida X temperatura reduzida)
		int nvecEmul;//para o caso em que o modelo de emulsao e dado pelo usuario por meio de pares BSWXmultiplicadore,
		//nvecEmul tamanho do vetor
		double* BSWVec;//vetor com os valores de emulsao fornecido pelo usuario
		double* emulVec;//vetor com os valores do multiuplicadores de viscosidade fornecido pelo usupario
		int miniTabAtraso;
		double miniTabDp;
		double miniTabDt;
		detTabViscPoroso tabVisc;
		detTabelaEntalpPoroso tabent;//tabela com os limites de pressao e temepratura para a tabela de PVTSim
		detTabelaEntalpPoroso tabentCrit;//tabela com os limites de pressao reduzida e temperatura reduzida para o
		//fator de compressibilidade do gas e suas derivadas
		composicionalPoroso compDet;
		int corrDeng=0;

		tabelaPemRelOA kRelOA;
		tabelaPemRelOG kRelOG;
		tabelaPresCapOA pcOA;
		tabelaPresCapGO pcGO;

		double satAconat;
		double ippoco;
		double* satAPoc;
		double* satLRes;
		double* tempoSatAPoc;
		double* tempoSatLRes;
		int nsatAPoc=0;
		int nsatLRes=0;

		int reinicia=0;

		int tipoModelOleo;
		double compOleo;
		double rhoRef;
		double pBolha;
		double BSW;
		varGlob1D* vg1dSP;
		int nthrdLocal;

		int rankLU;
		int colore;
		int solverMat;

		dadosPoro(varGlob1D* Vvg1dSP,string nomeArquivoEntrada);
		dadosPoro();
		dadosPoro(const dadosPoro&);
		dadosPoro& operator=(const dadosPoro&);
		~dadosPoro(){
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
			if(acop==1){
				if(mat!=0)delete[] mat;
				if(corte.diam!=0)delete[] corte.diam;
				if(corte.indmat!=0)delete[] corte.indmat;
				if(corte.discre!=0)delete[] corte.discre;
			}

		    if(tabp>0){
				  for(int i=0; i<npontos+2;i++) delete[] zdranP[i];
				  delete [] zdranP;
				  for(int i=0; i<npontos+2;i++) delete[] dzdpP[i];
				  delete [] dzdpP;
				  for(int i=0; i<npontos+2;i++) delete[] dzdtP[i];
				  delete [] dzdtP;
		     }

		    if(flashCompleto==1){
			   for(int j=0; j<tabent.npont+1;j++) delete [] flash.rholF[j];
			   delete [] flash.rholF;
			   for(int j=0; j<tabent.npont+1;j++) delete[] flash.rhogF[j];
			   delete [] flash.rhogF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDpF[j];
		       delete [] flash.DrholDpF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDpF[j];
		       delete [] flash.DrhogDpF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrholDtF[j];
		       delete [] flash.DrholDtF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhogDtF[j];
		       delete [] flash.DrhogDtF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.DrhowDtF[j];
		       delete [] flash.DrhowDtF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.RSF[j];
		       delete [] flash.RSF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cplF[j];
		       delete [] flash.cplF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpgF[j];
		       delete [] flash.cpgF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.cpwF[j];
		       delete [] flash.cpwF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HlF[j];
		       delete [] flash.HlF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.HgF[j];
		       delete [] flash.HgF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigOGF[j];
		       delete [] flash.sigOGF;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.sigWGF[j];
		       delete [] flash.sigWGF;
		       for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscO[j];//alteracao6
		       delete [] flash.viscO;
	           for(int j=0; j<tabent.npont+1;j++) delete[] flash.viscG[j];//alteracao6
		       delete [] flash.viscG;

		       delete [] flash.PBF;
		       delete [] flash.TBF;
		   }

		   if(nvecEmul>0){
		    	  delete [] BSWVec;
		    	  delete [] emulVec;
		    }
			if(flup.corrOM==7){
				if(tabVisc.parserie>0){
					delete [] tabVisc.visc;
					delete [] tabVisc.temp;
				}
			}
			if(nsatAPoc>0){
				delete [] satAPoc;
				delete [] tempoSatAPoc;
			}
			if(nsatLRes>0){
				delete [] satLRes;
				delete [] tempoSatLRes;
			}

		}
		void iniciaVariaveis();
		void parse_materiais(Value& material_json);
		void parse_corte(Value& corte_json);
		void parse_tabela(Value& tabela_json);
		void parse_kRelOA(Value& kRelOA_json);
		void parse_kRelOG(Value& kRelOG_json);
		void parse_pcOA(Value& pcOA_json);
		void parse_pcGO(Value& pcGO_json);
		void geraTabFlash(int var);
		void parse_fluido_producao(Value& fluidos_producao_json);
		void parse_PW(Value& pW_json);
		void indrazT(int& ind, double& raz);
		void indrazPW(int& ind, double& raz);
		void indraz(int& ind, double& raz, double* serieTempo, int nserie);
		void parse_fluido_complementar(Value& fluido_json);
		void parse_tempo(Value& tempo_json);
		void parse_Prop(Value& prop_json);
		void parse_CI(Value& CI_json);
		void parse_CC(Value& CC_json);
		void parse_malha(Value& malha_json);
		void lerPoroso(string nomeArquivoEntrada);

};



#endif /* DADOS1POROSO_H_ */
