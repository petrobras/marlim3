"""The developer skill and the folder instructions must not cite files or symbols that no longer exist."""

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SKILL = ROOT / ".github" / "skills" / "development"
DOCUMENTS = sorted(SKILL.rglob("*.md")) + sorted((ROOT / ".github" / "instructions").glob("*.instructions.md"))
TOP_LEVEL = {entry.name for entry in ROOT.iterdir()}
# Folders moved under tools/: a citation without the prefix is stale, so it is checked too.
TOOLS = {entry.name for entry in (ROOT / "tools").iterdir()}
# Rows of the three-column tables "| File | Symbols | Role |" in references/code-map.md.
TABLE_ROW = re.compile(r"^\|\s*`([^`]+)`\s*\|([^|]*)\|[^|]*\|\s*$", re.M)


def read(path):
    return path.read_text(encoding="utf-8-sig", errors="replace").replace("\r\n", "\n")


def inline_code(path):
    text = re.sub(r"```.*?```", "", read(path), flags=re.S)
    return re.findall(r"`([^`\n]+)`", text)


def frontmatter(path):
    match = re.match(r"---\n(.*?)\n---\n", read(path), re.S)
    assert match, f"{path.name} has no frontmatter"
    return {key: value.strip().strip("'\"") for key, value in re.findall(r"^([\w-]+):\s*(.*)$", match.group(1), re.M)}


def test_skill_frontmatter():
    fields = frontmatter(SKILL / "SKILL.md")
    assert fields["name"] == SKILL.name
    assert 0 < len(fields["description"]) <= 1024
    for path in (ROOT / ".github" / "instructions").glob("*.instructions.md"):
        fields = frontmatter(path)
        assert fields.get("description") and fields.get("applyTo"), path.name


def test_cited_paths_exist():
    missing = []
    for document in DOCUMENTS:
        for token in inline_code(document):
            if any(character in token for character in " <>*{}\\"):
                continue
            first = token.split("/")[0]
            if first not in TOP_LEVEL | {"references", "scripts"} and not ("/" in token and first in TOOLS):
                continue
            candidates = [ROOT / token]
            if token.startswith(("references/", "scripts/")):
                candidates.append(SKILL / token)
            if not any(candidate.exists() for candidate in candidates):
                missing.append(f"{document.relative_to(ROOT).as_posix()}: {token}")
    assert not missing, "paths cited in the skill no longer exist:\n" + "\n".join(missing)


def test_code_map_symbols_exist():
    missing = []
    for path, cell in TABLE_ROW.findall(read(SKILL / "references" / "code-map.md")):
        symbols = re.findall(r"`([^`]+)`", cell)
        if not symbols:
            continue
        source = read(ROOT / path)
        for symbol in symbols:
            if "::" in symbol:
                patterns = [re.escape(symbol) + r"\s*\("]
            elif "." in symbol:
                owner, member = symbol.rsplit(".", 1)
                patterns = [rf"\bclass\s+{re.escape(owner)}\b", rf"\bdef\s+{re.escape(member)}\b"]
            else:
                patterns = [rf"\b{re.escape(symbol)}\s*\("]
            if not all(re.search(pattern, source) for pattern in patterns):
                missing.append(f"{path}: {symbol}")
    assert not missing, "symbols cited in references/code-map.md were not found:\n" + "\n".join(missing)
