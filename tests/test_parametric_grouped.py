"""Ordered grouped AP sweeps and the mandatory Cartesian VFP contract."""

import json
from pathlib import Path
import re

import pytest

from tests.test_vfp_tables import engine as engine, generic_rows, run_ap, TABLE_FILES


ROOT = Path(__file__).resolve().parents[1]
SCHEMA = json.loads(
    (ROOT / "docs" / "schemas" / "leituraAP.pt.json").read_text(encoding="utf-8")
)
pytestmark = pytest.mark.simulacao


def grouped_analysis(mode=1, rates=(3000, 1000, 2000), pressures=(25, 10, 30)):
    return {
        "tipoAP": 0,
        "vfp": mode,
        "combinacao": False,
        "imprimePerfil": False,
        "nthread": 2,
        "psep": {"pressao": list(pressures)},
        "FonteLiquido": [{"indiceFL": 0, "vazLiq": list(rates)}],
    }


def selected_values(directory, label):
    text = (directory / "relacaoAP.dat").read_text(encoding="utf-8", errors="replace")
    pattern = rf"^\s*(\d+)\s*:.* {re.escape(label)}=\s*([-\d.eE+]+)"
    matches = re.findall(pattern, text, re.MULTILINE)
    assert matches, text
    assert [int(index) for index, _ in matches] == list(range(len(matches)))
    return [float(value) for _, value in matches]


def assert_rejected(engine, directory, analysis, message):
    run_ap(
        engine, directory, analysis.get("vfp", 1),
        analysis_override=analysis, expected_error=message,
    )
    for name in (
        "relacaoAP.dat", "sucessoAP.dat", "variaveisInteresseAP.dat",
        "tabelaGenericaAP.dat", "tabelaGenericaTemperaturaAP.dat",
        *(name for pair in TABLE_FILES for name in pair),
    ):
        assert not (directory / name).exists(), name


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("parallel", [False, True], ids=["serial", "parallel"])
def test_grouped_preserves_exact_rows(engine, tmp_path, mode, parallel):
    analysis = grouped_analysis(mode)
    analysis["FonteLiquido"][0].update({
        "temperatura": [80, 70, 90],
        "fluido": [0, 0, 0],
    })
    run_ap(engine, tmp_path, mode, parallel=parallel, analysis_override=analysis)
    assert selected_values(tmp_path, "Pressao Separador") == [25, 10, 30]
    assert selected_values(tmp_path, "Vazao de Liquido") == [3000, 1000, 2000]
    assert selected_values(tmp_path, "Temperatura") == [80, 70, 90]
    assert selected_values(tmp_path, "Fluido") == [0, 0, 0]
    results = re.findall(
        r"^\s*(\d+)\s*:\s*Resultado = (\w+)",
        (tmp_path / "sucessoAP.dat").read_text(encoding="utf-8"), re.MULTILINE,
    )
    assert results == [("0", "sucesso"), ("1", "sucesso"), ("2", "sucesso")]
    assert not (tmp_path / "tabelaGenericaAP.dat").exists()


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("parallel", [False, True], ids=["serial", "parallel"])
@pytest.mark.parametrize("pressures", [(25,), (30, 10, 30)])
def test_grouped_single_series_and_single_case(engine, tmp_path, mode, parallel, pressures):
    analysis = {
        "combinacao": False, "vfp": mode, "imprimePerfil": False,
        "psep": {"pressao": list(pressures)},
    }
    run_ap(engine, tmp_path, mode, parallel=parallel, analysis_override=analysis)
    assert selected_values(tmp_path, "Pressao Separador") == list(pressures)


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("parallel", [False, True], ids=["serial", "parallel"])
def test_grouped_all_singleton_arrays(engine, tmp_path, mode, parallel):
    analysis = grouped_analysis(mode, rates=(1000,), pressures=(10,))
    analysis["FonteLiquido"][0]["temperatura"] = [80]
    run_ap(engine, tmp_path, mode, parallel=parallel, analysis_override=analysis)
    assert selected_values(tmp_path, "Pressao Separador") == [10]
    assert selected_values(tmp_path, "Vazao de Liquido") == [1000]


@pytest.mark.parametrize("mode", range(4))
def test_grouped_serial_parallel_agree(engine, tmp_path, mode):
    analysis = grouped_analysis(mode)
    analysis["imprimePerfil"] = True
    serial = run_ap(
        engine, tmp_path / "serial", mode, analysis_override=analysis,
    )
    parallel = run_ap(
        engine, tmp_path / "parallel", mode, parallel=True, analysis_override=analysis,
    )
    for label in ("Pressao Separador", "Vazao de Liquido"):
        assert selected_values(serial, label) == selected_values(parallel, label)
    for index in range(3):
        name = f"resumoPermanente-seqAP-{index}.dat"
        serial_rows = generic_rows(serial / name)
        parallel_rows = generic_rows(parallel / name)
        assert len(serial_rows) == len(parallel_rows) == 1
        assert parallel_rows[0] == pytest.approx(serial_rows[0], rel=1e-4)


@pytest.mark.parametrize("mode", range(4))
def test_cartesian_default_matches_explicit_true(engine, tmp_path, mode):
    analysis = grouped_analysis(mode, rates=(2000, 1000), pressures=(20, 10))
    analysis["imprimePerfil"] = True
    del analysis["combinacao"]
    default = run_ap(engine, tmp_path / "default", mode, analysis_override=analysis)
    analysis["combinacao"] = True
    explicit = run_ap(engine, tmp_path / "explicit", mode, analysis_override=analysis)
    for label in ("Pressao Separador", "Vazao de Liquido"):
        assert selected_values(default, label) == selected_values(explicit, label)
    assert len(selected_values(explicit, "Pressao Separador")) == 4
    for index in range(4):
        name = f"resumoPermanente-seqAP-{index}.dat"
        assert (default / name).read_bytes() == (explicit / name).read_bytes()


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("parallel", [False, True], ids=["serial", "parallel"])
def test_vfp_rejects_grouped(engine, tmp_path, mode, parallel):
    analysis = {"tipoAP": 1, "vfp": mode, "combinacao": False}
    run_ap(
        engine, tmp_path, mode, parallel=parallel, analysis_override=analysis,
        expected_error="VFP requer combinacao=true",
    )
    assert not (tmp_path / "relacaoAP.dat").exists()
    assert not any((tmp_path / name).exists() for pair in TABLE_FILES for name in pair)


@pytest.mark.parametrize("mode", range(4))
def test_vfp_explicit_true_matches_default(engine, tmp_path, mode):
    default = run_ap(engine, tmp_path / "default", mode)
    analysis = {
        "tipoAP": 1, "vfp": mode, "combinacao": True,
        "psep": {"pressao": [10, 20]},
        "RGO-fluido0": {"RGO": [0]},
        "BSW-fluido0": {"BSW": [0]},
        "FonteLiquido": [{"indiceFL": 0, "vazLiq": [1000, 2000, 3000]}],
    }
    explicit = run_ap(engine, tmp_path / "explicit", mode, analysis_override=analysis)
    for name in TABLE_FILES[mode]:
        assert (default / name).read_bytes() == (explicit / name).read_bytes()


@pytest.mark.parametrize("value", [0, 1, "false", None, [], {}])
def test_combinacao_requires_boolean(engine, tmp_path, value):
    analysis = grouped_analysis()
    analysis["combinacao"] = value
    assert_rejected(engine, tmp_path, analysis, "combinacao deve ser um booleano")


@pytest.mark.parametrize("pressures", [[], [25], [25, 10]])
def test_grouped_rejects_empty_or_mismatched_series(engine, tmp_path, pressures):
    analysis = grouped_analysis(pressures=pressures)
    message = "serie nao vazia" if not pressures else "mesmo comprimento"
    assert_rejected(engine, tmp_path, analysis, message)


def test_grouped_requires_active_series(engine, tmp_path):
    assert_rejected(
        engine, tmp_path, {"combinacao": False}, "ao menos uma serie",
    )


def sweep_fields():
    for section, schema in SCHEMA["properties"].items():
        # The shared fixture is a production model, not an injector well.
        if section == "pocoInjetor":
            continue
        multiple = schema["type"] == "array"
        details = schema["items"] if multiple else schema
        for field, field_schema in details.get("properties", {}).items():
            if field_schema["type"] == "array":
                yield section, field, multiple, details.get("required", [])


@pytest.mark.parametrize(
    "section,field,multiple,required", list(sweep_fields()),
    ids=[f"{section}.{field}" for section, field, *_ in sweep_fields()],
)
def test_every_sweep_field_validates_grouped_lengths(
    engine, tmp_path, section, field, multiple, required,
):
    target = {field: [1, 2]}
    if multiple:
        target.update({key: 0 for key in required if key.startswith("indice")})
    analysis = {"combinacao": False}
    if section == "psep":
        analysis["FonteLiquido"] = [{"indiceFL": 0, "vazLiq": [1000, 2000, 3000]}]
    else:
        analysis["psep"] = {"pressao": [10, 20, 30]}
    analysis[section] = [target] if multiple else target
    assert_rejected(engine, tmp_path, analysis, "mesmo comprimento")


def test_grouped_validates_multiple_component_instances(engine, tmp_path):
    analysis = grouped_analysis()
    analysis["IPR"] = [
        {"indiceIPR": 0, "presEstatica": [240, 250, 260]},
        {"indiceIPR": 1, "tempRes": [80, 90]},
    ]
    assert_rejected(engine, tmp_path, analysis, "mesmo comprimento")
    assert "#/IPR/1/tempRes" in (tmp_path / "simulacao.log").read_text(encoding="utf-8")


def test_grouped_duplicate_rows_are_not_deduplicated(engine, tmp_path):
    analysis = grouped_analysis(rates=(2000, 1000, 2000), pressures=(25, 10, 25))
    run_ap(engine, tmp_path, 1, analysis_override=analysis)
    assert selected_values(tmp_path, "Pressao Separador") == [25, 10, 25]
    assert selected_values(tmp_path, "Vazao de Liquido") == [2000, 1000, 2000]


@pytest.mark.parametrize("mode", range(4))
def test_grouped_never_computes_cartesian_product(engine, tmp_path, mode):
    count = 10
    analysis = grouped_analysis(mode, rates=[1000] * count, pressures=[10] * count)
    analysis["FonteLiquido"][0].update({
        "temperatura": [80] * count, "beta": [0] * count, "fluido": [0] * count,
    })
    analysis.update({
        "RGO-fluido0": {"RGO": [0] * count},
        "BSW-fluido0": {"BSW": [0] * count},
        "Condutividade": [{"indiceMat": 0, "condutividade": [58] * count}],
        "dPdLHidro": [{"indicedPdLHidro": 0, "fator": [1] * count}],
        "dPdLFric": [{"indicedPdLFric": 0, "fator": [1] * count}],
    })
    assert count ** 10 > 2 ** 31 - 1
    base_model = json.loads(
        (ROOT / "demos" / "smoothPipeFriction" / "atritoTuboLiso.mr3").read_text(encoding="utf-8")
    )
    model_override = {
        "correcao": {
            "ativo": True,
            "dPdLHidro": [1.0],
            "dPdLFric": [1.0],
        },
        "dutosProducao": [
            {**duct, "dPdLHidro": 0, "dPdLFric": 0}
            for duct in base_model["dutosProducao"]
        ],
    }
    run_ap(
        engine, tmp_path, mode, analysis_override=analysis,
        model_override=model_override,
    )
    assert selected_values(tmp_path, "Pressao Separador") == [10] * count
    results = re.findall(
        r"^\s*(\d+)\s*:\s*Resultado = (\w+)",
        (tmp_path / "sucessoAP.dat").read_text(encoding="utf-8"), re.MULTILINE,
    )
    assert results == [(str(index), "sucesso") for index in range(count)]
