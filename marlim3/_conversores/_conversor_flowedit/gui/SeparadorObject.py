from typing import List, Dict, Any

    # Objeto representando a condição de contorno de pressão de chegada da linha de produção
class SeparadorObject:
    def __init__(self, oPressao: List[float], oTempo: List[float]):
        self.oPressao = oPressao
        self.oTempo = oTempo

    # Método para escrever esta condição de contorno no json:
    def WriteObjectToJsonFile(self, oSeparadorNode: Dict[str, Any]):
        oSeparadorNode["pressao"] = self.oPressao
        oSeparadorNode["tempo"] = self.oTempo
        oSeparadorNode["ativo"] = True