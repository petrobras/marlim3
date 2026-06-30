# Results

Output configuration in Marlim3 is organized around four concepts:

- Line profiles versus measured length at selected times.
- Time trends at selected measured-length points.
- Cross-section radial profiles.
- Cross-section radial trends.

---

## Output Objects

Top-level output objects are:

- `productionProfile` (EN) · `perfilProducao` (PT)
- `serviceProfile` (EN) · `perfilServico` (PT)
- `productionTrend` (EN) · `tendP` (PT)
- `serviceTrend` (EN) · `tendS` (PT)
- `crossProductionProfile` (EN) · `perfisTransP` (PT)
- `crossServiceProfile` (EN) · `perfisTransS` (PT)
- `crossProductionTrend` (EN) · `tendTransP` (PT)
- `crossServiceTrend` (EN) · `tendTransS` (PT)

---

## Profiles (Production/Service)

Profiles are objects with:

- `active` · `ativo`
- `time` · `tempo` (array of output instants)
- Boolean variable flags

For transient simulations, each listed time triggers one linewise profile write.

Common production-variable flags:

| Concept | EN key | PT key |
|--------|--------|--------|
| Pressure | `pressure` | `pressao` |
| Temperature | `temperature` | `temperatura` |
| Holdup | `holdup` | `holdup` |
| Complementary fraction | `complementaryFluidFraction` | `bet` |
| Gas superficial velocity | `usg` | `ugs` |
| Liquid superficial velocity | `usl` | `uls` |
| Gas velocity | `ug` | `ug` |
| Liquid velocity | `ul` | `ul` |
| Flow pattern id | `flowPattern` | `arra` |
| Friction gradient | `frictionPressureGradient` | `fric` |
| Hydrostatic gradient | `hydrostaticPressureGradient` | `hidro` |
| Gas in-situ density | `gasInSituDensity` | `rhog` |
| Liquid in-situ density | `liquidInSituDensity` | `rhol` |

There are many advanced flags in schema for diagnostics and specialized workflows (eigenvalues, pseudocomponent outputs, wax/hydrate diagnostics, etc.).

---

## Trends (Production/Service)

Unlike profiles, trends are **arrays of point objects**, not a single object.

Each item typically contains:

- `active` · `ativo`
- `measuredLength` · `comprimentoMedido`
- `dt` · `dt`
- `label` · `rotulo`
- Variable flags

One item defines one output point along the line.

Important parser behavior:

- In transient mode, `dt` must be positive for active trend items.
- Position is mapped to internal cell index and validated against mesh bounds.

---

## Cross-Section Outputs

### Cross Profiles

- `crossProductionProfile` / `crossServiceProfile` are objects.
- They use `time` arrays and `measuredLength` arrays to request radial snapshots.

### Cross Trends

- `crossProductionTrend` / `crossServiceTrend` are arrays of items.
- Each item uses:
  - `measuredLength` · `comprimentoMedido`
  - `layerIndex` · `camada`
  - `discretization` · `discretizacao`
  - `dt` · `dt`
  - `label` · `rotulo`

This is the right mechanism to monitor wall-layer temperatures versus time at specific radial locations.

---

## Configuration Patterns

### Minimal Profile Setup

```json
{
  "productionProfile": {
    "active": true,
    "time": [0, 1800, 3600],
    "pressure": true,
    "temperature": true,
    "holdup": true,
    "flowPattern": true,
    "frictionPressureGradient": true
  }
}
```

### Production Trend at Two Gauges

```json
{
  "productionTrend": [
    {
      "active": true,
      "measuredLength": 50.0,
      "dt": 5.0,
      "label": "near-inlet",
      "pressure": true,
      "temperature": true,
      "holdup": true
    },
    {
      "active": true,
      "measuredLength": 2500.0,
      "dt": 5.0,
      "label": "deep-gauge",
      "pressure": true,
      "temperature": true,
      "holdup": true
    }
  ]
}
```

### Cross-Section Trend Example

```json
{
  "crossProductionTrend": [
    {
      "active": true,
      "measuredLength": 2500.0,
      "layerIndex": 2,
      "discretization": 1,
      "dt": 10.0,
      "label": "insulation-mid"
    }
  ]
}
```

---

## Interpretation Checklist

1. Compare transient runs against a validated steady-state baseline.
2. Verify output positions against accessory and boundary locations.
3. Use short `dt` in fast-event windows and relaxed `dt` after transients settle.
4. Prefer focused variable sets; enable advanced diagnostics only when needed.
