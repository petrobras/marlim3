#ifndef _ACIDENTES
#define _ACIDENTES
#define _USE_MATH_DEFINES // para M_PI
#include "FerramentasNumericas.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "PropVapor.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>

using namespace std;

// The pressure and temperature variables are given in absolute kgf/cm² and degrees Celsius, respectively.
double dpVarA(const double vazmas, const double pres, const double temp, const double areaM, const double areaJ,
              const double alf, const double bet, const ProFlu fluido, const ProFluCol fluidocol);
double dTVarA(const double vazmas, const double, const double, const double, const double, const double, const double,
              const ProFlu, const ProFluCol);
double CdA(const double, const double);

class MudaArea {
  public:
    double AreaMon;
    double AreaJus;
    MudaArea(const double mon = 1, const double jus = 1) {
        AreaMon = mon;
        AreaJus = jus;
    } // construtor
    MudaArea(const MudaArea &antigo) {
        AreaMon = antigo.AreaMon;
        AreaJus = antigo.AreaJus;
    } // construtor de copia
    MudaArea &operator=(const MudaArea &antigo) {
        AreaMon = antigo.AreaMon;
        AreaJus = antigo.AreaJus;
        return *this;
    } // sobrecarga do operador =
    double dp(const double vazmas, const double pres, const double temp, const double alf, const double bet,
              const ProFlu fluido, const ProFluCol fluidocol) const {
        return dpVarA(vazmas, pres, temp, AreaMon, AreaJus, alf, bet,
                      fluido, fluidocol);
    }
    double dT(const double vazmas, const double pres, const double temp, const double alf, const double bet,
              const ProFlu fluido, const ProFluCol fluidocol) const {
        return dTVarA(vazmas, pres, temp, AreaMon, AreaJus, alf, bet,
                      fluido, fluidocol);
    }
};

double VelSomH(const double, const double, const double, const double, const ProFlu, const ProFluCol);
double VelSomG(const double, const double, const ProFlu);
double FunRPresCritH(const double, const double *const);
double RPresCritH(const double pres, const double temp, const double alf, const double bet,
                  const ProFlu fluido, const ProFluCol);
double RPresH(const double, const double, const double, const double, const double, const double, const double,
              const double, const ProFlu, const ProFluCol);
double RPresHVap(const double, const double, const double, const double, const double, const double);
double DPBocConH(const double, const double, const double, const double, const double, const double,
                 const double, const double, const ProFlu, const ProFluCol);
double DPBocConHVap(const double, const double, const double, const double, const double);
double DTBocConH(const double pres, const double temp, const double alf, const double bet, const double delp,
                 const ProFlu fluido, const ProFluCol);
double massica(double presEstag, double presGarg, const double temp, double alf, double bet, double x,
               const ProFlu fluido, const ProFluCol fluidoC,
               const double areaE, double areaG);
double MasMax(double presEstag, double presGarg, const double temp, double alf, double bet,
              double x, const ProFlu fluido, const ProFluCol fluidoC,
              const double areaG);

class choke {
  public:
    double AreaGarg;
    double AreaTub;
    double razpres;
    double cdchk;
    choke(const double tub = 1, const double garg = 1, const double tcd = 0.84); // construtor
    choke(const choke &antigo);                                                  // construtor de copia
    choke &operator=(const choke &antigo);
    double dp(const double pres, const double temp, const double alf, const double bet, const double Qg, const double Ql,
              const ProFlu fluido, const ProFluCol fluidoC) const {
        return DPBocConH(pres, temp, alf, bet, Qg, Ql, AreaTub, AreaGarg, fluido, fluidoC);
    }
    double dpVap(const double pres, const double temp, const double alf, const double Qg, const double Ql) const;
    double dT(const double pres, const double temp, const double alf, const double bet, const double Qg, const double Ql,
              const ProFlu fluido, const ProFluCol fluidoC) const {
        double delp = DPBocConH(pres, temp, alf, bet, Qg, Ql, AreaTub, AreaGarg, fluido, fluidoC) / 98066.5;
        return DTBocConH(pres, temp, alf, bet, delp, fluido, fluidoC);
    }
    double vazmass(double presEstag, double presGarg, const double temp, double alf, double bet, double x,
                   const ProFlu fluido, const ProFluCol fluidoC) {
        return massica(presEstag, presGarg, temp, alf, bet, x, fluido, fluidoC, AreaTub, AreaGarg);
    }
    double maxmass(double presEstag, double presGarg, const double temp, double alf, double bet, double x, const ProFlu fluido, const ProFluCol fluidoC) {
        return MasMax(presEstag, presGarg, temp, alf, bet, x, fluido, fluidoC, AreaGarg);
    }

    double razpresSachdF(double y, double presEstag, double temp, double alf, double bet, double x, const ProFlu fluido, const ProFluCol fluidoC);
    double razpresSachdZ(double a, double b, double presEstag, double temp, double alf, double bet, double x, const ProFlu fluido, const ProFluCol fluidoC,
                         double delta = 0.00001, double epsn = 0.00001, int maxit = 100);
    double vazmassSachd(double y, double presEstag, double temp, double alf,
                        double bet, double x, const ProFlu fluido, const ProFluCol fluidoC, double xmin = 0.01);
    double vazmaxSachd(double presEstag, double temp, double alf,
                       double bet, double x, const ProFlu fluido, const ProFluCol fluidoC, double xmin = 0.01);

    double razpresSachdFVap(double y, double presEstag, double temp, double alf, double x);
    double razpresSachdZVap(double a, double b, double presEstag, double temp, double alf, double x,
                            double delta = 0.00001, double epsn = 0.00001, int maxit = 100);
    double vazmassSachdVap(double y, double presEstag, double temp, double alf, double x);
    double vazmaxSachdVap(double presEstag, double temp, double alf, double x);
};

#endif