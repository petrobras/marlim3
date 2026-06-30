#ifndef _CEL
#define _CEL
#define _USE_MATH_DEFINES // Enables the M_PI mathematical constant.

#include "Geometria.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "acessorios.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>

using namespace std;

struct dadosParafina;

/**
 * Stores intermediate quantities used by the wax-deposition model.
 */
struct detalhaParafina {
    double tempInterDeposito;
    double difusividadeParafina;
    double gradienteConcentracao;
    double fluxMassParafina1;
    double fluxMassParafina2;
    double kDep;
    detalhaParafina() {
        tempInterDeposito = 0.;
        difusividadeParafina = 0.;
        gradienteConcentracao = 0.;
        fluxMassParafina1 = 0.;
        fluxMassParafina2 = 0.;
        kDep = 0.;
    }
    detalhaParafina(const detalhaParafina &temp) {
        tempInterDeposito = temp.tempInterDeposito;
        difusividadeParafina = temp.difusividadeParafina;
        gradienteConcentracao = temp.gradienteConcentracao;
        fluxMassParafina1 = temp.fluxMassParafina1;
        fluxMassParafina2 = temp.fluxMassParafina2;
        kDep = temp.kDep;
    }
    detalhaParafina &operator=(const detalhaParafina &temp) {
        if (this != &temp) {
            tempInterDeposito = temp.tempInterDeposito;
            difusividadeParafina = temp.difusividadeParafina;
            gradienteConcentracao = temp.gradienteConcentracao;
            fluxMassParafina1 = temp.fluxMassParafina1;
            fluxMassParafina2 = temp.fluxMassParafina2;
            kDep = temp.kDep;
        }
        return *this;
    }
};

/**
 * Represents a one-dimensional finite-volume cell and its neighboring state.
 *
 * The class stores geometry, fluid properties, thermodynamic state, mass-transfer
 * terms, heat-transfer data, flow-regime information, pigging state, and the local
 * linear system required by the pressure-velocity coupling algorithm.
 */
class Cel {
  public:
    // Hydrate solver.
    // Model III.
    double V_h_total;
    double V_h_dep;
    double V_h_disp;

    double e_dep;
    double D_h_eff;
    double A_eff;
    double phi_h_disp;

    double phi_h_eff;
    double mu_rel_slurry;
    double mu_base_liq;
    double mu_slurry;

    double j_H;
    double j_G;
    double j_W;
    double V_h;
    double V_w;
    double V_g;
    double FVH;
    double agua_consumida;
    double gas_consumido;

    double agua_consumida_massa;
    double gas_consumido_massa;

    double agua_consumida_massa_step;
    double gas_consumido_massa_step;

    double massa_hidrato;

    DadosGeo dutoL;    // Geometry data for the left control volume.
    DadosGeo duto;     // Geometry data for the current control volume.
    DadosGeo dutoR;    // Geometry data for the right control volume.
    acessorio acsr;    // Accessory associated with the control volume, such as a source, ESP, positive displacement pump, or valve.
    acessorio *acsrL;  // Pointer to the accessory associated with the left control volume.
    ProFlu flui;       // Fluid properties for the current control volume.
    ProFlu *fluiL;     // Pointer to the fluid properties of the left control volume.
    ProFlu *fluiR;     // Pointer to the fluid properties of the right control volume.
    ProFluCol fluicol; // Complementary-fluid properties for the current control volume.
    int fonteCompartilhada;
    double multiFComp;

    // Units: pressure in kgf/cm2; temperature in degrees Celsius; mass flow rate in kg/s; time in seconds;
    // diameter, perimeter, roughness, velocity, and cell length in meters; density in kg/m3;
    // volumetric flow rate in m3/s; angle in radians; light, heavy, and water volumes in Sm3;
    // and heat flux in W/m.

    int correlacaoMR2;
    int posic; // Index of the control-volume center, referenced from the bottom of the well.
    double dt; // Time increment, generally equal to the global time step, but it may differ when a local correction is required.
    double dt1;
    // A local time-step adjustment may be required to correct control-volume fraction constraints.
    double dt2; // Corrected time increment used when the beta factor would exceed its admissible bounds.
    // When this occurs, the time increment is recalculated to prevent the violation,
    // and the corrected value is stored in dt2.
    double dTdt; // Temperature time derivative, optionally included as a correction in the mass-conservation models.

    double dTdtIni;
    double dTdtL; // Temperature time derivative in the left control volume, optionally included in the mass-conservation models.

    double tempR;       // Temperature in the right control volume.
    double tempRini;    // Temperature in the right control volume at the previous time level.
    double temp;        // Temperature in the current control volume.
    double tempini;     // Temperature in the current control volume at the previous time level.
    double tempL;       // Temperature in the left control volume.
    double tempLini;    // Temperature in the left control volume at the previous time level.
    double presL;       // Pressure in the left control volume.
    double presLini;    // Pressure in the left control volume at the previous time level.
    double pres;        // Pressure in the current control volume.
    double presini;     // Pressure in the current control volume at the previous time level.
    double presauxL;    // Pressure at the left boundary of the left control volume.
    double presauxLini; // Pressure at the left boundary of the left control volume
    // at the previous time level.
    double presaux;  // Pressure at the left boundary of the current control volume.
    double dpresaux; // Mean pressure deviation at the left boundary of the current control volume relative to the
    // average pressure of neighboring volumes, including frictional and hydrostatic corrections.
    double presauxini;  // Pressure at the left boundary of the current control volume at the previous time level.
    double presauxR;    // Pressure at the left boundary of the right control volume.
    double presauxRini; // Pressure at the left boundary of the right control volume
    // at the previous time level.
    double presR;    // Pressure in the right control volume.
    double presRini; // Pressure in the right control volume at the previous time level.
    double ML;       // Mixture mass flow rate at the left boundary of the left control volume.
    double MLini;    // Mixture mass flow rate at the left boundary of the left control volume
    // at the previous time level.
    double MC;    // Mixture mass flow rate at the left boundary of the current control volume.
    double MCini; // Mixture mass flow rate at the left boundary of the current control volume
    // at the previous time level.
    double MR;    // Mixture mass flow rate at the left boundary of the right control volume.
    double MRini; // Mixture mass flow rate at the left boundary of the right control volume
    // at the previous time level.
    double MliqiniR;  // Liquid mass flow rate at the left boundary of the right control volume.
    double MliqiniR0; // Liquid mass flow rate at the left boundary of the right control volume
    // at the previous time level.
    double Mliqini;  // Liquid mass flow rate at the left boundary of the current control volume.
    double Mliqini0; // Liquid mass flow rate at the left boundary of the current control volume
    // at the previous time level.
    double MliqiniL;  // Liquid mass flow rate at the left boundary of the left control volume.
    double MliqiniL0; // Liquid mass flow rate at the left boundary of the left control volume
    // at the previous time level.
    double MComp;   // Auxiliary complementary-fluid mass flow rate used only for reverse flow in the steady-state model.
    double presBuf; // Pressure in the current control volume during an intermediate flow-network solution iteration.

    double presLiniBuf; // Pressure in the left control volume during an intermediate flow-network solution iteration;
    // this previous-time-level buffer is currently unused.
    double presLBuf; // Pressure in the left control volume during an intermediate flow-network solution iteration.

    double presRBuf; // Pressure in the right control volume during an intermediate flow-network solution iteration.

    double presauxBuf; // Pressure at the left boundary of the current control volume during an intermediate
    // flow-network solution iteration.
    double MCBuf; // Mixture flow rate at the left boundary of the current control volume during an intermediate
    // flow-network solution iteration.
    double MRBuf; // Mixture flow rate at the left boundary of the right control volume during an intermediate
    // flow-network solution iteration.
    double MliqiniRBuf; // Liquid flow rate at the left boundary of the right control volume during an intermediate
    // flow-network solution iteration.
    double MRiniBuf; // Currently unused.
    double MLBuf;    // Mixture flow rate at the left boundary of the left control volume during an intermediate
    // flow-network solution iteration.
    double MliqiniBuf; // Liquid flow rate at the left boundary of the current control volume during an intermediate
    // flow-network solution iteration.
    double MliqiniLBuf; // Liquid flow rate at the left boundary of the left control volume during an intermediate
    // flow-network solution iteration.
    double alfL;    // Void fraction in the left control volume.
    double alfLini; // Void fraction in the left control volume at the previous time level.
    double alfR;    // Void fraction in the right control volume.
    double alfRini; // Void fraction in the right control volume at the previous time level.
    double alf;     // Void fraction in the current control volume.
    double alfIter;
    double alfini;  // Void fraction in the current control volume at the previous time level.
    double betL;    // Beta fraction in the left control volume.
    double betLini; // Beta fraction in the left control volume at the previous time level.
    double betR;    // Beta fraction in the right control volume.
    double betRini; // Beta fraction in the right control volume at the previous time level.
    double bet;     // Beta fraction in the current control volume.
    double betini;  // Beta fraction in the current control volume at the previous time level.
    double betLI;   // Beta fraction at the left boundary of the left control volume.
    double betLIini;
    double betI; // Beta fraction at the left boundary of the current control volume.
    double betIini;
    double betRI; // Beta fraction at the left boundary of the right control volume.
    double betRIini;
    double FW;          // In-situ BSW of the current control volume.
    double FWini;       // In-situ BSW of the current control volume at the previous time level.
    double dxL;         // Length of the left control volume.
    double dx;          // Length of the current control volume.
    double dxR;         // Length of the right control volume.
    double razdxTM;     // Currently unused.
    double razdxTM0;    // Currently unused.
    double massfonteCH; // Currently unused.
    // Model T1 and T2 terms.
    double term1L; // T1 at the left boundary of the left control volume.
    double term2L; // T2 at the left boundary of the left control volume.
    double term1;  // T1 at the left boundary of the current control volume.
    double term2;  // T2 at the left boundary of the current control volume.
    double term1R; // T1 at the left boundary of the right control volume.
    double term2R; // T2 at the left boundary of the right control volume.
    double c0;     // Distribution parameter at the left boundary of the current control volume.
    double ud;     // Drift velocity at the left boundary of the current control volume.
    double c0ini;
    double udini;
    double c0Spare; // Auxiliary distribution parameter used when the flow regime or inclination changes between
    // the current and left control volumes. It stores the value associated with the previous
    // flow regime and is averaged with the distribution parameter of the current control volume.
    double udSpare; // Auxiliary drift velocity used when the flow regime or inclination changes between
    // the current and left control volumes.
    // Mass-transfer terms used by the mass-conservation equations.
    double transmassL;
    double transmassLini;
    double transmassR;
    double transmassRini;
    double FonteMudaFase;
    double FonteMudaFaseini;
    double DTransDt1;
    double DTransDt1ini;
    double DTransDt0;
    double DTransDt0ini;
    double DTransDxR;
    double DTransDxRini;
    double DTransDxL;
    double DTransDxLini;
    double DTransDxLp;
    double DTransDxRp;
    double DTransDxLpini;
    double DTransDxRpini;
    double DTransDx;
    double DTransDtp;
    double DTransDtpL;
    double DTransDtpini;
    double DTransDtT;
    double DTransDtTL;
    double DtransDxLinear;
    double coefTransBetini;
    double coefTransBet;
    double CoefDTRini;
    double CoefDTR;
    double CoefDTLini;
    double CoefDTL;
    double fontedissolv;
    double ativaDeri;
    int TMModel; // Mass-transfer model used in the current control volume: 0 selects the complete model,
    // 1 selects the explicit model, and 2 selects a simplified low-relevance model.
    int TMModelL;          // Mass-transfer model used in the left control volume.
    double fontemassLL;    // Hydrocarbon-liquid mass source in the left control volume.
    double fontemassLLini; // Hydrocarbon-liquid mass source in the left control volume at the previous time level.
    double fontemassGL;    // Gas mass source in the left control volume.
    double fontemassGLini; // Gas mass source in the left control volume at the previous time level.
    double fontemassCL;    // Complementary-liquid mass source in the left control volume.
    double fontemassCLini; // Complementary-liquid mass source in the left control volume at the previous time level.
    double fontemassLR;    // Hydrocarbon-liquid mass source in the current control volume.
    double fontemassLRini; // Hydrocarbon-liquid mass source in the current control volume at the previous time level.
    double fontemassGR;    // Gas mass source in the current control volume.
    double fontemassGRini; // Gas mass source in the current control volume at the previous time level.
    double fontemassCR;    // Complementary-liquid mass source in the current control volume.
    double fontemassCRini; // Complementary-liquid mass source in the current control volume at the previous time level.
    double dpB;            // Pressure gain at the right boundary of the current control volume due to an ESP.
    double potB;           // Power transferred to the fluid by an ESP or through a pressure increase.
    double potBT;          // Power transferred to the fluid by pumping equipment, accounting for
    // motor efficiency.
    double potTermo;
    double dpBL;
    double potBL;
    double potBTL;
    int transic; // Time-step counter used to smoothly transition the drift-flux parameters
    // when the flow regime changes between consecutive time steps.
    int transic0;
    int arranjo;         // Flow-regime indicator at the left boundary of the current control volume.
    int arranjoR;        // Flow-regime indicator at the right boundary of the current control volume.
    double chutearranjo; // Currently unused.
    double QLL;          // Liquid-mixture volumetric flow rate at the left boundary of the left control volume.
    double QLLini;       // Liquid-mixture volumetric flow rate at the left boundary of the left control volume at the previous time level.
    double QL;           // Liquid-mixture volumetric flow rate at the left boundary of the current control volume.
    double QLini;        // Liquid-mixture volumetric flow rate at the left boundary of the current control volume at the previous time level.
    double QLR;          // Liquid-mixture volumetric flow rate at the left boundary of the right control volume.
    double QLRini;       // Liquid-mixture volumetric flow rate at the left boundary of the right control volume at the previous time level.
    double QG;           // Gas volumetric flow rate at the left boundary of the current control volume.
    double QGini;        // Gas volumetric flow rate at the left boundary of the current control volume at the previous time level.
    double rpL;          // Hydrocarbon-liquid density in the left control volume.
    double rpC;          // Hydrocarbon-liquid density in the current control volume.
    double rpR;          // Hydrocarbon-liquid density in the right control volume.
    double rcL;          // Complementary-liquid density in the left control volume.
    double rcC;          // Complementary-liquid density in the current control volume.
    double rcR;          // Complementary-liquid density in the right control volume.
    double rgL;          // Gas density in the left control volume.
    double rgC;          // Gas density in the current control volume.
    double rgR;          // Gas density in the right control volume.
    double rpLi;         // Hydrocarbon-liquid density at the left boundary of the left control volume.
    double rpCi;         // Hydrocarbon-liquid density at the left boundary of the current control volume.
    double rpRi;         // Hydrocarbon-liquid density at the left boundary of the right control volume.
    double rcLi;         // Complementary-liquid density at the left boundary of the left control volume.
    double rcCi;         // Complementary-liquid density at the left boundary of the current control volume.
    double rcRi;         // Complementary-liquid density at the left boundary of the right control volume.
    double rgLi;         // Gas density at the left boundary of the left control volume.
    double rgCi;         // Gas density at the left boundary of the current control volume.
    double rgRi;         // Gas density at the left boundary of the right control volume.
    double drhogdt;      // Time derivative of gas density in the current control volume.
    double mipC;
    double micC;
    double migC;
    double mipR;
    double micR;
    double migR;
    Vcr<double> TL;        // Right-hand-side vector of the local linear system.
    FullMtx<double> local; // Local coefficient matrix.
    double perdaEstratL;   // Currently unused.
    double perdaEstratG;   // Currently unused.
    double cinematico;     // Currently unused.
    double VTemper;        // Temperature advection velocity at the left boundary of the current control volume.
    double VTemperini;     // Temperature advection velocity at the left boundary of the current control volume
    // at the previous time level.
    TransCal calor;    // Heat-transfer model associated with the current control volume.
    double fluxcalmed; // Heat flux.

    int estadoPig; // Indicates whether a pig is traveling through the current control volume.
    int estadoPigini;
    double dtPig; // Corrected time increment used when a pig would cross from one control volume to another
    // within a single time step. In this case, the time increment is recalculated so that
    // the pig reaches the control-volume boundary exactly, and the corrected value
    // is stored in dtPig.
    double alfPigE;    // Void fraction to the left of the pig when it is in the current control volume.
    double alfPigER;   // Void fraction to the left of the pig when it is in the right control volume.
    double alfPigEini; // Void fraction to the left of the pig in the current control volume
    // at the previous time level.
    double alfPigERini;
    double alfPigD;    // Void fraction to the right of the pig when it is in the current control volume.
    double alfPigDini; // Void fraction to the right of the pig in the current control volume
    // at the previous time level.
    double betPigE;    // Beta fraction to the left of the pig when it is in the current control volume.
    double betPigEini; // Beta fraction to the left of the pig in the current control volume
    // at the previous time level.
    double betPigD;    // Beta fraction to the right of the pig when it is in the current control volume.
    double betPigDini; // Beta fraction to the right of the pig in the current control volume
    // at the previous time level.
    double velPig;    // Pig velocity.
    double velPigini; // Pig velocity at the previous time level.
    double razPig;    // Relative pig position in the current control volume: 0 at the left boundary and 1 at the right boundary.
    double razPigini; // Relative pig position in the current control volume at the previous time level:
    // 0 at the left boundary and 1 at the right boundary.
    double RazAreaPig; // Currently unused.
    double DelPig;     // Currently unused.
    double VazaPig;    // Currently unused.
    double cdpig;      // Currently unused.
    int indpig;        // Index that identifies the pig position in the launch-time queue.
    int indpigini;

    double VolLeveST; // Light-component volume in the current control volume.
    double VolPesaST; // Heavy-component volume in the current control volume.
    double VolAguaST; // Water volume in the current control volume.

    double DmasschokeG; // Derivative of the gas mass flow rate through the surface choke with respect to pressure;
    // used to increase choke-model implicitness only in the last system cell.
    double DmasschokeL; // Derivative of the hydrocarbon-liquid mass flow rate through the surface choke with respect to pressure;
    // used to increase choke-model implicitness only in the last system cell.
    double DmasschokeC; // Derivative of the complementary-liquid mass flow rate through the surface choke with respect to pressure;
    // used to increase choke-model implicitness only in the last system cell.

    double corrigeMassaPres; // Currently unused.
    int mudaDT;
    int mudaDTL;
    double dpdt;
    double dpdtIni;
    double m2d;
    double m2dL;
    double d2pdt2;
    double termoHidro; // Hydrostatic pressure variation between the current and right control volumes.
    double termoFric;  // Frictional pressure variation between the current and right control volumes.

    double termRug; // Contribution of the pipe relative roughness to the friction-factor calculation.

    double angEsq; // Last nonzero-magnitude angle to the left of the current control volume,
    // used to determine the sign of the drift velocity in a horizontal pipe.

    double angDir; // Last nonzero-magnitude angle to the right of the current control volume,
    // used to determine the sign of the drift velocity in a horizontal pipe.


    int estabCol;
    int mudaArea;

    double nMol;
    double nMolIni;

    double dR;

    int indGeom;

    double dPdLHidro;
    double dPdLFric;
    double dTdLCor;

    int inddPdLHidro;
    int inddPdLFric;
    int inddTdLCor;

    int reiniciaAlf;
    int reiniciaBet;
    int reiniciaPig;

    int correrGlobHol;

    int miniTabAtraso;

    varGlob1D *vg1dSP;

    int modoParafina;
    int parafinado;
    double deltaPar;

    double fluxCal2D;

    double C2;
    double C3;
    double diamInterIni;
    double porosoPar;
    double MW_wax;
    double rhoWaxLiq;
    double Sum_dCwaxdT;

    double fluxcalAcopRedeP;
    double resAcopRedeP;

    double fonteCal;

    detalhaParafina detParCel;

    Cel(varGlob1D *Vvg1dSP = 0, const DadosGeo vdutoL = DadosGeo(),
        const DadosGeo vduto = DadosGeo(),
        const DadosGeo vdutoR = DadosGeo(),
        const ProFlu vflui = ProFlu(),
        const ProFluCol vfluiCol = ProFluCol(),
        const double vtempL = 25., const double vtemp = 25.,
        const double vtempR = 25., const double vtempini = 25.,
        const double vpresL = 10., const double vpres = 10., const double vpresR = 10.,
        const double vML = 0., const double vMC = 0., const double vMR = 0.,
        const double vMliqiniL = 0., const double vMliqini = 0., const double vMliqiniR = 0.,
        const double valfL = 1., const double valf = 1., const double valfR = 1.,
        const double vbetL = 1., const double vbet = 1., const double vbetR = 1.,
        const double vdxL = 1., const double vdx = 1., const double vdxR = 1.,
        const double vdt = -1.,
        const int vposic = -1,
        const TransCal vcalor = TransCal(),
        const acessorio vacsr = acessorio(),
        acessorio *vacsrL = 0); // Default constructor.
    Cel(const Cel &);           // Copy constructor.
    Cel &operator=(const Cel &);

    double Rey(double dia /* diameter */, double vel /* velocity */,
               double rho /* density */, double vis /* viscosity in cP */); // Computes the Reynolds number.
    double fric(double re /*Reynolds*/, double eps /* relative roughness */,
                int tipo = 1 /* indicates whether the roughness contribution to the friction factor must be recalculated or
                   whether the previously computed term should be reused; 1 reuses the previous term */
    );                       // Computes the friction factor.
    void avancalf(int &reinicia /* indicates whether the time increment must be corrected */,
                  int ncel /* number of cells in the system */); // Advances the void-fraction solution.
    void avancbet(int &reinicia /* indicates whether the time increment must be corrected */,
                  int ncel /* number of cells in the system */); // Advances the beta-fraction solution.
    double delpJus(double presfim);
    void GeraLocal(double presfim /* separator pressure */,
                   int masChkSup,
                   int ncel /* number of cells in the system */,
                   double razareativa /* indicates that master1 is partially open and its resistance
                     must therefore be included in the model */
                   ,
                   double presE /* inlet pressure boundary-condition value;
                    presE < 0 represents a closed boundary */
                   ,
                   double tempE /* inlet temperature for the pressure boundary condition */,
                   double titE = -1 /* inlet quality for the pressure boundary condition */,
                   double betE = -1 /* inlet beta value for the pressure boundary condition */,
                   int ciclo = 0 /* iteration counter controlling inclusion of dTdtL in mixture mass conservation;
                      at cycle 0 the term is omitted and is included from cycle 1 onward
                      when iterative correction is enabled */,
                   int modelo = 1, int noextremo = 0, int corrigeContSep = 1, double areaChoke = 1., int vexpi = 0); // Builds the local system used for pressure-velocity coupling.

    void avancPig(int &reinicia); // Computes the new relative pig position in the control volume. If it exceeds 1,
    // the time increment is corrected so that the position reaches exactly 1.
    void avancalfPig(); // Updates the void fractions to the left and right of the pig in the control volume.
    void avancbetPig(); // Updates the beta fractions to the left and right of the pig in the control volume.

    void avancalfPigExpli(); // Explicitly updates the void fractions to the left and right of the pig.
    void avancbetPigExpli(); // Explicitly updates the beta fractions to the left and right of the pig.

    void FeiticoDoTempo(); // Restores values after a time-evolution iteration
    // to their original values from the previous time level.

    void FeiticoDoTempo2(); // Restores values after a time-evolution iteration
    // to their original values from the previous time level.

    void FeiticoDoTempo3(); // Restores values after a time-evolution iteration
    // to their original values from the previous time level.
    double SIGN(double a, double b); // Helper function used by zriddr.
    double zriddrRhop(double x1, double x2, double rp);

    void expiAlfaBetaPres(int &reinicia, int ncel);
    void expiMassMist(double presfim, int masChkSup, int ncel, double razareativa,
                      double presE,
                      double tempE,
                      double titE = -1,
                      double betE = -1,
                      int noextremo = 0, double areaChoke = 1.);
    double somVel();
    double termAdSomVel();
    void WaxDeposition(dadosParafina &detalParafina, int ncel);

    FullMtx<double> Jacobiana();
    Vcr<double> autoVec(int ind, FullMtx<double> jac);
    Vcr<double> flutua();
};

#endif