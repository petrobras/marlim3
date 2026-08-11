from typing import List
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLineEdit, QHBoxLayout, QCheckBox, QLabel, QComboBox, QRadioButton, QTableWidget, QHeaderView, QPushButton
from IPRObject import IPRObject    




class IPRCustomWidget:
    def __init__(self, oGUIReference):
        self.oGUIReference = oGUIReference

        # Criar o widget
        self.oCreatedCustomWidget = self.CreateIPRCustomWidget()

    # Método para criar o widget
    def CreateIPRCustomWidget(self) -> QWidget:

        oCustomWidget = QWidget()
        oCustomWidgetLayout = QVBoxLayout()
        
        measuredLengthLayout = QHBoxLayout()
        self.chkIPRInEntryOfLineProd = QCheckBox("Condição de contorno na entrada")
        self.chkIPRInEntryOfLineProd.stateChanged.connect(self.StateChangedInCheckBoxOfEntranceCondition)
        measuredLengthLayout.addWidget(self.chkIPRInEntryOfLineProd)
        #measuredLengthLayout.addSpacing(50)
        measuredLengthLayout.addStretch()
        measuredLengthLabel = QLabel("Comprimento medido: ")
        measuredLengthLayout.addWidget(measuredLengthLabel)
        self.txtMeasuredLength = QLineEdit()
        self.txtMeasuredLength.setFixedWidth(100)
        measuredLengthLayout.addWidget(self.txtMeasuredLength)
        self.cmbMeasuredLengthUnit = QComboBox()
        self.oGUIReference.ConfigUnitComboBox(self.cmbMeasuredLengthUnit, "L")
        self.cmbMeasuredLengthUnit.setFixedWidth(50)
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
        #oFluidIdLayout.addSpacing(100)
        lblTipoIPR = QLabel("Modelo da IPR: ")
        oFluidIdLayout.addWidget(lblTipoIPR)
        self.cmbTipoIPR = QComboBox()
        self.cmbTipoIPR.addItems(["Linear", "Vogel combinada", "Vogel pura"])
        self.cmbTipoIPR.model().sort(0)
        self.cmbTipoIPR.setEditable(False)
        oFluidIdLayout.addWidget(self.cmbTipoIPR)

        oCustomWidgetLayout.addLayout(oFluidIdLayout)

        oCustomWidgetLayout.addSpacing(20)

        
        oVectorTableLayout = QVBoxLayout()
        self.oVectorTable = QTableWidget(1, 6)
        self.oVectorTable.resizeColumnsToContents()
        self.oVectorTable.verticalHeader().setVisible(False)
        self.oVectorTable.horizontalHeader().setVisible(False)
        #self.oVectorTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        self.cmbVectorTableTimeUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 0, "", "Tempo", "t", "")
        self.cmbVectorTableStaticPressureUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 1, "", "Pressão estática", "P", "")
        self.cmbVectorTableTemperatureUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 2, "", "Temperatura", "T", "")
        
        
        #cmbDummyVectorTableIIUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 3, "Índice de Injetividade. Quando não for relevante, apenas repetir o IP.", "         II", "", "[ (Sm3/d) / (Sm3/d) ]")
        self.cmbVectorTableIIUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 3, "Índice de Injetividade. Quando não for relevante, apenas repetir o IP.", "         II", "QOverP", "")
        
        
        #cmbDummyVectorTableIPTUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 4, "Índice de Produtividade - dispensável para Vogel pura.", "         IP", "", "[ (Sm3/d) / (Sm3/d) ]")
        self.cmbVectorTableIPUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 4, "Índice de Produtividade - dispensável para Vogel pura.", "         IP", "QOverP", "")
        
        
        self.cmbVectorTableQMAXUnit = self.oGUIReference.addCustomUnitHeaderWidgetToTable(self.oVectorTable, 0, 5, "Somente para IPR Vogel pura", "Vazão máxima", "Q", "")
        
        # Ajustando o tamanho das colunas:
        #self.oVectorTable.resizeColumnsToContents()
        #self.oVectorTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.oVectorTable.horizontalHeader().setSectionResizeMode(3, QHeaderView.ResizeToContents)
        self.oVectorTable.horizontalHeader().setSectionResizeMode(4, QHeaderView.ResizeToContents)
        for col in range(self.oVectorTable.columnCount()):
            if col not in [3, 4]:
                #self.oVectorTable.horizontalHeader().setSectionResizeMode(col, QHeaderView.Stretch)
                self.oVectorTable.horizontalHeader().setSectionResizeMode(col, QHeaderView.ResizeToContents)

        oVectorTableLayout.addWidget(self.oVectorTable)

        oCustomWidgetLayout.addLayout(oVectorTableLayout)

        oVectorTableButtonsLayout = QHBoxLayout()
        btnAddVectorTableRow = QPushButton("+", clicked = self.AddRowToIPRVectorTable)
        btnRemoveVectorTableRow = QPushButton("-", clicked = self.RemoveRowFromIPRVectorTable)
        oVectorTableButtonsLayout.addWidget(btnAddVectorTableRow)
        oVectorTableButtonsLayout.addWidget(btnRemoveVectorTableRow)
        oCustomWidgetLayout.addLayout(oVectorTableButtonsLayout)

        oCustomWidget.setLayout(oCustomWidgetLayout)

        return oCustomWidget

    # ========================== Eventos:
    def RemoveRowFromIPRVectorTable(self):
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

    def AddRowToIPRVectorTable(self):
        row_count = self.oVectorTable.rowCount()
        self.oVectorTable.insertRow(row_count)

    def StateChangedInCheckBoxOfEntranceCondition(self, state):
        bDisabled = state == 2
        self.txtMeasuredLength.setEnabled(not bDisabled)
        self.cmbMeasuredLengthUnit.setEnabled(not bDisabled)

    # 25-AGO-2023: Até esta linha, toda a classe já foi readaptada e conferida para o caso das IPRs!!

    # ========================= Obtenção do objeto correspondente a este Widget:
    def GetIPRObject(self, iId: int) -> IPRObject:

        # Índice do fluido:
        iFluidIndex = int(self.txtFluidId.text())

        # Tipo da IPR:
        txtIPR = self.cmbTipoIPR.currentText()

        if txtIPR == "Linear":
            iTipoIPR = 0
        elif txtIPR == "Vogel combinada":
            iTipoIPR = 1
        elif txtIPR == "Vogel pura":
            iTipoIPR = 2

        # Vetores:
        tempo = []
        pressaoEstatica = []
        temperatura = []
        indInjet = []
        indProd = []
        qMax = []
        
        for row in range(1, self.oVectorTable.rowCount()):

            tempo_item = self.oVectorTable.item(row, 0)
            pressaoEstatica_item = self.oVectorTable.item(row, 1)
            temperatura_item = self.oVectorTable.item(row, 2)
            indInjet_item = self.oVectorTable.item(row, 3)
            indProd_item = self.oVectorTable.item(row, 4)
            qMax_item = self.oVectorTable.item(row, 5)

            if tempo_item:                
                tempo_value = self.oGUIReference.ConvertUnit(float(tempo_item.text()), "t", self.cmbVectorTableTimeUnit, "s")                
                tempo.append(tempo_value)
            else:
                tempo.append(-100)


            if pressaoEstatica_item:
                pressaoEstatica_value = self.oGUIReference.ConvertUnit(float(pressaoEstatica_item.text()), "P", self.cmbVectorTableStaticPressureUnit, "kgf/cm2")
                pressaoEstatica.append(pressaoEstatica_value)
            else:
                pressaoEstatica.append(-100)


            if temperatura_item:
                temperatura_value = self.oGUIReference.ConvertUnit(float(temperatura_item.text()), "T", self.cmbVectorTableTemperatureUnit, "C")
                temperatura.append(temperatura_value)
            else:
                temperatura.append(-100)


            if indInjet_item:
                #indInjet_value = float(indInjet_item.text())
                indInjet_value = self.oGUIReference.ConvertUnit(float(indInjet_item.text()), "QOverP", self.cmbVectorTableIIUnit, "std m3 / d / (kgf/cm2)")
                indInjet.append(indInjet_value)
            else:
                indInjet.append(-100)


            if indProd_item:
                #indProd_value = float(indProd_item.text())
                indProd_value = self.oGUIReference.ConvertUnit(float(indProd_item.text()), "QOverP", self.cmbVectorTableIPUnit, "std m3 / d / (kgf/cm2)")
                indProd.append(indProd_value)
            else:
                indProd.append(-100)


            if qMax_item:
                qMax_value = self.oGUIReference.ConvertUnit(float(qMax_item.text()), "Q", self.cmbVectorTableQMAXUnit, "std m3 / d")
                qMax.append(qMax_value)
            else:
                qMax.append(-100)


        # Comprimento Medido:
        bIPRIsCC = self.chkIPRInEntryOfLineProd.isChecked()
        if bIPRIsCC:
            flowEditConversionObject = self.oGUIReference.oFlowEditConversionObject
            oFirstColProdPipe = flowEditConversionObject.oColProdPipeObjects[0]
            dFirstCellLength = oFirstColProdPipe.dFirstCellLength
            dMeasuredLength = dFirstCellLength / 2.0
        else:
            dMeasuredLength = self.oGUIReference.ConvertUnit(float(self.txtMeasuredLength.text()), "L", self.cmbMeasuredLengthUnit, "m")

        # PAREI AQUI EM 25-AGO-2023
        # Neste ponto, já foram obtidos todos os valores necessários para o preenchimento da IPR
        # Agora é só ajeitar a criação do objeto abaixo...
        oIPRObject = IPRObject(iId, dMeasuredLength, iTipoIPR, temperatura, pressaoEstatica, indProd, indInjet, iFluidIndex, tempo, qMax)
        return oIPRObject