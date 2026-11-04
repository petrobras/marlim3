#include "RootFindingSolvers.h"

#include "FerramentasNumericas.h"

namespace rootfinding {

int sign(double value) {
    if (value <= 0.)
        return -1;
    else
        return 1;
}

void reportIterationLimit(const char *message) {
    NumError(message);
}

}  // namespace rootfinding
