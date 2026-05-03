---
name: marlim3-materials-cross-sections
description: Use when the user needs to define pipe materials (steel, cement, insulation, fluids), cross-section geometry (layers, diameters, roughness), or formation rock properties for thermal exchange in a Marlim3 simulation.
---

# Marlim3 Materials & Cross-Sections

## `material` (array)

Defines thermal and mechanical properties of pipe wall materials.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Unique identifier |
| `rotulo` | string | — | Label/name |
| `tipo` | int | — | 0=solid, 2=fluid (completion fluid, annular fluid) |
| `condutividade` | number | W/(m·°C) | Thermal conductivity |
| `calorEspecifico` | number | J/(kg·°C) | Specific heat |
| `rho` | number | kg/m³ | Density |
| `visc` | number | Pa·s | Dynamic viscosity (only for tipo=2 fluids) |
| `beta` | number | 1/°C | Thermal expansion coefficient (for tipo=2) |

### Common Materials Reference

| Material | k [W/m·°C] | Cp [J/kg·°C] | ρ [kg/m³] | tipo |
|----------|-----------|-------------|----------|------|
| Carbon steel | 55-58 | 460-480 | 7833-7850 | 0 |
| Cement | 0.6 | 1000 | 500 | 0 |
| Insulation (polymer) | 0.12-0.19 | 1200-2100 | 500-510 | 0 |
| Completion fluid | 0.61 | 4180 | 1000 | 2 |
| Flexible pipe layers | 0.19-70 | 460-2300 | 500-7833 | 0 |

### Example

```json
"material": [
  { "id": 0, "rotulo": "Aço", "tipo": 0, "condutividade": 58, "calorEspecifico": 480, "rho": 7850 },
  { "id": 1, "rotulo": "Fluido de completação", "tipo": 2 },
  { "id": 2, "rotulo": "Cimento", "tipo": 0, "condutividade": 0.6, "calorEspecifico": 1000, "rho": 500 },
  { "id": 3, "rotulo": "Isolante", "tipo": 0, "condutividade": 0.12, "calorEspecifico": 1214, "rho": 510 }
]
```

## `secaoTransversal` (array)

Defines pipe cross-section geometry with radial layers.

### Cross-Section Properties

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Unique identifier |
| `rotulo` | string | — | Label |
| `anular` | bool | — | true if annular geometry (service line) |
| `diametroInterno` | number | m | Inner diameter of the flow bore |
| `diametroExterno` | number | m | Outer diameter (annular only) |
| `rugosidade` | number | m | Inner wall roughness |
| `camadas` | array | — | Radial layers from inside out |

### Layer Properties (`camadas[]`)

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `rotulo` | string | — | Layer label |
| `idMaterial` | int | — | Reference to `material[].id` |
| `tipoMedicaoCamada` | string | — | `"ESPESSURA"` (thickness) or omit for diameter mode |
| `espessura` | number | m | Layer thickness (when using ESPESSURA mode) |
| `diametro` | number | m | Outer diameter of this layer (when using diameter mode) |
| `discretizacao` | int | — | Number of radial nodes for thermal calculation |

### Example: Well Cross-Section (thickness mode)

```json
{
  "id": 0, "rotulo": "Corte do poço",
  "diametroInterno": 0.2032, "rugosidade": 0.000183,
  "camadas": [
    { "rotulo": "Aço interno", "idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.00635 },
    { "rotulo": "Fluido completação", "idMaterial": 1, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0762 },
    { "rotulo": "Revestimento", "idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0127 },
    { "rotulo": "Cimento", "idMaterial": 2, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0762 }
  ]
}
```

### Example: Flowline Cross-Section (diameter mode)

```json
{
  "id": 2, "rotulo": "Flexible pipe",
  "diametroInterno": 0.1524, "rugosidade": 0.0001,
  "camadas": [
    { "diametro": 0.1692, "idMaterial": 3, "discretizacao": 1 },
    { "diametro": 0.1832, "idMaterial": 4, "discretizacao": 1 },
    { "diametro": 0.23014, "idMaterial": 5, "discretizacao": 1 }
  ]
}
```

### Example: Annular Cross-Section (service line)

```json
{
  "id": 4, "rotulo": "Anular do poço",
  "anular": true,
  "diametroInterno": 0.2159, "diametroExterno": 0.3683,
  "rugosidade": 0.000183,
  "camadas": [
    { "idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0127 },
    { "idMaterial": 2, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0762 }
  ]
}
```

## `configuracaoInicial.Formacao` (object)

Rock formation properties for wellbore-formation thermal exchange.

| Field | Type | Description |
|-------|------|-------------|
| `TempoProducao` | number | Production time in **days** (affects thermal radius) |
| `Propriedades` | array | Rock types |

### Formation Rock Properties

| Field | Type | Unit |
|-------|------|------|
| `id` | int | — |
| `rotulo` | string | — |
| `condutividade` | number | W/(m·°C) |
| `calorEspecifico` | number | J/(kg·°C) |
| `massaEspecifica` | number | kg/m³ |

### Common Rock Types

| Rock | k | Cp | ρ |
|------|---|----|----|
| Generic | 2.5 | 1000 | 2500 |
| Shale | 1.6 | 2151 | 2057 |
| Sandstone | 2.5 | 737 | 2198 |
| Halite | 3.97 | 875 | 2160 |
| Limestone | 2.19 | 887 | 2540 |
| Dolomite | 3.56 | 933 | 2840 |

```json
"Formacao": {
  "TempoProducao": 365,
  "Propriedades": [
    { "id": 0, "condutividade": 2.5, "calorEspecifico": 1000, "massaEspecifica": 2500 }
  ]
}
```
