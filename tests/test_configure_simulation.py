"""
Testes de integração — execução real de simulações do Marlim3.

Baseados no notebook comparacao_horizontal_vertical.ipynb.
Requerem o executável Marlim3 compilado e disponível no pacote.

Execução:
    pytest tests/test_simulacao.py -v -m simulacao
"""

import copy
import importlib.resources as pkg_resources
import os
import platform

import numpy as np
import pandas as pd
import pytest

import marlim3


# ============================================================================
# Verificação do executável
# ============================================================================

def _executavel_disponivel():
    exe_name = "marlim3.exe" if platform.system() == "Windows" else "marlim3"
    for name in [exe_name, "Marlim3.exe" if platform.system() == "Windows" else "Marlim3"]:
        try:
            with pkg_resources.path("marlim3", name) as p:
                if p.exists():
                    return True
        except (FileNotFoundError, AttributeError, TypeError):
            continue
    return False


skip_sem_executavel = pytest.mark.skipif(
    not _executavel_disponivel(),
    reason="Executável Marlim3 não encontrado no pacote",
)


# ============================================================================
# Fixtures
# ============================================================================

@pytest.fixture
def caso_base_horizontal():
    caso = marlim3.Tramo()
    caso.fluidosProducao = [{
        "id": 0, "api": 32, "rgo": 100, "densidadeGas": 0.7, "bsw": 0.0,
    }]
    caso.material = [{
        "id": 0, "tipo": 0, "condutividade": 58, "calorEspecifico": 480, "rho": 7850,
    }]
    caso.secaoTransversal = [{
        "id": 0,
        "diametroInterno": 10 * 0.0254,
        "rugosidade": 0.183e-3,
        "camadas": [{"idMaterial": 0, "tipoMedicaoCamada": "ESPESSURA", "espessura": 0.0254}],
    }]
    caso.dutosProducao = [{
        "id": 0, "idCorte": 0, "ambienteExterno": 2, "angulo": 0,
        "discretizacao": [{"nCelulas": 20, "comprimento": 125.0}],
        "condicoesIniciais": {"compInter": [0, 1], "tempExterna": [40, 20], "velExterna": [0.5, 0.5]},
    }]
    caso.fonteLiquido = [{
        "id": 0, "indiFluidoPro": 0, "comprimentoMedido": 0.1,
        "tempo": [0], "vazaoLiquido": [1500], "temperatura": [40],
    }]
    caso.separador = {"tempo": [0], "pressao": [2]}
    caso.perfilProducao = {
        "tempo": [0], "pressao": True, "temperatura": True,
        "holdup": True, "arra": True, "fric": True, "hidro": True,
    }
    return caso


@pytest.fixture
def caso_base_vertical(caso_base_horizontal):
    caso = copy.deepcopy(caso_base_horizontal)
    caso.dutosProducao[0]["angulo"] = np.pi / 2
    return caso


def _simular(caso, label, tmp_path):
    """Helper: executa simulação em diretório temporário."""
    original_cwd = os.getcwd()
    try:
        os.chdir(str(tmp_path))
        caso.simular(label=label, diretorio="resultados")
    finally:
        os.chdir(original_cwd)


# ============================================================================
# Testes
# ============================================================================

@skip_sem_executavel
@pytest.mark.simulacao
class TestSimulacaoHorizontal:

    def test_simulacao_horizontal_roda(self, caso_base_horizontal, tmp_path):
        _simular(caso_base_horizontal, "horizontal", tmp_path)
        assert "perfilProducao" in caso_base_horizontal.resultados
        df = caso_base_horizontal.resultados["perfilProducao"]
        assert isinstance(df, pd.DataFrame)
        assert not df.empty


@skip_sem_executavel
@pytest.mark.simulacao
class TestSimulacaoVertical:

    def test_simulacao_vertical_roda(self, caso_base_vertical, tmp_path):
        _simular(caso_base_vertical, "vertical", tmp_path)
        assert "perfilProducao" in caso_base_vertical.resultados
        df = caso_base_vertical.resultados["perfilProducao"]
        assert isinstance(df, pd.DataFrame)
        assert not df.empty