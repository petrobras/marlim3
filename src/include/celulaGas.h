#ifndef _CELG
#define _CELG
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "Geometria.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "chokegas.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>

using namespace std;

/*!
 * Represent a control volume in the gas-injection and service-line model.
 *
 * Unless otherwise stated, the following units are used:
 * - Pressure: kgf/cm2
 * - Temperature: degrees Celsius
 * - Mass flow rate: kg/s
 * - Time: seconds
 * - Diameter, perimeter, roughness, velocity, and cell length: meters
 * - Density: kg/m3
 * - Volumetric flow rate: m3/s
 * - Angle: radians
 * - Standard volumes of light component, heavy component, and water: Sm3
 * - Heat flow per unit length: W/m
 */
class CelG {

  public:
    // Hydrate solver variables.
    double j_H;
    double j_G;
    double j_W;
    double V_h;
    double V_w;
    double FVHG;
    double agua_consumida;
    double massa_hidrato;

    DadosGeo dutoL; // Geometry data for the control volume to the left.
    DadosGeo duto;  // Geometry data for the current control volume.
    DadosGeo dutoR; // Geometry data for the control volume to the right.
    ProFlu flui;    // Fluid properties for the current control volume.
    TransCal calor; // Heat-transfer model associated with the control volume.
    int posic;      // Control-volume center index, referenced from the platform.
    int tipoCC;     // Gas-injection boundary-condition type: 0 -> pressure, 1 -> flow rate.
    double dt;      // Time-step increment.
    double dTdt;    // Temperature time derivative used as a correction term in mass-conservation models.
    double tempL;   // Temperature in the control volume to the left.
    double temp;    // Temperature in the current control volume.
    double tempR;   // Temperature in the control volume to the right.
    double presL;   // Pressure in the control volume to the left.
    double pres;    // Pressure in the current control volume.
    double presR;   // Pressure in the control volume to the right.
    double VGasL;   // Gas mass flow rate at the left boundary of the current control volume.
    double VGasR;   // Gas mass flow rate at the right boundary of the current control volume.
    double VGasRBuf; // Buffered gas mass flow rate at the right boundary during an intermediate
                     // transient-network convergence iteration.
    double VGasRR;      // Gas mass flow rate at the right boundary of the control volume to the right.
    double dx0;         // Length of the current control volume.
    double dx1;         // Length of the control volume to the right.
    double dxL;         // Length of the control volume to the left.
    double u1LL;        // Product of density and pipe cross-sectional area in the control volume to the left.
    double u1L;         // Product of density and pipe cross-sectional area in the current control volume.
    double u1R;         // Product of density and pipe cross-sectional area in the control volume to the right.
    double massfonteCH; // Mass-source term in the current control volume.
    double fluxcal;     // Heat flow.
    double salinidade;  // Completion-fluid salinity used by the gas-lift unloading model.
    int labelchk;       // Indicates whether a Master2 valve is installed at the right boundary: 1 when present.
    int fechamon;       // Currently unused.
    double razInter;    // Relative position of the gas/completion-fluid interface in the gas-lift unloading model:
                        // 0 -> left boundary of the control volume, 1 -> right boundary.
    double razInterIni; // Relative interface position at the previous time level.
    int *celInter;      // Pointer to the index of the control volume containing the gas/completion-fluid interface.
                        // Used during gas-lift unloading to identify the interface volume in GeraLocal().
    ChokeGas chkcell;      // Gas-injection choke associated with the control volume.
    double rpchk;          // Currently unused.
    Vcr<double> TL;        // Right-hand-side vector of the local system.
    FullMtx<double> local; // Local system matrix.

    double fonteM2;        // Auxiliary Master2 mass-source term when the choke is active at this valve.
    double tempLini;       // Left-control-volume temperature at the previous time level.
    double tempini;        // Current-control-volume temperature at the previous time level.
    double tempRini;       // Right-control-volume temperature at the previous time level.
    double presLini;       // Left-control-volume pressure at the previous time level.
    double presini;        // Current-control-volume pressure at the previous time level.
    double presRini;       // Right-control-volume pressure at the previous time level.
    double VGasLini;       // Gas mass flow rate at the left boundary at the previous time level.
    double VGasRini;       // Gas mass flow rate at the right boundary at the previous time level.
    double VGasRRini;      // Gas mass flow rate at the right boundary of the right control volume at the previous time level.
    double u1LLini;        // Previous-time-level density-area product in the control volume to the left.
    double u1Lini;         // Previous-time-level density-area product in the current control volume.
    double u1Rini;         // Previous-time-level density-area product in the control volume to the right.
    double massfonteCHini; // Mass-source term at the previous time level.
    double fonteM2ini;     // Auxiliary Master2 mass-source term at the previous time level when the choke is active.
    int fechamonini;  // Currently unused.
    int *celInterini; // Pointer to the interface-volume index at the previous time level.
                      // Used during gas-lift unloading to identify the interface volume in GeraLocal().
    int posicini;    // Currently unused.
    double rpchkini; // Currently unused.
    int vgl;         // Indicates that the control volume contains a gas-lift valve; used for trend output.
    double tEstag;   // Temperature upstream of the gas-lift valve, used for trend output.
    double pEstag;   // Pressure upstream of the gas-lift valve, used for trend output.
    double tGarg;    // Temperature downstream of the gas-lift valve, used for trend output.
    double pGarg;    // Pressure downstream of the gas-lift valve, used for trend output.
    double qGarg;    // Gas-lift valve flow rate in std m3/day, used for trend output.
    double areaGarg; // Gas-lift valve orifice area.

    double rg;
    double rgR;

    int indGeom;

    double dPdLHidro;
    double dPdLFric;
    double dTdLCor;

    int inddPdLHidro;
    int inddPdLFric;
    int inddTdLCor;

    double termoHidro; // Hydrostatic pressure variation between the current and right control volumes.
    double termoFric;  // Frictional pressure variation between the current and right control volumes.

    CelG(const DadosGeo vdutoL = DadosGeo(),
         const DadosGeo vduto = DadosGeo(),
         const DadosGeo vdutoR = DadosGeo(),
         const ProFlu vflui = ProFlu(),
         const ChokeGas vchkcell = ChokeGas(),
         const double vtempL = 25., const double vtemp = 25., const double vtempR = 25.,
         const double vpresL = 10., const double vpres = 10., const double vpresR = 10.,
         const double vVGasL = 0., const double vVGasR = 0., const double vVGasRR = 0.,
         const double vu1L = 0., const double vu1R = 0., const double vu1LL = 0.,
         const double vdx0 = 0., const double vdx1 = 0., const double vdxL = 0.,
         const double vdt = -1., const int vposic = -1, const int vfecham = 0, const int vtipoCC = 0,
         const TransCal vcalor = TransCal()); // Default constructor.
    CelG(const CelG &);                       // Copy constructor.
    CelG &operator=(const CelG &);

    double Rey(double dia /* Pipe diameter. */,
               double vel /* Mean flow velocity. */,
               double rho /* Density. */,
               double vis /* Viscosity. */); // Calculate the Reynolds number.
    double fric(double re /* Reynolds number. */,
                double eps /* Relative roughness. */); // Calculate the friction factor.
    double MasEspFlu(double pres, double temp) const; // Calculate the completion-fluid density.
    double VisFlu(double pres, double temp) const;    // Calculate the completion-fluid viscosity.
    double CalorLiq(double pres, double temp) const;  // Calculate the completion-fluid specific heat.
    double CondLiq(double pres, double temp) const;   // Calculate the completion-fluid thermal conductivity.
    double DrhoDtFlu(double pres, double temp) const; // Calculate the completion-fluid density derivative
                                                     // with respect to temperature in the service line.
    double psia(const double p) const { return (p * 0.9678411) * 14.69595; }        // Convert pressure from kgf/cm2 to psia.
    double Faren(const double t) const { return 1.8 * t + 32; }                     // Convert temperature from degrees Celsius to degrees Fahrenheit.
    void GeraLocal(int ncelGas, double presiniG, double tempiniG, double abertura); // Assemble the local matrix for the
                                                                                   // i-th service-line control volume.
    void FeiticoDoTempo();     // Restore control-volume attributes from the previous time level.
    void DeVoltaParaoFuturo(); // Save the most recent values before advancing to a new time level.
};

#endif