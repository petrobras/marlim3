from typing import List, Dict, Any
import os

    # Objeto centralizando configurações de simulação permanente ou transiente:
class TimeConfigObject:
    def __init__(self, bTransiente: bool, iCondicaoInicial: int, sSnapshotArq: str, dTempoFinal: float, oTemposDtMax: List[float], oDtMax: List[float],
                 oGravaMomento: List[float]):
        
        self.bTransiente = bTransiente
        self.iCondicaoInicial = iCondicaoInicial
        self.sSnapshotArq = sSnapshotArq
        self.dTempoFinal = dTempoFinal
        self.oTemposDtMax = oTemposDtMax
        self.oDtMax = oDtMax
        self.oGravaMomento = oGravaMomento

        if (len(oTemposDtMax) == 0) or (len(oDtMax) == 0):
            self.oTemposDtMax = [0.0]
            self.oDtMax = [5.0]

            # Segundo Gaspari, a configuração acima dá um passo de 5 segundos constante para toda uma simulação
            #       transiente - ainda segundo ele, um bom valor default, considerando o tamanho de célula que o
            #       pessoal usa.

    # Método para escrever este objeto no json:
    def WriteObjectToJsonFile(self, json_data: Dict[str, Any]):

        oConfigInicialNode = json_data.get("configuracaoInicial", {})

        oConfigInicialNode["transiente"] = self.bTransiente
        oConfigInicialNode["condicaoInicial"] = self.iCondicaoInicial
        if self.sSnapshotArq.strip() and (self.iCondicaoInicial == 2) and self.bTransiente:
            oConfigInicialNode["SnapShotArq"] = os.path.basename(self.sSnapshotArq)

        if self.bTransiente:

            oTempoNode = json_data.get("tempo")
            if oTempoNode is None:
                oTempoNode = {}
                json_data["tempo"] = oTempoNode
            else:
                oTempoNode.clear()

            oTempoNode["tempoFinal"] = self.dTempoFinal
            oTempoNode["tempos"] = self.oTemposDtMax
            oTempoNode["dtmax"] = self.oDtMax

            if len(self.oGravaMomento) > 0:
                oTempoNode["gravaMomento"] = self.oGravaMomento