# Tests

This directory contains the test suite for `Marlim3`. Tests are organized into three categories:

| File | Description |
|------|-------------|
| `test_marlim3.py` | Unit tests — model assembly, JSON serialization, input validation, geometry checks |
| `test_configure_simulation.py` | Integration tests — full simulation runs based on the tutorial notebook |
| `test_demos_steady_state.py` | Integration tests — runs each demo JSON and checks that results are produced |
| `test_regression.py` | Regression tests — runs demos, generates result DataFrames, and compares against stored references |

## Running tests

All tests require the compiled `Marlim3` executable to be available in the `marlim3/` package directory. See the root [README.md](../README.md#compilation) for build instructions.

### Run the full test suite

```bash
uv run pytest tests/ -v
```

### Run only unit tests (no executable required)

```bash
uv run pytest tests/test_marlim3.py -v
```

### Run only regression tests

```bash
uv run pytest tests/test_regression.py -v
```

### Run tests by marker

```bash
# Simulation tests (require executable)
uv run pytest -m simulacao -v

# Regression tests (require executable + reference files)
uv run pytest -m regressao -v
```

## Regression tests

The regression tests in `test_regression.py` work by:

1. Running each demo simulation in steady-state mode
2. Processing results into DataFrames using `Tramo.processar_perfis()` and `Tramo.processar_tendencias()` — the same methods used by the GUI
3. Comparing the resulting DataFrames against stored CSV references in `tests/comparison/<model-name>/`
4. Asserting numerical equality within tolerances (`rtol=1e-5`, `atol=1e-8`)

### Reference files structure

```
tests/comparison/
├── 2zonas-2VGLs-2-Check-correcPerfTerm/
│   ├── perfilProducao.csv
│   ├── perfilServico.csv
│   ├── tendP_7.csv
│   └── tendP_5131.csv
├── BCS-longo-eficMotor/
│   ├── perfilProducao.csv
│   ├── tendP_50.csv
│   ├── tendP_1000.csv
│   └── tendP_2000.csv
├── injec-Liq-TempoResidencia/
│   ├── perfilProducao.csv
│   └── perfilServico.csv
├── parada-longo-Combinado-BCS-GLC-PIG-completo/
│   ├── perfilProducao.csv
│   ├── perfilServico.csv
│   ├── tendP_10.csv
│   ├── tendP_1000.csv
│   ├── tendP_4000.csv
│   ├── tendS_10.csv
│   └── tendS_3000.csv
└── producaoSimplificado/
    ├── perfilProducao.csv
    └── perfilServico.csv
```

### Updating reference files

When a structural change intentionally alters simulation results, the reference files must be updated. Use the provided script:

```bash
# Update all references
python tests/update_regression_references.py --force

# Update specific models only
python tests/update_regression_references.py BCS-longo-eficMotor producaoSimplificado

# Interactive mode (asks before overwriting each model)
python tests/update_regression_references.py
```

After updating, **commit the new CSV files** in `tests/comparison/` so CI uses the updated references.

> **Important:** Always verify that the result changes are expected before updating references. Run a diff on the CSV files to inspect what changed.
