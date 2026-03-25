module PhaseProperties

    ! OBJETIVO: Conter rotinas de cálculo composicional de propriedades termofísicas e de transporte em geral.
    ! UNIDADES: TODA a programação contida neste módulo considera que TODOS os valores (entradas e saídas
    !   das rotinas e suas variáveis internas) estão no SI, EXCETO QUANDO devidamente explicitado...

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use Constants
    use VLECalculations

    implicit none

    contains

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseProperties_Approach1(iNComp, oMW, oTc, oPc, oW, oKij, oLij, oPeneloux, oParachors, oZ, &
                    dP, dT, iThermodynamicCondition, dMolarBeta, oLiqPhaseComposition, oVapPhaseComposition, &
                    iLiqDensityCalculationMethod, iVapDensityCalculationMethod, iLiqHAndCpThermodynamicModel, &
                    iVapHAndCpThermodynamicModel, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, iLiqModel, iVapModel, dVaporMassFraction, dLiquidZ, &
                    dLiquidZWithPeneloux, dLiqDensity, &
                    dDerivLiquidZWithT, dDerivLiquidZPenelouxWithT, dDerivLiquidRhoWithT, dDerivLiquidZWithP, dDerivLiquidZPenelouxWithP, &
                    dDerivLiquidRhoWithP, dVaporZ, dVaporZWithPeneloux, &
                    dVapDensity, dDerivVaporZWithT, dDerivVaporZPenelouxWithT, dDerivVaporRhoWithT, dDerivVaporZWithP, &
                    dDerivVaporZPenelouxWithP, dDerivVaporRhoWithP, dLiqMassEnthalpy, dVapMassEnthalpy, dLiqMassCp, dVapMassCp, dSurfaceTension, dLiqJouleThomsonCoef, &
                    dVapJouleThomsonCoef, iIER)

        ! OBJETIVO: Calcular, segundo a primeira sequência e metodologia propostas, as propriedades das fases em escoamento no MARLIM.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMW            ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), dimension(iNComp), intent(in) :: oParachors     ! Vetor de "parachors" para cada componente.
        real(c_double), dimension(iNComp), intent(in) :: oZ             ! Vetor composição global (indexado por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iThermodynamicCondition    ! Condição termodinâmica da mistura.
        real(c_double), value, intent(in) :: dMolarBeta                 ! Fração vaporizada em base molar.
        real(c_double), dimension(iNComp), intent(in) :: oLiqPhaseComposition    ! Vetor composição MOLAR do líquido (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oVapPhaseComposition    ! Vetor composição MOLAR do vapor (indexado por componente).
        integer(c_int), value, intent(in) :: iLiqDensityCalculationMethod  ! Índice do método de cálculo da massa específica do líquido, conforme convenção.
        integer(c_int), value, intent(in) :: iVapDensityCalculationMethod  ! Índice do método de cálculo da massa específica do vapor, conforme convenção.
        integer(c_int), value, intent(in) :: iLiqHAndCpThermodynamicModel  ! Modelo termodinâmico para cálculo da entalpia e Cp do líquido, conforme convencionado.
        integer(c_int), value, intent(in) :: iVapHAndCpThermodynamicModel  ! Modelo termodinâmico para cálculo da entalpia e Cp do vapor, conforme convencionado.
        integer(c_int), value, intent(in) :: iIGCoefsSource             ! Índice da fonte dos coeficientes polinomiais de gás ideal, conforme convenção.
        type(c_ptr), value, intent(in) :: oCpIGCoefs                    ! Matriz (se existir) dos coeficientes polinomiais do Cp de gás ideal.
        type(c_ptr), value, intent(in) :: oHIGCoefs                     ! Matriz (se existir) dos coeficientes polinomiais da entalpia de gás ideal.
        integer(c_int), value, intent(in) :: iLiqModel                  ! Modelo da fase líquida, conforme convenção.
        integer(c_int), value, intent(in) :: iVapModel                  ! Modelo da fase vapor, conforme convenção.

        real(c_double), intent(out) :: dVaporMassFraction               ! Fração mássica vaporizada.
        real(c_double), intent(out) :: dLiquidZ                         ! Fator de compressibilidade calculado para a fase líquida.
        real(c_double), intent(out) :: dLiquidZWithPeneloux
        real(c_double), intent(out) :: dLiqDensity                      ! Massa específica calculada para a fase líquida.
        real(c_double), intent(out) :: dDerivLiquidZWithT
        real(c_double), intent(out) :: dDerivLiquidZPenelouxWithT
        real(c_double), intent(out) :: dDerivLiquidRhoWithT
        real(c_double), intent(out) :: dDerivLiquidZWithP
        real(c_double), intent(out) :: dDerivLiquidZPenelouxWithP
        real(c_double), intent(out) :: dDerivLiquidRhoWithP
        real(c_double), intent(out) :: dVaporZ                          ! Fator de compressibilidade do vapor calculado.
        real(c_double), intent(out) :: dVaporZWithPeneloux
        real(c_double), intent(out) :: dVapDensity                      ! Massa específica calculada para a fase vapor.
        real(c_double), intent(out) :: dDerivVaporZWithT
        real(c_double), intent(out) :: dDerivVaporZPenelouxWithT
        real(c_double), intent(out) :: dDerivVaporRhoWithT
        real(c_double), intent(out) :: dDerivVaporZWithP
        real(c_double), intent(out) :: dDerivVaporZPenelouxWithP
        real(c_double), intent(out) :: dDerivVaporRhoWithP
        real(c_double), intent(out) :: dLiqMassEnthalpy                 ! Entalpia do líquido em base mássica ("J/kg").
        real(c_double), intent(out) :: dVapMassEnthalpy                 ! Entalpia do vapor em base mássica ("J/kg").
        real(c_double), intent(out) :: dLiqMassCp                       ! "Cp" do líquido em base mássica ("J/kg/K").
        real(c_double), intent(out) :: dVapMassCp                       ! "Cp" do vapor em base mássica ("J/kg/K").
        real(c_double), intent(out) :: dSurfaceTension                  ! Tensão superficial calculada.
        real(c_double), intent(out) :: dLiqJouleThomsonCoef             ! Coeficiente de Joule-Thomson para a fase líquida ("K/Pa")
        real(c_double), intent(out) :: dVapJouleThomsonCoef             ! Coeficiente de Joule-Thomson para a fase vapor ("K/Pa")
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dLiquidPhaseMW                                ! Massa molar da fase líquida (g/gmol).
        real(c_double) :: dVaporPhaseMW                                 ! Massa molar da fase vapor (g/gmol).
        real(c_double) :: dMixtureMW                                    ! Massa molar da mistura (g/gmol).
        logical :: bLiquidPresent, bVaporPresent
        real(c_double) :: dReferenceT                                   ! Temperatura de referência (fornecida ou determinada pela rotina).
        logical :: bLiqZAndDerivZWithTCalculated           ! Retorna "true" se "Z" e "dZdT" do líquido foram obtidos no curso deste cálculo.
        real(c_double) :: dLiquidMolarEnthalpy             ! Entalpia molar calculada para a fase líquida.
        real(c_double) :: dLiquidMolarCp                   ! "Cp" molar calculado para a fase líquida.
        logical :: bVapZAndDerivZWithTCalculated           ! Retorna "true" se "Z" e "dZdT" do vapor foram obtidos no curso deste cálculo.
        real(c_double) :: dVaporMolarEnthalpy              ! Entalpia molar calculada para a fase vapor.
        real(c_double) :: dVaporMolarCp                    ! "Cp" molar calculado para a fase vapor.

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                       ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! Calcular a fração mássica vaporizada:
        call CalculateMolecularWeightsAndVaporMassFraction(iThermodynamicCondition, iNComp, oMW, oZ, oLiqPhaseComposition, &
                oVapPhaseComposition, dMolarBeta, dLiquidPhaseMW, dVaporPhaseMW, dMixtureMW, dVaporMassFraction)

        ! Calcular as propriedades da fase líquida, caso esta fase esteja presente:
        bLiquidPresent = (iThermodynamicCondition.EQ.THERMOCOND_SubcooledLiquid).OR.(iThermodynamicCondition.EQ.THERMOCOND_SaturatedLiquid).OR. &
            (iThermodynamicCondition.EQ.THERMOCOND_LiquidVaporVLE).OR.(iThermodynamicCondition.EQ.THERMOCOND_SaturatedVapor)

        hasLiq: if(bLiquidPresent) then

            ! Calcular a massa específica da fase líquida:
            call CalculatePhaseDensity(iNComp, PHASE_Liquid, oLiqPhaseComposition, dP, dT, iLiqDensityCalculationMethod, oTc, oPc, oW, oKij, oLij, &
                      oPeneloux, dLiquidPhaseMW, dLiqDensity, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Calcular o fator de compressibilidade da fase líquida incluindo a influência do "shift volume" na massa específica:
            dLiquidZWithPeneloux = (dLiquidPhaseMW / 1000.0d0) * dP / (dLiqDensity * dR * dT)

            ! Calcular "Cp" e entalpia da fase líquida:
            call CalculatePhaseCpAndEnthalpy(iNComp, PHASE_Liquid, oLiqPhaseComposition, dP, dT, iLiqHAndCpThermodynamicModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, .true., dReferenceT, &
                    bLiqZAndDerivZWithTCalculated, dLiquidZ, dDerivLiquidZWithT, dLiquidMolarEnthalpy, dLiquidMolarCp, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Retornar a entalpia e o Cp MÁSSICOS, e não molares...
            dLiqMassEnthalpy = dLiquidMolarEnthalpy / (dLiquidPhaseMW / 1000.0d0)
            dLiqMassCp = dLiquidMolarCp / (dLiquidPhaseMW / 1000.0d0)

            ! Calcular derivadas de "Z" e de "rho" para a fase líquida:
            call CalculateTAndPDerivativesOfPhasicRhoAndZ(dP, dT, iLiqModel, PHASE_Liquid, iNComp, oLiqPhaseComposition, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dLiquidPhaseMW, dLiquidZ, dDerivLiquidZWithT, dDerivLiquidRhoWithT, dDerivLiquidZWithP, &
                    dDerivLiquidRhoWithP, dDerivLiquidZPenelouxWithP, dDerivLiquidZPenelouxWithT, iIER)

            if(iIER.NE.ERROR_EverythingOK) return
            bLiqZAndDerivZWithTCalculated = .true.

            ! Calcular o coeficiente de Joule-Thomson para a fase líquida, se possível:
            ! REFERÊNCIA "BIBLIOGRÁFICA": https://www.e-education.psu.edu/png520/m19_p3.html
            canLiqJTC: if(bLiqZAndDerivZWithTCalculated) then
                ! É possível calcular o coeficiente Joule-Thomson para a fase líquida!
                dLiqJouleThomsonCoef = dR * dT * dT / dP / dLiquidMolarCp * dDerivLiquidZPenelouxWithT
            else canLiqJTC
                ! Retornar valor "default" aleatório enquanto sabemos que esta possibilidade não ocorrerá.
                dLiqJouleThomsonCoef = -1000.0d0
            end if canLiqJTC

        else hasLiq
            ! Valores "default" aleatórios para as propriedades exclusivas da fase líquida:
            dLiqDensity = -1000.0d0
            dLiqMassEnthalpy = -1000.0d0
            dLiqMassCp = -1000.0d0
            dLiqJouleThomsonCoef = -1000.0d0
            dLiquidZ = -1000.0d0
            dDerivLiquidZWithT = -1000.0d0
            dDerivLiquidRhoWithT = -1000.0d0
            dDerivLiquidZWithP = -1000.0d0
            dDerivLiquidRhoWithP = -1000.0d0
            dLiquidZWithPeneloux = -1000.0d0
        end if hasLiq

        ! Calcular as propriedades da fase vapor, caso esta fase esteja presente:
        bVaporPresent = (iThermodynamicCondition.EQ.THERMOCOND_SaturatedLiquid).OR.(iThermodynamicCondition.EQ.THERMOCOND_LiquidVaporVLE).OR. &
            (iThermodynamicCondition.EQ.THERMOCOND_SaturatedVapor).OR.(iThermodynamicCondition.EQ.THERMOCOND_SuperheatedVapor)

        hasVap: if(bVaporPresent) then

            ! Calcular a massa específica da fase vapor:
            call CalculatePhaseDensity(iNComp, PHASE_Vapor, oVapPhaseComposition, dP, dT, iVapDensityCalculationMethod, oTc, oPc, oW, oKij, oLij, &
                      oPeneloux, dVaporPhaseMW, dVapDensity, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Calcular o fator de compressibilidade da fase vapor incluindo a influência do "shift volume" na massa específica:
            dVaporZWithPeneloux = (dVaporPhaseMW / 1000.0d0) * dP / (dVapDensity * dR * dT)

            ! Calcular "Cp" e entalpia da fase vapor:
            call CalculatePhaseCpAndEnthalpy(iNComp, PHASE_Vapor, oVapPhaseComposition, dP, dT, iVapHAndCpThermodynamicModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, .true., dReferenceT, &
                    bVapZAndDerivZWithTCalculated, dVaporZ, dDerivVaporZWithT, dVaporMolarEnthalpy, dVaporMolarCp, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Retornar a entalpia e o Cp MÁSSICOS, e não molares...
            dVapMassEnthalpy = dVaporMolarEnthalpy / (dVaporPhaseMW / 1000.0d0)
            dVapMassCp = dVaporMolarCp / (dVaporPhaseMW / 1000.0d0)

            ! Calcular derivadas de "Z" e de "rho" para a fase vapor:
            call CalculateTAndPDerivativesOfPhasicRhoAndZ(dP, dT, iVapModel, PHASE_Vapor, iNComp, oVapPhaseComposition, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dVaporPhaseMW, dVaporZ, dDerivVaporZWithT, dDerivVaporRhoWithT, dDerivVaporZWithP, &
                    dDerivVaporRhoWithP, dDerivVaporZPenelouxWithP, dDerivVaporZPenelouxWithT, iIER)

            if(iIER.NE.ERROR_EverythingOK) return
            bVapZAndDerivZWithTCalculated = .true.

            ! Calcular o coeficiente de Joule-Thomson para a fase vapor, se possível:
            canVapJTC: if(bVapZAndDerivZWithTCalculated) then
                ! É possível calcular o coeficiente Joule-Thomson para a fase vapor!
                dVapJouleThomsonCoef = dR * dT * dT / dP / dVaporMolarCp * dDerivVaporZPenelouxWithT
            else canVapJTC
                ! Retornar valor "default" aleatório enquanto sabemos que esta possibilidade não ocorrerá.
                dVapJouleThomsonCoef = -1000.0d0
            end if canVapJTC

        else hasVap
            ! Valores "default" aleatórios para as propriedades exclusivas da fase vapor:
            dVapDensity = -1000.0d0
            dVapMassEnthalpy = -1000.0d0
            dVapMassCp = -1000.0d0
            dVapJouleThomsonCoef = -1000.0d0
            dVaporZ = -1000.0d0
            dDerivVaporZWithT = -1000.0d0
            dDerivVaporRhoWithT = -1000.0d0
            dDerivVaporZWithP = -1000.0d0
            dDerivVaporRhoWithP = -1000.0d0
            dVaporZWithPeneloux = -1000.0d0
        end if hasVap

        ! Calcular propriedades que exijam a presença de ambas as fases:
        bothPhasesPresent: if(bLiquidPresent.AND.bVaporPresent) then

            ! Calcular a tensão superficial:
            call CalculateMixtureSurfaceTensionFromComponentParachors(iNComp, oParachors, dLiqDensity, dVapDensity, dLiquidPhaseMW, &
                    dVaporPhaseMW, oLiqPhaseComposition, oVapPhaseComposition, dSurfaceTension)

        else bothPhasesPresent
            ! Valores "default" aleatórios para propriedades de mistura bifásica:
            dSurfaceTension = -1000.0d0
        end if bothPhasesPresent

    end subroutine CalculatePhaseProperties_Approach1

    ! =============================================================
    ! =============================================================
    subroutine CalculateMolecularWeightsAndVaporMassFraction(iThermodynamicCondition, iNComp, oMW, oZ, oLiqPhaseComposition, &
        oVapPhaseComposition, dMolarBeta, dLiquidPhaseMW, dVaporPhaseMW, dMixtureMW, dVaporMassFraction)

        ! OBJETIVO: Calcular as massas molares das fases líquida e vapor e da mistura, bem como a fração mássica vaporizada de vapor.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iThermodynamicCondition    ! Condição termodinâmica da mistura.
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMW            ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oZ             ! Vetor composição global (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oLiqPhaseComposition    ! Vetor composição MOLAR do líquido (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oVapPhaseComposition    ! Vetor composição MOLAR do vapor (indexado por componente).
        real(c_double), value, intent(in) :: dMolarBeta                 ! Fração vaporizada em base molar.

        real(c_double), intent(out) :: dLiquidPhaseMW                   ! Massa molar da fase líquida (g/gmol).
        real(c_double), intent(out) :: dVaporPhaseMW                    ! Massa molar da fase vapor (g/gmol).
        real(c_double), intent(out) :: dMixtureMW                       ! Massa molar da mistura (g/gmol).
        real(c_double), intent(out) :: dVaporMassFraction               ! Fração mássica vaporizada.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bLiquidPresent, bVaporPresent
        integer :: i                                                    ! Índice de laços "do".

        ! ------------------ CÁLCULOS:

        ! Indicadores da presença de líquido e vapor:
        bLiquidPresent = (iThermodynamicCondition.EQ.THERMOCOND_SubcooledLiquid).OR.(iThermodynamicCondition.EQ.THERMOCOND_SaturatedLiquid).OR. &
            (iThermodynamicCondition.EQ.THERMOCOND_LiquidVaporVLE).OR.(iThermodynamicCondition.EQ.THERMOCOND_SaturatedVapor)

        bVaporPresent = (iThermodynamicCondition.EQ.THERMOCOND_SaturatedLiquid).OR.(iThermodynamicCondition.EQ.THERMOCOND_LiquidVaporVLE).OR. &
            (iThermodynamicCondition.EQ.THERMOCOND_SaturatedVapor).OR.(iThermodynamicCondition.EQ.THERMOCOND_SuperheatedVapor)

        ! Calcular as massas molares:
        dLiquidPhaseMW = 0.0d0
        dVaporPhaseMW = 0.0d0
        dMixtureMW = 0.0d0

        calcMW: do i = 1, iNComp

            if(bLiquidPresent) dLiquidPhaseMW = dLiquidPhaseMW + oLiqPhaseComposition(i) * oMW(i)

            if(bVaporPresent) dVaporPhaseMW = dVaporPhaseMW + oVapPhaseComposition(i) * oMW(i)

            dMixtureMW = dMixtureMW + oZ(i) * oMW(i)

        end do calcMW

        ! Converter a fração molar vaporizada para fração mássica vaporizada:
        if(.not.bVaporPresent) then
            dVaporMassFraction = 0.0d0
        else
            dVaporMassFraction = dMolarBeta * (dVaporPhaseMW / dMixtureMW)
        end if

    end subroutine CalculateMolecularWeightsAndVaporMassFraction

    ! =============================================================
    ! =============================================================
    subroutine CalculateMixtureSurfaceTensionFromComponentParachors(iNComp, oParachors, dRhoLiq, dRhoVap, dLiquidPhaseMW, &
                 dVaporPhaseMW, oLiqPhaseComposition, oVapPhaseComposition, dSurfaceTension)

        ! OBJETIVO: Calcular a tensão superficial de um sistema bifásico a partir das "parachors" dos componentes da mistura.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oParachors     ! Vetor de "parachors" para cada componente.
        real(c_double), value, intent(in) :: dRhoLiq                    ! Massa específica da fase líquida.
        real(c_double), value, intent(in) :: dRhoVap                    ! Massa específica da fase vapor.
        real(c_double), value, intent(in) :: dLiquidPhaseMW             ! Massa molar da fase líquida (g/gmol).
        real(c_double), value, intent(in) :: dVaporPhaseMW              ! Massa molar da fase vapor (g/gmol).
        real(c_double), dimension(iNComp), intent(in) :: oLiqPhaseComposition    ! Vetor composição MOLAR do líquido (indexado por componente).
        real(c_double), dimension(iNComp), intent(in) :: oVapPhaseComposition    ! Vetor composição MOLAR do vapor (indexado por componente).

        real(c_double), intent(out) :: dSurfaceTension                  ! Tensão superficial calculada.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i                                                ! Índice de laços "do".
        real(c_double) :: dDensL, dDensV

        ! ------------------ CÁLCULOS:

        ! Converter as unidades de massa específica para g/cm3:
        dDensL = dRhoLiq / 1000.0d0
        dDensV = dRhoVap / 1000.0d0

        ! Converter agora para gmol/cm3:
        dDensL = dDensL / dLiquidPhaseMW
        dDensV = dDensV / dVaporPhaseMW

        ! Calcular:
        dSurfaceTension = 0.0d0

        calcSigma: do i = 1, iNComp
            dSurfaceTension = dSurfaceTension + (oParachors(i) * (dDensL * oLiqPhaseComposition(i) - &
                    dDensV * oVapPhaseComposition(i)))
        end do calcSigma

        dSurfaceTension = dSurfaceTension ** 4.0d0

        ! Converter o resultado para o SI antes de retornar:
        dSurfaceTension = dSurfaceTension / 1000.0d0

    end subroutine CalculateMixtureSurfaceTensionFromComponentParachors

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseDensity(iNComp, iPhaseID, oPhaseComposition, dP, dT, iDensityCalculationMethod, oTc, oPc, oW, oKij, oLij, &
                      oPeneloux, dMW, dPhaseDensity, iIER)

        ! OBJETIVO: Calcular a massa específica de uma fase em escoamento.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cuja massa específica se deseja calcular, conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iDensityCalculationMethod  ! Índice representando o método de cálculo, conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), value, intent(in) :: dMW                        ! Massa molar da fase considerada (g/gmol)

        real(c_double), intent(out) :: dPhaseDensity                     ! Massa específica calculada.
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bUsingCubicEOS
        real(c_double) :: dLiquidDensity                    ! Massa específica calculada considerando a fase como líquida.
        real(c_double) :: dVaporDensity                     ! Massa específica calculada considerando a fase como vapor.

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        dPhaseDensity = -100.0d0

        ! O método selecionado é uma Equação de Estado Cúbica?
        bUsingCubicEOS = (iDensityCalculationMethod.EQ.PENG_ROBINSON_78_PENELOUX).or.(iDensityCalculationMethod.EQ.SRK_PENELOUX).or.(iDensityCalculationMethod.EQ.PENG_ROBINSON_PENELOUX)

        ! Proceder de acordo com o método selecionado:
        whichMethod: if(bUsingCubicEOS) then

            ! Calcular via Equação de Estado Cúbica:
            call CalculatePhaseDensityFromCubicEOS(iNComp, oPhaseComposition, dP, dT, iDensityCalculationMethod, oTc, oPc, oW, oKij, oLij, &
                                 oPeneloux, dMW, dLiquidDensity, dVaporDensity, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Selecionar a massa específica adequada:
            whichCubicRho: if(iPhaseID.EQ.PHASE_Liquid) then
                dPhaseDensity = dLiquidDensity
            else if(iPhaseID.EQ.PHASE_Vapor) then whichCubicRho
                dPhaseDensity = dVaporDensity
            end if whichCubicRho

        else whichMethod

            ! Método de cálculo não-reconhecido!
            iIER = ERROR_InvalidDensityCalculationMethod
            return

        end if whichMethod

    end subroutine CalculatePhaseDensity

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseCpAndEnthalpy(iNComp, iPhaseID, oPhaseComposition, dP, dT, iThermodynamicModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, bUseDefaultReferenceT, dReferenceT, &
                    bZAndDerivZWithTCalculated, dPhasicZ, dDerivPhasicZWithT, dPhaseEnthalpy, dPhaseCp, iIER)

        ! OBJETIVO: Calcular o "Cp" e a entalpia de uma fase em escoamento.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujas propriedades se deseja calcular, conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iThermodynamicModel        ! Indicador do modelo termodinâmico selecionado para o cálculo, conforme convencionado.
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iIGCoefsSource             ! Índice da fonte dos coeficientes polinomiais de gás ideal, conforme convenção.
        type(c_ptr), value, intent(in) :: oCpIGCoefs                    ! Matriz (se existir) dos coeficientes polinomiais do Cp de gás ideal.
        type(c_ptr), value, intent(in) :: oHIGCoefs                     ! Matriz (se existir) dos coeficientes polinomiais da entalpia de gás ideal.
        logical, intent(in) :: bUseDefaultReferenceT                    ! "True" para a rotina determinar o valor a usar de temperatura de referência.
        real(c_double), intent(inout) :: dReferenceT                    ! Temperatura de referência (fornecida ou determinada pela rotina).

        logical, intent(out) :: bZAndDerivZWithTCalculated              ! Retorna "true" se "Z" e "dZdT" foram obtidos no curso deste cálculo.
        real(c_double), intent(out) :: dPhasicZ                         ! Fator de compressibilidade calculado para a fase em questão.
        real(c_double), intent(out) :: dDerivPhasicZWithT               ! Derivada do fator de compressibilidade com a temperatura
        real(c_double), intent(out) :: dPhaseEnthalpy                   ! Entalpia calculada para a fase em questão.
        real(c_double), intent(out) :: dPhaseCp                         ! "Cp" calculado para a fase em questão.
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dResidualEnthalpy                             ! Entalpia residual calculada (unidades de "RT").
        real(c_double) :: dResidualCp                                   ! "Cp" residual calculado (unidades de "R").
        real(c_double) :: dIGEnthalpy                                   ! Entalpia de gás ideal calculada (unidades de "RT").
        real(c_double) :: dIGCp                                         ! "Cp" de gás ideal calculado (unidades de "R").

        ! ------------------ CÁLCULOS:

        ! Seguir a abordagem padrão --> calcular entalpia e "Cp" residuais:
        call CalculatePhaseResidualCpAndEnthalpy(iNComp, iPhaseID, oPhaseComposition, dP, dT, iThermodynamicModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dResidualEnthalpy, dResidualCp, bZAndDerivZWithTCalculated, dPhasicZ, dDerivPhasicZWithT, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular entalpia e "Cp" no estado de gás ideal:
        call CalculateMixtureIdealGasCpAndEnthalpy(iNComp, oPhaseComposition, dT, iIGCoefsSource, oCpIGCoefs, oHIGCoefs, &
                bUseDefaultReferenceT, dReferenceT, dIGEnthalpy, dIGCp, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Somando residual com gás ideal, obter finalmente as propriedades "reais":
        dPhaseEnthalpy = dResidualEnthalpy + dIGEnthalpy
        dPhaseCp = dResidualCp + dIGCp

    end subroutine CalculatePhaseCpAndEnthalpy

    ! =============================================================
    ! =============================================================
    subroutine CalculateMixtureIdealGasCpAndEnthalpy(iNComp, oMixComposition, dT, iCoefsSource, oCpIGCoefs, oHIGCoefs, &
            bUseDefaultReferenceT, dReferenceT, dIGEnthalpy, dIGCp, iIER)

        ! OBJETIVO: Calcular o "Cp" e a entalpia de uma mistura de componentes no estado de gás ideal.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oMixComposition    ! Vetor de frações molares da mistura (indexadas por componente).
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iCoefsSource               ! Índice referente à fonte de obtenção dos coeficientes polinomiais, conforme convenção.
        type(c_ptr), value, intent(in) :: oCpIGCoefs                    ! Matriz (se existir) dos coeficientes polinomiais do Cp de gás ideal.
        type(c_ptr), value, intent(in) :: oHIGCoefs                     ! Matriz (se existir) dos coeficientes polinomiais da entalpia de gás ideal.
        logical, intent(in) :: bUseDefaultReferenceT                    ! "True" para a rotina determinar o valor a usar de temperatura de referência.
        real(c_double), intent(inout) :: dReferenceT                    ! Temperatura de referência (fornecida ou determinada pela rotina).

        real(c_double), intent(out) :: dIGEnthalpy                      ! Entalpia de gás ideal calculada.
        real(c_double), intent(out) :: dIGCp                            ! "Cp" de gás ideal calculado.
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i
        real(c_double), dimension(:,:), pointer :: temp_oCpIGCoefs      ! Opção "default" de mapeamento do argumento "oCpIGCoefs".

        ! ------------------ CÁLCULOS:

        ! Proceder de acordo com a fonte de onde foram obtidos os coeficientes:
        whichSource: if(iCoefsSource.EQ.FLUIDEXTERNALSOURCE_PVTSIM_CTM) then

            ! Coeficientes (somente de Cp) obtidos do arquivo "*.ctm" do PVTSIM.
            ! Fornecem o "cP" em unidades de "R".

            ! Atribuir variável local no tamanho necessário:
            call c_f_pointer(oCpIGCoefs, temp_oCpIGCoefs, [4, iNComp])

            ! Temperatura de referência do PVTSIM?
            if(bUseDefaultReferenceT) dReferenceT = 273.15d0

            ! Calcular os polinômios:
            dIGEnthalpy = 0.0d0
            dIGCp = 0.0d0

            calcPvtSimPols: do i = 1, iNComp

                ! "Cp":
                dIGCp = dIGCp + oMixComposition(i) * ( &
                    temp_oCpIGCoefs(1, i) + &
                    temp_oCpIGCoefs(2, i) * dT + &
                    temp_oCpIGCoefs(3, i) * dT * dT + &
                    temp_oCpIGCoefs(4, i) * dT * dT * dT &
                    )

                ! "Entalpia":
                dIGEnthalpy = dIGEnthalpy + oMixComposition(i) * ( &
                    temp_oCpIGCoefs(1, i) * (dT - dReferenceT) + &
                    temp_oCpIGCoefs(2, i) / 2.0d0 * ((dT ** 2.0d0) - (dReferenceT ** 2.0d0)) + &
                    temp_oCpIGCoefs(3, i) / 3.0d0 * ((dT ** 3.0d0) - (dReferenceT ** 3.0d0)) + &
                    temp_oCpIGCoefs(4, i) / 4.0d0 * ((dT ** 4.0d0) - (dReferenceT ** 4.0d0)) &
                    )

            end do calcPvtSimPols

        else whichSource
            ! Fonte inválida de coeficientes!
            iIER = ERROR_InvalidSourceForIGCpOrHCoefs
            return
        end if whichSource

    end subroutine CalculateMixtureIdealGasCpAndEnthalpy

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseResidualCpAndEnthalpy(iNComp, iPhaseID, oPhaseComposition, dP, dT, iThermodynamicModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dResidualEnthalpy, dResidualCp, bZAndDerivZWithTCalculated, dPhasicZ, dDerivPhasicZWithT, iIER)

        ! OBJETIVO: Calcular o "Cp" e a entalpia residuais de uma fase em escoamento.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujos parâmetros se deseja calcular, conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iThermodynamicModel        ! Indicador do modelo termodinâmico selecionado para o cálculo, conforme convencionado.
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), intent(out) :: dResidualEnthalpy                ! Entalpia residual calculada.
        real(c_double), intent(out) :: dResidualCp                      ! "Cp" residual calculado.
        logical, intent(out) :: bZAndDerivZWithTCalculated              ! Retorna "true" se "Z" e "dZdT" foram obtidos no curso deste cálculo.
        real(c_double), intent(out) :: dPhasicZ                         ! Fator de compressibilidade calculado para a fase em questão.
        real(c_double), intent(out) :: dDerivPhasicZWithT               ! Derivada do fator de compressibilidade com a temperatura
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bThermodynamicModelIsCubicEOS                        ! "True" se o modelo termodinâmico escolhido for uma EOS cúbica.

        ! ------------------ CÁLCULOS:

        ! Inicializando:
        bZAndDerivZWithTCalculated = .false.

        ! O modelo termodinâmico selecionado é uma equação de estado cúbica?
        bThermodynamicModelIsCubicEOS = (iThermodynamicModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iThermodynamicModel.EQ.SRK_PENELOUX).or.(iThermodynamicModel.EQ.PENG_ROBINSON_PENELOUX)

        ! Proceder de acordo com o modelo termodinâmico selecionado:
        whichEnthalpyModel: if(bThermodynamicModelIsCubicEOS) then

            ! Calcular pela equação de estado cúbica selecionada!
            call CalculatePhaseResidualCpAndEnthalpyFromCubicEOS(iNComp, iPhaseID, oPhaseComposition, dP, dT, iThermodynamicModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dResidualEnthalpy, dResidualCp, dPhasicZ, dDerivPhasicZWithT, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            ! Sinalizar que "Z" e "dZ/dT" estão disponíveis:
            bZAndDerivZWithTCalculated = .true.

        else whichEnthalpyModel
            ! Modelo inválido para cálculo de entalpia e "Cp" residuais!
            iIER = ERROR_InvalidEnthalpyOrCpCalculationMethod
            return
        end if whichEnthalpyModel

    end subroutine CalculatePhaseResidualCpAndEnthalpy

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseResidualCpAndEnthalpyFromCubicEOS(iNComp, iPhaseID, oPhaseComposition, dP, dT, iCubicEOSModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dResidualEnthalpy, dResidualCp, dPhasicZ, dDerivPhasicZWithT, iIER)

        ! OBJETIVO: Calcular o "Cp" e a entalpia residual de uma fase em escoamento a partir de uma equação de estado cúbica.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujos parâmetros se deseja calcular, conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iCubicEOSModel             ! Indicador da EOS selecionada para este cálculo, conforme convencionado.
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), intent(out) :: dResidualEnthalpy                ! Entalpia residual calculada.
        real(c_double), intent(out) :: dResidualCp                      ! "Cp" residual calculado.
        real(c_double), intent(out) :: dPhasicZ                         ! Fator de compressibilidade calculado para a fase em questão.
        real(c_double), intent(out) :: dDerivPhasicZWithT               ! Derivada do fator de compressibilidade com a temperatura
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dAMix                                         ! Parâmetro "a" calculado para a mistura.
        real(c_double) :: dBMix                                         ! Parâmetro "b" calculado para a mistura.
        real(c_double) :: dCMix                                         ! Parâmetro "c" (volume de "shift") calculado para a mistura.
        real(c_double) :: dPhaseCapitalA                                ! "A" maiúsculo da fase considerada.
        real(c_double) :: dPhaseCapitalB                                ! "B" maiúsculo da fase considerada.
        real(c_double) :: dFLN                                          ! Logaritmo natural que aparece na expressão da entalpia residual.
        real(c_double) :: dDerivFLNWithT                                ! Derivada com a temperatura do logaritmo natural "FLN".
        real(c_double) :: dDerivAWithT                                  ! "da/dT" da fase
        real(c_double) :: dDerivTwoAWithTTwo                            ! "d2a/dT2" da fase
        real(c_double) :: dU                                            ! Parâmetro "u" da equação de estado
        real(c_double) :: dW                                            ! Parâmetro "w" da equação de estado
        real(c_double) :: dRootOfU2Minus4W

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                       ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! Obter "u" e "w" da equação de estado que se está utilizando:
        call GetCubicEOSGeneralFormParameters(iCubicEOSModel, dU, dW, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        dRootOfU2Minus4W = sqrt((dU ** 2.0d0) - (4.0d0 * dW))

        ! Obter as variáveis intermediárias necessárias:
        call CalculateCubicEOSParamsForPhasicEnthalpiesAndCp(iNComp, iPhaseID, oPhaseComposition, dP, dT, iCubicEOSModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dAMix, dBMix, dCMix, dPhasicZ, dDerivPhasicZWithT, dPhaseCapitalA, dPhaseCapitalB, &
                    dFLN, dDerivFLNWithT, dDerivAWithT, dDerivTwoAWithTTwo, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular a entalpia residual:
        dResidualEnthalpy = dAMix / dBMix / dRootOfU2Minus4W * dFLN
        dResidualEnthalpy = dResidualEnthalpy - (dT * dDerivAWithT / dBMix / dRootOfU2Minus4W * dFLN)
        dResidualEnthalpy = dResidualEnthalpy + dR * dT * (dPhasicZ - 1.0d0)

        ! Calcular o "Cp" residual:
        dResidualCp = dDerivAWithT / dBMix / dRootOfU2Minus4W * dFLN
        dResidualCp = dResidualCp + (dDerivFLNWithT * dAMix / dBMix / dRootOfU2Minus4W)
        dResidualCp = dResidualCp - (dDerivAWithT / dBMix / dRootOfU2Minus4W * dFLN)
        dResidualCp = dResidualCp - (dT * (dDerivTwoAWithTTwo / dBMix / dRootOfU2Minus4W * dFLN + dDerivFLNWithT * dDerivAWithT / dBMix / dRootOfU2Minus4W))
        dResidualCp = dResidualCp + dR * (dPhasicZ - 1.0d0)
        dResidualCp = dResidualCp + dR * dT * dDerivPhasicZWithT

    end subroutine CalculatePhaseResidualCpAndEnthalpyFromCubicEOS

    ! =============================================================
    ! =============================================================
    subroutine CalculateTAndPDerivativesOfPhasicRhoAndZ(dP, dT, iThermodynamicModel, iPhaseID, iNComp, oPhaseComposition, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dPhaseMW, dPhasicZ, dDerivPhasicZWithT, dDerivPhasicRhoWithT, dDerivPhasicZWithP, &
                    dDerivPhasicRhoWithP, dDerivPhasicZPenelouxWithP, dDerivPhasicZPenelouxWithT, iIER)

        ! OBJETIVO: Calcular, para dada fase indicada, suas derivadas de massa específica e de fator de compressibilidade
        !               com relação à temperatura e à pressão.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iThermodynamicModel        ! Indicador do modelo termodinâmico (EOS, etc) selecionado, conforme convencionado.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujas derivadas se deseja calcular, conforme convenção.
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), value, intent(in) :: dPhaseMW                   ! Massa molar da fase em questão (g/gmol).

        real(c_double), intent(out) :: dPhasicZ                         ! Fator de compressibilidade calculado para a fase em questão.
        real(c_double), intent(out) :: dDerivPhasicZWithT               ! Derivada do fator de compressibilidade com a temperatura
        real(c_double), intent(out) :: dDerivPhasicRhoWithT             ! Derivada da massa específica com a temperatura
        real(c_double), intent(out) :: dDerivPhasicZWithP               ! Derivada do fator de compressibilidade com a pressão.
        real(c_double), intent(out) :: dDerivPhasicRhoWithP             ! Derivada da massa específica com a pressão.
        real(c_double), intent(out) :: dDerivPhasicZPenelouxWithP       ! Derivada de "Z" com a pressão, incluindo "volume translation"
        real(c_double), intent(out) :: dDerivPhasicZPenelouxWithT       ! Derivada de "Z" com a temperatura, incluindo "volume translation"
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        logical :: bThermodynamicModelIsCubicEOS

        ! ------------------ PROCEDIMENTOS E CÁLCULOS:

        ! O modelo termodinâmico selecionado é uma equação de estado cúbica?
        bThermodynamicModelIsCubicEOS = (iThermodynamicModel.EQ.PENG_ROBINSON_78_PENELOUX).or.(iThermodynamicModel.EQ.SRK_PENELOUX).or.(iThermodynamicModel.EQ.PENG_ROBINSON_PENELOUX)

        whichThermoModel: if(bThermodynamicModelIsCubicEOS) then

            ! Calcular pela equação de estado cúbica selecionada!
            call CalculateCubicEOSTAndPDerivativesOfPhasicRhoAndZ(dP, dT, iThermodynamicModel, iPhaseID, iNComp, oPhaseComposition, oTc, oPc, oW, &
                        oKij, oLij, oPeneloux, dPhaseMW, dPhasicZ, dDerivPhasicZWithT, dDerivPhasicRhoWithT, dDerivPhasicZWithP, &
                        dDerivPhasicRhoWithP, dDerivPhasicZPenelouxWithP, dDerivPhasicZPenelouxWithT, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

        else whichThermoModel
            ! Modelo termodinâmico inválido para cálculo das derivadas!
            iIER = ERROR_InvalidZOrRhoTOrPDerivativesCalculationMethod
            return
        end if whichThermoModel

    end subroutine CalculateTAndPDerivativesOfPhasicRhoAndZ

    ! =============================================================
    ! =============================================================
    subroutine CalculateCubicEOSTAndPDerivativesOfPhasicRhoAndZ(dP, dT, iCubicEOSModel, iPhaseID, iNComp, oPhaseComposition, oTc, oPc, oW, &
                        oKij, oLij, oPeneloux, dPhaseMW, dPhasicZ, dDerivPhasicZWithT, dDerivPhasicRhoWithT, dDerivPhasicZWithP, &
                        dDerivPhasicRhoWithP, dDerivPhasicZPenelouxWithP, dDerivPhasicZPenelouxWithT, iIER)

        ! OBJETIVO: Calcular, para dada fase indicada, suas derivadas de massa específica e de fator de compressibilidade
        !               com relação à temperatura e à pressão, utilizando uma Equação Cúbica de Estado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iCubicEOSModel             ! Indicador da EOS selecionada para este cálculo, conforme convencionado.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujas derivadas se deseja calcular, conforme convenção.
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), value, intent(in) :: dPhaseMW                   ! Massa molar da fase em questão (g/gmol).

        real(c_double), intent(out) :: dPhasicZ                         ! Fator de compressibilidade calculado para a fase em questão.
        real(c_double), intent(out) :: dDerivPhasicZWithT               ! Derivada do fator de compressibilidade com a temperatura
        real(c_double), intent(out) :: dDerivPhasicRhoWithT             ! Derivada da massa específica com a temperatura
        real(c_double), intent(out) :: dDerivPhasicZWithP               ! Derivada do fator de compressibilidade com a pressão.
        real(c_double), intent(out) :: dDerivPhasicRhoWithP             ! Derivada da massa específica com a pressão.
        real(c_double), intent(out) :: dDerivPhasicZPenelouxWithP       ! Derivada de "Z" com a pressão, incluindo "volume translation"
        real(c_double), intent(out) :: dDerivPhasicZPenelouxWithT       ! Derivada de "Z" com a temperatura, incluindo "volume translation"
        integer(c_int), intent(out) :: iIER                             ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dAMix                                         ! Parâmetro "a" calculado para a mistura.
        real(c_double) :: dBMix                                         ! Parâmetro "b" calculado para a mistura.
        real(c_double) :: dCMix                                         ! Parâmetro "c" (volume de "shift") calculado para a mistura.
        real(c_double) :: dPhaseCapitalA                                ! "A" maiúsculo da fase considerada.
        real(c_double) :: dPhaseCapitalB                                ! "B" maiúsculo da fase considerada.
        real(c_double) :: dFLN                                          ! Logaritmo natural que aparece na expressão da entalpia residual.
        real(c_double) :: dDerivFLNWithT                                ! Derivada com a temperatura do logaritmo natural "FLN".
        real(c_double) :: dDerivAWithT                                  ! "da/dT" da fase
        real(c_double) :: dDerivTwoAWithTTwo                            ! "d2a/dT2" da fase
        real(c_double) :: dPhaseMW_SI                                   ! Massa molar no SI
        real(c_double) :: dDerivPhaseCapitalAWithP
        real(c_double) :: dDerivPhaseCapitalBWithP
        real(c_double) :: dCubicEOSCoef1, dCubicEOSCoef2
        real(c_double) :: dU                                            ! Parâmetro "u" da equação de estado
        real(c_double) :: dW                                            ! Parâmetro "w" da equação de estado
        real(c_double) :: dDerivEOSCoef1WithP, dDerivEOSCoef2WithP, dDerivEOSCoef3WithP

        real(c_double) :: dPhasicZWithPeneloux                          ! Fator de compressibilidade incluindo "volume translation"
        real(c_double) :: dDerivCMixWithP                               ! Derivada do "volume translation" com a pressão.
        real(c_double) :: dDerivCMixWithT                               ! Derivada do "volume translation" com a temperatura.
        real(c_double) :: dDerivPhasicRhoPenelouxWithP                  ! Derivada da massa específica com a pressão, incluindo "volume translation"
        real(c_double) :: dDerivPhasicRhoPenelouxWithT                  ! Derivada da massa específica com a temperatura, incluindo "volume translation"

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                       ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! Obter "u" e "w" da equação de estado que se está utilizando:
        call GetCubicEOSGeneralFormParameters(iCubicEOSModel, dU, dW, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Obter as variáveis intermediárias necessárias:
        call CalculateCubicEOSParamsForPhasicEnthalpiesAndCp(iNComp, iPhaseID, oPhaseComposition, dP, dT, iCubicEOSModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dAMix, dBMix, dCMix, dPhasicZ, dDerivPhasicZWithT, dPhaseCapitalA, dPhaseCapitalB, &
                    dFLN, dDerivFLNWithT, dDerivAWithT, dDerivTwoAWithTTwo, iIER)

        ! Calcular a derivada da massa específica da fase em questão com a temperatura:
        dPhaseMW_SI = dPhaseMW / 1000.0d0     ! para "kg/gmol" (lembrando que no SI, mol = gmol)

        dDerivPhasicRhoWithT = dP * dPhaseMW_SI / dR
        dDerivPhasicRhoWithT = dDerivPhasicRhoWithT / ((dPhasicZ * dT) ** 2.0d0)
        dDerivPhasicRhoWithT = dDerivPhasicRhoWithT * (dDerivPhasicZWithT * dT + dPhasicZ)
        dDerivPhasicRhoWithT = dDerivPhasicRhoWithT * (-1.0d0)

        ! Calcular as derivadas de "A" e "B" (maiúsculos) com a pressão:
        dDerivPhaseCapitalAWithP = dPhaseCapitalA / dP
        dDerivPhaseCapitalBWithP = dPhaseCapitalB / dP

        ! Calcular os coeficientes necessários da EOS cúbica:
        dCubicEOSCoef1 = -1.0d0 * (1.0d0 + dPhaseCapitalB - dU * dPhaseCapitalB)
        dCubicEOSCoef2 = dPhaseCapitalA + dW * (dPhaseCapitalB ** 2.0d0) - dU * dPhaseCapitalB - dU * (dPhaseCapitalB ** 2.0d0)

        ! Calcular as derivadas dos coeficientes da EOS cúbica em relação à pressão:
        dDerivEOSCoef1WithP = (dU - 1.0d0) * dDerivPhaseCapitalBWithP
        dDerivEOSCoef2WithP = dDerivPhaseCapitalAWithP + ((2.0d0 * dW - 2.0d0 * dU) * dPhaseCapitalB * dDerivPhaseCapitalBWithP) - (dU * dDerivPhaseCapitalBWithP)

        dDerivEOSCoef3WithP = (dPhaseCapitalB * dDerivPhaseCapitalAWithP) + &
                              (dPhaseCapitalA * dDerivPhaseCapitalBWithP) + &
                              (2.0d0 * dW * dPhaseCapitalB * dDerivPhaseCapitalBWithP) + &
                              (3.0d0 * dW * (dPhaseCapitalB ** 2.0d0) * dDerivPhaseCapitalBWithP)
        dDerivEOSCoef3WithP = -1.0d0 * dDerivEOSCoef3WithP

        ! Calcular a derivada do fator de compressibilidade com a pressão:
        dDerivPhasicZWithP = (dPhasicZ * dPhasicZ * dDerivEOSCoef1WithP) + (dPhasicZ * dDerivEOSCoef2WithP) + dDerivEOSCoef3WithP
        dDerivPhasicZWithP = dDerivPhasicZWithP / (3.0d0 * dPhasicZ * dPhasicZ + (2.0d0 * dCubicEOSCoef1 * dPhasicZ) + dCubicEOSCoef2)
        dDerivPhasicZWithP = -1.0d0 * dDerivPhasicZWithP

        ! Calcular a derivada da massa específica com a pressão:
        dDerivPhasicRhoWithP = dPhaseMW_SI / dR / dT
        dDerivPhasicRhoWithP = dDerivPhasicRhoWithP / (dPhasicZ * dPhasicZ)
        dDerivPhasicRhoWithP = dDerivPhasicRhoWithP * (dPhasicZ - (dDerivPhasicZWithP * dP))

        ! Até o momento, nesta biblioteca, supõe-se que o parâmetro de "volume translation" é constante.
        ! Logo...
        dDerivCMixWithP = 0.0d0
        dDerivCMixWithT = 0.0d0

        ! Incluir "volume translation" no "Z":
        dPhasicZWithPeneloux = dPhasicZ - (dCMix * dP / dR / dT)

        ! Incluir "volume translation" nas derivadas de "Z":
        dDerivPhasicZPenelouxWithP = dDerivPhasicZWithP - &
                (dP * dDerivCMixWithP + dCMix) / dR / dT

        dDerivPhasicZPenelouxWithT = dDerivPhasicZWithT - &
            ((dDerivCMixWithT * dT - dCMix) / dT / dT) * dP / dR

        ! Incluir "volume translation" nas derivadas da massa específica:
        dDerivPhasicRhoPenelouxWithP = dPhaseMW_SI / dR / dT
        dDerivPhasicRhoPenelouxWithP = dDerivPhasicRhoPenelouxWithP / (dPhasicZWithPeneloux * dPhasicZWithPeneloux)
        dDerivPhasicRhoPenelouxWithP = dDerivPhasicRhoPenelouxWithP * (dPhasicZWithPeneloux - (dDerivPhasicZPenelouxWithP * dP))

        dDerivPhasicRhoPenelouxWithT = dP * dPhaseMW_SI / dR
        dDerivPhasicRhoPenelouxWithT = dDerivPhasicRhoPenelouxWithT / ((dPhasicZWithPeneloux * dT) ** 2.0d0)
        dDerivPhasicRhoPenelouxWithT = dDerivPhasicRhoPenelouxWithT * (dDerivPhasicZPenelouxWithT * dT + dPhasicZWithPeneloux)
        dDerivPhasicRhoPenelouxWithT = dDerivPhasicRhoPenelouxWithT * (-1.0d0)

        ! Retonar sempre as derivadas da massa específica com os efeitos de "volume translation" considerados:
        dDerivPhasicRhoWithP = dDerivPhasicRhoPenelouxWithP
        dDerivPhasicRhoWithT = dDerivPhasicRhoPenelouxWithT

    end subroutine CalculateCubicEOSTAndPDerivativesOfPhasicRhoAndZ

    ! =============================================================
    ! =============================================================
    subroutine CalculateCubicEOSParamsForPhasicEnthalpiesAndCp(iNComp, iPhaseID, oPhaseComposition, dP, dT, iCubicEOSModel, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, dAMix, dBMix, dCMix, dPhasicZ, dDerivPhasicZWithT, dPhaseCapitalA, dPhaseCapitalB, &
                    dFLN, dDerivFLNWithT, dDerivAWithT, dDerivTwoAWithTTwo, iIER)

        ! OBJETIVO: Calcular, para dada fase indicada, diversas variáveis que são comuns aos cálculos de "Cp" e
        !               da entalpia através de uma Equação Cúbica de Estado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer(c_int), value, intent(in) :: iPhaseID                   ! Identificador da fase cujos parâmetros se deseja calcular, conforme convenção.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iCubicEOSModel             ! Indicador da EOS selecionada para este cálculo, conforme convencionado.
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.

        real(c_double), intent(out) :: dAMix                             ! Parâmetro "a" calculado para a mistura.
        real(c_double), intent(out) :: dBMix                             ! Parâmetro "b" calculado para a mistura.
        real(c_double), intent(out) :: dCMix                             ! Parâmetro "c" (volume de "shift") calculado para a mistura.
        real(c_double), intent(out) :: dPhasicZ                          ! Fator de compressibilidade calculado para a fase em questão.
        real(c_double), intent(out) :: dDerivPhasicZWithT                ! Derivada do fator de compressibilidade com a temperatura
        real(c_double), intent(out) :: dPhaseCapitalA                    ! "A" maiúsculo da fase considerada.
        real(c_double), intent(out) :: dPhaseCapitalB                    ! "B" maiúsculo da fase considerada.
        real(c_double), intent(out) :: dFLN                              ! Logaritmo natural que aparece na expressão da entalpia residual.
        real(c_double), intent(out) :: dDerivFLNWithT                    ! Derivada com a temperatura do logaritmo natural "FLN".
        real(c_double), intent(out) :: dDerivAWithT                      ! "da/dT" da fase
        real(c_double), intent(out) :: dDerivTwoAWithTTwo                ! "d2a/dT2" da fase
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: i, j
        real(c_double), dimension(iNComp) :: oCubicEOSaParameters       ! Vetor de parâmetros "a" calculados (indexado por componente)
        real(c_double), dimension(iNComp) :: oCubicEOSbParameters       ! Vetor de parâmetros "b" calculados (indexado por componente)
        real(c_double) :: dZLiquid                                      ! Fator de compressibilidade da fase líquida.
        real(c_double) :: dZVapor                                       ! Fator de compressibilidade da fase vapor.
        logical :: bComplexRootsWereFound                               ! "True" se foram encontrados fatores de compressibilidade complexos.
        real(c_double) :: dU                                            ! Parâmetro "u" da equação de estado
        real(c_double) :: dW                                            ! Parâmetro "w" da equação de estado
        real(c_double) :: dRootOfU2Minus4W
        real(c_double), dimension(iNComp) :: oFW                        ! Função do fator acêntrico de cada componente, de acordo com a EOS Cúbica.
        real(c_double) :: dCubicEOSGammaA
        real(c_double) :: dCubicEOSCoef1, dCubicEOSCoef2, dCubicEOSCoef3
        real(c_double) :: dDerivPhaseCapitalAWithT, dDerivPhaseCapitalBWithT
        real(c_double) :: dDerivEOSCoef1WithT, dDerivEOSCoef2WithT, dDerivEOSCoef3WithT
        logical :: bEOSIsPengRobinsonOrSRK                              ! "True" se a EOS cúbica é "Peng-Robinson" ou "SRK"
        real(c_double) :: dComponentPsiFunction, dComponentAlfaFunction, dComponentAc
        real(c_double), dimension(iNComp) :: oTDerivOfRootShortAsOverT  ! Vetor de "d/dT[sqrt(ai/T)]" dos componentes da fase.

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                   ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! Valor de constantes dependentes da equação de estado:
        bEOSIsPengRobinsonOrSRK = .false.

        EOSDependent: if(iCubicEOSModel.EQ.PENG_ROBINSON_78_PENELOUX) then

            dCubicEOSGammaA = 0.45724d0

            bEOSIsPengRobinsonOrSRK = .true.

        else if(iCubicEOSModel.EQ.PENG_ROBINSON_PENELOUX) then EOSDependent

            dCubicEOSGammaA = 0.45724d0

            bEOSIsPengRobinsonOrSRK = .true.

        else if(iCubicEOSModel.EQ.SRK_PENELOUX) then EOSDependent

            dCubicEOSGammaA = 0.42748d0

            bEOSIsPengRobinsonOrSRK = .true.

        else EOSDependent
            ! Erro!
            iIER = ERROR_InvalidCubicEOS
            return
        end if EOSDependent

        ! Determinar as constantes dos componentes puros:
        call CalculateCubicEOSParametersForEachComponent(iNComp, dT, iCubicEOSModel, oTc, oPc, oW, &
                                                   iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Determinar as constantes da mistura:
        call CalculateCubicEOSParametersForMixture(iNComp, oPhaseComposition, oKij, oLij, oCubicEOSaParameters, oCubicEOSbParameters, &
                                                       oPeneloux, dAMix, dBMix, dCMix)

        ! Calcular o fator de compressibilidade:
        call CalculateZFromCubicEOS(dP, dT, iCubicEOSModel, dAMix, dBMix, dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! NOTA: Será considerado aqui que a equação cúbica pode estar em uma região monotônica, possuindo apenas uma raíz real.
        !       Consequência imediata: está sendo admitida a existência de raízes complexas sem que isso configure erro!

        ! Armazenar no argumento de saída o fator de compressibilidade da fase em questão:
        whichZ: if(iPhaseID.EQ.PHASE_Liquid) then
            dPhasicZ = dZLiquid
        else if(iPhaseID.EQ.PHASE_Vapor) then whichZ
            dPhasicZ = dZVapor
        end if whichZ

        ! Calcular "A" e "B" da fase:
        dPhaseCapitalA = dAMix * dP / ((dR * dT) ** 2.0d0)
        dPhaseCapitalB = dBMix * dP / dR / dT

        ! Obter "u" e "w" da equação de estado que se está utilizando:
        call GetCubicEOSGeneralFormParameters(iCubicEOSModel, dU, dW, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular agora o logaritmo natural:
        dRootOfU2Minus4W = sqrt((dU ** 2.0d0) - (4.0d0 * dW))

        dFLN = 2.0d0 * dPhasicZ + dPhaseCapitalB * (dU + dRootOfU2Minus4W)
        dFLN = (2.0d0 * dPhasicZ + dPhaseCapitalB * (dU - dRootOfU2Minus4W)) / dFLN
        dFLN = log(dFLN)

        ! Fazer cálculos envolvendo apenas um "loop" sobre componentes:
        simpleLoop: do i = 1, iNComp

            ! Cálculo de "fW"...
            whichFWFormula: if(iCubicEOSModel.EQ.PENG_ROBINSON_78_PENELOUX) then

                ! "fW" por PR 78
                if(oW(i).LE.(0.491d0)) then
                    oFW(i) = 0.37464d0 + (1.54226d0 * oW(i)) - (0.26992d0 * (oW(i) ** 2.0d0))
                else
                    oFW(i) = 0.379642d0 + (1.48503d0 * oW(i)) - (0.164423d0 * (oW(i) ** 2.0d0)) + (0.016666d0 * (oW(i) ** 3.0d0))
                end if

            else if(iCubicEOSModel.EQ.PENG_ROBINSON_PENELOUX) then whichFWFormula

                ! "fW" por PR
                oFW(i) = 0.37464d0 + (1.54226d0 * oW(i)) - (0.26992d0 * (oW(i) ** 2.0d0))

            else if(iCubicEOSModel.EQ.SRK_PENELOUX) then whichFWFormula

                ! "fW" por SRK
                oFW(i) = 0.48d0 + (1.574d0 * oW(i)) - (0.176d0 * oW(i) * oW(i))

            else whichFWFormula
                ! Erro!
                iIER = ERROR_InvalidCubicEOS
                return
            end if whichFWFormula

            ! "aC" do componente:
            dComponentAc = dCubicEOSGammaA * (dR ** 2.0d0) * (oTc(i) ** 2.0d0) / oPc(i)

            ! Válidos para "Peng-Robinson" ou "SRK"...
            isPRorSRK: if(bEOSIsPengRobinsonOrSRK) then

                dComponentAlfaFunction = (1.0d0 + oFW(i) * (1.0d0 - sqrt(dT / oTc(i)))) ** 2.0d0

                dComponentPsiFunction = oFW(i) / 2.0d0 / sqrt(oTc(i) * dComponentAlfaFunction * dT) + 0.5d0 / dT
                dComponentPsiFunction = -1.0d0 * dComponentPsiFunction

                oTDerivOfRootShortAsOverT(i) = sqrt(dCubicEOSGammaA) * dR * oTc(i) * dComponentPsiFunction * sqrt(dComponentAlfaFunction)
                oTDerivOfRootShortAsOverT(i) = oTDerivOfRootShortAsOverT(i) / sqrt(oPc(i) * dT)

            else isPRorSRK
                ! Erro!
                iIER = ERROR_InvalidCubicEOS
                return
            end if isPRorSRK

        end do simpleLoop

        ! Preparar somatórios duplos em todos os componentes:
        dDerivAWithT = 0.0d0
        dDerivTwoAWithTTwo = 0.0d0

        doubleSumOuter: do i = 1, iNComp

            doubleSumInner: do j = 1, iNComp

                ! Somatório duplo da derivada de "a" com "T":
                dDerivAWithT = dDerivAWithT + &
                    oPhaseComposition(i) * oPhaseComposition(j) * (1.0d0 - oKij(i, j)) * &
                    (oFW(j)*sqrt(oCubicEOSaParameters(i)*oTc(j)/oPc(j)) + &
                     oFW(i)*sqrt(oCubicEOSaParameters(j)*oTc(i)/oPc(i)))

                ! Somatório duplo da segunda derivada de "a" com "T"
                dDerivTwoAWithTTwo = dDerivTwoAWithTTwo + &
                    oPhaseComposition(i) * oPhaseComposition(j) * (1.0d0 - oKij(i, j)) * &
                    (oFW(j) * sqrt(oTc(j)/oPc(j)) * oTDerivOfRootShortAsOverT(i) + &
                     oFW(i) * sqrt(oTc(i)/oPc(i)) * oTDerivOfRootShortAsOverT(j))

            end do doubleSumInner

        end do doubleSumOuter

        ! Complementando cálculos:
        dDerivAWithT = -dR * 0.50d0 * sqrt(dCubicEOSGammaA / dT) * dDerivAWithT

        dDerivTwoAWithTTwo = -dR * 0.50d0 * sqrt(dCubicEOSGammaA) * dDerivTwoAWithTTwo

        ! Calcular a derivada de "Z" (fator de compressibilidade) com a temperatura:
        dCubicEOSCoef1 = -1.0d0 * (1.0d0 + dPhaseCapitalB - dU * dPhaseCapitalB)
        dCubicEOSCoef2 = dPhaseCapitalA + dW * (dPhaseCapitalB ** 2.0d0) - dU * dPhaseCapitalB - dU * (dPhaseCapitalB ** 2.0d0)
        dCubicEOSCoef3 = -1.0d0 * dPhaseCapitalA * dPhaseCapitalB - dW * (dPhaseCapitalB ** 2.0d0) - dW * (dPhaseCapitalB ** 3.0d0)

        dDerivPhaseCapitalAWithT = dPhaseCapitalA / dAMix * dDerivAWithT - 2.0d0 * dPhaseCapitalA / dT
        dDerivPhaseCapitalBWithT = -1.0d0 * dPhaseCapitalB / dT

        dDerivEOSCoef1WithT = (dU - 1.0d0) * dDerivPhaseCapitalBWithT
        dDerivEOSCoef2WithT = dDerivPhaseCapitalAWithT + 2.0d0 * (dW - dU) * dPhaseCapitalB * dDerivPhaseCapitalBWithT - dU * dDerivPhaseCapitalBWithT
        dDerivEOSCoef3WithT = -1.0d0 * (dPhaseCapitalB * dDerivPhaseCapitalAWithT + dPhaseCapitalA * dDerivPhaseCapitalBWithT + &
                2.0d0 * dW * dPhaseCapitalB * dDerivPhaseCapitalBWithT  + 3.0d0 * dW * (dPhaseCapitalB ** 2.0d0) * dDerivPhaseCapitalBWithT)

        dDerivPhasicZWithT = (dPhasicZ ** 2.0d0) * dDerivEOSCoef1WithT + dPhasicZ * dDerivEOSCoef2WithT + dDerivEOSCoef3WithT
        dDerivPhasicZWithT = -1.0d0 * dDerivPhasicZWithT / (3.0d0 * (dPhasicZ ** 2.0d0) + 2.0d0 * dCubicEOSCoef1 * dPhasicZ + dCubicEOSCoef2)

        ! Calcular agora a derivada do logaritmo natural com a temperatura:
        dDerivFLNWithT = 2.0d0 * dDerivPhasicZWithT + (dU - dRootOfU2Minus4W) * dDerivPhaseCapitalBWithT
        dDerivFLNWithT = dDerivFLNWithT / (2.0d0 * dPhasicZ + dPhaseCapitalB * (dU - dRootOfU2Minus4W))
        dDerivFLNWithT = dDerivFLNWithT - ((2.0d0 * dDerivPhasicZWithT + (dU + dRootOfU2Minus4W) * dDerivPhaseCapitalBWithT) / &
                                           (2.0d0 * dPhasicZ + dPhaseCapitalB * (dU + dRootOfU2Minus4W)))

    end subroutine CalculateCubicEOSParamsForPhasicEnthalpiesAndCp

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseDensityFromCubicEOS(iNComp, oPhaseComposition, dP, dT, iCubicEOSModel, oTc, oPc, oW, oKij, oLij, &
                                 oPeneloux, dMW, dLiquidDensity, dVaporDensity, iIER)

        ! OBJETIVO: Calcular a massa específica de uma fase qualquer a partir de uma equação cúbica de estado.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPhaseComposition    ! Vetor de frações molares da fase (indexadas por componente).
        real(c_double), value, intent(in) :: dP                         ! Pressão
        real(c_double), value, intent(in) :: dT                         ! Temperatura
        integer(c_int), value, intent(in) :: iCubicEOSModel             ! Indicador da EOS selecionada para este cálculo, conforme convencionado.
        real(c_double), dimension(iNComp), intent(in) :: oTc            ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc            ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW             ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij    ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij    ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux      ! Vetor dos "volumes de shift" para cada componente.
        real(c_double), value, intent(in) :: dMW                        ! Massa molar da fase considerada (g/gmol)

        real(c_double), intent(out) :: dLiquidDensity                    ! Massa específica calculada considerando a fase como líquida.
        real(c_double), intent(out) :: dVaporDensity                     ! Massa específica calculada considerando a fase como vapor.
        integer(c_int), intent(out) :: iIER                              ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oCubicEOSaParameters       ! Vetor de parâmetros "a" calculados (indexado por componente)
        real(c_double), dimension(iNComp) :: oCubicEOSbParameters       ! Vetor de parâmetros "b" calculados (indexado por componente)
        real(c_double) :: dAMix                                         ! Parâmetro "a" calculado para a mistura.
        real(c_double) :: dBMix                                         ! Parâmetro "b" calculado para a mistura.
        real(c_double) :: dCMix                                         ! Parâmetro "c" (volume de "shift") calculado para a mistura.
        real(c_double) :: dZLiquid                                      ! Fator de compressibilidade da fase líquida.
        real(c_double) :: dZVapor                                       ! Fator de compressibilidade da fase vapor.
        logical :: bComplexRootsWereFound                               ! "True" se foram encontrados fatores de compressibilidade complexos.

        ! ------------------ CÁLCULOS:

        ! Determinar as constantes dos componentes puros:
        call CalculateCubicEOSParametersForEachComponent(iNComp, dT, iCubicEOSModel, oTc, oPc, oW, &
                                                   iIER, oCubicEOSaParameters, oCubicEOSbParameters)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Determinar as constantes da mistura:
        call CalculateCubicEOSParametersForMixture(iNComp, oPhaseComposition, oKij, oLij, oCubicEOSaParameters, oCubicEOSbParameters, &
                                                       oPeneloux, dAMix, dBMix, dCMix)

        ! Calcular o fator de compressibilidade:
        call CalculateZFromCubicEOS(dP, dT, iCubicEOSModel, dAMix, dBMix, dZLiquid, dZVapor, bComplexRootsWereFound, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! NOTA: Será considerado aqui que a equação cúbica pode estar em uma região monotônica, possuindo apenas uma raíz real.
        !       Consequência imediata: está sendo admitida a existência de raízes complexas sem que isso configure erro!

        ! Calcular as massas específicas considerando os dois fatores de compressibilidade:
        call CalculatePhaseDensityFromZAndC(dZLiquid, dT, dP, dCMix, dMW, dLiquidDensity)
        call CalculatePhaseDensityFromZAndC(dZVapor, dT, dP, dCMix, dMW, dVaporDensity)

    end subroutine CalculatePhaseDensityFromCubicEOS

    ! =============================================================
    ! =============================================================
    subroutine CalculatePhaseDensityFromZAndC(dZ, dT, dP, dC, dMW, dPhaseDensity)

        ! OBJETIVO: Calcular a massa específica de uma fase qualquer, dados seu fator de compressibilidade
        !           e o "volume de shift" a aplicar.
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dZ                     ! Fator de compressibilidade
        real(c_double), value, intent(in) :: dT                     ! Temperatura
        real(c_double), value, intent(in) :: dP                     ! Pressão
        real(c_double), value, intent(in) :: dC                     ! "Volume translation parameter" (como volume molar) a aplicar (usar ZERO para não corrigir).
        real(c_double), value, intent(in) :: dMW                    ! Massa molar (g/gmol)

        real(c_double), intent(out) :: dPhaseDensity                ! Massa específica calculada.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dMolarVolume
        real(c_double) :: dMW_SI                                    ! Massa molar no SI
        real(c_double) :: dNewZ

        ! ------------------ CONSTANTES:
        real(c_double), parameter :: dR = 8.314d0                   ! R no SI (J/mol/K)

        ! ------------------ CÁLCULOS:

        ! Observação: Da página 208 do livro do Riazi, "Characterization and Properties of Petroleum Fractions": "Equation
        !               5.50 can be applied to both vapor and liquid volumes".

        ! A partir do fator de compressibilidade, calcular o volume molar:
        dMolarVolume = dZ * dR * dT / dP

        ! Esse é o volume molar calculado pela equação de estado que foi usada.
        ! Aplicar a correção prescrita:
        dMolarVolume = dMolarVolume - dC

        dNewZ = dP * dMolarVolume / dR / dT

        ! Volume corrigido. Obter, a partir dele, a massa específica.
        dMW_SI = dMW / 1000.0d0     ! para "kg/gmol"

        dPhaseDensity = dMW_SI * dP / (dNewZ * dR * dT)

    end subroutine CalculatePhaseDensityFromZAndC

end module PhaseProperties
