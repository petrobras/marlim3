---
name: marlim3-artificial-lift
description: Use when adding artificial lift to a Marlim3 simulation — gas-lift valves (gasLiftSource) with orifice/calibrated/Venturi types, ESP pumps (esp) with performance curves, volumetric pumps, pressure-drop elements, and gas-lift unloading initialization with discharge control.
---

# Marlim3 Artificial Lift

## Authoritative files

- [docs/user-guide/accessories.md](../../../docs/user-guide/accessories.md) — gas-lift valves, ESP, volumetric pump, pressure drop (**read this**)
- [docs/user-guide/general.md](../../../docs/user-guide/general.md) §12 — gas-lift unloading (`initialCondition: 3`)
- [demos/simplifiedProduction.mr3](../../../demos/simplifiedProduction.mr3) — gas-lift valve example; [demos/extended-ESP-pumpEfic.mr3](../../../demos/extended-ESP-pumpEfic.mr3), [demos/MultiESP.mr3](../../../demos/MultiESP.mr3) — ESP examples
- [docs/tutorials/3-gas_lift.ipynb](../../../docs/tutorials/3-gas_lift.ipynb) — gas-lift optimization workflow

## Gas-lift valve — `gasLiftSource` (array; requires `gasLine: true` + `servicePipe` + `gasInj` + `gasFluid`)

Connects service line to production line:

- Positioning: `prodMeasuredLength` (PT `comprimentoMedidoProducao`) on the production line; `serviceMeasuredLength` on the service line — or `annulusColumnFlag: true` (PT `colunaEanular`) to infer the service tap at the same elevation.
- `valveType`: **0 orifice** (`orificeDiameter` [m]), **1 pressure-calibrated** (`outerDiameter` port + `calibrationPressure` [**psi**] + `calibrationTemperature` [**°F**]), **2 Venturi**.
- Coefficients: `vglDischCoef` (PT `cdvgl`), `liquidDischCoef`, `areaRatio`. (`recoveryFactor` appears in demos and [marlim3/translations.json](../../../marlim3/translations.json) but not in the public schema — treat as engine-internal.)

Typical: orifice 4–8 mm, `areaRatio` ~0.1. Verify both tap positions fall inside their line lengths and are elevation-consistent.

## ESP pump — `esp` (array; PT `bcs`)

- `measuredLength` [m] — pump position on the production line.
- Schedule: `time` + `frequency` [Hz]; `referenceFreq` (curve reference, typically 60), `minFrequency`.
- Performance curves (same length arrays, at `referenceFreq`): `flowRate` [**BPD**], `pumpHead` [**ft**], `power` [**hp**], `efficiency` [0–1].
- Stages: `stage` (installed), `manufacturerStage` (curve basis); `motorEfficiency`, `hiCorrection` (head/viscosity correction).

Affinity laws rescale curves with frequency. Curves must bracket the expected operating flow rate.

## Volumetric pump — `volumetricPump` (array; PT `bombaVolumetrica`)

`measuredLength`, `time` + `frequency`, `capacity`, `polyFactor` (isochoric-compression model).

## Pressure drop / boost — `pressureDrop` (array; PT `deltaPressao`)

Localized Δp schedule; can approximate pumps: `pressureDrop` schedule, `gasCompType` (0 adiabatic, 1 polytropic, 2 isothermal), `polyFacOrAdiabConst`, `liquidEfficiency`, `gasEfficiency`.

## Gas-lift unloading (`initialConfig.initialCondition: 3`)

Simulates displacing completion fluid with lift gas. Required in `initialConfig`:

- `gasLineInterfaceLength` / `prodLineInterfaceLength` [m from platform] — initial gas/completion-fluid interfaces
- `fluidSalinity` [g/kg-water]

Optional automated control: `dischargeControl: true` + `dischargeParameters` with `maxDischargeFlowRate` [sm³/d], `maxDischargePressure`, `minDischargePressure`, `workGasChargePressure`, `maxGasChargePressure`, `minGasChargePressure`, `initialGasDischargePressure` [kgf/cm²], `dischargeTemperature` [°C], `latencyTime` [s].

## Validation checklist

- ESP: curve arrays equal length; `measuredLength` within production line; frequency schedule paired with `time`.
- Gas-lift: service infrastructure complete (`gasLine`, `servicePipe`, `gasInj`, `gasFluid`); calibration units are psi/°F (not kgf/cm²/°C).
- Unloading: interfaces within line lengths; transient mode with adequate `time` schedule.
- One lift method at a time while validating; combine only after baseline runs.
