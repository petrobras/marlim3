---
name: marlim3-output-configuration
description: Use when configuring Marlim3 simulation outputs — spatial profiles along the line (productionProfile/serviceProfile), time trends at gauge points (productionTrend/serviceTrend), radial cross-section outputs, the .dat result files the engine writes, and how results surface as pandas DataFrames.
---

# Marlim3 Output Configuration & Results

## Authoritative files

- [docs/user-guide/results.md](../../../docs/user-guide/results.md) — output objects and variable flags (**read this**)
- [marlim3/_output_headers.py](../../../marlim3/_output_headers.py) — canonical result-column names (EN/PT)
- [marlim3/_tramo/_branch.py](../../../marlim3/_tramo/_branch.py) — `_process_profiles` / `_process_trends` (how `.dat` files become DataFrames)
- [tests/comparison/](../../../tests/comparison/) — reference CSVs showing real output shapes

## Profiles — snapshot along the line (`productionProfile` / `serviceProfile`, objects)

```json
"productionProfile": {
  "active": true,
  "time": [0, 1800, 3600],
  "pressure": true, "temperature": true, "holdup": true,
  "flowPattern": true, "frictionPressureGradient": true,
  "hydrostaticPressureGradient": true, "stdLiqFlowRate": true
}
```

Each listed `time` writes one line-wise profile (steady state: use `[0]`). Common flags: `pressure`, `temperature`, `holdup`, `complementaryFluidFraction`, `usg`/`usl` (superficial velocities), `ug`/`ul`, `flowPattern` (PT `arra`), `frictionPressureGradient` (PT `fric`), `hydrostaticPressureGradient` (PT `hidro`), `gasInSituDensity` (PT `rhog`), `liquidInSituDensity` (PT `rhol`). Many advanced diagnostic flags exist in the schema — enable only what is needed.

## Trends — time series at fixed points (`productionTrend` / `serviceTrend`, **arrays**)

One entry per gauge:

```json
"productionTrend": [
  { "active": true, "measuredLength": 50.0, "dt": 5.0, "label": "near-inlet",
    "pressure": true, "temperature": true, "holdup": true }
]
```

- `dt` [s] must be positive in transient mode; keep it well below event time scales.
- `measuredLength` is mapped to a cell index and validated against the mesh.
- Place gauges at: bottomhole, upstream/downstream of valves and pumps, base of riser, outlet.

## Cross-section (radial) outputs

- `crossProductionProfile` / `crossServiceProfile` (objects): `time` + `measuredLength` arrays → radial snapshots.
- `crossProductionTrend` / `crossServiceTrend` (arrays): `measuredLength`, `layerIndex` (PT `camada`), `discretization` (radial node), `dt`, `label` → wall-layer temperature vs time (cooldown studies).

## Files the engine writes (into the `-d` output directory)

| File pattern | Content |
|--------------|---------|
| `PERFISP*.dat` / `PERFISG*.dat` | production/service profiles (`;`-separated, header on 2nd line) |
| `TENDP-<pos>.dat` / `TENDG-<pos>.dat` | trends per gauge (3 header lines: position, label, cell) |
| `simulacao.log` | JSON run log — on failure, contains `resultadoSimulacao.logs[]` entries with `log: "FALHA"` |
| `LogEvento.dat` | progress log streamed during the run |
| `*.snp` / `*.snt` | snapshots (when `time.saveSnapshot` is set) |

## Results in Python

After `branch.simulate()`, `branch.resultados` holds:

- `resultados['productionProfile']` — DataFrame indexed by (time, cell) with unit-labeled columns like `Pressure (kgf/cm2) C`, `Temperature (C) C` (`C` = cell center, `F` = boundary/face).
- `resultados['productionTrend']` — dict {gauge-number → DataFrame indexed by time} with `.attrs` (`measured_length`, `label`, `cell_index`).
- Service-line equivalents exist when `gasLine: true`.

Plot helpers: `branch.plot_profiles()`, `branch.plot_trends()`, `branch.plot_geometry()`, `marlim3.Scenarios` for multi-case comparison.

## Validation checklist

- At least `productionProfile` configured (results processing expects it).
- `serviceProfile`/`serviceTrend` only when `gasLine: true`.
- Profile `time` values ≤ `time.finalTime`; trend `dt > 0` in transients.
- Trend positions within line length and aligned with the phenomena being studied.
