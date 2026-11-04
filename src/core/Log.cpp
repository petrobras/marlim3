/*
 * Log.cpp
 *
 *  Created on: 12/07/2017
 *      Author: zy5m
 */

#include "Log.h"
#include <stdlib.h>
#include <cmath>
#include <string>

using namespace rapidjson;

namespace {

void initializeResultadoSimulacao(stResultadoSimulacao_t &resultado) {
    // status da inicação do log
    resultado.started = false;
    // iniciar valores dos atributos
    resultado.sucesso = true;
    resultado.totalInfos = -1;
    resultado.totalAvisos = -1;
    resultado.totalFalhas = -1;
}

bool getLocalTime(time_t source, struct tm &dest) {
#ifdef _WIN32
    return localtime_s(&dest, &source) == 0;
#else
    return localtime_r(&source, &dest) != NULL;
#endif
}

string formatDataExecucao(const struct tm &agora) {
    return to_string(agora.tm_mday) + "/" + to_string(agora.tm_mon + 1) + "/" +
           to_string(agora.tm_year + 1900) + " hora: " + to_string(agora.tm_hour) + ":" +
           to_string(agora.tm_min);
}

// Contexto proprio de cada thread dentro de regioes paralelas (redes internas em paralelo).
thread_local etapa_t etapaThread = ETAPA_HERDADA;
thread_local const double *relogioThread = nullptr;
constexpr int SEQUENCIA_HERDADA = -2;
thread_local int sequenciaThread = SEQUENCIA_HERDADA;
thread_local bool tramoThreadDefinido = false;
thread_local string tramoThread;
thread_local int redeInternaThread = -1;

double arredonda4(double valor) {
    return std::round(valor * 1.e4) / 1.e4;
}

const char *nomeEtapa(etapa_t etapa) {
    switch (etapa) {
    case ETAPA_PRE:
        return "PRE";
    case ETAPA_PERMANENTE:
        return "PERMANENTE";
    case ETAPA_TRANSIENTE:
        return "TRANSIENTE";
    case ETAPA_POS:
        return "POS";
    default:
        return "PRE";
    }
}

} // namespace

void Logger::setEtapa(etapa_t novaEtapa) {
    if (omp_in_parallel()) {
        etapaThread = novaEtapa;
    } else {
        etapaThread = ETAPA_HERDADA;
        if (novaEtapa != ETAPA_HERDADA)
            etapa = novaEtapa;
    }
}

void Logger::setRelogio(const double *tempoSimulado) {
    if (omp_in_parallel()) {
        relogioThread = tempoSimulado;
    } else {
        relogioThread = nullptr;
        relogio = tempoSimulado;
    }
}

void Logger::setSequenciaAP(int sequencia) {
    if (omp_in_parallel()) {
        sequenciaThread = sequencia;
    } else {
        sequenciaThread = SEQUENCIA_HERDADA;
        sequenciaAP = sequencia;
    }
}

void Logger::setTramo(const string &arquivoTramo, int rede) {
    const size_t barra = arquivoTramo.find_last_of("/\\");
    const string nome = barra == string::npos ? arquivoTramo : arquivoTramo.substr(barra + 1);
    if (omp_in_parallel()) {
        tramoThreadDefinido = !nome.empty() || rede >= 0;
        tramoThread = nome;
        redeInternaThread = rede;
    } else {
        tramoThreadDefinido = false;
        tramo = nome;
        redeInterna = rede;
    }
}

string Logger::tramoAtual() const {
    return tramoThreadDefinido ? tramoThread : tramo;
}

int Logger::redeInternaAtual() const {
    return tramoThreadDefinido ? redeInternaThread : redeInterna;
}

int Logger::sequenciaAtual() const {
    return sequenciaThread != SEQUENCIA_HERDADA ? sequenciaThread : sequenciaAP;
}

etapa_t Logger::etapaAtual() const {
    return etapaThread != ETAPA_HERDADA ? etapaThread : etapa;
}

double Logger::tempoAtual() const {
    const double *r = relogioThread != nullptr ? relogioThread : relogio;
    if (r == nullptr || etapaAtual() != ETAPA_TRANSIENTE)
        return -1.;
    return *r;
}

int Logger::codigoSaida() const {
    const stResultadoSimulacao_t &res = getStResultadoSimulacao();
    if (res.sucesso)
        return 0;
    for (size_t i = 0; i < res.evento.tipos.size(); i++) {
        if (res.evento.tipos[i] != LOGGER_FALHA)
            continue;
        // cancelamento pelo usuario (SIGINT=2, SIGTERM=15) nao e falha da simulacao
        const bool cancelamento = res.evento.codigos[i] == LOG_INFO_SIMULATION_CANCELLED &&
                                  (res.evento.descricoes[i] == "2" || res.evento.descricoes[i] == "15");
        if (!cancelamento)
            return static_cast<int>(res.evento.etapas[i]);
    }
    return 0;
}

Logger::Logger() {
    initializeResultadoSimulacao(this->stResultadoSimulacao);
    changeResultadoSimulacao(ARQUIVO_LOG_PADRAO, ARQUIVO_LOG_PADRAO);
}

Logger::Logger(const string ARQUIVO_LOG) {
    initializeResultadoSimulacao(this->stResultadoSimulacao);
    changeResultadoSimulacao(ARQUIVO_LOG, ARQUIVO_LOG);
}

Logger &Logger::operator=(const Logger &vLogger) {
    if (this != &vLogger) {
        stResultadoSimulacao = vLogger.stResultadoSimulacao;
    }
    return *this;
}

void Logger::setNomeArqEntrada(string nomeArquivoEntrada) {
    // trocar o nome do arquivo de entrada
    if (logRede == 0)
        this->stResultadoSimulacao.nomeArqEntrada = nomeArquivoEntrada;
    else
        this->stResultadoSimulacao.nomeArqEntrada = nomeRedePrincipal;
}

void Logger::setNomeArqLog(string nomeArquivoLog) {
    // trocar o nome do arquivo do log
    this->stResultadoSimulacao.nomeArqLog = nomeArquivoLog;
}

void Logger::changeResultadoSimulacao(const string nomeArquivoEntrada, const string nomeArquivoLog) {
    // caso seja a configuracao inicial
    if (nomeArquivoEntrada.size() > 0) {
        if (!this->stResultadoSimulacao.started) {
            // obter a data corrente
            time_t hoje = time(0);
            struct tm agora;
            // definir a data corrente
            string dataExecucao;
            if (getLocalTime(hoje, agora)) {
                dataExecucao = formatDataExecucao(agora);
            }
            this->stResultadoSimulacao.dataExecucao = dataExecucao;
            // status da inicação do log
            this->stResultadoSimulacao.started = true;
            // iniciar a variavel de status da simulacao
            this->stResultadoSimulacao.sucesso = true;
            // inicar os contadores
            this->stResultadoSimulacao.totalFalhas = 0;
            this->stResultadoSimulacao.totalAvisos = 0;
            this->stResultadoSimulacao.totalInfos = 0;
        }
        // atribuir o nome do arquivo de entrada
        setNomeArqEntrada(nomeArquivoEntrada);
        // atribuir o nome do arquivo de log
        setNomeArqLog(nomeArquivoLog);
    }
}

void Logger::log(const log_t tipo, const string codigo, const string complemento,
                 const string propriedade, const string causa, const int linha) {
    // contexto lido fora da secao critica: e proprio da thread que registra o evento
    const etapa_t etapaEvento = etapaAtual();
    const double tempoEvento = tempoAtual();
    const int sequenciaEvento = sequenciaAtual();
    const string tramoEvento = tramoAtual();
    const int redeInternaEvento = redeInternaAtual();
    // Called from the parallel parametric analysis: counters and event vectors must change together.
#pragma omp critical(marlim_logger)
    {
    switch (tipo) {
    case LOGGER_INFO:
        // incrementar o total de informacoes
        this->stResultadoSimulacao.totalInfos++;
        break;
    case LOGGER_AVISO:
        // incrementar o total de avisos
        this->stResultadoSimulacao.totalAvisos++;
        break;
    case LOGGER_FALHA:
        // incluir o status de falha
        this->stResultadoSimulacao.sucesso = false;
        // incrementar o total de falhas
        this->stResultadoSimulacao.totalFalhas++;
        break;
    default:
        break;
    }
    // incluir o tipo, o código, a descrição, a propriedade e a causa do log no array de eventos
    this->stResultadoSimulacao.evento.tipos.push_back(tipo);
    this->stResultadoSimulacao.evento.codigos.push_back(codigo);
    this->stResultadoSimulacao.evento.descricoes.push_back(complemento);
    this->stResultadoSimulacao.evento.propriedades.push_back(propriedade);
    this->stResultadoSimulacao.evento.causas.push_back(causa);
    this->stResultadoSimulacao.evento.linhas.push_back(linha);
    this->stResultadoSimulacao.evento.etapas.push_back(etapaEvento);
    this->stResultadoSimulacao.evento.tempos.push_back(tempoEvento);
    this->stResultadoSimulacao.evento.sequencias.push_back(sequenciaEvento);
    this->stResultadoSimulacao.evento.tramos.push_back(tramoEvento);
    this->stResultadoSimulacao.evento.redesInternas.push_back(redeInternaEvento);
    }
}

void Logger::log(const log_t tipo, const string codigo, const string complemento,
                 const string propriedade, const string causa) {
    log(tipo, codigo, complemento, propriedade, causa, 0);
}

Document Logger::parseTemplateLogOutput() {
    string strTemplateLog("{\"resultadoSimulacao\":{\"arquivo\":\"teste1.mrt\",\"data\":\"19/06/2017\",\"status\":\"falha\",\"logs\":[{\"log\":\"AVISO\",\"codigo\":\"1011\",\"descricao\":\"Campo X do arquivo MRT e obrigatorio.\",\"propriedade\":\"#\",\"causa\":\"#\",\"linha\":0}]},\"excecoesConfiguradas\":{\"1000\":\"Encontrada falha inesperada:\",\"1001\":\"O arquivo lido apresentou falha na sintaxe JSON:\",\"1002\":\"Encontrada falha durante a validacao das propriedades do MRT:\",\"1003\":\"Encontrada falha durante a validacao das regras de negocio do MRT:\",\"1004\":\"Aviso sobre as regras de negocio do simulador:\",\"1005\":\"Falha numerica durante a solucao:\",\"1006\":\"Regime permanente nao convergiu:\",\"1100\":\"Fim de etapa do processo de parse do arquivo MRT:\",\"1101\":\"Informacao:\",\"1200\":\"Fim da execucao do simulador:\",\"1300\":\"Cancelamento da execucao do simulador:\"}}");
    // criar string para mensagem de falha
    char mensagemFalha[5000];
    // criar objeto do elemento raiz do json de saída da simulação
    Document logOutDoc;
    // realizar a leitura do template do log
    try {
        // realizar o parse do elemento raiz do json do template de saída da simulação
        logOutDoc.Parse(strTemplateLog.c_str());
        // caso haja erro de parse
        if (logOutDoc.HasParseError()) {
            // transpor os dados para a mensagem
            sprintf(mensagemFalha, "Posicao = %zu", logOutDoc.GetErrorOffset());
            // incluir falha no processo de parse
            cout << "FALHA: " << LOG_ERR_PARSE_JSON_FORMAT_VALIDATION << " - Parse Template Log Transiente - "
                 << GetParseError_Pt_BR(logOutDoc.GetParseError()) << endl;
            // encerrar a aplicacao
            exit(EXIT_FAILURE);
        }
    } catch (exception &e) {
        // incluir falha
        cout << "FALHA: " << LOG_ERR_UNEXPECTED_EXCEPTION << " - Parse Template Log Transiente - " << e.what() << endl;
        // encerrar a aplicacao
        exit(EXIT_FAILURE);
    }
    // retorna o documento do template de saída
    return logOutDoc;
}

void Logger::writeJsonFile(Document *outputDoc) {
    try {
        remove(getStResultadoSimulacao().nomeArqLog.c_str());
        // criar arquivo de saída da simulação
        FILE *logOutFile = fopen(getStResultadoSimulacao().nomeArqLog.c_str(), "wb"); // non-Windows use "w"
        // definir buffer de saída da simulação
        char writeBuffer[65536];
        // criar stream do arquivo de saída da simulação
        FileWriteStream logOutStream(logOutFile, writeBuffer, sizeof(writeBuffer));
        // criar objeto para escrita da saída da simulação
        PrettyWriter<FileWriteStream> logOutWriter(logOutStream);
        logOutWriter.SetMaxDecimalPlaces(4);
        // descarregar o documento Json alterado para o arquivo de saída
        outputDoc->Accept(logOutWriter);
        // fechar o arquivo
        fclose(logOutFile);
    } catch (exception &e) {
        cout << "EXCECAO INESPERADA: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void Logger::writeOutputLog(int tipoSaida) {
    // criar objeto do elemento raiz do json de saída da simulação
    Document logOutDoc = parseTemplateLogOutput();
    // definir alocator para para atribuir valores de chaves e valores de propriedades do json de saída
    Document::AllocatorType &allocator = logOutDoc.GetAllocator();

    logOutDoc.AddMember("versao", StringRef(versao.c_str()), allocator);

    try {
        // recuperar o objeto de resultado da simulação do templateJson
        Value &resultadoSimulacao = logOutDoc["resultadoSimulacao"];
        // alterar o nome do arquivo de entrada da simulação
        resultadoSimulacao["arquivo"].SetString(StringRef(getStResultadoSimulacao().nomeArqEntrada.c_str()));
        // alterar a data da simulação
        resultadoSimulacao["data"].SetString(StringRef(getStResultadoSimulacao().dataExecucao.c_str()));
        // caso o resultado da simulação seja bem sucedido
        if (getStResultadoSimulacao().sucesso) {
            resultadoSimulacao["status"].SetString("sucesso");
        } else {
            resultadoSimulacao["status"].SetString("falha");
        }
        // etapa em que a simulacao terminou (ou falhou) e instante simulado, se no transiente
        const int codigo = codigoSaida();
        const etapa_t etapaFinal = codigo > 0 ? static_cast<etapa_t>(codigo) : etapaAtual();
        resultadoSimulacao.AddMember("etapa", StringRef(nomeEtapa(etapaFinal)), allocator);
        resultadoSimulacao.AddMember("codigoSaida", codigo, allocator);
        if (tempoAtual() >= 0.)
            resultadoSimulacao.AddMember("tempoSimulado", arredonda4(tempoAtual()), allocator);
        if (tempoFinal >= 0.)
            resultadoSimulacao.AddMember("tempoFinal", arredonda4(tempoFinal), allocator);
        // recuperar o objeto de excecoesConfiguradas do templateJson
        Value &excCfg = logOutDoc["excecoesConfiguradas"];
        // recuperar o array de logs do templateJson
        Value &logs = resultadoSimulacao["logs"];
        // remove todos os elementos do array de logs - que tenham sido preenchidos no template Json
        if (!logs.Empty()) {
            logs.Clear();
        }
        // array de tipos de log
        string tipoLog[4] = {"NONE", "INFO", "AVISO", "FALHA"};
        // percorrer o array de logs
        for (int indLog = 0; indLog < getStResultadoSimulacao().totalFalhas + getStResultadoSimulacao().totalAvisos + getStResultadoSimulacao().totalInfos; indLog++) {
            // criar tipo do log - chave e valor
            Value chaveTipo(StringRef("log"));
            Value valorTipo(StringRef(tipoLog[getStResultadoSimulacao().evento.tipos[indLog]].c_str()));
            // criar codigo do log - chave e valor
            Value chaveCodigo(StringRef("codigo"));
            Value valorCodigo(StringRef(getStResultadoSimulacao().evento.codigos[indLog].c_str()));
            // criar descrição do log - chave e valor
            Value chaveDescricao(StringRef("descricao"));
            string descricaoLog = string(excCfg[getStResultadoSimulacao().evento.codigos[indLog].c_str()].GetString()) +
                                  getStResultadoSimulacao().evento.descricoes[indLog];
            Value valorDescricao;
            valorDescricao.SetString(descricaoLog.c_str(), static_cast<SizeType>(descricaoLog.size()), allocator);
            // criar propriedade do log - chave e valor
            Value chavePropriedade(StringRef("propriedade"));
            Value valorPropriedade;
            valorPropriedade.SetString(getStResultadoSimulacao().evento.propriedades[indLog].c_str(),
                                       static_cast<SizeType>(getStResultadoSimulacao().evento.propriedades[indLog].size()),
                                       allocator);
            // criar causa do log - chave e valor
            Value chaveCausa(StringRef("causa"));
            Value valorCausa;
            valorCausa.SetString(getStResultadoSimulacao().evento.causas[indLog].c_str(),
                                 static_cast<SizeType>(getStResultadoSimulacao().evento.causas[indLog].size()),
                                 allocator);
            // criar linha do arquivo - chave e valor
            Value chaveLinha(StringRef("linha"));
            Value valorLinha(getStResultadoSimulacao().evento.linhas[indLog]);
            // objeto do log com código, descrição, propriedade e causa - chaves e valores
            Value log(kObjectType);
            log.AddMember(chaveTipo, valorTipo, allocator);
            log.AddMember(chaveCodigo, valorCodigo, allocator);
            log.AddMember(chaveDescricao, valorDescricao, allocator);
            log.AddMember(chavePropriedade, valorPropriedade, allocator);
            log.AddMember(chaveCausa, valorCausa, allocator);
            log.AddMember(chaveLinha, valorLinha, allocator);
            const etapa_t etapaEvento = getStResultadoSimulacao().evento.etapas[indLog];
            log.AddMember("etapa", StringRef(nomeEtapa(etapaEvento)), allocator);
            if (getStResultadoSimulacao().evento.tempos[indLog] >= 0.)
                log.AddMember("tempo", arredonda4(getStResultadoSimulacao().evento.tempos[indLog]), allocator);
            if (getStResultadoSimulacao().evento.sequencias[indLog] >= 0)
                log.AddMember("sequenciaAP", getStResultadoSimulacao().evento.sequencias[indLog], allocator);
            const string &tramoLog = getStResultadoSimulacao().evento.tramos[indLog];
            if (!tramoLog.empty()) {
                Value valorTramo;
                valorTramo.SetString(tramoLog.c_str(), static_cast<SizeType>(tramoLog.size()), allocator);
                log.AddMember("tramo", valorTramo, allocator);
            }
            if (getStResultadoSimulacao().evento.redesInternas[indLog] >= 0)
                log.AddMember("redeInterna", getStResultadoSimulacao().evento.redesInternas[indLog], allocator);
            // incluir o objeto do log no array de eventos de log
            logs.PushBack(log, allocator);
        }

        // Remove o bloco de exceções configuradas para não aparecer no arquivo de log final
        if (logOutDoc.HasMember("excecoesConfiguradas")) {
            logOutDoc.RemoveMember("excecoesConfiguradas");
        }
        // grava o arquivo de saida JSON
        writeJsonFile(&logOutDoc);

        // mensagem de saida
        if (tipoSaida == 1)
            cout << "ARQUIVO DE LOG: " << getStResultadoSimulacao().nomeArqLog << endl;
    } catch (exception &e) {
        cout << "EXCECAO INESPERADA: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void Logger::log_write_logs_and_exit(const log_t tipo, const string codigo, const string complemento,
                                     const string propriedade, const string causa) {
    // incluir o evento de falha no log
    log_write_logs_and_exit(tipo, codigo, complemento, propriedade, causa, 0);
}

void Logger::log_write_logs_and_exit(const log_t tipo, const string codigo, const string complemento,
                                     const string propriedade, const string causa, const int linha) {
    // incluir o evento de falha no log
    log(tipo, codigo, complemento, propriedade, causa, linha);
    // gerar o arquivo de saida da simulacao e encerrar a aplicacao
    write_logs_and_exit();
}

void Logger::write_logs_and_exit() {
    // gerar o arquivo de saida da simulacao
    writeOutputLog();
    // encerrar a aplicacao: 0 sem falha, senao a etapa da primeira falha
    exit(codigoSaida());
}
