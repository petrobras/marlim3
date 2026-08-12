from __future__ import annotations

from dataclasses import asdict
from datetime import datetime, timezone
import hashlib
from http import HTTPStatus
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import json
import mimetypes
import os
from pathlib import Path
import platform
import shutil
import subprocess
import threading
import uuid
import webbrowser
from urllib.parse import parse_qs, unquote, urlparse

from .analysis import FlowGraphBuilder, compare_reports, incremental_contributions
from .build import ShadowSourceBuilder
from .config import ConfigManager, DATA_ROOT, PACKAGE_ROOT, initialize_data_directories
from .coverage import coverage_provider, parse_llvm_export
from .detection import ProjectDetector
from .domain import CoverageReport, CoverageSummary, TestCase, ensure_within
from .icons import (
    browser_icon_path,
    browser_icon_version,
    chrome_window_flags,
    favicon_path,
    prepare_linux_desktop_icon,
)
from .runner import TestRunner
from .storage import StorageRepository, discover_project_demos
from .uploads import ModelUploadStore, decode_upload
from .workflow import WorkflowStateStore


FRONTEND_DIST = PACKAGE_ROOT / "web"
REPORTS_CACHE_LIMIT = 16
GRAPH_CACHE_LIMIT = 4
ENDPOINT_NOT_FOUND = "Endpoint not found"


def _coverage_report(value: dict | None) -> CoverageReport | None:
    if not value:
        return None
    data = dict(value)
    data["summary"] = CoverageSummary(**data.get("summary", {}))
    return CoverageReport(**data)


def _percent(covered: int, total: int) -> float:
    return round(100.0 * covered / total, 2) if total else 0.0


class RegressionService:
    def __init__(self) -> None:
        initialize_data_directories()
        self.config_manager = ConfigManager()
        self.config = self.config_manager.load()
        self.storage = StorageRepository(DATA_ROOT / "regression.db")
        self.detector = ProjectDetector()
        self.workflow = WorkflowStateStore(DATA_ROOT / "workflow-state.json")
        self.jobs: dict[str, dict] = {}
        self.jobs_lock = threading.Lock()
        self.graph_cache: dict[str, dict] = {}
        self.reports_cache: dict[tuple[str, ...], list[tuple[str, CoverageReport]]] = {}
        self.uploads = ModelUploadStore(DATA_ROOT)

    def _invalidate_analysis_caches(self) -> None:
        """Drop coverage-derived caches after any change to tests or run results."""
        self.graph_cache.clear()
        self.reports_cache.clear()

    def visible_tests(self) -> list[TestCase]:
        """Project demos plus locally registered models.

        Demos are always rediscovered so models added to ``demos/`` appear without a
        restart; only the assembled, sorted list is reused within a single request.
        """
        discovered = discover_project_demos(self.config.resolved_project_root)
        seen_ids = {test.id for test in discovered}
        seen_paths = {Path(test.model_path).as_posix() for test in discovered}
        custom = [
            test for test in self.storage.list_tests()
            if test.id not in seen_ids and Path(test.model_path).as_posix() not in seen_paths
        ]
        return sorted([*discovered, *custom], key=lambda test: (test.suite, test.name.casefold()))

    def test_names(self) -> dict[str, str]:
        """Id-to-name map used by the graph and comparison responses."""
        return {test.id: test.name for test in self.visible_tests()}

    def get_test(self, test_id: str) -> TestCase | None:
        stored = self.storage.get_test(test_id)
        if stored:
            return stored
        return next((test for test in self.visible_tests() if test.id == test_id), None)

    def project_logo(self) -> Path | None:
        root = self.config.resolved_project_root
        for relative in ("assets/branding/logo.svg",):
            candidate = (root / relative).resolve()
            try:
                candidate.relative_to(root)
            except ValueError:
                continue
            if candidate.is_file():
                return candidate
        return None

    @staticmethod
    def _display_run_status(status: str) -> str:
        return "passed" if status == "passed" else "crashed"

    def _latest_payloads(self, test_ids: list[str] | None = None) -> dict[str, dict]:
        latest: dict[str, dict] = {}
        for payload in self.storage.latest_run_payloads(test_ids):
            test_id = str(payload.get("test_id", ""))
            if test_id and test_id not in latest:
                latest[test_id] = payload
        return latest

    def latest_reports(self, test_ids: list[str] | None = None) -> list[tuple[str, CoverageReport]]:
        cache_key = tuple(test_ids or [])
        cached = self.reports_cache.get(cache_key)
        if cached is not None:
            return cached
        payloads = self._latest_payloads(test_ids)
        ordered_ids = list(test_ids or payloads)
        reports: list[tuple[str, CoverageReport]] = []
        for test_id in ordered_ids:
            payload = payloads.get(test_id)
            if payload is None:
                continue
            report = _coverage_report(payload.get("coverage"))
            if report:
                reports.append((test_id, report))
        if len(self.reports_cache) >= REPORTS_CACHE_LIMIT:
            self.reports_cache.pop(next(iter(self.reports_cache)))
        self.reports_cache[cache_key] = reports
        return reports

    def _required_reports(self, test_ids: list[str], *, exact_count: int | None = None) -> list[tuple[str, CoverageReport]]:
        if exact_count is not None and len(test_ids) != exact_count:
            raise ValueError(f"Select exactly {exact_count} executed tests")
        if not test_ids:
            raise ValueError("Select at least one executed test")
        reports = self.latest_reports(test_ids)
        available = {test_id for test_id, _ in reports}
        missing = [test_id for test_id in test_ids if test_id not in available]
        if missing:
            raise ValueError("Run every selected test before analysing flow and coverage")
        return reports

    def _combined_report(self, reports: list[tuple[str, CoverageReport]]) -> CoverageReport | None:
        if not reports:
            return None
        if len(reports) == 1:
            return reports[0][1]
        profiles = [Path(report.profile_path) for _, report in reports if report.profile_path]
        signature = "|".join(
            f"{test_id}:{path}:{path.stat().st_mtime_ns if path.is_file() else 0}"
            for (test_id, _), path in zip(reports, profiles)
        )
        key = hashlib.sha256(signature.encode()).hexdigest()[:20]
        output = DATA_ROOT / "cache" / "combined" / key
        exported = output / "coverage.json"
        try:
            if exported.is_file():
                return parse_llvm_export(json.loads(exported.read_text(encoding="utf-8")))
            if len(profiles) == len(reports) and self.config.resolved_executable.is_file():
                return coverage_provider(self.config).merge(self.config.resolved_executable, profiles, output)
        except (OSError, RuntimeError, ValueError, json.JSONDecodeError):
            pass
        lines = set().union(*(report.covered_line_keys() for _, report in reports))
        functions = set().union(*(report.covered_function_keys() for _, report in reports))
        return CoverageReport(CoverageSummary(
            lines_covered=len(lines),
            lines_total=max(report.summary.lines_total for _, report in reports),
            functions_covered=len(functions),
            functions_total=max(report.summary.functions_total for _, report in reports),
            branches_covered=max(report.summary.branches_covered for _, report in reports),
            branches_total=max(report.summary.branches_total for _, report in reports),
        ))

    @staticmethod
    def _summary(report: CoverageReport | None, files_total: int = 0) -> dict:
        summary = report.summary if report else CoverageSummary()
        total_files = len(report.files) if report and report.files else files_total
        covered_files = sum(
            1 for source in (report.files if report else [])
            if any(int(line.get("count", 0)) > 0 for line in source.get("lines", []))
        )
        return {
            "lines": {"covered": summary.lines_covered, "total": summary.lines_total,
                      "percent": summary.line_percent},
            "functions": {"covered": summary.functions_covered, "total": summary.functions_total,
                          "percent": summary.function_percent},
            "branches": {"covered": summary.branches_covered, "total": summary.branches_total,
                         "percent": summary.branch_percent},
            "files": {"covered": covered_files, "total": total_files,
                      "percent": _percent(covered_files, total_files)},
        }

    def bootstrap(self) -> dict:
        tests = self.visible_tests()
        latest = self._latest_payloads()
        detection = self.detector.detect(self.config)
        result_tests = []
        for test in tests:
            item = test.to_dict()
            run = latest.get(test.id)
            coverage = _coverage_report(run.get("coverage")) if run else None
            raw_status = str(run.get("status", "pending")) if run else "pending"
            item["latest"] = {
                "status": self._display_run_status(raw_status) if run else "pending",
                "raw_status": raw_status,
                "run_id": str(run.get("id", "")) if run else "",
                "started_at": run.get("process", {}).get("started_at", "") if run else "",
                "duration": run.get("process", {}).get("duration_seconds", 0) if run else 0,
                "coverage": self._summary(coverage),
                "has_run": bool(run),
                "has_coverage": coverage is not None,
            }
            result_tests.append(item)
        active_reports = self.latest_reports([test.id for test in tests if test.active])
        combined = self._combined_report(active_reports)
        files_total = len({item.get("filename", "") for _, report in active_reports for item in report.files})
        workflow = self.workflow.load()
        if workflow.compiled and not self.config.resolved_executable.is_file():
            workflow = self.workflow.update(compiled=False)
        return {
            "project": {"name": self.config.resolved_project_root.name, "root": str(self.config.resolved_project_root)},
            "detection": detection.to_dict(),
            "build_ready": self.config.resolved_executable.is_file(),
            "workflow": workflow.to_dict(),
            "tests": result_tests,
            "summary": self._summary(combined, files_total),
            "generated_at": datetime.now(timezone.utc).isoformat(),
        }

    @staticmethod
    def _profile_stamp(report: CoverageReport) -> int:
        path = Path(report.profile_path) if report.profile_path else None
        return path.stat().st_mtime_ns if path and path.is_file() else 0

    def graph(self, test_ids: list[str]) -> dict:
        reports = self._required_reports(test_ids)
        signature = "|".join(
            f"{test_id}:{report.profile_path}:{self._profile_stamp(report)}"
            for test_id, report in reports
        )
        key = hashlib.sha256(signature.encode()).hexdigest()
        cached = self.graph_cache.get(key)
        if cached is not None:
            # Refresh recency so alternating between selections stays instant.
            self.graph_cache[key] = self.graph_cache.pop(key)
            return cached
        graph = FlowGraphBuilder().build_from_reports(reports, max_nodes=None)
        combined = self._combined_report(reports)
        files = {node["file"] for node in graph["nodes"] if node["file"]}
        statuses = {name: 0 for name in ("covered", "partial", "uncovered")}
        for node in graph["nodes"]:
            statuses[node["status"]] += 1
        names = self.test_names()
        contributions = [
            asdict(item) | {"name": names.get(item.test_id, item.test_id)}
            for item in incremental_contributions(reports)
        ]
        response = graph | {
            "summary": self._summary(combined, len(files)),
            "status_counts": statuses,
            "contributions": contributions,
            "selected_tests": [{"id": test_id, "name": names.get(test_id, test_id)} for test_id, _ in reports],
            "complete": True,
            "generated_at": datetime.now(timezone.utc).isoformat(),
        }
        if len(self.graph_cache) >= GRAPH_CACHE_LIMIT:
            self.graph_cache.pop(next(iter(self.graph_cache)))
        self.graph_cache[key] = response
        return response

    def compare(self, test_ids: list[str]) -> dict:
        reports = self._required_reports(test_ids, exact_count=2)
        graph = FlowGraphBuilder().build_from_reports(reports, max_nodes=None)
        left_id, right_id = test_ids
        for node in graph["nodes"]:
            hits = set(node["hit_tests"])
            if left_id in hits and right_id in hits:
                node["comparison"] = "common"
            elif left_id in hits:
                node["comparison"] = "left_only"
            elif right_id in hits:
                node["comparison"] = "right_only"
            else:
                node["comparison"] = "neither"
        report_by_id = dict(reports)
        names = self.test_names()
        left, right = report_by_id[left_id], report_by_id[right_id]
        combined = self._combined_report(reports)
        statuses = {name: 0 for name in ("covered", "partial", "uncovered")}
        for node in graph["nodes"]:
            statuses[node["status"]] += 1
        return graph | {
            "summary": self._summary(combined),
            "status_counts": statuses,
            "contributions": [],
            "selected_tests": [
                {"id": left_id, "name": names.get(left_id, left_id)},
                {"id": right_id, "name": names.get(right_id, right_id)},
            ],
            "comparison": {
                "left": {"id": left_id, "name": names.get(left_id, left_id), "summary": self._summary(left)},
                "right": {"id": right_id, "name": names.get(right_id, right_id), "summary": self._summary(right)},
                "delta": compare_reports(left, right),
            },
            "complete": True,
            "generated_at": datetime.now(timezone.utc).isoformat(),
        }

    def _project_source(self, filename: str) -> Path:
        candidate = Path(filename).resolve()
        shadow = (DATA_ROOT / "cache" / "shadow-source").resolve()
        try:
            relative = candidate.relative_to(shadow)
            candidate = (self.config.resolved_project_root / relative).resolve()
        except ValueError:
            pass
        candidate.relative_to(self.config.resolved_project_root)
        return candidate

    def source(self, filename: str, line: int, end_line: int, test_ids: list[str]) -> dict:
        path = self._project_source(filename)
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        reports = self._required_reports(test_ids)
        counts: dict[int, list[int]] = {}
        partial: set[int] = set()
        filenames = {filename, str(path)}
        shadow = DATA_ROOT / "cache" / "shadow-source"
        try:
            filenames.add(str(shadow / path.relative_to(self.config.resolved_project_root)))
        except ValueError:
            pass
        for _, report in reports:
            for source in report.files:
                if source.get("filename") not in filenames:
                    continue
                for item in source.get("lines", []):
                    number = int(item["line"])
                    counts.setdefault(number, []).append(int(item.get("count", 0)))
                    if item.get("partial"):
                        partial.add(number)
        center_end = max(line, end_line or line)
        start = max(1, line - 12) if line else 1
        stop = min(len(lines), max(center_end + 18, start + 58), start + 179)
        rendered = []
        for number in range(start, stop + 1):
            values = counts.get(number, [])
            count = sum(values)
            if number in partial or (values and any(value > 0 for value in values) and any(value == 0 for value in values)):
                status = "partial"
            elif count > 0:
                status = "covered"
            elif values:
                status = "uncovered"
            else:
                status = "neutral"
            rendered.append({"number": number, "text": lines[number - 1], "count": count, "status": status})
        return {"file": str(path), "relative_file": str(path.relative_to(self.config.resolved_project_root)),
                "start": start, "end": stop, "lines": rendered}

    def save_test(self, payload: dict) -> dict:
        """Register a single model. The test name always mirrors the model file name."""
        model_file = payload.get("model_file")
        if not model_file:
            raise ValueError("Select a .mr3 or .json model")
        test = self.uploads.build_test_case(
            decode_upload(model_file),
            suite=str(payload.get("suite", "custom")),
            test_id=str(payload.get("id", "")),
        )
        self.storage.save_test(test)
        self._invalidate_analysis_caches()
        return test.to_dict()

    def save_tests_batch(self, payload: dict) -> dict:
        """Register several models at once, naming each test after its input file.

        Every file is decoded before anything is written so a malformed entry cannot
        leave the batch half-registered.
        """
        entries = list(payload.get("model_files") or [])
        if not entries:
            raise ValueError("Select at least one .mr3 or .json model")
        suite = str(payload.get("suite", "custom"))
        uploads = [decode_upload(entry) for entry in entries]
        created = [self.uploads.build_test_case(upload, suite=suite) for upload in uploads]
        for test in created:
            self.storage.save_test(test)
        self._invalidate_analysis_caches()
        return {"created": [test.to_dict() for test in created], "count": len(created)}

    @staticmethod
    def _validation_errors(detection: object, backend: str) -> list[str]:
        report = detection
        errors = []
        if not report.valid_project:
            errors.append("The configured root does not contain a valid Marlim3 project")
        available = {tool.name: tool.available for tool in report.tools}
        required = ["cmake", "ninja", "gfortran"]
        required.extend(["clang++", "llvm-profdata", "llvm-cov"] if backend == "llvm" else ["g++", "gcov"])
        missing = [name for name in required if not available.get(name, False)]
        if missing:
            errors.append(f"Missing tools: {', '.join(missing)}")
        return errors

    def validate(self) -> dict:
        detection = self.detector.detect(self.config)
        errors = self._validation_errors(detection, self.config.coverage_backend)
        workflow = self.workflow.update(validated=not errors)
        return {
            "success": not errors,
            "message": "Project validated" if not errors else ". ".join(errors),
            "detection": detection.to_dict(),
            "workflow": workflow.to_dict(),
        }

    def reset_validation(self) -> dict:
        return {"workflow": self.workflow.update(validated=False).to_dict()}

    def reset_build(self) -> dict:
        return {"workflow": self.workflow.update(compiled=False).to_dict()}

    def delete_test(self, test_id: str) -> None:
        if self.storage.get_test(test_id) is None:
            raise ValueError("Project demos are read-only")
        self.storage.delete_test(test_id)
        upload_directory = ensure_within(DATA_ROOT / "inputs" / "models" / test_id, DATA_ROOT)
        if upload_directory.is_dir():
            shutil.rmtree(upload_directory)
        self._invalidate_analysis_caches()

    def _new_job(self, kind: str) -> dict:
        job = {"id": str(uuid.uuid4()), "kind": kind, "status": "running", "message": "", "log": [],
               "started_at": datetime.now(timezone.utc).isoformat(), "finished_at": ""}
        with self.jobs_lock:
            self.jobs[job["id"]] = job
        return job

    def _append_job_log(self, job: dict, text: str, stream: str = "status") -> None:
        with self.jobs_lock:
            job["log"].append({"stream": stream, "text": text})
            job["log"] = job["log"][-600:]

    @staticmethod
    def _run_status_label(status: str) -> str:
        return {
            "passed": "passed",
            "failed": "failed",
            "crashed": "crash",
            "timeout": "timeout",
            "cancelled": "cancelled",
            "configuration_error": "configuration error",
            "no_coverage": "no coverage",
            "internal_error": "internal error",
        }.get(status, status)

    def _append_terminal(self, job: dict, test_id: str, name: str, stream: str, text: str) -> None:
        if not text:
            return
        with self.jobs_lock:
            job["terminal"].append({"test_id": test_id, "name": name, "stream": stream, "text": text})
            job["terminal"] = job["terminal"][-5000:]

    def build(self) -> dict:
        self.workflow.update(compiled=False)
        job = self._new_job("build")

        def worker() -> None:
            def output(stream: str, value: str) -> None:
                with self.jobs_lock:
                    job["log"].append({"stream": stream, "text": value})
                    job["log"] = job["log"][-600:]
            try:
                result = ShadowSourceBuilder(self.config).build(output)
                job["status"] = "passed" if result.success else "failed"
                job["message"] = result.message
                self.workflow.update(compiled=result.success)
            except Exception as exc:  # surfaced in the job panel
                job["status"], job["message"] = "failed", str(exc)
                self.workflow.update(compiled=False)
            job["finished_at"] = datetime.now(timezone.utc).isoformat()
        threading.Thread(target=worker, daemon=True).start()
        return job

    def run_tests(self, test_ids: list[str]) -> dict:
        if not self.workflow.load().ready:
            raise ValueError("Validate and build the project before running tests")
        if not test_ids:
            raise ValueError("Select at least one test to run")
        if not self.config.resolved_executable.is_file():
            self.workflow.update(compiled=False)
            raise ValueError("The instrumented executable is unavailable. Build the project again")
        tests = []
        for test_id in test_ids:
            test = self.get_test(test_id)
            if not test:
                raise ValueError(f"Test not found: {test_id}")
            tests.append((test_id, test))
        job = self._new_job("run")
        job["items"] = [
            {
                "test_id": test_id,
                "name": test.name,
                "state": "pending",
                "result_status": "",
                "has_run": False,
                "has_coverage": False,
                "duration": 0.0,
                "exit_code": None,
            }
            for test_id, test in tests
        ]
        job["progress"] = {"completed": 0, "total": len(tests), "current_test_id": None}
        job["terminal"] = []
        job["message"] = f"Selection started: {len(tests)} test(s)"
        self._append_job_log(job, f"Selection started with {len(tests)} test(s).")
        self._append_job_log(job, "Execution queue:")
        for index, (_, test) in enumerate(tests, start=1):
            self._append_job_log(job, f"[{index}/{len(tests)}] Pending: {test.name}")

        def worker() -> None:
            results = []
            try:
                provider = coverage_provider(self.config)
                runner = TestRunner(self.config, provider)
                for index, (test_id, test) in enumerate(tests, start=1):
                    with self.jobs_lock:
                        item = job["items"][index - 1]
                        item["state"] = "running"
                        job["progress"]["current_test_id"] = test_id
                        job["message"] = f"[{index}/{len(tests)}] Running {test.name}"
                    self._append_job_log(job, f"[{index}/{len(tests)}] Starting: {test.name}")
                    self._append_terminal(job, test_id, test.name, "status", f"\n[{index}/{len(tests)}] {test.name}\n")
                    try:
                        # Bind the current item so the callback cannot observe a
                        # later loop iteration.
                        def terminal_output(stream: str, text: str,
                                            current_id: str = test_id,
                                            current_name: str = test.name) -> None:
                            self._append_terminal(job, current_id, current_name, stream, text)

                        record = runner.run(test, on_output=terminal_output)
                        self.storage.save_run(record)
                        status = record.status.value
                        duration = float(record.process.duration_seconds)
                        exit_code = record.process.exit_code
                        has_coverage = record.coverage is not None
                        result = {
                            "test_id": test_id,
                            "name": test.name,
                            "status": status,
                            "has_run": True,
                            "has_coverage": has_coverage,
                            "duration": duration,
                            "exit_code": exit_code,
                        }
                        results.append(result)
                        with self.jobs_lock:
                            item.update({
                                "state": "completed",
                                "result_status": status,
                                "has_run": True,
                                "has_coverage": has_coverage,
                                "duration": duration,
                                "exit_code": exit_code,
                            })
                            job["progress"]["completed"] = index
                            job["progress"]["current_test_id"] = None
                        coverage_text = "available" if has_coverage else "unavailable"
                        exit_text = "not available" if exit_code is None else str(exit_code)
                        self._append_job_log(
                            job,
                            f"[{index}/{len(tests)}] Finished: {test.name} | "
                            f"status: {self._run_status_label(status)} | exit: {exit_text} | "
                            f"duration: {duration:.2f}s | coverage: {coverage_text} | "
                            f"remaining: {len(tests) - index}",
                        )
                        self._append_terminal(
                            job, test_id, test.name, "status",
                            f"[{self._run_status_label(status)} | {duration:.2f}s]\n",
                        )
                    except Exception as exc:  # keep the remaining selection moving
                        results.append({
                            "test_id": test_id, "name": test.name, "status": "crashed",
                            "has_run": True, "has_coverage": False, "duration": 0.0, "exit_code": None,
                        })
                        with self.jobs_lock:
                            item.update({
                                "state": "error", "result_status": "crashed", "has_run": True,
                                "has_coverage": False, "duration": 0.0, "exit_code": None,
                            })
                            job["progress"]["completed"] = index
                            job["progress"]["current_test_id"] = None
                        self._append_job_log(
                            job,
                            f"[{index}/{len(tests)}] Crash: {test.name} | {exc}",
                            "error",
                        )
                        self._append_terminal(
                            job, test_id, test.name, "stderr", f"[crashed] {exc}\n",
                        )
                    # Stored run results changed; drop only the coverage-derived caches.
                    self.graph_cache.clear()
                    self.reports_cache.clear()
                with self.jobs_lock:
                    job["status"] = "passed" if results and all(item["status"] == "passed" for item in results) else "failed"
                    job["result"] = results
                    job["message"] = f"Selection finished: {len(results)}/{len(tests)} test(s) processed"
                self._append_job_log(
                    job,
                    f"Selection finished. {len(results)} of {len(tests)} test(s) processed.",
                )
            except Exception as exc:  # surfaced in the job panel
                with self.jobs_lock:
                    job["status"], job["message"] = "failed", str(exc)
                    job["progress"]["current_test_id"] = None
                self._append_job_log(job, f"Execution interrupted: {exc}", "error")
            with self.jobs_lock:
                job["finished_at"] = datetime.now(timezone.utc).isoformat()
        threading.Thread(target=worker, daemon=True).start()
        return job

    def simulation_log(self, test_id: str) -> dict:
        test = self.get_test(test_id)
        if not test:
            raise ValueError("Test not found")
        payloads = self.storage.latest_run_payloads([test_id])
        if not payloads:
            raise ValueError("This test has not been run yet")
        payload = payloads[0]
        artifact_value = str(payload.get("artifact_directory", ""))
        if not artifact_value:
            return {
                "test_id": test_id, "name": test.name, "status": "crashed",
                "available": False, "file": "", "content": "simulacao.log was not generated.", "truncated": False,
            }
        run_root = ensure_within(Path(artifact_value), DATA_ROOT)
        log_path = ensure_within(run_root / "work" / "output" / "simulacao.log", run_root)
        raw_status = str(payload.get("status", "crashed"))
        if not log_path.is_file():
            return {
                "test_id": test_id, "name": test.name, "status": self._display_run_status(raw_status),
                "available": False, "file": str(log_path.relative_to(DATA_ROOT)),
                "content": "simulacao.log was not generated by this run.", "truncated": False,
            }
        content = log_path.read_bytes()
        limit = 5 * 1024 * 1024
        truncated = len(content) > limit
        if truncated:
            content = content[-limit:]
        return {
            "test_id": test_id,
            "name": test.name,
            "status": self._display_run_status(raw_status),
            "available": True,
            "file": str(log_path.relative_to(DATA_ROOT)),
            "content": content.decode("utf-8", errors="replace"),
            "truncated": truncated,
        }

    def get_job(self, job_id: str) -> dict | None:
        with self.jobs_lock:
            value = self.jobs.get(job_id)
            return json.loads(json.dumps(value)) if value else None


class RegressionRequestHandler(SimpleHTTPRequestHandler):
    service: RegressionService

    def log_message(self, format: str, *args: object) -> None:
        return

    def _json(self, payload: object, status: int = HTTPStatus.OK) -> None:
        encoded = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        try:
            self.send_response(status)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Content-Length", str(len(encoded)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(encoded)
        except (BrokenPipeError, ConnectionResetError):
            return

    def _body(self) -> dict:
        length = int(self.headers.get("Content-Length", "0"))
        return json.loads(self.rfile.read(length).decode("utf-8")) if length else {}

    @staticmethod
    def _ids(query: dict[str, list[str]]) -> list[str]:
        return [item for value in query.get("test_ids", []) for item in value.split(",") if item]

    def do_GET(self) -> None:
        parsed = urlparse(self.path)
        query = parse_qs(parsed.query)
        try:
            if parsed.path in {"/api/app-icon", "/favicon.ico"}:
                icon = browser_icon_path() if parsed.path == "/api/app-icon" else favicon_path()
                content = icon.read_bytes()
                self.send_response(HTTPStatus.OK)
                self.send_header("Content-Type", mimetypes.guess_type(icon.name)[0] or "image/png")
                self.send_header("Content-Length", str(len(content)))
                self.send_header("Cache-Control", "no-store")
                self.end_headers()
                self.wfile.write(content)
                return
            if parsed.path == "/api/project/logo":
                logo = self.service.project_logo()
                if logo is None:
                    return self.send_error(HTTPStatus.NOT_FOUND)
                content = logo.read_bytes()
                self.send_response(HTTPStatus.OK)
                self.send_header("Content-Type", mimetypes.guess_type(logo.name)[0] or "image/svg+xml")
                self.send_header("Content-Length", str(len(content)))
                self.send_header("Cache-Control", "no-cache")
                self.end_headers()
                self.wfile.write(content)
                return
            if parsed.path == "/api/bootstrap":
                return self._json(self.service.bootstrap())
            if parsed.path == "/api/graph":
                return self._json(self.service.graph(self._ids(query)))
            if parsed.path == "/api/source":
                return self._json(self.service.source(
                    unquote(query.get("file", [""])[0]), int(query.get("line", ["1"])[0]),
                    int(query.get("end_line", ["0"])[0]), self._ids(query),
                ))
            parts = parsed.path.strip("/").split("/")
            if len(parts) == 4 and parts[:2] == ["api", "tests"] and parts[3] == "simulation-log":
                return self._json(self.service.simulation_log(unquote(parts[2])))
            if parsed.path.startswith("/api/jobs/"):
                job = self.service.get_job(parsed.path.rsplit("/", 1)[-1])
                return self._json(job or {"error": "Job not found"}, HTTPStatus.OK if job else HTTPStatus.NOT_FOUND)
            if parsed.path.startswith("/api/"):
                return self._json({"error": ENDPOINT_NOT_FOUND}, HTTPStatus.NOT_FOUND)
            return self._static(parsed.path)
        except (OSError, ValueError, RuntimeError, json.JSONDecodeError) as exc:
            return self._json({"error": str(exc)}, HTTPStatus.BAD_REQUEST)

    def do_POST(self) -> None:
        parsed = urlparse(self.path)
        try:
            body = self._body()
            if parsed.path == "/api/validate":
                return self._json(self.service.validate())
            if parsed.path == "/api/validate/reset":
                return self._json(self.service.reset_validation())
            if parsed.path == "/api/build":
                return self._json(self.service.build(), HTTPStatus.ACCEPTED)
            if parsed.path == "/api/build/reset":
                return self._json(self.service.reset_build())
            if parsed.path == "/api/run":
                return self._json(self.service.run_tests(list(body.get("test_ids", []))), HTTPStatus.ACCEPTED)
            if parsed.path == "/api/compare":
                return self._json(self.service.compare(list(body.get("test_ids", []))))
            if parsed.path == "/api/tests":
                return self._json(self.service.save_test(body), HTTPStatus.CREATED)
            if parsed.path == "/api/tests/batch":
                return self._json(self.service.save_tests_batch(body), HTTPStatus.CREATED)
            return self._json({"error": ENDPOINT_NOT_FOUND}, HTTPStatus.NOT_FOUND)
        except (OSError, ValueError, RuntimeError, json.JSONDecodeError) as exc:
            return self._json({"error": str(exc)}, HTTPStatus.BAD_REQUEST)

    def do_DELETE(self) -> None:
        parsed = urlparse(self.path)
        if parsed.path.startswith("/api/tests/"):
            self.service.delete_test(parsed.path.rsplit("/", 1)[-1])
            return self._json({"deleted": True})
        self._json({"error": ENDPOINT_NOT_FOUND}, HTTPStatus.NOT_FOUND)

    def _static(self, request_path: str) -> None:
        relative = unquote(request_path).lstrip("/") or "index.html"
        path = (FRONTEND_DIST / relative).resolve()
        try:
            path.relative_to(FRONTEND_DIST.resolve())
        except ValueError:
            return self.send_error(HTTPStatus.FORBIDDEN)
        if not path.is_file():
            path = FRONTEND_DIST / "index.html"
        content = path.read_bytes()
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", mimetypes.guess_type(path.name)[0] or "application/octet-stream")
        self.send_header("Content-Length", str(len(content)))
        self.end_headers()
        self.wfile.write(content)


def _chrome_executable() -> str | None:
    """Locate a Chromium-family browser on Linux, Windows or macOS."""
    names = (
        "google-chrome", "google-chrome-stable", "chromium", "chromium-browser",
        "chrome", "msedge",
    )
    for name in names:
        found = shutil.which(name)
        if found:
            return found
    system = platform.system()
    if system == "Windows":
        roots = [os.environ.get(key, "") for key in ("PROGRAMFILES", "PROGRAMFILES(X86)", "LOCALAPPDATA")]
        candidates = [
            Path(root) / vendor
            for root in roots if root
            for vendor in (
                Path("Google/Chrome/Application/chrome.exe"),
                Path("Microsoft/Edge/Application/msedge.exe"),
            )
        ]
    elif system == "Darwin":
        candidates = [
            Path("/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"),
            Path("/Applications/Chromium.app/Contents/MacOS/Chromium"),
            Path("/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge"),
        ]
    else:
        return None
    return next((str(path) for path in candidates if path.is_file()), None)


def _open_app(url: str) -> subprocess.Popen | None:
    chrome = _chrome_executable()
    if chrome:
        prepare_linux_desktop_icon(chrome)
        profile = DATA_ROOT / f"browser-profile-{browser_icon_version()}"
        profile.mkdir(parents=True, exist_ok=True)
        browser_log_path = DATA_ROOT / "logs" / "browser.log"
        browser_log_path.parent.mkdir(parents=True, exist_ok=True)
        if browser_log_path.is_file() and browser_log_path.stat().st_size > 2 * 1024 * 1024:
            browser_log_path.unlink()
        command = [
            chrome,
            f"--app={url}",
            f"--user-data-dir={profile}",
            *chrome_window_flags(),
            "--no-first-run",
            "--no-default-browser-check",
            "--disable-gpu",
            "--disable-background-networking",
            "--disable-component-update",
            "--disable-default-apps",
            "--disable-sync",
            "--disable-features=MediaRouter,OptimizationHints,PushMessaging,NotificationTriggers",
        ]
        browser_log = browser_log_path.open("ab")
        try:
            browser_log.write(f"\n[{datetime.now(timezone.utc).isoformat()}] Opening Chrome in application mode\n".encode())
            browser_log.flush()
            # Detach so closing the terminal does not kill the window; the flag
            # differs between POSIX and Windows.
            detach = ({"creationflags": subprocess.CREATE_NEW_PROCESS_GROUP}
                      if os.name == "nt" else {"start_new_session": True})
            return subprocess.Popen(
                command,
                stdout=browser_log,
                stderr=subprocess.STDOUT,
                **detach,
            )
        finally:
            browser_log.close()
    webbrowser.open(url)
    return None


def main() -> None:
    if not (FRONTEND_DIST / "index.html").is_file():
        from .bootstrap import ensure_frontend_assets
        ensure_frontend_assets()
    service = RegressionService()
    RegressionRequestHandler.service = service
    server = ThreadingHTTPServer(("127.0.0.1", 0), RegressionRequestHandler)
    url = f"http://127.0.0.1:{server.server_port}"
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    process = _open_app(url)
    try:
        if process:
            process.wait()
        else:
            thread.join()
    except KeyboardInterrupt:
        pass
    finally:
        server.shutdown()
        server.server_close()


if __name__ == "__main__":
    main()
