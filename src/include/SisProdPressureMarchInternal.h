#ifndef SISPRODPRESSUREMARCHINTERNAL_H_
#define SISPRODPRESSUREMARCHINTERNAL_H_

#include "SisProdSteadyState.h"

namespace sisprod::steady::detail {

/// Location of a pressure-limited march; cell remains -1 when the march completes.
struct PressureMarchStop {
    int cell = -1;
    double pressure = 0.;
};

/// Runs the fixed-pressure production march and optionally records its pressure-limit stop.
/// Returns the same residual or failure sentinel as the public march.
double marchProductionPressureToPressure(const SteadyStateState &state, const SteadyDependencies &dependencies,
                                         double flowRateGuess, PressureMarchStop *stop);

}  // namespace sisprod::steady::detail

#endif  // SISPRODPRESSUREMARCHINTERNAL_H_
