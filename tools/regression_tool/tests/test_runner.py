from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from marlim_regression.config import ToolConfig
from marlim_regression.domain import CoverageReport, CoverageSummary, ProcessResult, RunStatus, TestCase as RegressionCase
from marlim_regression.runner import TestRunner as RegressionRunner


class FakeCoverage:
    def environment_for_run(self, run_id, run_dir):
        return {}

    def collect(self, executable, run_dir):
        return CoverageReport(CoverageSummary(lines_covered=1, lines_total=1))


class FakeExecutor:
    def __init__(self, exit_code=0):
        self.exit_code = exit_code
        self.timeout = "not-called"

    def run(self, command, cwd, environment=None, timeout=None, on_output=None):
        self.timeout = timeout
        if on_output:
            on_output("stdout", "live output\n")
        return ProcessResult(
            command, str(cwd), self.exit_code, "live output\n", "",
            "start", "finish", 0.1, crashed=False,
        )

    def cancel(self):
        return False


class RunnerTests(unittest.TestCase):
    def configured_runner(self, root: Path, exit_code: int):
        project = root / "project"
        project.mkdir()
        (project / "model.mr3").write_text("model", encoding="utf-8")
        executable = root / "Marlim3"
        executable.write_text("binary", encoding="utf-8")
        config = ToolConfig(project_root=str(project), executable_path=str(executable))
        executor = FakeExecutor(exit_code)
        return RegressionRunner(config, FakeCoverage(), executor=executor), executor

    def test_model_execution_has_no_timeout_and_streams_output(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            runner, executor = self.configured_runner(root, 0)
            streamed = []
            with patch("marlim_regression.runner.DATA_ROOT", root / ".data"):
                record = runner.run(
                    RegressionCase("model", "model.mr3"),
                    on_output=lambda stream, text: streamed.append((stream, text)),
                )
        self.assertIsNone(executor.timeout)
        self.assertEqual(record.status, RunStatus.PASSED)
        self.assertEqual(streamed, [("stdout", "live output\n")])

    def test_any_failed_execution_is_reported_as_crashed(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            runner, _ = self.configured_runner(root, 1)
            with patch("marlim_regression.runner.DATA_ROOT", root / ".data"):
                record = runner.run(RegressionCase("model", "model.mr3"))
        self.assertEqual(record.status, RunStatus.CRASHED)


if __name__ == "__main__":
    unittest.main()
