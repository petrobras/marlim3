---
name: development
description: 'Developer guide to the Marlim3 code base: C++/Fortran engine (src/), Python package (tools/marlim3/), GUI, tests, demos and CI. Use when: locating where a feature, input key, log message, output file or solver step is implemented; changing or reviewing engine or API code; building, testing or debugging Marlim3; reproducing a bug report or GitHub issue on an official release engine; regenerating demo references; adding or renaming input keys.'
---

# Marlim3 development

Marlim3 is a 1D multiphase flow simulator. The engine (`src/`, C++ and Fortran, executable `Marlim3`) reads a JSON model and writes `simulacao.log` plus profile and trend files. The Python package (`tools/marlim3/`) builds the model, runs the engine and reads the results.

## Read this first

- **Do not read the large engine files whole.** `src/core/SisProd.cpp` is a small facade; its solvers live in the `SisProdSteadyState`, `SisProdSteadyStateSearch`, `SisProdTransient`, `SisProdThermal`, `SisProdFluidTracking`, `SisProdSources` and other modules. `src/core/Leitura.cpp`, `src/core/Num4Main.cpp` and several solver modules remain large. Find the definition first, then read only its lines:
  - `python .github/skills/development/scripts/symbol_index.py --path SisProd --grep Perm` prints name, `file:line` and signature of each match;
  - `git grep -n "<log text or JSON key>" -- src tools/marlim3` finds messages and keys.
- **Read the guides before the code.** `docs/dev-guide/` covers the entry point, input parsing, solvers, networks, parametric analysis, translations and dependencies. `CONTRIBUTING.md` has setup, repository layout, style, the input-key checklist and the test commands. The [code map](./references/code-map.md) links each topic to its files and functions.
- **Check facts against the code or a run.** Guides and comments can lag behind the code.
- Engine identifiers, comments and many messages are in Portuguese. Keep the surrounding naming when editing; new user-facing text is in English ("Style" in `CONTRIBUTING.md`).

## Procedures

### Build and run the engine

1. Once: `uv sync --locked --group dev`. Then `cmake --preset gcc-release` and `cmake --build --preset gcc-release -j` (Windows with MSYS2: `mingw-release`).
2. Every build copies the executable into `tools/marlim3/`, where the package and the tests look for it.
3. Run a model the way `Branch.simulate` does: `Marlim3 -d <run folder> -i <model file in that folder>`. Add `-s INJETOR` for an injection well, `-s REDE` for a network and `-t <threads>` for OpenMP. Results and `simulacao.log` are written to the run folder.

Commands, options and environment variables: [build and test](./references/build-and-test.md).

### Run the tests

- `uv run pytest tools/tests/ -m "not simulacao"` needs no engine; `uv run pytest tools/tests/ -v` runs everything with the engine in `tools/marlim3/`.
- One demo: `uv run pytest tools/tests/test_demos_steady_state.py -k <case>`.
- C++ unit tests: `-DMARLIM_BUILD_TESTS=ON` and `ctest` ("Tests" in `CONTRIBUTING.md`).

### Reproduce a bug report or an issue

1. Note the version and the exact input of the report. Start from the closest demo in `demos/`.
2. Run it on the reported release and on the current build, changing only what the report changes:
   `python .github/skills/development/scripts/repro_case.py demos/simplifiedProduction/simplifiedProduction.mr3 --tag v3.10.2 --exe tools/marlim3/Marlim3 --set initialConfig.transient=true --args -v RULES`
3. Compare exit code, `simulacao.log` status and the last events. The run folders are kept for inspection.
4. Report the exact command and results. If it no longer reproduces, find the change that fixed it (`git log -S`, release notes) before closing.

### Change simulation results on purpose

1. Give the physical or numerical reason in the pull request.
2. Regenerate the references: `uv run python -m marlim3.testing demos/manifest.json [<model>]`.
3. Review the CSV diff in `demos/<case>/reference/` before committing.
4. Prefer an external check (analytical solution, published data): references detect change, not correctness.

### Add or rename an input key

Follow "Input keys" in `CONTRIBUTING.md` (six places that must agree) and `docs/dev-guide/translations.md`. Then:

- regenerate the schema pages: `uv run python schema_to_markdown.py --batch`;
- run `uv run pytest tools/tests/test_translations.py tools/tests/test_schema_consistency.py`.

CMake regenerates `src/include/translations_data.h` from `tools/marlim3/translations.json` when it configures; commit both files.

### Debug a crash or a hang

See [debugging](./references/debugging.md): separate debug build, gdb, AddressSanitizer, OpenMP and encoding problems.

## Pitfalls

- The parser is case-sensitive and ignores unknown keys silently: a typo runs with the default value.
- `-v JSON|SCHEMA|RULES|OFF` is parsed, but `main()` in `Num4Main.cpp` then forces validation off, so wrong types and out-of-enum values are not rejected (issue #86).
- `-s CONVECNAT` (2D natural convection) reads `parametros.json` from the run folder; passing `-i` with it crashes (issue #88).
- A crash caught by `signalHandler` is logged with the signal in `causa` (for example `SIGSEGV - Acesso invalido a memoria`). A crash before the logger starts leaves no `simulacao.log`.
- With black-box correlations (`tipoModeloDrift = false`) the run is steady state only: `Ler::parse_configuracao_inicial` turns the transient off with an AVISO.
- Several friction-factor implementations coexist, with different transition thresholds and conventions (issue #100). Check which one a code path uses before comparing results.
- `Logger::log` is serialized for OpenMP (`omp critical(marlim_logger)` in `Log.cpp`); other shared state inside parallel regions is not.
- In Python, open JSON, logs and outputs with `encoding="utf-8"`: the Windows default code page garbles accents.
- When running the engine outside the API, copy the model's auxiliary files (PVT tables, `leituraAP.json`, meshes; the `aux` field of `demos/manifest.json`) into the run folder.
- Branches created before the release-label workflow (#144) fail the "Release version" check because `.github/scripts/release_version.py` is missing: merge `main` into the branch.

More observed behaviour (units, model constants, known limitations): [engine facts](./references/engine-facts.md).
