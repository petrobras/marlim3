from typing import List, Dict, Any

    # Objeto representando uma fonte de massa na linha de produção
class ProdMassSourceObject:
    def __init__(self, iId: int, fComprimentoMedido: float, iTipoTermo: int, oTemperatura: List[float], oVazaoMassG: List[float],
                 oVazaoMassT: List[float], oVazaoMassC: List[float], iIndFluidoPro: int, oTempo: List[float]):
        
        self.iId = iId
        self.fComprimentoMedido = fComprimentoMedido
        self.iTipoTermo = iTipoTermo
        self.oTemperatura = oTemperatura
        self.oVazaoMassG = oVazaoMassG
        self.oVazaoMassT = oVazaoMassT
        self.oVazaoMassC = oVazaoMassC
        self.iIndFluidoPro = iIndFluidoPro
        self.oTempo = oTempo

    # Método para escrever esta fonte de massa no json:
    def WriteObjectToJsonFile(self, oProdMassSourceCollectionNode: Dict[str, Any]):

        oProdMassSourceNode = {
            "id": self.iId,
            "comprimentoMedido": self.fComprimentoMedido,
            "tipoTermo": self.iTipoTermo,
            "temperatura": self.oTemperatura,
            "vazaoMassG": self.oVazaoMassG,
            "vazaoMassT": self.oVazaoMassT,
            "vazaoMassC": self.oVazaoMassC,
            "indiFluidoPro": self.iIndFluidoPro,
            "tempo": self.oTempo,
            "ativo": True
            }

        oProdMassSourceCollectionNode.append(oProdMassSourceNode)