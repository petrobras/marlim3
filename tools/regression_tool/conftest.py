"""Make ``marlim_regression`` importable when pytest runs from the repository root."""

from __future__ import annotations

from pathlib import Path
import sys


TOOL_ROOT = Path(__file__).resolve().parent

if str(TOOL_ROOT) not in sys.path:
    sys.path.insert(0, str(TOOL_ROOT))
