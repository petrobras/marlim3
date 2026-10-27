# Network Simulation

This document describes how the Marlim3 simulator solves **flow networks** — multiple pipeline segments (*branches*) connected at junction nodes with mass and energy exchange. It covers the network JSON parsing, topology pre-processing, graph decomposition, steady-state convergence, transient time-stepping, and the four supported network topologies.

**Key source files:**

| File | Role |
|------|------|
| [`src/LerRede.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerRede.h) / [`LerRede.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LerRede.cpp) | `Rede` class — reads the network JSON, builds the connectivity graph |
| [`src/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp) | All network solver functions: `preProcRede()`, `cicloRede()`, `SolveRedeTrans()`, `RedeProd()`, `RedeAnelGL()`, `RedeParalela()`, `RedeInj()` |
| [`src/SisProd.h`](https://github.com/petrobras/marlim3/blob/main/src/include/SisProd.h) / [`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) | `SProd` class — per-branch solver: `buscaProdPfundoPerm()`, `hidroreverso()`, `permanenteSimples()`, `SolveTrans()`, etc. |
| [`src/Leitura.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Leitura.h) / [`Leitura.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Leitura.cpp) | `Ler` class — reads individual branch JSON files |
| [`src/celula3.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celula3.h) / [`celula3.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celula3.cpp) | `Cel` class — per-cell state |
| [`src/PropFlu.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFlu.h) / [`PropFlu.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFlu.cpp) | `ProFlu` — fluid property models |

---

## Table of Contents

1. [Overview](#overview)
2. [Network Types](#network-types)
3. [Network JSON and the Rede Class](#network-json-and-the-rede-class)
4. [Data Structures](#data-structures)
5. [Pre-Processing Pipeline](#pre-processing-pipeline)
6. [Building SProd Objects — preparaRedeProd](#building-sprod-objects--prepararedeprod)
7. [Zero-Flow branch Removal — avaliaPerm](#zero-flow-branch-removal--avaliaperm)
8. [Steady-State Network Solver](#steady-state-network-solver)
9. [Node-Level Fluid Mixing — totalizaCicloRede](#node-level-fluid-mixing--totalizaciclorede)
10. [Convergence and Relaxation](#convergence-and-relaxation)
11. [Branch-Level Steady-State Dispatch](#branch-level-steady-state-dispatch)
12. [Initial Pressure Guess — chutePresRede](#initial-pressure-guess--chutepresrede)
13. [Transient Network Solver](#transient-network-solver)
14. [Transient Helper Functions](#transient-helper-functions)
15. [Production Network — solveRedeProd / RedeProd](#production-network--solveredeprod--redeprod)
16. [Gas-Lift Loop — RedeAnelGL](#gas-lift-loop--redeanelgl)
17. [Parallel Network — RedeParalela](#parallel-network--redeparalela)
18. [Injection Network — RedeInj](#injection-network--redeInj)
19. [Compositional Model Switching](#compositional-model-switching)
20. [Compositional Network Cycle — cicloRedeComp](#compositional-network-cycle--cicloredecomp)
21. [Blind Compositional Cycle — cicloRedeCompCego](#blind-compositional-cycle--cicloredecompcego)
22. [Cell-Level Composition Propagation](#cell-level-composition-propagation)
23. [Snapshot and Output](#snapshot-and-output)
24. [Summary of Key Functions](#summary-of-key-functions)

---

## Overview

In network mode (`-s REDE`), the simulator reads a **network JSON** file that references multiple branch JSON files and describes their connectivity. The top-level execution flow is:

```
main()  (tipoSimulacao == REDE)
  │
  ├─ Rede arqRede(networkJson)           ← parse network JSON
  ├─ Determine network sub-type (tipoRede)
  │
  ├─ tipoRede == 0 (production) or tipoRede == 1 (injection):
  │    ├─ descarteTramo(arqRede)          ← remove inactive branches
  │    ├─ preProcRede(arqRede)            ← decompose into sub-networks
  │    ├─ For each sub-network (parallel via OpenMP):
  │    │    ├─ preparaRedeProd()           ← read branch JSONs → SProd[]
  │    │    ├─ solveRedeProd()             ← steady-state + transient
  │    │    └─ Write reports + timing
  │    └─ (or RedeInj() for injection networks)
  │
  ├─ tipoRede == 2 (gas-lift loop):
  │    └─ RedeAnelGL()
  │
  └─ tipoRede == 3 (parallel):
       └─ RedeParalela()
```

Each branch is an independent `SProd` object. The network solver iterates over the graph, solving each branch individually, then passing boundary conditions (pressure, temperature, fluid composition) between connected branches at shared nodes until global convergence.

---

## Network Types

The `tipoRede` field determines the network topology and solver variant:

| `tipoRede` | JSON flag | Name | Description |
|------------|-----------|------|-------------|
| 0 | (default) | Production | Standard gathering network — wells feed into collectors and manifolds with arbitrary branching |
| 1 | `"Injecao": true` | Injection | Injection network — fluid distributed from surface to wells |
| 2 | `"AnelGL": true` | Gas-lift loop | Annular gas distribution line feeding multiple production wells |
| 3 | `"fonteRedeParalela": true` | Parallel | Two co-located pipelines coupled at shared source points |

Priority: if `AnelGL` is set, `Injecao` is forced off.

---

## Network JSON and the Rede Class

The `Rede` class ([`LerRede.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerRede.h) / [`LerRede.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LerRede.cpp)) reads the network JSON file. Its constructor calls `lerArq()`, which parses the file via RapidJSON and dispatches four methods:

```
Rede::lerArq()
  │
  ├─ parseEntrada()                       → RapidJSON Document
  ├─ (optional) writeSchemaRede()          → generate JSON Schema
  ├─ (optional) validateVsSchema()         → validate input
  │
  ├─ parse_configuracao_inicial(...)       → solver parameters
  ├─ parse_arquivos(...)                   → branch file list
  ├─ parse_conexao(...)                    → connectivity graph
  └─ parse_fonteReciproca(...)             → parallel couplings (tipoRede==3 only)
```

### `parse_configuracao_inicial()`

Reads `"configuracaoInicial"` and sets solver parameters with defaults:

| Parameter | JSON key | Default | Meaning |
|-----------|----------|---------|---------|
| `chutHol` | `"ParametroInicial"` | (required) | Initial holdup guess for hydrostatic estimates |
| `relax` | `"Relaxacao"` | 0.5 | Under-relaxation factor for node pressure updates |
| `limConverge` | `"limiteConvergencia"` | 0.001 | Convergence tolerance for network iterations |
| `chaveredeT` | `"Transiente"` | 0 | Enable transient after steady-state |
| `TmaxR` | `"TempoSimulacao"` | 0 | Maximum transient simulation time (s) |
| `nthrRede` | `"threadRede"` | 1 | Number of OpenMP threads for network solve |
| `tipoModeloDrift` | `"tipoModeloDrift"` | 1 | Drift-flux model variant |
| `fluidoRede` | `"fluidoRede"` | 1 | Fluid type: 1 = liquid-dominated, 0 = gas-dominated |
| `chute` | `"ChuteNos"` | 0 | Use user-supplied initial node pressures |
| `apenasPreProc` | `"apenasPreProc"` | 0 | Stop after pre-processing (dry run) |
| `tabelaDinamica` | `"modeloTabelaDinamica"` | 0 | Dynamic PVT table interpolation |

### `parse_arquivos()`

Reads `"Arquivos"` — a JSON array of branch file names (strings):

```json
"Arquivos": ["tramo-poco1.json", "tramo-riser.json", "tramo-flowline.json"]
```

Sets `nsisprod` = array size and populates `impfiles[]`.

### `parse_conexao()`

Reads `"Conexao"` — a JSON array with one entry per branch defining its topology:

```json
"Conexao": [
  {
    "permanente": 1,
    "ativo": 1,
    "coletores": [1, 2],
    "afluentes": [],
    "PressaoImposta": false,
    "reverso": 0,
    "derivaPrincipal": 0,
    "bloqueio": -1
  },
  ...
]
```

For each branch, the method populates a `conexao` struct (see [Data Structures](#data-structures)). Collector and affluent indices are range-checked against `[0, nsisprod]`. Blockage requires ≤ 2 collectors. For parallel networks (`tipoRede == 3`), only 2 connections are parsed and one must be marked `tramoPrimario`.

### `parse_fonteReciproca()` (parallel networks only)

Reads `"fonteRedeParalela"` — pairs of coupled cell positions between the primary and secondary branches:

```json
"fonteRedeParalela": [
  { "idNoPrimario": 5, "idNoSecundario": 10 }
]
```

---

## Data Structures

### `conexao` — Per-branch connectivity (in [`LerRede.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerRede.h))

| Field | Type | Meaning |
|-------|------|---------|
| `perm` | `int` | 1 = permanent connection, 0 = deactivated during solve |
| `ativo` | `int` | 1 = active branch, 0 = inactive (ignored) |
| `ncoleta` | `int` | Count of downstream collectors |
| `nafluente` | `int` | Count of upstream tributaries |
| `coleta` | `int*` | Array of downstream branch indices |
| `afluente` | `int*` | Array of upstream branch indices |
| `presimposta` | `int` | 1 = pressure is imposed at this branch's junction |
| `bloqueio` | `int` | Index of blocked collector (−1 = none) |
| `reverso` | `int` | Reverse-flow flag |
| `derivaPrincipal` | `int` | Branch derives from main trunk |
| `tramoPrimario` | `int` | Primary branch flag (parallel network) |
| `presMon` / `presJus` | `double` | User-supplied upstream/downstream pressure guesses |
| `tipoanel` | `int` | Ring sub-type (gas-lift loop only) |
| `compfonte` | `double` | Ring segment length (gas-lift loop only) |

### `tramoAtivo` — Working copy during pre-processing (in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

```cpp
struct tramoAtivo {
    int ncoleta, nafluente, nbloqueio;
    int ativo, bloqueio, permanente, presimposta;
    double presMon, presJus;
    int reverso, derivaPrincipal;
    vector<int> coleta, afluente;
    string tramoJson;           // path to the branch's JSON input file
};
```

### `noRede` — Node convergence tracking (in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

```cpp
struct noRede {
    int naflu, ncole;
    int aflu[40], cole[40];      // upstream/downstream branch indices
    double normaP1, normaP0;    // pressure norm: current / previous iteration
    double normaMass1, normaMass0;
    int cadastrado;
};
```

### `convergeNoPerm` — Mixed fluid at a node (in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

Accumulates flow-weighted averages from all branches feeding into a junction:

| Field | Meaning |
|-------|---------|
| `qostdTot` / `qgstdTot` | Total oil/gas rate at standard conditions |
| `apimist` | Mixed API gravity |
| `dengmist` | Mixed gas density |
| `bswmist` | Mixed BSW |
| `tempmist` | Mixed temperature (enthalpy-weighted) |
| `RGOmist` | Mixed gas-oil ratio |
| `mliqmist` / `mgasmist` | Mixed liquid/gas mass flow rates |
| `cpmist` | Mixed heat capacity |
| `flu` | `ProFlu` object with the resulting mixed properties |

### `fonteposic` — Mass source position in network (in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

```cpp
struct fonteposic {
    int nmani;           // number of manifolds connected
    int mani[10];        // manifold indices
    double posicmani;    // position along manifold
};
```

---

## Pre-Processing Pipeline

Before the network can be solved, the topology is cleaned and decomposed:

```
descarteTramo(arqRede)
  │  Remove inactive branches; prune connectivity
  ▼
preProcRede(arqRede)
  │  Discover connected components (DFS)
  │  Re-index local connections
  │  Write RedeInterna-{n}.json per sub-network
  ▼
redeLeitura = number of active sub-networks
```

### `descarteTramo()`

Scans all branches and removes those with `ativo == 0`:

1. For each inactive branch, clears its `nafluente`, `ncoleta`, `perm`, and `presimposta`
2. For each active branch, removes references to inactive neighbours from its `coleta[]` and `afluente[]` arrays
3. Clears `bloqueio` references to inactive branches

### `preProcRede()`

Decomposes the full network graph into independent **connected sub-networks**:

1. Copies `arqRede.malha[]` into the global `tramos[]` vector
2. Runs a **depth-first search** via `verficaConex()` starting from unvisited branches:
   - `verficaConex(i)` recursively visits all branches reachable through `afluente[]` and `coleta[]` links
   - Each connected component becomes one sub-network
3. Re-indexes connections from global to local sub-network indices using a `transporte[]` mapping array
4. Writes each sub-network as `RedeInterna-{n}.json` via `gravaRedeInterna()`
5. Sets `redeLeitura` = number of active sub-networks

The helper functions:

- **`match(i, cadastrados, ncadastro)`** — linear search: returns 1 if `i` is in the visited set
- **`verficaConex(i, ...)`** — recursive DFS: if branch `i` is not visited, push it into the current component, then recurse into all its `afluente[]` and `coleta[]`

### `gravaRedeInterna()`

Serializes a sub-network to a JSON file for diagnostics. The output includes the solver parameters (`Relaxacao`, `limiteConvergencia`, `TempoSimulacao`), the `Arquivos` array (branch file names), and the `Conexao` array with re-indexed local topology.

---

<a id="building-sprod-objects--prepararedeprod"></a>
## Building SProd Objects — preparaRedeProd

`preparaRedeProd()` reads the branch JSON files for a sub-network and constructs one `SProd` object per branch:

```
preparaRedeProd(malha[], arqRede, ...)
  │
  ├─ For each branch i:
  │    ├─ SProd temporario(pathArqEntrada + arqRede.impfiles[i], ...)
  │    ├─ malha[i] = temporario               ← full branch parse + mesh build
  │    ├─ Set noextremo = (ncoleta == 0)       ← terminal node?
  │    ├─ Set noinicial = (nafluente == 0)     ← source node?
  │    └─ If ConContEntrada == 1:
  │         ├─ Create injection accessory at cell[0]
  │         └─ Estimate initial flow rate from area ratio
  │
  ├─ Distribute initial flow: Q_i = Q_total × A_i / Σ A_j
  │
  └─ Store reverse-flow fluid: fluiRevRede = collector's cell[0] fluid
```

For branches with pressure BCs at the inlet (`ConContEntrada == 1`), an injection accessory (`injl` for liquid-dominated networks, `injg` for gas-dominated) is created at cell[0] with estimated RGO, BSW, API, and viscosities from the inlet conditions.

The initial flow rate for each branch is proportional to its pipe cross-sectional area:

$$Q_i = Q_{\text{total}} \cdot \frac{A_i}{\sum_j A_j}$$

---

<a id="zero-flow-branch-removal--avaliaperm"></a>
## Zero-Flow Branch Removal — avaliaPerm

`avaliaPerm()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) performs a multi-pass scan to identify and deactivate branches that carry no flow. It is called after `preparaRedeProd()` but before the steady-state solve.

### Algorithm

The function iterates `while(totPerm < narq)` — i.e., until every branch has been evaluated:

**Pass 1 — Leaf branches** (`nafluente == 0`, source nodes):

1. Check whether the inlet source produces any flow:
   - Gas injection (`tipo == 1`): `QGas ≈ 0`
   - Liquid injection (`tipo == 2`): `QLiq ≈ 0`
   - Mass injection (`tipo == 10`): `MassP + MassG + MassC ≈ 0`
   - IPR (`tipo == 3`), porous media (`tipo == 15`, `tipo == 16`), and `ConContEntrada == 2` branches: **never deactivated** — always kept active regardless of flow index
2. If zero (types 1, 2, 10 only) → `perm = 0`, increment `semPerm` counter

**Pass 2 — Non-leaf branches** (`nafluente > 0`):

Wait until all afluentes have been resolved, then:

1. If all afluentes have `perm == 0` and the branch itself has no local source → `perm = 0`
2. **Dual-source swap**: if a source exists at cell[1] but not at cell[0], the code swaps the accessory from cell[1] to cell[0] (handles edge case of reversed dual sources via `verificaFonteDuplaReversa()`)
3. Special handling for `ConContEntrada == 2` (pressure + flow-rate BC): checks whether the imposed flow rate is zero

**Pass 3 — Synthetic source injection**:

For branches where `nafluente > 0`, `perm == 1`, but all afluentes have `perm == 0` while multiple collectors are active at the same node: creates a synthetic source on the afluent branch to ensure mass balance feasibility.

### Result

After `avaliaPerm()`, branches with `perm == 0` are excluded from the steady-state network solve. The `semPerm` output count is used by `solveRedeProd()` to skip convergence if all branches are inactive.

---

## Steady-State Network Solver

The steady-state solver uses an **outer fixed-point iteration** with **inner branch-level solves**:

```
convergeRede(malha[], arqRede, ...)
  │
  │  norma = 1000, iterRede = 0
  │  while norma > limConverge AND iterRede < 200:
  │    │
  │    │  if stalling: relax *= 0.5 (min 0.1)
  │    │
  │    │  norma = cicloRede(malha[], arqRede, ...)
  │    │  iterRede++
  │    │
  │  end while
```

### `cicloRede()` — One Network Iteration

The core function. Traverses the network graph in **topological order** (leaves first, then dependents) and returns the convergence norm.

```
cicloRede(malha[], arqRede, ...)
  │
  ├─ Phase 1 — Leaf branches (nafluente == 0):
  │    ├─ Mark non-permanent branches as resolved
  │    └─ Solve each leaf branch (OpenMP parallel)
  │
  ├─ Phase 2 — Dependent branches:
  │    │  repeat until all resolved:
  │    │    for each unresolved branch i with nafluente > 0:
  │    │      if all afluentes resolved:
  │    │        ├─ Identify master collector (largest diameter)
  │    │        ├─ totalizaCicloRede(...)     ← mix fluids at junction
  │    │        ├─ Set BCs on master collector (fluid, flow rate)
  │    │        ├─ Set BCs on secondary collectors (pressure from master)
  │    │        ├─ Solve branch i
  │    │        └─ Propagate pressure upstream with relaxation:
  │    │             pGSup_aflu = ω · P_collector + (1−ω) · pGSup_aflu_old
  │    │
  ├─ Compute convergence norm:
  │    norma = sqrt( Σ_nodes (P_new − P_old)² ) / N_nodes
  │
  └─ return norma
```

**Master collector selection**: among all downstream collectors of a junction, the one with the largest pipe diameter is the master. A `derivaPrincipal` or `principal` flag can override this.

**Failure handling**: if a branch solve returns $|v| > 10^9$, it is deactivated (`inativo[i] = 1`), removed from neighbour lists, and the iteration restarts (`restartRede = 1`).

---

<a id="node-level-fluid-mixing--totalizaciclorede"></a>
## Node-Level Fluid Mixing — totalizaCicloRede

`totalizaCicloRede()` computes mixed fluid properties at a junction node from all upstream contributions. It uses **mass- and energy-weighted averaging**:

For each afluent branch $k$ with positive forward flow:

1. Stock-tank oil rate:

$$q_{o,ST,k} = \frac{\dot{m}_{liq,k} \cdot (1 - FW_k) \cdot (1 - \beta_k)}{B_{o,k} \cdot \rho_{l,IS,k}}$$

2. Gas rate:

$$q_{g,ST,k} = q_{o,ST,k} \cdot \text{RGO}_k$$

3. Weighted API (via density mixing):

$$\frac{141.5}{\text{API}_{mix} + 131.5} = \frac{\sum_k \frac{141.5}{(\text{API}_k + 131.5)} \cdot q_{o,ST,k}}{\sum_k q_{o,ST,k}}$$

   which is equivalent to:

$$\text{API}_{mix} = \frac{141.5}{\dfrac{\sum_k \frac{141.5}{\text{API}_k + 131.5} \cdot q_{o,ST,k}}{\sum_k q_{o,ST,k}}} - 131.5$$

4. Gas density (weighted by gas rate):

$$\rho_{g,mix} = \frac{\sum_k \rho_{g,k} \cdot q_{g,ST,k}}{\sum_k q_{g,ST,k}}$$

5. Temperature (enthalpy-weighted):

$$T_{mix} = \frac{\sum_k (\dot{m}_{liq,k} \cdot c_{p,l,k} + \dot{m}_{gas,k} \cdot c_{p,g,k}) \cdot T_k}{\sum_k (\dot{m}_{liq,k} \cdot c_{p,l,k} + \dot{m}_{gas,k} \cdot c_{p,g,k})}$$

6. BSW from water rate:

$$\text{BSW}_{mix} = \frac{q_{w,total}}{q_{l,std,total}}$$

7. Mixed RGO:

$$\text{RGO}_{mix} = \frac{q_{g,ST,total}}{q_{o,ST,total}}$$

**Negative-flow branches** (collectors flowing back into the node) contribute separately and are subtracted from the totals.

The resulting mixed properties are stored in a `convergeNoPerm` struct and applied to a `ProFlu` fluid object via `RenovaFluido()`.

For **compositional models**, `totalizaCicloRedeComp()` performs the same logic but tracks molar compositions and flash-calculated properties.

---

## Convergence and Relaxation

The network convergence norm is the **mean relative pressure change** at junction nodes:

$$\text{norma} = \frac{\sqrt{\displaystyle\sum_{\text{nodes}} \left(\frac{P_{\text{new}} - P_{\text{old}}}{P_{\text{old}}}\right)^2}}{N_{\text{nodes}}}$$

> **Note**: this is **not** a standard RMS — it is `sqrt(sum) / N`, not `sqrt(sum/N)`. The squared terms are normalised by $P_\text{old}^2$, making the metric dimensionless and scale-independent.

Convergence criteria:

| Parameter | Default | Source |
|-----------|---------|--------|
| `limConverge` | 0.001 | Network JSON `"limiteConvergencia"` |
| Maximum iterations | 200 | Hardcoded in `convergeRede()` |
| Relaxation factor `ω` | 0.5 | Network JSON `"Relaxacao"` |

**Adaptive relaxation**: if convergence stalls (error change < 0.01), the relaxation factor is halved (minimum 0.1):

$$\omega \leftarrow \max(0.5 \cdot \omega, \; 0.1)$$

Node pressure updates are under-relaxed:

$$P_{\text{upstream}} = \omega \cdot P_{\text{collector}} + (1 - \omega) \cdot P_{\text{upstream}}^{\text{old}}$$

---

## Branch-Level Steady-State Dispatch

Inside `cicloRede()`, each branch is solved by one of four methods depending on flow direction and boundary condition type:

| Condition | Method | Description |
|-----------|--------|-------------|
| Forward flow, choke opening > 0.6 | `buscaProdPfundoPerm()` | Root search on BHP; choke inactive |
| Forward flow, choke opening ≤ 0.6 | `buscaProdPfundoPerm2()` | Root search on BHP; active choke |
| Reverse flow | `buscaProdPfundoPermRev()` | Same root search, reversed march direction |
| Pressure-pressure BCs | `buscaProdPresPresPerm()` | Root search on flow rate given both-end pressures |

### `buscaProdPfundoPerm()`

Finds the bottom-hole pressure that makes the steady-state march match the imposed downstream pressure `pGSup`. Uses `marchaProdPerm1(pchute)` as the objective function and **Ridder's method** (`zriddr()`) for root finding.

**Initial pressure guess** — hydrostatic reverse march from `pGSup`:

$$p_{i-1} = p_i + \frac{(\rho_{\text{mix}} \, g \sin\theta + f_{\text{fric}}) \, \Delta x}{98066.5}$$

where $\rho_{\text{mix}} = (1-\alpha)\rho_l + \alpha \rho_g$ and the friction term is $f \rho_{\text{mix}} j|j| P_{\text{peri}} / (2A)$.

### `buscaProdPresPresPerm()`

For pressure-pressure BCs (imposed pressure at both ends), the unknown is the **flow rate**. The objective function `marchaProdPresPres1(mchute)` marches from the inlet at imposed pressure and returns the residual vs the imposed outlet pressure.

---

<a id="initial-pressure-guess--chutepresrede"></a>
## Initial Pressure Guess — chutePresRede

`chutePresRede()` provides initial node pressures before the first network iteration. It performs a **recursive hydrostatic march** from terminal collectors upstream:

1. Starting from the terminal branch's downstream pressure, calls `hidroreverso()` which marches backwards cell-by-cell:

$$p_{i-1} = p_i + \frac{\rho_{\text{mix}} \, g \sin\theta \, \Delta x + \Delta p_{\text{fric}} \, \Delta x}{98066.5}$$

2. The resulting pressure is assigned to all upstream afluents' `pGSup`
3. Recurses into each afluent's afluents
4. Flow rate per branch is estimated proportional to pipe area:

$$Q_i = Q_{\text{total}} \cdot \frac{A_i}{\sum_j A_j}$$

`hidroreverso()` also accounts for accessories: BCS pumps (tipo 4, 17) subtract pump head, generic ΔP accessories (tipo 7) subtract their `delp`, and pressure is clamped near IPR reservoir pressure.

---

## Transient Network Solver

After the steady-state solution converges, the transient solver advances the network in time with coupled boundary conditions.

### `SolveRedeTrans()` — Driver

([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

```
SolveRedeTrans(malha[], arqRede, ...)
  │
  │  while lixo5R < TmaxR:
  │    │
  │    ├─ Hydrate checks (FA_Hidrato) on each branch
  │    │
  │    ├─ Global CFL time step:
  │    │    dt = min_i( malha[i].determinaDT() )
  │    │    Apply dt uniformly to all branches and all cells
  │    │
  │    ├─ Valve state snapshot: aberturaVal0() on each branch
  │    ├─ Update time-dependent BCs: arq.atualiza(), atualizaCC1()
  │    ├─ Valve state check: aberturaVal1()
  │    │    If valve changed → force modeloCompleto = 0
  │    │
  │    ├─ Save initial-state backup:
  │    │    fluiIni[i], fluiFim[i], fluiInjM[i], fonteG[i], fonteP[i], fonteC[i]
  │    │    (pressures, BCs, mass sources, fluid objects at ghost cells)
  │    │
  │    ├─ Coupled pressure-volume iteration:
  │    │    for kontaAcop = 0 to modeloCompletoGlob:
  │    │      ├─ Set m2d flag per cell (dp/dt threshold for fully-implicit)
  │    │      ├─ EvoluiFrac(alfRev, betRev, kontaAcop)  ← advance volume fractions
  │    │      │    (includes porous sub-solvers: radialPoro.avancoSW, poroso2D.avancoSW)
  │    │      ├─ IF reinicia == -1: halve dt, restore backup, restart step
  │    │      ├─ celAfluFinal()         ← copy collector cell[0] into afluent ghost cell
  │    │      ├─ calcCCpres()           ← apply pressure BCs
  │    │      ├─ renovaterm()           ← update thermodynamics
  │    │      ├─ Inner BC propagation loop (iterRedeT < 1):
  │    │      │    ├─ CicloRedeTrans()  ← propagate BCs between branches
  │    │      │    ├─ SolveAcopPV()     ← coupled P-V solve
  │    │      │    ├─ renovaBuffer()    ← update buffer values
  │    │      │    └─ calcCCBuffer()    ← apply buffered BCs
  │    │      ├─ CicloRedeTrans()       ← final BC propagation
  │    │      ├─ SolveAcopPV(), renova(), marchaEnergTrans()
  │    │      └─ IF not last coupling iter:
  │    │           FeiticoDoTempo2/3() → shift time levels, restore backup
  │    │
  │    ├─ Post-coupling: update alfRev for inactive nodes, energy march
  │    ├─ SolveTrans() per branch       ← output trends
  │    ├─ AtualizaPig() per branch      ← advance PIG if active
  │    ├─ WriteSnapShot() at scheduled times (tempsnp[])
  │    │
  │    └─ lixo5R += dt
```

**Time step**: the global dt is the **minimum** CFL-limited time step across all active branches, applied uniformly to ensure synchronised advancement. `atenuaDtMax()` and `restringeDTporValv()` further constrain the time step.

**Valve transient safety**: `aberturaVal0()` records valve openings before BC updates; `aberturaVal1()` records them after. If any valve opening changed, `modeloCompleto` is forced to 0 (simplified model without $\partial p / \partial t$ correction), preventing numerical instability during valve transients.

**Coupling iteration**: the `kontaAcop` loop runs 1 or 2 times depending on `modeloCompletoGlob` (computed as the AND of all active branches' `modeloCompleto` flags). When running twice, `FeiticoDoTempo2()` / `FeiticoDoTempo3()` swap time-level arrays (e.g., $n \leftrightarrow n+1$) and restore the initial-state backup so the second pass uses improved pressure estimates.

**Restart mechanism**: if any branch signals `reinicia == -1` (CFL violation in `EvoluiFrac`), the global flag `reiniGlob` triggers a restart: the time step is halved, `ReiniEvolFrac0()` / `ReiniEvolFrac()` reset cell states to the beginning of the step, and the coupling loop restarts from scratch.

### `CicloRedeTrans()` — One Transient Iteration

([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

Propagates boundary conditions between branches for one time step. Uses the same topological traversal as the steady-state `cicloRede()`:

1. **Leaf branches first** (no afluentes) — marked as resolved
2. **Dependent branches** — wait until all afluentes are resolved, then:
   - Sort collectors by diameter → `ordCol[]` (largest = master)
   - Collect mass fluxes from afluent ghost cells (`cell[fim+1]`):
     - First iteration (`iterRedeT == 0`): use direct mass fluxes `fontemassLR`, `fontemassCR`, `fontemassGR`
     - Subsequent iterations: use buffered values `fontemassPRBuf`, `fontemassCRBuf`, `fontemassGRBuf`
   - Subtract reverse contributions from collectors (cell[0] mass with sign flip)
   - Mix fluid properties (same mass/enthalpy weighting as steady-state):
     - Oil/gas/water rates, RGO, BSW, API (density-weighted), gas density, temperature (enthalpy-weighted)
   - **Master collector** (largest diameter or `principal == 1`) receives:
     - Mass fluxes: `MassP`, `MassC`, `MassG` from the mass balance residual
     - Mixed fluid properties at its `injm` accessory
   - **Secondary collectors** receive:
     - `presE` from master's cell[0] pressure
     - Mixed `tempE`, `titE`, `betaE` from the node
     - Zero mass injection (`ConContEntrada = 1`)
   - Propagate `pGSup` upstream to each afluent with relaxation
   - Update afluent end-cell fluid to the mixed fluid object

**Reverse-flow handling**: `titrev[]`, `alfrev[]`, `betrev[]` arrays propagate reverse-flow composition upstream. If a branch's outlet mass is negative and the node is all-gas, liquid mass sources are zeroed via `corrigeVazNo()` / `corrigeVazNoBuf()`.

**Restart logic**: if any branch signals `reinicia == -1` (CFL violation), the time step is halved and the entire step restarts.

---

## Transient Helper Functions

These helper functions support boundary condition propagation and mass balance corrections during transient network simulation. All are in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp).

### `celAfluFinal()`

Copies the master collector's cell[0] state into the afluent branch's end ghost cell (cell `ncel`):

1. Finds master collector via `buscaNoColetorMrestre()`
2. Sums total mass (`MC`, `Mliqini`) across all collectors at the node
3. Copies geometry, fluid, temperature, pressure, alpha, beta, PIG fractions from collector cell[0] into the afluent's ghost cell
4. Sets `pGSup = malha[ncol].celula[0].pres`
5. If no choke at the top: updates alpha/beta from `titRev` / `betaRev`

### `corrigeVazNo()` / `corrigeVazNoBuf()`

Corrects mass flows at network nodes to prevent unphysical negative masses:

- **Gas-only node** (`titRev ≥ 1 − ε`): if liquid mass at end cell is negative, zeroes it and adjusts total mass
- **Liquid-only node** (`titRev ≤ ε`): if liquid mass is negative, sets total mass = liquid mass (gas = 0)

`corrigeVazNoBuf()` performs the same logic on buffered values (`fontemass*RBuf`).

### `verificaFonteDuplaReversa()`

Checks if a branch has dual sources (cell[0] and cell[1]) with opposing flow signs. Returns `−1` if cell[0]'s source should be swapped (cell[1] has larger magnitude), else `1`. Handles three accessory types:

- Gas injection (`tipo == 1`): compares `QGas` signs/magnitudes
- Liquid injection (`tipo == 2`): compares `QLiq` signs/magnitudes
- Mass injection (`tipo == 10`): compares total mass (`MassC + MassG + MassP`)

### `trocaFonteColetor()` / `retornaFonteColetor()`

- `trocaFonteColetor()`: same detection as `verificaFonteDuplaReversa()` but **actually swaps** cell[0] ↔ cell[1] accessory data when cell[1] has larger magnitude. Returns `−1` if swap occurred.
- `retornaFonteColetor()`: unconditionally swaps back cell[0] ↔ cell[1] (reverses a previous swap).

### `avaliaBloq()`

Evaluates **manifold blocking** configuration at a 2-way junction (2 collectors sharing afluentes):

1. If the node has ≠ 2 collectors: all afluentes belong to the same block → `nBloq[k] = 1` for all
2. If exactly 2 collectors (`col2`): partitions afluentes into three sets:
   - `nBloq[k] = 1`: shared (appears in both blocking lists)
   - `nBloq1[k] = 1`: belongs only to collector `i`
   - `nBloq2[k] = 1`: belongs only to collector `col2`

### `ranqueiaCol()`

Recursive function returning the depth of the network subtree downstream of branch $i$:

$$\text{rank}(i) = \begin{cases} 1 & \text{if any direct collector of } i \text{ is terminal (ncoleta == 0)} \\ \displaystyle\sum_{j \in \text{non-terminal collectors}(i)} (\text{rank}(j) + 1) & \text{otherwise} \end{cases}$$

Used during master collector selection to prefer the branch with the deepest downstream tree.

---

<a id="production-network--solveredeprod--redeprod"></a>
## Production Network — solveRedeProd / RedeProd

Production networks (`tipoRede == 0`) use two entry-point functions in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp):

- **`solveRedeProd()`** — called from `main()` inside the OpenMP parallel loop over sub-networks. Assumes SProd objects are already constructed by `preparaRedeProd()`. Orchestrates steady-state convergence, profile output, and initial-condition storage.
- **`RedeProd()`** — legacy self-contained driver that constructs SProd objects internally and then solves. Used when branch construction and solving are not separated.

### `solveRedeProd()` — Primary Entry Point

```
solveRedeProd(malha[], arqRede, ...)
  │
  ├─ testaBloqueio() per branch      ← detect manifold blocking
  ├─ avaliaPerm()                    ← deactivate zero-flow branches
  ├─ verificaTramoVazPres()          ← validate BCs on each collector branch
  ├─ Validate fluid model consistency (black-oil vs compositional)
  │
  ├─ IF contapermRede < narq (not all branches inactive):
  │    ├─ while restartRede == 1:
  │    │    ├─ Identify terminal collectors (ncoleta == 0)
  │    │    ├─ chutePresRede() or use user-supplied presJus/presMon
  │    │    ├─ Build normaEvol[] node topology
  │    │    │
  │    │    ├─ IF compositional (flashCompleto == 2):
  │    │    │    ├─ alteraModoFluidoCompBlack() → switch to black-oil
  │    │    │    ├─ convergeRede() → fast black-oil convergence
  │    │    │    ├─ alteraModoFluidoBlackComp() → restore compositional
  │    │    │    ├─ IF tabelaDinamica == 1:
  │    │    │    │    └─ cicloRedeCompCego() → build dynamic PVT tables
  │    │    │    └─ convergeRede() → full compositional convergence
  │    │    ├─ ELSE: convergeRede() → black-oil convergence
  │    │    └─ end while
  │
  ├─ Print profiles, trends, Poisson/Poroso outputs
  ├─ Store cell states as transient initial conditions:
  │    pres → presini, alf → alfini, bet → betini, etc.
  │
  └─ Write relatorioSucessoRede.dat (convergence status per branch)
```

### `RedeProd()` — Legacy Combined Driver

Follows the same algorithm as `solveRedeProd()` but additionally constructs the SProd objects internally:

1. If `narq > 1` (multi-branch): loops over all branches, constructs `SProd temporario(...)` from JSON, assigns to `malha[i]`, sets `noextremo`/`noinicial` flags
2. For pressure-BC branches (`ConContEntrada == 1`): creates inlet injection accessories (`injl`/`injg`), estimates initial RGO, BSW, API from inlet conditions
3. Distributes initial flow proportional to pipe area
4. Stores `fluiRevRede` from collector branches
5. Calls `avaliaPerm()`, validates fluid models, then enters the same `while(restartRede)` convergence loop
6. If `narq == 1`: constructs a single SProd and calls `SolveTramoSolteiro()`

---

<a id="gas-lift-loop--redeanelgl"></a>
## Gas-Lift Loop — RedeAnelGL

`RedeAnelGL()` solves a gas distribution annulus feeding multiple production wells:

### Topology

- **Well branches** (`tipoanel == 0`): production pipes with gas-lift injection
- **Annular branch** (`tipoanel == 1`): the gas distribution line

Each well is connected to the annulus at a "dreno" (drain) point. Gas injection accessories (`InjGas`) are placed at each dreno position on the annulus.

### Steady-State Algorithm

For `ConContEntrada == 0` (flow BC at annulus inlet):
- Uses **Ridder's method** on the annulus inlet pressure
- Objective function: `objetCC0()` calls `marchaProdPerm1()` for the annulus, then `calcPeriAnelGL()` to solve each well, then `calcErroGL()` for the gas balance error

For `ConContEntrada == 1` (pressure BC at annulus inlet):
- Direct iterative loop calling `calcPeriAnelGL()` + `calcErroGL()`
- Each well is solved via `buscaProdPfundoPerm()` or `buscaProdPfundoPerm2()` (depending on choke opening)
- Gas consumed by each well is fed back to the annulus as a sink

### Gas Balance

`calcErroGL()` computes the normalised gas mass balance error:

$$\text{erro} = \frac{\sum_i Q_{gas,i}}{|Q_{gas,\text{inlet}}|}$$

where $Q_{gas,i}$ sums over all injection points and drenos. Convergence: $|\text{erro}| < 10^{-4}$.

### Transient — TransAnel

`TransAnel()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) performs gas-lift transient time-stepping:

```
TransAnel(narq, nfontes, indfonte, indtramo, posicfonte, indAnel, dreno, malha, arqRede)
  │
  │  while lixo5R < TmaxR:
  │    ├─ Hydrate evaluations
  │    ├─ Global CFL-limited dt (min across all branches)
  │    ├─ Update valve/BC for each branch
  │    │
  │    ├─ Update drain gas flows:
  │    │    For each drain point:
  │    │      annulus cell gas injection -= well branch gas consumption
  │    │      (QGas -= VGasR × 86400 / ρ_gas_std)
  │    │
  │    ├─ Coupling loop (kontaAcop):
  │    │    ├─ EvoluiFrac() on all branches (production + annulus)
  │    │    ├─ Restart logic (halve dt if reinicia == -1)
  │    │    ├─ Porous media sub-solvers (if applicable)
  │    │    ├─ calcCCpres(), renovaterm(), SolveAcopPV()
  │    │    └─ marchaEnergTrans()
  │    │
  │    ├─ Post-coupling: update fontechk shared source conditions:
  │    │    primary → secondary: ambient pressure, temperature, quality
  │    │    secondary → primary: reciprocal conditions
  │    │
  │    ├─ SolveTrans() on all branches (output trends)
  │    └─ WriteSnapShot() at scheduled times
```

Key difference from `SolveRedeTrans()`: the annulus branch's gas injection sources at drain points are updated each time step based on the gas consumed by each production well, maintaining the gas mass balance dynamically.

---

<a id="parallel-network--redeparalela"></a>
## Parallel Network — RedeParalela

`RedeParalela()` solves two co-located pipelines (primary + secondary) coupled at shared source points (`fontechk`):

### Topology

- **Primary tramo** (`tramoPrimario == 1`): the main pipeline
- **Secondary tramo** (`tramoPrimario == 0`): the companion pipeline
- **Coupling points**: `conexFR[]` pairs mapping cell positions `noP ↔ noS`

### Steady-State Algorithm

Iterative alternation between the two branches:

```
repeat:
  1. Update ambient pressure/temperature on primary shared chokes from secondary
  2. Solve primary: SolveTramoSolteiro(malha[iP])
  3. Update thermal coupling fluxes from primary → secondary
  4. Update ambient conditions on secondary shared chokes from primary
  5. Solve secondary: SolveTramoSolteiro(malha[iS])
     (or chutePresRedeParalelaSec() for non-pressure BC)
  6. Compute error from relative change in resulP, resulS
until erro < 1e-5
```

**Thermal coupling**: if `verificaAcopRedeP == 1`, cells of primary and secondary exchange thermal resistance values via `fluxcalAcopRedeP`.

**Convergence criterion**:

$$\frac{|p_P^{n+1} - p_P^n|}{|p_P^n|} + \frac{|p_S^{n+1} - p_S^n|}{|p_S^n|} < 10^{-5}$$

### Transient — SolveRedeParalelaTrans

`SolveRedeParalelaTrans()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) performs time-stepping for parallel pipes:

```
SolveRedeParalelaTrans(malha[], arqRede, nrede)
  │
  │  while lixo5R < TmaxR:
  │    ├─ Thermal coupling: conectaPrincipal() + fluxcalAcopRedeP
  │    ├─ Hydrate evaluation, CFL dt, valve updates for both branches
  │    │
  │    ├─ Coupling loop (kontaAcop):
  │    │    ├─ EvoluiFrac() on both branches
  │    │    ├─ Restart logic (halve dt if reinicia == -1)
  │    │    ├─ Porous media sub-solvers (if applicable)
  │    │    ├─ calcCCpres(), renovaterm(), SolveAcopPV()
  │    │    └─ marchaEnergTrans()
  │    │
  │    ├─ Post-coupling: update fontechk shared source conditions:
  │    │    primary → secondary: ambient pressure, temperature, quality
  │    │    secondary → primary: reciprocal conditions
  │    │
  │    ├─ SolveTrans() on both branches (output trends)
  │    └─ WriteSnapShot() at scheduled times
```

**`conectaPrincipal()`**: copies flux data from the primary branch's cells to corresponding cells in the secondary branch at each coupling point (`conexFR[]`), ensuring both pipes share consistent pressure and temperature at the shared `fontechk` accessories.

**`chutePresRedeParalelaSec()`**: provides an initial pressure guess for the secondary branch based on a hydrostatic estimate from the primary's solution.

---

<a id="injection-network--redeInj"></a>
## Injection Network — RedeInj

`RedeInj()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) solves injection networks (`tipoRede == 1`) — typically water or gas injection from surface to wells. Steady-state only.

### Algorithm

```
RedeInj(malha[], arqRede, ...)
  │
  ├─ Construct all SProd branches, set noextremo/noinicial
  ├─ Accumulate somavaz/somaarea for initial flow distribution
  ├─ Validate fluid model consistency
  │
  ├─ chutePresRedeInj() → initial bottom-hole pressure guess
  │
  ├─ while restartRede == 1:
  │    while norma > 0.001 × (*vg1dSP).relax:
  │      norma = cicloRedeInj(malha, arqRede, inativo, indativo)
  │
  └─ Print profiles
```

### `chutePresRedeInj()` — Initial Pressure Guess

([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

Recursive hydrostatic pressure estimate for injection networks. Same principle as `chutePresRede()` but calls `hidroreversoInj()` (injection-direction march):

1. Compute proportional flow: $Q_i = Q_{\text{total}} \cdot A_i / \sum A_j$
2. Call `malha[i].hidroreversoInj(chutehol, vaz)` → estimated node pressure
3. For each afluente: set `arq.condpocinj.presfundo = presno`, recurse

### `cicloRedeInj()` — Injection Network Iteration Cycle

([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp))

One full iteration of the injection network solver. Returns the RMS pressure norm. Uses the same topological traversal as `cicloRede()` but with injection-specific solvers:

**Leaf branches** (`nafluente == 0`):

- If `condpocinj.CC == 3`: calls `buscaInjPfundoPerm1()`
- If `condpocinj.CC == 5`: calls `buscaInjPfundoPerm5()`

**Non-leaf branches** (once all afluentes are resolved):

1. Mix fluid properties from all afluentes (temperature weighted by flow rate, total liquid/gas rates)
2. Sort collectors by diameter → `ordCol[]` (largest = master)
3. **Master collector** (last in sorted order): receives total mixed flow, calls `buscaInjPfundoPerm2()` or `buscaInjPfundoPerm5()`
4. **Non-master collectors**: fraction of mixed flow proportional to pipe area:

$$Q_i = Q_{\text{total}} \cdot \frac{A_i}{\sum_j A_j}$$

   Calls `buscaInjPfundoPerm1()` for each.

5. Update bottom-hole pressures at afluentes via under-relaxation:

$$P_{\text{fund,aflu}} = \omega \cdot P_{\text{collector}} + (1 - \omega) \cdot P_{\text{fund,aflu}}^{\text{old}}$$

**Convergence norm**: $\text{norma} = \sqrt{\sum_{\text{nodes}} (P_{\text{new}} - P_{\text{old}})^2} \,/\, N_{\text{nodes}}$

**Failure handling**: if a branch solve fails (velocity $> 10^9$), it is deactivated via `inativoColetor()` / `inativoAfluente()`, and the iteration marks a restart.

---

## Compositional Model Switching

When any branch in the network uses a compositional fluid model (`flashCompleto == 2`), the solver cannot use the standard black-oil `cicloRede()`. Instead, it employs a **two-pass strategy** and a dedicated compositional cycle with molar mixing at network nodes.

### Two-Pass Strategy

The two-pass approach is implemented in `solveRedeProd()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)):

```
if flashCompleto == 2:
    ┌─ Pass 1 — Black-Oil Pre-Solve ─────────────────────────┐
    │  alteraModoFluidoCompBlack(malha, narq, ...)            │
    │    → Sets flashCompleto=0 on ALL cells/accessories      │
    │  convergeRede(malha, ...) using cicloRede()             │
    │    → Fast convergence with black-oil correlations       │
    │    → Establishes pressure/temperature/flow baseline     │
    └─────────────────────────────────────────────────────────┘
                           │
                           ▼
    ┌─ Pass 2 — Compositional Refinement ─────────────────────┐
    │  alteraModoFluidoBlackComp(malha, narq, ...)            │
    │    → Restores flashCompleto=2 on ALL cells/accessories  │
    │  IF tabelaDinamica == 1:                                │
    │    cicloRedeCompCego(malha, ...)                        │
    │      → One decoupled pass to prepare dynamic PVT tables │
    │  convergeRede(malha, ...) using cicloRedeComp()         │
    │    → Full compositional iteration with molar mixing     │
    └─────────────────────────────────────────────────────────┘
```

**Rationale:** Compositional flash calculations are expensive. Running a fast black-oil solve first provides a good initial guess for the pressure and flow-rate distribution, allowing the compositional solve to converge in fewer iterations.

### `alteraModoFluidoCompBlack()`

Temporarily switches every branch and every cell to black-oil mode. For each branch $j$ and each cell $i$:

| Action | Details |
|--------|---------|
| Save originals | `calclat0[j] = CalcLat`, `tipoFluido0[j] = tipoFluido` |
| Set black-oil mode | `flashCompleto = 0`, `modoBlackTemp = 0`, `blackOilTemp = 1` |
| Disable latent heat | `CalcLat = 0` |
| Override fluid type | `tipoFluido = 0` |

The switch propagates to **all** `ProFlu` instances nested inside accessories:
- `injg.FluidoPro` (gas injection, tipo 1)
- `injl.FluidoPro` (liquid injection, tipo 2)
- `ipr.FluidoPro` (IPR, tipo 3)
- `injm.FluidoPro` (mass injection, tipo 10)
- `radialPoro.flup` and all radial porous cells (tipo 15)
- `poroso2D.dados.flup`, transfer cells, and mesh elements (tipo 16)
- `fluiRevRede` (reverse-flow fluid)

### `alteraModoFluidoBlackComp()`

Reverses the switch after the black-oil pre-solve:

| Action | Details |
|--------|---------|
| Restore `flashCompleto = 2` | On all cells and accessory `ProFlu` instances |
| Restore `CalcLat` | From saved `calclat0[j]` |
| Restore `tipoFluido` | From saved `tipoFluido0[j]` |
| Reset search flag | `buscaIni = 0` (force fresh root-finding bracket search) |
| Sync `npseudo` | Copy pseudo-component count from interior cells to boundary ghost cells |

---

<a id="compositional-network-cycle--cicloredecomp"></a>
## Compositional Network Cycle — cicloRedeComp

`cicloRedeComp()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) replaces `cicloRede()` when `flashCompleto == 2`. It follows the same topological traversal pattern (leaves first, then dependents) but adds **molar composition tracking** at network nodes.

### How `convergeRede` selects the cycle

```cpp
if (malha[0].arq.flashCompleto != 2)
    norma = cicloRede(malha, arqRede, ...);      // standard black-oil
else
    norma = cicloRedeComp(malha, arqRede, ...);   // compositional
```

### Key differences from cicloRede

| Aspect | `cicloRede` | `cicloRedeComp` |
|--------|-------------|-----------------|
| Fluid mixing | Mass/energy-weighted averaging | **Molar-weighted** composition averaging |
| Mixing function | `totalizaCicloRede()` | `totalizaCicloRedeComp()` |
| Composition propagation | Not tracked | `fracMol[]` propagated from nodes to downstream branches |
| Fluid object at injection | BSW/RGO/API copied | Full `ProFlu` with `fracMol[]` assigned |
| Dynamic tables | Not involved | `tabelaDinamica = 0` reset at injection points |
| Convergence norm | Pressure change only | Pressure change **+ flow rate change** |
| Collector ranking | By diameter | By diameter, with `principal` flag override |

### Molar mixing at nodes

When multiple upstream branches feed into a node, `totalizaCicloRedeComp()` computes the mixed composition using **molar flow rate weighting**:

For each upstream branch $k$ with positive flow:

1. Compute the oil mass fraction (water-free):

$$\text{titW}_k = \frac{(1 - \text{fw}_k) \cdot \rho_{o,k}}{(1 - \text{fw}_k) \cdot \rho_{o,k} + \text{fw}_k \cdot \rho_{w,k}}$$

2. Compute the hydrocarbon mass flow rate (oil + gas, excluding water):

$$\dot{m}_{\text{HC},k} = \text{titW}_k \cdot (\dot{m}_{liq,k} - \dot{m}_{comp,k}) + \dot{m}_{gas,k}$$

3. Compute the average molecular weight from pseudo-component fractions:

$$\overline{M}_k = \sum_{j=1}^{n_{\text{pseudo}}} M_j \cdot z_{j,k}$$

where $z_{j,k}$ = `fracMol[j]` for branch $k$ and $M_j$ = `masMol[j]`.

4. Compute the molar flow rate:

$$\dot{n}_k = \frac{\dot{m}_{\text{HC},k}}{\overline{M}_k}$$

5. Accumulate the total molar flow and mixed composition:

$$\dot{n}_{\text{total}} = \sum_k \dot{n}_k$$

$$z_{j,\text{mix}} = \frac{\sum_k \dot{n}_k \cdot z_{j,k}}{\dot{n}_{\text{total}}}$$

The mixed composition is stored in `noConv.flu.fracMol[]` and assigned to the master collector's inlet accessory.

### Negative-flow contributions

Branches with negative mass flow at the node boundary (reverse flow from collectors) are tracked separately. Their molar contributions (`moleomistNeg`, `mliqmistNeg`, `mgasmistNeg`) are accumulated independently and added with correct sign when setting boundary conditions on the master collector.

### Master collector assignment

After mixing, the master collector (largest diameter or `principal == 1` flag) receives:

- **Composition**: `FluidoPro = noConv.flu` (the `ProFlu` with mixed `fracMol[]`)
- **Temperature**: `temp = noConv.tempmist` (enthalpy-weighted)
- **BSW**: `bet = noConv.betmist` (volume-fraction-weighted)
- **Flow rate**: mixed liquid or gas rate, depending on `fluidoRede`
- **Residence temperature**: `fluidocol.TR = noConv.TRmist` (for compositional tracking)
- **Dynamic table flag**: `tabelaDinamica = 0` (forces re-computation of PVT from composition)

Secondary collectors receive the same mixed fluid but with BCs derived from the master's pressure:

$$P_{\text{secondary}} = P_{\text{master, cell[0]}}$$

### Convergence norm

The compositional cycle includes **both pressure and flow-rate changes** in the norm. At each node where the master collector's flow rate is updated:

$$\text{norma} \mathrel{+}= \left(\frac{Q_{\text{old}} - Q_{\text{new}}}{Q_{\text{new}}}\right)^2$$

This ensures convergence accounts for composition-driven flow redistribution.

---

<a id="blind-compositional-cycle--cicloredecompcego"></a>
## Blind Compositional Cycle — cicloRedeCompCego

`cicloRedeCompCego()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) is a simplified, **decoupled** compositional pass used as a **transition step** between the black-oil pre-solve and the full compositional iteration. "Cego" (Portuguese for "blind") means it solves without inter-branch pressure coupling.

### When it is called

```cpp
// After black-oil → compositional switch
alteraModoFluidoBlackComp(malha, narq, calclat0, tipoFluido0);
if (malha[0].arq.tabelaDinamica == 1)
    cicloRedeCompCego(malha, arqRede, inativo, indativo, bloq);
// Then full compositional convergence
convergeRede(malha, arqRede, ...);  // → cicloRedeComp()
```

### How it differs from cicloRedeComp

| Aspect | `cicloRedeComp` | `cicloRedeCompCego` |
|--------|-----------------|---------------------|
| Pressure coupling | Full: secondary collectors coupled to master | **None**: each branch solved independently with current BCs |
| Purpose | Iterative convergence | One-shot initialization of compositional state |
| Dynamic PVT tables | Not pre-built | **Calls `preparaTabDin()`** per branch after solving |
| Number of passes | Many (within `convergeRede` loop) | **One** (single traversal) |
| Mixing | Full molar mixing via `totalizaCicloRedeCompCego()` | Simplified mixing (same formula but no pressure redistribution) |

### `preparaTabDin()` — Dynamic PVT Table Construction

After each branch is solved in the blind pass, `preparaTabDin()` pre-computes property tables on a $(P, T)$ grid:

1. **Determine grid bounds** from the solved cell pressure/temperature range:

$$P_{\min} = 0.6 \cdot P_{\min,\text{cells}}, \quad P_{\max} = 1.6 \cdot P_{\max,\text{cells}}$$

$$T_{\min} = T_{\min,\text{cells}} - 20°C, \quad T_{\max} = T_{\max,\text{cells}} + 20°C$$

2. **Build uniform grid** with spacing $\Delta P = 5$ kgf/cm², $\Delta T = 5°C$ (minimum 4 points per axis)

3. **Pre-compute properties** at each grid point via `atualizaPropComp()`:
   - Densities ($\rho_o$, $\rho_g$, $\rho_w$), their P and T derivatives
   - Viscosities ($\mu_o$, $\mu_g$)
   - Formation volume factors ($B_o$, $B_a$)
   - Solution GOR ($R_s$)
   - Heat capacities ($C_{p,l}$, $C_{p,g}$)
   - Enthalpies
   - Compressibilities

4. **Set `tabelaDinamica = 1`** — subsequent evaluations use fast bilinear interpolation on this grid instead of expensive flash calculations

This dramatically accelerates the convergence of the full compositional pass that follows.

---

## Cell-Level Composition Propagation

During the steady-state march within a compositional branch, composition must be propagated and mixed cell-by-cell. Two functions handle this:

### `atualizaComp(sistem1, i)` — Composition mixing at source cells

Called at cells where an external fluid source injects into the pipe (accessories tipo 1, 2, 3, 9, 10, 15, 16). Performs **molar-ratio mixing** between the upstream flow and the injected fluid:

1. **Extract source fluid** (`fluF`) from the accessory's `FluidoPro`
2. **Update source properties** via `atualizaPropComp()` at local $(P, T)$
3. **Compute molecular weights**:

$$\overline{M}_V = \sum_j M_j \cdot z_{j,V} \quad \text{(upstream flow)}$$

$$\overline{M}_F = \sum_j M_j \cdot z_{j,F} \quad \text{(source fluid)}$$

4. **Compute molar flow rates**:

$$\dot{n}_V = \frac{\dot{m}_{\text{HC,upstream}}}{\overline{M}_V}, \quad \dot{n}_F = \frac{\dot{m}_{\text{HC,source}}}{\overline{M}_F}$$

5. **Mix compositions**:

$$r = \frac{\dot{n}_V}{\dot{n}_V + \dot{n}_F}$$

$$z_{j,i} = r \cdot z_{j,i-1} + (1 - r) \cdot z_{j,F}$$

If the source molar flow $\dot{n}_F = 0$, the upstream composition passes through unchanged.

### `atualizaCel(sistem1, i)` — Composition propagation at plain cells

Called at cells with **no external source** (accessories without injection). Simply copies the upstream cell's composition downstream:

```cpp
celula[i].flui.fracMol[j] = celula[i-1].flui.fracMol[j];  // for all j
```

Additionally copies:
- Stock-tank thermodynamic condition flags
- Stock-tank phase densities and vapor mass fraction
- API gravity (recomputed from stock-tank liquid density)
- Gas density and RGO

Then calls `atualizaPropComp()` to recompute all properties at the new cell's $(P, T)$ using the propagated composition.

---

## Snapshot and Output

### Steady-state output

After `convergeRede()` converges, each branch's spatial profiles are written (pressure, temperature, holdup, velocities, etc.) via the standard profile output system.

### Transient output

During `SolveRedeTrans()`:
- **Trends** are written per time step via `SolveTrans()` on each branch
- **Snapshots** are written at user-specified times via `WriteSnapShot()`
- The global time step `dt` is logged for monitoring CFL behaviour

### Emergency snapshots

The `signalHandler()` function writes an emergency snapshot on SIGABRT, SIGFPE, SIGINT, SIGSEGV, or SIGTERM, allowing restart from the last saved state.

---

## Summary of Key Functions

| Function | File | Purpose |
|----------|------|---------|
| `Rede::Rede()` | LerRede.cpp | Parse network JSON → connectivity graph |
| `Rede::lerArq()` | LerRede.cpp | Master dispatcher: `parse_configuracao_inicial`, `parse_arquivos`, `parse_conexao`, `parse_fonteReciproca` |
| `Rede::parse_configuracao_inicial()` | LerRede.cpp | Solver parameters (relaxation, convergence, threads) |
| `Rede::parse_arquivos()` | LerRede.cpp | Branch file list → `impfiles[]` |
| `Rede::parse_conexao()` | LerRede.cpp | Connectivity: collectors, afluentes, blockages |
| `Rede::parse_fonteReciproca()` | LerRede.cpp | Parallel network coupling points |
| `descarteTramo()` | Num4Main.cpp | Remove inactive branches, prune connectivity |
| `preProcRede()` | Num4Main.cpp | DFS decomposition into independent sub-networks |
| `verficaConex()` | Num4Main.cpp | Recursive DFS graph traversal |
| `gravaRedeInterna()` | Num4Main.cpp | Write sub-network JSON for diagnostics |
| `preparaRedeProd()` | Num4Main.cpp | Build SProd objects, set initial BCs |
| `avaliaPerm()` | Num4Main.cpp | Multi-pass deactivation of zero-flow branches |
| `chutePresRede()` | Num4Main.cpp | Recursive hydrostatic initial pressure guess (production) |
| `chutePresRedeInj()` | Num4Main.cpp | Recursive hydrostatic initial pressure guess (injection) |
| `convergeRede()` | Num4Main.cpp | Outer convergence loop (max 200 iterations) |
| `cicloRede()` | Num4Main.cpp | One steady-state network iteration (topological order) |
| `cicloRedeComp()` | Num4Main.cpp | Compositional variant of `cicloRede()` |
| `cicloRedeCompCego()` | Num4Main.cpp | One-shot decoupled compositional pass (blind) |
| `cicloRedeInj()` | Num4Main.cpp | Injection network iteration cycle (topological order) |
| `totalizaCicloRede()` | Num4Main.cpp | Node-level fluid mixing (mass/energy-weighted) |
| `totalizaCicloRedeComp()` | Num4Main.cpp | Compositional node mixing (molar-weighted) |
| `totalizaCicloRedeCompCego()` | Num4Main.cpp | Simplified molar mixing (blind variant) |
| `CicloRedeTrans()` | Num4Main.cpp | One transient network time step (BC propagation) |
| `SolveRedeTrans()` | Num4Main.cpp | Transient production-network driver (time-marching loop) |
| `solveRedeProd()` | Num4Main.cpp | Production network entry point (SS convergence + output) |
| `RedeProd()` | Num4Main.cpp | Legacy combined production network driver (build + solve) |
| `RedeAnelGL()` | Num4Main.cpp | Gas-lift loop driver |
| `calcPeriAnelGL()` | Num4Main.cpp | Steady-state solve of each well in gas-lift loop |
| `calcErroGL()` | Num4Main.cpp | Gas balance error for gas-lift loop |
| `objetCC0()` | Num4Main.cpp | Objective function for gas-lift pressure matching |
| `zriddr()` | Num4Main.cpp | Ridder's root-finding method for gas-lift pressure |
| `TransAnel()` | Num4Main.cpp | Transient gas-lift loop driver |
| `RedeParalela()` | Num4Main.cpp | Parallel network driver |
| `conectaPrincipal()` | Num4Main.cpp | Couple flux data between primary and secondary branches |
| `chutePresRedeParalelaSec()` | Num4Main.cpp | Initial pressure guess for secondary branch |
| `SolveRedeParalelaTrans()` | Num4Main.cpp | Transient parallel network driver |
| `RedeInj()` | Num4Main.cpp | Injection network driver |
| `celAfluFinal()` | Num4Main.cpp | Copy collector cell[0] state into afluent ghost cell |
| `corrigeVazNo()` | Num4Main.cpp | Correct unphysical negative mass at network nodes |
| `corrigeVazNoBuf()` | Num4Main.cpp | Buffered variant of node mass correction |
| `verificaFonteDuplaReversa()` | Num4Main.cpp | Detect dual sources with opposing flow signs |
| `trocaFonteColetor()` | Num4Main.cpp | Swap cell[0] ↔ cell[1] accessories at reversed dual source |
| `retornaFonteColetor()` | Num4Main.cpp | Restore swapped accessories |
| `avaliaBloq()` | Num4Main.cpp | Evaluate manifold blocking at 2-way junctions |
| `buscaProdPfundoPerm()` | SisProd.cpp | Root search for BHP (forward, choke inactive) |
| `buscaProdPfundoPerm2()` | SisProd.cpp | Root search for BHP (forward, active choke) |
| `buscaProdPfundoPermRev()` | SisProd.cpp | Root search for BHP (reversed flow) |
| `buscaProdPresPresPerm()` | SisProd.cpp | Root search for flow rate (both-end pressure BCs) |
| `hidroreverso()` | SisProd.cpp | Backward hydrostatic march for pressure estimate |
| `alteraModoFluidoCompBlack()` | Num4Main.cpp | Switch all branches from compositional to black-oil |
| `alteraModoFluidoBlackComp()` | Num4Main.cpp | Restore compositional model |
| `cicloRedeComp()` | Num4Main.cpp | Compositional network iteration with molar mixing |
| `cicloRedeCompCego()` | Num4Main.cpp | One-shot decoupled compositional pass (blind) |
| `totalizaCicloRedeComp()` | Num4Main.cpp | Molar-weighted composition mixing at nodes |
| `totalizaCicloRedeCompCego()` | Num4Main.cpp | Simplified molar mixing (blind variant) |
| `atualizaComp()` | Num4Main.cpp | Cell-level molar mixing at source accessories |
| `atualizaCel()` | Num4Main.cpp | Cell-level composition propagation (no source) |
| `preparaTabDin()` | Num4Main.cpp | Build dynamic PVT tables on (P,T) grid |
| `ranqueiaCol()` | Num4Main.cpp | Recursive collector tree depth ranking |
