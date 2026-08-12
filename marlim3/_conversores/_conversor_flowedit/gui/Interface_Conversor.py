import sys
import os
import json
from tkinter import CURRENT

# Garante que o pacote 'conversor/' seja encontrado mesmo ao executar este
# script diretamente a partir da pasta 'gui/', sem precisar unificar pastas.
_GUI_DIR = os.path.dirname(os.path.abspath(__file__))
_FLOWEDIT_DIR = os.path.normpath(os.path.join(_GUI_DIR, ".."))
if _FLOWEDIT_DIR not in sys.path:
    sys.path.insert(0, _FLOWEDIT_DIR)
# Também adiciona o próprio diretório gui/ para os módulos locais da interface.
if _GUI_DIR not in sys.path:
    sys.path.insert(0, _GUI_DIR)

from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QLabel, QLineEdit, QPushButton, QFileDialog, QMessageBox, QTabWidget, QVBoxLayout, QHBoxLayout, QButtonGroup, QRadioButton, QCheckBox, QTableWidget, QTableWidgetItem, QAbstractItemView, QGroupBox, QComboBox, QHeaderView, QStackedWidget
from PyQt5.QtCore import Qt
from conversor.FloweditConversion import FloweditConversion
from conversor.FloweditConversionInputInfo import FloweditConversionInputInfo
from conversor.VGLObject import VGLObject
from ProductionFluidObject import ProductionFluidObject
from GasFluidObject import GasFluidObject
from GasInjObject import GasInjObject
from SeparadorObject import SeparadorObject
from ChokeSupObject import ChokeSupObject
from ProdMassSourceCustomWidget import ProdMassSourceCustomWidget
from IPRCustomWidget import IPRCustomWidget
from TimeConfigObject import TimeConfigObject
from ProfileCustomWidget import ProfileCustomWidget
from VariableSelectionBasedCustomWidget import TypeDuct
from TrendCustomWidget import TrendCustomWidget
from typing import List

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Conversor Flowedit —> MARLIM 3")
        #self.setFixedSize(800, 600)  # Tamanho fixo para a janela
        #self.setFixedSize(880, 630)
        self.setFixedSize(970, 630)

        # Remover o ícone:
        #self.setWindowFlags(self.windowFlags() & ~Qt.WindowTitleHint)
        #self.setWindowFlags(Qt.CustomizeWindowHint | Qt.WindowMinimizeButtonHint | Qt.WindowCloseButtonHint)
        #self.setWindowFlags(Qt.WindowTitleHint | Qt.WindowSystemMenuHint)
        #self.setWindowFlags(self.windowFlags() | Qt.CustomizeWindowHint | Qt.WindowMinimizeButtonHint)

        # Tab Container
        tab_widget = QTabWidget()
        self.setCentralWidget(tab_widget)
        #tab_widget.tabBar().setExpanding(True)

        # Primeira Tab - Flowedit
        flowedit_tab = QWidget()
        tab_widget.addTab(flowedit_tab, "Flowedit")

        # Arquivo Flowedit:
        flowedit_layout = QHBoxLayout()
        label_flowedit = QLabel("Arquivo Flowedit:")
        self.entry_flowedit = QLineEdit()
        button_flowedit = QPushButton("...", clicked=self.browse_file_1)
        flowedit_layout.addWidget(label_flowedit)
        flowedit_layout.addWidget(self.entry_flowedit)
        flowedit_layout.addWidget(button_flowedit)

        # Arquivo Json:
        json_layout = QHBoxLayout()
        label_json = QLabel("Arquivo Json:")
        self.entry_json = QLineEdit()
        button_json = QPushButton("...", clicked=self.browse_file_2)
        json_layout.addWidget(label_json)
        json_layout.addWidget(self.entry_json)
        json_layout.addWidget(button_json)

        # Layout vertical para a tab Flowedit:
        flowedit_tab_layout = QVBoxLayout()
        flowedit_tab_layout.addLayout(flowedit_layout)
        flowedit_tab_layout.addLayout(json_layout)
        flowedit_tab.setLayout(flowedit_tab_layout)

        # Segunda Tab - Fluido
        fluido_tab = QWidget()
        tab_widget.addTab(fluido_tab, "Modelo de Fluido")

        # ======================================== fluido
        rdg_fluido_layout = QVBoxLayout()
        self.rdg_fluido = QButtonGroup()

        rdg_fluid_not_inform = QRadioButton("Não preencher no JSON")
        rdg_fluido_layout.addWidget(rdg_fluid_not_inform)
        self.rdg_fluido.addButton(rdg_fluid_not_inform)
        rdg_fluid_not_inform.setChecked(True)
        self.rdg_fluid_not_inform = rdg_fluid_not_inform    # adicionado posteriormente

        rdg_fluid_black_oil = QRadioButton("Black-oil")
        rdg_fluido_layout.addWidget(rdg_fluid_black_oil)
        self.rdg_fluido.addButton(rdg_fluid_black_oil)
        self.rdg_fluid_black_oil = rdg_fluid_black_oil

        # Layout for TextBox and Button for rdg_fluid_inform_tab
        fluid_tab_layout = QHBoxLayout()
        rdg_fluid_inform_tab = QRadioButton("Tabela *.tab")
        fluid_tab_layout.addWidget(rdg_fluid_inform_tab)
        self.rdg_fluido.addButton(rdg_fluid_inform_tab)
        self.rdg_fluid_inform_tab = rdg_fluid_inform_tab      # adicionado posteriormente

        self.txtFluidTabFile = QLineEdit()
        self.txtFluidTabFile.setEnabled(False)
        fluid_tab_layout.addWidget(self.txtFluidTabFile)

        self.button_browse_fluid_tab = QPushButton("...", clicked=self.browse_fluid_tab)
        self.button_browse_fluid_tab.setEnabled(False)
        fluid_tab_layout.addWidget(self.button_browse_fluid_tab)

        rdg_fluido_layout.addLayout(fluid_tab_layout)


        fluid_ctm_layout = QHBoxLayout()
        rdg_fluid_compositional_ctm = QRadioButton("Composicional (*.ctm)")
        fluid_ctm_layout.addWidget(rdg_fluid_compositional_ctm)
        self.rdg_fluido.addButton(rdg_fluid_compositional_ctm)
        self.rdg_fluid_compositional_ctm = rdg_fluid_compositional_ctm       # adicionado posteriormente

        self.txtFluidCTMFile = QLineEdit()
        self.txtFluidCTMFile.setEnabled(False)
        fluid_ctm_layout.addWidget(self.txtFluidCTMFile)

        self.button_browse_fluid_ctm = QPushButton("...", clicked=self.browse_fluid_ctm)
        self.button_browse_fluid_ctm.setEnabled(False)
        fluid_ctm_layout.addWidget(self.button_browse_fluid_ctm)

        rdg_fluido_layout.addLayout(fluid_ctm_layout)

        fluido_tab.setLayout(rdg_fluido_layout)

        rdg_fluid_inform_tab.toggled.connect(self.toggle_fluid_tab)
        rdg_fluid_compositional_ctm.toggled.connect(self.toggle_fluid_ctm)
        # ======================================== fluido

    # Terceira Tab - Contorno Produção [Chegada]
        ccProductionEnd_tab = QWidget()
        tab_widget.addTab(ccProductionEnd_tab, "Contorno Produção [Chegada]")

        ccProductionEnd_layout = QVBoxLayout()

        ccProductionEnd_topLayout = QVBoxLayout()
        self.chkCCProductionEndWriteToJson = QCheckBox("Preencher no json")
        ccProductionEnd_topLayout.addWidget(self.chkCCProductionEndWriteToJson)

        ccProductionEnd_layout.addLayout(ccProductionEnd_topLayout)
        #ccProductionEnd_layout.addStretch(1)
        ccProductionEnd_layout.addSpacing(100)

        ccProductionEnd_MiddleLayout = QHBoxLayout()

        ccProductionEnd_grbPressure = QGroupBox("Pressão no separador (chegada)")
        ccProductionEnd_MiddleLayout.addWidget(ccProductionEnd_grbPressure)

        ccProductionEnd_grbPressureInnerLayout = QVBoxLayout()

        self.ccProductionEndPressureTable = QTableWidget(1, 2)
        self.ccProductionEndPressureTable.verticalHeader().setVisible(False)
        self.ccProductionEndPressureTable.horizontalHeader().setVisible(False)
        self.ccProductionEndPressureTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        
        iEndPressureHeaderRow = 0

        self.cmbEndPressureTimeUnit = self.addCustomUnitHeaderWidgetToTable(self.ccProductionEndPressureTable, iEndPressureHeaderRow, 0, "", "             Tempo", "t", "")
        self.cmbEndPressureUnit = self.addCustomUnitHeaderWidgetToTable(self.ccProductionEndPressureTable, iEndPressureHeaderRow, 1, "", "            Pressão", "P", "")

        ccProductionEnd_grbPressureInnerLayout.addWidget(self.ccProductionEndPressureTable)

        ccProductionEnd_PButtonsLayout = QHBoxLayout()
        ccProductionEnd_btnAddPressure = QPushButton("+", clicked=self.AddRowToProdEndPressureTable)
        ccProductionEnd_btnRemovePressure = QPushButton("-", clicked=self.RemoveRowFromProdEndPressureTable)
        ccProductionEnd_PButtonsLayout.addWidget(ccProductionEnd_btnAddPressure)
        ccProductionEnd_PButtonsLayout.addWidget(ccProductionEnd_btnRemovePressure)
        ccProductionEnd_grbPressureInnerLayout.addLayout(ccProductionEnd_PButtonsLayout)

        ccProductionEnd_grbPressure.setLayout(ccProductionEnd_grbPressureInnerLayout)

        ccProductionEnd_grbChokeSup = QGroupBox("Choke de superfície")
        ccProductionEnd_MiddleLayout.addWidget(ccProductionEnd_grbChokeSup)

        ccProductionEnd_grbChokeSupInnerLayout = QVBoxLayout()

        self.chkProductionEndActiveChokeSup = QCheckBox("Incluir na simulação")
        ccProductionEnd_grbChokeSupInnerLayout.addWidget(self.chkProductionEndActiveChokeSup)
        self.chkProductionEndActiveChokeSup.stateChanged.connect(self.IncludeChokeSupStateChanged)

        ccProductionEnd_grbChokeSupInnerLayout.addSpacing(25)

        ccProductionEnd_chokeSupCdLayout = QHBoxLayout()
        self.CCProductionEnd_txtChokeSupCd = QLineEdit()
        ccProductionEnd_chokeSupCdLayout.addWidget(QLabel("Coeficiente de descarga: "))
        ccProductionEnd_chokeSupCdLayout.addWidget(self.CCProductionEnd_txtChokeSupCd)
        ccProductionEnd_grbChokeSupInnerLayout.addLayout(ccProductionEnd_chokeSupCdLayout)

        ccProductionEnd_grbChokeSupInnerLayout.addSpacing(25)

        self.ccProductionEndChokeSupOpeningTable = QTableWidget(1, 2)
        self.ccProductionEndChokeSupOpeningTable.verticalHeader().setVisible(False)
        self.ccProductionEndChokeSupOpeningTable.horizontalHeader().setVisible(False)
        self.ccProductionEndChokeSupOpeningTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        self.cmbChokeSupOpeningTimeUnit = self.addCustomUnitHeaderWidgetToTable(self.ccProductionEndChokeSupOpeningTable, 0, 0, "", "              Tempo", "t", "")
        cmbDummyChokeSupOpeningUnit = self.addCustomUnitHeaderWidgetToTable(self.ccProductionEndChokeSupOpeningTable, 0, 1, "Relativa à área", "           Fração de abertura", "", "")



        ccProductionEnd_grbChokeSupInnerLayout.addWidget(self.ccProductionEndChokeSupOpeningTable)

        ccProductionEnd_ChokeSupOpeningButtonsLayout = QHBoxLayout()
        ccProductionEnd_btnAddChokeSupOpening = QPushButton("+", clicked=self.AddRowToProdEndChokeOpeningTable)
        ccProductionEnd_btnRemoveChokeSupOpening = QPushButton("-", clicked=self.RemoveRowFromProdEndChokeOpeningTable)
        ccProductionEnd_ChokeSupOpeningButtonsLayout.addWidget(ccProductionEnd_btnAddChokeSupOpening)
        ccProductionEnd_ChokeSupOpeningButtonsLayout.addWidget(ccProductionEnd_btnRemoveChokeSupOpening)
        ccProductionEnd_grbChokeSupInnerLayout.addLayout(ccProductionEnd_ChokeSupOpeningButtonsLayout)

        ccProductionEnd_grbChokeSup.setLayout(ccProductionEnd_grbChokeSupInnerLayout)

        ccProductionEnd_layout.addLayout(ccProductionEnd_MiddleLayout)
        #ccProductionEnd_layout.addStretch(1)

        ccProductionEnd_tab.setLayout(ccProductionEnd_layout)

        self.chkProductionEndActiveChokeSup.setChecked(True)
        self.chkProductionEndActiveChokeSup.setChecked(False)

	# Terceira Tab - Fluidos - Produção
        fluidos_producao_tab = QWidget()
        #tab_widget.insertTab(2, fluidos_producao_tab, "Fluidos - Produção")
        tab_widget.addTab(fluidos_producao_tab, "Fluidos - Produção")

        fluidos_producao_layout = QVBoxLayout()
        top_layout = QVBoxLayout()

        self.checkBox_preencher_json = QCheckBox("Preencher no json")
        top_layout.addWidget(self.checkBox_preencher_json)
        
        fluidos_producao_layout.addLayout(top_layout)
        fluidos_producao_layout.addStretch(1)
        
        
        	# Tabela de fluidos produzidos:
        center_layout = QVBoxLayout()
        
        	# Botões:
        fluidos_prod_buttons_layout = QHBoxLayout()
        add_row_button = QPushButton("+", clicked=self.add_row_to_table)
        fluidos_prod_buttons_layout.addWidget(add_row_button)
        
        remove_row_button = QPushButton("-", clicked=self.remove_row_from_table)
        fluidos_prod_buttons_layout.addWidget(remove_row_button)
        	
        fluidos_producao_layout.addLayout(fluidos_prod_buttons_layout)
        
        #self.table_widget = QTableWidget(self)
        self.table_widget = QTableWidget(0, 4)
        #self.table_widget.setRowCount(5)    
        self.table_widget.setColumnCount(4) 
        self.table_widget.setHorizontalHeaderLabels(["Id", "RGO [Sm3 / Sm3]", "BSW [fração]", "BSW de Inversão [fração]"])
        self.table_widget.resizeColumnsToContents()
        
        self.table_widget.verticalHeader().setVisible(False)  
        self.table_widget.setColumnWidth(0, 100)  
        self.table_widget.setColumnWidth(1, 150)  
        self.table_widget.setColumnWidth(2, 150)  
        #self.table_widget.setEditTriggers(QAbstractItemView.NoEditTriggers)
        
        
        
        
        center_layout.addWidget(self.table_widget)
        fluidos_producao_layout.addLayout(center_layout)
        fluidos_producao_layout.addStretch(1)
        
        fluidos_producao_tab.setLayout(fluidos_producao_layout)

        # Tab - IPRs
        iprTab = QWidget()
        tab_widget.addTab(iprTab, "IPRs")

        iprTabLayout = QVBoxLayout()

        iprTabTopLayout = QVBoxLayout()
        self.chkIPRWriteToJson = QCheckBox("Preencher no json")
        iprTabTopLayout.addWidget(self.chkIPRWriteToJson)
        iprTabLayout.addLayout(iprTabTopLayout)
        iprTabLayout.addSpacing(100)

        iprTabCenterLayout = QHBoxLayout()

        iprTabGrbIPRs = QGroupBox("IPRs")
        iprTabGrbIPRs.setFixedWidth(200)
        iprTabCenterLayout.addWidget(iprTabGrbIPRs)
        iprTabGrbDetails = QGroupBox("Detalhes")
        iprTabCenterLayout.addWidget(iprTabGrbDetails)

            # Inicializando Dicionário em branco:
        self.oIPRsDict = {}

        iprListTableLayout = QVBoxLayout()

        self.IPRListTable = QTableWidget(0, 2)
        self.IPRListTable.setHorizontalHeaderLabels(["Ind", "IPR"])
        self.IPRListTable.resizeColumnsToContents()
        self.IPRListTable.verticalHeader().setVisible(False)
        self.IPRListTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        #self.IPRListTable.cellClicked.connect()
        self.IPRListTable.itemSelectionChanged.connect(self.ItemSelectionChangedInIPRTable)
        iprListTableLayout.addWidget(self.IPRListTable)
        self.IPRListTable.setColumnHidden(0, True)

        iprListTableButtonsLayout = QHBoxLayout()
        btnAddIPR = QPushButton("+", clicked = self.PressedButtonAddIPR)
        btnRemoveIPR = QPushButton("-", clicked = self.PressedButtonRemoveIPR)
        iprListTableButtonsLayout.addWidget(btnAddIPR)
        iprListTableButtonsLayout.addWidget(btnRemoveIPR)
        iprListTableLayout.addLayout(iprListTableButtonsLayout)

        iprTabGrbIPRs.setLayout(iprListTableLayout)

        iprTabDetailsLayout = QVBoxLayout()

        self.iprTabStackedDetails = QStackedWidget()
        iprTabDetailsLayout.addWidget(self.iprTabStackedDetails)

        iprTabGrbDetails.setLayout(iprTabDetailsLayout)

        iprTabLayout.addLayout(iprTabCenterLayout)

        iprTab.setLayout(iprTabLayout)

        # Tab - Fontes de Massa
        prodMassSources_Tab = QWidget()
        tab_widget.addTab(prodMassSources_Tab, "Fontes de Massa [Produção]")

        prodMassSources_layout = QVBoxLayout()

        prodMassSources_topLayout = QVBoxLayout()
        self.chkProdMassSourcesWriteToJson = QCheckBox("Preencher no json")
        prodMassSources_topLayout.addWidget(self.chkProdMassSourcesWriteToJson)
        prodMassSources_layout.addLayout(prodMassSources_topLayout)
        #prodMassSources_layout.addStretch(1)
        prodMassSources_layout.addSpacing(100)

        prodMassSources_centerLayout = QHBoxLayout()

        prodMassSources_grbMassSources = QGroupBox("Fontes de Massa")
        prodMassSources_grbMassSources.setFixedWidth(200)
        prodMassSources_centerLayout.addWidget(prodMassSources_grbMassSources)
        prodMassSources_grbDetails = QGroupBox("Detalhes")
        prodMassSources_centerLayout.addWidget(prodMassSources_grbDetails)

            # Inicializando Dicionário em branco:
        self.oProdMassSourcesDict = {}

        prodMassSources_ListTableLayout = QVBoxLayout()

        self.prodMassSources_ListTable = QTableWidget(0,2)
        self.prodMassSources_ListTable.setHorizontalHeaderLabels(["Ind", "Fonte"])
        self.prodMassSources_ListTable.resizeColumnsToContents()
        self.prodMassSources_ListTable.verticalHeader().setVisible(False)
        self.prodMassSources_ListTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.prodMassSources_ListTable.cellClicked.connect(self.ClickedCellInProdMassSourceTable)
        self.prodMassSources_ListTable.itemSelectionChanged.connect(self.ItemSelectionChangedInProdMassSourceTable)
        prodMassSources_ListTableLayout.addWidget(self.prodMassSources_ListTable)
        self.prodMassSources_ListTable.setColumnHidden(0, True)

        prodMassSources_ListTableButtonsLayout = QHBoxLayout()
        btnAddProdMassSource = QPushButton("+", clicked=self.PressedButtonAddProdMassSource)
        btnRemoveProdMassSource = QPushButton("-", clicked=self.PressedButtonRemoveProdMassSource)
        prodMassSources_ListTableButtonsLayout.addWidget(btnAddProdMassSource)
        prodMassSources_ListTableButtonsLayout.addWidget(btnRemoveProdMassSource)
        prodMassSources_ListTableLayout.addLayout(prodMassSources_ListTableButtonsLayout)

        prodMassSources_grbMassSources.setLayout(prodMassSources_ListTableLayout)

        prodMassSource_DetailsLayout = QVBoxLayout()

        self.prodMassSourceStackedDetails = QStackedWidget()
        prodMassSource_DetailsLayout.addWidget(self.prodMassSourceStackedDetails)

        prodMassSources_grbDetails.setLayout(prodMassSource_DetailsLayout)

        prodMassSources_layout.addLayout(prodMassSources_centerLayout)

        prodMassSources_Tab.setLayout(prodMassSources_layout)

        # Tab - Contorno de Gas-Lift
        contornoGL_tab = QWidget()
        tab_widget.addTab(contornoGL_tab, "Gás-Lift - Contorno")

        contornoGL_layout = QVBoxLayout()
        contornoGL_topLayout = QVBoxLayout()

        self.chkContornoGLWriteToJson = QCheckBox("Preencher no json")
        contornoGL_topLayout.addWidget(self.chkContornoGLWriteToJson)

        contornoGL_layout.addLayout(contornoGL_topLayout)
        contornoGL_layout.addStretch(1)

        contornoGL_centerLayout = QVBoxLayout()
        contornoGL_InitialConfigGrb = QGroupBox("Configuração inicial")
        contornoGL_centerLayout.addWidget(contornoGL_InitialConfigGrb)

        contornoGL_InitialConfigGrbInnerLayout = QHBoxLayout()

        self.chkContornoGLActivateGLFlow = QCheckBox("Ativar escoamento na linha de gas-lift")
        contornoGL_InitialConfigGrbInnerLayout.addWidget(self.chkContornoGLActivateGLFlow)
        contornoGL_InitialConfigGrbInnerLayout.addSpacing(40)

        contornoGL_acopColAnulLayout = QHBoxLayout()

        lblAcopColAnulGL = QLabel("Resolução - Acoplamento térmico Anular-Coluna:")
        lblAcopColAnulGL.setToolTip("USAR DE 0 A 20. 0 para acelerar desempenho, 20 para melhor capturar as variações logarítmicas de temperatura próximo à ANM.")
        lblAcopColAnulGL.setCursor(Qt.PointingHandCursor)
        contornoGL_acopColAnulLayout.addWidget(lblAcopColAnulGL)
        txtAcopColAnulGL = QLineEdit()
        txtAcopColAnulGL.setText("20")
        contornoGL_acopColAnulLayout.addWidget(txtAcopColAnulGL)
        contornoGL_InitialConfigGrbInnerLayout.addLayout(contornoGL_acopColAnulLayout)

        self.txtAcopColAnulGL = txtAcopColAnulGL

        contornoGL_InitialConfigGrb.setLayout(contornoGL_InitialConfigGrbInnerLayout)

        contornoGL_layout.addLayout(contornoGL_centerLayout)
        contornoGL_layout.addStretch(1)

        contornoGL_bottomLayout = QVBoxLayout()
        contornoGL_CCGrb = QGroupBox("Contorno - UEP")
        contornoGL_bottomLayout.addWidget(contornoGL_CCGrb)
        
        contornoGL_CCGrbInnerLayout = QVBoxLayout()

        contornoGL_TipoCCLayout = QHBoxLayout()

        rdgTipoCCVazao = QRadioButton("Vazão de Injeção")
        rdgTipoCCVazao.setChecked(True)
        rdgTipoCCPressaoComQ = QRadioButton("Pressão de Injeção (com vazão estimada)")
        rdgTipoCCPressaoSemQ = QRadioButton("Pressão de Injeção (sem vazão estimada)")

        self.rdgTipoCCVazao = rdgTipoCCVazao
        self.rdgTipoCCPressaoComQ = rdgTipoCCPressaoComQ

        contornoGL_TipoCCLayout.addWidget(rdgTipoCCVazao)
        contornoGL_TipoCCLayout.addWidget(rdgTipoCCPressaoComQ)
        contornoGL_TipoCCLayout.addWidget(rdgTipoCCPressaoSemQ)
        contornoGL_CCGrbInnerLayout.addLayout(contornoGL_TipoCCLayout)

        contornoGL_CCUnitLayout = QHBoxLayout()
        cmbGLCCTimeUnit = QComboBox()
        self.ConfigUnitComboBox(cmbGLCCTimeUnit, "t")
        cmbGLCCTemperatureUnit = QComboBox()
        self.ConfigUnitComboBox(cmbGLCCTemperatureUnit, "T")
        cmbGLCCQUnit = QComboBox()
        self.ConfigUnitComboBox(cmbGLCCQUnit, "Q")
        cmbGLCCPUnit = QComboBox()
        self.ConfigUnitComboBox(cmbGLCCPUnit, "P")

        self.cmbGLCCTemperatureUnit = cmbGLCCTemperatureUnit
        self.cmbGLCCPUnit = cmbGLCCPUnit
        self.cmbGLCCQUnit = cmbGLCCQUnit
        self.cmbGLCCTimeUnit = cmbGLCCTimeUnit

        contornoGL_CCUnitLayout.addWidget(cmbGLCCTimeUnit)
        contornoGL_CCUnitLayout.addWidget(cmbGLCCTemperatureUnit)
        contornoGL_CCUnitLayout.addWidget(cmbGLCCQUnit)
        contornoGL_CCUnitLayout.addWidget(cmbGLCCPUnit)
        contornoGL_CCGrbInnerLayout.addLayout(contornoGL_CCUnitLayout)

        tableGasInj = QTableWidget(0, 4)
        tableGasInj.setHorizontalHeaderLabels(["Tempo", "Temperatura", "Vazão", "Pressão"])
        tableGasInj.resizeColumnsToContents()
        tableGasInj.verticalHeader().setVisible(False)
        tableGasInj.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        contornoGL_CCGrbInnerLayout.addWidget(tableGasInj)
        self.tableGasInj = tableGasInj

        contornoGL_CCGasInjButtonsLayout = QHBoxLayout()
        btnAddGasInjCC = QPushButton("+", clicked = self.addRowToGLCC)
        btnRemoveGasInjCC = QPushButton("-", clicked = self.removeRowFromGLCC)

        contornoGL_CCGasInjButtonsLayout.addWidget(btnAddGasInjCC)
        contornoGL_CCGasInjButtonsLayout.addWidget(btnRemoveGasInjCC)
        contornoGL_CCGrbInnerLayout.addLayout(contornoGL_CCGasInjButtonsLayout)

        contornoGL_CCGrb.setLayout(contornoGL_CCGrbInnerLayout)
        contornoGL_layout.addLayout(contornoGL_bottomLayout)

        contornoGL_tab.setLayout(contornoGL_layout)

        # Quinta Tab - Válvulas de Gas-Lift
        vgl_tab = QWidget()
        tab_widget.addTab(vgl_tab, "Válvulas de Gas-Lift")
        
        vgl_layout = QVBoxLayout()

        vgl_topLayout = QVBoxLayout()
        self.chkVGLWriteToJson = QCheckBox("Preencher no json")
        vgl_topLayout.addWidget(self.chkVGLWriteToJson)
        vgl_layout.addLayout(vgl_topLayout)
        vgl_layout.addStretch(1)

            # Tabela de VGLs
        vgl_centerLayout = QVBoxLayout()
        self.vglTableWidget = QTableWidget(1, 9)
        #self.vglTableWidget.setHorizontalHeaderLabels(["Id", "Comprimento Medido [Produção]", "Comprimento Medido [Serviço]", "Tipo", "Diâmetro do Orifício", "Cv", "Pressão de Calibração", "Temperatura de Calibração", "Razão de Área"])
        self.vglTableWidget.resizeColumnsToContents()
        self.vglTableWidget.verticalHeader().setVisible(False)
        self.vglTableWidget.horizontalHeader().setVisible(False)

        iHeaderRow = 0

        cmbDummyVGLId = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 0, "Todos os tipos de válvula.", "    Id", "", "")
        self.cmbVGLCompProdUnit = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 1, "Todos os tipos de válvula.", "Comprimento Medido", "L", "          [Produção]")
        self.cmbVGLCompServiceUnit = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 2, "Todos os tipos de válvula.", "Comprimento Medido", "L", "           [Serviço]")
        cmbDummyVGLType = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 3, "Todos os tipos de válvula.", "  Tipo", "", "")
        self.cmbVGLOrifDUnit = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 4, "Todos os tipos de válvula.", "Diâmetro do Orifício", "L", "")
        cmbDummyVGLCv = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 5, "Todos os tipos de válvula.", "    Cd", "", "")
        self.cmbVGLPCalibUnit = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 6, "Somente para válvulas de pressão.", "Pressão de Calibração", "P", "")
        self.cmbVGLTCalibUnit = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 7, "Somente para válvulas de pressão.", "Temperatura de Calibração", "T", "")
        cmbDummyVGLAreaRatio = self.addCustomUnitHeaderWidgetToTable(self.vglTableWidget, iHeaderRow, 8, "Somente para válvulas de pressão.", "Razão de Área", "", "")
        self.vglTableWidget.resizeColumnsToContents()




        #pCalib_headerWidget = QWidget()
        #pCalib_headerWidget.setToolTip("Teste tool tip")
        #pCalib_headerLayout = QVBoxLayout()
        #pCalib_headerLabel = QLabel("Pressão de Calibração")
        #self.pCalib_headerComboBox = QComboBox()
        #self.ConfigUnitComboBox(self.pCalib_headerComboBox, "P")
        #pCalib_headerLayout.addWidget(pCalib_headerLabel)
        #pCalib_headerLayout.addWidget(self.pCalib_headerComboBox)
        #pCalib_headerLayout.setContentsMargins(0, 0, 0, 0)
        #pCalib_headerWidget.setLayout(pCalib_headerLayout)
        #self.vglTableWidget.setCellWidget(3, 6, pCalib_headerWidget)
        #self.vglTableWidget.resizeRowToContents(3)

        #self.cmbVGLCompProdUnit = QComboBox()
        #self.ConfigUnitComboBox(self.cmbVGLCompProdUnit, "L")
        #self.vglTableWidget.setCellWidget(0, 1, self.cmbVGLCompProdUnit)

        vgl_centerLayout.addWidget(self.vglTableWidget)
        
        vgl_tableButtonsLayout = QHBoxLayout()
        btnAddVGL = QPushButton("+", clicked = self.addRowToVGLTable)
        btnRemoveVGL = QPushButton("-", clicked = self.removeRowFromVGLTable)
        btnGetVGLFromFlowedit = QPushButton("Importar VGLs da Flowedit", clicked = self.BtnClickImportVGLFromFlowedit)
        vgl_tableButtonsLayout.addWidget(btnAddVGL)
        vgl_tableButtonsLayout.addWidget(btnRemoveVGL)
        vgl_tableButtonsLayout.addWidget(btnGetVGLFromFlowedit)
        vgl_centerLayout.addLayout(vgl_tableButtonsLayout)

        vgl_layout.addLayout(vgl_centerLayout)
        
        vgl_layout.addStretch(1)
        vgl_tab.setLayout(vgl_layout)

        # Sexta Tab - Fluido Gás
        fluido_gas_tab = QWidget()
        #tab_widget.insertTab(3, fluido_gas_tab, "Gás - Linha de Serviço")
        tab_widget.addTab(fluido_gas_tab, "Gás - Linha de Serviço")

        fluido_gas_layout = QVBoxLayout()
        gas_top_layout = QVBoxLayout()

        self.chkFluidGasWriteToJson = QCheckBox("Preencher no json")
        gas_top_layout.addWidget(self.chkFluidGasWriteToJson)

        fluido_gas_layout.addLayout(gas_top_layout)
        fluido_gas_layout.addStretch(1)

        fluidoGas_centerLayout = QVBoxLayout()
        fluidoGas_BlackOilGrb = QGroupBox("Black-oil ou Tabela")
        fluidoGas_centerLayout.addWidget(fluidoGas_BlackOilGrb)

        fluidoGas_BOinnerLayout = QHBoxLayout()

        label_gas_density = QLabel("Densidade do gás:")
        txtFluidGasDensity = QLineEdit()
        self.txtFluidGasDensity = txtFluidGasDensity    # adicionado posteriormente
        fluidoGas_densityLayout = QHBoxLayout()
        fluidoGas_densityLayout.addWidget(label_gas_density)
        fluidoGas_densityLayout.addWidget(txtFluidGasDensity)
        fluidoGas_BOinnerLayout.addLayout(fluidoGas_densityLayout)

        label_gas_co2_fraction = QLabel("Frac. Molar CO2:")
        txtFluidGasCO2Fraction = QLineEdit()
        self.txtFluidGasCO2Fraction = txtFluidGasCO2Fraction
        fluidoGas_co2FractionLayout = QHBoxLayout()
        fluidoGas_co2FractionLayout.addWidget(label_gas_co2_fraction)
        fluidoGas_co2FractionLayout.addWidget(txtFluidGasCO2Fraction)
        fluidoGas_BOinnerLayout.addLayout(fluidoGas_co2FractionLayout)

        fluidoGas_BlackOilGrb.setLayout(fluidoGas_BOinnerLayout)

        fluido_gas_layout.addLayout(fluidoGas_centerLayout)
        fluido_gas_layout.addStretch(1)

        fluido_gas_tab.setLayout(fluido_gas_layout)

        # Tab de Tempo:
        timeTab = QWidget()
        tab_widget.addTab(timeTab, "Tempo")

        timeTabLayout = QVBoxLayout()

        timeTabTopLayout = QVBoxLayout()
        self.chkTimeTabWriteToJson = QCheckBox("Preencher no json")
        timeTabTopLayout.addWidget(self.chkTimeTabWriteToJson)
        timeTabLayout.addLayout(timeTabTopLayout)

        timeTabLayout.addStretch(1)
        timeTabSimulationTypeLayout = QVBoxLayout()
        timeTabSimulationTypeGrb = QGroupBox("Simulação...")
        timeTabSimulationTypeGrb.setFixedWidth(300)
        timeTabSimulationTypeLayout.addWidget(timeTabSimulationTypeGrb)
        grbSimulationTypeInnerLayout = QVBoxLayout()
        self.rdgStationarySimulation = QRadioButton("Permanente")
        self.rdgTransientSimulation = QRadioButton("Transiente")
        self.rdgTransientSimulation.setChecked(True)
        self.rdgTransientSimulation.toggled.connect(self.TransientRadioButtonChangedState)

        simulationTypeRdgBtnLayout = QHBoxLayout()
        simulationTypeRdgBtnLayout.addWidget(self.rdgStationarySimulation)
        simulationTypeRdgBtnLayout.addWidget(self.rdgTransientSimulation)
        grbSimulationTypeInnerLayout.addLayout(simulationTypeRdgBtnLayout)

        grbSimulationTypeInnerLayout.addSpacing(15)

        simulationFinalTimeLayout = QHBoxLayout()
        self.lblSimFinalTime = QLabel("Tempo final: ")
        self.txtSimFinalTime = QLineEdit()
        self.cmbFinalTimeUnit = QComboBox()
        self.ConfigUnitComboBox(self.cmbFinalTimeUnit, "t")
        simulationFinalTimeLayout.addWidget(self.lblSimFinalTime)
        simulationFinalTimeLayout.addWidget(self.txtSimFinalTime)
        simulationFinalTimeLayout.addWidget(self.cmbFinalTimeUnit)
        grbSimulationTypeInnerLayout.addLayout(simulationFinalTimeLayout)

        timeTabSimulationTypeGrb.setLayout(grbSimulationTypeInnerLayout)


        timeTabInitialConditionLayout = QVBoxLayout()
        self.timeTabInitialConditionGrb = QGroupBox("Condição inicial")
        timeTabInitialConditionInnerLayout = QVBoxLayout()
        timeTabInitialConditionRdgLayout = QHBoxLayout()
        self.rdgInitialConditionStationary = QRadioButton("Regime permanente")
        self.rdgInitialConditionStationary.setChecked(True)
        self.rdgInitialConditionSnapshotFile = QRadioButton("Arquivo \"snapshot\" (\"restart\")")
        self.rdgInitialConditionSnapshotFile.toggled.connect(self.SnapshotRadioButtonChangedState)
        timeTabInitialConditionRdgLayout.addWidget(self.rdgInitialConditionStationary)
        timeTabInitialConditionRdgLayout.addWidget(self.rdgInitialConditionSnapshotFile)
        timeTabInitialConditionInnerLayout.addLayout(timeTabInitialConditionRdgLayout)

        timeTabSnapshotFileLayout = QHBoxLayout()

        self.lblSnapshotFile = QLabel("Arquivo \"snapshot\": ")
        self.lblSnapshotFile.setCursor(Qt.PointingHandCursor)
        self.lblSnapshotFile.setToolTip("REGRA DE FORMAÇÃO DO NOME DO ARQUIVO: nome do arquivo da simulação + tempo da gravação em segundos + extensão \"snp\".")
        self.txtSnapshotFile = QLineEdit()
        self.btnFindSnapshotFile = QPushButton("...", clicked = self.FindSnapshotFileButtonPressed)
        timeTabSnapshotFileLayout.addWidget(self.lblSnapshotFile)
        timeTabSnapshotFileLayout.addWidget(self.txtSnapshotFile)
        timeTabSnapshotFileLayout.addWidget(self.btnFindSnapshotFile)

        timeTabInitialConditionInnerLayout.addSpacing(15)
        timeTabInitialConditionInnerLayout.addLayout(timeTabSnapshotFileLayout)

        self.timeTabInitialConditionGrb.setLayout(timeTabInitialConditionInnerLayout)
        timeTabInitialConditionLayout.addWidget(self.timeTabInitialConditionGrb)

        timeTabSecondLineLayout = QHBoxLayout()
        timeTabSecondLineLayout.addLayout(timeTabSimulationTypeLayout)
        timeTabSecondLineLayout.addLayout(timeTabInitialConditionLayout)

        timeTabLayout.addLayout(timeTabSecondLineLayout)
        timeTabLayout.addStretch(1)

        timeTabThirdLineLayout = QHBoxLayout()

        self.grbRecordSnapshot = QGroupBox("Gravação de \"snapshot\"")
        self.grbRecordSnapshot.setFixedWidth(300)
        self.RecordSnapshotTableWidget = QTableWidget(1, 1)
        self.RecordSnapshotTableWidget.verticalHeader().setVisible(False)
        self.RecordSnapshotTableWidget.horizontalHeader().setVisible(False)
        self.RecordSnapshotTableWidget.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
        self.cmbRecordSnapshotTimeUnit = self.addCustomUnitHeaderWidgetToTable(self.RecordSnapshotTableWidget, 0, 0, "", "Tempos para gravar", "t", "")
        grbRecordSnapshotInnerLayout = QVBoxLayout()
        grbRecordSnapshotInnerLayout.addWidget(self.RecordSnapshotTableWidget)

        RecordSnapshotButtonsLayout = QHBoxLayout()
        btnAddSnapshotRecordTime = QPushButton("+", clicked = self.AddRowToRecordSnapshotTable)
        btnRemoveSnapshotRecordTime = QPushButton("-", clicked = self.RemoveRowFromRecordSnapshotTable)
        RecordSnapshotButtonsLayout.addWidget(btnAddSnapshotRecordTime)
        RecordSnapshotButtonsLayout.addWidget(btnRemoveSnapshotRecordTime)
        grbRecordSnapshotInnerLayout.addLayout(RecordSnapshotButtonsLayout)

        self.grbRecordSnapshot.setLayout(grbRecordSnapshotInnerLayout)
        timeTabThirdLineLayout.addWidget(self.grbRecordSnapshot)


        self.grbTimeStepControl = QGroupBox("Controle do passo no tempo")
        self.MaxTimeStepTable = QTableWidget(2, 2)
        self.MaxTimeStepTable.verticalHeader().setVisible(False)
        self.MaxTimeStepTable.horizontalHeader().setVisible(False)
        self.MaxTimeStepTable.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.cmbTimeStepControlTimeUnit = self.addCustomUnitHeaderWidgetToTable(self.MaxTimeStepTable, 0, 0, "", "Tempo", "t", "")
        self.TimeStepControlMaxDtUnit = self.addCustomUnitHeaderWidgetToTable(self.MaxTimeStepTable, 0, 1, "Passo máximo de tempo, praticado quando permitido pela restrição CFL.", "Passo máximo", "t", "")
        timeTabThirdLineLayout.addWidget(self.grbTimeStepControl)

        self.MaxTimeStepTable.setItem(1, 0, QTableWidgetItem("0"))
        self.MaxTimeStepTable.setItem(1, 1, QTableWidgetItem("5"))

        grbTimeStepControlInnerLayout = QVBoxLayout()
        grbTimeStepControlInnerLayout.addWidget(self.MaxTimeStepTable)

        TimeStepControlButtonsLayout = QHBoxLayout()
        btnAddTimeStepControl = QPushButton("+", clicked = self.AddRowToTimeStepControlTable)
        btnRemoveTimeStepControl = QPushButton("-", clicked = self.RemoveRowFromTimeStepControlTable)
        TimeStepControlButtonsLayout.addWidget(btnAddTimeStepControl)
        TimeStepControlButtonsLayout.addWidget(btnRemoveTimeStepControl)
        grbTimeStepControlInnerLayout.addLayout(TimeStepControlButtonsLayout)

        self.grbTimeStepControl.setLayout(grbTimeStepControlInnerLayout)

        timeTabLayout.addLayout(timeTabThirdLineLayout)


        self.rdgStationarySimulation.setChecked(True)    # para disparar os eventos.

        timeTab.setLayout(timeTabLayout)

        # Tab de Perfis:
        perfisOnlyTab = QWidget()
        tab_widget.addTab(perfisOnlyTab, "Perfis")

        perfisOnlyTabLayout = QVBoxLayout()

        perfisOnlyTopLayout = QVBoxLayout()
        self.chkPerfisOnlyWriteToJson = QCheckBox("Preencher no json")
        perfisOnlyTopLayout.addWidget(self.chkPerfisOnlyWriteToJson)
        perfisOnlyTabLayout.addLayout(perfisOnlyTopLayout)

        perfisOnlyTabLayout.addSpacing(15)

        perfisOnlyRemainingLayout = QHBoxLayout()

        grbPerfisOnlyProd = QGroupBox("Coluna e Duto de Produção")
        perfisOnlyRemainingLayout.addWidget(grbPerfisOnlyProd)

        grbPerfisOnlyProdInnerLayout = QVBoxLayout()
        self.PerfisOnlyProdWidget = ProfileCustomWidget(self, TypeDuct.Producao)
        grbPerfisOnlyProdInnerLayout.addWidget(self.PerfisOnlyProdWidget.oCreatedCustomWidget)
        grbPerfisOnlyProd.setLayout(grbPerfisOnlyProdInnerLayout)

        grbPerfisOnlyServ = QGroupBox("Linha de Serviço e Anular do Poço")
        perfisOnlyRemainingLayout.addWidget(grbPerfisOnlyServ)

        grbPerfisOnlyServInnerLayout = QVBoxLayout()
        self.PerfisOnlyServWidget = ProfileCustomWidget(self, TypeDuct.Servico)
        grbPerfisOnlyServInnerLayout.addWidget(self.PerfisOnlyServWidget.oCreatedCustomWidget)
        grbPerfisOnlyServ.setLayout(grbPerfisOnlyServInnerLayout)

        perfisOnlyTabLayout.addLayout(perfisOnlyRemainingLayout)

        perfisOnlyTab.setLayout(perfisOnlyTabLayout)

        # Tab de Tendências:
        tendsOnlyTab = QWidget()
        tab_widget.addTab(tendsOnlyTab, "Tendências")

        tendOnlyTabLayout = QVBoxLayout()

        tendsOnlyTopLayout = QVBoxLayout()
        self.chkTendsOnlyWriteToJson = QCheckBox("Preencher no json")
        tendsOnlyTopLayout.addWidget(self.chkTendsOnlyWriteToJson)
        tendOnlyTabLayout.addLayout(tendsOnlyTopLayout)

        tendOnlyTabLayout.addSpacing(15)

        tendsOnlyRemainingLayout = QHBoxLayout()

        grbTendsOnlyProd = QGroupBox("Coluna e Duto de Produção")
        tendsOnlyRemainingLayout.addWidget(grbTendsOnlyProd)

        grbTendsOnlyProdInnerLayout = QVBoxLayout()
        self.TendOnlyProdWidget = TrendCustomWidget(self, TypeDuct.Producao)
        grbTendsOnlyProdInnerLayout.addWidget(self.TendOnlyProdWidget.oCreatedCustomWidget)
        grbTendsOnlyProd.setLayout(grbTendsOnlyProdInnerLayout)

        grbTendsOnlyServ = QGroupBox("Linha de Serviço e Anular do Poço")
        tendsOnlyRemainingLayout.addWidget(grbTendsOnlyServ)

        grbTendsOnlyServInnerLayout = QVBoxLayout()
        self.TendOnlyServWidget = TrendCustomWidget(self, TypeDuct.Servico)
        grbTendsOnlyServInnerLayout.addWidget(self.TendOnlyServWidget.oCreatedCustomWidget)
        grbTendsOnlyServ.setLayout(grbTendsOnlyServInnerLayout)

        tendOnlyTabLayout.addLayout(tendsOnlyRemainingLayout)

        tendsOnlyTab.setLayout(tendOnlyTabLayout)

        # Tab de Perfis e Tendências:
        #perfisAndTendsTab = QWidget()
        #tab_widget.addTab(perfisAndTendsTab, "Perfis e Tendências")

        #perfisAndTendsTabLayout = QVBoxLayout()

        #perfisAndTendsTopLayout = QVBoxLayout()
        #self.chkPerfisAndTendsWriteToJson = QCheckBox("Preencher no json")
        #perfisAndTendsTopLayout.addWidget(self.chkPerfisAndTendsWriteToJson)
        #perfisAndTendsTabLayout.addLayout(perfisAndTendsTopLayout)

        #perfisAndTendsTabLayout.addSpacing(15)

        #perfisAndTendsTabMainLayout = QVBoxLayout()

        #grbPerfisAndTendsProducao = QGroupBox("Coluna e Duto de Produção")
        #perfisAndTendsTabMainLayout.addWidget(grbPerfisAndTendsProducao)

        #perfisAndTendsProdGrbInnerLayout = QHBoxLayout()
        #perfisProdGrb = QGroupBox("Perfis")
        #perfisProdGrb.setMaximumWidth(310)
        #self.perfisProdWidget = ProfileCustomWidget(self, TypeDuct.Producao)
        #perfisProdGrbInnerLayout = QHBoxLayout()
        #perfisProdGrbInnerLayout.addWidget(self.perfisProdWidget.oCreatedCustomWidget)
        #perfisProdGrb.setLayout(perfisProdGrbInnerLayout)
        #tendsProdGrb = QGroupBox("Tendências")
        ##tendsProdGrb.setMaximumWidth(310)
        #self.tendsProdWidget = TrendCustomWidget(self, TypeDuct.Producao)
        #tendsProdGrbInnerLayout = QHBoxLayout()
        #tendsProdGrbInnerLayout.addWidget(self.tendsProdWidget.oCreatedCustomWidget)
        #tendsProdGrb.setLayout(tendsProdGrbInnerLayout)
        #perfisAndTendsProdGrbInnerLayout.addWidget(perfisProdGrb)
        #perfisAndTendsProdGrbInnerLayout.addWidget(tendsProdGrb)
        #grbPerfisAndTendsProducao.setLayout(perfisAndTendsProdGrbInnerLayout)

        #grbPerfisAndTendsService = QGroupBox("Linha de Serviço e Anular do Poço")
        #perfisAndTendsTabMainLayout.addWidget(grbPerfisAndTendsService)

        #perfisAndTendsServGrbInnerLayout = QHBoxLayout()
        #perfisServGrb = QGroupBox("Perfis")
        #perfisServGrb.setMaximumWidth(310)
        #self.perfisServWidget = ProfileCustomWidget(self, TypeDuct.Servico)
        #perfisServGrbInnerLayout = QHBoxLayout()
        #perfisServGrbInnerLayout.addWidget(self.perfisServWidget.oCreatedCustomWidget)
        #perfisServGrb.setLayout(perfisServGrbInnerLayout)
        #tendsServGrb = QGroupBox("Tendências")
        #self.tendsServWidget = TrendCustomWidget(self, TypeDuct.Servico)
        #tendsServGrbInnerLayout = QHBoxLayout()
        #tendsServGrbInnerLayout.addWidget(self.tendsServWidget.oCreatedCustomWidget)
        #tendsServGrb.setLayout(tendsServGrbInnerLayout)
        #perfisAndTendsServGrbInnerLayout.addWidget(perfisServGrb)
        #perfisAndTendsServGrbInnerLayout.addWidget(tendsServGrb)
        #grbPerfisAndTendsService.setLayout(perfisAndTendsServGrbInnerLayout)

        #perfisAndTendsTabLayout.addLayout(perfisAndTendsTabMainLayout)

        #perfisAndTendsTab.setLayout(perfisAndTendsTabLayout)

        # Botões
        button_convert = QPushButton("Converter", clicked=self.pressed_convert)
        button_exit = QPushButton("Sair", clicked=self.close)

        button_layout = QHBoxLayout()
        button_layout.addWidget(button_convert)
        button_layout.addWidget(button_exit)

        main_layout = QVBoxLayout()
        main_layout.addWidget(tab_widget)
        main_layout.addLayout(button_layout)

        widget = QWidget()
        widget.setLayout(main_layout)
        self.setCentralWidget(widget)

        # Remover o botão de maximizar:
        self.setWindowFlags(self.windowFlags() & ~Qt.WindowMaximizeButtonHint)


    # ================================ tempo
    def GetSimulationTimeConfigObject(self) -> TimeConfigObject:

        bTransientSimulation = self.rdgTransientSimulation.isChecked()

        tempoFinal = -100.0
        if bTransientSimulation:
            tempoFinal = self.ConvertUnit(float(self.txtSimFinalTime.text()), "t", self.cmbFinalTimeUnit, "s")

        iCondicaoInicial = 1
        sSnapshotArq = ""
        if bTransientSimulation:
            iCondicaoInicial = -100
            if self.rdgInitialConditionStationary.isChecked():
                iCondicaoInicial = 1
            elif self.rdgInitialConditionSnapshotFile.isChecked():
                iCondicaoInicial = 2
                sSnapshotArq = self.txtSnapshotFile.text()

        gravaMomento = []
        if bTransientSimulation:

            for row in range(1, self.RecordSnapshotTableWidget.rowCount()):
                gravaMomento_item = self.RecordSnapshotTableWidget.item(row, 0)

                if gravaMomento_item:
                    gravaMomento_value = self.ConvertUnit(float(gravaMomento_item.text()), "t", self.cmbRecordSnapshotTimeUnit, "s")
                    gravaMomento.append(gravaMomento_value)
                else:
                    gravaMomento.append(-100)

        tempos = []
        dtMax = []
        if bTransientSimulation:

            rowCount = self.MaxTimeStepTable.rowCount()
            if rowCount > 1:
                tempos.clear()
                dtMax.clear()

            for row in range(1, rowCount):

                tempos_item = self.MaxTimeStepTable.item(row, 0)
                dtMax_item = self.MaxTimeStepTable.item(row, 1)

                if tempos_item:
                    tempos_value = self.ConvertUnit(float(tempos_item.text()), "t", self.cmbTimeStepControlTimeUnit, "s")
                    tempos.append(tempos_value)
                else:
                    tempos.append(-100.0)

                if dtMax_item:
                    dtMax_value = self.ConvertUnit(float(dtMax_item.text()), "t", self.TimeStepControlMaxDtUnit, "s")
                    dtMax.append(dtMax_value)
                else:
                    dtMax.append(-100.0)


        oTimeConfigObject = TimeConfigObject(bTransientSimulation, iCondicaoInicial, sSnapshotArq, tempoFinal, tempos, dtMax, gravaMomento)
        return oTimeConfigObject



    def RemoveRowFromRecordSnapshotTable(self):
        selected_ranges = self.RecordSnapshotTableWidget.selectedRanges()

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
            self.RecordSnapshotTableWidget.removeRow(row)

    def AddRowToRecordSnapshotTable(self):
        row_count = self.RecordSnapshotTableWidget.rowCount()
        self.RecordSnapshotTableWidget.insertRow(row_count)

    def AddRowToTimeStepControlTable(self):
        row_count = self.MaxTimeStepTable.rowCount()
        self.MaxTimeStepTable.insertRow(row_count)

    def RemoveRowFromTimeStepControlTable(self):
        selected_ranges = self.MaxTimeStepTable.selectedRanges()

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
            self.MaxTimeStepTable.removeRow(row)

    def TransientRadioButtonChangedState(self, checked):
        self.lblSimFinalTime.setEnabled(checked)
        self.txtSimFinalTime.setEnabled(checked)
        self.cmbFinalTimeUnit.setEnabled(checked)
        self.timeTabInitialConditionGrb.setEnabled(checked)
        self.grbRecordSnapshot.setEnabled(checked)
        self.grbTimeStepControl.setEnabled(checked)

        self.lblSnapshotFile.setEnabled(self.rdgTransientSimulation.isChecked() and self.rdgInitialConditionSnapshotFile.isChecked())
        self.txtSnapshotFile.setEnabled(self.rdgTransientSimulation.isChecked() and self.rdgInitialConditionSnapshotFile.isChecked())
        self.btnFindSnapshotFile.setEnabled(self.rdgTransientSimulation.isChecked() and self.rdgInitialConditionSnapshotFile.isChecked())

    def SnapshotRadioButtonChangedState(self, checked):
        self.lblSnapshotFile.setEnabled(self.rdgTransientSimulation.isChecked() and self.rdgInitialConditionSnapshotFile.isChecked())
        self.txtSnapshotFile.setEnabled(self.rdgTransientSimulation.isChecked() and self.rdgInitialConditionSnapshotFile.isChecked())
        self.btnFindSnapshotFile.setEnabled(self.rdgTransientSimulation.isChecked() and self.rdgInitialConditionSnapshotFile.isChecked())

    def FindSnapshotFileButtonPressed(self):
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly

        file_name, _ = QFileDialog.getOpenFileName(self, "Selecione um arquivo \"snapshot\"", "", "Arquivos \"snapshot\" (*.snp);;Todos os arquivos (*)", options=options)
        
        if file_name:
            self.txtSnapshotFile.setText(file_name)
    # ================================ tempo

    # ================================= modelo de fluido
    def toggle_fluid_tab(self, state):
        self.txtFluidTabFile.setEnabled(state)
        self.button_browse_fluid_tab.setEnabled(state)

    def toggle_fluid_ctm(self, state):
        self.txtFluidCTMFile.setEnabled(state)
        self.button_browse_fluid_ctm.setEnabled(state)

    def browse_fluid_tab(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Selecionar tabela *.tab", "", "Arquivos Tab (*.tab)")
        if file_path:
            self.txtFluidTabFile.setText(file_path)

    def browse_fluid_ctm(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Selecionar arquivo *.ctm", "", "Arquivos CTM (*.ctm)")
        if file_path:
            self.txtFluidCTMFile.setText(file_path)
    # ================================= modelo de fluido

    # ================================= Contorno Produção [Chegada]
    def IncludeChokeSupStateChanged(self, state):
        bEnabled = state == 2
        self.CCProductionEnd_txtChokeSupCd.setEnabled(bEnabled)
        self.ccProductionEndChokeSupOpeningTable.setEnabled(bEnabled)


    def AddRowToProdEndChokeOpeningTable(self):
        row_count = self.ccProductionEndChokeSupOpeningTable.rowCount()
        self.ccProductionEndChokeSupOpeningTable.insertRow(row_count)

    def AddRowToProdEndPressureTable(self):
        row_count = self.ccProductionEndPressureTable.rowCount()
        self.ccProductionEndPressureTable.insertRow(row_count)

    def RemoveRowFromProdEndPressureTable(self):
        selected_ranges = self.ccProductionEndPressureTable.selectedRanges()

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
            self.ccProductionEndPressureTable.removeRow(row)

    def RemoveRowFromProdEndChokeOpeningTable(self):
        selected_ranges = self.ccProductionEndChokeSupOpeningTable.selectedRanges()

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
            self.ccProductionEndChokeSupOpeningTable.removeRow(row)

    
    def GetSeparadorObject(self) -> SeparadorObject:

        # Obter os dois vetores:
        tempo = []
        pressao = []

        for row in range(1, self.ccProductionEndPressureTable.rowCount()):
            tempo_item = self.ccProductionEndPressureTable.item(row, 0)
            pressao_item = self.ccProductionEndPressureTable.item(row, 1)

            if tempo_item:
                tempo_value = self.ConvertUnit(float(tempo_item.text()), "t", self.cmbEndPressureTimeUnit, "s")
                tempo.append(tempo_value)
            else:
                tempo.append(-100)

            if pressao_item:
                pressao_value = self.ConvertUnit(float(pressao_item.text()), "P", self.cmbEndPressureUnit, "kgf/cm2")
                pressao.append(pressao_value)
            else:
                pressao.append(-100)

        separadorObject = SeparadorObject(pressao, tempo)
        return separadorObject

    def GetChokeSupObject(self) -> ChokeSupObject:

        # Ativo ou não?
        bIncludeChoke = self.chkProductionEndActiveChokeSup.isChecked()

        # Coeficiente de descarga:
        dCoefDescarga = 1.0
        if bIncludeChoke:
            dCoefDescarga = float(self.CCProductionEnd_txtChokeSupCd.text())

        # Obter os dois vetores:
        tempo = []
        abertura = []

        for row in range(1, self.ccProductionEndChokeSupOpeningTable.rowCount()):
            tempo_item = self.ccProductionEndChokeSupOpeningTable.item(row, 0)
            opening_item = self.ccProductionEndChokeSupOpeningTable.item(row, 1)

            if tempo_item:
                tempo_value = self.ConvertUnit(float(tempo_item.text()), "t", self.cmbChokeSupOpeningTimeUnit, "s")
                tempo.append(tempo_value)
            else:
                tempo.append(-100)

            if opening_item:
                opening_value = float(opening_item.text())
                abertura.append(opening_value)
            else:
                abertura.append(-100)

        oChokeSupObject = ChokeSupObject(bIncludeChoke, dCoefDescarga, abertura, tempo)
        return oChokeSupObject

    # ================================= Contorno Produção [Chegada]

    # ================================= fluidos - produção 
    def GetGasFluidObject(self) -> GasFluidObject:
        if not self.chkFluidGasWriteToJson.isChecked():
            return None

        densidadeGas_value = float(self.txtFluidGasDensity.text())
        fracCO2_value = float(self.txtFluidGasCO2Fraction.text())
        oGasFluidObject = GasFluidObject(densidadeGas=densidadeGas_value , fracCO2=fracCO2_value)

        return oGasFluidObject

    def GetProductionFluidObjects(self) -> List[ProductionFluidObject]:
        if not self.checkBox_preencher_json.isChecked():
            return None
        
        production_fluids = []
        table_widget: QTableWidget = self.table_widget  
        
        for row in range(0, table_widget.rowCount()):  
            id_item = table_widget.item(row, 0)
            rgo_item = table_widget.item(row, 1)
            bsw_item = table_widget.item(row, 2)
            bsw_corte_item = table_widget.item(row, 3)
            
            if id_item and rgo_item and bsw_item and bsw_corte_item:
                id_value = int(id_item.text())
                rgo_value = float(rgo_item.text())
                bsw_value = float(bsw_item.text())
                bsw_corte_value = float(bsw_corte_item.text())
                
                fluid_object = ProductionFluidObject(id=id_value, rgo=rgo_value, bsw=bsw_value, bswCorte=bsw_corte_value)
                production_fluids.append(fluid_object)
        
        return production_fluids


    def add_row_to_table(self):
        row_count = self.table_widget.rowCount()
        self.table_widget.insertRow(row_count)
        self.table_widget.setItem(row_count, 0, QTableWidgetItem(str(row_count)))

    def remove_row_from_table(self):
        selected_cells = self.table_widget.selectedItems()
        if not selected_cells:
            return
    
        #current_cell = self.table_widget.currentItem()
        current_cell = selected_cells[0]
        if current_cell is None:
            return

        row = current_cell.row()
        fluid_id = self.table_widget.item(row, 0).text()

        # Pedir ao usuário para confirmar:
        response = QMessageBox.question(self, "Remover fluido", f"Remover fluido {fluid_id}?",
                                        QMessageBox.Ok | QMessageBox.Cancel)

        if response == QMessageBox.Ok:
            # Remover a linha
            self.table_widget.removeRow(row)

            # Atualizar os "id"
            for i in range(self.table_widget.rowCount()):
                self.table_widget.setItem(i, 0, QTableWidgetItem(str(i)))

        # Clear the current selection
        self.table_widget.clearSelection()
    # ================================= fluidos - produção

    # ================================= IPRS
    def PressedButtonAddIPR(self):
        row_count = self.IPRListTable.rowCount()

        new_key = len(self.oIPRsDict)
        if new_key > 0:
            new_key = max(self.oIPRsDict.keys()) + 1

        self.IPRListTable.insertRow(row_count)
        self.IPRListTable.setItem(row_count, 0, QTableWidgetItem(str(new_key)))
        self.IPRListTable.setItem(row_count, 1, QTableWidgetItem("IPR " + str(new_key)))

        self.oIPRsDict[new_key] = IPRCustomWidget(self)
        self.iprTabStackedDetails.addWidget(self.oIPRsDict[new_key].oCreatedCustomWidget)

        self.IPRListTable.setCurrentCell(row_count, 1)


    def PressedButtonRemoveIPR(self):
        selected_ranges = self.IPRListTable.selectedRanges()

        if not selected_ranges:
            return

        response = QMessageBox.question(self, "Remover IPRs", "Remover as IPRs selecionadas?", 
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

            iKeyItem = self.IPRListTable.item(row, 0)
            if iKeyItem:
                widgetKey = int(iKeyItem.text())
                self.iprTabStackedDetails.removeWidget(self.oIPRsDict[widgetKey].oCreatedCustomWidget)
                del self.oIPRsDict[widgetKey]

            self.IPRListTable.removeRow(row)

        row_count = self.IPRListTable.rowCount()
        if row_count > 0:
            self.IPRListTable.setCurrentCell(row_count - 1, 1)

    def ItemSelectionChangedInIPRTable(self):
        selected_items = self.IPRListTable.selectedItems()

        if not selected_items:
            return

        row = selected_items[0].row()

        iKeyItem = self.IPRListTable.item(row, 0)
        if iKeyItem:
            widgetKey = int(iKeyItem.text())
            self.iprTabStackedDetails.setCurrentWidget(self.oIPRsDict[widgetKey].oCreatedCustomWidget)
    # ================================= IPRS

    # ================================= Fontes de Massa [Produção]
    def PressedButtonRemoveProdMassSource(self):
        selected_ranges = self.prodMassSources_ListTable.selectedRanges()

        if not selected_ranges:
            return

        response = QMessageBox.question(self, "Remover fontes de massa", "Remover as fontes de massa selecionadas?", 
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

            iKeyItem = self.prodMassSources_ListTable.item(row, 0)
            if iKeyItem:
                widgetKey = int(iKeyItem.text())
                self.prodMassSourceStackedDetails.removeWidget(self.oProdMassSourcesDict[widgetKey].oCreatedCustomWidget)
                del self.oProdMassSourcesDict[widgetKey]

            self.prodMassSources_ListTable.removeRow(row)

        row_count = self.prodMassSources_ListTable.rowCount()
        if row_count > 0:
            self.prodMassSources_ListTable.setCurrentCell(row_count - 1, 1)

    def PressedButtonAddProdMassSource(self):
        row_count = self.prodMassSources_ListTable.rowCount()

        new_key = len(self.oProdMassSourcesDict)
        if new_key > 0:
            new_key = max(self.oProdMassSourcesDict.keys()) + 1

        self.prodMassSources_ListTable.insertRow(row_count)
        self.prodMassSources_ListTable.setItem(row_count, 0, QTableWidgetItem(str(new_key)))
        self.prodMassSources_ListTable.setItem(row_count, 1, QTableWidgetItem("Fonte " + str(row_count)))

        self.oProdMassSourcesDict[new_key] = ProdMassSourceCustomWidget(self)
        self.prodMassSourceStackedDetails.addWidget(self.oProdMassSourcesDict[new_key].oCreatedCustomWidget)

        self.prodMassSources_ListTable.setCurrentCell(row_count, 1)

    def ClickedCellInProdMassSourceTable(self, row, col):
        selected_ranges = self.prodMassSources_ListTable.selectedRanges()

        if not selected_ranges:
            return

        iKeyItem = self.prodMassSources_ListTable.item(row, 0)
        if iKeyItem:
            widgetKey = int(iKeyItem.text())
            self.prodMassSourceStackedDetails.setCurrentWidget(self.oProdMassSourcesDict[widgetKey].oCreatedCustomWidget)

    def ItemSelectionChangedInProdMassSourceTable(self):
        selected_items = self.prodMassSources_ListTable.selectedItems()

        if not selected_items:
            return

        row = selected_items[0].row()

        self.ClickedCellInProdMassSourceTable(row, 0)
    # ================================= Fontes de Massa [Produção]

    # ================================= Contorno de gás lift
    def GetGasInjObject(self) -> GasInjObject:
        # tipo da condição de contorno:
        iTipoCC = 0
        if self.rdgTipoCCVazao.isChecked():
            iTipoCC = 1

        # estimativa inicial para vazão 
        bChuteVazaoInjecao = self.rdgTipoCCPressaoComQ.isChecked()

        # Obter os quatro vetores:
        temperatura = []
        pressao_injecao = []
        vazao_gas = []
        tempo = []
        
        for row in range(self.tableGasInj.rowCount()):
            temperatura_item = self.tableGasInj.item(row, 1)
            pressao_injecao_item = self.tableGasInj.item(row, 3)
            vazao_gas_item = self.tableGasInj.item(row, 2)
            tempo_item = self.tableGasInj.item(row, 0)

            if temperatura_item:
                temperatura_value = self.ConvertUnit(float(temperatura_item.text()), "T", self.cmbGLCCTemperatureUnit, "C") if temperatura_item.text() else 0
                temperatura.append(temperatura_value)
            else:
                temperatura.append(0)

            if pressao_injecao_item:
                pressao_value = self.ConvertUnit(float(pressao_injecao_item.text()), "P", self.cmbGLCCPUnit, "kgf/cm2") if pressao_injecao_item.text() else 0
                pressao_injecao.append(pressao_value)
            else:
                pressao_injecao.append(0)

            if vazao_gas_item:
                vazao_value = self.ConvertUnit(float(vazao_gas_item.text()), "Q", self.cmbGLCCQUnit, "std m3 / d") if vazao_gas_item.text() else 0
                vazao_gas.append(vazao_value)
            else:
                vazao_gas.append(0)

            if tempo_item:
                tempo_value = self.ConvertUnit(float(tempo_item.text()), "t", self.cmbGLCCTimeUnit, "s") if tempo_item.text() else 0
                tempo.append(tempo_value)
            else:
                tempo.append(0)

        # Criar e retornar o objeto:
        gasInjObject = GasInjObject(iTipoCC=iTipoCC, bChuteVazaoInjecao=bChuteVazaoInjecao, oTemperatura=temperatura, oPressaoInjecao=pressao_injecao, oVazaoGas=vazao_gas, oTempo=tempo)
        return gasInjObject


    def removeRowFromGLCC(self):
        selected_ranges = self.tableGasInj.selectedRanges()

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
            self.tableGasInj.removeRow(row)

    def addRowToGLCC(self):
        row_count = self.tableGasInj.rowCount()
        self.tableGasInj.insertRow(row_count)
    # ================================= Contorno de gás lift

    # ================================= Válvulas de gas-lift
    def CreateVGLObjectsFromTable(self) -> List[VGLObject]:
        vgl_objects = []

        for row in range(1, self.vglTableWidget.rowCount()):

            iId = int(self.vglTableWidget.item(row, 0).text()) if self.vglTableWidget.item(row, 0) is not None else -100
            fCompProducao = float(self.vglTableWidget.item(row, 1).text()) if self.vglTableWidget.item(row, 1) is not None else -100
            fCompServico = float(self.vglTableWidget.item(row, 2).text()) if self.vglTableWidget.item(row, 2) is not None else -100
            sType = self.vglTableWidget.cellWidget(row, 3).currentText()
            fDiameter = float(self.vglTableWidget.item(row, 4).text()) if self.vglTableWidget.item(row, 4) is not None else -100
            fCv = float(self.vglTableWidget.item(row, 5).text()) if self.vglTableWidget.item(row, 5) is not None else -100
            fPCalib = float(self.vglTableWidget.item(row, 6).text()) if self.vglTableWidget.item(row, 6) is not None else -100
            fTCalib = float(self.vglTableWidget.item(row, 7).text()) if self.vglTableWidget.item(row, 7) is not None else -100
            fAreaRatio = float(self.vglTableWidget.item(row, 8).text()) if self.vglTableWidget.item(row, 8) is not None else -100

            fCompProducao = self.ConvertUnit(fCompProducao, "L", self.cmbVGLCompProdUnit, "m")
            fCompServico = self.ConvertUnit(fCompServico, "L", self.cmbVGLCompServiceUnit, "m")
            fDiameter = self.ConvertUnit(fDiameter, "L", self.cmbVGLOrifDUnit, "m")
            fPCalib = self.ConvertUnit(fPCalib, "P", self.cmbVGLPCalibUnit, "kgf/cm2")
            fTCalib = self.ConvertUnit(fTCalib, "T", self.cmbVGLTCalibUnit, "C")

            vgl_objects.append(VGLObject(iId, fCompProducao, fCompServico, sType, fDiameter, fCv, fPCalib, fTCalib, fAreaRatio))

        return vgl_objects


    def removeRowFromVGLTable(self):
        selected_cells = self.vglTableWidget.selectedItems()
        if not selected_cells:
            return

        current_cell = selected_cells[0]
        if current_cell is None:
            return

        row = current_cell.row()
        if row == 0:
            return

        vgl_id = self.vglTableWidget.item(row, 0).text()

        response = QMessageBox.question(self, "Remover VGL", f"Remover VGL {vgl_id}?", QMessageBox.Ok | QMessageBox.Cancel)

        if response == QMessageBox.Ok:
            self.vglTableWidget.removeRow(row)

            for i in range(1, self.vglTableWidget.rowCount()):
                self.vglTableWidget.setItem(i, 0, QTableWidgetItem(str(i-1)))

        self.vglTableWidget.clearSelection()

    def addRowToVGLTable(self):
        row_count = self.vglTableWidget.rowCount()
        self.vglTableWidget.insertRow(row_count)
        self.vglTableWidget.setItem(row_count, 0, QTableWidgetItem(str(row_count-1)))

        cmbType = QComboBox()
        cmbType.addItems(["Orifício", "Pressão", "Venturi"])
        cmbType.setEditable(False)
        self.vglTableWidget.setCellWidget(row_count, 3, cmbType)
        self.vglTableWidget.resizeColumnToContents(3)

    def BtnClickImportVGLFromFlowedit(self):
        for i in range(self.vglTableWidget.rowCount() - 1, 0, -1):
            self.vglTableWidget.removeRow(i)

        QApplication.setOverrideCursor(Qt.WaitCursor)

        try:
            oImportedVGLObjects = VGLObject.ImportVGLObjectsFromFlowedit(self.entry_flowedit.text())
        
            for oVGLObject in oImportedVGLObjects:
                self.addRowToVGLTable()
                row_count = self.vglTableWidget.rowCount()
                self.vglTableWidget.setItem(row_count-1, 1, QTableWidgetItem(str(round(oVGLObject.fCompProducao, 2))))
                self.vglTableWidget.setItem(row_count-1, 2, QTableWidgetItem(str(round(oVGLObject.fCompServico, 2))))
            
            if len(oImportedVGLObjects) > 0:
                self.cmbVGLCompProdUnit.setCurrentIndex(self.cmbVGLCompProdUnit.findText("m"))
                self.cmbVGLCompServiceUnit.setCurrentIndex(self.cmbVGLCompServiceUnit.findText("m"))

            QApplication.restoreOverrideCursor()
            QMessageBox.information(self, "Importação", "VGLs importadas com sucesso!")

        except Exception as e:
            QApplication.restoreOverrideCursor()
            QMessageBox.critical(self, "Erro", "Erro na importação das VGLs!")





    def addCustomUnitHeaderWidgetToTable(self, oTable: QTableWidget, iRow: int, iColumn: int, sTooltip: str, sMainHeaderLabelText: str, sComboBoxDimension: str, sOptionalSecondHeader: str) -> QComboBox:
        
        headerWidget = QWidget()

        bTooltipIsBlank = not sTooltip or sTooltip.isspace()

        if not bTooltipIsBlank:
            headerWidget.setToolTip(sTooltip)

        headerLayout = QVBoxLayout()

        headerLabel = QLabel(sMainHeaderLabelText)

        headerLayout.addWidget(headerLabel)

        bSecondHeaderIsBlank = not sOptionalSecondHeader or sOptionalSecondHeader.isspace()
        if not bSecondHeaderIsBlank:
            secondHeaderLabel = QLabel(sOptionalSecondHeader)
            headerLayout.addWidget(secondHeaderLabel)

        cmbheaderComboBox = None
        bComboBoxDimensionIsBlank = not sComboBoxDimension or sComboBoxDimension.isspace()
        if not bComboBoxDimensionIsBlank:
            cmbheaderComboBox = QComboBox()
            self.ConfigUnitComboBox(cmbheaderComboBox, sComboBoxDimension)
            headerLayout.addWidget(cmbheaderComboBox)

        headerLayout.setContentsMargins(0, 0, 0, 0)
        headerWidget.setLayout(headerLayout)

        oTable.setCellWidget(iRow, iColumn, headerWidget)
        oTable.resizeRowToContents(iRow)

        return cmbheaderComboBox
    # ================================= Válvulas de gas-lift

    # ================================= geral
    def WriteNonFloweditInfoToJson(self):

        # Abrir o json:
        sJsonFilePath = self.entry_json.text()
        with open(sJsonFilePath, 'r+', encoding='utf-8') as file:
            json_data = json.load(file)

        # Variáveis gerais do json:
        oConfigInicialNode = json_data.get("configuracaoInicial", {})
        oConfigInicialAdvancedNode = oConfigInicialNode["Avancado"]

        # Contorno Produção - Chegada:
        if self.chkCCProductionEndWriteToJson.isChecked():
            oSeparadorNode = json_data.get("separador", {})
            oSeparadorObject = self.GetSeparadorObject()
            oSeparadorObject.WriteObjectToJsonFile(oSeparadorNode)

            oChokeSupNode = json_data.get("chokeSup", {})
            oChokeSupObject = self.GetChokeSupObject()
            oChokeSupObject.WriteObjectToJsonFile(oChokeSupNode)

        # Fluidos - produção:
        if self.checkBox_preencher_json.isChecked():
            oProductionFluidsNode = json_data.get("fluidosProducao", {})
            oProductionFluidsNode.clear()

            oProductionFluidObjects = self.GetProductionFluidObjects()
            if oProductionFluidObjects:
                for oProductionFluid in oProductionFluidObjects:
                    oProductionFluid.WriteFluidObjectToJsonFile(oProductionFluidsNode)

        
        # Perfil Produção
        if self.chkPerfisOnlyWriteToJson.isChecked():

            oPerfilProdNode = json_data.get("perfilProducao")

            if oPerfilProdNode is None:
                oPerfilProdNode = {}
                json_data["perfilProducao"] = oPerfilProdNode

            self.PerfisOnlyProdWidget.GetProfileObject().WriteObjectToJsonFile(oPerfilProdNode)

        # Perfil Serviço
        if self.chkPerfisOnlyWriteToJson.isChecked():
            
            oPerfilServNode = json_data.get("perfilServico")

            if oPerfilServNode is None:
                oPerfilServNode = {}
                json_data["perfilServico"] = oPerfilServNode

            self.PerfisOnlyServWidget.GetProfileObject().WriteObjectToJsonFile(oPerfilServNode)

        # Tendências Produção:
        if self.chkTendsOnlyWriteToJson.isChecked():

            oTendProdNode = json_data.get("tendP")

            if oTendProdNode is None:
                oTendProdNode = []
                json_data["tendP"] = oTendProdNode
            else:
                oTendProdNode.clear()

            self.TendOnlyProdWidget.WriteTrendObjectsToJson(oTendProdNode)

        # Tendências Serviço:
        if self.chkTendsOnlyWriteToJson.isChecked():

            oTendServNode = json_data.get("tendS")

            if oTendServNode is None:
                oTendServNode = []
                json_data["tendS"] = oTendServNode
            else:
                oTendServNode.clear()

            self.TendOnlyServWidget.WriteTrendObjectsToJson(oTendServNode)

        # IPRs:
        if self.chkIPRWriteToJson.isChecked():

            oIPRCollectionNode = json_data.get("ipr")

            if oIPRCollectionNode is None:
                oIPRCollectionNode = []
                json_data["ipr"] = oIPRCollectionNode
            else:
                oIPRCollectionNode.clear()

            iWrittenIPRCount = 0
            for oIPR in self.oIPRsDict.values():
                oIPRObj = oIPR.GetIPRObject(iWrittenIPRCount)
                oIPRObj.WriteObjectToJsonFile(oIPRCollectionNode)
                iWrittenIPRCount = iWrittenIPRCount + 1

        # Fontes de Massa:
        if self.chkProdMassSourcesWriteToJson.isChecked():
            
            oProdMassSourcesNode = json_data.get("fonteMassa")

            if oProdMassSourcesNode is None:
                oProdMassSourcesNode = []
                json_data["fonteMassa"] = oProdMassSourcesNode
            else:
                oProdMassSourcesNode.clear()

            iWrittenProdMassSourcesCount = 0
            for oProdMassSource in self.oProdMassSourcesDict.values():
                oProdMassSourceObj = oProdMassSource.GetProdMassSourceObject(iWrittenProdMassSourcesCount)
                oProdMassSourceObj.WriteObjectToJsonFile(oProdMassSourcesNode)
                iWrittenProdMassSourcesCount = iWrittenProdMassSourcesCount + 1

        # Contorno - gás-lift
        if self.chkContornoGLWriteToJson.isChecked():
            oGasInjNode = json_data.get("gasInj", {})
            oGasInjNode.clear()
            oGasInjObject = self.GetGasInjObject()
            oGasInjObject.WriteObjectToJsonFile(oGasInjNode)

            oConfigInicialNode["linhaGas"] = self.chkContornoGLActivateGLFlow.isChecked()
            oConfigInicialNode["tabP"] = True
            oConfigInicialNode["tabG"] = True

            oConfigInicialAdvancedNode["acopColAnulPermForte"] = int(self.txtAcopColAnulGL.text())

        # Válvulas de gas-lift:
        if self.chkVGLWriteToJson.isChecked():
            oVGLCollectionNode = json_data.get("fonteGasLift")
            if oVGLCollectionNode is None:
                oVGLCollectionNode = []
                json_data["fonteGasLift"] = oVGLCollectionNode
            else:
                oVGLCollectionNode.clear()
            #oVGLCollectionNode = json_data.setdefault("fonteGasLift", {})

            oVGLObjectCollection = self.CreateVGLObjectsFromTable()
            if oVGLObjectCollection:
                for oVGLObject in oVGLObjectCollection:
                    oVGLObject.WriteVGLObjectToJson(oVGLCollectionNode)

        # Fluido Gás:
        if self.chkFluidGasWriteToJson.isChecked():
            oFluidGasNode = json_data.get("fluidoGas", {})
            oFluidGasNode.clear()

            oGasFluidObject = self.GetGasFluidObject()
            if oGasFluidObject:
                oGasFluidObject.WriteGasFluidObjectToJsonFile(oFluidGasNode)

        # Configurações de Tempo:
        if self.chkTimeTabWriteToJson.isChecked():
            oTimeConfigObject = self.GetSimulationTimeConfigObject()
            oTimeConfigObject.WriteObjectToJsonFile(json_data)

        # Salvar e fechar o json:
        with open(sJsonFilePath, 'w', encoding='utf-8') as file:
            json.dump(json_data, file, indent=2, ensure_ascii=False)
        file.close()

    # ================================= geral

    # ================================= unidades
    def ConfigUnitComboBox(self, oUnitComboBox, sVariableDimension):
        oUnitComboBox.clear()  

        if sVariableDimension == "Q":
            oUnitComboBox.addItems(["std m3 / d", "std m3 / h"])
        elif sVariableDimension == "P":
            oUnitComboBox.addItems(["bar", "kgf/cm2"])
        elif sVariableDimension == "t":
            oUnitComboBox.addItems(["s", "min", "h", "d"])
        elif sVariableDimension == "T":
            oUnitComboBox.addItems(["C"])
        elif sVariableDimension == "L":
            oUnitComboBox.addItems(["in", "m"])
        elif sVariableDimension == "W":
            oUnitComboBox.addItems(["kg/h", "kg/s"])
        elif sVariableDimension == "QOverP":
            oUnitComboBox.addItems(["std m3 / d / bar", "std m3 / d / (kgf/cm2)"])

        # ordem alfabética
        oUnitComboBox.model().sort(0)
        oUnitComboBox.setEditable(False)

        # Método para conversão de unidades
    def ConvertUnit(self, fOriginalValue, sDimension, cmbOriginalUnit, sDesiredUnit):

        sOriginalUnit = cmbOriginalUnit.currentText()

        if sOriginalUnit == sDesiredUnit:
            return fOriginalValue

        if sDimension == "P":
            kgfToSI = 98066.52
            barToSI = 1.0e5

            if sOriginalUnit == "kgf/cm2":
                valueSI = fOriginalValue * kgfToSI
            elif sOriginalUnit == "bar":
                valueSI = fOriginalValue * barToSI
            else:
                valueSI = fOriginalValue

            if sDesiredUnit == "kgf/cm2":
                return valueSI / kgfToSI
            elif sDesiredUnit == "bar":
                return valueSI / barToSI
            else:
                return fOriginalValue


        elif sDimension == "QOverP":
            
            if sOriginalUnit == "std m3 / d / bar":
                valueSI = fOriginalValue / 100000.0
            elif sOriginalUnit == "std m3 / d / (kgf/cm2)":
                valueSI = fOriginalValue / 98066.52
            else:
                valueSI = fOriginalValue
                
            if sDesiredUnit == "std m3 / d / bar":
                return valueSI * 100000.0
            elif sDesiredUnit == "std m3 / d / (kgf/cm2)":
                return valueSI * 98066.52
            else:
                return fOriginalValue

        elif sDimension == "t":
            minToSI = 60
            hToSI = 3600
            dToSI = 24 * 3600

            if sOriginalUnit == "min":
                valueSI = fOriginalValue * minToSI
            elif sOriginalUnit == "h":
                valueSI = fOriginalValue * hToSI
            elif sOriginalUnit == "d":
                valueSI = fOriginalValue * dToSI
            elif sOriginalUnit == "s":
                valueSI = fOriginalValue
            else:
                valueSI = fOriginalValue 

            if sDesiredUnit == "min":
                return valueSI / minToSI
            elif sDesiredUnit == "h":
                return valueSI / hToSI
            elif sDesiredUnit == "d":
                return valueSI / dToSI
            elif sDesiredUnit == "s":
                return valueSI
            else:
                return fOriginalValue
   
        elif sDimension == "L":
            inToSI = 0.0254

            if sOriginalUnit == "in":
                valueSI = fOriginalValue * inToSI
            else:
                valueSI = fOriginalValue

            if sDesiredUnit == "in":
                return valueSI / inToSI
            else:
                return valueSI

        elif sDimension == "W":
            kgPerHourToSI = 1.0 / 3600.0

            if sOriginalUnit == "kg/h":
                valueSI = fOriginalValue * kgPerHourToSI
            else:
                valueSI = fOriginalValue

            if sDesiredUnit == "kg/h":
                return valueSI * 3600.0
            else:
                return valueSI
        
        elif sDimension == "Q":
            
            if sOriginalUnit == "std m3 / h":
                valueSI = fOriginalValue / 3600.0
            elif sOriginalUnit == "std m3 / d":
                valueSI = fOriginalValue / 24.0 / 3600.0
            else:
                valueSI = fOriginalValue
                
            if sDesiredUnit == "std m3 / h":
                return valueSI * 3600.0
            elif sDesiredUnit == "std m3 / d":
                return valueSI * 3600.0 * 24.0
            else:
                return valueSI
        
        else:
            return fOriginalValue
    # =================================== unidades

    def pressed_convert(self):
        file_path_1 = self.entry_flowedit.text()
        file_path_2 = self.entry_json.text()

        # Obter os dados de entrada:
        sFloweditFileToConvert = self.entry_flowedit.text()
        sJsonFile = self.entry_json.text()
        bNotWriteFluidModelToJson = self.rdg_fluid_not_inform.isChecked() 
        bFluidModelBlackOil = self.rdg_fluid_black_oil.isChecked()
        bFluidModelTab = self.rdg_fluid_inform_tab.isChecked()
        bFluidModelCompositional = self.rdg_fluid_compositional_ctm.isChecked()
        sFluidModelFilePath = "aa"
        if bFluidModelTab:
            sFluidModelFilePath = self.txtFluidTabFile.text()
        else:
            if bFluidModelCompositional:
                sFluidModelFilePath = self.txtFluidCTMFile.text()
                
        # Criar a estrutura de dados de entrada:
        oInputInfo = FloweditConversionInputInfo(sFloweditFileToConvert, sJsonFile, bNotWriteFluidModelToJson, bFluidModelBlackOil, 
                                         bFluidModelTab, bFluidModelCompositional, sFluidModelFilePath, False, True)
        
        # Tentar a conversão:
        QApplication.setOverrideCursor(Qt.WaitCursor)
        try:
            obj = FloweditConversion(oInputInfo)
            self.oFlowEditConversionObject = obj
                  # Escrever dados não-oriundos da Flowedit:
            self.WriteNonFloweditInfoToJson()
            QApplication.restoreOverrideCursor()
            QMessageBox.information(self, "Conversão", "Conversão finalizada com sucesso!")
        except Exception as e:
            QApplication.restoreOverrideCursor()
            QMessageBox.critical(self, "Erro", "Erro na conversão do arquivo Flowedit para Json!")

    def browse_file_1(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Selecione um arquivo Flowedit", "", "Arquivos Excel (*.xlsx *.xlsm)")
        if file_path:
            self.entry_flowedit.setText(file_path)

    def browse_file_2(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Selecione um arquivo Json", "", "Arquivos JSON (*.json)")
        if file_path:
            self.entry_json.setText(file_path)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
