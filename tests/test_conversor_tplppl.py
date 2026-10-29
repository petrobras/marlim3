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
    saved_modules = {
        name: sys.modules.get(name)
        for name in (
            "marlim3",
            "marlim3._output_headers",
            "marlim3._conversores",
            "marlim3._conversores._conversor_marlim3_tplppl",
        )
    }
    package = types.ModuleType("marlim3")
    package.__version__ = "test"
    package.__path__ = [str(REPO_ROOT / "marlim3")]
    try:
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
    finally:
        for name, previous in saved_modules.items():
            if previous is None:
                sys.modules.pop(name, None)
            else:
                sys.modules[name] = previous


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


def _write_ap_trend(path, rows, position="10", english=False, label="PDG"):
    sequence, time, pressure = (("SA sequence", "Time (s)", "Pressure (kgf/cm2)")
                                if english else ("Sequencia AP", "Tempo (s)", "Pressao (kgf/cm2)"))
    header = (
        f"# Comprimento a partir do Fundo de Poco (m) = {position}\n"
        f"# Rotulo = {label}\n# Indice da Celula = 1\n"
        f"{sequence};{time};{pressure};Temperatura (C);Viscosidade do Liquido (cP);"
        "RGO (Sm3/Sm3);Vazao volumetrica standard de oleo morto + agua (Sm3/d);"
        "Vazao volumetrica standard de oleo morto + agua + liquido complementar (Sm3/d);"
        "Vazao volumetrica standard de gas livre + dissolvido (Sm3/d);\n"
    )
    path.write_text(header + "".join(
        f"{ap};{t};{p};80;0.4;537;1409;1409;680979;\n" for ap, t, p in rows
    ), encoding="utf-8")


def _write_ap_profile(path, times=(0,), pressure=100, length=20, service=False, english=False):
    time = "Time (s)" if english else "Tempo (s)"
    boundary = "Length (m) Boundary F" if english else "Comprimento (m) Fronteira F"
    center = "Length (m) Cell center C" if english else "Comprimento (m) Centro Volume C"
    elevation = ("Depth" if service else "Elevation") if english else ("Profundidade" if service else "Elevacao")
    pt = "Pressure" if english else "Pressao"
    header = f"{time};{boundary};{center};{elevation} (m) F;{elevation} (m) C;{pt} (kgf/cm2) C;RGO (Sm3/Sm3) F;\n"
    path.write_text("# Passo de tempo = 0\n" + header + "".join(
        f"{t};0;{length / 4};0;-2.5;{pressure + float(t)};537;\n"
        f"{t};{length / 2};{3 * length / 4};-5;-7.5;{pressure - 1 + float(t)};538;\n"
        for t in times
    ), encoding="utf-8")


def _read_plot(path):
    lines = [line.strip() for line in path.read_text(encoding="utf-8").splitlines()]
    catalog_start = lines.index("CATALOG")
    count = int(lines[catalog_start + 1])
    catalog = lines[catalog_start + 2:catalog_start + 2 + count]
    series_start = lines.index("TIME SERIES ' (S) '")
    series = [[float(value) for value in line.split()] for line in lines[series_start + 1:]]
    branches = {}
    for index, line in enumerate(lines):
        if line != "BRANCH":
            continue
        name = lines[index + 1].strip("'")
        cells = int(lines[index + 2])
        values = []
        cursor = index + 3
        while len(values) < 2 * (cells + 1):
            values.extend(float(value) for value in lines[cursor].split())
            cursor += 1
        assert len(values) == 2 * (cells + 1)
        branches[name] = (cells, values[:cells + 1], values[cells + 1:])
    assert int(lines[lines.index("NETWORK") + 1]) == len(branches)
    return catalog, series, branches


@pytest.mark.parametrize("english", [False, True])
@pytest.mark.parametrize("olga", [False, True])
def test_ap_trends_keep_all_six_time_zero_cases(tmp_path, english, olga):
    _write_ap_trend(tmp_path / "TENDP-AP-20.dat", [(ap, 0, 527 + ap) for ap in range(6)],
                    english=english)
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path), use_olga_names=olga)
    assert len(list(tmp_path.glob("*.tpl"))) == 6
    for ap in range(6):
        catalog, series, _ = _read_plot(tmp_path / f"{tmp_path.name}_AP-{ap}.tpl")
        assert len(catalog) == 7
        assert catalog[0].startswith("PT ")
        assert all("sequence" not in entry and "Sequencia" not in entry for entry in catalog)
        assert series == [[0, 527 + ap, 80, 0.4, 537, 1409, 1409, 680979]]


@pytest.mark.parametrize("english", [False, True])
@pytest.mark.parametrize("olga", [False, True])
def test_ap_profiles_consolidate_times_but_never_cases(tmp_path, english, olga):
    for ap in (0, 3):
        for time in (0, 60):
            _write_ap_profile(tmp_path / f"PERFISP-seqAP-{ap}-{time}.dat",
                              times=(time,), pressure=100 + ap, length=20 + ap, english=english)
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path), use_olga_names=olga)
    assert len(list(tmp_path.glob("*.ppl"))) == 2
    for ap in (0, 3):
        catalog, series, geometry = _read_plot(tmp_path / f"{tmp_path.name}_AP-{ap}.ppl")
        assert len(catalog) == 2
        assert series == [[0], [100 + ap, 99 + ap], [537, 538, 538],
                          [60], [160 + ap, 159 + ap], [537, 538, 538]]
        assert geometry["PRODUCAO"] == (2, [0, (20 + ap) / 2, 20 + ap], [0, -5, -10])


@pytest.mark.parametrize("same_times", [False, True])
def test_ap_profiles_consolidate_compatible_branches(tmp_path, same_times):
    _write_ap_profile(tmp_path / "PERFISP-seqAP-0-0.dat", times=(0, 60))
    _write_ap_profile(tmp_path / "PERFISG-seqAP-0-0.dat", times=(0, 60 if same_times else 120),
                      service=True)
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    name = f"{tmp_path.name}_AP-0"
    if same_times:
        catalog, series, geometry = _read_plot(tmp_path / f"{name}.ppl")
        assert len(catalog) == 4
        assert set(geometry) == {"PRODUCAO", "SERVICO"}
        assert len(series) == 10
    else:
        for branch, source in (("P", "PRODUCAO"), ("G", "SERVICO")):
            catalog, series, geometry = _read_plot(tmp_path / f"{name}_{branch}.ppl")
            assert len(catalog) == 2
            assert set(geometry) == {source}
            assert len(series) == 6


def test_ap_trends_group_selected_case_rows_and_distinct_monitors(tmp_path):
    _write_ap_trend(tmp_path / "TENDP-AP-20.dat",
                    [(3, 60, 360), (0, 0, 100), (3, 0, 300), (0, 60, 160)], position="20")
    _write_ap_trend(tmp_path / "TENDP-AP-30.dat",
                    [(0, "0.0", 200), (0, 60, 260)], position="30")
    _write_ap_trend(tmp_path / "TENDG-AP-20.dat",
                    [(0, 0, 400), (0, 60, 460)], position="20")
    # AP 3 occurs only in the first file; AP 0 has three compatible monitors.
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    catalog, series, geometry = _read_plot(tmp_path / f"{tmp_path.name}_AP-0.tpl")
    assert len(catalog) == 21
    assert set(geometry) == {"PRODUCAO", "SERVICO"}
    assert sorted(series[0][1::7]) == [100, 200, 400]
    assert sorted(series[1][1::7]) == [160, 260, 460]
    _, series, _ = _read_plot(tmp_path / f"{tmp_path.name}_AP-3.tpl")
    assert [row[:2] for row in series] == [[0, 300], [60, 360]]


def test_ap_trends_split_different_time_arrays_with_real_monitor_tags(tmp_path):
    for position, times in ((20, (0, 60)), (30, (0, 60)), (40, (0, 120))):
        _write_ap_trend(tmp_path / f"TENDP-AP-{position}.dat",
                        [(0, time, position + time) for time in times], position=str(position))
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    primary = tmp_path / f"{tmp_path.name}_AP-0.tpl"
    extra = tmp_path / f"{tmp_path.name}_AP-0_P-40.tpl"
    assert len(list(tmp_path.glob("*.tpl"))) == 2
    assert len(_read_plot(primary)[0]) == 14
    assert [row[:2] for row in _read_plot(extra)[1]] == [[0, 40], [120, 160]]


def test_ap_mixed_inputs_use_own_geometry_and_preserve_ordinary_names(tmp_path):
    _write_perfis_pt(tmp_path / "PERFISP-0.dat", [0])
    _write_tend(tmp_path / "TENDP-0.dat", [0], position="99")
    _write_ap_profile(tmp_path / "PERFISP-seqAP-0-0.dat", length=40)
    _write_ap_trend(tmp_path / "TENDP-AP-20.dat", [(0, 0, 200), (1, 0, 300)], position="20")
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    assert (tmp_path / f"{tmp_path.name}.ppl").exists()
    assert (tmp_path / f"{tmp_path.name}.tpl").exists()
    for extension in ("tpl", "ppl"):
        assert _read_plot(tmp_path / f"{tmp_path.name}_AP-0.{extension}")[2]["PRODUCAO"][1][-1] == 40
    assert _read_plot(tmp_path / f"{tmp_path.name}_AP-1.tpl")[2]["PRODUCAO"][1] == [0, 20]


def test_ap_service_only_profile_and_trend(tmp_path):
    _write_ap_profile(tmp_path / "PERFISG-seqAP-2-0.dat", service=True)
    _write_ap_trend(tmp_path / "TENDG-AP-20.dat", [(2, 0, 200)])
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    for suffix in ("_G.ppl", ".tpl"):
        _, _, geometry = _read_plot(tmp_path / f"{tmp_path.name}_AP-2{suffix}")
        assert set(geometry) == {"SERVICO"}


@pytest.mark.parametrize("value", ["-1", "1.5", "nan", "inf", "wrong"])
def test_ap_rejects_invalid_sequence(tmp_path, value):
    _write_ap_trend(tmp_path / "TENDP-AP-20.dat", [(value, 0, 100)])
    with pytest.raises(conversor.ConversionError, match="Invalid AP sequence"):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    assert conversor.read_tend_file.cache_info().currsize == 0


@pytest.mark.parametrize("name", ["PERFISP-seqAP-x-0.dat", "PERFISG-seqAP--1-0.dat", "TENDP-AP-x.dat"])
def test_ap_rejects_malformed_filename(tmp_path, name):
    (tmp_path / name).write_text("unused", encoding="utf-8")
    with pytest.raises(conversor.ConversionError, match="Malformed AP output filename"):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


@pytest.mark.parametrize("old,new,match", [
    ("Sequencia AP", "Case", "requires exactly one AP column"),
    ("Tempo (s)", "Clock", "missing required column"),
    ("0;0;100;", "0;0;", "columns; expected"),
    ("0;0;100;", "0;nan;100;", "Non-finite time"),
    ("0;0;100;", "0;0;bad;", "is not a number"),
    ("# Indice da Celula = 1", "# Indice da Celula = missing", "Invalid trend headers"),
])
def test_ap_rejects_malformed_trend_data(tmp_path, old, new, match):
    path = tmp_path / "TENDP-AP-20.dat"
    _write_ap_trend(path, [(0, 0, 100)])
    path.write_text(path.read_text(encoding="utf-8").replace(old, new), encoding="utf-8")
    with pytest.raises(conversor.ConversionError, match=match):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_ap_rejects_duplicate_times_within_one_monitor(tmp_path):
    _write_ap_trend(tmp_path / "TENDP-AP-20.dat", [(0, 0, 100), (0, "0.0", 200)])
    with pytest.raises(conversor.ConversionError, match="Duplicate time"):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_ap_rejects_incompatible_profile_geometry(tmp_path):
    _write_ap_profile(tmp_path / "PERFISP-seqAP-0-0.dat", length=20)
    _write_ap_profile(tmp_path / "PERFISP-seqAP-0-60.dat", times=(60,), length=40)
    with pytest.raises(conversor.ConversionError, match="Incompatible AP profile geometry"):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    assert conversor.read_perfis_file.cache_info().currsize == 0


@pytest.mark.parametrize("old,new,match", [
    (";100.0;", ";bad;", "is not a number"),
    (";100.0;", ";inf;", "Non-finite values"),
    (";100.0;", ";", "columns; expected"),
    ("Comprimento (m) Centro Volume C", "Unknown", "missing required column"),
    (";10.0;15.0;", ";0;15.0;", "Non-increasing AP profile boundaries"),
])
def test_ap_rejects_malformed_profiles(tmp_path, old, new, match):
    path = tmp_path / "PERFISP-seqAP-0-0.dat"
    _write_ap_profile(path)
    original = path.read_text(encoding="utf-8")
    assert old in original
    path.write_text(original.replace(old, new), encoding="utf-8")
    with pytest.raises(conversor.ConversionError, match=match):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_ap_rejects_duplicate_profile_times_and_inconsistent_columns(tmp_path):
    first = tmp_path / "PERFISP-seqAP-0-0.dat"
    second = tmp_path / "PERFISP-seqAP-0-60.dat"
    _write_ap_profile(first)
    _write_ap_profile(second)
    with pytest.raises(conversor.ConversionError, match="Duplicate AP profile time"):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    _write_ap_profile(second, times=(60,))
    second.write_text(second.read_text(encoding="utf-8").replace("RGO", "Other"), encoding="utf-8")
    with pytest.raises(conversor.ConversionError, match="Incompatible AP profile columns"):
        conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))


def test_ap_service_trend_only_and_cached_reads(tmp_path, monkeypatch):
    path = tmp_path / "TENDG-AP-20.dat"
    _write_ap_trend(path, [(0, 0, 100), (1, 0, 200)])
    original_reader = conversor._read_dat_lines
    reads = []

    def tracked_reader(file_path, min_lines):
        reads.append(file_path)
        return original_reader(file_path, min_lines)

    monkeypatch.setattr(conversor, "_read_dat_lines", tracked_reader)
    conversor.convert_to_ppl_tpl(str(tmp_path), str(tmp_path))
    assert reads == [str(path)]
    for ap in (0, 1):
        _, series, geometry = _read_plot(tmp_path / f"{tmp_path.name}_AP-{ap}.tpl")
        assert set(geometry) == {"SERVICO"}
        assert series[0][:2] == [0, 100 + 100 * ap]
