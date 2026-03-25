import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import matplotlib.colors as mcolors
import seaborn as sns
import warnings
import pandas as pd
import numpy as np
import matplotlib.animation as animation 

def _plotar_perfis(df, linha = 'producao', posicao_anm=None, gradiente=False, 
                   dt=1, rotulos=None):

    # Transformar a entrada em lista de DataFrames -------------------------
    if isinstance(df, pd.DataFrame):
        dfs = [df]
    elif isinstance(df, (list, tuple)) and all(isinstance(d, pd.DataFrame) for d in df):
        dfs = list(df)          # mantém a ordem recebida
    else:
        raise ValueError("A entrada deve ser um DataFrame ou uma lista de DataFrames")

    # Garantir que cada DataFrame já está ordenado pelo índice -------------
    dfs = [d if d.index.is_monotonic_increasing else d.sort_index()
           for d in dfs]
        
       # comprimento_centro = 'Comprimento (m) Centro Volume C'

    if linha == 'producao':
        cols_desconsiderar = ['Comprimento (m) Fronteira F',
    #                comprimento_centro,
                              'Comprimento (m) Centro Volume C',
                    'Unidade de Producao',
                    'Elevacao (m) F',
                    'Elevacao (m) C']
    else:
        #comprimento_centro = 'Comprimento (m) centro de Volume C'
        cols_desconsiderar = ['Comprimento (m) Fronteira F',
            'Comprimento (m) Centro Volume C',
            'Unidade de Servico',
            'Profundidade (m) F',
            'Profundidade (m) C',
            'comprimento_fundoPoco (m) F']



    if gradiente:
        if len(dfs) > 1:
            gradiente = False
        else:
            # gradiente de cores baseado no azul marlim3
            base_color = "#39C0E0"
            cmap_colors = ["#A7E1F2", base_color, "#012A3A"]
            colormap = LinearSegmentedColormap.from_list("custom_cmap", cmap_colors)
    if not gradiente:
        # palette = list(plt.get_cmap('tab20').colors)
        # palette += list(plt.get_cmap('tab20b').colors) + list(plt.get_cmap('tab20c').colors)
        # order_indices = [18, 19, 16, 17, 14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1]
        # colors = [palette[i] for i in order_indices]
        # colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
        colors = [
      '#39C0E0', # Cyan
      '#E69529', # Orange
      '#4CAF50', # Green
      '#9C27B0', # Purple
      '#795548', # Brown
      '#D64E4E', # Red
      '#3F51B5', # Indigo
      '#607D8B', # Blue Grey
      '#FF5722', # Deep Orange
      '#00BCD4', # Lighter Cyan
      '#FFC107', # Amber
      '#8BC34A', # Light Green
      '#E91E63', # Pink
      '#2196F3', # Blue
      '#FF9800', # Darker Orange
      '#CDDC39', # Lime
      '#009688', # Teal
      '#673AB7', # Deep Purple
      '#FFEB3B', # Yellow
      '#03A9F4'  # Light Blue
            ]

    # preparativos iniciais
    custom_params = {"axes.spines.right": False, "axes.spines.top": False}
    sns.set_theme(style="ticks", palette='pastel', rc=custom_params)

    # Selecionando variáveis dos eixos y com base no primeiro df
    variables = []
    for col in dfs[0].sort_index().columns:  # evita uns warnings chatos
        if col not in cols_desconsiderar:
            variable_name = col.rsplit(' ', 1)[0]
            variables.append(variable_name)

    # Configurar o layout dos subplots
    num_variables = len(variables)
    num_cols = 3
    num_rows = (num_variables + num_cols - 1) // num_cols

    # criando figura
    fig, axes = plt.subplots(nrows=num_rows, ncols=num_cols,
                             figsize=(12, 3 * num_rows), sharex=True)
    axes = axes.flatten()  # Achatar a matriz de eixos pra iteração fácil

    for j, df in enumerate(dfs):
        # ultimos preparativos....
        times = df.index.levels[0][::dt]
        if gradiente:
            colors = colormap(np.linspace(0, 1, len(times)))

        # plotando!
        for i, (ax, variable) in enumerate(zip(axes, variables)):
            for col in df.columns:
                if col.startswith(variable):
                    eixo_x_col = (
                        'Comprimento (m) Fronteira F' if col.endswith('F')
                        else 'Comprimento (m) Centro Volume C'
                    )

                    if gradiente:
                        for time, color in zip(times, colors):
                            sns.lineplot(x=df.loc[time, eixo_x_col],
                                         y=df.loc[time, col],
                                         ax=ax,
                                         color=color,
                                         linewidth=2)
                    else:
                        time = times[0]
                        sns.lineplot(x=df.loc[time, eixo_x_col],
                                     y=df.loc[time, col],
                                     ax=ax,
                                     color=colors[j],
                                     label=rotulos[j] if rotulos else None,
                                     linewidth=2)
                    if posicao_anm:
                        ax.axvline(posicao_anm,
                                   color='k', linestyle='--', linewidth=1)

                    # ISSO AQUI SO FUNCIONA DIREITO NO VIDEO....
                    if i >= (num_rows - 1) * num_cols or (i + num_cols) >= num_variables:
                        ax.set_xlabel('Comprimento medido (m)', fontsize=9)    

            ax.set_ylabel(variable, fontsize=9, labelpad=10)

            if ax.get_legend() is not None:
                ax.get_legend().remove()

    # Remover os subplots vazios, se houver
    for i in range(num_variables, len(axes)):
        fig.delaxes(axes[i])

    # Adicionar a barra de cores no eixo separado
    if gradiente and len(times) > 1:
        sm = plt.cm.ScalarMappable(cmap=colormap,
                                   norm=plt.Normalize(vmin=min(times / 3600),
                                                      vmax=max(times / 3600)))
        sm.set_array([])
        fig.subplots_adjust(right=0.92, hspace=0.4, wspace=0.3)
        cbar_ax = fig.add_axes([0.93, 0.15, 0.02, 0.7])
        fig.colorbar(sm, cax=cbar_ax, label="Tempo (h)")

    # Ajustar a legenda compartilhada
    if not gradiente:
        handles, labels = ax.get_legend_handles_labels()
        if rotulos:
            labels = rotulos
        fig.legend(handles, labels, loc='upper center', ncol=len(dfs),
                   bbox_to_anchor=(0.5, 1.05), fontsize=9)

    # ajustando
    with warnings.catch_warnings():
        warnings.filterwarnings("ignore",
                                category=UserWarning,
                                message=".*tight_layout.*")
        plt.tight_layout(rect=[0, 0, 0.92, 1])

    #plt.show()

    return fig, axes

##########################################################################

def _plotar_perfis_animados(df, linha = 'producao', 
                            posicao_anm = None, dt = 1): 

    #comprimento_centro = 'Comprimento (m) Centro Volume C'
    
    if linha == 'producao':
        cols_desconsiderar = ['Comprimento (m) Fronteira F',
                    'Comprimento (m) Centro Volume C',
                    'Unidade de Producao',
                    'Elevacao (m) F',
                    'Elevacao (m) C']
    else:
        #comprimento_centro = 'Comprimento (m) centro de Volume C'
        cols_desconsiderar = ['Comprimento (m) Fronteira F',
            'Comprimento (m) Centro Volume C',
            'Unidade de Servico',
            'Profundidade (m) F',
            'Profundidade (m) C',
            'comprimento_fundoPoco (m) F']

    base_color="#39C0E0"
    
    # evitando warnings chatos 
    with warnings.catch_warnings(): 
        warnings.filterwarnings("ignore", category=UserWarning, 
                                message=".*transformation singular.*") 
        warnings.filterwarnings("ignore", category=UserWarning, 
                                message=".*tight_layout.*") 

    # preparativos iniciais 
    custom_params = {"axes.spines.right": False, "axes.spines.top": False} 
    sns.set_theme(style="ticks", palette='pastel', rc=custom_params) 

    # Selecionando variáveis dos eixos y 
    #df = self.resultados['perfisP'] 
    df = df.sort_index()  # evita uns warnings chatos 

    variables = []
    for col in df.columns: 
        if col not in cols_desconsiderar: 
            variable_name = col.rsplit(' ', 1)[0] 
            variables.append(variable_name) 

    # Configurar o layout dos subplots 
    num_variables = len(variables) 
    num_cols = 3 
    num_rows = (num_variables + num_cols - 1) // num_cols 

    fig = plt.figure(figsize=(12, 3 * num_rows + 0.5))
    gs = fig.add_gridspec(num_rows + 1, num_cols, 
                            height_ratios=[1] * num_rows + [0.1]) 

    axes = [] 
    for i in range(num_rows): 
        for j in range(num_cols): 
            if i * num_cols + j < num_variables: 
                axes.append(fig.add_subplot(gs[i, j])) 

    # Adicionar eixo para o texto do tempo de simulação 
    time_ax = fig.add_subplot(gs[-1, :]) 

    # ultimos preparativos.... 
    times = df.index.levels[0][::dt] 
    lines = [] 

    # plotando! 
    for i, (ax, variable) in enumerate(zip(axes, variables)): 
        for col in df.columns: 
            if col.startswith(variable): 
                eixo_x_col = ( 
                    'Comprimento (m) Fronteira F' if col.endswith('F') 
                    else 'Comprimento (m) Centro Volume C' 
                ) 
                line, = ax.plot([], [], color=base_color, linewidth=2) 
                lines.append(line) 
                ax.set_xlim(df[eixo_x_col].min(), df[eixo_x_col].max()) 
                ax.set_ylim(df[col].min(), df[col].max()) 
                ax.set_ylabel(variable, fontsize=9, labelpad=10) 
                if i>=(num_rows-1)*num_cols or (i+num_cols)>=num_variables: 
                    ax.set_xlabel('Comprimento medido (m)', fontsize=9) 
                if posicao_anm: 
                    ax.axvline(posicao_anm, 
                                color='k', linestyle='--', linewidth=1) 

    # Configurar o texto do tempo de simulação 
    time_text = time_ax.text(0.5, 0.5, '', ha='center', 
                                va='center', fontsize=12) 
    time_ax.axis('off') 

    # preparando animação 
    def init(): 
        for line in lines: 
            line.set_data([], []) 
        time_text.set_text('') 
        return lines + [time_text] 

    # função para gerar animação 
    def animate(frame): 
        time = times[frame] 
        i = 0 
        for variable in variables: 
            for col in df.columns: 
                if col.startswith(variable): 
                    eixo_x_col = ( 
                        'Comprimento (m) Fronteira F' if col.endswith('F') 
                        else 'Comprimento (m) Centro Volume C' 
                    ) 
                    x = df.loc[time, eixo_x_col] 
                    y = df.loc[time, col] 
                    lines[i].set_data(x, y) 
                    i += 1 
        
        # Atualizar o texto do tempo de simulação 
        time_text.set_text(f'Tempo de simulação: {(time)/3600:.2f} h') 
        
        return lines + [time_text] 

    # ajustando 
    plt.tight_layout(rect=[0, 0, 1, 0.95]) 

    # gerando animação 
    anim = animation.FuncAnimation(fig, animate, init_func=init, 
                                    frames=len(times), 
                                    interval=200, blit=True) 

    # fechando a figura estática que abre automaticamente 
    plt.close(fig) 

    # tchau 
    return anim