from typing import List
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLineEdit, QHBoxLayout, QCheckBox, QLabel, QComboBox, QRadioButton, QTableWidget, QHeaderView, QPushButton
from VariableSelectionBasedCustomWidget import VariableSelectionBasedCustomWidget, TypeDuct
from TrendObject import TrendObject

# Classe para criar um controle customizado para informar os detalhes de uma tendência
class TrendDetailsCustomWidget(VariableSelectionBasedCustomWidget):
    def __init__(self, oGUIReference, eTypeDuct):
        super().__init__(oGUIReference, eTypeDuct)

        self.oCreatedCustomWidget = self.CreateCustomWidget()

    # Método para criar o widget de edição dos detalhes desta tendência:
    def CreateCustomWidget(self) -> QWidget:
        
        oCustomWidget = QWidget()
        oCustomWidgetLayout = QVBoxLayout()

        oMeasuredLengthLayout = QHBoxLayout()
        lblMeasuredLength = QLabel("Comprimento medido: ")
        self.txtMeasuredLength = QLineEdit()
        self.cmbMeasuredLengthUnit = QComboBox()
        self.oGUIReference.ConfigUnitComboBox(self.cmbMeasuredLengthUnit, "L")
        oMeasuredLengthLayout.addWidget(lblMeasuredLength)
        oMeasuredLengthLayout.addWidget(self.txtMeasuredLength)
        oMeasuredLengthLayout.addWidget(self.cmbMeasuredLengthUnit)
        #oCustomWidgetLayout.addLayout(oMeasuredLengthLayout)

        oTimeIntervalLayout = QHBoxLayout()
        lblTimeInterval = QLabel("Intervalo de tempo: ")
        self.txtTimeInterval = QLineEdit()
        self.cmbTimeIntervalUnit = QComboBox()
        self.oGUIReference.ConfigUnitComboBox(self.cmbTimeIntervalUnit, "t")
        oTimeIntervalLayout.addWidget(lblTimeInterval)
        oTimeIntervalLayout.addWidget(self.txtTimeInterval)
        oTimeIntervalLayout.addWidget(self.cmbTimeIntervalUnit)
        #oCustomWidgetLayout.addLayout(oTimeIntervalLayout)

        if self.eTypeDuct == TypeDuct.Producao:
            oDefaultSelectedVariables = ["pressao", "temperatura", "holdup"]
        elif self.eTypeDuct == TypeDuct.Servico:
            oDefaultSelectedVariables = ["pressao", "temperatura", "rhog"]

        self.oVariableSelectionTable = self.GenerateVariableSelectionTableWidget(self.GetSelectableVariablesStandardDictionary(), oDefaultSelectedVariables)
        oCustomWidgetLayout.addWidget(self.oVariableSelectionTable)
        #self.oVariableSelectionTable.setFixedWidth(200)
        self.oVariableSelectionTable.resizeColumnsToContents()
        self.oVariableSelectionTable.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)


        #oCustomWidgetLayout.addSpacing(15)

        chkOnlySelected = QCheckBox("Filtrar selecionadas")
        chkOnlySelected.stateChanged.connect(self.FilterSelectedCheckBoxChanged)
        oCustomWidgetLayout.addWidget(chkOnlySelected)
        oCustomWidgetLayout.addSpacing(15)

        oTextBoxesLayout = QVBoxLayout()
        #oTextBoxesLayout.addWidget(chkOnlySelected)
        #oTextBoxesLayout.addWidget(lblMeasuredLength)
        oTextBoxesLayout.addLayout(oMeasuredLengthLayout)
        oTextBoxesLayout.addSpacing(7)
        #oTextBoxesLayout.addWidget(lblTimeInterval)
        oTextBoxesLayout.addLayout(oTimeIntervalLayout)
        oCustomWidgetLayout.addLayout(oTextBoxesLayout)

        oCustomWidget.setLayout(oCustomWidgetLayout)
        return oCustomWidget


    # Método para obter um objeto representando estes dados de tendência:
    def GetTrendObject(self, iId: int, sRotulo: str) -> TrendObject:

        fComprimentoMeasured = self.oGUIReference.ConvertUnit(float(self.txtMeasuredLength.text()), "L", self.cmbMeasuredLengthUnit, "m")
        fTimeInterval = self.oGUIReference.ConvertUnit(float(self.txtTimeInterval.text()), "t", self.cmbTimeIntervalUnit, "s")
        oSelectedVariables = self.GetUserCheckedVariableKeys()

        oTrendObject = TrendObject(iId, fComprimentoMeasured, fTimeInterval, oSelectedVariables, sRotulo)
        return oTrendObject

    # Método para obter as variáveis possíveis de serem selecionadas pelo usuário para esta tendência:
    def GetSelectableVariablesStandardDictionary(self):
        if self.eTypeDuct == TypeDuct.Producao:
            oKeysDescriptions = {
                "pressao": "Pressão",
                "temperatura": "Temperatura",
                "holdup": "Holdup de líquido",
                "bet": "Fração volumétrica de líquido complementar (em relação à fase líquida)",
                "ugs": "Velocidade superficial do vapor",
                "uls": "Velocidade superficial do líquido",
                "ug": "Velocidade média local do vapor",
                "ul": "Velocidade média local do líquido",
                "arra": "Indicador de arranjo de fases",
                "viscosidadeLiquido": "Viscosidade do líquido",
                "viscosidadeGas": "Viscosidade do vapor",
                "rhog": "Massa específica do vapor @ P,T",
                "rhol": "Massa específica do líquido @ P,T",
                "vazaoMassicaGas": "Vazão mássica de vapor",
                "vazaoMassicaLiquido": "Vazão mássica de líquido",
                "c0": "Coeficiente de distribuição (modelo Drift-Flux)",
                "ud": "Velocidade de escorregamento (modelo Drift-Flux)",
                "RGO": "RGO considerando escorregamento",
                "deng": "",
                "yco2": "Fração molar de CO2 na fase vapor",
                "calor": "Fluxo de calor entre fases",
                "masstrans": "Transferência de massa entre fases",
                "QLstd": "Vazão volumétrica standard de pesados (líquido)",
                "QLWstd": "Vazão volumétrica standard de líquido",
                "QLstdTotal": "Vazão volumétrica standard total de líquido",
                "QGstd": "Vazão volumétrica standard de leves (vapor)",
                "api": "Grau API",
                "bsw": "BSW",
                "hidro": "Termo hidrostático",
                "fric": "Termo fricção",
                "dengD": "Densidade do gás dissolvido in situ",
                "dengL": "Densidade do gás livre in situ",
                "mlFonte": "Fonte mássica - líquido (HC + água)",
                "mgFonte": "Fonte mássica - gás",
                "mcFonte": "Fonte mássica - líquido complementar",
                "deltaPBomba": "",
                "potenciaBomba": "",
                "tempChokeJus": "Temperatura a jusante do Choke de Superfície",
                "ReyInterno": "Número de Reynolds interno da mistura",
                "ReyExterno": "Número de Reynolds externo",
                "GrashInterno": "Número de Grashof interno da mistura",
                "GrashExterno": "Número de Grashof externo",
                "NusselInterno": "Número de Nusselt interno da mistura",
                "NusselExterno": "Número de Nusselt externo",
                "PrandtlInterno": "Número de Prandtl interno da mistura",
                "PrandtlExterno": "Número de Prandtl externo",
                "Froud": "",
                "Rs": "Razão de solubilidade",
                "Bo": "Fator volume de formação",
                "Hint": "",
                "Hext": "",
                "volMonM1PT": "Volume de líquido a montante da M1 @ P,T",
                "volJusM1PT": "Volume de líquido a jusante da M1 @ P,T",
                "volMonM1ST": "Volume standard de líquido a montante da M1",
                "volJusM1ST": "Volume standard de líquido a jusante da M1"
            }

        elif self.eTypeDuct == TypeDuct.Servico:
            oKeysDescriptions = {
                "pressao": "Pressão",
                "temperatura": "Temperatura",
                "ugs": "Velocidade superficial do vapor",
                "ug": "Velocidade média local do vapor",
                "tensaoCisalhamento": "Tensão cisalhante",
                "viscosidadeGas": "Viscosidade do vapor",
                "rhog": "Massa específica do vapor @ P,T",
                "vazaoMassicaGas": "Vazão mássica do vapor",
                "hidro": "Termo hidrostático",
                "fric": "Termo fricção",
                "calor": "Fluxo de calor",
                "QGstd": "Vazão volumétrica standard do vapor",
                "presEstagVGL": "Pressão de estagnação (VGL)",
                "tempEstagVGL": "Temperatura de estagnação (VGL)",
                "presGargVGL": "Pressão na garganta (VGL)",
                "tempGargVGL": "Temperatura na garganta (VGL)",
                "vazaoVGL": "Vazão na VGL",
                "VelocidadeMaximaGarganta": "",
                "ReyInterno": "Número de Reynolds interno",
                "ReyExterno": "Número de Reynolds externo",
                "GrashInterno": "Número de Grashof interno",
                "GrashExterno": "Número de Grashof externo",
                "NusselInterno": "Número de Nusselt interno",
                "NusselExterno": "Numero de Nusselt externo",
                "PrandtlInterno": "Número de Prandtl interno",
                "PrandtlExterno": "Número de Prandtl externo",
                "Hint": "",
                "Hext": ""
            }

        # Experimentando ordenar em ordem alfabética antes de retornar:
        key_value_pairs = [(key, value) for key, value in oKeysDescriptions.items()]
        sorted_pairs = sorted(key_value_pairs, key=lambda item: item[1])
        ordered_dict = dict(sorted_pairs)
        oKeysDescriptions = ordered_dict

        return oKeysDescriptions


    # PAREI AQUI EM 31-AGO-2023
    # Segundo Gaspari, perfil tem que ter pelo menos 1, mas tend não tem que ter