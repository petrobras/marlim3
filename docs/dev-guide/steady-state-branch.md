# Steady-State Solution of a Single Branch

This document describes the logic and algorithms used by the Marlim3 simulator to compute the **steady-state (permanent) solution** of a single pipeline segment (*branch*). A branch is the fundamental simulation unit — a 1D sequence of control volumes (cells) representing a pipe or well, including artificial-lift devices, sources, and sinks.

**Key source files:**

| File | Role |
|------|------|
| [`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp) | Top-level orchestration: `SolveTramoSolteiro()`, `permanenteSimples()` |
| [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) | Core solver methods on the `SProd` class |
| [`src/include/SisProd.h`](https://github.com/petrobras/marlim3/blob/main/src/include/SisProd.h) | `SProd` class declaration |
| [`src/include/celula3.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celula3.h) / [`src/core/celula3.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celula3.cpp) | `Cel` class — per-cell state and low-level physics |
| [`src/include/PropFlu.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFlu.h) / [`src/core/PropFlu.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFlu.cpp) | `ProFlu` — fluid property correlations (black-oil & compositional) |
| [`src/core/FonteMas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/FonteMas.cpp) | Mass source term evaluation |
| [`src/core/GradientCorrelations.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/GradientCorrelations.cpp) | Standalone friction-factor and pressure-gradient correlations |
| [`src/core/Leitura.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Leitura.cpp) | JSON input parsing → `SProd` initialization |
| [`src/include/celulaGas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celulaGas.h) / [`src/core/celulaGas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celulaGas.cpp) | `CelG` class — gas-lift service line cell state and physics |
| [`src/include/chokegas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/chokegas.h) / [`src/core/chokegas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/chokegas.cpp) | `ChokeGas` — compressible gas choke model for VGL orifices |

---

## Table of Contents

1. [Overview](#overview)
2. [Call Hierarchy](#call-hierarchy)
3. [Step 1 — Object Construction](#step-1--object-construction)
4. [Step 2 — SolveTramoSolteiro: Fluid Model Strategy](#step-2--solvetramosolteiro-fluid-model-strategy)
5. [Step 3 — permanenteSimples: Boundary Condition Dispatch](#step-3--permanentesimples-boundary-condition-dispatch)
6. [Step 4 — buscaProdPfundoPerm: Bracket Search & Root Finding](#step-4--buscaprodpfundoperm-bracket-search--root-finding)
7. [Step 5 — marchaProdPerm1: The Cell-by-Cell March](#step-5--marchaprodperm1-the-cell-by-cell-march)
8. [The Marching Equations](#the-marching-equations)
9. [Convergence and Root Finding](#convergence-and-root-finding)
10. [Boundary Condition Variants](#boundary-condition-variants)
11. [Gas-Lift Service Line](#gas-lift-service-line)
12. [Compositional Model Acceleration](#compositional-model-acceleration)
13. [Output and Post-Processing](#output-and-post-processing)
14. [Summary of Key Methods](#summary-of-key-methods)
15. [Available Steady-State Multiphase Flow Models](#available-steady-state-multiphase-flow-models)

---

## Overview

The steady-state solver computes the **pressure, temperature, void fraction, and flow-rate profiles** along a branch under time-invariant conditions. At a high level, the solution strategy is:

1. **Guess** a boundary value (for example bottom-hole pressure or an inlet flow-related quantity)
2. **March** cell-by-cell from the upstream end to the downstream end, computing pressure, temperature, and transported mass quantities
3. **Evaluate the residual** — the mismatch between the computed downstream state and the imposed downstream boundary condition (e.g., separator pressure or choke-flow consistency)
4. **Iterate** with a bracketed root finder until the residual is driven to zero or to the configured tolerance band

This is a **shooting-method style formulation**: the unknown boundary value is the "shot", and the marching routines propagate that shot through the domain.

```
    Upstream (cell 0)                                  Downstream (cell ncel)
    ┌────────────────────────────────────────────────────────────┐
    │  Source/IPR    →  cell 1  →  cell 2  → ... →  cell ncel   │
    │  (BC: P or Q)    march     march              (BC: P_sep) │
    └────────────────────────────────────────────────────────────┘
           ↑ GUESS                                    ↑ RESIDUAL
           └──────────── Ridder's method ─────────────┘
```

---

## Call Hierarchy

The steady-state solution for a single branch follows this call chain:

```
main()
 └─ SolveTramoSolteiro(sistem1)          [`src/core/Num4Main.cpp`]
     ├─ (optional) Switch to black-oil mode for initial convergence
     ├─ permanenteSimples(sistem1)       [`src/core/Num4Main.cpp`]
     │   ├─ Detect BC type and flow direction
     │   └─ buscaProdPfundoPerm(chute)   [`src/core/SisProd.cpp`]
     │       ├─ Estimate initial pressure guess (hydrostatic walk)
     │       ├─ marchaProdPerm1(pchute)  [`src/core/SisProd.cpp`]  ← first trial
     │       │   ├─ Initialize cell 0 from source/IPR
     │       │   └─ for i = 1 to ncel:
     │       │       ├─ RenovaPresPermMon(i)    — P: cell center → left boundary
     │       │       ├─ atualizaPeriPmonProd(i) — Apply BCS/pump ΔP
     │       │       ├─ RenovaMassPerm(i)       — Mass flow + source terms
     │       │       ├─ RenovaTempPerm(i)       — Energy equation
     │       │       ├─ RenovaPresPermJus(i)    — P: left boundary → cell center
     │       │       ├─ atualizaPeriPjusProd(i) — Apply BCS/pump ΔP (jusante)
     │       │       └─ RenovaTransMassPerm(i-1) — Interphase mass transfer
     │       ├─ Bracket search: find `pchute_neg`, `pchute_pos` with opposite residual signs
     │       └─ zriddr(pchute_neg, pchute_pos) [`src/core/SisProd.cpp`]
     │           └─ multMarcha(x) → marchaProdPerm1(x) (repeatedly)
     ├─ (optional) Switch back to compositional, use black-oil result as initial guess
     ├─ permanenteSimples(sistem1)       ← second pass (compositional)
     └─ Print profiles, trends, log file
```

---

<a id="step-1--object-construction"></a>
## Step 1 — Object Construction

When `main()` creates the `SProd` object:

```cpp
SProd sistem1(nomeArquivoEntrada, nomeArquivoLog, validacaoJson, tipoSimulacao, &vg1dTramo);
```

The constructor ([`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) performs:

1. **Constructs the `arq` member** (type `Leitura`) — reads the JSON input file, parses geometry, fluid properties, boundary conditions, accessories, and mesh parameters
2. **Allocates the cell array** — `celula[0..ncel]`, each a `Cel` object containing geometry (`DadosGeo duto`), fluid properties (`ProFlu flui`, `ProFluCol fluicol`), and solver state (pressure, temperature, void fraction, mass flows)
3. **Initializes solver state** — zeroes counters, flags, pointers, and intermediate variables
4. **Allocates profile output tables** — `flut` for production line, `flutG` for gas-lift line
5. **(If gas-lift)** allocates the gas-line cell array `celulaG[]` and sets up valve positions

The `Cel` class ([`src/include/celula3.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celula3.h)) holds per-cell state organized into:

| Category | Key Fields |
|----------|------------|
| Geometry | `dutoL`, `duto`, `dutoR` — left/current/right geometry data |
| Pressure state | `presL`, `pres`, `presR`, plus interface/auxiliary values such as `presaux`, `presauxL`, `presauxR` and previous-step values like `presini` |
| Temperature state | `tempL`, `temp`, `tempR`, plus previous-step values such as `tempini`, `tempLini`, `tempRini` |
| Mass-flow state | `ML`, `MC`, `MR`, liquid-flow companions such as `MliqiniL`, `Mliqini`, `MliqiniR`, and auxiliary complementary-flow storage `MComp` |
| Void / phase fractions | `alf` (gas void fraction), `bet` (complementary-liquid fraction), and left/right/interface variants |
| Source terms | left/right source contributions such as `fontemassLL`, `fontemassCL`, `fontemassGL`, `fontemassLR`, `fontemassCR`, `fontemassGR` |
| Accessories | `acsr` — local source / pump / valve / IPR / porous model accessory |
| Drift-flux / closure terms | `c0`, `ud`, `term1`, `term2` and related left/right variants |
| Heat transfer | `calor` — `TrocaCalor` object with wall, insulation, ambient, and coupling data |
| Fluid models | `flui` (owned `ProFlu`), `fluiL`/`fluiR` (neighbor pointers), `fluicol` (`ProFluCol`) |

Each cell stores both current and neighboring state information, plus previous-step values used by the steady and transient algorithms. Structurally, each cell contains a single `acessorio` object (`acsr`), and the marching routines assume all local source/device effects for that control volume are represented through that object.

---

<a id="step-2--solvetramosolteirofluid-model-strategy"></a>
<a id="step-2--solvetramosolteiro-fluid-model-strategy"></a>
## Step 2 — SolveTramoSolteiro: Fluid Model Strategy

`SolveTramoSolteiro()` ([`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) is a wrapper that handles the **two-pass strategy** for compositional simulations:

### Black-Oil or Flash Table Modes (`flashCompleto != 2`)

For black-oil or flash table fluid models, a single call to `permanenteSimples()` suffices:

```
SolveTramoSolteiro
 └─ permanenteSimples(sistem1, chute0)  →  converged profiles
```

### Compositional Mode (`flashCompleto == 2`)

Compositional flash calculations are expensive. To accelerate convergence, the solver uses a **two-pass approach**:

```
SolveTramoSolteiro
 ├─ Pass 1: Switch to black-oil mode
 │   ├─ Set flashCompleto = 0 on all cells and accessories
 │   └─ permanenteSimples(sistem1, chute0)  →  approximate profiles
 │
 ├─ Extract initial guess from black-oil result:
 │   └─ inichute = cell[0].pres  (or derived flow rate)
 │
 ├─ Restore compositional mode (flashCompleto = 2)
 ├─ (optional) preparaTabDin() — build dynamic PVT tables from black-oil pressure range
 │
 └─ Pass 2: permanenteSimples(sistem1, inichute)  →  final compositional profiles
```

This avoids the expensive flash calculations during the initial bracket search, when pressure guesses may be far from the solution.

---

<a id="step-3--permanentesimples-boundary-condition-dispatch"></a>
## Step 3 — permanenteSimples: Boundary Condition Dispatch

`permanenteSimples()` ([`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) selects the solver variant based on the **upstream and downstream boundary conditions**.

### Decision Tree

A code-faithful summary is:

```
permanenteSimples
 │
 ├─ Production system (`pocinjec == 0`)
 │   │
 │   ├─ `ConContEntrada == 0`  → inlet represented by source/IPR/mass-source/porous source
 │   │   ├─ detect reverse flow only for explicit signed source types (`tipo` 1, 2, 10)
 │   │   ├─ if reverse → `buscaProdPfundoPermRev()`
 │   │   └─ if normal flow:
 │   │       ├─ if production choke is effectively open and `curvaDinamic == 0`
 │   │       │   ├─ gas-lift + pressure-controlled annulus may require a preliminary flow-controlled estimate
 │   │       │   └─ final production solve uses `buscaProdPfundoPerm()`
 │   │       └─ else
 │   │           ├─ gas-lift + pressure-controlled annulus may again use preliminary flow-controlled estimates
 │   │           └─ final production solve uses `buscaProdPfundoPerm2()`
 │   │
 │   ├─ `ConContEntrada == 1`  → inlet pressure prescribed, inlet flow unknown
 │   │   ├─ if choke wide open → `buscaProdPresPresPerm()`
 │   │   ├─ if choke active    → `buscaProdPresPresPerm2()`
 │   │   └─ if choke effectively closed → `buscaProdPresPresPerm3()`
 │   │
 │   └─ `ConContEntrada == 2`  → inlet pressure + flow-related condition
 │       └─ `buscaProdPfundoPerm3()`
 │
 └─ Injection system (`pocinjec == 1`)
     ├─ `CC == 0`       → `buscaInjPfundoPerm2()`
     ├─ `CC == 1 or 3`  → `buscaInjPfundoPerm1()`
     ├─ `CC == 2`       → `buscaInjPfundoPerm3()`
     ├─ `CC == 4`       → `buscaInjPfundoPerm4()`
     └─ otherwise       → `buscaInjPfundoPerm5()`
```

Important implementation notes:
- the reverse-flow detection at this level is only done directly for source types `1`, `2`, and `10`
- for gas-lift systems with annulus pressure as boundary condition, `permanenteSimples()` may temporarily switch the gas-line mode to a flow-controlled estimate before retrying the pressure-controlled solve
- the distinction between “choke open” and “choke active” is code-driven (`abertura`, `curvaDinamic`) rather than purely conceptual

### Key Boundary Condition Types

| Upstream BC (`ConContEntrada`) | Unknown | Main production march family | Root-finding target |
|-------------------------------|---------|-------------------------------|---------------------|
| `0` — source/IPR-type inlet | bottom-hole / inlet pressure guess | `marchaProdPerm1` or `marchaProdPerm2` | pressure-match or choke-flow residual |
| `1` — inlet pressure prescribed | inlet mass / flow-rate proxy | `marchaProdPresPres1/2/3` | downstream pressure or choke-related residual |
| `2` — mixed inlet prescription | specialized inlet pressure solve | `buscaProdPfundoPerm3` family | case-specific residual |

---

<a id="step-4--buscaprodpfundoperm-bracket-search--root-finding"></a>
## Step 4 — buscaProdPfundoPerm: Bracket Search & Root Finding

`buscaProdPfundoPerm()` ([`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) searches for the inlet / bottom-hole pressure that makes the production-line residual vanish. Conceptually it is still a bracketed shooting solve, but the implementation contains several fallback paths and guardrails that are important to mention.

### Phase 1: Initial Pressure Estimate

If `chute < 0`, the routine builds an initial estimate starting from `pGSup` and walking backward from the downstream end. The estimate is based on a coarse hydrostatic + friction reconstruction:

- local liquid/gas densities are estimated from the current fluid model
- a guessed void fraction / holdup is used to form a mixture density
- a friction term is estimated from a coarse Reynolds/friction-factor evaluation when inlet-source information is available
- constant pressure-gain devices (`acsr.tipo == 7`) are accounted for during the backward walk
- if an IPR is present at the upstream end, the estimate is clipped away from the static-pressure limit
- if a PVT table is active, the estimate is also clipped to remain inside the admissible pressure range

So, the backward walk is best interpreted as a **robust engineering guess generator**, not as an exact reconstruction of the later marching model.

### Phase 2: First March

The routine then evaluates:

```cpp
val = marchaProdPerm1(pchute);
```

Interpretation of the result is implementation-driven:
- `val < 0`: the guessed pressure is too high for the downstream target
- `val > 0`: the guessed pressure is too low
- `val ≈ ±1e10`: the march hit an admissibility limit or failed to complete properly
- `val ≈ ±1.1e10`: stronger failure / non-convergence signaling may be propagated in network-related contexts

If the first march lands in a failure sentinel region, `buscaProdPfundoPerm()` does **not** immediately give up. It constructs alternative estimates and retries before entering the actual bracketing logic.

### Phase 3: Recover a Valid Bracketing Region

Before Ridder's method can be used, the code first tries to recover a pair of usable march evaluations. This recovery stage includes logic such as:

- increasing the estimate when the first guess collapses pressure too early
- decreasing the estimate when the first guess crosses IPR/PVT limits
- switching to denser or lighter hydrostatic assumptions to obtain a more useful second trial
- repeatedly shrinking or expanding the trial pressure until the march returns a finite residual instead of a failure sentinel

Only after this stabilization phase does the routine proceed to a true sign-changing bracket search.

### Phase 4: Sign-Changing Bracket Search

Once a finite residual is available, the routine searches for opposite-sign residuals:

- if `val < 0`, it stores the current pressure as the negative side and progressively **reduces** the trial pressure
- if `val > 0`, it stores the current pressure as the positive side and progressively **increases** the trial pressure

The factors used for reduction/amplification are based on `arq.buscaFC`:
- reduction factor: `1 - buscaFC`
- amplification factor: `1 + buscaFC`

This phase also contains additional protection logic:
- if pressure is reduced too far and the march returns a low-pressure failure sentinel, the code bisects back toward the last usable point
- if pressure is increased too far and the march returns a high-pressure failure sentinel, the code retreats from the limit similarly
- for some configurations, if the response appears to reverse unexpectedly, the routine may switch to the reverse-flow variant `buscaProdPfundoPermRev(...)`

### Phase 5: Root Finding

After a valid opposite-sign bracket is found, the routine calls:

```cpp
return zriddr(chuteNeg, chutePos, 1, 0);
```

Each evaluation inside `zriddr()` calls `multMarcha(..., prod=1, tipoCC=0)`, which dispatches to `marchaProdPerm1(...)` or its reverse-flow counterpart depending on `revPerm`.

---

<a id="step-5--marchaprodperm1-the-cell-by-cell-march"></a>
## Step 5 — marchaProdPerm1: The Cell-by-Cell March

`marchaProdPerm1()` ([`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) is the **core marching routine**. Given a pressure guess `pchute` at cell 0, it sweeps from cell 1 to cell `ncel`, computing all profiles.

### Initialization (Cell 0)

`marchaProdPerm1()` initializes the first production cell according to `celula[0].acsr.tipo`, but the actual logic is more detailed than a simple lookup table. The code sets temperature, optionally refreshes compositional properties, estimates initial gas/liquid split, and then seeds `celula[0].pres`, `celula[0].alf`, `celula[0].bet`, and the zero-flow condition at the closed upstream boundary.

A faithful summary is:

| `tipo` | Source Type | Initialization behavior |
|--------|------------|--------------------------|
| `0` | None | Uses `arq.celp[0].textern`, sets `alfini = 1`, `betini = 0`; in compositional mode may call `atualizaPropComp()` |
| `1` | Gas injection | Uses source temperature `injg.temp`; distinguishes dry-gas and non-dry-gas cases; for wet gas, estimates gas/liquid/complementary split from source mass flow and local properties |
| `2` | Liquid injection | Uses `injl.temp`; estimates initial gas and liquid volumes from `QLiq`, `RGO`, `BSW`, `BOFunc`, `BAFunc`, and `bet` |
| `3` | IPR | Uses reservoir temperature `ipr.Tres`; computes initial gas/liquid split from `MasG()` and `MasL()` at the guessed pressure |
| `10` | Mass source | Uses `injm.temp`; may call `renovaFonte(0)` when needed; estimates initial split from `MassG`, `MassP`, and `MassC` |
| `15` | Radial porous | Uses `radialPoro.tRes`; sets `celula[0].pres = pchute`, calls `renovaFonte(0)`, and derives initial split from `fluxIni`, `fluxIniA`, `fluxIniG` |
| `16` | 2D porous | Uses `poroso2D.dados.tRes`; sets `celula[0].pres = pchute`, calls `renovaFonte(0)`, and derives initial split from transfer fluxes |

After this source-specific initialization, the routine applies the steady-state upstream-boundary convention used by this marcher:
- the left boundary is treated as closed for the marching formulation
- `ML`, `MC`, `MliqiniL`, `Mliqini`, `QL`, `QLL`, and `QG` are initialized consistently with zero incoming flow at the left boundary
- pressure and phase-fraction values are copied into the left-boundary / previous-step storage used by the later updates

So, the first-cell initialization is best understood as **source-aware state seeding**, followed by enforcement of the marcher’s upstream boundary convention.

### Main March Loop

```
for i = 1 to ncel:
    1. RenovaPresPermMon(i)     — Pressure: cell[i-1].center → cell[i].left boundary
    2. atualizaPeriPmonProd(i)   — Apply artificial-lift ΔP at cell[i].left boundary
    3. RenovaMassPerm(i)         — Mass flow rates at cell i
    4. RenovaTempPerm(i)         — Temperature at cell i
    5. RenovaPresPermJus(i)      — Pressure: cell[i].left boundary → cell[i].center
    6. atualizaPeriPjusProd(i)   — Apply artificial-lift ΔP at cell[i].center
    7. RenovaTransMassPerm(i-1)  — Interphase mass transfer at cell i-1
```

Each step is described in detail in [The Marching Equations](#the-marching-equations).

### Early Termination

The march returns sentinel values if:
- `−1e10`: pressure collapsed below the admissible minimum during the march, or the guessed condition is too low for the current branch configuration
- `+1e10`: pressure exceeded an admissible limit (for example IPR/static-pressure or PVT-table limits), or the guessed condition is too high
- `−1.1e10` / `+1.1e10`: non-convergence / failed permanent solve in contexts such as network iterations, where the code distinguishes hard failure from a normal bracketing miss
- `NaN`/invalid state detection may also trigger an early failure path

### Return Value

```cpp
return pGSup - (celula[ncel].pres + corrigePresF);
```

The residual is the difference between the **imposed downstream pressure** (`pGSup`, typically the separator pressure) and the **computed pressure at the last cell**. When this equals zero, the steady state is found.

### Inner Iteration (Gas-Lift Coupling)

If the branch has gas-lift valves (`arq.lingas > 0`), after the production-line march, the method also:

1. **Marches the gas-lift service line** (`marchaGasPerm1` / `buscaGasPresPerm2` / `buscaGasPresPerm3`), solving for gas-injection rate given injection pressure or vice versa
2. **Connects columns** — gas-lift valve flow rates depend on the differential pressure between the production and service lines at each valve position

The entire march (production + gas line) may repeat (`iterperm` loop) until the gas-lift coupling converges.

---

## The Marching Equations

### Pressure March: `RenovaPresPermMon`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

`RenovaPresPermMon()` advances pressure from the center of cell `i-1` to the left boundary of cell `i`. At a high level, the update is driven by:

- hydrostatic contribution from local mixture density and inclination
- friction contribution based on local mixture velocity, viscosity, and Reynolds number
- the active drift/closure model (`tipoModeloDrift` branch)
- first-pass versus correction-pass logic (`RK == 0` or `RK == 1`)

In the common drift-model branch, the routine:
1. builds local phase/mixture properties from the current guess of pressure, temperature, void fraction, and complementary-liquid fraction
2. computes superficial velocities and mixture velocity
3. evaluates Reynolds number via `Cel::Rey()`
4. evaluates friction factor via `Cel::fric()`
5. forms hydrostatic and friction gradients over a half-cell distance
6. writes the resulting interface pressure into `celula[i].presaux`

So the clean hydrostatic-plus-friction equation shown in this document should be understood only as a **conceptual summary**; the actual implementation contains model branches, correction passes, and property-evaluation details.

The friction factor is computed by `Cel::fric()` ([`src/core/celula3.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celula3.cpp)):
- **Laminar** ($Re \leq 2400$): $f = 16 / Re$ (Fanning)
- **Turbulent** ($Re > 2400$): Haaland-style initialization followed by two Colebrook-style updates

Reynolds number via `Cel::Rey()`:

$$Re = \frac{D \cdot |v| \cdot \rho}{\mu \times 10^{-3}}$$

where viscosity $\mu$ is in centipoise (cP).

### Pressure March: `RenovaPresPermJus`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

`RenovaPresPermJus()` advances pressure from the left boundary of cell `i` to the center of cell `i`. As in `RenovaPresPermMon()`, the implementation is more detailed than a single closed expression. The update may include:

- hydrostatic and friction contributions over the right half-cell
- property re-evaluation using the current intermediate pressure/temperature state
- pressure-gain contribution inherited from upstream devices through `dpB`
- optional area-change correction `dpArea` when `celula[i].mudaArea == 1`
- first-pass (`RK == 0`) and correction-pass (`RK == 1`) variants
- an alternative branch using `executarCorrelacao(...)` when `tipoModeloDrift != 1`

For that reason, this section should be read as an **implementation summary of a half-cell pressure advance**, not as a single universal equation valid for all branches.

### Artificial Lift: `atualizaPeriPmonProd`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

After computing `presaux` at the cell boundary, this method updates the boundary pressure state to reflect artificial-lift or imposed pressure-gain devices attached to the upstream control volume.

Typical cases handled by the documentation are still representative:

| `acsr.tipo` | Device | Pressure Contribution |
|-------------|--------|-----------------------|
| `4` | BCS (ESP pump) | pressure gain derived from pump head and local mixture properties |
| `7` | Generic ΔP | user-imposed pressure increment |
| `17` | MultiBCS | delegated to the multi-stage pump model |

The exact applied increment is then carried forward through quantities such as `dpB`, which are reused by the later pressure and temperature updates.

### Mass Flow: `RenovaMassPerm`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

`RenovaMassPerm()` is one of the densest routines in the steady-state solver. It does more than simply “add source terms to the incoming mass flow”. A more faithful description is:

1. **Source refresh**
   - calls `renovaFonte(i - 1)` to evaluate the source/accessory attached to the upstream cell under the current local state
   - copies right-side source values from cell `i-1` into the left-side bookkeeping of cell `i`

2. **Transported-flow bookkeeping**
   - propagates complementary-liquid transport (`MComp`)
   - determines the effective flow direction / sign used by several updates

3. **Thermodynamic-property update of the carried fluid**
   - evaluates black-oil style quantities such as `RS`, `BOFunc`, `BAFunc`
   - updates in-situ water fraction `FW`
   - updates carried fluid descriptors such as `RGO`, `BSW`, gas density surrogate (`Deng`), `yco2`, and, when relevant, `API`, `Denag`, and ASTM viscosity-pair support data
   - applies different mixing/update rules depending on the upstream accessory type

4. **Accessory-specific mixing logic**
   - no source / passive cell
   - gas source (`tipo == 1`, including dry-gas and wet-gas behavior)
   - liquid source (`tipo == 2`)
   - IPR (`tipo == 3`)
   - mass source (`tipo == 10`)
   - porous/radial / 2D porous / leak-like special cases in later branches

5. **Complementary-liquid fraction update**
   - recomputes `bet` when the mixture composition changes enough to require a new in-situ complementary-liquid fraction

6. **Fluid-model refresh**
   - for black-oil style operation, calls `RenovaFluido()` / `corrDeng()` after composition-like quantities are updated
   - for more complex configurations, preserves or reuses previously updated values according to the active model branch

So, `RenovaMassPerm()` should be understood as a **combined transported-mass update + fluid-mixture recomposition routine**, not merely as a conservation-law increment.

The earlier simplified summary still captures the broad intent:
- upstream source terms are evaluated at cell `i-1`
- transported mass quantities are propagated into cell `i`
- local fluid descriptors are recomputed to remain consistent with the changed mixture

### Source Terms: `renovaFonte`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

Evaluates mass source terms at a cell based on the accessory type:

| `acsr.tipo` | Source | Evaluation |
|-------------|--------|------------|
| `1` | Gas injection | `VMas(P, T)` → split into gas + liquid via flash / source model |
| `2` | Liquid injection | `QLiq` → split via local black-oil / fluid-property relations |
| `3` | IPR | `MasG(P,T)`, `MasL(P,T)` from the inflow-performance model |
| `5` | Choke / restriction-related source branch | case-specific flow contribution |
| `10` | Mass source | Direct `MassG`, `MassP`, `MassC` specification |
| `15` | Radial porous | Near-wellbore radial model |
| `16` | 2D porous | Near-wellbore 2D model |

Results are stored in the right-side source accumulators such as `fontemassGR`, `fontemassLR`, and `fontemassCR` for later propagation into the next cell.

### Temperature: `RenovaTempPerm`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

`RenovaTempPerm()` advances the production-line temperature using a **full energy-balance update**, not a single compact textbook equation. In the implementation, the temperature change combines several effects evaluated with local phase properties and flow rates:

- pressure-work / Joule-Thomson terms for gas and liquid contributions
- hydrostatic work
- optional kinetic-energy correction
- enthalpy carried by source terms (gas, liquid, IPR, porous inflow, leaks)
- optional latent-heat contribution when phase change is enabled
- radial heat transfer through the pipe wall via `calor.transperm()`
- optional annulus / gas-lift thermal coupling when the column is thermally connected to the service line

Important implementation details:
- the routine works with local mixture quantities assembled from gas, produced liquid, and complementary liquid
- for strong heat transfer, the code may **substep the temperature update** for stability instead of applying the whole change in one step
- if mixture velocity is very small, the code falls back to the external/ambient temperature model rather than using the full convective energy balance
- the computed temperature is clipped to an admissible interval in the implementation

So, this method should be read as a **numerical energy-balance march with heat-transfer and source-term coupling**, not as a single closed-form equation.

### Interphase Mass Transfer: `RenovaTransMassPerm`

**Source:** [`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)

`RenovaTransMassPerm()` computes the interphase mass-transfer term used by the energy model and by the steady-state bookkeeping of phase change. Conceptually, it represents gas liberation/absorption caused by changing thermodynamic state along the branch, but the implementation is more specific than a simple proportionality such as $\partial(R_s/B_o)/\partial x$.

In the code, the term is assembled from finite differences between neighboring-cell states and includes:
- solution-gas ratio `RS`
- oil formation volume factor `BOFunc`
- local BSW-dependent in-situ water fraction
- complementary-liquid fraction `bet`
- gas specific gravity / density correction terms
- local liquid volumetric flow quantities used to convert the thermodynamic change into a mass-transfer rate per unit length

The resulting quantity is stored in `transmassR`, propagated to the neighboring left-side value, and mirrored into `FonteMudaFase`. For compositional-gas style handling, the code may instead use `RenovaTransMassPermGas()`.

---

## Convergence and Root Finding

### multMarcha — March Dispatcher

`multMarcha()` ([`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) is the dispatch function called by the root finder:

| `prod` | `tipoCC` | Function Called |
|--------|----------|-----------------|
| `0` (gas) | `0` | `marchaGasPerm2` |
| `0` (gas) | `1` | `marchaGasPerm3` |
| `1` (production) | `0` | `marchaProdPerm1` (or `marchaProdPerm1Rev`) |
| `1` (production) | `1` | `marchaProdPerm2` |
| `2` (P-P) | `0` | `marchaProdPresPres1` (or `marchaProdPresPres1Rev`) |
| `2` (P-P) | `1` | `marchaProdPresPres2` |
| injection | any | `marchaInjPerm1` |

> **Implementation note:** although the higher-level permanent solver has separate logic for closed-choke pressure/pressure cases, the `multMarcha()` dispatcher itself currently routes the `prod = 2`, `tipoCC = 1` branch to `marchaProdPresPres2()`.

### zriddr — Ridder's Method

`zriddr()` ([`src/core/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) implements a practical **Ridder-style bracketed root finder**:

1. Starts from two guesses with opposite residual signs
2. Re-evaluates / nudges the bracket ends if necessary
3. Iterates up to 100 times with a Ridder update based on the midpoint and
   $$
   s = \sqrt{f_m^2 - f_l f_h}
   $$
4. Re-evaluates the march residual via `multMarcha(...)` and contracts the bracket

Important implementation notes:
- the code uses `xacc = 1e-5` as the principal **root/interval tolerance**
- parts of the residual tracking are scaled by `monitConvPermBase`, so the stopping logic is **not simply** “raw residual $|f| < 10^{-5}$ kgf/cm²”
- `monitConvPerm` is updated for monitoring using normalized residual values in several branches

So the most accurate summary is: **`zriddr()` uses a Ridder-type bracketed solve with a root-position tolerance of about `1e-5`, plus normalized residual monitoring.**

---

## Boundary Condition Variants

### `marchaProdPerm1` — Pressure at Downstream End

- **Unknown**: bottom-hole pressure $P_0$
- **March direction**: cell 0 → cell ncel
- **Residual**: $r = P_\text{sep} - P_{\text{ncel}}$
- **Called by**: `buscaProdPfundoPerm`

### `marchaProdPerm2` — Choke at Downstream End

- **Unknown**: bottom-hole pressure $P_0$
- **March direction**: cell 0 → cell ncel
- **Residual**: $r = \dot{m}_{\text{ncel}} - \dot{m}_\text{choke}(P_{\text{ncel}}, P_\text{sep})$
- **Called by**: `buscaProdPfundoPerm2`

When the choke is active (small opening), the downstream BC is no longer a simple pressure match. Instead, the choke equation relates the flow through the restriction to the pressure drop across it. The residual becomes the mismatch between the computed flow arriving at the choke and the flow the choke would pass at the computed upstream pressure.

### `marchaProdPresPres1` — Pressure at Both Ends

- **Unknown**: mass flow rate $\dot{m}_0$
- **March direction**: cell 0 → cell ncel
- **Residual**: $r = P_\text{sep} - P_{\text{ncel}}$
- **Called by**: `buscaProdPresPresPerm`

When both upstream and downstream pressures are known (e.g., a subsea pipeline between two platforms), the flow rate is the unknown.

### Reverse Flow Variants

When the source at cell 0 has negative flow rate, the solver switches to `marchaProdPerm1Rev` / `marchaProdPresPres1Rev`, which handle the reversed flow direction in the friction and mass-transfer terms.

---

## Gas-Lift Service Line

When a gas-lift service line is present (`arq.lingas > 0`), the steady-state solver must find a self-consistent solution between the production line and the gas-lift annulus.

### The Gas-Lift Service Line Model

The gas-lift service line is discretized into its own 1D cell array `celulaG[]` of type `CelG` (defined in [`src/include/celulaGas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celulaGas.h) / [`src/core/celulaGas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celulaGas.cpp)). Each `CelG` cell stores, among other things:

| Category | Key Members |
|----------|-------------|
| Geometry | `dutoL`, `duto`, `dutoR` |
| Pressure | `pres`, `presL`, `presR` |
| Temperature | `temp`, `tempL`, `tempR` |
| Flow variables | `VGasL`, `VGasR`, `VGasRR`, together with `u1LL`, `u1L`, `u1R` (density-area products used to convert between transported quantity and velocity) |
| Valve/source term | `massfonteCH` — valve / choke extraction or injection source term in the gas-line cell |
| Interface tracking | `razInter`, `razInterIni`, `celInter` |
| Stagnation/throat trend variables | `pEstag`, `tEstag`, `pGarg`, `tGarg`, `qGarg`, `areaGarg` |
| Fluid/thermal model | `flui`, `calor`, `chkcell` |

> **Implementation note:** although the variable names suggest “gas flow”, the gas-line model carries these quantities together with density-area terms (`u1L`, etc.), so it is safer to describe `VGasL/VGasR/VGasRR` as the gas-line transported flow variables used by the annulus solver, rather than as plain volumetric or plain mass flow without qualification.

### Gas-Line Marching Functions

At the production-line level, the steady-state branch solver couples to the gas line through three main entry points:

| Function | Role in branch solve |
|----------|----------------------|
| `marchaGasPerm1()` | Used when the gas-line inlet condition is injection pressure and the injection choke is effectively open |
| `buscaGasPresPerm2()` | Used when the gas-line condition is injection flow rate |
| `buscaGasPresPerm3()` | Used when the gas-line inlet is pressure-controlled but the injection choke is active/restricting |

More specifically:
- `marchaGasPerm1()` iterates on the **total injected gas mass rate**, repeatedly marching the annulus and recomputing the sum of valve outflows until the inlet-flow estimate and outlet valve extraction become consistent
- `marchaGasPerm2()` performs one gas-line march for a guessed inlet pressure and prescribed injection flow, returning the residual
  $$
  \sum \dot m_{\mathrm{VGL}} - \dot m_{\mathrm{inj}}
  $$
- `marchaGasPerm3()` performs one gas-line march for a guessed pressure downstream of the injection choke, using `chokeInj.massica()` to compute inlet mass flow, and returns
  $$
  \sum \dot m_{\mathrm{VGL}} - \dot m_{\mathrm{choke}}
  $$

Within all three gas-line marches, the per-cell sequence is effectively:
1. `RenovaPresGasPerm(i)`
2. `RenovaTempGasPerm(i)`
3. `calcVazGasPerm(i)`
4. update density / density-area bookkeeping (`rg`, `u1L`, `u1R`, `u1LL`)

So the gas-line helper breakdown can be stated explicitly for the current code.

---

## Compositional Model Acceleration

For compositional fluid models (`flashCompleto == 2`), `SolveTramoSolteiro` employs the two-pass approach described in [Step 2](#step-2--solvetramosolteirofluid-model-strategy).

Additionally, the `preparaTabDin()` function ([`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) can build **dynamic PVT tables** from the black-oil result. These pre-tabulated flash results cover the pressure and temperature range discovered during the first pass, making the compositional pass much faster by avoiding repeated flash calculations.

The approach within `preparaTabDin`:
1. Identifies P,T range from the black-oil solution
2. Performs flash calculations at a grid of (P,T) points
3. Stores results in look-up tables on each cell's fluid object
4. During the compositional march, `atualizaPropComp()` interpolates from the table instead of solving the full flash

---

## Output and Post-Processing

After `permanenteSimples` returns successfully, `SolveTramoSolteiro` writes:

1. **Profiles** via `arq.imprimeProfile()` — pressure, temperature, void fraction, velocities, flow rates vs. position
2. **Summary** via `arq.resumoPermanente()` — key values at inlet and outlet
3. **2D Poisson** results if thermal diffusion was coupled
4. **Near-wellbore profiles** (radial/2D porous models)
5. **Trend files** for each monitored variable at specified positions
6. **Gas-lift line profiles** if applicable
7. **Log file** with convergence status, timing, and version information

---

## Summary of Key Methods

| Method | File | Purpose |
|--------|------|---------|
| `SolveTramoSolteiro` | `src/core/Num4Main.cpp` | Top-level wrapper: handles compositional two-pass logic, calls `permanenteSimples`, writes output |
| `permanenteSimples` | `src/core/Num4Main.cpp` | Boundary-condition dispatcher and coupled gas-lift orchestration |
| `buscaProdPfundoPerm` | `src/core/SisProd.cpp` | Robust bracket-search and Ridder-style solve for the inlet / bottom-hole pressure in the standard production-pressure case |
| `buscaProdPfundoPerm2` | `src/core/SisProd.cpp` | Pressure search for the active-choke production case |
| `buscaProdPresPresPerm` | `src/core/SisProd.cpp` | Bracketed solve for inlet flow / mass-rate proxy when inlet pressure is prescribed |
| `marchaProdPerm1` | `src/core/SisProd.cpp` | Main production-line marching routine; returns the downstream pressure residual |
| `marchaProdPerm2` | `src/core/SisProd.cpp` | Production marcher for active-choke cases; returns the choke-flow residual |
| `marchaProdPresPres1` | `src/core/SisProd.cpp` | March routine for prescribed-pressure / prescribed-pressure production cases |
| `multMarcha` | `src/core/SisProd.cpp` | Dispatch helper used by the root finder |
| `zriddr` | `src/core/SisProd.cpp` | Ridder-style bracketed root finder with normalized residual monitoring |
| `RenovaPresPermMon` | `src/core/SisProd.cpp` | Half-cell pressure advance from cell center to left boundary |
| `RenovaPresPermJus` | `src/core/SisProd.cpp` | Half-cell pressure advance from left boundary to cell center |
| `atualizaPeriPmonProd` | `src/core/SisProd.cpp` | Applies upstream artificial-lift / imposed pressure-gain effects at the interface state |
| `RenovaMassPerm` | `src/core/SisProd.cpp` | Transported-mass update plus fluid-mixture recomposition |
| `RenovaTempPerm` | `src/core/SisProd.cpp` | Numerical energy-balance march with heat-transfer and source coupling |
| `RenovaTransMassPerm` | `src/core/SisProd.cpp` | Interphase mass-transfer update used by the steady-state energy model |
| `renovaFonte` | `src/core/SisProd.cpp` | Evaluates local source terms (gas/liquid/mass injection, IPR, porous, and related cases) |
| `Cel::fric` | `src/core/celula3.cpp` | Fanning friction factor evaluation |
| `Cel::Rey` | `src/core/celula3.cpp` | Reynolds-number evaluation |
| `hidroreverso` | `src/core/SisProd.cpp` | Reverse-direction hydrostatic estimate helper |
| `preparaTabDin` | `src/core/Num4Main.cpp` | Builds dynamic PVT tables for compositional acceleration |
| `marchaGasPerm1` | `src/core/SisProd.cpp` | Iterative gas-line steady-state march for pressure-controlled injection |
| `marchaGasPerm2` | `src/core/SisProd.cpp` | Single gas-line march for flow-controlled injection; returns annulus residual for root finding |
| `marchaGasPerm3` | `src/core/SisProd.cpp` | Single gas-line march for injection-choke-controlled cases; returns annulus residual for root finding |
| `RenovaPresGasPerm` | `src/core/SisProd.cpp` | Gas-line pressure advance |
| `RenovaTempGasPerm` | `src/core/SisProd.cpp` | Gas-line temperature advance |
| `calcVazGasPerm` | `src/core/SisProd.cpp` | Gas-line valve-flow calculation and annulus source update |
| `TempDescGL` | `src/core/SisProd.cpp` | Gas temperature drop across VGL / gas-lift throttling model |
| `CelG::fric` | `src/core/celulaGas.cpp` | Friction-factor evaluation for gas-line cells |
| `CelG::Rey` | `src/core/celulaGas.cpp` | Reynolds-number evaluation for gas-line cells |
| `chokeVGL[].massica` | `src/core/chokegas.cpp` | Compressible gas choke mass-flow model |


---

## Available Steady-State Multiphase Flow Models

The steady-state branch solver can evaluate multiphase pressure gradient and liquid holdup with two broad model families:

1. the **default drift-flux formulation**, which is the native Marlim3 branch model and the same base model used by the transient solver, and
2. the **standalone steady-state correlations/mechanistic models** dispatched from [`src/core/GradientCorrelations.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/GradientCorrelations.cpp), including the Gomez model in [`src/core/GomezModel.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/GomezModel.cpp).

### Default model: drift-flux formulation

The default steady-state model is the internal **drift-flux** formulation used by the branch marcher itself.

This is the same physics family used by the transient solver: each cell updates pressure, holdup/void fraction, mixture properties, source terms, and heat transfer using the native Marlim3 closure variables such as `c0`, `ud`, `term1`, and `term2`, together with the `Cel` and `SProd` marching routines documented in this guide.

Use this model as the **reference / standard** steady-state branch formulation when no standalone correlation is explicitly selected.

### Correlation-based steady-state models

The function `executarCorrelacao(...)` in [`src/core/GradientCorrelations.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/GradientCorrelations.cpp) exposes the following steady-state multiphase models through the integer selector `correlacao`.

| `correlacao` | Model | Summary |
|---|---|---|
| `0` | Poettmann-Carpenter | Classical vertical-flow correlation; implemented here with no-slip-style holdup. |
| `1` | Baxendell-Thomas | Vertical-flow correlation; implemented here with no-slip-style holdup. |
| `2` | Fancher-Brown | Vertical tubing/well correlation. |
| `3` | Hagedorn-Brown | Vertical two-phase model with bubble and slug/churn handling and acceleration term. |
| `4` | Duns-Ros | Vertical model with bubble, slug, mist, and transition regimes. |
| `5` | Orkiszewski | Vertical model with bubble, slug, mist, and transition handling. |
| `6` | Beggs & Brill | Inclined-pipe correlation with regime-dependent holdup and gradients. |
| `7` | Mukherjee-Brill | Inclined-flow model covering bubble, slug, mist, and stratified regimes. |
| `8` | Aziz | Aziz-Govier-Fogarasi style vertical-flow model. |
| `9` | Gray | Gas-well-oriented vertical correlation with liquid production. |
| `10` | Oliemans | Annular/dispersed-flow-oriented model with Dukler-based slip correction. |
| `11` | Dukler | Dukler pressure-gradient framework with Dukler holdup. |
| `12` | Beggs & Brill with Palmer correction | Same as `6`, with Palmer correction enabled. |
| `13` | Dukler + Eaton + Flanigan | Dukler framework using Eaton holdup / Flanigan elevation correction. |
| `14` | Dukler + Minami I | Dukler framework using Minami I holdup. |
| `15` | Dukler + Minami II | Dukler framework using Minami II holdup. |
| `16` | Gomez mechanistic model | Unified mechanistic model for horizontal-to-vertical upward two-phase flow. |

### Correlation family notes

Important implementation notes:

- **Poettmann-Carpenter**, **Baxendell-Thomas**, and **Fancher-Brown** are simpler vertical correlations and, in this implementation, use no-slip-style holdup assumptions.
- **Hagedorn-Brown**, **Duns-Ros**, **Orkiszewski**, **Beggs & Brill**, **Mukherjee-Brill**, **Aziz**, and **Gray** include explicit slip and/or flow-regime logic.
- The **Dukler family** (`11`, `13`, `14`, `15`) shares the same pressure-gradient framework and mainly changes the holdup subcorrelation:
  - `11`: Dukler holdup,
  - `13`: Eaton holdup,
  - `14`: Minami I holdup,
  - `15`: Minami II holdup.
- **Beggs & Brill with Palmer correction** is exposed as a dedicated option rather than a flag hidden from the user.
- **Oliemans** is implemented with a slip correction that reuses Dukler-style support quantities.

### Gomez mechanistic model

[`src/core/GomezModel.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/GomezModel.cpp) implements a **unified mechanistic model for steady-state two-phase flow from horizontal to vertical upward flow**, based on Gomez, Shoham, Schmidt, Chokshi, and Northug (2000).

This model is selected in `executarCorrelacao(...)` with `correlacao == 16`.

The main solve is performed by `calculateGomez(...)`, which first predicts a flow pattern and then dispatches to a pattern-specific submodel.

#### Flow patterns represented by the Gomez implementation

The current implementation predicts the following patterns:

- `StratifiedSmooth`
- `StratifiedWavy`
- `Slug`
- `AnnularMist`
- `Bubble`
- `DispersedBubble`

#### Main submodels used by Gomez

The implementation uses the following internal pattern-specific solvers:

- `solveStratified(...)`
- `solveSlug(...)`
- `solveAnnular(...)`
- `solveBubble(...)`
- `solveDispersedBubble(...)`

Flow-pattern selection is performed by `predictFlowPattern(...)`, and boundary smoothing is handled by `applySmoothing(...)`.

#### Main implementation features of the Gomez model

This implementation includes:

- **stratified geometry reconstruction** from liquid level,
- **annular geometry reconstruction** from film thickness,
- **pattern-specific force balances** for holdup and pressure gradient,
- **slug-film bifurcation** between near-vertical and lower-inclination cases, and
- **transition smoothing** near slug↔bubble and slug↔annular boundaries.

For slug flow, two film treatments are used:

- a **symmetric vertical film** model for inclinations $\theta \ge 86^\circ$, and
- a **stratified/horizontal film** model for inclinations below that threshold.

#### Quantities returned by the Gomez model

`calculateGomez(...)` returns, in SI-like internal form:

- predicted flow pattern,
- liquid holdup `HL`,
- frictional pressure gradient `dpdL_fric`, and
- gravitational pressure gradient `dpdL_grav`.

Inside `executarCorrelacao(...)`, these are converted into the field-unit outputs expected by the legacy correlation interface:

- `holdup`,
- `frictionGrad`,
- `gravityGrad`, and
- `totalGrad`.

#### Practical interpretation

Among the available steady-state alternatives, Gomez is the most explicitly **mechanistic** option in this codebase. Unlike the more empirical correlations in [`src/core/GradientCorrelations.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/GradientCorrelations.cpp), it first identifies the flow pattern and then applies a pattern-specific closure for holdup and pressure gradient.
