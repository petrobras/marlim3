# Materials

Materials define the thermal properties of the solid (or fluid) layers that surround the flow area. They are the bridge between geometry and temperature: they determine how fast heat moves through walls, how much energy walls can store, and ultimately how fluid temperature evolves along the system.

> **JSON key:** `material` (EN) · `material` (PT) — top-level array
> Common fields: `active` / `ativo`, `id` / `id`, `label` / `rotulo`

---

## Thermal Conductivity

Conductivity describes how easily heat flows through the material by conduction. High conductivity means the material transmits heat quickly (e.g., steel); low conductivity means the material resists heat flow (e.g., insulation foam).

Conductivity controls the **steady-state temperature gradient** across a layer: higher conductivity → smaller temperature difference across the layer.

> **JSON key:** `conductivity` (EN) · `condutividade` (PT) — unit: W/(m·°C)

## Specific Heat

Specific heat describes how much energy is needed to raise the temperature of one kilogram of the material by one degree. Materials with high specific heat absorb more energy before their temperature changes.

In practice, specific heat (together with density) controls the **thermal inertia** — how long it takes for temperature changes to propagate through the layer during transient events (cooldown, warmup, restart).

> **JSON key:** `specificHeat` (EN) · `calorEspecifico` (PT) — unit: J/(kg·°C)

## Density

Mass density contributes to thermal inertia. Combined with specific heat, it determines the volumetric heat capacity ($\rho \cdot C_p$) — the energy stored per unit volume per degree.

> **JSON key:** `rho` (EN) · `rho` (PT) — unit: kg/m³

## Viscosity

Viscosity describes a fluid's resistance to flow — more precisely, its internal friction when layers of fluid move relative to each other. For type 1 materials, viscosity is required to allow the simulator to characterize the fluid's convective behavior within a stagnant or quasi-stagnant annular layer.

In the context of a fluid-filled annulus, viscosity (together with thermal expansivity and density) governs the **Rayleigh number** — the dimensionless parameter that controls whether natural convection develops within the layer and, if so, how intensely. High viscosity suppresses convection; low viscosity promotes it.

> **JSON key:** `visc` (EN) · `visc` (PT) — unit: cP
> Required only for **Type 1** (user-defined fluid).

## Thermal Expansivity

Thermal expansivity (also called the coefficient of thermal expansion, β) quantifies how much the fluid's density changes with temperature at constant pressure. Fluids with higher expansivity become more buoyant as they are heated, which drives stronger natural convection cells within a confined layer.

Together with viscosity, density, and specific heat, β feeds directly into the **Rayleigh number**:

\$\$Ra = \frac{g \, \beta \, \Delta T \, L^3},{\nu \, \alpha}\$\$

where \$\nu\$ is the kinematic viscosity and \$\alpha\$ is the thermal diffusivity. A higher Rayleigh number indicates stronger buoyancy-driven convection, which increases the effective heat transfer across the fluid layer beyond pure conduction.

> **JSON key:** `beta` (EN) · `beta` (PT) — unit: 1/K
> Required only for **Type 1** (user-defined fluid).

!!! note
    `visc` and `beta` are **only meaningful for Type 1** materials. They are ignored for solids (Type 0) and are not required for the built-in water (Type 2) or air (Type 3) models, whose transport properties are computed internally.

## Thermal Diffusivity

The combination $\alpha = k / (\rho \cdot C_p)$ is the thermal diffusivity — it governs how fast temperature fronts propagate through the material. High diffusivity means rapid equilibration; low diffusivity means slow response.


## Material Types

Not all layers are solids with user-specified properties. Marlim3 supports four material types:

### Solid Material (Type 0)

Standard solid: steel, concrete, polymer insulation, cement. All three thermal properties (conductivity, specific heat, density) must be specified.

### User-Defined Fluid Layer (Type 1)

A stagnant or quasi-stagnant fluid layer (e.g., a fluid-filled annulus where convection is not explicitly modeled). Properties must be provided by the user.

For type 1, the following are required:

- `conductivity` / `condutividade`
- `specificHeat` / `calorEspecifico`
- `rho` / `rho`
- `visc` / `visc` (viscosity, cP)
- `beta` / `beta` (thermal expansivity, 1/K)

### Water (Type 2)

An internal water model is used by the simulator. Only the material type needs to be specified — no manual property input is needed. Use this for completion-fluid annuli, water-filled gaps, or seawater layers.

### Air (Type 3)

An internal air model is used by the simulator. Use this for gas-filled annuli or atmospheric gaps.

> **JSON key:** `type` (EN) · `tipo` (PT)
> Values: `0` = solid, `1` = user fluid, `2` = water, `3` = air

> **JSON keys for type 1 extras:** `visc` (EN) · `visc` (PT), `beta` (EN) · `beta` (PT)

!!! note
    For types 2 and 3, properties are computed internally from correlations. Only `id`, `type`, and optionally `label` need to be specified.

---

## Common Material Properties

| Material | Conductivity [W/(m·°C)] | Specific Heat [J/(kg·°C)] | Density [kg/m³] |
|----------|------------------------|--------------------------|-----------------|
| Carbon steel | 50 | 500 | 7800 |
| Stainless steel | 15 | 500 | 8000 |
| PU insulation (foam) | 0.03 | 1500 | 60 |
| Polypropylene (solid) | 0.22 | 1800 | 900 |
| Cement | 0.6 | 1000 | 500–2000 |
| Concrete weight coating | 1.5 | 880 | 2300 |

---

## Practical Guidance

- **Realistic insulation values:** Over-optimistic conductivity (< 0.02 W/(m·°C)) strongly distorts cooldown predictions. Use manufacturer-rated wet/aged values for subsea applications.
- **Reuse material IDs:** Define materials once in the top-level array and reference them by ID across multiple cross sections. Avoids duplication and inconsistency.
- **Reference in cross sections:** Layers reference materials by ID (`materialId` in EN JSON, `idMaterial` in PT JSON).
- **Completion fluids:** Use `type: 2` (water) for water-based completion-fluid annuli rather than manually specifying properties.

---

## Example JSON

```json
{
  "material": [
    {
      "id": 0,
      "active": true,
      "label": "Carbon steel",
      "type": 0,
      "conductivity": 50.0,
      "specificHeat": 500.0,
      "rho": 7800.0
    },
    {
      "id": 1,
      "active": true,
      "label": "PU insulation",
      "type": 0,
      "conductivity": 0.03,
      "specificHeat": 1500.0,
      "rho": 60.0
    },
    {
      "id": 2,
      "active": true,
      "label": "Annulus test fluid",
      "type": 1,
      "conductivity": 0.12,
      "specificHeat": 2200.0,
      "rho": 980.0,
      "visc": 12.0,
      "beta": 0.00045
    },
    {
      "id": 3,
      "active": true,
      "label": "Completion fluid",
      "type": 2
    },
    {
      "id": 4,
      "active": true,
      "label": "Cement",
      "type": 0,
      "conductivity": 0.6,
      "specificHeat": 1000.0,
      "rho": 500.0
    }
  ]
}
```

!!! tip
    For cooldown and restart studies, verify that insulation and cement conductivity values reflect actual field conditions (aged, wet, or damaged), not ideal laboratory values.
