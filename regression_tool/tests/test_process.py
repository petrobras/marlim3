from pathlib import Path
import sys
import tempfile
import unittest

from marlim_regression.process import ProcessExecutor


class ProcessExecutorTests(unittest.TestCase):
    def test_captures_output_and_exit_code(self):
        with tempfile.TemporaryDirectory() as temporary:
            result = ProcessExecutor().run([sys.executable, "-c", "import sys; print('out'); print('err', file=sys.stderr); raise SystemExit(4)"], Path(temporary), timeout=5)
        self.assertEqual(result.exit_code, 4)
        self.assertIn("out", result.stdout)
        self.assertIn("err", result.stderr)

    def test_timeout_terminates_process(self):
        with tempfile.TemporaryDirectory() as temporary:
            result = ProcessExecutor().run([sys.executable, "-c", "import time; time.sleep(5)"], Path(temporary), timeout=0.05)
        self.assertTrue(result.timed_out)
        self.assertLess(result.duration_seconds, 3)

    def test_streams_stdout_and_stderr_while_preserving_full_capture(self):
        events = []
        with tempfile.TemporaryDirectory() as temporary:
            result = ProcessExecutor().run(
                [sys.executable, "-u", "-c", "import sys; print('first'); print('problem', file=sys.stderr); print('last')"],
                Path(temporary), timeout=None,
                on_output=lambda stream, text: events.append((stream, text)),
            )
        self.assertIn(("stdout", "first\n"), events)
        self.assertIn(("stdout", "last\n"), events)
        self.assertIn(("stderr", "problem\n"), events)
        self.assertEqual(result.stdout, "first\nlast\n")
        self.assertEqual(result.stderr, "problem\n")


if __name__ == "__main__":
    unittest.main()
