module DebugFacilities

    ! OBJETIVO: Centralizar funcionalidades relacionadas ao debug ágil da biblioteca

    implicit none

    ! ===================================================================================================
    !       CONSTANTES
    ! ===================================================================================================
    logical, parameter :: GenerateDebugFile = .false.                                ! Mudar para "true" para gerar o arquivo.
    character(len=*), parameter :: sDebugFileName = "Composicional_Debug.txt"       ! Nome do arquivo com as informações de debug

    ! ===================================================================================================
    !       ROTINAS
    ! ===================================================================================================
    contains

    subroutine WriteDebugFileLine(sDebugFileLineToWrite, bConfirmWriteLine)

        ! OBJETIVO: Acrescentar uma linha com informações ao arquivo de "debug".
        implicit none

        ! ------------ DECLARAÇÃO E DESCRIÇÃO DOS ARGUMENTOS:
        character(len=*), intent(in) :: sDebugFileLineToWrite           ! Linha a acrescentar ao arquivo de "debug".
        logical, intent(in), optional :: bConfirmWriteLine              ! Somente prossegue-se com a escrita da linha se for "true"

        ! ------------ CONSTANTES:
        integer, parameter :: iDebugFileUnit = 3

        ! ------------ PROCEDIMENTOS:

        ! Não fazer nada se o debug não estiver ligado:
        if(.not.GenerateDebugFile) return

        ! Não escrever se for esta a opção:
        overrideIf: if (present(bConfirmWriteLine)) then
            if (.not. bConfirmWriteLine) return
        end if overrideIf

        ! Abre o arquivo EM MODO APPEND (sem sobrescrever o conteúdo existente):
        open(unit   = iDebugFileUnit,       &
             file   = sDebugFileName,       &
             status = "unknown",            &
             position = "append",           &
             action = "write")

        ! Escreve a linha recebida no arquivo:
        write(iDebugFileUnit, '(A)') trim(sDebugFileLineToWrite)

        ! Fecha o arquivo:
        close(iDebugFileUnit)

    end subroutine WriteDebugFileLine


end module DebugFacilities
