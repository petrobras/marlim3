/*
 * Log.h
 *
 *  Created on: 12/07/2017
 *      Author: zy5m
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/schema.h"
#include "rapidjson/error/pt_BR.h"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace rapidjson;

#define LOG_ERR_UNEXPECTED_EXCEPTION "1000"
#define LOG_ERR_PARSE_JSON_FORMAT_VALIDATION "1001"
#define LOG_ERR_PARSE_SCHEMA_VALIDATION "1002"
#define LOG_ERR_PARSE_BUSINESS_RULE_VALIDATION "1003"
#define LOG_WARN_PARSE_BUSINESS_RULE_VALIDATION "1004"
#define LOG_INFO_PARSE_PROCESS_FINISHED "1100"
#define LOG_INFO_PARSE_PROCESS_GENERIC "1101"
#define LOG_INFO_SIMULATION_FINISHED "1200"
#define LOG_INFO_SIMULATION_CANCELLED "1300"

#define ARQUIVO_LOG_PADRAO "simulacao.log"
#define ARQUIVO_TEMPLATE_LOG "template_log_transiente.json"
#define TAMANHO_MAXIMO_NOME_ARQUIVO 100

extern string versao;
extern int logRede;
extern string nomeRedePrincipal;

typedef enum {
  LOGGER_NONE, LOGGER_INFO, LOGGER_AVISO, LOGGER_FALHA
} log_t;

typedef struct {
    char *nomeArqEntrada;
    char *nomeArqLog;
    char *dataExecucao;
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
    } evento;
} stResultadoSimulacao_t;

class Logger {
  public:
    Logger();
    Logger(const string arquivo);
    Logger(const Logger&);    //construtor por copia
    Logger& operator=(const Logger&);
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
    void writeOutputLog(int tipoSaida=1);
    void write_logs_and_exit();
    void log_write_logs_and_exit(const log_t tipo, const string codigo, const string complemento,
        const string propriedade, const string causa);
    void log_write_logs_and_exit(const log_t tipo, const string codigo, const string complemento,
        const string propriedade, const string causa, const int linha);
    void setVersao(const std::string&){ vers = versao; }

    const stResultadoSimulacao_t& getStResultadoSimulacao() const {
      return this->stResultadoSimulacao;
    }
    void setStResultadoSimulacao(const stResultadoSimulacao_t& stResultadoSimulacao) {
      this->stResultadoSimulacao = stResultadoSimulacao;
    }

  //private:
    stResultadoSimulacao_t stResultadoSimulacao;
};

#endif /* LOG_H_ */


