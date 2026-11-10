---
description: "Use when editing Marlim3 tests or demo models: markers, the demo manifest, references and running simulations in tests."
applyTo: "tools/tests/**, demos/**"
---
# Marlim3 tests and demos

- `demos/manifest.json` is the only list of demo cases (tests, `tools/tests/run_models.py` and the regression tool). Each case folder holds the Portuguese model, the native English model, the auxiliary files and `reference/`.
- Mark tests that run the engine with `@pytest.mark.simulacao`; `uv run pytest tools/tests/ -m "not simulacao"` must pass without an engine.
- Regenerate references only for intended result changes: `uv run python -m marlim3.testing demos/manifest.json <model>`, then review the CSV diff.
- Run simulations in `tmp_path` and copy the case's auxiliary files (manifest field `aux`) next to the model.
- Prefer checks against an external reference (analytical solution, published data) for physics and numerics; references only detect change.
