from pathlib import Path
import tempfile
import unittest

from marlim_regression.domain import CoverageReport, CoverageSummary, TestCase as RegressionCase, ensure_within


class DomainTests(unittest.TestCase):
    def test_test_case_roundtrip(self):
        original = RegressionCase("demo", "demos/demo.mr3", tags=["fast"])
        restored = RegressionCase.from_dict(original.to_dict())
        self.assertEqual(restored.id, original.id)
        self.assertEqual(restored.tags, ["fast"])

    def test_coverage_sets(self):
        report = CoverageReport(CoverageSummary(), files=[{"filename": "a.cpp", "lines": [{"line": 3, "count": 2}, {"line": 4, "count": 0}]}], functions=[{"name": "run", "count": 1}])
        self.assertEqual(report.covered_line_keys(), {("a.cpp", 3)})
        self.assertEqual(report.covered_function_keys(), {"run"})

    def test_safe_path_rejects_escape(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            self.assertEqual(ensure_within(root / "child", root), (root / "child").resolve())
            with self.assertRaises(ValueError):
                ensure_within(root / ".." / "escape", root)


if __name__ == "__main__":
    unittest.main()
