import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np

def _plotar_tendencias(dict_dfs, posicoes = None):
    
    # preparativos iniciais
    custom_params = {"axes.spines.right": False, "axes.spines.top": False}
    sns.set_theme(style="ticks", palette='pastel', rc=custom_params)

    # Filtrar as chaves do dicionário com base em 'posicoes'
    if posicoes:
        if isinstance(posicoes, list):
            dict_dfs = {key: dict_dfs[key] for key in posicoes if key in dict_dfs}
            rotulos = None  # Sem rótulos personalizados
        elif isinstance(posicoes, dict):
            dict_dfs = {key: dict_dfs[key] for key in posicoes.keys() if key in dict_dfs}
            rotulos = posicoes  # Usar o dicionário 'posicoes' como rótulos
        else:
            raise ValueError("'posicoes' deve ser uma lista ou dicionário.")
    else:
        rotulos = None

    # Coletar todas as colunas únicas 
    all_columns = set() 
    for df in dict_dfs.values(): 
        all_columns.update(df.columns) 
     
    # Configurar o layout dos subplots
    num_variables = len(all_columns)
    num_cols = 3
    num_rows = (num_variables + num_cols - 1) // num_cols

    # Criar um dicionário para mapear colunas a subplots 
    column_to_subplot = {col: i for i, col in enumerate(all_columns)} 
     
    # Criar a figura e os subplots 
    fig, axes = plt.subplots(nrows=num_rows, ncols=num_cols, 
                             figsize=(4 * num_cols, 3 * num_rows), 
                             sharex=True) 
    if len(all_columns) == 1: 
        axes = [axes] 
     
    # Plotar os dados 
    for j, (key, df) in enumerate(dict_dfs.items()): 
        label_key = rotulos[key] if rotulos and key in rotulos else str(key)
        for i, col in enumerate(df.columns): 
            ax = axes.ravel()[column_to_subplot[col]] 
            sns.lineplot(x=df.index/(3600),
                         y=df[col],
                         label=label_key, 
                         linewidth=2,
                         ax=ax
                         )
            ax.set_ylabel(col,fontsize=9, labelpad=10)
            ax.set_xlabel('Tempo (h)',fontsize=9)
            ax.legend() 
     
    # Remover os subplots vazios, se houver
    for i in range(num_variables, len(axes.ravel())):
        fig.delaxes(axes.ravel()[i])

    # POR ALGUM MOTIVO ISSO NAO FUNCIONA
    for i in range(num_variables,  len(axes.ravel())):  # Subplots apagados estão a partir de len(y)
        row, col = divmod(i, num_cols)  # Converte índice linear para (linha, coluna)
        if row > 0:  # Verifica se há uma linha acima
            axes[row - 1, col].set_xlabel('Tempo (h)') 

    # Ajustar o layout 
    plt.tight_layout(pad=3.0, w_pad=2.0, h_pad=2.0) 
    
    return fig, axes

def _plotar_tendencias_cenarios(lista_dict_dfs, rotulos, posicoes=None):

    # Preparativos iniciais
    custom_params = {"axes.spines.right": False, "axes.spines.top": False}
    sns.set_theme(style="ticks", palette='pastel', rc=custom_params)

    rotulos_title = rotulos

    # Filtrar as chaves de cada dicionário na lista com base em 'posicoes'
    if posicoes:
        if isinstance(posicoes, list):
            lista_dict_dfs = [
                {key: dict_dfs[key] for key in posicoes if key in dict_dfs}
                for dict_dfs in lista_dict_dfs
            ]
            rotulos = None  # Sem rótulos personalizados
        elif isinstance(posicoes, dict):
            lista_dict_dfs = [
                {key: dict_dfs[key] for key in posicoes.keys() if key in dict_dfs}
                for dict_dfs in lista_dict_dfs
            ]
            rotulos = posicoes  # Usar o dicionário 'posicoes' como rótulos
        else:
            raise ValueError("'posicoes' deve ser uma lista ou dicionário.")
    else:
        rotulos = None

    # Coletar todas as colunas únicas de todos os dicionários
    all_columns = set()
    for dict_dfs in lista_dict_dfs:
        for df in dict_dfs.values():
            all_columns.update(df.columns)

    # Configurar o layout dos subplots
    num_variables = len(all_columns)
    num_scenarios = len(lista_dict_dfs)
    fig, axes = plt.subplots(
        nrows=num_variables, ncols=num_scenarios,
        figsize=(4 * num_scenarios, 3 * num_variables), sharex=True, sharey='row'
    )
    if num_variables == 1:
        axes = [axes]  # Garantir que 'axes' seja iterável
    elif num_scenarios == 1:
        axes = np.expand_dims(axes, axis=1)  # Transformar em matriz 2D para consistência

    # Criar um dicionário para mapear colunas a linhas (subplots)
    column_to_row = {col: i for i, col in enumerate(all_columns)}

    # Plotar os dados
    for j, dict_dfs in enumerate(lista_dict_dfs):  # Iterar sobre cenários
        for key, df in dict_dfs.items():
            label_key = rotulos[key] if rotulos and key in rotulos else str(key)
            for col in df.columns:  # Iterar sobre as colunas
                row_idx = column_to_row[col]
                ax = axes[row_idx, j]  # Selecionar o subplot correspondente
                sns.lineplot(
                    x=df.index / 3600,
                    y=df[col],
                    label=label_key,
                    linewidth=2,
                    ax=ax
                )
                ax.set_title(rotulos_title[j], fontsize=10) # Título para a coluna
                ax.set_ylabel(col, fontsize=9, labelpad=10)
                ax.set_xlabel('Tempo (h)', fontsize=9)
                ax.legend()

    # Remover subplots vazios, se houver
    for i in range(len(all_columns), axes.shape[0]):
        for j in range(len(lista_dict_dfs)):
            fig.delaxes(axes[i, j])

    # Ajustar o layout
    plt.tight_layout()
    
    return fig, axes