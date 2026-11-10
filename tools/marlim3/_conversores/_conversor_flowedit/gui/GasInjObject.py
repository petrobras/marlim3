from typing import List, Dict, Any

    # Objeto representando a condição de contorno de injeção de gás, da linha de serviço, lá na plataforma
class GasInjObject:
    def __init__(self, iTipoCC: int, bChuteVazaoInjecao: bool, oTemperatura: List[float], 
                 oPressaoInjecao: List[float], oVazaoGas: List[float], oTempo: List[float]):
        self.iTipoCC = iTipoCC
        self.bChuteVazaoInjecao = bChuteVazaoInjecao
        self.oTemperatura = oTemperatura
        self.oPressaoInjecao = oPressaoInjecao
        self.oVazaoGas = oVazaoGas
        self.oTempo = oTempo

    # Método para escrever esta condição de contorno no json:
    def WriteObjectToJsonFile(self, oGasInjNode: Dict[str, Any]):
        oGasInjNode["tipoCC"] = self.iTipoCC
        oGasInjNode["chuteVazaoInjecao"] = self.bChuteVazaoInjecao
        oGasInjNode["temperatura"] = self.oTemperatura
        oGasInjNode["pressaoInjecao"] = self.oPressaoInjecao
        oGasInjNode["vazaoGas"] = self.oVazaoGas
        oGasInjNode["tempo"] = self.oTempo