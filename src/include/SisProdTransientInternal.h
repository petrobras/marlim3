#ifndef SISPRODTRANSIENTINTERNAL_H_
#define SISPRODTRANSIENTINTERNAL_H_

#include "SisProdTransient.h"
#include "Leitura.h"

#include <concepts>
#include <ctime>
#include <iosfwd>
#include <utility>

namespace sisprod::transient {
namespace {

/// Updates every configured valve box from the adjacent production cells.
void updateValveBoxStates(const TransientStepState &state);

/// Writes liquid inventories on both sides of Master1, at local and standard conditions.
void writeLiquidInventory(const TransientStepState &state, std::ostream &output);


/// Liquid-mixture and gas densities evaluated at one upwind face.
struct FacePhaseDensities {
    double liquid;
    double gas;
};

/// Ordered application points of the single-line time-step policies.
enum class TimeStepHook {
    CouplingIterationStart, // start of a pressure-volume coupling iteration
    AfterPigUpdate,         // after the pig moves, in the same iteration
    AfterValveOpenings,     // after the step re-reads the valve openings
};

/// Live step bindings, coupling iteration, and explicit-scheme selector.
struct TimeStepPolicyContext {
    const TransientStepState &step;
    int couplingIteration;
    int explicitScheme;
};

constexpr int kOutsideCouplingLoop = -1;

/// Compile-time interface required of each time-step policy.
template <typename Policy>
concept TimeStepPolicy = requires(const TimeStepPolicyContext &context) {
    { Policy::hook } -> std::convertible_to<TimeStepHook>;
    { Policy::applies(context) } -> std::same_as<bool>;
    { Policy::apply(context) } -> std::same_as<void>;
};

/// Valve restriction at the first coupling iteration when enabled.
struct RestrictTimeStepByValvePolicy {
    static constexpr TimeStepHook hook = TimeStepHook::CouplingIterationStart;
    /// Tests whether this policy acts at the current step stage.
    static bool applies(const TimeStepPolicyContext &context) {
        return context.couplingIteration == 0 && context.step.input.controleDTvalv == 1;
    }
    /// Executes the policy action using the live step bindings.
    static void apply(const TimeStepPolicyContext &context) {
        restrictTimeStepByValve(context.step);
    }
};

/// Maximum-step damping at the coupling iteration selected by the model.
struct DampMaximumTimeStepPolicy {
    static constexpr TimeStepHook hook = TimeStepHook::AfterPigUpdate;
    /// Tests whether this policy acts at the current step stage.
    static bool applies(const TimeStepPolicyContext &context) {
        return context.couplingIteration == 1 * context.step.fullModel;
    }
    /// Executes the policy action using the live step bindings.
    static void apply(const TimeStepPolicyContext &context) {
        dampMaximumTimeStep(context.step);
    }
};

/// Pressure and temperature rate assessment after valve openings are read.
struct PressureRateOfChangePolicy {
    static constexpr TimeStepHook hook = TimeStepHook::AfterValveOpenings;
    /// Tests whether this policy acts at the current step stage.
    static bool applies(const TimeStepPolicyContext &context) {
        return context.step.fullModel == 1;
    }
    /// Executes the policy action using the live step bindings.
    static void apply(const TimeStepPolicyContext &context) {
        evaluatePressureRateOfChange(context.step, context.explicitScheme);
    }
};

/// Applies policies of the requested hook in their declared order.
template <TimeStepPolicy... Policies>
struct TimeStepPolicyRegistry {
    /// Executes matching policies without indirect dispatch.
    template <TimeStepHook Hook>
    static void apply(const TimeStepPolicyContext &context) {
        (applyIfAt<Policies, Hook>(context), ...);
    }

  private:
    /// Tests and executes a policy only at its own hook.
    template <typename Policy, TimeStepHook Hook>
    static void applyIfAt(const TimeStepPolicyContext &context) {
        if constexpr (Policy::hook == Hook) {
            if (Policy::applies(context))
                Policy::apply(context);
        }
    }
};

/// Policies applied by the single-line transient step.
using TimeStepPolicies = TimeStepPolicyRegistry<
    RestrictTimeStepByValvePolicy,
    DampMaximumTimeStepPolicy,
    PressureRateOfChangePolicy>;

/// Selects the collector fluid or the shared default fluid for reverse flow.
const ProFlu &reverseFlowFluid(const ProFlu *collectorFluid);

/// Returns the absolute gas-to-total mass-flow ratio at the outlet.
double flowQuality(double gasMassFlow, double totalMassFlow);

/// Computes the in-situ gas mass fraction from void fraction and mixture density.
double inSituGasMassFraction(const Cel &cell, double voidFraction, double pressure, double mixtureDensity);

/// Records accepted artificial gas with the final step length and clears pending additions.
void reportAcceptedArtificialGas(const TransientStepState &state);

/// Advances one ordered fraction, porous-flow, pressure-volume, and thermal coupling iteration.
void advanceCouplingIteration(const TransientSolveState &state, const TransientDependencies &dependencies, int kontaAcop, int celpos, int vExpli, int ciclomax, double titRev, double alfRev, double betRev);

/// Emits scheduled line, wall, and unit-cell profiles and advances their schedule indices.
void writeProfiles(const TransientSolveState &state, int nrede);

/// Samples and flushes the four trend channels using their individual initial-time rules.
void writeTrends(const TransientSolveState &state, const TransientDependencies &dependencies, int ordemImpT, double velmaxdesc, int nrede);

/// Writes selected screen variables and the elapsed duration of the step calculation.
void writeScreenOutput(const TransientSolveState &state, const chrono::steady_clock::time_point &begin, const chrono::steady_clock::time_point &end);

/// Appends due events using the input event sentinel and advances the event index.
void writeEventLog(const TransientSolveState &state, int maxEvento);

/// Writes progress, monitored values, and final-run information when a report is due.
void writeProgressReport(const TransientSolveState &state, int MaxKontaImpres);

/// Fills the minimum-pressure corners in temperature order, including bubble pressures.
void fillMiniTableCornersAtMinPressure(const TransientStepState &state, ProFlu &fluC, ProFlu &flui);

/// Fills the maximum-pressure corners in temperature order.
void fillMiniTableCornersAtMaxPressure(const TransientStepState &state, ProFlu &fluC, ProFlu &flui);

/// Propagates the current time step to the four cell increments in index order.
void assignCellTimeSteps(const TransientStepState &state);

/// Tests the scheduled instant with short-circuit reads of the time array and index.
bool profileIsDue(const TransientStepState &state, double *const &times, const int &index);

/// Advances a profile index, retaining the final schedule entry.
void advanceProfileIndex(int &index, const int &limit);

/// Appends the branch-qualified log path to the caller-owned stream.
void appendLogPath(std::ostringstream &saidaT, const TransientSolveState &state);

/// Writes the supplied local timestamp and its terminating flush.
void writeLogTimestamp(std::ostream &escreveIni, const std::tm *ltm);

/// Appends elapsed-run and closing information when the final time is reached.
void writeRunCompletionReport(const TransientSolveState &state, std::ostream &escreveIni);

/// Tests progress, report-count, and final-time triggers at the current clock value.
bool progressReportIsDue(const TransientSolveState &state, int MaxKontaImpres);

/// Updates the interior auxiliary pressure and returns its upstream flow area.
double updateInteriorAuxiliaryPressure(const TransientStepState &state, int i);

/// Updates interior volumetric flows and snapshots with the upstream face area.
void updateInteriorVolumeFlows(const TransientStepState &state, int i, double area);

/// Updates the outlet auxiliary pressure using the outlet-specific velocity estimate.
/// Honors the selected arithmetic-average or friction/hydrostatic reconstruction.
/// Zero mixture velocity has zero friction pressure loss.
void updateOutletAuxiliaryPressure(const TransientStepState &state, int i);

/// Updates outlet volumetric flows using the outlet-specific donor fraction.
void updateOutletVolumeFlows(const TransientStepState &state, int i);

/// Evaluates liquid-mixture density before gas density using the caller-owned donor copy.
FacePhaseDensities facePhaseDensitiesOf(const TransientStepState &state, int i,
                                             const ProFlu &flud, double tmed, double betI);

/// Applies choke startup, opening events, and the current CFL safety-factor rule.
void applyChokeTimeStepControls(const TransientStepState &state);

/// Limits the current step by production and gas-line transport in their sweep order.
void restrictTimeStepByTransport(const TransientStepState &state);

/// Aligns the limited step to events and updates the master-valve schedule state.
void applyScheduledTimeStepEvents(const TransientStepState &state);


/// Detects the current three-cell oscillation pattern behind closed chokes and penalizes the step.
void penalizeOscillatingFractions(const TransientStepState &state);

/// Applies one valve closing restriction and its artificial-gas effects using the shared pre-sweep limit.
double valveRestrictedTimeStep(const TransientStepState &state, int i, double dtaux);

/// Applies the outlet void-fraction rule inside the void-fraction sweep.
void updateOutletVoidFraction(const TransientStepState &state, int i, double alfrev);

/// Applies the outlet complementary-liquid rule inside its own fraction sweep.
void updateOutletComplementaryFraction(const TransientStepState &state, int i);

/// Prepares inlet and neighboring complementary-liquid fractions after serial pig transfers.
void preparePigFaceFractions(const TransientStepState &state);

/// Selects the current-state choke gas fraction with its current-flow and in-situ fallbacks.
double currentOutletGasMassFraction(const TransientStepState &state, double alfSup,
                                    double massgas, double masentrada, double romix);

/// Tests the current-state liquid-dominated pressure-drop criterion without changing its thresholds.
bool currentOutletUsesPressureDrop(const TransientStepState &state, double chokeGasMassFraction,
                                   double delp, double masentrada, double maxSup);

/// Tests the current-pressure and moving-mean conditions for retaining an open outlet.
bool currentOutletCanRemainOpen(const TransientStepState &state, double delp);

/// Tests the buffered liquid-dominated pressure-drop criterion with its own absolute difference.
bool bufferedOutletUsesPressureDrop(const TransientStepState &state, double chokeGasMassFraction,
                                    double delp, double masentrada, double maxSup);

/// Computes buffered network sources from buffered liquid flow and the selected reverse liquid fraction.
void updateBufferedNetworkSources(const TransientStepState &state, double betRev);

/// Sets the current model and stabilization flags before fraction evolution.
void prepareCouplingModelFlags(const TransientSolveState &state, int celpos);

/// Restarts porous and fraction evolution at the reduced step, then applies porous corrections.
void restartCouplingFractions(const TransientSolveState &state, const TransientDependencies &dependencies, int kontaAcop,
                              double alfRev, double betRev);

/// Restores intermediate cell, porous, outlet-source and choke state between coupling iterations.
void restoreIntermediateCouplingState(const TransientSolveState &state, int kontaAcop,
                                      double gasMassSource, double liquidMassSource,
                                      double complementaryMassSource);

/// Resets critical ratios, captures updated valve areas, and applies model
/// policies in order. Returns the thermal-cycle limit fixed for this step.
int prepareCouplingMode(const TransientStepState &state, int explicitScheme);

/// Advances standalone startup, boundaries, gas flow, and ordered coupling.
/// Returns the maximum volumetric unloading flow used by trend output.
double advanceStandaloneTransientStep(const TransientSolveState &state,
                                      const TransientDependencies &dependencies,
                                      double reverseQuality, double reverseVoidFraction,
                                      double reverseLiquidFraction);

/// Commits buried-wall time levels and scheduled mesh output in cell order.
/// The supplied list contains every 2D wall exactly once, including endpoints.
void finalizeBuriedPipeConduction(const TransientStepState &state,
                                 const std::vector<int> &cellIndices);

/// Finalizes derivatives, deposition, sources, thermal coupling, transported properties and densities in order.
void finalizeTransientCellState(const TransientSolveState &state, const TransientDependencies &dependencies, const ProFlu *fluiRev);

/// Updates weighted pressure, flux and void-fraction histories with the current one-sample removal rule.
void updateTransientMovingMeans(const TransientSolveState &state);

/// Sets active-pig properties and velocity, then applies every matching receiver in its original order.
void updateActivePigState(const TransientStepState &state, int i);

/// Updates the shared composition and writes one corner; indices select live minimum/maximum bounds.
void fillMiniTableCorner(const TransientStepState &state, ProFlu &fluC, ProFlu &flui,
                         int pressureIndex, int temperatureIndex);

/// Supplies gas properties from the selected sorted donor at gas-poor corners.
void supplyAbsentGasProperties(ProFlu &flui, const std::pair<double, int> (&titVec)[4]);

/// Supplies liquid properties from the selected sorted donor at liquid-poor corners.
void supplyAbsentLiquidProperties(ProFlu &flui, const std::pair<double, int> (&titVec)[4]);

/// Copies cell bounds, updates the accessory stock-tank and in-situ composition, and fills its enabled table.
void refreshAccessoryFluidMiniTable(const TransientStepState &state, int i, ProFlu &accessoryFluid);

/// Updates compositional pressure-boundary quality without changing the complementary fraction.
void refreshCompositionalInletQuality(const TransientStepState &state);

/// Updates gas-dominated pressure-boundary fractions using the first standard-ratio entry.
void refreshGasInletFractions(const TransientStepState &state);

}  // namespace
}  // namespace sisprod::transient

#endif  // SISPRODTRANSIENTINTERNAL_H_
