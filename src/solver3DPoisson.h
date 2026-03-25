/*
 * solverPoisson.h
 *
 *  Created on: 30 de ago. de 2023
 *      Author: Eduardo
 */

#ifndef SOLVER3DPOISSON_H_
#define SOLVER3DPOISSON_H_

#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <utility> // para std::pair
using namespace std;
//#include <ctime.h>
//#include <ctime.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
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
//#include "PropFluCol.h"
//#include "PropFlu.h"
#include "dados3DPoisson.h"
#include "variaveisGlobais1D.h"

//extern double tempo;
//extern int ntrhd;

// Usamos um hash para armazenar os níveis de forma eficiente
//using LevelMap = std::unordered_map<long long, int>; // Mapeia (i, j) -> nível


class solverP3D{
	public:

		dadosP3D dados;
		malha3d malha;
		//DadosGeo* dutosMRT;
		//ProFlu flui;
		//ProFluCol fluc;
		int nacop;

		double ki;
		double cpi;
		double rhoi;
		double visci;
		int kontaTempo;
		int itera;
		int penalizaDt;

		varGlob1D* DGP3D;

	    int rank;
	    int precn;
		//int noZeroIlu;
	    //int* veclevILU;
	    //int* vecRelILU;
	    //int* veclmILU;
	    //int* vecfnzILU;

		SparseMtx<double> matglob;
		Vcr<double> termolivre;

		solverP3D(string nomeArquivoEntrada,varGlob1D* DGP3DV, int nacopV, vector<DadosGeo>& dutoAux,
				double hi, double he, double ti);
		solverP3D();
		solverP3D(const solverP3D&);
		solverP3D& operator=(const solverP3D&);
		/*~solverP3D(){
			if(nacop>0)delete[] dutosMRT;
		}*/

		/*~solverP3D(){
			if(rank>0 && noZeroIlu>0){
				delete[] veclevILU;
				delete[] vecRelILU;
				delete[] veclmILU;
				delete[] vecfnzILU;
			}
		}

		inline long long key(int i, int j) {
		    return static_cast<long long>(i) * malha.nele + j;
		}

		void geraNivelFillinILU(
		    std::vector<int>& level,       // saída: níveis dos termos
		    std::vector<int>& linhaLev,    // saída: linha de cada termo
		    std::vector<int>& colunaLev    // saída: coluna de cada termo
		);

		void montaILUfillinCSR(
		    const std::vector<int>& level,         // níveis de cada termo (fill-in)
		    const std::vector<int>& linhaLev,      // linha correspondente a cada termo
		    const std::vector<int>& colunaLev     // coluna correspondente a cada termo
		);

		std::vector<std::vector<int>> montaForwardDependencias(int N, int* VvecfnzILU, int* VveclmILU);
		int cor_dependenciaForward(int i, const std::vector<std::vector<int>>& dependencies, const std::vector<int>& color);
		std::vector<int> coloreForwardILU();

		std::vector<std::vector<int>> montaBackwardDependencias(int N, int* VvecfnzILU, int* VveclmILU);
		int cor_dependenciaBackward(int i, const std::vector<std::vector<int>>& dependencies, const std::vector<int>& color);
		std::vector<int> coloreBackwardILU();*/


		void indrazT(int& ind, double& raz);
		int permanentePoisson();
		void inicializaPermanentePoisson();
		void inicializaTransientePoisson();
		double defineDeltPoisson();
		void transientePoisson(double delt);
		void renova();
		void transientePoissonDummy(double delt,int konta);
		void FeiticoDoTempo();

};



#endif /* SOLVER3DPOISSON_H_ */
