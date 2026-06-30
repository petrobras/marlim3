# Accessories

Accessories are localized devices and sources placed along the production or service system. They add inflow, leakage, restriction, pumping, gas lift, or pigging behavior that cannot be represented by boundary conditions alone.

This page focuses on **top-level accessory objects** such as sources, valves, pumps, and pigs. Endpoint objects like `separator`, `surfaceChoke`, and `gasInj` are described in [boundary-conditions.md](boundary-conditions.md), although they are closely related.

---

## Common Pattern

Most accessories share these concepts:

- `active` / `ativo` — enable or disable without removing the object
- `id` / `id` — unique identifier
- `measuredLength` / `comprimentoMedido` — location along the line
- `time` / `tempo` — event schedule for time-varying arrays

Not every accessory uses all of them. Some are arrays of objects (`valve`, `ipr`, `liquidSource`); some are single objects (`masterValve`, `surfaceChoke`).

---

## Reservoir and Inflow Sources

### IPR Source

The IPR object defines reservoir inflow as a function of drawdown between reservoir static pressure and tubing bottomhole pressure.

> **JSON key:** `ipr` (EN) · `ipr` (PT)

Core geometry and fluid reference:

- `prodFluidId` · `indiFluidoPro`
- `prodFluidIndex` · `indFluidoPro` (legacy alias)
- `measuredLength` · `comprimentoMedido`

Model selector:

- `iprType` · `tipoIPR`
  - `0` = linear IPR
  - `1` = combined Vogel
  - `2` = Vogel

Time-series groups:

- Static pressure: `staticPressureTime` / `tempoPressaoEstatica`, `staticPressure` / `pressaoEstatica`
- Reservoir temperature: `temperaturesTime` / `tempoTemperaturas`, `temperatures` / `temperaturas`
- Productivity index: `ipTime` / `tempoip`, `ip` / `ip`
- Maximum rate (Vogel): `qMaxTime` / `tempoqMax`, `qMax` / `qMax`
- Injectivity index: `iiTime` / `tempoii`, `ii` / `ii`

!!! note
    `ii` is relevant for reverse-flow and injection-related cases, not only conventional production.

### Liquid Source

Prescribed standard-condition liquid inflow. Associated free gas is inferred from the referenced production fluid and in-situ thermodynamics.

> **JSON key:** `liquidSource` (EN) · `fonteLiquido` (PT)

Main fields:

- `prodFluidId` · `indiFluidoPro`
- `measuredLength` · `comprimentoMedido`
- `time` · `tempo`
- `temperature` · `temperatura`
- `beta` · `beta` — complementary-fluid ratio
- `liquidFlowRate` · `vazaoLiquido`

### Mass Source

Prescribed total mass source with optional explicit gas split.

> **JSON key:** `massSource` (EN) · `fonteMassa` (PT)

Main fields:

- `prodFluidId` · `indiFluidoPro`
- `thermType` · `tipoTermo`
  - `0` = gas fraction computed by equilibrium at in-situ conditions
  - `1` = explicit gas mass flow must be provided
- `totalMassFlowRate` · `vazaoMassT`
- `complementaryMassFlowRate` · `vazaoMassC`
- `gasMassFlow` · `vazaoMassG` (only for `thermType = 1`)
- `temperature` · `temperatura`

### Gas Source

Prescribed gas inflow, either dry gas using `gasFluid` or rich gas tied to a production-fluid definition.

> **JSON key:** `gasSource` (EN) · `fonteGas` (PT)

Main fields:

- `dry` · `seco`
  - `true` = dry gas from `gasFluid`
  - `false` = rich gas linked to `prodFluidId`
- `prodFluidId` · `indiFluidoPro` (when `dry = false`)
- `gasFlowRate` · `vazaoGas`
- `complementaryFluidFlowRate` · `vazaoFluidoComplementar` (impurity fraction, ppm)
- `temperature` · `temperatura`

### Porous Sources

Two porous-source objects delegate configuration to external JSON files:

- `porousRadialSource` (EN) · `fontePoroRadial` (PT)
- `porous2DSource` (EN) · `fontePoro2D` (PT)

Both use:

- `measuredLength` · `comprimentoMedido`
- `file` · `arquivo`

---

## Pressure-Coupled Opening / Leak

### Pressure Source

`pressureSource` models a leak, opening, or exchange point between tubing and an external pressurized medium. Flow may go in either direction depending on local pressure difference and check setting.

> **JSON key:** `pressureSource` (EN) · `fontePressao` (PT)

Fluid definition:

- `fluidType` · `tipoFluido`
  - `1` = external hydrocarbon assumed equal to current tubing fluid
  - `0` = external fluid explicitly defined by `prodFluidId`
- `prodFluidId` · `indiFluidoPro`
- `gasAmbient` · `ambienteGas` — admit only gaseous fraction of external fluid

Flow-direction control:

- `check` · `check`
  - `0` = bidirectional opening
  - `1` = vacuum breaker (only environment to tubing)
  - `-1` = check-valve behavior (only tubing to environment)

Opening definition:

- `openingType` · `TipoAbertura`
  - `0` = opening given as area ratio
  - `1` = opening given as diameter ratio
- `opening` · `abertura`
- `time` · `tempo`
- `cd` · `cd`

External-medium state:

- `pressure` · `pressao`
- `temperature` · `temperatura`
- `beta` · `beta`
- `ambientFluidQuality` · `titAmb`

---

## Gas-Lift Valve Source (VGL)

### Gas-Lift Source

`gasLiftSource` defines a gas-lift valve connecting service line and production line. It is more detailed than a generic gas source because it needs both production-side and service-side tap positions and valve calibration data.

> **JSON key:** `gasLiftSource` (EN) · `fonteGasLift` (PT)

Geometry and tap positioning:

- `annulusColumnFlag` · `colunaEanular`
  - `true` = service tap is inferred at same elevation as production tap
  - `false` = user must give `serviceMeasuredLength`
- `prodMeasuredLength` · `comprimentoMedidoProducao`
- `serviceMeasuredLength` · `comprimentoMedidoServico`

Valve type:

- `valveType` · `tipoValvula`
  - `0` = orifice
  - `1` = pressure-calibrated valve
  - `2` = Venturi

Physical parameters:

- `orificeDiameter` · `diametroOrificio`
- `outerDiameter` · `diametroExterno` — port diameter for pressure-calibrated valve
- `vglDischCoef` · `cdvgl`
- `liquidDischCoef` · `cdvLiq`
- `areaRatio` · `razaoArea`
- `calibrationPressure` · `pressaoCalibracao` [psi]
- `calibrationTemperature` · `temperaturaCalibracao` [degF]

!!! note
    `annulusColumnFlag = true` is convenient when the service-line pressure tap should be automatically aligned by elevation with the production-side tap.

---

## Valves and Chokes

### Generic Two-Phase Valve

`valve` is an optional two-phase restriction using the Sachdeva model.

> **JSON key:** `valve` (EN) · `valvula` (PT)

Main fields:

- `cvCurve` · `curvaCV`
  - `0` = opening values are area ratios
  - `1` = opening values are stem displacement
- `measuredLength` · `comprimentoMedido`
- `time` · `tempo`
- `opening` · `abertura`
- `cd` · `cd`
- `x1`, `cv1` for stem-displacement calibration

### Production Master Valve

`masterValve` is the production Wet Christmas Tree valve.

> **JSON key:** `masterValve` (EN) · `master1` (PT)

Important details from schema:

- In steady-state it mainly defines position.
- In transient mode it should always be defined when WCT operation matters.
- Uses fixed discharge coefficient 0.84.
- Has `activeAreaRatio` · `razaoAreaAtiva` to avoid instability near fully open state.

Main fields:

- `active`, `cvCurve`, `activeAreaRatio`
- `measuredLength`
- `time`, `opening`
- `x1`, `cv1`

### Service-Line Master Valve

`masterValve2` is the simplified service-line WCT valve.

> **JSON key:** `masterValve2` (EN) · `master2` (PT)

Behavior is simplified/binary compared with `masterValve`.

### Surface Choke

`surfaceChoke` is a fixed-position outlet restriction analogous to a valve.

> **JSON key:** `surfaceChoke` (EN) · `chokeSup` (PT)

Main fields:

- `cvCurve` · `curvaCV`
- `time` · `tempo`
- `opening` · `abertura`
- `dischargeCoefficient` · `coeficienteDescarga`
- `model` · `modelo` (currently only 0 = Sachdeva)
- `x1`, `cv1`

### Injection Choke

`injectionChoke` is the service-line choke for single-phase gas.

> **JSON key:** `injectionChoke` (EN) · `chokeInj` (PT)

Main fields:

- `active` · `ativo`
- `time` · `tempo`
- `opening` · `abertura`
- `dischargeCoefficient` · `coeficienteDescarga`

---

## Pumps and Localized Pressure Devices

### ESP

The ESP object contains full curve data and speed scheduling.

> **JSON key:** `esp` (EN) · `bcs` (PT)

Main fields:

- `measuredLength` · `comprimentoMedido`
- `time` · `tempo`
- `frequency` · `frequencia`
- `flowRate` · `vazao` [BPD]
- `power` · `potencia` [hp]
- `efficiency` · `eficiencia`
- `pumpHead` · `head` [ft]
- `referenceFreq` · `freqref`
- `stage` · `nestag`
- `manufacturerStage` · `nestagFab`
- `motorEfficiency` · `EficienciaMotor`
- `minFrequency` · `FrequenciaMinima`
- `hiCorrection` · `correcHI`

### Volumetric Pump

Positive-displacement pump using an isochoric compression model.

> **JSON key:** `volumetricPump` (EN) · `bombaVolumetrica` (PT)

Main fields:

- `measuredLength` · `comprimentoMedido`
- `time` · `tempo`
- `frequency` · `frequencia`
- `capacity` · `capacidade`
- `polyFactor` · `fatorpoli`

### Pressure Drop / Increment

`pressureDrop` applies a localized pressure increment over time. It can also approximate pump behavior through gas-compression and phase-efficiency settings.

> **JSON key:** `pressureDrop` (EN) · `deltaPressao` (PT)

Main fields:

- `pressureDrop` · `deltaPressao`
- `gasCompType` · `tipoCompGas`
  - `0` = adiabatic
  - `1` = polytropic
  - `2` = isothermal
- `polyFacOrAdiabConst` · `fatPoli`
- `liquidEfficiency` · `eficLiq`
- `gasEfficiency` · `eficGas`

---

## Pigging

`pig` defines launch and receive positions and the launch time.

> **JSON key:** `pig` (EN/PT)

Main fields:

- `launcher` · `lancador`
- `receiver` · `recebedor`
- `time` · `tempo`

!!! note
    The current public schema documents launch position, receive position, and launch time. The parser contains some extra legacy/internal pig parameters, but they are not part of the public schema and are therefore intentionally not documented here.

---

## Practical Guidance

- Place accessories at physically meaningful measured lengths and verify geometry direction conventions before interpreting results.
- Introduce one active accessory at a time while validating a model.
- For time-varying valves/chokes, use ramps instead of instantaneous steps when possible.
- For gas-lift valves, verify production and service tap positions carefully; elevation consistency matters.
- For pressure sources, be explicit about allowed flow direction using `check`.
- For rich-gas sources (`dry = false`), ensure the linked production fluid is thermodynamically consistent with the scenario.

---

## Example: IPR Source

```json
{
  "ipr": [
    {
      "id": 0,
      "active": true,
      "prodFluidId": 0,
      "measuredLength": 2500.0,
      "iprType": 1,
      "staticPressureTime": [0],
      "staticPressure": [320.0],
      "temperaturesTime": [0],
      "temperatures": [85.0],
      "ipTime": [0],
      "ip": [12.0],
      "iiTime": [0],
      "ii": [1.0]
    }
  ]
}
```

## Example: Mass Source with Explicit Gas Split

```json
{
  "massSource": [
    {
      "id": 0,
      "active": true,
      "prodFluidId": 0,
      "measuredLength": 1500.0,
      "thermType": 1,
      "time": [0],
      "temperature": [60.0],
      "totalMassFlowRate": [20.0],
      "complementaryMassFlowRate": [0.0],
      "gasMassFlow": [2.0]
    }
  ]
}
```

## Example: Two-Phase Valve with Opening Ramp

```json
{
  "valve": [
    {
      "id": 0,
      "active": true,
      "measuredLength": 1500.0,
      "cvCurve": 0,
      "cd": 0.84,
      "time": [0, 60, 120],
      "opening": [0.0, 0.5, 1.0]
    }
  ]
}
```

## Example: ESP

```json
{
  "esp": [
    {
      "id": 0,
      "active": true,
      "measuredLength": 3000.0,
      "time": [0],
      "frequency": [60.0],
      "referenceFreq": 60.0,
      "stage": 100,
      "flowRate": [1000, 2000, 3000],
      "pumpHead": [200, 180, 140],
      "power": [20, 35, 50],
      "efficiency": [0.45, 0.60, 0.52]
    }
  ]
}
```

!!! tip
    When debugging a complex model, deactivate all but one accessory and validate the isolated effect before combining multiple controls, sources, and pumps.
