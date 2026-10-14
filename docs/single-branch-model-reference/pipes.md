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

The angle convention (`sentidoGeometriaSegueEscoamento`) follows flow direction even when geometry indexing is reversed. This is a **global** setting in `configuracaoInicial`, not per-segment.

### Direct Angle Mode

The segment angle is given directly relative to the horizontal.

> **JSON keys:**
>
> - Angle: `angle` (EN) · `angulo` (PT) — unit: **radians**

> - Geometry follows flow: `geometryFollowsFlow` (EN) · `sentidoGeometriaSegueEscoamento` (PT) — **global** in `configuracaoInicial`, default `true`

### XY Reconstruction Mode

If XY mode is active, inclination and segment length are inferred from segment endpoint coordinates.

> **JSON keys:** `xCoor`, `yCoor` (EN/PT)
> Requires `initialConfig.xyMode` / `configuracaoInicial.modoXY` set to `true`.
>
> Starting coordinates are set globally:
> - Production: `xProdInicio`, `yProdInicio` in `configuracaoInicial` (default `0`)
> - Service: `xServInicio`, `yServInicio` in `configuracaoInicial` (only when `linhaGas = true`)
> 
> In XY mode, each segment's `xCoor`/`yCoor` defines the **end** position of that segment; the start position is the previous segment's end (or the global start for the first segment).

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
> Values: `0` = user-defined medium (default), `1` = seawater, `2` = atmosphere
>
> **Production line:** required unless `idFormacao` is set or `acoplamentoTermico = 1`.
> **Service line:** required unless `idFormacao` is set.

### Convection Direction

> **JSON key:** `convectionDirection` (EN) · `direcaoConveccao` (PT)
> Values: `0` = transversal (default), `1` = longitudinal
> Only valid when `idFormacao` is not set. Typically `0` for immersion in infinite medium, `1` for annulus coupling.

### Thermal Coupling with Service Line

Production segments can be coupled to service-line segments.

> **JSON key:** `thermalCoupling` (EN) · `acoplamentoTermico` (PT)

Engine/validation behavior:

- Coupling is segment-level, not cross-section-level.
- Coupling requires service line enabled (`gasLine` / `linhaGas` true).
- Coupled segments should coincide in position, length, and discretization.

---

## Initial and Ambient Profiles per Segment

Each pipe segment can include an `initialConditions` (EN) · `condicoesIniciais` (PT) object that specifies initial-condition profiles (for transient startup with `condicaoInicial = 0`) and ambient/external profiles for heat exchange. Profiles are defined at relative positions from 0 to 1.

> **Position key:** `measuredPosition` (EN) · `compInter` (PT)
> Must start at `0.0` and end at `1.0`. Intermediate points are interpolated linearly.

### Initial-condition profiles (production line, `condicaoInicial = 0`)

> **JSON keys:**
>
> - Pressure: `pressure` · `pressao` — unit: `kgf/cm²`
> - Temperature: `temp` · `temp` — unit: `°C`
> - Holdup: `holdup` · `holdup`
> - Complementary fraction: `complementaryFluidFraction` · `bet` — only if complementary fluid is active
> - Superficial liquid velocity: `usl` · `uls` — unit: `m/s`
> - Superficial gas velocity: `usg` · `ugs` — unit: `m/s`

### Initial-condition profiles (service line, `condicaoInicial = 0`)

The service line has a reduced set of initial-condition fields (no holdup, slip velocities, or complementary fraction):

> **JSON keys:**
>
> - Pressure: `pressure` · `pressao` — unit: `kgf/cm²`
> - Temperature: `temp` · `temp` — unit: `°C`
> - Gas mass flow: `gasMassFlowRate` · `vazaoMassicaGas` — unit: `kg/s`

### Ambient/external profiles (production line)

Each ambient profile field has specific conditional requirements:

> **JSON keys:**
>
> - Ambient temperature: `ambientTemp` · `tempExterna` — unit: `°C`
>   :   Required **always, except** when `acoplamentoTermico = 1`.
> - Ambient velocity: `ambientVel` · `velExterna` — unit: `m/s`
>   :   Required **always, except** when `acoplamentoTermico = 1` **or** `idFormacao` is set.
> - Ambient conductivity: `ambientConductivity` · `kExterna` — unit: `W/(m·K)`
>   :   Required **always, except** when `acoplamentoTermico = 1`, `idFormacao` is set, **or** `ambienteExterno = 1 or 2`.
> - Ambient specific heat: `ambientSpecificHeat` · `calorEspecificoExterno` — unit: `J/(kg·K)`
>   :   Required **always, except** when `acoplamentoTermico = 1`, `idFormacao` is set, **or** `ambienteExterno = 1 or 2`.
> - Ambient density: `ambientDensity` · `rhoExterno` — unit: `kg/m³`
>   :   Required **always, except** when `acoplamentoTermico = 1`, `idFormacao` is set, **or** `ambienteExterno = 1 or 2`.
> - Ambient viscosity: `ambientVisc` · `viscExterna` — unit: `cP`
>   :   Required **always, except** when `acoplamentoTermico = 1`, `idFormacao` is set, **or** `ambienteExterno = 1 or 2`.

### Ambient/external profiles (service line)

Service line ambient fields have simpler conditional logic (no thermal coupling exceptions):

> - `tempExterna` — required **always**.
> - `velExterna` — required **always, except** when `idFormacao` is set.
> - `kExterna`, `calorEspecificoExterno`, `rhoExterno`, `viscExterna` — required **always, except** when `idFormacao` is set **or** `ambienteExterno = 1 or 2`.

> **Summary of when ambient profiles are NOT needed:**
>
> | Condition | tempExterna | velExterna | kExterna, calorEspecif, rho, visc |
> |---|---|---|---|
> | `acoplamentoTermico = 1` (prod. only) | ✗ | ✗ | ✗ |
> | `idFormacao` set | ✓ (serv) | ✗ | ✗ |
> | `ambienteExterno = 1 or 2` | ✓ | ✓ | ✗ |
>
> (✗ = not required, ✓ = still required)

---

## Example: Simple Horizontal Production Segment (PT keys)

```json
{
  "configuracaoInicial": {
    "linhaGas": false,
    "modoXY": false,
    "sentidoGeometriaSegueEscoamento": true
  },
  "dutosProducao": [
    {
      "id": 0,
      "ativo": true,
      "idCorte": 0,
      "idFormacao": 0,
      "angulo": 0.0,
      "ambienteExterno": 1,
      "agrupamento": true,
      "discretizacao": [
        {"nCelulas": 50, "comprimento": 20.0}
      ],
      "condicoesIniciais": {
        "compInter": [0.0, 1.0],
        "tempExterna": [4.0, 4.0],
        "velExterna": [0.3, 0.3]
      }
    }
  ]
}
```

## Example: Simple Horizontal Production Segment (EN keys)

```json
{
  "initialConfig": {
    "gasLine": false,
    "xyMode": false,
    "geometryFollowsFlow": true
  },
  "productionPipe": [
    {
      "id": 0,
      "active": true,
      "crossSectionId": 0,
      "angle": 0.0,
      "formationId": 0,
      "thermalCoupling": 1,
      "grouping": true,
      "discretization": [
        {"numCells": 50, "length": 20.0}
      ]
    }
  ]
}
```

## Example: Coupled Production/Service Segments (PT keys)

```json
{
  "configuracaoInicial": {
    "linhaGas": true,
    "modoXY": false,
    "sentidoGeometriaSegueEscoamento": true
  },
  "dutosProducao": [
    {
      "id": 0,
      "ativo": true,
      "idCorte": 0,
      "angulo": -1.57079632679,
      "idFormacao": 0,
      "acoplamentoTermico": 1,
      "agrupamento": true,
      "discretizacao": [
        {"nCelulas": 40, "comprimento": 10.0}
      ]
    }
  ],
  "dutosServico": [
    {
      "id": 0,
      "ativo": true,
      "idCorte": 1,
      "angulo": -1.57079632679,
      "agrupamento": true,
      "discretizacao": [
        {"nCelulas": 40, "comprimento": 10.0}
      ]
    }
  ]
}
```

## Example: Coupled Production/Service Segments (EN keys)

```json
{
  "initialConfig": {
    "gasLine": true,
    "xyMode": false,
    "geometryFollowsFlow": true
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

## Example: XY Mode with Correction Factors (PT keys)

```json
{
  "configuracaoInicial": {
    "linhaGas": false,
    "modoXY": true,
    "xProdInicio": 0.0,
    "yProdInicio": -2000.0
  },
  "dutosProducao": [
    {
      "id": 0,
      "ativo": true,
      "idCorte": 0,
      "idFormacao": 0,
      "xCoor": 500.0,
      "yCoor": -1800.0,
      "nCelulas_XY": 25,
      "dPdLFric": 0,
      "dTdL": 0
    },
    {
      "id": 1,
      "ativo": true,
      "idCorte": 0,
      "idFormacao": 0,
      "xCoor": 1000.0,
      "yCoor": -1500.0,
      "nCelulas_XY": 50,
      "dPdLFric": 1,
      "dTdL": 1
    }
  ]
}
```

!!! tip
    Start with coarse cells (20-50 m) to validate setup, then refine near strong gradients (valves, pumps, major elevation changes, thermal-coupling regions).

---

## Advanced Thermal Models

### 2D Thermal Diffusion (Buried Pipes)

When a pipe segment is buried, a 2D thermal diffusion model can be activated.

> **JSON keys:**
>
> - `diffusion2d` (EN) · `difusTerm2D` (PT): `0` = disabled (default), `1` = enabled
> - `diffusion2dJson` (EN) · `difusTerm2DJSON` (PT): name of the JSON file describing the 2D model (required when `diffusion2d = 1`)

### 3D Thermal Diffusion

Three complementary 3D thermal diffusivity arrays are available per segment:

> **JSON keys:**
>
> - `diffusion3d` (EN) · `difusTerm3D` (PT): array of integers
> - `diffusion3dFE` (EN) · `difusTerm3DFE` (PT): array of numbers (finite elements)
> - `diffusion3dCoupling` (EN) · `difusTerm3DAcop` (PT): array of strings (coupled)

!!! note
    3D diffusion also requires `diffusion3dMode` (EN) · `modoDifus3D` (PT) `= true` at the global level (`configuracaoInicial`).

### Parallel Network Thermal Coupling

> **JSON key:** `parallelNetworkThermalCoupling` (EN) · `acoplamentoTermicoRedeParalela` (PT) — default `0`
> Indicator for thermal coupling with a parallel network for heat exchange calculations.

---

## Pressure and Temperature Correction Factors

Each pipe segment can reference correction factors defined in the `correction` (EN) · `correcao` (PT) top-level object. These adjust gradients computed by the solver:

> **JSON keys:**
>
> - `correctionFactorHydro` (EN) · `dPdLHidro` (PT): index into `correcao.dPdLHidro[]` — hydrostatic pressure gradient correction
> - `correctionFactorFric` (EN) · `dPdLFric` (PT): index into `correcao.dPdLFric[]` — friction pressure gradient correction
> - `correctionFactorTemp` (EN) · `dTdL` (PT): index into `correcao.dTdL[]` — temperature gradient correction

---

## Legacy Marlim2 Correlations

When the Drift-Flux 1D model is disabled (`tipoModeloDrift = false` in `configuracaoInicial`), an empirical Marlim 2 correlation can be used for steady-state pressure gradient:

> **JSON key:** `ssFlowModel` (EN) · `correlacaoMR2` (PT)

| Value | Correlation |
|---|---|
| 0 | Poettmann-Carpenter |
| 1 | Baxendell-Thomas |
| 2 | Fancher-Brown |
| 3 | Hagedorn-Brown |
| 4 | Duns-Ros |
| 5 | Orkiszewski |
| 6 | Beggs & Brill |
| 7 | Mukherjee-Brill |
| 8 | Aziz |
| 9 | Gray |
| 10 | Oliemans |
| 11 | Dukler |
| 12 | Beggs & Brill with Palmer correction |
| 13 | Dukler, Eaton & Flanigan |
| 14 | Dukler & Minami I |
| 15 | Dukler & Minami II |
| 16 | Gomez |
