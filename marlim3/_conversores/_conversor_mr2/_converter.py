"""
Main converter orchestrator: MR2 (.mr2 XML) → MR3 (JSON dict).

This module coordinates reading the MR2 file, optionally running the legacy
simulator, and calling all domain parsers to produce a complete MR3 JSON dict
compatible with Tramo.from_json().
"""
import json
import os

from ._mr2_reader import Mr2Reader, Mr2SimulationResults
from ._mr2_simulator import Mr2Simulator
from ._parsers._base import kgf_por_cm2_manometrica_to_absoluta
from ._parsers._boundary import parse_condicao_pressao, parse_fonte_liquido, parse_ipr
from ._parsers._ducts import parse_dutos_and_sections
from ._parsers._fluids import parse_fluido_gas, parse_fluido_producao
from ._parsers._pumps import parse_bcs_list
from ._parsers._results import parse_tendencias_producao
from ._parsers._sources import (
    parse_fonte_gas_lift_list,
    parse_fonte_gas_list,
    parse_gas_injecao,
)
from ._parsers._thermal import (
    parse_correcao,
    parse_correlacoes_escorregamento,
    parse_formacao,
)
from ._parsers._valves import parse_master1, parse_master2, parse_separador


def converter_mr2_para_json(mr2_path, pvt_path=None, mr2_binary_path=None, simula_mr2=False):
    """
    Convert an MR2 file to a MR3 JSON dict (compatible with Tramo.from_json).

    Args:
        mr2_path: Path to the .mr2 file.
        pvt_path: Optional path to a PVT table file (.tab/.ctm).
                  If None, uses the bundled ANP2.tab.
        mr2_binary_path: Optional path to the legacy MR2 simulator binary.
                         Required only when the .mr2 file lacks embedded results.
        simula_mr2: If True, forces re-simulation even if results exist.

    Returns:
        dict: A complete MR3 JSON dict that can be passed to Tramo.from_json(data, is_string=True).

    Raises:
        FileNotFoundError: If the MR2 file doesn't exist or binary is needed but not found.
        RuntimeError: If simulation fails.
        ValueError: If the MR2 file has no results and no binary path is provided.
    """
    # Resolve PVT path
    if pvt_path is None:
        pvt_path = os.path.join(
            os.path.dirname(__file__), '_resources', 'ANP2.tab'
        )

    # Read MR2 file
    reader = Mr2Reader(mr2_path)

    # Simulate if needed
    if simula_mr2 or not reader.possui_resultados:
        if not mr2_binary_path:
            raise ValueError(
                "O arquivo .mr2 não possui resultados embutidos. "
                "Forneça mr2_binary_path para executar a simulação MR2."
            )
        simulator = Mr2Simulator(reader, pvt_path, mr2_binary_path)
        simulator.executar()

    # Parse simulation results
    resultados = Mr2SimulationResults(reader)

    # Load template
    template_path = os.path.join(
        os.path.dirname(__file__), '_resources', 'templateMR2toMRT.json'
    )
    with open(template_path, 'r', encoding='utf-8') as f:
        mr3 = json.load(f)

    # System type
    mr3['sistema'] = 'MULTIFASICO'

    # Get entry data
    mr2_entrada = reader.mr2_entrada
    tree = reader.tree

    # --- Apply parsers ---

    # Correction factors — DISABLED: MR2 correction factors don't translate
    # correctly to MR3's thermal model (sentinels, extreme/negative values).
    # correcao = parse_correcao(resultados)
    # if correcao:
    #     mr3['correcao'] = correcao
    correcao = None

    # Master1 (wellhead valve)
    master1 = parse_master1(mr2_entrada, resultados)
    mr3['master1'] = master1

    # IPR
    ipr = parse_ipr(mr2_entrada, tree, resultados)
    if ipr:
        mr3['ipr'] = [ipr]

    # Fonte de líquido (only if source is FONTE_MASSA, not RESERVATORIO)
    if not ipr:
        fonte_liquido = parse_fonte_liquido(mr2_entrada, tree, resultados)
        if fonte_liquido:
            mr3['fonteLiquido'] = [fonte_liquido]

    # Gas sources
    fonte_gas_list = parse_fonte_gas_list(tree, resultados)
    if fonte_gas_list:
        mr3['fonteGas'] = fonte_gas_list

    # Gas lift valves
    fonte_gas_lift_list = parse_fonte_gas_lift_list(tree, mr2_entrada, resultados)
    if fonte_gas_lift_list:
        mr3['fonteGasLift'] = fonte_gas_lift_list

    # Gas injection (only if gas lift valves exist)
    if fonte_gas_lift_list:
        gas_inj = parse_gas_injecao(mr2_entrada, resultados)
        mr3['gasInj'] = gas_inj

    # Pressure boundary condition
    cond_pressao = parse_condicao_pressao(resultados)
    if 'configuracaoInicial' not in mr3:
        mr3['configuracaoInicial'] = {}
    mr3['configuracaoInicial']['condicaoPressao'] = cond_pressao

    # Separator
    separador = parse_separador(mr2_entrada)
    mr3['separador'] = separador

    # BCS pumps
    comprimento_master1 = master1.get('comprimentoMedido', 0)
    bcs_list = parse_bcs_list(mr2_entrada, resultados, comprimento_master1)
    if bcs_list:
        mr3['bcs'] = bcs_list

    # Production fluid
    fluido_prod = parse_fluido_producao(reader, resultados, tree)
    if fluido_prod:
        existing = mr3.get('fluidosProducao', [])
        existing.append(fluido_prod)
        mr3['fluidosProducao'] = existing

    # Gas fluid
    fluido_gas = parse_fluido_gas(reader, resultados, tree)
    mr3['fluidoGas'] = fluido_gas

    # Slip correlations
    escorregamento_permanente = parse_correlacoes_escorregamento(reader, resultados)
    if 'configuracaoInicial' not in mr3:
        mr3['configuracaoInicial'] = {}
    # Store in a way that from_json will pick up
    mr3.setdefault('configuracaoInicial', {})['escorregamentoPermanente'] = escorregamento_permanente

    # Formation
    formacao = parse_formacao(mr2_entrada, resultados)
    if 'configuracaoInicial' in mr3 and 'Formacao' in mr3['configuracaoInicial']:
        mr3['configuracaoInicial']['Formacao']['TempoProducao'] = formacao['TempoProducao']

    # Ducts, cross-sections, and materials
    formacao_data = {
        'propriedades_existentes': mr3.get('configuracaoInicial', {}).get('Formacao', {}).get('Propriedades', [])
    }
    ducts_result = parse_dutos_and_sections(
        mr2_entrada, reader, resultados, correcao, formacao_data
    )

    mr3['dutosProducao'] = ducts_result['dutosProducao']
    mr3['dutosServico'] = ducts_result['dutosServico']
    mr3['secaoTransversal'] = ducts_result['secaoTransversal']
    mr3['material'] = ducts_result['material']

    # Update formation properties
    if 'configuracaoInicial' in mr3 and 'Formacao' in mr3['configuracaoInicial']:
        mr3['configuracaoInicial']['Formacao']['Propriedades'] = ducts_result['formacaoPropriedades']

    # Update linhaGas
    mr3.setdefault('configuracaoInicial', {})['linhaGas'] = ducts_result['linhaGas']

    # Master2 (service valve)
    master2 = parse_master2(reader, resultados)
    master2['ativo'] = len(ducts_result['dutosServico']) > 0
    mr3['master2'] = master2

    # Gas injection active status
    if 'gasInj' in mr3:
        mr3['gasInj']['ativo'] = ducts_result['linhaGas']

    # Production trends
    tendencias = parse_tendencias_producao(reader, resultados)
    if tendencias:
        mr3['tendP'] = tendencias

    # Service profile
    mr3.setdefault('perfilServico', {})['ativo'] = len(ducts_result['dutosServico']) > 0

    # Flash table file
    _parse_arquivo_tab(mr3, tree)

    # Adjust tabela pressure range to cover actual system pressures
    _ajustar_pressao_maxima_tabela(mr3)

    return mr3


def _parse_arquivo_tab(mr3, tree):
    """Check if there's a compositional fluid with a flash table file."""
    fluido_comp_nodes = tree.xpath('/marlim/entrada/fluidos/fluido_composicional')
    if not fluido_comp_nodes:
        return

    fontes_ativo = tree.xpath(
        "/marlim/entrada/sistema_producao/producao/fonte[@ativo='TRUE']"
    )
    if not fontes_ativo:
        return

    import xmltodict
    from lxml import etree as et

    fonte = xmltodict.parse(et.tostring(fontes_ativo[0]))
    id_fluido = fonte['fonte']['@id_fluido']

    fluido_comp_dict = xmltodict.parse(et.tostring(fluido_comp_nodes[0]))
    fluido_comp = fluido_comp_dict.get('fluido_composicional', {})

    if fluido_comp:
        nome_tab = fluido_comp.get('@tabela', '')
        if nome_tab:
            mr3.setdefault('configuracaoInicial', {})['pvtsimArq'] = nome_tab.strip()

        if id_fluido == fluido_comp.get('@id', None):
            mr3.setdefault('configuracaoInicial', {})['modeloFluidoTabelaFlash'] = True


def _ajustar_pressao_maxima_tabela(mr3):
    """Ensure tabela.pressaoMaxima covers the maximum system pressure.

    The template uses a fixed value (400 kgf/cm²) which may be insufficient
    for deep wells with high reservoir pressure. If the IPR static pressure
    or the initial conditions exceed the current pressaoMaxima, increase it
    with a 10% safety margin.
    """
    tabela = mr3.get('tabela')
    if not tabela:
        return

    pmax_atual = tabela.get('pressaoMaxima', 400)

    # Collect maximum pressure from IPR
    p_sistema = pmax_atual
    for ipr_item in mr3.get('ipr', []):
        for pe in ipr_item.get('pressaoEstatica', []):
            if pe > p_sistema:
                p_sistema = pe

    # Collect maximum pressure from initial conditions in ducts
    for duto in mr3.get('dutosProducao', []):
        ci = duto.get('condicoesIniciais', {})
        for p in ci.get('pressao', []):
            if p > p_sistema:
                p_sistema = p
    for duto in mr3.get('dutosServico', []):
        ci = duto.get('condicoesIniciais', {})
        for p in ci.get('pressao', []):
            if p > p_sistema:
                p_sistema = p

    if p_sistema > pmax_atual:
        # Add 10% margin above maximum system pressure
        tabela['pressaoMaxima'] = round(p_sistema * 1.1, 1)
