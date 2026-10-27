# Input Migration Guide

The engine resolves every input field by exact key name. A key that does not
match the schema is not applied, so a renamed field would silently fall back to
its default value and change simulation results without any warning.

To make that impossible, the discontinued spellings listed below are now
rejected with an explicit validation error. Update the affected keys in your
`.mr3` files and the model runs unchanged.

## Discontinued input keys

The engine now reads exactly the names published in the
[JSON schema](json-schema.md), which are also the names produced by the
`marlim3` Python API. If you build models through the Python API, no change is
required — files it generates already use the current names.

| Object | Discontinued key | Use instead |
|---|---|---|
| `fluidoComplementar` | `tipoFluido` | `tipoF` |
| `configuracaoInicial` | `modoDifus3DArq` | `modoDifus3DJson` |
| `configuracaoInicial.condicaoVazPres` | `Vazao Massica` | `VazMass` |
| `dutosProducao[]` | `difusTerm3DRotulo` | `difusTerm3DAcop` |
| `parafina` | `multiplicadorViscosidade` | `multVis` |
| `fontePressao[]` | `ambienteGas` | `ambGas` |
| `tendP[]` | `tempChokeJusante` | `tempChokeJus` |
| `perfilProducao` | `tempoResiLiqComp` | `TResi` |
| root | `poroRadial` | `fontePoroRadial` |
| root | `poro2D` | `fontePoro2D` |

English-language files are translated to the internal Portuguese names before
validation, so the same rules apply to their English equivalents — for example
`complementaryFluidType` for `tipoF` and `residenceTime` for `TResi`.

## Error message

A discontinued key aborts the run during input validation with a message that
names the replacement:

```text
#/fluidoComplementar/tipoFluido: chave descontinuada, utilize 'tipoF'
```

## Why the keys were not simply accepted as aliases

Keeping both spellings would let two different names address the same field,
which makes input files ambiguous and hides the divergence between the engine
and the published schema. Rejecting the old spelling surfaces the problem at
validation time, when it can still be corrected, instead of producing a result
that looks valid but was computed with a default value.

## Impact if a key is left unchanged

Before this change the field was ignored. Depending on the key, that could
disable a whole model feature — for example, `fluidoComplementar.tipoFluido = 2`
selects the friction-reducer fluid, so losing it removes the friction reduction
from the pressure solution and shifts the reported profiles.
