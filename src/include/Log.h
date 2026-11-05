/*
 * Log.h
 *
 * Created on: July 12, 2017
 *     Author: zy5m
 */

#ifndef LOG_H_
#define LOG_H_
#define LOG_ERR_UNEXPECTED_EXCEPTION "1000"
#define LOG_ERR_PARSE_JSON_FORMAT_VALIDATION "1001"
#define LOG_ERR_PARSE_SCHEMA_VALIDATION "1002"
#define LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION "1003"
#define LOG_WARN_PARSE_BUSINESS_RULE_VALIDATION "1004"
#define LOG_ERR_NUMERICAL_FAILURE "1005"
#define LOG_WARN_STEADY_STATE_NOT_CONVERGED "1006"
#define LOG_WARN_UNIT_DISCARDED "1007"
#define LOG_WARN_BRANCH_DISCARDED "1008"
#define CODIGO_SAIDA_EXECUCAO_PARCIAL 5
#define LOG_INFO_PARSE_PROCESS_FINISHED "1100"
#define LOG_INFO_PARSE_PROCESS_GENERIC "1101"
#define LOG_INFO_SIMULATION_FINISHED "1200"
#define LOG_WARN_SIMULATION_CANCELLED "1300"
#define ARQUIVO_LOG_PADRAO "simulacao.log"
#define ARQUIVO_TEMPLATE_LOG "template_log_transiente.json"
#define TAMANHO_MAXIMO_NOME_ARQUIVO 100

#include "rapidjson/document.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <map>
#include <exception>
#include <stdexcept>
#include <vector>
#include <omp.h>

using namespace std;

using namespace rapidjson;

extern string versao;

extern int logRede;

extern string nomeRedePrincipal;

typedef enum {
    LOGGER_NONE,
    LOGGER_INFO,
    LOGGER_AVISO,
    LOGGER_FALHA
} log_t;

// Etapa da simulacao; tambem e o codigo de saida do executavel quando ha FALHA nela.
// ETAPA_HERDADA so e usada em regioes paralelas, para voltar a etapa global.
typedef enum {
    ETAPA_HERDADA = 0,
    ETAPA_PRE = 1,
    ETAPA_PERMANENTE = 2,
    ETAPA_TRANSIENTE = 3,
    ETAPA_POS = 4
} etapa_t;

// Falha numerica de uma unidade de varredura (sequencia da analise parametrica): descarta so a unidade.
class ErroNumerico : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

// Excecoes nao podem atravessar uma regiao OpenMP: cada iteracao guarda a primeira e a thread mestre a relanca.
class CapturaExcecao {
  public:
    void guarda() {
#pragma omp critical(marlim_captura_excecao)
        if (!excecao)
            excecao = std::current_exception();
    }
    void relanca() {
        if (excecao)
            std::rethrow_exception(excecao);
    }

  private:
    std::exception_ptr excecao;
};

typedef struct {
    string nomeArqEntrada;
    string nomeArqLog;
    string dataExecucao;
    bool started;
    bool sucesso;
    int totalInfos;
    int totalAvisos;
    int totalFalhas;
    struct {
        vector<log_t> tipos;
        vector<string> codigos;
        vector<string> descricoes;
        vector<string> propriedades;
        vector<string> causas;
        vector<int> linhas;
        vector<etapa_t> etapas;
        vector<double> tempos; // instante simulado (s) no transiente; negativo fora dele
        vector<int> sequencias; // sequencia da analise parametrica; negativo fora dela
        vector<string> tramos; // arquivo do tramo da rede em solucao; vazio fora de rede
        vector<int> redesInternas; // negativo se a rede nao foi dividida
    } evento;
} stResultadoSimulacao_t;

class Logger {
  public:
    Logger();
    Logger(const string arquivo);
    Logger(const Logger &); // Copy constructor
    Logger &operator=(const Logger &);
    ~Logger() {
    }

    std::string vers;

    void setNomeArqLog(string nomeArquivoLog);
    void setNomeArqEntrada(string nomeArquivoEntrada);
    void changeResultadoSimulacao(string nomeArquivoEntrada, string nomeArquivoLog);
    Document parseTemplateLogOutput();
    void log(const log_t tipo, const string codigo, const string complemento, const string propriedade,
             const string causa, const int linha);
    void log(const log_t tipo, const string codigo, const string complemento, const string propriedade,
             const string causa);
    void writeJsonFile(Document *outputDoc);
    void writeOutputLog(int tipoSaida = 1);
    void write_logs_and_exit();
    void log_write_logs_and_exit(const log_t tipo, const string codigo, const string complemento,
                                 const string propriedade, const string causa);
    void log_write_logs_and_exit(const log_t tipo, const string codigo, const string complemento,
                                 const string propriedade, const string causa, const int linha);
    void setVersao(const std::string &) { vers = versao; }

    // Contexto de execucao gravado em cada evento. Chamadas dentro de uma regiao OpenMP ativa
    // valem so para a thread corrente (redes internas resolvidas em paralelo).
    void setEtapa(etapa_t etapa);
    void setRelogio(const double *tempoSimulado); // nullptr desliga o registro do instante
    void setTempoFinal(double tempo) { tempoFinal = tempo; }
    void setSequenciaAP(int sequencia); // -1 fora da analise parametrica
    int sequenciaAtual() const;
    void setTramo(const string &arquivoTramo, int redeInterna); // "" e -1 limpam
    string tramoAtual() const;
    int redeInternaAtual() const;
    // durante uma varredura (analise parametrica) falhas numericas viram ErroNumerico
    void entrarVarredura();
    void sairVarredura();
    bool emVarredura() const;
    // unidade descartada ("sequenciaAP" ou "tramo"); leva o codigo de saida a 5 se nao houver FALHA
    bool registrarParcial(const string &tipo, const string &item); // true se o item e novo
    // total de unidades da varredura; se todas forem descartadas, o log registra FALHA ao ser gravado
    void definirTotalUnidades(const string &tipo, int total);
    etapa_t etapaAtual() const;
    double tempoAtual() const; // negativo se nao houver relogio ativo
    // 0 sem FALHA; 5 sem FALHA mas com unidades descartadas; senao a etapa da primeira FALHA (1 a 4)
    int codigoSaida() const;

    const stResultadoSimulacao_t &getStResultadoSimulacao() const {
        return this->stResultadoSimulacao;
    }
    void setStResultadoSimulacao(const stResultadoSimulacao_t &stResultadoSimulacao) {
        this->stResultadoSimulacao = stResultadoSimulacao;
    }

    stResultadoSimulacao_t stResultadoSimulacao;

  private:
    etapa_t etapa = ETAPA_PRE;
    const double *relogio = nullptr;
    double tempoFinal = -1.;
    int sequenciaAP = -1;
    string tramo;
    int redeInterna = -1;
    map<string, vector<string>> parciais;
    map<string, int> totaisUnidades;
    map<string, etapa_t> etapasDescarte;
    bool descarteTotalVerificado = false;
    void verificarDescarteTotal();
    bool varredura = false;
};

#endif /* LOG_H_ */
