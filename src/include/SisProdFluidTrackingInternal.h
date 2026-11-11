#ifndef SISPRODFLUIDTRACKINGINTERNAL_H_
#define SISPRODFLUIDTRACKINGINTERNAL_H_

#include "SisProdFluidTracking.h"

#include <vector>

class ProFluCol;
template <typename T> class Vcr;

namespace sisprod::fluidtracking {
namespace {

/// What upwinding gives at one face of cell i: the liquid's black-oil properties
/// and the gas's density and composition, each taken from the side its phase
/// flows from.
struct BlackOilFace {
    double betI;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double bsw;
    double solutionGasRatio;
    double dgO;
    double yco2O;
    double API;
    double BSW;
    double denag;
    double viscL;
    double viscH;
    double razdgd;
    double razdgl;
    double rhogST;
    double dgG;
    double yco2G;
};

/// Cell i itself: its liquid holdup, complementary-liquid fraction and in-situ black-oil
/// properties, and the fluid properties it holds before this step (the *ini
/// members).
struct BlackOilCell {
    double liquidHoldup;
    double complementaryFraction;
    double rholST;
    double rhog;
    double rhogST;
    double solutionGasRatio;
    double razdgd;
    double razdgl;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double bsw;
    double dgini;
    double yco2ini;
    double APIini;
    double BSWini;
    double denagini;
    double viscLini;
    double viscHini;
};

/// The source of cell i, by accessory: the mass rates it brings and the
/// properties of the fluid it brings them in, which hold cell i's own values
/// when there is no source.
struct BlackOilSource {
    double dissolvedGas;
    double freeGas;
    double deadOil;
    double water;
    double API;
    double BSW;
    double denag;
    double dgO;
    double dgG;
    double yco2O;
    double yco2G;
    double viscL;
    double viscH;
    double rholST;
    double rhogST;
    double razdgl;
    double stockTankQuality;
};

/// The balances of cell i: the face fluxes (Mult*), the volumes at the end of
/// the step (vol*Fim) and the residuals (residuo*).
struct BlackOilBalance {
    double MultOe;
    double MultOd;
    double MultGe;
    double MultGd;
    double volleveFim;
    double residuo;
    double volpesFim;
    double volaguaFim;
    double MultPe;
    double MultPd;
    double residuoP;
    double MultAe;
    double MultAd;
    double residuoA;
};

/// What upwinding gives at one face of cell i in the phase molar-fraction
/// transport: the liquid's properties, the phase mass flow rates and quality, and
/// the molar weights of the mixture and of each phase.
struct PhaseFace {
    double betI;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double bsw;
    double BSW;
    double denag;
    double viscL;
    double viscH;
    double vazMasLiq;
    double vazMasGas;
    double titV;
    double pesoMol;
    double pesoMolO;
    double pesoMolG;
};

/// Cell i itself: its liquid holdup, complementary-liquid fraction and in-situ properties,
/// the properties it holds before this step (the *ini members), and its molar
/// weight.
struct PhaseCell {
    double liquidHoldup;
    double complementaryFraction;
    double solutionGasRatio;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double bsw;
    double BSWini;
    double denagini;
    double viscLini;
    double viscHini;
    double pesoMol;
};

/// Thermodynamic working values for one mixture-balance or inventory evaluation.
struct PhaseTransported {
    double betIV;
    double solutionGasRatio;
    double oilVolumeFactor;
    double waterVolumeFactor;
    double bswV;
    double rhoOVol;
    double rhoWVol;
    double titVol;
    double tempMol;
};

/// The source of cell i, by accessory: the mass rates it brings and the
/// properties of the fluid it brings them in.
struct PhaseSource {
    double dissolvedGas;
    double freeGas;
    double deadOil;
    double water;
    double BSW;
    double denag;
    double viscL;
    double viscH;
    double tit;
    double pesoMol;
};

/// The balances of cell i: the face fluxes (Mult*), the volumes at the end of
/// the step (vol*Fim) and the residuals (residuo*).
struct PhaseBalance {
    double volpesFim;
    double volaguaFim;
    double MultPe;
    double MultPd;
    double residuoP;
    double MultAe;
    double MultAd;
    double residuoA;
};

/// Updates black-oil properties from the ordered stock-tank balances.
/// Gas, oil, and water transport retain their individual model and inflow gates.
/// The published water density retains the cell's previous value.
void transportBlackOilBalances(const FluidTrackingState &state, const BlackOilFace &left,
                               const BlackOilFace &right, const BlackOilCell &cell,
                               const BlackOilSource &source, BlackOilBalance &balance, double dx,
                               double flowArea, int i, Vcr<double> &rgo, Vcr<double> &dg, Vcr<double> &yco2,
                               Vcr<double> &API, Vcr<double> &BSW, Vcr<double> &denag, Vcr<double> &VISCL,
                               Vcr<double> &VISCH, double dt);

/// Prepares a source carried by a local or upstream choke or volumetric pump.
/// The inlet and upstream paths retain their individual property expressions.
void blackOilSourceCellFluid(const FluidTrackingState &state, BlackOilSource &source, int i,
                             double temperatureHigh, double temperatureLow);

/// Prepares the 2D porous source using its transfer BSW and the selected
/// radialPoro.flup or cell fluid; selection follows the signed source mass rate.
void blackOilSourcePorous2D(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF, int i,
                            double temperatureHigh, double temperatureLow);

/// Prepares the radial porous source from its reservoir or cell fluid and BSW.
void blackOilSourceRadialPorous(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF, int i,
                                double temperatureHigh, double temperatureLow);

/// Prepares an annulus leak from the higher-pressure side using a local working
/// fluid. The ambient-gas rule controls the dissolved-gas source.
void blackOilSourceLeak(const FluidTrackingState &state, BlackOilSource &source, int i,
                        double temperatureHigh, double temperatureLow);

/// Prepares an IPR source using the reservoir fluid only above cell pressure.
void blackOilSourceInflowPerformance(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                     int i, double temperatureHigh, double temperatureLow);

/// Prepares a multiple source from the accessory fluid when its mass sum is positive.
void blackOilSourceMultipleSource(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                  int i, double temperatureHigh, double temperatureLow);

/// Prepares a liquid injection from the accessory fluid only for positive injection.
void blackOilSourceLiquidInjection(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                   int i, double temperatureHigh, double temperatureLow);

/// Prepares a wet gas injection with its selected fluid. The configured injection
/// fluid supplies the water-presence test and source water density.
void blackOilSourceWetGasInjection(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                   int i, double temperatureHigh, double temperatureLow);

/// Selects left-face standard gas density and gas composition from the gas-flow donor.
/// Zero gas flow selects the cell fluid.
void upwindLeftFaceGasProperties(const FluidTrackingState &state, BlackOilFace &left, int i);

/// Replaces the right-face black-oil liquid properties for strictly negative liquid flow.
void upwindRightFaceLiquidProperties(const FluidTrackingState &state, BlackOilFace &right, int i,
                                     double temperatureHigh, double temperatureLow);

/// Prepares left-face liquid properties with independent inlet/pig fraction rules.
/// Solution gas ratio is converted after evaluating the formation volume factors.
void upwindLeftFaceBlackOilLiquid(const FluidTrackingState &state, BlackOilFace &left, int i,
                                  double temperatureHigh, double temperatureLow);

/// Prepares face and source properties, transports black-oil properties,
/// and publishes this cell's three stock-tank volumes.
void transportCellBlackOilProperties(const FluidTrackingState &state, int i, Vcr<double> &rgo,
                                     Vcr<double> &dg, Vcr<double> &yco2, Vcr<double> &API, Vcr<double> &BSW,
                                     Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                     double temperatureHigh, double temperatureLow, double dt);

/// Copies a phase composition and normalizes it only when its sequential sum is positive.
/// The minimum search does not shift the composition; the subtraction uses zero.
/// npseudo is positive and the destination already contains npseudo elements.
void normalizePhaseFractions(std::vector<double> &fracMolFase, double *fracMolOriginal, int npseudo);

/// Updates liquid stock-tank volumes, BSW, and dead-oil viscosities under the
/// compositional transport gates. Published water density retains its previous value.
void transportPhaseVolumesAndViscosities(const FluidTrackingState &state, const PhaseFace &left,
                                         const PhaseFace &right, const PhaseCell &cell,
                                         const PhaseSource &source, PhaseBalance &balance, double dx,
                                         double flowArea, int i, Vcr<double> &BSW, Vcr<double> &denag,
                                         Vcr<double> &VISCL, Vcr<double> &VISCH, double dt);

/// Converts source oil/water mass rates to stock-tank volume rates and reads
/// accessory-specific liquid properties. Configured and selected fluid donors differ
/// for some accessories and retain their separate roles.
void phaseSourceStandardRatesByAccessory(const FluidTrackingState &state, PhaseSource &source, ProFlu &fluF,
                                         int i, double temperatureHigh, double temperatureLow);

/// Advances mixture molar fractions through limCorrige ordered corrections,
/// shifts negative trial fractions, normalizes, and refreshes enabled flash properties.
/// The molecular-weight update uses the original cell composition.
void solveCellPhaseMolarFractions(const FluidTrackingState &state, const PhaseFace &left,
                                  const PhaseFace &right, PhaseCell &cell, const PhaseSource &source,
                                  int limCorrige, ProFlu &fluF, int i, Vcr<double> &fracMol0,
                                  Vcr<double> &fracMol1, double dt, ProFlu *fluC, int ncomp);

/// Refreshes accessory flash state before selecting the working source fluid.
/// Both leak fluids are refreshed in pressure-side order; porous flashes use reservoir
/// temperature, while the selected liquid properties use cell temperature.
void readPhaseSourceFluid(const FluidTrackingState &state, PhaseSource &source, ProFlu &fluF, int i);

/// Prepares normalized vapour compositions and mixture compositions at both faces.
/// The mixture compositions and weights are consumed by the mixture balance;
/// phase-specific composition buffers remain separate working storage.
void upwindVapourAndMixtureFractions(const FluidTrackingState &state, PhaseFace &left, PhaseFace &right,
                                     int i, Vcr<double> &fracMol0, Vcr<double> &fracMol1,
                                     Vcr<double> &fracMol0G, Vcr<double> &fracMol1G, int ncomp);

/// Adjusts transported liquid mass for complementary fluid and water and prepares
/// normalized liquid compositions. Flow, complementary-fluid, and composition donors
/// retain their individual face and cell indices.
void upwindLiquidFractions(const FluidTrackingState &state, PhaseFace &left, PhaseFace &right, int i,
                           Vcr<double> &fracMol0O, Vcr<double> &fracMol1O, int ncomp);

/// Prepares left-face liquid volume factors, water cut, and liquid properties
/// from the liquid-flow donor, preserving the independent inlet/pig fraction rule.
void upwindLeftFacePhaseLiquidProperties(const FluidTrackingState &state, PhaseFace &left, int i,
                                         double temperatureHigh, double temperatureLow);

/// Prepares phase and mixture donors, updates source flash state, advances trial
/// mixture composition and mole inventory, then transports the liquid volume properties.
void transportCellPhaseMolarFractions(const FluidTrackingState &state, int i, Vcr<double> &BSW,
                                      Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                      Vcr<double> &fracMol0, Vcr<double> &fracMol1, Vcr<double> &fracMol0O,
                                      Vcr<double> &fracMol1O, Vcr<double> &fracMol0G, Vcr<double> &fracMol1G,
                                      double temperatureHigh, double temperatureLow, double dt, ProFlu *fluC,
                                      int ncomp);

/// Converts porous/IPR oil and water sources with the <= tiny and positive-oil-fraction rules.
/// The source water cut is a live reference to its model-specific storage.
void convertBlackOilPorousRates(const FluidTrackingState &state, BlackOilSource &source, const ProFlu &fluF,
                                const double &sourceWaterCut, double solutionGasRatioSource,
                                double temperatureHigh, double temperatureLow);

/// Converts multiple-source and liquid-injection rates with the strict water-cut threshold.
void convertBlackOilInjectionRates(const FluidTrackingState &state, BlackOilSource &source,
                                   const ProFlu &fluF, double solutionGasRatioSource, double temperatureHigh,
                                   double temperatureLow);

/// Evaluates black-oil liquid face properties in RS, volume-factor, and viscosity order.
/// The caller selects the donor and complementary fraction independently.
void prepareBlackOilLiquidFace(BlackOilFace &face, const ProFlu &fluid, const double &pressure,
                               const double &temperature, double temperatureHigh, double temperatureLow);

/// Evaluates compositional liquid-face volume factors, water cut and viscosities.
/// RS is working storage used only by the formation-volume-factor evaluation.
void preparePhaseLiquidFace(PhaseFace &face, const ProFlu &fluid, const double &pressure,
                            const double &temperature, double temperatureHigh, double temperatureLow);

/// Converts configured liquid-injection and multiple-source oil/water rates.
/// The configured fluid is used even when the working source donor is the cell.
void convertPhaseInjectionRates(const FluidTrackingState &state, PhaseSource &source, const ProFlu &fluid,
                                double solutionGasRatioSource, double temperatureHigh, double temperatureLow);

/// Converts configured IPR and porous oil/water rates with their positive-oil-fraction rule.
/// The water cut remains a live reference to the configured source or transfer storage.
void convertPhasePorousRates(const FluidTrackingState &state, PhaseSource &source, const ProFlu &fluid,
                             const double &sourceWaterCut, double solutionGasRatioSource,
                             double temperatureHigh, double temperatureLow);

/// Tests whether more than one production fluid or a network enables liquid-property transport.
bool hasMultipleFluidsOrNetwork(const FluidTrackingState &state);

/// Tests whether a production fluid or network enables the compositional source/copy path.
bool hasProductionFluidOrNetwork(const FluidTrackingState &state);

/// Tests the API/viscosity transport gate; a network enables it independently of the flash model.
bool tracksBlackOilOilProperties(const FluidTrackingState &state);

/// Applies the compositional initial-volume seed before computing enabled oil/water fluxes and residuals.
void computePhaseLiquidVolumeBalances(const FluidTrackingState &state, const PhaseFace &left,
                                      const PhaseFace &right, const PhaseSource &source,
                                      PhaseBalance &balance, double dx, double flowArea, int i, double dt);

/// Prepares black-oil faces in liquid-left, liquid-right, gas-left, gas-right order.
/// Right liquid properties are evaluated before the optional reverse-flow replacement.
void prepareBlackOilFaces(const FluidTrackingState &state, BlackOilFace &left, BlackOilFace &right, int i,
                          double temperatureHigh, double temperatureLow);

/// Reads cell thermodynamic properties and seeds the source from the current cell in order.
void prepareBlackOilCellSourceBasis(const FluidTrackingState &state, BlackOilCell &cell,
                                    BlackOilSource &source, int i, double temperatureHigh,
                                    double temperatureLow);

/// Applies accessory source selection and standard gas conversion, then the stock-tank quality rule.
/// The working fluid belongs to the caller throughout the subsequent balances.
void prepareBlackOilAccessorySource(const FluidTrackingState &state, BlackOilSource &source, ProFlu &fluF,
                                    int i, double temperatureHigh, double temperatureLow);

/// Computes gas-volume fluxes and residuals and the three ending stock-tank volumes.
/// The black-oil water-volume expression retains its in-situ water-cut factors.
void prepareBlackOilStockTankBalances(const FluidTrackingState &state, const BlackOilFace &left,
                                      const BlackOilFace &right, const BlackOilCell &cell,
                                      const BlackOilSource &source, BlackOilBalance &balance, double dx,
                                      double flowArea, int i, double dt);

/// Publishes one black-oil cell in serial order, renews its fluid, then corrects gas density.
/// The invalid-gas-density fallback reads the already published previous cell.
void publishBlackOilCellProperties(const FluidTrackingState &state, int i, Vcr<double> &rgo, Vcr<double> &dg,
                                   Vcr<double> &yco2, Vcr<double> &API, Vcr<double> &BSW, Vcr<double> &denag,
                                   Vcr<double> &VISCL, Vcr<double> &VISCH, double temperatureHigh,
                                   double temperatureLow);

/// Applies the outlet donor and enabled property rules in transport order.
/// The reverse-flow fluid is owned by the public transport caller.
void updateBlackOilOutletProperties(const FluidTrackingState &state, const ProFlu &fluiRev, Vcr<double> &rgo,
                                    Vcr<double> &dg, Vcr<double> &yco2, Vcr<double> &API, Vcr<double> &BSW,
                                    Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                    double temperatureHigh, double temperatureLow);

/// Publishes one trial compositional fluid and the enabled liquid properties in cell order.
void publishPhaseCellProperties(const FluidTrackingState &state, ProFlu *fluC, int i, Vcr<double> &BSW,
                                Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                double temperatureHigh, double temperatureLow);

/// Applies the outlet donor and enabled property rules in transport order.
/// The reverse-flow fluid is owned by the public transport caller.
void updatePhaseOutletProperties(const FluidTrackingState &state, const ProFlu &fluiRev, Vcr<double> &BSW,
                                 Vcr<double> &denag, Vcr<double> &VISCL, Vcr<double> &VISCH,
                                 double temperatureHigh, double temperatureLow);

/// Refreshes one source composition with its current valid beta/compositions or the unknown-phase seed.
/// Pressure, temperature, and the source fluid are read through their existing bindings.
void refreshPhaseSourceComposition(const FluidTrackingState &state, ProFlu &fluid, const double &pressure,
                                   const double &temperature);

/// Evaluates the selected source liquid properties at cell pressure and temperature and publishes its oil
/// mass fraction.
void readPhaseSourceLiquidQuality(const FluidTrackingState &state, PhaseSource &source, const ProFlu &fluF,
                                  int i);

/// Prepares liquid properties, original phase mass flows, and the cell mixture molecular weight.
void preparePhaseFacesAndCellWeight(const FluidTrackingState &state, PhaseFace &left, PhaseFace &right,
                                    PhaseCell &cell, int i, double temperatureHigh, double temperatureLow,
                                    int ncomp);

/// Initializes signed source masses, refreshes and selects its working fluid, and computes its molecular
/// weight.
void preparePhaseMixtureSource(const FluidTrackingState &state, PhaseSource &source, ProFlu &fluF, int i,
                               int ncomp);

/// Publishes mole inventory using trial-fluid densities and the original cell liquid-quality expression.
/// The trial fluid receives the molecular weight computed by the correction loop.
void updatePhaseMoleInventory(const FluidTrackingState &state, const PhaseCell &cell, ProFlu *fluC, int i);

/// Prepares original-cell liquid properties, converts source volume rates and publishes ending volumes.
/// This follows mixture correction and inventory evaluation; residual zero products remain significant.
void transportPhaseLiquidProperties(const FluidTrackingState &state, const PhaseFace &left,
                                    const PhaseFace &right, PhaseCell &cell, PhaseSource &source,
                                    ProFlu &fluF, int i, Vcr<double> &BSW, Vcr<double> &denag,
                                    Vcr<double> &VISCL, Vcr<double> &VISCH, double temperatureHigh,
                                    double temperatureLow, double dx, double flowArea, double dt);

/// Adjusts one liquid mass rate and prepares its phase composition with mixture fallback.
/// Liquid volume flow and complementary-fluid donor are independent of the phase-composition donor.
/// The phase vector is constructed after the property evaluations and destroyed before returning.
void prepareLiquidFaceTransport(PhaseFace &face, const ProFlu &fluid, const ProFluCol &complementaryFluid,
                                const double &pressure, const double &temperature,
                                const double &liquidVolumeFlow, const double &voidFraction,
                                Vcr<double> &fractions, int ncomp);

/// Prepares one normalized vapour composition and its molecular weight, retaining mixture fallback.
/// The phase vector remains local to the selected face evaluation.
void prepareVapourFaceComposition(PhaseFace &face, const ProFlu &fluid, const double &pressure,
                                  const double &temperature, const double &voidFraction,
                                  Vcr<double> &fractions, int ncomp);

/// Copies the selected mixture composition and accumulates its molecular weight in component order.
void copyMixtureFaceComposition(PhaseFace &face, const ProFlu &fluid, Vcr<double> &fractions, int ncomp);

} // namespace
} // namespace sisprod::fluidtracking

#endif // SISPRODFLUIDTRACKINGINTERNAL_H_
