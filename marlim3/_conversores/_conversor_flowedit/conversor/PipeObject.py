from typing import List, Dict, Any
from .CrossSectionObject import CrossSectionObject
from .WallObject import WallObject
import math

class PipeObject:
# ------------> CONSTRUTOR ANTIGO (comentado em Jul-2026)
# (caso necessário reativar, basta remover o "#" inicial de TODAS as linhas)
#    def __init__(self, iIndex: int, sLabelSuffix: str, oExcelLine: List[str], oPreviousExcelLine: List[str], oInternalFlowpath: List['PipeObject'],
#                 oFloweditWallObjectInstances: List['WallObject'], oCrossSectionObjectInstances: List['CrossSectionObject']):
#
#        # Armazenar informações:
#        self.iId = iIndex
#        self.sPipeLabel = oExcelLine[0]
#        self.dX = float(oExcelLine[1])
#        self.dY = float(oExcelLine[2])
#        self.dPreviousX = float(oPreviousExcelLine[1])
#        self.dPreviousY = float(oPreviousExcelLine[2])
#        self.iNCells = int(oExcelLine[3])
#        self.dInternalDiameterInMeters = float(oExcelLine[4]) * 0.0254
#        self.dExternalDiameterInMeters = float(oExcelLine[4]) * 0.0254
#        self.dRoughnessInMeters = float(oExcelLine[5]) / 1000
#        self.sGivenExternalWallLabel = oExcelLine[6]
#        self.bNullInternalFlowPath = oInternalFlowpath is None
#        self.bHasAnnularCrossSection = False
#
#        # Rótulo para o json:
#        self.sJsonLabel = self.sPipeLabel.strip() + " [" + sLabelSuffix.strip() + "]"
#
#        # Tentar determinar mais alguns membros de localização variável no Excel:
#        self.dExternalVelocity = 0.0
#        self.iExternalSurroundings = -100
#        if(len(oExcelLine) == 8):   
#            # coluna de produção ou anular do poço.
#            # Usar a temperatura da formação:
#            self.dExternalTemperature = float(oExcelLine[7])
#            
#            # Obter também a temperatura a montante:
#            self.dUpstreamExternalTemperature = float(oPreviousExcelLine[7])
#            #self.dUpstreamExternalTemperature = self.dExternalTemperature
#        else:
#            # duto de produção ou de serviço.
#
#            # Usar a temperatura do fluido externo:
#            self.dExternalTemperature = float(oExcelLine[8])
#
#            # Obter também a temperatura a montante:
#            self.dUpstreamExternalTemperature = float(oPreviousExcelLine[8])
#            #self.dUpstreamExternalTemperature = self.dExternalTemperature
#
#            # Obter a velocidade do fluido externo:
#            self.dExternalVelocity = float(oExcelLine[9])
#
#            # Determinar o código do ambiente externo:
#            sExternalSurrLabel = oExcelLine[7]
#            if sExternalSurrLabel.strip() == "Água do mar":
#                self.iExternalSurroundings = 1
#            else:
#                if sExternalSurrLabel.strip() == "Ar atmosférico":
#                    self.iExternalSurroundings = 2
#
#
#        # Inicializar mais alguns membros:
#        self.bIncludeInJson = True
#        self.iThermalCoupling = 0       # acoplamentoTermico do json
#        self.iConvectionDirection = 0   # direcaoConveccao do json
#
#        # Determinar ângulo:
#        deltaX = abs(self.dX - self.dPreviousX)
#        deltaY = abs(self.dY - self.dPreviousY)
#
#        # TESTE, APAGAR
#        #if(deltaX == 0.0):
#        #    print(self.sJsonLabel)
#
#        if(deltaX == 0.0):
#            dTangent = float('inf')
#            dAngleInRadians = math.pi / 2.0
#        else:
#            dTangent = deltaY / deltaX
#            dAngleInRadians = math.atan(dTangent)
#
#
#        if self.dY < self.dPreviousY:
#            dAngleInRadians *= -1
#        self.dAngleInRadians = dAngleInRadians
#
#        # Dado apurado com o Tobias: O sistema de referência de Y é sempre em relação ao nível do mar, e nunca muda na Flowedit.
#        # Y positivo indica acima do nível do mar, Y negativo indica abaixo do nível do mar.
#
#        # Calcular o comprimento da célula de discretização:
#        dCellLength = deltaX * deltaX + deltaY * deltaY
#        dCellLength = math.sqrt(dCellLength)
#        dCellLength /= self.iNCells
#        self.dCellLength = dCellLength
#
#        # Armazenar sempre o comprimento da primeira célula 
#        # (importante para a colocação de fontes de massa que servem de condição de contorno na entrada):
#        self.dFirstCellLength = self.dCellLength
#
#        # Obter a parede externa da seção transversal:
#        self.oExternalWallObject = None
#        for wall_object in oFloweditWallObjectInstances:
#                if wall_object.sWallName.strip() == self.sGivenExternalWallLabel.strip():
#                    self.oExternalWallObject = wall_object
#                    break
#
#        # Contato com a formação?
#        self.bCrossSectionEndsInFormation = False
#        self.iBoundaryFormationID = -100            # idFormacao no json
#        if self.oExternalWallObject.bWallEndsInRock:
#            self.bCrossSectionEndsInFormation = True
#            self.iBoundaryFormationID = self.oExternalWallObject.oExternalWallLayer.iRockID
#
#        # Providências específicas quando é anular e quando não é:
#        if self.bNullInternalFlowPath:
#            # NÃO é anular:
#            self.dExternalDiameterInMeters = 2 * self.oExternalWallObject.dTotalWallThicknessInMeters + self.dInternalDiameterInMeters
#            # RodrigoGDT: comentando abaixo o laço original após remanejamento:
#            #for wall_object in oFloweditWallObjectInstances:
#            #    if wall_object.sWallName.strip() == self.sGivenExternalWallLabel:
#            #        self.oExternalWallObject = wall_object
#            #        self.dExternalDiameterInMeters = 2 * wall_object.dTotalWallThicknessInMeters + self.dInternalDiameterInMeters
#            #        break
#        else:
#            # É anular:
#
#            # Procurar o tubo interno do anular:
#            bFoundInternalPipeObject = False
#            for internal_pipe in oInternalFlowpath:
#                if abs(internal_pipe.dX - self.dPreviousX) < 0.01 and abs(internal_pipe.dY - self.dPreviousY) < 0.01:
#                    self.oInternalPipeObject = internal_pipe
#                    bFoundInternalPipeObject = True
#                    break
#
#            # Determinar se é um anular COM ESCOAMENTO ou não:
#            if bFoundInternalPipeObject:
#                self.dInternalDiameterInMeters = self.oInternalPipeObject.dExternalDiameterInMeters
#                self.bHasAnnularCrossSection = True
#                if self.oInternalPipeObject.oExternalWallObject.bHasCompletionFluid or self.oInternalPipeObject.oExternalWallObject.bWallEndsInRock:
#                # RodrigoGDT: "IF" comentado e aprimorado na linha acima:
#                # if self.oInternalPipeObject.bHasCompletionFluid or self.oInternalPipeObject.bWallEndsInRock:
#                    self.bHasAnnularCrossSection = False
#
#                # Mudar alguns parâmetros na situação em que há gás escoando pelo anular:
#                if self.bHasAnnularCrossSection:
#                    self.oInternalPipeObject.iThermalCoupling = 1           # coluna de produção
#                    self.oInternalPipeObject.iConvectionDirection = 1       # coluna de produção
#                    
#            # Se há gás escoando pelo anular:...
#            if self.bHasAnnularCrossSection:
#                self.iThermalCoupling = 1
#                self.iConvectionDirection = 1       
#            else:
#                self.bIncludeInJson = False
#
#        # Instanciar o objeto seção transversal, se for o caso:
#        if self.bIncludeInJson:
#            self.oCrossSectionObject = CrossSectionObject(len(oCrossSectionObjectInstances), self.sJsonLabel,
#                                           self.bHasAnnularCrossSection, self.dExternalDiameterInMeters,
#                                           self.dInternalDiameterInMeters, self.dRoughnessInMeters,
#                                           self.oExternalWallObject)
#            # ------------> código para evitar a criação de seções transversais idênticas:
#            bFoundIdenticalCrossSection = False
#
#            for cross_section in oCrossSectionObjectInstances:
#                if cross_section.Compare(self.oCrossSectionObject):
#                    bFoundIdenticalCrossSection = True
#                    self.oCrossSectionObject = cross_section
#                    break  # Interrompe o laço for
#
#            if not bFoundIdenticalCrossSection:
#            # ------------> fim do código para evitar a criação de seções transversais idênticas:
#                oCrossSectionObjectInstances.append(self.oCrossSectionObject)
#
#        # Providências extras:
#        self.ExtendPipeDef()
# ------------> FIM DO CONSTRUTOR ANTIGO (comentado em Jul-2026)


# ------------> INÍCIO DO NOVO CONSTRUTOR (JUL-2026)
    def __init__(self,  
                 sLabelSuffix: str, 
                 oExcelLine: List[str], 
                 oPreviousExcelLine: List[str],  
                 oCrossSectionObjectInstances: List['CrossSectionObject'],
                 dOffsetInX: float,
                 dOffsetInY: float
                 ):

        # Armazenar informações:
        self.iId = int(oExcelLine[0])
        self.sPipeLabel = oExcelLine[1]
        self.dX = float(oExcelLine[2]) - dOffsetInX
        self.dY = float(oExcelLine[3]) - dOffsetInY
        self.dPreviousX = float(oPreviousExcelLine[2]) - dOffsetInX
        self.dPreviousY = float(oPreviousExcelLine[3]) - dOffsetInY
        self.iNCells = int(oExcelLine[10])

        # Rótulo para o json:
        self.sJsonLabel = self.sPipeLabel.strip() + " [" + sLabelSuffix.strip() + "]"

        # Localizar o objeto Seção Transversal deste duto:
        iCrossSectionId = int(oExcelLine[4])

        self.oCrossSectionObject = None
        for oCrossSectionObject in oCrossSectionObjectInstances:
            if oCrossSectionObject.iId == iCrossSectionId:
                # "id" do objeto = "id" da FLOWedit!
                self.oCrossSectionObject = oCrossSectionObject
                break

        if self.oCrossSectionObject is None:
            sErrorMsg = f"CrossSectionObject com iId={iCrossSectionId} não encontrado"
            raise ValueError(sErrorMsg)

        self.dInternalDiameterInMeters = self.oCrossSectionObject.dInternalDiameter
        self.dExternalDiameterInMeters = self.oCrossSectionObject.dExternalDiameter
        self.dRoughnessInMeters = self.oCrossSectionObject.dRoughness
        self.bHasAnnularCrossSection = self.oCrossSectionObject.bAnular
        
        # "compInter" e variáveis que dependem disto:
        sCompInter = oExcelLine[11]

        if sCompInter != "0,1":
            # Por enquanto, supor que a FLOWedit só trabalhará com "0, 1".
            # Lançar exceção para alertar caso venha a surgir configuração diferente.
            sErrorMsg = "Encontrado compInter diferente de 0, 1"
            raise ValueError(sErrorMsg)

        else:
            # Velocidade externa:
            sExternalVelocList = oExcelLine[13].split(",")
            self.dUpstreamExternalVelocity = float(sExternalVelocList[0])   # em m/s
            self.dDownstreamExternalVelocity = float(sExternalVelocList[1]) # em m/s

            # Temperatura externa:
            sExternalTempList = oExcelLine[12].split(",")
            self.dUpstreamExternalTemperature = float(sExternalTempList[0]) # oC
            self.dExternalTemperature = float(sExternalTempList[1])         # oC


        # Código do ambiente externo:
        self.iExternalSurroundings = -100
        sExternalSurrLabel = oExcelLine[5]
        if sExternalSurrLabel.strip() == "Água do mar":
            self.iExternalSurroundings = 1
        else:
            if sExternalSurrLabel.strip() == "Atmosfera":
                self.iExternalSurroundings = 2

        # Acoplamento térmico:
        sThermalCoupling = oExcelLine[7]
        self.iThermalCoupling = -100       # acoplamentoTermico do json
        if sThermalCoupling.strip() == "Sim":
            self.iThermalCoupling = 1
        else:
            if sThermalCoupling.strip() == "Não":
                self.iThermalCoupling = 0

        # Direção da convecção:
        sConvectionDirection = oExcelLine[8]       
        self.iConvectionDirection = -100   # direcaoConveccao do json
        if sConvectionDirection.strip() == "Longitudinal":
            self.iConvectionDirection = 1
        else:
            if sConvectionDirection.strip() == "Transversal":
                self.iConvectionDirection = 0


        # Determinar ângulo:
        deltaX = abs(self.dX - self.dPreviousX)
        deltaY = abs(self.dY - self.dPreviousY)

        if(deltaX == 0.0):
            dTangent = float('inf')
            dAngleInRadians = math.pi / 2.0
        else:
            dTangent = deltaY / deltaX
            dAngleInRadians = math.atan(dTangent)


        if self.dY < self.dPreviousY:
            dAngleInRadians *= -1

        self.dAngleInRadians = dAngleInRadians

        # Dado apurado com o Tobias: O sistema de referência de Y é sempre em relação ao nível do mar, e nunca muda na Flowedit.
        # Y positivo indica acima do nível do mar, Y negativo indica abaixo do nível do mar.

        # Calcular o comprimento da célula de discretização:
        dCellLength = deltaX * deltaX + deltaY * deltaY
        dCellLength = math.sqrt(dCellLength)
        dCellLength /= self.iNCells
        self.dCellLength = dCellLength

        # Armazenar sempre o comprimento da primeira célula 
        # (importante para a colocação de fontes de massa que servem de condição de contorno na entrada):
        self.dFirstCellLength = self.dCellLength

        # Obter a parede externa da seção transversal:
        self.oExternalWallObject = self.oCrossSectionObject.oWallObject

        # Formação:
        sFormationID = oExcelLine[6]
        self.bCrossSectionEndsInFormation = False
        self.iBoundaryFormationID = -100
        if sFormationID != "":
            self.bCrossSectionEndsInFormation = True
            self.iBoundaryFormationID = int(sFormationID)

        # Agrupamento:
        sGrouping = oExcelLine[9]
        self.bGrouping = (sGrouping.strip() == "Sim")

        # Providências extras:
        self.ExtendPipeDef()
# ------------> FIM DO NOVO CONSTRUTOR (JUL-2026)


    def ExtendPipeDef(self):
        pass

    # Método para escrever os dados no json
    def WriteDuctDataToJson(self, oDuctCollectionNode: Dict[str, Any]):
        # Criar um "node" para este duto:
        oDuctNode = {
            "id": self.iId,
            "rotulo": self.sJsonLabel,
            "idCorte": self.oCrossSectionObject.iId,
            "angulo": self.dAngleInRadians,
            "xCoor": self.dX,
            "yCoor": self.dY,
            "acoplamentoTermico": self.iThermalCoupling,    
            "idFormacao": self.iBoundaryFormationID,
            "direcaoConveccao": self.iConvectionDirection,      
            "ambienteExterno": self.iExternalSurroundings,
            "nCelulas_XY": self.iNCells,        
            #"agrupamento": True,       # Linha original comentada e substituída pela próxima (abaixo)
            "agrupamento": self.bGrouping,        
            # "dxCelula": [13.1404665779191, 16.8595334220809], # Omitindo após conversa com Gaspari (agrupamento "True" direciona pra "discretizacao").
            "discretizacao": [
                {
                    "nCelulas": self.iNCells,
                    "comprimento": self.dCellLength
                }
            ],
            "condicoesIniciais": {
                "compInter": [0, 1],
                "tempExterna": [self.dUpstreamExternalTemperature, self.dExternalTemperature],         
                "velExterna": [self.dUpstreamExternalVelocity, self.dDownstreamExternalVelocity],                              
                # ---> Comentados abaixo: somente se ambienteExterno for zero, indicando que é informação dada pelo usuário:
                #"kExterna": [0.0006274385656572484, 0.0006274385656572484],        
                #"calorEspecificoExterno": [100, 100],                              
                #"rhoExterno": [1000, 1000],                                        
                #"viscExterna": [1, 1],                                                         
            }
        }

        if not self.bCrossSectionEndsInFormation:
            del oDuctNode["idFormacao"]

        # Adicionar no "node" do "json":
        oDuctCollectionNode.append(oDuctNode)