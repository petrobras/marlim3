"""Parsers for valves: master1, master2, separador, choke."""
from lxml import etree
import xmltodict

from ._base import kgf_por_cm2_manometrica_to_absoluta


def parse_master1(mr2_entrada, resultados):
    """Parse Master1 valve from MR2 data.
    
    Master1 is the main valve at wellhead. It should be fully open (abertura=1) initially.
    Comprimento medido = total length of production units until first well unit (natureza in [5,7])
    """
    comprimento_acumulado = 0
    quantidade_unidades = resultados.total_unidades_producao()
    
    # Accumulate length of production units outside the well
    # Stop when reaching first well unit (natureza 5 or 7)
    for indice_duto in range(quantidade_unidades):
        if resultados.natureza_unidade_producao(indice_duto) in [5, 7]:
            break
        comprimento_acumulado += resultados.comprimento_total_unidade_producao(indice_duto)

    return {
        'comprimentoMedido': comprimento_acumulado,
        'tempo': [0],
        'abertura': [1],  # 100% open - corrected from 0 (was closed)
    }


def parse_master2(mr2_reader, resultados):
    """Parse Master2 valve from MR2 data."""
    tree = mr2_reader.tree
    injecao_nodes = tree.xpath('/marlim/entrada/sistema_producao/injecao')
    comprimento_acumulado = 0.0
    quantidade_unidades_servico = resultados.total_unidades_servico()

    if injecao_nodes:
        injecao = injecao_nodes[0]
        for ind in range(quantidade_unidades_servico):
            natureza = resultados.natureza_unidade_servico(ind)
            ul = 0.0

            if natureza == 2:
                ul_nodes = injecao.xpath(
                    f"linha_maritima_inj[@seq='{ind}']/medidas_inj/ul/text()"
                )
            elif natureza == 3:
                ul_nodes = injecao.xpath(
                    f"linha_aerea_inj[@seq='{ind}']/medidas_inj/ul/text()"
                )
            else:
                ul_nodes = []

            if ul_nodes:
                try:
                    ul = float(ul_nodes[0])
                except ValueError:
                    pass

            comprimento_acumulado += ul

    return {
        'comprimentoMedido': comprimento_acumulado,
        'tempo': [0],
        'abertura': [1.0],
    }


def parse_separador(mr2_entrada):
    """Parse separator from MR2 data."""
    ptub = float(mr2_entrada['opcoes_calculo']['ptub'])
    pressao = kgf_por_cm2_manometrica_to_absoluta(ptub)
    return {
        'ativo': True,
        'tempo': [0],
        'pressao': [pressao],
    }
