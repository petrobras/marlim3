/*
 * elem2dPoroso.h
 *
 * Created on: June 22, 2022
 * Author: Eduardo
 */

#ifndef elem2dPorosoPOISSON_H_
#define elem2dPorosoPOISSON_H_
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "Matriz.h"
#include "PropFlu.h"
#include "PropFluCol.h"
#include "Vetor.h"
#include "celRad.h"
#include "estruturaTabDin.h"
#include "estruturasPoroso.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

/*!
 * Store the geometry, state variables, gradients, and porous-medium
 * properties associated with a two-dimensional finite-volume element.
 */
struct elementoPoroso {
    int nvert;              // Number of vertices.
    int dim;                // Spatial dimension of the problem.
    int indEle;             // Element index.
    int nele;               // Total number of elements in the discretization.
    int *noElem;            // Indices of the element nodes.
    int *indFace;           // Indices of neighboring elements associated with each face.
    int *ccFace;            // Boundary-condition identifiers associated with each face.
    vector<int> indVizCres; // Neighbor indices sorted in ascending order.
    double *orientFace;     // Face-orientation correction factors that ensure
    // every face normal points outward from the element.
    double *centroElem;     // Coordinates of the element geometric center.
    double *centroideElem;  // Coordinates of the element centroid.
    double **centroideFace; // Coordinates of each face centroid.
    double **coordVert;     // Coordinates of each element vertex.
    double **dCF;           // Vectors from the element centroid to each face center.
    double *dCFMod;         // Magnitudes of the dCF vectors.
    double vElem;           // Element volume or area.
    int **noFace;           // Node indices associated with each face.
    double **sFace;         // Face-area vectors.
    double *sFaceMod;       // Magnitudes of the face-area vectors.
    double *ownFace;        // Indicates whether the element owns each face-area vector.
    double **vecE;          // Unit vectors from the element centroid toward neighboring centroids.
    double *modE;           // Distances between the element centroid and neighboring centroids.
    double **vecT;          // Unit vectors normal to vecE.
    double *modT;           // Magnitudes associated with vecT.
    double *fatG;           // Ratio between the face-intersection-to-centroid distance and
    // the distance between two neighboring element centroids.
    double *angES;   // Angle between the centroid-connecting line and the face-area vector.
    double **fInter; // Coordinates where the centroid-connecting line intersects each face.
    double **fIfC;   // Vectors from each intersection point to the corresponding face centroid.

    double **vecSDif;   // Diffusive surface vectors.
    double *vecSDifMod; // Magnitudes of the diffusive surface vectors.

    double presC;  // Cell-centered pressure.
    double presCI; // Pressure stored for the previous or intermediate iteration.
    double presC0; // Pressure at the previous time level.

    double *gradGreenP;        // Green-Gauss pressure gradient.
    double *gradGreenPI;       // Pressure gradient stored for the previous or intermediate iteration.
    double *gradGreenZdatum;   // Green-Gauss datum-elevation gradient.
    double *gradGreenZdatumI;  // Datum-elevation gradient stored for the previous or intermediate iteration.
    double *gradGreenAZdatum;  // Green-Gauss gradient of the elevation-related auxiliary term.
    double *gradGreenAZdatumI; // Previous or intermediate gradient of the elevation-related auxiliary term.
    double *gradGreenPcOG;     // Green-Gauss oil-gas capillary-pressure gradient.
    double *gradGreenPcAO;     // Green-Gauss water-oil capillary-pressure gradient.
    double *gradGreenPcAOI;    // Previous or intermediate water-oil capillary-pressure gradient.
    double *gradGreenSw;       // Green-Gauss water-saturation gradient.
    double *gradGreenSwI;      // Water-saturation gradient stored for the previous or intermediate iteration.

    double kY;      // Permeability in the y-direction.
    double kX;      // Permeability in the x-direction.
    double poro;    // Porosity.
    double compRoc; // Rock compressibility.

    double sLC; // Current liquid saturation.
    double sLI; // Previous or intermediate liquid saturation.
    double sWC; // Current water saturation.
    double sWI; // Previous or intermediate water saturation.

    double alfC;  // Current gas or phase fraction.
    double alfC0; // Gas or phase fraction at the previous time level.

    double vbo;     // Oil formation-volume-factor-related value.
    double rhostd;  // Oil density at standard conditions.
    double rhogstd; // Gas density at standard conditions.
    double rhoastd; // Water density at standard conditions.

    double rhoP;  // Oil density at current conditions.
    double rhogP; // Gas density at current conditions.
    double rhoaP; // Water density at current conditions.

    double rhoP0;  // Oil density at the previous time level.
    double rhogP0; // Gas density at the previous time level.
    double rhoaP0; // Water density at the previous time level.

    double pcOG; // Oil-gas capillary pressure.
    double pcAO; // Water-oil capillary pressure.

    double kabsol; // Absolute permeability.
    double zD;     // Datum elevation.

    int tipoModelOleo; // Oil-model selection.
    double compOleo;   // Oil compressibility.
    double rhoRef;     // Reference oil density.
    double pBolha;     // Bubble-point pressure.

    //! Initialize all pointers and state variables with default values.
    elementoPoroso() {
        nvert = 0;      // Number of vertices.
        dim = 0;        // Spatial dimension of the problem.
        indEle = 0;     // Element index.
        nele = 0;       // Total number of elements in the discretization.
        noElem = 0;     // Node-index array.
        indFace = 0;    // Neighbor-element index array.
        ccFace = 0;     // Face boundary-condition array.
        orientFace = 0; // Face-orientation correction array.
        // Ensures that every face normal points outward from the element.
        centroElem = 0;    // Element geometric-center coordinates.
        centroideElem = 0; // Element centroid coordinates.
        indVizCres = vector<int>(0);
        centroideFace = 0; // Face-centroid coordinates.
        coordVert = 0;     // Element-vertex coordinates.
        dCF = 0;           // Vectors from the element centroid to each face center.
        dCFMod = 0;        // Magnitudes of the dCF vectors.
        vElem = 0;         // Element volume or area.
        noFace = 0;
        sFace = 0;    // Face-area vectors.
        sFaceMod = 0; // Magnitudes of the face-area vectors.
        ownFace = 0;  // Face-area vector ownership flags.
        vecE = 0;     // Unit vectors from the element centroid toward neighboring centroids.
        modE = 0;     // Distances between the element centroid and neighboring centroids.
        vecT = 0;     // Unit vectors normal to vecE.
        modT = 0;
        fatG = 0; // Face-intersection geometric weighting factors.
        // Based on the distance between two neighboring element centroids.
        angES = 0;  // Angles between centroid-connecting lines and face-area vectors.
        fInter = 0; // Face-intersection coordinates.
        fIfC = 0;   // Vectors from face-intersection points to face centroids.

        vecSDif = 0;
        vecSDifMod = 0;

        presC = 0;
        presCI = 0;
        presC0 = 0;

        gradGreenP = 0;
        gradGreenPI = 0;
        gradGreenZdatum = 0;
        gradGreenZdatumI = 0;
        gradGreenAZdatum = 0;
        gradGreenAZdatumI = 0;
        gradGreenPcOG = 0;
        gradGreenPcAO = 0;
        gradGreenPcAOI = 0;
        gradGreenSw = 0;
        gradGreenSwI = 0;

        kY = 0;
        kX = 0;
        poro = 0;
        compRoc = 0;

        sLC = 0;
        sLI = 0;
        sWC = 0;
        sWI = 0;

        alfC = 0;
        alfC0 = 0;

        vbo = 0;
        rhostd = 0;
        rhogstd = 0;
        rhoastd = 0;

        rhoP = 0;
        rhogP = 0;
        rhoaP = 0;

        rhoP0 = 0;
        rhogP0 = 0;
        rhoaP0 = 0;

        pcOG = 0;
        pcAO = 0;

        kabsol = 0;
        zD = 0;

        pBolha = 1000.;
        tipoModelOleo = 1;
        compOleo = 1e-5;
        rhoRef = 900;
    }
};

/*!
 * Represent and assemble a two-dimensional finite-volume element for
 * multiphase flow in a porous medium.
 *
 * The class manages neighboring elements, face pressures and saturations,
 * phase flow rates, densities, viscosities, mobilities, Darcy velocities,
 * capillary-pressure gradients, boundary conditions, CFL restrictions,
 * and the local linear system.
 */
class elem2dPoroso {
  public:
    detCCPoroso CC;           // Porous-medium boundary-condition data.
    elementoPoroso cel2D;     // Data associated with the current porous element.
    int nvizinho;             // Number of neighboring elements.
    elementoPoroso **vizinho; // Pointers to neighboring elements.
    int *kvizinho;            // Local face or neighbor-index mapping.
    varGlob1D *vg1dSP;        // Pointer to shared one-dimensional global variables.

    //! Construct and initialize a two-dimensional porous-medium element.
    elem2dPoroso(varGlob1D *Vvg1dSP = 0, double **xcoor = 0, int **noEle = 0, int *tipo = 0, double *atributo = 0, int nVert = 0, int nele = 0, int nno = 0,
                 int vperm = 0, int vtrans = 1, int i = 0, ProFlu vflup = ProFlu(), tabelaPemRelOA vkRelOA = tabelaPemRelOA(), tabelaPemRelOG vkRelOG = tabelaPemRelOG(),
                 tabelaPresCapOA vpcOA = tabelaPresCapOA(), tabelaPresCapGO vpcGO = tabelaPresCapGO());

    //! Copy constructor.
    elem2dPoroso(const elem2dPoroso &);

    //! Copy-assignment operator.
    elem2dPoroso &operator=(const elem2dPoroso &);

    double deriFonteT; // Derivative of the source term.
    double FonteT;     // Source term.

    double *presF;   // Face pressures.
    double *swF;     // Face water saturations.
    double *alfF;    // Face gas or phase fractions.
    double *Qtotal;  // Total face flow rates.
    double *Qw;      // Water face flow rates.
    double *Qoleo;   // Oil face flow rates.
    double *Qgas;    // Gas face flow rates.
    double *rhoF;    // Oil densities at the faces.
    double *rhogF;   // Gas densities at the faces.
    double *rhoaF;   // Water densities at the faces.
    double *rhoZF;   // Elevation-weighted density terms at the faces.
    double *rhoaZF;  // Elevation-weighted water-density terms at the faces.
    double *mioF;    // Oil viscosities at the faces.
    double *migF;    // Gas viscosities at the faces.
    double *miaF;    // Water viscosities at the faces.
    double *kmedF;   // Effective oil permeabilities at the faces.
    double *kmedGF;  // Effective gas permeabilities at the faces.
    double *kmedAF;  // Effective water permeabilities at the faces.
    double *darcyOF; // Oil Darcy velocities at the faces.
    double *darcyGF; // Gas Darcy velocities at the faces.
    double *darcyAF; // Water Darcy velocities at the faces.
    double *pcAOF;   // Water-oil capillary pressures at the faces.

    double fluxCal; // Heat flux.
    double dt;      // Main time-step increment.
    double dtSL;    // Time-step restriction associated with liquid saturation.
    double dtSW;    // Time-step restriction associated with water saturation.
    double dh;      // Elevation difference or hydraulic-head increment.
    int reiniciaSL; // Liquid-saturation restart or time-step correction flag.
    int reiniciaSW; // Water-saturation restart or time-step correction flag.

    double **gradPface;    // Pressure gradients at the faces.
    double **gradZface;    // Elevation gradients at the faces.
    double **gradPcOGface; // Oil-gas capillary-pressure gradients at the faces.
    double **gradPcAOface; // Water-oil capillary-pressure gradients at the faces.

    int perm;  // Permanent or steady-state model flag.
    int trans; // Transient-model flag.

    // Pressure and saturation boundary-condition data.
    double *ccPD;
    double *ccSat;
    double *ccPVN;
    double *ccHR;
    double *ccPambR;

    // High-resolution coefficients and source terms.
    double *coefTHRC;
    double *coefTHRV;
    double *fonteTHR;

    ProFluCol fluc;           // Complementary-fluid properties.
    ProFlu flup;              // Production-fluid properties.
    tabelaPemRelOA kRelOACel; // Oil-water relative-permeability table.
    tabelaPemRelOG kRelOGCel; // Oil-gas relative-permeability table.
    tabelaPresCapOA pcOACel;  // Oil-water capillary-pressure table.
    tabelaPresCapGO pcGOCel;  // Gas-oil capillary-pressure table.

    double satConata; // Connate-water saturation.
    double ip;        // Well productivity or injectivity index.
    double tRes;      // Reservoir temperature.
    double sLRes;     // Reservoir liquid saturation.
    double sWRes;     // Reservoir water saturation.
    double pRes;      // Reservoir pressure.

    Vcr<double> TL;        // Right-hand-side vector of the local system.
    FullMtx<double> local; // Local system matrix.

    //! Release all dynamically allocated geometric, phase, gradient, and solver data.
    ~elem2dPoroso() {
        if (cel2D.nvert > 0) {
            delete[] cel2D.noElem;
            delete[] cel2D.indFace;
            delete[] cel2D.ccFace;
            delete[] cel2D.orientFace;
            delete[] cel2D.centroElem;
            delete[] cel2D.centroideElem;
            delete[] cel2D.dCFMod;
            delete[] cel2D.sFaceMod;
            delete[] cel2D.ownFace;
            delete[] ccPD;
            delete[] ccSat;
            delete[] ccPVN;
            delete[] ccHR;
            delete[] ccPambR;
            delete[] cel2D.vecSDifMod;
            for (int i = 0; i < cel2D.nvert; i++) {
                delete[] cel2D.centroideFace[i];
                delete[] cel2D.coordVert[i];
                delete[] cel2D.dCF[i];
                delete[] cel2D.sFace[i];
                delete[] cel2D.vecE[i];
                delete[] cel2D.vecT[i];
                delete[] cel2D.fInter[i];
                delete[] cel2D.fIfC[i];
                delete[] gradPface[i];
                delete[] gradZface[i];
                delete[] gradPcOGface[i];
                delete[] gradPcAOface[i];

                delete[] cel2D.vecSDif[i];
            }
            delete[] cel2D.centroideFace;
            delete[] cel2D.coordVert;
            delete[] cel2D.dCF;
            delete[] cel2D.sFace;
            delete[] vizinho;
            delete[] kvizinho;
            delete[] cel2D.vecE;
            delete[] cel2D.modE;
            delete[] cel2D.vecT;
            delete[] cel2D.modT;
            delete[] cel2D.fatG;
            delete[] cel2D.angES;
            delete[] cel2D.fInter;
            delete[] cel2D.fIfC;
            delete[] cel2D.gradGreenP;
            delete[] cel2D.gradGreenPI;
            delete[] cel2D.gradGreenZdatum;
            delete[] cel2D.gradGreenZdatumI;
            delete[] cel2D.gradGreenAZdatum;
            delete[] cel2D.gradGreenAZdatumI;
            delete[] cel2D.gradGreenPcOG;
            delete[] cel2D.gradGreenPcAO;
            delete[] cel2D.gradGreenPcAOI;
            delete[] cel2D.gradGreenSw;
            delete[] cel2D.gradGreenSwI;
            delete[] presF;
            delete[] swF;
            delete[] alfF;
            delete[] Qtotal;
            delete[] Qw;
            delete[] Qoleo;
            delete[] Qgas;
            delete[] rhoF;
            delete[] rhogF;
            delete[] rhoaF;
            delete[] rhoZF;
            delete[] rhoaZF;
            delete[] mioF;
            delete[] migF;
            delete[] miaF;
            delete[] kmedF;
            delete[] kmedGF;
            delete[] kmedAF;
            delete[] darcyOF;
            delete[] darcyGF;
            delete[] darcyAF;
            delete[] pcAOF;
            delete[] gradPface;
            delete[] gradZface;
            delete[] gradPcOGface;
            delete[] gradPcAOface;

            delete[] coefTHRC;
            delete[] coefTHRV;
            delete[] fonteTHR;

            delete[] cel2D.vecSDif;
        }
    }

    //! Identify the neighboring element associated with a face.
    void buscaVizinho(int **noEle, int *face, int elem, int nVert, int nEle);

    //! Determine the smallest neighboring-element index.
    void menorIndViz();

    //! Compute and store geometric details for each face.
    void faceDetalhes();

    /*!
     * Interpolate a value from a tabulated data series.
     * \param nserie Number of table entries.
     * \param valx Independent-variable value to be evaluated.
     * \param x Independent-variable array.
     * \param y Dependent-variable array.
     * \return Interpolated dependent-variable value.
     */
    double interpolaTabela(int nserie, double valx, double *x, double *y);

    /*!
     * Calculate the oil fractional-flow or mobility-related function.
     * \param satW Water saturation.
     * \param satG Gas saturation.
     * \return Oil fractional-flow or mobility-related value.
     */
    double fkO(double satW, double satG);

    //! Identify the porous-medium boundary-condition type associated with a face.
    void tipoCC(int i, int &diri, int &vn, int &rich, int &acoplado, int &kcc);

    //! Update the porous-medium boundary-condition values.
    void atualizaCC(int i);

    //! Calculate the Green-Gauss gradients.
    void calcGradGreen();

    //! Calculate the water-phase CFL restriction.
    double cflA();

    //! Advance the water-saturation equation and report whether a restart is required.
    void evoluiSW(int &reinicia);

    //! Calculate the dot product of two vectors.
    double escalar(double *v1, double *v2, int dimV);

    //! Locate the internal index associated with a given identifier.
    int achaInd(int i);

    //! Calculate the diffusive surface contribution.
    void calSupDifu();

    //! Assemble the local porous-medium system.
    void GeraLocal(double relax);

    /*!
     * Locate an interval in a time series and calculate the interpolation ratio.
     *
     * \param ind Index of the time-series value immediately before the requested time.
     * \param raz Relative position of the requested time between entries ind and ind + 1.
     * \param tempo Time at which the series must be evaluated.
     * \param parserie Number of entries in the time series.
     * \param serietemp Array containing the time-series time values.
     */
    void indraz(int &ind /* Index of the time-series value immediately before the requested time. */,
                double &raz /* Relative position between entries ind and ind + 1. */,
                double tempo /* Time at which the series must be evaluated. */,
                int parserie /* Number of entries in the time series. */,
                double *serietemp /* Array containing the time-series time values. */);
};

#endif /* elem2dPorosoPOISSON_H_ */