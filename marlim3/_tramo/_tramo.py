import importlib.resources as pkg_resources
import pandas as pd
import json
import os
import shutil
import requests
from .._conversores._conversor_marlim3_tplppl import convert_to_ppl_tpl
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.cm as cm
from .._plots._plots_perfis import _plotar_perfis, _plotar_perfis_animados
from .._plots._plots_tends import _plotar_tendencias
from .._plots._plots_geometria import _plotar_geometria
import pathlib
import re
import IPython.display
import platform

import subprocess
import time
from threading import Thread

class Tramo:

    def __init__(self, 
                 versao='1.0',
                 versaoJson='1.3.9',
                 sistema='MULTIFASICO',
                 configuracaoInicial=None,
                 tabela=None,
                 tempo=None,
                 correcao=None,
                 fluidosProducao=None,
                 fluidoComplementar=None,
                 fluidoGas=None,
                 material=None,
                 secaoTransversal=None,
                 dutosProducao=None,
                 dutosServico=None,
                 separador=None,
                 gasInj=None,
                 CondicaoContPocInjec=None,
                 ipr=None,
                 fonteMassa=None,
                 fonteGas=None,
                 fonteLiquido=None,
                 fontePressao=None,
                 fontePoroRadial=None,
                 fontePoro2D=None,
                 valvula=None,
                 fonteGasLift=None,
                 chokeSup=None,
                 chokeInj=None,
                 master1=None,
                 master2=None,
                 bcs=None,
                 bombaVolumetrica=None,
                 deltaPressao=None,
                 pig=None,
                 parafina=None,
                 perfilProducao=None,
                 perfilServico=None,
                 tendP=None,
                 tendS=None,
                 perfisTransP=None,
                 perfisTransS=None,
                 tendTransP=None,
                 tendTransS=None,
                 tela=None,
                 nome_tramo=None):

        self.sistema = sistema
        self.versao = versao
        self.versaoJson = versaoJson
        
        # configuracoes gerais
        self.configuracaoInicial = configuracaoInicial if configuracaoInicial is not None else {}
        self.tabela = tabela if tabela is not None else {}
        self.tempo = tempo if tempo is not None else {}
        self.correcao = correcao if correcao is not None else {}

        # fluidos
        self.fluidosProducao = fluidosProducao if fluidosProducao is not None else []
        self.fluidoComplementar = fluidoComplementar if fluidoComplementar is not None else {}
        self.fluidoGas = fluidoGas if fluidoGas is not None else {}

        # sistema de produção
        self.material = material if material is not None else []
        self.secaoTransversal = secaoTransversal if secaoTransversal is not None else []
        self.dutosProducao = dutosProducao if dutosProducao is not None else []
        self.dutosServico = dutosServico if dutosServico is not None else []

        # condicoes de contorno
        self.separador = separador if separador is not None else {}
        self.gasInj = gasInj if gasInj is not None else {}
        self.CondicaoContPocInjec = CondicaoContPocInjec if CondicaoContPocInjec is not None else {}

        # fontes
        self.ipr = ipr if ipr is not None else []
        self.fonteMassa = fonteMassa if fonteMassa is not None else []
        self.fonteGas = fonteGas if fonteGas is not None else []
        self.fonteLiquido = fonteLiquido if fonteLiquido is not None else []
        self.fontePressao = fontePressao if fontePressao is not None else []
        self.fontePoroRadial = fontePoroRadial if fontePoroRadial is not None else []
        self.fontePoro2D = fontePoro2D if fontePoro2D is not None else []

        # valvulas
        self.valvula = valvula if valvula is not None else []
        self.fonteGasLift = fonteGasLift if fonteGasLift is not None else []
        self.chokeSup = chokeSup if chokeSup is not None else []
        self.chokeInj = chokeInj if chokeInj is not None else []
        self.master1 = master1 if master1 is not None else []
        self.master2 = master2 if master2 is not None else []

        # bombas
        self.bcs = bcs if bcs is not None else []
        self.bombaVolumetrica = bombaVolumetrica if bombaVolumetrica is not None else []
        self.deltaPressao = deltaPressao if deltaPressao is not None else []

        # pig
        self.pig = pig if pig is not None else []

        # garesc
        self.parafina = parafina if parafina is not None else {}

        # resultados
        self.perfilProducao = perfilProducao if perfilProducao is not None else {}
        self.perfilServico = perfilServico if perfilServico is not None else {}
        self.tendP = tendP if tendP is not None else []
        self.tendS = tendS if tendS is not None else []
        self.perfisTransP = perfisTransP if perfisTransP is not None else {}
        self.perfisTransS = perfisTransS if perfisTransS is not None else {}
        self.tendTransP = tendTransP if tendTransP is not None else []
        self.tendTransS = tendTransS if tendTransS is not None else []
        self.tela = tela if tela is not None else []

        self.json_entrada_keys = set(self.__dict__.keys())

        self.resultados = {}

        self.nome_tramo = nome_tramo
            
    ###########################################################################
        
    def to_json(self, filename='marlim3_model', generate_empty_fields=False):

        if not filename.endswith('.json'):
            file_path = './' + filename + '.json'
        else:
            file_path = './' + filename

        self.versao = '1.0'
        self.versaoJson = '1.3.9'

        def filter_empty_objs(item):
            if isinstance(item, list):
                return [filter_empty_objs(i) for i in item] or (None if not generate_empty_fields else [])
            elif isinstance(item, dict):
                filtered_dict = {k: filter_empty_objs(v) for k, v in item.items()}
                return filtered_dict or (None if not generate_empty_fields else {})
            return item

        if generate_empty_fields:
            filtered_data = {
                key: filter_empty_objs(value)
                for key, value in self.__dict__.items()
                if key in self.json_entrada_keys
            }
        else:
            filtered_data = {
                key: filter_empty_objs(value)
                for key, value in self.__dict__.items()
                if key in self.json_entrada_keys and value is not None
            }
            filtered_data = {k: v for k, v in filtered_data.items() if v is not None}

        with open(file_path, 'w', encoding='utf-8') as file:
            json.dump(filtered_data, file, indent=2, ensure_ascii=True)

    ###########################################################################

    def from_json(self, json_input, is_string=False):

        if is_string:
            data = json_input
        else:
            with open(json_input, 'r') as file:
                data = json.load(file)
            if not hasattr(self, 'label'):
                if json_input.endswith('.json'):
                    label = json_input[:-5]
                else:
                    label = json_input
                self.label = label

        self.sistema = data.get('sistema')
        self.versao = data.get('versao')
        self.versaoJson = data.get('versaoJson')
        self.configuracaoInicial = data.get('configuracaoInicial')
        self.tabela = data.get('tabela')
        self.tempo = data.get('tempo')
        self.correcao = data.get('correcao')
        self.fluidosProducao = data.get('fluidosProducao', [])
        self.fluidoComplementar = data.get('fluidoComplementar')
        self.fluidoGas = data.get('fluidoGas')
        self.material = data.get('material', [])
        self.secaoTransversal = data.get('secaoTransversal', [])
        self.dutosProducao = data.get('dutosProducao', [])
        self.dutosServico = data.get('dutosServico', [])
        self.separador = data.get('separador')
        self.gasInj = data.get('gasInj')
        self.CondicaoContPocInjec = data.get('CondicaoContPocInjec')
        self.ipr = data.get('ipr', [])
        self.fonteMassa = data.get('fonteMassa', [])
        self.fonteGas = data.get('fonteGas', [])
        self.fonteLiquido = data.get('fonteLiquido', [])
        self.fontePressao = data.get('fontePressao', [])
        self.fontePoroRadial = data.get('fontePoroRadial', [])
        self.fontePoro2D = data.get('fontePoro2D', [])
        self.valvula = data.get('valvula', [])
        self.fonteGasLift = data.get('fonteGasLift', [])
        self.chokeSup = data.get('chokeSup', [])
        self.chokeInj = data.get('chokeInj', [])
        self.master1 = data.get('master1', [])
        self.master2 = data.get('master2', [])
        self.bcs = data.get('bcs', [])
        self.bombaVolumetrica = data.get('bombaVolumetrica', [])
        self.deltaPressao = data.get('deltaPressao', [])
        self.pig = data.get('pig', [])
        self.parafina = data.get('parafina')
        self.perfilProducao = data.get('perfilProducao')
        self.perfilServico = data.get('perfilServico')
        self.tendP = data.get('tendP')
        self.tendS = data.get('tendS')
        self.perfisTransP = data.get('perfisTransP')
        self.perfisTransS = data.get('perfisTransS')
        self.tendTransP = data.get('tendTransP')
        self.tendTransS = data.get('tendTransS')
        self.tela = data.get('tela')

    ###########################################################################

    def simular(self, kind='PRODUTOR', 
                label='marlim3_model', 
                diretorio='marlim3_resultados',
                gerar_ppl_tpl = False,
                simulation_id=None,
                websocket_handler=None,
                tracker=None,
                sanitized=False):
        
        # Verificar o sistema operacional
        if platform.system() == 'Windows':
            executavel_name = 'Marlim3.exe'
        else:
            executavel_name = 'Marlim3'

        if label != 'marlim3_model':
            self.label = label

        with pkg_resources.path('marlim3', 
                                executavel_name) as executavel:

            filename = label+'.json'
            
            if not os.path.isdir(diretorio):
                os.mkdir(diretorio)
            else:
                for item in os.listdir(diretorio):
                    item_path = os.path.join(diretorio, item)
                    if os.path.isfile(item_path):
                        os.remove(item_path)
                    elif os.path.isdir(item_path):
                        shutil.rmtree(item_path)

            if sanitized:

                if self.configuracaoInicial:
                    if 'pvtsimArq' in self.configuracaoInicial and self.configuracaoInicial['pvtsimArq']:
                        self.configuracaoInicial['pvtsimArq'] = sanitize_filename(str(self.configuracaoInicial['pvtsimArq']))
                
                if self.CondicaoContPocInjec:
                    if 'arquivoPvtsim' in self.CondicaoContPocInjec and self.CondicaoContPocInjec['arquivoPvtsim']:
                        self.CondicaoContPocInjec['arquivoPvtsim'] = sanitize_filename(str(self.CondicaoContPocInjec['arquivoPvtsim']))
                
                if self.parafina:
                    if 'arquivoWax' in self.parafina and self.parafina['arquivoWax']:
                        self.parafina['arquivoWax'] = sanitize_filename(str(self.parafina['arquivoWax']))
                
                if self.tempo:
                    if 'SnapShotArq' in self.tempo and self.tempo['SnapShotArq']:
                        self.tempo['SnapShotArq'] = sanitize_filename(str(self.tempo['SnapShotArq']))

            self.to_json(label)

            if self.sistema == 'INJETOR':
                comando_simulacao = f'{executavel} -d {diretorio} -i {filename} -s INJETOR'
            else:
                comando_simulacao = f'{executavel} -d {diretorio} -i {filename}'

            # Iniciar o processo de simulação
            if platform.system() == "Windows":
                process = subprocess.Popen(
                    comando_simulacao,
                    shell=True,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True
                )
                stdout, _ = process.communicate()
                print(stdout) 
            else:
                process = subprocess.Popen(
                    comando_simulacao,
                    shell=True,
                    preexec_fn=os.setsid
                )

            if tracker:
                tracker.process_pid = process.pid
                tracker.save()

            # Iniciar thread para monitorar o arquivo LogEvento.dat
            log_thread = Thread(target=self.monitor_log_evento, args=(diretorio, simulation_id,
                                                                      websocket_handler))
            log_thread.start()

            # Aguardar o término do processo
            process.wait()

            time.sleep(1)

            # Parar o monitoramento do log
            self.stop_monitoring = True
            log_thread.join()

            try:
            
                if self.perfilProducao is not None:
                    self.resultados['perfilProducao'] = self.processar_perfis(diretorio)
                if "linhaGas" in self.configuracaoInicial:
                    if self.perfilServico is not None and self.configuracaoInicial['linhaGas']:
                        self.resultados['perfilServico'] = self.processar_perfis(diretorio, linha = 'servico')
                if self.tendP is not None:
                    self.resultados['tendP'] = self.processar_tendencias(diretorio)
                if "linhaGas" in self.configuracaoInicial:
                    if self.tendS is not None and self.configuracaoInicial['linhaGas']:
                        self.resultados['tendS'] = self.processar_tendencias(diretorio, linha = 'servico')

                if is_dict_only_none_or_empty_dict(self.resultados):
                    raise Exception

            except:

                # Primeira parte
                for origin_file in [os.path.join(os.getcwd(), 'simulacao.log')]:
                    try:
                        destination_file = os.path.join(diretorio, os.path.basename(origin_file))
                        if os.path.exists(destination_file):
                            os.remove(destination_file)
                        if os.path.exists(origin_file):
                            shutil.move(origin_file, diretorio)
                            
                        log_file_path = os.path.join(diretorio, 'simulacao.log')
                        with open(log_file_path, 'r') as f:
                            log_data = json.load(f)
                            
                        # Procurar pela última mensagem de erro nos logs
                        error_message = None
                        if 'resultadoSimulacao' in log_data and 'logs' in log_data['resultadoSimulacao']:
                            for log_entry in reversed(log_data['resultadoSimulacao']['logs']):
                                if log_entry['log'] == 'FALHA':
                                    error_message = f"{log_entry['descricao']}"
                                    if log_entry['propriedade']:
                                        error_message += f" (Propriedade: {log_entry['propriedade']});"
                                    if log_entry['causa']:
                                        error_message += f" (Causa: {log_entry['causa']})"
                                    break
                        if error_message:
                            raise Exception(error_message)
                    except FileNotFoundError:
                        pass

            for origin_file in [os.path.join(os.getcwd(), 'simulacao.log'),
                                os.path.join(os.getcwd(),'EventosExternos.dat')]:
                try:
                    if os.path.exists(origin_file):
                        destination_file = os.path.join(diretorio, os.path.basename(origin_file))
                        if os.path.exists(destination_file):
                            os.remove(destination_file)
                        shutil.move(origin_file, diretorio)
                except FileNotFoundError:
                    pass  # Ignora se o arquivo não existir

            if gerar_ppl_tpl:
                diretorio_tpl_ppl = diretorio+'_ppl_tpl'
                if not os.path.isdir(diretorio_tpl_ppl):
                    os.mkdir(diretorio_tpl_ppl)
                convert_to_ppl_tpl(diretorio, diretorio_tpl_ppl)

    ###########################################################################

    def monitor_log_evento(self, diretorio, simulation_id, websocket_handler=None):
        self.stop_monitoring = False
        log_file = os.path.join(diretorio, 'LogEvento.dat')
        last_position = 0
        last_modified_time = 0
        wait_after_stop = 2 
        new_content = ""
        
        try:
            # Enviar mensagem inicial usando o handler
            if websocket_handler:
                websocket_handler.send_message("simulation.start", "Iniciando simulação...")
            
            # Loop principal de monitoramento
            while not self.stop_monitoring or time.time() - last_modified_time < wait_after_stop:
                try:
                    if os.path.exists(log_file):
                        current_modified_time = os.path.getmtime(log_file)
                        if current_modified_time > last_modified_time:
                            last_modified_time = current_modified_time
                            with open(log_file, 'r') as f:
                                f.seek(last_position)
                                new_content = f.read()
                                if new_content:
                                    last_position = f.tell()
                                    # Enviar conteúdo pelo websocket
                                    if websocket_handler:
                                        websocket_handler.send_message("simulation.progress", new_content.strip())
                    time.sleep(0.5)
                except Exception as inner_e:
                    break
                    
        except Exception as e:
            self.stop_monitoring = True

    ###########################################################################

    def processar_perfis(self, diretorio='marlim3_resultados', linha='producao'): 
        temp_dfs = [] 
     
        if linha == 'producao': 
            PERFIS = 'PERFISP' 
        elif linha == 'servico': 
            PERFIS = 'PERFISG' 
        else: 
            print('argumento linha só pode ser producao ou servico') 
            return None 
     
        for root, dirs, files in os.walk(diretorio): 
            for filename in files: 
                if filename.startswith(PERFIS) and filename.endswith(".dat"): 
                    file_path = os.path.join(root, filename) 
                    df = pd.read_csv(file_path, sep=';', skiprows=0, header=1) 
                    df.columns = [col.strip() for col in df.columns] 
                    df['Tempo (s)'] = df['Tempo (s)'].astype(int) 
                    temp_dfs.append(df) 
     
        if not temp_dfs: 
            print(f"Nenhum arquivo {PERFIS}*.dat encontrado em {diretorio}") 
            return None 
     
        concatenated_df = pd.concat(temp_dfs) 
         
        concatenated_df.set_index(['Tempo (s)', concatenated_df.index], inplace=True) 
     
        concatenated_df.index.set_levels([concatenated_df.index.levels[0],  
                                          concatenated_df.index.levels[1]],  
                                         level=[0, 1]) 
     
        concatenated_df = concatenated_df.loc[:,  
                             ~concatenated_df.columns.str.contains('^Unnamed')] 
     
        return concatenated_df 

    ###########################################################################
     
    def processar_tendencias(self, diretorio='marlim3_resultados', linha='producao'): 
        if linha not in ['producao', 'servico']: 
            raise ValueError(f"Argumento 'linha' deve ser 'producao' ou 'servico', não '{linha}'") 
     
        TEND = 'TENDP' if linha == 'producao' else 'TENDG' 
        resultados = {} 
     
        for root, dirs, files in os.walk(diretorio): 
            for filename in files: 
                if filename.startswith(TEND) and filename.endswith(".dat"): 
                    file_path = os.path.join(root, filename) 
                    try: 
                        numero_arquivo = int(re.search(r'\d+', os.path.splitext(filename)[0]).group()) 
     
                        with open(file_path, 'r') as f: 
                            linha1 = f.readline().strip() 
                            linha2 = f.readline().strip() 
                            linha3 = f.readline().strip() 
     
                        comprimento = int(re.search(r'= (\d+)', linha1).group(1)) 
                        rotulo = linha2.split('=')[1].strip() 
                        indice_celula = int(re.search(r'= (\d+)', linha3).group(1)) 
     
                        df = pd.read_csv(file_path, sep=';', skiprows=3, header=0) 
     
                        df.columns = [col.strip() for col in df.columns] 
                        df['Tempo (s)'] = df['Tempo (s)'].astype(float) 
                        df.set_index(['Tempo (s)'], inplace=True) 
                        df = df.loc[:, ~df.columns.str.contains('^Unnamed')] 
     
                        for col in df.columns: 
                            df[col] = df[col].astype(float) 
     
                        df.attrs['comprimento_medido'] = comprimento 
                        df.attrs['rotulo'] = rotulo 
                        df.attrs['indice_celula'] = indice_celula 
     
                        resultados[numero_arquivo] = df 
     
                    except Exception as e: 
                        print(f"Erro ao processar o arquivo {file_path}: {str(e)}") 
     
        return dict(sorted(resultados.items())) 

    ###########################################################################

    def plotar_perfis(self, linha = 'producao', rotulos = None, 
                      gradiente = False, indicar_anm = False):

        posicao_anm = None

        if linha == 'producao':
            PERFIS = 'perfilProducao'
        elif linha == 'servico':
            PERFIS = 'perfilServico'
        else:
            print('argumento linha só pode ser producao ou servico')         
    
        try:
            if indicar_anm:
                posicao_anm = self.master1['comprimentoMedido']
        except:
            print('posição da master1 não definida no modelo!')
            
    
        rotulos = rotulos if rotulos is not None else []

        fig, ax = _plotar_perfis(self.resultados[PERFIS], linha = linha,
                                 rotulos=rotulos, gradiente = gradiente,
                                 posicao_anm = posicao_anm)
        
        return fig, ax

    ###########################################################################

    def plotar_perfis_animados(self, linha = 'producao'):

        return _plotar_perfis_animados(self.resultados['perfilProducao'],
                                       linha = linha)

    ###########################################################################

    def plotar_tendencias(self, linha = 'producao', posicoes = None):

        if linha == 'producao':
            TEND = 'tendP'
        elif linha == 'servico':
            TEND = 'tendS'
        else:
            print('argumento linha só pode ser producao ou servico')         

        fig, ax = _plotar_tendencias(self.resultados[TEND], posicoes = posicoes)

        return fig, ax
    
    ###########################################################################

    def plotar_geometria(self):

        _plotar_geometria(self)
    
    ###########################################################################

    def exibir_tabela(self, campo):

        if campo == 'secaoTransversal':
            cols = ['camadas']
            index_cols = None
        elif campo == 'dutosProducao' or  campo == 'dutosServico':
            cols=['discretizacao','condicoesIniciais']
            index_cols = [None,'compInter']
        else:
            cols = None
            index_cols = None

        estilizar_df(pd.DataFrame(getattr(self, campo)).set_index('id'),
                     cols, index_cols)

###############################################################################


def estilizar_df (df_original, colunas_para_estilizar = None,index_cols=None):
    """
    Processa um DataFrame original, aplicando estilização em uma lista definida de colunas.
    Permite definir colunas específicas como índice para DataFrames internos.
    Se nenhuma coluna for especificada para estilização, o DataFrame será exibido diretamente.

    Args:
    - df_original (pd.DataFrame): O DataFrame original.
    - colunas_para_estilizar (list or None): Lista de colunas que devem ser estilizadas ou None.
    - index_cols (list or None): Lista com os nomes das colunas que devem ser usadas como índice
                                 para os DataFrames internos. Deve ter o mesmo comprimento que 
                                 colunas_para_estilizar. Use `None` para não definir índice em
                                 uma coluna específica.

    Returns:
    - styled_df: DataFrame estilizado (ou exibido diretamente, sem estilização, se colunas_para_estilizar for None).
    """
    if colunas_para_estilizar is None:
        # Exibir o DataFrame diretamente sem estilização
        IPython.display.display(df_original)
        return #df_original

    # Garantir que index_cols tenha o mesmo comprimento que colunas_para_estilizar
    if index_cols is None:
        index_cols = [None] * len(colunas_para_estilizar)
    elif len(index_cols) != len(colunas_para_estilizar):
        raise ValueError("index_cols deve ter o mesmo comprimento que colunas_para_estilizar")

    df_copia = df_original.copy()

    # Função para converter dicionário em DataFrame, lidando com dicionários vazios
    def dict_to_df(d, index_col=None):
        if isinstance(d, dict):
            if not d:  # Se o dicionário estiver vazio
                return pd.DataFrame()
            elif all(isinstance(value, list) for value in d.values()):  # Caso seja um dicionário de listas
                df = pd.DataFrame(d)  # Cria um DataFrame diretamente
            else:
                df = pd.DataFrame(d, index=[0])  # Cria um DataFrame com índice caso não seja dicionário de listas
            
            # Define a coluna especificada como índice, se aplicável
            if index_col is not None and index_col in df.columns:
                df.set_index(index_col, inplace=True)
            return df
        return d

    # Função para converter lista de dicionários em lista de DataFrames
    def list_dict_to_df_list(list_dict, index_col=None):
        if isinstance(list_dict, list):
            return [dict_to_df(d, index_col=index_col) for d in list_dict]
        return list_dict

    # Aplicar a função de conversão em todas as colunas especificadas
    for coluna, index_col in zip(colunas_para_estilizar, index_cols):
        if coluna in df_copia.columns:
            df_copia[coluna] = df_copia[coluna].apply(
                lambda x: dict_to_df(x, index_col=index_col) if isinstance(x, dict) else list_dict_to_df_list(x, index_col=index_col)
            )

    # Função para renderizar a lista de DataFrames aninhados como HTML
    def render_nested_df_list(data_list):
        if isinstance(data_list, pd.DataFrame):  # Caso seja um DataFrame resultante de um dict
            if data_list.empty:
                return "Empty DataFrame"
            else:
                return data_list.to_html()
        elif isinstance(data_list, list):
            html_list = []
            for item in data_list:
                if isinstance(item, pd.DataFrame):
                    if item.empty:
                        html_list.append("Empty DataFrame")
                    else:
                        html_list.append(item.to_html())
                else:
                    html_list.append(str(item))
            return '<br>'.join(html_list)
        return str(data_list)

    # Aplicar a função de renderização em todas as colunas especificadas
    for coluna in colunas_para_estilizar:
        if coluna in df_copia.columns:
            df_copia[coluna] = df_copia[coluna].apply(render_nested_df_list)

    # Exibir o resultado usando style
    styled_df = df_copia.style.format({coluna: lambda x: x for coluna in colunas_para_estilizar})

    # Exibir o DataFrame estilizado
    IPython.display.display(styled_df)

def is_dict_only_none_or_empty_dict(d):
    return all(value is None or value == {} for value in d.values())

def sanitize_filename(filename):
    """Remove ou substitui caracteres problemáticos em nomes de arquivo"""
    # Substitui espaços por underscores
    sanitized = filename.replace(' ', '_')
    # Remove caracteres especiais
    sanitized = ''.join(c for c in sanitized if c.isalnum() or c in '_-.')
    return sanitized
