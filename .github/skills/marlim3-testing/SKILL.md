---
name: marlim3-testing
description: Use when writing or running tests for Marlim3 simulations — repo pytest conventions (simulacao/regressao markers, executable skip guard, tmp_path pattern), the test-suite layout, how to add a test for a new simulation case, and the regression-reference workflow.
---

# Marlim3 Testing Conventions

## Authoritative files

- [tests/README.md](../../../tests/README.md) — suite overview and regression workflow (**read this**)
- [tests/test_configure_simulation.py](../../../tests/test_configure_simulation.py) — the template for new simulation tests (fixtures + tmp_path + markers)
- [tests/test_demos_steady_state.py](../../../tests/test_demos_steady_state.py) — pattern for running `.mr3` files with auxiliary files
- [tests/test_marlim3.py](../../../tests/test_marlim3.py) — unit-test patterns (no executable needed)
- [tests/test_regression.py](../../../tests/test_regression.py) + [tests/update_regression_references.py](../../../tests/update_regression_references.py) — regression comparison
- [pyproject.toml](../../../pyproject.toml) — pytest config and marker definitions

## Commands

```bash
uv run pytest tests/ -v                      # full suite
uv run pytest tests/test_marlim3.py -v       # unit tests only (no executable)
uv run pytest -m simulacao -v                # simulation tests (need executable)
uv run pytest -m regressao -v                # regression tests
uv run pytest tests/test_my_case.py -v       # a single new test file
```

Registered markers (defined in [pyproject.toml](../../../pyproject.toml)): `simulacao` (runs the simulator, requires compiled executable) and `regressao` (compares against stored references). Unmarked tests must run without the executable.

## Mandatory pattern for simulation tests

Every test that calls `simulate()` must (copy from [tests/test_configure_simulation.py](../../../tests/test_configure_simulation.py)):

1. Guard on the executable:

```python
import pytest
from marlim3._download import executable_exists

skip_sem_executavel = pytest.mark.skipif(
    not executable_exists(), reason="Executável Marlim3 não encontrado")
```

2. Mark with `@pytest.mark.simulacao` (class or function level) and apply the skip guard.
3. Run inside `tmp_path` with a chdir guard — `simulate()` writes `<label>.mr3` to the CWD and **clears** its output directory:

```python
def _simular(caso, label, tmp_path):
    original_cwd = os.getcwd()
    try:
        os.chdir(str(tmp_path))
        caso.simulate(label=label, directory="resultados")
    finally:
        os.chdir(original_cwd)
```

4. Copy any auxiliary files (PVT `.tab`/`.ctm`, snapshots) into the run directory first — see the `DEMOS` dict pattern in [tests/test_demos_steady_state.py](../../../tests/test_demos_steady_state.py).
5. Assert on results: `resultados` non-empty, expected keys present, DataFrame non-empty, plus the physical acceptance criteria from the ADR (e.g., pressure monotonicity, value ranges).

## Test template for a new simulation case `<slug>`

Create `tests/test_<slug>.py`:

```python
"""Integration test for the <slug> simulation (see docs/<slug>.adr.md)."""
import json, os
import pandas as pd
import pytest
import marlim3
from marlim3._download import executable_exists

CASE = os.path.join(os.path.dirname(__file__), "..", "simulations", "<slug>", "<slug>.mr3")

skip_sem_executavel = pytest.mark.skipif(
    not executable_exists(), reason="Executável Marlim3 não encontrado")


def test_input_loads_and_cross_references():        # no executable needed
    caso = marlim3.Branch()
    caso.from_json(CASE)
    cs_ids = {c["id"] for c in caso.crossSection}
    assert all(p["crossSectionId"] in cs_ids for p in caso.productionPipe)
    # ...one assert per ADR cross-reference rule...


@skip_sem_executavel
@pytest.mark.simulacao
def test_simulation_runs(tmp_path):
    caso = marlim3.Branch()
    caso.from_json(CASE)
    cwd = os.getcwd()
    try:
        os.chdir(str(tmp_path))
        caso.simulate(label="<slug>", directory="resultados")
    finally:
        os.chdir(cwd)
    assert "productionProfile" in caso.resultados
    df = caso.resultados["productionProfile"]
    assert isinstance(df, pd.DataFrame) and not df.empty
    # ...ADR acceptance criteria asserts...
```

Steady-state variants of transient cases: set `caso.initialConfig["transient"] = False` to keep CI fast (the demos test does exactly this).

## Regression references (only for cases promoted to [demos/](../../../demos))

References live in [tests/comparison/](../../../tests/comparison)`/<model-name>/` (`perfilProducao.csv`, `tendP_*.csv`, …). Update via `python tests/update_regression_references.py [--force|model names]`, diff the CSVs to confirm changes are intended, and commit them. Register new demos in the `DEMOS` dict of [tests/test_demos_steady_state.py](../../../tests/test_demos_steady_state.py) with their auxiliary files.
