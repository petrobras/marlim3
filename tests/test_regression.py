"""
Testes de regressão — executa todos os demos e compara os DataFrames
de perfis e tendências contra referências armazenadas.

Os DataFrames de referência ficam em tests/comparison/<model-name>/ e
são gerados/atualizados pelo script scripts/update_regression_references.py.

Execução:
    pytest tests/test_regression.py -v -m regressao
"""

import os
import shutil

import numpy as np
import pandas as pd
import pytest

import marlim3
from marlim3._download import executable_exists

# Compatibilidade de idioma para referências históricas em português.
EN_TO_PT_COLUMN_MAP = {
    "Length (m) Boundary F": "Comprimento (m) Fronteira F",
    "Length (m) Cell center C": "Comprimento (m) Centro Volume C",
    "Pressure (kgf/cm2) C": "Pressao (kgf/cm2) C",
    "Temperature (C) C": "Temperatura (C) C",
    "Liquid holdup (-) C": "Holdup de liquido (-) C",
    "Complementary liquid vol. fraction (-) C": "Fracao vol. de liquido complementar (-) C",
    "Gas superficial velocity (m/s) F": "Velocidade superficial de gas (m/s) F",
    "Liquid superficial velocity (m/s) F": "Velocidade superficial do liquido (m/s) F",
    "Gas velocity (m/s) F": "Velocidade de gas (m/s) F",
    "Liquid velocity (m/s) F": "Velocidade do liquido (m/s) F",
    "Phase pattern indicator (-) F": "Indicador de arranjo de fases (-) F",
    "Liquid viscosity (cP) C": "Viscosidade do Liquido (cP) C",
    "Gas viscosity (cP) C": "Viscosidade do Gas (cP) C",
    "Gas density (kg/m3) C": "Massa Especifica do Gas (kg/m3) C",
    "Liquid density (kg/m3) C": "Massa Especifica do Liquido (kg/m3) C",
    "Oil density (kg/m3) C": "Massa Especifica do Oleo (kg/m3) C",
    "Water density (kg/m3) C": "Massa Especifica da Agua (kg/m3) C",
    "Mixture density (kg/m3) C": "Massa Especifica da Mistura (kg/m3) C",
    "Solubility ratio (Sm3/Sm3) C": "Razao de Solubilidade (Sm3/Sm3) C",
    "Gas mass flow rate (kg/s) F": "Vazao Massica do Gas (kg/s) F",
    "Liquid mass flow rate (kg/s) F": "Vazao Massica do Liquido (kg/s) F",
    "Distribution coefficient: C0 (-) F": "Coeficiente de distribuição: C0 (-) F",
    "Slip velocity: Ud (m/s) F": "Velocidade de escorregamento: Ud (m/s) F",
    "Gas specific gravity (-) C": "Densidade do Gas (-) C",
    "CO2 molar fraction (-) C": "Fracao Molar de CO2 (-) C",
    "Heat flow between flow and wall (W/m) C": "Fluxo de calor entre escoamento e parede (W/m) C",
    "Interphase mass transfer (kg / [s m]) C": "Transferencia de Massa entre Fases (kg / [s m]) C",
    "Gas specific heat at constant pressure (J/[kg C]) C": "Calor Especifico a pressao constante do Gas (J/[kg C]) C",
    "Liquid specific heat at constant pressure (J/[kg C]) C": "Calor Especifico a pressao constante do Liquido (J/[kg C]) C",
    "Standard dead oil volumetric flow rate (Sm3/d) F": "Vazao volumetrica standard de oleo morto (Sm3/d) F",
    "Standard dead oil + water volumetric flow rate (Sm3/d) F": "Vazao volumetrica standard de oleo morto + agua (Sm3/d) F",
    "Standard dead oil + water + complementary liquid volumetric flow rate (Sm3/d) F": "Vazao volumetrica standard de oleo morto + agua + liquido complementar (Sm3/d) F",
    "Standard free + dissolved gas volumetric flow rate (Sm3/d) F": "Vazao volumetrica standard de gas livre + dissolvido (Sm3/d) F",
    "API gravity (-) C": "Grau API (-) C",
    "Hydrostatic term (Pa/m) F": "Termo Hidrostatico (Pa/m) F",
    "Friction term (Pa/m) F": "Termo Fricao (Pa/m) F",
    "Mass flow relation term 1 (-) F": "Termo de relacao de vazao massica 1 (-) F",
    "Mass flow relation term 2 (kg/s) F": "Termo de relacao de vazao massica 2 (kg/s) F",
    "In-situ dissolved gas specific gravity (-) C": "Densidade Gas Dissolvido In Situ(-) C",
    "In-situ free gas specific gravity (-) C": "Densidade Gas Livre In Situ(-) C",
    "Internal mixture Reynolds (-) F": "Reynolds interno da mistura (-) F",
    "External Reynolds (-) F": "Reynolds externo (-) F",
    "Froude (-) F": "Froud (-) F",
    "Internal mixture Grashof (-) F": "Grashof interno da mistura (-) F",
    "External Grashof (-) F": "Grashof externo (-) F",
    "Internal mixture Nusselt (-) F": "Nusselt interno da mistura (-) F",
    "External Nusselt (-) F": "Nusselt externo (-) F",
    "Internal mixture film coefficient (W / [m2 K]) F": "Coeficiente de pelicula interno da mistura (W / [m2 K]) F",
    "External film coefficient (W / [m2 K]) F": "Coeficiente de pelicula externo (W / [m2 K]) F",
    "Internal mixture Prandtl (-) F": "Prandtl interno da mistura (-) F",
    "External Prandtl (-) F": "Prandtl externo (-) F",
    "Solubility ratio (-) F": "Razao de Solubilidade (-) F",
    "Formation volume factor (-) C": "Fator Volume de Formacao (-) C",
    "Ambient temperature (C) C": "Temperatura Ambiente (C) C",
    "Gas Prandtl (-) C": "Prandtl do Gas (-) C",
    "Liquid Prandtl (-) C": "Prandtl do Liquido (-) C",
    "Complementary-fluid residence time (s) C": "Tempo de Residencia-Fluido Complementar (s) C",
    "Friction reduction factor (-) C": "Fator de reducao de friccao (-) C",
    "Angle (radian) C": "Angulo (radiano) C",
    "Inner diameter (m) C": "Diametro Interno (m) C",
    "Internal wall temperature (C) C": "Temperatura Parede Interna (C) C",
    "Subcooling (C) C": "Subresfriamento (C) C",
    "duct id": "id do duto",
    "Elevation (m) F": "Elevacao (m) F",
    "Elevation (m) C": "Elevacao (m) C",
    "bottomhole_length (m) F": "comprimento_fundoPoco (m) F",
    "Depth (m) F": "Profundidade (m) F",
    "Depth (m) C": "Profundidade (m) C",
    "Pressure (kgf/cm2)": "Pressao (kgf/cm2)",
    "Temperature (C)": "Temperatura (C)",
    "Gas superficial velocity (m/s)": "Velocidade superficial do gas (m/s)",
    "Gas velocity (m/s)": "Velocidade do gas (m/s)",
    "Shear stress (N/m2)": "Tensao Cisalhante (N/m2)",
    "Gas viscosity (cP)": "Viscosidade do Gas (cP)",
    "Gas density (kg/m3)": "Massa Especifica do Gas (kg/m3)",
    "Gas mass flow rate (kg/s)": "Vazao Massica do Gas (kg/s)",
    "Hydrostatic term (Pa/m)": "Termo Hidrostatico (Pa/m)",
    "Friction term (Pa/m)": "Termo Friccao (Pa/m)",
    "Heat flow between flow and wall (W/m)": "Fluxo de calor entre escoamento e parede (W/m)",
    "Standard gas volumetric flow rate (Sm3/d)": "Vazao volumetrica standard de gas (Sm3/d)",
    "VGL stagnation pressure (kgf/cm2)": "Pressao de Estagnacao VGL (kgf/cm²)",
    "VGL stagnation temperature (C)": "Temperatura de Estagnacao VGL (C)",
    "VGL throat pressure (kgf/cm2)": "Pressao na Garganta VGL (kgf/cm²)",
    "VGL throat temperature (C)": "Temperatura na Garganta VGL (C)",
    "Velocity in VGL (m/s)": "Velocidade na VGL (m/s)",
    "Volumetric flow rate in VGL (m3/s)": "Vazao volumetrica na VGL (m³/s)",
}

# ============================================================================
# Verificação do executável
# ============================================================================

def _executavel_disponivel():
    return executable_exists()


skip_sem_executavel = pytest.mark.skipif(
    not _executavel_disponivel(),
    reason="Executável Marlim3 não encontrado",
)

# ============================================================================
# Caminhos
# ============================================================================

TESTS_DIR = os.path.dirname(__file__)
PROJECT_ROOT = os.path.normpath(os.path.join(TESTS_DIR, ".."))
DEMOS_DIR = os.path.join(PROJECT_ROOT, "demos")
COMPARISON_DIR = os.path.join(TESTS_DIR, "comparison")
OUTPUT_DIR = os.path.join(PROJECT_ROOT, "regression_output")

# ============================================================================
# Definição dos demos e seus arquivos auxiliares
# ============================================================================

DEMOS = {
    "2zonas-2VGLs-2-Check-correcPerfTerm": {
        "json": "pt-br/2zonas-2VGLs-2-Check-correcPerfTerm.mr3",
        "aux": ["PVTSIM-MARLIM.tab"],
    },
    "BCS-longo-eficMotor": {
        "json": "pt-br/BCS-longo-eficMotor.mr3",
        "aux": ["PVTSIM-MARLIM.tab"],
    },
    "injec-Liq-TempoResidencia": {
        "json": "pt-br/injec-Liq-TempoResidencia.mr3",
        "aux": [],
    },
    # "MultiBCS": {
    #     "json": "pt-br/MultiBCS.json",
    #     "aux": ["PVTSIM-MARLIM.tab"],
    # },
    "parada-longo-Combinado-BCS-GLC-PIG-completo": {
        "json": "pt-br/parada-longo-Combinado-BCS-GLC-PIG-completo.mr3",
        "aux": ["PVTSIM-MARLIM.tab"],
    },
    "producaoSimplificado": {
        "json": "pt-br/producaoSimplificado.mr3",
        "aux": [],
    },
}

DEMO_IDS = sorted(DEMOS.keys())

# ============================================================================
# Tolerâncias
# ============================================================================

RTOL = 1e-5   # tolerância relativa para comparação numérica
ATOL = 1e-8   # tolerância absoluta para comparação numérica

# ============================================================================
# Utilitários
# ============================================================================

def _run_simulation(model_name):
    """Executa a simulação de um demo e retorna o dicionário de resultados
    no mesmo formato que a GUI (productionProfile, serviceProfile, productionTrend, serviceTrend).
    """
    info = DEMOS[model_name]
    json_path = os.path.join(DEMOS_DIR, info["json"])

    caso = marlim3.Branch()
    caso.from_json(json_path)

    # Forçar modo permanente para acelerar
    if "transient" in (caso.initialConfig or {}):
        caso.initialConfig["transient"] = False

    out_dir = os.path.join(OUTPUT_DIR, model_name)
    os.makedirs(out_dir, exist_ok=True)

    # Copiar arquivos auxiliares
    for aux in info["aux"]:
        src = os.path.join(DEMOS_DIR, aux)
        if os.path.isfile(src):
            shutil.copy2(src, out_dir)

    results_dir = os.path.join(out_dir, f"resultados_{model_name}")

    original_cwd = os.getcwd()
    try:
        os.chdir(out_dir)
        caso.simulate(label=model_name, directory=results_dir)
    finally:
        os.chdir(original_cwd)

    # Processar resultados da mesma forma que a GUI
    resultados = {}
    cfg = caso.initialConfig or {}
    has_linha_gas = cfg.get("gasLine", False)

    if caso.productionProfile is not None:
        perfis = caso._process_profiles(results_dir)
        if perfis is not None:
            resultados["productionProfile"] = perfis

    if has_linha_gas and caso.serviceProfile is not None:
        perfis_s = caso._process_profiles(results_dir, line="service")
        if perfis_s is not None:
            resultados["serviceProfile"] = perfis_s

    if caso.productionTrend is not None:
        tend = caso._process_trends(results_dir)
        if tend:
            resultados["productionTrend"] = tend

    if has_linha_gas and caso.serviceTrend is not None:
        tend_s = caso._process_trends(results_dir, line="service")
        if tend_s:
            resultados["serviceTrend"] = tend_s

    return resultados


def _save_results(model_name, resultados, dest_dir=None):
    """Salva os DataFrames de resultados como CSV em dest_dir/model_name/."""
    if dest_dir is None:
        dest_dir = COMPARISON_DIR
    model_dir = os.path.join(dest_dir, model_name)
    os.makedirs(model_dir, exist_ok=True)

    for key, data in resultados.items():
        if isinstance(data, pd.DataFrame):
            csv_path = os.path.join(model_dir, f"{key}.csv")
            data.to_csv(csv_path)
        elif isinstance(data, dict):
            # Dict de DataFrames (tendências)
            for sub_key, sub_df in data.items():
                if isinstance(sub_df, pd.DataFrame):
                    csv_path = os.path.join(model_dir, f"{key}_{sub_key}.csv")
                    sub_df.to_csv(csv_path)


def _load_reference(model_name):
    """Carrega os DataFrames de referência de tests/comparison/model_name/."""
    model_dir = os.path.join(COMPARISON_DIR, model_name)
    if not os.path.isdir(model_dir):
        return None

    profile_key_map = {
        "perfilProducao": "productionProfile",
        "perfilServico": "serviceProfile",
        "productionProfile": "productionProfile",
        "serviceProfile": "serviceProfile",
    }
    trend_prefix_map = {
        "tendP_": "productionTrend",
        "tendS_": "serviceTrend",
        "productionTrend_": "productionTrend",
        "serviceTrend_": "serviceTrend",
    }

    reference = {}
    csv_files = sorted(f for f in os.listdir(model_dir) if f.endswith(".csv"))

    for csv_file in csv_files:
        csv_path = os.path.join(model_dir, csv_file)
        name = csv_file[:-4]  # remove .csv

        # Detectar se é tendência (aceita nomes antigos e novos)
        for prefix, tend_key in trend_prefix_map.items():
            if name.startswith(prefix):
                sub_key = int(name[len(prefix):])
                df = pd.read_csv(csv_path, index_col=0)
                if tend_key not in reference:
                    reference[tend_key] = {}
                reference[tend_key][sub_key] = df
                break
        else:
            # É perfil (aceita nomes antigos e novos)
            df = pd.read_csv(csv_path, index_col=[0, 1])
            mapped_name = profile_key_map.get(name, name)
            reference[mapped_name] = df

    return reference if reference else None


def _compare_dataframes(df_actual, df_ref, label):
    """Compara dois DataFrames numéricos com tolerância."""
    # Compatibilidade de idioma: normaliza nomes de colunas EN -> PT
    # para comparar com referências históricas em português.
    df_actual = df_actual.rename(columns=EN_TO_PT_COLUMN_MAP)
    df_ref = df_ref.rename(columns=EN_TO_PT_COLUMN_MAP)

    # Compatibilidade adicional para variantes sem acento encontradas em saídas antigas.
    normalize_pt_variants = {
        "Coeficiente de distribuicao: C0 (-) F": "Coeficiente de distribuição: C0 (-) F",
        "Velocidade superficial de gas (m/s) F": "Velocidade superficial do gas (m/s) F",
        "Standard gas volumetric flow rate (Sm3/(d)) F": "Vazao volumetrica standard de gas (Sm3/(d)) F",
        "Termo Fricao (Pa/m) F": "Termo friccao (Pa/m) F",
        "Liquid holdup (-)": "Holdup de liquido (-)",
        "Complementary liquid vol. fraction (-)": "Fracao vol. de liquido complementar (-)",
        "Phase pattern indicator (-)": "Indicador de arranjo de fases (-)",
        "Standard dead oil volumetric flow rate (Sm3/d)": "Vazao volumetrica standard de oleo morto (Sm3/d)",
        "Standard free + dissolved gas volumetric flow rate (Sm3/d)": "Vazao volumetrica standard de gas livre + dissolvido (Sm3/d)",
        "Liquid superficial velocity (m/s)": "Velocidade superficial do liquido (m/s)",
        "Vazao volumetrica standard de gas (Sm3/d)": "Vazao volumetrica standard de Gas (Sm3/d)",
        "Standard gas volumetric flow rate (Sm3/d)": "Vazao volumetrica standard de Gas (Sm3/d)",
    }
    df_actual = df_actual.rename(columns=normalize_pt_variants)
    df_ref = df_ref.rename(columns=normalize_pt_variants)

    # Alinhar colunas
    common_cols = sorted(set(df_actual.columns) & set(df_ref.columns))
    missing_in_actual = set(df_ref.columns) - set(df_actual.columns)
    missing_in_ref = set(df_actual.columns) - set(df_ref.columns)

    assert not missing_in_actual, (
        f"[{label}] Colunas faltando no resultado atual: {missing_in_actual}"
    )
    assert not missing_in_ref, (
        f"[{label}] Colunas novas no resultado atual (não presentes na referência): {missing_in_ref}"
    )

    # Comparar shape
    assert df_actual.shape == df_ref.shape, (
        f"[{label}] Shape diferente: atual={df_actual.shape}, referência={df_ref.shape}"
    )

    # Comparar valores numéricos
    for col in common_cols:
        if pd.api.types.is_numeric_dtype(df_actual[col]) and pd.api.types.is_numeric_dtype(df_ref[col]):
            actual_vals = df_actual[col].values.astype(float)
            ref_vals = df_ref[col].values.astype(float)

            # Pular colunas que são todas NaN em ambos
            if np.all(np.isnan(actual_vals)) and np.all(np.isnan(ref_vals)):
                continue

            np.testing.assert_allclose(
                actual_vals, ref_vals,
                rtol=RTOL, atol=ATOL,
                err_msg=f"[{label}] Coluna '{col}' diverge da referência",
            )
        else:
            # Colunas não numéricas: comparação exata
            pd.testing.assert_series_equal(
                df_actual[col].reset_index(drop=True),
                df_ref[col].reset_index(drop=True),
                check_names=False,
                obj=f"[{label}] Coluna '{col}'",
            )


# ============================================================================
# Testes
# ============================================================================

@skip_sem_executavel
@pytest.mark.regressao
@pytest.mark.parametrize("model_name", DEMO_IDS, ids=DEMO_IDS)
def test_regression(model_name):
    """Executa o demo, gera DataFrames e compara com a referência armazenada."""
    ref_dir = os.path.join(COMPARISON_DIR, model_name)
    if not os.path.isdir(ref_dir) or not os.listdir(ref_dir):
        pytest.skip(
            f"Reference not found for '{model_name}'. "
            f"Run: python tests/update_regression_references.py"
        )

    # Executar simulação
    resultados = _run_simulation(model_name)
    assert resultados, f"Simulação de '{model_name}' não gerou resultados"

    # Carregar referência
    reference = _load_reference(model_name)
    assert reference is not None, (
        f"Falha ao carregar referência de '{model_name}'"
    )

    # Comparar cada resultado
    for key in reference:
        assert key in resultados, (
            f"Resultado '{key}' presente na referência mas ausente na simulação atual"
        )

        ref_data = reference[key]
        actual_data = resultados[key]

        if isinstance(ref_data, pd.DataFrame) and isinstance(actual_data, pd.DataFrame):
            _compare_dataframes(actual_data, ref_data, f"{model_name}/{key}")
        elif isinstance(ref_data, dict) and isinstance(actual_data, dict):
            for sub_key in ref_data:
                assert sub_key in actual_data, (
                    f"Sub-resultado '{key}/{sub_key}' ausente na simulação atual"
                )
                _compare_dataframes(
                    actual_data[sub_key], ref_data[sub_key],
                    f"{model_name}/{key}/{sub_key}",
                )
