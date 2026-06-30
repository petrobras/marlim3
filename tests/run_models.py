from __future__ import annotations

import ast
import json
import os
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


TESTS_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = TESTS_DIR.parent
MODELS_FILE = PROJECT_ROOT / "data" / "models" / "models.mr3"
CHECK_RESULT = TESTS_DIR / "check_result.py"
REGRESSION_TESTS_FILE = TESTS_DIR / "test_regression.py"
JSON_MODEL_GROUPS = ("benchmark", "transiente", "injetor", "rede", "convecnat")
COMMAND_GROUPS = (*JSON_MODEL_GROUPS, "regression")

RED = "\033[0;31m"
GREEN = "\033[0;32m"
YELLOW = "\033[1;33m"
BLUE = "\033[0;34m"
CYAN = "\033[0;36m"
MAGENTA = "\033[0;35m"
NC = "\033[0m"


@dataclass(frozen=True)
class Model:
    name: str
    type: str
    input_path: str
    pvt_path: str
    output_file: str
    group: str


@dataclass(frozen=True)
class ModelRun:
    model: Model
    passed: bool
    elapsed: float
    status: str
    code: str
    reason: str


def executable_path() -> Path:
    names = ("Marlim3.exe", "Marlim3") if os.name == "nt" else ("Marlim3", "Marlim3.exe")
    for name in names:
        path = PROJECT_ROOT / "build" / name
        if path.is_file():
            return path
    return PROJECT_ROOT / "build" / names[0]


def group_color(group: str) -> str:
    return {
        "benchmark": RED,
        "transiente": CYAN,
        "injetor": MAGENTA,
        "rede": GREEN,
        "convecnat": YELLOW,
    }.get(group, NC)


def show_help() -> None:
    print(f"{BLUE}Marlim3 - Model Execution System{NC}\n")
    print(f"{YELLOW}USAGE:{NC}")
    print("  python tests/run_models.py [OPTION] [ARGUMENT]\n")
    print(f"{YELLOW}OPTIONS:{NC}")
    print("  --list, -l              List all available models")
    print("  --all, -a               Run all models")
    print("  --group, -g <group>     Run all models in a group")
    print("  --model, -m <model>     Run one model")
    print("  --help, -h              Show this help message\n")
    print(f"{YELLOW}GROUPS:{NC}")
    print("  benchmark transiente injetor rede convecnat regression\n")
    print(f"{YELLOW}EXAMPLES:{NC}")
    print("  python tests/run_models.py --list")
    print("  python tests/run_models.py teste1")
    print("  python tests/run_models.py --model producaoSimplificado")
    print("  python tests/run_models.py --group rede")
    print("  python tests/run_models.py --group regression")
    print("  python tests/run_models.py --all")


def load_models() -> dict[str, list[Model]]:
    models: dict[str, list[Model]] = {group: [] for group in JSON_MODEL_GROUPS}
    if not MODELS_FILE.is_file():
        return models

    with MODELS_FILE.open(encoding="utf-8") as fp:
        data = json.load(fp)

    for group in JSON_MODEL_GROUPS:
        group_models = []
        for item in data.get(group, []):
            directory = item["dir"]
            if directory == ".":
                input_path = f"data/exemplos-json/{item['input']}"
                pvt_path = "data/exemplos-json/"
            else:
                input_path = f"data/exemplos-json/{directory}/{item['input']}"
                pvt_path = f"data/exemplos-json/{directory}/"

            group_models.append(
                Model(
                    name=item["name"],
                    type=item.get("type") or group.upper(),
                    input_path=input_path,
                    pvt_path=pvt_path,
                    output_file=item["output"],
                    group=group,
                )
            )
        models[group] = group_models
    return models


def load_regression_models() -> list[Model]:
    with REGRESSION_TESTS_FILE.open(encoding="utf-8") as fp:
        tree = ast.parse(fp.read(), filename=str(REGRESSION_TESTS_FILE))

    demos = None
    for node in tree.body:
        if isinstance(node, ast.Assign):
            for target in node.targets:
                if isinstance(target, ast.Name) and target.id == "DEMOS":
                    demos = ast.literal_eval(node.value)
                    break
        if demos is not None:
            break

    if not isinstance(demos, dict):
        raise SystemExit(f"Definicao DEMOS nao encontrada em {REGRESSION_TESTS_FILE}")

    return [
        Model(
            name=name,
            type="TRANSIENTE",
            input_path=f"demos/{info['json']}",
            pvt_path="demos/",
            output_file=f"{name}.log",
            group="regression",
        )
        for name, info in sorted(demos.items())
    ]


def available_models(models: dict[str, list[Model]]) -> dict[str, list[Model]]:
    return {**models, "regression": load_regression_models()}


def get_model_config(model_name: str, models: dict[str, list[Model]]) -> Model | None:
    model_lower = model_name.lower()

    for group_models in models.values():
        for model in group_models:
            if model.name.lower() == model_lower:
                return model

    for group_models in models.values():
        for model in group_models:
            if model_lower in model.name.lower():
                return model

    return None


def list_models(models: dict[str, list[Model]]) -> None:
    all_models = available_models(models)
    print()
    print(f"{GREEN}Available models (organized by simulation type):{NC}")
    print(f"{'NAME':<35} {'GROUP':<12} INPUT")
    print("-" * 70)

    for group in COMMAND_GROUPS:
        color = group_color(group)
        print(f"\n{color}{group.upper()}{NC}")
        for model in all_models[group]:
            print(f"{color}{model.name:<35}{NC} {model.type:<12} {model.input_path}")
    print()


def model_paths(model: Model) -> tuple[str, str, Path, Path]:
    model_output_dir = PROJECT_ROOT / "output" / model.name
    model_log_dir = model_output_dir / "log"
    model_out_dir = model_output_dir / "out"
    output_log = model_log_dir / model.output_file
    return model.input_path, model.pvt_path, model_out_dir, output_log


def execute_model(model: Model, extra_args: list[str]) -> tuple[Path, Path, float]:
    input_path, pvt_path, out_dir, output_log = model_paths(model)
    log_dir = output_log.parent
    execution_log = log_dir / f"marlim3_{model.name}.log"

    log_dir.mkdir(parents=True, exist_ok=True)
    out_dir.mkdir(parents=True, exist_ok=True)

    command = [
        str(executable_path()),
        "-s",
        model.type,
        "-i",
        input_path,
        "-p",
        pvt_path,
        "-d",
        str(out_dir),
        "-o",
        str(output_log),
        *extra_args,
    ]

    start = time.monotonic()
    with execution_log.open("w", encoding="utf-8", errors="replace") as fp:
        subprocess.run(
            command,
            cwd=PROJECT_ROOT,
            stdout=fp,
            stderr=subprocess.STDOUT,
            text=True,
            errors="replace",
            check=False,
        )
    elapsed = time.monotonic() - start
    return out_dir, output_log, elapsed


def check_model(model: Model, out_dir: Path, output_log: Path) -> dict[str, str]:
    command = [
        sys.executable,
        str(CHECK_RESULT),
        model.name,
        "--output-dir",
        str(PROJECT_ROOT / "output" / model.name),
        "--out-dir",
        str(out_dir),
        "--output-log",
        str(output_log),
        "--json",
    ]
    process = subprocess.run(
        command,
        cwd=PROJECT_ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        errors="replace",
        check=False,
    )

    try:
        result = json.loads(process.stdout)
    except json.JSONDecodeError:
        result = {
            "status": "FAILED",
            "code": "VALIDATION_ERROR",
            "reason": (process.stderr or process.stdout or "tests/check_result.py nao retornou JSON").strip(),
        }

    if process.returncode != 0:
        result["status"] = "FAILED"

    return {
        "status": str(result.get("status") or "FAILED"),
        "code": str(result.get("code") or "VALIDATION_ERROR"),
        "reason": str(result.get("reason") or "falha sem motivo informado"),
    }


def format_duration(seconds: float) -> str:
    if seconds < 60:
        return f"{seconds:.1f}s"
    minutes, remaining_seconds = divmod(seconds, 60)
    if minutes < 60:
        return f"{int(minutes)}m {remaining_seconds:.1f}s"
    hours, remaining_minutes = divmod(minutes, 60)
    return f"{int(hours)}h {int(remaining_minutes)}m {remaining_seconds:.1f}s"


def run_marker(index: int | None, total: int | None) -> str:
    if index is None or total is None:
        return ""
    width = len(str(total))
    return f"[{index:>{width}}/{total}]"


def print_run_header(group: str, total: int) -> None:
    color = group_color(group)
    group_label = group.upper()
    line = "═" * 64
    print()
    print(f"{BLUE}{line}{NC}")
    print(f"{GREEN}Running group: {color}{group_label}{NC}")
    print(f"{CYAN}Models to run: {total}{NC}")
    print(f"{BLUE}{line}{NC}")
    print()


def print_running(model: Model, index: int | None = None, total: int | None = None) -> None:
    marker = run_marker(index, total)
    line = "─" * 64
    if marker:
        print(f"{YELLOW}{marker} Running: {model.name}{NC}", flush=True)
    print(f"{BLUE}{line}{NC}", flush=True)
    print(f"{GREEN}Running model: {model.name}{NC}", flush=True)
    print(f"Group: {group_color(model.group)}{model.group}{NC} | Type: {model.type}", flush=True)
    print(f"Status: {CYAN}RUNNING{NC}", flush=True)


def print_result(run: ModelRun) -> None:
    duration = format_duration(run.elapsed)
    if run.passed:
        print(f"Result: ✅ {GREEN}PASSED{NC}")
        print(f"{CYAN}Time: {duration}{NC}")
        print()
        return

    print(f"Result: ❌ {RED}FAILED{NC}")
    print(f"{CYAN}Time: {duration}{NC}")
    print(f"{RED}Código: {run.code}{NC}")
    print(f"{RED}Motivo: {run.reason}{NC}")
    print()


def print_summary(runs: list[ModelRun], total_elapsed: float) -> None:
    total = len(runs)
    passed = sum(1 for run in runs if run.passed)
    failed = total - passed
    status_color = GREEN if failed == 0 else RED
    line = "═" * 64

    print(f"{BLUE}{line}{NC}")
    print(f"{GREEN}Execution summary{NC}")
    print(f"Passed: {status_color}{passed}/{total}{NC}")
    print(f"Failed: {status_color}{failed}{NC}")
    print(f"{CYAN}Total time: {format_duration(total_elapsed)}{NC}")
    print(f"{BLUE}{line}{NC}")


def build_run(model: Model, elapsed: float, result: dict[str, str]) -> ModelRun:
    status = result["status"].upper()
    return ModelRun(
        model=model,
        passed=status == "PASSED",
        elapsed=elapsed,
        status=status,
        code=result["code"],
        reason=result["reason"],
    )


def run_one(
    model: Model,
    extra_args: list[str],
    index: int | None = None,
    total: int | None = None,
) -> ModelRun:
    print_running(model, index, total)
    out_dir, output_log, elapsed = execute_model(model, extra_args)
    result = check_model(model, out_dir, output_log)
    run = build_run(model, elapsed, result)
    print_result(run)
    return run


def run_model(model_name: str, extra_args: list[str], models: dict[str, list[Model]]) -> int:
    model = get_model_config(model_name, models)
    if model is None:
        print(f"{RED}Error: Model '{model_name}' not found.{NC}")
        print("Use 'python tests/run_models.py --list' to see available models.")
        return 1
    return 0 if run_one(model, extra_args).passed else 1


def run_group(group: str, extra_args: list[str], models: dict[str, list[Model]]) -> int:
    if group not in COMMAND_GROUPS:
        print(f"{RED}Error: Group '{group}' does not exist.{NC}")
        print(f"Available groups: {' '.join(COMMAND_GROUPS)}")
        return 1

    if group == "regression":
        group_models = load_regression_models()
    elif group == "benchmark" and not models[group]:
        group_models = load_regression_models()
    else:
        group_models = models[group]

    print_run_header(group, len(group_models))
    start = time.monotonic()
    runs = [
        run_one(model, extra_args, index, len(group_models))
        for index, model in enumerate(group_models, start=1)
    ]
    total_elapsed = time.monotonic() - start
    print_summary(runs, total_elapsed)
    return 1 if any(not run.passed for run in runs) else 0


def run_all(extra_args: list[str], models: dict[str, list[Model]]) -> int:
    failed = 0
    for group in JSON_MODEL_GROUPS:
        for model in models[group]:
            if not run_one(model, extra_args).passed:
                failed += 1
    return 1 if failed else 0


def main(argv: list[str]) -> int:
    if not executable_path().is_file():
        print(f"{YELLOW}Warning: Marlim3 executable not found. Build the project first.{NC}")

    if not CHECK_RESULT.is_file():
        print(f"{RED}Error: Verification script not found: {CHECK_RESULT}{NC}")
        return 1

    models = load_models()

    if not argv:
        show_help()
        return 0

    command = argv[0]
    if command in ("--help", "-h"):
        show_help()
        return 0
    if command in ("--list", "-l"):
        list_models(models)
        return 0
    if command in ("--all", "-a"):
        return run_all(argv[1:], models)
    if command in ("--model", "-m"):
        if len(argv) < 2:
            print(f"{RED}Error: Specify a model.{NC}")
            return 1
        return run_model(argv[1], argv[2:], available_models(models))
    if command in ("--group", "-g"):
        if len(argv) < 2:
            print(f"{RED}Error: Specify a group.{NC}")
            print(f"Available groups: {' '.join(COMMAND_GROUPS)}")
            return 1
        return run_group(argv[1], argv[2:], models)
    return run_model(command, argv[1:], available_models(models))


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
