---
name: marlim3-qa-checklist
description: Use when verifying a Marlim3 simulation implementation against its ADR — the full validation checklist (plan conformance, schema validation, ID cross-references, array consistency, physical plausibility, tests), and the QA report template written to docs/<slug>.qa.md.
---

# Marlim3 QA Checklist & Report

QA compares the implementation (`.mr3` / Python / tests) against its ADR at `docs/<slug>.adr.md` and produces `docs/<slug>.qa.md`. QA **reports**; it does not fix (small, unambiguous typos may be flagged with a suggested patch).

## 1. Plan conformance

- [ ] Every ADR "Decisions" section is implemented; field values match ADR numbers exactly (units included).
- [ ] All ADR deliverables exist (input file, script if promised, test file).
- [ ] No unauthorized additions — extra accessories/settings not in the ADR are findings.
- [ ] Assumed defaults in the ADR match what the implementation actually uses.

## 2. Schema validation (run it, don't eyeball)

```bash
uv run --group gui python -c "
import json, jsonschema
schema = json.load(open('docs/schema_branch.json'))     # schema_tramo.json for PT files
data = json.load(open('simulations/<slug>/<slug>.mr3'))
jsonschema.validate(data, schema); print('SCHEMA OK')"
```

Also load it through the API (catches key typos — unknown keys silently pass JSON but die in the engine): `marlim3.Branch().from_json(path)` then spot-check translated attributes.

## 3. Cross-references (validate each per the table in [marlim3-json-schema](../marlim3-json-schema/SKILL.md))

- [ ] `crossSectionId`, `materialId`, `formationId`, `prodFluidId`, `initialFluidId` all resolve.
- [ ] No duplicate IDs within an array.
- [ ] Every `measuredLength` (sources, valves, pumps, trends, PIG launcher/receiver) ≤ total line length; PIG `launcher < receiver`.
- [ ] Gas-lift taps within their respective lines; service-line objects only when `gasLine: true`.

## 4. Array consistency

- [ ] All paired schedules equal length (`time`/`pressure`, `time`/`opening`, `times`/`maxDT`, ESP curves, IPR `*Time` pairs).
- [ ] Time arrays monotonic, starting at 0; profile output times ≤ `time.finalTime`.
- [ ] `measuredPosition` profiles span 0→1; openings within [0, 1].

## 5. Physical plausibility

| Quantity | Sane range |
|----------|-----------|
| API | 5–60 | 
| GOR | 0–2000 sm³/sm³ |
| BSW | 0–1 |
| Gas relative density | 0.55–1.5 |
| Pressures | > 0, typically 1–700 kgf/cm² |
| Temperatures | −10 to 200 °C |
| Inner diameter | 0.05–0.6 m |
| Roughness | 1e-6–1e-3 m |
| Angle | −π/2 to π/2 rad |
| Insulation conductivity | ≥ 0.02 W/(m·°C) (flag lower) |
| Trend `dt` | > 0 in transient |

Also: total line lengths match the ADR topology; cell sizes reasonable (1–200 m); `compTable` grid covers reservoir-to-separator P/T envelope.

## 6. Execute

- [ ] Run the case's tests: `uv run pytest tests/test_<slug>.py -v` (and `-m simulacao` if the executable is available). Record pass/fail/skips verbatim.
- [ ] If the simulation ran: inspect `simulacao.log` for `FALHA` entries; confirm expected result keys/DataFrames exist; check ADR acceptance criteria against actual numbers.
- [ ] Confirm no repo pollution: no stray output directories, `.mr3` files at repo root, or modified unrelated files (`git status`).

## Report template — write to `docs/<slug>.qa.md`

```markdown
# QA Report: <title>

- **Status**: PASS | PASS WITH WARNINGS | FAIL
- **ADR**: docs/<slug>.adr.md
- **Implementation**: <files reviewed, with commit/state>
- **Date**: <YYYY-MM-DD>

## Summary
<2–4 sentences: what was verified and the verdict.>

## Checks
| # | Check | Result | Evidence |
|---|-------|--------|----------|
| 1 | Plan conformance | ✅/⚠️/❌ | ... |
| 2 | Schema validation | ✅/❌ | command output |
| 3 | Cross-references | ✅/❌ | ... |
| 4 | Array consistency | ✅/❌ | ... |
| 5 | Physical plausibility | ✅/⚠️ | ... |
| 6 | Tests & simulation run | ✅/❌ | pytest output summary |

## Findings
| # | Severity | Location | Description | Suggested fix |
|---|----------|----------|-------------|---------------|
| 1 | ERROR/WARNING/INFO | file:field | ... | ... |

## Acceptance criteria
<Each ADR criterion with measured value and verdict.>

## Recommendation
<Accept, or return to specialist with the ERROR-severity findings listed.>
```

Severity: **ERROR** = would fail or invalidate the simulation (blocks acceptance); **WARNING** = runs but questionable physics/robustness; **INFO** = style/consistency.
