# Copilot instructions

Marlim3 is Petrobras's 1D multiphase flow simulator. Inputs are JSON files (`.mr3`), built and run through the bilingual (EN/PT) Python API (`marlim3.Branch` / `marlim3.Tramo`) or the compiled executable. Authoritative references: [docs/single-branch-model-reference/](../docs/single-branch-model-reference/index.md), [docs/schemas/branch.en.json](../docs/schemas/branch.en.json), [demos/](../demos/simplifiedProduction/simplifiedProduction.mr3), and [tests/README.md](../tests/README.md).

## Simulation agent pipeline

This repo ships custom agents in [.github/agents/](agents/marlim3.agent.md) and skills in `.github/skills/` for building complete simulations. For any "create a simulation" request, prefer delegating to this pipeline instead of improvising:

1. **marlim3** (orchestrator) first runs the environment preflight (`.github/skills/marlim3-python-api/scripts/check_environment.py`) and stops, reporting what is missing, unless it prints `READY`; then it coordinates everything below and manages the QA fix loop.
2. **marlim3-planner** interviews the user (only the gaps), writes an ADR to `<session>/<slug>.adr.md`, and gets it approved. It never writes code.
3. **marlim3-specialist** implements the approved ADR — `<session>/<slug>.mr3` plus `<session>/test_<slug>.py`, following `.github/skills/marlim3-testing/SKILL.md` — and verifies it with `.github/skills/marlim3-json-schema/scripts/check_case.py`.
4. **marlim3-qa** verifies the implementation against the ADR and writes `<session>/<slug>.qa.md` with a PASS/FAIL verdict.

**Session folder.** `<session>` above is `agent_sessions/<YYYY-MM-DD>-<slug>/`: one subfolder per pipeline run inside the git-ignored `agent_sessions/`. Every file the pipeline generates goes there and nowhere else in the repository. Because the folder is git-ignored, workspace search may skip it — list it and open its files by explicit path.

**Keep context small.** Skills are the primary context; open the documents they link only for what a skill doesn't cover. Never read the large references whole — `docs/schemas/branch.en.json` (~157 KB) and the ESP/shutdown demos (80–100 KB): search them for the field or object you need.

See [README-agents.md](README-agents.md) for the user-facing guide.
