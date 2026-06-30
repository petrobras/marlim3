/*
 * SisProd.h
 *
 * Created on: December 21, 2016
 *     Author: Eduardo
 */

#ifndef SISPROD_H_
#define SISPROD_H_
#define _USE_MATH_DEFINES // Enables M_PI on supported platforms

#include "Acidentes2.h"
#include "Bcsm2.h"
#include "BombaVol.h"
#include "FerramentasNumericas.h"
#include "FonteMas.h"
#include "FonteMassCHK.h"
#include "Geometria.h"
#include "GradientCorrelations.h"
#include "Leitura.h"
#include "Log.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "TrocaCalor.h"
#include "Vetor.h"
#include "acessorios.h"
#include "celula3.h"
#include "celulaGas.h"
#include "chokegas.h"
#include "criterioIntermiSevera.h"
#include "dados3DPoisson.h"
#include "estrat.h"
#include "estruturaTabDin.h"
#include "mapa.h"
#include "multiBCS.h"
#include "solver3DPoisson.h"
#include "variaveisGlobais1D.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

/// Application or simulator version string.
extern string versao;
/// Global simulation start timestamp.
extern time_t nowGlobIni;
/// Broken-down local time corresponding to nowGlobIni.
extern tm *ltmGlobIni;
/// Day component of the simulation start time.
extern int diaIni;
/// Hour component of the simulation start time.
extern int horaIni;
/// Minute component of the simulation start time.
extern int minutoIni;
/// Second component of the simulation start time.
extern int segundoIni;
/// Global simulation end timestamp.
extern time_t nowGlobFim;
/// Broken-down local time corresponding to nowGlobFim.
extern tm *ltmGlobFim;

/**
 * @brief Models and solves a one-dimensional production system.
 *
 * The class owns the production and gas-service-line state, fluid-property
 * tables, boundary conditions, network-coupling data, transient buffers,
 * and steady-state/transient solution procedures.
 */
class SProd {
  public:
    /**
     * @brief Selects the specific-heat model used by the black-oil formulation. The value is read from the JSON
     * input and determines whether specific heat is calculated by the internal black-oil model or obtained
     * from a PVT table.
     */
    int ModelCp;
    /**
     * @brief Selects the liquid Joule-Thomson model used by the black-oil formulation. The value is read from the
     * JSON input and determines whether the coefficient is calculated internally or obtained from a PVT
     * table.
     */
    int Modeljtl;
    /**
     * @brief Enables latent-heat calculations for black-oil simulations.
     */
    int CalcLat;
    /**
     * @brief Enables transport equations for primitive black-oil properties, including API gravity, BSW, gas-oil
     * ratio, and light/heavy mass fractions.
     */
    int trackRGO;
    /**
     * @brief Enables transport equations for gas density and the gas-phase CO2 molar fraction.
     */
    int trackDeng;
    /**
     * @brief Number of gas sources; retained for compatibility with arq.ninjgas.
     */
    int ninjgas;
    /**
     * @brief Indicates whether a gas service line is coupled to the production system.
     */
    int lingas;
    /**
     * @brief Indicates that the current case represents an injection well.
     */
    int injPoc;

    /**
     * @brief Section index when this object belongs to a pipeline network.
     */
    int indTramo;
    /**
     * @brief Number of control volumes in the production line.
     */
    int ncel;
    /**
     * @brief Requests rollback and time-step reevaluation when at least one control volume produces a holdup or
     * volume fraction outside the physical [0, 1] range.
     */
    int reinicia;

    /**
     * @brief Control parameter for slowly varying thermal coupling.
     */
    double trocaTermicaLenta;
    /**
     * @brief Recent Master1 ratio values for the active state.
     */
    double vRazMast1[10];
    /**
     * @brief Recent Master1 ratio values for the inactive state.
     */
    double vRazMast0[10];
    /**
     * @brief Critical Master1 ratio history used by the switching logic.
     */
    double vRazMastCrit[10];

    /**
     * @brief Current inlet pressure boundary condition.
     */
    double presE;
    /**
     * @brief Current inlet temperature boundary condition.
     */
    double tempE;
    /**
     * @brief Current inlet gas mass fraction.
     */
    double titE;
    /**
     * @brief Computed inlet void fraction.
     */
    double alfE;
    /**
     * @brief Computed inlet complementary-liquid fraction.
     */
    double betaE;
    /**
     * @brief Inlet pressure stored at the previous time level.
     */
    double presEini;
    /**
     * @brief Inlet temperature stored at the previous time level.
     */
    double tempEini;
    /**
     * @brief Inlet gas mass fraction stored at the previous time level.
     */
    double titEini;
    /**
     * @brief Inlet void fraction stored at the previous time level.
     */
    double alfEini;
    /**
     * @brief Inlet complementary-liquid fraction stored at the previous time level.
     */
    double betaEini;

    /**
     * @brief Pressure in the last production-line control volume. When the surface choke is open, this value is
     * equal to the separator pressure.
     */
    double presfim;
    /**
     * @brief Previous-time-level value of presfim.
     */
    double presfimini;
    /**
     * @brief Gas mass fraction imposed during reverse flow at the last control volume. Used only by transient
     * network simulations.
     */
    double titRev;
    /**
     * @brief Previous-time-level value of titRev.
     */
    double titRevini;
    /**
     * @brief Complementary-liquid fraction imposed during reverse flow at the last control volume. Used only by
     * transient network simulations.
     */
    double betaRev;
    /**
     * @brief Previous-time-level value of betaRev.
     */
    double betaRevini;
    /**
     * @brief Separator pressure or pressure at the inlet of the downstream network section.
     */
    double pGSup;
    /**
     * @brief Previous-time-level value of pGSup.
     */
    double pGSupIni;
    /**
     * @brief Previous-time-level downstream or separator temperature.
     */
    double tGSupIni;
    /**
     * @brief Separator temperature or temperature at the inlet of the downstream section.
     */
    double tGSup;
    /**
     * @brief Production-line inlet temperature when no inlet-pressure boundary condition is imposed.
     */
    double temperatura;

    /**
     * @brief Reserved mass-flow state; currently unused.
     */
    double masSup;
    /**
     * @brief Reserved temperature state; currently unused.
     */
    double tempSup;
    /**
     * @brief Number of control volumes in the gas service line.
     */
    int ncelGas;
    /**
     * @brief Gas-injection pressure.
     */
    double presiniG;
    /**
     * @brief Gas-injection temperature in the service line.
     */
    double tempiniG;
    /**
     * @brief Reserved source mass-flow value; currently unused.
     */
    double massfonte;
    /**
     * @brief CFL safety factor, typically set to 0.8.
     */
    double mult;

    /**
     * @brief Time-averaged pressure in the final production-line control volume. Used to decide whether the
     * surface choke behaves as a localized pressure loss or as a discharge-flow model.
     */
    double presMedMov;
    /**
     * @brief Time-averaged mixture volumetric flux in the final production-line control volume. Used by the
     * surface-choke operating-mode logic.
     */
    double jMedMov;
    /**
     * @brief Time-averaged void fraction in the final production-line control volume. Used by the surface-choke
     * operating-mode logic.
     */
    double alfMedMov;
    /**
     * @brief Start time of the moving-average window.
     */
    double tMedMov;
    /**
     * @brief Duration of the moving-average window.
     */
    double ktMedMov;
    /**
     * @brief Accumulated pressure used to compute presMedMov.
     */
    double pTotal;
    /**
     * @brief Accumulated mixture flux used to compute jMedMov.
     */
    double jTotal;
    /**
     * @brief Accumulated void fraction used to compute alfMedMov.
     */
    double alfTotal;
    /**
     * @brief Pressure samples used by the moving-average calculation.
     */
    vector<double> presVet;
    /**
     * @brief Mixture-flux samples used by the moving-average calculation.
     */
    vector<double> jVet;
    /**
     * @brief Void-fraction samples used by the moving-average calculation.
     */
    vector<double> alfVet;
    /**
     * @brief Time samples associated with the moving-average window.
     */
    vector<double> tVet;

    /**
     * @brief Current surface-choke open/closed state.
     */
    int aberto;
    /**
     * @brief Previous-time-level surface-choke state.
     */
    int abertoini;
    /**
     * @brief Counter that delays transitions out of active-choke mode.
     */
    int tempoaberto;
    /**
     * @brief Previous-time-level value of tempoaberto.
     */
    int tempoabertoini;
    /**
     * @brief Current Master1 valve state.
     */
    int EstadoMaster1;
    /**
     * @brief Counter used while changing the Master1 state.
     */
    int contaMaster1;
    /**
     * @brief Indicates whether the surface choke is active.
     */
    int masChkSup;
    /**
     * @brief Previous-time-level value of masChkSup.
     */
    int masChkSupini;
    /**
     * @brief Signals a surface-choke operating-mode transition.
     */
    int mudaModoChk;
    /**
     * @brief Previous-time-level value of mudaModoChk.
     */
    int mudaModoChkini;
    /**
     * @brief Selects the interphase mass-transfer model: 0 = complete, 1 = fully explicit, 2 = simplified, and 3
     * = disabled.
     */
    int TransMassModel;
    /**
     * @brief Number of pigs currently moving through the line.
     */
    int indpigP;
    /**
     * @brief Previous-time-level value of indpigP.
     */
    int indpigPini;
    /**
     * @brief Number of pigs scheduled for launch.
     */
    int npig;
    /**
     * @brief Cell indices where pigs are received.
     */
    int *receb;

    /**
     * @brief Number of points in the fluid-property tables.
     */
    int npontos;
    /**
     * @brief Number of production fluids.
     */
    int nfluP;
    /**
     * @brief Black-oil gas-compressibility-factor table.
     */
    double **zdranP;
    /**
     * @brief Pressure derivative of the black-oil compressibility-factor table.
     */
    double **dzdpP;
    /**
     * @brief Temperature derivative of the black-oil compressibility-factor table.
     */
    double **dzdtP;
    /**
     * @brief Black-oil gas specific-heat table.
     */
    double **cpg;
    /**
     * @brief Black-oil produced-liquid specific-heat table.
     */
    double **cpl;
    /**
     * @brief Temperature derivative of liquid density.
     */
    double **drholdT;
    /**
     * @brief Black-oil latent-heat table.
     */
    double **HLat;

    /**
     * @brief Parsed user input and simulation configuration.
     */
    Ler arq;
    /**
     * @brief Buffer used to write gas-line profiles.
     */
    FullMtx<double> flutG;
    /**
     * @brief Buffer used to write production-line profiles.
     */
    FullMtx<double> flut;
    /**
     * @brief Global pressure-velocity coupling matrix for the gas line.
     */
    BandMtx<double> matglobG;
    /**
     * @brief Right-hand side and solution vector for the gas-line pressure-velocity system.
     */
    Vcr<double> termolivreG;
    /**
     * @brief Global pressure-velocity coupling matrix for the multiphase production line.
     */
    BandMtx<double> matglobP;
    /**
     * @brief Right-hand side and solution vector for the production-line pressure-velocity system.
     */
    Vcr<double> termolivreP;

    /**
     * @brief Current time step.
     */
    double dt;
    /**
     * @brief Time step used at the previous time level.
     */
    double dtini;
    /**
     * @brief Simulation end time.
     */
    double tfinal;

    /**
     * @brief Production-line cell indices associated with gas-lift valves.
     */
    int *posicVGLP;
    /**
     * @brief Service-line cell indices associated with gas-lift valves.
     */
    int *posicVGLG;

    /**
     * @brief Number of scheduled Master1 opening events.
     */
    int nabreM1;
    /**
     * @brief Number of scheduled Master1 closing events.
     */
    int nfechaM1;
    /**
     * @brief Times at which Master1 closes.
     */
    double *fechaM1;
    /**
     * @brief Times at which Master1 opens.
     */
    double *abreM1;

    /**
     * @brief Gas-line cells where radial temperature profiles are written.
     */
    int *ncelperftransg;
    /**
     * @brief Maximum number of samples stored for each gas-line trend.
     */
    int *TrendLengthG;
    /**
     * @brief Buffered gas-line trend data.
     */
    double ***MatTrendG;
    /**
     * @brief Times at which gas-line trend buffers are reset.
     */
    double *resettrendg;
    /**
     * @brief Number of gas-line trend samples currently stored.
     */
    int *ntrendg;
    /**
     * @brief Number of gas-line trend samples stored before the last flush.
     */
    int *ntrendgB;
    /**
     * @brief Maximum number of wall-temperature samples stored for each gas-line trend.
     */
    int *TrendLengthTransG;
    /**
     * @brief Buffered gas-line wall-temperature trend data.
     */
    double ***MatTrendTransG;
    /**
     * @brief Times at which gas-line wall-temperature buffers are reset.
     */
    double *resettrendtransg;
    /**
     * @brief Number of gas-line wall-temperature samples currently stored.
     */
    int *ntrendtransg;
    /**
     * @brief Number of gas-line wall-temperature samples stored before the last flush.
     */
    int *ntrendtransgB;

    /**
     * @brief Production-line cells where radial temperature profiles are written.
     */
    int *ncelperftransp;
    /**
     * @brief Maximum number of samples stored for each production-line trend.
     */
    int *TrendLengthP;
    /**
     * @brief Buffered production-line trend data.
     */
    double ***MatTrendP;
    /**
     * @brief Times at which production-line trend buffers are reset.
     */
    double *resettrend;
    /**
     * @brief Number of production-line trend samples currently stored.
     */
    int *ntrend;
    /**
     * @brief Number of production-line trend samples stored before the last flush.
     */
    int *ntrendB;
    /**
     * @brief Maximum number of wall-temperature samples stored for each production-line trend.
     */
    int *TrendLengthTransP;
    /**
     * @brief Buffered production-line wall-temperature trend data.
     */
    double ***MatTrendTransP;
    /**
     * @brief Times at which production-line wall-temperature buffers are reset.
     */
    double *resettrendtrans;
    /**
     * @brief Number of production-line wall-temperature samples currently stored.
     */
    int *ntrendtrans;
    /**
     * @brief Number of production-line wall-temperature samples stored before the last flush.
     */
    int *ntrendtransB;

    /**
     * @brief Service-line index where column-annulus thermal coupling begins.
     */
    int AnulaColunaIni;
    /**
     * @brief Service-line index where column-annulus thermal coupling ends.
     */
    int AnulaColunaFim;
    /**
     * @brief Production-column index aligned with AnulaColunaIni.
     */
    int ColunaAnulaIni;
    /**
     * @brief Production-column index aligned with AnulaColunaFim.
     */
    int ColunaAnulaFim;
    /**
     * @brief Indicates whether column-annulus thermal coupling is enabled.
     */
    int verificaAcop;
    /**
     * @brief Secondary-line index where parallel-network coupling with the primary line begins.
     */
    int SecPrimIniRedeP;
    /**
     * @brief Secondary-line index where parallel-network coupling with the primary line ends.
     */
    int SecPrimFimRedeP;
    /**
     * @brief Primary-line index aligned with SecPrimIniRedeP.
     */
    int PrimSecIniRedeP;
    /**
     * @brief Primary-line index aligned with SecPrimFimRedeP.
     */
    int PrimSecFimRedeP;
    /**
     * @brief Indicates thermal coupling on the primary branch of a parallel network.
     */
    int verificaAcopRedeP;
    /**
     * @brief Indicates thermal coupling on the secondary branch of a parallel network.
     */
    int verificaAcopRedeS;

    /**
     * @brief Current production-profile output index.
     */
    int kontaTempoProf;
    /**
     * @brief Current gas-line profile output index.
     */
    int kontaTempoProfG;
    /**
     * @brief Current production-wall-temperature profile output index.
     */
    int kontaTempoTransProf;
    /**
     * @brief Current gas-line wall-temperature profile output index.
     */
    int kontaTempoTransProfG;
    /**
     * @brief In-memory stream used to assemble event-log output.
     */
    ostringstream saidaLog;
    /**
     * @brief Event-log file name.
     */
    string tmpLog;
    /**
     * @brief Number of events written to the event log.
     */
    int contaLog;

    /**
     * @brief Smallest production-line control-volume length.
     */
    double menorDx;
    /**
     * @brief Number of iterations used to bracket the initial steady-state root.
     */
    int iterperm;
    /**
     * @brief Simulation time-step counter.
     */
    int kSP;
    /**
     * @brief Controls event-log output frequency.
     */
    int KontaImprime;
    /**
     * @brief Index of the next scheduled simulation event.
     */
    int indevento;
    /**
     * @brief Reserved steady-state mode flag; currently unused.
     */
    int modoPerm;
    /**
     * @brief Current complete-model activation state.
     */
    int modeloCompleto;
    /**
     * @brief Previous complete-model activation state.
     */
    int modeloCompleto0;
    /**
     * @brief Counter used when switching between model formulations.
     */
    int kontaMudaModelo;

    /**
     * @brief History of recently accepted time steps.
     */
    vector<double> dtSim;
    /**
     * @brief History of time steps proposed by the CFL criterion.
     */
    vector<double> dtCFL;
    /**
     * @brief Average time step proposed by the CFL criterion.
     */
    double dtCFLMed;
    /**
     * @brief Average time step actually used by the simulation.
     */
    double dtSimMed;
    /**
     * @brief Indicates that time-step growth must remain restricted.
     */
    int restriDt;
    /**
     * @brief Number of remaining steps under the current time-step restriction.
     */
    int kontarestriDt;
    /**
     * @brief Counter for segregation-related time-step restrictions.
     */
    int kontarestriSegrega;
    /**
     * @brief Accumulated CFL time steps used to compute dtCFLMed.
     */
    double dtCFLTotal;
    /**
     * @brief Accumulated accepted time steps used to compute dtSimMed.
     */
    double dtSimTotal;
    /**
     * @brief Auxiliary CFL time-step accumulator.
     */
    double dtauxCFL;
    /**
     * @brief Auxiliary accepted-time-step accumulator.
     */
    double dtauxFinal;
    /**
     * @brief Counts alternating liquid-flow oscillations near an active surface choke.
     */
    int kontaGolfada;

    /**
     * @brief Surface-choke model.
     */
    choke chokeSup;
    /**
     * @brief Gas-injection choke model.
     */
    ChokeGas chokeInj;
    /**
     * @brief Gas-lift valves installed in the system.
     */
    ChokeGas *chokeVGL;
    /**
     * @brief Gas service-line control volumes.
     */
    CelG *celulaG;
    /**
     * @brief Multiphase production-line control volumes.
     */
    Cel *celula;

    /**
     * @brief Bubble-pressure values imported from PVTSim for the Livia solution-gas-ratio correlation. This table
     * is separate from the full PVTSim fluid-property model.
     */
    double *PBPVTSim;
    /**
     * @brief Bubble-temperature values imported from PVTSim for the Livia solution-gas-ratio correlation. This
     * table is separate from the full PVTSim fluid-property model.
     */
    double *TBPVTSim;
    /**
     * @brief Precomputed solution-gas-ratio table for the Livia correlation, used to avoid repeating its
     * expensive calculation during the simulation.
     */
    double **RSLivia;
    /**
     * @brief Enables reading bubble-pressure and bubble-temperature tables in black-oil mode.
     */
    int LerPB;
    /**
     * @brief Enables reading a solution-gas-ratio table in black-oil mode.
     */
    int lerRS;

    /**
     * @brief Current service-line cell containing the completion-fluid/gas interface.
     */
    int celInter;
    /**
     * @brief Maximum time step that keeps the unloading interface within one gas-line cell.
     */
    double dtInter;
    /**
     * @brief Current completion-fluid/gas interface velocity.
     */
    double velInter;
    /**
     * @brief Previous-time-level interface cell.
     */
    int celInterIni;
    /**
     * @brief Previous-time-level interface-limited time step.
     */
    double dtInterIni;
    /**
     * @brief Previous-time-level interface velocity.
     */
    double velInterIni;

    /**
     * @brief Time horizon used by the gas-lift unloading controller.
     */
    double tempMedContDesc;
    /**
     * @brief Maximum number of flow samples retained by the unloading PI controller.
     */
    double maxVecContDesc;
    /**
     * @brief Average maximum completion-fluid mass flow through the gas-lift valves.
     */
    double vazmedDesc;
    /**
     * @brief Averaging interval used for gas-lift-valve mass flow.
     */
    double tempmedDEsc;
    /**
     * @brief Maximum valve mass-flow samples used by the unloading controller.
     */
    vector<double> vazmaxMedDesc;
    /**
     * @brief Time-step samples associated with the unloading-flow average.
     */
    vector<double> dtDesc;

    /**
     * @brief Indicates that the section outlet is not connected to another network section.
     */
    int noextremo;
    /**
     * @brief Indicates that the section inlet is not connected to another network section.
     */
    int noinicial;
    /**
     * @brief Indicates that this section is a branch of a gas-lift ring network.
     */
    int derivaAnel;

    /**
     * @brief Intermediate production-liquid inflow estimate for a network section.
     */
    double fontemassPRBuf;
    /**
     * @brief Intermediate complementary-liquid inflow estimate for a network section.
     */
    double fontemassCRBuf;
    /**
     * @brief Intermediate gas inflow estimate for a network section.
     */
    double fontemassGRBuf;

    /**
     * @brief Temporary-network flag; currently expected to remain zero.
     */
    int redeTemporario;

    /**
     * @brief Trend-output cycle counter; a value of one triggers header output.
     */
    double kimpT;

    /**
     * @brief Multiplier applied to a small artificial upstream gas flow during difficult Master1 closures. It
     * mitigates a pressure blind spot when the upstream side contains only liquid and increases if the
     * problem persists.
     */
    double momentoDesesp;

    /**
     * @brief Recent maximum pressure-change rates.
     */
    vector<double> taxaDpMax;
    /**
     * @brief Average maximum pressure-change rate.
     */
    double DpMaxMed;

    /**
     * @brief Recent maximum temperature-change rates.
     */
    vector<double> taxaDTMax;
    /**
     * @brief Average maximum temperature-change rate.
     */
    double DTMaxMed;
    /**
     * @brief Initial holdup estimate used by the steady-state solver.
     */
    double chuteHol;
    /**
     * @brief Controls the search for an initial steady-state estimate.
     */
    int buscaIni;

    /**
     * @brief Dynamic fluid-property tables.
     */
    vector<tabelaDinamica> tabDin;
    /**
     * @brief Number of dynamic property tables.
     */
    int ntabDin;
    /**
     * @brief Counter controlling compositional-property refreshes.
     */
    int kontaRenovaComp;
    /**
     * @brief Section-blocking state.
     */
    int bloq;

    /**
     * @brief Fluid state received from downstream during reverse network flow.
     */
    ProFlu fluiRevRede;
    /**
     * @brief Temperature associated with reverse network flow.
     */
    double tempRev;
    /**
     * @brief Indicates reverse flow in the steady-state network solution.
     */
    int revPerm;
    /**
     * @brief Index corrections used by coupled thermal sections.
     */
    vector<int> acertaIndAcop;
    /**
     * @brief Shared one-dimensional simulation settings.
     */
    varGlob1D *vg1dSP;
    /**
     * @brief Three-dimensional Poisson solver used by the thermal model.
     */
    solverP3D poisson3D;
    /**
     * @brief Minimum time step allowed by the current cycle.
     */
    double dtCicMin;

    /**
     * @brief Production cells handled by the two-dimensional Poisson model.
     */
    vector<int> indCelPoisson2D;
    /**
     * @brief Number of cells handled by the two-dimensional Poisson model.
     */
    int nCelulaPoisson2D;
    /**
     * @brief Indicates that the thermal source term is disabled.
     */
    int semTermo;
    /**
     * @brief Current steady-state convergence monitor.
     */
    double monitConvPerm;
    /**
     * @brief Reference value for the steady-state convergence monitor.
     */
    double monitConvPermBase;
    /**
     * @brief Signals that the time step must be adjusted.
     */
    int alteraTempo;

    /**
     * @brief Initial source indices for parallel-network coupling.
     */
    vector<int> indFonteRedeParalelaIni;
    /**
     * @brief Initial production-liquid sources for the parallel network.
     */
    vector<double> fonteMpRedeParalelaIni;
    /**
     * @brief Initial complementary-liquid sources for the parallel network.
     */
    vector<double> fonteMcRedeParalelaIni;
    /**
     * @brief Initial gas sources for the parallel network.
     */
    vector<double> fonteMgRedeParalelaIni;
    /**
     * @brief Boundary-condition type applied to the secondary parallel-network branch.
     */
    int redeParalelaCCsecundario;
    /**
     * @brief Primary branch index in a parallel network.
     */
    int redeParalelaP;
    /**
     * @brief Secondary branch index in a parallel network.
     */
    int redeParalelaS;

    vector<int> kontaTempoCelUni;

    const char *saidaTextoSis[15] = {"                          Post Coitum Omine Animal Triste Est                   ",
                                     "           'Ouca-me. O fim quase nunca esta longe, em nenhum momento!'          ",
                                     "      So nos curamos de um sofrimento depois de o haver suportado ate o fim.    ",
                                     "                   Infeliz e o espirito ansioso pelo futuro.                    ",
                                     "                                    Memento Mori                                ",
                                     " Somente um progresso calmo e constante, livre de precipitacao, conduz ao objetivo.",
                                     "             Paciencia, nove mulheres nao conseguem gerar uma crianca em um mes. ",
                                     "                  A necessidade e a mae da inovacao, mas a paciencia e o pai    ",
                                     "O sucesso nao e uma linha reta, e um jogo de resistencia, e cada tropeco e apenas um degrau a mais para a vitoria!",
                                     "                        Quem vive de navegar, o vento e quem lhe comanda                ",
                                     "    Uma vez me perguntaram o que achava da passagem do tempo, e eu disse: sou contra    ",
                                     "                 Nao importa o quanto voce va devagar, desde que nao pare                ",
                                     "Um simulador que resolve uma parada de producao, comeca avancando pequenos incrementos de tempo",
                                     "                            Nada e permanente, exceto a mudanca                           ",
                                     "                  Uma jornada de mil quilometros comeca com um unico passo                "};
    const char *saidaSubTextoSis[15] = {
        "                         Galeno de Pergamo do Transiente Longo                          ",
        "                     J. California Cooper depois da simulacao divergir                  ",
        "                                Marcel Proust no CrossFit                               ",
        "                              Seneca do Mindfulness                                     ",
        "                                   Zuleica da Funeraria                                 ",
        "                                       China In Box                                     ",
        "                                      Tiao do Linkedin                                  ",
        "                                      Marcao da Oficina                                 ",
        "                                    Mario Pascal do Insta                               ",
        "                          Seu Pereira na feira de artesanatos numericos                 ",
        "                        Luis Fernando Verissimo das Simulacoes Permanentes              ",
        "                             Confucio vendo a simulacao emperrar                       ",
        "                               Confucio das simulacoes sem fim                         ",
        "          Heraclito de Efeso vendo tudo mudar a cada incremento de tempo                ",
        "    Lao-Tse tomando coragem para simular um caso de parafinacao em dutos de producao    "};

    /// Constructs and initializes a production-system simulation from the input and log files.
    SProd(string nomeArquivoEntrada, string nomeArquivoLog, tipoValidacaoJson_t validacaoJson,
          tipoSimulacao_t tipoSimulacao, varGlob1D *Vvg1dSP = 0, int TD = -1, int vbloq = 0,
          int temporario = 0,
          int reverso = 0,
          double *compfonte = 0,
          int *posicfonte = 0,
          int nfontes = 0,
          int redeperm = 1);
    /// Creates an empty production-system object.
    SProd();

    /// Releases dynamically allocated simulation buffers and cell arrays.
    ~SProd() {
        if (arq.lingas > 0)
            delete[] celulaG;
        if (chokeVGL!=0 && arq.lingas > 0)
            delete[] chokeVGL;
        if (posicVGLP!=0 && arq.lingas > 0)
            delete[] posicVGLP;
        if (posicVGLG!=0 && arq.lingas > 0)
            delete[] posicVGLG;
        if (nabreM1 > 0)
            delete[] abreM1;
        if (nfechaM1 > 0)
            delete[] fechaM1;

        if (arq.nperfistransp > 0)
            delete[] ncelperftransp;
        if (arq.nperfistransg > 0 && arq.lingas > 0)
            delete[] ncelperftransg;

        if (arq.ntendp > 0 && redeTemporario == 0) {
            for (int i = 0; i < arq.ntendp && MatTrendP && TrendLengthP; i++) {
                if (MatTrendP[i]) {
                    for (int j = 0; j < TrendLengthP[i]; j++)
                        delete[] MatTrendP[i][j];
                    delete[] MatTrendP[i];
                }
            }
            if (MatTrendP!=0)
                delete[] MatTrendP;
            if (TrendLengthP!=0)
                delete[] TrendLengthP;
            if (resettrend!=0)
                delete[] resettrend;
            if (ntrend!=0)
                delete[] ntrend;
            if (ntrendB!=0)
                delete[] ntrendB;
        }

        if (arq.ntendg > 0 && arq.lingas > 0 && redeTemporario == 0) {
            for (int i = 0; i < arq.ntendg && MatTrendG && TrendLengthG; i++) {
                if (MatTrendG[i]) {
                    for (int j = 0; j < TrendLengthG[i]; j++)
                        delete[] MatTrendG[i][j];
                    delete[] MatTrendG[i];
                }
            }
            if (MatTrendG!=0)
                delete[] MatTrendG;
            if (TrendLengthG!=0)
                delete[] TrendLengthG;
            if (resettrendg!=0)
                delete[] resettrendg;
            if (ntrendg!=0)
                delete[] ntrendg;
            if (ntrendgB!=0)
                delete[] ntrendgB;
        }

        if (arq.ntendtransp > 0 && redeTemporario == 0) {
            for (int i = 0; i < arq.ntendtransp && MatTrendTransP && TrendLengthTransP; i++) {
                if (MatTrendTransP[i]) {
                    for (int j = 0; j < TrendLengthTransP[i]; j++)
                        delete[] MatTrendTransP[i][j];
                    delete[] MatTrendTransP[i];
                }
            }
            if (MatTrendTransP!=0)
                delete[] MatTrendTransP;
            if (TrendLengthTransP!=0)
                delete[] TrendLengthTransP;
            if (resettrendtrans!=0)
                delete[] resettrendtrans;
            if (ntrendtrans!=0)
                delete[] ntrendtrans;
            if (ntrendtransB!=0)
                delete[] ntrendtransB;
        }

        if (arq.ntendtransg > 0 && redeTemporario == 0) {
            for (int i = 0; i < arq.ntendtransg && MatTrendTransG && TrendLengthTransG; i++) {
                if (MatTrendTransG[i]) {
                    for (int j = 0; j < TrendLengthTransG[i]; j++)
                        delete[] MatTrendTransG[i][j];
                    delete[] MatTrendTransG[i];
                }
            }
            if (MatTrendTransG!=0)
                delete[] MatTrendTransG;
            if (TrendLengthTransG!=0)
                delete[] TrendLengthTransG;
            if (resettrendtransg!=0)
                delete[] resettrendtransg;
            if (ntrendtransg!=0)
                delete[] ntrendtransg;
            if (ntrendtransgB!=0)
                delete[] ntrendtransgB;
        }

        int ndiv = arq.tabent.npont - 1;
        if (CalcLat > 0 && arq.flashCompleto == 0) {
            for (int i = 0; i < ndiv + 2; i++)
                delete[] HLat[i];
            delete[] HLat;
        }

        if (LerPB > 0) {
            delete[] PBPVTSim;
            delete[] TBPVTSim;
            if (lerRS == 1) {
                for (int i = 0; i < ndiv + 2; i++)
                    delete[] RSLivia[i];
                delete[] RSLivia;
            }
        }

        if (ncel > 0)
            delete[] celula;
        if (npig > 0)
            delete[] receb;
        if (arq.tabelaDinamica == 1)
            tabDin.clear();
    }

    /// Performs a deep copy of the production-system state.
    SProd &operator=(const SProd &);

    /// Copies an already parsed system configuration without reading the JSON file again.
    void copiaSemJson(Ler &, int vnoextremo, int vnoinicial, int vderivaAnel, int vbloq,
                      double vbetaRev, double vbetaRevini, double vtitRev, double vtitRevini,
                      double vdtCicMin);

    /// Builds the production section after input parsing.
    void montasistema(double *compfonte = 0,
                      int *posicfonte = 0,
                      int nfontes = 0);

    /// Initializes the gas service line for gas-lift unloading.
    void HidroDescargaG();
    /// Initializes the production line for gas-lift unloading.
    void HidroDescargaP();

    /// Estimates the gas-injection pressure correction required to avoid erosional valve velocity.
    double prescordesc(double velmax, int ivalv, double fator, int sinal);
    /// Computes the unloading injection-pressure correction for one gas-lift valve.
    double CalcPresValvDesc(double velGarg, int ivalv);
    /// Controls injection and upstream-choke pressures from gas-lift-valve flow rates.
    double BuscaPresInjDesc();

    /// Updates gas-line state after solving pressure-velocity coupling.
    void renovaGas();
    /// Updates intermediate gas-line state during network convergence.
    void renovaGasBuf();

    /// Calculates gas-lift-valve opening area from calibration and operating conditions.
    double areaValvCali(double PCal, double TCal, double PVO, double PT,
                        double dextern, double areagarg, double Rvalv, double Temp);
    /// Advances the temperature of one gas-line control volume.
    void calctempGas(int i, double tempantiga, int modoPerm = 0);
    /// Solves gas-line pressure and flow in the completion-fluid region during unloading.
    void resolveDescarga();
    /// Updates gas-line temperature in the completion-fluid region during unloading.
    void tempDescarga(int i);
    /// Advances the completion-fluid/gas interface in the service line.
    void avancInter();
    /// Calculates gas temperature across a gas-lift valve using the Joule-Thomson model.
    double TempDescGL(int igl);
    /// Maps gas-lift-valve positions to gas-line control volumes.
    void ValvGasTrans();
    /// Advances the coupled gas-line pressure, velocity, and temperature solution.
    void subtempoGas();
    /// Advances the intermediate gas-line state used by network convergence.
    void subtempoGasBuf();

    /// Exchanges heat-transfer data between the production column and annulus.
    void conectaColuna();
    /// Interpolates latent heat from enthalpy tables.
    double interpolaHLatente(double pres, double temp);
    /// Advances the temperature of one production-line control volume.
    void calctemp(int i, double tempantiga, int modoPerm = 0);

    /// Returns the mixture enthalpy helper value; currently unused.
    double calcHmix(int i);
    /// Returns the mixture-energy helper value; currently unused.
    double energmix(int i, int jp0, int jt, double razp);
    /// Updates temperature from enthalpy; currently unused.
    void calcTempEntalp(int i);
    /// Evaluates thermal mass-transfer terms; currently unused.
    void calcTransMassTermo(int i);

    /// Calculates flow through Master1 while it operates as a choke.
    void FonteValv(int ind);
    /// Stores source terms from the previous time level for possible rollback.
    void salvaFonte();
    /// Updates IPR, gas, liquid, leak, and gas-lift source terms.
    void renovaFonte(int ind);
    /// Stores previous void fractions and updates pig motion and reception.
    void renovaalbetini();
    /// Caches cell and face densities to avoid repeated property calculations.
    void renovaMasEsp();

    /// Evaluates C0 and Ud using the Bhagwat-Ghajar drift-flux correlation.
    void BhagwatGhajar(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                       double ug1, double ul1, double dia, double rug, double tet, double &c0,
                       double &ud, double correcHor = 1.);
    /// Evaluates C0 and Ud using the Bhagwat-Ghajar drift-flux correlation.
    void BhagwatGhajarMod(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                          double ug1, double ul1, double dia, double rug, double tet, double &c0,
                          double &ud, double correcHor = 1.);
    /// Evaluates C0 and Ud using the Choi drift-flux correlation.
    void Choi(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
              double ug1, double ul1, double dia, double rug, double tet, double &c0,
              double &ud, double correcHor = 1.);
    /// Evaluates C0 and Ud using the Hibiki-Ishii drift-flux correlation.
    void HibikiIshii(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                     double ug1, double ul1, double dia, double rug, double tet, double &c0,
                     double &ud, double correcHor = 1.);
    /// Evaluates C0 and Ud using the Franca-Lahey drift-flux correlation.
    void FrancaLahey(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                     double ug1, double ul1, double dia, double rug, double tet, double &c0,
                     double &ud, double correcHor = 1.);
    /// Evaluates C0 and Ud for dispersed flow.
    void C0UdDisperso(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                      double ug1, double ul1, double dia, double rug, double tet, double &c0,
                      double &ud, double correcHor = 1., int estabCol = 0);
    /// Evaluates C0 and Ud for annular or churn flow.
    void C0UdAnularChurn(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                         double ug1, double ul1, double dia, double rug, double tet, double &c0,
                         double &ud, double correcHor = 1., int estabCol = 0);
    /// Evaluates C0 and Ud for stratified flow.
    void C0UdEstratificado(double rhol, double rhog, double tensup, double alf, double reymix, double reymixL,
                           double ug1, double ul1, double dia, double rug, double tet, double &c0,
                           double &ud, double correcHor = 1., int estabCol = 0);

    /// Selects and evaluates the slip correlation at a production-line face.
    void CalcC0Ud(int ind, double &c0, double &ud);
    /// Evaluates slip parameters for the intermediate network state.
    void CalcC0UdBuf(int ind, double &c0, double &ud);
    /// Evaluates slip parameters at the inlet of an internal network section.
    void CalcC0UdIni(int ind, double &c0, double &ud);
    /// Evaluates slip parameters at the inlet of an internal network section.
    void CalcC0UdIniBuf(int ind, double &c0, double &ud);

    /// Applies hydrostatic and friction corrections; currently unused.
    void correcHidroFric(int i, double &hidro, double &fric);
    /// Prepares auxiliary data for a local fluid-property table.
    void auxMiniTab(ProFlu &flu);
    /// Generates the local fluid-property table.
    void geraMiniTabFlu();

    /// Loads pressure-velocity results into cell and face state variables.
    void renova(int expli = 0);
    /// Updates phase and mixture flow rates.
    void renovaVaz();
    /// Updates only section-end states during intermediate network convergence.
    void renovaBuffer();
    /// Copies previous states when an active choke bypasses the intermediate network solve.
    void renovaBufferCego();

    /// Returns the sign of a value; currently unused.
    int sign(double var);

    /// Updates distributed mass-transfer terms used by void-fraction and mixture-mass equations.
    void renovaTemp();
    /// Evaluates wax deposition and its effects.
    void avaliaParafina();
    /// Transports black-oil properties such as GOR, API, gas density, and CO2 fraction.
    void renovaRGOdgYco2(ProFlu fluiRev = ProFlu());
    /// Transports compositional molar fractions.
    void renovaFracMol(ProFlu fluiRev = ProFlu());
    /// Applies the alternate compositional molar-fraction transport update.
    void renovaFracMol2(ProFlu fluiRev = ProFlu());
    /// Computes T1 and T2 used to split mixture mass flow into liquid and gas flows.
    void renovaterm(int aflu = 0);
    /// Computes T1 and T2 at the outlet of an internal network section.
    void renovatermAfluFim();
    /// Computes T1 and T2 at the inlet of an internal network section.
    void renovatermColIni();

    /// Calculates outlet-choke flow from the last-cell and separator pressures.
    void calcCCpres(double titRev = 1., double alfRev = 1., double betRev = 0.);
    /// Calculates outlet-choke flow for the intermediate network state.
    void calcCCBuffer(double titRev = 1., double alfRev = 1., double betRev = 0.);

    /// Selects a stable time step from CFL and additional model restrictions.
    void determinaDT(int vexpli = 0);
    /// Selects a stable time step from CFL and additional model restrictions.
    void determinaDTExpli();
    /// Limits time-step growth when accepted steps remain well below the CFL estimate.
    void atenuaDtMax();
    /// Checks whether liquid-density variation requires the complete formulation.
    void avaliaVariaDpDt(double razMast = 0, double razMast0 = 0, int vexpli = 0);
    /// Updates the valve logic for the active-state formulation.
    void aberturaVal1();
    /// Updates the valve logic for the inactive-state formulation.
    void aberturaVal0();
    /// Restricts the time step during valve-state transitions.
    void restringeDTporValv();

    /// Advances the gas-injection system and prepares its coupling with the production line.
    void solveLinGas();
    /// Advances phase volume fractions.
    void EvoluiFrac(double alfrev = 1., double betrev = 0., int ciclo = 0);
    /// Restores the initial fraction state after an invalid update.
    void ReiniEvolFrac0();
    /// Stores only the volume fractions required by fraction rollback.
    void SubReiniEvolFrac();
    /// Restores previous volume fractions after a nonphysical update.
    void ReiniEvolFrac();
    /// Updates phase fractions in cells affected by a moving pig.
    void AtualizaPig();
    /// Assembles and solves the global pressure-velocity coupling system.
    void SolveAcopPV(int vexpli = 0, int ciclo = 0);

    /// Prepares the multidimensional heat-diffusion problem for one cell.
    void prepDifusCalorND(int i);
    /// Advances the transient energy equation.
    void marchaEnergTrans(int ciclo = 0, int ciclomax = 0);
    /// Refreshes local dynamic fluid-property tables.
    void atualizaMiniTab();
    /// Updates the first boundary condition.
    void atualizaCC1();

    /// Advances the complete transient production-system solution and handles output and logging.
    void SolveTrans(double titRev = 1., double alfRev = 1., double betRev = 0.,
                    int nrede = -1, ProFlu fluiRev = ProFlu());

    /// Appends one production-line trend sample to its output buffer.
    void ImprimeTrendP(int i, int nrede = -1);
    /// Appends one production-line trend sample to its output buffer.
    void ImprimeTrendPCab(int i, int nrede = -1);
    /// Appends one gas-line trend sample to its output buffer.
    void ImprimeTrendG(int i, int nrede = -1);
    /// Appends one gas-line trend sample to its output buffer.
    void ImprimeTrendGCab(int i, int nrede = -1);
    /// Appends one production-line wall-temperature trend sample.
    void ImprimeTrendTransP(int i);
    /// Appends one production-line wall-temperature trend sample.
    void ImprimeTrendTransPCab(int i);
    /// Appends one gas-line wall-temperature trend sample.
    void ImprimeTrendTransG(int i);
    /// Appends one gas-line wall-temperature trend sample.
    void ImprimeTrendTransGCab(int i);

    /// Marches the steady production solution using a bottomhole-pressure guess with outlet pressure prescribed.
    double marchaProdPerm1(double pchute);
    /// Marches the steady production solution using a bottomhole-pressure guess with outlet pressure prescribed.
    double marchaProdPerm1Rev(double pchute);
    /// Marches the steady production solution using a bottomhole-pressure guess with an outlet choke.
    double marchaProdPerm2(double pchute);
    /// Brackets and solves the bottomhole-pressure root for marchaProdPerm1.
    double buscaProdPfundoPerm(double chute = -1., int kontaTenta = -1);
    /// Brackets and solves the reverse-flow bottomhole-pressure root.
    double buscaProdPfundoPermRev(double chute = -1.);
    /// Brackets and solves the bottomhole-pressure root for marchaProdPerm2.
    double buscaProdPfundoPerm2(double chute = -1., int kontaTenta = -1);
    /// Runs a direct march when inlet pressure and flow rate are known.
    double buscaProdPfundoPerm3(double pentrada);
    /// Marches the steady production solution using a bottomhole mass-flow guess.
    double marchaProdPresPres1(double mchute);
    /// Marches the steady production solution using a bottomhole mass-flow guess.
    double marchaProdPresPres1Rev(double mchute);
    /// Brackets and solves the mass-flow root for marchaProdPresPres1.
    double buscaProdPresPresPerm(double mchute, double maxvaz = 0., int kontaiter = 0);
    /// Brackets and solves the reverse-flow mass-flow root.
    double buscaProdPresPresPermRev(double mchute, double maxvaz = 0., int kontaiter = 0);
    /// Marches the steady production solution for the second pressure-pressure boundary formulation.
    double marchaProdPresPres2(double mchute);
    /// Brackets and solves the mass-flow root for marchaProdPresPres2.
    double buscaProdPresPresPerm2(double mchute, double maxvaz = 0.);
    /// Marches the steady production solution for the third pressure-pressure boundary formulation.
    double marchaProdPresPres3(double mchute);
    /// Brackets and solves the mass-flow root for marchaProdPresPres3.
    double buscaProdPresPresPerm3(double mchute, double maxvaz = 0.);

    /// Marches the steady gas line with prescribed injection pressure and a mass-flow guess.
    double marchaGasPerm1(double chutemass = -1);
    /// Marches the steady gas line with prescribed injection flow and a pressure guess.
    double marchaGasPerm2(double pchute, double chutemass = -1);
    /// Marches the steady gas line across the injection choke using a downstream-pressure guess.
    double marchaGasPerm3(double pchute);
    /// Brackets and solves the pressure root for marchaGasPerm2.
    double buscaGasPresPerm2();
    /// Brackets and solves the pressure root for marchaGasPerm3.
    double buscaGasPresPerm3();

    /// Marches pressure from the previous cell center to the downstream face.
    void RenovaPresPermMon(int i, int RK);
    /// Marches pressure from the last cell center to the outlet face.
    double RenovaPresPermNcel();
    /// Calculates the pressure contribution caused by an area change.
    double calcDpArea(int i, double rhomix, double rey, double jmix);
    /// Marches pressure from the upstream face to the current cell center.
    void RenovaPresPermJus(int i, int RK);
    /// Corrects gas density in one control volume.
    void corrDeng(int i);
    /// Updates steady-state mass flow and fluid state after a source term.
    void RenovaMassPerm(int i);
    /// Updates steady-state mass flow for reverse flow.
    void RenovaMassPermRev(int i);
    /// Updates steady-state mass flow and pseudocomponent composition after a source term.
    void RenovaMassPermComp(int i);
    /// Updates steady-state mass flow and pseudocomponent composition after a source term.
    void RenovaMassPermCompRev(int i);
    /// Calculates steady-state slip parameters at a downstream face.
    void CalcC0UdPerm(int ind, double &c0, double &ud);
    /// Calculates steady-state interphase mass transfer.
    void RenovaTransMassPerm(int i);
    /// Calculates steady-state interphase mass transfer.
    void RenovaTransMassPermGas(int i);
    /// Marches steady-state temperature from cell i-1 to cell i.
    void RenovaTempPerm(int i, int RK);
    /// Marches steady-state temperature in the reverse direction.
    void RenovaTempPermRev(int i, int RK);

    /// Adds pump pressure gain at the upstream face of a production cell.
    void atualizaPeriPmonProd(int i);
    /// Synchronizes neighboring face pressures after updating a cell-center pressure.
    void atualizaPeriPjusProd(int i);
    /// Synchronizes neighboring face temperatures after updating a cell-center temperature.
    void atualizaPeriTempProd(int i);

    /// Marches steady-state pressure along the gas service line.
    void RenovaPresGasPerm(int i);
    /// Estimates gas-line pressure variation without advancing the full pressure march.
    double delpGasPerm(int i);
    /// Estimates pressure variation in an injection-well system.
    double delpInjPerm(int i);
    /// Calculates gas-lift-valve flow from gas-line and production-line pressures.
    void calcVazGasPerm(int i);
    /// Initializes gas-lift-valve flow estimates before the steady-state march.
    void IniciaVazValvGasPerm(int i);
    /// Marches steady-state gas-line temperature from cell i-1 to cell i.
    void RenovaTempGasPerm(int i);

    /// Exchanges steady-state heat-transfer data between column and annulus.
    void conectaColunaPerm();
    /// Initializes estimated column-annulus heat transfer before the steady-state march.
    void IniciaconectaColunaPerm();

    /// Refreshes fluid properties.
    void atualizaProp();
    /// Updates steady-state velocities and thermal terms.
    void atualizaVelTermPerm();
    /// Calculates the pseudo-transient time step.
    void calcDTPseudoTrans();

    /// Marches the steady injection-well solution using a pressure or flow-rate guess.
    double marchaInjPerm1(double chute);
    /// Solves injection cases CC1 and CC3.
    double buscaInjPfundoPerm1(double chute = -1.);
    /// Solves injection case CC0.
    double buscaInjPfundoPerm2(double chute = -1.);
    /// Solves injection case CC2.
    double buscaInjPfundoPerm3(double chute = -1.);
    /// Solves injection case CC4.
    double buscaInjPfundoPerm4();
    /// Solves injection case CC5.
    double buscaInjPfundoPerm5(double chute = -1.);

    /// Dispatches the selected production, gas-line, or injection steady-state marching method.
    double multMarcha(double chute, int prod, int tipoCC);

    /// Returns the magnitude of the first argument with the sign of the second.
    double SIGN(double a, double b);
    /// Finds a root using Brent's method; currently unused.
    double zbrent(double, double, int prod, int tipoCC,
                  double tol = 0.00001, double epsn = 0.00001, int maxit = 100);
    /// Finds a root using the false-position method; currently unused.
    double falsacorda(double a, double b, int prod, int tipoCC);
    /// Finds a root using Ridders' method.
    double zriddr(double x1, double x2, int prod, int tipoCC);

    /// Estimates steady production-network node pressures from hydrostatics.
    double hidroreverso(double hol, double vaz = 0, double vazG = 0);
    /// Estimates steady injection-network node pressures from hydrostatics.
    double hidroreversoInj(double hol, double vaz = 0);
    /// Estimates secondary-branch pressures in a parallel production network.
    double hidroTramoSecundario(double titulo);
    /// Builds a hydrostatic estimate for the gas service line.
    void hidroLinServ();

    /// Solves the secondary-branch flow rate in a steady parallel network.
    double buscaTramoSecVazPerm(double pPartida, int indPartida);
    /// Marches the steady secondary branch of a parallel network.
    double marchaTramoSecVaz(double pchute, double chutemass = -1);

    /// Calculates temperature downstream from the surface choke for network coupling.
    void calcTempFim();
};

#endif /* SISPROD_H_ */
