/*
 * criterioIntermiSevera.h
 *
 *  Created on: 5 de jan. de 2026
 *      Author: Eduardo
 */

#ifndef CRITERIOINTERMISEVERA_H_
#define CRITERIOINTERMISEVERA_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility> // para std::pair
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace std;

#include "Acidentes2.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Geometria.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Vetor.h"
#include "Matriz.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "acessorios.h"
#include "celula3.h"
#include "TrocaCalor.h"
#include "Log.h"

class critInterSev{

	public:

	int criterio;
	int indAcumulaIni;
	int indAcumulaFim;
	int indFim;
	double cotaAcumulo;
	double cotaColuna;
	double zMed;
	double compAcumula;
	double volumeAcumula;
	double alfMedAcumula;
	double area0;
	double tempMedAcumula;
	double RGas;
	double rhog0;
	double rhol0;
	double rholR;
	double rhogMedAcumula;
	double rholMedAcumula;
	double pMedAcumula;
	double pJus;
	double ugs0;
	double uls0;
	double massGas0;
	double massLiq0;
	double uslCol;
	double alfPenetra;
	double qualidadeArranjo;


	critInterSev();
    /*
     * Fields Constructor
     */
	critInterSev(Cel* celula, int vini, int vfim, int fimcota,int vtipoCrit, double valfPenetra=0.1);
    /*
     * Copy Constructor
     */
	critInterSev(const critInterSev&);
    /*
     * Assignment Operator
     */
	critInterSev& operator=(const critInterSev&);

	void atualiza(Cel* celula);
	double uls0Boe(Cel* celula);
	double difPresTeB(Cel* celula);
	double testaCriterio(int tipo, Cel* celula);
};


#endif /* CRITERIOINTERMISEVERA_H_ */
