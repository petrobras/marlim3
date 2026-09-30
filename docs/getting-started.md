# Getting Started

Installation, usage, and building Marlim3 from source.

- [Installation](#installation)
- [Usage](#usage)
- [Building from source](#building-from-source)

---

## Installation

Choose the option that matches how you intend to use Marlim3.

| You want to… | Use |
|---|---|
| script simulations in Python | [pip package](#install-via-pip) |
| a graphical interface, no Python setup | [desktop application](#desktop-application) |
| run simulations from a terminal | [command-line executable](#command-line-executable) |
| modify Marlim3 itself | [developer setup](#developer-setup) |

### Install via pip

```bash
pip install marlim3
```

On supported platforms, the first package import downloads the matching native
executable from the GitHub Release when it is not already present. No compiler
is required for the published Linux x64, Windows x64, and macOS Apple Silicon
executables.

### Desktop application

Each tagged release provides a standalone desktop application containing the
Streamlit GUI, its Python runtime and dependencies, and the native Marlim3
simulation engine:

| Platform | Release asset |
|----------|---------------|
| Linux x64 | `Marlim3-desktop-linux-x64` |
| Windows x64 | `Marlim3-desktop-windows-x64.exe` |
| macOS Apple Silicon | `Marlim3-desktop-macos-arm64.dmg` |

Download the matching asset from the GitHub Releases page and run it. No Python
installation or compiler is required. The Linux desktop application requires
only glibc 2.34 or newer from the operating system.

See [marlim3_desktop/README.md](../marlim3_desktop/README.md) for platform
details, checksum verification, and diagnostic logs.

### Command-line executable

You can also download the native `Marlim3` executable for Linux, Windows, or
macOS from the GitHub Releases page. This smaller standalone executable runs
simulations directly from a terminal without installing the Python package.

### Developer setup

For development, use [uv](https://docs.astral.sh/uv/) to manage the Python
environment and dependencies.

**Step 1 — Install dependencies and the Python package:**

```bash
uv sync --locked --group dev
```

This creates a `.venv` with Python 3.12+, installs all dev tools (pytest,
flake8, jupyter, etc.), and installs `marlim3` in editable mode. After this step
you can already use `import marlim3` in your scripts:

```bash
uv run python -c "import marlim3; print(marlim3.__version__)"
```

**Step 2 — Build and register the C++/Fortran executable** (required to run
simulations):

See [Building from source](#building-from-source) below. The CMake build
automatically copies the executable into `marlim3/` after each successful build
(via a `POST_BUILD` step), so no manual copy is needed. Then run:

```bash
MARLIM3_SKIP_BUILD=1 uv sync --locked
```

`MARLIM3_SKIP_BUILD=1` skips local CMake compilation. Set
`MARLIM3_SKIP_EXECUTABLE_RESOLUTION=1` only when an import must not resolve or
download the executable.

---

## Usage

### Desktop application

The desktop application starts the Streamlit server locally on `127.0.0.1` and
renders it in an embedded Qt WebEngine desktop window. The packaged application
includes Qt and its Chromium runtime, so it does not depend on an installed web
browser or open the interface in the system browser. Simulation data remains on
the local machine. The window opens maximized with the Marlim3 logo as its
application icon. Closing the window with the title-bar `X` shuts down the local
Streamlit process tree and releases its dynamically allocated port.

**Linux**

```bash
chmod +x Marlim3-desktop-linux-x64
./Marlim3-desktop-linux-x64
```

The Linux desktop executable is a self-contained ELF binary. Its only system
runtime requirement is glibc 2.34 or newer.

**Windows** — double-click `Marlim3-desktop-windows-x64.exe`.

**macOS** — open `Marlim3-desktop-macos-arm64.dmg`, then drag `Marlim3.app` to
the `Applications` folder. The current macOS release targets Apple Silicon
(`arm64`). A signed and notarized release opens normally through Gatekeeper.
Unsigned development or fork builds may require the user to explicitly allow the
application: try to open it once, then go to **System Settings** → **Privacy &
Security** → **Open Anyway**.

SHA-256 checksum files are published alongside every desktop artifact.

Developers can build the standalone application for the current operating system
with:

```bash
uv run python -m marlim3_desktop.build
```

The script detects the platform, builds the native engine, installs the desktop
dependencies, and writes one release-named artifact to `dist/`.

### Python package

Use Marlim3 as a Python library in your scripts or notebooks. See
[marlim3/README.md](../marlim3/README.md) for the API guide, including the
bilingual interface and a worked example.

**Graphical interface.** Marlim3 includes a Streamlit-based GUI for
interactively building inputs, running simulations, and visualizing results in
the browser. From a local clone:

```bash
uv sync --group gui
uv run streamlit run gui/app.py
```

The GUI auto-detects an executable from `build/` or `marlim3/` under the
repository root. If neither is available, provide the executable path manually
in the app.

**Regression tool.** Marlim3 also includes a regression and native coverage tool
for registering models, running them against the instrumented engine, and
inspecting the execution flow graph and LLVM coverage:

```bash
uv sync --group regression_tool
uv run regression_tool/run.py
```

See [regression_tool/README.md](../regression_tool/README.md) for the toolchain
requirements, local data locations, and the full workflow.

### Command-line executable

Run Marlim3 directly from the terminal using the compiled executable available
in the Releases section on GitHub.

There are four simulation types:

```bash
# 1. Simple production system
./Marlim3 -d output_directory -i input_file

# 2. Simple injection system
./Marlim3 -d output_directory -i input_file -s INJETOR

# 3. Flow network
./Marlim3 -d output_directory -i input_file -s REDE

# 4. Natural convection in cross-section
./Marlim3 -d output_directory -i input_file -s CONVECNAT
```

**Arguments**

| Flag | Meaning |
|---|---|
| `-d directory` | **Output** directory for simulation results |
| `-i file` | Input file name (JSON format) |
| `-s TYPE` | Simulation type: `INJETOR`, `REDE`, or `CONVECNAT` |

> `-d` sets where results are *written*. The input file given with `-i` is
> resolved relative to the current working directory, not to `-d`.

**Platform-specific**

```bash
# Linux/macOS
./Marlim3 -d ./output -i simulation.json -s REDE
```

```powershell
# Windows
Marlim3.exe -d .\output -i simulation.json -s REDE
```

> **Tip:** To export results to the current working directory, use `./`
> (Linux/macOS) or `.\` (Windows) as the directory name.

---

## Building from source

Compilation is only necessary if you need to rebuild the executable from source.

### Requirements

- GCC/G++ >= 10.0 (the sources are built as C++20)
- GFortran >= 10.0
- CMake >= 3.16

### Presets

The project uses [CMake presets](../CMakePresets.json):

| Preset | Platform | Description |
|--------|----------|-------------|
| `gcc-release` / `gcc-debug` | Linux / macOS | GCC portable build with GNU runtime linking configured by CMake |
| `mingw-release` / `mingw-debug` | Windows | MinGW portable build with full static linking |
| `clang-release` / `clang-debug` | Linux / macOS | Clang 20 + GFortran portable build |

Release assets are built and tested for Linux x64, Windows x64, and macOS ARM64,
so end users do not need GCC runtime libraries installed.

### Linux

```bash
cmake --preset gcc-release
cmake --build --preset gcc-release -j$(nproc)
```

### macOS (Apple Silicon)

Install Homebrew GCC and CMake before building locally:

```bash
brew install gcc cmake
cmake --preset gcc-release
cmake --build --preset gcc-release -j$(sysctl -n hw.ncpu)
```

### Windows (MSYS2 / MinGW64)

Ensure `g++` and `gfortran` are in your PATH (e.g., via
[MSYS2](https://www.msys2.org/) with the `mingw-w64-x86_64-gcc` and
`mingw-w64-x86_64-gcc-fortran` packages).

```bash
cmake --preset mingw-release
cmake --build --preset mingw-release -j%NUMBER_OF_PROCESSORS%
```

The resulting `build/Marlim3.exe` is fully statically linked and does not
require external DLLs.

### After building

The compiled executable is placed at `build/Marlim3` on Linux and macOS, or
`build/Marlim3.exe` on Windows.

A CMake `POST_BUILD` step automatically copies the executable to `marlim3/`
after each successful build, so the Python package always picks up the latest
binary. No manual copy is needed.

Then activate the package locally, skipping recompilation:

```bash
MARLIM3_SKIP_BUILD=1 uv sync --locked
```

### Optional C++ unit tests

Verification tests for the numerical core are opt-in, so they do not affect the
default build:

```bash
cmake -S . -B build -DMARLIM_BUILD_TESTS=ON
cmake --build build --target test_friction_factor
ctest --test-dir build --output-on-failure
```

### Run the Python test suite

```bash
uv run pytest tests/ -v
```

See [tests/README.md](../tests/README.md) for markers, regression references,
and how to update them.
