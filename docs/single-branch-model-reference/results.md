# Results

Output configuration in Marlim3 is organized around four concepts:

- **Line profiles** — variable snapshots versus measured length at selected times.
- **Line trends** — time evolution at selected measured-length points.
- **Cross-section profiles** — radial temperature snapshots at selected times and positions.
- **Cross-section trends** — time evolution at specific radial layers and discretization points.

---

## Output Objects

Top-level output objects:

| Concept | EN key | PT key | Type |
|---------|--------|--------|------|
| Production profile | `productionProfile` | `perfilProducao` | object |
| Service profile | `serviceProfile` | `perfilServico` | object |
| Production trend | `productionTrend` | `tendP` | array |
| Service trend | `serviceTrend` | `tendS` | array |
| Cross production profile | `crossProductionProfile` | `perfisTransP` | object |
| Cross service profile | `crossServiceProfile` | `perfisTransS` | object |
| Cross production trend | `crossProductionTrend` | `tendTransP` | array |
| Cross service trend | `crossServiceTrend` | `tendTransS` | array |
| Screen output | `screenConfig` | `tela` | array |

---

## Profiles (Production/Service)

Profiles are objects with:

- `active` · `ativo` — whether the profile output is enabled
- `time` · `tempo` — array of output instants [s] (valid for transient simulations)
- Variable flags — booleans that request specific physical quantities

For transient simulations, each time in the array triggers one linewise profile write.

### Common Production-Variable Flags

| Quantity | EN key | PT key | Unit | Notes |
|----------|--------|--------|------|-------|
| Pressure (cell center) | `pressure` | `pressao` | kgf/cm² | |
| Pressure (cell left boundary) | — | `pressaoFront` | kgf/cm² | PT only |
| Temperature (cell center) | `temperature` | `temperatura` | °C | |
| Liquid holdup | `holdup` | `holdup` | — | |
| Complementary fluid fraction | — | `bet` | — | vol. fraction of complementary fluid in liquid phase |
| Gas superficial velocity | `usg` | `ugs` | m/s | at cell left boundary |
| Liquid superficial velocity | `usl` | `uls` | m/s | at cell left boundary |
| Gas velocity | — | `ug` | m/s | at cell left boundary |
| Liquid velocity | — | `ul` | m/s | at cell left boundary |
| Flow pattern | — | `arra` | — | -2=annular/churn, -1=stratified, 0=single-phase, 1=bubble, 2=slug |
| Friction pressure loss | — | `fric` | Pa/m | |
| Hydrostatic pressure loss | — | `hidro` | Pa/m | |
| Gas in-situ density | — | `rhog` | kg/m³ | |
| Liquid in-situ density | — | `rhol` | kg/m³ | |

Many additional flags are available in the schema for diagnostics and specialized workflows: mixture density (`rhoMix`), conductivities (`condliq`, `condoleo`, `condgas`), solubility ratio (`RS`), mass flow rates, drift-flux coefficients (`c0`, `ud`), GOR (`RGO`), gas density (`deng`), CO₂ fraction (`yco2`), heat flux (`calor`), mass transfer (`masstrans`), Cp values, standard volumetric flow rates, API, BSW, Reynolds/Nusselt/Prandtl/Grashof numbers, formation volume factor (`Bo`), pseudocomponent fractions (compositional models), wax data, and more.

### Service-Variable Flags

Service profiles support a smaller set: `pressao`, `temperatura`, `ugs`, `ug`, `tensaoCisalhamento` (shear stress), `viscosidadeGas`, `rhog`, `vazaoMassicaGas`, `QGstd`, `fric`, `calor`, `hidro`, plus Reynolds/Nusselt/Prandtl/Grashof numbers, heat coefficients, ambient temperature, angle, internal diameter, and wall temperature.

---

## Trends (Production/Service)

Unlike profiles, trends are **arrays of point objects**, not a single object.

Each trend item contains:

- `active` · `ativo` — whether this point is active
- `measuredLength` · `comprimentoMedido` [m] — position along the line
- `dt` · `dt` [s] — time interval between recordings
- `label` · `rotulo` — human-readable label for this point
- Variable flags — same boolean flags available in profiles

One item defines one output monitoring point along the line.

**Parser behavior:**

- In transient mode, `dt` must be positive for active trend items.
- Position is mapped to internal cell index and validated against mesh bounds.

---

## Cross-Section Outputs

### Cross Profiles

- `crossProductionProfile` / `crossServiceProfile` are **objects** that record radial temperature profiles at specific cross sections.
- They use:
  - `time` · `tempo` — array of instants [s] at which radial snapshots are taken
  - `measuredLength` · `comprimentoMedido` — array of positions [m] along the line

### Cross Trends

- `crossProductionTrend` / `crossServiceTrend` are **arrays** of items that record the time evolution of temperature at specific points within a cross-section layer.
- Each item uses:
  - `measuredLength` · `comprimentoMedido` [m] — position along the line
  - `layerIndex` · `camada` — cross-section layer index (integer)
  - `discretization` · `discretizacao` — discretization point within the layer (integer)
  - `dt` · `dt` [s] — time interval between recordings
  - `label` · `rotulo` — human-readable label

This is the recommended mechanism to monitor wall-layer temperatures versus time at specific radial locations.

---

## Screen Output (`screenConfig` / `tela`)

Configures which columns are printed to screen during simulation execution.

> **JSON key:** `screenConfig` (EN) · `tela` (PT) — top-level array

Each item is an object with:

- `active` · `ativo` — whether this screen column is active
- Variable flags (same as trend/profile flags)

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
