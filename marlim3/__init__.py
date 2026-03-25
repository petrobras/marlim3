"""
Marlim3 - Simulacao de escoamento multifasico permanente e transiente.
"""

# Import version from root _version.py
import sys
from pathlib import Path

# Try to import from root _version.py (for development)
try:
    _root_dir = Path(__file__).parent.parent
    if (_root_dir / '_version.py').exists():
        exec(open(_root_dir / '_version.py').read())
    else:
        # Fallback for installed package
        __version__ = '3.3.0'
except:
    # Fallback
    __version__ = '3.3.0'

# Ensure executable is downloaded on first import
from ._download import ensure_executable
ensure_executable()

# Import main classes
from ._tramo._tramo import Tramo
from ._rede._rede import Rede
from ._cenarios._cenarios import Cenarios

__all__ = ['Tramo','Rede','Cenarios', '__version__']