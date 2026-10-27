from typing import Dict, Any

class GasFluidObject:
    def __init__(self, id: int = 0, correlacaoCritica: int = 1, densidadeGas: float = -100, fracCO2: float = -100, ativo: bool = True):
        self.id = id
        self.correlacaoCritica = correlacaoCritica
        self.densidadeGas = densidadeGas
        self.fracCO2 = fracCO2
        self.ativo = ativo

        # Método para escrever este fluido no json:
    def WriteGasFluidObjectToJsonFile(self, oFluidGasNode: Dict[str, Any]):
        #oFluidNode = {
        #    "id": self.id,
        #    "densidadeGas": self.densidadeGas,               
        #    "fracCO2": self.fracCO2,                         
        #    "correlacaoCritica": self.correlacaoCritica,     
        #    "ativo": self.ativo
        #}
        #oFluidGasNode.append(oFluidNode)

        oFluidGasNode["id"] = self.id
        oFluidGasNode["densidadeGas"] = self.densidadeGas
        oFluidGasNode["fracCO2"] = self.fracCO2
        oFluidGasNode["correlacaoCritica"] = self.correlacaoCritica
        oFluidGasNode["ativo"] = self.ativo