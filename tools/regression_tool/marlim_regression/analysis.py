from __future__ import annotations

from collections import deque
from dataclasses import dataclass
from pathlib import Path
import re
import shutil
import subprocess
from typing import Iterable

from .domain import CoverageReport


@dataclass
class Contribution:
    test_id: str
    added_lines: int
    added_functions: int
    line_points: float
    redundancy_percent: float


def incremental_contributions(reports: list[tuple[str, CoverageReport]]) -> list[Contribution]:
    known_lines: set[tuple[str, int]] = set()
    known_functions: set[str] = set()
    result = []
    for test_id, report in reports:
        lines = report.covered_line_keys()
        functions = report.covered_function_keys()
        added_lines = len(lines - known_lines)
        added_functions = len(functions - known_functions)
        redundancy = 100.0 * len(lines & known_lines) / len(lines) if lines else 0.0
        line_points = 100.0 * added_lines / report.summary.lines_total if report.summary.lines_total else 0.0
        result.append(Contribution(test_id, added_lines, added_functions, round(line_points, 2), round(redundancy, 2)))
        known_lines |= lines
        known_functions |= functions
    return result


def compare_reports(left: CoverageReport, right: CoverageReport) -> dict[str, object]:
    left_functions, right_functions = left.covered_function_keys(), right.covered_function_keys()
    left_lines, right_lines = left.covered_line_keys(), right.covered_line_keys()
    return {
        "common_functions": sorted(left_functions & right_functions),
        "left_only_functions": sorted(left_functions - right_functions),
        "right_only_functions": sorted(right_functions - left_functions),
        "common_lines": len(left_lines & right_lines),
        "left_only_lines": len(left_lines - right_lines),
        "right_only_lines": len(right_lines - left_lines),
        "line_percent_delta": round(right.summary.line_percent - left.summary.line_percent, 2),
    }


def function_hit_matrix(reports: list[tuple[str, CoverageReport]]) -> dict[str, list[str]]:
    matrix: dict[str, list[str]] = {}
    for test_id, report in reports:
        for name in report.covered_function_keys():
            matrix.setdefault(name, []).append(test_id)
    return matrix


class CallGraphAnalyzer:
    """Creates a bounded, coverage-filtered static call graph; edges are approximate, not a runtime trace."""

    FUNCTION = re.compile(r"(?m)^[\w:<>,~*&\s]+\s+([A-Za-z_~][\w:~]*)\s*\([^;{}]*\)\s*\{")
    CALL = re.compile(r"\b([A-Za-z_][\w:]*)\s*\(")
    KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "catch"}

    def build(self, source_roots: Iterable[Path], covered_functions: set[str], max_nodes: int = 500) -> dict[str, list[dict]]:
        short_names = {name.split("(")[0].split("::")[-1] for name in covered_functions}
        nodes: dict[str, dict] = {}
        edges: set[tuple[str, str]] = set()
        for root in source_roots:
            if not root.exists():
                continue
            for path in root.rglob("*.cpp"):
                text = path.read_text(encoding="utf-8", errors="replace")
                matches = list(self.FUNCTION.finditer(text))
                for index, match in enumerate(matches):
                    caller = match.group(1)
                    if short_names and caller.split("::")[-1] not in short_names:
                        continue
                    nodes.setdefault(caller, {"id": caller, "label": caller, "file": str(path), "covered": True})
                    body_end = matches[index + 1].start() if index + 1 < len(matches) else min(len(text), match.end() + 12000)
                    for called in self.CALL.findall(text[match.end():body_end]):
                        short = called.split("::")[-1]
                        if short not in self.KEYWORDS and (not short_names or short in short_names):
                            nodes.setdefault(called, {"id": called, "label": called, "file": "", "covered": short in short_names})
                            edges.add((caller, called))
                    if len(nodes) >= max_nodes:
                        break
                if len(nodes) >= max_nodes:
                    break
        return {"nodes": list(nodes.values())[:max_nodes], "edges": [{"source": a, "target": b, "approximate": True} for a, b in edges if a in nodes and b in nodes]}


def _mangled_part(symbol: str) -> str:
    position = symbol.find("_Z")
    return symbol[position:] if position >= 0 else symbol


_DEMANGLE_MEMO: dict[str, str] = {}
_SOURCE_MEMO: dict[str, tuple[int, list[str]]] = {}
_SOURCE_MEMO_LIMIT = 512


def _source_lines(filename: str) -> list[str]:
    """Read a source file once per revision, keyed by modification time."""
    try:
        stamp = Path(filename).stat().st_mtime_ns
    except OSError:
        return []
    cached = _SOURCE_MEMO.get(filename)
    if cached is not None and cached[0] == stamp:
        return cached[1]
    try:
        lines = Path(filename).read_text(encoding="utf-8", errors="replace").splitlines()
    except OSError:
        lines = []
    if len(_SOURCE_MEMO) >= _SOURCE_MEMO_LIMIT:
        _SOURCE_MEMO.pop(next(iter(_SOURCE_MEMO)))
    _SOURCE_MEMO[filename] = (stamp, lines)
    return lines


def demangle_symbols(symbols: Iterable[str]) -> dict[str, str]:
    """Demangle a symbol set in one c++filt process, preserving unknown names.

    Results are memoised process-wide: symbol names are stable for a given build,
    so repeated selections reuse them instead of spawning c++filt again.
    """
    unique = list(dict.fromkeys(str(symbol) for symbol in symbols))
    resolved = {symbol: _DEMANGLE_MEMO[symbol] for symbol in unique if symbol in _DEMANGLE_MEMO}
    pending = [symbol for symbol in unique if symbol not in resolved]
    if not pending:
        return {symbol: resolved[symbol] for symbol in unique}
    tool = shutil.which("c++filt")
    inputs = [_mangled_part(symbol) for symbol in pending]
    demangled = dict(zip(pending, inputs))
    if tool:
        try:
            result = subprocess.run(
                [tool], input="\n".join(inputs) + "\n", capture_output=True,
                text=True, timeout=20, check=False,
            )
            outputs = result.stdout.splitlines()
            if result.returncode == 0 and len(outputs) == len(pending):
                demangled = dict(zip(pending, outputs))
        except (OSError, subprocess.SubprocessError):
            pass
    _DEMANGLE_MEMO.update(demangled)
    resolved.update(demangled)
    return {symbol: resolved[symbol] for symbol in unique}


class FlowGraphBuilder:
    """Build a readable covered call graph from LLVM symbols and source regions."""

    CALL = re.compile(r"\b([A-Za-z_~][A-Za-z0-9_:~]*)\s*\(")
    IGNORED_CALLS = {
        "if", "for", "while", "switch", "return", "sizeof", "catch",
        "static_cast", "dynamic_cast", "reinterpret_cast", "const_cast",
    }

    @staticmethod
    def _short_name(display: str) -> str:
        base = display.split("(", 1)[0].strip()
        return base.rsplit("::", 1)[-1]

    @staticmethod
    def _region(function: dict) -> tuple[int, int]:
        regions = function.get("regions") or []
        if not regions:
            return 0, 0
        region = max(regions, key=lambda value: (int(value[2]) - int(value[0]), int(value[3]) - int(value[1])))
        return int(region[0]), int(region[2])

    NODE_FIELDS = (
        "id", "label", "short", "file", "line", "end_line", "hit_tests",
        "test_count", "selected_count", "count", "status", "regions_covered",
        "regions_total", "namespace", "directory",
    )

    @staticmethod
    def _collect_symbols(reports: list[tuple[str, CoverageReport]]) -> dict[str, dict]:
        """Merge per-test LLVM function records into one symbol table."""
        symbols: dict[str, dict] = {}
        for test_id, report in reports:
            for function in report.functions:
                name = str(function.get("name", ""))
                if not name:
                    continue
                entry = symbols.setdefault(name, {
                    "id": name, "name": name, "filenames": function.get("filenames", []),
                    "regions": function.get("regions", []), "counts": {}, "region_hits": {},
                })
                entry["counts"][test_id] = max(int(function.get("count", 0)), int(entry["counts"].get(test_id, 0)))
                for region in function.get("regions", []):
                    if len(region) < 8 or int(region[7]) != 0:
                        continue
                    key = (int(region[0]), int(region[1]), int(region[2]), int(region[3]))
                    entry["region_hits"][key] = int(entry["region_hits"].get(key, 0)) + int(region[4])
                if not entry["filenames"] and function.get("filenames"):
                    entry["filenames"] = function["filenames"]
                if not entry["regions"] and function.get("regions"):
                    entry["regions"] = function["regions"]
        return symbols

    @staticmethod
    def _coverage_status(total_calls: int, regions_covered: int, regions_total: int) -> str:
        if total_calls <= 0:
            return "uncovered"
        if regions_total and regions_covered < regions_total:
            return "partial"
        return "covered"

    def _describe_symbols(self, symbols: dict[str, dict], selected_count: int) -> dict[str, list[str]]:
        """Attach display metadata to each symbol and index them by short name."""
        display_names = demangle_symbols(symbols)
        short_index: dict[str, list[str]] = {}
        for symbol, entry in symbols.items():
            display = display_names.get(symbol, symbol)
            short = self._short_name(display)
            start_line, end_line = self._region(entry)
            filename = str((entry.get("filenames") or [""])[0])
            hits = [test_id for test_id, count in entry["counts"].items() if count > 0]
            total_calls = sum(int(count) for count in entry["counts"].values())
            region_counts = list(entry["region_hits"].values())
            regions_covered = sum(1 for count in region_counts if count > 0)
            qualified = display.split("(", 1)[0].strip()
            entry.update({
                "label": display, "short": short, "file": filename,
                "line": start_line, "end_line": end_line, "hit_tests": hits,
                "test_count": len(hits), "selected_count": selected_count,
                "count": total_calls,
                "status": self._coverage_status(total_calls, regions_covered, len(region_counts)),
                "regions_covered": regions_covered,
                "regions_total": len(region_counts),
                "namespace": qualified.rsplit("::", 1)[0] if "::" in qualified else "global",
                "directory": str(Path(filename).parent),
            })
            short_index.setdefault(short, []).append(symbol)
        return short_index

    def _resolve_edges(self, symbols: dict[str, dict], short_index: dict[str, list[str]]) -> set[tuple[str, str]]:
        """Approximate call edges by scanning each covered function body."""
        edges: set[tuple[str, str]] = set()
        # Resolving a called short name to a symbol is pure with respect to the
        # symbol table, so memoise it across every caller in this build.
        target_memo: dict[tuple[str, str], str | None] = {}
        for caller in symbols.values():
            filename, start_line = caller["file"], caller["line"]
            if not filename or start_line <= 0:
                continue
            lines = _source_lines(filename)
            if not lines:
                continue
            end_line = min(max(start_line, caller["end_line"]), len(lines))
            body = "\n".join(lines[start_line - 1:end_line])
            caller_short, caller_id = caller["short"], caller["id"]
            for called in self.CALL.findall(body):
                short = called.rsplit("::", 1)[-1]
                if short in self.IGNORED_CALLS or short == caller_short:
                    continue
                memo_key = (short, filename)
                if memo_key not in target_memo:
                    target_memo[memo_key] = max(
                        short_index.get(short, ()),
                        key=lambda symbol: (
                            symbols[symbol]["file"] == filename,
                            symbols[symbol]["test_count"] > 0,
                            symbols[symbol]["count"],
                        ),
                        default=None,
                    )
                target = target_memo[memo_key]
                if target is not None and target != caller_id:
                    edges.add((caller_id, target))
        return edges

    @staticmethod
    def _roots(symbols: dict[str, dict], incoming: dict[str, int], focus: str) -> list[str]:
        roots = [entry["id"] for entry in symbols.values() if entry["short"] == "main"]
        if not roots:
            roots = [entry["id"] for entry in symbols.values() if incoming.get(entry["id"], 0) == 0]
        roots.sort(key=lambda symbol: symbols[symbol]["count"], reverse=True)
        focus_lower = focus.strip().lower()
        if focus_lower:
            matching = [entry["id"] for entry in symbols.values() if focus_lower in entry["label"].lower()]
            return matching or roots
        return roots

    @staticmethod
    def _recursion_depths(roots: list[str], adjacency: dict[str, list[str]]) -> dict[str, int]:
        depths: dict[str, int] = {}
        queue = deque((root, 0) for root in roots)
        while queue:
            symbol, depth = queue.popleft()
            previous = depths.get(symbol)
            if previous is not None and previous <= depth:
                continue
            depths[symbol] = depth
            queue.extend((target, depth + 1) for target in adjacency.get(symbol, []))
        return depths

    @staticmethod
    def _rank(symbols: dict[str, dict], adjacency: dict[str, list[str]]) -> list[dict]:
        return sorted(
            symbols.values(),
            key=lambda entry: (entry["test_count"] > 0, entry["count"], len(adjacency.get(entry["id"], []))),
            reverse=True,
        )

    def _select_nodes(
        self,
        symbols: dict[str, dict],
        adjacency: dict[str, list[str]],
        incoming: dict[str, int],
        roots: list[str],
        max_nodes: int | None,
    ) -> tuple[list[str], dict[str, int]]:
        """Breadth-first selection from the roots, then rank-based completion."""
        node_limit = len(symbols) if max_nodes is None else max_nodes
        chosen: list[str] = []
        levels: dict[str, int] = {}
        queue = deque((root, 0) for root in roots[:3])
        while queue and len(chosen) < node_limit:
            symbol, level = queue.popleft()
            if symbol in levels:
                continue
            levels[symbol] = level
            chosen.append(symbol)
            targets = sorted(adjacency.get(symbol, []), key=lambda item: symbols[item]["count"], reverse=True)
            branch_limit = len(targets) if max_nodes is None else 4
            queue.extend((target, level + 1) for target in targets[:branch_limit])

        if max_nodes is None:
            fallback_level = max(levels.values(), default=0) + 1
            for entry in self._rank(symbols, adjacency):
                if entry["id"] not in levels:
                    levels[entry["id"]] = fallback_level
                    chosen.append(entry["id"])
        elif len(chosen) < min(12, node_limit):
            for entry in self._rank(symbols, adjacency):
                if len(chosen) >= min(node_limit, 28):
                    break
                symbol = entry["id"]
                if symbol not in levels and (entry["test_count"] > 0 or incoming.get(symbol, 0)):
                    levels[symbol] = max(levels.values(), default=0) + 1
                    chosen.append(symbol)
        return chosen, levels

    def build_from_reports(
        self,
        reports: list[tuple[str, CoverageReport]],
        max_nodes: int | None = 48,
        focus: str = "",
    ) -> dict[str, object]:
        if not reports:
            return {"nodes": [], "edges": [], "max_recursion_depth": 0}

        symbols = self._collect_symbols(reports)
        short_index = self._describe_symbols(symbols, len(reports))
        edges = self._resolve_edges(symbols, short_index)

        adjacency: dict[str, list[str]] = {}
        incoming: dict[str, int] = {}
        for source, target in edges:
            adjacency.setdefault(source, []).append(target)
            incoming[target] = incoming.get(target, 0) + 1

        roots = self._roots(symbols, incoming, focus)
        recursion_depths = self._recursion_depths(roots, adjacency)
        chosen, levels = self._select_nodes(symbols, adjacency, incoming, roots, max_nodes)

        chosen_set = set(chosen)
        graph_nodes = [
            {key: symbols[symbol][key] for key in self.NODE_FIELDS}
            | {"level": levels[symbol], "recursion_depth": recursion_depths.get(symbol)}
            for symbol in chosen
        ]
        graph_edges = [
            {"source": source, "target": target, "approximate": True,
             "covered": symbols[source]["test_count"] > 0 and symbols[target]["test_count"] > 0}
            for source, target in edges if source in chosen_set and target in chosen_set
        ]
        return {
            "nodes": graph_nodes,
            "edges": graph_edges,
            "max_recursion_depth": max(recursion_depths.values(), default=0),
        }
