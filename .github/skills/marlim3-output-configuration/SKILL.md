---
name: marlim3-output-configuration
description: Use when the user needs to configure simulation output in Marlim3, including production/service line profiles, time-series trend monitors, transient snapshots, screen output, or gradient correction factors.
---

# Marlim3 Output Configuration

## `perfilProducao` (object) — Production Line Profiles

Configures which variables are output along the production line at specified times.

### Control Fields

| Field | Type | Description |
|-------|------|-------------|
| `ativo` | bool | Active flag |
| `tempo` | array[number] | Output times in seconds |

### Available Variables (boolean flags)

| Flag | Description |
|------|-------------|
| `pressao` | Pressure (kgf/cm²) |
| `temperatura` | Temperature (°C) |
| `holdup` | Liquid holdup |
| `bet` | Complementary fluid fraction |
| `ugs` | Superficial gas velocity (m/s) |
| `uls` | Superficial liquid velocity (m/s) |
| `ug` | Gas velocity (m/s) |
| `ul` | Liquid velocity (m/s) |
| `arra` | Flow pattern |
| `viscosidadeLiquido` | Liquid viscosity |
| `viscosidadeGas` | Gas viscosity |
| `rhog` | Gas density |
| `rhol` | Liquid density |
| `rhoo` | Oil density |
| `rhoa` | Water density |
| `rhoMix` | Mixture density |
| `vazaoMassicaGas` | Gas mass flow rate (kg/s) |
| `vazaoMassicaLiquido` | Liquid mass flow rate (kg/s) |
| `c0` | Distribution parameter |
| `ud` | Drift velocity (m/s) |
| `RGO` | Gas-Oil Ratio |
| `deng` | Gas density ratio |
| `yco2` | CO2 mole fraction |
| `calor` | Heat transfer |
| `masstrans` | Mass transfer |
| `cpgas` | Gas specific heat |
| `cpliq` | Liquid specific heat |
| `QLstd` | Standard liquid flow rate |
| `QLWstd` | Standard water flow rate |
| `QLstdTotal` | Total standard liquid flow rate |
| `QGstd` | Standard gas flow rate |
| `api` | API gravity along the pipe |
| `bsw` | BSW along the pipe |
| `hidro` | Hydrostatic gradient |
| `fric` | Friction gradient |
| `RS` | Solution gas ratio |
| `Bo` | Oil formation volume factor |
| `Hint` | Internal heat transfer coefficient |
| `Hext` | External heat transfer coefficient |
| `temperaturaAmbiente` | Ambient temperature |
| `angulo` | Pipe angle |
| `diametroInterno` | Internal diameter |
| `tempParede` | Wall temperature |
| `subResfria` | Subcooling |
| `FVH` | Hydrate volume fraction |
| `dadosParafina` | Wax deposition data |

### Example: Basic Profile Output

```json
"perfilProducao": {
  "tempo": [0],
  "pressao": true, "temperatura": true, "holdup": true, "arra": true
}
```

### Example: Comprehensive Transient Profile Output

```json
"perfilProducao": {
  "ativo": true,
  "pressao": true, "temperatura": true, "holdup": true,
  "bet": true, "ugs": true, "uls": true, "arra": true,
  "vazaoMassicaGas": true, "vazaoMassicaLiquido": true,
  "c0": true, "ud": true, "RGO": true, "deng": true,
  "QLstd": true, "QGstd": true, "api": true, "bsw": true,
  "tempo": [0, 1000, 3000, 7000, 10000, 20000, 30000, 40000, 50000]
}
```

## `perfilServico` (object) — Service Line Profiles

Same concept as `perfilProducao` but for the gas service line. Available variables differ (single-phase gas):

| Flag | Description |
|------|-------------|
| `pressao` | Pressure |
| `temperatura` | Temperature |
| `ugs` | Gas superficial velocity |
| `ug` | Gas velocity |
| `tensaoCisalhamento` | Wall shear stress |
| `viscosidadeGas` | Gas viscosity |
| `rhog` | Gas density |
| `vazaoMassicaGas` | Gas mass flow rate |
| `QGstd` | Standard gas flow rate |
| `fric` | Friction gradient |
| `calor` | Heat transfer |
| `hidro` | Hydrostatic gradient |
| `FVHG` | Gas hydrate volume fraction |

```json
"perfilServico": {
  "tempo": [0], "pressao": true, "temperatura": true, "hidro": true, "fric": true
}
```

## `tendP` (array) — Production Line Trend Monitors

Time-series monitors at specific positions along the production line.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Monitor position |
| `dt` | number | s | Sampling interval |
| `rotulo` | string | — | Label for the monitor |

Plus the same boolean variable flags as `perfilProducao` (pressao, temperatura, holdup, etc.)

### Example: Trend Monitors at Key Positions

```json
"tendP": [
  {
    "id": 0, "ativo": true, "comprimentoMedido": 10, "dt": 20,
    "pressao": true, "temperatura": true, "holdup": true, "QLstd": true, "QGstd": true,
    "rotulo": "Fundo"
  },
  {
    "id": 1, "ativo": true, "comprimentoMedido": 2000, "dt": 20,
    "pressao": true, "temperatura": true, "QLstd": true, "QGstd": true,
    "rotulo": "Superficie"
  }
]
```

## `tendS` (array) — Service Line Trend Monitors

Same concept for service line. Available flags: pressao, temperatura, ugs, ug, tensaoCisalhamento, viscosidadeGas, rhog, vazaoMassicaGas, hidro, fric, FVHG, calor, QGstd, plus VGL-specific: presEstagVGL, tempEstagVGL, presGargVGL, tempGargVGL, vazaoVGL.

## `tendTransP` / `tendTransS` (array) — Transverse Temperature Trends

Monitors temperature through the pipe wall layers at a specific position.

| Field | Type | Description |
|-------|------|-------------|
| `ativo` | bool | Active flag |
| `comprimentoMedido` | number | Position (m) |
| `camada` | int | Wall layer index |
| `discretizacao` | int | Radial discretization point |
| `dt` | number | Sampling interval (s) |
| `rotulo` | string | Label |

## `perfisTransP` / `perfisTransS` (object) — Transient Profile Snapshots

Saves full spatial profiles at specific times + specific positions.

| Field | Type | Description |
|-------|------|-------------|
| `ativo` | bool | Active flag |
| `tempo` | array | Snapshot times (s) |
| `comprimentoMedido` | array | Positions to output (m) |

## `tela` (array) — Screen Output

Controls what is displayed on screen during simulation.

| Field | Type | Description |
|-------|------|-------------|
| `ativo` | bool | Active flag |
| `coluna` | bool | true=production, false=service |
| `celula` | int | Cell index |
| `variavel` | int | Variable index (1=pressure, 3=holdup, etc.) |

```json
"tela": [
  { "coluna": true, "celula": 0, "variavel": 1 },
  { "coluna": true, "celula": 249, "variavel": 3 }
]
```

## `correcao` (object) — Gradient Correction Factors

Applies multipliers to pressure and temperature gradients.

| Field | Type | Description |
|-------|------|-------------|
| `ativo` | bool | Active flag |
| `dPdLHidro` | array | Hydrostatic gradient multipliers per duct |
| `dPdLFric` | array | Friction gradient multipliers per duct |
| `dTdL` | array | Temperature gradient multipliers per duct |
