from typing import List, Dict, Any

    # Objeto representando uma solicitação de perfil ao Marlim 3
class ProfileObject:
    def __init__(self, oTempo: List[float], oSelectedVariables: List[str]):
        
        self.oTempo = oTempo
        self.oSelectedVariables = oSelectedVariables

        if not (len(oTempo) > 0):
            self.oTempo = [0.0]

    # Método para escrever este perfil no json:
    def WriteObjectToJsonFile(self, oProfileNode: Dict[str, Any]):

        oProfileNode.clear()
        
        for variable in self.oSelectedVariables:
            oProfileNode[variable] = True

        oProfileNode["tempo"] = self.oTempo