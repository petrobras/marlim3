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

def get_platform_asset_info():
    """Determine which asset to download based on the platform.
    
    Returns tuple: (asset_name, executable_name)
    """
    system = platform.system()
    
    if system == 'Windows':
        return ('Marlim3.exe', 'Marlim3.exe')
    elif system == 'Linux':
        return ('Marlim3', 'Marlim3')
    else:
        raise RuntimeError(f"Unsupported platform: {system}. Only Windows and Linux are supported.")


def get_executable_path():
    """Get the path where the executable should be located."""
    package_dir = Path(__file__).parent
    asset_name, exe_name = get_platform_asset_info()
    return package_dir / exe_name


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


def ensure_executable():
    """Ensure the executable exists, download if necessary.
    
    This is called automatically on package import.
    """
    # Skip if user wants to skip
    if os.environ.get('MARLIM3_SKIP_BUILD'):
        return
    
    # Check if executable exists
    if executable_exists():
        return
    
    # Try to download
    print("\n" + "!" * 80)
    print("MARLIM3: Executable not found. Downloading from GitHub releases...")
    print("!" * 80)
    
    success = download_executable(silent=False)
    
    if not success:
        print("\n" + "!" * 80)
        print("[WARNING] Failed to download Marlim3 executable.")
        print("[WARNING] The package can still be imported, but simulation will not work.")
        print("[WARNING] Set MARLIM3_COMPILE_FROM_SOURCE=1 and reinstall to compile from source.")
        print("!" * 80 + "\n")
