# Copilot instructions

Marlim3 is a 1D multiphase flow simulator: a C++/Fortran engine (`src/`) that reads a JSON model (`.mr3`) and a bilingual (EN/PT) Python package (`tools/marlim3/`) that builds, runs and reads it. Contributor guide: [CONTRIBUTING.md](../CONTRIBUTING.md).

Pick the skill by the task:

- **Building, running or checking a simulation model** (`.mr3`, Python API, demos used as templates): the [model-handling skill](skills/model-handling/SKILL.md). For a complete "build me a simulation" request, use the `marlim3` agent ([README-agents.md](README-agents.md)); it keeps everything it generates in the git-ignored `agent_sessions/`.
- **Changing or reviewing the engine, the package, the interfaces, the tests or CI**: the [development skill](skills/development/SKILL.md). Per-folder rules in `.github/instructions/` apply automatically to the files being edited.

Do not read the large sources whole (`src/core/Leitura.cpp`, `src/core/Num4Main.cpp`, the `SisProd*` solver modules, `docs/schemas/*.json`): search for the definition or field and read only those lines. `src/core/SisProd.cpp` is the facade; keep solver responsibilities in their modules.
