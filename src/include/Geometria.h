#ifndef _GEOM
#define _GEOM
#define _USE_MATH_DEFINES // Enable mathematical constants such as M_PI.

#include <math.h>

using namespace std;

/*!
 * Store the geometric and multilayer thermal properties of a flow section.
 *
 * The class supports circular and annular flow sections. It stores the
 * hydraulic geometry, line inclination, roughness, and the thermophysical
 * properties of the surrounding material layers.
 *
 * All dynamically allocated layer-property arrays are owned by this class
 * and are copied using deep-copy semantics.
 */
class DadosGeo {
  public:
    double a;         // Primary flow-section diameter or geometric dimension in meters.
    double b;         // Secondary diameter used by annular flow sections in meters.
    double teta;      // Line inclination angle in radians.
    double area;      // Flow cross-sectional area in square meters.
    double dia;       // Circular diameter or annular hydraulic diameter in meters.
    double rug;       // Absolute roughness.
    double peri;      // Wetted perimeter in meters.
    int ncamadas;     // Number of surrounding material layers.
    int *indmat;      // Material identifier associated with each layer.
    double *cond;     // Thermal conductivity of each layer.
    double *diamC;    // External diameter associated with each material layer.
    double *espessuR; // Radial thickness of each material layer.
    double *cp;       // Specific heat capacity of each material layer.
    double *rhoC;     // Density of each material layer.
    double *visc;     // Dynamic viscosity of each material layer.
    double *beta;     // Thermal-expansion coefficient of each material layer.
    int *tipomat;     // Material-type identifier for each layer.
    int revest;       // Flow-section type: circular when zero, annular otherwise.
    int indD0;        // Index offset associated with inserted internal layers.

    /*!
     * Construct the flow-section geometry and copy the material-layer data.
     *
     * \param va Primary flow-section diameter or dimension.
     * \param vb Secondary diameter used for annular sections.
     * \param vang Line inclination angle in radians.
     * \param vrug Absolute roughness.
     * \param vrevest Flow-section type selector.
     * \param vn Number of material layers.
     * \param vcond Thermal conductivity of each layer.
     * \param vdiamC External diameter of each layer.
     * \param vcp Specific heat capacity of each layer.
     * \param vrhoc Density of each layer.
     * \param vvisc Dynamic viscosity of each layer.
     * \param vbeta Thermal-expansion coefficient of each layer.
     * \param vtipomat Material type of each layer.
     * \param vindmat Material identifier of each layer.
     */
    DadosGeo(double va = 0, double vb = 0, double vang = 0, double vrug = 0., int vrevest = 0, int vn = 0,
             double *vcond = 0, double *vdiamC = 0, double *vcp = 0,
             double *vrhoc = 0, double *vvisc = 0, double *vbeta = 0, int *vtipomat = 0, int *vindmat = 0) {
        a = va;
        b = vb;
        dia = a;
        teta = vang;
        rug = vrug;
        indD0 = 0;
        double MPI = 3.14159265359;

        if (vn > 0) {
            ncamadas = vn;
            cond = new double[ncamadas];
            diamC = new double[ncamadas];
            espessuR = new double[ncamadas];
            cp = new double[ncamadas];
            rhoC = new double[ncamadas];
            visc = new double[ncamadas];
            beta = new double[ncamadas];
            tipomat = new int[ncamadas];
            indmat = new int[ncamadas];
            for (int i = 0; i < ncamadas; i++) {
                cond[i] = vcond[i];
                diamC[i] = vdiamC[i];
                if (i > 0)
                    espessuR[i] = (diamC[i] - diamC[i - 1]) / 2.;
                else
                    espessuR[i] = (diamC[i] - a) / 2.;
                cp[i] = vcp[i];
                rhoC[i] = vrhoc[i];
                visc[i] = vvisc[i] / 1000.;
                beta[i] = vbeta[i];
                tipomat[i] = vtipomat[i];
                indmat[i] = vindmat[i];
            }
        } else {
            ncamadas = 0;
            cond = 0;
            diamC = 0;
            espessuR = 0;
            cp = 0;
            rhoC = 0;
            visc = 0;
            beta = 0;
            tipomat = 0;
            indmat = 0;
        }
        revest = vrevest;
        if (revest == 0) {
            area = MPI * a * a / 4.;
            peri = MPI * a;
        } else {
            area = MPI * (a * a - b * b) / 4.;
            peri = MPI * (a + b);
            dia = 4. * area / peri;
        }
    }

    //! Deep-copy constructor.
    DadosGeo(const DadosGeo &OutraGeo) {
        a = OutraGeo.a;
        b = OutraGeo.b;
        dia = OutraGeo.dia;
        teta = OutraGeo.teta;

        area = OutraGeo.area;
        rug = OutraGeo.rug;
        peri = OutraGeo.peri;
        revest = OutraGeo.revest;

        indD0 = OutraGeo.indD0;

        if (OutraGeo.ncamadas > 0) {
            ncamadas = OutraGeo.ncamadas;
            cond = new double[ncamadas];
            diamC = new double[ncamadas];
            espessuR = new double[ncamadas];
            cp = new double[ncamadas];
            rhoC = new double[ncamadas];
            visc = new double[ncamadas];
            beta = new double[ncamadas];
            tipomat = new int[ncamadas];
            indmat = new int[ncamadas];
            for (int i = 0; i < ncamadas; i++) {
                cond[i] = OutraGeo.cond[i];
                diamC[i] = OutraGeo.diamC[i];
                espessuR[i] = OutraGeo.espessuR[i];
                cp[i] = OutraGeo.cp[i];
                rhoC[i] = OutraGeo.rhoC[i];
                visc[i] = OutraGeo.visc[i];
                beta[i] = OutraGeo.beta[i];
                tipomat[i] = OutraGeo.tipomat[i];
                indmat[i] = OutraGeo.indmat[i];
            }
        } else {
            ncamadas = 0;
            cond = 0;
            diamC = 0;
            espessuR = 0;
            cp = 0;
            rhoC = 0;
            visc = 0;
            beta = 0;
            tipomat = 0;
            indmat = 0;
        }
    }

    //! Deep-copy assignment operator.
    DadosGeo &operator=(const DadosGeo &vgeo) {
        if (ncamadas > 0) {
            if (this != &vgeo) { // Prevent self-assignment.
                if (ncamadas != vgeo.ncamadas) {
                    if (cond != 0 && cond != nullptr)
                        delete[] cond;
                    if (diamC != 0 && diamC != nullptr)
                        delete[] diamC;
                    if (espessuR != 0 && espessuR != nullptr)
                        delete[] espessuR;
                    if (cp != 0 && cp != nullptr)
                        delete[] cp;
                    if (rhoC != 0 && rhoC != nullptr)
                        delete[] rhoC;
                    if (visc != 0 && visc != nullptr)
                        delete[] visc;
                    if (beta != 0 && beta != nullptr)
                        delete[] beta;
                    if (tipomat != 0 && tipomat != nullptr)
                        delete[] tipomat;
                    if (indmat != 0 && indmat != nullptr)
                        delete[] indmat;

                    ncamadas = vgeo.ncamadas;
                    cond = new double[ncamadas];
                    diamC = new double[ncamadas];
                    espessuR = new double[ncamadas];
                    cp = new double[ncamadas];
                    rhoC = new double[ncamadas];
                    visc = new double[ncamadas];
                    beta = new double[ncamadas];
                    tipomat = new int[ncamadas];
                    indmat = new int[ncamadas];
                }
                a = vgeo.a;
                b = vgeo.b;
                dia = vgeo.dia;
                teta = vgeo.teta;

                area = vgeo.area;
                rug = vgeo.rug;
                peri = vgeo.peri;
                revest = vgeo.revest;

                indD0 = vgeo.indD0;

                for (int i = 0; i < ncamadas; i++) {
                    cond[i] = vgeo.cond[i];
                    diamC[i] = vgeo.diamC[i];
                    espessuR[i] = vgeo.espessuR[i];
                    cp[i] = vgeo.cp[i];
                    rhoC[i] = vgeo.rhoC[i];
                    visc[i] = vgeo.visc[i];
                    beta[i] = vgeo.beta[i];
                    tipomat[i] = vgeo.tipomat[i];
                    indmat[i] = vgeo.indmat[i];
                }
            }
        } else {
            a = vgeo.a;
            b = vgeo.b;
            dia = vgeo.dia;
            teta = vgeo.teta;

            area = vgeo.area;
            rug = vgeo.rug;
            peri = vgeo.peri;
            revest = vgeo.revest;
            indD0 = vgeo.indD0;

            if (vgeo.ncamadas > 0) {
                ncamadas = vgeo.ncamadas;
                cond = new double[ncamadas];
                diamC = new double[ncamadas];
                espessuR = new double[ncamadas];
                cp = new double[ncamadas];
                rhoC = new double[ncamadas];
                visc = new double[ncamadas];
                beta = new double[ncamadas];
                tipomat = new int[ncamadas];
                indmat = new int[ncamadas];
                for (int i = 0; i < ncamadas; i++) {
                    cond[i] = vgeo.cond[i];
                    diamC[i] = vgeo.diamC[i];
                    espessuR[i] = vgeo.espessuR[i];
                    cp[i] = vgeo.cp[i];
                    rhoC[i] = vgeo.rhoC[i];
                    visc[i] = vgeo.visc[i];
                    beta[i] = vgeo.beta[i];
                    tipomat[i] = vgeo.tipomat[i];
                    indmat[i] = vgeo.indmat[i];
                }
            }
        }
        return *this;
    }

    //! Release all dynamically allocated material-layer arrays.
    ~DadosGeo() {

        if (cond != 0 && cond != nullptr)
            delete[] cond;
        if (diamC != 0 && diamC != nullptr)
            delete[] diamC;
        if (espessuR != 0 && espessuR != nullptr)
            delete[] espessuR;
        if (cp != 0 && cp != nullptr)
            delete[] cp;
        if (rhoC != 0 && rhoC != nullptr)
            delete[] rhoC;
        if (visc != 0 && visc != nullptr)
            delete[] visc;
        if (beta != 0 && beta != nullptr)
            delete[] beta;
        if (tipomat != 0 && tipomat != nullptr)
            delete[] tipomat;
        if (indmat != 0 && indmat != nullptr)
            delete[] indmat;
    }

    /*!
     * Recalculate the external diameter of every material layer from the
     * current flow-section diameter and radial layer thicknesses.
     */
    void renovaD() {
        for (int i = 0; i < ncamadas; i++) {
            if (i > 0)
                diamC[i] = diamC[i - 1] + 2. * espessuR[i];
            else
                diamC[i] = a + 2. * espessuR[i];
        }
    }

    /*!
     * Insert a new innermost solid layer and update the flow geometry.
     *
     * Existing layer data are shifted outward by one position. The flow
     * diameter is reduced by twice the inserted layer thickness.
     *
     * \param espessura Thickness of the inserted layer.
     * \param rugosidade Updated absolute roughness.
     * \param cpW Specific heat capacity of the inserted layer.
     * \param kW Thermal conductivity of the inserted layer.
     * \param rhoW Density of the inserted layer.
     */
    void atualizaCamada(double espessura, double rugosidade, double cpW, double kW, double rhoW) {
        DadosGeo vgeo = *this;

        if (cond != 0 && cond != nullptr)
            delete[] cond;
        if (diamC != 0 && diamC != nullptr)
            delete[] diamC;
        if (espessuR != 0 && espessuR != nullptr)
            delete[] espessuR;
        if (cp != 0 && cp != nullptr)
            delete[] cp;
        if (rhoC != 0 && rhoC != nullptr)
            delete[] rhoC;
        if (visc != 0 && visc != nullptr)
            delete[] visc;
        if (beta != 0 && beta != nullptr)
            delete[] beta;
        if (tipomat != 0 && tipomat != nullptr)
            delete[] tipomat;
        if (indmat != 0 && indmat != nullptr)
            delete[] indmat;

        rug = rugosidade;

        ncamadas++;
        indD0++;
        cond = new double[ncamadas];
        diamC = new double[ncamadas];
        espessuR = new double[ncamadas];
        cp = new double[ncamadas];
        rhoC = new double[ncamadas];
        visc = new double[ncamadas];
        beta = new double[ncamadas];
        tipomat = new int[ncamadas];
        indmat = new int[ncamadas];

        for (int i = 0; i < ncamadas - 1; i++) {
            cond[i + 1] = vgeo.cond[i];
            diamC[i + 1] = vgeo.diamC[i];
            espessuR[i + 1] = vgeo.espessuR[i];
            cp[i + 1] = vgeo.cp[i];
            rhoC[i + 1] = vgeo.rhoC[i];
            visc[i + 1] = vgeo.visc[i];
            beta[i + 1] = vgeo.beta[i];
            tipomat[i + 1] = vgeo.tipomat[i];
            indmat[i + 1] = vgeo.indmat[i];
        }
        double MPI = 3.14159265359;
        cond[0] = kW;
        diamC[0] = a;
        espessuR[0] = espessura;
        cp[0] = cpW;
        rhoC[0] = rhoW;
        visc[0] = 0;
        beta[0] = 0;
        tipomat[0] = 0;
        indmat[0] = -1;
        dia = a - 2. * espessura;
        a = dia;
        if (revest == 0) {
            area = MPI * a * a / 4.;
            peri = MPI * a;
        } else {
            area = MPI * (a * a - b * b) / 4.;
            peri = MPI * (a + b);
            dia = 4. * area / peri;
        }
    }

    /*!
     * Increase the thickness of the innermost layer and update the flow
     * geometry and layer properties.
     *
     * \param espessura Thickness increment applied to the innermost layer.
     * \param cpW Updated specific heat capacity.
     * \param kW Updated thermal conductivity.
     * \param rhoW Updated density.
     */
    void atualizaCamada2(double espessura, double cpW, double kW, double rhoW) {
        espessuR[0] += espessura;
        dia = a - 2 * espessura;
        a = dia;
        double MPI = 3.14159265359;
        if (revest == 0) {
            area = MPI * a * a / 4.;
            peri = MPI * a;
        } else {
            area = MPI * (a * a - b * b) / 4.;
            peri = MPI * (a + b);
            dia = 4. * area / peri;
        }
        cond[0] = kW;
        cp[0] = cpW;
        rhoC[0] = rhoW;
    }

    /*!
     * Set the absolute thickness of the innermost layer and update the flow
     * geometry using the difference from its previous thickness.
     *
     * \param espessura New absolute thickness of the innermost layer.
     * \param cpW Updated specific heat capacity.
     * \param kW Updated thermal conductivity.
     * \param rhoW Updated density.
     */
    void atualizaCamada3(double espessura, double cpW, double kW, double rhoW) {
        double delta = espessura - espessuR[0];
        espessuR[0] = espessura;
        dia = a - 2 * delta;
        a = dia;
        double MPI = 3.14159265359;
        if (revest == 0) {
            area = MPI * a * a / 4.;
            peri = MPI * a;
        } else {
            area = MPI * (a * a - b * b) / 4.;
            peri = MPI * (a + b);
            dia = 4. * area / peri;
        }
        cond[0] = kW;
        cp[0] = cpW;
        rhoC[0] = rhoW;
    }
};

#endif