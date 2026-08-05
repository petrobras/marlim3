---
name: marlim3-valves-choke
description: Use when adding flow restrictions or events to a Marlim3 simulation — inline two-phase valves, production/service master valves (Wet Christmas Tree), surface and injection chokes, PIG runs, and pressure-coupled leaks/openings (pressureSource). Essential for shutdown/restart scenarios.
---

# Marlim3 Valves, Chokes, PIG & Leaks

## Authoritative files

- [docs/user-guide/accessories.md](../../../docs/user-guide/accessories.md) — all devices below (**read this**)
- [docs/user-guide/boundary-conditions.md](../../../docs/user-guide/boundary-conditions.md) — `surfaceChoke`, `injectionChoke` context
- [demos/extended-shutdown-combined-ESP-CGL-PIG-complete.mr3](../../../demos/extended-shutdown-combined-ESP-CGL-PIG-complete.mr3) — shutdown with valves + PIG

## Common pattern

Opening schedules pair `time` with `opening` (0 = closed, 1 = fully open). `cvCurve`: 0 = openings are **area ratios**, 1 = openings are **stem displacement** (then calibrate with `x1`/`cv1` arrays). Prefer ramps over instantaneous steps for numerical stability. Sachdeva is the two-phase choke model.

## Inline two-phase valve — `valve` (array; PT `valvula`)

`id`, `measuredLength`, `cvCurve`, `cd` (typ. 0.84), `time`, `opening`, optional `x1`/`cv1`.

## Production master valve — `masterValve` (object; PT `master1`)

Wet-Christmas-Tree valve on the production line. In steady state mainly marks position; **always define it in transients where WCT operation matters** (shutdown/restart). Fields: `active`, `measuredLength`, `cvCurve`, `time`, `opening`, `activeAreaRatio` (PT `razaoAreaAtiva`, avoids instability near fully-open), `x1`/`cv1`. Discharge coefficient is fixed at 0.84 internally.

## Service master valve — `masterValve2` (object; PT `master2`)

Simplified/binary service-line WCT valve; same shape as `masterValve`.

## Surface choke — `surfaceChoke` (object; PT `chokeSup`)

Outlet restriction upstream of the separator: `cvCurve`, `time`, `opening`, `dischargeCoefficient`, `model` (only 0 = Sachdeva), `x1`/`cv1`.

## Injection choke — `injectionChoke` (object; PT `chokeInj`)

Single-phase gas choke on the service line: `active`, `time`, `opening`, `dischargeCoefficient`.

## PIG — `pig` (array)

`launcher` (PT `lancador`), `receiver` (PT `recebedor`) — measured lengths with `launcher < receiver`, both within the production line — and `time` (launch instant).

## Leak / opening — `pressureSource` (array; PT `fontePressao`)

Exchange point between tubing and an external pressurized medium:

- Fluid: `fluidType` 1 = same as tubing fluid, 0 = external fluid via `prodFluidId`; `gasAmbient` to admit only gas.
- Direction `check`: 0 bidirectional, 1 vacuum-breaker (in only), −1 check-valve (out only).
- Opening: `openingType` (0 area ratio, 1 diameter ratio), `opening`, `time`, `cd`.
- External state: `pressure`, `temperature`, `beta`, `ambientFluidQuality` (PT `titAmb`).

## Shutdown/restart recipe (typical transient)

1. Steady state (`initialCondition: 1`) as the initial state.
2. Close `surfaceChoke` and/or `masterValve` with a ramp (e.g., `time: [t0, t0+300]`, `opening: [1, 0]`).
3. Tighten `time.maxDT` around the closure window; consider `time` segregation windows for long cooldowns.
4. Restart: reopen with ramps; optionally start from a snapshot (`initialCondition: 2`).

## Validation checklist

- All `time`/`opening` arrays equal length; openings within [0, 1]; times monotonic.
- Positions within line lengths; PIG `launcher < receiver`.
- Valve events covered by output trends (place `productionTrend` gauges near valves) and by tighter `maxDT` in the time schedule.
