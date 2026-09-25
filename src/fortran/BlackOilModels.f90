module BlackOilModels

    ! OBJETIVO: Conter as subrotinas que implementam cálculos de modelo black-oil para subsidiar outros propósitos desta biblioteca.
    ! UNIDADES: Diferentemente da grande maioria dos demais arquivos "*.f90" desta biblioteca, a programação contida neste módulo
    !       não adota o SI (Sistema Internacional) como padrão. Esta opção visa facilitar a comparação das equações implementadas
    !       com as respectivas referências bibliográficas, que raramente baseiam-se no SI.

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use Constants

    implicit none

    contains

    ! =============================================================
    !           ROTINA PARA CÁLCULOS DE Bo
    ! =============================================================
    subroutine CalculateFVFOfOilByCorrelations(iBoCorrelation, pres, temp, varPb, &
                                               dGivenRs, iRsCorrelation, iRsCorrelationForPSat, &
                                               dPSEP_VB, dTSEP_VB, API, RGO, Deng, &
                                               pbtemp, BoResult, iIER)

        ! OBJETIVO: Calcular "Bo" (fator volume de formação do óleo) usando correlações black-oil.
        !
        ! IMPORTANTE (consistência): esta rotina pode usar Rs e PB oriundos de modelos diferentes,
        !    dependendo dos valores de dGivenRs / iRsCorrelation / varPb / iRsCorrelationForPSat.
        !
        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iBoCorrelation          ! Índice da correlação escolhida para "Bo" (ver constantes abaixo)
        real(c_double), intent(in) :: pres                    ! Pressão (kgf/cm2)
        real(c_double), intent(in) :: temp                    ! Temperatura (°C)
        real(c_double), intent(in) :: varPb                   ! Pressão de bolha em psia (negativa para cálculo automático)

        real(c_double), intent(in) :: dGivenRs                ! Rs fornecido (m3/m3); se <0, calcular via correlação de Rs
        integer(c_int), intent(in) :: iRsCorrelation          ! Correlação de Rs para calcular Rs se dGivenRs<0; se <0, usar a mesma de Bo quando aplicável
        integer(c_int), intent(in) :: iRsCorrelationForPSat   ! Correlação de Rs para calcular PB se varPb<0; se <0, usar a mesma de Bo quando aplicável (exceto Bacia de Campos -> erro)

        real(c_double), intent(in) :: dPSEP_VB                ! Pressão do separador (kgf/cm2) (usada p/ SG100 e/ou Rs/PB)
        real(c_double), intent(in) :: dTSEP_VB                ! Temperatura do separador (°C)  (usada p/ SG100 e/ou Rs/PB)
        real(c_double), intent(in) :: API                     ! Grau API do óleo
        real(c_double), intent(in) :: RGO                     ! RGO (Sm3/Sm3)
        real(c_double), intent(in) :: Deng                    ! gammaG (ar=1), adimensional (densidade relativa do gás produzido)

        real(c_double), intent(out) :: pbtemp                 ! Pressão de bolha usada (psia)
        real(c_double), intent(out) :: BoResult               ! Resultado de Bo (adimensional, m3/m3)
        integer(c_int), intent(out) :: iIER                   ! Código de erro, conforme convenção

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:

        ! Conversões / constantes auxiliares:
        real(c_double), parameter :: KGFCM2_TO_PSIA = 14.2233426d0
        real(c_double), parameter :: ATM_TOPSI      = 14.6959488d0
        real(c_double), parameter :: SM3SM3_TO_SCFSTB = 35.31467d0 / 6.29d0  ! ~5.61458 (consistente com rotina de Rs)
        real(c_double), parameter :: EPS = 1.0d-15

        ! Variáveis internas em unidades do equacionamento original:
        real(c_double) :: P_psia, T_F
        real(c_double) :: PB_psia
        real(c_double) :: R_scfstb
        real(c_double) :: RS_scfstb

        ! Variáveis auxiliares das correlações (mesmos símbolos do código original):
        real(c_double) :: A, B, C, D, BOB, CO, SGO
        real(c_double) :: F1, RHOS, PD, FBO

        ! SG100 (densidade do gás a 100 psig e T): no Pascal vem pronto; aqui estimamos conforme lógica VB.
        real(c_double) :: pSep_psia, tSep_F
        real(c_double) :: SG100             ! Referência 1, pág 104: gas specific gravity [at] separator pressure of 100 psig

        ! Controle de chamadas da rotina de Rs:
        integer(c_int) :: iCorrRS, iCorrPB
        real(c_double) :: dPbFromRs, dRsFromCorr
        integer(c_int) :: iIER_Rs

        ! ------------------ CÁLCULOS:

        ! 1) ---> CONVERSÕES E INICIALIZAÇÕES:

        ! Inicializações:
        iIER = ERROR_EverythingOK
        BoResult = -1.0d0               ! Inicializando com valor inválido

        ! Converter pressões e temperaturas para unidades internas:
        P_psia = pres * KGFCM2_TO_PSIA      ! de kgf/cm2 para psia
        T_F    = 1.8d0 * temp + 32.0d0      ! de oC para oF 

        pSep_psia = dPSEP_VB * KGFCM2_TO_PSIA   ! de kgf/cm2 para psia
        tSep_F    = 1.8d0 * dTSEP_VB + 32.0d0   ! de oC para oF

        ! "d60": Referência 1, pág 103, Equação B-5
        SGO = 141.5d0 / (131.5d0 + API)

        ! Converter RGO de Sm3/Sm3 para scft/bbl:
        R_scfstb = RGO * SM3SM3_TO_SCFSTB

        ! 2) ---> CORRELAÇÃO PARA PSAT (caso necessário):

        ! Determinar correlação para cálculo automático da pressão de bolha (se varPb < 0):
        iCorrPB = iRsCorrelationForPSat

        chkBoCorrForPSat: if ((iCorrPB.lt.0).AND.(varPb.lt.(0.0d0))) then

            ! Usar a mesma correlação de Bo quando aplicável.
            atribBoCorrToPSat: select case (iBoCorrelation)
                    
                case (BOCORRELATION_VAZQUEZBEGGS) atribBoCorrToPSat
                    iCorrPB = RSCORRELATION_VAZQUEZBEGGS
                case (BOCORRELATION_STANDING) atribBoCorrToPSat
                    iCorrPB = RSCORRELATION_STANDING
                case (BOCORRELATION_GLASO) atribBoCorrToPSat
                    iCorrPB = RSCORRELATION_GLASO
                case (BOCORRELATION_BACIACAMPOS) atribBoCorrToPSat

                    ! Esta correlação de "Bo" não calcula pressão de bolha.
                    iIER = ERROR_MissingRsCorrelationForPSat
                    return

                case default atribBoCorrToPSat

                    ! Correlação inválida para "Bo"!
                    iIER = ERROR_InvalidBoCorrelation
                    return

            end select atribBoCorrToPSat

        end if chkBoCorrForPSat

        ! 3) ---> CORRELAÇÃO PARA RS (caso necessário):

        ! Determinar correlação para "Rs" (a ser usada quando dGivenRs < 0 e "Rs" for necessário):
        iCorrRS = iRsCorrelation

        chkBoCorrForRs: if((iCorrRS.lt.0).AND.(dGivenRs.lt.(0.0d0))) then

            ! Usar a mesma correlação de "Bo" quando aplicável.
            atribBoCorrToRs: select case (iBoCorrelation)

                case (BOCORRELATION_VAZQUEZBEGGS) atribBoCorrToRs
                    iCorrRS = RSCORRELATION_VAZQUEZBEGGS
                case (BOCORRELATION_STANDING) atribBoCorrToRs
                    iCorrRS = RSCORRELATION_STANDING
                case (BOCORRELATION_GLASO) atribBoCorrToRs
                    iCorrRS = RSCORRELATION_GLASO
                case (BOCORRELATION_BACIACAMPOS) atribBoCorrToRs

                    ! Para "Bo" por Bacia de Campos, "Rs" não é necessário. Não é erro.
                    iCorrRS = -1

                case default atribBoCorrToRs 

                    ! Correlação inválida para "Bo"!
                    iIER = ERROR_InvalidBoCorrelation
                    return

            end select atribBoCorrToRs

        end if chkBoCorrForRs

        ! 4) ---> DETERMINAÇÃO DA PSAT:

        ! Determinando a pressão de saturação:
        PB_psia = varPb

        dirPbCalc: if(PB_psia.lt.(0.0d0)) then

            ! Calcular a pressão de bolha usando a correlação especificada / determinada:
            call CalculateBlackOilSolutionGasOilRatio(iCorrPB, pres, temp, varPb, dPSEP_VB, dTSEP_VB, &
                                                        API, RGO, Deng, dPbFromRs, dRsFromCorr, iIER)

            if(iIER.NE.ERROR_EverythingOK) return

            PB_psia = dPbFromRs
            RS_scfstb = dRsFromCorr     ! Armazenar "provisoriamente", para o caso de as correlações de Rs e Pb coincidirem!

        end if dirPbCalc

        pbtemp = PB_psia    ! Argumento de saída

        ! 5) ---> DETERMINAÇÃO DE RS:

        ! Determinar "Rs" para uso nas situações necessárias:
        dirRsCalc: if (iBoCorrelation.EQ.BOCORRELATION_BACIACAMPOS) then

            ! "Bo" por "Bacia de Campos": "Rs" não é necessário; podemos ignorar "dGivenRs" / "iRsCorrelation" para "Bo".
            RS_scfstb = 0.0d0

        else dirRsCalc

            ! Vai precisar de "Rs"!

            chkGaveRs: if (dGivenRs.ge.(0.0d0)) then

                ! Adotar o "Rs" fornecido
                RS_scfstb = dGivenRs * SM3SM3_TO_SCFSTB

            else chkGaveRs

                ! Calcular "Rs" por correlação.
                calcPBAndRs: if ((varPb.lt.(0.0d0)).AND.(iCorrPB.eq.iCorrRS)) then

                    ! Cálculo de Pb já realizado, e pela mesma correlação de Rs.
                    ! "Rs" já foi determinado, nada mais a fazer aqui!

                else calcPBAndRs

                    ! Pressão de bolha já conhecida, ou calculada por outra correlação que não a de "Rs".
                    ! Em ambos os casos, calcular agora "Rs" pela devida correlação.
                    call CalculateBlackOilSolutionGasOilRatio(iCorrRS, pres, temp, PB_psia, dPSEP_VB, &
                            dTSEP_VB, API, RGO, Deng, dPbFromRs, dRsFromCorr, iIER)

                    if(iIER.NE.ERROR_EverythingOK) return

                    RS_scfstb = dRsFromCorr

                end if calcPBAndRs

            end if chkGaveRs

        end if dirRsCalc

        ! 6) ---> CÁLCULO DO BO DE ACORDO COM A CORRELAÇÃO SELECIONADA:

        ! Calcular SG100 antes (aqui,pois afeta mais de uma correlação).

            ! Equação B-9 da pág 104 da Referência 1:
        SG100 = 5.912d-5 * API * tSep_F * log10(pSep_psia / 114.7d0)
        SG100 = Deng * (1.0d0 + SG100)
        if (SG100.lt.(0.5538d0)) SG100 = 0.5538d0   ! Metano (de acordo com o código-fonte do MARLIM 2).
        
            ! Proceder de acordo com a correlação selecionada:
        calcBoByCorr: select case (iBoCorrelation)

            case (BOCORRELATION_VAZQUEZBEGGS) calcBoByCorr
                ! --- CALFVF, JCODE=0 (Vazquez & Beggs)

                    ! Ver Referência 1, pág 105, Equação B-18
                D = (T_F - 60.0d0) * API / SG100

                    ! Referência 1, pág 106, Tabela B-3:
                VBCtes: if ((API > 30.0d0).or.(R_scfstb > 750.0d0)) then
                    A = 4.670d-04
                    B = 1.100d-05
                    C = 1.337d-09
                else VBCtes
                    A = 4.677d-04
                    B = 1.751d-05
                    C = -1.811d-08
                end if VBCtes

                    ! Proceder de acordo com a pressão de bolha:
                vbBoCorr: if (P_psia >= PB_psia) then

                        ! Referência 1, pág 105, Equação B-18:
                    BOB = 1.0d0 + A*R_scfstb + D*(B + C*R_scfstb)

                        ! Referência 1, pág 106, Equação B-19:
                    CO  = (-1433.0d0 + 5.0d0*R_scfstb + 17.2d0*T_F - 1180.0d0*SG100 + 12.61d0*API) / (P_psia*1.0d5)

                        ! TODO: Verificar, no 2o termo acima, faz entido estar usando RGO em vez do Rs
                        ! por estar acima de PB? MARLIM 2 também faz assim?
                    
                        ! Referência 1, pág 105, Equação B-16:
                    BoResult = BOB * exp(CO * (PB_psia - P_psia))

                else vbBoCorr

                        ! Referência 1, pág 105, Equação B-18:
                    BoResult = 1.0d0 + A*RS_scfstb + D*(B + C*RS_scfstb)

                end if vbBoCorr

        
            case (BOCORRELATION_STANDING) calcBoByCorr
                ! --- CALFVF, JCODE=1 (Standing)

                    ! Proceder de acordo com a pressão de bolha:
                stBoCorr: if (P_psia >= PB_psia) then

                        ! Referência 1, pág 105, Equação B-15
                    BOB = 0.972d0 + 1.47d-4 * ( (R_scfstb * sqrt(Deng/SGO) + 1.25d0*T_F) ** 1.175d0 )

                        ! Referência 1, pág 106, Equação B-19:
                    CO  = (-1433.0d0 + 5.0d0*R_scfstb + 17.2d0*T_F - 1180.0d0*SG100 + 12.61d0*API) / (P_psia*1.0d5)
                    
                        ! Referência 1, pág 105, Equação B-16:
                    BoResult = BOB * exp(CO * (PB_psia - P_psia))

                else stBoCorr

                        ! Referência 1, pág 105, Equação B-15:
                    A = sqrt(Deng/SGO)
                    B = RS_scfstb*A + 1.25d0*T_F
                    BoResult = 0.972d0 + 1.47d-4 * (B ** 1.175d0)

                    ! TODO: Equação B-15 mesmo? Coeficientes um pouco diferentes nesta última
                    !   linha, e na primeira do bloco anterior. 
                    !   MARLIM 2 usa assim? Algum livro confirma?

                    ! UPDATE: em primeiro check, coeficientes conferem com MARLIM 2.
                    ! Coeficientes também conferem com os da rotina CALFVF do livro de Beggs e Brill!
                    ! Esse livro vai precisar figurar nas referências desta rotina como Referência 2!

                end if stBoCorr

                
            case (BOCORRELATION_GLASO) calcBoByCorr
                ! --- CALFVF, JCODE=2 (Glaso)

                    ! Proceder de acordo com a pressão de bolha:
                glasoBoCorr: if (P_psia >= PB_psia) then

                        ! Referência 1, pág 106, Equação B-21c: 
                    A = R_scfstb * ( (Deng/SGO) ** 0.526d0 ) + 0.968d0*T_F

                        ! Referência 1, pág 106, Equação B-21b:
                    B = log10(A)
                    C = -6.58511d0 + 2.91329d0*B - 0.27683d0*(B*B)

                        ! Referência 1, pág 106, Equação B-21a:
                    BOB = (10.0d0 ** C) + 1.0d0

                        ! Referência 1, pág 106, Equação B-19:
                    CO  = (-1433.0d0 + 5.0d0*R_scfstb + 17.2d0*T_F - 1180.0d0*SG100 + 12.61d0*API) / (P_psia*1.0d5)
                    
                        ! Referência 1, pág 105, Equação B-16:
                    BoResult = BOB * exp(CO * (PB_psia - P_psia))

                else glasoBoCorr

                        ! Referência 1, pág 106, Equação B-21c:
                    A = (Deng/SGO) ** 0.526d0
                    A = RS_scfstb*A + 0.968d0*T_F

                        ! Referência 1, pág 106, Equação B-21b:
                    B = log10(A)
                    C = -6.58511d0 + 2.91329d0*B - 0.27683d0*(B*B)

                        ! Referência 1, pág 106, Equação B-21a:
                    BoResult = (10.0d0 ** C) + 1.0d0

                end if glasoBoCorr


            case (BOCORRELATION_BACIACAMPOS) calcBoByCorr
                ! "Bacia de Campos e Espírito Santo"
                ! (conforme "procedure FVFO" no arquivo "PVT.pas" do código original)

                F1  = (Deng/SGO) ** 0.76d0
                BOB = 1.0d0 + 1.328865d-4 * ( (F1*R_scfstb + 1.35d0*T_F) ** 1.170318d0 )
                RHOS = (SGO + 2.178d-4 * Deng * R_scfstb) / BOB
                CO = (13.81083d0 - 1032.687d0*log10(RHOS)) / 14.22d6

                    ! Proceder de acordo com a pressão de bolha:
                bCESPb: if (P_psia.ge.PB_psia) then

                    BoResult = BOB * (10.0d0 ** (CO*(PB_psia - P_psia)/2.3025851d0))

                else bCESPb

                    PD = (P_psia - ATM_TOPSI) / (PB_psia - ATM_TOPSI)

                    if (PD <= 0.05d0) PD = 0.05d0

                    bCESPd: if (PD <= 0.32d0) then
                        FBO = 0.8587983d0 * (PD ** 0.2493832d0)
                    else bCESPd
                        FBO = 0.5195096d0 * PD + 0.4806904d0
                    end if bCESPd

                    BoResult = FBO*(BOB - 1.0d0) + 1.0d0

                end if bCESPb


            case default calcBoByCorr

                ! Correlação inválida para "Bo"!
                BoResult = -1.0d0
                iIER = ERROR_InvalidBoCorrelation
                return

        end select calcBoByCorr

        ! Proteção do Pascal: BO >= 1.0
        if (BoResult < 1.0d0) BoResult = 1.0d0

        ! PRIMEIRA CONFERÊNCIA ATÉ AQUI CONCLUÍDA EM 14-AGO-2026!

        ! Bo é adimensional: bbl/stb == m3/m3 (mesma razão de volumes)
        ! Então, não há conversão numérica adicional aqui.

    end subroutine CalculateFVFOfOilByCorrelations


    ! =============================================================
    !           ROTINAS PARA CÁLCULOS DE Rs
    ! =============================================================
    subroutine CalculateBlackOilSolutionGasOilRatio(corrSat, pres, temp, varPb, dPSEP_VB, dTSEP_VB, API, RGO, Deng, pbtemp, RsResult, iIER)

        ! OBJETIVO: Calcular "Rs" usando uma das correlações disponíveis.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer, intent(in) :: corrSat              ! Índice da correlação escolhida para o cálculo de "Rs".
        real(c_double), intent(in) :: pres          ! Pressão (em kgf/cm2).
        real(c_double), intent(in) :: temp          ! Temperatura (em graus Celsius).
        real(c_double), intent(in) :: varPb         ! Pressão de bolha em psia (negativa para cálculo automático).
        real(c_double), intent(in) :: dPSEP_VB      ! Vazquez e Beggs, pág 104 da Referência 1: "actual separator pressure", em kgf/cm2 (MARLIM 2 usa "standard")
        real(c_double), intent(in) :: dTSEP_VB      ! Vazquez e Beggs, pág 104 da Referência 1: "actual separator temperature", em graus Celsius (MARLIM 2 usa "standard")
        real(c_double), intent(in) :: API           ! Grau API do óleo
        real(c_double), intent(in) :: RGO           ! RGO (Sm3/Sm3)
        real(c_double), intent(in) :: Deng          ! Referência 1, pág 104: gammaG = "specific gravity of gas (air = 1.0)"

        real(c_double), intent(out) :: pbtemp       ! Pressão de bolha usada no cálculo de Rs (fornecida ou determinada automaticamente), em psia.
        real(c_double), intent(out) :: RsResult     ! Resultado do cálculo de "Rs" (unidades: scft/bbl)
        integer(c_int), intent(out) :: iIER         ! Código de erros, conforme convencionado.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: ipres, itemp, rstemp, tFahrenheit
        real(c_double) :: GMF1, EMWGT, SGO, A, B, BPPF1
        real(c_double) :: Avb, Bvb, Cvb, IRGO
        real(c_double) :: pSep_psia, tSep_Fahrenheit
        real(c_double) :: SG100vb                   ! Referência 1, pág 104: gas specific gravity [at] separator pressure of 100 psig

        ! ------------------ CÁLCULOS:

        ! Conversão de unidades:
        ipres = pres * 14.2233426               ! Conversão de kgf/cm2 para psia.
        itemp = temp * 1.8 + 491.67             ! Conversão de °C para Rankine.
        IRGO = RGO * 35.31467 / 6.29            ! Conversão de Sm3/Sm3 para scft/bbl
        IRGO = RGO * 5.614583                   ! (update: "mesma" conversão acima, fator ligeiramente diferente)

        tFahrenheit = (1.8d0 * temp) + 32.0d0   ! Conversão de °C para Fahrenheit.

        pSep_psia = dPSEP_VB * 14.2233426
        tSep_Fahrenheit = (1.8d0 * dTSEP_VB) + 32.0d0

        ! Calcular "Rs" de acordo com a correlação selecionada:
        whichRsCorr: select case(corrSat)

            case (RSCORRELATION_VAZQUEZBEGGS) whichRsCorr

                ! Correlação de Vázquez & Beggs

                    ! Equação B-9 da pág 104 da Referência 1:
                SG100vb = 5.912d-5 * API * tSep_Fahrenheit * log10(pSep_psia / 114.7d0)
                SG100vb = Deng * (1.0d0 + SG100vb)
                if(SG100vb.lt.0.5538d0) SG100vb = 0.5538d0  ! Metano (de acordo com o código-fonte do MARLIM 2)

                    ! Constantes da "Table B-2", pág 104 da Referência 1:
                ctesVB: if(API > 30) then
                    Avb = 1.187       ! "C2" da Referência 1, consistente com o equacionamento abaixo e da referência
                    Bvb = 56.06       ! + ou - 1/"C1" da Referência 1, consistente com o equacionamento abaixo e da referência
                    Cvb = 10.393      ! log10(e) * "C3" da Referência 1, consistente com o equacionamento abaixo e da referência
                else ctesVB
                    Avb = 1.0937      ! "C2" da Referência 1, consistente com o equacionamento abaixo e da referência
                    Bvb = 27.64       ! + ou - 1/"C1" da Referência 1, consistente com o equacionamento abaixo e da referência
                    Cvb = 11.172      ! log10(e) * "C3" da Referência 1, consistente com o equacionamento abaixo e da referência
                end if ctesVB

                    ! Equação B-8, pág 104 da Referência 1
                    ! (equação original modificada de forma consistente com as constantes acima)
                rstemp = (SG100vb * (ipres**Avb) / Bvb) * (10.0**(Cvb * API / itemp))

                if (varPb < 0) then
                    ! Equação de "rstemp" acima, resolvida para a pressão, com a RGO no lugar da Rs:
                    ! "pbtemp" saindo em psia
                    pbtemp = (Bvb * IRGO / (SG100vb * (10.0**(Cvb * API / itemp))))**(1.0 / Avb)
                else
                    pbtemp = varPb
                end if

                limitsVB: if(rstemp.lt.(0.0d0)) then
                    rstemp = 0.0d0
                else if (ipres >= pbtemp) then limitsVB
                    rstemp = IRGO
                end if limitsVB


            case (RSCORRELATION_LASATER) whichRsCorr

                ! Lasater

                    ! API para densidade d60 do óleo:
                SGO = 141.5d0 / (131.5d0 + API)

                    ! Determinação do "peso molecular efetivo do óleo".
                    ! (ver Referência 1, pág 104, para mais detalhes sobre o "effective molecular weight of tank oil")
                ref1Mo: if (API < 51.1d0) then
                    EMWGT = InterpolaLas3(API)
                else ref1Mo
                    ! Extrapolação, de acordo com o código fonte do MARLIM 2 (procedure CALRS no arquivo PVT.pas)
                    EMWGT = 6084.0d0 / (API - 5.9d0)
                end if ref1Mo

                    ! Parâmetro de correlação "mole fraction of gas":
                    ! (mais detalhes na página 104 da Referência 1)
                GMF1 = 1.0d0 / (1.0d0 + 132755.0d0 * SGO / IRGO / EMWGT)

                ! OBS: A equação acima corresponde à própria correlação de Lasater para "Rs", resolvida para
                !   a "mole fraction of gas" com a RGO no lulgar do "Rs" (ver equação B-7 na página 104 da Referência 1).

                    ! Obter o "bubblepoint pressure factor":
                BPPF1 = InterpolaLas1(GMF1)

                    ! "Pressão de bolha" (Referência 1, pág 107, Fig B-7, equação no eixo "y"):
                pbtemp = BPPF1 * itemp / Deng

                    ! Recalcular...
                BPPF1 = ipres * Deng / itemp

                    ! Partindo para o cálculo de "Rs":
                rsLas: if (ipres >= pbtemp) then
                    rstemp = IRGO
                else rsLas

                    ! Código fonte do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "Extrapolação para BPPF1 menor do que 0.17 ou maior do
                    !   que 6.7 feita com base em equação polinomial de ajuste. No futuro implantar essa equação."

                    limLas1: if(BPPF1.gt.0.17d0) then

                        limLas2: if(BPPF1.le.6.7d0) then
                            ! Código fonte do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "BPPF1 entre 0.17 e 6.7 (região da curva de Lasater)"
                            GMF1 = InterpolaLas2(BPPF1)
                        else limLas2
                            ! Código fonte do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "Extrapola exponencialmente (GMF1 sempre menor do que 1)"
                            GMF1 = 1.0d0 - 0.99308d0 * exp(-0.28075d0 * BPPF1)
                            if(GMF1.eq.1.0d0) GMF1 = 0.999d0
                        end if limLas2

                    else limLas1

                        ! Código fonte do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "Extrapola linearmente"
                        GMF1 = (BPPF1-0.06263d0) / 2.1474d0
                        if(GMF1.lt.0.0) GMF1 = 0.0d0

                    end if limLas1

                        ! Equação B-7 da pág 104 da Referência 1:
                    rstemp= (132755.0d0 * SGO / EMWGT) * (GMF1 / (1.0d0 - GMF1))
                    if(rstemp.lt.0.0) rstemp = 0.0d0

                end if rsLas


            case (RSCORRELATION_STANDING) whichRsCorr

                ! Standing

                    ! "A" = Expoente na equação B-6 na pág 104 da Referência 1:
                A = 0.0125d0 * API - 0.00091d0 * tFahrenheit
                B = 10.0d0 ** A

                    ! Pressão de bolha:
                bblP_Standing: if (varPb < 0.0d0) then
                    ! Cálculo automático: equação B-17 na pág 105 da Referência 1:
                    pbtemp = 18.2d0 * ( &
                                        (  (1.0d0 / B) * (IRGO / Deng) ** 0.83d0   ) - 1.4d0 &
                                      )
                else bblP_Standing
                    ! Usar valor fornecido:
                    pbtemp = varPb
                end if bblP_Standing

                    ! Cálculo de "Rs":
                rs_Standing: if (ipres > pbtemp) then
                    rstemp = IRGO
                else rs_Standing
                    ! Equação B-6 da pág 104 da Referência 1 (obs: 1/0.83 = 1.2048):
                    rstemp = Deng * ( &
                                        ((1.4d0 + ipres / 18.2d0) * B) ** (1.0d0 / 0.83d0) &
                                    )
                end if rs_Standing

                if(rstemp.lt.0.0d0) rstemp = 0.0d0


            case (RSCORRELATION_GLASO) whichRsCorr

                ! Glaso

                    ! Pressão de bolha:
                bblP_Glaso: if (varPb < 0.0d0) then

                    ! Cálculo automático: Fazer Rs = RGO e resolver para a pressão a equação B-10 da pág 104 da Referência 1
                    A = ((IRGO / Deng) ** 0.816d0) * (tFahrenheit ** 0.172d0) * (API ** (-0.989d0))             ! (OBS: 1 / 1.2255 = 0.816)
                    B = log10(A)            ! OBS: A = pb asterisco, e B portanto é o expoente em sua equação (ver pág 104 na Referência 1)

                    ! NOTA: A equação abaixo, obtida do MARLIM 2, pode ser obtida da Referência 1 apenas com manipulação algébrica e
                    !       dos valores numéricos, sem qualquer modificação. Após rearranjar a equação do expoente (Referência 1) para explicitar o
                    !       logaritmo da pressão como função de "B", basta multiplicar numerador e denominador por 0.60436 ^ 2 = 0.365251
                    !       para chegar na expressão do MARLIM 2.

                    pbtemp = 10.0d0 ** ((5.17967d0 - (0.60436d0 * B - 1.7447d0) ** 2.0d0) / 1.20872d0)

                else bblP_Glaso
                    ! Usar valor fornecido:
                    pbtemp = varPb
                end if bblP_Glaso

                    ! Referência 1, pág 105: "Analysis of the expression for PB* shows that the correlation cannot be used for pressures in excess of 19285 psi".
                    ! No código do MARLIM 2, o valor adotado é 19284 psia.
                if(pbtemp.gt.(19284.0d0)) pbtemp = 19284.0d0

                    ! Cálculo de "Rs":
                rs_Glaso: if (ipres > pbtemp) then
                    rstemp = IRGO
                else rs_Glaso

                        ! Cálculo de "B" = expoente da equação de PB Asterisco (pág 104 na Referência 1).
                    A = 5.17967d0 - 1.20872d0 * log10(ipres)    ! Expressão entre colchetes na Referência 1, multiplicada por 0.365251 = 0.60436 ^ 2
                    if (A < 0) A = 0.0d0
                    A = sqrt(A)                                 ! Raíz da expressão entre colchetes na Referência 1, multiplicada por sqrt(0.365251) = 0.60436
                    B = (1.7447d0 - A) / 0.60436d0              ! Expressão do expoente apresentada na pág 104 da Referência 1.

                        ! Equação B-10, pág 104 da Referência 1:
                    rstemp = Deng * ((API ** 0.989d0) * (10.0d0 ** B) * tFahrenheit ** (-0.172d0)) ** 1.22549d0

                    if(rstemp.lt.0) rstemp = 0.0d0

                end if rs_Glaso

            case default whichRsCorr

                ! Correlação de "Rs" inválida!
                RsResult = -1.0d0                       ! Atribuir valor inválido.
                iIER = ERROR_InvalidRsCorrelation
                return

        end select whichRsCorr

        ! Incluindo verificação do código original (arquivo "PVT.pas", procedure "CALRS"):
        if(rstemp.gt.IRGO) rstemp = IRGO

        ! Retornar resultado:
        RsResult = rstemp

                ! PAREI AQUI EM 24-JUL-2025
                ! Rotina está concluída! Pode seguir!!

    end subroutine CalculateBlackOilSolutionGasOilRatio

    ! =============================================================
    ! =============================================================
    function InterpolaLas3(var) result(interpolatedValue)

        ! OBJETIVO: Reproduzir a interpolação de Mo ("effective molecular weight of tank oil") a partir da figura B-6
        !       da pág 107 da Referência Bibliográfica 1 detalhada abaixo.

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), intent(in) :: var                               ! Valor de grau API para o qual se deseja a interpolação.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: comp, pos
        logical :: para
        real(c_double) :: raz
        real(c_double) :: APIEMW(10), EMW(10)
        real(c_double) :: interpolatedValue

        ! ------------------ "CONSTANTES":

            ! Código do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "effective oil molecular weight vs oil API gravity data"
        APIEMW = (/ 5.0d0, 13.0d0, 19.0d0, 24.0d0, 28.0d0, 33.0d0, 38.0d0, 44.0d0, 52.0d0, 70.0d0 /)

            ! Código do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "data arrays for Lasater correlation"
        EMW = (/ 600.0d0, 500.0d0, 450.0d0, 400.0d0, 350.0d0, 300.0d0, 250.0d0, 200.0d0, 150.0d0, 100.0d0 /)

        ! ------------------ CÁLCULOS:

        ! Inicializações:
        comp = size(APIEMW)         ! Número de elementos no vetor APIEMW.
        para = .false.              ! Controle do loop.
        pos = 1                     ! Posição inicial no vetor.

        ! Loop para encontrar os intervalos de interpolação:
        findInterval: do while (.not. para)

            if (pos == comp) then
                para = .true. ! Última posição alcançada.
            else if (var >= APIEMW(pos) .and. var < APIEMW(pos + 1)) then
                para = .true. ! Intervalo encontrado.
            else
                pos = pos + 1 ! Avança para a próxima posição.
            end if

        end do findInterval

        ! Cálculo da interpolação:
        interp: if (pos == comp) then
            ! Caso o valor esteja fora do intervalo, retorna o último valor de EMW.
            interpolatedValue = EMW(comp)
        else interp
            ! Realiza a interpolação linear.
            raz = (var - APIEMW(pos)) / (APIEMW(pos + 1) - APIEMW(pos))
            interpolatedValue = raz * EMW(pos + 1) + (1.0d0 - raz) * EMW(pos)
        end if interp

    end function InterpolaLas3

    ! =============================================================
    ! =============================================================
    function InterpolaLas2(var) result(interpolatedValue)

        ! OBJETIVO: Automatizar a leitura "invertida" da figura B-7 da pág 107 da Referência 1 detalhada abaixo, possibilitando
        !       obter o "gas mole fraction" a partir de dado "bubblepoint pressure factor".

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), intent(in) :: var                               ! Valor de "bubblepoint pressure factor" para o qual se deseja a interpolação.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: comp, pos
        logical :: para
        real(c_double) :: raz
        real(c_double) :: GMF(17), BPPF(17)
        real(c_double) :: interpolatedValue

        ! ------------------ "CONSTANTES":

            ! Código do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "bubble point pressure factor vs gas mole fraction data"
        GMF = (/ 0.05d0, 0.10d0, 0.15d0, 0.20d0, 0.25d0, 0.30d0, 0.35d0, 0.40d0, 0.45d0, 0.50d0, 0.55d0, 0.60d0, &
          0.65d0, 0.70d0, 0.75d0, 0.80d0, 0.85d0 /)

        BPPF = (/ 0.17d0, 0.30d0, 0.43d0, 0.58d0, 0.75d0, 0.94d0, 1.19d0, 1.47d0, 1.74d0, 2.10d0, 2.70d0, 3.29d0, &
            3.80d0, 4.30d0, 4.90d0, 5.70d0, 6.70d0 /)

        ! Inicialização das variáveis:
        comp = size(BPPF) ! Número de elementos no vetor BPPF.
        para = .false.   ! Controle do loop.
        pos = 1          ! Posição inicial no vetor.

        ! Loop para encontrar os intervalos de interpolação:
        findInterval: do while (.not. para)

            if (pos == comp) then
                para = .true. ! Última posição alcançada.
            else if (var >= BPPF(pos) .and. var < BPPF(pos + 1)) then
                para = .true. ! Intervalo encontrado.
            else
                pos = pos + 1 ! Avança para a próxima posição.
            end if

        end do findInterval

        ! Cálculo da interpolação:
        if (pos == comp) then
            ! Caso o valor esteja fora do intervalo, retorna o último valor de GMF.
            interpolatedValue = GMF(comp)
        else
            ! Realiza a interpolação linear.
            raz = (var - BPPF(pos)) / (BPPF(pos + 1) - BPPF(pos))
            interpolatedValue = raz * GMF(pos + 1) + (1.0d0 - raz) * GMF(pos)
        end if

    end function InterpolaLas2

    ! =============================================================
    ! =============================================================
    function InterpolaLas1(var) result(interpolatedValue)

        ! OBJETIVO: Automatizar a leitura da figura B-7 da pág 107 da Referência 1 detalhada abaixo, possibilitando
        !       obter o "bubblepoint pressure factor" a partir de dado "gas mole fraction".

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), intent(in) :: var                               ! Valor de "gas mole fraction" para o qual se deseja a interpolação.

        ! ------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer :: comp, pos
        logical :: para
        real(c_double) :: raz
        real(c_double) :: GMF(17), BPPF(17)
        real(c_double) :: interpolatedValue

        ! ------------------ "CONSTANTES":

            ! Código do MARLIM 2 (procedure CALRS no arquivo PVT.pas): "bubble point pressure factor vs gas mole fraction data"
        GMF = (/ 0.05d0, 0.10d0, 0.15d0, 0.20d0, 0.25d0, 0.30d0, 0.35d0, 0.40d0, 0.45d0, 0.50d0, 0.55d0, 0.60d0, &
          0.65d0, 0.70d0, 0.75d0, 0.80d0, 0.85d0 /)

        BPPF = (/ 0.17d0, 0.30d0, 0.43d0, 0.58d0, 0.75d0, 0.94d0, 1.19d0, 1.47d0, 1.74d0, 2.10d0, 2.70d0, 3.29d0, &
            3.80d0, 4.30d0, 4.90d0, 5.70d0, 6.70d0 /)

        ! Inicialização das variáveis:
        comp = size(GMF) ! Número de elementos no vetor GMF.
        para = .false.   ! Controle do loop.
        pos = 1          ! Posição inicial no vetor.

        ! Loop para encontrar os intervalos de interpolação:
        findInterval: do while (.not. para)

            if (pos == comp) then
                para = .true. ! Última posição alcançada.
            else if (var >= GMF(pos) .and. var < GMF(pos + 1)) then
                para = .true. ! Intervalo encontrado.
            else
                pos = pos + 1 ! Avança para a próxima posição.
            end if

        end do findInterval

        ! Cálculo da interpolação:
        if (pos == comp) then
            ! Caso o valor esteja fora do intervalo, retorna o último valor de BPPF.
            interpolatedValue = BPPF(comp)
        else
            ! Realiza a interpolação linear.
            raz = (var - GMF(pos)) / (GMF(pos + 1) - GMF(pos))
            interpolatedValue = raz * BPPF(pos + 1) + (1.0d0 - raz) * BPPF(pos)
        end if

    end function InterpolaLas1

    ! =============================================================
    !  ROTINAS PARA CORREÇÃO VIA REGRESSÃO LINEAR CONTRA ANÁLISE PVT
    ! =============================================================

    subroutine FitBlackOilPVTAnalysisCalibrationModels(iNPVTPoints, oExpPressures_Arg, oExpRs_Arg, dExpTemperature, dPSEP_VB, dTSEP_VB, dAPI, &
        dRGO_Arg, dDeng, oTestedRsCorrelations, oCorrelationsRs, iBestRsCorrelation, iRsRegressionModelType, dRsRegressionA, dRsRegressionB, &
        oCorrectedRs, oCalibrationInfoSummary, iIER)

        ! OBJETIVO: Executa o procedimento completo de ajuste (regressão) de modelos de calibração para propriedades black-oil 
        !           a partir de dados experimentais de Análise PVT, retornando os parâmetros desses modelos ajustados, junto com
        !           resultados intermediários importantes.
        !           (Fluxo inspirado na rotina PVT2A do código Pascal original, do MARLIM 2 - arquivo fonte MRL_pvt.pas.)

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none

        !------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iNPVTPoints                               ! Número de pontos experimentais da Análise PVT
        real(c_double), dimension(iNPVTPoints), intent(in) :: oExpPressures_Arg ! Pressões da Análise PVT (kgf/cm2).
        real(c_double), dimension(iNPVTPoints), intent(in) :: oExpRs_Arg        ! Valores de Rs medidos na Análise PVT (m3/m3).
        real(c_double), intent(in) :: dExpTemperature                           ! Temperatura da Análise PVT (°C)
        real(c_double), intent(in) :: dPSEP_VB                ! Vazquez e Beggs, pág 104 da Referência 1: "actual separator pressure", em kgf/cm2 (MARLIM 2 usa "standard")
        real(c_double), intent(in) :: dTSEP_VB                ! Vazquez e Beggs, pág 104 da Referência 1: "actual separator temperature", em graus Celsius (MARLIM 2 usa "standard")             
        real(c_double), intent(in) :: dAPI                    ! Grau API do óleo
        real(c_double), intent(in) :: dRGO_Arg                ! RGO (Sm3/Sm3) - fornecer valor negativo para determinação automática
        real(c_double), intent(in) :: dDeng                   ! Referência 1, pág 104: gammaG = "specific gravity of gas (air = 1.0)"

        integer(c_int), dimension(RSCORRELATION_COUNT), intent(out) :: oTestedRsCorrelations ! Vetor de índices convencionados de correlações de Rs testadas (desconsiderar elementos negativos).
        real(c_double), dimension(RSCORRELATION_COUNT+1, iNPVTPoints), intent(out) :: oCorrelationsRs ! Elemento [i, j]: Rs em m3/m3, calculado pela correlação "i" do vetor
                                                                                                      ! "oTestedRsCorrelations",  nas condições do ponto experimental "j"
                                                                                                      ! (valores experimentais na última linha da matriz).
        integer(c_int), intent(out) :: iBestRsCorrelation          ! Índice (confome convenção da biblioteca) da correlação de Rs que mais se aproxima dos resultados da Análise PVT (sem correção)
        integer(c_int), intent(out) :: iRsRegressionModelType      ! Tipo do modelo de calibração ajustado para Rs (1=linear, 0=potencial)
        real(c_double), intent(out) :: dRsRegressionA              ! Coeficiente angular do modelo de calibração ajustado para Rs.
        real(c_double), intent(out) :: dRsRegressionB              ! Coeficiente linear do modelo de calibração ajustado para Rs.
        real(c_double), dimension(iNPVTPoints), intent(out) :: oCorrectedRs ! Rs calibrado (resultante do modelo ajustado) em cada ponto da Análise PVT, em m3/m3.
        real(c_double), dimension(4, iNPVTPoints), intent(out) :: oCalibrationInfoSummary   ! Matriz apenas para facilitar primeiras verificações e gráficos
                                                                                            ! (mais detalhes nos comentários descritivos no corpo da subrotina)
        integer(c_int), intent(out) :: iIER                        ! Código de erros, conforme convenção da biblioteca.

        !------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double), dimension(iNPVTPoints) :: oExpPressures ! Pressões da Análise PVT (kgf/cm2), em ordem DECRESCENTE!
        real(c_double), dimension(iNPVTPoints) :: oExpRs        ! Valores de Rs medidos na Análise PVT (m3/m3) em ordem correspondente a "oExpPressures".
        real(c_double) :: dRGO                                  ! RGO (Sm3/Sm3) fornecido ou determinado automaticamente
        integer(c_int) :: iPb                                   ! Índice do ponto de pressão de bolha nos dados da Análise PVT
        real(c_double) :: dPSat                                 ! Pressão de saturação (kgf/cm2)
        integer(c_int) :: i, j, iX
        logical :: bPVTIsDescending
        real(c_double) :: dTempP, dTempRs

            ! Variáveis locais para chamadas intermediárias:

            ! Para regressão:
        real(c_double), dimension(iNPVTPoints) :: dX, dY
        real(c_double) :: dC, dR2, dC_pot, dR2_pot
    
            ! Para chamada de rotina de insumos adicionais:
        integer(c_int), dimension(RSCORRELATION_COUNT) :: oTestedRsCorrLoc
        real(c_double), dimension(RSCORRELATION_COUNT+1, iNPVTPoints) :: oCorrelationsRsLoc
        integer(c_int) :: iBestRsCorrLoc, iPbLoc
        real(c_double) :: dPSatLoc

        !------------------ CÁLCULOS:

        ! 1. Garantir ordem decrescente das pressões (e Rs correspondente)
        oExpPressures = oExpPressures_Arg
        oExpRs = oExpRs_Arg

        bPVTIsDescending = .true.

        doChkDescending: do i = 2, iNPVTPoints

            chkDescending: if (oExpPressures(i) > oExpPressures(i-1)) then
                bPVTIsDescending = .false.
                exit doChkDescending
            end if chkDescending

        end do doChkDescending

        ! Garantir que os pontos da Análise PVT estejam ordenados no sentido decrescente das pressões.
        hasToSortPVT: if (.not.bPVTIsDescending) then

            ! Selection Sort decrescente, custo computacional irrelevante para vetores pequenos
            outerPVTSort: do i = 1, iNPVTPoints-1

                innerPVTSort: do j = i+1, iNPVTPoints

                    chkOutOfOrder: if (oExpPressures(j) > oExpPressures(i)) then

                        dTempP = oExpPressures(i)
                        oExpPressures(i) = oExpPressures(j)
                        oExpPressures(j) = dTempP

                        dTempRs = oExpRs(i)
                        oExpRs(i) = oExpRs(j)
                        oExpRs(j) = dTempRs

                    end if chkOutOfOrder

                end do innerPVTSort

            end do outerPVTSort

        end if hasToSortPVT

        ! 2. Determinação automática de RGO, se necessário
        dRGO = dRGO_Arg

        findRGO: if (dRGO.LT.(0.0d0)) then

            ! Determinação automática da RGO (a partir da Análise PVT).
            ! Obter do último ponto antes de Rs experimental diminuir.
            loopRs1: do i = 2, iNPVTPoints

                rsDiminishes: if (oExpRs(i) < oExpRs(i-1)) then

                    hasMinPoints: if (i > 2) then

                        dRGO = oExpRs(i-1)
                        exit loopRs1

                    else hasMinPoints

                        ! Não aceitar RGO a partir da Análise PVT caso esta não contenha pelo menos
                        !   2 medidas acima da pressão de saturação.
                        iIER = ERROR_CouldNotDetermineGORFromPVTAnalysis
                        return

                    end if hasMinPoints

                end if rsDiminishes

            end do loopRs1

            foundRGO: if (dRGO.LT.(0.0d0)) then
                iIER = ERROR_CouldNotDetermineGORFromPVTAnalysis
                return
            end if foundRGO

        end if findRGO

        ! 3. Chamada à rotina de cálculo de dados preliminares para subsidiar a regressão
        !    dos modelos de calibração:

        call CalculateBlackOilInputForPVTAnalysisRegression(iNPVTPoints, oExpPressures, oExpRs, &
            dExpTemperature, dPSEP_VB, dTSEP_VB, dAPI, dRGO, dDeng, &
            oTestedRsCorrLoc, oCorrelationsRsLoc, iPbLoc, dPSatLoc, iBestRsCorrLoc, iIER)

        if (iIER.NE.ERROR_EverythingOK) return

            ! Repasse dos resultados de interesse para os argumentos de saída
        oTestedRsCorrelations = oTestedRsCorrLoc
        oCorrelationsRs = oCorrelationsRsLoc
        iBestRsCorrelation = iBestRsCorrLoc
        iPb = iPbLoc
        dPSat = dPSatLoc

        ! 4. Preparar os vetores para regressão do modelo de calibração de Rs:
        
            ! Encontrar o índice da linha de oCorrelationsRs correspondente à melhor correlação para Rs:
            ! ("melhor correlação" = a que mais se aproxima da Análise PVT, sem qualquer calibração)
        iX = -1
        doBestRsCorrIndex: do i = 1, RSCORRELATION_COUNT

            chkBestRsCorrIndex: if (oTestedRsCorrelations(i).EQ.iBestRsCorrelation) then
                iX = i
                exit doBestRsCorrIndex
            end if chkBestRsCorrIndex

        end do doBestRsCorrIndex

            ! Unidades de m3/m3 na regressão do modelo de calibração de Rs, abaixo:
        dX = oCorrelationsRs(iX, :)     ! Todos os elementos da linha iX
        dY = oExpRs

        ! 5. Chamada à rotina de regressão para Rs
        call CalculateBlackOilPVTAnalysisCorrectionModelParameters(iPb, iNPVTPoints, dX, dY, &
            iRsRegressionModelType, dRsRegressionA, dRsRegressionB, dC, dR2, dC_pot, dR2_pot)

        ! 6. Calcular Rs calibrado em cada ponto experimental:
        doCalcCalibratedRs: do i = 1, iNPVTPoints
            call CorrectBlackOilRsWithPVTAnalysisRegression( &
                iRsRegressionModelType, dRsRegressionA, dRsRegressionB, dX(i), dPSat, &
                dRGO, oExpPressures(i), oCorrectedRs(i))
        end do doCalcCalibratedRs

        ! FINAL PARA Rs: "Matriz resumo", para facilitar primeiras verificações ou gráficos:
        oCalibrationInfoSummary(1, :) = oExpPressures       ! Pressões em ordem decrescente, em kgf/cm2
        oCalibrationInfoSummary(2, :) = oExpRs              ! Rs experimentais usados na regressão, em m3/m3
        oCalibrationInfoSummary(3, :) = oCorrelationsRs(iX, :)  ! Rs calculados usados na regressão, em m3/m3
        oCalibrationInfoSummary(4, :) = oCorrectedRs        ! Valores calibrados de Rs, em m3/m3

        ! ROTINA CONFERIDA ATÉ O FINAL EM 16/04/2026. CONCLUÍDA! PROSSEGUIR PARA A PRÓXIMA!

        ! ================ ESCRITA OPCIONAL NA TELA =============================
        wantsToWriteOnScreen: if(.false.) then

            ! Escrever a "oCalibrationInfoSummary":
            write(*, '(/, A)') '----> oCalibrationInfoSummary:'
            write(*, '("      ", A)') 'Linha 1: Pressões da Análise PVT (kgf/cm2)'
            write(*, '("      ", A)') 'Linha 2: Rs experimentais da Análise PVT (m3/m3)'
            write(*, '("      ", A)') 'Linha 3: Rs calculados, usados na regressão (m3/m3)'
            write(*, '("      ", A)') 'Linha 4: Rs calibrados (m3/m3)'

            writeCalibrationInfoSummaryToScreen: do i = 1, 4

                write(*, '(/, /"Linha ", I3, ":", /)') i

                writeInfoSummaryColumnsToScreen: do j = 1, iNPVTPoints
                    write(*, '(F15.4, "      ")', advance='no') oCalibrationInfoSummary(i, j)
                end do writeInfoSummaryColumnsToScreen
            end do  writeCalibrationInfoSummaryToScreen

            ! Escrever a "oCorrelationsRs":
            write(*, '(/, A)') '----> oCorrelationsRs (Valores em m3/m3):'
            write(*, '("      Linha ", A)') 'Linha 1: Vasquez and Beggs'
            write(*, '("      Linha ", A)') 'Linha 2: Lasater'
            write(*, '("      Linha ", A)') 'Linha 3: Standing'
            write(*, '("      Linha ", A)') 'Linha 4: Glaso'
            write(*, '("      Linha ", A)') 'Linha 5: Análise PVT'

            writeCorrRsLinesToScreen: do i = 1, (RSCORRELATION_COUNT + 1)

                write(*, '(/, /"Linha ", I3, ":", /)') i

                writeCorrRsColsToScreen: do j = 1, iNPVTPoints
                    write(*, '(F15.4, "      ")', advance='no') oCorrelationsRs(i, j)          
                end do writeCorrRsColsToScreen

            end do writeCorrRsLinesToScreen

        end if wantsToWriteOnScreen
        ! ================ FIM DA ESCRITA OPCIONAL NA TELA ======================

    end subroutine FitBlackOilPVTAnalysisCalibrationModels

    ! =============================================================
    ! =============================================================
    subroutine CalculateBlackOilInputForPVTAnalysisRegression(nPVTPoints, oExpPressures, oExpRs, dExpTemperature, &
        dPSEP_VB, dTSEP_VB, dAPI, dRGO, dDeng, oTestedRsCorrelations, oCorrelationsRs, iPb, dPSat, iBestRsCorrelation, iIER)
  
        ! OBJETIVO: Preparar os dados de entrada necessários para a regressão de correlações Black-Oil contra Análise PVT:
        !           --> determina a pressão de saturação, além de localizá-la nos dados experimentais;
        !           --> calcula os valores de Rs por todas as correlações disponíveis para cada ponto da Análise PVT,
        !               além de identificar a correlação que mais se aproxima destes dados (sem correção).

        ! REFERÊNCIA BIBLIOGRÁFICA 1: "Multiphase Flow in Wells", James P. Brill e Hemanta Mukherjee, 1999
        implicit none
  
        !------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: nPVTPoints              ! Número de pontos experimentais da Análise PVT       
        real(c_double), dimension(nPVTPoints), intent(in) :: oExpPressures        ! Pressões da Análise PVT (kgf/cm2), em ordem DECRESCENTE!
        real(c_double), dimension(nPVTPoints), intent(in) :: oExpRs               ! Valores de Rs medidos na Análise PVT (m3/m3)
        real(c_double), intent(in) :: dExpTemperature         ! Temperatura da Análise PVT (°C)
        real(c_double), intent(in) :: dPSEP_VB                ! Vazquez e Beggs, pág 104 da Referência 1: "actual separator pressure", em kgf/cm2 (MARLIM 2 usa "standard")
        real(c_double), intent(in) :: dTSEP_VB                ! Vazquez e Beggs, pág 104 da Referência 1: "actual separator temperature", em graus Celsius (MARLIM 2 usa "standard")
        real(c_double), intent(in) :: dAPI                    ! Grau API do óleo
        real(c_double), intent(in) :: dRGO                    ! RGO (Sm3/Sm3)
        real(c_double), intent(in) :: dDeng                   ! Referência 1, pág 104: gammaG = "specific gravity of gas (air = 1.0)"
        
        integer(c_int), dimension(RSCORRELATION_COUNT), intent(out) :: oTestedRsCorrelations           ! Vetor de índices convencionados de correlações de Rs testadas (desconsiderar elementos negativos).
        real(c_double), dimension(RSCORRELATION_COUNT+1, nPVTPoints), intent(out) :: oCorrelationsRs   ! Elemento [i, j]: Rs em m3/m3, calculado pela correlação "i" do vetor
                                                                                                       ! "oTestedRsCorrelations",  nas condições do ponto experimental "j"
                                                                                                       ! (valores experimentais na última linha da matriz).
        integer(c_int), intent(out) :: iPb                    ! Índice do ponto de pressão de bolha nos dados da Análise PVT
        real(c_double), intent(out) :: dPSat                  ! Pressão de saturação em kgf/cm2 (fornecida, ou obtida a partir de oExpRs, ou obtida a partir da correlação para Rs)
        integer(c_int), intent(out) :: iBestRsCorrelation     ! Índice (confome convenção da biblioteca) da correlação de Rs que mais se aproxima dos resultados da Análise PVT (sem correção)
        integer(c_int), intent(out) :: iIER                   ! Código de erros, conforme convencionado.

        !------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer(c_int) :: i, iRsCorr, iBest, iFound
        real(c_double) :: dGivenPSat                            ! Pressão de saturação fornecida (em kgf/cm2)
                                                                ! NOTA: variável "dummy" por enquanto, criada apenas para facilitar eventual modificação da rotina visando
                                                                !       possibilidade de passar este valor via argumento de entrada (para fazer isso, basta meramente "promover"
                                                                !       esta variável local a argumento de entrada, e apagar sua inicialização no corpo da subrotina).
        real(c_double) :: dPbTemp, dRsResult        
        real(c_double), dimension(RSCORRELATION_COUNT) :: oRsNormL2
        real(c_double) :: dNorm, dNormMin, dDiff
        real(c_double), dimension(RSCORRELATION_COUNT) :: oRsCorrelationsPSat    ! Elemento "i" = pressão de bolha em kgf/cm2 determinada (somente se necessário) pela 
                                                                                 ! correlação "i" do vetor "oTestedRsCorrelations".

        ! ------------------ CÁLCULOS:

        ! INICIALIZAÇÕES:
        dGivenPSat = -10.0d0        ! Inicializando variável "dummy" (por enquanto) propositalmente com valor negativo.
        
        iPb = -1                    ! Inicializando índice com valor negativo (ainda não possui um valor válido).
        dPSat = -10.0d0             ! Inicializando com valor negativo (ainda não possui um valor válido).

        ! DETERMINAÇÃO DA PRESSÃO DE SATURAÇÃO E DE iPb:
        findPSat: if (dGivenPSat.LE.(0.0d0)) then

            ! Pressão de saturação não está disponível a priori.
            ! Buscar a pressão de bolha como o último ponto antes de Rs experimental diminuir.
            loopRs1: do i = 2, nPVTPoints

                rsDiminishes: if (oExpRs(i) < oExpRs(i-1)) then
                    dPSat = oExpPressures(i-1)          ! Confere com MARLIM 2
                    iPb = i-1
                    exit loopRs1
                end if rsDiminishes

            end do loopRs1

            resetIPb: if (iPb == 1) then
                ! Não aceitar PSat a partir da Análise PVT caso esta não contenha pelo menos 2 medidas acima de PSat.
                iPb = -1
                dPSat = -10.0d0
            end if resetIPb

        else findPSat

            ! Pressão de saturação conhecida a priori. Adotá-la!
            dPSat = dGivenPSat
            iFound = -1

            loopPVT1: do i = 1, nPVTPoints

                if (oExpPressures(i) < dGivenPSat) then
                    iFound = i
                    exit loopPVT1
                end if

            end do loopPVT1

            iPb = iFound                    ! = -1 se, por exemplo, for especificada uma PSat extremamente baixa.
            
            if(iPb.lt.0) dPSat = -10.0d0    ! Nesta situação, melhor tentar o cálculo por uma correlação.

        end if findPSat
    
        ! CÁLCULO DE Rs POR TODAS AS CORRELAÇÕES, PARA TODOS OS PONTOS DA ANÁLISE PVT:

            ! Caso se deseje excluir determinada correlação abaixo da rotina, basta substituir sua constante por um valor negativo no vetor.
        oTestedRsCorrelations(1) = RSCORRELATION_VAZQUEZBEGGS
        oTestedRsCorrelations(2) = RSCORRELATION_LASATER
        oTestedRsCorrelations(3) = RSCORRELATION_STANDING
        oTestedRsCorrelations(4) = RSCORRELATION_GLASO

        loopPVTPressures: do i = 1, nPVTPoints

            rsInPVTPressures: do iRsCorr = 1, RSCORRELATION_COUNT

                corrApplies: if(oTestedRsCorrelations(iRsCorr).lt.0) then
                    oCorrelationsRs(iRsCorr, i) = -10.0d0
                    oRsCorrelationsPSat(iRsCorr) = -10.0d0
                    cycle rsInPVTPressures
                end if corrApplies

                call CalculateBlackOilSolutionGasOilRatio( &
                    oTestedRsCorrelations(iRsCorr), oExpPressures(i), dExpTemperature, (dPSat * 14.22334), dPSEP_VB, dTSEP_VB, &
                    dAPI, dRGO, dDeng, dPbTemp, dRsResult, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

                oCorrelationsRs(iRsCorr, i) = dRsResult / 1.589873d-1 * 2.831685d-2       ! Convertendo de scft/bbl para m3/m3 antes de armazenar.

                oRsCorrelationsPSat(iRsCorr) = -10.0d0
                keepCorrPSat: if (dPSat.lt.(0.0d0)) then
                    ! Pressão de saturação foi calculada automaticamente junto com "Rs". Armazenar o resultado!
                    oRsCorrelationsPSat(iRsCorr) = dPbTemp / 14.22334    ! Convertendo de psia para kgf/cm2 
                end if keepCorrPSat

            end do rsInPVTPressures

        end do loopPVTPressures
        
        ! ADICIONA Rs EXPERIMENTAL NA ÚLTIMA LINHA:
        includeExpRs: do i = 1, nPVTPoints
            oCorrelationsRs(RSCORRELATION_COUNT+1, i) = oExpRs(i)
        end do includeExpRs

        ! CÁLCULO DA NORMA L2 PARA CADA CORRELAÇÃO DE Rs:
        calcRsL2: do iRsCorr = 1, RSCORRELATION_COUNT
   
            corrApplies2:if(oTestedRsCorrelations(iRsCorr).lt.0) then
                oRsNormL2(iRsCorr) = 1.0d8    ! Valor alto arbitrário!
                cycle calcRsL2
            end if corrApplies2

            dNorm = 0.0d0
            
            RsCorrL2Sum: do i = 1, nPVTPoints
                dDiff = oCorrelationsRs(iRsCorr, i) - oExpRs(i)
                dNorm = dNorm + (dDiff*dDiff)         
            end do RsCorrL2Sum
            
            oRsNormL2(iRsCorr) = sqrt(dNorm)
        
        end do calcRsL2
  
        ! SELECIONA A MELHOR CORRELAÇÃO PARA Rs (MENOR NORMA L2):
        dNormMin = oRsNormL2(1)
        iBest = -1

        findBestRsCorr: do iRsCorr = 1, RSCORRELATION_COUNT

            if(oTestedRsCorrelations(iRsCorr).lt.0) cycle findBestRsCorr

            isBetterRsCorr: if (oRsNormL2(iRsCorr).le.dNormMin) then
                dNormMin = oRsNormL2(iRsCorr)
                iBest = iRsCorr
            end if isBetterRsCorr

        end do findBestRsCorr

        iBestRsCorrelation = oTestedRsCorrelations(iBest)
        if(iPb.lt.0) dPSat = oRsCorrelationsPSat(iBest)
  
        ! AJUSTE FINAL DE iPb SE NECESSÁRIO:
        iPbFinalChk: if(iPb.lt.0) then

            iFound = -1

            finalFindIPB: do i = 1, nPVTPoints
                if (oExpPressures(i) < dPSat) then
                    iFound = i
                    exit finalFindIPB 
                end if
            end do finalFindIPB

            iPb = iFound

        end if iPbFinalChk

        ! Garantir que encontrou iPb, pois sem ele, não é possível fazer o ajuste:
        hasValidIpb: if(iPb.lt.0) then
            iIER = ERROR_SatPressureNotFoundWithinPVTAnalysisBounds
            return
        end if hasValidIpb

        ! SUBROTINA CONFERIDA ATÉ O FINAL EM 26/03/2026. SEGUIR!

    end subroutine CalculateBlackOilInputForPVTAnalysisRegression

    ! =============================================================
    ! =============================================================
    subroutine CorrectBlackOilRsWithPVTAnalysisRegression(iRsRegressionModelType, dRsRegressionA, dRsRegressionB, &
        dRsCalculated, dSaturationPressure, dRGO_RsUpperLimit, dLocalPressure, dRsCorrected)

        ! OBJETIVO: Centraliza o cálculo do valor final de Rs em contexto de ajuste contra Análise PVT, usando quando necessário os coeficientes
        !           de regressão obtidos para este fim, considerando também a pressão de saturação e limites físicos aplicáveis.
        !           (Aplica proteções conforme práticas das rotinas PVT2 e PVT2A do código original).
        !           Almeja-se que esta subrotina seja acionada tanto internamente a este código, quanto diretamente pelo MARLIM 3 ao longo de uma simulação.

        ! IMPORTANTE: Considera-se que no ajuste dos parâmetros "dRsRegressionA" e "dRsRegressionB", tenha sido
        !             feita regressão de Rs x Rs sempre usando as unidades de m3/m3! 

        implicit none

        !------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iRsRegressionModelType   ! Tipo do modelo de regressão: 1=linear, 0=potencial (IRS em PVT2 / PVT2A)
        real(c_double), intent(in) :: dRsRegressionA           ! Coeficiente angular do modelo de regressão (ARS em PVT2 / PVT2A)
        real(c_double), intent(in) :: dRsRegressionB           ! Coeficiente linear do modelo de regressão (BRS ou DBRS em PVT2 / PVT2A)
        real(c_double), intent(in) :: dRsCalculated            ! Rs calculado pela correlação, em m3/m3 (RST em PVT2 / PVT2A)
        real(c_double), intent(in) :: dSaturationPressure      ! Pressão de saturação (kgf/cm2)
        real(c_double), intent(in) :: dRGO_RsUpperLimit        ! Limite superior de Rs (RGO ou GORM do código original, em Sm3/Sm3)
        real(c_double), intent(in) :: dLocalPressure           ! Pressão local (kgf/cm2)

        real(c_double), intent(out) :: dRsCorrected            ! Rs corrigido (resultado final), em m3/m3

        !------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dRsTemp

        !------------------ CÁLCULOS:

        checkPSat: if (dLocalPressure >= dSaturationPressure) then

            ! Acima da pressão de saturação: Rs saturado
            dRsCorrected = dRGO_RsUpperLimit

        else checkPSat

            ! Abaixo da pressão de saturação: aplicar modelo de regressão
            checkPotMod: if ((iRsRegressionModelType == 0).and.(dRsCalculated <= 0.0d0)) then

                ! Para o modelo potencial, o resultado poderia ser indefinido (especialmente para
                !   expoentes não inteiros).
                dRsCorrected = 0.0d0

            else checkPotMod

                call ApplyBlackOilPVTAnalysisCorrectionModel( &
                    iRsRegressionModelType, dRsRegressionA, dRsRegressionB, dRsCalculated, dRsTemp)

                dRsCorrected = dRsTemp

            end if checkPotMod

        end if checkPSat

        ! Proteção: Rs negativo
        if (dRsCorrected < 0.0d0) dRsCorrected = 0.0d0

        ! Proteção: Rs maior que limite superior:
        if (dRsCorrected > dRGO_RsUpperLimit) dRsCorrected = dRGO_RsUpperLimit

        ! ROTINA CONFERIDA ATÉ O FINAL EM 06/04/2026!
        ! CONFERÊNCIA FINALIZADA! PODE SEGUIR COM O PRÓXIMO PASSO DO TRABALHO!

    end subroutine CorrectBlackOilRsWithPVTAnalysisRegression

    ! =============================================================
    ! =============================================================
    subroutine ApplyBlackOilPVTAnalysisCorrectionModel(iModelType, dA, dB, dValueCalculated, dValueCorrected)

        ! OBJETIVO: Aplica o modelo de correção calibrado (linear ou potencial) sobre um valor
        !           calculado por correlação (Rs ou Bo), retornando o valor corrigido correspondente.
        !           Importante: Os coeficientes de calibração (a, b) e o tipo de modelo são aqueles fornecidos
        !           pela rotina de regressão "CalculateBlackOilPVTAnalysisCorrectionModelParameters".
        !           (Corresponde à aplicação dos modelos na rotina PVT2 do arquivo fonte "MRL_pvt.pas" do código Pascal original.)

        implicit none

        !------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iModelType         ! Tipo de modelo: 1=linear, 0=potencial (IRS / IBO no código original, em PVT2 / PVT2A)
        real(c_double), intent(in) :: dA                 ! Coeficiente angular do modelo de correção (ARS / ABO em PVT2A)
        real(c_double), intent(in) :: dB                 ! Coeficiente linear do modelo de correção (BRS / BBO em PVT2A, ou DBRS / DBBO em modelo potencial)
        real(c_double), intent(in) :: dValueCalculated   ! Valor calculado por correlação black-oil (RST / BO em PVT2)

        real(c_double), intent(out) :: dValueCorrected   ! Valor corrigido da propriedade black-oil (RS / BO em PVT2)

        !------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dB_Converted                   ! Coeficiente linear convertido para modelo potencial (BRS / BBO em PVT2)

        !------------------ CÁLCULOS:

        checkModelType: select case (iModelType)

            case (1) checkModelType

                ! Modelo linear: Y = b + a * X
                ! (RS := BRS + RST * ARS) ou (BO := BBO + BO * ABO) em PVT2
                dValueCorrected = dB + dA * dValueCalculated

            case (0) checkModelType

                ! Modelo potencial: Y = b * X^a, sendo b = 10^dB (dB = DBRS / DBBO em PVT2A, linha 298 de "MRL_pvt.pas")
                ! (RS := BRS * POWER(RST, ARS)) ou (BO := BBO * POWER(BO, ABO)) em PVT2
                dB_Converted = 10.0d0 ** dB
                dValueCorrected = dB_Converted * (dValueCalculated ** dA)

            case default checkModelType

                ! Modelo desconhecido: retorna valor não corrigido
                dValueCorrected = dValueCalculated

        end select checkModelType

        ! ROTINA CONFERIDA ATÉ O FINAL EM 31/03/2026 (inclusive contra o MARLIM 2). SEGUIR!

    end subroutine ApplyBlackOilPVTAnalysisCorrectionModel

    ! =============================================================
    ! =============================================================
    subroutine CalculateBlackOilPVTAnalysisCorrectionModelParameters(iIndexStart, iIndexEnd, dX, dY, &
                                                     iModelType, dA_Final, dB_Final, dC, dR2, dC_pot, &
                                                     dR2_pot)

        ! OBJETIVO: Implementa, de forma genérica para Rs e Bo, os seguintes passos de ajuste de
        !           calibração contra Análise PVT:
        !           1 - Executa regressão linear (mínimos quadrados)
        !           2 - Executa regressão potencial (mínimos quadrados nos logs)
        !           3 - Compara os erros dos dois modelos
        !           4 - Escolhe o melhor modelo para a propriedade
        
        ! REFERÊNCIA: Linhas 272 a 313 do arquivo-fonte original MRL_pvt.pas 

        implicit none

        !------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iIndexStart      ! Índice inicial dos dados (IPB do original)
        integer(c_int), intent(in) :: iIndexEnd        ! Índice final dos dados (NPVT do original)
        real(c_double), intent(in) :: dX(:)            ! Vetor de valores calculados pela correlação (RSCC / BOCC no original)
        real(c_double), intent(in) :: dY(:)            ! Vetor de valores medidos na Análise PVT (RSPVT / BOPVT no original)

        integer(c_int), intent(out) :: iModelType      ! Tipo de modelo escolhido: 1=linear, 0=potencial (IRS / IBO no original)
        real(c_double), intent(out) :: dA_Final        ! Coef. angular do modelo final
        real(c_double), intent(out) :: dB_Final        ! Coef. linear do modelo final
        real(c_double), intent(out) :: dC              ! Erro/qualidade do ajuste linear (CRS / CBO no original)
        real(c_double), intent(out) :: dR2             ! R2 = coeficiente de determinação do modelo linear
        real(c_double), intent(out) :: dC_pot          ! Erro/qualidade do ajuste potencial (DCRS / DCBO no original)
        real(c_double), intent(out) :: dR2_pot         ! R2 = coeficiente de determinação do modelo potencial

        !------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        real(c_double) :: dA              ! Coef. angular do modelo linear (ARS / ABO no original)
        real(c_double) :: dB              ! Coef. linear do modelo linear (BRS / BBO no original)
        real(c_double) :: dA_pot          ! Coef. angular do modelo potencial (DARS / DABO no original)
        real(c_double) :: dB_pot          ! Coef. linear do modelo potencial (DBRS / DBBO no original)
        integer(c_int) :: i, iLogNPoints
        real(c_double), allocatable :: dLogX(:), dLogY(:)
        integer(c_int) :: iNPoints

        !------------------ CÁLCULOS:

        ! REGRESSÃO LINEAR (mínimos quadrados):
        call CalculateBlackOilPVTAnalysisLinearFitParameters(iIndexStart, iIndexEnd, dX, dY, dA, dB, dC, dR2)

        ! REGRESSÃO POTENCIAL (mínimos quadrados nos logs):
        iNPoints = iIndexEnd - iIndexStart + 1

        allocate(dLogX(iNPoints))
        allocate(dLogY(iNPoints))

        iLogNPoints = 0

        getLogs: do i = iIndexStart, iIndexEnd

            ! Só considera pares onde ambos X e Y são positivos
            isPositive: if (dX(i) > 0.0d0 .and. dY(i) > 0.0d0) then
                iLogNPoints = iLogNPoints + 1
                dLogX(iLogNPoints) = log10(dX(i))   ! (DRSC / DBOC no original)
                dLogY(iLogNPoints) = log10(dY(i))   ! (DRSPVT / DBOPVT no original)
            end if isPositive

        end do getLogs

        if (iLogNPoints >= 2) then
            call CalculateBlackOilPVTAnalysisLinearFitParameters(1, iLogNPoints, dLogX, dLogY, dA_pot, dB_pot, dC_pot, dR2_pot)
        else
            dA_pot = 0.0d0
            dB_pot = 0.0d0
            dC_pot = -1.0d7
            dR2_pot = -1.0d7
        end if

        deallocate(dLogX)
        deallocate(dLogY)

        ! ESCOLHA DO MELHOR MODELO (linear ou potencial):
        
        ! Critério: maior qualidade de ajuste (maior dC / dC_pot)
        chooseBestModel: if (dC >= dC_pot) then

            iModelType = 1   ! 1 = linear (IRS / IBO = 1 no original)

            dA_Final = dA
            dB_Final = dB

        else chooseBestModel

            iModelType = 0   ! 0 = potencial (IRS / IBO = 0 no original)

            dA_Final = dA_pot
            dB_Final = dB_pot

        end if chooseBestModel

        ! ROTINA CONFERIDA INTEGRALMENTE, INCLUSIVE CONTRA PVT2A ORIGINAL.

    end subroutine CalculateBlackOilPVTAnalysisCorrectionModelParameters
    
    ! =============================================================
    ! =============================================================
    subroutine CalculateBlackOilPVTAnalysisLinearFitParameters(iPb, iNppvtm, dX, dY, dA, dB, dC, dR2)

        ! OBJETIVO: Transcrição da rotina MMQ do arquivo MRL_pvt.pas do MARLIM 2.
        !           Realiza ajuste linear pelo método dos mínimos quadrados.
        !           Calcula os coeficientes angular (A), linear (B) e de correlação (C)
        !           para o ajuste Y = A*X + B, usando os dados de entrada.

        implicit none

        !------------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iPb         ! Índice do ponto de pressão de bolha nos dados da Anáise PVT (IPB do original)
        integer(c_int), intent(in) :: iNppvtm     ! Número total de pontos da Análise PVT (NPPVTM do original)
        real(c_double), intent(in) :: dX(:)       ! Vetor de valores X (XP do original)
        real(c_double), intent(in) :: dY(:)       ! Vetor de valores Y (YP do original)
        
        real(c_double), intent(out) :: dA         ! Coeficiente angular da reta (A do original)
        real(c_double), intent(out) :: dB         ! Coeficiente linear da reta (B do original)
        real(c_double), intent(out) :: dC         ! Coeficiente de correlação (C do original)
        real(c_double), intent(out) :: dR2        ! R2 = coeficiente de determinação (não estava no código original)

        !------------------ DECLARAÇÃO E DESCRIÇÃO DE VARIÁVEIS LOCAIS:
        integer(c_int) :: iIndex                  ! Índice do loop (i do original)
        real(c_double) :: dSumX                   ! Soma dos X (SX do original)
        real(c_double) :: dSumY                   ! Soma dos Y (SY do original)
        real(c_double) :: dSumX2                  ! Soma dos X^2 (SX2 do original)
        real(c_double) :: dSumY2                  ! Soma dos Y^2 (SY2 do original)
        real(c_double) :: dSumXY                  ! Soma dos X*Y (SXY do original)
        real(c_double) :: dSSXY                   ! Soma dos produtos centrados (SSXY do original)
        real(c_double) :: dSSXX                   ! Soma dos quadrados centrados de X (SSXX do original)
        real(c_double) :: dSSYY                   ! Soma dos quadrados centrados de Y (SSYY do original)
        integer(c_int) :: iNPoints                ! Número de pontos usados no ajuste
  
        !------------------ CÁLCULOS:
        dSumX  = 0.0d0
        dSumY  = 0.0d0
        dSumX2 = 0.0d0
        dSumY2 = 0.0d0
        dSumXY = 0.0d0

        ! NOTA: Ao que tudo indica, IPB no código original é o índice do ponto de pressão de bolha nos dados PVT.
        !       Ou seja: para i < IPB o Rs (por exemplo) é fixo. E para i >= IPB, aplica-se a calibração.
        !       A regressão começa em IPB para garantir que o ajuste dos coeficientes seja feito apenas na região fisicamente
        !       relevante, evitando distorções causadas por pontos onde Rs, Bo e viscosidade não variam com a pressão.

        do iIndex = iPb, iNppvtm
            dSumX  = dSumX  + dX(iIndex)
            dSumY  = dSumY  + dY(iIndex)
            dSumX2 = dSumX2 + dX(iIndex) * dX(iIndex)
            dSumY2 = dSumY2 + dY(iIndex) * dY(iIndex)
            dSumXY = dSumXY + dX(iIndex) * dY(iIndex)
        end do

        iNPoints = iNppvtm - iPb + 1
        
        dSSXY = dSumXY - dSumX * dSumY / iNPoints
        dSSXX = dSumX2 - dSumX * dSumX / iNPoints
        dSSYY = dSumY2 - dSumY * dSumY / iNPoints

        dA = dSSXY / dSSXX
        dB = (dSumY - dA * dSumX) / iNPoints
        dC = abs(dSSXY / sqrt(dSSXX * dSSYY))    ! Valor absoluto do coeficiente de correlação de Pearson
        dR2 = dC * dC

        ! ROTINA INTEGRALMENTE CONFERIDA CONTRA MMQ ORIGINAL EM 12/03/2026!
        ! SUBROTINA PRONTA! PODE SEGUIR!

    end subroutine CalculateBlackOilPVTAnalysisLinearFitParameters

end module BlackOilModels
