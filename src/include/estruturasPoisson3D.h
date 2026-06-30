/*
 * estruturas.h
 *
 * Created on: July 28, 2022
 * Author: Eduardo
 *
 * Data structures used by the three-dimensional Poisson heat-transfer model,
 * including time settings, region-based material properties, initial
 * conditions, thermal boundary conditions, region transitions, coupled
 * boundaries, wall materials, and pipe cross-sections.
 */

#ifndef ESTRUTURASPOISSON3D_H_
#define ESTRUTURASPOISSON3D_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

/*!
 * Store time-integration and output settings for the three-dimensional
 * Poisson model.
 *
 * The structure owns the time-step and output-time arrays.
 */
struct detTempoPoisson3D {
    double relax;  // Solver relaxation factor.
    int parserie;  // Number of entries in the time-step series.
    double *dtmax; // Maximum time-step values.
    double *tempo; // Times associated with dtmax.

    double *tempoImp; // Requested output or reporting times.
    int parserieImp;  // Number of requested output times.

    //! Initialize all settings and pointers with default values.
    detTempoPoisson3D() {
        relax = 1;
        parserie = 0;
        dtmax = 0;
        tempo = 0;
        tempoImp = 0;
        parserieImp = 0;
    }

    //! Release the owned time-series arrays.
    ~detTempoPoisson3D() {
        if (parserie > 0) {
            delete[] dtmax;
            delete[] tempo;
        }
        if (parserieImp > 0)
            delete[] tempoImp;
    }
};

/*!
 * Store region-based thermophysical properties for the three-dimensional
 * Poisson model.
 */
struct detPropPoisson3D {
    int parserie;   // Number of material-property regions.
    string *regiao; // Region identifiers.
    double *rho;    // Density values.
    double *cp;     // Specific-heat values.
    double *cond;   // Thermal-conductivity values.

    //! Initialize all pointers and counters with default values.
    detPropPoisson3D() {
        parserie = 0;
        regiao = 0;
        rho = 0;
        cp = 0;
        cond = 0;
    }

    //! Release the owned region and property arrays.
    ~detPropPoisson3D() {
        if (parserie > 0) {
            delete[] regiao;
            delete[] rho;
            delete[] cp;
            delete[] cond;
        }
    }
};

/*!
 * Store region-based initial temperatures for the three-dimensional
 * Poisson model.
 */
struct detCIPoisson3D {
    int parserie;   // Number of initial-condition regions.
    string *regiao; // Region identifiers.
    double *temp;   // Initial temperatures.

    //! Release the owned region and temperature arrays.
    ~detCIPoisson3D() {
        if (parserie > 0) {
            delete[] regiao;
            delete[] temp;
        }
    }
};

/*!
 * Store a time-dependent Dirichlet temperature boundary condition.
 */
struct detDiriPoisson3D {
    string rotulo; // Boundary-region label.
    double *val;   // Prescribed temperature values.
    double *tempo; // Times associated with val.
    int nserie;    // Number of time-series entries.

    //! Initialize the condition with default values.
    detDiriPoisson3D() {
        rotulo = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }

    //! Release the owned time-series arrays.
    ~detDiriPoisson3D() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
        }
    }

    //! Deep-copy assignment operator.
    detDiriPoisson3D &operator=(const detDiriPoisson3D &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
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

    //! Move-assignment operator.
    detDiriPoisson3D &operator=(detDiriPoisson3D &&condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = std::move(condic.rotulo);
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
struct detVNPoisson3D {
    string rotulo; // Boundary-region label.
    double *val;   // Prescribed normal-gradient or heat-flux values.
    double *tempo; // Times associated with val.
    int nserie;    // Number of time-series entries.

    //! Initialize the condition with default values.
    detVNPoisson3D() {
        rotulo = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }

    //! Release the owned time-series arrays.
    ~detVNPoisson3D() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
        }
    }

    //! Deep-copy assignment operator.
    detVNPoisson3D &operator=(const detVNPoisson3D &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
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

    //! Move-assignment operator.
    detVNPoisson3D &operator=(detVNPoisson3D &&condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = std::move(condic.rotulo);
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
struct detRicPoisson3D {
    string rotulo;  // Boundary-region label.
    double *valAmb; // Ambient-temperature values.
    double *hAmb;   // Convective heat-transfer coefficients.
    double *tempo;  // Times associated with the condition.
    int nserie;     // Number of time-series entries.

    //! Initialize the condition with default values.
    detRicPoisson3D() {
        rotulo = "vazio";
        valAmb = 0;
        hAmb = 0;
        tempo = 0;
        nserie = 0;
    }

    //! Release the owned time-series arrays.
    ~detRicPoisson3D() {
        if (nserie > 0) {
            delete[] valAmb;
            delete[] hAmb;
            delete[] tempo;
        }
    }

    //! Deep-copy assignment operator.
    detRicPoisson3D &operator=(const detRicPoisson3D &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] valAmb;
                delete[] hAmb;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
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

    //! Move-assignment operator.
    detRicPoisson3D &operator=(detRicPoisson3D &&condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] valAmb;
                delete[] hAmb;
                delete[] tempo;
            }
            rotulo = std::move(condic.rotulo);
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
 * Identify a boundary or mesh region where the material region changes.
 */
struct detMudaRegiao3D {
    string rotulo; // Region-change label.

    //! Initialize the label with its default value.
    detMudaRegiao3D() {
        rotulo = "vazio";
    }

    //! Copy-assignment operator.
    detMudaRegiao3D &operator=(const detMudaRegiao3D &condic) {
        if (this != &condic) {
            rotulo = condic.rotulo;
        }
        return *this;
    }

    //! Move-assignment operator.
    detMudaRegiao3D &operator=(detMudaRegiao3D &&condic) noexcept {
        if (this != &condic) {
            rotulo = std::move(condic.rotulo);
        }
        return *this;
    }
};

/*!
 * Aggregate the boundary and coupling definitions used by the
 * three-dimensional Poisson solver.
 *
 * The structure owns arrays of coupled-region labels, Dirichlet, Robin,
 * Neumann, and material-region-change definitions.
 */
struct detCCPoisson3D {
    int nDiri;                   // Number of Dirichlet conditions.
    int nRic;                    // Number of Robin conditions.
    int nVN;                     // Number of Neumann conditions.
    int nMuda;                   // Number of material-region changes.
    int nAcop;                   // Number of coupled boundaries.
    string *rotuloAcop;          // Labels assigned to coupled boundaries.
    detDiriPoisson3D *ccDir;     // Dirichlet boundary conditions.
    detRicPoisson3D *ccRic;      // Robin boundary conditions.
    detVNPoisson3D *ccVN;        // Neumann boundary conditions.
    detMudaRegiao3D *mudaRegiao; // Material-region-change definitions.

    //! Initialize the condition collection with default values.
    detCCPoisson3D() {
        nAcop = 0;
        rotuloAcop = 0;
        nDiri = 0;
        nRic = 0;
        nVN = 0;
        nMuda = 0;
        ccDir = 0;
        ccRic = 0;
        ccVN = 0;
        mudaRegiao = 0;
    }

    //! Release all owned condition and label arrays.
    ~detCCPoisson3D() {
        if (nAcop > 0)
            delete[] rotuloAcop;
        if (nDiri > 0)
            delete[] ccDir;
        if (nRic > 0)
            delete[] ccRic;
        if (nVN > 0)
            delete[] ccVN;
        if (nMuda > 0)
            delete[] mudaRegiao;
    }

    //! Deep-copy assignment operator.
    detCCPoisson3D &operator=(const detCCPoisson3D &condic) {
        if (this != &condic) {
            if (nAcop > 0)
                delete[] rotuloAcop;
            if (nDiri > 0)
                delete[] ccDir;
            if (nRic > 0)
                delete[] ccRic;
            if (nVN > 0)
                delete[] ccVN;
            if (nMuda > 0)
                delete[] mudaRegiao;

            nAcop = condic.nAcop;
            nDiri = condic.nDiri;
            nRic = condic.nRic;
            nVN = condic.nVN;
            nMuda = condic.nMuda;
            rotuloAcop = new string[nAcop];
            ccDir = new detDiriPoisson3D[nDiri];
            ccRic = new detRicPoisson3D[nRic];
            ccVN = new detVNPoisson3D[nVN];
            mudaRegiao = new detMudaRegiao3D[nMuda];
            for (int iCC = 0; iCC < nAcop; iCC++)
                rotuloAcop[iCC] = condic.rotuloAcop[iCC];
            for (int iCC = 0; iCC < nDiri; iCC++)
                ccDir[iCC] = condic.ccDir[iCC];
            for (int iCC = 0; iCC < nRic; iCC++)
                ccRic[iCC] = condic.ccRic[iCC];
            for (int iCC = 0; iCC < nVN; iCC++)
                ccVN[iCC] = condic.ccVN[iCC];
            for (int iCC = 0; iCC < nMuda; iCC++)
                mudaRegiao[iCC] = condic.mudaRegiao[iCC];
        }
        return *this;
    }

    //! Move-assignment operator.
    detCCPoisson3D &operator=(detCCPoisson3D &&condic) {
        if (this != &condic) {
            if (nAcop > 0)
                delete[] rotuloAcop;
            if (nDiri > 0)
                delete[] ccDir;
            if (nRic > 0)
                delete[] ccRic;
            if (nVN > 0)
                delete[] ccVN;
            if (nMuda > 0)
                delete[] mudaRegiao;

            nAcop = condic.nAcop;
            nDiri = condic.nDiri;
            nRic = condic.nRic;
            nVN = condic.nVN;
            nMuda = condic.nMuda;
            rotuloAcop = new string[nAcop];
            ccDir = new detDiriPoisson3D[nDiri];
            ccRic = new detRicPoisson3D[nRic];
            ccVN = new detVNPoisson3D[nVN];
            mudaRegiao = new detMudaRegiao3D[nMuda];
            for (int iCC = 0; iCC < nAcop; iCC++)
                rotuloAcop[iCC] = std::move(condic.rotuloAcop[iCC]);
            for (int iCC = 0; iCC < nDiri; iCC++)
                ccDir[iCC] = condic.ccDir[iCC];
            for (int iCC = 0; iCC < nRic; iCC++)
                ccRic[iCC] = condic.ccRic[iCC];
            for (int iCC = 0; iCC < nVN; iCC++)
                ccVN[iCC] = condic.ccVN[iCC];
            for (int iCC = 0; iCC < nMuda; iCC++)
                mudaRegiao[iCC] = condic.mudaRegiao[iCC];
        }
        return *this;
    }
};

/*!
 * Describe a material used in a three-dimensional pipe-wall or thermal region.
 */
struct materialPoisson3D {
    int id;      // Material identifier.
    double cond; // Thermal conductivity in W/(m.K).
    double cp;   // Specific heat capacity in J/(kg.K).
    double rho;  // Density in kg/m3.
    int tipo;    // Material type: 0 -> solid, 1 -> user-defined fluid, 2 -> water, 3 -> air.
    double visc; // Viscosity in cP; used only for type 1.
    double beta; // Thermal-expansion coefficient; used only for type 1.
};

/*!
 * Describe a multilayer pipe-wall or annular cross-section used by the
 * three-dimensional Poisson model.
 */
struct cortedutoPoisson3D {
    int id;       // Cross-section identifier.
    int ncam;     // Number of material layers.
    int anul;     // Indicates whether the flow region is annular.
    double a;     // Internal diameter; for annular flow, the smaller annular diameter.
    double b;     // For annular flow, the larger annular diameter.
    double rug;   // Absolute roughness in meters.
    double *diam; // Largest diameter of each material layer.
    int *indmat;  // Material indices associated with materialPoisson3D definitions.
    int *discre;  // Discretization assigned to each material layer.
    double comp;  // Additional cross-section or material-property coefficient.
};

#endif /* ESTRUTURASPOISSON3D_H_ */