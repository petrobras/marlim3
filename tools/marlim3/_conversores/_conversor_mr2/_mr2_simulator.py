"""
Wrapper for the legacy Marlim2 simulator binary.

The binary is NOT bundled — the user must provide the path to it.
"""
import os
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

from ..._process import hidden_process_kwargs


class Mr2Simulator:
    """Runs the legacy MR2 binary to generate simulation results."""

    def __init__(self, mr2_reader, pvt_path=None, mr2_binary_path=None):
        self.mr2_reader = mr2_reader
        self.pvt_path = pvt_path
        self.mr2_binary_path = mr2_binary_path

    def executar(self):
        """
        Execute the MR2 simulator to produce results in the .mr2 file.

        The MR2 binary reads the .mr2 file, runs the simulation, and writes
        results back into the same file.

        Raises:
            FileNotFoundError: If the MR2 binary path is not provided or doesn't exist.
            RuntimeError: If the simulation fails.
        """
        if not self.mr2_binary_path:
            raise FileNotFoundError(
                "O caminho do binário do simulador MR2 (mr2_binary_path) é obrigatório "
                "quando o arquivo .mr2 não possui resultados embutidos."
            )

        binary_path = Path(self.mr2_binary_path)
        if not binary_path.exists():
            raise FileNotFoundError(
                f"Binário do simulador MR2 não encontrado: {self.mr2_binary_path}"
            )

        # Create a temporary simulation directory
        timestamp = datetime.now().strftime('%Y%m%d%H%M%S%f')
        sim_dir = Path(self.mr2_reader.diretorio) / f'_mr2_sim_{timestamp}'
        sim_dir.mkdir(parents=True, exist_ok=True)

        try:
            # Copy MR2 file to simulation directory
            mr2_file = sim_dir / self.mr2_reader.nome_completo
            shutil.copy(self.mr2_reader.caminho_mr2, mr2_file)

            # Create log file
            log_file = sim_dir / 'arquivo.log'
            log_file.touch()

            # Copy binary to simulation directory
            shutil.copy(binary_path, sim_dir)

            # Copy associated libraries if they exist
            binary_dir = binary_path.parent
            if sys.platform == 'win32':
                libs = ['libcomposicional.dll', 'libpvtsim_parser.dll']
            else:
                libs = ['libcomposicional.so', 'libpvtsim_parser.so']
            for lib in libs:
                lib_path = binary_dir / lib
                if lib_path.exists():
                    shutil.copy(lib_path, sim_dir)

            # Build command
            exe_name = binary_path.name
            commands = [
                str(sim_dir / exe_name),
                'sim',
                str(mr2_file),
                str(log_file),
            ]

            # Add PVT path if available
            if self.pvt_path and os.path.exists(self.pvt_path):
                pvt_dir = sim_dir / 'pvt_chave'
                pvt_dir.mkdir(exist_ok=True)
                shutil.copy(self.pvt_path, pvt_dir)
                commands.append(str(pvt_dir))

            # Run simulation
            process = subprocess.run(
                commands,
                cwd=str(sim_dir),
                capture_output=True,
                text=True,
                encoding='latin-1',
                **hidden_process_kwargs(),
            )

            if process.returncode != 0:
                raise RuntimeError(
                    f"Simulação MR2 falhou (código {process.returncode}): "
                    f"{process.stderr}"
                )

            # Move result .mr2 file back
            shutil.copy(mr2_file, self.mr2_reader.caminho_mr2)

            # Reload the reader content
            with open(self.mr2_reader.caminho_mr2, 'r', encoding='latin-1') as f:
                self.mr2_reader._conteudo = f.read()
            # Invalidate cached properties
            if hasattr(self.mr2_reader, '_tree'):
                del self.mr2_reader._tree
            if hasattr(self.mr2_reader, '_dados_geometria'):
                del self.mr2_reader._dados_geometria

        finally:
            # Cleanup
            shutil.rmtree(sim_dir, ignore_errors=True)
