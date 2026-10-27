from __future__ import annotations

from dataclasses import asdict, dataclass, field
from enum import Enum
from pathlib import Path
from typing import Any
import uuid


class RunStatus(str, Enum):
    PENDING = "pending"
    RUNNING = "running"
    PASSED = "passed"
    FAILED = "failed"
    CRASHED = "crashed"
    TIMEOUT = "timeout"
    CANCELLED = "cancelled"
    CONFIG_ERROR = "configuration_error"
    NO_COVERAGE = "no_coverage"


@dataclass
class ValidationRule:
    kind: str
    expected: Any = None
    path: str | None = None
    pattern: str | None = None
    tolerance_abs: float = 0.0
    tolerance_rel: float = 0.0
    options: dict[str, Any] = field(default_factory=dict)

    @classmethod
    def from_dict(cls, value: dict[str, Any]) -> "ValidationRule":
        allowed = {name for name in cls.__dataclass_fields__}
        return cls(**{key: item for key, item in value.items() if key in allowed})


@dataclass
class TestCase:
    name: str
    model_path: str
    id: str = field(default_factory=lambda: str(uuid.uuid4()))
    description: str = ""
    arguments: list[str] = field(default_factory=list)
    working_directory: str = "."
    environment: dict[str, str] = field(default_factory=dict)
    active: bool = True
    tags: list[str] = field(default_factory=list)
    suite: str = "default"
    expected_exit_code: int = 0
    validations: list[ValidationRule] = field(default_factory=list)
    auxiliary_files: list[str] = field(default_factory=list)
    notes: str = ""

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, value: dict[str, Any]) -> "TestCase":
        data = dict(value)
        data["validations"] = [ValidationRule.from_dict(rule) for rule in data.get("validations", [])]
        allowed = {name for name in cls.__dataclass_fields__}
        return cls(**{key: item for key, item in data.items() if key in allowed})


@dataclass
class ProcessResult:
    command: list[str]
    cwd: str
    exit_code: int | None
    stdout: str
    stderr: str
    started_at: str
    finished_at: str
    duration_seconds: float
    timed_out: bool = False
    cancelled: bool = False
    crashed: bool = False
    generated_files: list[str] = field(default_factory=list)


@dataclass
class ValidationResult:
    rule: str
    passed: bool
    message: str


@dataclass
class CoverageSummary:
    lines_covered: int = 0
    lines_total: int = 0
    functions_covered: int = 0
    functions_total: int = 0
    branches_covered: int = 0
    branches_total: int = 0

    @staticmethod
    def percentage(covered: int, total: int) -> float:
        return round(100.0 * covered / total, 2) if total else 0.0

    @property
    def line_percent(self) -> float:
        return self.percentage(self.lines_covered, self.lines_total)

    @property
    def function_percent(self) -> float:
        return self.percentage(self.functions_covered, self.functions_total)

    @property
    def branch_percent(self) -> float:
        return self.percentage(self.branches_covered, self.branches_total)


@dataclass
class CoverageReport:
    summary: CoverageSummary
    files: list[dict[str, Any]] = field(default_factory=list)
    functions: list[dict[str, Any]] = field(default_factory=list)
    raw_path: str | None = None
    profile_path: str | None = None

    def covered_line_keys(self) -> set[tuple[str, int]]:
        keys: set[tuple[str, int]] = set()
        for item in self.files:
            name = item.get("filename", "")
            for line in item.get("lines", []):
                if line.get("count", 0) > 0:
                    keys.add((name, int(line["line"])))
        return keys

    def covered_function_keys(self) -> set[str]:
        return {str(item.get("name", "")) for item in self.functions if item.get("count", 0) > 0}


@dataclass
class RunRecord:
    id: str
    test_id: str
    status: RunStatus
    process: ProcessResult
    validations: list[ValidationResult] = field(default_factory=list)
    coverage: CoverageReport | None = None
    artifact_directory: str = ""

    @property
    def regression_passed(self) -> bool:
        return bool(self.validations) and all(item.passed for item in self.validations)


def ensure_within(path: Path, allowed_root: Path) -> Path:
    resolved = path.resolve()
    root = allowed_root.resolve()
    try:
        resolved.relative_to(root)
    except ValueError as exc:
        raise ValueError(f"Path escapes controlled directory: {resolved}") from exc
    return resolved
