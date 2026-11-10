from typing import List

class MaterialObject:
    def __init__(self, iIndex: int, oExcelLine: List[str]):
        #self.iId = iIndex
        #self.sName, self.sConductivity, self.sRho, self.sCP, self.sType, self.sViscosity, self.sThermalExpCoef = self.__GetMaterialAttributes(oExcelLine)
        self.iId, self.sName, self.sType, self.sConductivity, self.sCP, self.sRho, self.sViscosity, self.sThermalExpCoef = self.__GetMaterialAttributes(oExcelLine)
        self.sType = self.__convertTypeToCode(self.sType)




    def __GetMaterialAttributes(self, oExcelLine: List[str]) -> List[str]:
        attributes = oExcelLine[:6]  

        for i in range(6, 8):
            if i < len(oExcelLine) and oExcelLine[i] != "":
                attributes.append(oExcelLine[i])
            else:
                attributes.append("-100")

        return attributes



    def __convertTypeToCode(self, sType: str) -> str:
        if sType == "Sólido":
            return "0"
        elif sType == "Fluido":
            return "1"
        elif sType == "Água":
            return "2"
        elif sType == "Ar":
            return "3"
        else:
            return sType


    def Compare(self, other: 'MaterialObject') -> bool:
        # Verifica se o objeto recebido é da mesma classe
        if not isinstance(other, MaterialObject):
            return False
    
        # Compara todos os atributos
        return (
            self.sName == other.sName and
            self.sConductivity == other.sConductivity and
            self.sRho == other.sRho and
            self.sCP == other.sCP and
            self.sType == other.sType and
            self.sViscosity == other.sViscosity and
            self.sThermalExpCoef == other.sThermalExpCoef)