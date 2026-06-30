"""
_branch.py — Bilingual (EN/PT) Branch class for Marlim3.

``Branch`` stores all configuration under English attribute names internally,
but supports transparent Portuguese access via ``__getattr__``/``__setattr__``
(e.g., ``branch.dutosProducao`` maps to ``branch.productionPipe``).

The ``to_json()`` method accepts a ``language`` parameter ('en' or 'pt') to
output in either language.  When loading a Portuguese-keyed JSON (one without
``"language": "en"``), :meth:`from_json` automatically translates all keys to
English before populating the attributes.
"""

from fileinput import filename
import json
import os
import re
import shutil
import platform
import subprocess
import time
from contextlib import nullcontext
from threading import Thread

import pandas as pd

from .._download import get_executable_path
from .._conversores._conversor_marlim3_tplppl import convert_to_ppl_tpl
from .._output_headers import CANONICAL_TIME_COLUMN, normalize_time_column, parse_trend_headers
from .._plots._plots_perfis import _plotar_perfis, _plotar_perfis_animados
from .._plots._plots_tends import _plotar_tendencias
from .._plots._plots_geometria import _plotar_geometria
from ._keys import (
    translate as _translate_pt_to_en,
    translate_en_to_pt as _translate_en_to_pt,
    PT_TO_EN as _PT_TO_EN,
    EN_TO_PT as _EN_TO_PT,
    _make_bilingual,
)



def _is_empty(d):
    return all(v is None or v == {} for v in d.values())


def _sanitize_filename(filename):
    sanitized = filename.replace(' ', '_')
    sanitized = ''.join(c for c in sanitized if c.isalnum() or c in '_-.')
    return sanitized


class Branch:
    """Marlim3 production system (branch/tramo) with an all-English API."""

    @property
    def results(self):
        return self.resultados

    @results.setter
    def results(self, value):
        self.resultados = value

    def __getattr__(self, name):
        """Allow Portuguese attribute access (e.g., branch.dutosProducao)."""
        en_name = _PT_TO_EN.get(name)
        if en_name is not None and en_name != name:
            try:
                return object.__getattribute__(self, en_name)
            except AttributeError:
                pass
        raise AttributeError(
            f"'{type(self).__name__}' object has no attribute '{name}'"
        )

    def __setattr__(self, name, value):
        """Allow Portuguese attribute assignment (e.g., branch.sistema = 'PROD').

        When setting via a Portuguese name, nested dict keys and enum values
        are also translated PT→EN so the user can build models fully in PT.
        All dict/list values are wrapped in bilingual containers for
        transparent PT/EN nested access.
        """
        en_name = _PT_TO_EN.get(name)
        if en_name is not None and en_name != name:
            # Translate nested PT keys/values by wrapping in a dict
            translated = _translate_pt_to_en({name: value})
            object.__setattr__(self, en_name, _make_bilingual(translated[en_name]))
        else:
            # English or internal name: wrap complex values for bilingual access
            if isinstance(value, (dict, list)):
                value = _make_bilingual(value)
            object.__setattr__(self, name, value)

    def __init__(self,
                 system='PROD',
                 initialConfig=None,
                 compTable=None,
                 time=None,
                 correction=None,
                 productionFluid=None,
                 complementaryFluid=None,
                 gasFluid=None,
                 material=None,
                 crossSection=None,
                 productionPipe=None,
                 servicePipe=None,
                 separator=None,
                 gasInj=None,
                 injectionWellBC=None,
                 ipr=None,
                 massSource=None,
                 gasSource=None,
                 liquidSource=None,
                 pressureSource=None,
                 porousRadialSource=None,
                 porous2DSource=None,
                 valve=None,
                 gasLiftSource=None,
                 surfaceChoke=None,
                 injectionChoke=None,
                 masterValve=None,
                 masterValve2=None,
                 esp=None,
                 volumetricPump=None,
                 pressureDrop=None,
                 pig=None,
                 wax=None,
                 productionProfile=None,
                 serviceProfile=None,
                 productionTrend=None,
                 serviceTrend=None,
                 crossProductionProfile=None,
                 crossServiceProfile=None,
                 crossProductionTrend=None,
                 crossServiceTrend=None,
                 screenConfig=None,
                 name=None):

        self.language = "en"
        self.system      = system

        self.initialConfig  = initialConfig  if initialConfig  is not None else {}
        self.compTable      = compTable      if compTable      is not None else {}
        self.time           = time
        self.correction     = correction

        self.productionFluid    = productionFluid   if productionFluid   is not None else []
        self.complementaryFluid = complementaryFluid
        self.gasFluid           = gasFluid

        self.material        = material        if material        is not None else []
        self.crossSection    = crossSection    if crossSection    is not None else []
        self.productionPipe  = productionPipe  if productionPipe  is not None else []
        self.servicePipe     = servicePipe     if servicePipe     is not None else []

        self.separator       = separator
        self.gasInj          = gasInj
        self.injectionWellBC = injectionWellBC
        self.ipr             = ipr             if ipr             is not None else []
        self.massSource      = massSource      if massSource      is not None else []
        self.gasSource       = gasSource       if gasSource       is not None else []
        self.liquidSource    = liquidSource    if liquidSource    is not None else []
        self.pressureSource  = pressureSource  if pressureSource  is not None else []
        self.porousRadialSource = porousRadialSource if porousRadialSource is not None else []
        self.porous2DSource  = porous2DSource  if porous2DSource  is not None else []

        self.valve          = valve          if valve          is not None else []
        self.gasLiftSource  = gasLiftSource  if gasLiftSource  is not None else []
        self.surfaceChoke   = surfaceChoke
        self.injectionChoke = injectionChoke
        self.masterValve    = masterValve
        self.masterValve2   = masterValve2

        self.esp            = esp            if esp            is not None else []
        self.volumetricPump = volumetricPump if volumetricPump is not None else []
        self.pressureDrop   = pressureDrop   if pressureDrop   is not None else []
        self.pig            = pig            if pig            is not None else []
        self.wax            = wax            if wax            is not None else {}

        self.productionProfile       = productionProfile       if productionProfile       is not None else {}
        self.serviceProfile          = serviceProfile          if serviceProfile          is not None else {}
        self.productionTrend         = productionTrend         if productionTrend         is not None else []
        self.serviceTrend            = serviceTrend            if serviceTrend            is not None else []
        self.crossProductionProfile  = crossProductionProfile  if crossProductionProfile  is not None else {}
        self.crossServiceProfile     = crossServiceProfile     if crossServiceProfile     is not None else {}
        self.crossProductionTrend    = crossProductionTrend    if crossProductionTrend    is not None else []
        self.crossServiceTrend       = crossServiceTrend       if crossServiceTrend       is not None else []
        self.screenConfig               = screenConfig               if screenConfig               is not None else []

        self.json_entrada_keys = set(self.__dict__.keys())

        self.resultados = {}
        self.nome_tramo = name

    def to_json(self, filename='marlim3_model', language='en',
               generate_empty_fields=False):
        
        file_path = './' + filename
        if not filename.endswith('.mr3'):
            file_path = './' + filename + '.mr3'
        else:
            file_path = './' + filename

        def filter_empty_objs(item):
            if isinstance(item, list):
                return [filter_empty_objs(i) for i in item] or (None if not generate_empty_fields else [])
            elif isinstance(item, dict):
                filtered = {k: filter_empty_objs(v) for k, v in item.items()}
                return filtered or (None if not generate_empty_fields else {})
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

        if language == 'pt':
            filtered_data.pop('language', None)
            filtered_data = _translate_en_to_pt(filtered_data)

        with open(file_path, 'w', encoding='utf-8') as fh:
            json.dump(filtered_data, fh, indent=2, ensure_ascii=True)

    def from_json(self, json_input, is_string=False):
        """Load configuration from a JSON file path or dict.

        Automatically translates Portuguese-keyed JSON to English when no
        ``"language": "en"`` marker is present.
        """
        if is_string:
            data = json_input
        else:
            with open(json_input, 'r') as fh:
                data = json.load(fh)
            if not hasattr(self, 'label'):
                if json_input.endswith('.json'):
                    label = json_input[:-5]     # remove '.json'
                elif json_input.endswith('.mr3'):
                    label = json_input[:-4]     # remove '.mr3'
                else:
                    label = json_input
                self.label = label

        lang = data.get('language', '').lower() if isinstance(data, dict) else ''
        if lang != 'en':
            data = _translate_pt_to_en(data, _root=True)
            data['language'] = 'en'

        d = data

        self.system          = d.get('system',              'PROD')
        self.initialConfig   = d.get('initialConfig')       or {}
        self.compTable      = d.get('compTable')            or {}
        self.time            = d.get('time')
        self.correction      = d.get('correction')

        self.productionFluid    = d.get('productionFluid',   [])
        self.complementaryFluid = d.get('complementaryFluid')
        self.gasFluid           = d.get('gasFluid')

        self.material        = d.get('material',       [])
        self.crossSection    = d.get('crossSection',   [])
        self.productionPipe  = d.get('productionPipe', [])
        self.servicePipe     = d.get('servicePipe',   [])

        self.separator       = d.get('separator')
        self.gasInj          = d.get('gasInj')
        self.injectionWellBC = d.get('injectionWellBC')

        self.ipr             = d.get('ipr',             [])
        self.massSource      = d.get('massSource',      [])
        self.gasSource       = d.get('gasSource',       [])
        self.liquidSource    = d.get('liquidSource',    [])
        self.pressureSource  = d.get('pressureSource',  [])
        self.porousRadialSource = d.get('porousRadialSource', [])
        self.porous2DSource  = d.get('porous2DSource',  [])

        self.valve           = d.get('valve',           [])
        self.gasLiftSource   = d.get('gasLiftSource',   [])
        self.surfaceChoke    = d.get('surfaceChoke')
        self.injectionChoke  = d.get('injectionChoke')
        self.masterValve     = d.get('masterValve')
        self.masterValve2    = d.get('masterValve2')

        self.esp             = d.get('esp',             [])
        self.volumetricPump  = d.get('volumetricPump',  [])
        self.pressureDrop    = d.get('pressureDrop',    [])
        self.pig             = d.get('pig',             [])
        self.wax             = d.get('wax')             or {}

        self.productionProfile         = d.get('productionProfile')
        self.serviceProfile            = d.get('serviceProfile')
        self.productionTrend           = d.get('productionTrend')
        self.serviceTrend              = d.get('serviceTrend')
        self.crossProductionProfile    = d.get('crossProductionProfile')
        self.crossServiceProfile       = d.get('crossServiceProfile')
        self.crossProductionTrend      = d.get('crossProductionTrend')
        self.crossServiceTrend         = d.get('crossServiceTrend')
        self.screenConfig               = d.get('screenConfig')

    def from_mr2(self, mr2_path, pvt_path=None, mr2_binary_path=None,
                 simula_mr2=False):
        from .._conversores._conversor_mr2 import converter_mr2_para_json

        data = converter_mr2_para_json(
            mr2_path,
            pvt_path=pvt_path,
            mr2_binary_path=mr2_binary_path,
            simula_mr2=simula_mr2,
        )
        self.from_json(data, is_string=True)

        if not hasattr(self, 'label') or not self.label:
            nome = os.path.basename(mr2_path)
            if nome.endswith('.mr2'):
                nome = nome[:-4]
            self.label = nome

    def simulate(self, kind='PRODUTOR',
                 label='marlim3_model',
                 directory='marlim3_resultados',
                 generate_ppl_tpl=False,
                 simulation_id=None,
                 websocket_handler=None,
                 tracker=None,
                 sanitized=False):
        if label != 'marlim3_model':
            self.label = label

        with nullcontext(get_executable_path()) as executavel:

            filename = label + '.mr3'

            if not os.path.isdir(directory):
                os.mkdir(directory)
            else:
                for item in os.listdir(directory):
                    item_path = os.path.join(directory, item)
                    if os.path.isfile(item_path):
                        os.remove(item_path)
                    elif os.path.isdir(item_path):
                        shutil.rmtree(item_path)

            if sanitized:
                if self.initialConfig:
                    if self.initialConfig.get('pvtFile'):
                        self.initialConfig['pvtFile'] = _sanitize_filename(
                            str(self.initialConfig['pvtFile']))
                if self.injectionWellBC:
                    if self.injectionWellBC.get('pvtsimFile'):
                        self.injectionWellBC['pvtsimFile'] = _sanitize_filename(
                            str(self.injectionWellBC['pvtsimFile']))
                if self.wax:
                    if self.wax.get('waxFile'):
                        self.wax['waxFile'] = _sanitize_filename(
                            str(self.wax['waxFile']))
                if self.time:
                    if self.time.get('snapshotFile'):
                        self.time['snapshotFile'] = _sanitize_filename(
                            str(self.time['snapshotFile']))

            self.to_json(label)

            if self.system == 'INJETOR':
                cmd = f'"{executavel}" -d "{directory}" -i "{filename}" -s INJETOR'
            else:
                cmd = f'"{executavel}" -d "{directory}" -i "{filename}"'

            if platform.system() == "Windows":
                process = subprocess.Popen(
                    cmd, shell=True,
                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
                stdout, _ = process.communicate()
                print(stdout)
            else:
                process = subprocess.Popen(cmd, shell=True,
                                           preexec_fn=os.setsid)

            if tracker:
                tracker.process_pid = process.pid
                tracker.save()

            log_thread = Thread(
                target=self._monitor_log,
                args=(directory, simulation_id, websocket_handler))
            log_thread.start()

            process.wait()
            time.sleep(1)

            self.stop_monitoring = True
            log_thread.join()

            try:
                if self.productionProfile is not None:
                    self.resultados['productionProfile'] = self._process_profiles(directory)
                if 'gasLine' in self.initialConfig:
                    if self.serviceProfile is not None and self.initialConfig['gasLine']:
                        self.resultados['serviceProfile'] = self._process_profiles(
                            directory, line='service')
                if self.productionTrend is not None:
                    self.resultados['productionTrend'] = self._process_trends(directory)
                if 'gasLine' in self.initialConfig:
                    if self.serviceTrend is not None and self.initialConfig['gasLine']:
                        self.resultados['serviceTrend'] = self._process_trends(
                            directory, line='service')

                if _is_empty(self.resultados):
                    raise Exception

            except Exception:
                for origin_file in [os.path.join(os.getcwd(), 'simulacao.log')]:
                    try:
                        dest = os.path.join(directory, os.path.basename(origin_file))
                        if os.path.exists(dest):
                            os.remove(dest)
                        if os.path.exists(origin_file):
                            shutil.move(origin_file, directory)

                        log_path = os.path.join(directory, 'simulacao.log')
                        with open(log_path, 'r') as fh:
                            log_data = json.load(fh)

                        error_message = None
                        if ('resultadoSimulacao' in log_data
                                and 'logs' in log_data['resultadoSimulacao']):
                            for entry in reversed(log_data['resultadoSimulacao']['logs']):
                                if entry['log'] == 'FALHA':
                                    error_message = f"{entry['descricao']}"
                                    if entry['propriedade']:
                                        error_message += f" (Propriedade: {entry['propriedade']});"
                                    if entry['causa']:
                                        error_message += f" (Causa: {entry['causa']})"
                                    break
                        if error_message:
                            raise Exception(error_message)
                    except FileNotFoundError:
                        pass

            for origin_file in [os.path.join(os.getcwd(), 'simulacao.log')]:
                try:
                    if os.path.exists(origin_file):
                        dest = os.path.join(directory, os.path.basename(origin_file))
                        if os.path.exists(dest):
                            os.remove(dest)
                        shutil.move(origin_file, directory)
                except FileNotFoundError:
                    pass

            if generate_ppl_tpl:
                dir_tpl_ppl = directory + '_ppl_tpl'
                if not os.path.isdir(dir_tpl_ppl):
                    os.mkdir(dir_tpl_ppl)
                convert_to_ppl_tpl(directory, dir_tpl_ppl)

    def _monitor_log(self, directory, simulation_id, websocket_handler=None):
        self.stop_monitoring = False
        log_file = os.path.join(directory, 'LogEvento.dat')
        last_position = 0
        last_modified_time = 0
        wait_after_stop = 2

        try:
            if websocket_handler:
                websocket_handler.send_message("simulation.start", "Iniciando simulação...")

            while not self.stop_monitoring or time.time() - last_modified_time < wait_after_stop:
                try:
                    if os.path.exists(log_file):
                        current_modified_time = os.path.getmtime(log_file)
                        if current_modified_time > last_modified_time:
                            last_modified_time = current_modified_time
                            with open(log_file, 'r') as fh:
                                fh.seek(last_position)
                                new_content = fh.read()
                                if new_content:
                                    last_position = fh.tell()
                                    if websocket_handler:
                                        websocket_handler.send_message(
                                            "simulation.progress", new_content.strip())
                    time.sleep(0.5)
                except Exception:
                    break
        except Exception:
            self.stop_monitoring = True

    def _process_profiles(self, directory='marlim3_resultados', line='production'):
        temp_dfs = []

        if line == 'production':
            prefix = 'PERFISP'
        elif line == 'service':
            prefix = 'PERFISG'
        else:
            print("argument 'line' must be 'production' or 'service'")
            return None

        for root, dirs, files in os.walk(directory):
            for filename in files:
                if filename.startswith(prefix) and filename.endswith('.dat'):
                    file_path = os.path.join(root, filename)
                    df = pd.read_csv(file_path, sep=';', skiprows=0, header=1)
                    df.columns = [col.strip() for col in df.columns]
                    df = normalize_time_column(df)
                    df[CANONICAL_TIME_COLUMN] = df[CANONICAL_TIME_COLUMN].astype(int)
                    temp_dfs.append(df)

        if not temp_dfs:
            print(f"No {prefix}*.dat files found in {directory}")
            return None

        concatenated = pd.concat(temp_dfs)
        concatenated.set_index([CANONICAL_TIME_COLUMN, concatenated.index], inplace=True)
        concatenated.index.set_levels(
            [concatenated.index.levels[0], concatenated.index.levels[1]],
            level=[0, 1])
        concatenated = concatenated.loc[
            :, ~concatenated.columns.str.contains('^Unnamed')]
        return concatenated

    def _process_trends(self, directory='marlim3_resultados', line='production'):
        if line not in ('production', 'service'):
            raise ValueError(
                f"Argument 'line' must be 'production' or 'service', not '{line}'")

        prefix = 'TENDP' if line == 'production' else 'TENDG'
        results = {}

        for root, dirs, files in os.walk(directory):
            for filename in files:
                if filename.startswith(prefix) and filename.endswith('.dat'):
                    file_path = os.path.join(root, filename)
                    try:
                        file_number = int(
                            re.search(r'\d+', os.path.splitext(filename)[0]).group())

                        with open(file_path, 'r') as fh:
                            line1 = fh.readline().strip()
                            line2 = fh.readline().strip()
                            line3 = fh.readline().strip()

                        measured_length, label, cell_index = parse_trend_headers(
                            line1, line2, line3)

                        df = pd.read_csv(file_path, sep=';', skiprows=3, header=0)
                        df.columns = [col.strip() for col in df.columns]
                        df = normalize_time_column(df)
                        df[CANONICAL_TIME_COLUMN] = df[CANONICAL_TIME_COLUMN].astype(float)
                        df.set_index([CANONICAL_TIME_COLUMN], inplace=True)
                        df = df.loc[:, ~df.columns.str.contains('^Unnamed')]
                        for col in df.columns:
                            df[col] = df[col].astype(float)

                        df.attrs['measured_length'] = measured_length
                        df.attrs['label'] = label
                        df.attrs['cell_index'] = cell_index

                        results[file_number] = df

                    except Exception as exc:
                        print(f"Error processing {file_path}: {exc}")

        return dict(sorted(results.items()))

    def plot_profiles(self, line='production', labels=None,
                      gradient=False, indicate_anm=False):
        if line == 'production':
            result_key = 'productionProfile'
        elif line == 'service':
            result_key = 'serviceProfile'
        else:
            print("argument 'line' must be 'production' or 'service'")
            return None, None

        posicao_anm = None
        if indicate_anm:
            try:
                mv = self.masterValve
                if isinstance(mv, dict):
                    posicao_anm = mv.get('measuredLength')
                elif isinstance(mv, list) and mv:
                    posicao_anm = mv[0].get('measuredLength')
            except Exception:
                print('master valve position not defined in the model!')

        labels = labels if labels is not None else []
        fig, ax = _plotar_perfis(self.resultados[result_key], linha=line,
                                 rotulos=labels, gradiente=gradient,
                                 posicao_anm=posicao_anm)
        return fig, ax

    def plot_animated_profiles(self, line='production'):
        return _plotar_perfis_animados(self.resultados['productionProfile'],
                                       linha=line)

    def plot_trends(self, line='production', positions=None):
        if line == 'production':
            result_key = 'productionTrend'
        elif line == 'service':
            result_key = 'serviceTrend'
        else:
            print("argument 'line' must be 'production' or 'service'")
            return None, None

        fig, ax = _plotar_tendencias(self.resultados[result_key],
                                     posicoes=positions,
                                     language='en')
        return fig, ax

    def plot_geometry(self):
        _plotar_geometria(self)

    def display_table(self, field):
        field = _PT_TO_EN.get(field, field)
        if field == 'crossSection':
            cols = ['layers']
            index_cols = None
        elif field in ('productionPipe', 'servicePipe'):
            cols = ['discretization', 'initialAndAmbientConditions']
            index_cols = [None, 'measuredPosition']
        else:
            cols = None
            index_cols = None

        _estilizar_df(pd.DataFrame(getattr(self, field)).set_index('id'),
                      cols, index_cols)

    def load_mr2(self, path, pvt_path=None, mr2_binary_path=None,
                 re_simulate=False):
        return self.from_mr2(mr2_path=path, pvt_path=pvt_path,
                              mr2_binary_path=mr2_binary_path,
                              simula_mr2=re_simulate)


def _estilizar_df(df_original, colunas_para_estilizar=None, index_cols=None):
    import IPython.display

    if colunas_para_estilizar is None:
        IPython.display.display(df_original)
        return

    if index_cols is None:
        index_cols = [None] * len(colunas_para_estilizar)
    elif len(index_cols) != len(colunas_para_estilizar):
        raise ValueError("index_cols must have the same length as colunas_para_estilizar")

    df_copia = df_original.copy()

    def dict_to_df(d, index_col=None):
        if isinstance(d, dict):
            if not d:
                return pd.DataFrame()
            elif all(isinstance(v, list) for v in d.values()):
                df = pd.DataFrame(d)
            else:
                df = pd.DataFrame(d, index=[0])
            if index_col is not None and index_col in df.columns:
                df.set_index(index_col, inplace=True)
            return df
        return d

    def list_dict_to_df_list(lst, index_col=None):
        if isinstance(lst, list):
            return [dict_to_df(d, index_col=index_col) for d in lst]
        return lst

    for coluna, index_col in zip(colunas_para_estilizar, index_cols):
        if coluna in df_copia.columns:
            df_copia[coluna] = df_copia[coluna].apply(
                lambda x: dict_to_df(x, index_col=index_col)
                if isinstance(x, dict)
                else list_dict_to_df_list(x, index_col=index_col))

    def render_nested(data_list):
        if isinstance(data_list, pd.DataFrame):
            return "Empty DataFrame" if data_list.empty else data_list.to_html()
        elif isinstance(data_list, list):
            parts = []
            for item in data_list:
                if isinstance(item, pd.DataFrame):
                    parts.append("Empty DataFrame" if item.empty else item.to_html())
                else:
                    parts.append(str(item))
            return '<br>'.join(parts)
        return str(data_list)

    for coluna in colunas_para_estilizar:
        if coluna in df_copia.columns:
            df_copia[coluna] = df_copia[coluna].apply(render_nested)

    styled = df_copia.style.format(
        {col: lambda x: x for col in colunas_para_estilizar})
    IPython.display.display(styled)
