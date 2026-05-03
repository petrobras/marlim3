---
name: marlim3-advanced-settings
description: Use when the user needs to configure advanced numerical parameters, threading, hydrate modeling, wax deposition, severe slugging, 3D thermal diffusion, P/T table generation, or other specialized features in Marlim3.
---

# Marlim3 Advanced Settings

## `configuracaoInicial.Avancado` (object)

Numerical tuning parameters for the solver. Most have sensible defaults — change only when needed.

### Convergence & Stability

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `CriterioMonofasico` | number | 1e-5 | Min void fraction to consider monophasic. Lower = more precise but slower |
| `CriterioCondensacao` | number | 1e-3 | Min void fraction for mass transfer model |
| `CriterioBuscaFalsaCorda` | number | 0.1 | Increment fraction for false chord method in steady-state. Reduce for high-IP wells |
| `taxaDespre` | number | 0.01 | Depressurization rate threshold (kgf/cm²·s) for full vs simplified model |
| `CriterioConvergPerm` | number | 0.001 | Steady-state convergence criterion |
| `AceleraConvergPerm` | bool | true | Accelerated steady-state convergence (recommended) |

### Solver Options

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `MedSimpPresFront` | bool | true | Simplified boundary pressure calculation. false=full marching (less stable) |
| `escorregamentoCelulaContorno` | bool | true | Slip model in last cell. false may cause instability at low pressure |
| `correcaoContracorPerm` | bool | false | Counter-flow temperature correction in SS |
| `corrigeContSep` | bool | true | Correct separator pressure comparison error |
| `estabCol` | bool | false | Simplified model in production column (for smart completions) |

### Full Liquid Density Model

| Field | Type | Description |
|-------|------|-------------|
| `TcorrecaoModComp` | array[number] | Time breakpoints (s) for toggling full model |
| `correcaoModComp` | array[bool] | Whether to use full model at each breakpoint |
| `desligaDeriTransMassDTemp` | bool | Disable mass transfer temperature derivatives |

Example: Always use full model
```json
"TcorrecaoModComp": [0, 50001],
"correcaoModComp": [true, true]
```

### Mass Transfer & Energy

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `limTransMass` | number | 10 | Disable latent heat when mass transfer > this value (kg/s·m) |
| `RelaxaDTChoke` | bool | false | Penalize dt when choke flow oscillates |
| `desligaPenalizaDT` | bool | false | Disable dt penalization |
| `controleDTvalv` | bool | false | Control dt around valves |

### Threading

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `nthrd` | int | 1 | Number of threads for simulation |
| `nthrdMatriz` | int | 1 | Number of threads for matrix solver |

**Recommendation**: Set both to the number of available CPU cores for parallel execution.

```json
"Avancado": { "nthrd": 6, "nthrdMatriz": 6 }
```

### Sonic CFL

| Field | Type | Description |
|-------|------|-------------|
| `Tsonico` | array[number] | Time breakpoints for sonic CFL |
| `sonico` | array[int] | Enable (1) or disable (0) sonic CFL at each breakpoint |

### Dynamic Mini-Tables (Compositional)

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `miniTabDinAtraso` | int | steps | Generate mini-table every N time steps |
| `miniTabDinDp` | number | kgf/cm² | Pressure increment/decrement for mini-table |
| `miniTabDinDt` | number | °C | Temperature increment/decrement for mini-table |

### Area Changes & Coupling

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `mudaArea` | int | 0 | 0=no localized loss, 1=add localized loss at area changes |
| `acopColAnulPermForte` | int | 0 | Number of pseudo-transient steps for thermal coupling near ANM |

## `tabela` (object) — P/T Table Parameters

For pre-computing compressibility tables (`tabP`/`tabG` in `configuracaoInicial`).

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `nPontos` | int | — | Grid points in P and T directions |
| `pressaoMaxima` | number | kgf/cm² | Max pressure |
| `pressaoMinima` | number | kgf/cm² | Min pressure |
| `temperaturaMaxima` | number | °C | Max temperature |
| `temperaturaMinima` | number | °C | Min temperature |

```json
"tabela": {
  "nPontos": 21,
  "pressaoMaxima": 600, "pressaoMinima": 2,
  "temperaturaMaxima": 200, "temperaturaMinima": -100
}
```

## `hidrato` (object) — Hydrate Modeling

Models gas hydrate formation and inhibition.

| Field | Type | Description |
|-------|------|-------------|
| `calculoInterno` | bool | Use internal hydrate equilibrium curve |
| `pressao` | array | User-provided equilibrium pressure curve (kgf/cm²) |
| `temperatura` | array | User-provided equilibrium temperature curve (°C) |
| `modeloHidrato` | object | `tipoHmodel`: hydrate model type |
| `Hammerschmidt` | object | Hammerschmidt inhibitor model |
| `PropFluHidrato` | object | Hydrate fluid properties |
| `ModeloTurner` | object | Turner dissociation kinetics model |

### Hammerschmidt Inhibitor

| Field | Type | Description |
|-------|------|-------------|
| `MMH` | number | Molar mass of hydrate |
| `MMG` | number | Molar mass of gas |
| `MMW` | number | Molar mass of water |
| `Whamm` | number | Hammerschmidt inhibitor weight fraction |
| `inibidor` | string | Inhibitor name |
| `fracFWcarregada` | number | Loaded free water fraction |

## `parafina` (object) — Wax Deposition

Enable with `configuracaoInicial.modoParafina: true`.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `arquivoWax` | string | — | WAX file from PVTSim |
| `usuarioPorosidade` | bool | — | User-defined porosity |
| `porosidade` | number | — | Deposit porosity |
| `usuarioC2C3` | bool | — | User-defined C2/C3 coefficients |
| `c2`, `c3` | number | — | Deposition model coefficients |
| `usuarioDifus` | bool | — | User-defined diffusivity |
| `difus` | number | m²/s | Diffusivity |
| `rugosidade` | number | m | Deposit roughness |
| `alteraViscFlu` | bool | — | Modify fluid viscosity due to wax |
| `multVis` | number | — | Viscosity multiplier |
| `DmultipWax`, `EmultipWax`, `FmultipWax` | number | — | Wax multiplier coefficients |

## `intermitenciaSevera` (array) — Severe Slugging

Models severe slugging in pipeline-riser systems.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `inicioTrechoAcumula` | number | m | Start of accumulation zone |
| `fimTrechoAcumula` | number | m | End of accumulation zone |
| `fimTrechoColuna` | number | m | End of riser column |
| `fracaoVazioPenetracao` | number | — | Void fraction for gas penetration |
| `criterio` | int | — | Severe slugging criterion |

## 3D Thermal Diffusion

Enable with `configuracaoInicial.modoDifus3D: true`.

| Field | Type | Description |
|-------|------|-------------|
| `modoDifus3D` | bool | Enable 3D thermal diffusion |
| `threadP3D` | int | Number of threads for 3D calculation |
| `modoDifus3DJson` | string | JSON file with 3D diffusion configuration |

## Sensitivity Analysis

| Field | Type | Description |
|-------|------|-------------|
| `AS` | bool | Enable sensitivity analysis from `leituraAS.json` |
| `paralelizaAS` | bool | Parallelize sensitivity analysis |

## Example: Full Advanced Configuration

```json
"configuracaoInicial": {
  "transiente": true,
  "condicaoInicial": 1,
  "transferenciaMassa": 0,
  "tabP": true,
  "tabG": true,
  "trackRgo": true,
  "trackDensidadeGas": true,
  "Avancado": {
    "CriterioMonofasico": 1e-4,
    "CriterioCondensacao": 0.001,
    "MedSimpPresFront": true,
    "escorregamentoCelulaContorno": false,
    "TcorrecaoModComp": [0, 50001],
    "correcaoModComp": [true, true],
    "nthrd": 6,
    "nthrdMatriz": 6
  },
  "correlacoesEscorregamento": true,
  "correlacoesPorArranjo": {
    "estratificado": 1,
    "bolhaGolfada": 1,
    "anularChurn": 1
  }
}
```
