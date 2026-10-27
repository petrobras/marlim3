# Injection Well Simulation

This document describes the **injection well** simulation mode in Marlim3 (`-s INJETOR` / `tipoSimulacao_t::poco_injetor`). Injection well simulations are **steady-state only** and model injection flow from the surface down to one or more injection zones.

**Key source files:**

| File | Role |
|------|------|
| [`src/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp) | Entry point: `SolveTramoSolteiro()`, `permanenteSimples()` dispatching |
| [`src/SisProd.h`](https://github.com/petrobras/marlim3/blob/main/src/include/SisProd.h) / [`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) | `SProd` class: `marchaInjPerm1()`, `buscaInjPfundoPerm1–5()`, `delpInjPerm()` |
| [`src/Leitura.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Leitura.h) / [`Leitura.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Leitura.cpp) | `detCondConInjec` structure, `parse_condcont_pocinjec()` |
| [`src/PropFluCol.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFluCol.h) / [`PropFluCol.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFluCol.cpp) | `ProFluCol` — injection fluid properties (water, CO₂, compositional) |
| [`src/FonteMas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/FonteMas.h) / [`FonteMas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/FonteMas.cpp) | `IPR` class — injectivity index model |

---

## Table of Contents

1. [Overview](#overview)
2. [Boundary Condition Types](#boundary-condition-types)
3. [JSON Input](#json-input)
4. [Injection Fluid Types](#injection-fluid-types)
5. [Call Hierarchy](#call-hierarchy)
6. [The Marching Method — marchaInjPerm1](#the-marching-method--marchainjperm1)
7. [Root-Finding and Iterative Methods](#root-finding-and-iterative-methods)
   - [buscaInjPfundoPerm1 — CC 1 or 3](#buscainjpfundoperm1--cc-1-or-3)
   - [buscaInjPfundoPerm2 — CC 0](#buscainjpfundoperm2--cc-0)
   - [buscaInjPfundoPerm3 — CC 2](#buscainjpfundoperm3--cc-2)
   - [buscaInjPfundoPerm4 — CC 4](#buscainjpfundoperm4--cc-4)
   - [buscaInjPfundoPerm5 — CC 5](#buscainjpfundoperm5--cc-5)
8. [IPR Model for Injection](#ipr-model-for-injection)
9. [Pressure Drop Estimation — delpInjPerm](#pressure-drop-estimation--delpinjperm)
10. [Injection Network — RedeInj](#injection-network--redeinj)
11. [Compositional Injection](#compositional-injection)
12. [Key Differences from Production](#key-differences-from-production)
13. [Summary of Key Methods](#summary-of-key-methods)

---

## Overview

Injection well simulations model the downward flow of an injection fluid from the surface wellhead to one or more reservoir injection zones. The solver computes pressure, temperature, and flow-rate profiles along the wellbore under steady-state conditions.

For user-defined liquid and saline-water modes, the model is effectively single-phase. For table-based CO₂ and especially compositional injection, the code may also initialize and propagate gas/liquid split information (for example via `alfini`, `betini`, and `FracMassHidra()` in `marchaInjPerm1`), so the implementation should not be interpreted as strictly single-phase in every injection case.

The fundamental approach is the same shooting method used for production:

1. **Guess** an unknown boundary value (surface pressure or injection rate)
2. **March** cell-by-cell from surface to bottom, computing pressure and temperature
3. **Evaluate a residual** — the mismatch at the bottom boundary (IPR-predicted rate vs. computed rate, or specified vs. computed bottom-hole pressure)
4. **Iterate** using Ridder's root-finding method when the selected boundary-condition formulation requires bracketing of a scalar unknown

The key constraint: **injection wells are steady-state only** (no transient simulation).

---

## Boundary Condition Types

The `detCondConInjec` structure ([`Leitura.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Leitura.h)) defines six BC configurations via the `CC` flag:

| CC | Given | Solved for | Root-finding? |
|----|-------|-----------|---------------|
| 0 | Flow rate + IPR at bottom | Surface pressure | Yes (`buscaInjPfundoPerm2`) |
| 1 | Injection pressure (surface) + IPR at bottom | Flow rate | Yes (`buscaInjPfundoPerm1`) |
| 2 | Bottom-hole pressure + IPR at bottom | Flow rate | Iterative pressure-profile correction (`buscaInjPfundoPerm3`) |
| 3 | Injection pressure + bottom-hole pressure | Flow rate | Yes (`buscaInjPfundoPerm1`) |
| 4 | Flow rate + injection pressure | Bottom pressure/profile computed directly | No |
| 5 | Flow rate + bottom-hole pressure | Surface pressure | Yes (`buscaInjPfundoPerm5`) |

### `detCondConInjec` members

| Member | Type | Units | Description |
|--------|------|-------|-------------|
| `CC` | `int` | — | BC type (0–5) |
| `tipoFlui` | `int` | — | Fluid type (0 = user, 1 = water, 2 = CO₂ PVTSim table, 3 = compositional) |
| `salin` | `double` | g/L | Water salinity (for `tipoFlui == 1`) |
| `tempinj` | `double` | °C | Injection temperature |
| `vazinj` | `double` | Sm³/d | Injection flow rate |
| `presinj` | `double` | kgf/cm² | Surface injection pressure |
| `presfundo` | `double` | kgf/cm² | Bottom-hole pressure |
| `pvtsimarqInj` | `string` | — | PVTSim file path for CO₂/compositional fluid |

---

## JSON Input

Injection well parameters are parsed by `parse_condcont_pocinjec()` from the `"CondicaoContPocInjec"` JSON object:

```json
"CondicaoContPocInjec": {
  "ativo": true,
  "condcontorno": 2,
  "tipoFluido": 1,
  "salinidade": 20.0,
  "tempInj": 40.0,
  "vazLiq": 1000.0,
  "presInjec": 33.91,
  "presFundo": 339.0,
  "arquivoPvtsim": "PVTSIM_MARLIM.tab"
}
```

**Conditional field requirements** (enforced at parse time):

| CC values | Required fields |
|-----------|----------------|
| 0, 4, 5 | `vazLiq` |
| 1, 3, 4 | `presInjec` |
| 2, 3, 5 | `presFundo` |

---

## Injection Fluid Types

The injection fluid is modeled by `ProFluCol`, dispatched via `injPoc`:

| `tipoFlui` | `injPoc` | Fluid model | Properties source |
|------------|----------|-------------|-------------------|
| 0 | 0 | User-defined liquid | ASTM viscosity, constant Cp, constant conductivity, compressibility model |
| 1 | 2 | Saline water | Built-in correlations: $\rho = f(P, T, \text{salinity})$, Arrhenius $\mu$, polynomial $C_p$ and $k$ |
| 2 | 3 | CO₂-rich gas (supercritical) | Bilinear interpolation from PVTSim 2D (P × T) tables for $\rho$, $\mu$, $k$, $C_p$ |
| 3 | — | Compositional | Full cubic EOS flash via Fortran compositional library |

For CO₂ injection (`tipoFlui == 2`), property tables are loaded from a PVTSim `.tab` file and stored as 2D arrays (`RhoInj[][]`, `ViscInj[][]`, `CondInj[][]`, `CpInj[][]`, `DrhoDtInj[][]`). The `interpolaVarInj()` method performs bilinear interpolation on these arrays after first clamping pressure and temperature to the table bounds; therefore, out-of-range states are clipped to the nearest tabulated boundary rather than extrapolated.

---

## Call Hierarchy

```
main()
 └─ SolveTramoSolteiro(sistem1)                         [Num4Main.cpp]
     ├─ (compositional) Switch to black-oil mode, run permanenteSimples
     ├─ (compositional) Restore compositional, use black-oil as initial guess
     └─ permanenteSimples(sistem1)                       [Num4Main.cpp]
         └─ Detect pocinjec != 0, dispatch by CC:
             ├─ CC 1 or 3 → buscaInjPfundoPerm1(chute)  [SisProd.cpp]
             ├─ CC 0      → buscaInjPfundoPerm2(chute)
             ├─ CC 2      → buscaInjPfundoPerm3(chute)
             ├─ CC 4      → buscaInjPfundoPerm4()
             └─ CC 5      → buscaInjPfundoPerm5(chute)
                             │
                             └─ marchaInjPerm1(x)        [SisProd.cpp]
                                 ├─ Set initial T from injection source
                                 ├─ Handle surface choke ΔP
                                 └─ March cell-by-cell (i=1 to ncel):
                                     ├─ RenovaPresPermMon(i)
                                     ├─ RenovaMassPerm(i)
                                     ├─ RenovaTempPerm(i)
                                     ├─ RenovaPresPermJus(i)
                                     └─ RenovaTransMassPerm(i-1)
```

---

<a id="the-marching-method--marchainjperm1"></a>
## The Marching Method — marchaInjPerm1

`SProd::marchaInjPerm1(double chute)` ([`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) is the core marching function for injection wells.

### Input interpretation

The meaning of `chute` depends on the BC type:

| CC | `chute` represents |
|----|--------------------|
| 1, 2, 3 | Flow rate (Sm³/d) — sets `injl.QLiq` or `injg.QGas` |
| 0, 5 | Surface pressure (kgf/cm²) — sets `pGSup` |

### Algorithm

1. Set initial temperature from the injection source (`injl.temp` or `injg.temp`)
2. If surface choke is active (opening < 0.6): compute ΔP across choke
3. For compositional gas: compute initial phase fractions ($\alpha_{ini}$, $\beta_{ini}$)
4. **Cell-by-cell march** from surface (i=1) to bottom (i=ncel):
   ```
   while i ≤ ncel AND P ≥ 1 kgf/cm² AND mass_balance ≥ 0:
       RenovaPresPermMon(i)       — pressure: center → upstream face
       RenovaMassPerm(i)          — mass balance (including IPR sources)
       RenovaTempPerm(i)          — energy equation
       RenovaPresPermJus(i)       — pressure: upstream face → center
       RenovaTransMassPerm(i-1)   — interphase mass transfer
       Accumulate mass sources from IPR zones
   ```
5. Return the **objective function** value:
   - For CC ≠ 3, 5: net mass flow at bottom (`masfim`). Zero means mass balance is satisfied.
   - For CC = 3, 5: $P_{\text{bottom,specified}} - P_{\text{bottom,computed}}$. Zero means pressure match.

### Error conditions

- $P < 1$ kgf/cm² — pressure dropped below physical minimum
- $P_{\text{reservoir}} > P_{\text{bottom}}$ — well would produce instead of inject
- Net mass at bottom < 0 — flow direction inconsistency

---

## Root-Finding and Iterative Methods

The injection-well solution methods fall into two groups:

1. **Bracketed scalar solves** using `zriddr` (Ridder's method)
2. **Direct or iterative profile construction** without Ridder bracketing

<a id="buscainjpfundoperm1--cc-1-or-3"></a>
### buscaInjPfundoPerm1 — CC 1 or 3

- **Given:** injection pressure (or both pressures for CC 3)
- **Unknown:** flow rate
- **Initial guess:** hydrostatic pressure walk from the surface combined with cumulative IPR-based injection-rate accumulation along the well, including density correction for `tipoFlui ≥ 2`
- **Bracketing:** multiplies guess by 0.9 / 1.1 until sign change in `marchaInjPerm1`
- **Root-finding:** `zriddr(chuteNeg, chutePos)`

<a id="buscainjpfundoperm2--cc-0"></a>
### buscaInjPfundoPerm2 — CC 0

- **Given:** injection flow rate + IPR at bottom
- **Unknown:** surface pressure
- **Initial guess:** reverse pressure estimate from the bottom condition using explicit hydrostatic and friction terms evaluated along the wellbore
- **Bracketing:** multiplies by 0.9 / 1.1
- **Root-finding:** `zriddr`

<a id="buscainjpfundoperm3--cc-2"></a>
### buscaInjPfundoPerm3 — CC 2

- **Given:** bottom-hole pressure + IPR
- **Unknown:** flow rate
- **Method type:** iterative pressure-profile correction, not a Ridder/bracketed root solve
- **Algorithm:** sets `celula[ncel].pres = presfundo`, estimates flow rate from the terminal IPR, reconstructs the pressure profile upward, then remarches from the surface and repeats until the computed bottom-hole pressure matches the specified value
- **Pressure update:** uses `delpInjPerm()` during the reverse pressure reconstruction step
- **Convergence:** $|P_{\text{computed}} - P_{\text{specified}}| / P_{\text{specified}} < 0.01\%$

<a id="buscainjpfundoperm4--cc-4"></a>
### buscaInjPfundoPerm4 — CC 4

- **Given:** flow rate + injection pressure
- **Method type:** direct march
- **Algorithm:** performs a forward march with the prescribed inlet condition and computes the resulting bottom pressure/profile as an output
- Returns net mass flow at bottom for diagnostics

<a id="buscainjpfundoperm5--cc-5"></a>
### buscaInjPfundoPerm5 — CC 5

- **Given:** flow rate + bottom-hole pressure
- **Unknown:** surface pressure
- **Initial guess:** reverse pressure estimate from the bottom condition using explicit hydrostatic and friction terms evaluated along the wellbore
- **Root-finding:** `zriddr`

---

## IPR Model for Injection

Injection zones use a **linear injectivity index** model. At each cell with `acsr.tipo == 3`:

$$Q_{\text{inj}} = -II \cdot (P_{\text{res}} - P_{\text{bottom}})$$

where:
- $II$ = injectivity index (`celula[i].acsr.ipr.ij`)
- $P_{\text{res}}$ = reservoir pressure (`celula[i].acsr.ipr.Pres`)

Since $P_{\text{bottom}} > P_{\text{res}}$ for injection, the term $(P_{\text{res}} - P_{\text{bottom}})$ is negative, making $Q_{\text{inj}}$ positive (fluid flows into the reservoir).

For tabulated fluids (`tipoFlui ≥ 2`), the IPR is corrected for in-situ density:

$$Q_{\text{inj,std}} = -\frac{II \cdot \rho(P,T)}{\rho_{\text{std}}} \cdot (P_{\text{res}} - P_{\text{bottom}})$$

Multiple injection zones along the wellbore are supported — each cell can independently host an IPR accessory.

---

<a id="pressure-drop-estimation--delpinjperm"></a>
## Pressure Drop Estimation — delpInjPerm

`SProd::delpInjPerm(int i)` estimates the pressure drop across the interface between cells $i-1$ and $i$ using a **two-half-cell evaluation**: one contribution from the right half of cell $i-1$ / left connection, and another from the left half of cell $i$. In each half-step, the code reevaluates local geometry, temperature, density, viscosity, Reynolds number, and friction factor.

A compact conceptual representation of the implemented model is:

$$
\Delta P \approx \frac{f \rho |v| v S_i \Delta x /(2A) + \rho g \sin\theta \Delta x}{98066.5}
$$

where $f$ is the friction factor, $\rho$ is the fluid density from `fluicol`, and the result is in kgf/cm². In the actual implementation, this expression is applied separately over two half-cell segments, and the user-specified correction factors `dPdLFric` and `dPdLHidro` are applied to the friction and hydrostatic contributions respectively.

This method is used mainly in reverse pressure reconstruction and pressure-guess support routines such as `buscaInjPfundoPerm3()` and `hidroreversoInj()`.

---

<a id="injection-network--redeinj"></a>
## Injection Network — RedeInj

When the simulation type is `REDE` and the network is tagged as injection (`arqRede.injec == 1`), the `RedeInj()` function ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) manages the network-level solve.

### Algorithm

1. Construct all tramo `SProd` objects from JSON files
2. Tag each tramo: `noextremo` (end of network / leaf), `noinicial` (start / root)
3. **Initial pressure guess** via `chutePresRedeInj()`: walks the network tree from collectors to leaves, distributing flow proportionally by pipe area and estimating node pressures via `hidroreversoInj()`

$$Q_{\text{tramo}} = Q_{\text{total}} \cdot \frac{A_{\text{tramo}}}{A_{\text{total}}}$$

4. **Iterative convergence loop** (`cicloRedeInj`):
   - Solve leaf tramos first (no upstream dependencies) using `buscaInjPfundoPerm1` or `buscaInjPfundoPerm5`
   - Then solve collector tramos once all upstream tributaries converge
   - At collectors: mix temperatures and flow rates from upstream tributaries
   - Update inter-tramo boundary conditions (upstream pressure → downstream injection pressure)
   - Convergence criterion: `norma < 0.001 * relax`
5. If a tramo fails convergence, mark it inactive and propagate to downstream collectors

### hidroreversoInj

`hidroreversoInj(double hol, double vaz)` estimates pressures in reverse direction (bottom to surface). Starting from the bottom-hole pressure (or IPR-estimated pressure), it marches upward using `delpInjPerm` to compute the surface pressure. Used to provide initial pressure guesses for network nodes.

---

## Compositional Injection

For compositional injection (`tipoFlui == 3`, `flashCompleto == 2`), `SolveTramoSolteiro` applies the same two-pass strategy as production:

1. Temporarily set `flashCompleto = 3` (marks "injection compositional in black-oil pre-solve phase")
2. Run `permanenteSimples` in black-oil mode to get an initial guess
3. Restore `flashCompleto = 2` and run `permanenteSimples` again with the black-oil solution as initial guess
4. Optionally build dynamic PVT tables via `preparaTabDin()`

This avoids expensive flash calculations during the initial bracket search.

### Sensitivity Analysis

When `analiseSens.listaV.vpocinj == 1`, injection parameters (`presfundo`, `temp`, `vaz`) can be swept parametrically for sensitivity analysis.

---

## Key Differences from Production

| Aspect | Production | Injection |
|--------|-----------|-----------|
| Flow direction | Reservoir → Surface | Surface → Reservoir |
| Pressure profile | Decreases upward | Increases downward |
| IPR sign convention | $P_{\text{res}} > P_{\text{bottom}}$ (inflow) | $P_{\text{bottom}} > P_{\text{res}}$ (outflow into reservoir) |
| Fluid model | Black-oil multiphase (`ProFlu`) | Injection-fluid model centered on `ProFluCol` for user liquid / water / table-based CO₂, with compositional handling available in injection mode |
| Gas-lift line | Supported | Not applicable |
| Transient | Supported | **Not supported** (steady-state only) |
| Choke model | At outlet (surface separator) | At inlet (surface, restricts injection) |
| BCS / ESP pumps | Supported (production aids) | Not applicable |
| Network solver | `solveRedeProd()` / `SolveRedeTrans()` | Separate `RedeInj()` solver |
| Marcha method | `marchaProdPerm1()` / `marchaProdPerm2()` | `marchaInjPerm1()` |
| Busca methods | `buscaProdPfundoPerm()` / `buscaProdPfundoPerm2()` | `buscaInjPfundoPerm1()` – `buscaInjPfundoPerm5()` |

---

## Summary of Key Methods

| Method | Location | Description |
|--------|----------|-------------|
| `marchaInjPerm1(chute)` | `SisProd.cpp` | Cell-by-cell march for injection; returns residual |
| `buscaInjPfundoPerm1(chute)` | `SisProd.cpp` | Root-finding for CC 1 or 3: given pressure, find flow rate |
| `buscaInjPfundoPerm2(chute)` | `SisProd.cpp` | Root-finding for CC 0: given flow rate + IPR, find surface pressure |
| `buscaInjPfundoPerm3(chute)` | `SisProd.cpp` | Iterative solve for CC 2: given BHP + IPR, find flow rate |
| `buscaInjPfundoPerm4()` | `SisProd.cpp` | Direct march for CC 4: fully determined (no root-finding) |
| `buscaInjPfundoPerm5(chute)` | `SisProd.cpp` | Root-finding for CC 5: given flow rate + BHP, find surface pressure |
| `delpInjPerm(i)` | `SisProd.cpp` | Two-half-cell single-phase-like $\Delta P$ estimate used in reverse pressure reconstruction |
| `hidroreversoInj(hol, vaz)` | `SisProd.cpp` | Reverse walk for pressure estimation (bottom → surface) |
| `RedeInj()` | `Num4Main.cpp` | Injection network solver |
| `chutePresRedeInj()` | `Num4Main.cpp` | Initial pressure guess for injection network |
| `parse_condcont_pocinjec()` | `Leitura.cpp` | JSON parsing for injection well parameters |
| `interpolaVarInj(P, T, Var)` | `PropFluCol.cpp` | Bilinear interpolation with pressure/temperature clamping to PVTSim table bounds |
