// Test double for the external TUFFP ABI. This is not a physical TUFFP model:
// predictable patterns and finite gradients exercise the engine integration.
#include "tuffp_runtime_binding.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#define TUFFP_TEST_EXPORT __declspec(dllexport)
#else
#define TUFFP_TEST_EXPORT
#endif

extern "C" TUFFP_TEST_EXPORT void tuffp_fp_gl_unified(
    tuffp::Psp1 *pipe, tuffp::F1 *gas, tuffp::F1 *liquid,
    tuffp::FilmM *, tuffp::SlugM *, tuffp::Prg *gradient,
    tuffp::Film *, tuffp::FilmN *filmReport, tuffp::Slug *, tuffp::SlugN *) {
    const char *pattern = std::getenv("MARLIM3_TEST_TUFFP_PATTERN");
    gradient->fpc = pattern ? std::atoi(pattern) : static_cast<int32_t>(tuffp::Pattern::Slug);
    gradient->hlg = 0.25;
    const double density = gradient->hlg * liquid->den + (1.0 - gradient->hlg) * gas->den;
    gradient->gra = density * 9.80665 * std::sin(pipe->ang);
    gradient->fri = 10.0 * (gas->vel + liquid->vel);
    gradient->ace = 0.0;
    gradient->tot = gradient->gra + gradient->fri;
    if (std::getenv("MARLIM3_TEST_TUFFP_WARNING")) {
        filmReport->gas.f.er = true;
        filmReport->gas.f.it = 7;
        std::strcpy(filmReport->gas.f.nm, "contract warning");
    }
}
