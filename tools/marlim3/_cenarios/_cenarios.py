from .._plots._plots_perfis import _plotar_perfis
from .._plots._plots_tends import _plotar_tendencias_cenarios

class Scenarios:

    def __init__ (self, casos = None):

        self.casos = casos if casos is not None else {}

        # adicionar analise de sensibilidade nativa do marlim

    def plotar_perfis(self, linha = 'producao'):

        dfs = []

        if linha in ('producao', 'production'):
            PERFIS = 'productionProfile'
        elif linha in ('servico', 'service'):
            PERFIS = 'serviceProfile'
        else:
            print("argument line must be 'producao'/'servico' or 'production'/'service'")
            return None, None

        for rotulo, caso in self.casos.items():
            dfs.append(caso.resultados[PERFIS])

        fig, axes = _plotar_perfis(dfs, 
                                   rotulos=list(self.casos.keys()),
                                   linha=linha)

        return fig, axes

    def plotar_tendencias(self, language='pt'):

        tends_list = []
        
        for rotulo, caso in self.casos.items():
            tends_list.append(caso.resultados['productionTrend'])

        _plotar_tendencias_cenarios(tends_list,
                                    rotulos=list(self.casos.keys()),
                                    language=language)

    ###########################################################################
    # English API
    ###########################################################################

    def plot_profiles(self, line='production'):
        return self.plotar_perfis(linha=line)

    def plot_trends(self):
        return self.plotar_tendencias(language='en')


# Deprecated alias
Cenarios = Scenarios