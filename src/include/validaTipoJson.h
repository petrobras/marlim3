/*
 * validaJson.h
 *
 *  Created on: Mar 20, 2026
 *      Author: Eduardo
 */

#ifndef VALIDATIPOJSON_H_
#define VALIDATIPOJSON_H_

#include "JSON_entrada.h"
#include "Log.h"
#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <string>
#include <vector>

#ifdef linux
// Directory separator used on Linux systems
#define BARRA "/"
#elif LINUX
// Directory separator used on Linux systems
#define BARRA "/"
#elif Linux
// Directory separator used on Linux systems
#define BARRA "/"
#elif UNIX
// Directory separator used on Unix-based systems
#define BARRA "/"
#elif defined WIN32
// Directory separator used on Windows systems
#define BARRA "\\"
#elif defined Win32
// Directory separator used on Windows systems
#define BARRA "\\"
#elif defined win32
// Directory separator used on Windows systems
#define BARRA "\\"
#else
// Fallback directory separators for other platforms
#define BARRA "/\\"
#endif

using namespace std;
using namespace rapidjson;

// Output stream used to generate simulation data file reports
extern ofstream arqRelatorioPerfis;
// Logger instance used by the application
extern Logger logger;
// Path to the simulator input files
extern string pathArqExtEntrada;
// Output file path prefix used for POCO_INJETOR
extern string pathPrefixoArqSaida;
extern string versao;

class validadorTipo {
  public:
    validadorTipo() {}
    void validaGeral(Document &jsonDoc);
    // Methods used to validate the input file
    void valida_configuracao_inicial(Value &configuracao_inicial_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_condcont_pocinjec(Value &condcont_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_tempo(Value &tempo_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_materiais(Value &material_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_corte(Value &corte_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_tabela(Value &tabela_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_parafina(Value &parafina_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fluidos_producao(Value &fluidos_producao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fluido_complementar(Value &fluido_complementar_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fluido_gas(Value &fluido_gas_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_unidades_producao(Value &duto_producao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_unidades_servico(Value &duto_servico_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_hidrato(Value &hidrato_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_ipr(Value &ipr_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_separador(Value &separador_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_correcao(Value &correcao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_chokeSup(Value &chokeSup_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_chokeInj(Value &chokeInj_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_master1(Value &master1_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_master2(Value &master2_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_valv(Value &valvula_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_gasInj(Value &gasInj_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonte_gas(Value &fonte_gas_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonte_liquido(Value &fonte_liquido_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonte_PoroRadial(Value &fonte_poroRadial_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonte_Poro2D(Value &fonte_poro2D_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonte_massa(Value &fonte_massa_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_furo(Value &fontePressao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fontechk(Value &fontechk_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_pig(Value &pig_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_bcs(Value &bcs_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_multibcs(Value &multibcs_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_bomba_volumetrica(Value &bomba_volumetrica_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_delta_pressao(Value &delta_pressao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonteCalor(Value &fonteCalor_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_fonte_gaslift(Value &fonte_gaslift_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_intermitencia(Value &intermitencia_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_perfil_producao(Value &perfis_producao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_perfil_servico(Value &perfis_servico_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_tendencia_producao(Value &tendencia_producao_json, std::vector<std::string> &erros, bool &sucesso);
    void valida_tendencia_servico(Value &tendencia_servico_json, std::vector<std::string> &erros, bool &sucesso);
};

#endif /* VALIDATIPOJSON_H_ */