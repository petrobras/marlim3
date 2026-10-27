import importlib.util
from pathlib import Path


SCRIPT_PATH = Path(__file__).parents[1] / ".github" / "scripts" / "wait_for_pypi_release.py"


def load_script():
    spec = importlib.util.spec_from_file_location("wait_for_pypi_release", SCRIPT_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_simple_has_version_matches_normalized_release_filename():
    script = load_script()
    simple_data = {
        "files": [
            {"filename": "other-1.0.0.tar.gz"},
            {"filename": "marlim3-3.8.0-py3-none-any.whl"},
        ],
    }

    assert script.simple_has_version(simple_data, "Marlim3", "3.8.0")


def test_release_is_installable_requires_pypi_visibility_and_pip_resolution(monkeypatch):
    script = load_script()
    calls = []

    monkeypatch.setattr(script, "pypi_has_release", lambda project, version: True)
    monkeypatch.setattr(script, "pip_can_resolve", lambda project, version: True)
    monkeypatch.setattr(script, "purge_pip_cache", lambda: calls.append("purged"))

    assert script.release_is_installable("marlim3", "3.8.0")
    assert calls == ["purged"]


def test_wait_for_release_retries_until_installable(monkeypatch):
    script = load_script()
    attempts = [False, False, True]
    sleeps = []

    monkeypatch.setattr(script, "release_is_installable", lambda project, version: attempts.pop(0))
    monkeypatch.setattr(script.time, "sleep", lambda seconds: sleeps.append(seconds))

    assert script.wait_for_release("marlim3", "3.8.0", attempts=3, delay_seconds=15)
    assert sleeps == [15, 15]


def test_wait_for_release_fails_after_limited_attempts(monkeypatch):
    script = load_script()
    sleeps = []

    monkeypatch.setattr(script, "release_is_installable", lambda project, version: False)
    monkeypatch.setattr(script.time, "sleep", lambda seconds: sleeps.append(seconds))

    assert not script.wait_for_release("marlim3", "3.8.0", attempts=2, delay_seconds=15)
    assert sleeps == [15]
