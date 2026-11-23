"""Parsers for boundary conditions: IPR, fonteLiquido, condicaoPressao."""
from ._base import kgf_por_cm2_manometrica_to_absoluta


def parse_ipr(mr2_entrada, tree, resultados):
    """Parse IPR from MR2 data. Returns a dict or None if no IPR is present."""
    import xmltodict
    from lxml import etree as et

    node_reservatorio = tree.xpath(
        "/marlim/entrada/sistema_producao/producao/fonte[@ativo='TRUE'][@tipo='RESERVATORIO']"
    )
    if not node_reservatorio:
        return None

    reservatorio_dict = xmltodict.parse(et.tostring(node_reservatorio[0]))
    fonte = reservatorio_dict['fonte']
    modelo_ipr = fonte['modelo']['@ipr'].upper()

    if modelo_ipr not in {'VOGEL', 'LINEAR'}:
        return None
    
    if modelo_ipr == 'VOGEL':
        modelo_ipr = 'VOGEL_COMBINADA'

    # Pressure
    pr = float(fonte['pr'])
    pressao_estatica = kgf_por_cm2_manometrica_to_absoluta(pr)

    # Temperature
    temp = float(fonte['tmonf'])

    # IP
    ip_str = fonte.get('dados_ipr', {}).get('ip', '')
    ip = float(ip_str) if ip_str else 0

    # Map model name to tipoIPR integer
    tipo_ipr_map = {'LINEAR': 0, 'VOGEL': 2, 'VOGEL_COMBINADA': 1}
    tipo_ipr = tipo_ipr_map.get(modelo_ipr, 1)

    # Comprimento medido = total production length
    comprimento_medido = resultados.comprimento_total_producao()

    result = {
        'ativo': True,
        'id': 0,
        'indFluidoPro': 0,
        'comprimentoMedido': comprimento_medido,
        'tipoIPR': tipo_ipr,
        'tempoPressaoEstatica': [0],
        'pressaoEstatica': [pressao_estatica],
        'tempoTemperaturas': [0],
        'temperaturas': [temp],
        'tempoip': [0],
        'ip': [ip],
        'tempoii': [0],
        'ii': [ip],
    }

    # tempoqMax/qMax only for Vogel pura (tipoIPR=2)
    if tipo_ipr == 2:
        q_max_str = fonte.get('dados_ipr', {}).get('vazao_max', '')
        q_max = float(q_max_str) if q_max_str else 0
        result['tempoqMax'] = [0]
        result['qMax'] = [q_max]
    return result


def parse_fonte_liquido(mr2_entrada, tree, resultados):
    """Parse fonteLiquido from MR2 data. Returns a dict or None if no FONTE_MASSA source."""
    import xmltodict
    from lxml import etree as et

    node_fonte_massa = tree.xpath(
        "/marlim/entrada/sistema_producao/producao/fonte[@ativo='TRUE'][@tipo='FONTE_MASSA']"
    )
    if not node_fonte_massa:
        return None

    fonte_dict = xmltodict.parse(et.tostring(node_fonte_massa[0]))
    fonte = fonte_dict['fonte']

    # Temperature
    temp = float(fonte.get('tmonf', 0))

    # Comprimento medido = total production length
    comprimento_medido = resultados.comprimento_total_producao()

    # Flow rate: gas source uses qgsc, liquid uses qlsc
    tipo_fluido = fonte.get('@tipo_fluido', '').upper()
    opcoes = mr2_entrada.get('opcoes_calculo', {})
    if tipo_fluido == 'GAS':
        vazao_str = opcoes.get('qgsc', '0')
    else:
        vazao_str = opcoes.get('qlsc', '0')
    try:
        vazao = float(vazao_str) if vazao_str else 0.0
    except (ValueError, TypeError):
        vazao = 0.0

    return {
        'ativo': True,
        'id': 0,
        'indiFluidoPro': 0,
        'comprimentoMedido': comprimento_medido,
        'tempo': [0.0],
        'temperatura': [temp],
        'beta': [0.0],
        'vazaoLiquido': [vazao],
    }


def parse_condicao_pressao(resultados):
    """Parse pressure boundary condition from MR2 simulation results."""
    pressao = resultados.get_pressao_montante()
    temperatura = resultados.get_temperatura_montante()

    # Calculate titulo (quality)
    total_unidades = resultados.total_unidades_producao()
    titulo = None

    if total_unidades > 0:
        denl = resultados.get_massas_especificas_liquido_unidade_producao(total_unidades - 1)
        deng = resultados.get_massas_especificas_gas_unidade_producao(total_unidades - 1)
        vell = resultados.get_ulss_unidade_producao(total_unidades - 1)
        velg = resultados.get_ugss_unidade_producao(total_unidades - 1)

        if denl and deng and vell and velg:
            denl_last = denl[-1]
            deng_last = deng[-1]
            vell_last = vell[-1]
            velg_last = velg[-1]

            denom = (velg_last * deng_last) + (vell_last * denl_last)
            if denom != 0:
                titulo = (velg_last * deng_last) / denom
            else:
                titulo = 0

    ativo = False  # Production wells use IPR + master1, not fixed pressure BC
    return {
        'ativo': ativo,
        'tempo': [0.0],
        'pressao': [pressao if pressao else 0.0],
        'temperatura': [temperatura if temperatura else 0.0],
        'titulo': [titulo if titulo is not None else 0.0],
        'razaoBeta': [0],
    }
