#ifndef TRANSIENTKINETICRESPONSE_H_
#define TRANSIENTKINETICRESPONSE_H_

namespace sisprod::thermal::detail {

inline bool includesTransientKineticWork(int cellIndex, int lastCell, bool ordinaryAccessories,
                                       double chokeThroatArea, double chokeTubeArea) {
    constexpr double minimumOpenOutletAreaRatio = 0.6;
    if (cellIndex < 0 || cellIndex >= lastCell)
        return false;
    if (cellIndex == lastCell - 1)
        return chokeThroatArea > minimumOpenOutletAreaRatio * chokeTubeArea;
    return ordinaryAccessories;
}

} // namespace sisprod::thermal::detail

#endif // TRANSIENTKINETICRESPONSE_H_
