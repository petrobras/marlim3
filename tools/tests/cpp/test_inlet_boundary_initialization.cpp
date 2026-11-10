// Inactive inlet conditions must be empty even in reused, nonzero storage.
// Use explicit checks so this regression test also runs in Release builds.
#include "Leitura.h"

#include <array>
#include <cstdio>
#include <new>

namespace {

int failures = 0;

void check(bool condition, const char *field, unsigned char pattern) {
    if (!condition) {
        std::printf("FAIL %s after initialization over pattern 0x%02x\n",
                    field, static_cast<unsigned int>(pattern));
        ++failures;
    }
}

template <typename T>
void checkCommon(const T &value, unsigned char pattern) {
    check(value.parserie == 0, "parserie", pattern);
    check(value.temperatura == nullptr, "temperatura", pattern);
    check(value.pres == nullptr, "pres", pattern);
    check(value.bet == nullptr, "bet", pattern);
    check(value.tempo == nullptr, "tempo", pattern);
}

template <typename T, typename ExtraChecks>
void checkInitialization(unsigned char pattern, ExtraChecks extra) {
    alignas(T) std::array<unsigned char, sizeof(T)> storage;
    storage.fill(pattern);
    T *value = ::new (storage.data()) T;
    checkCommon(*value, pattern);
    extra(*value, pattern);
    value->~T();
}

} // namespace

int main() {
    for (unsigned char pattern : {0x55, 0xaa, 0xff}) {
        checkInitialization<detPresEnt>(
            pattern, [](const detPresEnt &value, unsigned char seed) {
                check(value.indFluido == -1, "indFluido", seed);
                check(value.tit == nullptr, "tit", seed);
            });
        checkInitialization<detVazPresEnt>(
            pattern, [](const detVazPresEnt &value, unsigned char seed) {
                check(value.mass == nullptr, "mass", seed);
            });
    }
    if (failures != 0) {
        std::printf("FAILED: %d initialization checks\n", failures);
        return 1;
    }
    std::printf("PASSED: inlet conditions start empty in reused storage\n");
    return 0;
}
