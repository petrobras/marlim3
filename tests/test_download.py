import pytest

from marlim3 import _download


@pytest.mark.parametrize(
    ("system", "machine", "expected_asset", "expected_executable"),
    [
        ("Windows", "AMD64", "Marlim3-windows-x64.exe", "Marlim3.exe"),
        ("Linux", "x86_64", "Marlim3-linux-x64", "Marlim3"),
        ("Darwin", "arm64", "Marlim3-macos-arm64", "Marlim3"),
    ],
)
def test_get_platform_asset_info_matches_release_asset_names(
    monkeypatch,
    system,
    machine,
    expected_asset,
    expected_executable,
):
    monkeypatch.setattr(_download.platform, "system", lambda: system)
    monkeypatch.setattr(_download.platform, "machine", lambda: machine)

    asset_name, executable_name = _download.get_platform_asset_info()

    assert asset_name == expected_asset
    assert executable_name == expected_executable


def test_get_platform_asset_info_rejects_macos_intel(monkeypatch):
    monkeypatch.setattr(_download.platform, "system", lambda: "Darwin")
    monkeypatch.setattr(_download.platform, "machine", lambda: "x86_64")

    with pytest.raises(RuntimeError, match="macOS Intel is not supported"):
        _download.get_platform_asset_info()


def test_download_failure_points_to_repository_build_instructions(
    monkeypatch,
    tmp_path,
    capsys,
):
    def fail_download(url, destination):
        raise _download.urllib.error.HTTPError(url, 404, "Not Found", None, None)

    monkeypatch.setattr(_download.platform, "system", lambda: "Linux")
    monkeypatch.setattr(_download.platform, "machine", lambda: "x86_64")
    monkeypatch.setattr(_download, "__file__", str(tmp_path / "_download.py"))
    monkeypatch.setattr(_download.urllib.request, "urlretrieve", fail_download)

    assert not _download.download_executable()

    output = capsys.readouterr().out
    assert "follow the repository build instructions." in output
    assert "MARLIM3_COMPILE_FROM_SOURCE=1" not in output
