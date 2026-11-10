"""Regressions for upstream fixes adapted to the extracted SisProd modules."""

import json
import os
from pathlib import Path
import shutil
import subprocess

import numpy as np
import pandas as pd
import pytest

from tests.test_initial_fluid_id import engine as engine

ROOT = Path(__file__).resolve().parents[2]
pytestmark = pytest.mark.simulacao


@pytest.fixture
def production_model():
    path = ROOT / "demos" / "simplifiedProduction" / "producaoSimplificado.mr3"
    model = json.loads(path.read_text(encoding="utf-8"))
    model["configuracaoInicial"].update({"permanente": True, "transiente": False, "saidaTela": False})
    return model


def run_model(engine, directory, model, environment=None):
    directory.mkdir()
    (directory / "model.mr3").write_text(json.dumps(model), encoding="utf-8")
    process = subprocess.run(
        [str(engine), "-d", str(directory), "-i", "model.mr3", "-t", "1"],
        cwd=directory, env=environment, capture_output=True, text=True, errors="replace", timeout=60,
    )
    assert process.returncode == 0, process.stdout[-3000:] + process.stderr[-3000:]
    return json.loads((directory / "simulacao.log").read_text(encoding="utf-8"))["resultadoSimulacao"]


def read_profile(directory):
    frame = pd.read_csv(directory / "PERFISP-0.dat", sep=";", skiprows=1, skipinitialspace=True)
    frame = frame.dropna(axis="columns", how="all")
    frame.columns = frame.columns.str.strip()
    assert not frame.empty and np.isfinite(frame.to_numpy()).all()
    return frame


def test_fully_open_icv_preserves_unrestricted_ipr_flow(engine, production_model, tmp_path):
    reference_dir = tmp_path / "unrestricted"
    assert run_model(engine, reference_dir, production_model)["status"] == "sucesso"
    reference = read_profile(reference_dir)
    assert reference.filter(regex="Vazao").to_numpy().max() > 0

    production_model["ipr"][0].update({"ICV": True, "tempoICV": [0], "abertura": [1.0], "cd": 0.84})
    open_dir = tmp_path / "fully_open"
    assert run_model(engine, open_dir, production_model)["status"] == "sucesso"
    np.testing.assert_allclose(read_profile(open_dir).to_numpy(), reference.to_numpy(), rtol=1e-8, atol=1e-10)


def select_tuffp(model):
    model["configuracaoInicial"]["tipoModeloDrift"] = False
    for pipe in model["dutosProducao"]:
        pipe["correlacaoMR2"] = 51


def test_tuffp_is_rejected_during_input_when_library_is_missing(engine, production_model, tmp_path):
    select_tuffp(production_model)
    environment = {**os.environ, "MARLIM3_TUFFP_LIB": str(tmp_path / "missing-tuffp-library")}
    result = run_model(engine, tmp_path / "missing", production_model, environment)
    assert result["status"] == "falha"
    assert any(event["log"] == "FALHA" and "TUFFP" in event["descricao"] for event in result["logs"])


@pytest.fixture(scope="module")
def tuffp_contract_library(tmp_path_factory):
    cmake = shutil.which("cmake")
    if not cmake:
        pytest.skip("CMake and a C++ compiler are required for the TUFFP integration test double")
    build = tmp_path_factory.mktemp("tuffp-contract-build")
    commands = [
        [cmake, "-S", str(ROOT / "tools" / "tests" / "fixtures" / "tuffp"), "-B", str(build),
         "-DCMAKE_BUILD_TYPE=Release", f"-DMARLIM3_INCLUDE_DIR={ROOT / 'src' / 'include'}"],
        [cmake, "--build", str(build), "--config", "Release", "--parallel", "2"],
    ]
    for command in commands:
        process = subprocess.run(command, capture_output=True, text=True, errors="replace", timeout=120)
        assert process.returncode == 0, process.stdout + process.stderr
    libraries = [path for path in build.rglob("*marlim3_tuffp_contract_fixture.*")
                 if path.suffix in {".dll", ".so", ".dylib"}]
    assert len(libraries) == 1, libraries
    return libraries[0]


@pytest.mark.parametrize(("pattern", "arrangement"), [(10, 1), (11, 1), (12, 2), (14, -1), (15, -1), (16, -2), (18, -2)])
def test_tuffp_patterns_reach_refactored_steady_solver(
    engine, production_model, tuffp_contract_library, pattern, arrangement, tmp_path,
):
    select_tuffp(production_model)
    environment = {**os.environ, "MARLIM3_TUFFP_LIB": str(tuffp_contract_library),
                   "MARLIM3_TEST_TUFFP_PATTERN": str(pattern)}
    directory = tmp_path / "tuffp"
    result = run_model(engine, directory, production_model, environment)
    assert result["status"] == "sucesso", result["logs"]
    profile = read_profile(directory)
    patterns = profile["Indicador de arranjo de fases (-) F"].iloc[2:]
    assert (patterns == arrangement).all(), patterns.to_list()


def test_tuffp_numerical_warning_is_reported_once(engine, production_model, tuffp_contract_library, tmp_path):
    select_tuffp(production_model)
    environment = {**os.environ, "MARLIM3_TUFFP_LIB": str(tuffp_contract_library), "MARLIM3_TEST_TUFFP_WARNING": "1"}
    result = run_model(engine, tmp_path / "warning", production_model, environment)
    assert result["status"] == "sucesso", result["logs"]
    reports = [event for event in result["logs"] if "contract warning" in event["causa"]]
    assert len(reports) == 1
    assert reports[0]["log"] == "AVISO" and reports[0]["codigo"] == "1004"
    assert "film gas friction (it=7, contract warning)" in reports[0]["causa"]
