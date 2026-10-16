---
name: marlim3-python-api
description: Use when writing Python code that builds or runs Marlim3 simulations — the bilingual Branch/Tramo class, to_json/from_json, simulate() behavior and its side effects, result DataFrames, plotting helpers, Scenarios comparison, the command-line executable, and the uv-based dev environment.
---

# Marlim3 Python API & Execution

## Authoritative files

- [README.md](../../../README.md) — canonical scripting example and CLI usage (**read this**)
- [marlim3/_tramo/_branch.py](../../../marlim3/_tramo/_branch.py) — `Branch` implementation (`simulate`, `to_json`, `from_json`, plotting)
- [marlim3/_tramo/_keys.py](../../../marlim3/_tramo/_keys.py) — bilingual key system
- [docs/tutorials/](../../../docs/tutorials/index.md) — 5 notebook walkthroughs (steady, nodal, gas-lift, correlations, transient)
- [marlim3/_cenarios/_cenarios.py](../../../marlim3/_cenarios/_cenarios.py) — `Scenarios` multi-case comparison
- [marlim3/_rede/_rede.py](../../../marlim3/_rede/_rede.py) — `Network` (flow networks, `-s REDE`)

## Canonical workflow

```python
import marlim3

case = marlim3.Branch()                      # marlim3.Tramo is the PT alias (same class)
case.productionFluid = [{"id": 0, "api": 32, "gor": 100, "gasDensity": 0.7, "bsw": 0.0}]
case.material = [{"id": 0, "type": 0, "conductivity": 58, "specificHeat": 480, "rho": 7850}]
case.crossSection = [{"id": 0, "innerDiameter": 0.254, "roughness": 1.83e-4,
                      "layers": [{"materialId": 0, "layerMeasurementType": "THICKNESS", "thickness": 0.0254}]}]
case.productionPipe = [{"id": 0, "crossSectionId": 0, "environment": 2, "angle": 0,
                        "discretization": [{"numCells": 20, "length": 125.0}],
                        "initialConditions": {"measuredPosition": [0, 1],
                                              "ambientTemp": [40, 20], "ambientVel": [0.5, 0.5]}}]
case.liquidSource = [{"id": 0, "prodFluidId": 0, "measuredLength": 0.1,
                      "time": [0], "liquidFlowRate": [1500], "temperature": [40]}]
case.separator = {"time": [0], "pressure": [2]}
case.productionProfile = {"time": [0], "pressure": True, "temperature": True, "holdup": True}

case.simulate(label="my_case", directory="results")
df = case.resultados["productionProfile"]    # pandas DataFrame
case.plot_profiles()
```

## Bilingual behavior (see [docs/dev-guide/translations.md](../../../docs/dev-guide/translations.md))

- Attributes, nested dict keys, and enum values accept **both** EN and PT everywhere (`case.dutosProducao` ≡ `case.productionPipe`; `fluid["rgo"]` ≡ `fluid["gor"]`). Internal storage is always English.
- `case.to_json("name")` writes `name.mr3` (EN keys + `"language": "en"`); `to_json("name", language='pt')` exports PT.
- `case.from_json(path)` loads either language and normalizes to EN.

## `simulate()` — important side effects

`simulate(kind='PRODUTOR', label=..., directory=..., generate_ppl_tpl=False, ...)`:

1. **Deletes all existing contents of `directory`** before running — never point it at a directory with valuable files.
2. Writes `<label>.mr3` into the **current working directory** (tests `chdir` into a tmp dir first — copy that pattern).
3. Runs the bundled executable (`-s INJETOR` added automatically when `system` is `"INJ"`/`"INJETOR"`).
4. Loads results into `case.resultados`: keys `productionProfile`, `productionTrend`, and `serviceProfile`/`serviceTrend` when `gasLine` is true.
5. On failure, raises with the `FALHA` entry parsed from `simulacao.log` — always read that log when debugging.
6. Auxiliary files (`.tab`/`.ctm` PVT, snapshots) must be in the CWD at run time.

Executable availability: `from marlim3._download import executable_exists, get_executable_path`. On import, the package tries to resolve/download the executable (skippable via `MARLIM3_SKIP_EXECUTABLE_RESOLUTION=1`).

## Post-processing

- Profiles DataFrame: MultiIndex (time, cell); columns like `Pressure (kgf/cm2) C`, `Length (m) Cell center C` (`C` center, `F` face). Canonical names: [marlim3/_output_headers.py](../../../marlim3/_output_headers.py).
- Trends: dict {gauge → DataFrame indexed by time, `.attrs['measured_length'/'label'/'cell_index']`}.
- Plots: `plot_profiles(line=..., gradient=..., indicate_anm=...)`, `plot_trends(positions=...)`, `plot_geometry()`, `plot_animated_profiles()`.
- Multi-case: `marlim3.Scenarios(casos={label: branch, ...})` → `plot_profiles()` / `plot_trends()`.
- OLGA-style export: `case.simulate(generate_ppl_tpl=True)` or `marlim3.convert_to_ppl_tpl(...)`.

## Environment & CLI

```bash
uv sync --locked --group dev            # dev env (Python ≥3.12, pytest, flake8…)
uv run python my_script.py              # run anything inside the env
# Direct CLI (executable from build/ or marlim3/):
./Marlim3 -d ./output -i case.mr3                # production
./Marlim3 -d ./output -i case.mr3 -s INJETOR     # injection
./Marlim3 -d ./output -i case.mr3 -s REDE        # network
./Marlim3 -d ./output -i case.mr3 -s CONVECNAT   # natural convection
```

Build from source if needed (see [README.md](../../../README.md#compilation)): `cmake --preset gcc-release && cmake --build --preset gcc-release -j$(nproc)` — the executable is auto-copied into `marlim3/`.
