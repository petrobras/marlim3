import json
import os
import tempfile
import warnings

from .FloweditConversionInputInfo import FloweditConversionInputInfo
from .FloweditConversion import FloweditConversion


def converter_flowedit_para_json(
    excel_path,
    json_template_dict=None,
    fluid_model=None,
    fluid_model_path=None,
    keep_flowedit_xy=True,
    write_log=False,
    vazao_gas_lift=200000.0,
    temperatura_gas_lift_servico=50.0,
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
    vazao_gas_lift : float
        Gas-lift injection rate [sm3/d].
    temperatura_gas_lift_servico : float
        Injection temperature at the service-line inlet [degC].

    Gas lift follows the FLOWedit file: with service pipes, injection through
    the service line (``gasInj`` flow-rate BC, VGL parameters taken from the
    template). Without them, a ``fonteGas`` at the deepest VGL, at the lowest
    external temperature of the production pipes.
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

    _configurar_gas_lift(result, json_template_dict or {}, vazao_gas_lift, temperatura_gas_lift_servico)

    return result


_ROTULO_FONTE_VGL = "VGL"
_CHAVES_POSICAO_VGL = {"id", "ativo", "colunaEanular", "comprimentoMedidoProducao", "comprimentoMedidoServico"}


def _configurar_gas_lift(result, template, vazao, temperatura_servico):
    tem_servico = bool(result.get("dutosServico"))
    vgls = result.get("fonteGasLift") or []

    result.setdefault("configuracaoInicial", {})["linhaGas"] = tem_servico
    if "perfilServico" in result:
        result["perfilServico"]["ativo"] = tem_servico

    fontes_gas = result.setdefault("fonteGas", [])
    fonte_vgl = next((f for f in fontes_gas if f.get("rotulo") == _ROTULO_FONTE_VGL), None)

    if tem_servico:
        vgl_template = next(iter(template.get("fonteGasLift") or []), {})
        parametros = {k: v for k, v in vgl_template.items() if k not in _CHAVES_POSICAO_VGL}
        for vgl in vgls:
            vgl.update(parametros)
            vgl["ativo"] = True
        result["gasInj"] = {
            "ativo": True,
            "tipoCC": 1,
            "chuteVazaoInjecao": False,
            "tempo": [0],
            "temperatura": [temperatura_servico],
            "vazaoGas": [vazao],
        }
        if fonte_vgl is not None:
            fonte_vgl["ativo"] = False
        return

    result["fonteGasLift"] = []
    result["gasInj"] = {"ativo": False, "tempo": [0]}
    for duto in result.get("dutosProducao", []):
        duto["acoplamentoTermico"] = 0

    if not vgls:
        if fonte_vgl is not None:
            fonte_vgl["ativo"] = False
        warnings.warn("Planilha sem linha de serviço e sem VGL: nenhuma injeção de gás-lift criada.")
        return

    temperatura_fonte = min(
        t for duto in result.get("dutosProducao", [])
        for t in duto.get("condicoesIniciais", {}).get("tempExterna", [])
    )
    if fonte_vgl is None:
        fonte_vgl = {
            "id": max((f.get("id", -1) for f in fontes_gas), default=-1) + 1,
            "rotulo": _ROTULO_FONTE_VGL,
            "seco": True,
        }
        fontes_gas.append(fonte_vgl)
    fonte_vgl.update({
        "ativo": True,
        "comprimentoMedido": max(v["comprimentoMedidoProducao"] for v in vgls),
        "tempo": [0],
        "temperatura": [temperatura_fonte],
        "vazaoGas": [vazao],
        "vazaoFluidoComplementar": [0],
    })
