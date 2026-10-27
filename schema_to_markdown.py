import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parent


def collect_units(schema: dict, prefix: str = "") -> dict[str, str]:
    units = {}
    for name, details in schema.get("properties", {}).items():
        path = f"{prefix}.{name}" if prefix else name
        if "unit" in details:
            units[path] = str(details["unit"])
        units.update(collect_units(details, path))
        items = details.get("items")
        if isinstance(items, dict):
            units.update(collect_units(items, f"{path}[]"))
    return units


def generate_markdown(schema: dict) -> str:
    from jsonschema_markdown import generate

    units = collect_units(schema)
    markdown = generate(schema, footer=False, hide_empty_columns=True)
    lines = []
    documented_units = set()
    in_table = False
    separator = False
    for line in markdown.splitlines():
        if line.startswith("| Property |"):
            in_table = True
            separator = True
            lines.append(line + " Unit |")
        elif in_table and line.startswith("|"):
            if separator:
                lines.append(line + " ---- |")
                separator = False
                continue
            path = line[1:].partition("|")[0].strip()
            unit = units.get(path, "")
            if path in units:
                documented_units.add(path)
            unit = unit.replace("|", "&#124;").replace("\n", "<br />")
            lines.append(line + f" {unit} |")
        else:
            in_table = False
            lines.append(line)
    missing = units.keys() - documented_units
    if missing:
        raise ValueError(f"Units missing from generated table: {sorted(missing)}")
    return "\n".join(lines) + "\n"


def generate_all_schemas(schema_dir: Path, output_dir: Path) -> list[Path]:
    schema_paths = sorted(schema_dir.glob("*.json"))
    if not schema_paths:
        raise ValueError(f"No JSON schemas found in {schema_dir}")

    generated_paths = []
    for schema_path in schema_paths:
        schema = json.loads(schema_path.read_text(encoding="utf-8"))
        output_path = output_dir / f"{schema_path.stem}.md"
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(generate_markdown(schema), encoding="utf-8")
        generated_paths.append(output_path)
    return generated_paths


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert JSON Schema to Markdown with units.")
    parser.add_argument("schema", type=Path, nargs="?")
    parser.add_argument("output", type=Path, nargs="?")
    parser.add_argument("--batch", action="store_true", help="Convert every JSON schema in a directory.")
    parser.add_argument("--schema-dir", type=Path, default=ROOT / "docs" / "schemas")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "docs" / "schemas" / "generated")
    args = parser.parse_args()

    if args.batch:
        if args.schema is not None or args.output is not None:
            parser.error("positional schema/output cannot be used with --batch")
        generate_all_schemas(args.schema_dir, args.output_dir)
        return

    if args.schema is None or args.output is None:
        parser.error("schema and output are required unless --batch is used")

    schema = json.loads(args.schema.read_text(encoding="utf-8"))
    args.output.write_text(generate_markdown(schema), encoding="utf-8")


if __name__ == "__main__":
    main()