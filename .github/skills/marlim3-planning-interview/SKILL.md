---
name: marlim3-planning-interview
description: Use when planning a Marlim3 simulation from a user's natural-language request — how to interview the user with structured questions, which decisions must be captured, safe engineering defaults, and the ADR (Architecture Decision Record) template written to docs/<slug>.adr.md that drives implementation.
---

# Marlim3 Planning Interview & ADR Authoring

The planner turns a request like "simulate a shutdown of a gas-lifted well" into a complete, implementable ADR. The ADR is the **single source of truth** for the specialist and QA.

## Interview protocol

Use the ask-questions tool (`vscode/askQuestions`) in **batches of related questions** (3–6 per batch), offering concrete options with a recommended default plus free-text. Never assume silently: anything not answered must appear in the ADR's "Assumed defaults" table. Ask in this order, skipping domains the request already answers:

### Batch 1 — System scope
1. Producer (`PROD`) or injector (`INJ`)? Gas-lift/service line present?
2. Steady-state only, or transient? If transient: what event (shutdown, startup, unloading, pulse…) and total simulated time?
3. Deliverable: `.mr3` JSON file, Python script, or both? Where to save (default: `simulations/<slug>/`)?

### Batch 2 — Fluids
4. Fluid model: black-oil (default) / flash table (.tab) / compositional (.ctm)? PVT file available?
5. API, GOR [sm³/sm³], gas relative density, BSW, CO₂ fraction?
6. Complementary fluid (glycol/brine/inhibitor)? Emulsion behavior known?

### Batch 3 — Geometry & thermal
7. Line topology and lengths: well depth (vertical/deviated), flowline length, riser height; diameters (ID) per section; annular flow anywhere?
8. Wall construction per section: steel thickness, insulation, cement, completion-fluid annulus?
9. Environment: water depth/seabed temperature (default 4 °C), formation properties, production history [days]?

### Batch 4 — Boundary conditions & equipment
10. Inflow: IPR (static pressure, PI, reservoir temperature) or prescribed rate source?
11. Outlet: separator pressure schedule? Surface choke?
12. Equipment: gas-lift valves (positions, type), ESP (curves available?), master valves, PIG?

### Batch 5 — Events, outputs, acceptance
13. Transient events timeline (valve ramps, injection changes) with times.
14. Outputs: profile variables/times; trend gauge positions and `dt`; cross-section outputs for cooldown?
15. Acceptance criteria: what makes the simulation correct (e.g., "steady-state converges, bottomhole pressure between X and Y", "temperature stays above WAT for 8 h")?

## Safe defaults (record every one used in the ADR)

| Item | Default |
|------|---------|
| Fluid | black-oil, API 25, GOR 100, gasDensity 0.7, BSW 0 |
| Pipe | 8" ID (0.2032 m), roughness 1.83e-4 m, carbon steel walls (k=50, Cp=500, ρ=7800) |
| Separator | 10 kgf/cm² |
| Reservoir | 150 kgf/cm² static, 90 °C, linear IPR with IP 200 sm³/d/(kgf/cm²) |
| Environment | seawater 4 °C at seabed, `ambientVel` 0.3 m/s; atmosphere 20 °C topside |
| Formation | k=2.5 W/(m·°C), Cp=1000, ρ=2500 kg/m³, productionTime 365 d |
| Discretization | 50–125 m cells, refined near equipment |
| Numerics | all `initialConfig` defaults; `pressureTable: true` for long transients |

## ADR file convention

Write to **`docs/<slug>.adr.md`** where `<slug>` is short-kebab-case (e.g., `docs/glw-shutdown-8h.adr.md`). One ADR per simulation.

## ADR template

```markdown
# ADR: <Title>

## Status
Proposed | Accepted | Implemented | Verified

## Context
<User request, purpose of the study, and any constraints.>

## System Description
<Physical narrative: well/flowline/riser topology with lengths, depths, fluids, equipment, environment.>

## Decisions
### Simulation mode        <PROD/INJ, steady/transient, finalTime, initialCondition>
### Fluids                 <model, properties, PVT file, complementary/gas fluid>
### Materials & cross-sections  <material table, per-section layer stack with IDs>
### Pipeline geometry      <segment table: id, label, crossSectionId, angle/XY, cells×length, environment/formation, coupling>
### Boundary conditions    <closure strategy, inflow, outlet, gasInj>
### Equipment & events     <valves/pumps/gas lift with positions; transient event timeline>
### Time schedule          <times/maxDT table, snapshots, segregation>
### Outputs                <profile flags+times, trend gauges (position, dt, label), cross outputs>
### Advanced settings      <only non-defaults, each with justification>

## Assumed defaults
| Parameter | Value | Reason |

## Cross-reference table
<Every ID link: pipe→crossSection, layer→material, source→fluid, formationId, measured lengths vs line totals.>

## Deliverables
- simulations/<slug>/<slug>.mr3           (and/or build script simulations/<slug>/build_model.py)
- tests/test_<slug>.py                    (pytest, marker `simulacao`)
- docs/<slug>.qa.md                       (QA report, written by QA)

## Acceptance criteria
<Checkable statements the QA agent will verify.>

## Skills referenced
<List of .github/skills/* consulted.>
```

## Rules

- The planner **never** writes simulation JSON/Python — only the ADR.
- Quantify everything: lengths, pressures, times, positions — with units from the json-schema skill.
- The cross-reference table must be complete enough that QA can validate without guessing.
- If the user's request is infeasible (e.g., service-line features without a gas line), resolve it in the interview, not in implementation.
