"""Run Marlim3 cases described in a manifest (public demos or private suites).

A manifest is a JSON list of entries::

    {"model": "pt-br/injetor-tabela/injetor-tabela.json",
     "mode": "INJETOR",
     "aux": ["fluido-injecao.tab"],
     "languages": ["pt", "en"]}

``model`` is relative to the manifest folder; ``aux`` files are looked up in the
model folder first and then in the manifest folder. Models are written in the
requested language before running, so a Portuguese model also exercises the
PT -> EN translation when ``"en"`` is listed.
"""

import json
import os
import shutil
from dataclasses import dataclass, field
from pathlib import Path

import numpy as np

from ._rede._rede import Network
from ._tramo._branch import Branch

PRODUCAO, INJETOR, REDE = "PRODUCAO", "INJETOR", "REDE"
MODES = (PRODUCAO, INJETOR, REDE)
LANGUAGES = ("pt", "en")


@dataclass(frozen=True)
class Case:
    model: str
    mode: str = PRODUCAO
    aux: tuple = ()
    languages: tuple = LANGUAGES
    steady_state: bool = True

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


def load_manifest(path):
    """Read and validate a manifest, returning the list of :class:`Case`."""
    entries = json.loads(Path(path).read_text(encoding="utf-8"))
    cases = []
    for i, entry in enumerate(entries):
        unknown = set(entry) - {"model", "mode", "aux", "languages", "steady_state"}
        if unknown or "model" not in entry:
            raise ValueError(f"{path} [{i}]: invalid keys {sorted(unknown) or ['model (missing)']}")
        case = Case(
            model=entry["model"],
            mode=entry.get("mode", PRODUCAO),
            aux=tuple(entry.get("aux", ())),
            languages=tuple(entry.get("languages", LANGUAGES)),
            steady_state=entry.get("steady_state", True),
        )
        if case.mode not in MODES:
            raise ValueError(f"{path} [{i}]: mode '{case.mode}' not in {MODES}")
        if not case.languages or set(case.languages) - set(LANGUAGES):
            raise ValueError(f"{path} [{i}]: languages must be a non-empty subset of {LANGUAGES}")
        if case.mode == REDE and "en" in case.languages:
            raise ValueError(f"{path} [{i}]: the engine network reader only accepts Portuguese keys")
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
        missing += [f"{case.model}: {name}" for name in case.aux
                    if not (model.parent / name).is_file() and not (base_dir / name).is_file()]
    return missing


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
    return CaseResult(case, language, log["resultadoSimulacao"]["status"], work, profiles, log)


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
