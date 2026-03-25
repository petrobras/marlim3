module PvtSimCTMFileImport

    ! OBJETIVO: Conter as rotinas que fazem a leitura automática de arquivos "ctm" do PVTSIM.

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use Constants

    implicit none

    contains

    ! =============================================================
    ! =============================================================
    subroutine ReadMixtureComponentsFromPVTSIMFileCTM(sPvtSimCTMFileName, bReadOnlyINComp, iNComp, oZ, oMW, oTc, oPc, oW, oParachors, &
                                                        oKij, oLij, oTIndep_Peneloux, iThermodynamicModel, oCpIGCoefs, oPVTSIMViscCSPTuningParam, &
                                                        iIER)

        ! OBJETIVO: Ler automaticamente, a partir de um arquivo "ctm" exportado do PVTSIM,
        !   todas as propriedades dos componentes de um fluido que se deseja calcular "composicionalmente"
        !   no Marlim Transiente.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        character(len=*), intent(in) :: sPvtSimCTMFileName              ! Nome do arquivo "ctm" do PvtSim do qual se deseja importar os dados.
        logical, intent(in) :: bReadOnlyINComp                          ! "True" para ler apenas o número de componentes e sair.
        integer(c_int), intent(out) :: iNComp                           ! RESULTADO: Número de componentes.
        real(c_double), dimension(:), intent(out) :: oZ                 ! RESULTADO: Frações molares dos componentes na mistura.
        real(c_double), dimension(:), intent(out) :: oMW                ! RESULTADO: Massas molares dos componentes na mistura (g/gmol).
        real(c_double), dimension(:), intent(out) :: oTc                ! RESULTADO: Temperaturas críticas dos componentes na mistura.
        real(c_double), dimension(:), intent(out) :: oPc                ! RESULTADO: Pressões críticas dos componentes na mistura.
        real(c_double), dimension(:), intent(out) :: oW                 ! RESULTADO: Fatores acêntricos dos componentes na mistura.
        real(c_double), dimension(:), intent(out) :: oParachors         ! RESULTADO: "Parachors" dos componentes na mistura.
        real(c_double), dimension(:,:), intent(out) :: oKij             ! RESULTADO: Parâmetros de interação "kij".
        real(c_double), dimension(:,:), intent(out) :: oLij             ! RESULTADO: Parâmetros de interação "Lij".
        real(c_double), dimension(:), intent(out) :: oTIndep_Peneloux   ! RESULTADO: Coeficiente de Peneloux independente de temperatura, como volume molar (SI).
        integer(c_int), intent(out) :: iThermodynamicModel              ! RESULTADO: Índice do modelo termodinâmico (EOS), conforme convenção.
        real(c_double), dimension(:,:), intent(out) :: oCpIGCoefs       ! RESULTADO: Coeficientes polinomiais de Cp de gás ideal em função de T.
                                                                        ! Cp(componente i) = coef(1,i) + coef(2, i)*T + coef(3, i)*T2 + coef(4, i)*T3
        real(c_double), dimension(4), intent(out) :: oPVTSIMViscCSPTuningParam       ! Coeficientes ajustados para o método CSP de viscosidade
        integer(c_int), intent(out) :: iIER                             ! RESULTADO: Código de erros, conforme convenção.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iFileOperationsError                                 ! Indicador de erros ou sucesso em operações do Fortran com o arquivo.
        character(len=200) :: sPvtSimCTMFileLine
        logical :: bReadNumberOfComponentsFromThisLine                  ! "True" para ler da atual linha o número de componentes.
        real(c_double) :: dMultFactor                                   ! Fator multiplicativo a aplicar sobre cada valor lido.
        real(c_double), allocatable, dimension(:) :: oKij_PR_Read       ! "Kij" de Peng-Robinson que foi lido do arquivo.
        real(c_double), allocatable, dimension(:) :: oKij_SRK_Read      ! "Kij" de SRK que foi lido do arquivo.
        real(c_double), allocatable, dimension(:) :: oKij_Read          ! "Kij" que foi lido do arquivo.
        integer :: iKijCTMVectorSize                                    ! Tamanho do vetor de "kij" lido do arquivo.
        integer :: iMemoryAllocationError
        integer :: i, j                                                 ! Índices de laços "do".
        integer :: iKijLine, iKijColumn
        logical :: bUsePengRobinsonKij, bUseSRKKij
        real(c_double), allocatable, dimension(:) :: oTIndep_Peneloux_PR
        logical :: bUsePengRobinsonPeneloux, bUseSRKPeneloux
        real(c_double), allocatable, dimension(:) :: oTIndep_Peneloux_SRK
        real(c_double), allocatable, dimension(:) :: oCpIGCoefs_Read
        integer :: iCpIGCoefDegree
        character(len=200) :: sGarbageLine
        real(c_double), dimension(17) :: oFluidFPROPSValues
        integer :: iCommaCount
        integer, allocatable, dimension(:) :: oFluidIPROPSValues

        real(c_double), allocatable, dimension(:) :: oTDep_Peneloux_SRK
        real(c_double), allocatable, dimension(:) :: oTDep_Peneloux_PR

        ! ------------ CONSTANTES:
        integer, parameter :: iPvtSimCTMFileUnit = 1
        logical, parameter :: bReadEOSFromFluidIPROPSSection = .true.

        ! ------------ PROCEDIMENTOS:

        ! Abrir o arquivo:
        OPEN(UNIT=iPvtSimCTMFileUnit, FILE=sPvtSimCTMFileName, STATUS='OLD', ACTION='READ', IOSTAT=iFileOperationsError)

        if(iFileOperationsError.NE.0) then
            ! Não foi possível abrir o arquivo!
            iIER = ERROR_FailedToOpenExternalFileForMixtureProperties
            return
        end if

        ! Inicializações necessárias:
        bReadNumberOfComponentsFromThisLine = .false.

            ! Alocar logo o vetor intermediário de coeficientes de Cp:
        allocateCp: if(iNComp.GT.0) then

            allocate(oCpIGCoefs_Read(iNComp), STAT=iMemoryAllocationError)

            if(iMemoryAllocationError.NE.0) then
                ! Erro!
                iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                return
            end if

        end if allocateCp

        ! Percorrer linha a linha do arquivo:
        readLoop: do

            ! Ler uma linha:
            read(iPvtSimCTMFileUnit, '(A)', IOSTAT=iFileOperationsError) sPvtSimCTMFileLine

            ! Arquivo acabou?
            if(iFileOperationsError.LT.0) exit readLoop

            ! Leitura deu erro?
            if(iFileOperationsError.GT.0) then
                iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                return
            end if

            ! Processar devidamente a linha atual:
            lineType: if(bReadNumberOfComponentsFromThisLine) then

                ! Ler desta linha o número de componentes da mistura:
                read(sPvtSimCTMFileLine, '(I5)', IOSTAT=iFileOperationsError) iNComp

                if(iFileOperationsError.NE.0) then
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                bReadNumberOfComponentsFromThisLine = .false.

                if(bReadOnlyINComp) then

                    ! Rebobinar e fechar o arquivo!
                    REWIND(UNIT=iPvtSimCTMFileUnit)
                    CLOSE(UNIT=iPvtSimCTMFileUnit)

                    ! Retornar
                    return

                end if

                cycle readLoop

            else if(sPvtSimCTMFileLine(1:17) == '<Short Comp Name>') then lineType

                ! Preparar para ler, da próxima linha, o número de componentes:
                bReadNumberOfComponentsFromThisLine = .true.
                cycle readLoop

            else if(    ((sPvtSimCTMFileLine(1:29) == '<Comp Amount CT> mole percent').OR.(sPvtSimCTMFileLine(1:28) == '<Comp Amount CT> mol percent')) &
                        .AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler, das próximas linhas, as porcentagens molares:
                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, 0.01d0, &
                        oZ, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:17) == '<Acentric Factor>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler das próximas linhas os fatores acêntricos:
                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, 1.0d0, &
                        oW, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:4) == '<Tc>').AND.(.not.bReadOnlyINComp)) then lineType

                    ! Ler das próximas linhas as temperaturas críticas:
                    tCUnit: if(sPvtSimCTMFileLine(5:len_trim(sPvtSimCTMFileLine)) == ' (K)') then
                        dMultFactor = 1.0d0
                    else tCUnit
                        ! Erro! Unidade não reconhecida!
                        iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                        return
                    end if tCUnit

                    call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                        oTc, iIER)

                    if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:4) == '<Pc>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler das próximas linhas as pressões críticas:
                pCUnit: if(sPvtSimCTMFileLine(5:len_trim(sPvtSimCTMFileLine)) == ' (atm)') then
                    dMultFactor = 101324.99658d0
                else pCUnit
                        ! Erro! Unidade não reconhecida!
                        iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                        return
                end if pCUnit

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oPc, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:9) == '<Cpen PR>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler coeficientes de Peneloux para EOS de Peng-Robinson.
                cPenPRUnit: if(sPvtSimCTMFileLine(10:len_trim(sPvtSimCTMFileLine)) == ' c/R (K/atm)') then
                    dMultFactor = 82.06d0               ! R em "cm3*atm / (mol*K)" --> levando para "cm3 / mol"
                    dMultFactor = dMultFactor * 1.0d-6  ! --> levando para "m3/mol" (SI)
                else cPenPRUnit
                    ! Erro! Unidade não reconhecida!
                    iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                    return
                end if cPenPRUnit

                allocate(oTIndep_Peneloux_PR(iNComp), STAT=iMemoryAllocationError)

                if(iMemoryAllocationError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oTIndep_Peneloux_PR, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:10) == '<Cpen SRK>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler coeficientes de Peneloux para EOS de SRK.
                cPenSRKUnit: if(sPvtSimCTMFileLine(11:len_trim(sPvtSimCTMFileLine)) == ' c/R (K/atm)') then
                    dMultFactor = 82.06d0               ! R em "cm3*atm / (mol*K)" --> levando para "cm3 / mol"
                    dMultFactor = dMultFactor * 1.0d-6  ! --> levando para "m3/mol" (SI)
                else cPenSRKUnit
                    ! Erro! Unidade não reconhecida!
                    iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                    return
                end if cPenSRKUnit

                allocate(oTIndep_Peneloux_SRK(iNComp), STAT=iMemoryAllocationError)

                if(iMemoryAllocationError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oTIndep_Peneloux_SRK, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:12) == '<Cpen T SRK>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler coeficientes de Peneloux (dependência com a temperatura) para EOS de SRK
                cPenTSRKUnit: if(sPvtSimCTMFileLine(13:len_trim(sPvtSimCTMFileLine)) == ' cT/R (1/atm)') then
                    dMultFactor = 82.06d0               ! R em "cm3*atm / (mol*K)" --> levando para "cm3 / (mol*K)"
                    dMultFactor = dMultFactor * 1.0d-6  ! --> levando para "m3/(mol*K)" (SI)
                else cPenTSRKUnit
                    ! Erro! Unidade não reconhecida!
                    iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                    return                    
                end if cPenTSRKUnit

                allocate(oTDep_Peneloux_SRK(iNComp), STAT=iMemoryAllocationError)

                if(iMemoryAllocationError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oTDep_Peneloux_SRK, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

                ! 2 DE FEVEREIRO DE 2026: trecho de código lido e conferido. Leitura funcionando (confirmado via debug).

            else if((sPvtSimCTMFileLine(1:11) == '<Cpen T PR>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler coeficientes de Peneloux (dependência com a temperatura) para EOS de PR
                cPenTPRUnit: if(sPvtSimCTMFileLine(12:len_trim(sPvtSimCTMFileLine)) == ' cT/R (1/atm)') then
                    dMultFactor = 82.06d0               ! R em "cm3*atm / (mol*K)" --> levando para "cm3 / (mol*K)"
                    dMultFactor = dMultFactor * 1.0d-6  ! --> levando para "m3/(mol*K)" (SI)
                else cPenTPRUnit
                    ! Erro! Unidade não reconhecida!
                    iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                    return   
                end if cPenTPRUnit

                allocate(oTDep_Peneloux_PR(iNComp), STAT=iMemoryAllocationError)

                if(iMemoryAllocationError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oTDep_Peneloux_PR, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

                ! PR e SRK CONFERIDOS EM 23-OUT-2025. Só falta: (i) testar a leitura de PenT para PR e SRK, e (ii) encaminhar para argumento de saída.

                ! 2 DE FEVEREIRO DE 2026: trecho de código lido e conferido. Leitura funcionando (confirmado via debug)!

            else if((sPvtSimCTMFileLine(1:20) == '<Mwn> Number average').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler das próximas linhas as massas molares:
                mWUnit: if(sPvtSimCTMFileLine(21:28) == ' (g/mol)') then
                    dMultFactor = 1.0d0
                else mWUnit
                        ! Erro! Unidade não reconhecida!
                        iIER = ERROR_UnrecognizedUnitInExternalFileForMixtureProperties
                        return
                end if mWUnit

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oMW, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:10) == '<Parachor>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler das próximas linhas as "parachors":
                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, 1.0d0, &
                        oParachors, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:8) == '<kij PR>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler das próximas linhas os "kij" de Peng-Robinson.
                iKijCTMVectorSize = (iNComp * iNComp - iNComp) / 2
                allocate(oKij_PR_Read(iKijCTMVectorSize), STAT=iMemoryAllocationError)

                if(iMemoryAllocationError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iKijCTMVectorSize, 1.0d0, &
                    oKij_PR_Read, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:14) == '<Fluid FPROPS>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Agora, ler 17 coeficientes:
                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, 17, 1.0d0, &
                    oFluidFPROPSValues, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

                ! Separar os coeficientes recuperados conforme o interesse:

                    ! Coeficientes ajustados do método CSP de viscosidade:
                oPVTSIMViscCSPTuningParam(1) = oFluidFPROPSValues(2)
                oPVTSIMViscCSPTuningParam(2) = oFluidFPROPSValues(3)
                oPVTSIMViscCSPTuningParam(3) = oFluidFPROPSValues(13)
                oPVTSIMViscCSPTuningParam(4) = oFluidFPROPSValues(14)

            else if((sPvtSimCTMFileLine(1:9) == '<kij SRK>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler das próximas linhas os "kij" de SRK.
                iKijCTMVectorSize = (iNComp * iNComp - iNComp) / 2
                allocate(oKij_SRK_Read(iKijCTMVectorSize), STAT=iMemoryAllocationError)

                if(iMemoryAllocationError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iKijCTMVectorSize, 1.0d0, &
                    oKij_SRK_Read, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

            else if((sPvtSimCTMFileLine(1:3) == '<Cp').AND. &
                    (sPvtSimCTMFileLine(11:44) == 'coefficient in expression for Cp/R').AND. &
                    (.not.bReadOnlyINComp)) then lineType

                ! Ler mais coeficientes de Cp de gás ideal:
                dMultFactor = 8.314d0   ! R em "J/(mol*K)" --> levando o Cp do polinômio a estas mesmas unidades (SI)

                call ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNComp, dMultFactor, &
                    oCpIGCoefs_Read, iIER)

                if(iIER.NE.ERROR_EverythingOK) return

                read(sPvtSimCTMFileLine(4:4), &
                         * , IOSTAT=iFileOperationsError) iCpIGCoefDegree

                if(iFileOperationsError.NE.0) then
                    ! Erro!
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                do i = 1, iNComp
                    oCpIGCoefs(iCpIGCoefDegree, i) = oCpIGCoefs_Read(i)
                end do

            else if((sPvtSimCTMFileLine(1:14) == '<Fluid IPROPS>').AND.(.not.bReadOnlyINComp)) then lineType

                ! Ler duas linhas; ficar com a segunda (contém todas as opções separadas por vírgula):
                read(iPvtSimCTMFileUnit, '(A)', IOSTAT=iFileOperationsError) sPvtSimCTMFileLine
                read(iPvtSimCTMFileUnit, '(A)', IOSTAT=iFileOperationsError) sPvtSimCTMFileLine

                ! Contar quantas vírgulas existem na linha:
                iCommaCount = 0
                commaCountLoop: do i = 1, len_trim(sPvtSimCTMFileLine)
                    if(sPvtSimCTMFileLine(i:i) == ',') iCommaCount = iCommaCount + 1
                end do commaCountLoop

                ! Ler todos os valores de "iprops":
                allocate(oFluidIPROPSValues(iCommaCount+1), STAT=iMemoryAllocationError)
                read(sPvtSimCTMFileLine, *, IOSTAT=iFileOperationsError) (oFluidIPROPSValues(i), i = 1, iCommaCount+1)
                
                ! Ler a Equação de Estado daqui, se for a opção ligada:
                getEosFromHere: if(bReadEOSFromFluidIPROPSSection) then

                    ! Inicializando:
                    bUsePengRobinsonKij = .false.
                    bUseSRKKij = .false.

                    bUsePengRobinsonPeneloux = .false.
                    bUseSRKPeneloux = .false.

                    treatIPROPSEOS: select case(oFluidIPROPSValues(2))

                        case(1) treatIPROPSEOS

                            ! PVTSIM: SRK PENELOUX
                            iThermodynamicModel = SRK_PENELOUX

                            bUseSRKKij = .true.
                            bUseSRKPeneloux = .true.

                        case(2) treatIPROPSEOS

                            ! PVTSIM: PR
                            iThermodynamicModel = PENG_ROBINSON_PENELOUX

                            bUsePengRobinsonKij = .true.

                        case(3) treatIPROPSEOS

                            ! PVTSIM: SRK
                            iThermodynamicModel = SRK_PENELOUX

                            bUseSRKKij = .true.

                        case(4) treatIPROPSEOS

                            ! PVTSIM: PR PENELOUX
                            iThermodynamicModel = PENG_ROBINSON_PENELOUX

                            bUsePengRobinsonKij = .true.
                            bUsePengRobinsonPeneloux = .true.

                        case(5) treatIPROPSEOS

                            ! PVTSIM: PR78
                            iThermodynamicModel = PENG_ROBINSON_78_PENELOUX

                            bUsePengRobinsonKij = .true.

                        case(6) treatIPROPSEOS

                            ! PVTSIM: PR 78 PENELOUX
                            iThermodynamicModel = PENG_ROBINSON_78_PENELOUX

                            bUsePengRobinsonKij = .true.
                            bUsePengRobinsonPeneloux = .true.

                        case(7) treatIPROPSEOS

                            ! PVTSIM: SRK PENELOUX T
                            iThermodynamicModel = -1        ! EOS mapeada, mas ainda não implementada

                            ! Apagar quando implementar:
                            iIER = ERROR_UnrecognizedEOSInExternalFileForMixtureProperties
                            return

                        case(8) treatIPROPSEOS

                            ! PVTSIM: PR PENELOUX T
                            iThermodynamicModel = -1        ! EOS mapeada, mas ainda não implementada

                            ! Apagar quando implementar:
                            iIER = ERROR_UnrecognizedEOSInExternalFileForMixtureProperties
                            return

                        case(9) treatIPROPSEOS

                            ! PVTSIM: PR 78 PENELOUX T
                            iThermodynamicModel = -1        ! EOS mapeada, mas ainda não implementada

                            ! Apagar quando implementar:
                            iIER = ERROR_UnrecognizedEOSInExternalFileForMixtureProperties
                            return                           

                        case default treatIPROPSEOS

                            ! Equação de Estado inválida!
                            iIER = ERROR_UnrecognizedEOSInExternalFileForMixtureProperties
                            return

                    end select treatIPROPSEOS

                end if getEosFromHere

            !else if((sPvtSimCTMFileLine(1:20) == '<Fluid ExtraInfo CT>').AND.(.not.bReadOnlyINComp)) then lineType
            !   19/02/2021 - linha acima comentada e substituída pela abaixo.
            else if((sPvtSimCTMFileLine(1:16) == '<Fluid Extra CT>').AND.(.not.bReadOnlyINComp).AND.(.not.bReadEOSFromFluidIPROPSSection)) then lineType

                ! Ler da próxima linha a Equação de Estado:
                read(iPvtSimCTMFileUnit, '(A)', IOSTAT=iFileOperationsError) sPvtSimCTMFileLine

                ! Leitura deu erro?
                if(iFileOperationsError.GT.0) then
                    iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                    return
                end if

                ! Coluna por coluna...
                findEOS: do i = 1, len_trim(sPvtSimCTMFileLine)

                    endOfLine: if((i+5).EQ.len_trim(sPvtSimCTMFileLine)) then
                        iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                        return
                    end if endOfLine

                    lookForEOS: if((sPvtSimCTMFileLine(i:i+5) == 'EOS = ').OR. &
                                   (sPvtSimCTMFileLine(i:i+4) == 'EOS= ')) then

                        checkEOS: if((sPvtSimCTMFileLine(i+6:i+18) == 'PR78 Peneloux').OR. &
                                     (sPvtSimCTMFileLine(i+5:i+8) == 'PR78')) then
                            iThermodynamicModel = PENG_ROBINSON_78_PENELOUX

                            bUsePengRobinsonKij = .true.
                            bUseSRKKij = .false.

                            bUsePengRobinsonPeneloux = .true.
                            bUseSRKPeneloux = .false.

                            cycle readLoop

                        else if((sPvtSimCTMFileLine(i+5:i+7) == 'SRK').OR. &
                                (sPvtSimCTMFileLine(i+6:i+17) == 'SRK Peneloux')) then checkEOS
                            iThermodynamicModel = SRK_PENELOUX

                            bUsePengRobinsonKij = .false.
                            bUseSRKKij = .true.

                            bUsePengRobinsonPeneloux = .false.
                            bUseSRKPeneloux = .true.

                            cycle readLoop

                        end if checkEOS

                        ! Equação de Estado inválida!
                        iIER = ERROR_UnrecognizedEOSInExternalFileForMixtureProperties
                        return

                    end if lookForEOS

                end do findEOS

            end if lineType

        end do readLoop

        ! ------------------ Retornar os devidos coeficientes de "Peneloux":
        oTIndep_Peneloux(:) = 0.0d0
        if(bUsePengRobinsonPeneloux) then
            do i = 1, iNComp
                oTIndep_Peneloux(i) = oTIndep_Peneloux_PR(i)
            end do
        else if(bUseSRKPeneloux) then
            do i = 1, iNComp
                oTIndep_Peneloux(i) = oTIndep_Peneloux_SRK(i)
            end do
        end if

        ! ------------------ Retornar "kij" no formato adequado:
        iKijCTMVectorSize = (iNComp * iNComp - iNComp) / 2
        allocate(oKij_Read(iKijCTMVectorSize), STAT=iMemoryAllocationError)

        if(iMemoryAllocationError.NE.0) then
             ! Erro!
             iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
             return
        end if

        if(bUsePengRobinsonKij) then
            oKij_Read = oKij_PR_Read
        else if(bUseSRKKij) then
            oKij_Read = oKij_SRK_Read
        end if

        iKijLine = 2
        iKijColumn = 0

        kijVectorLoop: do i = 1, iKijCTMVectorSize

            iKijColumn = iKijColumn + 1

            if(iKijColumn.LT.iKijLine) then
                oKij(iKijLine, iKijColumn) = oKij_Read(i)
                oKij(iKijColumn, iKijLine) = oKij(iKijLine, iKijColumn)
            else
                oKij(iKijColumn, iKijColumn) = 0.0d0
                iKijLine = iKijLine + 1
                iKijColumn = 1
                oKij(iKijLine, iKijColumn) = oKij_Read(i)
                oKij(iKijColumn, iKijLine) = oKij(iKijLine, iKijColumn)
                cycle kijVectorLoop
            end if

        end do kijVectorLoop

            ! CORREÇÃO EM 25/11/2021
            ! A lógica acima passa direto pelo último elemento da matriz, que também é nulo!!
        oKij(iNComp, iNComp) = 0.0d0
        oKij(1, 1) = 0.0d0

        ! ------------------ Retornar "lij" no formato adequado:
        lijAssignOuterLoop: do i = 1, iNComp
            lijAssignInnerLoop: do j = 1, iNComp
                oLij(i, j) = 0.0d0
            end do lijAssignInnerLoop
        end do lijAssignOuterLoop

    end subroutine ReadMixtureComponentsFromPVTSIMFileCTM

    ! =============================================================
    ! =============================================================
    subroutine ReadSequenceOfComponentValuesFromNextCTMFileLines(iPvtSimCTMFileUnit, iNumberOfValuesToRead, dMultFactor, &
             oValuesObtained, iIER)

        ! OBJETIVO: Ler, das próximas linhas do arquivo CTM do PVTSIM, uma lista de valores
        !   correspondentes a dada propriedade dos componentes que se está importando.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer, intent(in) :: iPvtSimCTMFileUnit               ! Arquivo CTM do PVTSim
        integer, intent(in) :: iNumberOfValuesToRead            ! Número de valores a ler.
        real(c_double), intent(in) :: dMultFactor               ! Fator multiplicativo a aplicar sobre cada valor lido.
        real(c_double), dimension(iNumberOfValuesToRead), intent(out) :: oValuesObtained   ! Valores obtidos do arquivo
        integer(c_int), intent(out) :: iIER                     ! RESULTADO: Código de erros, conforme convenção.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: iFileOperationsError
        character(len=200) :: sGarbageLine
        integer :: iNumberOfLinesRead                           ! Contagem de linhas já lidas.
        integer :: i                                            ! Índice de laços "do".
        character(len=200) :: sPvtSimCTMFileLine
        integer :: iInitialValPosition, iFinalValPosition, iValuesRead
        real(c_double) :: dValueFromFile

        ! ------------ PROCEDIMENTOS:

        ! A primeira linha (próxima a ser lida) é lixo:
        read(iPvtSimCTMFileUnit, '(A)', IOSTAT=iFileOperationsError) sGarbageLine

        if(iFileOperationsError.NE.0) then
            ! Erro!
            iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
            return
        end if

        ! Inicializações:
        iNumberOfLinesRead = 0

        ! As próximas linhas têm que ser lidas continuamente, até que se consiga o número
        ! esperado de valores numéricos:
        iValuesRead = 0

        readLoop: do

            ! Contagem de linhas para evitar "loop" infinito:
            iNumberOfLinesRead = iNumberOfLinesRead + 1
            if(iNumberOfLinesRead.GT.250) then
                ! Erro!
                iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                return
            end if

            ! Ler próxima linha:
            read(iPvtSimCTMFileUnit, '(A)', IOSTAT=iFileOperationsError) sPvtSimCTMFileLine

            if(iFileOperationsError.NE.0) then
               ! Erro!
                iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                return
            end if

            ! Ler todos os valores que a linha contém
            iInitialValPosition = 1

            characByCharac: do i = 2, len_trim(sPvtSimCTMFileLine)

                ! É vírgula?
                testChar: if((sPvtSimCTMFileLine(i:i) == ',').OR.(i.EQ.len_trim(sPvtSimCTMFileLine))) then

                    ! Já pode ler mais um valor:
                    iFinalValPosition = i - 1
                    if(i.EQ.len_trim(sPvtSimCTMFileLine)) iFinalValPosition = i

                    read(sPvtSimCTMFileLine(iInitialValPosition:iFinalValPosition), &
                         * , IOSTAT=iFileOperationsError) dValueFromFile

                    if(iFileOperationsError.NE.0) then
                        ! Erro!
                        iIER = ERROR_ErrorWhileReadingExternalFileForMixtureProperties
                        return
                    end if

                    ! Leitura com sucesso!
                    iInitialValPosition = i + 1

                    oValuesObtained(iValuesRead + 1) = dValueFromFile * dMultFactor
                    iValuesRead = iValuesRead + 1
                    if(iValuesRead.EQ.iNumberOfValuesToRead) return

                end if testChar

            end do characByCharac

        end do readLoop

    end subroutine ReadSequenceOfComponentValuesFromNextCTMFileLines

end module PvtSimCTMFileImport
