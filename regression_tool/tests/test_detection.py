from pathlib import Path
import unittest

from marlim_regression.config import ToolConfig
from marlim_regression.detection import ProjectDetector


class DetectionTests(unittest.TestCase):
    def test_detects_real_checkout(self):
        root = Path(__file__).resolve().parents[2]
        config = ToolConfig(project_root=str(root))
        report = ProjectDetector().detect(config)
        self.assertTrue(report.valid_project)
        self.assertEqual(report.cpp_standard, "C++11")
        self.assertEqual(report.executable_name, "Marlim3.exe" if __import__("os").name == "nt" else "Marlim3")
        self.assertGreaterEqual(len(report.demos), 2)


if __name__ == "__main__":
    unittest.main()

