# General Configuration

The general configuration block governs the overarching setup of your simulation — from physical modeling choices and flow conventions to core numerical settings and performance tweaks.

These parameters define simulation modes, solver behaviors, and the relationship between geometry, physics, and available fluid and thermal models.

> **JSON key:** `initialConfig` (EN) · `configuracaoInicial` (PT) — top-level object

---

## 1. System Declaration

At the root level, declare the system role:

- `system` (EN) · `sistema` (PT):
  - `"PROD"` / `"INJ"` (EN)
  - `"MULTIFASICO"` / `"INJETOR"` (PT)

Defines producer vs injector top-level logic.

---

## 2. Simulation Mode

These two flags are the most fundamental controls in the entire configuration.

- `transient` · `transiente`:
  `true` = transient simulation; `false` = steady-state (default).

- `initialCondition` · `condicaoInicial`:
  - `0`: user-defined initial conditions
  - `1`: from steady-state solution (default)
  - `2`: from snapshot file — requires `snapshotFile` · `SnapShotArq` to be provided
  - `3`: gas-lift unloading mode — see [Section 12](#12-gas-lift-unloading) for details

> For full time-stepping and transient control options, see [time.md](time.md).

---

## 3. Simulation Geometry & Flow Direction

- `geometryFollowsFlow` · `sentidoGeometriaSegueEscoamento`:
  If `true`, indices increase in the flow direction (`0` = pipe inlet, default).
  If `false`, indices increase against the flow; angles are still measured with respect to the flow direction for compatibility with Marlim 2.

- `gasLine` · `linhaGas`:
  If `true`, includes a gas lift/service line.

- `xyMode` · `modoXY`:
  If `true`, pipe inclinations are inferred from XY coordinates of each pipe segment endpoint.
  Provide:
  - `xProdStart` · `xProdInicio`, `yProdStart` · `yProdInicio` (for production line)
  - `xServiceStart` · `xServInicio`, `yServiceStart` · `yServInicio` (for service/gas line, when `gasLine: true`)

  If `false`, inclinations are specified explicitly via angles in pipe segment definitions.

  - `checkValve` · `CheckValve`:
  `1` = a check valve is placed at the production system outlet, preventing reverse flow at the last boundary when indicated by pressure differential.
  `0` = reverse gas inflow at the system boundary is allowed (default).

---

## 4. Multiphase Model & Solver Controls

### Core Physics Flags

- `steadyStateSlip` · `escorregamentoPermanente`
- `transientSlip` · `escorregamentoTransiente`

Both control whether to enable slip (phase velocity difference) modeling in the steady-state and transient solvers, respectively.
**Default:** `true` for both.

- `driftModel` · `tipoModeloDrift`
  `true` = Drift-flux closure (default; always enforced in transient — setting `false` in transient mode is silently ignored).
  `false` = Black-box correlations (steady-state only), defined in `productionPipe.ssFlowModel` · `dutosProducao.correlacaoMR2`.

---

### Mass Transfer Model

- `massTransfer` · `transferenciaMassa`:

  - `0`: Full model, implicit numerical method (more stable — recommended default)
  - `1`: Full model, explicit numerical method (same physics as `0`, but less stable)
  - `2`: Simplified isothermal model, without transient mass-transfer terms
  - `3`: No mass transfer

---

### Flow-Pattern Map and Correlation Selection

- `flowPatternMap` · `mapaArranjo`
  `0` = Barnea simplified
  `1` = Barnea complete

- `correlationsByPattern` · `correlacoesPorArranjo`

  Per-pattern slip correlation selection:

  - `stratified` · `estratificado`
    `0`: Choi et al; `1`: Bhagwat & Ghajar (default); `2`: Franca & Lahey; `4`: Modified Bhagwat & Ghajar

  - `slugBubble` · `bolhaGolfada`
    `0`: Choi et al; `1`: Bhagwat & Ghajar (default); `4`: Modified Bhagwat & Ghajar

  - `annularChurn` · `anularChurn`
    `0`: Choi et al; `1`: Bhagwat & Ghajar (default); `3`: Hibiki & Ishii (annular); `4`: Modified Bhagwat & Ghajar

---

## 5. Thermal and Fluid Properties Controls

- `thermalEquilibrium` · `equilibrioTermico`
  If `true` (default), temperature is linearly interpolated between ambient and fluid across the pipe wall cross-section.
  If `false`, ambient temperature is used throughout the entire wall cross-section.

- `latentHeatCond` · `condlatente`
  If `false`, disables latent heat in condensation mass-transfer processes in the mixture energy equation.
  Useful in rare extreme retrograde-condensation cases (exotic oils, mainly with compositional models).
  **Default:** `true`.

- `steadyStateOrder` · `ordemperm`
  `1` = First order (default, robust)
  `2` = Runge-Kutta second order (increased accuracy; steady-state only — transient always uses first order)

- `steadyGuess` · `chutePerm`
  Initial guess for the steady-state solver (pressure or flow rate, depending on boundary conditions).
  If not provided or set to a negative value, the code computes an internal guess from hydrostatics (and friction when a non-IPR mass source is present).

- `reverseTemp` · `tempReves`
  Temperature to use for reverse gas inflow at the last system boundary in transient simulations.
  If not provided, ambient temperature is assumed.

- `initialFluidId` · `iniFluidoP`
  ID of the fluid that initially fills the production system.
  Only relevant when `initialCondition: 0` (user-defined initial conditions).
  If not provided, defaults to `0`.


---

## 6. Fluid Modeling Flags

Fluid property modeling follows a strict priority chain:

```
flashTableFluidModel: true  →  uses PVT flash table
        ↓ (if false)
compositionalFluidModel: true  →  uses full compositional model
        ↓ (if false)
black-oil model (default)
```


Available flags:

- `flashTableFluidModel` · `modeloFluidoTabelaFlash` — PVT flash table model
- `compositionalFluidModel` · `modeloFluidoComposicional` — full compositional model (only evaluated when `flashTableFluidModel` is `false`)
- `fluidProp` · `propFluido` — legacy hybrid model: black-oil-dominant, but with some properties (enthalpies, optionally phase Cp, and liquid Joule-Thomson) sourced from PVTSim files. `0` = black oil (default); `1` = hybrid
- `latentHeat` · `latente` — if `true`, uses PVT table interpolation for black-oil enthalpy; if `false`, uses the black-oil model directly. Requires a PVTSim file (`pvtFile`)
- `pvtFile` · `pvtsimArq` — name of the `.tab` or `.ctm` PVT file (valid for producer wells)
- `cpModel` · `modeloCp` — black-oil Cp calculation: `0` = black-oil model (default); `1` = PVT table interpolation
- `jtlModel` · `modeloJTL` — if `1`, uses d(ρ_l)/dT from a PVTSim table even when black-oil model is selected
- `dynamicTableModel` · `modeloTabelaDinamica` — generates flash property tables post-hoc from a black-oil run using the compositional model, for high-fidelity multi-segment simulations; valid for steady-state compositional only; can significantly reduce runtime in network cases
- `fluidType` · `tipoFluido` — `0`: liquid-dominated fluid (mass-transfer models tied to black-oil logic); `1`: gas-dominated fluid (mass-transfer models derived from quality/void-fraction relations)

> For detailed guidance on fluid model selection, PVT file setup, and compositional workflows, see [fluids.md](fluids.md).

---

## 7. Table Lookup, Tracking & Performance

These keywords enable data-driven performance improvements:

- `pressureTable` · `tabP`:
  Precomputes compressibility tables for black-oil estimation, indexed by reduced P/T (improves speed in long transient runs).

- `gasTable` · `tabG`:
  Same as `pressureTable`, but for gas in the service line.

- `RsPbTable` · `tabelaRSPB`:
  Pre-builds a solubility-ratio (RS) table using the black-oil model before simulation start.
  Especially beneficial when using Livia Fulchignoni's RS model, which is computationally expensive.

- `trackGOR` · `trackRgo`:
  Enables tracking of GOR (and associated stream properties such as API and BSW) along the pipeline using user mixing rules when streams merge.
  **Default:** `true`.

- `trackGasDensity` · `trackDensidadeGas`:
  Tracks gas density (and other variables such as CO2 fraction) along the system using mixing rules.
  **Default:** `true`.

- `freeGasDensityCorrectionBO` · `correcaoDenGasLivreBlackOil`:
  If `false` (default), in-situ density is calculated from standard conditions.
  If `true`, accounts for in-situ deviation due to higher pressure, resulting in release of lighter hydrocarbons.

---

## 8. Sensitivity analysis

- `sensitivityAnalysis` · `AS`:
  If `true`, runs steady-state sensitivity analysis using the `leituraAS.json` file.

- `parallelizeSA` · `paralelizaAS`:
  If `true`, parallelizes sensitivity analysis. Requires `sensitivityAnalysis: true` to be meaningful.

---

## 9. Formations & Lithology

For simulations involving heat exchange with surrounding rock, use:

> **JSON key:** `initialConfig.formation` (EN) · `configuracaoInicial.Formacao` (PT)

Provide rock thermal properties (conductivity, specific heat, density — all per rock ID) and production time for heated-radius calculation.

> For full field descriptions, units, and usage guidance, see [rock-formation.md](rock-formation.md).

---

## 10. Boundary Objects

**Pressure and flow boundary conditions** are formally specified as:

- `pressureCondition` · `condicaoPressao` — pressure boundary at pipeline inlet
- `flowRatePressureCondition` · `condicaoVazPres` — combined flow rate and pressure boundary at pipeline inlet (steady-state only; fully determines the system from inlet, ignoring any outlet boundary condition)

Populate each with relevant vectors:
`time` · `tempo`, `pressure` · `pressao`, `temperature` · `temperatura`, and others as applicable.

> **Note:** If neither boundary object is declared, the pipeline is treated as closed, and fluids must enter through internal mass sources (`massSource`, `IPR`, etc.).

> For full field descriptions, units, and usage guidance, see [boundary-conditions.md](boundary-conditions.md).

---

## 11. Output & Display

- `classicOutput` · `saidaClassica`:
  Cosmetic flag for the style of final text output (no impact on results).

- `screenPrint` · `saidaTela`:
  If `true`, shows detailed simulation progress (time, time step, variables).
  If `false` (default), only the completion percentage is shown.

---

## 12. Gas-Lift Unloading

Activated when `initialCondition: 3`. This mode simulates the unloading of completion fluid from the well using gas-lift injection.

### Interface Positions

- `gasLineInterfaceLength` · `comprimentoMedidoInterfaceLinhaGas`:
  Initial position (referenced from platform) of the gas/completion-fluid interface in the service line. Unit: m.

- `prodLineInterfaceLength` · `comprimentoMedidoInterfaceLinhaProd`:
  Initial position (referenced from platform) of the gas/completion-fluid interface in the production line. Unit: m.

- `fluidSalinity` · `SalinidadeFluido`:
  Salinity of the completion fluid used in the unloading simulation. Unit: g/(kg water).

### Automated Unloading Control

- `dischargeControl` · `controleDescarga`:
  If `true`, enables automated unloading control — gas injection is regulated to avoid erosional velocities at gas-lift valves during the unloading process.

When `dischargeControl: true`, populate `dischargeParameters` · `parametrosDescarga`:

- `maxDischargeFlowRate` · `vazaoLimiteDescarga`: Maximum liquid flow rate at valve (erosional limit). Unit: sm³/d
- `maxDischargePressure` · `pressaoLimiteDescarga`: Maximum downstream pressure at production choke during control. Unit: kgf/cm²
- `minDischargePressure` · `pressaoMinimaDescarga`: Minimum downstream pressure at production choke during control. Unit: kgf/cm²
- `workGasChargePressure` · `pressaoTrabalhoDescargaGas`: Choke downstream pressure at which unloading starts. Unit: kgf/cm²
- `maxGasChargePressure` · `pressaoLimiteDescargaGas`: Maximum injection pressure during control. Unit: kgf/cm²
- `minGasChargePressure` · `pressaoMinimaDescargaGas`: Minimum injection pressure during control. Unit: kgf/cm²
- `initialGasDischargePressure` · `pressaoInicialDescargaGas`: Initial gas line pressure at unloading start. Unit: kgf/cm²
- `dischargeTemperature` · `temperaturaDescarga`: Temperature at which unloading is performed. Unit: °C
- `latencyTime` · `tempoLatencia`: Time between simulation stabilization and unloading start. Unit: s

---

## 13. Advanced Block

Fine-tuning for convergence, solver stability, and rare or edge-case configurations.

> **JSON key:** `initialConfig.advanced` (EN) · `configuracaoInicial.Avancado` (PT)

Only tune after baseline results are established. Change one parameter at a time and always compare against a known working result.

---

### Single-Phase & Condensation Thresholds

- `monophasicCriterion` · `CriterioMonofasico`
  **Default:** `1e-4`
  Minimum void fraction below which the system is treated as single-phase. Very small void fractions can cause numerical problems. Avoid excessively high values — they may artificially remove mass from the system. Valid for transient simulations.

- `condensationCriterion` · `CriterioCondensacao`
  **Default:** `1e-3`
  Minimum void fraction for applying interphase mass-transfer modeling. Very small void fractions can drive condensation behavior that violates physical holdup limits, forcing very restrictive time-step corrections. Avoid high values — they can cause numerical instability or poor physical representation. Valid for transient simulations.

---

### Steady-State Solver Controls

- `accelerateSteadyConvergence` · `AceleraConvergPerm`
  **Default:** `true`
  Enables a simplification in the steady-state solver that accelerates convergence and improves stability. This is the current simulator default. In practice, there is rarely a need to disable this accelerator.

- `steadyConvergenceCriterion` · `CriterioConvergPerm`
  **Default:** `0.001`
  Convergence criterion for the rigorous steady-state solver mode. Currently less used due to `accelerateSteadyConvergence`.

- `regulaFalsiSearchCriterion` · `CriterioBuscaFalsaCorda`
  **Default:** `0.1`
  Increment/decrement fraction applied to the objective-function input while searching for the second point in steady-state false-position solving. In high-IPR scenarios (common in pre-salt cases), reducing this value can help avoid numerical issues caused by excessively high flow rates. Valid for steady-state simulations.

- `strongAnnularColCoupling` · `acopColAnulPermForte`
  **Default:** `0`
  Number of pseudo-transient steps applied to obtain higher numerical resolution of the temperature profile just below the ANM, in both annulus and production column. The steady-state model generally does not capture heat exchange well in this region. Valid for steady-state only.

- `counterflowCorrectionSteady` · `correcaoContracorPerm`
  **Default:** `false`
  When `true`, applies a correction to gas temperature entering the annulus using an approximate countercurrent heat-exchange model with column flow. Note: in transient mode, this countercurrent exchange is modeled with full fidelity.

- `correctSepCondition` · `corrigeContSep`
  **Default:** `true`
  Corrects the error introduced in the marching objective function when comparing pressure at the center of the last cell against separator pressure at the cell end.

---

### Pressure Frontier & Boundary Cell

- `simplePressureFrontier` · `MedSimpPresFront`
  **Default:** `true`
  Cell-boundary pressure calculation mode.
  `true`: average of adjacent cell-center pressures (default, stable).
  `false`: full method with hydrostatic and friction marching from cell center to boundary. Watch for numerical instabilities when set to `false`. Valid for transient simulations (steady-state always uses marching).

- `slipBoundaryCell` · `escorregamentoCelulaContorno`
  **Default:** `true`
  Controls slip modeling in the last production-system control volume. Oscillations may occur there due to high-frequency flow-pattern changes, common at low pressure. Disabling slip mitigates this. Watch for nonphysical holdup variations in the last cell when doing so.

---

### Timestep Controls

- `relaxChokeTimestep` · `RelaxaDTChoke`
  **Default:** `false`
  Penalizes time-step growth when oscillations occur in liquid flow entering and leaving the surface choke.

- `valveTimestepControl` · `controleDTvalv`
  **Default:** `false`
  Applies a time-step restriction during valve opening/closing events to avoid numerical oscillations.

- `disablePenalizeTimestep` · `desligaPenalizaDT`
  **Default:** `false`
  Disables time-step penalization triggered by abrupt holdup oscillations, especially during segregation processes typical of production shutdown. ⚠️ Do not disable unless you fully understand the trade-offs.

---

### Latent Heat Limiting

- `massTransferLimit` · `limTransMass`
  **Default:** `10.0` kg/(s·m)
  Disables latent heat in interphase mass-transfer processes (condensation or evaporation) in the mixture energy equation at points where interphase mass transfer exceeds this limit. Like `latentHeatCond`, useful in rare extreme retrograde-condensation scenarios (exotic oils, mainly in compositional models).

---

### Full Model (Liquid Density Time Derivatives)

These parameters control when the full model — which includes time derivatives of liquid specific mass — is active. The simplified model (without those derivatives) reduces computational cost.

- `compModelCorrectionTime` · `TcorrecaoModComp`
  Array of time instants (unit: s) at which the full/simplified model switch is evaluated.

- `compModelCorrectionFlag` · `correcaoModComp`
  Array of booleans matching `compModelCorrectionTime`; `true` activates the full model at the corresponding instant.

- `despressRate` · `taxaDespre`
  **Default:** `0.01` kgf/(cm²·s)
  If the average depressurization rate along the production system is below this value, only one evolution step is used (simplified model), reducing runtime. Only applied when at least one element of `compModelCorrectionFlag` is `true`. Valid for transient simulations.

- `columnStabilization` · `estabCol`
  **Default:** `false`
  Uses the simplified model (no liquid density time derivatives) exclusively in the production column, to keep it numerically stable in scenarios with intelligent completion valves. Valid for transient mode.

---

### Sonic Phenomena

- `sonicTime` · `Tsonico`
  Array of time instants (unit: s) paired with `sonicFlag`, determining when the simulator uses sufficiently small time steps (sonic CFL condition) to capture pressure-wave phenomena.

- `sonicFlag` · `sonico`
  Integer array matching `sonicTime`; non-zero activates sonic time-stepping at the corresponding instant.

> ⚠️ Marlim3 uses first-order models and has limited resolution for sonic phenomena. These parameters provide a mitigation mechanism only — they do not guarantee accurate sonic representation.

---

### Compositional Transient Mini-Tables

These parameters are applicable to transient simulations with compositional fluid models only.

- `dynTableMinDelay` · `miniTabDinAtraso`
  **Default:** `0`
  Every N time steps (as defined here), a local flash mini-table is generated per cell, storing P and T flash data using the increments defined below.

- `dynTableMinDp` · `miniTabDinDp`
  **Default:** `0` kgf/cm²
  Pressure increment/decrement used when generating flash mini-tables.

- `dynTableMinDt` · `miniTabDinDt`
  **Default:** `0` °C
  Temperature increment/decrement used when generating flash mini-tables.

---

### Performance & Threading

- `threads` · `nthrd`
  **Default:** `1`
  Number of threads used for simulation execution.

- `matrixThreads` · `nthrdMatriz`
  **Default:** `1`

---

### Area Changes & Misc

- `areaChange` · `mudaArea`
  **Default:** `0`
  If `1`, the simulator introduces localized pressure losses for each cross-sectional area change from one cell to the next.

- `liquidJTSimple` · `JTLiquidoSimple`
  **Default:** `0`
  If `1`, disables the d_ρ/dT term in the liquid Joule-Thomson calculation. Useful for legacy cases converted from Marlim 2, especially in the production column.

- `disableMassTransferTempDeriv` · `desligaDeriTransMassDTemp`
  **Default:** `false`

- `minTimestepCriterion` · `CriterioDTMin`
  Minimum allowable time step. Leave unset unless you have a specific reason to enforce a hard lower bound.

---

## 14. Special Features

- `diffusion3dMode` · `modoDifus3D`: If `true`, activates the 3D thermal diffusion model.
- `diffusion3dThreads` · `threadP3D`: Number of threads for 3D diffusion calculations. **Default:** `1`.
- `diffusion3dJson` · `modoDifus3DJson`: Name of the JSON file defining the 3D thermal diffusion model.
- `waxMode` · `modoParafina`: If `true`, activates the wax-deposition model.

See schema for further detail on all of the above.

---

## Example

```json
{
  "system": "PROD",
  "initialConfig": {
    "transient": false,
    "initialCondition": 1,
    "steadyStateSlip": true,
    "transientSlip": true,
    "driftModel": true,
    "flowPatternMap": 0,
    "massTransfer": 0,
    "steadyStateOrder": 1,
    "checkValve": 0,
    "geometryFollowsFlow": true,
    "gasLine": false,
    "pressureTable": true,
    "correlationsByPattern": {
      "stratified": 1,
      "slugBubble": 1,
      "annularChurn": 1
    },
    "advanced": {
      "threads": 1,
      "massTransferLimit": 10.0,
      "simplePressureFrontier": true,
      "accelerateSteadyConvergence": true
    }
  }
}
```

> **Tip:** Keep advanced and optional features at their defaults until your first baseline results are validated. When tuning for performance, stability, or physics fidelity, change only one setting at a time — always compare against your previous working result.

---

## Practical Guidance

- **Performance:** Use precomputed tables (`pressureTable`, `gasTable`) for computationally intensive transient cases.

- **Slip & drift:** Leave slip enabled and use the drift model unless you have robust justification otherwise.

- **Boundary conditions:** Always declare at least one inlet/outlet boundary object, or set up internal source(s). An undeclared boundary means a closed system, which will raise an error.

- **Sanity checks:** Do not deactivate penalization or stability features (`disablePenalizeTimestep`, `simplePressureFrontier: false`, etc.) unless you truly understand the trade-offs.