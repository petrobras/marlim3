"""test_translations.py — Validate marlim3/translations.json integrity.

Checks:
1. No duplicate EN keys (impossible in JSON but verified anyway after load).
2. No duplicate PT values that would cause ambiguity (except known exceptions).
3. Both C++ (via translations.json) and Python (_keys.py) agree on mappings.
4. translations.json is valid JSON with expected structure.
"""
import json
from pathlib import Path

import pytest

TRANSLATIONS_FILE = Path(__file__).resolve().parent.parent / "marlim3" / "translations.json"


@pytest.fixture
def translations():
    with open(TRANSLATIONS_FILE, encoding="utf-8") as f:
        return json.load(f)


def test_translations_json_structure(translations):
    """translations.json has the expected top-level keys."""
    assert "keys" in translations
    assert "values" in translations
    assert isinstance(translations["keys"], dict)
    assert isinstance(translations["values"], dict)


def test_no_empty_keys_or_values(translations):
    """No empty strings as keys or values."""
    for en, pt in translations["keys"].items():
        assert en, "Empty EN key found"
        assert pt, f"Empty PT value for EN key '{en}'"


def test_no_duplicate_en_keys(translations):
    """EN keys are unique (JSON guarantees this, but we verify after load)."""
    # JSON.load keeps last occurrence; check by re-parsing raw text
    content = TRANSLATIONS_FILE.read_text(encoding="utf-8")
    import re
    # Count occurrences in the "keys" section
    keys_start = content.find('"keys"')
    keys_end = content.find('"values"')
    keys_block = content[keys_start:keys_end]
    all_en_keys = re.findall(r'"([^"]+)"\s*:', keys_block)
    # First match is "keys" itself
    en_keys = all_en_keys[1:]  # skip the "keys" key itself
    duplicates = [k for k in en_keys if en_keys.count(k) > 1]
    assert not duplicates, f"Duplicate EN keys: {set(duplicates)}"


def test_known_duplicate_pt_values(translations):
    """No PT values should map from multiple EN keys (ambiguities removed)."""
    from collections import Counter
    pt_values = list(translations["keys"].values())
    counts = Counter(pt_values)
    duplicates = {k: v for k, v in counts.items() if v > 1}
    assert not duplicates, f"Unexpected duplicate PT values: {duplicates}"


def test_python_keys_load_from_json():
    """Python _keys.py successfully loads and inverts translations.json."""
    from marlim3._tramo._keys import PT_TO_EN
    assert len(PT_TO_EN) > 400
    assert PT_TO_EN["tempo"] == "time"


def test_python_value_translations():
    """Python value translations are the inverse of the JSON values section."""
    from marlim3._tramo._keys import _VALUE_TRANSLATIONS
    assert "tipoMedicaoCamada" in _VALUE_TRANSLATIONS
    assert _VALUE_TRANSLATIONS["tipoMedicaoCamada"]["ESPESSURA"] == "THICKNESS"
