---
name: marlim3-materials-cross-sections
description: Use when defining pipe wall materials, radial cross-section geometry (diameters, roughness, concentric layers, annular flow), or rock formation thermal properties for a Marlim3 simulation. Covers material types (solid, user fluid, water, air), layer construction, and formation coupling.
---

# Marlim3 Materials, Cross-Sections & Formation

## Authoritative files

- [docs/user-guide/materials.md](../../../docs/user-guide/materials.md) — material types and properties (**read this**)
- [docs/user-guide/cross-sections.md](../../../docs/user-guide/cross-sections.md) — radial geometry and layers (**read this**)
- [docs/user-guide/rock-formation.md](../../../docs/user-guide/rock-formation.md) — formation thermal properties
- [demos/simplifiedProduction.mr3](../../../demos/simplifiedProduction.mr3) — realistic well/flowline/riser cross-sections

## Materials — `material` (top-level array)

Each entry: `id`, optional `label`, `type`, and thermal properties.

| `type` | Meaning | Required properties |
|--------|---------|---------------------|
| 0 | Solid (steel, insulation, cement) | `conductivity` W/(m·°C), `specificHeat` J/(kg·°C), `rho` kg/m³ |
| 1 | User-defined stagnant fluid layer | those above + `visc` (cP) + `beta` (1/K, thermal expansivity) |
| 2 | Water (internal model) | none — only `id`, `type` |
| 3 | Air (internal model) | none — only `id`, `type` |

Typical values: carbon steel 50/500/7800; stainless 15/500/8000; PU foam 0.03/1500/60; polypropylene 0.22/1800/900; cement 0.6/1000/500–2000; concrete coating 1.5/880/2300. Use `type: 2` for completion-fluid annuli. Use aged/wet insulation values, not laboratory ones.

## Cross-sections — `crossSection` (top-level array)

Per entry:

- `id`, optional `label`
- `innerDiameter` [m] — hydraulic inner boundary (for annular flow: smallest annulus diameter)
- `outerDiameter` [m] — only when `annular: true` (largest annulus diameter; must exceed `innerDiameter`)
- `roughness` [m] — 1.5e-6 (polished) to 4.6e-4 (corroded); common default 1.83e-4
- `annular` (default false) — annular hydraulic geometry (tubing-casing production). Changes hydraulics only; thermal coupling is a pipe-segment setting.
- `layers` — concentric layers from inner wall outward:

```json
"layers": [
  { "label": "steel", "materialId": 0, "layerMeasurementType": "THICKNESS",
    "thickness": 0.00635, "discretization": 1 },
  { "label": "insulation", "materialId": 1, "layerMeasurementType": "THICKNESS",
    "thickness": 0.0508, "discretization": 3 }
]
```

- `layerMeasurementType`: `"THICKNESS"` (radial thickness, cumulative outward) or `"DIAMETER"` (outer diameter of the layer; the default when omitted — then use key `diameter`).
- `discretization` (default 1): radial heat-conduction nodes per layer. 1 node for steady state; 2–3 in insulation/cement for transient cooldown accuracy.

Well completion pattern (see demo): tubing steel → completion fluid (type 2) → casing steel → cement. Service annulus pattern: `annular: true` + casing/cement layers.

## Rock formation — `initialConfig.formation`

```json
"initialConfig": { "formation": {
  "productionTime": 365,
  "properties": [ { "id": 0, "conductivity": 2.5, "specificHeat": 850.0, "density": 2500.0 } ]
} }
```

- `productionTime` [**days**]: pre-heating history of surrounding rock — larger = slower early cooldown.
- Pipe segments opt in via `productionPipe[].formationId`; segments without it use ambient environment settings instead.
- Typical rock: k = 1.5–3 W/(m·°C), Cp = 850–1000 J/(kg·°C), ρ = 2400–2600 kg/m³.

## Cross-reference checklist

- Every `layers[].materialId` exists in `material[].id`.
- Every `formationId` exists in `initialConfig.formation.properties[].id`.
- Annular sections: `outerDiameter > innerDiameter`.
- Buried/downhole segments → formation coupling; exposed subsea segments → `environment: 1` (seawater); topside → `environment: 2` (atmosphere).
