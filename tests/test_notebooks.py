"""
Execute tutorial notebooks as integration tests.

Each notebook is run top-to-bottom with nbconvert's ExecutePreprocessor.
Requires: compiled Marlim3.exe, scipy (for gas_lift notebook).
"""

from pathlib import Path

import nbformat
import pytest
from nbconvert.preprocessors import ExecutePreprocessor

TUTORIALS_DIR = Path(__file__).resolve().parent.parent / "docs" / "tutorials"
NOTEBOOKS = sorted(TUTORIALS_DIR.glob("*.ipynb"))


@pytest.mark.simulacao
@pytest.mark.parametrize("notebook", NOTEBOOKS, ids=lambda p: p.stem)
def test_notebook_execution(notebook):
    """Execute a tutorial notebook and assert no cells raise."""
    with open(notebook, encoding="utf-8") as f:
        nb = nbformat.read(f, as_version=4)

    ep = ExecutePreprocessor(timeout=600, kernel_name="python3")
    ep.preprocess(nb, {"metadata": {"path": str(notebook.parent)}})
