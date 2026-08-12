import unittest

from marlim_regression.analysis import FlowGraphBuilder, compare_reports, demangle_symbols, incremental_contributions
from marlim_regression.domain import CoverageReport, CoverageSummary


def report(name: str, lines: list[int], functions: list[str]) -> CoverageReport:
    return CoverageReport(
        CoverageSummary(len(lines), 10, len(functions), 5),
        files=[{"filename": name, "lines": [{"line": line, "count": 1} for line in lines]}],
        functions=[{"name": function, "count": 1} for function in functions],
    )


class AnalysisTests(unittest.TestCase):
    def test_incremental_union_is_not_sum_of_percentages(self):
        left = report("a.cpp", [1, 2, 3], ["a", "shared"])
        right = report("a.cpp", [3, 4], ["shared", "b"])
        values = incremental_contributions([("A", left), ("B", right)])
        self.assertEqual(values[1].added_lines, 1)
        self.assertEqual(values[1].added_functions, 1)
        self.assertEqual(values[1].redundancy_percent, 50.0)

    def test_comparison(self):
        result = compare_reports(report("a", [1, 2], ["a"]), report("a", [2, 3], ["b"]))
        self.assertEqual(result["common_lines"], 1)
        self.assertEqual(result["left_only_functions"], ["a"])

    def test_demangles_symbols(self):
        values = demangle_symbols(["_ZN3Ler6lerArqEv"])
        self.assertIn("Ler::lerArq", values["_ZN3Ler6lerArqEv"])

    def test_flow_graph_uses_llvm_regions_and_calls(self):
        import tempfile
        from pathlib import Path
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary) / "flow.cpp"
            source.write_text("int helper() { return 1; }\nint main() { return helper(); }\n", encoding="utf-8")
            coverage = CoverageReport(
                CoverageSummary(),
                functions=[
                    {"name": "_Z6helperv", "count": 1, "filenames": [str(source)], "regions": [[1, 1, 1, 27, 1, 0, 0, 0]]},
                    {"name": "main", "count": 1, "filenames": [str(source)], "regions": [[2, 1, 2, 32, 1, 0, 0, 0]]},
                ],
            )
            graph = FlowGraphBuilder().build_from_reports([("test", coverage)])
        self.assertGreaterEqual(len(graph["nodes"]), 2)
        self.assertTrue(any(edge["source"] == "main" and edge["target"] == "_Z6helperv" for edge in graph["edges"]))

    def test_complete_graph_keeps_every_function_and_region_status(self):
        coverage = CoverageReport(
            CoverageSummary(),
            functions=[
                {"name": "covered", "count": 3, "regions": [[1, 1, 2, 2, 3, 0, 0, 0]]},
                {"name": "partial", "count": 1, "regions": [[3, 1, 4, 2, 1, 0, 0, 0], [4, 3, 5, 2, 0, 0, 0, 0]]},
                {"name": "uncovered", "count": 0, "regions": [[6, 1, 7, 2, 0, 0, 0, 0]]},
            ],
        )
        graph = FlowGraphBuilder().build_from_reports([("test", coverage)], max_nodes=None)
        statuses = {node["id"]: node["status"] for node in graph["nodes"]}
        self.assertEqual(len(graph["nodes"]), 3)
        self.assertEqual(statuses, {"covered": "covered", "partial": "partial", "uncovered": "uncovered"})

    def test_recursion_depth_starts_with_direct_main_calls_at_one(self):
        import tempfile
        from pathlib import Path
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary) / "depth.cpp"
            source.write_text(
                "int leaf() { return 1; }\n"
                "int helper() { return leaf(); }\n"
                "int main() { return helper(); }\n",
                encoding="utf-8",
            )
            coverage = CoverageReport(
                CoverageSummary(),
                functions=[
                    {"name": "leaf", "count": 1, "filenames": [str(source)], "regions": [[1, 1, 1, 25, 1, 0, 0, 0]]},
                    {"name": "helper", "count": 1, "filenames": [str(source)], "regions": [[2, 1, 2, 32, 1, 0, 0, 0]]},
                    {"name": "main", "count": 1, "filenames": [str(source)], "regions": [[3, 1, 3, 32, 1, 0, 0, 0]]},
                ],
            )
            graph = FlowGraphBuilder().build_from_reports([("test", coverage)], max_nodes=None)

        depths = {node["id"]: node["recursion_depth"] for node in graph["nodes"]}
        self.assertEqual(depths, {"main": 0, "helper": 1, "leaf": 2})
        self.assertEqual(graph["max_recursion_depth"], 2)


if __name__ == "__main__":
    unittest.main()
