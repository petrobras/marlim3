#ifndef SISPRODCONSTANTS_H_
#define SISPRODCONSTANTS_H_

/// Physical constants, accessory kinds and gas-line inlet conditions shared by
/// SProd and its modules; the unit conversions are in UnitConversions.h. The
/// static_asserts at the end pin each value to its spelling.
namespace sisprod {

// ---------------------------------------------------- physical constants ----

/// Density of air at standard conditions, in kg/m3. Always multiplied by a gas
/// specific gravity, giving the gas density at standard conditions.
inline constexpr double kAirDensityAtStandardConditions = 1.225;

/// Gravitational acceleration as this program uses it, not the standard gravity
/// units::kStandardGravity: "correcting" it would change every hydrostatic term.
inline constexpr double kGravity = 9.82;

/// The gas-lift unloading hydrostatics spell gravity differently; the two are
/// not interchangeable without changing results.
inline constexpr double kGravityUnloadingVariant = 9.81;

/// Written into the inlet source rate when a pressure-to-pressure search finds no
/// flowing solution; the network reads it back to report the branch.
inline constexpr double kNoFlowSolutionMarker = -2121212121.;

/// Standard conditions as this program spells them, for gas density at
/// surface: one kgf/cm^2 and fifteen degrees Celsius.
inline constexpr double kStandardPressureKgfPerCm2 = 1.;
inline constexpr double kStandardTemperatureCelsius = 15.;

// ------------------------------------------------------------- numerics ----

/// Backward perturbation for numerical derivatives.
inline constexpr double kDerivativePerturbationFactor = 0.999;

/// Below this a phase-change mass rate is treated as zero.
inline constexpr double kPhaseChangeFloor = 1e-25;

/// The temperature range the solution is clamped to, in degrees Celsius.
inline constexpr double kMinimumTemperatureCelsius = -50.;
inline constexpr double kMaximumTemperatureCelsius = 200.;

/// The lowest cell pressure, in kgf/cm^2, a production steady march goes on
/// with; at or below it the march stops as a pressure too low.
inline constexpr double kMarchMinimumPressure = 0.1;

/// True when a cell's pressure stops a production steady march: at or below
/// kMarchMinimumPressure, or NaN, which fails every comparison.
inline constexpr bool marchPressureTooLow(double pressure) {
    return !(pressure > kMarchMinimumPressure);
}

/// Magnitude of the low/high pressure failure sentinels returned by steady marches.
inline constexpr double kSteadyMarchLimit = 1e10;

/// Magnitude separating ordinary residuals from steady march failure sentinels.
inline constexpr double kSteadyMarchSentinelThreshold = 0.9e10;

/// Magnitude separating march limits from steady convergence failures.
inline constexpr double kSteadyConvergenceFailureThreshold = 1.01e10;

/// Magnitude returned when a steady search or march fails to converge.
/// Its sign follows the network iteration policy at the call site.
inline constexpr double kSteadyConvergenceFailure = 1.1e10;

/// The temperature marches of the production column and of the gas line keep a
/// cell's kinetic term only while the line is shorter than this many times the
/// cell's length.
inline constexpr double kKineticTermMaximumLengthRatio = 1e6;

// ------------------------------------------------------ accessory kinds ----

/// The value of `celula[i].acsr.tipo`, selecting the accessory attached to a
/// cell. Read from where the field is assigned, in Leitura.cpp and
/// LeituraVapor.cpp; the contradictory table in acessorios.h belongs to a dead
/// function. Type 7 is absent because no assignment names it.
enum AccessoryKind : int {
    kAccessoryNone = 0,             ///< no source attached
    kAccessoryGasInjection = 1,     ///< acsr.injg
    kAccessoryLiquidInjection = 2,  ///< acsr.injl
    kAccessoryInflowPerformance = 3,///< acsr.ipr
    kAccessoryPump = 4,             ///< acsr.bcs, electrical submersible pump
    kAccessoryChoke = 5,            ///< acsr.chk
    kAccessoryVolumetricPump = 8,   ///< acsr.bvol
    kAccessoryLeak = 9,             ///< acsr.fontechk
    kAccessoryMultipleSource = 10,  ///< acsr.injm
    kAccessoryRadialPorous = 15,    ///< acsr.radialPoro
    kAccessoryPorous2D = 16,        ///< acsr.poroso2D
    kAccessoryMultiPump = 17,       ///< acsr.multibcs
    kAccessoryRecirculationUpstream = 18,
    kAccessoryRecirculationDownstream = 19,
};

// ------------------------------------------------------ gas-lift valves ----

/// The value of a gas-lift valve's `ChokeGas::tipo`, from the input's
/// tipoValvula: an orifice, or a pressure valve calibrated at the surface.
enum GasLiftValveKind : int {
    kGasLiftOrificeValve = 0,
    kGasLiftCalibratedValve = 1,
};

/// The value of `acsr.injg.tipoflu`: what a gas-lift valve delivers into its
/// production cell, gas or the completion fluid behind the unloading interface.
enum InjectedFluid : int {
    kInjectedGas = 0,
    kInjectedCompletionFluid = 1,
};

// ------------------------------------------------ gas-line inlet condition ----

/// The value of `tipoCC` on the gas line's first cell, and of the configured
/// `gasinj.tipoCC`: what drives the gas line at the injection point. From the
/// input schema (docs/schemas/branch.pt.json, gasInj.tipoCC): 0 is the
/// injection pressure, 1 the injection flow rate.
enum : int {
    kGasInletInjectionPressure = 0, ///< the injection pressure is given
    kGasInletInjectionFlowRate = 1, ///< the injection flow rate is given
};

/// The two conditions as types. Where the gas line behaves differently under
/// each, the difference is written once per condition, as an overload taking
/// one of these, in the module that owns the data, instead of the flag being
/// tested at every place that cares.
struct InjectionPressureCondition {};
struct InjectionFlowRateCondition {};

/// The one place the inlet condition is decided: calls `visitor` with the
/// condition `tipoCC` names. Zero is the injection pressure and anything else
/// the flow rate; the input does not reject a value outside {0, 1}. The one site
/// that reads the flag the other way round says so where it is.
///
/// Called where the condition matters, not once in an adapter: the first gas
/// cell exists only when there is a gas line, and every call sits behind a
/// guard that says so.
template <typename Visitor>
decltype(auto) withGasInletCondition(int tipoCC, Visitor &&visitor) {
    if (tipoCC == kGasInletInjectionPressure)
        return visitor(InjectionPressureCondition{});
    return visitor(InjectionFlowRateCondition{});
}

// ------------------------------------------------ solution and run state ----

/// Unknowns per production-line cell in the pressure-velocity system, and the
/// diagonals of its band matrix to the left and to the right of the main one.
inline constexpr int kProductionUnknownsPerCell = 2;
inline constexpr int kProductionLeftBandwidth = 3;
inline constexpr int kProductionRightBandwidth = 2;

/// Unknowns per gas-line cell in the gas line's pressure-velocity system, and the
/// diagonals of its band matrix on each side of the main one.
inline constexpr int kGasLineUnknownsPerCell = 3;
inline constexpr int kGasLineBandwidth = 5;

/// Coefficients in each equation of a gas-line cell: the unknowns of the cell
/// and of its two neighbours.
inline constexpr int kGasLineStencilCoefficients = 3 * kGasLineUnknownsPerCell;

/// Columns of a profile row besides the variables the input asks for.
inline constexpr int kProductionProfileFixedColumns = 6;
inline constexpr int kGasLineProfileFixedColumns = 7;

/// The value the first columns of a trend row hold until the row is written.
inline constexpr double kTrendSentinel = -10000.;

/// The interface cell a gas line starts from: beyond any cell, so that no cell
/// holds the completion-fluid interface.
inline constexpr int kNoInterfaceCell = 10000000;

/// Length of the Master1 ratio histories, and the critical ratio they start from.
inline constexpr int kMasterRatioHistorySize = 10;
inline constexpr double kDefaultMasterCriticalRatio = 0.5;

/// The averaging window of the unloading controller, in seconds, and the number
/// of samples it keeps.
inline constexpr double kUnloadingAveragingWindow = 10.;
inline constexpr int kUnloadingSampleCapacity = 1000;

/// The starting value of the surface choke's slug counter.
inline constexpr int kInitialSlugCount = 1000;

/// The steady-state convergence monitor before the first iteration, and the
/// default control parameter of the slowly varying thermal coupling.
inline constexpr double kInitialConvergenceMonitor = 1000.;
inline constexpr double kSlowHeatTransferThreshold = 0.01;

static_assert(kProductionUnknownsPerCell == 2);
static_assert(kProductionLeftBandwidth == 3);
static_assert(kProductionRightBandwidth == 2);
static_assert(kGasLineUnknownsPerCell == 3);
static_assert(kGasLineBandwidth == 5);
static_assert(kGasLineStencilCoefficients == 9);
static_assert(kProductionProfileFixedColumns == 6);
static_assert(kGasLineProfileFixedColumns == 7);
static_assert(kTrendSentinel == -10000.);
static_assert(kNoInterfaceCell == 10000000);
static_assert(kMasterRatioHistorySize == 10);
static_assert(kDefaultMasterCriticalRatio == 0.5);
static_assert(kUnloadingAveragingWindow == 10.);
static_assert(kUnloadingSampleCapacity == 1000);
static_assert(kInitialSlugCount == 1000);
static_assert(kInitialConvergenceMonitor == 1000.);
static_assert(kSlowHeatTransferThreshold == 0.01);

static_assert(kAccessoryNone == 0);
static_assert(kAccessoryGasInjection == 1);
static_assert(kAccessoryLiquidInjection == 2);
static_assert(kAccessoryInflowPerformance == 3);
static_assert(kAccessoryPump == 4);
static_assert(kAccessoryChoke == 5);
static_assert(kAccessoryVolumetricPump == 8);
static_assert(kAccessoryLeak == 9);
static_assert(kAccessoryMultipleSource == 10);
static_assert(kAccessoryRadialPorous == 15);
static_assert(kAccessoryPorous2D == 16);
static_assert(kAccessoryMultiPump == 17);
static_assert(kAccessoryRecirculationUpstream == 18);
static_assert(kAccessoryRecirculationDownstream == 19);
static_assert(kGasInletInjectionPressure == 0);
static_assert(kGasInletInjectionFlowRate == 1);
static_assert(kGasLiftOrificeValve == 0);
static_assert(kGasLiftCalibratedValve == 1);
static_assert(kInjectedGas == 0);
static_assert(kInjectedCompletionFluid == 1);

static_assert(kAirDensityAtStandardConditions == 1.225);
static_assert(kGravity == 9.82);
static_assert(kGravityUnloadingVariant == 9.81);
static_assert(kNoFlowSolutionMarker == -2121212121.);
static_assert(kStandardPressureKgfPerCm2 == 1.);
static_assert(kStandardTemperatureCelsius == 15.);
static_assert(kDerivativePerturbationFactor == 0.999);
static_assert(kPhaseChangeFloor == 1e-25);
static_assert(kMinimumTemperatureCelsius == -50.);
static_assert(kMaximumTemperatureCelsius == 200.);
static_assert(kMarchMinimumPressure == 0.1);
static_assert(kSteadyMarchLimit == 1e10);
static_assert(kSteadyMarchSentinelThreshold == 0.9e10);
static_assert(kSteadyConvergenceFailureThreshold == 1.01e10);
static_assert(kSteadyConvergenceFailure == 1.1e10);
static_assert(marchPressureTooLow(0.1) && !marchPressureTooLow(0.2));
static_assert(kKineticTermMaximumLengthRatio == 1e6);

}  // namespace sisprod

#endif  // SISPRODCONSTANTS_H_
