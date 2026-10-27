# Transient Solver for a Single Branch

This document describes the logic and algorithms used by the Marlim3 simulator to compute the **transient (time-dependent) solution** of a single pipeline segment (*branch*). While the [steady-state solver](steady-state-branch.md) uses a shooting method with cell-by-cell spatial marching, the transient solver advances the entire domain in time, coupling mass, momentum, and energy equations through a **drift-flux multi-phase model** on a staggered grid.

**Key source files:**

| File | Role |
|------|------|
| [`src/SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) | `SolveTrans()` main loop and all system-level transient methods |
| [`src/SisProd.h`](https://github.com/petrobras/marlim3/blob/main/src/include/SisProd.h) | `SProd` class declaration |
| [`src/celula3.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celula3.cpp) | `Cel` class — cell-level transport, matrix assembly, state rewind |
| [`src/celula3.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celula3.h) | `Cel` class declaration and member variables |
| [`src/PropFlu.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFlu.h) / [`PropFlu.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFlu.cpp) | Fluid property correlations (black-oil & compositional) |
| [`src/estrat.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/estrat.cpp) | Stratified flow model (Taitel-Dukler map) |
| [`src/FonteMas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/FonteMas.cpp) | Mass source term evaluation |
| [`src/TrocaCalor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/TrocaCalor.cpp) | Radial heat-transfer (pipe wall, insulation, seabed) |
| [`src/celulaGas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celulaGas.h) / [`celulaGas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celulaGas.cpp) | `CelG` class — gas-lift service line cell state, matrix assembly, rewind |
| [`src/chokegas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/chokegas.h) / [`chokegas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/chokegas.cpp) | `ChokeGas` — compressible gas choke model for VGL orifices |

---

## Table of Contents

1. [Overview](#overview)
2. [Conservation Equations](#conservation-equations)
3. [The Drift-Flux Closure Model](#the-drift-flux-closure-model)
4. [Time Stepping and CFL Condition](#time-stepping-and-cfl-condition)
   - [Time Step Heuristics](#time-step-heuristics-determinadt)
5. [SolveTrans — Top-Level Walkthrough](#solvetrans--top-level-walkthrough)
6. [EvoluiFrac — Void Fraction Evolution](#evoluifrac--void-fraction-evolution)
7. [SolveAcopPV — Pressure-Velocity Coupling](#solveacoppv--pressure-velocity-coupling)
8. [marchaEnergTrans — Transient Energy Equation](#marchaenergtrans--transient-energy-equation)
9. [Boundary Conditions](#boundary-conditions)
10. [State Finalization (renova)](#state-finalization-renova)
11. [Closure-Law Update (renovaterm)](#closurelaw-update-renovaterm)
12. [Gas-Lift Service Line Coupling](#gas-lift-service-line-coupling)
13. [Mass Transfer Model](#mass-transfer-model)
   - [TMModelL — Model Selection](#tmmodell--mass-transfer-model-selection)
   - [CritCond — Phase Transition Cutoff](#critcond--phase-transition-cutoff)
   - [Mass Transfer Attributes](#mass-transfer-attributes-per-cell)
14. [PIG Tracking](#pig-tracking)
15. [Adaptive Model Complexity](#adaptive-model-complexity)
16. [Time Step Restart Logic](#time-step-restart-logic)
17. [Compositional and Scalar Transport](#compositional-and-scalar-transport)
18. [Output and Post-Processing](#output-and-post-processing)
19. [Summary of Key Methods](#summary-of-key-methods)

---

## Overview

The transient solver computes the **time evolution of pressure, velocity, void fraction, complementary liquid fraction, and temperature** along a branch. The algorithm can be summarized as:

1. **Determine the time step** $\Delta t$ from CFL and operational constraints
2. **Advance the void fraction** $\alpha$ and complementary liquid fraction $\beta$ explicitly in time (hyperbolic transport)
3. **Solve the coupled pressure-velocity system** implicitly via a banded linear system
4. **March the energy equation** (temperature) semi-implicitly
5. **Finalize** state variables, update fluid properties, write output, and advance time

Unlike the steady-state solver which marches spatially from one end to the other, the transient solver marches **in time**, updating all cells simultaneously at each time step.

The drift-flux model is **always** used in transient mode (regardless of the `tipoModeloDrift` setting, which only affects the steady-state solver). This means the phase velocities are not solved independently; instead, a kinematic relation links gas velocity to mixture velocity via the distribution parameter $C_0$ and drift velocity $u_d$.

```
┌──────────────────────────────────────────────────────────────────────┐
│                     SolveTrans — Main Time Loop                      │
│                                                                      │
│  for each time step:                                                 │
│    ┌─────────────────────────────────────────────────────────────┐   │
│    │ 1. determinaDT()       — compute Δt (CFL + constraints)    │   │
│    │ 2. solveLinGas()       — gas-lift service line              │   │
│    │ 3. atualizaCC1()       — update boundary conditions        │   │
│    │ 4. avaliaVariaDpDt()   — adaptive model complexity         │   │
│    │ 5. EvoluiFrac()        — advance α, β explicitly           │   │
│    │ 6. AtualizaPig()       — PIG tracking                      │   │
│    │ 7. calcCCpres()        — outlet pressure BC                │   │
│    │ 8. renovaterm()        — update c₀, ud (drift-flux closure) │   │
│    │ 9. SolveAcopPV()       — solve for P, ṁ
│    │10. renova()            — extract P, M from solution vector  │   │
│    │11. marchaEnergTrans()  — energy equation (temperature)      │   │
│    │12. renovaTemp()        — update T-dependent properties      │   │
│    │13. renovaMasEsp()      — update densities                   │   │
│    │14. renovaRGOdgYco2()   — advect RGO, API, BSW, CO₂         │   │
│    │15. Output              — profiles, trends, snapshots        │   │
│    └─────────────────────────────────────────────────────────────┘   │
│    advance time: t += Δt                                             │
└──────────────────────────────────────────────────────────────────────┘
```

---

## Conservation Equations

The transient solver solves four conservation equations for a 1D multiphase mixture flowing through a pipe of cross-sectional area $A$ and perimeter $S$:

### Mass conservation — gas phase

For the full derivation, see [mass-momentum-balances.md](../theoretical-reference/mass-momentum-balances.md). The solver tracks three mass equations: produced liquid ($\dot{M}_p$), complementary (water) liquid ($\dot{M}_c$), and gas ($\dot{M}_g$). The gas mass equation includes interphase mass transfer $\Gamma_g$ from flashing/condensation via the black-oil $R_s$ term. See the theoretical reference (Equation `eq:mass_gas_final`) for the complete form.

### Mass conservation — produced liquid and complementary liquid phases

The produced-liquid and complementary-liquid mass equations are coupled through the complementary fraction $\beta$. The liquid density is computed as:

$$
\rho_l = (1-\beta)\,\rho_{lp} + \beta\,\rho_{lc}
$$

where $\rho_{lp}$ is the produced-liquid density (oil + dissolved gas) and $\rho_{lc}$ is the complementary-liquid density (water). See the theoretical reference (Equations `eq:mass_liq_final` and `eq:psi_black oil`) for the complete formulation including interphase transfer terms.

### Mixture momentum conservation

For the full derivation, see [mass-momentum-balances.md](../theoretical-reference/mass-momentum-balances.md). The momentum equation uses the **Fanning** friction factor $f_m$ (not the Darcy factor). In the staggered grid discretization, momentum is solved at cell faces (half-integer indices) while mass variables live at integer-index cells. See the theoretical reference (Equation `eq:momentum_final`) for the complete form with the wetted perimeter $S_w$ formulation.

### Energy conservation

For the complete derivation, see [energy-balance.md](../theoretical-reference/energy-balance.md). The solver uses a non-conservative temperature equation derived from the conservative internal energy form. The final equation (Equation `eq:energy_final` in the theoretical reference) relates temperature evolution to advection, Joule-Thomson effects, compression work, interphase enthalpy transfer, and radial heat loss to the wellbore environment.

### Discretization approach

- **Void fraction** ($\alpha$) and **complementary liquid fraction** ($\beta$) are advanced **explicitly** in time using upwind finite differences (`avancalf`, `avancbet`)
- **Pressure** ($P$) and **mixture mass flow** ($\dot{m}$) are solved **implicitly** via a coupled banded linear system (`GeraLocal` → `SolveAcopPV`)
- **Temperature** ($T$) is advanced **semi-implicitly** — the advection is explicit, but heat loss is evaluated at the new time level (`calctemp` inside `marchaEnergTrans`)

---

## The Drift-Flux Closure Model

The drift-flux model provides the **kinematic closure** that relates gas velocity to mixture velocity. Instead of solving separate momentum equations for each phase, the gas velocity is expressed as:

$$
u_g = C_0 \, j + u_d
$$

where:
- $j = u_{sg} + u_{sl}$ is the mixture superficial velocity
- $C_0$ is the **distribution parameter** (accounts for non-uniform void fraction and velocity profiles across the pipe cross-section; $C_0 > 1$ means gas concentrates in the faster-flowing core)
- $u_d$ is the **drift velocity** (accounts for buoyancy-driven relative motion between phases)

### CalcC0Ud — Flow-pattern-dependent closure

The method `SProd::CalcC0Ud()` (`SisProd.cpp`) computes $C_0$ and $u_d$ for each cell face. The calculation depends on the **flow pattern**, which is determined by flow-pattern maps:

1. **Near-horizontal pipes** ($|\theta| < 45°$): the **Taitel-Dukler** map (`estrat::mapaTD()`) classifies the flow as:
   - **Stratified** → blended `C0UdEstratificado` / `C0UdDisperso`
   - **Intermittent (slug)** → `C0UdDisperso` (dispersed-bubble/slug closure)
   - **Annular** → `C0UdAnularChurn`

2. **Inclined/vertical pipes**: the **Barnea-type** map (`arranjo::verificaArr()`) classifies the flow and dispatches to the appropriate closure.

3. **PIG present**: forces homogeneous flow ($C_0 = 1$, $u_d = 0$) around the PIG location.

4. **Smooth transitions**: when the flow pattern changes between time steps, the code applies a relaxation (blending over a `transic` counter) to avoid abrupt jumps in $C_0$ and $u_d$. The counter starts at zero when the flow pattern first changes and is incremented each time step. Only when `transic >= 20` does the blending complete. During the blend, $C_0$ and $u_d$ are linearly interpolated. Additionally, if the flow pattern differs between the left and right faces of a cell, the left-face drift-flux parameters are computed as a weighted average of the two flow-pattern-specific values.

5. **Optional override**: if `escorregaTran == 0` in the input, slip is disabled globally ($C_0 = 1$, $u_d = 0$), reducing the model to a homogeneous mixture.

6. **PIG/BCS enforcement**: at faces where a PIG is present or a BCS pump (tipo 8) is operating, homogeneous flow is forced ($C_0 = 1$, $u_d = 0$) regardless of the flow-pattern classification. This prevents unphysical slip predictions in regions where the physical geometry (PIG, mechanical pump) constrains phase mixing.

The closure is called during the `renovaterm()` phase (step 8 in the main loop), which propagates void fractions to cell faces and then evaluates the drift-flux parameters at each interior face.

---

## Time Stepping and CFL Condition

The time step $\Delta t$ is determined by `SProd::determinaDT()` and must satisfy multiple constraints:

### CFL condition (explicit mode)

When the explicit solver is active (`vExpli == 1`), the time step is limited by the Courant-Friedrichs-Lewy condition:

$$
\Delta t \leq \min_i \frac{\Delta x_i}{\max\left(|u_{\text{adv},i} + c_i|, \; |u_{\text{adv},i} - c_i|\right)}
$$

where:
- $u_{\text{adv},i}$ = advection velocity at cell $i$ (from `Cel::termAdSomVel()`)
- $c_i$ = sound speed in the mixture (from `Cel::somVel()`)

This is computed by `determinaDTExpli()`, which loops over all cells and takes the global minimum.

### Time Step Heuristics (`determinaDT`)

`SProd::determinaDT()` applies a hierarchy of constraints and operational penalties on top of the CFL limit. Each constraint is evaluated in sequence, and the minimum survives:

1. **Base CFL**: the primary constraint comes from the acoustic speed (gas velocity limit) and thermal advection speed, evaluated separately for the production line and the gas-lift service line
2. **Maximum user limit**: the Δt is never allowed to exceed the user-defined maximum from the JSON input (`arq.dtmax`)
3. **Valve opening penalties**: during opening or closing of surface chokes or Master1 valves, if the pressure difference between upstream and downstream is extreme, the solver applies an aggressive Δt penalty until pressures stabilize. This prevents choking-solver oscillations during valve transients
4. **Choke oscillation penalization**: during production shutdown, the surface choke model can exhibit mass flow oscillations where the liquid flow rate at the choke upstream face flips between positive and negative from one step to the next. When detected, the solver reduces Δt until the oscillation is under control. Without this, time steps can become infinitesimally small and the simulation may stall
5. **Liquid-piston arrival**: when a liquid piston (slug) reaches the last cell while the surface choke is "active," the solver has difficulty resolving the sudden change in choke flow conditions. A Δt penalty is applied until the solver adapts
6. **Sub-time-step limit**: if `subtempo > 0`, the solver computes a sub-Δt (typically based on the time for a slug to traverse a cell) and limits Δt accordingly

### Monophase transition band

The simulator uses a transition band to avoid excessive time-step penalization near the monophase boundaries ($\alpha = 0$ or $\alpha = 1$). The threshold is configured via the JSON entry **`Avancado.CriterioMonofasico`** (default `1e-4`), which is internally assigned to `(*vg1dSP).localtiny`. Rather than checking for exact $\alpha = 0$ or $\alpha = 1$, the code applies:

- If $\alpha < \text{localtiny}$: the cell is effectively all-liquid
- If $\alpha > 1 - \text{localtiny}$: the cell is effectively all-gas
- Otherwise: true multiphase

This prevents the solver from oscillating around the monophase boundary, which would trigger endless time-step halving. For $\beta$, an analogous check uses a per-cell `localtinyTemp` (defaulting to the global `localtiny` or `1e-9`).

### Time step restart

If the explicit advance of $\alpha$ produces out-of-bounds values ($\alpha < 0$ or $\alpha > 1$), the cell computes the maximum stable sub-step `dt1` (or `dt2` for $\beta$, `dtPig` for PIG), sets `reinicia = -1`, and the main loop **undoes** the entire step, halves $\Delta t$, and retries. See [Time Step Restart Logic](#time-step-restart-logic).

---

<a id="solvetrans--top-level-walkthrough"></a>
## SolveTrans — Top-Level Walkthrough

`SProd::SolveTrans()` (`SisProd.cpp`) orchestrates the entire transient computation. Here is the step-by-step logic:

### Initialization (before the time loop)

1. **Compositional mini-table**: if `flashCompleto == 2`, initializes the compositional PVT interpolation tables
2. **Initial output**: writes the profile and trend data at $t = 0$
3. **Initial property update**: `renovaTemp()` evaluates temperature-dependent fluid properties

### Time loop body

Each iteration advances the solution by one time step:

```
while (t < t_final):

    1. atualizaSonico()           — update sound speed per cell
    2. determinaDT(vExpli)        — compute Δt
    3. dtauxCFL = dt              — save CFL-limited Δt for diagnostics

    4. aberturaVal0/Val1          — valve opening/closing schedule
    5. BuscaPresInjDesc()         — descent control (if controDesc == 1)

    6. solveLinGas()              — advance gas-lift service line
    7. arq.atualiza(...)          — update time-dependent BCs from schedule
    8. atualizaCC1()              — set inlet conditions (P, T, tit, β)

    9. avaliaVariaDpDt(...)       — decide model complexity for this step

   10. COUPLING LOOP (kontaAcop = 0 to 1*modeloCompleto):

       a. Set m2d flags per cell (adaptive complexity control)
       b. EvoluiFrac(αrev, βrev, kontaAcop)      — advance α, β
       c. Near-wellbore advance (if porous BCs)
       d. restringeDTporValv()                    — valve Δt limits

       e. IF reinicia == -1:
            ReiniEvolFrac0()      — find min stable sub-dt
            dt /= 2              — halve time step
            ReiniEvolFrac()      — revert α, β, PIG to old state
            EvoluiFrac(...)      — retry with smaller Δt
            (repeat if still unstable)

       f. AtualizaPig()          — advance PIG position
       g. atenuaDtMax()          — smooth Δt limiting
       h. calcCCpres(...)        — outlet BC (choke model)
       i. renovaterm()           — update c₀, ud (drift-flux closure)
       j. SolveAcopPV(vExpli)    — solve for P, ṁ
       k. IF not last iteration:
            compute dpdt per cell
            FeiticoDoTempo2()    — rewind fractions for re-coupling
       l. IF last iteration:
            renova()             — finalize P, ṁ into cell state
       m. marchaEnergTrans()     — energy equation (T)

   11. salvaFonte()              — store source terms
   12. renovaTemp()              — T-dependent property update
   13. renovaalbetini()          — save α, β as initial for next step. For cells with PIGs, also calculates PIG average velocity and displacement over Δt, advancing PIG position within the cell and updating sub-cell fractions (alfPigE, alfPigD, betPigE, betPigD) to account for PIG-induced phase separation. The PIG model is simplified: it acts as a physical barrier preventing fluid passage, creating distinct α/β values upstream and downstream of the PIG interface within the same cell.
   14. renovaRGOdgYco2() / renovaFracMol2()  — advect scalars
   15. renovaMasEsp()            — density update

   16. Moving averages (P, v, α)
   17. Output: profiles, trends, logs, progress
   18. t += Δt
```

### The coupling loop (`kontaAcop`)

When `modeloCompleto == 1`, the coupling loop runs **twice** (`kontaAcop = 0, 1`). The first pass:

- Advances $\alpha, \beta$ and solves pressure-velocity
- Computes `dpdt` for each cell (pressure rate of change)
- Calls `FeiticoDoTempo2()` to **rewind** the fraction state back to the beginning of the step

The second pass:

- Re-advances $\alpha, \beta$ with the updated `dpdt` (which feeds back into the mass transfer terms)
- Solves pressure-velocity again
- Calls `renova()` to finalize

This two-pass scheme provides an **implicit-like coupling** between pressure changes and void fraction evolution, improving stability during rapid transients (e.g., blowdown, slug arrival at surface).

### Key globals and operational notes

- **`chaverede == 0`**: indicates **single-branch mode** (the branch is not part of a network). When `chaverede != 0`, the solver operates as part of a network managed by `Num4Main`.
- **`BuscaPresInjDesc()`**: if `controDesc == 1` (descent control enabled), the solver applies pressure descent limits at discharge — enforcing `presMaxDesc` (maximum descending injection pressure) and `presMinDesc` (minimum) thresholds. This prevents erratic injection pressure changes when the producer pressure drops rapidly, effectively limiting the injection rate ramp.
- **`arq.atualiza()`**: called at each time step, this method (in class `Leitura`) evaluates all user-defined time-dependent schedule events — separator pressure changes, Master1 choke opening, BCS frequency changes, surface choke opening profiles, injection pressure schedules, and any other temporal data defined in the JSON input.
- **`kSP`**: global time-step counter, incremented at each time step.
- **`lixo5`**: global wall-clock time variable, updated to the current simulation time $t$ at step completion.

When `modeloCompleto == 0` (quasi-steady periods), only one pass is executed, saving roughly half the computational cost.

---

<a id="evoluifrac--void-fraction-evolution"></a>
## EvoluiFrac — Void Fraction Evolution

`SProd::EvoluiFrac()` advances the void fraction $\alpha$ and complementary liquid fraction $\beta$ by one time step. It consists of four OpenMP-parallel loops:

### Loop 1 — Advance $\alpha$ (`avancalf`)

For each interior cell ($i < n_{\text{cel}}$), calls `celula[i].avancalf()` to advance $\alpha$ explicitly. The last cell ($i = n_{\text{cel}}$) uses boundary-dependent logic — either copies from the upstream neighbor or calls `avancalf()` depending on network topology and choke configuration.

### Cell-level `avancalf` (celula3.cpp)

The void fraction transport equation is:

$$
\alpha^{n+1}_i = \alpha^n_i + \Delta t \cdot \frac{F_{\alpha}}{1 + K_{\alpha}}
$$

where $F_{\alpha}$ (`term1Mass`) gathers the flux and source terms from the holdup evolution equation (Equation `eq:holdup_simp` in the theoretical reference). Crucially, it includes the black-oil interphase mass transfer terms:

- **Convective flux**: upwinded produced-liquid and complementary-liquid mass flow differences divided by their respective densities ($\rho_{lp}$ and $\rho_{lc}$)
- **Liquid injection sources**: $Q_l$ terms weighted by $(1-\beta)(1-F_w)R_s\gamma_g\rho_{air}^{std}/B_o$
- **Inter-phase transfer**: $\Gamma_{lp}/(A\rho_{lp})$ and $\Gamma_{cp}/(A\rho_{lc})$ — the full $R_s$, $B_o$, $F_w$ coupling from the black-oil model (see `eq:psi_blackoil` in the theoretical reference)
- **Source terms**: gas injection, liquid injection, condensate

The divisor $K_{\alpha}$ (`CoefDTR / (rpC * area)`) provides implicit stabilization.

**Full form note:** The simplified equation above omits the $R_s$, $B_o$, $F_w$ Taylor-expansion terms that the code actually includes. These terms couple void fraction evolution to pressure and temperature changes through the black-oil solution-gas ratio $R_s(P)$ and formation-volume factor $B_o(P,T)$, with $F_w$ controlling the water-cut partitioning of flashed gas. For the full derivation, see the theoretical reference (Equation `eq:holdup_ev_disc1`).

### Loop 2 — Advance $\beta$ (`avancbet`)

Same structure as $\alpha$, but for the complementary liquid fraction. The discrete transport equation corresponds to Equation `eq:beta_disc` in the theoretical reference. The solver uses the already-updated $\alpha^{n+1}$ (from Loop 1) and advances $\beta$ explicitly:

$$
\beta^{n+1}_i = \beta^n_i + \Delta t \left[ \frac{\Gamma_{cp}}{A(1-\alpha)\rho_{lc}\Delta L} - \frac{\beta}{(1-\alpha)}\frac{\alpha^{n+1}-\alpha^n}{\Delta t} - \frac{1}{A(1-\alpha)\rho_{lc}}\frac{\partial \dot{M}_c}{\partial x} \right]
$$

If $\alpha \approx 1$ (all-gas cell), $\beta$ is not updated.

### Loop 3 — Advance PIG fractions

If a PIG is present in cell $i$ (`estadoPig == 1`), calls `avancPig()` to advance the PIG position ratio within the cell, then `avancalfPig()` / `avancbetPig()` to split $\alpha, \beta$ into upstream/downstream sub-cell values (`alfPigE`, `alfPigD`, `betPigE`, `betPigD`).

### Loop 4 — Restart check

Serial loop that checks if any cell flagged `reiniciaAlf < 0`, `reiniciaBet < 0`, or `reiniciaPig < 0`. If so, sets the global `reinicia = -1`, triggering the time step restart logic in `SolveTrans`.

---

<a id="solveacoppv--pressure-velocity-coupling"></a>
## SolveAcopPV — Pressure-Velocity Coupling

`SProd::SolveAcopPV()` is the implicit core of the transient solver. It assembles and solves the linearized pressure-velocity system.

### Assembly: `Cel::GeraLocal`

For each cell $i$ (OpenMP-parallel), `GeraLocal()` (celula3.cpp) builds a $2 \times 6$ local matrix `local[2][6]` and right-hand side `TL[2]`.

**Row 0 — Pressure-velocity coupling equation.** This corresponds to Equation `eq:p_m_acop` in the theoretical reference. The six columns correspond to the stencil $[\dot{m}_{i-2},\; \dot{m}_{i-1},\; P_{i-1},\; \dot{m}_i,\; P_i,\; \dot{m}_{i+1}]$:

- **Pressure derivative terms**: gas compressibility $(\alpha / \rho_g) (\partial \rho_g / \partial P)^{-1}$ and liquid compressibility terms
- **Mass flow terms**: use $T_1$ / $T_2$ coefficients from the drift-flux decomposition, where $T_1$ is the fraction of mixture mass flow carried by the liquid phase and $T_2$ is the slip-dependent correction. The full pressure equation from the theoretical reference (Equation `eq:p_m_acop`) is discretized with these coefficients multiplying the mass flow terms
- **Inter-phase transfer**: `DTransDx*` and `DTransDt*` discretize $\partial \dot{m}_{\text{transfer}} / \partial x$ and $\partial \dot{m}_{\text{transfer}} / \partial t$
- **Sources**: gas injection (`fontemassG`), liquid injection (`fontemassL`), condensate (`fontemassC`), IPR inflow

The right-hand side `TL[0]` contains the known terms evaluated at the current state plus the temporal derivative contribution $A \Delta x (\ldots) P^n / \Delta t$.

**Row 1 — Momentum conservation (simplified).** The full momentum equation (Equation `eq:momentum_final` in the theoretical reference) is simplified in the staggered grid. The solver uses the **Fanning** friction factor $f_m$ with wetted perimeter $S_w$:

$$
\frac{\dot{m}_i^{n+1} - \dot{m}_i^n}{\Delta t} + \dots = -A \frac{\partial P}{\partial x} - f_m \frac{\rho_m |j| j S_w}{2} - \rho_m g A \sin\theta
$$

The pressure gradient is treated implicitly (columns carry $\pm A / \Delta x_{\text{med}}$). Friction and hydrostatic forces appear in `TL[1]`. Note: the momentum equation in transient mode is treated semi-implicitly (evolution of $\dot{m}$), which is less coupled than the mass equation.

### Global system assembly

The local $2 \times 6$ matrices are assembled into a **banded global matrix** `matglobP` with bandwidth ≈ 6. Each cell contributes 2 rows:
- Row $2i$: mass conservation
- Row $2i+1$: momentum conservation

### Solution

`matglobP.GaussElimPP(termolivreP)` solves the banded system using **Gauss elimination with partial pivoting** adapted for the banded structure. The solution vector `termolivreP` contains alternating $(\dot{m}_i, P_i)$ values.

The pressure-velocity coupling may run **multiple passes** controlled by `cicloAcopTerm` (default `0`, i.e., a single pass). When `cicloAcopTerm == 1`:

1. The first pass solves pressure-velocity as described above
2. The energy equation (`calctemp`) is then solved
3. If `cicloAcopTerm == 1`, the solver **rewinds** to the start of the step via `FeiticoDoTempo()`, restoring pressure and temperature
4. Pressure-velocity is re-solved with the updated temperature-dependent properties
5. This iterative loop continues until convergence

This thermal coupling is important because gas compressibility $\partial \rho_g / \partial T$ is omitted from the mass equation by default. With `cicloAcopTerm == 1`, the temperature feedback is captured iteratively, improving accuracy during strong thermal transients.

### Auxiliary pressure (`presaux`) — pressure BC at cell faces

The auxiliary pressure `presaux` is used to compute $T_1$ and $T_2$ at cell faces for the liquid mass split. The `renova()` method reconstructs it after solution. There are two options controlled by the user input `MedSimpPresFront`:

- **`MedSimpPresFront = 0` (accurate):** `presaux` includes explicit friction and hydrostatic corrections:
  $$P_{\text{aux}} = P_i + \frac{F_{\text{fric}} + F_{\text{grav}} - \Delta P_B}{98066.5}$$
  This is more precise but can make the coupled system less stable.

- **`MedSimpPresFront = 1` (simplified):** `presaux` is the arithmetic average of the left and right neighbor cell pressures. This is more numerically stable but less physically accurate.

The default is `true` (simplified averaging).

---

<a id="marchaenergtrans--transient-energy-equation"></a>
## marchaEnergTrans — Transient Energy Equation

`SProd::marchaEnergTrans()` advances the temperature field. The strategy is:

### Boundary fix

At the outlet cell ($i = n_{\text{cel}}$), if the liquid mass flow is negative (reverse flow), clamp it to zero to prevent unphysical energy transport.

### Interior cells — `calctemp()`

For each interior cell, `SProd::calctemp()` calls `celula[i].calctemp()`. The energy equation corresponds to the non-conservative form derived in [energy-balance.md](../theoretical-reference/energy-balance.md) (final form: Equation `eq:energy_final`). The equation is solved for temperature using the relationship between internal energy and enthalpy via $de/dh$ derivatives for real gas and both liquid phases.

The core relation is:

$$
\rho c_p \frac{\partial T}{\partial t} + \rho c_p u \frac{\partial T}{\partial x} = \frac{\partial P}{\partial t} \left[ T \left(\frac{\partial \rho}{\partial T}\right)_P - \rho \right] + \dots - \frac{\partial q_{\text{wall}}}{\partial A_{\text{wet}}}
$$

where $c_p$ is evaluated separately for gas (using real-gas correlations with compressibility factor $Z$) and each liquid phase (black-oil oil with dissolved gas, water). The $\Gamma_{cp}$ interphase transfer contributes an enthalpy source term proportional to the enthalpy difference between phases.

**Key implementation detail:** The real-gas energy equation uses iterative updates — for each cell, the gas density at the new time step requires solving $P = \rho Z(T, P) R T$ iteratively. The liquid densities use the equation of state for each liquid type (`propOil` for produced-liquid, `propWater` for complementary).

### 2D/3D heat diffusion

If enabled (`modoDifus3D`), cells with active 2D Poisson sub-grids solve the radial temperature distribution first (OpenMP-parallel), providing the wall heat flux that feeds back into the 1D energy balance.

---

## Boundary Conditions

### Inlet boundary (`atualizaCC1`)

The upstream boundary sets:
- **Pressure** $P_E$ (or mass flow rate, depending on `ConContEntrada` mode)
- **Temperature** $T_E$
- **Gas quality** $\text{tit}_E$ (gas mass fraction)
- **Complementary liquid fraction** $\beta_E$

These may be time-varying according to a schedule read from the JSON input (`arq.atualiza()`).

### Outlet boundary (`calcCCpres`)

`SProd::calcCCpres()` evaluates the downstream pressure condition. The actual code uses the Sachdeva choke model (`FonteMas::vazmassSachd` / `vazmaxSachd`) for critical/subcritical flow computation:

1. **Determine outlet mass flow** from the last cell face
2. **Call Sachdeva model**: computes critical flow through a choke/orifice using the pressure ratio $y = P_{\text{downstream}} / P_{\text{upstream}}$. If $y$ is below the critical pressure ratio, the flow is choked (maximum mass flow `vazmaxSachd`). Otherwise, use the subcritical formula `vazmassSachd`.
3. **Pressure correction**: The outlet pressure is adjusted so that the computed choke mass flow equals the actual mass flow from the last cell
4. **Joule-Thomson cooling**: The temperature drop across the choke is computed from the quality-weighted JT coefficients

### Choke quality ambiguity

A significant complication in transient choke modeling is defining the gas mass fraction (quality) of the stream entering the choke. The original approach used the cell-level void fraction and densities, but this proved unstable and inconsistent with steady-state results. The current implementation uses the left-face mass flow:

$$
\text{quality} = \frac{\dot{m}_{g,i}}{\dot{m}_{\text{total},i}}
$$

**Reverse flow detection:** When a multiphase reverse flow occurs at the last cell face (common during production shutdown), the quality definition becomes ambiguous. The solver detects phase direction reversal and applies a time-step penalty for safety. This prevents choke oscillations caused by incorrect quality estimates from a face not immediately adjacent to the choke.

**Choke-to-local-loss fallback:** When upstream mass flow is positive but the upstream/downstream pressure difference suggests reverse flow through the choke (common during shutdown as pressures equalize), the solver switches the boundary from a choke flow model to a localized pressure-loss model. This avoids choke oscillations near pressure-equalization points.

**Single liquid at choke:** When the quality approaches zero (pure liquid through the choke), the Sachdeva choke model becomes "blind" since it does not account for liquid compressibility. In this case, the solver uses the localized pressure-loss model instead.

---

## State Finalization (renova)

`SProd::renova()` extracts the solution from the global system and updates cell state:

1. **Pressure and mass flow**: for each interior cell:
   $$
   P_i = \texttt{termolivreP}[2i+1], \qquad \dot{m}_i = \texttt{termolivreP}[2i]
   $$
2. **NaN check**: aborts with `NumError` if any value is NaN
3. **Pressure rate**: $dP/dt = (P^{n+1} - P^n) / \Delta t$ (stored for the next step's coupling)
4. **Liquid split**: $\dot{m}_{l,i} = \text{term1}_i \cdot \dot{m}_i + \text{term2}_i$
5. **Propagate to faces**: left/right neighbor face values (`presL`, `presR`, `ML`, `MR`, etc.)
6. **Auxiliary pressure** `presaux`: a friction+hydrostatic-corrected pressure used for the momentum equation stencil:
   $$
   P_{\text{aux},i} = P_i + \frac{F_{\text{fric}} + F_{\text{grav}} - \Delta P_B}{98066.5}
   $$
   where friction is $\frac{1}{2} f_m \rho_m |j| j \cdot S_w \cdot \Delta x / A$ and gravity is $9.82 \sin\theta \, \rho_m \, \Delta x$.

---

<a id="closurelaw-update-renovaterm"></a>
## Closure-Law Update (renovaterm)

`SProd::renovaterm()` runs between `EvoluiFrac` and `SolveAcopPV` to refresh the drift-flux closure parameters:

1. **Save old values**: $C_0^n, u_d^n$ for relaxation
2. **Propagate fractions**: copy $\alpha_i, \beta_i$ to left/right faces of neighboring cells
3. **Detect closed accessories**: if a valve (tipo 5) or BCS pump (tipo 8) at the cell face is closed, skip that face
4. **Compute upwind velocities**: superficial gas velocity $u_{sg}$ and liquid velocity $u_{sl}$ using upwinded densities
5. **Call `CalcC0Ud()`**: evaluates the flow-pattern-dependent drift-flux parameters
6. **Derive `term1`, `term2`**: the linear mass split coefficients used by `GeraLocal`:
   $$
   \dot{m}_l = \text{term1} \cdot \dot{m} + \text{term2}
   $$
   where `term1` and `term2` encode the drift-flux relation.

**Single-phase detection:** The `renovaterm()` function also contains extensive branching logic to detect single-phase conditions:
- If $\alpha \approx 0$ (all-liquid), gas-related terms are zeroed and the momentum equation is simplified
- If $\alpha \approx 1$ (all-gas), liquid terms are zeroed
- PIG presence forces homogeneous flow ($C_0 = 1, u_d = 0$) regardless of flow pattern

**Single-phase heuristics:** Beyond the simple threshold checks, `renovaterm()` employs a logical heuristics sequence that forecasts whether the flow at each face will be single-phase or multiphase:
- If the flow is predicted to be all-liquid: `term1 = 1`, `term2 = 0` (face carries mixture entirely)
- If the flow is predicted to be all-gas: `term1 = 0`, `term2 = 0` (no liquid mass flow at face)
- If the flow is predicted to be multiphase: full $T_1$,$T_2$ calculation proceeds

**Additional single-phase sanity check:** After computing $T_1$,$T_2$ for a face, the solver performs a final validation. Using the newly computed $C_0$ and $u_d$, it estimates a superficial liquid velocity. If this estimate is positive while the left-face void fraction is $1$ (all-gas), the prediction was wrong — the actual flow is all-gas, and $T_1$,$T_2$ are reset. Symmetrically, a negative estimated liquid velocity followed by right-face $\alpha = 1$ also triggers a forced single-phase-gas assignment. This double-check prevents production-shutdown errors in the churn/annular transition region.

**Last-cell homogeneous override:** At the last cell ($i = n_{\text{cel}}$), oscillations in mass flow were observed when the flow pattern changed. These were attributed to $C_0$,$u_d$ sensitivity to pattern boundaries. A configuration option (`homogeneLast` or similar) forces homogeneous slip ($C_0 = 1, u_d = 0$) at the last cell only, which eliminates these numerical oscillations while preserving physically realistic slip in the rest of the pipeline.

**Accessory suppression:** When a cell contains an accessory (BCS, valve, etc.), all interphase transfer terms are zeroed, since the compact geometry of accessories does not support the distributed mass-transfer processes modeled in the regular cells.

---

## Mass Transfer Model

Interphase mass transfer between phases (evaporation, condensation, dissolution) is modeled through source terms that modify the mass fractions in each cell. The transfer terms affect the void fraction evolution (`EvoluiFrac`), the complementary liquid fraction evolution, and the mixture mass equation.

<a id="tmmodell--mass-transfer-model-selection"></a>
### TMModelL — Mass Transfer Model Selection

The `TMModelL` attribute on each cell controls the mass transfer behavior. This is a per-cell integer that defaults to 0 (complete model) but can vary across cells:

| Value | Mode | Behavior |
|-------|------|----------|
| 0 | **Complete implicit** (default) | Mass transfer terms are distributed into the implicit system via `DTransDxR`, `DTransDxL`, `DTransDtR`, `DTransDtL` coefficients. These Jacobian entries couple the transfer terms into the linear solver matrix, improving stability for stiff transfer rates. |
| 1 | **Fully explicit** | Transfer terms are applied directly as source terms via the standard Equation (166), without distributing into the implicit matrix. Simpler but potentially less stable for fast transfer rates. |
| 2 | **Simplified** | A simplified (lower-fidelity) transfer model with limited practical relevance. Uses reduced coefficients. |
| 3 | **No transfer** | All mass transfer is disabled — `FonteMudaFase` is zeroed and all `DTrans*Dx*D` coefficients are set to 0. |

**Accessory cells:** When a cell contains an accessory (BCS, valve, leak, etc.), `TMModelL` is automatically forced to mode 3 (no transfer). The compact geometry of accessories does not support the distributed mass-transfer processes modeled in regular pipeline cells.

<a id="critcond--phase-transition-cutoff"></a>
### CritCond — Phase Transition Cutoff

Near the monophase boundaries (α = 0 or α = 1), mass transfer terms can become unstable and cause infinite time-step oscillation loops. During evaporation or condensation, the transfer rate may be tiny but sufficient to push α slightly beyond the monophase threshold (e.g., α = 1 + 10⁻¹⁵), triggering a time-step halving that doesn't actually resolve the issue — the next step produces the same tiny overshoot.

To prevent this, `CritCond` defines a **cutoff band** near the monophase limits:
- If α > 1 − `CritCond`: mass transfer is **zeroed** (the cell is effectively all-gas)
- If α < `CritCond`: mass transfer is **zeroed** (the cell is effectively all-liquid)

`CritCond` is a global configurable parameter specified in the JSON input file. The default value is the same as `localtiny` (typically 10⁻⁹). This creates a "dead zone" near α = 0 and α = 1 where mass transfer is suppressed, preventing the solver from chasing infinitesimal corrections.

### Mass Transfer Attributes (per cell)

The following `Celula3` attributes store the computed mass transfer quantities:

| Attribute | Description |
|-----------|-------------|
| `FonteMudaFase` | Interphase mass transfer rate [kg/s] — the net mass transferred from one phase to another per unit time. This is the source term applied to the mass equations. |
| `DTransDxR` | Right-face mass transfer contribution to the momentum equation row (implicit Jacobian) |
| `DTransDxL` | Left-face mass transfer contribution to the momentum equation row (implicit Jacobian) |
| `DTransDtR` | Right-face mass transfer time-derivative coefficient |
| `DTransDtL` | Left-face mass transfer time-derivative coefficient |
| `TransmassL` / `TransmassR` | Left and right face total transfer rates used in the cell mass balance |
| `DTransDxRp` | Right-face transfer coefficient for pressure derivatives |
| `DTransDxLp` | Left-face transfer coefficient for pressure derivatives |
| `DTransDxLinear` | Linear (non-derivative) transfer contribution to the momentum equation |
| `coefTransBet` | Coefficient for complementary liquid fraction transfer |
| `coefDtR` / `coefDtL` | Time-derivative coefficients for right/left faces |

These attributes are computed by `renovaterm()` (for implicit models) and applied during `EvoluiFrac` and `GeraLocal` (the momentum equation assembly). The coefficients encode the sensitivity of the mass transfer to pressure and void-fracture changes at each cell face.

---

## Gas-Lift Service Line Coupling

`SProd::solveLinGas()` couples the gas-lift service line to the production line. The gas-lift line has its own 1D cell array `celulaG[]` of type `CelG` (defined in [`celulaGas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celulaGas.h) / [`celulaGas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celulaGas.cpp)), modelling **single-phase compressible gas** above the liquid interface and completion fluid below it.

### solveLinGas — Orchestration

The top-level coupling method performs three steps:

1. **`ValvGasTrans()`**: solves the transient gas-lift valve model. For each VGL:
   - If the valve cell is in the **gas zone** (above `celInter`): computes `presEstag` from the gas-cell pressure, `presGarg` from the production-column pressure with recovery fraction `frec`, then calls `chokeVGL[i].massica()` for the compressible gas choke mass flow (in [`chokegas.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/chokegas.cpp))
   - For **IPO valves** (`tipo == 1`): the effective orifice area is modulated by `areaValvCali()`, which models the calibrated area vs. differential pressure characteristic
   - If the valve cell is in the **liquid zone** (below `celInter`): uses `massica(1, salinidade)` for liquid-phase flow with completion-fluid properties
   - A **master-2 choke** at `posicM2` handles bidirectional flow between production and service lines, with hydrostatic correction

2. **Map gas-line cells to production cells**: for each valve position, transfers:
   - Injection gas flow rate `QGas` to the production cell's gas-source accessory (`celula[posGLP].acsr.injg`)
   - Injection temperature computed by `TempDescGL()` — isenthalpic expansion from stagnation to throat using real-gas thermodynamics (stepwise pressure march in 50 kgf/cm² increments with the formula $T_1 = 1/(1/T_0 - (286.998/\rho_g) \cdot (\partial Z/\partial T)/c_{p,g} \cdot \ln(P_1/P_0))$, falling back to Joule-Thomson for small steps)
   - Stagnation/throat conditions (`pEstag`, `tEstag`, `pGarg`, `tGarg`) are stored on the gas cell for trending

3. **`subtempoGas()`**: advances the gas-line transient solver for one production-line time step, potentially using **sub-timestepping** (multiple smaller steps).

### subtempoGas — Gas-Line Transient Solver

The gas-line solver is structurally similar to the production-line solver but operates on `CelG` cells with 3 DOFs per cell (pressure, mass flow, temperature):

```
subtempoGas():
  1. Save state          — DeVoltaParaoFuturo() for each gas cell
  2. Interface advance   — track gas/liquid boundary (if discharge model)
  3. Coupling loop (ciclomax iterations):
     a. Compute injection choke source (if tipoCC == 0)
     b. CelG::GeraLocal() for each cell (OMP-parallel)
        → assemble 3×9 local matrix into banded global system matglobG
     c. Gauss elimination: matglobG.GaussElimPP(termolivreG)
     d. renovaGas() — scatter solution to cell P, VGasR
     e. calctempGas() — temperature march for gas cells
     f. FeiticoDoTempo() — rewind if more coupling iterations remain
  4. Update densities: rg, u1L, u1R, u1LL for all cells
  5. resolveDescarga() — if liquid-discharge model is active
```

### CelG::GeraLocal — Gas-Line Matrix Assembly

Each `CelG` cell assembles a $3 \times 9$ local matrix and 3-element RHS vector (`celulaGas.cpp`):

| Row | Equation | Physics |
|-----|----------|---------|
| 0 | **Continuity** | $\partial(\rho A)/\partial t + \partial \dot{m}/\partial x = \text{source}$ — uses gas compressibility $\partial\rho/\partial P$, temperature derivative $\partial\rho/\partial T$, VGL extraction `massfonteCH`, master-2 source `fonteM2` |
| 1 | **Momentum** | $\partial \dot{m}/\partial t + v \cdot \partial \dot{m}/\partial x + A \cdot \partial P/\partial x + F_{\text{fric}} + F_{\text{grav}} = 0$ — Fanning friction via `CelG::fric()` (Haaland + Colebrook), hydrostatic via $\sin\theta$ |
| 2 | **Temperature** | Identity equation $T = T_{\text{current}}$ — temperature is solved separately by `calctempGas()` after the pressure-velocity solve |

The 9-column stencil spans 3 cells (left, center, right) with 3 DOFs each. Boundary conditions: cell 0 applies either pressure or flow BC; the last cell applies zero-flow. Cells below the liquid interface (`celInter`) receive identity equations (frozen state).

### CelG State Management

- `CelG::DeVoltaParaoFuturo()` — saves current state into `*ini` copies (commits the time level)
- `CelG::FeiticoDoTempo()` — restores all state from `*ini` copies (rolls back to the start of the step)
- `renovaGas()` — scatters the global solution vector `termolivreG[]` back into each cell's pressure and mass flow

**Face convention:** The gas mass flow for each `CelG` cell is stored as `VGasR` at the **right face** of the cell. This consistent right-face convention matches the production-line mass flow convention (`MR` stores to the right face of cell $i$, while `MR` of cell $i-1$ also refers to the same boundary between cells $i-1$ and $i$). The mass fonte (VGL injection source) is also stored as `massfonteCH` on each gas-line cell for trending purposes.

---

## PIG Tracking

`SProd::AtualizaPig()` tracks pipeline inspection gauge (PIG) movement:

### PIG position advance (`avancPig`)

Each cell has a `razPig` ∈ [0, 1] representing where the PIG is within the cell:

$$
\text{razPig}^{n+1} = \text{razPig}^n + \frac{v_{\text{PIG}} \cdot \Delta t}{\Delta x}
$$

If $\text{razPig} \geq 1$: PIG moves to the next cell downstream.
If $\text{razPig} \leq 0$: PIG moves upstream (reverse flow).

Out-of-bounds triggers a time step restart, same as for $\alpha$.

### Cell-to-cell transfer

When the PIG crosses a cell boundary:
- The `estadoPig` flag is transferred from cell $i$ to cell $i \pm 1$
- The PIG velocity and index are propagated
- Upstream/downstream sub-cell fractions (`alfPigE`, `alfPigD`, `betPigE`, `betPigD`) are updated

### Complementary liquid fraction interface

After PIG update, the upwinded complementary liquid fraction at each face (`betI`) is set accounting for the PIG-partitioned sub-cell values, ensuring that the liquid composition on each side of the PIG is tracked correctly.

---

## Adaptive Model Complexity

`SProd::avaliaVariaDpDt()` dynamically adjusts the solver complexity:

### Algorithm

1. Scan all cells in blocks of 10
2. For each block, compute the average pressure rate:
   $$
   \overline{|dP/dt|} = \frac{1}{N_{\text{block}}} \sum_{j \in \text{block}} \frac{|P_j^n - P_j^{n-1}|}{\Delta t}
   $$
3. If any block exceeds $2 \times \text{taxaDespre}$ (the user-defined depressurization rate threshold), set `modeloCompleto = 1`
4. Maintain a rolling window (last 10 steps) of the maximum rates `taxaDpMax` and `taxaDTMax` for smoothed decision-making

### Effect on solver

| `modeloCompleto` | Coupling iterations | Cost |
|---|---|---|
| 0 | 1 pass | ~50% cheaper |
| 1 | 2 passes (with rewind) | Full accuracy |

During quasi-steady periods (e.g., stable production), the solver automatically drops to single-pass mode. During transient events (valve closure, slug arrival, blowdown), it switches to two-pass coupled mode.

Additionally, `modeloCompleto` controls per-cell flags (`m2d`, `mudaDT`, `ativaDeri`) that enable/disable:
- Pressure-dependent inter-phase mass transfer corrections
- Gas compressibility derivatives in the mass equation
- Liquid compressibility terms

---

## Time Step Restart Logic

When the explicit advance produces out-of-bounds fractions, the solver must retry with a smaller time step. The restart mechanism involves three methods:

### `ReiniEvolFrac0()` — Find minimum stable sub-step

Scans all cells and finds the minimum of `dt`, `dt1`, `dt2`, `dtPig` — these are the per-cell maximum stable sub-steps computed during `avancalf`, `avancbet`, and `avancPig`.

### `ReiniEvolFrac()` — Revert state

1. Sets all cells to the new (smaller) global $\Delta t$
2. Reverts gas-line cells via `FeiticoDoTempo()` (cell-level time-rewind)
3. Calls `SubReiniEvolFrac()` to restore $\alpha$ to $\alpha^n$ (initial)
4. Restores $\beta$, `razPig`, and PIG sub-cell fractions to their initial values

### `FeiticoDoTempo` / `FeiticoDoTempo2` (celula3.cpp)

These are the **cell-level rewind** methods:

- `FeiticoDoTempo()`: restores pressure, mass flow, temperature, flow rates, and heat-transfer state to their values at the start of the time step
- `FeiticoDoTempo2()`: restores fractions ($\alpha$, $\beta$), PIG state, drift-flux parameters ($C_0$, $u_d$), and flow pattern transition counters

The naming comes from the Portuguese *feitiço do tempo* ("time spell" — literally rewinding time).

### Restart sequence in SolveTrans

```
IF reinicia == -1:
    ReiniEvolFrac0()          // find min stable dt
    dt = min(dt, dt/2)        // halve global dt
    ReiniEvolFrac()           // revert α, β, PIG to old values
    EvoluiFrac(...)           // re-advance with smaller dt
    IF still reinicia == -1:
        repeat halving...
```

---

## Compositional and Scalar Transport

### Scalar advection (`renovaRGOdgYco2`)

`SProd::renovaRGOdgYco2()` advects fluid property scalars along the pipeline using the mass-flow-weighted transport:

- **RGO** (gas-oil ratio)
- **dg** (gas specific gravity)
- **yCO₂** (CO₂ mole fraction)
- **API** (oil gravity)
- **BSW** (basic sediment & water)
- **Liquid and water viscosity**

Each scalar is transported using upwind differencing based on the cell-face mass flow direction.

### Compositional updates (`renovaFracMol2`)

When `flashCompleto == 2` (compositional mode), `renovaFracMol2()` updates the mole fractions of each component in the oil and gas phases. The compositional model uses a **mini-table** (`miniTab`) approach: a local interpolation table is built around the current (P, T) state to avoid calling the full flash calculation at every cell and time step.

### Density update (`renovaMasEsp`)

`SProd::renovaMasEsp()` re-evaluates gas, oil, and water densities at the updated (P, T) state for each cell, ensuring consistency between the thermodynamic state and the flow equations for the next time step.

---

## Output and Post-Processing

The transient solver produces several types of output at user-specified intervals:

| Output type | Method | Content |
|---|---|---|
| **Spatial profiles** | `imprimeProfile()` | P, T, α, β, velocities along the pipeline |
| **Production trends** | `imprimeTrend()` | Time series of flow rates, pressures, temperatures at key points |
| **Gas trends** | `imprimeTrendGas()` | Gas-lift line pressures and injection rates |
| **Thermal layer trends** | `imprimeTrendTermico()` | Pipe wall and insulation temperatures |
| **Log events** | `log.registra()` | Valve events, PIG passage, alarms |
| **Snapshots** | `salvaArquivoSnap()` | Full state dump for restart |
| **Progress** | Console output | Time, dt, CFL fraction, iteration count |
| **Radial thermal profiles** | `imprimeProfileRadial()` | Temperature distribution across the pipe cross-section |
| **Thermal profiles (spatial)** | `imprimeProfileTrans()` / `imprimeProfileTransG()` | Pipe wall temperature profiles along the pipeline — production line and gas-lift line respectively |
| **Wall thermal trends** | `imprimeTrendTransP()` / `imprimeTrendTransG()` | Time series of pipe wall temperatures at key radial positions — production line and gas-lift line respectively |

Moving averages of pressure, velocity, and void fraction are maintained for trend output smoothing, using a configurable window.

---

## Summary of Key Methods

| Method | File | Purpose |
|---|---|---|
| `SolveTrans()` | SisProd.cpp | Main transient time-loop orchestrator |
| `determinaDT()` | SisProd.cpp | Time step calculation (CFL + constraints) |
| `determinaDTExpli()` | SisProd.cpp | CFL-limited Δt from sound speed + advection |
| `EvoluiFrac()` | SisProd.cpp | Explicit advance of α, β |
| `avancalf()` | celula3.cpp | Cell-level void fraction transport |
| `avancbet()` | celula3.cpp | Cell-level complementary liquid fraction transport |
| `avancPig()` | celula3.cpp | Cell-level PIG position advance |
| `SolveAcopPV()` | SisProd.cpp | Pressure-velocity coupling (banded system) |
| `GeraLocal()` | celula3.cpp | Local 2×6 matrix assembly (pressure-velocity + momentum) |
| `marchaEnergTrans()` | SisProd.cpp | Transient energy equation |
| `calctemp()` | SisProd.cpp | Per-cell temperature advance |
| `renova()` | SisProd.cpp | Extract solution, update cell state |
| `renovaterm()` | SisProd.cpp | Update drift-flux closure (c₀, ud, term1, term2) |
| `CalcC0Ud()` | SisProd.cpp | Flow-pattern-dependent c₀, ud |
| `calcCCpres()` | SisProd.cpp | Outlet Sachdeva choke boundary condition |
| `solveLinGas()` | SisProd.cpp | Gas-lift service line coupling |
| `AtualizaPig()` | SisProd.cpp | PIG tracking and cell transfer |
| `avaliaVariaDpDt()` | SisProd.cpp | Adaptive model complexity |
| `ReiniEvolFrac0()` | SisProd.cpp | Find min stable sub-dt |
| `ReiniEvolFrac()` | SisProd.cpp | Revert state for dt restart |
| `FeiticoDoTempo()` | celula3.cpp | Cell-level state rewind (P, ṁ, T) |
| `FeiticoDoTempo2()` | celula3.cpp | Cell-level state rewind (α, β, PIG, c₀) |
| `renovaRGOdgYco2()` | SisProd.cpp | Scalar transport (RGO, API, BSW, CO₂) |
| `renovaMasEsp()` | SisProd.cpp | Density re-evaluation |
| `renovaTemp()` | SisProd.cpp | T-dependent property refresh |
| `DTransDx()` / `DTransDt()` | SisProd.cpp | Inter-phase mass transfer derivatives |
| `ValvGasTrans()` | SisProd.cpp | Transient gas-lift valve model (choke flow + IPO) |
| `subtempoGas()` | SisProd.cpp | Gas-line transient sub-timestepping |
| `renovaGas()` | SisProd.cpp | Scatter gas-line solution to CelG cells |
| `TempDescGL()` | SisProd.cpp | Isenthalpic gas temperature drop across VGL |
| `CelG::GeraLocal()` | celulaGas.cpp | Gas-line local 3×9 matrix assembly (continuity + momentum + T) |
| `CelG::FeiticoDoTempo()` | celulaGas.cpp | Gas-line cell-level state rewind |
| `CelG::DeVoltaParaoFuturo()` | celulaGas.cpp | Gas-line cell-level state commit |
| `chokeVGL[].massica()` | chokegas.cpp | Compressible gas choke mass-flow model |
| `renovaalbetini()` | SisProd.cpp | Save α,β initial; calculate PIG velocity/displacement within cell |
| `areaValvCali()` | SisProd.cpp | IPO valve calibration — calibrated area vs. differential pressure |
| `buscaPresInjDesc()` | SisProd.cpp | Descent control — enforces presMaxDesc/presMinDesc thresholds |
| `imprimeProfileTrans()` | SisProd.cpp | Pipe wall temperature profiles along pipeline (production line) |
| `imprimeTrendTransP()` | SisProd.cpp | Time series of pipe wall temperatures (production line) |
| `imprimeProfileTransG()` | SisProd.cpp | Pipe wall temperature profiles along gas-lift line |
| `imprimeTrendTransG()` | SisProd.cpp | Time series of pipe wall temperatures (gas-lift line) |