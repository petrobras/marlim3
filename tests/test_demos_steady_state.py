"""
Testes de integração e regressão — demos descritas em demos/manifest.json.

O manifesto é a única lista de modelos. Cada demo PT roda com o modelo gravado
em português e, quando o manifesto lista "en", também traduzido para inglês; o
modelo "native" (escrito à mão em inglês) também roda. Os perfis das execuções
devem coincidir entre si e com a referência em demos/<caso>/reference/.
Os modos PRODUCAO, INJETOR e REDE acionam o motor com o "-s" correspondente
(ver marlim3.testing).

Execução:
    pytest tests/test_demos_steady_state.py -v -m simulacao
    python -m marlim3.testing demos/manifest.json   # atualiza as referências
"""

from pathlib import Path

import pytest

from marlim3._download import executable_exists
from marlim3.testing import check_case, load_manifest, missing_files, roundtrip_problems

ROOT = Path(__file__).resolve().parents[1]
DEMOS_DIR = ROOT / "demos"
OUTPUT_DIR = ROOT / "saida_testes"
CASES = load_manifest(DEMOS_DIR / "manifest.json")


def test_manifest_lists_existing_files():
    assert not missing_files(CASES, DEMOS_DIR)


@pytest.mark.parametrize("case", CASES, ids=[c.model for c in CASES])
def test_api_preserves_input_keys(case):
    problems = roundtrip_problems(case, DEMOS_DIR)
    assert not problems, f"{case.model}: {problems}"


@pytest.mark.skipif(not executable_exists(), reason="Executável Marlim3 não encontrado")
@pytest.mark.simulacao
@pytest.mark.parametrize("case", CASES, ids=[c.model for c in CASES])
def test_demo_simula(case):
    problems = check_case(case, DEMOS_DIR, OUTPUT_DIR)
    assert not problems, f"{case.model}: {problems}"
