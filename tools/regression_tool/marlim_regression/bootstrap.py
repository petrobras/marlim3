from __future__ import annotations

import argparse
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys

from .config import DATA_ROOT, DEFAULT_PROJECT_ROOT, PACKAGE_ROOT, TOOL_ROOT, initialize_data_directories


FRONTEND_ASSETS = PACKAGE_ROOT / "web"
FRONTEND_SOURCE = TOOL_ROOT / "frontend"


def ensure_supported_python() -> None:
    if sys.version_info < (3, 10):
        raise RuntimeError("Regression Tool requires Python 3.10 or newer")


def ensure_frontend_assets() -> Path:
    """Use the shipped UI, building it only in a source checkout when necessary."""
    index = FRONTEND_ASSETS / "index.html"
    if index.is_file():
        return FRONTEND_ASSETS
    package_json = FRONTEND_SOURCE / "package.json"
    npm = shutil.which("npm")
    if not package_json.is_file():
        raise RuntimeError("The distribution is incomplete: the interface assets were not found")
    if not npm:
        raise RuntimeError(
            "The prebuilt interface is missing. Install Node.js 20+ and run "
            "'uv run install.py', or use a complete release package"
        )
    lockfile = FRONTEND_SOURCE / "package-lock.json"
    install = [npm, "ci"] if lockfile.is_file() else [npm, "install"]
    subprocess.run(install, cwd=FRONTEND_SOURCE, check=True)
    subprocess.run([npm, "run", "build"], cwd=FRONTEND_SOURCE, check=True)
    if not index.is_file():
        raise RuntimeError("The interface build finished without producing the expected assets")
    return FRONTEND_ASSETS


def _linux_install_commands() -> list[list[str]]:
    elevated = [] if hasattr(os, "geteuid") and os.geteuid() == 0 else ["sudo"]
    if shutil.which("apt-get"):
        return [
            [*elevated, "apt-get", "update"],
            [*elevated, "apt-get", "install", "-y", "cmake", "ninja-build", "clang", "llvm", "gfortran", "libomp-dev"],
        ]
    if shutil.which("dnf"):
        return [[*elevated, "dnf", "install", "-y", "cmake", "ninja-build", "clang", "llvm", "gcc-gfortran", "libomp"]]
    if shutil.which("pacman"):
        return [[*elevated, "pacman", "-S", "--needed", "cmake", "ninja", "clang", "llvm", "gcc-fortran", "openmp"]]
    if shutil.which("zypper"):
        return [[*elevated, "zypper", "install", "-y", "cmake", "ninja", "clang", "llvm", "gcc-fortran", "libomp-devel"]]
    return []


def native_install_commands() -> list[list[str]]:
    system = platform.system()
    if system == "Linux":
        return _linux_install_commands()
    if system == "Darwin" and shutil.which("brew"):
        return [["brew", "install", "cmake", "ninja", "llvm", "gcc", "libomp"]]
    if system == "Windows" and shutil.which("winget"):
        return [
            ["winget", "install", "--id", "Kitware.CMake", "--exact", "--accept-package-agreements", "--accept-source-agreements"],
            ["winget", "install", "--id", "Ninja-build.Ninja", "--exact", "--accept-package-agreements", "--accept-source-agreements"],
            ["winget", "install", "--id", "LLVM.LLVM", "--exact", "--accept-package-agreements", "--accept-source-agreements"],
            ["winget", "install", "--id", "BrechtSanders.WinLibs.POSIX.UCRT", "--exact", "--accept-package-agreements", "--accept-source-agreements"],
        ]
    return []


def install_native_dependencies() -> None:
    commands = native_install_commands()
    if not commands:
        raise RuntimeError(
            "No compatible package manager was found. Install CMake, Ninja, Clang/LLVM, "
            "GFortran and OpenMP with the system package manager"
        )
    for command in commands:
        print("Running:", " ".join(command), flush=True)
        subprocess.run(command, check=True)


def missing_native_tools() -> list[str]:
    alternatives = {
        "cmake": ("cmake",),
        "ninja": ("ninja",),
        "clang++": ("clang++", *tuple(f"clang++-{major}" for major in range(22, 12, -1))),
        "llvm-profdata": ("llvm-profdata", *tuple(f"llvm-profdata-{major}" for major in range(22, 12, -1))),
        "llvm-cov": ("llvm-cov", *tuple(f"llvm-cov-{major}" for major in range(22, 12, -1))),
        "gfortran": ("gfortran",),
    }
    return [name for name, commands in alternatives.items() if not any(shutil.which(command) for command in commands)]


def prepare(*, install_native: bool = False) -> list[str]:
    ensure_supported_python()
    if not (DEFAULT_PROJECT_ROOT / "CMakeLists.txt").is_file():
        raise RuntimeError(
            "Place regression_tool inside the Marlim3 root or set MARLIM_PROJECT_ROOT"
        )
    ensure_frontend_assets()
    initialize_data_directories()
    missing = missing_native_tools()
    if missing and install_native:
        install_native_dependencies()
        missing = missing_native_tools()
    return missing


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Prepare the Regression Tool on this computer")
    parser.add_argument(
        "--native-deps",
        action="store_true",
        help="install native dependencies using apt, dnf, pacman, zypper, Homebrew or winget",
    )
    args = parser.parse_args(argv)
    try:
        missing = prepare(install_native=args.native_deps)
    except (OSError, RuntimeError, subprocess.CalledProcessError) as exc:
        print(f"Preparation failed: {exc}", file=sys.stderr)
        return 1
    print(f"Project: {DEFAULT_PROJECT_ROOT}")
    print(f"Local data: {DATA_ROOT}")
    print("Interface: ready")
    if missing:
        print("Missing native dependencies:", ", ".join(missing))
        print("Run again with --native-deps to try installing them automatically.")
        return 2
    print("Native toolchain: ready")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
