from typing import List
from .MaterialObject import MaterialObject
from .RockObject import RockObject

class WallLayerObject:
    def __init__(self, iIndex: int, dWidth: float, sMaterialName: str, oMaterialObjectList: List[MaterialObject], oRockObjectList: List[RockObject]):
        self.iSequentialOrder = iIndex
        self.dWidthInMeters = dWidth / 1000
        self.idMaterial = -100

        # buscar um MaterialObject em oMaterialObjectList pelo nome fornecido
        for material_object in oMaterialObjectList:
            #if material_object.sName == sMaterialName:     # RodrigoGDT: espaços em branco causando problema?
            if material_object.sName.strip() == sMaterialName.strip():
                self.idMaterial = material_object.iId
                break

        self.bIsCompletionFluid = False
        if sMaterialName.strip() == "Fluido de completação":
            self.bIsCompletionFluid = True

        self.bIsRockLayer = False
        self.iRockID = -100
        for rock_object in oRockObjectList:
            if rock_object.sName.strip() == sMaterialName.strip():
                self.bIsRockLayer = True
                self.iRockID = rock_object.iId
                break

        self.sTypeLayerMeasurement = "ESPESSURA"
        self.iDiscretization = 1
        self.sGivenMaterialName = sMaterialName


    def Compare(self, other: 'WallLayerObject') -> bool:
        # Verifica se o objeto recebido é da mesma classe
        if not isinstance(other, WallLayerObject):
            return False
    
        # Compara todos os atributos
        return (self.iSequentialOrder == other.iSequentialOrder and
            self.dWidthInMeters == other.dWidthInMeters and
            self.idMaterial == other.idMaterial and
            self.bIsCompletionFluid == other.bIsCompletionFluid and
            self.bIsRockLayer == other.bIsRockLayer and
            self.iRockID == other.iRockID and
            self.sTypeLayerMeasurement == other.sTypeLayerMeasurement and
            self.iDiscretization == other.iDiscretization and
            self.sGivenMaterialName.strip() == other.sGivenMaterialName.strip())
