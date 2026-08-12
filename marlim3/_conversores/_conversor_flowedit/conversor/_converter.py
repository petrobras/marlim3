import json
import os
import tempfile

from .FloweditConversionInputInfo import FloweditConversionInputInfo
from .FloweditConversion import FloweditConversion


def converter_flowedit_para_json(
    excel_path,
    json_template_dict=None,
    fluid_model=None,
    fluid_model_path=None,
    keep_flowedit_xy=True,
    write_log=False,
):
    """Convert a FLOWedit Excel file to a Marlim3 JSON dict.

    Parameters
    ----------
    excel_path : str
        Absolute path to the FLOWedit .xlsm/.xlsx file.
    json_template_dict : dict or None
        Initial JSON template (Portuguese keys). When *None*, an empty dict is
        used — the converter writes all geometry keys from scratch.
    fluid_model : {'blackoil', 'tab', 'compositional'} or None
        When *None*, no fluid model keys are written to the JSON.
    fluid_model_path : str or None
        Path to the PVT/CTM file required for 'tab' or 'compositional' models.
    keep_flowedit_xy : bool
        Keep the FLOWedit origin coordinates instead of normalising to (0, 0).
    write_log : bool
        Write a conversion log file next to the Excel file.
    """
    b_black_oil = fluid_model == 'blackoil'
    b_tab = fluid_model == 'tab'
    b_compositional = fluid_model == 'compositional'
    b_not_write_fluid = fluid_model is None

    # converter writes to these keys expecting lists; seed them if not in template
    _MINIMAL_TEMPLATE = {
        "configuracaoInicial": {},
        "material": [],
        "secaoTransversal": [],
        "dutosProducao": [],
        "dutosServico": [],
    }
    if json_template_dict is not None:
        template = {**_MINIMAL_TEMPLATE, **json_template_dict}
    else:
        template = _MINIMAL_TEMPLATE

    tmp_fd, tmp_json_path = tempfile.mkstemp(suffix='.json')
    try:
        with os.fdopen(tmp_fd, 'w', encoding='utf-8') as fh:
            json.dump(template, fh, indent=2, ensure_ascii=False)

        input_info = FloweditConversionInputInfo(
            sFloweditFileToConvert=excel_path,
            sJsonTemplate=tmp_json_path,
            bNotWriteFluidModelToJson=b_not_write_fluid,
            bFluidModelBlackOil=b_black_oil,
            bFluidModelTab=b_tab,
            bFluidModelCompositional=b_compositional,
            sFluidModelFilePath=fluid_model_path or '',
            bWriteLog=write_log,
            bKeepFloweditInitialXY=keep_flowedit_xy,
        )

        FloweditConversion(input_info)

        with open(tmp_json_path, 'r', encoding='utf-8') as fh:
            result = json.load(fh)

    finally:
        try:
            os.remove(tmp_json_path)
        except OSError:
            pass

    return result
