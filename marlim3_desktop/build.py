#!/usr/bin/env python3
"""Build one standalone Marlim3 desktop artifact for the current platform."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import platform
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from .checkpoints import REQUIRED_SMOKE_CHECKPOINTS


GLIBC_BASELINE = (2, 34)


ROOT = Path(__file__).resolve().parents[1]
DIST_DIR = ROOT / "dist"
WORK_DIR = ROOT / "build" / "desktop"
SPEC_FILE = ROOT / "marlim3_desktop" / "packaging" / "marlim3-desktop.spec"


def run(
    command: list[str],
    *,
    env: dict[str, str] | None = None,
    cwd: Path = ROOT,
    timeout: int | None = None,
) -> None:
    print(f"+ {' '.join(command)}", flush=True)
    subprocess.run(command, cwd=cwd, env=env, check=True, timeout=timeout)


def normalized_machine() -> str:
    machine = platform.machine().lower()
    return {"amd64": "x86_64", "x64": "x86_64", "aarch64": "arm64"}.get(
        machine, machine
    )


def platform_config() -> tuple[str, str, str]:
    supported = {
        ("Linux", "x86_64"): ("gcc-release", "Marlim3", "Marlim3-desktop-linux-x64"),
        ("Windows", "x86_64"): (
            "mingw-release",
            "Marlim3.exe",
            "Marlim3-desktop-windows-x64.exe",
        ),
        ("Darwin", "arm64"): (
            "gcc-release",
            "Marlim3",
            "Marlim3-desktop-macos-arm64.dmg",
        ),
    }
    key = (platform.system(), normalized_machine())
    if key not in supported:
        raise RuntimeError(
            f"Unsupported desktop build platform: {key[0]} {key[1]}. "
            "Supported targets are Linux x64, Windows x64, and macOS ARM64."
        )
    return supported[key]


def require_tool(name: str) -> None:
    if shutil.which(name) is None:
        raise RuntimeError(f"Required tool not found in PATH: {name}")


def build_engine(preset: str, engine_name: str) -> None:
    require_tool("cmake")
    run(["cmake", "--preset", preset])
    run(["cmake", "--build", "--preset", preset, f"-j{os.cpu_count() or 1}"])
    if not (ROOT / "marlim3" / engine_name).is_file():
        raise FileNotFoundError(f"Native engine was not generated: marlim3/{engine_name}")


def sync_dependencies() -> None:
    require_tool("uv")
    if os.environ.get("MARLIM_DEPENDENCIES_PREPARED") == "1":
        return
    env = os.environ.copy()
    env["MARLIM3_SKIP_BUILD"] = "1"
    env["MARLIM3_SKIP_EXECUTABLE_RESOLUTION"] = "1"
    if env.get("MARLIM_DEPENDENCY_MODE") == "corporate":
        run(
            [
                sys.executable,
                str(ROOT / "scripts" / "dependencies.py"),
                "sync",
                "--mode", "corporate",
                "--no-dev",
                "--group", "gui",
                "--group", "desktop",
            ],
            env=env,
        )
        return
    run(
        [
            "uv",
            "sync",
            "--locked",
            "--no-dev",
            "--group",
            "gui",
            "--group",
            "desktop",
        ],
        env=env,
    )


def build_pyinstaller_bundle() -> None:
    require_tool("uv")
    shutil.rmtree(DIST_DIR, ignore_errors=True)
    DIST_DIR.mkdir(parents=True)
    run(
        [
            "uv",
            "run",
            "--no-sync",
            "--no-dev",
            "--group",
            "gui",
            "--group",
            "desktop",
            "pyinstaller",
            str(SPEC_FILE),
            "--noconfirm",
            "--clean",
        ]
    )


def validation_environment() -> dict[str, str]:
    env = os.environ.copy()
    if platform.system() == "Linux":
        if env.get("DISPLAY"):
            env.setdefault("QT_QPA_PLATFORM", "xcb")
        else:
            env.setdefault("QT_QPA_PLATFORM", "offscreen")
    elif platform.system() == "Darwin":
        env.pop("QT_QPA_PLATFORM", None)
    env.setdefault("QTWEBENGINE_DISABLE_SANDBOX", "1")
    return env


def verify_glibc_baseline(executable: Path) -> None:
    result = subprocess.run(
        ["objdump", "-T", str(executable)],
        capture_output=True,
        text=True,
        check=False,
    )
    symbols = re.findall(r"GLIBC_(\d+)\.(\d+)", result.stdout)
    if not symbols:
        return
    newest = max((int(major), int(minor)) for major, minor in symbols)
    if newest > GLIBC_BASELINE:
        required = ".".join(str(part) for part in newest)
        baseline = ".".join(str(part) for part in GLIBC_BASELINE)
        raise RuntimeError(
            f"Desktop bootloader requires glibc {required}, newer than the "
            f"supported {baseline} baseline"
        )


def write_checksum(artifact: Path) -> Path:
    digest = hashlib.sha256(artifact.read_bytes()).hexdigest()
    checksum = artifact.with_name(artifact.name + ".sha256")
    checksum.write_text(f"{digest}  {artifact.name}\n", encoding="utf-8")
    return checksum


def validate_executable(executable: Path, smoke_test: bool) -> None:
    run([str(executable), "--print-config"], timeout=90)
    if not smoke_test:
        return
    with tempfile.TemporaryDirectory(prefix="marlim3-smoke-") as temp_dir:
        report = Path(temp_dir) / "checkpoints.jsonl"
        run(
            [
                str(executable),
                "--window-smoke-test",
                "--checkpoint-file",
                str(report),
            ],
            env=validation_environment(),
            timeout=120,
        )
        checkpoints = {
            json.loads(line)["checkpoint"]
            for line in report.read_text(encoding="utf-8").splitlines()
        }
        missing = set(REQUIRED_SMOKE_CHECKPOINTS) - checkpoints
        if missing:
            raise RuntimeError(
                f"Desktop smoke test missed checkpoints: {', '.join(sorted(missing))}"
            )


def validate_linux(smoke_test: bool) -> Path:
    source = DIST_DIR / "Marlim3-Desktop"
    if not source.is_file():
        raise FileNotFoundError(f"PyInstaller output not found: {source}")
    verify_glibc_baseline(source)
    validate_executable(source, smoke_test)
    return source


def validate_windows(smoke_test: bool) -> Path:
    source = DIST_DIR / "Marlim3-Desktop.exe"
    if not source.is_file():
        raise FileNotFoundError(f"PyInstaller output not found: {source}")
    validate_executable(source, smoke_test)
    return source


def verify_macos_bundle(app_bundle: Path) -> None:
    executable = app_bundle / "Contents" / "MacOS" / "Marlim3"
    run(["lipo", str(executable), "-verify_arch", "arm64"])
    icon_file = subprocess.run(
        ["plutil", "-extract", "CFBundleIconFile", "raw",
         str(app_bundle / "Contents" / "Info.plist")],
        capture_output=True,
        text=True,
        check=True,
    ).stdout.strip()
    if not icon_file:
        raise RuntimeError("Info.plist does not declare CFBundleIconFile")
    if not (app_bundle / "Contents" / "Resources" / icon_file).is_file():
        raise FileNotFoundError(f"App bundle icon is missing: {icon_file}")


def validate_macos(smoke_test: bool) -> Path:
    app_bundle = DIST_DIR / "Marlim3.app"
    executable = app_bundle / "Contents" / "MacOS" / "Marlim3"
    if not executable.is_file():
        raise FileNotFoundError(f"PyInstaller app bundle not found: {app_bundle}")
    verify_macos_bundle(app_bundle)
    validate_executable(executable, smoke_test)
    return app_bundle


def package_linux() -> Path:
    artifact = DIST_DIR / "Marlim3-desktop-linux-x64"
    (DIST_DIR / "Marlim3-Desktop").replace(artifact)
    write_checksum(artifact)
    return artifact


def package_windows() -> Path:
    artifact = DIST_DIR / "Marlim3-desktop-windows-x64.exe"
    shutil.copy2(DIST_DIR / "Marlim3-Desktop.exe", artifact)
    write_checksum(artifact)
    return artifact


def package_macos() -> Path:
    app_bundle = DIST_DIR / "Marlim3.app"
    require_tool("hdiutil")
    dmg_root = WORK_DIR / "dmg-root"
    shutil.rmtree(dmg_root, ignore_errors=True)
    dmg_root.mkdir(parents=True)
    shutil.copytree(app_bundle, dmg_root / "Marlim3.app")
    (dmg_root / "Applications").symlink_to("/Applications")
    artifact = DIST_DIR / "Marlim3-desktop-macos-arm64.dmg"
    run(
        [
            "hdiutil",
            "create",
            "-volname",
            "Marlim3",
            "-srcfolder",
            str(dmg_root),
            "-ov",
            "-format",
            "UDZO",
            str(artifact),
        ]
    )
    shutil.rmtree(app_bundle)
    write_checksum(artifact)
    return artifact


VALIDATORS = {"Linux": validate_linux, "Windows": validate_windows}
PACKAGERS = {"Linux": package_linux, "Windows": package_windows}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--skip-engine-build", action="store_true")
    parser.add_argument("--skip-dependency-sync", action="store_true")
    parser.add_argument("--skip-smoke-test", action="store_true")
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="Reuse a PyInstaller bundle produced by a previous run.",
    )
    parser.add_argument(
        "--skip-validate",
        action="store_true",
        help="Skip the print-config and smoke-test validation.",
    )
    parser.add_argument(
        "--skip-package",
        action="store_true",
        help="Stop before producing the renamed artifact and checksum.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    system = platform.system()
    preset, engine_name, expected_artifact = platform_config()
    validate = VALIDATORS.get(system, validate_macos)
    package = PACKAGERS.get(system, package_macos)

    if not args.skip_build:
        print(
            f"Building Marlim3 desktop for {system} {normalized_machine()}",
            flush=True,
        )
        if not args.skip_engine_build:
            build_engine(preset, engine_name)
        elif not (ROOT / "marlim3" / engine_name).is_file():
            raise FileNotFoundError(
                f"--skip-engine-build requires marlim3/{engine_name} to exist"
            )
        if not args.skip_dependency_sync:
            sync_dependencies()
        build_pyinstaller_bundle()

    if not args.skip_validate:
        validate(not args.skip_smoke_test)

    if args.skip_package:
        return 0

    artifact = package()
    if artifact.name != expected_artifact or not artifact.is_file():
        raise RuntimeError(f"Expected desktop artifact was not generated: {artifact}")
    print(f"Desktop artifact: {artifact}", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
