<h1 align="center">
<img src="img/logo_marlim3.svg" alt="drawing" width="300"/>

</h1><br>

`Marlim3` is a 1D multiphase flow simulator developed by Petrobras.
  
## Core capabilities (steady-state and transient)

- **Production wells**
- **Injection wells**: Water or gas injection wells, both single-phase and multiphase
- **Networks**
    - Production networks 
    - Injection networks
    - Gas lift loops
- **Artificial Lift models**: gas lift valves, pumps

## Advanced modeling

- **Natural convection**: 2D solutions for natural convection analysis in confined spaces (single-phase or two-phase), such as pipeline cross-sections during production shutdowns
- **Compositional fluid model library**
- **Near wellbore model**: radial and 2D models to consider phenomena such as water coning
- **Thermal diffusion**: 2D and 3D coupled to the 1D flow model

## Installation

### Option 1: Install via pip

Install `Marlim3` as a Python package:

```bash
pip install marlim3
```

### Option 2: Use the executable directly

You can download the `Marlim3` executable for Linux, Windows or Mac from the Releases section on GitHub. This standalone executable allows you to run simulations directly from the terminal, without the need to install the Python package. Detailed instructions are provided below.

### Option 3: Developer setup (uv)

For development, use [uv](https://docs.astral.sh/uv/) to manage the Python environment and dependencies.

**Step 1 — Install dependencies and the Python package:**

```bash
uv sync --locked --group dev
```

This creates a `.venv` with Python 3.12+, installs all dev tools (pytest, flake8, jupyter, etc.), and installs `marlim3` in editable mode. After this step you can already use `import marlim3` in your scripts:

```bash
uv run python -c "import marlim3; print(marlim3.__version__)"
```

**Step 2 — Build and register the C++/Fortran executable** (required to run simulations):

See [Compilation](#compilation) below. The CMake build automatically copies the executable into `marlim3/` after each successful build (via a `POST_BUILD` step), so no manual copy is needed. Then run:

```bash
MARLIM3_SKIP_BUILD=1 uv sync --locked
```

`MARLIM3_SKIP_BUILD=1` skips local CMake compilation. Set `MARLIM3_SKIP_EXECUTABLE_RESOLUTION=1` only when an import must not resolve or download the executable.

## Usage

### Option 1: Python Package

Use `Marlim3` as a Python library in your scripts or notebooks.

#### GUI

Marlim3 also includes a **Streamlit-based GUI** for interactively building inputs, running simulations, and visualizing results in the browser.

From a local clone:

```bash
uv sync --group gui
uv run streamlit run gui/app.py
```

The GUI auto-detects an executable from `build/` or `marlim3/` under the repository root. If neither is available, provide the executable path manually in the app.

The GUI provides forms for defining fluids, geometry, boundary conditions, and output variables, launches simulations, and displays profile plots — all in the browser.

#### Scripting

For automated workflows, parametric studies, or integration with other tools, use the Python API directly:

```python
import numpy as np
import marlim3

branch = marlim3.Branch()

# 1. Production fluid (black-oil)
branch.productionFluid = [{
    "id": 0,
    "api": 32,
    "gor": 100,
    "gasDensity": 0.7,
    "bsw": 0.0,
}]

# 2. Material (steel)
branch.material = [{
    "id": 0,
    "type": 0,
    "conductivity": 58,       # W/m.K
    "specificHeat": 480,      # J/kg.K
    "rho": 7850,              # kg/m3
}]

# 3. Cross-section (10" ID, single steel layer)
branch.crossSection = [{
    "id": 0,
    "innerDiameter": 10 * 0.0254,  # m
    "roughness": 0.183e-3,          # m
    "layers": [{"materialId": 0, "layerMeasurementType": "THICKNESS", "thickness": 0.0254}],
}]

# 4. Pipe (2500 m horizontal pipeline, 20 cells)
n_cells = 20
total_length = 2500  # m
branch.productionPipe = [{
    "id": 0,
    "crossSectionId": 0,
    "environment": 2,  # atmosphere
    "angle": 0,        # rad (0 = horizontal)
    "discretization": [{"numCells": n_cells, "length": total_length / n_cells}],
    "initialConditions": {
        "measuredPosition": [0, 1],
        "ambientTemp": [40, 20],   # degC
        "ambientVel": [0.5, 0.5],  # m/s
    },
}]

# 5. Boundary conditions
branch.liquidSource = [{
    "id": 0,
    "prodFluidId": 0,
    "measuredLength": 0.1,
    "time": [0],
    "liquidFlowRate": [1500],  # sm3/d
    "temperature": [40],       # degC
}]
branch.separator = {"time": [0], "pressure": [2]}  # kgf/cm2

# 6. Output specification
branch.productionProfile = {
    "time": [0],
    "pressure": True,
    "temperature": True,
    "holdup": True,
    "flowPattern": True,
    "frictionPressureGradient": True,
    "hydrostaticPressureGradient": True,
}

# 7. Simulate and plot
branch.simulate()
branch.plot_profiles()
```

The Python API is **fully bilingual** — you can use Portuguese or English interchangeably:

```python
import marlim3

# Build a model entirely in Portuguese
tramo = marlim3.Tramo()
tramo.sistema = "PROD"
tramo.fluidosProducao = [{"id": 0, "api": 30, "rgo": 100, "densidadeGas": 0.7, "bsw": 0.0}]
tramo.secaoTransversal = [{"id": 0, "diametroInterno": 0.254, "rugosidade": 1.83e-4,
                           "camadas": [{"idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0254}]}]

# Nested access also works in both languages
tramo.fluidosProducao[0]["densidadeGas"]  # → 0.7
tramo.productionFluid[0]["gasDensity"]    # → 0.7 (same data)

# Export in Portuguese
tramo.to_json("modelo", language='pt')
```

See [Bilingual Support](docs/translations.md) for details. For examples, refer to the tutorials in `docs/`.

### Option 2: Command-Line Executable

Run `Marlim3` directly from the terminal using the compiled executable available in the Releases section on GitHub.

#### Available Commands

There are four simulation types available:

**1. Simple Production System**
```bash
./executable_name -d directory_name -i input_file
```

**2. Simple Injection System**
```bash
./executable_name -d directory_name -i input_file -s INJETOR
```

**3. Flow Network**
```bash
./executable_name -d directory_name -i input_file -s REDE
```

**4. Natural Convection in Cross-Section**
```bash
./executable_name -d directory_name -i input_file -s CONVECNAT
```

#### Command-Line Arguments

- `-d directory_name`: Output directory for simulation results
- `-i input_file`: Input file name (JSON format)
- `-s SIMULATION_TYPE`: Simulation type (INJETOR, REDE, or CONVECNAT)

#### Platform-Specific Notes

**Linux/macOS:**
```bash
./Marlim3 -d ./output -i simulation.json -s REDE
```

**Windows:**
```powershell
Marlim3.exe -d .\output -i simulation.json -s REDE
```

> **Tip:** To export results to the current working directory, use `./` (Linux/macOS) or `.\` (Windows) as the directory name.

## Compilation

Compilation is only necessary if you need to rebuild the executable from source.

### Requirements

- GCC/G++ >= 9.0
- GFortran >= 9.0
- CMake >= 3.16

### Build the executable

The project uses [CMake presets](CMakePresets.json). Available presets:

| Preset | Platform | Description |
|--------|----------|-------------|
| `gcc-release` / `gcc-debug` | Linux / macOS | GCC portable build with GNU runtime linking configured by CMake |
| `mingw-release` / `mingw-debug` | Windows | MinGW portable build with full static linking |
| `clang-release` / `clang-debug` | Linux / macOS | Clang 20 + GFortran portable build |

Release assets are built and tested for Linux x64, Windows x64, and macOS ARM64.
Linux, Windows, and macOS ARM64 release executables are built so end users do not need GCC runtime libraries installed.

#### Linux

```bash
cmake --preset gcc-release
cmake --build --preset gcc-release -j$(nproc)
```
#### MacOS - Apple

On macOS, install Homebrew GCC and CMake before building locally:

```bash
brew install gcc cmake
cmake --preset gcc-release
cmake --build --preset gcc-release -j$(sysctl -n hw.ncpu)
```

#### Windows (MSYS2 / MinGW64)

Ensure `g++` and `gfortran` are in your PATH (e.g., via [MSYS2](https://www.msys2.org/) with the `mingw-w64-x86_64-gcc` and `mingw-w64-x86_64-gcc-fortran` packages).

```bash
cmake --preset mingw-release
cmake --build --preset mingw-release -j%NUMBER_OF_PROCESSORS%
```

The resulting `build/Marlim3.exe` is fully statically linked and does not require external DLLs.

The compiled executable is placed at `build/Marlim3`.

A CMake `POST_BUILD` step automatically copies the executable to `marlim3/` after each successful build, so the Python package always picks up the latest binary. No manual copy is needed.

Then activate the package locally (skipping recompilation):

```bash
MARLIM3_SKIP_BUILD=1 uv sync --locked
```

`MARLIM3_SKIP_BUILD=1` skips local CMake compilation; installed packages can still download the release executable on import.

### Run tests

```bash
uv run pytest tests/ -v
```

## Note

Several resources and portions of the source code are currently written in Portuguese. We plan to gradually translate all content into English.

The Python API is fully bilingual (EN/PT) — see [docs/translations.md](docs/translations.md).
