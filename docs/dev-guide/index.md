# Marlim3 Developer Documentation

Internal architecture of `Marlim3` core engine.

## Contents

| Page | Description |
|------|-------------|
| [Model overview](model-overview.md) | Overview of the computational model tree hierarchy |
| [Num4Main.cpp — Main Entry Point](num4main.md) | Main source file containing `main()`, global variable declarations, network convergence data structures, and top-level solver orchestration for steady-state and transient simulations |
| [Core Domain Classes](core-domain-classes.md) | High-level overview of the main classes composing a simulation: each class's role, responsibilities, and relationships |
| [Input Parsing and SProd Construction](input-parsing.md) | How the simulator reads a JSON input file, validates it, maps its contents to internal data structures, and constructs the fully initialized `SProd` simulation object |
| [Steady-State Solvers](steady-state-branch.md) | Logic and algorithms used to compute the steady-state (permanent) solution of a single pipeline branch, including artificial-lift devices, sources, and sinks |
| [Transient Solver](transient-solver.md) | Time-dependent solution of a single branch: drift-flux multiphase model on a staggered grid, coupling mass, momentum, and energy equations through semi-implicit time-stepping |
| [Network Simulation](network-simulation.md) | Solving flow networks of multiple connected branches: topology pre-processing, graph decomposition, steady-state convergence, transient time-stepping, and supported network topologies |
| [Fluid Thermodynamic Modeling](fluid-thermodynamics.md) | Fluid property modeling framework — black-oil correlations, compositional EOS calculations, PVT properties, and phase equilibrium |
| [Heat Transfer Modeling](heat-transfer.md) | Radial thermal resistance network, convection correlations, formation conduction, 2D/3D soil diffusion, and coupling to the axial energy equation |
| [Embedded 2D/3D Solvers](embedded-solvers.md) | Specialised multi-dimensional solvers embedded inside pipeline cells: radial/axial heat conduction through complex pipe walls and near-wellbore reservoir flow with multiphase saturations |
| [Parametric Analysis](parametric-analysis.md) | Automated execution of multiple steady-state simulations over a user-defined parameter space, producing VFP tables and summary reports for each combination |
| [Injection Well Simulation](injection-well.md) | Steady-state injection well mode (`-s INJETOR`): flow from surface down to one or more injection zones |
| [2D Natural Convection Solver](natural-convection.md) | Natural convection simulation mode (`-s CONVECNAT`): laminar incompressible flow with heat transfer in 2D domains using FVM on unstructured triangular meshes |
| [Translations](translations.md) | Bilingual JSON key translation system: mapping between English and Portuguese input keys driven by `marlim3/translations.json` |