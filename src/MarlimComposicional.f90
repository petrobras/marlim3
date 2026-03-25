module MarlimComposicional

    ! OBJETIVO: Conter as rotinas a serem acionadas quando o simulador Marlim Transiente necessitar realizar cálculos composicionais.
    ! UNIDADES: TODA a programação contida neste módulo considera que TODOS os valores (entradas e saídas
    !   das rotinas e suas variáveis internas) estão no SI, EXCETO QUANTO devidamente explicitado...

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use VLECalculations
    use PvtSimCTMFileImport
    use PhaseProperties
    use BlackOilFromCompositional
    use FlashExtras
    use PvtSimWAXFileImport

    implicit none

! ===================================================================================================
!       "TYPE" PARA ARGUMENTOS DE SAÍDA
! ===================================================================================================
    type, bind(C) :: ZRhoAndDerivs
        ! Estrutura de "struct" para conter resultados de Z, massa específica e derivadas de ambos.
        real(c_double) :: dLiquidZ
        real(c_double) :: dLiquidZWithPeneloux
        real(c_double) :: dDerivLiquidZWithT
        real(c_double) :: dDerivLiquidRhoWithT
        real(c_double) :: dDerivLiquidZWithP
        real(c_double) :: dDerivLiquidRhoWithP
        real(c_double) :: dVaporZWithPeneloux
        real(c_double) :: dDerivVaporZWithT
        real(c_double) :: dDerivVaporRhoWithT
        real(c_double) :: dDerivVaporZWithP
        real(c_double) :: dDerivVaporRhoWithP
        real(c_double) :: dDerivLiquidZPenelouxWithT
        real(c_double) :: dDerivLiquidZPenelouxWithP
        real(c_double) :: dDerivVaporZPenelouxWithT
        real(c_double) :: dDerivVaporZPenelouxWithP
    end type ZRhoAndDerivs

    type, bind(C) :: InputForViscosity
        ! Estrutura de "struct" para conter os dados necessários para o cálculo da viscosidade.
        real(c_double), dimension(4) :: oPVTSIMViscCSPTuningParam       ! Coeficientes ajustados para o método CSP de viscosidade no PVTSIM
    end type InputForViscosity


    contains

! ===================================================================================================
!       ROTINA PRINCIPAL A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR AS PROPRIEDADES!!
! ===================================================================================================
    subroutine MarlimCompositionalProps(dFlashPressure, dFlashTemperature, iNComp, oZ, oMW, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                            iIGCoefsSource, oCpIGCoefs, oHIGCoefs, oParachors, iLiqModel, iVapModel, iLiqDensityCalculationMethod, &
                            iVapDensityCalculationMethod, iLiqHAndCpThermodynamicModel, iVapHAndCpThermodynamicModel, &
                            dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                            dCalculatedBeta, oLiqCompositionAtPAndT, oVapCompositionAtPAndT, &
                            dVaporMassFraction, dBubbleP, dLiqDensity, dVapDensity, dLiqMassEnthalpy, &
                            dVapMassEnthalpy, dLiqMassCp, dVapMassCp, dSurfaceTension, dVaporZ, dLiqJouleThomsonCoef, dVapJouleThomsonCoef, &
                            oZRhoAndDerivs, iIER) bind(C, name = "MarlimCompositionalProps")

        ! OBJETIVO: Encaminhar os cálculos composicionais de propriedades requeridos pelo simulador Marlim,
        !           e retornar os resultados destes cálculos.
        ! IMPORTANTE: Como fica claro pela lista de argumentos, esta rotina parte do pressuposto de que os componentes individuais
        !   (pseudos, SCN, etc) e suas propriedades já foram definidos no começo da simulação. Esta aqui é a rotina que o Marlim
        !   chamará PONTO A PONTO para calcular "flash" e propriedades!

        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure      ! Pressão na qual deseja-se calcular o "flash".
        real(c_double), value, intent(in) :: dFlashTemperature   ! Temperatura na qual deseja-se calcular o "flash".
        integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
        type(c_ptr), value, intent(in) :: oZ                     ! Vetor de frações molares GLOBAIS dos componentes na mistura.
        type(c_ptr), value, intent(in) :: oMW                    ! Vetor de massas molares (indexado por componente).
        type(c_ptr), value, intent(in) :: oTc                    ! Vetor de temperaturas críticas dos componentes.
        type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
        type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
        type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
        type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
        type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada compponente.
        integer(c_int), value, intent(in) :: iIGCoefsSource      ! Índice da fonte dos coeficientes polinomiais de gás ideal, conforme convenção.
        type(c_ptr), value, intent(in) :: oCpIGCoefs             ! Matriz (se existir) dos coeficientes polinomiais do Cp de gás ideal.
        type(c_ptr), value, intent(in) :: oHIGCoefs              ! Matriz (se existir) dos coeficientes polinomiais da entalpia de gás ideal.
        type(c_ptr), value, intent(in) :: oParachors             ! Vetor de "parachors" para cada componente.
        integer(c_int), value, intent(in) :: iLiqModel           ! Modelo da fase líquida, conforme convenção.
        integer(c_int), value, intent(in) :: iVapModel           ! Modelo da fase vapor, conforme convenção.
        integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod  ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
        integer(c_int), value, intent(in) :: iVapDensityCalculationMethod  ! Índice do método de cálculo da massa específica do vapor, conforme convenção.
        integer(c_int), value, intent(in) :: iLiqHAndCpThermodynamicModel  ! Modelo termodinâmico para cálculo da entalpia e Cp do líquido, conforme convencionado.
        integer(c_int), value, intent(in) :: iVapHAndCpThermodynamicModel  ! Modelo termodinâmico para cálculo da entalpia e Cp do vapor, conforme convencionado.
        real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash".
                                                                             ! (OBS: Dar valor NEGATIVO caso não se disponha de estimativas iniciais).
        type(c_ptr), value, intent(in) :: oGivenInitialLiqComposition        ! Estimativa inicial de composição da fase líquida para o "flash".
        type(c_ptr), value, intent(in) :: oGivenInitialVapComposition        ! Estimativa inicial de composição da fase vapor para o "flash".


        real(c_double), intent(out) :: dCalculatedBeta                       ! Fração molar vaporizada determinada.
        type(c_ptr), value, intent(in) :: oLiqCompositionAtPAndT             ! Composição molar da fase líquida a P e T dados.
        type(c_ptr), value, intent(in) :: oVapCompositionAtPAndT             ! Composição molar da fase vapor a P e T dados.


        real(c_double), intent(out) :: dVaporMassFraction        ! Fração mássica vaporizada.
        real(c_double), intent(out) :: dBubbleP                  ! Pressão de bolha.
        real(c_double), intent(out) :: dLiqDensity               ! Massa específica da fase líquida.
        real(c_double), intent(out) :: dVapDensity               ! Massa específica calculada para a fase vapor.
        real(c_double), intent(out) :: dLiqMassEnthalpy          ! Entalpia do líquido em base mássica ("J/kg").
        real(c_double), intent(out) :: dVapMassEnthalpy          ! Entalpia do vapor em base mássica ("J/kg").
        real(c_double), intent(out) :: dLiqMassCp                ! "Cp" do líquido em base mássica ("J/kg/K").
        real(c_double), intent(out) :: dVapMassCp                ! "Cp" do vapor em base mássica ("J/kg/K").
        real(c_double), intent(out) :: dSurfaceTension           ! Tensão superficial calculada.
        real(c_double), intent(out) :: dVaporZ                   ! Fator de compressibilidade do vapor calculado.
        real(c_double), intent(out) :: dLiqJouleThomsonCoef      ! Coeficiente de Joule-Thomson para a fase líquida ("K/Pa")
        real(c_double), intent(out) :: dVapJouleThomsonCoef      ! Coeficiente de Joule-Thomson para a fase vapor ("K/Pa")
        type(ZRhoAndDerivs), intent(out) :: oZRhoAndDerivs       ! Diversos resultados agrupados (Z, massa específica e derivadas)
        integer(c_int), intent(out) :: iIER                      ! Código de erros, conforme convenção.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double), dimension(:), pointer :: temp_oParachors ! Mapeamento do argumento "oParachors"
        real(c_double), dimension(:), pointer :: temp_oMW        ! Mapeamento do argumento "oMW"
        real(c_double), dimension(:), pointer :: temp_oZ         ! Mapeamento do argumento "oZ"
        real(c_double), dimension(:), pointer :: temp_oTc        ! Mapeamento do argumento "oTc"
        real(c_double), dimension(:), pointer :: temp_oPc        ! Mapeamento do argumento "oPc"
        real(c_double), dimension(:), pointer :: temp_oW         ! Mapeamento do argumento "oW"
        real(c_double), dimension(:,:), pointer :: temp_oKij     ! Mapeamento do argumento "oKij"
        real(c_double), dimension(:,:), pointer :: temp_oLij     ! Mapeamento do argumento "oLij"
        real(c_double), dimension(:), pointer :: temp_oPeneloux  ! Mapeamento do argumento "oPeneloux"
        real(c_double), dimension(:), pointer :: temp_oGivenInitialLiqComposition ! Mapeamento do argumento "oGivenInitialLiqComposition"
        real(c_double), dimension(:), pointer :: temp_oGivenInitialVapComposition ! Mapeamento do argumento "oGivenInitialVapComposition"
        real(c_double), dimension(:), pointer :: oCalculatedLiqComposition ! Composição molar determinada da fase líquida.
        real(c_double), dimension(:), pointer :: oCalculatedVapComposition ! Composição molar determinada da fase vapor.
        integer :: i, j                                          ! Índices para laços ("do", etc)
        real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc     ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
        real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc     ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.

        real(c_double) :: dTBubble, dTOrv
        integer(c_int) :: iIER_BubbleT, iIER_DewT, iIER_Flash, iCalculatedThermodynamicCondition, iIER_BubbleP


        real(c_double) :: dLiquidZ                                          ! Fator de compressibilidade calculado para a fase líquida
        real(c_double) :: dLiquidZWithPeneloux
        real(c_double) :: dDerivLiquidZWithT
        real(c_double) :: dDerivLiquidRhoWithT
        real(c_double) :: dDerivLiquidZWithP
        real(c_double) :: dDerivLiquidRhoWithP

        real(c_double) :: dVaporZWithPeneloux
        real(c_double) :: dDerivVaporZWithT
        real(c_double) :: dDerivVaporRhoWithT
        real(c_double) :: dDerivVaporZWithP
        real(c_double) :: dDerivVaporRhoWithP

        real(c_double) :: dDerivLiquidZPenelouxWithT
        real(c_double) :: dDerivLiquidZPenelouxWithP
        real(c_double) :: dDerivVaporZPenelouxWithT
        real(c_double) :: dDerivVaporZPenelouxWithP

        ! ---------------------------------
        ! -------- VARIÁVEIS DE TESTE!! Algumas se tornarão definitivas! APAGAR as demais DEPOIS!!
        real(c_double) :: dSumL, dSumV, dGottenBubbleT, dGottenDewT, dCurrentBubbleP, dPhaseEnvInitialBeta
        integer :: iUnit
        logical :: bGotBubbleT, bGotDewT
        real(c_double) :: dTenLiqPorcent, dTTenLiqPorcent, dThirtyLiqPorcent, dTThirtyLiqPorcent, dFiftyLiqPorcent, dTFiftyLiqPorcent, &
            dSeventyLiqPorcent, dTSeventyLiqPorcent, dPOfIter, dTOfIter
        real(c_double), dimension(iNComp) :: oPhaseEnvInitialLiqComposition, oPhaseEnvInitialVapComposition
        real(c_double), dimension(:,:), pointer :: temp_oCpIGCoefs
        ! ---------FIM DAS VARIÁVEIS DE TESTE
        ! ---------------------------------------

        ! VARIÁVEIS LOCAIS PARA TRATAMENTO DE COMPONENTES "ZERADOS":
        logical, dimension(iNComp) :: isZeroedComp             ! "True" se o componente "i" estava zerado originalmente
        integer(c_int) :: iNComp_Fixed                         ! Número de componentes não-nulos.
        integer :: iMemoryAllocationError
        real(c_double), allocatable, dimension(:) :: oZ_Fixed  ! Frações molares globais dos componentes não-nulos.
        real(c_double), allocatable, dimension(:) :: oMW_Fixed
        real(c_double), allocatable, dimension(:) :: oTc_Fixed
        real(c_double), allocatable, dimension(:) :: oPc_Fixed
        real(c_double), allocatable, dimension(:) :: oW_Fixed
        real(c_double), allocatable, dimension(:) :: oPeneloux_Fixed
        real(c_double), allocatable, dimension(:) :: oGivenInitialLiqComp_Fixed
        real(c_double), allocatable, dimension(:) :: oGivenInitialVapComp_Fixed
        real(c_double), allocatable, dimension(:,:) :: oKij_Fixed
        real(c_double), allocatable, dimension(:,:) :: oLij_Fixed
        real(c_double), allocatable, dimension(:) :: oCalculatedLiqComp_Fixed
        real(c_double), allocatable, dimension(:) :: oCalculatedVapComp_Fixed

        ! ------------ CONSTANTES:
        logical, parameter :: bEnableCalculationsWithoutZeroedComponents = .TRUE.       ! "True" para desconsiderar em alguns cálculos os componentes de
                                                                                        !   fração molar nula na composição global, quando houver possibilidade
                                                                                        !   de sua presença causar erros numéricos.

        ! ------------ MAPEANDO VETORES E MATRIZES:
        call c_f_pointer(oZ, temp_oZ, [iNComp])
        call c_f_pointer(oTc, temp_oTc, [iNComp])
        call c_f_pointer(oPc, temp_oPc, [iNComp])
        call c_f_pointer(oW, temp_oW, [iNComp])
        call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
        call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
        call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])
        call c_f_pointer(oMW, temp_oMW, [iNComp])
        call c_f_pointer(oParachors, temp_oParachors, [iNComp])
        call c_f_pointer(oGivenInitialLiqComposition, temp_oGivenInitialLiqComposition, [iNComp])
        call c_f_pointer(oGivenInitialVapComposition, temp_oGivenInitialVapComposition, [iNComp])
        call c_f_pointer(oLiqCompositionAtPAndT, oCalculatedLiqComposition , [iNComp])
        call c_f_pointer(oVapCompositionAtPAndT, oCalculatedVapComposition , [iNComp])

        ! ------------ POR CONVENIÊNCIA, TRANSPONDO MATRIZES:
            ! NOTA: Por enquanto, as matrizes "temp" abaixo estão sendo geradas em outra rotina Fortran, e não em C++.
            !   Por isso, não estava fazendo sentido transpô-las.
        oKij_ForCalc = transpose(transpose(temp_oKij))
        oLij_ForCalc = transpose(transpose(temp_oLij))


        ! TRATAMENTO DE COMPONENTES DE FRAÇÃO MOLAR NULA NA COMPOSIÇÃO GLOBAL:
        treatZeroedComps: if(bEnableCalculationsWithoutZeroedComponents) then

            ! Fazer a remoção desses componentes:

            ! Contar:
            call CountZeroedComponents(iNComp, temp_oZ, isZeroedComp, iNComp_Fixed)

            ! Alocar memória:
            allocate(oMW_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oZ_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oTc_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oPc_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oW_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oPeneloux_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oKij_Fixed(iNComp_Fixed, iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oLij_Fixed(iNComp_Fixed, iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oGivenInitialLiqComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oGivenInitialVapComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oCalculatedLiqComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oCalculatedVapComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)

            ! Pegar os valores:
            call RemoveZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, temp_oZ, temp_oMW, temp_oTc, temp_oPc, temp_oW, temp_oPeneloux, temp_oGivenInitialLiqComposition, &
                temp_oGivenInitialVapComposition, oKij_ForCalc, oLij_ForCalc, (dGivenInitialBeta.GE.(0.0d0)), oZ_Fixed, oMW_Fixed, oTc_Fixed, oPc_Fixed, oW_Fixed, oPeneloux_Fixed, &
                oKij_Fixed, oLij_Fixed, oGivenInitialLiqComp_Fixed, oGivenInitialVapComp_Fixed)

        end if treatZeroedComps



        ! CALCULAR PRIMEIRAMENTE A CONDIÇÃO TERMODINÂMICA!
        iIER = ERROR_EverythingOK

        deactivate: if(.false.) then
            ! Chamada à subrotina antiga
            call CalculateMixtureThermodynamicCondition(dFlashPressure, dFlashTemperature, iNComp, temp_oZ, temp_oTc, temp_oPc, temp_oW, &
                    temp_oPeneloux, oKij_ForCalc, oLij_ForCalc, iLiqModel, iVapModel, &
                    (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, &
                    dTBubble, iIER_BubbleT, &
                    dTOrv, iIER_DewT, dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, iIER_Flash, &
                    dBubbleP, iIER_BubbleP, iCalculatedThermodynamicCondition, iIER)

        else deactivate

            deactivateV2: if(.false.) then

                call CalculateMixtureThermodynamicCondition_V2(dFlashPressure, dFlashTemperature, iNComp, temp_oMW, temp_oZ, temp_oTc, temp_oPc, temp_oW, &
                        temp_oPeneloux, oKij_ForCalc, oLij_ForCalc, iLiqModel, iVapModel, (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, &
                        temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, dCalculatedBeta, oCalculatedLiqComposition, &
                        oCalculatedVapComposition, iCalculatedThermodynamicCondition, iIER_Flash, iIER)

            else deactivateV2

                proceedWithoutZeroeds: if(bEnableCalculationsWithoutZeroedComponents) then

                    ! Chamada sem componentes zerados:
                    call CalculateMixtureThermodynamicCondition_V3(dFlashPressure, dFlashTemperature, iNComp_Fixed, oMW_Fixed, &
                        oZ_Fixed, oTc_Fixed, oPc_Fixed, oW_Fixed, oKij_Fixed, oLij_Fixed, oPeneloux_Fixed, iLiqModel, iVapModel, &
                        (dGivenInitialBeta.GE.(0.0d0)), oGivenInitialLiqComp_Fixed, oGivenInitialVapComp_Fixed, &
                        dCalculatedBeta, oCalculatedLiqComp_Fixed, oCalculatedVapComp_Fixed, iCalculatedThermodynamicCondition, iIER_Flash, iIER)

                else proceedWithoutZeroeds

                    ! Chamada original:
                    call CalculateMixtureThermodynamicCondition_V3(dFlashPressure, dFlashTemperature, iNComp, temp_oMW, &
                        temp_oZ, temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, temp_oPeneloux, iLiqModel, iVapModel, &
                        (dGivenInitialBeta.GE.(0.0d0)), temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, &
                        dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, iCalculatedThermodynamicCondition, iIER_Flash, iIER)

                end if proceedWithoutZeroeds

            end if deactivateV2

            dTBubble = -1000.0d0
            iIER_BubbleT = -1
            dTOrv = -1000.0d0
            iIER_DewT = -1
            dBubbleP = -1000.0d0
            iIER_BubbleP = 0            ! Atenção...

        end if deactivate

        ! Concluir o tratamento de componentes zerados, se for o caso:
        concludeZeroedsTreat: if(bEnableCalculationsWithoutZeroedComponents) then

            ! "Recompor" as composições calculadas:
            call ReInsertZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, oCalculatedLiqComp_Fixed, oCalculatedLiqComposition)
            call ReInsertZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, oCalculatedVapComp_Fixed, oCalculatedVapComposition)

        end if concludeZeroedsTreat





        if(iIER.NE.ERROR_EverythingOK) return       ! Não há como prosseguir se houve erro na determinação da condição termodinâmica.

        ! CALCULAR AS PROPRIEDADES TERMOFÍSICAS E DE TRANSPORTE!
        call CalculatePhaseProperties_Approach1(iNComp, temp_oMW, temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_Forcalc, temp_oPeneloux, temp_oParachors, temp_oZ, &
                    dFlashPressure, dFlashTemperature, iCalculatedThermodynamicCondition, dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, &
                    iLiqDensityCalculationMethod, iVapDensityCalculationMethod, iLiqHAndCpThermodynamicModel, &
                    iVapHAndCpThermodynamicModel, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, iLiqModel, iVapModel, dVaporMassFraction, dLiquidZ, &
                    dLiquidZWithPeneloux, dLiqDensity, &
                    dDerivLiquidZWithT, dDerivLiquidZPenelouxWithT, dDerivLiquidRhoWithT, dDerivLiquidZWithP, dDerivLiquidZPenelouxWithP, &
                    dDerivLiquidRhoWithP, dVaporZ, dVaporZWithPeneloux, &
                    dVapDensity, dDerivVaporZWithT, dDerivVaporZPenelouxWithT, dDerivVaporRhoWithT, dDerivVaporZWithP, dDerivVaporZPenelouxWithP, &
                    dDerivVaporRhoWithP, dLiqMassEnthalpy, dVapMassEnthalpy, dLiqMassCp, dVapMassCp, dSurfaceTension, dLiqJouleThomsonCoef, dVapJouleThomsonCoef, &
                    iIER)

        ! Atribuir as variáveis locais com resultados aos devidos campos de "structs":
        oZRhoAndDerivs%dLiquidZ = dLiquidZ
        oZRhoAndDerivs%dLiquidZWithPeneloux = dLiquidZWithPeneloux
        oZRhoAndDerivs%dDerivLiquidZWithT = dDerivLiquidZWithT
        oZRhoAndDerivs%dDerivLiquidRhoWithT = dDerivLiquidRhoWithT
        oZRhoAndDerivs%dDerivLiquidZWithP = dDerivLiquidZWithP
        oZRhoAndDerivs%dDerivLiquidRhoWithP = dDerivLiquidRhoWithP
        oZRhoAndDerivs%dVaporZWithPeneloux = dVaporZWithPeneloux
        oZRhoAndDerivs%dDerivVaporZWithT = dDerivVaporZWithT
        oZRhoAndDerivs%dDerivVaporRhoWithT = dDerivVaporRhoWithT
        oZRhoAndDerivs%dDerivVaporZWithP = dDerivVaporZWithP
        oZRhoAndDerivs%dDerivVaporRhoWithP = dDerivVaporRhoWithP
        oZRhoAndDerivs%dDerivLiquidZPenelouxWithT = dDerivLiquidZPenelouxWithT
        oZRhoAndDerivs%dDerivLiquidZPenelouxWithP = dDerivLiquidZPenelouxWithP
        oZRhoAndDerivs%dDerivVaporZPenelouxWithT = dDerivVaporZPenelouxWithT
        oZRhoAndDerivs%dDerivVaporZPenelouxWithP = dDerivVaporZPenelouxWithP

        if(iIER_BubbleP.NE.ERROR_EverythingOK) then
            ! Tratar erros na pressão de bolha igual nas demais propriedades exigidas pelo Marlim:
            iIER = iIER_BubbleP
        end if




        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        !       POSSIBILIDADE DE ESCRITA DE DADOS DE ENTRADA NA TELA (para testes)
        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                writeInputToScreen: if(.false.) then

                    ! Avisar que serão escritos os dados de entrada:
                    write(*, '(/, A)') '===========> FORTRAN COMPOSICIONAL ESCREVENDO DADOS DE ENTRADA!'



                        do i = 1, iNComp
                            write(*, '(A, I2, A, F13.7, A, I2, A, F10.4, A, I2, A, F13.7, A, I2, A, F9.7, A, I2, A, F8.3, A, I2, A, E10.4)') 'z(', i, ') = ', temp_oZ(i), '      Tc(', i, ') = ', temp_oTc(i), &
                                '      Pc(', i, ') em bar = ', temp_oPc(i)/100000.0d0, '       w(', i, ') = ', temp_oW(i), &
                                '      MW(', i, ') = ', temp_oMW(i), '      Pen(', i, ') = ', temp_oPeneloux(i)
                        end do


                        write(*, '(//, A)') 'Matriz Kij'

                        do i = 1, iNComp
                            write(*, '(20(F9.4, 3X))') (oKij_ForCalc(i, j), j = 1, iNComp)
                        end do


                        write(*, '(//, A)') 'Coeficientes para CP de gás ideal'
                        call c_f_pointer(oCpIGCoefs, temp_oCpIGCoefs, [4, iNComp])

                        do i = 1, iNComp
                            write(*, '(20(E13.5, 3X))') (temp_oCpIGCoefs(j, i), j = 1, 4)
                        end do

                    ! Avisar que o composicional acabou de escrever os dados de entrada:
                    write(*, '(/, A)') '===========> FIM DA EXIBIÇÃO DOS DADOS DE ENTRADA!'


                end if writeInputToScreen
        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        !           FIM DA ESCRITA DE DADOS DE ENTRADA NA TELA (para testes)
        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        !       POSSIBILIDADE DE ESCRITA DE RESULTADOS NA TELA (para testes)
        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

                writeOnScreen: if(.FALSE.) then

                        ! Avisar que é o composicional escrevendo:
                        write(*, '(/, A)') '===========> FORTRAN COMPOSICIONAL ESCREVENDO'

                        ! TESTE: CÁLCULO DE PROPRIEDADES OK?
                        if(iIER.EQ.ERROR_EverythingOK) then
                            write(*, '(//, A)') 'Sem erros no cálculo de propriedades!'
                        else
                            write(*, '(//, A)') 'Erro no cálculo de propriedades!'
                        end if


                        ! TESTE - PROVISÓRIO - ESCRITA DE RESULTADOS NA TELA
                        write(*, '(/, A, F13.7, /)') 'P FLASH (kgf/cm2) = ', dFlashPressure / 98066.5d0

                        if(iIER_BubbleP.NE.ERROR_EverythingOK) then
                            write(*, '(/, A, I2)') 'ERRO EM P BOLHA = ', iIER_BubbleP
                        else
                            write(*, '(/, A, F13.7, /)') 'P BOLHA (kgf/cm2) = ', dBubbleP / 98066.5d0
                        end if


                        write(*, '(/, A, F13.7, /)') 'BETA = ', dCalculatedBeta

                        write(*, '(/, A, I2, /)') 'COND TERMODINAMICA = ', iCalculatedThermodynamicCondition

                        dSumL = 0.0d0
                        dSumV = 0.0d0
                        do i = 1, iNComp
                            write(*, '(A, I2, A, F13.7, A, I2, A, F13.7, A, I2, A, F13.7)') 'x(', i, ') = ', oCalculatedLiqComposition(i), '      y(', i, ') = ', oCalculatedVapComposition(i), &
                                '      K(', i, ') = ', (oCalculatedVapComposition(i) / oCalculatedLiqComposition(i))

                            dSumL = dSumL + oCalculatedLiqComposition(i)
                            dSumV = dSumV + oCalculatedVapComposition(i)
                        end do

                        write(*, '(/, A, F13.7)') 'Soma dos x = ', dSumL
                        write(*, '(/, A, F13.7)') 'Soma dos y = ', dSumV

                        if(iIER_Flash.EQ.ERROR_EverythingOK) then
                            write(*, '(//, A)') 'Sem erros no flash!'
                        else
                            write(*, '(//, A, I2)') 'ERRO NO FLASH = ', iIER_Flash
                        end if

                        write(*, '(/, A, F13.7, /)') 'T FLASH  = ', dFlashTemperature

                        if(iIER_BubbleT.EQ.ERROR_EverythingOK) then
                            write(*, '(/, A, F13.7, /)') 'T BOLHA = ', dTBubble
                            !write(*, '(A)') 'Sem erros em T Bolha!'
                        else
                            write(*, '(/, A, I2)') 'ERRO EM T BOLHA = ', iIER_BubbleT
                        end if

                        if(iIER_DewT.EQ.ERROR_EverythingOK) then
                            write(*, '(/, A, F13.7, /)') 'T ORVALHO = ', dTOrv
                            !write(*, '(A)') 'Sem erros em T Orvalhyo!'
                        else
                            write(*, '(/, A, I15)') 'ERRO EM T ORVALHO = ', iIER_DewT
                        end if

                        ! Avisar que o composicional acabou de escrever:
                        write(*, '(/, A)') '===========> FIM DO OUTPUT DO FORTRAN COMPOSICIONAL'

                end if writeOnScreen

        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        !       FIM DO CÓDIGO DE ESCRITA DE RESULTADOS NA TELA (para testes)
        ! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    end subroutine MarlimCompositionalProps
! ===================================================================================================
! ===================================================================================================


! ========================================================================================================================
!       ROTINA AUXILIAR A SER CHAMADA PELO MARLIM TRANSIENTE PARA OBTER SOMENTE O NÚMERO DE "PSEUDOS" DE ARQUIVO EXTERNO!
! ========================================================================================================================
subroutine GetMixtureComponentNumberFromExternalFile(sExternalFileName, iExternalFileNameLength, iExternalFileSource, &
                iNComp, iIER) bind(C, name = "GetMixtureComponentNumberFromExternalFile")

    ! OBJETIVO: Obter o número de componentes da mistura (pseudocomponentes, componentes puros, etc) que será calculada
    !   "composicionalmente" pelo Marlim Transiente a partir de um arquivo externo ("ctm" do PVTSIM, etc).
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    character(kind=c_char), intent(in) :: sExternalFileName(*)      ! Nome (caminho completo) do arquivo externo.
    integer(c_int), intent(in), value :: iExternalFileNameLength    ! Tamanho do nome (caminho completo) desse arquivo externo.
    integer(c_int), value, intent(in) :: iExternalFileSource        ! Índice referente à fonte de obtenção do arquivo externo, conforme convenção.

    integer(c_int), intent(out) :: iNComp                         ! RESULTADO: Número de componentes.
    integer(c_int), intent(out) :: iIER                           ! RESULTADO: Código de erros, conforme convenção.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    type(c_ptr) :: oZ                               ! "Placeholder" sem função.
    type(c_ptr) :: oMW                              ! "Placeholder" sem função.
    type(c_ptr) :: oTc                              ! "Placeholder" sem função.
    type(c_ptr) :: oPc                              ! "Placeholder" sem função.
    type(c_ptr) :: oW                               ! "Placeholder" sem função.
    type(c_ptr) :: oParachors                       ! "Placeholder" sem função.
    type(c_ptr) :: oKij                             ! "Placeholder" sem função.
    type(c_ptr) :: oLij                             ! "Placeholder" sem função.
    type(c_ptr) :: oTIndep_Peneloux                 ! "Placeholder" sem função.
    integer(c_int) :: iThermodynamicModel           ! "Placeholder" sem função.
    type(c_ptr) :: oCpIGCoefs                       ! "Placeholder" sem função.
    type(c_ptr) :: oInputForViscosity               ! "Placeholder" sem função.

    ! ------------ PROCEDIMENTOS:
    iNComp = -10

    call ReadMixtureComponentsFromExternalFile(sExternalFileName, iExternalFileNameLength, iExternalFileSource, &
         iNComp, oZ, oMW, oTc, oPc, oW, oParachors, oKij, oLij, oTIndep_Peneloux, iThermodynamicModel, &
         oCpIGCoefs, oInputForViscosity, iIER)

end subroutine GetMixtureComponentNumberFromExternalFile


! ===================================================================================================
!       ROTINA AUXILIAR A SER CHAMADA PELO MARLIM TRANSIENTE PARA LER OS "PSEUDOS" DE ARQUIVO EXTERNO!
! ===================================================================================================
subroutine ReadMixtureComponentsFromExternalFile(sExternalFileName, iExternalFileNameLength, iExternalFileSource, &
         iNComp, oZ, oMW, oTc, oPc, oW, oParachors, oKij, oLij, oTIndep_Peneloux, iThermodynamicModel, &
         oCpIGCoefs, oInputForViscosity, iIER) bind(C, name = "ReadMixtureComponentsFromExternalFile")

    ! OBJETIVO: Ler automaticamente todas as propriedades dos componentes da mistura (pseudocomponentes, componentes puros, etc) que
    !   será calculada "composicionalmente" pelo Marlim Transiente a partir de um arquivo externo ("ctm do PVTSIM", etc).
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    character(kind=c_char), intent(in) :: sExternalFileName(*)      ! Nome (caminho completo) do arquivo externo.
    integer(c_int), intent(in), value :: iExternalFileNameLength    ! Tamanho do nome (caminho completo) desse arquivo externo.
    integer(c_int), value, intent(in) :: iExternalFileSource        ! Índice referente à fonte de obtenção do arquivo externo, conforme convenção.

    integer(c_int), intent(inout) :: iNComp                         ! RESULTADO: Número de componentes (dar negativo para ler apenas ele).
    type(c_ptr), value, intent(in) :: oZ                            ! RESULTADO: Vetor de frações molares GLOBAIS dos componentes na mistura.
    type(c_ptr), value, intent(in) :: oMW                           ! RESULTADO: Vetor de massas molares dos componentes na mistura (g/gmol).
    type(c_ptr), value, intent(in) :: oTc                           ! RESULTADO: Vetor de temperaturas críticas dos componentes na mistura.
    type(c_ptr), value, intent(in) :: oPc                           ! RESULTADO: Vetor de pressões críticas dos componentes na mistura.
    type(c_ptr), value, intent(in) :: oW                            ! RESULTADO: Vetor de fatores acêntricos dos componentes na mistura.
    type(c_ptr), value, intent(in) :: oParachors                    ! RESULTADO: Vetor de "parachors" dos componentes na mistura.
    type(c_ptr), value, intent(in) :: oKij                          ! RESULTADO: Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                          ! RESULTADO: Matriz dos parâmetros de interação "Lij".
    type(c_ptr), value, intent(in) :: oTIndep_Peneloux              ! RESULTADO: Vetor de coeficientes de Peneloux independentes de temperatura, como volume molar (SI).
    integer(c_int), intent(out) :: iThermodynamicModel              ! RESULTADO: Índice do modelo termodinâmico (EOS), conforme convenção.
    type(c_ptr), value, intent(in) :: oCpIGCoefs                    ! RESULTADO: Matriz dos coeficientes polinomiais do Cp de gás ideal dos componentes.
    type(c_ptr), value, intent(in) :: oInputForViscosity            ! RESULTADO: dados específicos para o cálculo da viscosidade.
    integer(c_int), intent(out) :: iIER                             ! RESULTADO: Código de erros, conforme convenção.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    character(:), allocatable :: sFileNameToUse
    integer :: i                                                    ! Índice de laços "do", etc.
    real(c_double), dimension(:), pointer :: temp_oZ                ! Mapeamento do argumento "oZ"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:), pointer :: temp_oParachors        ! Mapeamento do argumento "oParachors"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij".
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij".
    real(c_double), dimension(:), pointer :: temp_oTIndep_Peneloux  ! Mapeamento do argumento "oTIndep_Peneloux".
    real(c_double), dimension(:), pointer :: temp_oMW               ! Mapeamento do argumento "oMW"
    real(c_double), dimension(:,:), pointer :: temp_oCpIGCoefs      ! Mapeamento do argumento "oCpIGCoefs".
    real(c_double), dimension(4) :: oPVTSIMViscCSPTuningParam_local ! Coeficientes ajustados para o método CSP de viscosidade
    type(InputForViscosity), pointer :: temp_oInputForViscosity
    logical :: bReadingNumberOfComponents
    integer :: iPvtSimCTMFileUnit

    ! ------------ PROCEDIMENTOS:

    ! Lendo apenas o número de componentes?
    bReadingNumberOfComponents = (iNComp.LT.0)

    ! Pré-tratamento do nome do arquivo:
    allocate(character(iExternalFileNameLength) :: sFileNameToUse)
    forall(i=1:iExternalFileNameLength) sFileNameToUse(i:i) = sExternalFileName(i)

    ! Proceder de acordo com o tipo de arquivo:
    whichFileType: if(iExternalFileSource.EQ.FLUIDEXTERNALSOURCE_PVTSIM_CTM) then

        ! Arquivo "ctm" do PVTSIM!

        ! Mapeamento de vetores:
        if(iNComp.GT.0) then
            call c_f_pointer(oZ, temp_oZ, [iNComp])
            call c_f_pointer(oTc, temp_oTc, [iNComp])
            call c_f_pointer(oPc, temp_oPc, [iNComp])
            call c_f_pointer(oW, temp_oW, [iNComp])
            call c_f_pointer(oParachors, temp_oParachors, [iNComp])
            call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
            call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
            call c_f_pointer(oTIndep_Peneloux, temp_oTIndep_Peneloux, [iNComp])
            call c_f_pointer(oMW, temp_oMW, [iNComp])
            call c_f_pointer(oCpIGCoefs, temp_oCpIGCoefs, [4, iNComp])
            call c_f_pointer(oInputForViscosity, temp_oInputForViscosity)
        end if

        call ReadMixtureComponentsFromPVTSIMFileCTM(sFileNameToUse, (iNComp.LT.0), iNComp, temp_oZ, temp_oMW, temp_oTc, &
                        temp_oPc, temp_oW, temp_oParachors, temp_oKij, temp_oLij, temp_oTIndep_Peneloux, iThermodynamicModel, &
                        temp_oCpIGCoefs, oPVTSIMViscCSPTuningParam_local, iIER)

        inquire(FILE=sFileNameToUse, NUMBER=iPvtSimCTMFileUnit)
        if(iPvtSimCTMFileUnit.NE.(-1)) then
            ! Rebobinar e fechar o arquivo!
            REWIND(UNIT=iPvtSimCTMFileUnit)
            CLOSE(UNIT=iPvtSimCTMFileUnit)
        end if

        if(iIER.NE.ERROR_EverythingOK) return

            ! "Struct" de viscosidade:
        if(.NOT.bReadingNumberOfComponents) then
            temp_oInputForViscosity%oPVTSIMViscCSPTuningParam = oPVTSIMViscCSPTuningParam_local
        end if

    else whichFileType

        ! Erro!
        iIER = ERROR_InvalidExternalFileForMixtureProperties

    end if whichFileType

end subroutine ReadMixtureComponentsFromExternalFile

! ===================================================================================================
!       ROTINAS AUXILIARES PARA CÁLCULOS DE DEPOSIÇÃO DE PARAFINAS NO MARLIM 3
! ===================================================================================================

subroutine Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation(dPressure, dTemperature, iWaxComponentCount, iPressureCount, &
                iTemperatureCount, oPressurePoints, oCloudPointTemperatures, oStructurePressures, oStructureTemperatures, &
                oStructureDensities, oStructureLiqMW, oStructureCPWax, oStructureThermCond, oStructureWaxConcs, &
                oMolecularWeightsOfWaxComponents, oLiquidDensitiesOfWaxComponents, iIER, iWaxComponentCount_output, &
                oMolecularWeightsOfWaxComponents_output, oLiquidDensitiesOfWaxComponents_output, dCloudPointT_output, &
                dInterpolatedCPWax_output, dInterpolatedThermCond_output, dInterpolatedDens_output, dInterpolatedLiqMW_output, &
                oInterpolatedWaxConcsTDeriv_output, oInterpolatedWaxConcs_output) bind(C, name = "Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation")

    ! OBJETIVO: Obter, para dadas condições locais de escoamento determinadas no simulador MARLIM 3, os parâmetros necessários
    !       para cálculos de deposição de parafinas adaptados de arquivos padrão "wax PVTSIM".
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dPressure           ! Pressão na qual deseja-se fazer os cálculos (kgf/cm2).
    real(c_double), value, intent(in) :: dTemperature        ! Temperatura na qual deseja-se fazer os cálculos (oC).
    integer(c_int), value, intent(in) :: iWaxComponentCount  ! Variável obtida do arquivo "wax" do PVTSIM
    integer(c_int), value, intent(in) :: iPressureCount      ! Variável obtida do arquivo "wax" do PVTSIM
    integer(c_int), value, intent(in) :: iTemperatureCount   ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oPressurePoints        ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oCloudPointTemperatures   ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructurePressures    ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructureTemperatures ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructureDensities    ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructureLiqMW        ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructureCPWax        ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructureThermCond    ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oStructureWaxConcs     ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oMolecularWeightsOfWaxComponents  ! Variável obtida do arquivo "wax" do PVTSIM
    type(c_ptr), value, intent(in) :: oLiquidDensitiesOfWaxComponents   ! Variável obtida do arquivo "wax" do PVTSIM

    integer(c_int), intent(out) :: iIER                         ! Código de erros "geral" desta rotina, conforme convencionado.
    integer(c_int), intent(out) :: iWaxComponentCount_output
    type(c_ptr), value, intent(in) :: oMolecularWeightsOfWaxComponents_output
    type(c_ptr), value, intent(in) :: oLiquidDensitiesOfWaxComponents_output
    real(c_double), intent(out) :: dCloudPointT_output
    real(c_double), intent(out) :: dInterpolatedCPWax_output
    real(c_double), intent(out) :: dInterpolatedThermCond_output
    real(c_double), intent(out) :: dInterpolatedDens_output
    real(c_double), intent(out) :: dInterpolatedLiqMW_output
    type(c_ptr), value, intent(in) :: oInterpolatedWaxConcsTDeriv_output
    type(c_ptr), value, intent(in) :: oInterpolatedWaxConcs_output

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double) :: dPressure_Conv                                ! Pressão convertida para as unidades do arquivo "wax" (Pa)
    real(c_double) :: dTemperature_Conv                             ! Temperatura convertida para as unidades do arquivo "wax" (C).
    real(c_double), dimension(:), pointer :: temp_oPressurePoints   ! Mapeamento do argumento "oPressurePoints".
    real(c_double), dimension(:), pointer :: temp_oCloudPointTemperatures   ! Mapeamento do argumento "oCloudPointTemperatures"
    real(c_double), dimension(:), pointer :: temp_oStructurePressures       ! Mapeamento do argumento "oStructurePressures".
    real(c_double), dimension(:,:), pointer :: temp_oStructureTemperatures  ! Mapeamento do argumento "oStructureTemperatures".
    real(c_double), dimension(:,:), pointer :: temp_oStructureDensities     ! Mapeamento do argumento "oStructureDensities"
    real(c_double), dimension(:,:), pointer :: temp_oStructureLiqMW         ! Mapeamento do argumento "oStructureLiqMW"
    real(c_double), dimension(:,:), pointer :: temp_oStructureCPWax         ! Mapeamento do argumento "oStructureCPWax"
    real(c_double), dimension(:,:), pointer :: temp_oStructureThermCond     ! Mapeamento do argumento "oStructureThermCond"
    real(c_double), dimension(:,:,:), pointer :: temp_oStructureWaxConcs    ! Mapeamento do argumento "oStructureWaxConcs"
    real(c_double), dimension(:), pointer :: temp_oMolecularWeightsOfWaxComponents  ! Mapeamento do argumento "oMolecularWeightsOfWaxComponents"
    real(c_double), dimension(:), pointer :: temp_oLiquidDensitiesOfWaxComponents   ! Mapeamento do argumento "oLiquidDensitiesOfWaxComponents"
    real(c_double), dimension(:), pointer :: temp_oMolecularWeightsOfWaxComponents_output   ! Mapeamento do argumento "oMolecularWeightsOfWaxComponents_output"
    real(c_double), dimension(:), pointer :: temp_oLiquidDensitiesOfWaxComponents_output    ! Mapeamento do argumento "oLiquidDensitiesOfWaxComponents_output"
    real(c_double), dimension(:), pointer :: temp_oInterpolatedWaxConcsTDeriv_output        ! Mapeamento do argumento "oInterpolatedWaxConcsTDeriv_output"
    real(c_double), dimension(:), pointer :: temp_oInterpolatedWaxConcs_output      ! Mapeamento do argumento "oInterpolatedWaxConcs_output".

    ! ------------ PROCEDIMENTOS:

    ! MAPEANDO VETORES E MATRIZES:
    call c_f_pointer(oPressurePoints, temp_oPressurePoints, [iPressureCount])
    call c_f_pointer(oCloudPointTemperatures, temp_oCloudPointTemperatures, [iPressureCount])
    call c_f_pointer(oStructurePressures, temp_oStructurePressures, [iPressureCount])
    call c_f_pointer(oStructureTemperatures, temp_oStructureTemperatures, [iPressureCount, iTemperatureCount])
    call c_f_pointer(oStructureDensities, temp_oStructureDensities, [iPressureCount, iTemperatureCount])
    call c_f_pointer(oStructureLiqMW, temp_oStructureLiqMW, [iPressureCount, iTemperatureCount])
    call c_f_pointer(oStructureCPWax, temp_oStructureCPWax, [iPressureCount, iTemperatureCount])
    call c_f_pointer(oStructureThermCond, temp_oStructureThermCond, [iPressureCount, iTemperatureCount])
    call c_f_pointer(oStructureWaxConcs, temp_oStructureWaxConcs, [iPressureCount, iTemperatureCount, iWaxComponentCount])
    call c_f_pointer(oMolecularWeightsOfWaxComponents, temp_oMolecularWeightsOfWaxComponents, [iWaxComponentCount])
    call c_f_pointer(oLiquidDensitiesOfWaxComponents, temp_oLiquidDensitiesOfWaxComponents, [iWaxComponentCount])
    call c_f_pointer(oMolecularWeightsOfWaxComponents_output, temp_oMolecularWeightsOfWaxComponents_output, [iWaxComponentCount])
    call c_f_pointer(oLiquidDensitiesOfWaxComponents_output, temp_oLiquidDensitiesOfWaxComponents_output, [iWaxComponentCount])
    call c_f_pointer(oInterpolatedWaxConcsTDeriv_output, temp_oInterpolatedWaxConcsTDeriv_output, [iWaxComponentCount])
    call c_f_pointer(oInterpolatedWaxConcs_output, temp_oInterpolatedWaxConcs_output, [iWaxComponentCount])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! Chamada à rotina interna:
    call GetPvtSimWAXFileInfoFor1DFlowSimulation(dPressure_Conv, dTemperature_Conv, iWaxComponentCount, iPressureCount, iTemperatureCount, &
                    temp_oPressurePoints, temp_oCloudPointTemperatures, temp_oStructurePressures, temp_oStructureTemperatures, &
                    temp_oStructureDensities, temp_oStructureLiqMW, temp_oStructureCPWax, temp_oStructureThermCond, &
                    temp_oStructureWaxConcs, temp_oMolecularWeightsOfWaxComponents, temp_oLiquidDensitiesOfWaxComponents, iIER, &
                    iWaxComponentCount_output, temp_oMolecularWeightsOfWaxComponents_output, temp_oLiquidDensitiesOfWaxComponents_output, &
                    dCloudPointT_output, dInterpolatedCPWax_output, dInterpolatedThermCond_output, dInterpolatedDens_output, &
                    dInterpolatedLiqMW_output, temp_oInterpolatedWaxConcsTDeriv_output, temp_oInterpolatedWaxConcs_output)

    if(iIER.NE.ERROR_EverythingOK) return

end subroutine Marlim_GetPvtSimWAXFileInfoFor1DFlowSimulation

    ! =============================================================
    ! =============================================================
subroutine GetWaxParameterExternalFileDimensions(sExternalFileName, iExternalFileNameLength, iExternalFileSource, iIER, &
                iWaxComponentCount, iPressureCount, iTemperatureCount) bind(C, name = "GetWaxParameterExternalFileDimensions")

    ! OBJETIVO: Obter, de arquivo externo contendo parâmetros para cálculo de deposição de parafinas, contagens necessárias
    !           para viabilizar a leitura automática deste arquivo (contagens de componentes, pontos de pressão, etc).
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    character(kind=c_char), intent(in) :: sExternalFileName(*)      ! Nome (caminho completo) do arquivo externo.
    integer(c_int), intent(in), value :: iExternalFileNameLength    ! Tamanho do nome (caminho completo) desse arquivo externo.
    integer(c_int), value, intent(in) :: iExternalFileSource        ! Índice referente à fonte de obtenção do arquivo externo, conforme convenção.

    integer(c_int), intent(out) :: iIER                             ! RESULTADO: Código de erros, conforme convenção.
    integer(c_int), intent(out) :: iWaxComponentCount
    integer(c_int), intent(out) :: iPressureCount
    integer(c_int), intent(out) :: iTemperatureCount

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    character(:), allocatable :: sFileNameToUse
    integer :: i
    integer :: iPvtSimWaxFileUnit

    ! ------------ PROCEDIMENTOS:

    ! Pré-tratamento do nome do arquivo:
    allocate(character(iExternalFileNameLength) :: sFileNameToUse)
    forall(i=1:iExternalFileNameLength) sFileNameToUse(i:i) = sExternalFileName(i)

    ! Proceder de acordo com o tipo de arquivo:
    whichFileType: if(iExternalFileSource.EQ.WAXEXTERNALSOURCE_PVTSIM_WAX) then

        ! Arquivo "*.wax" no padrão do PVTSIM!
        call GetPVTSIMWaxFileDimensions(sFileNameToUse, iIER, iWaxComponentCount, iPressureCount, iTemperatureCount)

        inquire(FILE=sFileNameToUse, NUMBER=iPvtSimWaxFileUnit)
        if(iPvtSimWaxFileUnit.NE.(-1)) then
            ! Rebobinar e fechar o arquivo!
            REWIND(UNIT=iPvtSimWaxFileUnit)
            CLOSE(UNIT=iPvtSimWaxFileUnit)
        end if

        if(iIER.NE.ERROR_EverythingOK) return

    else whichFileType
        ! Erro!
        iIER = ERROR_InvalidExternalFileForWaxParameters
        return
    end if whichFileType

end subroutine GetWaxParameterExternalFileDimensions

    ! =============================================================
    ! =============================================================
subroutine ReadWaxCalculationParametersFromExternalFile(sExternalFileName, iExternalFileNameLength, iExternalFileSource, &
                iWaxComponentCount, iPressureCount, iTemperatureCount, iIER, oPressurePoints, oCloudPointTemperatures, &
                oStructurePressures, oStructureTemperatures, oStructureDensities, oStructureLiqMW, oStructureCPWax, &
                oStructureThermCond, oStructureWaxConcs, oMolecularWeightsOfWaxComponents, &
                oLiquidDensitiesOfWaxComponents, &
                dOilWaxViscosityMultiplierD, dOilWaxViscosityMultiplierE, dOilWaxViscosityMultiplierF) bind(C, name = "ReadWaxCalculationParametersFromExternalFile")

    ! OBJETIVO: Obter automaticamente, a partir de arquivos externos de formato padronizado, os parâmetros necessários
    !   para cálculos de deposição de parafina no MARLIM 3.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    character(kind=c_char), intent(in) :: sExternalFileName(*)      ! Nome (caminho completo) do arquivo externo.
    integer(c_int), intent(in), value :: iExternalFileNameLength    ! Tamanho do nome (caminho completo) desse arquivo externo.
    integer(c_int), value, intent(in) :: iExternalFileSource        ! Índice referente à fonte de obtenção do arquivo externo, conforme convenção.
    integer(c_int), value, intent(in) :: iWaxComponentCount  ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    integer(c_int), value, intent(in) :: iPressureCount      ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    integer(c_int), value, intent(in) :: iTemperatureCount   ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)

    integer(c_int), intent(out) :: iIER                             ! RESULTADO: Código de erros, conforme convenção.
    type(c_ptr), value, intent(in) :: oPressurePoints               ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oCloudPointTemperatures   ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructurePressures       ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructureTemperatures    ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructureDensities       ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructureLiqMW           ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructureCPWax           ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructureThermCond       ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oStructureWaxConcs        ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oMolecularWeightsOfWaxComponents  ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    type(c_ptr), value, intent(in) :: oLiquidDensitiesOfWaxComponents   ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    real(c_double), intent(out) :: dOilWaxViscosityMultiplierD      ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    real(c_double), intent(out) :: dOilWaxViscosityMultiplierE      ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)
    real(c_double), intent(out) :: dOilWaxViscosityMultiplierF      ! Variável obtida do arquivo "wax" do PVTSIM (ou arquivo externo de padrão equivalente)

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    character(:), allocatable :: sFileNameToUse
    integer :: i
    integer :: iPvtSimWaxFileUnit
    real(c_double), dimension(:), pointer :: temp_oPressurePoints   ! Mapeamento do argumento "oPressurePoints".
    real(c_double), dimension(:), pointer :: temp_oCloudPointTemperatures   ! Mapeamento do argumento "oCloudPointTemperatures"
    real(c_double), dimension(:), pointer :: temp_oStructurePressures       ! Mapeamento do argumento "oStructurePressures".
    real(c_double), dimension(:,:), pointer :: temp_oStructureTemperatures  ! Mapeamento do argumento "oStructureTemperatures".
    real(c_double), dimension(:,:), pointer :: temp_oStructureDensities     ! Mapeamento do argumento "oStructureDensities"
    real(c_double), dimension(:,:), pointer :: temp_oStructureLiqMW         ! Mapeamento do argumento "oStructureLiqMW"
    real(c_double), dimension(:,:), pointer :: temp_oStructureCPWax         ! Mapeamento do argumento "oStructureCPWax"
    real(c_double), dimension(:,:), pointer :: temp_oStructureThermCond     ! Mapeamento do argumento "oStructureThermCond"
    real(c_double), dimension(:,:,:), pointer :: temp_oStructureWaxConcs    ! Mapeamento do argumento "oStructureWaxConcs"
    real(c_double), dimension(:), pointer :: temp_oMolecularWeightsOfWaxComponents  ! Mapeamento do argumento "oMolecularWeightsOfWaxComponents"
    real(c_double), dimension(:), pointer :: temp_oLiquidDensitiesOfWaxComponents   ! Mapeamento do argumento "oLiquidDensitiesOfWaxComponents"

    ! ------------ PROCEDIMENTOS:

    ! Pré-tratamento do nome do arquivo:
    allocate(character(iExternalFileNameLength) :: sFileNameToUse)
    forall(i=1:iExternalFileNameLength) sFileNameToUse(i:i) = sExternalFileName(i)

    ! Proceder de acordo com o tipo de arquivo:
    whichFileType: if(iExternalFileSource.EQ.WAXEXTERNALSOURCE_PVTSIM_WAX) then

        ! Arquivo "*.wax" no padrão do PVTSIM!

        ! Mapeamento de vetores:
        call c_f_pointer(oPressurePoints, temp_oPressurePoints, [iPressureCount])
        call c_f_pointer(oCloudPointTemperatures, temp_oCloudPointTemperatures, [iPressureCount])
        call c_f_pointer(oStructurePressures, temp_oStructurePressures, [iPressureCount])
        call c_f_pointer(oStructureTemperatures, temp_oStructureTemperatures, [iPressureCount, iTemperatureCount])
        call c_f_pointer(oStructureDensities, temp_oStructureDensities, [iPressureCount, iTemperatureCount])
        call c_f_pointer(oStructureLiqMW, temp_oStructureLiqMW, [iPressureCount, iTemperatureCount])
        call c_f_pointer(oStructureCPWax, temp_oStructureCPWax, [iPressureCount, iTemperatureCount])
        call c_f_pointer(oStructureThermCond, temp_oStructureThermCond, [iPressureCount, iTemperatureCount])
        call c_f_pointer(oStructureWaxConcs, temp_oStructureWaxConcs, [iPressureCount, iTemperatureCount, iWaxComponentCount])
        call c_f_pointer(oMolecularWeightsOfWaxComponents, temp_oMolecularWeightsOfWaxComponents, [iWaxComponentCount])
        call c_f_pointer(oLiquidDensitiesOfWaxComponents, temp_oLiquidDensitiesOfWaxComponents, [iWaxComponentCount])

        ! Chamar a rotina
        call ReadWaxCalculationParametersFromPVTSIMFileWax(sFileNameToUse, iIER, temp_oPressurePoints, temp_oCloudPointTemperatures, &
                temp_oStructurePressures, temp_oStructureTemperatures, temp_oStructureDensities, temp_oStructureLiqMW, temp_oStructureCPWax, &
                temp_oStructureThermCond, temp_oStructureWaxConcs, temp_oMolecularWeightsOfWaxComponents, &
                temp_oLiquidDensitiesOfWaxComponents, &
                dOilWaxViscosityMultiplierD, dOilWaxViscosityMultiplierE, dOilWaxViscosityMultiplierF)

        inquire(FILE=sFileNameToUse, NUMBER=iPvtSimWaxFileUnit)
        if(iPvtSimWaxFileUnit.NE.(-1)) then
            ! Rebobinar e fechar o arquivo!
            REWIND(UNIT=iPvtSimWaxFileUnit)
            CLOSE(UNIT=iPvtSimWaxFileUnit)
        end if

        if(iIER.NE.ERROR_EverythingOK) return

    else whichFileType

        ! Erro!
        iIER = ERROR_InvalidExternalFileForWaxParameters

    end if whichFileType

end subroutine ReadWaxCalculationParametersFromExternalFile

! ===========================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR A CONDIÇÃO TERMODINÂMICA DA MISTURA FLUIDA
! ===========================================================================================================
subroutine Marlim_CalculateMixtureThermodynamicCondition(dFlashPressure, dFlashTemperature, iNComp, oMW, oZ, oTc, oPc, oW, &
                    oPeneloux, oKij, oLij, iLiqPhaseModel, iVapPhaseModel, &
                    dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                    dCalculatedBubbleT, iIER_BubbleT, dCalculatedDewT, &
                    iIER_DewT, dCalculatedBeta, oCalculatedLiqComposition, oCalculatedVapComposition, iIER_Flash, dCalculatedBubbleP, &
                    iIER_BubbleP, iCalculatedThermodynamicCondition, dLiquidPhaseMW, dVaporPhaseMW, dVaporMassFraction, iIER) bind(C, name = "Marlim_CalculateMixtureThermodynamicCondition")

    ! OBJETIVO: Permitir que o simulador Marlim Transiente determine "composicionalmente" a condição termodinâmica do fluido que escoa.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dFlashPressure                  ! Pressão na qual deseja-se calcular o "flash" (kgf/cm2).
    real(c_double), value, intent(in) :: dFlashTemperature               ! Temperatura na qual deseja-se calcular o "flash" (oC).
    integer(c_int), value, intent(in) :: iNComp                          ! Número de componentes.
    type(c_ptr), value, intent(in) :: oMW                                ! Vetor de massas molares (indexado por componente).
    type(c_ptr), value, intent(in) :: oZ                                 ! Vetor composição global (indexado por componente).
    type(c_ptr), value, intent(in) :: oTc                                ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                                ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                                 ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oPeneloux                          ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    type(c_ptr), value, intent(in) :: oKij                               ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                               ! Matriz dos parâmetros "lij".
    integer(c_int), value, intent(in) :: iLiqPhaseModel                  ! Modelo da fase líquida, conforme convenção.
    integer(c_int), value, intent(in) :: iVapPhaseModel                  ! Modelo da fase vapor, conforme convenção.
    real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash" (dar negativo caso indisponível).
    type(c_ptr), value, intent(in) :: oGivenInitialLiqComposition        ! Estimativa inicial de composição da fase líquida para o "flash".
    type(c_ptr), value, intent(in) :: oGivenInitialVapComposition        ! Estimativa inicial de composição da fase vapor para o "flash".

    real(c_double), intent(out) :: dCalculatedBubbleT                    ! Valor calculado da temperatura de bolha.
    integer(c_int), intent(out) :: iIER_BubbleT                          ! Código de erros na determinação da temperatura de bolha, conforme convencionado.
    real(c_double), intent(out) :: dCalculatedDewT                       ! Valor calculado da temperatura de orvalho.
    integer(c_int), intent(out) :: iIER_DewT                             ! Código de erros na determinação da temperatura de orvalho, conforme convencionado.
    real(c_double), intent(out) :: dCalculatedBeta                       ! Fração molar vaporizada determinada.
    type(c_ptr), value, intent(in) :: oCalculatedLiqComposition          ! Composição molar determinada da fase líquida.
    type(c_ptr), value, intent(in) :: oCalculatedVapComposition          ! Composição molar determinada da fase vapor.
    integer(c_int), intent(out) :: iIER_Flash                            ! Código de erros no cálculo do "flash", conforme convencionado.
    real(c_double), intent(out) :: dCalculatedBubbleP                    ! Pressão de bolha calculada.
    integer(c_int), intent(out) :: iIER_BubbleP                          ! Código de erros no cálculo da pressão de bolha, conforme convencionado.
    integer(c_int), intent(out) :: iCalculatedThermodynamicCondition     ! Código da condição termodinâmica determinada, conforme convencionado.
    real(c_double), intent(out) :: dLiquidPhaseMW                        ! Massa molar da fase líquida (g/gmol).
    real(c_double), intent(out) :: dVaporPhaseMW                         ! Massa molar da fase vapor (g/gmol).
    real(c_double), intent(out) :: dVaporMassFraction                                 ! Fração mássica vaporizada.
    integer(c_int), intent(out) :: iIER                                  ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double) :: dFlashPressure_Conv                                ! Pressão convertida para o SI
    real(c_double) :: dFlashTemperature_Conv                             ! Temperatura convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oMW                    ! Mapeamento do argumento "oMW"
    real(c_double), dimension(:), pointer :: temp_oZ                     ! Mapeamento do argumento "oZ"
    real(c_double), dimension(:), pointer :: temp_oTc                    ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc                    ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                     ! Mapeamento do argumento "oW"
    real(c_double), dimension(:), pointer :: temp_oPeneloux              ! Mapeamento do argumento "oPeneloux"
    real(c_double), dimension(:,:), pointer :: temp_oKij                 ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc             ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij                 ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc             ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:), pointer :: temp_oCalculatedLiqComposition      ! Mapeamento do argumento "oCalculatedLiqComposition"
    real(c_double), dimension(:), pointer :: temp_oCalculatedVapComposition      ! Mapeamento do argumento "oCalculatedVapComposition"
    real(c_double), dimension(:), pointer :: temp_oGivenInitialLiqComposition    ! Mapeamento do argumento "oGivenInitialLiqComposition"
    real(c_double), dimension(:), pointer :: temp_oGivenInitialVapComposition    ! Mapeamento do argumento "oGivenInitialVapComposition"
    real(c_double) :: dMixtureMW                                         ! Massa molar da mistura (g/gmol).


    logical, dimension(iNComp) :: isZeroedComp             ! "True" se o componente "i" estava zerado originalmente
    integer(c_int) :: iNComp_Fixed                         ! Número de componentes não-nulos.
    real(c_double), allocatable, dimension(:) :: oZ_Fixed  ! Frações molares globais dos componentes não-nulos.
    integer :: iMemoryAllocationError
    real(c_double), allocatable, dimension(:) :: oMW_Fixed
    real(c_double), allocatable, dimension(:) :: oTc_Fixed
    real(c_double), allocatable, dimension(:) :: oPc_Fixed
    real(c_double), allocatable, dimension(:) :: oW_Fixed
    real(c_double), allocatable, dimension(:) :: oPeneloux_Fixed
    real(c_double), allocatable, dimension(:) :: oGivenInitialLiqComp_Fixed
    real(c_double), allocatable, dimension(:) :: oGivenInitialVapComp_Fixed
    real(c_double), allocatable, dimension(:,:) :: oKij_Fixed
    real(c_double), allocatable, dimension(:,:) :: oLij_Fixed
    real(c_double), allocatable, dimension(:) :: oCalculatedLiqComp_Fixed
    real(c_double), allocatable, dimension(:) :: oCalculatedVapComp_Fixed

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! Mapeando vetores e matrizes:
    call c_f_pointer(oMW, temp_oMW, [iNComp])
    call c_f_pointer(oZ, temp_oZ, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oCalculatedLiqComposition, temp_oCalculatedLiqComposition, [iNComp])
    call c_f_pointer(oCalculatedVapComposition, temp_oCalculatedVapComposition, [iNComp])
    call c_f_pointer(oGivenInitialLiqComposition, temp_oGivenInitialLiqComposition, [iNComp])
    call c_f_pointer(oGivenInitialVapComposition, temp_oGivenInitialVapComposition, [iNComp])

    ! Conversão de unidades, quando necessário:
    dFlashPressure_Conv = dFlashPressure * 98066.52d0
    dFlashTemperature_Conv = dFlashTemperature + 273.15d0

    ! Transposição de matrizes, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! Inicializando códigos de erros:
    iIER_BubbleT = ERROR_EverythingOK
    iIER_DewT = ERROR_EverythingOK
    iIER_Flash = ERROR_EverythingOK
    iIER_BubbleP = ERROR_EverythingOK
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL:
    deactivate: if(.false.) then
        ! Chamada à rotina obsoleta:
        call CalculateMixtureThermodynamicCondition(dFlashPressure_Conv, dFlashTemperature_Conv, iNComp, temp_oZ, temp_oTc, temp_oPc, temp_oW, &
                    temp_oPeneloux, oKij_ForCalc, oLij_ForCalc, iLiqPhaseModel, iVapPhaseModel, &
                    (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, &
                    dCalculatedBubbleT, iIER_BubbleT, &
                    dCalculatedDewT, iIER_DewT, dCalculatedBeta, temp_oCalculatedLiqComposition, temp_oCalculatedVapComposition, iIER_Flash, &
                    dCalculatedBubbleP, iIER_BubbleP, iCalculatedThermodynamicCondition, iIER)

    else deactivate

        ! ====================================================
        !   7/2/2022 - TRATAMENTO DE COMPONENTES ZERADOS
        if(.true.) then

            ! Contar:
            call CountZeroedComponents(iNComp, temp_oZ, isZeroedComp, iNComp_Fixed)

            ! Alocar memória:
            allocate(oMW_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oZ_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oTc_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oPc_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oW_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oPeneloux_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oKij_Fixed(iNComp_Fixed, iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oLij_Fixed(iNComp_Fixed, iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oGivenInitialLiqComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oGivenInitialVapComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oCalculatedLiqComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)
            allocate(oCalculatedVapComp_Fixed(iNComp_Fixed), STAT=iMemoryAllocationError)

            ! Pegar os valores:
            call RemoveZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, temp_oZ, temp_oMW, temp_oTc, temp_oPc, temp_oW, temp_oPeneloux, temp_oGivenInitialLiqComposition, &
                temp_oGivenInitialVapComposition, oKij_ForCalc, oLij_ForCalc, (dGivenInitialBeta.GE.(0.0d0)), oZ_Fixed, oMW_Fixed, oTc_Fixed, oPc_Fixed, oW_Fixed, oPeneloux_Fixed, &
                oKij_Fixed, oLij_Fixed, oGivenInitialLiqComp_Fixed, oGivenInitialVapComp_Fixed)

            ! Calcular a condição termodinâmica:
            deactivateV2: if(.false.) then
                call CalculateMixtureThermodynamicCondition_V2(dFlashPressure_Conv, dFlashTemperature_Conv, iNComp_Fixed, oMW_Fixed, oZ_Fixed, oTc_Fixed, oPc_Fixed, oW_Fixed, &
                        oPeneloux_Fixed, oKij_Fixed, oLij_Fixed, iLiqPhaseModel, iVapPhaseModel, (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, &
                        oGivenInitialLiqComp_Fixed, oGivenInitialVapComp_Fixed, dCalculatedBeta, oCalculatedLiqComp_Fixed, &
                        oCalculatedVapComp_Fixed, iCalculatedThermodynamicCondition, iIER_Flash, iIER)
            else deactivateV2
                call CalculateMixtureThermodynamicCondition_V3(dFlashPressure_Conv, dFlashTemperature_Conv, iNComp_Fixed, oMW_Fixed, &
                    oZ_Fixed, oTc_Fixed, oPc_Fixed, oW_Fixed, oKij_Fixed, oLij_Fixed, oPeneloux_Fixed, iLiqPhaseModel, iVapPhaseModel, &
                    (dGivenInitialBeta.GE.(0.0d0)), oGivenInitialLiqComp_Fixed, oGivenInitialVapComp_Fixed, &
                    dCalculatedBeta, oCalculatedLiqComp_Fixed, oCalculatedVapComp_Fixed, iCalculatedThermodynamicCondition, iIER_Flash, iIER)
            end if deactivateV2

            ! "Recompor" as composições calculadas:
            call ReInsertZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, oCalculatedLiqComp_Fixed, temp_oCalculatedLiqComposition)
            call ReInsertZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, oCalculatedVapComp_Fixed, temp_oCalculatedVapComposition)

        else
        !   7/2/2022 - FIM DO TRATAMENTO DE COMPONENTES ZERADOS
        ! ====================================================


            return ! Por segurança, para não chamar mais a V2 agora que já está disponível a V3 (10-JUL-2023)

            call CalculateMixtureThermodynamicCondition_V2(dFlashPressure_Conv, dFlashTemperature_Conv, iNComp, temp_oMW, temp_oZ, temp_oTc, temp_oPc, temp_oW, &
                        temp_oPeneloux, oKij_ForCalc, oLij_ForCalc, iLiqPhaseModel, iVapPhaseModel, (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, &
                        temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, dCalculatedBeta, temp_oCalculatedLiqComposition, &
                        temp_oCalculatedVapComposition, iCalculatedThermodynamicCondition, iIER_Flash, iIER)


        end if  ! INSERIDO EM 7/2/2022 PARA TRATAMENTO DE COMPONENTES ZERADOS

        dCalculatedBubbleT = -1000.0d0
        iIER_BubbleT = -1
        dCalculatedDewT = -1000.0d0
        iIER_DewT = -1
        dCalculatedBubbleP = -1000.0d0
        iIER_BubbleP = -1

    end if deactivate

    ! Calcular resultados que só dependem do "flash":
    if(iIER.NE.ERROR_EverythingOK) return

        ! Massas molares das duas fases em equilíbrio:
    call CalculateMolecularWeightsAndVaporMassFraction(iCalculatedThermodynamicCondition, iNComp, temp_oMW, temp_oZ, temp_oCalculatedLiqComposition, &
                temp_oCalculatedVapComposition, dCalculatedBeta, dLiquidPhaseMW, dVaporPhaseMW, dMixtureMW, dVaporMassFraction)

end subroutine Marlim_CalculateMixtureThermodynamicCondition


! ===========================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR A TENSÃO SUPERFICIAL DA MISTURA FLUIDA
! ===========================================================================================================
subroutine Marlim_CalculateMixtureSurfaceTension(dPressure, dTemperature, iNComp, oTc, oPc, oW, oPeneloux, oKij, oLij, oParachors, dLiquidPhaseMW, dVaporPhaseMW, &
                oLiqPhaseComposition, oVapPhaseComposition, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, &
                dSurfaceTension, iIER) bind(C, name = "Marlim_CalculateMixtureSurfaceTension")

    ! OBJETIVO: Permitir que o Marlim Transiente calcule "composicionalmente" a tensão superficial da mistura em escoamento.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dPressure           ! Pressão na qual deseja-se calcular a tensão superficial (kgf/cm2).
    real(c_double), value, intent(in) :: dTemperature        ! Temperatura na qual deseja-se calcular a tensão superficial (oC).
    integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    type(c_ptr), value, intent(in) :: oParachors             ! Vetor de "parachors" para cada componente.
    real(c_double), value, intent(in) :: dLiquidPhaseMW      ! Massa molar da fase líquida (g/gmol).
    real(c_double), value, intent(in) :: dVaporPhaseMW       ! Massa molar da fase vapor (g/gmol).
    type(c_ptr), value, intent(in) :: oLiqPhaseComposition   ! Vetor composição MOLAR do líquido (indexado por componente).
    type(c_ptr), value, intent(in) :: oVapPhaseComposition   ! Vetor composição MOLAR do vapor (indexado por componente).
    integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod  ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
    integer(c_int), value, intent(in) :: iVapDensityCalculationMethod  ! Índice do método de cálculo da massa específica do vapor, conforme convenção.

    real(c_double), intent(out) :: dSurfaceTension           ! Tensão superficial calculada.
    integer(c_int), intent(out) :: iIER                      ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double), dimension(:), pointer :: temp_oParachors           ! Mapeamento do argumento "oParachors"
    real(c_double), dimension(:), pointer :: temp_oLiqPhaseComposition ! Mapeamento do argumento "oLiqPhaseComposition"
    real(c_double), dimension(:), pointer :: temp_oVapPhaseComposition ! Mapeamento do argumento "oVapPhaseComposition"
    real(c_double) :: dPressure_Conv                                   ! Pressão convertida para o SI
    real(c_double) :: dTemperature_Conv                                ! Temperatura convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oTc                  ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc                  ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                   ! Mapeamento do argumento "oW"
    real(c_double), dimension(:,:), pointer :: temp_oKij               ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc           ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij               ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc           ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:), pointer :: temp_oPeneloux            ! Mapeamento do argumento "oPeneloux"
    real(c_double) :: dLiqDensity                                      ! Massa específica da fase líquida.
    real(c_double) :: dVapDensity                                      ! Massa específica da fase vapor.

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! MAPEANDO VETORES E MATRIZES:
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oParachors, temp_oParachors, [iNComp])
    call c_f_pointer(oLiqPhaseComposition, temp_oLiqPhaseComposition, [iNComp])
    call c_f_pointer(oVapPhaseComposition, temp_oVapPhaseComposition, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature + 273.15d0

    ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! Calcular a massa específica da fase líquida:
    call CalculatePhaseDensity(iNComp, PHASE_Liquid, temp_oLiqPhaseComposition, dPressure_Conv, dTemperature_Conv, &
              iLiqDensityCalculationMethod, temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, &
              temp_oPeneloux, dLiquidPhaseMW, dLiqDensity, iIER)

    if(iIER.NE.ERROR_EverythingOK) return

    ! Calcular a massa específica da fase vapor:
    call CalculatePhaseDensity(iNComp, PHASE_Vapor, temp_oVapPhaseComposition, dPressure_Conv, dTemperature_Conv, &
              iVapDensityCalculationMethod, temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, &
                      temp_oPeneloux, dVaporPhaseMW, dVapDensity, iIER)

    if(iIER.NE.ERROR_EverythingOK) return

    ! CHAMADA PRINCIPAL:
    call CalculateMixtureSurfaceTensionFromComponentParachors(iNComp, temp_oParachors, dLiqDensity, dVapDensity, dLiquidPhaseMW, &
                 dVaporPhaseMW, temp_oLiqPhaseComposition, temp_oVapPhaseComposition, dSurfaceTension)

end subroutine Marlim_CalculateMixtureSurfaceTension


! ===========================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR ENTALPIA OU CP DAS FASES EM ESCOAMENTO
! ===========================================================================================================
subroutine Marlim_CalculatePhaseCpAndEnthalpy(dPressure, dTemperature, iPhaseID, oPhaseComposition, iNComp, oTc, oPc, oW, oPeneloux, oKij, oLij, &
                iThermodynamicModel, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, dPhaseMW, dMassEnthalpy, dMassCp, iIER) bind(C, name = "Marlim_CalculatePhaseCpAndEnthalpy")

    ! OBJETIVO: Permitir que o Marlim Transiente calcule "composicionalmente" a entalpia ou o Cp de qualquer uma das fases em escoamento.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dPressure           ! Pressão na qual deseja-se fazer os cálculos (kgf/cm2).
    real(c_double), value, intent(in) :: dTemperature        ! Temperatura na qual deseja-se fazer os cálculos (oC).
    integer(c_int), value, intent(in) :: iPhaseID            ! Identificador da fase cujas propriedades se deseja calcular, conforme convenção.
    type(c_ptr), value, intent(in) :: oPhaseComposition      ! Vetor de frações molares da fase (indexadas por componente).
    integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    integer(c_int), value, intent(in) :: iThermodynamicModel ! Indicador do modelo termodinâmico selecionado para o cálculo, conforme convencionado.
    integer(c_int), value, intent(in) :: iIGCoefsSource      ! Índice da fonte dos coeficientes polinomiais de gás ideal, conforme convenção.
    type(c_ptr), value, intent(in) :: oCpIGCoefs             ! Matriz (se existir) dos coeficientes polinomiais do Cp de gás ideal.
    type(c_ptr), value, intent(in) :: oHIGCoefs              ! Matriz (se existir) dos coeficientes polinomiais da entalpia de gás ideal.
    real(c_double), value, intent(in) :: dPhaseMW            ! Massa molar da fase (g/gmol).

    real(c_double), intent(out) :: dMassEnthalpy             ! Entalpia da fase em base mássica ("J/kg").
    real(c_double), intent(out) :: dMassCp                   ! "Cp" da fase em base mássica ("J/kg/K").
    integer(c_int), intent(out) :: iIER                      ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double), dimension(:), pointer :: temp_oPhaseComposition ! Mapeamento do argumento "oPhaseComposition"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc        ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc        ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:), pointer :: temp_oPeneloux         ! Mapeamento do argumento "oPeneloux"
    real(c_double) :: dPressure_Conv                                ! Pressão convertida para o SI
    real(c_double) :: dTemperature_Conv                             ! Temperatura convertida para o SI
    real(c_double) :: dReferenceT                                   ! Temperatura de referência.
    logical :: bZAndDerivZWithTCalculated              ! Retorna "true" se "Z" e "dZdT" da fase foram obtidos no curso deste cálculo.
    real(c_double) :: dPhasicZ                         ! Fator de compressibilidade da fase calculado.
    real(c_double) :: dDerivPhasicZWithT               ! Derivada do fator de compressibilidade da fase com a temperatura
    real(c_double) :: dPhaseMolarEnthalpy              ! Entalpia molar calculada para a fase.
    real(c_double) :: dPhaseMolarCp                    ! "Cp" molar calculado para a fase.

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! MAPEANDO VETORES E MATRIZES:
    call c_f_pointer(oPhaseComposition, temp_oPhaseComposition, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature + 273.15d0

    ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL:
    call CalculatePhaseCpAndEnthalpy(iNComp, iPhaseID, temp_oPhaseComposition, dPressure_Conv, dTemperature_Conv, iThermodynamicModel, temp_oTc, temp_oPc, temp_oW, &
                    oKij_ForCalc, oLij_ForCalc, temp_oPeneloux, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, .true., dReferenceT, &
                    bZAndDerivZWithTCalculated, dPhasicZ, dDerivPhasicZWithT, dPhaseMolarEnthalpy, dPhaseMolarCp, iIER)

    if(iIER.NE.ERROR_EverythingOK) return

    ! Retornar a entalpia e o Cp MÁSSICOS, e não molares...
    dMassEnthalpy = dPhaseMolarEnthalpy / (dPhaseMW / 1000.0d0)
    dMassCp = dPhaseMolarCp / (dPhaseMW / 1000.0d0)

end subroutine Marlim_CalculatePhaseCpAndEnthalpy

! ===========================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR A MASSA ESPECÍFICA DAS FASES EM ESCOAMENTO
! ===========================================================================================================
subroutine Marlim_CalculatePhaseDensity(dPressure, dTemperature, iPhaseID, oPhaseComposition, iNComp, oTc, oPc, &
                  oW, oPeneloux, oKij, oLij, iDensityCalculationMethod, dPhaseMW, dPhaseDensity, dPhaseCompressibilityFactor, &
                  iIER) bind(C, name = "Marlim_CalculatePhaseDensity")

    ! OBJETIVO: Permitir que o Marlim Transiente calcule "composicionalmente" a massa específica ou o fator de compressibilidade de qualquer
    !           uma das fases em escoamento.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dPressure           ! Pressão na qual deseja-se fazer os cálculos (kgf/cm2).
    real(c_double), value, intent(in) :: dTemperature        ! Temperatura na qual deseja-se fazer os cálculos (oC).
    integer(c_int), value, intent(in) :: iPhaseID            ! Identificador da fase cujas propriedades se deseja calcular, conforme convenção.
    type(c_ptr), value, intent(in) :: oPhaseComposition      ! Vetor de frações molares da fase (indexadas por componente).
    integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    integer(c_int), value, intent(in) :: iDensityCalculationMethod  ! Índice do método de cálculo da massa específica, conforme convenção.
    real(c_double), value, intent(in) :: dPhaseMW            ! Massa molar da fase (g/gmol).

    real(c_double), intent(out) :: dPhaseDensity                ! Massa específica calculada para a fase em questão.
    real(c_double), intent(out) :: dPhaseCompressibilityFactor  ! Fator de compressibilidade da fase.
    integer(c_int), intent(out) :: iIER                         ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double), dimension(:), pointer :: temp_oPhaseComposition ! Mapeamento do argumento "oPhaseComposition"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:), pointer :: temp_oPeneloux         ! Mapeamento do argumento "oPeneloux"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc        ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc        ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double) :: dPressure_Conv                                ! Pressão convertida para o SI
    real(c_double) :: dTemperature_Conv                             ! Temperatura convertida para o SI
    real(c_double) :: dMW_SI                                        ! Massa molar no SI

    ! ------------------ CONSTANTES:
    real(c_double), parameter :: dR = 8.314d0                       ! R no SI (J/mol/K)

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! MAPEANDO VETORES E MATRIZES:
    call c_f_pointer(oPhaseComposition, temp_oPhaseComposition, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature + 273.15d0
    dMW_SI = dPhaseMW / 1000.0d0     ! para "kg/gmol"

    ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL:
    call CalculatePhaseDensity(iNComp, iPhaseID, temp_oPhaseComposition, dPressure_Conv, dTemperature_Conv, iDensityCalculationMethod, &
                                temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, &
                      temp_oPeneloux, dPhaseMW, dPhaseDensity, iIER)

    if(iIER.NE.ERROR_EverythingOK) return

    ! CÁLCULO DO FATOR DE COMPRESSIBILIDADE (como resultado complementar):
    dPhaseCompressibilityFactor = dMW_SI * dPressure_Conv / (dPhaseDensity * dR * dTemperature_Conv)

    ! NOTA: Calculado desta forma, o "Z" pode não corresponder ao obtido pela equação de estado, devido ao
    !       possível uso de "volume translation" (para mais detalhes, ver a subrotina "CalculatePhaseDensityFromZAndC").
    !       ANALISAR qual Z é necessário no Marlim: o da equação de estado, ou o que garante a equação usada acima.

end subroutine Marlim_CalculatePhaseDensity

! ===================================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR AS DERIVADAS DE "Z" e "RHO" EM RELAÇÃO A "T" e "P"
! ===================================================================================================================
subroutine Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ(dPressure, dTemperature, iThermodynamicModel, iPhaseID, iNComp, oPhaseComposition, &
                    oTc, oPc, oW, oKij, oLij, oPeneloux, dPhaseMW, dPhasicZ, dDerivPhasicZWithT, dDerivPhasicRhoWithT, &
                    dDerivPhasicZWithP, dDerivPhasicRhoWithP, iIER) bind(C, name = "Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ")

    ! OBJETIVO: Permitir que o Marlim Transiente calcule "composicionalmente", para qualquer fase em escoamento,
    !               as derivadas de massa específica e de fator de compressibilidade com a pressão e a temperatura.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dPressure           ! Pressão na qual deseja-se fazer os cálculos (kgf/cm2).
    real(c_double), value, intent(in) :: dTemperature        ! Temperatura na qual deseja-se fazer os cálculos (oC).
    integer(c_int), value, intent(in) :: iThermodynamicModel ! Modelo termodinâmico a ser utilizado nos cálculos, conforme convenção.
    integer(c_int), value, intent(in) :: iPhaseID            ! Identificador da fase cujas propriedades se deseja calcular, conforme convenção.
    integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
    type(c_ptr), value, intent(in) :: oPhaseComposition      ! Vetor de frações molares da fase (indexadas por componente).
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    real(c_double), value, intent(in) :: dPhaseMW            ! Massa molar da fase (g/gmol).

    real(c_double), intent(out) :: dPhasicZ                  ! Fator de compressibilidade calculado para a fase em questão.
    real(c_double), intent(out) :: dDerivPhasicZWithT        ! Derivada do fator de compressibilidade com a temperatura (UNIDADES: "1/K")
    real(c_double), intent(out) :: dDerivPhasicRhoWithT      ! Derivada da massa específica com a temperatura (UNIDADES: "(kg/m3)/K")
    real(c_double), intent(out) :: dDerivPhasicZWithP        ! Derivada do fator de compressibilidade com a pressão (UNIDADES: "cm2/kgf").
    real(c_double), intent(out) :: dDerivPhasicRhoWithP      ! Derivada da massa específica com a pressão (UNIDADES: "(kg/m3)/(kgf/cm2)").
    integer(c_int), intent(out) :: iIER                      ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double) :: dPressure_Conv                                ! Pressão convertida para o SI
    real(c_double) :: dTemperature_Conv                             ! Temperatura convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oPhaseComposition ! Mapeamento do argumento "oPhaseComposition"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc        ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc        ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:), pointer :: temp_oPeneloux         ! Mapeamento do argumento "oPeneloux"
    real(c_double) :: dDerivPhasicZPenelouxWithT
    real(c_double) :: dDerivPhasicZPenelouxWithP

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! MAPEANDO VETORES E MATRIZES:
    call c_f_pointer(oPhaseComposition, temp_oPhaseComposition, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature + 273.15d0

   ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL:
    call CalculateTAndPDerivativesOfPhasicRhoAndZ(dPressure_Conv, dTemperature_Conv, iThermodynamicModel, iPhaseID, iNComp, temp_oPhaseComposition, &
                    temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, temp_oPeneloux, dPhaseMW, dPhasicZ, dDerivPhasicZWithT, &
                    dDerivPhasicRhoWithT, dDerivPhasicZWithP, &
                    dDerivPhasicRhoWithP, dDerivPhasicZPenelouxWithP, dDerivPhasicZPenelouxWithT, iIER)

            ! EXCEPCIONALMENTE: Retornar, para fins de Marlim, as derivadas de "Z" sempre considerando o "volume translation".
            !   Dando certo, transformar os próprios em argumentos de saída e remover as duas próximas linhas:
            dDerivPhasicZWithT = dDerivPhasicZPenelouxWithT
            dDerivPhasicZWithP = dDerivPhasicZPenelouxWithP

    if(iIER.NE.ERROR_EverythingOK) return

    ! CONVERSÃO DE UNIDADES DOS RESULTADOS:
    dDerivPhasicZWithP = dDerivPhasicZWithP * 98066.52d0
    dDerivPhasicRhoWithP = dDerivPhasicRhoWithP * 98066.52d0

end subroutine Marlim_CalculateTAndPDerivativesOfPhasicRhoAndZ

! ===================================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM 3 PARA AJUSTAR COMPOSIÇÕES AO RGO DESEJADO
! ===================================================================================================================
subroutine Marlim_AdjustGlobalCompositionForTargetGOR(iNComp, oOriginalGlobalComposition, oMW, oTc, oPc, oW, oPeneloux, &
            oKij, oLij, iLiqPhaseModel, iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, &
            dStockTankPressure, dStockTankTemperature, dTargetGOR, dGivenInitialBeta, oGivenInitialLiqComposition, &
            oGivenInitialVapComposition, oGORAdjustedGlobalComp, dAdjustedGlobalCompGOR, &
            iIER) bind(C, name = "Marlim_AdjustGlobalCompositionForTargetGOR")

    ! OBJETIVO: Disponibilizar para o simulador MARLIM a funcionalidade de adaptar a composição de determinado fluido a
    !       um valor especificado pelo usuário para a RGO.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes.
    type(c_ptr), value, intent(in) :: oOriginalGlobalComposition        ! Vetor composição global (indexado por componente).
    type(c_ptr), value, intent(in) :: oMW                    ! Vetor de massas molares (indexado por componente).
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    integer(c_int), value, intent(in) :: iLiqPhaseModel      ! Modelo da fase líquida, conforme convenção.
    integer(c_int), value, intent(in) :: iVapPhaseModel      ! Modelo da fase vapor, conforme convenção.
    integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod  ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
    integer(c_int), value, intent(in) :: iVapDensityCalculationMethod  ! Índice do método de cálculo da massa específica do vapor, conforme convenção.
    real(c_double), value, intent(in) :: dStockTankPressure            ! Pressão correspondente às "stock-tank conditions" (kgf/cm2).
    real(c_double), value, intent(in) :: dStockTankTemperature         ! Temperatura correspondente às "stock-tank conditions" (oC).
    real(c_double), value, intent(in) :: dTargetGOR                    ! Valor especificado de RGO, para o qual deseja-se ajustar a composição.
    real(c_double), value, intent(in) :: dGivenInitialBeta             ! Estimativa inicial de fração molar vaporizada para o "flash" (dar negativo caso indisponível).
    type(c_ptr), value, intent(in) :: oGivenInitialLiqComposition        ! Estimativa inicial de composição da fase líquida para o "flash".
    type(c_ptr), value, intent(in) :: oGivenInitialVapComposition        ! Estimativa inicial de composição da fase vapor para o "flash".

    type(c_ptr), value, intent(in) :: oGORAdjustedGlobalComp           ! RESULTADO: Composição global ajustada para o RGO desejado.
    real(c_double), intent(out) :: dAdjustedGlobalCompGOR              ! RESULTADO: RGO calculado com a composição global ajustada.
    integer(c_int), intent(out) :: iIER                                ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double), dimension(:), pointer :: temp_oOriginalGlobalComposition                ! Mapeamento do argumento "oOriginalGlobalComposition"
    real(c_double), dimension(:), pointer :: temp_oMW               ! Mapeamento do argumento "oMW"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:), pointer :: temp_oPeneloux         ! Mapeamento do argumento "oPeneloux"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc        ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc        ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double) :: dStockTankPressure_Conv                       ! Pressão "stock-tank" convertida para o SI
    real(c_double) :: dStockTankTemperature_Conv                    ! Temperatura "stock-tank" convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oGivenInitialLiqComposition    ! Mapeamento do argumento "oGivenInitialLiqComposition"
    real(c_double), dimension(:), pointer :: temp_oGivenInitialVapComposition    ! Mapeamento do argumento "oGivenInitialVapComposition"
    real(c_double), dimension(:), pointer :: temp_oGORAdjustedGlobalComp         ! Mapeamento do argumento "oGORAdjustedGlobalComp"

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! Mapeando vetores e matrizes:
    call c_f_pointer(oOriginalGlobalComposition, temp_oOriginalGlobalComposition, [iNComp])
    call c_f_pointer(oMW, temp_oMW, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oGivenInitialLiqComposition, temp_oGivenInitialLiqComposition, [iNComp])
    call c_f_pointer(oGivenInitialVapComposition, temp_oGivenInitialVapComposition, [iNComp])
    call c_f_pointer(oGORAdjustedGlobalComp, temp_oGORAdjustedGlobalComp, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dStockTankPressure_Conv = dStockTankPressure * 98066.52d0
    dStockTankTemperature_Conv = dStockTankTemperature + 273.15d0

    ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL:
    call AdjustGlobalCompositionForTargetGOR_Approach1(iNComp, temp_oOriginalGlobalComposition, temp_oMW, temp_oTc, temp_oPc, temp_oW, &
                temp_oPeneloux, oKij_ForCalc, oLij_ForCalc, &
                    iLiqPhaseModel, iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure_Conv, &
                    dStockTankTemperature_Conv, dTargetGOR, (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, &
                    temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, &
                    temp_oGORAdjustedGlobalComp, dAdjustedGlobalCompGOR, iIER)

end subroutine Marlim_AdjustGlobalCompositionForTargetGOR

! ===================================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA SIMULAÇÕES "STOCK-TANK" DE DETERMINADO FLUIDO
! ===================================================================================================================
subroutine Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions(iNComp, oZ, oMW, oTc, oPc, oW, oPeneloux, oKij, oLij, &
                iLiqPhaseModel, iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure, &
                dStockTankTemperature, &
                dGivenInitialBeta, oGivenInitialLiqComposition, oGivenInitialVapComposition, &
                iCalculatedStockTankThermodynamicCondition, dStockTankVaporMassFraction, dStockTankLiquidDensity, &
                dStockTankVaporDensity, dStockTankCompositionalGOR, iIER) bind(C, name = "Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions")

    ! OBJETIVO: Permitir que o simulador Marlim Transiente calcule "composicionalmente", à temperatura e pressão "stock-tank", propriedades
    !           comumente necessárias em cálculos "black-oil" posteriores.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
    type(c_ptr), value, intent(in) :: oZ                     ! Vetor composição global (indexado por componente).
    type(c_ptr), value, intent(in) :: oMW                    ! Vetor de massas molares (indexado por componente).
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    integer(c_int), value, intent(in) :: iLiqPhaseModel      ! Modelo da fase líquida, conforme convenção.
    integer(c_int), value, intent(in) :: iVapPhaseModel      ! Modelo da fase vapor, conforme convenção.
    integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod  ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
    integer(c_int), value, intent(in) :: iVapDensityCalculationMethod  ! Índice do método de cálculo da massa específica do vapor, conforme convenção.
    real(c_double), value, intent(in) :: dStockTankPressure            ! Pressão correspondente às "stock-tank conditions" (kgf/cm2).
    real(c_double), value, intent(in) :: dStockTankTemperature         ! Temperatura correspondente às "stock-tank conditions" (oC).
    real(c_double), value, intent(in) :: dGivenInitialBeta               ! Estimativa inicial de fração molar vaporizada para o "flash" (dar negativo caso indisponível).
    type(c_ptr), value, intent(in) :: oGivenInitialLiqComposition        ! Estimativa inicial de composição da fase líquida para o "flash".
    type(c_ptr), value, intent(in) :: oGivenInitialVapComposition        ! Estimativa inicial de composição da fase vapor para o "flash".

    integer(c_int), intent(out) :: iCalculatedStockTankThermodynamicCondition   ! Código da condição termodinâmica determinada às "stock-tank conditions",
                                                                                ! conforme convencionado.
    real(c_double), intent(out) :: dStockTankVaporMassFraction                  ! Fração mássica vaporizada determinada às condições "stock-tank".
    real(c_double), intent(out) :: dStockTankLiquidDensity                      ! Massa específica da fase líquida calculada às condições "stock-tank".
    real(c_double), intent(out) :: dStockTankVaporDensity                       ! Massa específica da fase vapor calculada às condições "stock-tank".
    real(c_double), intent(out) :: dStockTankCompositionalGOR                   ! "Rgo" (ADIMENSIONAL) obtida nas condições "stock-tank" por cálculos composicionais.
    integer(c_int), intent(out) :: iIER                                         ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double), dimension(:), pointer :: temp_oZ                ! Mapeamento do argumento "oZ"
    real(c_double), dimension(:), pointer :: temp_oMW               ! Mapeamento do argumento "oMW"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:), pointer :: temp_oPeneloux         ! Mapeamento do argumento "oPeneloux"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc        ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc        ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double) :: dStockTankPressure_Conv                       ! Pressão "stock-tank" convertida para o SI
    real(c_double) :: dStockTankTemperature_Conv                    ! Temperatura "stock-tank" convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oGivenInitialLiqComposition    ! Mapeamento do argumento "oGivenInitialLiqComposition"
    real(c_double), dimension(:), pointer :: temp_oGivenInitialVapComposition    ! Mapeamento do argumento "oGivenInitialVapComposition"

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! Mapeando vetores e matrizes:
    call c_f_pointer(oZ, temp_oZ, [iNComp])
    call c_f_pointer(oMW, temp_oMW, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oGivenInitialLiqComposition, temp_oGivenInitialLiqComposition, [iNComp])
    call c_f_pointer(oGivenInitialVapComposition, temp_oGivenInitialVapComposition, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dStockTankPressure_Conv = dStockTankPressure * 98066.52d0
    dStockTankTemperature_Conv = dStockTankTemperature + 273.15d0

    ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL:
    call CalculateCommonlyRequiredValuesAtStockTankConditions(iNComp, temp_oZ, temp_oMW, temp_oTc, temp_oPc, temp_oW, temp_oPeneloux, oKij_ForCalc, &
                            oLij_ForCalc, iLiqPhaseModel, &
                            iVapPhaseModel, iLiqDensityCalculationMethod, iVapDensityCalculationMethod, dStockTankPressure_Conv, dStockTankTemperature_Conv, &
                            (dGivenInitialBeta.GE.(0.0d0)), dGivenInitialBeta, temp_oGivenInitialLiqComposition, temp_oGivenInitialVapComposition, &
                            iCalculatedStockTankThermodynamicCondition, dStockTankVaporMassFraction, dStockTankLiquidDensity, dStockTankVaporDensity, &
                            dStockTankCompositionalGOR, iIER)

end subroutine Marlim_CalculateCommonlyRequiredValuesAtStockTankConditions

! ===================================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR "COMPOSICIONALMENTE" O RS
! ===================================================================================================================
subroutine Marlim_CalculateSolutionGasOilRatio(dVaporMassFraction, dStockTankVaporMassFraction, dStockTankLiquidDensity, dStockTankVaporDensity, &
                dCompositionalSolutionGasOilRatio) bind(C, name = "Marlim_CalculateSolutionGasOilRatio")

    ! OBJETIVO: Permitir que o simulador Marlim Transiente calcule "Rs" a partir de resultados composicionais.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dVaporMassFraction             ! Fração mássica vaporizada à P e T locais.
    real(c_double), value, intent(in) :: dStockTankVaporMassFraction    ! Fração mássica vaporizada nas condições "stock-tank".
    real(c_double), value, intent(in) :: dStockTankLiquidDensity        ! Massa específica calculada para a fase líquida nas condições "stock-tank".
    real(c_double), value, intent(in) :: dStockTankVaporDensity         ! Massa específica calculada para a fase vapor nas condições "stock-tank".

    real(c_double), intent(out) :: dCompositionalSolutionGasOilRatio    ! "Rs" (expresso em "scft/stdBBL") calculado pela via composicional.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! CHAMADA PRINCIPAL:
    call CalculateSolutionGasOilRatio(dVaporMassFraction, dStockTankVaporMassFraction, dStockTankLiquidDensity, dStockTankVaporDensity, &
                        dCompositionalSolutionGasOilRatio)

    ! Converter o "Rs" para "scft/stdBBL":
    dCompositionalSolutionGasOilRatio = dCompositionalSolutionGasOilRatio * (1.0d0 / 2.831685d-2)   ! "scft/m3"
    dCompositionalSolutionGasOilRatio = dCompositionalSolutionGasOilRatio * 1.589873d-1             ! "scft/bbl"

end subroutine Marlim_CalculateSolutionGasOilRatio

! ===================================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR "COMPOSICIONALMENTE" O Bo
! ===================================================================================================================
subroutine Marlim_CalculateOilFormationVolumeFactor(dPressure, dTemperature, iNComp, oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqDensityCalculationMethod, &
                oLiqPhaseComposition, dLiquidPhaseMW, dVaporMassFraction, dStockTankLiquidDensity, dStockTankVaporMassFraction, &
                dCompositionalOilFormationVolumeFactor, iIER) bind(C, name = "Marlim_CalculateOilFormationVolumeFactor")

    ! OBJETIVO: Permitir que o simulador Marlim Transiente calcule "Bo" a partir de resultados composicionais.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dPressure                      ! Pressão na qual deseja-se fazer os cálculos (kgf/cm2).
    real(c_double), value, intent(in) :: dTemperature                   ! Temperatura na qual deseja-se fazer os cálculos (oC).
    integer(c_int), value, intent(in) :: iNComp                         ! Número de componentes.
    type(c_ptr), value, intent(in) :: oTc                               ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                               ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                                ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oKij                              ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                              ! Matriz dos parâmetros "lij".
    type(c_ptr), value, intent(in) :: oPeneloux                         ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod   ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
    type(c_ptr), value, intent(in) :: oLiqPhaseComposition              ! Vetor composição MOLAR do líquido (indexado por componente).
    real(c_double), value, intent(in) :: dLiquidPhaseMW                 ! Massa molar da fase líquida (g/gmol).
    real(c_double), value, intent(in) :: dVaporMassFraction             ! Fração mássica vaporizada à P e T locais.
    real(c_double), value, intent(in) :: dStockTankLiquidDensity        ! Massa específica calculada para a fase líquida nas condições "stock-tank".
    real(c_double), value, intent(in) :: dStockTankVaporMassFraction    ! Fração mássica vaporizada nas condições "stock-tank".

    real(c_double), intent(out) :: dCompositionalOilFormationVolumeFactor  ! "Bo" ADIMENSIONAL (m3/m3) calculado pela via composicional.
    integer(c_int), intent(out) :: iIER                                    ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double) :: dPressure_Conv                                    ! Pressão convertida para o SI
    real(c_double) :: dTemperature_Conv                                 ! Temperatura convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oLiqPhaseComposition  ! Mapeamento do argumento "oLiqPhaseComposition"
    real(c_double), dimension(:), pointer :: temp_oTc                   ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc                   ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                    ! Mapeamento do argumento "oW"
    real(c_double), dimension(:,:), pointer :: temp_oKij                ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc            ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij                ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc            ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:), pointer :: temp_oPeneloux             ! Mapeamento do argumento "oPeneloux"
    real(c_double) :: dLiquidDensity                                    ! Massa específica calculada para a fase líquida.

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! MAPEANDO VETORES E MATRIZES:
    call c_f_pointer(oLiqPhaseComposition, temp_oLiqPhaseComposition, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature + 273.15d0

    ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.

    ! INICIALIZAÇÕES:
    iIER = ERROR_EverythingOK

    ! Calcular a massa específica da fase líquida à P e T:
    call CalculatePhaseDensity(iNComp, PHASE_Liquid, temp_oLiqPhaseComposition, dPressure_Conv, dTemperature_Conv, iLiqDensityCalculationMethod, temp_oTc, &
                      temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, &
                      temp_oPeneloux, dLiquidPhaseMW, dLiquidDensity, iIER)

    if(iIER.NE.ERROR_EverythingOK) return

    ! CHAMADA PRINCIPAL:
    call CalculateOilFormationVolumeFactor(dLiquidDensity, dStockTankLiquidDensity, dVaporMassFraction, dStockTankVaporMassFraction, &
                    dCompositionalOilFormationVolumeFactor)

end subroutine Marlim_CalculateOilFormationVolumeFactor

! ===========================================================================================================
!       ROTINA A SER CHAMADA PELO MARLIM TRANSIENTE PARA CALCULAR A PRESSÃO DE BOLHA DA MISTURA EM ESCOAMENTO
! ===========================================================================================================
subroutine Marlim_CalculateBubblePressure(dTemperature, iNComp, oZ, oTc, oPc, oW, oKij, oLij, oPeneloux, iLiqPhaseModel, &
                                             iVapPhaseModel, dPressure, dCalculatedBubbleP, iIER) bind(C, name = "Marlim_CalculateBubblePressure")

    ! OBJETIVO: Permitir que o Marlim Transiente calcule "composicionalmente" a pressão de bolha de dada mistura fluida em escoamento.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    real(c_double), value, intent(in) :: dTemperature        ! Temperatura na qual deseja-se fazer o cálculo de pressão de bolha (oC).
    integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
    type(c_ptr), value, intent(in) :: oZ                     ! Vetor composição global (indexado por componente).
    type(c_ptr), value, intent(in) :: oTc                    ! Vetor temperatura crítica (indexado por componente).
    type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
    type(c_ptr), value, intent(in) :: oW                     ! Vetor de fatores acêntricos dos componentes.
    type(c_ptr), value, intent(in) :: oKij                   ! Matriz dos parâmetros de interação "kij".
    type(c_ptr), value, intent(in) :: oLij                   ! Matriz dos parâmetros "lij".
    type(c_ptr), value, intent(in) :: oPeneloux              ! Segundo o código original, vetor dos "volumes de shift" para cada componente.
    integer(c_int), value, intent(in) :: iLiqPhaseModel      ! Modelo da fase líquida, conforme convenção.
    integer(c_int), value, intent(in) :: iVapPhaseModel      ! Modelo da fase vapor, conforme convenção.
    real(c_double), value, intent(in) :: dPressure           ! Pressão na qual deseja-se fazer os cálculos (kgf/cm2). CASO INDISPONÍVEL, especificar
                                                             !      um valor de pressão que esteja aproximadamente na mesma ordem de grandeza do
                                                             !      resultado esperado (a função deste argumento é apenas a de dar subsídios iniciais para
                                                             !      o método numérico utilizado; sua exatidão não é importante). MESMO QUE DISPONIVEL, uma
                                                             !      sugestão interessante para o Marlim Transiente seria especificar a pressão de bolha de
                                                             !      algum ponto de malha adjacente, ou então uma pressão de bolha já calculada para a célula desejada.

    real(c_double), intent(out) :: dCalculatedBubbleP        ! Pressão de bolha calculada.
    integer(c_int), intent(out) :: iIER                      ! Código de erros "geral" desta rotina, conforme convencionado.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    real(c_double) :: dStepOnP                                      ! Passo na pressão utilizado pelo método numérico de cálculo.
    real(c_double) :: dPressure_Conv                                ! Pressão convertida para o SI
    real(c_double) :: dTemperature_Conv                             ! Temperatura convertida para o SI
    real(c_double), dimension(:), pointer :: temp_oZ                ! Mapeamento do argumento "oZ"
    real(c_double), dimension(:), pointer :: temp_oTc               ! Mapeamento do argumento "oTc"
    real(c_double), dimension(:), pointer :: temp_oPc               ! Mapeamento do argumento "oPc"
    real(c_double), dimension(:), pointer :: temp_oW                ! Mapeamento do argumento "oW"
    real(c_double), dimension(:,:), pointer :: temp_oKij            ! Mapeamento do argumento "oKij"
    real(c_double), dimension(iNComp,iNComp) :: oKij_ForCalc        ! Argumento "oKij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:,:), pointer :: temp_oLij            ! Mapeamento do argumento "oLij"
    real(c_double), dimension(iNComp,iNComp) :: oLij_ForCalc        ! Argumento "oLij" tratado e pronto para ser usado nos cálculos.
    real(c_double), dimension(:), pointer :: temp_oPeneloux         ! Mapeamento do argumento "oPeneloux"
    integer(c_int) :: iIER_BubbleT                                  ! "Dummy variable" (T. Bolha não está sendo calculada).
    real(c_double) :: dCalculatedBubbleT                            ! "Dummy variable" (T. Bolha não está sendo calculada).
    integer(c_int) :: iIER_DewT                                     ! "Dummy variable" (T. Orvalho não está sendo calculada).
    real(c_double) :: dCalculatedDewT                               ! "Dummy variable" (T. Orvalho não está sendo calculada).
    integer(c_int) :: iIER_BubbleP                                  ! Código de possíveis erros no cálculo de P. Bolha, conforme convenção.

    logical :: bGotEstimateForBubblePoint, bGotEstimateForDewPoint, bHasInitialEstimates
    real(c_double) :: dClosestToBubbleT, dClosestToDewT, dInitialBeta, dBubblePObjectiveFunction
    real(c_double), dimension(iNComp) :: oVapCompositionClosestToBubbleT, oLiqCompositionClosestToDewT, oInitialLiqComposition, oInitialVapComposition

    ! ------------ PROCEDIMENTOS, CHAMADAS E CÁLCULOS:

    ! Mapeando vetores e matrizes:
    call c_f_pointer(oZ, temp_oZ, [iNComp])
    call c_f_pointer(oTc, temp_oTc, [iNComp])
    call c_f_pointer(oPc, temp_oPc, [iNComp])
    call c_f_pointer(oW, temp_oW, [iNComp])
    call c_f_pointer(oKij, temp_oKij, [iNComp, iNComp])
    call c_f_pointer(oLij, temp_oLij, [iNComp, iNComp])
    call c_f_pointer(oPeneloux, temp_oPeneloux, [iNComp])

    ! CONVERSÃO DE UNIDADES:
    dPressure_Conv = dPressure * 98066.52d0
    dTemperature_Conv = dTemperature + 273.15d0

   ! TRANSPOSIÇÃO DE MATRIZES, quando necessário:

    !oKij_ForCalc = transpose(transpose(temp_oKij))
    oKij_ForCalc = temp_oKij    ! OBS: Por enquanto, os Kijs estão sendo fornecidos por outra subrotina FORTRAN. Por isso, por enquanto
                                !           não há necessidade de fazer transposição.

    !oLij_ForCalc = transpose(transpose(temp_oLij))
    oLij_ForCalc = temp_oLij    ! OBS: Mesma observação; por enquanto os Lijs estão sendo fornecidos por outra subrotina FORTRAN.


    ! INICIALIZAÇÕES:
    dStepOnP = -10.0d0              ! Para mais detalhes, consultar a subrotina "CalcPressureStepwiseBubbleAndDewVariables".
    iIER_BubbleT = ERROR_EverythingOK
    iIER_DewT = ERROR_EverythingOK
    iIER_BubbleP = ERROR_EverythingOK
    iIER = ERROR_EverythingOK

    ! CHAMADA PRINCIPAL ("commented out" em 4/8/2021 em favor do método mais eficiente):
    !call CalcPressureStepwiseBubbleAndDewVariables(dStepOnP, dPressure_Conv, dTemperature_Conv, iNComp, &
    !                                                     temp_oZ, temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, iLiqPhaseModel, iVapPhaseModel, &
    !                                                     temp_oPeneloux, .false., .false., .true., iIER_BubbleT, &
    !                                                     dCalculatedBubbleT, iIER_DewT, dCalculatedDewT, iIER_BubbleP, dCalculatedBubbleP, iIER)

    ! ==========================================
    !   INSERÇÃO EM 30/7/2021!

    ! Substituir o cálculo da pressão de bolha acima por outro mais eficiente:

    call GenerateVLEExtremaInitialEstimates(dPressure_Conv, iNComp, temp_oZ, temp_oTc, temp_oPc, temp_oW, oKij_ForCalc, oLij_ForCalc, &
                            iLiqPhaseModel, iVapPhaseModel, temp_oPeneloux, &
                            bGotEstimateForBubblePoint, dClosestToBubbleT, oVapCompositionClosestToBubbleT, bGotEstimateForDewPoint, dClosestToDewT, &
                            oLiqCompositionClosestToDewT, bHasInitialEstimates, dInitialBeta, oInitialLiqComposition, oInitialVapComposition)

    call SecantMethod_CalculateBubbleOrDewPressure(-10.0d0, -10.0d0, dTemperature_Conv, iNComp, temp_oZ, temp_oTc, temp_oPc, temp_oW, &
            temp_oPeneloux, oKij_ForCalc, oLij_ForCalc, iLiqPhaseModel, iVapPhaseModel, CALCTYPE_BubblePoint, bGotEstimateForBubblePoint, &
            oVapCompositionClosestToBubbleT, dCalculatedBubbleP, dBubblePObjectiveFunction, iIER_BubbleP)

    ! PAREI AQUI EM 2/8/2021
    ! Verificar a lógica acima, talvez ajeitar a declaração dos argumentos, comentar...!!

    !   FIM DA INSERÇÃO EM 30/7/2021
    ! ==========================================

    hasErrors: if((iIER.NE.ERROR_EverythingOK).OR.(iIER_BubbleP.NE.ERROR_EverythingOK)) then
        ! Deu erro!
        if(iIER.EQ.ERROR_EverythingOK) iIER = iIER_BubbleP
        dCalculatedBubbleP = -5000.0d0
        return
    end if hasErrors

    ! Converter para "kgf/cm2" para uso no Marlim Transiente:
    dCalculatedBubbleP = dCalculatedBubbleP / 98066.52d0

end subroutine Marlim_CalculateBubblePressure

! ===================================================================================================================
!       ROTINAS A SEREM CHAMADAS PARA TRATAR COMPONENTES "ZERADOS"
! ===================================================================================================================
subroutine CountZeroedComponents(iNComp, oZ, isZeroedComp, iNComp_Fixed)

    ! OBJETIVO: Localizar e contar os componentes "zerados" nos argumentos de entrada de um cálculo composicional (foi detectado que
    !       a presença deles pode piorar o desempenho das simulações de escoamento 1D).
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    integer(c_int), value, intent(in) :: iNComp                         ! Número original de componentes.
    real(c_double), dimension(iNComp), intent(in) :: oZ                 ! Vetor composição global original (indexado por componente).

    logical, dimension(iNComp), intent(out) :: isZeroedComp             ! "True" se o componente "i" estava zerado originalmente
    integer(c_int), intent(out) :: iNComp_Fixed                         ! Número de componentes não-nulos.

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    integer :: i

    ! ------------ PROCEDIMENTOS:

    ! Inicializações:
    iNComp_Fixed = iNComp

    ! "Loop" principal:
    mainLoop: do i = 1, iNComp

        isZeroedComp(i) = .false.

        isZeroedComponent: if(oZ(i).LT.(5.0d-6)) then

            isZeroedComp(i) = .true.
            iNComp_Fixed = iNComp_Fixed - 1

        end if isZeroedComponent

    end do mainLoop

end subroutine CountZeroedComponents



subroutine RemoveZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, oZ, oMW, oTc, oPc, oW, oPeneloux, oGivenInitialLiqComp, &
                oGivenInitialVapComp, oKij, oLij, bHasInitialFlashEstimates, oZ_Fixed, oMW_Fixed, oTc_Fixed, oPc_Fixed, oW_Fixed, oPeneloux_Fixed, &
                oKij_Fixed, oLij_Fixed, oGivenInitialLiqComp_Fixed, oGivenInitialVapComp_Fixed)

    ! OBJETIVO: Remover os componentes "zerados" nos argumentos de entrada de um cálculo composicional (foi detectado que
    !       a presença deles pode piorar o desempenho das simulações de escoamento 1D).
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    integer(c_int), value, intent(in) :: iNComp                         ! Número original de componentes.
    logical, dimension(iNComp), intent(in) :: isZeroedComp              ! "True" se o componente "i" estava zerado originalmente
    integer(c_int), value, intent(in) :: iNComp_Fixed                          ! Número de componentes não-nulos.
    real(c_double), dimension(iNComp), intent(in) :: oZ                 ! Vetor composição global original (indexado por componente).
    real(c_double), dimension(iNComp), intent(in) :: oMW
    real(c_double), dimension(iNComp), intent(in) :: oTc
    real(c_double), dimension(iNComp), intent(in) :: oPc
    real(c_double), dimension(iNComp), intent(in) :: oW
    real(c_double), dimension(iNComp), intent(in) :: oPeneloux
    real(c_double), dimension(iNComp), intent(in) :: oGivenInitialLiqComp
    real(c_double), dimension(iNComp), intent(in) :: oGivenInitialVapComp
    real(c_double), dimension(iNComp, iNComp), intent(in) :: oKij
    real(c_double), dimension(iNComp, iNComp), intent(in) :: oLij
    logical, intent(in) :: bHasInitialFlashEstimates

    real(c_double), dimension(iNComp_Fixed), intent(out) :: oZ_Fixed    ! Frações molares globais dos componentes não-nulos.
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oMW_Fixed
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oTc_Fixed
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oPc_Fixed
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oW_Fixed
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oPeneloux_Fixed
    real(c_double), dimension(iNComp_Fixed, iNComp_Fixed), intent(out) :: oKij_Fixed
    real(c_double), dimension(iNComp_Fixed, iNComp_Fixed), intent(out) :: oLij_Fixed
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oGivenInitialLiqComp_Fixed
    real(c_double), dimension(iNComp_Fixed), intent(out) :: oGivenInitialVapComp_Fixed

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    integer :: i, j
    integer :: iNonZeroCount
    integer :: iNonZeroColCount

    ! ------------ PROCEDIMENTOS:

    ! Inicializando:
    iNonZeroCount = 0

    ! Loops principais:
    outer: do i = 1, iNComp

        if(isZeroedComp(i)) cycle outer

        iNonZeroCount = iNonZeroCount + 1

        oZ_Fixed(iNonZeroCount) = oZ(i)
        oMW_Fixed(iNonZeroCount) = oMW(i)
        oTc_Fixed(iNonZeroCount) = oTc(i)
        oPc_Fixed(iNonZeroCount) = oPc(i)
        oW_Fixed(iNonZeroCount) = oW(i)
        oPeneloux_Fixed(iNonZeroCount) = oPeneloux(i)

        if(bHasInitialFlashEstimates) then
            oGivenInitialLiqComp_Fixed(iNonZeroCount) = oGivenInitialLiqComp(i)
            oGivenInitialVapComp_Fixed(iNonZeroCount) = oGivenInitialVapComp(i)
        end if

        iNonZeroColCount = 0

        inner: do j = 1, iNComp

            if(isZeroedComp(j)) cycle inner

            iNonZeroColCount = iNonZeroColCount + 1

            oKij_Fixed(iNonZeroCount, iNonZeroColCount) = oKij(i, j)
            oLij_Fixed(iNonZeroCount, iNonZeroColCount) = oLij(i, j)

        end do inner

    end do outer

end subroutine RemoveZeroedComponents


subroutine ReInsertZeroedComponents(iNComp, isZeroedComp, iNComp_Fixed, oFixedComposition, oComposition)

    ! OBJETIVO: Reinserir nas devidas posições os componentes "zerados" nas composições obtidas de um cálculo composicional.
    implicit none

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
    integer(c_int), value, intent(in) :: iNComp                                 ! Número original de componentes.
    logical, dimension(iNComp), intent(in) :: isZeroedComp                      ! "True" se o componente "i" estava zerado originalmente
    integer(c_int), value, intent(in) :: iNComp_Fixed                           ! Número de componentes não-nulos.
    real(c_double), dimension(iNComp_Fixed), intent(in) :: oFixedComposition    ! Composição obtida sem os componentes "zerados"

    real(c_double), dimension(iNComp), intent(out) :: oComposition              ! Composição com os componentes "zerados"

    ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
    integer :: i
    integer :: iNonZeroCount

    ! ------------ PROCEDIMENTOS:

    ! Inicializando:
    iNonZeroCount = 0

    ! Loop principal:
    outer: do i = 1, iNComp

        oComposition(i) = 0.0d0

        if(isZeroedComp(i)) cycle outer

        iNonZeroCount = iNonZeroCount + 1

        oComposition(i) = oFixedComposition(iNonZeroCount)

    end do outer

end subroutine ReInsertZeroedComponents

end module MarlimComposicional
