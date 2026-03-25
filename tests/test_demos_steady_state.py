"""
Testes de integração — execução dos casos JSON da pasta demos/.

Carrega cada JSON, instancia um Tramo, executa a simulação e verifica
se os resultados foram gerados.

Execução:
    pytest tests/test_demos.py -v -m simulacao
"""

import importlib.resources as pkg_resources
import os
import platform
import shutil

import pandas as pd
import pytest

import marlim3

# ============================================================================
# Verificação do executável
# ============================================================================

def _executavel_disponivel():
    exe_name = "marlim3.exe" if platform.system() == "Windows" else "marlim3"
    for name in [exe_name, "Marlim3.exe" if platform.system() == "Windows" else "Marlim3"]:
        try:
            with pkg_resources.path("marlim3", name) as p:
                if p.exists():
                    return True
        except (FileNotFoundError, AttributeError, TypeError):
            continue
    return False


skip_sem_executavel = pytest.mark.skipif(
    not _executavel_disponivel(),
    reason="Executável Marlim3 não encontrado no pacote",
)

# ============================================================================
# Definição dos demos e seus arquivos auxiliares
# ============================================================================
#
# Chave  : nome do JSON principal (demo a ser simulado)
# Valor  : lista de arquivos auxiliares necessários (da pasta demos/)
#
# Apenas os JSONs listados aqui serão executados como teste.
# Arquivos que são apenas auxiliares (ex: leituraAS.json) NÃO devem
# ter entrada própria como chave.

DEMOS = {
   # "2zonas-2VGLs-2-Check-AS.json":            ["leituraAS.json", "PVTSIM-MARLIM.tab"],
    "2zonas-2VGLs-2-Check-correcPerfTerm.json": ["PVTSIM-MARLIM.tab"],
    "BCS-longo-eficMotor.json":                 ["PVTSIM-MARLIM.tab"],
    "injec-Liq-TempoResidencia.json":           [],
    "parada-longo-Combinado-BCS-GLC-PIG-completo.json": ["PVTSIM-MARLIM.tab"],
    "producaoSimplificado.json":                [],
}

# ============================================================================
# Caminhos
# ============================================================================

DEMOS_DIR = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "demos"))
OUTPUT_DIR = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "saida_testes"))

DEMO_FILES = sorted(DEMOS.keys())

# ============================================================================
# Testes
# ============================================================================

@skip_sem_executavel
@pytest.mark.simulacao
@pytest.mark.parametrize("json_file", DEMO_FILES, ids=DEMO_FILES)
def test_demo_simula(json_file):
    """Carrega o JSON, executa a simulação e verifica se gerou resultados."""
    json_path = os.path.join(DEMOS_DIR, json_file)
    label = os.path.splitext(json_file)[0]

    caso = marlim3.Tramo()
    caso.from_json(json_path)

    # Passando para permanente
    # Reduzir tempo de simulação para acelerar os testes
    if "transiente" in caso.configuracaoInicial:
        caso.configuracaoInicial["transiente"] = False

    out_dir = os.path.join(OUTPUT_DIR, label)
    os.makedirs(out_dir, exist_ok=True)

    # Copia apenas os arquivos auxiliares necessários para este demo
    for aux in DEMOS[json_file]:
        src = os.path.join(DEMOS_DIR, aux)
        if os.path.isfile(src):
            shutil.copy2(src, out_dir)

    # Usa caminho absoluto para o diretório de resultados, evitando
    # problemas com caminhos relativos após troca de CWD.
    results_dir = os.path.join(out_dir, f"resultados_{label}")

    original_cwd = os.getcwd()
    try:
        os.chdir(out_dir)
        caso.simular(label=label, diretorio=results_dir)
    finally:
        os.chdir(original_cwd)

    # O método simular() move simulacao.log do CWD para results_dir.
    # Copiamos de volta para out_dir para facilitar a consulta.
    log_in_results = os.path.join(results_dir, "simulacao.log")
    log_in_outdir = os.path.join(out_dir, "simulacao.log")
    if os.path.isfile(log_in_results):
        shutil.copy2(log_in_results, log_in_outdir)
    elif os.path.isfile(log_in_outdir):
        pass  # já está no lugar certo

    assert caso.resultados, f"Simulação de {json_file} não gerou resultados"
