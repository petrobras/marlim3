"""_keys.py — Bilingual (Portuguese ↔ English) JSON key translator.

Used by Branch.from_json() to convert Portuguese-keyed JSON files to
the English key scheme, and by Branch.to_json() to optionally output
in Portuguese.

The C++ engine handles EN→PT translation at simulation time when
``"language": "en"`` is present in the JSON.

Both this module and the C++ JSONKeyTranslator load from the same
source of truth: ``marlim3/translations.json``.
"""
import json
from pathlib import Path

_TRANSLATIONS_FILE = Path(__file__).resolve().parent.parent / "translations.json"


def _load_translations():
    """Load translations.json and build bidirectional maps."""
    with open(_TRANSLATIONS_FILE, encoding="utf-8") as f:
        data = json.load(f)

    en_to_pt = data["keys"]

    # --- PT → EN (for loading Portuguese JSON) ---
    pt_to_en = {}
    for en, pt in en_to_pt.items():
        pt_to_en[pt] = en

    # Identity overrides: abbreviated keys that are the same in both languages.
    _IDENTITY_KEYS = [
        "cdPig", "compP", "compT", "correlacoesPorArranjo",
        "difusTerm3DFE", "fracTermMotorEfic", "mapaArranjo",
        "tabG", "tabP", "taxaDespre", "xCoor", "yCoor",
    ]
    for k in _IDENTITY_KEYS:
        pt_to_en[k] = k

    # --- Value translations (both directions) ---
    # PT→EN values (for loading)
    value_translations_pt_en = {}
    for en_key, mapping in data.get("values", {}).items():
        pt_key = en_to_pt.get(en_key, en_key)
        value_translations_pt_en[pt_key] = {v: k for k, v in mapping.items()}

    # EN→PT values (for saving)
    value_translations_en_pt = {}
    for en_key, mapping in data.get("values", {}).items():
        value_translations_en_pt[en_key] = dict(mapping)

    return pt_to_en, en_to_pt, value_translations_pt_en, value_translations_en_pt


# ---------------------------------------------------------------------------
# Module-level maps (loaded once on import)
# ---------------------------------------------------------------------------
PT_TO_EN, EN_TO_PT, _VALUE_TRANSLATIONS, _VALUE_TRANSLATIONS_EN_PT = (
    _load_translations()
)


def translate(data, _root=False):
    """Recursively translate Portuguese JSON keys (and some values) to English.

    Args:
        data: A JSON-like structure (dict, list, or scalar).
        _root: Internal flag — kept for API compatibility.

    Returns:
        A new structure with all Portuguese keys replaced by English equivalents.
    """
    if isinstance(data, dict):
        result = {}
        for k, v in data.items():
            en_key = PT_TO_EN.get(k, k)
            if k in _VALUE_TRANSLATIONS and isinstance(v, str):
                v = _VALUE_TRANSLATIONS[k].get(v.upper(), v)
            result[en_key] = translate(v, _root=False)
        return result
    elif isinstance(data, list):
        return [translate(item, _root=False) for item in data]
    else:
        return data


def translate_en_to_pt(data):
    """Recursively translate English JSON keys (and enum values) to Portuguese.

    Args:
        data: A JSON-like structure (dict, list, or scalar).

    Returns:
        A new structure with English keys replaced by Portuguese equivalents.
    """
    if isinstance(data, dict):
        result = {}
        for k, v in data.items():
            pt_key = EN_TO_PT.get(k, k)
            if k in _VALUE_TRANSLATIONS_EN_PT and isinstance(v, str):
                v = _VALUE_TRANSLATIONS_EN_PT[k].get(v, v)
            result[pt_key] = translate_en_to_pt(v)
        return result
    elif isinstance(data, list):
        return [translate_en_to_pt(item) for item in data]
    else:
        return data


# ---------------------------------------------------------------------------
# Bilingual containers — accept both PT and EN keys transparently
# ---------------------------------------------------------------------------

class BilingualDict(dict):
    """Dict that transparently accepts both Portuguese and English keys.

    Data is always stored under English keys internally. Lookups via
    Portuguese keys are translated on-the-fly.
    """

    def __init__(self, data=None, **kwargs):
        super().__init__()
        if data is not None:
            if isinstance(data, dict):
                for k, v in data.items():
                    self[k] = v
            else:
                for k, v in data:
                    self[k] = v
        for k, v in kwargs.items():
            self[k] = v

    def __getitem__(self, key):
        en_key = PT_TO_EN.get(key, key)
        return super().__getitem__(en_key)

    def __setitem__(self, key, value):
        en_key = PT_TO_EN.get(key, key)
        # Translate nested PT keys/values via the translate function
        translated = translate({key: value})
        value = translated[en_key]
        super().__setitem__(en_key, _make_bilingual(value))

    def __contains__(self, key):
        en_key = PT_TO_EN.get(key, key)
        return super().__contains__(en_key)

    def get(self, key, default=None):
        en_key = PT_TO_EN.get(key, key)
        return super().get(en_key, default)

    def __delitem__(self, key):
        en_key = PT_TO_EN.get(key, key)
        super().__delitem__(en_key)

    def pop(self, key, *args):
        en_key = PT_TO_EN.get(key, key)
        return super().pop(en_key, *args)

    def setdefault(self, key, default=None):
        en_key = PT_TO_EN.get(key, key)
        if en_key not in self:
            super().__setitem__(en_key, _make_bilingual(default))
        return super().__getitem__(en_key)

    def update(self, other=None, **kwargs):
        if other:
            if hasattr(other, 'items'):
                for k, v in other.items():
                    self[k] = v
            else:
                for k, v in other:
                    self[k] = v
        for k, v in kwargs.items():
            self[k] = v


class BilingualList(list):
    """List that ensures nested dicts/lists are bilingual containers."""

    def __setitem__(self, index, value):
        if isinstance(value, (dict, list)):
            value = translate(value)
        super().__setitem__(index, _make_bilingual(value))

    def append(self, value):
        if isinstance(value, (dict, list)):
            value = translate(value)
        super().append(_make_bilingual(value))

    def extend(self, values):
        super().extend(
            _make_bilingual(translate(v) if isinstance(v, (dict, list)) else v)
            for v in values
        )

    def insert(self, index, value):
        if isinstance(value, (dict, list)):
            value = translate(value)
        super().insert(index, _make_bilingual(value))


def _make_bilingual(value):
    """Recursively wrap dicts and lists in bilingual containers.

    Used internally to wrap already-translated (English-keyed) data without
    re-translating keys.
    """
    if isinstance(value, BilingualDict):
        return value
    if isinstance(value, dict):
        bd = BilingualDict.__new__(BilingualDict)
        dict.__init__(bd)
        for k, v in value.items():
            dict.__setitem__(bd, k, _make_bilingual(v))
        return bd
    if isinstance(value, BilingualList):
        return value
    if isinstance(value, list):
        bl = BilingualList.__new__(BilingualList)
        list.__init__(bl, [_make_bilingual(item) for item in value])
        return bl
    return value
