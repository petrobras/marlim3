#include "Bcsm2.h"
#include "DynamicTablePolicy.h"
#include "FrictionTransition.h"

#include <cmath>
#include <cstdio>
#include <limits>

namespace {
int failures = 0;

void expectNear(double actual, double expected, const char *name) {
    if (!std::isfinite(actual) || std::fabs(actual - expected) > 1.e-5) {
        std::printf("FAIL %s: %.17g != %.17g\n", name, actual, expected);
        ++failures;
    }
}

void checkFrictionTransition() {
    expectNear(friction::transitionFanningFactor(2400., .02), 16. / 2400., "laminar endpoint");
    expectNear(friction::transitionFanningFactor(2800., .02), .75 * 16. / 2800. + .25 * .02, "lower transition");
    expectNear(friction::transitionFanningFactor(3200., .02), .0125, "transition midpoint");
    expectNear(friction::transitionFanningFactor(3600., .02), .25 * 16. / 3600. + .75 * .02, "upper transition");
    expectNear(friction::transitionFanningFactor(4000., .02), .02, "turbulent endpoint");
    expectNear(friction::transitionFanningFactor(5000., .03), .03, "turbulent factor preserved");
}

void checkMiniTablePolicy() {
    for (double increment : {-1., 0., 4.9, 5.})
        expectNear(fluidtables::minimumMiniTableIncrement(increment), 5., "minimum flash increment");
    expectNear(fluidtables::minimumMiniTableIncrement(10.), 10., "larger flash increment");
    expectNear(fluidtables::kDefaultMiniTableDelay, 1., "default mini-table delay");
}

BomCentSub makePump(const double *heads) {
    constexpr int pointCount = 4;
    const double flows[pointCount] = {0., 100., 200., 300.};
    const double powers[pointCount] = {1., 2., 3., 4.};
    const double efficiencies[pointCount] = {10., 50., 60., 30.};
    return BomCentSub(pointCount, flows, heads, powers, efficiencies, 60.);
}

void checkPumpRoots() {
    const double decliningHeads[] = {100., 80., 60., 40.};
    const auto pump = makePump(decliningHeads);
    expectNear(pump.MaxVaz(), 500., "zero head beyond sampled curve");
    expectNear(pump.Fhead(400.), 20., "positive head before root");
    expectNear(pump.Fhead(500.), 0., "zero at physical root");
    expectNear(pump.Fhead(600.), 0., "head clipped after root");
    const double endpointHeads[] = {60., 40., 20., 0.};
    expectNear(makePump(endpointHeads).MaxVaz(), 300., "zero head at final sample");
    const double flatHeads[] = {100., 80., 40., 40.};
    expectNear(makePump(flatHeads).MaxVaz(), -1., "flat tail has no extrapolated root");
}

void checkEmptyPump(const BomCentSub &pump) {
    expectNear(pump.MaxVaz(), 1.e10, "empty pump flow bound");
    expectNear(pump.Fhead(100.), 0., "empty head");
    expectNear(pump.Fpower(100.), 0., "empty power");
    expectNear(pump.Fefic(100.), 0., "empty efficiency");
    expectNear(pump.Defic(100.), 0., "empty efficiency derivative");
}
}

int main() {
    checkFrictionTransition();
    checkMiniTablePolicy();
    checkPumpRoots();
    const BomCentSub empty;
    checkEmptyPump(empty);
    const double heads[] = {100., 80., 60., 40.};
    auto assigned = makePump(heads);
    assigned = empty;
    checkEmptyPump(assigned);
    return failures == 0 ? 0 : 1;
}
