---
name: marlim3-pipeline-geometry
description: Use when the user needs to define pipeline geometry, including production ducts, service line ducts, discretization, inclination angles, XY coordinates, initial conditions, and thermal coupling for a Marlim3 simulation.
---

# Marlim3 Pipeline Geometry

## `dutosProducao` (array) — Production Line Segments

The production line is composed of ordered duct segments. Flow goes from the first duct (wellbore bottom) to the last (platform arrival).

### Duct Properties

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Unique identifier |
| `rotulo` | string | — | Label |
| `idCorte` | int | — | Reference to `secaoTransversal[].id` |
| `idFormacao` | int | — | Reference to `Formacao.Propriedades[].id` (for wellbore) |
| `angulo` | number | rad | Inclination: π/2=vertical up, 0=horizontal, -π/2=vertical down |
| `ambienteExterno` | int | — | 0=formation (default), 1=sea/ocean, 2=air |
| `direcaoConveccao` | int | — | External convection direction |
| `acoplamentoTermico` | int | — | 0=no coupling, 1=coupled with service line |
| `agrupamento` | bool | — | Duct grouping flag |
| `xCoor` | number | m | X coordinate of duct end (modoXY) |
| `yCoor` | number | m | Y coordinate of duct end (modoXY) |
| `nCelulas_XY` | int | — | Number of cells for XY mode |

### Discretization (`discretizacao[]`)

Each duct can have multiple discretization segments:

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `nCelulas` | int | — | Number of cells in this segment |
| `comprimento` | number | m | Length of this segment |

Total duct length = sum of all `comprimento` values.

### Initial Conditions (`condicoesIniciais`)

Interpolation arrays defined at duct boundaries (`compInter` goes from 0 to 1):

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `compInter` | array | — | Normalized positions [0..1] for interpolation |
| `temp` | array | °C | Temperature at each compInter point |
| `pressao` | array | kgf/cm² | Pressure at each point |
| `holdup` | array | — | Liquid holdup at each point |
| `bet` | array | — | Beta (complementary fluid fraction) |
| `uls` | array | m/s | Superficial liquid velocity |
| `ugs` | array | m/s | Superficial gas velocity |
| `tempExterna` | array | °C | External (ambient) temperature |
| `velExterna` | array | m/s | External fluid velocity (sea current) |
| `kExterna` | array | W/(m·°C) | External fluid conductivity |
| `calorEspecificoExterno` | array | J/(kg·°C) | External fluid Cp |
| `rhoExterno` | array | kg/m³ | External fluid density |
| `viscExterna` | array | Pa·s | External fluid viscosity |

**Minimum required**: `compInter` and `tempExterna` (for thermal BCs).

### Example: Vertical Well Segment

```json
{
  "id": 0, "rotulo": "Poço", "idCorte": 0, "idFormacao": 0,
  "angulo": 1.5707963267948966,
  "discretizacao": [{ "nCelulas": 20, "comprimento": 100 }],
  "condicoesIniciais": {
    "compInter": [0, 1],
    "tempExterna": [90, 60]
  }
}
```

### Example: Subsea Flowline

```json
{
  "id": 1, "rotulo": "Flowline", "idCorte": 1,
  "ambienteExterno": 1, "angulo": 0,
  "discretizacao": [{ "nCelulas": 50, "comprimento": 40 }],
  "condicoesIniciais": {
    "compInter": [0, 1],
    "tempExterna": [4, 4],
    "velExterna": [0.2, 0.2]
  }
}
```

### Example: Vertical Riser

```json
{
  "id": 2, "rotulo": "Riser", "idCorte": 1,
  "ambienteExterno": 1, "angulo": 1.5708,
  "discretizacao": [{ "nCelulas": 20, "comprimento": 50 }],
  "condicoesIniciais": {
    "compInter": [0, 1],
    "tempExterna": [4, 20],
    "velExterna": [0.2, 1.0]
  }
}
```

## `dutosServico` (array) — Service Line Segments

Service/gas injection line. Required when `configuracaoInicial.linhaGas: true`. Similar structure to `dutosProducao` but single-phase gas flow (no holdup/bet/uls).

Key difference in initial conditions:
- Uses `vazaoMassicaGas` instead of `uls`/`ugs`/`holdup`/`bet`

Flow direction is typically **opposite** to the production line (gas flows from platform down to the well), so angles are typically negative.

### Example: Service Riser (downward)

```json
{
  "id": 0, "rotulo": "Riser Serviço", "idCorte": 3,
  "ambienteExterno": 1, "angulo": -1.5708,
  "discretizacao": [{ "nCelulas": 20, "comprimento": 50 }],
  "condicoesIniciais": {
    "compInter": [0, 1],
    "temp": [20, 4],
    "tempExterna": [20, 4],
    "velExterna": [1.0, 0.2]
  }
}
```

## XY Coordinate Mode

When `configuracaoInicial.modoXY: true`, angles are auto-calculated from endpoint coordinates. Each duct specifies its endpoint `xCoor`/`yCoor`, and the system automatically computes inclination angles.

Set starting points:
- `configuracaoInicial.xProdInicio`, `yProdInicio` (production line start)
- `configuracaoInicial.xServInicio`, `yServInicio` (service line start, if applicable)

### Example with XY Mode

```json
"configuracaoInicial": { "modoXY": true, "linhaGas": true },
"dutosProducao": [
  { "id": 0, "idCorte": 0, "xCoor": 0, "yCoor": 1000, "nCelulas_XY": 20,
    "discretizacao": [{ "nCelulas": 20, "comprimento": 100 }] },
  { "id": 1, "idCorte": 1, "xCoor": 2000, "yCoor": 1000, "nCelulas_XY": 50,
    "discretizacao": [{ "nCelulas": 50, "comprimento": 40 }] },
  { "id": 2, "idCorte": 1, "xCoor": 2000, "yCoor": 0, "nCelulas_XY": 20,
    "discretizacao": [{ "nCelulas": 20, "comprimento": 50 }] }
]
```

## Thermal Coupling

`acoplamentoTermico: 1` couples a production duct with the corresponding service duct for thermal exchange (e.g., wellbore column + annular). The coupling is positional.

## Angle Convention

- `π/2` (1.5708) = vertical upward (wellbore going up, riser)
- `0` = horizontal (flowline)
- `-π/2` (-1.5708) = vertical downward (service line descending)
- Small positive = slight uphill
- Angles are always relative to the flow direction
