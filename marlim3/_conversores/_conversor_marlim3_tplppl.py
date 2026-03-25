# Script de conversão para abrir arquivos de saída do Marlim3 no flotools
# Para os perfis, verifica se os tempos são os mesmos. Caso positivo, consolida resultados dos arquivos PERFISP e PERFISG em um mesmo .ppl. Caso negativo salva um <nome>_P.ppl e um <nome>_G.ppl
# Para os arquivos TENDP e TENDG, agrupa por array de tempo. Se todos os arquivos tiverem o mesmo array de tempo, consolida em um único .tpl. Caso contrário, salva um _incompleto.tpl com o maior grupo e os demais com nomes detalhados.
# O flotools consegue interpretar as unidades, exceto por algumas temperaturas que aparecem como Coulomb
# O visualizador do Olga não interpreta as unidades, precisaria ter o nome das variáveis do Olga pelos testes realizados

# Histórico de versões:
# v0 por Postal (BEU0) em dezembro de 2024 com auxílio de ChatPetrobras (GPT-4o e Claude-3.5 Sonnet)
# v1 por Postal (BEU0) em 11/06/2025 com GPT-4.1, considera a saída em múltiplos .ppl e múltiplos .tpl, somente quando necessário (diferentes amostragens de tempo)
# v2 por Postal (BEU0) em 17/06/2025 com GPT-4.1, pressão como PT e temperatura como TM para permitir Calculations padrão do flotools como DTHYD e remoção das unidades no nome das variáveis

import os 
import glob 
import sys

# Lista de variáveis que devem ser ignoradas no processamento
skip_variables = [
    "Comprimento (m) Fronteira F",
    "Comprimento (m) Centro Volume C",
    "Elevacao (m) F",
    "Elevacao (m) C",
    "Comprimento (m) centro de Volume C",
    "comprimento_fundoPoco (m) F",
    "Profundidade (m) F",
    "Profundidade (m) C"
]

# Cabeçalho padrão para os arquivos de saída
header_template = """'Marlim3' 
{plot_type} 
INPUT FILE 
'Marlim3.genkey' 
PVT FILE 
'./3phase.tab' 
DATE 
'24-10-23 12:59:00' 
PROJECT 
'Marlim 3' 
TITLE 
'Convertido a partir da saída do Marlim 3' 
AUTHOR 
'Petrobras' 
NETWORK 
{network_count} 
GEOMETRY' (M) ' 
{geometry_blocks}"""

def read_tend_file(file_path): 
    with open(file_path, 'r', encoding='utf-8') as file: 
        lines = file.readlines()     
    headers = lines[:3] 
    variable_line = lines[3] 
    data_lines = lines[4:] 
     
    # Extract the position, Rotulo, and celula from the headers 
    position = None 
    rotulo = None 
    celula = None 
    for header in headers: 
        if 'Comprimento a partir do Fundo de Poco (m)' in header: 
            position = header.split('=')[-1].strip() 
        if 'Comprimento a partir da Plataforma (m)' in header:
            position = header.split('=')[-1].strip() 
        if 'Rotulo' in header: 
            rotulo = header.split('=')[-1].strip() 
        if 'Indice da Celula' in header: 
            celula = header.split('=')[1].strip() 
     
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
        file.write(header_template.format(plot_type="TIME PLOT", network_count="2" if "SERVICO" in geometry_blocks else "1", geometry_blocks=geometry_blocks)) 
         
        file.write("CATALOG\n") 
        file.write(f"{len(all_variables)}\n") 
         
        for var, position, file_number, celula in all_variables: 
            if '(' in var: 
                rotulo, comprimento = position_mapping[(var, position, file_number)] 
                var_type = "BOUNDARY" if var.endswith("F") else "SECTION" 
                unit = var.split('(')[1].split(')')[0] 
                if var.strip() == "Temperatura (C)":
                    var_out = "TM"
                elif var.strip() == "Pressao (kgf/cm2)":
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
        file.write(header_template.format(plot_type="PROFILE PLOT", network_count="2" if perfisg_variables else "1", geometry_blocks=geometry_blocks)) 
         
        file.write("CATALOG\n") 
        all_variables = perfisp_variables + perfisg_variables 
        file.write(f"{len(all_variables)}\n") 
         
        for var in perfisp_variables: 
            var_type = "BOUNDARY" if var.endswith("F") else "SECTION" 
            unit = var.split('(')[1].split(')')[0] 
            if var.strip().startswith("Temperatura (C)"):
                var_out = "TM"
            elif var.strip().startswith("Pressao (kgf/cm2)"):
                var_out = "PT"
            else:
                # Remove tudo a partir do primeiro parêntese (inclusive)
                idx = var.find('(')
                var_out = var[:idx].strip() if idx > 0 else var
            file.write(f"{var_out} '{var_type}:' 'BRANCH:' 'PRODUCAO' '({unit})' '{var}'\n") 
         
        for var in perfisg_variables: 
            var_type = "BOUNDARY" if var.endswith("F") else "SECTION" 
            unit = var.split('(')[1].split(')')[0]
            if var.strip().startswith("Temperatura (C)"):
                var_out = "TM"
            elif var.strip().startswith("Pressao (kgf/cm2)"):
                var_out = "PT"
            else:
                idx = var.find('(')
                var_out = var[:idx].strip() if idx > 0 else var
            file.write(f"{var_out} '{var_type}:' 'BRANCH:' 'SERVICO' '({unit})' '{var}'\n") 
         
        file.write("TIME SERIES ' (S) '\n") 
         
        for time in times: 
            file.write(f"{float(time):.6e}\n") 
            data_for_time = sorted_data[time] 
             
            # Process PERFISP (PRODUCAO) variables 
            for var in perfisp_variables: 
                values = [value for value, source in data_for_time.get(var, []) if source == 'PRODUCAO']
                if values:
                    if var.endswith("F"): # BOUNDARY variable 
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + f" {float(values[-1]):.6e}\n") 
                    else: # SECTION variable 
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + "\n") 
             
            # Process PERFISG (SERVICO) variables 
            for var in perfisg_variables: 
                values = [value for value, source in data_for_time.get(var, []) if source == 'SERVICO']
                if values:
                    if var.endswith("F"): # BOUNDARY variable 
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + f" {float(values[-1]):.6e}\n") 
                    else: # SECTION variable 
                        file.write(" ".join(f"{float(value):.6e}" for value in values) + "\n") 

def process_tend_file(input_file, output_directory, geometry_blocks):
    headers, variables, data, position, rotulo, celula = read_tend_file(input_file)
    
    tpl_all_data = {}
    tpl_all_variables = []
    tpl_position_mapping = {}

    file_number = os.path.basename(input_file).split('-')[1].split('.')[0]

    for var in variables:
        var_name = var.strip()
        if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
            tpl_all_variables.append((var_name, position, file_number, celula))
            tpl_position_mapping[(var_name, position, file_number)] = (rotulo, position)

    for row in data:
        time = row[variables.index("Tempo (s)")]
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
        time_index = variables.index("Tempo (s)")
        time_array = tuple(row[time_index] for row in data)
        time_arrays.append(time_array)
    return time_arrays

def all_time_arrays_equal(time_arrays):
    if not time_arrays:
        return True
    first = time_arrays[0]
    return all(arr == first for arr in time_arrays)

def get_common_time_array(perfis_files):
    # Retorna o array de tempo do primeiro arquivo, se todos forem iguais
    time_arrays = get_perfis_time_arrays(perfis_files)
    if all_time_arrays_equal(time_arrays):
        return time_arrays[0]
    return None

def get_all_unique_times(perfis_files):
    times = set()
    for file_path in perfis_files:
        variables, data = read_perfis_file(file_path)
        time_index = variables.index("Tempo (s)")
        for row in data:
            times.add(row[time_index])
    return set(times)

def process_perfis_files_conditional(perfisp_files, perfisg_files, output_directory, geometry_blocks, input_dir_name):
    # Avalia conjuntos de tempo únicos
    perfisp_times_set = get_all_unique_times(perfisp_files) if perfisp_files else set()
    perfisg_times_set = get_all_unique_times(perfisg_files) if perfisg_files else set()
    # Se ambos existem e os conjuntos de tempo são iguais, consolida
    if perfisp_times_set and perfisg_times_set and perfisp_times_set == perfisg_times_set:
        print(f"Gerado arquivo único: {input_dir_name}.ppl")
        ppl_all_data = {}
        perfisp_variables = []
        perfisg_variables = []
        ppl_times = sorted(perfisp_times_set, key=float)
        # ...processamento igual ao antigo, mas para ambos juntos...
        for input_file in perfisp_files:
            variablesp, datap = read_perfis_file(input_file)
            for var in variablesp:
                var_name = var.strip()
                if '(' in var_name and var_name not in perfisp_variables and var_name != "Tempo (s)" and var_name not in skip_variables:
                    perfisp_variables.append(var_name)
            for row in datap:
                time = row[variablesp.index("Tempo (s)")]
                if time not in ppl_all_data:
                    ppl_all_data[time] = {}
                for var, value in zip(variablesp, row):
                    var_name = var.strip()
                    if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
                        if var_name not in ppl_all_data[time]:
                            ppl_all_data[time][var_name] = []
                        ppl_all_data[time][var_name].append((value, 'PRODUCAO'))
        for input_file in perfisg_files:
            variablesg, datag = read_perfis_file(input_file)
            for var in variablesg:
                var_name = var.strip()
                if '(' in var_name and var_name not in perfisg_variables and var_name != "Tempo (s)" and var_name not in skip_variables:
                    perfisg_variables.append(var_name)
            for row in datag:
                time = row[variablesg.index("Tempo (s)")]
                if time not in ppl_all_data:
                    ppl_all_data[time] = {}
                for var, value in zip(variablesg, row):
                    var_name = var.strip()
                    if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
                        if var_name not in ppl_all_data[time]:
                            ppl_all_data[time][var_name] = []
                        ppl_all_data[time][var_name].append((value, 'SERVICO'))
        ppl_sorted_times = sorted(ppl_times, key=float)
        output_file = os.path.join(output_directory, f"{input_dir_name}.ppl")
        write_ppl_output_file(output_file, perfisp_variables, perfisg_variables, ppl_all_data, ppl_sorted_times, geometry_blocks)
    else:
        # Gera um ppl para cada grupo
        if perfisp_files:
            ppl_all_data = {}
            perfisp_variables = []
            ppl_times = []
            for input_file in perfisp_files:
                variablesp, datap = read_perfis_file(input_file)
                for var in variablesp:
                    var_name = var.strip()
                    if '(' in var_name and var_name not in perfisp_variables and var_name != "Tempo (s)" and var_name not in skip_variables:
                        perfisp_variables.append(var_name)
                for row in datap:
                    time = row[variablesp.index("Tempo (s)")]
                    ppl_times.append(time)
                    if time not in ppl_all_data:
                        ppl_all_data[time] = {}
                    for var, value in zip(variablesp, row):
                        var_name = var.strip()
                        if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
                            if var_name not in ppl_all_data[time]:
                                ppl_all_data[time][var_name] = []
                            ppl_all_data[time][var_name].append((value, 'PRODUCAO'))
            ppl_sorted_times = sorted(set(ppl_times), key=float)
            if not perfisg_files:
                output_file = os.path.join(output_directory, f"{input_dir_name}.ppl")
            else:
                output_file = os.path.join(output_directory, f"{input_dir_name}_P.ppl")
            write_ppl_output_file(output_file, perfisp_variables, [], ppl_all_data, ppl_sorted_times, geometry_blocks)
        if perfisg_files:
            ppl_all_data = {}
            perfisg_variables = []
            ppl_times = []
            for input_file in perfisg_files:
                variablesg, datag = read_perfis_file(input_file)
                for var in variablesg:
                    var_name = var.strip()
                    if '(' in var_name and var_name not in perfisg_variables and var_name != "Tempo (s)" and var_name not in skip_variables:
                        perfisg_variables.append(var_name)
                for row in datag:
                    time = row[variablesg.index("Tempo (s)")]
                    ppl_times.append(time)
                    if time not in ppl_all_data:
                        ppl_all_data[time] = {}
                    for var, value in zip(variablesg, row):
                        var_name = var.strip()
                        if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
                            if var_name not in ppl_all_data[time]:
                                ppl_all_data[time][var_name] = []
                            ppl_all_data[time][var_name].append((value, 'SERVICO'))
            ppl_sorted_times = sorted(set(ppl_times), key=float)
            output_file = os.path.join(output_directory, f"{input_dir_name}_G.ppl")
            write_ppl_output_file(output_file, [], perfisg_variables, ppl_all_data, ppl_sorted_times, geometry_blocks)

def get_time_array(file_path):
    headers, variables, data, _, _, _ = read_tend_file(file_path)
    time_index = variables.index("Tempo (s)")
    return [row[time_index] for row in data]

def group_tend_files_by_time_array(tend_files):
    time_array_groups = {}
    for file_path in tend_files:
        time_array = tuple(get_time_array(file_path))  # tuple para poder usar como chave do dicionário
        if time_array not in time_array_groups:
            time_array_groups[time_array] = []
        time_array_groups[time_array].append(file_path)
    return time_array_groups

def process_tend_file_group(file_group, output_directory, geometry_blocks, output_file_name):
    all_variables = []  # lista de (var_name, position, file_number, celula)
    position_mapping = {}
    all_data = {}
    time_array = None
    var_indices = {}  # mapeia (var_name, position, file_number, celula) -> índice na linha

    # Processa cada arquivo do grupo
    for input_file in file_group:
        headers, variables, data, position, rotulo, celula = read_tend_file(input_file)
        file_number = os.path.basename(input_file).split('-')[1].split('.')[0]
        # Guarda o array de tempo do primeiro arquivo (todos são iguais no grupo)
        if time_array is None:
            time_index = variables.index("Tempo (s)")
            time_array = [row[time_index] for row in data]
        # Processa as variáveis
        for i, var in enumerate(variables):
            var_name = var.strip()
            if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
                var_tuple = (var_name, position, file_number, celula)
                if var_tuple not in all_variables:
                    var_indices[var_tuple] = len(all_variables) + 1  # +1 porque coluna 0 é tempo
                    all_variables.append(var_tuple)
                    # Expande todas as linhas já existentes em all_data para o novo tamanho
                    for t in all_data:
                        while len(all_data[t]) < len(all_variables) + 1:
                            all_data[t].append(0.0)
                position_mapping[(var_name, position, file_number)] = (rotulo, position)
        # Processa os dados
        for row in data:
            time = row[variables.index("Tempo (s)")]
            if time not in all_data:
                all_data[time] = [time] + [0.0]*len(all_variables)
            # Garante que a linha está no tamanho correto
            while len(all_data[time]) < len(all_variables) + 1:
                all_data[time].append(0.0)
            for i, var in enumerate(variables):
                var_name = var.strip()
                if '(' in var_name and var_name != "Tempo (s)" and var_name not in skip_variables:
                    var_tuple = (var_name, position, file_number, celula)
                    if var_tuple in var_indices:
                        idx = var_indices[var_tuple]
                        all_data[time][idx] = row[i]
    # Ordena os dados por tempo
    sorted_times = sorted(all_data.keys(), key=float)
    sorted_data = []
    for time in sorted_times:
        row = all_data[time]
        # Garante que a linha está no tamanho correto
        while len(row) < len(all_variables) + 1:
            row.append(0.0)
        sorted_data.append(row)
    # Escreve o arquivo consolidado
    write_tpl_output_file(
        os.path.join(output_directory, output_file_name),
        all_variables,
        sorted_data,
        position_mapping,
        geometry_blocks
    )

def convert_to_ppl_tpl(input_directory, output_directory): 
    tend_files = glob.glob(os.path.join(input_directory, 'TEND*.dat')) 
    perfisp_files = glob.glob(os.path.join(input_directory, 'PERFISP-*.dat')) 
    perfisg_files = glob.glob(os.path.join(input_directory, 'PERFISG-*.dat')) 
     
    if not perfisp_files and not perfisg_files: 
        raise ValueError("No PERFISP or PERFISG files found in the input directory.") 

    # Create geometry blocks 
    geometry_blocks = "" 
    if perfisp_files: 
        variables, data = read_perfis_file(perfisp_files[0]) 
        sections = [row[variables.index('Comprimento (m) Fronteira F')] for row in data] 
        elevations = [row[variables.index('Elevacao (m) F')] for row in data] 
        geometry_blocks += format_geometry_block(sections, elevations, "PRODUCAO") 

    if perfisg_files: 
        variables, data = read_perfis_file(perfisg_files[0]) 
        sections = [row[variables.index('Comprimento (m) Fronteira F')] for row in data] 
        elevations = [row[variables.index('Profundidade (m) F')] for row in data] 
        geometry_blocks += format_geometry_block(sections, elevations, "SERVICO") 

    # Agrupa os arquivos TEND*.dat por array de tempo
    if tend_files:
        time_array_groups = group_tend_files_by_time_array(tend_files)
        print(f"Encontrados {len(time_array_groups)} grupos de arquivos TEND*.dat com arrays de tempo idênticos")
        input_dir_name = os.path.basename(os.path.normpath(input_directory))
        if len(time_array_groups) == 1:
            # Todos compartilham o mesmo array de tempo
            only_group = list(time_array_groups.values())[0]
            print(f"Processando grupo único com {len(only_group)} arquivos: {[os.path.basename(f) for f in only_group]}")
            process_tend_file_group(only_group, output_directory, geometry_blocks, f"{input_dir_name}.tpl")
        else:
            # Identifica o maior grupo
            max_group = max(time_array_groups.values(), key=len)
            max_group_set = set(max_group)
            # Salva o maior grupo como _incompleto.tpl
            print(f"Processando grupo INCOMPLETO com {len(max_group)} arquivos: {[os.path.basename(f) for f in max_group]}")
            process_tend_file_group(max_group, output_directory, geometry_blocks, f"{input_dir_name}_incompleto.tpl")
            # Salva os demais grupos com nomes detalhados
            for time_array, file_group in time_array_groups.items():
                if set(file_group) == max_group_set:
                    continue
                # Monta o nome do arquivo conforme solicitado
                tags = []
                for f in file_group:
                    base = os.path.basename(f)
                    tipo = 'G' if 'TENDG' in base else 'P'
                    dist = base.split('-')[1].split('.')[0]
                    tags.append(f"{tipo}-{dist}")
                tag_str = '_'.join(tags)
                process_tend_file_group(file_group, output_directory, geometry_blocks, f"{input_dir_name}_{tag_str}.tpl")

    # Novo: nome da pasta para os arquivos ppl
    input_dir_name = os.path.basename(os.path.normpath(input_directory))
    process_perfis_files_conditional(perfisp_files, perfisg_files, output_directory, geometry_blocks, input_dir_name)