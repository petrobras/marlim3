"""
Testes de integração — execução real de simulações do Marlim3.

Baseados no notebook comparacao_horizontal_vertical.ipynb.
Requerem o executável Marlim3 compilado e disponível no pacote.

Execução:
    pytest tests/test_simulacao.py -v -m simulacao
"""

import copy
import os

import numpy as np
import pandas as pd
import pytest

import marlim3
from marlim3._download import executable_exists


# ============================================================================
# Verificação do executável
# ============================================================================

def _executavel_disponivel():
    return executable_exists()


skip_sem_executavel = pytest.mark.skipif(
    not _executavel_disponivel(),
    reason="Executável Marlim3 não encontrado",
)


# ============================================================================
# Fixtures
# ============================================================================

@pytest.fixture
def caso_base_horizontal():
    caso = marlim3.Branch()
    caso.productionFluid = [{
        "id": 0, "api": 32, "gor": 100, "gasDensity": 0.7, "bsw": 0.0,
    }]
    caso.material = [{
        "id": 0, "type": 0, "conductivity": 58, "specificHeat": 480, "rho": 7850,
    }]
    caso.crossSection = [{
        "id": 0,
        "innerDiameter": 10 * 0.0254,
        "roughness": 0.183e-3,
        "layers": [{"materialId": 0, "layerMeasurementType": "THICKNESS", "thickness": 0.0254}],
    }]
    caso.productionPipe = [{
        "id": 0, "crossSectionId": 0, "environment": 2, "angle": 0,
        "discretization": [{"numCells": 20, "length": 125.0}],
        "initialAndAmbientConditions": {"measuredPosition": [0, 1], "ambientTemp": [40, 20], "ambientVel": [0.5, 0.5]},
    }]
    caso.liquidSource = [{
        "id": 0, "prodFluidId": 0, "measuredLength": 0.1,
        "time": [0], "liquidFlowRate": [1500], "temperature": [40],
    }]
    caso.separator = {"time": [0], "pressure": [2]}
    caso.productionProfile = {
        "time": [0], "pressure": True, "temperature": True,
        "holdup": True, "flowPattern": True, "frictionPressureGradient": True, "hydrostaticPressureGradient": True,
    }
    return caso


@pytest.fixture
def caso_base_vertical(caso_base_horizontal):
    caso = copy.deepcopy(caso_base_horizontal)
    caso.productionPipe[0]["angle"] = np.pi / 2
    return caso


def _simular(caso, label, tmp_path):
    """Helper: executa simulação em diretório timerário."""
    original_cwd = os.getcwd()
    try:
        os.chdir(str(tmp_path))
        caso.simulate(label=label, directory="resultados")
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
        assert "productionProfile" in caso_base_horizontal.resultados
        df = caso_base_horizontal.resultados["productionProfile"]
        assert isinstance(df, pd.DataFrame)
        assert not df.empty


@skip_sem_executavel
@pytest.mark.simulacao
class TestSimulacaoVertical:

    def test_simulacao_vertical_roda(self, caso_base_vertical, tmp_path):
        _simular(caso_base_vertical, "vertical", tmp_path)
        assert "productionProfile" in caso_base_vertical.resultados
        df = caso_base_vertical.resultados["productionProfile"]
        assert isinstance(df, pd.DataFrame)
        assert not df.empty
