from typing import List
from PyQt5.QtWidgets import QApplication, QTableWidgetItem, QWidget, QVBoxLayout, QLineEdit, QHBoxLayout, QCheckBox, QLabel, QComboBox, QRadioButton, QTableWidget, QHeaderView, QPushButton
from enum import Enum    
from PyQt5.QtCore import Qt

class TypeDuct(Enum):
    Producao = "Producao"
    Servico = "Servico"

# Classe para definir funcionalidades comuns a Widgets onde o usuário selecionará variáveis de saída:
class VariableSelectionBasedCustomWidget:
    def __init__(self, oGUIReference, eTypeDuct):
        self.oGUIReference = oGUIReference
        self.eTypeDuct = eTypeDuct


    # Método para permitir recuperar as chaves de todas as variáveis selecionadas pelo usuário:
    def GetUserCheckedVariableKeys(self):
        checked_variable_keys = []

        for row in range(self.oVariableSelectionTable.rowCount()):
            checkbox_item = self.oVariableSelectionTable.cellWidget(row, 1)
            if isinstance(checkbox_item, QCheckBox) and checkbox_item.isChecked():
                key_item = self.oVariableSelectionTable.item(row, 0)
                if key_item:
                    checked_variable_keys.append(key_item.text())

        return checked_variable_keys

    # Método para permitir a uma check-Box filtrar apenas as variáveis selecionadas:
    def FilterSelectedCheckBoxChanged(self, state):
        if state == Qt.Checked:
            for row in range(self.oVariableSelectionTable.rowCount()):
                item = self.oVariableSelectionTable.item(row, 2)
                checkbox = self.oVariableSelectionTable.cellWidget(row, 1)

                if item is not None and item.text() and checkbox.isChecked():
                    self.oVariableSelectionTable.setRowHidden(row, False)
                else:
                    self.oVariableSelectionTable.setRowHidden(row, True)
        else:
            for row in range(self.oVariableSelectionTable.rowCount()):
                item = self.oVariableSelectionTable.item(row, 2)
                if item is None or not item.text():
                    self.oVariableSelectionTable.setRowHidden(row, True)
                else:
                    self.oVariableSelectionTable.setRowHidden(row, False)


    # Método para criar uma tabela que permita ao usuário selecionar as variáveis pertinentes:
    def GenerateVariableSelectionTableWidget(self, oSelectableVariables, oDefaultSelectedVariables):
        table = QTableWidget()
        table.setColumnCount(3)
        table.setHorizontalHeaderLabels(["Chave", "", "Variável"])
        table.verticalHeader().setVisible(False)
        table.horizontalHeader().setVisible(False)

        for key, value in oSelectableVariables.items():
            row_position = table.rowCount()
            table.insertRow(row_position)

            checkbox = QCheckBox()
            checkbox.setChecked(key in oDefaultSelectedVariables)

            table.setItem(row_position, 0, QTableWidgetItem(key))
            table.setCellWidget(row_position, 1, checkbox)

            var_item = QTableWidgetItem(value)
            table.setItem(row_position, 2, var_item)
            var_item.setToolTip(value)
            #table.setItem(row_position, 2, QTableWidgetItem(value))

            table.item(row_position, 0).setFlags(table.item(row_position, 0).flags() & ~Qt.ItemIsEditable)
            table.item(row_position, 2).setFlags(table.item(row_position, 2).flags() & ~Qt.ItemIsEditable)

            #table.setItem(row_position, 2, QTableWidgetItem(value))
            #table.item(row_position, 0).setFlags(table.item(row_position, 0).flags() ^ 2)
            #table.item(row_position, 2).setFlags(table.item(row_position, 2).flags() ^ 2)
        
        for row in range(table.rowCount()):
            item = table.item(row, 2)
            if item is None or not item.text():
                table.setRowHidden(row, True)


        table.setColumnHidden(0, True)
        table.resizeColumnsToContents()
        #table.setColumnWidth(2, 100)

        return table



