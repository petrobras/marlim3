# Fluid Thermodynamic Modeling

This document describes the fluid property modeling framework in Marlim3 — from black-oil correlations through compositional EOS calculations, covering all PVT properties and phase equilibrium.

**Key source files:**

| File | Role |
|------|------|
| [`src/include/PropFlu.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFlu.h) / [`src/core/PropFlu.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFlu.cpp) | `ProFlu` class — main hydrocarbon fluid: black-oil, PVTSim tables, and compositional hooks |
| [`src/include/PropFluCol.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFluCol.h) / [`src/core/PropFluCol.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFluCol.cpp) | `ProFluCol` class — complementary fluid property model |
| [`src/include/PropFluColVF.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFluColVF.h) / [`src/core/PropFluColVF.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFluColVF.cpp) | `ProFluColVF` class — complementary-fluid model used by the VF formulation, with gas-viscosity and non-Newtonian viscosity support |
| [`src/include/PropVapor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropVapor.h) / [`src/core/PropVapor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropVapor.cpp) | `ProVap` class — pure water/steam table-based property package |
| [`src/fortran/MarlimComposicional.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/MarlimComposicional.f90) | Fortran compositional module — flash + all EOS property calculations |
| [`src/include/MarlimComposicional.h`](https://github.com/petrobras/marlim3/blob/main/src/include/MarlimComposicional.h) | C/Fortran interface (bind(C) declarations) |
| [`src/fortran/VLECalculations.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/VLECalculations.f90) | VLE flash algorithms, cubic EOS, fugacity coefficients |
| [`src/fortran/PhaseProperties.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/PhaseProperties.f90) | Density, enthalpy, Cp, surface tension from cubic EOS |
| [`src/fortran/FlashExtras.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/FlashExtras.f90) | Rigorous flash: SS + DEM + TPD stability + Newton |
| [`src/fortran/Newton.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/Newton.f90) | Newton 2nd-order minimiser for flash and stability |
| [`src/fortran/BlackOilModels.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/BlackOilModels.f90) | Black-oil Rs correlations in Fortran |
| [`src/fortran/BlackOilFromCompositional.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/BlackOilFromCompositional.f90) | Derive Rs, Bo, GOR from compositional flash results |
| [`src/fortran/ViscosityCalculationModule.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/ViscosityCalculationModule.f90) | Pedersen CSP viscosity model |
| [`src/fortran/Constants.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/Constants.f90) | Physical constants and enumerations |
| [`src/fortran/PvtSimCTMFileImport.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/PvtSimCTMFileImport.f90) | PVTSIM CTM fluid file parser |
| [`src/fortran/PvtSimWAXFileImport.f90`](https://github.com/petrobras/marlim3/blob/main/src/fortran/PvtSimWAXFileImport.f90) | PVTSIM WAX file parser |

---

## Table of Contents

1. [Overview — Three Fluid Modeling Modes](#overview--three-fluid-modeling-modes)
2. [The ProFlu Class](#the-proflu-class)
3. [Black-Oil Correlations](#black-oil-correlations)
   - [Solution Gas-Oil Ratio (Rs)](#solution-gas-oil-ratio-rs)
   - [Bubble Point Pressure (Pb)](#bubble-point-pressure-pb)
   - [Oil Formation Volume Factor (Bo)](#oil-formation-volume-factor-bo)
   - [Gas Compressibility Factor (Z)](#gas-compressibility-factor-z)
   - [Oil Viscosity](#oil-viscosity)
   - [Gas Viscosity](#gas-viscosity)
   - [Emulsion Viscosity](#emulsion-viscosity)
   - [Oil and Liquid Density](#oil-and-liquid-density)
   - [Gas Density](#gas-density)
   - [Surface Tension](#surface-tension)
   - [Specific Heat and Enthalpy](#specific-heat-and-enthalpy)
   - [Thermal Conductivity](#thermal-conductivity)
   - [Joule-Thomson Coefficients](#joule-thomson-coefficients)
   - [Gas Gravity Decomposition](#gas-gravity-decomposition)
4. [PVT Table System](#pvt-table-system)
5. [Compositional Modeling — Fortran EOS Library](#compositional-modeling--fortran-eos-library)
   - [Equations of State](#equations-of-state)
   - [Mixing Rules](#mixing-rules)
   - [Flash Algorithms](#flash-algorithms)
   - [Fugacity Coefficients](#fugacity-coefficients)
   - [Phase Density](#phase-density)
   - [Enthalpy and Heat Capacity](#enthalpy-and-heat-capacity)
   - [Surface Tension (Parachor Method)](#surface-tension-parachor-method)
   - [Viscosity — Pedersen CSP](#viscosity--pedersen-csp)
   - [Black-Oil Properties from Compositional Results](#black-oil-properties-from-compositional-results)
6. [C/Fortran Interface](#cfortran-interface)
7. [PVTSIM File Import](#pvtsim-file-import)
8. [Wax (Paraffin) Model](#wax-paraffin-model)
9. [Complementary Fluid — ProFluCol](#complementary-fluid--proflucol)
10. [Steam Properties — ProVap](#steam-properties--provap)
11. [Multiphase Gradient Correlations](#multiphase-gradient-correlations)
12. [Correlation Selector Enums](#correlation-selector-enums)
13. [Summary of Named Correlations](#summary-of-named-correlations)

---

<a id="overview--three-fluid-modeling-modes"></a>
## Overview — Three Fluid Modeling Modes

The `ProFlu` class supports three main modeling modes, controlled primarily by the `flashCompleto` flag:

| `flashCompleto` | Mode | Description |
|-----------------|------|-------------|
| 0 (or 3) | **Black-Oil** | Empirical correlations (Standing, Vasquez-Beggs, Lasater, etc.) for Rs, Bo, μ, ρ, Z |
| 1 | **PVTSim Tables** | Bilinear interpolation on pre-computed 2D (P × T) tables exported from PVTSIM |
| 2 | **Compositional** | Cubic EOS (PR78 or SRK with Peneloux volume shift) via Fortran routines |

This flag is the main dispatcher, but actual property evaluation is more nuanced than a single `if/else` split. In several methods the final path also depends on flags such as `tab`, `viscBlackOil`, `tabelaDinamica`, and transient-mode state (`modoTransiente`). In particular, some compositional paths can fall back to black-oil-style calculations or to precomputed dynamic tables.

---

<a id="the-proflu-class"></a>
## The ProFlu Class

`ProFlu` (defined in [`src/include/PropFlu.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFlu.h) / [`src/core/PropFlu.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFlu.cpp)) is the central fluid-property engine. Each simulation cell holds one `ProFlu` object (`flui`).

### Key member variables

**Fluid characterisation (from JSON input):**

| Variable | Units | Meaning |
|----------|-------|---------|
| `API` | °API | Oil API gravity |
| `RGO` | m³/m³ | Gas-oil ratio |
| `IRGO` | scf/bbl | Imperial RGO = `RGO × 35.31467 / 6.29` |
| `Deng` | — | Gas specific gravity (air = 1) |
| `BSW` | fraction | Basic sediment & water |
| `Denag` | — | Water specific gravity |
| `yco2` | mole fraction | CO₂ content |

**ASTM dead-oil viscosity:**

`TempL`, `LVisL`, `TempH`, `LVisH` — two (T, μ) pairs for ASTM D341 interpolation; `bASTM1`, `bASTM2` — precomputed slopes.

**Vasquez-Beggs parameters:**

`FAC`, `SG100vb` — separator gas gravity correction; `Avb`, `Bvb`, `Cvb` — V-B correlation coefficients (differ for API > 30 vs ≤ 30).

**Gas critical properties:**

`PC` (psia), `TC` (°R) — pseudocritical pressure and temperature at standard conditions; `PCis`, `TCis` — in-situ values after free-gas gravity correction.

**Gas gravity decomposition:**

`rDgD` — dissolved-gas / separator-gas gravity ratio; `rDgL` — free-gas / separator-gas gravity ratio.

**Compositional model data:**

`npseudo`, `fracMol[]`, `masMol[]`, `tempCrit[]`, `presCrit[]`, `fatAcent[]`, `TIndepPeneloux[]`, `kij[]`, `lij[]`, `CpIGCoefs[]`, `liqModel` — EOS parameters passed to Fortran routines.

### RenovaFluido()

Recalculates all derived constants when primary inputs change:

1. Recomputes `FAC`, `SG100vb`, `Avb`/`Bvb`/`Cvb` (API-dependent), applying the same methane-floor clamp `SG100vb >= 0.5538` used during construction
2. Converts `RGO` → `IRGO`
3. Recomputes gas critical properties `PC`, `TC` via selected correlation
4. Recalculates ASTM coefficients `bASTM1`, `bASTM2`
5. Computes stock-tank vapor mass fraction via `FracMass(1.0, 20.0)`

---

<a id="black-oil-correlations"></a>
## Black-Oil Correlations

### Solution Gas-Oil Ratio (Rs)

`ProFlu::RS(pres, temp)` — returns Rs in scf/bbl. Input: P (kgf/cm²), T (°C). Capped at `IRGO` above bubble point.

| `corrSat` | Correlation | Formula |
|-----------|-------------|---------|
| 0 | **Vasquez & Beggs (1980)** | $R_s = \dfrac{SG_{100} \cdot P^{A_{vb}}}{B_{vb}} \cdot 10^{C_{vb} \cdot \text{API} / T_R}$ |
| 1 | **Lasater (1958)** | Tabulated BPPF vs GMF vectors; inverts effective molecular weight from API |
| 2 | **Standing (1947)** | $R_s = \gamma_g \left[\left(1.4 + \frac{P}{18.2}\right) \cdot 10^A\right]^{1/0.83}$, $A = 0.0125 \cdot \text{API} - 0.00091 \cdot T_F$ |
| 3 | **Glasø (1980)** | Log-log: $P_b = 10^{(5.17967 - (0.60436 \cdot \log_{10} A^* - 1.7447)^2) / 1.20872}$, inverted for Rs |
| 4 | **Lívia (custom, C++ black-oil path)** | 18-coefficient parametric model with CO₂-corrected gas gravity: $R_{sr} = a_1 P_r^{a_2} + (1 - a_1) P_r^{a_3}$ |

For compositional mode, `RS()` may call `Marlim_CalculateSolutionGasOilRatio(...)`, use dynamic-table interpolation (`tabDin`), or use mini-table interpolation (`miniTabDin`) in transient updates, depending on solver state.

> **Implementation note:** the Fortran enum table in `src/fortran/Constants.f90` defines only four named Rs correlations (`Vazquez-Beggs`, `Lasater`, `Standing`, `Glaso`). The Lívia correlation exists in the C++ black-oil implementation and is not represented in that Fortran enum set.

### Bubble Point Pressure (Pb)

`ProFlu::PB(pres, temp)` — returns Pb in psia.

Each `corrSat` implements the corresponding Pb equation consistent with its Rs model. The Vasquez-Beggs form:

$$P_b = \left[\frac{B_{vb} \cdot \text{IRGO}}{SG_{100} \cdot 10^{C_{vb} \cdot \text{API} / T_R}}\right]^{1/A_{vb}}$$

For the Lívia branch (`corrSat == 4`), the bubble-point pressure calculation uses a **two-path dispatch** based on CO₂ content:

- **`yco2 < 0.1` (low CO₂):** falls back to the **Standing** bubble-point equation:
  $$P_b = 18.2\left[\left(\frac{\text{IRGO}}{\gamma_g}\right)^{0.83} \cdot 10^{-A} - 1.4\right], \quad A = 0.0125 \cdot \text{API} - 0.00091 \cdot T_F$$
  (or reads from the pre-loaded `tabRSPB` table if available)

- **`yco2 ≥ 0.1` (CO₂-rich):** uses the full Lívia Pb correlation with CO₂, N₂, and H₂S correction factors:
  $$P_b = 10^{1.7669 + 1.7447 \log_{10} P^* - 0.30218 (\log_{10} P^*)^2} \cdot f_{CO_2} \cdot f_{N_2} \cdot f_{H_2S}$$
  where $P^* = (GOR / \gamma_{g,HC})^{0.816} \cdot T_F^{0.172} / \text{API}^{0.989}$ and the correction factors are functions of the respective mole fractions.

> **Note:** currently `yN2 = 0` and `yH2S = 0` are hard-coded in the implementation (marked for future correction). The CO₂ threshold of 0.1 is a deliberate design choice — below it, the Standing equation is considered accurate enough and avoids potential numerical issues with small CO₂ values in the correction formula.

### Oil Formation Volume Factor (Bo)

`ProFlu::BOFunc(pres, temp)` — **Vasquez & Beggs** correlation:

**Below bubble point (saturated):**

$$B_o = 1 + 4.67 \times 10^{-4} R_s + A_{vb2} \cdot D_{vb} \times 10^{-4} + B_{vb2} \cdot R_s \cdot D_{vb} \times 10^{-8}$$

where $D_{vb} = (T_F - 60) \cdot \text{API} / SG_{100}$ and $A_{vb2}$, $B_{vb2}$ depend on API > 30.

**Above bubble point (undersaturated):**

$$B_o = B_{ob} \cdot \exp(-C_o \cdot (P_b - P))$$

$$C_o = \frac{-1433 + 5 \cdot \text{IRGO} + 17.2 \cdot T_F - 1180 \cdot SG_{100} + 12.61 \cdot \text{API}}{P \times 10^5}$$

**Zero-GOR oil (dead oil):** thermal expansion + compressibility model:

$$\rho_1 = \rho_{std} - \frac{624}{\rho_{std}} (T - 15.56), \quad B_o = \rho_{std} / \rho_{is}$$

For compositional mode: calls `Marlim_CalculateOilFormationVolumeFactor(...)`.

### Gas Compressibility Factor (Z)

Two correlations are available:

**Dranchuk-Abou-Kassem (DAK)** — `ZdranOriginal()`:

Solves the DAK EOS implicitly via the **Regula Falsi** method (`FalsaCorda()`). The residual function $G(\rho_r)$:

$$G = A\rho_r^6 + B\rho_r^3 + C\rho_r^2 + D\rho_r + E\rho_r^3(1 + f_d\rho_r^2)e^{-f_d\rho_r^2} - 0.27P_r$$

where $P_r = P / P_c$, $T_r = T / T_c$, $\rho_r$ is the reduced density, and $f_d = 0.68446549$ is the DAK density coefficient (named `Fdran` in the code).

**Gopal** — `ZGopal()`:

Piecewise polynomial using a 48-element coefficient array, partitioned by reduced pressure and temperature ranges.

The dispatcher `Zdran()` selects between direct calculation and table interpolation (when `tab == 1`). Analytical derivatives `DZDT()` and `DZDP()` are computed from implicit differentiation of the DAK equation, or interpolated from tables.

### Oil Viscosity

`ProFlu::ViscOleo(pres, temp)` — three-stage calculation:

**Stage 1 — Dead-oil viscosity** `VisOM(temp)`:

| `corrOM` | Correlation |
|----------|-------------|
| 0 | **ASTM D341** — $\mu_{od} = \rho_l \left[10^{10^{b \cdot \log_{10}(T_K/T_{L,K}) + b_2}} - 0.7\right]$ |
| 1 | **Beggs & Robinson** — $x = y \cdot T_F^{-1.163}$; $\mu_{od} = 10^x - 1$ where $y = e^{13.108 - 6.591/SG_o}$ |
| 2 | **Beggs & Robinson Modified** — $x = 10^z \cdot T_F^{-0.5644}$; $z = 1.8653 - 0.025086 \cdot \text{API}$ |
| 3 | **Glasø** |
| 4 | **Kartoatmodjo & Schmidt** |
| 5 | **Petrosky & Farshad** |
| 6 | **Beal** — $\mu_{od} = (0.32 + 1.8 \times 10^7 / \text{API}^{4.53}) \cdot (360/(T_F + 199.67))^z$ where $z = 10^{0.43 + 8.33/\text{API}}$ |
| 7 | **User Table** — interpolation using paired `viscTab[]` and `tempTab[]` entries from the same table rows |

**Stage 2 — Saturated (live) oil viscosity** (below Pb):

| `corrOV` | Correlation | Key form |
|----------|-------------|----------|
| 0 | **Beggs & Robinson** | $\mu_o = x \cdot \mu_{od}^y$; $x = 10.715/(R_s+100)^{0.515}$, $y = 5.439/(R_s+150)^{0.338}$ |
| 1 | **Kartoatmodjo & Schmidt** | $\mu_{sat} = -0.06821 + 0.9824 y' + 0.0004034 {y'}^2$ |
| 2 | **Petrosky & Farshad** | Coefficient-modified power law |

**Stage 3 — Undersaturated oil viscosity** (above Pb):

| `corrOS` | Correlation | Formula |
|----------|-------------|---------|
| 0 | **Vasquez & Beggs** | $\mu_o = \mu_{sat} \cdot (P/P_b)^m$ |
| 1 | **Kartoatmodjo & Schmidt** | Linear pressure correction |
| 2 | **Petrosky & Farshad** | Log-polynomial correction |
| 3 | **Beal** | $\mu_o = \mu_{sat} + 0.001(P - P_b)(x + y)$ |
| 4 | **Khan** | $\mu_o = \mu_{sat} \cdot e^{9.6 \times 10^{-5}(P - P_b)}$ |

Final viscosity is multiplied by `coefViscWax` (wax correction) and then the emulsion model is applied if `semEmul == 0`.

### Gas Viscosity

`ProFlu::ViscGas(pres, temp)` — **Lee-Gonzalez-Eakin** correlation:

$$K = \frac{(9.4 + 0.02 M_g) T_R^{1.5}}{209 + 19 M_g + T_R}$$

$$X = 3.5 + \frac{986}{T_R} + 0.01 M_g$$

$$Y = 2.4 - 0.2X$$

$$\mu_g = K \cdot \exp(X \cdot \rho_g^Y) / 10000$$

where $M_g = \gamma_{gL} \cdot \gamma_g \cdot 29$ is the pseudo-molecular-weight of the gas mixture used in the viscosity correlation, $\rho_g$ in g/cm³.

> **Note:** the gas viscosity correlation uses the rounded air MW of **29** g/mol for $M_g$, while gas density (`MasEspGas`) uses the more precise value **28.9625** g/mol. This is an intentional difference that preserves fidelity to the original Lee-Gonzalez-Eakin paper (which uses 29) while keeping the density calculation accurate.

### Emulsion Viscosity

`ProFlu::emul(pres, temp)` — returns a multiplier on oil viscosity:

| `tipoemul` | Model | Formula |
|------------|-------|---------|
| 1 | **Woelflin Weak** | $\text{mult} = (\rho_L/\rho_O) \cdot \exp((2.6672 f_w + 2.4740) f_w)$ |
| 2 | **Woelflin Medium** | $\text{mult} = (\rho_L/\rho_O) \cdot \exp((5.1920 f_w + 1.6691) f_w)$ |
| 3 | **Woelflin Strong** | $\text{mult} = (\rho_L/\rho_O) \cdot \exp((5.2456 f_w + 2.1102) f_w)$ |
| 4 | **Exponential (user)** | $\text{mult} = (\rho_L/\rho_O) \cdot \exp((a \cdot f_w + b) \cdot f_w)$ |
| 5 | **Pal & Rhodes** | $\text{mult} = (1 + r/(1.1884 - r))^{2.5}$ where $r = f_w / \Phi_{100}$ |
| 6 | **User table** | Piecewise-linear interpolation of `BSWVec[]` → `emulVec[]` |
| 7 | **Water regime** | Returns 1.0 (uses water viscosity above `bswCorte`) |

`bswCorte` is the emulsion inversion BSW. Below it, the emulsion multiplier applies; above it, linear mixing or pure water viscosity.

### Oil and Liquid Density

**Oil density** — `MasEspoleo(pres, temp)`:

$$\rho_o = \frac{\rho_{std} + r_{DgD} \cdot \gamma_g \cdot 1.225 \cdot R_s \cdot (6.29/35.31)}{B_o}$$

where $\rho_{std} = 141500 / (131.5 + \text{API})$ kg/m³.

**Liquid density** — `MasEspLiq(pres, temp)` — oil + water mixture:

$$\rho_L = \frac{m_{oil} + m_{water}}{(1 - \text{BSW}) \cdot B_o + \text{BSW} \cdot B_w}$$

where $m_{oil} = (1-\text{BSW})(\rho_{std} + r_{DgD} \gamma_g \cdot 1.225 \cdot R_s \cdot 6.29/35.31)$ and $m_{water} = \text{BSW} \cdot 1000 \cdot \gamma_w$.

> **Implementation note:** in compositional mode, the code does not simply return a pure hydrocarbon EOS liquid density. Depending on branch and solver state, it can blend EOS liquid density with water contribution through `BSW`, `MasEspAgua()`, and `BOFunc()`-based in-situ fraction logic.

### Gas Density

`MasEspGas(pres, temp)` — real-gas law:

$$\rho_g = \frac{\gamma_{gL} \cdot \gamma_g \cdot 28.9625 \cdot P \cdot 98066.5}{8046.5 \cdot Z \cdot (T + 273.15)}$$

### Surface Tension

`TensSuper()` — **Baker & Swerdloff** type approach:

1. Base oil surface tension: $\sigma_{68} = 39 - 0.2571 \cdot \text{API}$, $\sigma_{100} = 37.5 - 0.2571 \cdot \text{API}$
2. Temperature interpolation between 68 °F and 100 °F
3. Pressure correction via tabulated percentage-retained vs pressure curves
4. Water surface tension: interpolated between curves at 74 °F and 280 °F
5. Mixture: $\sigma_L = \sigma_o \cdot F_o + \sigma_w \cdot F_w$ (in-situ fraction weighting)

### Specific Heat and Enthalpy

**Liquid Cp** — `CalorLiqOriginal(pres, temp)`:

Oil Cp (Gambill-type):

$$C_{p,o} = 4187 \left[(2.6948 \times 10^{-6} \text{API} + 3.884 \times 10^{-4}) T_F + (2.7665 \times 10^{-3} \text{API} + 0.366) - 0.061\right]$$

Water Cp: polynomial in $T_K$ (split at 410 K):

$$C_{p,w} = 4185.5 \cdot f(T_K)$$

Mixed: $C_{p,L} = x_w \cdot C_{p,w} + (1 - x_w) \cdot C_{p,o}$

**Gas Cp** — `CalorGasOriginal(pres, temp)`:

Ideal-gas Cp from pseudo-composition (methane, ethane, propane, butane, CO₂ fractions estimated from gas gravity):

$$C_{p,0} = \frac{\sum y_i \cdot C_{p,0,i}(T)}{\gamma_g \cdot 28.9625}$$

Departure function from DAK EOS:

$$C_v = C_{v,0} - 5.1046 \cdot R \cdot \frac{\text{DAK correction}}{T_r^3}$$

$$C_p = C_v + R \cdot \frac{(Z + T_r \cdot \partial Z/\partial T_r)^2}{Z - P_r \cdot \partial Z/\partial P_r}$$

**Enthalpy** — `EntalpLiq()`, `EntalpGas()`:

For black-oil, path integrals via **Gauss-Legendre quadrature** (`quadleg`, `gauleg`):

$$H_L = \int_0^T C_{p,L}(P_{ref}, T') \, dT' + \int_{P_{ref}}^P \frac{1}{\rho_L} \, dP'$$

$$H_G = \int_0^T C_{p,G}(P_{ref}, T') \, dT' + \int_{P_{ref}}^P \frac{T(R/P)(Z + T \cdot \partial Z/\partial T) - 1/\rho_g}{\;} \, dP'$$

### Thermal Conductivity

- **Liquid** `CondLiq()`: oil $k_o = 0.1168(1 - 3(T_F-32)/10000)$ W/m·K; water from polynomial; mixed geometrically: $k_L = k_o (k_w/k_o)^{x_w}$
- **Gas** `CondGas()`: pressure-corrected correlation based on $T/191.1$ factor

### Joule-Thomson Coefficients

- **Liquid** `JTL(pres, temp)`: $\mu_{JT,L} = -(1/\rho + T \cdot d\rho/dT / \rho^2)$, mixed with water JT
- **Gas** `JTG(pres, temp)`: $\mu_{JT,G} = T \cdot (\partial Z/\partial T) / (Z \cdot \rho_g)$

### Gas Gravity Decomposition

`razDegD()` and `rzDegL()` split separator gas gravity into dissolved and free components:

$$SG_{DG} = \frac{\text{API} + 12.5}{50} - 3.5715 \times 10^{-6} \cdot \text{API} \cdot R_s$$

$$SG_{FG} = \gamma_g \cdot \frac{\text{RGO} - r_{DgD} \cdot R_s}{\text{RGO} - R_s}$$

`SG_{FG}` is clamped to $[0.5538, \gamma_g]$ (methane floor to separator-gas ceiling).

> **Implementation note:** the code also forces the dissolved-gas gravity estimate `SGDG` to be no lower than separator-gas gravity `Deng` before forming `rDgD`. This clamp is part of the implemented split logic.

**Pseudocritical properties** are available via three correlations (`corrC`): default Marlim2, CO₂-corrected (Wichert-Aziz style), and Stewart-Burkhardt-Voo style.

---

## PVT Table System

### PVTSim Flash Tables (`flashCompleto == 1`)

External PVT tables (exported from PVTSIM) are stored as 2D arrays `Var[i_pres][i_temp]`:

- Row 0 / Column 0 store the P and T axes
- `interpolaVarProd()` performs bilinear interpolation with accelerated index search using equally-spaced blocks (`pchange`/`dpchange`/`ipchange`)

Table arrays: `rholF`, `rhogF`, `DrholDpF`, `DrhogDpF`, `DrholDtF`, `DrhogDtF`, `RSF`, `cplF`, `cpgF`, `HlF`, `HgF`, `sigOGF`, `sigWGF`, `viscO`, `viscG`.

### Z-Factor Tables

When `tab == 1`, the Z-factor and its derivatives are stored in 2D reduced-coordinate tables (`zdranP`, `dzdpP`, `dzdtP`) for fast lookup instead of iterative DAK solving.

### Dynamic Tables (`tabelaDina tabDin`)

For compositional transient mode: precomputed uniform P-T grids with `npontosP × npontosT` points and fixed $\Delta P$, $\Delta T$ steps. Contains all properties (ρ_l, ρ_g, derivatives, Rs, Bo, Z, Hl, Hg, Cp, viscosities, Pb). Index arithmetic: `posicP = floor((P − P_{min}) / \Delta P)`.

### Mini Dynamic Tables (`miniTabelaDina miniTabDin`)

2 × 2 tables bracketing the current (P, T) state for fast transient-mode lookups.

---

<a id="compositional-modeling--fortran-eos-library"></a>
## Compositional Modeling — Fortran EOS Library

The compositional thermodynamic library is implemented in Fortran 90 modules called from C++ via `bind(C)` interfaces.

### Equations of State

Two cubic EOS are supported (selected from the PVTSIM CTM file):

**Peng-Robinson 78 with Peneloux volume shift:**

$$a_i = 0.45724 \frac{R^2 T_{c,i}^2}{P_{c,i}} \alpha_i(T)$$

$$b_i = 0.077796 \frac{R T_{c,i}}{P_{c,i}}$$

$$f(\omega) = \begin{cases} 0.37464 + 1.54226\omega - 0.26992\omega^2 & \omega \le 0.491 \\ 0.379642 + 1.48503\omega - 0.164423\omega^2 + 0.016666\omega^3 & \omega > 0.491 \end{cases}$$

Generalised form parameters: $u = 2$, $w = -1$.

**SRK with Peneloux volume shift:**

$$a_i = 0.42748 \frac{R^2 T_{c,i}^2}{P_{c,i}} \alpha_i(T)$$

$$b_i = 0.08664 \frac{R T_{c,i}}{P_{c,i}}$$

$$f(\omega) = 0.48 + 1.574\omega - 0.176\omega^2$$

Generalised form parameters: $u = 1$, $w = 0$.

Both EOS are solved in the generalised cubic form:

$$Z^3 + a_1 Z^2 + a_2 Z + a_3 = 0$$

via Cardano's analytical method (`CalculateZFromCubicEOS`), returning both $Z_L$ and $Z_V$.

### Mixing Rules

Classical van der Waals one-fluid rules with binary interaction parameters:

$$a_{mix} = \sum_i \sum_j x_i x_j \sqrt{a_i a_j} (1 - k_{ij})$$

$$b_{mix} = \frac{1}{2} \sum_i \sum_j x_i x_j (b_i + b_j)(1 - l_{ij})$$

$$c_{mix} = \sum_i x_i c_i \quad \text{(Peneloux volume shift, temperature-independent)}$$

### Flash Algorithms

The flash system has two tiers:

**Tier 1 — `CalcFlashPTVLE`** (basic PT flash):

1. Initialize K-factors from **Wilson correlation**: $K_i = \dfrac{P_{c,i}}{P} \exp\left[5.37(1 + \omega_i)\left(1 - \dfrac{T_{c,i}}{T}\right)\right]$
2. **Successive Substitution (SS)** loop with Rachford-Rice:
   - Solve $g(\beta) = \sum_i \dfrac{z_i(K_i - 1)}{1 + \beta(K_i - 1)} = 0$ using Newton's method with Michelsen bounds
   - Update K-factors from fugacity coefficients: $K_i = \hat{\phi}_i^L / \hat{\phi}_i^V$
3. Convergence via Mukherjee-Brill criterion: $\sum[(K_i x_i / y_i - 1)^2] \le 5 \times 10^{-6}$

**Tier 2 — `CalculateIsothermalPTVLEFlash`** (rigorous flash, reference: Michelsen & Mollerup, 2nd Ed.):

1. SS iterations as above
2. **DEM (Dominant Eigenvalue Method)** extrapolation applied periodically to accelerate convergence
3. **Gibbs energy monitoring** — tracks whether two-phase $G$ decreases vs single-phase $G$
4. **Tangent Plane Distance (TPD) stability analysis** when SS struggles (Gibbs energy not decreasing or $\beta$ out of bounds)
5. **Newton's method** fallback (`TryFlashCalcWith2ndOrderMinimizationIfNecessary`) — solves the flash as a 2nd-order minimisation problem if SS + DEM does not converge

**Bubble/dew point calculations** use the **secant method** (`SecantMethod_CalculateBubbleOrDewPressure`, max 50 iterations) with bracket search (`FindBubbleOrDewPRootInterval`).

**Pressure-stepping flash** (`CalcPressureStepwiseFlashPTVLE`): starts at low pressure (easy convergence) and advances stepwise to the target pressure using previous results as initial estimates.

### Fugacity Coefficients

Computed from the **Helmholtz free energy** formulation (Michelsen & Mollerup, pp. 63–64):

`CalculateLnFugAndDerivativesForEachComponent` computes the reduced Helmholtz function $F$ and all its derivatives ($\partial F/\partial n_i$, $\partial^2 F/\partial V^2$, $\partial^2 F/\partial n_i \partial V$, $\partial^2 F/\partial n_i \partial n_j$).

For $l_{ij} = 0$, uses the Reid-Prausnitz-Poling equation (4th ed., Table 5-13). For non-zero $l_{ij}$, uses a generalised UFRJ equation.

### Phase Density

From the cubic EOS Z-factor with Peneloux correction:

$$\rho = \frac{MW \cdot P}{(Z - c_{mix} P / RT) \cdot R \cdot T}$$

where $c_{mix}$ is the mixture Peneloux volume shift.

Derivatives $\partial \rho / \partial T$ and $\partial \rho / \partial P$ are computed via implicit differentiation of the cubic equation, including Peneloux effects (`CalculateCubicEOSTAndPDerivativesOfPhasicRhoAndZ`).

### Enthalpy and Heat Capacity

$$H = H^{ig}(T) + H^{res}(T, P)$$

$$C_p = C_p^{ig}(T) + C_p^{res}(T, P)$$

**Ideal-gas Cp** from PVTSIM polynomial:

$$C_p^{ig} = \sum_i x_i (a_{0,i} + a_{1,i} T + a_{2,i} T^2 + a_{3,i} T^3)$$

with reference $T = 273.15$ K.

**Residual enthalpy** from cubic EOS:

$$H^{res} = \frac{a - T(da/dT)}{b\sqrt{u^2 - 4w}} \ln\left(\frac{2Z + B(u - \delta)}{2Z + B(u + \delta)}\right) + RT(Z - 1)$$

**Residual Cp** derived from $d^2 a / dT^2$ and $dZ/dT$.

**Joule-Thomson coefficient:**

$$\mu_{JT} = \frac{RT^2}{P \cdot C_{p,mol}} \cdot \frac{\partial Z_{Peneloux}}{\partial T}$$

### Surface Tension (Parachor Method)

$$\sigma = \left[\sum_i \mathcal{P}_i (\rho_L^{mol} x_i - \rho_V^{mol} y_i)\right]^4 / 1000$$

where $\mathcal{P}_i$ is the parachor of component $i$.

<a id="viscosity--pedersen-csp"></a>
### Viscosity — Pedersen CSP

The **Pedersen Corresponding States Principle** method (references: Pedersen, Fredenslund & Thomassen, 1989; Pedersen et al., Chem. Eng. Sci., 1984):

1. **Mixture molecular weight** via Pedersen's method:
   $$MW_{mix} = MW_n + 1.304 \times 10^{-4}(MW_w^{2.303} - MW_n^{2.303})$$

2. **Pseudocritical properties** from double summation: $T_{c,mix}$, $P_{c,mix}$

3. **Reference conditions**: $P_{ref} = P \cdot P_{c0} / P_{c,mix}$, $T_{ref} = T \cdot T_{c0} / T_{c,mix}$

4. **Methane density** at reference conditions via McCarty BWR EOS (32-constant modified BWR, solved by secant method)

5. **Methane viscosity** at reference conditions using Hanley correlation (dilute gas + second virial + excess terms with Pedersen-Fredenslund blending)

6. **α-correction** for high-MW mixtures

7. **Final viscosity**:

$$\eta_{mix} = \eta_{CH_4}(T_{ref}', P_{ref}') \cdot \left(\frac{T_{c,mix}}{T_{c0}}\right)^{-1/6} \cdot \left(\frac{P_{c,mix}}{P_{c0}}\right)^{2/3} \cdot \sqrt{\frac{MW_{mix}}{MW_0}} \cdot \frac{\alpha_{mix}}{\alpha_0}$$

Reference fluid: methane ($T_{c0} = 190.564$ K, $P_{c0} = 45.39$ atm, $MW_0 = 16.0428$, $\rho_{c0} = 162.66$ kg/m³).

### Black-Oil Properties from Compositional Results

`BlackOilFromCompositional.f90` derives black-oil properties from flash results:

**Solution gas-oil ratio:**

$$R_s = \frac{\rho_{L,ST}}{\rho_{V,ST}} \cdot \frac{w_{v,ST} - w_v}{1 - w_{v,ST}}$$

**Oil formation volume factor:**

$$B_o = \frac{\rho_{L,ST}}{\rho_L} \cdot \frac{1 - w_v}{1 - w_{v,ST}}$$

**GOR:**

$$\text{GOR} = \frac{w_v}{1 - w_v} \cdot \frac{\rho_L}{\rho_V}$$

`AdjustGlobalCompositionForTargetGOR_Approach1` iteratively recombines stock-tank liquid and vapor compositions to achieve a target GOR (same approach as OLGA 7.3.3), using the secant method with max 200 iterations.

---

## C/Fortran Interface

All compositional routines use `bind(C, name="...")` in Fortran and are declared as `extern "C"` in [`src/include/MarlimComposicional.h`](https://github.com/petrobras/marlim3/blob/main/src/include/MarlimComposicional.h). Two C-interop structs are defined:

- **`ZRhoAndDerivs`** — 15 doubles: Z, Z_Peneloux, dZ/dT, dρ/dT, dZ/dP, dρ/dP for liquid and vapor, plus Peneloux-corrected derivatives
- **`InputForViscosity`** — 4-element array of PVTSIM CSP viscosity tuning parameters

C pointers (`type(c_ptr)`) are mapped to Fortran arrays via `c_f_pointer`. Matrices are transposed where needed for C row-major vs Fortran column-major layout.

The main entry point is `MarlimComposicionalProps(P, T, NComp, z, MW, Tc, Pc, ω, Kij, Lij, Peneloux, IGcoefs, parachors, models...) → β, x, y, ρ_L, ρ_V, H_L, H_V, Cp_L, Cp_V, σ, Z_V, μ_JT, ZRhoAndDerivs, IER` which performs a flash + all property calculations at a given (P, T). It handles zeroed-component removal/reinsertion to avoid numerical singularities.

---

## PVTSIM File Import

### CTM Files — `PvtSimCTMFileImport.f90`

Parses PVTSIM CTM (Compositional Tracking Model) export files. Tag-based text format with comma-separated data lines.

Data read per component:
- Short name, mole fractions, acentric factors, critical T and P, molecular weights, parachors
- Binary interaction parameters ($k_{ij}$ for PR and SRK — reads both, uses the appropriate one)
- Peneloux volume shifts (converted from $c/R$ (K/atm) to m³/mol)
- Ideal gas Cp polynomial coefficients (converted from $C_p/R$ to J/(mol·K))
- EOS type: recognises "PR78 Peneloux" or "SRK"
- CSP viscosity tuning parameters (4 values)

### WAX Files — `PvtSimWAXFileImport.f90`

Reads PVTSIM WAX parameter files for wax deposition calculations:
- Wax component count, names, molecular weights, liquid densities, heats of melting, feed concentrations
- Pressure points, cloud point temperatures
- Structure tables (P × T): densities, liquid MW, Cp of wax, thermal conductivity, wax concentrations
- Oil-wax viscosity multipliers (D, E, F)

---

## Wax (Paraffin) Model

When `modoParafina == 1`, `ProFlu::atualizaPropParafina(P, T)` calls the Fortran routine `Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation(...)`. Outputs: cloud point temperature, wax concentration, Cp of wax, thermal conductivity, and density. The oil viscosity is corrected via the `coefViscWax` multiplier.

---

<a id="complementary-fluid--proflucol"></a>
## Complementary Fluid — ProFluCol

`ProFluCol` ([`src/include/PropFluCol.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFluCol.h) / [`src/core/PropFluCol.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFluCol.cpp)) models the **complementary fluid** used by the simplified flow model. In practice it is a property model/holder for an additional liquid or injected fluid, not a full thermodynamic package with phase equilibrium or dissolution into the hydrocarbon system.

Examples of use in the code include completion/injection fluids, water-like fluids, and fluids represented by imported property tables.

The dispatch is controlled not only by `tipoF`, but also by `injPoc` and `descarga`:

- **User-defined fluid branch**: `(injPoc <= 1 || injPoc > 3) && descarga == 0`
- **Water-like correlation branch**: `injPoc == 2 || descarga == 1 || tipoF == 1`
- **Interpolated table branch**: typically `injPoc == 3`

The `tipoF` flag is therefore only part of the selection logic:

| `tipoF` | Meaning | Notes |
|---------|---------|-------|
| 0 | User-defined / generic complementary fluid | Combined with `injPoc` and `descarga` logic |
| 1 | Water-like handling | Uses built-in water correlations |

For CO₂-rich injection and similar imported cases, properties are obtained from 2D interpolation tables through the `injPoc == 3` path rather than from a dedicated `tipoF` mode.

Each cell stores one `ProFluCol` object as `fluicol`.

### Water mode

| Property | Correlation |
|----------|-------------|
| Density | Formation volume factor: $B_w = 1 + 1.2 \times 10^{-4}(T_F - 60) + 10^{-6}(T_F - 60)^2 - 3.33 \times 10^{-6} P_{psi}$, with salt mixing |
| Viscosity | Arrhenius: $\mu_w = 2.414 \times 10^{-5} \cdot 10^{247.8/(T + 133.15)} \times 1000$ (cP) |
| Cp | Polynomial in $T_K$, piecewise at 410 K |
| Conductivity | $k = 0.565 + (1.75 \times 10^{-3} - 6.21 \times 10^{-6} T) T$ |
| Surface tension | Defaults to $0.072\ \mathrm{N/m}$ at $25\,^\circ\mathrm{C}$ and can be overridden with `fluidoComplementar.tensup` |

### CO₂ injection mode (`injPoc == 3`)

All properties from bilinear interpolation of 2D (P × T) tables imported from PVTSIM.

### Drag reduction

`calcDR(Re)`: $\Delta R_0 = \sqrt{(0.025 \cdot Re - 704.156)} / 100$ for $Re \ge 28200$, with exponential decay over residence time.

### ProFluColVF

`ProFluColVF` is the extended complementary-fluid class used by the VF formulation. Relative to `ProFluCol`, it adds:
- `VisGas()` — Lee-Gonzalez-Eakin-style gas-viscosity calculation using stored gas-density data
- non-Newtonian viscosity support through bilinear interpolation on a `viscNaoNew[temp][deform]` table
- auxiliary gas-density fields (`rhogStd`, `rhog`)

It is best understood as a VF-oriented extension of the complementary-fluid property model, rather than as a separate full vapor-fraction thermodynamic package.

---

<a id="steam-properties--provap"></a>
## Steam Properties — ProVap

`ProVap` ([`src/include/PropVapor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropVapor.h) / [`src/core/PropVapor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropVapor.cpp)) is a thermodynamic package for **pure water/steam**, independent of `ProFlu`. It supports compressed liquid, two-phase, and superheated-vapour regions through interpolation on JSON-format steam tables.

### Data tables (loaded by `LerArqVap()`)

| File | Contents |
|------|----------|
| `VaporSat.json` | Saturation curve: P, T, $v_l$, $v_v$, $u_l$, $u_v$, $h_l$, $h_{lv}$, $h_v$, $s_l$, $s_v$, σ |
| `VaporSupAq.json` | Superheated vapour: per-pressure arrays of (T, $v_v$, $u_v$, $h_v$, $s_v$) |
| `AguaComp.json` | Compressed liquid: per-pressure arrays of (T, $v_l$, $u_l$, $h_l$, $s_l$) |
| Conductivity/viscosity files | Separate tables for liquid and gas |

This is a **table-based** water/steam package. It does not implement IAPWS equations directly in the code shown here.

### Phase determination (`verificaFase`)

- Critical point: 220.9 bar, 374.1 °C
- Classifies as subcooled liquid (`fase = 0`), superheated vapour (`fase = 1`), or two-phase (`fase = 2`)
- Supports three input modes: temperature, internal energy, or enthalpy

### Properties

All primary properties are obtained by binary search + interpolation in the loaded tables. Several derived quantities and some thermodynamic derivatives are then evaluated numerically, commonly with finite-difference perturbations of about 1% in the local routines.

Derived quantities include: $C_v$, $C_p$, isentropic exponent $\kappa$, Joule-Thomson coefficient, steam quality $x = (h - h_l)/(h_v - h_l)$, mixture energy density $(1-\alpha)\rho_l u_l + \alpha \rho_g u_g$.

## Correlation Selector Enums

| Variable | Controls | Values |
|----------|----------|--------|
| `corrSat` | Rs / Pb | 0 = Vasquez-Beggs, 1 = Lasater, 2 = Standing, 3 = Glasø, 4 = Lívia *(C++ black-oil path; not represented in the Fortran Rs enum constants)* |
| `corrOM` | Dead-oil μ | 0 = ASTM, 1 = Beggs-Robinson, 2 = B-R Modified, 3 = Glasø, 4 = Kartoatmodjo-Schmidt, 5 = Petrosky-Farshad, 6 = Beal, 7 = User table |
| `corrOV` | Live-oil μ | 0 = Beggs-Robinson, 1 = Kartoatmodjo-Schmidt, 2 = Petrosky-Farshad |
| `corrOS` | Undersaturated μ | 0 = Vasquez-Beggs, 1 = Kartoatmodjo-Schmidt, 2 = Petrosky-Farshad, 3 = Beal, 4 = Khan |
| `corrC` | Gas Pc/Tc | 0 = default, 1 = CO₂-corrected (Wichert-Aziz style), 2 = Stewart-Burkhardt-Voo style |
| `tipoemul` | Emulsion μ | 0 = linear, 1–3 = Woelflin (weak/medium/strong), 4 = exponential, 5 = Pal-Rhodes, 6 = user table, 7 = water regime |

> **Historical note:** one of the `ProFlu` constructors previously contained the undocumented assignment `if(corrSat==1) corrSat=4;`, which silently redirected requests for the Lasater correlation to the Lívia model. This remap was inconsistent with the other constructors and has been removed. All three constructors now honour `corrSat==1` as Lasater.

---

<a id="multiphase-gradient-correlations"></a>
## Summary of Named Correlations

| Property | Authors / Methods |
|----------|-------------------|
| Rs / Pb | Vasquez-Beggs, Lasater, Standing, Glasø, Lívia *(Lívia implemented in the C++ black-oil branch)* |
| Bo | Vasquez-Beggs (saturated + undersaturated) |
| Dead-oil μ | ASTM D341, Beggs-Robinson, B-R Modified, Glasø, Kartoatmodjo-Schmidt, Petrosky-Farshad, Beal |
| Live-oil μ | Beggs-Robinson, Kartoatmodjo-Schmidt, Petrosky-Farshad |
| Undersaturated-oil μ | Vasquez-Beggs, Kartoatmodjo-Schmidt, Petrosky-Farshad, Beal, Khan |
| Z-factor | Dranchuk-Abou-Kassem (DAK), Gopal |
| Gas μ | Lee-Gonzalez-Eakin |
| Emulsion μ | Woelflin (3 grades), Pal-Rhodes |
| Surface tension | Baker-Swerdloff type (oil), parachor (compositional) |
| Gas Pc / Tc | Default, Wichert-Aziz style, Stewart-Burkhardt-Voo style |
| Water μ | Arrhenius exponential |
| Water ρ | Formation volume factor (McCain-type) |
| EOS | Peng-Robinson 78 + Peneloux, SRK + Peneloux |
| Flash | Rachford-Rice + Successive Substitution + DEM + TPD + Newton (Michelsen & Mollerup) |
| Compositional μ | Pedersen CSP with McCarty BWR methane reference |
| Liquid Cp | Gambill (oil), polynomial (water) |
| Gas Cp | Pseudo-composition ideal + DAK departure |
| Enthalpy | Gauss-Legendre quadrature (black-oil), cubic EOS residual (compositional) |
| Gradient correlations | Poettmann-Carpenter, Baxendell-Thomas, Fancher-Brown, Hagedorn-Brown, Duns-Ros, Orkiszewski, Beggs-Brill, Mukherjee-Brill, Aziz-Govier-Fogarasi, Gray, Oliemans, Dukler |
| Friction factor | Colebrook-White (Aitken-accelerated) |
