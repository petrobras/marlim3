module ViscosityCalculationModule

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use Constants

    implicit none

contains

    ! =============================================================
    ! =============================================================
    subroutine MethaneDensityObjectiveFunction(dTemperature, dPressure, dDensity, dPobj)

        ! OBJETIVO: Calcular a função objetivo que a massa específica do metano precisa satisfazer ("zerar") de acordo
        !   com a referência: artigo "A Modified Benedict-Webb-Rubin equation of state for methane using recent experimental
        !   data", de R. D. McCarty (Cryogenics, 1974) (https://doi.org/10.1016/0011-2275(74)90228-8)
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), intent(in) :: dTemperature                      ! Temperatura no SI
        real(c_double), intent(in) :: dPressure                         ! Pressão no SI
        real(c_double), intent(in) :: dDensity                          ! Massa específica em g/L
        real(c_double), intent(out) :: dPobj                            ! Valor calculado da função objetivo.

        ! ---------------- CONSTANTES:

            ! Constantes da tabela 2 do artigo
        real(c_double), parameter :: N1 = -1.8439486666d-02
        real(c_double), parameter :: N2 = 1.0510162064
        real(c_double), parameter :: N3 = -16.057820303
        real(c_double), parameter :: N4 = 8.4844027562d02
        real(c_double), parameter :: N5 = -4.2738409106d04
        real(c_double), parameter :: N6 = 7.6565285254d-04
        real(c_double), parameter :: N7 = -4.8360724197d-01
        real(c_double), parameter :: N8 = 85.195473835
        real(c_double), parameter :: N9 = -1.6607434721d04
        real(c_double), parameter :: N10 = -3.7521074532d-05
        real(c_double), parameter :: N11 = 2.8616309259d-02
        real(c_double), parameter :: N12 = -2.8685285973
        real(c_double), parameter :: N13 = 1.1906973942d-04
        real(c_double), parameter :: N14 = -8.5315715699d-03
        real(c_double), parameter :: N15 = 3.8365063841
        real(c_double), parameter :: N16 = 2.4986828379d-05
        real(c_double), parameter :: N17 = 5.7974531455d-06
        real(c_double), parameter :: N18 = -7.1648329297d-03
        real(c_double), parameter :: N19 = 1.2577853784d-04
        real(c_double), parameter :: N20 = 2.2240102466d04
        real(c_double), parameter :: N21 = -1.4800512328d06
        real(c_double), parameter :: N22 = 50.498054887
        real(c_double), parameter :: N23 = 1.6428375992d06
        real(c_double), parameter :: N24 = 2.1325387196d-01
        real(c_double), parameter :: N25 = 37.791273422
        real(c_double), parameter :: N26 = -1.1857016815d-05
        real(c_double), parameter :: N27 = -31.630780767
        real(c_double), parameter :: N28 = -4.1006782941d-06
        real(c_double), parameter :: N29 = 1.4870043284d-03
        real(c_double), parameter :: N30 = 3.1512261532d-09
        real(c_double), parameter :: N31 = -2.1670774745d-06
        real(c_double), parameter :: N32 = 2.4000551079d-05

        real(c_double), parameter :: gama = 0.0096
        real(c_double), parameter :: R = 0.08205616
        real(c_double), parameter :: MW = 16.0428

        ! ----------------- DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double) :: dRho, dRho2, dRho3, dRho4, dRho5, dRho6, dRho7, dRho8, dRho9, dRho11, dRho13
        real(c_double) :: dexp

        real(c_double) :: P, T, T2, T3, T4, T12, Pobj

        ! ----------------- PROCEDIMENTOS E CÁLCULOS:

        ! Conversões e grandezas necessárias:
        dRho = dDensity/MW      !conversão para mol/litro
        P = dPressure/101325.0  !conversão para atm
        T = dTemperature
        T2  = T*T
        T3 = T2*T
        T4 = T3*T
        T12 = sqrt(T)

        ! Potências de "rho":
        dRho2 = dRho*dRho
        dRho3 = dRho2*dRho
        dRho4 = dRho3*dRho
        dRho5 = dRho4*dRho
        dRho6 = dRho5*dRho
        dRho7 = dRho6*dRho
        dRho8 = dRho7*dRho
        dRho9 = dRho8*dRho
        dRho11 = dRho9*dRho2
        dRho13 = dRho11*dRho2

        ! Exponencial que aparece na Equação 1 do artigo:
        dexp = exp(-gama*dRho2)

        ! Equação 1 do artigo:
        Pobj = dRho*R*T
        Pobj = Pobj + dRho2*(N1*T + N2*T12 + N3 + N4/T + N5/T2)
        Pobj = Pobj + dRho3*(N6*T + N7 + N8/T + N9/T2)
        Pobj = Pobj + dRho4*(N10*T + N11 + N12/T)
        Pobj = Pobj + dRho5*N13
        Pobj = Pobj + dRho6*(N14/T + N15/T2)
        Pobj = Pobj + dRho7*(N16/T)
        Pobj = Pobj + dRho8*(N17/T + N18/T2)
        Pobj = Pobj + dRho9*(N19/T2)
        Pobj = Pobj + dRho3*(N20/T2 + N21/T3)*dexp
        Pobj = Pobj + dRho5*(N22/T2 + N23/T4)*dexp
        Pobj = Pobj + dRho7*(N24/T2 + N25/T3)*dexp
        Pobj = Pobj + dRho9*(N26/T2 + N27/T4)*dexp
        Pobj = Pobj + dRho11*(N28/T2 + N29/T3)*dexp
        Pobj = Pobj + dRho13*(N30/T2 + N31/T3 + N32/T4)*dexp

        ! Calcular a função objetivo que a massa específica tem que zerar:
        dPobj = P - Pobj

    end subroutine MethaneDensityObjectiveFunction

    ! =============================================================
    ! =============================================================
    subroutine CalculateMethaneDensity(dTemperature, dPressure, dDensity, converged)

        ! OBJETIVO: Calcular a massa específica do metano (para fins de cálculo de viscosidade) a partir da equação de estado BWR apresentada
        !   na referência: artigo "A Modified Benedict-Webb-Rubin equation of state for methane using recent experimental
        !   data", de R. D. McCarty (Cryogenics, 1974) (https://doi.org/10.1016/0011-2275(74)90228-8)
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), intent(in) :: dTemperature  ! Temperatura de interesse, em K
        real(c_double), intent(in) :: dPressure     ! Pressão de interesse, em Pa
        real(c_double), intent(inout) :: dDensity   ! Estimativa inicial de massa específica
        logical, intent(out) :: converged           ! Indica se o procedimento convergiu. Em caso positivo, o valor da massa específica será
                                                    !   retornado em dDensity

        ! ----------------- DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double) :: dTempDensity1
        real(c_double) :: dTempDensity2
        real(c_double) :: dPobj1
        real(c_double) :: dPobj2
        integer :: iCount

        ! ---------------- CONSTANTES:
        integer, parameter :: IMAXCOUNT = 1000
        real(c_double), parameter :: dRelativeToleranceForRoot = 5.0d-7
        real(c_double), parameter :: dAbsoluteToleranceForObjectiveFunction = 5.0d-7

        ! ----------------- PROCEDIMENTOS E CÁLCULOS:
        converged = .false.

        ! Método da Secante
        dTempDensity1 = dDensity
        dTempDensity2 = dDensity + 20.0

        call MethaneDensityObjectiveFunction(dTemperature, dPressure, dTempDensity1, dPobj1)
        call MethaneDensityObjectiveFunction(dTemperature, dPressure, dTempDensity2, dPobj2)

        do iCount = 1, IMAXCOUNT
            dDensity = dTempDensity1*dPobj2 - dTempDensity2*dPobj1
            dDensity = dDensity/(dPobj2 - dPobj1)

            dTempDensity1 = dTempDensity2
            dTempDensity2 = dDensity
            dPobj1 = dPobj2

            call MethaneDensityObjectiveFunction(dTemperature, dPressure, dTempDensity2, dPobj2)

            converged = dabs((dTempDensity1 - dTempDensity2)/dTempDensity2).le.dRelativeToleranceForRoot
            converged = converged .and. (dabs(dPobj2).le.dAbsoluteToleranceForObjectiveFunction)

            if (converged) then
                return
            end if

        end do

    end subroutine CalculateMethaneDensity

    ! =============================================================
    ! =============================================================
    subroutine CalculateMethaneViscosity(dRhoco, dRhoRef, dTRef, dEta)

        ! OBJETIVO: Calcular a viscosidade do metano (para fins de uso como referência nos cálculos de viscosidades
        !           de frações de petróleo, conforme descrito no Livro-Referência abaixo).
        implicit none

        ! LIVRO-REFERÊNCIA: "Properties of Oils and Natural Gases", de Pedersen, Fredenslund e
        !                           Thomassen, edição de 1989.

        ! ARTIGO-REFERÊNCIA: "Equations for the Viscosity and Thermal Conductivity Coefficients of Methane",
        !                           Hanley, McCarty e Haynes (Cryogenics, 1975)!

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), intent(in) :: dRhoco
        real(c_double), intent(in) :: dRhoRef
        real(c_double), intent(in) :: dTRef
        real(c_double), intent(out) :: dEta

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dEta0, dEta1, dDeltaEtaPrime, dDeltaEtaPrime2
        real(c_double) :: dHTAN, dF1, dF2, dDeltaT

        ! ---------------- CONSTANTES:
        real(c_double), parameter :: dTFP = 90.675  ! Freezing point do metano. Obtido do multiflash

            ! Constantes pata calculo da viscosidade de referência
            ! Fonte: tabela 4 do artigo-referência.
        real(c_double), parameter, dimension(9) :: GV = (/  -2.090975e5, &
            2.647269e5, &
            -1.472818e5, &
            4.716740e4, &
            -9.491872e3, &
            1.219979e3, &
            -9.627993e1, &
            4.274152, &
            -8.141531e-2 /)

            ! Constantes das correlações de Hanley
            ! Fonte: tabela 2 do artigo-referência
        real(c_double), parameter :: A = 1.696985927
        real(c_double), parameter :: B = -0.133372346
        real(c_double), parameter :: C = 1.4
        real(c_double), parameter :: F = 168.0

        real(c_double), parameter :: j1 = -1.035060586d1
        real(c_double), parameter :: j2 = 1.7571599671d1
        real(c_double), parameter :: j3 = -3.0193918656d3
        real(c_double), parameter :: j4 = 1.8873011594d2
        real(c_double), parameter :: j5 = 4.2903609488d-2
        real(c_double), parameter :: j6 = 1.4529023444d2
        real(c_double), parameter :: j7 = 6.1276818706d3

            ! Constantes das correlações de Pedersen e Fredenslund
            ! Fonte: equação 11-40 do Livro-Referência
        real(c_double), parameter :: k1 = -9.74602
        real(c_double), parameter :: k2 = 18.0834
        real(c_double), parameter :: k3 = -4126.66
        real(c_double), parameter :: k4 = 44.6055
        real(c_double), parameter :: k5 = 0.9676544
        real(c_double), parameter :: k6 = 81.8134
        real(c_double), parameter :: k7 = 15649.9

        ! ------------------ CÁLCULOS:

        ! Equação 9 do artigo-referência:
        dEta0 = GV(1)/dTRef
        dEta0 = dEta0 + GV(2)/(dTRef ** (2.0/3.0))
        dEta0 = dEta0 + GV(3)/(dTRef ** (1.0/3.0))
        dEta0 = dEta0 + GV(4)
        dEta0 = dEta0 + GV(5)*(dTRef ** (1.0/3.0))
        dEta0 = dEta0 + GV(6)*(dTRef ** (2.0/3.0))
        dEta0 = dEta0 + GV(7)*dTRef
        dEta0 = dEta0 + GV(8)*(dTRef ** (4.0/3.0))
        dEta0 = dEta0 + GV(9)*(dTRef ** (5.0/3.0))

        ! Equação 3 do artigo-referência:
        dEta1 = A + B*((C - log(dTRef/F))**2)

        ! hanley, 1975
        ! Equação 11-38 do Livro-Referência (apresentada também no manual do PVTSIM)
        dDeltaEtaPrime = ((dRhoRef/1000.0)**0.1) * (j2 + j3/(dTRef ** (3.0/2.0)))
        dDeltaEtaPrime = dDeltaEtaPrime + ((dRhoRef - dRhoco)/dRhoco)*sqrt(dRhoRef/1000.0)*(j5 + j6/dTRef + j7/(dTRef*dTRef))
        dDeltaEtaPrime = exp(dDeltaEtaPrime) - 1.0
        dDeltaEtaPrime = exp(j1 + j4/dTRef)*dDeltaEtaPrime

        ! Pedersen e Fredenslund, 1987
        ! Equação 11-40 do Livro-Referência (apresentada também no manual do PVTSIM).
        dDeltaEtaPrime2 = ((dRhoRef/1000.0)**0.1) * (k2 + k3/(dTRef ** (3.0/2.0)))
        dDeltaEtaPrime2 = dDeltaEtaPrime2 + ((dRhoRef - dRhoco)/dRhoco)*sqrt(dRhoRef/1000.0)*(k5 + k6/dTRef + k7/(dTRef*dTRef))
        dDeltaEtaPrime2 = exp(dDeltaEtaPrime2) - 1.0
        dDeltaEtaPrime2 = exp(k1 + k4/dTRef)*dDeltaEtaPrime2

        ! Equação 11-41 do Livro-Referência (também apresentadas no manual do PVTSIM)
        dDeltaT = dTRef - dTFP
        dHTAN = (exp(dDeltaT) - exp(-dDeltaT))/(exp(dDeltaT) + exp(-dDeltaT))
        dF1 = (1.0 + dHTAN)/2.0
        dF2 = (1.0 - dHTAN)/2.0

        ! Equação 11-41 do Livro-Referência (aparece também no manual do PVTSIM):
        dEta = dEta0 + dEta1*(dRhoRef/1000.0) + dF1 * dDeltaEtaPrime + dF2 * dDeltaEtaPrime2

    end subroutine CalculateMethaneViscosity

    ! =============================================================
    ! =============================================================
    ! Calculo da viscosidade por Pedersen
    subroutine CalculateViscosityPedersen(dFlashPressure, dFlashTemperature, iNComp, oTc, oPc, oMW, &
                        oComposition, dEta, iIER)

        ! PAREI AQUI EM 27/7/2021:
        !   - equações todas rotuladas
        !   - conferir mais uma vez "por alto" a estrutura da rotina
        !   - observar que tem chamadas que retornam um "flag" de convergência... e o "flag" não está sendo checado.

        ! OBJETIVO:
        implicit none

        ! LIVRO-REFERÊNCIA: "Properties of Oils and Natural Gases", de Pedersen, Fredenslund e
        !           Thomassen, edição de 1989.

        ! ARTIGO-REFERÊNCIA: "Viscosity of Crude Oils", de Pedersen, Fredenslund, Christensen e Thomassen;
        !           "Chemical Engineering Science", 1984.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure      ! Pressão na qual deseja-se calcular a viscosidade
        real(c_double), value, intent(in) :: dFlashTemperature   ! Temperatura na qual deseja-se calcular a viscosidade
        integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
        real(c_double), dimension(iNComp) :: oTc                 ! Vetor de temperaturas críticas dos componentes.
        real(c_double), dimension(iNComp) :: oPc                 ! Vetor de pressões críticas dos componentes.
        real(c_double), dimension(iNComp) :: oMW                 ! Vetor de massas molares (indexado por componente).
        real(c_double), dimension(iNComp) :: oComposition        ! Composição molar

        real(c_double), intent(out) :: dEta                      ! Viscosidade da composição, em cP
        integer(c_int), intent(out) :: iIER                      ! Código de erros, conforme convenção.

        ! ---------------- CONSTANTES:

        ! Constantes das correlações de Pedersen
        !real(c_double), parameter :: b1 = 0.291
        !real(c_double), parameter :: b2 = 7.747d-5
        !real(c_double), parameter :: b3 = 4.265
        !real(c_double), parameter :: b4 = 0.8579
        !real(c_double), parameter :: b5 = 8.374d-4

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dTco
        real(c_double) :: dPco
        real(c_double) :: dWo
        real(c_double) :: dMWo
        real(c_double) :: dRhoco, dRhoR
        real(c_double) :: dMWn
        real(c_double) :: dMWw
        real(c_double) :: dMWmix
        real(c_double) :: dTcmix
        real(c_double) :: dPcmix
        real(c_double) :: dAlpha0
        real(c_double) :: dAlphaMix
        real(c_double) :: dPRef
        real(c_double) :: dTRef
        real(c_double) :: dRhoRef1, dRhoRef2, dRhoRef
        real(c_double) :: aux1, aux2, aux3, aux4
        real(c_double) :: num, denom
        real(c_double) :: dZRef
        logical :: converged
        integer :: i, j

        ! ------------------ CÁLCULOS:

        ! O metano é o terceiro elemento na sequencia de componentes
        ! Mas vamos usar os valores de tabela do Multiflash
        dPco = 45.39057 * 101325.0      !oPc(3)
        dTco = -82.586 + 273.15         !oTc(3)
        dWo = 0.0104                    !oW(3)
        dMWo = 16.0428                  !oMW(3)

        ! Neste ponto, seria recuperada a massa específica do metano na condicao crítica
        !        call Marlim_CalculateSingleComponentZAndRho(dTco, dPco, dWo, 0.0d0, dMWo, PHASE_Vapor, dPco, &
        !            dTco, PENG_ROBINSON_78_PENELOUX, dZRef, dRhoco, iIER)
        !
        !        call Marlim_CalculateSingleComponentZAndRho(dTco, dPco, dWo, 0.0d0, dMWo, PHASE_Liquid, dPco, &
        !            dTco, PENG_ROBINSON_78_PENELOUX, dZRef, dRhoco, iIER)

        dRhoco = 162.66 ! Forçando o valor obtido do Multiflash

        ! Cálculo da viscosidade do gás

        ! Equações 11-32 e 11-33 do Livro-Referência:
        dMWn = 0.0
        dMWw = 0.0

        do i = 1, iNComp
            aux1 = oComposition(i)*oMW(i)
            dMWn = dMWn + aux1
            dMWw = dMWw + aux1*oMW(i)
        end do

        dMWw = dMWw/dMWn

        ! ATENÇÃO (26/7/2021): TODO: Usar acima as massas molares corretas, em acordo com o manual do PVTSIM...

        ! ok até aqui! Seguir daqui (RGDT: Parei aqui em 15/7/2021)!
        ! Sempre comparar com o PVTSIM. A questão das massas molares surgiu acima e acho que fará diferença...
        !       e já que teria que importar os coeficientes...

        !Essa é a versão no artigo original de Pedersen
        !dMWmix = dMWn + b1*(dMWw - dMWn)

        ! Equação 11.31 do Livro-Referência, modificada com os coeficientes do PVTSIM:
        !   TODO: Acrescentar os coeficientes do ajuste do PVTSIM
        dMWmix =  dMWn + 1.304d-04 *((dMWw**2.303) - (dMWn**2.303))

        ! Equações 19 e 20 do Artigo-Referência.
        ! Também estão no manual do PVTSIM.
        ! Equações 11-27 e 11-30 do Livro-Referência (neste livro, a 11-30 com um pequeno erro em relação às outra duas referências):
        num = 0.0
        denom = 0.0
        do i = 1, iNComp
            do j = 1, iNComp
                aux1 = oComposition(i)*oComposition(j)
                aux2 = ((oTc(i)/oPc(i))**(1.0/3.0) + (oTc(j)/oPc(j))**(1.0/3.0))**3.0
                aux3 = sqrt(oTc(i)*oTc(j))

                aux4 = aux1*aux2

                num = num + aux4*aux3
                denom = denom + aux4
            end do
        end do

        dTcmix = num/denom
        dPcmix = 8.0*num/(denom*denom)

        ! Ver Equação 11-36 do livro-Referência:
        dPRef = dFlashPressure * dPco/dPcmix
        dTRef = dFlashTemperature * dTco/dTcmix

        ! Neste ponto, seria recuperada a massa específica do metano na condicao de referencia
        ! Massa específica de qual fase deve ser usada? No momento, escolhendo o maior valor
        !        call Marlim_CalculateSingleComponentZAndRho(dTco, dPco, dWo, 0.0d0, dMWo, PHASE_Vapor, dPRef, &
        !            dTRef, PENG_ROBINSON_78_PENELOUX, dZRef, dRhoRef1, iIER)
        !
        !        call Marlim_CalculateSingleComponentZAndRho(dTco, dPco, dWo, 0.0d0, dMWo, PHASE_Liquid, dPRef, &
        !            dTRef, PENG_ROBINSON_78_PENELOUX, dZRef, dRhoRef2, iIER)
        !
        !        if (dRhoRef1 > dRhoRef2) then
        !            dRhoRef = dRhoRef1
        !        else
        !            dRhoRef = dRhoRef2
        !        end if

        ! Massa específica por McCarty
        dRhoRef = 1000.0
        call CalculateMethaneDensity(dTRef, dPRef, dRhoRef, converged)

        ! Massa específica reduzida
        ! Equação 11-36 do livro-referência.
        ! Também tem no manual do PVTSIM.
        dRhoR = dRhoRef/dRhoco

        !Versões originais no artigo de Pedersen
        !dAlpha0 = 1.0 + b5 * (dRhoR **b3)
        !dAlphaMix = 1.0 + b2 * (dRhoR **b3)*(dMWmix**b4)

        ! Equações 11-35 e 11-34 do livro-Referência.
        dAlpha0 = 1.0 + 0.031 * (dRhoR **1.847)
        dAlphaMix = 1.0 + 7.378d-03 * (dRhoR **1.847)*(dMWmix**0.5173)

        ! Viscosidade
        ! A pressão e temperatura de referência mudam novamente

        ! Equações 11-20 e 11-21 do Livro-Referência.
        ! Também estão (com erros) no manual do PVTSIM.
        dPRef = dPRef * (dAlpha0/dAlphaMix)
        dTRef = dTRef * (dAlpha0/dAlphaMix)

        ! Neste ponto, a massa específica do metano na condicao de referencia precisa ser recalculada
        ! Massa específica de qual fase deve ser usada?
        !        call Marlim_CalculateSingleComponentZAndRho(dTco, dPco, dWo, 0.0d0, dMWo, PHASE_Vapor, dPRef, &
        !            dTRef, PENG_ROBINSON_78_PENELOUX, dZRef, dRhoRef1, iIER)
        !
        !        call Marlim_CalculateSingleComponentZAndRho(dTco, dPco, dWo, 0.0d0, dMWo, PHASE_Liquid, dPRef, &
        !            dTRef, PENG_ROBINSON_78_PENELOUX, dZRef, dRhoRef2, iIER)

        ! Massa específica do metano por McCarty
        dRhoRef = 1000.0 !Estimativa alta
        call CalculateMethaneDensity(dTRef, dPRef, dRhoRef, converged)

        ! Viscosidade do metano
        call CalculateMethaneViscosity(dRhoco, dRhoRef, dTRef, dEta)

        ! Calcular a viscosidade usando a expressão para "viscosity of a mixture" indicada pelo manual do PVTSIM para
        ! o "Corresponding States Method". Trata-se também da "final expression for the viscosity of a given mixture"
        ! correspondente à equação 11-19 do livro-referência.
        dEta = dEta * ((dTcmix/dTco) **(-1.0/6.0))
        dEta = dEta * ((dPcmix/dPco) **(2.0/3.0))
        dEta = dEta * sqrt(dMWmix/dMWo)
        dEta = dEta * (dAlphaMix/dAlpha0)

        ! Conversão para cP
        dEta = dEta * 0.0001

    end subroutine CalculateViscosityPedersen

    ! =============================================================
    ! =============================================================
    ! Interface com C/C++
    subroutine MarlimCalculateViscosityPedersen(dFlashPressure, dFlashTemperature, iNComp, oTc, oPc, oMW, &
        oComposition, dEta, iIER) bind(C, name = "MarlimCalculateViscosityPedersen")

            ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dFlashPressure      ! Pressão na qual deseja-se calcular a viscosidade
        real(c_double), value, intent(in) :: dFlashTemperature   ! Temperatura na qual deseja-se calcular a viscosidade
        integer(c_int), value, intent(in) :: iNComp              ! Número de componentes.
        type(c_ptr), value, intent(in) :: oTc                    ! Vetor de temperaturas críticas dos componentes.
        type(c_ptr), value, intent(in) :: oPc                    ! Vetor de pressões críticas dos componentes.
        type(c_ptr), value, intent(in) :: oMW                    ! Vetor de massas molares (indexado por componente).
        type(c_ptr), value, intent(in) :: oComposition ! Composição molar
        real(c_double), intent(out) :: dEta                     ! Viscosidade da composição, em cP
        integer(c_int), intent(out) :: iIER                      ! Código de erros, conforme convenção.

        real(c_double), dimension(:), pointer :: temp_oTc        ! Mapeamento do argumento "oTc"
        real(c_double), dimension(:), pointer :: temp_oPc        ! Mapeamento do argumento "oPc"
        real(c_double), dimension(:), pointer :: temp_oMW         ! Mapeamento do argumento "oMW"
        real(c_double), dimension(:), pointer :: temp_oComposition         ! Mapeamento do argumento "oComposition"

        call c_f_pointer(oTc, temp_oTc, [iNComp])
        call c_f_pointer(oPc, temp_oPc, [iNComp])
        call c_f_pointer(oMW, temp_oMW, [iNComp])
        call c_f_pointer(oComposition, temp_oComposition, [iNComp])

        call CalculateViscosityPedersen(dFlashPressure, dFlashTemperature, iNComp, temp_oTc, temp_oPc, temp_oMW, &
            temp_oComposition, dEta, iIER)

        iIER = 0
    end subroutine

end module ViscosityCalculationModule


! PENDÊNCIAS:
!   1) Importar do "ctm":
!       a) as massas molares que faltam, para suprir o "todo" que deixei registrado
!       b) os coeficientes do PVTSIM da massa molar da mistura (tb deixei anotado)
