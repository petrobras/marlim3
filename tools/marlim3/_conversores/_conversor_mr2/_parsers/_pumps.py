"""Parser for BCS (Electrical Submersible Pump)."""
from ._base import get_float


def parse_bcs_list(mr2_entrada, resultados, comprimento_master1):
    """Parse all BCS pumps from MR2 data."""
    bcs_data = mr2_entrada.get('conjuntos_bcs', {}).get('conjunto_bcs')
    if not bcs_data:
        return []

    if not isinstance(bcs_data, list):
        bcs_data = [bcs_data]

    distancia_mesa_rotativa = float(mr2_entrada['sistema_producao']['dmrcp'])
    lista_bcs = []

    for id_bcs, bcs_mr2 in enumerate(bcs_data):
        bcs = _parse_single_bcs(bcs_mr2, distancia_mesa_rotativa, comprimento_master1, id_bcs)
        if bcs:
            lista_bcs.append(bcs)

    return lista_bcs


def _parse_single_bcs(bcs_mr2, distancia_mesa_rotativa, comprimento_master1, id_bcs):
    """Parse a single BCS from MR2 data."""
    if 'bomba' not in bcs_mr2:
        return None

    # Active status
    ativo = bcs_mr2.get('@ativo', '').strip().lower() == 'true'

    # Position
    bcs_local = bcs_mr2['bcs_local_instalacao']
    profundidade_medida = float(bcs_local['#text'])
    local_instalacao = bcs_local['@tipo']

    if local_instalacao == 'FORA':
        comprimento_medido = profundidade_medida
    elif local_instalacao == 'DENTRO':
        comprimento_medido = (
            profundidade_medida - distancia_mesa_rotativa + comprimento_master1
        )
    else:
        comprimento_medido = profundidade_medida

    # Performance table
    bomba = bcs_mr2['bomba']
    freq_referencia = float(bomba.get('freq_motor', 60))
    n_estagios_fab = int(bomba.get('estagios', 1))
    n_estagios = int(bcs_mr2.get('estagios', n_estagios_fab))

    vazao_arr = []
    head_arr = []
    eficiencia_arr = []
    potencia_arr = []
    linhas = bomba.get('tabela_desempenho', {}).get('linha_desempenho', [])
    if not isinstance(linhas, list):
        linhas = [linhas]
    for ld in linhas:
        valores = ld.get('valor', [])
        if len(valores) >= 4:
            vazao_arr.append(float(valores[0]))
            head_arr.append(float(valores[1]))
            eficiencia_arr.append(float(valores[2]))
            potencia_arr.append(float(valores[3]))

    # Frequency
    freq_operacao = float(bcs_mr2.get('freq_motor', freq_referencia))

    return {
        'id': id_bcs,
        'ativo': ativo,
        'comprimentoMedido': comprimento_medido,
        'tempo': [0],
        'frequencia': [freq_operacao],
        'vazao': vazao_arr,
        'head': head_arr,
        'eficiencia': eficiencia_arr,
        'potencia': potencia_arr,
        'freqref': freq_referencia,
        'nestag': n_estagios,
        'nestagFab': n_estagios_fab,
    }
