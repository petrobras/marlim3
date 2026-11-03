import importlib.util
from pathlib import Path

import pytest


SCRIPT_PATH = Path(__file__).parents[1] / ".github" / "scripts" / "release_version.py"


def load_script():
    spec = importlib.util.spec_from_file_location("release_version", SCRIPT_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


@pytest.fixture
def pr(monkeypatch, tmp_path):
    """Script with main at 3.10.1, tags up to v3.10.1, and a PR branch _version.py at 3.10.1."""
    script = load_script()
    version_file = tmp_path / "_version.py"
    version_file.write_text('"""Version."""\n\n__version__ = \'3.10.1\'\n', encoding="utf-8")
    monkeypatch.setattr(script, "VERSION_FILE", version_file)
    monkeypatch.setattr(script, "release_tags", lambda: [(3, 10, 0), (3, 10, 1)])
    monkeypatch.setattr(script, "git", lambda *args: "__version__ = '3.10.1'\n")
    return script, version_file


@pytest.mark.parametrize("level,expected", [("patch", (3, 10, 2)), ("minor", (3, 11, 0)), ("major", (4, 0, 0))])
def test_bump_levels(level, expected):
    assert load_script().bump((3, 10, 1), level) == expected


def test_next_release_skips_existing_tag():
    script = load_script()
    assert script.next_release("patch", (3, 10, 0), tags=[(3, 10, 1), (3, 9, 1)]) == (3, 10, 2)


def test_level_from_labels():
    script = load_script()
    assert script.level_from_labels(["bug", "Release-Minor"]) == "minor"
    assert script.level_from_labels(["documentation"]) is None
    with pytest.raises(ValueError):
        script.level_from_labels(["Release-Patch", "Release-Major"])


def test_write_version_keeps_other_lines(pr):
    script, version_file = pr
    script.write_version((3, 11, 0), version_file)
    assert version_file.read_text(encoding="utf-8") == '"""Version."""\n\n__version__ = \'3.11.0\'\n'


def test_sync_relabel_replaces_previous_bump(pr):
    script, version_file = pr
    script.main(["sync", '["Release-Patch"]'])
    assert script.read_version(version_file) == (3, 10, 2)
    script.main(["sync", '["Release-Minor"]'])
    assert script.read_version(version_file) == (3, 11, 0)


def test_sync_restore_when_label_removed(pr):
    script, version_file = pr
    script.main(["sync", '["Release-Patch"]'])
    script.main(["sync", "[]", "--restore"])
    assert script.read_version(version_file) == (3, 10, 1)


def test_sync_without_label_keeps_manual_version(pr):
    script, version_file = pr
    script.write_version((3, 10, 2), version_file)
    script.main(["sync", '["bug"]'])
    assert script.read_version(version_file) == (3, 10, 2)


@pytest.mark.parametrize("labels,version,ok", [
    ("[]", (3, 10, 1), True),                  # no release
    ("[]", (3, 10, 2), True),                  # manual single-step bump (fork PR)
    ("[]", (3, 12, 0), False),                 # skips a minor
    ("[]", (3, 10, 0), False),                 # goes backwards
    ('["Release-Patch"]', (3, 10, 2), True),
    ('["Release-Patch"]', (3, 10, 1), False),  # label without bump
    ('["Release-Minor"]', (3, 10, 2), False),  # bump does not match label
])
def test_check_pr(pr, labels, version, ok):
    script, version_file = pr
    script.write_version(version, version_file)
    if ok:
        assert script.main(["check-pr", labels]) == 0
    else:
        with pytest.raises(SystemExit, match="expected"):
            script.main(["check-pr", labels])


def test_check_pr_rejects_reused_tag(pr, monkeypatch):
    script, version_file = pr
    monkeypatch.setattr(script, "release_tags", lambda: [(3, 10, 1), (3, 10, 2)])
    script.write_version((3, 10, 2), version_file)
    with pytest.raises(SystemExit, match="expected 3.10.1 or 3.10.3"):
        script.main(["check-pr", "[]"])


def test_check_tag(pr):
    script, _ = pr
    with pytest.raises(SystemExit, match="does not match"):
        script.main(["check-tag", "v3.10.2"])
    assert script.main(["check-tag", "v3.10.1"]) == 0
