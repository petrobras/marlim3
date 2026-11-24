"""
Testes de integração e regressão — demos descritas em demos/manifest.json.

O manifesto é a única lista de modelos. Cada demo PT roda com o modelo gravado
em português e, quando o manifesto lista "en", também traduzido para inglês. Os
perfis das execuções devem coincidir entre si e com a referência em
demos/<caso>/reference/. A versão em inglês ("language": "en") de cada pasta é
conferida sem o motor: a API precisa gerar dela a mesma entrada que gera do
modelo em português (translation_problems).
Os modos PRODUCAO, INJETOR e REDE acionam o motor com o "-s" correspondente
(ver marlim3.testing).

Execução:
    pytest tools/tests/test_demos_steady_state.py -v -m simulacao
    python -m marlim3.testing demos/manifest.json   # atualiza as referências
"""

import json
from pathlib import Path

import pytest

from marlim3._download import executable_exists
from marlim3.testing import check_case, load_manifest, missing_files, roundtrip_problems, translation_problems

ROOT = Path(__file__).resolve().parents[2]
DEMOS_DIR = ROOT / "demos"
OUTPUT_DIR = ROOT / "saida_testes"
CASES = load_manifest(DEMOS_DIR / "manifest.json")
# Todas as pastas de demos, também as que não estão no manifesto.
MODELS = sorted(DEMOS_DIR.glob("*/*.mr3"))
PORTUGUESE_MODELS = [path for path in MODELS
                     if json.loads(path.read_text(encoding="utf-8")).get("language") != "en"]


def test_manifest_lists_existing_files():
    assert not missing_files(CASES, DEMOS_DIR)


@pytest.mark.parametrize("case", CASES, ids=[c.model for c in CASES])
def test_api_preserves_input_keys(case):
    problems = roundtrip_problems(case, DEMOS_DIR)
    assert not problems, f"{case.model}: {problems}"


def test_every_demo_folder_has_one_model_per_language():
    for folder in sorted({path.parent for path in MODELS}):
        portuguese = [p.name for p in PORTUGUESE_MODELS if p.parent == folder]
        english = [p.name for p in MODELS if p.parent == folder and p not in PORTUGUESE_MODELS]
        assert len(portuguese) == 1 and len(english) == 1, f"{folder.name}: PT {portuguese}, EN {english}"


@pytest.mark.parametrize("model", PORTUGUESE_MODELS, ids=[p.parent.name for p in PORTUGUESE_MODELS])
def test_english_version_matches_portuguese(model):
    problems = translation_problems(model)
    assert not problems, f"{model.relative_to(DEMOS_DIR)}: {problems}"


@pytest.fixture
def smooth_pipe_pair(tmp_path):
    """Cópia do par português/inglês de smoothPipeFriction; devolve (modelo PT, dados EN, gravar EN)."""
    for path in (DEMOS_DIR / "smoothPipeFriction").glob("*.mr3"):
        (tmp_path / path.name).write_bytes(path.read_bytes())
    english = tmp_path / "smoothPipeFriction.mr3"
    data = json.loads(english.read_text(encoding="utf-8"))

    def write(changed):
        english.write_text(json.dumps(changed, ensure_ascii=False), encoding="utf-8")

    return tmp_path / "atritoTuboLiso.mr3", data, write


def test_translation_ignores_labels(smooth_pipe_pair):
    model, english, write = smooth_pipe_pair
    english["material"][0]["label"] = "Steel"
    write(english)
    assert translation_problems(model) == []


def test_translation_detects_untranslated_key(smooth_pipe_pair):
    model, english, write = smooth_pipe_pair
    english["productionFluid"][0]["gasOilRatioTypo"] = english["productionFluid"][0].pop("gor")
    write(english)
    problems = translation_problems(model)
    assert any("lacks /fluidosProducao[0]/rgo" in p for p in problems), problems
    assert any("has extra /fluidosProducao[0]/gasOilRatioTypo" in p for p in problems), problems


def test_translation_detects_changed_value(smooth_pipe_pair):
    model, english, write = smooth_pipe_pair
    english["productionFluid"][0]["api"] = 31
    write(english)
    assert translation_problems(model) == ["smoothPipeFriction.mr3 differs at /fluidosProducao[0]/api: 30 -> 31"]


def test_translation_rejects_two_english_versions(smooth_pipe_pair):
    model, english, write = smooth_pipe_pair
    (model.parent / "copia.mr3").write_text(json.dumps(english), encoding="utf-8")
    assert translation_problems(model)[0].startswith("more than one English version")


def test_manifest_native_field_is_obsolete(tmp_path):
    manifest = tmp_path / "manifest.json"
    manifest.write_text(json.dumps([{"model": "a/a.mr3", "native": "a/b.mr3"}]), encoding="utf-8")
    with pytest.warns(DeprecationWarning, match="native"):
        cases = load_manifest(manifest)
    assert [c.model for c in cases] == ["a/a.mr3"]


@pytest.mark.skipif(not executable_exists(), reason="Executável Marlim3 não encontrado")
@pytest.mark.simulacao
@pytest.mark.parametrize("case", CASES, ids=[c.model for c in CASES])
def test_demo_simula(case):
    problems = check_case(case, DEMOS_DIR, OUTPUT_DIR)
    assert not problems, f"{case.model}: {problems}"
