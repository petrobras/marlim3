from pathlib import Path
import shutil
import sys
import unittest
import uuid

from marlim_regression.build import FixtureBuilder
from marlim_regression.config import DATA_ROOT, ToolConfig, initialize_data_directories
from marlim_regression.coverage import LLVMCoverageProvider, parse_llvm_export
from marlim_regression.process import ProcessExecutor


class CoverageParserTests(unittest.TestCase):
    def test_parses_export(self):
        payload = {"data": [{"totals": {"lines": {"covered": 1, "count": 2}, "functions": {"covered": 1, "count": 1}, "branches": {"covered": 1, "count": 2}}, "files": [{"filename": "x.cpp", "segments": [[2, 1, 4, True, True, False], [3, 1, 0, True, True, False]]}], "functions": [{"name": "run", "filenames": ["x.cpp"], "regions": [[2, 1, 4, 2, 4, 0, 0, 0]]}]}]}
        report = parse_llvm_export(payload)
        self.assertEqual(report.summary.line_percent, 50.0)
        self.assertEqual(report.covered_function_keys(), {"run"})


@unittest.skipUnless(shutil.which("clang++") and shutil.which("llvm-profdata") and shutil.which("llvm-cov"), "LLVM coverage tools unavailable")
class LLVMIntegrationTests(unittest.TestCase):
    def test_two_fixture_profiles_merge(self):
        initialize_data_directories()
        config = ToolConfig(project_root=str(Path(__file__).resolve().parents[2]))
        source = Path(__file__).parent / "fixtures" / "coverage_fixture.cpp"
        executable = DATA_ROOT / "build" / "fixture-test" / ("fixture.exe" if sys.platform.startswith("win") else "fixture")
        built = FixtureBuilder(config).build(source, executable)
        self.assertEqual(built.exit_code, 0, built.stderr)
        provider = LLVMCoverageProvider(config)
        profiles = []
        reports = []
        for mode in ("positive", "negative"):
            run_id = f"test-{mode}-{uuid.uuid4()}"
            run_dir = DATA_ROOT / "runs" / run_id
            result = ProcessExecutor().run([str(executable), mode], run_dir, provider.environment_for_run(run_id, run_dir), timeout=10)
            self.assertEqual(result.exit_code, 0)
            coverage = provider.collect(executable, run_dir)
            self.assertIsNotNone(coverage)
            profiles.append(Path(coverage.profile_path))
            reports.append(coverage)
        combined = provider.merge(executable, profiles, DATA_ROOT / "coverage" / f"test-combined-{uuid.uuid4()}")
        self.assertGreaterEqual(combined.summary.lines_covered, max(item.summary.lines_covered for item in reports))
        names = " ".join(combined.covered_function_keys())
        self.assertIn("positive_path", names)
        self.assertIn("negative_path", names)

        timeout_dir = DATA_ROOT / "runs" / f"test-timeout-{uuid.uuid4()}"
        timed_out = ProcessExecutor().run([str(executable), "timeout"], timeout_dir, provider.environment_for_run("timeout", timeout_dir), timeout=0.05)
        self.assertTrue(timed_out.timed_out)

        failure_dir = DATA_ROOT / "runs" / f"test-failure-{uuid.uuid4()}"
        failed = ProcessExecutor().run([str(executable), "fail"], failure_dir, provider.environment_for_run("failure", failure_dir), timeout=10)
        self.assertEqual(failed.exit_code, 3)
        self.assertIn("requested failure", failed.stderr)


if __name__ == "__main__":
    unittest.main()
