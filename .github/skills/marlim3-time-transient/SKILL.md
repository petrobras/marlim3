---
name: marlim3-time-transient
description: Use when the user needs to configure time stepping, transient vs steady-state mode, initial conditions, mass transfer models, slip models, or numerical solver settings for a Marlim3 simulation.
---

# Marlim3 Time & Transient Configuration

## Steady-State vs Transient

Set in `configuracaoInicial`:
- `transiente: false` (default) → Steady-state simulation
- `transiente: true` → Transient simulation (requires `tempo` section)

## `configuracaoInicial` — Simulation Mode Settings

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `transiente` | bool | false | Enable transient mode |
| `condicaoInicial` | int | 1 | Initial condition: 0=user-defined, 1=from steady-state (recommended), 2=from snapshot file, 3=gas-lift unloading |
| `ordemperm` | int | 1 | Steady-state numerical order: 1=first order, 2=Runge-Kutta 2nd |
| `transferenciaMassa` | int | 0 | Mass transfer model (see below) |
| `escorregamentoPermanente` | bool | true | Slip in steady-state solver |
| `escorregamentoTransiente` | bool | true | Slip in transient solver |
| `mapaArranjo` | int | 0 | Flow pattern map: 0=simplified Barnea, 1=full Barnea |
| `tipoModeloDrift` | bool | true | true=drift-flux model, false=black-box correlations |
| `SnapShotArq` | string | — | Snapshot file name (.snp) for condicaoInicial=2 |
| `CheckValve` | int | 0 | 0=allow reverse flow, 1=check valve at outlet |

### Mass Transfer Models (`transferenciaMassa`)

| Value | Description |
|-------|-------------|
| 0 | Full model, implicit method (most stable, default) |
| 1 | Full model, explicit method (same physics, less stable) |
| 2 | Simplified isothermal, no transient mass transfer terms |
| 3 | No mass transfer |

### Initial Condition Options (`condicaoInicial`)

| Value | Description |
|-------|-------------|
| 0 | User-defined initial conditions (in `dutosProducao[].condicoesIniciais`) |
| 1 | From steady-state solution (most common — runs SS first, then transient) |
| 2 | From snapshot file (`.snp`) |
| 3 | Gas-lift unloading initial condition |

## `tempo` (object) — Time Stepping

Controls the transient simulation timeline.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `tempoFinal` | number | s | Final simulation time |
| `tempos` | array | s | Time breakpoints for time-step control |
| `dtmax` | array | s | Maximum time step at each breakpoint |
| `tempoSegrega` | array | s | Segregation time points |
| `segrega` | array | int | Segregation flags |
| `gravaMomento` | array | s | Snapshot save times |

The `tempos[]` and `dtmax[]` arrays work together: between consecutive `tempos[i]` and `tempos[i+1]`, the maximum time step is `dtmax[i]`.

### Example: Simple Transient

```json
"configuracaoInicial": {
  "transiente": true,
  "condicaoInicial": 1,
  "transferenciaMassa": 0
},
"tempo": {
  "tempoFinal": 50001,
  "tempos": [0, 7000, 7005, 7100, 7110],
  "dtmax":  [4,    4,    4,    4,    4]
}
```

### Example: Variable Time Steps (finer around events)

```json
"tempo": {
  "tempoFinal": 50001,
  "tempos": [0, 26000, 26010, 27000, 27010, 32500, 32520, 33020, 33080],
  "dtmax":  [4,     4,     4,     4,     4,     4,     4,     4,     4]
}
```

## Slip Correlations

### `configuracaoInicial.correlacoesEscorregamento` (bool)

If true, enables per-pattern slip correlations.

### `configuracaoInicial.correlacoesPorArranjo` (object)

| Field | Type | Values | Description |
|-------|------|--------|-------------|
| `estratificado` | int | 0,1,2,4 | Stratified pattern: 0=Choi, 1=Bhagwat&Ghajar (default), 2=França&Lahey, 4=B&G modified |
| `bolhaGolfada` | int | 0,1,4 | Bubble/slug: 0=Choi, 1=B&G (default), 4=B&G modified |
| `anularChurn` | int | 0,1,3,4 | Annular/churn: 0=Choi, 1=B&G (default), 3=Hibiki&Ishii, 4=B&G modified |

```json
"configuracaoInicial": {
  "correlacoesEscorregamento": true,
  "correlacoesPorArranjo": {
    "estratificado": 1,
    "bolhaGolfada": 1,
    "anularChurn": 1
  }
}
```

## Gas-Lift Unloading Mode

For `condicaoInicial: 3` (gas-lift unloading):

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `SalinidadeFluido` | number | g/kg | Completion fluid salinity |
| `comprimentoMedidoInterfaceLinhaGas` | number | m | Gas/fluid interface position in service line |
| `comprimentoMedidoInterfaceLinhaProd` | number | m | Gas/fluid interface position in production line |
| `controleDescarga` | bool | — | Automated unloading control |
| `parametrosDescarga` | object | — | Unloading control parameters |

### Unloading Parameters

| Field | Type | Unit |
|-------|------|------|
| `vazaoLimiteDescarga` | number | Sm³/d |
| `pressaoLimiteDescarga` | number | kgf/cm² |
| `pressaoMinimaDescarga` | number | kgf/cm² |
| `pressaoTrabalhoDescargaGas` | number | kgf/cm² |
| `pressaoLimiteDescargaGas` | number | kgf/cm² |
| `pressaoMinimaDescargaGas` | number | kgf/cm² |
| `pressaoInicialDescargaGas` | number | kgf/cm² |
| `temperaturaDescarga` | number | °C |
| `tempoLatencia` | number | s |
