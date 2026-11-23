from typing import List, Dict
from .MaterialObject import MaterialObject
from .RockObject import RockObject
from .WallObject import WallObject

class CrossSectionObject:
    # ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # ------------------ CONSTRUTOR ORIGINAL PRESERVADO
    # ----> Para voltar a usar (se necessário), basta descomentar TODAS as linhas deste bloco.
    #def __init__(self, iIndex: int, sCrossSectionLabel: str, bAnular: bool, dExternalDiameter: float,
    #             dInternalDiameter: float, dRoughness: float, oWall: WallObject):
    #    self.iId = iIndex
    #    self.sCrossSectionLabel = sCrossSectionLabel
    #    self.bAnular = bAnular
    #    self.dExternalDiameter = dExternalDiameter
    #    self.dInternalDiameter = dInternalDiameter
    #    self.dRoughness = dRoughness
    #    self.oCrossSectionLayerList = oWall.oWallLayerList
    # ------------------ FIM DO CONSTRUTOR ORIGINAL PRESERVADO    
    # ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    def __init__(self, oExcelLine: List[str], oMaterialObjectList: List[MaterialObject], oRockObjectList: List[RockObject]):

        # ---------> TESTE 6-JUL-2026
        #print(f"DEBUG: oExcelLine = {oExcelLine}")
        #print(f"DEBUG: len(oExcelLine) = {len(oExcelLine)}")
        #print(f"DEBUG: Índices disponíveis: 0-{len(oExcelLine)-1}")
        # ---------> FIM DO TESTE 6-JUL-2026

        oWall = WallObject(oExcelLine, oMaterialObjectList, oRockObjectList)

        self.iId = int(oExcelLine[0])
        self.sCrossSectionLabel = oExcelLine[1]
        self.bAnular = str(oExcelLine[2]).strip() == "Sim"
        self.dInternalDiameter = float(oExcelLine[4]) * 0.0254        # converter para metros

        if self.bAnular:
            self.dExternalDiameter = float(oExcelLine[3]) * 0.0254    # convertendo para metros
        else:
            self.dExternalDiameter = self.dInternalDiameter + (2.0 * oWall.dTotalWallThicknessInMeters)

        self.dRoughness = float(oExcelLine[5]) / 1000.0      # converter para metros
        self.oCrossSectionLayerList = oWall.oWallLayerList

        self.oWallObject = oWall


    def Compare(self, other: 'CrossSectionObject') -> bool:
        # Verifica se o objeto recebido é da mesma classe
        if not isinstance(other, CrossSectionObject):
            return False

        # Comparar o atributo oCrossSectionLayerList
        if len(self.oCrossSectionLayerList) != len(other.oCrossSectionLayerList):
            return False

        for key in self.oCrossSectionLayerList:
            if key not in other.oCrossSectionLayerList or not self.oCrossSectionLayerList[key].Compare(other.oCrossSectionLayerList[key]):
                return False

        # Comparar outros atributos relevantes
        return (self.bAnular == other.bAnular and
            self.dExternalDiameter == other.dExternalDiameter and
            self.dInternalDiameter == other.dInternalDiameter and
            self.dRoughness == other.dRoughness)