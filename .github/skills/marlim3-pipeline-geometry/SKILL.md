---
name: marlim3-pipeline-geometry
description: Use when building the axial geometry of a Marlim3 simulation — production and service pipe segments, inclination angles or XY coordinates, cell discretization, ambient environment (formation, seawater, atmosphere), initial condition profiles, and production/service thermal coupling.
---

# Marlim3 Pipeline Geometry

## Authoritative files

- [docs/user-guide/pipes.md](../../../docs/user-guide/pipes.md) — segment fields and both examples (**read this**)
- [demos/simplifiedProduction.mr3](../../../demos/simplifiedProduction.mr3) — full well + flowline + riser + service line in XY mode
- [docs/user-guide/cross-sections.md](../../../docs/user-guide/cross-sections.md) — the radial geometry referenced by `crossSectionId`

## Segments — `productionPipe` / `servicePipe` (arrays)

The production line is the main multiphase path (wellbore → flowline → riser); the service line (gas-lift/circulation) requires `initialConfig.gasLine: true`. Per segment:

- `id`, optional `label`, `active`
- `crossSectionId` (PT `idCorte`) → `crossSection[].id`
- `formationId` (PT `idFormacao`) → formation properties id — for buried/downhole segments
- `environment` (PT `ambienteExterno`): `0` user-defined medium, `1` seawater, `2` atmosphere
- `angle` [radians, relative to horizontal] or XY coordinates
- `thermalCoupling` (PT `acoplamentoTermico`): couples this production segment to the service line
- `convectionDirection`: 0 transversal, 1 longitudinal
- discretization + `initialConditions` / `initialAndAmbientConditions` (see below)

## Inclination

**Direct angle mode** (`initialConfig.xyMode: false`): `angle` in radians, measured in the flow direction. Ascending flow = +π/2 (1.5707963), horizontal = 0, descending = −π/2. In the demo, the well and riser (flow going up) use +π/2; the service line (flow going down from platform) uses −π/2.

**XY mode** (`initialConfig.xyMode: true`): give each segment endpoint `xCoor`/`yCoor` plus `numCellsXY`; angles are inferred. Line start coordinates go in `initialConfig`: `xProdStart`/`yProdStart` (and `xServiceStart`/`yServiceStart` for the service line).

`geometryFollowsFlow` (default true): indices increase in flow direction; angles always follow flow for Marlim2 compatibility.

## Discretization

Grouped blocks (default, `grouping: true`):

```json
"discretization": [ { "numCells": 20, "length": 125.0 } ]   // 20 cells × 125 m
```

Explicit cell sizes: `grouping: false` + `cellDx` array. Start coarse (20–50 m cells), refine near valves, pumps, elevation changes, and thermally coupled regions.

## Initial and ambient profiles

Both live under `initialConditions` (PT `condicoesIniciais`) or the fuller alias `initialAndAmbientConditions` (PT `condicoesIniciaisEAmbiente`), with all profiles given at relative positions `measuredPosition` (PT `compInter`) from **0 to 1** along the segment:

- Ambient (needed per `environment`/`formationId`): `ambientTemp` [°C], `ambientVel` [m/s], and for user-defined media `ambientConductivity`, `ambientSpecificHeat`, `ambientDensity`, `ambientVisc`.
- Initial state (only when `initialConfig.transient: true` **and** `initialCondition: 0`): `pressure`, `temp`, `holdup`, `complementaryFluidFraction`, `usl`, `usg`; service line: `gasMassFlowRate`.

Example seabed profile: `{"measuredPosition": [0, 1], "ambientTemp": [4, 4], "ambientVel": [0.3, 0.3]}`. Riser: `ambientTemp` from 4 °C (seabed) to ~20 °C (surface).

## Thermal coupling (annulus ↔ column)

- Set `thermalCoupling: 1` on the production segment (and matching service segment); requires `gasLine: true`.
- Coupled segments must coincide in position, length, and discretization.
- Coupling is a **pipe** setting; `crossSection.annular` only changes hydraulics.

## Validation checklist

- Sum of `numCells × length` per segment = intended physical length; measured lengths of accessories must fall inside the total line length.
- Every segment references an existing cross-section; buried segments reference an existing formation id.
- XY mode: coordinates consistent with intended lengths/angles; `numCellsXY` present per segment.
- Service line present ⇔ `gasLine: true`.
