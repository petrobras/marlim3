module FlashExtras

    ! OBJETIVO: Conter subrotinas adicionais para melhorar os cálculos de "flash" da biblioteca.
    ! UNIDADES: TODA a programação contida neste módulo considera que TODOS os valores (entradas e saídas
    !   das rotinas e suas variáveis internas) estão no SI, EXCETO QUANDO devidamente explicitado...

    use iso_c_binding   ! Para viabilizar a comunicação com C.....
    use VLECalculations
    use Newton, only: TryFlashCalcWith2ndOrderMinimizationIfNecessary, TryStabilityAnalysisWith2ndOrderMinimization

    implicit none

    ! ---------------- INTERFACES DE ROTINAS DO Newton.f90

    ! Interfaces necessárias para eliminar dependência circular entre este arquivo e o "Newton.f90"
    ! (essa interdependência por meio de comandos "use" estava causando problemas de compilação).

    !interface

    !    subroutine TryFlashCalcWith2ndOrderMinimizationIfNecessary(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, &
    !                oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, bGibbsEnergyDecreases, bFoundValidBeta, &
    !                bTPDAnalysisSuccessfullyDone, oLeastGPhaseLnFug, iLeastGPhaseID, oLiqStationaryPointW, oVapStationaryPointW, &
    !                oImprovedKFactors, iIER, dBetaVap, oX, oY, bIgnoreOutcomeAndResume, &
    !                bFeedIsStableAsSinglePhase, oLiqStationaryPointLnFug, oVapStationaryPointLnFug)
    !        use iso_c_binding
    !        implicit none
    !        real(c_double), value, intent(in) :: dTemperature
    !        real(c_double), value, intent(in) :: dPressure
    !        integer(c_int), value, intent(in) :: iNComp
    !        real(c_double), dimension(iNComp), intent(in) :: oZ
    !        real(c_double), dimension(iNComp), intent(in) :: oTc
    !        real(c_double), dimension(iNComp), intent(in) :: oPc
    !        real(c_double), dimension(iNComp), intent(in) :: oW
    !        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij
    !        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij
    !        real(c_double), dimension(iNComp), intent(in) :: oPeneloux
    !        integer(c_int), value, intent(in) :: iLiqPhaseModel
    !        integer(c_int), value, intent(in) :: iVapPhaseModel
    !        logical, intent(in) :: bGibbsEnergyDecreases
    !        logical, intent(in) :: bFoundValidBeta
    !        logical, intent(inout) :: bTPDAnalysisSuccessfullyDone
    !        real(c_double), dimension(iNComp), intent(in) :: oLeastGPhaseLnFug
    !        integer(c_int), value, intent(in) :: iLeastGPhaseID
    !        real(c_double), dimension(iNComp), intent(inout) :: oLiqStationaryPointW
    !        real(c_double), dimension(iNComp), intent(inout) :: oVapStationaryPointW
    !        real(c_double), dimension(iNComp), intent(inout) :: oImprovedKFactors
    !        integer(c_int), intent(out) :: iIER
    !        real(c_double), intent(inout) :: dBetaVap
    !        real(c_double), dimension(iNComp), intent(out) :: oX
    !        real(c_double), dimension(iNComp), intent(inout) :: oY
    !        logical, intent(out) :: bIgnoreOutcomeAndResume
    !        logical, intent(out) :: bFeedIsStableAsSinglePhase
    !        real(c_double), dimension(iNComp), intent(out) :: oLiqStationaryPointLnFug
    !        real(c_double), dimension(iNComp), intent(out) :: oVapStationaryPointLnFug
    !    end subroutine TryFlashCalcWith2ndOrderMinimizationIfNecessary


    !    subroutine TryStabilityAnalysisWith2ndOrderMinimization(dTemperature, dPressure, iNComp, oTc, oPc, oW, oKij, oLij, oPeneloux, &
    !                iTrialPhaseID, iTrialPhaseThermodynamicModel, oCurrentW, oD, dNegativeTPDCriteriaToUse, iIER, bIgnoreOutcomeAndResume, &
    !                oStationaryPointW, oStationaryPointLnFug, dStationaryPointTPD, dLastTrialWSumBeforeNormalizationMinusUnity)
    !        use iso_c_binding
    !        implicit none
    !        real(c_double), value, intent(in) :: dTemperature
    !        real(c_double), value, intent(in) :: dPressure
    !        integer(c_int), value, intent(in) :: iNComp
    !        real(c_double), dimension(iNComp), intent(in) :: oTc
    !        real(c_double), dimension(iNComp), intent(in) :: oPc
    !        real(c_double), dimension(iNComp), intent(in) :: oW
    !        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij
    !        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij
    !        real(c_double), dimension(iNComp), intent(in) :: oPeneloux
    !        integer(c_int), value, intent(in) :: iTrialPhaseID
    !        integer(c_int), value, intent(in) :: iTrialPhaseThermodynamicModel
    !        real(c_double), dimension(iNComp), intent(in) :: oCurrentW
    !        real(c_double), dimension(iNComp), intent(in) :: oD
    !        real(c_double), intent(in) :: dNegativeTPDCriteriaToUse
    !        integer(c_int), intent(out) :: iIER
    !        logical, intent(out) :: bIgnoreOutcomeAndResume
    !        real(c_double), dimension(iNComp), intent(out) :: oStationaryPointW
    !        real(c_double), dimension(iNComp), intent(out) :: oStationaryPointLnFug
    !        real(c_double), intent(out) :: dStationaryPointTPD
    !        real(c_double), intent(out) :: dLastTrialWSumBeforeNormalizationMinusUnity
    !    end subroutine TryStabilityAnalysisWith2ndOrderMinimization

    !end interface

    ! ---------------- FIM DAS INTERFACES DE ROTINAS DO Newton.f90

    contains

    ! =============================================================
    ! =============================================================
    subroutine CalculateMixtureThermodynamicCondition_V3(dPressure, dTemperature, iNComp, oMW, &
           oZ, oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, &
           bHasInitialFlashEstimates, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
           dBetaVap, oLiqComposition, oVapComposition, iCalculatedThermodynamicCondition, iIER_Flash, iIER)

        ! OBJETIVO: Determinar a condição termodinâmica de uma mistura especificada sob dada condições de T e P.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dPressure                  ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dTemperature               ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMW                 ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor temperatura crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor pressão crítica (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor fator acêntrico (indexado por componente).
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iLiqPhaseModel              ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel              ! Modelo selecionado para a fase vapor, conforme índices convencionados.

        logical, intent(in) :: bHasInitialFlashEstimates         ! "True" caso se deseje fornecer estimativas iniciais para o cálculo de "flash".
                                                                 ! "False" para a própria rotina calcular essas estimativas.
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialLiqComposition    ! Estimativa inicial de composição da fase líquida para o "flash".
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialVapComposition    ! Estimativa inicial de composição da fase vapor para o "flash".

        real(c_double), intent(out) :: dBetaVap                       ! Fração molar vaporizada determinada.
        real(c_double), dimension(iNComp), intent(out) :: oLiqComposition    ! Vetor composição da fase líquida (indexado por componente)
        real(c_double), dimension(iNComp), intent(out) :: oVapComposition    ! Vetor composição da fase vapor (indexado por componente)
        integer(c_int), intent(out) :: iCalculatedThermodynamicCondition            ! Código da condição termodinâmica determinada, conforme convencionado.
        integer(c_int), intent(out) :: iIER_Flash                            ! Código de erros no cálculo do "flash", conforme convencionado.
        integer(c_int), intent(out) :: iIER                       ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bErrorInFlashCalculation
        logical :: bBubblePWasCalculatedOk
        logical :: bDewPWasCalculatedOk
        logical :: bGetThermoCondFromEstimates
        logical :: bFoundEstimateForThermoCond
        real(c_double) :: dMixtureMW
        integer :: i
        logical :: bUseSuperheatedVapEstimate

        ! ------------------ CONSTANTES:
        logical, parameter :: bUnblockBubbleAndDewCalculations = .false.    ! Possível bloqueio a cálculos de bolha e orvalho.
        logical, parameter :: bUnblockThermoCondEstimates = .true.

        ! ------------------ CÁLCULOS:

        ! 1 === > TENTAR CALCULAR O "FLASH":
        iIER_Flash = ERROR_EverythingOK

        call CalculateIsothermalPTVLEFlash(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                 iLiqPhaseModel, iVapPhaseModel, bHasInitialFlashEstimates, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                 iIER_Flash, dBetaVap, oLiqComposition, oVapComposition)

        ! Determinar e inicializar indicadores para a sequência:
        bErrorInFlashCalculation = (iIER_Flash.NE.ERROR_EverythingOK)

        bBubblePWasCalculatedOk = .false.
        bDewPWasCalculatedOk = .false.
        bGetThermoCondFromEstimates = .false.
        bFoundEstimateForThermoCond = .false.
        bUseSuperheatedVapEstimate = .false.

        ! 2 ===> TRATAR EVENTUAL ERRO DO "FLASH"
        treatFlashError: if(bErrorInFlashCalculation) then

            ! 2a ===> Recorrer a cálculos de bolha / orvalho?
            bubbleAndDewCalcs: if(bUnblockBubbleAndDewCalculations) then
                ! TODO: Implementar no futuro? Somente se o tempo mostrar ser necessário!
                bBubblePWasCalculatedOk = .false.
                bDewPWasCalculatedOk = .false.
            end if bubbleAndDewCalcs

            ! 2b ===> Recorrer a estimativas?
            bGetThermoCondFromEstimates = ((.not.bBubblePWasCalculatedOk).and. &
                                           (.not.bDewPWasCalculatedOk).and. &
                                           (bUnblockThermoCondEstimates))

            makeEstimatesForThermoCond: if(bGetThermoCondFromEstimates) then

                ! Inicializando...
                bFoundEstimateForThermoCond = .false.

                ! Verificar possíveis estimativas pela massa molar da mistura:
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

        ! 3 ===> DETERMINAR CONDIÇÃO TERMODINÂMICA:
        whichThermoCond: if((.not.bErrorInFlashCalculation).and.(dBetaVap.lt.(0.0005d0))) then
            ! Líquido subresfriado:
            iCalculatedThermodynamicCondition = THERMOCOND_SubcooledLiquid
        else if( &
               ((.not.bErrorInFlashCalculation).and.(dBetaVap.gt.(1.0d0-0.0005d0))) .or. &
                bUseSuperheatedVapEstimate &
               ) then whichThermoCond
            ! Vapor superaquecido:
            iCalculatedThermodynamicCondition = THERMOCOND_SuperheatedVapor
            dBetaVap = 1.0d0
            oVapComposition = oZ
        else if(.not.bErrorInFlashCalculation) then whichThermoCond
            ! Equilíbrio Líquido-Vapor:
            iCalculatedThermodynamicCondition = THERMOCOND_LiquidVaporVLE
        else whichThermoCond
            ! Condição não contemplada:
            iIER = ERROR_CouldNotDetermineThermodynamicCondition
            return
        end if whichThermoCond

    end subroutine CalculateMixtureThermodynamicCondition_V3

    ! =============================================================
    !           ROTINAS DE CÁLCULO DE FLASH
    ! =============================================================

    subroutine CalculateIsothermalPTVLEFlash(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                 iLiqPhaseModel, iVapPhaseModel, bHasInitialFlashEstimates, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                 iIER, dBetaVap, oX, oY)

        ! OBJETIVO: Fazer o cálculo eficiente de um "flash" P,T bifásico (líquido / vapor).
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature             ! Temperatura
        real(c_double), value, intent(in) :: dPressure                ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                   ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ           ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oTc          ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc          ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW           ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iLiqPhaseModel              ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel              ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        logical, intent(in) :: bHasInitialFlashEstimates         ! "True" caso se deseje fornecer estimativas iniciais para o cálculo de "flash".
                                                                 ! "False" para a própria rotina calcular essas estimativas.
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialLiqComposition    ! Estimativa inicial de composição da fase líquida para o "flash".
        real(c_double), dimension(iNComp), intent(inout) :: oGivenInitialVapComposition    ! Estimativa inicial de composição da fase vapor para o "flash".

        integer(c_int), intent(out) :: iIER                       ! Código de erros, conforme convencionado.
        real(c_double), intent(out) :: dBetaVap                   ! Fração molar calculada para a fase vapor
        real(c_double), dimension(iNComp), intent(out) :: oX      ! Vetor composição da fase líquida
        real(c_double), dimension(iNComp), intent(out) :: oY      ! Vetor composição da fase vapor

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i
        real(c_double), dimension(iNComp) :: oK, oPreviousK, oK_Calc                   ! Vetores de constantes de equilíbrio
        logical :: bBothPhasesWithSameGEnergy  ! "True" se for calculada a mesma energia de Gibbs para ambas as fases únicas.
        integer(c_int) :: iLeastGPhaseID       ! Identificador convencionado da fase única na qual a mistura possui a menor energia de Gibbs.
        real(c_double) :: dLeastGibbsEnergy    ! Energia de Gibbs associada à fase única de menor G
        real(c_double), dimension(iNComp) :: oLeastGPhaseLnFug    ! Coeficientes de fugacidade correspondentes à fase única de menor energia de Gibbs.
        real(c_double), dimension(iNComp) :: oLiqLnFug, oPreviousLiqLnFug    ! Vetor dos "ln" dos coeficientes de fugacidade na fase líquida
        real(c_double), dimension(iNComp) :: oVapLnFug, oPreviousVapLnFug    ! Vetor dos "ln" dos coeficientes de fugacidade na fase vapor
        real(c_double) :: dTwoPhaseGibbsEnergy, dPreviousTwoPhaseGibbsEnergy            ! Energias de Gibbs da mistura bifásica
        integer :: iIter
        logical :: bApplyDEMInCurrentIteration
        real(c_double), dimension(iNComp) :: oLnK_JustUpdated           ! Valores de "ln K" ajustados na iteração atual de substituição sucessiva.
        real(c_double), dimension(iNComp) :: oLnK_From1IterationBack    ! Valores de "ln K" ajustados na iteração anterior.
        real(c_double), dimension(iNComp) :: oLnK_From2IterationBack    ! Valores de "ln K" ajustados duas iterações atrás.
        real(c_double), dimension(iNComp) :: oLnK_From3IterationBack    ! Valores de "ln K" ajustados três iterações atrás.
        real(c_double), dimension(iNComp) :: oLnK_DEMResult             ! Vetor de "Ln K" a utilizar na sequência do DEM no cálculo do "flash"
        logical :: bRachfordRiceSolutionExists       ! "True" se a equação de Rachford-Rice foi resolvida com sucesso.
        logical :: bFlashConverged                   ! "True" se foi constatada convergência no cálculo do "flash".
        real(c_double) :: dBetaLiq                   ! Fração molar calculada para a fase líquida
        logical :: bGibbsEnergyDecreases, bFoundValidBeta, bPerformStabilityAnalysisInCurrentIteration
        logical :: bTPDAnalysisSuccessfullyDone, bFeedIsStableAsSinglePhase
        real(c_double), dimension(iNComp) :: oLiqStationaryPointW         ! "Composição" do líquido no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oLiqStationaryPointLnFug     ! Ln dos coeficientes de fugacidade na fase líquida no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oVapStationaryPointW         ! "Composição" do vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oVapStationaryPointLnFug     ! Ln dos coeficientes de fugacidade na fase vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oImprovedKFactors            ! Estimativas melhoras das constantes de equilíbrio "K" (no caso de mistura instável).
        logical :: bIgnoreOutcomeAndResume                                ! Uma das saídas da rotina do método de Newton (consultar lá para mais detalhes).
        !real(c_double) :: dPreviousBetaVap

        ! ------------------ CONSTANTES:
        logical, parameter :: bDEMExtrapolationActive = .true.
        integer, parameter :: iMaxSuccessiveSubstIterations = 1500
        integer, parameter :: iIterationFrequencyForDEM = 5
        integer, parameter :: iMaxIterationsBeforeNewton = 10

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dBetaVap = 0.5d0

        bTPDAnalysisSuccessfullyDone = .false.

        ! Obter, se necessário, estimativas iniciais de "K":
        if(.not.bHasInitialFlashEstimates) call InitializeLiquidAndVaporCompositions(dPressure, dTemperature, iNComp, oZ, oTc, oPc, oW, &
                    oGivenInitialLiqComposition, oGivenInitialVapComposition)

        do i = 1, iNComp
            oK(i) = oGivenInitialVapComposition(i) / oGivenInitialLiqComposition(i)
        end do

        ! Obter a menor energia de Gibbs possível como fase única:
        call GetMixtureVLESinglePhaseWithLeastGibbsEnergy(dTemperature, dPressure, iNComp, oZ, iLiqPhaseModel, &
            iVapPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, bBothPhasesWithSameGEnergyArg = bBothPhasesWithSameGEnergy, &
            iLeastGPhaseIDArg = iLeastGPhaseID, dLeastGibbsEnergyArg = dLeastGibbsEnergy, oLeastGPhaseLnFugArg = oLeastGPhaseLnFug)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Ajustes iniciais para Substituição Sucessiva:
        do i = 1, iNComp
            oLiqLnFug(i) = 1.0d-6
            oVapLnFug(i) = 1.0d-6
        end do

        dTwoPhaseGibbsEnergy = 10.0d0 * dLeastGibbsEnergy

        ! Primeira tentativa de solução do "flash": via Substituição Sucessiva.
        successiveSubstLoop: do iIter = 1, iMaxSuccessiveSubstIterations

            ! Armazenar valor anterior:
            dPreviousTwoPhaseGibbsEnergy = dTwoPhaseGibbsEnergy
            !dPreviousBetaVap = dBetaVap

            ! Aplicar a aceleração DEM agora?
            bApplyDEMInCurrentIteration = bDEMExtrapolationActive .and. (mod(iIter, iIterationFrequencyForDEM).eq.0)

            checkDEM: if(bApplyDEMInCurrentIteration) then

                oPreviousK = oK

                call ExtrapolatePTFlashKWithDEM(iNComp, oLnK_JustUpdated, oLnK_From1IterationBack, oLnK_From2IterationBack, &
                        oLnK_From3IterationBack, oLnK_DEMResult)

                do i = 1, iNComp
                    oK(i) = exp(min(log(huge(1.0)), oLnK_DEMResult(i)))
                end do

            end if checkDEM

            ! Armazenar valores anteriores:
            oPreviousLiqLnFug = oLiqLnFug
            oPreviousVapLnFug = oVapLnFug

            ! Fazer os cálculos de uma "rodada completa" de Substituição Sucessiva:
            call PerformPTVLEFlashSuccessiveSubstitutionIteration(dTemperature, dPressure, iNComp, oZ, oK, &
                oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, &
                iIER, bRachfordRiceSolutionExists, bFlashConverged, dBetaVap, dBetaLiq, oX, oY, oLiqLnFug, &
                oVapLnFug, dTwoPhaseGibbsEnergy, oK_Calc)

            if(iIER.NE.ERROR_EverythingOK) return

            oK = oK_Calc

            ! TESTE!! AVALIAR E APAGAR SE FOR O CASO!!
            ! bFlashConverged = bFlashConverged .and. (abs(dBetaVap-dPreviousBetaVap).lt.(0.005))

            checkConv1: if(bFlashConverged) then

                ! TESTE:
                !write(*, '(I2, 5X, F13.7)') iIter, dBetaVap

                ! Cálculo convergido!
                return
            end if checkConv1

            ! Voltar atrás se a aplicação do DEM não gerou progresso:
            checkDEMProgress: if(bApplyDEMInCurrentIteration) then

                checkGibbsEnPostDEM: if(dTwoPhaseGibbsEnergy.gt.dPreviousTwoPhaseGibbsEnergy) then

                    call PerformPTVLEFlashSuccessiveSubstitutionIteration(dTemperature, dPressure, iNComp, oZ, oPreviousK, &
                        oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, &
                        iIER, bRachfordRiceSolutionExists, bFlashConverged, dBetaVap, dBetaLiq, oX, oY, oLiqLnFug, &
                        oVapLnFug, dTwoPhaseGibbsEnergy, oK_Calc)

                    if(iIER.NE.ERROR_EverythingOK) return

                    oK = oK_Calc

                end if checkGibbsEnPostDEM

            end if checkDEMProgress

            ! Atualização das entradas do "DEM":
            updateDEMInput: if(bDEMExtrapolationActive.and.bRachfordRiceSolutionExists) then

                oLnK_From3IterationBack = oLnK_From2IterationBack
                oLnK_From2IterationBack = oLnK_From1IterationBack
                oLnK_From1IterationBack = oLnK_JustUpdated

                do i = 1, iNComp
                    oLnK_JustUpdated(i) = log(oK(i))
                end do

            end if updateDEMInput

            ! TESTE:
            !write(*, '(I2, 5X, F13.7)') iIter, dBetaVap


            ! Fazer o teste de estabilidade de fases se a energia de Gibbs não diminuiu, ou se a fração vaporizada
            ! está fora do intervalo entre 0 e 1:
            bGibbsEnergyDecreases = (dTwoPhaseGibbsEnergy.lt.dLeastGibbsEnergy)

            bFoundValidBeta = (.not.isnan(dBetaVap)).and.(dBetaVap.gt.(0.0d0)).and.(dBetaVap.lt.(1.0d0))

            bPerformStabilityAnalysisInCurrentIteration = (((.not.bGibbsEnergyDecreases).or.(.not.bFoundValidBeta)) &   ! (i) G não diminuiu ou "beta" não deu válido...
                                                             .and.(iIter.eq.iIterationFrequencyForDEM)) &               !     ... e estamos na iteração do primeiro DEM.
                                                             .or.(.not.bRachfordRiceSolutionExists)                     ! OU (ii) não resolveu RR

            doTPDTest: if(bPerformStabilityAnalysisInCurrentIteration) then

                chkDoneBefore: if(.not.bTPDAnalysisSuccessfullyDone) then

                    call PerformTangentPlaneAnalysisForVLE(dTemperature, dPressure, iNComp, oZ, oLeastGPhaseLnFug, oTc, oPc, oW, oKij, oLij, &
                        oPeneloux, iLiqPhaseModel, iVapPhaseModel, iLeastGPhaseID, bTPDAnalysisSuccessfullyDone, &
                        bFeedIsStableAsSinglePhaseArg = bFeedIsStableAsSinglePhase, oLiqStationaryPointWArg = oLiqStationaryPointW, &
                        oLiqStationaryPointLnFugArg = oLiqStationaryPointLnFug, oVapStationaryPointWArg = oVapStationaryPointW, &
                        oVapStationaryPointLnFugArg = oVapStationaryPointLnFug, oImprovedKFactorsArg = oImprovedKFactors)

                    chkStable1: if(bFeedIsStableAsSinglePhase) then
                        call GetResultsForSinglePhaseFeed(iNComp, oZ, iLeastGPhaseID, dBetaVap, oX, oY)
                        return
                    else chkStable1
                        oK = oImprovedKFactors
                    end if chkStable1

                else chkDoneBefore

                    ! Análise TPD já foi tentada.
                    ! Recorrer ao método de Newton.
                    call TryFlashCalcWith2ndOrderMinimizationIfNecessary(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, &
                            oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, bGibbsEnergyDecreases, bFoundValidBeta, &
                            bTPDAnalysisSuccessfullyDone, oLeastGPhaseLnFug, iLeastGPhaseID, oLiqStationaryPointW, oVapStationaryPointW, &
                            oImprovedKFactors, iIER, dBetaVap, oX, oY, bIgnoreOutcomeAndResume, &
                            bFeedIsStableAsSinglePhase, oLiqStationaryPointLnFug, oVapStationaryPointLnFug)

                    if(.not.bIgnoreOutcomeAndResume) return

                    ! TODO: Implementar método de Newton.
                    ! Enquanto não implementa...
                    ! iIER = ERROR_FlashCalculationFoundNoSolution
                    !return

                end if chkDoneBefore

            end if doTPDTest

            ! "Switch" para Método de Newton caso não convirja antes de determinada iteração:
            tryNewton: if(iIter.eq.iMaxIterationsBeforeNewton) then

                call TryFlashCalcWith2ndOrderMinimizationIfNecessary(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, &
                            oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, bGibbsEnergyDecreases, bFoundValidBeta, &
                            bTPDAnalysisSuccessfullyDone, oLeastGPhaseLnFug, iLeastGPhaseID, oLiqStationaryPointW, oVapStationaryPointW, &
                            oImprovedKFactors, iIER, dBetaVap, oX, oY, bIgnoreOutcomeAndResume, &
                            bFeedIsStableAsSinglePhase, oLiqStationaryPointLnFug, oVapStationaryPointLnFug)

                if(.not.bIgnoreOutcomeAndResume) return

            end if tryNewton

        end do successiveSubstLoop

        ! Se chegou aqui, o cálculo de "flash" não convergiu:
        iIER = ERROR_FlashCalculationFoundNoSolution

    end subroutine CalculateIsothermalPTVLEFlash

    ! =============================================================
    ! =============================================================
    subroutine GetResultsForSinglePhaseFeed(iNComp, oZ, iPhaseID, dBetaVap, oX, oY) &
        bind(C, name='GetResultsForSinglePhaseFeed')

        ! OBJETIVO: Ajustar os resultados quando cálculos prévios apontarem que determinada mistura está em uma única fase.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                   ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ           ! Vetor composição da mistura (indexado por componente)
        integer(c_int), value, intent(in) :: iPhaseID                 ! Identificador da fase única em questão, conforme convenção.

        real(c_double), intent(out) :: dBetaVap                   ! Fração molar calculada para a fase vapor
        real(c_double), dimension(iNComp), intent(out) :: oX      ! Vetor composição da fase líquida
        real(c_double), dimension(iNComp), intent(out) :: oY      ! Vetor composição da fase vapor

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i

        ! ------------------ CÁLCULOS:

        ! Proceder de acordo com a fase que foi determinada:
        whichSinglePhase: if(iPhaseID.EQ.PHASE_Liquid) then

            dBetaVap = 0.0d0
            oX = oZ

            do i = 1, iNComp
                oY(i) = 0.0d0
            end do

        else if(iPhaseID.EQ.PHASE_Vapor) then whichSinglePhase

            dBetaVap = 1.0d0
            oY = oZ

            do i = 1, iNComp
                oX(i) = 0.0d0
            end do

        else whichSinglePhase

            ! Erro!
            dBetaVap = -1000.0d0
            do i = 1, iNComp
                oX(i) = -10.0d0
                oY(i) = -10.0d0
            end do

        end if whichSinglePhase

    end subroutine GetResultsForSinglePhaseFeed

    ! =============================================================
    ! =============================================================
    subroutine PerformPTVLEFlashSuccessiveSubstitutionIteration(dTemperature, dPressure, iNComp, oZ, oK_Given, &
                oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, &
                iIER, bRachfordRiceSolutionExists, bFlashConverged, dBetaVap, dBetaLiq, oX, oY, oLiqLnFug, &
                oVapLnFug, dTwoPhaseGibbsEnergy, oK_Calc)

        ! OBJETIVO: Realizar o ciclo completo de cálculos de uma iteração de Substituição Sucessiva em cálculo de
        !           "Flash P,T líquido/vapor", recebendo um vetor de "K" e recalculando-o ao final.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature                    ! Temperatura
        real(c_double), value, intent(in) :: dPressure                       ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                   ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ           ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oK_Given     ! Vetor inicial de constantes de equilíbrio (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iLiqPhaseModel              ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel              ! Modelo selecionado para a fase vapor, conforme índices convencionados.

        integer(c_int), intent(out) :: iIER                       ! Código de erros, conforme convencionado.
        logical, intent(out) :: bRachfordRiceSolutionExists       ! "True" se a equação de Rachford-Rice foi resolvida com sucesso.
        logical, intent(out) :: bFlashConverged                   ! "True" se foi constatada convergência no cálculo do "flash".
        real(c_double), intent(out) :: dBetaVap                   ! Fração molar calculada para a fase vapor
        real(c_double), intent(out) :: dBetaLiq                   ! Fração molar calculada para a fase líquida
        real(c_double), dimension(iNComp), intent(out) :: oX      ! Vetor composição da fase líquida
        real(c_double), dimension(iNComp), intent(out) :: oY      ! Vetor composição da fase vapor
        real(c_double), dimension(iNComp), intent(out) :: oLiqLnFug   ! Vetor dos "ln" dos coeficientes de fugacidade na fase líquida
        real(c_double), dimension(iNComp), intent(out) :: oVapLnFug   ! Vetor dos "ln" dos coeficientes de fugacidade na fase vapor
        real(c_double), intent(out) :: dTwoPhaseGibbsEnergy           ! Energia de Gibbs da mistura bifásica
        real(c_double), dimension(iNComp), intent(out) :: oK_Calc     ! Valores atualizados de "K"

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dLiqGibbsEnergy, dVapGibbsEnergy
        integer :: i
        real(c_double) :: dRelVar

        ! ------------------ CONSTANTES:
        !real(c_double), parameter :: dRelVarTol = 1.0d6 * epsilon(1.0)      ! Valor ORIGINAL
        real(c_double), parameter :: dRelVarTol = 0.35d0 * 1.0d5 * epsilon(1.0)       ! Valor MODIFICADO (qualquer coisa, reverter para o ORIGINAL)

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        bFlashConverged = .false.

        ! Resolver a equação de Rachford-Rice:
        call SolveRachfordRiceEquation(iNComp, oZ, oK_Given, bRachfordRiceSolutionExists, dBetaVap, dBetaLiq, oX, oY)

        checkRRSol: if(bRachfordRiceSolutionExists) then

            ! Calcular os coeficientes de fugacidade da fase líquida:
            call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Liquid, dTemperature, dPressure, oX, &
                iLiqPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLiqLnFug)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Calcular os coeficientes de fugacidade da fase vapor:
            call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Vapor, dTemperature, dPressure, oY, &
                iVapPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oVapLnFug)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Calcular a energia de Gibbs da mistura bifásica:
            call CalculateRelativeMolarGibbsEnergy(iNComp, oX, oLiqLnFug, dLiqGibbsEnergy)
            call CalculateRelativeMolarGibbsEnergy(iNComp, oY, oVapLnFug, dVapGibbsEnergy)

            dTwoPhaseGibbsEnergy = (dBetaLiq * dLiqGibbsEnergy) + (dBetaVap * dVapGibbsEnergy)

            ! Calcular valores atualizados de "K":
            updateK: do i = 1, iNComp
                oK_Calc(i) = exp(oLiqLnFug(i) - oVapLnFug(i))
            end do updateK

            ! Calcular a variação em relação à iteração anterior:
            dRelVar = maxval(abs((oK_Calc - oK_Given) / oK_Given))

            checkRelVar: if(dRelVar.lt.dRelVarTol) then

                ! Atualizar a solução:
                call SolveRachfordRiceEquation(iNComp, oZ, oK_Calc, bRachfordRiceSolutionExists, dBetaVap, dBetaLiq, oX, oY)

                ! Somente permitir solução bifásica:
                bFlashConverged = ((dBetaVap.ge.(0.0d0)).and.(dBetaVap.le.(1.0d0)))
                bRachfordRiceSolutionExists = bFlashConverged

            end if checkRelVar

        end if checkRRSol

    end subroutine PerformPTVLEFlashSuccessiveSubstitutionIteration

    ! =============================================================
    ! =============================================================
    subroutine SolveRachfordRiceEquation(iNComp, oZ, oK, bSolutionExists, dBetaVap, dBetaLiq, oXArg, oYArg) &
        bind(C, name='SolveRachfordRiceEquation')

        ! OBJETIVO: Determinar "beta" que satisfaz a equação de Rachford-Rice, obtendo também as composições
        !   molares correspondentes das duas fases.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                   ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ           ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oK           ! Vetor de constantes de equilíbrio (indexado por componente)

        logical, intent(out) :: bSolutionExists               ! "True" se houver uma raíz para a equação, "false" caso contrário

        real(c_double), intent(out), optional :: dBetaVap     ! Fração molar calculada para a fase vapor
        real(c_double), intent(out), optional :: dBetaLiq     ! Fração molar calculada para a fase líquida
        real(c_double), dimension(iNComp), intent(out), optional :: oXArg   ! Vetor composição da fase líquida
        real(c_double), dimension(iNComp), intent(out), optional :: oYArg   ! Vetor composição da fase vapor

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dMaxK, dMinK, dMaxBeta, dMinBeta, dBeta, dPreviousBeta, dFinalBetaVap
        real(c_double) :: dRREq0, dRRDeriv0, dRREq1, dRRDeriv1, dRREq, dRRDeriv
        integer :: i, iIter
        integer(c_int) :: iEquationPhaseID                    ! Identificador da fase de "beta" na equação, conforme convenção.
        logical :: bConverged
        real(c_double), dimension(iNComp) :: oDenominatorVec
        real(c_double), dimension(iNComp) :: oX, oY
        real(c_double) :: dDeltaBetaNewton, dDeltaBetaEff, dDeltaBetaEff_1IterBack

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dKOneTol = 10.0d0 * epsilon(1.0)
        integer, parameter :: iMaxIterations = 1500
        real(c_double), parameter :: dNewtonTol = epsilon(1.0) * 1.0d2

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        bSolutionExists = .true.

        ! Verificar situações em que a solução não existe:
        dMaxK = maxval(oK)
        dMinK = minval(oK)

        checkForSolutionAndBounds: if((dMinK.lt.(1.0d0)).and.(dMaxK.gt.(1.0d0))) then

            checkIfAllAreOne: if((abs(dMinK-1.0d0).lt.dKOneTol).and.(abs(dMaxK-1.0d0).lt.dKOneTol)) then
                bSolutionExists = .false.
                return
            end if checkIfAllAreOne

                ! Referência 1?
            dMaxBeta = 1.0d0 / (1.0d0 - dMinK)
            dMinBeta = -1.0d0 / (dMaxK - 1.0d0)

        else checkForSolutionAndBounds

            dMaxBeta = 1.0d0
            dMinBeta = 0.0d0

            call CalculateRachfordRiceEquationAndDerivative(iNComp, oZ, oK, PHASE_Vapor, dMinBeta, dRREq0, dRRDeriv0)
            call CalculateRachfordRiceEquationAndDerivative(iNComp, oZ, oK, PHASE_Vapor, dMaxBeta, dRREq1, dRRDeriv1)

                ! Referência 1, pág 252: "The function is thus monotonically decreasing..."
            checkCrossAxis: if(.not.((dRREq0.gt.(0.0d0)).and.(dRREq1.lt.(0.0d0)))) then
                bSolutionExists = .false.
                return
            end if checkCrossAxis

        end if checkForSolutionAndBounds

        ! Solução existe!
        ! Delimitar intervalo de busca...
        searchIntervalLoop: do i = 1, iNComp
            searchIntervalIf: if(oK(i).gt.(1.0d0)) then
                ! Referência 1, pág 253, equação 7:
                dMinBeta = max(dMinBeta, (oK(i) * oZ(i) - 1.0d0) / (oK(i) - 1.0d0))
            else if(oK(i).lt.(1.0d0)) then searchIntervalIf
                ! Referência 1, pág 253, equação 8:
                dMaxBeta = min(dMaxBeta, (1.0d0 - oZ(i)) / (1.0d0 - oK(i)))
            end if searchIntervalIf
        end do searchIntervalLoop

        ! Determinar se a equação de Rachford-Rice será resolvida para a fração molar do
        ! vapor ou do líquido (empregando se possível o critério descrito na pág 253 da Referência 1):
        iEquationPhaseID = PHASE_Vapor

        canApplyCriteria: if(dMaxBeta.gt.(0.5d0)) then

            call CalculateRachfordRiceEquationAndDerivative(iNComp, oZ, oK, PHASE_Vapor, 0.5d0, dRREq, dRRDeriv)

            rrSign: if(dRREq.lt.(0.0d0)) then
                dMaxBeta = 0.5d0
            else rrSign
                iEquationPhaseID = PHASE_Liquid
                dMinBeta = 1.0d0 - dMaxBeta
                dMaxBeta = 0.5d0
            end if rrSign

        end if canApplyCriteria

        ! Estimativa inicial para o método de Newton:
        dBeta = 0.5d0 * (dMaxBeta + dMinBeta)
        dDeltaBetaEff = 0.0d0

        bConverged = .false.

        newtonLoop: do iIter = 1, iMaxIterations

            ! Evitar divisão por zero:
            zeroDiv: if(iEquationPhaseID.eq.PHASE_Vapor) then
                oDenominatorVec = 1.0d0 + (dBeta * (oK - 1.0d0))
            else zeroDiv
                oDenominatorVec = oK - (dBeta * (oK - 1.0d0))
            end if zeroDiv

            if(any(oDenominatorVec.eq.(0.0d0))) then
                bSolutionExists = .false.
                return
            end if

            ! Calcular a equação de Rachford-Rice e sua derivada:
            call CalculateRachfordRiceEquationAndDerivative(iNComp, oZ, oK, iEquationPhaseID, dBeta, dRREq, dRRDeriv)

            ! Atualizar os limites de acordo:
            updateLim: if(iEquationPhaseID.eq.PHASE_Vapor) then

                if(dRREq.gt.(0.0d0)) then
                    dMinBeta = dBeta
                else if(dRREq.lt.(0.0d0)) then
                    dMaxBeta = dBeta
                end if

            else updateLim

                if(dRREq.lt.(0.0d0)) then
                    dMinBeta = dBeta
                else if(dRREq.gt.(0.0d0)) then
                    dMaxBeta = dBeta
                end if

            end if updateLim

            ! Prosseguir com o Método de Newton:
            dDeltaBetaNewton = -1.0d0 * dRREq / dRRDeriv
            dPreviousBeta = dBeta
            dBeta = dBeta + dDeltaBetaNewton
            dDeltaBetaEff_1IterBack = dDeltaBetaEff

            if((dBeta.lt.dMinBeta).or.(dBeta.gt.dMaxBeta)) dBeta = 0.5d0 * (dMinBeta + dMaxBeta)

            dDeltaBetaEff = dBeta - dPreviousBeta

            checkConvergence: if((abs(dRREq).lt.dNewtonTol).and.(abs(dDeltaBetaEff).lt.dNewtonTol)) then
                bConverged = .true.
                exit newtonLoop
            end if checkConvergence

        end do newtonLoop

        ! Independente de como terminou, retornar os "beta" correspondentes:
        if(present(dBetaVap)) dBetaVap = dBeta
        if(present(dBetaLiq)) dBetaLiq = 1.0d0 - dBeta
        dFinalBetaVap = dBeta

        if(iEquationPhaseID.eq.PHASE_Liquid) then
            if(present(dBetaVap)) dBetaVap = 1.0d0 - dBeta
            if(present(dBetaLiq)) dBetaLiq = dBeta
            dFinalBetaVap = 1.0d0 - dBeta
        end if

        if(.not.bConverged) then
            bSolutionExists = .false.
            return
        end if

        ! Tendo o "beta", já é possível calcular as composições molares das duas fases:
        getCompositions: do i = 1, iNComp

            if(present(oXArg)) then
                oX(i) = oZ(i) / (1.0d0 + (dFinalBetaVap * (oK(i) - 1.0d0)))
            end if

            if(present(oYArg)) then
                oY(i) = oK(i) * oZ(i) / (1.0d0 + (dFinalBetaVap * (oK(i) - 1.0d0)))
            end if

        end do getCompositions

        if(present(oXArg)) oXArg = oX
        if(present(oYArg)) oYArg = oY

    end subroutine SolveRachfordRiceEquation

    ! =============================================================
    ! =============================================================
    subroutine CalculateRachfordRiceEquationAndDerivative(iNComp, oZ, oK, iEquationPhaseID, dPhaseBeta, dRREq, dRRDeriv)

        ! OBJETIVO: Determinar, em meio a um processo iterativo de cálculo de "flash", o valor da equação
        !   de Rachford-Rice e de sua derivada (ambas, tanto em função da fração molar vapor ou líquida,
        !   conforme indicado).
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                   ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ           ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oK           ! Vetor de constantes de equilíbrio (indexado por componente)
        integer(c_int), value, intent(in) :: iEquationPhaseID         ! Identificador da fase de "beta" na equação, conforme convenção.
        real(c_double), intent(in) :: dPhaseBeta                      ! Fração molar da fase em questão.

        real(c_double), intent(out) :: dRREq                          ! Valor calculado para a equação de Rachford-Rice
        real(c_double), intent(out) :: dRRDeriv                       ! Valor calculado para a derivada da equação de Rachford-Rice

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dTerm
        integer :: i

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dRREq = 0.0d0
        dRRDeriv = 0.0d0

        ! Proceder conforme a fase especificada para o cálculo (líquido ou vapor):
        whichEq: select case (iEquationPhaseID)

            case (PHASE_Vapor) whichEq

                ! Fração vaporizada

                ! Equação de Rachford-Rice para o vapor:
                RRForVapLoop: do i = 1, iNComp

                    dTerm = oZ(i) * (oK(i) - 1.0d0) / (1.0d0 + (dPhaseBeta * (oK(i) - 1.0d0)))

                    dRREq = dRREq + dTerm

                    dTerm = (oK(i) - 1.0d0) / (1.0d0 + (dPhaseBeta * (oK(i) - 1.0d0)))
                    dTerm = dTerm * dTerm
                    dTerm = oZ(i) * dTerm
                    dTerm = -1.0d0 * dTerm

                    dRRDeriv = dRRDeriv + dTerm

                end do RRForVapLoop

            case (PHASE_Liquid) whichEq

                ! Fração líquida

                ! Equação de Rachford-Rice para o líquido:
                RRForLiqLoop: do i = 1, iNComp

                    dTerm = oZ(i) * (oK(i) - 1.0d0) / (oK(i) - (dPhaseBeta * (oK(i) - 1.0d0)))

                    dRREq = dRREq + dTerm

                    dTerm = oK(i) - 1.0d0
                    dTerm = dTerm / (oK(i) - (dPhaseBeta * (oK(i) - 1.0d0)))
                    dTerm = dTerm * dTerm
                    dTerm = oZ(i) * dTerm

                    dRRDeriv = dRRDeriv + dTerm

                end do RRForLiqLoop

            case default whichEq

                ! Opção inválida:
                dRREq = -100.0d0
                dRRDeriv = -100.0d0

        end select whichEq

    end subroutine CalculateRachfordRiceEquationAndDerivative


    ! =============================================================
    !           ROTINAS AUXILIARES
    ! =============================================================

    subroutine CalculateLnFugAndDerivativesForEachComponent(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg, oDerivLnCoefFugWithNjArg) &
             bind(C, name='CalculateLnFugAndDerivativesForEachComponent')

        ! OBJETIVO: Calcular, para cada componente de dada fase, os respectivos coeficientes de fugacidade
        !   e suas derivadas.

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                       ! Identificador da fase em questão, conforme convenção.
        real(c_double), value, intent(in) :: dTemperature                   ! Temperatura
        real(c_double), value, intent(in) :: dPressure                      ! Pressão.
        real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition ! Vetor composição da fase em questão (indexado por componente)
        integer(c_int), value, intent(in) :: iPhasicThermodynamicModel        ! EOS Cúbica para a fase em questão, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        integer(c_int), intent(out) :: iIER                                     ! Código de erros, conforme convencionado.
        real(c_double), dimension(iNComp), intent(out), optional :: oLnFugArg   ! Vetor dos "ln" dos coeficientes de fugacidade
        real(c_double), dimension(iNComp, iNComp), intent(out), optional :: oDerivLnCoefFugWithNjArg    ! Matriz das derivadas dos coeficientes de fugacidade com a composição

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oLnFug                          ! Vetor dos "ln" dos coeficientes de fugacidade
        integer :: i, j
        real(c_double) :: dPhasicZ                                           ! Fator de compressibilidade calculado.
        real(c_double), dimension(iNComp) :: oDerivFWithNi                   ! Derivadas de F com relação a "ni" na referência
        logical :: bPhasicZAvailable
        real(c_double) :: dDeriv2FWithVTwo
        real(c_double), dimension(iNComp) :: oDerivFWithNiAndV, oDerivPWithNi
        real(c_double), dimension(iNComp, iNComp) :: oDerivFWithNiNj
        real(c_double), dimension(iNComp, iNComp) :: oDerivLnCoefFugWithNj
        real(c_double) :: dN, dV, dDerivPWithV

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                   ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! PRIMEIRO PASSO: Calcular a função F e suas derivadas
        ! ("Função F" apresentada na Referência Bibliográfica, pág 63, equação "6")
        call CalculateFunctionFValuesAndDerivatives(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
            iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oDerivFWithNiArgg = oDerivFWithNi, &
            bPhasicZAvailableArg = bPhasicZAvailable, dPhasicZArgg = dPhasicZ, &
            dDeriv2FWithVTwoArgg = dDeriv2FWithVTwo, oDerivFWithNiAndVArgg = oDerivFWithNiAndV, oDerivFWithNiNjArgg = oDerivFWithNiNj)

        if(iIER.NE.ERROR_EverythingOK) return

        ! SEGUNDO PASSO: Calcular o fator de compressibilidade Z, caso ainda não esteja disponível:
        calcZIfNecessary: if(.not.bPhasicZAvailable) then

            call CalculatePhasicCompressibilityFactor(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
                iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, dPhasicZ, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

        end if calcZIfNecessary

        ! TERCEIRO PASSO: Calcular os coeficientes de fugacidade para cada componente:
        calcEachLnFugLoop: do i = 1, iNComp
            ! Equação "13" da página 64 da referência bibliográfica
            oLnFug(i) = oDerivFWithNi(i) - log(dPhasicZ)
        end do calcEachLnFugLoop

        if(present(oLnFugArg)) oLnFugArg = oLnFug

        ! QUARTO PASSO: Calcular as derivadas dos coeficientes de fugacidade com a composição:
        dN = sum(oPhasicComposition)

        dV = dN * dR * dTemperature * dPhasicZ / dPressure

        dDerivPWithV = (-1.0d0 * dR * dTemperature * dDeriv2FWithVTwo) - (dN * dR * dTemperature / dV / dV)  ! Equação 9 da pág 63 da referência bibliográfica.

        calcDerivPWithNi: do i = 1, iNComp
            ! Equação "11" da pág 63 da referência bibliográfica:
            oDerivPWithNi(i) = (-1.0d0 * dR * dTemperature * oDerivFWithNiAndV(i)) + (dR * dTemperature / dV)
        end do calcDerivPWithNi

        calcDLnPhiDN_Outer: do i = 1, iNComp

            calcDLnPhiDN_Inner: do j = 1, iNComp

                ! Equação "16" da pág 64 da referência bibliográfica, dividida por "n":
                oDerivLnCoefFugWithNj(i, j) = oDerivFWithNiNj(i, j) + (1.0d0 / dN) + &
                    (oDerivPWithNi(j) * oDerivPWithNi(i) / dDerivPWithV / dR / dTemperature)

            end do calcDLnPhiDN_Inner

        end do calcDLnPhiDN_Outer

        if(present(oDerivLnCoefFugWithNjArg)) oDerivLnCoefFugWithNjArg = oDerivLnCoefFugWithNj

    end subroutine CalculateLnFugAndDerivativesForEachComponent

    ! =============================================================
    ! =============================================================
    subroutine CalculateFunctionFValuesAndDerivatives(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
            iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oDerivFWithNiArgg, bPhasicZAvailableArg, dPhasicZArgg, &
            dDeriv2FWithVTwoArgg, oDerivFWithNiAndVArgg, oDerivFWithNiNjArgg)

        ! OBJETIVO: Calcular valores e derivadas relacionadas à "função F" definida na página 63,
        !   equação 6 da referência abaixo.

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                       ! Identificador da fase em questão, conforme convenção.
        real(c_double), value, intent(in) :: dTemperature                   ! Temperatura
        real(c_double), value, intent(in) :: dPressure                      ! Pressão.
        real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition ! Vetor composição da fase em questão (indexado por componente)
        integer(c_int), value, intent(in) :: iPhasicThermodynamicModel        ! Modelo termodinâmico para a fase em questão, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        integer(c_int), intent(out) :: iIER                                           ! Código de erros, conforme convencionado.
        real(c_double), dimension(iNComp), intent(out), optional :: oDerivFWithNiArgg     ! Derivada de F com os "ni" na referência.
        logical, intent(out), optional :: bPhasicZAvailableArg                           ! "True" caso a rotina retorne o fator de compressibilidade.
        real(c_double), intent(out), optional :: dPhasicZArgg                             ! Fator de compressibilidade.

        real(c_double), intent(out), optional :: dDeriv2FWithVTwoArgg                     ! Derivada segunda de "F" com o volume.
        real(c_double), dimension(iNComp), intent(out), optional :: oDerivFWithNiAndVArgg ! Derivadas de F com o volume e com os "ni" na referência.
        real(c_double), dimension(iNComp, iNComp), intent(out), optional :: oDerivFWithNiNjArgg ! Derivadas de "F" com "ni" e "nj" na referência bibliográfica.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oDerivFWithNi     ! Derivada de F com os "ni" na referência.
        logical :: bPhasicZAvailable                           ! "True" caso a rotina retorne o fator de compressibilidade.
        real(c_double) :: dPhasicZ                             ! Fator de compressibilidade.

        logical :: bThermodynamicModelIsCubicEOS            ! "True" se o modelo termodinâmico for uma equação cúbica de estado.

        real(c_double) :: dDeriv2FWithVTwo
        real(c_double), dimension(iNComp) :: oDerivFWithNiAndV
        real(c_double), dimension(iNComp, iNComp) :: oDerivFWithNiNj

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        bPhasicZAvailable = .false.

        ! O modelo termodinâmico selecionado é uma equação de estado cúbica?
        bThermodynamicModelIsCubicEOS = (iPhasicThermodynamicModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iPhasicThermodynamicModel.EQ.SRK_PENELOUX).or.(iPhasicThermodynamicModel.EQ.PENG_ROBINSON_PENELOUX)

        ! Proceder de acordo com o método selecionado:
        whichMethod: if(bThermodynamicModelIsCubicEOS) then

            bPhasicZAvailable = .true.

            call CalculateFunctionFValuesAndDerivativesFromCubicEOS(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
                iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oDerivFWithNiArg=oDerivFWithNi, dPhasicZArg=dPhasicZ, &
                dDeriv2FWithVTwoArg = dDeriv2FWithVTwo, oDerivFWithNiAndVArg = oDerivFWithNiAndV, oDerivFWithNiNjArg = oDerivFWithNiNj)

            if(iIER.NE.ERROR_EverythingOK) return

        else whichMethod

            ! Método de cálculo não reconhecido!
            iIER = ERROR_InvalidModelForHelmholtzFCalculation
            return

        end if whichMethod

        ! Argumentos de saída:
        if(present(oDerivFWithNiArgg)) oDerivFWithNiArgg = oDerivFWithNi
        if(present(bPhasicZAvailableArg)) bPhasicZAvailableArg = bPhasicZAvailable
        if(present(dPhasicZArgg)) dPhasicZArgg = dPhasicZ
        if(present(dDeriv2FWithVTwoArgg)) dDeriv2FWithVTwoArgg = dDeriv2FWithVTwo
        if(present(oDerivFWithNiAndVArgg)) oDerivFWithNiAndVArgg = oDerivFWithNiAndV
        if(present(oDerivFWithNiNjArgg)) oDerivFWithNiNjArgg = oDerivFWithNiNj

    end subroutine CalculateFunctionFValuesAndDerivatives

    ! =============================================================
    ! =============================================================
    subroutine CalculateFunctionFValuesAndDerivativesFromCubicEOS(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
            iPhasicCubicEOSModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oDerivFWithNiArg, dPhasicZArg, &
            dDeriv2FWithVTwoArg, oDerivFWithNiAndVArg, oDerivFWithNiNjArg)

        ! OBJETIVO: Calcular, a partir de equações cúbicas de estado, valores e derivadas relacionadas à "função F" definida na página 63,
        !   equação 6 da referência abaixo.

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                       ! Identificador da fase em questão, conforme convenção.
        real(c_double), value, intent(in) :: dTemperature                   ! Temperatura
        real(c_double), value, intent(in) :: dPressure                      ! Pressão.
        real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition ! Vetor composição da fase em questão (indexado por componente)
        integer(c_int), value, intent(in) :: iPhasicCubicEOSModel        ! EOS Cúbica para a fase em questão, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        integer(c_int), intent(out) :: iIER                                           ! Código de erros, conforme convencionado.
        real(c_double), dimension(iNComp), intent(out), optional :: oDerivFWithNiArg     ! Derivada de F com os "ni" na referência.
        real(c_double), intent(out), optional :: dPhasicZArg                             ! Fator de compressibilidade.
        real(c_double), intent(out), optional :: dDeriv2FWithVTwoArg                     ! Derivada segunda de "F" com o volume.
        real(c_double), dimension(iNComp), intent(out), optional :: oDerivFWithNiAndVArg ! Derivadas de F com o volume e com os "ni" na referência.
        real(c_double), dimension(iNComp, iNComp), intent(out), optional :: oDerivFWithNiNjArg ! Derivadas de "F" com "ni" e "nj" na referência bibliográfica.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oDerivFWithNi     ! Derivada de F com os "ni" na referência.
        real(c_double) :: dPhasicZ                             ! Fator de compressibilidade.
        real(c_double), dimension(iNComp) :: oCubicEOSaParameters      ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp) :: oCubicEOSbParameters      ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp, iNComp) :: dShortAIJ, dShortBIJ
        real(c_double) :: dMolarVolume
        real(c_double) :: dN                ! "Número total de mols" da mistura
        real(c_double), dimension(iNComp) :: oNPerComp
        real(c_double) :: dV                ! Volume total da mistura
        real(c_double) :: dDelta1, dDelta2
        real(c_double) :: dBMix             ! Parâmetro "b" calculado para a mistura
        integer :: i, j
        real(c_double) :: dReferenceB
        real(c_double) :: dG                    ! "g" da página 88, equação 61 da referência bibliográfica.
        real(c_double) :: dFn, dGB, dShortF     ! "Fn", "gB", "f" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double) :: dShortFV, dShortFB, dCapitalFB    ! "fV", "fB", "FB" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double) :: dCapitalD, dCapitalFD             ! "D", "FD" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double) :: dSum
        real(c_double), dimension(iNComp) :: dCapitalBI, dCapitalDI

        real(c_double) :: dShortFVV, dShortFBB     ! "fVV" e "fBB" no equacionamento da página 91 da referência bibliográfica.
        real(c_double) :: dShortGVV, dShortGBB     ! "gVV" e "gBB" no equacionamento da página 90 da referência bibliográfica.
        real(c_double) :: dCapitalFVV   ! "FVV" no equacionamento da página 90 da referência bibliográfica.
        real(c_double) :: dShortGV, dCapitalFShortNV, dCapitalFDV      ! "gV", "FnV" e "FDV" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double) :: dShortFBV, dShortGBV, dCapitalFBV            ! "fBV", "gBV" e "FBV" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double), dimension(iNComp) :: oDerivFWithNiAndV         ! Derivada de F com V e com os "ni" na referência.
        real(c_double), dimension(iNComp, iNComp) :: oCapitalBIJ, oCapitalDIJ   ! "Bij" e "Dij" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double) :: dCapitalFBB, dCapitalFnB, dCapitalFBD        ! "FBB", "FnB" e "FBD" no equacionamento das páginas 88 a 91 da referência bibliográfica.
        real(c_double), dimension(iNComp, iNComp) :: oDerivFWithNiNj     ! Derivada de F com os "ni" e "nj" na referência.

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                   ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! Calcular o fator de compressibilidade:
        call CalculatePhasicCompressibilityFactorFromCubicEOS(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicCubicEOSModel, oTc, oPc, oW, oKij, oLij, oPeneloux, dPhasicZ, iIER, oCubicEOSaParametersArg=oCubicEOSaParameters, &
             oCubicEOSbParametersArg=oCubicEOSbParameters, dBMixArg=dBMix)

        if(iIER.NE.ERROR_EverythingOK) return

        if(present(dPhasicZArg)) dPhasicZArg = dPhasicZ

        ! Calcular os "aij" (eq 50 da pág 86 da referência bibliográfica):
        do i = 1, iNComp
            do j = 1, iNComp
                dShortAIJ(i, j) = sqrt(oCubicEOSaParameters(i) * oCubicEOSaParameters(j)) * (1.0d0 - oKij(i, j))
            end do
        end do

        ! Calcular os "bij" (eq 52 da pág 86 da referência bibliográfica):
        do i = 1, iNComp
            do j = 1, iNComp
                dShortBIJ(i, j) = 0.5d0 * (oCubicEOSbParameters(i) + oCubicEOSbParameters(j)) * (1.0d0 - oLij(i, j))
            end do
        end do

        ! Calcular o volume molar:
        dMolarVolume = dPhasicZ * dR * dTemperature / dPressure

        ! Obter os "números de mols" individuais por componente:
        oNPerComp = oPhasicComposition

        ! Calcular o "número total de mols" da mistura:
        dN = sum(oPhasicComposition)

        ! Calcular o volume total da mistura:
        dV = dN * dMolarVolume

        ! Parâmetros da forma genérica da equação de estado:
        call GetCubicEOSDeltas(iPhasicCubicEOSModel, dDelta1, dDelta2, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular o "B" da referência bibliográfica (pág 86):
        dReferenceB = dN * dBMix

        ! Calcular a função "g" da referência bibliográfica (equação 61, página 88):
        dG = log(dV - dReferenceB) - log(dV)

        ! Por definição...
        dFn = -1.0d0 * dG                                   ! Eq 75 da pág 89 da referência
        dGB = -1.0d0 * (1.0d0 / (dV - dReferenceB))         ! Eq 81 da pág 89 da referência

        dShortF = 1.0d0 / dR / dReferenceB / (dDelta1 - dDelta2)
        dShortF = dShortF * log((dV + dDelta1 * dReferenceB) / (dV + dDelta2 * dReferenceB))      ! Eq 62 da pág 88 da referência

        dShortFV = -1.0d0 / dR / (dV + dDelta1 * dReferenceB) / (dV + dDelta2 * dReferenceB)     ! Eq 82 da pág 90 da referência

        dShortFB = -1.0d0
        dShortFB = dShortFB * (dShortF + dV * dShortFV) / dReferenceB       ! Eq 83 da pág 90 da referência

        dCapitalD = 0.0d0
        calcCapitalD: do i = 1, iNComp

            dSum = 0.0d0
            do j = 1, iNComp
                dSum = dSum + (oNPerComp(j) * dShortAIJ(i, j))
            end do

            dSum = oNPerComp(i) * dSum
            dCapitalD = dCapitalD + dSum        ! Eq 65 da pág 88 da referência

        end do calcCapitalD

        dCapitalFB = (-1.0d0 * dN * dGB) - dCapitalD / dTemperature * dShortFB     ! Eq 78 da pág 89 da referência

        calcDCapitalBI: do i = 1, iNComp

            dCapitalBI(i) = 0.0d0

            do j = 1, iNComp
                dCapitalBI(i) = dCapitalBI(i) + (oNPerComp(j) * dShortBIJ(i, j))
            end do

            dCapitalBI(i) = (2.0d0 * dCapitalBI(i) - dReferenceB) / dN              ! Eq 108 da pág 91 da referência

        end do calcDCapitalBI

        dCapitalFD = -1.0d0 * dShortF / dTemperature            ! Eq 79 da pág 89 da referência

        calcDCapitalDI: do i = 1, iNComp

            dCapitalDI(i) = 0.0d0

            do j = 1, iNComp
                dCapitalDI(i) = dCapitalDI(i) + (oNPerComp(j) * dShortAIJ(i, j))
            end do

            dCapitalDI(i) = 2.0d0 * dCapitalDI(i)       ! Eq 101 da pág 91 da referência

        end do calcDCapitalDI

        ! Calcular derivadas de F:
        calcFAndDerivs: do i = 1, iNComp
            ! Equação "66" da página 88 da referência:
            oDerivFWithNi(i) = dFn + dCapitalFB * dCapitalBI(i) + dCapitalFD * dCapitalDI(i)
        end do calcFAndDerivs

        if(present(oDerivFWithNiArg)) oDerivFWithNiArg = oDerivFWithNi

        ! ------------ PROSSEGUIR, para possibilitar o cálculo das derivadas dos coeficientes de fugacidade com
        ! relação à composição:

        dShortFVV = (1.0d0 / (dV + dDelta1 * dReferenceB)) ** 2.0d0
        dShortFVV = (1.0d0 / (dV + dDelta2 * dReferenceB)) ** 2.0d0 - dShortFVV
        dShortFVV = 1.0d0 / dR / dReferenceB / (dDelta1 - dDelta2) * dShortFVV             ! Equação 98 da pág 91 da referência

        dShortGVV = (1.0d0 / (dV - dReferenceB)) ** 2.0d0
        dShortGVV = 1.0d0 / dV / dV - dShortGVV                 ! Equação 95 da pág 90 da referência

        dCapitalFVV = (-1.0d0 * dN * dShortGVV) - (dCapitalD / dTemperature * dShortFVV)      ! Equação 94 da pág 90 da referência.

        if(present(dDeriv2FWithVTwoArg)) dDeriv2FWithVTwoArg = dCapitalFVV      ! Equação 74 da pág 89 da referência

        dShortGV = dReferenceB / dV / (dV - dReferenceB)         ! Equação 80 da pág 89 da referência

        dCapitalFShortNV = -1.0d0 * dShortGV                     ! Equação 84 da pág 90 da referência

        dCapitalFDV = -1.0d0 * dShortFV / dTemperature       ! Equação 91 da pág 90 da referência

        dShortFBV = -1.0d0 * (2.0d0 * dShortFV + dV * dShortFVV) / dReferenceB        ! Equação 99 da página 91 da referência bibliográfica.

        dShortGBV = (1.0d0 / (dV - dReferenceB)) ** 2.0d0        ! Equação 96 da página 90 da referência bibliográfica.

        dCapitalFBV = -1.0d0 * dN * dShortGBV - (dCapitalD / dTemperature * dShortFBV)              ! Equação 89 da pág 90 da referência

        ! Calcular mais derivadas de F:
        moreDerivsOfF: do i = 1, iNComp

            ! Equação 71 da página 89 da referência:
            oDerivFWithNiAndV(i) = dCapitalFShortNV + dCapitalFBV * dCapitalBI(i) + dCapitalFDV * dCapitalDI(i)

        end do moreDerivsOfF

        if(present(oDerivFWithNiAndVArg)) oDerivFWithNiAndVArg = oDerivFWithNiAndV

        calcCapitalBAndDij_Outer: do i = 1, iNComp

            calcCapitalBAndDij_Inner: do j = 1, iNComp

                oCapitalBIJ(i, j) = (2.0d0 * dShortBIJ(i, j) - dCapitalBI(i) - dCapitalBI(j)) / dN     ! Equação 109 da pág 91 da referência

                oCapitalDIJ(i, j) = 2.0d0 * dShortAIJ(i, j)                                            ! Equação 103 da pág 91 da referência

            end do calcCapitalBAndDij_Inner

        end do calcCapitalBAndDij_Outer

        dShortGBB = -1.0d0 * ((1.0d0 / (dV - dReferenceB)) ** 2.0d0)                      ! Equação 97 da pág 90 da referência

        dShortFBB = -1.0d0 * (2.0d0 * dShortFB + dV * dShortFBV) / dReferenceB            ! Equação 100 da pág 91 da referência

        dCapitalFBB = -1.0d0 * dN * dShortGBB - (dCapitalD / dTemperature * dShortFBB)    ! Equação 90 da pág 90 da referência

        dCapitalFnB = -1.0d0 * dGB                              ! Equação 85 da pág 90 da referência

        dCapitalFBD = -1.0d0 * dShortFB / dTemperature          ! Equação 92 da pág 90 da referência

        calcDerivFNiNj_Outer: do i = 1, iNComp

            calcDerivFNiNj_Inner: do j = 1, iNComp

                ! Equação 69 na pág 89 da referência:
                oDerivFWithNiNj(i, j) = dCapitalFnB * (dCapitalBI(i) + dCapitalBI(j)) + &
                                        dCapitalFBD * (dCapitalBI(i) * dCapitalDI(j) + dCapitalBI(j) * dCapitalDI(i)) + &
                                        dCapitalFB * oCapitalBIJ(i, j) + &
                                        dCapitalFBB * dCapitalBI(i) * dCapitalBI(j) + &
                                        dCapitalFD * oCapitalDIJ(i, j)

            end do calcDerivFNiNj_Inner

        end do calcDerivFNiNj_Outer

        if(present(oDerivFWithNiNjArg)) oDerivFWithNiNjArg = oDerivFWithNiNj

    end subroutine CalculateFunctionFValuesAndDerivativesFromCubicEOS

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhasicCompressibilityFactor(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, dPhasicZ, iIER)

        ! OBJETIVO: Calcular o fator de compressibilidade de determinada fase, utilizando o modelo termodinâmico apontado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                       ! Identificador da fase em questão, conforme convenção.
        real(c_double), value, intent(in) :: dTemperature                   ! Temperatura
        real(c_double), value, intent(in) :: dPressure                      ! Pressão.
        real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition ! Vetor composição da fase em questão (indexado por componente)
        integer(c_int), value, intent(in) :: iPhasicThermodynamicModel        ! EOS Cúbica para a fase em questão, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), intent(out) :: dPhasicZ                          ! Fator de compressibilidade calculado.
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bThermodynamicModelIsCubicEOS                         ! "True" se o modelo termodinâmico especificado for uma equação cúbica de estado.

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dPhasicZ = -10.0d0

        ! O modelo termodinâmico selecionado é uma equação de estado cúbica?
        bThermodynamicModelIsCubicEOS = (iPhasicThermodynamicModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iPhasicThermodynamicModel.EQ.SRK_PENELOUX).or.(iPhasicThermodynamicModel.EQ.PENG_ROBINSON_PENELOUX)

        ! Proceder de acordo com o método selecionado:
        whichMethod: if(bThermodynamicModelIsCubicEOS) then

            ! Cálculo via equação cúbica de estado:
            call CalculatePhasicCompressibilityFactorFromCubicEOS(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
                iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, dPhasicZ, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

        else whichMethod

            ! Método de cálculo não reconhecido!
            iIER = ERROR_InvalidThermodynamicMethodForZCalculation
            return

        end if whichMethod

    end subroutine CalculatePhasicCompressibilityFactor

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhasicCompressibilityFactorFromCubicEOS(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicCubicEOSModel, oTc, oPc, oW, oKij, oLij, oPeneloux, dPhasicZ, iIER, oCubicEOSaParametersArg, oCubicEOSbParametersArg, &
             dAMixArg, dBMixArg, dCMixArg)

        ! OBJETIVO: Concentrar em uma só rotina os procedimentos necessários para calcular, por via de
        !   uma equação cúbica de estado, o fator de compressibilidade de determinada fase.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                       ! Identificador da fase em questão, conforme convenção.
        real(c_double), value, intent(in) :: dTemperature                   ! Temperatura
        real(c_double), value, intent(in) :: dPressure                      ! Pressão.
        real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition ! Vetor composição da fase em questão (indexado por componente)
        integer(c_int), value, intent(in) :: iPhasicCubicEOSModel        ! EOS Cúbica para a fase em questão, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), intent(out) :: dPhasicZ                          ! Fator de compressibilidade calculado.
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        real(c_double), dimension(iNComp), intent(out), optional :: oCubicEOSaParametersArg      ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp), intent(out), optional :: oCubicEOSbParametersArg      ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double), intent(out), optional :: dAMixArg                                        ! Parâmetro "a" calculado para a mistura.
        real(c_double), intent(out), optional :: dBMixArg                                        ! Parâmetro "b" calculado para a mistura.
        real(c_double), intent(out), optional :: dCMixArg                                        ! Parâmetro "c" (volume de "shift") calculado para a mistura.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dZLiquid                                     ! Fator de compressibilidade da fase líquida.
        real(c_double) :: dZVapor                                      ! Fator de compressibilidade da fase vapor.
        logical :: bComplexRootsWereFound                              ! "True" se foram encontrados fatores de compressibilidade complexos.

        real(c_double), dimension(iNComp) :: oCubicEOSaParameters      ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp) :: oCubicEOSbParameters      ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double) :: dAMix                                        ! Parâmetro "a" calculado para a mistura.
        real(c_double) :: dBMix                                        ! Parâmetro "b" calculado para a mistura.
        real(c_double) :: dCMix                                        ! Parâmetro "c" (volume de "shift") calculado para a mistura.

        ! ------------------ CÁLCULOS:

        ! Determinar as constantes dos componentes puros nas condições do "flash":
        call CalculateCubicEOSParametersForEachComponent(iNComp, dTemperature, iPhasicCubicEOSModel, oTc, oPc, oW, &
                                                   iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        if(iIER.NE.ERROR_EverythingOK) return

        if(present(oCubicEOSaParametersArg)) oCubicEOSaParametersArg = oCubicEOSaParameters
        if(present(oCubicEOSbParametersArg)) oCubicEOSbParametersArg = oCubicEOSbParameters

        ! Determinar as constantes da mistura:
        call CalculateCubicEOSParametersForMixture(iNComp, oPhasicComposition, oKij, oLij, oCubicEOSaParameters, oCubicEOSbParameters, &
                                                       oPeneloux, dAMix, dBMix, dCMix)

        if(present(dAMixArg)) dAMixArg = dAMix
        if(present(dBMixArg)) dBMixArg = dBMix
        if(present(dCMixArg)) dCMixArg = dCMix

        ! Calcular o fator de compressibilidade:
        call CalculateZFromCubicEOS(dPressure, dTemperature, iPhasicCubicEOSModel, dAMix, dBMix, &
                                         dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Retornar o fator de compressibilidade apropriado:
        whichCubicZ: if(iPhaseID.EQ.PHASE_Liquid) then
            dPhasicZ = dZLiquid
        else if(iPhaseID.EQ.PHASE_Vapor) then whichCubicZ
            dPhasicZ = dZVapor
        end if whichCubicZ

    end subroutine CalculatePhasicCompressibilityFactorFromCubicEOS

    ! =============================================================
    ! =============================================================
    subroutine GetCubicEOSDeltas(iCubicEOSModel, dDelta1, dDelta2, iIER)

        ! OBJETIVO: Obter os parâmetros "delta 1" e "delta 2" da forma genérica de equação cúbica de estado
        !   utilizada na referência bibliográfica abaixo (equação 5, pág 74).

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iCubicEOSModel            ! Índice convencionado representando o modelo com o qual se está trabalhando.

        real(c_double), intent(out) :: dDelta1
        real(c_double), intent(out) :: dDelta2
        integer(c_int), intent(out) :: iIER                            ! Código de erros, conforme convencionado.

        ! ------------------ CÁLCULOS:

        ! Identificar a equação cúbica e proceder de acordo:
        whichEOS: if(iCubicEOSModel.EQ.PENG_ROBINSON_78_PENELOUX) then

            dDelta1 = 1.0d0 + sqrt(2.0d0)
            dDelta2 = 1.0d0 - sqrt(2.0d0)

        else if(iCubicEOSModel.EQ.PENG_ROBINSON_PENELOUX) then whichEOS

            dDelta1 = 1.0d0 + sqrt(2.0d0)
            dDelta2 = 1.0d0 - sqrt(2.0d0)

        else if(iCubicEOSModel.EQ.SRK_PENELOUX) then whichEOS

            dDelta1 = 1.0d0
            dDelta2 = 0.0d0

        else whichEOS

            ! Equação de Estado Cúbica não identificada!
            iIER = ERROR_InvalidCubicEOS

        end if whichEOS

    end subroutine GetCubicEOSDeltas

    ! =============================================================
    !           ROTINAS PARA "TANGENT PLANE ANALYSIS"
    ! =============================================================

    subroutine GetMixtureVLESinglePhaseWithLeastGibbsEnergy(dTemperature, dPressure, iNComp, oZ, iLiqPhaseModel, &
        iVapPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, bBothPhasesWithSameGEnergyArg, iLeastGPhaseIDArg, &
        dLeastGibbsEnergyArg, oLeastGPhaseLnFugArg)

        ! OBJETIVO: Verificar se determinada composição apresenta menor energia de Gibbs como fase líquida
        !   ou vapor, e retornar as informações necessárias deste cálculo.

        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature                ! Temperatura
        real(c_double), value, intent(in) :: dPressure                   ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                      ! Número de componentes
        real(c_double), dimension(iNComp), intent(in) :: oZ              ! Vetor composição da mistura (indexado por componente)
        integer(c_int), value, intent(in) :: iLiqPhaseModel              ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel              ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij     ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij     ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        logical, intent(out), optional :: bBothPhasesWithSameGEnergyArg     ! "True" se for calculada a mesma energia de Gibbs para ambas as fases.
        integer(c_int), intent(out), optional :: iLeastGPhaseIDArg   ! Identificador convencionado da fase na qual a mistura possui a menor energia de Gibbs.
        real(c_double), intent(out), optional :: dLeastGibbsEnergyArg       ! Energia de Gibbs associada à fase de menor G
        real(c_double), dimension(iNComp), intent(out), optional :: oLeastGPhaseLnFugArg    ! Coeficientes de fugacidade correspondentes à fase de menor energia de Gibbs.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oLnFugAsLiquid, oLnFugAsVapor, oLeastGPhaseLnFug
        real(c_double) :: dLiqGibbsEnergy, dVapGibbsEnergy
        real(c_double) :: dRelDiffInG
        logical :: bBothPhasesWithSameGEnergy
        integer(c_int) :: iLeastGPhaseID
        real(c_double) :: dLeastGibbsEnergy
        logical :: bForceLiquidBecauseOfEqualG
        integer :: i
        real(c_double) :: dEstimatedTc

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dSameGRelTol = 100.0d0 * epsilon(1.0)
        logical, parameter :: bCheckForLiquidWhenEqualGibbs = .true.

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        bForceLiquidBecauseOfEqualG = .false.

        ! Calcular os coeficientes de fugacidade como fase líquida:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Liquid, dTemperature, dPressure, oZ, &
                iLiqPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg = oLnFugAsLiquid)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular a energia de Gibbs como fase líquida:
        call CalculateRelativeMolarGibbsEnergy(iNComp, oZ, oLnFugAsLiquid, dLiqGibbsEnergy)

        ! Calcular os coeficientes de fugacidade como fase vapor:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Vapor, dTemperature, dPressure, oZ, &
                iVapPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg = oLnFugAsVapor)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular a energia de Gibbs como fase vapor:
        call CalculateRelativeMolarGibbsEnergy(iNComp, oZ, oLnFugAsVapor, dVapGibbsEnergy)

        ! Comparar e retornar resultados:
        dRelDiffInG = abs((dLiqGibbsEnergy - dVapGibbsEnergy) / dLiqGibbsEnergy)

        bBothPhasesWithSameGEnergy = (dRelDiffInG.LT.dSameGRelTol)

        ! Passo adicional quando encontrar ambas as fases com a mesma energia de Gibbs?
        checkForLiqWhenEqualGibbs: if(bBothPhasesWithSameGEnergy.and.bCheckForLiquidWhenEqualGibbs) then

            ! Verificar pela temperatura pseudo-crítica se é melhor "forçar" líquido:
            dEstimatedTc = 0.0d0
            do i = 1, iNComp
                dEstimatedTc = dEstimatedTc + (oZ(i) * oTc(i))
            end do

            bForceLiquidBecauseOfEqualG = (dEstimatedTc.gt.(50.0d0 + 273.15d0))

        end if checkForLiqWhenEqualGibbs

        whichIsSmaller: if((dLiqGibbsEnergy.LT.dVapGibbsEnergy).or.bForceLiquidBecauseOfEqualG) then

            ! Menor energia de Gibbs como fase líquida!
            iLeastGPhaseID = PHASE_Liquid
            oLeastGPhaseLnFug = oLnFugAsLiquid
            dLeastGibbsEnergy = dLiqGibbsEnergy

        else whichIsSmaller

            ! Menor energia de Gibbs como fase vapor!
            iLeastGPhaseID = PHASE_Vapor
            oLeastGPhaseLnFug = oLnFugAsVapor
            dLeastGibbsEnergy = dVapGibbsEnergy

        end if whichIsSmaller

        ! Retornando argumentos opcionais:
        if(present(bBothPhasesWithSameGEnergyArg)) bBothPhasesWithSameGEnergyArg = bBothPhasesWithSameGEnergy
        if(present(iLeastGPhaseIDArg)) iLeastGPhaseIDArg = iLeastGPhaseID
        if(present(oLeastGPhaseLnFugArg)) oLeastGPhaseLnFugArg = oLeastGPhaseLnFug
        if(present(dLeastGibbsEnergyArg)) dLeastGibbsEnergyArg = dLeastGibbsEnergy

    end subroutine GetMixtureVLESinglePhaseWithLeastGibbsEnergy

    ! =============================================================
    ! =============================================================
    subroutine CalculateRelativeMolarGibbsEnergy(iNComp, oMolarFractions, oLnFugs, dGibbsEnergy) &
        bind(C, name='CalculateRelativeMolarGibbsEnergy')

        ! OBJETIVO: Calcular, para dada composição, uma energia de Gibbs com o propósito de determinar diferenças
        !   deste valor entre fases e estados físicos, no contexto do cálculo de um "flash".

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes
        real(c_double), dimension(iNComp), intent(in) :: oMolarFractions    ! Frações molares dos componentes
        real(c_double), dimension(iNComp), intent(in) :: oLnFugs            ! Ln dos coeficientes de fugacidade dos componentes

        real(c_double), intent(out) :: dGibbsEnergy                         ! Energia de Gibbs calculada

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i

        ! ------------------ CÁLCULOS:

        ! Somatórios na equação 39 (pág 266) do Capítulo 10 da Referência:
        dGibbsEnergy = 0.0d0

        do i = 1, iNComp
            ! Observação: o "if" abaixo foi colocado como proteção contra frações molares nulas entrando de argumento para o "log"
            !             na expressão. Só que descobriu-se posteriormente que tais valores nulos vinham chegando aqui em função de
            !             um erro de implementação em outro local (prontamente corrigido). Ou seja: a rigor, esse "if" não é necessário.
            if(oMolarFractions(i).gt.(0.0d0)) dGibbsEnergy = dGibbsEnergy + (oMolarFractions(i) * (log(oMolarFractions(i)) + oLnFugs(i)))
        end do

    end subroutine CalculateRelativeMolarGibbsEnergy

    ! =============================================================
    ! =============================================================
    subroutine PerformTangentPlaneAnalysisForVLE(dTemperature, dPressure, iNComp, oZ, oMixtureLnFug, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                iLiqPhaseModel, iVapPhaseModel, iLeastGPhaseID, bTPDAnalysisSuccessfullyDone, bFeedIsStableAsSinglePhaseArg, &
                oLiqStationaryPointWArg, oLiqStationaryPointLnFugArg, oVapStationaryPointWArg, oVapStationaryPointLnFugArg, oImprovedKFactorsArg) &
                bind(C, name='PerformTangentPlaneAnalysisForVLE')

        ! OBJETIVO: Fazer a verificação de estabilidade pelo critério do plano tangente no contexto do
        !   cálculo de um equilíbrio de fases bifásico líquido-vapor.

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature                    ! Temperatura
        real(c_double), value, intent(in) :: dPressure                       ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ                  ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oMixtureLnFug       ! Ln dos coeficientes de fugacidade na composição da mistura
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iLiqPhaseModel              ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel              ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iLeastGPhaseID   ! Identificador convencionado da fase na qual a mistura possui a menor energia de Gibbs.

        logical, intent(out) :: bTPDAnalysisSuccessfullyDone             ! "True" se a análise de estabilidade for concluída sem erros.

        logical, intent(out), optional :: bFeedIsStableAsSinglePhaseArg                             ! "True" para mistura original estável.
        real(c_double), dimension(iNComp), intent(out), optional :: oLiqStationaryPointWArg         ! "Composição" do líquido no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out), optional :: oLiqStationaryPointLnFugArg     ! Ln dos coeficientes de fugacidade na fase líquida no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out), optional :: oVapStationaryPointWArg         ! "Composição" do vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out), optional :: oVapStationaryPointLnFugArg     ! Ln dos coeficientes de fugacidade na fase vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out), optional :: oImprovedKFactorsArg            ! Estimativas melhoras das constantes de equilíbrio "K" (no caso de mistura instável).

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i
        real(c_double), dimension(iNComp) :: oLiqWInitialGuess, oVapWInitialGuess
        real(c_double), dimension(iNComp) :: oKInitialGuess
        integer(c_int) :: iIER_LiqTPDMin, iIER_VapTPDMin
        real(c_double), dimension(iNComp) :: oLiqStationaryPointW, oVapStationaryPointW
        real(c_double), dimension(iNComp) :: oLiqStationaryPointLnFug, oVapStationaryPointLnFug
        real(c_double) :: dLiqStationaryPointTPD, dVapStationaryPointTPD
        logical :: bLiqTPDNegative, bVapTPDNegative
        logical :: bFeedIsStableAsSinglePhase
        real(c_double), dimension(iNComp) :: oImprovedKFactors
        real(c_double) :: dLastTrialLiqWSumBeforeNormalizationMinusUnity, dLastTrialVapWSumBeforeNormalizationMinusUnity

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dNegativeTPDLimit = -epsilon(1.0) * 1000.0d0
        real(c_double), parameter :: dExpMax = log(huge(1.0))

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        bTPDAnalysisSuccessfullyDone = .true.
        iIER_LiqTPDMin = ERROR_EverythingOK
        iIER_VapTPDMin = ERROR_EverythingOK

        ! Obter estimativas iniciais dos "K":
        call InitializeLiquidAndVaporCompositions(dPressure, dTemperature, iNComp, oZ, oTc, oPc, oW, &
            oLiqWInitialGuess, oVapWInitialGuess)

        do i = 1, iNComp
            oKInitialGuess(i) = oVapWInitialGuess(i) / oLiqWInitialGuess(i)
        end do

        ! PÁG 268 da Referência: "For a liquid feed, we shall search for a vapor-like trial phase which minimizes tm,
        !       and for a vapor feed the search is for a liquid-like trial phase. Unfortunately, at high pressures it
        !       is not possible to make a unique phase identification based on the properties of the feed only, and it
        !       is therefore necessary to investigate both possibilities".

        ! Obter estimativas iniciais das "trial phase" líquida e vapor:
        do i = 1, iNComp
            oLiqWInitialGuess(i) = oZ(i) / oKInitialGuess(i)
            oVapWInitialGuess(i) = oKInitialGuess(i) * oZ(i)
        end do

        ! TESTANDO: Normalizar os chutes iniciais (para evitar estimativas iniciais absurdas. Testes preliminares
        !   mostraram que omitir este passo pode comprometer o resultado final do teste TPD).
        oLiqWInitialGuess = oLiqWInitialGuess / sum(oLiqWInitialGuess)
        oVapWInitialGuess = oVapWInitialGuess / sum(oVapWInitialGuess)

        ! NOTA: Testes preliminares vêm mostrando que, ao menos em determinadas situações, a estimativa inicial destas
        !   composições pode influenciar decisivamente o resultado final do teste TPD. Pode ser o caso de, no futuro,
        !   incluir argumentos opcionais de entrada em que se possa especificar "de fora" estes chutes iniciais, em vez
        !   de ter apenas Wilson como opção.

        ! +++++++++++++++++++++++++++++++++
        !   TESTE - APAGAR
        !oLiqWInitialGuess(1) = 0.3560014
        !oLiqWInitialGuess(2) = 0.3672262
        !oLiqWInitialGuess(3) = 0.0485744
        !oLiqWInitialGuess(4) = 0.0345025
        !oLiqWInitialGuess(5) = 0.0201899
        !oLiqWInitialGuess(6) = 0.0205756
        !oLiqWInitialGuess(7) = 0.0333430
        !oLiqWInitialGuess(8) = 0.0641366
        !oLiqWInitialGuess(9) = 0.0177805
        !oLiqWInitialGuess(10) = 0.0376698

        !oVapWInitialGuess(1) = 0.4092658
        !oVapWInitialGuess(2) = 0.4126861
        !oVapWInitialGuess(3) = 0.0481644
        !oVapWInitialGuess(4) = 0.0315602
        !oVapWInitialGuess(5) = 0.0173134
        !oVapWInitialGuess(6) = 0.0160147
        !oVapWInitialGuess(7) = 0.0230861
        !oVapWInitialGuess(8) = 0.0338897
        !oVapWInitialGuess(9) = 0.0046670
        !oVapWInitialGuess(10) = 0.0033524

        ! FIM DO TESTE - APAGAR ATÉ AQUI
        ! +++++++++++++++++++++++++++++++++


        ! Fazer os cálculos com TPD para líquido:
        call MinimizeTPDFunction(dTemperature, dPressure, iNComp, oZ, oMixtureLnFug, PHASE_Liquid, oLiqWInitialGuess, &
            iLiqPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER_LiqTPDMin, oStationaryPointW = oLiqStationaryPointW, &
            oStationaryPointLnFug = oLiqStationaryPointLnFug, dStationaryPointTPD = dLiqStationaryPointTPD, &
            dLastTrialWSumBeforeNormalizationMinusUnity = dLastTrialLiqWSumBeforeNormalizationMinusUnity)

                ! Correu ok?
        bTPDAnalysisSuccessfullyDone = bTPDAnalysisSuccessfullyDone .AND. (iIER_LiqTPDMin.EQ.ERROR_EverythingOK)

        bLiqTPDNegative = (iIER_LiqTPDMin.EQ.ERROR_EverythingOK).AND.(dLiqStationaryPointTPD.LT.dNegativeTPDLimit)

            ! TESTE - Não estava no algoritmo original - somente aceitar "liquid trial" para "feed" vapor:
        if(bLiqTPDNegative) bLiqTPDNegative = (iLeastGPhaseID.EQ.PHASE_Vapor)

        ! Fazer os cálculos com TPD para vapor:
        call MinimizeTPDFunction(dTemperature, dPressure, iNComp, oZ, oMixtureLnFug, PHASE_Vapor, oVapWInitialGuess, &
            iVapPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER_VapTPDMin, oStationaryPointW = oVapStationaryPointW, &
            oStationaryPointLnFug = oVapStationaryPointLnFug, dStationaryPointTPD = dVapStationaryPointTPD, &
            dLastTrialWSumBeforeNormalizationMinusUnity = dLastTrialVapWSumBeforeNormalizationMinusUnity)

                ! Correu ok?
        bTPDAnalysisSuccessfullyDone = bTPDAnalysisSuccessfullyDone .AND. (iIER_VapTPDMin.EQ.ERROR_EverythingOK)

        bVapTPDNegative = (iIER_VapTPDMin.EQ.ERROR_EverythingOK).AND.(dVapStationaryPointTPD.LT.dNegativeTPDLimit)

            ! TESTE - Não estava no algoritmo original - somente aceitar "vapor trial" para "feed" líquida:
        if(bVapTPDNegative) bVapTPDNegative = (iLeastGPhaseID.EQ.PHASE_Liquid)

        ! Interpretar e encaminhar os resultados:
        allOkIf: if(bTPDAnalysisSuccessfullyDone) then

            isFeedStable: if(bLiqTPDNegative.OR.bVapTPDNegative) then

                ! Instável como fase única!
                bFeedIsStableAsSinglePhase = .false.

                ! PÁG 267 da Referência: "[...] if the stability analysis reveals that the feed is unstable,
                !       we need to generate an improved K-Factor estimate for the subsequent phase split calculation."

                improvedEstimates: if(bLiqTPDNegative.AND.bVapTPDNegative) then

                    do i = 1, iNComp
                        oImprovedKFactors(i) = exp(min(dExpMax, oLiqStationaryPointLnFug(i) - oVapStationaryPointLnFug(i)))
                    end do

                else if (bLiqTPDNegative) then improvedEstimates

                    do i = 1, iNComp
                            ! PÁG 266 da Referência, ítem "b":
                        oImprovedKFactors(i) = exp(min(dExpMax, oLiqStationaryPointLnFug(i) - oMixtureLnFug(i)))
                        oLiqStationaryPointLnFug(i) = 1.0d-6
                        oVapStationaryPointLnFug(i) = 1.0d-6
                    end do

                else if (bVapTPDNegative) then improvedEstimates

                    do i = 1, iNComp
                            ! PÁG 266 da Referência, ítem "b":
                        oImprovedKFactors(i) = exp(min(dExpMax, oMixtureLnFug(i) - oVapStationaryPointLnFug(i)))
                        oLiqStationaryPointLnFug(i) = 1.0d-6
                        oVapStationaryPointLnFug(i) = 1.0d-6
                    end do

                end if improvedEstimates

                ! Retornar os "K" reestimados, se for o caso:
                if(present(oImprovedKFactorsArg)) oImprovedKFactorsArg = oImprovedKFactors
                if(present(oLiqStationaryPointLnFugArg)) oLiqStationaryPointLnFugArg = oLiqStationaryPointLnFug
                if(present(oVapStationaryPointLnFugArg)) oVapStationaryPointLnFugArg = oVapStationaryPointLnFug

            else isFeedStable

                ! Estável como fase única!
                bFeedIsStableAsSinglePhase = .true.

            end if isFeedStable

        end if allOkIf

        ! Resultados opcionais:
        if(present(bFeedIsStableAsSinglePhaseArg)) bFeedIsStableAsSinglePhaseArg = bFeedIsStableAsSinglePhase
        if(present(oLiqStationaryPointWArg)) oLiqStationaryPointWArg = oLiqStationaryPointW
        if(present(oVapStationaryPointWArg)) oVapStationaryPointWArg = oVapStationaryPointW

    end subroutine PerformTangentPlaneAnalysisForVLE

    ! =============================================================
    ! =============================================================
    subroutine MinimizeTPDFunction(dTemperature, dPressure, iNComp, oMixtureComposition, oMixtureLnFug, iTrialPhaseID, oWInitialGuess, &
            iTrialPhaseThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oStationaryPointW, oStationaryPointLnFug, &
            dStationaryPointTPD, dLastTrialWSumBeforeNormalizationMinusUnity)

        ! OBJETIVO: Minimizar a função TPD no contexto de uma "tangent plane analysis", com a função dada
        !   na forma da equação 44 do Capítulo 10 da Referência Bibliográfica.

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature                    ! Temperatura
        real(c_double), value, intent(in) :: dPressure                       ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMixtureComposition ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oMixtureLnFug       ! Ln dos coeficientes de fugacidade na composição da mistura
        integer(c_int), value, intent(in) :: iTrialPhaseID                   ! Identificador da "trial phase", conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oWInitialGuess      ! Chute inicial para a "composição"da "trial phase"
        integer(c_int), value, intent(in) :: iTrialPhaseThermodynamicModel   ! Modelo termodinâmico para a "trial phase", conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.

        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.

                ! OBS: Os resultados a seguir referem-se a pontos estacionários encontrados OU ENTÃO a
                !      composições onde tenha sido encontrado valor negativo de TPD!
        real(c_double), dimension(iNComp), intent(out), optional :: oStationaryPointW       ! "w" que corresponde a um ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out), optional :: oStationaryPointLnFug   ! Ln dos coeficientes de fugacidade correspondentes ao ponto estacionário encontrado.
        real(c_double), intent(out), optional :: dStationaryPointTPD                        ! Valor da função TPD no ponto estacionário encontrado
        real(c_double), intent(out), optional :: dLastTrialWSumBeforeNormalizationMinusUnity

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i, iIter
        real(c_double), dimension(iNComp) :: oD         ! "di" da eq 3 do Cap 9 da referência
        real(c_double), dimension(iNComp) :: oCurrentW, oStationaryPointW_Newton
        real(c_double), dimension(iNComp) :: oCurrentWLnFug, oPreviousWLnFug
        real(c_double) :: dCurrentWTPD
        real(c_double) :: dNegativeTPDCriteriaToUse
        logical :: bSuccessiveSubstitutionFoundNegativeTPD
        logical :: bTryAnotherNumericalMethod
        logical :: bSuccessiveSubstitutionConverged
        real(c_double) :: dError
        real(c_double) :: dLastTrialWSumBeforeNormalizationMinusUnity_Loc
        logical :: bIgnoreNewtonOutcomeAndResume

        ! ------------------ CONSTANTES:
        integer, parameter :: iSuccessiveSubstitutionMaxAttempts = 30       ! Limite máximo de iterações para Substituição Sucessiva
                                                                            ! (caso se deseje reverter: era 30 o valor original de "iSuccessiveSubstitutionMaxAttempts")
                                                                            ! (caso se deseje reverter: era 60 o valor anterior de "iSuccessiveSubstitutionMaxAttempts")
                                                                            ! (caso se deseje reverter: era 3 o valor anterior de "iSuccessiveSubstitutionMaxAttempts")
                                                                            ! NOTA: A prática de rodar apenas 3 Substituições Sucessivas aqui antes de chavear para Newton
                                                                            !       estava originando resultados errados lá, de solução difícil.
        real(c_double), parameter :: dNegativeTPDCriteria = -0.001
        !real(c_double), parameter :: dDefaultRelativeTol = epsilon(1.0) * 1.0d5            ! VALOR ORIGINAL
        !real(c_double), parameter :: dDefaultRelativeTol = epsilon(1.0) * 1.0d5 * 0.35d0    ! VALOR MODIFICADO (caso necessário, só reverter para o ORIGINAL acima)
        real(c_double), parameter :: dDefaultRelativeTol = epsilon(1.0) * 1.0d5 * 0.46d0    ! VALOR MODIFICADO (caso necessário, só reverter para o ORIGINAL acima)

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dNegativeTPDCriteriaToUse = dNegativeTPDCriteria
        bSuccessiveSubstitutionFoundNegativeTPD = .false.
        bSuccessiveSubstitutionConverged = .false.

        ! Calcular os "di" (eq 3 do Cap 9 da referência - atentar que a eq 43 do Cap 10 tem um pequeno erro em "di"):
        calcDi: do i = 1, iNComp
            oD(i) = 0.0d0
            if(oMixtureComposition(i).GT.(0.0d0)) oD(i) = log(oMixtureComposition(i)) + oMixtureLnFug(i)

            ! Só aproveitando o laço para inicializar...
            oCurrentWLnFug(i) = 0.0d0

        end do calcDi

        ! Tentar por Substituição Sucessiva:
        oCurrentW = oWInitialGuess

        successiveSubstMainLoop: do iIter = 1, iSuccessiveSubstitutionMaxAttempts

            ! Armazenar o resultado da iteração anterior:
            oPreviousWLnFug = oCurrentWLnFug

            ! Calcular os coeficientes de fugacidade do "W atual":
            call CalculateLnFugAndDerivativesForEachComponent(iNComp, iTrialPhaseID, dTemperature, dPressure, oCurrentW, &
                iTrialPhaseThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg = oCurrentWLnFug)

            if(iIER.NE.ERROR_EverythingOK) return

            call CalculateTPD(iNComp, oCurrentW, oCurrentWLnFug, oD, dCurrentWTPD)

            ! Verificar se encontrou "tpd" negativo:
            bSuccessiveSubstitutionFoundNegativeTPD = (dCurrentWTPD.LT.dNegativeTPDCriteriaToUse)
            checkNegativeTPD: if(bSuccessiveSubstitutionFoundNegativeTPD) then

                ! Encerrar porque achou tpd negativo, violando assim o critério de estabilidade.
                exit successiveSubstMainLoop

                ! NOTA: Esta prática - de encerrar um método iterativo ANTES DE SUA CONVERGÊNCIA ao deparar-se com um
                !   valor intermediário negativo de TPD - já produziu resultados inválidos, e por isto já foi descontinuada
                !   no Método de Newton. Espera-se que seja descontinuada no futuro aqui em Substituições Sucessivas também.
                !   Para mais detalhes, ver comentários na rotina "TerminateStabAnalysisPrematurely" de "Newton.f90".

            end if checkNegativeTPD

            ! Calcular o erro atual para verificação de convergência:
            dError = 0.0d0

            calcErrorLoop: do i = 1, iNComp
                chooseErrorContribution: if(abs(oPreviousWLnFug(i)).lt.(100.0d0 * dDefaultRelativeTol)) then
                    dError = max(dError, abs(oCurrentWLnFug(i) - oPreviousWLnFug(i)))
                else chooseErrorContribution
                    dError = max(dError, abs((oCurrentWLnFug(i) - oPreviousWLnFug(i)) / oPreviousWLnFug(i)))
                end if chooseErrorContribution
            end do calcErrorLoop

            ! Erro dentro da convergência?
            bSuccessiveSubstitutionConverged = (dError.lt.dDefaultRelativeTol)
            checkSSConvergence: if(bSuccessiveSubstitutionConverged) then
                ! Encerrar, pois o resultado já foi encontrado.
                exit successiveSubstMainLoop
            end if checkSSConvergence

            ! Atualizar a "composição w":
            dLastTrialWSumBeforeNormalizationMinusUnity_Loc = -1.0d0
            updateWLoop: do i = 1, iNComp
                ! Equação 51 (pág 269) do capítulo 10 da referência:
                if(oMixtureComposition(i).GT.(0.0d0)) oCurrentW(i) = exp(oD(i) - oCurrentWLnFug(i))

                dLastTrialWSumBeforeNormalizationMinusUnity_Loc = dLastTrialWSumBeforeNormalizationMinusUnity_Loc + oCurrentW(i)

            end do updateWLoop

            if(present(dLastTrialWSumBeforeNormalizationMinusUnity)) dLastTrialWSumBeforeNormalizationMinusUnity = dLastTrialWSumBeforeNormalizationMinusUnity_Loc

            ! TESTANDO - Normalizar "W" entre iterações:
            oCurrentW = oCurrentW / sum(oCurrentW)

        end do successiveSubstMainLoop

        ! Substituição Sucessiva encerrada.
        ! É para tentar outro método numérico? Ou já chegamos na conclusão?
        bTryAnotherNumericalMethod = (.not.bSuccessiveSubstitutionFoundNegativeTPD)
        bTryAnotherNumericalMethod = bTryAnotherNumericalMethod .and. (.not.bSuccessiveSubstitutionConverged)

        tryingAnotherNumericalMethod: if(bTryAnotherNumericalMethod) then

            ! Acionar a minimização por Newton:
            call TryStabilityAnalysisWith2ndOrderMinimization(dTemperature, dPressure, iNComp, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                    iTrialPhaseID, iTrialPhaseThermodynamicModel, oCurrentW, oD, dNegativeTPDCriteriaToUse, iIER, bIgnoreNewtonOutcomeAndResume, &
                    oStationaryPointW_Newton, oCurrentWLnFug, dCurrentWTPD, dLastTrialWSumBeforeNormalizationMinusUnity_Loc)

            if(iIER.NE.ERROR_EverythingOK) return

            considerNewton: if(.not.bIgnoreNewtonOutcomeAndResume) then

                oCurrentW = oStationaryPointW_Newton

                if(present(dLastTrialWSumBeforeNormalizationMinusUnity)) dLastTrialWSumBeforeNormalizationMinusUnity = dLastTrialWSumBeforeNormalizationMinusUnity_Loc

            end if considerNewton

            ! TODO: Implementar aqui o emprego de um "second order minimizer", conforme recomendado no
            !   Capítulo 10 da referência bibliográfica. Por enquanto, se a execução chegar aqui, será
            !   considerado que o cálculo da TPD não atingiu convergência.
            !iIER = ERROR_MinimizationOfTPDFunctionDidNotConverge
            !return

        end if tryingAnotherNumericalMethod

        ! Normalizar:
        oCurrentW = oCurrentW / sum(oCurrentW)

        ! Retornar resultados:
        if(present(oStationaryPointW)) oStationaryPointW = oCurrentW
        if(present(oStationaryPointLnFug)) oStationaryPointLnFug = oCurrentWLnFug
        if(present(dStationaryPointTPD)) dStationaryPointTPD = dCurrentWTPD

    end subroutine MinimizeTPDFunction

    ! =============================================================
    ! =============================================================
    subroutine CalculateTPD(iNComp, oTrialPhaseW, oTrialPhaseLnFug, oD, dTPD) &
        bind(C, name='CalculateTPD')

        ! OBJETIVO: Calcular a "tangent plane distance" conforme equação 44 do Capítulo 10 da referência bibliográfica.

        ! REFERÊNCIA BIBLIOGRÁFICA: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oTrialPhaseW        ! "Composição" da "trial phase"
        real(c_double), dimension(iNComp), intent(in) :: oTrialPhaseLnFug    ! "Ln" dos coeficientes de fugacidade da "trial phase"
        real(c_double), dimension(iNComp), intent(in) :: oD                  ! Parâmetros "di" que figuram na referida equação 44

        real(c_double), intent(out) :: dTPD                                  ! "Tangent plane distance" calculada.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i

        ! ------------------ CÁLCULOS:

        dTPD = 1.0d0

        calcSumTPD: do i = 1, iNComp
            if(oTrialPhaseW(i).GT.(0.0d0)) dTPD = dTPD + (oTrialPhaseW(i) * (log(oTrialPhaseW(i)) + oTrialPhaseLnFug(i) - oD(i) - 1.0d0))
        end do calcSumTPD

    end subroutine CalculateTPD

    ! =============================================================
    !           ROTINAS PARA "DOMINANT EIGENVALUE METHOD"
    ! =============================================================

    subroutine ExtrapolatePTFlashKWithDEM(iNComp, oLnK_JustUpdated, oLnK_From1IterationBack, oLnK_From2IterationBack, &
            oLnK_From3IterationBack, oLnK_Result)

        ! OBJETIVO: Acelerar a convergência da Substituição Sucessiva aplicada a um problema de "flash" através da
        !   aplicação da técnica GDEM (General Dominant Eigenvalue Method), conforme descrito nas referências abaixo.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição

        ! REFERÊNCIA BIBLIOGRÁFICA 2: "Convergence Promotion in the Simulation of Chemical Processes - The
        !       General Dominant Eigenvalue Method", C.M. Crowe e M. Nishio, AIChE Journal (1975)

        ! OBSERVAÇÕES IMPORTANTES:

        !   i) Referência 1, pág 261: "The acceleration method can be used equally well with the usual choice of the
        !           ln Ki as the independent variables, and this is normally preferable, as a better scaling of the
        !           variables is obtained."

        !   ii) A Referência 2 define "delta" como "forward difference operator". De acordo com uma rápida pesquisa,
        !           isso significa que "delta x n" nessa referência seria "delta x n = x_(n+1) - x n". Isto é consistente
        !           com a "definição" de "delta" convencionada na equação 2 do Apêndice A da Referência 1.

        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oLnK_JustUpdated           ! Valores de "ln K" ajustados na iteração atual de substituição sucessiva.
        real(c_double), dimension(iNComp), intent(in) :: oLnK_From1IterationBack    ! Valores de "ln K" ajustados na iteração anterior.
        real(c_double), dimension(iNComp), intent(in) :: oLnK_From2IterationBack    ! Valores de "ln K" ajustados duas iterações atrás.
        real(c_double), dimension(iNComp), intent(in) :: oLnK_From3IterationBack    ! Valores de "ln K" ajustados três iterações atrás.

        real(c_double), dimension(iNComp), intent(out) :: oLnK_Result               ! Vetor de "Ln K" a utilizar na sequência do cálculo do "flash"

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oDeltaIterPlus1, oDeltaIterPlus0, oDeltaIterMinus1
        real(c_double) :: dLambdaOne, dUDenominator, dU1, dU2
        real(c_double), dimension(0:2, 1:2) :: dB

        ! ------------------ CONSTANTES:
        integer, parameter :: iNumberOfDominantEigenvalues = 2      ! Número suposto de autovalores dominantes.

        ! ------------------ CÁLCULOS:

        ! Proceder de acordo com o número suposto de autovalores dominantes:
        checkNOfEigenvalues: select case(iNumberOfDominantEigenvalues)

            case (1) checkNOfEigenvalues

                ! Calcular "lambda1" (Referência 1, pág 370, resultado da eq 23)
                oDeltaIterPlus1 = oLnK_JustUpdated - oLnK_From1IterationBack
                oDeltaIterPlus0 = oLnK_From1IterationBack - oLnK_From2IterationBack

                dLambdaOne = sum(oDeltaIterPlus0 * oDeltaIterPlus1) / sum(oDeltaIterPlus0 * oDeltaIterPlus0)

                ! Fazer a extrapolação?

                    ! Referência 1, pág 262: "[...] an estimated lambda-1 value above unity should not be used at all
                    !                           for extrapolation, as it leads to a step in the wrong direction".

                checkAndExtrapolate: if(dLambdaOne.LT.(1.0d0)) then

                    ! Extrapolar (equação 22 da página 370 da Referência 1):
                    oLnK_Result = oLnK_JustUpdated + (oDeltaIterPlus1 / (1.0d0 - dLambdaOne))
                            ! TODO: Dúvida se "oLnK_JustUpdated" acima não deveria ser "oLnK_From1IterationBack" para
                            !           de fato seguirmos a equação 22 da página 370 da Referência 1... e também é o que
                            !           parece necessário para que esse cálculo aí respeite a equação 22 da Referência 2,
                            !           em conjunto com a definição de "delta x n" apresentada para esta referência na abertura
                            !           desta rotina. SUGESTÃO: rodar o "flash" analisando o comportamento com as duas possibilidades.

                            ! TESTE 7-JUL-2023: APAGAR A LINHA ABAIXO SE FOR O CASO:
                            ! (parece ter melhorado só um pouco os resultados. No caso de 2 autovalores a melhora foi maior. Acompanhar!)
                            oLnK_Result = oLnK_From1IterationBack + (oDeltaIterPlus1 / (1.0d0 - dLambdaOne))

                else checkAndExtrapolate

                    ! Não extrapolar:
                    oLnK_Result = oLnK_JustUpdated

                end if checkAndExtrapolate

            case (2) checkNOfEigenvalues

                ! Calcular os "delta":
                oDeltaIterPlus1 = oLnK_JustUpdated - oLnK_From1IterationBack
                oDeltaIterPlus0 = oLnK_From1IterationBack - oLnK_From2IterationBack
                oDeltaIterMinus1 = oLnK_From2IterationBack - oLnK_From3IterationBack

                ! OBS: Verifica-se pela Eq 22 da Referência 2 que seu "delta x n" corresponde a "oDeltaIterPlus1"
                !       calculado acima para o caso de um autovalor...

                ! Podemos inferir então que:
                !   --> "delta x n - 1" corresponde a "oDeltaIterPlus0"
                !   --> "delta x n - 2" corresponde a "oDeltaIterMinus1"

                ! Seguindo a Referência 2, Equações 26 e 27: antes, calcular os "b" (Eq 16 da mesma referência):
                dB(0,1) = sum(oDeltaIterPlus1 * oDeltaIterPlus0)
                dB(0,2) = sum(oDeltaIterPlus1 * oDeltaIterMinus1)
                dB(1,1) = sum(oDeltaIterPlus0 * oDeltaIterPlus0)
                dB(1,2) = sum(oDeltaIterPlus0 * oDeltaIterMinus1)
                dB(2,1) = sum(oDeltaIterMinus1 * oDeltaIterPlus0)
                dB(2,2) = sum(oDeltaIterMinus1 * oDeltaIterMinus1)

                ! Calcular agora os "u1" e "u2" (equação 27 da Referência 2):
                dUDenominator = dB(1, 1) * dB(2, 2) - (dB(1, 2) * dB(1, 2))

                denGTZero: if(abs(dUDenominator).gt.(1.0d-20)) then

                    dU1 = dB(0, 2) * dB(1, 2) - (dB(0, 1) * dB(2, 2))
                    dU1 = dU1 / dUDenominator

                    dU2 = dB(0, 1) * dB(1, 2) - (dB(0, 2) * dB(1, 1))
                    dU2 = dU2 / dUDenominator

                    ! Fazer a extrapolação (equação 26 da Referência 2):
                    oLnK_Result = oLnK_JustUpdated + ((oDeltaIterPlus1 - (dU2 * oDeltaIterPlus0)) / (1.0d0 + dU1 + dU2))

                        ! TESTE! APAGAR A LINHA ABAIXO SE FOR O CASO!! - 7-JUL-2023
                        ! (parece estar mais de acordo com a referência conforme observações abaixo... e parece produzir melhores resultados também!)
                        oLnK_Result = oLnK_From1IterationBack + ((oDeltaIterPlus1 - (dU2 * oDeltaIterPlus0)) / (1.0d0 + dU1 + dU2))

                        ! TODO: Os sinais estão corretos no denominador?? Ou era pra subtrair dU1 e dU2?? Erro no artigo?
                        ! TODO 2: Vale aqui a mesma observação do caso de um autovalor, com relação à convenção de "x n".
                        !           Temos que partir dos 4 últimos valores disponíveis de ln K, e por isso não há discussão
                        !           quanto à escolha de "delta x n", "delta x n-1" e "delta x n-2" necessários para a aplicação
                        !           da fórmula. E se "delta x n" é fixado por isto, "x n" que entra na fórmula também é...
                        ! SUGESTÃO: Rodar o flash e ver como ele vai se comportar com a implementação atual. E aí reanalisar
                        !           tudo, inclusive os sinais do denominador. Mas parece quase certo que um teste com a obs
                        !           acima será necessário.

                else denGTZero

                    ! Não extrapolar:
                    oLnK_Result = oLnK_JustUpdated

                end if denGTZero

            case default checkNOfEigenvalues

                ! Não extrapolar:
                oLnK_Result = oLnK_JustUpdated

        end select checkNOfEigenvalues

    end subroutine ExtrapolatePTFlashKWithDEM

    ! =============================================================
    !           ROTINA PARA TESTES
    ! =============================================================
    subroutine TestFlashExtras(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, &
             ! NOVOS ARGUMENTOS
             oZ, iLiqPhaseModel, iVapPhaseModel) bind(C, name="TestFlashExtras")

        ! OBJETIVO: Escrever resultados de cálculos puramente para fins de testes
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                       ! Identificador da fase em questão, conforme convenção.
        real(c_double), value, intent(in) :: dTemperature                   ! Temperatura
        real(c_double), value, intent(in) :: dPressure                      ! Pressão.
        real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition ! Vetor composição da fase em questão (indexado por componente)
        integer(c_int), value, intent(in) :: iPhasicThermodynamicModel        ! EOS Cúbica para a fase em questão, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij        ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij        ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp), intent(in) :: oZ             ! Composição global da mistura
        integer(c_int), value, intent(in) :: iLiqPhaseModel
        integer(c_int), value, intent(in) :: iVapPhaseModel

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer(c_int) :: iIER                           ! Código de erros, conforme convencionado.
        real(c_double), dimension(iNComp) :: oLnFug      ! Vetor dos "ln" dos coeficientes de fugacidade
        integer :: i
        real(c_double), dimension(iNComp) :: oTraditionalFugCoef      ! Vetor de coeficientes de fugacidade calculados (indexado por componente).
        real(c_double), dimension(iNComp) :: oDifLnFugs
        real(c_double), dimension(iNComp) :: oTraditionalLnFugCoef
        real(c_double), dimension(iNComp) :: oCubicEOSaParameters       ! Vetor de parâmetros "a" dos componentes (indexado por componente)
        real(c_double), dimension(iNComp) :: oCubicEOSbParameters       ! Vetor de parâmetros "b" dos componentes (indexado por componente)
        real(c_double) :: dMixCubicEOSaShort                     ! Parâmetro "a" minúsculo da mistura.
        real(c_double) :: dMixCubicEOSbShort                     ! Parâmetro "b" minúsculo da mistura.
        real(c_double) :: dMixCubicEOSc                          ! Parâmetro "c" da mistura
        real(c_double) :: dPhasicZ


        logical :: bBothPhasesWithSameGEnergy        ! "True" se for calculada a mesma energia de Gibbs para ambas as fases.
        integer(c_int):: iLeastGPhaseID              ! Identificador convencionado da fase na qual a mistura possui a menor energia de Gibbs.
        real(c_double) :: dLeastGibbsEnergy                       ! Energia de Gibbs associada à fase de menor G
        real(c_double), dimension(iNComp) :: oLeastGPhaseLnFug    ! Coeficientes de fugacidade correspondentes à fase de menor energia de Gibbs.


        logical :: bTPDAnalysisSuccessfullyDone             ! "True" se a análise de estabilidade for concluída sem erros.

        logical :: bFeedIsStableAsSinglePhase                             ! "True" para mistura original estável.
        real(c_double), dimension(iNComp) :: oLiqStationaryPointW         ! "Composição" do líquido no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oLiqStationaryPointLnFug     ! Ln dos coeficientes de fugacidade na fase líquida no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oVapStationaryPointW         ! "Composição" do vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oVapStationaryPointLnFug     ! Ln dos coeficientes de fugacidade na fase vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp) :: oImprovedKFactors

        ! ------------------ CONSTANTES:
        logical, parameter :: bAllowWrite = .false.

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        iIER = ERROR_EverythingOK

        ! Escrever, se for o caso...
        if(bAllowWrite) write(*, '(///, A)') '===========> FLASH EXTRAS ESCREVENDO'

        ! Calcular coeficientes de fugacidade pelo Framework novo:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg = oLnFug)

        ! Obter dados de entrada para cálculo dos coeficientes de fugacidade pelo método tradicional:
        call CalculatePhasicCompressibilityFactorFromCubicEOS(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, dPhasicZ, iIER, oCubicEOSaParametersArg=oCubicEOSaParameters, &
             oCubicEOSbParametersArg=oCubicEOSbParameters, &
             dAMixArg=dMixCubicEOSaShort, dBMixArg=dMixCubicEOSbShort, dCMixArg=dMixCubicEOSc)

        ! Calcular os coeficientes de fugacidade pelo método tradicional:
        call CalculateCubicEOSFugacityCoefficientsForEachComponent(iNComp, oPhasicComposition, oKij, oLij, oCubicEOSaParameters, &
                                oCubicEOSbParameters, dPhasicZ, dPressure, dTemperature, dMixCubicEOSaShort, dMixCubicEOSbShort, &
                                dMixCubicEOSc, iPhasicThermodynamicModel, iPhaseID, oTraditionalFugCoef, iIER)

        ! Trocar os coeficientes de fugacidade tradicionais para "ln":
        do i = 1, iNComp
            oTraditionalLnFugCoef(i) = log(oTraditionalFugCoef(i))
            oDifLnFugs(i) = (oTraditionalLnFugCoef(i) - oLnFug(i)) / oTraditionalLnFugCoef(i) * 100.0d0
        end do

        ! Escrever, se for o caso...
        if(bAllowWrite) then

            if(iIER.EQ.ERROR_EverythingOK) then
               write(*, '(/, A, /)') 'Sucesso no cálculo novo de coeficientes de fugacidade!'
            else
               write(*, '(/, A, /)') 'Erro no cálculo novo de coeficientes de fugacidade!'
            end if

        end if

        ! Escrever, se for o caso...
        if(bAllowWrite) then

            do i = 1, iNComp

                write(*, '(A, I2, A, F13.7, 5X, A, I2, A, F13.7, 5X, A, F13.7)') 'Ln Fug Novo (', i, ') = ', oLnFug(i), &
                                                            'Ln Fug Tradicional (', i, ') = ', oTraditionalLnFugCoef(i), &
                                                            'Diferença Percentual = ', oDifLnFugs(i)

            end do

        end if


        ! Testes para o cálculo da estabilidade de fases:
        call GetMixtureVLESinglePhaseWithLeastGibbsEnergy(dTemperature, dPressure, iNComp, oZ, iLiqPhaseModel, &
            iVapPhaseModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, &
            bBothPhasesWithSameGEnergyArg = bBothPhasesWithSameGEnergy, iLeastGPhaseIDArg = iLeastGPhaseID, &
            dLeastGibbsEnergyArg = dLeastGibbsEnergy, oLeastGPhaseLnFugArg = oLeastGPhaseLnFug)

        if(bAllowWrite) then

            if(iIER.EQ.ERROR_EverythingOK) then

               if(iLeastGPhaseID.EQ.PHASE_Liquid) then
                    write(*, '(/, A, F13.7, 3X, A, /)') 'Menor energia de Gibbs = ', dLeastGibbsEnergy, '  (líquido)'
               else if(iLeastGPhaseID.EQ.PHASE_Vapor) then
                    write(*, '(/, A, F13.7, 3X, A, /)') 'Menor energia de Gibbs = ', dLeastGibbsEnergy, '  (vapor)'
               else
                    write(*, '(/, A, /)') 'Algo errado: não foi possível identificar a fase com a menor energia de Gibbs.'
               end if

            else
               write(*, '(/, A, /)') 'Erro no cálculo da menor energia de Gibbs da mistura!'
            end if

        end if


        call PerformTangentPlaneAnalysisForVLE(dTemperature, dPressure, iNComp, oZ, oLeastGPhaseLnFug, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                iLiqPhaseModel, iVapPhaseModel, iLeastGPhaseID, bTPDAnalysisSuccessfullyDone, &
                bFeedIsStableAsSinglePhaseArg = bFeedIsStableAsSinglePhase, &
                oLiqStationaryPointWArg = oLiqStationaryPointW, oLiqStationaryPointLnFugArg = oLiqStationaryPointLnFug, &
                oVapStationaryPointWArg = oVapStationaryPointW, oVapStationaryPointLnFugArg = oVapStationaryPointLnFug, &
                oImprovedKFactorsArg = oImprovedKFactors)

        if(bAllowWrite) then

            if(bTPDAnalysisSuccessfullyDone) then

                if(bFeedIsStableAsSinglePhase) then
                    write(*, '(/, A, /)') 'Mistura estável como fase única!'
                else
                    write(*, '(/, A, /)') 'Mistura instável como fase única!'

                    do i = 1, iNComp

                        write(*, '(A, I2, A, F13.7, 5X, A, I2, A, F13.7, 5X, A, F13.7)') 'xSP (', i, ') = ', oLiqStationaryPointW(i), &
                                                            'ySP (', i, ') = ', oVapStationaryPointW(i), &
                                                            'Z = ', oZ(i)

                    end do

                end if

            else
               write(*, '(/, A, /)') 'Erro no cálculo de estabilidade de fases!'
            end if

        end if


    end subroutine TestFlashExtras


end module FlashExtras
