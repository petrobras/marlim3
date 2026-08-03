<h1 align="center">
	<img src="img/logo_marlim3.svg" alt="Marlim3 logo" width="320"/>
</h1>

`Marlim3` is a one-dimensional multiphase flow simulator based on the drift-flux model, featuring steady-state and transient capabilities for oil and gas production and injection systems.

## Core Capabilities

- **Production wells**
- **Injection wells** (water or gas)
- **Integrated subsea arrangements**
	- Production networks
	- Injection networks
	- Gas-lift loops
- **Artificial lift models**
	- Gas-lift valves
	- ESP pumps
	- Volumetric pumps

## Advanced Modeling

- **Natural convection**: 2D solutions for natural convection analysis in confined spaces (single-phase or two-phase), such as for the cooldown in pipeline cross-sections during shutdowns
- **Compositional fluid model library**
- **Near wellbore model**: radial and 2D models to consider phenomena such as water coning (multiphase flow in a porous medium)
- **Thermal diffusion**: 2D and 3D coupled to the 1D flow model (for partially and fully buried flowlines)

## How You Can Use `Marlim3`

- **Streamlit GUI** for interactive model setup and result inspection (available also as a standalone app in GitHub Releases section)
- **Python package** for automation, integration, and parametric studies
- **Command-line executable** for direct simulation workflows

For installation instructions and usage guidelines in any of these settings, please refer to the [official repository](https://github.com/petrobras/marlim3).

## Documentation Map

| Section | Content |
|---------|---------|
| [Tutorials](tutorials/index.md) | Step-by-step notebook-based workflows for common simulation tasks |
| [Single-Branch Model Reference](single-branch-model-reference/index.md) | Full field catalog for fluids, pipes, accessories, boundary conditions, and more |
| [Theoretical Reference](theoretical-reference/index.md) | Mathematical foundations: mass/momentum/energy balances and discretization |
| [Developer Guide](dev-guide/index.md) | Source code architecture, classes, solvers, and extension points |
| [Schemas](schemas/index.md) | JSON schemas for model verification and auto-completing Marlim3 input files |

## Build This Documentation

```bash
uv sync --group docs
uv run mkdocs serve
```
