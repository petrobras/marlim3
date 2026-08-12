from typing import List

class RockObject:
    def __init__(self, iIndex: int, oExcelLine: List[str]):
        #self.iId = iIndex
        #self.sName, self.sConductivity, self.sRho, self.sCP = oExcelLine[:4]
        self.iId, self.sName, self.sConductivity, self.sCP, self.sRho = oExcelLine[:5]
