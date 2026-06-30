#ifndef _FBCS
#define _FBCS
#define _USE_MATH_DEFINES // para M_PI

#include "Matriz.h"
#include "Vetor.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

// BCS is a class that works with spline interpolation.
// Initially, only two vectors would be required for interpolation, but four vectors will be used,
// with three functions being interpolated based on the first vector.
class BomCentSub {
  public:
    int lenth;       // Vector size.
    double *vaz;     // Flow rate vector in BPD.
    double *head;    // Head vector in ft.
    double *power;   // Power vector in hp.
    double *efic;    // Efficiency vector.
    double *d2head;  // Head second-derivative vector.
    double *d2power; // Power second-derivative vector.
    double *d2efic;  // Efficiency second-derivative vector.
    double BEP;      // Best efficiency point.
    double Qzero;    // Point at which the pump head is maximum.
    double freq;     // Pump frequency in hertz.
    double freqnova;
    double eficM;
    double freqMinima;
    int nestag; // Number of stages.
    double Hvis;
    double Pvis;
    double Evis;
    int correcHI;
    double fracTermMotorEfic;

    double FalsaCorda(int, double, double, double delta = 0.000001, double epsn = 0.000001, int maxit = 100) const; // Calculo de zero de fun��o apenas para calculo de zero na FNDran

    BomCentSub(int n = 0, const double *const Vvaz = 0, const double *const Vhead = 0,
               const double *const Vpower = 0, const double *const Vefic = 0, double Vfreq = 0, int nestag = 1, double VeficM = 100,
               double freqcMinima = 0., int vcorrecHI = 1, double vfracTermMotorEfic = 0.); // construtor
    BomCentSub(const BomCentSub &);                                                         // construtor de copia
    ~BomCentSub() {
        if (lenth > 0 && vaz != 0)
            delete[] vaz;
        if (lenth > 0 && head != 0)
            delete[] head;
        if (lenth > 0 && power != 0)
            delete[] power;
        if (lenth > 0 && efic != 0)
            delete[] efic;
        if (lenth > 1 && d2head != 0)
            delete[] d2head;
        if (lenth > 1 && d2power != 0)
            delete[] d2power;
        if (lenth > 1 && d2efic != 0)
            delete[] d2efic;
    }                                                 // destrutor
    BomCentSub &operator=(const BomCentSub &);        // sobrecarga do operador =
    double operator[](int i) const { return vaz[i]; } // sobrecarga do operador []
    double MelhorVaz() const { return BEP; }
    double MaxVaz() const { return Qzero; }
    int size() const { return lenth; }
    ostream &curva(ostream &, double Vfreq, double estag, int npontos) const; // Output for curve construction.
    double Fhead(double) const;
    double Fpower(double) const;
    double Fefic(double) const;
    double Defic(double) const;
    double segundaH(int i) const { return d2head[i]; }
    double segundaP(int i) const { return d2power[i]; }
    double segundaE(int i) const { return d2efic[i]; }
    void NovaVis(double, double, double);
};

ostream &operator<<(ostream &s, const BomCentSub &); // Pump output values.

#endif