# Pipes

Pipes define axial geometry: where flow moves, where gravity acts, and where heat is exchanged with formation, seawater, atmosphere, or a coupled service line. Each pipe is split into segments, and each segment is discretized into cells for the 1D solver.

---

## Production and Service Lines

Two segment arrays can be present:

- **Production line**: main multiphase path (wellbore, flow line, riser).
- **Service line**: auxiliary gas/service path (gas-lift, circulation).

> **JSON keys:**
>
> - Production: `productionPipe` (EN) · `dutosProducao` (PT)
> - Service: `servicePipe` (EN) · `dutosServico` (PT)

Common segment fields:

> `active` / `ativo`, `id` / `id`, `crossSectionId` / `idCorte`

---

## Inclination

Inclination controls hydrostatic pressure and strongly influences flow patterns.

### Direct Angle Mode

The segment angle is given directly relative to the horizontal.

> **JSON key:** `angle` (EN) · `angulo` (PT) — unit: **radians**

Angle convention follows flow direction even when geometry indexing is reversed:

> **JSON key:** `geometryFollowsFlow` (EN) · `sentidoGeometriaSegueEscoamento` (PT)

### XY Reconstruction Mode

If XY mode is active, inclination is inferred from segment endpoints:

> **JSON keys:** `xCoor`, `yCoor` (EN/PT)
> Requires `initialConfig.xyMode` / `configuracaoInicial.modoXY` set to true.


---

## Segment Thermal Context

Each segment can exchange heat with formation, ambient fluid, or a thermally coupled pipe.

### Cross-Section and Formation References

> **JSON keys:**
>
> - Cross section: `crossSectionId` (EN) · `idCorte` (PT)
> - Formation: `formationId` (EN) · `idFormacao` (PT)

### Ambient Medium Type

> **JSON key:** `environment` (EN) · `ambienteExterno` (PT)
> Values: `0` = user-defined medium, `1` = seawater, `2` = atmosphere

### Convection Direction

> **JSON key:** `convectionDirection` (EN) · `direcaoConveccao` (PT)
> Values: `0` = transversal, `1` = longitudinal

### Thermal Coupling with Service Line

Production segments can be coupled to service-line segments.

> **JSON key:** `thermalCoupling` (EN) · `acoplamentoTermico` (PT)

Engine/validation behavior:

- Coupling is segment-level, not cross-section-level.
- Coupling requires service line enabled (`gasLine` / `linhaGas` true).
- Coupled segments should coincide in position, length, and discretization.

---

## Initial and Ambient Profiles per Segment

Profiles inside `initialAndAmbientConditions` / `condicoesIniciaisEAmbiente` are defined at relative positions from 0 to 1.

> **Position key:** `measuredPosition` (EN) · `compInter` (PT)

### Initial-condition profiles (transient, `initialCondition = 0`)

> **JSON keys:**
>
> - Pressure: `pressure` · `pressao`
> - Temperature: `temp` · `temp`
> - Holdup: `holdup` · `holdup`
> - Complementary fraction: `complementaryFluidFraction` · `bet`
> - Superficial liquid velocity: `usl` · `uls`
> - Superficial gas velocity: `usg` · `ugs`

Service-line specific initial profile:

> - Gas mass flow: `gasMassFlowRate` · `vazaoMassicaGas`

### Ambient/external profiles

> **JSON keys:**
>
> - Ambient temperature: `ambientTemp` · `tempExterna`
> - Ambient velocity: `ambientVel` · `velExterna`
> - Ambient conductivity: `ambientConductivity` · `kExterna`
> - Ambient specific heat: `ambientSpecificHeat` · `calorEspecificoExterno`
> - Ambient density: `ambientDensity` · `rhoExterno`
> - Ambient viscosity: `ambientVisc` · `viscExterna`

Ambient profiles are conditionally required depending on `formationId`, `environment`, and `thermalCoupling`.

---

## Discretization

### Grouped blocks (default)

> **JSON keys:** `grouping: true` (`agrupamento: true`) and `discretization` / `discretizacao`
>
> - `numCells` · `nCelulas`
> - `length` · `comprimento` [m]

### Explicit cell sizes

> **JSON key:** `cellDx` (EN) · `dxCelula` (PT) when `grouping = false`

### XY mode cell count

> **JSON key:** `numCellsXY` (EN) · `nCelulas_XY` (PT)

---

## Example: Simple Horizontal Production Segment

```json
{
  "initialConfig": {
    "gasLine": false,
    "xyMode": false
  },
  "productionPipe": [
    {
      "id": 0,
      "active": true,
      "crossSectionId": 0,
      "formationId": 0,
      "angle": 0.0,
      "environment": 1,
      "grouping": true,
      "discretization": [
        {"numCells": 50, "length": 20.0}
      ],
      "initialConditions": {
        "measuredPosition": [0.0, 1.0],
        "ambientTemp": [4.0, 4.0],
        "ambientVel": [0.3, 0.3]
      }
    }
  ]
}
```

## Example: Coupled Production/Service Segments

```json
{
  "initialConfig": {
    "gasLine": true,
    "xyMode": false
  },
  "productionPipe": [
    {
      "id": 0,
      "active": true,
      "crossSectionId": 0,
      "angle": -1.57079632679,
      "formationId": 0,
      "thermalCoupling": 1,
      "grouping": true,
      "discretization": [
        {"numCells": 40, "length": 10.0}
      ]
    }
  ],
  "servicePipe": [
    {
      "id": 0,
      "active": true,
      "crossSectionId": 1,
      "angle": -1.57079632679,
      "grouping": true,
      "discretization": [
        {"numCells": 40, "length": 10.0}
      ]
    }
  ]
}
```

!!! tip
    Start with coarse cells (20-50 m) to validate setup, then refine near strong gradients (valves, pumps, major elevation changes, thermal-coupling regions).
