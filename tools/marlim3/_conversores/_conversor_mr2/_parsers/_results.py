"""Parser for results configuration: production trends."""
import xmltodict
from lxml import etree


def parse_tendencias_producao(mr2_reader, resultados):
    """Parse production trend points from gas lift valve positions."""
    tree = mr2_reader.tree
    total_producao = resultados.total_unidades_producao()
    indices_valvula = resultados.indices_unidades_producao_valvulas()
    tendencias = []
    comprimento_acumulado = 0.0

    if not indices_valvula:
        return tendencias

    for ind in range(total_producao):
        comprimento_acumulado += resultados.comprimento_total_unidade_producao(ind)

        if indices_valvula[ind] <= 0:
            continue

        id_valvula = indices_valvula[ind] - 1
        valvula_node = tree.xpath(
            f"/marlim/entrada/gas_lift/valvula_instalada[@seq='{id_valvula}']"
        )
        if not valvula_node:
            continue

        valvula = valvula_node[0]
        orificio_nodes = valvula.xpath("./vgl_inst/orificio/text()")
        orificio = orificio_nodes[0] if orificio_nodes else None

        # Only create trend for blind (CEGA) valves
        if not orificio or orificio.strip().upper() != 'CEGA':
            continue

        rotulo_nodes = valvula.xpath("./vgl_inst/fabricante/text()")
        rotulo = rotulo_nodes[0] if rotulo_nodes else ''

        tendencia = {
            'ativo': True,
            'comprimentoMedido': comprimento_acumulado,
            'intervaloTempo': 1000,
            'rotulo': rotulo,
            'pressao': True,
            'temperatura': True,
            'holdup': True,
            'bet': True,
            'ugs': True,
            'uls': True,
            'ug': True,
            'ul': True,
            'arranjo': True,
            'viscosidadeLiquido': True,
            'viscosidadeGas': True,
            'rhoLiquido': True,
            'rhoGas': True,
            'vazaoMassicaGas': True,
            'vazaoMassicaLiquido': True,
        }
        tendencias.append(tendencia)

    return tendencias
