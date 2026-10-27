# Embedded 2D/3D Solvers — Poisson Heat Diffusion & Porous Medium Flow

Marlim3 embeds specialised multi-dimensional solvers **inside individual pipeline cells** to resolve phenomena that the 1D pipeline discretisation cannot capture: radial/axial heat conduction through complex pipe walls and near-wellbore reservoir flow with multiphase saturations. These solvers are not standalone simulation modes — they live as member objects of cells or accessories and exchange boundary data with the 1D marching solver at every time step.

> **Note on scope:** the equations in this document are **compact governing forms**. The implementation includes geometry-dependent face reconstructions, capillary/elevation correction terms, relaxation, and iterative coupling procedures that are represented in the code through the assembled finite-volume operators and auxiliary state variables.

**Source files:**

| Solver | Header | Implementation |
|--------|--------|----------------|
| Poisson 2D | [`solverPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solverPoisson.h) | [`solverPoisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/solverPoisson.cpp) |
| Poisson 3D | [`solver3DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solver3DPoisson.h) | [`solver3DPoisson.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/solver3DPoisson.cpp) |
| Poroso 2D | [`solverPoroso.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solverPoroso.h) | [`solverPoroso.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/solverPoroso.cpp) |
| Radial porous (1D) | [`PorosoRad-Simples.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PorosoRad-Simples.h) | [`PorosoRad-Simples.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PorosoRad-Simples.cpp) |
| Radial porous (full) | [`PorosoRad.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PorosoRad.h) | [`PorosoRad.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PorosoRad.cpp) |

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Poisson 2D — Transverse Heat Diffusion](#poisson-2d--transverse-heat-diffusion)
3. [Poisson 3D — Axially-Extended Heat Diffusion](#poisson-3d--axially-extended-heat-diffusion)
4. [Radial Porous Flow — Accessory Type 15](#radial-porous-flow--accessory-type-15)
5. [Poroso 2D — Accessory Type 16](#poroso-2d--accessory-type-16)
6. [Coupling to the 1D Pipeline](#coupling-to-the-1d-pipeline)
7. [Shared Numerical Infrastructure](#shared-numerical-infrastructure)
8. [Output Files](#output-files)

---

<a id="architecture-overview"></a>
## 1. Architecture Overview

The following diagram shows where each embedded solver lives in the object hierarchy:

```
SProd (production system)
 │
 ├── celula[i] : Cel                          ← 1D pipeline cell
 │    ├── calor : TransCal                    ← heat-transfer object
 │    │    ├── difus2D : int                  ← flag (1 = Poisson 2D active)
 │    │    └── poisson2D : solverP            ← 2D heat-diffusion solver
 │    │
 │    └── acsr : acessorio                    ← cell accessory
 │         ├── tipo == 15 → radialPoro : PorosRadSimp   ← 1D radial porous
 │         └── tipo == 16 → poroso2D   : solverPoro     ← 2D porous flow
 │                           └── dados.transfer : PorosRad  ← 1D radial coupling annulus
 │
 └── poisson3D : solverP3D                    ← 3D heat-diffusion (system-level)
```

Key points:

- **Poisson 2D** is embedded per-cell in the `TransCal` heat-transfer object, activated by `difus2D == 1`. It solves transverse heat conduction in the pipe cross-section.
- **Poisson 3D** is a single system-level object on `SProd`, coupling to **multiple** pipeline cells simultaneously. It solves heat conduction in a 3D solid (e.g. a buried pipe section).
- **Accessory type 15** (`PorosRadSimp`) is a simplified 1D radial Darcy-flow solver for near-wellbore inflow.
- **Accessory type 16** (`solverPoro`) is a full 2D unstructured porous-flow solver, which internally contains a 1D radial annulus (`PorosRad`) for the wellbore coupling.

---

<a id="poisson-2d--transverse-heat-diffusion"></a>
## 2. Poisson 2D — Transverse Heat Diffusion

**Class:** `solverP` — [`solverPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solverPoisson.h)

### Governing Equation

Fourier's transient heat conduction in 2D:

$$\rho \, c_p \frac{\partial T}{\partial t} = \nabla \cdot (k \nabla T)$$

where $\rho$, $c_p$ and $k$ are the local density, specific heat and thermal conductivity of the solid (pipe wall, insulation, soil, etc.). The solver resolves the **cross-sectional temperature field** that the standard 1D radial-resistance model cannot capture (e.g. pipes with asymmetric burial, partial insulation, or irregular geometry).

### Mesh

- **Unstructured 2D triangular** — generated externally (Triangle `.ele`/`.node` format or UNV)
- Represents a single pipe cross-section perpendicular to the flow axis
- Classes: `malha2d` ([`Malha2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha2DPoisson.h)), `elementoPoisson` ([`Elem2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem2DPoisson.h))
- Each element stores: centroid, 3 face normals (`sFace`), face areas, neighbour indices, interpolation factor (`fatG`), non-orthogonality correction vectors (`vecE`, `vecT`, `angES`)

### Numerical Method

**Cell-centred finite-volume method (FVM)** with:

- **Green-Gauss gradient reconstruction** (`calcGradGreen()`) for computing $\nabla T$ at cell centres
- **Deferred-correction** for non-orthogonal face gradients using the decomposition into orthogonal (`vecE`) and tangential (`vecT`) components
- Local matrix has shape `(1, nvert+1)` — one equation per cell with contributions from the cell itself and its `nvert` neighbours
- Global coefficients are stored in the sparse structure `SparseMtx<double>`
- The temperature field is advanced through iterative local reconstruction, local assembly, and global coefficient updates over the finite-volume mesh

### Boundary Conditions

| BC type | Struct | Description |
|---------|--------|-------------|
| **Dirichlet** | `detDiriPoisson` | Fixed temperature (time-series capable) |
| **Von Neumann** | `detVNPoisson` | Fixed heat flux (time-series) |
| **Robin** | `detRicPoisson` | Convective-type boundary using ambient temperature and heat-transfer coefficient |
| **Coupling** | `rotuloAcop` | Coupled boundary segment to the 1D pipeline, implemented through a local thermal-resistance formulation using `condLoc`, `hI`, and a wall/interface temperature consistent with the coupled heat-flow balance |

### Key Methods

| Method | Purpose |
|--------|---------|
| `solverP(vg1dSP, meshFile, condGlob, condLoc, hE, hInt, Tint, Tamb, diamI, diamE, indCel)` | Full constructor: reads mesh, applies material properties, sets coupling parameters |
| `permanentePoisson()` | Iterative steady-state solve |
| `inicializaPermanentePoisson()` / `inicializaTransientePoisson()` | Switch all cells between permanent/transient mode flags |
| `transientePoisson(delt)` | Advance one time step through transient finite-volume assembly and iterative temperature update |
| `transientePoissonDummy(delt)` | Same, but also updates `tempC0` — used during initialisation sub-cycling |
| `defineDeltPoisson()` | Returns $\Delta t$ from a piecewise time schedule |
| `finalizaPassoTransiente(delt, indTramo)` | Updates `tempC0 = tempC`, prints profile at scheduled output times |
| `imprimePermanente(indTramo)` | Writes steady-state temperature field to file |
| `FeiticoDoTempo()` | **Rollback**: restores `tempC = tempC0` (undo a failed step) |

### Data Structures

| Structure | File | Purpose |
|-----------|------|---------|
| `dadosP` | [`dados1Poisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/dados1Poisson.h) | Input container: mesh file path, material properties, BCs, time control, coupling parameters |
| `elementoPoisson` | [`Elem2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem2DPoisson.h) | FV element: vertices, faces, normals, `tempC`/`tempC0`, `rho`, `cp`, `cond`, gradient vectors, local matrix |
| `malha2d` | [`Malha2DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha2DPoisson.h) | Mesh container: element array `mlh2d[]`, vertex coordinates, face connectivity |
| `detCCPoisson` | [`estruturasPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturasPoisson.h) | BC collection: Dirichlet/Neumann/Robin/Coupling arrays, face label maps |

---

<a id="poisson-3d--axially-extended-heat-diffusion"></a>
## 3. Poisson 3D — Axially-Extended Heat Diffusion

**Class:** `solverP3D` — [`solver3DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solver3DPoisson.h)

### What 3D Adds

While the 2D Poisson solver works on a single cross-section for **one** pipeline cell, the 3D solver discretises a **volumetric solid** (e.g. a buried pipeline segment including surrounding soil) with **tetrahedral elements**. It couples to **multiple** pipeline cells simultaneously, each with its own coupling surface label, fluid temperature, and convection coefficient.

### Mesh

- **Unstructured tetrahedral** — imported from UNV format with named regions
- Classes: `malha3d` ([`Malha3DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha3DPoisson.h)), `elementoPoisson3D` ([`Elem3DPoisson.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem3DPoisson.h))
- Volume computed via the scalar triple product of edge vectors
- 4 triangular faces per element, normals via cross product
- **Region-based material assignment**: element region strings matched against named material properties in the input

### Multiple Coupling Surfaces

| Member | Type | Purpose |
|--------|------|---------|
| `nacop` | `int` | Number of coupling surfaces (one per pipeline cell that touches the 3D domain) |
| `dados.tInt[i]` | `double[]` | Fluid temperature from pipeline cell `i` |
| `dados.hI[i]` | `double[]` | Internal convection coefficient from pipeline cell `i` |
| `dados.qAcop[i]` | `double[]` | Computed heat flux through coupling surface `i` |
| `dados.tParede[i]` | `double[]` | Coupling-side temperature value associated with surface `i`; in the current implementation it is set equal to `tInt[i]` |
| `dados.CC.rotuloAcop[i]` | `string[]` | UNV region label identifying coupling surface `i` |

### Key Methods

| Method | Purpose |
|--------|---------|
| `solverP3D(meshFile, vg1d, nacop, dutoAux, hi, he, ti)` | Constructor: reads UNV mesh, assigns materials by region, and initializes the coupling structures and starting values |
| `permanentePoisson()` | Steady-state solve (returns divergence flag) |
| `transientePoisson(delt)` | Single transient time step |
| `transientePoissonDummy(delt, konta)` | Initialisation sub-cycling variant |
| `renova()` | Commit: `tempC0 = tempC` |
| `FeiticoDoTempo()` | Rollback: `tempC = tempC0` |

### Initialisation in Num4Main.cpp

At lines 13604–13665 of [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp):

1. The `solverP3D` object is constructed from the UNV mesh file (`modoDifus3DJson`)
2. Each coupling surface label is matched to a pipeline cell index
3. The pipeline refreshes `tInt[]` and `hI[]` from the corresponding cell temperatures and convection coefficients
4. A sub-cycling loop calls `transientePoissonDummy()` until the change in `qTotal[0]` between successive pseudo-steps falls below the imposed tolerance

---

<a id="radial-porous-flow--accessory-type-15"></a>
## 4. Radial Porous Flow — Accessory Type 15

**Class:** `PorosRadSimp` — [`PorosoRad-Simples.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PorosoRad-Simples.h)

This is a **1D radial Darcy-flow** solver representing the near-wellbore reservoir. It is the simpler of the two porous-flow models and is attached to a pipeline cell as accessory type 15.

### Governing Equation

Multiphase radial Darcy flow in cylindrical coordinates:

$$\frac{1}{r}\frac{\partial}{\partial r}\left(r \, \frac{k_{abs} \, k_{r\alpha}}{\mu_\alpha} \frac{\partial}{\partial r}\left(P - P_{c,\alpha} - \rho_\alpha g z\right)\right) = \phi \, c_t \frac{\partial P}{\partial t}$$

for each phase $\alpha \in \{o, w, g\}$ (oil, water, gas). Three-phase flow with:

- **Relative permeability** tables: `kRelOA` (oil-water), `kRelOG` (oil-gas)
- **Capillary pressure** tables: `pcOA` (oil-water), `pcGO` (gas-oil)
- **Formation volume factors** and **viscosities** computed via the embedded `ProFlu` and `ProFluCol` objects

### Geometry

The `DadosGeoPoro` class ([`GeometriaPoro.h`](https://github.com/petrobras/marlim3/blob/main/src/include/GeometriaPoro.h)) defines a multi-layer radial geometry:

| Member | Purpose |
|--------|---------|
| `a` | Inner diameter (wellbore) |
| `b` | Outer diameter (reservoir boundary) |
| `ncamadas` | Number of material layers |
| `kX[i]` | Radial permeability of layer `i` |
| `kY[i]` | Tangential permeability of layer `i` |
| `poro[i]` | Porosity of layer `i` |
| `compRoc[i]` | Rock compressibility of layer `i` (÷1000 in constructor) |
| `diamC[i]` | Outer diameter of layer `i` |
| `espessuR[i]` | Thickness of layer `i` |

### Radial Cells

`celradSimp` ([`celRad-Simples.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celRad-Simples.h)) stores per-cell state for the 1D radial grid:

- Pressures: `Pcamada`, `PcamadaL`, `PcamadaR`, `Pini`, `Piter`
- Phase flow rates: `QocamadaR/L`, `QwcamadaR/L`, `QgcamadaR/L`
- Saturations: `sL`, `sW`, `sLini`, `sWini`, `sLiter`, `sWiter` (+ left/right)
- Densities and viscosities: `rhoP`, `rhogP`, `rhoPa`, `mio`, `mig`, `mia` (current + previous-step)
- Relative permeabilities: `kmed`, `kmedG`, `kmedA` (+ previous-step)
- Capillary pressures: `pcOG`, `pcAO` (+ previous-step)
- Geometry: `r0`, `r1`, `rm` (inner, outer, mid radius), `zD` (datum elevation)

### Key Methods

| Method | Purpose |
|--------|---------|
| `geraCel(Pcamada, sL, sW, sLRes, sWRes)` | Generate radial cell array with initial pressures and saturations |
| `transperm(mastot)` | Steady-state solve via Ridder's root-finding (`zriddr`) — finds total mass flow that satisfies the radial pressure profile |
| `pseudoTrans()` | Pseudo-transient: pressure from root-finding, used during SS pipeline march |
| `avancoPressao()` | Implicit pressure advance (transient IMPES) |
| `avancoSW(delt)` | Explicit saturation advance (CFL-limited) |
| `avancoSWcorrec()` | Saturation correction step |
| `marchaperm(mastot)` | Single radial sweep: given total mass flow, compute pressure profile cell-by-cell |
| `renovaPres(i, mTot)` | Update pressure and saturations at cell `i` |
| `perfil()` | Return radial profile as `FullMtx<double>` (radius, pressure, flow rates, saturations) |
| `FeiticoDoTempo()` | Rollback all cells to previous step |
| `atualizaIni()` | Commit current state as new initial state |

### Coupling

At [`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) lines 4744–4759, during the 1D pressure march:

```
radialPoro.sWPoc  = in-situ water fraction (from pipeline cell)
radialPoro.Pint   = celula[ind].pres        (wellbore pressure)
radialPoro.avancoPressao()                   (solve radial flow)
celula[ind].fontemassLR = fluxIni + fluxIniA (liquid mass source → pipeline)
celula[ind].fontemassGR = fluxIniG           (gas mass source → pipeline)
```

---

<a id="poroso-2d--accessory-type-16"></a>
## 5. Poroso 2D — Accessory Type 16

**Class:** `solverPoro` — [`solverPoroso.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solverPoroso.h)

This is a full **2D unstructured finite-volume** solver for multiphase porous-medium flow, representing the near-wellbore reservoir in plan view (areal). It is attached to a pipeline cell as accessory type 16.

### Governing Equations

**IMPES form** (Implicit Pressure, Explicit Saturation) for three-phase Darcy flow:

**Pressure equation**:

$$
\nabla \cdot \left[ \lambda_o \nabla \Phi_o + \lambda_w \nabla \Phi_w + \lambda_g \nabla \Phi_g \right] = \phi \, c_t \frac{\partial P}{\partial t}
$$

with mobilities and phase-potential-like terms:

$$
\lambda_\alpha = \frac{k_{abs} \, k_{r\alpha}}{\mu_\alpha}, \qquad \Phi_o = P - \rho_o g z, \qquad \Phi_w = P - P_{c,AO} - \rho_w g z, \qquad \Phi_g = P + P_{c,GO} - \rho_g g z
$$

The porous formulation is implemented together with reconstructed auxiliary gradients such as `gradGreenPcAO`, `gradGreenPcOG`, `gradGreenZdatum`, and `gradGreenAZdatum`, plus pressure/flux exchange with the embedded radial transfer model.

**Saturation update**:

$$
\phi \frac{\partial S}{\partial t} + \nabla \cdot (\text{phase fluxes}) = 0
$$

Saturations are updated explicitly through face-based transport operators (`evoluiSW(...)`) on both the 2D porous mesh and, when active, the radial transfer model.

### Mesh

- **Unstructured 2D triangular** — same FVM topology as Poisson 2D
- Classes: `malha2dPoro` ([`Malha2DPoroso.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha2DPoroso.h)), `elementoPoroso` ([`Elem2DPoroso.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem2DPoroso.h))
- Green-Gauss gradient reconstruction with non-orthogonality correction
- **Anisotropic permeability** (`kX`, `kY`) per element
- **Datum elevation** `zD` interpolated per cell (tilted reservoir)
- A regular structured output grid (`malhaH`) for visualisation

### Per-Element State

Each `elementoPoroso` stores (beyond the FVM geometry shared with Poisson):

| Field group | Examples |
|-------------|----------|
| Pressure | `presC`, `presC0`, `presIter` |
| Saturations | `sWC`, `sLC`, `alfC` (gas fraction), `sWC0`, `sLC0` |
| Fluid properties | `rhoL`, `rhoG`, `rhoA`, `mioL`, `mioG`, `mioA`, `bo`, `ba`, `rs`, `bg` |
| Relative permeabilities | `kRelOA`, `kRelOG` (tables), `kmed`, `kmedG`, `kmedA` |
| Capillary pressures | `pcOA`, `pcGO` (tables) |
| Rock properties | `kX`, `kY`, `poro`, `compRoc` |
| Gradient arrays | `gradGreenP`, `gradGreenZdatum`, `gradGreenPcAO`, `gradGreenPcOG` |

### Boundary Conditions

| BC type | Struct | Description |
|---------|--------|-------------|
| **Dirichlet** | `detDiriPoroso` | Fixed pressure + saturation (time-series) |
| **Von Neumann** | `detVNPoroso` | Fixed flux + saturation (time-series) |
| **Robin** | `detRicPoroso` | Productivity-index-like BC |
| **Coupling** | `rotuloAcop` + `satAcop` | Two-way boundary exchange with the embedded radial transfer model, including averaged boundary pressure/saturation transfer and redistributed coupling fluxes |

### Internal 1D Radial Annulus

The 2D solver contains an embedded `PorosRad` object (`dados.transfer`) that represents the **radial annulus** between the wellbore and the 2D reservoir mesh boundary. The coupling chain is:

```
Pipeline cell  ←→  1D radial annulus (PorosRad)  ←→  2D reservoir mesh (solverPoro)
                    │                                     │
                    Pint, sWPoc                      coupling faces labelled by `rotuloAcop`
                    fluxIni, fluxIniG               averaged boundary pressure/saturation exchange
```

### Key Methods

| Method | Purpose |
|--------|---------|
| `solverPoro(vg1dSP, meshFile)` | Constructor: reads mesh, material properties, fluid model, BCs |
| `avancoPressao()` | Pressure-update stage: assembles sparse global coefficients, updates the 2D pressure field, and exchanges pressure/flux data with the radial `transfer` object |
| `avancoSW(delt)` | Explicit saturation advance from face fluxes (CFL-limited) |
| `avancoSWcorrec()` | Saturation correction step |
| `reavaliaDT(delt)` | Re-evaluate time step after CFL violation |
| `reiniciaEvoluiSW(delt)` | Reset local time-step data before redoing the saturation evolution with reduced $\Delta t$ |
| `pseudoTransientePoroso()` | Pseudo-transient / pseudo-steady pressure-coupling solve used during the steady marching procedure |
| `transientePoroso(delt)` | Full transient step: pressure + saturation + CFL check |
| `defineDT(perm)` | Compute $\Delta t$ from CFL conditions and user-specified schedule |
| `preparaTabDin()` | Build dynamic PVT tables for the porous domain |
| `geraMiniTabFlu()` | Generate mini fluid-property tables for fast look-up |
| `atualizaCel2D(i)` | Update fluid properties at element `i` from pressure/saturation |
| `atualizaCelTransfer(i)` | Update fluid properties at radial-annulus cell `i` |
| `imprimePseudo()` | Write pseudo-steady-state results to file |
| `imprimeMalhaRegular(minP)` | Interpolate unstructured solution onto regular grid and write to file |
| `FeiticoDoTempo()` | Rollback pressure + saturation on both the 2D mesh and the radial annulus |
| `FeiticoDoTempoSW()` | Rollback saturation only |
| `FeiticoDoTempoPQ()` | Rollback pressure only |

### Coupling

At [`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) lines 4761–4800, during the 1D pressure march:

```
poroso2D.dados.pW.val[0]      = celula[ind].pres    (wellbore pressure → Dirichlet BC)
poroso2D.sWPoc                = in-situ water fraction
poroso2D.dados.transfer.sWPoc = same
poroso2D.dados.transfer.Pint  = celula[ind].pres
poroso2D.avancoPressao()                             (solve 2D pressure field)
celula[ind].fontemassLR = transfer.fluxIni + transfer.fluxIniA  (liquid mass → pipeline)
celula[ind].fontemassGR = transfer.fluxIniG                     (gas mass → pipeline)
```

---

<a id="coupling-to-the-1d-pipeline"></a>
## 6. Coupling to the 1D Pipeline

### Steady-State Coupling

During the steady-state pressure march (`marchaProdPerm*` methods in `SProd`), when the marching algorithm encounters a cell with accessory type 15 or 16:

1. The pipeline cell pressure and in-situ water fraction are passed to the porous solver
2. The solver computes the reservoir inflow (mass flow rates for liquid, water and gas)
3. These mass sources are written back to `fontemassLR` / `fontemassGR` on the pipeline cell
4. The 1D march continues with the updated source terms

For the porous accessories, the steady marching workflow uses the pseudo-steady coupling methods:
- **type 15:** `pseudoTrans()`
- **type 16:** `pseudoTransientePoroso()`

For Poisson 2D, the steady-state thermal coupling is handled through the coupled boundary segments and the associated local/global resistance treatment.

### Transient Coupling

During the transient time loop in [`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp):

| Phase | What happens | Line refs |
|-------|-------------|-----------|
| **Pressure advance** | `avancoPressao()` on each type-15/16 accessory (transient pressure update coupled to pipeline cell pressure) | L4747, L4782 |
| **Saturation advance** | `avancoSW(dt)` on each type-15/16 accessory (explicit, after 1D fraction evolution `EvoluiFrac()`) | L12619, L12625 |
| **CFL check** | If any porous solver sets `reinicia == -1`, the pipeline triggers rollback of the 1D fraction evolution, then calls `reavaliaDT()` and `reiniciaEvoluiSW()` with reduced $\Delta t$ | L12630–L12655 |
| **Saturation correction** | `avancoSWcorrec()` after convergence | L12663, L12665 |
| **Poisson 2D** | `transientePoisson(dt)` called within the `TransCal` heat-balance loop; fluid temperature `tInt` and convection data are refreshed from the pipeline cell | per-cell in `TransCal` |
| **Poisson 3D** | `transientePoisson(dt)` called once per time step on the system-level `poisson3D` object; each coupling surface receives its cell's temperature and convection coefficient | L12405 in `SisProd.cpp` |
| **Rollback** | On outer-iteration failure, `FeiticoDoTempo()` restores both the 1D pipeline state and all embedded solver states to the previous time level | all solvers |

### IMPES Time-Step Control

The porous solvers use an IMPES-like workflow where:

- **Pressure** is updated implicitly/iteratively through assembled global coefficients
- **Saturation** is advanced explicitly and is CFL-limited

When the saturation advance violates the CFL condition, the porous solver sets `reinicia = -1`. The 1D pipeline detects this, rolls back the global 1D state, and restarts the step with a smaller $\Delta t$. This ensures that the 1D pipeline and porous solvers remain synchronised.

---

<a id="shared-numerical-infrastructure"></a>
## 7. Shared Numerical Infrastructure

All three FVM solvers (Poisson 2D, Poisson 3D, Poroso 2D) share the same numerical backbone:

### Gradient Reconstruction

**Green-Gauss method**: the gradient at cell centre $C$ is computed conceptually as:

$$(\nabla \phi)_C = \frac{1}{V_C} \sum_{f} \phi_f \, \vec{S}_f$$

where $\phi_f$ is the face value (interpolated from neighbour cells) and $\vec{S}_f$ is the outward face-area vector. Each element stores Green-Gauss gradient arrays for the primary variables and auxiliary fields.

### Non-Orthogonality Correction

Face gradients use a deferred-correction approach. For each face between cells $C$ and $N$:

$$\left(\frac{\partial \phi}{\partial n}\right)_f = \frac{\phi_N - \phi_C}{|\vec{E}|} + \text{correction}(\vec{T}, \nabla\phi)$$

where $\vec{E}$ is the vector connecting cell centres and $\vec{T}$ is the tangential correction. The decomposition is controlled by the angle `angES` between $\vec{E}$ and the face normal $\vec{S}$, with the interpolation factor `fatG`.

### Sparse Linear System

| Component | Implementation |
|-----------|----------------|
| Matrix format | Sparse coefficient storage via `SparseMtx<double>` |
| Iteration style | Repeated local reconstruction, local assembly, and field update loops |
| Global update | Each solver builds local finite-volume contributions, stores them in the sparse global structure, and advances the solution through the corresponding iterative update procedure |

### Rollback Mechanism — FeiticoDoTempo

All embedded solvers implement `FeiticoDoTempo()` ("time spell" / "time magic"), which restores the solution to the previous time level:

- **Poisson**: `tempC = tempC0` for all elements
- **Poroso 2D**: `presC = presC0`, `sWC = sWC0`, `sLC = sLC0` for all elements + radial annulus
- Variants: `FeiticoDoTempoSW()` (saturation only), `FeiticoDoTempoPQ()` (pressure only)

This mechanism supports the outer coupling iteration: if the 1D pipeline needs to retry a time step, all embedded solvers can be rolled back without re-reading data.

---

<a id="output-files"></a>
## 8. Output Files

| File pattern | Solver | Content |
|--------------|--------|---------|
| `PerfisPocoRadial-{t}-{pos}.dat` | Type 15 (`PorosRadSimp`) | Radial profile: radius, pressure, oil/gas/water flow rates, liquid/water saturation, gas void fraction |
| Poisson 2D steady-state output | `solverP::imprimePermanente()` | Cross-sectional temperature field after steady-state convergence |
| Poisson 2D transient output | `solverP::finalizaPassoTransiente()` | Temperature field at scheduled output times |
| Poroso 2D pseudo-steady output | `solverPoro::imprimePseudo()` | 2D pressure/saturation field |
| Regular-grid visualisation | `solverPoro::imprimeMalhaRegular()` | Interpolated solution on structured grid (pressure, saturation, gas fraction) |
