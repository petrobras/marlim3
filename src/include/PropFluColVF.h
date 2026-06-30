/*
 * PropFluColVF.h
 *
 * Created on: December 21, 2015
 *     Author: bwz1
 */

#ifndef PROPFLUCOLVF_H_
#define PROPFLUCOLVF_H_

class ProFluColVF {
  public:
    double rhogStd;
    double rhog;
    double rholStd;  // Liquid density at standard conditions, specified by the user, kg/m3
    double compresP; // Liquid compressibility, specified by the user, 1/Pa
    double compresT; // Liquid thermal expansion coefficient, specified by the user, 1/K
    double tenssup;  // Surface tension, specified by the user, N/m
    double cp;       // Liquid specific heat capacity, specified by the user, J/kgK
    double cond;     // Liquid thermal conductivity, specified by the user, W/mK
    double TempL;    // 7 -> ASTM reference-point temperature used to calculate liquid viscosity, degrees Celsius
    double LVisL;    // 8 -> ASTM reference-point liquid viscosity, cP, associated with TempL: (vtempl, vlvisl)
    double TempH;    // 9 -> ASTM reference-point temperature used to calculate liquid viscosity, degrees Celsius
    double LVisH;    // 10 -> ASTM reference-point liquid viscosity, cP, associated with TempH: (vtemph, vlvish)
    double salin;    // Water salinity when the complementary fluid is water or a completion fluid
    int injPoc;      // Indicates that the current fluid is used in an injection well
    // injPoc = 3: CO2-rich gas using property tables
    int descarga; // When set to 1, the complementary fluid is used as a completion fluid
    // in a gas-lift unloading model
    int npontos; // Number of points in the property table used when the injection fluid
    // is a CO2-rich gas
    int tipoF; // Used to explicitly identify the fluid as water or a drag-reducing agent
    double TR;

    // Property tables for the CO2-rich injection gas
    double **RhoInj;
    double **ViscInj;
    double **CondInj;
    double **CpInj;
    double **DrhoDtInj;

    int tabelaVisc;
    int naoN;
    int nTemp;
    int nDeform;
    double **viscNaoNew;

    ProFluColVF(double = 1000, double = 0, double = 0, double = 1e-6, double = 4000, double = 1, double = 0,
                double = 1, double = 2, double = 40, double = 1, int = 0, int vtipo = 0, double vrhogstd = 0., double vrhog = 0.,
                int vtab = 0, int vnTemp = 0, int vnNew = 0, int vnaoN = 0); // Default constructor
    ProFluColVF(const ProFluColVF &);                                        // Copy constructor
    ProFluColVF &operator=(const ProFluColVF &);

    ~ProFluColVF() {
        if (tabelaVisc > 0) {
            if (nTemp > 0) {
                for (int i = 0; i < nTemp + 1; i++)
                    delete[] viscNaoNew[i];
                delete[] viscNaoNew;
            }
        }
    }

    double operator[](int i) const;
    double MasEspFlu(double pres /*pressure in kgf/cm2*/,
                     double temp /*temperature in degrees Celsius*/) const; // Returns the fluid density, kg/m3
    double VisFlu(double pres /*pressure in kgf/cm2*/,
                  double temp /*temperature in degrees Celsius*/, double deform = 0) const; // Returns the liquid viscosity, cP
    double VisGas(double pres /*pressure in kgf/cm2*/,
                  double temp /*temperature in degrees Celsius*/) const; // Returns the gas viscosity, cP
    double TensSuper(double pres /*pressure in kgf/cm2*/,
                     double temp /*temperature in degrees Celsius*/) const { return tenssup; } // Returns the surface tension, N/m
    double CalorLiq(double pres /*pressure in kgf/cm2*/,
                    double temp /*temperature in degrees Celsius*/) const; // Returns the liquid specific heat capacity, J/kgK
    double CondLiq(double pres /*pressure in kgf/cm2*/,
                   double temp /*temperature in degrees Celsius*/) const; // Returns the liquid thermal conductivity, W/mK
    double DrhoDtFlu(double pres /*pressure in kgf/cm2*/,
                     double temp /*temperature in degrees Celsius*/) const; // Derivative of density with respect to temperature, kg/m3/K
    double interpolaVarInj(double pres /*pressure in kgf/cm2*/,
                           double temp /*temperature in degrees Celsius*/,
                           double **VarInj /*property table for the CO2-rich gas*/) const; // Interpolates injection-fluid properties
    // loaded from a PVTSim file when the injection fluid is a CO2-rich gas
    double psia(const double p) const { return (p * 0.9678411) * 14.69595; } // Converts pressure from kgf/cm2 to psia
    double Faren(const double t) const { return 1.8 * t + 32; }              // Converts temperature from degrees Celsius to degrees Fahrenheit
    double calcDR(double ReyL);
};

#endif /* PROPFLUCOLVF_H_ */