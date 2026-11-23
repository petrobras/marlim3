"""
Parsers for ducts (production + service), cross-sections, and materials.

This is the most complex parser group — it handles geometry, layering,
initial conditions, and cross-section construction.
"""
import math

import xmltodict
from lxml import etree

from ._base import graus_to_radianos, get_float, kgf_por_cm2_manometrica_to_absoluta


def parse_dutos_and_sections(mr2_entrada, mr2_reader, resultados, correcao, formacao_data):
    """
    Parse all ducts and cross-sections, returning the data in MR3 JSON format.

    Returns a dict with keys:
        - dutosProducao: list of production duct dicts
        - dutosServico: list of service duct dicts
        - secaoTransversal: list of cross-section dicts
        - material: list of material dicts
        - configuracaoInicial.Formacao.Propriedades: list of formation property dicts
        - linhaGas: bool indicating if service line exists
    """
    tree = mr2_reader.tree
    total_producao = resultados.total_unidades_producao()
    total_servico = resultados.total_unidades_servico()
    injecao = mr2_entrada.get('sistema_producao', {}).get('injecao', {})

    # --- Validate: block conversion if any unit is linha enterrada ---
    for ind in range(total_producao):
        if resultados.natureza_unidade_producao(ind) == 4:
            raise ValueError(
                "Modelo de linha enterrada ainda não disponível no Marlim3"
            )
    for ind in range(total_servico):
        if resultados.natureza_unidade_servico(ind) == 4:
            raise ValueError(
                "Modelo de linha enterrada ainda não disponível no Marlim3"
            )

    dutos_producao = []
    dutos_servico = []
    secoes_transversais = []
    materiais = []
    formacao_propriedades = formacao_data.get('propriedades_existentes', [])

    id_duto_producao = 0
    id_duto_servico = 0

    # --- Parse service ducts outside well ---
    indices_valvula = resultados.indices_unidades_producao_valvulas()
    ultima_vgl_comp = _get_ultima_vgl_comprimento(
        tree, mr2_entrada, resultados, indices_valvula, total_producao
    )
    linha_gas = ultima_vgl_comp is not None and ultima_vgl_comp > 0

    # Compute base gas mass flow rate for service line
    vazao_massica_gas_base = _compute_vazao_massica_gas_base(
        resultados, mr2_entrada, linha_gas
    )

    if total_servico and total_servico > 0 and linha_gas:
        for ind_servico in range(total_servico):
            natureza = resultados.natureza_unidade_servico(ind_servico)
            if natureza not in {2, 3}:
                continue

            # Material (deduplicated)
            mat_tubo_raw, mat_iso_raw = _parse_materiais_servico_fora_poco(
                resultados, ind_servico, len(materiais)
            )
            id_mat_tubo = _find_or_add_material(materiais, mat_tubo_raw)
            has_iso = resultados.get_espessura_isolamento_servico(ind_servico) > 0
            id_mat_iso = _find_or_add_material(materiais, mat_iso_raw) if has_iso else None

            # Cross-section (deduplicated)
            di = resultados.get_diametro_interno_servico(ind_servico)
            rug = resultados.get_rugosidade_servico(ind_servico)
            esp_tubo = resultados.get_espessura_coluna_servico(ind_servico)
            esp_iso = resultados.get_espessura_isolamento_servico(ind_servico)

            camadas = [{'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_tubo, 'discretizacao': 1, 'idMaterial': id_mat_tubo}]
            if has_iso:
                camadas.append({'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_iso, 'discretizacao': 1, 'idMaterial': id_mat_iso})

            rotulo_corte = f'Corte_Serv_{ind_servico}'
            st = {
                'id': 0, 'rotulo': rotulo_corte, 'anular': False,
                'camadas': camadas, 'diametroInterno': di,
                'rugosidade': rug,
            }
            id_corte = _find_or_add_secao_transversal(secoes_transversais, st)

            # Duct
            duto = _build_duto_servico(
                ind_servico, id_duto_servico, id_corte, -1,
                resultados, correcao, injecao, tree, mr2_entrada, is_within_well=False
            )
            # Set gas mass flow rate for external service ducts
            duto['condicoesIniciais']['vazaoMassicaGas'] = [
                vazao_massica_gas_base, vazao_massica_gas_base
            ]
            dutos_servico.append(duto)

            id_duto_servico += 1

    # --- Parse ALL production ducts in MR2 order (platform → reservoir) ---
    comprimento_acumulado_producao = 0

    for ind in range(total_producao):
        natureza = resultados.natureza_unidade_producao(ind)

        # Skip non-duct units (BCS, special equipment, choke, etc.)
        if natureza in {6, 8, 9, 10}:
            continue

        comp_duto = resultados.comprimento_total_unidade_producao(ind)

        if natureza in {1, 2, 3}:
            # --- Outside-well duct (lines + riser) ---
            # Materials (deduplicated)
            mat_tubo_raw, mat_iso_raw = _parse_materiais_producao_fora_poco(resultados, ind, id_duto_producao)
            id_mat_tubo = _find_or_add_material(materiais, mat_tubo_raw)
            has_iso = resultados.get_espessura_isolamento(ind) > 0
            id_mat_iso = _find_or_add_material(materiais, mat_iso_raw) if has_iso else None

            # Cross-section (deduplicated)
            di = resultados.get_diametro_interno_producao(ind)
            rug = resultados.get_rugosidade_producao(ind)
            esp_tubo = resultados.get_espessura_coluna_producao(ind)
            esp_iso = resultados.get_espessura_isolamento(ind)

            camadas = [{'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_tubo, 'discretizacao': 1, 'idMaterial': id_mat_tubo}]
            if has_iso:
                camadas.append({'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_iso, 'discretizacao': 1, 'idMaterial': id_mat_iso})

            rotulo_corte = _parse_rotulo_corte(tree, resultados, ind, natureza)
            st = {
                'id': 0, 'rotulo': rotulo_corte, 'anular': False,
                'camadas': camadas, 'diametroInterno': di,
                'rugosidade': rug,
            }
            id_corte = _find_or_add_secao_transversal(secoes_transversais, st)

            # Duct
            duto = _build_duto_producao(
                ind, id_duto_producao, id_corte, -1,
                resultados, correcao, tree, mr2_entrada
            )
            dutos_producao.append(duto)
            comprimento_acumulado_producao += comp_duto
            id_duto_producao += 1

        elif natureza in {5, 7}:
            # --- Inside-well duct (poço com/sem coluna) ---
            # Formation properties for this well unit
            form_prop = _parse_formacao_propriedade(resultados, ind)
            form_id = _find_or_add_formacao(formacao_propriedades, form_prop)

            # Check coupling before building cross-section
            is_coupled = (linha_gas and ultima_vgl_comp
                          and (comprimento_acumulado_producao + comp_duto) <= ultima_vgl_comp)

            di = resultados.get_diametro_interno_producao(ind)
            rug = resultados.get_rugosidade_producao(ind)
            rotulo_corte = _parse_rotulo_corte(tree, resultados, ind, natureza)

            if is_coupled:
                # --- Coupled case: separate cross-sections for production and service ---

                # Production cross-section: only tubing + optional insulation
                mats_prod_raw = _parse_materiais_acoplado_producao(resultados, ind)
                mats_prod = []
                for m in mats_prod_raw:
                    mat_id = _find_or_add_material(materiais, m)
                    mats_prod.append({'id': mat_id, **{k: v for k, v in m.items() if k != 'id'}})
                camadas_prod = _build_camadas_acoplado_producao(resultados, ind, mats_prod)

                st_prod = {
                    'id': 0, 'rotulo': rotulo_corte, 'anular': False,
                    'camadas': camadas_prod, 'diametroInterno': di,
                    'rugosidade': rug,
                }
                id_corte_prod = _find_or_add_secao_transversal(secoes_transversais, st_prod)

                # Service cross-section: anular=True, casing + cement
                mats_serv_raw = _parse_materiais_acoplado_servico(resultados, ind)
                mats_serv = []
                for m in mats_serv_raw:
                    mat_id = _find_or_add_material(materiais, m)
                    mats_serv.append({'id': mat_id, **{k: v for k, v in m.items() if k != 'id'}})
                camadas_serv = _build_camadas_acoplado_servico(resultados, ind, mats_serv)

                esp_tubo = resultados.get_espessura_coluna_producao(ind)
                esp_iso = resultados.get_espessura_isolamento(ind)
                di_serv = di + 2 * esp_tubo + 2 * esp_iso
                de_serv = resultados.get_diametro_interno_revestimento(ind)

                st_serv = {
                    'id': 0, 'rotulo': f'{rotulo_corte}_inj', 'anular': True,
                    'camadas': camadas_serv, 'diametroInterno': di_serv,
                    'diametroExterno': de_serv, 'rugosidade': rug,
                }
                id_corte_serv = _find_or_add_secao_transversal(secoes_transversais, st_serv)

                id_corte = id_corte_prod
            else:
                # --- Uncoupled case: full well cross-section ---
                mats_well_raw = _parse_materiais_producao_dentro_poco(resultados, ind, 0, natureza)
                mats_well = []
                for m in mats_well_raw:
                    mat_id = _find_or_add_material(materiais, m)
                    mats_well.append({'id': mat_id, **{k: v for k, v in m.items() if k != 'id'}})
                camadas = _build_camadas_dentro_poco(resultados, ind, natureza, mats_well)

                st = {
                    'id': 0, 'rotulo': rotulo_corte, 'anular': False,
                    'camadas': camadas, 'diametroInterno': di,
                    'rugosidade': rug,
                }
                id_corte = _find_or_add_secao_transversal(secoes_transversais, st)

            # Production duct inside well
            comprimento_acumulado_producao += comp_duto

            # Coupled ducts get id_formacao=-1 (original behavior)
            duto_form_id = -1 if is_coupled else form_id
            duto = _build_duto_producao(
                ind, id_duto_producao, id_corte, duto_form_id,
                resultados, correcao, tree, mr2_entrada
            )
            dutos_producao.append(duto)

            # Coupled service duct inside well
            if is_coupled:
                # Mark production duct as thermally coupled
                duto['acoplamentoTermico'] = 1  # COM_ACOPLAMENTO
                duto['direcaoConveccao'] = 1    # LONGITUDINAL

                # Build coupled service duct with its own cross-section and CI
                ci_serv = _build_condicoes_iniciais_servico_dentro_poco(resultados, ind, mr2_entrada)

                # Compute vazaoMassicaGas for this internal service duct
                vazao_start = vazao_massica_gas_base
                vazao_end = vazao_massica_gas_base
                if indices_valvula and ind < len(indices_valvula) and indices_valvula[ind] > 0:
                    # This duct has a valve — subtract its injected flow from the end point
                    valve_idx = indices_valvula[ind] - 1
                    qgi_valve = resultados.vazao_gas_valvula(valve_idx)
                    dens_gas = _get_densidadeGas_producao(mr2_entrada)
                    vazao_end = _compute_vazao_massica_after_valve(
                        resultados, qgi_valve, dens_gas
                    )
                ci_serv['vazaoMassicaGas'] = [vazao_start, vazao_end]
                # Update base for next service duct downstream
                vazao_massica_gas_base = vazao_end

                duto_serv = _build_duto_servico(
                    ind, id_duto_servico, id_corte_serv, form_id,
                    resultados, correcao, injecao, tree, mr2_entrada, is_within_well=True,
                    id_duto_rotulo=id_duto_producao
                )
                duto_serv['condicoesIniciais'] = ci_serv
                dutos_servico.append(duto_serv)
                id_duto_servico += 1

            id_duto_producao += 1

    # Determine if linhaGas should be active
    if not dutos_servico:
        linha_gas = False

    return {
        'dutosProducao': dutos_producao,
        'dutosServico': dutos_servico,
        'secaoTransversal': secoes_transversais,
        'material': materiais,
        'formacaoPropriedades': formacao_propriedades,
        'linhaGas': linha_gas,
    }


# --- Duct building helpers ---

def _build_duto_producao(ind, id_duto, id_corte, id_formacao, resultados, correcao, tree, mr2_entrada):
    """Build a production duct dict."""
    natureza = resultados.natureza_unidade_producao(ind)
    angulo = _get_angulo_producao(resultados, ind)
    rotulo = _get_rotulo_duto_producao(natureza, id_duto)
    disc = _build_discretizacao(resultados, ind, is_producao=True)
    ambiente = _get_ambiente_externo_producao(natureza)
    ci = _build_condicoes_iniciais(resultados, ind, is_producao=True, mr2_entrada=mr2_entrada, ambiente_externo=ambiente)

    # dpdl = _get_correcao_index(correcao, resultados, ind, 'P')
    # dtdl = _get_correcao_index(correcao, resultados, ind, 'T')
    corr_mr2 = _get_correlacao_mr2(tree, resultados, ind)

    duto = {
        'id': id_duto,
        'idCorte': id_corte,
        'angulo': angulo,
        'rotulo': rotulo,
        'discretizacao': disc,
        'acoplamentoTermico': 0,  # SEM_ACOPLAMENTO
        'condicoesIniciais': ci,
        'direcaoConveccao': 0,  # TRANSVERSAL
    }

    if id_formacao != -1:
        duto['idFormacao'] = id_formacao

    # ambienteExterno only makes sense for lines/risers, not for well-type ducts
    if natureza not in {5, 7}:
        duto['ambienteExterno'] = ambiente

    duto['correlacaoMR2'] = corr_mr2 if corr_mr2 is not None else 6

    return duto


def _build_duto_servico(ind, id_duto, id_corte, id_formacao, resultados, correcao, injecao, tree, mr2_entrada, is_within_well=False, id_duto_rotulo=None):
    """Build a service duct dict."""
    if is_within_well:
        angulo = -1 * _get_angulo_producao(resultados, ind)
        natureza = resultados.natureza_unidade_producao(ind)
        rotulo = _get_rotulo_duto_servico(natureza, id_duto_rotulo if id_duto_rotulo is not None else id_duto, dentro_poco=True)
        ambiente = 0
        acoplamento = 1  # COM_ACOPLAMENTO
    else:
        angulo = _get_angulo_servico(resultados, ind, injecao, tree)
        natureza = resultados.natureza_unidade_servico(ind)
        rotulo = _get_rotulo_duto_servico(natureza, id_duto, dentro_poco=False)
        ambiente = _get_ambiente_externo_servico(natureza)
        acoplamento = 0  # SEM_ACOPLAMENTO

    disc = _build_discretizacao(resultados, ind, is_producao=is_within_well, injecao=injecao, tree=tree, is_servico=not is_within_well)
    ci = _build_condicoes_iniciais(resultados, ind, is_producao=is_within_well, mr2_entrada=mr2_entrada, ambiente_externo=ambiente)

    # dpdl = _get_correcao_index(correcao, resultados, ind, 'P')
    # dtdl = _get_correcao_index(correcao, resultados, ind, 'T')

    duto = {
        'id': id_duto,
        'idCorte': id_corte,
        'angulo': angulo,
        'rotulo': rotulo,
        'discretizacao': disc,
        'condicoesIniciais': ci,
    }
    if id_formacao != -1:
        duto['idFormacao'] = id_formacao
    # Only include non-default values (original uses exclude_defaults=True)
    if acoplamento != 0:  # SEM_ACOPLAMENTO is default
        duto['acoplamentoTermico'] = acoplamento
    if ambiente != 0:  # DEFINIDO_PELO_USUARIO is default
        duto['ambienteExterno'] = ambiente
    return duto



# --- Geometry helpers ---

def _get_angulo_producao(resultados, ind):
    """Get angle for a production unit (from simulation results, converted to radians)."""
    # MR2 stores all unit angles in a single semicolon-separated string in #ANG
    dados = resultados.resultado_simulacao_formatado.get('#ANG', [])
    if not dados:
        return 0.0
    try:
        vals = [v for v in dados[0].strip().split(';') if v.strip()]
        if ind >= len(vals):
            return 0.0
        return graus_to_radianos(float(vals[ind]))
    except (ValueError, IndexError):
        return 0.0


def _get_angulo_servico(resultados, ind, injecao, tree):
    """Get angle for a service unit."""
    natureza = resultados.natureza_unidade_servico(ind)
    tipo_map = {2: 'linha_maritima_inj', 3: 'linha_aerea_inj', 4: 'linha_enterrada_inj'}
    tipo = tipo_map.get(natureza)
    if not tipo:
        return 0.0

    try:
        node = injecao.get(tipo)
        if isinstance(node, list):
            node = node[ind] if ind < len(node) else node[0]
        ang = float(node.get('medidas_inj', {}).get('ang', 0))
        return graus_to_radianos(ang)
    except (TypeError, ValueError, KeyError, AttributeError):
        return 0.0


def _build_discretizacao(resultados, ind, is_producao=True, injecao=None, tree=None, is_servico=False):
    """Build discretization list for a duct."""
    if is_producao and not is_servico:
        ul = resultados.comprimento_total_unidade_producao(ind)
        # MR2 stores all cell counts in a single semicolon-separated string in #NTREC
        dados = resultados.resultado_simulacao_formatado.get('#NTREC', [])
        n_celulas = 10  # default
        if dados:
            try:
                vals = [v for v in dados[0].strip().split(';') if v.strip()]
                if ind < len(vals):
                    n_celulas = int(vals[ind])
            except (ValueError, IndexError):
                pass
    elif is_servico and injecao:
        natureza = resultados.natureza_unidade_servico(ind)
        tipo_map = {2: 'linha_maritima_inj', 3: 'linha_aerea_inj', 4: 'linha_enterrada_inj'}
        tipo = tipo_map.get(natureza, 'linha_maritima_inj')
        try:
            node = injecao.get(tipo)
            if isinstance(node, list):
                node = node[ind] if ind < len(node) else node[0]
            ul = float(node.get('medidas_inj', {}).get('ul', 100))
            n_celulas = int(node.get('medidas_inj', {}).get('ntrec', 10))
        except (TypeError, ValueError, KeyError, AttributeError):
            ul = 100
            n_celulas = 10
    else:
        ul = resultados.comprimento_total_unidade_producao(ind)
        n_celulas = 10

    if n_celulas < 3:
        n_celulas = 3

    comprimento_celula = ul / n_celulas
    return [{'nCelulas': n_celulas, 'comprimento': comprimento_celula}]


def _get_resultados_vector(resultados, ind, method_names):
    """Return first available vector from simulation results using method name fallbacks."""
    for method_name in method_names:
        method = getattr(resultados, method_name, None)
        if not callable(method):
            continue
        try:
            values = method(ind)
        except (TypeError, ValueError, IndexError):
            continue

        if values is None:
            continue
        if isinstance(values, (int, float)):
            return [float(values)]
        if isinstance(values, list):
            return values
        try:
            return list(values)
        except TypeError:
            continue
    return []


def _get_endpoint_pair(current_values, next_values, default_value):
    """Build [start, end] pair using original MR2 parser convention.

    Start: first point of current unit.
    End: first point of next unit when it exists; otherwise last point of current unit.
    """
    start = current_values[0] if current_values else default_value
    if next_values:
        end = next_values[0]
    elif current_values:
        end = current_values[-1]
    else:
        end = default_value
    return start, end


def _build_condicoes_iniciais(resultados, ind, is_producao=True, mr2_entrada=None, ambiente_externo=None):
    """Build initial conditions (two points: start and end of duct)."""
    if is_producao:
        total = resultados.total_unidades_producao()
        has_next = ind < (total - 1)

        temps_curr = _get_resultados_vector(resultados, ind, ['get_temperaturas_unidade_producao'])
        temps_next = _get_resultados_vector(resultados, ind + 1, ['get_temperaturas_unidade_producao']) if has_next else []

        pressoes_curr = _get_resultados_vector(resultados, ind, ['get_pressoes_unidade_producao'])
        pressoes_next = _get_resultados_vector(resultados, ind + 1, ['get_pressoes_unidade_producao']) if has_next else []

        temps_ext_curr = _get_resultados_vector(
            resultados,
            ind,
            ['get_temperatura_externa_unidade_producao'],
        )
        temps_ext_next = _get_resultados_vector(
            resultados,
            ind + 1,
            ['get_temperatura_externa_unidade_producao'],
        ) if has_next else []

        holdups_curr = _get_resultados_vector(
            resultados,
            ind,
            ['get_holdups_unidade_producao', 'get_hold_ups_unidade_producao'],
        )
        holdups_next = _get_resultados_vector(
            resultados,
            ind + 1,
            ['get_holdups_unidade_producao', 'get_hold_ups_unidade_producao'],
        ) if has_next else []

        ulss_curr = _get_resultados_vector(resultados, ind, ['get_ulss_unidade_producao'])
        ulss_next = _get_resultados_vector(resultados, ind + 1, ['get_ulss_unidade_producao']) if has_next else []

        ugss_curr = _get_resultados_vector(resultados, ind, ['get_ugss_unidade_producao'])
        ugss_next = _get_resultados_vector(resultados, ind + 1, ['get_ugss_unidade_producao']) if has_next else []
    else:
        total = resultados.total_unidades_servico()
        has_next = ind < (total - 1)

        temps_curr = _get_resultados_vector(resultados, ind, ['get_temperaturas_servico'])
        temps_next = _get_resultados_vector(resultados, ind + 1, ['get_temperaturas_servico']) if has_next else []

        pressoes_curr = _get_resultados_vector(resultados, ind, ['get_pressoes_servico'])
        pressoes_next = _get_resultados_vector(resultados, ind + 1, ['get_pressoes_servico']) if has_next else []

        temps_ext_curr = _get_resultados_vector(
            resultados,
            ind,
            ['get_temperatura_externa_unidade_servico'],
        )
        temps_ext_next = _get_resultados_vector(
            resultados,
            ind + 1,
            ['get_temperatura_externa_unidade_servico'],
        ) if has_next else []

        holdups_curr = []
        holdups_next = []
        ulss_curr = []
        ulss_next = []
        ugss_curr = []
        ugss_next = []

    t0, t1 = _get_endpoint_pair(temps_curr, temps_next, 20.0)
    p0_raw, p1_raw = _get_endpoint_pair(pressoes_curr, pressoes_next, 10.0)
    # MR2 pressures are gauge (manometric); convert to absolute for MR3
    p0 = kgf_por_cm2_manometrica_to_absoluta(p0_raw)
    p1 = kgf_por_cm2_manometrica_to_absoluta(p1_raw)
    # If external temperature vectors are unavailable, fallback to local fluid temperature
    # instead of forcing a cold 4.0 C profile.
    temp_ext_0, temp_ext_1 = _get_endpoint_pair(temps_ext_curr, temps_ext_next, t0)
    # MR2 returns 0.0 for external temperature on well accessories (packers, etc.)
    # that have no thermal data. Replace with fluid temperature as fallback.
    if temp_ext_0 == 0.0:
        temp_ext_0 = t0
    if temp_ext_1 == 0.0:
        temp_ext_1 = t1
    holdup_0, holdup_1 = _get_endpoint_pair(holdups_curr, holdups_next, 0.5)
    uls_0, uls_1 = _get_endpoint_pair(ulss_curr, ulss_next, 0.0)
    ugs_0, ugs_1 = _get_endpoint_pair(ugss_curr, ugss_next, 0.0)
    
    vel_ext_0 = _get_velocidade_externa(resultados, ind, is_producao, mr2_entrada)
    vel_ext_1 = vel_ext_0  # Same for both ends (environmental velocity)

    ci = {
        'compInter': [0, 1],
        'pressao': [p0, p1],
        'temp': [t0, t1],
        'holdup': [holdup_0, holdup_1] if is_producao else None,
        'bet': [0.0, 0.0] if is_producao else None,
        'ugs': [ugs_0, ugs_1] if is_producao else None,
        'uls': [uls_0, uls_1] if is_producao else None,
        'tempExterna': [temp_ext_0, temp_ext_1],
        'velExterna': [vel_ext_0, vel_ext_1],
    }

    # add external fluid properties for heat transfer
    ci['kExterna'] = [0.32, 0.32]
    ci['viscExterna'] = [0.001, 0.001]
    ci['rhoExterno'] = [1000.0, 1000.0]
    ci['calorEspecificoExterno'] = [4182.0, 4182.0]
    ci['vazaoMassicaGas'] = [0.0, 0.0]
    
    # Remove None values
    return {k: v for k, v in ci.items() if v is not None}


def _get_velocidade_externa(resultados, ind, is_producao, mr2_entrada):
    """
    Extract external velocity (wind/current) based on duct type.

    For production ducts: looks up per-duct by @seq.
    For service ducts: always uses the first entry (matching original converter).

    - Marine line (natureza=2): reads 'vmar'
    - Aerial line (natureza=3): reads 'vento'
    - Others: returns 0
    """
    try:
        if is_producao:
            natureza = resultados.natureza_unidade_producao(ind)
        else:
            natureza = resultados.natureza_unidade_servico(ind)

        if natureza == 2:  # Marítima (Marine)
            chave = 'linha_maritima' if is_producao else 'linha_maritima_inj'
            campo = 'vmar'
        elif natureza == 3:  # Aérea (Aerial)
            chave = 'linha_aerea' if is_producao else 'linha_aerea_inj'
            campo = 'vento'
        else:
            return 0.0

        section = 'producao' if is_producao else 'injecao'
        entrada_data = mr2_entrada.get('sistema_producao', {}).get(section, {})
        node = entrada_data.get(chave, {})
        # Normalize to list
        if isinstance(node, dict):
            node = [node]
        if not isinstance(node, list) or not node:
            return 0.0

        if is_producao:
            # Production: find entry by @seq
            seq = str(ind)
            entry = next((n for n in node if str(n.get('@seq', '')) == seq), None)
        else:
            # Service: always use first entry (matching original converter)
            entry = node[0]

        if entry is None:
            return 0.0
        vel = entry.get(campo)
        if vel:
            return float(vel)
        return 0.0

    except (TypeError, ValueError, KeyError, AttributeError, IndexError):
        return 0.0


# --- Gas mass flow rate helpers ---

def _get_densidadeGas_producao(mr2_entrada):
    """Get gas density from the production fluid entry data."""
    try:
        fluido = mr2_entrada.get('fluidos', {}).get('fluido', {})
        if isinstance(fluido, list):
            fluido = fluido[0]
        val = fluido.get('sgfog', 0)
        return float(val) if val else 0.0
    except (TypeError, ValueError, KeyError, AttributeError):
        return 0.0


def _compute_vazao_massica_gas_base(resultados, mr2_entrada, linha_gas):
    """Compute base gas mass flow rate for service ducts.

    Formula: (QGIT / 86400) * densidadeGas * 1.225
    Where:
        - QGIT: total injected gas volumetric flow (m³/day)
        - densidadeGas: relative gas density from production fluid
        - 1.225: air density reference factor (kg/m³)
    """
    if not linha_gas:
        return 0.0
    qgit = resultados.vazao_injecao_calculada(0)
    if not qgit or qgit == 0:
        return 0.0
    dens_gas = _get_densidadeGas_producao(mr2_entrada)
    if dens_gas == 0:
        return 0.0
    return (qgit / 86400.0) * dens_gas * 1.225


def _compute_vazao_massica_after_valve(resultados, qgi_valve, dens_gas):
    """Compute gas mass flow rate after a valve injection point.

    Formula: (QGIT - qgi_valve) / 86400 * densidadeGas * 1.225
    """
    qgit = resultados.vazao_injecao_calculada(0)
    if not qgit:
        return 0.0
    remaining = qgit - qgi_valve
    if remaining < 0:
        remaining = 0.0
    if dens_gas == 0:
        return 0.0
    return (remaining / 86400.0) * dens_gas * 1.225


def _get_ambiente_externo_producao(natureza):
    mapping = {1: 1, 2: 1, 3: 2, 5: -1, 7: -1}
    return mapping.get(natureza, -1)


def _get_ambiente_externo_servico(natureza):
    mapping = {2: 1, 3: 2, 5: 0, 7: 0}
    return mapping.get(natureza, 0)


def _get_rotulo_duto_producao(natureza, id_duto):
    mapping = {1: 'Riser', 2: 'LinhaM', 3: 'LinhaA', 4: 'LinhaE', 5: 'PocoComColuna', 7: 'PocoSemColuna'}
    tipo = mapping.get(natureza, 'Duto')
    return f'Prod_{tipo}_{id_duto}'


def _get_rotulo_duto_servico(natureza, id_duto, dentro_poco=False):
    if dentro_poco:
        mapping = {1: 'Riser', 5: 'PocoComColuna', 7: 'PocoSemColuna'}
    else:
        mapping = {2: 'LinhaM', 3: 'LinhaA', 4: 'LinhaE'}
    tipo = mapping.get(natureza, 'Duto')
    return f'Serv_{tipo}_{id_duto}'


def _get_correcao_index(correcao, resultados, ind, tipo):
    """Get correction factor index for a unit."""
    if not correcao:
        return 0
    if tipo == 'P':
        val = resultados.get_correcao_P(ind)
        lista = correcao.get('dPdLHidro', [])
    else:
        val = resultados.get_correcao_T(ind)
        lista = correcao.get('dTdL', [])

    if val is None or not lista:
        return 0
    try:
        return lista.index(val)
    except ValueError:
        return 0


# --- Correlation helpers ---

_CORR_MR2_MAP = {
    'POETTMANN_CARPENTER': 0,
    'BAXENDELL_THOMAS': 1,
    'FANCHER_BROWN': 2,
    'HAGEDORN_BROWN': 3,
    'DUNS_ROS': 4,
    'ORKISZEWSKI': 5,
    'BEGGS_BRILL': 6,
    'MUKHERJEE_BRILL': 7,
    'AZIZ': 8,
    'GRAY': 9,
    'OLIEMANS': 10,
    'DUKLER': 11,
    'BEGGS_BRILL_PALMER': 12,
    'DUKLER_EATON_FLANIGAN': 13,
    'DUKLER_MINAMI_I': 14,
    'DUKLER_MINAMI_II': 15,
    'SHOHAM': 16
}


def _get_correlacao_mr2(tree, resultados, ind_unidade):
    """Get the black-box multiphase correlation index for a production unit."""
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
            f'/marlim/entrada/sistema_producao/producao/{tipo}[@seq={saidaxorig}]/corr/@tipo'
        )
        if node:
            return _CORR_MR2_MAP.get(node[0].strip())
    except Exception:
        pass
    return None


# --- Material helpers ---

def _parse_materiais_producao_fora_poco(resultados, ind, id_duto):
    """Create material dicts for a production duct outside well."""
    cond_tubo = resultados.get_condutividade_coluna_producao(ind)
    mat_tubo = {
        'id': id_duto,
        'rotulo': f'Flowtec_{id_duto}',
        'tipo': 0,
        'condutividade': cond_tubo if cond_tubo else 58.0,
        'rho': 0,
        'calorEspecifico': 0.0,
    }

    cond_iso = resultados.get_condutividade_isolamento(ind)
    mat_iso = {
        'id': id_duto + 1,
        'rotulo': f'Flowtec_isol_{id_duto}',
        'tipo': 0,
        'condutividade': cond_iso if cond_iso else 0.15,
        'rho': 0,
        'calorEspecifico': 0.0,
    }
    return mat_tubo, mat_iso


def _parse_materiais_servico_fora_poco(resultados, ind, id_material):
    """Create material dicts for a service duct outside well."""
    cond_tubo = resultados.get_condutividade_coluna_servico(ind)
    mat_tubo = {
        'id': id_material,
        'rotulo': f'Flowtec_{id_material}',
        'tipo': 0,
        'condutividade': cond_tubo if cond_tubo else 58.0,
        'rho': 0,
        'calorEspecifico': 0.0,
    }

    cond_iso = resultados.get_condutividade_isolamento_servico(ind)
    mat_iso = {
        'id': id_material + 1,
        'rotulo': f'Flowtec_isol_{id_material}',
        'tipo': 0,
        'condutividade': cond_iso if cond_iso else 0.15,
        'rho': 0,
        'calorEspecifico': 0.0,
    }
    return mat_tubo, mat_iso


def _parse_materiais_producao_dentro_poco(resultados, ind, id_material, natureza):
    """Create material dicts for a well unit.
    
    Handles: tubing, insulation, annular fluid, casing, and cement.
    """
    materiais = []

    # Tubing (always present for nat 1, 5)
    if natureza in {1, 5}:
        cond = resultados.get_condutividade_coluna_producao(ind)
        materiais.append({
            'id': id_material + len(materiais),
            'rotulo': f'ColProd_{ind}',
            'tipo': 0,
            'condutividade': cond if cond else 58.0,
            'rho': 0,
            'calorEspecifico': 0.0,
        })

        # Insulation (optional)
        esp_iso = resultados.get_espessura_isolamento(ind)
        if esp_iso > 0:
            cond_iso = resultados.get_condutividade_isolamento_producao_poco(ind)
            materiais.append({
                'id': id_material + len(materiais),
                'rotulo': f'ColProd_isol_{ind}',
                'tipo': 0,
                'condutividade': cond_iso if cond_iso else 0.15,
                'rho': 0,
                'calorEspecifico': 0.0,
            })

        # Annular fluid (air, water, etc)
        tipo_fluido_anular = _get_tipo_fluido_anular(resultados, ind)
        materiais.append({
            'id': id_material + len(materiais),
            'rotulo': 'AR' if tipo_fluido_anular == 3 else ('AGUANULAR' if tipo_fluido_anular == 2 else f'Anular_{ind}'),
            'tipo': tipo_fluido_anular,
            'condutividade': _get_condutividade_anular(tipo_fluido_anular),
            'rho': _get_densidade_anular(tipo_fluido_anular),
            'calorEspecifico': _get_cp_anular(tipo_fluido_anular),
        })

    # Casing (revestimento)
    cond_rev = resultados.get_condutividade_revestimento_producao_poco(ind)
    materiais.append({
        'id': id_material + len(materiais),
        'rotulo': f'revestimento_{ind}',
        'tipo': 0,
        'condutividade': cond_rev if cond_rev else 58.0,
        'rho': 0,
        'calorEspecifico': 0.0,
    })

    # Cement (for nat 5 and 7)
    if natureza in {5, 7}:
        cond_cim = resultados.get_condutividade_cimento(ind)
        materiais.append({
            'id': id_material + len(materiais),
            'rotulo': f'cimento_{ind}',
            'tipo': 0,
            'condutividade': cond_cim if cond_cim else 0.7,
            'rho': 0,
            'calorEspecifico': 0.0,
        })

    return materiais


def _get_tipo_fluido_anular(resultados, ind):
    """Get annular fluid type mapped to MR3 MaterialTipo enum.

    MR2 #NANU values: 0=solid, 1=air, 2=water
    MR3 MaterialTipo:  0=SOLIDO, 3=AR, 2=AGUA
    """
    mr2_to_mr3 = {0: 0, 1: 3, 2: 2}  # MR2 → MR3 mapping
    try:
        tipo_mr2 = resultados.get_fluido_anular(ind)
        return mr2_to_mr3.get(tipo_mr2, 0)
    except (AttributeError, ValueError, IndexError):
        return 0  # Default to solid


def _get_condutividade_anular(tipo_fluido):
    """Get thermal conductivity of annular fluid.
    - Solid (0): 0.5 W/m.K (structural)
    - Air (3): 0.026 W/m.K
    - Water (2): 0.6 W/m.K
    """
    mapping = {0: 0.5, 3: 0.026, 2: 0.6}
    return mapping.get(tipo_fluido, 0.5)


def _get_densidade_anular(tipo_fluido):
    """Get density of annular fluid.
    - Solid (0): 1000 kg/m³
    - Air (3): 1.225 kg/m³
    - Water (2): 1000 kg/m³
    """
    mapping = {0: 1000.0, 3: 1.225, 2: 1000.0}
    return mapping.get(tipo_fluido, 1000.0)


def _get_cp_anular(tipo_fluido):
    """Get specific heat of annular fluid.
    - Solid (0): 500 J/kg.K
    - Air (3): 1005 J/kg.K
    - Water (2): 4182 J/kg.K
    """
    mapping = {0: 500.0, 3: 1005.0, 2: 4182.0}
    return mapping.get(tipo_fluido, 500.0)


def _build_camadas_dentro_poco(resultados, ind, natureza, materiais):
    """Build cross-section layers for a well unit."""
    camadas = []
    mat_idx = 0

    if natureza in {1, 5}:
        # Tubing layer
        esp_tubo = resultados.get_espessura_coluna_producao(ind)
        camadas.append({
            'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_tubo,
            'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
        })
        mat_idx += 1

        # Insulation (optional)
        esp_iso = resultados.get_espessura_isolamento(ind)
        if esp_iso > 0:
            camadas.append({
                'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_iso,
                'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
            })
            mat_idx += 1

        # Annular gap (diameter-based)
        di_rev = resultados.get_diametro_interno_revestimento(ind)
        camadas.append({
            'tipoMedicaoCamada': 'DIAMETRO', 'diametro': di_rev,
            'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
        })
        mat_idx += 1

        # Casing
        esp_rev = resultados.get_espessura_revestimento(ind)
        camadas.append({
            'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_rev,
            'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
        })
        mat_idx += 1

        # Cement (for nat 5)
        if natureza == 5:
            esp_cim = resultados.get_espessura_cimento(ind)
            if esp_cim > 0 and mat_idx < len(materiais):
                camadas.append({
                    'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_cim,
                    'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
                })

    elif natureza == 7:
        # No tubing — directly casing + cement
        esp_rev = resultados.get_espessura_revestimento(ind)
        camadas.append({
            'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_rev,
            'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
        })
        mat_idx += 1

        esp_cim = resultados.get_espessura_cimento(ind)
        if esp_cim > 0 and mat_idx < len(materiais):
            camadas.append({
                'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_cim,
                'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
            })

    return camadas


# --- Coupled duct helpers (gas-lift annular) ---

def _parse_materiais_acoplado_producao(resultados, ind):
    """Materials for the PRODUCTION side of a coupled well cross-section.

    The original converter uses rho=0, cp=0 for coupled materials — only the
    conductivity matters for the thermal coupling calculation.

    Returns list of material dicts: [tubing, (optional insulation)].
    """
    mats = []
    cond_tubo = resultados.get_condutividade_coluna_producao(ind)
    mats.append({
        'id': 0,
        'rotulo': f'ColProd_{ind}',
        'tipo': 0,
        'condutividade': cond_tubo if cond_tubo else 58.0,
        'rho': 0,
        'calorEspecifico': 0.0,
    })
    esp_iso = resultados.get_espessura_isolamento(ind)
    if esp_iso > 0:
        cond_iso = resultados.get_condutividade_isolamento_producao_poco(ind)  # #XKI
        mats.append({
            'id': 0,
            'rotulo': f'ColProd_isol_{ind}',
            'tipo': 0,
            'condutividade': cond_iso if cond_iso else 0.15,
            'rho': 0,
            'calorEspecifico': 0.0,
        })
    return mats


def _parse_materiais_acoplado_servico(resultados, ind):
    """Materials for the SERVICE (annular) side of a coupled well cross-section.

    Returns list: [casing, cement].
    """
    cond_rev = resultados.get_condutividade_revestimento_producao_poco(ind)  # #XKR
    cond_cim = resultados.get_condutividade_cimento(ind)      # #XKC
    return [
        {
            'id': 0,
            'rotulo': f'revestimento_{ind}',
            'tipo': 0,
            'condutividade': cond_rev if cond_rev else 58.0,
            'rho': 0,
            'calorEspecifico': 0.0,
        },
        {
            'id': 0,
            'rotulo': f'cimento_{ind}',
            'tipo': 0,
            'condutividade': cond_cim if cond_cim else 0.7,
            'rho': 0,
            'calorEspecifico': 0.0,
        },
    ]


def _build_camadas_acoplado_producao(resultados, ind, materiais):
    """Build cross-section layers for the production side when coupled.

    Only tubing + optional insulation (no annular gap, casing or cement).
    """
    camadas = []
    mat_idx = 0
    esp_tubo = resultados.get_espessura_coluna_producao(ind)
    camadas.append({
        'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_tubo,
        'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
    })
    mat_idx += 1
    esp_iso = resultados.get_espessura_isolamento(ind)
    if esp_iso > 0 and mat_idx < len(materiais):
        camadas.append({
            'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_iso,
            'discretizacao': 1, 'idMaterial': materiais[mat_idx]['id'],
        })
    return camadas


def _build_camadas_acoplado_servico(resultados, ind, materiais):
    """Build cross-section layers for the service (annular) side when coupled.

    Casing (revestimento) + cement layers.
    """
    esp_rev = resultados.get_espessura_revestimento(ind)
    esp_cim = resultados.get_espessura_cimento(ind)
    camadas = [
        {
            'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_rev,
            'discretizacao': 1, 'idMaterial': materiais[0]['id'],
        },
    ]
    if esp_cim > 0:
        camadas.append({
            'tipoMedicaoCamada': 'ESPESSURA', 'espessura': esp_cim,
            'discretizacao': 1, 'idMaterial': materiais[1]['id'],
        })
    return camadas


def _build_condicoes_iniciais_servico_dentro_poco(resultados, ind, mr2_entrada):
    """Build initial conditions for a service duct inside the well.

    Uses service-by-production data (#TInjProd / #PInjProd) when available,
    falling back to production data.  External temperature comes from the
    production line.  velExterna is always 0.0 inside the well.
    """
    total = resultados.total_unidades_producao()
    has_next = ind < (total - 1)
    total_servico = resultados.total_unidades_servico()

    # Temperature — prefer service-by-production data
    if total_servico:
        temps_curr = _get_resultados_vector(resultados, ind, ['get_temperaturas_servico_pela_producao'])
        temps_next = _get_resultados_vector(resultados, ind + 1, ['get_temperaturas_servico_pela_producao']) if has_next else []
    else:
        temps_curr = []
        temps_next = []
    # Fallback to production if service data unavailable
    if not temps_curr:
        temps_curr = _get_resultados_vector(resultados, ind, ['get_temperaturas_unidade_producao'])
        temps_next = _get_resultados_vector(resultados, ind + 1, ['get_temperaturas_unidade_producao']) if has_next else []

    # Pressure — prefer service-by-production data
    if total_servico:
        press_curr = _get_resultados_vector(resultados, ind, ['get_pressoes_servico_pela_producao'])
        press_next = _get_resultados_vector(resultados, ind + 1, ['get_pressoes_servico_pela_producao']) if has_next else []
    else:
        press_curr = []
        press_next = []
    pressure_is_gauge = False
    if not press_curr:
        press_curr = _get_resultados_vector(resultados, ind, ['get_pressoes_unidade_producao'])
        press_next = _get_resultados_vector(resultados, ind + 1, ['get_pressoes_unidade_producao']) if has_next else []
        pressure_is_gauge = True  # #P is gauge; needs conversion

    # External temperature always from production line
    temps_ext_curr = _get_resultados_vector(resultados, ind, ['get_temperatura_externa_unidade_producao'])
    temps_ext_next = _get_resultados_vector(resultados, ind + 1, ['get_temperatura_externa_unidade_producao']) if has_next else []

    t0, t1 = _get_endpoint_pair(temps_curr, temps_next, 20.0)
    p0_raw, p1_raw = _get_endpoint_pair(press_curr, press_next, 10.0)
    # service-by-production already returns absolute; production #P is gauge
    if pressure_is_gauge:
        p0 = kgf_por_cm2_manometrica_to_absoluta(p0_raw)
        p1 = kgf_por_cm2_manometrica_to_absoluta(p1_raw)
    else:
        p0, p1 = p0_raw, p1_raw
    temp_ext_0, temp_ext_1 = _get_endpoint_pair(temps_ext_curr, temps_ext_next, t0)
    # MR2 returns 0.0 for external temperature on well accessories (packers, etc.)
    # that have no thermal data. Replace with fluid temperature as fallback.
    if temp_ext_0 == 0.0:
        temp_ext_0 = t0
    if temp_ext_1 == 0.0:
        temp_ext_1 = t1

    return {
        'compInter': [0, 1],
        'pressao': [p0, p1],
        'temp': [t0, t1],
        'tempExterna': [temp_ext_0, temp_ext_1],
        'velExterna': [0.0, 0.0],
        'kExterna': [0.0, 0.0],
        'viscExterna': [0.0, 0.0],
        'rhoExterno': [0.0, 0.0],
        'calorEspecificoExterno': [0.0, 0.0],
        'vazaoMassicaGas': [0.0, 0.0],
    }




def _parse_formacao_propriedade(resultados, ind):
    """Parse formation thermal properties for a well unit."""
    massa_especifica = 2000.0
    difusividade = 1e-6
    calor_especifico = 1000.0
    condutividade = resultados.get_condutividade_formacao(ind)

    if condutividade and condutividade != 0:
        difusividade = resultados.get_difusividade(ind)
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


def _find_or_add_formacao(formacao_list, formacao_prop):
    """Find existing formation with same properties or add new one."""
    for i, existing in enumerate(formacao_list):
        if (existing['condutividade'] == formacao_prop['condutividade'] and
            existing['calorEspecifico'] == formacao_prop['calorEspecifico'] and
            existing['massaEspecifica'] == formacao_prop['massaEspecifica']):
            return i
    new_id = len(formacao_list)
    new_formacao = {
        'id': new_id,
        'condutividade': formacao_prop['condutividade'],
        'calorEspecifico': formacao_prop['calorEspecifico'],
        'massaEspecifica': formacao_prop['massaEspecifica'],
        'rotulo': f'Rotulo_MR2_{new_id}',
    }
    formacao_list.append(new_formacao)
    return new_id


# --- Cross-section label helper ---

def _parse_rotulo_corte(tree, resultados, ind, natureza):
    """Get cross-section label from MR2 XML."""
    mapping = {
        1: 'riser', 2: 'linha_maritima', 3: 'linha_aerea',
        4: 'linha_enterrada', 5: 'poco_com_coluna', 7: 'poco_sem_coluna',
    }
    tipo = mapping.get(natureza)
    if not tipo:
        return f'Corte_{ind}'

    saidaxorig = resultados.identificadores_unidade_producao(ind)
    if saidaxorig is None:
        return f'Corte_{ind}'

    nome_nodes = tree.xpath(
        f'/marlim/entrada/sistema_producao/producao/{tipo}[@seq={saidaxorig}]/nome'
    )
    try:
        if nome_nodes and nome_nodes[0].text:
            return f'Corte_{nome_nodes[0].text}'
    except (AttributeError, TypeError):
        pass
    return f'Corte_{ind}'


# --- Gas lift position helper ---

def _get_ultima_vgl_comprimento(tree, mr2_entrada, resultados, indices_valvula, total_producao):
    """Get the accumulated production length at the last active gas lift valve."""
    if not indices_valvula:
        return None

    comprimento_acum = 0
    ultima_comp = None

    for i in range(total_producao):
        comprimento_acum += resultados.comprimento_total_unidade_producao(i)
        if indices_valvula[i] > 0:
            id_valvula = indices_valvula[i] - 1
            # Check if it's an active DESEMPENHO valve
            node = tree.xpath(
                f"/marlim/entrada/gas_lift/valvula_instalada[@seq={id_valvula} and @operacao='DESEMPENHO']"
            )
            if node:
                node_dict = xmltodict.parse(etree.tostring(node[0]))
                vgl_inst = node_dict['valvula_instalada'].get('vgl_inst', {})
                orificio = vgl_inst.get('orificio', '')
                if orificio and orificio.upper() == 'CEGA':
                    continue
                # diam_orificio == 0 is equivalent to a blind valve
                try:
                    diam = float(vgl_inst.get('diam_orificio', -1))
                    if diam == 0:
                        continue
                except (ValueError, TypeError):
                    pass
                ultima_comp = comprimento_acum

    return ultima_comp


# --- Deduplication helpers ---

def _valores_iguais(a, b, rel_tol=1e-6):
    """Compare two numeric values for near-equality, handling None."""
    if a is None and b is None:
        return True
    if a is None or b is None:
        return False
    return math.isclose(a, b, rel_tol=rel_tol)


def _find_or_add_material(materiais, material_novo):
    """Find an existing material with identical properties, or add a new one.

    Mirrors the original converter's ``adiciona_material`` deduplication:
    compares condutividade, tipo, rho, calorEspecifico, visc, and beta.
    Uses rel_tol=1e-6 to match the original converter's tolerance.
    Returns the id of the (possibly reused) material.
    """
    for mat in materiais:
        if (
            _valores_iguais(mat.get('condutividade'), material_novo.get('condutividade'), rel_tol=1e-6)
            and mat.get('tipo') == material_novo.get('tipo')
            and _valores_iguais(mat.get('rho'), material_novo.get('rho'), rel_tol=1e-6)
            and _valores_iguais(mat.get('calorEspecifico'), material_novo.get('calorEspecifico'), rel_tol=1e-6)
            and _valores_iguais(mat.get('visc', 0), material_novo.get('visc', 0), rel_tol=1e-6)
            and _valores_iguais(mat.get('beta', 0), material_novo.get('beta', 0), rel_tol=1e-6)
        ):
            return mat['id']

    material_novo['id'] = len(materiais)
    materiais.append(material_novo)
    return material_novo['id']


def _camadas_iguais(camadas_a, camadas_b):
    """Compare two lists of layer dicts for equality.

    Uses exact float equality (==) to match the original converter's
    Camada.__eq__ behavior.
    """
    if len(camadas_a) != len(camadas_b):
        return False
    for a, b in zip(camadas_a, camadas_b):
        if a.get('tipoMedicaoCamada') != b.get('tipoMedicaoCamada'):
            return False
        if a.get('idMaterial') != b.get('idMaterial'):
            return False
        if a.get('discretizacao') != b.get('discretizacao'):
            return False
        # Compare the dimension value using exact equality (matching original)
        val_a = a.get('espessura', a.get('diametro'))
        val_b = b.get('espessura', b.get('diametro'))
        if val_a != val_b:
            return False
    return True


def _find_or_add_secao_transversal(secoes, secao_nova):
    """Find an existing cross-section with identical properties, or add a new one.

    Mirrors the original converter's ``adiciona_secao_transversal`` deduplication:
    compares anular, diametroInterno, rugosidade, camadas, and diametroExterno (only when anular=True).
    Uses exact float equality (==) to match the original converter's
    SecaoTransversal.__eq__ behavior.
    Returns the id of the (possibly reused) cross-section.
    """
    for st in secoes:
        if (
            st.get('anular') == secao_nova.get('anular')
            and st.get('diametroInterno') == secao_nova.get('diametroInterno')
            and st.get('diametroExterno') == secao_nova.get('diametroExterno')
            and st.get('rugosidade') == secao_nova.get('rugosidade')
            and _camadas_iguais(st.get('camadas', []), secao_nova.get('camadas', []))
        ):
            return st['id']

    secao_nova['id'] = len(secoes)
    secoes.append(secao_nova)
    return secao_nova['id']
