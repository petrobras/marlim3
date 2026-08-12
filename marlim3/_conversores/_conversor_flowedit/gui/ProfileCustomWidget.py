from typing import List
from PyQt5.QtCore import Qt
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLineEdit, QHBoxLayout, QCheckBox, QLabel, QComboBox, QRadioButton, QTableWidget, QHeaderView, QPushButton
from VariableSelectionBasedCustomWidget import VariableSelectionBasedCustomWidget, TypeDuct
from ProfileObject import ProfileObject


# Classe para definir um controle customizado onde o usuário monta solicitações de perfis de saída para o Marlim:
class ProfileCustomWidget(VariableSelectionBasedCustomWidget):
    def __init__(self, oGUIReference, eTypeDuct):
        super().__init__(oGUIReference, eTypeDuct)

        self.oCreatedCustomWidget = self.CreateCustomWidget()


    # Método para criar o widget de edição deste perfil:
    def CreateCustomWidget(self) -> QWidget:
        
        oCustomWidget = QWidget()

        #oCustomWidget.setMaximumWidth(300)

        oCustomWidgetLayout = QHBoxLayout()

        oVariableSelectionPortionLayout = QVBoxLayout()

        

        if self.eTypeDuct == TypeDuct.Producao:
            oDefaultSelectedVariables = ["pressao", "temperatura", "holdup"]
        elif self.eTypeDuct == TypeDuct.Servico:
            oDefaultSelectedVariables = ["pressao", "temperatura", "rhog"]

        self.oVariableSelectionTable = self.GenerateVariableSelectionTableWidget(self.GetSelectableVariablesStandardDictionary(), oDefaultSelectedVariables)
        oVariableSelectionPortionLayout.addWidget(self.oVariableSelectionTable)
        #self.oVariableSelectionTable.setFixedWidth(200)
        self.oVariableSelectionTable.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)

        chkOnlySelected = QCheckBox("Filtrar selecionadas")
        chkOnlySelected.stateChanged.connect(self.FilterSelectedCheckBoxChanged)
        oVariableSelectionPortionLayout.addWidget(chkOnlySelected)

        oCustomWidgetLayout.addLayout(oVariableSelectionPortionLayout)

        oTimesInsertionPortionLayout = QVBoxLayout()
        self.oTimesTable = QTableWidget(1, 1)
        self.oTimesTable.verticalHeader().setVisible(False)
        self.oTimesTable.horizontalHeader().setVisible(False)
        #self.oTimesTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.cmbTimesUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oTimesTable, 0, 0, "", "Instantes de", "t", "Tempo")

        #self.cmbTimesUnit.setFixedWidth(90)
        self.oTimesTable.resizeColumnsToContents()
        #self.oTimesTable.setSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        #self.oTimesTable.setMaximumWidth(70)
        self.oTimesTable.setMaximumWidth(self.oTimesTable.columnWidth(0)+3)

        oTimesInsertionPortionLayout.addWidget(self.oTimesTable, alignment=Qt.AlignHCenter)
        
        oTimesInsertionButtonsLayout = QHBoxLayout()
        btnAddProfileTime = QPushButton("+", clicked = self.AddRowToTimesTable)
        btnAddProfileTime.setMaximumWidth(40)
        btnRemoveProfileTimes = QPushButton("-", clicked = self.RemoveRowFromTimesTable)
        btnRemoveProfileTimes.setMaximumWidth(40)
        oTimesInsertionButtonsLayout.addWidget(btnAddProfileTime)
        oTimesInsertionButtonsLayout.addWidget(btnRemoveProfileTimes)
        oTimesInsertionPortionLayout.addLayout(oTimesInsertionButtonsLayout)
        oCustomWidgetLayout.addLayout(oTimesInsertionPortionLayout)


        oCustomWidget.setLayout(oCustomWidgetLayout)
        return oCustomWidget


    # Botões de adicionar e remover tempos:
    def RemoveRowFromTimesTable(self):
        selected_ranges = self.oTimesTable.selectedRanges()

        if not selected_ranges:
            return

        rows_to_remove = set()

        for selected_range in selected_ranges:
            top_row = selected_range.topRow()
            bottom_row = selected_range.bottomRow()

            for row in range(top_row, bottom_row + 1):
                rows_to_remove.add(row)

        rows_to_remove = sorted(rows_to_remove, reverse=True)

        for row in rows_to_remove:
            self.oTimesTable.removeRow(row)

    def AddRowToTimesTable(self):
        row_count = self.oTimesTable.rowCount()
        self.oTimesTable.insertRow(row_count)


    # Método para obter um objeto centralizando as informações deste perfil de saída:
    def GetProfileObject(self) -> ProfileObject:

        tempos = []
        for row in range(1, self.oTimesTable.rowCount()):
            item = self.oTimesTable.item(row, 0)

            if item:
                #print("cheguei aqui")
                item_value = self.oGUIReference.ConvertUnit(float(item.text()), "t", self.cmbTimesUnit, "s")
                tempos.append(item_value)
            else:
                tempos.append(-100.0)

        oSelectedVariables = self.GetUserCheckedVariableKeys()

        oProfObj = ProfileObject(tempos, oSelectedVariables)
        return oProfObj

    # Método para obter as variáveis possíveis de serem selecionadas pelo usuário para este perfil:
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
                "RS": "Razão de solubilidade @ P,T",
                "vazaoMassicaGas": "Vazão mássica do vapor",
                "vazaoMassicaLiquido": "Vazão mássica do líquido",
                "c0": "Coeficiente de distribuição (modelo Drift-Flux)",
                "ud": "Velocidade de escorregamento (modelo Drift-Flux)",
                "RGO": "RGO considerando escorregamento",
                "deng": "",
                "yco2": "Fração molar de CO2 na fase vapor",
                "calor": "Fluxo de calor entre fases",
                "masstrans": "Transferência de massa entre fases",
                "cpgas": "Calor específico (CP) do vapor",
                "cpliq": "Calor específico (CP) do líquido",
                "QLstd": "Vazão volumétrica standard de pesados (líquido)",
                "QLWstd": "Vazão volumétrica standard de líquido",
                "QLstdTotal": "Vazão volumétrica standard total de líquido",
                "QGstd": "Vazão volumétrica standard de leves (vapor)",
                "api": "Grau API",
                "bsw": "BSW",
                "hidro": "Termo hidrostático",
                "fric": "Termo fricção",
                "Term1": "",
                "Term2": "",
                "dengD": "Densidade do gás dissolvido in situ",
                "dengL": "Densidade do gás livre in situ",
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
                "temperaturaAmbiente": "Temperatura Ambiente",
                "PrandtlGas": "Número de Prandtl do Gás",
                "PrandtlLiquido": "Número de Prandtl do Líquido",
                "pseudoLiquido": "",
                "pseudoGas": "",
                "pseudoMist": "",
                "TResi": "",
                "RedutorAtrito": ""
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
                "QGstd": "Vazão volumétrica standard do vapor",
                "hidro": "Termo hidrostático",
                "fric": "Termo fricção",
                "calor": "Fluxo de calor",
                "ReyInterno": "Número de Reynolds interno",
                "ReyExterno": "Número de Reynolds externo",
                "GrashInterno": "Número de Grashof interno",
                "GrashExterno": "Número de Grashof externo",
                "NusselInterno": "Número de Nusselt interno",
                "NusselExterno": "Número de Nusselt externo",
                "PrandtlInterno": "Número de Prandtl interno",
                "PrandtlExterno": "Número de Prandtl externo",
                "Hint": "",
                "Hext": "",
                "temperaturaAmbiente": "Temperatura ambiente"
}

        # Experimentando ordenar em ordem alfabética antes de retornar:
        key_value_pairs = [(key, value) for key, value in oKeysDescriptions.items()]
        sorted_pairs = sorted(key_value_pairs, key=lambda item: item[1])
        ordered_dict = dict(sorted_pairs)
        oKeysDescriptions = ordered_dict

        return oKeysDescriptions