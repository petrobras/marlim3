"""Parsers for thermal/formation properties and correction factors."""
from ._base import graus_to_radianos


def parse_correcao(resultados):
    """Parse correction factors from MR2 simulation results."""
    cp = resultados.correcao_P()
    ct = resultados.correcao_T()
    if not cp and not ct:
        return None
    return {
        'ativo': True,
        'dPdLHidro': cp if cp else [],
        'dPdLFric': cp if cp else [],
        'dTdL': ct if ct else [],
    }


def parse_formacao(mr2_entrada, resultados):
    """Parse formation thermal properties."""
    try:
        tempo = float(mr2_entrada['sistema_producao']['tempo'])
        if tempo == 0.0:
            tempo = 365.0
    except (KeyError, TypeError, ValueError):
        tempo = 365.0

    return {'TempoProducao': tempo}


def parse_formacao_propriedade(resultados, indice_duto):
    """Parse formation property for a specific duct unit."""
    condutividade = resultados.get_condutividade_formacao(indice_duto)
    massa_especifica = 2000.0
    difusividade = 1e-6
    calor_especifico = 1000.0

    if condutividade and condutividade != 0:
        difusividade = resultados.get_difusividade(indice_duto)
        if not difusividade or difusividade == 0:
            difusividade = 1e-6
        calor_especifico = condutividade / (difusividade * massa_especifica)
    else:
        condutividade = 2.5

    return {
        'condutividade': condutividade,
        'massaEspecifica': massa_especifica,
        'calorEspecifico': calor_especifico,
    }


def parse_correlacoes_escorregamento(mr2_reader, resultados):
    """
    Determine if permanent slip model should be enabled.
    Returns True if any well unit does NOT use HAGEDORN_BROWN.
    """
    import xmltodict
    from lxml import etree

    tree = mr2_reader.tree
    total_unidades = resultados.total_unidades_producao()

    for ind in range(total_unidades):
        natureza = resultados.natureza_unidade_producao(ind)
        if natureza in {1, 5, 7}:
            # Get correlation for this unit
            correlacao = _get_correlacao_escorregamento(tree, resultados, ind)
            if correlacao is None or correlacao.strip().upper() != 'HAGEDORN_BROWN':
                return True  # Enable permanent slip

    return False


def _get_correlacao_escorregamento(tree, resultados, ind_unidade):
    """Get the slip correlation for a production unit."""
    try:
        natureza = resultados.natureza_unidade_producao(ind_unidade)
        mapping = {
            1: 'riser', 2: 'linha_maritima', 3: 'linha_aerea',
            4: 'linha_enterrada', 5: 'poco', 7: 'poco',
        }
        tipo = mapping.get(natureza)
        if not tipo:
            return None

        saidaxorig = resultados.identificadores_unidade_producao(ind_unidade)
        if saidaxorig is None:
            return None

        node = tree.xpath(
            f'/marlim/entrada/sistema_producao/producao/{tipo}[@seq={saidaxorig}]/correlacao_esc/@tipo'
        )
        if node:
            return node[0]
    except Exception:
        pass
    return None
