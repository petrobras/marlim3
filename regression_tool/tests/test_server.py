import base64
from pathlib import Path
import subprocess
from types import SimpleNamespace
import tempfile
import threading
import time
import unittest
from unittest.mock import patch

from marlim_regression.domain import CoverageReport, CoverageSummary
from marlim_regression.icons import WINDOW_CLASS, browser_icon_path, favicon_path, native_icon_path
from marlim_regression.server import RegressionService, _open_app
from marlim_regression.uploads import ModelUploadStore, decode_upload
from marlim_regression.workflow import WorkflowStateStore


def coverage(functions: list[tuple[str, int]]) -> CoverageReport:
    return CoverageReport(
        CoverageSummary(lines_covered=1, lines_total=2, functions_covered=1, functions_total=2),
        functions=[{"name": name, "count": count, "regions": [[1, 1, 1, 2, count, 0, 0, 0]]} for name, count in functions],
    )


class RegressionServiceTests(unittest.TestCase):
    def test_platform_icons_are_available_in_the_expected_formats(self):
        self.assertEqual(native_icon_path("Linux").suffix, ".png")
        self.assertEqual(native_icon_path("Windows").suffix, ".ico")
        self.assertEqual(native_icon_path("Darwin").suffix, ".icns")
        self.assertEqual(native_icon_path("Linux").name, "app-icon-runtime.png")
        self.assertEqual(native_icon_path("Windows").name, "app-icon-runtime.ico")
        self.assertEqual(browser_icon_path().suffix, ".png")
        self.assertEqual(favicon_path("Windows").suffix, ".ico")

    def test_project_logo_is_read_from_the_current_marlim_checkout(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            logo = root / "assets" / "branding" / "logo.svg"
            logo.parent.mkdir(parents=True)
            logo.write_text("<svg/>", encoding="utf-8")
            service = RegressionService.__new__(RegressionService)
            service.config = SimpleNamespace(resolved_project_root=root)

            self.assertEqual(service.project_logo(), logo)

    def test_chrome_app_uses_quiet_isolated_runtime_flags(self):
        with tempfile.TemporaryDirectory() as temporary:
            data_root = Path(temporary) / ".data"
            with (
                patch("marlim_regression.server.DATA_ROOT", data_root),
                patch("marlim_regression.server.shutil.which", return_value="/usr/bin/chrome"),
                patch("marlim_regression.server.prepare_linux_desktop_icon") as prepare_icon,
                patch("marlim_regression.server.subprocess.Popen") as popen,
            ):
                process = _open_app("http://127.0.0.1:1234")

            self.assertIs(process, popen.return_value)
            command = popen.call_args.args[0]
            self.assertIn("--disable-gpu", command)
            self.assertIn("--disable-background-networking", command)
            self.assertIn(f"--class={WINDOW_CLASS}", command)
            prepare_icon.assert_called_once_with("/usr/bin/chrome")
            self.assertEqual(popen.call_args.kwargs["stderr"], subprocess.STDOUT)
            self.assertTrue(popen.call_args.kwargs["start_new_session"])
            self.assertTrue((data_root / "logs" / "browser.log").is_file())

    def test_default_models_are_read_directly_from_project_on_each_call(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "demos" / "pt-br").mkdir(parents=True)
            (root / "demos" / "english.mr3").write_text("model", encoding="utf-8")
            (root / "demos" / "pt-br" / "portugues.mr3").write_text("modelo", encoding="utf-8")
            service = RegressionService.__new__(RegressionService)
            service.config = SimpleNamespace(resolved_project_root=root)
            service.storage = SimpleNamespace(list_tests=lambda: [])

            self.assertEqual([test.model_path for test in service.visible_tests()], ["demos/english.mr3"])
            (root / "demos" / "new.json").write_text("{}", encoding="utf-8")
            self.assertEqual(
                {test.model_path for test in service.visible_tests()},
                {"demos/english.mr3", "demos/new.json"},
            )

    def test_upload_is_copied_under_tool_data(self):
        with tempfile.TemporaryDirectory() as temporary:
            data_root = Path(temporary) / "user-data"
            encoded = base64.b64encode(b"model-data").decode("ascii")
            upload = decode_upload({"name": "sample.mr3", "content_base64": encoded})
            relative = ModelUploadStore(data_root).store("test-id", upload)

            self.assertEqual(relative, "@data/inputs/models/test-id/sample.mr3")
            self.assertEqual((data_root / "inputs/models/test-id/sample.mr3").read_bytes(), b"model-data")

    def test_uploaded_test_is_named_after_its_model_file(self):
        with tempfile.TemporaryDirectory() as temporary:
            encoded = base64.b64encode(b"model-data").decode("ascii")
            upload = decode_upload({"name": "simplified-production.mr3", "content_base64": encoded})
            test = ModelUploadStore(Path(temporary)).build_test_case(upload, suite="custom")

            self.assertEqual(test.name, "simplified-production")
            self.assertEqual(test.suite, "custom")

    def test_batch_registration_creates_one_test_per_model(self):
        with tempfile.TemporaryDirectory() as temporary:
            data_root = Path(temporary) / "user-data"
            saved: list = []
            service = RegressionService.__new__(RegressionService)
            service.uploads = ModelUploadStore(data_root)
            service.storage = SimpleNamespace(save_test=saved.append)
            service.graph_cache, service.reports_cache = {}, {}
            encoded = base64.b64encode(b"model-data").decode("ascii")

            result = service.save_tests_batch({"model_files": [
                {"name": "first.mr3", "content_base64": encoded},
                {"name": "second.json", "content_base64": encoded},
            ]})

            self.assertEqual(result["count"], 2)
            self.assertEqual([test.name for test in saved], ["first", "second"])

    def test_batch_rejects_every_model_when_one_entry_is_invalid(self):
        with tempfile.TemporaryDirectory() as temporary:
            data_root = Path(temporary) / "user-data"
            saved: list = []
            service = RegressionService.__new__(RegressionService)
            service.uploads = ModelUploadStore(data_root)
            service.storage = SimpleNamespace(save_test=saved.append)
            service.graph_cache, service.reports_cache = {}, {}
            encoded = base64.b64encode(b"model-data").decode("ascii")

            with self.assertRaisesRegex(ValueError, "notes.txt"):
                service.save_tests_batch({"model_files": [
                    {"name": "valid.mr3", "content_base64": encoded},
                    {"name": "notes.txt", "content_base64": encoded},
                ]})

            self.assertEqual(saved, [])

    def test_run_is_rejected_until_workflow_is_ready(self):
        with tempfile.TemporaryDirectory() as temporary:
            service = RegressionService.__new__(RegressionService)
            service.workflow = WorkflowStateStore(Path(temporary) / "workflow.json")
            with self.assertRaisesRegex(ValueError, "Validate and build"):
                service.run_tests(["test"])

    def test_run_job_reports_each_test_progress_and_result(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            executable = root / "Marlim3"
            executable.write_text("binary", encoding="utf-8")
            service = RegressionService.__new__(RegressionService)
            service.workflow = WorkflowStateStore(root / "workflow.json")
            service.workflow.update(validated=True, compiled=True)
            service.config = SimpleNamespace(resolved_executable=executable)
            tests = {
                "a": SimpleNamespace(id="a", name="Model A"),
                "b": SimpleNamespace(id="b", name="Model B"),
            }
            saved = []
            service.storage = SimpleNamespace(get_test=tests.get, save_run=saved.append)
            service.jobs = {}
            service.jobs_lock = threading.Lock()
            service.graph_cache = {"old": {}}
            service.reports_cache = {("old",): []}

            class FakeRunner:
                def __init__(self, config, provider):
                    pass

                def run(self, test, on_output=None):
                    time.sleep(0.01)
                    if on_output:
                        on_output("stdout", f"running {test.name}\n")
                    return SimpleNamespace(
                        status=SimpleNamespace(value="passed"),
                        process=SimpleNamespace(duration_seconds=0.25, exit_code=0),
                        coverage=object(),
                    )

            with (
                patch("marlim_regression.server.coverage_provider", return_value=object()),
                patch("marlim_regression.server.TestRunner", FakeRunner),
            ):
                job = service.run_tests(["a", "b"])
                deadline = time.time() + 2
                while job["status"] == "running" and time.time() < deadline:
                    time.sleep(0.01)

            self.assertEqual(job["status"], "passed")
            self.assertEqual(job["progress"], {"completed": 2, "total": 2, "current_test_id": None})
            self.assertEqual([item["state"] for item in job["items"]], ["completed", "completed"])
            self.assertTrue(all(item["has_run"] and item["has_coverage"] for item in job["items"]))
            log = "\n".join(item["text"] for item in job["log"])
            self.assertIn("[1/2] Pending: Model A", log)
            self.assertIn("[2/2] Pending: Model B", log)
            self.assertIn("[1/2] Starting: Model A", log)
            self.assertIn("[1/2] Finished: Model A | status: passed", log)
            self.assertIn("[2/2] Starting: Model B", log)
            self.assertIn("Selection finished", log)
            self.assertEqual(len(saved), 2)
            self.assertTrue(any(item["stream"] == "stdout" and "running Model A" in item["text"] for item in job["terminal"]))

    def test_simulation_log_reads_latest_run_and_normalizes_legacy_timeout(self):
        with tempfile.TemporaryDirectory() as temporary:
            data_root = Path(temporary) / ".data"
            run_root = data_root / "runs" / "run-id"
            log_path = run_root / "work" / "output" / "simulacao.log"
            log_path.parent.mkdir(parents=True)
            log_path.write_text("line one\nline two\n", encoding="utf-8")
            service = RegressionService.__new__(RegressionService)
            service.storage = SimpleNamespace(
                get_test=lambda test_id: SimpleNamespace(id=test_id, name="Model"),
                latest_run_payloads=lambda ids: [{
                    "id": "run-id", "test_id": ids[0], "status": "timeout",
                    "artifact_directory": str(run_root),
                }],
            )
            with patch("marlim_regression.server.DATA_ROOT", data_root):
                result = service.simulation_log("test-id")

            self.assertTrue(result["available"])
            self.assertEqual(result["status"], "crashed")
            self.assertEqual(result["content"], "line one\nline two\n")

    def test_compare_marks_common_and_exclusive_functions(self):
        service = RegressionService.__new__(RegressionService)
        reports = [
            ("left", coverage([("common", 1), ("left_only", 1), ("right_only", 0)])),
            ("right", coverage([("common", 1), ("left_only", 0), ("right_only", 1)])),
        ]
        service.latest_reports = lambda ids: reports
        service.storage = SimpleNamespace(list_tests=lambda: [
            SimpleNamespace(id="left", name="Left", model_path="left.mr3", suite="custom"),
            SimpleNamespace(id="right", name="Right", model_path="right.mr3", suite="custom"),
        ])
        service.config = SimpleNamespace(resolved_executable=Path("/missing"), resolved_project_root=Path("/missing"))

        result = service.compare(["left", "right"])
        membership = {node["id"]: node["comparison"] for node in result["nodes"]}
        self.assertEqual(membership["common"], "common")
        self.assertEqual(membership["left_only"], "left_only")
        self.assertEqual(membership["right_only"], "right_only")
        self.assertEqual(result["comparison"]["left"]["name"], "Left")


if __name__ == "__main__":
    unittest.main()
