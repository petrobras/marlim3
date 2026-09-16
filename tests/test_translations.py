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
SCHEMA_DIR = Path(__file__).resolve().parent.parent / "docs" / "schemas"
NETWORK_PT = SCHEMA_DIR / "network.pt.json"
NETWORK_EN = SCHEMA_DIR / "network.en.json"

# Core EN->PT network mappings that must exist in translations.json so the
# bilingual Network class and network.en.json/network.pt.json stay in sync.
NETWORK_KEY_MAPPINGS = {
    "version": "versao",
    "initialConfig": "configuracaoInicial",
    "files": "Arquivos",
    "connection": "Conexao",
    "parallelNetworkSource": "fonteRedeParalela",
    "initialHoldupGuess": "ParametroInicial",
    "relaxation": "Relaxacao",
    "guessNodePressures": "ChuteNos",
    "networkTransient": "Transiente",
    "networkFluid": "fluidoRede",
    "simulationTime": "TempoSimulacao",
    "networkThreads": "threadRede",
    "preProcessingOnly": "apenasPreProc",
    "injection": "Injecao",
    "gasLiftRing": "AnelGL",
    "convergenceLimit": "limiteConvergencia",
    "imposedPressure": "PressaoImposta",
    "derivesFromMain": "derivaPrincipal",
    "primaryBranch": "tramoPrimario",
    "steadyStateActive": "permanente",
    "upstreamPressure": "PressaoMontante",
    "downstreamPressure": "PressaoJusante",
    "reverse": "reverso",
    "collectors": "coletores",
    "tributaries": "afluentes",
    "blockage": "bloqueio",
    "ring": "Anel",
    "ringLength": "ComprimentoAnel",
    "primaryNodeId": "idNoPrimario",
    "secondaryNodeId": "idNoSecundario",
}


@pytest.fixture
def translations():
    with open(TRANSLATIONS_FILE, encoding="utf-8") as f:
        return json.load(f)


def _schema_field_names(schema_path: Path) -> set:
    """Flat set of all property names declared in a JSON Schema file."""
    with open(schema_path, encoding="utf-8") as f:
        schema = json.load(f)

    names = set()

    def _walk(node):
        if not isinstance(node, dict):
            return
        for key, value in node.get("properties", {}).items():
            names.add(key)
            if isinstance(value, dict):
                if value.get("type") == "array":
                    items = value.get("items", {})
                    if isinstance(items, dict):
                        _walk(items)
                else:
                    _walk(value)

    _walk(schema)
    return names



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


def test_network_keys_present(translations):
    """All core network EN->PT mappings exist with the expected PT value."""
    keys = translations["keys"]
    missing = {
        en: pt for en, pt in NETWORK_KEY_MAPPINGS.items() if keys.get(en) != pt
    }
    assert not missing, (
        f"Network translation keys missing or mismatched: {missing}"
    )


def test_network_pt_schema_fully_translated(translations):
    """Every field in network.pt.json is reachable through translations.json
    (as a PT value or an identity EN key)."""
    pt_values = set(translations["keys"].values())
    en_keys = set(translations["keys"].keys())
    meta = {"$schema", "layout"}
    fields = _schema_field_names(NETWORK_PT) - meta
    untranslated = {f for f in fields if f not in pt_values and f not in en_keys}
    assert not untranslated, (
        f"network.pt.json fields without a translation entry: {sorted(untranslated)}"
    )


def test_network_en_schema_keys_recognized(translations):
    """Every field in network.en.json is a recognized English key in
    translations.json (identity keys allowed)."""
    en_keys = set(translations["keys"].keys())
    meta = {"$schema", "layout"}
    fields = _schema_field_names(NETWORK_EN) - meta
    unknown = {f for f in fields if f not in en_keys}
    assert not unknown, (
        f"network.en.json fields not recognized as English keys: {sorted(unknown)}"
    )


def test_network_class_bilingual_roundtrip():
    """The Network class translates PT constructor input to English internally
    and can export back to Portuguese."""
    from marlim3._rede._rede import Network

    net = Network(
        configuracaoInicial={"ParametroInicial": 0.5, "Transiente": True},
        Arquivos=["a.mr3"],
        Conexao=[{"PressaoImposta": True, "ativo": True}],
    )
    # English internal access
    assert net.initialConfig["initialHoldupGuess"] == 0.5
    # Portuguese access still works
    assert net.configuracaoInicial["ParametroInicial"] == 0.5
    assert net.connection[0]["imposedPressure"] is True


def test_complementary_water_fluid_type_key_roundtrip():
    """Water-mode complementary fluid uses the tipoF key accepted by the engine."""
    from marlim3._tramo._keys import translate, translate_en_to_pt

    english = {"complementaryFluid": {"active": True, "complementaryFluidType": 1}}
    portuguese = {"fluidoComplementar": {"ativo": True, "tipoF": 1}}

    assert translate(portuguese) == english
    assert translate_en_to_pt(english) == portuguese

