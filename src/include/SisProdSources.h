#ifndef SISPRODSOURCES_H_
#define SISPRODSOURCES_H_

#include <vector>

// Declared, not included: SourceState holds only references to these, so this
// header stays free of the cell, input-deck and globals headers and can still be
// compiled on its own.
class Cel;
class Ler;
struct SimulationContext;

namespace sisprod::sources {

/// What the source terms of a cell read and write: the mass each accessory
/// delivers to the cell in a step (fontemassLR, fontemassCR, fontemassGR).
///
/// Scalars are held by reference, not by value: a copy would read every one at
/// construction, before the branch that decides whether it is read at all.
struct SourceState {
    /// Production cells -- SProd::celula. Their source terms are written.
    Cel *const &cells;
    /// Input deck -- SProd::arq. Read only.
    const Ler &input;
    /// Shared 1D globals -- SProd::simulationContext. Read only.
    const SimulationContext *const &globals;
    /// Index of the last production cell -- SProd::ncel.
    const int &lastCell;
    /// Steady-state mode -- SProd::modoPerm. In it the porous media advance
    /// pseudo-transiently and the master valve adds no flow.
    const bool &steadyMode;
    /// Secondary branch index in a parallel network -- SProd::networkCoupling.parallelSecondaryBranch.
    const int &parallelSecondaryBranch;
    /// Boundary-condition type of the secondary parallel-network branch --
    /// SProd::networkCoupling.parallelSecondaryBoundaryCondition.
    const int &parallelSecondaryBoundaryCondition;
    /// Leak-source cells whose flows are recorded for the first iteration of a
    /// parallel network, and those flows -- SProd::networkCoupling.parallelSourceCells,
    /// parallelSourceProductionLiquid, parallelSourceComplementaryLiquid and parallelSourceGas.
    const std::vector<int> &parallelSourceCells;
    const std::vector<double> &parallelSourceProductionLiquid;
    const std::vector<double> &parallelSourceComplementaryLiquid;
    const std::vector<double> &parallelSourceGas;
};

/// Calculates the flow through Master1 while it operates as a choke, and adds
/// it to the source terms of cell cellIndex (leaving) and of the next cell
/// (arriving).
void addMasterValveFlow(const SourceState &state, int cellIndex);

/// Evaluates each recirculation connection once from the current endpoint states,
/// before the cell source sweep. Both endpoints then use the same phase rates;
/// local hydrate consumption is applied independently in renewSourceTerms.
void prepareRecirculationSourceRates(const SourceState &state);

/// Renews the source terms of cell cellIndex: what its accessory delivers in
/// the step (gas or liquid injection, IPR, master valve, volumetric pump, leak,
/// multiple source, porous medium), less what hydrate formation consumed.
/// Recirculation rates are prepared once for the sweep before calling this method.
void renewSourceTerms(const SourceState &state, int cellIndex);

}  // namespace sisprod::sources

#endif  // SISPRODSOURCES_H_
