---
name: marlim3-artificial-lift
description: Use when the user needs to configure artificial lift systems in Marlim3, including gas lift valves, ESP/BCS (Electric Submersible Pump) systems, multi-curve BCS, volumetric pumps, or pressure differential devices.
---

# Marlim3 Artificial Lift Configuration

## `fonteGasLift` (array) — Gas Lift Valves

Defines gas lift valves (VGLs) connecting the service line (annular/gas) to the production line.

**Prerequisites**: `configuracaoInicial.linhaGas: true`, `dutosServico` defined, `gasInj` configured.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Unique identifier |
| `ativo` | bool | — | Active flag |
| `colunaEanular` | bool | — | If true, valve connects column to annular |
| `comprimentoMedidoProducao` | number | m | Position on production line |
| `comprimentoMedidoServico` | number | m | Position on service line |
| `tipoValvula` | int | — | 0=orifice, 1=IPO (Injection Pressure Operated) |
| `diametroOrificio` | number | m | Orifice diameter |
| `cdvgl` | number | — | Gas discharge coefficient (default via model) |
| `frecupera` | number | — | Recovery factor |
| `cdvLiq` | number | — | Liquid discharge coefficient |
| `frecuperaLiq` | number | — | Liquid recovery factor |
| `razaoArea` | number | — | Area ratio (Ap/Ab for IPO valves) |
| `pressaoCalibracao` | number | kgf/cm² | Calibration pressure (bellows) |
| `temperaturaCalibracao` | number | °C | Calibration temperature |
| `diametroExterno` | number | m | External valve diameter |

### Example: Single Gas Lift Valve

```json
"fonteGasLift": [{
  "id": 0,
  "comprimentoMedidoProducao": 200,
  "comprimentoMedidoServico": 2800,
  "tipoValvula": 0,
  "diametroOrificio": 0.005,
  "frecupera": 0.0,
  "razaoArea": 0.1
}]
```

### Example: IPO Valve with Calibration

```json
"fonteGasLift": [{
  "id": 0,
  "comprimentoMedidoProducao": 190,
  "comprimentoMedidoServico": 6810,
  "tipoValvula": 0,
  "diametroOrificio": 0.007938,
  "cdvgl": 1.0,
  "frecupera": 0,
  "razaoArea": 0.1,
  "pressaoCalibracao": 0.2
}]
```

## `bcs` (array) — BCS/ESP Pumps

Electric Submersible Pump (Bomba Centrífuga Submersa) configuration.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position on production line |
| `tempo` | array | s | Time schedule |
| `frequencia` | array | Hz | Operating frequency at each time |
| `vazao` | array | m³/d | Performance curve: flow rate points |
| `head` | array | m | Performance curve: head per stage |
| `potencia` | array | kW | Performance curve: power per stage |
| `eficiencia` | array | % | Performance curve: efficiency |
| `freqref` | number | Hz | Reference frequency for the curves |
| `nestag` | int | — | Number of stages |
| `nestagFab` | int | — | Factory number of stages |
| `EficienciaMotor` | number | % | Motor efficiency (for heat dissipation) |
| `FrequenciaMinima` | number | Hz | Minimum operating frequency |
| `correcHI` | bool | — | Head correction for high gas fraction |
| `fracTermMotorEfic` | number | — | Fraction of motor heat to fluid |

### Example: BCS with Performance Curve

```json
"bcs": [{
  "id": 0, "ativo": true,
  "comprimentoMedido": 100,
  "tempo": [0, 500, 501, 7000, 7065, 30060, 30120],
  "frequencia": [60, 60, 60, 60, 0, 0, 60],
  "vazao": [0, 9375, 20000, 25000, 29750, 34375],
  "head": [53.5, 47, 39, 34, 26, 17],
  "potencia": [1.2, 1.3, 1.5, 1.5, 1.5, 1.4],
  "eficiencia": [0, 40, 64, 68, 64, 50],
  "freqref": 60,
  "nestag": 60,
  "EficienciaMotor": 85
}]
```

## `multibcs` (array) — Multi-Curve BCS

Multiple pump curves in a single BCS installation (e.g., different pump stages in series).

| Field | Type | Description |
|-------|------|-------------|
| `id` | int | Identifier |
| `ativo` | bool | Active flag |
| `comprimentoMedido` | number | Position (m) |
| `tempo` | array | Time schedule (s) |
| `frequencia` | array | Frequency schedule (Hz) |
| `freqref` | number | Reference frequency (Hz) |
| `EficienciaMotor` | number | Motor efficiency (%) |
| `FrequenciaMinima` | number | Min frequency (Hz) |
| `curva` | array | Array of pump curve objects |

Each `curva[]` element:
| Field | Type | Description |
|-------|------|-------------|
| `vazao` | array | Flow rate points (m³/d) |
| `head` | array | Head per stage (m) |
| `potencia` | array | Power per stage (kW) |
| `eficiencia` | array | Efficiency (%) |
| `nestag` | int | Number of stages |
| `nestagFab` | int | Factory stages |

## `bombaVolumetrica` (array) — Volumetric Pumps

Positive displacement pumps.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position |
| `tempo` | array | s | Time schedule |
| `frequencia` | array | Hz | Frequency at each time |
| `capacidade` | number | m³/d | Pump capacity at reference frequency |
| `fatorpoli` | number | — | Polytropic factor |

```json
"bombaVolumetrica": [{
  "comprimentoMedido": 4020,
  "tempo": [0, 500, 560, 7000, 7005, 30500, 30560],
  "frequencia": [0, 0, 60, 60, 0, 0, 60],
  "capacidade": 2000,
  "fatorpoli": 1.4
}]
```

## `deltaPressao` (array) — Pressure Differential Devices

Generic device imposing a time-dependent pressure drop.

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `id` | int | — | Identifier |
| `ativo` | bool | — | Active flag |
| `comprimentoMedido` | number | m | Position |
| `tempo` | array | s | Time schedule |
| `deltaPressao` | array | kgf/cm² | Pressure differential at each time |
| `tipoCompGas` | int | — | Gas compression type |
| `fatPoli` | number | — | Polytropic factor |
| `eficLiq` | number | — | Liquid efficiency |
| `eficGas` | number | — | Gas efficiency |
