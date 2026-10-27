# Num4Main.cpp — Main Entry Point

`Num4Main.cpp` is the main source file of the Marlim3 simulator. It contains the `main()` function, global variable declarations, data structures used for network convergence, and all top-level solver orchestration routines for steady-state and transient simulations.

**Source:** [`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)  

---

## Table of Contents

1. [Command-Line Interface](#command-line-interface)
2. [High-Level Execution Flow](#high-level-execution-flow)
3. [Simulation Modes Explained](#simulation-modes-explained)
4. [Global Variables](#global-variables)
5. [Local Data Structures](#local-data-structures)
6. [Key Functions](#key-functions)
   - [main()](#main)
   - [Single-Branch Solvers](#single-branch-solvers)
   - [Network Simulation](#network-simulation)
   - [Parametric Analysis](#parametric-analysis)
   - [Utility / Support Functions](#utility--support-functions)
7. [Output and Restart I/O](#output-and-restart-io)
8. [Dependencies (Included Headers)](#dependencies-included-headers)
9. [Platform Macros](#platform-macros)

---

## Command-Line Interface

The simulator accepts the following arguments (parsed in `main()`):

| Flag | Long Form | Description |
|------|-----------|-------------|
| `-h` | `--help` | Show usage information |
| `-i` | `--input FILE` | Input JSON file (default: `teste1.json` for single branch, `rede.json` for network) |
| `-o` | `--output FILE` | Log output file (default: `output/simulacao.log`) |
| `-d` | `--dir DIR` | Output directory for results |
| `-p` | `--path PATH` | Path to external input files (e.g. PVTSIM tables) |
| `-t` | `--threads N` | Number of threads (reserved for future use) |
| `-v` | `--validate MODE` | JSON validation mode: `JSON`, `SCHEMA`, `RULES`, or `OFF` |
| `-s` | `--simulate TYPE` | Simulation type (see below) |

### Simulation Types (`-s`)

| Value | Enum | Description |
|-------|------|-------------|
| `TRANSIENTE` | `tipoSimulacao_t::transiente` | Single branch: steady-state + transient (default) |
| `INJETOR` | `tipoSimulacao_t::poco_injetor` | Injection well system |
| `REDE` | `tipoSimulacao_t::rede` | Flow network (production or injection) |
| `CONVECNAT` | `tipoSimulacao_t::convecNatural` | 2D natural convection in confined cross-section |

### Validation Modes (`-v`)

| Value | Enum | Description |
|-------|------|-------------|
| *(default)* | `tipoValidacaoJson_t::none` | Default initial value; treated as no validation |
| `JSON` | `tipoValidacaoJson_t::json` | Validate JSON structure/format |
| `SCHEMA` | `tipoValidacaoJson_t::schema` | Validate JSON + schema syntax |
| `RULES` | `tipoValidacaoJson_t::rules` | Validate JSON + schema + business rules |
| `OFF` | `tipoValidacaoJson_t::off` | Skip all validation |

> **Note:** Validation is currently forced to `OFF` regardless of the user's choice (hardcoded override immediately after parsing the `-v` argument).

---

## High-Level Execution Flow

```
main()
 │
 ├── Record start time (nowGlobIni)
 ├── Parse command-line arguments (-i, -o, -s, -v, -d, -p, -t)
 ├── Register signal handlers (SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM)
 ├── logger.changeResultadoSimulacao(...) → register input filename and log path in the Logger; records execution date
 ├── determinarPathArqEntSai() → resolve input/output file paths
 ├── Open resultado.log manifest (unless INJETOR or CONVECNAT)
 │
 ├── IF tipoSimulacao == REDE
 │    ├── Rede arqRede(...) → read network JSON
 │    ├── Set fields on the global shared-state struct `varGlob1D` (defined in `variaveisGlobais1D.h`): network flags (`chaverede`, `chaveredeT`, `chaveAnelGL`, `chaveRedeParalela`), file count (`narq`), fluid type (`fluidoRede`), max temperature (`TmaxR`)
 │    ├── Determine network sub-type:
 │    │    ├── Standard production network (not GL-loop, not parallel, not injection)
 │    │    │    ├── descarteTramo() → mark inactive branches
 │    │    │    ├── preProcRede() → decompose into internal sub-networks, write RedeInterna-{i}.json
 │    │    │    ├── Write relatorioSucessoRede.dat header
 │    │    │    ├── IF apenasPreProc == 0 (not pre-processing-only mode):
 │    │    │    │    ├── Allocate SProd** malha[redeLeitura]
 │    │    │    │    ├── For each sub-network: read RedeInterna-{i}.json → Rede, preparaRedeProd()
 │    │    │    │    ├── #pragma omp parallel for: For each sub-network:
 │    │    │    │    │    ├── solveRedeProd() → steady-state network solution
 │    │    │    │    │    ├── IF chaveredeT == 1: SolveRedeTrans() → transient network solution
 │    │    │    │    │    └── Reset vg1d globals (restartRede, relax, erroRede, etc.)
 │    │    │    │    └── Append timing + date to relatorioSucessoRede.dat
 │    │    ├── Injection network → RedeInj() (steady-state only)
 │    │    ├── Gas-lift loop → RedeAnelGL() (steady-state + optional transient)
 │    │    └── Parallel network → RedeParalela() (steady-state + optional transient)
 │    │
 ├── ELSE IF tipoSimulacao == TRANSIENTE or INJETOR (single branch)
 │    ├── SProd sistem1(...) → build from input JSON (constructor triggers parse + montasistema)
 │    ├── ptrSistemaProducao = &sistem1 (for emergency snapshot on signal)
 │    │
 │    ├── IF perm == 1 (steady-state):
 │    │    ├── IF AP == 0: SolveTramoSolteiro(sistem1, chutePerm)
 │    │    │    ├── IF compositional (flashCompleto == 2):
 │    │    │    │    ├── Pass 1 — switch all cells to black-oil → permanenteSimples()
 │    │    │    │    ├── Extract pressure/flow-rate result as initial guess
 │    │    │    │    ├── Restore compositional flags on all cells + accessories
 │    │    │    │    ├── (Optional) preparaTabDin() → build dynamic PVT tables
 │    │    │    │    └── Pass 2 — permanenteSimples() with compositional model
 │    │    │    ├── IF black-oil or flash table: permanenteSimples() directly
 │    │    │    └── Print profiles, trends, Poisson/Poroso outputs, LogEvento
 │    │    ├── IF AP == 1, paralelAP == 0: leituraAP()
 │    │    └── IF AP == 1, paralelAP == 1: leituraAPparalelo()
 │
 │    ├── ELSE IF perm == 2 (restart from snapshot):
 │    │    └── ReadSnapShot(sistem1)
 │
 │    ├── IF transiente == 1:
 │    │    ├── IF ConContEntrada == 1 (inlet source → imposed-flow BC):
 │    │    │    └── Convert source terms to MC, Mliqini, QL, QG at cell 0; zero out sources
 │    │    ├── Initialize cell-level fluid properties for all i = 0..ncel:
 │    │    │    ├── Liquid density:   rpC, rpL (+ initial copies rpCi, rpLi)
 │    │    │    ├── Gas density:      rgC, rgL (+ rgCi, rgLi)
 │    │    │    ├── Compl. density:   rcC, rcL (+ rcCi, rcLi)
 │    │    │    ├── Oil viscosity:    mipC
 │    │    │    ├── Gas viscosity:    migC
 │    │    │    ├── Compl. viscosity: micC
 │    │    │    └── Propagate to right-face: rpR, rgR, rcR, mipR, migR, micR from cell i+1
 │    │    ├── determinaDT() → compute initial time step
 │    │    ├── renovaTemp() → prepare mass transfer model + update source terms
 │    │    ├── modoPerm = 0; modoTransiente = 1
 │    │    ├── (Optional) IF modoDifus3D == 1:
 │    │    │    ├── Construct solverP3D from UNV mesh
 │    │    │    ├── Match coupling surface labels to cell indices
 │    │    │    ├── Set tInt[], hI[] from cell temperatures and convection coefficients
 │    │    │    ├── inicializaTransientePoisson()
 │    │    │    └── Sub-cycling loop: transientePoissonDummy() until wall heat flux converges
 │    │    ├── Set RGOMax = 14000 (black-oil dead-oil safety bound)
 │    │    └── Time-stepping loop (while lixo5 < tfinal):
 │    │         ├── SolveTrans() → advance one time step
 │    │         ├── WriteSnapShot() at user-specified times (tempsnp[])
 │    │         └── WriteSnapShot() → final snapshot after loop exits
 │
 ├── ELSE IF tipoSimulacao == CONVECNAT
 │    └── solv2D::resolve() → 2D natural convection finite-volume solver
 │
 ├── Log INFO "SUCESSO", logger.writeOutputLog()
 ├── arqRelatorioPerfis.close() (skipped if INJETOR; called even for CONVECNAT despite manifest not being opened)
 └── return EXIT_SUCCESS
```

The sections below describe each simulation mode in more detail.

---

## Simulation Modes Explained

### Single Branch (default)

The simplest mode. A single pipeline segment (`SProd` object) is created from the input JSON. Two independent simulation modes are available:

- **Steady-state** — iterates on pressure from downstream to upstream (marching scheme with root-finding on the boundary condition) until convergence. Used for design-point calculations and as the initial condition for transient runs.
- **Transient** — advances the system in time with explicit time-stepping (`SolveTrans` loop), capturing slug flow, start-up/shutdown, and other time-dependent phenomena.

The input JSON selects which class to run (or both sequentially: steady-state first as initialisation, then transient).

### Network Modes

Four network topologies are supported. See [Network Simulation](network-simulation.md) for full details.

| Mode | Entry function | Description |
|------|----------------|-------------|
| Production | `solveRedeProd()` / `RedeProd()` | Tree topology (collectors + affluents). Pre-processing decomposes into sub-networks solved in parallel via OpenMP. Steady-state + optional transient. |
| Injection | `RedeInj()` | Same topology as production but reversed flow direction (surface → wells). Steady-state only. |
| Gas-Lift Loop | `RedeAnelGL()` | Production branches coupled with an annular gas distribution line through drain points. Iterates until gas mass balance converges. |
| Parallel | `RedeParalela()` | Two co-located pipelines (primary + secondary) coupled at shared `fontechk` sources. Alternating solves until pressure convergence. |

### Natural Convection (2D)

A separate 2D finite-volume solver (`solv2D`) for natural convection in confined cross-sections (e.g., pipeline annuli during shutdowns). Not related to the 1D multiphase flow solver.

---

## Global Variables

These variables are declared at file scope and shared across functions in `Num4Main.cpp`:

### Simulation Metadata

| Variable | Type | Purpose |
|----------|------|---------|
| `versao` | `string` | Version string, e.g. `"Marlim - 3.5.0"` |
| `nthrdMatriz` | `int` | Number of threads for matrix operations (default: 1) |
| `tipoSimulacao` | `tipoSimulacao_t` | Current simulation mode (default: `transiente`) |
| `logRede` | `int` | Flag: 1 if running in network mode |
| `nomeRedePrincipal` | `string` | Name of the main network input file |

### Timing

| Variable | Type | Purpose |
|----------|------|---------|
| `nowGlobIni` / `nowGlobFim` | `time_t` | Start / end wall-clock time |
| `diaIni`, `horaIni`, `minutoIni`, `segundoIni` | `int` | Decomposed start time |

### File Paths

| Variable | Type | Purpose |
|----------|------|---------|
| `pathArqEntrada` | `string` | Directory of the input file |
| `pathArqExtEntrada` | `string` | Directory for external inputs (PVTSIM, snapshots) |
| `pathPrefixoArqSaida` | `string` | Output directory with trailing separator |
| `arqSaidaSnapShot` | `string` | Snapshot output filename |
| `diretorioSaida` | `string` | Output directory from `-d` argument |

### I/O Objects

| Variable | Type | Purpose |
|----------|------|---------|
| `arqRelatorioPerfis` | `ofstream` | Profile results report file (`resultado.log`) |
| `logger` | `Logger` | Simulation logger (writes JSON log) |
| `ptrSistemaProducao` | `SProd*` | Pointer to the active production system (used by signal handler for emergency snapshots) |

### Network Globals

| Variable | Type | Purpose |
|----------|------|---------|
| `tramos` | `vector<tramoAtivo>` | List of active network segments |
| `redesAlinhadas` | `vector<tramoAtivo*>` | Aligned/ordered network pointers |
| `redeLeitura` | `int` | Number of internal sub-networks after pre-processing |

### Finite-Volume 2D Solver Globals

| Variable | Type | Purpose |
|----------|------|---------|
| `tempVF` | `detTempo` | Time-stepping parameters for the 2D FV solver |
| `prop` | `detProp` | Material properties for the 2D FV solver |
| `mapprop` | `detMapProp` | Property mapping for the 2D FV solver |
| `CI` | `detCI` | Initial conditions for the 2D FV solver |
| `CC` | `detCC` | Boundary conditions for the 2D FV solver |

### Easter Eggs

`saidaTexto[16]` and `saidaSubTexto[16]` — arrays of humorous quotes (in Portuguese) displayed on simulation completion. A random entry is selected with `rand() % 16` at each run.

---

## Local Data Structures

These `struct` types are defined within `Num4Main.cpp` and used exclusively by functions in this file.

### `fonteposic`

Describes the position of a mass source (injection point) within the network.

```cpp
struct fonteposic {
    int nmani;           // Number of manifolds connected to this source
    int mani[10];        // Indices of connected manifolds (max 10)
    double posicmani;    // Position of the source along the manifold
};
```

### `noRede`

Tracks convergence state at a network node during steady-state iterations.

```cpp
struct noRede {
    int naflu;           // Number of upstream (affluent) branches
    int ncole;           // Number of downstream (collector) branches
    int aflu[40];        // Indices of upstream branches (max 40)
    int cole[40];        // Indices of downstream branches (max 40)
    double normaP1;      // Pressure norm at current iteration
    double normaP0;      // Pressure norm at previous iteration
    double normaMass1;    // Mass norm at current iteration
    double normaMass0;    // Mass norm at previous iteration
    int cadastrado;      // Whether this node has been registered
};
```

### `tramoAtivo`

Describes an active segment in the network topology, including its connections and operational state.

```cpp
struct tramoAtivo {
    int ncoleta;           // Number of downstream collectors
    int nafluente;         // Number of upstream feeders
    int nbloqueio;         // Number of blockages
    int ativo;             // Active flag (1 = active)
    vector<int> coleta;    // Indices of downstream branches
    vector<int> afluente;  // Indices of upstream branches
    int bloqueio;          // Blockage flag
    int permanente;        // Steady-state convergence flag
    int presimposta;       // Whether pressure is imposed at this branch
    double presMon;        // Upstream (montante) pressure
    double presJus;        // Downstream (jusante) pressure
    int reverso;           // Reverse-flow flag
    int derivaPrincipal;   // Main branch flag
    string tramoJson;      // Path to the branch's JSON input file
};
```

### `tramoPart`

Auxiliary structure for tracking which branches participate in a blockage partition.

```cpp
struct tramoPart {
    int ntram;       // Number of branches in this partition
    int part[40];    // Indices of participating branches (max 40)
};
```

### `convergeNoPerm`

Holds mixed/aggregated fluid properties at a network node during steady-state convergence. These are weighted averages of all branches feeding into the node.

```cpp
struct convergeNoPerm {
    double tL, tH;              // Temperature range [low, high]
    double qostdTot;            // Total oil flow rate (std conditions)
    double qgstdTot;            // Total gas flow rate (std conditions)
    double dengmist;            // Mixed gas density
    double yco2mist;            // Mixed CO₂ mole fraction
    double apimist;             // Mixed API gravity
    double qwmist;              // Water flow rate
    double mliqmist;            // Mixed liquid mass flow rate
    double mgasmist;            // Mixed gas mass flow rate
    double cpmist;              // Mixed heat capacity
    double tempmist;            // Mixed temperature
    double qlmistStd;           // Mixed liquid volume rate (std)
    double RGOmist;             // Mixed gas-oil ratio
    double bswmist;             // Mixed BSW (basic sediment & water)
    // ... plus negative-flow counterparts and compositional fields
    ProFlu flu;                 // Resulting mixed fluid properties
};
```

---

## Key Functions

### `main()`

**Signature:** `int main(int argc, char **argv)`

The entry point of the simulator. See [High-Level Execution Flow](#high-level-execution-flow) for the full annotated call tree. Key responsibilities:

1. Records start time (`nowGlobIni`)
2. Parses command-line arguments (`-i`, `-o`, `-s`, `-v`, `-d`, `-p`, `-t`)
3. Registers OS signal handlers (SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM) → `signalHandler()`
4. Configures logger and resolves file paths (`determinarPathArqEntSai`)
5. Opens the output manifest file (`resultado.log`)
6. Dispatches to the appropriate solver based on `tipoSimulacao`:
   - **Network** (`REDE`): reads network JSON → `Rede`, determines sub-type (production / injection / gas-lift / parallel), pre-processes topology, solves sub-networks in parallel with OpenMP
   - **Single branch** (`TRANSIENTE` / `INJETOR`): constructs `SProd`; runs steady-state (`SolveTramoSolteiro` or parametric analysis); runs transient  (initialises cell-level densities/viscosities and runs the `SolveTrans` loop)
   - **Natural convection** (`CONVECNAT`): constructs and runs 2D FV solver (`solv2D::resolve`)
7. Logs "SUCESSO", writes `simulacao.log`, closes `resultado.log`

---

### Single-Branch Solvers

#### Steady-State

##### `permanenteSimples(SProd &sistem1, double inichute)`

Dispatches to the appropriate **busca** (root-finding) method based on the inlet boundary-condition type (`ConContEntrada`) and whether the system is a production or injection well:

| `ConContEntrada` | Condition | Method called |
|------------------|-----------|---------------|
| 0 | Source at inlet (gas, liquid, IPR, porous) — choke inactive | `buscaProdPfundoPerm(inichute)` |
| 0 | Source at inlet — choke active | `buscaProdPfundoPerm2(inichute)` |
| 0 | Source at inlet — reverse flow | `buscaProdPfundoPermRev(inichute)` |
| 1 | Pressure at inlet — choke inactive | `buscaProdPresPresPerm(chutemass)` |
| 1 | Pressure at inlet — choke active | `buscaProdPresPresPerm2(chutemass)` or `buscaProdPresPresPerm3(inichute)` |
| 2 | Pressure + injection flow rate | `buscaProdPfundoPerm3(pres)` |
| — | Injection well (`pocinjec == 1`) | `buscaInjPfundoPerm1..5(inichute)` depending on `condpocinj.CC` |

For `ConContEntrada == 0` with a gas-lift service line (`lingas == 1`) and pressure-BC on the gas side (`tipoCC == 0`), the solver implements a nested retry loop: it first estimates the gas injection flow rate, solves with flow-rate BC, then switches back to pressure BC using the converged bottom-hole pressure as the initial guess. This retry loop runs up to 40 iterations.

##### `SolveTramoSolteiro(SProd &sistem1, double chute0)`

Top-level driver for solving a standalone branch (not part of a network). Implements the **two-pass compositional strategy**:

1. **If compositional** (`flashCompleto == 2`):
   - **Pass 1** — Temporarily switch all cells and accessories (types 1, 2, 3, 10, 15, 16) to `flashCompleto = 0` (black-oil). Call `permanenteSimples()` with the user-supplied initial guess.
   - Extract the converged bottom-hole pressure (or flow rate) as a refined initial guess.
   - Restore `flashCompleto = 2` on all cells/accessories. Set `buscaIni = 0`.
   - (Optional) `preparaTabDin()` if `tabelaDinamica == 1`.
   - **Pass 2** — `permanenteSimples()` with the compositional model and the refined guess.
2. **If black-oil or table** — calls `permanenteSimples()` directly.
3. **Injection-well variant** — same two-pass logic but uses `flashCompleto = 3` for the first pass and injection-specific `busca` methods.

After convergence, writes:

- `imprimeProfile()` / `imprimeProfileG()` → spatial profiles (`PERFISP`, `PERFISG`)
- `resumoPermanente()` → steady-state summary (`resumoPermanente.dat`)
- Poisson 2D prints (`imprimePermanente`) for cells with `difus2D == 1`
- Poroso 2D prints (`imprimePseudo`) for type-16 accessories
- Radial porous profiles (`PerfisPocoRadial`) for type-15 accessories
- `ImprimeTrendPCab/P`, `ImprimeTrendGCab/G` → trend headers and initial data
- `LogEvento.dat` with date, duration, version (when transient is not active)

#### Transient

The single-branch transient simulation is driven by the `SolveTrans()` loop inside `main()` (see [High-Level Execution Flow](#high-level-execution-flow)). `SolveTrans()` is a method on `SProd` ([`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp)) rather than a free function in `Num4Main.cpp`. The transient loop in `main()` initialises cell-level properties, calls `determinaDT()`, and iterates `SolveTrans()` → `WriteSnapShot()` until `lixo5 ≥ tfinal`.

---

### Network Simulation

> Full documentation: [Network Simulation](network-simulation.md)

#### Pre-Processing

> Detailed documentation: [Network Simulation — Pre-Processing Pipeline](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#pre-processing-pipeline)

| Function | Purpose |
|----------|---------|
| `descarteTramo(Rede&, int)` | Scan branches, mark inactive ones, prune connectivity, populate global `tramos` vector |
| `preProcRede(Rede&, int)` | DFS graph decomposition into independent sub-networks; write `RedeInterna-{i}.json` per sub-network |
| `gravaRedeInterna(...)` | Serialize a sub-network to JSON (configuration + re-indexed topology) |
| `preparaRedeProd(...)` | Build SProd objects for a sub-network from branch JSON files |
| `avaliaPerm(...)` | Multi-pass scan to deactivate zero-flow branches (see [avaliaPerm](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#zero-flow-tramo-removal--avaliaperm)) |

#### Steady-State Solvers and Convergence

> Detailed documentation: [Network Simulation — Steady-State Network Solver](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#steady-state-network-solver), [Convergence](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#convergence-and-relaxation), [Node-Level Fluid Mixing](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#node-level-fluid-mixing--totalizaciclorede), [Initial Pressure Guess](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#initial-pressure-guess--chutepresrede)

**Convergence loop and iteration cycles:**

| Function | Purpose |
|----------|---------|
| `convergeRede(...)` | Outer convergence loop: calls `cicloRede`/`cicloRedeComp` until norm < `limConverge` or 200 iterations |
| `cicloRede(...)` | One iteration of the steady-state network solver (black-oil). Topological traversal, branch solves, node mixing, convergence norm |
| `cicloRedeComp(...)` | Compositional variant — molar composition tracking at nodes via `totalizaCicloRedeComp()` |
| `cicloRedeCompCego(...)` | "Blind" compositional cycle — decoupled one-shot pass for initial iterations |
| `cicloRedeInj(...)` | Injection network iteration cycle: topological traversal with injection-specific `busca` methods |

**Node-level mixing and initial guess:**

| Function | Purpose |
|----------|---------|
| `totalizaCicloRede(...)` | Aggregate fluid properties at a junction node (mass/energy-weighted mixing) |
| `totalizaCicloRedeComp(...)` | Compositional variant: molar-weighted composition mixing at nodes |
| `totalizaCicloRedeCompCego(...)` | Simplified molar mixing for blind compositional cycle |
| `chutePresRede(...)` | Recursive hydrostatic initial pressure guess for production networks |
| `chutePresRedeInj(...)` | Recursive hydrostatic initial pressure guess for injection networks |

**Topology variant drivers:**

> Detailed documentation: [Network Simulation — Production](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#production-network--solveredeprod--redeprod), [Gas-Lift](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#gas-lift-loop--redeanelgl), [Parallel](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#parallel-network--redeparalela), [Injection](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#injection-network--redeInj)

| Function | Purpose |
|----------|---------|
| `solveRedeProd(...)` | Production network entry point: `avaliaPerm` → `convergeRede` → output profiles |
| `RedeProd(...)` | Legacy combined driver: constructs SProd objects + solves. Single-branch fallback |
| `RedeInj(...)` | Injection network driver (steady-state only, uses `cicloRedeInj`) |
| `RedeAnelGL(...)` | Gas-lift loop driver: couples production branches with annular gas distribution |
| `RedeParalela(...)` | Parallel network driver: two co-located pipelines with shared `fontechk` sources |

#### Transient Solvers

> Detailed documentation: [Network Simulation — Transient Network Solver](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#transient-network-solver)

| Function | Purpose |
|----------|---------|
| `SolveRedeTrans(...)` | Transient production-network driver: CFL time-stepping, coupling iteration, BC propagation, restart logic, snapshot I/O |
| `CicloRedeTrans(...)` | Single time-step BC propagation: topological traversal, mass-flux collection, fluid mixing at nodes, master/secondary collector assignment |
| `TransAnel(...)` | Transient gas-lift annulus driver |
| `SolveRedeParalelaTrans(...)` | Transient parallel network driver |

---

### Parametric Analysis

#### `leituraAP(string nomeArquivoAP, SProd &sistem1)`

Runs a **sequential parametric analysis**: reads `leituraAP.json`, iterates over parameter combinations, calls `SolveTramoSolteiro()` for each case.

#### `leituraAPparalelo(string nomeArquivoAP, string nomeArquivoLog, tipoValidacaoJson_t validacaoJson, SProd &sistem1)`

**Parallel parametric analysis** using OpenMP. Each parameter case is solved in its own thread with an independent `SProd` copy.

#### `leituraAPparaleloReserva(...)`

Variant of parallel parametric analysis with reservoir-model coupling.

---

<a id="utility--support-functions"></a>
### Utility / Support Functions

#### Snapshot I/O

| Function | Description |
|----------|-------------|
| `WriteSnapShot(SProd&, double, int)` | Writes a snapshot file (`.snp`) of the full simulation state (all cell-level fields) to text |
| `ReadSnapShot(SProd&)` | Reads a snapshot to restart a simulation from a saved state |

#### Signal Handling and CLI

| Function | Description |
|----------|-------------|
| `signalHandler(int)` | Handles OS signals: emergency snapshot, log, exit |
| `showUsage(string)` | Prints the command-line help message |

#### Path Resolution

| Function | Description |
|----------|-------------|
| `determinarPathArqEntSai(...)` | Resolves input/output file paths from the input and log file arguments; sets `pathArqEntrada`, `pathPrefixoArqSaida`, and `arqSaidaSnapShot` |
| `determinarPathArqExtEntrada(string)` | Sets external input path (PVTSIM tables, snapshots) |
| `determinarPathSaida(string)` | Sets the output directory |

#### Flow-Rate Helpers

| Function | Description |
|----------|-------------|
| `FQpesado(Cel*, int)` | Oil flow rate at std conditions (heavy oil / black-oil) |
| `FQleve(Cel*, int)` | Gas flow rate at std conditions (light component) |

#### Network Helper Functions

> Detailed documentation: [Network Simulation — Transient Helper Functions](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#transient-helper-functions)

| Function | Description |
|----------|-------------|
| `celAfluFinal(...)` | Copies master collector's cell[0] into afluent ghost cell |
| `corrigeVazNo(...)` | Corrects unphysical negative mass at network nodes |
| `corrigeVazNoBuf(...)` | Buffered variant of node flow correction |
| `verificaFonteDuplaReversa(...)` | Detects dual sources with opposing flow signs |
| `trocaFonteColetor(...)` | Swaps cell[0] ↔ cell[1] accessories at reversed dual source |
| `retornaFonteColetor(...)` | Restores original source configuration after swap |
| `avaliaBloq(...)` | Evaluates manifold blocking at 2-way junctions |
| `verificaTramoVazPres(...)` | Finds branch with imposed flow/pressure boundary |
| `inativoColetor(...)` | Marks collector branches as inactive |
| `inativoAfluente(...)` | Marks affluent branches as inactive |
| `ranqueiaCol(...)` | Ranks collectors by depth in the network tree |
| `testaBloqueio(...)` | Tests if a branch is blocked |
| `conectaPrincipal(...)` | Couples flux data between primary and secondary branches at shared nodes |
| `chutePresRedeParalelaSec(...)` | Initial pressure guess for secondary branch |
| `calcPeriAnelGL(...)` | Steady-state solve of each well in gas-lift loop |
| `calcErroGL(...)` | Normalised gas mass balance error |
| `objetCC0(...)` | Objective function for annulus inlet pressure matching |
| `zriddr(...)` | Ridder's root-finding method for gas-lift pressure |

#### Compositional Fluid Updates

> Detailed documentation: [Network Simulation — Compositional Model Switching](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#compositional-model-switching), [Cell-Level Composition Propagation](https://github.com/petrobras/marlim3/blob/main/network-simulation.md#cell-level-composition-propagation)

| Function | Description |
|----------|-------------|
| `atualizaComp(SProd&, int)` | Molar mixing at source cells (injection accessories) |
| `atualizaCel(SProd&, int)` | Composition propagation at plain cells (no source) |
| `alteraModoFluidoCompBlack(...)` | Switches all branches/cells from compositional to black-oil |
| `alteraModoFluidoBlackComp(...)` | Restores compositional model on all branches/cells |
| `preparaTabDin(SProd&)` | Builds dynamic PVT tables on a $(P, T)$ grid for fast bilinear interpolation |

#### Warning Messages

| Function | Description |
|----------|-------------|
| `aviso(int i)` | Prints convergence failure warning for branch `i` |
| `aviso2(int i)` | Prints first-iteration convergence failure warning (suggests adjusting `ParametroInicial`) |

---

## Output and Restart I/O

All simulator output is **plain-text** (no binary files). Every output path is prefixed by the global `pathPrefixoArqSaida` (resolved from the `-d` / `-o` arguments, defaulting to `output/`). A manifest file (`resultado.log`) records the name of each generated data file so that downstream tools can discover them.

### Logger — Structured JSON Log

The `Logger` class ([`Log.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Log.h) / [`Log.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Log.cpp)) accumulates events during a run and serialises them to a JSON file via RapidJSON `PrettyWriter`.

| Severity | Enum | Label in JSON |
|----------|------|---------------|
| None | `LOGGER_NONE` | — |
| Informational | `LOGGER_INFO` | `"INFO"` |
| Warning | `LOGGER_AVISO` | `"AVISO"` |
| Failure | `LOGGER_FALHA` | `"FALHA"` |

Event codes are defined as string macros:

| Code | Meaning |
|------|---------|
| `1000` | Unexpected exception |
| `1001` | JSON format validation error |
| `1002` | Schema validation error |
| `1003` | Business rule validation failure |
| `1004` | Business rule validation warning |
| `1100` | Parse process finished (info) |
| `1101` | Generic parse info |
| `1200` | Simulation finished |
| `1300` | Simulation cancelled |

The default output filename is **`simulacao.log`** (macro `ARQUIVO_LOG_PADRAO`). The global `logger` object is created in `Num4Main.cpp` and configured with `changeResultadoSimulacao()` after argument parsing. `writeOutputLog()` produces:

```json
{
  "versao": "Marlim - 3.5.0",
  "resultadoSimulacao": {
    "arquivo": "<input_file>",
    "data": "dd/mm/yyyy",
    "status": "sucesso" | "falha",
    "logs": [
      {
        "log": "INFO" | "AVISO" | "FALHA",
        "codigo": "1xxx",
        "descricao": "<message + complement>",
        "propriedade": "...",
        "causa": "...",
        "linha": 0
      }
    ]
  }
}
```

Convenience methods `write_logs_and_exit()` and `log_write_logs_and_exit()` combine logging with immediate program termination.

### Snapshot Files (`.snp`) — Restart Checkpoints

`WriteSnapShot()` serialises the **complete simulation state** to a text file so that `ReadSnapShot()` can restore it for a restart run (triggered by `perm == 2`).

**Filename pattern:**

| Context | Pattern |
|---------|---------|
| Single branchh | `Saida{N}-{inputName}.snp` |
| Network branch | `Tramo{T}-Saida{N}-{inputName}.snp` |

where `N = round(porc)` is the snapshot index (percentage or sequence number).

**Format:** ASCII text, semicolon-delimited (` ; `), 12-digit precision. A decorative header with a random motivational quote precedes the data. Records are located by marker strings (`"->"`).

**Data layout (standard mode, `HISEP == 0`):**

1. **System-level record** — marker `"variavel de Sistema de Escoamento= ... ->"`:  
   `presfim`, `pGSup`, `masChkSup`, `temperatura`, `ncelGas`, `presiniG`, `tempiniG`, `massfonte`, `mult`, moving-average accumulators (`presMedMov`, `jMedMov`, `alfMedMov`, `tMedMov`), choke state (`aberto`, `tempoaberto`), intermittency totals, variable-length vectors (`presVet`, `jVet`, `alfVet`, `tVet`), intermittency cell data, buffered source terms.

2. **Production cells** — one line per cell, marker `"Celula de producao= {i} ->"`:  
   134+ fields per cell covering temperatures, pressures (including buffer/ini variants), mass fluxes (L/C/R + buffers), void fractions, water cut, drift-flux parameters (`c0`, `ud`), mass-transfer terms, BCS pump state, flow-pattern indicators, flow rates, densities, PIG state (position, velocity, void fractions), choke derivatives, riser-gas fractions, fluid properties (`API`, `RGO`, `Deng`, `BSW`, `yco2`, viscosities, emulsion data), thermal layer state (`calor.*`: internal velocities, temperatures, convection coefficients, wall-temperature array `Tcamada[j][k]`).  
   Conditional blocks: **paraffin** data (if `modoParafina == 1`), **compositional** data (if `flashCompleto == 2`: `npseudo`, `fracMol[]`, liquid/vapour compositions, beta, bubble-point, stock-tank densities).

3. **Gas-lift service cells** (if `lingas > 0`) — ~40 fields per gas cell including pressures, velocities, thermal data.

4. **Footer** — simulation time `lixo5`.

**HISEP mode** (`HISEP == 1`): writes a simplified CSV table (`t ; HL ; slipRatio ; Usg ; Usl ; FlowPattern ; cell`) for flow-pattern analysis.

`ReadSnapShot()` mirrors the write order exactly, using `>>` token extraction and consuming `";"` delimiters. After restoring fields it calls `RenovaFluido()` to recompute derived properties (and `atualizaPropComp()` / `atualizaPropCompStandard()` for compositional runs).

**Snapshot triggers:**

| Trigger | Location |
|---------|----------|
| User-specified transient times | `tempsnp[kontasnp]` match during `CicloRedeTrans` / `SolveRedeTrans` |
| End of transient simulation | Final snapshot after the time loop exits |
| OS signal (emergency) | `signalHandler()` — writes checkpoint + log before `exit()` |

### Profile Files — Spatial Distributions

Profile files capture the **spatial distribution** of variables along the pipeline at a given time instant.

| File pattern | Generator | Content |
|--------------|-----------|---------|
| `PERFISP-{t}.dat` | `Ler::imprimeProfile()` | Production-line profile (pressure, temperature, holdup, velocities, densities, flow pattern, etc.) |
| `PERFISG-{t}.dat` | `Ler::imprimeProfileG()` | Gas-lift service-line profile |
| `PERFISTRANSP-{t}-{cell}.dat` | `Ler::imprimeProfileTrans()` | Radial thermal profile (radius, temperature, heat flux) |
| `PerfisPocoRadial-{t}-{pos}.dat` | Num4Main | Radial porous-medium well profile (pressure, flow rates, saturations) |

Network branches prepend `Tramo{T}-R-{nrede}-` to the filename; parametric-analysis runs insert `seqAP-{seq}-`.

Format: text matrix, semicolon-delimited. A header line lists column names with units and an `F`/`C` annotation (face / centre). **Columns are user-configurable** via boolean flags in the input JSON (`profp.*`); the code conditionally includes each column in both the header and data rows.

### Trend Files — Time Series at Fixed Positions

Trend files record the **temporal evolution** of variables at user-selected axial positions during transient simulation.

| File pattern | Generator | Content |
|--------------|-----------|---------|
| `TENDP-{dist}.dat` | `SProd::ImprimeTrendPCab()` + `ImprimeTrendP()` | Production-line trends |
| `TENDG-{dist}.dat` | `SProd::ImprimeTrendGCab()` + `ImprimeTrendG()` | Gas-lift service-line trends |
| `TENDTRANSP-{cel}-{cam}-{disc}.dat` | `SProd::ImprimeTrendTransP()` | Thermal layer trends |

Format: text matrix, semicolon-delimited, `width=20`, `precision=19`. Three comment lines (`# Comprimento`, `# Rotulo`, `# Indice da Celula`) precede the column headers. Data is buffered in `MatTrendP[i]` / `MatTrendG[i]` matrices during time-stepping (filled by `Ler::imprimeTrend()`) and flushed to disk by the `ImprimeTrend*` methods in append mode (`ios_base::app`). Columns mirror the profile set and are controlled by the same per-position boolean flags (`trendp[i].*`).

### Steady-State Summary

`Ler::resumoPermanente()` writes **`resumoPermanente.dat`** — a single-row summary of key steady-state results: standard flow rates (oil, oil + water, total liquid, gas), inlet/outlet pressures, choke pressures, gas-lift data, BCS pump performance (ΔP, power, head, in-situ flow rates), and intermittency criteria. Semicolon-delimited with a column-name header.

### Output Manifest and Auxiliary Files

| File | Purpose |
|------|---------|
| **`resultado.log`** | Manifest listing every profile/trend file generated (one filename per line). Opened in `main()`; each `ImprimeTrend*` / `imprimeProfile*` call appends its filename and flushes. |
| **`relatorioSucessoRede.dat`** | Network convergence report: header `# RedeInterna ; # Tramo ; Permanente ; Ativo`, one row per branch indicating success/failure. |
| **`LogEvento.dat`** | Human-readable event log per internal network (date, time, duration, version, motivational quote). Written in append mode. |
| **`RedeInterna-{N}.json`** | JSON topology file for each internal sub-network (produced by `gravaRedeInterna()`). |
| **`sucessoAP.dat`** | Parametric-analysis success/failure report (colon-delimited). |
| **`variaveisInteresseAP.dat`** | Parametric-analysis variables of interest (sequence, status, inlet pressure, outlet temperature). |

---

## Dependencies (Included Headers)

The file includes headers organized into the following groups:

### Core Simulation

| Header | Module |
|--------|--------|
| `SisProd.h` | `SProd` — production system class |
| `LerRede.h` | `Rede` — network reader |
| `Leitura.h` | Input file parsing, enums |
| `LeituraVapor.h` | Vapor/steam input parsing |
| `LerAP.h` | Parametric analysis reader |
| `variaveisGlobais1D.h` | `varGlob1D` — shared simulation state |
| `Log.h` | `Logger` — structured logging |

### Fluid Properties

| Header | Module |
|--------|--------|
| `PropFlu.h` | `ProFlu` — fluid property calculations (black-oil) |
| `PropFluCol.h` | Collector fluid properties |
| `PropFluColVF.h` | Fluid properties for finite-volume solver |
| `PropVapor.h` | Vapor/steam properties |

### Cell & Geometry

| Header | Module |
|--------|--------|
| `celula3.h` | `Cel` — cell (control volume) data structure |
| `celulaGas.h` | Gas-phase cell extensions |
| `celulaVapor.h` | Vapor/steam cell extensions |
| `celRad.h` | Radial cell (porous medium) |
| `celRad-Simples.h` | Simplified radial cell |
| `Geometria.h` | Pipe geometry |
| `GeometriaPoro.h` | Porous media geometry |
| `estruturas.h` | Core data structures |
| `estruturasPoisson3D.h` | 3D Poisson solver data structures |
| `estruturasPoroso.h` | Porous media data structures |
| `estruturaTabDin.h` | Dynamic PVT table data structures |
| `estruturaUNV.h` | UNV mesh data structures (3D coupling) |

### Mesh & 2D Elements

| Header | Module |
|--------|--------|
| `Elem2D.h` | 2D finite-volume element |
| `Elem2DPoroso.h` | 2D porous-medium element |
| `Elem3DPoisson.h` | 3D Poisson element |
| `Malha2D.h` | 2D mesh for heat diffusion |
| `Malha2DPoroso.h` | 2D porous-medium mesh |
| `Malha3DPoisson.h` | 3D Poisson mesh |
| `dados1Poisson.h` | 1D Poisson data structures |
| `dados1Poroso.h` | 1D porous-medium data structures |
| `dados3DPoisson.h` | 3D Poisson data structures |

### Porous Media

| Header | Module |
|--------|--------|
| `PorosoRad.h` | Radial porous-medium model |
| `PorosoRad-Simples.h` | Simplified radial porous-medium model |

### Mass & Energy Sources

| Header | Module |
|--------|--------|
| `FonteMas.h` | Mass source terms |
| `FonteMassCHK.h` | Choke mass source |
| `FonteMasVap.h` | Vapor mass source |
| `TrocaCalor.h` | Heat transfer |

### Artificial Lift

| Header | Module |
|--------|--------|
| `Bcsm2.h` | ESP (BCS) pump model |
| `BombaVol.h` | Volumetric pump model |
| `multiBCS.h` | Multi-stage BCS |

### Solvers

| Header | Module |
|--------|--------|
| `solver.h` | `solv2D` — 2D finite-volume solver |
| `solverPoisson.h` | 2D Poisson (heat diffusion) solver |
| `solver3DPoisson.h` | 3D Poisson solver |
| `solverPoroso.h` | Porous media solver |

### Math

| Header | Module |
|--------|--------|
| `Vetor.h` | `Vcr<T>` — vector template |
| `Matriz.h` | Matrix operations |
| `FerramentasNumericas.h` | Numerical utilities |
| `GradientCorrelations.h` | Pressure-gradient correlations |

### Other

| Header | Module |
|--------|--------|
| `versao.h` | Version string (`VERSAO` macro) |
| `FA_Hidratos.h` | Hydrate formation model |
| `FA_Hidratos_Servico.h` | Hydrate formation — service-line variant |
| `chokegas.h` | Gas choke model |
| `estrat.h` | Flow pattern (stratification) |
| `criterioIntermiSevera.h` | Severe intermittency criterion |
| `Acidentes2.h` | Pipe fittings / singularities |
| `acessorios.h` | Accessories (valves, etc.) |
| `mapa.h` | Output mapping |

---

## Platform Macros

The `BARRA` macro defines the path separator character based on the target OS:

| Platform | `BARRA` Value |
|----------|--------------|
| `linux`, `LINUX`, `Linux`, `UNIX` | `"/"` |
| `WIN32`, `Win32`, `win32` | `"\\"` |
| Other | `"/\\"` |

This macro is used by `determinarPathArqEntSai()` and related functions to construct file paths portably.
