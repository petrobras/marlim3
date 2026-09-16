# Marlim3 — Python package

Python API for building models, running simulations, and inspecting results.

For installation, see [Getting Started](../docs/getting-started.md).

---

## Quick start

```python
import marlim3

branch = marlim3.Branch()
branch.from_json("demos/pt-br/producaoSimplificado.mr3")
branch.simulate()
branch.plot_profiles()
```

`from_json` accepts both Portuguese and English files — the language is detected
from the key names, or from an explicit `"language": "en"` entry.

## Editing an existing model

The usual workflow is to start from a demo close to your case and change what
matters. Model sections are plain Python lists and dicts:

```python
import marlim3

branch = marlim3.Branch()
branch.from_json("demos/pt-br/producaoSimplificado.mr3")

# raise the water cut and re-run
branch.productionFluid[0]["bsw"] = 0.30
branch.simulate()

# inspect results as pandas DataFrames
profile = branch.resultados["productionProfile"]
print(profile.columns.tolist())
```

For a parametric study:

```python
import marlim3

results = {}
for bsw in (0.0, 0.15, 0.30, 0.45):
    branch = marlim3.Branch()
    branch.from_json("demos/pt-br/producaoSimplificado.mr3")
    branch.productionFluid[0]["bsw"] = bsw
    branch.simulate(label=f"bsw_{bsw:.2f}")
    results[bsw] = branch.resultados["productionProfile"]
```

## Building a model from scratch

A complete model needs several interdependent sections, and the required set
depends on the system being modelled — a gas-lifted well needs a service line,
a gas injection source and gas-lift valves, none of which a simple pipeline
requires. Business-rule validation runs before the simulation and reports what
is missing:

```
Encontrada falha durante a validacao das regras de negocio do MRT:
'discretizacao do segmento de duto', valores <= 0 (Propriedade: #/dutosProducao/0)
```

Rather than reproducing a full model here, see:

- [`demos/`](../demos) — working models covering production wells, injection,
  gas lift, ESP, networks and shutdown transients;
- [Model reference](../docs/single-branch-model-reference/index.md) — every
  section, key and unit, with which are required;
- [Tutorials](../docs/tutorials) — step-by-step notebooks that build models
  progressively.

The sections common to most single-branch models are:

| Section | Purpose |
|---|---|
| `system` | simulation type (`PROD`, `INJETOR`, …) |
| `initialConfig` | global switches, rock formation, solver options |
| `productionFluid` | black-oil or compositional fluid definition |
| `gasFluid` | injected/lift gas properties |
| `material`, `crossSection` | pipe wall layers and geometry |
| `productionPipe` | discretized flow path |
| `ipr` / `liquidSource` | inlet boundary condition |
| `separator` | outlet boundary condition |
| `productionProfile` | which variables to output |

## Bilingual interface

The Python API is **fully bilingual** — Portuguese and English key names are
interchangeable, and both refer to the same underlying data:

```python
import marlim3

# Build a model entirely in Portuguese
tramo = marlim3.Tramo()
tramo.sistema = "PROD"
tramo.fluidosProducao = [{"id": 0, "api": 30, "rgo": 100, "densidadeGas": 0.7, "bsw": 0.0}]
tramo.secaoTransversal = [{
    "id": 0,
    "diametroInterno": 0.254,
    "rugosidade": 1.83e-4,
    "camadas": [{"idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0254}],
}]

# Nested access works in both languages
tramo.fluidosProducao[0]["densidadeGas"]  # → 0.7
tramo.productionFluid[0]["gasDensity"]    # → 0.7 (same data)
# Export in Portuguese
tramo.to_json("modelo", language="pt")
```

`Branch` and `Tramo` are the same class under two names. Key translation is
driven by [`translations.json`](translations.json), which is also the source of
truth for the C++ engine's translator — the two cannot drift apart.

See [Bilingual Support](../docs/dev-guide/translations.md) for details.

## Input files

Model files use the `.mr3` extension and contain JSON. Keys must match the
published schema exactly — the parser is case-sensitive and does not warn about
unrecognised names.

- [JSON schema (PT)](../docs/schemas/branch.pt.json)
- [JSON schema (EN)](../docs/schemas/branch.en.json)
- [Model reference](../docs/single-branch-model-reference/index.md)
- [Input migration guide](../docs/single-branch-model-reference/migration.md) —
  key names that changed and are no longer accepted

Worked examples live in [`demos/`](../demos), and step-by-step tutorials in
[`docs/tutorials/`](../docs/tutorials).

## Executable resolution

The package needs the native engine to run simulations. On import it looks for
the executable in this order:

1. `marlim3/Marlim3` (or `Marlim3.exe`) inside the package directory — where the
   CMake `POST_BUILD` step copies it;
2. `build/` under the repository root, for local development builds;
3. the matching GitHub Release asset, downloaded on first use.

Two environment variables control this behaviour:

| Variable | Effect |
|---|---|
| `MARLIM3_SKIP_BUILD=1` | skip local CMake compilation during `uv sync` |
| `MARLIM3_SKIP_EXECUTABLE_RESOLUTION=1` | do not resolve or download the executable on import |

## Submodules

| Path | Purpose |
|---|---|
| `_tramo/` | the `Branch`/`Tramo` model class, serialization, and simulation driver |
| `_plots/` | profile, trend, geometry, and network plotting |
| `_conversores/` | converters from other formats — see [FlowEdit converter](_conversores/_conversor_flowedit/README.md) |
| `_download.py` | release asset resolution and download |
| `translations.json` | PT ↔ EN key map, shared with the C++ engine |
