from __future__ import annotations

import argparse
import json
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("model")
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--output-log", type=Path, required=True)
    parser.add_argument("--json", action="store_true")
    return parser.parse_args()


def result(status: str, code: str = "", reason: str = "") -> dict[str, str]:
    return {"status": status, "code": code, "reason": reason}


def read_json_log(path: Path) -> tuple[dict, str | None]:
    try:
        with path.open(encoding="utf-8", errors="replace") as fp:
            return json.load(fp), None
    except OSError as exc:
        return {}, str(exc)
    except json.JSONDecodeError as exc:
        return {}, str(exc)


def check_result(output_log: Path, out_dir: Path) -> dict[str, str]:
    fallback_log = out_dir / "resultado.log"
    candidates = [output_log]
    if fallback_log != output_log:
        candidates.append(fallback_log)

    readable_logs = [path for path in candidates if path.is_file() and path.stat().st_size > 0]
    if not readable_logs:
        return result("FAILED", "LOG_NOT_FOUND", f"log nao encontrado: {output_log}")

    for path in readable_logs:
        data, error = read_json_log(path)
        if error is not None:
            continue

        simulation_result = data.get("resultadoSimulacao", {})
        failures = [
            entry
            for entry in simulation_result.get("logs", [])
            if str(entry.get("log", "")).upper() == "FALHA"
        ]

        if failures:
            failure = failures[0]
            code = str(failure.get("descricao") or "FALHA")
            reason = str(failure.get("causa") or failure.get("descricao") or "falha sem motivo informado")
            return result("FAILED", code, reason)

        return result("PASSED")

    return result("FAILED", "INVALID_LOG", f"log nao contem JSON valido: {readable_logs[0]}")


def main() -> int:
    args = parse_args()
    payload = check_result(args.output_log, args.out_dir)

    if args.json:
        print(json.dumps(payload, ensure_ascii=False))
    else:
        print(payload["status"])
        if payload["status"] != "PASSED":
            print(payload["code"])
            print(payload["reason"])

    return 0 if payload["status"] == "PASSED" else 1


if __name__ == "__main__":
    raise SystemExit(main())
