from __future__ import annotations

from dataclasses import asdict, dataclass, field
import platform
import re
import shutil
import subprocess

from .config import ToolConfig
from .storage import discover_project_demos


@dataclass
class ToolInfo:
    name: str
    path: str | None
    version: str = ""

    @property
    def available(self) -> bool:
        return bool(self.path)


@dataclass
class DetectionReport:
    project_root: str
    valid_project: bool
    operating_system: str
    architecture: str
    build_system: str
    cpp_standard: str
    executable_name: str
    tools: list[ToolInfo] = field(default_factory=list)
    demos: list[str] = field(default_factory=list)
    risks: list[str] = field(default_factory=list)
    recommended_backend: str = "none"

    def to_dict(self) -> dict:
        return asdict(self)


class ProjectDetector:
    TOOL_NAMES = ("cmake", "ninja", "clang++", "g++", "gfortran", "llvm-profdata", "llvm-cov", "gcov")

    @staticmethod
    def _version(command: str) -> str:
        candidates = ([command, "--version"], [command, "version"])
        for args in candidates:
            try:
                result = subprocess.run(args, capture_output=True, text=True, timeout=5, check=False)
                output = (result.stdout or result.stderr).strip().splitlines()
                if output:
                    return output[0][:180]
            except (OSError, subprocess.SubprocessError):
                continue
        return ""

    def detect(self, config: ToolConfig) -> DetectionReport:
        root = config.resolved_project_root
        cmake_file = root / "CMakeLists.txt"
        source = cmake_file.read_text(encoding="utf-8", errors="replace") if cmake_file.exists() else ""
        standard = re.search(r"CMAKE_CXX_STANDARD\s+(\d+)", source)
        tools: list[ToolInfo] = []
        configured = {
            "cmake": config.cmake_path,
            "clang++": config.cxx_compiler,
            "gfortran": config.fortran_compiler,
            "llvm-profdata": config.llvm_profdata,
            "llvm-cov": config.llvm_cov,
            "gcov": config.gcov_path,
        }
        for name in self.TOOL_NAMES:
            candidate = configured.get(name, name)
            path = shutil.which(candidate) if candidate else None
            tools.append(ToolInfo(name, path, self._version(path) if path else ""))
        found = {item.name: item.available for item in tools}
        llvm_ready = found.get("clang++") and found.get("llvm-profdata") and found.get("llvm-cov")
        gcc_ready = found.get("g++") and found.get("gcov")
        demos = [test.model_path for test in discover_project_demos(root)]
        risks = [
            "The original CMake generates headers and copies the executable into the source tree; a shadow copy is mandatory.",
            "The target mixes C++ and Fortran; LLVM Coverage covers the C++ side, while Fortran requires gcov or a dedicated tool.",
            "OpenMP discovery with Clang depends on libomp; the alternative GCC build may be required.",
        ]
        return DetectionReport(
            project_root=str(root),
            valid_project=bool(cmake_file.exists() and (root / "src" / "core" / "Num4Main.cpp").exists()),
            operating_system=platform.system(),
            architecture=platform.machine(),
            build_system="CMake 3.16+ with presets" if cmake_file.exists() else "unknown",
            cpp_standard=f"C++{standard.group(1)}" if standard else "unknown",
            executable_name="Marlim3.exe" if platform.system() == "Windows" else "Marlim3",
            tools=tools,
            demos=demos,
            risks=risks,
            recommended_backend="llvm" if llvm_ready else ("gcov" if gcc_ready else "none"),
        )
