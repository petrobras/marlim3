<h1 align="center">
<img src="assets/branding/logo.svg" alt="Marlim3 logo" width="300"/>

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

## Project status

Marlim3 is undergoing a **major architectural refactoring**. While that work is
in progress:

- **Not every mapped bug in the current version will be fixed.** Some defects
  live in code that the refactoring will replace, and fixing them twice is not a
  good use of anyone's time.
- **The current focus is verification.** We are consolidating reference cases for
  the regression suite and validation cases for limiting conditions, so that the
  refactored engine can be checked against known-correct behaviour rather than
  against itself.
- **Issues where external collaboration is welcome are labelled
  [`help wanted`](https://github.com/petrobras/marlim3/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22).**
  Those are scoped so they do not collide with the refactoring.

Bug reports remain valuable even when a fix is deferred — they inform what the
new architecture has to get right, and they are candidates for the reference
suite. Please keep them coming.

Broader contributions will be accepted once the refactoring settles. See
[CONTRIBUTING.md](CONTRIBUTING.md) for what is being accepted in the meantime.

## Documentation

| | |
|---|---|
| [Getting Started](docs/getting-started.md) | Installation, usage, and compilation |
| [Python package](marlim3/README.md) | Python API, bilingual interface, scripting |
| [Model reference](docs/single-branch-model-reference/index.md) | Every input section, key and unit |
| [Theory](docs/theoretical-reference/index.md) | Governing equations and discretization |
| [Developer guide](docs/dev-guide/index.md) | Engine internals |
| [Tutorials](docs/tutorials) | Step-by-step notebooks |
| [Desktop application](marlim3_desktop/README.md) | Platform notes and diagnostics |
| [Regression tool](regression_tool/README.md) | Model registration and native coverage |
| [Tests](tests/README.md) | Test suite and regression references |

The full documentation is published at
[petrobras.github.io/marlim3](https://petrobras.github.io/marlim3/).

## Contributing

Contributions are welcome — see [CONTRIBUTING.md](CONTRIBUTING.md), along with
the [Code of Conduct](CODE_OF_CONDUCT.md) and the
[Contributor License Agreement](CONTRIBUTOR_LICENSE_AGREEMENT.md). Note the
[project status](#project-status) above for what is being accepted during the
refactoring.

Please use the Discussions section for questions rather than opening issues.

## Note

Several resources and portions of the source code are currently written in
Portuguese. We plan to gradually translate all content into English.

The Python API is fully bilingual (EN/PT) — see
[Bilingual Support](docs/dev-guide/translations.md).
