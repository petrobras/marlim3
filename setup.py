"""
Setup configuration for Marlim3 Python package

The pre-compiled executable is automatically downloaded from GitHub releases
on the first import of the marlim3 package (see marlim3/_download.py).

This setup.py retains build-from-source capability for development.
To compile from source instead of downloading, set:
    MARLIM3_COMPILE_FROM_SOURCE=1
"""

import setuptools
from setuptools.command.build_py import build_py
from setuptools.command.develop import develop
from setuptools.command.install import install
import subprocess
import os
import sys
import shutil
import platform
from pathlib import Path

# Import version from root _version.py
exec(open('_version.py').read())
MARLIM3_VERSION = __version__

def build_executable():
    """Build the C++/Fortran executable from source."""
    print("\n" + "=" * 80)
    print("Building Marlim3 executable from source (C++/Fortran)...")
    print("=" * 80 + "\n")
    sys.stdout.flush()
    
    root_dir = Path(__file__).parent
    src_dir = root_dir / "src"
    python_dir = root_dir
    
    bin_dir = python_dir / "marlim3"
    
    # Create bin directory
    bin_dir.mkdir(parents=True, exist_ok=True)
    
    # Check if we should skip build
    if os.environ.get('MARLIM3_SKIP_BUILD'):
        print("[WARNING] MARLIM3_SKIP_BUILD set, skipping executable build")
        return
    
    # Build with Makefile
    try:
        print("→ Building with Makefile...")
        sys.stdout.flush()
        
        makefile = src_dir / "Makefile"
        if not makefile.exists():
            raise RuntimeError("Makefile not found")
        
        # Run make
        result = subprocess.run(
            #["make", "release"],
            ["make", "clean", "all"],
            cwd=src_dir,
        )
        
        if result.returncode != 0:
            raise RuntimeError("Make failed (see output above)")
        
        print("  [OK] Makefile build successful")
        
        # Copy executable
        exe_name = "Marlim3"
        exe_path = src_dir / exe_name
        if not exe_path.exists():
            # Try other names
            for name in ["marlim3", "Marlim3.exe", "marlim3.exe"]:
                if (src_dir / name).exists():
                    exe_path = src_dir / name
                    break
        
        if exe_path.exists():
            shutil.copy2(exe_path, bin_dir / "Marlim3")
            exe_size = exe_path.stat().st_size / (1024 * 1024)
            print(f"  [OK] Executable copied: {exe_size:.1f} MB")
            print(f"  [OK] Installed to {bin_dir.relative_to(root_dir)}/")
            print("\n[SUCCESS] Build completed successfully!\n")
            return
        else:
            raise RuntimeError("Executable not found after make")
            
    except Exception as e:
        print(f"  [ERROR] Makefile build failed: {e}")
        
        # Check if executable already exists
        if (bin_dir / "Marlim3").exists():
            print("\n[WARNING] Build failed, but executable already exists. Using existing executable.")
            return
        
        # Final failure
        print("\n" + "=" * 80)
        print("[ERROR] EXECUTABLE BUILD FAILED")
        print("=" * 80)
        print("\nMakefile build failed.")
        print("\nTo skip the build and install Python-only:")
        print("  export MARLIM3_SKIP_BUILD=1")
        print("  pip install -e .")
        print("\nOr build manually:")
        print("  cd src")
        print("  make clean all")
        print("=" * 80 + "\n")
        
        if not os.environ.get('MARLIM3_SKIP_EXECUTABLE_CHECK'):
            sys.exit(1)


def get_or_build_executable():
    """Compile executable from source if MARLIM3_COMPILE_FROM_SOURCE is set."""
    # Check if we should skip entirely
    if os.environ.get('MARLIM3_SKIP_BUILD'):
        return
    
    # Check if user wants to compile from source
    if os.environ.get('MARLIM3_COMPILE_FROM_SOURCE'):
        print("[INFO] MARLIM3_COMPILE_FROM_SOURCE set, compiling from source...")
        build_executable()


# Custom build commands that can compile from source if requested
class CustomBuildPy(build_py):
    """Custom build command that can compile the executable from source."""
    def run(self):
        get_or_build_executable()
        build_py.run(self)


class CustomDevelop(develop):
    """Custom develop command that gets the executable."""
    def run(self):
        get_or_build_executable()
        develop.run(self)


class CustomInstall(install):
    """Custom install command that gets the executable."""
    def run(self):
        get_or_build_executable()
        install.run(self)


with open('README.md') as f:
    README = f.read()

requirements = os.path.join(
    os.path.dirname(os.path.realpath(__file__)),
    'requirements.txt'
)

if os.path.isfile(requirements):
    with open(requirements) as f:
        install_requires = f.read().splitlines()
else:
    install_requires = []

# Python package directory
package_dir = {'': '.'}

setuptools.setup(
    name='marlim3',
    version=MARLIM3_VERSION,
    author="Equipe Marlim 3",
    author_email="cc-simuladormarlim3@petrobras.com.br",
    description='Simulação de escoamento multifásico permanente e transiente.',
    long_description=README,
    long_description_content_type='text/markdown',
    url='https://github.com/petrobras/marlim3',
    package_dir=package_dir,
    packages=setuptools.find_packages(where=list(package_dir.values())[0], include=['marlim3', 'marlim3.*']),
    include_package_data=True,
    package_data={
        'marlim3': ['./'],
    },
    python_requires=">=3.12",
    install_requires=install_requires,
    classifiers=[
        "Intended Audience :: Science/Research",
        "Programming Language :: Python :: 3.12",
        "Topic :: Scientific/Engineering",
    ],
    entry_points={
        'console_scripts': [
            # Podemos adicionar scripts CLI aqui se necessário
            # 'marlim3-cli=marlim3.cli:main',
        ],
    },
    cmdclass={
        'build_py': CustomBuildPy,
        'develop': CustomDevelop,
        'install': CustomInstall,
    },
)
