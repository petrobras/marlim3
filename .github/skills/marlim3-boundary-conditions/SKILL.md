---
name: marlim3-boundary-conditions
description: Use when the user needs to configure boundary conditions for a Marlim3 simulation, including IPR (reservoir inflow), fluid sources (liquid, gas, mass), separator pressure, gas injection, pressure boundaries, or injection well conditions.
---

# Marlim3 Boundary Conditions & Sources

## `ipr` (array) — Inflow Performance Relationship

The most common way to model reservoir fluid inflow. Defines the relationship between reservoir pressure and flow rate.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Unique identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Measured depth position along production line |
| `tipoIPR` | int | — | 0=linear (Darcy), other=Vogel-type |
| `indFluidoPro` / `indiFluidoPro` | int | — | Reference to `fluidosProducao[].id` |
| `pressaoEstatica` | array | kgf/cm² | Static reservoir pressure (time-dependent) |
| `tempoPressaoEstatica` | array | s | Time points for pressure |
| `temperaturas` | array | °C | Reservoir temperature (time-dependent) |
| `tempoTemperaturas` | array | s | Time points for temperature |
| `ip` | array | m³/d/(kgf/cm²) | Productivity Index (time-dependent) |
| `tempoip` | array | s | Time points for IP |
| `ii` | array | m³/d/(kgf/cm²) | Injectivity Index (time-dependent) |
| `tempoii` | array | s | Time points for II |
| `qMax` | array | m³/d | Maximum flow rate (Vogel IPR, time-dependent) |
| `tempoqMax` | array | s | Time points for qMax |

### Example: Linear IPR

```json
"ipr": [{
  "id": 0, "ativo": true,
  "tipoIPR": 0, "indiFluidoPro": 0,
  "comprimentoMedido": 0.1,
  "tempoPressaoEstatica": [0], "pressaoEstatica": [150],
  "tempoTemperaturas": [0], "temperaturas": [90],
  "tempoip": [0], "ip": [200],
  "tempoii": [0], "ii": [200]
}]
```

## `separador` (object) — Downstream Separator

Defines the outlet pressure boundary condition (platform separator).

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `tempo` | array | s | Time points |
| `pressao` | array | kgf/cm² | Separator pressure at each time |

```json
"separador": { "tempo": [0], "pressao": [10] }
```

## `fonteLiquido` (array) — Liquid Sources

Injects liquid at a specific position in the production line.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `indiFluidoPro` | int | — | Fluid reference |
| `comprimentoMedido` | number | m | Position |
| `tempo` | array | s | Time schedule |
| `vazaoLiquido` | array | m³/d | Liquid flow rate |
| `temperatura` | array | °C | Fluid temperature |
| `beta` | array | — | Complementary fluid fraction |

```json
"fonteLiquido": [{
  "id": 0, "indiFluidoPro": 0, "comprimentoMedido": 0.1,
  "tempo": [0], "vazaoLiquido": [1500], "temperatura": [90], "ativo": true
}]
```

## `fonteGas` (array) — Gas Sources

Injects gas at a specific position.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position |
| `tempo` | array | s | Time schedule |
| `vazaoGas` | array | Sm³/d | Gas flow rate |
| `temperatura` | array | °C | Gas temperature |
| `seco` | bool | — | If true, uses `fluidoGas` properties |
| `indiFluidoPro` | int | — | Fluid reference |

```json
"fonteGas": [{
  "id": 0, "comprimentoMedido": 200,
  "tempo": [0], "vazaoGas": [150000], "temperatura": [40], "ativo": true
}]
```

## `fonteMassa` (array) — Mass Sources

Injects fluid by mass flow rate components.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `tipoTermo` | int | — | Thermal model type |
| `comprimentoMedido` | number | m | Position |
| `tempo` | array | s | Time schedule |
| `temperatura` | array | °C | Temperature |
| `vazaoMassT` | array | kg/s | Total mass flow rate |
| `vazaoMassC` | array | kg/s | Condensate mass flow rate |
| `vazaoMassG` | array | kg/s | Gas mass flow rate |
| `indiFluidoPro` | int | — | Fluid reference |

## `fontePressao` (array) — Pressure Sources

Lateral connection with prescribed pressure.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position |
| `pressao` | number | kgf/cm² | Pressure |
| `temperatura` | number | °C | Temperature |
| `TipoAbertura` | int | — | Opening type |
| `abertura` | array | — | Opening schedule (0-1) |
| `tempo` | array | s | Time schedule |
| `cd` | number | — | Discharge coefficient |
| `tipoFluido` | int | — | Fluid type |
| `indiFluidoPro` | int | — | Fluid reference |
| `ambGas` | bool | — | External ambient is gas |

## `gasInj` (object) — Gas Injection Boundary

Top-of-line gas injection boundary for gas lift systems.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `tipoCC` | int | — | Boundary type: 0=flow rate, 1=pressure+flow |
| `tempo` | array | s | Time schedule |
| `temperatura` | array | °C | Gas temperature |
| `vazaoGas` | array | Sm³/d | Gas injection rate |
| `pressaoInjecao` | array | kgf/cm² | Injection pressure |
| `chuteVazaoInjecao` | bool | — | Initial guess for injection rate |

```json
"gasInj": {
  "tipoCC": 1,
  "tempo": [0], "temperatura": [40], "vazaoGas": [150000]
}
```

## `condicaoPressao` (object) — Upstream Pressure BC

Prescribes pressure at the production line inlet.

| Field | Type | Unit |
|-------|------|------|
| `ativo` | bool | — |
| `tempo` | array | s |
| `pressao` | array | kgf/cm² |
| `temperatura` | array | °C |
| `titulo` | array | — (gas mass fraction) |
| `razaoBeta` | array | — |

## `condicaoVazPres` (object) — Upstream Flow+Pressure BC

Fully determines the inlet (steady-state only). Ignores the outlet BC.

| Field | Type | Unit |
|-------|------|------|
| `ativo` | bool | — |
| `tempo` | array | s |
| `pressao` | array | kgf/cm² |
| `temperatura` | array | °C |
| `VazMass` | array | kg/s |
| `razaoBeta` | array | — |

## `CondicaoContPocInjec` (object) — Injection Well BC

For `sistema: "INJETOR"`.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `tipoFluido` | int | — | Injected fluid type |
| `salinidade` | number | g/kg | Salinity |
| `condContorno` | int | — | BC type |
| `tempInj` | number | °C | Injection temperature |
| `vazLiq` | number | m³/d | Liquid flow rate |
| `presInjec` | number | kgf/cm² | Injection pressure |
| `presFundo` | number | kgf/cm² | Bottomhole pressure |
| `arquivoPvtsim` | string | — | PVT file for injected fluid |
