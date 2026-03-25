from .._plots._plots_perfis import _plotar_perfis
from .._plots._plots_tends import _plotar_tendencias_cenarios

class Cenarios:

    def __init__ (self, casos = None):

        self.casos = casos if casos is not None else {}

        # adicionar analise de sensibilidade nativa do marlim

    def plotar_perfis(self, linha = 'producao'):

        dfs = []

        if linha == 'producao':
            PERFIS = 'perfilProducao'
        elif linha == 'servico':
            PERFIS = 'perfilServico'
        else:
            print('argumento linha só pode ser producao ou servico')  

        for rotulo, caso in self.casos.items():
            dfs.append(caso.resultados[PERFIS])

        fig, axes = _plotar_perfis(dfs, 
                                   rotulos=list(self.casos.keys()),
                                   linha=linha)

        return fig, axes

    def plotar_tendencias(self):

        tends_list = []
        
        for rotulo, caso in self.casos.items():
            tends_list.append(caso.resultados['tendP'])

        _plotar_tendencias_cenarios(tends_list,
                                    rotulos=list(self.casos.keys()))