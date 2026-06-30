/*
 * estruturas.h
 *
 * Created on: July 28, 2022
 * Author: Eduardo
 *
 * Data structures used by the two-dimensional Poisson heat-transfer model,
 * including time settings, region-based material properties, initial
 * conditions, thermal boundary conditions, wall materials, pipe
 * cross-sections, and homogeneous-mesh metadata.
 */

#ifndef ESTRUTURASPOISSON_H_
#define ESTRUTURASPOISSON_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

/*!
 * Store steady-state and transient time-integration settings for the
 * Poisson model.
 *
 * The structure owns the time-step and output-time arrays.
 */
struct detTempoPoisson {
    int perm;         // Steady-state simulation flag.
    int trans;        // Transient simulation flag.
    double tmax;      // Maximum simulation time.
    double relax;     // Solver relaxation factor.
    int parserie;     // Number of entries in the time-step series.
    double *dtmax;    // Maximum time-step values.
    double *tempo;    // Times associated with dtmax.
    double *tempoImp; // Requested output or reporting times.
    int parserieImp;  // Number of requested output times.

    //! Initialize all settings and pointers with default values.
    detTempoPoisson() {
        perm = 0;
        trans = 0;
        tmax = 0;
        relax = 0;
        parserie = 0;
        dtmax = 0;
        tempo = 0;
        tempoImp = 0;
        parserieImp = 0;
    }

    //! Release the owned time-series arrays.
    ~detTempoPoisson() {
        if (parserie > 0) {
            delete[] dtmax;
            delete[] tempo;
        }
        if (parserieImp > 0)
            delete[] tempoImp;
    }
};

/*!
 * Store region-based thermophysical properties for the Poisson model.
 *
 * Each entry defines a rectangular region and its density, specific heat,
 * and thermal conductivity.
 */
struct detPropPoisson {
    int parserie; // Number of property regions.
    double *xmax; // Maximum x-coordinate of each region.
    double *xmin; // Minimum x-coordinate of each region.
    double *ymax; // Maximum y-coordinate of each region.
    double *ymin; // Minimum y-coordinate of each region.
    double *rho;  // Density values.
    double *cp;   // Specific-heat values.
    double *cond; // Thermal-conductivity values.

    //! Initialize all pointers and counters with default values.
    detPropPoisson() {
        parserie = 0;
        xmax = 0;
        xmin = 0;
        ymax = 0;
        ymin = 0;
        rho = 0;
        cp = 0;
        cond = 0;
    }

    //! Release the owned property-region arrays.
    ~detPropPoisson() {
        if (parserie > 0) {
            delete[] xmax;
            delete[] xmin;
            delete[] ymax;
            delete[] ymin;
            delete[] rho;
            delete[] cp;
            delete[] cond;
        }
    }
};

/*!
 * Store region-based initial temperatures for the Poisson model.
 */
struct detCIPoisson {
    int parserie; // Number of initial-condition regions.
    double *xmax; // Maximum x-coordinate of each region.
    double *xmin; // Minimum x-coordinate of each region.
    double *ymax; // Maximum y-coordinate of each region.
    double *ymin; // Minimum y-coordinate of each region.
    double *temp; // Initial temperature assigned to each region.

    //! Initialize all pointers and counters with default values.
    detCIPoisson() {
        parserie = 0;
        xmax = 0;
        xmin = 0;
        ymax = 0;
        ymin = 0;
        temp = 0;
    }

    //! Release the owned initial-condition arrays.
    ~detCIPoisson() {
        if (parserie > 0) {
            delete[] xmax;
            delete[] xmin;
            delete[] ymax;
            delete[] ymin;
            delete[] temp;
        }
    }
};

/*!
 * Store a time-dependent Dirichlet temperature boundary condition.
 */
struct detDiriPoisson {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *val;      // Prescribed temperature values.
    double *tempo;    // Times associated with val.
    int nserie;       // Number of time-series entries.

    //! Initialize the condition with default values.
    detDiriPoisson() {
        rotulo = 0;
        rotuloSTR = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }

    //! Deep-copy constructor.
    detDiriPoisson(const detDiriPoisson &condic) {
        rotulo = condic.rotulo;
        rotuloSTR = condic.rotuloSTR;
        nserie = condic.nserie;
        val = new double[nserie];
        tempo = new double[nserie];
        for (int iCC = 0; iCC < nserie; iCC++) {
            val[iCC] = condic.val[iCC];
            tempo[iCC] = condic.tempo[iCC];
        }
    }

    //! Release the owned time-series arrays.
    ~detDiriPoisson() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
        }
    }

    //! Deep-copy assignment operator.
    detDiriPoisson &operator=(const detDiriPoisson &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
            rotuloSTR = condic.rotuloSTR;
            nserie = condic.nserie;
            val = new double[nserie];
            tempo = new double[nserie];
            for (int iCC = 0; iCC < nserie; iCC++) {
                val[iCC] = condic.val[iCC];
                tempo[iCC] = condic.tempo[iCC];
            }
        }
        return *this;
    }
};

/*!
 * Store a time-dependent Neumann or prescribed normal-gradient boundary
 * condition.
 */
struct detVNPoisson {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *val;      // Prescribed normal-gradient or heat-flux values.
    double *tempo;    // Times associated with val.
    int nserie;       // Number of time-series entries.

    //! Initialize the condition with default values.
    detVNPoisson() {
        rotulo = 0;
        rotuloSTR = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }

    //! Deep-copy constructor.
    detVNPoisson(const detVNPoisson &condic) {
        rotulo = condic.rotulo;
        rotuloSTR = condic.rotuloSTR;
        nserie = condic.nserie;
        val = new double[nserie];
        tempo = new double[nserie];
        for (int iCC = 0; iCC < nserie; iCC++) {
            val[iCC] = condic.val[iCC];
            tempo[iCC] = condic.tempo[iCC];
        }
    }

    //! Release the owned time-series arrays.
    ~detVNPoisson() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
        }
    }

    //! Deep-copy assignment operator.
    detVNPoisson &operator=(const detVNPoisson &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
            rotuloSTR = condic.rotuloSTR;
            nserie = condic.nserie;
            val = new double[nserie];
            tempo = new double[nserie];
            for (int iCC = 0; iCC < nserie; iCC++) {
                val[iCC] = condic.val[iCC];
                tempo[iCC] = condic.tempo[iCC];
            }
        }
        return *this;
    }
};

/*!
 * Store a time-dependent Robin or convective thermal boundary condition.
 */
struct detRicPoisson {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *valAmb;   // Ambient-temperature values.
    double *hAmb;     // Convective heat-transfer coefficients.
    double *tempo;    // Times associated with the condition.
    int nserie;       // Number of time-series entries.

    //! Initialize the condition with default values.
    detRicPoisson() {
        rotulo = 0;
        rotuloSTR = "vazio";
        valAmb = 0;
        hAmb = 0;
        tempo = 0;
        nserie = 0;
    }

    //! Deep-copy constructor.
    detRicPoisson(const detRicPoisson &condic) {
        rotulo = condic.rotulo;
        rotuloSTR = condic.rotuloSTR;
        nserie = condic.nserie;
        valAmb = new double[nserie];
        hAmb = new double[nserie];
        tempo = new double[nserie];
        for (int iCC = 0; iCC < nserie; iCC++) {
            valAmb[iCC] = condic.valAmb[iCC];
            hAmb[iCC] = condic.hAmb[iCC];
            tempo[iCC] = condic.tempo[iCC];
        }
    }

    //! Release the owned time-series arrays.
    ~detRicPoisson() {
        if (nserie > 0) {
            delete[] valAmb;
            delete[] hAmb;
            delete[] tempo;
        }
    }

    //! Deep-copy assignment operator.
    detRicPoisson &operator=(const detRicPoisson &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] valAmb;
                delete[] hAmb;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
            rotuloSTR = condic.rotuloSTR;
            nserie = condic.nserie;
            valAmb = new double[nserie];
            hAmb = new double[nserie];
            tempo = new double[nserie];
            for (int iCC = 0; iCC < nserie; iCC++) {
                valAmb[iCC] = condic.valAmb[iCC];
                hAmb[iCC] = condic.hAmb[iCC];
                tempo[iCC] = condic.tempo[iCC];
            }
        }
        return *this;
    }
};

/*!
 * Aggregate the thermal boundary conditions used by the Poisson solver.
 *
 * The structure owns arrays of Dirichlet, Robin, and Neumann conditions.
 */
struct detCCPoisson {
    int nDiri;             // Number of Dirichlet conditions.
    int nRic;              // Number of Robin conditions.
    int nVN;               // Number of Neumann conditions.
    int rotuloAcop;        // Label assigned to the coupled boundary.
    detDiriPoisson *ccDir; // Dirichlet boundary conditions.
    detRicPoisson *ccRic;  // Robin boundary conditions.
    detVNPoisson *ccVN;    // Neumann boundary conditions.

    //! Initialize the condition collection with default values.
    detCCPoisson() {
        rotuloAcop = 0;
        nDiri = 0;
        nRic = 0;
        nVN = 0;
        ccDir = 0;
        ccRic = 0;
        ccVN = 0;
    }

    //! Deep-copy constructor.
    detCCPoisson(const detCCPoisson &condic) {
        rotuloAcop = condic.rotuloAcop;
        nDiri = condic.nDiri;
        nRic = condic.nRic;
        nVN = condic.nVN;
        ccDir = new detDiriPoisson[nDiri];
        ccRic = new detRicPoisson[nRic];
        ccVN = new detVNPoisson[nVN];
        for (int iCC = 0; iCC < nDiri; iCC++)
            ccDir[iCC] = condic.ccDir[iCC];
        for (int iCC = 0; iCC < nRic; iCC++)
            ccRic[iCC] = condic.ccRic[iCC];
        for (int iCC = 0; iCC < nVN; iCC++)
            ccVN[iCC] = condic.ccVN[iCC];
    }

    //! Release the owned boundary-condition arrays.
    ~detCCPoisson() {
        if (nDiri > 0)
            delete[] ccDir;
        if (nRic > 0)
            delete[] ccRic;
        if (nVN > 0)
            delete[] ccVN;
    }

    //! Deep-copy assignment operator.
    detCCPoisson &operator=(const detCCPoisson &condic) {
        if (this != &condic) {
            if (nDiri > 0)
                delete[] ccDir;
            if (nRic > 0)
                delete[] ccRic;
            if (nVN > 0)
                delete[] ccVN;

            rotuloAcop = condic.rotuloAcop;
            nDiri = condic.nDiri;
            nRic = condic.nRic;
            nVN = condic.nVN;
            ccDir = new detDiriPoisson[nDiri];
            ccRic = new detRicPoisson[nRic];
            ccVN = new detVNPoisson[nVN];
            for (int iCC = 0; iCC < nDiri; iCC++)
                ccDir[iCC] = condic.ccDir[iCC];
            for (int iCC = 0; iCC < nRic; iCC++)
                ccRic[iCC] = condic.ccRic[iCC];
            for (int iCC = 0; iCC < nVN; iCC++)
                ccVN[iCC] = condic.ccVN[iCC];
        }
        return *this;
    }
};

/*!
 * Describe a material used in a pipe-wall or thermal-domain layer.
 */
struct materialP {
    int id;      // Material identifier.
    double cond; // Thermal conductivity in W/(m.K).
    double cp;   // Specific heat capacity in J/(kg.K).
    double rho;  // Density in kg/m3.
    int tipo;    // Material type: 0 -> solid, 1 -> user-defined fluid, 2 -> water, 3 -> air.
    double visc; // Viscosity in cP; used only for type 1.
    double beta; // Thermal-expansion coefficient; used only for type 1.
};

/*!
 * Describe a multilayer pipe-wall or annular cross-section.
 */
struct cortedutoP {
    int id;       // Cross-section identifier.
    int ncam;     // Number of material layers.
    int anul;     // Indicates whether the flow region is annular.
    double a;     // Internal diameter; for annular flow, the smaller annular diameter.
    double b;     // For annular flow, the larger annular diameter.
    double rug;   // Absolute roughness in meters.
    double *diam; // Largest diameter of each material layer.
    int *indmat;  // Material indices associated with the materialP definitions.
    int *discre;  // Discretization assigned to each material layer.
};

/*!
 * Describe a cell or point in a homogeneous two-dimensional Poisson mesh.
 */
struct detMalhaHomPoisson {
    double xcoor; // X-coordinate.
    double ycoor; // Y-coordinate.
    double delx;  // Cell size in the x-direction.
    double dely;  // Cell size in the y-direction.
    int indEle;   // Element index.
    int buraco;   // Hole or excluded-cell flag.
};

#endif /* ESTRUTURASPOISSON_H_ */