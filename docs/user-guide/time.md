# Time

Simulation time controls how the model evolves in time — from preparation of the initial state, to advanced scheduling of solver steps, to generating restart points. The time configuration defines:

- How the simulation starts (initial conditions)
- How the solver adapts the time-step limits throughout the run
- When the simulator saves snapshot files for restart or post-processing
- The final simulation time

> **JSON key:** `time` (EN/PT) · `tempo` (PT) — top-level object

---
## Simulation Mode
Choose between steady-state or transient simulation.

- `transient: false` → run only steady-state calculation and stop
- `transient: true` → start with transient simulation

> **JSON key:** `initialConfig.transient` (EN) · `configuracaoInicial.transiente` (PT)  
> Default: `false`

---
## Initial Condition Strategy
Controls how the initial state is established:

| Value | Meaning                                 |
|-------|-----------------------------------------|
| 0     | User-defined initial fields             |
| 1     | Initialize from steady-state solution   |
| 2     | Restart from snapshot file (`.snp`)     |
| 3     | Gas-lift unloading initialization       |

> **JSON key:** `initialConfig.initialCondition` (EN) · `configuracaoInicial.condicaoInicial` (PT)  
> Default: `1`

### Option 0: User-Defined Initial State
Explicitly set initial fluid fields in pipe cells.

> **JSON key:** `initialConfig.initialFluidId` (EN) · `configuracaoInicial.iniFluidoP` (PT)

### Option 2: Snapshot Restart
Initialize the simulation from a snapshot state file.

> **JSON key:** `initialConfig.snapshotFile` (EN) · `configuracaoInicial.SnapShotArq` (PT)

!!! warning
    If `initialCondition` = 2 is selected and the snapshot file does not exist, parsing fails.

### Option 3: Gas-Lift Unloading
Specialized initialization for gas-lift operations. Requires:

- `gasLineInterfaceLength` · `comprimentoMedidoInterfaceLinhaGas`
- `prodLineInterfaceLength` · `comprimentoMedidoInterfaceLinhaProd`
- `fluidSalinity` · `SalinidadeFluido`  

Optionally:  

- `dischargeControl` · `controleDescarga`
- `dischargeParameters` · `parametrosDescarga`

---
## Time Scheduling

Define how the solver advances in time, controls adaptive time-step size, and sets simulation stop conditions.

Core fields:

- `finalTime` · `tempoFinal` [s]: Total simulated time (when to stop)
- `times` · `tempos` [s]: Time points where the time-step schedule changes  
- `maxDT` · `dtmax` [s]: Maximum allowed step size for solver between each time interval

The schedule allows piecewise control: between `times[i]` and `times[i+1]`, the solver will not exceed `maxDT[i]`, though it may choose smaller steps internally for stability.

> **JSON key:** `time.finalTime`, `time.times`, `time.maxDT`

**Parser checks:**

- `times` must be monotonically increasing and start at 0
- `times` and `maxDT` arrays must be the same size

**Defaults:**

- If missing, `times = [0]` and `maxDT = [5]`

---

## Segregation Scheduling

Schedules time windows for special “segregation” mode, used in shutdown or fallback scenarios.

- `segregationTime` (EN) · `tempoSegrega` (PT): array (s), when mode changes apply
- `segregation` (EN) · `segrega` (PT): array, mode indicator (0 = normal, 1 = segregation)

**Behavior:**  

If these are omitted, a default schedule is created at `t = 0` in segregation mode.

---
## Snapshot Save Times

Controls when the simulation saves `.snp` or `.snt` snapshot files for restart or analysis.

> **JSON key:** `saveSnapshot` (EN) · `gravaMomento` (PT)
Each entry is a time [s] at which a snapshot is saved.

---
## Practical Guidance

- **Adaptive scheduling:** Define short timesteps for fast transients (startup, shut-down), then relax time-step for slower phases.
- **Restarts:** Use `saveSnapshot` to schedule checkpoint writes — especially useful before events requiring operator review or slow, manual steps.
- **Segregation windows:** For simulations with production shutdowns, use segregation scheduling to better control step size and convergence during sensitive transitions.

---
## Example: Standard Transient

```json
{
  "initialConfig": {
    "transient": true,
    "initialCondition": 1,
    "initialFluidId": 0
  },
  "time": {
    "finalTime": 3600,
    "times": [0, 10, 100],
    "maxDT": [0.1, 1.0, 5.0],
    "saveSnapshot": [1800, 3600]
  }
}
```

---
## Example: Restart from Snapshot

```json
{
  "initialConfig": {
    "transient": true,
    "initialCondition": 2,
    "snapshotFile": "shutdown_3600s.snp"
  },
  "time": {
    "finalTime": 7200,
    "times": [0, 100],
    "maxDT": [1.0, 5.0]
  }
}
```

---
## Example: Segregation Window

```json
{
  "time": {
    "finalTime": 86400,
    "times": [0, 60, 600, 3600],
    "maxDT": [0.5, 2.0, 10.0, 30.0],
    "segregationTime": [0, 600],
    "segregation": [0, 1]
  }
}
```