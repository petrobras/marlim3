"""Tests for the standalone Marlim3 tpl/ppl converter (no simulator required)."""
import importlib.util
import sys
import types
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parent.parent
CONVERSOR_PATH = REPO_ROOT / "marlim3" / "_conversores" / "_conversor_marlim3_tplppl.py"
OUTPUT_HEADERS_PATH = REPO_ROOT / "marlim3" / "_output_headers.py"


def _load_module_without_marlim3_init():
    """Import the converter module without triggering marlim3/__init__.py (avoids
    pulling in pandas/seaborn/the compiled simulator, which this test does not need)."""
    package = types.ModuleType("marlim3")
    package.__version__ = "test"
    package.__path__ = [str(REPO_ROOT / "marlim3")]
    sys.modules["marlim3"] = package

    headers_spec = importlib.util.spec_from_file_location("marlim3._output_headers", OUTPUT_HEADERS_PATH)
    headers_module = importlib.util.module_from_spec(headers_spec)
    sys.modules["marlim3._output_headers"] = headers_module
    headers_spec.loader.exec_module(headers_module)

    conversores_package = types.ModuleType("marlim3._conversores")
    conversores_package.__path__ = [str(REPO_ROOT / "marlim3" / "_conversores")]
    sys.modules["marlim3._conversores"] = conversores_package

    spec = importlib.util.spec_from_file_location(
        "marlim3._conversores._conversor_marlim3_tplppl", CONVERSOR_PATH)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


conversor = _load_module_without_marlim3_init()


# ---------------------------------------------------------------------------
# Fixtures de arquivos .dat sintéticos
# ---------------------------------------------------------------------------

def _write_perfis_pt(path, times, label_col="Elevacao (m) F"):
    header = "Tempo (s);Comprimento (m) Fronteira F;%s;Pressao (kgf/cm2) F\n" % label_col
    lines = [header]
    for t in times:
        lines.append(f"{t};0.0;0.0;100.0\n")
        lines.append(f"{t};10.0;-5.0;95.0\n")
    path.write_text("PERFISP\n" + "".join(lines), encoding="utf-8")


def _write_perfis_en(path, times):
    header = "Time (s);Length (m) Boundary F;Elevation (m) F;Pressure (kgf/cm2) F\n"
    lines = [header]
    for t in times:
        lines.append(f"{t};0.0;0.0;100.0\n")
        lines.append(f"{t};10.0;-5.0;95.0\n")
    path.write_text("PERFISP\n" + "".join(lines), encoding="utf-8")


def _write_tend(path, times, position="10.0", label="Poco1", cell="3", pt_col="Pressao (kgf/cm2)"):
    header = (
        f"# Comprimento a partir do Fundo de Poco (m) = {position}\n"
        f"# Rotulo = {label}\n"
        f"# Indice da Celula = {cell}\n"
        f"Tempo (s);{pt_col} F\n"
    )
    lines = [f"{t};{100.0 + i}\n" for i, t in enumerate(times)]
    path.write_text(header + "".join(lines), encoding="utf-8")


# ---------------------------------------------------------------------------
# Testes
# ---------------------------------------------------------------------------

def test_convert_consolidates_perfisp_and_perfisg_with_same_times(tmp_path):
    _write_perfis_pt(tmp_path / "PERFISP-0.dat", [0, 60])
    (tmp_path / "PERFISG-0.dat").write_text(
        "PERFISG\nTempo (s);Comprimento (m) Fronteira F;Profundidade (m) F;Temperatura (C) F\n"
        "0;0.0;0.0;50.0\n0;10.0;-5.0;48.0\n60;0.0;0.0;50.0\n60;10.0;-5.0;48.0\n",
        encoding="utf-8",
    )

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))

    ppl_file = tmp_path / f"{tmp_path.name}.ppl"
    assert ppl_file.exists()
    content = ppl_file.read_text(encoding="utf-8")
    assert "PT " in content
    assert "TM " in content
    assert not (tmp_path / f"{tmp_path.name}_P.ppl").exists()
    assert not (tmp_path / f"{tmp_path.name}_G.ppl").exists()


def test_convert_splits_perfisp_and_perfisg_with_different_times(tmp_path):
    _write_perfis_pt(tmp_path / "PERFISP-0.dat", [0, 60])
    (tmp_path / "PERFISG-0.dat").write_text(
        "PERFISG\nTempo (s);Comprimento (m) Fronteira F;Profundidade (m) F;Temperatura (C) F\n"
        "0;0.0;0.0;50.0\n0;10.0;-5.0;48.0\n120;0.0;0.0;50.0\n120;10.0;-5.0;48.0\n",
        encoding="utf-8",
    )

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))

    assert (tmp_path / f"{tmp_path.name}_P.ppl").exists()
    assert (tmp_path / f"{tmp_path.name}_G.ppl").exists()
    assert not (tmp_path / f"{tmp_path.name}.ppl").exists()


def test_convert_reads_english_perfis(tmp_path):
    _write_perfis_en(tmp_path / "PERFISP-0.dat", [0, 60])

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))

    ppl_file = tmp_path / f"{tmp_path.name}.ppl"
    assert ppl_file.exists()


def test_convert_groups_tend_files_by_time_array(tmp_path):
    _write_tend(tmp_path / "TENDP-0.dat", [0, 60, 120], position="10.0")
    _write_tend(tmp_path / "TENDP-1.dat", [0, 60], position="20.0")  # amostragem diferente

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))

    assert (tmp_path / f"{tmp_path.name}.tpl").exists()  # maior grupo
    detailed = list(tmp_path.glob(f"{tmp_path.name}_P-*.tpl"))
    assert len(detailed) == 1


def test_convert_uses_olga_names_by_default(tmp_path):
    _write_tend(tmp_path / "TENDP-0.dat", [0, 60])

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))

    content = (tmp_path / f"{tmp_path.name}.tpl").read_text(encoding="utf-8")
    assert "PT " in content


def test_convert_can_disable_olga_names(tmp_path):
    _write_perfis_pt(tmp_path / "PERFISP-0.dat", [0, 60])

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path), use_olga_names=False)

    content = (tmp_path / f"{tmp_path.name}.ppl").read_text(encoding="utf-8")
    assert "PT " in content  # pressão/temperatura sempre viram PT/TM


def test_convert_raises_when_no_recognized_files(tmp_path):
    with pytest.raises(ValueError):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_convert_raises_on_truncated_tend_file(tmp_path):
    (tmp_path / "TENDP-0.dat").write_text("# only one line\n", encoding="utf-8")

    with pytest.raises(ValueError):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_convert_raises_on_missing_geometry_column(tmp_path):
    (tmp_path / "PERFISP-0.dat").write_text(
        "PERFISP\nTempo (s);Pressao (kgf/cm2) F\n0;100.0\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_convert_builds_fallback_geometry_without_perfis(tmp_path):
    _write_tend(tmp_path / "TENDP-0.dat", [0, 60], position="15.0")

    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))

    content = (tmp_path / f"{tmp_path.name}.tpl").read_text(encoding="utf-8")
    assert "BRANCH" in content
    assert "'PRODUCAO'" in content
