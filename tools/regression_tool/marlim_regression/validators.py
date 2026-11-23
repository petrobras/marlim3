from __future__ import annotations

import csv
import hashlib
import json
import math
from pathlib import Path
import re
from typing import Callable

from .domain import ProcessResult, ValidationResult, ValidationRule


Validator = Callable[[ValidationRule, ProcessResult, Path], ValidationResult]


def _result(rule: ValidationRule, passed: bool, message: str) -> ValidationResult:
    return ValidationResult(rule.kind, passed, message)


class ValidatorRegistry:
    def __init__(self) -> None:
        self._validators: dict[str, Validator] = {}
        self._register_defaults()

    def register(self, kind: str, validator: Validator) -> None:
        self._validators[kind] = validator

    def validate(self, rules: list[ValidationRule], process: ProcessResult, run_dir: Path) -> list[ValidationResult]:
        results = []
        for rule in rules:
            validator = self._validators.get(rule.kind)
            results.append(validator(rule, process, run_dir) if validator else _result(rule, False, f"Unknown validator: {rule.kind}"))
        return results

    def _register_defaults(self) -> None:
        self.register("exit_code", lambda r, p, d: _result(r, p.exit_code == int(r.expected), f"exit code {p.exit_code}; expected {r.expected}"))
        self.register("no_crash", lambda r, p, d: _result(r, not p.crashed, "process did not crash" if not p.crashed else "process crashed"))
        self.register("stdout_contains", lambda r, p, d: _result(r, str(r.expected) in p.stdout, f"stdout contains {r.expected!r}"))
        self.register("stderr_absent", lambda r, p, d: _result(r, str(r.expected) not in p.stderr, f"stderr does not contain {r.expected!r}"))
        self.register("stdout_regex", lambda r, p, d: _result(r, bool(re.search(r.pattern or str(r.expected), p.stdout, re.MULTILINE)), "stdout regular expression"))
        self.register("file_exists", self._file_exists)
        self.register("file_absent", self._file_absent)
        self.register("file_size", self._file_size)
        self.register("file_hash", self._file_hash)
        self.register("text_equals", self._text_equals)
        self.register("binary_equals", self._binary_equals)
        self.register("json_equals", self._json_equals)
        self.register("csv_equals", self._csv_equals)
        self.register("numeric", self._numeric)
        self.register("max_duration", lambda r, p, d: _result(r, p.duration_seconds <= float(r.expected), f"duration {p.duration_seconds:.3f}s <= {r.expected}s"))

    @staticmethod
    def _safe_file(run_dir: Path, path: str | None) -> Path:
        candidate = (run_dir / (path or "")).resolve()
        candidate.relative_to(run_dir.resolve())
        return candidate

    def _file_exists(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        path = self._safe_file(d, r.path)
        return _result(r, path.is_file(), f"file {'exists' if path.is_file() else 'missing'}: {r.path}")

    def _file_absent(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        path = self._safe_file(d, r.path)
        return _result(r, not path.exists(), f"file {'absent' if not path.exists() else 'unexpected'}: {r.path}")

    def _file_size(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        path = self._safe_file(d, r.path)
        actual = path.stat().st_size if path.is_file() else -1
        return _result(r, actual == int(r.expected), f"size {actual}; expected {r.expected}")

    def _file_hash(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        path = self._safe_file(d, r.path)
        actual = hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else "missing"
        return _result(r, actual == str(r.expected), f"sha256 {actual}")

    def _reference(self, r: ValidationRule, d: Path) -> tuple[Path, Path]:
        actual = self._safe_file(d, r.path)
        reference = Path(str(r.expected)).expanduser().resolve()
        return actual, reference

    def _text_equals(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        actual, reference = self._reference(r, d)
        equal = actual.read_text(encoding="utf-8", errors="replace") == reference.read_text(encoding="utf-8", errors="replace")
        return _result(r, equal, f"text comparison with {reference}")

    def _binary_equals(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        actual, reference = self._reference(r, d)
        return _result(r, actual.read_bytes() == reference.read_bytes(), f"binary comparison with {reference}")

    def _json_equals(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        actual, reference = self._reference(r, d)
        equal = json.loads(actual.read_text(encoding="utf-8")) == json.loads(reference.read_text(encoding="utf-8"))
        return _result(r, equal, f"JSON comparison with {reference}")

    def _csv_equals(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        actual, reference = self._reference(r, d)
        with actual.open(newline="", encoding="utf-8") as left, reference.open(newline="", encoding="utf-8") as right:
            equal = list(csv.reader(left)) == list(csv.reader(right))
        return _result(r, equal, f"CSV comparison with {reference}")

    def _numeric(self, r: ValidationRule, p: ProcessResult, d: Path) -> ValidationResult:
        actual = float(r.options.get("actual", "nan"))
        expected = float(r.expected)
        equal = math.isclose(actual, expected, rel_tol=r.tolerance_rel, abs_tol=r.tolerance_abs)
        return _result(r, equal, f"numeric {actual}; expected {expected} (abs={r.tolerance_abs}, rel={r.tolerance_rel})")

