# Script de conversão para abrir arquivos de saída do Marlim3 no flotools

# Por padrão, os nomes das variáveis nos .tpl e .ppl seguem a nomenclatura do Olga (tabela OLGA_NAMES)
# para compatibilidade no flotools. Com use_olga_names=False, mantém os nomes originais das colunas
# dos arquivos .dat do Marlim, exceto pressão PT e temperatura TM.

# Para os perfis, verifica se os tempos são os mesmos. Caso positivo, consolida resultados dos arquivos PERFISP e PERFISG em um mesmo .ppl. Caso negativo salva um <nome>_P.ppl e um <nome>_G.ppl

# <nome> é o nome (sem extensão) do arquivo de entrada registrado no cabeçalho dos .dat (arquivo = / file =);
# na ausência dessa informação (saídas antigas), usa o nome do diretório de entrada.

# Para os arquivos TENDP e TENDG, agrupa por array de tempo. Se todos os arquivos tiverem o mesmo array de tempo, consolida em um único .tpl. Caso contrário, salva um .tpl com o maior grupo e os demais com nomes detalhados.
#
# Na analise parametrica (AP), separa PERFIS pelo seqAP do nome e TEND pela coluna
# Sequencia AP / SA sequence antes de consolidar. Cada caso usa o sufixo _AP-<sequencia>
# e sua propria geometria; a coluna Tempo / Time continua sendo o eixo temporal.

# O flotools consegue interpretar as unidades, exceto por algumas temperaturas que aparecem como Coulomb

# O visualizador do Olga não interpreta as unidades, precisaria ter o nome das variáveis do Olga pelos testes realizados


# Histórico de versões:
# v0 por Postal em dezembro de 2024 com auxílio de ChatPetrobras (GPT-4o e Claude-3.5 Sonnet)
# v1 por Postal em 11/06/2025 com GPT-4.1, considera a saída em múltiplos .ppl e múltiplos .tpl, somente quando necessário (diferentes amostragens de tempo)
# v2 por Postal em 17/06/2025 com GPT-4.1, pressão como PT e temperatura como TM para permitir Calculations padrão do flotools como DTHYD e remoção das unidades no nome das variáveis
# v3 por Afranio em 16/06/2026 com ChatPetrobras (Claude Sonnet), suporte a arquivos de saída em inglês
# v4 por Postal em 01/09/2026 com Claude Opus 5 e Sonnet 5, nomes de variáveis do Olga por padrão,
#    tratamento de erros básico, melhorias de desempenho e geometria de contingência na ausência de PERFIS
# v5 por Postal em 29/10/2026 com GPT-6.1 Sol, suporte à saída da análise paramétrica (AP)

import os
import glob
import functools
import math
import re
from decimal import Decimal, InvalidOperation
from pathlib import PureWindowsPath

from datetime import datetime
from .. import __version__

from .._output_headers import (
    SKIP_VARIABLES,
    is_pressure_variable,
    is_temperature_variable,
    parse_header_metadata,
    parse_trend_headers,
    split_trend_header_lines,
)

skip_variables = SKIP_VARIABLES

_AP_COLUMN_NAMES = ('Sequencia AP', 'SA sequence')

# Nome da coluna do .dat (sem unidade e sem o sufixo F/C) -> nome da variável no Olga, conforme
# as listas Volume/Boundary Output Variables. As colunas ausentes desta tabela não têm
# correspondência no Olga e mantêm o nome do próprio .dat quando use_olga_names=True.
OLGA_NAMES = {
    "Pressao": "PT",
    "Pressure": "PT",
    "Temperatura": "TM",
    "Temperature": "TM",
    "Holdup de liquido": "HOL",
    "Liquid holdup": "HOL",
    "Velocidade superficial de gas": "USG",
    "Velocidade superficial do gas": "USG",
    "Gas superficial velocity": "USG",
    "Velocidade superficial do liquido": "USL",
    "Liquid superficial velocity": "USL",
    "Velocidade de gas": "UG",
    "Velocidade do gas": "UG",
    "Gas velocity": "UG",
    "Velocidade do liquido": "UL",
    "Liquid velocity": "UL",
    "Velocidade de escorregamento: Ud": "UO",
    "Slip velocity: Ud": "UO",
    "Indicador de arranjo de fases": "ID",
    "Phase pattern indicator": "ID",
    "Tensao Cisalhante": "TAUWG",
    "Shear stress": "TAUWG",
    "Termo Hidrostatico": "DPZG",
    "Hydrostatic term": "DPZG",
    "Termo Fricao": "DPZF",
    "Termo Friccao": "DPZF",
    "Termo friccao": "DPZF",
    "Friction term": "DPZF",
    "Viscosidade do Liquido": "VISL",
    "Liquid viscosity": "VISL",
    "Viscosidade do Gas": "VISG",
    "Gas viscosity": "VISG",
    "Massa Especifica do Gas": "ROG",
    "Massa Especifica Gas": "ROG",
    "Gas density": "ROG",
    "Massa Especifica do Liquido": "ROL",
    "Liquid density": "ROL",
    "Massa Especifica da Mistura": "ROF",
    "Mixture density": "ROF",
    "Razao de Solubilidade": "RS",
    "Solubility ratio": "RS",
    "RGO": "GORST",
    "BSW": "WCST",
    "Vazao Massica do Gas": "GG",
    "Gas mass flow rate": "GG",
    "Vazao Massica do Liquido": "GLT",
    "Liquid mass flow rate": "GLT",
    "Vazao volumetrica standard de oleo morto": "QOST",
    "Standard dead oil volumetric flow rate": "QOST",
    "Vazao volumetrica standard de oleo morto + agua": "QLST",
    "Standard dead oil + water volumetric flow rate": "QLST",
    "Vazao volumetrica standard de gas livre + dissolvido": "QGST",
    "Standard free + dissolved gas volumetric flow rate": "QGST",
    "Vazao volumetrica standard de Gas": "QGST",
    "Vazao volumetrica standard de gas": "QGST",
    "Standard gas volumetric flow rate": "QGST",
    "Fluxo de calor entre escoamento e parede": "QM",
    "Fluxo de Calor": "QM",
    "Heat flow between flow and wall": "QM",
    "Heat flow": "QM",
    "Calor Especifico a pressao constante do Gas": "CPG",
    "Gas specific heat at constant pressure": "CPG",
    "Calor Especifico a pressao constante do Liquido": "CPHL",
    "Liquid specific heat at constant pressure": "CPHL",
    "Coeficiente de pelicula interno da mistura": "HTK",
    "Coeficiente de pelicula interno": "HTK",
    "Internal mixture film coefficient": "HTK",
    "Internal film coefficient": "HTK",
    "Coeficiente de pelicula externo": "HTKO",
    "External film coefficient": "HTKO",
    "Temperatura Ambiente": "TU",
    "Ambient temperature": "TU",
    "Temperatura Parede Interna": "TWS",
    "Temperatura Interna da Parede": "TWS",
    "Internal wall temperature": "TWS",
    "Diametro Interno": "IDIAM",
    "Diametro Interno da tubulacao": "IDIAM",
    "Inner diameter": "IDIAM",
    "Tubing inner diameter": "IDIAM",
    "Angulo": "INCL",
    "Angle": "INCL",
    "Froud": "FROUDE2P",
    "Froude": "FROUDE2P",
}

# ---------------------------------------------------------------------------
# Mapeamento bilíngue de nomes de colunas especiais
# ---------------------------------------------------------------------------

# Nomes da coluna de tempo em cada idioma
_TIME_COLUMN_NAMES = {
    'pt': 'Tempo (s)',
    'en': 'Time (s)',
}

# Nomes da coluna de comprimento/fronteira em cada idioma
_SECTION_COLUMN_NAMES = {
    'pt': 'Comprimento (m) Fronteira F',
    'en': 'Length (m) Boundary F',
}

# Nomes da coluna de elevação para o ramal de PRODUÇÃO em cada idioma
_ELEVATION_P_COLUMN_NAMES = {
    'pt': 'Elevacao (m) F',
    'en': 'Elevation (m) F',
}

# Nomes da coluna de elevação para o ramal de SERVIÇO em cada idioma
_ELEVATION_G_COLUMN_NAMES = {
    'pt': 'Profundidade (m) F',
    'en': 'Depth (m) F',
}


def detect_language_from_variables(variables):
    """Detecta o idioma do arquivo a partir da lista de variáveis.

    Retorna 'pt' (português) ou 'en' (inglês).
    A heurística compara os candidatos conhecidos de coluna de tempo.
    """
    variables_stripped = [v.strip() for v in variables]
    if _TIME_COLUMN_NAMES['en'] in variables_stripped:
        return 'en'
    return 'pt'


def get_time_column_name(language):
    return _TIME_COLUMN_NAMES.get(language, _TIME_COLUMN_NAMES['pt'])


def get_section_column_name(language):
    return _SECTION_COLUMN_NAMES.get(language, _SECTION_COLUMN_NAMES['pt'])


def get_elevation_p_column_name(language):
    return _ELEVATION_P_COLUMN_NAMES.get(language, _ELEVATION_P_COLUMN_NAMES['pt'])


def get_elevation_g_column_name(language):
    return _ELEVATION_G_COLUMN_NAMES.get(language, _ELEVATION_G_COLUMN_NAMES['pt'])


def variable_base_name(var_name):
    """Nome da coluna sem a unidade, que o Marlim grava entre parênteses ou após a última vírgula."""
    idx = var_name.find('(')
    if idx > 0:
        return var_name[:idx].strip()
    head, _, tail = var_name.rpartition(',')
    tail = tail.strip()
    if head and tail and ' ' not in tail:
        return head.strip()
    return var_name.strip()


def output_variable_name(var_name, use_olga_names):
    """Nome da variável gravado no CATALOG; pressão e temperatura sempre viram PT/TM."""
    if use_olga_names:
        olga = OLGA_NAMES.get(variable_base_name(var_name))
        if olga:
            return olga
    if is_temperature_variable(var_name):
        return "TM"
    if is_pressure_variable(var_name):
        return "PT"
    idx = var_name.find('(')
    return var_name[:idx].strip() if idx > 0 else var_name


# ---------------------------------------------------------------------------
# Header template
# ---------------------------------------------------------------------------

def _build_header_template():
    today = datetime.now().strftime("%d-%m-%y %H:%M:%S")
    return f"""'Marlim v{__version__}' 
{{plot_type}} 
INPUT FILE 
'Marlim3.genkey' 
PVT FILE 
'./3phase.tab' 
DATE 
'{today}' 
PROJECT 
'Marlim' 
TITLE 
'Convertido a partir da saída do Marlim 3' 
AUTHOR 
'Petrobras' 
NETWORK 
{{network_count}} 
GEOMETRY' (M) ' 
{{geometry_blocks}}"""


class ConversionError(ValueError):
    """Problema nos dados de entrada, com sugestão para o usuário."""

    def __init__(self, message, suggestion=""):
        full_message = f"{message} {suggestion}".strip() if suggestion else message
        super().__init__(full_message)
        self.suggestion = suggestion


# ---------------------------------------------------------------------------
# Leitura de arquivos
# ---------------------------------------------------------------------------

def _read_dat_lines(file_path, min_lines):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.read().splitlines()
    except OSError as error:
        raise ConversionError(
            f"Could not read file '{file_path}' ({error.strerror}).",
            "Check that the file exists and is not open in another program."
        ) from error
    if len(lines) < min_lines:
        raise ConversionError(
            f"File '{os.path.basename(file_path)}' is incomplete: "
            f"it has {len(lines)} line(s) and at least {min_lines} are expected.",
            "The Marlim 3 simulation may have been interrupted before writing this file."
        )
    return lines


def _require_columns(variables, names, file_path):
    missing = [name for name in names if name not in variables]
    if missing:
        raise ConversionError(
            f"File '{os.path.basename(file_path)}' is missing required column(s): "
            f"{', '.join(missing)}.",
            "Check whether this file is a Marlim 3 output in Portuguese or English."
        )


def numeric_column(data, index, name, file_path):
    values = []
    for line_number, row in enumerate(data, start=1):
        if index >= len(row):
            raise ConversionError(
                f"Data line {line_number} of file '{os.path.basename(file_path)}' "
                f"does not have column '{name}'.",
                "The file appears truncated; re-running the simulation is recommended."
            )
        try:
            values.append(float(row[index]))
        except ValueError as error:
            raise ConversionError(
                f"Value '{row[index]}' in column '{name}' "
                f"(data line {line_number} of file '{os.path.basename(file_path)}') is not a number.",
                "The file appears corrupted; re-running the simulation is recommended."
            ) from error
    return values


def _ap_sequence(value, file_path, context):
    try:
        number = Decimal(value)
    except InvalidOperation as error:
        raise ConversionError(f"Invalid AP sequence '{value}' in '{file_path}' ({context}).") from error
    if not number.is_finite() or number < 0 or number != number.to_integral_value():
        raise ConversionError(f"Invalid AP sequence '{value}' in '{file_path}' ({context}).")
    return int(number)


def _ap_file_metadata(file_path):
    name = os.path.basename(file_path)
    profile = re.fullmatch(r'PERFIS([PG])-seqAP-(\d+)-(-?\d+)\.dat', name)
    if profile:
        return int(profile[2]), profile[1], None
    trend = re.fullmatch(r'TEND([PG])-AP-(-?\d+(?:\.\d+)?)\.dat', name)
    if trend:
        return None, trend[1], trend[2]
    if '-seqAP-' in name or '-AP-' in name:
        raise ConversionError(f"Malformed AP output filename '{name}'.")
    return None


def _validate_ap_rows(variables, data, file_path):
    time_col = get_time_column_name(detect_language_from_variables(variables))
    _require_columns(variables, [time_col], file_path)
    for row_number, row in enumerate(data, 1):
        if len(row) != len(variables):
            raise ConversionError(
                f"Data line {row_number} of file '{os.path.basename(file_path)}' "
                f"has {len(row)} columns; expected {len(variables)}."
            )
    times = numeric_column(data, variables.index(time_col), time_col, file_path)
    if not all(math.isfinite(time) for time in times):
        raise ConversionError(f"Non-finite time in file '{file_path}', column '{time_col}'.")
    for index, name in enumerate(variables):
        if '(' in name and name != time_col and name not in skip_variables:
            values = numeric_column(data, index, name, file_path)
            if not all(math.isfinite(value) for value in values):
                raise ConversionError(f"Non-finite values in file '{file_path}', column '{name}'.")
    return times


def _split_ap_trend(file_path):
    _, variables, data, _, _, _ = read_tend_file(file_path)
    names = [name for name in _AP_COLUMN_NAMES if name in variables]
    if len(names) != 1:
        raise ConversionError(
            f"File '{file_path}' requires exactly one AP column: {', '.join(_AP_COLUMN_NAMES)}."
        )
    times = _validate_ap_rows(variables, data, file_path)
    index = variables.index(names[0])
    time_index = variables.index(get_time_column_name(detect_language_from_variables(variables)))
    groups = {}
    seen = set()
    for row_number, (row, time) in enumerate(zip(data, times), 1):
        sequence = _ap_sequence(row[index], file_path, f"data line {row_number}, column {names[0]}")
        if (sequence, time) in seen:
            raise ConversionError(
                f"Duplicate time {time} for AP {sequence} in file '{file_path}' (data line {row_number})."
            )
        seen.add((sequence, time))
        # Normalize numeric time spellings without modifying cached input rows.
        selected = row.copy()
        selected[time_index] = str(time)
        groups.setdefault(sequence, []).append(selected)
    for rows in groups.values():
        rows.sort(key=lambda row: float(row[time_index]))
    return groups


def _ap_geometry(perfis_files, source):
    geometry = None
    seen_times = set()
    schema = None
    for file_path in perfis_files:
        variables, data = read_perfis_file(file_path)
        if schema is not None and variables != schema:
            raise ConversionError(f"Incompatible AP profile columns in '{file_path}' ({source}).")
        schema = variables
        times = _validate_ap_rows(variables, data, file_path)
        lang = detect_language_from_variables(variables)
        section_col = get_section_column_name(lang)
        elevation_col = (get_elevation_p_column_name(lang) if source == 'PRODUCAO'
                         else get_elevation_g_column_name(lang))
        center_names = ('Comprimento (m) Centro Volume C', 'Comprimento (m) centro de Volume C',
                        'Length (m) Cell center C')
        center_col = next((name for name in center_names if name in variables), center_names[0])
        center_elevation_col = elevation_col[:-1] + 'C'
        _require_columns(variables, [section_col, elevation_col, center_col, center_elevation_col], file_path)
        sections = numeric_column(data, variables.index(section_col), section_col, file_path)
        elevations = numeric_column(data, variables.index(elevation_col), elevation_col, file_path)
        centers = numeric_column(data, variables.index(center_col), center_col, file_path)
        center_elevations = numeric_column(
            data, variables.index(center_elevation_col), center_elevation_col, file_path)
        snapshots = {}
        endpoints = {}
        for time, section, elevation, center, center_elevation in zip(
                times, sections, elevations, centers, center_elevations):
            if not all(math.isfinite(value) for value in (section, elevation, center, center_elevation)):
                raise ConversionError(f"Non-finite geometry in AP profile '{file_path}'.")
            snapshot = snapshots.setdefault(time, ([], []))
            snapshot[0].append(section)
            snapshot[1].append(elevation)
            endpoints[time] = (2 * center - section, 2 * center_elevation - elevation)
        for time, snapshot in snapshots.items():
            if time in seen_times:
                raise ConversionError(f"Duplicate AP profile time {time} in '{file_path}' ({source}).")
            seen_times.add(time)
            # DAT rows contain each cell's left boundary and midpoint, not the final boundary.
            snapshot[0].append(endpoints[time][0])
            snapshot[1].append(endpoints[time][1])
            if len(snapshot[0]) < 2:
                raise ConversionError(f"AP profile '{file_path}' requires at least 2 cell boundaries.")
            if any(right <= left for left, right in zip(snapshot[0], snapshot[0][1:])):
                raise ConversionError(f"Non-increasing AP profile boundaries in '{file_path}'.")
            if geometry is None:
                geometry = snapshot
            elif geometry != snapshot:
                raise ConversionError(f"Incompatible AP profile geometry in '{file_path}' ({source}).")
    return format_geometry_block(*geometry, source) if geometry is not None else ''


# Cada .dat é lido apenas uma vez; as listas retornadas são compartilhadas entre
# as chamadas (mesmo arquivo pode ser consultado por geometria, catálogo e dados) e
# não devem ser modificadas.
@functools.lru_cache(maxsize=None)
def read_tend_file(file_path):
    lines = _read_dat_lines(file_path, 5)
    _, headers, n_header_lines = split_trend_header_lines(lines)
    if len(lines) < n_header_lines + 2:
        lines = _read_dat_lines(file_path, n_header_lines + 2)
    variable_line = lines[n_header_lines]
    data_lines = lines[n_header_lines + 1:]

    try:
        position, rotulo, celula = parse_trend_headers(headers[0], headers[1], headers[2])
    except (ValueError, IndexError) as error:
        raise ConversionError(f"Invalid trend headers in file '{file_path}': {error}.") from error
    position = str(position)
    celula = str(celula)

    variables = variable_line.strip().split(';')
    variables = [var.strip() for var in variables if var.strip()]

    data = []
    for line in data_lines:
        line = line.strip()
        if line:
            values = [val.strip() for val in line.split(';') if val.strip()]
            data.append(values)

    if not data:
        raise ConversionError(
            f"File '{os.path.basename(file_path)}' has no data lines.",
            "The simulation did not record results for this monitoring point."
        )

    return headers, variables, data, position, rotulo, celula


@functools.lru_cache(maxsize=None)
def read_perfis_file(file_path):
    lines = _read_dat_lines(file_path, 3)

    variable_line = lines[1]
    data_lines = lines[2:]

    variables = variable_line.strip().split(';')
    variables = [var.strip() for var in variables if var.strip()]

    data = []
    for line in data_lines:
        line = line.strip()
        if line:
            values = [val.strip() for val in line.split(';') if val.strip()]
            data.append(values)

    if not data:
        raise ConversionError(
            f"File '{os.path.basename(file_path)}' has no data lines.",
            "The simulation did not record the corresponding profile."
        )

    return variables, data


def read_geometry_columns(file_path, section_column, elevation_column):
    variables, data = read_perfis_file(file_path)
    _require_columns(variables, [section_column, elevation_column], file_path)
    sections = numeric_column(data, variables.index(section_column), section_column, file_path)
    elevations = numeric_column(data, variables.index(elevation_column), elevation_column, file_path)
    if len(sections) < 2:
        raise ConversionError(
            f"File '{os.path.basename(file_path)}' defines only {len(sections)} cell boundary(ies), "
            "and at least 2 are required to build the pipe geometry.",
            "Check the mesh used in the simulation."
        )
    return sections, elevations


# ---------------------------------------------------------------------------
# Auxiliares de formatação / escrita
# ---------------------------------------------------------------------------

def format_geometry_block(sections, elevations, name):
    def format_block(values):
        return "\n".join(
            " ".join(f"{float(val):.16e}" for val in values[i:i+5])
            for i in range(0, len(values), 5)
        )

    sections_str = format_block(sections)
    elevations_str = format_block(elevations)

    return f"BRANCH\n'{name}'\n{len(sections) - 1}\n{sections_str}\n{elevations_str}\n"


def format_values(values):
    return " ".join(f"{float(value):.6e}" for value in values)


def write_tpl_output_file(output_path, all_variables, sorted_data, position_mapping, geometry_blocks, use_olga_names):
    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(_build_header_template().format(
            plot_type="TIME PLOT",
            network_count=str(geometry_blocks.count("BRANCH\n")),
            geometry_blocks=geometry_blocks,
        ))

        file.write("CATALOG\n")
        file.write(f"{len(all_variables)}\n")

        for var, position, file_number, celula in all_variables:
            if '(' in var:
                rotulo, comprimento = position_mapping[(var, position, file_number)]
                unit = var.split('(')[1].split(')')[0]
                var_out = output_variable_name(var.strip(), use_olga_names)
                file.write(f"{var_out} 'POSITION:' '{rotulo} {comprimento}m {celula}' '({unit})' '{var}'\n")

        file.write("TIME SERIES ' (S) '\n")

        file.writelines(format_values(row) + "\n" for row in sorted_data)


def write_ppl_output_file(output_path, perfisp_variables, perfisg_variables, sorted_data, times, geometry_blocks, use_olga_names):
    catalog = ([(var, 'PRODUCAO') for var in perfisp_variables]
               + [(var, 'SERVICO') for var in perfisg_variables])

    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(_build_header_template().format(
            plot_type="PROFILE PLOT",
            network_count=str(geometry_blocks.count("BRANCH\n")),
            geometry_blocks=geometry_blocks,
        ))

        file.write("CATALOG\n")
        file.write(f"{len(catalog)}\n")

        for var, branch in catalog:
            var_type = "BOUNDARY" if var.endswith("F") else "SECTION"
            unit = var.split('(')[1].split(')')[0]
            var_out = output_variable_name(var.strip(), use_olga_names)
            file.write(f"{var_out} '{var_type}:' 'BRANCH:' '{branch}' '({unit})' '{var}'\n")

        file.write("TIME SERIES ' (S) '\n")

        catalog_with_type = [(var, branch, var.endswith("F")) for var, branch in catalog]
        lines = []
        for time in times:
            lines.append(f"{float(time):.6e}\n")
            data_for_time = sorted_data[time]

            for var, branch, is_boundary in catalog_with_type:
                values = [value for value, source in data_for_time.get(var, []) if source == branch]
                if not values:
                    continue
                line = format_values(values)
                if is_boundary:  # repete o último valor para fechar a última fronteira
                    line += f" {float(values[-1]):.6e}"
                lines.append(line + "\n")
        file.writelines(lines)


# ---------------------------------------------------------------------------
# Processamento condicional de arquivos PERFIS
# ---------------------------------------------------------------------------

def collect_perfis_data(perfis_files, source_label, lang=None, normalize_times=False):
    """Lê uma lista de arquivos PERFIS e retorna variables, ppl_all_data e ppl_times.

    Detecta o idioma automaticamente a partir de cada arquivo quando `lang` não é fornecido.
    """
    collected_variables = []
    ppl_all_data = {}
    ppl_times = []

    for input_file in perfis_files:
        variables, data = read_perfis_file(input_file)
        file_lang = lang or detect_language_from_variables(variables)
        time_col = get_time_column_name(file_lang)
        time_index = variables.index(time_col)

        # Colunas aproveitadas, filtradas uma vez por arquivo em vez de a cada linha
        columns = [(i, var.strip()) for i, var in enumerate(variables)
                   if '(' in var and var.strip() != time_col and var.strip() not in skip_variables]
        for _, var_name in columns:
            if var_name not in collected_variables:
                collected_variables.append(var_name)

        n_variables = len(variables)
        for row in data:
            time = str(float(row[time_index])) if normalize_times else row[time_index]
            values_for_time = ppl_all_data.setdefault(time, {})
            ppl_times.append(time)
            row_columns = columns if len(row) >= n_variables else [(i, v) for i, v in columns if i < len(row)]
            for i, var_name in row_columns:
                values_for_time.setdefault(var_name, []).append((row[i], source_label))

    return collected_variables, ppl_all_data, ppl_times


def process_perfis_files_conditional(perfisp_files, perfisg_files, output_directory, geometry_blocks,
                                      input_dir_name, use_olga_names, branch_geometry=None):
    normalize = branch_geometry is not None
    perfisp_variables, ppl_data_p, ppl_times_p = collect_perfis_data(perfisp_files, 'PRODUCAO', normalize_times=normalize) if perfisp_files else ([], {}, [])
    perfisg_variables, ppl_data_g, ppl_times_g = collect_perfis_data(perfisg_files, 'SERVICO', normalize_times=normalize) if perfisg_files else ([], {}, [])

    # Se ambos existem e os conjuntos de tempo são iguais, consolida
    if ppl_data_p and ppl_data_g and ppl_data_p.keys() == ppl_data_g.keys():
        print(f"Gerado arquivo único: {input_dir_name}.ppl")
        for time, entries in ppl_data_g.items():
            destino = ppl_data_p[time]
            for var_name, entradas in entries.items():
                destino.setdefault(var_name, []).extend(entradas)
        ppl_sorted_times = sorted(ppl_data_p, key=float)
        output_file = os.path.join(output_directory, f"{input_dir_name}.ppl")
        write_ppl_output_file(output_file, perfisp_variables, perfisg_variables, ppl_data_p, ppl_sorted_times,
                               geometry_blocks, use_olga_names)
    else:
        # Arquivos P e G com arrays de tempo distintos → um .ppl para cada
        if perfisp_files:
            ppl_sorted_times = sorted(set(ppl_times_p), key=float)
            suffix = ".ppl" if not perfisg_files else "_P.ppl"
            output_file = os.path.join(output_directory, f"{input_dir_name}{suffix}")
            write_ppl_output_file(output_file, perfisp_variables, [], ppl_data_p, ppl_sorted_times,
                                   branch_geometry['PRODUCAO'] if branch_geometry is not None else geometry_blocks,
                                   use_olga_names)
        if perfisg_files:
            ppl_sorted_times = sorted(set(ppl_times_g), key=float)
            output_file = os.path.join(output_directory, f"{input_dir_name}_G.ppl")
            write_ppl_output_file(output_file, [], perfisg_variables, ppl_data_g, ppl_sorted_times,
                                   branch_geometry['SERVICO'] if branch_geometry is not None else geometry_blocks,
                                   use_olga_names)


# ---------------------------------------------------------------------------
# Processamento de grupos de arquivos TEND
# ---------------------------------------------------------------------------

def get_time_array(file_path, selected_rows=None):
    headers, variables, data, _, _, _ = read_tend_file(file_path)
    lang = detect_language_from_variables(variables)
    time_col = get_time_column_name(lang)
    time_index = variables.index(time_col)
    return [row[time_index] for row in (selected_rows if selected_rows is not None else data)]


def group_tend_files_by_time_array(tend_files, trend_rows=None):
    time_array_groups = {}
    for file_path in tend_files:
        time_array = tuple(get_time_array(file_path, trend_rows[file_path] if trend_rows is not None else None))
        if time_array not in time_array_groups:
            time_array_groups[time_array] = []
        time_array_groups[time_array].append(file_path)
    return time_array_groups


def process_tend_file_group(file_group, output_directory, geometry_blocks, output_file_name, use_olga_names,
                            trend_rows=None):
    all_variables = []
    position_mapping = {}
    all_data = {}
    var_indices = {}

    for input_file in file_group:
        headers, variables, data, position, rotulo, celula = read_tend_file(input_file)
        if trend_rows is not None:
            data = trend_rows[input_file]
            file_number = os.path.basename(input_file)[:-4]
        else:
            file_number = os.path.basename(input_file).split('-')[1].split('.')[0]

        lang = detect_language_from_variables(variables)
        time_col = get_time_column_name(lang)
        time_index = variables.index(time_col)

        # Colunas aproveitadas e sua posição na linha de saída, resolvidas uma vez por arquivo
        columns = []
        for i, var in enumerate(variables):
            var_name = var.strip()
            if '(' in var_name and var_name != time_col and var_name not in skip_variables:
                var_tuple = (var_name, position, file_number, celula)
                if var_tuple not in var_indices:
                    var_indices[var_tuple] = len(all_variables) + 1  # +1 porque coluna 0 é tempo
                    all_variables.append(var_tuple)
                    # Expande todas as linhas já existentes em all_data para o novo tamanho
                    for existing_row in all_data.values():
                        while len(existing_row) < len(all_variables) + 1:
                            existing_row.append(0.0)
                position_mapping[(var_name, position, file_number)] = (rotulo, position)
                columns.append((i, var_indices[var_tuple]))

        width = len(all_variables) + 1
        for row in data:
            time = row[time_index]
            line = all_data.get(time)
            if line is None:
                line = [time] + [0.0] * len(all_variables)
                all_data[time] = line
            elif len(line) < width:
                line.extend([0.0] * (width - len(line)))
            for i, idx in columns:
                line[idx] = row[i]

    sorted_times = sorted(all_data.keys(), key=float)
    sorted_data = []
    for time in sorted_times:
        row = all_data[time]
        while len(row) < len(all_variables) + 1:
            row.append(0.0)
        sorted_data.append(row)

    write_tpl_output_file(
        os.path.join(output_directory, output_file_name),
        all_variables,
        sorted_data,
        position_mapping,
        geometry_blocks,
        use_olga_names,
    )


# ---------------------------------------------------------------------------
# Ponto de entrada principal
# ---------------------------------------------------------------------------

def _convert_file_set(input_dir_name, output_directory, use_olga_names,
                      tend_files, perfisp_files, perfisg_files, trend_rows=None):
    geometry_blocks = ""
    branch_geometry = None
    if trend_rows is not None:
        branch_geometry = {
            'PRODUCAO': _ap_geometry(perfisp_files, 'PRODUCAO'),
            'SERVICO': _ap_geometry(perfisg_files, 'SERVICO'),
        }
        for branch, prefix in (('PRODUCAO', 'TENDP'), ('SERVICO', 'TENDG')):
            matching = [path for path in tend_files if os.path.basename(path).startswith(prefix)]
            if not branch_geometry[branch] and matching:
                _, _, _, position, _, _ = read_tend_file(matching[0])
                length = float(position)
                branch_geometry[branch] = format_geometry_block(
                    [0.0, length if length > 0 else 1.0], [0.0, 0.0], branch)
        geometry_blocks = ''.join(branch_geometry.values())
    else:
        if perfisp_files:
            variables, _ = read_perfis_file(perfisp_files[0])
            lang = detect_language_from_variables(variables)
            sections, elevations = read_geometry_columns(
                perfisp_files[0], get_section_column_name(lang), get_elevation_p_column_name(lang))
            geometry_blocks += format_geometry_block(sections, elevations, "PRODUCAO")
        elif tend_files and not perfisg_files:
            try:
                _, _, _, position, _, _ = read_tend_file(tend_files[0])
                pos_val = float(position)
                sections = [0.0, pos_val if pos_val > 0 else 1.0]
                geometry_blocks += format_geometry_block(sections, [0.0, 0.0], "PRODUCAO")
            except (ConversionError, ValueError, IndexError):
                geometry_blocks += format_geometry_block([0.0, 1.0], [0.0, 0.0], "PRODUCAO")
        if perfisg_files:
            variables, _ = read_perfis_file(perfisg_files[0])
            lang = detect_language_from_variables(variables)
            sections, elevations = read_geometry_columns(
                perfisg_files[0], get_section_column_name(lang), get_elevation_g_column_name(lang))
            geometry_blocks += format_geometry_block(sections, elevations, "SERVICO")

    if tend_files:
        groups = group_tend_files_by_time_array(tend_files, trend_rows)
        print(f"Encontrados {len(groups)} grupos de arquivos TEND*.dat com arrays de tempo idênticos")
        primary = max(groups.values(), key=len)
        for file_group in groups.values():
            output_name = f"{input_dir_name}.tpl"
            if file_group is not primary:
                tags = []
                for path in file_group:
                    base = os.path.basename(path)
                    branch = 'G' if 'TENDG' in base else 'P'
                    position = (_ap_file_metadata(path)[2] if trend_rows is not None
                                else base.split('-')[1].split('.')[0])
                    tags.append(f"{branch}-{position}")
                output_name = f"{input_dir_name}_{'_'.join(tags)}.tpl"
            process_tend_file_group(file_group, output_directory, geometry_blocks,
                                    output_name, use_olga_names, trend_rows)

    if perfisp_files or perfisg_files:
        process_perfis_files_conditional(perfisp_files, perfisg_files, output_directory,
                                          geometry_blocks, input_dir_name, use_olga_names, branch_geometry)


def _header_input_file(file_path):
    """Input file path recorded in a PERFIS/TEND header, or None for older outputs."""
    try:
        with open(file_path, 'r', encoding='utf-8', errors='replace') as file:
            lines = [file.readline().strip() for _ in range(4)]
    except OSError:
        return None
    if os.path.basename(file_path).startswith('TEND'):
        metadata = split_trend_header_lines(lines)[0]
    else:
        metadata = parse_header_metadata(lines[0])
    return metadata.get('input_file') or None


def _output_base_name(input_directory, paths):
    """Output name from the input file stem in the .dat headers, else the directory name.

    PureWindowsPath accepts both '/' and '\\', so paths written on either OS are parsed.
    """
    names = []
    for path in paths:
        input_file = _header_input_file(path)
        stem = PureWindowsPath(input_file).stem if input_file else ''
        if stem and stem not in names:
            names.append(stem)
    if len(names) > 1:
        print(f"Aviso: arquivos .dat de entradas diferentes ({', '.join(names)}); usando '{names[0]}'")
    return names[0] if names else os.path.basename(os.path.normpath(input_directory))


def convert_to_ppl_tpl(input_directory, output_directory, use_olga_names=True):
    """Convert ordinary outputs and independent AP cases to TPL/PPL files.

    Output files are named after the input file recorded in the .dat headers
    (e.g. case.mr3 -> case.ppl), falling back to the input directory name for
    outputs without it. AP outputs use <name>_AP-<sequence> names. Compatible
    times and branches consolidate only within a case; incompatible sampling
    produces extra files.
    """
    tend_files = sorted(glob.glob(os.path.join(input_directory, 'TEND*.dat')))
    perfisp_files = sorted(glob.glob(os.path.join(input_directory, 'PERFISP-*.dat')))
    perfisg_files = sorted(glob.glob(os.path.join(input_directory, 'PERFISG-*.dat')))
    if not perfisp_files and not perfisg_files and not tend_files:
        raise ConversionError(
            f"No PERFISP, PERFISG or TEND files found in directory '{input_directory}'.",
            "Provide the directory containing the Marlim 3 output files."
        )
    ordinary = ([], [], [])
    cases = {}
    try:
        for kind, paths in enumerate((tend_files, perfisp_files, perfisg_files)):
            for path in paths:
                metadata = _ap_file_metadata(path)
                if metadata is None:
                    ordinary[kind].append(path)
                elif kind == 0:
                    for sequence, rows in _split_ap_trend(path).items():
                        case = cases.setdefault(sequence, ([], [], [], {}))
                        case[0].append(path)
                        case[3][path] = rows
                else:
                    case = cases.setdefault(metadata[0], ([], [], [], {}))
                    case[kind].append(path)
        name = _output_base_name(input_directory, perfisp_files + perfisg_files + tend_files)
        if any(ordinary):
            _convert_file_set(name, output_directory, use_olga_names, *ordinary)
        for sequence, (trends, profiles_p, profiles_g, rows) in sorted(cases.items()):
            _convert_file_set(f"{name}_AP-{sequence}", output_directory, use_olga_names,
                              trends, profiles_p, profiles_g, rows)
    finally:
        read_tend_file.cache_clear()
        read_perfis_file.cache_clear()