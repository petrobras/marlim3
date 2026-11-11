#!/usr/bin/env python3
"""Compact index of the definitions in the Marlim3 sources.

Prints one line per function, method, type or Fortran unit: name, path:line and
signature. Several engine sources and extracted SisProd solver modules are
large, so look a symbol up here and read only the lines around it.

Examples:
    python .github/skills/development/scripts/symbol_index.py --grep Perm --path SisProd
    python .github/skills/development/scripts/symbol_index.py --count
    python .github/skills/development/scripts/symbol_index.py --roots tools/marlim3 --grep simulate
"""

from __future__ import annotations

import argparse
import ast
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
CPP_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp"}
FORTRAN_SUFFIXES = {".f", ".f90", ".f95", ".for"}
NOT_A_DEFINITION = re.compile(
    r"^(?:if|for|while|switch|return|else|do|case|namespace|using|typedef|template|extern|static_assert)\b"
)
CPP_FUNCTION = re.compile(r"((?:\w+::)*~?\w+)\s*\(")
CPP_TYPE = re.compile(r"^(?:class|struct|enum(?:\s+class)?|union)\s+(\w+)")
FORTRAN_UNIT = re.compile(
    r"^\s*(?:(?:recursive|pure|elemental|impure|module)\s+)*"
    r"(?:(?:integer|real|logical|character|complex|double\s+precision)(?:\s*\([^)]*\))?\s+)?"
    r"(subroutine|function|module|program)\s+(\w+)",
    re.IGNORECASE,
)


def read(path: Path) -> str:
    # utf-8-sig: some sources start with a byte order mark.
    return path.read_text(encoding="utf-8-sig", errors="replace")


def cpp_definitions(path: Path):
    lines = read(path).splitlines()
    for number, line in enumerate(lines, 1):
        if not line or line[0] in " \t#/*}{" or NOT_A_DEFINITION.match(line):
            continue
        stripped = line.rstrip()
        type_match = CPP_TYPE.match(stripped)
        if type_match and not stripped.endswith(";"):
            yield "type", type_match.group(1), number, stripped
            continue
        call = CPP_FUNCTION.search(stripped)
        if not call or stripped.endswith(";") or "=" in stripped[:call.start()]:
            continue
        # A definition opens its body before any ';' within the next few lines.
        window = " ".join(lines[number - 1:number + 5])
        brace, semicolon = window.find("{"), window.find(";")
        if brace != -1 and (semicolon == -1 or brace < semicolon):
            yield "func", call.group(1), number, stripped


def fortran_definitions(path: Path):
    for number, line in enumerate(read(path).splitlines(), 1):
        code = line.split("!", 1)[0]
        if re.match(r"^\s*end\b", code, re.IGNORECASE) or re.search(r"\bmodule\s+procedure\b", code, re.IGNORECASE):
            continue
        unit = FORTRAN_UNIT.match(code)
        if unit:
            yield unit.group(1).lower(), unit.group(2), number, code.strip()


def python_definitions(path: Path):
    try:
        tree = ast.parse(read(path))
    except SyntaxError as error:
        print(f"skipped {path.relative_to(ROOT).as_posix()}: {error} (use the project's Python 3.12+)", file=sys.stderr)
        return

    def walk(node, prefix):
        for child in ast.iter_child_nodes(node):
            if isinstance(child, ast.ClassDef):
                yield "type", prefix + child.name, child.lineno, f"class {child.name}"
                yield from walk(child, prefix + child.name + ".")
            elif isinstance(child, (ast.FunctionDef, ast.AsyncFunctionDef)):
                arguments = ", ".join(argument.arg for argument in child.args.args)
                yield "func", prefix + child.name, child.lineno, f"def {child.name}({arguments})"

    yield from walk(tree, "")


def definitions(roots):
    for root in roots:
        base = ROOT / root
        files = [base] if base.is_file() else sorted(p for p in base.rglob("*") if p.is_file())
        for path in files:
            suffix = path.suffix.lower()
            if suffix in CPP_SUFFIXES:
                parser = cpp_definitions
            elif suffix in FORTRAN_SUFFIXES:
                parser = fortran_definitions
            elif suffix == ".py":
                parser = python_definitions
            else:
                continue
            relative = path.relative_to(ROOT).as_posix()
            for kind, name, number, signature in parser(path):
                yield kind, name, relative, number, signature


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--roots", nargs="+", default=["src"],
                        help="folders or files relative to the repository root (default: src)")
    parser.add_argument("--grep", help="case-insensitive regular expression matched against the name and the signature")
    parser.add_argument("--path", help="case-insensitive substring of the file path, e.g. SisProd or src/fortran")
    parser.add_argument("--kind", choices=["func", "type", "subroutine", "function", "module", "program"])
    parser.add_argument("--count", action="store_true", help="print the number of definitions per file instead")
    parser.add_argument("--width", type=int, default=140, help="maximum signature width (0 for no limit)")
    args = parser.parse_args()

    pattern = re.compile(args.grep, re.IGNORECASE) if args.grep else None
    counts = {}
    for kind, name, path, number, signature in definitions(args.roots):
        if args.path and args.path.lower() not in path.lower():
            continue
        if args.kind and kind != args.kind:
            continue
        if pattern and not (pattern.search(name) or pattern.search(signature)):
            continue
        if args.count:
            counts[path] = counts.get(path, 0) + 1
            continue
        if args.width and len(signature) > args.width:
            signature = signature[: args.width - 3] + "..."
        print(f"{name}\t{path}:{number}\t{signature}")
    for path, count in sorted(counts.items(), key=lambda item: -item[1]):
        print(f"{count}\t{path}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except BrokenPipeError:
        # The reader (e.g. head) closed the pipe early.
        os.dup2(os.open(os.devnull, os.O_WRONLY), sys.stdout.fileno())
        sys.exit(0)
