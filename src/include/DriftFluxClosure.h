#ifndef DRIFTFLUXCLOSURE_H_
#define DRIFTFLUXCLOSURE_H_

/// Drift-flux closure relations.
///
/// The functions in driftflux::correlations are pure: they read nothing but
/// their arguments and write nothing but the c0 and ud output references, which
/// makes them testable in isolation against a tabulated sweep.
// Declared, not included: ClosureState below holds only references to these,
// so this header stays free of the cell, input-deck and globals headers and can
// still be compiled on its own.
class Cel;
class Ler;
struct SimulationContext;

namespace driftflux {
namespace correlations {

/// Evaluates C0 and Ud using the Bhagwat-Ghajar drift-flux correlation.
void BhagwatGhajar(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                   double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                   double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                   double horizontalDriftSign);

/// Evaluates C0 and Ud using the Bhagwat-Ghajar drift-flux correlation.
void BhagwatGhajarMod(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                      double mixtureReynolds, double liquidReynolds, double gasFlowRate,
                      double liquidFlowRate, double diameter, double roughness, double inclinationAngle,
                      double &c0, double &ud, double horizontalDriftSign);

/// Evaluates C0 and Ud using the Choi drift-flux correlation.
void Choi(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
          double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
          double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
          double horizontalDriftSign);

/// Evaluates C0 and Ud using the Hibiki-Ishii drift-flux correlation.
void HibikiIshii(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                 double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                 double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                 double horizontalDriftSign);

/// Evaluates C0 and Ud using the Franca-Lahey drift-flux correlation.
void FrancaLahey(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                 double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                 double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                 double horizontalDriftSign);

/// Correlation selected by the aggregators below, resolved from the integer
/// configuration field before the call.
///
/// The three aggregators accept different subsets of these values, and each
/// leaves c0 and ud untouched when the selector falls outside its own subset.
///
///   0  Choi              accepted by all three
///   1  BhagwatGhajar     accepted by all three
///   2  FrancaLahey       stratified only
///   3  HibikiIshii       annular/churn only
///   4  BhagwatGhajarMod  accepted by all three
///   5  angle blend       accepted by all three

/// The correlation each flow regime selects, read from the input once for each
/// closure state rather than on every cell.
///
/// The three fields are constant for a whole simulation: the engine parses them
/// from the input file and never writes them again. Holding them together says
/// so, and keeps configuration out of the per-cell path.
///
/// The values are the ones documented above. Each regime accepts its own subset
/// and ignores the rest, so the fields are not interchangeable.
struct RegimeSelectors {
    int dispersed;    ///< arq.CorreDisper, accepts 0, 1, 4 and 5.
    int annularChurn; ///< arq.CorreAnular, accepts 3 as well.
    int stratified;   ///< arq.CorreEstrat, accepts 2 as well.
};

/// Evaluates C0 and Ud for dispersed flow, using the already-resolved
/// correlationIndex (arq.CorreDisper at the call site) instead of reading the
/// configuration itself.
void C0UdDisperso(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                  double mixtureReynolds, double liquidReynolds, double gasFlowRate, double liquidFlowRate,
                  double diameter, double roughness, double inclinationAngle, double &c0, double &ud,
                  double horizontalDriftSign, int estabCol, int correlationIndex);

/// Evaluates C0 and Ud for annular or churn flow, using the already-resolved
/// correlationIndex (arq.CorreAnular at the call site).
void C0UdAnularChurn(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                     double mixtureReynolds, double liquidReynolds, double gasFlowRate,
                     double liquidFlowRate, double diameter, double roughness, double inclinationAngle,
                     double &c0, double &ud, double horizontalDriftSign, int estabCol, int correlationIndex);

/// Evaluates C0 and Ud for stratified flow, using the already-resolved
/// correlationIndex (arq.CorreEstrat at the call site).
void C0UdEstratificado(double liquidDensity, double gasDensity, double surfaceTension, double voidFraction,
                       double mixtureReynolds, double liquidReynolds, double gasFlowRate,
                       double liquidFlowRate, double diameter, double roughness, double inclinationAngle,
                       double &c0, double &ud, double horizontalDriftSign, int estabCol,
                       int correlationIndex);

}  // namespace correlations

/// Distribution coefficient: the five variants of CalcC0Ud.
///
/// These are not pure. They read the cells intensively, they write back into
/// them -- arranjo, arranjoR, transic, transic0, perdaEstratL/G, c0Spare,
/// udSpare -- and they call the correlations above. They call no method of
/// SProd: the only calls in the five bodies are fabs, pow and the two local
/// flow-pattern map objects.
///
/// The five differ far more than their names suggest: one builds the
/// flow-pattern map and writes the pattern back, another reads the pattern it
/// was given; one runs the transition counter, another does not. So each keeps
/// its own control flow, and only the blocks they share exactly are shared.
namespace coefficient {

/// The state the five read, and the only state they may read.
///
/// Same role as TrendState in the trend module: it names in one place what a
/// closure evaluation is allowed to touch, and it makes the five callable
/// without an SProd, so they can be driven over synthetic cells.
///
/// The scalars are held by reference, not by value: a copy would read every one
/// of them at construction, before the branch that decides whether it is read
/// at all.
struct ClosureState {
    /// The cell array -- SProd::celula. Written as well as read.
    Cel *const &cells;
    /// Index of the last cell -- SProd::ncel.
    const int &lastCell;
    /// Shared 1D globals; only localtiny is read, plus the pointer handed to
    /// the flow-pattern map -- SProd::simulationContext.
    SimulationContext *const &globals;
    /// Input deck -- SProd::arq. Read for mapaArranjo, escorregaTran,
    /// escorregaPerm and AceleraConvergPerm.
    const Ler &input;
    /// Correlation choice per regime -- SProd::arq's CorreDisper, CorreAnular
    /// and CorreEstrat. Held by value: the input never changes them once read.
    correlations::RegimeSelectors selectors;

    /// Gas temperature at the surface -- SProd::tGSup.
    const double &gasSurfaceTemperature;
    /// Inlet void fraction -- SProd::alfE. Read by the initialisation variants.
    const double &inletVoidFraction;
    /// Inlet column fraction -- SProd::betaE.
    const double &inletComplementaryFraction;
    /// Inlet pressure -- SProd::presE.
    const double &inletPressure;
    /// Inlet temperature -- SProd::tempE.
    const double &inletTemperature;
    /// Steady-state iteration counter -- SProd::steadyIteration.
    const int &steadyIteration;
};

/// Slip parameters at a production-line face, from the instantaneous state.
void instantaneous(const ClosureState &state, int cellIndex, double &c0, double &ud);

/// Slip parameters for the intermediate network state, from the buffered
/// fields.
void buffered(const ClosureState &state, int cellIndex, double &c0, double &ud);

/// Slip parameters at the inlet of an internal network section.
void initialization(const ClosureState &state, int cellIndex, double &c0, double &ud);

/// Slip parameters at the inlet of an internal network section, from the
/// buffered fields.
void bufferedInitialization(const ClosureState &state, int cellIndex, double &c0, double &ud);

/// Steady-state slip parameters at a downstream face.
void steadyState(const ClosureState &state, int cellIndex, double &c0, double &ud);

}  // namespace coefficient
}  // namespace driftflux

#endif  // DRIFTFLUXCLOSURE_H_
