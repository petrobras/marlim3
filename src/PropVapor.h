/*
 * PropVapor.h
 *
 *  Created on: 14 de mai de 2018
 *      Author: eduardo
 */

#ifndef PROPVAPOR_H_
#define PROPVAPOR_H_

#define _USE_MATH_DEFINES // para M_PI
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
#include <stdio.h>
#include <string.h>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


using namespace rapidjson;

enum termprop{TEMP,UINT,HENT};

class ProVap{
        public:
	int nSat;
	int nPSupAq;
	int nPCompL;
	string impfileSat;
	string impfileLiq;
	string impfileVap;
	string impfileCondL;
	string impfileCondG;
	string impfileViscL;
	string impfileViscG;

	vector<double> preSat;
	vector<double> tempSat;
	vector<double> vlSat;
	vector<double> vvSat;
	vector<double> ulSat;
	vector<double> uvSat;
	vector<double> hlSat;
	vector<double> hlvSat;
	vector<double> hvSat;
	vector<double> slSat;
	vector<double> svSat;
	vector<double> TensSupSat;

	vector<double> PresSupAq;
	vector<double>* TempSup;
	vector<double>* vvSup;
	vector<double>* uvSup;
	vector<double>* hvSup;
	vector<double>* svSup;
	vector<double> MaxTSupAq;
	vector<double> MinTSupAq;
	vector<double> MaxUSupAq;
	vector<double> MinUSupAq;
	vector<double> MaxHSupAq;
	vector<double> MinHSupAq;
	int* npropSup;

	vector<double> PresLiqC;
	vector<double>* TempLiqC;
	vector<double>* vlComp;
	vector<double>* ulComp;
	vector<double>* hlComp;
	vector<double>* slComp;
	vector<double> MaxTComp;
	vector<double> MinTComp;
	vector<double> MaxUComp;
	vector<double> MinUComp;
	vector<double> MaxHComp;
	vector<double> MinHComp;
	int* npropComp;

	int nPCondL;
	vector<double> PresCondL;
	vector<double>* TempCondL;
	vector<double>* CondL;
	int* nTCondL;
	//////////////////////////////////
	int nPCondG;
	vector<double> PresCondG;
	vector<double>* TempCondG;
	vector<double>* CondG;
	int* nTCondG;
	/////////////////////////////////
	int nPViscL;
	vector<double> PresViscL;
	vector<double>* TempViscL;
	vector<double>* ViscL;
	int* nTViscL;
	//////////////////////////////////
	int nPViscG;
	vector<double> PresViscG;
	vector<double>* TempViscG;
	vector<double>* ViscG;
	int* nTViscG;
	/////////////////////////////////

	int imPres;
	double iPres;
	int imVar;
	double iVar;

	int fase;


	ProVap(const ProVap&); //construtor por copia
	ProVap();
	ProVap& operator=(const ProVap&);

	~ProVap(){//destrutor{
		delete [] TempSup;
		delete [] vvSup;
		delete [] uvSup;
		delete [] hvSup;
		delete [] svSup;
		delete [] npropSup;

		delete [] TempLiqC;
		delete [] vlComp;
		delete [] ulComp;
		delete [] hlComp;
		delete [] slComp;
		delete [] npropComp;

		delete[] TempCondL;
		delete[] CondL;
		delete[] nTCondL;
		////////////////////////
		delete[] TempCondG;
		delete[] CondG;
		delete[] nTCondG;
		////////////////////////
		delete[] TempViscL;
		delete[] ViscL;
		delete[] nTViscL;
		////////////////////////
		delete[] TempViscG;
		delete[] ViscG;
		delete[] nTViscG;
	}

	void LerArqVap();

	void verificaFase(double pres, double var, termprop tipo=TEMP);
	void BuscaBinPres(double var,vector<double> vec);
	void BuscaBinVar(double var,vector<double> vec,int& iv, double& raz);
	double interp(double pres, double var, termprop tipo, int nsai);
	double MasEspLiq(double pres, double var, termprop tipo=TEMP);
	double MasEspGas(double pres, double var, termprop tipo=TEMP);
	double UiLiq(double pres, double var, termprop tipo=TEMP);
	double UiGas(double pres, double var, termprop tipo=TEMP);
	double HeLiq(double pres, double var, termprop tipo=TEMP);
	double HeGas(double pres, double var, termprop tipo=TEMP);
	double SLiq(double pres, double var, termprop tipo=TEMP);
	double SGas(double pres, double var, termprop tipo=TEMP);
	double TempFun(double pres, double var, termprop tipo=TEMP);
	double Titulo(double pres, double var, termprop tipo=TEMP);
	double ZFunc(double pres, double temp);
	double DZDTFunc(double pres, double temp);
	double DZDPFunc(double pres, double temp);
	double DRhogDTFunc(double pres, double temp);
	double DRhogDPFunc(double pres, double temp);
	double DRholDTFunc(double pres, double temp);
	double DRholDPFunc(double pres, double temp);
	double CVgFunc(double pres, double temp);
	double CVgFuncMod(double pres, double temp);
	double CVlFunc(double pres, double temp);
	double CPgFunc(double pres, double temp);
	double CPgFuncMod(double pres, double temp);
	double CPlFunc(double pres, double temp);
	double KAgFunc(double pres, double temp);
	double KAlFunc(double pres, double temp);
	double JTgFunc(double pres, double temp);
	double JTlFunc(double pres, double temp);
	double TensSupFunc(double pres, double temp);
	double CondLiq(double pres, double temp);
	double CondGas(double pres, double temp);
	double ViscLiq(double pres, double temp);
	double ViscGas(double pres, double temp);
	double energmix(double pres, double temp,double alf);
	double entalpmix(double pres, double temp,double uls, double ugs);
};



#endif /* PROPVAPOR_H_ */
