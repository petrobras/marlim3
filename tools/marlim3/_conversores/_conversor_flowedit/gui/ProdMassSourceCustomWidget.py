from typing import List
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLineEdit, QHBoxLayout, QCheckBox, QLabel, QComboBox, QRadioButton, QTableWidget, QHeaderView, QPushButton
from ProdMassSourceObject import ProdMassSourceObject

class ProdMassSourceCustomWidget:
    def __init__(self, oGUIReference):
        self.oGUIReference = oGUIReference

        # Criar o widget
        self.oCreatedCustomWidget = self.CreateProdMassSourceCustomWidget()

    # Método para criar o widget
    def CreateProdMassSourceCustomWidget(self) -> QWidget:

        oCustomWidget = QWidget()
        oCustomWidgetLayout = QVBoxLayout()
        

        measuredLengthLayout = QHBoxLayout()
        self.chkSourceInEntryOfLineProd = QCheckBox("Condição de contorno na entrada")
        self.chkSourceInEntryOfLineProd.stateChanged.connect(self.StateChangedInCheckBoxOfEntranceCondition)
        measuredLengthLayout.addWidget(self.chkSourceInEntryOfLineProd)
        measuredLengthLayout.addSpacing(50)
        measuredLengthLabel = QLabel("Comprimento medido: ")
        measuredLengthLayout.addWidget(measuredLengthLabel)
        self.txtMeasuredLength = QLineEdit()
        self.txtMeasuredLength.setFixedWidth(100)
        measuredLengthLayout.addWidget(self.txtMeasuredLength)
        self.cmbMeasuredLengthUnit = QComboBox()
        self.oGUIReference.ConfigUnitComboBox(self.cmbMeasuredLengthUnit, "L")
        measuredLengthLayout.addWidget(self.cmbMeasuredLengthUnit)

        oCustomWidgetLayout.addLayout(measuredLengthLayout)

        oCustomWidgetLayout.addSpacing(20)

        oFluidIdLayout = QHBoxLayout()
        lblIdFluid = QLabel("Identificador do fluido: ")
        self.txtFluidId = QLineEdit()
        oFluidIdLayout.addWidget(lblIdFluid)
        self.txtFluidId.setFixedWidth(100)
        oFluidIdLayout.addWidget(self.txtFluidId)
        oFluidIdLayout.addStretch()

        oCustomWidgetLayout.addLayout(oFluidIdLayout)

        oCustomWidgetLayout.addSpacing(20)

        oTipoTermoLayout = QHBoxLayout()
        self.rdgTipoTermoZero = QRadioButton("Vazões seguem relações termodinâmicas")
        self.rdgTipoTermoZero.setChecked(True)
        self.rdgTipoTermoUm = QRadioButton("Fixar vazões fornecidas pelo usuário")
        oTipoTermoLayout.addWidget(self.rdgTipoTermoZero)
        oTipoTermoLayout.addWidget(self.rdgTipoTermoUm)

        oCustomWidgetLayout.addLayout(oTipoTermoLayout)

        oCustomWidgetLayout.addSpacing(20)

        oVectorTableLayout = QVBoxLayout()
        self.oVectorTable = QTableWidget(1, 5)
        self.oVectorTable.resizeColumnsToContents()
        self.oVectorTable.verticalHeader().setVisible(False)
        self.oVectorTable.horizontalHeader().setVisible(False)
        self.oVectorTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        self.cmbVectorTableTimeUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 0, "", "Tempo", "t", "")
        self.cmbVectorTableTotalWUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 1, "Fluido complementar + água + hidrocarbonetos (líquido e gás)", "Vazão total", "W", "")
        self.cmbVectorTableGasWUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 2, "Somente quando optar por não seguir relações termodinâmicas", "Vazão de gás", "W", "")
        self.cmbVectorTableComplementarWUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 3, "Inerte, não dissolve gás. Exemplos: fluido de completação, inibidores(etanol, glicol), redutor de atrito, água livre (parcela não-emulsionada), etc.", "Vazão de fluido", "W", "complementar")
        self.cmbVectorTableTUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 4, "", "Temperatura", "T", "")
        self.oVectorTable.resizeColumnsToContents()

        oVectorTableLayout.addWidget(self.oVectorTable)

        oCustomWidgetLayout.addLayout(oVectorTableLayout)

        oVectorTableButtonsLayout = QHBoxLayout()
        btnAddVectorTableRow = QPushButton("+", clicked = self.AddRowToProdMassSourceVectorTable)
        btnRemoveVectorTableRow = QPushButton("-", clicked = self.RemoveRowFromProdMassSourceVectorTable)
        oVectorTableButtonsLayout.addWidget(btnAddVectorTableRow)
        oVectorTableButtonsLayout.addWidget(btnRemoveVectorTableRow)
        oCustomWidgetLayout.addLayout(oVectorTableButtonsLayout)

        oCustomWidget.setLayout(oCustomWidgetLayout)

        return oCustomWidget

    # ========================== Eventos:
    def RemoveRowFromProdMassSourceVectorTable(self):
        selected_ranges = self.oVectorTable.selectedRanges()

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
            self.oVectorTable.removeRow(row)

    def AddRowToProdMassSourceVectorTable(self):
        row_count = self.oVectorTable.rowCount()
        self.oVectorTable.insertRow(row_count)

    def StateChangedInCheckBoxOfEntranceCondition(self, state):
        bDisabled = state == 2
        self.txtMeasuredLength.setEnabled(not bDisabled)
        self.cmbMeasuredLengthUnit.setEnabled(not bDisabled)

    # ========================= Obtenção do objeto correspondente a este Widget:
    def GetProdMassSourceObject(self, iId: int) -> ProdMassSourceObject:

        # Índice do fluido:
        iFluidIndex = int(self.txtFluidId.text())

        # Tipo "termo":
        iTipoTermo = -1
        if self.rdgTipoTermoZero.isChecked():
            iTipoTermo = 0
        elif self.rdgTipoTermoUm.isChecked():
            iTipoTermo = 1

        # Vetores:
        tempo = []
        vazaoTotal = []
        vazaoGas = []
        vazaoComplementar = []
        temperatura = []

        for row in range(1, self.oVectorTable.rowCount()):

            tempo_item = self.oVectorTable.item(row, 0)
            vazaoTotal_item = self.oVectorTable.item(row, 1)
            vazaoGas_item = self.oVectorTable.item(row, 2)
            vazaoComplementar_item = self.oVectorTable.item(row, 3)
            temperatura_item = self.oVectorTable.item(row, 4)            

            if tempo_item:                
                tempo_value = self.oGUIReference.ConvertUnit(float(tempo_item.text()), "t", self.cmbVectorTableTimeUnit, "s")                
                tempo.append(tempo_value)
            else:
                tempo.append(-100)

            if vazaoTotal_item:
                vazaoTotal_value = self.oGUIReference.ConvertUnit(float(vazaoTotal_item.text()), "W", self.cmbVectorTableTotalWUnit, "kg/s")
                vazaoTotal.append(vazaoTotal_value)
            else:
                vazaoTotal.append(-100)

            if vazaoGas_item:
                vazaoGas_value = self.oGUIReference.ConvertUnit(float(vazaoGas_item.text()), "W", self.cmbVectorTableGasWUnit, "kg/s")
                vazaoGas.append(vazaoGas_value)
            else:
                vazaoGas.append(-100)

            if vazaoComplementar_item:
                vazaoComplementar_value = self.oGUIReference.ConvertUnit(float(vazaoComplementar_item.text()), "W", self.cmbVectorTableComplementarWUnit, "kg/s")
                vazaoComplementar.append(vazaoComplementar_value)
            else:
                vazaoComplementar.append(-100)

            if temperatura_item:
                temperatura_value = self.oGUIReference.ConvertUnit(float(temperatura_item.text()), "T", self.cmbVectorTableTUnit, "C")
                temperatura.append(temperatura_value)
            else:
                temperatura.append(-100)

        # Comprimento Medido:
        bProdMassSourceIsCC = self.chkSourceInEntryOfLineProd.isChecked()
        if bProdMassSourceIsCC:
            flowEditConversionObject = self.oGUIReference.oFlowEditConversionObject
            oFirstColProdPipe = flowEditConversionObject.oColProdPipeObjects[0]
            dFirstCellLength = oFirstColProdPipe.dFirstCellLength
            dMeasuredLength = dFirstCellLength / 2.0
        else:
            dMeasuredLength = self.oGUIReference.ConvertUnit(float(self.txtMeasuredLength.text()), "L", self.cmbMeasuredLengthUnit, "m")

        oProdMassSourceObject = ProdMassSourceObject(iId, dMeasuredLength, iTipoTermo, temperatura, vazaoGas, vazaoTotal, vazaoComplementar, iFluidIndex, tempo)
        return oProdMassSourceObject
        # PAREI AQUI EM 23-AGO-2023