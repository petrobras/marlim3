# JSON Schema Reference

Marlim3 uses JSON input files to define simulations. Two schema variants are available, and the Python API supports **bilingual input** (English and Portuguese keys interchangeably).

## Schema Files

| Schema | Language |
|--------|----------|
| [Tramo Schema](../schema_tramo.json) | Portuguese keys |
| [Branch Schema](../schema_branch.json) | English keys |

!!! note
    The Branch schema is the recommended starting point for new simulations. It uses English key names and is the format produced by `branch.to_json()` in the Python API.

## Bilingual Support

Marlim3 accepts both English and Portuguese keys at all levels. When the root-level key `"language": "en"` is present, the C++ engine translates English keys to Portuguese internally. The Python API always stores data in English and can export in either language via `to_json(language='pt')`.

See [Bilingual Support](../dev-guide/translations.md) for the full key-mapping reference.

## Top-Level Structure

```json
{
  "language": "en",
  "system": "PROD",
  "initialConfig": { ... },
  "productionFluid": [ ... ],
  "complementaryFluid": { ... },
  "gasFluid": { ... },
  "compTable": { ... },
  "material": [ ... ],
  "crossSection": [ ... ],
  "productionPipe": [ ... ],
  "servicePipe": [ ... ],
  "liquidSource": [ ... ],
  "massSource": [ ... ],
  "gasSource": [ ... ],
  "pressureSource": [ ... ],
  "porousRadialSource": [ ... ],
  "porous2DSource": [ ... ],
  "gasLiftSource": [ ... ],
  "ipr": [ ... ],
  "valve": [ ... ],
  "esp": [ ... ],
  "volumetricPump": [ ... ],
  "pressureDrop": [ ... ],
  "masterValve": { ... },
  "masterValve2": { ... },
  "pig": [ ... ],
  "separator": { ... },
  "gasInj": { ... },
  "surfaceChoke": { ... },
  "injectionChoke": { ... },
  "injectionWellBC": { ... },
  "productionProfile": { ... },
  "serviceProfile": { ... },
  "productionTrend": { ... },
  "serviceTrend": { ... },
  "crossProductionProfile": { ... },
  "crossServiceProfile": { ... },
  "crossProductionTrend": { ... },
  "crossServiceTrend": { ... },
  "screenConfig": { ... },
  "wax": { ... },
  "severeSlugging": { ... },
  "time": { ... }
}
```

## Tramo (PT) vs Branch (EN) Naming

The two schemas represent the same physics but use different key names. This table summarizes the main differences:

| Concept | Tramo (Portuguese) | Branch (English) |
|---------|-------------------|----------------------|
| Root system enum | `"sistema": "MULTIFASICO"` / `"INJETOR"` | `"system": "PROD"` / `"INJ"` |
| Global config | `configuracaoInicial` | `initialConfig` |
| Fluid array | `fluidosProducao` | `productionFluid` |
| Gas fluid | `fluidoGas` | `gasFluid` |
| Production pipe | `dutosProducao` | `productionPipe` |
| Service pipe | `dutosServico` | `servicePipe` |
| Separator | `separador` | `separator` |
| Cross section | `secaoTransversal` | `crossSection` |
| Material | `material` | `material` |
| Formation | `formacao` (inside `configuracaoInicial`) | `formation` (inside `initialConfig`) |

## Key Sections by Category

### Model Definition

| Section | Type | Description |
|---------|------|-------------|
| `system` | string | System type: `PROD` (producer) or `INJ` (injector) |
| `initialConfig` | object | Global settings: physics, correlations, numerics, boundary conditions, and advanced controls |
| `productionFluid` | array | Production fluid definitions (black-oil, compositional, or flash-table) |
| `complementaryFluid` | object | Optional third liquid phase (glycol, inhibitor, brine, completion fluid) |
| `gasFluid` | object | Dry-gas property definition for service line or gas sources |
| `compTable` | object | P-T grid resolution for precomputed property tables |

### Geometry

| Section | Type | Description |
|---------|------|-------------|
| `material` | array | Thermal properties of solid layers (steel, insulation, cement, etc.) |
| `crossSection` | array | Radial geometry: inner diameter, roughness, and concentric layers |
| `productionPipe` | array | Axial production-line segments with inclination, discretization, and thermal environment |
| `servicePipe` | array | Axial service-line segments (when `initialConfig.gasLine = true`) |

### Sources and Inflow

| Section | Type | Description |
|---------|------|-------------|
| `liquidSource` | array | Prescribed liquid volumetric inflow at standard conditions |
| `massSource` | array | Prescribed mass inflow |
| `gasSource` | array | Prescribed gas inflow |
| `pressureSource` | array | Pressure-coupled local exchange point |
| `ipr` | array | Reservoir inflow performance relation |
| `porousRadialSource` | array | Radial porous-media inflow model |
| `porous2DSource` | array | 2D porous-media inflow model |
| `gasLiftSource` | array | Gas-lift injection device/source |

### Accessories and Equipment

| Section | Type | Description |
|---------|------|-------------|
| `valve` | array | Generic two-phase valve (Sachdeva model) at a measured position |
| `esp` | array | Electrical submersible pump (curve-based) |
| `volumetricPump` | array | Positive-displacement pump model |
| `pressureDrop` | array | Prescribed pressure increment/decrement element |
| `masterValve` | object | Production-tree master valve |
| `masterValve2` | object | Secondary master valve (e.g., service tree) |
| `pig` | array | Pig launch/receive events and transport |

### Boundary Conditions

| Section | Type | Description |
|---------|------|-------------|
| `initialConfig.pressureCondition` | object | Inlet pressure-driven boundary (time, pressure, temperature, quality) |
| `initialConfig.flowRatePressureCondition` | object | Inlet pressure + mass flow rate (steady-state oriented) |
| `separator` | object | Downstream (outlet) pressure schedule |
| `surfaceChoke` | object | Surface choke restriction at outlet |
| `gasInj` | object | Gas injection schedule for service line |
| `injectionChoke` | object | Injection-side choke control |
| `injectionWellBC` | object | Injection-well boundary condition combinations |

### Output Configuration

| Section | Type | Description |
|---------|------|-------------|
| `productionProfile` | object | Spatial output along production line (pressure, temperature, holdup, etc.) |
| `serviceProfile` | object | Spatial output along service line |
| `productionTrend` | object | Temporal output at selected production-line positions |
| `serviceTrend` | object | Temporal output at selected service-line positions |
| `crossProductionProfile` | object | Radial/cross-section spatial output |
| `crossServiceProfile` | object | Radial/cross-section spatial output (service line) |
| `crossProductionTrend` | object | Radial/cross-section temporal output |
| `crossServiceTrend` | object | Radial/cross-section temporal output (service line) |
| `screenConfig` | object | Plot display options |

### Advanced Physics

| Section | Type | Description |
|---------|------|-------------|
| `wax` | object | Wax-deposition model configuration |
| `severeSlugging` | object | Severe-slugging detection and mitigation |
| `time` | object | Time-stepping schedule, snapshots, and segregation controls |

## Units Convention

Marlim3 uses the following unit system in JSON inputs:

| Quantity | Unit |
|----------|------|
| Length | m |
| Pressure | kgf/cm² |
| Temperature | °C |
| Time | s |
| Flow rate (liquid) | sm³/d (standard conditions) |
| Flow rate (mass) | kg/s |
| Density (fluid) | relative (air = 1 for gas; water = 1 for liquid) |
| Conductivity | W/(m·°C) |
| Specific heat | J/(kg·°C) |
| Density (solid/rock) | kg/m³ |
| Roughness | m |
| Viscosity | cP |

## Validation

Input files can be validated against the JSON schema:

```python
import json
import jsonschema

with open("schema_branch.json") as f:
    schema = json.load(f)

with open("my_input.json") as f:
    data = json.load(f)

jsonschema.validate(data, schema)
```

!!! tip
    Use the Branch schema (`schema_branch.json`) for validation of English-language inputs. Use `schema_tramo.json` for Portuguese-language inputs.
