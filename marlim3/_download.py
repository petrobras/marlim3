"""
Automatic download of Marlim3 executable from GitHub releases.

This module is called automatically on the first import of marlim3 package.
If the executable is not found, it will be downloaded from the GitHub release.
"""

import os
import sys
import platform
import urllib.request
from pathlib import Path


def _env_flag(name):
    return os.environ.get(name, "").strip().lower() not in {"", "0", "false", "no", "off"}


def get_platform_asset_info():
    """Determine which asset to download based on the platform.
    
    Returns tuple: (asset_name, executable_name)
    """
    system = platform.system()
    machine = platform.machine().lower()
    
    if system == 'Windows':
        return ('Marlim3.exe', 'Marlim3.exe')
    elif system == 'Linux':
        return ('Marlim3-linux-x64', 'Marlim3')
    elif system == 'Darwin':  # macOS
        if machine in ['arm64', 'aarch64']:
            return ('Marlim3-macos-arm64', 'Marlim3')
        elif machine in ['x86_64', 'amd64']:
            raise RuntimeError("macOS Intel is not supported by the Marlim3 prebuilt executable.")
        else:
            raise RuntimeError(f"Unsupported macOS architecture: {machine}")
    else:
        raise RuntimeError(f"Unsupported platform: {system}. Supported prebuilt platforms: Windows x64, Linux x64, macOS ARM64.")


def get_executable_path():
    """Get the path where the executable should be located.

    Checks in order:
    1. marlim3/ (package directory — installed or downloaded)
    2. build/   (local CMake build, avoids needing to copy)
    """
    package_dir = Path(__file__).parent
    asset_name, exe_name = get_platform_asset_info()

    pkg_exe = package_dir / exe_name
    if pkg_exe.exists():
        return pkg_exe

    # Also accept the executable produced by a local CMake build
    build_exe = package_dir.parent / 'build' / exe_name
    if build_exe.exists():
        return build_exe

    # Default location (does not exist yet — will be downloaded)
    return pkg_exe


def executable_exists():
    """Check if the Marlim3 executable exists."""
    exe_path = get_executable_path()
    return exe_path.exists()


def download_executable(silent=False):
    """Download pre-compiled executable from GitHub releases.
    
    Args:
        silent: If True, suppress most output messages
    """
    # Import version here to avoid circular dependency
    from . import __version__
    
    def log(msg):
        if not silent:
            print(msg)
            sys.stdout.flush()
    
    package_dir = Path(__file__).parent
    asset_name, exe_name = get_platform_asset_info()
    exe_path = package_dir / exe_name
    
    # Check if executable already exists
    if exe_path.exists() and not os.environ.get('MARLIM3_FORCE_DOWNLOAD'):
        log(f"Marlim3 executable already exists at: {exe_path}")
        return True
    
    log("\n" + "=" * 80)
    log(f"Downloading Marlim3 executable (v{__version__}) from GitHub releases...")
    log("This only happens once on first import.")
    log("=" * 80)
    
    try:
        # Build download URL
        download_url = f"https://github.com/petrobras/marlim3/releases/download/v{__version__}/{asset_name}"
        
        log(f"Downloading from: {download_url}")
        log("Please wait...")
        
        # Download file directly to the package directory
        urllib.request.urlretrieve(download_url, exe_path)
        
        file_size = exe_path.stat().st_size / (1024 * 1024)
        log(f"[OK] Downloaded: {file_size:.1f} MB")
        
        # Ensure executable has correct permissions on Unix-like systems
        if platform.system() != 'Windows':
            exe_path.chmod(0o755)
            log("[OK] Set executable permissions")
        
        log(f"[OK] Executable installed at: {exe_path}")
        log("=" * 80 + "\n")
        return True
        
    except urllib.error.HTTPError as e:
        log(f"[ERROR] Download failed: HTTP {e.code} - {e.reason}")
        log(f"[ERROR] URL: {download_url}")
        log("\nPossible reasons:")
        log(f"  - Release v{__version__} not found on GitHub")
        log("  - No internet connection")
        log("  - GitHub API rate limit exceeded")
        log("\nTo compile from source instead, set environment variable:")
        log("  MARLIM3_COMPILE_FROM_SOURCE=1")
        return False
        
    except Exception as e:
        log(f"[ERROR] Download failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def _check_executable_runs():
    """Check if the executable can actually run on this system (e.g. GLIBC compatibility)."""
    import subprocess
    exe_path = get_executable_path()
    if not exe_path.exists():
        return False
    try:
        result = subprocess.run(
            [str(exe_path), "--help"],
            capture_output=True, timeout=5,
        )
        # Even if exit code != 0, if it ran at all it's compatible
        return True
    except (OSError, subprocess.TimeoutExpired):
        return False
    except Exception:
        return False


def _try_build_from_source():
    """Attempt to build the executable from source using CMake."""
    import subprocess
    import shutil

    if _env_flag('MARLIM3_SKIP_BUILD'):
        return False

    package_dir = Path(__file__).parent
    root_dir = package_dir.parent

    cmake_lists = root_dir / "CMakeLists.txt"
    if not cmake_lists.exists():
        return False

    if not shutil.which("cmake"):
        return False

    cmake_preset = os.environ.get('MARLIM3_CMAKE_PRESET', 'gcc-release')

    try:
        print("[INFO] Compiling Marlim3 from source...")
        sys.stdout.flush()

        result = subprocess.run(
            ["cmake", "--preset", cmake_preset],
            cwd=root_dir, capture_output=True,
        )
        if result.returncode != 0:
            return False

        import multiprocessing
        nproc = str(multiprocessing.cpu_count())
        result = subprocess.run(
            ["cmake", "--build", "--preset", cmake_preset, f"-j{nproc}"],
            cwd=root_dir, capture_output=True,
        )
        if result.returncode != 0:
            return False

        exe_name = "Marlim3.exe" if platform.system() == "Windows" else "Marlim3"
        built_exe = root_dir / "build" / exe_name
        if built_exe.exists():
            dest = package_dir / exe_name
            shutil.copy2(built_exe, dest)
            if platform.system() != "Windows":
                dest.chmod(0o755)
            print("[OK] Compiled and installed Marlim3 from source.")
            return True
    except Exception:
        pass

    return False


def ensure_executable():
    """Ensure the executable exists, download if necessary.
    
    This is called automatically on package import.
    """
    if _env_flag('MARLIM3_SKIP_EXECUTABLE_RESOLUTION'):
        return
    
    # Check if executable exists and works
    try:
        has_executable = executable_exists()
    except RuntimeError as e:
        print("\n" + "!" * 80)
        print(f"[WARNING] {e}")
        print("[INFO] Attempting to build from source...")
        print("!" * 80)

        if _try_build_from_source():
            return

        print("\n" + "!" * 80)
        print("[WARNING] The package can still be imported, but simulation will not work.")
        print("[WARNING] Compile Marlim3 from source to use simulations on this platform.")
        print("!" * 80 + "\n")
        return

    if has_executable and _check_executable_runs():
        return

    if has_executable:
        # Exists but doesn't run (e.g. GLIBC mismatch)
        print("\n" + "!" * 80)
        print("[WARNING] Marlim3 executable exists but cannot run on this system.")
        print("[WARNING] This is likely a GLIBC version mismatch.")
        print("[INFO] Attempting to build from source...")
        print("!" * 80)

        if _try_build_from_source():
            return

        print("\n" + "!" * 80)
        print("[WARNING] Could not build from source automatically.")
        print("[WARNING] To fix, compile manually:")
        print("           cmake --preset gcc-release")
        print("           cmake --build --preset gcc-release -j$(nproc)")
        print("           cp build/Marlim3 marlim3/")
        print("!" * 80 + "\n")
        return

    # Try to download
    print("\n" + "!" * 80)
    print("MARLIM3: Executable not found. Downloading from GitHub releases...")
    print("!" * 80)
    
    try:
        success = download_executable(silent=False)
    except RuntimeError as e:
        print("\n" + "!" * 80)
        print(f"[WARNING] {e}")
        print("[INFO] Attempting to build from source...")
        print("!" * 80)

        if _try_build_from_source():
            return

        print("\n" + "!" * 80)
        print("[WARNING] The package can still be imported, but simulation will not work.")
        print("[WARNING] Compile Marlim3 from source to use simulations on this platform.")
        print("!" * 80 + "\n")
        return
    
    if not success:
        print("\n" + "!" * 80)
        print("[WARNING] Failed to download Marlim3 executable.")
        print("[WARNING] The package can still be imported, but simulation will not work.")
        print("[WARNING] Set MARLIM3_COMPILE_FROM_SOURCE=1 and reinstall to compile from source.")
        print("!" * 80 + "\n")
        return

    # Verify downloaded binary actually works
    if not _check_executable_runs():
        print("\n" + "!" * 80)
        print("[WARNING] Downloaded executable is not compatible with this system.")
        print("[INFO] Attempting to build from source...")
        print("!" * 80)

        if _try_build_from_source():
            return

        print("\n" + "!" * 80)
        print("[WARNING] Could not build from source automatically.")
        print("[WARNING] To fix, compile manually:")
        print("           cmake --preset gcc-release")
        print("           cmake --build --preset gcc-release -j$(nproc)")
        print("           cp build/Marlim3 marlim3/")
        print("!" * 80 + "\n")
