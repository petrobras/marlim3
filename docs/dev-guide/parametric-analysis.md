# Parametric Analysis

This document describes the **parametric analysis** (Análise Paramétrica — AP) framework in Marlim3. Parametric analysis automates the execution of multiple steady-state simulations over a user-defined parameter space, producing pressure/temperature profiles, VFP tables, and summary reports for each combination.

**Key source files:**

| File | Role |
|------|------|
| [`src/include/LerAP.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerAP.h) | `APara` class definition, parameter structs, `variaveis` activation flags |
| [`src/core/LerAP.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LerAP.cpp) | JSON parsing, sequence generation, parameter selection, output formatting |
| [`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp) | `leituraAP()`, `leituraAPparalelo()`, `leituraAPparaleloReserva()` |

---

## Table of Contents

1. [Overview](#overview)
2. [Activation from Main](#activation-from-main)
3. [JSON Input Format](#json-input-format)
4. [Sweepable Parameters](#sweepable-parameters)
5. [The APara Class](#the-apara-class)
6. [Sequence Generation — Full Factorial](#sequence-generation--full-factorial)
7. [Parameter Selection — selecaoAP](#parameter-selection--selecaoap)
8. [Serial Execution — leituraAP](#serial-execution--leituraap)
9. [Parallel Execution — leituraAPparalelo](#parallel-execution--leituraapparalelo)
10. [VFP Table Generation](#vfp-table-generation)
11. [Output Files](#output-files)
12. [Worked Example](#worked-example)

---

## Overview

Parametric analysis performs a **full factorial sweep** (Cartesian product) over user-specified parameter values. For each combination, the simulator:

1. Substitutes the parameter values into the `SProd` system
2. Solves the steady-state problem via `SolveTramoSolteiro()`
3. Collects results (bottom-hole pressure, temperature, profiles, trends)
4. Reports success or failure

**Scope:** Single tramo (pipeline segment) only — parametric analysis does not operate on networks.

**Output modes:**
- `tipoAP == 0` — standard output: writes profiles, trends, and summary per case
- `tipoAP == 1` — VFP table: builds bottom-hole pressure tables for reservoir simulation coupling

The `vfp` selector controls the output/table convention:
- `vfp == 1` — standard/Eclipse path
- `vfp == 0` — IMEX path
- `vfp == 2` and `vfp == 3` — additional code paths also supported by the implementation

---

## Activation from Main

Parametric analysis is triggered when the tramo JSON sets `AP == 1`. The dispatch in `main()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) is:

```
main()
 └─ tipoSimulacao != rede AND != convecNatural:
     └─ SProd sistem1(...)                         // build system
         └─ arq.perm == 1:                         // steady-state enabled
             ├─ arq.AP == 0 → SolveTramoSolteiro() // single solve
             └─ arq.AP == 1:
                 ├─ arq.paralelAP == 0 → leituraAP(arquivoAP, sistem1)
                 └─ arq.paralelAP == 1 → leituraAPparalelo(arquivoAP, ...)
```

The AP input file path is taken from `sistem1.arq.arquivoAP`, and `main()` prepends the input-directory path before calling `leituraAP()` or `leituraAPparalelo()`.

---

## JSON Input Format

The parametric analysis JSON file defines which parameters to sweep and their explicit value lists. Example:

```json
{
  "nthread": 4,
  "tipoAP": 0,
  "vfp": 1,

  "IPR": [
    {
      "indiceIPR": 0,
      "presEstatica": [240, 250, 260, 255]
    },
    {
      "indiceIPR": 1,
      "IP": [20, 30, 15]
    }
  ],

  "psep": {
    "pressao": [25, 10, 30]
  },

  "GasLift": {
    "vazGas": [150000.0, 100000, 80000, 200000]
  },

  "choke": {
    "abertura": [0.2, 0.8, 0.1]
  }
}
```

### Global settings

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `nthread` | int | 1 | Number of OpenMP threads for parallel mode |
| `tipoAP` | int | 0 | 0 = standard output, 1 = build VFP tables |
| `vfp` | int | 1 | VFP selector. The code explicitly supports `0`, `1`, `2`, and `3` |
| `imprimePerfil` | bool | `true` | Enables/disables per-case profile output in standard mode |

### Parameter sections

Each section corresponds to a component type. Only sections present in the JSON are activated. Values are given as **explicit arrays** — not min/max/step ranges. Each array element defines one level for that sub-parameter.

The parser uses RapidJSON directly and reads the schema filename constant `ARQUIVO_SCHEMA_AS_JSON`, defined as `schema_AS_1_0_0.json` in `LerAP.h`.

---

## Sweepable Parameters

The following table lists parameter types that can be swept. Each type maps to a detail struct in `APara` and a `variaveis` activation flag.

| JSON Section | Activation Flag | Detail Struct | Sweepable Sub-Parameters |
|-------------|----------------|---------------|-------------------------|
| `"IPR"` | `vipr` | `detIPRAP` | `presEstatica`, `tempRes`, `IP`, `II`, `qMax`, `fluido` |
| `"GasLift"` | `vgasinj` | `detGASINJAP` | `temperatura`, `presInj`, `vazGas` |
| `"psep"` | `vpsep` | `detPSEPAP` | `pressao` |
| `"RGO-fluido0"` | `vRGO` | `detRGOAP` | `RGO` |
| `"BSW-fluido0"` | `vBSW` | `detBSWAP` | `BSW` |
| `"pentrada"` | `vpresent` | `detPresEntAP` | `pressao`, `temperatura`, `titulo`, `beta` |
| `"vazpentrada"` | `vvazpresent` | `detVazPresEntAP` | `pressao`, `temperatura`, `vazMass`, `beta` |
| `"FonteLiquido"` | `vfonliq` | `detFONLIQAP` | `temperatura`, `beta`, `vazLiq`, `fluido` |
| `"FonteGas"` | `vfongas` | `detFONGASAP` | `temperatura`, `vazGas`, `vazComp` |
| `"FonteMassa"` | `vfonmas` | `detFONMASSAP` | `temperatura`, `vazaoProd`, `vazaoComp`, `vazaoGas`, `fluido` |
| `"BCS"` | `vbcs` | `detBCSAP` | `frequencia`, `estagio` |
| `"MultiBCS"` | `vmbcs` | `detBCSAP` / `APMBCS` storage | `frequencia` |
| `"BVol"` | `vbvol` | `detBVOLAP` | `frequencia`, `capacidade`, `fatorPoli` |
| `"Valvula"` | `vvalv` | `detValvAP` | `abertura`, `CD` |
| `"Vazamento"` | `vfuro` | `detFUROAP` | `pressao`, `temperatura`, `beta`, `CD`, `abertura`, `fluido` |
| `"DP"` | `vdp` | `detDPREQAP` | `deltapressao` |
| `"dPdLHidro"` | `vdpH` | `detDPHidro` | `fator` |
| `"dPdLFric"` | `vdpF` | `detDPFric` | `fator` |
| `"dTdL"` | `vdt` | `detDT` | `fator` |
| `"choke"` | `vchk` | `detCHOKESUPAP` | `abertura`, `CD` |
| `"secaoTransversal"` | `diam` | `detDiamRug` | `DiamIntMaior`, `DiamIntMenor`, `Rugosidade` |
| `"Condutividade"` | `kequiv` | `detCondEquiv` | `condutividade` |
| `"pocoInjetor"` | `vpocinj` | `detCondConInjecAP` | `pressaoInj`, `pressaoFinal`, `temperatura`, `vazao` |

**Multi-instance parameters:** `IPR`, `FonteLiquido`, `FonteGas`, `FonteMassa`, `BCS`, `MultiBCS`, `BVol`, `DP`, `dPdLHidro`, `dPdLFric`, `dTdL`, `Valvula`, `Vazamento`, `secaoTransversal`, and `Condutividade` are JSON arrays. Each element targets a specific accessory by its `indice*` field.

**Single-instance parameters:** `psep`, `RGO-fluido0`, `BSW-fluido0`, `GasLift`, `choke`, `pentrada`, `vazpentrada`, and `pocoInjetor` are single objects.

---

## The APara Class

The `APara` class ([`LerAP.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerAP.h)) manages the entire parametric-analysis lifecycle.

### Construction

```cpp
APara analisePara(vg1dSP, arquivoAP, ncel, celp, flup, bcs, multiBcs, fonteg);
```

The constructor calls `lerArq()`, which:

1. Parses the JSON input with `parseEntrada()`
2. Initializes global flags and defaults (`dim`, `nthrdAP`, `nVariaveis`, `tipoAP`, `vfp`)
3. Calls the individual `parse_*()` methods for active sections
4. Populates `varSeq[23]`
5. Allocates `sequenciaAP`, `genericoAP`, and `vecParSerie` when `nVariaveis > 1`
6. Builds the sweep dimensions with `constroiVecParSerie()` or `constroiVecParSerieImex()`
7. Generates the generic sequence via `inicializaSequen()`
8. Converts generic indices to typed indices via `traduzSeq()` or `traduzSeqImex()`
9. Allocates `saidaBHP` and `saidaVazLiq` when `tipoAP == 1`

### Key members

| Member | Type | Description |
|--------|------|-------------|
| `listaV` | `variaveis` | Activation flags — one `int` per parameter type |
| `dim` | `int` | Total number of independent dimensions in the sweep |
| `nVariaveis` | `int` | Total number of cases |
| `sequenciaAP` | `casoVEC*` | Typed per-case index storage |
| `genericoAP` | `genericoVEC*` | Generic Cartesian-product index tuples |
| `vecParSerie` | `int*` | Number of levels per dimension |
| `varSeq[23]` | `int[23]` | Activation-order array for the 23 top-level parameter groups |
| `tipoAP` | `int` | Output mode |
| `vfp` | `int` | VFP selector |
| `nthrdAP` | `int` | Thread count for parallel execution |
| `saidaBHP` | `double**` | BHP output table storage when `tipoAP == 1` |
| `saidaVazLiq` | `double**` | Liquid-rate table storage when `tipoAP == 1` |
| `imprimePerfil` | `int` | Controls whether standard profile files are emitted |

---

<a id="sequence-generation--full-factorial"></a>
## Sequence Generation — Full Factorial

The sweep generates all combinations using a Cartesian product. The total number of cases is

$$N_{\text{cases}} = \prod_{k=1}^{\text{dim}} n_k$$

where $n_k$ is the number of values for dimension $k$, stored in `vecParSerie[k]`.

### Step 1 — constroiVecParSerie

`constroiVecParSerie()` appends the active series sizes to `vecParSerie` in a fixed internal order. That order is not simply the JSON order; it follows the implementation logic and the `varSeq[23]` activation map.

### Step 2 — inicializaSequen

`inicializaSequen()` builds the full Cartesian-product counter into `genericoAP[seq].generico`.

### Step 3 — traduzSeq

`traduzSeq()` translates each generic tuple into typed indices stored in `sequenciaAP[seq]`, such as `IPRpres`, `PSEPpres`, `BCSfreq`, and others.

---

<a id="parameter-selection--selecaoap"></a>
## Parameter Selection — selecaoAP

`selecaoAP()` and its variants (`selecaoAPsemImpre()`, `selecaoAPImex()`, `selecaoAPImexsemImpre()`) apply a given case to the simulation state.

For each case:

1. The function reads the indices stored in `sequenciaAP[seq]`
2. It retrieves the selected values from the corresponding `AP*` storage structs
3. It updates the appropriate `SProd` inputs and accessory data
4. It optionally writes the chosen values to `relacaoAP.dat`

Additional post-processing is performed by the callers in `Num4Main.cpp`:
- Gas-lift `vazGas` is converted from standard volumetric flow to mass flow for `massfonteCH` when required
- Choke opening is copied to `chokep.abertura[0]`
- Pressure-type inlet BC updates `presE`, `betaE`, `tempE`, `titE`
- Flow-pressure inlet BC recomputes `MassC`, `MassG`, and `MassP`
- Injection-well mode updates `condpocinj.presfundo`, plus temperature/rate fields

Geometry and material changes are handled through helper methods such as `atualizaGeom()`, `atualizaMat()`, `atualizaRGO()`, `atualizaBSW()`, and `atualizaCorrecao()`.

---

<a id="serial-execution--leituraap"></a>
## Serial Execution — leituraAP

`leituraAP()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) runs cases sequentially:

```
leituraAP(nomeArquivoAP, sistem1):
│
├─ Construct APara → parse JSON and generate sequences
├─ Create "sucessoAP.dat"
│
└─ FOR iSeq = 0 to nVariaveis-1:
    ├─ selecaoAP(...) or selecaoAPImex(...)
    ├─ Post-process gas lift, choke, and inlet BC data
    ├─ Choose initial guess:
    │   ├─ first case / flow-pressure inlet / injector-well mode → no guess
    │   └─ otherwise use previous solution state
    ├─ SolveTramoSolteiro(sistem1, chute)
    ├─ if the solve with `chute` fails, retry without it
    ├─ append success/failure to `sucessoAP.dat`
    └─ either write standard outputs or populate VFP tables
```

In serial mode, the previous converged solution is reused as the initial guess whenever the boundary-condition mode allows it.

---

<a id="parallel-execution--leituraapparalelo"></a>
## Parallel Execution — leituraAPparalelo

`leituraAPparalelo()` ([`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp)) uses **OpenMP** to solve cases concurrently.

### Pre-parallel phase

1. Construct `APara`
2. Write headers through `cabecalhoAP()` or `cabecalhoAPImex()`
3. Create `sucessoAP.dat`
4. Emit trend headers
5. Allocate per-case copies of `Ler` and `varGlob1D`
6. Solve the base case once and, if successful, derive a common initial guess `chute`

### Parallel phase

Each iteration:
- Creates a thread-local `SProd` copy with `copiaSemJson(...)`
- Applies the selected AP case via `selecaoAPsemImpre()` or `selecaoAPImexsemImpre()`
- Performs the same post-processing as serial mode
- Tries `SolveTramoSolteiro(sistem2, chute)` first when a common initial guess is available
- If needed, retries with fallback guesses from neighboring solved AP cases (`iSeq-1`, `iSeq+1`, `iSeq-2`, and then another solved case)
- Stores success/failure plus summary data
- Writes per-case profile/trend outputs when `tipoAP == 0` and `imprimePerfil == 1`

### Post-parallel phase

After the OpenMP loop, the function:
1. Sorts collected results by AP index
2. Replays `selecaoAP()` / `selecaoAPImex()` with `imprime = 1` to write `relacaoAP.dat`
3. Writes `variaveisInteresseAP.dat`
4. Writes final success/failure and timing data to `sucessoAP.dat`
5. Populates VFP tables when `tipoAP != 0`

### Backup variant — leituraAPparaleloReserva

`leituraAPparaleloReserva()` is a heavier variant that constructs a full `SProd` object for each case before the parallel loop. It is more memory-intensive but isolates each case more aggressively.

---

## VFP Table Generation

When `tipoAP == 1`, the framework builds **Vertical Flow Performance (VFP) tables**.

### Standard path (`vfp == 1` or `vfp == 3`)

The code uses the standard selection/header path:
- `cabecalhoAP()`
- `selecaoAP()` / `selecaoAPsemImpre()`
- `imprimeVarInteresseAP()`
- `tabelaGenerica()`

### IMEX path (`vfp == 0` or `vfp == 2`)

The code uses the IMEX selection/header path:
- `cabecalhoAPImex()`
- `selecaoAPImex()` / `selecaoAPImexsemImpre()`
- `imprimeVarInteresseAPImex()`
- `tabelaGenerica()`

The resulting table dimensions follow the active AP dimensions. `saidaBHP` is allocated as `[nVariaveis][dim + 1]`, while `saidaVazLiq` is allocated as `[nVariaveis][APFonLiq[0].parserieVL]` when table generation is enabled.

### Outlet-temperature companion tables

Every `tipoAP == 1` run automatically writes an outlet-temperature table alongside
the BHP table, with no additional simulations or input options:

| `vfp` | BHP output | Temperature output | Temperature designation |
|-------|------------|--------------------|-------------------------|
| 0 | `bhpsIMEX.imx` | `whtsIMEX.imx` | `WHT` |
| 1 | `bhpsEclipse.ecp` | `thtsEclipse.ecp` | `THT` |
| 2 | `bhpsIMEXnew.imx` | `whtsIMEXnew.imx` | `WHT` |
| 3 | `bhpsEclipsenew.ecp` | `thtsEclipsenew.ecp` | `THT` |

Temperature is in degrees Celsius at the outlet-pressure boundary represented
by the `THP`/`WHP` axis (`psep`). When there is a surface-choke pressure drop,
the existing steady-state choke correction gives the downstream temperature;
otherwise the last-cell temperature is used. Both Eclipse modes declare
`METRIC`, including mode 3, which previously incorrectly declared `FIELD`.
The mode-0 temperature keyword is `*WHT`, rather than a `WHTTO` substitution
of the legacy pressure keyword `*BHPTO`.

The paired tables share axes, case indices, row grouping, and precision.
Eclipse rows group liquid rates; IMEX rows group separator pressures. Failed
cases retain their grid positions with `-1e10` in both tables. Serial and
parallel AP use the same table writer. Existing BHP outputs (apart from the
mode-3 unit-header correction), generic table columns, and AP summary
temperature definitions are unchanged.

---

## Output Files

### sucessoAP.dat

Reports success/failure for each case with execution timing:

```
relatório de falhas da Analise Parametrica para um Tramo
0 :  Resultado = sucesso
1 :  Resultado = sucesso
2 :  Resultado = falha
```

### variaveisInteresseAP.dat

Semicolon-separated table with one row per case. It is written by both serial and parallel AP workflows.

```
indice AP ; Sucesso ; Pressao Fundo ; Temperatura Plataforma ; <param1> ; <param2> ; ...
0 ; 1 ; 312.45 ; 42.3 ; 240 ; 25 ; ...
1 ; 1 ; 308.12 ; 41.8 ; 250 ; 25 ; ...
2 ; -1 ; -1e10 ; -1e10 ; 260 ; 25 ; ...
```

The header is generated by `cabecalhoAP()` or `cabecalhoAPImex()`, depending on the active `vfp` mode.

### relacaoAP.dat

Detailed parameter log per case, written during `selecaoAP()` / `selecaoAPImex()` when `imprime = 1`.

### Per-case output files

For `tipoAP == 0`, each case may generate the standard simulation outputs:

| Output | Content |
|--------|---------|
| Pressure/temperature profile | Cell-by-cell pressure, temperature, holdup |
| Steady-state summary | BHP, rates, GOR, water cut |
| Trend data | Variables at monitoring cells |
| Gas line profile | If `lingas == 1` |
| Poisson 2D | If `difus2D == 1` |

File naming includes the AP sequence index, for example `_AP_0.log`, `_AP_1.log`, and so on.

---

## Worked Example

Given the example JSON:

```json
{
  "tipoAP": 0,
  "IPR": [
    {"indiceIPR": 0, "presEstatica": [240, 250, 260, 255]},
    {"indiceIPR": 1, "IP": [20, 30, 15]}
  ],
  "psep": {"pressao": [25, 10, 30]},
  "GasLift": {"vazGas": [150000, 100000, 80000, 200000]},
  "choke": {"abertura": [0.2, 0.8, 0.1]}
}
```

**Activated flags:** `vipr = 1`, `vpsep = 1`, `vgasinj = 1`, `vchk = 1`

**Dimensions:**
- `IPR[0].presEstatica`: 4 values → `parseriePres = 4`
- `IPR[1].IP`: 3 values → `parserieIP = 3`
- `psep.pressao`: 3 values → `parseriePres = 3`
- `GasLift.vazGas`: 4 values → `parserieVazGas = 4`
- `choke.abertura`: 3 values → `parserieAbre = 3`

So:

$$\text{dim} = 5$$

and

$$nVariaveis = 4 \times 3 \times 3 \times 4 \times 3 = 432$$

Because the parser sorts many numeric arrays after reading them, the actual enumeration order follows the sorted values, not necessarily the original JSON order.
