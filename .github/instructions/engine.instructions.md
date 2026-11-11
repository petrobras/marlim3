---
description: "Use when editing the Marlim3 C++/Fortran engine: navigation of the large sources, logging, input keys, generated files and result changes."
applyTo: "src/**, CMakeLists.txt, CMakePresets.json"
---
# Marlim3 engine

- Find a definition before reading a large source: `python .github/skills/development/scripts/symbol_index.py --path <file> --grep <name>` or `git grep -n`, then read only that range. `SisProd.cpp` is a small facade; adapt solver changes in the corresponding `SisProd*` module, preserving explicit state and dependencies. The `development` skill has the code map and procedures.
- Keep the surrounding naming (often Portuguese) when editing; new user-facing text is in English.
- Report problems through `Logger::log` with a code from `src/include/Log.h` and the matching severity. `Logger::log` is serialized for OpenMP; other shared state inside parallel regions is not.
- An input key lives in six places that must agree (the "Input keys" checklist in `CONTRIBUTING.md`); renamed keys go to the discontinued list in `src/core/validaTipoJson.cpp` and to `docs/single-branch-model-reference/migration.md`.
- Do not edit `src/include/translations_data.h` (generated from `tools/marlim3/translations.json` by CMake) or `src/thirdparty/`.
- A change in results needs the reason in the pull request and regenerated references (`uv run python -m marlim3.testing demos/manifest.json`); an engine bug fix needs a test that fails before the fix.
