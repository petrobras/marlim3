/*
 * elem3dPoisson.h
 *
 * Created on: June 22, 2022
 * Author: Eduardo
 */

#ifndef elem3dPoissonPOISSON_H_
#define elem3dPoissonPOISSON_H_
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include "Matriz.h"
#include "Vetor.h"
#include "estruturaUNV.h"
#include "estruturasPoisson3D.h"
#include "variaveisGlobais1D.h"
#include <algorithm>
#include <complex>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

/*!
 * Store the geometry, temperature field, gradients, and thermal properties
 * associated with a three-dimensional Poisson finite-volume element.
 */
struct elementoPoisson3D {
    int nvert;      // Number of vertices.
    int dim;        // Spatial dimension of the problem.
    int indEle;     // Element index.
    int nele;       // Total number of elements in the discretization.
    int *noElem;    // Indices of the element nodes.
    int *indFace;   // Indices of neighboring elements associated with each face.
    string *ccFace; // Boundary-condition identifiers associated with each face.
    vector<int> indVizCres; // Neighbor indices sorted in ascending order.

    double *orientFace; // Face-orientation correction factors that ensure
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

    double tempC;       // Cell-centered temperature.
    double tempCI;      // Temperature stored for the previous or intermediate iteration.
    double tempC0;      // Temperature at the previous time level.
    double *gradGreenT; // Green-Gauss temperature gradient.
    double *gradGreenTI; // Temperature gradient stored for the previous or intermediate iteration.
    double *gradGreenT0; // Temperature gradient at the previous time level.
    double cond;        // Thermal conductivity.
    double cp;          // Specific heat capacity.
    double rho;         // Density.
};

/*!
 * Represent and assemble a three-dimensional finite-volume element for the
 * Poisson heat-transfer model.
 *
 * The class manages UNV element data, neighboring elements, face temperatures,
 * thermal boundary conditions, Green-Gauss gradients, auxiliary vectors,
 * high-resolution coefficients, and the local linear system.
 */
class elem3dPoisson {
  public:
    eleOBJ eleUNV;                 // Geometric element data loaded from the UNV mesh.
    detCCPoisson3D *CC;            // Pointer to the three-dimensional boundary-condition data.
    elementoPoisson3D cel3D;       // Data associated with the current element.
    int nvizinho;                  // Number of neighboring elements.
    elementoPoisson3D **vizinho;   // Pointers to neighboring elements.
    int *kvizinho;                 // Local face or neighbor-index mapping.

    double deriFonteT; // Derivative of the temperature source term.
    double FonteT;     // Temperature source term.
    double *tempF;     // Face temperatures.
    double fluxCal;    // Heat flux.
    double dt;         // Time-step increment.
    double **gradTface; // Temperature gradients at the faces.

    // Thermal boundary-condition data.
    double *ccTD;
    double *ccTVN;
    double *ccHR;
    double *ccTambR;

    // High-resolution temperature coefficients and source terms.
    double *coefTHRC;
    double *coefTHRV;
    double *fonteTHR;

    // Auxiliary three-dimensional vectors used by geometric calculations.
    double *v1;
    double *v2;
    double *v3;

    varGlob1D *DGP3D; // Pointer to shared global data used by the three-dimensional model.

    Vcr<double> TL;        // Right-hand-side vector of the local system.
    FullMtx<double> local; // Local system matrix.

    /*!
     * Construct and initialize a three-dimensional Poisson element.
     */
    elem3dPoisson(varGlob1D *DGP3DV = 0, eleOBJ velementoUNV = eleOBJ(), double **xcoor = 0, int **noEle = 0, int *tipo = 0,
                  double *atributo = 0, int nVert = 0, int nele = 0, int nno = 0,
                  int i = 0, double vtemp = 0., double vfluxCal = 0., double vcond = 0., double vcp = 0.,
                  double vrho = 0., double vdt = 0.);

    //! Copy constructor.
    elem3dPoisson(const elem3dPoisson &);

    //! Copy-assignment operator.
    elem3dPoisson &operator=(const elem3dPoisson &);

    //! Release all dynamically allocated geometric, thermal, auxiliary, and solver data.
    ~elem3dPoisson() {
        if (cel3D.nvert > 0) {
            delete[] cel3D.noElem;
            delete[] cel3D.indFace;
            delete[] cel3D.ccFace;
            delete[] cel3D.orientFace;
            delete[] cel3D.centroElem;
            delete[] cel3D.centroideElem;
            delete[] cel3D.dCFMod;
            delete[] cel3D.sFaceMod;
            delete[] cel3D.ownFace;
            delete[] ccTD;
            delete[] ccTVN;
            delete[] ccHR;
            delete[] ccTambR;
            for (int i = 0; i < cel3D.nvert; i++) {
                delete[] cel3D.centroideFace[i];
                delete[] cel3D.coordVert[i];
                delete[] cel3D.dCF[i];
                delete[] cel3D.sFace[i];
                delete[] cel3D.vecE[i];
                delete[] cel3D.vecT[i];
                delete[] cel3D.fInter[i];
                delete[] cel3D.fIfC[i];
                delete[] gradTface[i];
            }
            delete[] cel3D.centroideFace;
            delete[] cel3D.coordVert;
            delete[] cel3D.dCF;
            delete[] cel3D.sFace;
            delete[] vizinho;
            delete[] kvizinho;
            delete[] cel3D.vecE;
            delete[] cel3D.modE;
            delete[] cel3D.vecT;
            delete[] cel3D.modT;
            delete[] cel3D.fatG;
            delete[] cel3D.angES;
            delete[] cel3D.fInter;
            delete[] cel3D.fIfC;
            delete[] cel3D.gradGreenT;
            delete[] cel3D.gradGreenTI;
            delete[] cel3D.gradGreenT0;
            delete[] tempF;
            delete[] gradTface;

            delete[] coefTHRC;
            delete[] coefTHRV;
            delete[] fonteTHR;

            delete[] v1;
            delete[] v2;
            delete[] v3;
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

    //! Assemble the local three-dimensional Poisson system.
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

#endif /* elem3dPoissonPOISSON_H_ */