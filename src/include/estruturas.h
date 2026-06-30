/*
 * estruturas.h
 *
 * Created on: July 28, 2022
 * Author: Eduardo
 *
 * Data structures used by the two-dimensional finite-volume solver,
 * including time settings, physical-property tables, initial conditions,
 * boundary conditions, mesh metadata, interfaces, materials, and pipe
 * cross-section definitions.
 */

#ifndef ESTRUTURAS_H_
#define ESTRUTURAS_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

/*!
 * Store steady-state and transient simulation settings.
 *
 * This structure owns the time-step and output-time series and provides
 * deep-copy operations for the dynamically allocated arrays.
 */
struct detTempo {
    int perm;        // Steady-state simulation flag.
    int trans;       // Transient simulation flag.
    double tmax;     // Maximum simulation time.
    double relax;    // General relaxation factor.
    double relaxP;   // Pressure relaxation factor.
    double relaxT;   // Temperature relaxation factor.
    int nvfHR;       // High-resolution volume-fraction configuration.
    int parserie;    // Number of entries in the time-step series.
    double *dtmax;   // Maximum time-step values.
    double *tempoDT; // Times associated with dtmax.

    double *tempoImp; // Requested output or reporting times.
    int parserieImp;  // Number of requested output times.
    int confinado;    // Confined-domain flag.
    int pRef;         // Reference-pressure configuration.

    double tendTemp; // Temperature-trend setting.

    double errP;    // Pressure convergence tolerance.
    double errV;    // Velocity convergence tolerance.
    double errT;    // Temperature convergence tolerance.
    double erroRes; // Residual convergence tolerance.
    int maxIt;      // Maximum number of solver iterations.
    //! Initialize all settings and pointers with default values.
    detTempo() {
        perm = 0;
        trans = 0;
        tmax = 0;
        relax = 0;
        relaxP = 0;
        nvfHR = 0;
        parserie = 0;
        dtmax = 0;
        tempoDT = 0;

        tempoImp = 0;
        parserieImp = 0;
        confinado = 0;
        pRef = 0;
        tendTemp = 0;

        errP = 0;
        errV = 0;
        errT = 0;
        erroRes = 0;
        maxIt = 0;
    }
    //! Deep-copy constructor.
    detTempo(const detTempo &vtemp) {
        perm = vtemp.perm;
        trans = vtemp.trans;
        tmax = vtemp.tmax;
        relax = vtemp.relax;
        relaxP = vtemp.relaxP;
        nvfHR = vtemp.nvfHR;
        parserie = vtemp.parserie;
        parserieImp = vtemp.parserieImp;
        dtmax = new double[parserie];
        tempoDT = new double[parserie];
        tempoImp = new double[parserieImp];
        for (int i = 0; i < parserie; i++) {
            dtmax[i] = vtemp.dtmax[i];
            tempoDT[i] = vtemp.tempoDT[i];
        }
        for (int i = 0; i < parserieImp; i++)
            tempoImp[i] = vtemp.tempoImp[i];
        confinado = vtemp.confinado;
        pRef = vtemp.pRef;

        tendTemp = vtemp.tendTemp;

        errP = vtemp.errP;
        errV = vtemp.errV;
        errT = vtemp.errT;
        erroRes = vtemp.erroRes;
        maxIt = vtemp.maxIt;
    }
    //! Deep-copy assignment operator.
    detTempo &operator=(detTempo &vtemp) {
        if (this != &vtemp) {
            if (parserie > 0) {
                delete[] dtmax;
                delete[] tempoDT;
            }
            if (parserieImp > 0)
                delete[] tempoImp;
            perm = vtemp.perm;
            trans = vtemp.trans;
            tmax = vtemp.tmax;
            relax = vtemp.relax;
            relaxP = vtemp.relaxP;
            nvfHR = vtemp.nvfHR;
            parserie = vtemp.parserie;
            parserieImp = vtemp.parserieImp;
            dtmax = new double[parserie];
            tempoDT = new double[parserie];
            tempoImp = new double[parserieImp];
            for (int i = 0; i < parserie; i++) {
                dtmax[i] = vtemp.dtmax[i];
                tempoDT[i] = vtemp.tempoDT[i];
            }
            for (int i = 0; i < parserieImp; i++)
                tempoImp[i] = vtemp.tempoImp[i];
            confinado = vtemp.confinado;
            pRef = vtemp.pRef;

            tendTemp = vtemp.tendTemp;

            errP = vtemp.errP;
            errV = vtemp.errV;
            errT = vtemp.errT;
            erroRes = vtemp.erroRes;
            maxIt = vtemp.maxIt;
        }
        return *this;
    }
    //! Release the owned time-series arrays.
    ~detTempo() {
        if (parserie > 0) {
            delete[] dtmax;
            delete[] tempoDT;
            parserie = 0;
        }
        if (parserieImp > 0) {
            delete[] tempoImp;
            parserieImp = 0;
        }
    }
};
/*!
 * Store tabulated thermophysical properties.
 *
 * Each property array contains parserie entries and is released by the
 * destructor.
 */
struct detProp {
    int parserie; // Number of property-table entries.
    double *rho;  // Density values.
    double *visc; // Dynamic-viscosity values.
    double *cp;   // Specific-heat values.
    double *cond; // Thermal-conductivity values.
    double *beta; // Thermal-expansion coefficients.
    double *tRef; // Reference temperatures.
    ~detProp() {
        if (parserie > 0) {
            delete[] rho;
            delete[] visc;
            delete[] cp;
            delete[] cond;
            delete[] beta;
            delete[] tRef;
            parserie = 0;
        }
    }
};
/*!
 * Map mesh regions or positions to physical-property definitions.
 */
struct detMapProp {
    int parserie;
    int *interno;  // Internal-region mapping.
    int *inferior; // Lower-region mapping.
    int *indProp;  // Physical-property indices.
    ~detMapProp() {
        if (parserie > 0) {
            delete[] interno;
            delete[] indProp;
            parserie = 0;
        }
    }
};
/*!
 * Store region-based initial conditions for temperature, velocity, and pressure.
 */
struct detCI {
    int parserie;
    double *xmax; // Maximum x-coordinate of each initial-condition region.
    double *xmin; // Minimum x-coordinate of each initial-condition region.
    double *ymax; // Maximum y-coordinate of each initial-condition region.
    double *ymin; // Minimum y-coordinate of each initial-condition region.
    double *temp; // Initial temperature values.
    double *valU; // X-velocity values.
    double *valV; // Y-velocity values.
    double *pres; // Initial pressure values.
    ~detCI() {
        if (parserie > 0) {
            delete[] xmax;
            delete[] xmin;
            delete[] ymax;
            delete[] ymin;
            delete[] temp;
            delete[] valU;
            delete[] valV;
            delete[] pres;
            parserie = 0;
        }
    }
};
/*!
 * Store a time-dependent inlet velocity boundary condition.
 *
 * valU and valV contain the velocity components associated with the values
 * in the tempo time series.
 */
struct detInl {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *valU;
    double *valV;
    double *tempo; // Time values.
    int nserie;    // Number of time-series entries.
    detInl() {
        rotulo = -1;
        rotuloSTR = "vazio";
        valU = 0;
        valV = 0;
        tempo = 0;
        nserie = 0;
    }
    detInl(const detInl &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
        nserie = vinl.nserie;
        valU = new double[nserie];
        valV = new double[nserie];
        tempo = new double[nserie];
        for (int i = 0; i < nserie; i++) {
            valU[i] = vinl.valU[i];
            valV[i] = vinl.valV[i];
            tempo[i] = vinl.tempo[i];
        }
    }
    detInl &operator=(detInl &vinl) {
        if (this != &vinl) {
            if (nserie > 0) {
                delete[] valU;
                delete[] valV;
                delete[] tempo;
            }
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
            nserie = vinl.nserie;
            valU = new double[nserie];
            valV = new double[nserie];
            tempo = new double[nserie];
            for (int i = 0; i < nserie; i++) {
                valU[i] = vinl.valU[i];
                valV[i] = vinl.valV[i];
                tempo[i] = vinl.tempo[i];
            }
        }
        return *this;
    }
    ~detInl() {
        if (nserie > 0) {
            delete[] valU;
            delete[] valV;
            delete[] tempo;
            nserie = 0;
        }
    }
};
/*!
 * Store a time-dependent pressure boundary condition.
 */
struct detPres {
    int rotulo;
    string rotuloSTR;
    double *val;
    double *tempo;
    int nserie;
    detPres() {
        rotulo = -1;
        rotuloSTR = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }
    detPres(const detPres &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
        nserie = vinl.nserie;
        val = new double[nserie];
        tempo = new double[nserie];
        for (int i = 0; i < nserie; i++) {
            val[i] = vinl.val[i];
            tempo[i] = vinl.tempo[i];
        }
    }
    detPres &operator=(detPres &vinl) {
        if (this != &vinl) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
            nserie = vinl.nserie;
            val = new double[nserie];
            tempo = new double[nserie];
            for (int i = 0; i < nserie; i++) {
                val[i] = vinl.val[i];
                tempo[i] = vinl.tempo[i];
            }
        }
        return *this;
    }
    ~detPres() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
            nserie = 0;
        }
    }
};
/*!
 * Store a time-dependent moving-wall velocity boundary condition.
 */
struct detWall {
    int rotulo;
    string rotuloSTR;
    double *velW;
    double *tempo;
    int nserie;
    detWall() {
        rotulo = -1;
        rotuloSTR = "vazio";
        velW = 0;
        tempo = 0;
        nserie = 0;
    }
    detWall(const detWall &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
        nserie = vinl.nserie;
        velW = new double[nserie];
        tempo = new double[nserie];
        for (int i = 0; i < nserie; i++) {
            velW[i] = vinl.velW[i];
            tempo[i] = vinl.tempo[i];
        }
    }
    detWall &operator=(detWall &vinl) {
        if (this != &vinl) {
            if (nserie > 0) {
                delete[] velW;
                delete[] tempo;
            }
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
            nserie = vinl.nserie;
            velW = new double[nserie];
            tempo = new double[nserie];
            for (int i = 0; i < nserie; i++) {
                velW[i] = vinl.velW[i];
                tempo[i] = vinl.tempo[i];
            }
        }
        return *this;
    }
    ~detWall() {
        if (nserie > 0) {
            delete[] velW;
            delete[] tempo;
            nserie = 0;
        }
    }
};
/*!
 * Store a symmetry boundary-condition identifier.
 */
struct detSim {
    int rotulo;
    string rotuloSTR;
    detSim() {
        rotulo = -1;
        rotuloSTR = "vazio";
    }
    detSim(const detSim &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
    }
    detSim &operator=(detSim &vinl) {
        if (this != &vinl) {
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
        }
        return *this;
    }
};
/*!
 * Store a time-dependent Dirichlet boundary condition.
 */
struct detDiri {
    int rotulo;
    string rotuloSTR;
    double *val;
    double *tempo;
    int nserie;
    detDiri() {
        rotulo = -1;
        rotuloSTR = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }
    detDiri(const detDiri &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
        nserie = vinl.nserie;
        val = new double[nserie];
        tempo = new double[nserie];
        for (int i = 0; i < nserie; i++) {
            val[i] = vinl.val[i];
            tempo[i] = vinl.tempo[i];
        }
    }
    detDiri &operator=(detDiri &vinl) {
        if (this != &vinl) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
            nserie = vinl.nserie;
            val = new double[nserie];
            tempo = new double[nserie];
            for (int i = 0; i < nserie; i++) {
                val[i] = vinl.val[i];
                tempo[i] = vinl.tempo[i];
            }
        }
        return *this;
    }
    ~detDiri() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
            nserie = 0;
        }
    }
};
/*!
 * Store a time-dependent prescribed normal-gradient or Neumann condition.
 */
struct detVN {
    int rotulo;
    string rotuloSTR;
    double *val;
    double *tempo;
    int nserie;
    detVN() {
        rotulo = -1;
        rotuloSTR = "vazio";
        val = 0;
        tempo = 0;
        nserie = 0;
    }
    detVN(const detVN &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
        nserie = vinl.nserie;
        val = new double[nserie];
        tempo = new double[nserie];
        for (int i = 0; i < nserie; i++) {
            val[i] = vinl.val[i];
            tempo[i] = vinl.tempo[i];
        }
    }
    detVN &operator=(detVN &vinl) {
        if (this != &vinl) {
            if (nserie > 0) {
                delete[] val;
                delete[] tempo;
            }
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
            nserie = vinl.nserie;
            val = new double[nserie];
            tempo = new double[nserie];
            for (int i = 0; i < nserie; i++) {
                val[i] = vinl.val[i];
                tempo[i] = vinl.tempo[i];
            }
        }
        return *this;
    }
    ~detVN() {
        if (nserie > 0) {
            delete[] val;
            delete[] tempo;
            nserie = 0;
        }
    }
};
/*!
 * Store a time-dependent Robin boundary condition.
 *
 * valAmb contains the ambient values and hAmb contains the associated
 * transfer coefficients.
 */
struct detRic {
    int rotulo;
    string rotuloSTR;
    double *valAmb;
    double *hAmb;
    double *tempo;
    int nserie;
    detRic() {
        rotulo = -1;
        rotuloSTR = "vazio";
        valAmb = 0;
        hAmb = 0;
        tempo = 0;
        nserie = 0;
    }
    detRic(const detRic &vinl) {
        rotulo = vinl.rotulo;
        rotuloSTR = vinl.rotuloSTR;
        nserie = vinl.nserie;
        valAmb = new double[nserie];
        hAmb = new double[nserie];
        tempo = new double[nserie];
        for (int i = 0; i < nserie; i++) {
            valAmb[i] = vinl.valAmb[i];
            hAmb[i] = vinl.hAmb[i];
            tempo[i] = vinl.tempo[i];
        }
    }
    detRic &operator=(detRic &vinl) {
        if (this != &vinl) {
            if (nserie > 0) {
                delete[] valAmb;
                delete[] hAmb;
                delete[] tempo;
            }
            rotulo = vinl.rotulo;
            rotuloSTR = vinl.rotuloSTR;
            nserie = vinl.nserie;
            valAmb = new double[nserie];
            hAmb = new double[nserie];
            tempo = new double[nserie];
            for (int i = 0; i < nserie; i++) {
                valAmb[i] = vinl.valAmb[i];
                hAmb[i] = vinl.hAmb[i];
                tempo[i] = vinl.tempo[i];
            }
        }
        return *this;
    }

    ~detRic() {
        if (nserie > 0) {
            delete[] valAmb;
            delete[] hAmb;
            delete[] tempo;
            nserie = 0;
        }
    }
};
/*!
 * Aggregate all boundary-condition definitions used by the solver.
 *
 * The structure owns arrays for inlet, outlet-pressure, wall, symmetry,
 * Dirichlet, Robin, and Neumann conditions.
 */
struct detCC {
    int nInl;        // Number of inlet conditions.
    int nOut;        // Number of outlet-pressure conditions.
    int nWall;       // Number of wall conditions.
    int nSim;        // Number of symmetry conditions.
    int nDiri;       // Number of Dirichlet conditions.
    int nRic;        // Number of Robin conditions.
    int nVN;         // Number of Neumann conditions.
    detInl *ccInl;   // Inlet boundary conditions.
    detPres *ccPres; // Outlet-pressure boundary conditions.
    detWall *ccWall; // Wall boundary conditions.
    detSim *ccSim;   // Symmetry boundary conditions.
    detDiri *ccDir;  // Dirichlet boundary conditions.
    detRic *ccRic;   // Robin boundary conditions.
    detVN *ccVN;     // Neumann boundary conditions.
    int rotuloAcop;  // Label assigned to the coupled boundary.

    detCC() {
        nInl = 0;
        nOut = 0;
        nWall = 0;
        nSim = 0;
        nDiri = 0;
        nRic = 0;
        nVN = 0;
        ccInl = 0;
        ccPres = 0;
        ccWall = 0;
        ccSim = 0;
        ccDir = 0;
        ccRic = 0;
        ccVN = 0;
        rotuloAcop = -1;
    }
    detCC(const detCC &vinl) {
        rotuloAcop = vinl.rotuloAcop;
        nInl = vinl.nInl;
        nOut = vinl.nOut;
        nWall = vinl.nWall;
        nSim = vinl.nSim;
        nDiri = vinl.nDiri;
        nRic = vinl.nRic;
        nVN = vinl.nVN;
        ccInl = new detInl[nInl];
        ccPres = new detPres[nOut];
        ccWall = new detWall[nWall];
        ccSim = new detSim[nSim];
        ccDir = new detDiri[nDiri];
        ccRic = new detRic[nRic];
        ccVN = new detVN[nVN];
        for (int i = 0; i < nInl; i++)
            ccInl[i] = vinl.ccInl[i];
        for (int i = 0; i < nOut; i++)
            ccPres[i] = vinl.ccPres[i];
        for (int i = 0; i < nWall; i++)
            ccWall[i] = vinl.ccWall[i];
        for (int i = 0; i < nSim; i++)
            ccSim[i] = vinl.ccSim[i];
        for (int i = 0; i < nDiri; i++)
            ccDir[i] = vinl.ccDir[i];
        for (int i = 0; i < nRic; i++)
            ccRic[i] = vinl.ccRic[i];
        for (int i = 0; i < nVN; i++)
            ccVN[i] = vinl.ccVN[i];
    }
    detCC &operator=(detCC &vinl) {
        if (this != &vinl) {
            if (nInl > 0)
                delete[] ccInl;
            if (nOut > 0)
                delete[] ccPres;
            if (nWall > 0)
                delete[] ccWall;
            if (nSim > 0)
                delete[] ccSim;
            if (nDiri > 0)
                delete[] ccDir;
            if (nRic > 0)
                delete[] ccRic;
            if (nVN > 0)
                delete[] ccVN;
            rotuloAcop = vinl.rotuloAcop;
            nInl = vinl.nInl;
            nOut = vinl.nOut;
            nWall = vinl.nWall;
            nSim = vinl.nSim;
            nDiri = vinl.nDiri;
            nRic = vinl.nRic;
            nVN = vinl.nVN;
            ccInl = new detInl[nInl];
            ccPres = new detPres[nOut];
            ccWall = new detWall[nWall];
            ccSim = new detSim[nSim];
            ccDir = new detDiri[nDiri];
            ccRic = new detRic[nRic];
            ccVN = new detVN[nVN];
            for (int i = 0; i < nInl; i++)
                ccInl[i] = vinl.ccInl[i];
            for (int i = 0; i < nOut; i++)
                ccPres[i] = vinl.ccPres[i];
            for (int i = 0; i < nWall; i++)
                ccWall[i] = vinl.ccWall[i];
            for (int i = 0; i < nSim; i++)
                ccSim[i] = vinl.ccSim[i];
            for (int i = 0; i < nDiri; i++)
                ccDir[i] = vinl.ccDir[i];
            for (int i = 0; i < nRic; i++)
                ccRic[i] = vinl.ccRic[i];
            for (int i = 0; i < nVN; i++)
                ccVN[i] = vinl.ccVN[i];
        }
        return *this;
    }

    ~detCC() {
        if (nInl > 0) {
            delete[] ccInl;
            nInl = 0;
        }
        if (nOut > 0) {
            delete[] ccPres;
            nOut = 0;
        }
        if (nWall > 0) {
            delete[] ccWall;
            nWall = 0;
        }
        if (nSim > 0) {
            delete[] ccSim;
            nSim = 0;
        }
        if (nDiri > 0) {
            delete[] ccDir;
            nDiri = 0;
        }
        if (nRic > 0) {
            delete[] ccRic;
            nRic = 0;
        }
        if (nVN > 0) {
            delete[] ccVN;
            nVN = 0;
        }
    }
};

/*!
 * Describe a cell or point in a homogeneous Cartesian mesh.
 */
struct detMalhaHom {
    double xcoor; // X-coordinate.
    double ycoor; // Y-coordinate.
    double delx;  // Cell size in the x-direction.
    double dely;  // Cell size in the y-direction.
    int indEle;   // Element index.
    int buraco;   // Hole or excluded-cell flag.
};

/*!
 * Store the coordinates and bounding box of a two-dimensional interface.
 */
struct detInterFace {
    double *xcoor; // Interface x-coordinates.
    double *ycoor; // Interface y-coordinates.
    int nserie;    // Number of interface points.
    double ymax;   // Maximum y-coordinate.
    double ymin;   // Minimum y-coordinate.
    double xmax;   // Maximum x-coordinate.
    double xmin;   // Minimum x-coordinate.
    detInterFace() {
        xcoor = 0;
        ycoor = 0;
        nserie = 0;
        ymax = 0;
        ymin = 0;
        xmax = 0;
        xmin = 0;
    }
    ~detInterFace() {
        if (nserie > 0) {
            delete[] xcoor;
            delete[] ycoor;
            nserie = 0;
        }
    }
};

/*!
 * Describe a material used by the volume-fraction model.
 */
struct materialVF {
    int id;
    double cond; // Thermal conductivity in W/(m.K).
    double cp;   // Specific heat capacity in J/(kg.K).
    double rho;  // Density in kg/m3.
    int tipo;    // Material type: 0 -> solid, 1 -> user-defined fluid, 2 -> water, 3 -> air.
    double visc; // Viscosity in cP; used only for type 1.
    double beta; // Thermal-expansion coefficient; used only for type 1.
    //! Initialize the material definition with default values.
    materialVF() {
        id = 0;
        cond = 0; // Thermal conductivity in W/(m.K).
        cp = 0;   // Specific heat capacity in J/(kg.K).
        rho = 0;  // Density in kg/m3.
        tipo = 0; // Material type: 0 -> solid, 1 -> user-defined fluid, 2 -> water, 3 -> air.
        visc = 0; // Viscosity in cP; used only for type 1.
        beta = 0;
    }
};

/*!
 * Describe a multilayer pipe cross-section used by the volume-fraction model.
 */
struct cortedutoVF {
    int id;
    int ncam;     // Number of material layers.
    int anul;     // Indicates whether the flow region is annular.
    double a;     // Internal diameter; for annular flow, the smaller annular diameter.
    double b;     // For annular flow, the larger annular diameter.
    double rug;   // Absolute roughness in meters.
    double *diam; // Largest diameter of each material layer.
    int *indmat;  // Material indices associated with the materialVF definitions.
    int *discre;  // Discretization assigned to each material layer.
    //! Initialize the pipe cross-section with default values.
    cortedutoVF() {
        id = 0;
        ncam = 0;   // Number of material layers.
        anul = 0;   // Indicates whether the flow region is annular.
        a = 0;      // Internal diameter; for annular flow, the smaller annular diameter.
        b = 0;      // For annular flow, the larger annular diameter.
        rug = 0;    // Absolute roughness in meters.
        diam = 0;   // Largest diameter of each material layer.
        indmat = 0; // Material indices associated with the materialVF definitions.
        discre = 0; // Discretization assigned to each material layer.
    }
};

#endif /* ESTRUTURAS_H_ */