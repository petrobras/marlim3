module BlackOilFromCompositional

    ! OBJETIVO: Conter as subrotinas e funcionalidades que permitem calcular propriedades do modelo "black-oil"
    !           a partir de resultados de cálculos composicionais.
    ! UNIDADES: TODA a programação contida neste módulo considera que TODOS os valores (entradas e saídas
    !   das rotinas e suas variáveis internas) estão no SI, EXCETO QUANDO devidamente explicitado...

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use VLECalculations
    use PhaseProperties
    !!use Obsoletes, only: CalculateMixtureThermodynamicCondition
    use FlashExtras

    implicit none

    contains

    ! =============================================================
    ! =============================================================
    subroutine CalculateCommonlyRequiredValuesAtStockTankConditions(iNComp, oZ, oMW, oTc, oPc, oW, oPeneloux, oKij, oLij, iLiqPhaseModel, &
                            iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure, dStockTankTemperature, &
                            bHasInitialFlashEstimates, dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                            iCalculatedStockTankThermodynamicCondition, dStockTankVaporMassFraction, dStockTankLiquidDensity, dStockTankVaporDensity, &
                            dStockTankCompositionalGOR, iIER, oCalculatedStockTankLiqComposition_Arg, oCalculatedStockTankVapComposition_Arg, &
                            dStockTankVaporMW_Arg, dMixtureMW_Arg)

        ! OBJETIVO: Calcular determinados resultados nas condições "stock-tank" de temperatura e pressão
        !           que são frequentemente necessários em outros cálculos.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oMW                 ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod    ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
        integer(c_int), value, intent(in) :: iVapDensityCalculationMethod    ! Índice do método de cálculo da massa específica do vapor, conforme convenção.
        real(c_double), value, intent(in) :: dStockTankPressure              ! Pressão correspondente à condição "stock-tank".
        real(c_double), value, intent(in) :: dStockTankTemperature           ! Temperatura correspondente à condição "stock-tank".
        logical, intent(in) :: bHasInitialFlashEstimates                     ! "True" caso se deseje fornecer estimativas iniciais para o cálculo de "flash".
                                                                             ! "False" para a própria rotina calcular essas estimativas.
        real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash".
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialLiqComposition    ! Estimativa inicial de composição da fase líquida para o "flash".
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialVapComposition    ! Estimativa inicial de composição da fase vapor para o "flash".

        integer(c_int), intent(out) :: iCalculatedStockTankThermodynamicCondition     ! Código da condição termodinâmica determinada nas condições "stock-tank", conforme convencionado.
        real(c_double), intent(out) :: dStockTankVaporMassFraction           ! Fração mássica vaporizada nas condições "stock-tank".
        real(c_double), intent(out) :: dStockTankLiquidDensity               ! Massa específica calculada para a fase líquida nas condições "stock-tank".
        real(c_double), intent(out) :: dStockTankVaporDensity                ! Massa específica calculada para a fase vapor nas condições "stock-tank".
        real(c_double), intent(out) :: dStockTankCompositionalGOR            ! "Rgo" (ADIMENSIONAL) obtida nas condições "stock-tank" por cálculos composicionais.
        integer(c_int), intent(out) :: iIER                                  ! Código de erros "geral" desta rotina, conforme convencionado.

        real(c_double), dimension(iNComp), intent(out), optional :: oCalculatedStockTankLiqComposition_Arg
        real(c_double), dimension(iNComp), intent(out), optional :: oCalculatedStockTankVapComposition_Arg
        real(c_double), intent(out), optional :: dStockTankVaporMW_Arg                     ! Massa molar da fase vapor.
        real(c_double), intent(out), optional :: dMixtureMW_Arg                            ! Massa molar da mistura.


        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dCalculatedStockTankBubbleT                        ! Valor calculado da temperatura de bolha.
        integer(c_int) :: iIER_BubbleT                          ! Código de erros na determinação da temperatura de bolha, conforme convencionado.
        real(c_double) :: dCalculatedStockTankDewT              ! Valor calculado da temperatura de orvalho.
        integer(c_int) :: iIER_DewT                             ! Código de erros na determinação da temperatura de orvalho, conforme convencionado.
        real(c_double) :: dCalculatedStockTankBeta              ! Fração molar vaporizada determinada nas condições "stock-tank".
        real(c_double), dimension(iNComp) :: oCalculatedStockTankLiqComposition     ! Composição molar determinada da fase líquida nas condições "stock-tank".
        real(c_double), dimension(iNComp) :: oCalculatedStockTankVapComposition     ! Composição molar determinada da fase vapor nas condições "stock-tank".
        integer(c_int) :: iIER_Flash                            ! Código de erros no cálculo do "flash", conforme convencionado.
        real(c_double) :: dCalculatedStockTankBubbleP           ! Pressão de bolha calculada nas condições "stock-tank".
        integer(c_int) :: iIER_BubbleP                          ! Código de erros no cálculo da pressão de bolha, conforme convencionado.
        real(c_double) :: dStockTankLiquidMW                    ! Massa molar da fase líquida (g/gmol).
        real(c_double) :: dStockTankVaporMW                     ! Massa molar da fase vapor (g/gmol).
        real(c_double) :: dMixtureMW                            ! Massa molar da mistura (g/gmol).
        logical :: bLiquidPresentAtStockTankConditions          ! "True" caso seja prevista a existência de fase líquida nas "stock-tank" conditions.
        logical :: bVaporPresentAtStockTankConditions           ! "True" caso seja prevista a existência de fase vapor nas "stock-tank" conditions.
        integer :: i

        ! ------------------ CÁLCULOS:

        ! PASSO 1: Calcular a condição termodinâmica da mistura nas condições "stock-tank":
        call CalculateMixtureThermodynamicCondition_V3(dStockTankPressure, dStockTankTemperature, iNComp, oMW, &
           oZ, oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, &
           bHasInitialFlashEstimates, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
           dCalculatedStockTankBeta, oCalculatedStockTankLiqComposition, oCalculatedStockTankVapComposition, &
           iCalculatedStockTankThermodynamicCondition, iIER_Flash, iIER)

        ! ===================================
        !   10-JUL-2023 - COMMENT OUT

        ! Bloco de código comentado em favor da chamada otimizada acima
        !call CalculateMixtureThermodynamicCondition_V2(dStockTankPressure, dStockTankTemperature, iNComp, oMW, oZ, oTc, oPc, oW, &
        !                oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, bHasInitialFlashEstimates, dGivenInitialBeta, &
        !                oGivenInitialLiqComposition, oGivenInitialVapComposition, dCalculatedStockTankBeta, oCalculatedStockTankLiqComposition, &
        !                oCalculatedStockTankVapComposition, iCalculatedStockTankThermodynamicCondition, iIER_Flash, iIER)
        ! ===================================
        ! ===================================

        ! ==================================================
        !   21/12/2021 - COMMENT OUT

        !   Bloco de código comentado em favor da chamada otimizada acima

        !call CalculateMixtureThermodynamicCondition(dStockTankPressure, dStockTankTemperature, iNComp, oZ, oTc, oPc, oW, &
        !            oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
        !            bHasInitialFlashEstimates, dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
        !            dCalculatedStockTankBubbleT, iIER_BubbleT, &
        !            dCalculatedStockTankDewT, iIER_DewT, dCalculatedStockTankBeta, &
        !            oCalculatedStockTankLiqComposition, oCalculatedStockTankVapComposition, iIER_Flash, &
        !            dCalculatedStockTankBubbleP, iIER_BubbleP, iCalculatedStockTankThermodynamicCondition, iIER)

        ! =============================================
        !   INSERÇÃO EM 16/11/2021
        !goAroundError: if(iIER.NE.ERROR_EverythingOK) then

            ! Antecipar a massa molar da mistura para possibilitar verificação:
        !    dMixtureMW = 0.0d0
        !    do i = 1, iNComp
        !        dMixtureMW = dMixtureMW + (oZ(i) * oMW(i))
        !    end do

        !    errorTreatment: if(dMixtureMW.LT.(47.0d0)) then
                ! Fluido muito leve! Considerar tudo gás em condições padrão!
        !        dCalculatedStockTankBeta = 1.0d0
        !        oCalculatedStockTankVapComposition = oZ
        !        iIER_Flash = ERROR_EverythingOK
        !        iCalculatedStockTankThermodynamicCondition = THERMOCOND_SuperheatedVapor
        !        iIER = ERROR_EverythingOK
        !    end if errorTreatment

        !end if goAroundError
        !   FIM DA INSERÇÃO EM 16/11/2021
        ! =============================================

        ! 21/12/2021 - FIM DO "COMMENT OUT"
        ! =============================================

        if(iIER.NE.ERROR_EverythingOK) return

        ! Retornar, se for o caso, a composição determinada das fases nas condições padrão:
        if(present(oCalculatedStockTankLiqComposition_Arg)) oCalculatedStockTankLiqComposition_Arg = oCalculatedStockTankLiqComposition
        if(present(oCalculatedStockTankVapComposition_Arg)) oCalculatedStockTankVapComposition_Arg = oCalculatedStockTankVapComposition

        ! PASSO 2: Converter a fração molar vaporizada para fração mássica vaporizada:
        call CalculateMolecularWeightsAndVaporMassFraction(iCalculatedStockTankThermodynamicCondition, iNComp, oMW, oZ, oCalculatedStockTankLiqComposition, &
                oCalculatedStockTankVapComposition, dCalculatedStockTankBeta, dStockTankLiquidMW, dStockTankVaporMW, &
                dMixtureMW, dStockTankVaporMassFraction)

        ! Retornar argumentos opcionais, caso presentes:
        if(present(dStockTankVaporMW_Arg)) dStockTankVaporMW_Arg = dStockTankVaporMW
        if(present(dMixtureMW_Arg)) dMixtureMW_Arg = dMixtureMW

        ! PASSO 3: Determinar que fases estão presentes nas condições "stock-tank":
        bLiquidPresentAtStockTankConditions = .not.(iCalculatedStockTankThermodynamicCondition.EQ.THERMOCOND_SuperheatedVapor)
        bVaporPresentAtStockTankConditions = .not.(iCalculatedStockTankThermodynamicCondition.EQ.THERMOCOND_SubcooledLiquid)

        ! PASSO 4: Calcular a massa específica da fase líquida nas condições "stock-tank", se possível:
        isThereLiqForRho: if(bLiquidPresentAtStockTankConditions) then

            ! Calcular a massa específica da fase líquida:
            call CalculatePhaseDensity(iNComp, PHASE_Liquid, oCalculatedStockTankLiqComposition, dStockTankPressure, dStockTankTemperature, &
                      iLiqDensityCalculationMethod, oTc, oPc, oW, oKij, oLij, &
                      oPeneloux, dStockTankLiquidMW, dStockTankLiquidDensity, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

        end if isThereLiqForRho

        ! PASSO 5: Calcular a massa específica da fase vapor nas condições "stock-tank", se possível:
        isThereVapForRho: if(bVaporPresentAtStockTankConditions) then

            ! Calcular a massa específica da fase vapor:
            call CalculatePhaseDensity(iNComp, PHASE_Vapor, oCalculatedStockTankVapComposition, dStockTankPressure, dStockTankTemperature, &
                      iVapDensityCalculationMethod, oTc, oPc, oW, oKij, oLij, &
                      oPeneloux, dStockTankVaporMW, dStockTankVaporDensity, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

        end if isThereVapForRho

        ! PASSO 6: Calcular a GOR ("Gas/oil ratio" - "Rgo")
        calcGOR:if((.not.bLiquidPresentAtStockTankConditions).OR.(iCalculatedStockTankThermodynamicCondition.EQ.THERMOCOND_SaturatedVapor)) then

            ! Erro! Não é possível calcular a "Rgo" na ausência de fase líquida!
            iIER = ERROR_LiquidRequiredButNotDetectedInBlackOilCalculation
            dStockTankCompositionalGOR = 1.0d8
            return

        else if((.not.bVaporPresentAtStockTankConditions).OR.(iCalculatedStockTankThermodynamicCondition.EQ.THERMOCOND_SaturatedLiquid)) then calcGOR

            ! Na ausência de vapor...
            dStockTankCompositionalGOR = 0.0d0

        else calcGOR

            ! Ambas as fases presentes (em frações não-nulas)!
            dStockTankCompositionalGOR = dStockTankVaporMassFraction / (1.0d0 - dStockTankVaporMassFraction)
            dStockTankCompositionalGOR = dStockTankCompositionalGOR * (dStockTankLiquidDensity / dStockTankVaporDensity)

        end if calcGOR

    end subroutine CalculateCommonlyRequiredValuesAtStockTankConditions

    ! =============================================================
    ! =============================================================
    subroutine CalculateSolutionGasOilRatio(dVaporMassFraction, dStockTankVaporMassFraction, dStockTankLiquidDensity, dStockTankVaporDensity, &
                        dCompositionalSolutionGasOilRatio)

        ! OBJETIVO: Calcular o "Rs" a partir de resultados composicionais.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dVaporMassFraction                 ! Fração mássica vaporizada à P e T locais.
        real(c_double), value, intent(in) :: dStockTankVaporMassFraction        ! Fração mássica vaporizada nas condições "stock-tank".
        real(c_double), value, intent(in) :: dStockTankLiquidDensity            ! Massa específica calculada para a fase líquida nas condições "stock-tank".
        real(c_double), value, intent(in) :: dStockTankVaporDensity             ! Massa específica calculada para a fase vapor nas condições "stock-tank".

        real(c_double), intent(out) :: dCompositionalSolutionGasOilRatio        ! "Rs" ADIMENSIONAL (m3/m3) calculado pela via composicional.

        ! ------------------ CÁLCULOS:

        ! Calcular e retornar o "Rs":
        dCompositionalSolutionGasOilRatio = dStockTankLiquidDensity / dStockTankVaporDensity
        dCompositionalSolutionGasOilRatio = dCompositionalSolutionGasOilRatio * (dStockTankVaporMassFraction - dVaporMassFraction)
        dCompositionalSolutionGasOilRatio = dCompositionalSolutionGasOilRatio / (1.0d0 - dStockTankVaporMassFraction)

    end subroutine CalculateSolutionGasOilRatio

    ! =============================================================
    ! =============================================================
    subroutine CalculateOilFormationVolumeFactor(dLiquidDensity, dStockTankLiquidDensity, dVaporMassFraction, dStockTankVaporMassFraction, &
                    dCompositionalOilFormationVolumeFactor)

        ! OBJETIVO: Calcular o "Bo" a partir de resultados composicionais.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dLiquidDensity                     ! Massa específica calculada para a fase líquida a P e T locais.
        real(c_double), value, intent(in) :: dStockTankLiquidDensity            ! Massa específica calculada para a fase líquida nas condições "stock-tank".
        real(c_double), value, intent(in) :: dVaporMassFraction                 ! Fração mássica vaporizada à P e T locais.
        real(c_double), value, intent(in) :: dStockTankVaporMassFraction        ! Fração mássica vaporizada nas condições "stock-tank".

        real(c_double), intent(out) :: dCompositionalOilFormationVolumeFactor       ! "Bo" ADIMENSIONAL (m3/m3) calculado pela via composicional.

        ! ------------------ CÁLCULOS:

        ! Calcular e retornar o "Bo":
        dCompositionalOilFormationVolumeFactor = dStockTankLiquidDensity / dLiquidDensity
        dCompositionalOilFormationVolumeFactor = dCompositionalOilFormationVolumeFactor * (1.0d0 - dVaporMassFraction)
        dCompositionalOilFormationVolumeFactor = dCompositionalOilFormationVolumeFactor / (1.0d0 - dStockTankVaporMassFraction)

    end subroutine CalculateOilFormationVolumeFactor

    ! =============================================================
    ! =============================================================
    subroutine AdjustGlobalCompositionForTargetGOR_Approach1(iNComp, oOriginalGlobalComposition, oMW, oTc, oPc, oW, oPeneloux, oKij, oLij, &
                    iLiqPhaseModel, iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure, &
                    dStockTankTemperature, dTargetGOR, bHasInitialFlashEstimates, dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                    oGORAdjustedGlobalComp, dAdjustedGlobalCompGOR, iIER)

        ! OBJETIVO: Ajustar uma composição total fornecida de fluido para que passe a possuir um dado valor de RGO.

        ! Isto poderia ser conseguido de diversas formas. Na abordagem aplicada nesta subrotina, calcula-se para a composição
        ! global original as composições das fases líquido e vapor nas condições padrão. Em seguida, busca-se a proporção molar
        ! em que estas composições sejam recombinadas para originar uma composição global cujo RGO corresponda ao desejado.

        ! Trata-se do mesmo procedimento descrito no "help" do OLGA 7.3.3, em "General: GOR Adjustments", sob "Standard Conditions in
        ! OLGA", onde se lê o seguinte: "OLGA allows the GOR [...] of a fluid to be adjusted by user input [...]. In Compositional Tracking,
        ! this adjustment is performed by flashing the feed fluid at standard conditions and recombining the resulting phases according to the
        ! specified input value, thus changing the composition of the fluid source."

        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oOriginalGlobalComposition     ! Vetor composição global original (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oMW                 ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod    ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
        integer(c_int), value, intent(in) :: iVapDensityCalculationMethod    ! Índice do método de cálculo da massa específica do vapor, conforme convenção.
        real(c_double), value, intent(in) :: dStockTankPressure              ! Pressão correspondente à condição "stock-tank".
        real(c_double), value, intent(in) :: dStockTankTemperature           ! Temperatura correspondente à condição "stock-tank".
        real(c_double), value, intent(in) :: dTargetGOR                      ! Valor desejado de RGO
        logical, intent(in) :: bHasInitialFlashEstimates                     ! "True" caso se deseje fornecer estimativas iniciais para cálculos de "flash".
                                                                             ! "False" para a própria rotina calcular essas estimativas.
        real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash".
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialLiqComposition    ! Estimativa inicial de composição da fase líquida para o "flash".
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialVapComposition    ! Estimativa inicial de composição da fase vapor para o "flash".

        real(c_double), dimension(iNComp), intent(out) :: oGORAdjustedGlobalComp    ! Composição global ajustada para a "RGO" solicitada.
        real(c_double), intent(out) :: dAdjustedGlobalCompGOR                       ! RGO obtida para a composição global ajustada.
        integer(c_int), intent(out) :: iIER                                         ! Código de erros "geral" desta rotina, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer(c_int) :: iCalculatedStockTankThermodynamicCondition     ! Código da condição termodinâmica determinada nas condições "stock-tank", conforme convencionado.
        real(c_double) :: dStockTankVaporMassFraction                    ! Fração mássica vaporizada nas condições "stock-tank".
        real(c_double) :: dStockTankOriginalLiqDensity               ! Massa específica calculada para a fase líquida original nas condições "stock-tank".
        real(c_double) :: dStockTankOriginalVapDensity               ! Massa específica calculada para a fase vapor original nas condições "stock-tank".
        real(c_double) :: dOriginalCalculatedGOR            ! "Rgo" (ADIMENSIONAL) obtida nas condições "stock-tank" para a composição original.
        real(c_double), dimension(iNComp) :: oOriginalStockTankLiqComposition
        real(c_double), dimension(iNComp) :: oOriginalStockTankVapComposition
        real(c_double) :: dOriginalStockTankVaporMW, dOriginalMixtureMW
        real(c_double) :: dInitialEstForRecombRatio, dCurrentRecombRatio
        integer :: iIter
        real(c_double), dimension(iNComp) :: oNewAttemptGlobalComposition
        integer(c_int) :: iNewlyCalculatedStockTankThermodynamicCondition
        real(c_double) :: dStockTankNewLiqDensity, dStockTankNewVapDensity, dNewCalculatedGOR
        real(c_double) :: dPreviousRecombRatio, dPreviousGOR, dA, dB

        ! ------------------ CONSTANTES:
        integer, parameter :: iMaxIterations = 200
        real(c_double), parameter :: dTolOnGORDiff = 0.1d0

        ! ------------------ CÁLCULOS:

        ! PRIMEIRO PASSO: Fazer cálculos iniciais nas condições padrão:
        call CalculateCommonlyRequiredValuesAtStockTankConditions(iNComp, oOriginalGlobalComposition, oMW, oTc, oPc, oW, oPeneloux, oKij, oLij, iLiqPhaseModel, &
                            iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure, dStockTankTemperature, &
                            bHasInitialFlashEstimates, dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                            iCalculatedStockTankThermodynamicCondition, dStockTankVaporMassFraction, dStockTankOriginalLiqDensity, dStockTankOriginalVapDensity, &
                            dOriginalCalculatedGOR, iIER, oCalculatedStockTankLiqComposition_Arg = oOriginalStockTankLiqComposition, &
                            oCalculatedStockTankVapComposition_Arg = oOriginalStockTankVapComposition, &
                            dStockTankVaporMW_Arg = dOriginalStockTankVaporMW, dMixtureMW_Arg = dOriginalMixtureMW)

        if(iIER.NE.ERROR_EverythingOK) return

        stdConditionsHasTwoPhases: if(iCalculatedStockTankThermodynamicCondition.EQ.THERMOCOND_LiquidVaporVLE) then

            ! Duas fases (líquida e vapor) encontradas nas condições padrão.

            ! Estimar a razão de recombinação das duas fases necessária para obter o RGO desejado:
            dInitialEstForRecombRatio = dTargetGOR * dStockTankOriginalVapDensity / dStockTankOriginalLiqDensity
            dInitialEstForRecombRatio = dInitialEstForRecombRatio / (1.0d0 + dInitialEstForRecombRatio)
            dInitialEstForRecombRatio = dInitialEstForRecombRatio * (dOriginalMixtureMW / dOriginalStockTankVaporMW)

            ! TESTE 7-FEV-2024:
            ! if(dInitialEstForRecombRatio.GT.(1.0d0)) dInitialEstForRecombRatio = 0.85d0

            ! Preparar procedimento iterativo:
            dCurrentRecombRatio = dInitialEstForRecombRatio
            dPreviousRecombRatio = dStockTankVaporMassFraction * (dOriginalMixtureMW / dOriginalStockTankVaporMW)
            dPreviousGOR = dOriginalCalculatedGOR

            ! Iniciar procedimento iterativo:
            adjustRecombRatioLoop: do iIter = 1, iMaxIterations

                ! TESTE 16-FEV-24: impedir surgimento de recombinações anômalas:
                if(dCurrentRecombRatio.gt.(1.0d0)) dCurrentRecombRatio = 0.5d0 * (1.0d0 + dPreviousRecombRatio)

                ! Obter a nova tentativa de composição global da mistura:
                oNewAttemptGlobalComposition = (dCurrentRecombRatio * oOriginalStockTankVapComposition) + &
                             ((1.0d0 - dCurrentRecombRatio) * oOriginalStockTankLiqComposition)

                oNewAttemptGlobalComposition = oNewAttemptGlobalComposition / sum(oNewAttemptGlobalComposition)

                ! Obter a RGO dessa nova composição global:
                call CalculateCommonlyRequiredValuesAtStockTankConditions(iNComp, oNewAttemptGlobalComposition, oMW, oTc, oPc, oW, oPeneloux, oKij, oLij, iLiqPhaseModel, &
                            iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure, dStockTankTemperature, &
                            bHasInitialFlashEstimates, dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                            iNewlyCalculatedStockTankThermodynamicCondition, dStockTankVaporMassFraction, dStockTankNewLiqDensity, dStockTankNewVapDensity, &
                            dNewCalculatedGOR, iIER)

                if(iIER.NE.ERROR_EverythingOK) exit adjustRecombRatioLoop

                ! Testar se já se atingiu convergência no RGO:
                newRecombRatioHasTwoPhases: if(iNewlyCalculatedStockTankThermodynamicCondition.NE.THERMOCOND_LiquidVaporVLE) then
                    exit adjustRecombRatioLoop
                else newRecombRatioHasTwoPhases

                    hasConvergenceOnGOR: if(abs(dNewCalculatedGOR - dTargetGOR).LE.dTolOnGORDiff) then
                        ! Cálculo convergiu com sucesso!
                        ! Retornar os resultados daqui mesmo.

                        oGORAdjustedGlobalComp = oNewAttemptGlobalComposition
                        dAdjustedGlobalCompGOR = dNewCalculatedGOR
                        return

                    else hasConvergenceOnGOR
                        ! Cálculo ainda não convergiu!
                        ! Preparar para a próxima iteração:
                        dA = (dCurrentRecombRatio - dPreviousRecombRatio) / (dNewCalculatedGOR - dPreviousGOR)
                        dB = dCurrentRecombRatio - (dA * dNewCalculatedGOR)

                        dPreviousRecombRatio = dCurrentRecombRatio
                        dPreviousGOR = dNewCalculatedGOR

                        dCurrentRecombRatio = (dA * dTargetGOR) + dB

                    end if hasConvergenceOnGOR

                end if newRecombRatioHasTwoPhases

            end do adjustRecombRatioLoop

        end if stdConditionsHasTwoPhases

        ! Manter a composição original:
        oGORAdjustedGlobalComp = oOriginalGlobalComposition
        dAdjustedGlobalCompGOR = dOriginalCalculatedGOR

    end subroutine AdjustGlobalCompositionForTargetGOR_Approach1

end module BlackOilFromCompositional
