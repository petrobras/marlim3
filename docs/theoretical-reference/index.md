# Theoretical Reference

Mathematical foundations of the `Marlim3` transient drift-flux model.

`Marlim3` solves a one-dimensional, three-phase (gas–liquid–liquid) drift-flux system coupled to an energy equation and a finite-volume discretization on a staggered grid. These pages document the governing equations and the numerical schemes implemented in the solver.

## Contents

| Page | Description |
|------|-------------|
| [Drift-Flux vs. Two-Fluid](two-fluids_vs_drift-flux.md) | Conceptual comparison between the drift-flux and two-fluid formulations, including interfacial coupling, wave families, and drift-flux closure concepts |
| [Mass & Momentum Balances](mass-momentum-balances.md) | Drift-flux mass conservation equations, momentum balance, and flow-pattern closure relations |
| [Energy Balance](energy-balance.md) | One-dimensional energy conservation equations for gas and liquid phases, heat-flux terms, and coupling to the thermal model |
| [Discretization](discretization.md) | Staggered finite-volume grid, semi-implicit time-stepping scheme, and treatment of multiphase flow-regime transitions |
| [Service Line Model](service-line.md) | Single-phase gas service-line equations, staggered-grid discretization, boundary conditions, and coupling with the production line |
