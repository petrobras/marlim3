# Script de conversão para abrir arquivos de saída do Marlim3 no flotools

# Para os perfis, verifica se os tempos são os mesmos. Caso positivo, consolida resultados dos arquivos PERFISP e PERFISG em um mesmo .ppl. Caso negativo salva um <nome>_P.ppl e um <nome>_G.ppl

# Para os arquivos TENDP e TENDG, agrupa por array de tempo. Se todos os arquivos tiverem o mesmo array de tempo, consolida em um único .tpl. Caso contrário, salva um _incompleto.tpl com o maior grupo e os demais com nomes detalhados.

# O flotools consegue interpretar as unidades, exceto por algumas temperaturas que aparecem como Coulomb

# O visualizador do Olga não interpreta as unidades, precisaria ter o nome das variáveis do Olga pelos testes realizados


# Histórico de versões:
# v0 por Postal (BEU0) em dezembro de 2024 com auxílio de ChatPetrobras (GPT-4o e Claude-3.5 Sonnet)
# v1 por Postal (BEU0) em 11/06/2025 com GPT-4.1, considera a saída em múltiplos .ppl e múltiplos .tpl, somente quando necessário (diferentes amostragens de tempo)
# v2 por Postal (BEU0) em 17/06/2025 com GPT-4.1, pressão como PT e temperatura como TM para permitir Calculations padrão do flotools como DTHYD e remoção das unidades no nome das variáveis
# v3 por Postal (BEU0) em 16/06/2026 com ChatPetrobras (Claude Sonnet), suporte a arquivos de saída em inglês

import os
import glob
import sys

from datetime import datetime
from .. import __version__

from .._output_headers import (
    SKIP_VARIABLES,
    is_pressure_variable,
    is_temperature_variable,
    parse_trend_headers,
    resolve_time_column,
)

skip_variables = SKIP_VARIABLES

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
    'en': 'Elevation (m) F',
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


# ---------------------------------------------------------------------------
# Leitura de arquivos
# ---------------------------------------------------------------------------

def read_tend_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()
    headers = lines[:3]
    variable_line = lines[3]
    data_lines = lines[4:]

    position, rotulo, celula = parse_trend_headers(headers[0], headers[1], headers[2])
    position = str(position)
    celula = str(celula)

    variables = variable_line.strip().split(';')
    variables = [var.strip() for var in variables if var.strip()]

    data = []
    for line in data_lines:
        if line.strip():
            values = line.strip().split(';')
            values = [val.strip() for val in values if val.strip()]
            data.append(values)

    return headers, variables, data, position, rotulo, celula


def read_perfis_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    variable_line = lines[1]
    data_lines = lines[2:]

    variables = variable_line.strip().split(';')
    variables = [var.strip() for var in variables if var.strip()]

    data = []
    for line in data_lines:
        if line.strip():
            values = line.strip().split(';')
            values = [val.strip() for val in values if val.strip()]
            data.append(values)

    return variables, data


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


def write_tpl_output_file(output_path, all_variables, sorted_data, position_mapping, geometry_blocks):
    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(_build_header_template().format(
            plot_type="TIME PLOT",
            network_count="2" if "SERVICO" in geometry_blocks else "1",
            geometry_blocks=geometry_blocks,
        ))

        file.write("CATALOG\n")
        file.write(f"{len(all_variables)}\n")

        for var, position, file_number, celula in all_variables:
            if '(' in var:
                rotulo, comprimento = position_mapping[(var, position, file_number)]
                unit = var.split('(')[1].split(')')[0]
                if is_temperature_variable(var):
                    var_out = "TM"
                elif is_pressure_variable(var):
                    var_out = "PT"
                else:
                    idx = var.find('(')
                    var_out = var[:idx].strip() if idx > 0 else var
                file.write(f"{var_out} 'POSITION:' '{rotulo} {comprimento}m {celula}' '({unit})' '{var}'\n")

        file.write("TIME SERIES ' (S) '\n")

        for row in sorted_data:
            file.write(" ".join(f"{float(value):.6e}" for value in row) + "\n")


def write_ppl_output_file(output_path, perfisp_variables, perfisg_variables, sorted_data, times, geometry_blocks):
    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(_build_header_template().format(
            plot_type="PROFILE PLOT",
            network_count="2" if perfisg_variables else "1",
            geometry_blocks=geometry_blocks,
        ))

        file.write("CATALOG\n")
        all_variables = perfisp_variables + perfisg_variables
        file.write(f"{len(all_variables)}\n")

        for var in perfisp_variables:
            var_type = "BOUNDARY" if var.endswith("F") else "SECTION"
            unit = var.split('(')[1].split(')')[0]
            if is_temperature_variable(var):
                var_out = "TM"
            elif is_pressure_variable(var):
                var_out = "PT"
            else:
                idx = var.find('(')
                var_out = var[:idx].strip() if idx > 0 else var
            file.write(f"{var_out} '{var_type}:' 'BRANCH:' 'PRODUCAO' '({unit})' '{var}'\n")

        for var in perfisg_variables:
            var_type = "BOUNDARY" if var.endswith("F") else "SECTION"
            unit = var.split('(')[1].split(')')[0]
            if is_temperature_variable(var):
                var_out = "TM"
            elif is_pressure_variable(var):
                var_out = "PT"
            else:
                idx = var.find('(')
                var_out = var[:idx].strip() if idx > 0 else var
            file.write(f"{var_out} '{var_type}:' 'BRANCH:' 'SERVICO' '({unit})' '{var}'\n")

        file.write("TIME SERIES ' (S) '\n")

        for time in times:
            file.write(f"{float(time):.6e}\n")
            data_for_time = sorted_data[time]

            for var in perfisp_variables:
                values = [value for value, source in data_for_time.get(var, []) if source == 'PRODUCAO']
                if values:
                    if var.endswith("F"):
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + f" {float(values[-1]):.6e}\n")
                    else:
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + "\n")

            for var in perfisg_variables:
                values = [value for value, source in data_for_time.get(var, []) if source == 'SERVICO']
                if values:
                    if var.endswith("F"):
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + f" {float(values[-1]):.6e}\n")
                    else:
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + "\n")


# ---------------------------------------------------------------------------
# Processamento de arquivos TEND
# ---------------------------------------------------------------------------

def process_tend_file(input_file, output_directory, geometry_blocks):
    headers, variables, data, position, rotulo, celula = read_tend_file(input_file)

    lang = detect_language_from_variables(variables)
    time_col = get_time_column_name(lang)

    tpl_all_data = {}
    tpl_all_variables = []
    tpl_position_mapping = {}

    file_number = os.path.basename(input_file).split('-')[1].split('.')[0]

    for var in variables:
        var_name = var.strip()
        time_column = resolve_time_column(variables)
        if '(' in var_name and var_name != time_column and var_name not in skip_variables:
            tpl_all_variables.append((var_name, position, file_number, celula))
            tpl_position_mapping[(var_name, position, file_number)] = (rotulo, position)

    for row in data:
        time_column = resolve_time_column(variables)
        time = row[variables.index(time_column)]
        if time not in tpl_all_data:
            tpl_all_data[time] = []
        tpl_all_data[time].append([time])
        for i, value in enumerate(row):
            var_name = variables[i].strip()
            if '(' in var_name and var_name not in skip_variables:
                tpl_all_data[time][-1].append(value)

    tpl_sorted_times = sorted(tpl_all_data.keys(), key=float)
    tpl_sorted_data = [tpl_all_data[time][0] for time in tpl_sorted_times]

    output_file = os.path.join(output_directory, f"{os.path.basename(input_file).split('.')[0]}.tpl")
    write_tpl_output_file(output_file, tpl_all_variables, tpl_sorted_data, tpl_position_mapping, geometry_blocks)


def get_perfis_time_arrays(perfis_files):
    time_arrays = []
    for file_path in perfis_files:
        variables, data = read_perfis_file(file_path)
        lang = detect_language_from_variables(variables)
        time_col = get_time_column_name(lang)
        time_index = variables.index(time_col)
        time_array = tuple(row[time_index] for row in data)
        time_arrays.append(time_array)
    return time_arrays


def all_time_arrays_equal(time_arrays):
    if not time_arrays:
        return True
    first = time_arrays[0]
    return all(arr == first for arr in time_arrays)


def get_common_time_array(perfis_files):
    time_arrays = get_perfis_time_arrays(perfis_files)
    if all_time_arrays_equal(time_arrays):
        return time_arrays[0]
    return None


def get_all_unique_times(perfis_files):
    times = set()
    for file_path in perfis_files:
        variables, data = read_perfis_file(file_path)
        lang = detect_language_from_variables(variables)
        time_col = get_time_column_name(lang)
        time_index = variables.index(time_col)
        for row in data:
            times.add(row[time_index])
    return set(times)


# ---------------------------------------------------------------------------
# Processamento condicional de arquivos PERFIS
# ---------------------------------------------------------------------------

def _collect_perfis_data(perfis_files, source_label, lang=None):
    """Lê uma lista de arquivos PERFIS e retorna variables, ppl_all_data e ppl_times.

    Detecta o idioma automaticamente a partir do primeiro arquivo quando
    `lang` não é fornecido.
    """
    collected_variables = []
    ppl_all_data = {}
    ppl_times = []

    for input_file in perfis_files:
        variables, data = read_perfis_file(input_file)
        # Detecta idioma se não foi informado
        file_lang = lang or detect_language_from_variables(variables)
        time_col = get_time_column_name(file_lang)
        time_index = variables.index(time_col)

        for var in variables:
            var_name = var.strip()
            if (
                '(' in var_name
                and var_name not in collected_variables
                and var_name != time_col
                and var_name not in skip_variables
            ):
                collected_variables.append(var_name)

        for row in data:
            time = row[time_index]
            ppl_times.append(time)
            if time not in ppl_all_data:
                ppl_all_data[time] = {}
            for var, value in zip(variables, row):
                var_name = var.strip()
                if (
                    '(' in var_name
                    and var_name != time_col
                    and var_name not in skip_variables
                ):
                    if var_name not in ppl_all_data[time]:
                        ppl_all_data[time][var_name] = []
                    ppl_all_data[time][var_name].append((value, source_label))

    return collected_variables, ppl_all_data, ppl_times


def process_perfis_files_conditional(perfisp_files, perfisg_files, output_directory, geometry_blocks, input_dir_name):
    perfisp_times_set = get_all_unique_times(perfisp_files) if perfisp_files else set()
    perfisg_times_set = get_all_unique_times(perfisg_files) if perfisg_files else set()

    if perfisp_times_set and perfisg_times_set and perfisp_times_set == perfisg_times_set:
        # Consolida P e G em um único .ppl
        print(f"Gerado arquivo único: {input_dir_name}.ppl")

        perfisp_variables, ppl_data_p, _ = _collect_perfis_data(perfisp_files, 'PRODUCAO')
        perfisg_variables, ppl_data_g, _ = _collect_perfis_data(perfisg_files, 'SERVICO')

        # Mescla os dados nos mesmos instantes de tempo
        ppl_all_data = {}
        for time, entries in ppl_data_p.items():
            if time not in ppl_all_data:
                ppl_all_data[time] = {}
            ppl_all_data[time].update(entries)
        for time, entries in ppl_data_g.items():
            if time not in ppl_all_data:
                ppl_all_data[time] = {}
            for var_name, vals in entries.items():
                if var_name not in ppl_all_data[time]:
                    ppl_all_data[time][var_name] = []
                ppl_all_data[time][var_name].extend(vals)

        ppl_sorted_times = sorted(perfisp_times_set, key=float)
        output_file = os.path.join(output_directory, f"{input_dir_name}.ppl")
        write_ppl_output_file(output_file, perfisp_variables, perfisg_variables, ppl_all_data, ppl_sorted_times, geometry_blocks)

    else:
        # Arquivos P e G com arrays de tempo distintos → um .ppl para cada
        if perfisp_files:
            perfisp_variables, ppl_all_data, ppl_times = _collect_perfis_data(perfisp_files, 'PRODUCAO')
            ppl_sorted_times = sorted(set(ppl_times), key=float)
            suffix = ".ppl" if not perfisg_files else "_P.ppl"
            output_file = os.path.join(output_directory, f"{input_dir_name}{suffix}")
            write_ppl_output_file(output_file, perfisp_variables, [], ppl_all_data, ppl_sorted_times, geometry_blocks)

        if perfisg_files:
            perfisg_variables, ppl_all_data, ppl_times = _collect_perfis_data(perfisg_files, 'SERVICO')
            ppl_sorted_times = sorted(set(ppl_times), key=float)
            output_file = os.path.join(output_directory, f"{input_dir_name}_G.ppl")
            write_ppl_output_file(output_file, [], perfisg_variables, ppl_all_data, ppl_sorted_times, geometry_blocks)


# ---------------------------------------------------------------------------
# Processamento de grupos de arquivos TEND
# ---------------------------------------------------------------------------

def get_time_array(file_path):
    headers, variables, data, _, _, _ = read_tend_file(file_path)
    lang = detect_language_from_variables(variables)
    time_col = get_time_column_name(lang)
    time_index = variables.index(time_col)
    return [row[time_index] for row in data]


def group_tend_files_by_time_array(tend_files):
    time_array_groups = {}
    for file_path in tend_files:
        time_array = tuple(get_time_array(file_path))
        if time_array not in time_array_groups:
            time_array_groups[time_array] = []
        time_array_groups[time_array].append(file_path)
    return time_array_groups


def process_tend_file_group(file_group, output_directory, geometry_blocks, output_file_name):
    all_variables = []
    position_mapping = {}
    all_data = {}
    time_array = None
    var_indices = {}

    for input_file in file_group:
        headers, variables, data, position, rotulo, celula = read_tend_file(input_file)
        file_number = os.path.basename(input_file).split('-')[1].split('.')[0]

        lang = detect_language_from_variables(variables)
        time_col = get_time_column_name(lang)

        if time_array is None:
            time_index = variables.index(time_col)
            time_array = [row[time_index] for row in data]

        for i, var in enumerate(variables):
            var_name = var.strip()
            if '(' in var_name and var_name != time_col and var_name not in skip_variables:
                var_tuple = (var_name, position, file_number, celula)
                if var_tuple not in all_variables:
                    var_indices[var_tuple] = len(all_variables) + 1
                    all_variables.append(var_tuple)
                    for t in all_data:
                        while len(all_data[t]) < len(all_variables) + 1:
                            all_data[t].append(0.0)
                position_mapping[(var_name, position, file_number)] = (rotulo, position)

        for row in data:
            time = row[variables.index(time_col)]
            if time not in all_data:
                all_data[time] = [time] + [0.0] * len(all_variables)
            while len(all_data[time]) < len(all_variables) + 1:
                all_data[time].append(0.0)
            for i, var in enumerate(variables):
                var_name = var.strip()
                if '(' in var_name and var_name != time_col and var_name not in skip_variables:
                    var_tuple = (var_name, position, file_number, celula)
                    if var_tuple in var_indices:
                        idx = var_indices[var_tuple]
                        all_data[time][idx] = row[i]

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
    )


# ---------------------------------------------------------------------------
# Ponto de entrada principal
# ---------------------------------------------------------------------------

def convert_to_ppl_tpl(input_directory, output_directory):
    tend_files    = glob.glob(os.path.join(input_directory, 'TEND*.dat'))
    perfisp_files = glob.glob(os.path.join(input_directory, 'PERFISP-*.dat'))
    perfisg_files = glob.glob(os.path.join(input_directory, 'PERFISG-*.dat'))

    if not perfisp_files and not perfisg_files and not tend_files:
        raise ValueError("No PERFISP, PERFISG or TEND files found in the input directory.")

    # ---- Blocos de geometria -----------------------------------------------
    geometry_blocks = ""

    if perfisp_files:
        variables, data = read_perfis_file(perfisp_files[0])
        lang = detect_language_from_variables(variables)
        sec_col  = get_section_column_name(lang)
        elev_col = get_elevation_p_column_name(lang)
        sections   = [row[variables.index(sec_col)]  for row in data]
        elevations = [row[variables.index(elev_col)] for row in data]
        geometry_blocks += format_geometry_block(sections, elevations, "PRODUCAO")

    if perfisg_files:
        variables, data = read_perfis_file(perfisg_files[0])
        lang = detect_language_from_variables(variables)
        sec_col  = get_section_column_name(lang)
        elev_col = get_elevation_g_column_name(lang)
        sections   = [row[variables.index(sec_col)]  for row in data]
        elevations = [row[variables.index(elev_col)] for row in data]
        geometry_blocks += format_geometry_block(sections, elevations, "SERVICO")

    # ---- Arquivos TEND* ----------------------------------------------------
    if tend_files:
        time_array_groups = group_tend_files_by_time_array(tend_files)
        print(f"Encontrados {len(time_array_groups)} grupos de arquivos TEND*.dat com arrays de tempo idênticos")
        input_dir_name = os.path.basename(os.path.normpath(input_directory))

        if len(time_array_groups) == 1:
            only_group = list(time_array_groups.values())[0]
            print(f"Processando grupo único com {len(only_group)} arquivos: {[os.path.basename(f) for f in only_group]}")
            process_tend_file_group(only_group, output_directory, geometry_blocks, f"{input_dir_name}.tpl")
        else:
            max_group = max(time_array_groups.values(), key=len)
            max_group_set = set(max_group)
            print(f"Processando grupo INCOMPLETO com {len(max_group)} arquivos: {[os.path.basename(f) for f in max_group]}")
            process_tend_file_group(max_group, output_directory, geometry_blocks, f"{input_dir_name}_incompleto.tpl")

            for time_array, file_group in time_array_groups.items():
                if set(file_group) == max_group_set:
                    continue
                tags = []
                for f in file_group:
                    base = os.path.basename(f)
                    tipo = 'G' if 'TENDG' in base else 'P'
                    dist = base.split('-')[1].split('.')[0]
                    tags.append(f"{tipo}-{dist}")
                tag_str = '_'.join(tags)
                process_tend_file_group(file_group, output_directory, geometry_blocks, f"{input_dir_name}_{tag_str}.tpl")

    # ---- Arquivos PERFIS* --------------------------------------------------
    if perfisp_files or perfisg_files:
        input_dir_name = os.path.basename(os.path.normpath(input_directory))
        process_perfis_files_conditional(perfisp_files, perfisg_files, output_directory, geometry_blocks, input_dir_name)