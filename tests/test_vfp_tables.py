"""Integration checks for paired BHP and outlet-temperature VFP tables."""

import json
import math
import os
from pathlib import Path
import subprocess
import sys

import pytest


ROOT = Path(__file__).resolve().parents[1]
TABLE_FILES = (
    ("bhpsIMEX.imx", "whtsIMEX.imx"),
    ("bhpsEclipse.ecp", "thtsEclipse.ecp"),
    ("bhpsIMEXnew.imx", "whtsIMEXnew.imx"),
    ("bhpsEclipsenew.ecp", "thtsEclipsenew.ecp"),
)
pytestmark = pytest.mark.simulacao


@pytest.fixture(scope="module")
def engine():
    configured = os.environ.get("MARLIM3_TEST_EXECUTABLE")
    suffix = ".exe" if sys.platform == "win32" else ""
    candidates = [Path(configured)] if configured else [
        ROOT / "build" / f"Marlim3{suffix}",
        ROOT / "marlim3" / f"Marlim3{suffix}",
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    pytest.skip("Compiled engine not found; set MARLIM3_TEST_EXECUTABLE")


def run_ap(engine, directory, mode, parallel=False, rates=(1000, 2000, 3000),
           pressures=(10, 20), table=True, choke=None):
    directory.mkdir(parents=True, exist_ok=True)
    model = json.loads((ROOT / "demos/pt-br/atritoTuboLiso.mr3").read_text(encoding="utf-8"))
    model["configuracaoInicial"].update({
        "AP": True,
        "paralelizaAP": parallel,
        "arquivoAP": "leituraAP.json",
    })
    model["fonteLiquido"][0]["temperatura"] = [80.0]
    if choke is not None:
        model["chokeSup"] = {"tempo": [0], "abertura": [choke], "coeficienteDescarga": 0.84}
        model["tendP"] = [{
            "ativo": True, "comprimentoMedido": 5000, "dt": 1,
            "temperatura": True, "tempChokeJus": True,
        }]
    analysis = {
        "tipoAP": int(table),
        "vfp": mode,
        "nthread": 2,
        "imprimePerfil": choke is not None and not table,
        "psep": {"pressao": list(pressures)},
        "RGO-fluido0": {"RGO": [0]},
        "BSW-fluido0": {"BSW": [0]},
        "FonteLiquido": [{"indiceFL": 0, "vazLiq": list(rates)}],
    }
    (directory / "model.mr3").write_text(json.dumps(model), encoding="utf-8")
    (directory / "leituraAP.json").write_text(json.dumps(analysis), encoding="utf-8")
    process = subprocess.run(
        [str(engine), "--input", "model.mr3", "--dir", str(directory)], cwd=directory,
        capture_output=True, text=True, errors="replace", timeout=120,
    )
    assert process.returncode == 0, process.stdout[-4000:] + process.stderr[-4000:]
    return directory


def table_rows(path, count):
    lines = [line for line in path.read_text(encoding="utf-8", errors="replace").splitlines() if line.strip()]
    return [[float(value) for value in line.replace("/", "").split()] for line in lines[-count:]]


def generic_rows(path):
    rows = []
    for line in path.read_text(encoding="utf-8").splitlines():
        values = []
        for field in line.split(";"):
            try:
                values.append(float(field))
            except ValueError:
                continue
        if values:
            rows.append(values)
    return rows


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("parallel", [False, True], ids=["serial", "parallel"])
def test_paired_vfp_tables(engine, tmp_path, mode, parallel):
    run_ap(engine, tmp_path, mode, parallel)
    pressure_name, temperature_name = TABLE_FILES[mode]
    temperature_text = (tmp_path / temperature_name).read_text(encoding="utf-8")
    assert "BHP" not in temperature_text
    if mode in (1, 3):
        assert "METRIC" in temperature_text
        assert "FIELD" not in temperature_text
        assert "THT" in temperature_text
        assert "METRIC" in (tmp_path / pressure_name).read_text(encoding="utf-8")
        rows, values_per_row = 2, 3
    else:
        assert "*WHT\n" in temperature_text
        assert "[C]" in temperature_text
        rows, values_per_row = 3, 2

    pressure_rows = table_rows(tmp_path / pressure_name, rows)
    temperature_rows = table_rows(tmp_path / temperature_name, rows)
    for pressure, temperature in zip(pressure_rows, temperature_rows, strict=True):
        assert pressure[:-values_per_row] == temperature[:-values_per_row]
        assert len(pressure) == len(temperature)
        assert all(math.isfinite(value) and 20 < value < 100 for value in temperature[-values_per_row:])

    cases = generic_rows(tmp_path / "tabelaGenericaAP.dat")
    assert len(cases) == 6
    assert all(len(row) == 5 for row in cases)
    pressure_values = [value for row in pressure_rows for value in row[-values_per_row:]]
    assert pressure_values == [row[-1] for row in cases]


@pytest.mark.parametrize("mode", range(4))
def test_serial_parallel_vfp_agree(engine, tmp_path, mode):
    serial = run_ap(engine, tmp_path / "serial", mode)
    parallel = run_ap(engine, tmp_path / "parallel", mode, parallel=True)
    rows = 2 if mode in (1, 3) else 3
    for filename in TABLE_FILES[mode]:
        for serial_row, parallel_row in zip(
            table_rows(serial / filename, rows), table_rows(parallel / filename, rows), strict=True,
        ):
            assert serial_row == pytest.approx(parallel_row, rel=1e-4)


def test_temperature_matches_unrestricted_outlet(engine, tmp_path):
    tables = run_ap(engine, tmp_path / "tables", 3, parallel=True)
    summary = run_ap(engine, tmp_path / "summary", 3, parallel=True, table=False)
    summary_rows = [
        [float(value) for value in line.split(";") if value.strip()]
        for line in (summary / "variaveisInteresseAP.dat").read_text(encoding="utf-8").splitlines()[1:]
        if line.strip()
    ]
    temperatures = [value for row in table_rows(tables / "thtsEclipsenew.ecp", 2) for value in row[-3:]]
    assert temperatures == pytest.approx([row[3] for row in summary_rows], rel=1e-4)


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("parallel", [False, True], ids=["serial", "parallel"])
def test_temperature_matches_downstream_choke(engine, tmp_path, mode, parallel):
    tables = run_ap(engine, tmp_path / "tables", mode, parallel=parallel, rates=(1000, 2000), choke=0.02)
    summary = run_ap(engine, tmp_path / "summary", mode, rates=(1000, 2000), table=False, choke=0.02)
    trend = generic_rows(summary / "TENDP-AP-5000.dat")
    temperatures = [value for row in table_rows(tables / TABLE_FILES[mode][1], 2) for value in row[-2:]]
    assert len(trend) == len(temperatures) == 4
    assert temperatures == pytest.approx([row[3] for row in trend], abs=1e-3)
    assert any(abs(row[2] - row[3]) > 1e-3 for row in trend)


@pytest.mark.parametrize("mode", range(4))
def test_failed_vfp_case_preserves_position(engine, tmp_path, mode):
    run_ap(engine, tmp_path, mode, rates=(2000, 1e12), pressures=(10,))
    rows, values_per_row = (1, 2) if mode in (1, 3) else (2, 1)
    pressure_name, temperature_name = TABLE_FILES[mode]
    pressures = [value for row in table_rows(tmp_path / pressure_name, rows) for value in row[-values_per_row:]]
    temperatures = [value for row in table_rows(tmp_path / temperature_name, rows) for value in row[-values_per_row:]]
    assert pressures[0] != -1e10
    assert temperatures[0] != -1e10
    assert pressures[1] == temperatures[1] == -1e10