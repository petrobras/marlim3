"""Schema-level mode contract; runtime validation is tested separately."""

import json
from pathlib import Path

import pytest


jsonschema = pytest.importorskip("jsonschema")
SCHEMA = json.loads(
    (Path(__file__).resolve().parents[1] / "docs" / "schemas" / "leituraAP.pt.json")
    .read_text(encoding="utf-8")
)


def test_parametric_schema_is_valid():
    jsonschema.Draft7Validator.check_schema(SCHEMA)
    assert SCHEMA["properties"]["combinacao"]["default"] is True


@pytest.mark.parametrize("mode", range(4))
@pytest.mark.parametrize("tipo_ap", [None, 0, 1])
@pytest.mark.parametrize("combination", [None, False, True])
def test_parametric_schema_mode_contract(mode, tipo_ap, combination):
    analysis = {"vfp": mode}
    if tipo_ap is not None:
        analysis["tipoAP"] = tipo_ap
    if combination is not None:
        analysis["combinacao"] = combination
    valid = not (tipo_ap == 1 and combination is False)
    assert jsonschema.Draft7Validator(SCHEMA).is_valid(analysis) is valid


@pytest.mark.parametrize("value", [0, 1, "false", None, [], {}])
def test_parametric_schema_rejects_non_boolean_combinacao(value):
    assert not jsonschema.Draft7Validator(SCHEMA).is_valid({"combinacao": value})
