/*
 * estruturas.h
 *
 * Created on: July 28, 2022
 * Author: Eduardo
 *
 * Data structures used by the two-dimensional porous-medium solver,
 * including time settings, regional rock properties, initial and boundary
 * conditions, cross-section geometry, PVTSIM and compositional properties,
 * viscosity tables, relative-permeability curves, and capillary-pressure
 * curves.
 */

#ifndef ESTRUTURASPOROSO_H_
#define ESTRUTURASPOROSO_H_

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
 * porous-medium model.
 *
 * The structure owns the time-step and output-time arrays.
 */
struct detTempoPoroso {
    int perm;      // Steady-state simulation flag.
    int trans;     // Transient simulation flag.
    double tmax;   // Maximum simulation time.
    double relax;  // Solver relaxation factor.
    int parserie;  // Number of entries in the time-step series.
    double *dtmax; // Maximum time-step values.
    double *tempo; // Times associated with dtmax.

    double *tempoImp; // Requested output or reporting times.
    int parserieImp;  // Number of requested output times.
    //! Initialize all settings and pointers with default values.
    detTempoPoroso() {
        perm = 0;
        trans = 0;
        tmax = 0;
        relax = 0;
        dtmax = 0;
        tempo = 0;
        parserie = 0;
        tempoImp = 0;
        parserieImp = 0;
    }
    //! Release the owned time-series arrays.
    ~detTempoPoroso() {
        if (parserie > 0) {
            delete[] dtmax;
            delete[] tempo;
            parserie = 0;
        }
        if (parserieImp > 0) {
            delete[] tempoImp;
            parserieImp = 0;
        }
    }
};
/*!
 * Store region-based rock and porous-medium properties.
 *
 * Each entry defines a rectangular region and its directional
 * permeabilities, porosity, and rock compressibility.
 */
struct detPropPoroso {
    int parserie; // Number of property regions.
    double *xmax; // Maximum x-coordinate of each region.
    double *xmin; // Minimum x-coordinate of each region.
    double *ymax; // Maximum y-coordinate of each region.
    double *ymin; // Minimum y-coordinate of each region.

    double *kY;      // Permeability in the y-direction.
    double *kX;      // Permeability in the x-direction.
    double *poro;    // Porosity.
    double *compRoc; // Rock compressibility.
    //! Initialize all pointers and counters with default values.
    detPropPoroso() {
        parserie = 0;
        xmax = 0;
        xmin = 0;
        ymax = 0;
        ymin = 0;
        kY = 0;
        kX = 0;
        poro = 0;
        compRoc = 0;
    }
    //! Release the owned property-region arrays.
    ~detPropPoroso() {
        if (parserie > 0) {
            delete[] xmax;
            delete[] xmin;
            delete[] ymax;
            delete[] ymin;
            delete[] kY;
            delete[] kX;
            delete[] poro;
            delete[] compRoc;
            parserie = 0;
        }
    }
};
/*!
 * Store region-based initial conditions for the porous-medium model.
 *
 * The primary and auxiliary rectangular bounds are associated with the
 * initial values stored in val1 and val2.
 */
struct detCIPoroso {
    int parserie;  // Number of initial-condition regions.
    double *xmax;  // Maximum x-coordinate of each primary region.
    double *xmin;  // Minimum x-coordinate of each primary region.
    double *ymax;  // Maximum y-coordinate of each primary region.
    double *ymin;  // Minimum y-coordinate of each primary region.
    double *Dxmax; // Maximum x-coordinate of each auxiliary region.
    double *Dxmin; // Minimum x-coordinate of each auxiliary region.
    double *Dymax; // Maximum y-coordinate of each auxiliary region.
    double *Dymin; // Minimum y-coordinate of each auxiliary region.
    double *val1;  // First initial-condition value.
    double *val2;  // Second initial-condition value.
    //! Initialize all pointers and counters with default values.
    detCIPoroso() {
        parserie = 0;
        xmax = 0;
        xmin = 0;
        ymax = 0;
        ymin = 0;
        Dxmax = 0;
        Dxmin = 0;
        Dymax = 0;
        Dymin = 0;
        val1 = 0;
        val2 = 0;
    }
    //! Release the owned initial-condition arrays.
    ~detCIPoroso() {
        if (parserie > 0) {
            delete[] xmax;
            delete[] xmin;
            delete[] ymax;
            delete[] ymin;
            delete[] Dxmax;
            delete[] Dxmin;
            delete[] Dymax;
            delete[] Dymin;
            delete[] val1;
            delete[] val2;
        }
    }
};
/*!
 * Store a time-dependent Dirichlet pressure and saturation boundary condition.
 */
struct detDiriPoroso {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *val;      // Prescribed pressure values.
    double *valSat;   // Prescribed saturation values.
    double *tempo;    // Times associated with the condition.
    int nserie;       // Number of time-series entries.
    //! Initialize the condition data with default values.
    detDiriPoroso() {
        rotulo = 0;
        rotuloSTR = "vazio";
        val = 0;
        valSat = 0;
        tempo = 0;
        nserie = 0;
    }
    //! Release the owned condition arrays.
    ~detDiriPoroso() {
        if (nserie > 0) {
            delete[] val;
            delete[] valSat;
            delete[] tempo;
            nserie = 0;
        }
    }
    //! Deep-copy assignment operator.
    detDiriPoroso &operator=(const detDiriPoroso &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] valSat;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
            rotuloSTR = condic.rotuloSTR;
            nserie = condic.nserie;
            val = new double[nserie];
            valSat = new double[nserie];
            tempo = new double[nserie];
            for (int iCC = 0; iCC < nserie; iCC++) {
                val[iCC] = condic.val[iCC];
                valSat[iCC] = condic.valSat[iCC];
                tempo[iCC] = condic.tempo[iCC];
            }
        }
        return *this;
    }
};
/*!
 * Store a time-dependent Neumann or prescribed normal-gradient condition
 * for pressure and saturation.
 */
struct detVNPoroso {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *val;      // Prescribed pressure-gradient or flux values.
    double *valSat;   // Prescribed saturation-gradient or flux values.
    double *tempo;    // Times associated with the condition.
    int nserie;       // Number of time-series entries.
    //! Initialize the condition data with default values.
    detVNPoroso() {
        rotulo = 0;
        rotuloSTR = "vazio";
        val = 0;
        valSat = 0;
        tempo = 0;
        nserie = 0;
    }
    //! Release the owned condition arrays.
    ~detVNPoroso() {
        if (nserie > 0) {
            delete[] val;
            delete[] valSat;
            delete[] tempo;
            nserie = 0;
        }
    }
    //! Deep-copy assignment operator.
    detVNPoroso &operator=(const detVNPoroso &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] val;
                delete[] valSat;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
            rotuloSTR = condic.rotuloSTR;
            nserie = condic.nserie;
            val = new double[nserie];
            valSat = new double[nserie];
            tempo = new double[nserie];
            for (int iCC = 0; iCC < nserie; iCC++) {
                val[iCC] = condic.val[iCC];
                valSat[iCC] = condic.valSat[iCC];
                tempo[iCC] = condic.tempo[iCC];
            }
        }
        return *this;
    }
};
/*!
 * Store a time-dependent Robin boundary condition for the porous-medium model.
 */
struct detRicPoroso {
    int rotulo;       // Numeric boundary-condition label.
    string rotuloSTR; // Textual boundary-condition label.
    double *valAmb;   // Ambient or external pressure values.
    double *hAmb;     // Transfer coefficients associated with valAmb.
    double *valSat;   // Prescribed saturation values.
    double *tempo;    // Times associated with the condition.
    int nserie;       // Number of time-series entries.
    //! Initialize the condition data with default values.
    detRicPoroso() {
        rotulo = 0;
        rotuloSTR = "vazio";
        valAmb = 0;
        hAmb = 0;
        valSat = 0;
        tempo = 0;
        nserie = 0;
    }
    //! Release the owned condition arrays.
    ~detRicPoroso() {
        if (nserie > 0) {
            delete[] valAmb;
            delete[] hAmb;
            delete[] valSat;
            delete[] tempo;
            nserie = 0;
        }
    }
    //! Deep-copy assignment operator.
    detRicPoroso &operator=(const detRicPoroso &condic) {
        if (this != &condic) {
            if (nserie > 0) {
                delete[] valAmb;
                delete[] hAmb;
                delete[] valSat;
                delete[] tempo;
            }
            rotulo = condic.rotulo;
            rotuloSTR = condic.rotuloSTR;
            nserie = condic.nserie;
            valAmb = new double[nserie];
            hAmb = new double[nserie];
            valSat = new double[nserie];
            tempo = new double[nserie];
            for (int iCC = 0; iCC < nserie; iCC++) {
                valAmb[iCC] = condic.valAmb[iCC];
                hAmb[iCC] = condic.hAmb[iCC];
                valSat[iCC] = condic.valSat[iCC];
                tempo[iCC] = condic.tempo[iCC];
            }
        }
        return *this;
    }
};
/*!
 * Aggregate the pressure and saturation boundary conditions used by the
 * porous-medium solver.
 */
struct detCCPoroso {
    int nDiri;            // Number of Dirichlet conditions.
    int nRic;             // Number of Robin conditions.
    int nVN;              // Number of Neumann conditions.
    int rotuloAcop;       // Label assigned to the coupled boundary.
    double satAcop;       // Saturation imposed at the coupled boundary.
    detDiriPoroso *ccDir; // Dirichlet boundary conditions.
    detRicPoroso *ccRic;  // Robin boundary conditions.
    detVNPoroso *ccVN;    // Neumann boundary conditions.
    //! Initialize the condition data with default values.
    detCCPoroso() {
        rotuloAcop = 0;
        nDiri = 0;
        nRic = 0;
        nVN = 0;
        ccDir = 0;
        ccRic = 0;
        ccVN = 0;
        satAcop = 0.;
    }
    //! Release the owned condition arrays.
    ~detCCPoroso() {
        if (nDiri > 0)
            delete[] ccDir;
        if (nRic > 0)
            delete[] ccRic;
        if (nVN > 0)
            delete[] ccVN;
        nDiri = 0;
        nRic = 0;
        nVN = 0;
    }
    //! Deep-copy assignment operator.
    detCCPoroso &operator=(const detCCPoroso &condic) {
        if (this != &condic) {
            if (nDiri > 0)
                delete[] ccDir;
            if (nRic > 0)
                delete[] ccRic;
            if (nVN > 0)
                delete[] ccVN;

            rotuloAcop = condic.rotuloAcop;
            satAcop = condic.satAcop;
            nDiri = condic.nDiri;
            nRic = condic.nRic;
            nVN = condic.nVN;
            ccDir = new detDiriPoroso[nDiri];
            ccRic = new detRicPoroso[nRic];
            ccVN = new detVNPoroso[nVN];
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
 * Describe the directional permeability, porosity, and compressibility of
 * a porous material.
 */
struct materialPoroso {
    int id;         // Material identifier.
    double kY;      // Permeability in the y-direction.
    double kX;      // Permeability in the x-direction.
    double poro;    // Porosity.
    double compRoc; // Rock compressibility.
};

/*!
 * Describe a multilayer porous cross-section and its datum elevations.
 */
struct cortePoroso {
    int id;         // Cross-section identifier.
    int ncam;       // Number of porous-material layers.
    double a;       // Internal or smaller annular diameter.
    double b;       // Larger annular diameter.
    double *diam;   // Largest diameter of each porous-material layer.
    int *indmat;    // Material indices associated with materialPoroso definitions.
    int *discre;    // Discretization assigned to each material layer.
    double zDatum0; // Datum elevation at the beginning of the cross-section.
    double zDatum1; // Datum elevation at the end of the cross-section.
    //! Initialize the cross-section with default values.
    cortePoroso() {
        id = 0;
        ncam = 0;   // Number of porous-material layers.
        a = 0;      // Internal or smaller annular diameter.
        b = 0;      // Larger annular diameter.
        diam = 0;   // Largest diameter of each porous-material layer.
        indmat = 0; // Material indices associated with materialPoroso definitions.
        discre = 0; // Discretization assigned to each material layer.
        zDatum0 = 0;
        zDatum1 = 0;
    }
};

/*!
 * Describe a cell or point in a homogeneous two-dimensional porous mesh.
 */
struct detMalhaHomPoroso {
    double xcoor; // X-coordinate.
    double ycoor; // Y-coordinate.
    double delx;  // Cell size in the x-direction.
    double dely;  // Cell size in the y-direction.
    int indEle;   // Element index.
    int buraco;   // Hole or excluded-cell flag.
};

/*!
 * Store fluid-property tables loaded from a PVTSIM file.
 *
 * The tables contain phase densities and derivatives, solution-gas ratio,
 * heat capacities, enthalpies, interfacial tensions, viscosities, and the
 * pressure and temperature axes.
 */
struct tabelaFlashPoroso {
    int id;         // Table identifier.
    int TwoOrThree; // Phase configuration: two-phase or three-phase.
    string arquivo; // PVTSIM input-file name.
    int visc;       // Viscosity source: 0 -> PVTSIM table, 1 -> Black-Oil model.

    // Property tables loaded directly from the PVTSIM file.
    double **rholF;    // Liquid-density table.
    double **rhogF;    // Gas-density table.
    double **DrholDpF; // Liquid-density derivative with respect to pressure.
    double **DrhogDpF; // Gas-density derivative with respect to pressure.
    double **DrholDtF; // Liquid-density derivative with respect to temperature.
    double **DrhogDtF; // Gas-density derivative with respect to temperature.
    double **DrhowDtF; // Water-density derivative with respect to temperature.
    double **RSF;      // Solution gas-oil ratio table.
    double **cplF;     // Liquid specific-heat table.
    double **cpgF;     // Gas specific-heat table.
    double **cpwF;     // Water specific-heat table.
    double **HlF;      // Liquid-enthalpy table.
    double **HgF;      // Gas-enthalpy table.
    double **sigOGF;   // Oil-gas interfacial-tension table.
    double **sigWGF;   // Water-gas interfacial-tension table.
    double **viscO;    // Oil-viscosity table.
    double **viscG;    // Gas-viscosity table.
    double *PBF;       // Pressure-axis values.
    double *TBF;       // Temperature-axis values.
};

/*!
 * Store the component and interaction parameters used by the compositional
 * porous-fluid model.
 */
struct composicionalPoroso {
    string arquivo;             // Compositional-model input-file name.
    int npseudo = 0;            // Number of pseudo-components.
    int liqModel = 0;           // Liquid-phase thermodynamic-model selector.
    int vapModel = 0;           // Vapor-phase thermodynamic-model selector.
    double *fracMol = 0;        // Component molar fractions.
    double *masMol = 0;         // Component molar masses.
    double *tempCrit = 0;       // Critical temperatures.
    double *presCrit = 0;       // Critical pressures.
    double *fatAcent = 0;       // Acentric factors.
    double *parac = 0;          // Parachor values.
    double *TIndepPeneloux = 0; // Temperature-independent Peneloux corrections.
    double *kij = 0;            // Binary interaction coefficients.
    double *lij = 0;            // Additional binary interaction coefficients.
    double *CpIGCoefs = 0;      // Ideal-gas heat-capacity coefficients.
    //! Release the owned compositional-property arrays.
    ~composicionalPoroso() {
        if (npseudo > 0) {
            delete fracMol;
            delete CpIGCoefs;
            delete TIndepPeneloux;
            delete fatAcent;
            delete kij;
            delete lij;
            delete masMol;
            delete parac;
            delete presCrit;
            delete tempCrit;
        }
    }
};

/*!
 * Store a temperature-dependent viscosity table.
 */
struct detTabViscPoroso {
    int parserie; // Number of table entries.
    double *visc; // Viscosity values.
    double *temp; // Temperature values.
};

/*!
 * Define the pressure and temperature limits of a fluid-property table.
 */
struct detTabelaEntalpPoroso {
    int npont;   // Number of points along each table axis.
    double pmax; // Maximum pressure in kgf/cm2.
    double pmin; // Minimum pressure in kgf/cm2.
    double tmax; // Maximum temperature in degrees Celsius.
    double tmin; // Minimum temperature in degrees Celsius.
};

/*!
 * Store the gas and oil relative-permeability curves as functions of
 * gas saturation.
 *
 * The class owns its tabulated arrays and performs deep copies.
 */
class tabelaPemRelOG {
  public:
    int npont;         // Number of tabulated points.
    double *satG;      // Gas-saturation values.
    double *permRelG;  // Gas relative permeability.
    double *permRelOG; // Oil relative permeability in the oil-gas system.
    //! Initialize an empty table.
    tabelaPemRelOG() {
        npont = 0;
        satG = 0;
        permRelG = 0;
        permRelOG = 0;
    }
    //! Deep-copy constructor.
    tabelaPemRelOG(const tabelaPemRelOG &tabela) {
        npont = tabela.npont;
        if (npont > 0) {
            satG = new double[npont];
            permRelG = new double[npont];
            permRelOG = new double[npont];
            for (int itab = 0; itab < npont; itab++)
                satG[itab] = tabela.satG[itab];
            for (int itab = 0; itab < npont; itab++)
                permRelG[itab] = tabela.permRelG[itab];
            for (int itab = 0; itab < npont; itab++)
                permRelOG[itab] = tabela.permRelOG[itab];
        } else {
            satG = 0;
            permRelG = 0;
            permRelOG = 0;
        }
    }
    //! Release the owned tabulated arrays.
    ~tabelaPemRelOG() {
        if (npont > 0) {
            delete[] satG;
            delete[] permRelG;
            delete[] permRelOG;
            npont = 0;
        }
    }

    //! Deep-copy assignment operator.
    tabelaPemRelOG &operator=(const tabelaPemRelOG &tabela) {
        if (this != &tabela) {
            if (npont > 0) {
                delete[] satG;
                delete[] permRelG;
                delete[] permRelOG;
                npont = 0;
            }

            npont = tabela.npont;
            if (npont > 0) {
                satG = new double[npont];
                permRelG = new double[npont];
                permRelOG = new double[npont];
                for (int itab = 0; itab < npont; itab++)
                    satG[itab] = tabela.satG[itab];
                for (int itab = 0; itab < npont; itab++)
                    permRelG[itab] = tabela.permRelG[itab];
                for (int itab = 0; itab < npont; itab++)
                    permRelOG[itab] = tabela.permRelOG[itab];
            } else {
                satG = 0;
                permRelG = 0;
                permRelOG = 0;
                npont = 0;
            }
        }
        return *this;
    }
};

/*!
 * Store the water and oil relative-permeability curves as functions of
 * water saturation.
 *
 * The class owns its tabulated arrays and performs deep copies.
 */
class tabelaPemRelOA {
  public:
    int npont;         // Number of tabulated points.
    double *satW;      // Water-saturation values.
    double *permRelW;  // Water relative permeability.
    double *permRelOW; // Oil relative permeability in the oil-water system.
    //! Initialize an empty table.
    tabelaPemRelOA() {
        npont = 0;
        satW = 0;
        permRelW = 0;
        permRelOW = 0;
    }
    //! Deep-copy constructor.
    tabelaPemRelOA(const tabelaPemRelOA &tabela) {
        npont = tabela.npont;
        if (npont > 0) {
            satW = new double[npont];
            permRelW = new double[npont];
            permRelOW = new double[npont];
            for (int itab = 0; itab < npont; itab++)
                satW[itab] = tabela.satW[itab];
            for (int itab = 0; itab < npont; itab++)
                permRelW[itab] = tabela.permRelW[itab];
            for (int itab = 0; itab < npont; itab++)
                permRelOW[itab] = tabela.permRelOW[itab];
        } else {
            satW = 0;
            permRelW = 0;
            permRelOW = 0;
        }
    }
    //! Release the owned tabulated arrays.
    ~tabelaPemRelOA() {
        if (npont > 0) {
            delete[] satW;
            delete[] permRelW;
            delete[] permRelOW;
            npont = 0;
        }
    }

    //! Deep-copy assignment operator.
    tabelaPemRelOA &operator=(const tabelaPemRelOA &tabela) {
        if (this != &tabela) {
            if (npont > 0) {
                delete[] satW;
                delete[] permRelW;
                delete[] permRelOW;
                npont = 0;
            }

            npont = tabela.npont;
            if (npont > 0) {
                satW = new double[npont];
                permRelW = new double[npont];
                permRelOW = new double[npont];
                for (int itab = 0; itab < npont; itab++)
                    satW[itab] = tabela.satW[itab];
                for (int itab = 0; itab < npont; itab++)
                    permRelW[itab] = tabela.permRelW[itab];
                for (int itab = 0; itab < npont; itab++)
                    permRelOW[itab] = tabela.permRelOW[itab];
            } else {
                satW = 0;
                permRelW = 0;
                permRelOW = 0;
            }
        }
        return *this;
    }
};

/*!
 * Store the oil-water capillary-pressure curve as a function of
 * water saturation.
 *
 * The class owns its tabulated arrays and performs deep copies.
 */
class tabelaPresCapOA {
  public:
    int npont;         // Number of tabulated points.
    double *satW;      // Water-saturation values.
    double *presCapOW; // Oil-water capillary-pressure values.
    //! Initialize an empty table.
    tabelaPresCapOA() {
        npont = 0;
        satW = 0;
        presCapOW = 0;
    }
    //! Deep-copy constructor.
    tabelaPresCapOA(const tabelaPresCapOA &tabela) {
        npont = tabela.npont;
        if (npont > 0) {
            satW = new double[npont];
            presCapOW = new double[npont];
            for (int itab = 0; itab < npont; itab++)
                satW[itab] = tabela.satW[itab];
            for (int itab = 0; itab < npont; itab++)
                presCapOW[itab] = tabela.presCapOW[itab];
        } else {
            satW = 0;
            presCapOW = 0;
        }
    }
    //! Release the owned tabulated arrays.
    ~tabelaPresCapOA() {
        if (npont > 0) {
            delete[] satW;
            delete[] presCapOW;
            npont = 0;
        }
    }

    //! Deep-copy assignment operator.
    tabelaPresCapOA &operator=(const tabelaPresCapOA &tabela) {
        if (this != &tabela) {
            if (npont > 0) {
                delete[] satW;
                delete[] presCapOW;
                npont = 0;
            }

            npont = tabela.npont;
            if (npont > 0) {
                satW = new double[npont];
                presCapOW = new double[npont];
                for (int itab = 0; itab < npont; itab++)
                    satW[itab] = tabela.satW[itab];
                for (int itab = 0; itab < npont; itab++)
                    presCapOW[itab] = tabela.presCapOW[itab];
            } else {
                satW = 0;
                presCapOW = 0;
                npont = 0;
            }
        }
        return *this;
    }
};

/*!
 * Store the gas-oil capillary-pressure curve as a function of
 * gas saturation.
 *
 * The class owns its tabulated arrays and performs deep copies.
 */
class tabelaPresCapGO {
  public:
    int npont;         // Number of tabulated points.
    double *satG;      // Gas-saturation values.
    double *presCapGO; // Gas-oil capillary-pressure values.
    //! Initialize an empty table.
    tabelaPresCapGO() {
        npont = 0;
        satG = 0;
        presCapGO = 0;
    }
    //! Deep-copy constructor.
    tabelaPresCapGO(const tabelaPresCapGO &tabela) {
        npont = tabela.npont;
        if (npont > 0) {
            satG = new double[npont];
            presCapGO = new double[npont];
            for (int itab = 0; itab < npont; itab++)
                satG[itab] = tabela.satG[itab];
            for (int itab = 0; itab < npont; itab++)
                presCapGO[itab] = tabela.presCapGO[itab];
        } else {
            satG = 0;
            presCapGO = 0;
            npont = 0;
        }
    }
    //! Release the owned tabulated arrays.
    ~tabelaPresCapGO() {
        if (npont > 0) {
            delete[] satG;
            delete[] presCapGO;
            npont = 0;
        }
    }

    //! Deep-copy assignment operator.
    tabelaPresCapGO &operator=(const tabelaPresCapGO &tabela) {
        if (this != &tabela) {
            if (npont > 0) {
                delete[] satG;
                delete[] presCapGO;
                npont = 0;
            }

            npont = tabela.npont;
            if (npont > 0) {
                satG = new double[npont];
                presCapGO = new double[npont];
                for (int itab = 0; itab < npont; itab++)
                    satG[itab] = tabela.satG[itab];
                for (int itab = 0; itab < npont; itab++)
                    presCapGO[itab] = tabela.presCapGO[itab];
            } else {
                satG = 0;
                presCapGO = 0;
            }
        }
        return *this;
    }
};

#endif /* ESTRUTURASPOROSO_H_ */