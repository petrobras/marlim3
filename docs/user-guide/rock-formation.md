# Rock Formation

The formation acts as a thermal reservoir around the production system. It matters whenever a pipe segment references a formation ID, and it strongly influences long-term cooldown, restart, and wall-temperature evolution.

> **JSON key:** `initialConfig.formation` (EN) · `configuracaoInicial.Formacao` (PT)

---

## What This Object Represents

The formation object has two concepts:

- A production-history time scale (`productionTime`) that sets the thermal memory around the well.
- A list of rock-property sets (`properties`) identified by `id`.

Pipe segments consume these IDs using `productionPipe[].formationId`.

If no formation is referenced in the pipe segment, heat exchange is handled by ambient environment settings or thermal coupling, not by formation properties.

---

## Production Time

`productionTime` represents how long the well has already been producing before the simulation starts. Larger values imply a wider pre-heated rock zone and usually slower early cooldown.

> **JSON key:** `productionTime` (EN) · `TempoProducao` (PT)
> Unit: days

---

## Rock Property Set

Each entry in `properties` defines one rock type used by one or more pipe segments.

> **JSON key:** `properties` (EN) · `Propriedades` (PT)

Each element contains:

- `id` · `id`
- `conductivity` · `condutividade` [W/(m.degC)]
- `specificHeat` · `calorEspecifico` [J/(kg.degC)]
- `density` · `massaEspecifica` [kg/m3]

The volumetric thermal storage is $\rho C_p$, and thermal diffusivity is:

$$
\alpha = \frac{k}{\rho C_p}
$$

Higher $\alpha$ propagates temperature fronts faster into the formation.

---

## Practical Modeling Notes

- Use different formation IDs where lithology changes materially along depth/length.
- Avoid unrealistically low conductivity, which can over-insulate the line and overpredict retained heat.
- Buried lines often use formation coupling; exposed subsea lines often rely on seawater environment instead.

---

## Example: Single Rock Definition

```json
{
  "initialConfig": {
    "formation": {
      "productionTime": 365,
      "properties": [
        {
          "id": 0,
          "conductivity": 2.5,
          "specificHeat": 850.0,
          "density": 2500.0
        }
      ]
    }
  }
}
```

## Example: Two Formation IDs

```json
{
  "initialConfig": {
    "formation": {
      "productionTime": 180,
      "properties": [
        {
          "id": 0,
          "conductivity": 3.0,
          "specificHeat": 900.0,
          "density": 2400.0
        },
        {
          "id": 1,
          "conductivity": 1.5,
          "specificHeat": 950.0,
          "density": 2500.0
        }
      ]
    }
  }
}
```
