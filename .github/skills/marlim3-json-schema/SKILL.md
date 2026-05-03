---
name: marlim3-json-schema
description: Use when the user needs to understand the overall structure of a Marlim3 simulation JSON input file, validate a JSON, or create the skeleton/boilerplate for a new simulation. Covers the top-level fields, mandatory sections, versioning, and cross-reference validation rules.
---

# Marlim3 JSON Input Schema — Structure & Validation

## Top-Level JSON Structure

Every Marlim3 simulation input is a JSON object with the following top-level keys. The simulator reads this via the `JSON_entrada` root class defined in `src/JSON_entrada.h`.

### Mandatory Fields

```json
{
  "sistema": "MULTIFASICO",
  "versao": "1.0",
  "versaoJson": "1.3.9",
  "configuracaoInicial": { ... },
  "fluidosProducao": [ ... ],
  "material": [ ... ],
  "secaoTransversal": [ ... ],
  "dutosProducao": [ ... ]
}
```

### `sistema` (string, required)
Defines the simulation system type:
- `"MULTIFASICO"` — Production well/system (default, most common)
- `"INJETOR"` — Injection well

### `versao` (string, required)
Always `"1.0"`.

### `versaoJson` (string, required)
Always `"1.3.9"` for the current version.

## Complete List of Top-Level Sections

| Key | Type | Purpose | Required |
|-----|------|---------|----------|
| `sistema` | string | System type | Yes |
| `versao` | string | Version | Yes |
| `versaoJson` | string | JSON schema version | Yes |
| `configuracaoInicial` | object | General simulation parameters | Yes |
| `tabela` | object | P/T table parameters | No |
| `parafina` | object | Wax deposition model | No |
| `fluidoGas` | object | Gas properties (service line) | No |
| `fluidosProducao` | array | Production fluid definitions | Yes |
| `fluidoComplementar` | object | Custom complementary fluid | No |
| `valvula` | array | Inline valves | No |
| `fonteLiquido` | array | Liquid source terms | No |
| `fonteMassa` | array | Mass source terms | No |
| `fonteGas` | array | Gas source terms | No |
| `fontePoroRadial` | array | Radial porous sources | No |
| `fontePoro2D` | array | 2D porous sources | No |
| `fonteGasLift` | array | Gas lift valves | No |
| `material` | array | Material property definitions | Yes |
| `secaoTransversal` | array | Cross-section definitions | Yes |
| `tempo` | object | Time stepping configuration | Transient only |
| `dutosProducao` | array | Production duct segments | Yes |
| `dutosServico` | array | Service line duct segments | When linhaGas=true |
| `hidrato` | object | Hydrate modeling | No |
| `ipr` | array | Inflow Performance Relationship | Common |
| `fonteChoke` | object | Production choke | No |
| `bcs` | array | ESP/BCS pumps | No |
| `multibcs` | array | Multi-curve BCS pumps | No |
| `bombaVolumetrica` | array | Volumetric pumps | No |
| `deltaPressao` | array | Pressure differential devices | No |
| `fonteCalor` | array | Heat sources | No |
| `master1` | object | Downhole master valve (ANM) | No |
| `master2` | object | Secondary master valve | No |
| `pig` | array | PIG operations | No |
| `fontePressao` | array | Pressure sources | No |
| `tendP` | array | Production line trend monitors | No |
| `tendTransP` | array | Transverse temperature trends (prod) | No |
| `tendS` | array | Service line trend monitors | No |
| `tendTransS` | array | Transverse temperature trends (serv) | No |
| `tela` | array | Screen output configuration | No |
| `gasInj` | object | Gas injection boundary condition | No |
| `perfilProducao` | object | Production profile output config | Recommended |
| `perfilServico` | object | Service profile output config | No |
| `perfisTransP` | object | Transient profile snapshots (prod) | No |
| `perfisTransS` | object | Transient profile snapshots (serv) | No |
| `separador` | object | Separator boundary condition | Common |
| `correcao` | object | Gradient correction factors | No |
| `chokeSup` | object | Surface choke | No |
| `chokeInj` | object | Injection choke | No |
| `CondicaoContPocInjec` | object | Injection well boundary | INJETOR only |
| `intermitenciaSevera` | array | Severe slugging model | No |

## Cross-Reference Validation Rules

When building a simulation JSON, ensure these ID references are consistent:

1. **`dutosProducao[].idCorte`** and **`dutosServico[].idCorte`** must match an existing `secaoTransversal[].id`
2. **`secaoTransversal[].camadas[].idMaterial`** must match an existing `material[].id`
3. **`dutosProducao[].idFormacao`** and **`dutosServico[].idFormacao`** must match an existing `configuracaoInicial.Formacao.Propriedades[].id`
4. **`ipr[].indFluidoPro`** / **`ipr[].indiFluidoPro`** must match an existing `fluidosProducao[].id`
5. **`fonteLiquido[].indiFluidoPro`** must match an existing `fluidosProducao[].id`
6. **`fonteMassa[].indiFluidoPro`** must match an existing `fluidosProducao[].id`
7. **`fonteGas[].indiFluidoPro`** must match an existing `fluidosProducao[].id`
8. **`fontePressao[].indiFluidoPro`** must match an existing `fluidosProducao[].id`
9. **`configuracaoInicial.iniFluidoP`** must match an existing `fluidosProducao[].id`

## Minimal Working Example

A minimal steady-state production simulation:

```json
{
  "sistema": "MULTIFASICO",
  "versao": "1.0",
  "versaoJson": "1.3.9",
  "configuracaoInicial": {},
  "fluidosProducao": [
    { "id": 0, "api": 25, "rgo": 100, "densidadeGas": 0.7, "bsw": 0.0 }
  ],
  "material": [
    { "id": 0, "tipo": 0, "condutividade": 58, "calorEspecifico": 480, "rho": 7850 }
  ],
  "secaoTransversal": [
    {
      "id": 0, "diametroInterno": 0.2032, "rugosidade": 0.000183,
      "camadas": [
        { "idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.00635 }
      ]
    }
  ],
  "dutosProducao": [
    {
      "id": 0, "idCorte": 0, "angulo": 1.5708,
      "discretizacao": [{ "nCelulas": 20, "comprimento": 100 }],
      "condicoesIniciais": { "compInter": [0, 1], "tempExterna": [90, 4] }
    }
  ],
  "fonteLiquido": [
    {
      "id": 0, "indiFluidoPro": 0, "comprimentoMedido": 0.1,
      "tempo": [0], "vazaoLiquido": [1500], "temperatura": [90]
    }
  ],
  "separador": { "tempo": [0], "pressao": [10] },
  "perfilProducao": {
    "tempo": [0], "pressao": true, "temperatura": true, "holdup": true
  }
}
```

## Python API Equivalent

```python
import marlim3

caso = marlim3.Tramo()
caso.sistema = "MULTIFASICO"
caso.fluidosProducao = [{"id": 0, "api": 25, "rgo": 100, "densidadeGas": 0.7, "bsw": 0.0}]
caso.material = [{"id": 0, "tipo": 0, "condutividade": 58, "calorEspecifico": 480, "rho": 7850}]
caso.secaoTransversal = [{"id": 0, "diametroInterno": 0.2032, "rugosidade": 0.000183, "camadas": [...]}]
caso.dutosProducao = [{"id": 0, "idCorte": 0, "angulo": 1.5708, "discretizacao": [...]}]
caso.separador = {"tempo": [0], "pressao": [10]}
caso.perfilProducao = {"tempo": [0], "pressao": True, "temperatura": True}
caso.simular(label="my_sim", diretorio="results")
```
