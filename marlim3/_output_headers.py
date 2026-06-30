import re


CANONICAL_TIME_COLUMN = "Tempo (s)"
TIME_COLUMN_ALIASES = ("Tempo (s)", "Time (s)")
PRESSURE_COLUMN_ALIASES = ("Pressao (kgf/cm2)", "Pressure (kgf/cm2)")
TEMPERATURE_COLUMN_ALIASES = ("Temperatura (C)", "Temperature (C)")

SKIP_VARIABLES = {
    "Comprimento (m) Fronteira F",
    "Length (m) Boundary F",
    "Comprimento (m) Centro Volume C",
    "Length (m) Cell center C",
    "Comprimento (m) centro de Volume C",
    "Elevacao (m) F",
    "Elevation (m) F",
    "Elevacao (m) C",
    "Elevation (m) C",
    "comprimento_fundoPoco (m) F",
    "bottomhole_length (m) F",
    "Profundidade (m) F",
    "Depth (m) F",
    "Profundidade (m) C",
    "Depth (m) C",
}


def resolve_time_column(columns):
    for alias in TIME_COLUMN_ALIASES:
        if alias in columns:
            return alias
    raise KeyError(f"Time column not found. Expected one of: {TIME_COLUMN_ALIASES}")


def normalize_time_column(df):
    time_column = resolve_time_column(df.columns)
    if time_column != CANONICAL_TIME_COLUMN:
        return df.rename(columns={time_column: CANONICAL_TIME_COLUMN})
    return df


def is_pressure_variable(name):
    stripped = name.strip()
    return any(stripped.startswith(alias) for alias in PRESSURE_COLUMN_ALIASES)


def is_temperature_variable(name):
    stripped = name.strip()
    return any(stripped.startswith(alias) for alias in TEMPERATURE_COLUMN_ALIASES)


def parse_trend_headers(line1, line2, line3):
    measured_length_match = re.search(r"=\s*([-+]?\d+(?:\.\d+)?)", line1)
    cell_index_match = re.search(r"=\s*([-+]?\d+)", line3)
    if measured_length_match is None or cell_index_match is None:
        raise ValueError("Could not parse trend header lines")

    measured_length = float(measured_length_match.group(1))
    label = line2.split("=", 1)[1].strip()
    cell_index = int(cell_index_match.group(1))
    return measured_length, label, cell_index