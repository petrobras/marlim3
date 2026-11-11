#ifndef SISPRODFLUIDTRACKING_H_
#define SISPRODFLUIDTRACKING_H_

// The state holds references; its builders include the referenced definitions.
class Cel;
class Ler;
class ProFlu;
struct SimulationContext;
class SProd;

namespace sisprod::fluidtracking {

/// State shared by black-oil property tracking and phase molar-fraction transport.
///
/// Each transport reads its time step from cells[1]; lastCell is the outlet index.
///
/// Scalars are held by reference so each transport reads them only when its
/// selected fluid model and tracking options require them.
struct FluidTrackingState {
    Cel *&cells;                          // celula
    int &lastCell;                        // ncel
    Ler &input;                           // arq
    SimulationContext *&globals;                  // simulationContext
    int &endNode;                         // noextremo
    double &inletPressure;                // presE
    double &inletTemperature;             // tempE
    double &inletComplementaryFraction;   // betaE
    int &trackGasOilRatio;                // trackRGO
    int &trackGasGravity;                 // trackDeng
    int &compositionalRefreshCounter;     // kontaRenovaComp

    /// Iteration indicator used to choose the density-correction reference cell.
    const int &steadyIteration;
};

/// Updates cell and accessory gas-density corrections. The iteration indicator
/// selects cell i or i - 1 only when density correction is enabled.
void correctGasSpecificGravity(Cel *const &cells, const Ler &input,
                               const int &steadyIteration, int i);

/// Transports gas-oil ratio, gas specific gravity, CO2 fraction, API gravity,
/// water content and density, and dead-oil viscosities as enabled by the fluid
/// model and tracking options. fluiRev is the fluid entering through the outlet
/// under reverse flow, taken by value.
void transportBlackOilProperties(const FluidTrackingState &state, ProFlu fluiRev);

/// Advances mixture molar fractions using upwind mixture compositions and
/// liquid mass rates adjusted for water and complementary fluid. Transports
/// water content and density and dead-oil viscosities under their model gates.
/// Refreshes enabled flash properties. fluiRev is the outlet reverse-flow
/// fluid, taken by value.
void transportPhaseMolarFractions(const FluidTrackingState &state, ProFlu fluiRev);

}  // namespace sisprod::fluidtracking

#endif  // SISPRODFLUIDTRACKING_H_
