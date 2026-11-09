// ==============================================================================
// tuffp_runtime_binding.h
//
// Adapter for the Tulsa University Fluid Flow Projects (TUFFP) Unified
// gas-liquid model (v2026.0). TUFFP is NOT distributed with Marlim3: the
// shared library is loaded at runtime from
//   1. the path in the environment variable MARLIM3_TUFFP_LIB, or
//   2. a library placed next to the Marlim3 executable
//      (tuffp_v2026.0.dll on Windows, libtuffp.so on Linux,
//       libtuffp.dylib on macOS).
// ==============================================================================
#pragma once
#include <cstdint>
#include <string>

namespace tuffp {

// ------------------------------------------------------------------------------
// C ABI mirror of the TUFFP data types (point_model_dll.f90 / constants.f90).
// Fortran integers are int32 (ip => int32); the vendor header uses `long`,
// which is 8 bytes on LP64 platforms, so fixed-width types are used here.
// ------------------------------------------------------------------------------
constexpr int32_t NOT_USED = 100;

struct Loc1   { int32_t i; int32_t j; int32_t w; };
struct FilmM  { int32_t fe; double fm; int32_t wgl; int32_t wll; Loc1 f; };
struct SlugM  { int32_t hl; int32_t sm; int32_t vd; int32_t vt; Loc1 f; };

struct Inf    { bool er; int32_t it; char nm[21]; };
struct LayN   { Inf f; Inf h; Inf w; };
struct FilmN  { LayN gas; LayN liq; LayN oil; };
struct SlugN  { LayN mix; };

struct F1     { double den; double stn; double vel; double vis; };
struct Psp1   { double ang; double dia; double rou; };

struct Prg    { int32_t fpc; double hlg; double hll; double ace; double fri; double gra; double tot; };
struct Loc2   { double i; double w; };
struct Lay    { double h; double v; Loc2 f; Loc2 s; Loc2 t; };
struct Film   { double ent; double len; Lay gas; Lay liq; Lay oil; };
struct Slug   { double fre; double len; Lay mix; };

using FpGlUnifiedFn = void (*)(Psp1*, F1*, F1*, FilmM*, SlugM*,
                               Prg*, Film*, FilmN*, Slug*, SlugN*);

// TUFFP flow pattern codes (prg.fpc)
enum class Pattern : int32_t {
    NoPattern          = 0,
    SinglePhaseGas     = 1,
    SinglePhaseLiquid  = 2,
    Bubble             = 10,
    DispersedBubble    = 11,
    Slug               = 12,
    StratifiedSmooth   = 14,
    StratifiedWavy     = 15,
    Annular            = 16,
    AnnularTwoFilms    = 18
};

} // namespace tuffp

// ------------------------------------------------------------------------------
// Marlim3-facing API (SI units)
// ------------------------------------------------------------------------------
struct TuffpInput {
    double vSL;       // liquid superficial velocity [m/s]
    double vSG;       // gas superficial velocity [m/s]
    double rhoL;      // liquid density [kg/m3]
    double rhoG;      // gas density [kg/m3]
    double muL;       // liquid viscosity [Pa.s]
    double muG;       // gas viscosity [Pa.s]
    double sigma;     // gas-liquid surface tension [N/m]
    double diameter;  // pipe inner diameter [m]
    double roughness; // absolute wall roughness [m]
    double angle;     // inclination [deg, 0 = horizontal, 90 = vertical upward]
    // Liquid holdup assumed when there is no flow (both superficial
    // velocities ~0), where the result is a hydrostatic column.
    double noFlowHoldup = 1.0;
};

struct TuffpResult {
    int32_t     patternCode = 0;                 // TUFFP prg.fpc (see tuffp::Pattern)
    double      HL          = 0.0;               // gas-liquid holdup [-]
    double      dpdL_acc    = 0.0;               // acceleration component [Pa/m]
    double      dpdL_fric   = 0.0;               // friction component [Pa/m]
    double      dpdL_grav   = 0.0;               // gravity component [Pa/m]
    double      dpdL_tot    = 0.0;               // total [Pa/m]
    bool        error       = false;             // TUFFP error flag set or input outside model scope
    std::string errorReport;                     // description of the issue(s)
};

// Velocities below this magnitude [m/s] are treated as zero.
constexpr double TUFFP_ZERO_VELOCITY = 1.0e-12;

// Loads the TUFFP library once (thread-safe). Returns false and fills
// `message` when the library or the entry point cannot be found.
bool tuffpLoad(std::string& message);

// True when the library has been successfully loaded.
bool tuffpAvailable();

// Path of the library that was loaded (empty when not loaded).
std::string tuffpLibraryPath();

// Runs the TUFFP Unified gas-liquid model with the default TUFFP closures.
// TUFFP only handles co-current flow in the positive direction, so:
//  - reverse co-current flow is solved in the mirrored direction and the
//    gradients are returned in the pipe (geometric) direction;
//  - counter-current flow keeps only the phase moving with the mixture and
//    is flagged in `error`;
//  - no flow returns a hydrostatic column with `noFlowHoldup`.
// Calls are serialized because the TUFFP Fortran code is not guaranteed to
// be reentrant. Requires a successful tuffpLoad().
TuffpResult calculateTuffp(const TuffpInput& input);
