/*
 * dados1Poisson.h
 *
 * Created on: August 31, 2023
 * Author: Eduardo
 */

#ifndef DADOS1AxiSim_H_
#define DADOS1AxiSim_H_
#define _USE_MATH_DEFINES

#include "Elem2DPoissonAxiSim.h"
#include "Log.h"
#include "Malha2DPoissonAxiSim.h"
#include "Matriz.h"
#include "Vetor.h"
#include "Leitura.h"
#include "estruturasPoisson.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
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
 * Store and manage the input data for the two-dimensional Poisson model.
 *
 * The class contains the simulation configuration, material properties,
 * initial and boundary conditions, mesh data, thermal coupling parameters,
 * and linear-system solver settings. It also provides routines for parsing
 * the corresponding JSON sections and loading the complete Poisson input.
 */

struct materialAxiSim {
    double cond; // Thermal conductivity, W/mK
    double cp;   // Specific heat capacity, J/kgK
    double rho;  // Density, kg/m3
};

struct corteAxiSim {
    int ncam;     // Number of material layers
    double a;     // Internal diameter or, for annular flow, the smaller annulus diameter
    double *diam; // Largest diameter of each material layer
    int *indmat;  // Material identifier associated with the material structure
    int *discre;  // Discretization count for each material layer
};

class dadosPAxiSim {
  public:

	string entrada; // Main input-file path or name.

    int nmaterial;
    int ncorte;
	materialAxiSim *mat;
    corteAxiSim *corte;
    int nzonas;
    double* zonaProf;
    int profIni;
    int profFim;
    int ncelPoco;
    double* dxPoco;
    double* perfiGeoterm;

    double valAmb;

    detTempoPoisson temp; // Time-discretization and simulation-time data.
    detCCPoisson CC;      // Boundary-condition data.
    int acopDirichlet;

    int ndR;
    int nVert;
    int nno;          // Number of mesh nodes.
    int nele;         // Number of mesh elements.
    int noZero;       // Number of nonzero entries used by the sparse system.
    int **noEle;      // Element-to-node connectivity matrix.
    double *atributo; // Mesh or region attribute associated with each node.
    int *tipo;        // Node or boundary classification array.
    double *vecsra;   // Auxiliary sparse-matrix value vector.
    int *veclm;       // Auxiliary sparse-matrix index vector.
    int *vecfnz;      // Auxiliary first-nonzero-entry index vector.
    double **xcoor;   // Node-coordinate matrix.

    double tempo;   // Current simulation time.
    int acop;       // Thermal-coupling activation flag.
    double tParede; // Wall temperature.
    double* tInt;    // Internal-fluid temperature.
    double tAmb;    // Ambient temperature.

    double* resGlob;
    double hE;       // External convective heat-transfer coefficient.
    double* qAcop;    // Heat-transfer rate for the coupled model.
    double* tInt0; // Initial or reference internal-fluid temperature.
    double dimExt;   // External characteristic dimension.

    double* diamInt; // Internal diameter.
    double diamExt; // External diameter.

    double xmin; // Minimum x-coordinate of the computational domain.
    double xmax; // Maximum x-coordinate of the computational domain.
    double ymin; // Minimum y-coordinate of the computational domain.
    double ymax; // Maximum y-coordinate of the computational domain.

    int nthrdLocal; // Number of threads used by the local computation.

    int rankLU;    // Rank or configuration value used by the LU solver.
    int colore;    // Coloring configuration used by the solver.
    int solverMat; // Matrix-solver selection flag.

    /*!
     * Construct the Poisson-model data from an input file and optional
     * thermal and geometric parameters.
     */
    dadosPAxiSim(string nomeArquivoEntrada, double* vresGlob,
           double* vTint,double* vdiamI,
		   double* dxCel, int ncel, double* vgeoterm);

    //! Construct an empty Poisson-model data object.
    dadosPAxiSim();

    //! Copy constructor.
    dadosPAxiSim(const dadosPAxiSim &);

    //! Copy-assignment operator.
    dadosPAxiSim &operator=(const dadosPAxiSim &);

    //! Release the dynamically allocated mesh and sparse-system arrays.
    ~dadosPAxiSim() {

        if (nmaterial > 0)
            delete[] mat;

        if (nzonas > 0)
            delete[] zonaProf;

        if(ncelPoco>0){
        	delete[] dxPoco;
        	delete[] perfiGeoterm;
        	delete[] resGlob;
        	delete[] tInt;
        	delete[] tInt0;
        	delete[] diamInt;
        	delete[] qAcop;
        }

        if (ncorte > 0) {
            for (int i = 0; i < this->ncorte; i++) {
                delete[] corte[i].diam;
                delete[] corte[i].indmat;
                delete[] corte[i].discre;
            }
            delete[] corte;
        }


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
    }

    //! Initialize the class attributes with their default values.
    void iniciaVariaveis();


    void parse_materiais(Value &material_json);
    void parse_corte(Value &corte_json);
    void parse_NoElemento(Value &NoElemento_json);

    /*!
     * Parse the initial-configuration section of the input JSON document.
     * \param configuracaoInicial_json JSON object containing the initial configuration.
     */
    void parse_configIni(Value &configuracaoInicial_json);

    /*!
     * Parse the boundary-condition section of the input JSON document.
     * \param CC_json JSON object containing the boundary conditions.
     */
    void parse_CC(/*Value &CC_json*/);


    /*!
     * Read and initialize the complete Poisson-model input.
     *
     * \param nomeArquivoEntrada Path to the input file.
     * \param vCondGlob Global thermal conductance or conductivity parameter.
     * \param vCondLoc Local thermal conductance or conductivity parameter.
     * \param vhE External convective heat-transfer coefficient.
     * \param vhInt Internal convective heat-transfer coefficient.
     * \param vTint Internal-fluid temperature.
     * \param vTamb Ambient temperature.
     * \param vdiamI Internal diameter.
     * \param vdiamE External diameter.
     */
    void lerPoisson(string nomeArquivoEntrada, double* vresGlob = 0,
                    double* vTint = 0, double* vdiamI = 0, double* dxCel=0,
					int vncel=0, double* vgeoterm=0);
};

#endif /* DADOS1POISSON_H_ */
