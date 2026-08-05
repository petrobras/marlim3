---
name: marlim3-advanced-settings
description: Use when tuning Marlim3 solver behavior, performance, or special models — slip/drift correlations, mass-transfer model, threading, precomputed tables, convergence and timestep controls in the initialConfig advanced block, sensitivity analysis, wax, and 3D thermal diffusion. Only needed beyond defaults.
---

# Marlim3 Advanced Settings (`initialConfig` + `advanced` block)

**Golden rule:** keep defaults until a baseline result is validated; change one parameter at a time.

## Authoritative files

- [docs/user-guide/general.md](../../../docs/user-guide/general.md) — every flag below with defaults and warnings (**read this**)
- [docs/tutorials/4_black_box_correlations.ipynb](../../../docs/tutorials/4_black_box_correlations.ipynb) — black-box correlation usage

## Physics & correlation selection (`initialConfig`)

- `steadyStateSlip` / `transientSlip` — slip modeling (default true/true; keep on).
- `driftModel` — true = drift-flux closure (default; forced in transient); false = black-box steady-state correlations selected per pipe via `productionPipe[].ssFlowModel` (PT `correlacaoMR2`).
- `flowPatternMap` (PT `mapaArranjo`): 0 Barnea simplified (default), 1 Barnea complete.
- `correlationsByPattern` (PT `correlacoesPorArranjo`): per-pattern slip correlation — `stratified` (0 Choi, **1 Bhagwat&Ghajar default**, 2 França&Lahey, 4 modified B&G), `slugBubble` (0/1/4), `annularChurn` (0/1/3 Hibiki&Ishii/4).
- `massTransfer` (PT `transferenciaMassa`): **0 full implicit (default)**, 1 full explicit, 2 simplified isothermal, 3 none.
- `thermalEquilibrium` (default true), `steadyStateOrder` (1 default / 2 RK2 steady-state only), `steadyGuess`, `reverseTemp`.

## Performance

- `pressureTable` / `gasTable` + `compTable` grid — precomputed property tables (recommended for long transients; see fluid skill).
- `RsPbTable` — pre-build Rs table (essential with `RsPbModel: 4`).
- `advanced.threads` (PT `nthrd`, default 1) and `advanced.matrixThreads` — parallel execution.
- `dynamicTableModel` — posterior compositional tables for steady-state network cases.

## `initialConfig.advanced` block — key knobs (defaults in parentheses)

- Thresholds: `monophasicCriterion` (1e-4), `condensationCriterion` (1e-3).
- Steady solver: `accelerateSteadyConvergence` (true), `steadyConvergenceCriterion` (0.001), `regulaFalsiSearchCriterion` (0.1 — reduce for high-IPR pre-salt cases), `strongAnnularColCoupling` (0), `counterflowCorrectionSteady` (false), `correctSepCondition` (true).
- Pressure/boundary: `simplePressureFrontier` (true — keep true), `slipBoundaryCell` (true — disable only for last-cell oscillations).
- Timesteps: `relaxChokeTimestep` (false), `valveTimestepControl` (false), `disablePenalizeTimestep` (false — ⚠️ do not disable casually), `minTimestepCriterion` (unset).
- Latent heat: `massTransferLimit` (10.0 kg/(s·m)); global `latentHeatCond` (true).
- Full-model switching: `compModelCorrectionTime`/`compModelCorrectionFlag` arrays, `despressRate` (0.01), `columnStabilization` (false).
- Sonic: `sonicTime`/`sonicFlag` (mitigation only — first-order model).
- Compositional transient mini-tables: `dynTableMinDelay`/`dynTableMinDp`/`dynTableMinDt` (0).
- Misc: `areaChange` (0 → 1 adds localized losses at area changes), `liquidJTSimple` (0), `disableMassTransferTempDeriv` (false).

## Special features

- `sensitivityAnalysis` (PT `AS`) + `parallelizeSA` — steady-state sensitivity study driven by an auxiliary `leituraAS.json` (see [demos/leituraAP.json](../../../demos/leituraAP.json) for the related parametric format).
- `waxMode` + top-level `wax` object — wax deposition.
- `diffusion3dMode` + `diffusion3dJson` + `diffusion3dThreads` — 3D thermal diffusion coupled to 1D flow.
- `severeSlugging` top-level object — severe-slugging detection.
- `classicOutput`, `screenPrint` — cosmetic output controls.

## When QA should flag advanced settings

- Any non-default advanced value without a justification in the ADR.
- `disablePenalizeTimestep: true` or `simplePressureFrontier: false` — require explicit rationale.
- Threads > available cores; table grids not covering the P-T envelope.
