"""
Testes unitários do pacote marlim3.

Baseados no tutorial comparacao_horizontal_vertical.ipynb,
verificam a montagem de modelos (Tramo), serialização JSON e
a classe de cenários (Cenarios).
"""

import copy
import json
import os
import tempfile

import numpy as np
import pandas as pd
import pytest

import marlim3


# ============================================================================
# Fixtures
# ============================================================================

@pytest.fixture
def fluido():
    """Fluido black-oil padrão."""
    return {
        "id": 0,
        "api": 32,
        "rgo": 100,
        "densidadeGas": 0.7,
        "bsw": 0.0,
    }


@pytest.fixture
def material_aco():
    """Material aço."""
    return {
        "id": 0,
        "tipo": 0,
        "condutividade": 58,
        "calorEspecifico": 480,
        "rho": 7850,
    }


@pytest.fixture
def secao_transversal():
    """Seção transversal com uma camada de aço."""
    camada = {
        "idMaterial": 0,
        "tipoMedicaoCamada": "ESPESSURA",
        "espessura": 0.0254,
    }
    return {
        "id": 0,
        "diametroInterno": 10 * 0.0254,
        "rugosidade": 0.183e-3,
        "camadas": [camada],
    }


@pytest.fixture
def duto_horizontal():
    """Duto horizontal de 2500 m, 20 células."""
    ncel = 20
    comprimento_total = 2500
    linha = {
        "nCelulas": ncel,
        "comprimento": comprimento_total / ncel,
    }
    condicoes_ambiente = {
        "compInter": [0, 1],
        "tempExterna": [40, 20],
        "velExterna": [0.5, 0.5],
    }
    return {
        "id": 0,
        "idCorte": 0,
        "ambienteExterno": 2,
        "angulo": 0,
        "discretizacao": [linha],
        "condicoesIniciais": condicoes_ambiente,
    }


@pytest.fixture
def fonte_liquido():
    """Fonte de líquido a montante – 1500 sm3/d."""
    return {
        "id": 0,
        "indiFluidoPro": 0,
        "comprimentoMedido": 0.1,
        "tempo": [0],
        "vazaoLiquido": [1500],
        "temperatura": [40],
    }


@pytest.fixture
def separador():
    """Condição de contorno de pressão a jusante."""
    return {
        "tempo": [0],
        "pressao": [2],
    }


@pytest.fixture
def vars_saida():
    """Variáveis de saída para perfis."""
    nomes = ["pressao", "temperatura", "holdup", "arra", "fric", "hidro"]
    return {"tempo": [0]} | {var: True for var in nomes}


@pytest.fixture
def caso_horizontal(fluido, material_aco, secao_transversal,
                    duto_horizontal, fonte_liquido, separador, vars_saida):
    """Caso base horizontal completo (sem simulação)."""
    caso = marlim3.Tramo()
    caso.fluidosProducao = [fluido]
    caso.material = [material_aco]
    caso.secaoTransversal = [secao_transversal]
    caso.dutosProducao = [duto_horizontal]
    caso.fonteLiquido = [fonte_liquido]
    caso.separador = separador
    caso.perfilProducao = vars_saida
    return caso


@pytest.fixture
def caso_vertical(caso_horizontal):
    """Caso base vertical (cópia do horizontal com ângulo π/2)."""
    caso = copy.deepcopy(caso_horizontal)
    caso.dutosProducao[0]["angulo"] = np.pi / 2
    return caso


# ============================================================================
# Testes de construção do Tramo
# ============================================================================

class TestTramoConstruction:
    """Testes de instanciação e atributos do Tramo."""

    def test_tramo_default(self):
        """Tramo vazio deve ter atributos padrão."""
        t = marlim3.Tramo()
        assert t.sistema == "MULTIFASICO"
        assert t.versao == "1.0"
        assert t.versaoJson == "1.3.9"
        assert t.fluidosProducao == []
        assert t.material == []
        assert t.secaoTransversal == []
        assert t.dutosProducao == []
        assert t.fonteLiquido == []
        assert t.separador == {}
        assert t.perfilProducao == {}
        assert isinstance(t.resultados, dict)

    def test_tramo_default_lists_are_independent(self):
        """Listas padrão não devem ser compartilhadas entre instâncias."""
        t1 = marlim3.Tramo()
        t2 = marlim3.Tramo()
        t1.fluidosProducao.append({"test": 1})
        assert t2.fluidosProducao == []

    def test_tramo_with_kwargs(self, fluido, material_aco):
        """Tramo pode ser criado passando kwargs no construtor."""
        t = marlim3.Tramo(
            fluidosProducao=[fluido],
            material=[material_aco],
        )
        assert len(t.fluidosProducao) == 1
        assert t.fluidosProducao[0]["api"] == 32
        assert len(t.material) == 1
        assert t.material[0]["condutividade"] == 58

    def test_sistema_injetor(self):
        """Tramo pode ser criado com sistema INJETOR."""
        t = marlim3.Tramo(sistema="INJETOR")
        assert t.sistema == "INJETOR"


# ============================================================================
# Testes de montagem do modelo (padrão do notebook)
# ============================================================================

class TestModelAssembly:
    """Testes que reproduzem a montagem do modelo do notebook."""

    def test_fluido_assignment(self, caso_horizontal, fluido):
        assert caso_horizontal.fluidosProducao == [fluido]
        assert caso_horizontal.fluidosProducao[0]["api"] == 32
        assert caso_horizontal.fluidosProducao[0]["rgo"] == 100
        assert caso_horizontal.fluidosProducao[0]["densidadeGas"] == 0.7
        assert caso_horizontal.fluidosProducao[0]["bsw"] == 0.0

    def test_material_assignment(self, caso_horizontal, material_aco):
        assert caso_horizontal.material == [material_aco]

    def test_secao_transversal(self, caso_horizontal):
        sec = caso_horizontal.secaoTransversal[0]
        assert sec["diametroInterno"] == pytest.approx(10 * 0.0254)
        assert sec["rugosidade"] == pytest.approx(0.183e-3)
        assert len(sec["camadas"]) == 1
        assert sec["camadas"][0]["espessura"] == pytest.approx(0.0254)

    def test_duto_horizontal(self, caso_horizontal):
        duto = caso_horizontal.dutosProducao[0]
        assert duto["angulo"] == 0
        assert duto["ambienteExterno"] == 2
        assert duto["discretizacao"][0]["nCelulas"] == 20
        assert duto["discretizacao"][0]["comprimento"] == pytest.approx(125.0)

    def test_condicoes_ambiente(self, caso_horizontal):
        cond = caso_horizontal.dutosProducao[0]["condicoesIniciais"]
        assert cond["compInter"] == [0, 1]
        assert cond["tempExterna"] == [40, 20]
        assert cond["velExterna"] == [0.5, 0.5]

    def test_fonte_liquido(self, caso_horizontal):
        fl = caso_horizontal.fonteLiquido[0]
        assert fl["vazaoLiquido"] == [1500]
        assert fl["temperatura"] == [40]
        assert fl["comprimentoMedido"] == pytest.approx(0.1)

    def test_separador(self, caso_horizontal):
        assert caso_horizontal.separador["pressao"] == [2]

    def test_perfil_producao_vars(self, caso_horizontal):
        pp = caso_horizontal.perfilProducao
        assert pp["tempo"] == [0]
        for var in ["pressao", "temperatura", "holdup", "arra", "fric", "hidro"]:
            assert pp[var] is True

    def test_comprimento_total(self, caso_horizontal):
        """Comprimento total = nCelulas * comprimento_celula."""
        duto = caso_horizontal.dutosProducao[0]
        disc = duto["discretizacao"][0]
        total = disc["nCelulas"] * disc["comprimento"]
        assert total == pytest.approx(2500.0)


# ============================================================================
# Testes de criação do caso vertical (deepcopy + alteração)
# ============================================================================

class TestVerticalCase:
    """Testes da criação do caso vertical a partir do horizontal."""

    def test_angulo_vertical(self, caso_vertical):
        assert caso_vertical.dutosProducao[0]["angulo"] == pytest.approx(np.pi / 2)

    def test_deepcopy_independence(self, caso_horizontal, caso_vertical):
        """Alterar o caso vertical não altera o horizontal."""
        assert caso_horizontal.dutosProducao[0]["angulo"] == 0
        assert caso_vertical.dutosProducao[0]["angulo"] == pytest.approx(np.pi / 2)

    def test_vertical_preserves_other_fields(self, caso_horizontal, caso_vertical):
        """O deepcopy mantém os demais campos intactos."""
        assert caso_vertical.fluidosProducao == caso_horizontal.fluidosProducao
        assert caso_vertical.material == caso_horizontal.material
        assert caso_vertical.separador == caso_horizontal.separador


# ============================================================================
# Testes de variação de parâmetros (padrão do notebook – loop de vazões)
# ============================================================================

class TestParameterVariation:
    """Testes para variação de parâmetros como no notebook."""

    def test_variacao_vazao(self, caso_horizontal):
        vazoes = [200, 2000, 4000, 6000]
        casos = []
        for vazao in vazoes:
            c = copy.deepcopy(caso_horizontal)
            c.fonteLiquido[0]["vazaoLiquido"] = [vazao]
            casos.append(c)

        for i, vazao in enumerate(vazoes):
            assert casos[i].fonteLiquido[0]["vazaoLiquido"] == [vazao]

        # Caso original não foi alterado
        assert caso_horizontal.fonteLiquido[0]["vazaoLiquido"] == [1500]

    def test_variacao_diametro(self, caso_horizontal):
        diametros_pol = [5, 10, 15, 20]
        casos = []
        for d in diametros_pol:
            c = copy.deepcopy(caso_horizontal)
            c.secaoTransversal[0]["diametroInterno"] = d * 0.0254
            casos.append(c)

        for i, d in enumerate(diametros_pol):
            assert casos[i].secaoTransversal[0]["diametroInterno"] == pytest.approx(
                d * 0.0254
            )

    def test_variacao_rgo(self, caso_horizontal):
        rgos = [0, 50, 200, 500, 1000, 2000]
        casos = []
        for rgo in rgos:
            c = copy.deepcopy(caso_horizontal)
            c.fluidosProducao[0]["rgo"] = rgo
            casos.append(c)

        for i, rgo in enumerate(rgos):
            assert casos[i].fluidosProducao[0]["rgo"] == rgo

    def test_variacao_api(self, caso_horizontal):
        apis = [15, 20, 25]
        for api_val in apis:
            c = copy.deepcopy(caso_horizontal)
            c.fluidosProducao[0]["api"] = api_val
            assert c.fluidosProducao[0]["api"] == api_val

    def test_adicionar_camada_isolante(self, caso_horizontal):
        """Adiciona uma camada isolante à seção transversal."""
        c = copy.deepcopy(caso_horizontal)
        camada_isolante = {
            "idMaterial": 1,
            "tipoMedicaoCamada": "ESPESSURA",
            "espessura": 0.05,
        }
        c.secaoTransversal[0]["camadas"].append(camada_isolante)
        assert len(c.secaoTransversal[0]["camadas"]) == 2
        # Original não foi alterado
        assert len(caso_horizontal.secaoTransversal[0]["camadas"]) == 1


# ============================================================================
# Testes de serialização JSON (to_json / from_json)
# ============================================================================

class TestJsonSerialization:
    """Testes de serialização e desserialização JSON."""

    def test_to_json_creates_file(self, caso_horizontal):
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                caso_horizontal.to_json("test_model")
                assert os.path.isfile(os.path.join(tmpdir, "test_model.json"))
            finally:
                os.chdir(original_cwd)

    def test_to_json_valid_json(self, caso_horizontal):
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                caso_horizontal.to_json("test_model")
                with open(os.path.join(tmpdir, "test_model.json"), "r") as f:
                    data = json.load(f)
                assert isinstance(data, dict)
            finally:
                os.chdir(original_cwd)

    def test_to_json_contains_expected_keys(self, caso_horizontal):
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                caso_horizontal.to_json("test_model")
                with open(os.path.join(tmpdir, "test_model.json"), "r") as f:
                    data = json.load(f)

                assert "sistema" in data
                assert "fluidosProducao" in data
                assert "material" in data
                assert "secaoTransversal" in data
                assert "dutosProducao" in data
                assert "fonteLiquido" in data
                assert "separador" in data
            finally:
                os.chdir(original_cwd)

    def test_roundtrip_json(self, caso_horizontal):
        """to_json → from_json deve preservar os dados."""
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                caso_horizontal.to_json("roundtrip")

                novo = marlim3.Tramo()
                novo.from_json(os.path.join(tmpdir, "roundtrip.json"))

                assert novo.sistema == caso_horizontal.sistema
                assert novo.fluidosProducao == caso_horizontal.fluidosProducao
                assert novo.material == caso_horizontal.material
                assert novo.secaoTransversal == caso_horizontal.secaoTransversal
                assert novo.dutosProducao == caso_horizontal.dutosProducao
                assert novo.fonteLiquido == caso_horizontal.fonteLiquido
                assert novo.separador == caso_horizontal.separador
            finally:
                os.chdir(original_cwd)

    def test_roundtrip_vertical(self, caso_vertical):
        """Caso vertical sobrevive a roundtrip JSON."""
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                caso_vertical.to_json("roundtrip_v")
                novo = marlim3.Tramo()
                novo.from_json(os.path.join(tmpdir, "roundtrip_v.json"))
                assert novo.dutosProducao[0]["angulo"] == pytest.approx(np.pi / 2)
            finally:
                os.chdir(original_cwd)

    def test_to_json_empty_fields(self):
        """to_json com generate_empty_fields gera campos vazios."""
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                t = marlim3.Tramo()
                t.to_json("empty_model", generate_empty_fields=True)
                with open(os.path.join(tmpdir, "empty_model.json"), "r") as f:
                    data = json.load(f)
                # Com generate_empty_fields=True, campos vazios devem existir
                assert "sistema" in data
            finally:
                os.chdir(original_cwd)

    def test_from_json_with_dict(self, caso_horizontal):
        """from_json aceita dicionário via is_string=True."""
        with tempfile.TemporaryDirectory() as tmpdir:
            original_cwd = os.getcwd()
            try:
                os.chdir(tmpdir)
                caso_horizontal.to_json("dict_test")
                with open(os.path.join(tmpdir, "dict_test.json"), "r") as f:
                    data = json.load(f)

                novo = marlim3.Tramo()
                novo.from_json(data, is_string=True)
                assert novo.fluidosProducao == caso_horizontal.fluidosProducao
            finally:
                os.chdir(original_cwd)


# ============================================================================
# Testes da classe Cenarios
# ============================================================================

class TestCenarios:
    """Testes de construção e uso da classe Cenarios."""

    def test_cenarios_default(self):
        c = marlim3.Cenarios()
        assert c.casos == {}

    def test_cenarios_with_cases(self, caso_horizontal, caso_vertical):
        """Cenarios aceita dicionário de Tramos."""
        cenarios = marlim3.Cenarios({
            "horizontal": caso_horizontal,
            "vertical": caso_vertical,
        })
        assert "horizontal" in cenarios.casos
        assert "vertical" in cenarios.casos
        assert len(cenarios.casos) == 2

    def test_cenarios_access(self, caso_horizontal):
        """Acesso ao caso dentro de Cenarios retorna o Tramo."""
        cenarios = marlim3.Cenarios({"caso1": caso_horizontal})
        assert cenarios.casos["caso1"] is caso_horizontal

    def test_cenarios_multiple_vazoes(self, caso_horizontal):
        """Padrão do notebook: criar cenários com múltiplas vazões."""
        vazoes = [200, 2000, 4000, 6000]
        casos = {}
        for vazao in vazoes:
            c = copy.deepcopy(caso_horizontal)
            c.fonteLiquido[0]["vazaoLiquido"] = [vazao]
            casos[f"Q = {vazao} sm3/d"] = c

        cenarios = marlim3.Cenarios(casos)
        assert len(cenarios.casos) == 4
        assert "Q = 200 sm3/d" in cenarios.casos
        assert "Q = 6000 sm3/d" in cenarios.casos

        for vazao in vazoes:
            label = f"Q = {vazao} sm3/d"
            assert cenarios.casos[label].fonteLiquido[0]["vazaoLiquido"] == [vazao]


# ============================================================================
# Testes da classe Rede
# ============================================================================

class TestRedeConstruction:
    """Testes básicos de instanciação da classe Rede."""

    def test_rede_default(self):
        r = marlim3.Rede()
        assert r.Arquivos == []
        assert r.Conexao == []


# ============================================================================
# Testes de geometria (estrutura do duto)
# ============================================================================

class TestGeometry:
    """Testes de consistência geométrica dos modelos."""

    def test_multiple_dutos(self, caso_horizontal):
        """Modelo com dois dutos concatenados."""
        c = copy.deepcopy(caso_horizontal)
        segundo_duto = copy.deepcopy(c.dutosProducao[0])
        segundo_duto["id"] = 1
        segundo_duto["angulo"] = np.pi / 4  # 45°
        c.dutosProducao.append(segundo_duto)
        assert len(c.dutosProducao) == 2
        assert c.dutosProducao[0]["angulo"] == 0
        assert c.dutosProducao[1]["angulo"] == pytest.approx(np.pi / 4)

    def test_discretizacao_consistency(self, caso_horizontal):
        """Verifica que a discretização é consistente."""
        duto = caso_horizontal.dutosProducao[0]
        disc = duto["discretizacao"][0]
        assert disc["nCelulas"] > 0
        assert disc["comprimento"] > 0

    def test_secao_diametro_positive(self, caso_horizontal):
        assert caso_horizontal.secaoTransversal[0]["diametroInterno"] > 0

    def test_secao_rugosidade_positive(self, caso_horizontal):
        assert caso_horizontal.secaoTransversal[0]["rugosidade"] > 0


# ============================================================================
# Testes de validação de dados de entrada
# ============================================================================

class TestInputValidation:
    """Testes de consistência dos dados de entrada do modelo."""

    def test_fluido_api_range(self, fluido):
        """API gravity deve estar num intervalo razoável."""
        assert 5 <= fluido["api"] <= 70

    def test_fluido_rgo_nonnegative(self, fluido):
        assert fluido["rgo"] >= 0

    def test_fluido_bsw_range(self, fluido):
        assert 0.0 <= fluido["bsw"] <= 1.0

    def test_fluido_densidadeGas_range(self, fluido):
        assert 0.5 <= fluido["densidadeGas"] <= 1.5

    def test_pressao_separador_positive(self, separador):
        assert all(p > 0 for p in separador["pressao"])

    def test_temperatura_fonte_positive(self, fonte_liquido):
        assert all(t > 0 for t in fonte_liquido["temperatura"])

    def test_vazao_fonte_positive(self, fonte_liquido):
        assert all(q > 0 for q in fonte_liquido["vazaoLiquido"])

    def test_material_condutividade_positive(self, material_aco):
        assert material_aco["condutividade"] > 0

    def test_material_calor_especifico_positive(self, material_aco):
        assert material_aco["calorEspecifico"] > 0

    def test_material_rho_positive(self, material_aco):
        assert material_aco["rho"] > 0


# ============================================================================
# Testes de from_json com arquivos demo
# ============================================================================

class TestDemoFiles:
    """Testes de carregamento dos arquivos JSON de demonstração."""

    DEMO_DIR = os.path.join(os.path.dirname(__file__), "..", "demos")

    def _demo_files(self):
        if not os.path.isdir(self.DEMO_DIR):
            pytest.skip("Diretório demos/ não encontrado")
        return [
            f for f in os.listdir(self.DEMO_DIR) if f.endswith(".json")
        ]

    def test_demo_files_exist(self):
        demos = self._demo_files()
        assert len(demos) > 0, "Nenhum arquivo demo .json encontrado"

    @pytest.mark.parametrize("demo_file", [
        "teste1-2zonas-2VGLs-2-Check-AS.json",
        "teste1-BCS-longo-eficMotor.json",
        "teste1-caso-injecProd.json",
    ])
    def test_load_demo_json(self, demo_file):
        """Arquivos demo devem ser carregáveis como Tramo."""
        filepath = os.path.join(self.DEMO_DIR, demo_file)
        if not os.path.isfile(filepath):
            pytest.skip(f"{demo_file} não encontrado")
        t = marlim3.Tramo()
        t.from_json(filepath)
        assert t.sistema is not None
