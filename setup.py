"""
Custom build hooks for the Marlim3 C++/Fortran executable.

All package metadata and dependencies are declared in pyproject.toml.
This file only provides the CMake build integration via setuptools hooks.

To compile from source instead of downloading, set:
    MARLIM3_COMPILE_FROM_SOURCE=1

The CMake preset is controlled by:
    MARLIM3_CMAKE_PRESET  (default: gcc-release)
"""

from setuptools import setup
from setuptools.command.build_py import build_py
from setuptools.command.develop import develop
from setuptools.command.install import install
import subprocess
import os
import sys
import shutil
import platform
from pathlib import Path

# Read version from root _version.py (single source of truth)
_version_file = Path(__file__).parent / '_version.py'
_version_dict = {}
with open(_version_file) as f:
    exec(f.read(), _version_dict)
MARLIM3_VERSION = _version_dict['__version__']


def build_executable():
    """Build the C++/Fortran executable from source using CMake."""
    print("\n" + "=" * 80)
    print("Building Marlim3 executable from source (C++/Fortran via CMake)...")
    print("=" * 80 + "\n")
    sys.stdout.flush()

    root_dir = Path(__file__).parent
    build_dir = root_dir / "build"
    package_dir = root_dir / "marlim3"

    package_dir.mkdir(parents=True, exist_ok=True)

    if os.environ.get('MARLIM3_SKIP_BUILD'):
        print("[WARNING] MARLIM3_SKIP_BUILD set, skipping executable build")
        return

    cmake_preset = os.environ.get('MARLIM3_CMAKE_PRESET', 'gcc-release')

    try:
        print(f"→ Configuring with CMake preset '{cmake_preset}'...")
        sys.stdout.flush()
        result = subprocess.run(["cmake", "--preset", cmake_preset], cwd=root_dir)
        if result.returncode != 0:
            raise RuntimeError("CMake configure failed")

        print("→ Building...")
        sys.stdout.flush()
        result = subprocess.run(["cmake", "--build", "--preset", cmake_preset], cwd=root_dir)
        if result.returncode != 0:
            raise RuntimeError("CMake build failed")

        print("  [OK] CMake build successful")

        exe_name = "Marlim3.exe" if platform.system() == "Windows" else "Marlim3"
        exe_path = build_dir / exe_name
        if not exe_path.exists():
            for candidate in [root_dir / exe_name, build_dir / "Release" / exe_name]:
                if candidate.exists():
                    exe_path = candidate
                    break

        if exe_path.exists():
            dest = package_dir / exe_name
            shutil.copy2(exe_path, dest)
            exe_size = exe_path.stat().st_size / (1024 * 1024)
            print(f"  [OK] Executable copied: {exe_size:.1f} MB → {dest.relative_to(root_dir)}")
            print("\n[SUCCESS] Build completed successfully!\n")
        else:
            raise RuntimeError("Executable not found after CMake build")

    except Exception as e:
        print(f"  [ERROR] CMake build failed: {e}")

        if (package_dir / "Marlim3").exists():
            print("\n[WARNING] Build failed, but executable already exists. Using existing.")
            return

        print("\n[ERROR] EXECUTABLE BUILD FAILED")
        print("To build manually:  cmake --preset gcc-release && cmake --build --preset gcc-release")
        print("To skip build:      export MARLIM3_SKIP_BUILD=1 && uv sync")

        if not os.environ.get('MARLIM3_SKIP_EXECUTABLE_CHECK'):
            sys.exit(1)


def get_or_build_executable():
    """Compile from source if MARLIM3_COMPILE_FROM_SOURCE is set."""
    if os.environ.get('MARLIM3_SKIP_BUILD'):
        return
    if os.environ.get('MARLIM3_COMPILE_FROM_SOURCE'):
        build_executable()


class CustomBuildPy(build_py):
    def run(self):
        get_or_build_executable()
        super().run()


class CustomDevelop(develop):
    def run(self):
        get_or_build_executable()
        super().run()


class CustomInstall(install):
    def run(self):
        get_or_build_executable()
        super().run()


# Only version and cmdclass — all other metadata is in pyproject.toml
setup(
    version=MARLIM3_VERSION,
    cmdclass={
        'build_py': CustomBuildPy,
        'develop': CustomDevelop,
        'install': CustomInstall,
    },
)
