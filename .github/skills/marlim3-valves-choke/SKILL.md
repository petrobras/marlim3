---
name: marlim3-valves-choke
description: Use when the user needs to configure valves, chokes (surface or injection), master valves (ANM/downhole), inline valves, or PIG operations in a Marlim3 simulation.
---

# Marlim3 Valves, Chokes & PIG

## `valvula` (array) — Inline Valves

Generic inline valves with Cv curves and time-dependent opening.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position on production line |
| `curvaCV` | int | — | Cv curve type |
| `curvaDinamic` | int | — | Dynamic curve type |
| `tempo` | array | s | Time schedule |
| `abertura` | array | — | Opening fraction (0=closed, 1=fully open) |
| `cd` | number | — | Discharge coefficient |
| `x1` | array | — | Cv curve X points (opening fraction) |
| `cv1` | array | — | Cv curve Y points (Cv values) |

## `fonteChoke` (object) — Production Choke

Choke valve at the production side (typically downhole).

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `tempo` | array | s | Time schedule |
| `abertura` | array | — | Opening fraction (0-1) |
| `coeficienteDescarga` | number | — | Discharge coefficient |
| `modelo` | int | — | Choke model: 0=standard |

## `chokeSup` (object) — Surface Choke

Production choke at the surface/platform.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `curvaCV` | int | — | Cv curve type |
| `curvaDinamic` | int | — | Dynamic curve type |
| `tempo` | array | s | Time schedule |
| `abertura` | array | — | Opening fraction (0-1) |
| `coeficienteDescarga` | number | — | Discharge coefficient (typically 0.84) |
| `modelo` | int | — | Choke model: 0=standard |
| `x1` | array | — | Cv curve X points |
| `cv1` | array | — | Cv curve Y points |

### Example: Surface Choke with Events

```json
"chokeSup": {
  "coeficienteDescarga": 0.84,
  "modelo": 0,
  "abertura": [0.3, 0.3, 0.0, 0.0, 0.3],
  "tempo": [0, 7000, 7065, 8000, 8065]
}
```

## `chokeInj` (object) — Injection Choke

Choke on the injection/service line.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `tempo` | array | s | Time schedule |
| `abertura` | array | — | Opening fraction |
| `coeficienteDescarga` | number | — | Discharge coefficient |

## `master1` (object) — Downhole Master Valve (ANM)

Main wellhead/ANM valve. Critical for shutdown simulations.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position (typically at mudline) |
| `curvaCV` | int | — | Cv curve type |
| `razaoAreaAtiva` | number | — | Active area ratio |
| `tempo` | array | s | Time schedule |
| `abertura` | array | — | Opening fraction (0=closed, 1=open) |
| `x1` | array | — | Cv curve X points |
| `cv1` | array | — | Cv curve Y values |

### Example: Master Valve with Shutdown

```json
"master1": {
  "comprimentoMedido": 1980,
  "abertura": [1, 1, 0, 0, 1],
  "tempo": [0, 7000, 7065, 35000, 35060]
}
```

## `master2` (object) — Secondary Master Valve

Secondary wellhead valve (wing valve). Same structure as `master1` but without Cv fields.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position |
| `tempo` | array | s | Time schedule |
| `abertura` | array | — | Opening fraction |

```json
"master2": {
  "comprimentoMedido": 5020,
  "abertura": [1, 1, 0, 0, 1],
  "tempo": [0, 7000, 7065, 35000, 35065]
}
```

## `pig` (array) — PIG Operations

Pipeline pigging (Pipeline Inspection Gauge) operations.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `lancador` | number | m | Launcher measured depth |
| `recebedor` | number | m | Receiver measured depth |
| `tempo` | number | s | Launch time |
| `folgaArea` | number | — | Area clearance ratio (0 = perfect seal) |
| `deltaPressao` | number | kgf/cm² | PIG differential pressure |
| `cdPig` | number | — | PIG discharge coefficient |

### Example: PIG Operation

```json
"pig": [{
  "id": 0,
  "lancador": 2060,
  "recebedor": 6960,
  "tempo": 20000,
  "folgaArea": 0.0,
  "deltaPressao": 0.1,
  "cdPig": 0.84
}]
```

## Typical Shutdown Scenario

A common transient scenario involves closing valves and chokes for a well shutdown:

```json
{
  "master1": {
    "comprimentoMedido": 1980,
    "abertura": [1, 1, 0, 0, 1],
    "tempo": [0, 7000, 7065, 35000, 35060]
  },
  "master2": {
    "comprimentoMedido": 5020,
    "abertura": [1, 1, 0, 0, 1],
    "tempo": [0, 7000, 7065, 35000, 35065]
  },
  "chokeSup": {
    "coeficienteDescarga": 0.84, "modelo": 0,
    "abertura": [0.3, 0.3, 0, 0, 0.3],
    "tempo": [0, 7000, 7065, 8000, 8065]
  }
}
```

This closes all valves at t=7000-7065s and reopens at t=35000-35065s.
