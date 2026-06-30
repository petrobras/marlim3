<h1 align="center">
	<img src="img/logo_marlim3.svg" alt="Marlim3 logo" width="320"/>
</h1>

Marlim3 is a transient 1D multiphase-flow simulator for production and injection systems in oil and gas applications.

## Core Capabilities

- **Production wells**
- **Injection wells** (water or gas)
- **Network**
	- Production networks
	- Injection networks
	- Gas-lift loops
- **Artificial lift models**
	- Gas-lift valves
	- ESP pumps
	- Volumetric pumps

## Advanced modeling

- **Natural convection**: 2D solutions for natural convection analysis in confined spaces (single-phase or two-phase), such as pipeline cross-sections during production shutdowns
- **Compositional fluid model library**
- **Near wellbore model**: radial and 2D models to consider phenomena such as water coning
- **Thermal diffusion**: 2D and 3D coupled to the 1D flow model

## How You Can Use Marlim3

- **Streamlit GUI** for interactive model setup and result inspection
- **Python package** for automation, integration, and parametric studies
- **Command-line executable** for direct simulation workflows

For installation instructions and usage guidelines in any of these settings, please refer to the [official repository](https://github.com/petrobras/marlim3).

## User Guide Map

| Concept | What You Learn |
|---------|----------------|
| [Fluids](user-guide/fluids.md) | Fluid-model families, PVT files, and property applicability by model |
| [Materials](user-guide/materials.md) | Thermal-property definitions for wall layers and special material types |
| [Cross Sections](user-guide/cross-sections.md) | Radial geometry, annular behavior, and layer construction |
| [Rock Formation](user-guide/rock-formation.md) | Formation thermal memory and rock property assignment |
| [Pipes](user-guide/pipes.md) | Segment geometry, discretization, ambient coupling, and thermal links |
| [Accessories](user-guide/accessories.md) | Sources, valves, pumps, pressure devices, and pigging |
| [Boundary Conditions](user-guide/boundary-conditions.md) | Inlet/outlet closure strategies and injection boundary modes |
| [Time](user-guide/time.md) | Initialization strategies, step schedule, segregation windows, snapshots |
| [General](user-guide/general.md) | Global physics switches, correlations, and advanced numerical controls |
| [Results](user-guide/results.md) | Profile/trend/cross outputs and interpretation strategy |

## Reference and Tutorials

| Section | Content |
|---------|---------|
| [JSON Schema Reference](reference/json-schema.md) | Full field catalog, object structure, and units |
| [Tutorials](tutorials/index.md) | Practical notebook-based workflows |
| [Bilingual Support](dev-guide/translations.md) | English ↔ Portuguese key mapping and usage |

## Build This Documentation

```bash
uv sync --group docs
uv run mkdocs serve
```
