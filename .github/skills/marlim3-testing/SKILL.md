---
name: marlim3-testing
description: Use when writing or running tests for Marlim3 simulations — repo pytest conventions (simulacao marker, executable skip guard, tmp_path pattern), the test-suite layout, the test file for a session case (agent_sessions/<session>/test_<slug>.py), and the manifest-based regression workflow.
---

# Marlim3 Testing Conventions

## Authoritative files

- [tests/README.md](../../../tests/README.md) — suite overview, demos manifest, and reference workflow (open only for what this skill doesn't cover)
- [tests/test_configure_simulation.py](../../../tests/test_configure_simulation.py) — the template for new simulation tests (fixtures + tmp_path + markers)
- [tests/test_demos_steady_state.py](../../../tests/test_demos_steady_state.py) — runs every case in [demos/manifest.json](../../../demos/manifest.json) (with its auxiliary files) through `marlim3.testing`
- [tests/test_marlim3.py](../../../tests/test_marlim3.py) — unit-test patterns (no executable needed)
- [marlim3/testing.py](../../../marlim3/testing.py) — manifest runner: runs cases and compares them with `demos/<case>/reference/`
- [pyproject.toml](../../../pyproject.toml) — pytest config and marker definition

## Commands

```bash
uv run pytest tests/ -v                                    # full suite
uv run pytest tests/ -m "not simulacao"                    # no executable required
uv run pytest -m simulacao -v                              # simulation tests (need executable)
uv run pytest agent_sessions/<session>/test_<slug>.py -v   # a session's tests
```

Registered marker (defined in [pyproject.toml](../../../pyproject.toml)): `simulacao` (runs the simulator, requires compiled executable). Unmarked tests must run without the executable. Session tests live outside `testpaths` (`tests/`), so the main suite never collects them — run them by path.

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

4. Copy any auxiliary files (PVT `.tab`/`.ctm`, snapshots) from the session folder into the run directory first — the demo runner (`run_case` in [marlim3/testing.py](../../../marlim3/testing.py)) does the same with each manifest entry's `aux` files.
5. Assert on results: `resultados` non-empty, expected keys present, DataFrame non-empty, plus the physical acceptance criteria from the ADR (e.g., pressure monotonicity, value ranges).

## Test template for a session case `<slug>`

Create `agent_sessions/<session>/test_<slug>.py`, next to the case:

```python
"""Integration test for the <slug> simulation (see <slug>.adr.md in this folder)."""
import os

import pandas as pd
import pytest

import marlim3
from marlim3._download import executable_exists

CASE = os.path.join(os.path.dirname(__file__), "<slug>.mr3")

skip_sem_executavel = pytest.mark.skipif(
    not executable_exists(), reason="Executável Marlim3 não encontrado")


def test_input_loads():                              # no executable needed
    caso = marlim3.Branch()
    caso.from_json(CASE)
    assert caso.productionPipe
    # ...ADR-specific invariants (line totals, key positions); generic
    # cross-references are covered by check_case.py (marlim3-json-schema skill)...


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

Steady-state variants of transient cases: set `caso.initialConfig["transient"] = False` to keep runs fast (the demo runner does the same for manifest cases with `steady_state: true`).

## Promoting a session case to the demos (manual, outside the pipeline)

Session cases are not regression cases, and agents never write outside the session folder. Promoting one is a deliberate human decision: create `demos/<case>/` with the Portuguese model, its hand-written English twin, and the auxiliary files; register it in [demos/manifest.json](../../../demos/manifest.json) (`model`, `native`, `languages`, `aux`); generate the references with `uv run python -m marlim3.testing demos/manifest.json <case>/<model>.mr3`; inspect the CSV diff; and commit `demos/<case>/reference/` — see [tests/README.md](../../../tests/README.md).
