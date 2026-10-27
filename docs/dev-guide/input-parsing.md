# Input Parsing and SProd Construction

This document describes how the Marlim3 simulator reads a JSON input file, validates it, maps its contents to internal data structures, and constructs the `SProd` simulation object with a fully initialized mesh of cells, accessories, and boundary conditions.

**Key source files:**

| File | Role |
|------|------|
| [`src/JSONDataModel.h`](https://github.com/petrobras/marlim3/blob/main/src/include/JSONDataModel.h) / [`JSONDataModel.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/JSONDataModel.cpp) | Generic JSON data model built on RapidJSON |
| [`src/JSON_entrada.h`](https://github.com/petrobras/marlim3/blob/main/src/include/JSON_entrada.h) / [`JSON_entrada.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/JSON_entrada.cpp) | Typed JSON schema — macro-based class hierarchy mirroring JSON structure |
| [`src/Leitura.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Leitura.h) / [`Leitura.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Leitura.cpp) | `Ler` class — reads JSON, populates C structs, builds geometry and cells (~21,400 lines) |
| [`src/LeituraVapor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LeituraVapor.h) / [`LeituraVapor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LeituraVapor.cpp) | `LerVap` class — variant for steam injection simulations |
| [`src/LerAP.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerAP.h) / [`LerAP.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LerAP.cpp) | `APara` class — parametric analysis input reader |
| [`src/SisProd.h`](https://github.com/petrobras/marlim3/blob/main/src/include/SisProd.h) / [`SisProd.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProd.cpp) | `SProd` class — simulation engine; constructor invokes `Ler` and `montasistema()` |
| [`src/estruturas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturas.h) | Core data structs (`corteduto`, `detduto`, `detcelp`, etc.) |
| [`src/celula3.h`](https://github.com/petrobras/marlim3/blob/main/src/include/celula3.h) / [`celula3.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/celula3.cpp) | `Cel` class — per-cell simulation state |
| [`src/PropFlu.h`](https://github.com/petrobras/marlim3/blob/main/src/include/PropFlu.h) / [`PropFlu.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/PropFlu.cpp) | `ProFlu` — fluid property models |
| [`src/Geometria.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Geometria.h) | `DadosGeo` — pipe geometry (diameter, roughness, layers) |
| [`src/TrocaCalor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/TrocaCalor.h) / [`TrocaCalor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/TrocaCalor.cpp) | `TransCal` — radial heat-transfer model per cell |

---

## Table of Contents

1. [Overview](#overview)
2. [Architecture — The Three-Layer Pipeline](#architecture--the-three-layer-pipeline)
3. [Layer 1 — JSONDataModel: Generic JSON Parser](#layer-1--jsondatamodel-generic-json-parser)
4. [Layer 2 — JSON_entrada: Typed Schema Classes](#layer-2--json_entrada-typed-schema-classes)
5. [Layer 3 — Ler: The Data Reader](#layer-3--ler-the-data-reader)
6. [The Ler Constructor and lerArq()](#the-ler-constructor-and-lerarq)
7. [parse_configuracao_inicial — Simulation Configuration](#parse_configuracao_inicial--simulation-configuration)
8. [Geometry Parsing — Cross-Sections, Ducts, and Cells](#geometry-parsing--cross-sections-ducts-and-cells)
9. [Fluid Property Parsing](#fluid-property-parsing)
10. [Accessory Parsing — Sources, Pumps, Valves](#accessory-parsing--sources-pumps-valves)
11. [Boundary Conditions, Master Valve, PIG, and Output Parsing](#boundary-conditions-master-valve-pig-and-output-parsing)
12. [SProd Construction](#sprod-construction)
13. [montasistema — Building the Simulation Mesh](#montasistema--building-the-simulation-mesh)
14. [Time-Dependent BC Updates (atualiza)](#time-dependent-bc-updates-atualiza)
15. [Output Methods on Ler](#output-methods-on-ler)
16. [Parametric Analysis (APara)](#parametric-analysis-apara)
17. [Steam Injection Variant (LerVap)](#steam-injection-variant-lervap)
18. [JSON Input Structure Reference](#json-input-structure-reference)
19. [Summary of Key Methods](#summary-of-key-methods)

---

## Overview

The input parsing system transforms a single JSON file into a fully constructed `SProd` object ready for simulation. The process follows a **three-layer architecture**:

```
JSON file on disk
  │  RapidJSON FileReadStream + Document.ParseStream()
  ▼
JSONRootObject::load(path)         ← Layer 1: generic JSON data model
  │  recursive key matching into typed containers
  ▼
JSON_entrada subclasses            ← Layer 2: typed schema (macro-generated)
  │  contents["key"] → JSONNumber / JSONBoolean / JSONArray<...>
  ▼
Ler constructor → lerArq()         ← Layer 3: ~40 parse_* methods
  │  populates C structs: materials, ducts, cells, fluids, accessories
  ▼
SProd constructor
  │  arq = Ler(...)               ← triggers full parse in initializer list
  │  montasistema()               ← builds Cel[], CelG[], attaches accessories
  ▼
Ready for simulation (SolveTrans / permanenteSimples)
```

The `Ler` object (`arq`) is stored as a member of `SProd` and remains accessible throughout the simulation for time-dependent BC updates, output formatting, and parameter queries.

---

<a id="architecture--the-three-layer-pipeline"></a>
## Architecture — The Three-Layer Pipeline

| Layer | Class(es) | Responsibility |
|-------|-----------|----------------|
| **1. Generic parser** | `JSONRootObject`, `JSONObject`, `JSONArray<T>`, `JSONString`, `JSONNumber`, `JSONInteger`, `JSONBoolean` | Load JSON file via RapidJSON, provide type-safe recursive access to any key |
| **2. Typed schema** | `JSON_entrada`, `JSON_entrada_configuracaoInicial`, `JSON_entrada_fluidosProducao`, ... (~100 classes) | Mirror the JSON document structure as a C++ class hierarchy; each JSON key becomes a named accessor |
| **3. Data reader** | `Ler` (and variants `LerVap`, `ASens`) | Extract typed values from the schema layer, validate, convert units, populate simulation structs |

This separation means:
- Layer 1 is **JSON-generic** — it knows nothing about Marlim3
- Layer 2 is **schema-specific** — it knows the JSON key names but not their simulation meaning
- Layer 3 is **simulation-specific** — it applies physics defaults, unit conversions, and cross-validation

---

<a id="layer-1--jsondatamodel-generic-json-parser"></a>
## Layer 1 — JSONDataModel: Generic JSON Parser

Defined in [`JSONDataModel.h`](https://github.com/petrobras/marlim3/blob/main/src/include/JSONDataModel.h) / [`JSONDataModel.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/JSONDataModel.cpp), built on top of the **RapidJSON** library.

### Class hierarchy

```
JSONInstance (abstract)
 ├── JSONString          → wraps std::string, calls v.GetString()
 ├── JSONInteger         → wraps int, calls v.GetInt()
 ├── JSONNumber          → wraps double, calls v.GetDouble()
 ├── JSONBoolean         → wraps bool, calls v.GetBool()
 ├── JSONObject          → maps string → shared_ptr<JSONInstance> via contents map
 │    └── JSONRootObject → owns a RapidJSON Document, reads from file
 └── JSONArray<T>        → wraps vector<shared_ptr<T>>, iterates v.GetArray()
```

### Key pattern — recursive loading

`JSONObject::load()` iterates its `contents` map and matches against the RapidJSON `Value` members:

```cpp
void JSONObject::load(Value& v) {
    for (auto iter = contents.begin(); iter != contents.end(); iter++) {
        string key = iter->first;
        shared_ptr<JSONInstance>& o = iter->second;
        if (v.HasMember(key.c_str())) {
            o->load(v[key.c_str()]);
        }
    }
}
```

`JSONRootObject::load()` opens the file via `fopen` + `FileReadStream`, calls `Document::ParseStream()`, then delegates to `JSONObject::load()`. If parsing fails, the error offset is available via `GetErrorOffset()`.

Each `JSONInstance` subclass tracks whether its value was present in the JSON via the `bIsNull` flag (set `false` after a successful `load()`), accessible through the `exists()` method.

!!! warning "Key names are matched exactly"
    Because lookup is a plain `HasMember` test, a key whose name does not match
    the one registered in `contents` is not loaded at all: `exists()` stays
    `false` and the field silently keeps its default. Renaming a key in
    `JSON_entrada.cpp` is therefore a breaking change for existing input files,
    even though nothing fails at parse time.

    To keep that failure mode visible, `validaTipoJson.cpp` rejects the known
    discontinued spellings with an explicit validation error before the typed
    schema is populated. The list lives in `valida_chaves_descontinuadas()` and
    is mirrored in the
    [input migration guide](../single-branch-model-reference/migration.md).
    Any future rename must be added to both.

---

<a id="layer-2--json_entrada-typed-schema-classes"></a>
## Layer 2 — JSON_entrada: Typed Schema Classes

Defined in [`JSON_entrada.h`](https://github.com/petrobras/marlim3/blob/main/src/include/JSON_entrada.h) / [`JSON_entrada.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/JSON_entrada.cpp).

### Macro-based type aliasing

Each JSON field is declared via a `#define` mapping a hierarchical name to a `JSONDataModel` type:

```cpp
#define JSON_entrada_configuracaoInicial_transiente      JSONBoolean
#define JSON_entrada_configuracaoInicial_pvtsimArq        JSONString
#define JSON_entrada_configuracaoInicial_SalinidadeFluido JSONNumber
#define JSON_entrada_configuracaoInicial_modeloCp         JSONInteger
```

### Nested objects as classes

Each JSON object becomes a class inheriting `JSONObject`. The constructor registers child fields in the `contents` map:

```cpp
JSON_entrada_configuracaoInicial_Avancado::JSON_entrada_configuracaoInicial_Avancado() {
    contents["CriterioMonofasico"] = make_shared<...CriterioMonofasico>();
    contents["nthrd"]              = make_shared<...nthrd>();
    contents["dtmax"]              = make_shared<...dtmax>();
    // ...
}
```

Typed accessors return references for downstream use:

```cpp
JSON_entrada_configuracaoInicial_Avancado_nthrd&
JSON_entrada_configuracaoInicial_Avancado::nthrd() {
    return static_cast<...nthrd&>(*contents["nthrd"].get());
}
```

### Top-level JSON sections

The root `JSON_entrada` class registers these top-level keys:

| JSON key | C++ accessor | Content |
|----------|-------------|---------|
| `"sistema"` | `sistema()` | System type (multifásico, injetor, oleoduto) |
| `"versao"` | `versao()` | Input file format version |
| `"configuracaoInicial"` | `configuracaoInicial()` | Simulation settings |
| `"tempo"` | `tempo()` | Time discretization and schedule |
| `"tabela"` | `tabela()` | PVT table grid (P/T range, number of points) |
| `"fluidosProducao"` | `fluidosProducao()` | Production fluid array (API, GOR, BSW, ...) |
| `"fluidoGas"` | `fluidoGas()` | Gas fluid properties |
| `"fluidoComplementar"` | `fluidoComplementar()` | Complementary (water) fluid |
| `"material"` | `material()` | Material array (conductivity, Cp, ρ) |
| `"secaoTransversal"` | `secaoTransversal()` | Pipe cross-section definitions |
| `"dutosProducao"` | `dutosProducao()` | Production line segments |
| `"dutosServico"` | `dutosServico()` | Gas-lift service line segments |
| `"valvula"` | `valvula()` | Valve array (opening schedule, Cv curves) |
| `"fonteGas"` / `"fonteLiquido"` / `"fonteMassa"` | `fonteGas()` / ... | Source arrays |
| `"fonteGasLift"` | `fonteGasLift()` | Gas-lift valve array |
| `"bcs"` | `bcs()` | BCS (ESP) pump definitions |
| `"pig"` | `pig()` | PIG launcher/receiver |
| `"parafina"` | `parafina()` | Wax/paraffin deposition model |
| `"perfilProducao"` / `"perfilServico"` | ... | Output profile configuration |
| `"trendProducao"` / `"trendServico"` | ... | Output trend configuration |

---

<a id="layer-3--ler-the-data-reader"></a>
## Layer 3 — Ler: The Data Reader

Defined in [`Leitura.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Leitura.h) / [`Leitura.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/Leitura.cpp) (~21,400 lines).

The `Ler` class is the **central data reader** that bridges the typed JSON schema and the simulation engine. It reads, validates, unit-converts, and stores simulation data in C-style structs.

### Key enums

```cpp
typedef enum { none, json, schema, rules, off } tipoValidacaoJson_t;
typedef enum { transiente, poco_injetor, rede, convecNatural } tipoSimulacao_t;
typedef enum { jusante, montante } origemGeometria_t;
typedef enum { multifasico, injetor, oleoduto } sistemaSimulacao_t;
```

### Major data structs (defined in [`Leitura.h`](https://github.com/petrobras/marlim3/blob/main/src/include/Leitura.h) and [`estruturas.h`](https://github.com/petrobras/marlim3/blob/main/src/include/estruturas.h))

| Struct | Purpose | Key Fields |
|--------|---------|------------|
| `material` | Material thermal properties | `cond`, `cp`, `rho`, `visc`, `beta`, `tipo` |
| `corteduto` | Pipe cross-section definition | `id`, `ncam` (layers), `a`/`b` (inner/outer diameter), `rug` (roughness), `anul` (annular flag), `diam[]`, `indmat[]` |
| `detduto` | Duct segment angle + cross-section | `ang` (inclination), `indcorte` (→ `corteduto`), `servico` flag |
| `detalhaP` | Production unit (multiple cells) | `ncel`, `comp` (length), `dx[]`, `var[][]` (initial condition profiles), geometry flags |
| `detcelp` | Per-cell raw data (pre-construction) | `dx`, `temp`, `pres`, `hol`, `bet`, `uls`, `ugs`, `textern`, ambient properties, gradient multipliers |
| `detcelg` | Gas service-line cell (analogous) | Same structure as `detcelp` |
| `detIPR` | Inflow Performance Relationship | `pres[]`, `temp[]`, `ip[]`, `jp[]`, `qMax[]`, `tipoIPR`, `indcel`, `indfluP` |
| `detGASINJ` | Gas-lift injection BC | `presInj`, `tempInj`, `vazgas` |
| `detVALVGL` | Gas-lift valve geometry | `posicP`, `posicG`, `diam`, `cd`, `tipo` (orifice / IPO), `frec` |
| `detBCS` | ESP (BCS) pump curve | `ncurva`, `vaz[]`, `head[]`, `power[]`, `efic[]`, `freqref`, `nestag`, `freqMinima` |
| `detMultiBCS` | Multi-stage BCS configuration | Same plus staging info |
| `detFONGAS` / `detFONLIQ` / `detFONMASS` | Source time series | `tempo[]`, `valor[]`, `indcel`, `indfluP` |
| `detValv` | Valve opening schedule | `tempo[]`, `abertura[]`, `Cv[]`, `curvaDinamic` |
| `detMASTER1` | Master valve (wet christmas tree) | `abertura[]`, `razareaativ`, `tempo[]`, `curvaCV`, `ncv`, `cvCurv[]` |
| `detPSEP` | Separator pressure time series | `tempo[]`, `pres[]` |
| `detCHOKESUP` | Surface choke | `cd`, `modelo`, `abertura[]`, `tempo[]`, `curvaDinamic`, `cvCurv[]` |
| `detPig` | PIG data | `lanca` (launch cell), `receb` (receive cell), `compL` (launch position), `compR` (receive position), `tempo` (launch time) |
| `detTempo` | Time discretization | `dtmax[]`, `tempoDT[]`, convergence tolerances |
| `detPROFP` | Profile output config | Variable flags (`pres`, `temp`, `hol`, `ugs`, `uls`, ...), time interval |
| `detTRENDP` | Trend output config | Same flags plus cell position |
| `ProFlu` | Fluid property object | API, RGO, BSW, `dg` (gas density), emulsion model, PVT table pointers |

---

## The Ler Constructor and lerArq()

### Constructor

```cpp
Ler::Ler(const string IMPFILE, const string ARQUIVO_LOG,
         const tipoValidacaoJson_t VALIDACAO, const tipoSimulacao_t SIMULACAO,
         int vreverso, varGlob1D *Vvg1dSP, int vredeperm)
{
    iniciarVariaveis();    // zero-init ~100+ member variables
    impfile = IMPFILE;
    arquivoLog = ARQUIVO_LOG;
    validacaoJson = VALIDACAO;
    tipoSimulacao = SIMULACAO;
    reverso = vreverso;
    vg1dSP = Vvg1dSP;
    redeperm = vredeperm;
    lerArq();              // ← all parsing happens here
}
```

`iniciarVariaveis()` zeroes out all counters (`nfluP`, `nduto`, `ncelp`, `ncorte`, `nipr`, `nbcs`, ...), nulls all pointers (`flup`, `corte`, `duto`, `celp`, `IPRS`, `bcs`, ...), and sets flag defaults.

### lerArq() — The Master Dispatcher

`Ler::lerArq()` (~200 lines) orchestrates the entire parse. It:

1. Calls `parseEntrada()` to load and validate the JSON file
2. Reads the `"sistema"` key to determine the simulation type (`multifasico`, `injetor`, `oleoduto`)
3. Dispatches ~40 `parse_*` methods in dependency order:

```
lerArq()
  │
  ├─ parseEntrada()                        → JSON_entrada jsonDoc
  │    ├─ RapidJSON FileReadStream + ParseStream
  │    ├─ (optional) JSONKeyTranslator::translateEnToPt() if language == "en"
  │    └─ jsonDoc.loadFromDocument(rawDoc) → typed JSON_entrada schema
  │
  ├─ parse_configuracao_inicial(...)       → simulation flags, defaults
  ├─ parse_condcont_pocinjec(...)          → injection well BCs (if pocinjec == 1)
  │
  ├─ (business-rule cross-validations)     → warnings/errors for missing required keys
  │
  ├─ parse_parafina(...)                   → wax model (if modoParafina == 1)
  ├─ parse_materiais(...)                  → mat[] (material properties)
  ├─ parse_corte(...)                      → corte[] (cross-sections)
  ├─ parse_tempo(...)                      → dtmax, tfinal, schedule
  │    (only when pocinjec==0 AND transiente==1; otherwise defaults are set inline)
  ├─ parse_tabela(...)                     → tabent (PVT grid: P/T range, npoints)
  ├─ parse_fluidos_producao(...)           → flup[] (ProFlu objects)
  ├─ parse_fluido_complementar(...)        → complementary fluid
  ├─ parse_fluido_gas(...)                 → gas fluid
  ├─ parse_correcao(...)                   → gradient multiplier corrections (if present)
  │
  ├─ Allocate duto[] (nduto = nunidadep + nunidadeg)
  ├─ parse_unidades_producao(...)          → unidadeP[], duto[] (geometry)
  ├─ Allocate celp[] (ncelp cells)
  ├─ Populate celp[] from unidadeP[]      → interpolate IC profiles per cell
  ├─ parse_unidades_servico(...)           → service line ducts (if lingas > 0)
  ├─ Populate celg[] from unidadeG[]
  │
  ├─ parse_hidrato(...)                    → hydrate model (if present)
  ├─ parse_ipr(...)                        → IPRS[] (inflow models)
  ├─ parse_fonte_gaslift(...)              → valvgl[] (gas-lift valve geometry)
  ├─ parse_fontechk(...)                   → choke-type source (fonteChoke key)
  ├─ parse_fonte_gas(...)                  → gas source time series
  ├─ parse_fonte_liquido(...)              → liquid source time series
  ├─ parse_fonte_massa(...)                → mass source time series
  ├─ parse_fonte_PoroRadial(...)           → radial porous inflow sources
  ├─ parse_fonte_Poro2D(...)              → 2D porous inflow sources
  │
  ├─ (pocinjec == 0 block):
  │    ├─ parse_valv(...)                  → valve opening schedules
  │    ├─ parse_bcs(...)                   → bcs[] (ESP pump curves)
  │    ├─ parse_multibcs(...)              → multi-stage BCS
  │    ├─ parse_bomba_volumetrica(...)     → volumetric pumps
  │    ├─ parse_delta_pressao(...)         → generic ΔP accessories
  │    ├─ parse_fonteCalor(...)            → heat source accessories
  │    ├─ parse_furo(...)                  → leak/puncture sources (fontePressao key)
  │    ├─ parse_master1(...)               → master valve (wet christmas tree)
  │    ├─ parse_intermitencia(...)         → intermittent flow settings
  │    ├─ parse_pig(...)                   → PIG data
  │    ├─ parse_tendencia_producao(...)    → production trend output config
  │    ├─ parse_tendencia_trans_producao() → production radial trend config
  │    └─ (if lingas > 0):
  │         ├─ parse_gasInj(...)           → gas injection BCs at platform
  │         ├─ parse_master2(...)          → secondary master valve
  │         ├─ parse_tendencia_servico(...) → service trend output config
  │         └─ parse_tendencia_trans_servico()
  │
  ├─ parse_separador(...)                  → separator pressure schedule
  ├─ parse_chokeSup(...)                   → surface choke settings
  ├─ parse_chokeInj(...)                   → injection choke (if lingas > 0)
  ├─ parse_perfil_producao(...)            → output profile configuration
  └─ parse_perfis_trans_producao(...)      → radial profile configuration
```

### parseEntrada() — JSON File Loading

```cpp
JSON_entrada Ler::parseEntrada() {
    JSON_entrada jsonDoc;
    // First pass: raw RapidJSON parse to check syntax and detect "language" key
    FILE* fp = fopen(impfile.c_str(), "r");
    char buf[65536];
    FileReadStream stream(fp, buf, sizeof(buf));
    Document rawDoc;
    rawDoc.ParseStream(stream);
    fclose(fp);
    if (rawDoc.HasParseError()) {
        // Log error with position, line number, and localized message
        logger.log_write_logs_and_exit(LOGGER_FALHA, ...);
    }
    // Optional English-input translation:
    // if rawDoc["language"] == "en", all English keys are translated to
    // their Portuguese equivalents via JSONKeyTranslator::translateEnToPt()
    // before the typed schema is populated.
    jsonDoc.loadFromDocument(rawDoc);   // populate typed JSON_entrada schema
    return jsonDoc;
}
```

If `validacaoJson == tipoValidacaoJson_t::json`, the process stops after confirming the JSON is syntactically valid (useful for CI checks).

---

<a id="parse_configuracao_inicial--simulation-configuration"></a>
## parse_configuracao_inicial — Simulation Configuration

`parse_configuracao_inicial()` reads the `"configuracaoInicial"` JSON object and sets ~80 simulation flags and parameters. It follows a **defaults-then-override** pattern:

### Default values (set before reading JSON)

| Parameter | Default | Meaning |
|-----------|---------|---------|
| `perm` | `1` | Steady-state calculation enabled |
| `lingas` | `0` | No gas-lift service line |
| `equilterm` | `1` | Thermal equilibrium between phases |
| `transiente` | `0` | Transient mode disabled |
| `flashCompleto` | `0` | Black-oil fluid model |
| `mono` | `1e-4` | Monophasic criterion |
| `critcond` | `0.001` | Condensate criterion |
| `escorregaPerm` | `0` | No slip in steady-state |
| `escorregaTran` | `0` | No slip in transient |
| `CriterioConvergPerm` | `0.001` | Steady-state convergence tolerance |
| `origemGeometria` | `montante` | Geometry origin at upstream end |

### JSON overrides

Each field is read conditionally — only if present in the JSON (`exists()` check):

```cpp
if (configuracao_inicial_json.transiente().exists())
    transiente = configuracao_inicial_json.transiente();
if (configuracao_inicial_json.flashCompleto().exists())
    flashCompleto = configuracao_inicial_json.flashCompleto();
if (configuracao_inicial_json.escorregaPerm().exists())
    escorregaPerm = configuracao_inicial_json.escorregaPerm();
```

The `"Avancado"` sub-object contains expert-level settings:

| Parameter | JSON path | Meaning |
|-----------|-----------|---------|
| `nthrd` | `configuracaoInicial.Avancado.nthrd` | Number of OpenMP threads |
| `CriterioMonofasico` | `configuracaoInicial.Avancado.CriterioMonofasico` | Threshold for single-phase treatment |
| `taxaDespre` | `configuracaoInicial.Avancado.taxaDespre` | Depressurization rate threshold for adaptive model |
| `modoDifus3D` | `configuracaoInicial.Avancado.modoDifus3D` | 3D thermal diffusion coupling mode |

> **Note:** `dtmax` is **not** set here; it comes from `parse_tempo()`, which reads the `"tempo"` JSON section (called only in transient production mode — see below).

---

<a id="geometry-parsing--cross-sections-ducts-and-cells"></a>
## Geometry Parsing — Cross-Sections, Ducts, and Cells

The geometry is parsed in three stages: cross-sections → duct segments → individual cells.

### Stage 1 — `parse_corte()`: Cross-section definitions

Reads `"secaoTransversal"` → `corteduto corte[]`:

Each cross-section defines the pipe layers from inside out:
- Inner diameter `a`, outer diameter `b`
- Roughness `rug`
- Annular flag `anul` (for concentric pipes)
- Number of layers `ncam`
- Per-layer: diameter boundary `diam[j]`, material index `indmat[j]`

### Stage 2 — `parse_unidades_producao()`: Duct segments

Reads `"dutosProducao"` → `detalhaP unidadeP[]` + `detduto duto[]`:

Each production unit defines a section of the pipeline:
- Angle `ang` (inclination) — supports both explicit angle and XY-coordinate modes
- Cross-section reference `idCorte`
- Number of cells `ncel` in this segment
- Total length `comp`
- Cell widths `dx[]` (uniform or variable); when `agrupaDiscre == 1`, discretization is specified as groups of cells rather than individual lengths
- Initial condition profiles `var[][]` — temperature, pressure, holdup, water cut interpolated along the segment length

Similarly, `parse_unidades_servico()` reads the gas-lift service line segments.

### Stage 3 — Cell population in `lerArq()`

After parsing units, `lerArq()` allocates `celp[]` and fills each cell by interpolating the unit profiles:

```
for each production unit i:
    for each cell j in unit i:
        celp[j].duto    = unidadeP[i].duto       // cross-section index
        celp[j].dx      = unidadeP[i].dx[j]      // cell length
        celp[j].temp    = interpolated temperature
        celp[j].pres    = interpolated pressure
        celp[j].hol     = interpolated holdup
        celp[j].bet     = interpolated water cut
        celp[j].uls/ugs = interpolated superficial velocities
        celp[j].textern = external (ambient) temperature
        // ... ~15 fields per cell
```

---

## Fluid Property Parsing

### `parse_fluidos_producao()` — Production fluids

Reads `"fluidosProducao"` → `ProFlu flup[]`:

For each active fluid in the JSON array:
- **API gravity** → oil density
- **RGO** (gas-oil ratio) → flash calculations
- **BSW** (basic sediment & water) → water cut
- **Gas specific gravity** `dg`
- **Water density** `denag`
- **Emulsion model** (`tipoEmul`) — Woelflin, user table, or none
- **Viscosity tables** — dead-oil viscosity vs temperature, or user-supplied curves
- **PVT table file** — optional PVTSIM `.tab` file path for compositional models

### `parse_tabela()` — PVT table grid

Reads `"tabela"` → `detTabelaEntalp tabent`:

```
tabent.npont = number of interpolation points
tabent.pmax  = maximum pressure (kgf/cm²)
tabent.pmin  = minimum pressure (kgf/cm²)
tabent.tmax  = maximum temperature (°C)
tabent.tmin  = minimum temperature (°C)
```

These define the grid on which black-oil correlations (Rs, Bo, Ba, μ, ρ) are pre-tabulated for fast lookup during simulation.

### `parse_fluido_gas()` / `parse_fluido_complementar()`

Read the gas fluid (`flug`) and complementary (water) fluid (`flucol`) properties. For compositional models (`flashCompleto == 2`), these include component mole fractions and EOS parameters.

---

<a id="accessory-parsing--sources-pumps-valves"></a>
## Accessory Parsing — Sources, Pumps, Valves

Accessories are devices placed at specific cell positions. Each type is identified by an integer `acsr.tipo`:

| `tipo` | Device | Parse method | Struct |
|--------|--------|-------------|--------|
| 0 | No accessory | — | — |
| 1 | Gas injection source | `parse_fonte_gas()` | `detFONGAS` |
| 2 | Liquid injection source | `parse_fonte_liquido()` | `detFONLIQ` |
| 3 | IPR (reservoir inflow) | `parse_ipr()` | `detIPR` |
| 4 | BCS (ESP pump) | `parse_bcs()` | `detBCS` |
| 5 | Choke / valve | `parse_valv()` | `detValv` |
| 6 | Diameter change | (via `MudaArea` in `Acidentes2`) | — |
| 7 | Generic ΔP | `parse_delta_pressao()` | — |
| 8 | Volumetric pump | `parse_bomba_volumetrica()` | — |
| 9 | Fonte choke (tubing leak) | `parse_fonte_choke()` | `detFONCHK` |
| 10 | Multi-phase mass source | `parse_fonte_massa()` | `detFONMASS` |
| 11 | Steam IPR | `parse_ipr_vapor()` | `detIPRVAP` |
| 12 | Steam mass source | `parse_fonte_vapor()` | `detFONVAP` |
| 14 | Volumetric pump for steam | `parse_bomba_vol_vapor()` | — |
| 15 | Radial porous inflow | (special IPR variant) | — |
| 16 | 2D porous inflow | (special IPR variant) | — |
| 17 | Multi-stage BCS | `parse_multibcs()` | `detMultiBCS` |

> **Note:** Type 13 is unused. Steam types (11, 12, 14) are implemented in [`SisProdVap.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/SisProdVap.cpp).

Each `parse_*` method follows the same pattern:
1. Count active items in the JSON array
2. Allocate struct array
3. For each active item, map JSON fields to struct members
4. Store cell position index for later attachment by `montasistema()`

### Gas-lift valves

`parse_fonte_gaslift()` reads `"fonteGasLift"` → `detVALVGL valvgl[]`:
- Production cell position `posicP`
- Service cell position `posicG`
- Orifice diameter and discharge coefficient `cd`
- Valve type: `0` = orifice, `1` = IPO (Injection Pressure Operated)
- Recovery fraction `frec`

### BCS pumps

`parse_bcs()` reads `"bcs"` → `detBCS bcs[]`:
- Pump curve: flow rate vs head arrays (`vaz[]`, `head[]`)
- Power and efficiency curves
- Reference frequency and number of stages
- Minimum operating frequency
- Motor efficiency and thermal fraction

---

## Boundary Conditions, Master Valve, PIG, and Output Parsing

### `parse_master1()` — Master valve (wet christmas tree)

Reads the master valve control. The master valve is the main flow-control valve on the wet christmas tree, typically located at the seabed in offshore wells.

- Opening schedule: `tempo[]` → `abertura[]` (0 = closed, 1 = fully open)
- Area ratio for activation threshold `razareaativ`
- Optional Cv curve for pressure-drop calculation

### `parse_separador()` — Separator pressure

Reads the downstream pressure BC:
- Time series: `tempo[]` → `pres[]`
- Interpolated during transient via `atualiza()`

### `parse_chokeSup()` — Surface choke

Reads the surface choke (topside) parameters:
- Relative opening schedule `abertura[]` (fraction of pipe area)
- Discharge coefficient (`cd`)
- Multiphase choke model selector (`modelo`; 0 = Sachdeva)
- Optional Cv curve (`curvaCV`, `cvCurv[]`) and dynamic valve model (`curvaDinamic`)

### `parse_pig()` — PIG (Pipeline Inspection Gauge)

Reads PIG parameters (the PIG is an accessory that travels inside the pipeline):
- Launch cell index (`lanca`) and receive cell index (`receb`)
- Launch position along the line (`compL`) and receive position (`compR`), in metres from the well bottom
- Launch time (`tempo`)

### Output configuration

- `parse_perfil_producao()` / `parse_perfil_servico()`: which variables to write in spatial profiles, at what time intervals
- `parse_tendencia_producao()` / `parse_tendencia_servico()`: which variables to track over time at specific cell positions

---

## SProd Construction

The `SProd` constructor ties everything together:

```cpp
SProd::SProd(string nomeArquivoEntrada, string nomeArquivoLog,
             tipoValidacaoJson_t validacaoJson, tipoSimulacao_t tipoSimulacao,
             varGlob1D *Vvg1dSP = 0, int TD = -1, int vbloq = 0,
             int temporario = 0, int reverso = 0,
             double *compfonte = 0, int *posicfonte = 0,
             int nfontes = 0, int redeperm = 1)
    : arq(nomeArquivoEntrada, nomeArquivoLog, validacaoJson, tipoSimulacao,
          reverso, Vvg1dSP, redeperm),           // ← Ler constructed (full JSON parse)
      flut(arq.ncelp, arq.nvarprofp + 2 + 1 + 1 + 1 + 1),   // profile output table
      flutG(arq.ncelg, arq.nvarprofg + 2 + 1 + 1 + 1 + 1 + 1), // gas-line profile table
      matglobP(2 * arq.ncelp, 3, 2),              // production banded matrix
      termolivreP(2 * arq.ncelp),                 // production RHS vector
      matglobG(3 * arq.ncelg, 5, 5),              // gas-line banded matrix
      termolivreG(3 * arq.ncelg)                  // gas-line RHS vector
{
    // Zero ~80 member variables
    // ...
    if (TD >= 0)
        arq.tabelaDinamica = TD;    // optional dynamic-table override
    // ...
    montasistema(compfonte, posicfonte, nfontes);  // ← build the mesh
}
```

The initializer list is critical:
1. **`arq`** (type `Ler`) is constructed first — this triggers the full JSON parse
2. **`flut` / `flutG`** (profile tables) are sized from the parsed cell counts plus fixed extra columns
3. **`matglobP` / `matglobG`** (banded matrices) are sized: 2 DOFs/cell for production (P, ṁ), 3 DOFs/cell for gas line (P, ṁ, T)
4. **`termolivreP` / `termolivreG`** (RHS vectors) match the matrix sizes

The additional constructor parameters after `Vvg1dSP`:
| Parameter | Default | Meaning |
|-----------|---------|---------|
| `TD` | `-1` | Dynamic-table override index; applied to `arq.tabelaDinamica` when ≥ 0 |
| `vbloq` | `0` | Network blocking mode — forces an inlet source at cell 1 |
| `temporario` | `0` | Marks this section as a temporary network segment |
| `reverso` | `0` | Reverses geometry direction (passed through to `Ler`) |
| `compfonte` / `posicfonte` / `nfontes` | `0`/`0`/`0` | External gas sources from the network layer |
| `redeperm` | `1` | Enables steady-state network mode |

---

<a id="montasistema--building-the-simulation-mesh"></a>
## montasistema — Building the Simulation Mesh

`SProd::montasistema()` transforms the parsed `arq` data into the simulation-ready `Cel[]` and `CelG[]` arrays:

```
montasistema(compfonte, posicfonte, nfontes)
  │
  ├─ Copy global params from arq → SProd members
  │    (npontos, trackRGO, lingas, nfluP, ...)
  │
  ├─ Set up fluid properties for all fluids
  │    (npontos, table bounds, PVT file paths)
  │
  ├─ arq.geraduto()
  │    └─ detduto[] + corteduto[] + material[] → DadosGeo dutosMRT[]
  │       (each DadosGeo stores: diameter, roughness, perimeter, area,
  │        inclination angle, layer conductivities/densities/Cp)
  │
  ├─ celula = new Cel[ncel + 1]
  ├─ arq.geracelp(celula)
  │    └─ detcelp[] → Cel[]
  │       For each cell:
  │         - Clamp P/T to PVT table bounds
  │         - Compute face widths (dxL, dxR) and interpolation ratios
  │         - Compute face P/T by interpolation
  │         - Compute liquid/gas volumetric flows (QL, QG)
  │         - Evaluate void fraction from holdup
  │         - Evaluate densities via flup[].MasEspLiq() / MasEspGas()
  │         - Compute total and liquid mass flows (MC, Mliqini)
  │         - Assign geometry (duto, dutoL, dutoR) from dutosMRT[]
  │         - Build TransCal heat-transfer object per cell
  │
  ├─ Attach accessories (conditional on parsed counts):
  │    ├─ arq.geraipr(celula)          → acsr.tipo = 3  (IPR)
  │    ├─ arq.gerafgasVGL(celula)      → acsr.tipo = 1  (gas-lift valve)
  │    ├─ arq.gerafgasFonte(celula)    → acsr.tipo = 1  (gas injection)
  │    ├─ arq.gerafliqFonte(celula)    → acsr.tipo = 2  (liquid injection)
  │    ├─ arq.gerafmassFonte(celula)   → acsr.tipo = 10 (mass source)
  │    ├─ arq.geraFuro(celula)         → leak/puncture source
  │    ├─ arq.gerafBCS(celula)         → acsr.tipo = 4  (BCS pump)
  │    ├─ arq.gerafBVOL(celula)        → acsr.tipo = 8  (volumetric pump)
  │    ├─ arq.geraDPReq(celula)        → acsr.tipo = 7  (generic ΔP)
  │    ├─ arq.geraMaster1(celula)      → master valve
  │    ├─ arq.geraMaster2(celula)      → secondary master
  │    └─ ... (pig, choke, etc.)
  │
  ├─ Build gas-line cells (if lingas > 0):
  │    ├─ celulaG = new CelG[ncelGas + 1]
  │    ├─ arq.geracelg(celulaG)
  │    └─ Set up chokeVGL[] array, valve positions
  │
  └─ Initialize solver state
       (set presE, tempE, dt, counters, trend buffers, ...)
```

### Key mesh-building helpers

| Method | Input | Output |
|--------|-------|--------|
| `geraduto()` | `duto[]` + `corte[]` + `mat[]` | `DadosGeo dutosMRT[]` — fully characterized pipe geometry per segment |
| `novatrans()` | `dutosMRT[]` + cell external conditions | `TransCal` per cell — radial heat-transfer objects (layers, ambient T) |
| `geracelp()` | `celp[]` + `dutosMRT[]` + `flup[]` | `Cel celula[]` — simulation cells with initial P, T, α, β, ṁ, ρ, geometry |
| `geracelg()` | `celg[]` + gas fluid | `CelG celulaG[]` — gas-line cells |
| `geraipr()` | `IPRS[]` | Attaches `IPR` objects to cells (`acsr.tipo = 3`) |
| `gerafBCS()` | `bcs[]` | Attaches `BomCentSub` objects to cells (`acsr.tipo = 4`) |
| `gerafgasVGL()` | `valvgl[]` | Attaches `InjGas` objects to cells (`acsr.tipo = 1`) |

---

## Time-Dependent BC Updates (atualiza)

During transient simulations, `Ler::atualiza()` is called every time step to update time-varying boundary conditions by interpolating their schedule arrays:

1. **Master valve**: interpolates `master1.abertura[]` vs time → computes current `AreaGarg` (throat area). Supports Cv-curve or linear aperture
2. **Valves**: loop over `nvalv` valves, interpolate opening schedule. Supports a **dynamic valve model** (`curvaDinamic == 1`) with a spring-damper ODE for valve stem position
3. **Liquid density correction**: interpolates `VcorrecaoMassaEspLiq` vs time
4. **Inlet pressure/temperature**: if `ConContEntrada == 1`, interpolates inlet P and T from `CCPres` time series
5. **Separator pressure**: interpolates `psep` time series
6. **Source rates**: gas, liquid, and mass source flow rates are interpolated
7. **BCS frequency**: pump operating frequency schedule

The interpolation uses `indraz()` — a helper that finds the bracketing interval and returns the linear interpolation ratio.

---

## Output Methods on Ler

The `Ler` class also handles output formatting, since it knows the requested output variables:

### `imprimeProfile()` — Spatial profiles

Writes a row per cell to the profile matrix `flut[][]`:
- Column 0: cumulative distance
- Column 1: cell center distance
- Column 2: time stamp
- Subsequent columns: P, T, holdup, FVH (hydrate fraction), β, u_gs, u_ls, flow regime, etc. — only those requested in the JSON output config

### `imprimeProfileTrans()` — Radial temperature profiles

Writes radial temperature distributions for cells with 2D/3D thermal models.

### `imprimeTrend()` — Time-series trends

Writes a row per time step for a monitored cell:
- Column 0: time
- Subsequent columns: P, T, holdup, FVH, β, u_gs, u_ls, etc. at the monitored cell position

---

## Parametric Analysis (APara)

> Full documentation: [Parametric Analysis](parametric-analysis.md)

The `APara` class ([`LerAP.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LerAP.h) / [`LerAP.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LerAP.cpp)) reads parametric analysis JSON files. It allows automated parameter sweeps:

- Defines per-variable struct arrays (`detIPRAS`, `detBCSAS`, `detFONGASAS`, etc.) with `vector<>` storage for multiple values
- A `casoVEC` struct holds indices for building the parametric case matrix (all combinations)
- Constructor signature: `ASens(varGlob1D*, string IMPFILE, int vncel, detcelp*, ProFlu*, detBCS*, detFONGAS*)`
- Used by `Num4Main.cpp` to loop over parameter combinations and call the solver for each case

---

## Steam Injection Variant (LerVap)

The `LerVap` class ([`LeituraVapor.h`](https://github.com/petrobras/marlim3/blob/main/src/include/LeituraVapor.h) / [`LeituraVapor.cpp`](https://github.com/petrobras/marlim3/blob/main/src/core/LeituraVapor.cpp)) mirrors `Ler` but for **steam injection** simulations. It has:

- Analogous structs: `materialVap`, `cortedutoVap`, `detalhaPVap`, `detBCSVap`, etc.
- A simplified subset — no gas-lift service line, no multiphase slip, no compositional model
- Steam-specific properties: steam quality, enthalpy, saturation curves

---

## JSON Input Structure Reference

The formal JSON schema for a single-branch input file is maintained in [`docs/schemas/branch.pt.json`](../schemas/branch.pt.json). Refer to that file for the complete list of accepted keys, types, required fields, and allowed values.

---

## Summary of Key Methods

| Method | File | Purpose |
|--------|------|---------|
| `JSONRootObject::load()` | JSONDataModel.cpp | Load JSON file via RapidJSON, parse recursively |
| `JSONObject::load()` | JSONDataModel.cpp | Recursive key-matching into typed containers |
| `Ler::Ler()` | Leitura.cpp | Constructor — zero-init + call `lerArq()` |
| `Ler::iniciarVariaveis()` | Leitura.cpp | Zero-initialize ~100+ member variables |
| `Ler::parseEntrada()` | Leitura.cpp | Load JSON file, check parse errors, return `JSON_entrada` |
| `Ler::lerArq()` | Leitura.cpp | Master dispatcher — ~40 `parse_*` calls in order |
| `Ler::parse_configuracao_inicial()` | Leitura.cpp | Simulation flags and defaults |
| `Ler::parse_materiais()` | Leitura.cpp | Material thermal properties → `mat[]` |
| `Ler::parse_corte()` | Leitura.cpp | Cross-sections → `corte[]` |
| `Ler::parse_fluidos_producao()` | Leitura.cpp | Fluid properties → `flup[]` (ProFlu) |
| `Ler::parse_tabela()` | Leitura.cpp | PVT table grid → `detTabelaEntalp tabent` |
| `Ler::parse_tempo()` | Leitura.cpp | Time step + schedule → `dtmaxserie`, `tfinal` (transient production mode only) |
| `Ler::parse_unidades_producao()` | Leitura.cpp | Production line segments → `unidadeP[]`, `duto[]` |
| `Ler::parse_unidades_servico()` | Leitura.cpp | Gas service line segments |
| `Ler::parse_ipr()` | Leitura.cpp | IPR definitions → `IPRS[]` |
| `Ler::parse_bcs()` | Leitura.cpp | BCS pump curves → `bcs[]` |
| `Ler::parse_fonte_gaslift()` | Leitura.cpp | Gas-lift valve geometry → `valvgl[]` |
| `Ler::parse_valv()` | Leitura.cpp | Valve schedules → `detValv` |
| `Ler::parse_master1()` | Leitura.cpp | Master valve schedule (wet christmas tree) |
| `Ler::parse_separador()` | Leitura.cpp | Separator pressure schedule |
| `Ler::parse_pig()` | Leitura.cpp | PIG parameters (`lanca`, `receb`, `compL`, `compR`, `tempo`) |
| `Ler::geraduto()` | Leitura.cpp | `detduto[]` → `DadosGeo dutosMRT[]` |
| `Ler::novatrans()` | Leitura.cpp | Build `TransCal` heat-transfer objects per cell |
| `Ler::geracelp()` | Leitura.cpp | `detcelp[]` → `Cel celula[]` (production cells) |
| `Ler::geracelg()` | Leitura.cpp | `detcelg[]` → `CelG celulaG[]` (gas-line cells) |
| `Ler::geraipr()` | Leitura.cpp | Attach IPR to cells (`acsr.tipo = 3`) |
| `Ler::gerafBCS()` | Leitura.cpp | Attach BCS pumps (`acsr.tipo = 4`) |
| `Ler::gerafgasVGL()` | Leitura.cpp | Attach gas-lift valves (`acsr.tipo = 1`) |
| `Ler::atualiza()` | Leitura.cpp | Time-dependent BC interpolation |
| `Ler::imprimeProfile()` | Leitura.cpp | Write spatial profiles |
| `Ler::imprimeTrend()` | Leitura.cpp | Write time-series trends |
| `SProd::SProd()` | SisProd.cpp | Constructor — builds `arq`, sizes matrices, calls `montasistema()` |
| `SProd::montasistema()` | SisProd.cpp | Build mesh: `geraduto` → `geracelp` → attach accessories |
| `APara::APara()` | LerAP.cpp | Parametric analysis reader |
