"""
Reads and parses .mr2 XML files and their embedded simulation results.
"""
import os

import xmltodict
from lxml import etree

from ._parsers._base import (
    array_centimetros_to_metros,
    array_milimetros_to_metros,
    array_polegadas_to_metros,
    kgf_por_cm2_manometrica_to_absoluta,
)


class Mr2Reader:
    """Reads an .mr2 file, exposing XML input data and the lxml tree."""

    def __init__(self, caminho_mr2):
        self.caminho_mr2 = caminho_mr2
        self.diretorio = os.path.dirname(os.path.abspath(caminho_mr2))
        self.nome_completo = os.path.basename(caminho_mr2)
        self.nome = os.path.splitext(self.nome_completo)[0]

        with open(caminho_mr2, 'r', encoding='latin-1') as f:
            self._conteudo = f.read()

    @property
    def possui_resultados(self):
        markers = ['#RESULT=TODOS', '<!-- ##INICIO_SAIDA##']
        return all(m in self._conteudo for m in markers)

    @property
    def xml_string(self):
        """The input section of the .mr2 (XML before results)."""
        fim_entrada = '<!-- ##FIM_ENTRADA## -->'
        idx = self._conteudo.find(fim_entrada)
        if idx == -1:
            inicio_saida = '<!-- ##INICIO_SAIDA##'
            idx = self._conteudo.find(inicio_saida)
            if idx >= 0:
                idx -= 1
        return self._conteudo[:idx] if idx >= 0 else self._conteudo

    @property
    def tree(self):
        """lxml Element tree of the input XML."""
        if not hasattr(self, '_tree'):
            xml_bytes = self.xml_string.encode('latin-1')
            parser = etree.XMLParser(encoding='ISO-8859-1', recover=True)
            self._tree = etree.fromstring(xml_bytes, parser=parser)
        return self._tree

    @property
    def dados_geometria(self):
        """Full input data as a dict via xmltodict."""
        if not hasattr(self, '_dados_geometria'):
            self._dados_geometria = xmltodict.parse(self.xml_string)
        return self._dados_geometria

    @property
    def mr2_entrada(self):
        """The 'entrada' section dict."""
        return self.dados_geometria['marlim']['entrada']

    @property
    def nome_arquivo_pvt(self):
        """Name of the referenced PVT file (.tab), or empty string."""
        try:
            return (
                self.mr2_entrada['fluidos']['fluido_composicional']['@tabela']
                .strip()
                .lower()
            )
        except (KeyError, TypeError):
            return ''

    @property
    def possui_arquivo_pvt(self):
        if 'fluido_composicional' not in self._conteudo:
            return False
        fontes_ativo = self.tree.xpath(
            "/marlim/entrada/sistema_producao/producao/fonte[@ativo='TRUE']"
        )
        if not fontes_ativo:
            return False
        fonte = xmltodict.parse(etree.tostring(fontes_ativo[0]))
        id_fluido = fonte['fonte']['@id_fluido']
        fluido_comp = self.tree.xpath(
            f'/marlim/entrada/fluidos/fluido_composicional[@id={id_fluido}]'
        )
        return bool(fluido_comp)

    def conteudo_sem_resultados(self):
        """Return XML input content (for rewriting without results)."""
        return self.xml_string


class Mr2SimulationResults:
    """Parses the simulation results section of an .mr2 file."""

    SEPARADOR = ';'

    def __init__(self, mr2_reader):
        self._reader = mr2_reader
        self._resultado_formatado = None

    @property
    def _string_resultados(self):
        marker = '<!-- ##INICIO_SAIDA##'
        idx = self._reader._conteudo.find(marker)
        if idx == -1:
            return None
        return self._reader._conteudo[idx:]

    @property
    def resultado_simulacao_formatado(self):
        if self._resultado_formatado is not None:
            return self._resultado_formatado

        dados = {}
        text = self._string_resultados
        if not text:
            self._resultado_formatado = dados
            return dados

        ignore = ['#RESULT', '#FIM_SAIDA']
        for linha in text.splitlines():
            linha = linha.strip()
            if any(linha.startswith(ig) for ig in ignore):
                continue
            parts = linha.split('=')
            if len(parts) == 2:
                chave = parts[0].strip()
                valor = parts[1].strip()
                if chave not in dados:
                    dados[chave] = []
                dados[chave].append(valor)

        self._resultado_formatado = dados
        return dados

    def _pegar_valores_lista(self, chave, tipo=float):
        lista = []
        lista_mr2 = self.resultado_simulacao_formatado.get(chave, [])
        for item_mr2 in lista_mr2:
            item = [
                tipo(x) for x in item_mr2.split(self.SEPARADOR) if x
            ]
            lista.extend(item)
        return list(reversed(lista))

    def _safe_convert(self, s, target_type):
        try:
            return target_type(s)
        except (ValueError, TypeError):
            return None

    def _converted_list(self, lista_string, target_type):
        return [
            v for s in lista_string
            if (v := self._safe_convert(s, target_type)) is not None
        ]

    def _pega_lista_por_chave(self, chave):
        return self._pega_valor_lista_por_chave_indice(chave, 0)

    def _pega_valor_lista_por_chave_indice(self, chave, indice):
        resultado = self.resultado_simulacao_formatado
        vals = resultado.get(chave, [''])
        if indice >= len(vals):
            return ['']
        return vals[indice].split(self.SEPARADOR)

    def _pega_valor_por_chave_indice(self, chave, indice):
        return self._pega_lista_por_chave(chave)[indice]

    def _existe_valor_por_chave(self, chave):
        return bool(self.resultado_simulacao_formatado.get(chave))

    def _get_converted_list_by_key(self, chave, target_type, unique=False):
        lista_str = self._pega_lista_por_chave(chave)
        if unique:
            seen = set()
            result = []
            for s in lista_str:
                v = self._safe_convert(s, target_type)
                if v is not None and v not in seen:
                    seen.add(v)
                    result.append(v)
            return result
        return self._converted_list(lista_str, target_type)

    # --- Production units ---

    def total_unidades_producao(self):
        return int(self.resultado_simulacao_formatado.get('#NUN', ['0'])[0])

    def total_unidades_servico(self):
        if not self._existe_valor_por_chave('#NUN_INJ'):
            return 0
        return int(self.resultado_simulacao_formatado['#NUN_INJ'][0])

    def natureza_unidade_producao(self, indice_duto):
        return int(self._pega_valor_por_chave_indice('#NAT', indice_duto))

    def natureza_unidade_servico(self, indice_duto):
        return int(self._pega_valor_por_chave_indice('#NAT_INJ', indice_duto))

    def comprimento_total_unidade_producao(self, indice_duto):
        return float(self._pega_valor_por_chave_indice('#UL', indice_duto))

    def comprimento_total_producao(self):
        return sum(self._get_converted_list_by_key('#UL', float))

    def indices_unidades_producao_valvulas(self):
        if not self._existe_valor_por_chave('#VV'):
            return None
        return self._get_converted_list_by_key('#VV', int)

    def correcao_P(self):
        if not self._existe_valor_por_chave('#FCORRP'):
            return None
        return self._get_converted_list_by_key('#FCORRP', float, unique=True)

    def get_correcao_P(self, indice_unidade):
        if not self._existe_valor_por_chave('#FCORRP'):
            return None
        vals = self._get_converted_list_by_key('#FCORRP', float)
        return vals[indice_unidade] if indice_unidade < len(vals) else None

    def correcao_T(self):
        if not self._existe_valor_por_chave('#FCORRT'):
            return None
        return self._get_converted_list_by_key('#FCORRT', float, unique=True)

    def get_correcao_T(self, indice_unidade):
        if not self._existe_valor_por_chave('#FCORRT'):
            return None
        vals = self._get_converted_list_by_key('#FCORRT', float)
        return vals[indice_unidade] if indice_unidade < len(vals) else None

    def get_pressao_montante(self):
        val = self._pega_valor_por_chave_indice('#PWH', 0)
        return kgf_por_cm2_manometrica_to_absoluta(float(val))

    def get_temperatura_montante(self):
        val = self._pega_valor_por_chave_indice('#TWF', 0)
        return float(val)

    def get_temperaturas_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#T', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def get_pressoes_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#P', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def _get_vector_from_keys(self, indice_unidade, keys):
        """Return first available vector for a unit from a list of MR2 output keys."""
        resultado = self.resultado_simulacao_formatado
        for key in keys:
            dados = resultado.get(key, [])
            if not dados or indice_unidade >= len(dados):
                continue
            vals = self._converted_list(
                dados[indice_unidade].strip().split(self.SEPARADOR), float
            )
            if vals:
                return vals
        return []

    def get_ulss_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#VELL', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def get_ugss_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#VELG', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def get_massas_especificas_liquido_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#DENL', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def get_massas_especificas_gas_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#DENG', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def get_holdups_unidade_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#HOLD', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def temperaturas_revestimento_producao(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#TTI', [])
        if not dados or indice_unidade >= len(dados):
            return None
        lista_str = dados[indice_unidade].strip().split(self.SEPARADOR)
        return self._converted_list(lista_str, float)

    def get_temperatura_externa_unidade_producao(self, indice_unidade):
        """Temperature of the external medium around production unit cells.

        Uses MR2 ambient temperature key ``#TA`` (seawater / air / soil
        temperature profile).  This matches the original converter which
        reads ``CHAVE_TEMPERATURA_EXTERNA_UNIDADES = '#TA'``.
        """
        vals = self._get_vector_from_keys(
            indice_unidade,
            ['#TA'],
        )
        return vals if vals else []

    def get_temperatura_externa_unidade_servico(self, indice_unidade):
        """Temperature of the external medium around service unit cells.

        Uses MR2 ambient temperature key ``#TAinj``.  This matches the
        original converter which reads
        ``CHAVE_TEMPERATURA_EXTERNA_UNIDADES_SERVICO = '#TAinj'``.
        """
        vals = self._get_vector_from_keys(
            indice_unidade,
            ['#TAinj'],
        )
        return vals if vals else []

    def vazao_injecao_calculada(self, indice_valvula):
        try:
            val = self._pega_valor_por_chave_indice('#QGIT', 0)
            return float(val)
        except (IndexError, ValueError, TypeError):
            return 0.0

    def vazao_gas_valvula(self, indice_valvula):
        try:
            val = self._pega_valor_por_chave_indice('#QGI1', indice_valvula)
            return float(val)
        except (IndexError, ValueError, TypeError):
            return 0.0

    # --- Geometric/thermal properties per unit ---

    def _get_property_per_unit(self, chave, indice, converter=None):
        """Get a property value for a specific unit, optionally applying conversion."""
        if not self._existe_valor_por_chave(chave):
            return 0.0
        dados = self._pega_lista_por_chave(chave)
        if not dados or indice >= len(dados):
            return 0.0
        val = self._safe_convert(dados[indice], float)
        if val is None:
            return 0.0
        if converter:
            return converter(val)
        return val

    def get_diametro_interno_producao(self, indice):
        return self._get_property_per_unit('#DTI', indice, lambda v: v * 0.0254)

    def get_rugosidade_producao(self, indice):
        return self._get_property_per_unit('#EDA', indice, lambda v: v / 1000)

    def get_espessura_coluna_producao(self, indice):
        return self._get_property_per_unit('#ESPT', indice, lambda v: v / 100)

    def get_espessura_isolamento(self, indice):
        return self._get_property_per_unit('#ESPI', indice, lambda v: v / 100)

    def get_espessura_revestimento(self, indice):
        return self._get_property_per_unit('#ESPR', indice, lambda v: v / 100)

    def get_espessura_cimento(self, indice):
        return self._get_property_per_unit('#ESPC', indice, lambda v: v / 100)

    def get_condutividade_formacao(self, indice):
        return self._get_property_per_unit('#XKF', indice)

    def get_condutividade_coluna_producao(self, indice):
        return self._get_property_per_unit('#XKT', indice)

    def get_condutividade_isolamento(self, indice):
        return self._get_property_per_unit('#XKR', indice)

    def get_condutividade_isolamento_poco(self, indice):
        return self._get_property_per_unit('#XKI', indice)

    # Aliases matching original converter naming
    def get_condutividade_revestimento_producao_poco(self, indice):
        """Casing conductivity (#XKR) — same key as isolamento LM."""
        return self._get_property_per_unit('#XKR', indice)

    def get_condutividade_isolamento_producao_poco(self, indice):
        """Well insulation conductivity (#XKI)."""
        return self._get_property_per_unit('#XKI', indice)

    def get_condutividade_cimento(self, indice):
        return self._get_property_per_unit('#XKC', indice)

    def get_difusividade(self, indice):
        return self._get_property_per_unit('#ALFFOR', indice)

    def get_diametro_interno_revestimento(self, indice):
        return self._get_property_per_unit('#DRI', indice, lambda v: v * 0.0254)

    def get_fluido_anular(self, indice):
        return int(self._get_property_per_unit('#NANU', indice))

    # --- Service line properties ---

    # Mapping from results keys to XML tag names inside <coluna_producao>
    _SERVICE_XML_TAGS = {
        '#DTI_INJ': 'dti',
        '#EDA_INJ': 'eda',
        '#ESPT_INJ': 'espt',
        '#ESPI_INJ': 'espi_cp',
        '#XKT_INJ': 'xkt',
        '#XKR_INJ': 'xki_cp',
    }

    def _get_service_property(self, chave, indice):
        if self._existe_valor_por_chave(chave):
            dados = self._pega_lista_por_chave(chave)
            if dados and indice < len(dados):
                val = self._safe_convert(dados[indice], float)
                if val is not None:
                    return val
        # Fallback: read from XML <linha_maritima_inj> elements
        xml_tag = self._SERVICE_XML_TAGS.get(chave)
        if xml_tag:
            nodes = self._reader.tree.xpath('//linha_maritima_inj')
            if indice < len(nodes):
                cp = nodes[indice].find('coluna_producao')
                if cp is not None:
                    val_str = cp.findtext(xml_tag)
                    if val_str:
                        val = self._safe_convert(val_str, float)
                        if val is not None:
                            return val
        return 0.0

    def get_diametro_interno_servico(self, indice):
        val = self._get_service_property('#DTI_INJ', indice)
        return val * 0.0254  # in → m

    def get_rugosidade_servico(self, indice):
        val = self._get_service_property('#EDA_INJ', indice)
        return val / 1000  # mm → m

    def get_espessura_coluna_servico(self, indice):
        val = self._get_service_property('#ESPT_INJ', indice)
        return val / 100  # cm → m

    def get_espessura_isolamento_servico(self, indice):
        val = self._get_service_property('#ESPI_INJ', indice)
        return val / 100  # cm → m

    def get_condutividade_coluna_servico(self, indice):
        # Original converter reads from production key (#XKT) indexed by service unit index
        return self._get_property_per_unit('#XKT', indice)

    def get_condutividade_isolamento_servico(self, indice):
        return self._get_service_property('#XKR_INJ', indice)

    def get_temperaturas_servico(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#Tinj', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def get_pressoes_servico(self, indice_unidade):
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#Pinj', [])
        if not dados or indice_unidade >= len(dados):
            return []
        return self._converted_list(
            dados[indice_unidade].strip().split(self.SEPARADOR), float
        )

    def _get_indice_servico_pela_producao(self, indice_producao):
        """Map a production unit index to the corresponding service unit index.

        ``#TInjProd`` / ``#PInjProd`` are structured as:
          [0 .. NUN_INJ-1]  = outside-well service units
          [NUN_INJ .. end]  = well production units (in same order as #NAT)

        So for a production unit at ``indice_producao`` that is inside the
        well, the corresponding index in ``#TInjProd`` is:
          NUN_INJ + (indice_producao - first_well_production_index)
        """
        naturezas = self._get_converted_list_by_key('#NAT', int)
        # Find the index of the first well unit in #NAT
        first_well_idx = 0
        for i, nat in enumerate(naturezas):
            if nat not in (2, 3, 4, 9, 10):
                first_well_idx = i
                break
        else:
            # No well units found
            return 0

        nun_inj = self.total_unidades_servico()
        return nun_inj + (indice_producao - first_well_idx)

    def get_temperaturas_servico_pela_producao(self, indice_producao):
        """Service temperatures measured along the production line (``#TInjProd``)."""
        ind = self._get_indice_servico_pela_producao(indice_producao)
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#TInjProd', [])
        if not dados or ind >= len(dados):
            return []
        return self._converted_list(
            dados[ind].strip().split(self.SEPARADOR), float
        )

    def get_pressoes_servico_pela_producao(self, indice_producao):
        """Service pressures measured along the production line (``#PInjProd``)."""
        ind = self._get_indice_servico_pela_producao(indice_producao)
        resultado = self.resultado_simulacao_formatado
        dados = resultado.get('#PInjProd', [])
        if not dados or ind >= len(dados):
            return []
        vals = self._converted_list(
            dados[ind].strip().split(self.SEPARADOR), float
        )
        from ._parsers._base import kgf_por_cm2_manometrica_to_absoluta
        return [kgf_por_cm2_manometrica_to_absoluta(v) for v in vals]

    def identificadores_unidade_producao(self, indice_unidade):
        naturezas = self._get_converted_list_by_key('#NAT', int)
        if not naturezas:
            return None
        num_invalidas = sum(1 for i in range(indice_unidade) if naturezas[i] in (9, 10))
        ind = indice_unidade - num_invalidas
        return self._pega_valor_por_chave_indice('#SAIDAXORIG', ind)
