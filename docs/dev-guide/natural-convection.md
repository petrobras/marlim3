# 2D Natural Convection Solver

This document describes the **2D natural convection** simulation mode in Marlim3 (`-s CONVECNAT` / `tipoSimulacao_t::convecNatural`). This solver models laminar incompressible flow with heat transfer in 2D domains using the finite-volume method (FVM) on unstructured triangular meshes.

**Key source files:**

| File | Role |
|------|------|
| [`src/include/solver.h`](https://github.com/petrobras/marlim3/blob/main/src/include/solver.h) / [`src/core/solver.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/solver.cpp) | `solv2D` class — main solver: mesh, equations, time-stepping |
| [`src/include/Malha2D.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Malha2D.h) / [`src/core/Malha2D.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Malha2D.cpp) | Unstructured mesh data structure (Triangle `.node/.ele` format) |
| [`src/include/Elem2D.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Elem2D.h) / [`src/core/Elem2D.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Elem2D.cpp) | Element-level FVM operations (gradients, fluxes, assembly) |
| [`src/core/Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp) | Entry point: `resolucao(...)` function construction and `resolucao.resolve()` call |
| [`src/include/estruturas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturas.h) | `flucVF` (VOF data), `interface` struct, `cel2D` element data |

---

## Table of Contents

1. [Overview](#overview)
2. [Entry Point and Initialization](#entry-point-and-initialization)
3. [Mesh Formats](#mesh-formats)
4. [JSON Configuration](#json-configuration)
5. [Governing Equations](#governing-equations)
6. [FVM Discretization](#fvm-discretization)
7. [Pressure-Velocity Coupling](#pressure-velocity-coupling)
8. [Gradient Reconstruction](#gradient-reconstruction)
9. [Boundary Conditions](#boundary-conditions)
10. [Physical Models](#physical-models)
11. [Linear Solvers](#linear-solvers)
12. [Time Stepping and Convergence](#time-stepping-and-convergence)
13. [1D Wall Coupling](#1d-wall-coupling)
14. [Output Formats](#output-formats)
15. [Overall Algorithm Flow](#overall-algorithm-flow)

---

## Overview

The `CONVECNAT` simulation mode solves a 2D incompressible pressure-correction algorithm with variable mixture properties and Boussinesq-like buoyancy on unstructured triangular meshes. The solver supports:

- **Buoyancy-driven flows** via temperature-dependent density (Boussinesq-like)
- **Two-phase flows** via Volume of Fluid (VOF) interface tracking with holdup-dependent mixture properties
- **Non-Newtonian fluids** via viscosity look-up tables (temperature and deformation-rate dependent)
- **Coupled 1D wall heat transfer** via the `TransCal` module
- **Laminar flow only** — no turbulence models are implemented

Typical applications include natural convection in annular spaces, subsea equipment thermal analysis, and wellbore heat transfer in complex geometries.

---

## Entry Point and Initialization

When `tipoSimulacao == convecNatural`, `main()` in [`Num4Main.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Num4Main.cpp) constructs a `solv2D` object named `resolucao` and calls `resolucao.resolve()`.

### Construction

```cpp
varGlob1D vg1dConvec2D = varGlob1D();
solv2D resolucao(nomeArquivoEntrada, nomeArquivoLog, &vg1dConvec2D);
```

The constructor:

1. Parses the input JSON file specified by `nomeArquivoEntrada`
2. Opens a log file at `nomeArquivoLog`
3. Loads the mesh (Triangle or UNV format)
4. Allocates field arrays per element (`uC`, `vC`, `presC`, `tempC`, `holC`)
5. Sets initial and boundary conditions
6. Initializes material properties

### Key `solv2D` class members

| Member | Type | Description |
|--------|------|-------------|
| `malhaH` | `detMalhaHom**` | Array of mesh data structures |
| `nomeArquivoEntrada` | `string` | Path to input JSON file |
| `nomeArquivoLog` | `string` | Path to log output file |
| `flucVF` | `ProFluColVF` | VOF/holdup fluid properties struct |
| `corteVF` | `cortedutoVF` | Cross-section and layer definitions |
| `matVF` | `materialVF*` | Array of material properties |
| `vg1dSP` | `varGlob1D*` | Pointer to global variables (CFL, relaxation factors, gravity, angles) |
| `interface` | `detInterFace` | Interface tracking configuration |
| `nPrime` | `int` | Number of PISO corrector steps |
| `solverMat` | `int` | Linear solver selector (see Linear Solvers section) |
| `rankLU` | `int` | ILU preconditioner fill level (-1 = disabled) |
| `unv` | `int` | Mesh format flag (0 = Triangle, 1 = UNV) |

### Element-level data structure (`cel2D`)

Field variables are stored **per-element** via the `cel2D` struct inside each `elem2d`:

| Member | Type | Description |
|--------|------|-------------|
| `uC`, `vC` | `double` | Velocity components at element centroid |
| `presC` | `double` | Pressure at element centroid |
| `tempC` | `double` | Temperature at element centroid |
| `holC` | `double` | VOF holdup (volume fraction, 0–1) |
| `gradGreenU`, `gradGreenV` | `double[2]` | Green-Gauss gradients of velocity |
| `gradGreenPres` | `double[2]` | Green-Gauss gradient of pressure |
| `gradGreenTemp` | `double[2]` | Green-Gauss gradient of temperature |
| `vElem` | `double` | Element volume (area for 2D) |
| `nvert` | `int` | Number of vertices (typically 3 for triangles) |
| `dim` | `int` | Spatial dimension (2 for 2D) |
| `indFace[3]` | `int` | Neighbor element indices (-1 for boundary faces) |
| `sFace[3][2]` | `double` | Face area vectors (outward normal × length) |
| `centroideElem[2]` | `double` | Element centroid coordinates |

---

## Mesh Formats

### Triangle format (`.node` / `.ele`)

The primary mesh format. Generated by the [Triangle](https://www.cs.cmu.edu/~quake/triangle.html) mesh generator:

- **`.node` file**: node coordinates and boundary markers
- **`.ele` file**: element connectivity (3 nodes per triangle)

Loaded by the `malha2dVF` constructor and mesh parsing functions in `solver.cpp`.

### UNV / Salome format (`.unv`)

Alternative mesh format from the [Salome](https://www.salome-platform.org/) platform:

- Universal file format with numbered datasets
- Supports named groups for boundary regions

Enabled by setting `unv = 1` in the solver.

### Per-element mesh data structure

Unlike traditional CFD codes with a global face list, this solver stores **face information at the element level** via the `cel2D` struct:

| Field | Description |
|-------|-------------|
| `noElem[0..2]` | Three node indices of the triangle element |
| `indFace[0..2]` | Neighbor element across each face (-1 for boundary) |
| `ccFace[0..2]` | Boundary condition label for each face |
| `sFace[k][0..1]` | Face area vector (pointing outward from element) |
| `sFaceMod[k]` | Face length (modulus of `sFace`) |
| `centroideFace[k][0..1]` | Midpoint of face $k$ |
| `centroideElem[0..1]` | Element centroid coordinates |
| `vElem` | Element volume (area for 2D) |
| `vecE[k][0..1]` | Unit vector from centroid to neighbor centroid |
| `modE[k]` | Distance between cell centroids |
| `fatG[k]` | Geometric interpolation factor (0–1) |
| `angES[k]` | Cosine of angle between face normal and cell-to-cell vector |

Neighbor connectivity is resolved via pointers (`vizinho[k]`) in the `elem2d` class, not indices.

---

## JSON Configuration

The input JSON file (typically `parametros.json`) contains all 2D solver settings. The main entry point (`solv2D::resolve()`) parses JSON sections in this order:

```json
{
  "malha": {
    "arquivo": "mesh.node",
    "unv": 0
  },
  "configuracaoInicial": {
    "acopPV": 0,
    "razDtMinimo": 0.01,
    "cicloSeguranca": 100,
    "rankLU": -1,
    "solverMat": 0,
    "CFL": 0.5,
    "acop": 0
  },
  "CC": { },
  "prop": { },
  "CI": { },
  "fluido": {
    "rhogStd": 0.7,
    "temp1": 20.0,
    "visc1": 0.001,
    "temp2": 100.0,
    "visc2": 0.0005
  },
  "material": [
    {
      "tipo": 1,
      "condutividade": 0.6,
      "calorEspecifico": 4182.0,
      "rho": 998.0,
      "visc": 0.001,
      "beta": 0.000207
    }
  ]
}
```

| Section | Description |
|---------|-------------|
| `malha` | Mesh file path and format flag (`unv`: 0=Triangle, 1=UNV) |
| `configuracaoInicial` | Time integration settings: `acopPV` (pressure-velocity coupling method), `CFL`, `razDtMinimo`, `cicloSeguranca`, `rankLU`, `solverMat` |
| `CC` | Boundary condition definitions |
| `prop` | Global flow properties (density, viscosity, thermal properties) |
| `CI` | Initial conditions |
| `material` | Array of materials with `tipo` (0=solid, 1=fluid), `condutividade`, `calorEspecifico`, `rho`, `visc`, and optionally `beta` (thermal expansion) |
| `fluido` | VOF fluid properties and temperature-dependent viscosity table |

**Implementation note:** The JSON parsing sequence in `solv2D::resolve()` is: `malha` → `configuracaoInicial` → `CC` → `prop` → `CI` → `interface` (if present) → `mapProp` (if present) → `fluido` (if present) → `material` (if coupled 1D wall).

---

<a id="physical-models"></a>
## Governing Equations

### Momentum (incompressible Navier–Stokes with variable density)

$$\frac{\partial (\rho u_i)}{\partial t} + \nabla \cdot (\rho \mathbf{u} u_i) = -\frac{\partial P}{\partial x_i} + \nabla \cdot (\mu \nabla u_i) + S_i$$

where $S_i$ is the body-force term (gravity + buoyancy variation, depending on temperature and/or holdup):

$$S_y = -\rho \cdot \text{mulFC} \cdot g \cdot \sin(\theta_y) + \rho \cdot \beta \cdot (T - T_{\text{ref}}) \cdot g \cdot \sin(\theta_y)$$

Here `mulFC` is a multiplier for body force (typically 1.0), $g$ is gravitational acceleration (`gravVF`), and $\theta_y$ is the gravity angle (`angY`). The code implements this via the `B2Med[1]` and `BMedF[k][1]` fields.

### Continuity (pressure equation)

$$\nabla \cdot \mathbf{u} = 0$$

Enforced through the pressure-correction equation derived from the SIMPLE-based algorithm.

### Energy

$$\frac{\partial (\rho C_p T)}{\partial t} + \nabla \cdot (\rho C_p \mathbf{u} T) = \nabla \cdot (k \nabla T) + \Phi$$

where $\Phi$ is the viscous dissipation term:

$$\Phi = \mu \left[ 2\left(\frac{\partial u}{\partial x}\right)^2 + 2\left(\frac{\partial v}{\partial y}\right)^2 + \left(\frac{\partial u}{\partial y} + \frac{\partial v}{\partial x}\right)^2 \right]$$

### VOF phase transport (optional)

$$\frac{\partial h}{\partial t} + \nabla \cdot (\mathbf{u} h) = 0$$

where $h \in [0, 1]$ is the **holdup** (volume fraction), stored as `holC` in the code. Unlike the previous $\alpha$ notation, the code uses `holC`/`holF` naming throughout.

Mixture properties are interpolated at faces; however, the actual implementation is more nuanced:
- Cell-level mixture properties are updated from `holC` (volume fraction) and can also depend on temperature and deformation rate
- Face-level viscosity may be recomputed from `holF`, pressure, temperature, and strain-rate for non-Newtonian cases
- Simple linear blending is used for density and conductivity, but viscosity follows tabulated properties when available

Basic blend (when table is not used):
$$\rho_f = h \cdot \rho_1 + (1 - h) \cdot \rho_2, \quad \mu_f = h \cdot \mu_1 + (1 - h) \cdot \mu_2$$

---

## FVM Discretization

All equations are discretized using the cell-centered finite-volume method on triangular elements.

### Diffusion flux

For a face $f$ between elements $P$ (current) and $N$ (neighbor), the diffusion flux uses **linear interpolation** with geometric weighting:

$$F_{\text{diff}} = \Gamma_f \cdot \frac{\phi_N - \phi_P}{|\mathbf{d}_{PN}|} \cdot |S_f|$$

where $\Gamma_f$ is obtained by linear interpolation using the geometric factor `fatG`:

$$\Gamma_f = \text{fatG}_f \cdot \Gamma_P + (1 - \text{fatG}_f) \cdot \Gamma_N$$

**Cross-diffusion correction**: On non-orthogonal meshes, a correction term accounts for the misalignment between the face normal $\hat{n}$ and the cell-to-cell vector $\vec{E}$:

$$F_{\text{cross}} = \Gamma_f \left( \nabla \phi_f \cdot \vec{T} \right)$$

where $\vec{T}$ is the vector from the interpolated face point to the actual face centroid (stored in code as `fInter` to `centroideFace`).

### Convection flux - NVD High-Resolution scheme

Convective terms use a **Normalized Variable Diagram (NVD)** scheme controlled by `nvfHR`:

| `nvfHR` | Scheme | Description |
|---------|--------|-------------|
| 0 | Upwind | First-order |
| 1 | SMART | Sharp and Monotonic Algorithm for Realistic Transport |
| 2 | Minmod | Minimum modulus limiter |
| 3 | Osher | Osher limiter |
| 4 | MUSCL | Monotone Upstream-Centered Scheme |

The normalized variable $\tilde{\phi}$ is computed as:

$$\tilde{\phi} = \frac{\phi_{C} - \phi_{U}}{\phi_{D} - \phi_{U}}$$

where:
- $\phi_{C}$ = value at upwind cell centroid
- $\phi_{U}$ = extrapolated upwind value (2 cells upstream)
- $\phi_{D}$ = value at downwind cell centroid

**Note**: This is **not** the classical TVD ratio $r = (\phi_C - \phi_U)/(\phi_D - \phi_U)$ as the documentation previously implied.

### Rhie–Chow-like momentum interpolation

The solver computes face velocities using a momentum interpolation that includes:
1. Linear interpolation of cell velocities
2. Correction for pressure gradient difference (similar to Rhie-Chow)
3. Correction for body force difference (buoyancy-driven flow)

The implementation in `vazMass()` computes:

$$\mathbf{u}_f^{\text{RC}} = \overline{\mathbf{u}} - d_f \left( \nabla P - \overline{\nabla P} \right) + \text{(buoyancy corrections)}$$

where $d_f = \overline{a_P^{-1}}$ is the average inverse momentum coefficient (`difuPresRC`).

---

<a id="linear-solvers"></a>
## Pressure-Velocity Coupling

### SIMPLE-based algorithm variants

The solver implements a pressure-velocity coupling controlled by `metodoAcopPV` (stored in `varGlob1D`). Supported values are:

| `metodoAcopPV` | Variant | Description |
|----------------|---------|----------|
| -1 | Legacy | Deprecated; skips certain convergence checks |
| 0 | Basic | Standard SIMPLE approach |
| 1 | Modified | With modified coefficients for certain terms |
| 2 | Enhanced | Relaxation factor set to 1.0; direct explicit velocity update |
| 3 | With PISO | SIMPLE followed by PISO corrector steps (if `nPrime > 0` and convergence criteria met) |
| 4 | Alternate PISO | Alternative PISO corrector sequence (if `nPrime > 0` and convergence criteria met) |

### SIMPLE iteration cycle

1. Solve **momentum equations** for $u^*$, $v^*$ with current pressure field
2. Compute face mass fluxes using momentum interpolation (Rhie-Chow-like)
3. Assemble and solve the **pressure correction equation** derived from continuity constraint
4. Correct velocities: $\mathbf{u} = \mathbf{u}^* - d_P \nabla P'$ (applied as `uC -= difuPres * gradGreenPresCor`)
5. Correct pressure: $P = P^* + \alpha_P \cdot P'$ (applied with `relaxVFPcor`)
6. Check convergence (mass imbalance residual)

### PISO corrector steps

When `nPrime > 0`, additional corrector iterations are applied **only if** certain conditions are met:
- `metodoAcopPV` must be 3 or 4
- Velocity residual norm must be improving (`norma1Antiga > norma1`)
- Pressure or velocity errors must exceed thresholds (`norma0 > erroPres` or `norma1 > erroV`)

When these conditions are satisfied, the loop executes `kontaPiso <= nPrime` corrector steps, recomputing face fluxes and applying additional pressure corrections without re-solving the momentum equations.

Note: The member variable is named **`nPrime`** (not `nPISO`), representing the number of PISO-style corrector steps.

---

## Gradient Reconstruction

Cell gradients are computed using the **Green–Gauss cell-based** method with upwind extrapolation:

$$ \nabla \phi_P^{n+1} = \frac{1}{V_P} \sum_{\text{faces}} \phi_f \, \vec{S}_f $$

where:
- $\vec{S}_f$ is the face area vector (`sFace`)
- $\phi_f$ includes both interpolated cell values and deferred correction terms

For the NVD scheme, gradients are also used to compute upwind extrapolation:

$$\phi_U = \phi_D \mp 2 (\nabla \phi \cdot \vec{E}) \cdot |\vec{E}|$$

The sign depends on flow direction (via `massF >= 0` check).

Gradient arrays in the code:
- `gradGreenU`, `gradGreenV` — velocity gradients
- `gradGreenPres` — pressure gradient (for Rhie-Chow)
- `gradGreenPresCor` — pressure correction gradient (for velocity update)
- `gradGreenTemp` — temperature gradient
- `gradGreenHol` — holdup (VOF) gradient

Suffix `I` (e.g., `gradGreenUI`) indicates "initial" or previous-iteration values used for deferred corrections.

---

## Boundary Conditions

### Momentum BCs

| Type | Code Label | Treatment |
|------|------------|-----------|
| **Inlet** | `ccInl` | Fixed velocity: $u = u_{\text{in}}$, $v = v_{\text{in}}$. Values interpolated from time series. |
| **Outlet** | `ccPres` (pressure BC on outlet) | Zero-gradient velocity. Pressure set to specified value from series. |
| **Wall** | `ccWall` | Wall velocity magnitude (scalar). Face velocity is projected via: $u_f = \text{ccWall} \cdot |{-s_y}/{\|s\|}|$, $v_f = \text{ccWall} \cdot |{s_x}/{\|s\|}|$. Typically `ccWall=0` for no-slip. |
| **Symmetry** | `ccSim` | Zero normal velocity: $\mathbf{u} \cdot \hat{n} = 0$. Implemented via velocity component elimination. |

In the code, boundary condition values are stored per-face in arrays like `ccInU[k]`, `ccInV[k]`, `ccWall[k]`, etc., updated via `atualizaCC()`.

### Thermal BCs

| Type | Code Label | Treatment |
|------|------------|-----------|
| **Dirichlet** | `ccTD` | Fixed temperature: $T = T_{\text{wall}}$ (stored in `ccTD[k]`) |
| **Von Neumann** | `ccTVN` | Fixed heat flux: $q = q_{\text{specified}}$ (stored in `ccTVN[k]`) |
| **Robin/Convective** | `ccTambR`, `ccHR` | Convective: $q = h (T_{\text{amb}} - T)$ using ambient temperature and heat transfer coefficient arrays |
| **Coupled 1D Wall** | `rotuloAcop` | Temperature linked to 1D wall conduction model. Treated alongside Von Neumann in assembly (when `acop == 1`) |

Updated via `atualizaCCTemp()` with time-series interpolation using `indraz()`.

### VOF BCs

At inlets: `holC` inlet specification is currently a **TODO** — the implementation sets `holF[i] = 1.0` with a note to modify to use input value. At outlets: zero gradient (face value equals cell value). At walls: zero gradient or contact angle handled via CICSAM scheme (when `nvfHRHol == 1`).

---

<a id="time-stepping-and-convergence"></a>
## 1D Wall Coupling

When one-dimensional wall heat transfer is enabled, the 2D fluid domain is thermally coupled to a 1D radial wall conduction model via the `TransCal` module. See the full documentation in [`heat-transfer.md`](heat-transfer.md).

### Activation

Wall coupling is triggered when:
- `vg1dSP->acop == 1` (activation flag in global parameters)
- Boundary faces are labeled with the coupled thermal BC (matched against `CC.rotuloAcop`)

### Coupling mechanism

1. **Initialize wall data structures**: `paredeContorno()` identifies coupled boundary elements and maps wall segments via `unordered_map<int,int>` indices (`indPar`, `indPar2`)
2. **Create TransCal instances**: Array of `TransCal` objects (`vecTransfer`) instantiated with:
   - Multi-layer wall properties (`vncamada`, `vdrcamada` — number of layers and radii)
   - Wall temperature profiles (`vTcamada`)
   - Material properties from `matVF` array
3. **Compute heat flux**: At each coupled face, the 2D solver computes convective heat flux based on fluid temperature and wall temperature boundary condition
4. **Solve wall conduction**: `TransCal` solves the radial 1D heat equation (steady `transperm()` or transient `transtrans()`) through pipe/annulus wall layers
5. **Update wall temperature**: The new wall temperature is returned and applied as thermal boundary condition for coupled faces
6. **Iterate to convergence**: Within each time step, the coupling iterates until temperature residuals fall below tolerance

This allows simulation of heat transfer through multi-layer pipe walls (steel, insulation, concrete coating) and buried pipe soil conduction (via 2D/3D Poisson extensions in `TransCal`), with the 2D fluid domain resolving internal convection patterns.

### Related implementation notes

| Function | Source file | Purpose |
|----------|-------------|---------|
| `TransCal::transperm()` | `TrocaCalor.cpp` | Steady-state radial resistance network |
| `TransCal::transtrans()` | `TrocaCalor.cpp` | Transient radial thermal solver |
| `solv2D::paredeContorno()` | `solver.cpp` | Identify coupled boundary faces |
| `elem2d::tipoCCTemp()` | `Elem2D.cpp` | BC classification including `acoplado` flag |
| `solv2D::parse_materiais()` | `solver.cpp` | Material property parsing (note: possible bug where `beta` is assigned to `visc`) |
| `elem2d::calcGradGreenTemp()` | `Elem2D.cpp` | Temperature gradient with coupled BC handling |
---

<a id="output-formats"></a>
## Overall Algorithm Flow

The simulation proceeds in the following steps:

1. **Initialization**: Load mesh, set initial conditions, initialize material properties
2. **Time-stepping loop**:
   - **Momentum equations**: Solve for $u^*$, $v^*$ with current pressure field
   - **Face fluxes**: Compute mass fluxes using momentum interpolation
   - **Pressure correction**: Assemble and solve the pressure correction equation
   - **Velocity correction**: Correct velocities using pressure gradient
   - **Pressure correction**: Correct pressure using relaxation
   - **Convergence check**: Check for mass imbalance residual
3. **PISO corrector steps**: Apply additional corrector iterations after pressure-velocity coupling
4. **Output**: Save results to file

This is a simplified overview. The actual implementation involves more complex logic and additional steps.

---
