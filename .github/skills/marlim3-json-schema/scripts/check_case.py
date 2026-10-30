"""Deterministic checks for a Marlim3 input file (.mr3), reported all at once.

Usage, from the repository root:

    uv run python .github/skills/marlim3-json-schema/scripts/check_case.py <case.mr3>

Checks the JSON schema (every violation, plus keys the schema does not know), loading
through marlim3.Branch, ID cross-references, gasLine/transient dependencies, inputs that
crash the executable, paired and monotonic arrays, positions along the lines and
plausibility ranges. Prints one finding per line and exits with status 1 when there is
any ERROR. The simulator is never run.
"""
import copy
import json
import math
import os
import sys
import tempfile
from pathlib import Path

# The checks never run the engine: skip the executable download/build done on import.
os.environ.setdefault("MARLIM3_SKIP_EXECUTABLE_RESOLUTION", "1")

ROOT = Path(__file__).resolve().parents[4]
SCHEMAS = ROOT / "docs" / "schemas"

# Lists paired element by element with the "time" list of the same object.
TIME_PAIRED = {"pressure", "temperature", "opening", "frequency", "liquidFlowRate", "gasFlowRate",
               "totalMassFlowRate", "massFlowRate", "gasMassFlow", "complementaryMassFlowRate",
               "complementaryFluidFlowRate", "injectionPressures", "fluidQuality", "betaRatio", "beta",
               "pressureDrop", "ambientFluidQuality"}
# Lists paired with each other (but not with "time"); the engine tolerates uneven Cv calibration points.
PAIRED_GROUPS = (({"flowRate", "pumpHead", "power", "efficiency"}, "ERROR"), ({"x1", "cv1"}, "WARNING"))
PRODUCTION_POSITIONS = {
    "ipr": ("measuredLength",), "liquidSource": ("measuredLength",), "massSource": ("measuredLength",),
    "gasSource": ("measuredLength",), "pressureSource": ("measuredLength",),
    "porousRadialSource": ("measuredLength",), "porous2DSource": ("measuredLength",),
    "valve": ("measuredLength",), "esp": ("measuredLength",), "volumetricPump": ("measuredLength",),
    "pressureDrop": ("measuredLength",), "masterValve": ("measuredLength",), "pig": ("launcher", "receiver"),
    "gasLiftSource": ("prodMeasuredLength",), "productionTrend": ("measuredLength",),
    "crossProductionTrend": ("measuredLength",), "crossProductionProfile": ("measuredLength",),
}
SERVICE_POSITIONS = {
    "gasLiftSource": ("serviceMeasuredLength",), "serviceTrend": ("measuredLength",),
    "crossServiceTrend": ("measuredLength",), "crossServiceProfile": ("measuredLength",),
}
SERVICE_LINE_OBJECTS = ("servicePipe", "gasInj", "gasLiftSource", "serviceProfile", "serviceTrend")
ANGLE = math.pi / 2 + 1e-3
RANGES = [  # (array, field, low, high) — outside the range is a WARNING
    ("productionFluid", "api", 5, 60), ("productionFluid", "gor", 0, 2000), ("productionFluid", "bsw", 0, 1),
    ("productionFluid", "gasDensity", 0.55, 1.5), ("crossSection", "innerDiameter", 0.05, 0.6),
    ("crossSection", "roughness", 1e-6, 1e-3), ("productionPipe", "angle", -ANGLE, ANGLE),
    ("servicePipe", "angle", -ANGLE, ANGLE),
]


def _numbers(value):
    return isinstance(value, list) and bool(value) and all(
        isinstance(x, (int, float)) and not isinstance(x, bool) for x in value)


def _read_json(path):
    raw = Path(path).read_bytes()
    try:
        return json.loads(raw.decode("utf-8"))
    except UnicodeDecodeError:
        return json.loads(raw.decode("latin-1"))


def _where(parts):
    out = ""
    for part in parts:
        out += f"[{part}]" if isinstance(part, int) else (f".{part}" if out else str(part))
    return out or "(root)"


def _forbid_unknown_keys(node):
    if isinstance(node, dict):
        if "properties" in node:
            node.setdefault("additionalProperties", False)
        for value in node.values():
            _forbid_unknown_keys(value)
    elif isinstance(node, list):
        for value in node:
            _forbid_unknown_keys(value)


def schema_findings(raw):
    try:
        import jsonschema
    except ImportError:
        return [("WARNING", "(schema)",
                 "jsonschema is not installed, schema not checked — it ships with the project environment: "
                 "`uv sync --locked`")]
    english = str(raw.get("language", "")).lower() == "en"
    schema = _read_json(SCHEMAS / ("branch.en.json" if english else "branch.pt.json"))
    data = {k: v for k, v in raw.items() if k != "language"}
    found = [("ERROR", _where(e.absolute_path), e.message) for e in jsonschema.Draft7Validator(schema).iter_errors(data)]
    # The schema does not list every key in use (labels, ids, legacy sections), so a key only
    # counts as unknown when the API's translation table does not know it either.
    translations = _read_json(ROOT / "marlim3" / "translations.json")["keys"]
    known = set(translations) | set(translations.values())
    strict = copy.deepcopy(schema)
    _forbid_unknown_keys(strict)
    for e in jsonschema.Draft7Validator(strict).iter_errors(data):
        if e.validator == "additionalProperties":
            unknown = sorted(k for k in set(e.instance) - set(e.schema.get("properties", {}))
                             if k not in known and not k.endswith("_reserva"))
            if unknown:
                found.append(("WARNING", _where(e.absolute_path),
                              f"unknown key(s) {unknown} — not in the schema nor in marlim3/translations.json; "
                              "check for typos"))
    return found


def load_through_api(path):
    """Return the English-keyed model exactly as Branch.simulate() would hand it to the engine."""
    import marlim3
    case = marlim3.Branch()
    case.from_json(str(path))
    with tempfile.TemporaryDirectory() as tmp:
        cwd = os.getcwd()
        try:
            os.chdir(tmp)
            case.to_json("case")
        finally:
            os.chdir(cwd)
        return _read_json(Path(tmp) / "case.mr3")


def _ids(items):
    return {x.get("id") for x in items}


def _formation(d):
    return ((d.get("initialConfig") or {}).get("formation") or {}).get("properties") or []


def duplicate_id_findings(d):
    arrays = {k: v for k, v in d.items() if isinstance(v, list) and v and all(isinstance(x, dict) for x in v)}
    arrays["initialConfig.formation.properties"] = _formation(d)
    found = []
    for name, items in arrays.items():
        seen = [x.get("id") for x in items if "id" in x]
        found += [("ERROR", name, f"duplicate id {dup}") for dup in sorted({i for i in seen if seen.count(i) > 1}, key=str)]
    return found


def reference_findings(d):
    sections, materials = _ids(d.get("crossSection") or []), _ids(d.get("material") or [])
    fluids, formations = _ids(d.get("productionFluid") or []), _ids(_formation(d))
    refs = []  # (where, value, valid ids, target)
    for line in ("productionPipe", "servicePipe"):
        for i, pipe in enumerate(d.get(line) or []):
            refs.append((f"{line}[{i}].crossSectionId", pipe.get("crossSectionId"), sections, "crossSection"))
            refs.append((f"{line}[{i}].formationId", pipe.get("formationId"), formations,
                         "initialConfig.formation.properties"))
    for i, section in enumerate(d.get("crossSection") or []):
        for j, layer in enumerate(section.get("layers") or []):
            refs.append((f"crossSection[{i}].layers[{j}].materialId", layer.get("materialId"), materials, "material"))
    for source in ("ipr", "liquidSource", "massSource", "pressureSource", "gasSource"):
        for i, item in enumerate(d.get(source) or []):
            unused = (source == "gasSource" and item.get("dry") is not False) or \
                     (source == "pressureSource" and item.get("fluidType") == 1)
            if not unused:
                refs.append((f"{source}[{i}].prodFluidId", item.get("prodFluidId"), fluids, "productionFluid"))
    refs.append(("initialConfig.initialFluidId", (d.get("initialConfig") or {}).get("initialFluidId"), fluids,
                 "productionFluid"))
    return [("ERROR", where, f"{value!r} is not an id of {target} {sorted(valid, key=str)}")
            for where, value, valid, target in refs if value is not None and value not in valid]


def dependency_findings(d):
    ic, found = d.get("initialConfig") or {}, []
    if not ic.get("gasLine"):
        for key in SERVICE_LINE_OBJECTS:
            value = d.get(key)
            if isinstance(value, list):
                value = [x for x in value if x.get("active", True)]
            elif isinstance(value, dict) and not value.get("active", True):
                value = None
            if value:
                found.append(("WARNING", key, "is ignored while initialConfig.gasLine is false"))
    elif not d.get("servicePipe"):
        found.append(("ERROR", "initialConfig.gasLine", "is true but servicePipe is empty"))
    if ic.get("transient") and not (d.get("time") or {}).get("finalTime"):
        found.append(("ERROR", "time.finalTime", "is required when initialConfig.transient is true"))
    return found


def crash_findings(d):
    # Optional in the schema, but the executable reads them unconditionally and crashes
    # (segmentation fault, no simulacao.log) when they are missing.
    return [("ERROR", f"ipr[{i}]", "needs ii and iiTime (injectivity index; repeat ip/ipTime if unknown) — "
             "without them the executable crashes with no simulacao.log")
            for i, ipr in enumerate(d.get("ipr") or []) if not (ipr.get("ii") and ipr.get("iiTime"))]


def _time_schedule(node, path):
    time, found = node.get("time"), []
    if not _numbers(time):
        return found
    if any(b < a for a, b in zip(time, time[1:])):
        found.append(("ERROR", f"{path}.time", "must be non-decreasing"))
    for key in sorted(TIME_PAIRED & node.keys()):
        if _numbers(node[key]) and len(node[key]) != len(time):
            found.append(("ERROR", f"{path}.{key}", f"{len(node[key])} values for {len(time)} time entries"))
    return found


def _paired_lists(node, path):
    found = []
    for group, severity in PAIRED_GROUPS:
        lengths = {k: len(node[k]) for k in group if _numbers(node.get(k))}
        if len(set(lengths.values())) > 1:
            found.append((severity, path, f"{', '.join(sorted(lengths))} must have equal lengths ({lengths})"))
    for key, value in node.items():  # e.g. staticPressureTime/staticPressure, sonicTime/sonicFlag
        if key.endswith("Time") and _numbers(value):
            for partner in (key[:-4], key[:-4] + "Flag"):
                if _numbers(node.get(partner)) and len(node[partner]) != len(value):
                    found.append(("ERROR", f"{path}.{partner}",
                                  f"{len(node[partner])} values for {len(value)} {key} entries"))
    return found


def _step_schedule(node, path):
    times, found = node.get("times"), []
    if not _numbers(times):
        return found
    if times[0] != 0 or any(b <= a for a, b in zip(times, times[1:])):
        found.append(("ERROR", f"{path}.times", "must start at 0 and be strictly increasing"))
    if _numbers(node.get("maxDT")) and len(node["maxDT"]) != len(times):
        found.append(("ERROR", f"{path}.maxDT", f"{len(node['maxDT'])} values for {len(times)} times"))
    return found


def _profiles(node, path):
    positions, found = node.get("measuredPosition"), []
    if not _numbers(positions):
        return found
    if positions[0] != 0 or positions[-1] != 1 or any(b < a for a, b in zip(positions, positions[1:])):
        found.append(("ERROR", f"{path}.measuredPosition", "must go from 0 to 1"))
    for key, value in node.items():
        if key != "measuredPosition" and _numbers(value) and len(value) != len(positions):
            found.append(("ERROR", f"{path}.{key}", f"{len(value)} values for {len(positions)} measuredPosition entries"))
    return found


def _openings(node, path):
    opening = node.get("opening")
    if _numbers(opening) and node.get("cvCurve") != 1 and not all(0 <= x <= 1 for x in opening):
        return [("ERROR", f"{path}.opening", "values must be within [0, 1]")]
    return []


def array_findings(node, path=""):
    found = []
    if isinstance(node, dict):
        for check in (_time_schedule, _paired_lists, _step_schedule, _profiles, _openings):
            found += check(node, path)
        for key, value in node.items():
            found += array_findings(value, f"{path}.{key}" if path else key)
    elif isinstance(node, list):
        for i, value in enumerate(node):
            found += array_findings(value, f"{path}[{i}]")
    return found


def line_length(pipes, ic, line):
    """Total length [m] of a line, or None when the geometry cannot be summed."""
    if not pipes:
        return None
    if ic.get("xyMode"):
        x, y, total = ic.get(f"x{line}Start") or 0, ic.get(f"y{line}Start") or 0, 0.0
        for pipe in pipes:
            if "xCoor" not in pipe or "yCoor" not in pipe:
                return None
            total += math.hypot(pipe["xCoor"] - x, pipe["yCoor"] - y)
            x, y = pipe["xCoor"], pipe["yCoor"]
        return total
    total = 0.0
    for pipe in pipes:
        if pipe.get("grouping", True) is False and _numbers(pipe.get("cellDx")):
            total += sum(pipe["cellDx"])
        elif pipe.get("discretization"):
            total += sum(block.get("numCells", 0) * block.get("length", 0) for block in pipe["discretization"])
        else:
            return None
    return total


def _positions_on_line(d, fields, total, name):
    found = []
    for key, names in fields.items():
        value = d.get(key)
        items = value if isinstance(value, list) else [value] if isinstance(value, dict) else []
        for i, item in enumerate(items):
            where = f"{key}[{i}]" if isinstance(value, list) else key
            for field in names:
                raw = item.get(field)
                for x in raw if _numbers(raw) else [raw] if isinstance(raw, (int, float)) else []:
                    if not 0 <= x <= total * (1 + 1e-9):
                        found.append(("ERROR", f"{where}.{field}", f"{x} m is outside the {name} line (0–{total:g} m)"))
            if key == "pig" and isinstance(item.get("launcher"), (int, float)) \
                    and isinstance(item.get("receiver"), (int, float)) and item["launcher"] >= item["receiver"]:
                found.append(("ERROR", where, "launcher must be upstream of receiver"))
    return found


def position_findings(d):
    found, ic = [], d.get("initialConfig") or {}
    for line, name, pipes, fields in (("Prod", "production", d.get("productionPipe"), PRODUCTION_POSITIONS),
                                      ("Service", "service", d.get("servicePipe"), SERVICE_POSITIONS)):
        total = line_length(pipes, ic, line)
        if total is not None:
            found += _positions_on_line(d, fields, total, name)
    return found


def plausibility_findings(d):
    found = []
    for array, field, low, high in RANGES:
        for i, item in enumerate(d.get(array) or []):
            value = item.get(field)
            if isinstance(value, (int, float)) and not low <= value <= high:
                found.append(("WARNING", f"{array}[{i}].{field}", f"{value} is outside the usual range {low:g}–{high:g}"))
    for i, material in enumerate(d.get("material") or []):
        k = material.get("conductivity")
        if material.get("type", 0) in (0, 1) and isinstance(k, (int, float)) and k < 0.02:
            found.append(("WARNING", f"material[{i}].conductivity",
                          f"{k} W/(m·°C) is below 0.02 (better than any real insulation)"))
    return found


def main(argv):
    if len(argv) != 1:
        print(__doc__)
        return 2
    path = Path(argv[0]).resolve()
    found = schema_findings(_read_json(path))
    try:
        data = load_through_api(path)
    except Exception as exc:  # the API rejected the file: nothing else can be checked
        found.append(("ERROR", "(load)", f"marlim3.Branch().from_json failed: {exc}"))
    else:
        data = {k: v for k, v in data.items() if not k.endswith("_reserva")}  # parked sections are not read
        for check in (duplicate_id_findings, reference_findings, dependency_findings, crash_findings,
                      position_findings, plausibility_findings, array_findings):
            found += check(data)
    for severity, where, message in found:
        print(f"{severity:<7} {where}: {message}")
    errors = sum(severity == "ERROR" for severity, _, _ in found)
    print(f"check_case: {errors} error(s), {len(found) - errors} warning(s)")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
