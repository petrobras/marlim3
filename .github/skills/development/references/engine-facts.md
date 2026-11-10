# Engine facts

Behaviour observed in numerical experiments with v3.10.1 and v3.10.2 (single branch, steady state unless noted). Check the code before relying on a value here, and update this page when a fact changes.

## Units and conversions

- Input and output: pressure in kgf/cm², temperature in °C, standard rates in Sm³/d, lengths in m. The productivity index is in (Sm³/d)/(kgf/cm²); `tipoIPR` 0 is the linear IPR.
- Output `RS` in Sm³/Sm³ = RS in scf/bbl × 6.29 / 35.31467.
- In-situ water fraction: BSW / (BSW + (1 − BSW)·Bo). Water mass fraction of the liquid: water / (water + (1 − BSW)·ρo·Bo).
- Water viscosity (`ProFlu::VisAgua`): 2.414e-2 · 10^(247.8 / (T + 133.15)) cP, T in °C.
- Live-oil viscosity: Beggs-Robinson with the constants 10.715 and 5.439.

## Models and defaults

- `tipoModeloDrift` defaults to the drift-flux model. C0 and Ud come from Bhagwat-Ghajar by default (`configuracaoInicial.correlacoesPorArranjo`); with black-box correlations the outputs report C0 = 1 and Ud = 0.
- Emulsion (`ProFlu::emul`): models 1–4 and 8 multiply the result by ρL/ρo; models 5, 6 and 9 overwrite the multiplier. Models 8 and 9 exist only in the engine and in the Portuguese schema.
- A `fonteGas` at the same position as a `fonteLiquido` is ignored (gas rate 0). For two-phase test models use `fonteMassa` with `tipoTermo` 1 (`vazaoMassT`, `vazaoMassC`, `vazaoMassG`).

## Known limitations

- Below about 0.05–0.07 m/s the energy equation is not solved and the Reynolds and Nusselt outputs keep stale values.
- `TransCal::fric` returns four times the Haaland value, which raises the Nusselt number by about 2.5×.
- Black-box correlations: friction about 3.5% different in reverse flow; Gomez gives zero friction for single-phase liquid off the vertical.
- Descending liquid with a low outlet pressure has no feasible solution, and Mukherjee-Brill loops instead of failing.
- `SProd::buscaProdPfundoPerm` cannot start the `producaoSimplificado` demo with BSW between 0.98 and 0.995.
- A zero-flow steady state (static column) cannot be computed (issue #97); start from a transient instead.
