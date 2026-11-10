"""Unit conversion utilities for MR2 → MR3 conversion."""


def polegadas_to_metros(value):
    return value * 0.0254


def array_polegadas_to_metros(values):
    return [polegadas_to_metros(v) for v in values]


def kgf_por_cm2_manometrica_to_absoluta(value):
    return value + 1.03323


def array_kgf_por_cm2_manometrica_to_absoluta(values):
    return [kgf_por_cm2_manometrica_to_absoluta(v) for v in values]


def graus_to_radianos(value):
    return value * 0.017453292519


def array_graus_to_radianos(values):
    return [graus_to_radianos(v) for v in values]


def milimetros_to_metros(value):
    return value / 1000


def array_milimetros_to_metros(values):
    return [milimetros_to_metros(v) for v in values]


def centimetros_to_metros(value):
    return value / 100


def array_centimetros_to_metros(values):
    return [centimetros_to_metros(v) for v in values]


def get_float(dicionario, chave, default=0.0):
    """Safely extract a float from a dict, returning default if missing or invalid."""
    try:
        val = dicionario.get(chave)
        if val is None:
            return default
        return float(val)
    except (ValueError, TypeError):
        return default
