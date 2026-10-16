---
name: marlim3-boundary-conditions
description: Use when closing a Marlim3 model with boundary conditions and inflow sources — inlet pressure or flow-rate conditions, separator outlet, IPR reservoir inflow, liquid/mass/gas/pressure sources, service-line gas injection (gasInj), and injection-well boundary modes.
---

# Marlim3 Boundary Conditions & Sources

## Authoritative files

- [docs/user-guide/boundary-conditions.md](../../../docs/user-guide/boundary-conditions.md) — closure strategies and BC objects (**read this**)
- [docs/user-guide/accessories.md](../../../docs/user-guide/accessories.md) — source objects (`ipr`, `liquidSource`, `massSource`, `gasSource`, `pressureSource`)
- [demos/simplifiedProduction.mr3](../../../demos/simplifiedProduction.mr3) — IPR + separator + gasInj working example

## Closure strategy (pick exactly one inlet approach)

1. **Source-driven** (most common for wells): no inlet BC object; fluid enters through `ipr` / `liquidSource` / `massSource` / `gasSource` placed at a `measuredLength`. Line inlet is closed.
2. **Inlet pressure**: `initialConfig.pressureCondition` — schedule of `time`, `pressure`, `temperature`, `fluidQuality` (PT `titulo`), `betaRatio`.
3. **Inlet pressure + mass flow** (steady-state only): `initialConfig.flowRatePressureCondition` — `time`, `pressure`, `temperature`, `massFlowRate` [kg/s], `betaRatio`. Fully determines the system from the inlet.

An outlet is still required for cases 1–2: `separator` (optionally behind `surfaceChoke`).

## Outlet — `separator`

```json
"separator": { "active": true, "time": [0], "pressure": [10.0] }
```

Reverse-flow protection at outlet: `initialConfig.checkValve` (0 = reverse gas inflow allowed [default], 1 = blocked).

## Reservoir inflow — `ipr` (array)

`prodFluidId` → fluid id; `measuredLength` (use a small value like 0.1 for bottomhole at line start); `iprType`: 0 linear, 1 combined Vogel, 2 Vogel. Paired schedules (each `*Time` array matches its value array): `staticPressureTime`/`staticPressure` [kgf/cm²], `temperaturesTime`/`temperatures` [°C], `ipTime`/`ip` (linear PI, sm³/d per kgf/cm²), `qMaxTime`/`qMax` (Vogel), `iiTime`/`ii` (injectivity index — used for reverse flow, keep populated).

## Prescribed sources (arrays; all use `prodFluidId`, `measuredLength`, `time`, `temperature`)

- `liquidSource`: `liquidFlowRate` [sm³/d] standard-condition liquid; associated gas inferred from the fluid. Optional `beta` (complementary-fluid ratio).
- `massSource`: `totalMassFlowRate` [kg/s]; `thermType` 0 = equilibrium gas split, 1 = explicit `gasMassFlow`; `complementaryMassFlowRate`.
- `gasSource`: `gasFlowRate` [sm³/d]; `dry: true` uses `gasFluid`, `dry: false` links `prodFluidId`.
- `pressureSource`: leak/opening coupled to an external pressurized medium — see the valves-choke skill.
- `porousRadialSource` / `porous2DSource`: near-wellbore models delegated to external files (`measuredLength`, `file`).

## Service-line gas injection — `gasInj` (requires `gasLine: true`)

`bcType`: **0 = injection pressure** (`injectionPressures` [kgf/cm²]) or **1 = injection flow rate** (`gasFlowRate` [sm³/d]); plus `time`, `temperature`, optional `initialFlowRateGuess`.

## Injection wells — `system: "INJ"` + `injectionWellBC`

`fluidType`: 0 complementary-fluid liquid, 1 water (needs salinity), 2 CO₂-rich gas via flash table (`.tab`), 3 CO₂-rich gas compositional (`.ctm`); file via `pvtsimFile`. All modes need `injectionTemp`. `boundaryCondition` modes:

| Mode | Requires |
|------|----------|
| 0 | `stdLiquidFlowRate` + IPR |
| 1 | `injectionPressure` + IPR |
| 2 | `bottomholePressure` + IPR |
| 3 | `injectionPressure` + `bottomholePressure` |
| 4 | `stdLiquidFlowRate` + `injectionPressure` |
| 5 | `stdLiquidFlowRate` + `bottomholePressure` |

Injection-side choke: `injectionChoke` (`time`, `opening`, `dischargeCoefficient`). Note: `Branch.simulate()` automatically passes `-s INJETOR` to the executable when `system` is `"INJ"` / `"INJETOR"`.

## Validation checklist

- Exactly one closure strategy; at least one inflow AND one outlet (or a fully-determining inlet BC).
- Every paired time/value array has equal lengths; times monotonic from 0.
- Source `measuredLength` within line length; `prodFluidId` exists.
- Typical sanity: separator 2–15 kgf/cm²; reservoir static pressure 150–350 kgf/cm²; reservoir temperature 60–120 °C.
