---
description: "Use when editing the Marlim3 Python package, interface or tools: engine resolution, bilingual keys, file encoding and the project environment."
applyTo: "tools/marlim3/**, tools/gui/**, tools/regression_tool/**, tools/marlim3_desktop/**"
---
# Marlim3 Python code

- The package writes the model, runs the engine (`Branch.simulate`, `Network.simulate`) and reads the outputs. Keys are bilingual through `tools/marlim3/translations.json` (`docs/dev-guide/translations.md`); the engine reads Portuguese.
- Open JSON, logs and outputs with an explicit `encoding="utf-8"`: on Windows the default code page garbles accents.
- Importing `marlim3` resolves the engine (package copy, local build or release download, `tools/marlim3/_download.py`). Set `MARLIM3_SKIP_EXECUTABLE_RESOLUTION=1` where an import must not build or download.
- Use the project environment: `uv sync --locked --group dev`, then `uv run ...` (Python 3.12+). Lines up to 127 characters (`.flake8`).
- Tests for this code go to `tools/tests/`; mark the ones that run the engine with `simulacao`.
