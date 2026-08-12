from __future__ import annotations

from dataclasses import dataclass
import json
import os
from pathlib import Path
import shutil
from typing import Callable

from .config import DATA_ROOT, ToolConfig
from .domain import ProcessResult, ensure_within
from .process import ProcessExecutor


@dataclass
class BuildResult:
    configured: ProcessResult | None
    compiled: ProcessResult | None
    executable: str | None
    success: bool
    message: str


class ShadowSourceBuilder:
    """Builds from a disposable source mirror because upstream CMake writes to its source tree."""

    COPY_ENTRIES = ("CMakeLists.txt", "_version.py", "src", "marlim3/translations.json")

    def __init__(self, config: ToolConfig, executor: ProcessExecutor | None = None):
        self.config = config
        self.executor = executor or ProcessExecutor()
        self.shadow_root = DATA_ROOT / "cache" / "shadow-source"

    @staticmethod
    def _copy_file(source: Path, destination: Path) -> None:
        if source.is_symlink():
            raise ValueError(f"Refusing symlink in build input: {source}")
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)

    def prepare_shadow(self) -> Path:
        ensure_within(self.shadow_root, DATA_ROOT)
        if self.shadow_root.exists():
            shutil.rmtree(self.shadow_root)
        self.shadow_root.mkdir(parents=True)
        project = self.config.resolved_project_root
        for relative in self.COPY_ENTRIES:
            source = project / relative
            destination = self.shadow_root / relative
            if source.is_dir():
                shutil.copytree(source, destination, symlinks=False, ignore=shutil.ignore_patterns("*.o", "*.gcda", "*.gcno", "__pycache__"))
            elif source.is_file():
                self._copy_file(source, destination)
            else:
                raise FileNotFoundError(f"Required build input not found: {source}")
        (self.shadow_root / "marlim3").mkdir(exist_ok=True)
        self._apply_compatibility_patches()
        return self.shadow_root

    def _apply_compatibility_patches(self) -> None:
        """Apply narrowly-scoped compiler adaptations only to the disposable mirror."""
        patches = []
        if self.config.coverage_backend == "llvm":
            path = self.shadow_root / "src" / "core" / "LerAP.cpp"
            source = path.read_text(encoding="utf-8")
            old = "    int iVaria[dim] = {};"
            new = "    vector<int> iVaria(dim, 0);"
            count = source.count(old)
            if count != 1:
                raise RuntimeError(f"Expected one Clang VLA compatibility site, found {count}")
            path.write_text(source.replace(old, new), encoding="utf-8")
            patches.append({"file": "src/core/LerAP.cpp", "reason": "Clang rejects initialized variable-length arrays", "replacement": "VLA to zero-initialized std::vector"})
        (self.shadow_root / ".regression-tool-patches.json").write_text(json.dumps(patches, indent=2), encoding="utf-8")

    def configure_command(self) -> list[str]:
        build_dir = self.config.build_directory
        backend = self.config.coverage_backend
        if backend == "llvm":
            cxx_flags = "-O0 -g -fprofile-instr-generate -fcoverage-mapping -fno-omit-frame-pointer"
            fortran_flags = "-O0 -g"
            # CMake reuses CMAKE_EXE_LINKER_FLAGS while probing the Fortran
            # compiler. The C++ flags are also present on the mixed target's
            # C++ link command, so keep the global linker flags empty here.
            linker_flags = ""
        elif backend == "gcov":
            cxx_flags = "-O0 -g --coverage -fno-omit-frame-pointer"
            fortran_flags = "-O0 -g --coverage"
            linker_flags = "--coverage"
        else:
            raise ValueError(f"Unsupported coverage backend: {backend}")
        command = [
            self.config.cmake_path, "-S", str(self.shadow_root), "-B", str(build_dir),
            "-G", self.config.generator,
            "-DCMAKE_BUILD_TYPE=Debug",
            f"-DCMAKE_CXX_COMPILER={self.config.cxx_compiler}",
            f"-DCMAKE_Fortran_COMPILER={self.config.fortran_compiler}",
            "-DMARLIM_STATIC_GNU_RUNTIMES=OFF",
            f"-DCMAKE_CXX_FLAGS={cxx_flags}",
            f"-DCMAKE_Fortran_FLAGS={fortran_flags}",
        ]
        if linker_flags:
            command.append(f"-DCMAKE_EXE_LINKER_FLAGS={linker_flags}")
        return command

    def build(self, on_output: Callable[[str, str], None] | None = None) -> BuildResult:
        self.prepare_shadow()
        build_dir = self.config.build_directory
        ensure_within(build_dir, DATA_ROOT)
        if build_dir.exists():
            shutil.rmtree(build_dir)
        build_dir.mkdir(parents=True, exist_ok=True)
        configured = self.executor.run(self.configure_command(), DATA_ROOT, timeout=300, on_output=on_output)
        if configured.exit_code != 0:
            return BuildResult(configured, None, None, False, "CMake configuration failed")
        command = [self.config.cmake_path, "--build", str(build_dir), "--parallel", str(self.config.max_parallel_builds)]
        compiled = self.executor.run(command, DATA_ROOT, timeout=3600, on_output=on_output)
        executable_name = "Marlim3.exe" if os.name == "nt" else "Marlim3"
        executable = build_dir / executable_name
        success = compiled.exit_code == 0 and executable.is_file()
        return BuildResult(configured, compiled, str(executable) if executable.is_file() else None, success, "Instrumented build ready" if success else "Compilation failed or executable missing")


class FixtureBuilder:
    def __init__(self, config: ToolConfig, executor: ProcessExecutor | None = None):
        self.config = config
        self.executor = executor or ProcessExecutor()

    def build(self, source: Path, output: Path) -> ProcessResult:
        ensure_within(output, DATA_ROOT)
        output.parent.mkdir(parents=True, exist_ok=True)
        if self.config.coverage_backend == "llvm":
            command = [self.config.cxx_compiler, "-std=c++11", "-O0", "-g", "-fprofile-instr-generate", "-fcoverage-mapping", str(source), "-o", str(output)]
        else:
            command = ["g++", "-std=c++11", "-O0", "-g", "--coverage", str(source), "-o", str(output)]
        return self.executor.run(command, output.parent, timeout=120)
