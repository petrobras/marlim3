/*
 * Leitura.h
 *
 *  Created on: Mar 18, 2015
 *      Author: bwz1
 */

#ifndef LEITURA_H_
#define LEITURA_H_
#define _USE_MATH_DEFINES // Enables access to the M_PI constant

#include "Acidentes2.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Geometria.h"
#include "JSONDataModel.h"
#include "JSONKeyTranslator.h"
#include "JSON_entrada.h"
#include "Log.h"
#include "MarlimComposicional.h"
#include "Matriz.h"
#include "PorosoRad-Simples.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "acessorios.h"
#include "celula3.h"
#include "celulaGas.h"
#include "chokegas.h"
#include "criterioIntermiSevera.h"
#include "dados1Poisson.h"
#include "multiBCS.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "solverPoisson.h"
#include "solverPoroso.h"
#include "validaTipoJson.h"
#include "variaveisGlobais1D.h"
#include "SlugFlow_TaitelBarnea.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <utility> // Provides std::pair
#include <vector>

#ifdef linux
// Directory separator used on Linux systems
#define BARRA "/"
#elif LINUX
// Directory separator used on Linux systems
#define BARRA "/"
#elif Linux
// Directory separator used on Linux systems
#define BARRA "/"
#elif UNIX
// Directory separator used on Unix-based systems
#define BARRA "/"
#elif defined WIN32
// Directory separator used on Windows systems
#define BARRA "\\"
#elif defined Win32
// Directory separator used on Windows systems
#define BARRA "\\"
#elif defined win32
// Directory separator used on Windows systems
#define BARRA "\\"
#else
// Fallback directory separators for other platforms
#define BARRA "/\\"
#endif

using namespace std;
using namespace rapidjson;

// Output stream used to generate simulation data file reports
extern ofstream arqRelatorioPerfis;

// Logger instance used by the application
extern Logger logger;

// Path to the simulator input files
extern string pathArqExtEntrada;

// Output file path prefix used for POCO_INJETOR
extern string pathPrefixoArqSaida;

extern int nthrdMatriz;

extern string versao;

typedef enum { none,
               json,
               schema,
               rules,
               off } tipoValidacaoJson_t;
typedef enum { transiente,
               poco_injetor,
               rede,
               convecNatural } tipoSimulacao_t;
typedef enum { jusante,
               montante } origemGeometria_t;
typedef enum { multifasico,
               injetor,
               oleoduto } sistemaSimulacao_t;

struct detTMAX {
    int parserie;  // Length of the time series
    double *tempo; // Time values at which changes occur, in seconds
    double *valor; // Maximum time-step increments, in seconds
};

struct detRGO {
    int parserie;
    double *tempo;
    double *valor;
};

struct material {
    int id;
    double cond; // Thermal conductivity, W/mK
    double cp;   // Specific heat capacity, J/kgK
    double rho;  // Density, kg/m3
    int tipo;    // Material type: 0 = solid, 1 = user-defined fluid, 2 = water, 3 = air
    double visc; // Viscosity in cP, used only for type 1
    double beta; // Thermal expansion coefficient, used only for type 1
};

struct corteduto {
    int id;
    int ncam;     // Number of material layers
    int anul;     // Indicates whether the flow region is annular
    double a;     // Internal diameter or, for annular flow, the smaller annulus diameter
    double b;     // Larger annulus diameter
    double rug;   // Absolute roughness, in meters
    double *diam; // Largest diameter of each material layer
    int *indmat;  // Material identifier associated with the material structure
    int *discre;  // Discretization count for each material layer
};

struct detduto {
    double ang;   // Pipe angle in radians
    int indcorte; // Cross-section index associated with corteduto
    int servico;  // Indicates whether this is a production or service line
};

// Contains the complete data for a production flow unit
struct detalhaP {
    int ind;
    int duto;    // Pipe index associated with detduto
    int ncel;    // Number of unit discretizations
    double comp; // Unit length
    double Lini; // Unit start position referenced from the well bottom
    int dirconv; // External convection direction: 0 = crossflow against the pipe, 1 = parallel to the pipe
    int acopcol; // Indicates thermal coupling with a service-line unit
    int acopcolRede;
    int formacDuto; // Indicates that the external environment is a geological formation
    int lito;       // Formation lithology index associated with detFormacao
    int ambext;     // External environment type: 0 = user-defined, 1 = marine, 2 = atmospheric
    int nponts;     // Number of points containing environmental and, when applicable, initial-condition data
    double *dxVar;  // Relative positions at which external-environment and initial-condition values are provided
    double **var;   // Matrix containing the specified environmental and initial-condition data
    double dPdLHidro;
    double dPdLFric;
    double dTdL;
    int inddPdLHidro;
    int inddPdLFric;
    int inddTdL;
    int difusTerm2D;
    string difusTerm2DJSON;
    int *difusTerm3D;
    double *difusTerm3DFE;
    string *difusTerm3DAcop;

    // User-defined discretization settings
    int agrupaDiscre; // Indicates whether discretization is read as groups of cells
    // Otherwise, each cell length is defined individually in the system
    double *dx; // Length of each discretized cell
    int correlacaoMR2;
};

// Describes a production cell; populated after parsing the production unit rather than directly from JSON
struct detcelp {
    double dx;      // Cell length
    int duto;       // Pipe index associated with detduto
    double temp;    // Initial-condition temperature when the steady-state solution is not calculated
    double pres;    // Initial-condition pressure when the steady-state solution is not calculated
    double hol;     // Initial-condition holdup when the steady-state solution is not calculated
    double bet;     // Initial-condition beta when the steady-state solution is not calculated
    double uls;     // Initial liquid superficial velocity when the steady-state solution is not calculated
    double ugs;     // Initial gas superficial velocity when the steady-state solution is not calculated
    double textern; // Ambient temperature
    double vextern; // External-flow velocity when the environment is neither a formation nor a coupled service line
    double kextern; // External-environment thermal conductivity when the environment is not a formation or coupled service line
    // Also applies when the environment is neither marine nor atmospheric
    double cpextern; // External-environment specific heat when the environment is not a formation or coupled service line
    // Also applies when the environment is neither marine nor atmospheric
    double rhoextern; // External-environment density when the environment is not a formation or coupled service line
    // Also applies when the environment is neither marine nor atmospheric
    double viscextern; // External-environment viscosity when the environment is not a formation or coupled service line
    // Also applies when the environment is neither marine nor atmospheric
    int ambext;  // External environment type: 0 = user-defined, 1 = marine, 2 = atmospheric
    int dirconv; // External convection direction: 0 = crossflow against the pipe, 1 = parallel to the pipe
    int acopcol; // Indicates thermal coupling with a service-line unit
    int acopcolRede;
    int formacCel;    // Indicates that the external environment is a geological formation
    int lito;         // Formation lithology index associated with detFormacao
    double profundiM; // Depth at the cell center, referenced from the platform
    double profundiF; // Depth at the cell boundary, referenced from the platform
    double dPdLHidro; // Hydrostatic pressure-gradient multiplier; default value is 1
    double dPdLFric;  // Frictional pressure-gradient multiplier; default value is 1
    double dTdL;      // Temperature-gradient multiplier; default value is 1
    int inddPdLHidro; // Index selecting the hydrostatic pressure-gradient multiplier
    int inddPdLFric;  // Index selecting the frictional pressure-gradient multiplier
    int inddTdL;      // Index selecting the temperature-gradient multiplier

    int difusTerm2D;
    string difusTerm2DJSON;

    int difusTerm3D;
    double difusTerm3DFE;
    string difusTerm3DAcop;

    int correlacaoMR2;
};

// Contains the complete data for a service-line flow unit
struct detalhaG {
    int ind;
    int duto;         // Pipe index associated with detduto
    int ncel;         // Number of unit discretizations
    double comp;      // Unit length
    double Lini;      // Unit start position referenced from the well bottom
    int acopcol;      // Indicates thermal coupling with a production-line unit
    int formacDuto;   // Indicates that the external environment is a geological formation
    int lito;         // Formation lithology index associated with detFormacao
    int ambext;       // External environment type: 0 = user-defined, 1 = marine, 2 = atmospheric
    int nponts;       // Number of points containing environmental and, when applicable, initial-condition data
    double *dxVar;    // Relative positions at which external-environment and initial-condition values are provided
    double **var;     // Matrix containing the specified environmental and initial-condition data
    int agrupaDiscre; // Indicates whether discretization is read as groups of cells
    // Otherwise, each cell length is defined individually in the system
    double *dx; // Length of each discretized cell
    double dPdLHidro;
    double dPdLFric;
    double dTdL;
    int inddPdLHidro;
    int inddPdLFric;
    int inddTdL;
};

// Describes a service-line cell; populated after parsing the service line rather than directly from JSON
struct detcelg {
    double dx;      // Cell length
    int duto;       // Pipe index associated with detduto
    double temp;    // Initial-condition temperature when the steady-state solution is not calculated
    double pres;    // Initial-condition pressure when the steady-state solution is not calculated
    double mg;      // Initial-condition mass flow rate when the steady-state solution is not calculated
    double textern; // Ambient temperature
    double vextern; // External-flow velocity when the environment is neither a formation nor a coupled production line
    double kextern; // External-environment thermal conductivity when the environment is not a formation or coupled production line
    // Also applies when the environment is neither marine nor atmospheric
    double cpextern; // External-environment specific heat when the environment is not a formation or coupled production line
    // Also applies when the environment is neither marine nor atmospheric
    double rhoextern; // External-environment density when the environment is not a formation or coupled production line
    // Also applies when the environment is neither marine nor atmospheric
    double viscextern; // External-environment viscosity when the environment is not a formation or coupled production line
    // Also applies when the environment is neither marine nor atmospheric
    int acopcol;      // Indicates thermal coupling with a production-line unit
    int formacCel;    // Indicates that the external environment is a geological formation
    int ambext;       // External environment type: 0 = user-defined, 1 = marine, 2 = atmospheric
    int lito;         // Formation lithology index associated with detFormacao
    double profundiM; // Depth at the cell center, referenced from the platform
    double profundiF; // Depth at the cell boundary, referenced from the platform
    double dPdLHidro;
    double dPdLFric;
    double dTdL;
    int inddPdLHidro;
    int inddPdLFric;
    int inddTdL;
};

// Contains the IPR data required for JSON parsing
struct detIPR {
    int indcel;    // Cell index
    double comp;   // Source position in meters, referenced from the well bottom
    int tipoIPR;   // IPR model: 0 = linear IPR, 1 = combined Vogel, 2 = Vogel
    int seriep;    // Number of elements in the static-pressure time series
    double *pres;  // Pressure series
    double *tpres; // Times associated with the pressure series
    int seriet;    // Number of elements in the reservoir-temperature time series
    double *temp;  // Temperature series
    double *ttemp; // Times associated with the temperature series
    int serieip;   // Number of elements in the reservoir productivity-index time series
    double *ip;    // Productivity-index series in stdm3/d/kgf/cm2; not valid for pure Vogel, type 2
    double *tip;   // Times associated with the productivity-index series
    int seriejp;   // Number of elements in the reservoir injectivity-index time series
    double *jp;    // Injectivity-index series in stdm3/d/kgf/cm2
    double *tjp;   // Times associated with the injectivity-index series
    int serieqm;   // Number of elements in the reservoir maximum-flow-rate time series for pure Vogel
    double *qMax;  // Maximum-flow-rate series in stdm3/d
    double *tqMax; // Times associated with the maximum-flow-rate series
    int indfluP;   // Production-fluid index used by the IPR
};

// Defines gas-lift injection through the service line
struct detGASINJ {
    int tipoCC;   // Boundary-condition type: 0 = injection pressure, 1 = injection flow rate
    int parserie; // Number of elements in the injection flow-rate or pressure time series
    int chuteVaz; // Optional steady-state injection-flow initial guess for pressure-controlled injection
    // Used to improve convergence
    double *temperatura; // Injection-temperature time-series values
    double *presinj;     // Injection-pressure time-series values, when applicable
    double *vazgas;      // Injection-flow-rate time-series values, when applicable
    double *tempo;       // Time-series values
};

// Defines the pressure boundary condition at the pipe inlet
struct detPresEnt {
    int parserie; // Number of elements in the time series
    int indFluido;
    double *temperatura; // Pipe-inlet temperature time series
    double *pres;        // Pipe-inlet pressure time series
    double *tit;         // Pipe-inlet quality time series
    double *bet;         // Pipe-inlet beta time series
    double *tempo;       // Time-series values
};

// Defines the pressure and flow-rate boundary conditions at the pipe inlet
struct detVazPresEnt {
    int parserie;        // Number of elements in the time series
    double *temperatura; // Pipe-inlet temperature time series
    double *pres;        // Pipe-inlet pressure time series
    double *mass;        // Pipe-inlet mass-flow-rate time series, in kg/s
    double *bet;         // Pipe-inlet beta time series
    double *tempo;       // Time-series values
};

// Defines the gas-lift valve
struct detVALVGL {
    int posicP;      // Cell index in the production line where the GLV is installed
    int posicG;      // Cell index in the service line where the GLV is installed
    double compP;    // Position along the production line, referenced from the well bottom
    double compG;    // Position along the service line, referenced from the platform
    int tipo;        // GLV type: 0 = orifice, 1 = pressure-operated, 2 = Venturi
    double diagarg;  // Orifice diameter, in meters
    double frec;     // Not used
    double frecLiq;  // Not used
    double pcali;    // Calibration pressure, in psi
    double tcali;    // Calibration temperature, in degrees Fahrenheit
    double diaexter; // Port diameter, in meters
    double cd;       // Discharge coefficient
    double cdLiq;    // Liquid discharge coefficient for gas-lift unloading
    double razarea;  // Area ratio for a pressure-operated valve
};

// Defines a gas source
struct detFONGAS {
    int posicP;   // Cell index where the source is located in the production system
    double comp;  // Source position in the production system, referenced from the well bottom
    int parserie; // Number of elements in the time series
    int seco;
    int indfluP;
    double *temp;    // Source-temperature values
    double *vazgas;  // Source flow-rate values, in sm3/d
    double *vazcomp; // Complementary-flow-rate values, in sm3/d
    double *tempo;   // Time-series values
};

// Defines a radial porous-medium source
struct detPoroRad {
    int posicP;  // Cell index where the source is located in the production system
    double comp; // Source position in the production system, referenced from the well bottom
    string nomeArquivoEntrada;
};

// Defines a 2D porous-medium source
struct detPoro2D {
    int posicP;  // Cell index where the source is located in the production system
    double comp; // Source position in the production system, referenced from the well bottom
    string nomeArquivoEntrada;
};

// Defines CV-curve data
struct detCV {
    double x1;
    double x2;
    double cv1;
    double cv2;
};

// Defines a valve
struct detValv {
    int posicP;       // Cell index where the valve is located in the production system
    double comp;      // Valve position in the production system, referenced from the well bottom
    int parserie;     // Number of elements in the time series
    double *abertura; // Valve-opening values relative to the pipe area
    double *tempo;    // Time-series values
    double cd;        // Valve discharge coefficient
    int curvaCV;
    int curvaDinamic;
    int ncv;
    double xini;
    detCV *cvCurv;
};

// Defines a liquid source
struct detFONLIQ {
    int posicP;     // Cell index where the source is located in the production system
    double comp;    // Source position in the production system, referenced from the well bottom
    int parserie;   // Number of elements in the time series
    double *temp;   // Source-temperature values
    double *bet;    // Source beta values
    double *vazliq; // Liquid source flow-rate values, in sm3/d
    double *tempo;  // Time-series values
    int indfluP;    // Production-fluid index used by the source
};

// Defines a mass source
struct detFONMASS {
    int posicP;      // Cell index where the source is located in the production system
    double comp;     // Source position in the production system, referenced from the well bottom
    int parserie;    // Number of elements in the time series
    int condiTermo;  // When zero, thermodynamic equilibrium is assumed
    double *temp;    // Source-temperature values
    double *vazMasT; // Total source mass-flow rate, in kg/s
    double *vazMasC; // Complementary-liquid source mass-flow rate, in kg/s
    double *vazMasG; // Gas source mass-flow rate, in kg/s
    double *tempo;   // Time-series values
    int indfluP;     // Production-fluid index used by the source
};

// Defines a leakage-derived pressure source
struct detFURO {
    int posicP;       // Cell index where the source is located in the production system
    double comp;      // Source position in the production system, referenced from the well bottom
    int parserie;     // Number of elements in the time series
    int TipoAbertura; // Indicates whether the hole size is specified by diameter when type = 1
    // Otherwise, type = 0 specifies the ratio between the hole and pipe areas
    double pres; // Pressure external to the pipe
    double temp; // Temperature external to the pipe
    double beta; // Beta value external to the pipe
    double titAmb;
    double cd;        // Hole discharge coefficient
    double *abertura; // Hole-opening values for the time series
    double *tempo;    // Time-series values
    int tipoFlu;      // When 1, the first production fluid is used; otherwise, the fluid index is requested
    // Selects which production-fluid index is used
    int indFlu; // Index of the production fluid external to the pipe
    int *check;
    double *tempoChk;
    int parserieChk;
    int ambGas;
};

// Defines an ESP
struct detBCS {
    int posicP;        // Cell index where the ESP is located in the production system
    double comp;       // ESP position in the production system, referenced from the well bottom
    int parserie;      // Number of elements in the time series
    double *tempo;     // Time-series values
    double *freq;      // ESP-frequency time series, in hertz
    int ncurva;        // Number of points defining the ESP curve
    double *vaz;       // ESP-curve flow-rate values, in BPD
    double *head;      // ESP-curve head values, in feet
    double *power;     // ESP-curve power values, in horsepower
    double *efic;      // ESP-curve efficiency values
    double freqref;    // Reference frequency at which the curve was measured
    int nestag;        // Number of pump stages
    double eficM;      // Motor efficiency
    double freqMinima; // Minimum ESP operating frequency
    int correcHI;
    double fracTermMotorEfic;
};

struct detMultiBCS {
    int posicP;        // Cell index where the ESP is located in the production system
    double comp;       // ESP position in the production system, referenced from the well bottom
    double eficM;      // Motor efficiency
    double freqMinima; // Minimum ESP operating frequency
    int correcHI;
    int equilTerm;
    double fracTermMotorEfic;
    double freqref; // Reference frequency at which the curve was measured
    int nestag;     // Number of pump stages
    int parserie;   // Number of elements in the time series
    double *tempo;  // Time-series values
    double *freq;   // ESP-frequency time series, in hertz
    int nBCS;
    int *nestagParcFab;
    int *nestagParc;
    int *ncurva;
    detBCS *BCSinterno;
    detMultiBCS() {
        posicP = -1;
        comp = -1.;
        eficM = 0.;
        freqMinima = 0.;
        correcHI = -1;
        equilTerm = 1;
        freqref = 0.;
        nestag = 0;
        parserie = 0;
        tempo = 0;
        freq = 0;
        nBCS = 0;
        nestagParcFab = 0;
        nestagParc = 0;
        BCSinterno = 0;
        ncurva = 0;
        fracTermMotorEfic = 0.;
    }
    ~detMultiBCS() {
        if (nestagParc != 0 && nBCS > 0)
            delete[] nestagParc;
        if (nestagParcFab != 0 && nBCS > 0)
            delete[] nestagParcFab;
        if (ncurva != 0 && nBCS > 0)
            delete[] ncurva;
        if (BCSinterno != 0 && nBCS > 0) {
            for (int i = 0; i < nBCS; i++) {
                delete[] BCSinterno[i].tempo;
                delete[] BCSinterno[i].freq;
                delete[] BCSinterno[i].vaz;
                delete[] BCSinterno[i].head;
                delete[] BCSinterno[i].power;
                delete[] BCSinterno[i].efic;
            }
            delete[] BCSinterno;
            nBCS = 0;
            BCSinterno = 0;
        }
    }
};

// Defines a positive-displacement pump
struct detBVOL {
    int posicP;        // Cell index where the pump is located in the production system
    double comp;       // Pump position in the production system, referenced from the well bottom
    int parserie;      // Number of elements in the time series
    double *tempo;     // Time-series values
    double *freq;      // Pump-frequency time series, in hertz
    double capacidade; // Pump volumetric capacity, in m3
    double npoli;      // Polytropic factor
};

// Defines a localized pressure increase
struct detDPREQ {
    int posicP;      // Cell index where the pressure increase is located in the production system
    double comp;     // Pressure-increase position in the production system, referenced from the well bottom
    int parserie;    // Number of elements in the time series
    double *dp;      // Pressure-difference time-series values, in kgf/cm2
    double *tempo;   // Time-series values
    int tipoCompGas; // Gas-compression process: 0 = adiabatic
    // 1 = polytropic, 2 = isothermal
    double fatPoli; // Polytropic factor when tipoCompGas == 1; otherwise, the adiabatic constant
    double eficLiq; // Liquid-phase pumping efficiency; defaults to 100% when not specified
    double eficGas; // Gas-phase compression efficiency; defaults to 100% when not specified
};

// Defines the Master1 valve
struct detMASTER1 {
    int parserie;       // Number of elements in the time series
    int posic;          // Cell index where Master1 is located in the production system
    double comp;        // Master1 position in the production system, referenced from the well bottom
    double *abertura;   // Master1 relative-opening values referenced to the pipe area
    double *tempo;      // Time-series values
    double razareaativ; // Relative area at which Master1 begins to operate as a choke
    int curvaCV;
    int ncv;
    detCV *cvCurv;
};

// Defines the downstream line pressure, corresponding to the separator pressure
struct detPSEP {
    int parserie;  // Number of elements in the time series
    double *pres;  // Separator or downstream-pressure time-series values, in kgf/cm2
    double *tempo; // Time-series values
};

// Defines a heat source
struct detCalor {
    int posicP;    // Cell index where the source is located in the production system
    double comp;   // Source position in the production system, referenced from the well bottom
    int parserie;  // Number of elements in the time series
    double *cal;   // Heat-source power values, in watts
    double *tempo; // Time-series values
};

struct detCorrec {
    int parserieHidro;
    int parserieFric;
    int parserieDT;
    double *dPdLHidro;
    double *dPdLFric;
    double *dTdL;
};

// Defines the surface choke
struct detCHOKESUP {
    int parserie;     // Number of elements in the time series
    double cd;        // Discharge coefficient
    int modelo;       // Multiphase choke model: 0 = Sachdeva
    double *abertura; // Choke relative-opening values referenced to the pipe area
    double *tempo;    // Time-series values
    int curvaCV;
    int curvaDinamic;
    int ncv;
    double xini;
    detCV *cvCurv;
};

// Defines a pig operation
struct detPig {
    int lanca;      // Cell index where the pig is launched in the production system
    int receb;      // Cell index where the pig is received in the production system
    double razarea; // Not used
    double delpres; // Not used
    double cdPig;   // Not used
    double compL;   // Pig-launch position in the production system, referenced from the well bottom
    double compR;   // Pig-receiving position in the production system, referenced from the well bottom
    double tempo;   // Time at which the pig is launched
};

struct detIntermi {
    int indInicioTrechoAcumula;
    int indFimTrechoAcumula;
    int indFimTrechoColuna;
    int criterio;
    double fracaoVazioPenetracao;
};

// Defines the production-line profiles to be recorded
struct detPROFP {
    int n;     // Number of profiles to be recorded
    int pres;  // Indicates whether the pressure profile must be recorded
    int temp;  // Indicates whether the temperature profile must be recorded
    int hol;   // Indicates whether the holdup profile must be recorded
    int FVH;   // Indicates whether the hydrate volume-fraction profile must be recorded by the hydrate solver
    int bet;   // Indicates whether the beta profile must be recorded
    int ugs;   // Indicates whether the gas superficial-velocity profile must be recorded
    int uls;   // Indicates whether the liquid superficial-velocity profile must be recorded
    int ug;    // Indicates whether the gas average-velocity profile must be recorded
    int ul;    // Indicates whether the liquid average-velocity profile must be recorded
    int arra;  // Indicates whether the flow-pattern profile must be recorded
    int yco2;  // Indicates whether the CO2-fraction profile must be recorded
    int viscl; // Indicates whether the liquid-viscosity profile must be recorded
    int viscg; // Indicates whether the gas-viscosity profile must be recorded
    int rhog;  // Indicates whether the gas-density profile must be recorded
    int rhol;  // Indicates whether the liquid-density profile must be recorded
    int rhoo;  // Indicates whether the oil-density profile must be recorded
    int rhoa;  // Indicates whether the water-density profile must be recorded
    int rhoMix;
    int RS;
    int masg;      // Indicates whether the gas mass-flow-rate profile must be recorded
    int masl;      // Indicates whether the liquid mass-flow-rate profile must be recorded
    int qlst;      // Indicates whether the standard heavy-fraction flow-rate profile must be recorded
    int qlwst;     // Indicates whether the standard heavy-fraction-plus-water profile must be recorded
    int qgst;      // Indicates whether the standard light-fraction flow-rate profile must be recorded
    int qlstTot;   // Indicates whether the standard total-liquid-flow-rate profile must be recorded
    int c0;        // Indicates whether the distribution-parameter profile must be recorded
    int ud;        // Indicates whether the drift-velocity profile must be recorded
    int RGO;       // Indicates whether the local GOR profile must be recorded
    int deng;      // Indicates whether the gas-density profile must be recorded
    int calor;     // Indicates whether the heat-flux profile must be recorded
    int masstrans; // Indicates whether the mass-transfer profile must be recorded
    int cpg;       // Indicates whether the gas-specific-heat profile must be recorded
    int cpl;       // Indicates whether the liquid-specific-heat profile must be recorded
    int api;       // Indicates whether the API-gravity profile must be recorded
    int bsw;       // Indicates whether the BSW profile must be recorded
    int hidro;     // Indicates whether the hydrostatic-term profile must be recorded
    int fric;      // Indicates whether the friction-term profile must be recorded
    int Term1;
    int Term2;
    int dengD; // Indicates whether the dissolved-gas-density profile must be recorded
    int dengL; // Indicates whether the free-gas-density profile must be recorded

    int reyi;   // Indicates whether the internal Reynolds-number profile must be recorded
    int reye;   // Indicates whether the ambient Reynolds-number profile must be recorded
    int Fr;     // Indicates whether the Froude-number profile must be recorded
    int grashi; // Indicates whether the internal Grashof-number profile must be recorded
    int grashe; // Indicates whether the external Grashof-number profile must be recorded
    int nusi;   // Indicates whether the internal Nusselt-number profile must be recorded
    int nuse;   // Indicates whether the external Nusselt-number profile must be recorded
    int hi;     // Indicates whether the internal film-coefficient profile must be recorded
    int he;     // Indicates whether the external film-coefficient profile must be recorded
    int pri;    // Indicates whether the internal Prandtl-number profile must be recorded
    int pre;    // Indicates whether the external Prandtl-number profile must be recorded

    int Rs; // Indicates whether the solution gas-oil ratio profile must be recorded
    int Bo; // Indicates whether the oil formation-volume-factor profile must be recorded

    int tAmb; // Indicates whether the ambient-temperature profile must be recorded
    int PrG;  // Gas Prandtl number for internal flow
    int PrL;  // Liquid Prandtl number for internal flow

    int pseudoL;
    int pseudoG;
    int pseudoM;
    int TResi;
    int redAtrito;
    int angulo;
    int diamInt;
    int TempParede;
    int dadosParafina;
    int subResfria;
    int correlacaoBB;
    double *tempo; // Times at which the profiles must be recorded
};

// Defines the service-line profiles to be recorded
struct detPROFG {
    int n;     // Number of profiles to be recorded
    int pres;  // Indicates whether the pressure profile must be recorded
    int temp;  // Indicates whether the temperature profile must be recorded
    int ugs;   // Indicates whether the gas superficial-velocity profile must be recorded
    int ug;    // Indicates whether the gas average-velocity profile must be recorded
    int tens;  // Indicates whether the shear-stress profile must be recorded
    int viscg; // Indicates whether the gas-viscosity profile must be recorded
    int rhog;  // Indicates whether the gas-density profile must be recorded
    int masg;  // Indicates whether the gas mass-flow-rate profile must be recorded
    int masl;  // Indicates whether the liquid mass-flow-rate profile must be recorded
    int qgst;  // Indicates whether the standard gas-flow-rate profile must be recorded
    int hidro; // Indicates whether the hydrostatic-term profile must be recorded
    int fric;  // Indicates whether the friction-term profile must be recorded
    int FVHG;  // Indicates whether the service-line hydrate volume-fraction profile must be recorded
    int calor; // Indicates whether the heat-flux profile must be recorded

    int reyi;   // Indicates whether the internal Reynolds-number profile must be recorded
    int reye;   // Indicates whether the ambient Reynolds-number profile must be recorded
    int grashi; // Indicates whether the internal Grashof-number profile must be recorded
    int grashe; // Indicates whether the external Grashof-number profile must be recorded
    int nusi;   // Indicates whether the internal Nusselt-number profile must be recorded
    int nuse;   // Indicates whether the external Nusselt-number profile must be recorded
    int hi;     // Indicates whether the internal film-coefficient profile must be recorded
    int he;     // Indicates whether the external film-coefficient profile must be recorded
    int pri;    // Indicates whether the internal Prandtl-number profile must be recorded
    int pre;    // Indicates whether the external Prandtl-number profile must be recorded
    int tAmb;   // Indicates whether the ambient-temperature profile must be recorded

    int angulo;
    int diamInt;
    int TempParede;

    int subResfria;

    double *tempo; // Times at which the profiles must be recorded
};

// Defines thermal cross-section profiles to be recorded along the production line
struct detPROFTRANS {
    int n;         // Number of times at which profiles are recorded
    int *posic;    // Cross-section indices at which thermal profiles are recorded
    double *tempo; // Times at which the profiles are recorded
};

// Defines production-line trend recording settings
struct detTRENDP {
    double dt; // Time interval between trend records
    int posic; // Cell index from which the trend is obtained
    int pres;  // Indicates whether the pressure trend must be recorded
    int temp;  // Indicates whether the temperature trend must be recorded
    int hol;   // Indicates whether the holdup trend must be recorded
    int FVH;   // Indicates whether the hydrate volume-fraction trend must be recorded by the hydrate solver
    int bet;   // Indicates whether the beta trend must be recorded
    int ugs;   // Indicates whether the gas superficial-velocity trend must be recorded
    int uls;   // Indicates whether the liquid superficial-velocity trend must be recorded
    int ug;    // Indicates whether the gas average-velocity trend must be recorded
    int ul;    // Indicates whether the liquid average-velocity trend must be recorded
    int arra;  // Indicates whether the flow-pattern trend must be recorded
    int yco2;  // Indicates whether the CO2-fraction trend must be recorded
    int viscl; // Indicates whether the liquid-viscosity trend must be recorded
    int viscg; // Indicates whether the gas-viscosity trend must be recorded
    int rhog;  // Indicates whether the gas-density trend must be recorded
    int rhol;  // Indicates whether the liquid-density trend must be recorded
    int rhoMix;
    int masg;      // Indicates whether the gas mass-flow-rate trend must be recorded
    int masl;      // Indicates whether the liquid mass-flow-rate trend must be recorded
    int c0;        // Indicates whether the distribution-parameter trend must be recorded
    int ud;        // Indicates whether the drift-velocity trend must be recorded
    int RGO;       // Indicates whether the local GOR trend must be recorded
    int deng;      // Indicates whether the separator gas-density trend must be recorded
    int calor;     // Indicates whether the heat-flux trend must be recorded
    int masstrans; // Indicates whether the mass-transfer trend must be recorded
    int qlst;      // Indicates whether the standard heavy-fraction flow-rate trend must be recorded
    int qlwst;     // Indicates whether the standard heavy-fraction-plus-water trend must be recorded
    int qlstTot;   // Indicates whether the standard total-liquid-flow-rate trend must be recorded
    int qgst;      // Indicates whether the standard light-fraction flow-rate trend must be recorded
    int api;       // Indicates whether the API-gravity trend must be recorded
    int bsw;       // Indicates whether the BSW trend must be recorded
    int hidro;     // Indicates whether the hydrostatic-term trend must be recorded
    int fric;      // Indicates whether the friction-term trend must be recorded
    int dengD;     // Indicates whether the dissolved-gas-density trend must be recorded
    int dengL;     // Indicates whether the free-gas-density trend must be recorded

    int mlFonte; // Indicates whether the hydrocarbon-plus-water mass-source trend must be recorded
    int mgFonte; // Indicates whether the gas mass-source trend must be recorded
    int mcFonte; // Indicates whether the complementary-liquid mass-source trend must be recorded

    int dpB;  // Indicates the pressure increase produced by a pump
    int potB; // Indicates pumping power

    int tempChokeJus; // Indicates whether the downstream surface-choke temperature trend must be recorded

    int reyi;   // Indicates whether the internal Reynolds-number trend must be recorded
    int reye;   // Indicates whether the ambient Reynolds-number trend must be recorded
    int Fr;     // Indicates whether the Froude-number trend must be recorded
    int grashi; // Indicates whether the internal Grashof-number trend must be recorded
    int grashe; // Indicates whether the external Grashof-number trend must be recorded
    int nusi;   // Indicates whether the internal Nusselt-number trend must be recorded
    int nuse;   // Indicates whether the external Nusselt-number trend must be recorded
    int hi;     // Indicates whether the internal film-coefficient trend must be recorded
    int he;     // Indicates whether the external film-coefficient trend must be recorded
    int pri;    // Indicates whether the internal Prandtl-number trend must be recorded
    int pre;    // Indicates whether the external Prandtl-number trend must be recorded

    int Rs; // Indicates whether the solution gas-oil ratio trend must be recorded
    int Bo; // Indicates whether the oil formation-volume-factor trend must be recorded

    int volMonM1PT; // Upstream liquid volume at pressure and temperature conditions relative to Master1
    int volJusM1PT; // Downstream liquid volume at pressure and temperature conditions relative to Master1
    int volMonM1ST; // Upstream liquid volume at standard conditions relative to Master1
    int volJusM1ST; // Downstream liquid volume at standard conditions relative to Master1

    int autoVal;
    int autoVel;
    int flutuacao;

    int comp; // Position where the trend is recorded, referenced from the well bottom
    int diamInt;
    int TempParede;
    int dadosParafina;
    int inventarioGas;
    int inventarioLiq;
    int subResfria;
    string rotulo; // Label
};

struct detTela {
    int col;   // Indicates whether the variable belongs to the production or service line; 1 = production line
    int posic; // Cell index from which the variable value is obtained
    int var;   // Variable identifier:
    // Production: 1 = pressure, 2 = temperature, 3 = alpha, 4 = USG, 5 = USL
    // Service: 1 = pressure, 2 = temperature, 3 = USG
};

// Defines service-line trend recording settings
struct detTRENDG {
    double dt;     // Time interval between trend records
    int posic;     // Cell index from which the trend is obtained
    int pres;      // Indicates whether the pressure trend must be recorded
    int temp;      // Indicates whether the temperature trend must be recorded
    int ugs;       // Indicates whether the gas superficial-velocity trend must be recorded
    int ug;        // Indicates whether the gas average-velocity trend must be recorded
    int tens;      // Indicates whether the shear-stress trend must be recorded
    int viscg;     // Indicates whether the gas-viscosity trend must be recorded
    int rhog;      // Indicates whether the gas-density trend must be recorded
    int masg;      // Indicates whether the gas mass-flow-rate trend must be recorded
    int masl;      // Indicates whether the liquid mass-flow-rate trend must be recorded
    int hidro;     // Indicates whether the hydrostatic-term trend must be recorded
    int fric;      // Indicates whether the friction-term trend must be recorded
    int FVHG;      // Indicates whether the service-line hydrate volume-fraction trend must be recorded
    int calor;     // Indicates whether the heat-flux trend must be recorded
    int qgst;      // Indicates whether the standard gas-flow-rate trend must be recorded
    int velgarg;   // Indicates whether the gas-velocity trend through the GLV orifice must be recorded
    int pEstagVGL; // Indicates whether the upstream GLV pressure trend must be recorded
    int tEstagVGL; // Indicates whether the upstream GLV temperature trend must be recorded
    int pGargVGL;  // Indicates whether the downstream GLV pressure trend must be recorded
    int tGargVGL;  // Indicates whether the downstream GLV temperature trend must be recorded
    int qVGL;      // Indicates whether the GLV gas-flow-rate trend must be recorded

    int reyi;   // Indicates whether the internal Reynolds-number trend must be recorded
    int reye;   // Indicates whether the ambient Reynolds-number trend must be recorded
    int grashi; // Indicates whether the internal Grashof-number trend must be recorded
    int grashe; // Indicates whether the external Grashof-number trend must be recorded
    int nusi;   // Indicates whether the internal Nusselt-number trend must be recorded
    int nuse;   // Indicates whether the external Nusselt-number trend must be recorded
    int hi;     // Indicates whether the internal film-coefficient trend must be recorded
    int he;     // Indicates whether the external film-coefficient trend must be recorded
    int pri;    // Indicates whether the internal Prandtl-number trend must be recorded
    int pre;    // Indicates whether the external Prandtl-number trend must be recorded
    int comp;   // Position where the trend is recorded, referenced from the well bottom
    int diamInt;
    int TempParede;

    int subResfria;
    string rotulo; // Label
};

// Defines a temperature trend at a specific point in a cross section
struct detTRENDTrans {
    double dt;     // Time interval between trend records
    int posic;     // Cell in which the trend is recorded
    int camada;    // Cross-section layer in which the trend is recorded
    int discre;    // Discretization position within the layer
    int comp;      // Position in meters where the cell is located
    string rotulo; // Label
};

// Defines the limits used to generate fluid-property tables
struct detTabelaEntalp {
    int npont;   // Number of table points
    int npontB;  // Number of table points
    double pmax; // Maximum pressure, in kgf/cm2
    double pmin; // Minimum pressure, in kgf/cm2
    double tmax; // Maximum temperature, in degrees Celsius
    double tmin; // Minimum temperature, in degrees Celsius
};

// Supports event registration in the log file by identifying the event, its start and end,
// and the states changed during the event
struct LogEvento {
    double instante;  // Time at which the event begins
    double duracao;   // Event duration
    string descricao; // Event description
    double estIni;    // State before the event begins
    double estFim;    // State after the event ends
    bool operator<(const LogEvento &e) const {
        if (this->instante < e.instante)
            return true;
        else
            return false;
    }
};

// Defines injector-well boundary conditions and whether the injected fluid is water, CO2,
// or a user-defined liquid. The injection model does not support steady-state conditions
struct detCondConInjec {
    int CC; // Boundary-condition type:
    // Injection flow rate and bottomhole IPR, CC0
    // Injection pressure and bottomhole IPR, CC1
    // Bottomhole pressure and bottomhole IPR, CC2
    // Injection pressure and bottomhole pressure, CC3
    // Injection flow rate and injection pressure, CC4
    // Injection flow rate and bottomhole pressure, CC5
    int tipoFlui;        // Fluid type: 0 = user-defined, 1 = water, 2 = table-based
    double salin;        // Water salinity
    double tempinj;      // Injection temperature, in degrees Celsius
    double vazinj;       // Injection flow rate, in Sm3/d
    double presinj;      // Injection pressure, in kgf/cm2
    double presfundo;    // Bottomhole pressure, in kgf/cm2
    string pvtsimarqInj; // PVTSim file used for CO2-rich gas injection
};

// Defines the thermal properties of a well lithological layer
struct detFormacao {
    int id;
    double tempo; // Well production time, in days
    double cond;  // Formation thermal conductivity, in W/mK
    double cp;    // Formation specific heat, in J/kgK
    double rho;   // Formation density, in kg/m3
};

// Contains fluid properties obtained from a PVTSim table
struct tabelaFlash {
    int id;
    int TwoOrThree; // Indicates whether the table represents two or three phases
    string arquivo; // PVTSim file name
    int visc;       // Selects table-based or black-oil viscosity calculation
    // 0 = table values, 1 = black-oil model
    // Tables obtained directly from the PVTSim file
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
    double **viscO;
    double **viscG;
    double *PBF;
    double *TBF;
};

struct composicional {
    string arquivo;
    int npseudo;
    int liqModel;
    int vapModel;
    double *fracMol;
    double *masMol;
    double *tempCrit;
    double *presCrit;
    double *fatAcent;
    double *parac;
    double *TIndepPeneloux;
    double *kij;
    double *lij;
    double *CpIGCoefs;
    ~composicional() {
        if (npseudo > 0) {
            delete[] fracMol;
            delete[] CpIGCoefs;
            delete[] TIndepPeneloux;
            delete[] fatAcent;
            delete[] kij;
            delete[] lij;
            delete[] masMol;
            delete[] parac;
            delete[] presCrit;
            delete[] tempCrit;
        }
    }
};

struct detTabVisc {
    int parserie;
    double *visc;
    double *temp;
};

struct dadosParafina {
    string arquivo;
    int poroRey;
    double valRey;
    int C2C3;
    double valC2;
    double valC3;
    int difus;
    int alteraViscFlu;
    double multDifus;
    double rug;
    double multVis;
    double DViscWax;
    double EViscWax;
    double FViscWax;
    double DmultipWax;
    double EmultipWax;
    double FmultipWax;
};

struct acopDuto3D {
    int indCel;
    string rotulo;
    double FE;
    double comp;
};

struct detCelUnit {
    int posicP;
    double comp;
    int parserie;
    double *tempo;
};

class Ler {
  public:
    int tipoHmodel;
    double MMH, MMG, MMW, Whamm;
    double coefEsteq;
    string inibidor;
    double fracFWcarregada;
    string estruturaHidratos;
    double rd, rp;

    int saidaClassica; // Defines how the end-of-simulation routine is called; presentation-only setting
    int cicloAcopTerm; // Enables an additional iteration to account for the dT/dt term
    // in the mass-conservation equations; 1 = include dT/dt
    int flashCompleto; // Selects PVTSim tables or the black-oil model; 1 = PVTSim
    int tabelaDinamica;
    int usaTabela;      // Indicates that a table is being used by either the black-oil or PVTSim model
    int ConContEntrada; // Defines the production-system inlet boundary condition: 0 = closed
    // 1 = pressure, 2 = inlet flow rate and pressure
    string impfile;   // JSON file
    string pvtsimarq; // PVTSim file
    int modelcp;      // When 1, uses Cp values from a PVTSim table even with a black-oil model
    int modelJTL;     // When 1, uses Drhol/dT values from a PVTSim table even with a black-oil model
    int nmaterial;    // Number of materials registered in the JSON file
    int ncorte;       // Number of cross sections registered in the JSON file
    int nduto;        // Number of pipes registered in the JSON file
    int nunidadep;    // Number of production units registered in the JSON file
    int ncelp;        // Number of cells in the production system
    int lingas;       // When 1, enables a service line coupled to the production system
    int nunidadeg;    // Number of service-line units registered in the JSON file
    int ncelg;        // Number of cells in the gas system
    int nipr;         // Number of IPRs registered in the JSON file
    int nvalvgas;     // Number of GLVs registered in the JSON file
    int nvalv;        // Number of valves registered in the JSON file
    int nfontechk;    // Not used
    int ninjgas;      // Number of gas sources registered in the JSON file
    int ninjliq;      // Number of liquid sources registered in the JSON file
    int ninjmass;     // Number of mass sources registered in the JSON file
    int nPoroRad;     // Number of radial porous-medium sources registered in the JSON file
    int nPoro2D;      // Number of 2D porous-medium sources registered in the JSON file
    int nfuro;        // Number of pressure sources registered in the JSON file
    int nbcs;         // Number of ESPs registered in the JSON file
    int nmultibcs;
    int nbvol;         // Number of positive-displacement pumps registered in the JSON file
    int ndpreq;        // Number of pressure increases registered in the JSON file
    int ncalor;        // Number of heat sources registered in the JSON file
    int npig;          // Number of pigs registered in the JSON file
    int nperfisp;      // Number of production profiles to be written
    int nperfisg;      // Number of service-line profiles to be written
    int nperfistransp; // Number of production cross-section thermal profiles to be written
    int nperfistransg; // Number of gas-line cross-section thermal profiles to be written
    int ntendp;        // Number of production-line trend files to be written
    int ntendg;        // Number of gas-line trend files to be written
    int ntendtransp;   // Number of temperature-trend files
    // to be written for production-line cross sections
    int ntendtransg; // Number of temperature-trend files
    // to be written for gas-line cross sections
    int ntela;     // Number of variables displayed on screen during simulation
    int equilterm; // When 1 and the initial condition is user-defined, assumes that the pipe-wall
    // thermal profile is in equilibrium with the environment
    int latente; // Accounts for latent-heat effects when using a black-oil model
    // Requires a PVTSim file from which liquid and gas enthalpies are obtained
    int condlatente;
    int tabp; // For black-oil production-line simulations, builds a compressibility-factor table and derivatives
    // before the simulation begins
    int tabg; // For black-oil gas-line simulations, builds a compressibility-factor table and derivatives
    // before the simulation begins
    int transmass; // Selects the mass-transfer model: 0 = complete model
    // 1 = explicit model, 2 = simplified low-relevance model
    int trackRGO;  // When 1, tracks local GOR and related variables such as API gravity and BSW
    detRGO varRGO; // Currently unused
    int trackDeng; // When 1, tracks gas density
    // and related variables such as the CO2 fraction
    int perm;                          // When 1, calculates the steady-state solution before starting the simulation
    int snaps;                         // When 1, reads the initial condition from a data file
    origemGeometria_t origemGeometria; // Defines whether production-line geometry in JSON is referenced
    // from the platform or well bottom; internally, the reference is always the well bottom
    sistemaSimulacao_t sistemaSimulacao;  // Indicates whether the system represents production or injection
    bool sentidoGeometriaSegueEscoamento; // Currently unused
    int descarga;                         // When 1, enables a gas-lift unloading simulation
    double salinDescarga;                 // Completion-fluid salinity in a gas-lift unloading simulation
    double compInterDesc;                 // Initial gas/completion-fluid interface position, referenced from the platform
    // in the service line during a gas-lift unloading simulation
    double compInterDescP; // Initial gas/completion-fluid interface position, referenced from the well bottom
    // in the production line during a gas-lift unloading simulation
    int celdescarga; // Initial interface cell index, referenced from the platform
    // in the service line during a gas-lift unloading simulation
    int celdescargaP; // Initial interface cell index, referenced from the well bottom
    // in the production line during a gas-lift unloading simulation

    int controDesc; // When 1, enables automatic gas-lift unloading control
    // to prevent erosional velocities at the GLVs during unloading
    double vazDescControl;    // Maximum liquid-flow-rate limit at the valve, in m3/d
    double presMaxDesc;       // Maximum pressure downstream of the production choke during unloading
    double presMinDesc;       // Minimum pressure downstream of the production choke during unloading
    double presIniDesc;       // Downstream production-choke pressure at which unloading begins
    double tempDescarga;      // Unloading temperature
    double presMaxDescG;      // Maximum injection pressure during unloading
    double presMinDescG;      // Minimum injection pressure during unloading
    double presIniDescG;      // Initial gas-line pressure when unloading begins
    double tempoLatenciaDesc; // Delay between simulation stabilization and the start of unloading

    string snapshotArqIn;             // Input data file used when the initial condition is file-based
    int ordperm;                      // Steady-state solution order: 1 = first order, 2 = second order
    int transiente;                   // When 1, enables the transient simulation
    int imaxDG;                       // Currently unused
    int iminDG;                       // Currently unused
    int pocinjec;                     // When 1, enables injector-well simulation
    double tfinal;                    // Total simulation time
    double dtmax;                     // User-defined time-step limit at simulation startup
    double anmP;                      // ANM position along the production line, referenced from the well bottom
    double anmG;                      // ANM position along the service line, referenced from the platform
    double mono;                      // Minimum void-fraction or holdup value at which the problem is treated as single-phase
    double critcond;                  // Minimum void-fraction or holdup value below which the mass-transfer model is disabled
    int corrDeng;                     // When 1, distinguishes dissolved-gas density from free-gas density in the black-oil model
    vector<double> evento;            // Times at which events occur
    vector<LogEvento> logevento;      // All events, including profile output, with descriptions and state changes
    vector<LogEvento> logeventoEstat; // Accessory-related events only, excluding profile output
    // Includes their descriptions and state changes

    detTMAX dtmaxserie; // Time series containing maximum time-step limits
    int nfluP;          // Number of production fluids
    int indfluPIni;     // Production-fluid index used to initialize the simulation when the steady-state
    // solution is not requested
    int chkv; // When 1, indicates the presence of a surface check valve
    int *indFlup;
    ProFlu *flup;               // Production-fluid array
    ProFluCol fluc;             // Complementary-fluid object
    ProFlu flug;                // Gas-fluid object used by the service line or a gas source
    detTabelaEntalp tabent;     // Pressure and temperature limits for the PVTSim table
    detTabelaEntalp tabentCrit; // Reduced-pressure and reduced-temperature limits for the gas
    // compressibility factor and its derivatives
    material *mat;         // Materials registered in the JSON file
    corteduto *corte;      // Cross sections registered in the JSON file
    detduto *duto;         // Pipe data registered in the JSON file
    detalhaP *unidadeP;    // Production units registered in the JSON file
    detcelp *celp;         // Production-cell structures
    detalhaG *unidadeG;    // Service-line units registered in the JSON file
    detcelg *celg;         // Service-cell structures
    detIPR *IPRS;          // IPR structures registered in the JSON file
    detGASINJ gasinj;      // Gas-lift injection system and boundary-condition data
    detVALVGL *valvgl;     // GLV definitions registered in the JSON file
    detValv *valv;         // Valve definitions registered in the JSON file
    detFONGAS *fonteg;     // Gas-source definitions registered in the JSON file
    detFONLIQ *fontel;     // Liquid-source definitions registered in the JSON file
    detFONMASS *fontem;    // Mass-source definitions registered in the JSON file
    detPoroRad *porosoRad; // Radial porous-medium source definitions registered in the JSON file
    detPoro2D *poroso2D;   // 2D porous-medium source definitions registered in the JSON file
    detFURO *furo;         // Pressure-source definitions registered in the JSON file
    detBCS *bcs;           // ESP definitions registered in the JSON file
    detMultiBCS *multiBcs;
    detBVOL *bvol;      // Positive-displacement pump definitions registered in the JSON file
    detDPREQ *dpreq;    // Pressure-increase definitions registered in the JSON file
    detCalor *fonteCal; // Heat-source definitions registered in the JSON file
    detMASTER1 master1; // Master1 time-series data
    detMASTER1 master2; // Master2 time-series data
    detPSEP psep;       // Platform-pressure time-series data
    detCorrec correcao;
    detCHOKESUP chokep;         // Surface-choke time-series data
    detCHOKESUP *fontechk;      // Not used
    detCHOKESUP chokes;         // Gas-injection choke time-series data
    detPig *pig;                // Pig launch times and positions registered in the JSON file
    detPROFP profp;             // Production-profile output settings
    int nvarprofp;              // Number of variables with production profiles enabled
    detPROFG profg;             // Service-profile output settings
    int nvarprofg;              // Number of variables with service profiles enabled
    detPROFTRANS proftransp;    // Production-system cross-section temperature-profile output settings
    detPROFTRANS proftransg;    // Injection-system cross-section temperature-profile output settings
    detTRENDP *trendp;          // Production-trend variables and output positions
    int *nvartrendp;            // Number of variables written to each production trend
    detTRENDG *trendg;          // Service-trend variables and output positions
    int *nvartrendg;            // Number of variables written to each service trend
    detTRENDTrans *trendtransp; // Production-system cross-section temperature-trend positions
    detTRENDTrans *trendtransg; // Gas-system cross-section temperature-trend positions
    detTela *tela;              // Variables displayed on screen
    detCondConInjec condpocinj; // Injector-well boundary conditions
    int nform;                  // Number of lithologies registered in the JSON file
    detFormacao *formacPoc;     // Thermal data for lithologies registered in the JSON file
    detPresEnt CCPres;          // Pipe-inlet pressure boundary-condition data
    detVazPresEnt CCVPres;      // Pipe-inlet pressure and flow-rate boundary-condition data
    tabelaFlash *flash;         // Fluid-property tables obtained from a PVTSim file
    composicional *compDet;
    int MedSimpPresFront; // Defines how boundary pressures are calculated
    // When 1, average values are used
    double limTransMass; // Pressure threshold below which a simpler, more stable mass-transfer model is used
    // This improves stability at very low pressures, particularly during shut-ins
    int RelaxaDTChoke;          // Penalizes the time step when surface-choke liquid-flow oscillations are detected
    double CriterioConvergPerm; // Steady-state convergence criterion used by a stricter convergence model
    // This option is currently used infrequently because of AceleraConvergPerm
    int AceleraConvergPerm; // Enables a simplified, faster, and more stable steady-state solver
    // 1 = use the convergence accelerator
    int escorregamentoCelulaContorno; // Controls phase-slip modeling in the final production-system volume
    // Slip may be disabled there to reduce high-frequency flow-pattern oscillations
    int correcaoContracorPerm; // When 1, applies an approximate countercurrent heat-transfer correction
    // to the annular-gas inlet temperature in the steady-state solver; the transient model treats it explicitly

    // Total production-line length
    double nCompTotalUnidadesP;
    // Total service-line length
    double nCompTotalUnidadesG;

    DadosGeo *dutosMRT; // Pipe objects converted for use by cell objects
    // in the celula3 class

    int eventoabre; // Number of Master1 opening events used for opening monitoring
    // and time-step restrictions during these events
    double *Tevento; // Times at which Master1 opening begins
    int eventofecha; // Number of Master1 closing events used for closing monitoring
    // and time-step restrictions during these events
    double *Teventof;   // Times at which Master1 closing begins
    int eventoabrePXO;  // Not used
    double *TeventoPXO; // Not used
    double areagargPXO; // Not used

    int npontos;      // Number of points in black-oil and PVTSim tables
    double **zdranP;  // Black-oil compressibility-factor table indexed by reduced pressure and temperature
    double **dzdpP;   // Black-oil table containing the derivative of Z with respect to pressure
    double **dzdtP;   // Black-oil table containing the derivative of Z with respect to temperature
    double **cpg;     // Black-oil gas-specific-heat table indexed by temperature
    double **cpl;     // Black-oil liquid-specific-heat table indexed by temperature
    double **drholdT; // Black-oil table containing the derivative of liquid density with respect to temperature

    // Pressure in kgf/cm2 and temperature in degrees Celsius
    double **RhoInj;    // Injector-well fluid-density table obtained from PVTSim, in kg/m3
    double **ViscInj;   // Injector-well fluid-viscosity table obtained from PVTSim, in Pa.s
    double **CondInj;   // Injector-well fluid thermal-conductivity table obtained from PVTSim, in W/mK
    double **CpInj;     // Injector-well fluid specific-heat table obtained from PVTSim, in J/kgK
    double **DrhoDtInj; // Injector-well table containing the density derivative with respect to temperature, in kg/m3/K

    int nvecEmul;    // Number of user-defined BSW and viscosity-multiplier pairs for the emulsion model
    double *BSWVec;  // User-defined emulsion or BSW values
    double *emulVec; // User-defined viscosity multipliers

    int escorregaPerm; // Controls phase slip in the steady-state model; 1 enables slip by default
    int escorregaTran; // Controls phase slip in the transient model; 1 enables slip by default

    int tabRSPB; // Indicates whether a black-oil solution gas-oil ratio table is generated before simulation
    // This improves performance, particularly for Livia's computationally intensive RS model

    // Slip-correlation selectors
    int CorreEstrat; // Allowed values: 0, 1, 2, or 4
    int CorreDisper; // Allowed values: 0, 1, or 4
    int CorreAnular; // Allowed values: 0, 1, 3, or 4

    int mapaArranjo;
    // Flow-pattern map type
    // 0 = simplified Barnea
    // 1 = complete Barnea

    int saidaTela; // Defines the screen output displayed during simulation
    // 0 = standard percentage-only output; other modes include time, time step, and variable details
    int idiomaSaida; // Output-file language: 0 = pt-BR, 1 = English

    double valTempChokeJus; // Stores the temperature downstream of the surface choke
    // for optional trend-graph output

    int nsnp; // Number of simulation snapshots to be recorded
    // Used when the user requests that selected simulation moments be saved
    double *tempsnp; // User-defined times at which snapshots are recorded

    int nTcorrecaoMassaEspLiq;
    double *TcorrecaoMassaEspLiq;
    int *VcorrecaoMassaEspLiq;
    int nTsonico;
    double *Tempsonico;
    int *Vsonico;
    int correcaoMassaEspLiq;
    int desligaDeriTransMassDTemp;
    int corrigeContSep;
    double taxaDespre;
    int acopColAnulPermForte;
    int tipoFluido;
    int mudaArea;
    int npseudo;

    double *compLinServ; // Gas composition in the service line when the production line uses a compositional model
    // Also stores the gas-source composition used to calculate the production-line mixture after injection

    int miniTabAtraso;
    double miniTabDp;
    double miniTabDt;

    double tempReves;
    double razCompGasReves;

    int AP;
    int paralelAP;
    string arquivoAP;

    detTabVisc *tabVisc;

    int nthrd;

    double buscaFC;

    double chutePerm;

    int HISEP;

    int reverso;

    int modoSegrega;
    double *vecTSegrega;
    int *vecSegrega;
    int nsegrega;

    int estabCol;

    varGlob1D *vg1dSP;

    int modoXY;
    double xProd0;
    double yProd0;
    double xServ0;
    double yServ0;

    int modoParafina;
    dadosParafina detalParafina;

    double *oPressurePoints;
    double *oCloudPointTemperatures;
    double *oStructurePressures;
    double *oStructureTemperatures;
    double *oStructureDensities;
    double *oStructureLiqMW;
    double *oStructureCPWax;
    double *oStructureThermCond;
    double *oStructureWaxConcs;
    double *oMolecularWeightsOfWaxComponents;
    double *oLiquidDensitiesOfWaxComponents;
    double *oInterpolatedWaxConcs;

    int npseudoWax;
    int iPresWax;
    int iTempWax;

    int modoDifus3D;
    int threadP3D;
    string modoDifus3DJson;
    int nacop;
    vector<acopDuto3D> celAcop;
    vector<DadosGeo> geoAcop;

    int desligaPenalizaDT;
    int controleDTvalv;

    int indicaBeta;

    int conINIAmbP;
    int conINIAmbS;

    int redeperm;

    double **envelopeHidra;
    int existeEnvelope;
    int calculaEnvelope;
    int nparEnvelope;

    double pmin;
    double tmin;

    int tipoModeloDrift;
    double JTLiquidoSimple;

    int nintermi;
    detIntermi *intermi;

    int nCelUnit;
    detCelUnit* celUnit;

    /*
     * Default Constructor
     */
    Ler();
    /*
     * Fields Constructor
     */
    Ler(const string IMPFILE, const string ARQUIVO_LOG, const tipoValidacaoJson_t VALIDACAO_MRT,
        const tipoSimulacao_t SIMULACAO_MRT, int vreverso = 0, varGlob1D *Vvg1dSP = 0, int vredeperm = 1);
    /*
     * Copy Constructor
     */
    Ler(const Ler &);
    /*
     * Assignment Operator
     */
    Ler &operator=(const Ler &);
    /*
     * Destructor
     */
    ~Ler() {

        if (dtmaxserie.parserie > 0) {
            delete[] dtmaxserie.tempo;
            delete[] dtmaxserie.valor;
        }

        if (nfluP > 0) {
            delete[] flup;
            delete[] indFlup;
        }

        if (nform > 0)
            delete[] formacPoc;

        if (nmaterial > 0)
            delete[] mat;

        if (ncorte > 0) {
            for (int i = 0; i < this->ncorte; i++) {
                delete[] corte[i].diam;
                delete[] corte[i].indmat;
                delete[] corte[i].discre;
            }
            delete[] corte;
        }

        if (nduto > 0)
            delete[] duto;

        if (nunidadep > 0) {
            for (int i = 0; i < nunidadep; i++) {
                delete[] unidadeP[i].dx;
                delete[] unidadeP[i].dxVar;
                for (int j = 0; j < 12; j++)
                    delete[] unidadeP[i].var[j];
                delete[] unidadeP[i].var;
                if (modoDifus3D == 1) {
                    delete[] unidadeP[i].difusTerm3D;
                    delete[] unidadeP[i].difusTerm3DFE;
                    delete[] unidadeP[i].difusTerm3DAcop;
                }
            }
            delete[] unidadeP;
        }

        if (nunidadeg > 0 && lingas > 0) {
            for (int i = 0; i < nunidadeg; i++) {
                delete[] unidadeG[i].dx;
                delete[] unidadeG[i].dxVar;
                for (int j = 0; j < 9; j++)
                    delete[] unidadeG[i].var[j];
                delete[] unidadeG[i].var;
            }
            delete[] unidadeG;
        }

        if (ncelp > 0)
            delete[] celp;

        if (ncelg > 0)
            delete[] celg;

        if (nparEnvelope > 0) {
            for (int i = 0; i < nparEnvelope; i++) {
                delete[] envelopeHidra[i];
            }
            delete[] envelopeHidra;
        }

        if (nipr > 0) {
            for (int i = 0; i < this->nipr; i++) {
                delete[] IPRS[i].pres;
                delete[] IPRS[i].tpres;
                delete[] IPRS[i].temp;
                delete[] IPRS[i].ttemp;
                if (IPRS[i].tipoIPR == 0 || IPRS[i].tipoIPR == 1) {
                    delete[] IPRS[i].ip;
                    delete[] IPRS[i].tip;
                } else {
                    delete[] IPRS[i].qMax;
                    delete[] IPRS[i].tqMax;
                }
                delete[] IPRS[i].jp;
                delete[] IPRS[i].tjp;
            }
            delete[] IPRS;
        }
        if (ConContEntrada == 1) {
            delete[] CCPres.temperatura;
            delete[] CCPres.pres;
            delete[] CCPres.tit;
            delete[] CCPres.bet;
            delete[] CCPres.tempo;
        } else if (ConContEntrada == 2) {
            delete[] CCVPres.temperatura;
            delete[] CCVPres.pres;
            delete[] CCVPres.mass;
            delete[] CCVPres.bet;
            delete[] CCVPres.tempo;
        }

        if (lingas > 0 && controDesc == 0) {
            delete[] gasinj.tempo;
            if (gasinj.tipoCC == 1)
                delete[] gasinj.vazgas;
            else if (gasinj.tipoCC == 0) { // Initial-guess adjustment
                delete[] gasinj.presinj;
                delete[] gasinj.vazgas;
            } // Initial-guess adjustment
            delete[] gasinj.temperatura;

            if (chokes.parserie > 0) {
                delete[] chokes.tempo;
                delete[] chokes.abertura;
            }
        }

        if (nvalvgas > 0 && lingas > 0)
            delete[] valvgl;

        if (nvalv > 0) {
            for (int i = 0; i < this->nvalv; i++) {
                if (valv[i].ncv > 0)
                    delete[] valv[i].cvCurv;
                delete[] valv[i].abertura;
                delete[] valv[i].tempo;
            }
            delete[] valv;
        }

        if (nfontechk > 0) {
            for (int i = 0; i < this->nfontechk; i++) {
                delete[] fontechk[i].tempo;
                delete[] fontechk[i].abertura;
            }
            delete[] fontechk;
        }

        if (ninjgas > 0) {
            for (int i = 0; i < this->ninjgas; i++) {
                delete[] fonteg[i].temp;
                delete[] fonteg[i].vazgas;
                delete[] fonteg[i].vazcomp;
                delete[] fonteg[i].tempo;
            }
            delete[] fonteg;
        }

        if (ninjliq > 0) {
            for (int i = 0; i < this->ninjliq; i++) {
                delete[] fontel[i].temp;
                delete[] fontel[i].bet;
                delete[] fontel[i].vazliq;
                delete[] fontel[i].tempo;
            }
            delete[] fontel;
        }

        if (ninjmass > 0) {
            for (int i = 0; i < this->ninjmass; i++) {
                delete[] fontem[i].temp;
                delete[] fontem[i].vazMasT;
                delete[] fontem[i].vazMasC;
                delete[] fontem[i].vazMasG;
                delete[] fontem[i].tempo;
            }
            delete[] fontem;
        }

        if (nPoroRad > 0) {
            delete[] porosoRad;
        }

        if (nPoro2D > 0) {
            delete[] poroso2D;
        }

        if (nfuro > 0) {
            for (int i = 0; i < this->nfuro; i++) {
                delete[] furo[i].abertura;
                delete[] furo[i].tempo;

                delete[] furo[i].check;
                delete[] furo[i].tempoChk;
            }
            delete[] furo;
        }

        if (nbcs > 0) {
            for (int i = 0; i < this->nbcs; i++) {
                delete[] bcs[i].tempo;
                delete[] bcs[i].freq;
                delete[] bcs[i].vaz;
                delete[] bcs[i].head;
                delete[] bcs[i].power;
                delete[] bcs[i].efic;
            }
            delete[] bcs;
        }

        if (nmultibcs > 0)
            delete[] multiBcs;

        if (nbvol > 0) {
            for (int i = 0; i < this->nbvol; i++) {
                delete[] bvol[i].tempo;
                delete[] bvol[i].freq;
            }
            delete[] bvol;
        }

        if (ndpreq > 0) {
            for (int i = 0; i < this->ndpreq; i++) {
                delete[] dpreq[i].dp;
                delete[] dpreq[i].tempo;
            }
            delete[] dpreq;
        }

        if (ncalor > 0) {
            for (int i = 0; i < this->ncalor; i++) {
                delete[] fonteCal[i].cal;
                delete[] fonteCal[i].tempo;
            }
            delete[] fonteCal;
        }

        if (npig > 0)
            delete[] pig;

        int nevento = master1.parserie;
        delete[] master1.tempo;
        delete[] master1.abertura;
        if (master1.ncv > 0)
            delete[] master1.cvCurv;

        if (lingas > 0) {
            delete[] master2.tempo;
            delete[] master2.abertura;
        }

        if (psep.parserie > 0) {
            delete[] psep.tempo;
            delete[] psep.pres;
        }

        if (correcao.parserieHidro > 0)
            delete[] correcao.dPdLHidro;
        if (correcao.parserieFric > 0)
            delete[] correcao.dPdLFric;
        if (correcao.parserieDT > 0)
            delete[] correcao.dTdL;

        if (chokep.parserie > 0) {
            delete[] chokep.tempo;
            delete[] chokep.abertura;
            if (chokep.ncv > 0)
                delete[] chokep.cvCurv;
        }

        if (nperfisp > 0)
            delete[] profp.tempo;
        if (nperfisg > 0)
            delete[] profg.tempo;
        if (nperfistransp > 0) {
            delete[] proftransp.posic;
            delete[] proftransp.tempo;
        }
        if (nperfistransg > 0 && lingas > 0) {
            delete[] proftransg.posic;
            delete[] proftransg.tempo;
        }
        if (ntendp > 0) {
            delete[] trendp;
            delete[] nvartrendp;
        }
        if (ntendg > 0) {
            delete[] trendg;
            delete[] nvartrendg;
        }
        if (ntendtransp > 0)
            delete[] trendtransp;
        if (ntendtransg > 0)
            delete[] trendtransg;
        if (nduto > 0)
            delete[] dutosMRT;

        if (nevento > 0)
            delete[] Tevento;
        if (nevento > 0)
            delete[] Teventof;

        if (ntela > 0)
            delete[] tela;

        if (tabp > 0 || tabg > 0) {
            for (int i = 0; i < npontos + 2; i++)
                delete[] zdranP[i];
            delete[] zdranP;
            for (int i = 0; i < npontos + 2; i++)
                delete[] dzdpP[i];
            delete[] dzdpP;
            for (int i = 0; i < npontos + 2; i++)
                delete[] dzdtP[i];
            delete[] dzdtP;
        }
        int ndiv = tabent.npont - 1;
        if (modelcp > 0 && cpg != 0 && cpl != 0) {
            for (int i = 0; i < ndiv + 2; i++) {
                delete[] cpg[i];
                delete[] cpl[i];
            }
            delete[] cpg;
            delete[] cpl;
        }
        if (modelJTL == 1 && drholdT != 0) {
            for (int i = 0; i < ndiv + 2; i++) {
                delete[] drholdT[i];
            }
            delete[] drholdT;
        }
        if (flashCompleto == 1) {
            for (int i = 0; i < nfluP; i++) {
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].rholF[j];
                delete[] flash[i].rholF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].rhogF[j];
                delete[] flash[i].rhogF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].DrholDpF[j];
                delete[] flash[i].DrholDpF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].DrhogDpF[j];
                delete[] flash[i].DrhogDpF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].DrholDtF[j];
                delete[] flash[i].DrholDtF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].DrhogDtF[j];
                delete[] flash[i].DrhogDtF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].DrhowDtF[j];
                delete[] flash[i].DrhowDtF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].RSF[j];
                delete[] flash[i].RSF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].cplF[j];
                delete[] flash[i].cplF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].cpgF[j];
                delete[] flash[i].cpgF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].cpwF[j];
                delete[] flash[i].cpwF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].HlF[j];
                delete[] flash[i].HlF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].HgF[j];
                delete[] flash[i].HgF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].sigOGF[j];
                delete[] flash[i].sigOGF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].sigWGF[j];
                delete[] flash[i].sigWGF;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].viscO[j];
                delete[] flash[i].viscO;
                for (int j = 0; j < tabent.npont + 1; j++)
                    delete[] flash[i].viscG[j];
                delete[] flash[i].viscG;

                delete[] flash[i].PBF;
                delete[] flash[i].TBF;
            }
            delete[] flash;
        }
        if (flashCompleto == 2) {
            if (nfluP > 0)
                delete[] compDet;
            if (pocinjec == 0 && (lingas == 1 || ninjgas > 0))
                delete[] compLinServ;
        }
        if (pocinjec > 0 && condpocinj.tipoFlui == 2) {
            if (RhoInj!=0) {
                for (int i = 0; i < ndiv + 2; i++)
                    delete[] RhoInj[i];
                delete[] RhoInj;
            }
            if (ViscInj!=0) {
                for (int i = 0; i < ndiv + 2; i++)
                    delete[] ViscInj[i];
                delete[] ViscInj;
            }
            if (CondInj!=0) {
                for (int i = 0; i < ndiv + 2; i++)
                    delete[] CondInj[i];
                delete[] CondInj;
            }
            if (CpInj!=0) {
                for (int i = 0; i < ndiv + 2; i++)
                    delete[] CpInj[i];
                delete[] CpInj;
            }
            if (DrhoDtInj!=0) {
                for (int i = 0; i < ndiv + 2; i++)
                    delete[] DrhoDtInj[i];
                delete[] DrhoDtInj;
            }
        }
        if (nvecEmul > 0) {
            delete[] BSWVec;
            delete[] emulVec;
        }
        if (nsnp > 0)
            delete[] tempsnp;
        if (nTcorrecaoMassaEspLiq > 0) {
            delete[] VcorrecaoMassaEspLiq;
            delete[] TcorrecaoMassaEspLiq;
        }
        if (nTsonico > 0) {
            delete[] Vsonico;
            delete[] Tempsonico;
        }

        if (tabVisc != 0) {
            for (int i = 0; i < nfluP; i++) {
                if (tabVisc[i].parserie > 0) {
                    delete[] tabVisc[i].visc;
                    delete[] tabVisc[i].temp;
                }
            }
            delete[] tabVisc;
        }
        if (nsegrega > 0) {
            delete[] vecTSegrega;
            delete[] vecSegrega;
        }

        if (oPressurePoints != 0)
            delete[] oPressurePoints;
        if (oCloudPointTemperatures != 0)
            delete[] oCloudPointTemperatures;
        if (oStructurePressures != 0)
            delete[] oStructurePressures;
        if (oStructureTemperatures != 0)
            delete[] oStructureTemperatures;
        if (oStructureDensities != 0)
            delete[] oStructureDensities;
        if (oStructureLiqMW != 0)
            delete[] oStructureLiqMW;
        if (oStructureCPWax != 0)
            delete[] oStructureCPWax;
        if (oStructureThermCond != 0)
            delete[] oStructureThermCond;
        if (oStructureWaxConcs != 0)
            delete[] oStructureWaxConcs;
        if (oMolecularWeightsOfWaxComponents != 0)
            delete[] oMolecularWeightsOfWaxComponents;
        if (oLiquidDensitiesOfWaxComponents != 0)
            delete[] oLiquidDensitiesOfWaxComponents;
        if (oInterpolatedWaxConcs != 0)
            delete[] oInterpolatedWaxConcs;

        if (nintermi > 0) {
            delete[] intermi;
        }

        if (nCelUnit > 0) {
            for (int i = 0; i < this->nCelUnit; i++) {
                delete[] celUnit[i].tempo;
            }
            delete[] celUnit;
        }
    }

    void copiaSemJson(Ler &);
    void lerArq(); // Organizes JSON parsing and builds the cell-structure array
    // The resulting structures are later used by SisProd to construct celula3 objects
    void copiaArq(Ler &arqAntigo);

    void geraduto(); // Converts a detduto structure into a DadosGeo object used by celula3

    void novatrans(TransCal &transfer /*Heat-transfer object constructed by this method*/,
                   DadosGeo &geom /*Geometry-data object*/,
                   int *discre /*Number of discretizations in each wall layer, obtained
                    from a corteduto object*/
                   ,
                   double tint /*Internal pipe temperature*/,
                   double text /*External-environment temperature*/,
                   double vext /*External-environment velocity*/,
                   double vint /*Internal flow velocity used only to initialize the object*/,
                   long dircon /*External convection direction: crossflow against the pipe or parallel flow,
                    obtained from a detcelp or detcelg structure*/
                   ,
                   double dt /*Time-step value used only to initialize the object*/,
                   double ki /*Internal-fluid thermal conductivity used only to initialize the object*/, double cpi, double rhoi, double visci,
                   double ke /*External-environment thermal conductivity*/,
                   double cpe /*External-environment specific heat*/,
                   double rhoe /*External-environment density*/,
                   double visce /*External-environment viscosity*/,
                   int formac /*Indicates whether heat transfer with a formation is enabled*/,
                   int litologia /*Index selecting the lithology from the formacPoc array*/,
                   int ambext /*External-environment selector: user-defined, atmospheric, or marine*/,
                   double pres /*Internal pressure used only to initialize the object*/,
                   double dx /*Length of the cell associated with the TransCal object*/); // Constructs the
    // TransCal object assigned to cell objects

    int retornaNTab();
    void geracelp(Cel *celula);  // Generates the production-cell object array used by SisProd
    void geracelg(CelG *celula); // Generates the gas-cell object array used by SisProd
    int coluanulini();           // Determines where thermal coupling with the service line begins along the production line
    int coluanulfim();           // Determines where thermal coupling with the service line ends along the production line
    int anulcoluini();           // Determines where thermal coupling with the production line begins along the service line
    int anulcolufim();           // Determines where thermal coupling with the production line ends along the service line

    int acopPriRedeParalelaini(); // Determines where thermal coupling with secondary parallel-network lines begins on primary lines
    int acopPriRedeParalelafim(); // Determines where thermal coupling with secondary parallel-network lines ends on primary lines
    int acopSecRedeParalelaini(); // Determines where thermal coupling with primary lines begins on secondary parallel-network lines
    int acopSecRedeParalelafim(); // Determines where thermal coupling with primary lines ends on secondary parallel-network lines

    void geraipr(Cel *celula);     // Generates IPR source objects for SisProd
    void gerafgasVGL(Cel *celula); // Generates GLV gas-source objects in the production and service lines
    // and creates the GLV models used by SisProd
    void gerafgasFonte(Cel *celula);     // Generates production-line gas-source objects for SisProd
    void gerafliqFonte(Cel *celula);     // Generates production-line liquid-source objects for SisProd
    void gerafmassFonte(Cel *celula);    // Generates production-line mass-source objects for SisProd
    void gerafPoroRadFonte(Cel *celula); // Generates production-line radial porous-medium source objects for SisProd
    void gerafPoro2DFonte(Cel *celula);  // Generates production-line 2D porous-medium source objects for SisProd
    void geraFuro(Cel *celula);          // Generates production-line leakage objects for SisProd
    void gerafBCS(Cel *celula);          // Generates production-line ESP objects for SisProd
    void gerafmultiBCS(Cel *celula);
    void gerafBVOL(Cel *celula); // Generates production-line positive-displacement pump objects for SisProd
    void geraDPReq(Cel *celula); // Generates production-line pressure-increase objects for SisProd
    void geraFonteCalor(Cel *celula);
    void geraCalor(Cel *celula); // Generates production-line heat-source objects for SisProd
    void funcRazCV(double abertura, detCV *cvCurv, int ncurvaCV, double cdchk,
                   double AreaTub, double &razarea);
    void geraValv(Cel *celula);     // Generates production-line valve objects for SisProd
    void geraMaster1(Cel *celula);  // Generates the production-line Master1 valve object for SisProd
    void geraMaster2(CelG *celula); // Generates the service-line Master2 valve object for SisProd
    void gerapresfim(double &presfim /*Pressure upstream of the surface choke*/,
                     double &pGSup /*Pressure downstream of the choke*/); // Initializes the pressures upstream
    // and downstream of the choke
    void gerachokesup(choke &chokeSup);    // Creates the surface-choke object used by SisProd
    void gerachokeinj(ChokeGas &chokeInj); // Creates the injection-choke object used by SisProd
    void geraIntermitencia(Cel *celula);   // Generates severe-intermittency evaluation objects
    void indraz(int &ind /*Returns the time-series position immediately preceding the evaluated time*/,
                double &raz /*Relative time position between indices ind and ind + 1*/,
                double tempo /*Time at which interpolation is evaluated*/,
                int parserie /*Time-series length*/,
                double *serietemp /*Time-series values*/); // Obtains an interpolated value
    // from a time-series variable; used internally by atualiza
    void atualiza(int inicio /*Indicates whether the segment inlet is connected to another segment*/,
                  int extrem /*Indicates whether the pipe outlet is connected to another network pipe*/,
                  int anel /*Indicates whether the gas line is connected to a gas-lift ring*/,
                  choke &chokeSup /*Surface-choke object whose opening is updated by this method*/,
                  ChokeGas &chokeInj /*Injection-choke object whose opening is updated by this method*/,
                  Cel *celula /*Production-line cell array*/,
                  CelG *celulaG /*Gas-line cell array*/,
                  double &pGSup /*Pressure downstream of the surface choke*/,
                  double &temperatura /*Temperature at the production-line inlet
                   when a closed boundary condition is applied*/
                  ,
                  double &presiniG /*Gas-injection pressure*/,
                  double &tempiniG /*Gas-injection temperature*/,
                  double &presE /*Production-system inlet pressure for a pressure boundary condition*/,
                  double &tempE /*Production-system inlet temperature for a pressure boundary condition*/,
                  double &titE /*Production-system inlet quality for a pressure boundary condition*/,
                  double &betaE /*Production-system inlet beta for a pressure boundary condition*/,
                  double tempo /*Current simulation time*/, double dt = 0); // Updates the states of
    // production- and injection-system accessories and boundary conditions
    void atualizaSonico(double tempo, int &vExpli);

    // Methods that output data during the simulation
    void listaevento(int inic = 1, int extrem = 1);
    void geraevento(int inic = 1, int extrem = 1);

    double fqlst(Cel *const celula, int i, double tempo);
    double fqlstTot(Cel *const celula, int i, double tempo);
    double fqlwst(Cel *const celula, int i, double tempo);
    double fqgst(Cel *const celula, int i, double tempo);
    void imprimeProfile(Cel *const celula, FullMtx<double> &flut, double tempo, int indTramo, int nrede = -1);
    void imprimeProfileG(CelG *const celula, FullMtx<double> &flut, double tempo, int indTramo, int nrede = -1);
    void imprimeProfileTrans(Cel *const celula, int *length, double tempo, int indTramo, int nrede = -1);
    void imprimeProfileTransG(CelG *const celula, int *length, double tempo, int indTramo, int nrede = -1);
    void imprimeTrend(Cel *const celula, double **flut, double tempo, int trend, int linha);
    void imprimeTrendG(CelG *const celula, double **flut, double tempo,
                       int trend, int linha, double velgaslift = 0.); // Revision 5
    void imprimeTrendTrans(Cel *const celula,
                           double tempo, int trend);
    void resumoPermanente(Cel *const celula, CelG *const celulaG, double pGsup, double presiniG, int indTramo, int nrede = -1);
    void resumoIntermitencia(Cel *const celula, int indTramo, int nrede = -1);
    void relatorioCelulaUnitaria(Cel *const celula, int posic, int indTramo, int nrede = -1);

    // The following methods are used by lerArq while constructing production- and gas-cell
    // structures from the units and discretization data registered in JSON. They locate each
    // cell index using the unit length and the cumulative discretization lengths
    int buscaIndiceMeioP(double lverif);
    int buscaIndiceMeioG(double lverif);
    int buscaIndiceFrontP(double lverif);
    int buscaIndiceFrontG(double lverif);
    int buscaIndiceUnidade(int iniSeg, int nuni, double comp);
    int buscaIndiceUnidadeG(int iniSeg, int nuni, double comp);
    void geraTabCp();      // Builds the black-oil specific-heat table using PVTSim data
    void geraTabDrholDt(); // Builds the black-oil table for the derivative of liquid density
    // with respect to temperature using PVTSim data
    void geraTabInjCO2();                // Builds injector-well property tables when the injected fluid is a gas
    void geraTabFlash(int flu, int var); // Generates PVTSim fluid-property tables
    double interpolaTempEnvelope(double pres);
    std::string ler_linha_continua(std::ifstream &in);
    // Defines the simulation type: POCO_INJETOR, TRANSIENTE, or REDE
    tipoSimulacao_t tipoSimulacao;

  private:
    string arquivoLog;
    tipoValidacaoJson_t validacaoJson;
    void testaTipo();
    Document parseSchema();
    JSON_entrada parseEntrada();
    void validateVsSchema(Document *schemaMrtDoc, Document *mrtDoc);
    void iniciarVariaveis();
    void iniciarVariaveisConstrutorDefault();
    bool verificarUnicidade(const std::vector<int> &identificadores);
    int identificarMaterial(int identificador);
    int identificarCorte(int identificador);
    int identificarFormacao(int identificador);
    int identificarFluidoProducao(int identificador);
    int determinarLinhaErro(int posicaoErro);
    int determinarLinhaErro(const char *chaveJson);

    // Methods used to parse the MRT input file
    void parse_configuracao_inicial(JSON_entrada_configuracaoInicial &configuracao_inicial_json);
    void parse_condcont_pocinjec(JSON_entrada_CondicaoContPocInjec &condcont_json, int temFluido = 0);
    void parse_tempo(JSON_entrada_tempo &tempo_json);
    void parse_materiais(JSON_entrada_material &material_json);
    void parse_corte(JSON_entrada_secaoTransversal &corte_json);
    void parse_tabela(JSON_entrada_tabela &tabela_json);
    void parse_parafina(JSON_entrada_parafina &parafina_json);
    void parse_fluidos_producao(JSON_entrada_fluidosProducao &fluidos_producao_json);
    void parse_fluido_complementar(JSON_entrada_fluidoComplementar &fluido_complementar_json);
    void parse_fluido_gas(JSON_entrada_fluidoGas &fluido_gas_json);
    void parse_unidades_producao(JSON_entrada_dutosProducao &duto_producao_json);
    void parse_unidades_producaoAmb(JSON_entrada_dutosProducao &duto_producao_json);
    void parse_unidades_servico(JSON_entrada_dutosServico &duto_servico_json);
    void parse_unidades_servicoAmb(JSON_entrada_dutosServico &duto_servico_json);
    void parse_hidrato(JSON_entrada_hidrato &hidrato_json);
    void parse_ipr(JSON_entrada_ipr &ipr_json);
    void parse_separador(JSON_entrada_separador &separador_json);
    void parse_correcao(JSON_entrada_correcao &correcao_json);
    void parse_chokeSup(JSON_entrada_chokeSup &chokeSup_json);
    void parse_chokeInj(JSON_entrada_chokeInj &chokeInj_json);
    void parse_master1(JSON_entrada_master1 &master1_json);
    void parse_master2(JSON_entrada_master2 &master2_json);
    void parse_valv(JSON_entrada_valvula &valvula_json);
    void parse_gasInj(JSON_entrada_gasInj &gasInj_json);
    void parse_fonte_gas(JSON_entrada_fonteGas &fonte_gas_json);
    void parse_fonte_liquido(JSON_entrada_fonteLiquido &fonte_liquido_json);
    void parse_fonte_PoroRadial(JSON_entrada_fontePoroRadial &fonte_poroRadial_json);
    void parse_fonte_Poro2D(JSON_entrada_fontePoro2D &fonte_poro2D_json);
    void parse_fonte_massa(JSON_entrada_fonteMassa &fonte_massa_json);
    void parse_furo(JSON_entrada_fontePressao &fontePressao_json);
    void parse_fontechk(JSON_entrada_fonteChoke &fontechk_json);
    void parse_pig(JSON_entrada_pig &pig_json);
    void parse_bcs(JSON_entrada_bcs &bcs_json);
    void parse_multibcs(JSON_entrada_multibcs &multibcs_json);
    void parse_bomba_volumetrica(JSON_entrada_bombaVolumetrica &bomba_volumetrica_json);
    void parse_delta_pressao(JSON_entrada_deltaPressao &delta_pressao_json);
    void parse_fonteCalor(JSON_entrada_fonteCalor &fonteCalor_json);
    void troca_gaslift(detVALVGL &valv1, detVALVGL &valv2);
    void parse_fonte_gaslift(JSON_entrada_fonteGasLift &fonte_gaslift_json);
    void parse_intermitencia(JSON_entrada_intermitenciaSevera &intermitencia_json);
    void parse_celulaUnitaria(JSON_entrada_detalheCelulaUnitaria &celulaUnitaria_json);
    void parse_perfil_producao(JSON_entrada_perfilProducao &perfis_producao_json);
    void parse_perfil_servico(JSON_entrada_perfilServico &perfis_servico_json);
    void parse_perfis_trans_producao(JSON_entrada_perfisTransP &perfis_trans_producao_json);
    void parse_perfis_trans_servico(JSON_entrada_perfisTransS &perfis_trans_servico_json);
    void parse_tendencia_producao(JSON_entrada_tendP &tendencia_producao_json);
    void parse_tendencia_servico(JSON_entrada_tendS &tendencia_servico_json);
    void parse_tendencia_trans_producao(JSON_entrada_tendTransP &tendencia_trans_producao_json);
    void parse_tendencia_trans_servico(JSON_entrada_tendTransS &tendencia_trans_servico_json);
    void parse_tela(JSON_entrada_tela &tela_json);
    void copia_configuracao_inicial(Ler &arqAntigo);
    void copia_condcont_pocinjec(Ler &arqAntigo);
    void copia_tempo(Ler &arqAntigo);
    void copia_materiais(Ler &arqAntigo);
    void copia_corte(Ler &arqAntigo);
    void copia_tabela(Ler &arqAntigo);
    void copia_parafina(Ler &arqAntigo);
    void copia_fluidos_producao(Ler &arqAntigo);
    void copia_fluido_complementar(Ler &arqAntigo);
    void copia_fluido_gas(Ler &arqAntigo);
    void copia_unidades_producao(Ler &arqAntigo);
    void copia_unidades_servico(Ler &arqAntigo);
    void copia_hidrato(Ler &arqAntigo);
    void copia_ipr(Ler &arqAntigo);
    void copia_separador(Ler &arqAntigo);
    void copia_correcao(Ler &arqAntigo);
    void copia_chokeSup(Ler &arqAntigo);
    void copia_chokeInj(Ler &arqAntigo);
    void copia_master1(Ler &arqAntigo);
    void copia_master2(Ler &arqAntigo);
    void copia_valv(Ler &arqAntigo);
    void copia_gasInj(Ler &arqAntigo);
    void copia_fonte_gas(Ler &arqAntigo);
    void copia_fonte_liquido(Ler &arqAntigo);
    void copia_fonte_PoroRadial(Ler &arqAntigo);
    void copia_fonte_Poro2D(Ler &arqAntigo);
    void copia_fonte_massa(Ler &arqAntigo);
    void copia_furo(Ler &arqAntigo);
    void copia_fontechk(Ler &arqAntigo);
    void copia_pig(Ler &arqAntigo);
    void copia_bcs(Ler &arqAntigo);
    void copia_multibcs(Ler &arqAntigo);
    void copia_bomba_volumetrica(Ler &arqAntigo);
    void copia_delta_pressao(Ler &arqAntigo);
    void copia_fonteCalor(Ler &arqAntigo);
    void copia_fonte_gaslift(Ler &arqAntigo);
    void copia_intermitencia(Ler &arqAntigo);
    void copia_celulaUnitaria(Ler &arqAntigo);
    void copia_perfil_producao(Ler &arqAntigo);
    void copia_perfil_servico(Ler &arqAntigo);
    void copia_perfis_trans_producao(Ler &arqAntigo);
    void copia_perfis_trans_servico(Ler &arqAntigo);
    void copia_tendencia_producao(Ler &arqAntigo);
    void copia_tendencia_servico(Ler &arqAntigo);
    void copia_tendencia_trans_producao(Ler &arqAntigo);
    void copia_tendencia_trans_servico(Ler &arqAntigo);
    void copia_tela(Ler &arqAntigo);
};

#endif /* LEITURA_H_ */
