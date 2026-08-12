from __future__ import annotations

from dataclasses import asdict, dataclass, field
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
from typing import Any


TOOL_ROOT = Path(__file__).resolve().parents[1]
PACKAGE_ROOT = Path(__file__).resolve().parent


def _looks_like_marlim_root(path: Path) -> bool:
    return (path / "CMakeLists.txt").is_file() and (path / "src").is_dir()


def _default_project_root() -> Path:
    configured = os.environ.get("MARLIM_PROJECT_ROOT", "").strip()
    candidates = [Path(configured).expanduser()] if configured else []
    candidates.extend((TOOL_ROOT.parent, Path.cwd(), Path.cwd().parent))
    for candidate in candidates:
        resolved = candidate.resolve()
        if _looks_like_marlim_root(resolved):
            return resolved
    return TOOL_ROOT.parent.resolve()


DEFAULT_PROJECT_ROOT = _default_project_root()


def user_data_home() -> Path:
    """Return an OS-native, user-private state directory outside both repositories."""
    override = os.environ.get("MARLIM_REGRESSION_DATA_DIR", "").strip()
    if override:
        return Path(override).expanduser().resolve()
    system = platform.system()
    if system == "Windows":
        base = Path(os.environ.get("LOCALAPPDATA", Path.home() / "AppData" / "Local"))
        return base / "Marlim3" / "RegressionTool"
    if system == "Darwin":
        return Path.home() / "Library" / "Application Support" / "Marlim3 Regression Tool"
    base = Path(os.environ.get("XDG_STATE_HOME", Path.home() / ".local" / "state"))
    return base / "marlim3-regression-tool"


def project_data_root(project_root: Path = DEFAULT_PROJECT_ROOT) -> Path:
    resolved = project_root.resolve()
    identity = str(resolved).casefold() if os.name == "nt" else str(resolved)
    digest = hashlib.sha256(identity.encode("utf-8")).hexdigest()[:12]
    slug = re.sub(r"[^A-Za-z0-9._-]+", "-", resolved.name).strip("-_") or "project"
    return user_data_home() / f"{slug}-{digest}"


DATA_ROOT = project_data_root()


def _llvm_family() -> tuple[str, str, str]:
    """Prefer one complete, version-matched LLVM family."""
    for major in range(22, 12, -1):
        values = (f"clang++-{major}", f"llvm-profdata-{major}", f"llvm-cov-{major}")
        if all(shutil.which(value) for value in values):
            return values
    return "clang++", "llvm-profdata", "llvm-cov"


DEFAULT_CLANG, DEFAULT_PROFDATA, DEFAULT_LLVM_COV = _llvm_family()


@dataclass
class ToolConfig:
    project_root: str = ""
    build_backend: str = "llvm"
    coverage_backend: str = "llvm"
    cmake_path: str = "cmake"
    generator: str = "Ninja"
    cxx_compiler: str = DEFAULT_CLANG
    fortran_compiler: str = "gfortran"
    llvm_profdata: str = DEFAULT_PROFDATA
    llvm_cov: str = DEFAULT_LLVM_COV
    gcov_path: str = "gcov"
    max_parallel_builds: int = max(1, min(8, os.cpu_count() or 1))
    executable_path: str = "build/coverage/Marlim3.exe" if os.name == "nt" else "build/coverage/Marlim3"
    run_directory: str = "runs"
    source_directories: list[str] = field(default_factory=lambda: ["src/core", "src/include", "src/fortran"])
    coverage_excludes: list[str] = field(default_factory=lambda: ["src/thirdparty/*", "*/CMakeFiles/*"])
    default_arguments: list[str] = field(default_factory=lambda: ["-i", "{model}", "-d", "{output}", "-o", "{output}/simulacao.log"])
    environment: dict[str, str] = field(default_factory=dict)
    trace_mode: str = "disabled"
    storage_limit_mb: int = 4096

    @property
    def resolved_project_root(self) -> Path:
        if not self.project_root.strip():
            return DEFAULT_PROJECT_ROOT
        path = Path(self.project_root)
        return (TOOL_ROOT / path).resolve() if not path.is_absolute() else path.resolve()

    @property
    def data_root(self) -> Path:
        return DATA_ROOT

    @property
    def build_directory(self) -> Path:
        return DATA_ROOT / "build" / "coverage"

    @property
    def resolved_executable(self) -> Path:
        path = Path(self.executable_path)
        return (DATA_ROOT / path).resolve() if not path.is_absolute() else path.resolve()

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, value: dict[str, Any]) -> "ToolConfig":
        allowed = {name for name in cls.__dataclass_fields__}
        return cls(**{key: item for key, item in value.items() if key in allowed})


class ConfigManager:
    def __init__(self, path: Path | None = None):
        self.path = path or DATA_ROOT / "config.json"

    def load(self) -> ToolConfig:
        if not self.path.exists():
            return ToolConfig()
        return ToolConfig.from_dict(json.loads(self.path.read_text(encoding="utf-8")))

    def save(self, config: ToolConfig) -> None:
        self.path.parent.mkdir(parents=True, exist_ok=True)
        temporary = self.path.with_suffix(".tmp")
        temporary.write_text(json.dumps(config.to_dict(), indent=2, ensure_ascii=False), encoding="utf-8")
        temporary.replace(self.path)


def initialize_data_directories() -> None:
    for relative in (
        "build/coverage", "build/release", "coverage", "traces", "logs", "cache",
        "runs", "diagnostics", "inputs/models",
    ):
        (DATA_ROOT / relative).mkdir(parents=True, exist_ok=True)
