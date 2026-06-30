"""Parsers for gas sources and gas lift valves."""
import xmltodict
from lxml import etree

from ._base import kgf_por_cm2_manometrica_to_absoluta, polegadas_to_metros


def parse_fonte_gas_list(tree, resultados):
    """Parse all gas sources from MR2 data."""
    lista_fonte_gas = []
    id_fonte_gas = 0
    comprimento_acumulado = 0

    numero_unidades = resultados.total_unidades_producao()
    indices_valvula = resultados.indices_unidades_producao_valvulas()

    # Check if there's active non-blind gas lift
    fonte_gas_lift_ativa = False
    if indices_valvula:
        for i in range(numero_unidades):
            if indices_valvula[i] > 0:
                id_valvula = indices_valvula[i] - 1
                if _possui_fonte_gas_lift_ativa_nao_cega(tree, id_valvula):
                    fonte_gas_lift_ativa = True
                    break

    for i in range(numero_unidades):
        comprimento_acumulado += resultados.comprimento_total_unidade_producao(i)
        fonte_gas = _parse_fonte_gas_unit(tree, i, id_fonte_gas, resultados, comprimento_acumulado)

        if not fonte_gas and indices_valvula and indices_valvula[i] > 0:
            id_valvula = indices_valvula[i] - 1
            fonte_gas = _parse_valvula_como_fonte_gas(
                tree, id_valvula, id_fonte_gas, i, resultados,
                fonte_gas_lift_ativa, comprimento_acumulado
            )

        if fonte_gas:
            lista_fonte_gas.append(fonte_gas)
            id_fonte_gas += 1

    return lista_fonte_gas


def parse_fonte_gas_lift_list(tree, mr2_entrada, resultados):
    """Parse gas lift valves (DESEMPENHO type) from MR2 data."""
    comprimento_acumulado_servico = _parse_comprimento_servico_fora_poco(resultados, mr2_entrada)
    indices_valvula = resultados.indices_unidades_producao_valvulas()
    numero_unidades = resultados.total_unidades_producao()
    lista = []
    comprimento_acumulado_producao = 0

    for i in range(numero_unidades):
        comprimento_duto = resultados.comprimento_total_unidade_producao(i)
        comprimento_acumulado_producao += comprimento_duto
        natureza = resultados.natureza_unidade_producao(i)

        # Service line follows production inside well
        if natureza in {1, 5, 7}:
            if not comprimento_acumulado_servico and i == 0:
                comprimento_acumulado_servico = comprimento_duto
            elif comprimento_acumulado_servico:
                comprimento_acumulado_servico += comprimento_duto

        if not indices_valvula or indices_valvula[i] <= 0:
            continue

        id_valvula = indices_valvula[i] - 1
        node = tree.xpath(
            f"/marlim/entrada/gas_lift/valvula_instalada[@seq={id_valvula} and @operacao='DESEMPENHO']"
        )
        if not node:
            continue

        fgl_dict = xmltodict.parse(etree.tostring(node[0]))
        fgl_mr2 = fgl_dict['valvula_instalada']

        # Skip blind valves
        orificio = fgl_mr2.get('vgl_inst', {}).get('orificio', '')
        if orificio and orificio.upper() == 'CEGA':
            continue
        # diam_orificio == 0 is equivalent to a blind valve
        try:
            diam_orificio = float(fgl_mr2.get('vgl_inst', {}).get('diam_orificio', -1))
            if diam_orificio == 0:
                continue
        except (ValueError, TypeError):
            pass

        vgl = _parse_single_gas_lift_valve(
            id_valvula, fgl_mr2,
            comprimento_acumulado_producao,
            comprimento_acumulado_servico or 0
        )
        if vgl:
            lista.append(vgl)

    return lista


def parse_gas_injecao(mr2_entrada, resultados):
    """Parse gas injection boundary condition.
    
    Iterates through service units to find first active one for temperature/flow.
    """
    temperatura = 0.0
    vazao_gas = 0.0
    total_servico = resultados.total_unidades_servico()
    
    if total_servico and total_servico > 0:
        # Keep original behavior: prefer service unit index 0.
        try:
            temps = resultados.get_temperaturas_servico(0)
            if temps:
                temperatura = temps[0]
        except (ValueError, IndexError, AttributeError):
            temperatura = 0.0

        # Fallback: try other service units only if index 0 is unavailable.
        if not temperatura:
            for ind_servico in range(total_servico):
                try:
                    temps = resultados.get_temperaturas_servico(ind_servico)
                    if temps:
                        temperatura = temps[0]
                        break
                except (ValueError, IndexError, AttributeError):
                    continue
    
    if not temperatura:
        try:
            temp_revest = resultados.temperaturas_revestimento_producao(0)
            temperatura = temp_revest[0] if temp_revest else 0.0
        except (ValueError, IndexError, AttributeError):
            temperatura = 0.0
    
    # Try each service unit for flow (not just 0)
    if total_servico and total_servico > 0:
        for ind_servico in range(total_servico):
            try:
                vazao = resultados.vazao_injecao_calculada(ind_servico)
                if vazao and vazao > 0:
                    vazao_gas = vazao
                    break
            except (ValueError, IndexError, AttributeError):
                continue
    
    if not vazao_gas:
        try:
            vazao_gas = resultados.vazao_injecao_calculada(0)
        except (ValueError, IndexError, AttributeError):
            vazao_gas = 0.0

    try:
        prev = float(mr2_entrada.get('gas_lift', {}).get('prev', 0))
        pressao_injecao = kgf_por_cm2_manometrica_to_absoluta(prev)
    except (ValueError, TypeError):
        pressao_injecao = 0.0

    return {
        'ativo': True,
        'tipoCC': 1,
        'tempo': [0.0],
        'temperatura': [temperatura],
        'pressaoInjecao': [pressao_injecao],
        'vazaoGas': [vazao_gas],
        'chuteVazaoInjecao': vazao_gas > 0,
    }


# --- Internal helpers ---

def _parse_fonte_gas_unit(tree, indice_duto, id_fonte, resultados, comprimento_medido):
    """Try to parse a gas source from a production unit."""
    natureza = resultados.natureza_unidade_producao(indice_duto)
    mapping = {1: 'riser', 2: 'linha_maritima', 3: 'linha_aerea', 4: 'linha_enterrada'}
    tipo_unidade = mapping.get(natureza)
    if not tipo_unidade:
        return None

    saidaxorig = resultados.identificadores_unidade_producao(indice_duto)
    if saidaxorig is None:
        return None

    vazao_nodes = tree.xpath(
        f'/marlim/entrada/sistema_producao/producao/{tipo_unidade}[@seq={saidaxorig}]/qgi'
    )
    if not vazao_nodes:
        return None

    try:
        vazao = float(vazao_nodes[0].text)
    except (ValueError, AttributeError, TypeError):
        return None

    if vazao <= 0:
        return None

    return {
        'id': id_fonte,
        'ativo': True,
        'comprimentoMedido': comprimento_medido,
        'tempo': [0],
        'vazaoGas': [vazao],
        'temperatura': [40],
    }


def _parse_valvula_como_fonte_gas(tree, id_valvula, id_fonte, indice_duto, resultados, fonte_gas_lift_ativa, comprimento_medido):
    """Parse a gas lift valve operating as QGI/RGLI/RGLT as a gas source."""
    node = tree.xpath(
        f"/marlim/entrada/gas_lift/valvula_instalada[@seq={id_valvula} and (@operacao='QGI' or @operacao='RGLI' or @operacao='RGLT')]"
    )
    if not node:
        return None

    vgl_dict = xmltodict.parse(etree.tostring(node[0]))
    vgl = vgl_dict['valvula_instalada']

    # Skip blind valves (CEGA)
    vgl_inst = vgl.get('vgl_inst', {})
    orificio = vgl_inst.get('orificio', '')
    if orificio and orificio.upper() == 'CEGA':
        return None
    # diam_orificio == 0 is equivalent to a blind valve
    try:
        diam_orificio = float(vgl_inst.get('diam_orificio', -1))
        if diam_orificio == 0:
            return None
    except (ValueError, TypeError):
        pass

    # Flow rate
    try:
        vazao = float(vgl.get('qgi_valv', 0))
    except (ValueError, TypeError):
        vazao = 0.0

    # Skip if flow rate is zero (no actual gas injection)
    if vazao == 0.0:
        return None

    ativo = vgl.get('@ativo', 'FALSE').strip().upper() == 'TRUE'

    # Temperature
    total_servico = resultados.total_unidades_servico()
    if total_servico and total_servico > 0 and fonte_gas_lift_ativa:
        # The production-duct index may not match service-duct index.
        # Prefer same index when valid; otherwise fallback to first non-empty service profile.
        temperatura = 0.0
        try:
            if indice_duto < total_servico:
                temps = resultados.get_temperaturas_servico(indice_duto)
                if temps:
                    temperatura = temps[0]
        except (ValueError, IndexError, AttributeError):
            temperatura = 0.0

        if not temperatura:
            for ind_servico in range(total_servico):
                try:
                    temps = resultados.get_temperaturas_servico(ind_servico)
                    if temps:
                        temperatura = temps[0]
                        break
                except (ValueError, IndexError, AttributeError):
                    continue
    else:
        temp_revest = resultados.temperaturas_revestimento_producao(indice_duto)
        temperatura = temp_revest[0] if temp_revest else 0.0

    return {
        'id': id_fonte,
        'ativo': ativo,
        'comprimentoMedido': comprimento_medido,
        'tempo': [0],
        'vazaoGas': [vazao],
        'temperatura': [temperatura],
    }


def _parse_single_gas_lift_valve(id_valvula, fgl_mr2, comp_prod, comp_servico):
    """Parse a single gas lift valve (DESEMPENHO type)."""
    vgl_inst = fgl_mr2.get('vgl_inst', {})

    # Valve type: schema expects integer (0=orifício, 1=pressão, 2=Venturi)
    tipo_str = vgl_inst.get('@tipo', 'ORIFICIO')
    tipo_map = {'ORIFICIO': 0, 'PRESSAO': 1, 'VENTURI': 2}
    tipo_valvula = tipo_map.get(tipo_str, 0)

    # Diameters
    diam_orificio = vgl_inst.get('diam_orificio')
    diam_orificio = polegadas_to_metros(float(diam_orificio)) if diam_orificio else None

    diam_externo = vgl_inst.get('diam_externo')
    diam_externo = polegadas_to_metros(float(diam_externo)) if diam_externo else None

    # Calibration
    pcal_str = fgl_mr2.get('pcal', '0')
    pcal = kgf_por_cm2_manometrica_to_absoluta(float(pcal_str) if pcal_str else 0)
    pcal_psi = pcal * 14.223  # kgf/cm2 absolute -> psi
    tcal_str = fgl_mr2.get('tcal', '0')
    tcal = float(tcal_str) if tcal_str else 0
    tcal_f = tcal * 9.0 / 5.0 + 32.0  # °C -> °F

    # Area ratio
    razao_area = 1.0
    if tipo_valvula == 1:  # PRESSAO
        fator_r = vgl_inst.get('fator_r')
        if fator_r:
            razao_area = float(fator_r)

    # Recovery factor
    # tipo=0 (orifice): no bellows, frec must be 0 to avoid negative presGarg in simulator
    # tipo=1 (pressure/IPO): simulator forces frec=0 internally regardless of JSON
    # tipo=2 (Venturi): high pressure recovery = 0.96, but simulator also forces 0 internally
    if tipo_valvula == 2:
        frecupera = 0.96
    elif tipo_valvula == 0:
        frecupera = 0.0
    else:
        frecupera = 0.5

    return {
        'id': id_valvula,
        'ativo': True,
        'comprimentoMedidoServico': comp_servico,
        'comprimentoMedidoProducao': comp_prod,
        'tipoValvula': tipo_valvula,
        'diametroOrificio': diam_orificio,
        'cdvgl': 1.0,
        'cdvLiq': 1.0,
        'razaoArea': razao_area,
        'frecupera': frecupera,
        'pressaoCalibracao': pcal_psi,
        'temperaturaCalibracao': tcal_f,
        'diametroExterno': diam_externo,
    }


def _possui_fonte_gas_lift_ativa_nao_cega(tree, id_valvula):
    """Check if a valve is an active, non-blind DESEMPENHO gas lift valve."""
    node = tree.xpath(
        f"/marlim/entrada/gas_lift/valvula_instalada[@seq='{id_valvula}' and @operacao='DESEMPENHO']"
    )
    if not node:
        return False
    node_dict = xmltodict.parse(etree.tostring(node[0]))
    vgl_inst = node_dict['valvula_instalada'].get('vgl_inst', {})
    orificio = vgl_inst.get('orificio', '')
    if orificio and orificio.upper() == 'CEGA':
        return False
    # diam_orificio == 0 is equivalent to a blind valve
    try:
        diam = float(vgl_inst.get('diam_orificio', -1))
        if diam == 0:
            return False
    except (ValueError, TypeError):
        pass
    return True


def _parse_comprimento_servico_fora_poco(resultados, mr2_entrada):
    """Calculate total service line length outside the well."""
    total_servico = resultados.total_unidades_servico()
    if not total_servico:
        return None

    comprimento = 0
    node_injecao = mr2_entrada.get('sistema_producao', {}).get('injecao', {})

    for ind in range(total_servico):
        natureza = resultados.natureza_unidade_servico(ind)
        tipo_map = {2: 'linha_maritima_inj', 3: 'linha_aerea_inj', 4: 'linha_enterrada_inj'}
        tipo_duto = tipo_map.get(natureza)
        if not tipo_duto:
            continue

        node_tipo = node_injecao.get(tipo_duto)
        if not node_tipo:
            continue

        if isinstance(node_tipo, list):
            if ind >= len(node_tipo):
                break
            tipo_duto_seq = node_tipo[ind]
        else:
            tipo_duto_seq = node_tipo

        try:
            ul = float(tipo_duto_seq.get('medidas_inj', {}).get('ul', 0))
        except (TypeError, ValueError):
            ul = 0

        comprimento += ul

    return comprimento if comprimento > 0 else None
