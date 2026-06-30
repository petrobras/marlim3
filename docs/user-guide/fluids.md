# Fluids

Fluid modeling is the thermodynamic foundation of the simulation. Nearly every governing equation — momentum, energy, mass transfer — depends on fluid properties: density, viscosity, enthalpy, compressibility, and phase split. If fluid data is inconsistent, pressure and temperature predictions degrade quickly.

## Thermodynamic Model Family

Marlim3 supports three approaches to compute fluid properties, each with different fidelity and numerical cost:

### Black-Oil Model

The simplest and fastest approach. Fluid behavior is characterized by correlations that depend on API gravity, GOR, gas density, and BSW. Phase split is determined by a solution-gas-ratio (RS) correlation.

### Table File

Precomputed fluid properties from external software (PVTsim, Multiflash) stored in `.tab` files. The simulator interpolates properties at each P-T condition from this table. Accurate within the table range but depends entirely on the quality and coverage of the external data.

### Compositional Model

A rigorous thermodynamic model where the fluid is described by pseudocomponent molar fractions and equilibrium is computed via flash calculations. Required for rich-gas/condensate systems or when composition changes significantly along the simulation. More expensive computationally.

> **JSON keys (inside global config):**
>
> - Flash-table mode: `flashTableFluidModel` (EN) · `modeloFluidoTabelaFlash` (PT) — default `false`
> - Compositional mode: `compositionalFluidModel` (EN) · `modeloFluidoComposicional` (PT) — default `false`
> - PVT file: `pvtFile` (EN) · `pvtsimArq` (PT)

When both flags are `false`, the black-oil model is used. When both flags are `true`, the flash-table model is used.



**File format convention:**

- **Flash-table mode:** use a `.tab` file in `pvtFile` / `pvtsimArq`.
- **Compositional mode:** use a `.ctm` file in `pvtFile` / `pvtsimArq`.

!!! note
    The schema accepts `.tab` or `.ctm` at key level, but model selection defines the practical file type: flash tables read tabulated `.tab` data, while compositional runs read pseudocomponent data from `.ctm`.

## Quick Decision Flow

1. **Choose the thermodynamic model**
  - Fast engineering studies: black-oil
  - Tabulated PVT interpolation: flash-table
  - High-fidelity compositional behavior: compositional
2. **Set model flags in `initialConfig`**
  - Black-oil: `flashTableFluidModel = false`, `compositionalFluidModel = false`
  - Flash-table: `flashTableFluidModel = true` (compositional flag is ignored)
  - Compositional: `flashTableFluidModel = false`, `compositionalFluidModel = true`
3. **Use the appropriate PVT file in `pvtFile`**
  - Flash-table: `.tab`
  - Compositional: `.ctm`

### Minimum Input Checklist by Mode

| Mode | Must-have keys | Optional/conditional keys |
|------|----------------|---------------------------|
| Black-oil | `productionFluid` defined with black-oil properties, `gor`, `api`, `gasDensity` | `RsPbModel`, viscosity and emulsion model selectors |
| Flash-table | `productionFluid` defined, `flashTableFluidModel = true`, `pvtFile` with `.tab` | `blackOilViscModel`, `blackOilWaterModel`, viscosity and emulsion model selectors |
| Compositional | `productionFluid` defined, `compositionalFluidModel = true`, `pvtFile` with `.ctm` | `userMolarFraction`/`molarFraction`, `userGORComp` for composition correction, viscosity and emulsion model selectors |

!!! note
   In flash-table mode, correlation keys such as `deadOilModel`, `liveOilModel`, and `undersaturatedOilModel` are only active when `blackOilViscModel = 1`.

!!! note
    In compositional mode, black-oil correlations are used to compute gas viscosities, therefore black oil properties are needed.

---

## Production Fluid

Each simulation can define one or more production fluids. Sources, initial conditions, and mixing rules reference fluids by their integer identifier.

> **JSON key:** `productionFluid` (EN) · `fluidosProducao` (PT) — array of objects

### Property Applicability by Fluid Model

There is no single universal “required set” for all modes. Requirements depend on whether the run is black-oil, flash-table, or compositional, and on override flags.

| Property / option | Black-oil | Flash-table | Compositional | Notes |
|-------------------|-----------|-------------|---------------|-------|
| `gor` / `rgo` | used | used | used | Valid in all models. In compositional mode, if `userGORComp` is true, original molar fractions are corrected to match this value. In table mode, if this value is below the table GOR value, this value is used; if above, the excess is treated as additional free-gas mass (added in the table RS variable). |
| `bsw` | used | used | used | Valid in all models |
| `waterDensity` / `densidadeAgua` | used | used | used | Valid in all models |
| `emulsionType` and emulsion parameters | used | used | used | Valid in all models |
| `api` | used | not primary | not primary | Black-oil only |
| `RsPbModel` / `modeloRsPb` | used | not used | not used | Black-oil RS correlation |
| `gasDensity`, `CO2Fraction`, `criticalCorrelation` | used | conditional | used | In flash-table mode, mainly relevant when black-oil correlations are enabled |
| `deadOilModel`, `liveOilModel`, `undersaturatedOilModel` | used | conditional | used | In flash-table mode, used only with `blackOilViscModel = 1` |
| `temp1`, `visc1`, `temp2`, `visc2`, `deadOilTemp`, `deadOilVisc` | conditional | conditional | conditional | Needed only for selected viscosity models |
| `molarFraction`, `userMolarFraction` | not used | not used | optional | Compositional-only keys, used to input molar fractions not present in the CTM file |
| `userGORComp` / `RGOCompUsuario` | not used | not used | optional | Corrects composition to match GOR |

!!! note
  Most schema fields are optional, as they rely on preprogrammed defaults.

### Dead-Oil Viscosity

Dead-oil viscosity is the foundation of the viscosity model — it represents oil viscosity at atmospheric pressure with no dissolved gas. Several empirical correlations are available:

| Option | Correlation | Notes |
|--------|-------------|-------|
| 0 | ASTM | Requires two temperature-viscosity points |
| 1 | Beggs & Robinson | — |
| 2 | Modified Beggs & Robinson | — |
| 3 | Glaso | Default |
| 4 | Kartoatmodjo-Schmidt | — |
| 5 | Petrosky-Farshad | — |
| 6 | Beal | — |
| 7 | User table | Requires temperature and viscosity arrays |

For the ASTM model (0), provide two calibration points: temperature and viscosity at each point.

> **JSON keys:** `deadOilModel` (EN) · `modeloOleoMorto` (PT)
> ASTM points: `temp1`, `visc1`, `temp2`, `visc2`
> Table: `deadOilTemp` / `tempOleoMorto`, `deadOilVisc` / `viscOleoMorto`

### Live-Oil and Undersaturated-Oil Viscosity

Live-oil viscosity account for dissolved gas in situ. Undersaturated-oil viscosity accounts for pressures above bubble point.

**Live-oil models:**

| Option | Correlation |
|--------|-------------|
| 0 | Beggs & Robinson (default) |
| 1 | Kartoatmodjo-Schmidt |
| 2 | Petrosky-Farshad |

**Undersaturated-oil models:**

| Option | Correlation |
|--------|-------------|
| 0 | Vasquez & Beggs (default) |
| 1 | Kartoatmodjo-Schmidt |
| 2 | Petrosky-Farshad |
| 3 | Beal |
| 4 | Khan |

> **JSON keys:**
>
> - Live oil: `liveOilModel` (EN) · `modeloOleoVivo` (PT)
> - Undersaturated: `undersaturatedOilModel` (EN) · `modeloOleoSubSaturado` (PT)

!!! note
    In flash-table and compositional modes, viscosity correlations are only used when `blackOilViscModel` / `modeloViscBlackOil` is set to `1`.

### Solution Gas-Oil Ratio (Bubble-Point Behavior)

The solution gas-oil ratio (Rs) determines how much gas is dissolved in oil at a given pressure and temperature. At the bubble point, Rs equals the specified GOR.

| Option | Correlation |
|--------|-------------|
| 0 | Vazquez & Beggs (default) |
| 1 | Lasater |
| 2 | Standing |
| 3 | Glaso |
| 4 | Livia Fulchignoni |

> **JSON key:** `RsPbModel` (EN) · `modeloRsPb` (PT) — default `0`

!!! note
    The Livia Fulchignoni model (4) is computationally expensive. Consider enabling `RsPbTable` / `tabelaRSPB` in `initialConfig` to pre-build a lookup table for performance.

### Gas Critical Properties

Different correlations suit different gas compositions, particularly when CO₂ content is significant.

| Option | Correlation | Notes |
|--------|-------------|-------|
| 0 | Standard (inherited from Marlim2) | General purpose |
| 1 | Brown et al | Better for CO₂-rich gases (default) |
| 2 | Piper et al | Better for CO₂-rich gases |

> **JSON key:** `criticalCorrelation` (EN) · `correlacaoCritica` (PT) — default `1`

!!! note
    For the `gasFluid` object (service line or gas source), only options 1 and 2 are valid.

### Emulsion Effects

When water and oil flow together, emulsions can dramatically increase apparent viscosity. The emulsion model modifies oil viscosity based on water content:

| Option | Model | Additional input required |
|--------|-------|--------------------------|
| 0 | Linear water-quality weighting | None (default) |
| 1 | Weak Woelflin | — |
| 2 | Medium Woelflin | — |
| 3 | Strong Woelflin | — |
| 4 | Exponential | Coefficients A and B |
| 5 | Pal-Rhodes | Multiplier φ₁₀₀ |
| 6 | User table | BSW and multiplier vectors |
| 7 | Below-saturation BSW model | — |

> **JSON key:** `emulsionType` (EN) · `tipoEmul` (PT)
> Additional: `emulsionCoefA` / `coefAModeloExp`, `emulsionCoefB` / `coefBModeloExp`, `phi100` / `PHI100`, `bswVec` / `BSWVec`, `emulVec` / `emulVec`, `bswInversion` / `bswCorte`

### Compositional Fluid Options

When compositional mode is active, the fluid can be defined by molar fractions of pseudocomponents (same order as in the `.ctm` file). Optionally, compositions can be corrected to match a specified GOR.

> **JSON keys:**
>
> - User provides composition: `userMolarFraction` (EN) · `fracMolarUsuario` (PT) — default `false`
> - Molar fractions: `molarFraction` (EN) · `fracMolar` (PT) — array
> - Correct composition to match GOR: `userGORComp` (EN) · `RGOCompUsuario` (PT) — default `false`

### Flash-Table Viscosity Override

When using flash-table mode, viscosities normally come from the PVT table. However, in some cases the table viscosities may be inaccurate or unavailable for the conditions of interest. Enabling the black-oil viscosity override uses the dead-oil / live-oil / undersaturated-oil correlations instead of table values.

> **JSON key:** `blackOilViscModel` (EN) · `modeloViscBlackOil` (PT) — default `0`
> Values: `0` = use table viscosities; `1` = use black-oil viscosity correlations

### Flash-Table Water/JT Override

Similarly, the Joule-Thomson coefficient for the water phase can be computed from black-oil water correlations instead of using the table value.

> **JSON key:** `blackOilWaterModel` (EN) · `modeloAguaBlackOil` (PT) — default `1`
> Values: `0` = use table JT coefficient; `1` = use black-oil water correlations

### Fluid-Type Classification

The fluid type determines how interphase mass transfer is modeled:

- **Liquid-dominated (0):** Mass transfer follows black-oil logic (gas dissolves in/comes out of oil based on solubility ratio).
- **Gas-dominated (1):** Mass transfer uses quality/void-fraction relations (condensate drops out of gas).

> **JSON key:** `initialConfig.fluidType` (EN) · `configuracaoInicial.tipoFluido` (PT) — default `0`

---

## Specific-Heat Model

The specific heat (Cp) controls how temperature responds to pressure changes and heat exchange. Two options:

- **Black-oil correlation (0):** Internal model based on fluid parameters.
- **PVT table interpolation (1):** Reads Cp from the provided PVT file.

> **JSON key:** `cpModel` (EN) · `modeloCp` (PT) — default `0`

## Liquid Joule-Thomson Derivative

The Joule-Thomson effect describes temperature changes during isenthalpic expansion. By default the black-oil model computes the liquid density derivative with temperature (dρ_l/dT) internally. Alternatively, this value can be read from a PVTSim table for higher accuracy.

> **JSON key:** `jtlModel` (EN) · `modeloJTL` (PT) — default `0` (black-oil); `1` = use table

## Enthalpy from PVT Table

For more accurate thermal predictions, liquid and gas enthalpies can be read from the PVT file instead of using black-oil estimates.

> **JSON key:** `latentHeat` (EN) · `latente` (PT) — default `false`

## Free-Gas Density Correction

By default, in-situ gas density is computed from standard-condition values. When this correction is enabled, higher in-situ pressure is accounted for, recognizing that lighter hydrocarbons remain in solution under those conditions.

> **JSON key:** `freeGasDensityCorrectionBO` (EN) · `correcaoDenGasLivreBlackOil` (PT) — default `false`

## Latent-Heat Condensation Control

In rare cases of extreme retrograde condensation (exotic compositional fluids), latent heat from condensation mass transfer can cause numerical instabilities. This flag disables latent heat specifically in condensation processes within the energy equation.

> **JSON key:** `latentHeatCond` (EN) · `condlatente` (PT) — default `true` (enabled)

## Dynamic Compositional Table

For steady-state compositional simulations with multiple mixing zones (e.g., networks), repeating flash calculations at every iteration is expensive. When enabled, this option first runs a black-oil simulation to obtain a P-T map, then builds a posterior compositional table for each composition zone. Subsequent iterations interpolate from this table instead of computing full flashes.

> **JSON key:** `dynamicTableModel` (EN) · `modeloTabelaDinamica` (PT) — default `false`

## RS Pre-Table

The Lívia Fulchignoni RS correlation is computationally expensive. When enabled, the simulator pre-builds a solution-gas-ratio lookup table before starting the time loop, significantly improving performance.

> **JSON key:** `RsPbTable` (EN) · `tabelaRSPB` (PT) — default `false`

## Fluid Tracking Along the Pipeline

When multiple sources inject different fluids into the same line, properties evolve along the pipeline as streams mix. Two tracking flags control whether mixing-rule updates are applied:

- **GOR tracking** — updates GOR, API, and BSW along the line via mixing rules when streams merge.
- **Gas-density tracking** — updates gas relative density and CO₂ fraction along the line.

> **JSON keys:**
>
> - `trackGOR` (EN) · `trackRgo` (PT) — default `true`
> - `trackGasDensity` (EN) · `trackDensidadeGas` (PT) — default `true`

## Legacy Hybrid Model

A legacy option that selects a hybrid black-oil model where enthalpies, phase Cps, and liquid Joule-Thomson values are read from PVTSim files while other properties remain black-oil. Superseded by the more flexible `cpModel`, `jtlModel`, and `latentHeat` options.

> **JSON key:** `fluidProp` (EN) · `propFluido` (PT) — default `0` (pure black-oil); `1` = hybrid

---

## Precomputed Property Tables

Computing compressibility and its derivatives at every iteration can be expensive in transient simulations. Marlim3 can pre-build interpolation tables spanning a user-defined pressure-temperature grid before the simulation starts.

Two boolean flags enable the table generation:

- **Production-line table** — covers all declared production fluids.
- **Service-line gas table** — covers the gas fluid.

The grid dimensions and bounds are defined in a separate object.

> **JSON key (grid object):** `compTable` (EN) · `tabela` (PT)
>
> **JSON key (enable for production line):** `pressureTable` (EN) · `tabP` (PT) — boolean, inside initialConfig
>
> **JSON key (enable for service gas):** `gasTable` (EN) · `tabG` (PT) — boolean, inside initialConfig

Required grid parameters (inside `compTable` / `tabela`):

- **Number of points** in each dimension *(key: `numPoints` / `nPontos`)*
- **Pressure range** [kgf/cm²] *(keys: `minPressure` / `maxPressure` · `pressaoMinima` / `pressaoMaxima`)*
- **Temperature range** [°C] *(keys: `minTemperature` / `maxTemperature` · `temperaturaMinima` / `temperaturaMaxima`)*

!!! warning
    The grid must fully cover the expected operating envelope. Extrapolation outside the table bounds is unreliable and can cause numerical errors.

---

## Complementary Fluid

Some systems carry a **third liquid phase** that is inert, does not dissolve in gas, and does not slip relative to the hydrocarbon mixture. Examples include:

- Glycol or ethanol cushions (hydrate prevention before master valve opening)
- Chemical inhibitors
- Completion fluids (brine)

> **JSON key:** `complementaryFluid` (EN) · `fluidoComplementar` (PT)

### Fluid-Type Options

The type determines how properties are computed:

- **Generic (0):** User provides all physical properties (density, compressibility, viscosity, etc.)
- **Water-based (1):** Only salinity is required — correlations handle the rest.
- **Friction-reducer (2):** Like generic, but with internal friction-reduction treatment.

> **JSON key:** `complementaryFluidType` (EN) · `tipoF` (PT)

For the generic type, the following properties define the fluid:

- **Density** at standard conditions [kg/m³] *(key: `density` / `massaEspecifica`)*
- **Compressibility** [1/Pa] *(key: `compressibility` / `compP`)*
- **Thermal expansivity** [1/K] *(key: `thermalExpansivity` / `compT`)*
- **Surface tension** [N/m] *(key: `surfaceTension` / `tensup`)*
- **Specific heat** [J/(kg·K)] *(key: `specificHeat` / `calorEspecifico`)*
- **Conductivity** [W/(m·K)] *(key: `conductivity` / `condutividade`)*
- **Viscosity** via two ASTM points *(keys: `temp1`, `visc1`, `temp2`, `visc2`)*

For water-based type, provide only:

- **Salinity** [g/(kg water)] *(key: `salinity` / `salinidade`)*

---

## Gas Fluid (Service Line or Gas Source)

When a service or injection gas line is present, its gas properties must be defined separately within the gas fluid object, which characterizes dry gas unassociated with production fluids. This same fluid object can be applied to gas sources that do not rely on a service line, provided the gas is explicitly defined as dry in the gas source object (see Accessories documentation for details).

> **JSON key:** `gasFluid` (EN) · `fluidoGas` (PT)

Key concepts:

- **Gas relative density** — same convention as production fluid gas. *(key: `gasDensity` / `densidadeGas`)*
- **CO₂ fraction** — molar fraction of CO₂ in the gas. *(key: `CO2Fraction` / `fracCO2`, default: 0)*
- **Critical-property correlation** — selects the pseudo-critical correlations: `1` = Brown et al; `2` = Piper et al (both better for CO₂-rich gases). *(key: `criticalCorrelation` / `correlacaoCritica`)*
- **Flash-table usage** — whether to use the flash-table for service-line gas when flash-table mode is active globally. *(key: `useFlashTable` / `usaTabelaFlash`, default: `false`)*
- **Composition** — explicit molar fractions for compositional runs. *(keys: `userMolarFraction` / `fracMolarUsuario`, `molarFraction` / `fracMolar`)*

---

## Example JSON

```json
{
  "initialConfig": {
    "flashTableFluidModel": false,
    "compositionalFluidModel": false,
    "fluidType": 0,
    "cpModel": 0,
    "jtlModel": 0,
    "pressureTable": true,
    "pvtFile": "PVT_CASE.tab"
  },
  "productionFluid": [
    {
      "id": 0,
      "active": true,
      "api": 28.0,
      "gasDensity": 0.65,
      "gor": 150.0,
      "bsw": 0.10,
      "waterDensity": 1.02,
      "CO2Fraction": 0.03,
      "deadOilModel": 3,
      "liveOilModel": 0,
      "undersaturatedOilModel": 0,
      "criticalCorrelation": 1,
      "RsPbModel": 0,
      "emulsionType": 0
    }
  ],
  "compTable": {
    "active": true,
    "numPoints": 50,
    "minPressure": 1.0,
    "maxPressure": 500.0,
    "minTemperature": 4.0,
    "maxTemperature": 120.0
  }
}
```

## Example JSON (Compositional with .ctm)

```json
{
  "initialConfig": {
    "flashTableFluidModel": false,
    "compositionalFluidModel": true,
    "pvtFile": "PVT_CASE.ctm"
  },
  "productionFluid": [
    {
      "id": 0,
      "active": true,
      "gor": 180.0,
      "bsw": 0.05,
      "userMolarFraction": true,
      "molarFraction": [0.35, 0.20, 0.15, 0.30]
    }
  ]
}
```

!!! tip
    Start with one fluid and validate a simple steady-state case before adding advanced options (compositional corrections, emulsions, multiple fluids) incrementally.
