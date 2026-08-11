from typing import Dict, Any

class ProductionFluidObject:
    def __init__(self, id: int = -100, tipoEmul: int = -100, correlacaoCritica: int = -100,
                 api: float = -100, rgo: float = -100, densidadeGas: float = -100, bsw: float = -100,
                 densidadeAgua: float = -100, temp1: float = -100, visc1: float = -100, temp2: float = -100,
                 visc2: float = -100, bswCorte: float = -100, fracCO2: float = -100, ativo: bool = True):
        self.id = id
        self.tipoEmul = tipoEmul
        self.correlacaoCritica = correlacaoCritica
        self.api = api
        self.rgo = rgo
        self.densidadeGas = densidadeGas
        self.bsw = bsw
        self.densidadeAgua = densidadeAgua
        self.temp1 = temp1
        self.visc1 = visc1
        self.temp2 = temp2
        self.visc2 = visc2
        self.bswCorte = bswCorte
        self.fracCO2 = fracCO2
        self.ativo = ativo

        # Método para escrever este fluido produzido no json:
    def WriteFluidObjectToJsonFile(self, oFluidCollectionNode: Dict[str, Any]):
        oFluidNode = {
            "id": self.id,
            #"api": self.api,                                 # conforme conversa com Gaspari, excluindo inicialmente 
            "rgo": self.rgo,
            #"densidadeGas": self.densidadeGas,               # conforme conversa com Gaspari, excluindo inicialmente
            "bsw": self.bsw,
            #"densidadeAgua": self.densidadeAgua,             # conforme conversa com Gaspari, excluindo inicialmente
            #"temp1": self.temp1,                             # conforme conversa com Gaspari, excluindo inicialmente
            #"visc1": self.visc1,                             # conforme conversa com Gaspari, excluindo inicialmente
            #"temp2": self.temp2,                             # conforme conversa com Gaspari, excluindo inicialmente
            #"visc2": self.visc2,                             # conforme conversa com Gaspari, excluindo inicialmente
            #"tipoEmul": self.tipoEmul,                       # conforme conversa com Gaspari, excluindo inicialmente
            "bswCorte": self.bswCorte,
            #"fracCO2": self.fracCO2,                         # conforme conversa com Gaspari, excluindo inicialmente
            #"correlacaoCritica": self.correlacaoCritica,     # conforme conversa com Gaspari, excluindo inicialmente
            "ativo": self.ativo
        }
        oFluidCollectionNode.append(oFluidNode)