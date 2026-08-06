# Model Overview

The following interactive map of Marlim3's computational model hierarchy covers the most relevant modules in the simulation engine.
Click any node to expand or collapse its subtree; scroll to zoom and drag to pan.

~~~markmap
---
markmap:
 colorFreezeLevel: 2
 initialExpandLevel: 4
 maxWidth: 320
---

# Marlim3 Model Overview

## System

### Operation
#### Production
- Oil producing well or network
- Single-branch (`-s TRANSIENTE`) or network (`solveRedeProd` family)
#### Injection
- Water, gas, or CO2 injection (CCS)
- Single-branch (`-s INJETOR`) or open loop network (`RedeInj`)

### Subsea architecture
#### Open loop network
- Tree topology: wells, collectors, manifold
- Production (`solveRedeProd`, type 0) and injection (`RedeInj`, type 1)
- Pre-processing decomposes into independent sub-networks (OMP)
- `LerRede.h` / `preProcRede`
#### Closed loop network (gas-lift loop)
- Annular gas distribution line + N production branches
- Production only (type 2) — no injection variant
- Gas balance via Ridder's method or direct iteration
- `RedeAnelGL` / `TransAnel`
#### Parallel network
- Two co-located pipelines (primary + secondary)
- Production only (type 3) — no injection variant
- Coupled at shared `fontechk` source points
- `RedeParalela` / `SolveRedeParalelaTrans`
#### Single branch
- Standalone single branch (`SProd`), not part of a `Rede`
- Production (`-s TRANSIENTE`) and injection (`-s INJETOR`)

### Equipment & accessories
#### Pumps
##### ESP / BCS
- Centrifugal submersible pump
- H-Q curve, efficiency map
- Motor temperature model
- `Bcsm2.cpp` / `acessorios.cpp`
##### Multi-BCS (ESPs in series)
- `multiBCS.cpp`
##### Volumetric pump / PCP
- Constant-pressure displacement
- `BombaVol.cpp` / `BombaVolVap.cpp`
#### Valves
##### Surface choke
- Ashford-Pierce correlation
- Perkins correlation
- `chokegas.cpp`
##### Injection choke
- `injectionChoke`
##### Gas-lift valve (GLV)
- Orifice type (convergent nozzle)
- Venturi type (convergent-divergent)
- Critical / sub-critical flow
- `gasLiftSource` / `gasInj`
##### Master valve
- `masterValve` / `masterValve2`
##### Control valve
- Opening fraction, Cv
- `valve`
#### Sources & boundary conditions
##### Gas source
- Constant mass source
- `gasSource`
##### Liquid source
- `liquidSource`
##### Mass source
- `massSource` / `FonteMas.cpp` / `FonteMassCHK.cpp`
##### Pressure source
- `pressureSource`
##### Injection well BC
- Water, CO2, compositional
- `injectionWellBC`
##### Surface separator
- `separator.pressure`
##### Downhole separator
- `separator` (in-well)
##### Prescribed pressure drop
- `pressureDrop` (delta-P specified)
#### Reservoir contact models
##### Inflow performance relationship (IPR)
- Linear — constant productivity index
- Combined Vogel — linear above bubble point, Vogel curve below
- Vogel — pure curve using AOF (`qMax`)
- `FonteMas.cpp` (`IPR` class)
##### 1D radial porous medium
- Darcy / Vogel / table
- `PorosoRad.cpp` / `celRad.cpp`
##### 2D porous medium (FVM)
- Unstructured reservoir coupling
- Multiphase saturations
- `Malha2DPoroso.cpp` / `Elem2DPoroso.cpp`

---

## Physical models

### Multiphase hydrodynamics
#### Drift-flux (transient)
- Harald Laux formulation
- Zuber-Findlay slip
- Staggered-grid 1D FVM
- `celula3.cpp` / `solver.cpp`
#### Slug / intermittent flow
- Taitel-Barnea (1990) model
- Unit-cell approach
- `SlugFlow_TaitelBarnea.cpp`
#### Stratified flow
- Kelvin-Helmholtz stability
- `estrat.cpp` / `mapa.cpp`
#### Unified mechanistic model
- Gomez-Shoham-Schmidt (2000)
- Horizontal to vertical upward
- `GomezModel.cpp`
#### Gradient correlations (steady-state)
- Hagedorn-Brown
- Duns-Ros
- Orkiszewski
- Beggs & Brill (+ Palmer correction)
- Mukherjee-Brill
- Aziz
- Gray
- Dukler / Dukler-Eaton-Flanigan / Dukler-Minami
- `GradientCorrelations.cpp`

### Fluid / PVT models
#### Black-oil (`ProFlu`)
- Solution GOR: Standing, Vasquez-Beggs, Lasater, Petrosky
- Bubble point, Bo, Z-factor
- Oil/gas/liquid density
- Surface tension, Cp, enthalpy
- Joule-Thomson coefficients
- `PropFlu.cpp` + `BlackOilModels.f90`
#### PVT tables (PVTSIM)
- `.tab` / `.ctm` file import
- 2D interpolation (P, T)
- `PvtSimCTMFileImport.f90`
#### Compositional EOS (Fortran)
- PR78 / SRK + Pneloux volume shift
- Mixing rules (VdW, BIP)
- Flash: SS + DEM + TPD stability + Newton
- Fugacity coefficients
- Phase density, enthalpy, Cp
- Surface tension (Parachor)
- `MarlimComposicional.f90` / `VLECalculations.f90` / `FlashExtras.f90`
#### Black-oil from compositional
- Derive Rs, Bo, GOR from flash
- `BlackOilFromCompositional.f90`
#### Viscosity
- Pedersen CSP model (compositional)
- Dead/live oil correlations (black-oil)
- `ViscosityCalculationModule.f90`
#### Emulsion viscosity
- Adjustable model (tipoEmul, aemul, bemul)
- `PropFlu.cpp`
#### Complementary fluid
- Injection fluid, non-Newtonian
- `PropFluColVF.cpp`
#### Steam / pure water
- Property tables (ProVap)
- `PropVapor.cpp` / `celulaVapor.cpp`

### Heat transfer (`TransCal`)
#### Internal forced convection
- Petukhov-Gnielinski correlation
- `TrocaCalor.cpp`
#### External cross-flow convection
- Churchill-Bernstein (submarine pipes)
- `TrocaCalor.cpp`
#### External natural convection
- Churchill-Chu (vertical/horizontal)
- Hollands / Catton (confined annulus)
- `TrocaCalor.cpp` / `natural-convection.md`
#### Wall conduction
- Radial resistance network (series layers)
- Steel, cement, insulation, annular fluid
- `TrocaCalor.cpp`
#### Geological formation (Ramey)
- Transient thermal resistance
- `TrocaCalor.cpp`
#### 2D buried-pipe solver (Poisson)
- Unstructured triangular FVM mesh
- Asymmetric soil conduction
- `Malha2DPoisson.cpp` / `Elem2DPoisson.cpp`
#### 3D soil diffusion solver
- Axial-radial heat diffusion
- `dados3DPoisson.cpp` / `Elem3DPoisson.cpp`
#### Strong annular-column coupling
- Pseudo-transient marching (energy eq.)
- `strongAnnularColCoupling` (initialConfig.advanced)

---

## Flow assurance (GARESC)

### Hydrates
#### Production line model
- Equilibrium curve (T vs P)
- Hammerschmidt inhibitor (MEG / ethanol)
- `FA_Hidratos.cpp`
- `curva_base_hidrato.txt`
#### Service line model
- Same physics, separate annular flow
- `FA_Hidratos_Servico.cpp`
- `curva_base_hidrato_servico.txt`

### Wax / paraffin deposition
#### Molecular diffusion model
- Deposition driven by T/r wall gradient
- `WaxDeposition()` in `celula3.cpp`
#### Wax PVT import (PVTSIM)
- Wax appearance temperature
- Crystallisation data
- `PvtSimWAXFileImport.f90`
#### Dynamic deposit layer
- Updates internal radius over time
- Modifies U-value (thermal resistance)
- `heat-transfer.md Wax Deposition Layer`

### Severe slugging
#### Boe criterion
- Pressure-gradient stability check
- `criterioIntermiSevera.cpp`
#### Taitel-Barnea criterion
- Liquid accumulation in catenary/riser
- `criterioIntermiSevera.cpp`

### PIG tracking
#### Transient position tracking
- `estadoPig` / `velPig` state variables
- `celula3.cpp`
#### Holdup perturbation
- Liquid / gas redistribution ahead & behind PIG
- `alfPigE` / `alfPigD`

---

## Solution strategy

### Steady-state (with parametric analysis)
- Shooting method: cell-by-cell spatial marching (`marchaProdPerm1`)
- Bracket search + Ridder's method root-finding (`zriddr`)
- Parametric sweep: serial (`leituraAP`) or parallel OMP (`leituraAPparalelo`) — single-branch only
- `SolveTramoSolteiro` / `permanenteSimples` / `buscaProdPfundoPerm`

### Transient
- Void fraction α and complementary liquid fraction β: explicit upwind advance (`avancalf` / `avancbet`), CFL-limited
- Pressure and mixture mass flow: implicit coupled banded linear solve (`GeraLocal` → `SolveAcopPV`)
- Temperature: semi-implicit (explicit advection, heat loss at new time level via `calctemp`)
- Mass-transfer coupling: implicit (default) or fully explicit (`TMModelL` flag)
- Same scheme for single-branch and each network branch (`SolveTrans` / `SolveRedeTrans` / `TransAnel` / `SolveRedeParalelaTrans`)
~~~
