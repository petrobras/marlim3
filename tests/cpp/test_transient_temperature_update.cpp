#include "TransientTemperatureUpdate.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <limits>

namespace {
struct TemperatureCase {
    double previous;
    double candidate;
    double timeStep;
    double transportVelocity;
    double mixtureVelocity;
    double minimum;
    double expected;
};

constexpr std::array<TemperatureCase, 15> cases{{
    {40., 42., 1., 5., 1., -50., 42.},
    {40., 140., 1., 0., 1., -50., 50.},
    {40., -60., 1., 0., 1., -50., 30.},
    {40., 80., .5, 0., 1., -50., 45.},
    {40., 0., .5, 0., 1., -50., 35.},
    {40., 50., 1., 5., 1., -50., 50.},
    {40., 42., 1., 11., 1., -50., 40.},
    {40., 38., 1., -11., 1., -50., 40.},
    {40., 41., 1., 11., 1., -50., 41.},
    {40., 42., 1., 10., 1., -50., 42.},
    {40., 42., 1., 11., 0., -50., 40.},
    {199., 203., 1., 0., 1., -50., 200.},
    {-49., -55., 1., 0., 1., -50., -50.},
    {-38., -44., 1., 0., 1., -39., -39.},
    {40., 40., 1., 11., 1., -50., 40.},
}};

bool checkTemperature(const TemperatureCase &test) {
    const double actual = sisprod::thermal::detail::limitExplicitTemperature(
        test.previous, test.candidate, test.timeStep, test.transportVelocity,
        test.mixtureVelocity, test.minimum);
    if (actual == test.expected)
        return true;
    std::fprintf(stderr, "Temperature update: got %.17g, expected %.17g\n", actual, test.expected);
    return false;
}

bool preservesInvalidTemperature() {
    const double invalid = std::numeric_limits<double>::quiet_NaN();
    return std::isnan(sisprod::thermal::detail::limitExplicitTemperature(
        40., invalid, 1., 0., 1., -50.));
}
}

int main() {
    int failures = 0;
    for (const auto &test : cases)
        failures += !checkTemperature(test);
    failures += !preservesInvalidTemperature();
    return failures ? 1 : 0;
}
