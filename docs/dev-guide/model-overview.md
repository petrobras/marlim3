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
- Hydrocarbon production
- Single-branch and open-loop network
#### Injection
- Water, gas, or CO2 injection (CCS)
- Single-branch, open-loop network, and closed-loop (gas injection only)

### Subsea architecture
#### Single-branch
- Standalone well-platform interconnection
- Production and injection
#### Open-loop network
- Tree topology: manifolds or gathering lines for multiple wells  
- Production and injection
#### Closed-loop network (gas-lift injection loop)
- Interconnection between multiple injection manifolds
- More than one independent topside compression system  
- Gas balance via Ridder's method or direct iteration
#### Parallel network
- Two co-located pipelines coupled by a shared source node
- Production only

### Equipment & accessories
#### Pumps
##### ESP / BCS
- Centrifugal submersible pump
- H-Q curve, efficiency map
- Motor temperature model
##### Multi-BCS (ESPs in series)
##### Volumetric pump / PCP
#### Valves
##### Surface choke
- Ashford-Pierce correlation
- Perkins correlation
##### Injection choke
##### Gas-lift valve (GLV)
- Orifice type (convergent nozzle)
- Venturi type (convergent-divergent)
- Critical / sub-critical flow
##### Master valve
##### Control valve
#### Sources & boundary conditions
##### Gas source
##### Liquid source
##### Mass source
##### Pressure node
##### Injection well BC
- Water, CO2, compositional
##### Surface separator
##### Downhole separator
##### Prescribed pressure drop
#### Reservoir contact models
##### Inflow performance relationship (IPR)
- Linear — constant productivity index
- Combined Vogel — linear above bubble point, Vogel curve below
- Vogel
##### 2D porous medium
- Unstructured tetrahedral grids under FVM

---

## Physical models

### Multiphase hydrodynamics
#### Drift-flux (transient)
- Harald Laux formulation
- Zuber-Findlay slip
- 1D FVM staggered-grid
#### Slug / intermittent flow
- Taitel-Barnea (1990) model
- Unit-cell approach
#### Stratified flow
- Kelvin-Helmholtz stability
#### Unified mechanistic model
- Gomez-Shoham-Schmidt (2000)
- Horizontal to vertical upward
#### Gradient correlations (steady-state)
- Hagedorn-Brown
- Duns-Ros
- Orkiszewski
- Beggs & Brill (+ Palmer correction)
- Mukherjee-Brill
- Aziz
- Gray
- Dukler / Dukler-Eaton-Flanigan / Dukler-Minami

### Fluid / PVT models
#### Black-oil
- Solution GOR: Standing, Vasquez-Beggs, Lasater, Petrosky
- Bubble point, Bo, compressibility factor
- Oil/gas/liquid density
- Surface tension, Cp, enthalpy
- Joule-Thomson coefficients
#### PVT tables (PVTsim)
- Import of `.tab` / `.ctm` files
- 2D interpolation (P, T)
#### Compositional EoS (Fortran)
- PR78 / SRK + Pneloux volume shift
- Mixing rules (VdW, BIP)
- Flash: SS + DEM + TPD stability + Newton
- Fugacity coefficients
- Phase density, enthalpy, Cp
- Surface tension
#### Black-oil from compositional
- Derivation of Rs, Bo, GOR from flash
#### Viscosity
- Pedersen CSP model (compositional)
- Dead/live oil correlations (black-oil)
#### Emulsion viscosity
#### Complementary fluid
- Injection fluid, non-Newtonian modeling capabilities
#### Steam / pure water
- Property tables

### Heat transfer
#### Internal forced convection
- Petukhov-Gnielinski correlation
#### External cross-flow convection
- Churchill-Bernstein (flowlines)
#### External natural convection
- Churchill-Chu (vertical/horizontal)
- Hollands / Catton (confined annulus)
#### Wall conduction
- Radial resistance model
- Steel, cement, insulation, annular fluid
#### Geological formation
- Transient thermal resistance
#### 2D buried-pipe solver
- Unstructured triangular FVM mesh
- Asymmetric soil conduction
#### 3D soil diffusion solver
- Axial-radial heat diffusion
#### Strong annular-column coupling
- Pseudo-transient marching for energy equation

---

## Flow assurance

### Hydrates
#### Production line model
- Equilibrium curve (T vs P)
- Hammerschmidt inhibitor (MEG / ethanol)
#### Service line model
- Same physics, separate annular flow inside wells

### Wax / paraffin deposition
#### Molecular diffusion model
- Deposition driven by T/r wall gradient
#### Wax PVT import (PVTsim)
- Wax appearance temperature
- Crystallization data
#### Dynamic deposit layer
- Internal radius update over time
- Update of global convective heat transfer coefficient

### Severe slugging
#### Boe criterion
#### Taitel-Barnea criterion

### Pigging operations
#### Transient position tracking
#### Holdup perturbation

---

## Solution strategy

### Steady-state (with parametric analysis)
- Shooting method: cell-by-cell spatial marching
- Bracket search + Ridder's method root-finding
- Parametric studies: serial or parallel — single-branch only

### Transient
- Void fraction α and complementary liquid fraction β: explicit upwind advance, CFL-limited
- Pressure and mixture mass flow: implicit coupled banded linear solve
- Temperature: semi-implicit (explicit advection)
- Mass-transfer coupling: implicit (default) or fully explicit
~~~
