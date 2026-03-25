module Newton

    ! OBJETIVO: Armazenar as rotinas que aplicam o Método de Newton aos problemas do escopo desta
    !               biblioteca.

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use Constants
    !use FlashExtras
    !use FlashExtras, only: CalculateLnFugAndDerivativesForEachComponent, SolveRachfordRiceEquation, &
    !        PerformTangentPlaneAnalysisForVLE, GetResultsForSinglePhaseFeed, CalculateRelativeMolarGibbsEnergy, &
    !        CalculateTPD

    implicit none

    ! "Constantes" booleanas para controle:
    logical, parameter :: bAllowNewtonMinimization = .true.      ! Permitir (ou não permitir ) que todo e qualquer cálculo recorra ao método de Newton.
    logical, parameter :: bAllowFlashNewtonMinimization = .true. ! Permitir ou não que o "flash" (e somente ele) seja resolvido pelo Método de Newton.
    logical, parameter :: bAllowTPDNewtonMinimization = .true.   ! Permitir ou não que a verificação de estabilidade seja resolvida pelo Método de Newton.

    ! ---------------- INTERFACES DE ROTINAS DO FlashExtras.f90

    ! Interfaces necessárias para eliminar dependência circular entre este arquivo e o "FlashExtras.f90"
    ! (essa interdependência por meio de comandos "use" estava causando problemas de compilação).

    interface
        subroutine CalculateLnFugAndDerivativesForEachComponent(iNComp, iPhaseID, dTemperature, dPressure, oPhasicComposition, &
             iPhasicThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg, oDerivLnCoefFugWithNjArg) &
             bind(C, name='CalculateLnFugAndDerivativesForEachComponent')
            use iso_c_binding
            implicit none
            integer(c_int), value, intent(in) :: iNComp
            integer(c_int), value, intent(in) :: iPhaseID
            real(c_double), value, intent(in) :: dTemperature
            real(c_double), value, intent(in) :: dPressure
            real(c_double), dimension(iNComp), intent(in) :: oPhasicComposition
            integer(c_int), value, intent(in) :: iPhasicThermodynamicModel
            real(c_double), dimension(iNComp), intent(in) :: oTc
            real(c_double), dimension(iNComp), intent(in) :: oPc
            real(c_double), dimension(iNComp), intent(in) :: oW
            real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij
            real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij
            real(c_double), dimension(iNComp), intent(in) :: oPeneloux
            integer(c_int), intent(out) :: iIER
            real(c_double), dimension(iNComp), intent(out), optional :: oLnFugArg
            real(c_double), dimension(iNComp, iNComp), intent(out), optional :: oDerivLnCoefFugWithNjArg
        end subroutine CalculateLnFugAndDerivativesForEachComponent

        subroutine SolveRachfordRiceEquation(iNComp, oZ, oK, bSolutionExists, dBetaVap, dBetaLiq, oXArg, oYArg) &
            bind(C, name='SolveRachfordRiceEquation')
            use iso_c_binding
            implicit none
            integer(c_int), value, intent(in) :: iNComp
            real(c_double), dimension(iNComp), intent(in) :: oZ
            real(c_double), dimension(iNComp), intent(in) :: oK
            logical, intent(out) :: bSolutionExists
            real(c_double), intent(out), optional :: dBetaVap
            real(c_double), intent(out), optional :: dBetaLiq
            real(c_double), dimension(iNComp), intent(out), optional :: oXArg
            real(c_double), dimension(iNComp), intent(out), optional :: oYArg
        end subroutine SolveRachfordRiceEquation

        subroutine PerformTangentPlaneAnalysisForVLE(dTemperature, dPressure, iNComp, oZ, oMixtureLnFug, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                iLiqPhaseModel, iVapPhaseModel, iLeastGPhaseID, bTPDAnalysisSuccessfullyDone, bFeedIsStableAsSinglePhaseArg, &
                oLiqStationaryPointWArg, oLiqStationaryPointLnFugArg, oVapStationaryPointWArg, oVapStationaryPointLnFugArg, oImprovedKFactorsArg) &
                bind(C, name='PerformTangentPlaneAnalysisForVLE')
            use iso_c_binding
            implicit none
            real(c_double), value, intent(in) :: dTemperature
            real(c_double), value, intent(in) :: dPressure
            integer(c_int), value, intent(in) :: iNComp
            real(c_double), dimension(iNComp), intent(in) :: oZ
            real(c_double), dimension(iNComp), intent(in) :: oMixtureLnFug
            real(c_double), dimension(iNComp), intent(in) :: oTc
            real(c_double), dimension(iNComp), intent(in) :: oPc
            real(c_double), dimension(iNComp), intent(in) :: oW
            real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij
            real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij
            real(c_double), dimension(iNComp), intent(in) :: oPeneloux
            integer(c_int), value, intent(in) :: iLiqPhaseModel
            integer(c_int), value, intent(in) :: iVapPhaseModel
            integer(c_int), value, intent(in) :: iLeastGPhaseID
            logical, intent(out) :: bTPDAnalysisSuccessfullyDone
            logical, intent(out), optional :: bFeedIsStableAsSinglePhaseArg
            real(c_double), dimension(iNComp), intent(out), optional :: oLiqStationaryPointWArg
            real(c_double), dimension(iNComp), intent(out), optional :: oLiqStationaryPointLnFugArg
            real(c_double), dimension(iNComp), intent(out), optional :: oVapStationaryPointWArg
            real(c_double), dimension(iNComp), intent(out), optional :: oVapStationaryPointLnFugArg
            real(c_double), dimension(iNComp), intent(out), optional :: oImprovedKFactorsArg
        end subroutine PerformTangentPlaneAnalysisForVLE

        subroutine GetResultsForSinglePhaseFeed(iNComp, oZ, iPhaseID, dBetaVap, oX, oY) &
            bind(C, name='GetResultsForSinglePhaseFeed')
            use iso_c_binding
            implicit none
            integer(c_int), value, intent(in) :: iNComp
            real(c_double), dimension(iNComp), intent(in) :: oZ
            integer(c_int), value, intent(in) :: iPhaseID
            real(c_double), intent(out) :: dBetaVap
            real(c_double), dimension(iNComp), intent(out) :: oX
            real(c_double), dimension(iNComp), intent(out) :: oY
        end subroutine GetResultsForSinglePhaseFeed

        subroutine CalculateRelativeMolarGibbsEnergy(iNComp, oMolarFractions, oLnFugs, dGibbsEnergy) &
            bind(C, name='CalculateRelativeMolarGibbsEnergy')
            use iso_c_binding
            implicit none
            integer(c_int), value, intent(in) :: iNComp
            real(c_double), dimension(iNComp), intent(in) :: oMolarFractions
            real(c_double), dimension(iNComp), intent(in) :: oLnFugs
            real(c_double), intent(out) :: dGibbsEnergy
        end subroutine CalculateRelativeMolarGibbsEnergy

        subroutine CalculateTPD(iNComp, oTrialPhaseW, oTrialPhaseLnFug, oD, dTPD) &
            bind(C, name='CalculateTPD')
            use iso_c_binding
            implicit none
            integer(c_int), value, intent(in) :: iNComp
            real(c_double), dimension(iNComp), intent(in) :: oTrialPhaseW
            real(c_double), dimension(iNComp), intent(in) :: oTrialPhaseLnFug
            real(c_double), dimension(iNComp), intent(in) :: oD
            real(c_double), intent(out) :: dTPD
        end subroutine CalculateTPD

    end interface

    ! ---------------- FIM DAS INTERFACES DE ROTINAS DO FlashExtras.f90

    contains

    ! =============================================================
    ! =============================================================
    subroutine NewtonMinimization_V1(iXLength, iNParam, sRoutineForFunctionAndDerivatives, sRoutineForCorrectionOfDeltaX, sRoutineForCorrectionOfX, oX, &
                oProblemParams, oCompProperties, iMaxIter, dRelativeTolerance, bNewtonMinimizationConverged, oXSol, iNewtonIterationsForSol, iIER, &
                sFunctionForPrematureReturn, bNewtonPrematurelyEnded, sRoutineForAdditionalConvergenceCheck)

        ! OBJETIVO: Aplicar o Método de Newton aos problemas de minimização que surgem nas análises do escopo
        !           desta biblioteca.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ INTERFACES:
        abstract interface
            subroutine TypeRoutineForFunctionAndDerivatives(iXLength, iNParam, oX, oProblemParams, oCompProperties, &
                            dObjectiveFunctionValue, oObjectiveFunctionDerivatives, oHessianMatrix)
                use iso_c_binding
                implicit none
                integer(c_int), value, intent(in) :: iXLength
                integer, intent(in) :: iNParam
                real(c_double), dimension(iXLength), intent(in) :: oX
                real(c_double), dimension(iNParam), intent(inout) :: oProblemParams
                real(c_double), dimension(2*iXLength+4, iXLength), intent(in) :: oCompProperties
                real(c_double), intent(out) :: dObjectiveFunctionValue
                real(c_double), dimension(iXLength), intent(out) :: oObjectiveFunctionDerivatives
                real(c_double), dimension(iXLength, iXLength), intent(out) :: oHessianMatrix
            end subroutine TypeRoutineForFunctionAndDerivatives
        end interface

        abstract interface
            subroutine TypeFunctionForPrematureReturn(iXLength, iNParam, oX, oProblemParams, dObjectiveFunctionValue, bTerminate)
                use iso_c_binding
                implicit none
                integer(c_int), value, intent(in) :: iXLength
                integer, intent(in) :: iNParam
                real(c_double), dimension(iXLength), intent(in) :: oX
                real(c_double), dimension(iNParam), intent(in) :: oProblemParams
                real(c_double), intent(in) :: dObjectiveFunctionValue
                logical, intent(out) :: bTerminate
            end subroutine TypeFunctionForPrematureReturn
        end interface

        abstract interface
            subroutine TypeRoutineForCorrectionOfDeltaX(iXLength, iNParam, oX, oProblemParams, oDeltaX)
                use iso_c_binding
                implicit none
                integer(c_int), value, intent(in) :: iXLength
                integer, intent(in) :: iNParam
                real(c_double), dimension(iXLength), intent(in) :: oX
                real(c_double), dimension(iNParam), intent(in) :: oProblemParams
                real(c_double), dimension(iXLength), intent(inout) :: oDeltaX
            end subroutine TypeRoutineForCorrectionOfDeltaX
        end interface

        abstract interface
            subroutine TypeRoutineForCorrectionOfX(iXLength, iNParam, oX, oDeltaX, oProblemParams, dAlpha)
                use iso_c_binding
                implicit none
                integer(c_int), value, intent(in) :: iXLength
                integer, intent(in) :: iNParam
                real(c_double), dimension(iXLength), intent(inout) :: oX
                real(c_double), dimension(iXLength), intent(in) :: oDeltaX
                real(c_double), dimension(iNParam), intent(inout) :: oProblemParams
                real(c_double), intent(in) :: dAlpha
            end subroutine
        end interface

        abstract interface
            subroutine TypeRoutineForAdditionalConvergenceCheck(iXLength, oX, oPreviousX, iIter, dObjectiveFunctionValue, &
                    dPreviousObjectiveFunctionValue, dRelativeTolerance, dCurrentError, bConverged)
                use iso_c_binding
                implicit none
                integer(c_int), intent(in) :: iXLength
                real(c_double), dimension(iXLength), intent(in) :: oX
                real(c_double), dimension(iXLength), intent(in) :: oPreviousX
                integer, intent(in) :: iIter
                real(c_double), intent(in) :: dObjectiveFunctionValue
                real(c_double), intent(in) :: dPreviousObjectiveFunctionValue
                real(c_double), intent(in) :: dRelativeTolerance
                real(c_double), intent(in) :: dCurrentError
                logical, intent(inout) :: bConverged
            end subroutine
        end interface

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iXLength                             ! Número de variáveis independentes.
        integer, intent(in) :: iNParam                              ! Comprimento do vetor "oProblemParams"
        procedure(TypeRoutineForFunctionAndDerivatives) :: sRoutineForFunctionAndDerivatives
        !external :: sRoutineForFunctionAndDerivatives               ! Subrotina (com cabeçalho padronizado) para cálculos da
                                                                    ! função objetivo e suas derivadas.
        procedure(TypeRoutineForCorrectionOfDeltaX) :: sRoutineForCorrectionOfDeltaX
        !external :: sRoutineForCorrectionOfDeltaX                   ! Subrotina (com cabeçalho padronizado) para ajustes no
                                                                    ! passo calculado a cada iteração.
        procedure(TypeRoutineForCorrectionOfX) :: sRoutineForCorrectionOfX
        !external :: sRoutineForCorrectionOfX                        ! Subrotina (com cabeçalho padronizado) para correção das
                                                                    ! variáveis independentes a cada iteração.
        real(c_double), dimension(iXLength), intent(inout) :: oX    ! Valores iniciais das variáveis independentes (serão modificados).
        real(c_double), dimension(iNParam), intent(inout) :: oProblemParams  ! Parâmetros específicos do problema sendo resolvido.
        real(c_double), dimension(2*iXLength+4, iXLength), intent(in) :: oCompProperties   ! Matriz de propriedades das espécies químicas presentes no fluido.
        integer, intent(in) :: iMaxIter                             ! Máximo de iterações permitidas para o Método de Newton.
        real(c_double), intent(in) :: dRelativeTolerance            ! Tolerância relativa para o cálculo por Newton.

        logical, intent(out) :: bNewtonMinimizationConverged        ! "True" se atingiu convergência, "false" caso contrário
        real(c_double), dimension(size(oX)), intent(out) :: oXSol   ! Solução determinada
        integer(c_int), intent(out) :: iNewtonIterationsForSol      ! Número de iterações realizadas.
        integer(c_int), intent(out) :: iIER                         ! Código de erros, conforme convencionado.

        procedure(TypeFunctionForPrematureReturn), optional :: sFunctionForPrematureReturn
        !logical, external, optional :: sFunctionForPrematureReturn  ! Função (com cabeçalho padronizado) para verificar ocorrências
                                                                    ! frente às quais a minimização deve ser interrompida antes do fim.
        logical, intent(out), optional :: bNewtonPrematurelyEnded   ! "True" caso o método de Newton seja intencionalmente encerrado prematuramente.

        procedure(TypeRoutineForAdditionalConvergenceCheck), optional :: sRoutineForAdditionalConvergenceCheck  ! Subrotina (com cabeçalho padronizado) para verificações
                                                                                                                ! opcionais adicionais de convergência.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iIter
        real(c_double) :: dObjectiveFunctionValue, dPreviousObjectiveFunctionValue
        real(c_double), dimension(size(oX)) :: oObjectiveFunctionDerivatives
        real(c_double), dimension(size(oX), size(oX)) :: oHessianMatrix
        real(c_double), dimension(size(oX)) :: oDeltaX
        integer :: iGaussJordanError
        integer :: iN
        !integer :: iNParam
        real(c_double), dimension(size(oX)) :: oMinusG
        real(c_double) :: dOffsetInErrorCalc, dCurrentError
        logical :: bConverged, bTerminatePrematurely
        real(c_double) :: dAlpha
        integer(c_int) :: iIER_Local
        real(c_double), dimension(iXLength) :: oPreviousX

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Inicializações:
        !iNParam = size(oProblemParams)
        dObjectiveFunctionValue = -10000000.0d0     ! Valor aleatório
        bNewtonMinimizationConverged = .false.
        iIER_Local = ERROR_EverythingOK
        !iXLength = size(oX)

        if(present(bNewtonPrematurelyEnded)) bNewtonPrematurelyEnded = .false.

        mainLoop: do iIter = 1, iMaxIter

            ! Atualizar valores anteriores:
            dPreviousObjectiveFunctionValue = dObjectiveFunctionValue
            oPreviousX = oX     ! CUIDADO PRA NÃO CONFUNDIR: "dPreviousObjectiveFunctionValue" não corresponde a "oPreviousX"!

            ! Obter a matriz Hessiana e o vetor gradiente:
            call sRoutineForFunctionAndDerivatives(iXLength, iNParam, oX, oProblemParams, oCompProperties, dObjectiveFunctionValue, &
                    oObjectiveFunctionDerivatives, oHessianMatrix)

            iIER_Local = int(oProblemParams(iNParam))
            if(iIER_Local.NE.ERROR_EverythingOK) then
                iIER = iIER_Local
                return
            end if

            ! Testar ocorrências frente às quais a minimização deva ser interrompida antes do fim:
            hasPrematureReturnRoutine: if(present(sFunctionForPrematureReturn)) then

                call sFunctionForPrematureReturn(iXLength, iNParam, oX, oProblemParams, dObjectiveFunctionValue, bTerminatePrematurely)

                checkForPrematureClose: if(bTerminatePrematurely) then
                    iNewtonIterationsForSol = iIter
                    if(present(bNewtonPrematurelyEnded)) bNewtonPrematurelyEnded = .true.
                    oXSol = oX
                    return
                end if checkForPrematureClose

            end if hasPrematureReturnRoutine

            ! Resolver o sistema de equações lineares:
            iN = size(oHessianMatrix, 1)
            oMinusG(1:iN) = -oObjectiveFunctionDerivatives(1:iN)

            call SolveLinearEqsByGaussJordanElimination(oHessianMatrix, oMinusG, oDeltaX, iN, iN, iGaussJordanError)

            solvedLinSys: if(iGaussJordanError.ne.0) then
                iIER = ERROR_SingularEquationSystemEncountered
                return
            end if solvedLinSys

            ! Fazer possíveis ajustes necessários nas correções calculadas para as variáveis independentes:
            call sRoutineForCorrectionOfDeltaX(iXLength, iNParam, oX, oProblemParams, oDeltaX)

            ! Atualizar as variáveis independentes com os devidos ajustes:
            dAlpha = 1.0d0
            call sRoutineForCorrectionOfX(iXLength, iNParam, oX, oDeltaX, oProblemParams, dAlpha)

            ! TODO: Possivelmente, usar o último argumento "alfa" acima para garantir uma "descent direction",
            !       conforme discutido na página 373 da Referência 1.

            ! Calcular o erro:
            errorCalcWithOffset: if(abs(dPreviousObjectiveFunctionValue).lt.(0.99d0 * dRelativeTolerance)) then
                dOffsetInErrorCalc = 1.0d0
            else errorCalcWithOffset
                dOffsetInErrorCalc = 0.0d0
            end if errorCalcWithOffset

            dCurrentError = abs( (dObjectiveFunctionValue - dPreviousObjectiveFunctionValue + dOffsetInErrorCalc) / &
                                 (dPreviousObjectiveFunctionValue + dOffsetInErrorCalc) - &
                                 dOffsetInErrorCalc)

            ! Verificar se já pode considerar convergido:
            bConverged = (dCurrentError.lt.dRelativeTolerance)
            if(present(sRoutineForAdditionalConvergenceCheck)) call sRoutineForAdditionalConvergenceCheck(iXLength, oX, oPreviousX, iIter, &
                    dObjectiveFunctionValue, dPreviousObjectiveFunctionValue, dRelativeTolerance, dCurrentError, bConverged)

            bConverged = bConverged.AND.(iIter.GE.2)                ! Garantir duas passagens, por valores válidos de "oPreviousX" e "dPreviousObjectiveFunctionValue".

            convergedOrNot: if(bConverged) then
                bNewtonMinimizationConverged = .true.
                !oXSol = oX                                          ! Rigorosamente falando, seria o vetor "x" da iteração anterior...
                oXSol = oPreviousX                                   ! LINHA ADICIONADA NO LUGAR DA ORIGINAL ACIMA (comentada).
                                                                     ! Observar que no teste da convergência, "dObjectiveFunctionValue" foi calculada
                                                                     ! em "oPreviousX".
                iNewtonIterationsForSol = iIter
                return
            end if convergedOrNot

        end do mainLoop

    end subroutine NewtonMinimization_V1

    ! =============================================================
    ! =============================================================
    subroutine SolveLinearEqsByGaussJordanElimination(a, b, soln, ndim, n, error)

        ! NOTA: Subrotina transcrita de "Fortran 90/95 for Scientists and Engineers", Stephen J. Chapman,
        !   McGraw-Hill, 1998. O nome da subrotina original, "dsimul" (pág 433), foi modificado para maior clareza,
        !   mas os nomes de suas variáveis internas e os comentários foram reproduzidos na íntegra.

        ! OBJETIVO: Subroutine to solve a set of N linear equations in N unknowns, using Gauss-Jordan
        !   elimination and the maximum-pivot technique. This version of simul has been modified to use
        !   array sections and automatic arrays. It uses double-precision arithmetic to avoid cumulative
        !   round-off errors. It DOES NOT DESTROY the original input values.
        implicit none

        ! ------------ CONSTANTES:
        integer, parameter :: db1 = selected_real_kind(p=13)

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer, intent(in) :: ndim                                 ! Dimension of arrays a and b
        real(kind=db1), intent(in), dimension(ndim, ndim) :: a      ! Array of coefficients (N x N). This array is
                                                                    ! of size ndim x ndim, but only N x N of the
                                                                    ! coefficients are being used.
        real(kind=db1), intent(in), dimension(ndim) :: b            ! Input: right side of equations
        real(kind=db1), intent(out), dimension(ndim) :: soln        ! Output: solution vector
        integer, intent(in) :: n                                    ! Number of equations to solve.
        integer, intent(out) :: error                               ! Error flag: 0 - no error, 1 - singular equations

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(kind=db1), parameter :: epsilon = 1.0E-12              ! A "small" number for comparison when determining
                                                                    ! singular equations.

        real(kind=db1), dimension(n,n) :: a1                        ! Copy of "a" that will be destroyed during the solution.
        real(kind=db1) :: factor                        ! Factor to multiply equation irow by before adding to equation jrow.
        integer :: irow                                 ! Number of the equation currently being processed.
        integer :: ipeak                                ! Pointer to equation containing maximum-pivot value.
        integer :: jrow                                 ! Number of the equation compared to the current equation.
        real(kind=db1) :: temp                          ! Scratch value
        real(kind=db1), dimension(n) :: temp1           ! Scratch array

        ! ------------ PROCEDIMENTOS:

        ! Make copies of arrays "a" and "b" for local use:
        a1 = a(1:n, 1:n)
        soln = b(1:n)

        ! Process N times to get all equations...
        mainloop: DO irow = 1, n

            ! Find peak pivot for column irow in rows irow to N

            ipeak = irow
            max_pivot: DO jrow = irow+1, n
                IF(ABS(a1(jrow, irow)) > ABS(a1(ipeak, irow))) THEN
                    ipeak = jrow
                END IF
            END DO max_pivot

            ! Check for singular equations:
            singular: IF(ABS(a1(ipeak, irow)) < epsilon) THEN
                error = 1
                return
            END IF singular

            ! Otherwise, if ipeak /= irow, swap equations irow & ipeak:
            swap_eqn: IF(ipeak /= irow) THEN
                temp1 = a1(ipeak, 1:n)
                a1(ipeak, 1:n) = a1(irow, 1:n)  ! Swap rows in a
                a1(irow, 1:n) = temp1
                temp = soln(ipeak)
                soln(ipeak) = soln(irow)        ! Swap rows in b
                soln(irow) = temp
            END IF swap_eqn

            ! Multiply equation irow by -a1(jrow, irow) / a1(irow, irow),
            ! and add it to equation jrow (for all equations except irow itself).
            eliminate: DO jrow = 1, n
                IF(jrow /= irow) THEN
                    factor = -a1(jrow, irow)/a1(irow, irow)
                    a1(jrow, 1:n) = a1(irow, 1:n) * factor + a1(jrow, 1:n)
                    soln(jrow) = soln(irow) * factor + soln(jrow)
                END IF
            END DO eliminate

        END DO mainloop

        ! End of main loop over all equations. All off-diagonal terms are now zero.
        ! To get the final answer, we must divide each equation by the coefficient
        ! of its on-diagonal term.
        divide: DO irow = 1, n
            soln(irow) = soln(irow) / a1(irow, irow)
        END DO divide

        ! Set error flag to 0 and return.
        error = 0

    end subroutine SolveLinearEqsByGaussJordanElimination

    ! =============================================================
    !           ROTINAS PARA MÉTODO DE NEWTON NO "FLASH"
    ! =============================================================

    subroutine TryFlashCalcWith2ndOrderMinimizationIfNecessary(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, &
                    oKij, oLij, oPeneloux, iLiqPhaseModel, iVapPhaseModel, bGibbsEnergyDecreases, bFoundValidBeta, &
                    bTPDAnalysisSuccessfullyDone, oLeastGPhaseLnFug, iLeastGPhaseID, oLiqStationaryPointW, oVapStationaryPointW, &
                    oImprovedKFactors, iIER, dBetaVap, oX, oY, bIgnoreOutcomeAndResume, &
                    bFeedIsStableAsSinglePhase, oLiqStationaryPointLnFug, oVapStationaryPointLnFug)

        ! OBJETIVO: Faz as verificações necessárias e aciona (ou não) o método de Newton para resolver o flash P, T.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature         ! Temperatura
        real(c_double), value, intent(in) :: dPressure            ! Pressão.
        integer(c_int), value, intent(in) :: iNComp               ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ       ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oTc          ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc          ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW           ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iLiqPhaseModel       ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel       ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        logical, intent(in) :: bGibbsEnergyDecreases              ! Consultar rotina chamadora para significado.
        logical, intent(in) :: bFoundValidBeta                    ! Consultar rotina chamadora para significado.
        logical, intent(inout) :: bTPDAnalysisSuccessfullyDone       ! Consultar rotina chamadora para significado.
        real(c_double), dimension(iNComp), intent(in) :: oLeastGPhaseLnFug    ! Coeficientes de fugacidade correspondentes à fase única de menor energia de Gibbs.
        integer(c_int), value, intent(in) :: iLeastGPhaseID   ! Identificador convencionado da fase na qual a mistura possui a menor energia de Gibbs.

        real(c_double), dimension(iNComp), intent(inout) :: oLiqStationaryPointW         ! "Composição" do líquido no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(inout) :: oVapStationaryPointW         ! "Composição" do vapor no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(inout) :: oImprovedKFactors            ! Estimativas melhoras das constantes de equilíbrio "K" (no caso de mistura instável).

        integer(c_int), intent(out) :: iIER                       ! Código de erros, conforme convencionado.
        real(c_double), intent(inout) :: dBetaVap                   ! Fração molar calculada para a fase vapor
        real(c_double), dimension(iNComp), intent(out) :: oX      ! Vetor composição da fase líquida
        real(c_double), dimension(iNComp), intent(inout) :: oY      ! Vetor composição da fase vapor
        logical, intent(out) :: bIgnoreOutcomeAndResume           ! "True" para indicar que a rotina chamadora deve prosseguir sem analisar
                                                                  ! os resultados desta rotina, "false" caso tenha resultados para serem analisados.
        logical, intent(out) :: bFeedIsStableAsSinglePhase        ! Consultar rotina chamadora para significado.
        real(c_double), dimension(iNComp), intent(out) :: oLiqStationaryPointLnFug     ! Ln dos coeficientes de fugacidade na fase líquida no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out) :: oVapStationaryPointLnFug     ! Ln dos coeficientes de fugacidade na fase vapor no ponto estacionário da função TPD.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        logical :: bTangentPlaneAnalysisBeforeNewton, bFlashNewtonMinConverged

        ! ------------ CONSTANTES:
        logical, parameter :: bConsiderNewtonNonConvergenceAsError = .true.

        ! ------------ PROCEDIMENTOS:
        bIgnoreOutcomeAndResume = .false.

        doAnythingOrNot: if((.not.bAllowNewtonMinimization).or.(.not.bAllowFlashNewtonMinimization)) then
            ! Retornar sem fazer nada:
            bIgnoreOutcomeAndResume = .true.
            return
        end if doAnythingOrNot

        ! Verificar se será necessário fazer uma verificação de estabilidade antes de acionar o Método de Newton:

            ! PÁG 267 da Referência Bibliográfica 1: "If beta exceeds its bounds, it is very likely that the specification
            !       corresponds to a single phase. As no guarantee can be given, we prefer to continue with stability
            !       analysis also in this situation."

        bTangentPlaneAnalysisBeforeNewton = (.not.bGibbsEnergyDecreases).or.(.not.bFoundValidBeta)
        bTangentPlaneAnalysisBeforeNewton = bTangentPlaneAnalysisBeforeNewton.and.(.not.bTPDAnalysisSuccessfullyDone)

        doAnalysisOrNot: if(bTangentPlaneAnalysisBeforeNewton) then

                ! APAGAR ESTE COMENTÁRIO DEPOIS: chamada idêntica à de FlashExtras
            call PerformTangentPlaneAnalysisForVLE(dTemperature, dPressure, iNComp, oZ, oLeastGPhaseLnFug, oTc, oPc, oW, oKij, oLij, &
                        oPeneloux, iLiqPhaseModel, iVapPhaseModel, iLeastGPhaseID, bTPDAnalysisSuccessfullyDone, &
                        bFeedIsStableAsSinglePhaseArg = bFeedIsStableAsSinglePhase, oLiqStationaryPointWArg = oLiqStationaryPointW, &
                        oLiqStationaryPointLnFugArg = oLiqStationaryPointLnFug, oVapStationaryPointWArg = oVapStationaryPointW, &
                        oVapStationaryPointLnFugArg = oVapStationaryPointLnFug, oImprovedKFactorsArg = oImprovedKFactors)

            chkStable1: if(bFeedIsStableAsSinglePhase) then
                call GetResultsForSinglePhaseFeed(iNComp, oZ, iLeastGPhaseID, dBetaVap, oX, oY)
                return
            end if chkStable1

        end if doAnalysisOrNot

        ! Acionar o método de Newton:
        call PerformFlashCalculationWith2ndOrderMinimization(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, &
                oPeneloux, iLiqPhaseModel, iVapPhaseModel, oImprovedKFactors, oLiqStationaryPointW, oVapStationaryPointW, dBetaVap, &
                oX, oY, bFlashNewtonMinConverged, iIER)


        didNewtonConverge: if(.not.bFlashNewtonMinConverged) then

            checkNewtonOutcome: if(iIER.EQ.ERROR_EverythingOK) then

                signalError: if(bConsiderNewtonNonConvergenceAsError) then
                    iIER = ERROR_FlashCalculationFoundNoSolution
                else signalError
                    bIgnoreOutcomeAndResume = .true.
                end if signalError

            end if checkNewtonOutcome

        end if didNewtonConverge

    end subroutine TryFlashCalcWith2ndOrderMinimizationIfNecessary

    ! =============================================================
    ! =============================================================
    subroutine PerformFlashCalculationWith2ndOrderMinimization(dTemperature, dPressure, iNComp, oZ, oTc, oPc, oW, oKij, oLij, &
                oPeneloux, iLiqPhaseModel, iVapPhaseModel, oImprovedKFactors, oLiqStationaryPointW, oVapStationaryPointW, dBetaVap, &
                oX, oY, bFlashNewtonMinConverged, iIER)

        ! OBJETIVO: Resolver um cálculo de Flash T,P através da minimização por Newton.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature             ! Temperatura
        real(c_double), value, intent(in) :: dPressure                ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                 ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oZ         ! Vetor composição da mistura (indexado por componente)
        real(c_double), dimension(iNComp), intent(in) :: oTc          ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc          ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW           ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iLiqPhaseModel       ! Modelo selecionado para a fase líquida, conforme índices convencionados.
        integer(c_int), value, intent(in) :: iVapPhaseModel       ! Modelo selecionado para a fase vapor, conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oImprovedKFactors            ! Estimativas melhores das constantes de equilíbrio "K"
                                                                                      ! (oriundas da verificação de estabilidade).
        real(c_double), dimension(iNComp), intent(in) :: oLiqStationaryPointW         ! "Composição" do líquido no ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(in) :: oVapStationaryPointW         ! "Composição" do vapor no ponto estacionário da função TPD.

        real(c_double), intent(inout) :: dBetaVap                   ! Fração molar inicial(in) / calculada(out) para a fase vapor
        real(c_double), dimension(iNComp), intent(out) :: oX        ! Vetor composição da fase líquida
        real(c_double), dimension(iNComp), intent(inout) :: oY      ! Vetor composição da fase vapor
        logical, intent(out) :: bFlashNewtonMinConverged            ! "True" se atingiu convergência, "false" caso contrário
        integer(c_int), intent(out) :: iIER                         ! Código de erros, conforme convencionado.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        logical :: bReceivedBetaIsOK
        real(c_double), dimension(iNComp) :: oLiqMolarAmounts, oVapMolarAmounts  ! "Liquid and vapor molar amounts" (definição na pág 229 da Referência 1).
        real(c_double), dimension(2*iNComp+5) :: oProblemParams  ! Vetor de parâmetros e variáveis do cálculo de "flash"
        integer :: iLastPositionInParametersVector
        real(c_double), dimension(2*iNComp+4, iNComp) :: oCompProperties    ! Matriz de propriedades dos componentes
        real(c_double) :: dRelativeTolToUse
        integer :: iMaxIterToUse
        real(c_double), dimension(iNComp) :: oVapMolarAmounts_Sol
        integer(c_int) :: iFlashNewtonItersForSol
        real(c_double) :: dBetaLiq
        logical :: bRRSolutionExists
        real(c_double), dimension(iNComp) :: oRRX, oRRY
        integer :: iNParam

        ! ------------ CONSTANTES:
        real(c_double), parameter :: dFlashNewtonRelativeTolerance = 300.0d0 * epsilon(1.0)
        integer, parameter :: iFlashNewtonMaximumIterations = 1000

        ! ------------ PROCEDIMENTOS:

        bReceivedBetaIsOK = (dBetaVap.GE.(0.0d0)).and.(dBetaVap.LE.(1.0d0))

        prepInitialEstimates: if(bReceivedBetaIsOK) then

            ! Equações 34 e 35 da pág 229 da Referência 1:
            oVapMolarAmounts = dBetaVap * oY
            oLiqMolarAmounts = oZ - oVapMolarAmounts

        else prepInitialEstimates

            ! Na indisponibilidade de beta inicial válido, obter estimativas iniciais com base nos
            ! resultados da verificação de estabilidade:
            call SolveRachfordRiceEquation(iNComp, oZ, oImprovedKFactors, bRRSolutionExists, dBetaVap, dBetaLiq, oXArg = oRRX, oYArg = oRRY)

            goWithRRResults: if(bRRSolutionExists.and.(dBetaVap.ge.(0.0d0)).and.(dBetaVap.le.(1.0d0))) then

                ! Equações 34 e 35 da pág 229 da Referência 1:
                oVapMolarAmounts = dBetaVap * oRRY
                oLiqMolarAmounts = oZ - oVapMolarAmounts

            else if(dBetaVap.le.(0.0d0)) then goWithRRResults

                ! PÁG 269 da Referência Bibliográfica 1: "We locate a composition W where tm is negative [...]. In
                !       this case, we continue with flash calculations, W providing a composition estimate for the
                !       incipient phase."

                dBetaVap = 1.0d-3
                oLiqMolarAmounts = min((1.0d0 - dBetaVap) * oLiqStationaryPointW, (1.0d0 - 1.0d-6) * oZ)
                oVapMolarAmounts = oZ - oLiqMolarAmounts

            else goWithRRResults

                ! Conferir nota acima (PÁG 269 da Referência Bibliográfica 1).

                dBetaVap = 0.999d0
                oVapMolarAmounts = min(dBetaVap * oVapStationaryPointW, (1.0d0 - 1.0d-6) * oZ)
                oLiqMolarAmounts = oZ - oVapMolarAmounts

            end if goWithRRResults

        end if prepInitialEstimates

        ! Construir o vetor convencionado com os parâmetros do problema de "Flash":
        iLastPositionInParametersVector = size(oProblemParams)

        oProblemParams(1) = dTemperature
        oProblemParams(2) = dPressure
        oProblemParams(3:iNComp+2) = oZ
        oProblemParams(iNComp+3:2*iNComp+2) = oLiqMolarAmounts
        oProblemParams(iLastPositionInParametersVector - 1) = iLiqPhaseModel
        oProblemParams(iLastPositionInParametersVector - 2) = iVapPhaseModel
        oProblemParams(iLastPositionInParametersVector) = ERROR_EverythingOK

        ! Construir a matriz convencionada de propriedades das espécies químicas:
        oCompProperties(1, 1:iNComp) = oTc
        oCompProperties(2, 1:iNComp) = oPc
        oCompProperties(3, 1:iNComp) = oW
        oCompProperties(4:iNComp+3, 1:iNComp) = oKij
        oCompProperties(iNComp+4:2*iNComp+3, 1:iNComp) = oLij
        oCompProperties(2*iNComp+4, 1:iNComp) = oPeneloux

        ! Mais configurações:
        dRelativeTolToUse = dFlashNewtonRelativeTolerance
        iMaxIterToUse = iFlashNewtonMaximumIterations

        iNParam = size(oProblemParams)

        ! Acionar a minimização por Newton:
        call NewtonMinimization_V1(iNComp, iNParam, CalculateFlashNewtonObjectiveFunctionAndDerivatives, CheckAndCorrectMolarAmountsChange, &
                UpdateMolarAmounts, oVapMolarAmounts, oProblemParams, oCompProperties, &
                iMaxIterToUse, dRelativeTolToUse, bFlashNewtonMinConverged, oVapMolarAmounts_Sol, iFlashNewtonItersForSol, iIER)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Preparar e retornar os resultados:
        dBetaVap = sum(oVapMolarAmounts_Sol)        ! Equação 34 da pág 229 da Referência 1
        oY = oVapMolarAmounts_Sol / dBetaVap        ! Equação 34 da pág 229 da Referência 1

        oLiqMolarAmounts = oProblemParams(iNComp+3:2*iNComp+2)
        dBetaLiq = sum(oLiqMolarAmounts)            ! Equação 34 da pág 229 da Referência 1
        oX = oLiqMolarAmounts / dBetaLiq            ! Equação 34 da pág 229 da Referência 1

        ! Salvaguarda:
        if(isnan(dBetaVap)) iIER = ERROR_FlashCalculationFoundNoSolution

    end subroutine PerformFlashCalculationWith2ndOrderMinimization

    ! =============================================================
    ! =============================================================
    subroutine UpdateMolarAmounts(iNComp, iNParam, oVapMolarAmounts, oDeltaV, oProblemParams, dAlpha)

        ! OBJETIVO: Atualizar os "molar amounts" (definidos na página 229 da Referência 1) a cada iteração
        !           da solução do flash por minimização de Newton, cumprindo assim a função da subrotina
        !           "sRoutineForCorrectionOfX" em "NewtonMinimization_V1".

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer, intent(in) :: iNParam                                  ! Tamanho do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(inout) :: oVapMolarAmounts                   ! "Vapor molar amounts" (definição na pág 229 da Referência 1).
        real(c_double), dimension(iNComp), intent(in) :: oDeltaV          ! "Correction vector" calculado pelo método de Newton.
        real(c_double), dimension(2*iNComp+5), intent(inout) :: oProblemParams  ! Vetor de parâmetros e variáveis do cálculo de "flash"
        real(c_double), intent(in) :: dAlpha                                              ! Fator multiplicativo a ser aplicado sobre "oDeltaV" na atualização dos "molar amounts".

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        !integer :: iNComp
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqMolarAmountsLoc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oZ_Loc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oNewL, oNewV

        ! ------------ PROCEDIMENTOS:

        ! Obter as variáveis necessárias do vetor:
        !iNComp = size(oVapMolarAmounts)
        call GetFlashVariablesFromParametersVector(iNComp, oProblemParams, oLiqMolarAmounts = oLiqMolarAmountsLoc, oZ = oZ_Loc)

        ! Atualizar os valores na proporção desejada:
        oNewV = oVapMolarAmounts + dAlpha * oDeltaV
        oNewL = oZ_Loc - oNewV

        ! Retornar os valores atualizados:
        oVapMolarAmounts = oNewV
        oProblemParams(iNComp+3:2*iNComp+2) = oNewL

    end subroutine UpdateMolarAmounts

    ! =============================================================
    ! =============================================================
    subroutine CheckAndCorrectMolarAmountsChange(iNComp, iNParam, oVapMolarAmounts, oProblemParams, oDeltaV)

        ! OBJETIVO: Ajustar as correções calculadas em cada iteração da minimização por Newton de modo a
        !           impedir que levem as variáveis independentes do flash a assumirem valores fisicamente
        !           impossíveis (cumpre a função da subrotina "sRoutineForCorrectionOfDeltaX" em
        !           "NewtonMinimization_V1").

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer, intent(in) :: iNParam                                  ! Comprimento do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(in) :: oVapMolarAmounts                         ! "Vapor molar amounts" (definição na pág 229 da Referência 1).
        real(c_double), dimension(2*iNComp+5), intent(in) :: oProblemParams  ! Vetor de parâmetros e variáveis do cálculo de "flash"

        real(c_double), dimension(iNComp), intent(inout) :: oDeltaV          ! "Correction vector" calculado pelo método de Newton (in) e
                                                                                             ! ajustado por esta subrotina (out).

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: i
        real(c_double), dimension(iNComp) :: oLiqMolarAmountsLoc
        real(c_double), dimension(iNComp) :: oZ_Loc
        real(c_double), dimension(iNComp) :: oNewL, oNewV
        real(c_double) :: dReductionFactor

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Estratégia para impedir que os "molar amounts" assumam valores negativos: "proporcionar"
        ! o "correction vector" resultante do sistema linear, eventualmente reduzindo os "delta V"
        ! como necessário para que todos os "molar amounts", de ambas as fases, permaneçam positivos.

        ! Obter as variáveis necessárias do vetor:
        !iNComp = size(oVapMolarAmounts)
        call GetFlashVariablesFromParametersVector(iNComp, oProblemParams, oLiqMolarAmounts = oLiqMolarAmountsLoc, oZ = oZ_Loc)

        ! Calcular os vetores que resultariam da aplicação do "correction vector" recebido:
        oNewV = oVapMolarAmounts + oDeltaV
        oNewL = oZ_Loc - oNewV

        ! Determinar se será necessário reduzir a correção nos "vapor molar amounts", e em que proporção:
        dReductionFactor = 1.0d0

        checkMolarAmounts: do i = 1, iNComp

            if(oNewV(i).lt.(0.0d0)) dReductionFactor = min(dReductionFactor, -oVapMolarAmounts(i) / oDeltaV(i) )    ! "oDeltaV(i)" é negativo. "dReductionFactor" será positivo.

            if(oNewL(i).lt.(0.0d0)) dReductionFactor = min(dReductionFactor, (oZ_Loc(i) - oVapMolarAmounts(i)) / oDeltaV(i))    ! "oDeltaV(i)" é positivo. "dReductionFactor" será positivo.

        end do checkMolarAmounts

        ! As reduções proporcionais no "correction vector", se realmente necessárias, produziriam um "molar amount" nulo caso
        ! aplicadas conforme calculadas acima...
        if(dReductionFactor.lt.(1.0d0)) dReductionFactor = dReductionFactor * (1.0d0 - 1.0d-12)

        oDeltaV = oDeltaV * dReductionFactor

    end subroutine CheckAndCorrectMolarAmountsChange

    ! =============================================================
    ! =============================================================
    subroutine CalculateFlashObjectiveFunctionForNewtonMinimization(oVapMolarAmounts, oProblemParams, oCompProperties, dObjectiveFunctionValue)

        ! OBJETIVO: Calcular a função objetivo que precisa ser minimizada em um cálculo de "flash" - ou
        !           seja, a energia de Gibbs.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), dimension(:), intent(in) :: oVapMolarAmounts                         ! "Vapor molar amounts" (definição na pág 229 da Referência 1).
        real(c_double), dimension(2*size(oVapMolarAmounts)+5), intent(inout) :: oProblemParams  ! Vetor de parâmetros e variáveis do cálculo de "flash"
        real(c_double), dimension(2*size(oVapMolarAmounts)+4, size(oVapMolarAmounts)), intent(in) :: oCompProperties    ! Matriz de propriedades dos componentes

        real(c_double), intent(out) :: dObjectiveFunctionValue                                ! Valor calculado da função objetivo.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iNComp
        real(c_double) :: dTemperatureLoc
        real(c_double) :: dPressureLoc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqMolarAmountsLoc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oZ_Loc
        real(c_double) :: dVapBeta, dLiqBeta
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqX, oVapY
        real(c_double), dimension(size(oVapMolarAmounts)) :: oTc_Loc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(size(oVapMolarAmounts)) :: oPc_Loc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(size(oVapMolarAmounts)) :: oW_Loc              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(size(oVapMolarAmounts),size(oVapMolarAmounts)) :: oKij_Loc     ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(size(oVapMolarAmounts),size(oVapMolarAmounts)) :: oLij_Loc     ! Matriz dos parâmetros "lij".
        real(c_double), dimension(size(oVapMolarAmounts)) :: oPeneloux_Loc       ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int) :: iLiqPhaseModel_Loc
        integer(c_int) :: iVapPhaseModel_Loc
        integer(c_int) :: iIER
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqLnFug   ! Vetor dos "ln" dos coeficientes de fugacidade na fase líquida
        real(c_double), dimension(size(oVapMolarAmounts)) :: oVapLnFug   ! Vetor dos "ln" dos coeficientes de fugacidade na fase vapor
        real(c_double) :: dLiqGibbsEnergy, dVapGibbsEnergy

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! PÁG 251 da Referência 1: "An important feature of the isothermal equilibrium is that the correct
        !       solution represents the global minimum in Gibbs energy".

        ! Obter as variáveis necessárias do vetor:
        iNComp = size(oVapMolarAmounts)
        call GetFlashVariablesFromParametersVector(iNComp, oProblemParams, dTemperature = dTemperatureLoc, dPressure = dPressureLoc, &
                oLiqMolarAmounts = oLiqMolarAmountsLoc, oZ = oZ_Loc, iLiqPhaseModel = iLiqPhaseModel_Loc, iVapPhaseModel = iVapPhaseModel_Loc)

        ! Obter as propriedades necessárias dos componentes:
        call GetComponentPropertiesFromParameterMatrix(iNComp, oCompProperties, oTc = oTc_Loc, oPc = oPc_Loc, oW = oW_Loc, oKij = oKij_Loc, &
                    oLij = oLij_Loc, oPeneloux = oPeneloux_Loc)

        ! "Reverter" para frações molares:
        dVapBeta = sum(oVapMolarAmounts)
        dLiqBeta = sum(oLiqMolarAmountsLoc)

        oLiqX = oLiqMolarAmountsLoc / dLiqBeta
        oVapY = oVapMolarAmounts / dVapBeta

        ! Calcular os coeficientes de fugacidade:

            ! Coeficientes de fugacidade para a fase líquida:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Liquid, dTemperatureLoc, dPressureLoc, oLiqX, &
             iLiqPhaseModel_Loc, oTc_Loc, oPc_Loc, oW_Loc, oKij_Loc, oLij_Loc, oPeneloux_Loc, iIER, oLnFugArg = oLiqLnFug)

        if(iIER.NE.ERROR_EverythingOK) then
            oProblemParams(size(oProblemParams)) = iIER
            return
        end if

            ! Coeficientes de fugacidade para a fase vapor:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Vapor, dTemperatureLoc, dPressureLoc, oVapY, &
             iVapPhaseModel_Loc, oTc_Loc, oPc_Loc, oW_Loc, oKij_Loc, oLij_Loc, oPeneloux_Loc, iIER, oLnFugArg = oVapLnFug)

        if(iIER.NE.ERROR_EverythingOK) then
            oProblemParams(size(oProblemParams)) = iIER
            return
        end if

        ! Calcular a energia de Gibbs da mistura bifásica:
        call CalculateRelativeMolarGibbsEnergy(iNComp, oLiqX, oLiqLnFug, dLiqGibbsEnergy)
        call CalculateRelativeMolarGibbsEnergy(iNComp, oVapY, oVapLnFug, dVapGibbsEnergy)

        dObjectiveFunctionValue = (dLiqBeta * dLiqGibbsEnergy) + (dVapBeta * dVapGibbsEnergy)

    end subroutine CalculateFlashObjectiveFunctionForNewtonMinimization

    ! =============================================================
    ! =============================================================
    subroutine CalculateFlashNewtonObjectiveFunctionAndDerivatives(iNComp, iNParam, oVapMolarAmounts, oProblemParams, oCompProperties, dObjectiveFunctionValue, &
        oObjectiveFunctionDerivatives, oHessianMatrix)

        ! OBJETIVO: Calcular variáveis necessárias para a solução do flash por minimização da energia de
        !   Gibbs (função objetivo, vetor gradiente e matriz Hessiana).

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                             ! Número de componentes.
        integer, intent(in) :: iNParam                              ! Comprimento do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(in) :: oVapMolarAmounts                       ! "Vapor molar amounts" (definição na pág 229 da Referência 1).
        real(c_double), dimension(2*iNComp+5), intent(inout) :: oProblemParams  ! Vetor de parâmetros e variáveis do cálculo de "flash"
        real(c_double), dimension(2*iNComp+4, iNComp), intent(in) :: oCompProperties    ! Matriz de propriedades dos componentes

        real(c_double), intent(out) :: dObjectiveFunctionValue                                ! Valor calculado da função objetivo.
        real(c_double), dimension(iNComp), intent(out) :: oObjectiveFunctionDerivatives
        real(c_double), dimension(iNComp, iNComp), intent(out) :: oHessianMatrix

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        !integer :: iNComp
        real(c_double) :: dTemperatureLoc
        real(c_double) :: dPressureLoc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqMolarAmountsLoc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oZ_Loc
        integer(c_int) :: iLiqPhaseModel_Loc
        integer(c_int) :: iVapPhaseModel_Loc
        real(c_double), dimension(size(oVapMolarAmounts)) :: oTc_Loc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(size(oVapMolarAmounts)) :: oPc_Loc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(size(oVapMolarAmounts)) :: oW_Loc              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(size(oVapMolarAmounts),size(oVapMolarAmounts)) :: oKij_Loc     ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(size(oVapMolarAmounts),size(oVapMolarAmounts)) :: oLij_Loc     ! Matriz dos parâmetros "lij".
        real(c_double), dimension(size(oVapMolarAmounts)) :: oPeneloux_Loc       ! Vetor dos "volumes de shift" para cada componente.
        real(c_double) :: dVapBeta, dLiqBeta
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqX, oVapY
        integer(c_int) :: iIER
        real(c_double), dimension(size(oVapMolarAmounts)) :: oLiqLnFug   ! Vetor dos "ln" dos coeficientes de fugacidade na fase líquida
        real(c_double), dimension(size(oVapMolarAmounts)) :: oVapLnFug   ! Vetor dos "ln" dos coeficientes de fugacidade na fase vapor
        real(c_double) :: dLiqGibbsEnergy, dVapGibbsEnergy
        integer :: i
        real(c_double), dimension(size(oVapMolarAmounts), size(oVapMolarAmounts)) :: oDerivLnCoefFugWithNj_Liq, oDerivLnCoefFugWithNj_Vap
        real(c_double), dimension(size(oVapMolarAmounts), size(oVapMolarAmounts)) :: oPhiIJ_Liq, oPhiIJ_Vap     ! "Phi" da equação 12 da pág 255 da referência.
        real(c_double) :: dNLiq, dNVap
        real(c_double) :: dLogX, dLogY

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Inicializar:
        iIER = ERROR_EverythingOK

        ! PÁG 251 da Referência 1: "An important feature of the isothermal equilibrium is that the correct
        !       solution represents the global minimum in Gibbs energy".

        ! Obter as variáveis necessárias do vetor:
        !iNComp = size(oVapMolarAmounts)
        call GetFlashVariablesFromParametersVector(iNComp, oProblemParams, dTemperature = dTemperatureLoc, dPressure = dPressureLoc, &
                oLiqMolarAmounts = oLiqMolarAmountsLoc, oZ = oZ_Loc, iLiqPhaseModel = iLiqPhaseModel_Loc, iVapPhaseModel = iVapPhaseModel_Loc)

        ! Obter as propriedades necessárias dos componentes:
        call GetComponentPropertiesFromParameterMatrix(iNComp, oCompProperties, oTc = oTc_Loc, oPc = oPc_Loc, oW = oW_Loc, oKij = oKij_Loc, &
                    oLij = oLij_Loc, oPeneloux = oPeneloux_Loc)

        ! "Reverter" para frações molares:
        dVapBeta = sum(oVapMolarAmounts)
        dLiqBeta = sum(oLiqMolarAmountsLoc)

        oLiqX = oLiqMolarAmountsLoc / dLiqBeta
        oVapY = oVapMolarAmounts / dVapBeta

        ! Calcular os coeficientes de fugacidade:

            ! Coeficientes de fugacidade para a fase líquida:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Liquid, dTemperatureLoc, dPressureLoc, oLiqX, &
             iLiqPhaseModel_Loc, oTc_Loc, oPc_Loc, oW_Loc, oKij_Loc, oLij_Loc, oPeneloux_Loc, iIER, oLnFugArg = oLiqLnFug, &
             oDerivLnCoefFugWithNjArg = oDerivLnCoefFugWithNj_Liq)

        if(iIER.NE.ERROR_EverythingOK) then
            oProblemParams(size(oProblemParams)) = iIER
            return
        end if

            ! Coeficientes de fugacidade para a fase vapor:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, PHASE_Vapor, dTemperatureLoc, dPressureLoc, oVapY, &
             iVapPhaseModel_Loc, oTc_Loc, oPc_Loc, oW_Loc, oKij_Loc, oLij_Loc, oPeneloux_Loc, iIER, oLnFugArg = oVapLnFug, &
             oDerivLnCoefFugWithNjArg = oDerivLnCoefFugWithNj_Vap)

        if(iIER.NE.ERROR_EverythingOK) then
            oProblemParams(size(oProblemParams)) = iIER
            return
        end if

        ! Calcular a energia de Gibbs da mistura bifásica:
        call CalculateRelativeMolarGibbsEnergy(iNComp, oLiqX, oLiqLnFug, dLiqGibbsEnergy)
        call CalculateRelativeMolarGibbsEnergy(iNComp, oVapY, oVapLnFug, dVapGibbsEnergy)

        dObjectiveFunctionValue = (dLiqBeta * dLiqGibbsEnergy) + (dVapBeta * dVapGibbsEnergy)

        ! Calcular o vetor gradiente e a matriz Hessiana:

        ! --> Vetor gradiente: ver Equação 34, pág 263 da Referência 1, junto com a Equação 31 da pág 229,
        !                      e considerar que "log(a*b*c) = log(a) + log(b) + log(c)"

        ! --> Matriz Hessiana: Equação 13 na pág 255 da Referência 1.

        ! Equação 12, pág 255 da referência:
        dNLiq = sum(oLiqX)
        oPhiIJ_Liq = dNLiq * oDerivLnCoefFugWithNj_Liq

        dNVap = sum(oVapY)
        oPhiIJ_Vap = dNVap * oDerivLnCoefFugWithNj_Vap

        ! Inicializando a matriz Hessiana - equação 13 da pág 255 da referência:
        oHessianMatrix = (1.0d0 / dVapBeta / dLiqBeta) * (-1.0d0 + dLiqBeta * oPhiIJ_Vap + dVapBeta * oPhiIJ_Liq)

        gAndHLoop: do i = 1, iNComp

            ! Vetor gradiente:
            !checkFracsForG: if((oLiqX(i).gt.(0.0d0)).and.(oVapY(i).gt.(0.0d0))) then

                oObjectiveFunctionDerivatives(i) = log(oVapY(i)) + oVapLnFug(i) - log(oLiqX(i)) - oLiqLnFug(i)

            !else checkFracsForG

                ! Observação: este "if checkFracsForG" foi colocado como proteção contra frações molares nulas entrando de argumento para os "log"
                !             na expressão. Só que descobriu-se posteriormente que tais valores nulos vinham chegando aqui em função de
                !             um erro de implementação em outro local (prontamente corrigido). Ou seja: a rigor, este "if" não é necessário.

            !    dLogX = log(1.0d-6)
            !    dLogY = log(1.0d-6)

            !    if(oLiqX(i).gt.(0.0d0)) dLogX = log(oLiqX(i))
            !    if(oVapY(i).gt.(0.0d0)) dLogY = log(oVapY(i))

            !    oObjectiveFunctionDerivatives(i) = dLogY + oVapLnFug(i) - dLogX - oLiqLnFug(i)

            !end if checkFracsForG

            ! Matriz Hessiana - complementando com os termos da diagonal:
            oHessianMatrix(i, i) = oHessianMatrix(i, i) + (oZ_Loc(i) / oLiqX(i) / oVapY(i) / dLiqBeta / dVapBeta)

        end do gAndHLoop

    end subroutine CalculateFlashNewtonObjectiveFunctionAndDerivatives

    ! =============================================================
    ! =============================================================
    subroutine GetComponentPropertiesFromParameterMatrix(iNComp, oCompProperties, oTc, oPc, oW, oKij, oLij, oPeneloux)

        ! OBJETIVO: Obter as propriedades dos componentes (temperatura crítica, fator acêntrico, etc) a partir
        !           da matriz convencionada para uso com as rotinas do Método de Newton.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                     ! Número de componentes
        real(c_double), dimension(2*iNComp+4, iNComp), intent(in) :: oCompProperties    ! Matriz de propriedades dos componentes

        real(c_double), dimension(iNComp), intent(out), optional :: oTc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(out), optional :: oPc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(out), optional :: oW              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(out), optional :: oKij     ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(out), optional :: oLij     ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(out), optional :: oPeneloux       ! Vetor dos "volumes de shift" para cada componente.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        if(present(oTc)) oTc = oCompProperties(1, 1:iNComp)
        if(present(oPc)) oPc = oCompProperties(2, 1:iNComp)
        if(present(oW)) oW = oCompProperties(3, 1:iNComp)
        if(present(oKij)) oKij = oCompProperties(4:iNComp+3, 1:iNComp)
        if(present(oLij)) oLij = oCompProperties(iNComp+4:2*iNComp+3, 1:iNComp)
        if(present(oPeneloux)) oPeneloux = oCompProperties(2*iNComp+4, 1:iNComp)

    end subroutine GetComponentPropertiesFromParameterMatrix

    ! =============================================================
    ! =============================================================
    subroutine GetFlashVariablesFromParametersVector(iNComp, oProblemParams, dTemperature, dPressure, oLiqMolarAmounts, oZ, &
                    iLiqPhaseModel, iVapPhaseModel)

        ! OBJETIVO: Obter as variáveis de cálculos de "flash" a partir do vetor de parâmetros
        !   convencionado para uso com as rotinas do Método de Newton.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                  ! Número de componentes
        real(c_double), dimension(2*iNComp+5), intent(in) :: oProblemParams          ! Vetor de parâmetros cujas variáveis deseja-se separar.

        real(c_double), intent(out), optional :: dTemperature
        real(c_double), intent(out), optional :: dPressure
        real(c_double), dimension(iNComp), intent(out), optional :: oLiqMolarAmounts
        real(c_double), dimension(iNComp), intent(out), optional :: oZ                             ! Vetor composição da mistura (indexado por componente)

        integer(c_int), intent(out), optional :: iLiqPhaseModel
        integer(c_int), intent(out), optional :: iVapPhaseModel

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iLastPositionInParametersVector

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:
        iLastPositionInParametersVector = size(oProblemParams)

        if(present(dTemperature)) dTemperature = oProblemParams(1)
        if(present(dPressure)) dPressure = oProblemParams(2)
        if(present(oZ)) oZ = oProblemParams(3:iNComp+2)
        if(present(oLiqMolarAmounts)) oLiqMolarAmounts = oProblemParams(iNComp+3:2*iNComp+2)

        ! Últimas posições (IMPORTANTE: manter a "very last" para código de erro):
        if(present(iLiqPhaseModel)) iLiqPhaseModel = int(oProblemParams(iLastPositionInParametersVector - 1))
        if(present(iVapPhaseModel)) iVapPhaseModel = int(oProblemParams(iLastPositionInParametersVector - 2))

    end subroutine GetFlashVariablesFromParametersVector

    ! =============================================================
    !   ROTINAS PARA MÉTODO DE NEWTON NA "TANGENT PLANE ANALYSIS"
    ! =============================================================

    subroutine TryStabilityAnalysisWith2ndOrderMinimization(dTemperature, dPressure, iNComp, oTc, oPc, oW, oKij, oLij, oPeneloux, &
                    iTrialPhaseID, iTrialPhaseThermodynamicModel, oCurrentW, oD, dNegativeTPDCriteriaToUse, iIER, bIgnoreOutcomeAndResume, &
                    oStationaryPointW, oStationaryPointLnFug, dStationaryPointTPD, dLastTrialWSumBeforeNormalizationMinusUnity)

        ! OBJETIVO: Realizar a verificação de estabilidade através de método numérico de segunda ordem.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dTemperature           ! Temperatura
        real(c_double), value, intent(in) :: dPressure              ! Pressão.
        integer(c_int), value, intent(in) :: iNComp                 ! Número de componentes.
        real(c_double), dimension(iNComp), intent(in) :: oTc                 ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oPc                 ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp), intent(in) :: oW                  ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oKij         ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(iNComp,iNComp), intent(in) :: oLij         ! Matriz dos parâmetros "lij".
        real(c_double), dimension(iNComp), intent(in) :: oPeneloux           ! Vetor dos "volumes de shift" para cada componente.
        integer(c_int), value, intent(in) :: iTrialPhaseID          ! Identificador da "trial phase", conforme convenção.
        integer(c_int), value, intent(in) :: iTrialPhaseThermodynamicModel   ! Modelo termodinâmico para a "trial phase", conforme índices convencionados.
        real(c_double), dimension(iNComp), intent(in) :: oCurrentW  ! "W" que servirá de estimativa inicial para o Método de Newton.
        real(c_double), dimension(iNComp), intent(in) :: oD         ! "di" da eq 3 do Cap 9 da referência
        real(c_double), intent(in) :: dNegativeTPDCriteriaToUse

        integer(c_int), intent(out) :: iIER                       ! Código de erros, conforme convencionado.
        logical, intent(out) :: bIgnoreOutcomeAndResume           ! "True" para indicar que a rotina chamadora deve prosseguir sem analisar
                                                                  ! os resultados desta rotina, "false" caso tenha resultados para serem analisados.
        real(c_double), dimension(iNComp), intent(out) :: oStationaryPointW       ! "w" que corresponde a um ponto estacionário da função TPD.
        real(c_double), dimension(iNComp), intent(out) :: oStationaryPointLnFug   ! Ln dos coeficientes de fugacidade correspondentes ao ponto estacionário encontrado.
        real(c_double), intent(out) :: dStationaryPointTPD                        ! Valor da função TPD no ponto estacionário encontrado
        real(c_double), intent(out) :: dLastTrialWSumBeforeNormalizationMinusUnity

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNComp) :: oAlpha, oAlpha_Sol   ! Variáveis independentes transformadas.
        real(c_double), dimension(iNComp+6) :: oProblemParams     ! Vetor de parâmetros convencionado para o cálculo.
        integer :: iLastPositionInParametersVector
        real(c_double), dimension(2*iNComp+4, iNComp) :: oCompProperties    ! Matriz de propriedades dos componentes
        real(c_double) :: dRelativeTolToUse
        integer :: iMaxIterToUse
        logical :: bNewtonMinimizationConverged, bTerminatedPrematurely
        integer(c_int) :: iNewtonIterationsForSol
        integer :: iNParam

        ! ------------ CONSTANTES:
        real(c_double), parameter :: dTPDNewtonMinRelativeTol = 1.0d5 * epsilon(1.0)   ! Tolerância relativa para o cálculo por Newton.
        !real(c_double), parameter :: dTPDNewtonMinRelativeTol = 1.0d5 * epsilon(1.0) * 0.46d0
        !real(c_double), parameter :: dTPDNewtonMinRelativeTol = 1.0d5 * epsilon(1.0) * 0.1d0   ! VALOR ORIGINAL COMENTADO ACIMA

        integer :: iTPDNewtonMinMaxIter = 1000          ! Máximo de iterações permitido para o cálculo por Newton

        ! ------------ PROCEDIMENTOS:
        bIgnoreOutcomeAndResume = .false.

        doAnythingOrNot: if((.not.bAllowNewtonMinimization).or.(.not.bAllowTPDNewtonMinimization)) then
            ! Retornar sem fazer nada:
            bIgnoreOutcomeAndResume = .true.
            return
        end if doAnythingOrNot

        ! Aplicar a transformação das variáveis independentes (equação 53 da pág 269 da Referência 1):
        oAlpha = 2.0d0 * sqrt(oCurrentW)

        ! Preparar o vetor de parâmetros no padrão convencionado para aplicação do Método de Newton:
        iLastPositionInParametersVector = size(oProblemParams)

        oProblemParams(1) = dTemperature
        oProblemParams(2) = dPressure
        oProblemParams(3) = iTrialPhaseID
        oProblemParams(4:iNComp+3) = oD

        oProblemParams(iLastPositionInParametersVector) = ERROR_EverythingOK
        oProblemParams(iLastPositionInParametersVector-1) = iTrialPhaseThermodynamicModel
        oProblemParams(iLastPositionInParametersVector-2) = dNegativeTPDCriteriaToUse

        ! Construir a matriz convencionada de propriedades das espécies químicas:
        oCompProperties(1, 1:iNComp) = oTc
        oCompProperties(2, 1:iNComp) = oPc
        oCompProperties(3, 1:iNComp) = oW
        oCompProperties(4:iNComp+3, 1:iNComp) = oKij
        oCompProperties(iNComp+4:2*iNComp+3, 1:iNComp) = oLij
        oCompProperties(2*iNComp+4, 1:iNComp) = oPeneloux

        ! Mais configurações:
        dRelativeTolToUse = dTPDNewtonMinRelativeTol
        iMaxIterToUse = iTPDNewtonMinMaxIter

        iNParam = size(oProblemParams)

        ! Acionar a minimização por Newton:
        call NewtonMinimization_V1(iNComp, iNParam, CalculateStabAnalysisNewtonObjFunctionAndDerivatives, CheckAndCorrectTrialPhaseCompositionChange, &
                UpdateTrialPhaseComposition, oAlpha, oProblemParams, oCompProperties, iMaxIterToUse, dRelativeTolToUse, &
                bNewtonMinimizationConverged, oAlpha_Sol, iNewtonIterationsForSol, iIER, TerminateStabAnalysisPrematurely, &
                bNewtonPrematurelyEnded = bTerminatedPrematurely, &
                sRoutineForAdditionalConvergenceCheck = AdditionalNewtonConvergenceCheckForTangentPlaneVLEAnalysis)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Tolerar a "não-convergência" do Método de Newton somente se for por encerramento prematuro proposital:
        checkNewtonConv: if(.not.bNewtonMinimizationConverged) then

            wasPremature: if(.not.bTerminatedPrematurely) then
                iIER = ERROR_MinimizationOfTPDFunctionDidNotConverge
                return
            end if wasPremature

        end if checkNewtonConv

        ! "Desfazer" a transformação das variáveis independentes (equação 53 da pág 269 da Referência 1):
        oStationaryPointW = oAlpha_Sol * oAlpha_Sol / 4.0d0

        ! Calcular as demais variáveis de saída:
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, iTrialPhaseID, dTemperature, dPressure, oStationaryPointW, &
                iTrialPhaseThermodynamicModel, oTc, oPc, oW, oKij, oLij, oPeneloux, iIER, oLnFugArg = oStationaryPointLnFug)

        if(iIER.NE.ERROR_EverythingOK) return

        call CalculateTPD(iNComp, oStationaryPointW, oStationaryPointLnFug, oD, dStationaryPointTPD)

        dLastTrialWSumBeforeNormalizationMinusUnity = -10.0d0       ! Para a rotina chamadora não ficar sem.

    end subroutine TryStabilityAnalysisWith2ndOrderMinimization

    ! =============================================================
    ! =============================================================
    subroutine TerminateStabAnalysisPrematurely(iNComp, iNParam, oAlpha, oProblemParams, dObjectiveFunctionValue, bTerminateStabAnalysisPrematurely)

        ! OBJETIVO: Verificar se a execução iterativa da verificação de estabilidade pelo Método de Newton
        !           precisa ser encerrada imediatamente, cumprindo assim o papel da função "sFunctionForPrematureReturn"
        !           em "NewtonMinimization_V1".

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer, intent(in) :: iNParam                                  ! Tamanho do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(in) :: oAlpha              ! Variáveis independentes (composição "W" transformada conforme
                                                                        ! equação 53 do Capítulo 10 da Referência Bibliográfica 1)
        real(c_double), dimension(iNComp+6), intent(in) :: oProblemParams          ! Vetor de parâmetros contendo variáveis do cálculo.
        real(c_double), intent(in) :: dObjectiveFunctionValue           ! Valor calculado da função objetivo.
        logical, intent(out) :: bTerminateStabAnalysisPrematurely

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        !integer :: iNComp
        real(c_double) :: dTPD, dNegativeTPDCriteriaToUse_Loc

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Obter parâmetros necessários:
        !iNComp = size(oAlpha)
        call GetStabilityAnalysisVariablesFromParametersVector(iNComp, oProblemParams, dNegativeTPDCriteriaToUse = dNegativeTPDCriteriaToUse_Loc)

        ! Verificar se é o caso de encerrar prematuramente devido a tpd negativo:
        dTPD = dObjectiveFunctionValue

        bTerminateStabAnalysisPrematurely = (dTPD.lt.dNegativeTPDCriteriaToUse_Loc)

        ! ATENÇÃO: em desacordo com o algoritmo originalmente programado, a experiência de usos desta biblioteca veio
        !   sugerindo que esta prática de encerrar o método iterativo ANTES DE SUA CONVERGÊNCIA ao encontrar TPD negativo
        !   acaba produzindo resultados inválidos - por exemplo, misturas assim sendo diagnosticadas como bifásicas quando seu
        !   envelope de fases claramente indica ser monofásica. Após cuidadosa análise, optou-se por descontinuar este ítem
        !   específico da abordagem. COMO FOI FEITO: de modo a permitir mais testes antes de modificações mais difíceis de
        !   reverter, optou-se neste primeiro momento apenas por incluir a linha abaixo. Basta comentá-la para retornar à
        !   implementação original.
        bTerminateStabAnalysisPrematurely = .false.

    end subroutine TerminateStabAnalysisPrematurely

    ! =============================================================
    ! =============================================================
    subroutine UpdateTrialPhaseComposition(iNComp, iNParam, oAlpha, oDeltaAlpha, oProblemParams, dAlpha)

        ! OBJETIVO: Atualizar os "alfa" ("w" transformado) a cada iteração da verificação de estabilidade
        !           pelo Método de Newton, cumprindo assim a função da subrotina "sRoutineForCorrectionOfX"
        !           em "NewtonMinimization_V1".

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer, intent(in) :: iNParam                                  ! Tamanho do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(inout) :: oAlpha           ! Variáveis independentes (composição "W" transformada conforme
                                                                        ! equação 53 do Capítulo 10 da Referência Bibliográfica 1)
        real(c_double), dimension(iNComp), intent(in) :: oDeltaAlpha   ! "Newton correction" para "alfa" calculado na atual iteração do
                                                                             ! Método de Newton
        real(c_double), dimension(iNComp+6), intent(inout) :: oProblemParams          ! Vetor de parâmetros contendo variáveis do cálculo.
        real(c_double), intent(in) :: dAlpha                            ! Fator multiplicativo a ser aplicado sobre "oDeltaAlpha"

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        !integer :: iNComp

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        !iNComp = size(oAlpha)

        oAlpha(1:iNComp) = oAlpha(1:iNComp) + dAlpha * oDeltaAlpha(1:iNComp)

    end subroutine UpdateTrialPhaseComposition

    ! =============================================================
    ! =============================================================
    subroutine CheckAndCorrectTrialPhaseCompositionChange(iNComp, iNParam, oAlpha, oProblemParams, oDeltaAlpha)

        ! OBJETIVO: Realizar ajustes eventualmente necessários nas "Newton correction" calculadas
        !           para o vetor "alfa" ("w" transformado) a cada iteração do Método de Newton,
        !           desempenhando o papel da rotina "sRoutineForCorrectionOfDeltaX" em "NewtonMinimization_V1".

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer, intent(in) :: iNParam                                  ! Comprimento do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(in) :: oAlpha         ! Variáveis independentes (composição "W" transformada conforme
                                                                        ! equação 53 do Capítulo 10 da Referência Bibliográfica 1)
        real(c_double), dimension(iNComp+6), intent(in) :: oProblemParams          ! Vetor de parâmetros contendo variáveis do cálculo.
        real(c_double), dimension(iNComp), intent(inout) :: oDeltaAlpha   ! "Newton correction" para "alfa" calculado na atual iteração do
                                                                                ! Método de Newton

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: i
        real(c_double) :: dReductionFactor
        real(c_double), dimension(size(oAlpha)) :: oNewAlpha

        ! ------------ CONSTANTES:
        real(c_double), parameter :: dFactorOnFactor = 1.0d0 - 1.0d-12      ! Impedir "alfas" nulos.

        ! ------------ PROCEDIMENTOS:

        ! Estratégia: reduzir o vetor calculado de "Newton correction" na proporção requerida para
        !             que todos os novos valores de "alfa" permaneçam positivos.
        !iNComp = size(oAlpha)

        oNewAlpha = oAlpha + oDeltaAlpha        ! Aplicar a "Newton correction" recebida para os testes.

        dReductionFactor = 1.0d0

        checkLoop: do i = 1, iNComp

            correctIf: if(oNewAlpha(i).lt.(0.0d0)) then

                dReductionFactor = min(dReductionFactor, (-oAlpha(i) / oDeltaAlpha(i))*dFactorOnFactor)

            else if(oNewAlpha(i).eq.(0.0d0)) then correctIf

                dReductionFactor = min(dReductionFactor, dFactorOnFactor)

            end if correctIf

        end do checkLoop

        ! Aplicar a correção:
        if(dReductionFactor.lt.(1.0d0)) oDeltaAlpha = oDeltaAlpha * dReductionFactor

    end subroutine CheckAndCorrectTrialPhaseCompositionChange

    ! =============================================================
    ! =============================================================
    subroutine GetStabilityAnalysisVariablesFromParametersVector(iNComp, oProblemParams, dTemperature, dPressure, iTrialPhaseID, &
                iTrialPhaseThermodynamicModel, oD, dNegativeTPDCriteriaToUse)

        ! OBJETIVO: Obter as variáveis de cálculos de verificação de estabilidade a partir do vetor de parâmetros
        !           convencionado para uso com as rotinas do Método de Newton.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                                ! Número de componentes
        real(c_double), dimension(iNComp+6), intent(in) :: oProblemParams          ! Vetor de parâmetros cujas variáveis deseja-se separar.

        real(c_double), intent(out), optional :: dTemperature
        real(c_double), intent(out), optional :: dPressure
        integer(c_int), intent(out), optional :: iTrialPhaseID
        integer(c_int), intent(out), optional :: iTrialPhaseThermodynamicModel
        real(c_double), dimension(iNComp), intent(out), optional :: oD               ! Variáveis "di" que figuram na equação 44 do
                                                                                     !     Capítulo 10 da Referência Bibiográfica 1.
        real(c_double), intent(out), optional :: dNegativeTPDCriteriaToUse          ! Limite abaixo do qual considerar "tpd" como negativo.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iLastPositionInParametersVector

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:
        iLastPositionInParametersVector = size(oProblemParams)

        if(present(dTemperature)) dTemperature = oProblemParams(1)
        if(present(dPressure)) dPressure = oProblemParams(2)
        if(present(iTrialPhaseID)) iTrialPhaseID = int(oProblemParams(3))
        if(present(oD)) oD = oProblemParams(4:iNComp+3)

        ! Últimas posições (IMPORTANTE: manter a "very last" para código de erro):
        if(present(iTrialPhaseThermodynamicModel)) iTrialPhaseThermodynamicModel = int(oProblemParams(iLastPositionInParametersVector-1))
        if(present(dNegativeTPDCriteriaToUse)) dNegativeTPDCriteriaToUse = oProblemParams(iLastPositionInParametersVector-2)

    end subroutine GetStabilityAnalysisVariablesFromParametersVector

    ! =============================================================
    ! =============================================================
    subroutine CalculateStabAnalysisObjFunctionForNewtonMinimization(oAlpha, oProblemParams, oCompProperties, dObjectiveFunctionValue)

        ! OBJETIVO: Calcular a função objetivo que precisa ser minimizada em uma verificação de estabilidade.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), dimension(:), intent(in) :: oAlpha              ! Variáveis independentes (composição "W" transformada conforme
                                                                        ! equação 53 do Capítulo 10 da Referência Bibliográfica 1)
        real(c_double), dimension(size(oAlpha)+6), intent(inout) :: oProblemParams          ! Vetor de parâmetros contendo variáveis do cálculo.
        real(c_double), dimension(2*size(oAlpha)+4, size(oAlpha)), intent(in) :: oCompProperties    ! Matriz de propriedades dos componentes

        real(c_double), intent(out) :: dObjectiveFunctionValue          ! Valor calculado da função objetivo.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iIER

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Acionar os cálculos:
        call CalculateNewtonObjFunctionAndDerivsForTangentPlaneVLEAnalysis(oAlpha, oProblemParams, oCompProperties, iIER, &
                dObjectiveFunctionValue_Arg = dObjectiveFunctionValue)

        ! Tratar possível erro:
        checkIIER: if(iIER.NE.ERROR_EverythingOK) then
            oProblemParams(size(oProblemParams)) = iIER
            return
        end if checkIIER

    end subroutine CalculateStabAnalysisObjFunctionForNewtonMinimization

    ! =============================================================
    ! =============================================================
    subroutine CalculateStabAnalysisNewtonObjFunctionAndDerivatives(iNComp, iNParam, oAlpha, oProblemParams, oCompProperties, dObjectiveFunctionValue, &
                    oGradientVector, oHessianMatrix)

        ! OBJETIVO: Calcular variáveis necessárias para a verificação de estabilidade usando o Método de Newton.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), value, intent(in) :: iNComp                     ! Número de componentes.
        integer, intent(in) :: iNParam                              ! Comprimento do vetor "oProblemParams"
        real(c_double), dimension(iNComp), intent(in) :: oAlpha         ! Variáveis independentes (composição "W" transformada conforme
                                                                        ! equação 53 do Capítulo 10 da Referência Bibliográfica 1)
        real(c_double), dimension(iNComp+6), intent(inout) :: oProblemParams          ! Vetor de parâmetros contendo variáveis do cálculo.
        real(c_double), dimension(2*iNComp+4, iNComp), intent(in) :: oCompProperties    ! Matriz de propriedades dos componentes

        real(c_double), intent(out) :: dObjectiveFunctionValue                    ! Valor calculado da função objetivo.
        real(c_double), dimension(iNComp), intent(out) :: oGradientVector         ! "Gradient vector" para uso com o Método de Newton
        real(c_double), dimension(iNComp, iNComp), intent(out) :: oHessianMatrix  ! "Hessian matrix" para uso com o Método de Newton.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iIER

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Inicializar:
        iIER = ERROR_EverythingOK

        ! Acionar os cálculos:
        call CalculateNewtonObjFunctionAndDerivsForTangentPlaneVLEAnalysis(oAlpha, oProblemParams, oCompProperties, iIER, &
                dObjectiveFunctionValue_Arg = dObjectiveFunctionValue, oGradientVector_Arg = oGradientVector, oHessianMatrix_Arg = oHessianMatrix)

        ! Tratar possível erro:
        checkIIER: if(iIER.NE.ERROR_EverythingOK) then
            oProblemParams(size(oProblemParams)) = iIER
            return
        end if checkIIER

    end subroutine CalculateStabAnalysisNewtonObjFunctionAndDerivatives

    ! =============================================================
    ! =============================================================
    subroutine CalculateNewtonObjFunctionAndDerivsForTangentPlaneVLEAnalysis(oAlpha, oProblemParams, oCompProperties, iIER, &
                dObjectiveFunctionValue_Arg, oGradientVector_Arg, oHessianMatrix_Arg)

        ! OBJETIVO: Centralizar os cálculos de função objetivo, derivadas e matrizes necessários à aplicação do
        !           Método de Newton na verificação de estabilidade.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), dimension(:), intent(in) :: oAlpha              ! Variáveis independentes (composição "W" transformada conforme
                                                                        ! equação 53 do Capítulo 10 da Referência Bibliográfica 1)
        real(c_double), dimension(size(oAlpha)+6), intent(in) :: oProblemParams          ! Vetor de parâmetros contendo variáveis do cálculo.
        real(c_double), dimension(2*size(oAlpha)+4, size(oAlpha)), intent(in) :: oCompProperties    ! Matriz de propriedades dos componentes

        integer(c_int), intent(out) :: iIER                                  ! Código de erros, conforme convencionado.
        real(c_double), intent(out), optional :: dObjectiveFunctionValue_Arg ! Valor calculado da função objetivo.
        real(c_double), dimension(size(oAlpha)), intent(out), optional :: oGradientVector_Arg   ! "Gradient vector" para uso com o Método de Newton
        real(c_double), dimension(size(oAlpha), size(oAlpha)), intent(out), optional :: oHessianMatrix_Arg  ! "Hessian matrix" para uso com o Método de Newton.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iNComp, i, j
        real(c_double), dimension(size(oAlpha)) :: oW
        real(c_double), dimension(size(oAlpha)) :: oTc_Loc             ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(size(oAlpha)) :: oPc_Loc             ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(size(oAlpha)) :: oW_Loc              ! Vetor de fatores acêntricos dos componentes.
        real(c_double), dimension(size(oAlpha),size(oAlpha)) :: oKij_Loc     ! Matriz dos parâmetros de interação "kij".
        real(c_double), dimension(size(oAlpha),size(oAlpha)) :: oLij_Loc     ! Matriz dos parâmetros "lij".
        real(c_double), dimension(size(oAlpha)) :: oPeneloux_Loc       ! Vetor dos "volumes de shift" para cada componente.
        real(c_double) :: dTemperature_Loc
        real(c_double) :: dPressure_Loc
        integer(c_int) :: iTrialPhaseID_Loc
        integer(c_int) :: iTrialPhaseThermodynamicModel_Loc
        real(c_double), dimension(size(oAlpha)) :: oD_Loc
        real(c_double), dimension(size(oAlpha)) :: oWLnFug
        real(c_double), dimension(size(oAlpha), size(oAlpha)) :: oDerivLnCoefFugWithNj_W
        real(c_double) :: dTPD                                  ! "Tangent plane distance" calculada.
        real(c_double), dimension(size(oAlpha)) :: oGradientVector
        real(c_double), dimension(size(oAlpha), size(oAlpha)) :: oHessianMatrix, oPhiIJ
        real(c_double) :: dN

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! "Voltar" para as variáveis independentes originais ("W"):
        iNComp = size(oAlpha)

        returnToW: do i = 1, iNComp
            oW(i) = oAlpha(i) * oAlpha(i) / 4.0d0
        end do returnToW

        ! Obter parâmetros do cálculo necessários:
        call GetStabilityAnalysisVariablesFromParametersVector(iNComp, oProblemParams, dTemperature = dTemperature_Loc, &
                dPressure = dPressure_Loc, iTrialPhaseID = iTrialPhaseID_Loc, &
                iTrialPhaseThermodynamicModel = iTrialPhaseThermodynamicModel_Loc, oD = oD_Loc)

        ! Obter propriedades necessárias das espécies químicas:
        call GetComponentPropertiesFromParameterMatrix(iNComp, oCompProperties, oTc = oTc_Loc, oPc = oPc_Loc, oW = oW_Loc, oKij = oKij_Loc, &
                    oLij = oLij_Loc, oPeneloux = oPeneloux_Loc)

        ! Coeficientes de fugacidade e derivadas para a "composição W":
        call CalculateLnFugAndDerivativesForEachComponent(iNComp, iTrialPhaseID_Loc, dTemperature_Loc, dPressure_Loc, oW, &
             iTrialPhaseThermodynamicModel_Loc, oTc_Loc, oPc_Loc, oW_Loc, oKij_Loc, oLij_Loc, oPeneloux_Loc, iIER, oLnFugArg = oWLnFug, &
             oDerivLnCoefFugWithNjArg = oDerivLnCoefFugWithNj_W)

        if(iIER.NE.ERROR_EverythingOK) return

        ! Calcular a função objetivo:
        call CalculateTPD(iNComp, oW, oWLnFug, oD_Loc, dTPD)

        if(present(dObjectiveFunctionValue_Arg)) dObjectiveFunctionValue_Arg = dTPD

        ! Calcular o "gradient vector" (equação 54 do Capítulo 10 da Referência Bibliográfica 1):
        calcGradVector: do i = 1, iNComp

            if(oW(i).gt.(0.0d0)) then
                oGradientVector(i) = sqrt(oW(i)) * (log(oW(i)) + oWLnFug(i) - oD_Loc(i))
            else
                oGradientVector(i) = 0.0d0
            end if

        end do calcGradVector

        if(present(oGradientVector_Arg)) oGradientVector_Arg = oGradientVector

        ! Calcular a matriz Hessiana para o método de Newton (equação 55 do Capítulo 10 da Referência Bibliográfica 1):
        dN = sum(oW)
        oPhiIJ = dN * oDerivLnCoefFugWithNj_W      ! Eq 12 do Capítulo 10 da Referência Bibliográfica 1
        oHessianMatrix = oPhiIJ

        calcHMatrix_outer: do i = 1, iNComp

            calcHMatrix_inner: do j = 1, iNComp

                oHessianMatrix(i, j) = oHessianMatrix(i, j) * sqrt(oW(i) * oW(j))

                if(i.eq.j) oHessianMatrix(i, j) = oHessianMatrix(i, j) + 1.0d0

            end do calcHMatrix_inner

        end do calcHMatrix_outer

        if(present(oHessianMatrix_Arg)) oHessianMatrix_Arg = oHessianMatrix

    end subroutine CalculateNewtonObjFunctionAndDerivsForTangentPlaneVLEAnalysis

    ! =============================================================
    ! =============================================================
    subroutine AdditionalNewtonConvergenceCheckForTangentPlaneVLEAnalysis(iXLength, oX, oPreviousX, iIter, dObjectiveFunctionValue, &
            dPreviousObjectiveFunctionValue, dRelativeTolerance, dCurrentError, bConverged)

        ! OBJETIVO: Fazer verificações específicas do atingimento ou não de convergência numérica do Método de Newton
        !           quando aplicado à verificação de estabilidade, de modo a complementar a verificação "default" da
        !           rotina "genérica" de Newton.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Thermodynamic Models: Fundamentals and Computational Aspects",
        !       Michael L. Michelsen e Jorgen M. Mollerup, Segunda Edição
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iXLength                             ! Número de variáveis independentes (NÃO MUDAR AQUI).
        real(c_double), dimension(iXLength), intent(in) :: oX    ! Valores atuais das variáveis independentes (NÃO MUDAR AQUI).
        real(c_double), dimension(iXLength), intent(in) :: oPreviousX    ! Variáveis independentes na iteração anterior (NÃO MUDAR AQUI).
        integer, intent(in) :: iIter                                    ! Variável homônima em "NewtonMinimization_V1" (NÃO MUDAR AQUI).
        real(c_double), intent(in) :: dObjectiveFunctionValue           ! Variável homônima em "NewtonMinimization_V1" (NÃO MUDAR AQUI).
        real(c_double), intent(in) :: dPreviousObjectiveFunctionValue   ! Variável homônima em "NewtonMinimization_V1" (NÃO MUDAR AQUI).
        real(c_double), intent(in) :: dRelativeTolerance                ! Variável homônima em "NewtonMinimization_V1" (NÃO MUDAR AQUI).
        real(c_double), intent(in) :: dCurrentError                     ! Variável homônima em "NewtonMinimization_V1" (NÃO MUDAR AQUI).

        logical, intent(inout) :: bConverged            ! ENTRADA: valor atual de "bConverged" em "NewtonMinimization_V1".
                                                        ! SAÍDA: Decisão final se já convergiu ou não.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double), dimension(iXLength) :: oW, oPreviousW, oWAbsDiff, oWRelDiff
        integer :: i
        logical :: bWConverged, bNewCurrentErrorConverged
        real(c_double) :: dNewOffsetInErrorCalc, dNewCurrentError

        ! ------------ CONSTANTES:
        logical, parameter :: bAllowAdditionalCheck = .FALSE.     ! "True" para ativar esta verificação adicional, "false" para desativar.
        real(c_double), parameter :: dWRelTol = 0.02d0           ! Tolerância relativa entre valores de "W" para verificar convergência.
        real(c_double), parameter :: dWAbsTol = 0.005d0          ! Tolerância absoluta entre valores de "W" para verificar convergência.

        ! ------------ PROCEDIMENTOS:

        ! Somente proceder se esta checagem adicional for desejada:
        if(.not.bAllowAdditionalCheck) return

        ! Experimentar com versão modificada do critério original:
        errorCalcWithOffset: if(abs(dPreviousObjectiveFunctionValue).lt.(0.011802d0)) then
            dNewOffsetInErrorCalc = 1.0d0
        else errorCalcWithOffset
            dNewOffsetInErrorCalc = 0.0d0
        end if errorCalcWithOffset

        dNewCurrentError = abs( (dObjectiveFunctionValue - dPreviousObjectiveFunctionValue + dNewOffsetInErrorCalc) / &
                                 (dPreviousObjectiveFunctionValue + dNewOffsetInErrorCalc) - &
                                 dNewOffsetInErrorCalc)

        bNewCurrentErrorConverged = (dNewCurrentError.lt.dRelativeTolerance)

        bConverged = bConverged.OR.bNewCurrentErrorConverged

        if(.true.) return ! VERIFICAÇÃO ABAIXO DE FRAÇÃO MOLAR PRECISA DE AJUSTES NA TOLERÂNCIA.
                          ! Essas tolerâncias precisam ser muuuito reduzidas (relativa pra coisa de 0.000001 e absoluta pra 0.00000025).


        ! Converter de "alfa" para fração molar (equação 53 da pág 269 da Referência 1):
        oW = oX * oX / 4.0d0
        oPreviousW = oPreviousX * oPreviousX / 4.0d0

        ! Calcular vetores de diferenças absolutas e relativas entre iterações:
        oWAbsDiff = abs(oW - oPreviousW)
        oWRelDiff = abs(oW - oPreviousW) / abs(oW)

        ! Comparar com tolerâncias:
        bWConverged = .true.

        comp: do i = 1, iXLength

            bWConverged = bWConverged.AND.( &
                (oWRelDiff(i).le.dWRelTol).OR. &
                (oWAbsDiff(i).le.dWAbsTol) &
                )

        end do comp

        ! Incluir convergência de "W" no resultado final:
        bConverged = bConverged.OR.bWConverged

        ! PAREI AQUI EM 8-JUL-2025
        ! No caso do fluido P52, os pontos com erro... parecem estar sendo causados por Newton encaminhar uma solução
        ! "posterior" à convergida. Estou vendo casos em que converge estável mas depois dá negativo...

    end subroutine AdditionalNewtonConvergenceCheckForTangentPlaneVLEAnalysis

end module Newton

