from typing import List, Dict, Any

    # Objeto representando uma IPR na linha de produção
class IPRObject:
    def __init__(self, iId: int, fComprimentoMedido: float, iTipoIPR: int, oTemperatura: List[float], oPressaoEstatica: List[float],
                 oIP: List[float], oII: List[float], iIndFluidoPro: int, oTempo: List[float], oQMax: List[float]):
        
        self.iId = iId
        self.fComprimentoMedido = fComprimentoMedido
        self.iTipoIPR = iTipoIPR
        self.oTemperatura = oTemperatura
        self.oPressaoEstatica = oPressaoEstatica
        self.oIP = oIP
        self.oII = oII
        self.iIndFluidoPro = iIndFluidoPro
        self.oTempo = oTempo
        self.oQMax = oQMax

    # Método para escrever esta IPR no json:
    def WriteObjectToJsonFile(self, oIPRCollectionNode: Dict[str, Any]):

        oIPRNode = {
            "id": self.iId,
            "comprimentoMedido": self.fComprimentoMedido,
            "tipoIPR": self.iTipoIPR,
            "pressaoEstatica": self.oPressaoEstatica,
            "tempoPressaoEstatica": self.oTempo,
            "temperaturas": self.oTemperatura,
            "tempoTemperaturas": self.oTempo,
            "ip": self.oIP,
            "tempoip": self.oTempo,
            "ii": self.oII,
            "tempoii": self.oTempo,
            "qMax": self.oQMax,
            "tempoqMax": self.oTempo,
            "indFluidoPro": self.iIndFluidoPro
            }

        oIPRCollectionNode.append(oIPRNode)