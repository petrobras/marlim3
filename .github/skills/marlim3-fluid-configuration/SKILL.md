---
name: marlim3-fluid-configuration
description: Use when defining fluids for a Marlim3 simulation — production fluid (black-oil, flash-table, or compositional), gas fluid for service lines/gas sources, complementary fluid (glycol, brine, inhibitors), viscosity and emulsion correlations, PVT files, and precomputed property tables.
---

# Marlim3 Fluid Configuration

## Authoritative files

- [docs/user-guide/fluids.md](../../../docs/user-guide/fluids.md) — full field catalog and model-applicability tables (**read this**)
- [docs/user-guide/general.md](../../../docs/user-guide/general.md) §6 — fluid model flags in `initialConfig`
- [demos/PVTSIM-MARLIM.tab](../../../demos/PVTSIM-MARLIM.tab) — example PVT table file
- [docs/schema_branch.json](../../../docs/schema_branch.json) — field-level schema (`productionFluid`, `gasFluid`, `complementaryFluid`, `compTable`)

## Choosing the thermodynamic model (flags in `initialConfig`)

| Mode | `flashTableFluidModel` | `compositionalFluidModel` | `pvtFile` | Use for |
|------|------------------------|---------------------------|-----------|---------|
| Black-oil (default) | `false` | `false` | — | fast engineering studies |
| Flash table | `true` | ignored | `.tab` | tabulated PVT from PVTsim/Multiflash |
| Compositional | `false` | `true` | `.ctm` | rich gas / condensate, composition changes |

The PVT file must sit next to the input JSON when the executable runs (tests copy `PVTSIM-MARLIM.tab` into the run directory).

## Production fluid — `productionFluid` (array, ≥1 entry)

Black-oil core fields (per entry): `id`, `api`, `gor` (sm³/sm³), `gasDensity` (air=1), `bsw` (0–1), optional `waterDensity` (water=1), `CO2Fraction`.

Correlation selectors (all optional, sensible defaults):

- `deadOilModel` (PT `modeloOleoMorto`): 0 ASTM (needs `temp1`/`visc1`/`temp2`/`visc2`), 1 Beggs&Robinson, 2 modified B&R, **3 Glaso (default)**, 4 Kartoatmodjo-Schmidt, 5 Petrosky-Farshad, 6 Beal, 7 user table (`deadOilTemp`/`deadOilVisc`).
- `liveOilModel`: **0 Beggs&Robinson (default)**, 1 Kartoatmodjo-Schmidt, 2 Petrosky-Farshad.
- `undersaturatedOilModel`: **0 Vasquez&Beggs (default)**, 1 K-S, 2 P-F, 3 Beal, 4 Khan.
- `RsPbModel`: **0 Vazquez&Beggs (default)**, 1 Lasater, 2 Standing, 3 Glaso, 4 Lívia Fulchignoni (expensive — pair with `initialConfig.RsPbTable: true`).
- `criticalCorrelation`: 0 Marlim2 standard, **1 Brown (default)**, 2 Piper (1/2 better for CO₂-rich gas).
- `emulsionType`: **0 linear (default)**, 1–3 Woelflin weak/medium/strong, 4 exponential (`emulsionCoefA`/`emulsionCoefB`), 5 Pal-Rhodes (`phi100`), 6 user table (`bswVec`/`emulVec`), 7 below-saturation BSW.

Compositional extras: `userMolarFraction: true` + `molarFraction: [...]` (same order as `.ctm` pseudocomponents), `userGORComp: true` to correct composition to match `gor`.

Flash-table overrides: `blackOilViscModel` (1 = use black-oil viscosity correlations instead of table), `blackOilWaterModel` (default 1 = black-oil water JT).

## Gas fluid — `gasFluid` (object; required when `gasLine: true` or dry gas sources)

`gasDensity` (air=1), `CO2Fraction` (default 0), `criticalCorrelation` (1 or 2 only), `useFlashTable` (default false), optional `userMolarFraction`/`molarFraction`.

## Complementary fluid — `complementaryFluid` (optional 3rd liquid phase)

`complementaryFluidType` (PT `tipoF`): 0 generic (provide `density` kg/m³, `compressibility` 1/Pa, `thermalExpansivity` 1/K, `surfaceTension` N/m, `specificHeat`, `conductivity`, viscosity via `temp1`/`visc1`/`temp2`/`visc2`), 1 water-based (only `salinity` g/kg-water), 2 friction-reducer (generic + friction treatment).

## Precomputed property tables — performance for transients

Enable with `initialConfig.pressureTable: true` (production fluids) and/or `gasTable: true` (service gas), then define the grid:

```json
"compTable": { "numPoints": 50, "minPressure": 1.0, "maxPressure": 500.0,
               "minTemperature": 4.0, "maxTemperature": 120.0 }
```

⚠️ The grid must cover the whole operating envelope — extrapolation is unreliable.

## Other `initialConfig` fluid flags

`fluidType` (0 liquid-dominated default, 1 gas-dominated), `cpModel` (0 black-oil / 1 PVT table), `jtlModel` (0/1), `latentHeat` (enthalpy from PVT table, needs `pvtFile`), `trackGOR` (default true), `trackGasDensity` (default true), `freeGasDensityCorrectionBO` (default false), `dynamicTableModel` (steady-state compositional networks), `latentHeatCond` (default true).

## Typical starting values

API 25–32, GOR 100 sm³/sm³, gasDensity 0.7, BSW 0, waterDensity 1.02. Start with one black-oil fluid, validate steady-state, then add complexity.
