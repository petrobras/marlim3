#ifndef _FA_HIDRATO
#define _FA_HIDRATO

#include "SisProd.h"
#include <string>
#include <tuple>
#include <vector>

/*!
 * Model hydrate formation in the production system.
 *
 * The class loads the hydrate-equilibrium curve, applies inhibitor-related
 * corrections, checks whether local pressure and temperature conditions
 * favor hydrate formation, evaluates the kinetic formation rate, and updates
 * the gas, water, and hydrate quantities using first-order Euler integration.
 *
 * The production system is referenced but not owned by this class.
 */
class FA_Hidrato {
  public:
    /*!
     * Construct the hydrate-formation model for a production system.
     *
     * \param sistema Production system evaluated by the hydrate model.
     */
    FA_Hidrato(const SProd &sistema);

    //! Destroy the hydrate-formation model.
    ~FA_Hidrato();

    /*!
     * Execute the hydrate solver for the production system.
     *
     * The method evaluates the relevant production-system cells and updates
     * the hydrate-related state variables.
     */
    void solverHidrato();

    /*!
     * Check whether the specified pressure and temperature conditions are
     * within the hydrate-formation region.
     *
     * \param P Pressure evaluated by the hydrate model.
     * \param T Temperature evaluated by the hydrate model.
     * \return true when hydrate formation conditions are detected; otherwise,
     *         false.
     */
    bool checkHidrato(double P, double T);

    /*!
     * Evaluate the Turner hydrate-formation kinetic model.
     *
     * \param P Current pressure.
     * \param T Current temperature.
     * \param P_eq Equilibrium pressure.
     * \param T_eq Equilibrium temperature.
     * \param A_s Available surface area.
     * \param V_h Hydrate volume.
     * \param V_w Water volume.
     * \param r_d Dissociation-related kinetic parameter.
     * \param r_p Formation-related kinetic parameter.
     * \param estruturaHidratos Hydrate crystal-structure identifier.
     * \param A_s_input Input surface area before effective-area correction.
     * \param A_s_eff_out Effective surface area calculated by the model.
     * \return Hydrate-formation kinetic rate.
     */
    double TurnerHidrato(double P, double T, double P_eq, double T_eq,
                         double A_s, double V_h, double V_w,
                         double r_d, double r_p, const std::string &estruturaHidratos,
                         double A_s_input, double &A_s_eff_out);

    /*!
     * Advance the gas-related hydrate variable using first-order Euler
     * integration.
     *
     * \param i Production-system cell index.
     * \param j_G Current gas-related state value.
     * \param taxaCinetica Hydrate-formation kinetic rate.
     * \param A Effective reaction area.
     * \param MM_g Gas molar mass.
     * \return Updated gas-related state value.
     */
    double Euler1ordemGas(int i, double j_G, double taxaCinetica,
                          double A, double MM_g);

    /*!
     * Advance the water-related hydrate variable using first-order Euler
     * integration.
     *
     * \param i Production-system cell index.
     * \param j_W Current water-related state value.
     * \param taxaCinetica Hydrate-formation kinetic rate.
     * \param A Effective reaction area.
     * \param eta Stoichiometric or conversion coefficient.
     * \param MM_w Water molar mass.
     * \return Updated water-related state value.
     */
    double Euler1ordemAgua(int i, double j_W, double taxaCinetica,
                           double A, double eta, double MM_w);

    /*!
     * Advance the hydrate-related variable using first-order Euler integration.
     *
     * \param j_H Current hydrate-related state value.
     * \param taxaCinetica Hydrate-formation kinetic rate.
     * \param A Effective reaction area.
     * \param eta Stoichiometric or conversion coefficient.
     * \param MM_h Hydrate molar mass.
     * \param rho_h Hydrate density.
     * \return Updated hydrate-related state value.
     */
    double Euler1ordemHidrato(double j_H, double taxaCinetica,
                              double A, double eta, double MM_h, double rho_h);

  private:
    const SProd &sistema; // Referenced production system.

    std::vector<double> temperaturaCurva;          // Base equilibrium-curve temperatures.
    std::vector<double> pressaoCurva;              // Base equilibrium-curve pressures.
    std::vector<double> temperaturaCurvaDeslocada; // Inhibitor-shifted equilibrium temperatures.
    std::vector<double> pressaoCurvaDeslocada;     // Inhibitor-shifted equilibrium pressures.

    double K_Hamm_Etanol, K_Hamm_MEG; // Hamm correlation coefficients for ethanol and MEG.
    double MM_H, MM_G, MM_W, W_Hamm;  // Hydrate, gas, and water molar masses and Hamm parameter.
    double M_Etanol, M_MEG;           // No longer used, including as input parameters.
    double coefEsteq;                 // Hydrate stoichiometric coefficient.
    double rhoH;                      // Hydrate density.
    double r_d, r_p;                  // Kinetic parameters associated with hydrate dissociation and formation.
                                      // Related historical parameters: k1_sI, k2_sI, k1_sII, and k2_sII.
    string inibidor;                  // Hydrate-inhibitor identifier.
    string estruturaHidratos;         // Hydrate crystal-structure identifier.

    /*!
     * Load a hydrate-equilibrium curve from a file.
     *
     * \param nomeArquivo Path to the hydrate-curve file.
     */
    void carregarCurvaHidrato(const std::string &nomeArquivo);

    /*!
     * Save the inhibitor-shifted hydrate-equilibrium curve.
     *
     * \param nomeArquivo Path to the output file.
     */
    void salvarCurvaDeslocada(const std::string &nomeArquivo);

    /*!
     * Perform one-dimensional interpolation.
     *
     * \param x Independent-variable value to be evaluated.
     * \param xData Independent-variable data.
     * \param yData Dependent-variable data.
     * \return Interpolated dependent-variable value.
     */
    double interpolar(double x, const std::vector<double> &xData, const std::vector<double> &yData);

    /*!
     * Generate a hydrate-equilibrium curve corrected for an inhibitor.
     *
     * \param tempBase Base equilibrium-curve temperatures.
     * \param pressBase Base equilibrium-curve pressures.
     * \param K Inhibitor-correlation coefficient.
     * \param M Inhibitor-related parameter.
     * \param w Inhibitor mass fraction or concentration parameter.
     * \return Tuple containing the corrected temperature and pressure vectors.
     */
    std::tuple<std::vector<double>, std::vector<double>> gerarCurvaComInibidor(
        const std::vector<double> &tempBase,
        const std::vector<double> &pressBase,
        double K, double M, double w);
};

#endif