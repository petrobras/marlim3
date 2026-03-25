module PvtSimWAXFileImport

    ! OBJETIVO: Conter as rotinas necessárias para possibilitar a leitura automática de arquivos "*.wax"
    !       no formato exportado pelo PVTSIM.

    use iso_c_binding   ! Para viabilizar a comunicação com C...
    use Constants

    implicit none

    contains

    ! =============================================================
    ! =============================================================
    subroutine GetPVTSIMWaxFileDimensions(sPVTSIMWaxFileName, iIER, iWaxComponentCount, iPressureCount, iTemperatureCount)

        ! OBJETIVO: Obter contagens necessárias para viabilizar a leitura automática de arquivos padrão "PVTSIM wax"
        !           (número de componentes, de pontos de pressão, etc)
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        character(len=*), intent(in) :: sPVTSIMWaxFileName
        integer(c_int), intent(out) :: iIER                             ! RESULTADO: Código de erros, conforme convenção.
        integer(c_int), intent(out) :: iWaxComponentCount
        integer(c_int), intent(out) :: iPressureCount
        integer(c_int), intent(out) :: iTemperatureCount

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer(c_int) :: iFileOperationsError
        character(len=400) :: sLine
        integer :: iStart, i
        real(c_double), allocatable, dimension(:) :: oMolecularWeightsOfWaxComponents
        real(c_double), allocatable, dimension(:) :: oLiquidDensitiesOfWaxComponents
        real(c_double), allocatable, dimension(:) :: oHeatsOfMeltingOfWaxComponents
        real(c_double), allocatable, dimension(:) :: oConcentrationOfWaxComponentsInFeed
        integer(c_int), dimension(2) :: oPAndCloudTCount

        ! ------------ CONSTANTES:
        integer, parameter :: iPvtSimWaxFileUnit = 2

        ! ------------ PROCEDIMENTOS:

        ! Abrir o arquivo:
        OPEN(UNIT=iPvtSimWaxFileUnit, FILE=sPVTSIMWaxFileName, STATUS='OLD', ACTION='READ', IOSTAT=iFileOperationsError)

        if (iFileOperationsError.NE.0) then
            ! Não foi possível abrir o arquivo!
            iIER = ERROR_FailedToOpenExternalFileForWaxParameters
            return
        end if

        ! Leitura do número de "wax components", com tratamento linha a linha:
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! "Name of table"
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! nome dado pelo usuário
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! "Number of Wax Components"

        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(sLine, '(I9)', iostat=iFileOperationsError) iWaxComponentCount

        ! Leitura das linhas até as próximas quantidades:
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! "Names of Wax Components"
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! Nomes das espécies químicas

        !sNamesLine = trim(adjustl(sLine))
        findAllNames: do
            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
            iStart = index(sLine, "Molecular Weights of Wax")
            if(iStart.ne.0) then
                ! Lemos a "Molecular Weights of Wax Components"
                exit findAllNames
            else
                ! Ainda estamos lendo os nomes:
                !sNamesLine = trim(adjustl(sNamesLine)) // trim(adjustl(sLine))
            end if
        end do findAllNames

        ! Leitura das massas moleculares
        allocate(oMolecularWeightsOfWaxComponents(iWaxComponentCount))
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oMolecularWeightsOfWaxComponents(i), i=1,iWaxComponentCount)

        ! Leitura das densidades líquidas
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! Liquid Densities (kg/m3) of Wax Components
        allocate(oLiquidDensitiesOfWaxComponents(iWaxComponentCount))
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oLiquidDensitiesOfWaxComponents(i), i=1,iWaxComponentCount)

        ! Leitura dos calores de fusão
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! Heat of Melting (J/kg) of Wax Components
        allocate(oHeatsOfMeltingOfWaxComponents(iWaxComponentCount))
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oHeatsOfMeltingOfWaxComponents(i), i=1,iWaxComponentCount)

        ! Leitura das concentrações globais
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine      ! !Concentration of Wax Components in Feed
        allocate(oConcentrationOfWaxComponentsInFeed(iWaxComponentCount))
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oConcentrationOfWaxComponentsInFeed(i), i=1,iWaxComponentCount)

        ! Leitura dos multiplicadores de viscosidade (caso presentes no arquivo)
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine          ! "!"?
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine

        !dOilWaxViscosityMultiplierD = -1.0d0
        !dOilWaxViscosityMultiplierE = -1.0d0
        !dOilWaxViscosityMultiplierF = -1.0d0
        iStart = index(sLine, "OIL_WAX_VISCOSITY_MULTIPLIERS")
        hasViscMultipliers: if(iStart.ne.0) then
            ! Multiplicadores presentes!
            !iStart = 33
            !iEnd = index(sLine(iStart:),",")
            !read(sLine(iStart:iStart+iEnd-1), *, iostat=iFileOperationsError) dOilWaxViscosityMultiplierD
            !iStart = iStart+iEnd+2
            !iEnd = index(sLine(iStart:),",")
            !read(sLine(iStart:iStart+iEnd-1), *, iostat=iFileOperationsError) dOilWaxViscosityMultiplierE
            !iStart = iStart+iEnd+2
            !iEnd = len_trim(sLine)
            !read(sLine(iStart:iStart+iEnd-1), *, iostat=iFileOperationsError) dOilWaxViscosityMultiplierF

            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        end if hasViscMultipliers

        ! Contagem de pontos de pressão e de "cloud point temperatures":
        read(sLine, *, iostat=iFileOperationsError) (oPAndCloudTCount(i), i = 1,2)  ! PAREI AQUI
        iPressureCount = oPAndCloudTCount(1)
        iTemperatureCount =oPAndCloudTCount(2)

    end subroutine GetPVTSIMWaxFileDimensions

    ! =============================================================
    ! =============================================================
    subroutine ReadWaxCalculationParametersFromPVTSIMFileWax(sPVTSIMWaxFileName, iIER, oPressurePoints_output, &
                oCloudPointTemperatures_output, oStructurePressures_output, oStructureTemperatures_output, &
                oStructureDensities_output, oStructureLiqMW_output, oStructureCPWax_output, oStructureThermCond_output, &
                oStructureWaxConcs_output, oMolecularWeightsOfWaxComponents_output, oLiquidDensitiesOfWaxComponents_output, &
                dOilWaxViscosityMultiplierD_output, dOilWaxViscosityMultiplierE_output, dOilWaxViscosityMultiplierF_output)

        ! OBJETIVO: Ler automaticamente, a partir de um arquivo "*.wax" com o padrão exportado pelo PVTSIM,
        !   as informações necessárias para realizar cálculos de deposição de parafina em simulações de escoamento
        !   multifásico.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        character(len=*), intent(in) :: sPVTSIMWaxFileName

        integer(c_int), intent(out) :: iIER                             ! RESULTADO: Código de erros, conforme convenção.
        real(c_double), dimension(:), intent(out) :: oPressurePoints_output
        real(c_double), dimension(:), intent(out) :: oCloudPointTemperatures_output
        real(c_double), dimension(:), intent(out) :: oStructurePressures_output
        real(c_double), dimension(:, :), intent(out) :: oStructureTemperatures_output
        real(c_double), dimension(:, :), intent(out) :: oStructureDensities_output
        real(c_double), dimension(:, :), intent(out) :: oStructureLiqMW_output
        real(c_double), dimension(:, :), intent(out) :: oStructureCPWax_output
        real(c_double), dimension(:, :), intent(out) :: oStructureThermCond_output
        real(c_double), dimension(:, :, :), intent(out) :: oStructureWaxConcs_output
        real(c_double), dimension(:), intent(out) :: oMolecularWeightsOfWaxComponents_output
        real(c_double), dimension(:), intent(out) :: oLiquidDensitiesOfWaxComponents_output
        real(c_double), intent(out) :: dOilWaxViscosityMultiplierD_output
        real(c_double), intent(out) :: dOilWaxViscosityMultiplierE_output
        real(c_double), intent(out) :: dOilWaxViscosityMultiplierF_output

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer(c_int) :: iFileOperationsError, i, j, k
        integer(c_int) :: iWaxComponentCount                            ! DO ARQUIVO: "Number of Wax Components" apresentado pelo arquivo.
        character(len=400) :: sLine
        character(len=30), allocatable :: oNamesOfWaxComponents(:)     ! DO ARQUIVO: "Names of Wax Components" apresentados pelo arquivo.
        real(c_double), allocatable, dimension(:) :: oMolecularWeightsOfWaxComponents   ! DO ARQUIVO: "Molecular Weights of Wax Components" apresentados pelo arquivo.
        real(c_double), allocatable, dimension(:) :: oLiquidDensitiesOfWaxComponents    ! DO ARQUIVO: "Liquid densities of wax components" apresentados pelo arquivo.
        real(c_double), allocatable, dimension(:) :: oHeatsOfMeltingOfWaxComponents     ! DO ARQUIVO: "Heats of melting of wax components" apresentados pelo arquivo.
        real(c_double), allocatable, dimension(:) :: oConcentrationOfWaxComponentsInFeed    ! DO ARQUIVO: "Concentration of wax components in feed" apresentados pelo arquivo.
        real(c_double) :: dOilWaxViscosityMultiplierD, dOilWaxViscosityMultiplierE, dOilWaxViscosityMultiplierF
        integer :: iStart, iEnd
        integer(c_int), dimension(2) :: oPAndCloudTCount
        integer(c_int) :: iPressureCount, iTemperatureCount
        character(len=1000) :: sNamesLine
        real(c_double), allocatable, dimension(:) :: oPressurePoints, oCloudPointTemperatures
        real(c_double), allocatable, dimension(:) :: oStructurePressures
        real(c_double), allocatable, dimension(:) :: oTSequenceValues
        real(c_double), allocatable, dimension(:, :) :: oStructureTemperatures, oStructureDensities, oStructureGasMW, oStructureLiqMW, &
            oStructureWaxMW, oStructureHWax, oStructureCPWax, oStructureThermCond
        real(c_double), allocatable, dimension(:, :, :) :: oStructureWaxConcs

        ! ------------ CONSTANTES:
        integer, parameter :: iPvtSimWaxFileUnit = 2

        ! ------------ PROCEDIMENTOS:

        ! Abrir o arquivo:
        OPEN(UNIT=iPvtSimWaxFileUnit, FILE=sPVTSIMWaxFileName, STATUS='OLD', ACTION='READ', IOSTAT=iFileOperationsError)

        if (iFileOperationsError.NE.0) then
            ! Não foi possível abrir o arquivo!
            iIER = ERROR_FailedToOpenExternalFileForWaxParameters
            return
        end if

        ! Leitura do número de "wax components", com tratamento linha a linha:
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! "Name of table"
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! nome dado pelo usuário
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! "Number of Wax Components"

        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(sLine, '(I9)', iostat=iFileOperationsError) iWaxComponentCount

        ! Dimensionar vetores com a quantidade lida do arquivo:
        allocate(oNamesOfWaxComponents(iWaxComponentCount))
        allocate(oMolecularWeightsOfWaxComponents(iWaxComponentCount))
        allocate(oLiquidDensitiesOfWaxComponents(iWaxComponentCount))
        allocate(oHeatsOfMeltingOfWaxComponents(iWaxComponentCount))
        allocate(oConcentrationOfWaxComponentsInFeed(iWaxComponentCount))

        ! Leitura dos nomes dos componentes
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! "Names of Wax Components"

        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine

        sNamesLine = trim(adjustl(sLine))
        findAllNames: do
            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
            iStart = index(sLine, "Molecular Weights of Wax")
            if(iStart.ne.0) then
                ! Lemos a "Molecular Weights of Wax Components"
                exit findAllNames
            else
                ! Ainda estamos lendo os nomes:
                sNamesLine = trim(adjustl(sNamesLine)) // trim(adjustl(sLine))
            end if
        end do findAllNames

        call split_and_assign_names(oNamesOfWaxComponents, sNamesLine, iWaxComponentCount)

        ! Leitura das massas moleculares
        !read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oMolecularWeightsOfWaxComponents(i), i=1,iWaxComponentCount)

        ! Leitura das densidades líquidas
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oLiquidDensitiesOfWaxComponents(i), i=1,iWaxComponentCount)

        ! Leitura dos calores de fusão
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oHeatsOfMeltingOfWaxComponents(i), i=1,iWaxComponentCount)

        ! Leitura das concentrações globais
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oConcentrationOfWaxComponentsInFeed(i), i=1,iWaxComponentCount)

        ! Leitura dos multiplicadores de viscosidade (caso presentes no arquivo)
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine

        dOilWaxViscosityMultiplierD = 1.0d0
        dOilWaxViscosityMultiplierE = 1.0d0
        dOilWaxViscosityMultiplierF = 1.0d0     ! OBS: Valor unitário para estes multiplicadores caso não venham no "*.wax".
        iStart = index(sLine, "OIL_WAX_VISCOSITY_MULTIPLIERS")
        hasViscMultipliers: if(iStart.ne.0) then
            ! Multiplicadores presentes!
            iStart = 33
            iEnd = index(sLine(iStart:),",")
            read(sLine(iStart:iStart+iEnd-1), *, iostat=iFileOperationsError) dOilWaxViscosityMultiplierD
            iStart = iStart+iEnd+2
            iEnd = index(sLine(iStart:),",")
            read(sLine(iStart:iStart+iEnd-1), *, iostat=iFileOperationsError) dOilWaxViscosityMultiplierE
            iStart = iStart+iEnd+2
            iEnd = len_trim(sLine)
            read(sLine(iStart:iStart+iEnd-1), *, iostat=iFileOperationsError) dOilWaxViscosityMultiplierF

            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine
        end if hasViscMultipliers

        ! Contagem de pontos de pressão e de "cloud point temperatures":
        read(sLine, *, iostat=iFileOperationsError) (oPAndCloudTCount(i), i = 1,2)
        iPressureCount = oPAndCloudTCount(1)
        iTemperatureCount =oPAndCloudTCount(2)

        ! Ler as pressões na contagem indicada:
        allocate(oPressurePoints(iPressureCount))
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! P Points (Pa)
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oPressurePoints(i), i=1,iPressureCount)

        ! Ler as "cloud point temperatures" na contagem indicada:
        allocate(oCloudPointTemperatures(iPressureCount))                   ! É nesta contagem mesmo. Não está errado.
        read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! Cloud Point Temperatures (C)
        read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oCloudPointTemperatures(i), i=1,iPressureCount)

        ! Leitura até aqui validada com dois arquivos.

        ! Ler as estruturas das diversas pressões:
        allocate(oStructurePressures(iPressureCount))
        allocate(oTSequenceValues(iWaxComponentCount+8))
        allocate(oStructureTemperatures(iPressureCount, iTemperatureCount))
        allocate(oStructureWaxConcs(iPressureCount, iTemperatureCount, iWaxComponentCount))
        allocate(oStructureDensities(iPressureCount, iTemperatureCount))
        allocate(oStructureGasMW(iPressureCount, iTemperatureCount))
        allocate(oStructureLiqMW(iPressureCount, iTemperatureCount))
        allocate(oStructureWaxMW(iPressureCount, iTemperatureCount))
        allocate(oStructureHWax(iPressureCount, iTemperatureCount))
        allocate(oStructureCPWax(iPressureCount, iTemperatureCount))
        allocate(oStructureThermCond(iPressureCount, iTemperatureCount))

        readPStructs: do i = 1, iPressureCount

            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! Pressure Point No.  i, P in Pa

            read(iPvtSimWaxFileUnit, * , iostat=iFileOperationsError) oStructurePressures(i)

            read(iPvtSimWaxFileUnit, '(A)', iostat=iFileOperationsError) sLine  ! !T (C), Wax Concs (mol/mol), Dens (kg/m3), Gas Mw, Liq Mw, Wax Mw, Hwax (J/kg), Cpwax (J/kg C), Therm Cond (W/m K)

            readTStructs: do j = 1, iTemperatureCount

                read(iPvtSimWaxFileUnit, *, iostat=iFileOperationsError) (oTSequenceValues(k), k=1,(iWaxComponentCount+8))

                oStructureTemperatures(i, j) = oTSequenceValues(1)
                oStructureWaxConcs(i, j, 1:iWaxComponentCount) = oTSequenceValues(2:1+iWaxComponentCount)
                oStructureDensities(i, j) = oTSequenceValues(iWaxComponentCount+2)
                oStructureGasMW(i, j) = oTSequenceValues(iWaxComponentCount+3)
                oStructureLiqMW(i, j) = oTSequenceValues(iWaxComponentCount+4)
                oStructureWaxMW(i, j) = oTSequenceValues(iWaxComponentCount+5)
                oStructureHWax(i, j) = oTSequenceValues(iWaxComponentCount+6)
                oStructureCPWax(i, j) = oTSequenceValues(iWaxComponentCount+7)
                oStructureThermCond(i, j) = oTSequenceValues(iWaxComponentCount+8)

            end do readTStructs

        end do readPStructs

    ! Fechar o arquivo
    close(iPvtSimWaxFileUnit)

    ! Retornar parâmetros de saída:
    oPressurePoints_output = oPressurePoints
    oCloudPointTemperatures_output = oCloudPointTemperatures
    oStructurePressures_output = oStructurePressures
    oStructureTemperatures_output = oStructureTemperatures
    oStructureDensities_output = oStructureDensities
    oStructureLiqMW_output = oStructureLiqMW
    oStructureCPWax_output = oStructureCPWax
    oStructureThermCond_output = oStructureThermCond
    oStructureWaxConcs_output = oStructureWaxConcs
    oMolecularWeightsOfWaxComponents_output = oMolecularWeightsOfWaxComponents
    oLiquidDensitiesOfWaxComponents_output = oLiquidDensitiesOfWaxComponents
    dOilWaxViscosityMultiplierD_output = dOilWaxViscosityMultiplierD
    dOilWaxViscosityMultiplierE_output = dOilWaxViscosityMultiplierE
    dOilWaxViscosityMultiplierF_output = dOilWaxViscosityMultiplierF


    ! +++++++++++ escrita opcional na tela ++++++++++++++++++++++++
    writeOnScreen: if(.false.) then

        write(*, '(/, A)') '----------> DADOS RECUPERADOS DO ARQUIVO WAX DO PVTSIM:'

        write(*, '(/, A)', advance='no') 'Nomes dos componentes:                         '
        do i = 1, iWaxComponentCount
            write(*, '(A, "   ||   ")', advance='no') trim(adjustl(oNamesOfWaxComponents(i)))
        end do

        write(*, '(//, A)', advance='no') 'Massas molares dos componentes:               '
        do i = 1, iWaxComponentCount
            write(*, '(F7.2, "   ||   ")', advance='no') oMolecularWeightsOfWaxComponents(i)
        end do

        write(*, '(//, A)', advance='no') 'Massas específicas de líquido dos componentes:'
        do i = 1, iWaxComponentCount
            write(*, '(F7.2, "   ||   ")', advance='no') oLiquidDensitiesOfWaxComponents(i)
        end do

        write(*, '(//, A)', advance='no') 'Calores de fusão dos componentes:             '
        do i = 1, iWaxComponentCount
            write(*, '(F10.2, "   ||   ")', advance='no') oHeatsOfMeltingOfWaxComponents(i)
        end do

        write(*, '(//, A)', advance='no') 'Concentrações originais dos componentes:      '
        do i = 1, iWaxComponentCount
            write(*, '(E12.6, "   ||   ")', advance='no') oConcentrationOfWaxComponentsInFeed(i)
        end do

        write(*, '(//, A)', advance='no') 'Multiplicadores da viscosidade:               '
        write(*, '("D = ", F8.5, "   ||   ")', advance='no') dOilWaxViscosityMultiplierD
        write(*, '("E = ", F8.5, "   ||   ")', advance='no') dOilWaxViscosityMultiplierE
        write(*, '("F = ", F8.5, "   ||   ")') dOilWaxViscosityMultiplierF

        write(*, '(/, "P Count = ", I5, "   ||   ", "T Count = ", I5)') iPressureCount, iTemperatureCount

        write(*, '(//, A)') 'Pontos de pressão:'
        write(*, '(10(F11.1, " || "), /)') (oPressurePoints(i), i=1,iPressureCount)

        write(*, '(//, A)') 'Pontos de temperatura de névoa:'
        write(*, '(10(F7.2, " || "), /)') (oCloudPointTemperatures(i), i=1,iPressureCount)

        shouldWritePStructs: if(.true.) then

            writePStructs: do i = 1, iPressureCount

                ! Para economizar escrita:
                if(i.NE.iPressureCount) cycle writePStructs

                write(*, '(//, "------------------------> Estrutura de pressão ", I4, " || P = ", F11.1)') i, oStructurePressures(i)

                writeTStructs: do j = 1, iTemperatureCount
                    write(*, '(/, "--------> Temperatura ", I4, " = ", F7.2)') j, oStructureTemperatures(i, j)
                    write(*, '(/, A)', advance = 'no') 'Wax concs:  '
                    write(*, '(10(E12.6, " || "), /)') (oStructureWaxConcs(i, j, k), k=1,iWaxComponentCount)
                    write(*, '(/, "Dens = ", F7.2, " || ")', advance='no') oStructureDensities(i, j)
                    write(*, '("GasMW = ", F7.2, " || ")', advance='no') oStructureGasMW(i, j)
                    write(*, '("LiqMW = ", F7.2, " || ")', advance='no') oStructureLiqMW(i, j)
                    write(*, '("WaxMW = ", F7.2, " || ")', advance='no') oStructureWaxMW(i, j)
                    write(*, '("HWax = ", F17.7, " || ")', advance='no') oStructureHWax(i, j)
                    write(*, '("CPWax = ", F13.7, " || ")', advance='no') oStructureCPWax(i, j)
                    write(*, '("Therm Cond = ", F10.6, " || ")') oStructureThermCond(i, j)
                end do writeTStructs

            end do writePStructs

        end if shouldWritePStructs

    end if writeOnScreen
    ! +++++++++++ fim da escrita opcional na tela +++++++++++++++++

    ! ----------------------------------------
  contains  ! Subrotina "ReadWaxCalculationParametersFromPVTSIMFileWax"

    ! Subrotina interna para ler os "Names of Wax Components"
    subroutine split_and_assign_names(oNamesArray, sLine, iCount)
      implicit none
      character(len=30), allocatable, intent(inout) :: oNamesArray(:)   ! Vetor que armazenará os nomes separados dos componentes
      character(len=400), intent(in) :: sLine   ! Linha do arquivo, contendo os nomes separados por vírgula, sem espaços
      integer(c_int), intent(in) :: iCount      ! Quantidade de componentes (e de nomes a obter)
      character(len=400) :: sTmp
      integer :: i, start, end

      start = 2
      do i = 1, iCount
        !end = index(sLine(start:), ',')                     ! Procurando a posição da primeira vírgula após a posição "start" (posição na linha = end + start - 1)
        end = index(sLine(start:), "','")
        if (end.EQ.0) end = len_trim(sLine) - start + 1     ! Se "index" retornou zero, é porque não tem mais vírgula (0 = "no match").
        sTmp = sLine(start:start+end-2)                     ! Delimitando o próximo nome na linha, tirando a vírgula e a aspa
        oNamesArray(i) = trim(adjustl(sTmp))                ! Armazenando sem espaços em branco
        start = start + end + 2                             ! Avançando
      end do
    end subroutine split_and_assign_names

  end subroutine ReadWaxCalculationParametersFromPVTSIMFileWax

    ! =============================================================
    ! =============================================================
    subroutine InterpolatePVTSIMWaxCalcParameters(iWaxComponentCount, iPressureCount, iTemperatureCount, oPressurePoints, oCloudPointTemperatures, oStructurePressures, &
            oStructureTemperatures, oStructureCPWax, oStructureThermCond, oStructureDensities, oStructureLiqMW, oStructureWaxConcs, &
            dPressure, dTemperature, iIER, dCloudPointT, dInterpolatedCPWax, dInterpolatedThermCond, dInterpolatedDens, dInterpolatedLiqMW, &
            oInterpolatedWaxConcsTDeriv, oInterpolatedWaxConcs)

        ! OBJETIVO: Fazer interpolações em P e T de determinados parâmetros de cálculos de
        !   deposição de parafinas obtidos de arquivo "*.wax" do PVTSIM.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer(c_int), intent(in) :: iWaxComponentCount                                 ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        integer(c_int), intent(in) :: iPressureCount                                     ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        integer(c_int), intent(in) :: iTemperatureCount                                  ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount), intent(in) :: oPressurePoints         ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount), intent(in) :: oCloudPointTemperatures ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount), intent(in) :: oStructurePressures     ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount, iTemperatureCount), intent(in) :: oStructureTemperatures ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount, iTemperatureCount), intent(in) :: oStructureCPWax ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount, iTemperatureCount), intent(in) :: oStructureThermCond ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount, iTemperatureCount), intent(in) :: oStructureDensities ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount, iTemperatureCount), intent(in) :: oStructureLiqMW ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(iPressureCount, iTemperatureCount, iWaxComponentCount), intent(in) :: oStructureWaxConcs ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).

        real(c_double), value, intent(in) :: dPressure      ! Pressão local na qual deseja-se fazer as interpolações.
        real(c_double), value, intent(in) :: dTemperature   ! Temperatura local (interface) na qual deseja-se fazer as interpolações.

        integer(c_int), intent(out) :: iIER                     ! RESULTADO: Código de erros, conforme convenção.
        real(c_double), intent(out) :: dCloudPointT             ! RESULTADO: TIAC interpolada à pressão fornecida.
        real(c_double), intent(out) :: dInterpolatedCPWax       ! RESULTADO: "CPWax" interpolado à P e T de interesse
        real(c_double), intent(out) :: dInterpolatedThermCond   ! RESULTADO: "Therm Cond" interpolado à P e T de interesse
        real(c_double), intent(out) :: dInterpolatedDens        ! RESULTADO: "Dens" interpolado à P e T de interesse
        real(c_double), intent(out) :: dInterpolatedLiqMW       ! RESULTADO: "Liq MW" interpolado à P e T de interesse
        real(c_double), dimension(iWaxComponentCount), intent(out) :: oInterpolatedWaxConcsTDeriv    ! RESULTADO: Derivadas de concentrações com temperatura à P e T de interesse.
        real(c_double), dimension(iWaxComponentCount), intent(out) :: oInterpolatedWaxConcs  ! RESULTADO: Concentrações interpoladas na pressão e na temperatura.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: i, iPFloorIndex, k, j, l
        logical :: bFoundPOrTLimits
        real(c_double), dimension(2, 2) :: oTemperatureMiniTable  ! Temperaturas correspondentes às respectivas posições nas minitabelas.
        real(c_double), dimension(2, 2) :: oCPWaxMiniTable        ! CPWax nas P e T (respectivamente) limítrofes.
        real(c_double), dimension(2, 2) :: oThermCondMiniTable    ! ThermCond nas P e T (respectivamente) limítrofes.
        real(c_double), dimension(2, 2) :: oDensMiniTable         ! Dens nas P e T (respectivamente) limítrofes.
        real(c_double), dimension(2, 2) :: oLiqMWMiniTable        ! Liq MW nas P e T (respectivamente) limítrofes.
        real(c_double), dimension(iWaxComponentCount, 2, 2) :: oWaxConcsMiniTable ! "WaxConcs" dos componentes nas P e T (respectivamente) limítrofes.

        real(c_double), dimension(2) :: oLimitP                   ! Valores de pressão da tabela imediatamente superior e inferior à pressão de interesse, respectivamente
        real(c_double), dimension(2) :: oCPWaxInterpInT           ! Valores de CPWax interpolados na temperatura de interesse para as duas pressões limítrofes.
        real(c_double), dimension(2) :: oThermCondInterpInT       ! Valores de "Therm Cond" interpolados na temperatura de interesse para as duas pressões limítrofes.
        real(c_double), dimension(2) :: oDensInterpInT            ! Valores de "Dens" interpolados na temperatura de interesse para as duas pressões limítrofes.
        real(c_double), dimension(2) :: oLiqMWInterpInT           ! Valores de "Liq Mw" interpolados na temperatura de interesse para as duas pressões limítrofes.
        real(c_double), dimension(iWaxComponentCount, 2) :: oWaxConcsTDeriv ! Derivadas dos "WaxConcs" com a temperatura para as duas pressões limítrofes.
        real(c_double), dimension(iWaxComponentCount, 2) :: oWaxConcInterpInT   ! Valores de "Wax Concs" interpolados na temperatura de interesse para as duas pressões limítrofes.

        ! ------------ CONSTANTES:

        ! ------------ PROCEDIMENTOS:

        ! Obter, por interpolação, a TIAC à pressão especificada:
        call InterpolateInVectors(iPressureCount, oPressurePoints, oCloudPointTemperatures, dPressure, iIER, dCloudPointT)

        if (iIER.ne.ERROR_EverythingOK) return

        ! Percorrendo as curvas, localizar P limítrofe:
        bFoundPOrTLimits = .false.

        !findLimitP: do i = 1, iPressureCount                                   ! Linha original comentada (precisou ser adaptada abaixo).
        findLimitP: do i = 1, (iPressureCount-1)

            !foundLimitP: if(dPressure.ge.oStructurePressures(i)) then          ! Linha original comentada (contém "bug").
            foundLimitP: if(dPressure.lt.oStructurePressures(i+1)) then
                iPFloorIndex = i
                bFoundPOrTLimits = (i.lt.iPressureCount)
                bFoundPOrTLimits = bFoundPOrTLimits.and.(dPressure.ge.oStructurePressures(i))
                exit findLimitP
            end if foundLimitP

        end do findLimitP

        checkIfHasPInterval: if(.not.bFoundPOrTLimits) then
            iIER = ERROR_WaxInterpolationFoundNoInterval
            return
        end if checkIfHasPInterval

        ! Armazenar pressões limítrofes:
        oLimitP(1) = oStructurePressures(iPFloorIndex)
        oLimitP(2) = oStructurePressures(iPFloorIndex+1)

        ! Obter "mini-tabelas" de concentrações, CPWax...
        getMiniTables: do k = 0, 1

            ! "k = 0, 1", como se verá abaixo, será referente, respectivamente, às
            !       pressões imediatamente inferior e superior à pressão de interesse.
            bFoundPOrTLimits = .false.

            ! Localizar "T" limítrofe:
            !findLimitT: do j = 1, iTemperatureCount            ! Linha original comentada (precisou de adaptação abaixo).
            findLimitT: do j = 1, (iTemperatureCount-1)

                !foundLimitT: if(dTemperature.ge.oStructureTemperatures(iPFloorIndex+k, j)) then        ! Linha original comentada (contém "bug")
                foundLimitT: if(dTemperature.lt.oStructureTemperatures(iPFloorIndex+k, j+1)) then

                    bFoundPOrTLimits = (j.lt.iTemperatureCount)
                    bFoundPOrTLimits = bFoundPOrTLimits.and.(dTemperature.ge.oStructureTemperatures(iPFloorIndex+k, j))

                    canGetMiniTable: if(.not.bFoundPOrTLimits) then
                        ! Não vai dar para interpolar porque não foi possível delimitar um intervalo de temperaturas:
                        exit getMiniTables
                    else canGetMiniTable

                        ! Obter as "tabelinhas" necessárias para fazer posteriormente as interpolações:
                        oTemperatureMiniTable(k+1, 1) = oStructureTemperatures(iPFloorIndex+k, j)
                        oTemperatureMiniTable(k+1, 2) = oStructureTemperatures(iPFloorIndex+k, j+1)

                        oCPWaxMiniTable(k+1, 1) = oStructureCPWax(iPFloorIndex+k, j)
                        oCPWaxMiniTable(k+1, 2) = oStructureCPWax(iPFloorIndex+k, j+1)

                        oThermCondMiniTable(k+1, 1) = oStructureThermCond(iPFloorIndex+k, j)
                        oThermCondMiniTable(k+1, 2) = oStructureThermCond(iPFloorIndex+k, j+1)

                        oDensMiniTable(k+1, 1) = oStructureDensities(iPFloorIndex+k, j)
                        oDensMiniTable(k+1, 2) = oStructureDensities(iPFloorIndex+k, j+1)

                        oLiqMWMiniTable(k+1, 1) = oStructureLiqMW(iPFloorIndex+k, j)
                        oLiqMWMiniTable(k+1, 2) = oStructureLiqMW(iPFloorIndex+k, j+1)

                        compLoop: do l = 1, iWaxComponentCount
                            oWaxConcsMiniTable(l, k+1, 1) = oStructureWaxConcs(iPFloorIndex+k, j, l)
                            oWaxConcsMiniTable(l, k+1, 2) = oStructureWaxConcs(iPFloorIndex+k, j+1, l)
                        end do compLoop

                        ! Montadas as tabelinhas, deixar este "loop":
                        exit findLimitT

                    end if canGetMiniTable

                end if foundLimitT

            end do findLimitT

        end do getMiniTables

        checkIfHasTInterval: if(.not.bFoundPOrTLimits) then
            iIER = ERROR_WaxInterpolationFoundNoInterval
            return
        end if checkIfHasTInterval

        ! Fazer interpolações necessárias na temperatura:
        forTemp: do k = 0, 1

            ! Cpwax:
            call InterpolateInVectors(2, oTemperatureMiniTable(k+1, :), oCPWaxMiniTable(k+1, :), dTemperature, iIER, oCPWaxInterpInT(k+1))

            if (iIER.ne.ERROR_EverythingOK) return

            ! Therm Cond:
            call InterpolateInVectors(2, oTemperatureMiniTable(k+1, :), oThermCondMiniTable(k+1, :), dTemperature, iIER, oThermCondInterpInT(k+1))

            if (iIER.ne.ERROR_EverythingOK) return

            ! Dens:
            call InterpolateInVectors(2, oTemperatureMiniTable(k+1, :), oDensMiniTable(k+1, :), dTemperature, iIER, oDensInterpInT(k+1))

            if (iIER.ne.ERROR_EverythingOK) return

            ! Liq Mw:
            call InterpolateInVectors(2, oTemperatureMiniTable(k+1, :), oLiqMWMiniTable(k+1, :), dTemperature, iIER, oLiqMWInterpInT(k+1))

            if (iIER.ne.ERROR_EverythingOK) return

            ! Curvas de solubilidade:
            compLoopT: do l = 1, iWaxComponentCount
                oWaxConcsTDeriv(l, k+1) = (oWaxConcsMiniTable(l, k+1, 2) - oWaxConcsMiniTable(l, k+1, 1)) / &
                                          (oTemperatureMiniTable(k+1, 2) - oTemperatureMiniTable(k+1, 1))
            end do compLoopT

            ! Concentrações molares:
            compLoopT2: do l = 1, iWaxComponentCount

                call InterpolateInVectors(2, oTemperatureMiniTable(k+1, :), oWaxConcsMiniTable(l, k+1, :), dTemperature, iIER, oWaxConcInterpInT(l, k+1))

                if (iIER.ne.ERROR_EverythingOK) return

            end do compLoopT2

        end do forTemp

        ! Fazer interpolações necessárias na pressão:

        ! Cpwax:
        call InterpolateInVectors(2, oLimitP, oCPWaxInterpInT, dPressure, iIER, dInterpolatedCPWax)

        if (iIER.ne.ERROR_EverythingOK) return

        ! Therm Cond:
        call InterpolateInVectors(2, oLimitP, oThermCondInterpInT, dPressure, iIER, dInterpolatedThermCond)

        if (iIER.ne.ERROR_EverythingOK) return

        ! Dens:
        call InterpolateInVectors(2, oLimitP, oDensInterpInT, dPressure, iIER, dInterpolatedDens)

        if (iIER.ne.ERROR_EverythingOK) return

        ! Liq Mw:
        call InterpolateInVectors(2, oLimitP, oLiqMWInterpInT, dPressure, iIER, dInterpolatedLiqMW)

        if (iIER.ne.ERROR_EverythingOK) return

        ! Derivadas de "WaxConcs" com a temperatura:
        compLoopP: do l = 1, iWaxComponentCount

            call InterpolateInVectors(2, oLimitP, oWaxConcsTDeriv(l, :), dPressure, iIER, oInterpolatedWaxConcsTDeriv(l))

            if (iIER.ne.ERROR_EverythingOK) return

        end do compLoopP

        ! "Wax Concs":
        compLoopP2: do l = 1, iWaxComponentCount

            call InterpolateInVectors(2, oLimitP, oWaxConcInterpInT(l, :), dPressure, iIER, oInterpolatedWaxConcs(l))

            if (iIER.ne.ERROR_EverythingOK) return

        end do compLoopP2

        ! +++++++++++ escrita opcional na tela ++++++++++++++++++++++++
        writeOnScreen: if(.false.) then

            write(*, '(/, A)') '----------> ROTINA FORTRAN DE INTERPOLACAO WAX:'

            write(*, '(/, "P Recebida = ", F12.2, "   ||   ", "T Recebida = ", F6.2)') dPressure, dTemperature

            write(*, '(/, "TIAC interpolada = ", F6.2)') dCloudPointT

            write(*, '(/, "P Inf = ", F12.2, "   ||   ", "P Sup = ", F12.2)') oLimitP(1), oLimitP(2)

            write(*, '(/, A)') 'Tabela de temperaturas:'
            do k = 1, 2
                write(*, '("P", I1, 2X, "Val Inf = ", F6.2, 2X, "Val Sup = ", F6.2)') k, oTemperatureMiniTable(k, 1), oTemperatureMiniTable(k, 2)
            end do

            write(*, '(/, A)') 'Tabela de CPWax:'
            do k = 1, 2
                write(*, '("P", I1, 2X, "Val Inf = ", F9.3, 2X, "Val Sup = ", F9.3)') k, oCPWaxMiniTable(k, 1), oCPWaxMiniTable(k, 2)
            end do
            write(*, '("Interp em T: T1 = ", F9.3, 2X, "T2 = ", F9.3)') oCPWaxInterpInT(1), oCPWaxInterpInT(2)

            write(*, '(/, A)') 'Tabela de Therm Cond:'
            do k = 1, 2
                write(*, '("P", I1, 2X, "Val Inf = ", F9.5, 2X, "Val Sup = ", F9.5)') k, oThermCondMiniTable(k, 1), oThermCondMiniTable(k, 2)
            end do

            write(*, '(/, A)') 'Tabela de Dens:'
            do k = 1, 2
                write(*, '("P", I1, 2X, "Val Inf = ", F9.3, 2X, "Val Sup = ", F9.3)') k, oDensMiniTable(k, 1), oDensMiniTable(k, 2)
            end do

            write(*, '(/, A)') 'Tabela de Liq MW:'
            do k = 1, 2
                write(*, '("P", I1, 2X, "Val Inf = ", F9.3, 2X, "Val Sup = ", F9.3)') k, oLiqMWMiniTable(k, 1), oLiqMWMiniTable(k, 2)
            end do

            do l = 1, iWaxComponentCount
                write(*, '(/, A, I2, ":")') 'Tabela de Wax Conc ', l
                do k = 1, 2
                    write(*, '("P", I1, 2X, "Val Inf = ", E11.6, 2X, "Val Sup = ", E11.6)') k, oWaxConcsMiniTable(l, k, 1), oWaxConcsMiniTable(l, k, 2)
                end do
            end do

        end if writeOnScreen
        ! +++++++++++ fim da escrita opcional na tela ++++++++++++++++++++++++

    end subroutine InterpolatePVTSIMWaxCalcParameters

    ! =============================================================
    ! =============================================================
    subroutine InterpolateInVectors(iSizeOfVectors, oInputX, oInputY, dInterestX, iIER, dInterpolatedValue)

        ! OBJETIVO: Realizar interpolação linear dentro de dois vetores "x" e "y".
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        integer, intent(in) :: iSizeOfVectors                               ! Dimensão dos vetores fornecidos.
        real(c_double), dimension(iSizeOfVectors), intent(in) :: oInputX    ! Vetor "abcissa" de entrada
        real(c_double), dimension(iSizeOfVectors), intent(in) :: oInputY    ! Vetor "ordenada" de entrada
        real(c_double), intent(in) :: dInterestX                            ! Abcissa de interesse (para a qual deseja-se interpolar)

        integer(c_int), intent(out) :: iIER                                 ! RESULTADO: Código de erros, conforme convenção.
        real(c_double), intent(out) :: dInterpolatedValue                   ! RESULTADO: Valor interpolado na posição desejada

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        integer :: i, iFloorIndex
        logical :: bFoundInX
        real(c_double) :: dA, dB, dX

        ! ------------ PROCEDIMENTOS:

        ! Localizar as abcissas imediatamente inferior e superior:
        bFoundInX = .false.

        !findInX: do i = 1, iSizeOfVectors          ! Linha original comentada (precisou da adaptação abaixo)
        findInX: do i = 1, (iSizeOfVectors-1)

            !checkIfFoundInX: if(dInterestX.ge.oInputX(i)) then     ! Linha original comentada (contém bug).
            checkIfFoundInX: if(dInterestX.lt.oInputX(i+1)) then
                iFloorIndex = i
                bFoundInX = (iFloorIndex.lt.iSizeOfVectors)
                bFoundInX = bFoundInX.and.(dInterestX.ge.oInputX(i))
                exit findInX
            end if checkIfFoundInX

        end do findInX

        checkIfHasInterval: if(.not.bFoundInX) then
            iIER = ERROR_WaxInterpolationFoundNoInterval
            return
        end if checkIfHasInterval

        ! Fazer a interpolação:
        dA = (oInputY(iFloorIndex+1) - oInputY(iFloorIndex)) / (oInputX(iFloorIndex+1) - oInputX(iFloorIndex))  ! Coeficiente angular
        dB = oInputY(iFloorIndex)                                                                               ! Coeficiente linear
        dX = dInterestX - oInputX(iFloorIndex)                                                                  ! Abcissa

        dInterpolatedValue = dA * dX + dB

    end subroutine InterpolateInVectors

    ! =============================================================
    ! =============================================================
    subroutine GetPvtSimWAXFileInfoFor1DFlowSimulation(dPressure, dTemperature, iWaxComponentCount, iPressureCount, iTemperatureCount, oPressurePoints, oCloudPointTemperatures, &
                    oStructurePressures, oStructureTemperatures, oStructureDensities, oStructureLiqMW, oStructureCPWax, oStructureThermCond, &
                    oStructureWaxConcs, oMolecularWeightsOfWaxComponents, oLiquidDensitiesOfWaxComponents, iIER, &
                    iWaxComponentCount_output, oMolecularWeightsOfWaxComponents_output, oLiquidDensitiesOfWaxComponents_output, dCloudPointT_output, &
                    dInterpolatedCPWax_output, dInterpolatedThermCond_output, dInterpolatedDens_output, dInterpolatedLiqMW_output, oInterpolatedWaxConcsTDeriv_output, &
                    oInterpolatedWaxConcs_output)

        ! OBJETIVO: Obter, a partir das informações lidas de um arquivo padrão "wax PVTSIM", os parâmetros necessários
        !       para que uma simulação 1D de escoamento possa fazer cálculos de deposição de parafinas.
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        real(c_double), value, intent(in) :: dPressure      ! Pressão local na qual deseja-se fazer as interpolações.
        real(c_double), value, intent(in) :: dTemperature   ! Temperatura local (interface) na qual deseja-se fazer as interpolações.
        integer(c_int), intent(in) :: iWaxComponentCount      ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        integer(c_int), intent(in) :: iPressureCount          ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        integer(c_int), intent(in) :: iTemperatureCount       ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:), intent(in) :: oPressurePoints         ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:), intent(in) :: oCloudPointTemperatures ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:), intent(in) :: oStructurePressures     ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:, :), intent(in) :: oStructureTemperatures ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:, :), intent(in) :: oStructureDensities    ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:, :), intent(in) :: oStructureLiqMW        ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:, :), intent(in) :: oStructureCPWax        ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:, :), intent(in) :: oStructureThermCond    ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:, :, :), intent(in) :: oStructureWaxConcs ! Variável obtida do arquivo "wax" do PVTSIM (consultar rotina de leitura deste arquivo).
        real(c_double), dimension(:), intent(in) :: oMolecularWeightsOfWaxComponents   ! DO ARQUIVO: "Molecular Weights of Wax Components" apresentados pelo arquivo.
        real(c_double), dimension(:), intent(in) :: oLiquidDensitiesOfWaxComponents    ! DO ARQUIVO: "Liquid densities of wax components" apresentados pelo arquivo.

        integer(c_int), intent(out) :: iIER                                 ! RESULTADO: Código de erros, conforme convenção.
        integer(c_int), intent(out) :: iWaxComponentCount_output
        real(c_double), dimension(:), intent(out) :: oMolecularWeightsOfWaxComponents_output
        real(c_double), dimension(:), intent(out) :: oLiquidDensitiesOfWaxComponents_output
        real(c_double), intent(out) :: dCloudPointT_output             ! RESULTADO: TIAC interpolada à pressão fornecida.
        real(c_double), intent(out) :: dInterpolatedCPWax_output       ! RESULTADO: "CPWax" interpolado à P e T de interesse
        real(c_double), intent(out) :: dInterpolatedThermCond_output   ! RESULTADO: "Therm Cond" interpolado à P e T de interesse
        real(c_double), intent(out) :: dInterpolatedDens_output        ! RESULTADO: "Dens" interpolado à P e T de interesse
        real(c_double), intent(out) :: dInterpolatedLiqMW_output       ! RESULTADO: "Liq MW" interpolado à P e T de interesse
        real(c_double), dimension(:), intent(out) :: oInterpolatedWaxConcsTDeriv_output    ! RESULTADO: Derivadas de concentrações com temperatura à P e T de interesse.
        real(c_double), dimension(:) :: oInterpolatedWaxConcs_output ! RESULTADO: Concentrações interpoladas na pressão e na temperatura.

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DAS VARIÁVEIS LOCAIS:
        real(c_double) :: dCloudPointT, dInterpolatedCPWax, dInterpolatedThermCond, dInterpolatedDens, dInterpolatedLiqMW
        real(c_double), dimension(iWaxComponentCount) :: oInterpolatedWaxConcsTDeriv
        real(c_double), dimension(iWaxComponentCount) :: oInterpolatedWaxConcs

        ! ------------ PROCEDIMENTOS:

        ! Fazer as interpolações que forem necessárias:
        call InterpolatePVTSIMWaxCalcParameters(iWaxComponentCount, iPressureCount, iTemperatureCount, oPressurePoints, oCloudPointTemperatures, oStructurePressures, &
            oStructureTemperatures, oStructureCPWax, oStructureThermCond, oStructureDensities, oStructureLiqMW, oStructureWaxConcs, &
            dPressure, dTemperature, iIER, dCloudPointT, dInterpolatedCPWax, dInterpolatedThermCond, dInterpolatedDens, dInterpolatedLiqMW, &
            oInterpolatedWaxConcsTDeriv, oInterpolatedWaxConcs)

        if (iIER.ne.ERROR_EverythingOK) return

        ! Retornar os resultados:
        iWaxComponentCount_output = iWaxComponentCount
        oMolecularWeightsOfWaxComponents_output = oMolecularWeightsOfWaxComponents
        oLiquidDensitiesOfWaxComponents_output = oLiquidDensitiesOfWaxComponents
        dCloudPointT_output = dCloudPointT
        dInterpolatedCPWax_output = dInterpolatedCPWax
        dInterpolatedThermCond_output = dInterpolatedThermCond
        dInterpolatedDens_output = dInterpolatedDens
        dInterpolatedLiqMW_output = dInterpolatedLiqMW
        oInterpolatedWaxConcsTDeriv_output = oInterpolatedWaxConcsTDeriv
        oInterpolatedWaxConcs_output = oInterpolatedWaxConcs

    end subroutine GetPvtSimWAXFileInfoFor1DFlowSimulation

end module PvtSimWAXFileImport
