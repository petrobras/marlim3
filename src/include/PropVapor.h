/*
 * PropVapor.h
 *
 * Created on: May 14, 2018
 *     Author: eduardo
 */

#ifndef PROPVAPOR_H_
#define PROPVAPOR_H_
#define _USE_MATH_DEFINES // Enables mathematical constants such as M_PI

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

using namespace rapidjson;

// Thermodynamic property used as the independent interpolation variable.
enum termprop { TEMP,   // Temperature
                UINT,   // Internal energy
                HENT }; // Enthalpy

// Stores and interpolates thermodynamic and transport properties for vapor-liquid systems.
class ProVap {
  public:
    // Number of saturation, superheated-vapor, and compressed-liquid pressure levels.
    int nSat;
    int nPSupAq;
    int nPCompL;

    // Input files containing thermodynamic and transport-property tables.
    string impfileSat;
    string impfileLiq;
    string impfileVap;
    string impfileCondL;
    string impfileCondG;
    string impfileViscL;
    string impfileViscG;

    // Saturation-property tables.
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

    // Superheated-vapor property tables indexed by pressure.
    vector<double> PresSupAq;
    vector<double> *TempSup;
    vector<double> *vvSup;
    vector<double> *uvSup;
    vector<double> *hvSup;
    vector<double> *svSup;
    vector<double> MaxTSupAq;
    vector<double> MinTSupAq;
    vector<double> MaxUSupAq;
    vector<double> MinUSupAq;
    vector<double> MaxHSupAq;
    vector<double> MinHSupAq;
    int *npropSup;

    // Compressed-liquid property tables indexed by pressure.
    vector<double> PresLiqC;
    vector<double> *TempLiqC;
    vector<double> *vlComp;
    vector<double> *ulComp;
    vector<double> *hlComp;
    vector<double> *slComp;
    vector<double> MaxTComp;
    vector<double> MinTComp;
    vector<double> MaxUComp;
    vector<double> MinUComp;
    vector<double> MaxHComp;
    vector<double> MinHComp;
    int *npropComp;

    // Liquid thermal-conductivity tables.
    int nPCondL;
    vector<double> PresCondL;
    vector<double> *TempCondL;
    vector<double> *CondL;
    int *nTCondL;

    // Gas thermal-conductivity tables.
    int nPCondG;
    vector<double> PresCondG;
    vector<double> *TempCondG;
    vector<double> *CondG;
    int *nTCondG;

    // Liquid-viscosity tables.
    int nPViscL;
    vector<double> PresViscL;
    vector<double> *TempViscL;
    vector<double> *ViscL;
    int *nTViscL;

    // Gas-viscosity tables.
    int nPViscG;
    vector<double> PresViscG;
    vector<double> *TempViscG;
    vector<double> *ViscG;
    int *nTViscG;

    // Cached interpolation indices, interpolation ratios, and phase identifier.
    int imPres;
    double iPres;
    int imVar;
    double iVar;
    int fase;

    ProVap(const ProVap &);            // Copy constructor
    ProVap();                          // Default constructor
    ProVap &operator=(const ProVap &); // Copy-assignment operator
    ~ProVap() {                        // Destructor
        delete[] TempSup;
        delete[] vvSup;
        delete[] uvSup;
        delete[] hvSup;
        delete[] svSup;
        delete[] npropSup;
        delete[] TempLiqC;
        delete[] vlComp;
        delete[] ulComp;
        delete[] hlComp;
        delete[] slComp;
        delete[] npropComp;
        delete[] TempCondL;
        delete[] CondL;
        delete[] nTCondL;
        delete[] TempCondG;
        delete[] CondG;
        delete[] nTCondG;
        delete[] TempViscL;
        delete[] ViscL;
        delete[] nTViscL;
        delete[] TempViscG;
        delete[] ViscG;
        delete[] nTViscG;
    }

    // Loads vapor, liquid, saturation, conductivity, and viscosity property tables.
    void LerArqVap();

    // Determines the thermodynamic phase for the specified pressure and property value.
    void verificaFase(double pres, double var, termprop tipo = TEMP);

    // Locates interpolation intervals for pressure and the selected thermodynamic property.
    void BuscaBinPres(double var, vector<double> vec);
    void BuscaBinVar(double var, vector<double> vec, int &iv, double &raz);

    // Interpolates a requested property using pressure and the selected independent variable.
    double interp(double pres, double var, termprop tipo, int nsai);

    // Returns phase-specific thermodynamic properties.
    double MasEspLiq(double pres, double var, termprop tipo = TEMP);
    double MasEspGas(double pres, double var, termprop tipo = TEMP);
    double UiLiq(double pres, double var, termprop tipo = TEMP);
    double UiGas(double pres, double var, termprop tipo = TEMP);
    double HeLiq(double pres, double var, termprop tipo = TEMP);
    double HeGas(double pres, double var, termprop tipo = TEMP);
    double SLiq(double pres, double var, termprop tipo = TEMP);
    double SGas(double pres, double var, termprop tipo = TEMP);
    double TempFun(double pres, double var, termprop tipo = TEMP);
    double Titulo(double pres, double var, termprop tipo = TEMP);

    // Returns compressibility factors and their pressure and temperature derivatives.
    double ZFunc(double pres, double temp);
    double DZDTFunc(double pres, double temp);
    double DZDPFunc(double pres, double temp);

    // Returns phase-density derivatives with respect to temperature and pressure.
    double DRhogDTFunc(double pres, double temp);
    double DRhogDPFunc(double pres, double temp);
    double DRholDTFunc(double pres, double temp);
    double DRholDPFunc(double pres, double temp);

    // Returns constant-volume and constant-pressure heat capacities.
    double CVgFunc(double pres, double temp);
    double CVgFuncMod(double pres, double temp);
    double CVlFunc(double pres, double temp);
    double CPgFunc(double pres, double temp);
    double CPgFuncMod(double pres, double temp);
    double CPlFunc(double pres, double temp);

    // Returns gas and liquid adiabatic coefficients.
    double KAgFunc(double pres, double temp);
    double KAlFunc(double pres, double temp);

    // Returns gas and liquid Joule-Thomson coefficients.
    double JTgFunc(double pres, double temp);
    double JTlFunc(double pres, double temp);

    // Returns surface tension and phase transport properties.
    double TensSupFunc(double pres, double temp);
    double CondLiq(double pres, double temp);
    double CondGas(double pres, double temp);
    double ViscLiq(double pres, double temp);
    double ViscGas(double pres, double temp);

    // Returns mixture internal energy and enthalpy.
    double energmix(double pres, double temp, double alf);
    double entalpmix(double pres, double temp, double uls, double ugs);
};

#endif /* PROPVAPOR_H_ */