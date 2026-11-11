# Code map

Where to start for each part of Marlim3. Look the symbols up with `scripts/symbol_index.py` (or `git grep`) and read only their lines. `tools/tests/test_dev_skill.py` checks that every file and symbol in the tables below still exists.

## Repository

| Path | Contents |
|---|---|
| `src/core/` | engine sources (C++) |
| `src/include/` | headers; `src/include/translations_data.h` is generated from `tools/marlim3/translations.json` |
| `src/fortran/` | compositional thermodynamics and flash (Fortran) |
| `src/thirdparty/` | vendored libraries (RapidJSON); do not edit |
| `tools/marlim3/` | Python package |
| `tools/gui/` | Streamlit interface (`tools/gui/app.py`) |
| `tools/marlim3_desktop/` | standalone desktop build of the interface |
| `tools/regression_tool/` | local regression and coverage tool (`uv run tools/regression_tool/run.py`) |
| `tools/tests/` | pytest suite; C++ unit tests in `tools/tests/cpp/` |
| `demos/` | demo models, one folder per case, listed in `demos/manifest.json` |
| `docs/` | MkDocs site: developer guides in `docs/dev-guide/`, input schemas in `docs/schemas/` |
| `schema_to_markdown.py` | generates the schema reference pages |
| `tools/scripts/dependencies.py` | dependency lock policy and mirror helper |

## Engine: entry point and modes

Guide: `docs/dev-guide/num4main.md`.

| File | Symbols | Role |
|---|---|---|
| `src/core/Num4Main.cpp` | `main`, `showUsage`, `signalHandler` | command line, mode dispatch, crash and cancellation handling |
| `src/core/Num4Main.cpp` | `SolveTramoSolteiro`, `permanenteSimples` | single branch: steady state, then the transient |
| `src/core/Num4Main.cpp` | `RedeProd`, `RedeInj`, `RedeParalela`, `RedeAnelGL`, `preparaRedeProd`, `solveRedeProd`, `SolveRedeTrans`, `convergeRede` | networks (`-s REDE`) |
| `src/core/Num4Main.cpp` | `leituraAP`, `leituraAPparalelo` | parametric analysis, serial and OpenMP |
| `src/core/Num4Main.cpp` | `WriteSnapShot`, `ReadSnapShot` | snapshots used as initial condition |

## Engine: input

Guide: `docs/dev-guide/input-parsing.md`. Checklist for keys: "Input keys" in `CONTRIBUTING.md`.

| File | Symbols | Role |
|---|---|---|
| `src/core/JSONDataModel.cpp` | | generic JSON tree (layer 1) |
| `src/include/JSON_entrada.h` | | typed classes per input section (layer 2) |
| `src/core/JSON_entrada.cpp` | | key names the parser registers |
| `src/core/Leitura.cpp` | `Ler::lerArq`, `Ler::parse_configuracao_inicial`, `Ler::parse_tempo`, `Ler::parse_fluidos_producao`, `Ler::parse_fluido_gas`, `Ler::geracelp` | `Ler` (layer 3): reads the model into data structures and builds the cells |
| `src/core/validaTipoJson.cpp` | | type checks and the list of discontinued keys |
| `src/core/LerRede.cpp` | | network input |
| `src/core/LerAP.cpp` | | parametric-analysis input (`leituraAP.json`) |

## Engine: solver

Guides: `docs/dev-guide/steady-state-branch.md`, `docs/dev-guide/transient-solver.md`, `docs/dev-guide/network-simulation.md`, `docs/dev-guide/parametric-analysis.md`.

| File | Symbols | Role |
|---|---|---|
| `src/core/SisProd.cpp` | `SProd::SProd`, `SProd::SolveTrans`, `SProd::buscaProdPfundoPerm` | small production-system facade; delegates through explicit module state and dependencies |
| `src/core/SisProdSetup.cpp` | `SProd::montasistema`, `SProd::buildProductionCells` | builds the production system from `Ler`, allocates buffers and configures initial state |
| `src/core/SisProdSteadyStateSearch.cpp` | `searchProductionBottomHolePressure`, `searchProductionBottomHolePressureSecondary`, `solveSteadyRoot` | steady-state root searches around the cell-by-cell march |
| `src/core/SisProdSteadyState.cpp` | `marchProductionSteady`, `marchProductionSteadySecondary`, `advanceSteadyMass`, `advanceCompositionalSteadyMass`, `advanceUpstreamSteadyPressure`, `advanceDownstreamSteadyPressure`, `finalizeSteadyMassTwoPhase` | steady-state march and cell updates; includes black-box flow-pattern handling |
| `src/core/SisProdSources.cpp` | `renewSourceTerms`, `prepareRecirculationSourceRates` | accessory source rates and recirculation connection fluids |
| `src/core/SisProdFluidTracking.cpp` | `transportBlackOilProperties`, `transportPhaseMolarFractions` | black-oil and compositional fluid transport, including recirculation |
| `src/core/SisProdThermal.cpp` | `updateFlowPartitionTerms`, `updateTemperatureFromEnthalpy` | thermal and mass-transfer calculations |
| `src/core/SisProdGasLift.cpp` | `solveGasLine`, `updateTransientGasValves` | gas-lift line and valve coupling |
| `src/core/SisProdTransient.cpp` | `solveTransientStep`, `solvePressureVolumeCoupling` | transient time step and pressure-volume coupling |
| `src/core/SisProdTrendOutput.cpp` | `writeProductionTrendHeader`, `writeProductionTrendRows`, `writeProductionCrossSectionTrendRows` | production trends and cross-section trends (`TENDTRANSP-*`); gas-line counterparts in the same module |
| `src/include/SimulationContext.h` | | simulation-owned numerical state, reused workspaces and diagnostics; no process-wide solver globals |
| `src/core/SisProdVap.cpp` | | steam systems |

## Engine: cells, closures and properties

Guides: `docs/dev-guide/core-domain-classes.md`, `docs/dev-guide/fluid-thermodynamics.md`, `docs/dev-guide/heat-transfer.md`, `docs/dev-guide/embedded-solvers.md`.

| File | Symbols | Role |
|---|---|---|
| `src/core/celula3.cpp` | `Cel::fric` | cell (`Cel`) and its friction factor |
| `src/core/GradientCorrelations.cpp` | `executarCorrelacao` | black-box (MR2) gradient correlations |
| `src/include/FrictionFactor.h` | `frictionFactor` | standalone inline Moody-Darcy friction factor shared by the engine and its unit test |
| `src/core/DriftFluxClosure.cpp` | | drift-flux closure and exact gas-root reuse |
| `src/core/GomezModel.cpp` | | Gomez mechanistic model |
| `src/core/SlugFlow_TaitelBarnea.cpp` | | unit-cell report (Taitel-Barnea) |
| `src/core/TrocaCalor.cpp` | `TransCal::fric` | heat transfer (`TransCal`) |
| `src/core/PropFlu.cpp` | `ProFlu::emul`, `ProFlu::VisAgua`, `ProFlu::FalsaCorda` | black-oil properties (`ProFlu`): emulsion, water viscosity, false-position solver |
| `src/core/tuffp_runtime_binding.cpp` | | optional external TUFFP library (`MARLIM3_TUFFP_LIB`) |
| `src/fortran/` | | compositional model and flash |

## Engine: logging

| File | Symbols | Role |
|---|---|---|
| `src/include/Log.h` | | codes: 1000 unexpected failure, 1001 JSON syntax, 1002 schema, 1003 business rule, 1004 warning, 1100/1101 parse information, 1200 end of run, 1300 cancellation or signal; severities `LOGGER_INFO`, `LOGGER_AVISO`, `LOGGER_FALHA` |
| `src/core/Log.cpp` | `Logger::log`, `Logger::log_write_logs_and_exit` | writes `simulacao.log`; `Logger::log` runs inside `omp critical(marlim_logger)` |

## Python package

Guides: `tools/marlim3/README.md`, `docs/dev-guide/translations.md`.

| File | Symbols | Role |
|---|---|---|
| `tools/marlim3/__init__.py` | | public API: `Branch` (alias `Tramo`), `Network`, `Scenarios`, `convert_to_ppl_tpl`; resolves the engine on import |
| `tools/marlim3/_tramo/_branch.py` | `Branch.simulate` | writes the model, runs `Marlim3 -d <folder> -i <file>` (`-s INJETOR` for injectors) and reads the results |
| `tools/marlim3/_rede/_rede.py` | `Network.simulate` | network runs |
| `tools/marlim3/_download.py` | `ensure_executable`, `get_executable_path`, `download_executable` | finds, builds or downloads the engine |
| `tools/marlim3/testing.py` | | manifest runner: `python -m marlim3.testing demos/manifest.json` regenerates references |
| `tools/marlim3/translations.json` | | PT and EN keys and values, also compiled into the engine |
| `tools/marlim3/_conversores/` | | converters: MR2, FLOWedit and TPL/PPL output |

## Tests, demos and CI

| File | Symbols | Role |
|---|---|---|
| `tools/tests/README.md` | | how the suite works, markers, reference updates |
| `tools/tests/test_demos_steady_state.py` | | demos: key round trip, PT x EN x native runs, references |
| `tools/tests/test_translations.py` | | `tools/marlim3/translations.json` integrity and the committed `translations_data.h` |
| `tools/tests/test_schema_consistency.py` | | engine fields vs schemas vs translations (warnings) |
| `tools/tests/cpp/` | | C++ unit tests (`-DMARLIM_BUILD_TESTS=ON`) |
| `demos/manifest.json` | | the only list of demo cases |
| `.github/workflows/build-test-release.yml` | | builds and tests on Linux, Windows and macOS; desktop apps and release on `v*` tags |
| `.github/workflows/release-version.yml` | | `_version.py` against the `Release-*` label of a pull request |
| `.github/workflows/tag-release.yml` | | tags a merged version and starts the release |
