from typing import List, Dict, Any

    # Objeto representando o choke de superfície conforme preenchido no json do Marlim 3
class ChokeSupObject:
    def __init__(self, bIncludeChoke: bool, dCoefDescarga: float, oAbertura: List[float], oTempo: List[float]):
        self.dCoefDescarga = dCoefDescarga
        self.oAbertura = oAbertura
        self.oTempo = oTempo

        if not bIncludeChoke:
            self.oTempo = [0.0]
            self.oAbertura = [1.0]
            self.dCoefDescarga = 1.0

    # Método para escrever este objeto no json:
    def WriteObjectToJsonFile(self, oChokeSupNode: Dict[str, Any]):
        oChokeSupNode["coeficienteDescarga"] = self.dCoefDescarga
        oChokeSupNode["modelo"] = 0
        oChokeSupNode["abertura"] = self.oAbertura
        oChokeSupNode["tempo"] = self.oTempo