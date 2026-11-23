from __future__ import annotations

from dataclasses import asdict
import json
import os
from pathlib import Path
import shutil
from typing import Callable
import uuid

from .config import DATA_ROOT, ToolConfig
from .coverage import CoverageProvider
from .domain import RunRecord, RunStatus, TestCase, ValidationRule, ensure_within
from .process import ProcessExecutor
from .validators import ValidatorRegistry


class TestRunner:
    def __init__(self, config: ToolConfig, coverage: CoverageProvider, executor: ProcessExecutor | None = None, validators: ValidatorRegistry | None = None):
        self.config = config
        self.coverage = coverage
        self.executor = executor or ProcessExecutor()
        self.validators = validators or ValidatorRegistry()

    def cancel(self) -> bool:
        return self.executor.cancel()

    def _resolve_input(self, path: str) -> Path:
        if path.startswith("@data/"):
            return ensure_within(DATA_ROOT / path.removeprefix("@data/"), DATA_ROOT)
        candidate = Path(path)
        if candidate.is_absolute():
            return candidate.resolve()
        project_candidate = (self.config.resolved_project_root / candidate).resolve()
        return project_candidate

    def _prepare(self, test: TestCase, run_id: str) -> tuple[Path, Path, Path, Path]:
        run_dir = ensure_within(DATA_ROOT / "runs" / run_id, DATA_ROOT)
        work = run_dir / "work"
        output = work / "output"
        inputs = work / "inputs"
        for directory in (output, inputs, run_dir / "coverage"):
            directory.mkdir(parents=True, exist_ok=True)
        relative_work = Path(test.working_directory)
        if relative_work.is_absolute():
            raise ValueError("Test working directory must be relative to its isolated run directory")
        execution_cwd = ensure_within(work / relative_work, work)
        execution_cwd.mkdir(parents=True, exist_ok=True)
        model = self._resolve_input(test.model_path)
        if not model.is_file():
            raise FileNotFoundError(f"Model not found: {model}")
        copied_model = inputs / model.name
        shutil.copy2(model, copied_model)
        for item in test.auxiliary_files:
            source = self._resolve_input(item)
            if not source.is_file():
                raise FileNotFoundError(f"Auxiliary file not found: {source}")
            shutil.copy2(source, inputs / source.name)
            destination = execution_cwd / source.name
            if destination.resolve() != (inputs / source.name).resolve():
                shutil.copy2(source, destination)
        return run_dir, copied_model, output, execution_cwd

    def run(self, test: TestCase, on_output: Callable[[str, str], None] | None = None) -> RunRecord:
        run_id = str(uuid.uuid4())
        try:
            run_dir, model, output, execution_cwd = self._prepare(test, run_id)
            executable = self.config.resolved_executable
            if not executable.is_file():
                raise FileNotFoundError(f"Instrumented executable not found: {executable}")
            arguments = test.arguments or self.config.default_arguments
            values = {"model": str(model), "output": str(output), "work": str(run_dir / "work"), "project": str(self.config.resolved_project_root)}
            expanded = [part.format(**values) for part in arguments]
            environment = dict(self.config.environment)
            environment.update(test.environment)
            environment.update(self.coverage.environment_for_run(run_id, run_dir))
            process = self.executor.run(
                [str(executable), *expanded], execution_cwd, environment,
                timeout=None, on_output=on_output,
            )
            rules = list(test.validations)
            if not any(rule.kind == "exit_code" for rule in rules):
                rules.insert(0, ValidationRule("exit_code", expected=test.expected_exit_code))
            if not any(rule.kind == "no_crash" for rule in rules):
                rules.insert(1, ValidationRule("no_crash", expected=True))
            validations = self.validators.validate(rules, process, execution_cwd)
            report = None
            try:
                report = self.coverage.collect(executable, run_dir)
            except (OSError, RuntimeError, ValueError) as exc:
                (run_dir / "coverage" / "error.txt").write_text(str(exc), encoding="utf-8")
            status = RunStatus.PASSED if all(item.passed for item in validations) and report is not None else RunStatus.CRASHED
            record = RunRecord(run_id, test.id, status, process, validations, report, str(run_dir))
        except Exception as exc:  # Convert any model execution failure into the single failure state.
            from datetime import datetime, timezone
            from .domain import ProcessResult
            now = datetime.now(timezone.utc).isoformat()
            process = ProcessResult([], "", None, "", str(exc), now, now, 0.0)
            run_dir = DATA_ROOT / "runs" / run_id
            run_dir.mkdir(parents=True, exist_ok=True)
            record = RunRecord(run_id, test.id, RunStatus.CRASHED, process, artifact_directory=str(run_dir))
        metadata = Path(record.artifact_directory) / "run.json"
        metadata.write_text(json.dumps(asdict(record), indent=2, ensure_ascii=False), encoding="utf-8")
        return record
