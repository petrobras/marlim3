# Tests

This directory contains the test suite for `Marlim3`.

| File | Description |
|------|-------------|
| `test_marlim3.py` | Unit tests — model assembly, JSON serialization, input validation, geometry checks |
| `test_configure_simulation.py` | Integration tests — full simulation runs based on the tutorial notebook |
| `test_demos_steady_state.py` | Demo tests — API key round-trip, PT × EN × native runs and regression against stored references |

## Running tests

Tests marked `simulacao` require the compiled `Marlim3` executable in the `marlim3/` package directory. See the root [README.md](../README.md#compilation) for build instructions.

```bash
uv run pytest tests/ -v                    # full suite
uv run pytest tests/ -m "not simulacao"    # no executable required
uv run pytest tests/test_demos_steady_state.py -v
```

## Demos: `demos/manifest.json`

The manifest is the only list of models used by the tests, `run_models.py --group regression`
and the regression tool. Each case has its own folder in `demos/` holding the Portuguese
model, the hand-written English model and the auxiliary files:

```json
{"model": "simplifiedProduction/producaoSimplificado.mr3",
 "native": "simplifiedProduction/simplifiedProduction.mr3",
 "languages": ["pt", "en"],
 "aux": ["PVTSIM-MARLIM.tab"]}
```

| Field | Meaning |
|-------|---------|
| `model` | Portuguese model, relative to `demos/` |
| `native` | the same model written by hand in English (optional) |
| `languages` | languages the model is written in before running (`pt`, `en`) |
| `aux` | auxiliary files, looked up in the model folder and then in `demos/` |
| `mode` | `PRODUCAO` (default), `INJETOR` or `REDE` |
| `steady_state` | run with `transient` disabled (default `true`) |

For each case, `test_demos_steady_state.py` checks that:

1. reading and rewriting the model through the API loses, adds or changes no input key, and the
   `native` model has the same keys as the Portuguese one (no executable needed);
2. the model runs in every listed language and, when present, the `native` model runs too;
3. the production profiles of all those runs agree;
4. the profiles and trends of the first listed language match the reference in
   `demos/<case>/reference/` (`rtol=1e-5`, `atol=1e-8`; network cases have no reference).

The same runner (`marlim3.testing`) is used by private suites with their own manifest and
references.

### Updating references

When a change intentionally alters simulation results, regenerate the references and commit the
CSV files in `demos/<case>/reference/`:

```bash
uv run python -m marlim3.testing demos/manifest.json                     # all cases
uv run python -m marlim3.testing demos/manifest.json simplifiedProduction/producaoSimplificado.mr3
```

> **Important:** inspect the CSV diff and make sure the changes are expected before committing.
