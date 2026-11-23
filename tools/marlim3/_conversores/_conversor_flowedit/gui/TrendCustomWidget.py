from typing import List, Dict, Any
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QGroupBox, QTableWidgetItem, QStackedWidget, QWidget, QVBoxLayout, QLineEdit, QHBoxLayout, QCheckBox, QLabel, QComboBox, QRadioButton, QTableWidget, QHeaderView, QPushButton
from TrendDetailsCustomWidget import TrendDetailsCustomWidget

# Classe para criar um controle GUI customizado para a solicitação de tendências ao Marlim:
class TrendCustomWidget:
    def __init__(self, oGUIReference, eTypeDuct):
        self.oGUIReference = oGUIReference
        self.eTypeDuct = eTypeDuct

        self.oCreatedCustomWidget = self.CreateCustomWidget()

    # Método para criar o widget de edição deste perfil:
    def CreateCustomWidget(self) -> QWidget:

        # Inicializando Dicionário em branco:
        self.oTendsDict = {}

        oCustomWidget = QWidget()
        oCustomWidgetLayout = QHBoxLayout()

        oTendListPortionLayout = QVBoxLayout()

        self.TendListTable = QTableWidget(0, 2)
        self.TendListTable.setHorizontalHeaderLabels(["Ind", "Tendência"])
        self.TendListTable.resizeColumnsToContents()
        #self.TendListTable.setMaximumWidth(self.TendListTable.columnWidth(1)+3)
        self.TendListTable.verticalHeader().setVisible(False)
        self.TendListTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.TendListTable.itemSelectionChanged.connect(self.ItemSelectionChangedInTendsTable)
        self.TendListTable.setColumnHidden(0, True)
        oTendListPortionLayout.addWidget(self.TendListTable, alignment=Qt.AlignHCenter)

        oTendListButtonsLayout = QHBoxLayout()
        btnAddTend = QPushButton("+", clicked = self.PressedButtonAddTend)
        btnAddTend.setMaximumWidth(30)
        btnRemoveTend = QPushButton("-", clicked = self.PressedButtonRemoveTend)
        btnRemoveTend.setMaximumWidth(30)
        oTendListButtonsLayout.addWidget(btnAddTend)
        oTendListButtonsLayout.addWidget(btnRemoveTend)
        oTendListPortionLayout.addLayout(oTendListButtonsLayout)

        oCustomWidgetLayout.addLayout(oTendListPortionLayout)

        grbTendDetails = QGroupBox("Detalhes")
        oCustomWidgetLayout.addWidget(grbTendDetails)
        oTendDetailsPortionLayout = QVBoxLayout()
        self.TendStackedDetails = QStackedWidget()
        oTendDetailsPortionLayout.addWidget(self.TendStackedDetails)

        grbTendDetails.setLayout(oTendDetailsPortionLayout)
        #oCustomWidgetLayout.addLayout(oTendDetailsPortionLayout)

        oCustomWidget.setLayout(oCustomWidgetLayout)
        return oCustomWidget


    # Método para escrever todas as tendências no json do Marlim 3:
    def WriteTrendObjectsToJson(self, oTrendCollectionNode: Dict[str, Any]):

        iWrittenTrends = 0
        for row in range(self.TendListTable.rowCount()):
            item_key = self.TendListTable.item(row, 0)
            item_rotulo = self.TendListTable.item(row, 1)
    
            if item_key and item_rotulo:
                iKey = int(item_key.text())
                sRotulo = item_rotulo.text()
                self.oTendsDict[iKey].GetTrendObject(iWrittenTrends, sRotulo).WriteObjectToJsonFile(oTrendCollectionNode)
                iWrittenTrends = iWrittenTrends + 1

    # Pressionamento do botão de adicionar tendência:
    def PressedButtonAddTend(self):
        row_count = self.TendListTable.rowCount()

        new_key = len(self.oTendsDict)
        if new_key > 0:
            new_key = max(self.oTendsDict.keys()) + 1

        self.TendListTable.insertRow(row_count)
        self.TendListTable.setItem(row_count, 0, QTableWidgetItem(str(new_key)))
        self.TendListTable.setItem(row_count, 1, QTableWidgetItem("Tendência " + str(new_key)))

        self.oTendsDict[new_key] = TrendDetailsCustomWidget(self.oGUIReference, self.eTypeDuct)
        self.TendStackedDetails.addWidget(self.oTendsDict[new_key].oCreatedCustomWidget)

        self.TendListTable.setCurrentCell(row_count, 1)


    # Pressionamento do botão de remover tendência:
    def PressedButtonRemoveTend(self):
        selected_ranges = self.TendListTable.selectedRanges()

        if not selected_ranges:
            return

        response = QMessageBox.question(self.oGUIReference, "Remover tendências", "Remover as tendências selecionadas?", 
                                        QMessageBox.Ok | QMessageBox.Cancel)

        if not (response == QMessageBox.Ok):
            return

        rows_to_remove = set()

        for selected_range in selected_ranges:
            top_row = selected_range.topRow()
            bottom_row = selected_range.bottomRow()

            for row in range(top_row, bottom_row + 1):
                rows_to_remove.add(row)

        rows_to_remove = sorted(rows_to_remove, reverse=True)

        for row in rows_to_remove:

            iKeyItem = self.TendListTable.item(row, 0)
            if iKeyItem:
                widgetKey = int(iKeyItem.text())
                self.TendStackedDetails.removeWidget(self.oTendsDict[widgetKey].oCreatedCustomWidget)
                del self.oTendsDict[widgetKey]

            self.TendListTable.removeRow(row)

        row_count = self.TendListTable.rowCount()
        if row_count > 0:
            self.TendListTable.setCurrentCell(row_count - 1, 1)



    # Mudança de seleção na tabela de tendências:
    def ItemSelectionChangedInTendsTable(self):
        selected_items = self.TendListTable.selectedItems()

        if not selected_items:
            return

        row = selected_items[0].row()

        iKeyItem = self.TendListTable.item(row, 0)
        if iKeyItem:
            widgetKey = int(iKeyItem.text())
            self.TendStackedDetails.setCurrentWidget(self.oTendsDict[widgetKey].oCreatedCustomWidget)