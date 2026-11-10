from __future__ import annotations

from abc import ABC, abstractmethod
import json
from pathlib import Path
import subprocess
from typing import Iterable

from .config import DATA_ROOT, ToolConfig
from .domain import CoverageReport, CoverageSummary, ensure_within
from .process import ProcessExecutor


class CoverageProvider(ABC):
    @abstractmethod
    def environment_for_run(self, run_id: str, run_dir: Path) -> dict[str, str]: ...

    @abstractmethod
    def collect(self, executable: Path, run_dir: Path) -> CoverageReport | None: ...

    @abstractmethod
    def merge(self, executable: Path, profiles: Iterable[Path], output_dir: Path) -> CoverageReport: ...


class LLVMCoverageProvider(CoverageProvider):
    def __init__(self, config: ToolConfig, executor: ProcessExecutor | None = None):
        self.config = config
        self.executor = executor or ProcessExecutor()

    def environment_for_run(self, run_id: str, run_dir: Path) -> dict[str, str]:
        profile = run_dir / "coverage" / f"{run_id}-%p.profraw"
        profile.parent.mkdir(parents=True, exist_ok=True)
        return {"LLVM_PROFILE_FILE": str(profile)}

    def _merge_raw(self, profiles: Iterable[Path], destination: Path) -> None:
        profile_list = [str(path) for path in profiles if path.is_file()]
        if not profile_list:
            raise FileNotFoundError("No LLVM raw/profile data found")
        destination.parent.mkdir(parents=True, exist_ok=True)
        command = [self.config.llvm_profdata, "merge", "-sparse", *profile_list, "-o", str(destination)]
        result = self.executor.run(command, destination.parent, timeout=120)
        if result.exit_code != 0:
            raise RuntimeError(f"llvm-profdata failed: {result.stderr or result.stdout}")

    def _export(self, executable: Path, profile: Path, destination: Path) -> CoverageReport:
        command = [self.config.llvm_cov, "export", str(executable), f"-instr-profile={profile}"]
        result = self.executor.run(command, destination.parent, timeout=180)
        if result.exit_code != 0:
            raise RuntimeError(f"llvm-cov export failed: {result.stderr or result.stdout}")
        destination.write_text(result.stdout, encoding="utf-8")
        report = parse_llvm_export(json.loads(result.stdout))
        report.raw_path = str(destination)
        report.profile_path = str(profile)
        return report

    def collect(self, executable: Path, run_dir: Path) -> CoverageReport | None:
        raw = sorted((run_dir / "coverage").glob("*.profraw"))
        if not raw:
            return None
        profile = run_dir / "coverage" / "merged.profdata"
        self._merge_raw(raw, profile)
        return self._export(executable, profile, run_dir / "coverage" / "coverage.json")

    def merge(self, executable: Path, profiles: Iterable[Path], output_dir: Path) -> CoverageReport:
        ensure_within(output_dir, DATA_ROOT)
        output_dir.mkdir(parents=True, exist_ok=True)
        profile = output_dir / "combined.profdata"
        self._merge_raw(profiles, profile)
        return self._export(executable, profile, output_dir / "coverage.json")


class GcovCoverageProvider(CoverageProvider):
    """Portable interface placeholder for GCC builds; JSON collection requires gcovr."""

    def __init__(self, config: ToolConfig):
        self.config = config

    def environment_for_run(self, run_id: str, run_dir: Path) -> dict[str, str]:
        return {"GCOV_PREFIX": str(run_dir / "coverage"), "GCOV_PREFIX_STRIP": "0"}

    def collect(self, executable: Path, run_dir: Path) -> CoverageReport | None:
        raise RuntimeError("GCC execution is supported, but structured gcov collection requires gcovr; use LLVM for the full UI")

    def merge(self, executable: Path, profiles: Iterable[Path], output_dir: Path) -> CoverageReport:
        raise RuntimeError("GCC profile merge is not available without gcovr")


def parse_llvm_export(payload: dict) -> CoverageReport:
    data = (payload.get("data") or [{}])[0]
    totals = data.get("totals", {})
    lines_total = totals.get("lines", {})
    functions_total = totals.get("functions", {})
    branches_total = totals.get("branches", {})
    summary = CoverageSummary(
        lines_covered=int(lines_total.get("covered", 0)), lines_total=int(lines_total.get("count", 0)),
        functions_covered=int(functions_total.get("covered", 0)), functions_total=int(functions_total.get("count", 0)),
        branches_covered=int(branches_total.get("covered", 0)), branches_total=int(branches_total.get("count", 0)),
    )
    files = []
    for source in data.get("files", []):
        line_counts: dict[int, int] = {}
        partial: set[int] = set()
        for segment in source.get("segments", []):
            if len(segment) < 5 or not segment[3]:
                continue
            line, count = int(segment[0]), int(segment[2])
            previous = line_counts.get(line)
            line_counts[line] = max(count, previous or 0)
            if previous is not None and (previous == 0) != (count == 0):
                partial.add(line)
        file_summary = source.get("summary", {})
        files.append({
            "filename": source.get("filename", ""),
            "summary": file_summary,
            "lines": [{"line": line, "count": count, "partial": line in partial} for line, count in sorted(line_counts.items())],
            "branches": source.get("branches", []),
        })
    functions = []
    for function in data.get("functions", []):
        regions = function.get("regions", [])
        count = max((int(region[4]) for region in regions if len(region) > 4), default=0)
        functions.append({
            "name": function.get("name", ""), "count": count,
            "filenames": function.get("filenames", []), "regions": regions,
        })
    return CoverageReport(summary=summary, files=files, functions=functions)


def coverage_provider(config: ToolConfig) -> CoverageProvider:
    return LLVMCoverageProvider(config) if config.coverage_backend == "llvm" else GcovCoverageProvider(config)

