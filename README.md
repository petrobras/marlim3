<h1 align="center">
<img src="img/logo_marlim3.svg" alt="drawing" width="300"/>

</h1><br>

`Marlim3` is a 1D multiphase flow simulator developed by Petrobras.
 
The types of simulations offered by `Marlim3` include:
 
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

You can download the `Marlim3` executable for Linux or Windows from the Releases section on GitHub. This standalone executable allows you to run simulations directly from the terminal, without the need to install the Python package. Detailed instructions are provided below.

## Usage

### Option 1: Python Package

Use `Marlim3` as a Python library in your scripts:

```python
import marlim3

# Your simulation code here
# Example: configure and run simulations programmatically
```

For examples, refer to the tutorials available in the `docs` folder.

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

### Build the executable

```bash
cd src
make clean all
```

If you wish to use the new executable with the Python package, copy the generated file to the `marlim3` directory and install it locally using pip.

## Note

Several resources and portions of the source code are currently written in Portuguese. We plan to gradually translate all content into English.