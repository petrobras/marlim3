<h1 align="center">
<img src="img/logo_marlim3.svg" alt="drawing" width="300"/>

</h1><br>

Marlim3 is a multiphase flow simulator developed by Petrobras.

The types of simulations offered by Marlim3 include:

- **Multiphase flow systems**: Steady-state or transient production wells
- **Single-phase flow systems** (steady-state): Water or gas injection wells, typically under supercritical conditions
- **Networks**:
    - Standard production networks (steady-state and transient)
    - Standard injection networks (steady-state)
    - Gas lift rings (steady-state and transient)
- **Natural convection**: 2D solutions for natural convection analysis in confined spaces (single-phase or two-phase). Example: modeling natural convection in a pipeline cross-section during a production shutdown

## Installation

### Option 1: Install via pip

Install Marlim3 as a Python package:

```bash
python3 -m venv ~/marlim3-env
source ~/marlim3-env/bin/activate
pip install -e . -v
```

If you prefer a regular installation instead of editable mode:

```bash
pip install .
```

#### Requirements

- Python >= 3.12


### Option 2: Use the executable directly

If you already have the Marlim3 executable available, you can run simulations directly from the terminal without installing the Python package, as detailed below.

## Usage

Marlim3 can be used in **two different ways**:

### Option 1: Python Package

Use Marlim3 as a Python library in your scripts:

```python
import marlim3

# Your simulation code here
# Example: configure and run simulations programmatically
```

**This option is ideal for:**
- Integration with other Python tools and workflows
- Automated batch simulations
- Pre/post-processing with Python libraries (pandas, numpy, matplotlib, etc.)
- Building custom applications

### Option 2: Command-Line Executable

Run Marlim3 directly from the terminal using the compiled executable.

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

Compilation is only necessary if you need to build or rebuild the executable from source.

### Requirements

- GCC/G++ >= 9.0
- GFortran >= 9.0

### Build the executable

```bash
cd src
make clean all
```

If you wish to use the new executable with the Python package, copy the generated file to the `marlim3` directory and install it locally using pip.

## Note

Several resources and portions of the source code are currently written in Portuguese. We plan to gradually translate all content into English.