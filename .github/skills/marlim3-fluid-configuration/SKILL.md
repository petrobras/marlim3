---
name: marlim3-fluid-configuration
description: Use when the user needs to configure production fluids (oil, gas, water), fluid models (Black Oil, compositional, flash table), PVT properties, emulsion models, or gas properties for a Marlim3 simulation.
---

# Marlim3 Fluid Configuration

## Fluid Modeling Approaches

1. **Black Oil** (default): Correlations based on API, RGO, gas density
2. **Compositional**: Uses PVTSim `.ctm` files with flash calculations
3. **Flash Table**: Uses PVTSim `.tab` files with pre-computed flash tables

Selection via `configuracaoInicial`:
- Default (both false) → Black Oil
- `modeloFluidoTabelaFlash: true` → Flash table
- `modeloFluidoComposicional: true` (with table false) → Compositional

## `fluidosProducao` (array)

Each element defines a production fluid.

### Core Properties

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Unique identifier |
| `ativo` | bool | — | Active flag (default true) |
| `api` | number | °API | API gravity |
| `rgo` | number | Sm³/Sm³ | Gas-Oil Ratio |
| `densidadeGas` | number | — | Relative gas density (to air) |
| `bsw` | number | m³/m³ | Water cut (0-1), default 0 |
| `densidadeAgua` | number | — | Water density relative to pure water, default 1 |

### Dead Oil Viscosity

| Field | Type | Description |
|-------|------|-------------|
| `modeloOleoMorto` | int | 0=Beggs-Robinson (default), 1=user two-point |
| `temp1`, `visc1` | number | Temperature/viscosity point 1 (°C, cP) |
| `temp2`, `visc2` | number | Temperature/viscosity point 2 (°C, cP) |

### Emulsion Models (`tipoEmul`)

| Value | Model |
|-------|-------|
| 0 | Oil-water mixture weighted by water cut (default) |
| 1 | Woelflin — weak |
| 2 | Woelflin — medium |
| 3 | Woelflin — strong |
| 4 | Exponential (uses `coefAModeloExp`, `coefBModeloExp`) |
| 5 | Pal-Rhodes (uses `PHI100`) |
| 6 | User-defined table (`BSWVec`, `emulVec`) |
| 7 | Oil viscosity below saturation BSW |

Additional: `bswCorte` (inversion BSW, default 1.0)

### Compositional Properties

| Field | Type | Description |
|-------|------|-------------|
| `fracCO2` | number | CO2 fraction (default 0) |
| `correlacaoCritica` | int | 1=Brown et al, 2=Piper et al |
| `fracMolarUsuario` | bool | User provides molar fractions |
| `fracMolar` | array | Molar fractions of pseudocomponents |
| `RGOCompUsuario` | bool | Correct fractions to match user RGO |

### Example: Simple Black Oil

```json
"fluidosProducao": [{ "id": 0, "api": 25, "rgo": 100, "densidadeGas": 0.7, "bsw": 0.0 }]
```

### Example: Heavy Oil with Emulsion

```json
"fluidosProducao": [{
  "id": 0, "api": 15, "rgo": 30, "densidadeGas": 0.8, "bsw": 0.5,
  "tipoEmul": 3, "bswCorte": 0.8,
  "modeloOleoMorto": 1, "temp1": 20, "visc1": 300, "temp2": 60, "visc2": 30
}]
```

## `fluidoGas` (object)

Gas used in the service/injection line.

| Field | Type | Description |
|-------|------|-------------|
| `densidadeGas` | number | Gas density relative to air |
| `fracCO2` | number | CO2 fraction (default 0) |
| `correlacaoCritica` | int | 1=Brown, 2=Piper |

```json
"fluidoGas": { "densidadeGas": 0.7 }
```

## `fluidoComplementar` (object)

Custom single-phase fluid with user-defined properties.

| Field | Type | Unit |
|-------|------|------|
| `massaEspecifica` | number | kg/m³ |
| `compP` | number | 1/(kgf/cm²) |
| `compT` | number | 1/°C |
| `tensup` | number | N/m |
| `calorEspecifico` | number | J/(kg·°C) |
| `condutividade` | number | W/(m·°C) |
| `temp1`/`visc1`, `temp2`/`visc2` | number | °C / Pa·s |
| `salinidade` | number | g/kg |

## `configuracaoInicial` Fluid Flags

| Field | Default | Description |
|-------|---------|-------------|
| `modeloFluidoTabelaFlash` | false | Use PVT flash table |
| `modeloFluidoComposicional` | false | Use compositional model |
| `pvtsimArq` | — | PVT file name (.tab or .ctm) |
| `modeloCp` | 0 | Cp model: 0=BO, 1=PVT table |
| `latente` | false | Use PVT table for enthalpy |
| `trackRgo` | false | Track RGO along pipeline |
| `trackDensidadeGas` | false | Track gas density along pipeline |
| `iniFluidoP` | 0 | ID of fluid initially filling system |
| `tipoFluido` | 0 | 0=liquid-dominated, 1=gas-dominated |
