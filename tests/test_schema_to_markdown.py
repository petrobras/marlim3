import json

import pytest

import schema_to_markdown


def test_generate_all_schemas_writes_one_markdown_per_json(tmp_path, monkeypatch):
    schema_dir = tmp_path / "schemas"
    output_dir = tmp_path / "generated"
    schema_dir.mkdir()
    schema_names = ["alpha", "beta", "gamma"]

    for name in schema_names:
        (schema_dir / f"{name}.json").write_text(
            json.dumps({"title": name}), encoding="utf-8"
        )

    monkeypatch.setattr(
        schema_to_markdown,
        "generate_markdown",
        lambda schema: f"# {schema['title']}\n",
    )

    generated_paths = schema_to_markdown.generate_all_schemas(schema_dir, output_dir)

    assert [path.name for path in generated_paths] == [
        f"{name}.md" for name in schema_names
    ]
    assert [path.read_text(encoding="utf-8") for path in generated_paths] == [
        f"# {name}\n" for name in schema_names
    ]


def test_generate_all_schemas_rejects_empty_directory(tmp_path):
    with pytest.raises(ValueError, match="No JSON schemas found"):
        schema_to_markdown.generate_all_schemas(tmp_path, tmp_path / "generated")