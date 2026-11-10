// ==============================================================================
// tuffp_runtime_binding.cpp - runtime binding to the external TUFFP Unified model library.
// ==============================================================================
#include "tuffp_runtime_binding.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#include <climits>
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
#endif

namespace {

#if defined(_WIN32)
constexpr const char* DEFAULT_LIBRARY_NAME = "tuffp_v2026.0.dll";
constexpr char PATH_SEPARATOR = '\\';
#elif defined(__APPLE__)
constexpr const char* DEFAULT_LIBRARY_NAME = "libtuffp.dylib";
constexpr char PATH_SEPARATOR = '/';
#else
constexpr const char* DEFAULT_LIBRARY_NAME = "libtuffp.so";
constexpr char PATH_SEPARATOR = '/';
#endif

constexpr const char* ENV_LIBRARY_PATH = "MARLIM3_TUFFP_LIB";
constexpr const char* ENTRY_POINT = "tuffp_fp_gl_unified";
constexpr double PI = 3.14159265358979323846;
constexpr double GRAVITY = 9.80665; // same value used by TUFFP [m/s2]

struct LoaderState {
    std::once_flag once;
    bool loaded = false;
    std::string message;
    std::string path;
    tuffp::FpGlUnifiedFn fpGlUnified = nullptr;
};

LoaderState& state() {
    static LoaderState s;
    return s;
}

std::mutex& callMutex() {
    static std::mutex m;
    return m;
}

std::string executableDirectory() {
#if defined(_WIN32)
    std::vector<wchar_t> buffer(MAX_PATH);
    for (;;) {
        DWORD n = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (n == 0) return {};
        if (n < buffer.size()) {
            std::wstring path(buffer.data(), n);
            auto pos = path.find_last_of(L"\\/");
            if (pos == std::wstring::npos) return {};
            path.resize(pos);
            int len = WideCharToMultiByte(CP_UTF8, 0, path.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (len <= 0) return {};
            std::string out(static_cast<size_t>(len - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, path.c_str(), -1, out.data(), len, nullptr, nullptr);
            return out;
        }
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::vector<char> buffer(size + 1, '\0');
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) return {};
    char resolved[PATH_MAX];
    std::string path = realpath(buffer.data(), resolved) ? resolved : buffer.data();
    auto pos = path.find_last_of('/');
    return pos == std::string::npos ? std::string() : path.substr(0, pos);
#else
    char buffer[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (n <= 0) return {};
    std::string path(buffer, static_cast<size_t>(n));
    auto pos = path.find_last_of('/');
    return pos == std::string::npos ? std::string() : path.substr(0, pos);
#endif
}

#if defined(_WIN32)
std::wstring toWide(const std::string& s) {
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    if (len <= 0) {
        len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, nullptr, 0);
        std::wstring w(static_cast<size_t>(len > 0 ? len - 1 : 0), L'\0');
        if (len > 0) MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, w.data(), len);
        return w;
    }
    std::wstring w(static_cast<size_t>(len - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, w.data(), len);
    return w;
}

std::string lastErrorText() {
    DWORD code = GetLastError();
    LPWSTR text = nullptr;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, code, 0, reinterpret_cast<LPWSTR>(&text), 0, nullptr);
    std::string msg;
    if (text) {
        int len = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
        if (len > 1) {
            msg.assign(static_cast<size_t>(len - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, text, -1, msg.data(), len, nullptr, nullptr);
        }
        LocalFree(text);
    }
    while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r' || msg.back() == ' ')) msg.pop_back();
    return "error " + std::to_string(code) + (msg.empty() ? "" : " (" + msg + ")");
}
#endif

// Tries to open `path` and resolve the TUFFP entry point. Returns an empty
// string on success, or a description of the failure.
std::string tryLoad(const std::string& path, tuffp::FpGlUnifiedFn& fn) {
#if defined(_WIN32)
    HMODULE handle = LoadLibraryW(toWide(path).c_str());
    if (!handle) return "cannot load '" + path + "': " + lastErrorText();
    FARPROC symbol = GetProcAddress(handle, ENTRY_POINT);
    if (!symbol) {
        std::string err = "entry point '" + std::string(ENTRY_POINT) + "' not found in '" + path + "'";
        FreeLibrary(handle);
        return err;
    }
    fn = reinterpret_cast<tuffp::FpGlUnifiedFn>(reinterpret_cast<void*>(symbol));
#else
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        const char* e = dlerror();
        return "cannot load '" + path + "': " + (e ? e : "unknown error");
    }
    void* symbol = dlsym(handle, ENTRY_POINT);
    if (!symbol) {
        std::string err = "entry point '" + std::string(ENTRY_POINT) + "' not found in '" + path + "'";
        dlclose(handle);
        return err;
    }
    fn = reinterpret_cast<tuffp::FpGlUnifiedFn>(symbol);
#endif
    return {};
}

void loadOnce() {
    LoaderState& s = state();
    std::string candidate;
    const char* env = std::getenv(ENV_LIBRARY_PATH);
    if (env && *env) {
        candidate = env;
    } else {
        std::string dir = executableDirectory();
        candidate = dir.empty() ? std::string(DEFAULT_LIBRARY_NAME)
                                : dir + PATH_SEPARATOR + DEFAULT_LIBRARY_NAME;
    }

    std::string err = tryLoad(candidate, s.fpGlUnified);
    if (err.empty()) {
        s.loaded = true;
        s.path = candidate;
        return;
    }
    s.message = "TUFFP library unavailable: " + err +
                ". Set the environment variable " + ENV_LIBRARY_PATH +
                " to the TUFFP shared library path or place '" + DEFAULT_LIBRARY_NAME +
                "' next to the Marlim3 executable.";
}

void appendReport(std::string& report, const char* label, const tuffp::Inf& inf) {
    if (!inf.er) return;
    size_t len = strnlen(inf.nm, sizeof(inf.nm));
    std::string nm(inf.nm, len);
    while (!nm.empty() && nm.back() == ' ') nm.pop_back();
    if (!report.empty()) report += "; ";
    report += label;
    report += " (it=" + std::to_string(inf.it) + (nm.empty() ? "" : ", " + nm) + ")";
}

void appendLayer(std::string& report, const char* layer, const tuffp::LayN& lay) {
    // This runs for every correlation call; build diagnostic labels only on errors.
    if (!lay.f.er && !lay.h.er && !lay.w.er) return;
    std::string prefix(layer);
    appendReport(report, (prefix + " friction").c_str(), lay.f);
    appendReport(report, (prefix + " holdup").c_str(), lay.h);
    appendReport(report, (prefix + " wettability").c_str(), lay.w);
}

} // namespace

bool tuffpLoad(std::string& message) {
    LoaderState& s = state();
    std::call_once(s.once, loadOnce);
    message = s.loaded ? std::string() : s.message;
    return s.loaded;
}

bool tuffpAvailable() {
    std::string ignored;
    return tuffpLoad(ignored);
}

std::string tuffpLibraryPath() {
    return tuffpAvailable() ? state().path : std::string();
}

TuffpResult calculateTuffp(const TuffpInput& input) {
    TuffpResult result;
    std::string message;
    if (!tuffpLoad(message)) {
        result.error = true;
        result.errorReport = message;
        return result;
    }

    using namespace tuffp;

    auto addIssue = [&result](const std::string& issue) {
        if (!result.errorReport.empty()) result.errorReport += "; ";
        result.errorReport += issue;
    };

    // TUFFP solves co-current flow in the positive direction only. Orient the
    // problem along the mixture velocity and drop a counter-current phase.
    const double direction = (input.vSG + input.vSL) < 0.0 ? -1.0 : 1.0;
    double vSG = direction * input.vSG;
    double vSL = direction * input.vSL;
    if (vSG < -TUFFP_ZERO_VELOCITY || vSL < -TUFFP_ZERO_VELOCITY) {
        addIssue("counter-current flow is outside the TUFFP model scope; the phase moving against "
                 "the mixture was neglected");
    }
    if (vSG < TUFFP_ZERO_VELOCITY) vSG = 0.0;
    if (vSL < TUFFP_ZERO_VELOCITY) vSL = 0.0;
    const double angleRad = direction * input.angle * PI / 180.0;

    if (vSG == 0.0 && vSL == 0.0) {
        // No flow: hydrostatic column (TUFFP returns NaN friction here).
        const double hl = std::fmin(1.0, std::fmax(0.0, input.noFlowHoldup));
        result.HL = hl;
        result.dpdL_grav = (hl * input.rhoL + (1.0 - hl) * input.rhoG) * GRAVITY *
                           std::sin(input.angle * PI / 180.0);
        result.dpdL_tot = result.dpdL_grav;
        if (hl >= 1.0 - 1.0e-5) result.patternCode = static_cast<int32_t>(Pattern::SinglePhaseLiquid);
        else if (hl <= 1.0e-5) result.patternCode = static_cast<int32_t>(Pattern::SinglePhaseGas);
        else result.patternCode = static_cast<int32_t>(Pattern::StratifiedSmooth);
        return result;
    }

    Psp1 psp{};
    psp.ang = angleRad;
    psp.dia = input.diameter;
    psp.rou = input.diameter > 0.0 ? input.roughness / input.diameter : 0.0;

    F1 gas{input.rhoG, 0.0, vSG, input.muG};
    F1 liq{input.rhoL, input.sigma, vSL, input.muL};

    // TUFFP default closures (see vendor examples in TUFFP_v2026.cpp).
    FilmM filmM{0, 0.75, 0, NOT_USED, {0, NOT_USED, 0}};
    SlugM slugM{0, 0, 0, 0, {NOT_USED, NOT_USED, 0}};

    Prg prg{};
    Film film{};
    FilmN filmN{};
    Slug slug{};
    SlugN slugN{};

    {
        std::lock_guard<std::mutex> lock(callMutex());
        state().fpGlUnified(&psp, &gas, &liq, &filmM, &slugM, &prg, &film, &filmN, &slug, &slugN);
    }

    // Gradients are returned along the pipe (geometric) direction.
    result.patternCode = prg.fpc;
    result.HL = prg.hlg;
    result.dpdL_acc = direction * prg.ace;
    result.dpdL_fric = direction * prg.fri;
    result.dpdL_grav = direction * prg.gra;
    result.dpdL_tot = direction * prg.tot;

    std::string report;
    appendLayer(report, "film gas", filmN.gas);
    appendLayer(report, "film liquid", filmN.liq);
    appendLayer(report, "film oil", filmN.oil);
    appendLayer(report, "slug mixture", slugN.mix);
    if (!report.empty()) addIssue("TUFFP numerical report: " + report);

    if (prg.fpc == static_cast<int32_t>(Pattern::NoPattern)) addIssue("no flow pattern predicted");
    if (!std::isfinite(prg.hlg) || !std::isfinite(prg.ace) || !std::isfinite(prg.fri) ||
        !std::isfinite(prg.gra) || !std::isfinite(prg.tot)) {
        addIssue("non-finite holdup or pressure gradient");
    }

    result.error = !result.errorReport.empty();
    if (result.error) {
        char inputs[320];
        std::snprintf(inputs, sizeof(inputs),
                      " [inputs: vSG=%.6g m/s, vSL=%.6g m/s, rhoG=%.6g, rhoL=%.6g kg/m3, "
                      "muG=%.6g, muL=%.6g Pa.s, sigma=%.6g N/m, D=%.6g m, eps=%.6g m, angle=%.6g deg]",
                      input.vSG, input.vSL, input.rhoG, input.rhoL, input.muG, input.muL,
                      input.sigma, input.diameter, input.roughness, input.angle);
        result.errorReport += inputs;
    }
    return result;
}
