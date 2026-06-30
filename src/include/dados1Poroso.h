/*
 * dados1Poroso.h
 *
 * Created on: August 31, 2023
 * Author: Eduardo
 */

#ifndef DADOS1POROSO_H_
#define DADOS1POROSO_H_

#include "Elem2DPoroso.h"
#include "GeometriaPoro.h"
#include "Log.h"
#include "Malha2DPoroso.h"
#include "MarlimComposicional.h"
#include "Matriz.h"
#include "PorosoRad.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Vetor.h"
#include "celRad.h"
#include "estruturaTabDin.h"
#include "estruturasPoroso.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

// Simulation output-profile report stream.
extern ofstream arqRelatorioPerfis;

// Application logger.
extern Logger logger;

// Path to the main input file.
extern string pathArqEntrada;

// Path to the external PVTSIM and snapshot input files.
extern string pathArqExtEntrada;

// Path and filename prefix for POCO_INJETOR output files.
extern string pathPrefixoArqSaida;

// Directory containing the simulation output files.
extern string diretorioSaida;

/*!
 * Store and manage the input data for the two-dimensional porous-medium model.
 *
 * The class contains the simulation-time configuration, porous-material
 * properties, initial and boundary conditions, mesh data, well conditions,
 * thermal and hydraulic coupling parameters, fluid-property tables, relative
 * permeability and capillary-pressure tables, and linear-system solver settings.
 *
 * It also owns the dynamically allocated arrays created while reading the
 * porous-medium model and releases them in the destructor.
 */
class dadosPoro {
  public:
    detTempoPoroso temp; // Time-discretization and simulation-time data.
    detPropPoroso prop;  // Porous-medium and physical-property data.
    detCIPoroso CI;      // Initial-condition data.
    detDiriPoroso pW;    // Prescribed well-pressure or Dirichlet-condition data.
    detCCPoroso CC;      // Boundary-condition data.
    materialPoroso *mat; // Array containing the porous materials registered in the JSON input.
    cortePoroso corte;   // Cross-section data registered in the JSON input.
    DadosGeoPoro dutosMRT; // MRT geometry data used by the porous-medium model.
    PorosRad transfer;     // Radial porous-medium transfer model.

    int nno;          // Number of mesh nodes.
    int nele;         // Number of mesh elements.
    int noZero;       // Number of nonzero entries used by the sparse system.
    int **noEle;      // Element-to-node connectivity matrix.
    double *atributo; // Mesh or region attribute array.
    int *tipo;        // Node or boundary classification array.
    double *vecsra;   // Auxiliary sparse-matrix value vector.
    int *veclm;       // Auxiliary sparse-matrix index vector.
    int *vecfnz;      // Auxiliary first-nonzero-entry index vector.
    double **xcoor;   // Node-coordinate matrix.

    double pInt;      // Internal or well pressure.
    double pRes;      // Reservoir pressure.
    double zDatumRef; // Reference datum elevation.

    double condGlob; // Global transmissibility or conductivity parameter.
    double condLoc;  // Local transmissibility or conductivity parameter.
    double qTotal;   // Total flow rate.
    double pInt0 = pInt; // Initial or reference internal pressure.

    double tRes;         // Reservoir temperature.
    double espessuraRes; // Reservoir thickness.

    double tempo;    // Current simulation time.
    int nmaterial;   // Number of porous materials.
    int acop;        // Coupling activation flag.
    double angAcop;  // Angular position used by the coupling model.
    double pParede;  // Wall pressure.

    double hE; // External transfer coefficient.
    double hI; // Internal transfer coefficient.

    double qDesacop; // Flow rate calculated by the uncoupled model.
    double qAcop;    // Flow rate calculated by the coupled model.
    double qTotalW;  // Total water flow rate.
    double qTotalO;  // Total oil flow rate.
    double qTotalG;  // Total gas flow rate.

    double centroX; // X-coordinate of the model center.
    double centroY; // Y-coordinate of the model center.
    int nxMH;       // Number of cells or points in the x-direction of the auxiliary grid.
    int nyMH;       // Number of cells or points in the y-direction of the auxiliary grid.
    double xmin;    // Minimum x-coordinate of the computational domain.
    double xmax;    // Maximum x-coordinate of the computational domain.
    double ymin;    // Minimum y-coordinate of the computational domain.
    double ymax;    // Maximum y-coordinate of the computational domain.
    double dxH;     // Auxiliary-grid spacing in the x-direction.
    double dyH;     // Auxiliary-grid spacing in the y-direction.

    int unv;        // Mesh-input format or unit configuration flag.
    string elearq;  // Element-file path or name.
    string noarq;   // Node-file path or name.
    string polyarq; // Polygon-file path or name.
    string entrada; // Main input-file path or name.

    ProFluCol fluc; // Complementary-fluid properties.
    int flashCompleto; // Select the fluid-property model: 1 -> PVTSIM table; otherwise, Black Oil.
    int tabelaDinamica; // Dynamic-table activation flag.
    int usaTabela;      // Indicates that a property table is being used for either Black Oil or PVTSIM.
    string pvtsimarq;   // PVTSIM input-file path or name.
    int tabp;           // For Black Oil, indicates that compressibility-factor and derivative tables
                        // must be generated before starting the production-line simulation.
    ProFlu flup;             // Production-fluid properties.
    tabelaFlashPoroso flash; // Fluid-property tables loaded from a PVTSIM file.
    int npontos = 0;         // Number of points in the Black Oil or PVTSIM property tables.
    double **zdranP;         // Black-Oil compressibility-factor table indexed by reduced pressure and temperature.
    double **dzdpP;          // Pressure derivative of the Black-Oil compressibility-factor table.
    double **dzdtP;          // Temperature derivative of the Black-Oil compressibility-factor table.
    int nvecEmul;            // Number of user-provided BSW/viscosity-multiplier pairs in the emulsion model.
    double *BSWVec;          // User-provided BSW values for the emulsion model.
    double *emulVec;         // User-provided viscosity multipliers for the emulsion model.
    int miniTabAtraso;       // Delay or history setting used by the reduced property table.
    double miniTabDp;        // Pressure increment used by the reduced property table.
    double miniTabDt;        // Temperature increment used by the reduced property table.
    detTabViscPoroso tabVisc; // Porous-medium viscosity-table data.
    detTabelaEntalpPoroso tabent; // Pressure and temperature limits of the PVTSIM table.
    detTabelaEntalpPoroso tabentCrit; // Reduced-pressure and reduced-temperature limits used for
                                      // the gas compressibility factor and its derivatives.
    composicionalPoroso compDet; // Compositional-model configuration.
    int corrDeng = 0;            // Gas-density correction flag.

    tabelaPemRelOA kRelOA; // Oil-water relative-permeability table.
    tabelaPemRelOG kRelOG; // Oil-gas relative-permeability table.
    tabelaPresCapOA pcOA;  // Oil-water capillary-pressure table.
    tabelaPresCapGO pcGO;  // Gas-oil capillary-pressure table.

    double satAconat;       // Connate-water saturation.
    double ippoco;          // Well productivity or injectivity index.
    double *satAPoc;        // Time-dependent water-saturation values at the well.
    double *satLRes;        // Time-dependent liquid-saturation values in the reservoir.
    double *tempoSatAPoc;   // Time values associated with satAPoc.
    double *tempoSatLRes;   // Time values associated with satLRes.
    int nsatAPoc = 0;       // Number of points in the well water-saturation series.
    int nsatLRes = 0;       // Number of points in the reservoir liquid-saturation series.

    int reinicia = 0; // Restart or time-step reset flag.

    int tipoModelOleo; // Oil-model selection.
    double compOleo;   // Oil compressibility.
    double rhoRef;     // Reference density.
    double pBolha;     // Bubble-point pressure.
    double BSW;        // Basic sediment and water fraction.
    varGlob1D *vg1dSP; // Pointer to shared one-dimensional global variables.
    int nthrdLocal;    // Number of threads used by the local computation.

    int rankLU;    // Rank or configuration value used by the LU solver.
    int colore;    // Coloring configuration used by the solver.
    int solverMat; // Matrix-solver selection flag.

    /*!
     * Construct the porous-medium data from the shared global variables and
     * the specified input file.
     */
    dadosPoro(varGlob1D *Vvg1dSP, string nomeArquivoEntrada);

    //! Construct an empty porous-medium data object.
    dadosPoro();

    //! Copy constructor.
    dadosPoro(const dadosPoro &);

    //! Copy-assignment operator.
    dadosPoro &operator=(const dadosPoro &);

    //! Release all dynamically allocated mesh, property-table, and time-series arrays.
    ~dadosPoro() {
        if (nele > 0) {
            for (int i = 0; i < nele; i++)
                delete[] noEle[i];
            delete[] noEle;
        }
        if (nno > 0) {
            for (int i = 0; i < nno; i++)
                delete[] xcoor[i];
            delete[] xcoor;
            delete[] atributo;
            delete[] tipo;
        }
        if (noZero > 0)
            delete[] vecsra;
        if (noZero > 0)
            delete[] veclm;
        if (noZero > 0)
            delete[] vecfnz;
        if (acop == 1) {
            if (mat != 0)
                delete[] mat;
            if (corte.diam != 0)
                delete[] corte.diam;
            if (corte.indmat != 0)
                delete[] corte.indmat;
            if (corte.discre != 0)
                delete[] corte.discre;
        }

        if (tabp > 0) {
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

        if (flashCompleto == 1) {
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.rholF[j];
            delete[] flash.rholF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.rhogF[j];
            delete[] flash.rhogF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrholDpF[j];
            delete[] flash.DrholDpF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrhogDpF[j];
            delete[] flash.DrhogDpF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrholDtF[j];
            delete[] flash.DrholDtF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrhogDtF[j];
            delete[] flash.DrhogDtF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.DrhowDtF[j];
            delete[] flash.DrhowDtF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.RSF[j];
            delete[] flash.RSF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.cplF[j];
            delete[] flash.cplF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.cpgF[j];
            delete[] flash.cpgF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.cpwF[j];
            delete[] flash.cpwF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.HlF[j];
            delete[] flash.HlF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.HgF[j];
            delete[] flash.HgF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.sigOGF[j];
            delete[] flash.sigOGF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.sigWGF[j];
            delete[] flash.sigWGF;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.viscO[j];
            delete[] flash.viscO;
            for (int j = 0; j < tabent.npont + 1; j++)
                delete[] flash.viscG[j];
            delete[] flash.viscG;

            delete[] flash.PBF;
            delete[] flash.TBF;
        }

        if (nvecEmul > 0) {
            delete[] BSWVec;
            delete[] emulVec;
        }
        if (flup.corrOM == 7) {
            if (tabVisc.parserie > 0) {
                delete[] tabVisc.visc;
                delete[] tabVisc.temp;
            }
        }
        if (nsatAPoc > 0) {
            delete[] satAPoc;
            delete[] tempoSatAPoc;
        }
        if (nsatLRes > 0) {
            delete[] satLRes;
            delete[] tempoSatLRes;
        }
    }

    //! Initialize the class attributes with their default values.
    void iniciaVariaveis();

    /*!
     * Parse the porous-material definitions from the input JSON document.
     * \param material_json JSON object containing the material definitions.
     */
    void parse_materiais(Value &material_json);

    /*!
     * Parse the cross-section configuration from the input JSON document.
     * \param corte_json JSON object containing the cross-section data.
     */
    void parse_corte(Value &corte_json);

    /*!
     * Parse the fluid-property table configuration.
     * \param tabela_json JSON object containing the table data.
     */
    void parse_tabela(Value &tabela_json);

    /*!
     * Parse the oil-water relative-permeability table.
     * \param kRelOA_json JSON object containing the oil-water relative-permeability data.
     */
    void parse_kRelOA(Value &kRelOA_json);

    /*!
     * Parse the oil-gas relative-permeability table.
     * \param kRelOG_json JSON object containing the oil-gas relative-permeability data.
     */
    void parse_kRelOG(Value &kRelOG_json);

    /*!
     * Parse the oil-water capillary-pressure table.
     * \param pcOA_json JSON object containing the oil-water capillary-pressure data.
     */
    void parse_pcOA(Value &pcOA_json);

    /*!
     * Parse the gas-oil capillary-pressure table.
     * \param pcGO_json JSON object containing the gas-oil capillary-pressure data.
     */
    void parse_pcGO(Value &pcGO_json);

    /*!
     * Generate the flash-property tables.
     * \param var Table-generation mode or property selector.
     */
    void geraTabFlash(int var);

    /*!
     * Parse the production-fluid configuration.
     * \param fluidos_producao_json JSON object containing the production-fluid data.
     */
    void parse_fluido_producao(Value &fluidos_producao_json);

    /*!
     * Parse the prescribed well-pressure data.
     * \param pW_json JSON object containing the well-pressure configuration.
     */
    void parse_PW(Value &pW_json);

    /*!
     * Locate the current time interval and interpolation ratio for the general time series.
     * \param ind Output index of the selected interval.
     * \param raz Output interpolation ratio within the selected interval.
     */
    void indrazT(int &ind, double &raz);

    /*!
     * Locate the current time interval and interpolation ratio for the well-pressure series.
     * \param ind Output index of the selected interval.
     * \param raz Output interpolation ratio within the selected interval.
     */
    void indrazPW(int &ind, double &raz);

    /*!
     * Locate a time interval and interpolation ratio in an arbitrary time series.
     * \param ind Output index of the selected interval.
     * \param raz Output interpolation ratio within the selected interval.
     * \param serieTempo Time-series array.
     * \param nserie Number of entries in the time-series array.
     */
    void indraz(int &ind, double &raz, double *serieTempo, int nserie);

    /*!
     * Parse the complementary-fluid configuration.
     * \param fluido_json JSON object containing the complementary-fluid data.
     */
    void parse_fluido_complementar(Value &fluido_json);

    /*!
     * Parse the simulation-time configuration.
     * \param tempo_json JSON object containing the time settings.
     */
    void parse_tempo(Value &tempo_json);

    /*!
     * Parse the porous-medium physical properties.
     * \param prop_json JSON object containing the physical-property data.
     */
    void parse_Prop(Value &prop_json);

    /*!
     * Parse the initial conditions.
     * \param CI_json JSON object containing the initial-condition data.
     */
    void parse_CI(Value &CI_json);

    /*!
     * Parse the boundary conditions.
     * \param CC_json JSON object containing the boundary-condition data.
     */
    void parse_CC(Value &CC_json);

    /*!
     * Parse the computational mesh configuration.
     * \param malha_json JSON object containing the mesh data.
     */
    void parse_malha(Value &malha_json);

    /*!
     * Read and initialize the complete porous-medium model.
     * \param nomeArquivoEntrada Path to the input file.
     */
    void lerPoroso(string nomeArquivoEntrada);
};

#endif /* DADOS1POROSO_H_ */