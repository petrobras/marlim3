"""Run Marlim3 cases described in a manifest (public demos or private suites).

A manifest is a JSON list of entries::

    {"model": "injetor-tabela/injetor-tabela.json",
     "mode": "INJETOR",
     "aux": ["fluido-injecao.tab"],
     "languages": ["pt", "en"]}

``model`` is relative to the manifest folder; ``aux`` files are looked up in the
model folder first and then in the manifest folder. Models are written in the
requested language before running, so a Portuguese model also exercises the
PT -> EN translation when ``"en"`` is listed. ``native`` points to the same
model written by hand in English; it must lose no key when read by the API and
must give the same profiles as the Portuguese run.

The manifest is the only list of cases: besides the checks above, the profiles
of the first listed language are compared with the references stored in
``<model folder>/reference/`` (one case per folder). Update them with::

    python -m marlim3.testing demos/manifest.json [model ...]
"""

import argparse
import json
import os
import shutil
import sys
import tempfile
from dataclasses import dataclass, field, replace
from pathlib import Path

import numpy as np
import pandas as pd

from ._rede._rede import Network
from ._tramo._branch import Branch, _LEGACY_KEYS
from ._tramo._keys import translate_en_to_pt

PRODUCAO, INJETOR, REDE = "PRODUCAO", "INJETOR", "REDE"
MODES = (PRODUCAO, INJETOR, REDE)
LANGUAGES = ("pt", "en")
REFERENCE_FOLDER = "reference"
REFERENCE_KEYS = ("productionProfile", "serviceProfile", "productionTrend", "serviceTrend")
REFERENCE_RTOL, REFERENCE_ATOL = 1e-5, 1e-8


@dataclass(frozen=True)
class Case:
    model: str
    mode: str = PRODUCAO
    aux: tuple = ()
    languages: tuple = LANGUAGES
    steady_state: bool = True
    native: str = None

    @property
    def label(self):
        return os.path.splitext(self.model)[0].replace("/", "__").replace("\\", "__")


@dataclass
class CaseResult:
    case: Case
    language: str
    status: str
    directory: Path
    profiles: object = None
    log: dict = field(default_factory=dict)
    outputs: dict = field(default_factory=dict)


def load_manifest(path):
    """Read and validate a manifest, returning the list of :class:`Case`."""
    entries = json.loads(Path(path).read_text(encoding="utf-8"))
    cases = []
    for i, entry in enumerate(entries):
        unknown = set(entry) - {"model", "mode", "aux", "languages", "steady_state", "native"}
        if unknown or "model" not in entry:
            raise ValueError(f"{path} [{i}]: invalid keys {sorted(unknown) or ['model (missing)']}")
        case = Case(
            model=entry["model"],
            mode=entry.get("mode", PRODUCAO),
            aux=tuple(entry.get("aux", ())),
            languages=tuple(entry.get("languages", LANGUAGES)),
            steady_state=entry.get("steady_state", True),
            native=entry.get("native"),
        )
        if case.mode not in MODES:
            raise ValueError(f"{path} [{i}]: mode '{case.mode}' not in {MODES}")
        if not case.languages or set(case.languages) - set(LANGUAGES):
            raise ValueError(f"{path} [{i}]: languages must be a non-empty subset of {LANGUAGES}")
        if case.mode == REDE and "en" in case.languages:
            raise ValueError(f"{path} [{i}]: the engine network reader only accepts Portuguese keys")
        if any(Path(c.model).parent == Path(case.model).parent for c in cases):
            raise ValueError(f"{path} [{i}]: one case per folder (references are stored per folder)")
        cases.append(case)
    return cases


def missing_files(cases, base_dir):
    """Return the model/auxiliary files referenced by ``cases`` that do not exist."""
    base_dir = Path(base_dir)
    missing = []
    for case in cases:
        model = base_dir / case.model
        if not model.is_file():
            missing.append(case.model)
        if case.native and not (base_dir / case.native).is_file():
            missing.append(case.native)
        missing += [f"{case.model}: {name}" for name in case.aux
                    if not (model.parent / name).is_file() and not (base_dir / name).is_file()]
    return missing


def _read_json(path):
    raw = Path(path).read_bytes()
    try:
        return json.loads(raw.decode("utf-8"))
    except UnicodeDecodeError:
        return json.loads(raw.decode("latin-1"))


def _key_paths(obj, prefix=""):
    paths = {}
    if isinstance(obj, dict):
        for key, value in obj.items():
            # empty containers are not written back; "*_reserva" sections are parked and not read
            if value in ([], {}) or (not prefix and key.endswith("_reserva")):
                continue
            path = f"{prefix}/{key}"
            paths[path] = value if not isinstance(value, (dict, list)) else None
            paths.update(_key_paths(value, path))
    elif isinstance(obj, list):
        for i, value in enumerate(obj):
            paths.update(_key_paths(value, f"{prefix}[{i}]"))
    return paths


def _written_by_api(path, mode):
    """Load ``path`` through the API and return what it writes for the engine (PT)."""
    sim = Network() if mode == REDE else Branch()
    sim.from_json(str(path))
    with tempfile.TemporaryDirectory() as tmp:
        cwd = os.getcwd()
        try:
            os.chdir(tmp)
            sim.to_json("model", language="pt")
            written = next(Path(tmp).iterdir())
            return _read_json(written)
        finally:
            os.chdir(cwd)


def roundtrip_problems(case, base_dir):
    """Return input keys lost or changed when the API reads and rewrites the case.

    The model must come back identical (except discontinued version keys); the
    ``native`` translation must produce the same Portuguese keys as the model.
    """
    base_dir = Path(base_dir)
    original = _read_json(base_dir / case.model)
    if original.get("language") == "en":
        original = translate_en_to_pt({k: v for k, v in original.items() if k != "language"})
    original = {k: v for k, v in original.items() if k not in _LEGACY_KEYS and k not in ("version", "versao")}
    written = _written_by_api(base_dir / case.model, case.mode)
    expected, got = _key_paths(original), _key_paths(written)
    problems = [f"lost {p}" for p in expected if p not in got]
    problems += [f"added {p}" for p in got if p not in expected]
    problems += [f"changed {p}: {expected[p]!r} -> {got[p]!r}" for p in expected
                 if p in got and expected[p] != got[p]]
    if case.native:
        native = _key_paths(_written_by_api(base_dir / case.native, case.mode))
        problems += [f"native lacks {p}" for p in got if p not in native]
        problems += [f"native has extra {p}" for p in native if p not in got]
    return problems


def check_case(case, base_dir, output_dir):
    """Run ``case`` in every listed language and return a list of problems.

    Each run must end with status ``sucesso`` and, outside network mode, produce
    production profiles; PT and EN runs must give the same profiles.
    """
    results = {lang: run_case(case, base_dir, output_dir, lang) for lang in case.languages}
    problems = []
    for lang, result in results.items():
        if result.status != "sucesso":
            problems.append(f"[{lang}] status '{result.status}'")
        elif case.mode != REDE and result.profiles is None:
            problems.append(f"[{lang}] no production profiles")
    if not problems and {"pt", "en"} <= results.keys():
        problems += [f"PT x EN: {d}" for d in compare_profiles(results["pt"].profiles, results["en"].profiles)]
    if not problems and case.mode != REDE:
        problems += [f"reference: {d}" for d in reference_problems(results[case.languages[0]], base_dir)]
    if case.native and "pt" in results and not problems:
        native = run_case(replace(case, model=case.native), base_dir, output_dir, "en")
        if native.status != "sucesso":
            problems.append(f"[native] status '{native.status}'")
        else:
            problems += [f"PT x native: {d}" for d in compare_profiles(results["pt"].profiles, native.profiles)]
    return problems


def run_case(case, base_dir, output_dir, language="pt"):
    """Run ``case`` (relative to ``base_dir``) writing the model in ``language``."""
    base_dir = Path(base_dir)
    model_path = base_dir / case.model
    work = Path(output_dir) / case.label / language
    results = work / "resultados"
    work.mkdir(parents=True, exist_ok=True)

    for name in case.aux:
        for source in (model_path.parent / name, base_dir / name):
            if source.is_file():
                shutil.copy2(source, work)
                break
        else:
            raise FileNotFoundError(f"Auxiliary file '{name}' not found for {case.model}")

    if case.mode == REDE:
        sim = Network()
        sim.from_json(str(model_path))
    else:
        sim = Branch()
        sim.from_json(str(model_path))
        if case.mode == INJETOR:
            # Branch.simulate only passes "-s INJETOR" to the engine for this value.
            sim.system = "INJETOR"
        elif case.steady_state and "transient" in sim.initialConfig:
            sim.initialConfig["transient"] = False

    original_cwd = os.getcwd()
    try:
        os.chdir(work)
        if case.mode == REDE:
            sim.simulate(label=case.label, directory=str(results))
        else:
            sim.simulate(label=case.label, directory=str(results), language=language)
    finally:
        os.chdir(original_cwd)

    log = json.loads((results / "simulacao.log").read_text(encoding="utf-8", errors="replace"))
    profiles = None if case.mode == REDE else sim.resultados.get("productionProfile")
    outputs = {}
    for key in () if case.mode == REDE else REFERENCE_KEYS:
        value = sim.resultados.get(key)
        if isinstance(value, dict):  # trends: one frame per position
            outputs.update({f"{key}_{position}": frame for position, frame in value.items()})
        elif value is not None:
            outputs[key] = value
    return CaseResult(case, language, log["resultadoSimulacao"]["status"], work, profiles, log, outputs)


def reference_dir(case, base_dir):
    return Path(base_dir) / Path(case.model).parent / REFERENCE_FOLDER


def save_reference(result, base_dir):
    """Replace the stored reference of ``result.case`` with the profiles of ``result``."""
    folder = reference_dir(result.case, base_dir)
    if folder.exists():
        shutil.rmtree(folder)
    folder.mkdir(parents=True)
    for key, frame in result.outputs.items():
        frame.reset_index().to_csv(folder / f"{key}.csv", index=False)


def reference_problems(result, base_dir):
    """Return the differences between ``result`` and its stored reference."""
    folder = reference_dir(result.case, base_dir)
    if not folder.is_dir():
        return [f"missing {folder}; run: python -m marlim3.testing <manifest> {result.case.model}"]
    stored = {path.stem for path in folder.glob("*.csv")}
    problems = [f"{key} in reference but not produced" for key in sorted(stored - result.outputs.keys())]
    problems += [f"{key} produced but not in reference" for key in sorted(result.outputs.keys() - stored)]
    for key in sorted(stored & result.outputs.keys()):
        reference = pd.read_csv(folder / f"{key}.csv")
        problems += [f"{key}: {d}" for d in compare_profiles(
            reference, result.outputs[key].reset_index(), rtol=REFERENCE_RTOL, atol=REFERENCE_ATOL)]
    return problems


def update_references(manifest, models=(), output_dir=None):
    """Run the cases of ``manifest`` (all or only ``models``) and store their references.

    Returns the models that could not be updated.
    """
    base_dir = Path(manifest).parent
    cases = [c for c in load_manifest(manifest) if c.mode != REDE and (not models or c.model in models)]
    unknown = set(models) - {c.model for c in cases}
    if unknown:
        raise ValueError(f"not in {manifest} (or network cases, which have no reference): {sorted(unknown)}")
    failed = []
    with tempfile.TemporaryDirectory() as tmp:
        for case in cases:
            try:
                result = run_case(case, base_dir, output_dir or tmp, case.languages[0])
            except Exception as error:  # Branch.simulate raises on engine failure; keep updating the others
                print(f"FAILED {case.model}: {error}")
                failed.append(case.model)
                continue
            if result.status != "sucesso" or not result.outputs:
                print(f"FAILED {case.model}: status '{result.status}'")
                failed.append(case.model)
                continue
            save_reference(result, base_dir)
            print(f"updated {reference_dir(case, base_dir)} ({', '.join(result.outputs)})")
    return failed


def compare_profiles(reference, other, rtol=1e-6, atol=1e-9):
    """Return a list of differences between two production-profile DataFrames.

    Columns are matched by position: the engine writes profile headers in the
    language of the input model, so PT and EN runs have different names.
    """
    if reference is None or other is None:
        return [] if reference is other else ["profiles missing in one of the runs"]
    if reference.shape != other.shape:
        return [f"shape differs: {reference.shape} != {other.shape}"]
    differences = []
    for i, column in enumerate(reference.columns):
        a = reference.iloc[:, i]
        b = other.iloc[:, i]
        if a.dtype.kind not in "iuf" or b.dtype.kind not in "iuf":
            continue
        a, b = a.to_numpy(dtype=float), b.to_numpy(dtype=float)
        if not np.allclose(a, b, rtol=rtol, atol=atol, equal_nan=True):
            differences.append(f"{column} / {other.columns[i]}: max |diff| = {np.nanmax(np.abs(a - b)):.3g}")
    return differences


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python -m marlim3.testing",
                                     description="Run manifest cases and store their reference profiles.")
    parser.add_argument("manifest", help="manifest.json listing the cases")
    parser.add_argument("models", nargs="*", help="update only these entries (\"model\" field); default: all")
    args = parser.parse_args(argv)
    return 1 if update_references(args.manifest, args.models) else 0


if __name__ == "__main__":
    sys.exit(main())
