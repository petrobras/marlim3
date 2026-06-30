/*
 * Elem2D.h
 *
 * Created on: June 22, 2022
 * Author: Eduardo
 */

#ifndef ELEM2D_H_
#define ELEM2D_H_
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "Matriz.h"
#include "PropFluColVF.h"
#include "Vetor.h"
#include "estruturas.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

// Global time-discretization data used by the volume-fraction model.
extern detTempo tempVF;

// Global boundary-condition data.
extern detCC CC;

/*!
 * Store the geometry, state variables, gradients, and physical properties
 * associated with a two-dimensional finite-volume element.
 */
struct elemento {
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
    double *subVol;         // Face-associated subvolumes.
    int **noFace;           // Node indices associated with each face.
    double **sFace;         // Face-area vectors.
    double *sFaceMod;       // Magnitudes of the face-area vectors.
    double *ownFace;        // Indicates whether the element owns each face-area vector.
    double **vecE;          // Unit vectors from the element centroid toward neighboring centroids.
    double *modE;           // Distances between the element centroid and neighboring centroids.
    double **vecT;          // Unit vectors normal to vecE.
    double *modT;           // Magnitudes associated with vecT.
    double *fatG;           // Ratio between the face-intersection-to-centroid distance
                            // and the distance between two neighboring element centroids.
    double *angES;          // Angle between the centroid-connecting line and the face-area vector.
    double **fInter;        // Coordinates where the centroid-connecting line intersects each face.
    double **fIfC;          // Vectors from each intersection point to the corresponding face centroid.

    // Pressure-diffusion and non-orthogonal correction data.
    double *difuPres;
    double *difuPresRC;
    double **vecSDif;
    double *vecSDifMod;
    double *HcU;
    double *HcV;
    double *HcP;
    double *HcT;

    // Cell-centered pressure values.
    double presC;
    double presCEx;
    double presCI;
    double presC0;
    double presCcor;
    double presCIcor;

    // Cell-centered x-velocity values.
    double uC;
    double uCI;
    double uC0;
    double uCEx;

    // Cell-centered y-velocity values.
    double vC;
    double vCI;
    double vC0;
    double vCEx;

    // Cell-centered temperature values.
    double tempC;
    double tempCI;
    double tempC0;
    double tCEx;

    // Cell-centered holdup or volume-fraction values.
    double holC;
    double holCI;
    double holC0;

    double taxaDeform; // Cell deformation or strain rate.

    // Green-Gauss gradients at the current iteration.
    double *gradGreenPres;
    double *gradGreenPresCor;
    double *gradGreenU;
    double *gradGreenV;
    double *gradGreenTemp;
    double *gradGreenHol;

    // Green-Gauss gradients stored for the previous or intermediate iteration.
    double *gradGreenPresI;
    double *gradGreenPresCorI;
    double *gradGreenUI;
    double *gradGreenVI;
    double *gradGreenTempI;
    double *gradGreenHolI;

    double cond;    // Thermal conductivity.
    double cp;      // Specific heat capacity.
    double rho;     // Density.
    double visc;    // Dynamic viscosity.
    double beta;    // Thermal-expansion or model coefficient.
    double tempRef; // Reference temperature.
    double a0U;     // Central coefficient of the x-momentum equation.
    double a0V;     // Central coefficient of the y-momentum equation.
    double *B2Med;  // Cell-averaged body-force or source-term data.
    double **BMedF; // Face-averaged body-force or source-term data.
};

/*!
 * Represent and assemble a two-dimensional finite-volume cell.
 *
 * The class manages element geometry, neighboring cells, face-interpolated
 * variables, boundary conditions, gradients, source terms, CFL information,
 * and local systems for momentum, pressure correction, temperature, and
 * holdup or volume-fraction equations.
 */
class elem2d {
  public:
    elemento cel2D;     // Data associated with the current finite-volume element.
    int nvizinho;       // Number of neighboring elements.
    elemento **vizinho; // Pointers to neighboring elements.
    int *kvizinho;      // Local face or neighbor-index mapping.
    int *confinado;     // Flags identifying confined faces or neighboring regions.

    double deriFonteMass; // Derivative of the mass-source term.
    double FonteMass;     // Mass-source term.

    // Face-centered primitive variables.
    double *presF;
    double *presFcor;
    double *uF;
    double *vF;
    double *tempF;
    double *holF;

    // Upwind face values.
    double *tUpw;
    double *uUpw;
    double *vUpw;
    double *holUpw;

    // Mean face values.
    double *tMed;
    double *uMed;
    double *vMed;
    double *holMed;

    // Downwind face values.
    double *tDow;
    double *uDow;
    double *vDow;
    double *holDow;

    // Values associated with the owner cell K.
    double *tK;
    double *uK;
    double *vK;
    double *holK;

    // Values associated with the neighboring cell L.
    double *tL;
    double *uL;
    double *vL;
    double *holL;

    // Rhie-Chow face-velocity corrections.
    double *uRC;
    double *vRC;
    double *uRC0;
    double *vRC0;

    double *massF; // Mass flow rate through each face.

    // High-resolution x-momentum coefficients and source terms.
    double *coefUHRC;
    double *coefUHRV;
    double *fonteUHR;

    // High-resolution y-momentum coefficients and source terms.
    double *coefVHRC;
    double *coefVHRV;
    double *fonteVHR;

    // High-resolution temperature coefficients and source terms.
    double *coefTHRC;
    double *coefTHRV;
    double *fonteTHR;

    double fluxCal; // Heat flux.
    double dt;      // Time-step increment.

    // Face gradients.
    double **gradPresface;
    double **gradPresfaceCor;
    double **gradUface;
    double **gradVface;

    double *taxadeformFace; // Face deformation or strain rates.
    double *viscFace;       // Face viscosities.

    int perm;  // Permanent or steady-state model flag.
    int trans; // Transient-model flag.

    // Momentum and pressure boundary-condition data.
    double *ccInU;
    double *ccInV;
    double *ccWvel;
    double *ccPres;
    double *ccWall;
    double massTot;

    // Temperature boundary-condition data.
    double *ccTD;
    double *ccTVN;
    double *ccHR;
    double *ccTambR;
    double DCCN;
    double extrapSuaveCCN;

    // Momentum and temperature residual data.
    double residuoU;
    double residuoV;
    double denoResU;
    double denoResV;
    double residuoT;
    double denoResT;

    // Local CFL values and corresponding time-step restrictions.
    double *CFL;
    double *dtCFL;
    double CFLC;

    ProFluColVF flucVF; // Fluid-property model used by the volume-fraction formulation.

    Vcr<double> TLUV;          // Right-hand-side vector of the coupled momentum system.
    FullMtx<double> localUV;   // Local matrix of the coupled momentum system.
    Vcr<double> TLU;           // Right-hand-side vector of the x-momentum system.
    FullMtx<double> localU;    // Local matrix of the x-momentum system.
    Vcr<double> TLV;           // Right-hand-side vector of the y-momentum system.
    FullMtx<double> localV;    // Local matrix of the y-momentum system.
    Vcr<double> TLPCor;        // Right-hand-side vector of the pressure-correction system.
    FullMtx<double> localPCor; // Local matrix of the pressure-correction system.

    Vcr<double> TLT;        // Right-hand-side vector of the temperature system.
    FullMtx<double> localT; // Local matrix of the temperature system.

    varGlob1D *vg1dSP; // Pointer to shared one-dimensional global variables.

    /*!
     * Construct and initialize a two-dimensional finite-volume element.
     */
    elem2d(varGlob1D *Vvg1dSP = 0, double **xcoor = 0, int **noEle = 0, int *tipo = 0, double *atributo = 0, int nVert = 0, int nele = 0, int nno = 0, int i = 0,
           double vdt = 0., int vperm = 1, int vtrans = 0, ProFluColVF vflucVF = ProFluColVF(),
           double vpres = 0., double vu = 0., double vv = 0., double vt = 0., double vfluxCal = 0., double vcond = 0., double vcp = 0.,
           double vrho = 0., double vvisc = 0., double vbeta = 0.);

    //! Copy constructor.
    elem2d(const elem2d &);

    //! Copy-assignment operator.
    elem2d &operator=(const elem2d &);

    //! Release all dynamically allocated geometric, face, gradient, and solver data.
    ~elem2d() {
        if (cel2D.nvert > 0 && cel2D.nele > 0) {
            delete[] cel2D.noElem;
            delete[] cel2D.indFace;
            delete[] cel2D.ccFace;
            delete[] cel2D.orientFace;
            delete[] cel2D.centroElem;
            delete[] cel2D.centroideElem;
            delete[] cel2D.dCFMod;
            delete[] cel2D.sFaceMod;
            delete[] cel2D.subVol;
            delete[] cel2D.ownFace;
            delete[] cel2D.vecSDifMod;
            delete[] cel2D.HcU;
            delete[] cel2D.HcV;
            delete[] cel2D.HcP;
            delete[] cel2D.HcT;
            delete[] ccInU;
            delete[] ccInV;
            delete[] ccWvel;
            delete[] ccPres;
            delete[] ccWall;
            delete[] ccTD;
            delete[] ccTVN;
            delete[] ccHR;
            delete[] ccTambR;
            for (int i = 0; i < cel2D.nvert; i++) {
                delete[] cel2D.centroideFace[i];
                delete[] cel2D.coordVert[i];
                delete[] cel2D.dCF[i];
                delete[] cel2D.sFace[i];
                delete[] cel2D.vecE[i];
                delete[] cel2D.vecT[i];
                delete[] cel2D.fInter[i];
                delete[] cel2D.fIfC[i];
                delete[] gradPresface[i];
                delete[] gradPresfaceCor[i];
                delete[] gradUface[i];
                delete[] gradVface[i];
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
            delete[] cel2D.gradGreenPres;
            delete[] cel2D.gradGreenPresCor;
            delete[] cel2D.gradGreenU;
            delete[] cel2D.gradGreenV;
            delete[] cel2D.gradGreenTemp;
            delete[] cel2D.gradGreenHol;

            delete[] cel2D.gradGreenPresI;
            delete[] cel2D.gradGreenPresCorI;
            delete[] cel2D.gradGreenUI;
            delete[] cel2D.gradGreenVI;
            delete[] cel2D.gradGreenTempI;
            delete[] cel2D.gradGreenHolI;
            delete[] presF;
            delete[] presFcor;
            delete[] uF;
            delete[] vF;
            delete[] holF;
            delete[] tempF;
            delete[] tUpw;
            delete[] uUpw;
            delete[] vUpw;
            delete[] holUpw;
            delete[] tMed;
            delete[] uMed;
            delete[] vMed;
            delete[] holMed;
            delete[] tDow;
            delete[] uDow;
            delete[] vDow;
            delete[] holDow;
            delete[] tK;
            delete[] uK;
            delete[] vK;
            delete[] holK;
            delete[] tL;
            delete[] uL;
            delete[] vL;
            delete[] holL;
            delete[] uRC;
            delete[] vRC;
            delete[] uRC0;
            delete[] vRC0;
            delete[] massF;
            delete[] taxadeformFace;
            delete[] viscFace;
            delete[] gradPresface;
            delete[] gradPresfaceCor;
            delete[] gradUface;
            delete[] gradVface;

            delete[] coefUHRC;
            delete[] coefUHRV;
            delete[] fonteUHR;
            delete[] coefVHRC;
            delete[] coefVHRV;
            delete[] fonteVHR;
            delete[] coefTHRC;
            delete[] coefTHRV;
            delete[] fonteTHR;

            delete[] cel2D.difuPres;
            delete[] cel2D.difuPresRC;
            delete[] cel2D.vecSDif;
            for (int i = 0; i < cel2D.nvert; i++) {
                delete[] cel2D.BMedF[i];
            }
            delete[] cel2D.B2Med;
            delete[] cel2D.BMedF;
            delete[] confinado;
            delete[] dtCFL;
            delete[] CFL;
        }
    }

    /*!
     * Identify the neighboring element associated with a face.
     */
    void buscaVizinho(int **noEle, int *face, int elem, int nVert, int nEle);

    //! Determine the smallest neighboring-element index.
    void menorIndViz();

    //! Compute and store geometric details for each face.
    void faceDetalhes();

    /*!
     * Calculate face mass flow rates.
     * \param rlx Relaxation factor.
     * \param noRelax Disable relaxation when nonzero.
     */
    void vazMass(double rlx, int noRelax = 0);

    //! Calculate face velocity components.
    void calcVarFaceUV(int face);

    //! Calculate face temperature.
    void calcVarFaceTemp(int face);

    //! Calculate face pressure.
    void calcVarFacePres(int face);

    //! Calculate face pressure correction.
    void calcVarFacePresCor(int face);

    //! Calculate face holdup or volume fraction.
    void calcVarFaceHol(int face);

    //! Apply the implicit high-resolution formulation to a face.
    void impliHR(int face);

    //! Apply the explicit high-resolution formulation to a face.
    void expliHR(int face);

    //! Apply the implicit high-resolution temperature formulation to a face.
    void impliHRT(int face);

    //! Apply the explicit high-resolution temperature formulation to a face.
    void expliHRT(int face);

    //! Apply the explicit high-resolution holdup formulation to a face.
    void expliHRHol(int face);

    //! Calculate implicit high-resolution temperature coefficients.
    void impliCoefHRT(int face);

    //! Calculate explicit high-resolution temperature coefficients.
    void expliCoefHRT(int face);

    //! Calculate implicit high-resolution momentum coefficients.
    void impliCoefHR(int face);

    //! Calculate explicit high-resolution momentum coefficients.
    void expliCoefHR(int face);

    //! Identify the temperature boundary-condition type associated with a face.
    void tipoCCTemp(int i, int &diri, int &vn, int &rich, int &acoplado, int &kcc);

    //! Identify the flow boundary-condition type associated with a face.
    void tipoCC(int i, int &inl, int &out, int &wall, int &sim, int &kcc);

    //! Update temperature boundary-condition values.
    void atualizaCCTemp(int i);

    //! Update flow boundary-condition values.
    void atualizaCC(int i);

    //! Calculate body-force contributions.
    void calcForcCorp();

    //! Calculate Green-Gauss velocity gradients.
    void calcGradGreenUV();

    //! Calculate the Green-Gauss pressure gradient.
    void calcGradGreenPres();

    //! Calculate the Green-Gauss temperature gradient.
    void calcGradGreenTemp(int inicia = 0);

    //! Calculate the Green-Gauss pressure-correction gradient.
    void calcGradGreenPresCor();

    //! Calculate the Green-Gauss holdup or volume-fraction gradient.
    void calcGradGreenHol(int inicia = 0);

    //! Calculate the dot product of two vectors.
    double escalar(double *v1, double *v2, int dimV);

    //! Locate the internal index associated with a given identifier.
    int achaInd(int i);

    //! Calculate the diffusive surface contribution.
    void calSupDifu();

    //! Advance the velocity equations explicitly.
    void explicitUV();

    //! Assemble the local coupled momentum system.
    void GeraLocalUV(double rlx);

    //! Assemble the local x-momentum system.
    void GeraLocalU();

    //! Assemble the local y-momentum system.
    void GeraLocalV();

    //! Assemble the local pressure-correction system.
    void GeraLocalPcor();

    //! Assemble the local temperature system.
    void GeraLocalT(double rlx);

    //! Advance the holdup or volume-fraction equation explicitly.
    void explicitHol();

    //! Update the pressure-correction variables.
    void atualizaCor();

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

#endif /* ELEM2D_H_ */