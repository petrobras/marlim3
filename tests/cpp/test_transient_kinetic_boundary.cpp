#include "TransientKineticResponse.h"

#include <array>
#include <cstdio>

struct BoundaryCase {
    int cellIndex;
    int lastCell;
    bool ordinaryAccessories;
    double throatArea;
    double tubeArea;
    bool expected;
};

constexpr std::array boundaryCases = {
    BoundaryCase{1, 4, true, 0., 1., true},
    BoundaryCase{1, 4, false, 1., 1., false},
    BoundaryCase{3, 4, true, 0., 1., false},
    BoundaryCase{3, 4, true, 0.6, 1., false},
    BoundaryCase{3, 4, false, 0.61, 1., true},
    BoundaryCase{3, 4, true, 1.2, 2., false},
    BoundaryCase{4, 4, true, 1., 1., false},
    BoundaryCase{0, 1, false, 1., 1., true},
    BoundaryCase{0, 0, true, 1., 1., false},
    BoundaryCase{-1, 4, true, 1., 1., false},
};

int main() {
    for (const auto &test : boundaryCases)
        if (sisprod::thermal::detail::includesTransientKineticWork(
                test.cellIndex, test.lastCell, test.ordinaryAccessories,
                test.throatArea, test.tubeArea) != test.expected) {
            std::fprintf(stderr, "Incorrect kinetic boundary at cell %d\n", test.cellIndex);
            return 1;
        }
    return 0;
}
