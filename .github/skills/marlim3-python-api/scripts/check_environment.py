"""Environment preflight for the Marlim3 agents: stop up front instead of failing mid-session.

Usage, from the repository root:

    uv run python .github/skills/marlim3-python-api/scripts/check_environment.py

Reaching the script already proves that uv (the version the project pins) and the project
environment work. It then checks that jsonschema (used by check_case.py) is installed and that
the simulator executable exists and starts on this machine, without downloading or building it. Prints READY, or one
NOT READY line per problem with how to fix it, and exits with status 1 when not ready.
"""
import os
import subprocess
import sys

# Report the environment as it is: importing marlim3 must not download or build the executable.
os.environ.setdefault("MARLIM3_SKIP_EXECUTABLE_RESOLUTION", "1")

from marlim3._download import executable_exists, get_executable_path  # noqa: E402

BUILD = "build it from source (docs/getting-started.md#building-from-source)"
INSTALL = f"install it once, outside the agent session: `uv run python -c \"import marlim3\"` downloads it, or {BUILD}"


def problems():
    found = []
    try:
        import jsonschema  # noqa: F401 — check_case.py validates with it
    except ImportError:
        found.append("jsonschema is not installed — it ships with the project environment: `uv sync --locked`")
    try:
        path = get_executable_path()
    except RuntimeError as exc:  # no prebuilt executable for this platform
        return found + [f"{exc} — {BUILD}"]
    if not executable_exists():
        return found + [f"the Marlim3 executable is not in marlim3/ or build/ — {INSTALL}"]
    try:
        subprocess.run([str(path), "--help"], capture_output=True, timeout=5)
    except (OSError, subprocess.TimeoutExpired) as exc:
        found.append(f"{path} exists but does not run on this machine ({exc}) — {INSTALL}")
    return found


def main():
    found = problems()
    for problem in found:
        print(f"NOT READY: {problem}")
    if not found:
        print(f"READY: Marlim3 executable at {get_executable_path()}")
    return 1 if found else 0


if __name__ == "__main__":
    sys.exit(main())
