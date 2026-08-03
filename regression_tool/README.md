# Regression Tool

Desktop tool for registering, running and comparing regression tests of the
native `Marlim3` engine. It ships inside the `Marlim3` repository as
`regression_tool/` and shares the repository `uv` environment through the
`regression_tool` dependency group.

Coverage tells you what was executed. Regression tells you whether the observed
result is still correct. A process can exit with code zero and still be reported
as `crashed` when it fails a rule or does not produce the required coverage
profile.

## Core capabilities

- **Toolchain detection**: CMake, C++11, Fortran, Ninja, Clang/LLVM, GCC/gcov, executable and demos
- **Instrumented build** in an isolated shadow copy of the sources
- **Local test registry** in SQLite with search, groups, tags and multiple selection
- **Isolated execution** in separate processes with stdout, stderr, exit code, duration and cancellation
- **Extensible regression rules** for output, regex, files, hashes, text, binary, CSV, JSON, numbers and time
- **LLVM coverage** per test plus real profile merging with `llvm-profdata`
- **Execution flow graph** with every function found by LLVM, demangled symbols, `main` as root, recursion filter, search, scopes, zoom, pan and fit
- **Visual comparison** between exactly two executed tests, with common and exclusive functions
- **Live discovery** of `.mr3` and `.json` models in `../demos/`, excluding `demos/pt-br/`
- **Bilingual interface**: English by default, Portuguese via the toggle in the top bar
- **CLI** for environments without a graphical interface

## Requirements

- [uv](https://docs.astral.sh/uv/), used from the `Marlim3` repository root
- A modern browser; Chrome, Chromium or Edge enables the dedicated application window
- CMake 3.16 or newer and Ninja
- Clang/Clang++ with `llvm-profdata` and `llvm-cov` from the same version family
- GFortran and a compatible OpenMP implementation for the full `Marlim3` build

The compiled interface is part of the repository and does not depend on Node.js
or external services at runtime. Node.js 20+ and npm are required only to change
the frontend.

## Installation

From the `Marlim3` repository root:

```bash
uv sync --group regression_tool
uv run regression_tool/run.py
```

`uv sync --group regression_tool` reuses the repository `.venv` and `uv.lock`.
The tool does not create its own virtual environment, lock file or
`.python-version`.

To check the toolchain, or to try installing CMake, Ninja, Clang/LLVM, GFortran
and OpenMP with the native package manager:

```bash
uv run regression_tool/install.py
uv run regression_tool/install.py --native-deps
```

apt, dnf, pacman, zypper, Homebrew and winget are supported. Native installation
is optional and explicit because it may request administrative privileges.

## Local data

No local data is written inside `regression_tool/` or anywhere else in the
repository. Configuration, database, builds, runs, uploads, caches and the
browser profile live in a user-private directory, with a different folder for
each checkout:

| Platform | Location |
|----------|----------|
| Linux | `$XDG_STATE_HOME/marlim3-regression-tool/<project-hash>/` or `~/.local/state/marlim3-regression-tool/<project-hash>/` |
| macOS | `~/Library/Application Support/Marlim3 Regression Tool/<project-hash>/` |
| Windows | `%LOCALAPPDATA%\Marlim3\RegressionTool\<project-hash>\` |

Cloning or sharing the repository therefore always opens the default
experience. One person's state never travels with the tool files.

Set `MARLIM_PROJECT_ROOT` when the tool runs outside the repository, and
`MARLIM_REGRESSION_DATA_DIR` to override the local data directory.

## Usage

### First run

1. Click **Validate** and wait for the green **Validated** state.
2. Click **Build** and wait for the green **Built** state. The **Ready**
   indicator appears once both are confirmed.
3. Select one or more tests and use **Run selection**.

The green **Validated** and **Built** buttons can be clicked again to clear the
confirmation. While the project is not **Ready**, execution is blocked both in
the interface and in the API.

The required inputs are read from `Marlim3` and copied into the user cache
before configuration, because the `Marlim3` CMake generates files inside its own
source tree.

### Registering models

Choose **Add**, then drag one or several `.mr3` or `.json` files onto the file
area, or click it to open the system picker. Models can be registered
individually or in batch, and **each test is named after its own model file** —
there is no name field.

Every run gets its own directory in `<local-data>/runs/<uuid>/`. The model and
its auxiliary files are copied only into that transient environment; the
originals are never modified.

Additional validations use the rule list of the test:

```json
[
  {"kind": "stdout_contains", "expected": "finished"},
  {"kind": "file_exists", "path": "output/simulacao.log"},
  {"kind": "max_duration", "expected": 120.0}
]
```

### Following a run

During **Run selection**, the **Logs** tab shows the full queue, the current
model and the result of each item with status, exit code, duration, coverage and
remaining count. The **Terminal** tab receives stdout, stderr, prints and logs
from the running program in real time. There is no per-model time limit: every
attempt ends as `passed` or `crashed`, and the queue continues with the next
model.

In the list on the left, the current test gets a blue indicator. Each finished
test immediately gets a green marker when it passes or a red one when it
crashes. In the **Execution** tab, click any processed model to open its most
recent `simulacao.log` in the **Logs** tab, when the file was generated.

### Interpreting and comparing coverage

The flow, coverage and code panels use only the most recent reports of the
selected tests. A single selected test shows its individual coverage. Several
selected tests use a real merge with `llvm-profdata merge` and `llvm-cov
export`; individual percentages are never summed. A selection containing a test
that has not run yet blocks the analysis until it does. Incremental contribution
measures new lines and functions relative to the previous tests.

**Compare** is enabled only for exactly two executed tests. The comparison
separates common functions, functions exclusive to the first test and to the
second, and shows both coverages side by side.

The backend sends every function present in the LLVM report to the interface,
including uncovered ones. The **partial** state means the function was called
but at least one code region was not executed. **All** is the default scope;
**Main flow** shows covered or partial functions; **Pending** shows partial or
uncovered functions. The recursion filter considers calls from `main`: level 1
is the direct calls, level 2 adds the calls made by level 1, and so on.
**Normal** applies no depth cut. Edges come from approximate static analysis,
not from a temporal trace.

### Language

The toggle in the top bar switches the whole interface between English and
Portuguese. English is the default; the choice is stored per browser profile.

## Development

Frontend changes require a rebuild of the shipped assets:

```bash
npm --prefix regression_tool/frontend install
npm --prefix regression_tool/frontend run build
uv run regression_tool/run.py
```

Diagnostics without a GUI:

```bash
uv run python -m marlim_regression.cli detect
uv run python -m marlim_regression.cli validate-fixture
uv run python -m marlim_regression.cli build
```

Tests:

```bash
uv run pytest regression_tool/tests
```

## Platforms

The core uses `pathlib` and direct argument-list execution, with no Bash or
PowerShell. The tool follows the same platform support as `Marlim3`: Linux,
Windows and macOS. The executable name, the browser lookup, the process
detachment flags and the local data directory all adapt to the running system.
Linux was validated in this checkout; Windows with Clang/LLVM + MinGW/GFortran
and macOS with Homebrew GCC/LLVM are supported by the architecture and need
native validation on each system.

## Troubleshooting

- **Clang cannot find OpenMP**: install or configure libomp, or select a Clang toolchain that exposes OpenMP. Do not use the original build as a shortcut.
- **Empty profile**: confirm that the referenced executable is the instrumented one and that `llvm-profdata`/`llvm-cov` match the Clang version.
- **Model fails because of a missing file**: add the file under **Auxiliary files**; it is copied into the isolated workdir.
- **Interface assets missing**: run `npm --prefix regression_tool/frontend install && npm --prefix regression_tool/frontend run build`.
- **Window does not open**: confirm that Chrome, Chromium or Edge is installed; the launcher falls back to the default browser.
- **Chrome diagnostics**: the window runs with GPU and background services disabled. The browser output goes to `<local-data>/logs/browser.log`, separate from the Regression Tool logs.
