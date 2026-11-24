from pathlib import Path

import pytest

from marlim3._download import executable_exists
from marlim3._output_headers import parse_header_metadata, split_trend_header_lines
from marlim3.testing import load_manifest, run_case

ROOT = Path(__file__).resolve().parents[2]
DEMO_ROOT = ROOT / "demos"
SERVICE_CASE = next(case for case in load_manifest(DEMO_ROOT / "manifest.json")
                    if case.model.startswith("extended-shutdown-"))


@pytest.mark.simulacao
@pytest.mark.skipif(not executable_exists(), reason="Compiled engine is required")
@pytest.mark.parametrize("language", ["pt", "en"])
def test_engine_profiles_and_trends_include_input_metadata(tmp_path_factory, language):
    work = tmp_path_factory.mktemp("inputs # metadata")
    result = run_case(SERVICE_CASE, DEMO_ROOT, work, language)
    assert result.status == "sucesso"
    assert set(result.outputs) >= {"productionProfile", "serviceProfile", "productionTrend_10", "serviceTrend_10"}
    files = list(result.directory.rglob("*.dat"))
    profiles = [path for path in files if path.name.startswith(("PERFISP-", "PERFISG-"))]
    trends = [path for path in files if path.name.startswith(("TENDP-", "TENDG-"))]
    assert profiles and trends
    input_file = result.directory / f"{SERVICE_CASE.label}.mr3"
    for path in profiles + trends:
        assert_output_metadata(path, path in trends, input_file)


def assert_output_metadata(path, is_trend, input_file):
    lines = path.read_text(encoding="utf-8").splitlines()
    metadata = parse_header_metadata(lines[0])
    assert metadata["version"].startswith("Marlim - ")
    assert Path(metadata["input_file"]).is_absolute()
    assert Path(metadata["input_file"]).samefile(input_file)
    if is_trend:
        assert split_trend_header_lines(lines)[2] == 4
