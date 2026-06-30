/*
 * elem2dPoisson.h
 *
 * Created on: June 22, 2022
 * Author: Eduardo
 */

#ifndef elem2dPoissonPOISSON_H_
#define elem2dPoissonPOISSON_H_
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "Matriz.h"
#include "Vetor.h"
#include "estruturasPoisson.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

/*!
 * Store the geometry, temperature field, gradients, and thermal properties
 * associated with a two-dimensional Poisson finite-volume element.
 */
struct elementoPoisson {
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
    double *fatG;           // Ratio between the face-intersection-to-centroid distance
                            // and the distance between two neighboring element centroids.
    double *angES;          // Angle between the centroid-connecting line and the face-area vector.
    double **fInter;        // Coordinates where the centroid-connecting line intersects each face.
    double **fIfC;          // Vectors from each intersection point to the corresponding face centroid.

    double tempC;        // Cell-centered temperature.
    double tempCI;       // Temperature stored for the previous or intermediate iteration.
    double tempC0;       // Temperature at the previous time level.
    double *gradGreenT;  // Green-Gauss temperature gradient.
    double *gradGreenTI; // Temperature gradient stored for the previous or intermediate iteration.
    double cond;         // Thermal conductivity.
    double cp;           // Specific heat capacity.
    double rho;          // Density.
};

/*!
 * Represent and assemble a two-dimensional finite-volume element for the
 * Poisson heat-transfer model.
 *
 * The class manages element geometry, neighboring elements, face temperatures,
 * thermal boundary conditions, Green-Gauss gradients, high-resolution
 * coefficients, and the local linear system.
 */
class elem2dPoisson {
  public:
    detCCPoisson CC;           // Boundary-condition data.
    elementoPoisson cel2D;     // Data associated with the current element.
    int nvizinho;              // Number of neighboring elements.
    elementoPoisson **vizinho; // Pointers to neighboring elements.
    int *kvizinho;             // Local face or neighbor-index mapping.
    varGlob1D *vg1dSP;         // Pointer to shared one-dimensional global variables.

    /*!
     * Construct and initialize a two-dimensional Poisson element.
     */
    elem2dPoisson(varGlob1D *Vvg1dSP = 0, double **xcoor = 0, int **noEle = 0, int *tipo = 0, double *atributo = 0, int nVert = 0, int nele = 0, int nno = 0,
                  int vperm = 1, int vtrans = 0, int i = 0, double vtemp = 0., double vfluxCal = 0., double vcond = 0., double vcp = 0.,
                  double vrho = 0., double vdt = 0.);

    //! Copy constructor.
    elem2dPoisson(const elem2dPoisson &);

    //! Copy-assignment operator.
    elem2dPoisson &operator=(const elem2dPoisson &);

    double deriFonteT;  // Derivative of the temperature source term.
    double FonteT;      // Temperature source term.
    double *tempF;      // Face temperatures.
    double fluxCal;     // Heat flux.
    double dt;          // Time-step increment.
    double **gradTface; // Temperature gradients at the faces.
    int perm;           // Permanent or steady-state model flag.
    int trans;          // Transient-model flag.

    // Thermal boundary-condition data.
    double *ccTD;
    double *ccTVN;
    double *ccHR;
    double *ccTambR;

    // High-resolution temperature coefficients and source terms.
    double *coefTHRC;
    double *coefTHRV;
    double *fonteTHR;

    Vcr<double> TL;        // Right-hand-side vector of the local system.
    FullMtx<double> local; // Local system matrix.

    //! Release all dynamically allocated geometric, thermal, and solver data.
    ~elem2dPoisson() {
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
                delete[] gradTface[i];
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
            delete[] cel2D.gradGreenT;
            delete[] cel2D.gradGreenTI;
            delete[] tempF;
            delete[] gradTface;
            delete[] coefTHRC;
            delete[] coefTHRV;
            delete[] fonteTHR;
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

    //! Identify the thermal boundary-condition type associated with a face.
    void tipoCC(int i, int &diri, int &vn, int &rich, int &acoplado, int &kcc);

    //! Update the thermal boundary-condition values.
    void atualizaCC(int i);

    //! Calculate the Green-Gauss temperature gradient.
    void calcGradGreen();

    //! Calculate the dot product of two vectors.
    double escalar(double *v1, double *v2, int dimV);

    //! Locate the internal index associated with a given identifier.
    int achaInd(int i);

    //! Assemble the local Poisson system.
    void GeraLocal();

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

#endif /* elem2dPoissonPOISSON_H_ */