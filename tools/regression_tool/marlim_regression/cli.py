from __future__ import annotations

import argparse
from dataclasses import asdict
import json
from pathlib import Path
import sys
import uuid

from .build import FixtureBuilder, ShadowSourceBuilder
from .config import ConfigManager, DATA_ROOT, ToolConfig, initialize_data_directories
from .coverage import coverage_provider
from .detection import ProjectDetector
from .domain import TestCase
from .process import ProcessExecutor
from .runner import TestRunner
from .storage import StorageRepository, discover_project_demos


def services() -> tuple[ConfigManager, ToolConfig, StorageRepository]:
    initialize_data_directories()
    manager = ConfigManager()
    config = manager.load()
    storage = StorageRepository(DATA_ROOT / "regression.db")
    return manager, config, storage


def detect(config: ToolConfig, as_json: bool = False) -> int:
    report = ProjectDetector().detect(config)
    if as_json:
        print(json.dumps(report.to_dict(), indent=2, ensure_ascii=False))
    else:
        print(f"Project: {report.project_root}")
        print(f"Valid: {report.valid_project}")
        print(f"Platform: {report.operating_system} {report.architecture}")
        print(f"Build: {report.build_system}; {report.cpp_standard}")
        print(f"Coverage: {report.recommended_backend}")
        for tool in report.tools:
            print(f"  {tool.name}: {tool.path or 'missing'} {tool.version}")
        print(f"Demos: {len(report.demos)}")
    return 0 if report.valid_project else 2


def validate_fixture(config: ToolConfig) -> int:
    config.coverage_backend = "llvm"
    fixture = Path(__file__).resolve().parents[1] / "tests" / "fixtures" / "coverage_fixture.cpp"
    build_dir = DATA_ROOT / "build" / "fixture"
    executable = build_dir / ("coverage_fixture.exe" if sys.platform.startswith("win") else "coverage_fixture")
    built = FixtureBuilder(config).build(fixture, executable)
    if built.exit_code != 0:
        print(built.stderr or built.stdout, file=sys.stderr)
        return 1
    provider = coverage_provider(config)
    executor = ProcessExecutor()
    reports = []
    profiles = []
    for value in ("positive", "negative"):
        run_id = f"fixture-{value}-{uuid.uuid4()}"
        run_dir = DATA_ROOT / "runs" / run_id
        env = provider.environment_for_run(run_id, run_dir)
        result = executor.run([str(executable), value], run_dir, env, timeout=10)
        if result.exit_code != 0:
            print(result.stderr, file=sys.stderr)
            return 1
        report = provider.collect(executable, run_dir)
        if report is None:
            print("fixture produced no coverage", file=sys.stderr)
            return 1
        reports.append(report)
        profiles.append(Path(report.profile_path or ""))
    combined = provider.merge(executable, profiles, DATA_ROOT / "coverage" / "fixture-combined")
    print(json.dumps({
        "individual": [asdict(report.summary) for report in reports],
        "combined": asdict(combined.summary),
        "functions": sorted(combined.covered_function_keys()),
    }, indent=2))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Marlim3 regression and native coverage tool")
    sub = parser.add_subparsers(dest="command", required=True)
    detection = sub.add_parser("detect", help="Detect the Marlim3 project and native tools")
    detection.add_argument("--json", action="store_true")
    sub.add_parser("build", help="Create an isolated instrumented Marlim3 build")
    sub.add_parser("validate-fixture", help="Compile and merge two real LLVM coverage runs")
    listing = sub.add_parser("list-tests", help="List registered tests")
    listing.add_argument("--json", action="store_true")
    add = sub.add_parser("add-test", help="Register one test")
    add.add_argument("name")
    add.add_argument("model")
    add.add_argument("--suite", default="default")
    add.add_argument("--argument", action="append", default=[])
    run = sub.add_parser("run", help="Run a registered test")
    run.add_argument("test_id")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    manager, config, storage = services()
    if args.command == "detect":
        return detect(config, args.json)
    if args.command == "build":
        result = ShadowSourceBuilder(config).build(lambda stream, value: print(value, end=""))
        if result.success and result.executable:
            config.executable_path = str(Path(result.executable).relative_to(DATA_ROOT))
            manager.save(config)
        print(result.message)
        return 0 if result.success else 1
    if args.command == "validate-fixture":
        return validate_fixture(config)
    if args.command == "list-tests":
        demos = discover_project_demos(config.resolved_project_root)
        discovered_ids = {test.id for test in demos}
        tests = [*demos, *(test for test in storage.list_tests() if test.id not in discovered_ids)]
        if args.json:
            print(json.dumps([item.to_dict() for item in tests], indent=2, ensure_ascii=False))
        else:
            for item in tests:
                print(f"{item.id}\t{item.suite}\t{item.name}\t{'active' if item.active else 'disabled'}")
        return 0
    if args.command == "add-test":
        test = TestCase(args.name, args.model, suite=args.suite, arguments=args.argument)
        storage.save_test(test)
        print(test.id)
        return 0
    if args.command == "run":
        test = storage.get_test(args.test_id)
        if not test:
            test = next((item for item in discover_project_demos(config.resolved_project_root) if item.id == args.test_id), None)
        if not test:
            print(f"Unknown test: {args.test_id}", file=sys.stderr)
            return 2
        record = TestRunner(config, coverage_provider(config)).run(test)
        storage.save_run(record)
        print(json.dumps(asdict(record), indent=2, ensure_ascii=False))
        return 0 if record.status.value == "passed" else 1
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
