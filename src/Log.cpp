/*
 * Log.cpp
 *
 *  Created on: 12/07/2017
 *      Author: zy5m
 */

#include "Log.h"

using namespace rapidjson;

Logger::Logger() {
  // status da inicação do log
  this->stResultadoSimulacao.started = true;
  // iniciar valores dos atributos
  this->stResultadoSimulacao.sucesso = true;
  this->stResultadoSimulacao.totalInfos = -1;
  this->stResultadoSimulacao.totalAvisos = -1;
  this->stResultadoSimulacao.totalFalhas = -1;
  // alocar espaco para o nome do arquivo de log do parse
  this->stResultadoSimulacao.nomeArqLog = (char*) malloc(TAMANHO_MAXIMO_NOME_ARQUIVO * sizeof(char));
  // alocar espaco para o nome do arquivo de entrada do parse
  this->stResultadoSimulacao.nomeArqEntrada = (char*) malloc(TAMANHO_MAXIMO_NOME_ARQUIVO * sizeof(char));
  // alocar espaco para a data
  this->stResultadoSimulacao.dataExecucao = (char*) malloc(11 * sizeof(char));
  Logger("simulacao.log");
}

Logger::Logger(const string ARQUIVO_LOG) {
  changeResultadoSimulacao(ARQUIVO_LOG, ARQUIVO_LOG);
}

Logger& Logger::operator =(const Logger& vLogger) {
  if (this != &vLogger) {
    stResultadoSimulacao = vLogger.stResultadoSimulacao;
  }
  return *this;
}


void Logger::setNomeArqEntrada(string nomeArquivoEntrada) {
  // trocar o nome do arquivo de entrada
  if(logRede==0)
	  strcpy(this->stResultadoSimulacao.nomeArqEntrada, nomeArquivoEntrada.c_str());
  else strcpy(this->stResultadoSimulacao.nomeArqEntrada, nomeRedePrincipal.c_str());
}


void Logger::setNomeArqLog(string nomeArquivoLog) {
  // trocar o nome do arquivo do log
  strcpy(this->stResultadoSimulacao.nomeArqLog, nomeArquivoLog.c_str());
}


void Logger::changeResultadoSimulacao(const string nomeArquivoEntrada, const string nomeArquivoLog) {
  // caso seja a configuracao inicial
  if (nomeArquivoEntrada.size() > 0) {
	  if (! this->stResultadoSimulacao.started) {
//    if ((this->stResultadoSimulacao.nomeArqLog == NULL) || (strcmp(nomeArquivoLog.c_str(), this->stResultadoSimulacao.nomeArqLog) != 0)) {
      // alocar espaco para o nome do arquivo do log
      this->stResultadoSimulacao.nomeArqLog = (char*) malloc(TAMANHO_MAXIMO_NOME_ARQUIVO * sizeof(char));
      // alocar espaco para o nome do arquivo de entrada do parse
      this->stResultadoSimulacao.nomeArqEntrada = (char*) malloc(TAMANHO_MAXIMO_NOME_ARQUIVO * sizeof(char));
      // alocar espaco para a data
      this->stResultadoSimulacao.dataExecucao = (char*) malloc(11 * sizeof(char));
      // obter a data corrente
      time_t hoje = time(0);
      struct tm * agora = localtime(&hoje);
      // definir a data corrente
      sprintf(getStResultadoSimulacao().dataExecucao, "%d/%d/%d", agora->tm_mday, (agora->tm_mon + 1),
          (agora->tm_year + 1900));
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
}


void Logger::log(const log_t tipo, const string codigo, const string complemento,
    const string propriedade, const string causa) {
    log(tipo, codigo, complemento, propriedade, causa, 0);
}

Document Logger::parseTemplateLogOutput() {
  string strTemplateLog("{\"resultadoSimulacao\":{\"arquivo\":\"teste1.mrt\",\"data\":\"19/06/2017\",\"status\":\"falha\",\"logs\":[{\"log\":\"AVISO\",\"codigo\":\"1011\",\"descricao\":\"Campo X do arquivo MRT e obrigatorio.\",\"propriedade\":\"#\",\"causa\":\"#\",\"linha\":0}]},\"excecoesConfiguradas\":{\"1000\":\"Encontrada falha inesperada:\",\"1001\":\"O arquivo lido apresentou falha na sintaxe JSON:\",\"1002\":\"Encontrada falha durante a validacao das propriedades do MRT:\",\"1003\":\"Encontrada falha durante a validacao das regras de negocio do MRT:\",\"1004\":\"Aviso sobre as regras de negocio do simulador:\",\"1100\":\"Fim de etapa do processo de parse do arquivo MRT:\",\"1101\":\"Informacao:\",\"1200\":\"Fim da execucao do simulador:\",\"1300\":\"Cancelamento da execucao do simulador:\"}}");
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
      sprintf(mensagemFalha, "Posicao = %u", logOutDoc.GetErrorOffset());
      // incluir falha no processo de parse
      cout << "FALHA: " << LOG_ERR_PARSE_JSON_FORMAT_VALIDATION << " - Parse Template Log Transiente - "
          << GetParseError_Pt_BR(logOutDoc.GetParseError()) << endl;
      // encerrar a aplicacao
      exit(EXIT_SUCCESS);
    }
  } catch (exception& e) {
    // incluir falha
    cout << "FALHA: " << LOG_ERR_UNEXPECTED_EXCEPTION << " - Parse Template Log Transiente - " << e.what() << endl;
    // encerrar a aplicacao
    exit(EXIT_SUCCESS);
  }
  // retorna o documento do template de saída
  return logOutDoc;
}

void Logger::writeJsonFile(Document *outputDoc) {
  try {
    remove(getStResultadoSimulacao().nomeArqLog);
    // criar arquivo de saída da simulação
    FILE* logOutFile = fopen(getStResultadoSimulacao().nomeArqLog, "wb");    // non-Windows use "w"
    // definir buffer de saída da simulação
    char writeBuffer[65536];
    // criar stream do arquivo de saída da simulação
    FileWriteStream logOutStream(logOutFile, writeBuffer, sizeof(writeBuffer));
    // criar objeto para escrita da saída da simulação
    //Writer<FileWriteStream> logOutWriter(logOutStream);
    PrettyWriter<FileWriteStream> logOutWriter(logOutStream);
    // descarregar o documento Json alterado para o arquivo de saída
    outputDoc->Accept(logOutWriter);
    // fechar o arquivo
    fclose(logOutFile);
  } catch (exception& e) {
    cout << "EXCECAO INESPERADA: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}


void Logger::writeOutputLog(int tipoSaida) {
  // criar objeto do elemento raiz do json de saída da simulação
  Document logOutDoc = parseTemplateLogOutput();
  // definir alocator para para atribuir valores de chaves e valores de propriedades do json de saída
  Document::AllocatorType& allocator = logOutDoc.GetAllocator();

  logOutDoc.AddMember("versao", StringRef(versao.c_str()), allocator);

  try {
    // recuperar o objeto de resultado da simulação do templateJson
    Value& resultadoSimulacao = logOutDoc["resultadoSimulacao"];
    // alterar o nome do arquivo de entrada da simulação
    resultadoSimulacao["arquivo"].SetString(StringRef(getStResultadoSimulacao().nomeArqEntrada));
    // alterar a data da simulação
    resultadoSimulacao["data"].SetString(StringRef(getStResultadoSimulacao().dataExecucao));
    // caso o resultado da simulação seja bem sucedido
    if (getStResultadoSimulacao().sucesso) {
      resultadoSimulacao["status"].SetString("sucesso");
    } else {
      resultadoSimulacao["status"].SetString("falha");
    }
    // recuperar o objeto de excecoesConfiguradas do templateJson
    Value& excCfg = logOutDoc["excecoesConfiguradas"];
    // recuperar o array de logs do templateJson
    Value& logs = resultadoSimulacao["logs"];
    // remove todos os elementos do array de logs - que tenham sido preenchidos no template Json
    if (!logs.Empty()) {
      logs.Clear();
    }
    // array de tipos de log
    string tipoLog[4] = { "NONE", "INFO", "AVISO", "FALHA" };
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
      char* descricaoLog = (char*) malloc(
          sizeof(char)
              * (strlen(excCfg[getStResultadoSimulacao().evento.codigos[indLog].c_str()].GetString())
                  + strlen(getStResultadoSimulacao().evento.descricoes[indLog].c_str()) + 1));
      strcpy(descricaoLog, excCfg[getStResultadoSimulacao().evento.codigos[indLog].c_str()].GetString());
      strcat(descricaoLog, getStResultadoSimulacao().evento.descricoes[indLog].c_str());
      Value valorDescricao(StringRef(descricaoLog));
      // criar propriedade do log - chave e valor
      Value chavePropriedade(StringRef("propriedade"));
      char* propriedadeFalha = (char*) malloc(
          sizeof(char) * strlen(getStResultadoSimulacao().evento.propriedades[indLog].c_str()) + 1);
      strcpy(propriedadeFalha, getStResultadoSimulacao().evento.propriedades[indLog].c_str());
      Value valorPropriedade(StringRef(propriedadeFalha));
      // criar causa do log - chave e valor
      Value chaveCausa(StringRef("causa"));
      char* causaLog = (char*) malloc(
          sizeof(char) * strlen(getStResultadoSimulacao().evento.causas[indLog].c_str()) + 1);
      strcpy(causaLog, getStResultadoSimulacao().evento.causas[indLog].c_str());
      Value valorCausa(StringRef(causaLog));
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
    if(tipoSaida==1)cout << "ARQUIVO DE LOG: " << getStResultadoSimulacao().nomeArqLog << endl;
  } catch (exception& e) {
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
  // encerrar a aplicacao
  exit(EXIT_SUCCESS);
}

//template class Logger;

