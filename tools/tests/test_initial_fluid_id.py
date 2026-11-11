"""The initial fluid (`iniFluidoP`) is a fluid id, also when the steady state is computed."""

import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

import pytest


ROOT = Path(__file__).resolve().parents[2]
DEMO = ROOT / "demos" / "2zones-2GLVs-2-Check-PA"
MODEL = "2zonas-2VGLs-2-Check-AP.mr3"
pytestmark = pytest.mark.simulacao


@pytest.fixture(scope="module")
def engine():
    configured = os.environ.get("MARLIM3_TEST_EXECUTABLE")
    suffix = ".exe" if sys.platform == "win32" else ""
    candidates = [Path(configured)] if configured else [
        ROOT / "build" / f"Marlim3{suffix}",
        ROOT / "tools" / "marlim3" / f"Marlim3{suffix}",
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    pytest.skip("Compiled engine not found; set MARLIM3_TEST_EXECUTABLE")


def run_steady_state(engine, directory, id_offset, initial_fluid=None):
    directory.mkdir(parents=True)
    shutil.copy2(DEMO / "PVTSIM-MARLIM.tab", directory)
    model = json.loads((DEMO / MODEL).read_text(encoding="utf-8"))
    model["configuracaoInicial"].update({"AP": False, "transiente": False})
    model["configuracaoInicial"]["iniFluidoP"] += id_offset
    if initial_fluid is not None:
        model["configuracaoInicial"]["iniFluidoP"] = initial_fluid
    for fluid in model["fluidosProducao"]:
        fluid["id"] += id_offset
    for ipr in model["ipr"]:
        ipr["indiFluidoPro"] += id_offset
    (directory / "model.mr3").write_text(json.dumps(model), encoding="utf-8")
    process = subprocess.run(
        [str(engine), "--input", "model.mr3", "--dir", str(directory)], cwd=directory,
        capture_output=True, text=True, errors="replace", timeout=300,
    )
    assert process.returncode == 0, process.stdout[-3000:] + process.stderr[-3000:]
    # first line holds the run date
    return (directory / "PERFISP-0.dat").read_text(encoding="utf-8", errors="replace").splitlines()[1:]


def test_initial_fluid_id_does_not_need_to_match_its_position(engine, tmp_path):
    reference = run_steady_state(engine, tmp_path / "ids_from_zero", 0)
    shifted = run_steady_state(engine, tmp_path / "ids_from_five", 5)
    assert shifted == reference


def test_steady_state_keeps_legacy_initial_fluid_position_with_a_warning(engine, tmp_path):
    reference = run_steady_state(engine, tmp_path / "fluid_id", 5)
    directory = tmp_path / "legacy_position"
    legacy = run_steady_state(engine, directory, 5, initial_fluid=0)
    assert legacy == reference
    result = json.loads((directory / "simulacao.log").read_text(encoding="utf-8"))["resultadoSimulacao"]
    assert result["status"] == "sucesso"
    assert any(event["codigo"] == "1004" and "iniFluidoP" in event["descricao"] for event in result["logs"])
