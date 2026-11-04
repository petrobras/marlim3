#ifndef FRICTIONTRANSITION_H_
#define FRICTIONTRANSITION_H_

namespace friction {
inline constexpr double kLaminarReynoldsLimit = 2400.;
inline constexpr double kTurbulentReynoldsLimit = 4000.;
inline constexpr double kLaminarFanningNumerator = 16.;

inline double transitionFanningFactor(double reynolds, double turbulentFactor) {
    if (reynolds >= kTurbulentReynoldsLimit) return turbulentFactor;
    const double laminarFactor = kLaminarFanningNumerator / reynolds;
    if (reynolds <= kLaminarReynoldsLimit) return laminarFactor;
    const double turbulentWeight = (reynolds - kLaminarReynoldsLimit) /
                                  (kTurbulentReynoldsLimit - kLaminarReynoldsLimit);
    return (1. - turbulentWeight) * laminarFactor + turbulentWeight * turbulentFactor;
}
}
#endif
