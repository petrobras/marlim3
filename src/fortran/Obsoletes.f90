module Obsoletes

        ! OBJETIVO: Conter rotinas e funcionalidades em vias de serem descontinuadas.
        ! ATENÇÃO: É possível que o núcleo principal desta biblioteca ainda contenha chamadas a uma ou mais funcionalidades
        !   abaixo, cuja remoção de lá ainda esteja em andamento. Por isso, é necessário CRITÉRIO antes de excluir qualquer
        !   conteúdo do presente módulo.
        ! UNIDADES: TODA a programação contida neste módulo considera que TODOS os valores (entradas e saídas
        !   das rotinas e suas variáveis internas) estão no SI, EXCETO QUANTO devidamente explicitado...

    use VLECalculations
    use PhaseProperties

    implicit none


    contains
! ===================================================================================================
!               SUBROTINAS...
! ===================================================================================================

    ! A subrotina abaixo era a que se usava originalmente para calcular "flash", composições das fases em equilíbrio e
    ! a própria condição termodinâmica do fluido de interesse. Foi reescrita visando preservar suas funcionalidades
    ! principais e remover as desnecessárias, reordenando-se a sequencia interna de modo a atingir-se desempenho superior.

    subroutine CalculateMixtureThermodynamicCondition(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                    bHasInitialFlashEstimates, dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                    dCalculatedBubbleT, iIER_BubbleT, &
                    dCalculatedDewT, iIER_DewT, dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, iIER_Flash, &
                    dCalculatedBubbleP, iIER_BubbleP, iCalculatedThermodynamicCondition, iIER)

        ! OBJETIVO: Determinar a condição termodinâmica de uma mistura especificada sob dadas condições de T e P,
        !       junto com todos os resultados associados.
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
        logical, intent(in) :: bHasInitialFlashEstimates                     ! "True" caso se deseje fornecer estimativas iniciais para o cálculo de "flash".
                                                                             ! "False" para a própria rotina calcular essas estimativas.
        real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash".
        real(c_double), dimension(iNComp), intent(in) :: oGivenInitialLiqComposition    ! Estimativa inicial de composição da fase líquida para o "flash".
        real(c_double), dimension(iNComp), intent(in) :: oGivenInitialVapComposition    ! Estimativa inicial de composição da fase vapor para o "flash".

        real(c_double), intent(out) :: dCalculatedBubbleT                    ! Valor calculado da temperatura de bolha.
        integer(c_int), intent(out) :: iIER_BubbleT                          ! Código de erros na determinação da temperatura de bolha, conforme convencionado.
        real(c_double), intent(out) :: dCalculatedDewT                       ! Valor calculado da temperatura de orvalho.
        integer(c_int), intent(out) :: iIER_DewT                             ! Código de erros na determinação da temperatura de orvalho, conforme convencionado.
        real(c_double), intent(out) :: dCalculatedBeta                              ! Fração molar vaporizada determinada.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedLiqComposition ! Composição molar determinada da fase líquida.
        real(c_double), dimension(iNComp), intent(out) :: oCalculatedVapComposition ! Composição molar determinada da fase vapor.
        integer(c_int), intent(out) :: iIER_Flash                                   ! Código de erros no cálculo do "flash", conforme convencionado.

        real(c_double), intent(out) :: dCalculatedBubbleP                           ! Pressão de bolha calculada.
        integer(c_int), intent(out) :: iIER_BubbleP                                 ! Código de erros no cálculo da pressão de bolha, conforme convencionado.


        integer(c_int), intent(out) :: iCalculatedThermodynamicCondition            ! Código da condição termodinâmica determinada, conforme convencionado.
        integer(c_int), intent(out) :: iIER                                         ! Código de erros "geral" desta rotina, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dTInitial                                          ! Estimativa inicial da temperatura desejada.
        real(c_double) :: dCalculatedTemperature
        logical :: bIsSubcooledLiquid                                        ! "True" caso a mistura esteja na condição de "líquido subresfriado".
        logical :: bIsSuperheatedVapor                                       ! "True" caso a mistura esteja na condição de "vapor superaquecido".
        logical :: bCalculateFlash                                           ! "True" caso o "flash" precise ser calculado nas presentes condições.
        logical :: bGotEstimateForBubblePoint                                   ! "True" se conseguir estimativas para o ponto de bolha.
        real(c_double) :: dClosestToBubbleT                                     ! Temperatura mais próxima encontrada da temperatura de bolha.
        real(c_double), dimension(iNComp) :: oVapCompositionClosestToBubbleT    ! Composição da fase vapor correspondente a "dClosestToBubbleT".
        logical :: bGotEstimateForDewPoint                                      ! "True" se conseguir estimativas para o ponto de orvalho.
        real(c_double) :: dClosestToDewT                                        ! Temperatura mais próxima encontrada da temperatura de orvalho.
        real(c_double), dimension(iNComp) :: oLiqCompositionClosestToDewT       ! Composição da fase líquida correspondente a "dClosestToDewT".
        logical :: bHasInitialEstimates                             ! "True" caso possua chutes iniciais para o processo iterativo do "flash".
        real(c_double) :: dInitialBeta                              ! Estimativa inicial da fração molar vaporizada, caso exista.
        real(c_double), dimension(iNComp) :: oInitialLiqComposition ! Estimativa inicial da composição da fase líquida, caso exista.
        real(c_double), dimension(iNComp) :: oInitialVapComposition ! Estimativa inicial da composição da fase vapor, caso exista.
        logical :: bPStepwiseDetectedSubcooledLiq               ! "True" se o cálculo "stepwise" na pressão determinou que a mistura está subresfriada.
        real(c_double) :: dPStepwiseRoughBubblePEstimate        ! Estimativa da pressão de bolha encontrada pelo cálculo "pressure stepwise".
        integer :: iIER_PStepwiseBubbleOrDew
        real(c_double) :: dBubblePObjectiveFunction             ! Valor da função objetivo na pressão de bolha calculada (nem sempre disponível)
        logical :: bCalculateDewPressure                        ! "True" caso se conclua que será necessário tentar calcular a pressão de orvalho.
        real(c_double) :: dCalculatedDewP                       ! Pressão de Orvalho calculada, se for o caso.
        real(c_double) :: dDewPObjectiveFunction                ! Valor da função objetivo correspondente à pressão de orvalho obtida
        integer(c_int) :: iIER_DewP                             ! Indicador de erros no cálculo da pressão de orvalho, conforme convenção.
        logical :: bDewPressureValueIsAvailable                 ! "True" caso esteja disponível um valor de pressão de orvalho, "false" caso contrário
        logical :: bDewOrBubblePIsAvailable                     ! "True" caso esteja disponível a pressão de bolha ou de orvalho
        logical :: bSuspectedDryGas                             ! "True" caso sejam encontrados indícios de fluido "dry gas"
        logical :: bBypassBubbleAndDewPCalculations             ! "True" para "bypassar" cálculos de pressão de bolha e orvalho, "false" para não bypassar

        ! ------------------ CONSTANTES NECESSÁRIAS:
        logical, parameter :: bTryToCalculateBubbleAndDewTemp = .false.     ! "True" para tentar o cálculo das temperaturas de bolha e orvalho

        ! ------------------ CÁLCULOS:

        ! Ajustar estimativas iniciais:
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



        calcOrNotBubbleAndDewT: if(bTryToCalculateBubbleAndDewTemp) then

            ! PASSO 1: Calcular a temperatura de bolha:

            ! Estimativa inicial por Wilson:
            iIER_BubbleT = ERROR_EverythingOK

            if(.NOT.bGotEstimateForBubblePoint) then
                dTInitial = -100.0d0
                call CalculateBubbleOrDewTemperature(dFlashPressure, dTInitial, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, WILSON_EQCONST_CORRELATION, WILSON_EQCONST_CORRELATION, CALCTYPE_BubblePoint, &
                    bGotEstimateForBubblePoint, oVapCompositionClosestToBubbleT, dCalculatedTemperature, iIER_BubbleT)
            else
                dCalculatedTemperature = dClosestToBubbleT
            end if

            ! Cálculo para valer:
            if(iIER_BubbleT.NE.ERROR_EverythingOK) then
                dCalculatedTemperature = -100.0d0
                iIER_BubbleT = ERROR_EverythingOK
            end if

            call CalculateBubbleOrDewTemperature(dFlashPressure, dCalculatedTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, &
                    bGotEstimateForBubblePoint, oVapCompositionClosestToBubbleT, dCalculatedBubbleT, iIER_BubbleT)

            ! PASSO 2: Calcular a temperatura de orvalho:

            ! Estimativa inicial por Wilson:
            iIER_DewT = ERROR_EverythingOK

            if(.not.bGotEstimateForDewPoint) then
                dTInitial = -100.0d0
                call CalculateBubbleOrDewTemperature(dFlashPressure, dTInitial, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, WILSON_EQCONST_CORRELATION, WILSON_EQCONST_CORRELATION, CALCTYPE_DewPoint, &
                    bGotEstimateForDewPoint, oLiqCompositionClosestToDewT, dCalculatedTemperature, iIER_DewT)
            else
                dCalculatedTemperature = dClosestToDewT
            end if

            ! Cálculo para valer:
            if(iIER_DewT.NE.ERROR_EverythingOK) then
                dCalculatedTemperature = -100.0d0
                iIER_DewT = ERROR_EverythingOK
            end if

            call CalculateBubbleOrDewTemperature(dFlashPressure, dCalculatedTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_DewPoint, &
                    bGotEstimateForDewPoint, oLiqCompositionClosestToDewT, dCalculatedDewT, iIER_DewT)

        else calcOrNotBubbleAndDewT
            ! Sinalizar para que não se tente utilizar valores não-calculados de T Bolha e Orvalho:
            iIER_BubbleT = -1
            iIER_DewT = -1
        end if calcOrNotBubbleAndDewT


        ! PASSO 3: Calcular a pressão de bolha:
        bBypassBubbleAndDewPCalculations = bHasInitialFlashEstimates
        bBypassBubbleAndDewPCalculations = bBypassBubbleAndDewPCalculations.AND.(dGivenInitialBeta.GT.(0.10d0))
        bBypassBubbleAndDewPCalculations = bBypassBubbleAndDewPCalculations.AND.(dGivenInitialBeta.LT.(0.995d0))

        checkIfShouldTryBubbleAndDewP: if(.not.bBypassBubbleAndDewPCalculations) then

            iIER_BubbleP = ERROR_EverythingOK

            call SecantMethod_CalculateBubbleOrDewPressure(-10.0d0, -10.0d0, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, bGotEstimateForBubblePoint, &
                oVapCompositionClosestToBubbleT, dCalculatedBubbleP, dBubblePObjectiveFunction, iIER_BubbleP)

            ! =========================================================
            !   INSERÇÃO EM 9/3/2021

            ! PASSO 4: Tentar o cálculo da pressão de orvalho, se for o caso:
            bCalculateDewPressure = (iIER_BubbleP.NE.ERROR_EverythingOK)

            calculatingDewP: if(bCalculateDewPressure) then

                    iIER_DewP = ERROR_EverythingOK

                    call SecantMethod_CalculateBubbleOrDewPressure(-10.0d0, -10.0d0, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                        oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_DewPoint, bGotEstimateForDewPoint, &
                        oLiqCompositionClosestToDewT, dCalculatedDewP, dDewPObjectiveFunction, iIER_DewP)

                    bDewPressureValueIsAvailable = (iIER_DewP.EQ.ERROR_EverythingOK)

            else calculatingDewP
                ! Não vai calcular a pressão de orvalho:
                bDewPressureValueIsAvailable = .false.
            end if calculatingDewP

            !   FIM DA INSERÇÃO EM 9/3/2021
            ! =========================================================

        else checkIfShouldTryBubbleAndDewP
            ! Pressão de bolha e orvalho não será calculada!
            iIER_BubbleP = -1
            dCalculatedBubbleP = -10000.0d0
            bCalculateDewPressure = .false.
            iIER_DewP = -1
            dCalculatedDewP = -10000.0d0
            bDewPressureValueIsAvailable = .false.
        end if checkIfShouldTryBubbleAndDewP





        ! ========================================
        !   INSERÇÃO EM 2/6/2021
        bDewOrBubblePIsAvailable = (iIER_BubbleP.EQ.ERROR_EverythingOK).or.bDewPressureValueIsAvailable

        if(bDewOrBubblePIsAvailable) then
            ! OBS: Este "if" parece desnecessário. Sua eliminação ou manutenção não parece produzir qualquer impacto.
            iIER_BubbleT = -1
            iIER_DewT = -1
        end if

        !   FIM DA INSERÇÃO EM 2/6/2021
        ! =========================================

        ! PASSO 1a / 2a: Nova tentativa de cálculo da temperatura de bolha / orvalho, em caso de erro, e cálculo da pressão de bolha:
        call CalcPressureStepwiseBubbleAndDewVariables(-10.0d0, dFlashPressure, dFlashTemperature, iNComp, &
                                                         oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                                                         oPeneloux, .false., &
                                                         .false., .FALSE., iIER_BubbleT, &
                                                         dCalculatedBubbleT, iIER_DewT, dCalculatedDewT, iIER_BubbleP, dCalculatedBubbleP, &
                                                         iIER_PStepwiseBubbleOrDew)

        ! PASSO 3: Calcular, se for o caso, o equilíbrio de fases.

            ! O equilíbrio de fases não será calculado caso já se saiba COM CERTEZA que há
            ! somente uma fase presente.

            bIsSubcooledLiquid = (iIER_BubbleT.EQ.ERROR_EverythingOK).AND.(dFlashTemperature.LT.dCalculatedBubbleT)
            bIsSuperheatedVapor = (iIER_DewT.EQ.ERROR_EverythingOK).AND.(dFlashTemperature.GT.dCalculatedDewT)

                ! Basear-se na pressão de bolha pode fornecer conclusões mais seguras, o que foi observado
                ! principalmente em altas pressões... mas dá pra reconhecer aqui que o ideal seria basear-se
                ! no ponto crítico, igual o PVTSim faz.
                checkFromBubbleP: if(iIER_BubbleP.EQ.ERROR_EverythingOK) then

                    bIsSubcooledLiquid = (dFlashPressure.GT.dCalculatedBubbleP)

                    if(bIsSubcooledLiquid) bIsSuperheatedVapor = .FALSE.

                ! ++++++++++++++++++++++++++++++++++++
                !   INSERÇÃO EM 10/3/2021
                else if(bDewPressureValueIsAvailable) then checkFromBubbleP

                    ! Já que não dispomos de uma pressão de bolha, verificar pela pressão de orvalho:
                    !bIsSubcooledLiquid = (dFlashPressure.GT.dCalculatedDewP)
                    !bIsSuperheatedVapor = bIsSubcooledLiquid

                ! FIM DA INSERÇÃO EM 10/3/2021
                ! ++++++++++++++++++++++++++++++++++++

                ! CORREÇÃO EM 2/12/2021
                bIsSuperheatedVapor = (dFlashPressure.GT.dCalculatedDewP)
                bIsSubcooledLiquid = .FALSE.

                end if checkFromBubbleP

            bCalculateFlash = (bIsSubcooledLiquid.OR.bIsSuperheatedVapor)
            bCalculateFlash = (.NOT.bCalculateFlash)

            ! ++++++++++++++++++++++++++++++++++++
            !   TESTE - APAGAR BLOCO - 3/12/2020
            ! bCalculateFlash = .true.
            !   FIM DO TESTE - APAGAR BLOCO
            ! ++++++++++++++++++++++++++++++++++++

            calcFlashOrNot: if(bCalculateFlash) then

                ! Inicialização:
                bPStepwiseDetectedSubcooledLiq = .false.

                ! Calcular o "flash".
                iIER_Flash = ERROR_EverythingOK

                call CalcFlashPTVLE(dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition, dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, iIER_Flash)

                if(iIER_Flash.NE.ERROR_EverythingOK) then

                    ! Verificar se o cálculo com pressões sucessivas não funciona....
                    iIER_Flash = ERROR_EverythingOK

                    call CalcPressureStepwiseFlashPTVLE(-10.0d0, dFlashPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, &
                        iLiqPhaseModel, iVapPhaseModel, oPeneloux, (iIER_BubbleT.EQ.ERROR_EverythingOK), dCalculatedBeta, &
                        oCalculatedLiqComposition, oCalculatedVapComposition, bPStepwiseDetectedSubcooledLiq, dPStepwiseRoughBubblePEstimate, iIER_Flash)

                end if

                ! ================================
                !   INSERÇÃO EM 19/7/2021
                testTooCloseToBubbleP: if(iIER_Flash.EQ.ERROR_FlashCalculationFoundTrivialSolution) then
                    if((iIER_BubbleP.EQ.ERROR_EverythingOK).AND. &
                       (abs((dFlashPressure-dCalculatedBubbleP)/dCalculatedBubbleP).LT.(0.04d0))) then
                        ! Foi detectado que este tipo específico de "erro" acaba sendo gerado em pressões
                        ! próximas demais da Pressão de Bolha. É bom lembrar que o cálculo da pressão de bolha
                        ! possui incertezas e tolerâncias associadas - ou seja, próximo demais dela, é perfeitamente
                        ! possível que a mistura esteja monofásica, e que o "flash" não tenha de fato uma solução
                        ! bifásica. Será suposto aqui que nestas circunstâncias específicas, é isto que está acontecendo!
                        bPStepwiseDetectedSubcooledLiq = .true.
                        iIER_Flash = ERROR_EverythingOK
                    end if
                end if testTooCloseToBubbleP

                ! PAREI AQUI EM 23/7/2021
                ! Inserção conferida!

                !   FIM DA INSERÇÃO EM 19/7/2021
                ! ==============================

                ! ===============================
                !   INSERÇÃO EM 26/10/2021
                bSuspectedDryGas = .false.
                testForDryGas: if(iIER_Flash.EQ.ERROR_NoFlashConvergenceInLowerPressures) then
                    if(bDewPressureValueIsAvailable) then
                        if((dCalculatedDewP.ge.(80.0d5)).and.(dCalculatedDewP.le.(120.0d5))) then
                            if(dFlashTemperature.le.(60.0d0 + 273.15d0)) then
                                ! Com muita probabilidade, estamos falando de "dry gas"!
                                ! Dificilmente um cálculo de "flash" vai encontrar fração vaporizada molar menor que 90%.
                                ! Se o "flash" não encontrou resposta, é seguro retornar 100% vaporizados.
                                iIER_Flash = ERROR_EverythingOK
                                bSuspectedDryGas = .true.
                                dCalculatedBeta = 100.0d0
                            end if
                        end if
                    end if
                end if testForDryGas
                !   FIM DA INSERÇÃO EM 26/10/2021
                ! ===============================

                if(iIER_Flash.NE.ERROR_EverythingOK) then
                    ! Erro!
                    iIER = iIER_Flash
                    return
                end if

                ! Determinar a condição termodinâmica:
                whichCond: if((dCalculatedBeta.EQ.(0.0d0)).OR.((iIER_BubbleT.EQ.ERROR_EverythingOK).AND.(dFlashTemperature.EQ.dCalculatedBubbleT))) then
                    ! Líquido saturado!
                    iCalculatedThermodynamicCondition = THERMOCOND_SaturatedLiquid

                else if((dCalculatedBeta.EQ.(1.0d0)).OR.((iIER_DewT.EQ.ERROR_EverythingOK).AND.(dFlashTemperature.EQ.dCalculatedDewT))) then whichCond
                    ! Vapor saturado!
                    iCalculatedThermodynamicCondition = THERMOCOND_SaturatedVapor

                else if((dCalculatedBeta.GT.(0.0d0)).AND.(dCalculatedBeta.LT.(1.0d0))) then whichCond
                    ! Equilíbrio Líquido-Vapor!
                    iCalculatedThermodynamicCondition = THERMOCOND_LiquidVaporVLE

                else if(bPStepwiseDetectedSubcooledLiq) then whichCond
                    ! Líquido subresfriado!
                    iCalculatedThermodynamicCondition = THERMOCOND_SubcooledLiquid
                    dCalculatedBeta = 0.0d0
                    oCalculatedLiqComposition = oZ

                ! =======================================
                !   INSERÇÃO EM 26/10/2021
                else if(bSuspectedDryGas) then whichCond
                    ! Dry gas!
                    iCalculatedThermodynamicCondition = THERMOCOND_SuperheatedVapor
                    dCalculatedBeta = 1.0d0
                    oCalculatedVapComposition = oZ
                !   FIM DA INSERÇÃO EM 26/10/2021
                ! =======================================

                else
                    ! Duas possibilidades: ou é uma das outras duas condições termodinâmicas (dá pra testar calculando a função objetivo e
                    ! analisando o sinal) ou é erro! Por enquanto, tratar como erro... se os cálculos de T Bolha e T Orvalho, começarem a dar
                    ! muito problema, incluir aqui o teste com as funções objetivo.
                    iIER = ERROR_CouldNotDetermineThermodynamicCondition
                    return

                end if whichCond

            else calcFlashOrNot

                ! Não há necessidade de calcular o "flash".
                ! Preparar os resultados:
                iIER_Flash = ERROR_EverythingOK

                if(bIsSubcooledLiquid) then

                    ! Líquido subresfriado:
                    iCalculatedThermodynamicCondition = THERMOCOND_SubcooledLiquid
                    dCalculatedBeta = 0.0d0
                    oCalculatedLiqComposition = oZ

                else if(bIsSuperheatedVapor) then

                    ! Vapor superaquecido:
                    iCalculatedThermodynamicCondition = THERMOCOND_SuperheatedVapor
                    dCalculatedBeta = 1.0d0
                    oCalculatedVapComposition = oZ

                else

                    ! Erro!
                    iIER = ERROR_CouldNotDetermineThermodynamicCondition
                    return

                end if

            end if calcFlashOrNot

    end subroutine CalculateMixtureThermodynamicCondition


    ! =============================================================
    ! =============================================================

    ! A subrotina abaixo implementa um algoritmo por demais ineficiente para cálculo de pontos de bolha e orvalho.
    ! A experiência mostrou que seu uso pode inviabilizar simulações com cálculo composicional, e por isso é contra-indicado.

    subroutine CalcPressureStepwiseBubbleAndDewVariables(dStepOnP, dFlashPressure, dFlashTemperature, iNComp, &
                                                         oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                                                         oPeneloux, bCalculateBubbleT, bCalculateDewT, bCalculateBubbleP, iIER_BubbleT, &
                                                         dCalculatedBubbleT, iIER_DewT, dCalculatedDewT, iIER_BubbleP, dCalculatedBubbleP, iIER)

        ! OBJETIVO: Calcular pressões e temperaturas de bolha e orvalho, incrementando sucessivamente a pressão para
        !   obter bons chutes iniciais para os próximos cálculos, até se chegar na pressão desejada.
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
        logical, intent(in) :: bCalculateBubbleT                             ! "True" para calcular nesta rotina a temperatura de bolha.
        logical, intent(in) :: bCalculateDewT                                ! "True" para calcular nesta rotina a temperatura de orvalho.
        logical, intent(in) :: bCalculateBubbleP                             ! "True" para calcular nesta rotina a pressão de bolha.

        integer(c_int), intent(out) :: iIER_BubbleT
        real(c_double), intent(out) :: dCalculatedBubbleT                    ! Temperatura de bolha calculada.
        integer(c_int), intent(out) :: iIER_DewT
        real(c_double), intent(out) :: dCalculatedDewT                       ! Temperatura de orvalho calculada.
        integer(c_int), intent(out) :: iIER_BubbleP
        real(c_double), intent(out) :: dCalculatedBubbleP                    ! Pressão de bolha calculada.
        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.

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
        integer :: iIER_TrialFlash
        real(c_double) :: dIntermediaryBubbleT, dIntermediaryDewT
        logical :: bGotBasePressureForBubbleT, bGotBasePressureForDewT, bGotBasePressureForBubbleP
        real(c_double) :: dBasePressureForBubbleT, dCurrentBubbleT
        real(c_double) :: dBasePressureForDewT, dCurrentDewT
        real(c_double) :: dIntermediaryBeta
        real(c_double), dimension(iNComp) :: oIntermediaryLiqComposition, oIntermediaryVapComposition
        real(c_double) :: dBasePressureForBubbleP
        logical :: bFoundAllBasePressures
        real(c_double) :: dCurrentBubbleTStepValue, dTrialBubbleTPressure
        logical :: bDoneWithBubbleT, bDoneWithDewT, bDoneWithBubbleP, bDoneWithAllCalculations
        logical :: bHasBubbleTVapCompositionGuess
        real(c_double), dimension(iNComp) :: oBubbleTVapCompositionGuess, oDewTLiqCompositionGuess
        real(c_double) :: dCurrentDewTStepValue, dTrialDewTPressure
        logical :: bHasDewTLiqCompositionGuess
        integer :: iIterationCount
        real(c_double) :: dCurrentBubblePStepValue, dTrialBubblePPressure
        real(c_double) :: dBubblePCurrentBeta
        real(c_double), dimension(iNComp) :: oBubblePLiqComposition, oBubblePVapComposition
        real(c_double) :: dCalculatedObjectiveFunction
        integer :: iIER_BubblePObjFunction
        real(c_double), dimension(iNComp) :: oInitialBubblePVapComposition
        logical :: bBubblePObjFunctionConverged

        ! ------------------ CONSTANTES NECESSÁRIAS:
        real(c_double), parameter :: dMinimumTrialP = 101324.99d0

        ! ------------------ CÁLCULOS:

        ! PRIMEIRO PASSO: Verificar / ajustar o passo na pressão:
        dCurrentStepValue = dStepOnP
        if(dStepOnP.LT.(0.0d0)) dCurrentStepValue = 0.05d0 * dFlashPressure

        ! Inicializações necessárias:
        bGotBasePressureForBubbleT = .false.
        bGotBasePressureForDewT = .false.
        bGotBasePressureForBubbleP = .false.
        bFoundAllBasePressures = .false.
        dTrialPressure = dFlashPressure

        ! SEGUNDO PASSO: Reduzir sucessivamente a pressão (mantendo na temperatura desejada) até se chegar nos pontos de partida
        !   necessários:
        reduceP: do

            ! Reduzir a pressão em mais um passo:
            dTrialPressure = dTrialPressure - dCurrentStepValue

            ! Garantir que a pressão não caia abaixo de determinado nível mínimo:
            if(dTrialPressure.LT.dMinimumTrialP) exit reduceP

            ! Buscar, neste nível de pressão, estimativas iniciais para os cálculos de interesse:
            call GenerateVLEExtremaInitialEstimates(dTrialPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, oPeneloux, &
                            bGotEstimateForBubblePoint, dClosestToBubbleT, oVapCompositionClosestToBubbleT, bGotEstimateForDewPoint, dClosestToDewT, &
                            oLiqCompositionClosestToDewT, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition)

            ! Caso se deseje calcular a temperatura de bolha...
            ifCalcBubbleT: if(bCalculateBubbleT.AND.(.NOT.bGotBasePressureForBubbleT)) then

                ! ... verificar se a temperatura de bolha converge neste nível de pressão...
                iIER_BubbleT = ERROR_EverythingOK
                if(.NOT.bGotEstimateForBubblePoint) dClosestToBubbleT = -100.0d0

                call CalculateBubbleOrDewTemperature(dTrialPressure, dClosestToBubbleT, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, &
                    bGotEstimateForBubblePoint, oVapCompositionClosestToBubbleT, dIntermediaryBubbleT, iIER_BubbleT)

                ! ... e caso convirja...
                if(iIER_BubbleT.EQ.ERROR_EverythingOK) then
                    ! ... armazenar o ponto de partida para o cálculo "stepwise" da temperatura de bolha.
                    bGotBasePressureForBubbleT = .true.
                    dBasePressureForBubbleT = dTrialPressure
                    dCurrentBubbleT = dIntermediaryBubbleT

                    bHasBubbleTVapCompositionGuess = bGotEstimateForBubblePoint
                    oBubbleTVapCompositionGuess = oVapCompositionClosestToBubbleT
                end if

            end if ifCalcBubbleT

            ! Caso se deseje calcular a temperatura de orvalho...
            ifCalcDewT: if(bCalculateDewT.AND.(.NOT.bGotBasePressureForDewT)) then

                ! ... verificar se a temperatura de orvalho converge neste nível de pressão...
                iIER_DewT = ERROR_EverythingOK
                if(.NOT.bGotEstimateForDewPoint) dClosestToDewT = -100.0d0

                call CalculateBubbleOrDewTemperature(dTrialPressure, dClosestToDewT, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_DewPoint, &
                    bGotEstimateForDewPoint, oLiqCompositionClosestToDewT, dIntermediaryDewT, iIER_DewT)

                ! ... e caso convirja...
                if(iIER_DewT.EQ.ERROR_EverythingOK) then
                    ! ... armazenar o ponto de partida para o cálculo "stepwise" da temperatura de orvalho...
                    bGotBasePressureForDewT = .true.
                    dBasePressureForDewT = dTrialPressure
                    dCurrentDewT = dIntermediaryDewT

                    bHasDewTLiqCompositionGuess = bGotEstimateForDewPoint
                    oDewTLiqCompositionGuess = oLiqCompositionClosestToDewT
                end if

            end if ifCalcDewT

            ! Caso se deseje calcular a pressão de bolha...
            ifCalcBubbleP: if(bCalculateBubbleP.AND.(.NOT.bGotBasePressureForBubbleP)) then

                ! ... tentar calcular o "flash" neste nível de pressão (e na temperatura desejada)...
                iIER_TrialFlash = ERROR_EverythingOK

                call CalcFlashPTVLE(dTrialPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition, dIntermediaryBeta, &
                        oIntermediaryLiqComposition, oIntermediaryVapComposition, iIER_TrialFlash)

                ! ... e caso convirja para condições bifásicas...
                if(iIER_TrialFlash.EQ.ERROR_EverythingOK) then
                    if((dIntermediaryBeta.GT.(0.0d0)).AND.(dIntermediaryBeta.LT.(1.0d0)).AND.(.NOT.ISNAN(dIntermediaryBeta))) then
                        ! ... armazenar o ponto de partida para o cálculo "stepwise" da pressão de bolha...
                        bGotBasePressureForBubbleP = .true.
                        dBasePressureForBubbleP = dTrialPressure

                        dBubblePCurrentBeta = dIntermediaryBeta
                        oBubblePLiqComposition = oIntermediaryLiqComposition
                        oBubblePVapComposition = oIntermediaryVapComposition

                        oInitialBubblePVapComposition = oInitialVapComposition
                    end if
                end if

            end if ifCalcBubbleP

            ! Verificar se já é possível terminar este "loop":
            bFoundAllBasePressures = (bCalculateBubbleT.AND.bGotBasePressureForBubbleT).OR.(.NOT.bCalculateBubbleT)
            bFoundAllBasePressures = ((bCalculateDewT.AND.bGotBasePressureForDewT).OR.(.NOT.bCalculateDewT)).AND.bFoundAllBasePressures
            bFoundAllBasePressures = ((bCalculateBubbleP.AND.bGotBasePressureForBubbleP).OR.(.NOT.bCalculateBubbleP)).AND.bFoundAllBasePressures
            if(bFoundAllBasePressures) exit reduceP

        end do reduceP

        ! Foi terminado o procedimento de recuar na pressão.
        ! Verificar o motivo e proceder de acordo:
        if(.NOT.bFoundAllBasePressures) then

            iIER = ERROR_BasePressureNotFoundInPStepwiseCalc

            if(bCalculateBubbleT) iIER_BubbleT = iIER
            if(bCalculateDewT) iIER_DewT = iIER
            if(bCalculateBubbleP) iIER_BubbleP = iIER

            return

        end if

        ! Avançar com a pressão em um ritmo mais lento que o do recuo...
        dCurrentStepValue = dCurrentStepValue / 100.0d0

        dCurrentBubbleTStepValue = dCurrentStepValue
        dTrialBubbleTPressure = dBasePressureForBubbleT

        dCurrentDewTStepValue = dCurrentStepValue
        dTrialDewTPressure = dBasePressureForDewT

        dCurrentBubblePStepValue = dCurrentStepValue
        dTrialBubblePPressure = dBasePressureForBubbleP

        ! TERCEIRO PASSO: De posse de resultados convergidos em pressões mais baixas, avançar sucessivamente com a pressão,
        !   usando os resultados de um nível como chutes iniciais para o próximo:
        iIterationCount = 0
        bDoneWithBubbleT = (.NOT.bCalculateBubbleT)
        bDoneWithDewT = (.NOT.bCalculateDewT)
        bDoneWithBubbleP = (.NOT.bCalculateBubbleP)

        advanceP: do

            ! Controle de execução do "loop":
            bDoneWithAllCalculations = (bDoneWithBubbleT.AND.bDoneWithDewT.AND.bDoneWithBubbleP)
            if(bDoneWithAllCalculations) exit advanceP

            iIterationCount = iIterationCount + 1

            ! Caso ainda sejam necessários cálculos da temperatura de bolha...
            advanceOnBubbleT: if(.NOT.bDoneWithBubbleT) then

                ! ... avançar na pressão para a temperatura de bolha...
                dTrialBubbleTPressure = dTrialBubbleTPressure + dCurrentBubbleTStepValue

                ! ... garantir que não se está ultrapassando a pressão "alvo"...
                if(dTrialBubbleTPressure.GE.dFlashPressure) then
                    dTrialBubbleTPressure = dFlashPressure
                    bDoneWithBubbleT = .TRUE.
                end if

                ! Tentar calcular a temperatura de bolha neste novo nível de pressão, usando como chute inicial
                ! a temperatura de bolha convergida no nível de pressão mais baixo:
                iIER_BubbleT = ERROR_EverythingOK

                call CalculateBubbleOrDewTemperature(dTrialBubbleTPressure, dCurrentBubbleT, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, &
                    bHasBubbleTVapCompositionGuess, oBubbleTVapCompositionGuess, dIntermediaryBubbleT, iIER_BubbleT)

               ! Caso o cálculo neste nível de pressão tenha convergido...
               if(iIER_BubbleT.EQ.ERROR_EverythingOK) then
                    ! ... atualizar o chute inicial do próximo nível de pressão.
                    dCurrentBubbleT = dIntermediaryBubbleT
               end if

               ! Caso esta iteração esteja concluindo este cálculo...
               if(bDoneWithBubbleT) then
                    ! ... armazenar os devidos resultados de saída.
                    dCalculatedBubbleT = dIntermediaryBubbleT
               end if

            end if advanceOnBubbleT

            ! Caso ainda sejam necessários cálculos da temperatura de orvalho...
            advanceOnDewT: if(.NOT.bDoneWithDewT) then

                ! ... avançar na pressão para a temperatura de orvalho...
                dTrialDewTPressure = dTrialDewTPressure + dCurrentDewTStepValue

                ! ... garantir que não se está ultrapassando a pressão "alvo"...
                if(dTrialDewTPressure.GE.dFlashPressure) then
                    dTrialDewTPressure = dFlashPressure
                    bDoneWithDewT = .TRUE.
                end if

                ! Tentar calcular a temperatura de orvalho neste novo nível de pressão, usando como chute inicial
                ! a temperatura de orvalho convergida em um nível de pressão mais baixo:
                iIER_DewT = ERROR_EverythingOK

                call CalculateBubbleOrDewTemperature(dTrialDewTPressure, dCurrentDewT, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_DewPoint, &
                    bHasDewTLiqCompositionGuess, oDewTLiqCompositionGuess, dIntermediaryDewT, iIER_DewT)

                ! Caso o cálculo neste nível de pressão tenha convergido...
                if(iIER_DewT.EQ.ERROR_EverythingOK) then
                    ! ... atualizar o chute inicial do próximo nível de pressão.
                    dCurrentDewT = dIntermediaryDewT
                end if

                ! Caso esta iteração esteja concluindo este cálculo...
                if(bDoneWithDewT) then
                    ! ... armazenar os devidos resultados de saída.
                    dCalculatedDewT = dIntermediaryDewT
                end if

            end if advanceOnDewT

            ! Caso ainda sejam necessários cálculos da pressão de bolha...
            advanceOnBubbleP: if(.NOT.bDoneWithBubbleP) then

                ! ... verificar a necessidade de ajustes no passo...
                if(dBubblePCurrentBeta.LT.(0.1d0)) dCurrentBubblePStepValue = 250.0d0

                ! ... avançar com a pressão...
                dTrialBubblePPressure = dTrialBubblePPressure + dCurrentBubblePStepValue

                ! ... tentar calcular o "flash" neste novo nível de pressão, usando como chutes iniciais
                ! os resultados do nível de pressão anterior:
                iIER_TrialFlash = ERROR_EverythingOK

                ! ---------------------------------------------------
                ! ---------------------------------------------------
                ! INCLUSÃO DA FUNÇÃO OBJETIVO

                bBubblePObjFunctionConverged = .FALSE.
                if(.true.) then

                    iIER_BubblePObjFunction = ERROR_EverythingOK

                    call CalculateBubbleOrDewPointObjectiveFunction(dTrialBubblePPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, &
                        oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, .TRUE., oInitialBubblePVapComposition, &
                        dCalculatedObjectiveFunction, iIER_BubblePObjFunction)

                    bBubblePObjFunctionConverged = ((iIER_BubblePObjFunction.EQ.ERROR_EverythingOK).AND. &
                                                    (abs(dCalculatedObjectiveFunction).LE.(5.0D-7)))

                        ! NOTAS:
                        ! i) o valor de 5D-7 foi verificado por tentativas numéricas como sendo bom indicativo de convergência.
                        ! ii) a implementação atual, usando função objetivo, não reduz o passo na pressão e por isso tem o perigo
                        !       de estar usando passo largo demais a ponto de haver a troca de sinal fora da zona de 5D-7.
                        !       Por enquanto, não está acontecendo.
                        ! iii) quase certeza que a atual abordagem é muito ineficiente. Por enquanto, a prioridade está em garantir
                        !      que o valor da pressão de bolha será encontrado. Mais análises orientarão o ajuste deste código com
                        !      vistas ao bom desempenho das simulações.
                end if

                ! FIM DA INCLUSÃO DA FUNÇÃO OBJETIVO
                ! ---------------------------------------------------
                ! ---------------------------------------------------

                call CalcFlashPTVLE(dTrialBubblePPressure, dFlashTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                        oPeneloux, .TRUE., dBubblePCurrentBeta, oBubblePLiqComposition, oBubblePVapComposition, dIntermediaryBeta, &
                        oIntermediaryLiqComposition, oIntermediaryVapComposition, iIER_TrialFlash)

                ! Caso o cálculo do "flash" atinja convergência...
                if(iIER_TrialFlash.EQ.ERROR_EverythingOK) then
                    ! ... atualizar as estimativas iniciais do próximo nível de pressão:
                    dBubblePCurrentBeta = dIntermediaryBeta
                    oBubblePLiqComposition = oIntermediaryLiqComposition
                    oBubblePVapComposition = oIntermediaryVapComposition
                end if

                ! ... verificar se a pressão atual já pode ser considerada a pressão de bolha:
                if((dBubblePCurrentBeta.LT.(0.5d0 / 100.0d0)) .OR. &
                        bBubblePObjFunctionConverged) then
                    ! Pressão de bolha!
                    dCalculatedBubbleP = dTrialBubblePPressure
                    bDoneWithBubbleP = .TRUE.
                else if(iIterationCount.GT.8000) then
                    ! Atenção com "loop" infinito:
                    iIER_BubbleP = ERROR_CouldNotPStepwiselyFindBubbleP
                    bDoneWithBubbleP = .TRUE.
                end if

            end if advanceOnBubbleP

        end do advanceP

    end subroutine CalcPressureStepwiseBubbleAndDewVariables

    ! =============================================================
    ! =============================================================

    ! O cálculo de temperaturas de bolha e orvalho não vem sendo mais usado na biblioteca. A experiência mostrou que buscar
    ! pressões de bolha ou orvalho oferece melhores resultados.

    subroutine CalculateBubbleOrDewTemperature(dFlashPressure, dTInitial, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, &
                    oOtherPhaseCompositionEstimate, dCalculatedTemperature, iIER)

        ! OBJETIVO: Calcular a temperatura de bolha ou orvalho de determinada mistura especificada.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), intent(inout) :: dTInitial                    ! Estimativa inicial da temperatura desejada (dar negativo para a rotina gerar a sua própria).
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

        real(c_double), intent(out) :: dCalculatedTemperature                ! Valor calculado e retornado da temperatura desejada (bolha ou orvalho).
        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i                                                                                    ! Índice de laços "do".
        real(c_double) :: dTOther, dTTent
        real(c_double) :: dInitialObjectiveFunction, dOtherObjectiveFunction, dTentObjectiveFunction    ! Valor calculado da função objetivo desejada.
        real(c_double) :: dSign
        logical :: bIsBubbleCalculation, bIsDewCalculation
        logical :: bKeepSearching
        integer :: iIterationCount                                                  ! Contagem de iterações.
        logical :: bConverged                                                       ! "True" para convergência, "false" caso contrário

        ! ------------------ CONSTANTES NECESSÁRIAS:
        real(c_double), parameter :: dSearchDelta = 5.0d0                   ! Passo na busca pelo segundo ponto para o Método da Bisseção.
        integer, parameter :: iMaxIterations = 500                           ! Máximo número permitido de iterações.

        ! ------------------ CÁLCULOS:

        ! Bolha? Orvalho?
        bIsBubbleCalculation = (iCalculationFlag.EQ.CALCTYPE_BubblePoint)
        bIsDewCalculation = (iCalculationFlag.EQ.CALCTYPE_DewPoint)

        ! Estimar o ponto de partida, razoavelmente afastado do ponto crítico:
        generateInitialT: if(dTInitial.LT.(0.0d0)) then

            dTInitial = 0.0d0
            do i = 1, iNComp
                dTInitial = dTInitial + oZ(i) * oTc(i) * 0.7d0
            end do

        end if generateInitialT

        ! Calcular aí a função objetivo:
        call CalculateBubbleOrDewPointObjectiveFunction(dFlashPressure, dTInitial, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                    dInitialObjectiveFunction, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Fazer os ajustes para a busca de outro ponto:
        dSign = abs(dInitialObjectiveFunction) / dInitialObjectiveFunction
        if(bIsBubbleCalculation) dSign = -1.0d0 * dSign

        dOtherObjectiveFunction = dInitialObjectiveFunction
        dTOther = dTInitial

        iIterationCount = 0

        searchForOtherT: do

            ! Contagem de iterações:
            iIterationCount = iIterationCount + 1

            ! Controle de execução:
            bKeepSearching = ((dInitialObjectiveFunction / dOtherObjectiveFunction).GT.(0.0d0))
            bKeepSearching = (bKeepSearching.AND.(iIterationCount.LE.iMaxIterations))
            if(.NOT.bKeepSearching) exit searchForOtherT

            ! Avançar com a inicial:
            dTInitial = dTOther
            dInitialObjectiveFunction = dOtherObjectiveFunction

            ! Avançar com a "outra":
            dTOther = dTOther + dSign * dSearchDelta

            ! Calcular sua função:
            call CalculateBubbleOrDewPointObjectiveFunction(dFlashPressure, dTOther, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                    dOtherObjectiveFunction, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

        end do searchForOtherT

        ! Assegurar que foi encontrado o outro ponto para o Método da Bisseção:
        foundOtherT: if((dInitialObjectiveFunction / dOtherObjectiveFunction).GT.(0.0d0)) then

            ! Não foi possível delimitar a raíz! Sinalizar o erro!
            if(bIsBubbleCalculation) then
                iIER = ERROR_NumericalProblemInBubblePointCalculation
            else if(bIsDewCalculation) then
                iIER = ERROR_NumericalProblemInDewPointCalculation
            end if

            ! Retornar
            return

        end if foundOtherT

        ! Terminada a busca, já temos temperaturas em ambos os lados da raíz!
        ! Aplicar o Método da Bisseção:
        iIterationCount = 0
        bConverged = .FALSE.

        bissecLoop: do

            ! Contagem de iterações:
            iIterationCount = iIterationCount + 1

            ! Calcular tentativa:
            dTTent = (dTInitial + dTOther) / 2.0d0

            ! Calcular a função na tentativa:
            call CalculateBubbleOrDewPointObjectiveFunction(dFlashPressure, dTTent, iNComp, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iCalculationFlag, bHasOtherPhaseCompositionEstimate, oOtherPhaseCompositionEstimate, &
                    dTentObjectiveFunction, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Atualizar os extremos:
            if((dInitialObjectiveFunction*dTentObjectiveFunction).LT.(0.0d0)) then
                dOtherObjectiveFunction = dTentObjectiveFunction
                dTOther = dTTent
            else
                dInitialObjectiveFunction = dTentObjectiveFunction
                dTInitial = dTTent
            end if

            ! Controle de execução:
            bConverged = (abs(dTOther - dTInitial).LE.(0.0005d0))
            if(bConverged.OR.(iIterationCount.GE.iMaxIterations)) exit bissecLoop

        end do bissecLoop

        ! Caso se tenha atingido convergência, retornar o resultado.
        ! Caso contrário, sinalizar devidamente o erro:
        finalCheck: if(bConverged) then

            ! Convergiu! Retornar o resultado encontrado:
            dCalculatedTemperature = dTTent

        else finalCheck

            ! Erro!
            if(bIsBubbleCalculation) then
                iIER = ERROR_NumericalProblemInBubblePointCalculation
            else if(bIsDewCalculation) then
                iIER = ERROR_NumericalProblemInDewPointCalculation
            end if

            ! Retornar
            return

        end if finalCheck

    end subroutine CalculateBubbleOrDewTemperature

    ! =============================================================
    ! =============================================================
    subroutine PerformCubicEOSPhiPhiFlashNumericalAdaptations(iPhaseID, iCubicEOSModel, dCapitalA, dCapitalB, dZMix, iIER)

        ! OBJETIVO: Realizar as verificações e adaptações numéricas necessárias (segundo a referência) em meio a
        !   iterações de um cálculo de "flash" baseado em Equações Cúbicas de Estado.
        ! REFERÊNCIA: "Numerical Aspects of the Implementation of Cubic Equations of State in Flash Calculation Routines",
        !               Truls Gundersen, Computers and Chemical Engineering, vol 6, pp 245-255, 1982.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iPhaseID                        ! Índice da fase sendo calculada, conforme convenção.
        integer(c_int), value, intent(in) :: iCubicEOSModel                  ! Índice representando a equação de estado cúbica, conforme convenção.
        real(c_double), intent(in) :: dCapitalA                              ! Parâmetro A e B maiúsculos (adimensionais), conforme definições em
        real(c_double), intent(inout) :: dCapitalB                              !  "The Properties of Liquids and Gases - Fourth Edition".

        real(c_double), intent(inout) :: dZMix                               ! Fator de compressibilidade a adotar.
        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dU, dW                                             ! Parâmetros "u" e "w" da forma geral da equação de estado cúbica.
        real(c_double) :: dCoefA1, dCoefA2, dCoefA3                          ! Coeficientes da equação cúbica em "Z"
        real(c_double) :: dCubicQ, dCubicR                                   ! Variáveis intermediárias na solução da equação cúbica.
        real(c_double) :: dCubicD                                            ! "Discriminante" calculado na solução da equação cúbica.
        logical :: bComplexRootsWereFound
        real(c_double) :: dZExtrema1, dZExtrema2
        real(c_double) :: dS1, dS2
        real(c_double) :: dZSingleRealOrPhysicallyMeaningless                ! Única raíz real encontrada
        real(c_double) :: dSqrtOfCubicD
        real(c_double) :: dZ1, dZ2, dZ3, dTemp

        ! ------------------ CONSTANTES:
        logical, parameter :: bPerformReferenceAdaptation = .false.         ! "False" para não fazer no procedimento padrão as adaptações numéricas.
                                                                            ! Permite "desligar" esta adaptação a qualquer momento.

        ! ------------------ CÁLCULOS:

        ! ANTES DE MAIS NADA: Desconsiderar a adaptação numérica recomendada pelo artigo-referência, se for o caso:
        if(.NOT.bPerformReferenceAdaptation) return

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

        ! O artigo da referência expressamente recomenda o procedimento de usar os extremos apenas quando
        ! a equação só possui uma raíz...
        if(.NOT.bComplexRootsWereFound) return

        ! É necessário verificar se será possível calcular extremos reais...
        if((3.0d0 * dCoefA2).GT.(dCoefA1 * dCoefA1)) return

        ! Calcular os dois fatores de compressibilidade correspondentes aos extremos:
        dZExtrema1 = (-1.0d0 * dCoefA1 + sqrt(dCoefA1 * dCoefA1 - (3.0d0 * dCoefA2))) / 3.0d0
        dZExtrema2 = (-1.0d0 * dCoefA1 - sqrt(dCoefA1 * dCoefA1 - (3.0d0 * dCoefA2))) / 3.0d0

        ! Determinar a única raíz real:
        dSqrtOfCubicD = sqrt(dCubicD)
        dS1 = (dCubicR + dSqrtOfCubicD) ** (1.0d0 / 3.0d0)
        dS2 = (dCubicR - dSqrtOfCubicD) ** (1.0d0 / 3.0d0)

                ! Tratamento de aparente "bug" do Fortran:
                if((dCubicR - dSqrtOfCubicD).LT.(0.0d0)) then
                    dS2 = dCubicR - dSqrtOfCubicD
                    dS2 = -1.0d0 * dS2
                    dS2 = dS2 ** (1.0d0 / 3.0d0)
                    dS2 = -1.0d0 * dS2
                end if

        dZSingleRealOrPhysicallyMeaningless = dS1 + dS2 - dCoefA1 / 3.0d0

        ! Ordenar as três "raízes":
        dZ1 = dZExtrema1
        dZ2 = dZExtrema2
        dZ3 = dZSingleRealOrPhysicallyMeaningless

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

        ! De acordo com a fase em questão, retornar o fator de compressibilidade a tentar:
        if(iPhaseID.EQ.PHASE_Liquid) then
            dZMix = dZ1
        else if(iPhaseID.EQ.PHASE_Vapor) then
            dZMix = dZ3
        end if

        ! --------------------------
        ! TESTE - 22/10/2019

        ! Será que ajuda o líquido a evitar raízes negativas? Ou "ln" negativo?
        !if(iPhaseID.EQ.PHASE_Liquid) then
        !
        !    if(dZMix.LT.(0.0d0)) then
        !        dZMix = dZSingleRealOrPhysicallyMeaningless
        !        return
        !    end if
        !
        !end if

            ! NOTA: O teste acima NÃO SE MOSTROU EFICAZ no sentido de "driblar" Z's negativos...


        ! Alterar o valor de "B" conforme sugerido no artigo-referência:
        !dTemp = dZMix * dZMix * dZMix + dCoefA1 * dZMix * dZMix + dCoefA2 * dZMix + dCoefA3
        !dCapitalB = dCapitalB * (1.0d0 + dTemp / (dCapitalA * dCapitalB))

            ! NOTA: Esta última alteração está comentada por via das dúvidas, porque seu efeito ainda não foi analizado
            !       e porque o artigo só a realizou para a SRK (mas a premissa também parece válida para Peng-Robinson).
            !       De qualquer forma, merece uma leve conferida antes de se descomentar de vez.

            ! NOTA FINAL: O método por agora está desligado, porque sua eficácia ainda precisa ser mais bem testada.
            ! Pode não ser o caso de usar em qualquer nível de pressão... o artigo diz que curvas com esse máximo
            ! e mínimo "occur only at low and moderate pressure levels, and result basically from poor starting values
            ! of phase compositions". APESAR QUE, esta rotina testa a existência desses extremos para qualquer pressão...

        ! FIM DO TESTE - 22/10/2019
        ! -------------------------

    end subroutine PerformCubicEOSPhiPhiFlashNumericalAdaptations

    ! =============================================================
    ! =============================================================
    subroutine CalculateSingleComponentZAndRhoFromCubicEOS(dTc, dPc, dW, dC, dMW, iPhaseID, dFlashPressure, dFlashTemperature, iCubicEOSModel, dZ, dDensity, iIER)

        ! OBJETIVO:
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTc                                    ! Temperatura crítica do componente em questão.
        real(c_double), value, intent(in) :: dPc                                    ! Pressão crítica do componente em questão.
        real(c_double), value, intent(in) :: dW                                     ! Fator acêntrico do componente em questão
        real(c_double), value, intent(in) :: dC                         ! "Volume translation parameter" do componente (como volume molar) a aplicar (usar ZERO para não corrigir).
        real(c_double), value, intent(in) :: dMW                        ! Massa molar do componente (g/gmol)
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujos parâmetros se deseja calcular, conforme convenção.
        real(c_double), value, intent(in) :: dFlashPressure                         ! Pressão na qual deseja-se realizar os cálculos.
        real(c_double), value, intent(in) :: dFlashTemperature                      ! Temperatura na qual deseja-se realizar os cálculos.
        integer(c_int), value, intent(in) :: iCubicEOSModel                         ! Índice da EOS Cúbica, conforme convenção.

        real(c_double), intent(out) :: dZ                                           ! Fator de compressibilidade do componente em questão.
        real(c_double), intent(out) :: dDensity                                     ! Massa específica do componente em questão.
        integer(c_int), intent(out) :: iIER                                         ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(1) :: oTc                             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(1) :: oPc                             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(1) :: oW                              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(1) :: oCubicEOSaParameters       ! Vetor de parâmetros "a" calculados (indexado por componente)
        real(c_double), dimension(1) :: oCubicEOSbParameters       ! Vetor de parâmetros "b" calculados (indexado por componente)
        real(c_double) :: dZLiquid              ! Fator de compressibilidade calculado para a fase líquida.
        real(c_double) :: dZVapor               ! Fator de compressibilidade calculado para a fase vapor.
        logical :: bComplexRootsWereFound                       ! "True" se foram encontrados fatores de compressibilidade complexos.


        ! ------------------ CÁLCULOS:

        ! ETAPA PRELIMINAR: Preparar argumentos:
        oTc(1) = dTc
        oPc(1) = dPc
        oW(1) = dW

        ! 1o PASSO: Calcular os parâmetros "a" e "b" da equação de estado cúbica em questão:
        call CalculateCubicEOSParametersForEachComponent(1, dFlashTemperature, iCubicEOSModel, oTc, oPc, oW, &
                                                   iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        if(iIER.NE.ERROR_EverythingOK) return

        ! 2o PASSO: Calcular o fator de compressibilidade a "T" e "P":
        call CalculateZFromCubicEOS(dFlashPressure, dFlashTemperature, iCubicEOSModel, oCubicEOSaParameters(1), oCubicEOSbParameters(1), &
                                         dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! 3o PASSO: Separar o fator de compressibilidade adequado:
        whichZ: if(iPhaseID.EQ.PHASE_Liquid) then
            dZ = dZLiquid
        else if(iPhaseID.EQ.PHASE_Vapor) then whichZ
            dZ = dZVapor
        end if whichZ

        ! 4o PASSO: Calcular a massa específica do componente a T e P:
        call CalculatePhaseDensityFromZAndC(dZ, dFlashTemperature, dFlashPressure, dC, dMW, dDensity)

        ! PAREI AQUI!! CONFERIR ESTA ROTINA!!! - 1/12/2020
        !              CONFERIR A OUTRA TB e o *.h!! ESCREVER OS OBJETIVOS!! E NA OUTRA, MUDAR O NOME (SÓ O NOME) DA VARIÁVEL MODELO...

    end subroutine CalculateSingleComponentZAndRhoFromCubicEOS

! ===================================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM 2 PARA CALCULAR "Z" ou "RHO" DE UM ÚNICO COMPONENTE
! ===================================================================================================================
subroutine Marlim_CalculateSingleComponentZAndRho(dTc, dPc, dW, dC, dMW, iPhaseID, dFlashPressure, &
                    dFlashTemperature, iCubicEOSModel, dZ, dDensity, iIER) bind(C, name = "Marlim_CalculateSingleComponentZAndRho")

    ! OBJETIVO:
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dTc                                    ! Temperatura crítica do componente em questão.
    real(c_double), value, intent(in) :: dPc                                    ! Pressão crítica do componente em questão.
    real(c_double), value, intent(in) :: dW                                     ! Fator acêntrico do componente em questão
    real(c_double), value, intent(in) :: dC                         ! "Volume translation parameter" do componente (como volume molar) a aplicar (usar ZERO para não corrigir).
    real(c_double), value, intent(in) :: dMW                        ! Massa molar do componente (g/gmol)
    integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujos parâmetros se deseja calcular, conforme convenção.
    real(c_double), value, intent(in) :: dFlashPressure                         ! Pressão na qual deseja-se realizar os cálculos.
    real(c_double), value, intent(in) :: dFlashTemperature                      ! Temperatura na qual deseja-se realizar os cálculos.
    integer(c_int), value, intent(in) :: iCubicEOSModel                         ! Índice da EOS Cúbica, conforme convenção.

    real(c_double), intent(out) :: dZ                                           ! Fator de compressibilidade do componente em questão.
    real(c_double), intent(out) :: dDensity                                     ! Massa específica do componente em questão.
    integer(c_int), intent(out) :: iIER                                         ! Código de erros, conforme convencionado.

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! CHAMADA PRINCIPAL:
    call CalculateSingleComponentZAndRhoFromCubicEOS(dTc, dPc, dW, dC, dMW, iPhaseID, dFlashPressure, dFlashTemperature, iCubicEOSModel, dZ, dDensity, iIER)

end subroutine Marlim_CalculateSingleComponentZAndRho

end module Obsoletes
