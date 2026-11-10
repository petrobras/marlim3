---
name: model-handling
description: 'Guide to building, running and checking Marlim3 simulation models (.mr3 JSON) with the Python API or the executable. Use when: creating or editing a simulation input; choosing fluids, materials, cross-sections, pipe geometry, boundary conditions, artificial lift (gas lift, ESP, pumps), valves, chokes, PIG, leaks, transient schedule, outputs or solver settings; validating a model against the schemas and ID cross-references; writing tests for a case; planning a simulation with the user (interview and ADR); reviewing a case against its plan. Not for changing the engine or the package code (use the development skill).'
---

# Marlim3 model handling

Marlim3 is a 1D multiphase flow simulator. A model is a JSON file (`.mr3`) with English or Portuguese keys, built and run through the bilingual Python API (`marlim3.Branch` / `marlim3.Tramo`) or the compiled executable. This skill covers building correct models; the [development skill](../development/SKILL.md) covers the code base.

## Read this first

- **Check the environment**: `uv run python .github/skills/model-handling/scripts/check_environment.py` prints `READY` or what is missing (project environment, engine executable).
- **Load [json-schema](references/json-schema.md) first** for any model: structure, units, EN/PT keys and ID cross-references. Then load only the references the model needs (table below).
- **Validate every model**: `uv run python .github/skills/model-handling/scripts/check_case.py <model.mr3>` checks the official schema, ID cross-references and array sizes.
- **Keep context small.** The references link the authoritative documents (`docs/single-branch-model-reference/`, `docs/schemas/`, `demos/`); open them only for what a reference does not settle, and search the schemas and the large demos for the field you need instead of reading them whole.
- **Start from a demo.** `demos/manifest.json` lists the cases; each folder has the Portuguese model and, for most, an English twin.

## References

| Reference | Load when |
|---|---|
| [json-schema](references/json-schema.md) | Always: structure, units, EN/PT keys, cross-references, validation |
| [fluid-configuration](references/fluid-configuration.md) | Black-oil, flash table or compositional fluid, gas and complementary fluids, PVT files |
| [materials-cross-sections](references/materials-cross-sections.md) | Wall materials, radial layers, annular flow, rock formation |
| [pipeline-geometry](references/pipeline-geometry.md) | Segments, angles or XY coordinates, discretization, environment, thermal coupling |
| [boundary-conditions](references/boundary-conditions.md) | Inlet and outlet conditions, IPR, sources, separator, gas injection, injection wells |
| [artificial-lift](references/artificial-lift.md) | Gas-lift valves, ESP, volumetric pumps, annulus unloading |
| [valves-choke](references/valves-choke.md) | Valves, master valves, chokes, PIG, leaks, shutdown and restart |
| [time-transient](references/time-transient.md) | Steady state or transient, initial condition, time steps, snapshots |
| [output-configuration](references/output-configuration.md) | Profiles, trends, radial outputs, `.dat` files and result DataFrames |
| [advanced-settings](references/advanced-settings.md) | Slip and mass-transfer models, threads, tables, convergence, wax, 3D diffusion |
| [python-api](references/python-api.md) | Python code that builds or runs models, results, plots, the command line |
| [testing](references/testing.md) | Tests for a case and the repository pytest conventions |
| [planning-interview](references/planning-interview.md) | Planning a model with the user: questions, defaults, ADR template |
| [qa-checklist](references/qa-checklist.md) | Reviewing a model against its ADR: checklist and report template |

## Scripts

| Script | What it does |
|---|---|
| [`scripts/check_environment.py`](scripts/check_environment.py) | Preflight: project environment and engine executable; prints `READY` or what is missing |
| [`scripts/check_case.py`](scripts/check_case.py) | Validates a model: schema, ID cross-references, array sizes; `--help` lists the options |

## Simulation agents

For a complete "build me a simulation" request, the agents in `.github/agents/` run the whole flow (plan → implement → QA) with these references; see [README-agents.md](../../README-agents.md). Everything they generate goes to the git-ignored `agent_sessions/<YYYY-MM-DD>-<slug>/` folder.
