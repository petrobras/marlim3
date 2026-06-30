"""
Marlim3 - Simulacao de escoamento multifasico permanente e transiente.
"""

# Import version from root _version.py
from importlib.metadata import PackageNotFoundError, version
from pathlib import Path

# Try to import from root _version.py (for development)
try:
    _root_dir = Path(__file__).parent.parent
    if (_root_dir / '_version.py').exists():
        exec(open(_root_dir / '_version.py').read())
    else:
        __version__ = version("marlim3")
except PackageNotFoundError:
    __version__ = '3.3.0'

# Ensure executable is downloaded on first import
from ._download import ensure_executable
ensure_executable()

# Import main classes
from ._tramo._branch import Branch
from ._rede._rede import Network
from ._cenarios._cenarios import Scenarios

# Import tpl/ppl converter
from ._conversores._conversor_marlim3_tplppl import convert_to_ppl_tpl

Tramo = Branch  # Portuguese alias

__all__ = ['Branch', 'Tramo', 'Network', 'Scenarios', 
           'convert_to_ppl_tpl', '__version__']
