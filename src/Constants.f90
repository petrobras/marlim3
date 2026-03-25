module Constants

    ! OBJETIVO: Centralizar declarações e definições necessários a todo este código.

    use iso_c_binding   ! Para viabilizar a comunicação com C...

    implicit none

! ===================================================================================================
!       CONSTANTES
! ===================================================================================================

    ! Possíveis modelos para as fases líquido e vapor (Equações de Estado, etc)
    integer(c_int), parameter :: WILSON_EQCONST_CORRELATION = 0
    integer(c_int), parameter :: PENG_ROBINSON_78_PENELOUX = 1
    integer(c_int), parameter :: SRK_PENELOUX = 2
    integer(c_int), parameter :: PENG_ROBINSON_PENELOUX = 3

    ! Códigos de possíveis erros:
    integer(c_int), parameter :: ERROR_EverythingOK = 0
    integer(c_int), parameter :: ERROR_InvalidCubicEOS = 1
    integer(c_int), parameter :: ERROR_FoundComplexZInKiCalculation = 2
    integer(c_int), parameter :: ERROR_InvalidKiModeling = 3
    integer(c_int), parameter :: ERROR_FlashCalculationBlewMaxIterations = 4
    integer(c_int), parameter :: ERROR_FlashCalculationFoundTrivialSolution = 5
    integer(c_int), parameter :: ERROR_FlashCalculationGotToBetaOutOfBounds = 6
    integer(c_int), parameter :: ERROR_FlashCalculationFoundNoSolution = 7
    integer(c_int), parameter :: ERROR_ConvergenceProblemInBubblePointCalculation = 8
    integer(c_int), parameter :: ERROR_ConvergenceProblemInDewPointCalculation = 9
    integer(c_int), parameter :: ERROR_NumericalProblemInBubblePointCalculation = 10
    integer(c_int), parameter :: ERROR_NumericalProblemInDewPointCalculation = 11
    integer(c_int), parameter :: ERROR_CouldNotDetermineThermodynamicCondition = 12
    integer(c_int), parameter :: ERROR_InvalidExternalFileForMixtureProperties = 13
    integer(c_int), parameter :: ERROR_FailedToOpenExternalFileForMixtureProperties = 14
    integer(c_int), parameter :: ERROR_ErrorWhileReadingExternalFileForMixtureProperties = 15
    integer(c_int), parameter :: ERROR_UnrecognizedUnitInExternalFileForMixtureProperties = 16
    integer(c_int), parameter :: ERROR_UnrecognizedEOSInExternalFileForMixtureProperties = 17
    integer(c_int), parameter :: ERROR_InvalidDensityCalculationMethod = 18
    integer(c_int), parameter :: ERROR_InvalidEnthalpyOrCpCalculationMethod = 19
    integer(c_int), parameter :: ERROR_InvalidSourceForIGCpOrHCoefs = 20
    integer(c_int), parameter :: ERROR_NoFlashConvergenceInLowerPressures = 21
    integer(c_int), parameter :: ERROR_InconsistentBubbleTAndPStepwiseResult = 22
    integer(c_int), parameter :: ERROR_BasePressureNotFoundInPStepwiseCalc = 23
    integer(c_int), parameter :: ERROR_CouldNotPStepwiselyFindBubbleP = 24
    integer(c_int), parameter :: ERROR_InvalidZOrRhoTOrPDerivativesCalculationMethod = 25
    integer(c_int), parameter :: ERROR_LiquidRequiredButNotDetectedInBlackOilCalculation = 26
    integer(c_int), parameter :: ERROR_BubblePressureConvergedForTemperatureAboveCritical = 27
    integer(c_int), parameter :: ERROR_InvalidThermodynamicMethodForZCalculation = 28
    integer(c_int), parameter :: ERROR_InvalidModelForHelmholtzFCalculation = 29
    integer(c_int), parameter :: ERROR_MinimizationOfTPDFunctionDidNotConverge = 30
    integer(c_int), parameter :: ERROR_FailedToOpenExternalFileForWaxParameters = 31
    integer(c_int), parameter :: ERROR_InvalidExternalFileForWaxParameters = 32
    integer(c_int), parameter :: ERROR_SingularEquationSystemEncountered = 33
    integer(c_int), parameter :: ERROR_WaxInterpolationFoundNoInterval = 34
    integer(c_int), parameter :: ERROR_InvalidRsCorrelation = 35
    integer(c_int), parameter :: ERROR_SatPressureNotFoundWithinPVTAnalysisBounds = 36

    ! Códigos de possíveis tipos de cálculo:
    integer(c_int), parameter :: CALCTYPE_BubblePoint = 0
    integer(c_int), parameter :: CALCTYPE_DewPoint = 1

    ! Códigos de possíveis condições termodinâmicas:
    integer(c_int), parameter :: THERMOCOND_SubcooledLiquid = 0
    integer(c_int), parameter :: THERMOCOND_SaturatedLiquid = 1
    integer(c_int), parameter :: THERMOCOND_LiquidVaporVLE = 2
    integer(c_int), parameter :: THERMOCOND_SaturatedVapor = 3
    integer(c_int), parameter :: THERMOCOND_SuperheatedVapor = 4

    ! Códigos de possíveis fontes de fluido (componentes puros, pseudos, etc):
    integer(c_int), parameter :: FLUIDEXTERNALSOURCE_PVTSIM_CTM = 0

    ! Códigos de possíveis fontes de parâmetros para cálculos de deposição de parafina:
    integer(c_int), parameter :: WAXEXTERNALSOURCE_PVTSIM_WAX = 0

    ! Código de possiveis fases no escoamento (líquido, vapor, etc):
    integer(c_int), parameter :: PHASE_Liquid = 0
    integer(c_int), parameter :: PHASE_Vapor = 1

    ! Código de possíveis correlações "black-oil" para cálculo de "Rs":
    integer(c_int), parameter :: RSCORRELATION_VAZQUEZBEGGS = 0
    integer(c_int), parameter :: RSCORRELATION_LASATER = 1
    integer(c_int), parameter :: RSCORRELATION_STANDING = 2
    integer(c_int), parameter :: RSCORRELATION_GLASO = 3
    integer(c_int), parameter :: RSCORRELATION_COUNT = 4    ! Contagem de correlações de Rs disponíveis!

end module Constants
