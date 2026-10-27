# Heat Transfer Modeling

This document describes the heat transfer subsystem in Marlim3 — from the radial thermal resistance network through convection correlations, formation conduction, 2D/3D soil diffusion, and the coupling to the axial energy equation.

**Key source files:**

| File | Role |
|------|------|
| [`src/TrocaCalor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/TrocaCalor.h) / [`TrocaCalor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/TrocaCalor.cpp) | `TransCal` class — radial heat transfer (1D and 2D), convection correlations |
| [`src/Geometria.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Geometria.h) | `DadosGeo` — pipe geometry and wall layer definitions |
| [`src/celula3.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celula3.h) | `Cel` — multiphase cell thermal members (temperature, advection, heat source) |
| [`src/celulaGas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celulaGas.h) | `CelG` — gas-lift cell thermal members |
| [`src/celulaVapor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celulaVapor.h) | `CelVap` — steam cell thermal members |
| [`src/Elem2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem2DPoisson.h) / [`Elem2DPoisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Elem2DPoisson.cpp) | 2D FVM element for buried-pipe soil conduction |
| [`src/Malha2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha2DPoisson.h) / [`Malha2DPoisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Malha2DPoisson.cpp) | 2D unstructured triangular mesh |
| [`src/dados1Poisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/dados1Poisson.h) / [`dados1Poisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/dados1Poisson.cpp) | 2D Poisson solver data and I/O |
| [`src/estruturasPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturasPoisson.h) | Structures for 2D Poisson elements |
| [`src/estruturasPoisson3D.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturasPoisson3D.h) | Structures for 3D Poisson elements |
| [`src/dados3DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/dados3DPoisson.h) / [`dados3DPoisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/dados3DPoisson.cpp) | 3D Poisson solver data |
| [`src/Elem3DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem3DPoisson.h) / [`Elem3DPoisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Elem3DPoisson.cpp) | 3D FVM element |
| [`src/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) | Axial energy equation and coupling loop |

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [The TransCal Class](#the-transcal-class)
   - [Geometry — DadosGeo](#geometry--dadosgeo)
   - [Temperature and Heat Flux Fields](#temperature-and-heat-flux-fields)
   - [Internal Fluid Properties](#internal-fluid-properties)
   - [External Fluid Properties](#external-fluid-properties)
   - [Control Flags](#control-flags)
   - [Formation / Wellbore Parameters](#formation--wellbore-parameters)
   - [Gas Properties for Annulus](#gas-properties-for-annulus)
3. [Dimensionless Numbers](#dimensionless-numbers)
4. [Convection Correlations](#convection-correlations)
   - [Internal Forced Convection — Petukhov-Gnielinski](#internal-forced-convection--petukhov-gnielinski)
   - [External Cross-Flow — Churchill-Bernstein](#external-cross-flow--churchill-bernstein)
   - [External Natural Convection — Churchill-Chu](#external-natural-convection--churchill-chu)
   - [Confined Natural Convection (Annulus) — Hollands / Catton](#confined-natural-convection-annulus--hollands--catton)
   - [Mixed Convection Regime](#mixed-convection-regime)
5. [Friction Factor — Haaland](#friction-factor--haaland)
6. [Formation Thermal Resistance — Ramey](#formation-thermal-resistance--ramey)
7. [Ambient Fluid Properties](#ambient-fluid-properties)
   - [Seawater](#seawater)
   - [Air](#air)
   - [Gas / Nitrogen (Annulus)](#gas--nitrogen-annulus)
   - [Thermal Expansion Coefficient](#thermal-expansion-coefficient)
8. [Radial Resistance Network](#radial-resistance-network)
   - [Layer Resistance Types](#layer-resistance-types)
   - [Internal Convection Coefficient — hInt()](#internal-convection-coefficient--hint)
   - [External Convection Coefficient — hExt()](#external-convection-coefficient--hext)
   - [Wall Conductance — condParede()](#wall-conductance--condparede)
9. [Steady-State Solver — transperm()](#steady-state-solver--transperm)
10. [Transient Solver — transtrans()](#transient-solver--transtrans)
    - [Radial Discretization](#radial-discretization)
    - [Local Element Matrix — transcel()](#local-element-matrix--transcel)
    - [Global Assembly and Solve](#global-assembly-and-solve)
11. [Axial Energy Equation](#axial-energy-equation)
    - [Energy Balance Terms](#energy-balance-terms)
    - [Coupling: Radial Heat Flux → Axial Solver](#coupling-radial-heat-flux--axial-solver)
12. [Thermal Members in Cell Classes](#thermal-members-in-cell-classes)
13. [2D Buried-Pipe Solver (Poisson)](#2d-buried-pipe-solver-poisson)
    - [Mesh and Elements](#mesh-and-elements)
    - [Boundary Conditions](#boundary-conditions)
    - [Assembly — GeraLocal()](#assembly--geralocal)
    - [Gradient Reconstruction — Green-Gauss](#gradient-reconstruction--green-gauss)
    - [Linear Solve](#linear-solve)
    - [Coupling to 1D Flow](#coupling-to-1d-flow)
14. [3D Soil Diffusion Solver](#3d-soil-diffusion-solver)
15. [Special Configurations](#special-configurations)
    - [Subsea Pipelines](#subsea-pipelines)
    - [Risers and Wellbores](#risers-and-wellbores)
    - [Buried / Trenched Pipelines](#buried--trenched-pipelines)
    - [Lined / Annular Pipes](#lined--annular-pipes)
    - [Annulus with Forced Flow](#annulus-with-forced-flow)
    - [Column Configuration](#column-configuration)
    - [Prescribed Wall Temperature](#prescribed-wall-temperature)
    - [Wax Deposition Layer](#wax-deposition-layer)
16. [Summary of Named Correlations](#summary-of-named-correlations)

---

## Architecture Overview

Each 1D flow cell (`Cel`, `CelG`, `CelVap`) owns a `TransCal calor` object that computes radial heat exchange between the flowing fluid and the environment through a multi-layered pipe wall. The radial heat flux feeds into the axial energy equation as a source term.

```
                        ┌──────────────────────────────────────────────┐
  Axial energy          │  TransCal  (radial heat transfer)            │
  equation in           │                                              │
  SisProd.cpp           │  Fluid ──h_i──[Layer 0]──[Layer 1]──...     │
       │                │    (Tint)      steel    insulation           │
       │ fluxcal        │                  ...──[Layer N-1]──h_e──Env  │
       ◄────────────────┤                                   (Textern) │
       │                │  or: 2D Poisson soil solver (buried pipe)    │
       │                │  or: 3D Poisson soil solver                  │
       ▼                └──────────────────────────────────────────────┘
  T^{n+1} update
```

Three radial heat transfer modes:

| `difus2D` / `modoDifus3D` | Mode | Implementation |
|---------------------------|------|----------------|
| 0 / 0 | **1D Radial** | Concentric cylindrical resistances — `transperm()` / `transtrans()` |
| 1 / 0 | **2D Poisson** | Unstructured FVM for soil around buried pipe — `transperm2D()` / `transtrans2D()` |
| — / 1 | **3D Poisson** | Full 3D FVM soil conduction — `solverP3D` |

---

## The TransCal Class

`TransCal` (defined in [`TrocaCalor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/TrocaCalor.h) / [`TrocaCalor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/TrocaCalor.cpp)) is the central radial heat transfer engine.

<a id="geometry--dadosgeo"></a>
### Geometry — DadosGeo

The `DadosGeo` structure ([`Geometria.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Geometria.h)) describes the pipe cross-section:

| Member | Type | Units | Description |
|--------|------|-------|-------------|
| `a` | `double` | m | Inner diameter (or inner annulus OD for lined pipe) |
| `b` | `double` | m | Inner annulus ID (for lined pipe, `revest=1`) |
| `dia` | `double` | m | Hydraulic diameter |
| `teta` | `double` | rad | Pipe inclination |
| `rug` | `double` | m | Surface roughness |
| `area` | `double` | m² | Cross-sectional flow area |
| `peri` | `double` | m | Wetted perimeter |
| `ncamadas` | `int` | — | Number of wall layers |
| `cond[i]` | `double*` | W/(m·K) | Thermal conductivity of layer $i$ |
| `diamC[i]` | `double*` | m | Outer diameter of layer $i$ |
| `espessuR[i]` | `double*` | m | Thickness of layer $i$ |
| `cp[i]` | `double*` | J/(kg·K) | Specific heat of layer $i$ |
| `rhoC[i]` | `double*` | kg/m³ | Density of layer $i$ |
| `tipomat[i]` | `int*` | — | **0** = solid (conduction), **2** = liquid (convection), **3** = gas (convection) |
| `revest` | `int` | — | 0 = circular pipe, 1 = annular (lined) pipe |

<a id="temperature-and-heat-flux-fields"></a>
### Temperature and Heat Flux Fields

| Member | Type | Units | Description |
|--------|------|-------|-------------|
| `Tcamada[i][j]` | `double**` | °C | Temperature at node $j$ of layer $i$ — current step |
| `Tini[i][j]` | `double**` | °C | Temperature — previous time step |
| `Qcamada[i][j]` | `double**` | W/m | Heat flux at node $j$ of layer $i$ — current step |
| `Qini[i][j]` | `double**` | W/m | Heat flux — previous time step |
| `Tint`, `Tint2` | `double` | °C | Internal fluid temperature(s) |
| `Textern1`, `Textern2` | `double` | °C | External environment temperature(s) |
| `fluxIni` | `double` | W/m | Heat flux at inner wall |
| `fluxFim` | `double` | W/m | Heat flux at outer wall |
| `resGlob` | `double` | m·K/W | Overall thermal resistance |

<a id="internal-fluid-properties"></a>
### Internal Fluid Properties

| Member | Type | Units | Description |
|--------|------|-------|-------------|
| `kint` | `double` | W/(m·K) | Thermal conductivity |
| `cpint` | `double` | J/(kg·K) | Specific heat |
| `rhoint` | `double` | kg/m³ | Density |
| `viscint` | `double` | Pa·s | Dynamic viscosity |
| `betint` | `double` | 1/K | Thermal expansion coefficient |
| `Vint` | `double` | m/s | Flow velocity |

<a id="external-fluid-properties"></a>
### External Fluid Properties

Two sets of external properties are stored (primary and secondary):

| Member | Units | Description |
|--------|-------|-------------|
| `kextern1`, `kextern2` | W/(m·K) | Conductivity |
| `cpextern1`, `cpextern2` | J/(kg·K) | Specific heat |
| `rhoextern1`, `rhoextern2` | kg/m³ | Density |
| `viscextern1`, `viscextern2` | Pa·s | Viscosity |
| `betext` | 1/K | External expansion coefficient |
| `Vextern1` | m/s | External flow velocity |
| `Vconf` | m/s | Annulus flow velocity |

<a id="control-flags"></a>
### Control Flags

| Flag | Type | Values |
|------|------|--------|
| `permanente` | `int` | 1 = steady-state, 0 = transient |
| `dirconvExt` | `int` | **0** = cross-flow (Churchill-Bernstein), **1** = parallel flow (Petukhov) |
| `ambext` | `int` | **0** = user-specified, **1** = seawater (auto-properties), **2** = air (auto-properties) |
| `formacPoc` | `int` | **0** = pipe in fluid, **1** = wellbore in rock formation |
| `condiTparede` | `int` | **0** = convective inner BC, **1** = prescribed wall temperature |
| `novoHi` | `double` | User-override for internal $h$ (applied if > 0) |
| `difus2D` | `int` | **0** = 1D radial, **1** = 2D Poisson solver (buried pipe) |
| `npet` | `double` | Petukhov viscosity-ratio exponent (0.25 cooling, 0.11 heating) |

<a id="formation--wellbore-parameters"></a>
### Formation / Wellbore Parameters

| Member | Type | Units | Description |
|--------|------|-------|-------------|
| `tempprod` | `double` | days | Production time |
| `condform` | `double` | W/(m·K) | Formation thermal conductivity |
| `cpform` | `double` | J/(kg·K) | Formation specific heat |
| `rhoform` | `double` | kg/m³ | Formation density |
| `resFim` | `double` | m·K/W | Computed formation resistance |

<a id="gas-properties-for-annulus"></a>
### Gas Properties for Annulus

| Member | Value / Type | Description |
|--------|-------------|-------------|
| `airMW` | 28.97 | Air/gas molecular weight |
| `RGas` | 8314.4621 | Universal gas constant [J/(kmol·K)] |
| `pressao` | `double` | System pressure [kgf/cm²] |
| `TCNitro` | `double` | N₂ critical temperature [°R] |
| `PCNitro` | `double` | N₂ critical pressure [psia] |
| `CoefGopalHT[48]` | `constexpr` | Gopal Z-factor coefficients for gas density |

---

## Dimensionless Numbers

All dimensionless numbers are computed as member methods of `TransCal`:

**Grashof** — `Grash(ΔT, β, ν, L)`:

$$\mathrm{Gr} = \frac{g \, \beta \, L^3 \, \Delta T}{\nu^2}, \quad g = 9.82\;\text{m/s}^2$$

**Prandtl** — `Pr(ν, α)`:

$$\mathrm{Pr} = \frac{\nu}{\alpha}$$

**Rayleigh** — `Ra(Gr, Pr)`:

$$\mathrm{Ra} = \mathrm{Gr} \cdot \mathrm{Pr}$$

**Internal Rayleigh** — `RaInt(ΔT, β, ν, α)`:

$$\mathrm{Ra}_i = \frac{g \, \beta \, (0.2854 \, D)^3 \, \Delta T}{\nu \, \alpha}$$

The factor 0.2854 converts pipe diameter to the appropriate length scale.

**External Rayleigh** — `RaExt(ΔT, β, ν, α)`:

$$\mathrm{Ra}_e = \frac{g \, \beta \, (D_{\text{ext}} |\cos\theta|)^3 \, \Delta T}{\nu \, \alpha}$$

where $\theta$ is the pipe inclination. The projected length $D_{\text{ext}} |\cos\theta|$ accounts for inclination effects on buoyancy-driven flow.

**Reynolds** — `ReyIn(V, μ, ρ, D)`:

$$\mathrm{Re} = \frac{D \, \rho \, |V|}{\mu}$$

### Stored Dimensionless Numbers

| Member | Description |
|--------|-------------|
| `reyi`, `reye` | Reynolds — internal, external |
| `grashi`, `grashe` | Grashof — internal, external |
| `nusi`, `nuse` | Nusselt — internal, external |
| `hi`, `he` | Heat transfer coefficient — internal, external [W/(m²·K)] |
| `pri`, `pre` | Prandtl — internal, external |
| `prG`, `prL` | Prandtl — gas, liquid phases |

---

## Convection Correlations

<a id="internal-forced-convection--petukhov-gnielinski"></a>
### Internal Forced Convection — Petukhov-Gnielinski

`nussPet(Re, Pr, ε, μ_b, μ_w)` — primary internal convection correlation.

**Turbulent flow** ($\mathrm{Re} > 2400$) with moderate viscosity ratio ($\mu_w / \mu_b < 40$):

$$\mathrm{Nu} = \frac{\mathrm{Re} \cdot \mathrm{Pr}}{1.07 + 12.7\left(\mathrm{Pr}^{2/3} - 1\right)\sqrt{f/8}} \cdot \frac{f}{8} \cdot \left(\frac{\mu_b}{\mu_w}\right)^n$$

where $f$ is the Haaland friction factor and $n$ is the viscosity-ratio exponent:
- $n = 0.25$ for cooling ($T_{\text{wall}} < T_{\text{fluid}}$)
- $n = 0.11$ for heating ($T_{\text{wall}} > T_{\text{fluid}}$)

The exponent $n$ is set by `definePet()`.

**Sleicher-Rouse** for high viscosity ratio ($\mu_w / \mu_b \ge 40$):

$$\mathrm{Nu} = 5 + 0.016 \, \mathrm{Re}^a \, \mathrm{Pr}^b$$

$$a = 0.88 - \frac{0.24}{4 + \mathrm{Pr}}, \quad b = 0.33 + 0.5 \, e^{-0.6 \, \mathrm{Pr}}$$

**Laminar** ($\mathrm{Re} \le 2400$):

$$\mathrm{Nu} = 3.6$$

<a id="external-cross-flow--churchill-bernstein"></a>
### External Cross-Flow — Churchill-Bernstein

`nussChuBer(Re, Pr)` — external convection over a cylinder in cross-flow.

**Churchill-Bernstein (1977):**

$$\mathrm{Nu} = 0.3 + \frac{0.62\sqrt{\mathrm{Re}}\;\mathrm{Pr}^{1/3}}{\left[1 + (0.4/\mathrm{Pr})^{2/3}\right]^{1/4}} \cdot \left[1 + \left(\frac{\mathrm{Re}}{282000}\right)^{5/8}\right]^{4/5}$$

A variant exponent of 1/2 is used for $20000 < \mathrm{Re} < 400000$.

<a id="external-natural-convection--churchill-chu"></a>
### External Natural Convection — Churchill-Chu

`nussNatExt(Ra, Pr)` — free convection from an isothermal cylinder.

**Churchill-Chu (1975):**

$$\mathrm{Nu} = \left[0.6 + \frac{0.387 \, \mathrm{Ra}^{1/6}}{\left(1 + (0.559/\mathrm{Pr})^{9/16}\right)^{8/27}}\right]^2$$

<a id="confined-natural-convection-annulus--hollands--catton"></a>
### Confined Natural Convection (Annulus) — Hollands / Catton

`NussConf2(Ra, H, δ, θ)` — natural convection in an inclined annular gap. Three angle regimes:

**For inclination $|\theta| < 60°$** — Hollands et al. (1976):

$$\mathrm{Nu} = 1 + 1.44\left[1 - \frac{1708}{\mathrm{Ra}\cos\theta}\right]^+\!\left[1 - \frac{1708\sin^{1.6}(1.8|\theta|)}{\mathrm{Ra}\cos\theta}\right] + \left[\left(\frac{\mathrm{Ra}\cos\theta}{5830}\right)^{1/3} - 1\right]^+$$

where $[\;]^+$ denotes $\max(0, \cdot)$.

**For $60° \le |\theta| < 90°$** — linear interpolation:

$$\mathrm{Nu} = \frac{(\pi/2 - |\theta|)\,\mathrm{Nu}_{60} + (|\theta| - \pi/3)\,\mathrm{Nu}_{90}}{\pi/6}$$

**For $|\theta| = 90°$ (vertical)** — maximum of three Catton sub-correlations:

$$\mathrm{Nu}_{90} = \max\!\left(0.0605\,\mathrm{Ra}^{1/3},\;\left[1 + \left(\frac{0.104\,\mathrm{Ra}^{0.293}}{1+(6310/\mathrm{Ra})^{1.36}}\right)^3\right]^{1/3},\;0.242\left(\frac{\mathrm{Ra}}{A}\right)^{0.272}\right)$$

**`defineConf(Pr, Ra)`** sets a simpler power-law fallback $\mathrm{Nu} = C \cdot \mathrm{Ra}^n \cdot (H/\delta)^m$:
- $\mathrm{Pr} \in [0.5, 2]$, $\mathrm{Ra} \in [2 \times 10^3, 2 \times 10^5]$: $C = 0.197$, $n = 1/4$, $m = -1/9$
- $\mathrm{Ra} > 2 \times 10^5$: $C = 0.073$, $n = 1/3$, $m = -1/9$
- Otherwise: $\mathrm{Nu} = 1$ (pure conduction)

<a id="mixed-convection-regime"></a>
### Mixed Convection Regime

At low external Reynolds number, if $\mathrm{Gr}/\mathrm{Re}^2 > 0.9$, the natural convection Nusselt is **added** to the forced convection Nusselt:

$$\mathrm{Nu}_{\text{eff}} = \mathrm{Nu}_{\text{forced}} + \mathrm{Nu}_{\text{natural}}$$

---

<a id="friction-factor--haaland"></a>
## Friction Factor — Haaland

`fric(Re, ε/D)` — used inside the Petukhov-Gnielinski correlation.

**Turbulent** ($\mathrm{Re} > 2400$) — Haaland (1983):

$$f = 4 \left[ -1.8 \log_{10}\!\left(\frac{6.9}{\mathrm{Re}} + \left(\frac{\varepsilon/D}{3.7}\right)^{1.11}\right) \right]^{-2}$$

**Laminar** ($\mathrm{Re} \le 2400$):

$$f = \frac{64}{\mathrm{Re}}$$

---

<a id="formation-thermal-resistance--ramey"></a>
## Formation Thermal Resistance — Ramey

`ResForm()` — wellbore formation resistance based on Ramey (1962).

**Dimensionless time:**

$$\tau_D = \frac{4 \, t_{\text{prod}} \cdot 86400 \cdot \alpha_f}{D_{\text{ext}}^2}, \quad \alpha_f = \frac{k_f}{\rho_f \, c_{p,f}}$$

where $t_{\text{prod}}$ is in days and $\alpha_f$ is the formation thermal diffusivity.

**Time function used in the current implementation** (piecewise):

$$f(\tau_D) = \begin{cases}
1.1281\sqrt{\tau_D}\,(1 - 0.3\sqrt{\tau_D}) & \tau_D \le 1 \\[4pt]
0.432 + 0.4792\tau_D - 0.127\tau_D^2 + 0.0201\tau_D^3 - 0.0013\tau_D^4 & \tau_D \le 4.5 \\[4pt]
(0.4063 + 0.5\ln\tau_D)(1 + 0.6/\tau_D) & \tau_D > 4.5
\end{cases}$$

**Important note:** the middle branch above is written exactly as implemented in `ResForm()`. In many references the intermediate polynomial is expressed using $\ln(\tau_D)$ instead of $\tau_D$, so this branch should be interpreted as the **current code behavior**, not necessarily the canonical published form. As the implementation is dated, it is currently under investigation to determine whether this constitutes a bug or an intentional modification.

**Formation resistance:**

$$R_{\text{form}} = \frac{f(\tau_D)}{2\pi \, k_f}$$

---

## Ambient Fluid Properties

<a id="seawater"></a>
### Seawater / External Liquid

When `ambext == 1`, external liquid properties are updated automatically at the film temperature $T_f = 0.5(T_{\text{ext}} + T_{\text{wall,outer}})$.

The current implementation uses a simplified water/brine-style set of correlations:

| Property | Correlation used in code |
|----------|---------------------------|
| Density | McCain-style brine-volume-factor expression |
| Viscosity | $\mu = 2.414 \times 10^{-5} \cdot 10^{247.8/(T+133.15)}$ with internal unit conversion before storage in Pa·s |
| Conductivity | $k = 0.565 + 1.75 \times 10^{-3} T - 6.21 \times 10^{-6} T^2$ W/(m·K) |
| Specific heat | Piecewise polynomial in temperature |

**Implementation detail:** despite the documentation shorthand "seawater", the routine `MasEspLiq()` currently sets salinity fraction to zero internally, so the behavior is closer to a simplified water / weak-brine property model than to a full seawater model.

### Air

When `ambext == 2`, air properties are updated from fitted correlations:

| Property | Correlation |
|----------|---------------------------|
| Density | $\rho = \dfrac{101325 \cdot 28.97}{8314.46 \cdot (T + 273.15)}$ kg/m³ |
| Viscosity | $\mu = 1.799 \times 10^{-5} + 1.000 \times 10^{-7} T - 1.370 \times 10^{-9} T^2 + 6.96 \times 10^{-12} T^3$ Pa·s |
| Conductivity | $k = 0.0241 + 7.586 \times 10^{-5} T$ W/(m·K) |
| Specific heat | $c_p = 1000(1.005 + 1.096 \times 10^{-5} T + 4.600 \times 10^{-7} T^2)$ J/(kg·K) |

<a id="gas--nitrogen-annulus"></a>
### Gas / Nitrogen (Annulus)

For gas-filled annular layers (`tipomat == 3`), the implementation uses internal fitted correlations for compressed-gas density, heat capacity, conductivity, and viscosity.

| Property | Formula / model family used in code |
|----------|-------------------------------------|
| Density | $\rho_g = \dfrac{\gamma_g \cdot 28.96 \cdot P \cdot 98066.5}{8046.5 \cdot Z(P_R, T_R) \cdot (T + 273)}$ with implementation-specific constants |
| Specific heat | Linear temperature fit with gas-gravity dependence |
| Conductivity | Pressure-corrected empirical polynomial |
| Viscosity | Lee-Gonzalez-Eakin-style empirical form |

The Z-factor is computed by the Gopal correlation (`ZGopal`), using the 48-coefficient piecewise polynomial table `CoefGopalHT[]`:

$$Z = P_R(A \, T_R + B) + C \, T_R + D$$

with $A, B, C, D$ selected from the table based on reduced pressure/temperature regions. High-pressure branch ($P_R > 5.4$):

$$Z = P_R(0.711 + 3.66 \, T_R)^{-1.4667} - \frac{1.637}{0.319 \, T_R + 0.522} + 2.071$$

### Thermal Expansion Coefficient

`beta(T, type)` — numerical differentiation:

$$\beta = -\frac{1}{\rho}\frac{\partial\rho}{\partial T} \approx -\frac{1}{\rho(T)}\frac{\rho(T + \delta T) - \rho(T)}{\delta T}$$

where $\delta T = 0.01 T$ (or 0.1 if $T \approx 0$). Type: 1 = external liquid, 2 = gas, 3 = air.

---

<a id="radial-resistance-network"></a>
<a id="layer-resistance-types"></a>
<a id="internal-convection-coefficient--hint"></a>
### Internal Convection Coefficient — hInt()

```
if Re > 2400:
    Nu = nussPet(Re, Pr, ε, μ_b, μ_w)           ← Petukhov-Gnielinski
else:
    compute Ra_internal
    if Ra < 1000 or mixed-convection suppression criterion is active:
        Nu = 3.6
    else:
        Nu = 0.22·(Ra/0.2)^0.28 · 3.5^(-0.25)   ← code form used for natural convection branch
    For lined pipe (revest = 1) in transient local assembly:
        confined natural convection may be used through NussConf2

h_i = Nu · k_int / D_inner
```

**Implementation note:** `novoHi` does **not** generally override `hInt()`. It is only used in the prescribed-wall-temperature branch of `transcel()` (`condiTparede == 1`), where a user-specified boundary coefficient can replace the default large penalty coefficient.

<a id="external-convection-coefficient--hext"></a>
### External Convection Coefficient — hExt()

```
if formacPoc == 0 (pipe in fluid):
    if ambext == 1: update external-liquid properties at film T
    if ambext == 2: update air properties at film T

    if dirconvExt == 0 (cross-flow):
        Nu = nussChuBer(Re, Pr)
    else (parallel flow):
        Nu = nussPet(Re, Pr, ε, μ_b, μ_w)

    h_e = Nu · k_ext / D_outer

if formacPoc == 1 (wellbore):
    h_e = 1 / (π · D_outer · R_formation)
```

For cross-flow cases, an additional natural-convection contribution is added only in the specific branches where the code evaluates mixed external convection, namely when the outer region is fluid (`formacPoc == 0`), `dirconvExt == 0`, and the relevant steady/transient branch enables the low-Reynolds correction.

<a id="wall-conductance--condparede"></a>
### Wall Conductance — condParede()

Builds the per-unit-length thermal conductance `tec[j]` for each wall layer or intermediate annular region:

- **Solid layers:**

$$\text{tec}_j = \frac{2\pi k}{\ln(D_o / D_i)}$$

- **Fluid layers (stagnant):** compute $\mathrm{Gr}$, $\mathrm{Ra}$, and a confined-convection Nusselt number, then

$$\text{tec}_j = \frac{\pi (D_o - D_i) h}{\ln(D_o / D_i)}$$

- **Fluid layers (forced annular flow):** compute an annular hydraulic diameter $D_h = 4A/P$, apply the internal forced-convection correlation, and convert the result to an equivalent radial conductance.

Returns the overall wall conductance:

$$k_{\text{wall}} = \left(\sum_j \frac{1}{\text{tec}_j}\right)^{-1}$$

---

<a id="steady-state-solver--transperm"></a>
## Steady-State Solver — transperm()

`TransCal::transperm()` computes the steady radial heat flux per unit length.

**Total thermal resistance:**

$$R_{\text{total}} = \frac{1}{\pi D_i h_i} + \sum_{j=1}^{N_{\text{layers}}} \frac{1}{\text{tec}_j} + \frac{1}{\pi D_o h_e} + R_{\text{annulus}}$$

where in the implementation:
- `tec[0] = \pi D_i h_i` is the **inner convective conductance**,
- `tec[1] ... tec[N]` are the wall / intermediate-layer conductances,
- `tec[N+1] = \pi D_o h_e` is the **outer convective conductance**.

**Heat flux per unit length:**

$$q = \frac{T_{\text{ext}} - T_{\text{int}}}{R_{\text{total}}} \quad [\text{W/m}]$$

**Interface-temperature march used in code:**

$$T_{\text{wall,inner}} = T_{\text{int}} + \frac{q}{\pi D_i h_i} = T_{\text{int}} + \frac{q}{\text{tec}[0]}$$

For subsequent interfaces, the code advances through the radial chain using the corresponding layer conductances `tec[k]`.

**External mixed convection:** in the cross-flow branch, when the implementation is in the low-Reynolds correction path, the natural-convection contribution is evaluated from `RaExt()` and added to the forced-convection Nusselt number only if the code criterion based on $\mathrm{Gr}/\mathrm{Re}^2$ is satisfied.

If `difus2D == 1`, the routine delegates the external-soil problem to `transperm2D()` and still uses the 1D wall / interface reconstruction around that coupled result.

---

<a id="transient-solver--transtrans"></a>
## Transient Solver — transtrans()

`TransCal::transtrans()` solves the transient radial thermal problem across the multilayer wall.

### Numerical Formulation

The underlying physics is the cylindrical transient heat equation,

$$\rho \, c_p \frac{\partial T}{\partial t} = \frac{1}{r} \frac{\partial}{\partial r}\!\left(r \, k \frac{\partial T}{\partial r}\right),$$

but the implementation is assembled in a **mixed temperature / radial-flux form** rather than as a simple temperature-only finite-difference stencil.

Each radial location contributes two unknowns to the global linear system:
- temperature,
- an auxiliary radial heat-flow variable later converted into `Qcamada = 2\pi q`.

The total number of algebraic unknowns is therefore:

$$N_{\text{unk}} = 2\left(1 + \sum_i n_{\text{camada},i}\right).$$

<a id="radial-discretization"></a>
### Radial Discretization and Local Assembly

`transcel(icam, idisc)` assembles a 2-row local contribution stored in `localmat[2][6]` and `localvet[2]`. The exact coefficients depend on whether the point is:
- an interior solid node,
- an inner convective boundary,
- an outer convective boundary,
- or an interface adjacent to a fluid annulus treated through an equivalent convective resistance.

For interior locations, the transient accumulation term appears explicitly through:

$$\frac{\rho c_p}{\Delta t} T^{n+1} = \frac{\rho c_p}{\Delta t} T^n + \text{radial conductive / convective couplings}.$$

At the inner boundary (`icam = 0`, `idisc = 0`), the code imposes a Robin-type condition driven by `h_i`. At the outermost boundary, a Robin condition driven by `h_e` or by the equivalent formation resistance is imposed.

### Global Assembly and Solve

`transtrans()`:

1. assembles all local contributions into a banded matrix,
2. solves the global linear system with Gaussian elimination,
3. stores temperatures back into `Tcamada`,
4. stores radial heat-flow variables back into `Qcamada` after multiplying by $2\pi$,
5. updates `fluxIni` and `fluxFim` from the first and last solved radial fluxes.

Thus the transient solver should be understood as a **coupled mixed radial conduction problem**, not merely as a scalar explicit finite-difference temperature update.

**`FeiticoDoTempo()`** restores all `Tcamada` and `Qcamada` values from `Tini` and `Qini`, enabling rollback during outer coupled iterations.

If `difus2D == 1`, the routine delegates the soil-side problem to `transtrans2D()`.

---

<a id="axial-energy-equation"></a>
<a id="energy-balance-terms"></a>
<a id="coupling-radial-heat-flux--axial-solver"></a>
<a id="thermal-members-in-cell-classes"></a>
## 2D Buried-Pipe Solver (Poisson)

When `difus2D == 1`, the external buried-soil problem is handled by a 2D finite-volume Poisson / diffusion solver, while the pipe wall and internal convection remain represented through equivalent 1D conductances.

### Coupling Philosophy

This is not a full replacement of the entire radial thermal model by a 2D mesh. Instead:

1. the inner convection coefficient `hI` is still computed from the 1D `TransCal` correlations,
2. the multilayer wall is condensed into equivalent conductances through `condParede()` and `condParedeLocal()`,
3. the external environment / soil is solved in 2D,
4. the resulting total heat flux is then mapped back to the 1D wall-temperature reconstruction.

The quantities passed from `TransCal` to the Poisson solver are explicitly:

```text
poisson2D.dados.tInt    = Tint
poisson2D.dados.tAmb    = Textern1
poisson2D.dados.condGlob = condParede()
poisson2D.dados.condLoc  = condParedeLocal()
poisson2D.dados.hE       = hExt()
poisson2D.dados.hI       = hInt()
```

### Mesh and Elements

The 2D solver uses a cell-centered finite-volume formulation on unstructured triangular meshes.

**Key classes:**

| Class | File | Role |
|-------|------|------|
| `elementoPoisson` | [`estruturasPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturasPoisson.h) | Element geometry, centroids, face data, temperature, conductivity, density, specific heat |
| `elem2dPoisson` | [`Elem2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem2DPoisson.h) | Neighbor connectivity, local assembly, Green-Gauss gradients, thermal BC handling |
| `malha2d` | [`Malha2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha2DPoisson.h) | Mesh container and geometric preprocessing |
| `solverP` | `solverPoisson.h` | Global data, sparse assembly, steady/transient iterations |

### Boundary Conditions

Defined in [`estruturasPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturasPoisson.h):

| BC Type | Structure | Description |
|---------|-----------|-------------|
| **Dirichlet** | `detDiriPoisson` | Prescribed temperature |
| **Von Neumann** | `detVNPoisson` | Prescribed heat flux |
| **Richardson** | `detRicPoisson` | Convective boundary condition |
| **Coupled** | `rotuloAcop` | Coupling between buried-soil domain and equivalent pipe-wall model |

<a id="assembly--geralocal"></a>
### Assembly — GeraLocal()

For each element, `GeraLocal()` assembles:
- orthogonal diffusion between neighboring cells,
- non-orthogonal correction terms from Green-Gauss gradients,
- transient accumulation terms when transient mode is active,
- source terms and boundary contributions.

The method uses a face-based finite-volume balance, with conductivity interpolation and boundary-condition-specific source / coefficient terms.

<a id="gradient-reconstruction--green-gauss"></a>
### Gradient Reconstruction — Green-Gauss

`calcGradGreen()` reconstructs temperature gradients from face values using a Green-Gauss approach with non-orthogonality correction. Distortion filtering is applied when the angle between centroid-connecting vectors and face-area vectors becomes unfavorable.

<a id="local-element-matrix--transcel"></a>
### Linear Solve

The global sparse system is assembled and solved iteratively in both steady and transient modes. The solver workflow includes:

1. gradient reconstruction,
2. local assembly,
3. sparse global assembly,
4. Krylov linear solve,
5. temperature update,
6. convergence check,
7. coupled-boundary heat-flux update when required.

### Coupling to 1D Flow

**`transperm2D()`**:

1. updates `tInt`, `tAmb`, `condGlob`, `condLoc`, `hE`, and `hI`,
2. initializes the 2D field,
3. applies a pseudo-transient acceleration loop through `transientePoissonDummy(deltFic)` until the wall heat flux stabilizes,
4. solves the final steady 2D problem,
5. returns

$$q = -q_{\text{Total,2D}}.$$

**`transtrans2D()`**:

1. switches the 2D model to transient mode when needed,
2. updates convective Richardson boundary data from the current ambient state,
3. advances the 2D thermal field with the current 1D time step,
4. returns

$$q = -q_{\text{Total,2D}}.$$

After either call, `TransCal` still reconstructs the 1D wall-interface temperatures using the equivalent radial conductance chain and the returned total heat flux.

---

## 3D Soil Diffusion Solver

The 3D thermal-diffusion capability extends the buried-pipe concept to a fully three-dimensional surrounding domain. In the codebase, this functionality is centered on `dadosP3D`, `Elem3DPoisson`, `Malha3DPoisson`, and `solverP3D`.

### Main role in the coupled thermal model

When the global configuration enables 3D diffusion (`modoDifus3D == 1`), the surrounding thermal field is no longer represented only by the 1D radial resistance chain or by the 2D buried-soil model. Instead, the external domain is solved in 3D and the resulting heat exchange is coupled back into the 1D axial energy balance.

In `SisProd.cpp`, this contribution appears through the total heat-flow array produced by the 3D solver, which is converted to the axial source term used by each 1D cell.

### Main 3D data structures

`dadosP3D` stores, among other quantities:

- transient / steady flags in `temp`
- material and region definitions in `prop`
- coupling and boundary-condition information in `CC`
- mesh connectivity in `noEle`
- node coordinates in `xcoor`
- per-coupling-location thermal data such as:
  - `tParede[]`
  - `tInt[]`
  - `hE[]`
  - `hI[]`
  - `qAcop[]`
  - `qTotal[]`
  - `diamRef[]`

This organization shows that the 3D model supports multiple coupled surfaces or thermal interfaces simultaneously.

### Coupling interpretation

Conceptually, the 3D solver plays the same role as the 2D buried-pipe solver, but over a full volumetric domain:

1. internal-fluid and wall-side quantities are still provided by the 1D / radial thermal model,
2. the external environment is solved in 3D,
3. the total exchanged heat is returned to the axial energy equation.

Thus, the 3D model should be understood as a **surrounding-domain thermal solver coupled to the 1D flow model**, not as a replacement for all internal radial wall physics.

---

## Special Configurations

The heat-transfer model supports several special configurations that modify geometry interpretation, boundary conditions, or the way radial and external thermal resistances are assembled.

### Subsea Pipelines

For subsea pipelines, the outer environment is typically handled as an external liquid domain. In this case:

- `ambext == 1` activates automatic external-liquid property updates,
- `dirconvExt` selects cross-flow or parallel-flow treatment,
- mixed external convection may be added in the appropriate cross-flow branches,
- the radial chain remains the default representation unless 2D or 3D external diffusion is enabled.

This is the standard use case for the outer convective coefficient `hExt()` together with the multilayer wall resistance chain.

### Risers and Wellbores

The well / formation case is activated through `formacPoc == 1`.

In this configuration:

- the outer environment is not treated as a free external fluid,
- `ResForm()` computes an equivalent formation resistance,
- the outer thermal boundary is converted into

$$h_e = \frac{1}{\pi D_{\text{outer}} R_{\text{form}}}.$$

This is the main mechanism used to represent radial heat exchange with the formation in wellbore-like configurations.

<a id="buried--trenched-pipelines"></a>
### Buried / Trenched Pipelines

For buried or trenched pipelines, the external thermal domain can be treated in two different ways:

- **2D external diffusion** through `difus2D == 1`, using `transperm2D()` / `transtrans2D()`;
- **3D external diffusion** through the global 3D mode.

In both cases, the pipe wall itself is still represented through equivalent 1D conductances, while the surrounding soil is handled by the external Poisson / diffusion solver.

<a id="lined--annular-pipes"></a>
### Lined / Annular Pipes

Annular or lined geometries are identified by `geom.revest == 1`.

For these cases:

- `DadosGeo` interprets the geometry using both `a` and `b`,
- the hydraulic diameter is computed from annular area and wetted perimeter,
- internal natural-convection treatment may differ from the simple circular-pipe branch,
- confined-convection correlations can be used in annular thermal gaps.

This is especially important when the internal region between `a` and `b` must be treated as an annular thermal space rather than as a single circular conduit.

### Annulus with Forced Flow

If an intermediate layer is a fluid (`tipomat != 0`) and the annular velocity `Vconf` is non-negligible, the model switches from stagnant-gap natural convection to a forced-convection treatment.

The code then:

1. computes annular area and wetted perimeter,
2. derives an annular hydraulic diameter,
3. evaluates a Reynolds number for the annular flow,
4. applies the forced-convection correlation,
5. converts the result into an equivalent radial conductance.

This affects both `condParede()` / `condParedeLocal()` and the transient local assembly when the fluid annulus is part of the multilayer wall representation.

### Column Configuration

`TransCal` also includes the flags `coluna` and `colunaDia`, which alter the reference diameter used in some external-convection branches.

This configuration is relevant when the external thermal exchange should be referenced to a column-like surrounding geometry rather than directly to the last pipe-wall diameter. In those cases, `colunaDia` can become the characteristic diameter for the external convection calculation.

### Prescribed Wall Temperature

The flag `condiTparede` controls whether the inner wall uses:

- a convective boundary condition (`condiTparede == 0`), or
- a prescribed-wall-temperature style enforcement branch (`condiTparede == 1`) in the transient local assembly.

In the prescribed-wall-temperature branch, the model replaces the regular inner convective treatment by a large equivalent coefficient, and `novoHi` may optionally provide a user-specified substitute value for that coefficient.

### Wax Deposition Layer

Wax-related thermal coupling appears mainly through the main 1D cell model rather than inside `TransCal` alone.

The `Cel` class stores wax-deposition-related quantities such as:

- `MW_wax`
- `rhoWaxLiq`
- `Sum_dCwaxdT`
- `detalhaParafina`

and auxiliary deposition quantities including:

- interfacial deposition temperature
- wax diffusivity
- concentration gradient
- mass flux terms
- deposition conductivity-like parameter `kDep`

From a heat-transfer perspective, wax matters because it can alter the effective thermal response near the wall and introduces additional thermo-compositional coupling in the axial energy model.

---

## Summary of Named Correlations

The table below summarizes the main named correlations and implementation models referenced in this document.

| Topic | Function / mechanism | Main use in code |
|------|-----------------------|------------------|
| Internal forced convection | `nussPet()` | Pipe-side and annular forced convection |
| Turbulent friction factor | `fric()` | Input to Petukhov/Gnielinski-style heat transfer |
| External cross-flow convection | `nussChuBer()` | Cylinder in external cross-flow |
| External natural convection | `nussNatExt()` | Low-Re / mixed external convection correction |
| Confined natural convection | `NussConf2()` | Inclined annular or gap natural convection |
| Fallback confined-convection coefficients | `defineConf()` | Power-law constants for confined convection |
| Heating/cooling viscosity exponent | `definePet()` | Selects exponent in `nussPet()` |
| Wellbore formation resistance | `ResForm()` | Equivalent outer thermal resistance in formation |
| External liquid properties | `MasEspLiq()`, `VisLiq()`, `CondLiq()`, `CalorLiq()` | Auto-updated outer liquid properties |
| External air properties | `MasEspAr()`, `VisAr()`, `CondAr()`, `CalorAr()` | Auto-updated outer air properties |
| Gas / annulus properties | `MasEspGas()`, `ViscGas()`, `CondGas()`, `CalorGas()`, `ZGopal()` | Gas-filled annular regions |
| Thermal expansion | `beta()` | Buoyancy and Rayleigh-number evaluation |
| 1D steady radial model | `transperm()` | Steady multilayer radial heat flux |
| 1D transient radial model | `transtrans()` | Transient multilayer radial heat flux |
| 2D buried-soil model | `transperm2D()`, `transtrans2D()` | External soil diffusion around buried pipe |
| 3D surrounding-domain model | `solverP3D`, `dadosP3D` | Full 3D external thermal diffusion |

Together, these models form a layered thermal architecture:

1. internal-fluid convection,
2. multilayer radial wall / annulus resistance,
3. external environment exchange by direct convection, formation resistance, 2D buried diffusion, or 3D diffusion,
4. feedback of the resulting radial heat flux into the axial energy equation.

---
