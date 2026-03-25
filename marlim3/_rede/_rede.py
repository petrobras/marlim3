import importlib.resources as pkg_resources
import os
import numpy as np
import json 
import shutil
from .._plots._plots_perfis import _plotar_perfis, _plotar_perfis_animados
from .._plots._plots_redes import _plotar_rede
from .._tramo._tramo import Tramo
import subprocess
import time
from threading import Thread
import re
import pandas as pd
from datetime import datetime
import platform

class Rede:

    def __init__(self, 
                 versao='1.0',
                 configuracaoInicial=None,
                 Arquivos = None,
                 Conexao = None,
                 layout = None,
                 nome_rede=None):

        self.versao = versao
        self.configuracaoInicial = configuracaoInicial
        self.Arquivos = Arquivos if Arquivos is not None else []
        self.Conexao = Conexao if Conexao is not None else []
        self.layout = layout if layout is not None else {}

        #self.tramos = {nome: Tramo(nome_tramo=nome) for nome in self.Arquivos} if self.Arquivos else {}

        # Garantir que Conexao.coletores e Conexao.afluentes sejam listas vazias
        # self.Conexao['coletores'] = self.Conexao.get('coletores', [])
        # self.Conexao['afluentes'] = self.Conexao.get('afluentes', [])

        self.json_entrada_keys = set(self.__dict__.keys())

        self.nome_rede = nome_rede
        
        # Remove file extensions before creating tramos
        self.tramos = {
            os.path.splitext(nome)[0]: Tramo(nome_tramo=os.path.splitext(nome)[0]) 
            for nome in self.Arquivos
        } if self.Arquivos else {}


        #self.resultados = {}

        self.redes_internas = []

    ###########################################################################

    def plotar_rede(self):

        _plotar_rede(self)
        
    def to_json(self, filename='marlim3_rede', generate_empty_fields=False): 
        if not filename.endswith('.json'):
            file_path = './' + filename + '.json'
        else:
            file_path = './' + filename

        self.versao = '1.0'

        def filter_empty_objs(item):
            if isinstance(item, list):
                # Retorna uma lista vazia diretamente (não substitui por None)
                return [filter_empty_objs(i) for i in item] if item else []
            elif isinstance(item, dict):
                filtered_dict = {k: filter_empty_objs(v) for k, v in item.items()}
                # Retorna um dicionário vazio diretamente (não substitui por None)
                return filtered_dict if filtered_dict else {}
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

        self.versao = data.get('versao')
        self.configuracaoInicial = data.get('configuracaoInicial',{})
        self.Arquivos = data.get('Arquivos', [])
        self.Conexao = data.get('Conexao', [])
        self.layout = data.get('layout', {})

        self.tramos = {
            os.path.splitext(nome)[0]: Tramo(nome_tramo=os.path.splitext(nome)[0]) 
            for nome in self.Arquivos
        } if self.Arquivos else {}

    ###########################################################################

    def simular(self, label='marlim3_rede',
                diretorio='marlim3_rede_resultados',
                simulation_id=None,
                websocket_handler=None,
                tracker=None,
                sanitized=False):
        
        # Verificar o sistema operacional
        if platform.system() == 'Windows':
            executavel_name = 'Marlim3.exe'
        else:
            executavel_name = 'Marlim3'
        
        if label != 'marlim3_rede':
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
                
            self.to_json(label)
            
            comando_simulacao = f'{executavel} -d {diretorio} -i {filename} -s REDE'

            # Iniciar o processo de simulação
            if platform.system() == "Windows":
                process = subprocess.Popen(
                    comando_simulacao,
                    shell=True,
                    creationflags=subprocess.CREATE_NEW_PROCESS_GROUP
                )
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

            # Mover arquivos de log para o diretório de resultados antes de processá-los
            for origin_file in [os.path.join(os.getcwd(), 'simulacao.log'),
                                os.path.join(os.getcwd(),'EventosExternos.dat')]:
                try:
                    destination_file = os.path.join(diretorio, os.path.basename(origin_file))
                    if os.path.exists(destination_file):
                        os.remove(destination_file)
                    if os.path.exists(origin_file):
                        shutil.move(origin_file, diretorio)
                except FileNotFoundError:
                    pass  # Ignora se o arquivo não existir

            try:
                # Verificar primeiro se há erros no log de simulação
                error_message = self.check_simulation_log(diretorio)
                if error_message:
                    raise Exception(error_message)
                
                # Processar resultados
                self.processar_perfis(diretorio, linha='producao')
                self.processar_perfis(diretorio, linha='servico')
                
                # Verificar se os resultados foram gerados corretamente
                # Verificamos se pelo menos um tramo tem resultados
                # has_results = False
                # for tramo_name, tramo in self.tramos.items():
                #     if hasattr(tramo, 'resultados') and tramo.resultados:
                #         if 'perfilProducao' in tramo.resultados or 'perfilServico' in tramo.resultados:
                #             has_results = True
                #             break
                
                # if not has_results:
                #     # Verificar novamente o log para erros, caso não tenhamos encontrado antes
                #     error_message = self.check_simulation_log(diretorio)
                #     if error_message:
                #         raise Exception(error_message)
                #     else:
                #         raise Exception("Nenhum resultado de simulação foi gerado")
                    
            except Exception as e:
                # Se o tracker existir, atualizar com a mensagem de erro
                if tracker:
                    tracker.status = 'error'
                    tracker.error_message = str(e)
                    tracker.completed_at = datetime.now()
                    tracker.save()
                
                # Enviar mensagem de erro pelo websocket
                if websocket_handler:
                    websocket_handler.send_message(
                        "simulation.error", 
                        f"Erro na simulação: {str(e)}"
                    )
                    
                # Propagar a exceção para ser tratada pelo chamador
                raise e

    def check_simulation_log(self, diretorio):
        """
        Verifica o arquivo de log da simulação para encontrar mensagens de erro.
        Retorna a mensagem de erro se encontrada, ou None caso contrário.
        """
        log_file_path = os.path.join(diretorio, 'simulacao.log')
        if not os.path.exists(log_file_path):
            return None
            
        try:
            with open(log_file_path, 'r') as f:
                log_data = json.load(f)
                
            # Verificar se a simulação falhou
            if 'resultadoSimulacao' in log_data and log_data['resultadoSimulacao'].get('status') == 'falha':
                # Procurar pela mensagem de erro nos logs
                if 'logs' in log_data['resultadoSimulacao']:
                    for log_entry in log_data['resultadoSimulacao']['logs']:
                        if log_entry['log'] == 'FALHA':
                            error_message = f"{log_entry['descricao']}"
                            if log_entry.get('propriedade'):
                                error_message += f" (Propriedade: {log_entry['propriedade']});"
                            if log_entry.get('causa'):
                                error_message += f" (Causa: {log_entry['causa']})"
                            return error_message
                            
                # Se não encontrou uma mensagem específica, mas o status é falha
                return "Falha na simulação sem mensagem específica"
                
            return None
                
        except json.JSONDecodeError:
            return "Erro ao analisar o arquivo de log da simulação"
        except Exception as e:
            return f"Erro ao processar o log de simulação: {str(e)}"

    def processar_perfis(self, diretorio='marlim3_resultados', linha='producao'): 
        """
        Processa perfis para cada tramo da rede considerando a convenção de nomenclatura
        dos arquivos para redes internas (TramoX-R-Y-PERFISP-Z)
        """
        if linha not in ['producao', 'servico']:
            print('argumento linha só pode ser producao ou servico')
            return
            
        if linha == 'producao': 
            PERFIS = 'PERFISP' 
        else: 
            PERFIS = 'PERFISG'
        
        # Primeiro, vamos mapear os índices dos tramos para seus nomes em cada rede interna
        tramo_index_to_name = {}
        
        # Carregar os JSONs das redes internas
        input_dir = os.getcwd() #os.path.dirname(diretorio)  # O diretório de entrada está acima do diretório de resultados

      #  print('TO NA AREA')
        
        # Procurar arquivos de rede interna
        for file in os.listdir(os.getcwd()):
           # print('FILE', file)
            if file.startswith("RedeInterna-") and file.endswith(".json"):
               # print('FILE', file)
                network_id = file.replace("RedeInterna-", "").replace(".json", "")
                file_path = os.path.join(input_dir, file)
                
                try:
                    with open(file_path, 'r') as f:
                        network_data = json.load(f)
                        self.redes_internas.append(Rede().from_json(network_data,is_string=True))
                        #print("CARREGANDO JSON: ", network_data)
                        
                    # Obter a lista de arquivos (nomes dos tramos)
                    arquivos = network_data.get('Arquivos', [])

                   # print("ARQUIVOS: ", arquivos)
                    
                    # Mapear índices para nomes
                    for i, arquivo in enumerate(arquivos):
                        # Remover extensão para obter o nome do tramo
                        tramo_name = os.path.splitext(arquivo)[0]
                        # Chave de mapeamento: "rede_id-indice"
                        mapping_key = f"{network_id}-{i}"
                        tramo_index_to_name[mapping_key] = tramo_name
                       # print(f"Mapeamento: Tramo{i}-R-{network_id} -> {tramo_name}")
                except Exception as e:
                    print(f"Erro ao processar rede interna {file}: {e}")
        
        # Padrão de regex para extrair informações do nome do arquivo
        pattern = r'Tramo(\d+)-R-(\d+)-' + PERFIS
        
        # Processar os arquivos de perfil
        #print('oooo',diretorio)
        for root, dirs, files in os.walk(diretorio):
            #print('DIRETORIO', root,dirs,files)
            for filename in files:
                if PERFIS in filename and filename.endswith(".dat"):
                    # Extrair índice do tramo e ID da rede do nome do arquivo
                    match = re.search(pattern, filename)
                    if match:
                        tramo_idx = match.group(1)
                        network_id = match.group(2)
                        mapping_key = f"{network_id}-{tramo_idx}"

                        # Verificar se temos um mapeamento para este tramo
                        if mapping_key in tramo_index_to_name:
                            tramo_name = tramo_index_to_name[mapping_key]
                            file_path = os.path.join(root, filename)
                            
                            #print(f"Processando arquivo {filename} para tramo {tramo_name}")
                            
                            try:
                                # Verificar se o tramo já existe em self.tramos
                                if tramo_name not in self.tramos:
                                    self.tramos[tramo_name] = Tramo(nome_tramo=tramo_name)
                                    
                                # Garantir que o objeto tramo tenha um dicionário de resultados
                                if not hasattr(self.tramos[tramo_name], 'resultados'):
                                    self.tramos[tramo_name].resultados = {}
                                    
                                # Ler o arquivo de perfil
                                df = pd.read_csv(file_path, sep=';', skiprows=0, header=1)
                                df.columns = [col.strip() for col in df.columns]
                                df['Tempo (s)'] = df['Tempo (s)'].astype(int)
                                
                                if linha == 'producao':
                                    key = 'perfilProducao'
                                else:
                                    key = 'perfilServico'

                                # Inicializa o dicionário de resultados se necessário
                                if key not in self.tramos[tramo_name].resultados:
                                    self.tramos[tramo_name].resultados[key] = []
                                
                                # Adiciona o DataFrame à lista usando list.append (não DataFrame.append)
                                dfs_list = self.tramos[tramo_name].resultados[key]
                                dfs_list.append(df)
                                self.tramos[tramo_name].resultados[key] = dfs_list

                            except Exception as e:
                                print(f"Erro ao processar arquivo {file_path} para tramo {tramo_name}: {e}")
                        else:
                            print(f"Aviso: Não foi encontrado mapeamento para arquivo {filename} (chave {mapping_key})")


        # Agora, concatenar os DataFrames para cada tramo após processar todos os arquivos
        for tramo_name, tramo in self.tramos.items():
            if hasattr(tramo, 'resultados'):
                for key in ['perfilProducao', 'perfilServico']:
                    if key in tramo.resultados and isinstance(tramo.resultados[key], list) and tramo.resultados[key]:
                        try:
                            temp = pd.concat(tramo.resultados[key])
                            temp.set_index(['Tempo (s)', temp.index], inplace=True)
                            temp = temp.loc[:, ~temp.columns.str.contains('^Unnamed')]
                            tramo.resultados[key] = temp
                        except Exception as e:
                            print(f"Erro ao concatenar DataFrames para tramo {tramo_name}, perfil {key}: {e}")

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


    # --------------------------------------------------------------------
    def plotar_perfis(self,
                             linha='producao',
                             tramos=None,
                             rotulos=None,
                             gradiente=False,
                             indicar_anm=False):
        """
        Desenha numa figura única os perfis (produção ou serviço) de vários
        tramos pertencentes a esta rede.

        Parameters
        ----------
        linha : 'producao' | 'servico'
        tramos : list[str] ou None
            Lista com os nomes (keys) de self.tramos que irão aparecer.
            Se None, usa todos os tramos que possuam resultado.
        rotulos : list[str] ou None
            Legenda a ser mostrada – mesmo comprimento de 'tramos'.
            Se None, usa o próprio nome do tramo.
        gradiente : bool
            Habilita gradiente somente quando um único tramo for passado.
        indicar_anm : bool
            Se True, plota a posição da master1 (quando existir) como linha vertical.
        """
        # ------------------------- seleção de tramos ---------------------
        if tramos is None:
            tramos = [nome for nome, t in self.tramos.items()
                      if hasattr(t, 'resultados')
                         and (('perfilProducao' in t.resultados) or
                              ('perfilServico'  in t.resultados))]

        if not tramos:
            raise ValueError("Nenhum tramo com resultados para plotar.")

        if rotulos is None:
            rotulos = tramos
        if len(rotulos) != len(tramos):
            raise ValueError("'rotulos' deve ter o mesmo tamanho de 'tramos'.")

        # ------------------------- coleta dos DataFrames -----------------
        dfs = []
        posicao_anm = None
        for nome in tramos:
            tramo = self.tramos[nome]
            if linha == 'producao':
                key = 'perfilProducao'
            elif linha == 'servico':
                key = 'perfilServico'
            else:
                raise ValueError("linha deve ser 'producao' ou 'servico'.")

            if key not in tramo.resultados:
                raise ValueError(f"Tramo '{nome}' não tem resultados para '{linha}'.")

            df = tramo.resultados[key]
            dfs.append(df)

            if indicar_anm and posicao_anm is None:
                posicao_anm = getattr(tramo, 'master1', {}).get('comprimentoMedido')

        # ------------------------- chama a função gráfica ----------------
        fig, ax = _plotar_perfis(dfs,
                                 linha=linha,
                                 rotulos=rotulos,
                                 gradiente=gradiente,
                                 posicao_anm=posicao_anm)
        return fig, ax
