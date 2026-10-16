---
name: marlim3-time-transient
description: Use when configuring steady-state vs transient mode, initial-condition strategy (user-defined, steady-state, snapshot restart, gas-lift unloading), the time-step schedule (finalTime, times, maxDT), segregation windows, and snapshot saving for a Marlim3 simulation.
---

# Marlim3 Time & Transient Configuration

## Authoritative files

- [docs/user-guide/time.md](../../../docs/user-guide/time.md) — full time-block reference with examples (**read this**)
- [docs/user-guide/general.md](../../../docs/user-guide/general.md) §2, §12 — simulation-mode flags and unloading
- [docs/tutorials/5_temperature_pulse.ipynb](../../../docs/tutorials/5_temperature_pulse.ipynb) — complete transient walkthrough

## Mode selection — `initialConfig`

- `transient: false` (default) → steady-state only; the `time` block is not needed.
- `transient: true` → transient run; **requires** top-level `time` with `finalTime`.

## Initial-condition strategy — `initialConfig.initialCondition`

| Value | Meaning | Extra requirements |
|-------|---------|--------------------|
| 0 | User-defined initial fields | per-segment `initialConditions` profiles (`pressure`, `temp`, `holdup`, `usl`, `usg`, …); optional `initialFluidId` (default 0) |
| 1 | From steady-state solution (**default**) | — |
| 2 | Restart from snapshot | `snapshotFile` (`.snp`) — parsing fails if missing |
| 3 | Gas-lift unloading | interface lengths + salinity (see artificial-lift skill) |

## Time schedule — top-level `time` object

```json
"time": {
  "finalTime": 3600,
  "times":  [0, 10, 100],
  "maxDT":  [0.1, 1.0, 5.0],
  "saveSnapshot": [1800, 3600]
}
```

- `finalTime` [s] — total simulated time.
- `times`/`maxDT` — piecewise cap on solver step: between `times[i]` and `times[i+1]` the step never exceeds `maxDT[i]` (solver may go smaller). Must be equal length; `times` monotonic starting at 0. Defaults if omitted: `times=[0]`, `maxDT=[5]`.
- `saveSnapshot` (PT `gravaMomento`) — instants [s] to write `.snp`/`.snt` restart files.

Design the schedule around events: small `maxDT` (0.1–1 s) during valve closures, startups, and pulses; relax (5–30 s) in quiet phases.

## Segregation windows (shutdown scenarios)

`segregationTime` (PT `tempoSegrega`) + `segregation` (PT `segrega`, 0 = normal, 1 = segregation) schedule a special mode for liquid-gas segregation during shutdowns. If omitted, a default schedule starts at t=0 in segregation mode.

## Related knobs (see advanced-settings skill for the full list)

- `initialConfig.steadyGuess` — initial guess for the steady-state solver.
- Timestep penalization controls (`relaxChokeTimestep`, `valveTimestepControl`, `disablePenalizeTimestep`) — leave at defaults unless justified.
- Sonic capture: `sonicTime`/`sonicFlag` arrays (limited first-order resolution — mitigation only).

## Validation checklist

- `transient: true` ⇒ `time.finalTime` present; profile/trend output times ≤ `finalTime`.
- `times` and `maxDT` same length; `times[0] = 0`, strictly increasing.
- `initialCondition: 0` ⇒ initial profiles present on every segment; `2` ⇒ snapshot file exists next to the input.
- Trend `dt` values positive and much smaller than event durations you want to resolve.
