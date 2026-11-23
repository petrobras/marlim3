from typing import List, Dict
from .MaterialObject import MaterialObject
from .RockObject import RockObject
from .WallLayerObject import WallLayerObject

class WallObject:
    # ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # ------------------ CONSTRUTOR ORIGINAL PRESERVADO
    # ----> Para voltar a usar (se necessário), basta descomentar TODAS as linhas deste bloco.
    #def __init__(self, iIndex: int, oExcelLine: List[str], oMaterialObjectList: List[MaterialObject], oRockObjectList: List[RockObject]):
    #    self.iId = iIndex
    #    self.sWallName = oExcelLine[0]
    #    sWallLayerWidthList = oExcelLine[1].split(",")
    #    sWallLayerMaterialList = oExcelLine[2].split(",")
    #    self.oWallLayerList = {}
    #
    #    for index, sWallLayerMaterial in enumerate(sWallLayerMaterialList):
    #        sWallLayerWidth = sWallLayerWidthList[index]
    #        wall_layer = WallLayerObject(iIndex=index, dWidth=float(sWallLayerWidth), sMaterialName=sWallLayerMaterial, oMaterialObjectList=oMaterialObjectList, oRockObjectList=oRockObjectList)
    #        self.oWallLayerList[wall_layer.iSequentialOrder] = wall_layer
    #
    #    self.dTotalWallThicknessInMeters = sum(wall_layer.dWidthInMeters for wall_layer in self.oWallLayerList.values())
    #    self.bHasCompletionFluid = any(wall_layer.bIsCompletionFluid for wall_layer in self.oWallLayerList.values())
    #    self.oExternalWallLayer = self.oWallLayerList[max(self.oWallLayerList.keys())] if self.oWallLayerList else None
    #    self.bWallEndsInRock = self.oExternalWallLayer.bIsRockLayer if self.oExternalWallLayer else False
    # ------------------ FIM DO CONSTRUTOR ORIGINAL PRESERVADO    
    # ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    def __init__(self, oExcelLine: List[str], oMaterialObjectList: List[MaterialObject], oRockObjectList: List[RockObject]):
        self.iId = int(oExcelLine[0])
        self.sWallName = oExcelLine[1]
        sWallLayerWidthList = oExcelLine[6].split(",")
        sWallLayerMaterialIDList = oExcelLine[7].split(",")    # Variável nova!
        self.oWallLayerList = {}

        # Código novo (para evitar modificação no construtor de "WallLayerObject"):
        sWallLayerMaterialList = []
        for sIdStr in sWallLayerMaterialIDList:
            iMaterialId = int(sIdStr.strip())
            # Buscar o MaterialObject com esse iId
            for oMaterial in oMaterialObjectList:
                if oMaterial.iId == iMaterialId:
                    sWallLayerMaterialList.append(oMaterial.sName)
                    break        

        # Retomando conforme construtor original:
        for index, sWallLayerMaterial in enumerate(sWallLayerMaterialList):
            sWallLayerWidth = sWallLayerWidthList[index]
            wall_layer = WallLayerObject(iIndex=index, dWidth=float(sWallLayerWidth), sMaterialName=sWallLayerMaterial, oMaterialObjectList=oMaterialObjectList, oRockObjectList=oRockObjectList)
            self.oWallLayerList[wall_layer.iSequentialOrder] = wall_layer

        self.dTotalWallThicknessInMeters = sum(wall_layer.dWidthInMeters for wall_layer in self.oWallLayerList.values())
        self.bHasCompletionFluid = any(wall_layer.bIsCompletionFluid for wall_layer in self.oWallLayerList.values())
        self.oExternalWallLayer = self.oWallLayerList[max(self.oWallLayerList.keys())] if self.oWallLayerList else None
        self.bWallEndsInRock = self.oExternalWallLayer.bIsRockLayer if self.oExternalWallLayer else False




    def Compare(self, other: 'WallObject') -> bool:
        # Verifica se o objeto recebido é da mesma classe
        if not isinstance(other, WallObject):
            return False

        # Comparar o atributo oWallLayerList
        if len(self.oWallLayerList) != len(other.oWallLayerList):
            return False

        for key in self.oWallLayerList:
            if key not in other.oWallLayerList or not self.oWallLayerList[key].Compare(other.oWallLayerList[key]):
                return False

        # Comparar o atributo oExternalWallLayer
        if (not(self.oExternalWallLayer is None)) and (other.oExternalWallLayer is None):
            return False
        elif (self.oExternalWallLayer is None) and (not(other.oExternalWallLayer is None)):
            return False
        else:
            if not self.oExternalWallLayer.Compare(other.oExternalWallLayer):
                return False

        # Comparar outros atributos relevantes
        return (self.dTotalWallThicknessInMeters == other.dTotalWallThicknessInMeters and
            self.bHasCompletionFluid == other.bHasCompletionFluid and
            self.bWallEndsInRock == other.bWallEndsInRock)
