class FloweditConversionInputInfo:
    def __init__(self, sFloweditFileToConvert: str, sJsonTemplate: str, bNotWriteFluidModelToJson: bool, 
                 bFluidModelBlackOil: bool, bFluidModelTab: bool,
                 bFluidModelCompositional: bool, sFluidModelFilePath: str, bWriteLog: bool, 
                 bKeepFloweditInitialXY: bool):
        self.sFloweditFileToConvert = sFloweditFileToConvert
        self.sJsonTemplate = sJsonTemplate
        self.bNotWriteFluidModelToJson = bNotWriteFluidModelToJson
        self.bFluidModelBlackOil = bFluidModelBlackOil
        self.bFluidModelTab = bFluidModelTab
        self.bFluidModelCompositional = bFluidModelCompositional
        self.sFluidModelFilePath = sFluidModelFilePath
        self.bWriteLog = bWriteLog
        self.bKeepFloweditInitialXY = bKeepFloweditInitialXY # Origem (0, 0) ou da própria FLOWedit para linhas de produção e serviço