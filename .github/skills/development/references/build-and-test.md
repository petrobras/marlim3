# Build and test

Setup and the short command list are in "Setting up" and "Tests" of `CONTRIBUTING.md`; the toolchains per system are in "Building from source" of `docs/getting-started.md`. This page collects the details.

## Environment

- Python 3.12+ and uv 0.13.1: `uv sync --locked --group dev` creates `.venv` with the package installed in editable mode.
- After building the engine: `MARLIM3_SKIP_BUILD=1 uv sync --locked`.
- Dependency changes: `docs/dev-guide/dependencies.md`.

## Engine

| Task | Command |
|---|---|
| Release build (Linux, macOS) | `cmake --preset gcc-release` then `cmake --build --preset gcc-release -j` |
| Release build (Windows, MSYS2) | `cmake --preset mingw-release` then `cmake --build --preset mingw-release -j` |
| Debug build in its own folder | `cmake --preset gcc-debug -B build-debug` then `cmake --build build-debug -j` |
| C++ unit tests | `cmake -S . -B build -DMARLIM_BUILD_TESTS=ON`, `cmake --build build`, `ctest --test-dir build --output-on-failure` |

- All presets use `build/` unless `-B` is given. Every build copies its executable into `tools/marlim3/`, so a debug build replaces the release engine used by the tests until the next release build.
- The C++ tests cover inlet initialization, friction, transient temperature and kinetic boundaries, upstream numerical contracts and the TUFFP runtime binding. The vendor-library test is skipped (exit code 77) unless `MARLIM3_TUFFP_LIB` points to the TUFFP library.

## Engine command line

| Option | Meaning |
|---|---|
| `-i FILE` | model to read (default `teste1.json`; `rede.json` with `-s REDE`) |
| `-d FOLDER` | output folder |
| `-o FILE` | log file (default `simulacao.log`) |
| `-p PATH` | folder of other input files (PVTSim) |
| `-s TRANSIENTE`, `-s INJETOR`, `-s REDE` | simulation type; `-s CONVECNAT` (2D natural convection) is not listed in the usage text and reads `parametros.json` |
| `-v JSON`, `-v SCHEMA`, `-v RULES`, `-v OFF` | validation level; currently forced to `OFF` in `main()` |
| `-t N` | OpenMP threads |
| `-h` | usage text |

## Python tests

| Task | Command |
|---|---|
| No engine needed | `uv run pytest tools/tests/ -m "not simulacao"` |
| Full suite | `uv run pytest tools/tests/ -v` |
| Demos | `uv run pytest tools/tests/test_demos_steady_state.py -k <case>` |
| Regression tool | `uv run pytest tools/regression_tool/tests` |
| Lint used in CI | `uv run flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics` |
| Dependency policy (CI) | `python tools/scripts/dependencies.py check` |

Tests that run the engine are marked `simulacao` and need the executable in `tools/marlim3/`.

## Environment variables

| Variable | Effect |
|---|---|
| `MARLIM3_SKIP_EXECUTABLE_RESOLUTION=1` | importing `marlim3` does not look for, build or download the engine |
| `MARLIM3_SKIP_BUILD=1` | no CMake build from `uv sync` or from the import fallback |
| `MARLIM3_FORCE_DOWNLOAD` | any value: download the release engine even if one exists |
| `MARLIM3_CMAKE_PRESET` | preset of the import fallback build (default `gcc-release`) |
| `MARLIM3_COMPILE_FROM_SOURCE=1` | `setup.py` compiles the engine when installing from source |
| `MARLIM3_TUFFP_LIB` | path of the optional TUFFP shared library |
| `MARLIM3_EXEC` | engine used by the interface in `tools/gui/` |
| `OMP_NUM_THREADS` | OpenMP threads |

## CI

`.github/workflows/build-test-release.yml` builds the engine on Linux (manylinux), Windows (MSYS2) and macOS ARM64 and runs `pytest tools/tests/` on each with that build. On `v*` tags it also builds the desktop apps and publishes the GitHub release and the PyPI package. Releases come from `Release-*` labels on pull requests ("Releasing a new version" in `CONTRIBUTING.md`).
