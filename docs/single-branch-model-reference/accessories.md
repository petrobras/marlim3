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

### ICV (Interval Control Valve) on IPR

When `ICV` is `true`, the IPR distinguishes between the bottomhole pressure at the ICV and the annular pressure that actually defines the drawdown. The ICV acts as a controllable choke coupled to the IPR source, enabling time-varying opening schedules.

> The ICV fields live **inside the IPR object**.

ICV fields:

- `ICV` · `ICV` — set to `true` to activate the ICV on this IPR
- `icvTime` · `tempoICV` — time instants for ICV opening events [s]; the EN key `icvTime` follows the convention of other time-array IPR fields but is not yet present in the EN schema
- `opening` · `abertura` — opening values over time (area ratio or stem displacement, depending on `cvCurve`)
- `cd` · `cd` — discharge coefficient (default 0.84)
- `cvCurve` · `curvaCV`
  - `0` = opening values are area ratios
  - `1` = opening values are stem displacement
- `x1` · `x1` — CV curve abscissa (stem displacement percentage); valid for `cvCurve = 1`
- `cv1` · `cv1` — CV curve ordinate (valve coefficient in (gal/min)/(psi)); valid for `cvCurve = 1`

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
  - `0` = gas fraction computed by equilibrium at in-situ conditions (based on the hydrocarbon-rate difference `totalMassFlowRate` − `complementaryMassFlowRate`)
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
- `gasAmbient` · `ambGas` — admit only gaseous fraction of external fluid

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
- `chkTime` · `tempoChk` — time series for check-valve opening

### Recirculation Mode

When `recirculate` / `recircula` is `true`, the pressure source acts as a recirculation system. `measuredLength` indicates the downstream tap (source of recirculated fluid) and `recirculationMeasuredLength` / `comprimentoMedidoRecircula` indicates the upstream tap (destination). Flow direction is downstream-to-upstream (e.g., pump discharge to pump suction). The ambient fluid is always the fluid at the recirculation point. Reverse flow can be prevented using `check`.

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
- `vglDischCoef` · `cdvgl` — discharge coefficient, single-phase model (default 0.84)
- `liquidDischCoef` · `cdvLiq` — discharge coefficient for the liquid-flowing (gas-lift discharge) case (default 0.9)
- `areaRatio` · `razaoArea` — port-to-diaphragm area ratio, for pressure-calibrated valves (`valveType = 1`), based on the Marlim 2 VGL model
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
- `dynamicCurve` · `curvaDinamic` — when set to `1`, emulates dynamic actuator behavior responding to line pressure, actuator pressure, spring stiffness, and actuator mass

### Valve Box (Caixa de Válvula)

Both `valve` and `masterValve` support a *valve box* feature. When `valveBox` is `true`, the confined region between two hypothetical valves (upstream and downstream) is modeled with a simple integral balance when both are closed, tracking pressure, temperature, quality, and void fraction. When the valve box is active, the main valve's `time` and `opening` arrays are derived from the upstream/downstream hypothetical valves — the smaller opening of the two is used.

> The valve-box fields live **inside the valve or masterValve object**.

Valve-box fields:

- `valveBox` · `caixaValvula` — set to `true` to activate the valve box
- `boxLength` · `compCaixa` — length of the confined region between valves [m]
- `crossSectionIndex` · `indSecTrans` — index of the cross-section used for the confined region
- `environment` · `ambiente` — index of the external environment for heat exchange in the confined space
- `ambientTemp` · `tempAmbiente` — ambient temperature for heat exchange [degC]
- `ambientVel` · `velAmbiente` — external velocity for film coefficient [m/s]
- `formation` · `formac` — set to `1` if the confined space contacts a formation (then use `lito`); `0` for external environment
- `lithology` · `lito` — index into the `formation` array for formation properties (when `formac = 1`); **not present in the `masterValve` valve-box block** (see note)
- `upstreamOpening` · `aberturaMon` — opening series of the hypothetical upstream valve
- `upstreamOpeningTime` · `tempoAberturaMon` — time series for upstream valve [s]
- `downstreamOpening` · `aberturaJus` — opening series of the hypothetical downstream valve
- `downstreamOpeningTime` · `tempoAberturaJus` — time series for downstream valve [s]
- `sourceTemperature` · `temperaturaFonte` — temperature for mass sources in the confined space [degC]
- `sourceTime` · `tempoFonte` — time instants for mass sources [s]
- `massProdLiquid` · `massLiqP` — mass source of produced liquid in the confined space [kg/s]
- `massComplLiquid` · `massLiqC` — mass source of complementary liquid in the confined space [kg/s]
- `massGas` · `massGas` — mass source of gas in the confined space [kg/s]

!!! note
    In `branch.pt.json`, the valve-box block on `master1` (`masterValve`) is a slightly reduced set: it has no `lito` (lithology) field, and it is declared **outside** `properties`. Use the full field list above for `valve`; for `masterValve`, omit `lithology`.

### Production Master Valve

`masterValve` is the production Wet Christmas Tree valve.

> **JSON key:** `masterValve` (EN) · `master1` (PT)

Important details from schema:

- In steady-state it mainly defines position.
- In transient mode it should always be defined when WCT operation matters.
- Uses fixed discharge coefficient 0.84.
- Has `activeAreaRatio` · `razaoAreaAtiva` (default 0.4): an opening-ratio threshold. Below it the valve model is applied; above it the valve is treated as fully open. It is used to avoid numerical instabilities in transient mode (valid for the transient regime).

Main fields:

- `active`, `cvCurve`, `activeAreaRatio`
- `measuredLength`
- `time`, `opening`
- `x1`, `cv1`

### Service-Line Master Valve

`masterValve2` is the simplified service-line WCT valve.

> **JSON key:** `masterValve2` (EN) · `master2` (PT)

Behavior is simplified/binary compared with `masterValve`: the valve is open when `opening` is below 0.5 and closed when above 0.5.

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
- `dynamicCurve` · `curvaDinamic` — when set to `1`, emulates dynamic choke actuator behavior responding to line pressure, actuator pressure, and spring stiffness

### Injection Choke

`injectionChoke` is the service-line choke for single-phase gas.

> **JSON key:** `injectionChoke` (EN) · `chokeInj` (PT)

!!! note
    Unlike the surface choke, the injection choke does **not** use a CV curve (`cvCurve`) and the flow is always single-phase gas.

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
- `motorEfficiency` · `EficienciaMotor` — default 100%
- `minFrequency` · `FrequenciaMinima`
- `hiCorrection` · `correcHI` — Hydraulic Institute head-degradation correction; default true
- `motorHeatFraction` · `fracTermMotorEfic` — fraction of motor heat transferred to the fluid (affects thermal balance); default 0

### Volumetric Pump

Positive-displacement pump using an isochoric compression model.

> **JSON key:** `volumetricPump` (EN) · `bombaVolumetrica` (PT)

Main fields:

- `measuredLength` · `comprimentoMedido`
- `time` · `tempo`
- `frequency` · `frequencia`
- `capacity` · `capacidade`
- `polyFactor` · `fatorpoli`

### Multi-ESP (Stage-by-Stage)

`multiESP` models an ESP with internal stage-by-stage marching instead of simple multiplication by stage count. This corrects per-stage variables such as free-gas fraction, fluid temperature, viscosity effects, and head degradation. Multiple distinct curves can be defined within the assembly — useful for ESPs with axial initial stages (handling high gas fractions at suction) followed by radial stages.

> **JSON key:** `multiESP` (EN) · `multibcs` (PT)

Main fields:

- `measuredLength` · `comprimentoMedido`
- `time` · `tempo` — frequency schedule [s]
- `frequency` · `frequencia` [Hz]
- `referenceFreq` · `freqref` [Hz]
- `motorEfficiency` · `EficienciaMotor` [%], default 100
- `motorHeatFraction` · `fracTermMotorEfic` — motor heat to fluid; default 0
- `minFrequency` · `FrequenciaMinima` [Hz]
- `hiCorrection` · `correcHI` — Hydraulic Institute head-degradation correction; default true
- `thermalEquilibrium` · `equilTerm` — if `true`, gas dissolution in oil is considered per stage; if `false`, equilibrium is deferred until after the full march (useful for two-phase oil-gas streams)

The `curve` / `curva` array holds each stage group (ordered upstream to downstream):

- `flowRate` · `vazao` [BPD]
- `pumpHead` · `head` [ft]
- `power` · `potencia` [hp]
- `efficiency` · `eficiencia` [%]
- `stage` · `nestag` — number of stages in this group
- `manufacturerStage` · `nestagFab` — stages used when the manufacturer measured the curve; default 1

!!! note
    Unlike `esp` which multiplies a single-stage curve by `stage`, `multiESP` recalculates per-stage properties including free-gas title, fluid temperature, and viscosity corrections. For two-phase oil-gas streams, setting `thermalEquilibrium = false` prevents gas re-dissolution during the march, applying it only once at the outlet.

### Pressure Drop / Increment

`pressureDrop` applies a localized pressure increment over time. The liquid and gas efficiencies make it usable as an analogy for a volumetric pump: the total required power (defining the desired pump) is the sum of liquid pumping power and gas compression power.

> **JSON key:** `pressureDrop` (EN) · `deltaPressao` (PT)

Main fields:

- `pressureDrop` · `deltaPressao`
- `gasCompType` · `tipoCompGas`
  - `0` = adiabatic
  - `1` = polytropic
  - `2` = isothermal
- `polyFacOrAdiabConst` · `fatPoli`
- `liquidEfficiency` · `eficLiq` — default 100%
- `gasEfficiency` · `eficGas` — default 100%

---

## Pigging

`pig` defines launch and receive positions, launch time, and bypass parameters.

> **JSON key:** `pig` (EN/PT)

Main fields:

- `active` · `ativo`
- `id` · `id`
- `launcher` · `lancador` — launch position [m]
- `receiver` · `recebedor` — receive position [m]
- `time` · `tempo` — launch time [s]
- `gapArea` · `folgaArea` — gap area for fluid bypass around the pig
- `pigCD` · `cdPig` — discharge coefficient for flow through the pig gap
- `pressureDrop` · `deltaPressao` — pressure difference across the pig

---

## Heat Source

Localized heat source (or sink) placed at the nearest cell center to the given measured length. Positive values add heat to the fluid; negative values remove it.

> **JSON key:** `heatSource` (EN) · `fonteCalor` (PT)

Main fields:

- `measuredLength` · `comprimentoMedido`
- `time` · `tempo` — time instants [s]
- `heat` · `calor` — thermal power over time [kW]

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
