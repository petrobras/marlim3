#ifndef DYNAMICTABLEPOLICY_H_
#define DYNAMICTABLEPOLICY_H_

namespace fluidtables {
inline constexpr int kDefaultMiniTableDelay = 1;
inline constexpr double kMinimumMiniTableIncrement = 5.;

inline double minimumMiniTableIncrement(double increment) {
    return increment < kMinimumMiniTableIncrement ? kMinimumMiniTableIncrement : increment;
}
}
#endif
