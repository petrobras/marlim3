#!/usr/bin/env python3
"""Run a Marlim3 model on one or more engines and summarize the results.

Used to reproduce bug reports: patch a model with --set/--delete, run it on an
official release (--tag, downloaded and checked against its .sha256) and/or a
local build (--exe), and compare exit code, status and the last log events.
Each run gets its own folder (kept for inspection) with the model's auxiliary
files.

Examples:
    python repro_case.py demos/simplifiedProduction/simplifiedProduction.mr3 --tag v3.10.2 \\
        --set initialConfig.transient=true --set initialConfig.initialCondition=1
    python repro_case.py model.mr3 --tag v3.10.2 --exe build/Marlim3 --args -v RULES
    python repro_case.py model.mr3 --exe tools/marlim3/Marlim3 --delete productionPipe
    python repro_case.py parametros.json --tag v3.10.2 --no-input --args -s CONVECNAT
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import platform
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import time
import urllib.error
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
RELEASES = "https://github.com/petrobras/marlim3/releases/download"
PATH_TOKEN = re.compile(r"([^.\[\]]+)|\[(\d+)\]")


def platform_suffix() -> str:
    system, machine = platform.system(), platform.machine().lower()
    if system == "Windows":
        return "windows-x64.exe"
    if system == "Linux" and machine in ("x86_64", "amd64"):
        return "linux-x64"
    if system == "Darwin" and machine in ("arm64", "aarch64"):
        return "macos-arm64"
    raise SystemExit(f"no official engine for {system} {machine}; use --exe")


def fetch(url: str) -> bytes:
    with urllib.request.urlopen(url, timeout=120) as response:
        return response.read()


def official_engine(tag: str, cache: Path) -> Path:
    """Download the release engine once and check it against the published SHA-256."""
    suffix = platform_suffix()
    target = cache / tag / ("Marlim3.exe" if suffix.endswith(".exe") else "Marlim3")
    if target.is_file():
        return target
    # Releases after v3.10.2 carry the tag in the asset name.
    for asset in (f"Marlim3-{tag}-{suffix}", f"Marlim3-{suffix}"):
        try:
            expected = fetch(f"{RELEASES}/{tag}/{asset}.sha256").decode().split()[0].lower()
            data = fetch(f"{RELEASES}/{tag}/{asset}")
        except urllib.error.HTTPError as error:
            if error.code == 404:
                continue
            raise SystemExit(f"could not download {asset} of {tag}: {error}; use --exe")
        except urllib.error.URLError as error:
            raise SystemExit(f"could not download {asset} of {tag}: {error.reason}; use --exe")
        if hashlib.sha256(data).hexdigest() != expected:
            raise SystemExit(f"SHA-256 mismatch for {asset} of {tag}")
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
        target.chmod(0o755)
        return target
    raise SystemExit(f"release {tag} has no engine for {suffix}")


def tokens(path: str):
    return [int(index) if index else key for key, index in PATH_TOKEN.findall(path)]


def set_value(model, path: str, value):
    keys = tokens(path)
    node = model
    for key, following in zip(keys[:-1], keys[1:]):
        if isinstance(key, int):
            node = node[key]
        else:
            node = node.setdefault(key, [] if isinstance(following, int) else {})
    last = keys[-1]
    if isinstance(last, int) and last == len(node):
        node.append(value)
    else:
        node[last] = value


def delete_value(model, path: str):
    keys = tokens(path)
    node = model
    for key in keys[:-1]:
        node = node[key]
    del node[keys[-1]]


def parse_assignment(text: str):
    path, separator, raw = text.partition("=")
    if not separator:
        raise SystemExit(f"--set expects PATH=VALUE, got {text!r}")
    try:
        return path, json.loads(raw)
    except json.JSONDecodeError:
        return path, raw


def prepare_run(model_path: Path, model, folder: Path, input_name: str, copy_aux: bool):
    folder.mkdir(parents=True, exist_ok=True)
    if copy_aux:
        for item in model_path.parent.iterdir():
            if item.is_file() and item != model_path:
                shutil.copy2(item, folder / item.name)
    with open(folder / input_name, "w", encoding="utf-8") as handle:
        json.dump(model, handle, ensure_ascii=False, indent=1)


def run_engine(engine: Path, folder: Path, input_name: str, extra, timeout: float, use_input: bool):
    command = [str(engine), "-d", str(folder)]
    if use_input:
        command += ["-i", input_name]
    command += extra
    start = time.monotonic()
    with open(folder / "stdout.txt", "w", encoding="utf-8", errors="replace") as output:
        process = subprocess.Popen(command, cwd=folder, stdout=output, stderr=subprocess.STDOUT)
        timed_out = False
        try:
            code = process.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            # SIGTERM first, so the engine's signal handler can still write the log.
            timed_out = True
            process.terminate()
            try:
                code = process.wait(timeout=15)
            except subprocess.TimeoutExpired:
                process.kill()
                code = process.wait()
    return command, code, time.monotonic() - start, timed_out


def describe_exit(code: int, timed_out: bool) -> str:
    text = f"exit {code}"
    if code < 0:
        try:
            text += f" ({signal.Signals(-code).name})"
        except ValueError:
            pass
    return text + (" after timeout" if timed_out else "")


def read_log(folder: Path, events: int) -> dict:
    path = folder / "simulacao.log"
    if not path.is_file():
        return {"log": "missing"}
    try:
        data = json.loads(path.read_text(encoding="utf-8-sig", errors="replace"))
    except json.JSONDecodeError as error:
        return {"log": f"unreadable ({error})"}
    result = data.get("resultadoSimulacao", data)
    summary = {key: result[key] for key in ("status", "etapa", "codigoSaida", "tempoSimulado") if key in result}
    summary["events"] = [
        {key: entry.get(key) for key in ("log", "codigo", "descricao", "causa", "etapa") if entry.get(key) not in (None, "")}
        for entry in result.get("logs", [])[-events:]
    ]
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("model", type=Path, help="model file (.mr3/.json); auxiliary files are copied from its folder")
    parser.add_argument("--tag", action="append", default=[], help="official release to run, e.g. v3.10.2 (repeatable)")
    parser.add_argument("--exe", action="append", default=[], type=Path, help="local engine to run (repeatable)")
    parser.add_argument("--set", action="append", default=[], metavar="PATH=VALUE",
                        help="set a key before running; PATH like time.finalTime or crossProductionTrend[0].dt; "
                             "VALUE is JSON or a plain string")
    parser.add_argument("--delete", action="append", default=[], metavar="PATH", help="remove a key before running")
    parser.add_argument("--input-name", help="file name the engine reads (default: the model's name)")
    parser.add_argument("--no-input", action="store_true", help="do not pass -i (e.g. -s CONVECNAT reads parametros.json)")
    parser.add_argument("--no-aux", action="store_true", help="do not copy the other files of the model folder")
    parser.add_argument("--timeout", type=float, default=300, help="seconds before stopping the engine (default 300)")
    parser.add_argument("--threads", type=int, help="OMP_NUM_THREADS for the runs")
    parser.add_argument("--events", type=int, default=3, help="log events shown per run (default 3)")
    parser.add_argument("--workdir", type=Path, help="folder for the runs (default: a new temporary folder)")
    parser.add_argument("--cache", type=Path, default=Path.home() / ".cache" / "marlim3" / "engines",
                        help="where official engines are kept (default ~/.cache/marlim3/engines)")
    parser.add_argument("--json", action="store_true", help="print the summary as JSON")
    parser.add_argument("--args", nargs=argparse.REMAINDER, default=[], help="extra engine arguments, e.g. --args -v RULES")
    args = parser.parse_args()

    model_path = args.model.resolve()
    model = json.loads(model_path.read_text(encoding="utf-8-sig"))
    for assignment in args.set:
        set_value(model, *parse_assignment(assignment))
    for path in args.delete:
        delete_value(model, path)

    engines = [(tag, official_engine(tag, args.cache)) for tag in args.tag]
    engines += [(str(exe), exe.resolve()) for exe in args.exe]
    if not engines:
        local = ROOT / "tools" / "marlim3" / ("Marlim3.exe" if os.name == "nt" else "Marlim3")
        if not local.is_file():
            raise SystemExit("give --tag or --exe (no engine in tools/marlim3/)")
        engines = [("tools/marlim3/" + local.name, local)]
    if args.threads:
        os.environ["OMP_NUM_THREADS"] = str(args.threads)

    workdir = (args.workdir or Path(tempfile.mkdtemp(prefix="marlim3-repro-"))).resolve()
    input_name = args.input_name or model_path.name
    results = []
    for number, (label, engine) in enumerate(engines, 1):
        folder = workdir / f"{number}-{re.sub(r'[^A-Za-z0-9._-]+', '_', Path(label).name)}"
        prepare_run(model_path, model, folder, input_name, not args.no_aux)
        command, code, elapsed, timed_out = run_engine(engine, folder, input_name, args.args, args.timeout, not args.no_input)
        results.append({"engine": label, "command": command, "exit": describe_exit(code, timed_out),
                        "seconds": round(elapsed, 1), "folder": str(folder), **read_log(folder, args.events)})

    if args.json:
        print(json.dumps(results, ensure_ascii=False, indent=1))
        return 0
    changes = len(args.set) + len(args.delete)
    print(f"model: {model_path} ({changes} change{'s' if changes != 1 else ''}); runs in {workdir}")
    for result in results:
        extra = ", ".join(f"{key} {result[key]}" for key in ("etapa", "codigoSaida", "tempoSimulado") if key in result)
        state = f"status {result['status']}" if "status" in result else f"simulacao.log {result.get('log')}"
        print(f"- {result['engine']}: {result['exit']}, {result['seconds']} s, {state}{f' ({extra})' if extra else ''}")
        for event in result.get("events", []):
            text = event.get("descricao", "") + (f" | {event['causa']}" if event.get("causa") else "")
            print(f"    {event.get('log')} {event.get('codigo')}: {text[:160]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
