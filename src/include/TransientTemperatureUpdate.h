#ifndef TRANSIENTTEMPERATUREUPDATE_H_
#define TRANSIENTTEMPERATUREUPDATE_H_

#include "SisProdConstants.h"

#include <algorithm>
#include <cmath>

namespace sisprod::thermal::detail {

inline double limitExplicitTemperature(double previousTemperature, double temperature,
                                       double timeStep, double transportVelocity,
                                       double referenceMixtureVelocity, double minimumTemperature) {
    constexpr double maximumTemperatureRate = 10.;
    constexpr double transportResetTemperatureRate = 1.;
    constexpr double maximumTransportVelocityRatio = 10.;
    const double change = temperature - previousTemperature;
    const double rate = std::abs(change) / timeStep;
    if (rate > maximumTemperatureRate)
        temperature = previousTemperature +
                      (std::abs(change) / change) * maximumTemperatureRate * timeStep;
    else if (rate > transportResetTemperatureRate &&
             std::abs(transportVelocity) > maximumTransportVelocityRatio * referenceMixtureVelocity)
        temperature = previousTemperature;
    return std::clamp(temperature, minimumTemperature, kMaximumTemperatureCelsius);
}

}

#endif
