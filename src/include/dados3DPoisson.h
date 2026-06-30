/*
 * dados1Poisson.h
 *
 * Created on: August 31, 2023
 * Author: Eduardo
 */

#ifndef DADOS3DPOISSON_H_
#define DADOS3DPOISSON_H_

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
// Legacy alternative: #include <ctime.h>
#include "Elem3DPoisson.h"
#include "Geometria.h"
#include "Log.h"
#include "Malha3DPoisson.h"
#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Vetor.h"
#include "estruturaUNV.h"
#include "estruturasPoisson3D.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
 * Store and manage input data for the three-dimensional Poisson model.
 *
 * The class contains simulation-time settings, physical properties,
 * boundary conditions, mesh connectivity and coordinates, thermal-coupling
 * data, geometric references, and linear-system solver configuration.
 *
 * It also owns the dynamically allocated arrays created while loading the
 * three-dimensional model and releases them in the destructor.
 */
class dadosP3D {
  public:
    detTempoPoisson3D temp; // Time-discretization and simulation-time data.
    detPropPoisson3D prop;  // Material and physical-property data.
    detCCPoisson3D CC;      // Boundary-condition and coupling data.

    int nno;          // Number of mesh nodes.
    int nele;         // Number of mesh elements.
    int noZero;       // Number of nonzero entries used by the sparse system.
    int **noEle;      // Element-to-node connectivity matrix.
    double *atributo; // Mesh or region attribute array.
    int *tipo;        // Node, element, or boundary classification array.
    double *vecsra;   // Auxiliary sparse-matrix value vector.
    int *veclm;       // Auxiliary sparse-matrix index vector.
    int *vecfnz;      // Auxiliary first-nonzero-entry index vector.
    double **xcoor;   // Node-coordinate matrix.

    eleOBJ *elementoO; // Array containing auxiliary geometric or UNV element data.

    double tempo;    // Current simulation time.
    int nmaterial;   // Number of materials in the model.
    int acop;        // Thermal-coupling activation flag.
    double angAcop;  // Angular position used by the thermal-coupling model.
    double *tParede; // Wall temperatures at the coupled locations.
    double *tInt;    // Internal-fluid temperatures at the coupled locations.

    double *hE;      // External convective heat-transfer coefficients.
    double *hI;      // Internal convective heat-transfer coefficients.
    double *qAcop;   // Coupled heat-transfer rates.
    double *qTotal;  // Total heat-transfer rates.
    double *tInt0;   // Initial or reference internal-fluid temperatures.
    double dimExt;   // External characteristic dimension.
    double *diamRef; // Reference diameters at the coupled locations.

    int rank;      // Rank or configuration value used by the linear solver.
    int colorido;  // Coloring configuration used by the solver.
    int solverMat; // Matrix-solver selection flag.

    string elearq;  // Element-file path or name.
    string noarq;   // Node-file path or name.
    string polyarq; // Polygon or geometry-file path or name.
    string entrada; // Main input-file path or name.

    /*!
     * Construct the three-dimensional Poisson-model data from an input file.
     * \param nomeArquivoEntrada Path to the input file.
     */
    dadosP3D(string nomeArquivoEntrada);

    //! Construct an empty three-dimensional Poisson-model data object.
    dadosP3D();

    //! Copy constructor.
    dadosP3D(const dadosP3D &);

    //! Copy-assignment operator.
    dadosP3D &operator=(const dadosP3D &);

    //! Release all dynamically allocated mesh, element, and coupling arrays.
    ~dadosP3D() {
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
        if (nele > 0)
            delete[] vecsra;
        if (nele > 0)
            delete[] veclm;
        if (nele > 0)
            delete[] vecfnz;
        if (elementoO != 0)
            delete[] elementoO;
        if (acop == 1 && CC.nAcop > 0) {
            if (tParede != 0)
                delete[] tParede;
            if (tInt != 0)
                delete[] tInt;
            if (hE != 0)
                delete[] hE;
            if (hI != 0)
                delete[] hI;
            if (qAcop != 0)
                delete[] qAcop;
            if (qTotal != 0)
                delete[] qTotal;
            if (tInt0 != 0)
                delete[] tInt0;
            if (diamRef != 0)
                delete[] diamRef;
        }
    }

    //! Initialize the class attributes with their default values.
    void iniciaVariaveis();

    /*!
     * Parse the cross-section or coupled-geometry configuration.
     * \param corte_json JSON object containing the cross-section data.
     */
    void parse_corte(Value &corte_json);

    /*!
     * Parse the initial simulation configuration.
     * \param configuracaoInicial_json JSON object containing the initial configuration.
     */
    void parse_configIni(Value &configuracaoInicial_json);

    /*!
     * Parse the material and physical-property configuration.
     * \param prop_json JSON object containing the physical-property data.
     */
    void parse_Prop(Value &prop_json);

    /*!
     * Parse the initial-condition configuration.
     * \param CI_json JSON object containing the initial-condition data.
     */
    void parse_CI(Value &CI_json);

    /*!
     * Parse the boundary-condition and coupling configuration.
     * \param CC_json JSON object containing the boundary-condition data.
     */
    void parse_CC(Value &CC_json);

    /*!
     * Parse the three-dimensional mesh configuration.
     * \param malha_json JSON object containing the mesh data.
     */
    void parse_malha(Value &malha_json);

    /*!
     * Read and initialize the complete three-dimensional Poisson model.
     * \param nomeArquivoEntrada Path to the input file.
     */
    void lerPoisson(string nomeArquivoEntrada);

    /*!
     * Read and initialize the model using data from an auxiliary instance.
     * \param nomeArquivoEntrada Path to the input file.
     * \param aux Auxiliary three-dimensional Poisson data object.
     */
    void lerPoisson2(string nomeArquivoEntrada, const dadosP3D &aux);
};

#endif /* DADOS3DPOISSON_H_ */