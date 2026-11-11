# Marlim3 developer skill

A skill and per-folder instructions that give AI coding agents what they need to work on Marlim3 without rereading the large engine sources in every session: a code map, procedures, pitfalls and two scripts. People can use the scripts and the references directly too.

## What is included

| File | When it is read | Purpose |
|---|---|---|
| `SKILL.md` | when a request matches its description, or when called with `/development` | procedures and pitfalls |
| `references/code-map.md` | when the task needs it | files and functions per topic |
| `references/build-and-test.md` | when the task needs it | commands, engine options, environment variables, CI |
| `references/debugging.md` | when the task needs it | crashes, hangs, OpenMP, encoding |
| `references/engine-facts.md` | when the task needs it | units, model constants, known limitations |
| `scripts/symbol_index.py` | run on demand | one line per definition: name, `file:line`, signature |
| `scripts/repro_case.py` | run on demand | runs a model on official releases and local builds and summarizes the logs |
| `.github/instructions/*.instructions.md` | automatically, when the files being edited match their `applyTo` | short rules for the engine, the Python code and the tests |

Only the name and description of the skill stay in the agent's context; the rest is loaded when a task needs it.

## Using it with an agent

### VS Code (GitHub Copilot Chat, agent mode)

1. Open the repository folder. VS Code finds skills in `.github/skills/` and instructions in `.github/instructions/`. If the skill does not show up, check that agent skills are enabled in the settings (search for "skills").
2. Ask as usual. Examples:
   - "Where is the steady-state march of a single branch?"
   - "Reproduce issue #86 on v3.10.2 and on my build."
   - "Add the input key X to the configuration section."
   The agent loads the skill when the request matches its description.
3. To call it explicitly, type `/development` followed by the request.
4. The instructions are applied automatically when you create or edit files under `src/`, `tools/marlim3/`, `tools/gui/`, `tools/regression_tool/`, `tools/marlim3_desktop/`, `tools/tests/` or `demos/`.

### Other agents

Agents that support the Agent Skills format may look for skills in another folder (for example `.claude/skills/` or `.agents/skills/`); check the agent's documentation. For any agent, you can also point it to `.github/skills/development/SKILL.md` at the start of the session.

## Using the scripts directly

Both scripts use only the Python standard library (Python 3.8 or newer). Run them from the repository root; `--help` lists all options.

### symbol_index.py

```bash
# steady-state facade and extracted solver functions
python .github/skills/development/scripts/symbol_index.py --path SisProd --grep Perm

# where a Python method is defined
python .github/skills/development/scripts/symbol_index.py --roots tools/marlim3 --grep simulate

# number of definitions per file
python .github/skills/development/scripts/symbol_index.py --count
```

Each output line is `name<TAB>file:line<TAB>signature`. The default scope is `src/` (C, C++ and Fortran); `--roots` adds other folders, including Python packages.

### repro_case.py

```bash
# a demo made transient, on the official v3.10.2 engine and on the local build
python .github/skills/development/scripts/repro_case.py demos/simplifiedProduction/simplifiedProduction.mr3 \
    --tag v3.10.2 --exe tools/marlim3/Marlim3 \
    --set initialConfig.transient=true --set initialConfig.initialCondition=1

# a required block removed, with an engine option
python .github/skills/development/scripts/repro_case.py demos/simplifiedProduction/simplifiedProduction.mr3 \
    --tag v3.10.2 --delete productionPipe --args -v RULES

# a value inside a list, a short timeout and JSON output
python .github/skills/development/scripts/repro_case.py model.mr3 --exe build/Marlim3 \
    --set 'crossProductionTrend[0].dt=30' --timeout 60 --json
```

- `--tag` downloads the official engine of that release for the current system to `~/.cache/marlim3/engines/<tag>/` and checks it against the published `.sha256`. `--exe` runs a local engine (`tools\marlim3\Marlim3.exe` on Windows). Both can be repeated to compare engines.
- `--set PATH=VALUE` takes a JSON value (`true`, `600`, `[0, 10]`, `{"a": 1}`); anything that is not valid JSON is used as a string. `--delete PATH` removes a key.
- Each run gets its own folder (a temporary one, or `--workdir`) with the model, the other files of the model's folder, `stdout.txt` and the engine's outputs. The summary shows exit code, time, status and the last events of `simulacao.log`.
- `--args` must come last: everything after it goes to the engine.

## Maintaining it

- Update the skill in the same pull request that changes what it describes: moved files, renamed functions, new procedures, fixed pitfalls.
- `tools/tests/test_dev_skill.py` fails when a path or a symbol cited in the skill or in the instructions no longer exists.
- `SKILL.md` is loaded whole: keep it short, put details in `references/` and link to `docs/dev-guide/` instead of copying it.
- Write facts that can be checked in the code, cite functions by name rather than by line number, and say how a fact was observed when it is not obvious from the code.
- The repository is public: nothing about private models, internal paths or internal infrastructure.
