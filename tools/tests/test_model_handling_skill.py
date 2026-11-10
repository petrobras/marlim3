"""The model-handling skill, the simulation agents and the Copilot instructions must not cite files that no longer exist."""

import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GITHUB = ROOT / ".github"
SKILL = GITHUB / "skills" / "model-handling"
DOCUMENTS = sorted(SKILL.rglob("*.md")) + sorted((GITHUB / "agents").glob("*.agent.md")) + [
    GITHUB / "README-agents.md", GITHUB / "copilot-instructions.md"]
LINK = re.compile(r"\]\(([^)\s#]+)(?:#[^)]*)?\)")


def read(path):
    return path.read_text(encoding="utf-8-sig", errors="replace").replace("\r\n", "\n")


def frontmatter(path):
    match = re.match(r"---\n(.*?)\n---\n", read(path), re.S)
    assert match, f"{path.name} has no frontmatter"
    return {key: value.strip().strip("'\"") for key, value in re.findall(r"^([\w-]+):\s*(.*)$", match.group(1), re.M)}


def test_skills_frontmatter():
    for skill in sorted(path.parent for path in (GITHUB / "skills").glob("*/SKILL.md")):
        fields = frontmatter(skill / "SKILL.md")
        assert fields["name"] == skill.name, skill.name
        assert 0 < len(fields["description"]) <= 1024, skill.name
    # Skills are found one level below .github/skills/ only.
    assert not list((GITHUB / "skills").glob("*/*/SKILL.md"))


def test_agents_frontmatter():
    for path in (GITHUB / "agents").glob("*.agent.md"):
        fields = frontmatter(path)
        assert fields.get("name") and fields.get("description"), path.name


def test_every_reference_is_indexed():
    index = read(SKILL / "SKILL.md")
    missing = [path.name for path in (SKILL / "references").glob("*.md") if f"references/{path.name}" not in index]
    assert not missing, "references missing from model-handling/SKILL.md: " + ", ".join(missing)


def test_links_resolve():
    missing = []
    for document in DOCUMENTS:
        for target in LINK.findall(read(document)):
            if re.match(r"[a-z]+:", target):
                continue
            if not (document.parent / target).exists():
                missing.append(f"{document.relative_to(ROOT).as_posix()}: {target}")
    assert not missing, "links that do not resolve:\n" + "\n".join(missing)


def test_cited_repository_paths_exist():
    # Commands and inline paths such as `.github/skills/model-handling/scripts/check_case.py` or `tools/tests/`.
    pattern = re.compile(r"(?<![\w/.<-])((?:\.github|tools|docs|demos|src)/[\w./-]*[\w/])")
    missing = []
    for document in DOCUMENTS:
        for path in pattern.findall(read(document)):
            if not (ROOT / path).exists():
                missing.append(f"{document.relative_to(ROOT).as_posix()}: {path}")
    assert not missing, "paths cited in the model-handling skill or agents no longer exist:\n" + "\n".join(missing)


def test_check_case_accepts_a_demo():
    result = subprocess.run(
        [sys.executable, str(SKILL / "scripts" / "check_case.py"),
         str(ROOT / "demos" / "simplifiedProduction" / "simplifiedProduction.mr3")],
        capture_output=True, text=True, cwd=ROOT,
    )
    assert result.returncode == 0, result.stdout[-2000:] + result.stderr[-2000:]
