module VLECalculations

    ! OBJETIVO: Conter as rotinas de cálculo de Equilíbrio Líquido-Vapor.
    ! UNIDADES: TODA a programação contida neste módulo considera que TODOS os valores (entradas e saídas
    !   das rotinas e suas variáveis internas) estão no SI, EXCETO QUANDO devidamente explicitado...

    use iso_c_binding   ! Para viabilizar a comunicação com C.....
    use Constants

    implicit none

    contains

    ! =============================================================
    ! =============================================================
    subroutine CalculateMixtureThermodynamicCondition_V2(dFlashPressure, dFlashTemperature, iNComp, oMW, oZ, oTc, oPc, oW, &
                        oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, bHasInitialFlashEstimates, dGivenInitialBeta, &
                        oGivenInitialLiqComposition, oGivenInitialVapComposition, dCalculatedBeta, oCalculatedLiqComposition, &
                        oCalculatedVapComposition, iCalculatedThermodynamicCondition, iIER_Flash, iIER)

        ! OBJETIVO: Determinar a condição termodinâmica de uma mistura especificada sob dada condições de T e P.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMW                 ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        logical, intent(in) :: bHasInitialFlashEstimates                     ! "True" caso se deseje fornecer estimativas iniciais para o cálculo de "flash".
                                                                             ! "False" para a própria rotina calcular essas estimativas.
        real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash".
        real(c_double), dimension(iNComp), intent(in) :: oGivenInitialLiqComposition    ! Estimativa inicial de composição da fase líquida para o "flash".
        real(c_double), dimension(iNComp), intent(in) :: oGivenInitialVapComposition    ! Estimativa inicial de composição da fase vapor para o "flash".


        real(c_double), intent(out) :: dCalculatedBeta                              ! Fração molar vaporizada determinada.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedLiqComposition ! Composição molar determinada da fase líquida.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedVapComposition ! Composição molar determinada da fase vapor.
        integer(c_int), intent(out) :: iCalculatedThermodynamicCondition            ! Código da condição termodinâmica determinada, conforme convencionado.
        integer(c_int), intent(out) :: iIER_Flash                            ! Código de erros no cálculo do "flash", conforme convencionado.
        integer(c_int), intent(out) :: iIER                                         ! Código de erros "geral" desta rotina, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bHasInitialEstimates                             ! "True" caso possua chutes iniciais para o processo iterativo do "flash".
        real(c_double) :: dInitialBeta                              ! Estimativa inicial da fração molar vaporizada, caso exista.
        real(c_double), dimension(iNComp) :: oInitialLiqComposition ! Estimativa inicial da composição da fase líquida, caso exista.
        real(c_double), dimension(iNComp) :: oInitialVapComposition ! Estimativa inicial da composição da fase vapor, caso exista.
        logical :: bGotEstimateForBubblePoint                                   ! "True" se conseguir estimativas para o ponto de bolha.
        real(c_double) :: dClosestToBubbleT                                     ! Temperatura mais próxima encontrada da temperatura de bolha.
        real(c_double), dimension(iNComp) :: oVapCompositionClosestToBubbleT    ! Composição da fase vapor correspondente a "dClosestToBubbleT".
        logical :: bGotEstimateForDewPoint                                      ! "True" se conseguir estimativas para o ponto de orvalho.
        real(c_double) :: dClosestToDewT                                        ! Temperatura mais próxima encontrada da temperatura de orvalho.
        real(c_double), dimension(iNComp) :: oLiqCompositionClosestToDewT       ! Composição da fase líquida correspondente a "dClosestToDewT".
        logical :: bPStepwiseDetectedSubcooledLiq               ! "True" se o cálculo "stepwise" na pressão determinou que a mistura está subresfriada.
        real(c_double) :: dPStepwiseRoughBubblePEstimate        ! Estimativa da pressão de bolha encontrada pelo cálculo "pressure stepwise".
        logical :: bErrorInFlashCalculation
        logical :: bFlashResultOkAndTwoPhase
        logical :: bUnblockBubbleAndDewCalculations
        integer(c_int) :: iIER_BubbleP                          ! Código de erros no cálculo da pressão de bolha, conforme convencionado.
        real(c_double) :: dCalculatedBubbleP                    ! Pressão de bolha calculada.
        real(c_double) :: dBubblePObjectiveFunction             ! Valor da função objetivo na pressão de bolha calculada (nem sempre disponível)
        integer(c_int) :: iIER_DewP                             ! Indicador de erros no cálculo da pressão de orvalho, conforme convenção.
        real(c_double) :: dCalculatedDewP                       ! Pressão de Orvalho calculada, se for o caso.
        real(c_double) :: dDewPObjectiveFunction                ! Valor da função objetivo correspondente à pressão de orvalho obtida
        logical :: bBubblePWasCalculatedOk
        logical :: bDewPWasCalculatedOk
        logical :: bGetThermoCondFromEstimates
        logical :: bFoundFlashPBelowBubbleP
        logical :: bFoundFlashPBelowDewP
        logical :: bUseSubcooledLiqEstimate
        logical :: bFoundEstimateForThermoCond
        real(c_double) :: dMixtureMW
        integer :: i
        logical :: bUseSuperheatedVapEstimate
        logical :: bFoundFlashPAboveBubbleP
        logical :: bFoundFlashPAboveDewP

        ! ================================================
        !   INSERÇÃO EM 4/2/2022
        real(c_double) :: dPseudoCritP                          ! Pressão pseudocrítica estimada da mistura
        real(c_double) :: dPseudoCritT                          ! Temperatura pseudocrítica estimada da mistura
        real(c_double) :: dVeryLightsMoleFraction               ! Fração molar total de componentes muito leves na mistura
        !   FIM DA INSERÇÃO EM 4/2/2022
        ! ================================================

        ! ------------------ CÁLCULOS:

        ! AJUSTAR ESTIMATIVAS INICIAIS:
        hasGivenInitialEstimates: if(bHasInitialFlashEstimates) then

            ! Usar as estimativas iniciais fornecidas:
            bHasInitialEstimates = .true.
            dInitialBeta = dGivenInitialBeta
            oInitialLiqComposition = oGivenInitialLiqComposition
            oInitialVapComposition = oGivenInitialVapComposition

            bGotEstimateForBubblePoint = .true.
            dClosestToBubbleT = dFlashTemperature
            oVapCompositionClosestToBubbleT = oGivenInitialVapComposition

            bGotEstimateForDewPoint = .true.
            dClosestToDewT = dFlashTemperature
            oLiqCompositionClosestToDewT = oGivenInitialLiqComposition

        else hasGivenInitialEstimates

            ! Tentar calcular estimativas iniciais rigorosas:
            bGotEstimateForBubblePoint = .false.
            bGotEstimateForDewPoint = .false.
            bHasInitialEstimates = .false.

            call GenerateVLEExtremaInitialEstimates(dFlashPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, oPeneloux, &
                            bGotEstimateForBubblePoint, dClosestToBubbleT, oVapCompositionClosestToBubbleT, bGotEstimateForDewPoint, dClosestToDewT, &
                            oLiqCompositionClosestToDewT, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition)

        end if hasGivenInitialEstimates

        ! TENTAR CALCULAR O "FLASH".
        bPStepwiseDetectedSubcooledLiq = .false.
        iIER_Flash = ERROR_EverythingOK

        call CalcFlashPTVLE(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition, dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, iIER_Flash)

        attemptFlashPStepwise: if(iIER_Flash.NE.ERROR_EverythingOK) then

            ! Verificar se o cálculo com pressões sucessivas funciona:
            iIER_Flash = ERROR_EverythingOK

            call CalcPressureStepwiseFlashPTVLE(-10.0d0, dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, &
                   iLiqPhaseModel, iVapPhaseModel, oPeneloux, .FALSE., dCalculatedBeta, &
                   oCalculatedLiqComposition, oCalculatedVapComposition, bPStepwiseDetectedSubcooledLiq, dPStepwiseRoughBubblePEstimate, iIER_Flash)

        end if attemptFlashPStepwise

        ! Determinar indicadores para a sequência:
        bErrorInFlashCalculation = (iIER_Flash.NE.ERROR_EverythingOK)

            ! Na presente abordagem, não é desejável detectar líquido subresfriado pela "flag" abaixo...
            bErrorInFlashCalculation = bErrorInFlashCalculation.OR.bPStepwiseDetectedSubcooledLiq

        bFlashResultOkAndTwoPhase = (.NOT.bErrorInFlashCalculation).and.(dCalculatedBeta.GE.(0.0d0)).and.(dCalculatedBeta.LE.(1.0d0))

        ! TRATAR POSSÍVEIS ERROS NO CÁLCULO DO "FLASH":
        bBubblePWasCalculatedOk = .false.
        bDewPWasCalculatedOk = .false.
        bGetThermoCondFromEstimates = .false.
        bFoundFlashPBelowBubbleP = .false.
        bUseSubcooledLiqEstimate = .false.
        bFoundFlashPAboveBubbleP = .false.
        bUseSuperheatedVapEstimate = .false.
        bFoundFlashPAboveDewP = .false.
        bFoundFlashPBelowDewP = .false.

        treatFlashError: if(bErrorInFlashCalculation) then

            ! ================================================
            !   INSERÇÃO EM 4/2/2022

            ! Calcular variáveis auxiliares para a sequência:
            dPseudoCritP = 0.0d0
            dPseudoCritT = 0.0d0
            dVeryLightsMoleFraction = 0.0d0

            auxLoop: do i = 1, iNComp
                dPseudoCritP = dPseudoCritP + (oZ(i) * oPc(i))
                dPseudoCritT = dPseudoCritT + (oZ(i) * oTc(i))
                if(oMW(i).LT.(31.0d0)) dVeryLightsMoleFraction = dVeryLightsMoleFraction + oZ(i)
            end do auxLoop

            !   FIM DA INSERÇÃO EM 4/2/2022
            ! ================================================


            ! Possível bloqueio a cálculos de bolha e orvalho:
            bUnblockBubbleAndDewCalculations = .true.

            bubbleAndDewCalcs: if(bUnblockBubbleAndDewCalculations) then

                ! Tentar calcular primeiro a pressão de bolha:
                iIER_BubbleP = ERROR_EverythingOK

                call SecantMethod_CalculateBubbleOrDewPressure(-10.0d0, -10.0d0, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, bGotEstimateForBubblePoint, &
                    oVapCompositionClosestToBubbleT, dCalculatedBubbleP, dBubblePObjectiveFunction, iIER_BubbleP)

            ! ================================================
            !   INSERÇÃO EM 4/2/2022

            ! Verificar se eventual convergência de P Bolha é plausível:
            if(iIER_BubbleP.EQ.ERROR_EverythingOK) then
                if(dVeryLightsMoleFraction.GT.(0.7d0)) then
                    ! Situação inusitada em que a pressão de bolha converge para uma mistura levíssima...
                    if(dFlashTemperature.GT.(dPseudoCritT + 45.0d0)) then
                        ! Critério arbitrário: Invalidar a pressão de bolha calculada para os fins desta rotina.
                        iIER_BubbleP = ERROR_BubblePressureConvergedForTemperatureAboveCritical
                    end if
                end if
            end if

            !   FIM DA INSERÇÃO EM 4/2/2022
            ! ================================================

                bBubblePWasCalculatedOk = (iIER_BubbleP.EQ.ERROR_EverythingOK)
                bFoundFlashPBelowBubbleP = bBubblePWasCalculatedOk.AND.(dFlashPressure.LT.dCalculatedBubbleP)
                bFoundFlashPAboveBubbleP = bBubblePWasCalculatedOk.AND.(dFlashPressure.GT.dCalculatedBubbleP)

                ! Tentar o cálculo da pressão de orvalho, se for o caso:
                calculatingDewP: if(iIER_BubbleP.NE.ERROR_EverythingOK) then

                    iIER_DewP = ERROR_EverythingOK

                    call SecantMethod_CalculateBubbleOrDewPressure(-10.0d0, -10.0d0, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                        oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_DewPoint, bGotEstimateForDewPoint, &
                        oLiqCompositionClosestToDewT, dCalculatedDewP, dDewPObjectiveFunction, iIER_DewP)

                    bDewPWasCalculatedOk = (iIER_DewP.EQ.ERROR_EverythingOK)
                    bFoundFlashPBelowDewP = bDewPWasCalculatedOk.AND.(dFlashPressure.LT.dCalculatedDewP)
                    bFoundFlashPAboveDewP = bDewPWasCalculatedOk.AND.(dFlashPressure.GT.dCalculatedDewP)

                end if calculatingDewP

            end if bubbleAndDewCalcs

            ! Aproximações serão necessárias?
            bGetThermoCondFromEstimates = ((.not.bBubblePWasCalculatedOk).and.(.not.bDewPWasCalculatedOk)) &
                .OR.(bFoundFlashPBelowBubbleP.or.bFoundFlashPBelowDewP)

            makeEstimatesForThermoCond: if(bGetThermoCondFromEstimates) then

                bFoundEstimateForThermoCond = .false.   ! Inicializando...

                estimateByBubbleP: if(bFoundFlashPBelowBubbleP) then
                    ! Estamos dentro do envelope de fases, mas já sabemos que o "flash" não funcionou.
                    ! Dependendo de quão próximos estivermos da pressão de bolha, considerar líquido sub-resfriado.
                    bUseSubcooledLiqEstimate = (iIER_Flash.EQ.ERROR_FlashCalculationFoundTrivialSolution).AND. &
                        (abs((dFlashPressure-dCalculatedBubbleP)/dCalculatedBubbleP).LT.(0.04d0))

                    bFoundEstimateForThermoCond = bUseSubcooledLiqEstimate

                end if estimateByBubbleP

                estimateByMW: if(.not.bFoundEstimateForThermoCond) then

                    ! Calcular a massa molar da mistura:
                    dMixtureMW = 0.0d0
                    do i = 1, iNComp
                        dMixtureMW = dMixtureMW + (oZ(i) * oMW(i))
                    end do

                    ! Aproximar por vapor superaquecido?
                    bUseSuperheatedVapEstimate = (dMixtureMW.LE.(36.0d0))
                    bFoundEstimateForThermoCond = bUseSuperheatedVapEstimate

                end if estimateByMW

                ! Se nenhuma aproximação funcionar, retornar o erro:
                if(.not.bFoundEstimateForThermoCond) then
                    iIER = iIER_Flash
                    return
                end if

            end if makeEstimatesForThermoCond

        end if treatFlashError

        ! DETERMINAR CONDIÇÃO TERMODINÂMICA:
        whichThermoCond: if((.not.bErrorInFlashCalculation).and.(dCalculatedBeta.EQ.(0.0d0))) then
            ! Líquido saturado!
            iCalculatedThermodynamicCondition = THERMOCOND_SaturatedLiquid

        else if((.not.bErrorInFlashCalculation).and.(dCalculatedBeta.EQ.(1.0d0))) then whichThermoCond
            ! Vapor saturado!
            iCalculatedThermodynamicCondition = THERMOCOND_SaturatedVapor

        else if(bFlashResultOkAndTwoPhase) then whichThermoCond
            ! Equilíbrio Líquido-Vapor!
            iCalculatedThermodynamicCondition = THERMOCOND_LiquidVaporVLE

        else if(bFoundFlashPAboveBubbleP.or.bUseSubcooledLiqEstimate) then whichThermoCond
            ! Líquido subresfriado!
            iCalculatedThermodynamicCondition = THERMOCOND_SubcooledLiquid
            dCalculatedBeta = 0.0d0
            oCalculatedLiqComposition = oZ

        else if(bFoundFlashPAboveDewP.or.bUseSuperheatedVapEstimate) then whichThermoCond
            ! Vapor superaquecido!
            iCalculatedThermodynamicCondition = THERMOCOND_SuperheatedVapor
            dCalculatedBeta = 1.0d0
            oCalculatedVapComposition = oZ

        else whichThermoCond
            ! Condição não contemplada:
            iIER = ERROR_CouldNotDetermineThermodynamicCondition
            return

        end if whichThermoCond

    end subroutine CalculateMixtureThermodynamicCondition_V2

    ! =============================================================
    ! =============================================================
    subroutine SecantMethod_CalculateBubbleOrDewPressure(dFlashPressure1, dFlashPressure2, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
        oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
        dRootFlashPressure, dCalculatedObjectiveFunction, iIER)

        ! OBJETIVO: Utilizar o Método da Secante para tentar o cálculo da pressão de bolha ou de orvalho.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure1                 ! Primeira estimativa de pressão
        real(c_double), value, intent(in) :: dFlashPressure2                 ! Segunda estimativa de pressão
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se realizar os cálculos.
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iCalculationFlag                ! Indicativo do tipo de cálculo (bolha ou orvalho), conforme convenção.
        logical, intent(in) :: bHasOtherPhaseCompositionEstimate             ! "True" se já estimou a composição do líquido (orvalho) ou do vapor (bolha).
        real(c_double), dimension(iNComp), intent(in) :: oOtherPhaseCompositionEstimate ! Composição estimada do líquido (orvalho) ou do vapor (bolha).

        real(c_double), intent(out) :: dRootFlashPressure            ! Estimativa calculada para a pressão de bolha ou orvalho, caso o algoritmo da secante convirja.
        real(c_double), intent(out) :: dCalculatedObjectiveFunction
        integer(c_int), intent(out) :: iIER                          ! Código de erros, conforme convencionado.

        ! ----------------- DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double) :: dCalculatedObjectiveFunction1, dCalculatedObjectiveFunction2          ! Variáveis auxiliares para armazenamento dos Valores calculados da função objetivo.
        real(c_double) :: dTempFlashPressure1, dTempFlashPressure2                              ! Variáveis auxiliares para armazenamento das estimativas de pressão de bolha ou orvalho.
        integer :: iCount
        logical :: converged
        logical :: bIsBubbleCalculation, bIsDewCalculation
        logical :: bObjFunctionValueIsExact

        real(c_double) :: dLastPOk, dObjFOfLastPOk, dLastRelErrorOk

        ! ---------------- CONSTANTES:
        integer, parameter :: IMAXCOUNT = 50  ! 22/11/2021 - diminuindo de 1000 pra 50 com vistas à otimização de desempenho
        real(c_double), parameter :: dRelativeToleranceForRoot = 0.5d-2
        real(c_double), parameter :: dAbsoluteToleranceForObjectiveFunction = 1.0d-4

        ! ----------------- PROCEDIMENTOS E CÁLCULOS:

        ! Bolha? Orvalho?
        bIsBubbleCalculation = (iCalculationFlag.EQ.CALCTYPE_BubblePoint)
        bIsDewCalculation = (iCalculationFlag.EQ.CALCTYPE_DewPoint)

        ! Inicialização
        !dTempFlashPressure1 = 1.0 * 101325.0
        !dTempFlashPressure2 = 600.0 * 101325.0

                ! TESTE - 10/3/2021
                !dTempFlashPressure1 = 270.0d5
                !dTempFlashPressure2 = 340.0d5

                ! TESTE - 11/5/2021 - COMENTEI
                call FindBubbleOrDewPRootInterval(dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                    iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, dTempFlashPressure1, dTempFlashPressure2, iIER)

                if (iIER .ne. ERROR_EverythingOK) return

!        block
!            real(c_double) :: temp
!            temp = dTempFlashPressure1
!            do while (temp.lt.dTempFlashPressure2)
!                call CalculateBubbleOrDewPointObjectiveFunction(temp, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
!                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
!                    dCalculatedObjectiveFunction, iIER)
!                write(*, *) temp, dCalculatedObjectiveFunction
!                temp = temp + (dTempFlashPressure2 - dTempFlashPressure1)/100.0
!            end do
!        end block

        call CalculateBubbleOrDewPointObjectiveFunction(dTempFlashPressure1, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
            oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
            dCalculatedObjectiveFunction1, iIER)

        if (iIER .ne. ERROR_EverythingOK) return

        call CalculateBubbleOrDewPointObjectiveFunction(dTempFlashPressure2, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
            oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
            dCalculatedObjectiveFunction2, iIER)

        if (iIER .ne. ERROR_EverythingOK) return

        do iCount = 1, IMAXCOUNT
            dRootFlashPressure =  dTempFlashPressure1*dCalculatedObjectiveFunction2 - dTempFlashPressure2*dCalculatedObjectiveFunction1
            dRootFlashPressure = dRootFlashPressure /(dCalculatedObjectiveFunction2 - dCalculatedObjectiveFunction1)

                dRootFlashPressure = 0.5d0 * (dTempFlashPressure1 + dTempFlashPressure2)    ! Trocando para Método da Bisseção

            bObjFunctionValueIsExact = .true.

            call CalculateBubbleOrDewPointObjectiveFunction(dRootFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                dCalculatedObjectiveFunction, iIER)

            ! =======================================
            !   INSERÇÃO EM 15/9/2021
            if(iIER.EQ.ERROR_EverythingOK) then
                ! Guardar informações sobre o último valor que não deu erro:
                dLastPOk = dRootFlashPressure
                dObjFOfLastPOk = dCalculatedObjectiveFunction

                if((dObjFOfLastPOk/dCalculatedObjectiveFunction1).gt.(0.0d0)) then
                    dLastRelErrorOk = abs(dLastPOk - dTempFlashPressure2) / dTempFlashPressure2
                else
                    dLastRelErrorOk = abs(dLastPOk - dTempFlashPressure1) / dTempFlashPressure1
                end if

            end if
            !   FIM DA INSERÇÃO EM 15/9/2021
            ! =======================================

            !if (iIER .ne. ERROR_EverythingOK) return    ! Linha original (abordagem original em caso de erro)
            ! Nova abordagem em caso de erro (para não encerrar precocemente a busca se um erro puder ser contornado) +++++++++++++++++++++++++++
            if (iIER .ne. ERROR_EverythingOK) then
                dCalculatedObjectiveFunction = dCalculatedObjectiveFunction1 + &
                    ((dCalculatedObjectiveFunction2 - dCalculatedObjectiveFunction1) / (dTempFlashPressure2 - dTempFlashPressure1) * &
                     (dRootFlashPressure - dTempFlashPressure1))
                bObjFunctionValueIsExact = .false.
                iIER = ERROR_EverythingOK
            end if
            ! Fim da nova abordagem em caso de erro +++++++++++++++++++++++++++

            if ((dCalculatedObjectiveFunction*dCalculatedObjectiveFunction1).lt.0.0) then
                dTempFlashPressure2 = dRootFlashPressure
                dCalculatedObjectiveFunction2 = dCalculatedObjectiveFunction
            else if ((dCalculatedObjectiveFunction*dCalculatedObjectiveFunction2).lt.0.0) then
                dTempFlashPressure1 = dRootFlashPressure
                dCalculatedObjectiveFunction1 = dCalculatedObjectiveFunction
            else
            end if

                ! Critérios de convergência!
            converged = dabs((dTempFlashPressure1 - dTempFlashPressure2)/dTempFlashPressure2).le.dRelativeToleranceForRoot
            converged = converged .and. (dabs(dCalculatedObjectiveFunction).le.dAbsoluteToleranceForObjectiveFunction)

                                ! Critério específico
            converged = converged.OR.((dabs(dCalculatedObjectiveFunction).LE.(3.5d-5)).and. &
                                      (dabs((dTempFlashPressure1 - dTempFlashPressure2) / dTempFlashPressure2).LE.(0.01d0)).and. &
                                      (dabs((dTempFlashPressure1 - dTempFlashPressure2) / 98066.52).LE.(7.5d0)))

                                ! Outro critério específico
            converged = converged.OR.((dabs(dCalculatedObjectiveFunction).LE.(3.96d-6)).and. &
                                      (dabs((dTempFlashPressure1 - dTempFlashPressure2) / dTempFlashPressure2).LE.(0.015d0)).and. &
                                      (dabs((dTempFlashPressure1 - dTempFlashPressure2) / 98066.52).LE.(6.0d0)))

                                ! Outro critério específico
            converged = converged.OR.((dabs(dCalculatedObjectiveFunction).LE.(4.0d-4)).AND. &
                                      (dabs((dTempFlashPressure1 - dTempFlashPressure2) / dTempFlashPressure2).LE.(2.0d-5)))

            converged = converged.AND.bObjFunctionValueIsExact  ! Não aceitar estimativas intermediárias!

            if (converged) then

                iIER = ERROR_EverythingOK

                    ! ++++++++++++++++++++++++++++
                    !   INSERÇÃO EM 13/10/2021

                        ! Garantir que a convergência não ocorreu para um valor absurdo:
                        if(dRootFlashPressure.LT.(100.0d0 * 98066.52d0)) then

                            iIER = ERROR_ConvergenceProblemInBubblePointCalculation
                            if(bIsDewCalculation) iIER = ERROR_ConvergenceProblemInDewPointCalculation

                        end if

                    !   FIM DA INSERÇÃO EM 13/10/2021
                    ! ++++++++++++++++++++++++++++

                return

            end if

            ! =====================================
            !   INSERÇÃO EM 15/9/2021
            if(.not.converged) then
                ! Se for a última iteração, última chance de tentar algo...
                if(iCount.EQ.IMAXCOUNT) then
                    ! Caso o último valor disponível tenha sido calculado em boas condições, mesmo
                    ! que não nas ideais... melhor retorná-lo que reportar um erro.
                    if(   (dabs(dObjFOfLastPOk).LE.(3.5d-5)) .and. &
                          (dabs(dLastRelErrorOk).LE.(0.035d0)) ) then

                          converged = .true.
                          iIER = ERROR_EverythingOK
                          dRootFlashPressure = dLastPOk
                          dCalculatedObjectiveFunction = dObjFOfLastPOk
                          return

                    end if
                end if
            end if
            !   FIM DA INSERÇÃO EM 15/9/2021
            ! =====================================

        end do

        ! Número máximo de iterações atingido. Informar através do código de erro.
        iIER = ERROR_ConvergenceProblemInBubblePointCalculation
        if(bIsDewCalculation) iIER = ERROR_ConvergenceProblemInDewPointCalculation

    end subroutine SecantMethod_CalculateBubbleOrDewPressure

    ! =============================================================
    ! =============================================================
    subroutine FindBubbleOrDewPRootInterval(dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, dLowerIntervalP, dUpperIntervalP, iIER)

        ! OBJETIVO: Encontrar duas pressões que delimitem um intervalo no interior do qual esteja a pressão de
        !   Bolha ou Orvalho.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se realizar os cálculos.
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iCalculationFlag                ! Indicativo do tipo de cálculo (bolha ou orvalho), conforme convenção.
        logical, intent(in) :: bHasOtherPhaseCompositionEstimate             ! "True" se já estimou a composição do líquido (orvalho) ou do vapor (bolha).
        real(c_double), dimension(iNComp), intent(in) :: oOtherPhaseCompositionEstimate ! Composição estimada do líquido (orvalho) ou do vapor (bolha).

        real(c_double), intent(out) :: dLowerIntervalP               ! Pressão inferior do intervalo delimitado
        real(c_double), intent(out) :: dUpperIntervalP               ! Pressão superior do intervalo delimitado
        integer(c_int), intent(out) :: iIER                          ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: iIterationCount
        real(c_double) :: dInitialPressure
        logical :: bInitialPressureSearchFailed, bOtherPressureSearchFailed
        real(c_double) :: dNewTrialObjectiveFunction
        integer :: iIER_LocalObjectiveFunction
        logical :: bIsBubbleCalculation, bIsDewCalculation
        real(c_double), dimension(2) :: dOtherPressureTrial
        integer :: i
        real(c_double) :: dInitialPressureObjFunction
        real(c_double) :: dOtherPressure

        ! ------------------ CONSTANTES NECESSÁRIAS:
        real(c_double), parameter :: dInitialPressureFirstTrial = 50.0d0 * 98066.52d0
        real(c_double), parameter :: dInitialPressureSearchStep = 50.0d0 * 98066.52d0 / 2.0d0
        real(c_double), parameter :: dInitialPressureSearchLimit = 650.0d0 * 98066.52d0
        real(c_double), parameter :: dOtherPressureSearchStep = 25.0d0 * 98066.52d0
        integer, parameter :: iInitialPressureSearchIterLimit = 100
        integer, parameter :: iOtherPressureSearchIterLimit = 25

        ! ------------------ CÁLCULOS:

            ! Bolha? Orvalho?
        bIsBubbleCalculation = (iCalculationFlag.EQ.CALCTYPE_BubblePoint)
        bIsDewCalculation = (iCalculationFlag.EQ.CALCTYPE_DewPoint)

            ! Buscar um primeiro valor de pressão em que o cálculo da função objetivo funcione:
        iIterationCount = 0
        dInitialPressure = dInitialPressureFirstTrial

        searchForInitialP: do

            ! Atualizar contagem:
            iIterationCount = iIterationCount + 1

            ! Testar a função objetivo:
            iIER_LocalObjectiveFunction = ERROR_EverythingOK

            call CalculateBubbleOrDewPointObjectiveFunction(dInitialPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                dNewTrialObjectiveFunction, iIER_LocalObjectiveFunction)

            ! Se o cálculo funcionou, já pode parar a busca:
            searchOK: if((iIER_LocalObjectiveFunction.EQ.ERROR_EverythingOK).AND. &
                            (dabs(dNewTrialObjectiveFunction).GT.(1.0D-4))) then    ! Observação "empírica": evitar valor inicial muito perto da raíz...
                bInitialPressureSearchFailed = .FALSE.
                dInitialPressureObjFunction = dNewTrialObjectiveFunction
                exit searchForInitialP
            end if searchOK

            ! Atualizar a pressão a ser testada:
            dInitialPressure = dInitialPressure + dInitialPressureSearchStep

            ! Verificar outras condições de saída para falha:
            bInitialPressureSearchFailed = (dInitialPressure.GE.dInitialPressureSearchLimit)
            bInitialPressureSearchFailed = bInitialPressureSearchFailed.OR.(iIterationCount.GT.iInitialPressureSearchIterLimit)
            if(bInitialPressureSearchFailed) exit searchForInitialP

        end do searchForInitialP

            ! Abortar a rotina caso não tenha sido encontrada uma pressão válida:
        checkIfFoundValidInitialP: if(bInitialPressureSearchFailed) then

            iIER = ERROR_ConvergenceProblemInBubblePointCalculation
            if(bIsDewCalculation) iIER = ERROR_ConvergenceProblemInDewPointCalculation

            return

        end if checkIfFoundValidInitialP

            ! Partindo da pressão inicial, buscar outra em que o valor da função objetivo tenha sinal oposto.
        iIterationCount = 0

        dOtherPressureTrial(1) = dInitialPressure
        dOtherPressureTrial(2) = dInitialPressure

        searchForOtherP: do

            ! Atualizar contagem:
            iIterationCount = iIterationCount + 1

            ! Criar novas tentativas de pressão em ambas as direções:
            dOtherPressureTrial(1) = dOtherPressureTrial(1) + dOtherPressureSearchStep
            dOtherPressureTrial(2) = dOtherPressureTrial(2) - dOtherPressureSearchStep

            ! Avaliar individualmente cada uma das novas tentativas:
            testEachOtherP: do i = 1, 2

                iIER_LocalObjectiveFunction = ERROR_EverythingOK

                call CalculateBubbleOrDewPointObjectiveFunction(dOtherPressureTrial(i), dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                    dNewTrialObjectiveFunction, iIER_LocalObjectiveFunction)

                otherPTrialValid: if(iIER_LocalObjectiveFunction.EQ.ERROR_EverythingOK) then

                    otherPTrialDifSign: if((dNewTrialObjectiveFunction * dInitialPressureObjFunction).LT.(0.0d0)) then

                        if(dOtherPressureTrial(i).GT.(0.0d0)) then
                            ! Encontrada uma pressão que já delimita o intervalo desejado!
                            dOtherPressure = dOtherPressureTrial(i)
                            exit searchForOtherP
                        end if

                    end if otherPTrialDifSign

                end if otherPTrialValid

            end do testEachOtherP

            ! Testar condição de falha:
            bOtherPressureSearchFailed = (iIterationCount.gt.iOtherPressureSearchIterLimit)

            otherPNotFound: if(bOtherPressureSearchFailed) then

                ! Não foi possível delimitar o intervalo desejado.
                iIER = ERROR_ConvergenceProblemInBubblePointCalculation
                if(bIsDewCalculation) iIER = ERROR_ConvergenceProblemInDewPointCalculation

                return

            end if otherPNotFound

        end do searchForOtherP

        ! Retornar as duas pressões que delimitam o intervalo:
        dLowerIntervalP = dInitialPressure
        dUpperIntervalP = dOtherPressure

        checkOrder: if(dLowerIntervalP.GT.dUpperIntervalP) then
            dLowerIntervalP = dOtherPressure
            dUpperIntervalP = dInitialPressure
        end if checkOrder

    end subroutine FindBubbleOrDewPRootInterval

    ! =============================================================
    ! =============================================================
    subroutine CalcPressureStepwiseFlashPTVLE(dStepOnP, dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, &
                iLiqPhaseModel, iVapPhaseModel, oPeneloux, bIsTwoPhaseAccordingtoBubbleT, dCalculatedBeta, oCalculatedLiqComposition, &
                oCalculatedVapComposition, bPStepwiseDetectedSubcooledLiq, dPStepwiseRoughBubblePEstimate, iIER)

        ! OBJETIVO: Calcular um "flash" PT através de passos sucessivos na pressão. A ideia é começar de pressões
        !   mais baixas, de convergência teoricamente mais fácil, e ir fornecendo os resultados como chutes iniciais
        !   para os pŕoximos cálculos em pressões sucessivamente maiores.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dStepOnP                        ! Passo na pressão (dar negativo para a rotina ajustar).
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        logical, intent(in) :: bIsTwoPhaseAccordingtoBubbleT                 ! "True" se um cálculo prévio de temperatura de bolha mostrou este ser um caso bifásico.

        real(c_double), intent(out) :: dCalculatedBeta                              ! Fração molar vaporizada determinada.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedLiqComposition ! Composição molar determinada da fase líquida.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedVapComposition ! Composição molar determinada da fase vapor.
        logical, intent(out) :: bPStepwiseDetectedSubcooledLiq               ! "True" se este cálculo determinou que a mistura está subresfriada.
        real(c_double), intent(out) :: dPStepwiseRoughBubblePEstimate        ! Estimativa da pressão de bolha encontrada por este cálculo.
        integer(c_int), intent(out) :: iIER                                         ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dCurrentStepValue
        real(c_double) :: dTrialPressure
        logical :: bGotEstimateForBubblePoint                   ! "True" se conseguir estimativas para o ponto de bolha.
        real(c_double) :: dClosestToBubbleT                     ! Temperatura mais próxima encontrada da temperatura de bolha.
        real(c_double), dimension(iNComp) :: oVapCompositionClosestToBubbleT   ! Composição da fase vapor correspondente a "dClosestToBubbleT".
        logical :: bGotEstimateForDewPoint                      ! "True" se conseguir estimativas para o ponto de orvalho.
        real(c_double) :: dClosestToDewT                        ! Temperatura mais próxima encontrada da temperatura de orvalho.
        real(c_double), dimension(iNComp) :: oLiqCompositionClosestToDewT  ! Composição da fase líquida correspondente a "dClosestToDewT".
        logical :: bHasInitialEstimates                             ! "True" caso possua chutes iniciais para o processo iterativo do "flash".
        real(c_double) :: dInitialBeta                              ! Estimativa inicial da fração molar vaporizada, caso exista.
        real(c_double), dimension(iNComp) :: oInitialLiqComposition ! Estimativa inicial da composição da fase líquida, caso exista.
        real(c_double), dimension(iNComp) :: oInitialVapComposition ! Estimativa inicial da composição da fase vapor, caso exista.
        integer(c_int) :: iIER_TrialFlash                           ! Código de erros de tentativas no "flash", conforme convencionado.
        logical :: bTwoPhaseFlash
        logical :: bFoundBasePressure                               ! "True" se a rotina conseguir convergência em uma pressão mais baixa.
        logical :: bLastAdvanceOnP

        ! ------------------ CONSTANTES NECESSÁRIAS:
        real(c_double), parameter :: dMinimumTrialP = 101324.99d0

        ! ------------------ CÁLCULOS:

        ! PRIMEIRO PASSO: Verificar / ajustar o passo na pressão:
        dCurrentStepValue = dStepOnP
        if(dStepOnP.LT.(0.0d0)) dCurrentStepValue = 0.1d0 * dFlashPressure
            ! 30/11/2021 - fração acima originalmente era "0.05". Foi modificada para valor maior
            !   na busca por melhor desempenho das simulações.

        ! Inicializações necessárias:
        bPStepwiseDetectedSubcooledLiq = .false.
        bFoundBasePressure = .false.
        dTrialPressure = dFlashPressure

        ! SEGUNDO PASSO: Reduzir sucessivamente a pressão (mantendo na temperatura desejada) até se chegar na primeira
        !   convergência:
        reduceP: do

            ! Reduzir a pressão em mais um passo:
            dTrialPressure = dTrialPressure - dCurrentStepValue

            ! Garantir que a pressão não caia abaixo de determinado nível mínimo:
            if(dTrialPressure.LT.dMinimumTrialP) exit reduceP

            ! Buscar, neste nível de pressão, alguma temperatura em que o cálculo do "flash" convirja:
            call GenerateVLEExtremaInitialEstimates(dTrialPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, oPeneloux, &
                            bGotEstimateForBubblePoint, dClosestToBubbleT, oVapCompositionClosestToBubbleT, bGotEstimateForDewPoint, dClosestToDewT, &
                            oLiqCompositionClosestToDewT, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition)

            ! Tentar calcular o "flash" neste nível de pressão (e na temperatura desejada):
            iIER_TrialFlash = ERROR_EverythingOK

            call CalcFlashPTVLE(dTrialPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition, dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, iIER_TrialFlash)

            ! Caso tenha ocorrido erro, tentar no próximo nível de pressão:
            if(iIER_TrialFlash.NE.ERROR_EverythingOK) cycle reduceP

            ! Não aceitar "flashes" que tenham convergido com betas ínfimos, porque este tipo de resultado
            ! não cumpre bem o objetivo desta etapa, que é a de gerar bons chutes iniciais para as pressões
            ! mais altas.
            if(dCalculatedBeta.LT.(0.01d0)) cycle reduceP

            ! Deu bifásico?
            bTwoPhaseFlash = (dCalculatedBeta.GT.(0.0d0)).AND.(dCalculatedBeta.LT.(1.0d0)).AND.(.NOT.ISNAN(dCalculatedBeta))

            ! Se deu bifásico (e por enquanto, SÓ SE deu bifásico), aceitar este nível de pressão como o de partida:
            if(bTwoPhaseFlash) then
                bFoundBasePressure = .true.
                bHasInitialEstimates = .true.
                dInitialBeta = dCalculatedBeta
                oInitialLiqComposition = oCalculatedLiqComposition
                oInitialVapComposition = oCalculatedVapComposition
                exit reduceP
            end if

        end do reduceP

        ! Não será possível prosseguir se não tiver conseguido convergência em uma pressão mais reduzida:
        if(.NOT.bFoundBasePressure) then
            iIER = ERROR_NoFlashConvergenceInLowerPressures
            return
        end if

        ! Avançar com a pressão em um ritmo mais lento que o do recuo...
        dCurrentStepValue = dCurrentStepValue / 3.0d0
            ! 29/11/2021 - No código originalmente validado, o passo acima era dividido por "100".
                ! Isso significa que para cada recuo anterior, seria necessário "reavançar" 100 vezes.
                ! Ou seja... 10 recuos na pressão implicariam 1000 avanços!
                ! Imagina-se que isso pode estar prejudicando muito o desempenho das simulações.
                ! E talvez seja excessivo.

        ! TERCEIRO PASSO: De posse de resultados convergidos em uma pressão mais baixa, avançar sucessivamente com a pressão,
        !   usando os resultados de um nível como chutes iniciais para o próximo:
        bLastAdvanceOnP = .false.

        advanceP: do

            ! Ver se é para executar mais esta passagem:
            if(bLastAdvanceOnP) then
                iIER = iIER_TrialFlash
                exit advanceP
            end if

            ! Avançar com a pressão:
            dTrialPressure = dTrialPressure + dCurrentStepValue

            ! Cuidar para não ultrapassar a pressão desejada:
            if(dTrialPressure.GE.dFlashPressure) then
                dTrialPressure = dFlashPressure
                bLastAdvanceOnP = .true.
            end if

            ! Tentar calcular o "flash" na nova pressão:
            iIER_TrialFlash = ERROR_EverythingOK

            call CalcFlashPTVLE(dTrialPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition, dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, iIER_TrialFlash)

            ! Deu bifásico?
            bTwoPhaseFlash = (dCalculatedBeta.GT.(0.0d0)).AND.(dCalculatedBeta.LT.(1.0d0)).AND.(.NOT.ISNAN(dCalculatedBeta)).AND.(iIER_TrialFlash.EQ.ERROR_EverythingOK)

            ! Aprimorar, se possível, a estimativa inicial da próxima passagem:
            if(bTwoPhaseFlash) then
                bHasInitialEstimates = .true.
                dInitialBeta = dCalculatedBeta
                oInitialLiqComposition = oCalculatedLiqComposition
                oInitialVapComposition = oCalculatedVapComposition
            end if

            ! É preciso ficar atento e fazer ajustes caso se esteja operando próximo ao ponto de bolha.
            checkBetaOuter: if(bTwoPhaseFlash.AND.(dCalculatedBeta.LT.(0.1d0))) then             ! Caminhando para perto do ponto de bolha...
                checkPOuter: if(dFlashPressure.GT.(50.0d5)) then                         ! Pressão desejada não é exatamente baixa...

                    if((dTrialPressure / dFlashPressure).LT.(0.9d0)) then   ! Pela distância, suspeita de mistura subresfriada...
                        dCurrentStepValue = 250.0d0                         ! Adotar valor de passo reduzido, determinado com "experimentos"...
                    end if

                    if(dCalculatedBeta.LT.(0.5d0 / 100.0d0)) then                                       ! Praticamente em cima do ponto de bolha...
                        if(((dFlashPressure - dTrialPressure) / dCurrentStepValue).GT.(100.0d0)) then   ! Ainda faltam muitos passos...
                            ! Já é seguro dizer que a mistura está subresfriada.
                            ! Uma solução bifásica não será encontrada:
                            bPStepwiseDetectedSubcooledLiq = .true.
                            dPStepwiseRoughBubblePEstimate = dTrialPressure
                            dCalculatedBeta = -1.0d0        ! Na pressão DESEJADA, não calculamos o "beta".

                            checkForInconsistency: if(bIsTwoPhaseAccordingtoBubbleT) then
                                iIER = ERROR_InconsistentBubbleTAndPStepwiseResult
                                return
                            end if checkForInconsistency

                            exit advanceP

                        end if
                    end if

                end if checkPOuter
            end if checkBetaOuter

        end do advanceP

    end subroutine CalcPressureStepwiseFlashPTVLE

    ! =============================================================
    ! =============================================================
    subroutine GenerateVLEExtremaInitialEstimates(dFlashPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, oPeneloux, &
                            bGotEstimateForBubblePoint, dClosestToBubbleT, oVapCompositionClosestToBubbleT, bGotEstimateForDewPoint, dClosestToDewT, &
                            oLiqCompositionClosestToDewT, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition)

        ! OBJETIVO: Gerar estimativas iniciais razoáveis de composições e temperaturas para os cálculos necessários de
        !           "flash" e pontos de bolha e orvalho.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.

        logical, intent(out) :: bGotEstimateForBubblePoint                   ! "True" se conseguir estimativas para o ponto de bolha.
        real(c_double), intent(out) :: dClosestToBubbleT                     ! Temperatura mais próxima encontrada da temperatura de bolha.
        real(c_double), dimension(iNComp), intent(out) :: oVapCompositionClosestToBubbleT   ! Composição da fase vapor correspondente a "dClosestToBubbleT".
        logical, intent(out) :: bGotEstimateForDewPoint                      ! "True" se conseguir estimativas para o ponto de orvalho.
        real(c_double), intent(out) :: dClosestToDewT                        ! Temperatura mais próxima encontrada da temperatura de orvalho.
        real(c_double), dimension(iNComp), intent(out) :: oLiqCompositionClosestToDewT  ! Composição da fase líquida correspondente a "dClosestToDewT".
        logical, intent(out) :: bHasInitialEstimates                             ! "True" caso possua chutes iniciais para o processo iterativo do "flash".
        real(c_double), intent(out) :: dInitialBeta                              ! Estimativa inicial da fração molar vaporizada, caso exista.
        real(c_double), dimension(iNComp), intent(out) :: oInitialLiqComposition ! Estimativa inicial da composição da fase líquida, caso exista.
        real(c_double), dimension(iNComp), intent(out) :: oInitialVapComposition ! Estimativa inicial da composição da fase vapor, caso exista.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dStepOnT                                          ! "Passo" a ser adotado na temperatura.
        real(c_double) :: dTrialTemperature                                 ! Temperatura "da vez" a ser tentada.
        integer :: iLoopCount                                               ! Contagem de temperaturas tentadas.
        logical :: bStopIterating                                           ! "True" para parar de iterar.
        integer(c_int) :: iIER_Flash                                        ! Código de erros no cálculo do "flash", conforme convencionado.
        real(c_double) :: dCalculatedBeta                                   ! Fração molar vaporizada determinada.
        real(c_double), dimension(iNComp) :: oCalculatedLiqComposition      ! Composição molar determinada da fase líquida.
        real(c_double), dimension(iNComp) :: oCalculatedVapComposition      ! Composição molar determinada da fase vapor.
        logical :: bTwoPhaseFlash                                           ! "True" se o "flash" previu a existência de duas fases.

        ! ------------------ CONSTANTES NECESSÁRIAS:
        integer, parameter :: iMaxLoops = 20                                ! Máxima contagem permitida de temperaturas a tentar.
            ! 29/11/2021 - alterado acima de 50 para 20, para fins de otimização de desempenho

        ! ------------------ CÁLCULOS:

        ! Inicializações necessárias:
        dStepOnT = 25.0d0
        dTrialTemperature = 190.0d0 - dStepOnT         ! Começar com temperatura bem baixa, e ir subindo...
        iLoopCount = 0
        bStopIterating = .false.

        bGotEstimateForBubblePoint = .false.
        bGotEstimateForDewPoint = .false.

        bHasInitialEstimates = .false.
        dInitialBeta = -10.0d0
        oInitialLiqComposition = oZ
        oInitialVapComposition = oZ

        ! "Loop" para tentativas espaçadas...
        nextT: do

            ! Atualizar contagem:
            iLoopCount = iLoopCount + 1

            ! Encerrar?
            bStopIterating = bStopIterating.OR.(iLoopCount.GT.iMaxLoops)
            if(bStopIterating) exit nextT

            ! Calcular a temperatura da vez:
            dTrialTemperature = dTrialTemperature + dStepOnT

            ! Calcular o "flash" da vez:
            iIER_Flash = ERROR_EverythingOK

            call CalcFlashPTVLE(dFlashPressure, dTrialTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition,  dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, iIER_Flash)

            if(iIER_Flash.NE.ERROR_EverythingOK) cycle nextT

            ! Deu bifásico?
            bTwoPhaseFlash = (dCalculatedBeta.GT.(0.0d0)).AND.(dCalculatedBeta.LT.(1.0d0)).AND.(.NOT.ISNAN(dCalculatedBeta))

            ! Ficar com a primeira estimativa bifásica que convergir para o ponto de bolha!
            estimatesForBubble: if((.NOT.bGotEstimateForBubblePoint).AND.bTwoPhaseFlash) then
                dClosestToBubbleT = dTrialTemperature
                oVapCompositionClosestToBubbleT = oCalculatedVapComposition
                bGotEstimateForBubblePoint = .true.
            end if estimatesForBubble

            ! Sempre que der bifásico, atualizar a estimativa de ponto de orvalho:
            estimatesForDew: if(bTwoPhaseFlash) then
                dClosestToDewT = dTrialTemperature
                oLiqCompositionClosestToDewT = oCalculatedLiqComposition
                bGotEstimateForDewPoint = .true.
            end if estimatesForDew

            ! Chegou aqui e deu bifásico? Atualizar, para a próxima iteração, as estimativas iniciais.
            updateInitials: if(bTwoPhaseFlash) then
                refineInitialFlashGuess: if((.NOT.bHasInitialEstimates).OR.((dCalculatedBeta.GE.0.2d0).AND.(dCalculatedBeta.LE.0.7d0))) then
                    bHasInitialEstimates = .true.
                    dInitialBeta = dCalculatedBeta
                    oInitialLiqComposition = oCalculatedLiqComposition
                    oInitialVapComposition = oCalculatedVapComposition
                end if refineInitialFlashGuess  ! 19/12/2019
            end if updateInitials

        end do nextT

    end subroutine GenerateVLEExtremaInitialEstimates

    ! =============================================================
    ! =============================================================
    subroutine CalculateBubbleOrDewPointObjectiveFunction(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                    dCalculatedObjectiveFunction, iIER)

        ! OBJETIVO: Calcular a função objetivo cuja raíz é o ponto de bolha ou orvalho de dada mistura.
        !           Rotina a ser utilizada tanto em cálculos de pressão quanto de temperatura de bolha ou orvalho.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iCalculationFlag                ! Indicativo do tipo de cálculo (bolha ou orvalho), conforme convenção.
        logical, intent(in) :: bHasOtherPhaseCompositionEstimate             ! "True" se já estimou a composição do líquido (orvalho) ou do vapor (bolha).
        real(c_double), dimension(iNComp), intent(in) :: oOtherPhaseCompositionEstimate ! Composição estimada do líquido (orvalho) ou do vapor (bolha).

        real(c_double), intent(out) :: dCalculatedObjectiveFunction          ! Valor calculado da função objetivo desejada.
        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i                                                         ! Índice de laços "do"
        real(c_double), dimension(iNComp) :: oLiqComposition                 ! Vetor composição da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp) :: oVapComposition                 ! Vetor composição da fase vapor (indexado por componente)
        real(c_double), dimension(iNComp) :: oEquilibriumRatios_Old          ! Vetor de constantes de equilíbrio calculadas (indexado por componente)
        integer :: iCurrentIteration                                         ! Iteração corrente do procedimento iterativo.
        logical :: bIsWithinMaxIterations                                    ! "True" enquanto o procedimento iterativo não estourar o limite de iterações.
        logical :: bConverged                                                ! "True" se o cálculo iterativo atingiu convergência.
        logical :: bKeepIterating                                            ! "True" para prosseguir com o processo iterativo.
        logical :: bIsBubbleCalculation, bIsDewCalculation                   ! "True" ou "false" conforme se esteja calculando (ou não) ponto de bolha ou orvalho.
        real(c_double), dimension(iNComp) :: oEquilibriumRatios_New          ! Vetor de constantes de equilíbrio calculadas (indexado por componente)
        real(c_double) :: dSumConv, dSumEq
        logical :: bLiquidPhaseModelIsCubicEOS, bVaporPhaseModelIsCubicEOS, bBothPhaseModelsAreCubicEOS
        real(c_double), dimension(iNComp) :: oLiqFugCoef           ! Vetor de coeficientes de fugacidade calculados para a fase líquida (indexado por componente).
        real(c_double), dimension(iNComp) :: oVapFugCoef           ! Vetor de coeficientes de fugacidade calculados para a fase vapor (indexado por componente).
        real(c_double), dimension(iNComp) :: oTempComposition

        ! ------------------ CONSTANTES NECESSÁRIAS:
        integer, parameter :: iMaxIterations = 100                           ! Limite máximo de iterações

        ! ------------------ CÁLCULOS:

        ! Verificar quanto ao uso de Equações de Estado Cúbicas:
        bLiquidPhaseModelIsCubicEOS = (iLiqPhaseModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iLiqPhaseModel.EQ.SRK_PENELOUX).or.(iLiqPhaseModel.EQ.PENG_ROBINSON_PENELOUX)
        bVaporPhaseModelIsCubicEOS = (iVapPhaseModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iVapPhaseModel.EQ.SRK_PENELOUX).or.(iVapPhaseModel.EQ.PENG_ROBINSON_PENELOUX)
        bBothPhaseModelsAreCubicEOS = (bLiquidPhaseModelIsCubicEOS.AND.bVaporPhaseModelIsCubicEOS)

        ! Bolha? Orvalho?
        bIsBubbleCalculation = (iCalculationFlag.EQ.CALCTYPE_BubblePoint)
        bIsDewCalculation = (iCalculationFlag.EQ.CALCTYPE_DewPoint)

        ! Conforme o caso, inicializar composições:
        compositionInitialGuess: if(bHasOtherPhaseCompositionEstimate) then

            do i = 1, iNComp

                if(bIsBubbleCalculation) then
                    oLiqComposition(i) = oZ(i)
                    oVapComposition(i) = oOtherPhaseCompositionEstimate(i)
                else if(bIsDewCalculation) then
                    oLiqComposition(i) = oOtherPhaseCompositionEstimate(i)
                    oVapComposition(i) = oZ(i)
                end if

                oEquilibriumRatios_Old(i) = oVapComposition(i) / oLiqComposition(i)

            end do

        else compositionInitialGuess

            ! Inicializar as composições com um "chute inicial":
            call InitializeLiquidAndVaporCompositions(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oLiqComposition, oVapComposition)

            ! Normalizar as composições:
            oTempComposition = oLiqComposition
            call NormComposition(iNComp, oTempComposition, oLiqComposition)
            oTempComposition = oVapComposition
            call NormComposition(iNComp, oTempComposition, oVapComposition)

            do i = 1, iNComp
                oEquilibriumRatios_Old(i) = oVapComposition(i) / oLiqComposition(i)
            end do

        end if compositionInitialGuess

        ! Preparar para procedimento iterativo:
        iCurrentIteration = 0
        bConverged = .FALSE.

        ! Procedimento iterativo:
        iterative: do

            ! Contagem de iterações:
            iCurrentIteration = iCurrentIteration + 1

            ! Controle de execução:
            bIsWithinMaxIterations = (iCurrentIteration.LE.iMaxIterations)
            bKeepIterating = (bIsWithinMaxIterations.AND.(.NOT.bConverged))

            if(.NOT.bKeepIterating) exit iterative

            ! Atualizar as composições:
            updateCompositions: do i = 1, iNComp

                if(bIsBubbleCalculation) then
                    oLiqComposition(i) = oZ(i)
                    oVapComposition(i) = oEquilibriumRatios_Old(i) * oLiqComposition(i)
                else if(bIsDewCalculation) then
                    oLiqComposition(i) = oZ(i) / oEquilibriumRatios_Old(i)
                    oVapComposition(i) = oZ(i)
                end if

            end do updateCompositions

            ! Calcular as "novas" constantes de equilíbrio:
            call CalculateVLEEquilibriumRatios(iNComp, oLiqComposition, oVapComposition, oKij, oLij, dFlashPressure, dFlashTemperature, &
                                            iLiqPhaseModel, iVapPhaseModel, oTc, oPc, oW, oPeneloux, oEquilibriumRatios_New, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Testar a convergência:
            dSumConv = 0.0d0
            do i = 1, iNComp
                dSumConv = dSumConv + ((oEquilibriumRatios_New(i) / oEquilibriumRatios_Old(i)  - 1.0d0) ** 2.0d0)
            end do

            bConverged = (dSumConv.LE.(0.0001d0))

            ! Se ambas as fases estiverem sendo modeladas por Equações de Estado Cúbicas,
            ! aprimorar as constantes de equilíbrio da próxima iteração por meio de uma abordagem mais elaborada:
            updatingForCubicEOS: if(bBothPhaseModelsAreCubicEOS) then

                ! Resgatar os coeficientes de fugacidade dos componentes em ambas as fases:
                call CalculatePhiPhiEquilibriumRatiosFromCubicEOS(iNComp, oLiqComposition, oVapComposition, oKij, oLij, dFlashPressure, dFlashTemperature, &
                                            iLiqPhaseModel, iVapPhaseModel, oTc, oPc, oW, oPeneloux, oLiqFugCoef, oVapFugCoef, oEquilibriumRatios_New, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

                ! Estimar os coeficientes de equilíbrio para a próxima iteração:
                do i = 1, iNComp
                    oEquilibriumRatios_Old(i) = oEquilibriumRatios_Old(i) * (oLiqFugCoef(i) * oLiqComposition(i)) / (oVapFugCoef(i) * oVapComposition(i))
                end do

            else updatingForCubicEOS

                ! Preparar para a próxima iteração:
                oEquilibriumRatios_Old = oEquilibriumRatios_New

            end if updatingForCubicEOS

        end do iterative

        ! Verificar como foi o final do procedimento iterativo:
        iterativeEndedOK: if(.NOT.bConverged) then

                if(bIsBubbleCalculation) then
                    iIER = ERROR_ConvergenceProblemInBubblePointCalculation
                else if(bIsDewCalculation) then
                    iIER = ERROR_ConvergenceProblemInDewPointCalculation
                end if

            return

        end if iterativeEndedOK

        ! Calcular e retornar a equação:
        dSumEq = 0.0d0
        do i = 1, iNComp

            if(bIsBubbleCalculation) then
                dSumEq = dSumEq + oEquilibriumRatios_New(i) * oZ(i)
            else if(bIsDewCalculation) then
                dSumEq = dSumEq + oZ(i) / oEquilibriumRatios_New(i)
            end if

        end do

        dCalculatedObjectiveFunction = dSumEq - 1.0d0

    end subroutine CalculateBubbleOrDewPointObjectiveFunction

    ! =============================================================
    ! =============================================================
    subroutine CalcFlashPTVLE(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition, dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, iIER)

        ! OBJETIVO: Calcular um "flash PT" para Equilíbrio Líquido-Vapor, assumindo direto que o equilíbrio existe! Caso seja necessário testar
        !       se a mistura está superaquecida ou subresfriada, deve-se fazê-lo antes de chamar esta rotina.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        logical, intent(in) :: bHasInitialEstimates                          ! "True" caso possua chutes iniciais para o processo iterativo.
        real(c_double), value, intent(in) :: dInitialBeta                    ! Estimativa inicial da fração molar vaporizada, caso exista.
        real(c_double), dimension(iNComp), intent(in) :: oInitialLiqComposition ! Estimativa inicial da composição da fase líquida, caso exista.
        real(c_double), dimension(iNComp), intent(in) :: oInitialVapComposition ! Estimativa inicial da composição da fase vapor, caso exista.

        real(c_double), intent(out) :: dCalculatedBeta                              ! Fração molar vaporizada determinada.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedLiqComposition ! Composição molar determinada da fase líquida.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedVapComposition ! Composição molar determinada da fase vapor.
        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.


        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oLiqComposition    ! Vetor composição da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp) :: oVapComposition    ! Vetor composição da fase vapor (indexado por componente)
        real(c_double), dimension(iNComp) :: oNormalizedLiqComposition    ! Vetor composição normalizada da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp) :: oNormalizedVapComposition    ! Vetor composição normalizada da fase vapor (indexado por componente)
        logical :: bKeepIterating                               ! "Flag" que mantém o procedimento iterativo rodando enquanto for "true"
        integer :: iCurrentIteration                            ! Iteração corrente do procedimento iterativo.
        logical :: bIsWithinMaxIterations                       ! "True" enquanto estivermos dentro do limite de iterações.
        real(c_double) :: dMaxStep                              ! Métrica a ser comparada com a tolerância da solução numérica iterativa.
        logical :: bMaxStepConverged                            ! "True" quando for detectada convergência da métrica "dMaxStep".
        real(c_double) :: dBeta                                 ! Variável "beta" do "flash".
        logical :: bBetaIsWithinLimits                          ! "True" enquanto o "beta" permanecer dentro dos limites aplicáveis.
        real(c_double), dimension(iNComp) :: oEquilibriumRatios ! Vetor de constantes de equilíbrio calculadas (indexado por componente)
        integer :: i                                            ! Índice de laços "do"
        real(c_double) :: dFBeta, dDerivFBeta
        real(c_double) :: dStep, dAux, dAux2, dTrivial
        logical :: bTrivialLimNotReached                        ! "True" enquanto o limite referente à solução trivial não for atingido.
        logical :: bFoundTrivialSolution                        ! "True" para indicar que foi encontrada solução trivial
                                                                !   (veio da UFRJ; é diferente do "bTrivialLimNotReached").
        real(c_double) :: dMukBrillSum                          ! Somatório do critério de convergência do livro de Mukherjee e Brill
        logical :: bMukBrillCritConverged                       ! "True" se o cálculo convergir pelo critério do livro de Mukherjee e Brill.

        ! ------------------ CONSTANTES NECESSÁRIAS:
        integer, parameter :: iMaxIterations = 10000            ! Limite máximo de iterações
        real(c_double), parameter :: dTol = 1.0d-6              ! Tolerância a ser atingida por solução numérica iterativa.
        real(c_double), parameter :: dBetaTol = 1.0d-12         ! Tolerância que precisa ser atendida pelo "beta".
        real(c_double), parameter :: dTrivialLim = 0.001d0      ! Limite referente a cálculos de solução trivial

        ! ------------------ CÁLCULOS:

        ! Criar estimativas iniciais para as composições das duas fases:
        call InitializeLiquidAndVaporCompositions(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                oLiqComposition, oVapComposition)

        if(bHasInitialEstimates) then
            oLiqComposition = oInitialLiqComposition
            oVapComposition = oInitialVapComposition
        end if

        ! Resultados "default":
        dCalculatedBeta = -1.0d0

        ! Inicializações:
        iCurrentIteration = 0
        dMaxStep = 10000.0d0
        dBeta = 0.5d0
        dTrivial = 1.0d+10
        bMukBrillCritConverged = .false.

        if(bHasInitialEstimates) dBeta = dInitialBeta

        ! Procedimento iterativo:
        solveVLE: do

            ! Contagem de iterações:
            iCurrentIteration = iCurrentIteration + 1

            ! Controle de execução:
            bIsWithinMaxIterations = (iCurrentIteration.LE.iMaxIterations)
            bMaxStepConverged = (dMaxStep.LE.dTol)
            bBetaIsWithinLimits = (dBeta.GT.dBetaTol).AND.((1.0d0 - dBeta).GT.dBetaTol)
            bTrivialLimNotReached = (dTrivial.GT.dTrivialLim)

            bMaxStepConverged = bMaxStepConverged.OR.bMukBrillCritConverged

            ! Interromper?
            bKeepIterating = (bIsWithinMaxIterations.AND.(.NOT.bMaxStepConverged).AND.bBetaIsWithinLimits.AND.bTrivialLimNotReached)
            if(.NOT.bKeepIterating) exit solveVLE

            ! Normalizar as composições:
            call NormComposition(iNComp, oLiqComposition, oNormalizedLiqComposition)
            call NormComposition(iNComp, oVapComposition, oNormalizedVapComposition)

            ! Atualizar os "Ki":
            call CalculateVLEEquilibriumRatios(iNComp, oNormalizedLiqComposition, oNormalizedVapComposition, oKij, oLij, dFlashPressure, dFlashTemperature, &
                                            iLiqPhaseModel, iVapPhaseModel, oTc, oPc, oW, oPeneloux, oEquilibriumRatios, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Calcular o critério de convergência do livro de Mukherjee e Brill:
            useMukBrillCrit: if(.true.) then
                dMukBrillSum = 0.0d0
                mukBrillCritLoop: do i = 1, iNComp
                    dMukBrillSum = dMukBrillSum + (((oEquilibriumRatios(i) * oNormalizedLiqComposition(i) / oNormalizedVapComposition(i)) - 1.0d0) ** 2.0d0)
                end do mukBrillCritLoop

                bMukBrillCritConverged = (dMukBrillSum.LE.(0.0001))

                ! 2/7/2021 - Resultados mostraram que a tolerância de 0.0001 funciona quase sempre, mas
                !   que ocasionalmente faz um cálculo ou outro "convergirem" antes do que deveriam. A
                !   linha abaixo repete o teste com um valor mais baixo, que mostrou melhores resultados.
                bMukBrillCritConverged = (dMukBrillSum.LE.(2.5d-5))

                ! 27/8/2021 - Novos resultados revelaram que a tolerância de 2.5e-5 continua sendo muito alta.
                ! Funciona na maioria dos casos, mas em algumas temperaturas mas baixas, acusa convergência antes da hora.
                ! Novos testes revelaram que aparentemente, quando um cálculo já está convergido, ele consegue prosseguir até
                ! atingir a tolerância de 5e-6, e com poucas iterações adicionais. Por este motivo, esta tolerância será adotada.
                bMukBrillCritConverged = (dMukBrillSum.LE.(5.0d-6))

            end if useMukBrillCrit

            ! Cálculo referente à solução trivial:
            trivialIf: if(iCurrentIteration.GE.2) then

                dTrivial = 0.0d0
                do i = 1, iNComp
                    dTrivial = dTrivial + abs(oLiqComposition(i) - oVapComposition(i))
                end do

            end if trivialIf

            ! Atualizar as composições das fases:
            updateCompositions: do i = 1, iNComp
                oLiqComposition(i) = oZ(i) / (1.0d0 + dBeta * (oEquilibriumRatios(i) - 1.0d0))
                oVapComposition(i) = oEquilibriumRatios(i) * oLiqComposition(i)
            end do updateCompositions

            ! Calcular o resíduo e sua derivada DE ACORDO COM O CÓDIGO DA UFRJ.
            ! NOTAR QUE esse código parece não estar resolvendo a Equação do Flash...
            ! VERIFICAÇÃO COM A UFRJ: me informaram que esta forma utilizada "é só uma outra maneira
            !   de fazer o Rachford-Rice", mas que também pode utilizar a forma tradicional se preferir.
            !   Neste caso, "F" seria a própria equação de Rachford-Rice, e "dF" seria sua derivada com "beta".
            ! DECISÃO: Será usada a equação tradicional de Rachford-Rice mesmo. É disparado o método mais recomendado
            !       na literatura.
            dFBeta = 0.0d0
            dDerivFBeta = 0.0d0

            computeBetaFunctions: do i = 1, iNComp

                dFBeta = dFBeta + ((oZ(i) * (oEquilibriumRatios(i) - 1.0d0)) / (1.0d0 + dBeta * (oEquilibriumRatios(i) - 1.0d0)))

                dDerivFBeta = dDerivFBeta + (((oEquilibriumRatios(i) - 1.0d0) ** 2.0d0) * oZ(i) / ((dBeta * (oEquilibriumRatios(i) - 1.0d0) + 1.0d0) ** 2.0d0))

            end do computeBetaFunctions

            dDerivFBeta = -1.0d0 * dDerivFBeta

            ! Agora, em vez de praticar exatamente o Método de Newton, fazer os ajustes do código da UFRJ:

                ! "Inverting Jacobian Matrix"
            dStep = dFBeta / dDerivFBeta

                ! "Step brake"
            dAux = dBeta - dStep
            if(dAux.GT.1.0d0) then
                dBeta = dBeta + 0.95d0 * (1.0d0 - dBeta)
            else if(dAux.LT.0.0d0) then
                dBeta = 0.05d0 * dBeta
            else
                dBeta = dAux
            end if

                ! "Convergence criteria"
            dMaxStep = abs(dStep / dBeta)

            updateCompositionsAgain: do i = 1, iNComp

                ! Líquido
                dAux2 = oZ(i) / (1.0d0 + dBeta * (oEquilibriumRatios(i) - 1.0d0))
                dAux = abs(oLiqComposition(i) - dAux2) / dAux2
                if(dAux.GT.dMaxStep) dMaxStep = dAux
                oLiqComposition(i) = dAux2

                ! Vapor
                dAux2 = oEquilibriumRatios(i) * oLiqComposition(i)
                dAux = abs(oVapComposition(i) - dAux2) / dAux2
                if(dAux.GT.dMaxStep) dMaxStep = dAux
                oVapComposition(i) = dAux2

            end do updateCompositionsAgain

        end do solveVLE

        ! Procedimento iterativo foi terminado.
        ! Proceder de acordo com o motivo.

        ! Solução trivial?
        bFoundTrivialSolution = (dTrivial.LT.0.01d0).OR.(dBeta.LT.1.0d-10).OR.((1.0d0 - dBeta).LT.1.0d-10)
        trivialSol: if(bFoundTrivialSolution) then
            ! ATENÇÃO: PROVISORIAMENTE, a solução trivial está sendo tratada como um erro, porque é raríssima,
            ! e não convém retardar o desenvolvimento do trabalho para tratá-la. Mas neste caso, em vez de abortar
            ! o cálculo por "erro", deveria-se fazer uns poucos cálculos adicionais (vide código da UFRJ) para determinar
            ! se a fase é toda líquida ou toda vapor.
            iIER = ERROR_FlashCalculationFoundTrivialSolution
            return

        else if(.NOT.bBetaIsWithinLimits) then trivialSol
            ! Fração vaporizada fora dos limites!
            iIER = ERROR_FlashCalculationGotToBetaOutOfBounds
            return

        else if(.NOT.bIsWithinMaxIterations) then trivialSol
            ! Estourou o máximo de iterações!
            iIER = ERROR_FlashCalculationBlewMaxIterations
            return

        else if(bMaxStepConverged) then trivialSol
            ! Convergiu!

            ! "Beta" para retornar:
            dCalculatedBeta = dBeta

            ! Composições das fases para retornar:
            oCalculatedLiqComposition = oLiqComposition
            oCalculatedVapComposition = oVapComposition

        else trivialSol
            ! Não foi encontrada uma solução!
            iIER = ERROR_FlashCalculationFoundNoSolution
            return
        end if trivialSol

    end subroutine CalcFlashPTVLE

    ! =============================================================
    ! =============================================================
    subroutine InitializeLiquidAndVaporCompositions(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
        oLiqComposition, oVapComposition)

        ! OBJETIVO: Criar um "chute inicial" para as composições das fases líquido e vapor no "flash". Esse
        !   chute inicial será refinado no cálculo do "flash" até se chegar ao resultado final.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp), intent(out) :: oLiqComposition    ! Vetor composição da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp), intent(out) :: oVapComposition    ! Vetor composição da fase vapor (indexado por componente)

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i                                                         ! Índice de laços "do".
        real(c_double) :: dEqConst                                           ! Constante de equilíbrio (resultado intermediário).

        ! ------------------ CÁLCULOS:
        componentLoop: do i = 1, iNComp

            ! Estimar o coeficiente "K" (constante de equilíbrio) do componente "i":
            !       NOTA: Para este passo, está sendo usada a "correlação de Wilson"! Segundo a referência
            !           (DANESH, A. - 1998 - PVT And Phase Behaviour of Petroleum Reservoir Fluids),
            !           esta correlação é indicada para pressão abaixo de 3,5MPa. Mas como só está sendo usada
            !           neste ponto para fins de estimativa inicial (os valores serão aprimorados), este limite
            !           de pressão não está sendo verificado.
            dEqConst = oTc(i) / dFlashTemperature
            dEqConst = 1.0d0 - dEqConst
            dEqConst = dEqConst * (1.0d0 + oW(i)) * 5.37d0
            dEqConst = exp(dEqConst)
            dEqConst = dEqConst * oPc(i) / dFlashPressure

            ! Composição estimada da fase líquida:
            oLiqComposition(i) = oZ(i)

            ! Composição estimada da fase vapor:
            oVapComposition(i) = dEqConst * oLiqComposition(i)

        end do componentLoop

    end subroutine InitializeLiquidAndVaporCompositions

    ! =============================================================
    ! =============================================================
    subroutine NormComposition(iNComp, oGivenComposition, oNormalizedComposition)

        ! OBJETIVO: Normalizar dada composição de fase, garantindo que a soma das frações
        !   de todos os componentes resulte em 1.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                 ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oGivenComposition          ! Vetor composição original (indexado por componente).
        real(c_double), dimension(iNComp), intent(out) :: oNormalizedComposition    ! Vetor composição normalizada (indexado por componente)

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i                                                                ! Índice de laços "do".
        real(c_double) :: dNorm                                                     ! Soma dos valores originais de todas as frações.

        ! ------------------ CÁLCULOS:

        ! Obter a soma de todos os valores originais das frações dos componentes:
        dNorm = 0.0d0
        getNorm: do i = 1, iNComp
            dNorm = dNorm + oGivenComposition(i)
        end do getNorm

        ! Normalizar valor por valor:
        normalize: do i = 1, iNComp
            oNormalizedComposition(i) = oGivenComposition(i) / dNorm
        end do normalize

    end subroutine NormComposition

    ! =============================================================
    ! =============================================================
    subroutine CalculateVLEEquilibriumRatios(iNComp, oLiqComposition, oVapComposition, oKij, oLij, dFlashPressure, dFlashTemperature, &
                                            iLiqPhaseModel, iVapPhaseModel, oTc, oPc, oW, oPeneloux, oEquilibriumRatios, iIER)

        ! OBJETIVO: Calcular as constantes de equilíbrio líquido-vapor de todos os componentes de dada mistura.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                      ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oLiqComposition    ! Vetor composição da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oVapComposition    ! Vetor composição da fase vapor (indexado por componente)
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), value, intent(in) :: dFlashPressure              ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature           ! Temperatura na qual deseja-se calcular os Ki.
        integer(c_int), value, intent(in) :: iLiqPhaseModel              ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel              ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), dimension(iNComp), intent(out) :: oEquilibriumRatios    ! Vetor de constantes de equilíbrio calculadas (indexado por componente)
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bLiquidPhaseModelIsCubicEOS                           ! "True" se o modelo selecionado para a fase líquida for uma EOS Cúbica.
        logical :: bVaporPhaseModelIsCubicEOS                            ! "True" se o modelo selecionado para a fase vapor for uma EOS Cúbica.
        logical :: bBothPhaseModelsAreCubicEOS                           ! "True" se ambas as fases estiverem sendo modeladas por EOS Cúbicas.
        logical :: bCalculateKiFromWilsonCorr                            ! "True" para calcular as contantes de equilíbrio a partir da correlação de Wilson.
        integer :: i                                                     ! Índice de laços "do".
        real(c_double) :: dEqConst
        real(c_double), dimension(iNComp) :: oLiqFugCoef           ! Vetor de coeficientes de fugacidade calculados para a fase líquida (indexado por componente).
        real(c_double), dimension(iNComp) :: oVapFugCoef           ! Vetor de coeficientes de fugacidade calculados para a fase vapor (indexado por componente).

        ! ------------------ CÁLCULOS:

        ! Proceder de acordo com os modelos selecionados para cada fase:
        bLiquidPhaseModelIsCubicEOS = (iLiqPhaseModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iLiqPhaseModel.EQ.SRK_PENELOUX).or.(iLiqPhaseModel.EQ.PENG_ROBINSON_PENELOUX)
        bVaporPhaseModelIsCubicEOS = (iVapPhaseModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iVapPhaseModel.EQ.SRK_PENELOUX).or.(iVapPhaseModel.EQ.PENG_ROBINSON_PENELOUX)
        bBothPhaseModelsAreCubicEOS = (bLiquidPhaseModelIsCubicEOS.AND.bVaporPhaseModelIsCubicEOS)
        bCalculateKiFromWilsonCorr = (iLiqPhaseModel.EQ.WILSON_EQCONST_CORRELATION).AND.(iVapPhaseModel.EQ.WILSON_EQCONST_CORRELATION)

        whichKiModel: if(bBothPhaseModelsAreCubicEOS) then

            ! Abordagem Phi-Phi usando equações cúbicas para ambas as fases!
            call CalculatePhiPhiEquilibriumRatiosFromCubicEOS(iNComp, oLiqComposition, oVapComposition, oKij, oLij, dFlashPressure, dFlashTemperature, &
                                            iLiqPhaseModel, iVapPhaseModel, oTc, oPc, oW, oPeneloux, oLiqFugCoef, oVapFugCoef, oEquilibriumRatios, iIER)

        else if(bCalculateKiFromWilsonCorr) then whichKiModel

            ! Calcular usando a correlação de Wilson:
            calcFromWilson: do i = 1, iNComp

                dEqConst = oTc(i) / dFlashTemperature
                dEqConst = 1.0d0 - dEqConst
                dEqConst = dEqConst * (1.0d0 + oW(i)) * 5.37d0
                dEqConst = exp(dEqConst)
                dEqConst = dEqConst * oPc(i) / dFlashPressure

                oEquilibriumRatios(i) = dEqConst

            end do calcFromWilson

        else whichKiModel

            ! Modelagem não-suportada.
            iIER = ERROR_InvalidKiModeling

        end if whichKiModel

    end subroutine CalculateVLEEquilibriumRatios

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhiPhiEquilibriumRatiosFromCubicEOS(iNComp, oLiqComposition, oVapComposition, oKij, oLij, dFlashPressure, dFlashTemperature, &
                                            iLiqCubicEOSModel, iVapCubicEOSModel, oTc, oPc, oW, oPeneloux, oLiqFugCoef, oVapFugCoef, oEquilibriumRatios, iIER)

        ! OBJETIVO: Calcular as constantes de equilíbrio líquido-vapor de todos os componentes de dada mistura
        !   a partir dos respectivos coeficientes de fugacidade (abordagem "phi-phi") e a partir de dada equação
        !   cúbica de estado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                      ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oLiqComposition    ! Vetor composição da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oVapComposition    ! Vetor composição da fase vapor (indexado por componente)
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), value, intent(in) :: dFlashPressure              ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature           ! Temperatura na qual deseja-se calcular os Ki.
        integer(c_int), value, intent(in) :: iLiqCubicEOSModel           ! EOS Cúbica para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapCubicEOSModel           ! EOS Cúbica para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), dimension(iNComp), intent(out) :: oLiqFugCoef           ! Vetor de coeficientes de fugacidade calculados para a fase líquida (indexado por componente).
        real(c_double), dimension(iNComp), intent(out) :: oVapFugCoef           ! Vetor de coeficientes de fugacidade calculados para a fase vapor (indexado por componente).
        real(c_double), dimension(iNComp), intent(out) :: oEquilibriumRatios    ! Vetor de constantes de equilíbrio calculadas (indexado por componente)
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oCubicEOSaParameters      ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp) :: oCubicEOSbParameters      ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double) :: dAMix                                        ! Parâmetro "a" calculado para a mistura.
        real(c_double) :: dBMix                                        ! Parâmetro "b" calculado para a mistura.
        real(c_double) :: dCMix                                        ! Parâmetro "c" (volume de "shift") calculado para a mistura.
        real(c_double) :: dZLiquid                                     ! Fator de compressibilidade da fase líquida.
        real(c_double) :: dZVapor                                      ! Fator de compressibilidade da fase vapor.
        logical :: bComplexRootsWereFound                              ! "True" se foram encontrados fatores de compressibilidade complexos.
        integer :: i                                                   ! Índice de laços "do".

        ! ------------------ CÁLCULOS:

        ! FASE LÍQUIDA:

        ! Determinar as constantes dos componentes puros nas condições do "flash":
        call CalculateCubicEOSParametersForEachComponent(iNComp, dFlashTemperature, iLiqCubicEOSModel, oTc, oPc, oW, &
                                                   iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Determinar as constantes da mistura:
        call CalculateCubicEOSParametersForMixture(iNComp, oLiqComposition, oKij, oLij, oCubicEOSaParameters, oCubicEOSbParameters, &
                                                       oPeneloux, dAMix, dBMix, dCMix)

        ! Calcular o fator de compressibilidade:
        call CalculateZFromCubicEOS(dFlashPressure, dFlashTemperature, iLiqCubicEOSModel, dAMix, dBMix, &
                                         dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular os coeficientes de fugacidade na fase líquida:
        call CalculateCubicEOSFugacityCoefficientsForEachComponent(iNComp, oLiqComposition, oKij, oLij, oCubicEOSaParameters, &
                                oCubicEOSbParameters, dZLiquid, dFlashPressure, dFlashTemperature, dAMix, dBMix, &
                                dCMix, iLiqCubicEOSModel, PHASE_Liquid, oLiqFugCoef, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! FASE VAPOR:

        ! Caso necessário, determinar novamente as constantes dos componentes puros nas condições do "flash":
        if(iVapCubicEOSModel.NE.iLiqCubicEOSModel) call CalculateCubicEOSParametersForEachComponent(iNComp, dFlashTemperature, iVapCubicEOSModel, oTc, oPc, oW, &
                                                            iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Determinar as constantes da mistura:
        call CalculateCubicEOSParametersForMixture(iNComp, oVapComposition, oKij, oLij, oCubicEOSaParameters, oCubicEOSbParameters, &
                                                       oPeneloux, dAMix, dBMix, dCMix)

        ! Calcular o fator de compressibilidade:
        call CalculateZFromCubicEOS(dFlashPressure, dFlashTemperature, iVapCubicEOSModel, dAMix, dBMix, &
                                         dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular os coeficientes de fugacidade na fase vapor:
        call CalculateCubicEOSFugacityCoefficientsForEachComponent(iNComp, oVapComposition, oKij, oLij, oCubicEOSaParameters, &
                                oCubicEOSbParameters, dZVapor, dFlashPressure, dFlashTemperature, dAMix, dBMix, &
                                dCMix, iVapCubicEOSModel, PHASE_Vapor, oVapFugCoef, iIER)

        if(iIER.NE.ERROR_EverythingOK) return


        ! CALCULAR AS CONSTANTES DE EQUILÍBRIO:
        calcK: do i = 1, iNComp
            oEquilibriumRatios(i) = oLiqFugCoef(i) / oVapFugCoef(i)
        end do calcK

    end subroutine CalculatePhiPhiEquilibriumRatiosFromCubicEOS

    ! =============================================================
    ! =============================================================
    subroutine CalculateCubicEOSParametersForEachComponent(iNComp, dFlashTemperature, iCubicEOSModel, oTc, oPc, oW, &
                                                   iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        ! OBJETIVO: Calcular os parâmetros "a" e "b" para cada componente da mistura e para
        !   determinada Equação de Estado Cúbica.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                 ! Número de componentes.
        real(c_double), value, intent(in) :: dFlashTemperature                      ! Temperatura na qual deseja-se calcular os parâmetros.
        integer(c_int), value, intent(in) :: iCubicEOSModel                         ! Índice representando o modelo com o qual se está trabalhando,
                                                                                    ! ("Peng Robinson 78 Peneloux", etc), conforme convencionado.

        real(c_double), dimension(iNComp), intent(in) :: oTc                        ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc                        ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW                         ! Vetor de fatores acêntricos dos componentes.


        integer(c_int), intent(out) :: iIER                                         ! Código de erros, conforme convencionado.
        real(c_double), dimension(iNComp), intent(out) :: oCubicEOSaParameters      ! Vetor de parâmetros "a" calculados (indexado por componente)
        real(c_double), dimension(iNComp), intent(out) :: oCubicEOSbParameters      ! Vetor de parâmetros "b" calculados (indexado por componente)

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i                                                                ! Índice de laços "do".
        real(c_double) :: dFw                                                       ! Função do fator acêntrico...
        real(c_double) :: dA                                                        ! Variável intermediária auxiliar.
        real(c_double) :: dB                                                        ! Variável intermediária auxiliar.
        real(c_double), parameter :: dR = 8.314d0                                   ! Constante dos gases.

         ! ------------------ CÁLCULOS:

         ! Proceder de acordo com a Equação de Estado Cúbica indicada:
         whichEOS: if(iCubicEOSModel.EQ.PENG_ROBINSON_78_PENELOUX) then

            ! EOS CÚBICA = "Peng Robinson 78 Peneloux"

                ! Referência desta equação de estado, para eventual consulta:
                ! "Extension of the PPR78 model (Predictive 1978, Peng-Robinson EOS with temperature dependent kij 
                !  calculated through a group contribution method) to systems containing naphtenic compounds", 
                ! publicado em 2006 na "Fluid Phase Equilibria", autores Stéphane Vitu, Jean-Noel Jaubert e
                ! Fabrice Mutelet

            PR78Pen: do i = 1, iNComp

                dFw = 0.37464d0 + (1.54226d0 * oW(i)) - (0.26992d0 * (oW(i) ** 2.0d0))

                if(oW(i).GT.(0.491d0)) dFw = 0.379642d0 + 1.48503d0 * oW(i) - 0.164423d0 * (oW(i) ** 2.0d0) + 0.016666d0 * (oW(i) ** 3.0d0)

                dA = 0.45724d0 * (dR ** 2.0d0) * (oTc(i) ** 2.0d0) / oPc(i)

                dA = dA * ((1.0d0 + dFw * (1.0d0 - sqrt(dFlashTemperature / oTc(i)))) ** 2.0d0)

                dB = 0.077796d0 * dR * oTc(i) / oPc(i)

                oCubicEOSaParameters(i) = dA
                oCubicEOSbParameters(i) = dB

            end do PR78Pen

         else if(iCubicEOSModel.EQ.PENG_ROBINSON_PENELOUX) then whichEOS

            ! EOS CÚBICA = "PR Peneloux"

            PRPen: do i = 1, iNComp

                dFw = 0.37464d0 + (1.54226d0 * oW(i)) - (0.26992d0 * (oW(i) ** 2.0d0))

                dA = 0.45724d0 * (dR ** 2.0d0) * (oTc(i) ** 2.0d0) / oPc(i)

                dA = dA * ((1.0d0 + dFw * (1.0d0 - sqrt(dFlashTemperature / oTc(i)))) ** 2.0d0)

                dB = 0.077796d0 * dR * oTc(i) / oPc(i)

                oCubicEOSaParameters(i) = dA
                oCubicEOSbParameters(i) = dB

            end do PRPen

         else if(iCubicEOSModel.EQ.SRK_PENELOUX) then whichEOS

            ! EOS CÚBICA = "SRK Peneloux"

            SRKPen: do i = 1, iNComp

                dFw = 0.48d0 + (1.574d0 * oW(i)) - (0.176d0 * (oW(i) ** 2.0d0))

                dA = 0.42748d0 * (dR ** 2.0d0) * (oTc(i) ** 2.0d0) / oPc(i)

                dA = dA * ((1.0d0 + dFw * (1.0d0 - sqrt(dFlashTemperature / oTc(i)))) ** 2.0d0)

                dB = 0.08664d0 * dR * oTc(i) / oPc(i)

                oCubicEOSaParameters(i) = dA
                oCubicEOSbParameters(i) = dB

            end do SRKPen


         else whichEOS

            ! Equação de Estado Cúbica não identificada!
            iIER = ERROR_InvalidCubicEOS

         end if whichEOS

    end subroutine CalculateCubicEOSParametersForEachComponent

    ! =============================================================
    ! =============================================================
    subroutine CalculateCubicEOSFugacityCoefficientsForEachComponent(iNComp, oMixtureComposition, oKij, oLij, oCubicEOSaParameters, &
                                oCubicEOSbParameters, dZMix, dFlashPressure, dFlashTemperature, dMixCubicEOSaShort, dMixCubicEOSbShort, &
                                dMixCubicEOSc, iCubicEOSModel, iPhaseID, oFugCoef, iIER)

        ! OBJETIVO: Calcular os coeficientes de fugacidade dos componentes em dada mistura, utilizando
        !   determinada equação cúbica de estado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                 ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMixtureComposition        ! Vetor de frações molares da mistura (indexadas por componente).
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij                ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij                ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oCubicEOSaParameters       ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oCubicEOSbParameters       ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double), intent(inout) :: dZMix                                      ! Fator de compressibilidade da mistura.
        real(c_double), value, intent(in) :: dFlashPressure                         ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature                      ! Temperatura na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dMixCubicEOSaShort                     ! Parâmetro "a" minúsculo da mistura.
        real(c_double), value, intent(in) :: dMixCubicEOSbShort                     ! Parâmetro "b" minúsculo da mistura.
        real(c_double), value, intent(in) :: dMixCubicEOSc                          ! Parâmetro "c" da mistura
        integer(c_int), value, intent(in) :: iCubicEOSModel                         ! Índice representando o modelo com o qual se está trabalhando,
                                                                                    ! ("Peng Robinson 78 Peneloux", etc), conforme convencionado.
        integer(c_int), value, intent(in) :: iPhaseID                               ! Índice da fase sendo calculada, conforme convenção.

        real(c_double), dimension(iNComp), intent(out) :: oFugCoef      ! Vetor de coeficientes de fugacidade calculados (indexado por componente).
        integer(c_int), intent(out) :: iIER                                         ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i, j                                                             ! Índices de laço "do".
        real(c_double) :: dDaDx, dDbDx
        logical :: bLijIsZero                                                       ! "True" se todos os "lij" forem nulos, "false" caso contrário.
        real(c_double) :: dVolMix                                                   ! Volume molar da mistura.
        real(c_double) :: dAux
        real(c_double) :: dEpsilon, dSigma
        real(c_double) :: dDelta, dU, dW, dRootOfU2Minus4W, dMixCapitalA, dMixCapitalB

        ! ------------------ CÁLCULOS:

        ! PONTO IMPORTANTE: o ideal seria sempre basear esta subrotina em uma equação para o coeficiente de fugacidade que levasse sempre em
        !   conta o "lij". Tal equação foi obtida junto à UFRJ, porém, apesar dos esforços junto a eles, não foi possível localizar uma
        !   referência para essa equação, devendo-se por isso adotá-la com cautela. Por outro lado, suspeita-se que raramente o "lij" será
        !   utilizado com este código. Solução encontrada: para uma matriz Lij nula, adotar a equação que pode ser encontrada na quarta edição
        !   do livro "The Properties of Liquids and Gases". Para Lij não-nula, adotar por enquanto a equação da UFRJ... :
        bLijIsZero = .TRUE.
        checkAllLijOuter: do i = 1, iNComp
            checkAllLijInner: do j = 1, iNComp
                if(ABS(oLij(i, j)).GT.(1.0D-5)) bLijIsZero = .FALSE.
            end do checkAllLijInner
        end do checkAllLijOuter

        ! Proceder de acordo com a verificação da matriz "Lij":
        selectFugCoefEquation: if(bLijIsZero) then

            ! Todos os "lij" são nulos! Adotar a equação referenciada.

            dMixCapitalA = dMixCubicEOSaShort * dFlashPressure / ((8.314d0 * dFlashTemperature) ** 2.0d0)
            dMixCapitalB = dMixCubicEOSbShort * dFlashPressure / 8.314d0 / dFlashTemperature

            ! 16-DEZ-2021: - A experiência mostrou que o método numérico implementado pela subrotina abaixo não fez muita diferença.
            !              - A subrotina foi então considerada "obsoleta", e por isso a chamada a ela está comentada abaixo.
            !              - Optou-se por COMENTAR em vez de REMOVER com o objetivo de não perder o ponto exato do cálculo em que
            !                   a subrotina precisa ser chamada caso seja necessário testar novamente a aplicação desse método.
            !call PerformCubicEOSPhiPhiFlashNumericalAdaptations(iPhaseID, iCubicEOSModel, dMixCapitalA, dMixCapitalB, dZMix, iIER)

            ! Para cada componente...
            outerLoopOne: do i = 1, iNComp

                ! Inicializando:
                dDelta = 0.0d0

                innerLoopOne: do j = 1, iNComp
                    dDelta = dDelta + oMixtureComposition(j) * sqrt(oCubicEOSaParameters(j)) * (1.0d0 - oKij(i, j))
                end do innerLoopOne

                dDelta = 2.0d0 * sqrt(oCubicEOSaParameters(i)) / dMixCubicEOSaShort * dDelta

                ! Obter o resultado (Tabela 5-13 da 4a edição de "The Properties of Liquids and Gases"):
                call GetCubicEOSGeneralFormParameters(iCubicEOSModel, dU, dW, iIER)
                if(iIER.NE.ERROR_EverythingOK) return

                dRootOfU2Minus4W = sqrt(dU * dU - 4.0d0 * dW)

                oFugCoef(i) = oCubicEOSbParameters(i) / dMixCubicEOSbShort * (dZMix - 1.0d0)
                oFugCoef(i) = oFugCoef(i) - log(dZMix - dMixCapitalB)
                oFugCoef(i) = oFugCoef(i) + (dMixCapitalA / dMixCapitalB / dRootOfU2Minus4W) * &
                    (oCubicEOSbParameters(i) / dMixCubicEOSbShort - dDelta) * &
                    log((2.0d0 * dZMix + dMixCapitalB * (dU + dRootOfU2Minus4W)) / (2.0d0 * dZMix + dMixCapitalB * (dU - dRootOfU2Minus4W)))

                oFugCoef(i) = exp(oFugCoef(i))

            end do outerLoopOne

        else selectFugCoefEquation

            ! Existem "Lij" não-nulos. Adotar a equação (sem referência encontrada) da UFRJ...

            ! Parâmetros da equação de estado cúbica...
            whichEOS: if(iCubicEOSModel.EQ.PENG_ROBINSON_78_PENELOUX) then

                ! Peng-Robinson!
                dSigma = 1.0d0 + sqrt(2.0d0)
                dEpsilon = 1.0d0 - sqrt(2.0d0)
            
            else if(iCubicEOSModel.EQ.PENG_ROBINSON_PENELOUX) then whichEOS

                ! Peng-Robinson!
                dSigma = 1.0d0 + sqrt(2.0d0)
                dEpsilon = 1.0d0 - sqrt(2.0d0)

                ! NOTA (05/03/2026): Uma possível fonte para estas constantes é a pág 75 do livro "Thermodynamic Models: Fundamentals
                !                    and Computational Aspects", de M. Michelsen e J. Mollerup, 2a edição. IMPORTANTE: em comparação
                !                    com esta referência bibliográfica, constata-se que as constantes acima estão atribuídas na ordem
                !                    contrária às da equação de SRK (abaixo). Vale a pena conferir isto, especialmente se o emprego deste
                !                    código (inexistente até o momento) fornecer resultados errados.

            else if(iCubicEOSModel.EQ.SRK_PENELOUX) then whichEOS

                ! SRK!
                dSigma = 0.0d0
                dEpsilon = 1.0d0

            else whichEOS

                ! Equação de Estado Cúbica não identificada!
                iIER = ERROR_InvalidCubicEOS
                return

            end if whichEOS

            ! Grandeza intermediária auxiliar:
            dAux = dFlashPressure / (8.314d0 * dFlashTemperature)

            ! Calcular o volume molar da mistura:
            dVolMix = dZMix * 8.314d0 * dFlashTemperature / dFlashPressure

            ! Para cada componente...
            outerLoop: do i = 1, iNComp

                ! Inicializando...
                dDaDx = 0.0d0
                dDbDx = 0.0d0

                ! Para cada componente...
                innerLoop: do j = 1, iNComp

                    dDaDx = dDaDx + oMixtureComposition(j) * sqrt(oCubicEOSaParameters(i) * oCubicEOSaParameters(j)) * (1.0d0 - oKij(i, j))
                    dDbDx = dDbDx + oMixtureComposition(j) * (oCubicEOSbParameters(i) + oCubicEOSbParameters(j)) * (1.0d0 - oLij(i, j))

                end do innerLoop

                dDaDx = dDaDx * 2.0d0

                dVolMix = dVolMix + dMixCubicEOSc

                oFugCoef(i) = -1.0d0 * dMixCubicEOSaShort / (dMixCubicEOSbShort * 8.314d0 * dFlashTemperature * (dEpsilon - dSigma))
                oFugCoef(i) = oFugCoef(i) * (dDaDx / dMixCubicEOSaShort - (dDbDx - dMixCubicEOSbShort) / dMixCubicEOSbShort)
                oFugCoef(i) = oFugCoef(i) * log((dVolMix + dEpsilon * dMixCubicEOSbShort) / (dVolMix + dSigma * dMixCubicEOSbShort))
                oFugCoef(i) = oFugCoef(i) - dMixCubicEOSc * dAux
                oFugCoef(i) = oFugCoef(i) + (dDbDx - dMixCubicEOSbShort) / dMixCubicEOSbShort * (dVolMix * dAux - 1.0d0)
                oFugCoef(i) = oFugCoef(i) - log((dVolMix - dMixCubicEOSbShort) * dAux)
                oFugCoef(i) = exp(oFugCoef(i))

                dVolMix = dVolMix - dMixCubicEOSc

            end do outerLoop

        end if selectFugCoefEquation

    end subroutine CalculateCubicEOSFugacityCoefficientsForEachComponent

    ! =============================================================
    ! =============================================================
    subroutine CalculateCubicEOSParametersForMixture(iNComp, oMixtureComposition, oKij, oLij, oCubicEOSaParameters, oCubicEOSbParameters, &
                                                       oPeneloux, dAMix, dBMix, dCMix)

        ! OBJETIVO: Calcular os parâmetros "a", "b" e "c" de uma equação de estado cúbica para dada mistura de componentes.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                 ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMixtureComposition        ! Vetor de frações molares da mistura (indexadas por componente).
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij                ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij                ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oCubicEOSaParameters       ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oCubicEOSbParameters       ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux                  ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), intent(out) :: dAMix                                        ! Parâmetro "a" calculado para a mistura.
        real(c_double), intent(out) :: dBMix                                        ! Parâmetro "b" calculado para a mistura.
        real(c_double), intent(out) :: dCMix                                        ! Parâmetro "c" (volume de "shift") calculado para a mistura.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i, j                                                             ! Índices de laços "do".
        real(c_double) :: dAi, dBi, dAj, dBj                                        ! Parâmetros "a" e "b" dos componentes i e j (variáveis intermediárias auxiliares)
        real(c_double) :: dXi, dXj                                                  ! Frações molares dos componentes i e j (variáveis intermediárias auxiliares).
        real(c_double) :: dKij                                                      ! Parâmetro de interação kij (variável auxiliar intermediária).
        real(c_double) :: dLij                                                      ! Parâmetro lij (variável auxiliar intermediária).

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dAMix = 0.0d0
        dBMix = 0.0d0
        dCMix = 0.0d0

        ! Para cada par de componentes... :
        loopComp1: do i = 1, iNComp

            ! Parâmetros "a" e "b" do componente "i":
            dAi = oCubicEOSaParameters(i)
            dBi = oCubicEOSbParameters(i)

            ! Fração molar do componente "i":
            dXi = oMixtureComposition(i)

            ! Incrementar devidamente o "c" da mistura:
            dCMix = dCMix + (dXi * oPeneloux(i))

            loopComp2: do j = 1, iNComp

                ! Parâmetros "a" e "b" do componente "j":
                dAj = oCubicEOSaParameters(j)
                dBj = oCubicEOSbParameters(j)

                ! Fração molar do componente "j":
                dXj = oMixtureComposition(j)

                ! Parâmetro de interação binária deste par de substâncias:
                dKij = oKij(i, j)

                ! Aplicar a Regra da Mistura:
                dAMix = dAMix + (dXi * dXj * sqrt(dAi * dAj) * (1.0d0 - dKij))

                ! Pegar o "Lij":
                dLij = oLij(i, j)

                ! Aplicar novamente a Regra da Mistura (para o "b"):
                dBMix = dBMix + (dXi * dXj * (dBi + dBj) * (1.0d0 - dLij))

            end do loopComp2

        end do loopComp1

        ! Corrigindo o "bMix":
        dBMix = 0.5d0 * dBmix

    end subroutine CalculateCubicEOSParametersForMixture

    ! =============================================================
    ! =============================================================
    subroutine CalculateZFromCubicEOS(dFlashPressure, dFlashTemperature, iCubicEOSModel, dCubicEOSaShort, dCubicEOSbShort, &
                                         dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        ! OBJETIVO: Calcular as raízes (fatores de compressibilidade) a partir de equações cúbicas de estado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iCubicEOSModel                  ! Índice representando o modelo com o qual se está trabalhando,
                                                                             ! ("Peng Robinson 78 Peneloux", etc), conforme convencionado.
        real(c_double), value, intent(in) :: dCubicEOSaShort                 ! Parâmetro "a" minúsculo da equação de estado cúbica.
        real(c_double), value, intent(in) :: dCubicEOSbShort                 ! Parâmetro "b" minúsculo da equação de estado cúbica.

        real(c_double), intent(out) :: dZLiquid                              ! Fator de compressibilidade da fase líquida.
        real(c_double), intent(out) :: dZVapor                               ! Fator de compressibilidade da fase vapor.
        logical, intent(out) :: bComplexRootsWereFound                       ! "True" se foram encontrados fatores de compressibilidade complexos.
        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dCapitalA, dCapitalB                               ! Parâmetro A e B maiúsculos (adimensionais), conforme definições em
                                                                             !  "The Properties of Liquids and Gases - Fourth Edition".
        real(c_double) :: dU, dW                                             ! Parâmetros "u" e "w" da forma geral da equação de estado cúbica.
        real(c_double) :: dCoefA1, dCoefA2, dCoefA3                          ! Coeficientes da equação cúbica em "Z"
        real(c_double) :: dCubicQ, dCubicR                                   ! Variáveis intermediárias na solução da equação cúbica.
        real(c_double) :: dCubicD                                            ! "Discriminante" calculado na solução da equação cúbica.
        real(c_double) :: dS1, dS2
        real(c_double) :: dZSingleRealOrPhysicallyMeaningless                ! Raíz sem significado físico, ou única raíz real encontrada
        real(c_double) :: dDifferent, dEqual
        real(c_double) :: dAngle, dAngle1, dAngle2, dAngle3                  ! Ãngulos necessários para cômputo das raízes da equação cúbica.
        real(c_double) :: dZ1, dZ2, dZ3, dTemp
        real(c_double) :: dSqrtOfCubicD

        ! ------------------ CÁLCULOS:

        ! Calcular "A" e "B" maiúsculos:
        dCapitalA = dCubicEOSaShort * dFlashPressure / ((8.314d0 * dFlashTemperature) ** 2.0d0)
        dCapitalB = dCubicEOSbShort * dFlashPressure / 8.314d0 / dFlashTemperature

        ! Obter os coeficientes da equação cúbica na forma "Z^3 + a1*Z^2 + a2*Z + a3 = 0":
        call GetCubicEOSGeneralFormParameters(iCubicEOSModel, dU, dW, iIER)
        if(iIER.NE.ERROR_EverythingOK) return

        dCoefA1 = -1.0d0 * (1.0d0 + dCapitalB - dU * dCapitalB)
        dCoefA2 = dCapitalA + dW * (dCapitalB ** 2.0d0) - dU * dCapitalB - dU * (dCapitalB ** 2.0d0)
        dCoefA3 = -1.0d0 * dCapitalA * dCapitalB - dW * (dCapitalB ** 2.0d0) - dW * (dCapitalB ** 3.0d0)

        ! Obter "Q" e "R":
        dCubicQ = (3.0d0 * dCoefA2 - (dCoefA1 ** 2.0d0)) / 9.0d0
        dCubicR = (9.0d0 * dCoefA1 * dCoefA2 - 27.0d0 * dCoefA3 - 2.0d0 * (dCoefA1 ** 3.0d0)) / 54.0d0

        ! Obter o "discriminante":
        dCubicD = (dCubicQ ** 3.0d0) + (dCubicR ** 2.0d0)

        ! Verificar se há raizes complexas:
        bComplexRootsWereFound = dCubicD.GT.(0.0d0)

        ! Proceder conforme o caso:
        rootNature: if(bComplexRootsWereFound) then

            ! Há somente uma raíz real, e duas complexas conjugadas!
            ! Determinar a única real (sem significado físico):
            dSqrtOfCubicD = sqrt(dCubicD)

            ! Evitando erro de ponto flutuante no cálculo da outra raiz
            dS1 = dCubicR + dSqrtOfCubicD
            dS1 = sign(dabs(dS1) ** (1.0d0 / 3.0d0), dS1)
            dS2 = dCubicR - dSqrtOfCubicD
            dS2 = sign(dabs(dS2) ** (1.0d0 / 3.0d0), dS2)

            dZSingleRealOrPhysicallyMeaningless = dS1 + dS2 - dCoefA1 / 3.0d0

            ! Considerar que em altas pressões, a equação cúbica talvez se torne monotônica
            ! sem que isso configure erro...
            dZLiquid = dZSingleRealOrPhysicallyMeaningless
            dZVapor = dZSingleRealOrPhysicallyMeaningless

        else rootNature

            ! Há três raízes reais!
            disciminantZero: if(dCubicD.EQ.(0.0d0)) then

                ! Há três raízes reais, com ao menos duas repetidas!
                dDifferent = 2.0d0 * (dCubicR ** (1.0d0 / 3.0d0)) - dCoefA1 / 3.0d0
                dEqual = -1.0d0 * (dCubicR ** (1.0d0 / 3.0d0)) - dCoefA1 / 3.0d0

                if(dDifferent.GT.dEqual) then
                    dZVapor = dDifferent
                    dZLiquid = dEqual
                    dZSingleRealOrPhysicallyMeaningless = dEqual
                else
                    dZLiquid = dDifferent
                    dZVapor = dEqual
                    dZSingleRealOrPhysicallyMeaningless = dEqual
                end if

            else disciminantZero

                ! Há três raízes reais distintas!
                ! Obter essas três raízes!

                ! Ângulo em radianos:
                dAngle = ACOS(dCubicR / sqrt(-1.0d0 * (dCubicQ ** 3.0d0)))

                ! Convertendo para graus:
                dAngle = dAngle * (180.0d0 / 3.141593d0)

                ! Ângulos necessários para o cômputo das raízes:
                dAngle1 = dAngle / 3.0d0 +120.0d0
                dAngle2 = dAngle / 3.0d0 +240.0d0
                dAngle3 = dAngle / 3.0d0

                ! Calcular as três raízes:
                dZ1 = 2.0d0 * sqrt(-1.0d0 * dCubicQ) * cos(dAngle1 * 3.141593d0 / 180.0d0) - dCoefA1 / 3.0d0
                dZ2 = 2.0d0 * sqrt(-1.0d0 * dCubicQ) * cos(dAngle2 * 3.141593d0 / 180.0d0) - dCoefA1 / 3.0d0
                dZ3 = 2.0d0 * sqrt(-1.0d0 * dCubicQ) * cos(dAngle3 * 3.141593d0 / 180.0d0) - dCoefA1 / 3.0d0

                ! Ordenar as três raízes:
                if(dZ1.GT.dZ2) then
                    dTemp = dZ2
                    dZ2 = dZ1
                    dZ1 = dTemp
                end if

                if(dZ3.LE.dZ1) then
                    dTemp = dZ3
                    dZ3 = dZ2
                    dZ2 = dZ1
                    dZ1 = dTemp
                else if((dZ3.GT.dZ1).AND.(dZ3.LE.dZ2)) then
                    dTemp = dZ3
                    dZ3 = dZ2
                    dZ2 = dTemp
                end if

                ! Finalmente, atribuir as três raízes distintas a seus "significados
                ! termodinâmicos", de acordo com a ordem:
                dZLiquid = dZ1
                dZSingleRealOrPhysicallyMeaningless = dZ2
                dZVapor = dZ3

            end if disciminantZero

        end if rootNature

    end subroutine CalculateZFromCubicEOS

    ! =============================================================
    ! =============================================================
    subroutine GetCubicEOSGeneralFormParameters(iCubicEOSModel, dU, dW, iIER)

        ! OBJETIVO: Fornecer sempre que necessário os parâmetros "u" e "w" de equações de estado cúbicas,
        !   conforme forma geral apresentada em "The Properties of Liquids and Gases - Fourth Edition - 
        !   Reid, Prausnitz e Poling".
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iCubicEOSModel                         ! Índice representando o modelo com o qual se está trabalhando,
                                                                                    ! ("Peng Robinson 78 Peneloux", etc), conforme convencionado.
        real(c_double), intent(out) :: dU                                           ! Parâmetro "u" desejado
        real(c_double), intent(out) :: dW                                           ! Parâmetro "w" desejado
        integer(c_int), intent(out) :: iIER                                         ! Código de erros, conforme convencionado.

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dU = -10.0d0
        dW = -10.0d0

        ! Proceder de acordo com o modelo especificado:
        whichEOS: if(iCubicEOSModel.EQ.PENG_ROBINSON_78_PENELOUX) then

            dU = 2.0d0
            dW = -1.0d0

        else if(iCubicEOSModel.EQ.PENG_ROBINSON_PENELOUX) then whichEOS

            dU = 2.0d0
            dW = -1.0d0

        else if(iCubicEOSModel.EQ.SRK_PENELOUX) then whichEOS

            dU = 1.0d0
            dW = 0.0d0

        else whichEOS

            ! Equação de Estado Cúbica não identificada!
            iIER = ERROR_InvalidCubicEOS

        end if whichEOS

    end subroutine GetCubicEOSGeneralFormParameters

end module VLECalculations
