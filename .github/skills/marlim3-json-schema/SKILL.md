---
name: marlim3-json-schema
description: Use when creating, reading, or validating a Marlim3 simulation input file (.mr3 / JSON). Covers the top-level structure, the bilingual EN/PT key system, units conventions, ID cross-reference rules, and how to validate against the official JSON schemas. Load this FIRST for any simulation-building task.
---

# Marlim3 JSON Input — Structure, Bilingual Keys & Validation

Marlim3 simulations are defined in a single JSON file (conventionally `.mr3`). The Python API (`marlim3.Branch`) and the C++ engine both consume this format.

## Authoritative files — read these before generating input

| File | What it defines |
|------|-----------------|
| [docs/schema_branch.json](../../../docs/schema_branch.json) | Full JSON Schema, **English keys** (recommended format) |
| [docs/schema_tramo.json](../../../docs/schema_tramo.json) | Full JSON Schema, Portuguese keys |
| [docs/reference/json-schema.md](../../../docs/reference/json-schema.md) | Human-readable top-level catalog + units |
| [marlim3/translations.json](../../../marlim3/translations.json) | EN ↔ PT key/value mapping (single source of truth) |
| [docs/dev-guide/translations.md](../../../docs/dev-guide/translations.md) | How the bilingual system works |
| [demos/simplifiedProduction.mr3](../../../demos/simplifiedProduction.mr3) | Working English-keyed example (well + gas-lift line) |
| `demos/pt-br/*.mr3` | Working Portuguese-keyed examples |

## Language rules

- **Prefer English keys** and include `"language": "en"` at the root — the engine translates EN→PT internally. This is the format `Branch.to_json()` produces.
- Portuguese-keyed files (no `language` marker, or PT keys) are equally valid; never mix languages in one file.
- Enum value translations: `system`: `"PROD"` / `"INJ"` (EN) ↔ `"MULTIFASICO"` / `"INJETOR"` (PT); `layerMeasurementType`: `"THICKNESS"` / `"DIAMETER"` ↔ `"ESPESSURA"` / `"DIAMETRO"`.

## Top-level structure (EN keys)

```json
{
  "language": "en",
  "system": "PROD",
  "initialConfig": {},          // global physics, numerics, formation, inlet BCs
  "productionFluid": [],        // ≥1 required in practice
  "gasFluid": {},               // required when gasLine or dry gas sources
  "complementaryFluid": {},     // optional 3rd liquid phase
  "compTable": {},              // P-T grid for precomputed tables
  "material": [],               // ≥1 required in practice
  "crossSection": [],           // ≥1 required in practice
  "productionPipe": [],         // ≥1 required in practice
  "servicePipe": [],            // required when initialConfig.gasLine = true
  "time": {},                   // required when initialConfig.transient = true
  "ipr": [], "liquidSource": [], "massSource": [], "gasSource": [],
  "pressureSource": [], "porousRadialSource": [], "porous2DSource": [],
  "gasLiftSource": [], "valve": [], "esp": [], "volumetricPump": [],
  "pressureDrop": [], "masterValve": {}, "masterValve2": {}, "pig": [],
  "separator": {}, "gasInj": {}, "surfaceChoke": {}, "injectionChoke": {},
  "injectionWellBC": {},
  "productionProfile": {}, "serviceProfile": {},
  "productionTrend": [], "serviceTrend": [],
  "crossProductionProfile": {}, "crossServiceProfile": {},
  "crossProductionTrend": [], "crossServiceTrend": [],
  "wax": {}, "severeSlugging": {}, "screenConfig": []
}
```

The schema marks nothing as formally required, but a runnable production case needs at minimum: `system`, `productionFluid`, `material`, `crossSection`, `productionPipe`, one inflow (source or inlet BC), one outlet (`separator`), and `productionProfile`.

Do **not** invent `versao`/`versaoJson` fields — they are not part of the current schema.

## Units (strict — no exceptions)

| Quantity | Unit | | Quantity | Unit |
|---|---|---|---|---|
| Length/diameter/roughness | m | | Pressure | kgf/cm² |
| Temperature | °C | | Time | s |
| Angle | **radians** (π/2 = vertical up in flow direction) | | Liquid rate | sm³/d |
| Mass rate | kg/s | | Gas rate | sm³/d |
| Conductivity | W/(m·°C) | | Specific heat | J/(kg·°C) |
| Solid/rock density | kg/m³ | | Fluid density | relative (air=1 gas, water=1 liquid) |
| Viscosity | cP | | ESP: rate BPD, head ft, power hp |
| Gas-lift valve calibration | pressure **psi**, temperature **°F** | | Formation `productionTime` | days |

## ID cross-reference rules (validate every one)

| Reference | Must point to |
|-----------|---------------|
| `productionPipe[].crossSectionId` / `servicePipe[].crossSectionId` | `crossSection[].id` |
| `crossSection[].layers[].materialId` | `material[].id` |
| `productionPipe[].formationId` | `initialConfig.formation.properties[].id` |
| `ipr[].prodFluidId`, `liquidSource[].prodFluidId`, `massSource[].prodFluidId`, `pressureSource[].prodFluidId`, `gasSource[].prodFluidId` (when `dry=false`) | `productionFluid[].id` |
| `initialConfig.initialFluidId` | `productionFluid[].id` |
| Any `measuredLength` / `prodMeasuredLength` / `serviceMeasuredLength` | within total length of the corresponding line |

Additional structural rules:

- IDs unique within each array (convention: sequential from 0).
- Paired schedule arrays must have equal length (`time`/`pressure`, `time`/`opening`, `times`/`maxDT`, ESP `flowRate`/`pumpHead`/`power`/`efficiency`, IPR `*Time` / value pairs).
- All `time` arrays monotonically increasing, starting at 0.
- `measuredPosition` (PT `compInter`) profiles go from 0 to 1.
- `servicePipe`, `gasInj`, `gasLiftSource`, `serviceProfile`, `serviceTrend` all require `initialConfig.gasLine: true`.
- `initialConfig.transient: true` requires `time.finalTime`.

## Validation snippet

```python
import json, jsonschema
schema = json.load(open("docs/schema_branch.json"))   # EN keys; use schema_tramo.json for PT
data = json.load(open("my_case.mr3"))
jsonschema.validate(data, schema)                     # raises on violation
```

`jsonschema` is available via the `gui` dependency group: `uv sync --group gui`.
