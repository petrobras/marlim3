#ifndef _PROFL
#define _PROFL
#define _USE_MATH_DEFINES // for M_PI

#include "Log.h"
#include "MarlimComposicional.h"
#include "Matriz.h"
#include "Vetor.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <math.h>

extern int contaExit;

// Simulation output data report stream
extern ofstream arqRelatorioPerfis;

// Output-file path and filename prefix for POCO_INJETOR
extern string pathPrefixoArqSaida;

// Application logger object
extern Logger logger;

extern int modoTransiente;

extern int simulaTransiente;

constexpr double CoefGopal[] = {1.6643, -2.2114, -0.3647, 1.4385, 0.5220, -0.8511,
                                -0.0364, 1.0490, 0.1391, -0.2988, 0.0007, 0.9969,
                                0.0295, -0.0825, 0.0009, 0.9967, -1.3570, 1.4942,
                                4.6315, -4.7009, 0.1717, -0.3232, 0.5869, 0.1229,
                                0.0984, -0.2053, 0.0621, 0.8580, 0.0211, -0.0527,
                                0.0127, 0.9549, -0.3278, 0.4752, 1.8223, -1.9036,
                                -0.2521, 0.3871, 1.6087, -1.6635, -0.0284, 0.0625,
                                0.4714, -0.0011, 0.0041, 0.0039, 0.0607, 0.7927};

struct tabelaDina {
    double **rholF;
    double **rhogF;
    double **DrholDpF;
    double **DrhogDpF;
    double **DrholDtF;
    double **DrhogDtF;
    double **tit;
    double **rs;
    double **cplF;
    double **cpgF;
    double **HlF;
    double **HgF;
    double **valBO;
    double **valZ;
    double **valdZdT;
    double **valdZdP;
    double **viscO;
    double **viscG;
    double *PBF;
    double *TBF;
    double pmax;
    double pmin;
    double tmax;
    double tmin;
    double delP;
    double delT;
    int npontosT;
    int npontosP;
};

struct miniTabelaDina {
    double **rholF;
    double **rhogF;
    double **DrholDpF;
    double **DrhogDpF;
    double **DrholDtF;
    double **DrhogDtF;
    double **tit;
    double **rs;
    double **cplF;
    double **cpgF;
    double **HlF;
    double **HgF;
    double **valBO;
    double **valZ;
    double **valdZdT;
    double **valdZdP;
    double *PBF;
    double *TBF;
    double pmax;
    double pmin;
    double tmax;
    double tmin;
};

class ProFlu {
  public:
    int id;
    int npseudo; // Number of pseudocomponents in the compositional model
    int posic;
    double mascor; // 1 -> Two-phase stream mass flow rate, kg/s
    double API;    // 2 -> API gravity of the oil in the two-phase stream
    double RGO;    // 3 -> Gas-oil ratio of the two-phase stream, m3/m3
    double rDgD;   // Ratio between dissolved-gas density and separator-gas density
    double rDgL;   // Ratio between free-gas density and separator-gas density
    double Deng;   // 4 -> Gas density
    double BSW;    // 5 -> Stream BSW, m3/m3, ranging from 0 to 1
    double Denag;  // 6 -> Water density
    double TempL;  // 7 -> Temperature in degrees Celsius of the lower ASTM point used for liquid-viscosity calculations
    double LVisL;  // 8 -> Viscosity in cP at the lower ASTM point, paired with vtempl as (vtempl, vlvisl)
    double TempH;  // 9 -> Temperature in degrees Celsius of the upper ASTM point used for liquid-viscosity calculations
    double LVisH;  // 10 -> Viscosity in cP at the upper ASTM point, paired with vtemph as (vtemph, vlvish)
    int tipoemul;  // Emulsion-correlation selector: 0 -> oil-water mixture viscosity,
    // weighted by water fraction; 1 -> weak Woelflin; 2 -> medium Woelflin; 3 -> strong Woelflin; 4 -> exponential;
    // 5 -> Pal-Rhodes; 6 -> user-defined BSW/multiplier pairs; 7 -> oil viscosity below
    // the emulsion saturation BSW
    double bswCorte; // Emulsion saturation BSW

    int nvecEmul;    // Number of BSW/multiplier pairs used by emulsion model 6
    double *BSWVec;  // User-provided BSW values for emulsion model 6
    double *emulVec; // User-provided emulsion multipliers for emulsion model 6

    // Parameters for the Beggs-Vazquez solution gas-oil ratio model
    double FAC;
    double SG100vb;
    double Avb;
    double Bvb;
    double Cvb;
    double IRGO; // Gas-oil ratio in field units
    double PC;   // Critical pressure in psia for free gas using separator standard-condition density
    double TC;   // Critical temperature in degrees Rankine for free gas using separator standard-condition density
    double PCis; // Critical pressure in psia for free gas using in-situ density
    double TCis; // Critical temperature in degrees Rankine for free gas using in-situ density
    // The following auxiliary terms are used in the homogeneous-flow pressure-drop approximation:
    double term1; // Unused
    double term2; // Unused
    double term3; // Unused
    double term4; // Unused
    double term5; // Unused
    double PCF;   // Unused; units: kgf/cm2
    double TCF;   // Unused; units: degrees Celsius
    double yco2;  // CO2 mole fraction
    int corrC;    // Correlation selector used to calculate the gas critical pressure and temperature,
    // three correlations imported from Marlim2; options 1 and 2 are more suitable for CO2-rich cases

    int tab; // For the Black Oil model, indicates whether the gas compressibility factor and its derivatives are obtained
    // through table interpolation: 0 -> direct calculation; 1 -> interpolation
    int npontos;  // Number of table points
    int npontosB; // Number of table points
    int nfluP;    // Unused
    int ModelCp;  // For the Black Oil model, selects either a direct correlation or
    // flash-table data: 0 -> direct correlation; 1 -> table
    int Modeljtl; // For the Black Oil model, selects either a direct correlation or
    // flash-table data: 0 -> direct correlation; 1 -> table
    int CalcLat;     // Unused
    int trackRGO;    // Unused
    int trackDeng;   // Unused
    int corrDeng;    // Unused
    int ninjgas;     // Unused
    int lingas;      // Unused
    double **zdranP; // Black Oil compressibility-factor table, when table usage is
    // enabled
    double **dzdpP; // Black Oil table of the compressibility-factor derivative with respect to reduced pressure, when table usage is
    // enabled
    double **dzdtP; // Black Oil table of the compressibility-factor derivative with respect to reduced temperature, when table usage is
    // enabled
    double **cpg; // Black Oil gas specific-heat table, when table usage is
    // enabled
    double **cpl; // Black Oil liquid specific-heat table, when table usage is
    // enabled
    double **drholdT; // Black Oil table of the liquid-density derivative with respect to temperature, when table usage is
    // enabled

    // Auxiliary arrays for the Lasater solution gas-oil ratio correlation, initialized by the constructor
    // following the implementation used in Marlim 2
    Vcr<double> BPPF;
    Vcr<double> GMF;
    Vcr<double> APIEMW;
    Vcr<double> EMW;
    int corrSat; // Solution gas-oil ratio correlation, scf/bbl: 0 -> Vazquez-Beggs; 1 -> Lasater; 2 -> Standing; 3 -> Glaso; 4 -> Livia
    int corrOM;  // Dead-oil viscosity correlation, cP: 0 -> ASTM; 1 -> Beggs-Robinson; 2 -> modified Beggs-Robinson; 3 -> Glaso; 4 -> Kartoatmodjo-Schmidt; 5 -> Petrosky-Farshad; 6 -> Beal
    int corrOV;  // Live-oil viscosity correlation, cP: 0 -> Beggs-Robinson; 1 -> Kartoatmodjo-Schmidt; 2 -> Petrosky-Farshad
    int corrOS;  // Undersaturated-oil viscosity correlation, cP: 0 -> Vazquez-Beggs; 1 -> Kartoatmodjo-Schmidt; 2 -> Petrosky-Farshad; 3 -> Beal; 4 -> Khan
    // Parameters used by the Livia solution gas-oil ratio correlation
    double A0Liv;
    double A1Liv;
    double A2Liv;
    double A3Liv;
    double A4Liv;
    double B0Liv;
    double B1Liv;
    double B2Liv;
    double B3Liv;
    double B4Liv;
    double C0Liv;
    double C1Liv;
    double C2Liv;
    double C3Liv;
    double C4Liv;
    double D0Liv;
    double D1Liv;
    double D2Liv;
    double *PBPVTSim;    // Bubble-point pressure values loaded from a PVTSim file
    double *TBPVTSim;    // Bubble-point temperature values loaded from a PVTSim file
    double **TabRSLivia; // Table of solution gas-oil ratio values for the Black Oil model when using
    // interpolated correlation values instead of direct calculation
    double aemul;  // Parameter a of the exponential emulsion model
    double bemul;  // Parameter b of the exponential emulsion model
    double PHI100; // Emulsion-multiplier parameter for the Pal-Rhodes model
    int tabRSPB;   // Indicates whether the Black Oil model uses a solution gas-oil ratio table
    // instead of direct correlation evaluation; 1 -> use table
    int flashCompleto; // Selects the PVTSim table model: 1 -> PVTSim table
    // 2 -> compositional model
    int tabelaDinamica;
    int viscBlackOil; // For PVTSim tables, indicates whether viscosity values should still be obtained from
    // a Black Oil model
    int indiceFlash; // Unused

    // Property tables loaded from a PVTSim file
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

    int *itempAnt;
    int *ipresAnt;

    // Compositional model
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
    int liqModel;
    int iIER_BubbleT;
    int iIER_DewT;
    int iIER_Flash;
    int iIER_BubbleP;
    int iIER;
    double Pmol;

    double dCalculatedBubbleT;
    double dCalculatedDewT;
    double dCalculatedBeta;
    double *oCalculatedLiqComposition;
    double *oCalculatedVapComposition;
    double dCalculatedBubbleP;
    int iCalculatedThermodynamicCondition;
    int iCalculatedStockTankThermodynamicCondition;
    double dLiquidPhaseMW;
    double dVaporPhaseMW;
    double dVaporMassFraction;
    double dStockTankVaporMassFraction;
    double dStockTankLiquidDensity;
    double dStockTankVaporDensity;
    int modoBlackTemp;
    tabelaDina tabDin;
    miniTabelaDina miniTabDin;
    int parserie;
    double *viscTab;
    double *tempTab;

    int modelaAgua; // When using a PVTSim table, indicates whether an internal correlation is used
    // to calculate the water Joule-Thomson coefficient instead of the PVTSim table values
    double pMinEqu; // Unused
    double tMinEqu; // Unused
    int delpTab;    // Unused
    // Auxiliary attributes that accelerate PVTSim-table interpolation by improving
    // the search for the pressure and temperature intervals containing the interpolation point
    // These parameters are originally obtained when the Leitura class reads the table.
    // PVTSim tables generally contain equally spaced pressure and temperature intervals
    // grouped into ranges. Below standard-condition pressure and temperature values,
    // the intervals are assumed to be nonuniform. The indices identifying where values fall
    // below standard pressure and temperature are stored in indMinEquP and indMinEquT.
    // Above these indices, pressure and temperature intervals are uniform within each range.
    // Auxiliary arrays store the corresponding dP and dT values for each table-index range.
    // pchange stores the pressure at which a new dP interval begins;
    // dpchange stores dP values; ipchange stores the pressure indices where each new dP begins;
    // tchange stores the temperature at which a new dT interval begins;
    // dtchange stores dT values; itchange stores the temperature indices where each new dT begins.
    vector<double> pchange;
    vector<double> dpchange;
    vector<int> ipchange;
    int indMinEquP;

    int deltTab;
    vector<double> tchange;
    vector<double> dtchange;
    vector<int> itchange;
    int indMinEquT;
    int erroP; // Enables or disables the pressure interpolation accelerator described above
    // 1 -> enabled
    int erroT; // Enables or disables the temperature interpolation accelerator described above
    // 1 -> enabled

    double bASTM1; // Auxiliary values used to calculate dead-oil viscosity with the
    // ASTM method
    double bASTM2; // Auxiliary values used to calculate dead-oil viscosity with the
    // ASTM method
    varGlob1D *vg1dSP;

    int modoParafina;
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

    int npseudoWax;
    int npseudoWaxOut;
    int iPresWax;
    int iTempWax;
    int iIERW;
    int iWaxComponentCountOutput;
    double dCloudPointTOutput;
    double dInterpolatedCPWaxOutput;
    double dInterpolatedThermCondOutput;
    double dInterpolatedDensOutput;
    double dInterpolatedLiqMWOutput;
    double *oLiquidDensitiesOfWaxComponentsOutput;
    double *oInterpolatedWaxConcsTDerivOutput;
    double *oInterpolatedMassWaxConcsTDerivOutput;
    double *oInterpolatedWaxConcs;
    double *oMolecularWeightsOfWaxComponentsOut;
    double coefViscWax;
    double JTLiquidoSimple;

    double FalsaCorda(double, double, double, double, double delta = 0.00001, double epsn = 0.00001, int maxit = 100) const; // Root-finding routine used exclusively by FNDran
    // The integrand has the form f(x); x contains the quadrature points and w contains their weights
    void gauleg(const double x1, const double x2, double *const x, double *const w, int size) const; // Auxiliary routine for Gaussian quadrature
    // The integrand has the form f(x); x contains the quadrature points and w contains their weights
    double quadleg(const double x1, const double x2, int tipo, int fase, const double par, const int n) const; // Gaussian quadrature using Legendre polynomials; x1 -> lower limit; x2 -> upper limit
    // n -> number of quadrature points; tipo -> integrate over pressure (1) or temperature (2); fase -> gas (1) or liquid (2); par -> pressure when tipo=2, temperature when tipo=1

    ProFlu(varGlob1D *Vvg1dSP = 0, double vapi = 1, double vrgo = 1, double vdeng = 1, double vbsw = 1,
           double vdenag = 1, double vtempl = 10, double vlvisl = 1,
           double vtemph = 10, double vlvish = 1, int vtipoemul = 0, double vaemul = 0, double vbemul = 0,
           double vPHI100 = 0.765, double vbswCorte = 0, int vtab = 0, double vyco2 = 0, int vcorrC = 0,
           int vcorrSat = 0, int vcorrOM = 0, int vcorrOV = 0, int vcorrOS = 0,
           int vflash = 0, int id = 0, int vnpseudo = 0, double vpcf = 0, double vtcf = 0); // Default constructor

    ProFlu(varGlob1D *Vvg1dSP, const double *const fluido, int vtipoemul,
           double vaemul, double vbemul, double vPHI100, double vbswCorte,
           int vtab, double vyco2, int vcorrC, int vcorrSat, int vcorrOM, int vcorrOV, int vcorrOS,
           int vflash,
           int vid, int vnpseudo = 0);

    ProFlu(varGlob1D *Vvg1dSP, const double *const fluido, const double *const astm, int vtipoemul, double vaemul, double vbemul, double vPHI100,
           double vbswCorte, int vtab, double vyco2, int vcorrC, int vcorrSat,
           int vcorrOM, int vcorrOV, int vcorrOS, int vflash, int vid, int vnpseudo);
    ProFlu(const ProFlu &); // Copy constructor

    ProFlu &operator=(const ProFlu &);
    double operator[](int i) const;
    ProFlu operator+(const ProFlu &) const;

    ~ProFlu() {
        if (flashCompleto == 2 && npseudo > 0)
            delete[] fracMol;
        if (flashCompleto == 2 && npseudo > 0)
            delete[] oCalculatedLiqComposition;
        if (flashCompleto == 2 && npseudo > 0)
            delete[] oCalculatedVapComposition;
        if (flashCompleto == 2 && npseudo > 0 && (*vg1dSP).simulaTransiente == 1) {
            for (int j = 0; j < 2; j++) {
                delete[] miniTabDin.rhogF[j];
                delete[] miniTabDin.rholF[j];
                delete[] miniTabDin.DrhogDpF[j];
                delete[] miniTabDin.DrhogDtF[j];
                delete[] miniTabDin.DrholDpF[j];
                delete[] miniTabDin.DrholDtF[j];
                delete[] miniTabDin.valBO[j];
                delete[] miniTabDin.HgF[j];
                delete[] miniTabDin.HlF[j];
                delete[] miniTabDin.cpgF[j];
                delete[] miniTabDin.cplF[j];
                delete[] miniTabDin.valZ[j];
                delete[] miniTabDin.valdZdT[j];
                delete[] miniTabDin.valdZdP[j];
                delete[] miniTabDin.tit[j];
                delete[] miniTabDin.rs[j];
            }
            delete[] miniTabDin.rhogF;
            delete[] miniTabDin.rholF;
            delete[] miniTabDin.DrhogDpF;
            delete[] miniTabDin.DrhogDtF;
            delete[] miniTabDin.DrholDpF;
            delete[] miniTabDin.DrholDtF;
            delete[] miniTabDin.valBO;
            delete[] miniTabDin.HgF;
            delete[] miniTabDin.HlF;
            delete[] miniTabDin.cpgF;
            delete[] miniTabDin.cplF;
            delete[] miniTabDin.valZ;
            delete[] miniTabDin.valdZdT;
            delete[] miniTabDin.valdZdP;
            delete[] miniTabDin.tit;
            delete[] miniTabDin.rs;
            delete[] miniTabDin.TBF;
            delete[] miniTabDin.PBF;
        }
        if (modoParafina == 1 && npseudoWax > 0) {
            delete[] oMolecularWeightsOfWaxComponentsOut;
            delete[] oInterpolatedWaxConcsTDerivOutput;
            delete[] oInterpolatedMassWaxConcsTDerivOutput;
            delete[] oInterpolatedWaxConcs;
            delete[] oLiquidDensitiesOfWaxComponentsOutput;
        }
    }

    void MudaVazMas(double NovaVaz) { mascor = NovaVaz; } // Updates the stream mass flow rate
    double *TermAux(double *saida) const {
        saida[0] = term1;
        saida[1] = term2;
        saida[2] = term3;
        saida[3] = term4;
        saida[4] = term5;
        return saida;
    } // Returns the auxiliary terms used by the Beggs-Brill model

    double PreRog(double a /*pressure initial guess*/,
                  double b /*pressure initial guess*/, double rog /*gas density*/,
                  double temp /*temperature*/, double delta = 0.00000000001,
                  double epsn = 0.00000000001, int maxit = 300) const; // Returns pressure in kgf/cm2
    // for a specified gas density in kg/m3 and temperature in degrees Celsius
    void RenovaFluido(); // Updates fluid parameters, including Black Oil parameters,
    // gas critical pressure and temperature when properties such as API gravity and gas density change,
    // CO2 mole fraction, and separator gas-oil ratio
    void PcTcIS();                                                        // For Black Oil models, calculates the in-situ critical pressure and temperature of free gas
    double VisAgua(double temp /*temperature in degrees Celsius*/) const; // Calculates water viscosity
    double VisOM(double temp /*temperature in degrees Celsius*/) const;   // Calculates dead-oil viscosity
    double interpolaLas1(double var) const;                               // Interpolation function for the Lasater solution gas-oil ratio model
    double interpolaLas2(double var) const;                               // Interpolation function for the Lasater solution gas-oil ratio model
    double interpolaLas3(double var) const;                               // Interpolation function for the Lasater solution gas-oil ratio model
    double interpolaPB(double var) const;                                 // Interpolates bubble-point pressure from the PVTSim table
    double interpolaRS(double pres /*pressure in kgf/cm2*/,
                       double temp /*temperature in degrees Celsius*/) const; // Interpolates the solution gas-oil ratio in scf/bbl obtained from a Black Oil model
    double RS(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
              double varPb = -1.                                                                                                 /*Bubble-point pressure in psia when already available; a negative value causes the method to calculate it*/
    ) const;                                                                                                                     // Returns the solution gas-oil ratio in scf/bbl
    double PB(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const;                        // Returns bubble-point pressure in psia
    double ViscOleo(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/, int SemEmul = 0) const; // Returns viscosity in cP for
    // saturated oil mixed with water
    double ViscGas(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Returns the viscosity
    // of the gas, in cP
    double emul(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Returns the multiplier
    // for the emulsion model
    double ZGopal(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Compressibility factor
    // calculated using the Gopal correlation
    double FNdran(double presR /*reduced pressure*/, double temp /*reduced temperature*/,
                  double denR /*reduced density*/) const; // Auxiliary function for calculating the compressibility factor
    // using the Dranchuk correlation
    double ZdranOriginal(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                         int cordg = -1 /*gas-density correction when distinguishing between dissolved and free gas densities*/
                         ,
                         double = 0.9, double = 10, int = 1) const; // Calculates the compressibility factor
    // using the Dranchuk correlation
    double Zdran(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/, int cordg = -1 /*gas-density correction when distinguishing between dissolved and free gas densities*/
                 ,
                 double masesp = -1) const; // Returns the compressibility factor and internally selects
    // between the Dranchuk correlation and table interpolation
    double DZDTOriginal(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                        int cordg = -1 /*gas-density correction when distinguishing between dissolved and free gas densities*/
    ) const;                           // Calculates the derivative of the compressibility factor with respect to temperature
    // using the Dranchuk correlation
    double DZDT(double, double, double masesp = -1) const; // Returns the compressibility-factor derivative
    // with respect to temperature, selecting between the Dranchuk correlation and table interpolation
    double DZDPOriginal(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                        int cordg = -1 /*gas-density correction when distinguishing between dissolved and free gas densities*/
    ) const;                           // Calculates the derivative of the compressibility factor with respect to pressure
    // using the Dranchuk correlation
    double DZDP(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/, double masesp = -1) const; // Returns the compressibility-factor derivative
    // with respect to pressure, selecting between the Dranchuk correlation and table interpolation
    void tabela(double pmax /*maximum reduced pressure in the table*/,
                double tmax /*maximum reduced temperature in the table*/,
                double pmin /*minimum reduced pressure in the table*/,
                double tmin /*minimum reduced temperature in the table*/) const; // Generates the Black Oil compressibility-factor table
    void tabelaDZDP(double pmax /*maximum reduced pressure in the table*/,
                    double tmax /*maximum reduced temperature in the table*/,
                    double pmin /*minimum reduced pressure in the table*/,
                    double tmin /*minimum reduced temperature in the table*/) const; // Generates the Black Oil table of the compressibility-factor derivative with respect to pressure
    void tabelaDZDT(double pmax /*maximum reduced pressure in the table*/,
                    double tmax /*maximum reduced temperature in the table*/,
                    double pmin /*minimum reduced pressure in the table*/,
                    double tmin /*minimum reduced temperature in the table*/) const; // Generates the Black Oil table of the compressibility-factor derivative with respect to temperature
    double interpolaVarProd(double pres /*pressure in kgf/cm2*/,
                            double temp /*temperature in degrees Celsius*/, double **Var /*property table obtained from PVTSim*/) const; // Method
    // that interpolates a PVTSim property at the specified pressure and temperature
    double BOFunc(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                  double varRS = -1 /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                        // Calculates the formation volume factor
    // of oil
    double BAFunc(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Calculates the formation volume factor
    // of water
    double MasEspoleo(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                      double varRS = -1                                                                               /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                                                                                                          // Calculates oil density in kg/m3
    double MasEspOleoComp(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Calculates oil density in kg/m3
    void razDegD(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/);                // Returns the ratio
    // of dissolved-gas density to gas density at standard conditions
    void rzDegL(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/); // Returns the ratio
    // of free-gas density to gas density at standard conditions
    double MasEspGas(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Calculates the
    // gas density, kg/m3
    double MasEspGasComp(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Calculates the
    // gas density, kg/m3
    double DMasEspAgua(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const;
    double MasEspAgua(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Calculates the
    // water density, kg/m3
    double MasEspLiq(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/, double varRS = -1 /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                                                                                                                // Calculates the mixture density
    // for oil and water, kg/m3
    double MasEspLiqComp(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Calculates the mixture density
                                                                                                                     // for oil and water, kg/m3
    double MasOleo(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                   double varRS = -1 /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                         // Oil mass in kg per 1 m3 of dead oil
    double MasOleoHidra(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                        double varRS = -1 /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                              // Oil mass in kg per 1 m3 of dead-oil/water mixture
    double MasLiq(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                  double varRS = -1                                                                            /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                                                                                                   // Liquid mass in kg per 1 m3 of dead-oil/water mixture
    double MasAgua(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Water mass in kg per 1 m3 of dead-oil/water mixture
    double MasGasLivre(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                       double varRS = -1 /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                             // Free-gas mass in kg per 1 m3 of dead oil
    double MasGasLivreHidra(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                            double varRS = -1                                                                        /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                                                                                                         // Free-gas mass in kg per 1 m3 of dead-oil/water mixture
    double FracMass(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const;      // Free-gas mass fraction in the oil-gas mixture
    double FracMassHidra(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/) const; // Free-gas mass fraction in the oil-gas-water mixture
    double FracAguaInSitu(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                          double varRS = -1                           /*Solution gas-oil ratio in scf/bbl when already available; a negative value causes the method to calculate it*/
    ) const;                                                          // In-situ BSW
    double interpola(Vcr<double> abs, Vcr<double> coord, double var); // Unused
    double TensSuper(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/,
                     double bet = 0. /*beta fraction*/,
                     double tensupBet = 0. /*surface tension of the beta liquid phase*/);                      // Surface tension, N/m
    double TensSuperOleo(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/); // Oil surface tension, N/m
    double TensSuperAgua(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/); // Water surface tension, N/m
    double CalorLiqOriginal(double pres /*pressure in kgf/cm2*/,
                            double temp /*temperature in degrees Celsius*/) const; // Black Oil liquid specific heat, J/(kg K)
    double CalorGasOriginal(double pres /*pressure in kgf/cm2*/,
                            double temp /*temperature in degrees Celsius*/) const; // Black Oil gas specific heat, J/(kg K)
    double DrholDTOriginal(double pres /*pressure in kgf/cm2*/,
                           double temp /*temperature in degrees Celsius*/) const; // Derivative of liquid density with respect to temperature,
    // for the Black Oil model
    double interpolaCpl(double pres /*pressure in kgf/cm2*/
                        ,
                        double temp /*temperature in degrees Celsius*/) const; // Interpolates liquid specific heat, J/(kg K),
    // for the Black Oil model
    double interpolaCpg(double pres /*pressure in kgf/cm2*/
                        ,
                        double temp /*temperature in degrees Celsius*/) const; // Interpolates gas specific heat, J/(kg K),
    // for the Black Oil model
    double interpolaDrholDT(double pres /*pressure in kgf/cm2*/
                            ,
                            double temp /*temperature in degrees Celsius*/) const; // Interpolates the derivative of
    // liquid density with respect to temperature from a Black Oil table
    double CalorLiq(double pres /*pressure in kgf/cm2*/
                    ,
                    double temp /*temperature in degrees Celsius*/) const; // Calculates liquid specific heat using either interpolation or
    // direct correlation evaluation, J/(kg K)
    double CalorGas(double pres /*pressure in kgf/cm2*/
                    ,
                    double temp /*temperature in degrees Celsius*/) const; // Calculates gas specific heat using either interpolation or
    // direct correlation evaluation, J/(kg K)
    double drhodp(double pres /*pressure in kgf/cm2*/
                  ,
                  double temp /*temperature in degrees Celsius*/) const; // Calculates the derivative of gas density with respect to pressure
    double drhodt(double pres                                            /*pressure in kgf/cm2*/
                  ,
                  double temp /*temperature in degrees Celsius*/) const; // Calculates the derivative of gas density with respect to temperature
    double DrholDT(double pres /*pressure in kgf/cm2*/,
                   double temp /*temperature in degrees Celsius*/) const; // Calculates the derivative of gas density with respect to
    // temperature using either a correlation or a table
    double CalorGasVolMod(double pres /*pressure in kgf/cm2*/,
                          double temp /*temperature in degrees Celsius*/, double rhogini = -1.) const; // Calculates the modified specific-heat relation
    // at constant gas volume; see the transient report; obtained by interpolation or direct correlation, J/(kg K)
    double CalorGasVolMod2(double pres /*pressure in kgf/cm2*/,
                           double temp /*temperature in degrees Celsius*/, double rhogini = -1.) const; // Calculates the modified gas specific-heat relation
    // at constant volume; see the transient report; obtained by interpolation or direct correlation, J/(kg K)
    double CalorGasPresMod(double pres /*pressure in kgf/cm2*/,
                           double temp /*temperature in degrees Celsius*/, double rhogini = -1.) const; // Calculates the modified gas specific-heat relation
    // at constant pressure; see the transient report; obtained by interpolation or direct correlation, J/(kg K)
    double ConstAdG(double pres /*pressure in kgf/cm2*/,
                    double temp /*temperature in degrees Celsius*/, double rhogini = -1.) const; // Calculates the adiabatic exponent
    double CondLiq(double pres /*pressure in kgf/cm2*/,
                   double temp /*temperature in degrees Celsius*/) const; // Calculates liquid thermal conductivity, W/(m K)
    double CondOleo(double pres /*pressure in kgf/cm2*/,
                    double temp /*temperature in degrees Celsius*/) const; // Calculates oil thermal conductivity, W/(m K)
    double CondGas(double pres /*pressure in kgf/cm2*/,
                   double temp /*temperature in degrees Celsius*/) const; // Calculates gas thermal conductivity, W/(m K)
    double EntalpLiq(double, double) const;
    double EntalpLiqIsoB(double, double) const; // Unused
    double EntalpLiqIsoT(double, double) const; // Unused
    double JTL(double pres /*pressure in kgf/cm2*/,
               double temp /*temperature in degrees Celsius*/) const;
    double EntalpGas(double, double) const;
    double EntalpGasIsoB(double, double) const; // Unused
    double EntalpGasIsoT(double, double) const; // Unused
    double JTG(double pres /*pressure in kgf/cm2*/, double temp /*temperature in degrees Celsius*/, double rhog = -1) const;
    double dRSdP(double, double) const;                                      // Unused
    double dBOSatdP(double, double) const;                                   // Unused
    double retornaR() const { return 8.0465 * 1000 / (Deng * 28.9625); }     // Returns the gas constant
    double psia(const double p) const { return (p * 0.9678411) * 14.69595; } // Converts pressure from kgf/cm2 to psia
    double Faren(const double t) const { return 1.8 * t + 32; }              // Converts temperature from degrees Celsius to degrees Fahrenheit
    double cel(const double t) const { return (t - 32) / 1.8; }              // Converts temperature from degrees Fahrenheit to degrees Celsius
    double kgf(const double p) const { return p / (14.69595 * 0.9678411); }  // Converts pressure from psia to kgf/cm2
    double ft3bbl() const { return RGO * 35.31467 / 6.29; }                  // Converts gas-oil ratio from m3/m3 to ft3/bbl
    double m3m() const { return RGO * 6.29 / 35.31467; }                     // Converts gas-oil ratio from ft3/bbl to m3/m3

    // Updates compositional-model variables for the specified pressure and temperature
    void atualizaPropCompStandard(double GivenInitialBeta = -1.0, double *GivenInitialLiqComposition = NULL,
                                  double *GivenInitialVapComposition = NULL) {
        double pres = 1.03322;
        double temp = 20.;
        double tempRGO = 100.;
        int nuloL = 0;
        if (GivenInitialLiqComposition == NULL) {
            GivenInitialLiqComposition = new double[npseudo];
            for (int k = 0; k < npseudo; k++)
                GivenInitialLiqComposition[k] = 0;
            nuloL = 1;
        }
        int nuloV = 0;
        if (GivenInitialVapComposition == NULL) {
            GivenInitialVapComposition = new double[npseudo];
            for (int k = 0; k < npseudo; k++)
                GivenInitialVapComposition[k] = 0;
            nuloV = 1;
        }
        Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions(npseudo, fracMol, masMol, tempCrit,
                                                                    presCrit, fatAcent,
                                                                    TIndepPeneloux, kij, lij,
                                                                    liqModel, liqModel, liqModel, liqModel, pres, temp,
                                                                    GivenInitialBeta, GivenInitialLiqComposition, GivenInitialVapComposition,
                                                                    &iCalculatedStockTankThermodynamicCondition,
                                                                    &dStockTankVaporMassFraction, &dStockTankLiquidDensity, &dStockTankVaporDensity,
                                                                    &tempRGO, &iIER);

        if (dStockTankLiquidDensity > 0.01) {
            API = 141.5 / (dStockTankLiquidDensity / 1000.) - 131.5;
        } else
            API = 50;
        Deng = dStockTankVaporDensity / 1.225;
        RGO = tempRGO;
        if (dStockTankVaporMassFraction > 0 && RGO < 1e-15)
            RGO = 1e6;
        IRGO = RGO * 35.31467 / 6.29;
        if (nuloL == 1)
            delete[] GivenInitialLiqComposition;
        if (nuloV == 1)
            delete[] GivenInitialVapComposition;
    }

    void atualizaPropComp(double pres, double temp, double GivenInitialBeta = -1.0,
                          double *GivenInitialLiqComposition = NULL, double *GivenInitialVapComposition = NULL, int monoGas = 0) {

        double dMixtureMW = 0.;

        int nuloL = 0;
        if (GivenInitialLiqComposition == NULL) {
            GivenInitialLiqComposition = new double[npseudo];
            for (int k = 0; k < npseudo; k++)
                GivenInitialLiqComposition[k] = 0;
            nuloL = 1;
        }
        int nuloV = 0;
        if (GivenInitialVapComposition == NULL) {
            GivenInitialVapComposition = new double[npseudo];
            for (int k = 0; k < npseudo; k++)
                GivenInitialVapComposition[k] = 0;
            nuloV = 1;
        }

        for (int i = 0; i < npseudo; i++) {
            dMixtureMW += fracMol[i] * masMol[i];
        }
        if (monoGas == 1) {
            iCalculatedThermodynamicCondition = 4;
            dVaporMassFraction = 1.;
            for (int i = 0; i < npseudo; i++) {
                oCalculatedVapComposition[i] = fracMol[i];
                oCalculatedLiqComposition[i] = 0.;
                dCalculatedBeta = 1.;
                iIER_Flash = 0;
                iIER = 0;
                dVaporPhaseMW = dMixtureMW;
            }
        } else {
            dCalculatedBubbleP = 50.;

            Marlim_CalculateMixtureThermodynamicCondition(pres, temp, npseudo,
                                                          masMol, fracMol, tempCrit, presCrit, fatAcent, TIndepPeneloux, kij, lij, liqModel,
                                                          liqModel, GivenInitialBeta, GivenInitialLiqComposition, GivenInitialVapComposition,
                                                          &dCalculatedBubbleT, &iIER_BubbleT, &dCalculatedDewT, &iIER_DewT,
                                                          &dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, &iIER_Flash,
                                                          &dCalculatedBubbleP, &iIER_BubbleP, &iCalculatedThermodynamicCondition, &dLiquidPhaseMW,
                                                          &dVaporPhaseMW, &dVaporMassFraction, &iIER);
        }
        if (monoGas == 0 && dMixtureMW <= (30. + 1e-15) && iIER > 0) {
            iCalculatedThermodynamicCondition = 4;
            dVaporMassFraction = 1.;
            for (int i = 0; i < npseudo; i++) {
                oCalculatedVapComposition[i] = fracMol[i];
                oCalculatedLiqComposition[i] = 0.;
                dCalculatedBeta = 1.;
                iIER_Flash = 0;
                iIER = 0;
                dVaporPhaseMW = dMixtureMW;
            }
        }
        if (nuloL == 1)
            delete[] GivenInitialLiqComposition;
        if (nuloV == 1)
            delete[] GivenInitialVapComposition;
    }

    void atualizaPropParafina(double pres, double temp) {
        Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation(pres, temp, npseudoWax /*number of components*/,
                                                       iPresWax,
                                                       iTempWax, oPressurePoints, oCloudPointTemperatures, oStructurePressures,
                                                       oStructureTemperatures, oStructureDensities, oStructureLiqMW, oStructureCPWax,
                                                       oStructureThermCond, oStructureWaxConcs,
                                                       oMolecularWeightsOfWaxComponents /*wax-component molecular weight*/,
                                                       oLiquidDensitiesOfWaxComponents /*liquid density of each component*/,
                                                       &iIERW, &iWaxComponentCountOutput, oMolecularWeightsOfWaxComponentsOut,
                                                       oLiquidDensitiesOfWaxComponentsOutput, &dCloudPointTOutput,
                                                       &dInterpolatedCPWaxOutput /*wax specific heat at the specified pressure and temperature*/,
                                                       &dInterpolatedThermCondOutput /*wax thermal conductivity*/,
                                                       &dInterpolatedDensOutput /*wax density*/,
                                                       &dInterpolatedLiqMWOutput,
                                                       oInterpolatedWaxConcsTDerivOutput /*solubility curve*/,
                                                       oInterpolatedMassWaxConcsTDerivOutput /*mass-based solubility curve*/,
                                                       oInterpolatedWaxConcs);
    }
};

ostream &operator<<(ostream &s, const ProFlu &); // Writes fluid-property values to the output stream

#endif