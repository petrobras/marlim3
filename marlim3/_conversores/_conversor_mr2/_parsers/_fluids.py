"""Parsers for fluid-related fields: fluidosProducao and fluidoGas."""
import xmltodict
from lxml import etree

from ._base import get_float


def parse_fluido_producao(mr2_reader, resultados, tree, proximo_id=0):
    """Parse production fluid from MR2 to MR3 dict format."""
    fonte = _get_fonte_ativa(tree)
    if not fonte:
        return None

    fluido = _get_fluido_associado(tree, fonte)
    if not fluido:
        return None

    # ASTM values: only populated if viscosity model is ASTM
    astm_values = {}
    id_fluido = fonte.get('@id_fluido')
    _update_astm_values_from_xml(tree, id_fluido, astm_values)

    tipo_razao = (fonte.get('razao_fluido', {}).get('@tipo', '') or 'RGO').upper()
    api = get_float(fluido, 'api', 36.0)
    densidade_gas = get_float(fluido, 'sgfog', 0.67)
    densidade_agua = get_float(fluido, 'sgw', 0.999141575)
    fracao_co2 = get_float(fluido, 'yco2', 0.0)
    rgo = get_float(fonte.get('razao_fluido', {}), 'rgof', 370.0)
    bsw = get_float(fonte.get('razao_fluido', {}), 'bsw', 0.0) / 100

    rgl_val = fonte.get('razao_fluido', {}).get('rglf')
    rgl = float(rgl_val) if rgl_val else rgo * (1 - bsw)

    correlacao_critica = 1

    # PVT correlations
    tipo_pvt = fluido.get('pvt', {}).get('@tipo', '').upper()
    path_corr = 'pvt/analise_pvt' if tipo_pvt == 'ANALISE' else 'pvt'
    id_fluido = fonte.get('@id_fluido')

    modelo_rs_pb = _parse_modelo_rs_pb(tree, id_fluido, path_corr)
    modelo_oleo_morto = _parse_modelo_oleo_morto(tree, id_fluido, path_corr, astm_values)
    modelo_oleo_vivo = _parse_modelo_oleo_vivo(tree, id_fluido, path_corr)
    modelo_oleo_subsaturado = _parse_modelo_oleo_subsaturado(tree, id_fluido, path_corr)
    tipo_emulsao = _parse_tipo_emulsao(fluido)
    bsw_corte = _parse_bsw_corte(fluido, tipo_emulsao)
    coef_a = _parse_coef_exponencial(tree, id_fluido, 'coef_a', 6.42)
    coef_b = _parse_coef_exponencial(tree, id_fluido, 'coef_b', 0.53)

    result = {
        'id': proximo_id,
        'rotulo': 'fluido_MR2',
        'ativo': True,
        'api': api,
        'rgo': rgo,
        'rgl': rgl,
        'densidadeGas': densidade_gas,
        'bsw': bsw,
        'densidadeAgua': densidade_agua,
        'tipoEmul': tipo_emulsao,
        'fracCO2': fracao_co2,
        'correlacaoCritica': correlacao_critica,
        'coefAModeloExp': coef_a,
        'coefBModeloExp': coef_b,
        'modeloRsPb': modelo_rs_pb,
        'modeloOleoMorto': modelo_oleo_morto,
        'modeloOleoVivo': modelo_oleo_vivo,
        'modeloOleoSubSaturado': modelo_oleo_subsaturado,
        'modeloViscBlackOil': 0,  # TABELA_FLASH
        'modeloAguaBlackOil': 1,  # BLACK_OIL
        'fracMolar': [0.0],
    }

    # Only include ASTM values if viscosity model is ASTM
    if astm_values:
        result['temp1'] = astm_values['temp1']
        result['visc1'] = astm_values['visc1']
        result['temp2'] = astm_values['temp2']
        result['visc2'] = astm_values['visc2']

    # Only include bswCorte if emulsion is active
    if bsw_corte is not None:
        result['bswCorte'] = bsw_corte
    return result


def parse_fluido_gas(mr2_reader, resultados, tree):
    """Parse gas fluid from MR2 to MR3 dict format."""
    # Try gas_lift/sgig first, then fall back to fluido/sgig (injection gas SG)
    densidade_gas = 0.0
    for xpath in ['/marlim/entrada/gas_lift/sgig',
                  '/marlim/entrada/fluidos/fluido/sgig']:
        nodes = tree.xpath(xpath)
        if nodes:
            try:
                texto = nodes[0].text
                val = float(texto) if texto else 0.0
                if val > 0:
                    densidade_gas = val
                    break
            except (ValueError, TypeError):
                continue

    return {
        'ativo': True,
        'densidadeGas': densidade_gas,
        'fracCO2': 0.0,
        'correlacaoCritica': 1,  # BROWN_ET_AL
        'fracMolarUsuario': False,
        'fracMolar': [0.0],
    }


# --- Helper functions ---

def _get_fonte_ativa(tree):
    fonte = tree.xpath(
        "/marlim/entrada/sistema_producao/producao/fonte[@ativo='TRUE']"
    )
    if not fonte:
        return None
    return xmltodict.parse(etree.tostring(fonte[0]))['fonte']


def _get_fluido_associado(tree, fonte):
    id_fluido = fonte.get('@id_fluido')
    if not id_fluido:
        return None
    fluido = tree.xpath(f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']")
    if not fluido:
        return None
    return xmltodict.parse(etree.tostring(fluido[0]))['fluido']


def _parse_modelo_rs_pb(tree, id_fluido, path_corr):
    try:
        xpath = f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/{path_corr}/correlacoes/razao_solubilidade/icode/@tipo"
        tipo = tree.xpath(xpath)
        mapa = {'VAZQUEZ_BEGGS': 0, 'VASQUEZ_BEGGS': 0, 'LASATER': 1, 'STANDING': 2, 'GLASO': 3}
        return mapa.get(tipo[0].strip().upper(), 0) if tipo else 0
    except Exception:
        return 0


def _parse_modelo_oleo_morto(tree, id_fluido, path_corr, astm_values):
    try:
        caminho = f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/{path_corr}/correlacoes/viscosidade"
        tipo = tree.xpath(f'{caminho}/lcode/@tipo')
        mapa = {
            'ASTM': 0, 'BEGGS_ROBINSON': 1, 'BEGGS_ROBINSON_MODIFICADO': 2,
            'GLASO': 3, 'KARTOATMODJO_SCHMIDT': 4, 'PETROSKY_FARSHAD': 5, 'BEAL': 6,
        }
        return mapa.get(tipo[0].strip().upper(), 0) if tipo else 0
    except Exception:
        return 0


def _parse_modelo_oleo_vivo(tree, id_fluido, path_corr):
    try:
        xpath = f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/{path_corr}/correlacoes/viscosidade/oleo_vivo/@sat"
        tipo = tree.xpath(xpath)
        mapa = {'BEGGS_ROBINSON': 0, 'CAMPOS': 0, 'KARTOATMODJO_SCHMIDT': 1, 'PETROSKY_FARSHAD': 2}
        return mapa.get(tipo[0].strip().upper(), 0) if tipo else 0
    except Exception:
        return 0


def _parse_modelo_oleo_subsaturado(tree, id_fluido, path_corr):
    try:
        xpath = f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/{path_corr}/correlacoes/viscosidade/oleo_vivo/@subsat"
        tipo = tree.xpath(xpath)
        mapa = {'VAZQUEZ_BEGGS': 0, 'VASQUEZ_BEGGS': 0, 'KARTOATMODJO_SCHMIDT': 1, 'PETROSKY_FARSHAD': 2, 'BEAL': 3, 'KHAN': 4}
        return mapa.get(tipo[0].strip().upper(), 0) if tipo else 0
    except Exception:
        return 0


def _parse_tipo_emulsao(fluido):
    try:
        if fluido.get('@iemul', '').strip().upper() not in ['EMULSAO', 'TRUE']:
            return 0
        tipo = fluido.get('emulsao', {}).get('@tipo', '').strip().upper()
        mapa = {
            'WOELFLIN_FRACA': 1, 'WOELFLIN_MEDIA': 2, 'CAMPOS': 2,
            'RONNINGSEN': 2, 'OLEO_VIVO': 2, 'WOELFLIN_FORTE': 3,
            'EXPONENCIAL': 4,
        }
        return mapa.get(tipo, 0)
    except Exception:
        return 0


def _parse_bsw_corte(fluido, tipo_emulsao):
    if tipo_emulsao > 0:
        fwre = fluido.get('emulsao', {}).get('fwre')
        if fwre is not None:
            try:
                return float(fwre)
            except (ValueError, TypeError):
                pass
        return None
    return None


def _parse_coef_exponencial(tree, id_fluido, coef_tag, default):
    try:
        val = tree.xpath(
            f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/emulsao/{coef_tag}"
        )
        if val and hasattr(val[0], 'text') and val[0].text:
            return float(val[0].text.strip())
    except (ValueError, TypeError):
        pass
    return default


def _update_astm_values_from_xml(tree, id_fluido, astm_values):
    """Populate ASTM correlation values (temp1, visc1, temp2, visc2) from XML.
    
    Only populates astm_values dict if the viscosity model is ASTM.
    If not ASTM, the dict remains empty and no temp/visc fields are output.
    """
    if not id_fluido:
        return
    
    try:
        # Determine PVT type (analise or direct)
        tipo_pvt_nodes = tree.xpath(f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/pvt/@tipo")
        tipo_pvt = tipo_pvt_nodes[0].upper() if tipo_pvt_nodes else 'DIRETO'
        
        if tipo_pvt == 'ANALISE':
            path_corr = 'pvt/analise_pvt'
        else:
            path_corr = 'pvt'
        
        # Get viscosity model info
        visc_model_nodes = tree.xpath(
            f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/{path_corr}/correlacoes/viscosidade/lcode/@tipo"
        )
        
        if visc_model_nodes and visc_model_nodes[0].strip().upper() == 'ASTM':
            # Populate ASTM values from XML
            caminho = f"/marlim/entrada/fluidos/fluido[@id='{id_fluido}']/{path_corr}/correlacoes/viscosidade"
            for tag, key in [('t1', 'temp1'), ('visd1', 'visc1'), ('t2', 'temp2'), ('visd2', 'visc2')]:
                val_nodes = tree.xpath(f'{caminho}/{tag}/text()')
                if val_nodes:
                    try:
                        astm_values[key] = float(val_nodes[0].strip())
                    except (ValueError, TypeError):
                        pass
    except Exception:
        pass
