from typing import List, Dict, Any

    # Objeto representando uma tendência dos resultados do Marlim 3
class TrendObject:
    def __init__(self, iId: int, fComprimentoMedido: float, fTimeInterval: float, oSelectedVariables: List[str], sRotulo: str):
        
        self.iId = iId
        self.fComprimentoMedido = fComprimentoMedido
        self.fTimeInterval = fTimeInterval
        self.oSelectedVariables = oSelectedVariables
        self.sRotulo = sRotulo

    # Método para escrever esta tendência no json:
    def WriteObjectToJsonFile(self, oTrendCollectionNode: Dict[str, Any]):

        oTrendNode = {
            "id": self.iId,
            "comprimentoMedido": self.fComprimentoMedido,
            "dt": self.fTimeInterval,
            "rotulo": self.sRotulo,
            "ativo": True
            }

        for variable in self.oSelectedVariables:
            oTrendNode[variable] = True

        oTrendCollectionNode.append(oTrendNode)